//============================================================================================
/**
 * @file		box2_seq.h
 * @brief		ボックス画面 メインシーケンス処理
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	モジュール名：BOX2SEQ
 */
//============================================================================================
#pragma	once

#include "box2_main.h"


//============================================================================================
//	定数定義
//============================================================================================

// メインシーケンス
enum {
	BOX2SEQ_MAINSEQ_INIT = 0,		// 初期化
	BOX2SEQ_MAINSEQ_RELEASE,		// 解放
	BOX2SEQ_MAINSEQ_WIPE,			// ワイプ処理待ち
	BOX2SEQ_MAINSEQ_PALETTE_FADE,	// パレットフェード
	BOX2SEQ_MAINSEQ_WAIT,			// ウェイト
	BOX2SEQ_MAINSEQ_VFUNC,			// VBLANC処理中

	BOX2SEQ_MAINSEQ_TRGWAIT,		// キー/タッチ待ち

	BOX2SEQ_MAINSEQ_YESNO,			// はい・いいえ
	BOX2SEQ_MAINSEQ_BUTTON_ANM,		// ボタンアニメ

	BOX2SEQ_MAINSEQ_SUB_PROCCALL,	// サブプロセス呼び出し
	BOX2SEQ_MAINSEQ_SUB_PROCMAIN,	// サブプロセスメイン

	BOX2SEQ_MAINSEQ_START,			// 開始

	BOX2SEQ_MAINSEQ_ARRANGE_MAIN,					// ボックス整理・メイン
	BOX2SEQ_MAINSEQ_ARRANGE_TRAYPOKE_CHG_END,		// ボックス整理・トレイのポケモン移動後
	BOX2SEQ_MAINSEQ_ARRANGE_POKEMENU_RCV,			// ボックス整理・ポケモンメニューに戻る
	BOX2SEQ_MAINSEQ_ARRANGE_STATUS_RCV,				// ボックス整理・ステータス画面からの復帰
	BOX2SEQ_MAINSEQ_ARRANGE_STRIN_RCV,				// ボックス整理・文字入力画面からの復帰
	BOX2SEQ_MAINSEQ_ARRANGE_BAG_RCV,				// ボックス整理・バッグ画面からの復帰
	BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_MSG,			// ボックス整理・道具をもたせたメッセージ表示
	BOX2SEQ_MAINSEQ_ARRANGE_MARKING_INIT,			// ボックス整理・マーキング設定
	BOX2SEQ_MAINSEQ_ARRANGE_MARKING_FRMIN,			// ボックス整理・マーキングフレームイン
	BOX2SEQ_MAINSEQ_ARRANGE_MARKING_START,			// ボックス整理・マーキング処理開始
	BOX2SEQ_MAINSEQ_ARRANGE_MARKING,				// ボックス整理・マーキング処理メイン
	BOX2SEQ_MAINSEQ_ARRANGE_MARKING_RCV,			// ボックス整理・マーキングから復帰
	BOX2SEQ_MAINSEQ_MARKING_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_START,			// ボックス整理・ポケモンを逃がす開始
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ANM,			// ボックス整理・ポケモンを逃がすアニメ
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ENTER1,		// ボックス整理・ポケモンを逃がす
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ENTER2,		// ボックス整理・ポケモンを逃がす
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ENTER3,		// ボックス整理・ポケモンを逃がす
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR1,		// ボックス整理・ポケモンを逃がす
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR2,		// ボックス整理・ポケモンを逃がす
	BOX2SEQ_MAINSEQ_ARRANGE_POKEFREE_ERROR3,		// ボックス整理・ポケモンを逃がす
	BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_FREE_END,		// ボックス整理・手持ちポケモン逃がした後
	BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_FRMIN,		// ボックス整理・手持ちフレームイン
	BOX2SEQ_MAINSEQ_ARRANGE_PARTY_START,
	BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN,
	BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_FRMOUT,		// ボックス整理・手持ちフレームアウト
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_BUTTON_IN,		// ボックス整理・移動/手持ちボタンイン
	BOX2SEQ_MAINSEQ_ARRANGE_PARTYPOKE_CHG_END,		// ボックス整理・手持ちポケモン入れ替え後
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_START,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN,		// ボックス整理・手持ち・トレイ入れ替えメイン
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTYPOKE_CHG_END,	// ボックス整理・手持ち・トレイ入れ替え後
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTYPOKE_CHG_END_KEY,	// ボックス整理・手持ち・トレイ入れ替え後（キー取得）
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_ERROR,
	BOX2SEQ_MAINSEQ_ARRANGE_PARTYFRM_MOVE_R_END,	// ボックス整理・手持ち・手持ちフレーム右移動後
	BOX2SEQ_MAINSEQ_ARRANGE_PARTYFRM_MOVE_L_END,	// ボックス整理・手持ち・手持ちフレーム左移動後
	BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMIN,			// ボックス整理・ボックス移動フレームイン
	BOX2SEQ_MAINSEQ_ARRANGE_EXIT_BUTTON_FRMIN,		// ボックス整理・やめるフレームイン
	BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMOUT_SET,		// ボックス整理・ボックス移動フレームアウト設定
	BOX2SEQ_MAINSEQ_ARRANGE_BOXMOVE_FRMOUT,			// ボックス整理・ボックス移動フレームアウト
	BOX2SEQ_MAINSEQ_ARRANGE_BOX_RET_BUTTON_IN,		// ボックス整理・ボックス/もどるボタンイン
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_BUTTON_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_TRAY_IN,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_EXIT_IN,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MENU_IN,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MSG_SET,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_EXIT_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_MENU_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_TRAY_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_BUTTON_IN,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_MAIN,		// ボックス整理・ボックステーマ変更メイン
	BOX2SEQ_MAINSEQ_ARRANGE_BOXTHEMA_CHG_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_FRM_IN,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_MENU_IN,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_START,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_MAIN,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_MENU_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_FRM_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_END,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHANGE,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_WHITE_OUT,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_WHITE_IN,
	BOX2SEQ_MAINSEQ_WALLPAPER_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ARRANGE_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ARRANGE_BOXPARTY_CHG_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ARRANGE_PARTY_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_BOXARRANGE_POKE_MENU_RCV,
	BOX2SEQ_MAINSEQ_BOXARRANGE_PARTY_MENU_RCV,
	BOX2SEQ_MAINSEQ_ARRANGE_ITEMSET_EGG_ERROR,		// ボックス整理：タマゴにアイテムを持たせられないメッセージ表示
	BOX2SEQ_MAINSEQ_ARRANGE_ITEMGET_MAIL_ERROR,		// ボックス整理：メールを預かれないメッセージ表示

