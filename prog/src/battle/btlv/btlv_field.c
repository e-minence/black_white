
//============================================================================================
/**
 * @file  btlv_field.c
 * @brief �퓬��ʕ`��p�֐��i�t�B�[���h�j
 * @author  soga
 * @date  2008.11.18
 */
//============================================================================================

#include <gflib.h>

#include "btlv_effect.h"

#include "battle/batt_bg_tbl.h"

#include "arc_def.h"
#include "battle/battgra_wb.naix"
#include "batt_bg_tbl.naix"

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

typedef enum
{ 
  BTLV_FIELD_ANM_TYPE_LOOP = 0,   //�A�j���[�V�����^�C�v�F���[�v
  BTLV_FIELD_ANM_TYPE_REQ,        //�A�j���[�V�����^�C�v�F���N�G�X�g
}BTLV_FIELD_ANM_TYPE;

typedef struct
{ 
  BTLV_FIELD_ANM_TYPE anm_type;
  BOOL                anm_req_flag;
  fx32                anm_speed;
  fx32                anm_distance;
}BTLV_FIELD_ANM_WORK;

struct _BTLV_FIELD_WORK
{
  GFL_G3D_RES*          field_resource;
  int                   anm_count;
  BTLV_FIELD_ANM_WORK*  field_anm_work;
  GFL_G3D_RES**         field_anm_resource;
  GFL_G3D_ANM**         field_anm;
  GFL_G3D_RND*          field_render;
  GFL_G3D_OBJ*          field_obj;
  GFL_G3D_OBJSTATUS     field_status;
  EFFTOOL_PAL_FADE_WORK epfw;
  u32                   vanish_flag :1;
  u32                               :31;
  HEAPID                heapID;
};

typedef struct
{
  BTLV_FIELD_WORK   *bfw;
  EFFTOOL_MOVE_WORK emw;
}BTLV_FIELD_TCB_WORK;

#define BTLV_FIELD_ANM_MAX        ( 0 )         //�w�i�̃A�j���[�V������
                                                //�i�������֐�����P�̎��͂O��v������Ă���̂Ŏ��ۂ͂P�j
#define	BTLV_FIELD_ANM_WAIT       ( FX32_ONE )  //���~�̃A�j���[�V�����E�F�C�g

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

//============================================================================================
/**
 *  �V�X�e��������
 *
 * @param[in] rule    �퓬���[��
 * @param[in] index   �ǂݍ��ރ��\�[�X��INDEX
 * @param[in] season  �G��INDEX
 * @param[in] heapID  �q�[�vID
 */
//============================================================================================
BTLV_FIELD_WORK *BTLV_FIELD_Init( BtlRule rule, int index, u8 season, HEAPID heapID )
{
  BTLV_FIELD_WORK *bfw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_FIELD_WORK ) );
  BOOL  ret;
  BATT_BG_TBL_BG_TABLE* bbtbt = GFL_ARC_LoadDataAlloc( ARCID_BATT_BG_TBL, NARC_batt_bg_tbl_batt_bg_bin, heapID );

  bfw->heapID = heapID;

  //���\�[�X�ǂݍ���
#if 0
  if( rule == BTL_RULE_ROTATION )
  {
    //���[�e�[�V�����o�g���͐�p�w�i
    bfw->field_resource = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, NARC_battgra_wb_batt_fd_vs3_nsbmd );
  }
