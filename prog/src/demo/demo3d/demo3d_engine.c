//=============================================================================
/**
 *
 *	@file		demo3d_engine.c
 *	@brief  3D�f���Đ��G���W��
 *	@author	hosaka genya
 *	@data		2009.12.08
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/rtc_tool.h"

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE

// ICA
#include "system/ica_anime.h"
#include "system/ica_camera.h"

//�A�[�J�C�u
#include "arc_def.h"

#include "demo3d_local.h"
#include "demo3d_graphic.h"
#include "demo3d_data.h"
#include "demo3d_cmd.h" 
#include "demo3d_engine.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

#define ICA_BUFF_SIZE (10) ///< ICA�J�����̍Đ��Ŏg�p����o�b�t�@�T�C�Y

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
struct _DEMO3D_ENGINE_WORK {
  // [IN]
  DEMO3D_GRAPHIC_WORK*    graphic;
  DEMO3D_ID               demo_id;
//  u32                     start_frame;
  DEMO3D_SCENE_ENV        env;

  // [PRIVATE]
  BOOL          is_double;
  fx32          anime_speed;  ///< �A�j���[�V�����X�s�[�h
  ICA_ANIME*    ica_anime;
  GFL_G3D_UTIL*   g3d_util;
  GFL_G3D_CAMERA* camera;
  fx32 def_top;
  fx32 def_buttom;
  DEMO3D_CMD_WORK*  cmd;

  u16* unit_idx; // unit_idx�ێ��iALLOC)
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

// DoubleDisp�pVIntr���荞�݊֐�
static void vintrFunc(void)
{
	GFL_G3D_DOUBLE3D_VblankIntr();
}

//-------------------------------------
///	�J�����ʒu
//=====================================
static const VecFx32 sc_CAMERA_PER_POS		= { 0,0,FX32_CONST( 0 ) };	//�ʒu
static const VecFx32 sc_CAMERA_PER_UP			= { 0,FX32_ONE,0 };					//�����
static const VecFx32 sc_CAMERA_PER_TARGET	= { 0,0,FX32_CONST( 0 ) };	//�^�[�Q�b�g

#if 0
//Perspective�J�����̐���
static inline GFL_G3D_CAMERA* GRAPHIC_G3D_CAMERA_Create
		( const VecFx32* cp_pos, const VecFx32* cp_up, const VecFx32* cp_target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJPERS, 
									FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									defaultCameraAspect, 0,
									defaultCameraNear, defaultCameraFar, 0,
									cp_pos, cp_up, cp_target, heapID );
}
#endif

//-----------------------------------------------------------------------------
/**
 *	@brief  Frust�J���� �폜
 *
 *	@param	GFL_G3D_CAMERA* p_camera 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void FrustCamera_Delete( GFL_G3D_CAMERA* p_camera )
{
  GF_ASSERT( p_camera );
  GFL_G3D_CAMERA_Delete( p_camera );
}


//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N ������
 *
 *	@param	DEMO3D_GRAPHIC_WORK* graphic  �O���t�B�b�N���[�N
 *	@param	DEMO3D_PARAM*
 *	@param	heapID �q�[�vID
 *
 *	@retval DEMO3D_ENGINE_WORK* ���[�N
 */
