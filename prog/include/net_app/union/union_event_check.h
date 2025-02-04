//==============================================================================
/**
 * @file    union_event_check.h
 * @brief   ユニオン：イベント起動チェックのヘッダ
 * @author  matsuda
 * @date    2009.07.09(木)
 */
//==============================================================================
#pragma once

#include "net_app/union/union_main.h"


//==============================================================================
//  外部関数宣言
//==============================================================================
extern GMEVENT * UnionEvent_CommCheck( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
extern GMEVENT * UnionEvent_TalkCheck( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