#endif
//  else if( bbtbt[ index ].file[ BATT_BG_TBL_FILE_NSBMD ][ season ] != BATT_BG_TBL_NO_FILE )
  if( bbtbt[ index ].file[ BATT_BG_TBL_FILE_NSBMD ][ season ] != BATT_BG_TBL_NO_FILE )
  {
    bfw->field_resource = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, bbtbt[ index ].file[ BATT_BG_TBL_FILE_NSBMD ][ season ] );
  }
  else
  {
    bfw->field_resource = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, bbtbt[ index ].file[ BATT_BG_TBL_FILE_NSBMD ][ 0 ] );
  }
  ret = GFL_G3D_TransVramTexture( bfw->field_resource );
  GF_ASSERT( ret == TRUE );

  bfw->anm_count = 0;

  {
    int i;

    for( i = BATT_BG_TBL_FILE_NSBCA ; i < BATT_BG_TBL_FILE_NSBMA + 1 ; i++ )
    {
      if( bbtbt[ index ].file[ i ][ season ] != BATT_BG_TBL_NO_FILE )
      {
        bfw->anm_count++;
      }
    }
  }

  //RENDER����
  bfw->field_render = GFL_G3D_RENDER_Create( bfw->field_resource, 0, bfw->field_resource );

  //���[�e�[�V�����͐�p�A�j��
  if( rule == BTL_RULE_ROTATION )
  { 
    int i, cnt;

    bfw->anm_count = 1;

    bfw->field_anm_resource = GFL_HEAP_AllocMemory( bfw->heapID, 4 * bfw->anm_count );
    bfw->field_anm = GFL_HEAP_AllocMemory( bfw->heapID, 4 * bfw->anm_count );
    bfw->field_anm_work = GFL_HEAP_AllocClearMemory( bfw->heapID, sizeof( BTLV_FIELD_ANM_WORK ) );

    //ANIME����
    bfw->field_anm_resource[ 0 ] = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, NARC_battgra_wb_batt_fd_vs3_nsbta );
    bfw->field_anm[ 0 ] = GFL_G3D_ANIME_Create( bfw->field_render, bfw->field_anm_resource[ 0 ], BTLV_FIELD_ANM_MAX );
    bfw->field_anm_work[ 0 ].anm_type = BTLV_FIELD_ANM_TYPE_LOOP;

    //OBJ����
    bfw->field_obj = GFL_G3D_OBJECT_Create( bfw->field_render, bfw->field_anm, bfw->anm_count );
    //ANIME�N��
    for( i = 0 ; i < bfw->anm_count ; i++ )
    {
      GFL_G3D_OBJECT_EnableAnime( bfw->field_obj, i );
    }

  }
  else if( bfw->anm_count )
  {
    int i, cnt;

    bfw->field_anm_resource = GFL_HEAP_AllocMemory( bfw->heapID, 4 * bfw->anm_count );
    bfw->field_anm = GFL_HEAP_AllocMemory( bfw->heapID, 4 * bfw->anm_count );
    bfw->field_anm_work = GFL_HEAP_AllocClearMemory( bfw->heapID, sizeof( BTLV_FIELD_ANM_WORK ) );

    cnt = 0;

    for( i = BATT_BG_TBL_FILE_NSBCA ; i < BATT_BG_TBL_FILE_NSBMA + 1 ; i++ )
    {
      if( bbtbt[ index ].file[ i ][ season ] != BATT_BG_TBL_NO_FILE )
      {
        //ANIME����
        bfw->field_anm_resource[ cnt ] = GFL_G3D_CreateResourceArc( ARCID_BATTGRA, bbtbt[ index ].file[ i ][ season ] );
        bfw->field_anm[ cnt ] = GFL_G3D_ANIME_Create( bfw->field_render, bfw->field_anm_resource[ cnt ], BTLV_FIELD_ANM_MAX );
        bfw->field_anm_work[ cnt ].anm_type = BTLV_FIELD_ANM_TYPE_LOOP;
        cnt++;
      }
    }
    //OBJ����
    bfw->field_obj = GFL_G3D_OBJECT_Create( bfw->field_render, bfw->field_anm, bfw->anm_count );
    //ANIME�N��
    for( i = 0 ; i < bfw->anm_count ; i++ )
    {
      GFL_G3D_OBJECT_EnableAnime( bfw->field_obj, i );
    }
  }
  else
  {
    bfw->field_obj = GFL_G3D_OBJECT_Create( bfw->field_render, NULL, 0 );
  }

  bfw->field_status.trans.x = 0;
  bfw->field_status.trans.y = 0;
  bfw->field_status.trans.z = 0;
  bfw->field_status.scale.x = FX32_ONE;
  bfw->field_status.scale.y = FX32_ONE;
  bfw->field_status.scale.z = FX32_ONE;
  MTX_Identity33( &bfw->field_status.rotate );

  //�p���b�g�t�F�[�h�p���[�N����
  {
    NNSG3dResFileHeader*  header = GFL_G3D_GetResourceFileHeader( bfw->field_resource );
    NNSG3dResTex*         pTex = NNS_G3dGetTex( header );
    u32                   size = (u32)pTex->plttInfo.sizePltt << 3;

    bfw->epfw.g3DRES     = GFL_HEAP_AllocMemory( bfw->heapID, 4 );
    bfw->epfw.pData_dst  = GFL_HEAP_AllocMemory( bfw->heapID, 4 );

    bfw->epfw.g3DRES[ 0 ]     = bfw->field_resource;
    bfw->epfw.pData_dst[ 0 ]  = GFL_HEAP_AllocMemory( bfw->heapID, size );
    bfw->epfw.pal_fade_flag   = 0;
    bfw->epfw.pal_fade_count  = 1;
  }

  GFL_HEAP_FreeMemory( bbtbt );

  return bfw;
}

