//=============================================================================================
/**
 * @file	bmp_win.h
 * @brief	BG描画システムプログラム
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 */
//=============================================================================================
#ifndef _BMP_WIN_H_
#define _BMP_WIN_H_

#undef GLOBAL
#ifdef __BMP_WIN_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

// 無効ビットマップインデックス値
#define	GFL_BMPWIN_FRM_NULL		(0xff)

///BMPWINデータ構造体
typedef	struct	gfl_bmpwin_data	GFL_BMPWIN_DATA;

///BMPWIN設定データ構造体
typedef struct{
	u8			frm_num;	///<ウインドウ使用フレーム
	u8			pos_x;		///<ウインドウ領域の左上のX座標（キャラ単位で指定）
	u8			pos_y;		///<ウインドウ領域の左上のY座標（キャラ単位で指定）
	u8			siz_x;		///<ウインドウ領域のXサイズ（キャラ単位で指定）
	u8			siz_y;		///<ウインドウ領域のYサイズ（キャラ単位で指定）
	u8			palnum;		///<ウインドウ領域のパレットナンバー
	u16			chrnum;		///<ウインドウキャラ領域の開始キャラクタナンバー
}BMPWIN_SET;


//ビットマップシフト用定義
#define	GFL_BMPWIN_SHIFT_U		( 0 )
#define	GFL_BMPWIN_SHIFT_D		( 1 )
#define	GFL_BMPWIN_SHIFT_L		( 2 )		//未完成
#define	GFL_BMPWIN_SHIFT_R		( 3 )		//未完成

// BitmapWindowのビットモード
enum {
	GFL_BMPWIN_BITMODE_4 = 0,
	GFL_BMPWIN_BITMODE_8 = 1
};




//--------------------------------------------------------------------------------------------
/**
 * BMPWINデータ確保
 *
 * @param	heapID	ヒープID
 * @param	bgl		BGシステム構造体
 * @param	num		確保するBMPウィンドウの数
 *
 * @return	確保したデータのアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL GFL_BMPWIN_DATA * GFL_BMPWIN_sysInit( u32 heapID, GFL_BG_INI *bgl, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * BMPWINデータ破棄
 *
 * @param	wk		BMPWINデータ
 */
