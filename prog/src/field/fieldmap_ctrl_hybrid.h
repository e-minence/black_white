//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl_hybrid.h
 *	@brief  �}�b�v�R���g���[���@�n�C�u���b�h
 *	@author	tomoya takahashi
 *	@date		2009.10.27
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

#include "field_player_grid.h"
#include "field_player_nogrid.h"

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
///	FIELDMAP_CTRL_HYBRID���[�N
//=====================================
typedef struct _FIELDMAP_CTRL_HYBRID FIELDMAP_CTRL_HYBRID;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern const DEPEND_FUNCTIONS FieldMapCtrl_HybridFunctions;



// ���̎擾
extern FLDMAP_BASESYS_TYPE FIELDMAP_CTRL_HYBRID_GetBaseSystemType( const FIELDMAP_CTRL_HYBRID* cp_wk );
extern FIELD_PLAYER_GRID* FIELDMAP_CTRL_HYBRID_GetFieldPlayerGrid( const FIELDMAP_CTRL_HYBRID* cp_wk );
extern FIELD_PLAYER_NOGRID* FIELDMAP_CTRL_HYBRID_GetFieldPlayerNoGrid( const FIELDMAP_CTRL_HYBRID* cp_wk );

// ���̏�ŁA���[���ƃO���b�h��؂�ւ���
extern void FIELDMAP_CTRL_HYBRID_ChangeBaseSystem( FIELDMAP_CTRL_HYBRID* p_wk, FIELDMAP_WORK* p_fieldmap );

#ifdef _cplusplus
}	// extern "C"{
#endif



