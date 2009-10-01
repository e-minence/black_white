//======================================================================
/**
 * @file	field_player_grid.c
 * @brief グリッド専用 フィールドプレイヤー制御
 * @author kagaya
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "field_player.h"

#include "field_effect.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
/// FIELD_PLAYER_GRID_REQBIT
//--------------------------------------------------------------
typedef enum
{
  ///自機を２速歩行に
  FIELD_PLAYER_GRID_REQBIT_NORMAL = (1<<0),
  ///自機を自転車に
  FIELD_PLAYER_GRID_REQBIT_CYCLE  = (1<<1),
  ///自機を波乗り状態に
  FIELD_PLAYER_GRID_REQBIT_SWIM = (1<<2),
}FIELD_PLAYER_GRID_REQBIT;

#define FIELD_PLAYER_GRID_REQBIT_MAX (3)

//======================================================================
//	struct
//======================================================================
///FIELD_PLAYER_GRID
typedef struct _TAG_FIELD_PLAYER_GRID FIELD_PLAYER_GRID;

//======================================================================
//	extern
//======================================================================
extern FIELD_PLAYER_GRID * FIELD_PLAYER_GRID_Init(
		FIELD_PLAYER *fld_player, HEAPID heapID );
extern void FIELD_PLAYER_GRID_Delete( FIELD_PLAYER_GRID *g_jiki );

extern void FIELD_PLAYER_GRID_Move(
		FIELD_PLAYER_GRID *g_jiki, int key_trg, int key_cont );

extern PLAYER_MOVE_VALUE FIELD_PLAYER_GRID_GetMoveValue(
    FIELD_PLAYER_GRID *gjiki, u16 dir );

extern void FIELD_PLAYER_GRID_SetRequest(
  FIELD_PLAYER_GRID *gjiki, FIELD_PLAYER_GRID_REQBIT req_bit );
extern void FIELD_PLAYER_GRID_UpdateRequest( FIELD_PLAYER_GRID *gjiki );

extern BOOL FIELD_PLAYER_GRID_CheckStartMove(
    FIELD_PLAYER_GRID *gjiki, u16 dir );

extern void FIELD_PLAYER_GRID_ForceStop( FIELD_PLAYER *fld_player );
extern BOOL FIELD_PLAYER_GRID_SetMoveStop( FIELD_PLAYER_GRID *gjiki );
extern void FIELD_PLAYER_GRID_SetEffectTaskWork(
    FIELD_PLAYER_GRID *gjiki, FLDEFF_TASK *task );
extern FLDEFF_TASK * FIELD_PLAYER_GRID_GetEffectTaskWork(
    FIELD_PLAYER_GRID *gjiki );
extern void FIELD_PLAYER_SetNaminori( FIELD_PLAYER_GRID *gjiki );
extern void FIELD_PLAYER_SetNaminoriEnd( FIELD_PLAYER_GRID *gjiki );
extern u16 FIELD_PLAYER_GRID_GetKeyDir( FIELD_PLAYER_GRID *gjiki, int key );

extern FIELD_PLAYER * FIELD_PLAYER_GRID_GetFieldPlayer(
    FIELD_PLAYER_GRID *gjiki );
extern MMDL * FIELD_PLAYER_GRID_GetMMdl( FIELD_PLAYER_GRID *gjiki );

//field_player_grid_event.c
extern GMEVENT * FIELD_PLAYER_GRID_CheckMoveEvent( FIELD_PLAYER *fld_player,
    int key_trg, int key_cont, PLAYER_EVENTBIT evbit );
