//============================================================================================
/**
 * @file	bgwinfrm.h
 * @brief	ＢＧスクリーン配置処理
 * @author	Hiroyuki Nakamura
 * @date	08.08.19
 */
//============================================================================================
#ifndef BGWINFRM_H
#define	BGWINFRM_H


//============================================================================================
//	定数定義
//============================================================================================

typedef struct _BGWINFRM_WORK	BGWINFRM_WORK;		// フレームワーク

// 表示状態
enum {
	BGWINFRM_ON = 0,	// 表示
	BGWINFRM_OFF,		// 非表示
};

// 転送モード
enum {
	BGWINFRM_TRANS_NONE = 0,	// 転送しない
	BGWINFRM_TRANS_NORMAL,		// 通常転送（即転送）
	BGWINFRM_TRANS_VBLANK,		// VBLANK内で転送
};


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * フレームワーム作成
 *
 * @param	mode	転送モード
 * @param	max		登録最大数
 * @param	heapID	ヒープＩＤ
 *
 * @return	フレームワーム
 */
//--------------------------------------------------------------------------------------------
extern BGWINFRM_WORK * BGWINFRM_Create( u32 mode, u32 max, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * フレームワーム解放
 *
 * @param	wk		フレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_Exit( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * フレーム追加
 *
 * @param	wk		フレームワーク
 * @param	index	インデックス
 * @param	frm		ＢＧフレーム
 * @param	sx		Ｘサイズ
 * @param	sy		Ｙサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_Add( BGWINFRM_WORK * wk, u32 index, u32 frm, u32 sx, u32 sy );

//--------------------------------------------------------------------------------------------
/**
 * フレームにスクリーンをセット
 *
 * @param	wk		フレームワーク
 * @param	index	フレームインデックス
 * @param	scr		スクリーンデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_FrameSet( BGWINFRM_WORK * wk, u32 index, u16 * scr );

//--------------------------------------------------------------------------------------------
/**
 * フレームにアーカイブスクリーンデータをセット
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	fileIdx		アーカイブファイルインデックス
 * @param	dataIdx		データインデックス
 * @param	comp		圧縮フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_FrameSetArc(
				BGWINFRM_WORK * wk, u32 index, u32 fileIdx, u32 dataIdx, BOOL comp );

//--------------------------------------------------------------------------------------------
/**
 * フレームにアーカイブスクリーンデータをセット（ハンドル指定）
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	ah			アーカイブハンドル
 * @param	dataIdx		データインデックス
 * @param	comp		圧縮フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_FrameSetArcHandle(
				BGWINFRM_WORK * wk, u32 index, ARCHANDLE * ah, u32 dataIdx, BOOL comp );

//--------------------------------------------------------------------------------------------
/**
 * フレームをＢＧに配置（位置指定）
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	px			Ｘ座標
 * @param	py			Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_FramePut( BGWINFRM_WORK * wk, u32 index, s8 px, s8 py );

//--------------------------------------------------------------------------------------------
/**
 * フレームをＢＧに配置（記憶位置）
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_FrameOn( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレームを非表示
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_FrameOff( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレーム自動移動設定
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	mvx			Ｘ移動量（１フレーム）
 * @param	mvy			Ｙ移動量（１フレーム）
 * @param	cnt			移動フレーム数
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_MoveInit( BGWINFRM_WORK * wk, u32 index, s8 mvx, s8 mvy, u8 cnt );

//--------------------------------------------------------------------------------------------
/**
 * フレーム自動移動メイン（全体）
 *
 * @param	wk			フレームワーク
 *
 * @return	none
 *
 * @li	BGWINFRM_MoveOne() と同時に使用すると 2move/1frame になるので注意
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_MoveMain( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * フレーム自動移動メイン（個別）
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @retval	"0 = 停止中"
 * @retval	"1 = 移動中"
 *
 * @li	BGWINFRM_MoveMain() と同時に使用すると 2move/1frame になるので注意
 */
//--------------------------------------------------------------------------------------------
extern u32 BGWINFRM_MoveOne( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレーム自動移動チェック
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @retval	"0 = 停止中"
 * @retval	"1 = 移動中"
 */
//--------------------------------------------------------------------------------------------
extern u32 BGWINFRM_MoveCheck( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレーム表示チェック
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @retval	"BGWINFRM_ON = 表示"
 * @retval	"BGWINFRM_OFF = 非表示"
 */
//--------------------------------------------------------------------------------------------
extern u32 BGWINFRM_VanishCheck( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレームを表示するＢＧを変更
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	frm			変更先のＢＧフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_BGFrameChange( BGWINFRM_WORK * wk, u32 index, u32 frm );

//--------------------------------------------------------------------------------------------
/**
 * 指定位置のパレットを変更
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	px			Ｘ座標
 * @param	py			Ｙ座標
 * @param	sx			Ｘサイズ
 * @param	sy			Ｙサイズ
 * @param	pal			パレット
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_PaletteChange( BGWINFRM_WORK * wk, u32 index, u8 px, u8 py, u8 sx, u8 sy, u8 pal );

//--------------------------------------------------------------------------------------------
/**
 * BMPWINを書き込む
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	win			BMPWIN
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_BmpWinOn( BGWINFRM_WORK * wk, u32 index, GFL_BMPWIN * win );

//--------------------------------------------------------------------------------------------
/**
 * フレームのスクリーンを取得
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @return	スクリーンのポインタ
 */
//--------------------------------------------------------------------------------------------
extern u16 * BGWINFRM_FrameBufGet( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレームの表示ＢＧフレームを取得
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 *
 * @return	ＢＧフレーム
 */
//--------------------------------------------------------------------------------------------
extern u8 BGWINFRM_BGFrameGet( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * フレームの表示位置を取得
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	px			Ｘ座標格納場所
 * @param	py			Ｙ座標格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_PosGet( BGWINFRM_WORK * wk, u32 index, s8 * px, s8 * py );

//--------------------------------------------------------------------------------------------
/**
 * フレームのサイズを取得
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	sx			Ｘサイズ格納場所
 * @param	sy			Ｙサイズ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_SizeGet( BGWINFRM_WORK * wk, u32 index, u16 * sx, u16 * sy );

//--------------------------------------------------------------------------------------------
/**
 * 表示エリア設定
 *
 * @param	wk			フレームワーク
 * @param	index		フレームインデックス
 * @param	lx			左Ｘ座標
 * @param	rx			右Ｘ座標
 * @param	uy			上Ｙ座標
 * @param	dy			下Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BGWINFRM_PutAreaSet( BGWINFRM_WORK * wk, u32 index, s8 lx, s8 rx, s8 uy, s8 dy );


#endif	// BGWINFRM_H
