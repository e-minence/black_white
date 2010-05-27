//============================================================================================
/**
 * @file		zukantop.h
 * @brief		図鑑トップ画面
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 *
 *	モジュール名：ZUKANTOP
 */
//============================================================================================
#pragma	once

#include "gamesystem/game_data.h"
#include "savedata/mystatus.h"


//============================================================================================
//	定数定義
//============================================================================================

// 外部設定データ
typedef struct {
	// [in]
  GAMEDATA * gamedata;				// ゲームデータ
	MYSTATUS * mystatus;				// プレイヤーデータ
	// [out]
	u32	retMode;								// 終了モード
}ZUKANTOP_DATA;

// 終了モード
enum {
	ZKNTOP_RET_EXIT = 0,			// 図鑑終了
	ZKNTOP_RET_EXIT_X,				// 図鑑を終了してメニューを閉じる
	ZKNTOP_RET_LIST,					// リスト画面へ
};


//============================================================================================
//	グローバル
//============================================================================================

// 図鑑トップ画面PROCデータ
extern const GFL_PROC_DATA ZUKANTOP_ProcData;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

FS_EXTERN_OVERLAY(zukan_top);
