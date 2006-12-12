//=============================================================================================
/**
 * @file	bmp.c
 * @brief	BMPシステム
 * @author	Hiroyuki Nakamura
 * @date	2006/10/18
 *
 *	ポケモンDPのbg_system.cから分割
 *		BGL		->	bg_sys.c
 *		BMP		->	bmp.c
 *		BMPWIN	->	bmp_win.c
 */
//=============================================================================================
#include "gflib.h"

#define	__BMP_H_GLOBAL__
#include "bmp.h"
#include "bg_sys.h"


//=============================================================================================
//=============================================================================================
//	ビットマップ関連
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * BMP初期化
 *
 * @param	sizex	Xサイズ
 * @param	sizey	Yサイズ
 * @param	col		カラーモード（GFL_BMP_16_COLOR:16色　GFL_BMP_256_COLOR:256色）
 * @param	heapID	ヒープＩＤ
 *
 * @return	取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
GFL_BMP_DATA * GFL_BMP_sysInit( int sizex, int sizey, int col, u32 heapID )
{
	GFL_BMP_DATA * bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );

	bmp->size_x	=	sizex;
	bmp->size_y	=	sizey;
	bmp->adrs	=	GFL_HEAP_AllocMemoryClear( heapID, sizex * sizey * col );

	return bmp;
}

//--------------------------------------------------------------------------------------------
/**
 * システムワークエリア開放
 *
 * @param	bgl		システムワークエリアへのポインタ
 */
//--------------------------------------------------------------------------------------------
void	GFL_BMP_sysExit( GFL_BMP_DATA *bmp )
{
	GFL_HEAP_FreeMemory( bmp->adrs );
	GFL_HEAP_FreeMemory( bmp );
}

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
GFL_BMP_DATA * GFL_BMP_CharLoad( int arcID, int datID, int compflag, u32 heapID )
{
	GFL_BMP_DATA		*bmp = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_BMP_DATA) );
	void				*src;
	NNSG2dCharacterData	*chr;

	src=GFL_ARC_UtilLoad( arcID, datID, compflag, heapID );
	if( NNS_G2dGetUnpackedBGCharacterData( src, &chr ) == FALSE){
		OS_Panic("GFL_BMP_CharLoad:Faild\n");
		return NULL;
	}

	bmp->size_x	=	chr->W*GFL_BG_1CHRDOTSIZ;
	bmp->size_y	=	chr->H*GFL_BG_1CHRDOTSIZ;
	bmp->adrs	=	GFL_HEAP_AllocMemory( heapID, chr->szByte );
	GFL_STD_MemCopy32( chr->pRawData, bmp->adrs, chr->szByte );

	GFL_HEAP_FreeMemory( src );

	return bmp;
}

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
#define	NULLPAL_L	(nuki_col)

#define SRC_ADRS	(src->adrs)
#define SRC_POSX	(pos_sx)
#define SRC_POSY	(pos_sy)
#define SRC_SIZX	(src->size_x)
#define SRC_SIZY	(src->size_y)
#define	SRC_XARG	(((SRC_SIZX) + (SRC_SIZX & 7))>>3)

#define DST_ADRS	(dest->adrs)
#define DST_POSX	(pos_dx)
#define DST_POSY	(pos_dy)
#define DST_SIZX	(dest->size_x)
#define DST_SIZY	(dest->size_y)
#define	DST_XARG	(((DST_SIZX) + (DST_SIZX & 7))>>3)

#define WRT_SIZX	(size_x)
#define WRT_SIZY	(size_y)

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


