//======================================================================
/**
 * @file	field_player_grid.c
 * @brief �O���b�h��p �t�B�[���h�v���C���[����
 * @author kagaya
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "field_player.h"

//======================================================================
//	define
//======================================================================

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
