//============================================================================================
/**
 * @file	box2_bmp.h
 * @brief	新ボックス画面 BMP関連
 * @author	Hiroyuki Nakamura
 * @date	08.05.13
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================
/*
// メニューウィンドウ４つ分のサイズ
#define	BOX2BMP_POKEMENU_SX		( 11 )
#define	BOX2BMP_POKEMENU_SY		( 3 )

//「てもちポケモン」サイズ
#define	BOX2BMP_BTN_TEMOCHI_SX	( 11 )
#define	BOX2BMP_BTN_TEMOCHI_SY	( 3 )
//「ポケモンいどう」サイズ
#define	BOX2BMP_BTN_IDOU_SX		( 11 )
#define	BOX2BMP_BTN_IDOU_SY		( 3 )
//「もどる」サイズ
#define	BOX2BMP_BTN_MODORU_SX	( 8 )
#define	BOX2BMP_BTN_MODORU_SY	( 3 )
*/
//「ポケモンをあずける」サイズ
#define	BOX2BMP_BOXMVMENU_SX	( 11 )
#define	BOX2BMP_BOXMVMENU_SY	( 6 )
/*
//「ボックスをきりかえる」サイズ
#define	BOX2BMP_BOXMVBTN_SX		( 17 )
#define	BOX2BMP_BOXMVBTN_SY		( 3 )
*/

// メニューボタンデータ
typedef struct {
	u16	strID;		// 文字列ＩＤ
	u16	type;		// ボタンタイプ
}BOX2BMP_BUTTON_LIST;

// ボタンタイプ
enum {
	BOX2BMP_BUTTON_TYPE_WHITE = 0,	// 白
	BOX2BMP_BUTTON_TYPE_RED,		// 赤
};

// 逃がすメッセージ表示ID
enum {
	BOX2BMP_MSGID_POKEFREE_CHECK = 0,	// ほんとうに　にがしますか？
	BOX2BMP_MSGID_POKEFREE_ENTER,		// @0を　そとに　にがしてあげた
	BOX2BMP_MSGID_POKEFREE_BY,			// ばいばい　@0！
	BOX2BMP_MSGID_POKEFREE_EGG,			// タマゴを　にがすことは　できません！
	BOX2BMP_MSGID_POKEFREE_RETURN,		// @0が　もどってきちゃった！
	BOX2BMP_MSGID_POKEFREE_FEAR,		// しんぱい　だったのかな……
	BOX2BMP_MSGID_POKEFREE_ONE,			// たたかうポケモンが　いなくなります！
};

// ボックステーマ変更メッセージ表示ID
enum {
	BOX2BMP_MSGID_THEMA_INIT = 0,		// ボックスを　どうする？
	BOX2BMP_MSGID_THEMA_WALL,			// どの　かべがみに　する？
};

// ポケモンをつれていくメッセージ表示ID
enum {
	BOX2BMP_MSGID_PARTYIN_INIT = 0,			// ポケモンを　タッチ！
	BOX2BMP_MSGID_PARTYIN_MENU,					// @01を　どうする？
	BOX2BMP_MSGID_PARTYIN_ERR,					// てもちが　いっぱいです！
	BOX2BMP_MSGID_PARTYOUT_INIT,				// どこへ　あずける？
	BOX2BMP_MSGID_PARTYOUT_BOXMAX,			// このボックスは　いっぱいだ！
	BOX2BMP_MSGID_PARTYOUT_MAIL,				// メールを　はずしてください！
//	BOX2BMP_MSGID_PARTYOUT_CAPSULE,		// ボールカプセルを　はずしてください！
};

// 道具整理メッセージ表示ＩＤ
enum {
	BOX2BMP_MSGID_ITEM_A_SET = 0,		// どうぐを　もたせます
	BOX2BMP_MSGID_ITEM_A_INIT,			// @0を　どうしますか？
	BOX2BMP_MSGID_ITEM_A_RET_CHECK,		// @0を　しまいますか？
	BOX2BMP_MSGID_ITEM_A_RET,			// @0を　バッグに　いれた！
	BOX2BMP_MSGID_ITEM_A_MAIL,			// メールをしまうことは　できません！
	BOX2BMP_MSGID_ITEM_A_EGG,			// タマゴは　どうぐを　もてません！
	BOX2BMP_MSGID_ITEM_A_MAX,			// バッグが　いっぱいです！
	BOX2BMP_MSGID_ITEM_A_MAIL_MOVE,		// メールは　いどうできません！
};

