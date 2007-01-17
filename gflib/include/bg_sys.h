//=============================================================================================
/**
 * @file	bg_sys.h
 * @brief	BG描画システムプログラム
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 */
//=============================================================================================
#ifndef _BG_SYS_H_
#define _BG_SYS_H_

#undef GLOBAL
#ifdef __BG_SYS_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

//=============================================================================================
//	inline関数を仮置き
//=============================================================================================
//==============================================================================
/**
 *	メモリフィル関数
 * 
 * @param   adrs	メモリフィルするアドレス
 * @param   value	メモリフイルする値
 * @param   size	メモリフィルするサイズ
 *
 */
//==============================================================================
inline void memset(void *adrs,u8 value,int size)
{
	MI_CpuFill8(adrs,value,size);
}

//==============================================================================
/**
 *	メモリコピー関数
 * 
 * @param   dest	メモリコピー先アドレス
 * @param   src		メモリコピー元アドレス
 * @param   size	メモリコピーするサイズ
 *
 */
//==============================================================================
inline void memcpy(void *dest,void *src,int size)
{
	MI_CpuCopy8(dest,src,size);
}

//=============================================================================================
//	型宣言
//=============================================================================================
#define	GFL_BG_FRAME_MAX	(8)			///< BGフレーム数
#define GFL_BG_1SCRDATASIZ	(0x2)		///< スクリーンデータのバイト数

//メイン・サブ　スクリーン指定
#define GFL_BG_MAIN_DISP	(0)
#define GFL_BG_SUB_DISP		(1)

//モード定義
#define GFL_BG_MODE_TEXT	(0)		// テキスト
#define GFL_BG_MODE_AFFINE	(1)		// アフィン
#define GFL_BG_MODE_256X16	(2)		// アフィン拡張BG

//ＢＧナンバー定義
#define GFL_BG_FRAME0_M	(0)
#define GFL_BG_FRAME1_M	(1)
#define GFL_BG_FRAME2_M	(2)
#define GFL_BG_FRAME3_M	(3)
#define GFL_BG_FRAME0_S	(4)
#define GFL_BG_FRAME1_S	(5)
#define GFL_BG_FRAME2_S	(6)
#define GFL_BG_FRAME3_S	(7)

//ＢＧスクリーンサイズ定義
#define	GFL_BG_SCRSIZ_128x128		( 0 )
#define	GFL_BG_SCRSIZ_256x256		( 1 )
#define	GFL_BG_SCRSIZ_256x512		( 2 )
#define	GFL_BG_SCRSIZ_512x256		( 3 )
#define	GFL_BG_SCRSIZ_512x512		( 4 )
#define	GFL_BG_SCRSIZ_1024x1024		( 5 )

// データ構造定義
#define GFL_BG_DATA_LZH	(0)

//ＢＧスクロールモード定義
#define GFL_BG_SCROLL_X_SET	(0)
#define GFL_BG_SCROLL_X_INC	(1)
#define GFL_BG_SCROLL_X_DEC	(2)
#define GFL_BG_SCROLL_Y_SET	(3)
#define GFL_BG_SCROLL_Y_INC	(4)
#define GFL_BG_SCROLL_Y_DEC	(5)

// 拡縮・回転変更パラメータ
enum {
	GFL_BG_RADION_SET = 0,	// 回転角度変更（指定した値をセット）
	GFL_BG_RADION_INC,		// 回転角度変更（指定した値を＋）
	GFL_BG_RADION_DEC,		// 回転角度変更（指定した値を−）

	GFL_BG_SCALE_X_SET,		// X方向拡縮変更（指定した値をセット）
	GFL_BG_SCALE_X_INC,		// X方向拡縮変更（指定した値を＋）
	GFL_BG_SCALE_X_DEC,		// X方向拡縮変更（指定した値を−）

	GFL_BG_SCALE_Y_SET,		// Y方向拡縮変更（指定した値をセット）
	GFL_BG_SCALE_Y_INC,		// Y方向拡縮変更（指定した値を＋）
	GFL_BG_SCALE_Y_DEC,		// Y方向拡縮変更（指定した値を−）

	GFL_BG_CENTER_X_SET,	// 回転中心X座標変更（指定した値をセット）
	GFL_BG_CENTER_X_INC,	// 回転中心X座標変更（指定した値を＋）
	GFL_BG_CENTER_X_DEC,	// 回転中心X座標変更（指定した値を−）

