//============================================================================================
/**
 * @file    event_cg_wireless.c
 * @brief	  イベント：パレスとTVトランシーバー
 * @author	k.ohno
 * @date	  2009.12.13
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
#include "./event_cg_wireless.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "net_app/irc_match/ircbattlematch.h"
#include "net_app/irc_match/ircbattlemenu.h"
#include "net_app/friend_trade/ircbattlefriend.h"
#include "net_app/pokemontrade.h"
#include "net_app/gsync.h"

#include "net/dwc_raputil.h"

#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h"      //GFL_HEAPID_APP参照

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "savedata/dreamworld_data.h"



//============================================================================================
//============================================================================================



FS_EXTERN_OVERLAY(gamesync);
FS_EXTERN_OVERLAY(fieldmap);
FS_EXTERN_OVERLAY(wifi_login);
extern const GFL_PROC_DATA WiFiLogin_ProcData;


#define _LOCALMATCHNO (100)

enum _EVENT_CG_WIRELESS {
  _FIELD_FADE_START,
  _FIELD_FADE_CLOSE,
  _CALL_GAMESYNC_MENU,
  _WAIT_GAMESYNC_MENU,
  _FIELD_FADEOUT,
  _WAIT_IRCBATTLE_MATCH,
  _BATTLE_MATCH_START,
  _TIMING_SYNC_CALL_BATTLE,
  _FIELD_OPEN,
  _FIELD_FADEIN,
  _FIELD_END,
  _FADEIN_WIFIUTIL,

  _FIELD_FADEOUT_IRCBATTLE,
  _FIELD_END_IRCBATTLE,
  _CALL_GAMESYNC,
  _WAIT_GAMESYNCLOGIN,
  _GAMESYNC_MAINPROC,
  _WAIT_GAMESYNC_MAINPROC,
  _GAMESYNC_CALLBOX,
  _GAMESYNC_CALLBOX_WAIT,
};


//============================================================================================
//
//    サブイベント
//
//============================================================================================
static GMEVENT_RESULT EVENT_CG_WirelessMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_CG_WIRELESS_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;

  switch (*seq) {
  case _FIELD_FADE_START:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, dbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case _FIELD_FADE_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    (*seq) = _CALL_GAMESYNC_MENU;
//     (*seq) = _GAMESYNC_MAINPROC;
    break;
  case _CALL_GAMESYNC_MENU:

    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushPlayEventBGM( fsnd, SEQ_BGM_GAME_SYNC );
      dbw->push=TRUE;
    }

    dbw->isEndProc = FALSE;
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(gamesync), &GameSyncMenuProcData, dbw);
    (*seq)++;
    break;
  case _WAIT_GAMESYNC_MENU:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    if(dbw->selectType == WIRELESS_CALLTYPE_PALACE )
    {
      *seq = _CALL_GAMESYNC;
    }
    else if(dbw->selectType == WIRELESS_CALLTYPE_TV )
    {
      *seq = _FIELD_FADEOUT;
    }
    else{
      *seq = _FIELD_OPEN;
    }
    break;

  case _FIELD_FADEOUT:
    dbw->isEndProc = FALSE;
    (*seq)++;
    break;
  case _FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case _FIELD_FADEIN:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, dbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
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
  case _FIELD_FADEOUT_IRCBATTLE:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, dbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq)++;
    break;
  case _FIELD_END_IRCBATTLE:
    (*seq)++;
    break;
  case _CALL_GAMESYNC:
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &dbw->aLoginWork);
    (*seq)++;
    break;
  case _WAIT_GAMESYNCLOGIN:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      if(FALSE==DREAMWORLD_SV_GetSleepPokemonFlg(DREAMWORLD_SV_GetDreamWorldSaveData(dbw->ctrl))){
        (*seq)=_GAMESYNC_CALLBOX;
      }
      else{
        (*seq)=_GAMESYNC_MAINPROC;
      }
    }
    break;
 case _GAMESYNC_MAINPROC:
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(gamesync), &G_SYNC_ProcData, dbw);
    (*seq)++;
    break;
  case _WAIT_GAMESYNC_MAINPROC:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL)
    {
      GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
      (*seq)=_CALL_GAMESYNC_MENU;
    }
    break;
  case _GAMESYNC_CALLBOX:
    dbw->boxParam.gamedata = GAMESYSTEM_GetGameData( gsys );
    dbw->boxParam.sv_box = GAMEDATA_GetBoxManager(dbw->boxParam.gamedata);
    dbw->boxParam.pokeparty = GAMEDATA_GetMyPokemon(dbw->boxParam.gamedata);			// 手持ちモケモン
    dbw->boxParam.myitem = GAMEDATA_GetMyItem(dbw->boxParam.gamedata);					// 所持アイテム（バッグで使用）
    dbw->boxParam.mystatus = GAMEDATA_GetMyStatus(dbw->boxParam.gamedata);				// プレイヤーデータ（バッグで使用）
    dbw->boxParam.cfg = SaveData_GetConfig(GAMEDATA_GetSaveControlWork(dbw->boxParam.gamedata));								// コンフィグデータ
    dbw->boxParam.zknMode = 0;								// 図鑑ナンバー表示モード
    dbw->boxParam.callMode = BOX_MODE_SLEEP;	// 寝かせる;					// 呼び出しモード
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &dbw->boxParam );
    (*seq)++;
    break;
  case _GAMESYNC_CALLBOX_WAIT:
    dbw->boxNo = dbw->boxParam.retTray;      		// 終了時に開いていたトレイ（寝かせる用）
    dbw->boxIndex = dbw->boxParam.retPoke;   		// 終了時に選択された位置（寝かせる用）
    dbw->selectType = CG_WIRELESS_CALLTYPE_BOXSET;  // ポケモンセット後
    (*seq) = _GAMESYNC_MAINPROC;
    
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_CG_Wireless(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * prevevent,BOOL bCreate)
{
  GMEVENT * event = prevevent;
  BATTLE_SETUP_PARAM * para;
  EVENT_CG_WIRELESS_WORK * dbw;

  if(bCreate){
    event = GMEVENT_Create(gsys, NULL, EVENT_CG_WirelessMain, sizeof(EVENT_CG_WIRELESS_WORK));
  }
  else{
    GMEVENT_Change( event,EVENT_CG_WirelessMain, sizeof(EVENT_CG_WIRELESS_WORK) );
  }
  dbw = GMEVENT_GetEventWork(event);
  dbw->ctrl = SaveControl_GetPointer();
  dbw->gsys = gsys;
  dbw->gameData = GAMESYSTEM_GetGameData(gsys);
  dbw->fieldmap = fieldmap;
  dbw->aLoginWork.gsys=gsys;
  dbw->aLoginWork.gamedata = GAMESYSTEM_GetGameData(gsys);
  dbw->aLoginWork.bDreamWorld = TRUE;
  return event;
}


