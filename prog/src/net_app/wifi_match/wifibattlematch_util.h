//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_util.h
 *	@brief  世界対戦用共通モジュール
 *	@author	Toru=Nagihashi
 *	@date		2009.11.29
 *
 *	WBM = WifiBattleMatch
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "system/main.h"    //HEAPID
#include "print/gf_font.h"  //GFL_FONT
#include "system/bmp_menulist.h"  //BMPMENULIST_NULL
#include "gamesystem/game_data.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					テキスト描画構造体
 *            ・文字描画はストリーム
 *            ・テキスト範囲は画面下部文字2行ほど
 *            ・BMPWINの周りにフレームがつく
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	描画方式
//=====================================
typedef enum
{
  WBM_TEXT_TYPE_QUE,     //プリントキューを使う
  WBM_TEXT_TYPE_STREAM,  //ストリームを使う
  WBM_TEXT_TYPE_WAIT,   //待機アイコンをつかう  終了はEndWait

  WBM_TEXT_TYPE_MAX,    //c内部にて使用
} WBM_TEXT_TYPE;

//-------------------------------------
///	メッセージウィンドウ
//=====================================
typedef struct _WBM_TEXT_WORK WBM_TEXT_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern WBM_TEXT_WORK * WBM_TEXT_Init( u16 frm, u16 font_plt, u16 frm_plt, u16 frm_chr, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern void WBM_TEXT_Exit( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_Main( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_Print( WBM_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, WBM_TEXT_TYPE type );
extern BOOL WBM_TEXT_IsEnd( const WBM_TEXT_WORK* cp_wk );
extern void WBM_TEXT_EndWait( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_ClearCharacterVram( WBM_TEXT_WORK* p_wk );


extern void WBM_TEXT_PrintDebug( WBM_TEXT_WORK* p_wk, const u16 *cp_str, u16 len, GFL_FONT *p_font );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  リスト
 *				    ・簡単にリストを出すために最大値決めうち
 *            ・表示数＝リスト最大数
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	定数
//=====================================
#define WBM_LIST_SELECT_NULL    (BMPMENULIST_NULL)
#define WBM_LIST_WINDOW_MAX     (4)

//-------------------------------------
///	選択リスト
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg;
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  u32 strID[WBM_LIST_WINDOW_MAX];
  u32 list_max;

  u16 frm;
  u16 font_plt;
  u16 frm_plt;
  u16 frm_chr;

  BOOL  is_cancel;
  u16   cancel_idx;
  u16   default_idx;
} WBM_LIST_SETUP;

//-------------------------------------
///	選択リスト
//=====================================
typedef struct _WBM_LIST_WORK WBM_LIST_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern WBM_LIST_WORK * WBM_LIST_Init( const WBM_LIST_SETUP *cp_setup, HEAPID heapID );
extern WBM_LIST_WORK * WBM_LIST_InitEx( const WBM_LIST_SETUP *cp_setup, u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
extern void WBM_LIST_Exit( WBM_LIST_WORK *p_wk );
extern u32 WBM_LIST_Main( WBM_LIST_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  シーケンス管理
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	シーケンスワーク
//=====================================
typedef struct _WBM_SEQ_WORK WBM_SEQ_WORK;

//-------------------------------------
///	シーケンス関数
//=====================================
typedef void (*WBM_SEQ_FUNCTION)( WBM_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	パブリック
//=====================================
extern WBM_SEQ_WORK *WBM_SEQ_Init( void *p_wk_adrs, WBM_SEQ_FUNCTION seq_function, HEAPID heapID );
extern void WBM_SEQ_Exit( WBM_SEQ_WORK *p_wk );
extern void WBM_SEQ_Main( WBM_SEQ_WORK *p_wk );
extern BOOL WBM_SEQ_IsEnd( const WBM_SEQ_WORK *cp_wk );
extern void WBM_SEQ_SetNext( WBM_SEQ_WORK *p_wk, WBM_SEQ_FUNCTION seq_function );
extern void WBM_SEQ_End( WBM_SEQ_WORK *p_wk );
extern void WBM_SEQ_SetReservSeq( WBM_SEQ_WORK *p_wk, int seq );
extern void WBM_SEQ_NextReservSeq( WBM_SEQ_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  サブプロセス
 *				    ・プロセスを行き来するシステム
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	ワーク
//=====================================
typedef struct _WBM_SUBPROC_WORK WBM_SUBPROC_WORK;

//-------------------------------------
///	サブプロセス初期化・解放関数コールバック
//=====================================
typedef void *(*WBM_SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs );
typedef BOOL (*WBM_SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );

//-------------------------------------
///	サブプロセス設定構造体
//=====================================
typedef struct 
{
	FSOverlayID							    ov_id;
	const GFL_PROC_DATA			    *cp_procdata;
	WBM_SUBPROC_ALLOC_FUNCTION	alloc_func;
	WBM_SUBPROC_FREE_FUNCTION		free_func;
} WBM_SUBPROC_DATA;

//-------------------------------------
///	パブリック
//=====================================
extern WBM_SUBPROC_WORK * WBM_SUBPROC_Init( const WBM_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID );
extern void WBM_SUBPROC_Exit( WBM_SUBPROC_WORK *p_wk );
extern BOOL WBM_SUBPROC_Main( WBM_SUBPROC_WORK *p_wk );
extern GFL_PROC_MAIN_STATUS WBM_SUBPROC_GetStatus( const WBM_SUBPROC_WORK *cp_wk );
extern void WBM_SUBPROC_CallProc( WBM_SUBPROC_WORK *p_wk, u32 procID );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *			  勝敗カウント
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
extern void WBM_RECORD_Count( GAMEDATA *p_gamedata, BtlResult result );

