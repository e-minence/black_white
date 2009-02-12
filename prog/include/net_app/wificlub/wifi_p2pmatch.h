//============================================================================================
/**
 * @file	wifi_p2pmatch.h
 * @bfief	WIFI P2P接続のマッチング
 * @author	k.ohno
 * @date	06.04.07
 */
//============================================================================================
#ifndef __WIFI_P2PMATCH_H__
#define __WIFI_P2PMATCH_H__

#include <gflib.h>
#include "gamesystem/game_event.h"
#include "wifi_p2pmatch_def.h"

//============================================================================================
//	定義
//============================================================================================

//------------------------------------------------------
/**
 * WIFIともだちリストPROCパラメータ
 */
//------------------------------------------------------
typedef struct {
	SAVE_CONTROL_WORK*  pSaveData;
    int seq;                // どこに分岐してほしいかが入っている
    int targetID;   //対戦 交換する人が誰なのかが入っている
}WIFIP2PMATCH_PROC_PARAM;

extern const GFL_PROC_DATA WifiP2PMatchProcData;

// ポインタ参照だけできるレコードコーナーワーク構造体
typedef struct _WIFIP2PMATCH_WORK WIFIP2PMATCH_WORK;	

// 呼び出すためのイベント
extern void EventCmd_P2PMatchProc(GMEVENT * event);
// 世界交換前に呼び出すためのイベント
extern void EventCmd_DPWInitProc(GMEVENT * event);
//
extern void EventCmd_DPWInitProc2(GMEVENT * event, u16* ret);


//------------------------------------------------------------------
/*
   @title WIFIクラブイベントメモリを確保
 */
//------------------------------------------------------------------
extern void* WIFICLUB_CreateWork(void);


#endif //__WIFI_P2PMATCH_H__
