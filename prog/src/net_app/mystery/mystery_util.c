//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_util.c
 *	@brief  ふしぎなおくりもので使用するモジュール類
 *	@author	Toru=Nagihashi
 *	@data		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/gfl_use.h"
#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"
#include "system/main.h"    //HEAPID
#include "print/gf_font.h"  //GFL_FONT
#include "app/app_keycursor.h"
#include "app/app_printsys_common.h"
#include "sound/pm_sndsys.h"
#include "system/time_icon.h"

//アーカイブ
#include "arc_def.h"
#include "arc\mystery.naix"

//文字表示
#include "print/printsys.h"

//外部公開
#include "mystery_util.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWINメッセージ描画構造体
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	メッセージウィンドウ
//=====================================
struct _MYSTERY_MSGWIN_WORK
{
	PRINT_UTIL				util;				//文字描画
	PRINT_QUE					*p_que;			//文字キュー
	GFL_BMPWIN*				p_bmpwin;		//BMPWIN
	STRBUF*						p_strbuf;		//文字バッファ
	u16								frm;        //BG面
  PRINTSYS_LSB      color;      //文字色
  GFL_POINT         ofs;        //文字描画位置オフセット
  MYSTERY_MSGWIN_POS pos_type;  //描画位置タイプ
  MYSTERY_MSGWIN_TRANS_MODE mode; //転送モード
};
//-------------------------------------
///	座標計算
//=====================================
static void Mystery_Msgwin_CalcPos( const MYSTERY_MSGWIN_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos );
static void Mystery_Msgwin_CalcPosCore( MYSTERY_MSGWIN_POS type, const GFL_POINT *cp_ofs, GFL_BMP_DATA *p_bmp, STRBUF *p_strbuf, GFL_FONT *p_font, GFL_POINT *p_pos );
//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  初期化
 *
 *	@param	u16 frm BG面
 *	@param	x       X座標
 *	@param	y       Y座標
 *	@param	w       幅
 *	@param	h       高さ
 *	@param	plt     フォントパレット
 *	@param	*p_que  キュー
 *	@param	heapID  ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_MSGWIN_WORK * MYSTERY_MSGWIN_Init( MYSTERY_MSGWIN_TRANS_MODE mode, u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID )
{ 
  MYSTERY_MSGWIN_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_MSGWIN_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_MSGWIN_WORK) );
  p_wk->p_que			= p_que;
  p_wk->frm       = frm;
  p_wk->color     = MYSTERY_MSGWIN_DEFAULT_COLOR;
  p_wk->mode      = mode;

  //バッファ作成
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN作成
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );

	//プリントキュー設定
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//転送
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), PRINTSYS_LSB_GetB( p_wk->color ) );
  if( p_wk->mode == MYSTERY_MSGWIN_TRANS_MODE_AUTO )
  { 
    GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame( p_wk->p_bmpwin ) );
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  破棄
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_Exit( MYSTERY_MSGWIN_WORK* p_wk )
{ 
  //BMPWIN破棄
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//バッファ破棄
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  クリア
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_Clear( MYSTERY_MSGWIN_WORK* p_wk )
{ 
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  プリント開始
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 *	@param	*p_msg            メッセージ
 *	@param	strID             メッセージID
 *	@param	*p_font           フォント
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_Print( MYSTERY_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//一端消去
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), PRINTSYS_LSB_GetB( p_wk->color ) );	

	//文字列作成
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //位置計算
  Mystery_Msgwin_CalcPos( p_wk, p_font, &pos );

	//文字列描画
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  STRBUFプリント開始
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 *	@param	*p_strbuf         STRBUF  すぐ破棄してもかまいません
 *	@param	*p_font           フォント
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_PrintBuf( MYSTERY_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//一端消去
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), PRINTSYS_LSB_GetB( p_wk->color ) );	

	//文字列作成
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //位置計算
  Mystery_Msgwin_CalcPos( p_wk, p_font, &pos );

	//文字列描画
	PRINT_UTIL_PrintColor( &p_wk->util, p_wk->p_que, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  色指定
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 *	@param  lsb               色指定
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_SetColor( MYSTERY_MSGWIN_WORK* p_wk, PRINTSYS_LSB color )
{
  p_wk->color = color;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  位置指定
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 *	@param	x                       X座標
 *	@param	y                       Y座標
 *	@param	type                    指定
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_SetPos( MYSTERY_MSGWIN_WORK* p_wk, s16 x, s16 y, MYSTERY_MSGWIN_POS type )
{ 
  p_wk->ofs.x = x;
  p_wk->ofs.y = y;
  p_wk->pos_type  = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ  プリント書き込み処理
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk   ワーク
 *
 *	@return TRUEならば書き込んだ  FALSEならばまだ
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_MSGWIN_PrintMain( MYSTERY_MSGWIN_WORK* p_wk )
{ 
  if( p_wk->mode == MYSTERY_MSGWIN_TRANS_MODE_AUTO )
  { 
    return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
  }
  else
  {
    if( p_wk->util.transReq )
    {
      if( !PRINTSYS_QUE_IsExistTarget(p_wk->p_que, GFL_BMPWIN_GetBmp(p_wk->util.win)) )
      {
        p_wk->util.transReq = FALSE;
      }
    }
    return !(p_wk->util.transReq);
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINメッセージ転送関数
 *
 *	@param	MYSTERY_MSGWIN_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWIN_Trans( MYSTERY_MSGWIN_WORK* p_wk )
{ 
  GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
  if( p_wk->mode == MYSTERY_MSGWIN_TRANS_MODE_MANUAL )
  { 
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( p_wk->p_bmpwin ) );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  描画座標計算
 *
 *	@param	const MYSTERY_MSGWIN_WORK* cp_wk  ワーク
 *	@param  フォント
 *	@param	*p_pos                            描画位置受け取り
 *
 */
//-----------------------------------------------------------------------------
static void Mystery_Msgwin_CalcPos( const MYSTERY_MSGWIN_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  Mystery_Msgwin_CalcPosCore( cp_wk->pos_type, &cp_wk->ofs, GFL_BMPWIN_GetBmp(cp_wk->p_bmpwin), cp_wk->p_strbuf, p_font, p_pos );
}
//----------------------------------------------------------------------------
/**
 *	@brief  描画位置計算コア部分
 *
 *	@param	MYSTERY_MSGWIN_POS type 位置の種類
 *	@param  cp_ofs                  位置オフセット
 *	@param	GFL_BMP_DATA *cp_bmp    BMP
 *	@param	*p_strbuf               文字列
 *	@param	*p_font                 フォント
 *	@param	*p_pos                  座標受け取り
 */
//-----------------------------------------------------------------------------
static void Mystery_Msgwin_CalcPosCore( MYSTERY_MSGWIN_POS type, const GFL_POINT *cp_ofs, GFL_BMP_DATA *p_bmp, STRBUF *p_strbuf, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  switch( type )
  { 
  case MYSTERY_MSGWIN_POS_ABSOLUTE:     //絶対位置
    *p_pos  = *cp_ofs;
    break;
  case MYSTERY_MSGWIN_POS_WH_CENTER:  //相対座標  幅、高さともに中央
    { 
      u32 x, y;
      x = GFL_BMP_GetSizeX( p_bmp ) / 2;
      y = GFL_BMP_GetSizeY( p_bmp ) / 2;
      x -= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 ) / 2;
      y -= PRINTSYS_GetStrHeight( p_strbuf, p_font ) / 2;
      p_pos->x  = x + cp_ofs->x;
      p_pos->y  = y + cp_ofs->y;
    }
    break;
  case MYSTERY_MSGWIN_POS_H_CENTER:    //相対座標  高さのみ中央
    { 
      u32 y;
      y = GFL_BMP_GetSizeY( p_bmp ) / 2;
      y -= PRINTSYS_GetStrHeight( p_strbuf, p_font ) / 2;
      p_pos->x  = cp_ofs->x;
      p_pos->y  = y + cp_ofs->y;
    }
    break;

  case MYSTERY_MSGWIN_POS_W_BACK: //Xのみ後ろから
    { 
      s32 x;

      x = GFL_BMP_GetSizeX( p_bmp );
      x -= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 );
      x = MATH_IMax( x, 0 );

      p_pos->x  = x + cp_ofs->x;
      p_pos->y  = cp_ofs->y;
    }
    break;
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					テキスト描画構造体
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	定数
//=====================================
#define MYSTERY_TEXT_TYPE_NONE  (MYSTERY_TEXT_TYPE_MAX)

//-------------------------------------
/// メッセージウィンドウ
//=====================================
struct _MYSTERY_TEXT_WORK
{
  GFL_MSGDATA       *p_msg;
  GFL_FONT          *p_font;
  PRINT_STREAM      *p_stream;
  GFL_TCBLSYS       *p_tcbl;
  GFL_BMPWIN*       p_bmpwin;
  STRBUF*           p_strbuf;
  u16               clear_chr;
  u16               heapID;
  PRINT_UTIL        util;
  PRINT_QUE         *p_que;
  u32               print_update;
  BOOL              is_end_print;
  APP_KEYCURSOR_WORK* p_keycursor;
  APP_PRINTSYS_COMMON_WORK  common;
  TIMEICON_WORK     *p_time;
} ;

//-------------------------------------
///	プロトタイプ
//=====================================
static void MYSTERY_TEXT_PrintInner( MYSTERY_TEXT_WORK* p_wk, MYSTERY_TEXT_TYPE type );

//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  初期化
 *
 *	@param	u16 frm       BG面
 *	@param	font_plt      フォントパレット
 *	@param  PRINT_QUE     プリントQ
 *	@param  GFL_FONT      フォント
 *	@param	heapID        ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_TEXT_WORK * MYSTERY_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  MYSTERY_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = MYSTERY_TEXT_TYPE_NONE;


  //バッファ作成
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 768, heapID );

	//BMPWIN作成
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, 1, 19, 30, 4, font_plt, GFL_BMP_CHRAREA_GET_B );

	//プリントキュー設定
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//転送
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  p_wk->p_tcbl    = GFL_TCBL_Init( heapID, heapID, 1, 32 );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  初期化  １行メッセージ
 *
 *	@param	u16 frm       BG面
 *	@param	font_plt      フォントパレット
 *	@param  PRINT_QUE     プリントQ
 *	@param  GFL_FONT      フォント
 *	@param	heapID        ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_TEXT_WORK * MYSTERY_TEXT_InitOneLine( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  MYSTERY_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = MYSTERY_TEXT_TYPE_NONE;

  APP_PRINTSYS_COMMON_PrintStreamInit( &p_wk->common, APP_PRINTSYS_COMMON_TYPE_KEY);

  //バッファ作成
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN作成
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, 1, 21, 30, 2, font_plt, GFL_BMP_CHRAREA_GET_B );

	//プリントキュー設定
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//転送
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  p_wk->p_tcbl    = GFL_TCBL_Init( heapID, heapID, 1, 32 );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  破棄
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Exit( MYSTERY_TEXT_WORK* p_wk )
{ 
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }
  if( p_wk->p_time )
  { 
    TIMEICON_Exit( p_wk->p_time );
    p_wk->p_time  = NULL;
  }
  if( p_wk->p_keycursor )
  { 
    APP_KEYCURSOR_Delete( p_wk->p_keycursor );
  }

  GFL_TCBL_Exit( p_wk->p_tcbl );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  描画処理
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk   ワーク
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Main( MYSTERY_TEXT_WORK* p_wk )
{ 
  switch( p_wk->print_update )
  { 
  default:
    /* fallthor */
  case MYSTERY_TEXT_TYPE_NONE:
    break;

  case MYSTERY_TEXT_TYPE_WAIT:
    PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case MYSTERY_TEXT_TYPE_QUE:
    p_wk->is_end_print  = PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case MYSTERY_TEXT_TYPE_STREAM:
    if( p_wk->p_stream )
    { 
      BOOL is_end;

      if( p_wk->p_keycursor )
      { 
        APP_KEYCURSOR_Main( p_wk->p_keycursor, p_wk->p_stream, p_wk->p_bmpwin );
      }

      is_end  = APP_PRINTSYS_COMMON_PrintStreamFunc( &p_wk->common, p_wk->p_stream );
      if( is_end )
      { 
        p_wk->is_end_print  = TRUE;
      }
    }
  }

  GFL_TCBL_Main( p_wk->p_tcbl );

}
//----------------------------------------------------------------------------
/*
 *	@brief  テキスト プリント開始
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ワーク
 *	@param	*p_msg            メッセージ
 *	@param	strID             メッセージID
 *	@param  type              描画タイプ
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_Print( MYSTERY_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, MYSTERY_TEXT_TYPE type )
{ 
  //文字列作成
  GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //文字描画
  MYSTERY_TEXT_PrintInner( p_wk, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  プリント開始  バッファ版
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ワーク
 *	@param	STRBUF *cp_strbuf       文字バッファ
 *	@param	type                    描画タイプ
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_PrintBuf( MYSTERY_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, MYSTERY_TEXT_TYPE type )
{ 
  //文字列作成
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //文字描画
  MYSTERY_TEXT_PrintInner( p_wk, type );
}
//----------------------------------------------------------------------------
/**
 *	@brief  テキスト文字描画プライベート
 *
 *	@param	MYSTERY_TEXT_WORK* p_wk ワーク
 *	@param	type              描画タイプ
 *
 */
//-----------------------------------------------------------------------------
static void MYSTERY_TEXT_PrintInner( MYSTERY_TEXT_WORK* p_wk, MYSTERY_TEXT_TYPE type )
{ 
  //一端消去
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );

  //ストリーム破棄
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  if( p_wk->p_keycursor )
  { 
    APP_KEYCURSOR_Delete( p_wk->p_keycursor );
    p_wk->p_keycursor = NULL;
  }

  MYSTERY_TEXT_EndWait( p_wk );

  //タイプごとの文字描画
  switch( type )
  { 
  case MYSTERY_TEXT_TYPE_WAIT:    //待機アイコン
    p_wk->p_time  = TIMEICON_Create( GFUser_VIntr_GetTCBSYS(), p_wk->p_bmpwin, p_wk->clear_chr,
        TIMEICON_DEFAULT_WAIT, p_wk->heapID );

    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = MYSTERY_TEXT_TYPE_QUE;
    break;

  case MYSTERY_TEXT_TYPE_QUE:     //プリントキューを使う
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = MYSTERY_TEXT_TYPE_QUE;
    break;

  case MYSTERY_TEXT_TYPE_STREAM:  //ストリームを使う

    APP_PRINTSYS_COMMON_PrintStreamInit( &p_wk->common, APP_PRINTSYS_COMMON_TYPE_KEY);
    p_wk->p_keycursor  = APP_KEYCURSOR_Create( p_wk->clear_chr, TRUE, FALSE, p_wk->heapID );
    p_wk->p_stream  = PRINTSYS_PrintStream( p_wk->p_bmpwin, 0, 0, p_wk->p_strbuf,
        p_wk->p_font, MSGSPEED_GetWait(), p_wk->p_tcbl, 0, p_wk->heapID, p_wk->clear_chr );
    p_wk->print_update  = MYSTERY_TEXT_TYPE_STREAM;
    break;
  }

  p_wk->is_end_print  = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  プリント終了待ち  （QUEの場合はQUE終了、STREAMのときは最後まで文字描画終了）
 *
 *	@param	const MYSTERY_TEXT_WORK *cp_wk ワーク
 *
 *	@return TRUEならば文字描画完了  FALSEならば最中。
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_TEXT_IsEndPrint( const MYSTERY_TEXT_WORK *cp_wk )
{ 
  return cp_wk->is_end_print;
}

//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  枠を描画
 *
 *	@param	MYSTERY_TEXT_WORK *p_wk   ワーク
 *	@param  フレームのキャラ
 *	@param  フレームのパレット
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_WriteWindowFrame( MYSTERY_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt )
{ 
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );
}
//----------------------------------------------------------------------------
/**
 *	@brief  テキスト  待機アイコンを消去
 *
 *	@param	WBM_TEXT_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_TEXT_EndWait( MYSTERY_TEXT_WORK *p_wk )
{ 
  p_wk->is_end_print  = TRUE;
  if( p_wk->p_time )
  { 
    TIMEICON_Exit( p_wk->p_time );
    p_wk->p_time  = NULL;

    //タイムアイコンのあとはスクリーンが壊れているため、再転送
    GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(p_wk->p_bmpwin) );
  }
}
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  リスト
 *				    ・簡単にリストを出すために最大値決めうち
 *            ・表示数＝リスト最大数
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	定数
//=====================================/
#define MYSTERY_LIST_W  (12)
//-------------------------------------
///	選択リスト
//=====================================
struct _MYSTERY_LIST_WORK
{ 
  GFL_BMPWIN        *p_bmpwin;
  PRINT_QUE         *p_que;
  PRINT_UTIL        print_util;
  BMPMENULIST_WORK  *p_list;
  BMP_MENULIST_DATA *p_list_data;
};
//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  リスト初期化
 *
 *	@param	MYSTERY_LIST_SETUP *cp_setup  リスト設定ワーク
 *	@param	heapID                    heapID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_LIST_WORK * MYSTERY_LIST_Init( const MYSTERY_LIST_SETUP *cp_setup, HEAPID heapID )
{ 
  MYSTERY_LIST_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_LIST_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_LIST_WORK) );
  p_wk->p_que = cp_setup->p_que;

  //BMPWIN作成
  { 
    //右下、テキストボックスの上に位置するため
    //表示項目から位置、高さを計算
    const u8 w  = MYSTERY_LIST_W;
    const u8 h  = cp_setup->list_max * 2;
    const u8 x  = 32 - w - 1; //1はフレーム分
    const u8 y  = 24 - h - 1 - 6; //１は自分のフレーム分と6はテキスト分
    p_wk->p_bmpwin  = GFL_BMPWIN_Create( cp_setup->frm, x, y, w, h, cp_setup->font_plt, GFL_BMP_CHRAREA_GET_B );
    BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_OFF, cp_setup->frm_chr, cp_setup->frm_plt );
    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

    PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
  }
  //リストデータ作成
  { 
    int i;
    p_wk->p_list_data = BmpMenuWork_ListCreate( cp_setup->list_max, heapID );
    for( i = 0; i < cp_setup->list_max; i++ )
    { 
      BmpMenuWork_ListAddArchiveString( p_wk->p_list_data,
          cp_setup->p_msg, cp_setup->strID[ i ], i, heapID );
    }
  }
  //リスト作成
  { 
    static const BMPMENULIST_HEADER sc_def_header =
    { 
      NULL,
      NULL,
      NULL,
      NULL,

      0,  //count
      0,  //line
      0,  //rabel_x
      13, //data_x  
      0,  //cursor_x
      3,  //line_y
      1,  //f
      15, //b
      2,  //s
      0,  //msg_spc
      1,  //line_spc
      BMPMENULIST_NO_SKIP,  //page_skip
      0,  //font
      0,  //c_disp_f
      NULL,
      12,
      12,

      NULL,
      NULL,
      NULL,
      NULL,
      
    };
    BMPMENULIST_HEADER  header;
    header  = sc_def_header;

    header.list         = p_wk->p_list_data;
    header.count        = cp_setup->list_max;
    header.line         = cp_setup->list_max;
    header.win          = p_wk->p_bmpwin;
    header.msgdata      = cp_setup->p_msg;
    header.print_util   = &p_wk->print_util;
    header.print_que    = cp_setup->p_que;
    header.font_handle  = cp_setup->p_font;
    p_wk->p_list  = BmpMenuList_Set( &header, 0, 0, heapID );

    BmpMenuList_SetCursorBmp( p_wk->p_list, heapID );
    BmpMenuList_SetCancelMode( p_wk->p_list, BMPMENULIST_CANCELMODE_USE );
    
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  リスト破棄処理
 *
 *	@param	MYSTERY_LIST_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_LIST_Exit( MYSTERY_LIST_WORK *p_wk )
{ 
  BmpMenuList_Exit( p_wk->p_list, NULL, NULL );
  BmpMenuWork_ListDelete( p_wk->p_list_data );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  リストメイン処理
 *
 *	@param	MYSTERY_LIST_WORK *p_wk   ワーク
 *
 *	@return 選択していないならばMYSTERY_LIST_SELECT_NULL それ以外ならば選択したリストインデックス
 */
//-----------------------------------------------------------------------------
u32 MYSTERY_LIST_Main( MYSTERY_LIST_WORK *p_wk )
{ 
  u32 ret;
  PRINT_UTIL_Trans( &p_wk->print_util, p_wk->p_que );
  ret = BmpMenuList_Main( p_wk->p_list );

  if( ret == BMPMENULIST_CANCEL )
  { 
    //キャンセルのときは、
    //一番後尾を選んだことにする
    ret = BmpListParamGet( p_wk->p_list, BMPMENULIST_ID_COUNT ) - 1;
  }

  return ret;
}
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  BMPWINの集まり
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	メッセージウィンドウ
//=====================================
struct _MYSTERY_MSGWINSET_WORK
{ 
  const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl;
  u32 tbl_len;
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg;
  MYSTERY_MSGWIN_WORK *p_msgwin[0];
};
//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET 初期化
 *  
 *  @param  mode                                      反映モード
 *	@param	const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl 設定テーブル
 *	@param	tbl_len                                   テーブル数
 *	@param	frm                                       フレーム
 *	@param	plt                                       パレット
 *	@param	*p_que                                    キュー
 *	@param	*p_msg                                    メッセージ
 *	@param	*p_font                                   フォント
 *	@param	heapID                                    ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_MSGWINSET_WORK * MYSTERY_MSGWINSET_Init( MYSTERY_MSGWIN_TRANS_MODE mode, const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl, u32 tbl_len, u16 frm, u8 plt, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, GFL_FONT *p_font, HEAPID heapID )
{ 
  MYSTERY_MSGWINSET_WORK *p_wk;
  u32 size;

  size  = sizeof(MYSTERY_MSGWINSET_WORK) + sizeof(MYSTERY_MSGWIN_WORK *) * tbl_len;
  p_wk  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_wk, size );
  p_wk->cp_tbl  = cp_tbl;
  p_wk->tbl_len = tbl_len;
  p_wk->p_font  = p_font;
  p_wk->p_msg   = p_msg;

  //作成  ＆　書き込み
  { 
    int i;
    const MYSTERY_MSGWINSET_SETUP_TBL *cp_setup;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      cp_setup  = &cp_tbl[i];
      p_wk->p_msgwin[i] = MYSTERY_MSGWIN_Init( mode, frm, cp_setup->x, cp_setup->y, 
          cp_setup->w, cp_setup->h, plt, p_que, heapID );

      MYSTERY_MSGWIN_SetPos( p_wk->p_msgwin[i], cp_setup->pos_x, cp_setup->pos_y, cp_setup->pos_type );
      MYSTERY_MSGWIN_SetColor( p_wk->p_msgwin[i], cp_setup->color );

      if( cp_setup->p_strbuf )
      { 
        MYSTERY_MSGWIN_PrintBuf( p_wk->p_msgwin[i], cp_setup->p_strbuf, p_wk->p_font );
      }
      else
      { 
        MYSTERY_MSGWIN_Print( p_wk->p_msgwin[i], p_msg, cp_setup->strID, p_wk->p_font );
      }
    }
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  再描画  
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk            ワーク
 *	@param	MYSTERY_MSGWINSET_PRINT_TBL *cp_print   書き込みデータ
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_Print( MYSTERY_MSGWINSET_WORK* p_wk, const MYSTERY_MSGWINSET_PRINT_TBL *cp_tbl )
{ 
  //作成  ＆　書き込み
  { 
    int i;
    const MYSTERY_MSGWINSET_PRINT_TBL *cp_print;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      cp_print  = &cp_tbl[i];

      if( cp_print->is_print )
      { 
        MYSTERY_MSGWIN_SetPos( p_wk->p_msgwin[i], cp_print->pos_x, cp_print->pos_y, cp_print->pos_type );
        MYSTERY_MSGWIN_SetColor( p_wk->p_msgwin[i], cp_print->color );

        if( cp_print->p_strbuf )
        { 
          MYSTERY_MSGWIN_PrintBuf( p_wk->p_msgwin[i], cp_print->p_strbuf, p_wk->p_font );
        }
        else
        { 
          MYSTERY_MSGWIN_Print( p_wk->p_msgwin[i], p_wk->p_msg, cp_print->strID, p_wk->p_font );
        }
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET 破棄
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_Exit( MYSTERY_MSGWINSET_WORK* p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      MYSTERY_MSGWIN_Exit( p_wk->p_msgwin[i] );
    }
  }

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSETクリア
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_Clear( MYSTERY_MSGWINSET_WORK* p_wk )
{ 
  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      MYSTERY_MSGWIN_Clear( p_wk->p_msgwin[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET プリント街
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk  ワーク
 *
 *	@return TRUEで描画完了  FALSEで描画中
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_MSGWINSET_PrintMain( MYSTERY_MSGWINSET_WORK* p_wk )
{ 
  BOOL ret  = TRUE;

  { 
    int i;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      ret &= MYSTERY_MSGWIN_PrintMain( p_wk->p_msgwin[i] );
    }
  }

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  個別消去
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk  ワーク
 *	@param	idx                           インデックス
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_ClearOne( MYSTERY_MSGWINSET_WORK* p_wk, u16 idx )
{ 
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_msgwin[idx]->p_bmpwin),
      PRINTSYS_LSB_GetB( p_wk->p_msgwin[idx]->color ) );
  GFL_BMPWIN_TransVramCharacter( p_wk->p_msgwin[idx]->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BMPWINSET MYSTERY_MSGWIN_TRANS_MODE_AUTOモードの場合使用する転送関数
 *
 *	@param	MYSTERY_MSGWINSET_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_Trans( MYSTERY_MSGWINSET_WORK* p_wk )
{ 
  int i;
  for( i = 0; i < p_wk->tbl_len; i++ )
  { 
    MYSTERY_MSGWIN_Trans( p_wk->p_msgwin[i] );
  }
}
//=============================================================================
/**
 *    メモリ上に描画しておき、一気に書き込むためのバッファ
 */
//=============================================================================
struct _MYSTERY_MSGWINBUFF_WORK
{ 
  HEAPID heapID;
  MYSTERY_MSGWINSET_WORK  *p_winset;
  GFL_BMP_DATA  *p_bmp[0];
};

//----------------------------------------------------------------------------
/**
 *	@brief  バッファの作成
 *
 *	@param	MYSTERY_MSGWINSET_WORK *p_wk  ワーク
 *	@param	heapID                        ヒープID
 *
 *	@return バッファ
 */
//-----------------------------------------------------------------------------
MYSTERY_MSGWINBUFF_WORK *MYSTERY_MSGWINSET_CreateBuff( MYSTERY_MSGWINSET_WORK *p_wk, HEAPID heapID )
{ 
  u32 size;
  MYSTERY_MSGWINBUFF_WORK *p_buff;

  size    = sizeof(MYSTERY_MSGWINBUFF_WORK) + sizeof(GFL_BMP_DATA*) * p_wk->tbl_len;
  p_buff  = GFL_HEAP_AllocMemory( heapID, size );
  GFL_STD_MemClear( p_buff, size );
  p_buff->p_winset  = p_wk;
  p_buff->heapID    = heapID;

  { 
    int i;
    GFL_BMP_DATA  *p_src;
    for( i = 0; i < p_wk->tbl_len; i++ )
    { 
      p_src  = GFL_BMPWIN_GetBmp( p_wk->p_msgwin[i]->p_bmpwin );
      p_buff->p_bmp[i]  = GFL_BMP_Create( GFL_BMP_GetSizeX(p_src)/8, 
          GFL_BMP_GetSizeY(p_src)/8, GFL_BMP_GetColorFormat(p_src), heapID );
    }
  }

  return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief  バッファの破棄
 *
 *	@param	MYSTERY_MSGWINBUFF_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_DeleteBuff( MYSTERY_MSGWINBUFF_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < p_wk->p_winset->tbl_len; i++ )
  { 
    GFL_BMP_Delete( p_wk->p_bmp[i] );
  }

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  バッファへの書き込み
 *
 *	@param	MYSTERY_MSGWINBUFF_WORK  *p_wk        ワーク
 *	@param	MYSTERY_MSGWINSET_PRINT_TBL *cp_tbl 書き込みテーブル
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_PrintBuff( MYSTERY_MSGWINBUFF_WORK *p_wk, const MYSTERY_MSGWINSET_PRINT_TBL *cp_tbl, PRINT_QUE *p_que )
{ 
  //作成  ＆　書き込み
  { 
    int i;
    const MYSTERY_MSGWINSET_PRINT_TBL *cp_print;
    GFL_POINT ofs;
    GFL_POINT pos;

    STRBUF  *p_strbuf = GFL_STR_CreateBuffer( 512, p_wk->heapID );

    for( i = 0; i < p_wk->p_winset->tbl_len; i++ )
    { 
      cp_print  = &cp_tbl[i];

      if( cp_print->is_print )
      { 
        GFL_POINT pos;

        //一端消去
        GFL_BMP_Clear( p_wk->p_bmp[i], PRINTSYS_LSB_GetB( cp_print->color ) );	

        //位置計算
        ofs.x = cp_print->pos_x;
        ofs.y = cp_print->pos_y;
        Mystery_Msgwin_CalcPosCore( cp_print->pos_type, &ofs, p_wk->p_bmp[i], p_strbuf, p_wk->p_winset->p_font, &pos );

        //文字列作成
        if( cp_print->p_strbuf )
        { 
          GFL_STR_CopyBuffer( p_strbuf, cp_print->p_strbuf );
        }
        else
        { 
          GFL_MSG_GetString( p_wk->p_winset->p_msg, cp_print->strID, p_strbuf );
        }

        //文字列描画
        PRINTSYS_PrintQueColor( p_que, p_wk->p_bmp[i], pos.x, pos.y, p_strbuf, p_wk->p_winset->p_font, cp_print->color );
      }
    }

    GFL_HEAP_FreeMemory( p_strbuf );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  バッファの設定（転送はTrans関数で行なう）
 *  
 *	@param	MYSTERY_MSGWINSET_WORK *p_wk    ワーク
 *	@param	MYSTERY_MSGWINBUFF_WORK *cp_buff バッファ
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGWINSET_SetBuff( MYSTERY_MSGWINBUFF_WORK *p_wk )
{ 
  int i;
  GFL_BMP_DATA  *p_src_bmp;
  GFL_BMP_DATA  *p_dst_bmp;
  u8  *p_src;
  u8  *p_dst;


  for( i = 0; i < p_wk->p_winset->tbl_len; i++ )
  { 
    p_src_bmp = p_wk->p_bmp[i];
    p_dst_bmp = GFL_BMPWIN_GetBmp(p_wk->p_winset->p_msgwin[i]->p_bmpwin);
    GFL_BMP_Copy( p_src_bmp, p_dst_bmp );
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  メニュー
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	定数
//=====================================
static const GFL_POINT sc_menu_cursor_pos[] =
{ 
  { 
    128,16
  },
  { 
    128,44
  },
  { 
    128,72
  },
  { 
    128,104
  },
};

//-------------------------------------
///	選択メニュー
//=====================================
struct _MYSTERY_MENU_WORK
{ 
  MYSTERY_MSGWINSET_WORK *p_winset;
  GFL_CLWK          *p_cursor;
  u32               select;
  u32               list_max;
  u16								clear_chr;	//クリアキャラ
  u16               color[ MYSTERY_MENU_WINDOW_MAX ];
  u16               color_buff[ MYSTERY_MENU_WINDOW_MAX ];
  u16               change_color;
  u16               cnt;
  u16               font_plt;
  u16               font_frm;
  u16               bg_frm;
  u16               bg_ofs;
  s16               chr_y_ofs;
  u16               anm_seq;
  MYSTERY_MENU_CURSOR_CALLBACK callback;
  void              *p_wk_adrs;
  BOOL              is_blink[ MYSTERY_MENU_WINDOW_MAX ];
  BOOL              is_blink_req[ MYSTERY_MENU_WINDOW_MAX ];
  BOOL              is_cancel;
  MYSTERY_MENU_DATA *cp_menu_data;
};
//-------------------------------------
///	プロトタイプ
//=====================================
//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  メニュー  初期化
 *
 *	@param	const MYSTERY_MENU_SETUP *cp_setup  設定構造体
 *	@param	heapID                              ヒープID 
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_MENU_WORK * MYSTERY_MENU_Init( const MYSTERY_MENU_SETUP *cp_setup, HEAPID heapID )
{
  MYSTERY_MENU_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_MENU_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_MENU_WORK) );
  p_wk->list_max  = cp_setup->list_max;
  p_wk->p_cursor  = cp_setup->p_cursor;
  p_wk->font_plt  = cp_setup->font_plt;
  p_wk->bg_frm    = cp_setup->bg_frm;
  p_wk->bg_ofs    = cp_setup->bg_ofs;
  p_wk->font_frm  = cp_setup->font_frm;
  p_wk->callback  = cp_setup->callback;
  p_wk->p_wk_adrs = cp_setup->p_wk_adrs;
  p_wk->chr_y_ofs = cp_setup->chr_y_ofs;
  p_wk->anm_seq   = cp_setup->anm_seq;
  p_wk->cp_menu_data  = cp_setup->p_data;
  if( cp_setup->p_data )
  { 
    p_wk->select  = cp_setup->p_data->cursor_pos;
    p_wk->select  = MATH_CLAMP( p_wk->select, 0, p_wk->list_max-1 );
  }

  //各窓の背景色と決定色をコピー
  { 
    int i;

    for( i = 0; i < p_wk->list_max; i++ )
    {
      const u16* dst = (u16*)(HW_BG_PLTT+cp_setup->bg_ofs*0x20+(0xA+i)*2);
      p_wk->color[i]  = *dst;
    }
    p_wk->change_color  = *(u16*)(HW_BG_PLTT+cp_setup->bg_ofs*0x20+(0xF)*2);
  }

  //BMPWIN作成
  { 
    int i;
    MYSTERY_MSGWINSET_SETUP_TBL tbl[] =
    { 
      //一番上
      { 
        5,2,27,2, 0, NULL, MYSTERY_MSGWIN_POS_H_CENTER, 0, 0, PRINTSYS_MACRO_LSB( 0xf, 2, 0 )
      },
      //二番目
      {
        5,5,27,3, 0, NULL, MYSTERY_MSGWIN_POS_H_CENTER, 0, 0,PRINTSYS_MACRO_LSB( 0xf, 2, 0 )
      },
      //三番目
      {
        5,9,27,2, 0, NULL, MYSTERY_MSGWIN_POS_H_CENTER, 0, 0,PRINTSYS_MACRO_LSB( 0xf, 2, 0 )
      },
      //下
      {
        5,13,5,2, 0, NULL, MYSTERY_MSGWIN_POS_H_CENTER, 0, 0,PRINTSYS_MACRO_LSB( 0xf, 2, 0 )
      },
    };

    for( i = 0; i < p_wk->list_max; i++ )
    {
      //Yオフセット
      tbl[i].y  += cp_setup->chr_y_ofs;
      //文字列Xオフセット
      tbl[i].x  += cp_setup->chr_x_ofs_str;

      //文字設定
      if( cp_setup->p_msg )
      { 
        tbl[i].p_strbuf	= GFL_MSG_CreateString( cp_setup->p_msg, cp_setup->strID[ i ] );
      }
      else
      { 
        tbl[i].p_strbuf	= GFL_STR_CreateCopyBuffer( cp_setup->p_strbuf[i], GFL_HEAP_LOWID( heapID) );
      }
    }

    p_wk->p_winset  = MYSTERY_MSGWINSET_Init( MYSTERY_MSGWIN_TRANS_MODE_AUTO, tbl, p_wk->list_max, cp_setup->font_frm, cp_setup->font_plt, cp_setup->p_que, cp_setup->p_msg, cp_setup->p_font, heapID );

    for( i = 0; i < p_wk->list_max; i++ )
    {
      if( tbl[i].p_strbuf )
      { 
        GFL_STR_DeleteBuffer( tbl[i].p_strbuf );
      }
    }
  }

  //カーソル設定
  {
    GFL_CLACTPOS  pos;
    pos.x = sc_menu_cursor_pos[p_wk->select].x;
    pos.y = sc_menu_cursor_pos[p_wk->select].y + cp_setup->chr_y_ofs * 8;
    GFL_CLACT_WK_SetPos( p_wk->p_cursor, &pos, CLSYS_DEFREND_MAIN );
    GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, TRUE );
    GFL_CLACT_WK_SetAnmSeq( p_wk->p_cursor, p_wk->anm_seq );
    GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_cursor, TRUE );
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  メニュー  破棄
 *
 *	@param	MYSTERY_MENU_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MENU_Exit( MYSTERY_MENU_WORK *p_wk )
{ 
  if( p_wk->cp_menu_data ) 
  { 
    if( p_wk->select != 3 )
    { 
      p_wk->cp_menu_data->cursor_pos  = p_wk->select;
    }
  }

  GFL_CLACT_WK_SetDrawEnable( p_wk->p_cursor, FALSE );
  MYSTERY_MSGWINSET_Clear( p_wk->p_winset );
  MYSTERY_MSGWINSET_Exit( p_wk->p_winset );
  GFL_BG_LoadScreenReq( p_wk->font_frm );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  メニュー  メイン処理
 *
 *	@param	MYSTERY_MENU_WORK *p_wk   ワーク
 *
 *	@return 選択したもの
 */
//-----------------------------------------------------------------------------
u32 MYSTERY_MENU_Main( MYSTERY_MENU_WORK *p_wk )
{ 
  int trg = GFL_UI_KEY_GetTrg(); 
  int repeat  = GFL_UI_KEY_GetRepeat();
  BOOL is_update  = FALSE;

  //操作
  if( repeat & PAD_KEY_UP )
  { 
    if( p_wk->select == 0 )
    { 
      p_wk->select  = p_wk->list_max - 1;
    }
    else
    { 
      p_wk->select--;
    }
    if( p_wk->list_max != 1 )
    { 
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
    is_update  = TRUE;
  }
  else if( repeat & PAD_KEY_DOWN )
  { 
    if( p_wk->list_max != 1 )
    { 
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
    p_wk->select++;
    p_wk->select  %= p_wk->list_max;
    is_update  = TRUE;
  }
  else if( trg & PAD_BUTTON_DECIDE )
  { 
    PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    return p_wk->select;
  }
  else if( trg & PAD_BUTTON_CANCEL )
  { 
    PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
    if( p_wk->list_max == 1 )
    { 
      return MYSTERY_MENU_SELECT_CENCEL;
    }
    else
    { 
      if( p_wk->select != 3 )
      { 
        p_wk->cp_menu_data->cursor_pos  = p_wk->select;
      }
      p_wk->select  = p_wk->list_max-1;
      return p_wk->select;
    }
  }

  //アップデート
  if( is_update )
  {
    //カーソル設定
    {
      GFL_CLACTPOS  pos;
      pos.x = sc_menu_cursor_pos[p_wk->select].x;
      pos.y = sc_menu_cursor_pos[p_wk->select].y + p_wk->chr_y_ofs * 8;
      GFL_CLACT_WK_SetPos( p_wk->p_cursor, &pos, CLSYS_DEFREND_MAIN );

      if( p_wk->select == 3 )
      { 
        GFL_CLACT_WK_SetAnmSeq( p_wk->p_cursor, 2 );
      }
      else
      { 
        GFL_CLACT_WK_SetAnmSeq( p_wk->p_cursor, p_wk->anm_seq );
      }
    }

    //コールバック
    if( p_wk->callback )
    { 
      p_wk->callback( p_wk->p_wk_adrs );
    }
  }

  //背景色の変更
  { 
    int i;

    //パレットアニメ
    if( p_wk->cnt + 0x400 >= 0x10000 )
    {
      p_wk->cnt = p_wk->cnt+0x400-0x10000;
    }
    else
    {
      p_wk->cnt += 0x400;
    }

    for( i = 0; i < MYSTERY_MENU_WINDOW_MAX; i++ )
    { 
      //点滅同期
      if( p_wk->is_blink_req[i] != p_wk->is_blink[i] )
      { 
        if( 0x7FFF - 0x200 <= p_wk->cnt && p_wk->cnt <= 0x7FFF + 0x200  )
        { 
          p_wk->is_blink[i]  = p_wk->is_blink_req[i];
        }
      }

      //点滅処理
      if( p_wk->is_blink[ i ] )
      { 
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_MAIN, &p_wk->color_buff[i], p_wk->cnt,
            p_wk->bg_ofs, 0xA+i, p_wk->color[i], p_wk->change_color );
      }
    }
  }

  return MYSTERY_MENU_SELECT_NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  文字表示メイン
 *
 *	@param	MYSTERY_MENU_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_MENU_PrintMain( MYSTERY_MENU_WORK *p_wk )
{ 
  //文字描画
  return MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset );
}
//----------------------------------------------------------------------------
/**
 *	@brief  リストの背景を明滅
 *
 *	@param	MYSTERY_MENU_WORK *p_wk   ワーク
 *	@param	list_num                  明滅させるリスト
 *	@param  is_blink                  TRUEで明滅開始  FALSEで明滅終了
 */
//-----------------------------------------------------------------------------
void MYSTERY_MENU_SetBlink( MYSTERY_MENU_WORK *p_wk, u32 list_num, BOOL is_blink )
{
  GF_ASSERT( list_num < MYSTERY_MENU_WINDOW_MAX );
  p_wk->is_blink_req[ list_num ]  = is_blink;
}
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  シーケンス管理
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	シーケンスワーク
//=====================================
struct _MYSTERY_SEQ_WORK
{
	MYSTERY_SEQ_FUNCTION	seq_function;		//実行中のシーケンス関数
	BOOL is_end;									//シーケンスシステム終了フラグ
	int seq;											//実行中のシーケンス関数の中のシーケンス
	void *p_wk_adrs;							//実行中のシーケンス関数に渡すワーク
  int reserv_seq;               //予約シーケンス
};

//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	初期化
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	ワーク
 *	@param	*p_param				パラメータ
 *	@param	seq_function		シーケンス
 *
 */
//-----------------------------------------------------------------------------
MYSTERY_SEQ_WORK * MYSTERY_SEQ_Init( void *p_wk_adrs, MYSTERY_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
  MYSTERY_SEQ_WORK *p_wk;

	//作成
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_SEQ_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(MYSTERY_SEQ_WORK) );

	//初期化
	p_wk->p_wk_adrs	= p_wk_adrs;

	//セット
	MYSTERY_SEQ_SetNext( p_wk, seq_function );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	破棄
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_Exit( MYSTERY_SEQ_WORK *p_wk )
{
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	メイン処理
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_Main( MYSTERY_SEQ_WORK *p_wk )
{	
	if( !p_wk->is_end )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	終了取得
 *
 *	@param	const MYSTERY_SEQ_WORK *cp_wk		ワーク
 *
 *	@return	TRUEならば終了	FALSEならば処理中
 */	
//-----------------------------------------------------------------------------
BOOL MYSTERY_SEQ_IsEnd( const MYSTERY_SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	次のシーケンスを設定
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	ワーク
 *	@param	seq_function		シーケンス
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_SetNext( MYSTERY_SEQ_WORK *p_wk, MYSTERY_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	終了
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_End( MYSTERY_SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ 次のシーケンスを予約
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk  ワーク
 *	@param	seq             次のシーケンス
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_SetReservSeq( MYSTERY_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ 予約されたシーケンスへ飛ぶ
 *
 *	@param	MYSTERY_SEQ_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_SEQ_NextReservSeq( MYSTERY_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}
//=============================================================================
/**
 *    MSGOAM
 */
//=============================================================================
//-------------------------------------
///	定数
//=====================================
//-------------------------------------
///	メッセージOAM
//=====================================
struct _MYSTERY_MSGOAM_WORK
{ 
  GFL_BMP_DATA        *p_bmp;     //文字用BMP
  PRINTSYS_LSB        color;      //文字色
  BOOL                trans_req;  //BMP転送リクエストフラグ
  GFL_POINT           ofs;        //文字描画位置オフセット
  MYSTERY_MSGOAM_POS  pos_type;   //描画位置タイプ
  STRBUF              *p_strbuf;  //文字バッファ
  BMPOAM_ACT_PTR		  p_bmpoam_wk;  //BMPOAM
  PRINT_QUE           *p_que;       //描画キュー
};

//-------------------------------------
///	文字描画位置計算
//=====================================
static void Mystery_Msgoam_CalcPos( const MYSTERY_MSGOAM_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos );

//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  作成
 *
 *	@param	const GFL_CLWK_DATA *cp_data  OAM情報
 *	@param	w                             幅  キャラ単位
 *	@param	h                             高さ  キャラ単位
 *	@param	plt_idx                       GFL_CLGRPに登録したパレットインデックス
 *	@param  plt_ofs                       パレットオフセット
 *	@param	p_bmpoam_sys                  BMPOAMシステム
 *	@param	*p_que  メッセージ描画キュー
 *	@param	heapID  ヒープID
 *
 *	@return MSGOAMワーク
 */
//-----------------------------------------------------------------------------
MYSTERY_MSGOAM_WORK * MYSTERY_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID )
{ 
  MYSTERY_MSGOAM_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_MSGOAM_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_MSGOAM_WORK) );
  p_wk->p_que = p_que;

  //バッファ作成
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 128, heapID );

  //BMP作成
  p_wk->p_bmp = GFL_BMP_Create( w, h, GFL_BMP_16_COLOR, heapID );

	//BMPOAM作成
	{	
		BMPOAM_ACT_DATA	actdata;
		
		GFL_STD_MemClear( &actdata, sizeof(BMPOAM_ACT_DATA) );
		actdata.bmp	= p_wk->p_bmp;
		actdata.x		= cp_cldata->pos_x;
		actdata.y		= cp_cldata->pos_y;
		actdata.pltt_index	= plt_idx;
		actdata.soft_pri		= cp_cldata->softpri;
		actdata.setSerface	= draw_type;
		actdata.draw_type		= draw_type;
		actdata.bg_pri			= cp_cldata->bgpri;
    actdata.pal_offset  = plt_ofs;
		p_wk->p_bmpoam_wk	  = BmpOam_ActorAdd( p_bmpoam_sys, &actdata );
	}

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM　破棄
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGOAM_Exit( MYSTERY_MSGOAM_WORK* p_wk )
{ 
  BmpOam_ActorDel( p_wk->p_bmpoam_wk );
  GFL_BMP_Delete( p_wk->p_bmp );
  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  背景色でクリア
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGOAM_Clear( MYSTERY_MSGOAM_WORK* p_wk )
{
  GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB(p_wk->color) );
  BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  文字描画
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk ワーク
 *	@param	*p_msg                    メッセージ
 *	@param	strID                     文字番号
 *	@param	*p_font                   フォント
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGOAM_Print( MYSTERY_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//一端消去
	GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB( p_wk->color ) );	

	//文字列作成
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //位置計算
  Mystery_Msgoam_CalcPos( p_wk, p_font, &pos );

	//文字列描画
  PRINTSYS_PrintQueColor( p_wk->p_que, p_wk->p_bmp, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
  p_wk->trans_req = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  文字描画STRBUF版
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk     ワーク
 *	@param	STRBUF *cp_strbuf             文字列
 *	@param	*p_font                       フォント
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGOAM_PrintBuf( MYSTERY_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//一端消去
	GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB( p_wk->color ) );	

	//文字列作成
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //位置計算
  Mystery_Msgoam_CalcPos( p_wk, p_font, &pos );

	//文字列描画
  PRINTSYS_PrintQueColor( p_wk->p_que, p_wk->p_bmp, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
  p_wk->trans_req = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  文字色を設定
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk   ワーク
 *	@param	color                       文字色
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGOAM_SetStrColor( MYSTERY_MSGOAM_WORK* p_wk, PRINTSYS_LSB color )
{ 
  p_wk->color = color;
}

//----------------------------------------------------------------------------
/**
 *	@brief  文字描画位置設定
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk ワーク
 *	@param	x 座標
 *	@param	y 座標
 *	@param	type  座標タイプ
 */
//-----------------------------------------------------------------------------
void MYSTERY_MSGOAM_SetStrPos( MYSTERY_MSGOAM_WORK* p_wk, s16 x, s16 y, MYSTERY_MSGOAM_POS type )
{ 
  p_wk->ofs.x = x;
  p_wk->ofs.y = y;
  p_wk->pos_type  = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  表示処理
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk ワーク
 *
 *	@return TRUEで描画完了  FALSEで描画中
 */
//-----------------------------------------------------------------------------
BOOL MYSTERY_MSGOAM_PrintMain( MYSTERY_MSGOAM_WORK* p_wk )
{ 
  if( p_wk->trans_req )
  { 
    if( !PRINTSYS_QUE_IsExistTarget( p_wk->p_que, p_wk->p_bmp ) )
    { 
      BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
      p_wk->trans_req = FALSE;
    }
  }

  return !p_wk->trans_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM取得
 *
 *	@param	MYSTERY_MSGOAM_WORK* p_wk   ワーク
 *
 *	@return BMPOAM
 */
//-----------------------------------------------------------------------------
BMPOAM_ACT_PTR MYSTERY_MSGOAM_GetBmpOamAct( MYSTERY_MSGOAM_WORK* p_wk )
{ 
  return p_wk->p_bmpoam_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  描画座標計算
 *
 *	@param	const MYSTERY_MSGOAM_WORK* cp_wk  ワーク
 *	@param  フォント
 *	@param	*p_pos                            描画位置受け取り
 *
 */
//-----------------------------------------------------------------------------
static void Mystery_Msgoam_CalcPos( const MYSTERY_MSGOAM_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  switch( cp_wk->pos_type )
  { 
  case MYSTERY_MSGOAM_POS_ABSOLUTE:     //絶対位置
    *p_pos  = cp_wk->ofs;
    break;
  case MYSTERY_MSGOAM_POS_WH_CENTER:  //相対座標  幅、高さともに中央
    { 
      u32 x, y;
      x = GFL_BMP_GetSizeX( cp_wk->p_bmp ) / 2;
      y = GFL_BMP_GetSizeY( cp_wk->p_bmp ) / 2;
      x -= PRINTSYS_GetStrWidth( cp_wk->p_strbuf, p_font, 0 ) / 2;
      y -= PRINTSYS_GetStrHeight( cp_wk->p_strbuf, p_font ) / 2;
      p_pos->x  = x + cp_wk->ofs.x;
      p_pos->y  = y + cp_wk->ofs.y;
    }
    break;
  case MYSTERY_MSGOAM_POS_H_CENTER:    //相対座標  高さのみ中央
    { 
      u32 y;
      y = GFL_BMP_GetSizeY( cp_wk->p_bmp ) / 2;
      y -= PRINTSYS_GetStrHeight( cp_wk->p_strbuf, p_font ) / 2;
      p_pos->x  = cp_wk->ofs.x;
      p_pos->y  = y + cp_wk->ofs.y;
    }
    break;
  }
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ETC
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェード
 *
 *	@param	NNS_GFD_DST_TYPE type 転送先
 *	@param	*p_buff               バッファ
 *	@param	cnt                   カウント
 *	@param	plt_num               パレット縦
 *	@param	plt_col               パレット横
 *	@param	start                 開始色
 *	@param	end                   終了色
 */
//-----------------------------------------------------------------------------
void MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{ 
  //0〜１に変換
  const fx16 cos = (FX_CosIdx(cnt)+FX16_ONE)/2;
  const u8 start_r  = (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  const u8 start_g  = (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  const u8 start_b  = (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
  const u8 end_r  = (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  const u8 end_g  = (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  const u8 end_b  = (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

  const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
  const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
  const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);


  *p_buff = GX_RGB(r, g, b);

  { 
    BOOL ret;
    ret = NNS_GfdRegisterNewVramTransferTask( type,
        plt_num * 32 + plt_col * 2,
        p_buff, 2 );

    GF_ASSERT(ret);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェード１行すべて塗り替える版
 *
 *	@param	NNS_GFD_DST_TYPE type 転送先
 *	@param	*p_buff               バッファ0x10個分
 *	@param	cnt                   カウント
 *	@param	plt_num               パレット行
 *	@param	GXRgb start[]         開始色0x10個分
 *	@param	end[]                 終了色0x10個分
 */
//-----------------------------------------------------------------------------
void MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, GXRgb start[], GXRgb end[] )
{ 
  int i;
 //0〜１に変換
  for( i = 0; i < 0x10; i++ )
  { 
    const fx16 cos = (FX_CosIdx(cnt)+FX16_ONE)/2;
    const u8 start_r  = (start[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 start_g  = (start[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 start_b  = (start[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 end_r  = (end[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 end_g  = (end[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 end_b  = (end[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);

    p_buff[i] = GX_RGB(r, g, b);
  }

  { 
    BOOL ret;
    ret = NNS_GfdRegisterNewVramTransferTask( type,
        plt_num * 32,
        p_buff, 32 );
    GF_ASSERT(ret);
  }

}
