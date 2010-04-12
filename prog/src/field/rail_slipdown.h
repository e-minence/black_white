//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		rail_slipdown.h
 *	@brief  ���[���@����~��
 *	@author	tomoya takahashi
 *	@date		2010.04.12
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "fldmmdl.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(field_d09_slipdown);

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	RAIL_SLIPDOWN_WORK
//=====================================
typedef struct _RAIL_SLIPDOWN_WORK RAIL_SLIPDOWN_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


// TCB�œ������B
extern RAIL_SLIPDOWN_WORK* RailSlipDown_Create(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, MMDL* mmdl, BOOL jiki);
extern void RailSlipDown_Delete( RAIL_SLIPDOWN_WORK* p_wk );
extern BOOL RailSlipDown_IsEnd( const RAIL_SLIPDOWN_WORK* cp_wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



