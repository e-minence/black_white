////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ポケモンリーグフロントでのイベント
 * @file   event_league_front02.h
 * @author obata
 * @date   2010.04.09
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>

//------------------------------------------------------------------------------------------
/**
 * @brief リフト降下イベントを作成する
 * 
 * @param gsys     ゲームシステム
 * @param fieldmap フィールドマップ
 *
 * @return リフト降下イベント
 */
//------------------------------------------------------------------------------------------
extern GMEVENT* EVENT_LFRONT02_LiftDown( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );

