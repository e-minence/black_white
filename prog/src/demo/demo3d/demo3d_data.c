//============================================================================
/**
 *
 *	@file		demo3d_data.c
 *	@brief  3D�f���G���W�� �R���o�[�g�f�[�^�ւ̃A�N�Z�T
 *	@author		hosaka genya
 *	@data		2009.11.27
 *
 */
//============================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/ica_anime.h"
#include "arc/arc_def.h"

#include "sound/pm_sndsys.h" // for SEQ_SE_XXX

#include "demo/demo3d.h"

#include "arc/demo3d.naix"

#include "demo3d_data.h"

//--------------------------------------------------------------
///	�f�� �Z�b�g�A�b�v�f�[�^
//==============================================================
typedef struct {
  const GFL_G3D_UTIL_SETUP* setup;
  int max;
  int camera_idx;
  fx32 anime_speed;
  fx32 fovy_sin;
  fx32 fovy_cos;
} DEMO3D_SETUP_DATA;

// �R���o�[�g�f�[�^
#include "demo3d_resdata.cdat"

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
ICA_ANIME* Demo3D_DATA_CreateICACamera( DEMO3D_ID id, HEAPID heapID, int buf_interval )
{
  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );
  return ICA_ANIME_CreateStreamingAlloc( heapID, ARCID_DEMO3D_GRA, c_demo3d_setup_data[id].camera_idx, buf_interval );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^���烆�j�b�g�����擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval�@���j�b�g��
 */
//-----------------------------------------------------------------------------
u8 Demo3D_DATA_GetUnitMax( DEMO3D_ID id )
{
  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );
  return c_demo3d_setup_data[ id ].max;
}

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
const GFL_G3D_UTIL_SETUP* Demo3D_DATA_GetG3DUtilSetup( DEMO3D_ID id, u8 setup_idx )
{
  const GFL_G3D_UTIL_SETUP* setup;

  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );
  GF_ASSERT( setup_idx < Demo3D_DATA_GetUnitMax( id ) );

  setup = &c_demo3d_setup_data[ id ].setup[ setup_idx ];

  return setup;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����A�j���[�V�����X�s�[�h���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval fx32 �A�j���[�V�����X�s�[�h
 */
//-----------------------------------------------------------------------------
const fx32 Demo3D_DATA_GetAnimeSpeed( DEMO3D_ID id )
{
  return c_demo3d_setup_data[ id ].anime_speed;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����p�[�X�lSIN���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval fx32 fovy_sin �p�[�X�lSIN
 */
//-----------------------------------------------------------------------------
const fx32 Demo3D_DATA_GetCameraFovySin( DEMO3D_ID id )
{
  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );

  return c_demo3d_setup_data[ id ].fovy_sin;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����p�[�X�lCOS���擾
 *
 *	@param	DEMO3D_ID id  �f��ID
 *
 *	@retval fx32 fovy_sin �p�[�X�lCOS
 */
//-----------------------------------------------------------------------------
const fx32 Demo3D_DATA_GetCameraFovyCos( DEMO3D_ID id )
{
  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );

  return c_demo3d_setup_data[ id ].fovy_cos;
}


// �R���o�[�g�f�[�^�ǂݍ���
#include "demo3d_cmd_data.cdat"

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����R�}���h�f�[�^���擾
 *
 *	@param	DEMO3D_ID id �f��ID
 *
 *	@retval DEMO3D_CMD_DATA* �R�}���h�f�[�^�z��ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
const DEMO3D_CMD_DATA* Demo3D_DATA_GetCmdData( DEMO3D_ID id )
{
  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );
  return demo3d_cmd_access_tbl[ id ];
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R���o�[�g�f�[�^����I���R�}���h�f�[�^���擾
 *
 *	@param	DEMO3D_ID id �f��ID
 *
 *	@retval DEMO3D_CMD_DATA* �R�}���h�f�[�^�z��ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
const DEMO3D_CMD_DATA* Demo3D_DATA_GetEndCmdData( DEMO3D_ID id )
{
  GF_ASSERT( id < DEMO3D_ID_MAX && id != DEMO3D_ID_NULL );
  return demo3d_endcmd_access_tbl[ id ];
}