//-----------------------------------------------------------------------------
DEMO3D_ENGINE_WORK* Demo3D_ENGINE_Init( DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_PARAM* param, HEAPID heapID )
{
  DEMO3D_ENGINE_WORK* wk;
  u8 unit_max;

  GF_ASSERT( graphic );

  // ���C�����[�N �A���P�[�g
  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(DEMO3D_ENGINE_WORK) );

  // �����o������
  wk->graphic       = graphic;

  wk->demo_id       = param->demo_id;
  
  //���p�����[�^�Z�b�g
  wk->env.demo_id = wk->demo_id;
  wk->env.scene_id = param->scene_id;
  
  wk->env.start_frame = param->start_frame;
  wk->env.time_zone = GFL_RTC_ConvertHourToTimeZone( param->hour );
  wk->env.player_sex = param->player_sex;

  IWASAWA_Printf("start_frame=%d\n",wk->env.start_frame);

  // �R���o�[�^�f�[�^����J��������
  {
    fx32 fovySin;
    fx32 fovyCos;

    fovySin = Demo3D_DATA_GetCameraFovySin( wk->demo_id );
    fovyCos = Demo3D_DATA_GetCameraFovyCos( wk->demo_id );

    fovySin = FX_SinIdx( (fovySin>>FX32_SHIFT) / 2 * PERSPWAY_COEFFICIENT );
    fovyCos = FX_CosIdx( (fovyCos>>FX32_SHIFT) / 2 * PERSPWAY_COEFFICIENT );

    wk->camera = GFL_G3D_CAMERA_CreateFrustumByPersPrm( 
                  &sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET,
                  fovySin, fovyCos, defaultCameraAspect, FX32_CONST(0.1), FX32_CONST(2048), heapID );

    // �f�t�H���g��top/buttom���擾
    GFL_G3D_CAMERA_GetTop( wk->camera, &wk->def_top );
    GFL_G3D_CAMERA_GetBottom( wk->camera, &wk->def_buttom );

  }
   
  // �A�j���[�V�����X�s�[�h���擾
  wk->anime_speed = Demo3D_DATA_GetAnimeSpeed( wk->demo_id );
  HOSAKA_Printf("anime_speed=%d\n", wk->anime_speed );
  
  // 2��ʘA���t���O���擾
  wk->is_double = Demo3D_DATA_GetDoubleFlag( wk->demo_id );

#ifdef PM_DEBUG
  //@TODO �Z���N�g�������Ȃ���N������Ɠ��ʃt���O���]
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT )
  {
    wk->is_double ^= 1;
  }
#endif // PM_DEBUG
  
  HOSAKA_Printf("is_double=%d\n", wk->is_double );
  
  // 2��ʘA���ݒ菉����
  if( wk->is_double )
  {
    GFL_G3D_DOUBLE3D_Init( heapID );
		GFUser_SetVIntrFunc(vintrFunc);
  }
  
  wk->cmd = Demo3D_CMD_Init( wk->demo_id, wk->env.start_frame, heapID );

  unit_max = Demo3D_DATA_GetUnitMax( wk->demo_id );

  // ���j�b�gID ���I�z�� �A���P�[�g
  wk->unit_idx = GFL_HEAP_AllocClearMemory( heapID, sizeof(u16) * unit_max );

  // ica�f�[�^�����[�h
  wk->ica_anime = Demo3D_DATA_CreateICACamera( wk->demo_id, heapID, ICA_BUFF_SIZE );

  // ica�A�j���t���[��������
  ICA_ANIME_SetAnimeFrame( wk->ica_anime, FX32_CONST(wk->env.start_frame) );

  // 3D�Ǘ����[�e�B���e�B�[�̐���
  wk->g3d_util = GFL_G3D_UTIL_Create( 10, 16, heapID );

  // ���j�b�g�ǉ�
  {
    GFL_G3D_UTIL_SETUP* setup;
    int i;

    setup = Demo3D_DATA_InitG3DUtilSetup(heapID);
    for( i=0; i<unit_max; i++ )
    {

      Demo3D_DATA_GetG3DUtilSetup( setup, &wk->env, i );

      IWASAWA_Printf("demoid=%d setup_idx=%d setup objcnt=%d resCount=%d \n",wk->demo_id, i, setup->objCount, setup->resCount);

      wk->unit_idx[i] = GFL_G3D_UTIL_AddUnit( wk->g3d_util, setup );
    }
    Demo3D_DATA_FreeG3DUtilSetup(setup);
  }
  
  // �A�j���[�V�����L����
  {
    int i;
    for( i=0; i<unit_max; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj;
      int anime_count;
      
      obj         = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->unit_idx[i] );
      anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );

      for( j=0; j<anime_count; j++ )
      {
        const int frame = FX32_CONST(wk->env.start_frame);

        GFL_G3D_OBJECT_EnableAnime( obj, j );
        GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &frame );
      }
    }
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N �J��
 *
 *	@param	DEMO3D_ENGINE_WORK* wk  ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void Demo3D_ENGINE_Exit( DEMO3D_ENGINE_WORK* wk )
{ 
  // �J�����j��
  FrustCamera_Delete( wk->camera );

  if( wk->is_double )
  {
		//�I��
		GFUser_ResetVIntrFunc();
    GFL_G3D_DOUBLE3D_Exit();
  }
  
  Demo3D_CMD_Exit( wk->cmd );

  // ICA�j��
  ICA_ANIME_Delete( wk->ica_anime );

  // ���j�b�g�j��
  {
    int i;
    for( i=0; i<Demo3D_DATA_GetUnitMax( wk->demo_id ); i++ )
    {
      GFL_G3D_UTIL_DelUnit( wk->g3d_util, wk->unit_idx[i] );
    }
  }
  
  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( wk->g3d_util );

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk->unit_idx );
  GFL_HEAP_FreeMemory( wk );
}

