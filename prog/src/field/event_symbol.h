//==============================================================================
/**
 * @file    event_symbol.h
 * @brief   �N���F�V���{���|�P����
 * @author  matsuda
 * @date    2010.03.16(��)
 */
//==============================================================================
#pragma once

#include "savedata/symbol_save.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * EVENT_SymbolPokeBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
    POKEMON_PARAM *pp, u16 *result_ptr, HEAPID heap_id);
extern GMEVENT * EVENT_ReqIntrudeSymbolParam(GAMESYS_WORK *gsys, u16 *result_ptr, SYMBOL_MAP_ID symbol_map_id);
extern GMEVENT * EVENT_SymbolMapWarp(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 *result_ptr, const VecFx32 *warp_pos, u16 warp_dir, SYMBOL_MAP_ID symbol_map_id);
extern GMEVENT * EVENT_SymbolMapWarpEasy(
    GAMESYS_WORK * gsys, u16 warp_dir, SYMBOL_MAP_ID symbol_map_id );