	BOX2SEQ_MAINSEQ_PARTYIN_MAIN,					// ポケモンをつれていく・メイン
	BOX2SEQ_MAINSEQ_PARTYIN_MAIN_RCV,				// ポケモンをつれていく・メインに復帰
	BOX2SEQ_MAINSEQ_PARTYIN_PARTY_FRM_IN,
	BOX2SEQ_MAINSEQ_PARTYIN_POKEMOVE_SET,
	BOX2SEQ_MAINSEQ_PARTYIN_PARTY_FRM_OUT,
	BOX2SEQ_MAINSEQ_PARTYIN_RET_FRM_IN,
	// つれていく仕様変更　7/28
	BOX2SEQ_MAINSEQ_PARTYIN_POKECHG_END,
	BOX2SEQ_MAINSEQ_PARTYIN_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_PARTYIN_MENU_CLOSE_END,
	BOX2SEQ_MAINSEQ_PARTYIN_POKE_MENU_RCV,

	BOX2SEQ_MAINSEQ_PARTYOUT_MAIN,
	BOX2SEQ_MAINSEQ_PARTYOUT_START_CHECK,
	BOX2SEQ_MAINSEQ_PARTYOUT_START_ERROR,
	BOX2SEQ_MAINSEQ_PARTYOUT_START_RET_FRM_IN,
	BOX2SEQ_MAINSEQ_PARTYOUT_MENUFRM_IN,
	BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_START,
	BOX2SEQ_MAINSEQ_PARTYOUT_BOXSELECT_MAIN,
	BOX2SEQ_MAINSEQ_PARTYOUT_BOXMENU_FRM_OUT,
	BOX2SEQ_MAINSEQ_PARTYOUT_END_RET_FRM_OUT,
	BOX2SEQ_MAINSEQ_PARTYOUT_BOXMOVE_FRM_OUT,
	BOX2SEQ_MAINSEQ_PARTYOUT_END_RET_FRM_IN,
	BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_BUTTON,
	BOX2SEQ_MAINSEQ_PARTYOUT_CHECK,
	BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_PARTYFRM_RIGHT,
	BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_TRAY_SCROLL,
	BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_POKE_MOVE,
	BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_PARTYFRM_LEFT,
	BOX2SEQ_MAINSEQ_PARTYOUT_ENTER_RET_IN,
	BOX2SEQ_MAINSEQ_PARTYOUT_COMP,
	// あずける仕様変更　7/28
	BOX2SEQ_MAINSEQ_PARTYOUT_POKECHG_END,
	BOX2SEQ_MAINSEQ_PARTYOUT_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_PARTYOUT_MENU_CLOSE_END,
	BOX2SEQ_MAINSEQ_PARTYOUT_BOXSEL_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_PARTYOUT_POKE_MENU_RCV,

	BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN,
	BOX2SEQ_MAINSEQ_KEYGET_POKEMOVE_MAIN_CURMOVE_RET,

	BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_SET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_CHECK,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_ENTER,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_CANCEL,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_RET_BAG_END,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_GET_BAG_CHECK,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_ITEMSET_MSG,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_RET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_POKEADD,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_CHANGE,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_ICONMOVE_CHANGE_END,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_SUBFRM_OUT,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_POKEADD,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_ICONMOVE_END,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOXPARTY_CHG_ERROR,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_MOVE_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_KEYGET_CHANGE_CHECK,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_MAIN_CURMOVE_RET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_RET,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_POKEADD_END,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_CHANGE,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_CHANGE_END,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_ICONMOVE_ERR,

	// ボタン処理追加
	BOX2SEQ_MAINSEQ_BOXEND_INIT,
	BOX2SEQ_MAINSEQ_BOXEND_CANCEL_INIT,
	BOX2SEQ_MAINSEQ_ARRANGE_PARTY_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_MOVE_BUTTON,
	BOX2SEQ_MAINSEQ_STATUS_BUTTON,
	BOX2SEQ_MAINSEQ_ITEM_BUTTON,
	BOX2SEQ_MAINSEQ_MARKING_BUTTON,
	BOX2SEQ_MAINSEQ_MARKING_END_BUTTON,
	BOX2SEQ_MAINSEQ_FREE_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_CLOSE_BUTTON,
	BOX2SEQ_MAINSEQ_PARTY_CHG_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_PARTY_CLOSE_BUTTON,
	BOX2SEQ_MAINSEQ_PARTY_END_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_BOX_MOVE_BUTTON,
	BOX2SEQ_MAINSEQ_BOX_MOVE_END_BUTTON,
	BOX2SEQ_MAINSEQ_BOXTHEMA_TRAYCHG_BUTTON,
	BOX2SEQ_MAINSEQ_BOXTHEMA_WALLPAPER_BUTTON,
	BOX2SEQ_MAINSEQ_BOXTHEMA_NAME_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_WALLPAPER_CHG_CANCEL_BUTTON,
	BOX2SEQ_MAINSEQ_PARTYIN_BUTTON,
	BOX2SEQ_MAINSEQ_PARTYIN_CLOSE_BUTTON,
	BOX2SEQ_MAINSEQ_PARTYOUT_BUTTON,
	BOX2SEQ_MAINSEQ_PARTYOUT_CLOSE_BUTTON,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_BUTTON,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_MOVE_BUTTON,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BAG_BUTTON,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_CLOSE_BUTTON,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_BOX_MOVE_BUTTON,
	BOX2SEQ_MAINSEQ_ITEM_CHG_BUTTON,
	BOX2SEQ_MAINSEQ_ITEMARRANGE_PARTY_CLOSE_BUTTON,
	BOX2SEQ_MAINSEQ_ARRANGE_MOVE_MENU_BUTTON,

	BOX2SEQ_MAINSEQ_END
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス制御
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		seq			メインシーケンス
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = 終了"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2SEQ_Main( BOX2_SYS_WORK * syswk, int * seq );
