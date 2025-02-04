//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_util.h
 *	@brief  ふしぎなおくりもので使用するモジュール類
 *	@author	Toru=Nagihashi
 *	@date		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//システム
#include "system/main.h"    //HEAPID
#include "print/gf_font.h"  //GFL_FONT
#include "print/printsys.h"
#include "system/bmp_menulist.h"  //BMPMENULIST_NULL
#include "system/bmp_oam.h"


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWINメッセージ描画構造体
 *					  ・文字描画はPRINTQUE
 *            ・自動転送モードと任意転送モードがある
 *            ・自動転送モードは作成や書込み後即時反映
 *            ・任意転送モードはMYSTERY_MSGWIN_Transをするまで画面反映をしない
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	座標位置
//=====================================
typedef enum 
{
  MYSTERY_MSGWIN_POS_ABSOLUTE,     //絶対位置
  MYSTERY_MSGWIN_POS_WH_CENTER,  //相対座標  幅、高さともに中央
  MYSTERY_MSGWIN_POS_H_CENTER,    //相対座標  高さのみ中央
  MYSTERY_MSGWIN_POS_W_BACK,     //後ろから
} MYSTERY_MSGWIN_POS;

#define MYSTERY_MSGWIN_DEFAULT_COLOR  (PRINTSYS_MACRO_LSB( 1, 2, 0 ))
#define MYSTERY_MSGWIN_WHITE_COLOR    (PRINTSYS_MACRO_LSB( 0xf, 2, 0 ))

//-------------------------------------
///	書き込みモード
//=====================================
typedef enum
{
  MYSTERY_MSGWIN_TRANS_MODE_AUTO,  //即時転送
  MYSTERY_MSGWIN_TRANS_MODE_MANUAL, //任意タイミングで転送関数MYSTERY_MSGWIN_Transを呼ぶことで反映
} MYSTERY_MSGWIN_TRANS_MODE;

