//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_attr.h
 *	@brief  ���[���@�A�g���r���[�g  
 *	@author	tomoya takahashi
 *	@date		2009.08.21
 *
 *	���W���[�����FRAIL_ATTR
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "map_attr.h"

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
///	RAIL_ATTR_DATA
//=====================================
typedef struct _RAIL_ATTR_DATA RAIL_ATTR_DATA;    


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// ���[���A�g���r���[�g�f�[�^�̐����j��
extern RAIL_ATTR_DATA* RAIL_ATTR_Create( u32 heapID );
extern void RAIL_ATTR_Delete( RAIL_ATTR_DATA* p_work );

// �f�[�^�ǂݍ��݁A�����[�X
extern void RAIL_ATTR_Load( RAIL_ATTR_DATA* p_work, u32 datano, u32 heapID );
extern void RAIL_ATTR_Release( RAIL_ATTR_DATA* p_work );
extern BOOL RAIL_ATTR_IsLoadData( const RAIL_ATTR_DATA* cp_work );


// ���[���̃A�g���r���[�g�擾
extern MAPATTR RAIL_ATTR_GetAttribute( const RAIL_ATTR_DATA* cp_work, const RAIL_LOCATION* cp_location );


// ���[���}�b�v�p�A�g���r���[�gVALUE�`�F�b�N
extern BOOL RAIL_ATTR_VALUE_CheckSlipDown( const MAPATTR_VALUE val );


#ifdef PM_DEBUG
extern void RAIL_ATTR_DEBUG_LoadBynary( RAIL_ATTR_DATA* p_work, void* p_data, u32 datasize, u32 heapID );
extern const void* RAIL_ATTR_DEBUG_GetData( const RAIL_ATTR_DATA* cp_work );
extern u32 RAIL_ATTR_DEBUG_GetDataSize( const RAIL_ATTR_DATA* cp_work );
#endif

#ifdef _cplusplus
}	// extern "C"{
#endif