//============================================================================================
/**
 *  �V�X�e���I��
 *
 * @param[in] bfw BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_FIELD_Exit( BTLV_FIELD_WORK *bfw )
{
  GFL_G3D_OBJECT_Delete( bfw->field_obj );

  if( bfw->anm_count )
  {
    int i;

    for( i = 0 ; i < bfw->anm_count ; i++ )
    {
      GFL_G3D_ANIME_Delete( bfw->field_anm[ i ] );
      GFL_G3D_DeleteResource( bfw->field_anm_resource[ i ] );
    }
		GFL_HEAP_FreeMemory( bfw->field_anm_work );
    GFL_HEAP_FreeMemory( bfw->field_anm );
    GFL_HEAP_FreeMemory( bfw->field_anm_resource );
  }

  GFL_G3D_RENDER_Delete( bfw->field_render );
  GFL_G3D_DeleteResource( bfw->field_resource );

  GFL_HEAP_FreeMemory( bfw->epfw.pData_dst[ 0 ] );
  GFL_HEAP_FreeMemory( bfw->epfw.pData_dst );
  GFL_HEAP_FreeMemory( bfw->epfw.g3DRES );

  GFL_HEAP_FreeMemory( bfw );
}

//============================================================================================
/**
 *  �V�X�e�����C��
 *
 * @param[in] bfw BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_FIELD_Main( BTLV_FIELD_WORK *bfw )
{
  //�A�j���[�V����
	if(	bfw->anm_count ){
    int i;
    for( i = 0 ; i < bfw->anm_count ; i++ )
    { 
      fx32 speed = 0;
      switch( bfw->field_anm_work[ i ].anm_type ){ 
      case BTLV_FIELD_ANM_TYPE_LOOP:
        speed = BTLV_FIELD_ANM_WAIT; 
        break;
      case BTLV_FIELD_ANM_TYPE_REQ:
        if( bfw->field_anm_work[ i ].anm_req_flag == TRUE )
        { 
          speed = bfw->field_anm_work[ i ].anm_speed;
          if( --bfw->field_anm_work[ i ].anm_distance == 0 )
          { 
            bfw->field_anm_work[ i ].anm_req_flag = FALSE;
          }
        }
        break;
      }
      if( speed )
      { 
        GFL_G3D_OBJECT_LoopAnimeFrame( bfw->field_obj, i, speed ); 
      }
    }
	}
  //�p���b�g�t�F�[�h
  BTLV_EFFTOOL_CalcPaletteFade( &bfw->epfw );
}

//============================================================================================
/**
 *  �`��
 *
 * @param[in] bfw BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_FIELD_Draw( BTLV_FIELD_WORK *bfw )
{
  int i;

  if( bfw->vanish_flag )
  {
    return;
  }

  GFL_G3D_DRAW_DrawObject( bfw->field_obj, &bfw->field_status );
}

//============================================================================================
/**
 *  �p���b�g�t�F�[�h�Z�b�g
 *
 * @param[in] bfw       BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 * @param[in] start_evy �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in] end_evy   �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in] wait      �Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in] rgb       �Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//============================================================================================
void  BTLV_FIELD_SetPaletteFade( BTLV_FIELD_WORK *bfw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{
  GF_ASSERT( bfw );

  bfw->epfw.pal_fade_flag      = 1;
  bfw->epfw.pal_fade_start_evy = start_evy;
  bfw->epfw.pal_fade_end_evy   = end_evy;
  bfw->epfw.pal_fade_wait      = 0;
  bfw->epfw.pal_fade_wait_tmp  = wait;
  bfw->epfw.pal_fade_rgb       = rgb;
}

//============================================================================================
/**
 *  �p���b�g�t�F�[�h���s�����`�F�b�N����
 *
 * @param[in] bfw  BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval  TRUE:���s���@FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_FIELD_CheckExecutePaletteFade( BTLV_FIELD_WORK* bfw )
{
  return ( bfw->epfw.pal_fade_flag != 0 );
}

//============================================================================================
/**
 *  �o�j�b�V���t���O�Z�b�g
 *
 * @param[in] bsw   BTLV_FIELD�Ǘ����[�N�ւ̃|�C���^
 * @param[in] flag  �Z�b�g����t���O( BTLV_FIELD_VANISH_ON BTLV_FIELD_VANISH_OFF )
 */
//============================================================================================
void  BTLV_FIELD_SetVanishFlag( BTLV_FIELD_WORK* bfw, BTLV_FIELD_VANISH flag )
{
  bfw->vanish_flag = flag;
}

