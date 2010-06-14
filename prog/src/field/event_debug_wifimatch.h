//============================================================================================
/**
 * @file	  event_wifibattlematch.h
 * @brief	  �C�x���g�FWIFI���E�ΐ�
 * @author	Toru=Nagihashi
 * @date	  2009.11.24
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "field/fieldmap.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "net_app/debug_wifi_match.h"

FS_EXTERN_OVERLAY(event_debug_wifimatch);

//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {
  FIELDMAP_WORK* fieldmap;
} EV_DEBUG_WIFIMATCH;



extern GMEVENT* EVENT_DEBUG_CallWifiMatch( GAMESYS_WORK * gsys, void* work );

