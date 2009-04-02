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

typedef struct {
	GAMESYS_WORK * gsys;
	FIELD_MAIN_WORK * fieldmap;
    SAVE_CONTROL_WORK *ctrl;
    BATTLE_SETUP_PARAM para;
    BOOL isEndProc;
    int selectType;
} EVENT_WIFICLUB_WORK;


extern void EVENT_WiFiClub(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,GMEVENT * event);

