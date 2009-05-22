//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea.h
 *	@brief  �t�B�[���h�@����V�[���̈�Ǘ�
 *	@author	tomoya takahashi
 *	@data		2009.05.22
 *
 *	���W���[�����FFLD_SCENEAREA
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


#include "gflib.h"
#include "field/fieldmap_proc.h"
#include "field_camera.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
// �V�[���Ɏ�������p�����[�^�T�C�Y�̍ő�l
#define FLD_SCENEAREA_SCENEPARAM_SIZE  ( 32 )

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�[���G���A
//=====================================
typedef struct _FLD_SCENEAREA FLD_SCENEAREA;



//-------------------------------------
///	�V�[���G���A���
//=====================================
typedef struct _FLD_SCENEAREA_DATA FLD_SCENEAREA_DATA;

// �R�[���o�b�N�֐��̌^
// �G���A�͈̓`�F�b�N   �߂�l�FTRUE  �G���A��  FALSE�@�G���A�O
typedef BOOL (FLD_SCENEAREA_CHECK_AREA_FUNC)( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
typedef void (FLD_SCENEAREA_UPDATA_FUNC)( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );

// �\���̒�`
struct _FLD_SCENEAREA_DATA{

  // �V�[���p�����[�^
  u32 area[FLD_SCENEAREA_SCENEPARAM_SIZE/4];

  // �R�[���o�b�N�֐�
  FLD_SCENEAREA_CHECK_AREA_FUNC* p_checkArea;   // �͈͓��`�F�b�N
  FLD_SCENEAREA_UPDATA_FUNC* p_updata;      // ���̍X�V�i�G���A���̏ꍇ�}�C�t���[���s�������j
  FLD_SCENEAREA_UPDATA_FUNC* p_inside;      // �͈͂ɓ������u�Ԃ̏���
  FLD_SCENEAREA_UPDATA_FUNC* p_outside;     // �͈͂𔲂����u�Ԃ̏���
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern FLD_SCENEAREA* FLD_SCENEAREA_Create( u32 heapID, FIELD_CAMERA * p_camera );
extern void FLD_SCENEAREA_Delete( FLD_SCENEAREA* p_sys );

extern void FLD_SCENEAREA_Load( FLD_SCENEAREA* p_sys, const FLD_SCENEAREA_DATA* cp_data, u32 datanum );
extern void FLD_SCENEAREA_Release( FLD_SCENEAREA* p_sys );
extern void FLD_SCENEAREA_Updata( FLD_SCENEAREA* p_sys, const VecFx32* cp_pos );

// �e����̎󂯓n��
extern FIELD_CAMERA* FLD_SCENEAREA_GetFieldCamera( const FLD_SCENEAREA* cp_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif



