//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		event_battlerecorder.h
 *	@brief  �o�g�����R�[�_�[�C�x���g
 *	@author	Toru=Nagihashi
 *	@date		2009.12.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gflib.h>

#include "field/fieldmap.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "net_app/battle_recorder.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
FS_EXTERN_OVERLAY(event_btlrecorder);


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {
  FIELDMAP_WORK* fieldmap;
  BR_MODE mode;
} EV_WIFIBATTLERECORDER_PARAM;

//=============================================================================
/**
 *					�O�����J
 *  GMEVENT_CreateOverlayEventCall�p
*/
//=============================================================================
extern GMEVENT* EVENT_CallWifiBattleRecorder( GAMESYS_WORK * gsys, void* work );