//--------------------------------------------------------------------------------------------
GLOBAL	void	GFL_BMPWIN_sysExit( GFL_BMPWIN_DATA * wk );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN登録チェック
 *
 * @param	win		BMPWINデータ
 *
 * @retval	"TRUE = 登録済み"
 * @retval	"FALSE = 未登録"
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BMPWIN_AddCheck( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN追加
 *
 * @param	win		BMPWINデータ格納場所
 * @param	frmnum	BGフレーム番号
 * @param	posx	X座標（キャラ単位）
 * @param	posy	Y座標（キャラ単位）
 * @param	sizx	Xサイズ（キャラ単位）
 * @param	sizy	Xサイズ（キャラ単位）
 * @param	palnum	使用パレット番号
 * @param	chrofs	使用CGXオフセット
 * @param	heapID	ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Add(
		GFL_BMPWIN_DATA * win, u8 frmnum,
		u8 posx, u8 posy, u8 sizx, u8 sizy, u8 palnum, u16 chrofs, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN追加（OBJ用にキャラ領域だけを確保。4bit専用。）
 *
 * @param	win			BMPWINデータ格納場所
 * @param	sizx		Xサイズ（キャラ単位）
 * @param	sizy		Xサイズ（キャラ単位）
 * @param	chrofs		使用CGXオフセット
 * @param   fill_color	埋め尽くしカラー番号(0〜0xf)
 * @param	heapID		ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_AddChar(
		GFL_BMPWIN_DATA * win, u8 sizx, u8 sizy, u16 chrofs, u8 fill_color, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMPWIN追加（データ指定）
 *
 * @param	win			BMPWINデータ格納場所
 * @param	dat			BMPWIN_SET構造体型データへのポインタ
 * @param	heapID		ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_AddEx( GFL_BMPWIN_DATA * win, const BMPWIN_SET * dat, u32 heapID );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ削除
 *
 * @param	win		BMPWINデータ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Del( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ削除（BMPデータも開放）
 *
 * @param	win		BMPWINデータ
 * @param	num		個数
 *
 * @return	none
 *
 *	GFL_BMPWIN_AllocGet()で確保した場合に使用して下さい
 *	
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Free( GFL_BMPWIN_DATA * win, u8 num );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ表示（スクリーン、キャラ転送）
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_On( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ表示（スクリーン転送リクエスト、キャラ転送）
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_OnVReq( GFL_BMPWIN_DATA * win );

//------------------------------------------------------------------
/**
 * Window領域スクリーンを関連BG内部バッファに作成する
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_MakeScrn( GFL_BMPWIN_DATA * win );

//------------------------------------------------------------------
/**
 * Window領域スクリーンを関連BG内部バッファに作成する
 * （サイズ指定可。BitmapWindow作成時よりも小さい領域のスクリーン作成が可能。大きいのはＮＧ。）
 *
 * @param   win			BitmapWindow へのポインタ
 * @param   width		スクリーン作成領域の幅（キャラ単位）
 * @param   height		スクリーン作成領域の高さ（キャラ単位）
 *
 */
//------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_MakeScrnLimited( GFL_BMPWIN_DATA * win, u32 width, u32 height );

//------------------------------------------------------------------
/**
 * 関連BG内部バッファのWindow領域スクリーンをクリアする
 *
 * @param   win		
 *
 */
//------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_ClearScrn( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ表示（キャラ転送のみ）
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_CgxOn( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ非表示（スクリーンクリア＆転送）
 *
 * @param	win		BMPデータ格納場所
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Off( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ非表示（スクリーンクリア＆転送リクエスト）
 *
 * @param	win		BMPデータ格納場所
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_OffVReq( GFL_BMPWIN_DATA * win );


//--------------------------------------------------------------------------------------------
/**
 * 指定カラーで全体を塗りつぶす
 *
 * @param	win		BMPデータ格納場所
 * @param	col		カラーナンバー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_DataFill( GFL_BMPWIN_DATA * win, u8 col );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップ描画
 *
 * @param	win		BMPデータ格納場所
 * @param	src		ビットマップ表示データポインタ
 * @param	src_x	ビットマップ表示開始位置Ｘ
 * @param	src_y	ビットマップ表示開始位置Ｙ
 * @param	src_dx	ビットマップデータサイズＸ
 * @param	src_dy	ビットマップデータサイズＸ
 * @param	win_x	ウインドウ表示開始位置Ｘ
 * @param	win_y	ウインドウ表示開始位置Ｙ
 * @param	win_dx	ウインドウデータサイズＸ
 * @param	win_dy	ウインドウデータサイズＹ
 *
 * @retval	なし
 *
 * ビットマップデータの書き込みを実行
 * ビットマップデータの切り取りを可能にしたもの
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Print(
			GFL_BMPWIN_DATA * win, void * src,
			u16 src_x, u16 src_y, u16 src_dx, u16 src_dy,
			u16 win_x, u16 win_y, u16 win_dx, u16 win_dy );

//--------------------------------------------------------------------------------------------
/**
 *	ビットマップ描画（拡張版）
 *
 * @param	win		BMPデータ格納場所
 * @param	src		ビットマップ表示データポインタ
 * @param	src_x	ビットマップ表示開始位置Ｘ
 * @param	src_y	ビットマップ表示開始位置Ｙ
 * @param	src_dx	ビットマップデータサイズＸ
 * @param	src_dy	ビットマップデータサイズＸ
 * @param	win_x	ウインドウ表示開始位置Ｘ
 * @param	win_y	ウインドウ表示開始位置Ｙ
 * @param	win_dx	ウインドウデータサイズＸ
 * @param	win_dy	ウインドウデータサイズＹ
 * @param	nuki	透明色指定（0〜15 0xff:透明色指定なし）
 *
 * @retval	なし
 *
 * ビットマップデータの書き込みを実行
 * ビットマップデータの切り取りを可能にしたもの
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_PrintEx(
		GFL_BMPWIN_DATA * win, void * src,
		u16 src_x, u16 src_y, u16 src_dx, u16 src_dy,
		u16 win_x, u16 win_y, u16 win_dx, u16 win_dy, u16 nuki );

//--------------------------------------------------------------------------------------------
/**
 * 指定範囲を塗りつぶす
 *
 * @param	win			BMPデータ
 * @param	col			塗りつぶしカラー
 * @param	px			開始X座標
 * @param	py			開始Y座標
 * @param	sx			Xサイズ
 * @param	sy			Yサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Fill( GFL_BMPWIN_DATA * win, u8 col, u16 px, u16 py, u16 sx, u16 sy );

//---------------------------------------------------------------------------------------------
/**
 * 文字表示専用処理効率アップ版
 *
 * @param	win		BMPデータ格納場所
 * @param	src		キャラデータ
 * @param	ssx		フォントXサイズ
 * @param	ssy		フォントYサイズ
 * @param	wx		書き込みX座標
 * @param	wy		書き込みY座標
 *
 * @return	none
 */
//---------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_PrintMsg( GFL_BMPWIN_DATA * win, u8 * src, u16 ssx, u16 ssy, u16 wx, u16 wy );

GLOBAL void GFL_BMPWIN_PrintMsgWide(
		GFL_BMPWIN_DATA * win, const u8 * src, u16 ssx, u16 ssy, u16 wx, u16 wy, u16 tbl );

//--------------------------------------------------------------------------------------------
/**
 * ビットマップウインドウデータシフト
 *
 * @param	win		BMPデータ格納場所
 * @param	direct	方向
 * @param	offset	シフト幅
 * @param	data	空白になった領域を埋めるデータ
 *
 * @return	none
 *
 * ビットマップデータ(全領域)のシフトを実行
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_Shift( GFL_BMPWIN_DATA * win, u8 direct, u8 offset, u8 data );


//=============================================================================================
//=============================================================================================
//	BMPデータ操作
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ取得：BGフレーム番号
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	win->frmnum
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BMPWIN_GetFrame( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ取得：Xサイズ
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	win->sizx
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BMPWIN_GetSizeX( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ取得：Yサイズ
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	win->sizy
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BMPWIN_GetSizeY( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ取得：X座標
 *
 * @param	win		BMPデータ格納場所
 * @param	index	ビットマップ使用設定ヘッダINDEX
 *
 * @return	win->posx
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BMPWIN_GetPosX( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ取得：Y座標
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	win->posy
 */
//--------------------------------------------------------------------------------------------
GLOBAL u8 GFL_BMPWIN_GetPosY( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ取得：キャラ番号
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	win->chrofs
 */
//--------------------------------------------------------------------------------------------
GLOBAL u16 GFL_BMPWIN_GetChrofs( GFL_BMPWIN_DATA * win );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ設定：X座標
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_SetPosX( GFL_BMPWIN_DATA * win, u8 px );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ設定：Y座標
 *
 * @param	win		BMPデータ格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_SetPosY( GFL_BMPWIN_DATA * win, u8 py );

//--------------------------------------------------------------------------------------------
/**
 * BMPデータ設定：パレット番号
 *
 * @param	win		BMPデータ格納場所
 * @param   palnum	パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMPWIN_SetPal( GFL_BMPWIN_DATA * win, u8 palnum );



#undef GLOBAL
#endif
