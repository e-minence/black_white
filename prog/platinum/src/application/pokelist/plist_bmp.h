//============================================================================================
/**
 * @file	plist_bmp.h
 * @brief	ポケモンリスト画面BMP関連
 * @author	Hiroyuki Nakamura
 * @date	05.09.29
 */
//============================================================================================
#ifndef	PLIST_BMP_H
#define	PLIST_BMP_H
#undef	GLOBAL
#ifdef	PLIST_BMP_H_GLOBAL
#define	GLOBAL /*	*/
#else
#define	GLOBAL extern
#endif


//============================================================================================
//	定数定義
//============================================================================================
#define	PL_MSG_COMP_EXPAND	( 0xffffffff )	// 表示するメッセージがすでに展開済み


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMP追加
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListBmpAdd( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ破棄
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListBmpWinExit( PLIST_WORK * wk );


//--------------------------------------------------------------------------------------------
/**
 * 名前作成
 *
 * @param	wk		ポケモンリストのワーク
 * @param	pp		POKEMON_PARAM
 * @param	pos		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListNameMake( PLIST_WORK * wk, POKEMON_PARAM * pp, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * 名前表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListNamePut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * HP表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListHPPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * HP表示クリア
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListHPClear( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * HPゲージ表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListHPBerPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * レベル表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListLvPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * パラメータ全体を表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListParamPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * パラメータ全体のBMPのCGXを転送
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListParamBmpCgxOn( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * パラメータ全体を非表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListParamBmpWinOff( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * 進化チェックコメントを表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_PanelShinkaCommPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * 技覚えチェックコメントを表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_PanelWazaOboeCommPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * コンテスト参加チェックコメントを表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_PanelContestCommPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * 名前＆レベル表示（戦闘参加選択用）
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_PanelBattleInNamePut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * 戦闘参加チェックコメントを表示
 *
 * @param	wk		ポケモンリストのワーク
 * @param	num		何匹目か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_PanelBattleInCommPut( PLIST_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * メニュー項目文字列取得
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListMenuStrMake( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * メニューデータ作成
 *
 * @param	wk		ポケモンリストのワーク
 * @param	prm		作成データ
 * @param	siz		サイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListMenuDataMake( PLIST_WORK * wk, const u8 * prm, u8 siz );

//--------------------------------------------------------------------------------------------
/**
 * メニューメッセージ作成
 *
 * @param	wk		ポケモンリストのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListMenuMsgMake( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 技メニューの文字列取得
 *
 * @param	wk		ポケモンリストのワーク
 * @param	waza	技番号
 * @param	pos		位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_WazaMenuStrGet( PLIST_WORK * wk, u16 waza, u8 pos );


//--------------------------------------------------------------------------------------------
/**
 * 「けってい」「もどる」表示
 *
 * @param	wk		ポケモンリストワーク
 * @param	flg		表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListSelectMsgPut( PLIST_WORK * wk, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * １行メッセージ表示
 *
 * @param	wk		ポケモンリストワーク
 ` @param	id		メッセージID
 * @param	flg		ウィンドウ表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListSmallMsgPut( PLIST_WORK * wk, u32 id, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * ２行メッセージ表示
 *
 * @param	wk		ポケモンリストワーク
 * @param	id		メッセージID
 * @param	flg		ウィンドウ表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListMiddleMsgPut( PLIST_WORK * wk, u32 id, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * 会話（風）メッセージ表示
 *
 * @param	wk		ポケモンリストワーク
 * @param	id		メッセージID
 * @param	flg		ウィンドウ表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeListLargeMsgPut( PLIST_WORK * wk, u32 id, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * 会話（風）メッセージスタート
 *
 * @param	wk		ポケモンリストワーク
 * @param	id		メッセージID
 * @param	flg		ウィンドウ表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_TalkMsgStart( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえ表示
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_YesNoSelectInit( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * レベルアップウィンドウ表示
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_LvWinPut( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * レベルアップ後のパラメータ表示
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_LvUpParamPut( PLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * レベルアップウィンドウ削除
 *
 * @param	wk		ポケモンリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void PokeList_LvUpWinExit( PLIST_WORK * wk );



#undef GLOBAL
#endif
