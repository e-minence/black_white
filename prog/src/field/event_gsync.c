//============================================================================================
/**
 * @file  event_ircbattle.c
 * @brief イベント：赤外線バトル
 * @author  k.ohno
 * @date  2009.03.24
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"
#include "field/field_sound.h"

#include "./event_fieldmap_control.h"
#include "./event_gsync.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "net_app/irc_match/ircbattlematch.h"
#include "net_app/irc_match/ircbattlemenu.h"
#include "net_app/friend_trade/ircbattlefriend.h"
#include "net_app/pokemontrade.h"
#include "net_app/irc_compatible.h"
#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h"      //GFL_HEAPID_APP参照

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// バトル用定義
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------


FS_EXTERN_OVERLAY(irc_compatible);

#define HEAPID_CORE GFL_HEAPID_APP

FS_EXTERN_OVERLAY(ohno_debugapp);
FS_EXTERN_OVERLAY(fieldmap);
FS_EXTERN_OVERLAY(ircbattlematch);
FS_EXTERN_OVERLAY(pokemon_trade);
extern const GFL_PROC_DATA G_SYNC_ProcData;

#define _LOCALMATCHNO (100)

enum _EVENT_IRCBATTLE {
  _IRCBATTLE_START,
  _IRCBATTLE_START_FIELD_CLOSE,
  _CALL_IRCBATTLE_MENU,
  _WAIT_IRCBATTLE_MENU,
  _FIELD_FADEOUT,
  _CALL_IRCBATTLE_MATCH,
  _WAIT_IRCBATTLE_MATCH,
  _BATTLE_MATCH_START,
  _TIMING_SYNC_CALL_BATTLE,
  _CALL_BATTLE,
  _WAIT_BATTLE,
  _CALL_IRCBATTLE_FRIEND,
  _WAIT_IRCBATTLE_FRIEND,
  _CALL_TRADE,
  _WAIT_TRADE,
  _CALL_NET_END,
  _WAIT_NET_END,
  _FIELD_OPEN,
  _FIELD_FADEIN,
  _FIELD_END,

  _FIELD_FADEOUT_IRCBATTLE,
  _FIELD_END_IRCBATTLE,
  _CALL_IRCCOMMPATIBLE,
  _WAIT_IRCCOMMPATIBLE,
};

struct _EVENT_GSYNC_WORK{
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  SAVE_CONTROL_WORK *ctrl;
  BATTLE_SETUP_PARAM para;
  BOOL isEndProc;
  int selectType;
  IRC_COMPATIBLE_PARAM  compatible_param; //赤外線メニューに渡す情報
  BOOL push;
#if PM_DEBUG
  int debugseq;
#endif

};


//============================================================================================
//
//    サブイベント
//
//============================================================================================
static GMEVENT_RESULT EVENT_GSyncMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_GSYNC_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;

#if PM_DEBUG
  if(dbw->debugseq != *seq){
    OS_TPrintf("e irc %d\n");
    dbw->debugseq = *seq;
  }
#endif

  switch (*seq) {
  case _IRCBATTLE_START:
    GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, dbw->fieldmap, FIELD_FADE_BLACK));
    (*seq) ++;
    break;
  case _IRCBATTLE_START_FIELD_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    //
    (*seq) = _CALL_IRCBATTLE_MENU;
    break;
  case _CALL_IRCBATTLE_MENU:
    dbw->isEndProc = FALSE;
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(ohno_debugapp), &G_SYNC_ProcData, dbw);
    (*seq)++;
    break;
  case _WAIT_IRCBATTLE_MENU:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    *seq = _FIELD_OPEN;
    break;
  case _FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case _FIELD_FADEIN:
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, dbw->fieldmap, 0));
    (*seq) ++;
    break;
  case _FIELD_END:
    if(dbw->push){
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PopBGM( fsnd );
      dbw->push=FALSE;
    }
    PMSND_FadeInBGM(60);
    return GMEVENT_RES_FINISH;

  //相性チェックはプロセス移動
  case _FIELD_FADEOUT_IRCBATTLE:
    GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, dbw->fieldmap, FIELD_FADE_BLACK));
    (*seq)++;
    break;
  case _FIELD_END_IRCBATTLE:
    //すでにCloseされている
    //GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    (*seq)++;
    break;
  case _CALL_IRCCOMMPATIBLE:  //相性チェック画面へ
    dbw->compatible_param.p_gamesys   = dbw->gsys;
#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
    dbw->compatible_param.is_only_play  = FALSE;
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(irc_compatible), &IrcCompatible_ProcData, &dbw->compatible_param );
    (*seq)++;
    break;
  case _WAIT_IRCCOMMPATIBLE:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL)
    {
      GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
      NET_PRINT("相性チェック画面おわり\n");
      (*seq)=_CALL_IRCBATTLE_MENU;
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_GSync(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * prevevent,BOOL bCreate)
{
  GMEVENT * event = prevevent;
  BATTLE_SETUP_PARAM * para;
  EVENT_GSYNC_WORK * dbw;

  if(bCreate){
    event = GMEVENT_Create(gsys, NULL, EVENT_GSyncMain, sizeof(EVENT_GSYNC_WORK));
  }
  else{
    GMEVENT_Change( event,EVENT_GSyncMain, sizeof(EVENT_GSYNC_WORK) );
  }
  dbw = GMEVENT_GetEventWork(event);
  dbw->ctrl = SaveControl_GetPointer();
  dbw->gsys = gsys;
  dbw->fieldmap = fieldmap;
  return event;
}


