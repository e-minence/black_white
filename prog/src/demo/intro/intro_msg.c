//=============================================================================
/**
 *
 *	@file		intro_msg.c
 *	@brief  イントロ メッセージ処理
 *	@author	hosaka genya
 *	@data		2009.12.15
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/bmp_winframe.h" // for BmpWinFrame_
#include "system/bmp_menulist.h" // for BMPMENU_LIST_WORK

// 文字列関連
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" // for PRINT_QUE

#include "gamesystem/msgspeed.h"  // for MSGSPEED_GetWait

#include "font/font.naix"

#include "sound/pm_sndsys.h" // for 

//アーカイブ
#include "arc_def.h" // for ARCID_XXX

#include "print/printsys.h" // for PRINTSYS_

#include "intro_sys.h"
#include "intro_msg.h" // for extern

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================
enum
{ 
  CGX_BMPWIN_FRAME_POS = 1,
  STRBUF_SIZE = 1600,
};

#define MSG_SKIP_BTN (PAD_BUTTON_A|PAD_BUTTON_B)

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

#define LISTDATA_STR_LENGTH	(32)

//--------------------------------------------------------------
/// KEYCURSOR_WORK
//--------------------------------------------------------------
typedef struct
{
  u8 cursor_anm_no;
  u8 cursor_anm_frame;
  GFL_BMP_DATA *bmp_cursor;
}KEYCURSOR_WORK;

//======================================================================
//  キー送りカーソル
//======================================================================

// 送りカーソルデータ
static const u8 ALIGN4 skip_cursor_Character[128] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x22,0x22,0x22,0x22,
0x21,0x22,0x22,0x22,0x10,0x22,0x22,0x12, 0x00,0x21,0x22,0x11,0x00,0x10,0x12,0x01,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x00,
0x11,0x00,0x00,0x00,0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

//--------------------------------------------------------------
/**
 * キー送りカーソル 初期化
 * @param work KEYCURSOR_WORK
 * @param heapID HEAPID
 * @retval nothing
 */
//--------------------------------------------------------------
static void KeyCursor_Init( KEYCURSOR_WORK *work, HEAPID heapID )
{
  MI_CpuClear8( work, sizeof(KEYCURSOR_WORK) );
  work->bmp_cursor = GFL_BMP_CreateWithData(
        (u8*)skip_cursor_Character,
        2, 2, GFL_BMP_16_COLOR, heapID );
}

//--------------------------------------------------------------
/**
 * キー送りカーソル 削除
 * @param work KEYCURSOR_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void KeyCursor_Delete( KEYCURSOR_WORK *work )
{
  GFL_BMP_Delete( work->bmp_cursor );
}

//--------------------------------------------------------------
/**
 * キー送りカーソル クリア
 * @param work KEYCURSOR_WORK
 * @param bmp 表示先GFL_BMP_DATA
 * @param n_col 透明色指定 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
static void KeyCursor_Clear( KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - 11;
  y = GFL_BMP_GetSizeY( bmp ) - 9;
  offs = tbl[work->cursor_anm_no];
  GFL_BMP_Fill( bmp, x, y+offs, 10, 7, n_col );
//  HOSAKA_Printf("x=%d y=%d offs=%d \n",x,y,offs);
}

//--------------------------------------------------------------
/**
 * キー送りカーソル 表示
 * @param work KEYCURSOR_WORK
 * @param bmp 表示先GFL_BMP_DATA
 * @param n_col 透明色指定 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
static void KeyCursor_Write( KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  KeyCursor_Clear( work, bmp, n_col );
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= 4*2 ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= 3;
  }

  x = GFL_BMP_GetSizeX( bmp ) - 11;
  y = GFL_BMP_GetSizeY( bmp ) - 9;
  offs = tbl[work->cursor_anm_no];
  
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 2, x, y+offs, 10, 7, 0x00 );
//  HOSAKA_Printf("x=%d y=%d offs=%d \n",x,y,offs);
}

//-------------------------------------
///	リスト
//=====================================
typedef struct 
{
	BMPMENULIST_WORK	*p_list;
	BMP_MENULIST_DATA *p_list_data;
	u32		select;
} INTRO_LIST_WORK;

//--------------------------------------------------------------
///	メッセージ処理 メインワーク
//==============================================================
struct _INTRO_MSG_WORK {
  // [PRIVATE]
  HEAPID        heap_id;
  PRINT_QUE*    print_que;
  WORDSET*      wordset;
  GFL_MSGDATA*  msghandle;
  GFL_FONT*     font;

  BOOL          push_flag;

  // ストリーム再生
  PRINT_STREAM* print_stream;
  GFL_TCBLSYS*  msg_tcblsys;
  GFL_BMPWIN*   win_dispwin;
  
  // キーカーソル
  KEYCURSOR_WORK cursor_work;
  
  // リスト
  INTRO_LIST_WORK   list;
  PRINT_UTIL        print_util;
  GFL_BMPWIN*       win_list;

  // 文字列
  STRBUF*   strbuf;
  STRBUF*   exp_strbuf;
};

//=============================================================================
/**
 *							プロトタイプ宣言
 */
