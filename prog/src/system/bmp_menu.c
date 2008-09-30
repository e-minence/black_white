//======================================================================
/**
 * @file	bmp_menu.c
 * @brief	BMPメニュー処理
 * @author	Hiroyuki Nakamura
 * @date	2004.11.10
 *
 *	・メニューがスクロールするならBMPLISTを使用すること
 *	・縦、横のメニューに対応
 *	・2008.09 DPからWBへ移植
 */
//======================================================================
#include <gflib.h>
#include <textprint.h>

#include "include/system/gfl_use.h"
#include "arc/arc_def.h"
#include "system/bmp_winframe.h"
#include "system/bmp_cursor.h"
#include "system/bmp_menu.h"

//======================================================================
//	シンボル定義
//======================================================================
enum {
	MV_UP = 0,
	MV_DOWN,
	MV_LEFT,
	MV_RIGHT
};

struct _BMPMENU_WORK {
	BMPMENU_HEADER	hed;
	BMPCURSOR *cursor;
	
	u32	cancel;
	
	u8	index;
	u8	cur_pos;
	u8	len;
	u8	px;
	
	u8	py;
	u8	sx;
	u8	sy;
	u8	mv;
	
	u8	heap_id;	// メモリ取得モード
	u8  dmy[3];		// ビット余り
};

//======================================================================
//	プロトタイプ宣言
//======================================================================
#if 0
static void BmpMenuWinWriteMain(
	GF_BGL_INI * ini, u8 frm, u8 px, u8 py, u8 sx, u8 sy, u8 pal, u16 cgx );
static void BmpTalkWinWriteMain(
	GF_BGL_INI * ini, u8 frm, u8 px, u8 py, u8 sx, u8 sy, u8 pal, u16 cgx );
#endif

static BOOL BmpMenuCursorMove( BMPMENU_WORK * mw, u8 mv, u16 se );
static u8 BmpMenuCursorMoveCheck( BMPMENU_WORK * mw, u8 mv );
static u8 BmpMenuStrLen( BMPMENU_WORK * buf );
static void BmpMenuStrPut( BMPMENU_WORK * mw );
static void BmpMenuCursorPut( BMPMENU_WORK * mw );
static void CursorWritePosGet( BMPMENU_WORK * mw, u8 * x, u8 * y, u8 pos );

#if 0
static void BmpWinOn( GFL_BMPWIN *bmpwin );
#endif

//======================================================================
//	BMPメニュー
//======================================================================
//--------------------------------------------------------------
/**
 * BMPメニュー登録（スクリーン転送しない）
 * @param	dat			ヘッダデータ
 * @param	px			項目表示X座標
 * @param	py			項目表示Y座標
 * @param	pos			初期カーソル位置
 * @aram	heap_id		メモリ取得モード
 * @param	cancel		キャンセルボタン
 * @return	BMPメニューワーク
 * @li	BMPリストワークはsys_AllocMemoryで確保
 */
//--------------------------------------------------------------
BMPMENU_WORK * BmpMenu_AddNoTrans( const BMPMENU_HEADER *dat,
		u8 px, u8 py, u8 pos, u8 heap_id, u32 cancel )
{
	BMPMENU_WORK *mw;
	
	mw = (BMPMENU_WORK *)GFL_HEAP_AllocClearMemory(
			heap_id, sizeof(BMPMENU_WORK) );
	
	mw->hed		= *dat;
	mw->cursor  = BmpCursor_Create( heap_id );
	mw->cancel  = cancel;
	mw->cur_pos	= pos;
	mw->len		= BmpMenuStrLen( mw );
	mw->heap_id = heap_id;
	
	mw->px = px;
	mw->py = py;
	
#if 0
	mw->sx = FontHeaderGet( dat->font, FONT_HEADER_SIZE_X )
				+ FontHeaderGet( dat->font, FONT_HEADER_SPACE_X );
	mw->sy = FontHeaderGet( dat->font, FONT_HEADER_SIZE_Y )
				+ FontHeaderGet( dat->font, FONT_HEADER_SPACE_Y );
#else
	mw->sx = mw->hed.font_size_x;
	mw->sy = mw->hed.font_size_y;
#endif
	
	BmpMenuStrPut( mw );
	BmpMenuCursorPut( mw );
	
	return mw;
}