	GFL_BG_CENTER_Y_SET,	// 回転中心Y座標変更（指定した値をセット）
	GFL_BG_CENTER_Y_INC,	// 回転中心Y座標変更（指定した値を＋）
	GFL_BG_CENTER_Y_DEC		// 回転中心Y座標変更（指定した値を−）
};

//ＢＧスクリーンデータ埋め尽くし用定義
#define GFL_BG_SCRWRT_PALNL		(16)	//パレット無視（スクリーン上データを引き継ぎ）
#define GFL_BG_SCRWRT_PALIN		(17)	//パレット入り（転送元にパレットデータ込み）

//BGスクリーンデータのバッファモード定義
#define GFL_BG_MODE_1DBUF		(0)		//通常の1次元配列データ
#define GFL_BG_MODE_2DBUF		(1)		//折り返し有りの2次元?配列データ

#define GFL_BG_1CHRDATASIZ	(0x20)		///< キャラデータサイズ（４ビット）
#define GFL_BG_8BITCHRSIZ	(0x40)		///< キャラデータサイズ（８ビット）
#define GFL_BG_1CHRDOTSIZ	(0x08)		///< キャラデータの一辺のドット数


//--------------------------------------------------------------------
//ＢＧシステム構造体
typedef struct {
	GXDispMode 	dispMode;		//表示モード指定
	GXBGMode	bgMode;			//ＢＧモード指定(メインスクリーン)
	GXBGMode	bgModeSub;		//ＢＧモード指定(サブスクリーン)
	GXBG0As		bg0_2Dor3D;		//ＢＧ０の２Ｄ、３Ｄモード選択
}GFL_BG_SYS_HEADER;

//--------------------------------------------------------------------
//ＢＧコントロール設定構造体
typedef struct {
	int		scrollX;		//初期表示Ｘ設定
	int		scrollY;		//初期表示Ｙ設定
	u32		scrbufferSiz;	//スクリーンバッファサイズ( 0 = 使用しない )
	u32		scrbufferOfs;	//スクリーンバッファオフセット

    u8		screenSize;		//スクリーンサイズ
    u8		colorMode;		//カラーモード
    u8		screenBase;		//スクリーンベースブロック
    u8		charBase;		//キャラクタベースブロック
    u8		bgExtPltt;		//ＢＧ拡張パレットスロット選択
	u8		priority;		//表示プライオリティー
	u8		areaOver;		//エリアオーバーフラグ
	u8		dummy;			//
	BOOL	mosaic;			//モザイク設定

}GFL_BG_BGCNT_HEADER;




typedef struct _GFL_BG_INI	GFL_BG_INI;



//--------------------------------------------------------------------
// BG面再設定パラメータ
enum {
	BGL_RESET_COLOR = 0,	// BG再設定フラグ：カラーモード
	BGL_RESET_SCRBASE,		// BG再設定フラグ：スクリーンベース
	BGL_RESET_CHRBASE		// BG再設定フラグ：キャラベース
};









//--------------------------------------------------------------------------------------------
/**
 * BGL初期化
 *
 * @param	heapID	ヒープＩＤ
 *
 * @return	取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL GFL_BG_INI * GFL_BG_sysInit( u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * システムワークエリア開放
 *
 * @param	bgl		システムワークエリアへのポインタ
 */
//--------------------------------------------------------------------------------------------
GLOBAL void	GFL_BG_sysExit( GFL_BG_INI *bgl );

//--------------------------------------------------------------------------------------------
/**
 * エリアマネージャを使用して未使用領域を検索して領域を確保
 *
 * @param	frmnum		ＢＧフレーム番号
 * @param	size		確保するサイズ
 * 
 * @return	pos			確保した領域のポジション（確保できなかった時はAREAMAN_POS_NOTFOUND）
 */
//--------------------------------------------------------------------------------------------
GLOBAL	u32 GFL_BG_CharAreaGet( u32 frmnum, u32 size );

//--------------------------------------------------------------------------------------------
/**
 * 確保した領域を開放
 *
 * @param	frmnum		ＢＧフレーム番号
 * @param	pos			確保した領域の先頭位置
 * @param	size		開放するサイズ
 */
