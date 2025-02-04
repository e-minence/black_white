//==============================================================================
/**
 * @file    event_intrude_subscreen.h
 * @brief   簡単な説明を書く
 * @author  matsuda
 * @date    2009.10.18(日)
 */
//==============================================================================
#pragma once


//==============================================================================
//  外部関数宣言
//==============================================================================
extern GMEVENT * EVENT_ChangeIntrudeSubScreen(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_SUBSCREEN_MODE change_mode);
extern GMEVENT * EVENT_IntrudeTownWarp(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 zone_id, BOOL partner);
extern GMEVENT * EVENT_IntrudePlayerWarp(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, int player_netid);
extern GMEVENT * EVENT_ChangeSubScreen_to_CGear(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_SUBSCREEN_WORK *subscreen);