//--------------------------------------------------------------
/**
 * BMPメニュー登録（キャンセルボタン指定）
 *
 * @param	dat			ヘッダデータ
 * @param	px			項目表示X座標
 * @param	py			項目表示Y座標
 * @param	pos			初期カーソル位置
 * @aram	mode		メモリ取得モード
 * @param	cancel		キャンセルボタン
 *
 * @return	BMPメニューワーク
 *
 * @li	BMPリストワークはsys_AllocMemoryで確保
 */
//--------------------------------------------------------------
BMPMENU_WORK * BmpMenu_AddEx( const BMPMENU_HEADER * dat,
		u8 px, u8 py, u8 pos, u8 heap_id, u32 cancel )
{
	BMPMENU_WORK *mw;
	mw = BmpMenu_AddNoTrans( dat, px, py, pos, heap_id, cancel );
//	BmpWinOn( mw->hed.win );
	return mw;
}

//--------------------------------------------------------------
/**
 * BMPメニュー登録（簡易版）
 *
 * @param	dat			ヘッダデータ
 * @param	pos			初期カーソル位置
 * @aram	mode		メモリ取得モード
 *
 * @return	BMPメニューワーク
 *
 * @li	BMPリストワークはsys_AllocMemoryで確保
 * @li	Bボタンキャンセル
 */
//--------------------------------------------------------------
BMPMENU_WORK * BmpMenu_Add( const BMPMENU_HEADER * dat, u8 pos, u8 heap_id )
{
#if 0
	return BmpMenu_AddEx(
		dat, FontHeaderGet(dat->font,FONT_HEADER_SIZE_X), 0,
		pos, mode, PAD_BUTTON_CANCEL );
#else
	return BmpMenu_AddEx(
		dat, dat->font_size_x, 0, pos, heap_id, PAD_BUTTON_CANCEL );
#endif
}

//--------------------------------------------------------------
/**
 * BMPメニュー破棄
 *
 * @param	mw		BMPメニューワーク
 * @param	backup	カーソル位置保存場所
 *
 * @return	none
 */
//--------------------------------------------------------------
void BmpMenu_Exit( BMPMENU_WORK *mw, u8 * backup )
{
	if( backup != NULL ){
		*backup = mw->cur_pos;
	}
	
	BmpCursor_Delete( mw->cursor );
	GFL_HEAP_FreeMemory( mw );
}

//--------------------------------------------------------------
/**
 * メニュー動作関数
 *
 * @param	mw		BMPメニューワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------
u32 BmpMenu_Main( BMPMENU_WORK * mw )
{
	int trg = GFL_UI_KEY_GetTrg();
	mw->mv = BMPMENU_MOVE_NONE;

	if( trg & PAD_BUTTON_DECIDE ){
//		Snd_SePlay( SE_DECIDE );
		return mw->hed.menu[ mw->cur_pos ].param;
	}

	if( trg & mw->cancel ){
//		Snd_SePlay( SE_CANCEL );
		return BMPMENU_CANCEL;
	}
	if( trg & PAD_KEY_UP ){
//		if( BmpMenuCursorMove( mw, MV_UP, SEQ_SE_DP_SELECT ) == TRUE ){
		if( BmpMenuCursorMove( mw, MV_UP, 0 ) == TRUE ){
			mw->mv = BMPMENU_MOVE_UP;
		}
		return BMPMENU_NULL;
	}
	if( trg & PAD_KEY_DOWN ){
//		if( BmpMenuCursorMove( mw, MV_DOWN, SEQ_SE_DP_SELECT ) == TRUE ){
		if( BmpMenuCursorMove( mw, MV_DOWN, 0 ) == TRUE ){
			mw->mv = BMPMENU_MOVE_DOWN;
		}
		return BMPMENU_NULL;
	}
	if( trg & PAD_KEY_LEFT ){
//		if( BmpMenuCursorMove( mw, MV_LEFT, SEQ_SE_DP_SELECT ) == TRUE ){
		if( BmpMenuCursorMove( mw, MV_LEFT, 0 ) == TRUE ){
			mw->mv = BMPMENU_MOVE_LEFT;
		}
		return BMPMENU_NULL;
	}
	if( trg & PAD_KEY_RIGHT ){
//		if( BmpMenuCursorMove( mw, MV_RIGHT, SEQ_SE_DP_SELECT ) == TRUE ){
		if( BmpMenuCursorMove( mw, MV_RIGHT, 0 ) == TRUE ){
			mw->mv = BMPMENU_MOVE_RIGHT;
		}
		return BMPMENU_NULL;
	}

	return BMPMENU_NULL;
}

//--------------------------------------------------------------
/**
 * メニュー動作関数（十字キーのSEを指定できる）
 *
 * @param	mw		BMPメニューワーク
 * @param	key_se	十字キーのSE
 *
 * @return	動作結果
 */
