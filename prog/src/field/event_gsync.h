//============================================================================================
/**
 * @file	  event_gsync.h
 * @brief	  イベント：GAMESYNC
 * @author	k.ohno
 * @date	  2009.01.19
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap.h"
#include "net_app/gtsnego.h"


typedef struct _EVENT_GSYNC_WORK EVENT_GSYNC_WORK;

struct _EVENT_GSYNC_WORK{
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  SAVE_CONTROL_WORK *ctrl;
  BATTLE_SETUP_PARAM para;
  BOOL isEndProc;
  int selectType;
  BOOL push;
  EVENT_GTSNEGO_WORK aLoginWork;
};

extern void EVENT_GSYNC_SetEnd(EVENT_GSYNC_WORK* pWk);


extern GMEVENT* EVENT_GSync(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * prevevent,BOOL bCreate);

