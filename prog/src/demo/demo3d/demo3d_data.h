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

#include "demo/demo3d.h" // for DEMO3D_ID
#include "demo3d_cmd_def.h" // for DEMO3D_CMD_DATA

#include "system/ica_anime.h" // for ICA_ANIME

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
 *	@brief  �R���o�[�g�f�[�^����Z�b�g�A�b�v�f�[�^���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *	@param	setup_idx     �Z�b�g�A�b�v�f�[�^�̃C���f�b�N�X
 *
 *	@retval GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern const GFL_G3D_UTIL_SETUP* Demo3D_DATA_GetG3DUtilSetup( DEMO3D_ID id, u8 setup_idx );

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

