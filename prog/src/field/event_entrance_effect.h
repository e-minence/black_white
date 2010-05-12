//============================================================================================
/**
 * @file field_door_anime.h
 * @brief
 * @date  2009.07.30
 * @author  tamada GAME FREAK inc.
 *
 */
//============================================================================================
#pragma once
#include "event_field_fade.h" // for FIELD_FADE_TYPE

extern GMEVENT * EVENT_FieldDoorInAnime ( GAMESYS_WORK * gsys, 
                                          FIELDMAP_WORK * fieldmap, 
                                          u16 prevZoneID, u16 nextZoneID,
                                          BOOL cam_anm_flag, 
                                          BOOL season_disp_flag,
                                          FIELD_FADE_TYPE fadeType,
                                          EXIT_TYPE exitType );

extern GMEVENT * EVENT_FieldDoorOutAnime ( GAMESYS_WORK * gsys, 
                                           FIELDMAP_WORK * fieldmap, 
                                           BOOL cam_anm_flag, 
                                           BOOL season_disp_flag, 
                                           u8 start_season,
                                           u8 end_season,
                                           FIELD_FADE_TYPE fadeType,
                                           EXIT_TYPE exitType );

// 自機の一歩先の座標を取得する
extern void EVENT_ENTRANCE_EFFECT_GetPlayerFrontPos( FIELDMAP_WORK* fieldmap, VecFx32* dest );
