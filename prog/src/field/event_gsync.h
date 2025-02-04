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
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "net/dreamworld_netdata.h"
#include "app/box2.h"
#include "app/box_mode.h"


FS_EXTERN_OVERLAY(event_gsync);



//-----------------------------------------------------------------------------
/**
 *    オーバーレイ管理＋イベント起動
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	GMEVENT_CreateOverlayEventCall関数用コールバック関数
//
//  void* work には FIELDMAP_WORK*を渡す。
//=====================================
extern GMEVENT* EVENT_CallGSync( GAMESYS_WORK* gsys, void* work );



//-----------------------------------------------------------------------------
/**
 */
//-----------------------------------------------------------------------------
typedef enum{
  GSYNC_CALLTYPE_INFO,    // 状態を確認>ダウンロード
  GSYNC_CALLTYPE_BOXSET,  // BOXポケモンセット後
  GSYNC_CALLTYPE_BOXSET_NOACC,// BOXポケモンセット後NOACC
  GSYNC_CALLTYPE_POKELIST,    // 選択可能なポケモンダウンロード
  GSYNC_CALLTYPE_BOXNULL,    // BOXにポケモンがから
} GSYNC_CALLTYPE_ENUM;



typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gameData;
  SAVE_CONTROL_WORK *ctrl;
  DREAM_WORLD_SERVER_STATUS_DATA aDreamStatus;  //ダウンロードしてきたステータス値
  BOOL baDreamStatusON;  //ダウンロードしたら1
  //  BATTLE_SETUP_PARAM para;
  DWCSvlResult aSVL;
  int selectType;
  BOOL push;
  u16 boxNo;
  u16 boxIndex;
  int PlayBGM;
  DREAM_WORLD_SERVER_POKEMONLIST_DATA selectPokeList;
  WIFILOGIN_PARAM aLoginWork;
  WIFILOGOUT_PARAM   logout;
  BOX2_GFL_PROC_PARAM boxParam;
  BOOL white_in;    //20100606 add Saito
} EVENT_GSYNC_WORK;

extern void EVENT_GSYNC_SetEnd(EVENT_GSYNC_WORK* pWk);





