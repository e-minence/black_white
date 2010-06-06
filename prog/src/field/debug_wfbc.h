//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		debug_wfbc.h
 *	@brief  WFBC�I�[�g�f�o�b�N�@�\
 *	@author	tomoya takahashi
 *	@date		2010.05.24
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
#include "system/vm_cmd.h"

#include "fieldmap.h"



FS_EXTERN_OVERLAY(debug_menu_wfbc_check);


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

extern GMEVENT * DEBUG_WFBC_View( GAMESYS_WORK *gsys );

extern GMEVENT * DEBUG_WFBC_PeopleCheck(
    GAMESYS_WORK *gsys );

extern GMEVENT * DEBUG_WFBC_100SetUp(
    GAMESYS_WORK *gsys );


#ifdef _cplusplus
}	// extern "C"{
#endif