//===========================================================================
// debug camera test
//===========================================================================
#ifdef DEBUG_CAMERA_CONTROL

static BOOL debug_vec_move( VecFx32* pos, int num )
{ 
  if( CHECK_KEY_CONT( PAD_KEY_UP ) )
  {
    pos->y += num;
    //OS_Printf("{ 0x%x, 0x%x, 0x%x } \n", pos->x, pos->y, pos->z );
    return TRUE;
  }
  else if( CHECK_KEY_CONT( PAD_KEY_DOWN ) )
  {
    pos->y -= num;
    //OS_Printf("{ 0x%x, 0x%x, 0x%x } \n", pos->x, pos->y, pos->z );
    return TRUE;
  }    
  else if( CHECK_KEY_CONT( PAD_KEY_LEFT ) )
  {
    pos->x += num;
    //OS_Printf("{ 0x%x, 0x%x, 0x%x } \n", pos->x, pos->y, pos->z );
    return TRUE;
  }
  else if( CHECK_KEY_CONT( PAD_KEY_RIGHT ) )
  {
    pos->x -= num;
    //OS_Printf("{ 0x%x, 0x%x, 0x%x } \n", pos->x, pos->y, pos->z );
    return TRUE;
  }
  else if( CHECK_KEY_CONT( PAD_BUTTON_L ) )
  {
    pos->z += num;
    //OS_Printf("{ 0x%x, 0x%x, 0x%x } \n", pos->x, pos->y, pos->z );
    return TRUE;
  }
  else if( CHECK_KEY_CONT( PAD_BUTTON_R ) )
  {
    pos->z -= num;
    //OS_Printf("{ 0x%x, 0x%x, 0x%x } \n", pos->x, pos->y, pos->z );
    return TRUE;
  }

  return FALSE;
}

