/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  調査レーダー 調査内容変更画面
 * @file   research_select.h
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "research_common.h"       // for RESEARCH_COMMON_WORK


//===============================================================================
// □定数
//=============================================================================== 
// メイン関数 終了結果
typedef enum {
  RESEARCH_SELECT_RESULT_NONE,      // ダミー
  RESEARCH_SELECT_RESULT_CONTINUE,  // 画面継続
  RESEARCH_SELECT_RESULT_TO_MENU,   // 調査報告初期画面へ
} RESEARCH_SELECT_RESULT;


//=============================================================================== 
// □調査初期画面 ワーク
//=============================================================================== 
typedef struct _RESEARCH_SELECT_WORK RESEARCH_SELECT_WORK;


//=============================================================================== 
// □調査初期画面 制御関数
//=============================================================================== 
// ワーク生成/破棄
RESEARCH_SELECT_WORK* CreateResearchSelectWork( RESEARCH_COMMON_WORK* commonWork );
void DeleteResearchSelectWork( RESEARCH_SELECT_WORK* work );

// メイン動作
RESEARCH_SELECT_RESULT ResearchSelectMain( RESEARCH_SELECT_WORK* work );
