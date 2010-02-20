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
#include "app/app_taskmenu.h"


//============================================================================================
//	定数定義
//============================================================================================

// ワーク
typedef struct {
	SAVE_CONTROL_WORK * sv;
  GAMESYS_WORK * gsys;

  FIELDMAP_WORK * fieldWork;
  FLDMSGBG * msgBG;

	GFL_TCBLSYS * tcbl;

  GFL_MSGDATA * msgData;
	GFL_BMPWIN * win;
	PRINT_STREAM * stream;
	STRBUF * strBuff;

	APP_TASKMENU_ITEMWORK	ynList[2];
	APP_TASKMENU_RES * ynRes;
	APP_TASKMENU_WORK * ynWork;

  HEAPID	heapID;

}FMENU_REPORT_EVENT_WORK;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * メイン処理
 *
 * @param		wk		ワーク
 * @param		seq		シーケンス
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = 終了"
 */
//--------------------------------------------------------------------------------------------
extern BOOL REPORTEVENT_Main( FMENU_REPORT_EVENT_WORK * wk, int * seq );
