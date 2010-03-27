//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_rail_slipdown.h
 *	@brief  �`�����s�I���}�b�v�@���[���}�b�v�@���藎���C�x���g
 *	@author	tomoya takahashi
 *	@date		2009.10.16
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
#include "gamesystem/game_event.h"

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
//-------------------------------------
///	RAIL_SLIPDOWN_WORK
//=====================================
typedef struct _RAIL_SLIPDOWN_WORK RAIL_SLIPDOWN_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

extern GMEVENT* EVENT_RailSlipDown(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);
extern GMEVENT* EVENT_RailSlipDownObj(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, MMDL* mmdl);


// TCB�œ������B
extern RAIL_SLIPDOWN_WORK* RailSlipDown_Create(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, MMDL* mmdl, BOOL jiki);
extern void RailSlipDown_Delete( RAIL_SLIPDOWN_WORK* p_wk );
extern BOOL RailSlipDown_IsEnd( const RAIL_SLIPDOWN_WORK* cp_wk );


#ifdef _cplusplus
}	// extern "C"{
#endif



