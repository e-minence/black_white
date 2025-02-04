/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  BmpWin + 文字列描画 パッケージオブジェクト
 * @file   bg_font.h
 * @author obata
 * @date   2010.02.09
 */
///////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "bg_font.h"
#include "research_common.h"
#include "research_common_def.h"
#include "print/printsys.h"  // for PRINTSYS_xxxx


//===============================================================================
// ■BG_FONT
//===============================================================================
struct _BG_FONT {

  BG_FONT_PARAM param;    // 基本パラメータ
  GFL_FONT*     font;     // 使用するフォント
  GFL_MSGDATA*  message;  // 参照するメッセージデータ
  GFL_BMPWIN*   bmpWin;   // ビットマップウィンドウ
  BOOL          draw_enable_flag; // 描画フラグ
};


//===============================================================================
// ■プロトタイプ宣言
//=============================================================================== 
// センタリングするためのx描画オフセットを計算する
static int CalcXOffsetForCentering( const BG_FONT* BGFont, const STRBUF* str );



//-------------------------------------------------------------------------------
/**
 * @brief BG_FONT オブジェクトを生成する
 *
 * @param param   生成オブジェクトのパラメータ
 * @param font    使用するフォント
 * @param message 参照するメッセージデータ
 * @param heapID  使用するヒープID
 *
 * @return 生成した BG_FONT オブジェクト
 */
//-------------------------------------------------------------------------------
BG_FONT* BG_FONT_Create( const BG_FONT_PARAM* param, 
                         GFL_FONT* font, GFL_MSGDATA* message, HEAPID heapID )
{
  BG_FONT* BGFont;

  // 生成
  BGFont = GFL_HEAP_AllocMemory( heapID, sizeof(BG_FONT) );

  // 初期化
  BGFont->param   = *param;
  BGFont->font    = font;
  BGFont->message = message;
  BGFont->bmpWin  = GFL_BMPWIN_Create( param->BGFrame, 
                                       param->posX, param->posY, 
                                       param->sizeX, param->sizeY,
                                       param->paletteNo,
                                       GFL_BMP_CHRAREA_GET_F ); 
  BGFont->draw_enable_flag = TRUE;
  return BGFont;
}

//-------------------------------------------------------------------------------
/**
 * @brief BG_FONT オブジェクトを破棄する
 *
 * @param BGFont
 */
//-------------------------------------------------------------------------------
void BG_FONT_Delete( BG_FONT* BGFont )
{
  GF_ASSERT( BGFont );
  GF_ASSERT( BGFont->bmpWin );

  GFL_BMPWIN_Delete( BGFont->bmpWin );
  GFL_HEAP_FreeMemory( BGFont );
}

//-------------------------------------------------------------------------------
/**
 * @brief 文字列を設定する
 *
 * @param BGFont
 * @param strID  書き込む文字列ID
 */
//-------------------------------------------------------------------------------
void BG_FONT_SetMessage( BG_FONT* BGFont, u32 strID )
{
  STRBUF* strbuf;

  // 文字列を取得
  strbuf = GFL_MSG_CreateString( BGFont->message, strID );

  // 書き込み
  BG_FONT_SetString( BGFont, strbuf );

  GFL_HEAP_FreeMemory( strbuf );
}

//-------------------------------------------------------------------------------
/**
 * @brief 文字列を設定する
 *
 * @param BGFont
 * @param strbuf 書き込む文字列
 */