//-------------------------------------
///	メッセージウィンドウ
//=====================================
typedef struct _MYSTERY_MSGWIN_WORK MYSTERY_MSGWIN_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern MYSTERY_MSGWIN_WORK * MYSTERY_MSGWIN_Init( MYSTERY_MSGWIN_TRANS_MODE mode, u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID );
extern void MYSTERY_MSGWIN_Clear( MYSTERY_MSGWIN_WORK* p_wk );
extern void MYSTERY_MSGWIN_Exit( MYSTERY_MSGWIN_WORK* p_wk );
extern void MYSTERY_MSGWIN_Print( MYSTERY_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void MYSTERY_MSGWIN_PrintBuf( MYSTERY_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void MYSTERY_MSGWIN_SetColor( MYSTERY_MSGWIN_WORK* p_wk, PRINTSYS_LSB color );
extern void MYSTERY_MSGWIN_SetPos( MYSTERY_MSGWIN_WORK* p_wk, s16 x, s16 y, MYSTERY_MSGWIN_POS type );
extern BOOL MYSTERY_MSGWIN_PrintMain( MYSTERY_MSGWIN_WORK* p_wk );
extern void MYSTERY_MSGWIN_Trans( MYSTERY_MSGWIN_WORK* p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					テキスト描画構造体
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	描画方式
//=====================================
typedef enum
{
  MYSTERY_TEXT_TYPE_QUE,     //プリントキューを使う
  MYSTERY_TEXT_TYPE_STREAM,  //ストリームを使う
  MYSTERY_TEXT_TYPE_WAIT,   //待機アイコン付き

  MYSTERY_TEXT_TYPE_MAX,    //c内部にて使用
} MYSTERY_TEXT_TYPE;

//-------------------------------------
///	メッセージウィンドウ
//=====================================
typedef struct _MYSTERY_TEXT_WORK MYSTERY_TEXT_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern MYSTERY_TEXT_WORK * MYSTERY_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern MYSTERY_TEXT_WORK * MYSTERY_TEXT_InitOneLine( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern void MYSTERY_TEXT_Exit( MYSTERY_TEXT_WORK* p_wk );
extern void MYSTERY_TEXT_Main( MYSTERY_TEXT_WORK* p_wk );

extern void MYSTERY_TEXT_Print( MYSTERY_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, MYSTERY_TEXT_TYPE type );
extern void MYSTERY_TEXT_PrintBuf( MYSTERY_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, MYSTERY_TEXT_TYPE type );
extern BOOL MYSTERY_TEXT_IsEndPrint( const MYSTERY_TEXT_WORK *cp_wk );

extern void MYSTERY_TEXT_WriteWindowFrame( MYSTERY_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt );
extern void MYSTERY_TEXT_EndWait( MYSTERY_TEXT_WORK *p_wk );
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  リスト
 *				    ・簡単にリストを出すために最大値決めうち
 *            ・表示数＝リスト最大数
 *            ・位置固定
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	定数
//=====================================
#define MYSTERY_LIST_SELECT_NULL  (BMPMENULIST_NULL)
#define MYSTERY_LIST_WINDOW_MAX   (3)

//-------------------------------------
///	選択リスト
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg;
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  u32 strID[MYSTERY_LIST_WINDOW_MAX];
  u32 list_max;

  u16 frm;
  u16 font_plt;
  u16 frm_plt;
  u16 frm_chr;

} MYSTERY_LIST_SETUP;

//-------------------------------------
///	選択リスト
//=====================================
typedef struct _MYSTERY_LIST_WORK MYSTERY_LIST_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern MYSTERY_LIST_WORK * MYSTERY_LIST_Init( const MYSTERY_LIST_SETUP *cp_setup, HEAPID heapID );
extern void MYSTERY_LIST_Exit( MYSTERY_LIST_WORK *p_wk );
extern u32 MYSTERY_LIST_Main( MYSTERY_LIST_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  BMPWINの集まり  いろんな文字をはる情報画面などで使用
 *				    ・BMPWINを一気に作成し、一気に書き込む
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	設定ワーク
//=====================================
typedef struct 
{
  u8  x;
  u8  y;
  u8  w;
  u8  h;
  u32 strID;
  STRBUF *p_strbuf;
  MYSTERY_MSGWIN_POS  pos_type;
  s16 pos_x;
  s16 pos_y;
  PRINTSYS_LSB  color;
} MYSTERY_MSGWINSET_SETUP_TBL;

//-------------------------------------
///	書き直し
//=====================================
typedef struct 
{
  BOOL  is_print;
  u32 strID;
  STRBUF *p_strbuf;
  MYSTERY_MSGWIN_POS  pos_type;
  s16 pos_x;
  s16 pos_y;
  PRINTSYS_LSB  color;
} MYSTERY_MSGWINSET_PRINT_TBL;

//-------------------------------------
///	メッセージウィンドウ
//=====================================
typedef struct _MYSTERY_MSGWINSET_WORK MYSTERY_MSGWINSET_WORK;
//-------------------------------------
///	パブリック
//=====================================
MYSTERY_MSGWINSET_WORK * MYSTERY_MSGWINSET_Init( MYSTERY_MSGWIN_TRANS_MODE mode, const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl, u32 tbl_len, u16 frm, u8 plt, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, GFL_FONT *p_font, HEAPID heapID );
extern void MYSTERY_MSGWINSET_Exit( MYSTERY_MSGWINSET_WORK* p_wk );
extern void MYSTERY_MSGWINSET_Clear( MYSTERY_MSGWINSET_WORK* p_wk );
extern BOOL MYSTERY_MSGWINSET_PrintMain( MYSTERY_MSGWINSET_WORK* p_wk );
extern void MYSTERY_MSGWINSET_Print( MYSTERY_MSGWINSET_WORK* p_wk, const MYSTERY_MSGWINSET_PRINT_TBL *cp_tbl );
extern void MYSTERY_MSGWINSET_ClearOne( MYSTERY_MSGWINSET_WORK* p_wk, u16 idx );

extern void MYSTERY_MSGWINSET_Trans( MYSTERY_MSGWINSET_WORK* p_wk );

//-------------------------------------
/// メモリ上に描画しておき、一気に書き込むためのバッファ
//=====================================
typedef struct _MYSTERY_MSGWINBUFF_WORK MYSTERY_MSGWINBUFF_WORK;

extern MYSTERY_MSGWINBUFF_WORK *MYSTERY_MSGWINSET_CreateBuff( MYSTERY_MSGWINSET_WORK *p_wk, HEAPID heapID );
extern void MYSTERY_MSGWINSET_DeleteBuff( MYSTERY_MSGWINBUFF_WORK *p_wk );

extern void MYSTERY_MSGWINSET_PrintBuff( MYSTERY_MSGWINBUFF_WORK *p_wk, const MYSTERY_MSGWINSET_PRINT_TBL *cp_tbl, PRINT_QUE *p_que );
extern void MYSTERY_MSGWINSET_SetBuff( MYSTERY_MSGWINBUFF_WORK *p_wk );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  メニュー
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	定数
//=====================================
#define MYSTERY_MENU_SELECT_NULL  (BMPMENULIST_NULL)
#define MYSTERY_MENU_SELECT_CENCEL  (BMPMENULIST_CANCEL)
#define MYSTERY_MENU_WINDOW_MAX   (4)


//-------------------------------------
///	カーソル保持等MENUが終了しても残しておくデータです
//  これをワーキングメモリにおいて、
//  MYSTERY_MENU_SETUPへポインタを渡してください
//=====================================
typedef struct 
{ 
  u32 cursor_pos;
} MYSTERY_MENU_DATA;

typedef void (*MYSTERY_MENU_CURSOR_CALLBACK)( void *p_wk_adrs );
//-------------------------------------
///	メニュー設定
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg; //NULLの場合は下記p_strbufを使用する、さもなくばstrIDを使い文章を作成
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  GFL_CLWK    *p_cursor;
  union
  { 
    u32     strID[MYSTERY_MENU_WINDOW_MAX];
    STRBUF  *p_strbuf[MYSTERY_MENU_WINDOW_MAX];  //すぐ開放してもOK
  };
  u32 list_max;
  u16 font_frm;
  u16 font_plt;
  u16 bg_frm; //枠  
  u16 bg_ofs;  //BGパレット

  s16 chr_y_ofs;
  s16 chr_x_ofs_str;

  u16 anm_seq;
  u16 padding_16;

  MYSTERY_MENU_CURSOR_CALLBACK callback;
  void *p_wk_adrs;

  MYSTERY_MENU_DATA *p_data;
} MYSTERY_MENU_SETUP;

//-------------------------------------
///	メニュー
//=====================================
typedef struct _MYSTERY_MENU_WORK MYSTERY_MENU_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern MYSTERY_MENU_WORK * MYSTERY_MENU_Init( const MYSTERY_MENU_SETUP *cp_setup, HEAPID heapID );
extern void MYSTERY_MENU_Exit( MYSTERY_MENU_WORK *p_wk );
extern u32 MYSTERY_MENU_Main( MYSTERY_MENU_WORK *p_wk );
extern BOOL MYSTERY_MENU_PrintMain( MYSTERY_MENU_WORK *p_wk );

extern void MYSTERY_MENU_SetBlink( MYSTERY_MENU_WORK *p_wk, u32 list_num, BOOL is_blink );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  シーケンス管理
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	シーケンスワーク
//=====================================
typedef struct _MYSTERY_SEQ_WORK MYSTERY_SEQ_WORK;

//-------------------------------------
///	シーケンス関数
//=====================================
typedef void (*MYSTERY_SEQ_FUNCTION)( MYSTERY_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	パブリック
//=====================================
extern MYSTERY_SEQ_WORK *MYSTERY_SEQ_Init( void *p_wk_adrs, MYSTERY_SEQ_FUNCTION seq_function, HEAPID heapID );
extern void MYSTERY_SEQ_Exit( MYSTERY_SEQ_WORK *p_wk );
extern void MYSTERY_SEQ_Main( MYSTERY_SEQ_WORK *p_wk );
extern BOOL MYSTERY_SEQ_IsEnd( const MYSTERY_SEQ_WORK *cp_wk );
extern void MYSTERY_SEQ_SetNext( MYSTERY_SEQ_WORK *p_wk, MYSTERY_SEQ_FUNCTION seq_function );
extern void MYSTERY_SEQ_End( MYSTERY_SEQ_WORK *p_wk );
extern void MYSTERY_SEQ_SetReservSeq( MYSTERY_SEQ_WORK *p_wk, int seq );
extern void MYSTERY_SEQ_NextReservSeq( MYSTERY_SEQ_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  MSGOAM  OAMで文字を書くモジュール
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	座標位置
//=====================================
typedef enum 
{
  MYSTERY_MSGOAM_POS_ABSOLUTE,    //絶対位置
  MYSTERY_MSGOAM_POS_WH_CENTER,  //相対座標  幅、高さともに中央
  MYSTERY_MSGOAM_POS_H_CENTER,  //相対座標  高さのみ中央
} MYSTERY_MSGOAM_POS;

//-------------------------------------
///	メッセージOAM
//=====================================
typedef struct _MYSTERY_MSGOAM_WORK MYSTERY_MSGOAM_WORK;
//-------------------------------------
///	パブリック
//=====================================
extern MYSTERY_MSGOAM_WORK * MYSTERY_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID );
extern void MYSTERY_MSGOAM_Exit( MYSTERY_MSGOAM_WORK* p_wk );
extern void MYSTERY_MSGOAM_Clear( MYSTERY_MSGOAM_WORK* p_wk );
extern void MYSTERY_MSGOAM_Print( MYSTERY_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void MYSTERY_MSGOAM_PrintBuf( MYSTERY_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void MYSTERY_MSGOAM_SetStrColor( MYSTERY_MSGOAM_WORK* p_wk, PRINTSYS_LSB color );
extern void MYSTERY_MSGOAM_SetStrPos( MYSTERY_MSGOAM_WORK* p_wk, s16 x, s16 y, MYSTERY_MSGOAM_POS type );
extern BOOL MYSTERY_MSGOAM_PrintMain( MYSTERY_MSGOAM_WORK* p_wk );
extern BMPOAM_ACT_PTR MYSTERY_MSGOAM_GetBmpOamAct( MYSTERY_MSGOAM_WORK* p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ETC
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
extern void MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
extern void MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, GXRgb start[], GXRgb end[] );
