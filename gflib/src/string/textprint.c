//=============================================================================================
/**
 * @file	textprint.c
 * @brief	ビットマップ文字列表示用システム
 * 
 */
//=============================================================================================
#include "gflib.h"

#include "textprint.h"
#include "sjisconv_inter.h"
//------------------------------------------------------------------
/**
 * ワーク構造体
 */
//------------------------------------------------------------------
typedef struct {
	GFL_FONT_DATA		fdata;
	u16					nowx;
	u16					nowy;
}TEXT_WORK;

TEXT_WORK* tw;

//------------------------------------------------------------------
/**
 *
 * 初期化
 *
 */
//------------------------------------------------------------------
void
	GFL_TEXT_CreateSystem
		( const char* fontDataPath )
{
	GFL_FONT_Create( fontDataPath );
	tw = GFL_HEAP_AllocMemory( GFL_HEAPID_SYSTEM, sizeof(TEXT_WORK) );
}


//------------------------------------------------------------------
/**
 *
 * 終了
 *
 */
//------------------------------------------------------------------
void
	GFL_TEXT_DeleteSystem
		( void )
{
	GFL_HEAP_FreeMemory( tw );
	GFL_FONT_Delete();
}


//------------------------------------------------------------------
/**
 * ビットマップの内部CGX領域に文字を描画する(STRBUF)
 *
 * @param   text		文字列
 * @param   param		描画用パラメータ
 */
//------------------------------------------------------------------
void
	GFL_TEXT_Print
		( const STRBUF* text, GFL_TEXT_PRINTPARAM* param )
{
	STRCODE* strdat = (STRCODE*)GFL_STR_GetStringCodePointer( text );
	GFL_TEXT_PrintCode( strdat, param );
}


//------------------------------------------------------------------
/**
 * ビットマップの内部CGX領域に文字を描画する(STRCODE)
 *
 * @param   textcode	文字列
 * @param   param		描画用パラメータ
 */
//------------------------------------------------------------------
void
	GFL_TEXT_PrintCode
		( const STRCODE* textcode, GFL_TEXT_PRINTPARAM* param )
{
	u8				sizemaxY	= GFL_FONT_GetSizeMaxY();
	u8				sizeSPC		= GFL_FONT_GetSizeSPC();
	STRCODE			fcode;
	GFL_BMP_DATA	*bmpfont;

	//描画位置初期化
	tw->nowx = param->writex;
	tw->nowy = param->writey;

	//色設定
	GFL_FONT_SetColor( param->colorF, param->colorB );

	//フォントビットマップ情報作成
	bmpfont = GFL_BMP_sysCreate( &tw->fdata.data[0], tw->fdata.sizex, sizemaxY, 
									GFL_BMP_16_COLOR, GFL_HEAPID_SYSTEM );
	
	//フォントデータ取得
	while( (fcode = *textcode ) != EOM_ ) 
	{
		textcode++;

		switch(fcode){

		case EOM_:	/* 終了コード */
			GFL_HEAP_FreeMemory( tw );
			return;

		case CR_:	/* 改行コード */
			tw->nowx = param->writex;
			tw->nowy += ( sizemaxY + param->spacey ); 
			break;

		case spc_:	/* 全角スペースコード */
			tw->nowx += sizeSPC;
			break;

		case h_spc_:	/* 半角スペースコード */
			tw->nowx += (sizeSPC/2);
			break;

		default:
			GFL_FONT_GetData( fcode - STR_DEFAULT_CODE_MAX, &tw->fdata );

			GFL_BMP_PrintMain(	bmpfont, param->bmp, 0, 0, tw->nowx, tw->nowy, 
								tw->fdata.sizex, sizemaxY, 0 );

			//次の文字の描画位置を設定
			tw->nowx += ( tw->fdata.sizex + param->spacex );
			break;
		}
	}
	GFL_BMP_sysDelete(bmpfont);
}


//------------------------------------------------------------------
/**
 * ビットマップの内部CGX領域に文字を描画する(SjisCode)
 *
 * @param   textcode	文字列
 * @param   param		描画用パラメータ
 */
//------------------------------------------------------------------
#define SJISCONVSIZE_MAX (256)//暫定256文字まで
void
	GFL_TEXT_PrintSjisCode
		( const char* textcode, GFL_TEXT_PRINTPARAM* param )
{
	STRCODE* tmpcode = GFL_HEAP_AllocMemory( GFL_HEAPID_SYSTEM, SJISCONVSIZE_MAX*2 );

	GFL_STR_CONV_Sjis2Systemfontcode( textcode, tmpcode, SJISCONVSIZE_MAX );
	GFL_TEXT_PrintCode( tmpcode, param );

	GFL_HEAP_FreeMemory( tmpcode );
}