//--------------------------------------------------------------
u32 BmpMenu_MainSE( BMPMENU_WORK * mw, u16 key_se )
{
	int trg = GFL_UI_KEY_GetTrg();
	mw->mv = BMPMENU_MOVE_NONE;

	if( trg & PAD_BUTTON_DECIDE ){
//		Snd_SePlay( SE_DECIDE );
		return mw->hed.menu[ mw->cur_pos ].param;
	}
	if( trg & mw->cancel ){
//		Snd_SePlay( SE_CANCEL );
		return BMPMENU_CANCEL;
	}
	if( trg & PAD_KEY_UP ){
		if( BmpMenuCursorMove( mw, MV_UP, key_se ) == TRUE ){
			mw->mv = BMPMENU_MOVE_UP;
		}
		return BMPMENU_NULL;
	}
	if( trg & PAD_KEY_DOWN ){
		if( BmpMenuCursorMove( mw, MV_DOWN, key_se ) == TRUE ){
			mw->mv = BMPMENU_MOVE_DOWN;
		}
		return BMPMENU_NULL;
	}
	if( trg & PAD_KEY_LEFT ){
		if( BmpMenuCursorMove( mw, MV_LEFT, key_se ) == TRUE ){
			mw->mv = BMPMENU_MOVE_LEFT;
		}
		return BMPMENU_NULL;
	}
	if( trg & PAD_KEY_RIGHT ){
		if( BmpMenuCursorMove( mw, MV_RIGHT, key_se ) == TRUE ){
			mw->mv = BMPMENU_MOVE_RIGHT;
		}
		return BMPMENU_NULL;
	}

	return BMPMENU_NULL;
}

//--------------------------------------------------------------
/**
 * メニュー外部コントロール
 *
 * @param	mw		BMPメニューワーク
 * @param	prm		コントロールパラメータ
 *
 * @return	動作結果
 */
//--------------------------------------------------------------
u32 BmpMenu_MainOutControl( BMPMENU_WORK * mw, u8 prm )
{
	switch( prm ){
	case BMPMENU_CNTROL_DECIDE:		// 決定
//		Snd_SePlay( SE_DECIDE );
		return mw->hed.menu[ mw->cur_pos ].param;

	case BMPMENU_CNTROL_CANCEL:		// キャンセル
//		Snd_SePlay( SE_CANCEL );
		return BMPMENU_CANCEL;

	case BMPMENU_CNTROL_UP:			// 上
//		BmpMenuCursorMove( mw, MV_UP, SEQ_SE_DP_SELECT );
		BmpMenuCursorMove( mw, MV_UP, 0 );
		return BMPMENU_NULL;

	case BMPMENU_CNTROL_DOWN:		// 下
//		BmpMenuCursorMove( mw, MV_DOWN, SEQ_SE_DP_SELECT );
		BmpMenuCursorMove( mw, MV_DOWN, 0 );
		return BMPMENU_NULL;

	case BMPMENU_CNTROL_LEFT:		// 左
//		BmpMenuCursorMove( mw, MV_LEFT, SEQ_SE_DP_SELECT );
		BmpMenuCursorMove( mw, MV_LEFT, 0 );
		return BMPMENU_NULL;

	case BMPMENU_CNTROL_RIGHT:		// 右
//		BmpMenuCursorMove( mw, MV_RIGHT, SEQ_SE_DP_SELECT );
		BmpMenuCursorMove( mw, MV_RIGHT, 0 );
		return BMPMENU_NULL;
	}

	return BMPMENU_NULL;
}

