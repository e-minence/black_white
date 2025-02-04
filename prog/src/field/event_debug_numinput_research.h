////////////////////////////////////////////////////////////////////////
/**
 *  @brief  すれ違い調査関連の数値入力
 *  @file   event_debug_numinput_research.h
 *  @author obata
 *  @date   2010.04.16
 */
//////////////////////////////////////////////////////////////////////// 
#pragma once
#include "event_debug_numinput.h"


//======================================================================
// ■オーバーレイID extern 宣言
//======================================================================
FS_EXTERN_OVERLAY( d_numinput_research );


//----------------------------------------------------------------------
/**
 * 数値入力のジャンル選択メニューイベント生成
 */
//----------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_FLDMENU_ResearchNumInput( GAMESYS_WORK* gsys, void* work, int page );

//----------------------------------------------------------------------
/**
 * @brief 数値入力メイン処理
 * @param gsys
 * @param dni_param
 * @param id
 */
//----------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_ResearchNumInput(
    GAMESYS_WORK * gsys, const DEBUG_NUMINPUT_PARAM* dni_param, u32 id ); 
