//==============================================================================
/**
 * @file  symbol_map.h
 * @brief シンボルエンカウント用マップ関連
 * @author  tamada GAME FREAK inc.
 * @date  2010.03.19
 */
//==============================================================================

#pragma once

//--------------------------------------------------------------
//--------------------------------------------------------------
extern u16 SYMBOLMAP_GetZoneID( SYMBOL_MAP_ID symmap_id );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern SYMBOL_MAP_ID SYMBOLMAP_GetNextSymbolMapID( SYMBOL_MAP_ID now_symmap_id, u16 dir_id );

extern BOOL SYMBOLMAP_IsEntranceID( SYMBOL_MAP_ID symmap_id );
extern BOOL SYMBOLMAP_IsKeepzoneID( SYMBOL_MAP_ID symmap_id );
