//============================================================================================
/**
 * @file	b_plist_bmp.h
 * @brief	戦闘用ポケモンリスト画面BMP関連
 * @author	Hiroyuki Nakamura
 * @date	05.02.01
 */
//============================================================================================
#pragma	once


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ初期化
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_BmpInit( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ページごとのBMPウィンドウ追加
 *
 * @param	wk		ワーク
 * @param	page	ページ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_BmpAdd( BPLIST_WORK * wk, u32 page );

//--------------------------------------------------------------------------------------------
/**
 * 追加BMPウィンドウ削除
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_BmpFree( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * BMPウィンドウ全削除
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_BmpFreeAll( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * BMP書き込み
 *
 * @param	wk		ワーク
 * @param	page	ページ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_BmpWrite( BPLIST_WORK * wk, u32 page );

//--------------------------------------------------------------------------------------------
/**
 * メッセージ表示
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_TalkMsgSet( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * メッセージ表示開始
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_TalkMsgStart( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * HP表示 & HPゲージ表示
 *
 * @param	wk		ワーク
 * @param	pos		ポケモン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_P1_HPPut( BPLIST_WORK * wk, u8 pos );

//--------------------------------------------------------------------------------------------
/**
 * ページ１のLv表示
 *
 * @param	wk		ワーク
 * @param	pos		並び位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_P1_LvPut( BPLIST_WORK * wk, u8 pos );

//--------------------------------------------------------------------------------------------
/**
 * 技回復選択ページのBMP表示
 *
 * @param	wk		ワーク
 * @param	widx	ウィンドウインデックス
 * @param	pos		技位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_PPRcv( BPLIST_WORK * wk, u16 widx, u16 pos );

//--------------------------------------------------------------------------------------------
/**
 * 技忘れエラーメッセージ表示
 *
 * @param		wk    ワーク
 * @param		err		エラー状況
 *
 * @return  none
 *
 *  ページ６で使用
 */
//--------------------------------------------------------------------------------------------
extern void BPL_HidenMsgPut( BPLIST_WORK * wk, u32 mode );

//--------------------------------------------------------------------------------------------
/**
 * アイテム使用メッセージセット
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BattlePokeList_ItemUseMsgSet( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 技によるアイテム使用エラーメッセージセット
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_ItemUseSkillErrMsgSet( BPLIST_WORK * wk );


extern void BPLISTBMP_PrintMain( BPLIST_WORK * wk );

extern void BPLISTBMP_SetStrScrn( BPLIST_WORK * wk );
extern void BPLISTBMP_SetCommentScrn( BPLIST_WORK * wk );

extern void BPLISTBMP_PokeSelInfoMesPut( BPLIST_WORK * wk );
extern void BPLISTBMP_DeadSelInfoMesPut( BPLIST_WORK * wk );
