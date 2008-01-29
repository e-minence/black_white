//=============================================================================================
/**
 * @file	textprint.c
 * @brief	ビットマップ文字列表示用システム
 * 
 */
//=============================================================================================
#include "gflib.h"

#include "textprint.h"
#include "systemfontcode.h"
#include "sjisconv_inter.h"

//------------------------------------------------------------------
/**
 * define定義
 */
//------------------------------------------------------------------

#define	FONT_SIZE_X	(8)
#define	FONT_SIZE_Y	(8)

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
	STRCODE			fcode, EOMCode;
	GFL_BMP_DATA	*bmpfont;

	//描画位置初期化
	tw->nowx = param->writex;
	tw->nowy = param->writey;

	//色設定
	GFL_FONT_SetColor( param->colorF, param->colorB );

	//フォントビットマップ情報作成
	//GFL_FONT_GetDataが呼ばれないとtw->fdata.sizexが不定のため、固定値に変更
//	bmpfont = GFL_BMP_CreateWithData( &tw->fdata.data[0], tw->fdata.sizex, sizemaxY, GFL_BMP_16_COLOR, GFL_HEAPID_SYSTEM );
	bmpfont = GFL_BMP_CreateWithData( &tw->fdata.data[0], FONT_SIZE_X, FONT_SIZE_Y, GFL_BMP_16_COLOR, GFL_HEAPID_SYSTEM );
	EOMCode = GFL_STR_GetEOMCode();
	//フォントデータ取得
	while( (fcode = *textcode ) != EOMCode ) 
	{
		textcode++;

		switch(fcode){

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

			GFL_BMP_Print( bmpfont, param->bmp, 0, 0, tw->nowx, tw->nowy, tw->fdata.sizex, sizemaxY, 0 );

			//次の文字の描画位置を設定
			tw->nowx += ( tw->fdata.sizex + param->spacex );
			break;
		}
	}

	param->writex = tw->nowx;
	param->writey = tw->nowy;

	GFL_BMP_Delete(bmpfont);
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


