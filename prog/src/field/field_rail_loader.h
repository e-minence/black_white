//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_rail_loader.h
 *	@brief	�t�B�[���h���[�����[�_�[
 *	@author	tomoya takahashi
 *	@date		2009.07.07
 *
 *	���W���[�����FFIELD_RAIL_LOADER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_rail.h"

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
///	���[�N
//=====================================
typedef struct _FIELD_RAIL_LOADER FIELD_RAIL_LOADER;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �V�X�e���쐬�E�j��
extern FIELD_RAIL_LOADER* FIELD_RAIL_LOADER_Create( u32 heapID );
extern void FIELD_RAIL_LOADER_Delete( FIELD_RAIL_LOADER* p_sys );

// ���[�����ǂݍ��݁E�N���A
extern void FIELD_RAIL_LOADER_Load( FIELD_RAIL_LOADER* p_sys, u32 datano, u32 heapID );
extern void FIELD_RAIL_LOADER_Clear( FIELD_RAIL_LOADER* p_sys );

// ���[���Z�b�g�A�b�v���擾
extern const RAIL_SETTING* FIELD_RAIL_LOADER_GetData( const FIELD_RAIL_LOADER* cp_sys );


// �f�o�b�N�@�\
#ifdef PM_DEBUG
extern void FIELD_RAIL_LOADER_DEBUG_LoadBinary( FIELD_RAIL_LOADER* p_sys, void* p_dat, u32 size );
extern void* FIELD_RAIL_LOADER_DEBUG_GetData( const FIELD_RAIL_LOADER* cp_sys );
extern u32 FIELD_RAIL_LOADER_DEBUG_GetDataSize( const FIELD_RAIL_LOADER* cp_sys );
#endif

#ifdef _cplusplus
}	// extern "C"{
#endif



