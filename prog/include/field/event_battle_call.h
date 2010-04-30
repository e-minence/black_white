//============================================================================
/**
 *
 *	@file		event_battle_call.h
 *	@brief  イベント：バトル呼び出し
 *	@author hosaka genya
 *	@data		2010.01.22
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "battle/battle.h"
#include "demo/comm_btl_demo.h"
#include "net_app/btl_rec_sel.h"


FS_EXTERN_OVERLAY(event_battlecall);

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================
enum
{ 
  EVENT_BATTLE_ADD_CMD_TBL_TIMING = 200, ///< 同期NO
};

// PROC
extern const GFL_PROC_DATA CommBattleCommProcData;

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================
  
//--------------------------------------------------------------
///	通信バトル呼び出しPROC
//==============================================================
typedef struct {
  // [IN]
  GAMEDATA* gdata;
  BATTLE_SETUP_PARAM  *btl_setup_prm;
  COMM_BTL_DEMO_PARAM *demo_prm;
  int battle_mode;      ///<BATTLE_MODE_xxx
  int fight_count;      ///<連勝数
  // [PRIVATE]
  BTL_REC_SEL_PARAM  btl_rec_sel_param;
} COMM_BATTLE_CALL_PROC_PARAM;

//==============================================================================
//  通信バトル呼び出しイベント用ワーク
//==============================================================================
typedef struct{
  // [IN]
  GAMESYS_WORK * gsys;  
  BATTLE_SETUP_PARAM  *btl_setup_prm;
  COMM_BTL_DEMO_PARAM *demo_prm;
  // [PRIVATE]
  COMM_BATTLE_CALL_PROC_PARAM cbc;
}EVENT_BATTLE_CALL_WORK;


//-------------------------------------
///	パラメータ
//=====================================
typedef struct {
  BATTLE_SETUP_PARAM *btl_setup_prm;
  COMM_BTL_DEMO_PARAM *demo_prm;
} EV_BATTLE_CALL_PARAM;


//=============================================================================
/**
 *								EXTERN宣言
 */
//
//-------------------------------------
///	GMEVENT_CreateOverlayEventCall関数用コールバック関数
//
//  void* work には EV_BATTLE_CALL_PARAM*を渡す。
//=====================================

//==================================================================
/**
 * イベント作成：通信バトル呼び出し
 *
 * @param   gsys		
 * @param   work
 */
//==================================================================
extern GMEVENT * EVENT_CallCommBattle(GAMESYS_WORK * gsys, void* work );

