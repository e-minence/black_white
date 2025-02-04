//============================================================================================
/**
 * @file	world_trade.h
 * @bfief	世界交換処理
 * @author	Akito Mori
 * @date	06.04.16
 */
//============================================================================================
#ifndef _WORLD_TRADE_H_
#define _WORLD_TRADE_H_

#include "savedata/worldtrade_data.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "savedata/config.h"
#include "savedata/record.h"
#include "savedata/system_data.h"
#include "savedata/myitem_savedata.h"
#include "savedata/box_savedata.h"
#include "savedata/wifilist.h"
#include "gamesystem/gamesystem.h"
#include "savedata/zukan_savedata.h"

//============================================================================================
//	定義
//============================================================================================
extern const GFL_PROC_DATA WorldTrade_ProcData;


typedef struct{
	WORLDTRADE_DATA *worldtrade_data;	// 世界交換データ
	SYSTEMDATA		*systemdata;		// システムセーブデータ（DPWライブラリ用ともだちコードを保存）
	POKEPARTY       *myparty;			// 手持ちポケモン
	BOX_MANAGER        *mybox;				// ボックスデータ
	ZUKAN_SAVEDATA      *zukanwork;			// 図鑑データ
	WIFI_LIST		*wifilist;			// ともだち手帳
	WIFI_HISTORY	*wifihistory;		// 地球儀データ
	MYSTATUS		*mystatus;			// トレーナー情報
	CONFIG			*config;			// コンフィグ構造体
	RECORD			*record;			// レコードコーナーポインタ
	MYITEM			*myitem;			// アイテムポケットポインタ
	int				zukanmode;			// 図鑑モード（シンオウ・全国)
	int				profileId;			// GameSpyプロファイルID
	int				contestflag;		// コンテスト見えてるかフラグ
	int				connect;			// 初回ＧＳＩＤを取得した（1:取得してきた 0:もう持ってる)
	SAVE_CONTROL_WORK * savedata;
  GAMESYS_WORK *gamesys;
}WORLDTRADE_PARAM;

#endif
