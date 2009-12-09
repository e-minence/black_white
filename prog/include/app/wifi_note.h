//============================================================================================
/**
 * @file	wifi_note.h
 * @bfief	WIFI友達手帳
 * @author	k.ohno
 * @date	06.04.05
 */
//============================================================================================
#pragma once

//============================================================================================
//	定義
//============================================================================================

#include "gamesystem/game_data.h"

//------------------------------------------------------
/**
 * WIFIともだちリストPROCパラメータ
 */
//------------------------------------------------------
typedef struct {
  GAMEDATA* pGameData;
}WIFINOTE_PROC_PARAM;


// プロセス定義データ
extern const GFL_PROC_DATA WifiNoteProcData;

// ポインタ参照だけできるレコードコーナーワーク構造体
typedef struct _WIFINOTE_WORK WIFINOTE_WORK;	

