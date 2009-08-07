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
#define FLD_SCENEAREA_SCENEPARAM_SIZE  ( 64 )

// Update�̖߂�l
#define FLD_SCENEAREA_UPDATE_NONE ( 0xffffffff )

//  �֐�ID�@NULL
#define FLD_SCENEAREA_FUNC_NULL	( 0xffff )

// �A�N�e�B�uNONE
#define FLD_SCENEAREA_ACTIVE_NONE (FLD_SCENEAREA_UPDATE_NONE)

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

  // �R�[���o�b�N�֐�ID
	u16 checkArea_func;
	u16 update_func;
	u16 inside_func;
	u16 outside_func;
};

//  �֐��e�[�u��
typedef struct
{
  FLD_SCENEAREA_CHECK_AREA_FUNC*const* cpp_checkArea;   // �͈͓��`�F�b�N
  FLD_SCENEAREA_UPDATA_FUNC*const* cpp_update;					// �X�V�֐�
	u16 checkarea_count;
	u16 update_count;
} FLD_SCENEAREA_FUNC;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern FLD_SCENEAREA* FLD_SCENEAREA_Create( u32 heapID, FIELD_CAMERA * p_camera );
extern void FLD_SCENEAREA_Delete( FLD_SCENEAREA* p_sys );

extern void FLD_SCENEAREA_Load( FLD_SCENEAREA* p_sys, const FLD_SCENEAREA_DATA* cp_data, u32 datanum, const FLD_SCENEAREA_FUNC* cp_func );
extern void FLD_SCENEAREA_Release( FLD_SCENEAREA* p_sys );
extern u32 FLD_SCENEAREA_Update( FLD_SCENEAREA* p_sys, const VecFx32* cp_pos );

extern u32 FLD_SCENEAREA_GetActiveArea( const FLD_SCENEAREA* cp_sys );
extern u32 FLD_SCENEAREA_GetUpdateFuncID( const FLD_SCENEAREA* cp_sys );

extern void FLD_SCENEAREA_SetActiveFlag( FLD_SCENEAREA* p_sys, BOOL flag );
extern BOOL FLD_SCENEAREA_GetActiveFlag( const FLD_SCENEAREA* cp_sys );

// �e����̎󂯓n��
extern FIELD_CAMERA* FLD_SCENEAREA_GetFieldCamera( const FLD_SCENEAREA* cp_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif



