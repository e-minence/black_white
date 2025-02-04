/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  BmpWin + 文字列描画 パッケージオブジェクト
 * @file   bg_font.h
 * @author obata
 * @date   2010.02.09
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "print/gf_font.h"


//===============================================================================
// □不完全形宣言
//===============================================================================
typedef struct _BG_FONT BG_FONT;


//===============================================================================
// □BG_FONT オブジェクト生成パラメータ
//===============================================================================
typedef struct
{
  u8   BGFrame;       // 対象BGフレーム
  u8   posX;          // x座標 (キャラクター単位)
  u8   posY;          // x座標 (キャラクター単位)
  u8   sizeX;         // xサイズ (キャラクター単位)
  u8   sizeY;         // yサイズ (キャラクター単位)
  u8   offsetX;       // 書き込み先x座標 (ドット単位)
  u8   offsetY;       // 書き込み先y座標 (ドット単位)
  u8   paletteNo;     // 使用パレット番号
  u8   colorNo_L;     // 文字のカラー番号
  u8   colorNo_S;     // 影のカラー番号
  u8   colorNo_B;     // 背景のカラー番号
  BOOL centeringFlag; // センタリングするかどうか

} BG_FONT_PARAM; 


//===============================================================================
// □
//===============================================================================
// 生成・破棄
extern BG_FONT* BG_FONT_Create( 
    const BG_FONT_PARAM* param, GFL_FONT* font, GFL_MSGDATA* message, HEAPID heapID );
extern void BG_FONT_Delete( BG_FONT* BGFont );

// 文字列の設定
extern void BG_FONT_SetMessage( BG_FONT* BGFont, u32 strID );
extern void BG_FONT_SetString( BG_FONT* BGFont, const STRBUF* strbuf );

// 表示・非表示の設定
extern void BG_FONT_SetDrawEnable( BG_FONT* BGFont, BOOL enable );

// パレットナンバーを変更する
extern void BG_FONT_SetPalette( BG_FONT* BGFont, u8 palnum );
