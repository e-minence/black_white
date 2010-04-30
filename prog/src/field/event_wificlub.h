//============================================================================================
/**
 * @file	event_wificlub.h
 * @brief	�C�x���g�FWIFI�N���u
 * @author	ohno
 * @date	2009.03.31
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap.h"

#include "battle/battle.h"
//typedef struct _EVENT_WIFICLUB_WORK EVENT_WIFICLUB_WORK;


FS_EXTERN_OVERLAY(event_wificlub);

typedef struct {
  GMEVENT * event;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
//  SAVE_CONTROL_WORK *ctrl;
  void* pWork;
  BOOL isEndProc;
  BOOL bFieldEnd;
  int selectType;
} EVENT_WIFICLUB_WORK;




//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� EV_WIFICLUB_PARAM*��n���B
//=====================================
//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {
  FIELDMAP_WORK * fieldmap;
  BOOL bFieldEnd;
} EV_WIFICLUB_PARAM;
extern GMEVENT* EVENT_CallWiFiClub( GAMESYS_WORK * gsys, void * work );

