//============================================================================
/**
 *  @file   manual.h
 *  @brief  ゲーム内マニュアル
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  モジュール名：MANUAL_TOUCHBAR
 */
//============================================================================
#pragma once


// インクルード
#include <gflib.h>

#include "gamesystem/gamedata_def.h"

#include "manual_common.h"


//=============================================================================
/**
*  定数定義
*/
//=============================================================================


//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
//-------------------------------------
/// ワーク
//=====================================
typedef struct _MANUAL_TOUCHBAR_WORK MANUAL_TOUCHBAR_WORK;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================
// 初期化処理
extern  MANUAL_TOUCHBAR_WORK*  MANUAL_TOUCHBAR_Init(
    MANUAL_COMMON_WORK*  cmn_wk
);
// 終了処理
extern  void  MANUAL_TOUCHBAR_Exit(
    MANUAL_TOUCHBAR_WORK*     work
);
// 主処理
extern  void  MANUAL_TOUCHBAR_Main(
    MANUAL_TOUCHBAR_WORK*     work
);

