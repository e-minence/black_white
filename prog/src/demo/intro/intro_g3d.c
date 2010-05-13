//=============================================================================
/**
 *
 *	@file		intro_g3d.c
 *	@brief
 *	@author	hosaka genya
 *	@data		2009.12.20
 *
 */
//=============================================================================
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "sound/pm_sndsys.h"

//�A�[�J�C�u
#include "arc_def.h"

#include "intro_graphic.h"
#include "intro_g3d.h"

#include "intro.naix"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  UNIT_MAX = 1,   ///< ���j�b�g�ő�l
};

#define	UNIT_NONE		( 0xffff )	///< ���j�b�g�Ȃ�

//RES
static const GFL_G3D_UTIL_RES res_unit_select[] = {
  { ARCID_INTRO_GRA, NARC_intro_intro_bg_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_INTRO_GRA, NARC_intro_intro_bg_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_INTRO_GRA, NARC_intro_intro_bg_nsbma, GFL_G3D_UTIL_RESARC },
};
//ANM
static const GFL_G3D_UTIL_ANM anm_unit_select[] = {
  { 1, 0 },
  { 2, 0 },
};
//OBJ
static const GFL_G3D_UTIL_OBJ obj_unit_select[] = {
  0, 0, 0, 
  anm_unit_select, NELEMS(anm_unit_select),
};
// �Z�b�g�A�b�v
static const GFL_G3D_UTIL_SETUP sc_g3d_setup[ UNIT_MAX ] = 
{ 
  { res_unit_select, NELEMS(res_unit_select), obj_unit_select, NELEMS(obj_unit_select), },
};


#if 0
static const VecFx32 sc_camera_pos    = { 0xfffffdeb, 0x809b, CAMERA_POS_Z };
static const VecFx32 sc_camera_up     = { 0x0, 0x4272, 0x9f0 };
static const VecFx32 sc_camera_target = { 0x0, 0x23e0, 0xffff3802 }; 

static const VecFx32 sc_camera_pos = { 0, FX32_CONST(7), FX32_CONST(32) }; 
static const VecFx32 sc_camera_up =  { 0x0, 0x4272, 0x9f0 }; 
static const VecFx32 sc_camera_target = { 0x0, FX32_CONST(5.1), FX32_CONST(0.5) };
#endif

