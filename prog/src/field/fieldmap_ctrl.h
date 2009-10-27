//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl.h
 *	@brief  �t�B�[���h�@�}�b�v�R���g���[�����@�A�N�Z�X�֐�
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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern FIELD_PLAYER_GRID* FIELDMAP_GetPlayerGrid( const FIELDMAP_WORK *fieldWork );
extern FIELD_PLAYER_NOGRID* FIELDMAP_GetPlayerNoGrid( const FIELDMAP_WORK *fieldWork );


#ifdef _cplusplus
}	// extern "C"{
#endif



