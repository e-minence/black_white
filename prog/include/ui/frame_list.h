//============================================================================================
/**
 * @file		frame_list.h
 * @brief		ＢＧフレーム全体を使用したリスト処理
 * @author	Hiroyuki Nakamura
 * @date		10.01.23
 *
 *	モジュール名：FRAMELIST
 *
 *	・１画面につき、ＢＧ１面を占有します（スクロールするため）
 *	・内部でBMPWINを確保しているため、BMPWINの初期化後にリストを作成してください
 */
//============================================================================================
#pragma	once

#include "print/printsys.h"


//============================================================================================
//	定数定義
//============================================================================================

// リスト動作結果
#define	FRAMELIST_RET_CURSOR_ON		( 0xfffffff8 )		// カーソル表示
#define	FRAMELIST_RET_MOVE				( 0xfffffff9 )		// カーソル移動
#define	FRAMELIST_RET_SCROLL			( 0xfffffffa )		// スクロール通常
#define	FRAMELIST_RET_RAIL				( 0xfffffffb )		// レールスクロール
#define	FRAMELIST_RET_SLIDE				( 0xfffffffc )		// スライドスクロール
#define	FRAMELIST_RET_JUMP_TOP		( 0xfffffffd )		// リスト最上部へジャンプ
#define	FRAMELIST_RET_JUMP_BOTTOM	( 0xfffffffe )		// リスト最下部へジャンプ
#define	FRAMELIST_RET_NONE				( 0xffffffff )		// 動作なし

#define	FRAMELIST_SUB_BG_NONE	( 0xff )		// 上画面ＢＧ未使用

// リストモード
enum {
	FRAMELIST_MODE_NORMAL = 0,		// 通常モード（タッチをすると選択項目のパラメータを返す）
	FRAMELIST_MODE_REEL,					// 項目部分をタッチするとスクロールする
};

// タッチパラメータ
enum {
	FRAMELIST_TOUCH_PARAM_SLIDE = 0,					// スライド

	FRAMELIST_TOUCH_PARAM_RAIL,								// レール

	FRAMELIST_TOUCH_PARAM_UP,									// 上移動
	FRAMELIST_TOUCH_PARAM_DOWN,								// 下移動

	FRAMELIST_TOUCH_PARAM_PAGE_UP,						// ページ上移動
	FRAMELIST_TOUCH_PARAM_PAGE_DOWN,					// ページ下移動

	FRAMELIST_TOUCH_PARAM_LIST_TOP,						// リスト最上位へ
	FRAMELIST_TOUCH_PARAM_LIST_BOTTOM,				// リスト最下位へ

//	FRAMELIST_TOUCH_PARAM_ITEM = 0x80000000,	// 項目
	FRAMELIST_TOUCH_PARAM_ITEM,								// 項目
};

// コールバック関数
typedef struct {
	// ワーク, 項目位置, PRINT_UTIL, 表示Ｙ座標, 描画先
	void (*draw)(void*,u32,PRINT_UTIL*,s16,BOOL);		// 描画処理
	// ワーク, カーソル位置
	void (*move)(void*,u32);												// 移動処理
	// ワーク, スクロール値
	void (*scroll)(void*,s8);												// スクロール時
}FRAMELIST_CALLBACK;

// タッチデータ
typedef struct {
	GFL_UI_TP_HITTBL	tbl;
	u32	prm;
}FRAMELIST_TOUCH_DATA;

// ヘッダーデータ
typedef struct {
	u8	mainBG;								// 下画面ＢＧ
	u8	subBG;								// 上画面ＢＧ

	u8	selPal;								// 選択項目のパレット

	s8	itemPosX;							// 項目フレーム表示開始Ｘ座標
	s8	itemPosY;							// 項目フレーム表示開始Ｙ座標
	u8	itemSizX;							// 項目フレーム表示Ｘサイズ
	u8	itemSizY;							// 項目フレーム表示Ｙサイズ

	u8	bmpPosX;							// フレーム内に表示するBMPWINの表示Ｘ座標
	u8	bmpPosY;							// フレーム内に表示するBMPWINの表示Ｙ座標
	u8	bmpSizX;							// フレーム内に表示するBMPWINの表示Ｘサイズ
	u8	bmpSizY;							// フレーム内に表示するBMPWINの表示Ｙサイズ
	u8	bmpPal;								// フレーム内に表示するBMPWINのパレット

	u16	itemMax;							// 項目登録数
	u16	graMax;								// 背景登録数

	u8	initPos;							// 初期位置
	u8	posMax;								// カーソル移動範囲
	u16	initScroll;						// 初期スクロール値

	u8	scrollSpeed[6];				// スクロール速度 [0] = 最速

	PRINT_QUE * que;					// プリントキュー

	const FRAMELIST_TOUCH_DATA * touch;	// タッチデータ

	const FRAMELIST_CALLBACK * cbFunc;	// コールバック関数
	void * cbWork;											// コールバック関数に渡されるワーク

}FRAMELIST_HEADER;

typedef struct _FRAMELIST_WORK	FRAMELIST_WORK;		// ＢＧフレームリストワーク


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリスト作成
 *
 * @param		hed			ヘッダーデータ
 * @param		heapID	ヒープＩＤ
 *
 * @return	ワーク
 */
//--------------------------------------------------------------------------------------------
extern FRAMELIST_WORK * FRAMELIST_Create( FRAMELIST_HEADER * hed, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリスト削除
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_Exit( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目追加
 *
 * @param		wk			ワーク
 * @param		type		背景番号
 * @param		param		パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_AddItem( FRAMELIST_WORK * wk, u32 type, u32 param );

//--------------------------------------------------------------------------------------------
/**
 * @brief		背景グラフィック読み込み
 *
 * @param		wk				ワーク
 * @param		ah				アーカイブハンドル
 * @param		dataIdx		データインデックス
 * @param		comp			圧縮フラグ TRUE = 圧縮
 * @param		frameNum	背景番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void FRAMELIST_LoadFrameGraphicAH( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, BOOL comp, u32 frameNum );

extern void FRAMELIST_LoadBlinkPalette( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, u32 startPal, u32 endPal );

//extern PRINT_UTIL * FRAMELIST_GetPrintUtil( FRAMELIST_WORK * wk, u32 printIdx );

extern u32 FRAMELIST_GetItemParam( FRAMELIST_WORK * wk, u32 itemIdx );


//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリスト初期描画
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern BOOL FRAMELIST_Init( FRAMELIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリストメイン処理
 *
 * @param		wk		ワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
extern u32 FRAMELIST_Main( FRAMELIST_WORK * wk );
