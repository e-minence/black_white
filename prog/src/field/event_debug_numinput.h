/*
 *  @file   event_debug_numinput.c
 *  @brief  デバッグ数値入力
 *  @author Miyuki Iwasawa
 *  @date   09.11.25
 */

#pragma once

//オーバーレイID extern
FS_EXTERN_OVERLAY( d_numinput );

//--------------------------------------------------------------
/**
 * 数値入力デバッグメニューイベント生成
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=イベント継続
 */
//--------------------------------------------------------------
extern GMEVENT* EVENT_DMenuNumInput( GAMESYS_WORK* gsys, void* work );

