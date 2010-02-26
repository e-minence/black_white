//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_saveaime.h
 *	@brief  �Z�[�u�A�j��    VBlank���g�p����BG�A�j��
 *	@author	tomoya takahashi
 *	@date		2010.02.26
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

#include "fieldmap.h"

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
///	FIELD_SAVEANIME
//=====================================
typedef struct _FIELD_SAVEANIME FIELD_SAVEANIME;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	FIELD_SAVEANIME
//=====================================
//  �A�j���[�V���������E�j��
extern FIELD_SAVEANIME* FIELD_SAVEANIME_Create( HEAPID heapID, FIELDMAP_WORK* p_fieldmap );
extern void FIELD_SAVEANIME_Delete( FIELD_SAVEANIME* p_wk );

// �A�j���[�V�����J�n
extern void FIELD_SAVEANIME_Start( FIELD_SAVEANIME* p_wk );

// �A�j���[�V������~
extern void FIELD_SAVEANIME_End( FIELD_SAVEANIME* p_wk );




#ifdef _cplusplus
}	// extern "C"{
#endif



