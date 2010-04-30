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
#include "net_app/wifibattlematch.h"

FS_EXTERN_OVERLAY(event_wifibtlmatch);

#ifdef PM_DEBUG
#define DEBUG_MODE_BIT    (0x10000)
#define DEBUG_MODE(mode)  (mode|DEBUG_MODE_BIT)
#else
#define DEBUG_MODE(mode)  (mode)
#endif //PM_DEBUG




//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� EV_WIFIBATTLEMATCH_PARAM*��n���B
//=====================================
//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {

  FIELDMAP_WORK* fieldmap;
  WIFIBATTLEMATCH_MODE mode;
  WIFIBATTLEMATCH_POKE  poke;
  WIFIBATTLEMATCH_BTLRULE btl_rule;
  
} EV_WIFIBATTLEMATCH_PARAM;

extern GMEVENT* EVENT_CallWifiBattleMatch( GAMESYS_WORK* gsys, void* work );

