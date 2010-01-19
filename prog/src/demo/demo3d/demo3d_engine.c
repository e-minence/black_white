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

#include "demo3d_graphic.h"

//�f�[�^
#include "demo3d_data.h"

// 3D�f���R�}���h
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
  u32                     start_frame;
  // [PRIVATE]
  BOOL          is_double;
  VecFx32       def_camera_pos;
  fx32          anime_speed;  ///< �A�j���[�V�����X�s�[�h
  ICA_ANIME*    ica_anime;
  GFL_G3D_UTIL* g3d_util;
  GFL_TCB*      dbl3DdispVintr;
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
 *								�O�����J�֐�
 */
//=============================================================================

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�O���t�B�b�N ������
 *
 *	@param	DEMO3D_GRAPHIC_WORK* graphic  �O���t�B�b�N���[�N
 *	@param	demo_id �f��ID
 *	@param	start_frame �����t���[���l(1SYNC=1)
 *	@param	heapID �q�[�vID
 *
 *	@retval DEMO3D_ENGINE_WORK* ���[�N
 */
//-----------------------------------------------------------------------------
DEMO3D_ENGINE_WORK* Demo3D_ENGINE_Init( DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ID demo_id, u32 start_frame, HEAPID heapID )
{
  DEMO3D_ENGINE_WORK* wk;
  u8 unit_max;

  GF_ASSERT( graphic );

  HOSAKA_Printf("start_frame=%d\n",start_frame);

  // ���C�����[�N �A���P�[�g
  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(DEMO3D_ENGINE_WORK) );

  // �����o������
  wk->graphic       = graphic;
  wk->demo_id       = demo_id;
  wk->start_frame   = start_frame;

  // �R���o�[�^�f�[�^����̏�����
  {
    GFL_G3D_CAMERA* p_camera;
    fx32 fovySin;
    fx32 fovyCos;

    p_camera = DEMO3D_GRAPHIC_GetCamera( wk->graphic );

    fovySin = Demo3D_DATA_GetCameraFovySin( demo_id );
    fovyCos = Demo3D_DATA_GetCameraFovyCos( demo_id );

    GFL_G3D_CAMERA_SetfovySin( p_camera, FX_SinIdx( (fovySin>>FX32_SHIFT) / 2 * PERSPWAY_COEFFICIENT ) );
    GFL_G3D_CAMERA_SetfovyCos( p_camera, FX_CosIdx( (fovyCos>>FX32_SHIFT) / 2 * PERSPWAY_COEFFICIENT ) );
  
    // �N�����̃J����POS��ێ�
    GFL_G3D_CAMERA_GetPos( p_camera, &wk->def_camera_pos );
  }
   
  // �A�j���[�V�����X�s�[�h���擾
  wk->anime_speed = Demo3D_DATA_GetAnimeSpeed( demo_id );
  HOSAKA_Printf("anime_speed=%d\n", wk->anime_speed );
  
  // 2��ʘA���t���O���擾
  wk->is_double =  Demo3D_DATA_GetDoubleFlag( demo_id );
  
  // 2��ʘA���ݒ菉����
  if( wk->is_double )
  {
    GFL_G3D_DOUBLE3D_Init( heapID );
    wk->dbl3DdispVintr = GFUser_VIntr_CreateTCB( GFL_G3D_DOUBLE3D_VblankIntrTCB, NULL, 0 );
  }
  
  wk->cmd = Demo3D_CMD_Init( demo_id, start_frame, heapID );

  unit_max = Demo3D_DATA_GetUnitMax( wk->demo_id );

  // ���j�b�gID ���I�z�� �A���P�[�g
  wk->unit_idx = GFL_HEAP_AllocClearMemory( heapID, sizeof(u16) * unit_max );

  // ica�f�[�^�����[�h
  wk->ica_anime = Demo3D_DATA_CreateICACamera( wk->demo_id, heapID, ICA_BUFF_SIZE );

  // ica�A�j���t���[��������
  ICA_ANIME_SetAnimeFrame( wk->ica_anime, FX32_CONST(start_frame) );

  // 3D�Ǘ����[�e�B���e�B�[�̐���
  wk->g3d_util = GFL_G3D_UTIL_Create( 10, 16, heapID );

  // ���j�b�g�ǉ�
  {
    int i;
    for( i=0; i<unit_max; i++ )
    {
      const GFL_G3D_UTIL_SETUP* setup;

      setup = Demo3D_DATA_GetG3DUtilSetup( wk->demo_id, i );

      HOSAKA_Printf("demoid=%d setup_idx=%d setup objcnt=%d resCount=%d \n",wk->demo_id, i, setup->objCount, setup->resCount);

      wk->unit_idx[i] = GFL_G3D_UTIL_AddUnit( wk->g3d_util, setup );
    }
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
        const int frame = FX32_CONST(wk->start_frame);

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
  if( wk->is_double )
  {
		//�I��
		GFL_TCB_DeleteTask( wk->dbl3DdispVintr );
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
  BOOL is_loop;

  OS_Printf("frame=%f \n", FX_FX32_TO_F32(ICA_ANIME_GetNowFrame( wk->ica_anime )) );

  // �R�}���h���s
  Demo3D_CMD_Main( wk->cmd, ICA_ANIME_GetNowFrame( wk->ica_anime ) );

  p_camera = DEMO3D_GRAPHIC_GetCamera( wk->graphic );

  //@TODO
#if 0
  // �Е��̉�ʂ̕\���ʒu�����炷
  if( wk->is_double )
  {
    VecFx32 pos;

    if( GFL_G3D_DOUBLE3D_GetFlip() )
    {
      pos.x = wk->def_camera_pos.x;
      pos.y = wk->def_camera_pos.y + FX32_CONST(2.0);
      pos.z = wk->def_camera_pos.z;
    }
    else
    {
      // ����
      pos.x = wk->def_camera_pos.x;
      pos.y = wk->def_camera_pos.y;
      pos.z = wk->def_camera_pos.z;
    }

    HOSAKA_Printf("camera pos=%d %d %d \n", pos.x, pos.y, pos.z );
      
    GFL_G3D_CAMERA_SetPos( p_camera, &pos );
  }
#endif 
  
  // ICA�J�����X�V
  is_loop = ICA_ANIME_IncAnimeFrame( wk->ica_anime, wk->anime_speed );

  ICA_CAMERA_SetCameraStatus( p_camera, wk->ica_anime );
  
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
  return is_loop;
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
fx32 DEMO3D_ENGINE_GetNowFrame( DEMO3D_ENGINE_WORK* wk )
{
  GF_ASSERT(wk);
  GF_ASSERT(wk->ica_anime);

  return ICA_ANIME_GetNowFrame( wk->ica_anime );
}