//--------------------------------------------------------------------------------------------
GLOBAL	void GFL_BG_CharAreaFree( u32 frmnum, u32 pos, u32 size );

//--------------------------------------------------------------------------------------------
/**
 * ヒープID取得
 *
 * @param	bgl		BGLデータ
 *
 * @return	BGLのヒープID
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32	GFL_BG_HeapIDGet( GFL_BG_INI * bgl );

//--------------------------------------------------------------------------------------------
/**
 * BGモード設定
 *
 * @param	data	BGモード設定データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_InitBG( const GFL_BG_SYS_HEADER * data );

//--------------------------------------------------------------------------------------------
/**
 * BGモード設定（画面ごと）
 *
 * @param	data	BGモード設定データ
 * @param	flg		設定する画面
 *
 * @return	none
 *
 * @li	flg = GFL_BG_MAIN_DISP : メイン画面
 * @li	flg = GFL_BG_SUB_DISP : サブ画面
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_InitBGDisp( const GFL_BG_SYS_HEADER * data, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * BG面設定
 *
 * @param	frmnum		ＢＧフレーム番号
 * @param	data		ＢＧコントロールデータ
 * @param	mode		ＢＧモード
 * 
 * @return	none
 *
 * @li	mode = GFL_BG_MODE_TEXT		: テキスト
 * @li	mode = GFL_BG_MODE_AFFINE	: アフィン
 * @li	mode = GFL_BG_MODE_256X16	: アフィン拡張BG
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_BGControlSet( GFL_BG_INI * bgl, u8 frmnum, const GFL_BG_BGCNT_HEADER * data, u8 mode );

//--------------------------------------------------------------------------------------------
/**
 * BG面再設定
 *
 * @param	frmnum		ＢＧフレーム番号
 * @param	flg			変更パラメータ
 * @param	prm			変更値
 * 
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_BGControlReset( GFL_BG_INI * bgl, u8 frm, u8 flg, u8 prm );

//--------------------------------------------------------------------------------------------
/**
 * GFL_BG_BGControlSetで取得したメモリを開放
 *
 * @param	ini			BGLデータ
 * @param	frmnum		BGフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_BGControlExit( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * 表示プライオリティ設定
 *
 * @param	frmnum		BGフレーム番号
 * @param	priority	プライオリティ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_PrioritySet( u8 frmnum, u8 priority );

//--------------------------------------------------------------------------------------------
/**
 * 表示ON・OFF設定
 *
 * @param	frmnum		BGフレーム番号
 * @param	visible		VISIBLE_ON or VISIBLE_OFF
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_VisibleSet( u8 frmnum, u8 visible );


//=============================================================================================
//=============================================================================================
//	スクロール関数
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * スクロール処理
 *
 * @param	frmnum		BGフレーム番号
 * @param	mode		スクロールモード
 * @param	value		スクロール値
 *
 * @return	none
 *
 * @li	拡縮面が拡縮・回転する場合はGFL_BG_AffineScrollSet(...)を使用すること
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrollSet( GFL_BG_INI * bgl, u8 frmnum, u8 mode, int value );

//--------------------------------------------------------------------------------------------
/**
 * スクロール値Ｘ取得
 *
 * @param	frmnum		BGフレーム番号
 *
 * @return	int			スクロール値Ｘ
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_BG_ScrollGetX( GFL_BG_INI * bgl, u32 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * スクロール値Ｙ取得
 *
 * @param	frmnum		BGフレーム番号
 *
 * @return	int			スクロール値Ｙ
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_BG_ScrollGetY( GFL_BG_INI * bgl, u32 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * 拡縮面の拡縮・回転・スクロール処理
 *
 * @param	frmnum		BGフレーム番号
 * @param	mode		スクロールモード
 * @param	value		スクロール値
 * @param	mtx			変換行列
 * @param	cx			回転中心X座標
 * @param	cy			回転中心Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_AffineScrollSet( GFL_BG_INI * bgl,
		u8 frmnum, u8 mode, int value, const MtxFx22 * mtx, int cx, int cy );

//--------------------------------------------------------------------------------------------
/**
 * 拡縮・回転処理
 *
 * @param	frmnum		BGフレーム番号
 * @param	mtx			変換行列
 * @param	cx			回転中心X座標
 * @param	cy			回転中心Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_AffineSet( GFL_BG_INI * bgl, u8 frmnum, const MtxFx22 * mtx, int cx, int cy );


//=============================================================================================
//=============================================================================================
//	展開関数
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * データ展開
 *
 * @param	src			展開元
 * @param	dst			展開先
 * @param	datasiz		サイズ
 *
 * @return	none
 *
 * @li	datasiz = GFL_BG_DATA_LZH : 圧縮データ
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_DataDecord( const void * src, void * dst, u32 datasiz );


//=============================================================================================
//=============================================================================================
//	転送関数
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * フレームに設定されたスクリーンデータを全転送
 *
 * @param	frmnum		BGフレーム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_LoadScreenReq( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * 指定データをスクリーンに転送
 *
 * @param	frmnum		BGフレーム番号
 * @param	src			転送するデータ
 * @param	datasiz		転送サイズ
 * @param	offs		オフセット
 *
 * @return	none
 *
 * @li	datasiz = GFL_BG_DATA_LZH : 圧縮データ
 *
 *	圧縮されたデータは解凍場所にsys.bgl->bgsys[frmnum].screen_bufを使用するため
 *	sys.bgl->bgsys[frmnum].screen_bufにデータがセットされるが、非圧縮の場合は、
 *	セットされないので、注意すること。
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_LoadScreen( GFL_BG_INI * bgl, u8 frmnum, const void * src, u32 datasiz, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 * 指定データをスクリーンに転送（ファイル参照）
 *
 * @param	frmnum		BGフレーム番号
 * @param	path		ファイルのパス名
 * @param	offs		オフセット
 *
 * @return	none
 *
 *	圧縮未対応
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_LoadScreenFile( GFL_BG_INI * bgl, u8 frmnum, const char * path, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 *	スクリーンデータをバッファにコピー
 *
 * @param	frmnum		BGフレーム
 * @param	dat			指定データ
 * @param	datasizpx	データサイズ
 *
 * @retrn	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScreenBufSet( GFL_BG_INI * bgl, u8 frmnum, const void * dat, u32 datasiz );

//--------------------------------------------------------------------------------------------
/**
 * キャラクター転送
 *
 * @param	frmnum		BGフレーム番号
 * @param	src			転送するデータ
 * @param	datasiz		転送サイズ
 * @param	offs		オフセット
 *
 * @return	none
 *
 * @li	datasiz = GFL_BG_DATA_LZH : 圧縮データ
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_LoadCharacter( GFL_BG_INI * bgl, u8 frmnum, const void * src, u32 datasiz, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 * キャラクター転送（ファイル参照）
 *
 * @param	frmnum		BGフレーム番号
 * @param	src			転送するデータ
 * @param	datasiz		転送サイズ
 * @param	offs		オフセット
 *
 * @return	none
 *
 *	圧縮未対応
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_LoadCharacterFile( GFL_BG_INI * bgl, u8 frmnum, const char * path, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 * ０クリアされたキャラをセット
 *
 * @param	frmnum		BGフレーム番号
 * @param	datasiz		転送サイズ
 * @param	offs		オフセット
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ClearCharSet( u8 frmnum, u32 datasiz, u32 offs, u32 heap );

//--------------------------------------------------------------------------------------------
/**
 * 指定値でクリアされたキャラをセット
 *
 * @param	frmnum		BGフレーム番号
 * @param	clear_code	クリアコード
 * @param	charcnt		クリアするキャラ数
 * @param	offs		キャラ領域先頭からのオフセット（キャラ数）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_CharFill( GFL_BG_INI * bgl, u32 frmnum, u32 clear_code, u32 charcnt, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 * パレット転送
 *
 * @param	frmnum		BGフレーム番号
 * @param	buf			パレットデータ
 * @param	siz			転送サイズ（バイト単位）
 * @param	ofs			オフセット（バイト単位）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_PaletteSet( u8 frmnum, void * buf, u16 siz, u16 ofs );

//--------------------------------------------------------------------------------------------
/**
 * バックグラウンドカラー転送（パレット０のカラー指定）
 *
 * @param	frmnum		BGフレーム番号
 * @param	col			カラー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_BackGroundColorSet( u8 frmnum, u16 col );


//=============================================================================================
//=============================================================================================
//	スクリーン関連
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * スクリーンデータ書き込み
 *
 * @param	frmnum		BGフレーム
 * @param	buf			書き込むデータ
 * @param	px			書き込み開始Ｘ座標
 * @param	py			書き込み開始Ｙ座標
 * @param	sx			書き込みＸサイズ
 * @param	sy			書き込みＹサイズ
 *
 * @retrn	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrWrite( GFL_BG_INI * bgl, u8 frmnum, const void * buf, u8 px, u8 py, u8 sx, u8 sy );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンデータ書き込み（拡張版）
 *
 * @param	write_px	書き込み開始Ｘ座標
 * @param	write_px	書き込み開始Ｙ座標
 * @param	write_sx	書き込みＸサイズ
 * @param	write_sy	書き込みＹサイズ
 * @param	buf			読み込みデータ
 * @param	buf_px		読み込み開始Ｘ座標
 * @param	buf_py		読み込み開始Ｙ座標
 * @param	buf_sx		読み込みデータＸサイズ（write_sx,syとは違い、読み込みデータの
 * @param	buf_sy		読み込みデータＹサイズ　全体の大きさそのものを代入する）
 *
 * @retrn	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrWriteExpand( GFL_BG_INI * bgl,
				u8 frmnum, u8 write_px, u8 write_py, u8 write_sx, u8 write_sy,
				const void * buf, u8 buf_px, u8 buf_py, u8 buf_sx, u8 buf_sy );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンデータ書き込み(折り返しありデータ版)
 *
 * @param	frmnum		BGフレーム
 * @param	write_px	書き込み開始Ｘ座標
 * @param	write_px	書き込み開始Ｙ座標
 * @param	write_sx	書き込みＸサイズ
 * @param	write_sy	書き込みＹサイズ
 * @param	buf			読み込みデータ
 * @param	buf_px		読み込み開始Ｘ座標
 * @param	buf_py		読み込み開始Ｙ座標
 * @param	buf_sx		読み込みデータＸサイズ（write_sx,syとは違い、読み込みデータの
 * @param	buf_sy		読み込みデータＹサイズ　全体の大きさそのものを代入する）
 *
 * @retrn	none
 *
 * @li	折り返し有り1x1〜64x64キャラのフリーサイズスクリーンデータの矩形書き込み
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrWriteFree( GFL_BG_INI * bgl,
				u8 frmnum, u8 write_px, u8 write_py, u8 write_sx, u8 write_sy,
				const void * buf, u8 buf_px, u8 buf_py, u8 buf_sx, u8 buf_sy );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンデータバッファ埋め尽くし
 *
 * @param	frmnum		BGフレーム
 * @param	dat			指定データ
 * @param	px			書き込み開始Ｘ座標
 * @param	py			書き込み開始Ｙ座標
 * @param	sx			書き込みＸサイズ
 * @param	sy			書き込みＹサイズ
 * @param	mode		パレット番号など
 *
 * @retrn	none
 *
 * @li	mode = GFL_BG_SCRWRT_PALNL : 現在のスクリーンのパレットを引き継ぐ
 * @li	mode = GFL_BG_SCRWRT_PALIN : datにパレットデータを含む
 * @li	mode = 0 〜 15 : パレット番号
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrFill( GFL_BG_INI * bgl, u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode );

//--------------------------------------------------------------------------------------------
/**
 * 指定フレームのスクリーンの指定位置のパレットを変更する
 *
 * @param	frmnum	BGフレーム番号
 * @param	px		開始X位置
 * @param	py		開始Y位置
 * @param	sx		Xサイズ
 * @param	sy		Yサイズ
 * @param	pal		パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrPalChange( GFL_BG_INI * bgl, u8 frmnum, u8 px, u8 py, u8 sx, u8 sy, u8 pal );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンバッファをクリアして転送
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrClear( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンバッファを指定コードでクリアして転送
 *
 * @param	frmnum	BGフレーム番号
 * @param   clear_code	クリアコード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrClearCode( GFL_BG_INI * bgl, u8 frmnum, u16 clear_code );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンバッファを指定コードでクリアして転送リクエスト
 *
 * @param	frmnum	BGフレーム番号
 * @param   clear_code	クリアコード
 *
 * @return	none
 *
 * @li	転送はVBlankで
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrClearCodeVReq( GFL_BG_INI * bgl, u8 frmnum, u16 clear_code );


//=============================================================================================
//=============================================================================================
//	キャラクター関連
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * キャラデータを取得
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	キャラデータのアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL void * GFL_BG_CgxGet( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * 4bitのキャラデータを8bitに変換する（変換先指定）
 *
 * @param	chr			変換元データ（4bitキャラ）
 * @param	chr_size	変換元データのサイズ
 * @param	buf			変換先
 * @param	pal_ofs		元パレット番号 ( 0 〜 16 )
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_4BitCgxChange8BitMain( const u8 * chr, u32 chr_size, u8 * buf, u8 pal_ofs );

//--------------------------------------------------------------------------------------------
/**
 * 4bitのキャラデータを8bitに変換する（変換先取得）
 *
 * @param	chr			変換元データ（4bitキャラ）
 * @param	chr_size	変換元データのサイズ
 * @param	pal_ofs		元パレット番号 ( 0 〜 16 )
 * @param	heap		ヒープID
 *
 * @return	取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL void * GFL_BG_4BitCgxChange8Bit( const u8 * chr, u32 chr_size, u8 pal_ofs, u32 heap );


//=============================================================================================
//=============================================================================================
//	BGLステータス取得
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンバッファアドレス）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクリーンバッファアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL void * GFL_BG_ScreenAdrsGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンバッファサイズ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクリーンバッファサイズ
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 GFL_BG_ScreenSizGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンバッファオフセット）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクリーンバッファオフセット
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 GFL_BG_ScreenOfsGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンタイプ取得）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	GFL_BG_SCRSIZ_128x128 等
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 GFL_BG_ScreenTypeGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクロールカウンタX）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクロールカウンタX
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_BG_ScreenScrollXGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクロールカウンタY）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクロールカウンタY
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_BG_ScreenScrollYGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（BGモード）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	BGモード
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BG_BgModeGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（カラーモード）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	カラーモード
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BG_ScreenColorModeGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（キャラサイズ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	キャラサイズ
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BG_BaseCharSizeGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（回転角度）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	回転角度
 */
