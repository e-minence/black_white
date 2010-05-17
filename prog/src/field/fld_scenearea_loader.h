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
//-------------------------------------
///	LOAD_TYPE
//=====================================
typedef enum {
  FLD_SCENEAREA_LOAD_RAIL,
  FLD_SCENEAREA_LOAD_GRID,

  FLD_SCENEAREA_LOADTYPE_MAX,
} FLD_SCENEAREA_LOADTYPE;

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
extern FLD_SCENEAREA_LOADER* FLD_SCENEAREA_LOADER_Create( HEAPID heapID );
extern void FLD_SCENEAREA_LOADER_Delete( FLD_SCENEAREA_LOADER* p_sys );


// �ǂݍ��݁A�N���A
extern void FLD_SCENEAREA_LOADER_Load( FLD_SCENEAREA_LOADER* p_sys, FLD_SCENEAREA_LOADTYPE loadtype, u32 datano, HEAPID heapID );
extern void FLD_SCENEAREA_LOADER_LoadOriginal( FLD_SCENEAREA_LOADER* p_sys, u32 arcID, u32 datano, HEAPID heapID );
extern void FLD_SCENEAREA_LOADER_Clear( FLD_SCENEAREA_LOADER* p_sys );

// ���̎擾
extern const FLD_SCENEAREA_DATA* FLD_SCENEAREA_LOADER_GetData( const FLD_SCENEAREA_LOADER* cp_sys );
extern u32 FLD_SCENEAREA_LOADER_GetDataNum( const FLD_SCENEAREA_LOADER* cp_sys );
extern const FLD_SCENEAREA_FUNC* FLD_SCENEAREA_LOADER_GetFunc( const FLD_SCENEAREA_LOADER* cp_sys );

// �w��G���A�̏����擾

// �f�o�b�N�@�\
#ifdef PM_DEBUG
extern void FLD_SCENEAREA_LOADER_LoadBinary( FLD_SCENEAREA_LOADER* p_sys, void* p_dat, u32 size );
extern void* FLD_SCENEAREA_LOADER_DEBUG_GetData( const FLD_SCENEAREA_LOADER* cp_sys );
extern u32 FLD_SCENEAREA_LOADER_DEBUG_GetDataSize( const FLD_SCENEAREA_LOADER* cp_sys );
#endif

#ifdef _cplusplus
}	// extern "C"{
#endif