// �f�[�^�f���o��
static void _debug_vec_print( const VecFx32* pos, const VecFx32* tar )
{
  OS_Printf("======\n");
  OS_Printf("up pos = { 0x%x, 0x%x, 0x%x }; \n",
      pos[TRUE].x,
      pos[TRUE].y,
      pos[TRUE].z );
  
  OS_Printf("up tar = { 0x%x, 0x%x, 0x%x }; \n",
      tar[TRUE].x, 
      tar[TRUE].y,
      tar[TRUE].z );
  
  OS_Printf("down pos = { 0x%x, 0x%x, 0x%x }; \n",
      pos[FALSE].x,
      pos[FALSE].y,
      pos[FALSE].z );
  
  OS_Printf("down tar = { 0x%x, 0x%x, 0x%x }; \n",
      tar[FALSE].x, 
      tar[FALSE].y,
      tar[FALSE].z );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  DEBUG:�J������POS/TARGET���㉺��ʖ��ɑ���/�ݒ�
 *
 *	@param	GFL_G3D_CAMERA* p_camera 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void debug_camera_control( GFL_G3D_CAMERA* p_camera )
{ 
  VecFx32 pos;
  VecFx32 tar;
  
  GFL_G3D_CAMERA_GetPos( p_camera, &pos );
  GFL_G3D_CAMERA_GetTarget( p_camera, &tar );
  {
    static BOOL _is_up = TRUE;

    static VecFx32 _pos[2] = {
      { 0 },
 //     { 0x0,   0xfffff844, 0x28 },
    };
    static VecFx32 _tar[2] = {
      { 0 },
//      { 0x140, 0x8ac,      0x0 },
    };

    static int _mode = 0;
    static int _num = 20;
   
    // �����ʂ̐؂�ւ�
    if( CHECK_KEY_TRG( PAD_BUTTON_Y ) )
    {
      _is_up ^= 1;
      if( _is_up == 0 ){ OS_TPrintf("** set down disp **\n"); } else
      if( _is_up == 1 ){ OS_TPrintf("** set up disp **\n"); }
    }
    // ���썀�ڐ؂�ւ�
    else if( CHECK_KEY_TRG( PAD_BUTTON_X ) )
    {
      _mode = (_mode+1)%2;
      if( _mode == 0 ){ OS_TPrintf("** set pos mode **\n"); } else
      if( _mode == 1 ){ OS_TPrintf("** set target mode **\n"); }
    }
    // ������W���Z�b�g
    else if( CHECK_KEY_TRG( PAD_BUTTON_SELECT ) )
    {
      _pos[_is_up] = (VecFx32){0};
      _tar[_is_up] = (VecFx32){0};
    }
    
    // ���W����
    if( _mode == 0 )
    {
      if( debug_vec_move( &_pos[_is_up], _num ) )
      {
        _debug_vec_print( _pos, _tar );
      }
    }
    else
    {
      if( debug_vec_move( &_tar[_is_up], _num ) )
      {
        _debug_vec_print( _pos, _tar );
      }
    }

    if( GFL_G3D_DOUBLE3D_GetFlip() == FALSE )
    {
      // ������W���f
      pos.x += _pos[TRUE].x;
      pos.y += _pos[TRUE].y;
      pos.z += _pos[TRUE].z;
      tar.x += _tar[TRUE].x;
      tar.y += _tar[TRUE].y;
      tar.z += _tar[TRUE].z;
    }
    else
    {
      // ������W���f
      pos.x += _pos[FALSE].x;
      pos.y += _pos[FALSE].y;
      pos.z += _pos[FALSE].z;
      tar.x += _tar[FALSE].x;
      tar.y += _tar[FALSE].y;
      tar.z += _tar[FALSE].z;
    }
  }
    
  GFL_G3D_CAMERA_SetPos( p_camera, &pos );
  GFL_G3D_CAMERA_SetTarget( p_camera, &tar );

}

#endif // DEBUG_CAMERA_CONTROL

static void set_camera_disp_offset( GFL_G3D_CAMERA* p_camera, const fx32 def_top, const fx32 def_buttom )
{
  static int offset = 610;

  // �f�o�b�O��������
  if( CHECK_KEY_CONT( PAD_KEY_UP ) )
  {
    offset += 10;
    HOSAKA_Printf("offset=%d\n",offset);
  }
  else if( CHECK_KEY_CONT( PAD_KEY_DOWN ) )
  {
    offset -= 10;
    HOSAKA_Printf("offset=%d\n",offset);
  }

  // ����
  if( GFL_G3D_DOUBLE3D_GetFlip() == FALSE )
  {
    fx32 top = def_top + offset;
    fx32 buttom = def_buttom + offset;

    // �N���b�v�ʂ���ɂ��炷
    GFL_G3D_CAMERA_SetTop( p_camera, top );
    GFL_G3D_CAMERA_SetBottom( p_camera, buttom );
  }
  // �����
  else
  {
    // �f�t�H���g�l
    GFL_G3D_CAMERA_SetTop( p_camera, def_top );
    GFL_G3D_CAMERA_SetBottom( p_camera, def_buttom );
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N �又��
 *
 *	@param	DEMO3D_ENGINE_WORK* wk ���[�N
 *
 *	@retval TRUE : �I��
 */
//-----------------------------------------------------------------------------
BOOL Demo3D_ENGINE_Main( DEMO3D_ENGINE_WORK* wk )
{
  GFL_G3D_CAMERA* p_camera;
  BOOL is_end;

//  OS_Printf("frame=%f \n", FX_FX32_TO_F32(ICA_ANIME_GetNowFrame( wk->ica_anime )) );

  // �R�}���h���s
  Demo3D_CMD_Main( wk->cmd, ICA_ANIME_GetNowFrame( wk->ica_anime ) );

  p_camera = wk->camera;
  
  // ICA�J�����X�V
  is_end = ICA_ANIME_IncAnimeFrame( wk->ica_anime, wk->anime_speed );

  // ICA�J�������W��ݒ�
  ICA_CAMERA_SetCameraStatus( p_camera, wk->ica_anime );
  
#ifdef DEBUG_USE_KEY
  // �A�j���Đ��؂�ւ�
  if( CHECK_KEY_TRG( PAD_BUTTON_START ) )
  {
    static BOOL _is_anime = TRUE;
    _is_anime ^= 1;
    if( _is_anime == TRUE ) {
      wk->anime_speed = Demo3D_DATA_GetAnimeSpeed( wk->demo_id );
    } else {
      wk->anime_speed = 0;
    }
  }
#endif

  // �Е��̉�ʂ̕\���ʒu�����炷
  if( wk->is_double )
  {
#ifdef DEBUG_CAMERA_CONTROL
    // �J������POS/TARGET���㉺��ʖ��ɑ���/�ݒ�
    debug_camera_control( p_camera );
#endif
#ifdef DEBUG_CAMERA_DISP_OFFSET_CONTROL
    set_camera_disp_offset( p_camera, wk->def_top, wk->def_buttom );
#endif
  }
  // �A�j���[�V�����X�V
	{
    int i;
    for( i=0; i<Demo3D_DATA_GetUnitMax( wk->demo_id ); i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->unit_idx[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, wk->anime_speed );
      }
    }
  }

  // �J�����؂�ւ�
  GFL_G3D_CAMERA_Switching( p_camera );

  // �`��
	DEMO3D_GRAPHIC_3D_StartDraw( wk->graphic );
	
  {
    int i;
    GFL_G3D_OBJSTATUS status;

    // �X�e�[�^�X������
    VEC_Set( &status.trans, 0, 0, 0 );
    VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
    MTX_Identity33( &status.rotate );

    for( i=0; i<Demo3D_DATA_GetUnitMax( wk->demo_id ); i++ )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( wk->g3d_util, wk->unit_idx[i] );
      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }

	DEMO3D_GRAPHIC_3D_EndDraw( wk->graphic );

  if( wk->is_double )
  {
	  GFL_G3D_DOUBLE3D_SetSwapFlag();
  }

  // ���[�v���o�ŏI��
  return is_end;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���݂̃t���[���l���擾
 *
 *	@param	DEMO3D_ENGINE_WORK* wk �@���[�N
 *
 *	@retval �t�[�����l
 */
//-----------------------------------------------------------------------------
fx32 DEMO3D_ENGINE_GetNowFrame( const DEMO3D_ENGINE_WORK* wk )
{
  GF_ASSERT(wk);
  GF_ASSERT(wk->ica_anime);

  return ICA_ANIME_GetNowFrame( wk->ica_anime );
}



