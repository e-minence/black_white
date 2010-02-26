//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_saveaime.c
 *	@brief  �Z�[�u�A�j��    VBlank���g�p����BG�A�j��
 *	@author	tomoya takahashi
 *	@date		2010.02.26
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc_def.h"
#include "fieldmap/save_icon.naix"


#include "system/gfl_use.h"

#include "field_saveanime.h"
#include "field_camera.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
// VBlankTCB�D�揇��
#define FIELD_SAVEANIME_TCB_PRI (128)


// ������
#define FIELD_SAVEANIME_PAL_NO  ( 9 ) // �g�p�p���b�g
#define FIELD_SAVEANIME_PAL_USE (1)   // �g�p�{��
#define FIELD_SAVEANIME_BG_FRAME  ( GFL_BG_FRAME3_M ) // �g�pBG�t���[��

#define FIELD_SAVEANIME_BG_CHAR_SIZX  ( 4 )
#define FIELD_SAVEANIME_BG_CHAR_SIZY  ( 4 )

#define FIELD_SAVEANIME_ONE_INDEX_CHAR_SIZ  ( FIELD_SAVEANIME_BG_CHAR_SIZX*FIELD_SAVEANIME_BG_CHAR_SIZY*GFL_BG_1CHRDATASIZ )

// �r�b�g�}�b�v�`��ʒu�I�t�Z�b�g
#define FIELD_SAVEANIME_BMP_POS_X_OFS ( -16 )
#define FIELD_SAVEANIME_BMP_POS_Y_OFS ( -24 )
#define FIELD_SAVEANIME_POS_Y_OFS ( 28*FX32_ONE )

// ���ʕʃp���b�g�C���f�b�N�X
static const int sc_PLTT_INDEX[ PM_NEUTRAL ] = 
{
  0, 1,
};


// �t���[�����
typedef struct {
  u16 index;
  u16 draw_frame;
}ANIME_FRAME;
#define FIELD_SAVEANIME_FRAME_MAX (80)
static const ANIME_FRAME sc_FIELD_SAVEANIME_FRAME[] = 
{
  { 0, 20 },
  { 1, 40 },
  { 0, 60 },
  { 2, FIELD_SAVEANIME_FRAME_MAX },
};

#define FIELD_SAVEANIME_TBL_MAX (NELEMS(sc_FIELD_SAVEANIME_FRAME))


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	FIELD_SAVEANIME
//=====================================
struct _FIELD_SAVEANIME{

  FIELDMAP_WORK* p_fieldmap;

  GFL_BMPWIN* p_win;

  GFL_TCB* p_tcb;

  void* p_charbuf;
  NNSG2dCharacterData* p_char;


  int frame;
  int anime_index;
} ;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�摜
//=====================================
static void SAVEANIME_LoadResource( FIELD_SAVEANIME* p_wk, HEAPID heapID );
static void SAVEANIME_ReleaseResource( FIELD_SAVEANIME* p_wk );

//-------------------------------------
///	�r�b�g�}�b�v
//=====================================
static void SAVEANIME_CreateBmpWin( FIELD_SAVEANIME* p_wk );
static void SAVEANIME_DeleteBmpWin( FIELD_SAVEANIME* p_wk );
static void SAVEANIME_OnBmpWin( FIELD_SAVEANIME* p_wk );
static void SAVEANIME_OffBmpWin( FIELD_SAVEANIME* p_wk );
static void SAVEANIME_TransIndex( FIELD_SAVEANIME* p_wk, int index );



//-------------------------------------
///	VBlank�֐�
//=====================================
static void SAVEANIME_VBlank( GFL_TCB* p_tcb, void* p_work );


//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u�A�j���[�V�����̐���
 *
 *	@param	heapID        �q�[�vID
 *	@param	p_fieldmap    �t�B�[���h�}�b�v
 *
 *	@return
 */
//-----------------------------------------------------------------------------
FIELD_SAVEANIME* FIELD_SAVEANIME_Create( HEAPID heapID, FIELDMAP_WORK* p_fieldmap )
{
  FIELD_SAVEANIME* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_SAVEANIME) );

  p_wk->p_fieldmap = p_fieldmap;

  // �摜�ǂݍ���
  SAVEANIME_LoadResource( p_wk, heapID );

  // �r�b�g�}�b�v����
  SAVEANIME_CreateBmpWin( p_wk );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u�A�j���@�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_SAVEANIME_Delete( FIELD_SAVEANIME* p_wk )
{
  // �ꉞ�ǂ�ł���
  FIELD_SAVEANIME_End( p_wk );
  
  // �r�b�g�}�b�v�j��
  SAVEANIME_DeleteBmpWin( p_wk );

  // �摜�j��
  SAVEANIME_ReleaseResource( p_wk );

  // ���[�N�j��
  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����J�n
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_SAVEANIME_Start( FIELD_SAVEANIME* p_wk )
{
  GF_ASSERT( p_wk->p_tcb == NULL );

  GFL_BG_ClearFrame( FIELD_SAVEANIME_BG_FRAME );
  
  p_wk->frame = 0;
  // WinOn
  SAVEANIME_OnBmpWin( p_wk );

  //Vblank�o�^
  p_wk->p_tcb = GFUser_VIntr_CreateTCB( SAVEANIME_VBlank, p_wk, FIELD_SAVEANIME_TCB_PRI );

}


//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���[�V�����I��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void FIELD_SAVEANIME_End( FIELD_SAVEANIME* p_wk )
{
  if(p_wk->p_tcb){
    
    // VBlank�j��
    GFL_TCB_DeleteTask( p_wk->p_tcb );
    p_wk->p_tcb = NULL;

    // WinOff
    SAVEANIME_OffBmpWin( p_wk );
  }
}





//-----------------------------------------------------------------------------
/**
 *      private
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�摜
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�ǂݍ���
 *
 *	@param	p_wk      ���[�N
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_LoadResource( FIELD_SAVEANIME* p_wk, HEAPID heapID )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_FIELD_SAVE_ICON, heapID );
  GAMESYS_WORK * p_gsys = FIELDMAP_GetGameSysWork( p_wk->p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
  const MYSTATUS* cp_mystatus = GAMEDATA_GetMyStatus( p_gdata );
  u32 sex = MyStatus_GetMySex( cp_mystatus );
  
  // �������j�������H
  GF_ASSERT( sex < NELEMS(sc_PLTT_INDEX) );

  // BGR
  p_wk->p_charbuf = GFL_ARCHDL_UTIL_LoadBGCharacter( p_handle, NARC_save_icon_report_icon_NCGR, FALSE,
      &p_wk->p_char, heapID );

  // PLTT�]��
  GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_save_icon_report_icon_NCLR,
      PALTYPE_MAIN_BG, sc_PLTT_INDEX[sex]*32, FIELD_SAVEANIME_PAL_NO*32, 
      FIELD_SAVEANIME_PAL_USE*32, heapID );

  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���\�[�X�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_ReleaseResource( FIELD_SAVEANIME* p_wk )
{
  GFL_HEAP_FreeMemory( p_wk->p_charbuf );
  p_wk->p_charbuf = NULL;
}


//-------------------------------------
///	�r�b�g�}�b�v
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �r�b�g�}�b�v����
 *
 *	@param	p_wk  ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_CreateBmpWin( FIELD_SAVEANIME* p_wk )
{
  FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_wk->p_fieldmap );
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_wk->p_fieldmap );
  const GFL_G3D_CAMERA *cp_g3Dcamera = FIELD_CAMERA_GetCameraPtr( p_camera ); //g3Dcamera Lib �n���h��
  VecFx32 pos;
  int  pos_x, pos_y;
  
  // �r�b�g�}�b�v�`��ʒu�����߂�
  {
    VecFx32 camera_way;
    VecFx32 camera_way_xz;
    VecFx32 camera_side;
    VecFx32 camera_up;
    VecFx32 camera_pos;
    VecFx32 camera_target;
    static const VecFx32 up_way = { 0,FX32_ONE,0 };

	  GFL_G3D_CAMERA_Switching( cp_g3Dcamera );

    GFL_G3D_CAMERA_GetTarget( cp_g3Dcamera, &camera_target );
    GFL_G3D_CAMERA_GetPos( cp_g3Dcamera, &camera_pos );

    VEC_Subtract( &camera_target, &camera_pos, &camera_way );
    // XZ���ʉ������擾
    camera_way_xz = camera_way;
    camera_way_xz.y = 0;
    VEC_Normalize( &camera_way_xz, &camera_way_xz );

    // ���������O�ςŋ��߂�
    VEC_CrossProduct( &camera_way_xz, &up_way, &camera_side );
    // ���ƃJ�����������������߂�
    VEC_CrossProduct( &camera_way, &camera_side, &camera_up );
    VEC_Normalize( &camera_up, &camera_up );

    FIELD_PLAYER_GetPos( p_player, &pos );
    // ������Ɉړ�
    pos.x += FX_Mul( camera_up.x, -FIELD_SAVEANIME_POS_Y_OFS );
    pos.y += FX_Mul( camera_up.y, -FIELD_SAVEANIME_POS_Y_OFS );
    pos.z += FX_Mul( camera_up.z, -FIELD_SAVEANIME_POS_Y_OFS );
    NNS_G3dWorldPosToScrPos( &pos, &pos_x, &pos_y );
    pos_x += FIELD_SAVEANIME_BMP_POS_X_OFS; // X�̓X�N���[�����W�ł��킹��
    pos_y += FIELD_SAVEANIME_BMP_POS_Y_OFS; // X�̓X�N���[�����W�ł��킹��
    
  }

  // �ʒu����
  {

    pos_x /= 8;
    pos_y /= 8;

    if( pos_x < 0 ){
      pos_x = 0;
    }
    if( pos_y < 0 ){
      pos_y = 0;
    }
    if( pos_x > 31 ){
      pos_x = 31;
    }
    if( pos_y > 24 ){
      pos_y = 24;
    }
  }
  OS_TPrintf( "pos_x %d pos_y %d\n", pos_x, pos_y );

  // �r�b�g�}�b�v����
  p_wk->p_win = GFL_BMPWIN_Create(
    FIELD_SAVEANIME_BG_FRAME,
    pos_x, pos_y, 
    FIELD_SAVEANIME_BG_CHAR_SIZX,FIELD_SAVEANIME_BG_CHAR_SIZY,
    FIELD_SAVEANIME_PAL_NO, GFL_BMP_CHRAREA_GET_B );

  // ��ʔ��f
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_win), 15);
  GFL_BMPWIN_TransVramCharacter(p_wk->p_win);

  // 1�C���f�b�N�X��������
  SAVEANIME_TransIndex( p_wk, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r�b�g�}�b�v�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_DeleteBmpWin( FIELD_SAVEANIME* p_wk )
{
  GFL_BMPWIN_Delete( p_wk->p_win );
  p_wk->p_win = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BmpWinOn
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_OnBmpWin( FIELD_SAVEANIME* p_wk )
{
  // �X�N���[����������
  GFL_BMPWIN_MakeScreen(p_wk->p_win);
  GFL_BG_LoadScreenV_Req( FIELD_SAVEANIME_BG_FRAME );
  GFL_BG_SetVisible( FIELD_SAVEANIME_BG_FRAME, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief  BmpWinClear
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_OffBmpWin( FIELD_SAVEANIME* p_wk )
{
  // �X�N���[���N���A
  GFL_BMPWIN_ClearScreen( p_wk->p_win );
  GFL_BG_LoadScreenV_Req( FIELD_SAVEANIME_BG_FRAME );
  GFL_BG_SetVisible( FIELD_SAVEANIME_BG_FRAME, VISIBLE_OFF );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �r�b�g�}�b�v�]��
 *
 *	@param	p_wk    ���[�N
 *	@param	frame   �t���[��
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_TransIndex( FIELD_SAVEANIME* p_wk, int index )
{
  u32 char_ofs;
  u32 res_ofs;
  int i;
  u8* p_data = (u8*)p_wk->p_char->pRawData;

  // �]����I�t�Z�b�g
  char_ofs = GFL_BMPWIN_GetChrNum( p_wk->p_win );

  // ���\�[�X�̃I�t�Z�b�g
  res_ofs = FIELD_SAVEANIME_ONE_INDEX_CHAR_SIZ * index;

  // �E�B���h�E�̃L�����N�^�I�t�Z�b�g�ɓ]���B
  GFL_BG_LoadCharacter( FIELD_SAVEANIME_BG_FRAME, &p_data[ res_ofs ],
      FIELD_SAVEANIME_ONE_INDEX_CHAR_SIZ, char_ofs );
}




//-------------------------------------
///	VBlank�֐�
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  VBlank���C��
 *
 *	@param	p_tcb   TCB
 *	@param	p_work  ���[�N
 */
//-----------------------------------------------------------------------------
static void SAVEANIME_VBlank( GFL_TCB* p_tcb, void* p_work )
{
  FIELD_SAVEANIME* p_wk = p_work;
  int i;
  int nex_index;
  
  // �o���邾�������������̂łׂ�����

  // �t���[���J�E���g
  p_wk->frame ++;

  // ���̃t���[���̌��E�l�𒴂����玟��
  if( sc_FIELD_SAVEANIME_FRAME[ p_wk->anime_index ].draw_frame <= p_wk->frame ){
    p_wk->anime_index ++;

    // ���[�v�`�F�b�N
    if( p_wk->anime_index >= FIELD_SAVEANIME_TBL_MAX ){
      p_wk->anime_index = 0;
      p_wk->frame = 0;
    }

    // �]��
    SAVEANIME_TransIndex( p_wk, sc_FIELD_SAVEANIME_FRAME[ p_wk->anime_index ].index );
  }
}



