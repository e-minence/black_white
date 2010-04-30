//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		event_gts.h
 *	@brief  �C�x���g�FGTS
 *	@author	Toru=Nagihashi
 *	@date		2010.03.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

FS_EXTERN_OVERLAY(event_gts);

//-------------------------------------
///	EVENT_CallGts�p�p�����[�^
//=====================================
typedef struct {
  FIELDMAP_WORK* fieldmap;
  BOOL is_debug;
} EV_GTS_PARAM;


//=============================================================================
/**
 *					�O�����J
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
*/
//=============================================================================
extern GMEVENT* EVENT_CallGts( GAMESYS_WORK * p_gamesys, void* work );