//--------------------------------------------------------------------------------------------
GLOBAL u16 GFL_BG_RadianGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（X方向の拡縮パラメータ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	X方向の拡縮パラメータ
 */
//--------------------------------------------------------------------------------------------
GLOBAL fx32 GFL_BG_ScaleXGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（Y方向の拡縮パラメータ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	Y方向の拡縮パラメータ
 */
//--------------------------------------------------------------------------------------------
GLOBAL fx32 GFL_BG_ScaleYGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（回転中心X座標）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	回転中心X座標
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_BG_CenterXGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（回転中心Y座標）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	回転中心Y座標
 */
//--------------------------------------------------------------------------------------------
GLOBAL int GFL_BG_CenterYGet( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGプライオリティ取得
 *
 * @param	frm		BGフレーム番号
 *
 * @return	プライオリティ
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BG_PriorityGet( GFL_BG_INI * bgl, u8 frm );



//=============================================================================================
//=============================================================================================
//	NITRO-CHARACTERデータ展開処理
//=============================================================================================
//=============================================================================================

GLOBAL void GFL_BG_NTRCHR_CharLoadEx( GFL_BG_INI * bgl, u8 frmnum, const char * path, u32 offs, u32 size );

//--------------------------------------------------------------------------------------------
/**
 * NITRO-CHARACTERのキャラデータを読み込む
 *
 * @param	frmnum	BGフレーム番号
 * @param	path	ファイルパス
 * @param	offs	オフセット（キャラ単位）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_NTRCHR_CharLoad( GFL_BG_INI * bgl, u8 frmnum, const char * path, u32 offs );




//--------------------------------------------------------------------------------------------
/**
 * NITRO-CHARACTERのキャラデータを取得
 *
 * @param	buf		展開用バッファ
 * @param	mode	指定ヒープ領域定義
 * @param	path	ファイルパス
 *
 * @return	キャラデータの構造体
 */
//--------------------------------------------------------------------------------------------
GLOBAL NNSG2dCharacterData * GFL_BG_NTRCHR_CharGet( void ** buf, int mode, const char * path );

//--------------------------------------------------------------------------------------------
/**
 * NITRO-CHARACTERのパレットデータを展開
 *
 * @param	mem		展開場所
 * @param	mode	指定ヒープ領域定義
 * @param	path	ファイルパス
 *
 * @return	パレットデータ
 *
 * @li		pal->pRawData = パレットデータ
 * @li		pal->szByte   = サイズ
 */
//--------------------------------------------------------------------------------------------
GLOBAL NNSG2dPaletteData * GFL_BG_NTRCHR_PalLoad( void ** mem, int mode, const char * path );

//--------------------------------------------------------------------------------------------
/**
 * NITRO-CHARACTERのスクリーンデータを読み込む
 *
 * @param	frmnum	BGフレーム番号
 * @param	path	ファイルパス
 * @param	offs	オフセット（キャラ単位）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_NTRCHR_ScrnLoad( GFL_BG_INI * bgl, u8 frmnum, const char * path, u32 offs );


//=============================================================================================
//=============================================================================================
//	VBlank関連
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * VBlank転送
 *
 * @param	none
 *
 * @return	none
 *
 * @li	VBlank内で呼んで下さい
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_VBlankFunc(GFL_BG_INI * bgl);


//--------------------------------------------------------------------------------------------
/**
 * スクリーン転送リクエスト ( VBlank )
 *
 * @param	frmnum		BGフレーム番号
 *
 * @return	none
 *
 * @li	VBlank内でGFL_BG_VBlankFunc(...)を呼ぶこと
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_LoadScreenV_Req( GFL_BG_INI * bgl, u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * スクロールリクエスト
 *
 * @param	frmnum		BGフレーム番号
 * @param	mode		スクロールモード
 * @param	value		スクロール値
 *
 * @return	none
 *
 * @li	VBlank内でGFL_BG_VBlankFunc(...)を呼ぶこと
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScrollReq( GFL_BG_INI * bgl, u8 frmnum, u8 mode, int value );

//--------------------------------------------------------------------------------------------
/**
 * 回転角度変更リクエスト
 *
 * @param	frmnum		BGフレーム番号
 * @param	mode		角度変更モード
 * @param	value		回転値
 *
 * @return	none
 *
 * @li	VBlank内でGFL_BG_VBlankFunc(...)を呼ぶこと
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_RadianSetReq( GFL_BG_INI * bgl, u8 frmnum, u8 mode, u16 value );

//--------------------------------------------------------------------------------------------
/**
 * 拡縮変更リクエスト
 *
 * @param	frmnum		BGフレーム番号
 * @param	mode		拡縮変更モード
 * @param	value		変更値
 *
 * @return	none
 *
 * @li	VBlank内でGFL_BG_VBlankFunc(...)を呼ぶこと
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_ScaleSetReq( GFL_BG_INI * bgl, u8 frmnum, u8 mode, fx32 value );

//--------------------------------------------------------------------------------------------
/**
 * 回転中心座標変更リクエスト
 *
 * @param	frmnum		BGフレーム番号
 * @param	mode		変更モード
 * @param	value		変更値
 *
 * @return	none
 *
 * @li	VBlank内でGFL_BG_VBlankFunc(...)を呼ぶこと
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BG_RotateCenterSetReq( GFL_BG_INI * bgl, u8 frmnum, u8 mode, int value );


//=============================================================================================
//=============================================================================================
//	その他
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 指定座標のドットをチェック
 *
 * @param	frmnum	BGフレーム番号
 * @param	px		X座標
 * @param	py		Y座標
 * @param	pat		判定データ
 *
 * @retval	"TRUE = 判定データ(pat)にあり"
 * @retval	"FALSE = 判定データ(pat)になし"
 *
 * @li	１６色の場合、patはビットでチェック
 *			例）*pat = 0xfffd;
 *				カラー0と1の場合、FALSEが返る
 *
 * @li	２５６色の場合、patの最後に0xffffを付加
 *			例）pat[] = { 88, 124, 223, 0xffff };
 *				カラー88, 124, 223以外はFALSEが返る
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BG_DotCheck( GFL_BG_INI * bgl, u8 frmnum, u16 px, u16 py, u16 * pat );





#undef GLOBAL
#endif
