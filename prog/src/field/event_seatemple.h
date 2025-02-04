//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_seatemple.h
 *	@brief  海底遺跡　イベント起動
 *	@author	tomoya takahashi 
 *	@date		2010.03.11
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap_proc.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern GMEVENT * EVENT_SeaTemple_GetStepEvent(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap );


// 遷移
extern GMEVENT * EVENT_SeaTemple_GetDivingDownEvent(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap );

extern GMEVENT * EVENT_SeaTemple_GetDivingUpEvent(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BOOL kurukuru );


#ifdef _cplusplus
}	// extern "C"{
#endif



