/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  調査レーダー 初期画面 ( メニュー画面 )
 * @file   research_menu.h
 * @author obata
 * @date   2010.02.03
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "research_common.h"       // for RESEARCH_COMMON_WORK


//===============================================================================
// ■定数
//=============================================================================== 
// メイン関数の終了結果
typedef enum {
  RESEARCH_MENU_RESULT_NONE,       // ダミー
  RESEARCH_MENU_RESULT_CONTINUE,   // 画面継続
  RESEARCH_MENU_RESULT_TO_SELECT,  // 調査内容変更画面へ
  RESEARCH_MENU_RESULT_TO_CHECK,   // 調査報告確認画面へ
  RESEARCH_MENU_RESULT_EXIT,       // 調査レーダー終了
} RESEARCH_MENU_RESULT;


//=============================================================================== 
// ■調査初期画面ワークの不完全型
//=============================================================================== 
typedef struct _RESEARCH_MENU_WORK RESEARCH_MENU_WORK;


//=============================================================================== 
// ■調査初期画面ワークの生成・破棄
//=============================================================================== 
// ワークを生成する
RESEARCH_MENU_WORK* CreateResearchMenuWork( RESEARCH_COMMON_WORK* commonWork );
// ワークを破棄する
void DeleteResearchMenuWork( RESEARCH_MENU_WORK* work ); 

//=============================================================================== 
// ■調査初期画面の動作
//=============================================================================== 
// メイン動作
RESEARCH_MENU_RESULT ResearchMenuMain( RESEARCH_MENU_WORK* work );