// VBLANKで表示するメッセージID
enum {
	BOX2BMP_MSGID_VBLANK_AZUKERU = 0,		// ボックスに　おけます！
	BOX2BMP_MSGID_VBLANK_TSURETEIKU,		// てもちに　くわえることが　できます！
	BOX2BMP_MSGID_VBLANK_AZUKERU_ERR,		// このボックスは　いっぱいです！
	BOX2BMP_MSGID_VBLANK_TSURETEIKU_ERR,	// てもちは　いっぱいです！
	BOX2BMP_MSGID_VBLANK_ITEM_SET,			// どうぐを　もたせます
	BOX2BMP_MSGID_VBLANK_ITEM_INIT,			// @0を　どうしますか？
	BOX2BMP_MSGID_VBLANK_MAIL_ERR,			// メールを　もっています！
	BOX2BMP_MSGID_VBLANK_CAPSULE_ERR,		// ボールカプセルを　もっています！
	BOX2BMP_MSGID_VBLANK_BATTLE_POKE_ERR,	// たたかうポケモンが　いなくなります！
	BOX2BMP_MSGID_VBLANK_MAIL_MOVE,			// メールは　いどうできません！
	BOX2BMP_MSGID_VBLANK_ITEM_MOVE,			// ポケモンに　セット！
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＭＰ関連初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_Init( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＭＰ関連削除
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_Exit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		プリントメイン
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PrintUtilTrans( BOX2_APP_WORK * appwk );






//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧウィンドウフレーム作成：各メニュー
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeMenuBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧウィンドウフレーム作成：「てもちポケモン」
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TemochiButtonBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧウィンドウフレーム作成：「ポケモンいどう」
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_IdouButtonBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧウィンドウフレーム作成：「もどる」「やめる」
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ModoruButtonBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧウィンドウフレーム作成：「とじる」
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ToziruButtonBgFrmWkMake( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンフレームにBMPWINをセット
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PartyCngButtonFrmPut( BOX2_APP_WORK * appwk );




//--------------------------------------------------------------------------------------------
/**
 * @brief		タイトル表示
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TitlePut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		デフォルト文字列表示
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_DefStrPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ名表示
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		cgx			背景キャラデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2BMP_TrayNamePut( BOX2_SYS_WORK * syswk, u8 * cgx );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「てもちポケモン」ボタン作成
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutTemochi( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「ポケモンいどう」ボタン作成（持ち物整理モードの場合は「もちものせいり」）
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutIdou( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		「もどる」ボタン作成
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutModoru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		マーキングフレームににBMPWINをセット
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingButtonFrmPut( BOX2_APP_WORK * appwk );



















//--------------------------------------------------------------------------------------------
/**
 * 上画面ポケモンデータ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	info	表示データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeDataPut( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info );

//--------------------------------------------------------------------------------------------
/**
 * 上画面ポケモンデータ非表示
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeDataOff( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * 上画面技表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	info	表示データ
 *
 * @return	表示したBMPWINのID
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2BMP_PokeSkillWrite( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info );

//--------------------------------------------------------------------------------------------
/**
 * 上画面アイテム情報表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	item	アイテム番号
 *
 * @return	表示したBMPWINのID
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2BMP_PokeItemInfoWrite( BOX2_SYS_WORK * syswk, u16 item );

//--------------------------------------------------------------------------------------------
/**
 * タイトル表示
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TitlePut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * デフォルト文字列表示
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_DefStrPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * トレイ名表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	cgx		背景キャラデータ
 * @param	chr		転送キャラ位置
 * @param	sx		Ｘサイズ
 * @param	sy		Ｙサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TrayNamePut( BOX2_SYS_WORK * syswk, u8 * cgx, u32 chr, u32 sx, u32 sy );

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動処理のボックス名・収納数表示
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxMoveNameNumWrite( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更処理の名前表示
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_WallPaperNameWrite( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモン」ボタン作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutTemochi( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスいどう」ボタン作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutIdou( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「もどる」ボタン作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutModoru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「やめる」ボタン作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutYameru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「とじる」ボタン作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutToziru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * メニュー作成
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	list	メニューリスト
 * @param	max		メニュー数
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MenuStrPrint( BOX2_SYS_WORK * syswk, const BOX2BMP_BUTTON_LIST * list, u32 max );

//--------------------------------------------------------------------------------------------
/**
 * 道具整理メニュー作成
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	item	アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemArrangeMenuStrPrint( BOX2_SYS_WORK * syswk, u16 item );

//--------------------------------------------------------------------------------------------
/**
 * メニュー非表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	max		メニュー数
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MenuVanish( BOX2_SYS_WORK * syswk, u32 max );

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをきりかえる」ボタン作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxMoveButtonPrint( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをきりかえる」作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxMoveMenuPrint( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマメニュー作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxThemaMenuPrint( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙メニュー作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_WallPaperChgMenuPrint( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンを預ける」メニュー作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PartyOutMenuPrint( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧウィンドウフレーム作成：各メニュー
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeMenuBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧウィンドウフレーム作成：「てもちポケモン」
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_TemochiButtonBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧウィンドウフレーム作成：「ポケモンいどう」
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_IdouButtonBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧウィンドウフレーム作成：「もどる」「やめる」
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ModoruButtonBgFrmWkMake( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧウィンドウフレーム作成：「とじる」
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ToziruButtonBgFrmWkMake( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * マーキングフレームのボタン描画
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingButtonPut( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレームに「いれかえ」描画
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PartyChangeButtonPut( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレームに「やめる」描画
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PartySelectButtonPut( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * マーキングフレームににBMPWINをセット
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingButtonFrmPut( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレームににBMPWINをセット
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PartyCngButtonFrmPut( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * Ｙステータスボタン作成
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ButtonPutYStatus( BOX2_SYS_WORK * syswk );


//============================================================================================
//	システムウィンドウ
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * システムウィンドウ非表示（ＶＢＬＡＮＫで転送）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_SysWinVanish( BOX2_APP_WORK * appwk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * システムウィンドウ非表示（転送なし）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_SysWinVanishEz( BOX2_APP_WORK * appwk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * システムウィンドウ非表示（即転送）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_SysWinVanishTrans( BOX2_APP_WORK * appwk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * ボックス終了メッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 * @param	flg		タッチ or キー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxEndMsgPut( BOX2_SYS_WORK * syswk, u32 winID, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * アイテム取得チェックメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	item	アイテム番号
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemGetCheckMsgPut( BOX2_SYS_WORK * syswk, u32 item, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * メール取得エラーメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MailGetErrorPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * メール移動エラーメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MailMoveErrorPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * アイテム取得メッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	item	アイテム番号
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemGetMsgPut( BOX2_SYS_WORK * syswk, u32 item, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * アイテム取得エラーメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemGetErrorPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * アイテムセットメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	item	アイテム番号
 * @param	winID	BMPWIN ID
 *
 * @return	none
 *
 *	item = 0 のときは、はっきんだまを持たせようとしたときのエラーとする
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemSetMsgPut( BOX2_SYS_WORK * syswk, u32 item, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * マーキングメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_MarkingMsgPut( BOX2_SYS_WORK * syswk, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * 逃がすメッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	msgID	メッセージＩＤ
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeFreeMsgPut( BOX2_SYS_WORK * syswk, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更メッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	msgID	メッセージＩＤ
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_BoxThemaMsgPut( BOX2_SYS_WORK * syswk, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをつれていく」メッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		ポケモンの位置
 * @param	msgID	メッセージＩＤ
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_PokeSelectMsgPut( BOX2_SYS_WORK * syswk, u32 pos, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * 道具整理メッセージ表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	msgID	メッセージＩＤ
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_ItemArrangeMsgPut( BOX2_SYS_WORK * syswk, u32 msgID, u32 winID );

//--------------------------------------------------------------------------------------------
/**
 * VBLANKで表示を実行するメッセージをセット
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 * @param	msgID	メッセージＩＤ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_VBlankMsgSet( BOX2_SYS_WORK * syswk, u32 winID, u32 msgID );

//--------------------------------------------------------------------------------------------
/**
 * VBLANKでメッセージを表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	winID	BMPWIN ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2BMP_VBlankMsgPut( BOX2_SYS_WORK * syswk, u32 winID );
