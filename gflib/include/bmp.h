//=============================================================================================
/**
 * @file	bmp.h
 * @brief	BG描画システムプログラム
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 */
//=============================================================================================
#ifndef _BMP_H_
#define _BMP_H_

#undef GLOBAL
#ifdef __BMP_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif


//--------------------------------------------------------------------
//ビットマップ描画データ構造体
typedef struct{
	u8 * adrs;		///<キャラクタデータの先頭アドレス
	u16	size_x;		///<キャラクタデータのXサイズ
	u16	size_y;		///<キャラクタデータのYサイズ
}GFL_BMP_DATA;


#define	GFL_BMP_16_COLOR	(0x20)
#define	GFL_BMP_256_COLOR	(0x40)

//ビットマップ書き込み用定義
#define	GF_BMPPRT_NOTNUKI	( 0xffff )	// 抜き色指定なし

// BMP書き込みマクロ
#if 0
#define DPPCALC(adrs, pos_x, pos_y, size_x)		\
	(u8*)((adrs) +								\
	(((pos_x)>>1) & 3) +						\
	(((pos_x)>>3) << 5) +						\
	((((pos_y)>>3) * (size_x)) << 5) +			\
	((u32)((pos_y)<<29)>>27)					\
	)

#define DPPCALC_256(adrs, pos_x, pos_y, size_x)	\
	(u8*)((adrs) +								\
	(pos_x & 7) +								\
	((pos_x>>3) << 6) +							\
	(((pos_y>>3) * size_x) << 6) +				\
	((u32)((pos_y)<<29)>>26)					\
	)
#else
#define DPPCALC(adrs, pos_x, pos_y, size_x)		\
	(u8*)((adrs) +								\
	((pos_x >> 1) & 0x00000003) +				\
	((pos_x << 2) & 0x00003fe0) +				\
	(((pos_y << 2) & 0x00003fe0) * size_x) +	\
	((u32)((pos_y << 2)&0x0000001c))			\
	)

#define DPPCALC_256(adrs, pos_x, pos_y, size_x)	\
	(u8*)((adrs) +								\
	(pos_x & 0x00000007) +						\
	((pos_x << 3) & 0x00007fc0) +				\
	(((pos_y << 3) & 0x00007fc0) * size_x) +	\
	((u32)((pos_y << 3)&0x00000038))			\
	)
#endif

//--------------------------------------------------------------------------------------------
/**
 * BMP初期化
 *
 * @param	sizex	Xサイズ
 * @param	sizey	Yサイズ
 * @param	col		カラーモード
 * @param	heapID	ヒープＩＤ
 *
 * @return	取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL	GFL_BMP_DATA * GFL_BMP_sysInit( int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * システムワークエリア開放
 *
 * @param	bgl		システムワークエリアへのポインタ
 */
//--------------------------------------------------------------------------------------------
GLOBAL	void	GFL_BMP_sysExit( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/**
 * BMPキャラロード（BMPデータの元データ作成）
 *
 * @param	arcID		キャラのアーカイブID
 * @param	datID		キャラのアーカイブ内のデータインデックス
 * @param	compflag	圧縮、非圧縮フラグ
 * @param	heapID		ヒープＩＤ
 *
 * @return	取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
GLOBAL	GFL_BMP_DATA * GFL_BMP_CharLoad( int arcID, int datID, int compflag, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * 読み込みと書き込みのアドレスと範囲を指定してキャラを描画(透明色指定あり）
 *
 * @param	src			読み込み元キャラデータヘッダー構造体ポインタ
 * @param	dest		書き込み先キャラデータヘッダー構造体ポインタ
 * @param	pos_sx		読み込み元読み込み開始X座標
 * @param	pos_sy		読み込み元読み込み開始Y座標
 * @param	pos_dx		書き込み先書き込み開始X座標
 * @param	pos_dy		書き込み先書き込み開始Y座標
 * @param	size_x		描画範囲Xサイズ
 * @param	size_y		描画範囲Yサイズ
 * @param	nuki_col	透明色指定（0〜15 0xff:透明色指定なし）
 *
 * @return	none
 *
 * @li	１６色用
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMP_PrintMain(
			const GFL_BMP_DATA * src, GFL_BMP_DATA * dest,
			u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
			u16 size_x, u16 size_y, u16 nuki_col );

//--------------------------------------------------------------------------------------------
/**
 * 読み込みと書き込みのアドレスと範囲を指定してキャラを描画(透明色指定あり）
 *
 * @param	src			読み込み元キャラデータヘッダー構造体ポインタ
 * @param	dest		書き込み先キャラデータヘッダー構造体ポインタ
 * @param	pos_sx		読み込み元読み込み開始X座標
 * @param	pos_sy		読み込み元読み込み開始Y座標
 * @param	pos_dx		書き込み先書き込み開始X座標
 * @param	pos_dy		書き込み先書き込み開始Y座標
 * @param	size_x		描画範囲Xサイズ
 * @param	size_y		描画範囲Yサイズ
 * @param	nuki_col	透明色指定（0〜15 0xff:透明色指定なし）
 *
 * @return	none
 *
 * @li	２５６色用
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMP_PrintMain256(
		const GFL_BMP_DATA * src, GFL_BMP_DATA * dest,
		u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
		u16 size_x, u16 size_y, u16 nuki_col );

//--------------------------------------------------------------------------------------------
/**
 * 範囲を指定して指定された色コードで塗りつぶし
 *
 * @param	dest		書き込み先キャラデータヘッダー構造体ポインタ
 * @param	pos_dx		書き込み先書き込み開始X座標
 * @param	pos_dy		書き込み先書き込み開始Y座標
 * @param	size_x		描画範囲Xサイズ
 * @param	size_y		描画範囲Yサイズ
 * @param	col_code	塗りつぶし色コード
 *
 * @return	none
 *
 * @li	１６色用
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMP_Fill(
		GFL_BMP_DATA * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code );

//--------------------------------------------------------------------------------------------
/**
 * 範囲を指定して指定された色コードで塗りつぶし
 *
 * @param	dest		書き込み先キャラデータヘッダー構造体ポインタ
 * @param	pos_dx		書き込み先書き込み開始X座標
 * @param	pos_dy		書き込み先書き込み開始Y座標
 * @param	size_x		描画範囲Xサイズ
 * @param	size_y		描画範囲Yサイズ
 * @param	col_code	塗りつぶし色コード
 *
 * @return	none
 *
 * @li	２５６色用
 */
//--------------------------------------------------------------------------------------------
GLOBAL void GFL_BMP_Fill256(
		GFL_BMP_DATA * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code );


#undef GLOBAL
#endif
