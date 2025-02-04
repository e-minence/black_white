/*---------------------------------------------------------------------------*
  Project:  TwlSDK - HW - include
  File:     lcd.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/

#ifndef NITRO_HW_COMMON_LCD_H_
#define NITRO_HW_COMMON_LCD_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/
// 水平ドット数
#define HW_LCD_WIDTH                256 // 表示画面幅ドット数
#define HW_LCD_HBLANK               99 // Hブランク期間ドット数
#define HW_LCD_COLUMNS              ( HW_LCD_WIDTH + HW_LCD_HBLANK )

// 垂直ライン数
#define HW_LCD_HEIGHT               192 // 表示画面高ライン数
#define HW_LCD_VBLANK               71 // Vブランク期間ライン数
#define HW_LCD_LINES                ( HW_LCD_HEIGHT + HW_LCD_VBLANK )

// LCDクロック
#define HW_LCD_CLOCK                33513982    // 33.513982MHz
#define HW_LCD_CLOCK_PER_DOT        6  // LCDクロック 6分周

// Hサイクル( 63.5556us )
#define HW_LCD_H_CYCLE_NS           ((u32)( 1000000000ULL * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))
#define HW_LCD_H_CYCLE_US           ((u32)(    1000000ULL * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))
#define HW_LCD_H_CYCLE_MS           ((u32)(       1000ULL * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK )

// Vサイクル( 16.7151ms )
#define HW_LCD_V_CYCLE_NS           ((u32)( 1000000000ULL * HW_LCD_LINES * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))
#define HW_LCD_V_CYCLE_US           ((u32)(    1000000ULL * HW_LCD_LINES * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))
#define HW_LCD_V_CYCLE_MS           ((u32)(       1000ULL * HW_LCD_LINES * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))

// nライン走査に要する時間
#define HW_LCD_LINES_CYCLE_NS(n)    ((u32)( 1000000000ULL * (n) * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))
#define HW_LCD_LINES_CYCLE_US(n)    ((u32)(    1000000ULL * (n) * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))
#define HW_LCD_LINES_CYCLE_MS(n)    ((u32)(       1000ULL * (n) * HW_LCD_COLUMNS * HW_LCD_CLOCK_PER_DOT / HW_LCD_CLOCK ))


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_HW_COMMON_LCD_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
