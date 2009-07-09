//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea_loader.h
 *	@brief  �t�B�[���h�@����V�[���̈�O���f�[�^�ǂݍ���
 *	@author	tomoya takahashi
 *	@date		2009.07.09
 *
 *	���W���[�����FFLD_SCENEAREA_LOADER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "fld_scenearea.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�ǂݍ��݃V�X�e��
//=====================================
typedef struct _FLD_SCENEAREA_LOADER FLD_SCENEAREA_LOADER;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//�����@�j��
extern FLD_SCENEAREA_LOADER* FLD_SCENEAREA_LOADER_Create( u32 heapID );
extern void FLD_SCENEAREA_LOADER_Delete( FLD_SCENEAREA_LOADER* p_sys );


// �ǂݍ��݁A�N���A
extern void FLD_SCENEAREA_LOADER_Load( FLD_SCENEAREA_LOADER* p_sys, u32 datano, u32 heapID );
extern void FLD_SCENEAREA_LOADER_Clear( FLD_SCENEAREA_LOADER* p_sys );

// ���̎擾
extern const FLD_SCENEAREA_DATA* FLD_SCENEAREA_LOADER_GetData( const FLD_SCENEAREA_LOADER* cp_sys );
extern u32 FLD_SCENEAREA_LOADER_GetDataNum( const FLD_SCENEAREA_LOADER* cp_sys );
extern const FLD_SCENEAREA_FUNC* FLD_SCENEAREA_LOADER_GetFunc( const FLD_SCENEAREA_LOADER* cp_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif



