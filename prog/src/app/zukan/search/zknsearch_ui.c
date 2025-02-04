//============================================================================================
/**
 * @file		zknsearch_ui.c
 * @brief		図鑑検索画面 ＵＩ関連
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	モジュール名：ZKNSEARCHUI
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "ui/touchbar.h"

#include "zknsearch_main.h"
#include "zknsearch_ui.h"


//============================================================================================
//	定数定義
//============================================================================================
// タッチバー上のボタンデータ
#define	TB_SX			( 24 )
#define	TB_SY			( 24 )
#define	TB_PY			( TOUCHBAR_ICON_Y )
#define	TB_PX07		( TOUCHBAR_ICON_X_07 )
#define	TB_PX06		( TB_PX07 - TB_SX )
#define	TB_PX05		( TB_PX06 - TB_SX )

// カーソル表示するキー
#define	PAD_MAIN_CURSOR_ON	(PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_BUTTON_DECIDE)


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void ChangePosMenuMainKey( ZKNSEARCHMAIN_WORK * wk, u32 pos );

static void MainCallBack_On( void * work, int now_pos, int old_pos );
static void MainCallBack_Off( void * work, int now_pos, int old_pos );
static void MainCallBack_Move( void * work, int now_pos, int old_pos );
static void MainCallBack_Touch( void * work, int now_pos, int old_pos );


//============================================================================================
//	グローバル
//============================================================================================

// メインページのカーソル移動データ
static const CURSORMOVE_DATA MainCursorMoveData[] =
{
	{ 16,  0, 16, 3,	5, 1, 0, 0,	{   0,  23, 128, 255 } },		// 00: 並び
	{ 16,  3, 16, 3,	0, 2, 1, 1,	{  24,  47, 128, 255 } },		// 01: 名前
	{ 16,  6, 16, 3,	1, 3, 2, 2,	{  48,  71, 128, 255 } },		// 02: タイプ
	{ 16,  9, 16, 3,	2, 4, 3, 3,	{  72,  95, 128, 255 } },		// 03: 色
	{ 16, 12, 16, 5,	3, 5, 4, 4,	{  96, 135, 128, 255 } },		// 04: 形
	{ 14, 18, 18, 3,	4, 0, 6, 6,	{ 144, 167, 113, 255 } },		// 05: スタート
	{  0, 18,  7, 3,	4, 0, 5, 5,	{ 144, 167,   0,  50 } },		// 06: リセット

	{ 0, 0, 0, 0,	7, 7, 7, 7, { TB_PY, TB_PY+TB_SY-1, TB_PX05, TB_PX05+TB_SX-1 } },		// 07: Ｙ
	{ 0, 0, 0, 0,	8, 8, 8, 8, { TB_PY, TB_PY+TB_SY-1, TB_PX06, TB_PX06+TB_SX-1 } },		// 08: Ｘ
	{ 0, 0, 0, 0,	9, 9, 9, 9, { TB_PY, TB_PY+TB_SY-1, TB_PX07, TB_PX07+TB_SX-1 } },		// 09: 戻る

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
// メインページのカーソル移動コールバック
static const CURSORMOVE_CALLBACK MainCallBack = {
	MainCallBack_On,
	MainCallBack_Off,
	MainCallBack_Move,
	MainCallBack_Touch,
};

// リストページタッチ座標
static const GFL_UI_TP_HITTBL ListTouchHitTbl[] =
{
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 },		// 00: 戻る

	{   0,  23, 128, 143 },		// 01: チェックボックス
	{  24,  47, 128, 143 },		// 02: チェックボックス
	{  48,  71, 128, 143 },		// 03: チェックボックス
	{  72,  95, 128, 143 },		// 04: チェックボックス
	{  96, 119, 128, 143 },		// 05: チェックボックス
	{ 120, 143, 128, 143 },		// 06: チェックボックス
	{ 144, 167, 128, 143 },		// 07: チェックボックス

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};

// リストページタッチ座標（フォルム用）
static const GFL_UI_TP_HITTBL FormListTouchHitTbl[] =
{
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 },		// 00: 戻る

	{   0,  39, 128, 143 },		// 01: チェックボックス
	{  40,  79, 128, 143 },		// 02: チェックボックス
	{  80, 119, 128, 143 },		// 03: チェックボックス
	{ 120, 159, 128, 143 },		// 04: チェックボックス

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		メインページのカーソル移動処理初期化
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHUI_MainCursorMoveInit( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	BOOL	flg;

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		wk->cmwk = CURSORMOVE_Create(
								MainCursorMoveData, &MainCallBack, wk, FALSE, pos, HEAPID_ZUKAN_SEARCH );
	}else{
		wk->cmwk = CURSORMOVE_Create(
								MainCursorMoveData, &MainCallBack, wk, TRUE, pos, HEAPID_ZUKAN_SEARCH );
		ZKNSEARCHUI_ChangeCursorPalette( wk, pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
	}
	CURSORMOVE_VanishModeSet( wk->cmwk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインページのカーソル移動処理解放
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHUI_CursorMoveExit( ZKNSEARCHMAIN_WORK * wk )
{
	if( wk->cmwk != NULL ){
		CURSORMOVE_Exit( wk->cmwk );
		wk->cmwk = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目のパレット変更
 *
 * @param		wk		図鑑検索画面ワーク
 * @param		pos		位置
 * @param		pal		パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHUI_ChangeCursorPalette( ZKNSEARCHMAIN_WORK * wk, u32 pos, u32 pal )
{
	if( pos <= ZKNSEARCHUI_RESET ){
		const CURSORMOVE_DATA * dat = CURSORMOVE_GetMoveData( wk->cmwk, pos );
		GFL_BG_ChangeScreenPalette( GFL_BG_FRAME0_M, dat->px, dat->py, dat->sx, dat->sy, pal );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインページのカーソル移動処理
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	入力結果
 */