// �J�����ݒ�
/*
#define CAMERA_POS_Z	( FX32_CONST(32.728027f) )
static const VecFx32 sc_camera_pos = { 0x0, FX32_CONST(7.996582f), CAMERA_POS_Z }; 
static const VecFx32 sc_camera_up =  { 0x0, FX32_CONST(1), 0x0 }; 
static const VecFx32 sc_camera_target = { 0x0, FX32_CONST(4.541260f), FX32_CONST(-0.790039f) }; 
*/
#define CAMERA_POS_Z	( 0 )
/*
static const VecFx32 sc_camera_pos = { 0, 0, FX32_CONST(300) }; 
static const VecFx32 sc_camera_up =  { 0, FX32_ONE, 0 }; 
static const VecFx32 sc_camera_target = { 0, 0, 0 }; 
*/
/*
	{
    static const VecFx32 cam_pos = {FX32_CONST(-41.0f),
                                    FX32_CONST(  0.0f),
                                    FX32_CONST(101.0f)};
    static const VecFx32 cam_target = {FX32_CONST(0.0f),
                                       FX32_CONST(0.0f),
                                       FX32_CONST(-1.0f)};
    static const VecFx32 cam_up = {0,FX32_ONE,0};
*/
#define	ORTHO_HEIGHT	( 3 )
#define	ORTHO_WIDTH		( 4 )


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
struct _INTRO_G3D_WORK {
  // [IN]
  INTRO_GRAPHIC_WORK* graphic;
  HEAPID heap_id;
  // [PRIVATE]
  GFL_G3D_UTIL*   g3d_util;
  GFL_G3D_CAMERA* camera;
  u16           unit_idx[ UNIT_MAX ];
  BOOL          is_draw;
  BOOL          is_load;
  u32	is_mode;
  u32 seq;
  u32 start_seq;
  u32 cnt;
  fx32 anmseq;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�֘A���W���[�� ����
 *
 *	@param	INTRO_GRAPHIC_WORK* graphic 
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
INTRO_G3D_WORK* INTRO_G3D_Create( INTRO_GRAPHIC_WORK* graphic, INTRO_SCENE_ID scene, HEAPID heap_id )
{
  INTRO_G3D_WORK* wk;

  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof( INTRO_G3D_WORK ) );
  
  wk->heap_id = heap_id;
  wk->graphic = graphic;

  // ���[�e�B���e�B�[���Z�b�g�A�b�v
  wk->g3d_util = GFL_G3D_UTIL_Create( 10, 16, heap_id );

  // ��p�J������p��
	{
		VecFx32 pos			= { 0, 0, FX32_ONE };
		VecFx32 up			= { 0, FX32_ONE, 0 };
		VecFx32 target	= { 0, 0, -FX32_ONE };

	  wk->camera = GFL_G3D_CAMERA_Create(
									GFL_G3D_PRJORTH,
									FX32_CONST(ORTHO_HEIGHT),
									-FX32_CONST(ORTHO_HEIGHT),
									-FX32_CONST(ORTHO_WIDTH),
									FX32_CONST(ORTHO_WIDTH),
									FX32_ONE,
									FX32_CONST(1024),
									FX32_ONE,
									&pos, &up, &target, heap_id );

	}
  
	if( scene != INTRO_SCENE_ID_05_RETAKE_YESNO ){
    int i;
    
	  // ���j�b�g�ǉ�
    for( i=0; i<UNIT_MAX; i++ )
    {
      const GFL_G3D_UTIL_SETUP* setup;

      setup = &sc_g3d_setup[i];

      wk->unit_idx[i] = GFL_G3D_UTIL_AddUnit( wk->g3d_util, setup );
    }
  
	  // �A�j���[�V�����L����
    for( i=0; i<UNIT_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj;
      int anime_count;
      
      obj         = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->unit_idx[i] );
      anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );

      for( j=0; j<anime_count; j++ )
      {
        const int frame = FX32_CONST(20); 

        GFL_G3D_OBJECT_EnableAnime( obj, j );
        GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &frame );
      }
    }
  }else{
    int i;
    for( i=0; i<UNIT_MAX; i++ ){
      wk->unit_idx[i] = UNIT_NONE;
    }
	}

  wk->is_load = TRUE;

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�֘A���W���[�� �j��
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_G3D_Exit( INTRO_G3D_WORK* wk )
{
  int i;

  // ��p�J������j��
  GFL_G3D_CAMERA_Delete( wk->camera );

  for( i=0; i<UNIT_MAX; i++ )
  {
		if( wk->unit_idx[i] != UNIT_NONE ){
			GFL_G3D_UTIL_DelUnit( wk->g3d_util, wk->unit_idx[i] );
		}
  }

  GFL_G3D_UTIL_Delete( wk->g3d_util );

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �`�� �又��
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
#include "..\..\ui\debug_code\g3d\camera_test.c"
#include "..\..\ui\debug_code\g3d\g3d_util_anime_frame_test.c"
#endif

void INTRO_G3D_Main( INTRO_G3D_WORK* wk )
{
  int i;
  
#ifdef PM_DEBUG
  // �A�j���[�V��������
//  debug_g3d_util_anime_frame_test( wk->g3d_util, 0 );
 
  // ���W����
//  debug_camera_test( wk->camera );
#endif

  // ��p�J�����ɃX�C�b�`
  GFL_G3D_CAMERA_Switching( wk->camera );

  if( wk->is_draw && wk->is_load )
  {
    GFL_G3D_OBJSTATUS status;

    // �X�e�[�^�X������
    VEC_Set( &status.trans, 0, 0, 0 );
    VEC_Set( &status.scale, FX32_CONST(1), FX32_CONST(1), FX32_CONST(1) );
    MTX_Identity33( &status.rotate );

    // �`��
    for( i=0; i<UNIT_MAX; i++ )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->unit_idx[i] );

      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�I�����X�^�[�g
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL INTRO_G3D_SelectStart( INTRO_G3D_WORK* wk )
{
  fx32 val;
  VecFx32 pos;

  switch( wk->start_seq )
  {
  case 0:
		// �f�t�H���g�ʒu�̃t���[��
		INTRO_G3D_SelectSet( wk, INTRO_3D_SEL_SEX_DEF_FRAME );
    // �J�����𗣂�
/*
    GFL_G3D_CAMERA_GetPos( wk->camera, &pos );
    pos.z = FX32_ONE;
    GFL_G3D_CAMERA_SetPos( wk->camera, &pos );
*/
    // �u�����h���[�h�A�Ώۖʎw��
    G2_SetBlendAlpha( GX_PLANEMASK_BG0, GX_PLANEMASK_BG3|GX_PLANEMASK_BG0, 0, 0 );
		PMSND_PlaySE( SEQ_SE_OPEN2 );
    wk->start_seq++;
    break;

  case 1:
/*
    // �g��
    GFL_G3D_CAMERA_GetPos( wk->camera, &pos );

    val = MATH_IAbs( pos.z - CAMERA_POS_Z ) / 4;

    // �덷�C��
    if( val < 0x100 ){ val = 0x100; }

    pos.z -= val;

    if( pos.z < CAMERA_POS_Z )
    {
      pos.z = CAMERA_POS_Z;
      GFL_G3D_CAMERA_SetPos( wk->camera, &pos );

      return TRUE;
    }
      
    GFL_G3D_CAMERA_SetPos( wk->camera, &pos );
    break;
*/
		return TRUE;

  default : GF_ASSERT(0);
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �\����\���؂�ւ�
 *
 *	@param	INTRO_G3D_WORK* wk
 *	@param	is_visible 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_G3D_SelectVisible( INTRO_G3D_WORK* wk, BOOL is_visible )
{
  wk->is_draw = is_visible;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���艉�o�J�n
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_G3D_SelectDecideStart( INTRO_G3D_WORK* wk, u32 is_mode )
{
  wk->is_mode = is_mode;
  wk->seq = 0;

	switch( wk->is_mode ){
	case INTRO_3D_SEL_SEX_MODE_MOVE_DEFAULT:
		wk->cnt = INTRO_3D_SEL_SEX_DEF_FRAME;
		wk->is_mode = INTRO_3D_SEL_SEX_MODE_MOVE_MALE;
		break;

	case INTRO_3D_SEL_SEX_MODE_MOVE_MALE:
		wk->cnt = INTRO_3D_SEL_SEX_MOVE_FEMALE_FRAME;
		break;

	case INTRO_3D_SEL_SEX_MODE_MOVE_FEMALE:
		wk->cnt = INTRO_3D_SEL_SEX_MOVE_MALE_FRAME;
		break;

	case INTRO_3D_SEL_SEX_MODE_ENTER_MALE:
		wk->cnt = INTRO_3D_SEL_SEX_MOVE_MALE_FRAME;
		break;

	case INTRO_3D_SEL_SEX_MODE_ENTER_FEMALE:
		wk->cnt = INTRO_3D_SEL_SEX_MOVE_FEMALE_FRAME;
		break;
	}
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���艉�o �又��
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL INTRO_G3D_SelectDecideWait( INTRO_G3D_WORK* wk )
{
	switch( wk->is_mode ){
	case INTRO_3D_SEL_SEX_MODE_MOVE_MALE:
    wk->cnt--;
		INTRO_G3D_SelectSet( wk, wk->cnt );
    if( wk->cnt == INTRO_3D_SEL_SEX_MOVE_MALE_FRAME ){ return TRUE; }
		break;

	case INTRO_3D_SEL_SEX_MODE_MOVE_FEMALE:
    wk->cnt++;
		INTRO_G3D_SelectSet( wk, wk->cnt );
    if( wk->cnt == INTRO_3D_SEL_SEX_MOVE_FEMALE_FRAME ){ return TRUE; }
		break;

	case INTRO_3D_SEL_SEX_MODE_ENTER_MALE:
    wk->cnt--;
		INTRO_G3D_SelectSet( wk, wk->cnt );
    if( wk->cnt == INTRO_3D_SEL_SEX_ENTER_MALE_FRAME ){ return TRUE; }
		break;

	case INTRO_3D_SEL_SEX_MODE_ENTER_FEMALE:
    wk->cnt++;
		INTRO_G3D_SelectSet( wk, wk->cnt );
    if( wk->cnt == INTRO_3D_SEL_SEX_ENTER_FEMALE_FRAME ){ return TRUE; }
		break;
	}
/*
  if( wk->is_mode )
  {
    wk->cnt--;
    INTRO_G3D_SelectSet( wk, wk->cnt );
    if( wk->cnt <= 0 ){ return TRUE; }
  }
  else
  {
    wk->cnt++;
    INTRO_G3D_SelectSet( wk, wk->cnt );
    if( wk->cnt >= 120 ){ return TRUE; }
  }
*/

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�j���I���A���^�[��
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL INTRO_G3D_SelectDecideReturn( INTRO_G3D_WORK* wk )
{
  if( wk->is_mode == INTRO_3D_SEL_SEX_MODE_ENTER_MALE )
  {
    wk->cnt++;
    INTRO_G3D_SelectSet( wk, wk->cnt );
  }
  else
  {
    wk->cnt--;
    INTRO_G3D_SelectSet( wk, wk->cnt );
  }

  if( wk->cnt == INTRO_3D_SEL_SEX_DEF_FRAME ){ return TRUE; }

  return FALSE;

}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�j���I���A�t���[���w��
 *
 *	@param	INTRO_G3D_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_G3D_SelectSet( INTRO_G3D_WORK* wk, u32 in_frame )
{
  int i;

  // �A�j���t���[���w��
  for( i=0; i<UNIT_MAX; i++ )
  {
    int j;
    GFL_G3D_OBJ* obj;
    int anime_count;
    
    obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->unit_idx[i] );
    anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );

    for( j=0; j<anime_count; j++ )
    {
      const int frame = FX32_CONST(in_frame);

      GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &frame );
    }
  }
}


//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


