//============================================================================================
/**
 * @file		report_event.h
 * @brief		フィールドメニュー・レポート処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 */
//============================================================================================
#pragma	once

#include "gamesystem/gamesystem.h"
#include "system/time_icon.h"
#include "app/app_taskmenu.h"


//============================================================================================
//	定数定義
//============================================================================================

typedef struct _REPORT_EVENT_LOCAL REPORT_EVENT_LOCAL;	// ローカルワーク

// ワーク
typedef struct {
	// 外から渡してください
	SAVE_CONTROL_WORK * sv;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldWork;
  FLDMSGBG * msgBG;
  HEAPID	heapID;
	void * resultAddr;

	REPORT_EVENT_LOCAL * local;		// ローカルワーク（内部で作成されます）

}FMENU_REPORT_EVENT_WORK;

enum {
	REPORTEVENT_RET_SAVE = 0,		// セーブ実行
	REPORTEVENT_RET_CANCEL,			// キャンセル
	REPORTEVENT_RET_NONE,				// 処理中
};

//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		メイン処理
 *
 * @param		wk		ワーク
 * @param		seq		シーケンス
 *
 * @retval	"REPORTEVENT_RET_SAVE = セーブ実行"
 * @retval	"REPORTEVENT_RET_CANCEL = キャンセル"
 * @retval	"REPORTEVENT_RET_NONE = 処理中"
 */
//--------------------------------------------------------------------------------------------
extern int REPORTEVENT_Main( FMENU_REPORT_EVENT_WORK * wk, int * seq );
