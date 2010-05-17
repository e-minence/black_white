/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_in.h
 * @breif  出入口への進入時のイベント作成
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "entrance_event_common.h"

//---------------------------------------------------------------------------------------
/**
 * @brief 出入口進入イベントを作成する
 *
 * @param evdata 出入り口イベントの共通データ
 *
 * @return 作成したイベント
 */
//---------------------------------------------------------------------------------------
extern GMEVENT* EVENT_EntranceIn( ENTRANCE_EVDATA* evdata );
