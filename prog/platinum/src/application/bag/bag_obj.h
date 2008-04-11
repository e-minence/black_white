//============================================================================================
/**
 * @file	bag_obj.h
 * @brief	バッグ画面OBJ処理
 * @author	Hiroyuki Nakamura
 * @date	05.10.31
 */
//============================================================================================
#ifndef BAG_OBJ_H
#define BAG_OBJ_H
#undef GLOBAL
#ifdef BAG_OBJ_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	定数定義
//============================================================================================
#define	CURSOR_ACT_PX	( 177 )		// アイテム選択カーソル表示X座標
#define	CURSOR_ACT_PY	( 24 )		// アイテム選択カーソル表示Y座標

enum {
	ACT_OFF = 0,	// アクター非表示
	ACT_ON			// アクター表示
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * セルアクター初期化
 *
 * @param	wk		バッグ画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagCellActorInit( BAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * セルアクター解放
 *
 * @param	wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BAGOBJ_ClactFree( BAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン変更
 *
 * @param	wk		バッグ画面のワーク
 * @param	item	アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagItemIconChange( BAG_WORK * wk, u16 item );

//--------------------------------------------------------------------------------------------
/**
 * アイテム選択カーソルのパレット変更
 *
 * @param	wk		バッグ画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagObj_CursorPalChg( BAG_WORK * wk, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * アニメ
 *
 * @param	wk		バッグ画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void Bag_ObjAnmMain( BAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケットカーソル動作チェック
 *
 * @param	wk		バッグ画面のワーク
 *
 * @retval	"TRUE = 停止中"
 * @retval	"FALSE = 動作中"
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 BagPocketCursorMoveCheck( BAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケットカーソル動作初期化
 *
 * @param	wk		バッグ画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagPocketCursorMoveInit( BAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケットカーソル動作メイン
 *
 * @param	wk		バッグ画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagPocketCursorMoveMain( BAG_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 個数選択矢印セット
 *
 * @param	wk		バッグ画面のワーク
 * @param	mode	使用モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void Bag_NumArrowSet( BAG_WORK * wk, u8 mode );

//--------------------------------------------------------------------------------------------
/**
 * 個数選択矢印表示切替
 *
 * @param	wk		バッグ画面のワーク
 * @param	flg		表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void Bag_NumArrowPut( BAG_WORK * wk, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * 技マシンのアイコンセット
 *
 * @param	wk		バッグ画面のワーク
 * @param	item	アイテム番号
 * @param	flg		表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagObj_WazaIconPut( BAG_WORK * wk, u16 item, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * サブボタンエフェクト初期化
 *
 * @param	wk		バッグ画面のワーク
 * @param	x		表示X座標
 * @param	y		表示Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagObj_SubButtonEffInit( BAG_WORK * wk, s16 x, s16 y );

//--------------------------------------------------------------------------------------------
/**
 * サブボタンエフェクトメイン
 *
 * @param	wk		バッグ画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void BagObj_SubButtonEffMain( BAG_WORK * wk );



#undef GLOBAL
#endif	/* BAG_OBJ_H */