//=============================================================================
//static STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID );

//=============================================================================
/**
 *								外部公開関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  メッセージ管理モジュール 生成
 *
 *	@param	HEAPID heap_id ヒープID
 *
 *	@retval INTRO_MSG_WORK* wk メインワーク
 */
//-----------------------------------------------------------------------------
INTRO_MSG_WORK* INTRO_MSG_Create( HEAPID heap_id )
{
  INTRO_MSG_WORK* wk;

  // メインワーク アロケート
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INTRO_MSG_WORK) );
  
  // メンバ初期化
  wk->heap_id = heap_id;
  wk->wordset = WORDSET_Create( heap_id );
  // @TODO intro.cに既にある。統合する。
  wk->print_que = PRINTSYS_QUE_Create( heap_id );

  wk->msg_tcblsys = GFL_TCBL_Init( wk->heap_id, wk->heap_id, 1, 0 );

  // メッセージ用フォント転送
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0x20*PLTID_BG_TEXT_M, 0x20, heap_id );

  // フレームウィンドウ用のキャラを用意
  BmpWinFrame_GraphicSet( BG_FRAME_TEXT_M, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_WIN_M, MENU_TYPE_FIELD, heap_id );

  // フォントを展開
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_MEMORY, FALSE, wk->heap_id );

  wk->strbuf      = GFL_STR_CreateBuffer( STRBUF_SIZE, wk->heap_id );
  wk->exp_strbuf  = GFL_STR_CreateBuffer( STRBUF_SIZE, wk->heap_id );

  // ウィンドウ生成
  // @TODO リストを出す場所指定
  wk->win_list = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 22, 13, 9, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->win_list ), 0 ); // クリアしておく
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win_list );

  KeyCursor_Init( &wk->cursor_work, wk->heap_id );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  メッセージ管理モジュール 破棄
 *
 *	@param	INTRO_MSG_WORK* wk  メインワーク
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_Exit( INTRO_MSG_WORK* wk )
{
  KeyCursor_Delete( &wk->cursor_work );

  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_STR_DeleteBuffer( wk->exp_strbuf );
  
  GFL_FONT_Delete( wk->font );

  // まだ終了していない文字列表示が会った場合は破棄
  if( wk->print_stream )
  {
    PRINTSYS_PrintStreamDelete( wk->print_stream );
  }
  
  if( wk->win_dispwin )
  {
    GFL_BMPWIN_Delete( wk->win_dispwin );
  }
  
  if( wk->win_list )
  {
    GFL_BMPWIN_Delete( wk->win_list );
  }

  GFL_TCBL_Exit( wk->msg_tcblsys );

  PRINTSYS_QUE_Delete( wk->print_que );
  
  if( wk->msghandle )
  {
    GFL_MSG_Delete( wk->msghandle );
  }

  WORDSET_Delete( wk->wordset );

  // メインワーク 破棄
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  メッセージリソースをリロード
 *
 *	@param	INTRO_MSG_WORK* wk
 *	@param	type
 *	@param	msg_dat_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LoadGmm( INTRO_MSG_WORK* wk, GflMsgLoadType type, u16 msg_dat_id )
{
  if( wk->msghandle )
  {
    GFL_MSG_Delete( wk->msghandle );
  }
  wk->msghandle = GFL_MSG_Create( type, ARCID_MESSAGE, msg_dat_id, wk->heap_id );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  メッセージ管理モジュール 主処理
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_Main( INTRO_MSG_WORK* wk )
{
  GFL_TCBL_Main( wk->msg_tcblsys );
	PRINTSYS_QUE_Main( wk->print_que );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  メッセージプリント 開始
 *
 *	@param	INTRO_MSG_WORK* wk　メインワーク
 *	@param	str_id メッセージID
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
//void INTRO_MSG_SetPrint( INTRO_MSG_WORK* wk, int str_id, WORDSET_CALLBACK callback_func, void* callback_arg )
void INTRO_MSG_SetPrint( INTRO_MSG_WORK* wk, int str_id )
{
  const u8 clear_color = 15;
  GFL_BMPWIN* win;
  int msgspeed;

  // ウィンドウ生成
  if( wk->win_dispwin == NULL )
  {
    wk->win_dispwin = GFL_BMPWIN_Create( BG_FRAME_TEXT_M, 1, 19, 30, 4, PLTID_BG_TEXT_M, GFL_BMP_CHRAREA_GET_B );
  }
  
  msgspeed  = MSGSPEED_GetWait();
  win       = wk->win_dispwin;

//  HOSAKA_Printf("msgspeed = %d str_id=%d \n", msgspeed, str_id);

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win), clear_color);
  GFL_FONTSYS_SetColor(1, 2, clear_color);

  GFL_MSG_GetString( wk->msghandle, str_id, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->strbuf ); ///< 破損チェック
#endif

#if 0
  // コールバックでWORDSET
  if( callback_func )
  {
    callback_func( wk->wordset, callback_arg );
  }
#endif

  WORDSET_ExpandStr( wk->wordset, wk->exp_strbuf, wk->strbuf );

#ifdef PM_DEBUG
  GFL_STR_CheckBufferValid( wk->exp_strbuf ); ///< 破損チェック
#endif

  wk->print_stream = PRINTSYS_PrintStream( win, 4, 0, wk->exp_strbuf, wk->font, msgspeed,
                                           wk->msg_tcblsys, 0xffff, wk->heap_id, clear_color );

  BmpWinFrame_Write( win, WINDOW_TRANS_ON_V, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_M );

  // 転送
  GFL_BMPWIN_TransVramCharacter( win );
  GFL_BMPWIN_MakeScreen( win );

  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  イントロデモ プリントストリーム状態取得
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
PRINTSTREAM_STATE INTRO_MSG_GetPrintState( INTRO_MSG_WORK* wk )
{
  GF_ASSERT( wk );
 
  if( wk->print_stream == NULL )
  {
    // NULL状態でも値を返してしまうのはセキュリティ上好ましくないが、
    // intro_cmd.cのCMD_MCSS_SET_PEPACHI_WITH_PRINTで、
    // プリントコマンド終了後にこの関数から戻り値を受け取りたいので、やむを得ず。
    return PRINTSTREAM_STATE_DONE;
  }

  return PRINTSYS_PrintStreamGetState( wk->print_stream );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  メッセージプリント 終了待ち
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval TRUE:終了
 */
//-----------------------------------------------------------------------------
BOOL INTRO_MSG_PrintProc( INTRO_MSG_WORK* wk )
{
  PRINTSTREAM_STATE state;

//  HOSAKA_Printf("print state= %d \n", state );

  if( wk->print_stream )
  {
    state = PRINTSYS_PrintStreamGetState( wk->print_stream );
    switch( state )
    {
    case PRINTSTREAM_STATE_DONE : // 終了
      wk->push_flag = FALSE;
      // キーカーソルをクリア
//    KeyCursor_Clear( &wk->cursor_work, GFL_BMPWIN_GetBmp(wk->win_dispwin), 15 );   
//    GFL_BMPWIN_TransVramCharacter( wk->win_dispwin );

      PRINTSYS_PrintStreamDelete( wk->print_stream );
      wk->print_stream = NULL;
      return TRUE;

    case PRINTSTREAM_STATE_PAUSE : // 一時停止中
      // キー入力待ち
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE || (  GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL ) || GFL_UI_TP_GetTrg() )
      {
        wk->push_flag = TRUE;
        PRINTSYS_PrintStreamReleasePause( wk->print_stream );
        GFL_SOUND_PlaySE( SEQ_SE_DECIDE1 );
        KeyCursor_Clear( &wk->cursor_work, GFL_BMPWIN_GetBmp(wk->win_dispwin), 15 );
      }
      else
      {
        if( wk->push_flag == FALSE )
        {
          KeyCursor_Write( &wk->cursor_work, GFL_BMPWIN_GetBmp(wk->win_dispwin), 15 );
        }
      }

      GFL_BMPWIN_TransVramCharacter( wk->win_dispwin );
      break;

    case PRINTSTREAM_STATE_RUNNING :  // 実行中
      wk->push_flag = FALSE;
      // メッセージスキップ
      if( (GFL_UI_KEY_GetCont() & MSG_SKIP_BTN) || GFL_UI_TP_GetTrg() )
      {
        PRINTSYS_PrintStreamShortWait( wk->print_stream, 0 );
      }
      break;

    default :
      wk->push_flag = FALSE;
      break;
    }

    return FALSE;
  }

  return FALSE;
}





//=============================================================================
/**
 *								static関数
 */
//=============================================================================

#if 0
//----------------------------------------------------------------------------
/**
 *  @brief  文字列展開しつつMSGを取得
 *
 *  @param  WORDSET *wordset  ワードセット
 *  @param  *MsgManager       メッセージ
 *  @param  strID             文字ID
 *  @param  heapID            テンポラリヒープID
 *
 *  @return 文字列展開後のSTRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID )
{
  STRBUF  *src;
  STRBUF  *dst;
  src = GFL_MSG_CreateString( MsgManager, strID );
  dst = GFL_STR_CreateBuffer( GFL_STR_GetBufferLength( src ), heapID );

  WORDSET_ExpandStr( wordset, src, dst );

  GFL_STR_DeleteBuffer( src );

  return dst;
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	PRINT_UTILを設定し取得
 *
 *	@param	MSG_WORK *wk	ワーク
 *	@param	BMPWIN
 *
 *	@return	PRINT_UTIL
 */
//-----------------------------------------------------------------------------
static PRINT_UTIL * MSG_GetPrintUtil( INTRO_MSG_WORK *wk, GFL_BMPWIN*	p_bmpwin )
{	
	PRINT_UTIL_Setup( &wk->print_util, p_bmpwin );
	return &wk->print_util;
}

//=============================================================================
/**
 *					LIST
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム　初期化
 *
 *	@param	INTRO_LIST_WORK *wk	ワーク
 *	@param	*cp_tbl					設定テーブル
 *	@param	tbl_max					設定テーブル数
 *	@param  is_cansel       キャンセル
 *
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LIST_Start( INTRO_MSG_WORK* wk, const INTRO_LIST_DATA *cp_tbl, u32 tbl_max, BOOL is_cansel )
{	
  INTRO_LIST_WORK* list = &wk->list;
  HEAPID heapID         = wk->heap_id;
  GFL_BMPWIN* p_bmpwin  = wk->win_list;

	//クリア
	GFL_STD_MemClear( list, sizeof(INTRO_LIST_WORK));

	//LISTDATA create
	{	
		int i;

		list->p_list_data	= BmpMenuWork_ListCreate( tbl_max, heapID );

		for( i = 0; i < tbl_max; i++ )
		{	
      GFL_MSG_GetString( wk->msghandle, cp_tbl[i].str_id, wk->strbuf );

      BmpMenuWork_ListAddString( &list->p_list_data[i], wk->strbuf, cp_tbl[i].param, heapID );
		}
	}

	//LIST create
	{	
		BMPMENULIST_HEADER	header;

		GFL_STD_MemClear( &header, sizeof(BMPMENULIST_HEADER));
		header.list				= list->p_list_data;
		header.win				= p_bmpwin;
		header.count			= tbl_max;
		header.line				= 5;
		header.rabel_x		= 0;
		header.data_x			= 16;
		header.cursor_x		= 0;
		header.line_y			= 2;
		header.f_col			= 1;
		header.b_col			= 15;
		header.s_col			= 2;
		header.msg_spc		= 0;
		header.line_spc		= 0;
		header.page_skip	= BMPMENULIST_LRKEY_SKIP;
		header.font				= 0;
		header.c_disp_f		= 0;
		header.work				= NULL;
		header.font_size_x= 16;
		header.font_size_y= 16;
		header.msgdata		= NULL;
		header.print_util	= MSG_GetPrintUtil( wk, p_bmpwin );
		header.print_que	= wk->print_que;
		header.font_handle= wk->font;

		list->p_list	= BmpMenuList_Set( &header, 0, 0, heapID );
		BmpMenuList_SetCursorBmp( list->p_list, heapID );
	}

  GFL_BMPWIN_MakeTransWindow( p_bmpwin ); // スクリーン＆キャラ転送
  BmpWinFrame_Write( p_bmpwin, WINDOW_TRANS_ON_V, CGX_BMPWIN_FRAME_POS, PLTID_BG_TEXT_M ); // 周りにフレームを書く

  if( is_cansel )
  {
    BmpMenuList_SetCancelMode( list->p_list, BMPMENULIST_CANCELMODE_USE );
  }
  else
  {
    BmpMenuList_SetCancelMode( list->p_list, BMPMENULIST_CANCELMODE_NOT );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム	破棄
 *
 *	@param	INTRO_LIST_WORK *wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LIST_Finish( INTRO_MSG_WORK *wk )
{
  INTRO_LIST_WORK* list = &wk->list;

  BmpWinFrame_Clear( wk->win_list, WINDOW_TRANS_ON_V );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->win_list ), 0 ); // クリア
  GFL_BMPWIN_TransVramCharacter( wk->win_list ); // 転送

	BmpMenuList_Exit( list->p_list, NULL, NULL );
	BmpMenuWork_ListDelete( list->p_list_data );
	GFL_STD_MemClear( list, sizeof(INTRO_LIST_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リストシステム	メイン処理
 *
 *	@param	INTRO_LIST_WORK *wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
void INTRO_MSG_LIST_Main( INTRO_MSG_WORK *wk )
{	
  INTRO_LIST_WORK* list = &wk->list;

	list->select	= BmpMenuList_Main(	list->p_list );
}

//----------------------------------------------------------------------------
/**
 *	@brief	リスト決定待ち
 *
 *	@param	const INTRO_LIST_WORK *cwk	ワーク
 *	@param	*p_select								リストのインデックス
 *
 *	@retval	TRUEならば決定
 *	@retval	FALSEならば選択中
 */
//-----------------------------------------------------------------------------
INTRO_LIST_SELECT INTRO_MSG_LIST_IsDecide( INTRO_MSG_WORK *wk, u32 *p_select )
{	
  INTRO_LIST_WORK* list = &wk->list;

	switch( list->select)
  { 
  case BMPMENULIST_NULL:
    return INTRO_LIST_SELECT_NONE;

	case BMPMENULIST_CANCEL:
    return INTRO_LIST_SELECT_CANCEL;

  default:
		if( p_select )
		{	
			*p_select	= list->select;
		}
		return INTRO_LIST_SELECT_DECIDE;
  }

	return INTRO_LIST_SELECT_NONE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  WORDSETへのポインタを取得
 *
 *	@param	INTRO_MSG_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
WORDSET* INTRO_MSG_GetWordSet( INTRO_MSG_WORK* wk )
{
  return wk->wordset;
}
