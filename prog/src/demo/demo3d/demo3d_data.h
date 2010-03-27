//============================================================================
/**
 *
 *	@file		demo3d_data.h
 *	@brief  �R���o�[�g�f�[�^�̃A�N�Z�T
 *	@author	hosaka genya
 *	@data		2009.11.27
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "arc/fieldmap/zone_id.h"

#include "demo/demo3d.h" // for DEMO3D_ID
#include "demo3d_local.h"
#include "demo3d_cmd_def.h" // for DEMO3D_CMD_DATA

#include "system/ica_anime.h" // for ICA_ANIME


#define DEMO3D_BGM_NONE         (0xFFFF)

typedef enum{
  DEMO3D_FRAME_RATE_60FPS,
  DEMO3D_FRAME_RATE_30FPS,
}DEMO3D_FRAME_RATE;

typedef enum{
  DEMO3D_FADE_BLACK,
  DEMO3D_FADE_WHITE,
}DEMO3D_FADE_TYPE;

///�V�[���p�����[�^
typedef struct _DEMO3D_SCENE_DATA{
  u16   camera_bin_id;
  u16   zone_id;
  u16   bgm_no;
 
  u16   frame_rate:1; //<�t���[�����[�g 0:60fps, 1:30fps
  u16   double_view_f:1;
  u16   alpha_blend_f:1;
  u16   anti_alias_f:1;
  u16   fog_f:1;
  u16   edge_marking_f:1;
  u16   sbuf_sort_mode:1;
  u16   sbuf_buf_mode:1;
  u16   alpha_test_f:1;
  u16   alpha_test_val:5;
  
  fx32  fovy_sin, fovy_cos, aspect;
  fx32  near, far;
  fx32  scale_w;

  s16   dview_main_ofs, dview_sub_ofs;
  fx32  poly_1dot_depth;

  GXRgb clear_col;
  u16   clear_alpha;
  u16   clear_poly_id:15;
  u16   clear_fog_f:1;
  u16   clear_depth;

  GXRgb fog_col;
  u8    fog_alpha;
  u8    fog_shift;
  u16   fog_offset:15;
  u16   fog_mode:1;
  u8    fog_tbl[8];
  
  GXRgb edge_col[8];

  u8    fadein_type;
  u8    fadein_sync;
  u8    fadeout_type;
  u8    fadeout_sync;
}DEMO3D_SCENE_DATA;

/*
 *  @brief  �V�[���f�[�^�擾
 */
extern const DEMO3D_SCENE_DATA* Demo3D_DATA_GetSceneData( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^���烆�j�b�g�����擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval�@���j�b�g��
 */
//-----------------------------------------------------------------------------
extern u8 Demo3D_DATA_GetUnitMax( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  GFL_G3D_UTIL_SETUP�̓��I������
 *
 *	@retval GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern GFL_G3D_UTIL_SETUP* Demo3D_DATA_InitG3DUtilSetup( HEAPID heapID );

//-----------------------------------------------------------------------------
/**
 *	@brief  GFL_G3D_UTIL_SETUP�̉��
 *
 *	@param GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern void Demo3D_DATA_FreeG3DUtilSetup( GFL_G3D_UTIL_SETUP* sp );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����Z�b�g�A�b�v�f�[�^���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *	@param	setup_idx     �Z�b�g�A�b�v�f�[�^�̃C���f�b�N�X
 *
 *	@retval GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern void Demo3D_DATA_GetG3DUtilSetup( GFL_G3D_UTIL_SETUP* sp, DEMO3D_SCENE_ENV* env, u8 idx );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����ICA�J�����𐶐�
 *
 *	@param	DEMO3D_ID id  �f��ID
 *	@param	heapID  �q�[�vID
 *	@param	buf_interval �o�b�t�@�����O���銴�o
 *
 *	@retval ICA_ANIME*
 */
//-----------------------------------------------------------------------------
extern ICA_ANIME* Demo3D_DATA_CreateICACamera( DEMO3D_ID id, HEAPID heapID, int buf_interval );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����R�}���h�f�[�^���擾
 *
 *	@param	DEMO3D_ID id �f��ID
 *
 *	@retval DEMO3D_CMD_DATA* �R�}���h�f�[�^�z��ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
extern const DEMO3D_CMD_DATA* Demo3D_DATA_GetCmdData( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����I���R�}���h�f�[�^���擾
 *
 *	@param	DEMO3D_ID id �f��ID
 *
 *	@retval DEMO3D_CMD_DATA* �R�}���h�f�[�^�z��ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
extern const DEMO3D_CMD_DATA* Demo3D_DATA_GetEndCmdData( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����A�j���[�V�����X�s�[�h���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval fx32 �A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
extern const fx32 Demo3D_DATA_GetAnimeSpeed( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����2��ʘA���\���t���O���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval BOOL 2��ʃt���O
 */
//-----------------------------------------------------------------------------
extern const BOOL Demo3D_DATA_GetDoubleFlag( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����p�[�X�lSIN���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval fx32 fovy_sin �p�[�X�lSIN
 */
//-----------------------------------------------------------------------------
extern const fx32 Demo3D_DATA_GetCameraFovySin( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����p�[�X�lCOS���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval fx32 fovy_sin �p�[�X�lCOS
 */
//-----------------------------------------------------------------------------
extern const fx32 Demo3D_DATA_GetCameraFovyCos( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����t�F�[�h�C���E�A�E�g�p�����[�^���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 */
//-----------------------------------------------------------------------------
extern void Demo3D_DATA_GetFadeParam( DEMO3D_ID id, u8 inout_f, u8* outType, u8* outWait );

