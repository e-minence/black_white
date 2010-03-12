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

///�V�[���p�����[�^
typedef struct _DEMO3D_SCENE_DATA{
  u16   camera_bin_id;
  u16   zone_id;
  u16   bgm_no;
  u8    frame_rate; //<�t���[�����[�g 0:60fps, 1:30fps
  u8    double_view_f:1;
  u8    fog_f:1;
  u8    edge_marking_f:1;
//  fx32  anm_speed;  //<60fps = 1.0, 30fps = 0.5
  fx32  fovy_sin,fovy_cos;
  fx32  near,far;

  GXRgb clear_col;
  u8    clear_alpha;

  GXRgb fog_col;
  u8    fog_alpha;
  u8    fog_shift;
  u16   fog_offset;
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