//--------------------------------------------------------------
/**
 * カーソル位置取得
 *
 * @param	mw		BMPメニューワーク
 *
 * @return	カーソル位置
 */
//--------------------------------------------------------------
u8 BmpMenu_CursorPosGet( BMPMENU_WORK * mw )
{
	return mw->cur_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * 移動方向取得
 *
 * @param	mw		BMPメニューワーク
 *
 * @return	移動方向取得
 */
//--------------------------------------------------------------------------------------------
u8 BmpMenu_MoveSiteGet( BMPMENU_WORK * mw )
{
	return mw->mv;
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動
 *
 * @param	mw		BMPメニューワーク
 * @param	mv		移動方向
 * @param	se		SE
 *
 * @retval	"TRUE = 移動した"
 * @retval	"FALSE = 移動してない"
 */
//--------------------------------------------------------------------------------------------
static BOOL BmpMenuCursorMove( BMPMENU_WORK * mw, u8 mv, u16 se )
{
	u8	old = mw->cur_pos;

	if( BmpMenuCursorMoveCheck( mw, mv ) == FALSE ){
		return FALSE;
	}

	{
		u8	px, py;
		u8	col;
#if 0	//old dp
		col = FontHeaderGet( mw->hed.font, FONT_HEADER_B_COLOR );
#else
		{
			u8 l,s;
			GFL_FONTSYS_GetColor( &l, &s, &col );
		}
#endif
		CursorWritePosGet( mw, &px, &py, old );
		
#if 0	//old dp
		GF_BGL_BmpWinFill( mw->hed.win, col, px, py, 8, mw->sy );
#else
//		GFL_BMP_Fill(
//			GFL_BMPWIN_GetBmp(mw->hed.win), px, py, mw->sx, mw->sy, col );
		GFL_BMP_Fill(
			GFL_BMPWIN_GetBmp(mw->hed.win), px, py, mw->sx, mw->sy, 0xff );
#endif
	}
	
	BmpMenuCursorPut( mw );
//	Snd_SePlay( se );
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動チェック
 *
 * @param	mw		BMPメニューワーク
 * @param	mv		移動方向
 *
 * @retval	"TRUE = 移動した"
 * @retval	"FALSE = 移動してない"
 */
//--------------------------------------------------------------------------------------------
static u8 BmpMenuCursorMoveCheck( BMPMENU_WORK * mw, u8 mv )
{
	s8	new_pos;

	if( mv == MV_UP ){
		if( mw->hed.y_max <= 1 ){ return FALSE; }
		if( ( mw->cur_pos % mw->hed.y_max ) == 0 ){
			if( mw->hed.loop_f == 0 ){ return FALSE; }
//			mw->cur_pos += ( mw->hed.y_max - 1 );
			new_pos = mw->cur_pos + ( mw->hed.y_max - 1 );
		}else{
//			mw->cur_pos -= 1;
//			return TRUE;
			new_pos = mw->cur_pos - 1;
		}
	}else if( mv == MV_DOWN ){
		if( mw->hed.y_max <= 1 ){ return FALSE; }
		if( ( mw->cur_pos % mw->hed.y_max ) == ( mw->hed.y_max - 1 ) ){
			if( mw->hed.loop_f == 0 ){ return FALSE; }
//			mw->cur_pos -= ( mw->hed.y_max - 1 );
			new_pos = mw->cur_pos - ( mw->hed.y_max - 1 );
		}else{
//			mw->cur_pos += 1;
//			return TRUE;
			new_pos = mw->cur_pos + 1;
		}
	}else if( mv == MV_LEFT ){
		if( mw->hed.x_max <= 1 ){ return FALSE; }
		if( mw->cur_pos < mw->hed.y_max ){
			if( mw->hed.loop_f == 0 ){ return FALSE; }
//			mw->cur_pos += ( mw->hed.y_max * ( mw->hed.x_max - 1 ) );
			new_pos = mw->cur_pos + ( mw->hed.y_max * ( mw->hed.x_max - 1 ) );
		}else{
//			mw->cur_pos -= mw->hed.y_max;
//			return TRUE;
			new_pos = mw->cur_pos - mw->hed.y_max;
		}
	}else{
		if( mw->hed.x_max <= 1 ){ return FALSE; }
		if( mw->cur_pos >= ( mw->hed.y_max * ( mw->hed.x_max - 1 ) ) ){
			if( mw->hed.loop_f == 0 ){ return FALSE; }
//			mw->cur_pos %= mw->hed.y_max;
			new_pos = mw->cur_pos % mw->hed.y_max;
		}else{
//			mw->cur_pos += mw->hed.y_max;
//			return TRUE;
			new_pos = mw->cur_pos + mw->hed.y_max;
		}
	}

	if( mw->hed.menu[ new_pos ].param == BMPMENU_DUMMY ){
		return FALSE;
	}
	mw->cur_pos = new_pos;
	return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * 最長項目の文字数を取得
 *
 * @param	buf		BMPメニューワーク
 *
 * @return	最長項目の文字数
 */
//--------------------------------------------------------------------------------------------
static u8 BmpMenuStrLen( BMPMENU_WORK * buf )
{
	u8	len = 0;
	u8	i, j;
	
	for( i=0; i<buf->hed.x_max*buf->hed.y_max; i++ ){
#if 0	//old_dp
		j = FontProc_GetPrintStrWidth(
			buf->hed.font, buf->hed.menu[i].str, 0 );
#else
		j = PRINTSYS_GetStrWidth(
			buf->hed.menu[i].str, buf->hed.font_handle, 0 );
#endif
		if( len < j ){ len = j; }
	}
	
	return len;
}

//--------------------------------------------------------------------------------------------
/**
 * メニュー項目表示
 *
 * @param	mw		BMPメニューワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpMenuStrPut( BMPMENU_WORK * mw )
{
	const void* str;
	u8	px, py, plus;
	u8	i, j;

#if 0	//old dp
	GF_BGL_BmpWinDataFill(
		mw->hed.win, FontHeaderGet( mw->hed.font, FONT_HEADER_B_COLOR ) );

	px = mw->px;
//	plus = px * ( mw->len + 2 );
	plus = mw->len + mw->sx * 2;
	for( i=0; i<mw->hed.x_max; i++ ){
		for( j=0; j<mw->hed.y_max; j++ ){
			str = mw->hed.menu[i*mw->hed.y_max+j].str;
			py  = ( mw->sy + mw->hed.line_spc ) * j + mw->py;
			GF_STR_PrintSimple( mw->hed.win, mw->hed.font, str, px, py, MSG_NO_PUT, NULL );
		}
		px += plus;
	}
#else
	{
//		u8 l,s,b;
//		GFL_FONTSYS_GetColor( &l, &s, &b );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(mw->hed.win), 0xff );
	}
	
	px = mw->px;
//	plus = px * ( mw->len + 2 );
	plus = mw->len + mw->sx * 2;
	
	for( i=0; i<mw->hed.x_max; i++ ){
		for( j=0; j<mw->hed.y_max; j++ ){
			str = mw->hed.menu[i*mw->hed.y_max+j].str;
			py  = ( mw->sy + mw->hed.line_spc ) * j + mw->py;
			PRINT_UTIL_Print(
					mw->hed.print_util, mw->hed.print_que,
					px, py, str, mw->hed.font_handle );
		}
		px += plus;
	}
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル表示
 *
 * @param	mw		BMPメニューワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BmpMenuCursorPut( BMPMENU_WORK * mw )
{
	u8	px, py;

	if( mw->hed.c_disp_f == 1 ){ return; }

	CursorWritePosGet( mw, &px, &py, mw->cur_pos );
	BmpCursor_Print( mw->cursor, px, py,
		mw->hed.print_util, mw->hed.print_que, mw->hed.font_handle );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル表示位置取得
 *
 * @param	buf		BMPメニューデータ
 * @param	x		表示X座標取得場所
 * @param	y		表示Y座標取得場所
 * @param	pos		カーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorWritePosGet( BMPMENU_WORK * mw, u8 * x, u8 * y, u8 pos )
{
//	*x = ( pos / mw->hed.y_max ) * ( mw->len + 2 ) * mw->sx;
	*x = ( pos / mw->hed.y_max ) * ( mw->len + mw->sx * 2 );
	*y = ( pos % mw->hed.y_max ) * ( mw->sy + mw->hed.line_spc ) + mw->py;
}

#if 0
//============================================================================================
//	はい・いいえ処理
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * はい・いいえウィンドウセット（カーソル位置指定）
 *
 * @param	ini		BGLデータ
 * @param	data	ウィンドウデータ
 * @param	cgx		ウィンドウキャラ位置
 * @param	pal		ウィンドウパレット番号
 * @param	pos		初期カーソル位置
 * @param	heap	ヒープID
 *
 * @return	BMPメニューワーク
 *
 * @li	BMPウィンドウとBMPメニューワークをAllocで取得している
 */
//--------------------------------------------------------------------------------------------
BMPMENU_WORK * BmpMenu_YesNoSelectInitEx(
	const BMPWIN_DAT *data, u16 cgx, u8 pal, u8 pos, u32 heap )
{
	BMPMENU_HEADER hed;
	MSGDATA_MANAGER * man;
	BMP_MENULIST_DATA * ld;
	
	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_ev_win_dat, heap );
	ld  = BMP_MENULIST_Create( 2, heap );
	BMP_MENULIST_AddArchiveString( ld, man, msg_ev_win_046, 0 );
	BMP_MENULIST_AddArchiveString( ld, man, msg_ev_win_047, BMPMENU_CANCEL );
	MSGMAN_Delete( man );

	hed.menu     = ld;
	hed.win      = GF_BGL_BmpWinAllocGet( heap, 1 );
	hed.font     = FONT_SYSTEM;
	hed.x_max    = 1;
	hed.y_max    = 2;
	hed.line_spc = 0;
	hed.c_disp_f = 0;
	hed.loop_f = 0;

	GF_BGL_BmpWinAddEx( ini, hed.win, data );
	BmpMenuWinWrite( hed.win, WINDOW_TRANS_OFF, cgx, pal );
	return BmpMenuAddEx( &hed, 8, 0, pos, heap, PAD_BUTTON_CANCEL );
}

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえウィンドウセット
 *
 * @param	ini		BGLデータ
 * @param	data	ウィンドウデータ
 * @param	cgx		ウィンドウキャラ位置
 * @param	pal		ウィンドウパレット番号
 * @param	heap	ヒープID
 *
 * @return	BMPメニューワーク
 *
 * @li	BMPウィンドウとBMPメニューワークをAllocで取得している
 */
//--------------------------------------------------------------------------------------------
BMPMENU_WORK * BmpYesNoSelectInit(
					GF_BGL_INI * ini, const BMPWIN_DAT * data, u16 cgx, u8 pal, u32 heap )
{
/*
	BMPMENU_HEADER hed;
	MSGDATA_MANAGER * man;
	BMP_MENULIST_DATA * ld;

	man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_ev_win_dat, heap );
	ld  = BMP_MENULIST_Create( 2, heap );
	BMP_MENULIST_AddArchiveString( ld, man, msg_ev_win_046, 0 );
	BMP_MENULIST_AddArchiveString( ld, man, msg_ev_win_047, BMPMENU_CANCEL );
	MSGMAN_Delete( man );

	hed.menu     = ld;
	hed.win      = GF_BGL_BmpWinAllocGet( heap, 1 );
	hed.font     = FONT_SYSTEM;
	hed.x_max    = 1;
	hed.y_max    = 2;
	hed.line_spc = 0;
	hed.c_disp_f = 0;
	hed.line_spc = 0;

	GF_BGL_BmpWinAddEx( ini, hed.win, data );
	BmpMenuWinWrite( hed.win, WINDOW_TRANS_OFF, cgx, pal );
	return BmpMenuAddEx( &hed, 8, 0, 0, heap, PAD_BUTTON_CANCEL );
*/
	return BmpYesNoSelectInitEx( ini, data, cgx, pal, 0, heap );
}

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえ選択ウィンドウの制御
 *
 * @param	ini		BGLデータ
 * @param	heap	ヒープID
 *
 * @retval	"BMPMENU_NULL	選択されていない"
 * @retval	"0				はいを選択"
 * @retval	"BMPMENU_CANCEL	いいえorキャンセル"
 */
//--------------------------------------------------------------------------------------------
u32 BmpYesNoSelectMain( BMPMENU_WORK * mw, u32 heap )
{
	u32	ret = BmpMenuMain( mw );

	if( ret != BMPMENU_NULL ){
		BmpYesNoWinDel( mw, heap );
	}
	return	ret;
}

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえ選択ウィンドウ外部コントロール
 *
 * @param	ini		BGLデータ
 * @param	prm		コントロールパラメータ
 * @param	heap	ヒープID
 *
 * @retval	"BMPMENU_NULL	選択されていない"
 * @retval	"0				はいを選択"
 * @retval	"BMPMENU_CANCEL	いいえorキャンセル"
 */
//--------------------------------------------------------------------------------------------
u32 BmpYesNoSelectMainOutControl( BMPMENU_WORK * mw, u8 prm, u32 heap )
{
	u32	ret = BmpMenuMainOutControl( mw, prm );

	if( ret != BMPMENU_NULL ){
		BmpYesNoWinDel( mw, heap );
	}
	return	ret;
}

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえウィンドウ削除
 *
 * @param	ini		BGLデータ
 * @param	heap	ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BmpYesNoWinDel( BMPMENU_WORK * mw, u32 heap )
{
	BmpMenuWinClear( mw->hed.win, WINDOW_TRANS_ON );
	GF_BGL_BmpWinDel( mw->hed.win );
	sys_FreeMemory( heap, mw->hed.win );
	BMP_MENULIST_Delete( (BMP_MENULIST_DATA *)mw->hed.menu );
	BmpMenuExit( mw, NULL );
}


//------------------------------------------------------------------
/**
 * ウィンドウにカーソル画像を描画
 *
 * @param   win		ウィンドウ
 * @param   x		描画Ｘ座標（ドット）
 * @param   y		描画Ｙ座標（ドット）
 *
 */
//------------------------------------------------------------------
void BmpWin_DrawCursorImage(GF_BGL_BMPWIN* win, u32 x, u32 y)
{
	static const u8 CursorBitmapImage[] = {
		0xff,0xff,0xff,0x00,
		0xff,0xff,0xff,0x00,
		0x21,0xff,0xff,0x00,
		0x11,0xf2,0xff,0x00,
		0x11,0x21,0xff,0x00,
		0x11,0x11,0xf2,0x00,
		0x11,0x11,0x21,0x00,
		0x11,0x11,0x22,0x00,

		0x11,0x21,0xf2,0x00,
		0x11,0x22,0xff,0x00,
		0x21,0xf2,0xff,0x00,
		0x22,0xff,0xff,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,
	};

	GF_BGL_BmpWinPrint( win, (void*)CursorBitmapImage, 0, 0, 8, 16, x, y, 8, 16 );

}
#endif

//======================================================================
//
//======================================================================
//--------------------------------------------------------------
//	bitmap on
//	GF_BGL_BmpWinOn()
//--------------------------------------------------------------
#if 0
static void BmpWinOn( GFL_BMPWIN *bmpwin )
{
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(bmpwin) );
	GFL_BMPWIN_TransVramCharacter( bmpwin );
}
#endif

//--------------------------------------------------------------
/**
 * カーソル文字列セット
 * @param	mw	BMPMENU_WORK
 * @param	strID	文字列ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void BmpMenu_SetCursorString( BMPMENU_WORK *mw, u32 strID )
{
	BmpCursor_SetCursorFontMsg( mw->cursor, mw->hed.msgdata, strID );
	BmpMenuCursorPut( mw );
}
