//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fldeff_bubble.h
 *	@brief  �[�C�@���A�G�t�F�N�g
 *	@author	tomoya takahashi
 *	@date		2010.02.23
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern void * FLDEFF_BUBBLE_Init( FLDEFF_CTRL* p_fectrl, HEAPID heapID );
extern void FLDEFF_BUBBLE_Delete( FLDEFF_CTRL* p_fectrl, void* p_work );

extern void FLDEFF_BUBBLE_SetMMdl(
    MMDL* p_mmdl, FLDEFF_CTRL* p_fectrl );

#ifdef _cplusplus
}	// extern "C"{
#endif



