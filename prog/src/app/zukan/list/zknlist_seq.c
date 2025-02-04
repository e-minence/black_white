//============================================================================================
/**
 * @file		zknlist_seq.c
 * @brief		図鑑リスト画面 シーケンス処理
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	モジュール名：ZKNLISTSEQ
 */
//============================================================================================
#include <gflib.h>

#include "system/wipe.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "app/app_menu_common.h"

#include "zknlist_main.h"
#include "zknlist_seq.h"
#include "zknlist_obj.h"
#include "zknlist_bmp.h"
#include "zknlist_ui.h"
#include "zknlist_snd_def.h"


//============================================================================================
//	定数定義
//============================================================================================

// メインシーケンス
enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_WIPE,

	MAINSEQ_INIT_LIST_WAIT,
	MAINSEQ_MAIN,
	MAINSEQ_PAGE_MOVE,

	MAINSEQ_BUTTON_ANM,
	MAINSEQ_ITEM_ANM,

	MAINSEQ_END_SET,
	MAINSEQ_END,
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static int MainSeq_Init( ZKNLISTMAIN_WORK * wk );
static int MainSeq_Release( ZKNLISTMAIN_WORK * wk );
static int MainSeq_Wipe( ZKNLISTMAIN_WORK * wk );

static int MainSeq_InitListWait( ZKNLISTMAIN_WORK * wk );
static int MainSeq_Main( ZKNLISTMAIN_WORK * wk );
static int MainSeq_PageMove( ZKNLISTMAIN_WORK * wk );

static int MainSeq_ButtonAnm( ZKNLISTMAIN_WORK * wk );
static int MainSeq_ItemAnm( ZKNLISTMAIN_WORK * wk );
static int MainSeq_EndSet( ZKNLISTMAIN_WORK * wk );

static void SetInitPalFade( ZKNLISTMAIN_WORK * wk );
static int SetFadeIn( ZKNLISTMAIN_WORK * wk, int next );
static int SetFadeOut( ZKNLISTMAIN_WORK * wk, int next );
static int SetButtonAnm( ZKNLISTMAIN_WORK * wk, u32 id, u32 anm, int next );
static BOOL CheckInfoData( ZKNLISTMAIN_WORK * wk, int pos );
static void SetDefaultMons( ZKNLISTMAIN_WORK * wk, int pos );
static int SetInfoData( ZKNLISTMAIN_WORK * wk, int pos );
static void SetShortCut( ZKNLISTMAIN_WORK * wk );

FS_EXTERN_OVERLAY(ui_common);


//============================================================================================
//	グローバル
//============================================================================================

