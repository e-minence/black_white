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

//ＢＧキャラサイズ定義
#define	GFL_BG_CHRSIZ_128x128		( 0x2000 )
#define	GFL_BG_CHRSIZ_256x128		( 0x4000 )
#define	GFL_BG_CHRSIZ_128x256		( 0x4000 )
#define	GFL_BG_CHRSIZ_256x256		( 0x8000 )
#define	GFL_BG_CHRSIZ256_128x128	( 0x2000 * 2 )
#define	GFL_BG_CHRSIZ256_256x128	( 0x4000 * 2 )
#define	GFL_BG_CHRSIZ256_128x256	( 0x4000 * 2 )
#define	GFL_BG_CHRSIZ256_256x256	( 0x8000 * 2 )

// データ構造定義
#define GFL_BG_DATA_LZH	(0)

//ＢＧスクロールモード定義
#define GFL_BG_SCROLL_X_SET	(0)
#define GFL_BG_SCROLL_X_INC	(1)
#define GFL_BG_SCROLL_X_DEC	(2)
#define GFL_BG_SCROLL_Y_SET	(3)
#define GFL_BG_SCROLL_Y_INC	(4)
#define GFL_BG_SCROLL_Y_DEC	(5)

// VRAM確保方向定義
#define GFL_BG_CHRAREA_GET_F	(0)		//VRAM前方確保
#define GFL_BG_CHRAREA_GET_B	(1)		//VRAM後方確保

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

    u32		charSize;		//キャラクタエリアサイズ

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
extern	void	GFL_BG_Init( HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * システムワークエリア開放
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_BG_Exit( void );

//--------------------------------------------------------------------------------------------
/**
 * エリアマネージャを使用して未使用領域を検索して領域を確保
 *
 * @param	frmnum		ＢＧフレーム番号
 * @param	size		確保するサイズ
 * @param	dir			確保する方向
 * 
 * @return	pos			確保した領域のポジション（確保できなかった時はAREAMAN_POS_NOTFOUND）
 */
//--------------------------------------------------------------------------------------------
extern	u32 GFL_BG_AllocCharacterArea( u32 frmnum, u32 size, u8 dir );

//--------------------------------------------------------------------------------------------
/**
 * 確保した領域を開放
 *
 * @param	frmnum		ＢＧフレーム番号
 * @param	pos			確保した領域の先頭位置
 * @param	size		開放するサイズ
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_BG_FreeCharacterArea( u32 frmnum, u32 pos, u32 size );

//--------------------------------------------------------------------------------------------
/**
 * BGL取得
 *
 * @return	BGL
 */
//--------------------------------------------------------------------------------------------
extern GFL_BG_INI * GFL_BG_GetBGL( void );

//--------------------------------------------------------------------------------------------
/**
 * ヒープID取得
 *
 * @return	BGLのヒープID
 */
//--------------------------------------------------------------------------------------------
extern u32	GFL_BG_GetHeapID( void );

//--------------------------------------------------------------------------------------------
/**
 * BGモード設定
 *
 * @param	data	BGモード設定データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BG_InitBG( const GFL_BG_SYS_HEADER * data );

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
extern void GFL_BG_InitBGDisp( const GFL_BG_SYS_HEADER * data, u8 flg );

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
extern void GFL_BG_SetBGControl( u8 frmnum, const GFL_BG_BGCNT_HEADER * data, u8 mode );

//--------------------------------------------------------------------------------------------
/**
 * GFL_BG_SetBGControlで取得したメモリを開放
 *
 * @param	frmnum		BGフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BG_FreeBGControl( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BG面設定(３Ｄ専用：BG0)
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BG_SetBGControl3D( u8 priority );

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
extern void GFL_BG_ResetBGControl( u8 frm, u8 flg, u8 prm );

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
extern void GFL_BG_SetPriority( u8 frmnum, u8 priority );

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
extern void GFL_BG_SetVisible( u8 frmnum, u8 visible );


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
extern void GFL_BG_SetScroll( u8 frmnum, u8 mode, int value );

//--------------------------------------------------------------------------------------------
/**
 * スクロール値Ｘ取得
 *
 * @param	frmnum		BGフレーム番号
 *
 * @return	int			スクロール値Ｘ
 */
//--------------------------------------------------------------------------------------------
extern int GFL_BG_GetScrollX( u32 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * スクロール値Ｙ取得
 *
 * @param	frmnum		BGフレーム番号
 *
 * @return	int			スクロール値Ｙ
 */
//--------------------------------------------------------------------------------------------
extern int GFL_BG_GetScrollY( u32 frmnum );

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
extern void GFL_BG_SetAffineScroll( u8 frmnum, u8 mode, int value, const MtxFx22 * mtx, int cx, int cy );

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
extern void GFL_BG_SetAffine( u8 frmnum, const MtxFx22 * mtx, int cx, int cy );


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
extern void GFL_BG_DecodeData( const void * src, void * dst, u32 datasiz );


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
extern void GFL_BG_LoadScreenReq( u8 frmnum );

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
extern void GFL_BG_LoadScreen( u8 frmnum, const void * src, u32 datasiz, u32 offs );

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
extern void GFL_BG_LoadScreenBuffer( u8 frmnum, const void * dat, u32 datasiz );

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
extern void GFL_BG_LoadCharacter( u8 frmnum, const void * src, u32 datasiz, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 * エリアマネージャに領域確保をしてキャラクタデータを転送
 *
 * @param	frmnum	領域確保するフレームナンバー（VRAMアドレスを取得するのに使用）
 * @param	mem		転送先アドレス
 * @param	size	確保＆転送サイズ
 *
 * @retval	データを読み込んだアドレス
 */
//--------------------------------------------------------------------------------------------
extern	u32 GFL_BG_LoadCharacterAreaMan( u32 frmnum, void *mem, u32 size );

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
extern void GFL_BG_SetClearCharacter( u8 frmnum, u32 datasiz, u32 offs, HEAPID heap );

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
extern void GFL_BG_FillCharacter( u32 frmnum, u32 clear_code, u32 charcnt, u32 offs );

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
extern void GFL_BG_LoadPalette( u8 frmnum, void * buf, u16 siz, u16 ofs );

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
extern void GFL_BG_SetBackGroundColor( u8 frmnum, u16 col );


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
extern void GFL_BG_WriteScreen( u8 frmnum, const void * buf, u8 px, u8 py, u8 sx, u8 sy );

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
extern void GFL_BG_WriteScreenExpand( u8 frmnum, u8 write_px, u8 write_py, u8 write_sx, u8 write_sy,
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
extern void GFL_BG_WriteScreenFree( u8 frmnum, u8 write_px, u8 write_py, u8 write_sx, u8 write_sy,
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
extern void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode );

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
extern void GFL_BG_ChangeScreenPalette( u8 frmnum, u8 px, u8 py, u8 sx, u8 sy, u8 pal );

//--------------------------------------------------------------------------------------------
/**
 * スクリーンバッファをクリアして転送
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BG_ClearScreen( u8 frmnum );

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
extern void GFL_BG_ClearScreenCode( u8 frmnum, u16 clear_code );

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
extern void GFL_BG_ClearScreenCodeVReq( u8 frmnum, u16 clear_code );


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
extern void * GFL_BG_GetCharacterAdrs( u8 frmnum );

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
extern void GFL_BG_ChangeCharacter4BitTo8Bit( const u8 * chr, u32 chr_size, u8 * buf, u8 pal_ofs );

//--------------------------------------------------------------------------------------------
/**
 * 4bitのキャラデータを8bitに変換する（変換先取得）
 *
 * @param	chr			変換元データ（4bitキャラ）
 * @param	chr_size	変換元データのサイズ
 * @param	pal_ofs		元パレット番号 ( 0 〜 16 )
 * @param	heapID		ヒープID
 *
 * @return	取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
extern void * GFL_BG_ChangeCharacter4BitTo8BitAlloc( const u8 * chr, u32 chr_size, u8 pal_ofs, HEAPID heapID );


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
extern void * GFL_BG_GetScreenBufferAdrs( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンバッファサイズ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクリーンバッファサイズ
 */
//--------------------------------------------------------------------------------------------
extern u32 GFL_BG_GetScreenBufferSize( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンバッファオフセット）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクリーンバッファオフセット
 */
//--------------------------------------------------------------------------------------------
extern u32 GFL_BG_GetScreenBufferOffset( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクリーンタイプ取得）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	GFL_BG_SCRSIZ_128x128 等
 */
//--------------------------------------------------------------------------------------------
extern u32 GFL_BG_GetScreenType( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクロールカウンタX）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクロールカウンタX
 */
//--------------------------------------------------------------------------------------------
extern int GFL_BG_GetScreenScrollX( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（スクロールカウンタY）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	スクロールカウンタY
 */
//--------------------------------------------------------------------------------------------
extern int GFL_BG_GetScreenScrollY( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（BGモード）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	BGモード
 */
//--------------------------------------------------------------------------------------------
extern u8 GFL_BG_GetBgMode( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（カラーモード）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	カラーモード
 */
//--------------------------------------------------------------------------------------------
extern u8 GFL_BG_GetScreenColorMode( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（キャラサイズ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	キャラサイズ
 */
//--------------------------------------------------------------------------------------------
extern u8 GFL_BG_GetBaseCharacterSize( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（回転角度）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	回転角度
 */
//--------------------------------------------------------------------------------------------
extern u16 GFL_BG_GetRadian( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（X方向の拡縮パラメータ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	X方向の拡縮パラメータ
 */
//--------------------------------------------------------------------------------------------
extern fx32 GFL_BG_GetScaleX( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（Y方向の拡縮パラメータ）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	Y方向の拡縮パラメータ
 */
//--------------------------------------------------------------------------------------------
extern fx32 GFL_BG_GetScaleY( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（回転中心X座標）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	回転中心X座標
 */
//--------------------------------------------------------------------------------------------
extern int GFL_BG_GetCenterX( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGLステータス取得（回転中心Y座標）
 *
 * @param	frmnum	BGフレーム番号
 *
 * @return	回転中心Y座標
 */
//--------------------------------------------------------------------------------------------
extern int GFL_BG_GetCenterY( u8 frmnum );

//--------------------------------------------------------------------------------------------
/**
 * BGプライオリティ取得
 *
 * @param	frm		BGフレーム番号
 *
 * @return	プライオリティ
 */
//--------------------------------------------------------------------------------------------
extern u8 GFL_BG_GetPriority( u8 frm );



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
extern void GFL_BG_VBlankFunc( void );


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
extern void GFL_BG_LoadScreenV_Req( u8 frmnum );

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
extern void GFL_BG_SetScrollReq( u8 frmnum, u8 mode, int value );

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
extern void GFL_BG_SetRadianReq( u8 frmnum, u8 mode, u16 value );

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
extern void GFL_BG_SetScaleReq( u8 frmnum, u8 mode, fx32 value );

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
extern void GFL_BG_SetRotateCenterReq( u8 frmnum, u8 mode, int value );


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
extern u8 GFL_BG_CheckDot( u8 frmnum, u16 px, u16 py, u16 * pat );

//--------------------------------------------------------------------------------------------
/**
 * ファイルパス指定系の関数は削除するのでここにまとめておく（読み込みは、GFL_ARCを使用するのを推奨）
 */
//--------------------------------------------------------------------------------------------
#if 0
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
extern void GFL_BG_LoadScreenFile( u8 frmnum, const char * path, u32 offs );

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
extern void GFL_BG_LoadCharacterFile( u8 frmnum, const char * path, u32 offs );

//--------------------------------------------------------------------------------------------
/**
 * キャラクター転送（ファイル参照）（エリアマネージャを使用して開いてる領域に自動で転送）
 *
 * @param	frmnum		BGフレーム番号
 * @param	src			転送するデータ
 * @param	datasiz		転送サイズ
 *
 * @return	キャラクター確保領域のポジション
 *
 *	圧縮未対応
 */
//--------------------------------------------------------------------------------------------
extern	u32 GFL_BG_LoadCharacterFileAreaMan( u8 frmnum, const char * path );

//=============================================================================================
//=============================================================================================
//	NITRO-CHARACTERデータ展開処理
//=============================================================================================
//=============================================================================================

extern void GFL_BG_NTRCHR_CharLoadEx( u8 frmnum, const char * path, u32 offs, u32 size );

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
extern void GFL_BG_NTRCHR_CharLoad( u8 frmnum, const char * path, u32 offs );




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
extern NNSG2dCharacterData * GFL_BG_NTRCHR_CharGet( void ** buf, int mode, const char * path );

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
extern NNSG2dPaletteData * GFL_BG_NTRCHR_PalLoad( void ** mem, int mode, const char * path );

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
extern void GFL_BG_NTRCHR_ScrnLoad( u8 frmnum, const char * path, u32 offs );



#endif

#endif
