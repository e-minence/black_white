//============================================================================
/**
 *  @file   zukan_info.h
 *  @brief  図鑑情報
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   infowin.h, touchbar.h, ui_template.hを参考にして作成しました。
 *
 *  モジュール名：ZUKAN_INFO
 */
//============================================================================
#pragma once

// lib
#include <gflib.h>

// system
#include "gamesystem/gamesystem.h"

//=============================================================================
/**
 *  定数定義
 */
//=============================================================================
//-------------------------------------
/// 表示面
//=====================================
typedef enum
{
  ZUKAN_INFO_DISP_M,
  ZUKAN_INFO_DISP_S,
}
ZUKAN_INFO_DISP;

//-------------------------------------
/// 起動方法
//=====================================
typedef enum
{
  ZUKAN_INFO_LAUNCH_TOROKU,
  ZUKAN_INFO_LAUNCH_NICKNAME,
  //ZUKAN_INFO_LAUNCH_LIST,
}
ZUKAN_INFO_LAUNCH;

//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
//-------------------------------------
/// ワーク
//=====================================
typedef struct _ZUKAN_INFO_WORK ZUKAN_INFO_WORK;

//=============================================================================
/**
 *  関数宣言
 */
//=============================================================================
//-------------------------------------
/// 初期化処理
//=====================================
extern ZUKAN_INFO_WORK* ZUKAN_INFO_Init( HEAPID a_heap_id, POKEMON_PARAM* a_pp,
                                         ZUKAN_INFO_LAUNCH a_launch,
                                         ZUKAN_INFO_DISP a_disp, u8 a_bg_priority,
                                         GFL_CLUNIT* a_clunit,
                                         GFL_FONT* a_font,
                                         PRINT_QUE* a_print_que );

//-------------------------------------
/// 終了処理
//=====================================
extern void ZUKAN_INFO_Exit( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// 主処理
//=====================================
extern void ZUKAN_INFO_Main( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// ポケモン2D以外のものを破棄する
//=====================================
extern void ZUKAN_INFO_DeleteOthers( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// ポケモン2Dの移動開始許可フラグを立てる
//=====================================
extern void ZUKAN_INFO_Move( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// ポケモンの鳴き声フラグを立てる
//=====================================
extern void ZUKAN_INFO_Cry( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// ポケモン2Dが移動中か否か
//=====================================
extern BOOL ZUKAN_INFO_IsMove( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// ポケモン2Dが中央でじっとしているか否か
//=====================================
extern BOOL ZUKAN_INFO_IsCenterStill( ZUKAN_INFO_WORK* work );

//-------------------------------------
/// 処理を開始させる
//=====================================
extern void ZUKAN_INFO_Start( ZUKAN_INFO_WORK* work );
