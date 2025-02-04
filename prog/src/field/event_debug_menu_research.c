//======================================================================
/**
 * @brief  デバッグメニュー『すれ違い調査』
 * @file   event_debug_menu_research.c
 * @author obata
 * @date   2010.05.01
 */
//======================================================================
#ifdef  PM_DEBUG

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "gamesystem/game_event.h"
#include "gamesystem/gamesystem.h"
#include "arc_def.h"

#include "field/field_msgbg.h"

#include "debug_message.naix"
#include "msg/debug/msg_d_field.h"

#include "fieldmap.h"
#include "font/font.naix"
#include "field_debug.h"
#include "field_event_check.h"
#include "event_debug_local.h"
#include "event_debug_numinput_research.h"
#include "event_debug_menu_research.h"


//======================================================================
//  define
//======================================================================
#define D_MENU_CHARSIZE_X (30)    //メニュー横幅
#define D_MENU_CHARSIZE_Y (16)    //メニュー縦幅


//======================================================================
//  proto
//======================================================================
static FLDMENUFUNC * DEBUGFLDMENU_InitExPos( FIELDMAP_WORK * fieldmap, HEAPID heapID,
    const DEBUG_MENU_INITIALIZER * init, u16 list_pos, u16 cursor_pos );
static void DebugMenu_IineCallBack(BMPMENULIST_WORK* lw,u32 param,u8 y); 
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk );
static BOOL debugMenuCallProc_Team( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_QTotal( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_ATotal( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_QToday( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_AToday( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_Random( DEBUG_MENU_EVENT_WORK * p_wk );
static BOOL debugMenuCallProc_Clear( DEBUG_MENU_EVENT_WORK * p_wk );


//======================================================================
//  デバッグメニューリスト
//======================================================================
//--------------------------------------------------------------
/// デバッグメニューリスト
/// データを追加する事でメニューの項目も増えます。
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_DebugMenuList[] =
{
  { DEBUG_FIELD_RESEARCH_team,   debugMenuCallProc_Team },   // 調査隊関連
  { DEBUG_FIELD_RESEARCH_Qtotal, debugMenuCallProc_QTotal }, // いままでの調査人数 ( 質問 )
  { DEBUG_FIELD_RESEARCH_Atotal, debugMenuCallProc_ATotal }, // いままでの調査人数 ( 回答 )
  { DEBUG_FIELD_RESEARCH_Qtoday, debugMenuCallProc_QToday }, // 今日の調査人数 ( 質問 )
  { DEBUG_FIELD_RESEARCH_Atoday, debugMenuCallProc_AToday }, // 今日の調査人数 ( 回答 )
  { DEBUG_FIELD_RESEARCH_Random, debugMenuCallProc_Random }, // ランダムセットアップ
  { DEBUG_FIELD_RESEARCH_clear,  debugMenuCallProc_Clear },  // 調査データクリア
}; 

//--------------------------------------------------------------
/// メニューヘッダー
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_DebugMenuListHeader =
{
  1,    //リスト項目数
  9,    //表示最大項目数
  0,    //ラベル表示Ｘ座標
  13,   //項目表示Ｘ座標
  0,    //カーソル表示Ｘ座標
  0,    //表示Ｙ座標
  1,    //表示文字色
  15,   //表示背景色
  2,    //表示文字影色
  0,    //文字間隔Ｘ
  2,    //文字間隔Ｙ
  FLDMENUFUNC_SKIP_LRKEY, //ページスキップタイプ
  12,   //文字サイズX(ドット
  12,   //文字サイズY(ドット
  0,    //表示座標X キャラ単位
  0,    //表示座標Y キャラ単位
  0,    //表示サイズX キャラ単位
  0,    //表示サイズY キャラ単位
  DebugMenu_IineCallBack,  //１行ごとのコールバック
};

//--------------------------------------------------------------
/**
 * @brief フィールドデバッグメニュー初期化データ
 */
//--------------------------------------------------------------
static const DEBUG_MENU_INITIALIZER DebugMenuData = {
  NARC_debug_message_d_field_dat,
  NELEMS(DATA_DebugMenuList),
  DATA_DebugMenuList,
  &DATA_DebugMenuListHeader,
  1, 1, D_MENU_CHARSIZE_X, D_MENU_CHARSIZE_Y,
  NULL,
  NULL,
};

//======================================================================
//  イベント：フィールドデバッグメニュー
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドデバッグメニューイベント起動
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param heapID  HEAPID
 * @param page_id デバッグメニューページ
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_DebugMenu_Research( GAMESYS_WORK *gsys, void* wk )
{
  DEBUG_MENU_EVENT_WORK * dmew;
  GMEVENT * event;

  event = GMEVENT_Create(
    gsys, NULL, DebugMenuEvent, sizeof(DEBUG_MENU_EVENT_WORK));

  dmew = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( dmew, sizeof(DEBUG_MENU_EVENT_WORK) );

  dmew->gmSys     = gsys;
  dmew->gmEvent   = event;
  dmew->gdata     = GAMESYSTEM_GetGameData( gsys );
  dmew->fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  dmew->heapID    = HEAPID_PROC;

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * イベント：フィールドデバッグメニュー
 * @param event GMEVENT
 * @param seq   シーケンス
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugMenuEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_MENU_EVENT_WORK *work = wk;

  switch (*seq) {
  case 0:
    work->menuFunc = DEBUGFLDMENU_Init( work->fieldWork, work->heapID,  &DebugMenuData );
    (*seq)++;
    break;

  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( work->menuFunc );

      if( ret == FLDMENUFUNC_NULL ) {  // 操作無し
        break;
      }
			else if( ret == FLDMENUFUNC_CANCEL ) {  // キャンセル
				FLDMENUFUNC_DeleteMenu( work->menuFunc );
				return( GMEVENT_RES_FINISH );
      }
			else {  // 決定
        work->call_proc = (DEBUG_MENU_CALLPROC)ret;
				FLDMENUFUNC_DeleteMenu( work->menuFunc );
        (*seq)++;
      }
    }
    break;

  case 2:
		if( work->call_proc(work) == TRUE ) { 
			(*seq) = 0;
		}
    break;

	case 3:
		return( GMEVENT_RES_FINISH );
  } 
  return( GMEVENT_RES_CONTINUE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  １行ごとのコールバック
 *
 *  @param  lw    ワーク
 *  @param  param 引数
 *  @param  y     Y位置
 */
//-----------------------------------------------------------------------------
static void DebugMenu_IineCallBack( BMPMENULIST_WORK* lw, u32 param, u8 y )
{
  if( param == BMPMENULIST_LABEL ) {
    BmpMenuList_TmpColorChange( lw, 4, 0, 4 );
  }
	else {
    BmpMenuList_TmpColorChange( lw, 1, 0, 2 );
  }
}

//======================================================================
//  デバッグメニュー呼び出し
//======================================================================

//--------------------------------------------------------------
/// デバッグメニュー：調査隊関連
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Team( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 0 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// デバッグメニュー：いままでの調査人数 ( 質問 )
//--------------------------------------------------------------
static BOOL debugMenuCallProc_QTotal( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 1 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// デバッグメニュー：いままでの調査人数 ( 回答 )
//--------------------------------------------------------------
static BOOL debugMenuCallProc_ATotal( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 2 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// デバッグメニュー：今日の調査人数 ( 質問 )
//--------------------------------------------------------------
static BOOL debugMenuCallProc_QToday( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 3 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// デバッグメニュー：今日の調査人数 ( 回答 )
//--------------------------------------------------------------
static BOOL debugMenuCallProc_AToday( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 4 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// デバッグメニュー：ランダムセットアップ
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Random( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 5 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}
//--------------------------------------------------------------
/// デバッグメニュー：調査データクリア
//--------------------------------------------------------------
static BOOL debugMenuCallProc_Clear( DEBUG_MENU_EVENT_WORK * p_wk )
{
  GMEVENT* event;

  event = DEBUG_EVENT_FLDMENU_ResearchNumInput( p_wk->gmSys, p_wk, 6 );

  GMEVENT_CallEvent( p_wk->gmEvent, event );

  return TRUE;
}

#endif  //  PM_DEBUG 
