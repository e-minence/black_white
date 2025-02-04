//============================================================================================
/**
 * @file		zukan.h
 * @brief		図鑑画面
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	モジュール名：ZUKAN
 */
//============================================================================================
#pragma	once

#include "gamesystem/game_data.h"
#include "savedata/zukan_savedata.h"
#include "savedata/mystatus.h"


//============================================================================================
//	定数定義
//============================================================================================
typedef struct {
	// [ in ]
  GAMEDATA * gamedata;				// ゲームデータ
	ZUKAN_SAVEDATA * savedata;	// 図鑑セーブデータ
	MYSTATUS * mystatus;				// プレイヤーデータ

	u16	callMode;					// 呼び出しモード

	// [ out ]
	u16	retMode;					// 終了モード
}ZUKAN_PARAM;

// 呼び出しモード
enum {
	ZUKAN_MODE_TOP = 0,			// トップ画面
	ZUKAN_MODE_LIST,				// リスト画面
	ZUKAN_MODE_INFO,				// 情報
	ZUKAN_MODE_MAP,					// 分布
	ZUKAN_MODE_VOICE,				// 鳴き声
	ZUKAN_MODE_FORM,				// 姿
	ZUKAN_MODE_SEARCH,			// 検索
};

// 終了モード
enum {
	ZUKAN_RET_NORMAL = 0,		// 通常終了（戻るボタン）
	ZUKAN_RET_MENU_CLOSE,		// 終了してフィールドメニューを閉じる（×ボタン）
};


//============================================================================================
//	グローバル
//============================================================================================

// 図鑑PROCデータ
extern const GFL_PROC_DATA ZUKAN_ProcData;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

FS_EXTERN_OVERLAY(zukan);