void GFL_BMP_PrintMain(
			const GFL_BMP_DATA * src, const GFL_BMP_DATA * dest,
			u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
			u16 size_x, u16 size_y, u16 nuki_col )
{
	int	sx, dx, sy, dy, src_dat, shiftval, x_max, y_max, srcxarg, dstxarg;
	u8	*srcadrs, *dstadrs;

	if((DST_SIZX - DST_POSX) < WRT_SIZX){
		x_max = DST_SIZX - DST_POSX + SRC_POSX;
	}else{
		x_max = WRT_SIZX + SRC_POSX;
	}

	if((DST_SIZY - DST_POSY) < WRT_SIZY){
		y_max = DST_SIZY - DST_POSY + SRC_POSY;
	}else{
		y_max = WRT_SIZY + SRC_POSY;
	}

	srcxarg = SRC_XARG;
	dstxarg = DST_XARG;

	if(nuki_col==GF_BMPPRT_NOTNUKI){	//抜き色指定なし
		for(sy=SRC_POSY, dy=DST_POSY; sy < y_max; sy++, dy++){
			for(sx=SRC_POSX, dx=DST_POSX; sx < x_max; sx++, dx++){
				srcadrs	= DPPCALC(SRC_ADRS, sx, sy, srcxarg);
				dstadrs = DPPCALC(DST_ADRS, dx, dy, dstxarg);
	
				src_dat = (*srcadrs >> ((sx & 1)*4)) & 0x0f;
				shiftval = (dx & 1)*4;
				*dstadrs = (u8)((src_dat << shiftval)|(*dstadrs & (0xf0 >> shiftval)));
			}
		}
	}else{				//抜き色指定あり
		for(sy=SRC_POSY, dy=DST_POSY; sy < y_max; sy++, dy++){
			for(sx=SRC_POSX, dx=DST_POSX; sx < x_max; sx++, dx++){
				srcadrs	= DPPCALC(SRC_ADRS, sx, sy, srcxarg);
				dstadrs = DPPCALC(DST_ADRS, dx, dy, dstxarg);
	
				src_dat = (*srcadrs >> ((sx & 1)*4)) & 0x0f;
	
				if(src_dat != NULLPAL_L){
					shiftval = (dx & 1)*4;
					*dstadrs = (u8)((src_dat << shiftval)|(*dstadrs & (0xf0 >> shiftval)));
				}
			}
		}
	}
}

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
void GFL_BMP_PrintMain256(
		const GFL_BMP_DATA * src, const GFL_BMP_DATA * dest,
		u16 pos_sx, u16 pos_sy, u16 pos_dx, u16 pos_dy,
		u16 size_x, u16 size_y, u16 nuki_col )
{
	int	sx, dx, sy, dy, x_max, y_max, srcxarg, dstxarg;
	u8 * srcadrs;
	u8 * dstadrs;

	if( (DST_SIZX - DST_POSX) < WRT_SIZX ){
		x_max = DST_SIZX - DST_POSX + SRC_POSX;
	}else{
		x_max = WRT_SIZX + SRC_POSX;
	}

	if( (DST_SIZY - DST_POSY) < WRT_SIZY ){
		y_max = DST_SIZY - DST_POSY + SRC_POSY;
	}else{
		y_max = WRT_SIZY + SRC_POSY;
	}
	srcxarg = SRC_XARG;
	dstxarg = DST_XARG;

	if( nuki_col == GF_BMPPRT_NOTNUKI ){	//抜き色指定なし
		for( sy=SRC_POSY, dy=DST_POSY; sy<y_max; sy++, dy++ ){
			for( sx=SRC_POSX, dx=DST_POSX; sx<x_max; sx++, dx++ ){
				srcadrs	= DPPCALC_256( SRC_ADRS, sx, sy, srcxarg );
				dstadrs = DPPCALC_256( DST_ADRS, dx, dy, dstxarg );
				*dstadrs = *srcadrs;
			}
		}
	}else{				//抜き色指定あり
		for( sy=SRC_POSY, dy=DST_POSY; sy<y_max; sy++, dy++ ){
			for( sx=SRC_POSX, dx=DST_POSX; sx<x_max; sx++, dx++ ){
				srcadrs	= DPPCALC_256( SRC_ADRS, sx, sy, srcxarg );
				dstadrs = DPPCALC_256( DST_ADRS, dx, dy, dstxarg );
				if( *srcadrs != NULLPAL_L ){
					*dstadrs = *srcadrs;
				}
			}
		}
	}
}

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
void GFL_BMP_Fill(
		const GFL_BMP_DATA * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
{
	int	x,y,x_max,y_max,xarg;
	u8	*destadrs;

	x_max = pos_dx + size_x;
	if(x_max > dest->size_x){
		x_max = dest->size_x;
	}

	y_max = pos_dy + size_y;
	if(y_max > dest->size_y){
		y_max = dest->size_y;
	}

	xarg = (((dest->size_x) + (dest->size_x&7))>>3);

	for(y = pos_dy; y < y_max; y++){
		for(x = pos_dx; x < x_max; x++){

			destadrs=DPPCALC(dest->adrs, x, y, xarg);
			if(x&1){
				*destadrs&=0x0f;
				*destadrs|=(col_code<<4);
			}
			else{
				*destadrs&=0xf0;
				*destadrs|=col_code;
			}
		}
	}
}

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
void GFL_BMP_Fill256(
		const GFL_BMP_DATA * dest,
		u16 pos_dx, u16 pos_dy, u16 size_x, u16 size_y, u8 col_code )
{
	int	x,y,x_max,y_max,xarg;
	u8	*destadrs;

	x_max = pos_dx + size_x;
	if( x_max > dest->size_x ){
		x_max = dest->size_x;
	}

	y_max = pos_dy + size_y;
	if( y_max > dest->size_y ){
		y_max = dest->size_y;
	}

	xarg = ( dest->size_x + (dest->size_x & 7) ) >> 3;

	for( y=pos_dy; y<y_max; y++ ){
		for( x=pos_dx; x<x_max; x++ ){
			destadrs = DPPCALC_256( dest->adrs, x, y, xarg );
			*destadrs = col_code;
		}
	}
}
