///////////////////////////////////////////////////////////////////////////
/**
 * @brief  ゲーム内マニュアルの呼び出しイベント
 * @file   event_game_manual.c
 * @author obata
 * @date   2010.04.27
 */
///////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h" 
#include "gamesystem/game_event.h" 


//-------------------------------------------------------------------------
/**
 * @brief ゲーム内マニュアルの呼び出しイベントを生成する
 *
 * @param gameSystem
 *
 * @return ゲーム内マニュアル呼び出しイベント
 */
//-------------------------------------------------------------------------
extern GMEVENT* EVENT_GameManual( GAMESYS_WORK* gameSystem );
