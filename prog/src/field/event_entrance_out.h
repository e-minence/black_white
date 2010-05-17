/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_out.h
 * @breif  出入口からの退出時のイベント作成
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "entrance_event_common.h"

//---------------------------------------------------------------------------------------
/**
 * @brief 出入口退出イベントを作成する
 *
 * @param evdata 出入り口イベント共通ワーク
 *
 * @return 作成したイベント
 */
//---------------------------------------------------------------------------------------
extern GMEVENT* EVENT_EntranceOut( ENTRANCE_EVDATA* evdata );