//--------------------------------------------------------------------------------------------
u32 ZKNSEARCHUI_MenuMain( ZKNSEARCHMAIN_WORK * wk )
{
	u32	ret = CURSORMOVE_MainCont( wk->cmwk );

	if( ret != CURSORMOVE_NONE ){
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_X;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_Y;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		ChangePosMenuMainKey( wk, ZKNSEARCHUI_START );
		return ZKNSEARCHUI_START;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		ChangePosMenuMainKey( wk, ZKNSEARCHUI_RESET );
		return ZKNSEARCHUI_RESET;
	}

	if( GFL_UI_KEY_GetTrg() ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
			return CURSORMOVE_CURSOR_ON;
		}
	}	

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置変更
 *
 * @param		wk		図鑑検索画面ワーク
 * @param		pos		カーソル位置移動先
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangePosMenuMainKey( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	ZKNSEARCHUI_ChangeCursorPalette( wk, CURSORMOVE_PosGet(wk->cmwk), ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
	ZKNSEARCHUI_ChangeCursorPalette( wk, pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );

	CURSORMOVE_PosSet( wk->cmwk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：カーソル表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		now_pos		現在のカーソル位置
 * @param		old_pos		前回のカーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_On( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk;
	int	trg;
	
	wk  = work;
	trg = GFL_UI_KEY_GetTrg();

	// キャンセル時はカーソルをONにしない
	if( ( trg & PAD_BUTTON_CANCEL ) && !( trg & PAD_MAIN_CURSOR_ON ) ){
		return;
	}

	ZKNSEARCHUI_ChangeCursorPalette( wk, now_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
	BLINKPALANM_InitAnimeCount( wk->blink );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：カーソル非表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		now_pos		現在のカーソル位置
 * @param		old_pos		前回のカーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_Off( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk;

	wk  = work;

	ZKNSEARCHUI_ChangeCursorPalette( wk, old_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
	ZKNSEARCHUI_ChangeCursorPalette( wk, now_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：カーソル移動
 *
 * @param		work			図鑑検索画面ワーク
 * @param		now_pos		現在のカーソル位置
 * @param		old_pos		前回のカーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_Move( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHUI_ChangeCursorPalette( wk, old_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
	ZKNSEARCHUI_ChangeCursorPalette( wk, now_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
	BLINKPALANM_InitAnimeCount( wk->blink );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：タッチ
 *
 * @param		work			図鑑検索画面ワーク
 * @param		now_pos		現在のカーソル位置
 * @param		old_pos		前回のカーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHUI_ChangeCursorPalette( wk, old_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );

	if( now_pos == ZKNSEARCHUI_Y ){
		CURSORMOVE_PosSet( wk->cmwk, old_pos );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		検索結果表示後の入力待ち
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @retval	"TRUE = 入力された"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNSEARCHUI_Result( ZKNSEARCHMAIN_WORK * wk )
{
	if( wk->loadingCnt == 90 ){
		wk->loadingCnt = 0;
		return TRUE;
	}
	wk->loadingCnt++;

	if( GFL_UI_TP_GetTrg() ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		wk->loadingCnt = 0;
		return TRUE;
	}

	if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		wk->loadingCnt = 0;
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト入力処理
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	入力結果
 */
//--------------------------------------------------------------------------------------------
int ZKNSEARCHUI_ListMain( ZKNSEARCHMAIN_WORK * wk )
{
	int	ret;

	ret = GFL_UI_TP_HitTrg( ListTouchHitTbl );

	if( ret != GFL_UI_TP_HIT_NONE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			FRAMELIST_SetCursorPosPalette( wk->lwk );
		}
		return ZKNSEARCHUI_LIST_RESET;
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト入力処理（フォルム用）
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	入力結果
 */
//--------------------------------------------------------------------------------------------
int ZKNSEARCHUI_FormListMain( ZKNSEARCHMAIN_WORK * wk )
{
	int	ret;

	ret = GFL_UI_TP_HitTrg( FormListTouchHitTbl );

	if( ret != GFL_UI_TP_HIT_NONE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			FRAMELIST_SetCursorPosPalette( wk->lwk );
		}
		return ZKNSEARCHUI_LIST_RESET;
	}

	return ret;
}