// メインシーケンス
static const pZKNLIST_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Wipe,

	MainSeq_InitListWait,
	MainSeq_Main,
	MainSeq_PageMove,

	MainSeq_ButtonAnm,
	MainSeq_ItemAnm,

	MainSeq_EndSet,
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス
 *
 * @param		wk		図鑑リストワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNLISTSEQ_MainSeq( ZKNLISTMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

	ZKNLISTOBJ_AnmMain( wk );
	ZKNLISTBMP_PrintUtilTrans( wk );
	ZKNCOMM_ScrollBaseBG( GFL_BG_FRAME3_M, GFL_BG_FRAME3_S, &wk->BaseScroll );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：初期化
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Init( ZKNLISTMAIN_WORK * wk )
{
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

	// 表示初期化
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// ブレンド初期化
	G2_BlendNone();
	G2S_BlendNone();
	// 輝度を最低にしておく
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );
	// サブ画面をメインに
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	ZKNLISTMAIN_InitPaletteFade( wk );

	ZKNLISTMAIN_InitVram();
	ZKNLISTMAIN_InitBg();
	ZKNLISTMAIN_LoadBgGraphic();
	ZKNLISTMAIN_InitMsg( wk );

	ZKNLISTBMP_Init( wk );
	ZKNLISTOBJ_Init( wk );

	ZKNLISTMAIN_LoadLocalNoList( wk );
	ZKNLISTMAIN_MakeList( wk );

	if( wk->dat->callMode == ZKNLIST_CALL_NORMAL ){
		ZKNLISTBMP_PutPokeEntryStr( wk );
	}else{
		ZKNLISTBMP_PutPokeSearchStr( wk );
	}

	GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_ZUKAN_LIST );

	ZKNLISTMAIN_SetBlendAlpha();
	ZKNLISTMAIN_SetWindow();

	ZKNLISTMAIN_InitFrameScroll( wk );

	ZKNLISTMAIN_InitVBlank( wk );
	ZKNLISTMAIN_InitHBlank( wk );

	return MAINSEQ_INIT_LIST_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：解放
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Release( ZKNLISTMAIN_WORK * wk )
{
	if( ZKNLISTMAIN_MainFrameScroll( wk ) == TRUE || PaletteFadeCheck(wk->pfd) != 0 ){
		return MAINSEQ_RELEASE;
	}

	ZKNLISTMAIN_ExitHBlank( wk );
	ZKNLISTMAIN_ExitVBlank( wk );

	ZKNLISTMAIN_ResetWindow();

	ZKNLISTMAIN_FreeList( wk );
	ZKNLISTMAIN_FreeLocalNoList( wk );

	ZKNLISTOBJ_Exit( wk );
	ZKNLISTBMP_Exit( wk );

	ZKNLISTMAIN_ExitMsg( wk );
	ZKNLISTMAIN_ExitBg();

	ZKNLISTMAIN_ExitPaletteFade( wk );

	// 輝度を最低にしておく
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );
	// ブレンド初期化
	G2_BlendNone();
	G2S_BlendNone();
	// 表示初期化
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

	return MAINSEQ_END;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：ワイプ待ち
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Wipe( ZKNLISTMAIN_WORK * wk )
{
	if( PaletteFadeCheck(wk->pfd) == 0 ){
		return wk->wipeSeq;
	}
	return MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：リスト初期化待ち
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_InitListWait( ZKNLISTMAIN_WORK * wk )
{
	switch( wk->listInit ){
	case 0:
		if( FRAMELIST_Init( wk->lwk ) == FALSE ){
			ZKNLISTOBJ_SetListPageArrowAnime( wk, FALSE );	// 矢印アニメ変更
			ZKNLISTOBJ_SetScrollBar( wk );
			ZKNLISTOBJ_SetPutPokeIconFlag( wk );
			SetInitPalFade( wk );
			wk->listInit++;
		}
		break;

	case 1:
		if( PaletteFadeCheck(wk->pfd) == 0 ){
			GX_SetMasterBrightness( 0 );
			GXS_SetMasterBrightness( 0 );
			ZKNLISTMAIN_SetPalFadeSeq( wk, 16, 0 );
			ZKNLISTMAIN_SetFrameScrollParam( wk, -ZKNCOMM_BAR_SPEED );
			wk->listInit++;
		}
		break;

	case 2:
		if( ZKNLISTMAIN_MainFrameScroll( wk ) == FALSE && PaletteFadeCheck(wk->pfd) == 0 ){
			wk->listInit = 0;
			return MAINSEQ_MAIN;
		}
	}

	return MAINSEQ_INIT_LIST_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：メイン
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_Main( ZKNLISTMAIN_WORK * wk )
{
	int	seq;
	u32	ret;

	seq = MAINSEQ_MAIN;
	ret = FRAMELIST_Main( wk->lwk );

	switch( ret ){
	case FRAMELIST_RET_CURSOR_ON:		// カーソル表示
		break;

	case FRAMELIST_RET_MOVE:				// カーソル移動
	case FRAMELIST_RET_SCROLL:			// スクロール通常
	case FRAMELIST_RET_RAIL:				// レールスクロール
	case FRAMELIST_RET_SLIDE:				// スライドスクロール
		ZKNLISTOBJ_SetListPageArrowAnime( wk, FALSE );
		break;

	case FRAMELIST_RET_PAGE_UP:			// １ページ上へ
		ZKNLISTOBJ_SetAutoAnm( wk, ZKNLISTOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON );
		wk->buttonID = ZKNLISTOBJ_IDX_TB_LEFT;
		wk->subSeq = 0;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_DOWN:		// １ページ下へ
		ZKNLISTOBJ_SetAutoAnm( wk, ZKNLISTOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON );
		wk->buttonID = ZKNLISTOBJ_IDX_TB_RIGHT;
		wk->subSeq = 0;
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_PAGE_UP_NONE:		// ページ数が足りなくて１ページ上にスクロールできなかった
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_LEFT, APP_COMMON_BARICON_CURSOR_LEFT_ON, MAINSEQ_MAIN );

	case FRAMELIST_RET_PAGE_DOWN_NONE:	// ページ数が足りなくて１ページ下にスクロールできなかった
		return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_RIGHT, APP_COMMON_BARICON_CURSOR_RIGHT_ON, MAINSEQ_MAIN );

	case FRAMELIST_RET_JUMP_TOP:		// リスト最上部へジャンプ
	case FRAMELIST_RET_JUMP_BOTTOM:	// リスト最下部へジャンプ
		ZKNLISTOBJ_VanishJumpPokeIcon( wk );
		wk->buttonID = ZKNLISTOBJ_IDX_MAX;
#ifdef	BUGFIX_BTS7796_20100714
		wk->subSeq = 1;
#else		// BUGFIX_BTS7796_20100714
		wk->subSeq = 0;
#endif	// BUGFIX_BTS7796_20100714
		seq = MAINSEQ_PAGE_MOVE;
		break;

	case FRAMELIST_RET_NONE:				// 動作なし
		{
			int	mode = GFL_UI_CheckTouchOrKey();

			ret = ZKNLISTUI_ListMain( wk );

			switch( ret ){
			case ZKNLISTUI_ID_POKE:				// 00: ポケモン正面絵
				{
					int	pos = FRAMELIST_GetListPos( wk->lwk );
					if( CheckInfoData( wk, pos ) == TRUE ){
						PMSND_PlaySE( ZKNLIST_SE_DECIDE );
						return SetInfoData( wk, pos );
					}else{
						GFL_UI_SetTouchOrKey( mode );
					}
				}
				break;

			case ZKNLISTUI_ID_ICON1:			// 01: ポケモンアイコン
			case ZKNLISTUI_ID_ICON2:			// 02: ポケモンアイコン
			case ZKNLISTUI_ID_ICON3:			// 03: ポケモンアイコン
			case ZKNLISTUI_ID_ICON4:			// 04: ポケモンアイコン
			case ZKNLISTUI_ID_ICON5:			// 05: ポケモンアイコン
			case ZKNLISTUI_ID_ICON6:			// 06: ポケモンアイコン
			case ZKNLISTUI_ID_ICON7:			// 07: ポケモンアイコン
				{
					int	pos = FRAMELIST_GetScrollCount( wk->lwk ) + ret - ZKNLISTUI_ID_ICON1;
					if( CheckInfoData( wk, pos ) == TRUE ){
						PMSND_PlaySE( ZKNLIST_SE_DECIDE );
						FRAMELIST_SetCursorPos( wk->lwk, ret-ZKNLISTUI_ID_ICON1 );
						return SetInfoData( wk, pos );
					}
				}
				break;

			case ZKNLISTUI_ID_START:			// 08: スタート
				PMSND_PlaySE( ZKNLIST_SE_DECIDE );
				wk->dat->retMode = ZKNLIST_RET_SEARCH;
				SetDefaultMons( wk, FRAMELIST_GetListPos(wk->lwk) );
				return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_START, ZKNLISTOBJ_ANM_START_ANM, MAINSEQ_END_SET );

			case ZKNLISTUI_ID_SELECT:			// 09: セレクト
				PMSND_PlaySE( ZKNLIST_SE_DECIDE );
				if( ZUKANSAVE_GetZukanMode( wk->dat->savedata ) == TRUE ){
					ZUKANSAVE_SetZukanMode( wk->dat->savedata, FALSE );
				}else{
					ZUKANSAVE_SetZukanMode( wk->dat->savedata, TRUE );
				}
				wk->dat->retMode = ZKNLIST_RET_MODE_CHANGE;
				SetDefaultMons( wk, FRAMELIST_GetListPos(wk->lwk) );
				return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_SELECT, ZKNLISTOBJ_ANM_SELECT_ANM, MAINSEQ_END_SET );

			case ZKNLISTUI_ID_Y:					// 10: Ｙ
				PMSND_PlaySE( ZKNLIST_SE_Y );
				SetShortCut( wk );
				break;

			case ZKNLISTUI_ID_X:					// 11: Ｘ
				PMSND_PlaySE( ZKNLIST_SE_CLOASE );
				wk->dat->retMode = ZKNLIST_RET_EXIT_X;
				SetDefaultMons( wk, FRAMELIST_GetListPos(wk->lwk) );
				return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_EXIT, APP_COMMON_BARICON_EXIT_ON, MAINSEQ_END_SET );

			case ZKNLISTUI_ID_RETURN:			// 12: 戻る
				PMSND_PlaySE( ZKNLIST_SE_CANCEL );
				wk->dat->retMode = ZKNLIST_RET_EXIT;
				SetDefaultMons( wk, FRAMELIST_GetListPos(wk->lwk) );
				return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON, MAINSEQ_END_SET );

			case ZKNLISTUI_ID_CANCEL:			// 13: キャンセルボタン
				PMSND_PlaySE( ZKNLIST_SE_CANCEL );
				wk->dat->retMode = ZKNLIST_RET_EXIT;
				SetDefaultMons( wk, FRAMELIST_GetListPos(wk->lwk) );
				return SetButtonAnm( wk, ZKNLISTOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_ON, MAINSEQ_END_SET );
			}
		}
		break;

	// 項目選択
	default:
		{
			int	pos = FRAMELIST_GetListPos( wk->lwk );
			ZKNLISTOBJ_SetListPageArrowAnime( wk, FALSE );
			if( CheckInfoData( wk, pos ) == TRUE ){
				PMSND_PlaySE( ZKNLIST_SE_DECIDE );
				return SetInfoData( wk, pos );
			}
		}
		break;
	}

	ZKNLISTOBJ_SetScrollBar( wk );

	ZKNLISTOBJ_SetPutPokeIconFlag( wk );

	return seq;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：ページ切り替え
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_PageMove( ZKNLISTMAIN_WORK * wk )
{
	switch( wk->subSeq ){
	case 0:
		if( FRAMELIST_Main( wk->lwk ) == FRAMELIST_RET_NONE ){
			wk->subSeq++;
		}
		break;

	case 1:
		if( wk->buttonID == ZKNLISTOBJ_IDX_MAX || ZKNLISTOBJ_CheckAnm( wk, wk->buttonID ) == FALSE ){
			wk->subSeq = 0;
			ZKNLISTOBJ_SetListPageArrowAnime( wk, FALSE );
			return MAINSEQ_MAIN;
		}
		break;
	}

	return MAINSEQ_PAGE_MOVE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：ボタンアニメ
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ButtonAnm( ZKNLISTMAIN_WORK * wk )
{
	if( ZKNLISTOBJ_CheckAnm( wk, wk->buttonID ) == FALSE ){
		ZKNLISTOBJ_SetListPageArrowAnime( wk, FALSE );
		return wk->nextSeq;
	}	
	return MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：リスト項目アニメ
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_ItemAnm( ZKNLISTMAIN_WORK * wk )
{
	switch( wk->buttonSeq ){
	case 0:
	case 2:
		if( wk->buttonCnt == 0 ){
			FRAMELIST_ChangePosPalette( wk->lwk, FRAMELIST_GetCursorPos(wk->lwk), 1 );
			wk->buttonCnt = 4;
			wk->buttonSeq++;
		}else{
			wk->buttonCnt--;
		}
		break;

	case 1:
	case 3:
		if( wk->buttonCnt == 0 ){
			FRAMELIST_ChangePosPalette( wk->lwk, FRAMELIST_GetCursorPos(wk->lwk), 2 );
			GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
			wk->buttonCnt = 4;
			wk->buttonSeq++;
		}else{
			wk->buttonCnt--;
		}
		break;

	case 4:
		if( wk->buttonCnt == 0 ){
			wk->buttonSeq = 0;
			return MAINSEQ_END_SET;
		}else{
			wk->buttonCnt--;
		}
	}

	return MAINSEQ_ITEM_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：終了設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndSet( ZKNLISTMAIN_WORK * wk )
{
	ZKNLISTMAIN_SetPalFadeSeq( wk, 0, 16 );
	ZKNLISTMAIN_SetFrameScrollParam( wk, ZKNCOMM_BAR_SPEED );
	return MAINSEQ_RELEASE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		初期パレットフェード設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetInitPalFade( ZKNLISTMAIN_WORK * wk )
{
	PaletteWorkSet_VramCopy( wk->pfd, FADE_MAIN_BG, 0, FADE_PAL_ALL_SIZE );
	PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_BG, 0, FADE_PAL_ALL_SIZE );
	PaletteWorkSet_VramCopy( wk->pfd, FADE_MAIN_OBJ, 0, FADE_PAL_ALL_SIZE );
	PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 0, FADE_PAL_ALL_SIZE );

	ZKNLISTMAIN_SetPalFadeSeq( wk, 16, 16 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フェードインセット
 *
 * @param		wk		図鑑リストワーク
 * @param		next	フェード後のシーケンス
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int SetFadeIn( ZKNLISTMAIN_WORK * wk, int next )
{
	ZKNLISTMAIN_SetPalFadeSeq( wk, 16, 0 );
	wk->wipeSeq = next;
	return MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フェードアウトセット
 *
 * @param		wk		図鑑リストワーク
 * @param		next	フェード後のシーケンス
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int SetFadeOut( ZKNLISTMAIN_WORK * wk, int next )
{
	ZKNLISTMAIN_SetPalFadeSeq( wk, 0, 16 );
	wk->wipeSeq = next;
	return MAINSEQ_WIPE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンアニメセット
 *
 * @param		wk		図鑑リストワーク
 * @param		id		ボタンＩＤ
 * @param		anm		アニメ番号
 * @param		next	フェード後のシーケンス
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int SetButtonAnm( ZKNLISTMAIN_WORK * wk, u32 id, u32 anm, int next )
{
	ZKNLISTOBJ_SetAutoAnm( wk, id, anm );
	wk->buttonID = id;
	wk->nextSeq  = next;
	return MAINSEQ_BUTTON_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト項目チェック
 *
 * @param		wk		図鑑リストワーク
 * @param		pos		項目位置
 *
 * @retval	"TRUE = 項目あり"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckInfoData( ZKNLISTMAIN_WORK * wk, int pos )
{
	if( GET_LIST_INFO( FRAMELIST_GetItemParam(wk->lwk,pos) ) == 0 ){
		return FALSE;
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デフォルトポケモン設定
 *
 * @param		wk		図鑑リストワーク
 * @param		pos		項目位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetDefaultMons( ZKNLISTMAIN_WORK * wk, int pos )
{
	if( CheckInfoData( wk, pos ) == TRUE ){
		ZUKANSAVE_SetDefaultMons( wk->dat->savedata, FRAMELIST_GetItemParam(wk->lwk,pos) );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目作成
 *
 * @param		wk		図鑑リストワーク
 * @param		pos		項目位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static int SetInfoData( ZKNLISTMAIN_WORK * wk, int pos )
{
	SetDefaultMons( wk, pos );
	wk->dat->retMode = ZKNLIST_RET_INFO;
	return MAINSEQ_ITEM_ANM;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ショートカット設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetShortCut( ZKNLISTMAIN_WORK * wk )
{
	if( GAMEDATA_GetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_LIST ) == TRUE ){
		GAMEDATA_SetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_LIST, FALSE );
		ZKNLISTOBJ_SetAutoAnm( wk, ZKNLISTOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_OFF );
	}else{
		GAMEDATA_SetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_LIST, TRUE );
		ZKNLISTOBJ_SetAutoAnm( wk, ZKNLISTOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_ON );
	}
}