//------------------------------------------------------------------------------- 
void BG_FONT_SetString( BG_FONT* BGFont, const STRBUF* strbuf )
{
  BG_FONT_PARAM* param;
  GFL_BMP_DATA*  bmpData;
  PRINTSYS_LSB   color;
  int xOffset, yOffset;

  param   = &(BGFont->param);
  color   = PRINTSYS_LSB_Make( param->colorNo_L, param->colorNo_S, param->colorNo_B );
  bmpData = GFL_BMPWIN_GetBmp( BGFont->bmpWin );

  // クリア
  GFL_BMP_Clear( bmpData, BGFont->param.colorNo_B );

  // 書き込みオフセット座標を決定
  if( BGFont->param.centeringFlag ) {
    // センタリングあり
    xOffset = CalcXOffsetForCentering( BGFont, strbuf );
    yOffset = param->offsetY;
  }
  else { 
    // センタリングなし
    xOffset = param->offsetX;
    yOffset = param->offsetY;
  }

  // 書き込み
  PRINTSYS_PrintColor( bmpData, xOffset, yOffset, strbuf, BGFont->font, color ); 

  if( BGFont->draw_enable_flag ) {
    // VRAMへ転送
    GFL_BMPWIN_MakeTransWindow_VBlank( BGFont->bmpWin );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief 表示・非表示を設定する
 *
 * @param BGFont
 * @param enable 表示する場合TRUE, 非表示にする場合FALSE
 */
//-------------------------------------------------------------------------------
void BG_FONT_SetDrawEnable( BG_FONT* BGFont, BOOL enable )
{ 
  BOOL now = BGFont->draw_enable_flag;

  if( !now && enable ) {
    // スクリーンを作成
    GFL_BMPWIN_MakeTransWindow_VBlank( BGFont->bmpWin );
  }
  else if( now && !enable ) {
    // スクリーンをクリア
    GFL_BMPWIN_ClearTransWindow_VBlank( BGFont->bmpWin );
  }

  BGFont->draw_enable_flag = enable;
}

//-------------------------------------------------------------------------------
/**
 * @brief パレットナンバーを変更する
 *
 * @param BGFont
 * @param palnum パレットナンバー
 */
//-------------------------------------------------------------------------------
extern void BG_FONT_SetPalette( BG_FONT* BGFont, u8 palnum )
{
  GF_ASSERT( palnum < 16 ); // 不正値

  GFL_BMPWIN_SetPalette( BGFont->bmpWin, palnum ); // パレットを変更
  //GFL_BMPWIN_MakeScreen( BGFont->bmpWin ); // スクリーンを作成
  GFL_BG_ChangeScreenPalette( 
      GFL_BMPWIN_GetFrame( BGFont->bmpWin ), 
      GFL_BMPWIN_GetPosX( BGFont->bmpWin ),
      GFL_BMPWIN_GetPosY( BGFont->bmpWin ),
      GFL_BMPWIN_GetScreenSizeX( BGFont->bmpWin ),
      GFL_BMPWIN_GetScreenSizeY( BGFont->bmpWin ),
      palnum ); // 該当スクリーンのパレットを変更

  if( BGFont->draw_enable_flag ) {
    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame( BGFont->bmpWin ) ); // VRAMへ転送
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief センタリングするためのx描画オフセットを計算する
 *
 * @param BGFont 書き込み対象
 * @param str    書き込む文字列
 *
 * @return センタリングして書き込むためのx描画オフセット
 */
//-------------------------------------------------------------------------------
static int CalcXOffsetForCentering( const BG_FONT* BGFont, const STRBUF* str )
{
  int targetWidth; // 書き込み先の幅
  int strWidth;    // 文字列の幅
  int offset;

  targetWidth = GFL_BMPWIN_GetSizeX( BGFont->bmpWin ) * DOT_PER_CHARA;
  strWidth    = PRINTSYS_GetStrWidth( str, BGFont->font, 0 );

  // 書き込み先よりも文字列の幅が大きい
  if( targetWidth < strWidth ) {
    OS_Printf( "---------------------------------------------------\n" );
    OS_Printf( "RESEARCH-RADAR: bitmap width (%d) < string width (%d)\n", targetWidth, strWidth );
    OS_Printf( "---------------------------------------------------\n" );
  }

  // センタリングのためのオフセット値を算出
  offset = (targetWidth - strWidth) * 0.5f; 

  // オフセットがマイナスにならないよう補正
  if( offset < 0 ){ offset = 0; }

  return offset;
}
