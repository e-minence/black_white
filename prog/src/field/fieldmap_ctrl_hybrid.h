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

// ���̏�ŁA���[���ƃO���b�h��؂�ւ���
extern void FIELDMAP_CTRL_HYBRID_ChangeBaseSystem( FIELDMAP_CTRL_HYBRID* p_wk, FIELDMAP_WORK* p_fieldmap );

#ifdef PM_DEBUG

// �x�[�X�V�X�e����ݒ�̕��ɂȂ肷�܂���B
extern void FIELDMAP_CTRL_HYBRID_DEBUG_SetBaseSystem( FIELDMAP_CTRL_HYBRID* p_wk, FLDMAP_BASESYS_TYPE type );

#endif

#ifdef _cplusplus
}	// extern "C"{
#endif



