//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_light.h
 *	@brief		�t�B�[���h���C�g�V�X�e��
 *	@author		tomoya takahashi
 *	@data		2009.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_LIGHT_H__
#define __FIELD_LIGHT_H__

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "field_fog.h"

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
///	�t�B�[���h���C�g�V�X�e��
//=====================================
typedef struct _FIELD_LIGHT FIELD_LIGHT;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�V�X�e���쐬�E�j��
//=====================================
extern FIELD_LIGHT* FIELD_LIGHT_Create( u32 light_no, u32 season, int rtc_second, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight, u32 heapID );
extern void FIELD_LIGHT_Delete( FIELD_LIGHT* p_sys );

//-------------------------------------
///	�V�X�e�����C��
//=====================================
extern void FIELD_LIGHT_Main( FIELD_LIGHT* p_sys, int rtc_second );


//-------------------------------------
///	���C�g���̕ύX
//=====================================
extern void FIELD_LIGHT_Change( FIELD_LIGHT* p_sys, u32 light_no, u32 season, u32 heapID );
extern void FIELD_LIGHT_ChangeEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, u32 heapID );

// FIELD_LIGHT_Create or FIELD_LIGHT_Change�Őݒ肵��light_no season�̃��C�g���ɖ߂�
extern void FIELD_LIGHT_ReLoadDefault( FIELD_LIGHT* p_sys, u32 heapID );

//-------------------------------------
///	���C�g��񔽉f��ON�EOFF
//=====================================
extern void FIELD_LIGHT_SetReflect( FIELD_LIGHT* p_sys, BOOL flag );
extern BOOL FIELD_LIGHT_GetReflect( const FIELD_LIGHT* cp_sys );


//-------------------------------------
///	��Ԃ̎擾
//=====================================
extern BOOL FIELD_LIGHT_GetNight( const FIELD_LIGHT* cp_sys );





#ifdef _cplusplus
}	// extern "C"{
#endif

#endif		// __FIELD_LIGHT_H__


