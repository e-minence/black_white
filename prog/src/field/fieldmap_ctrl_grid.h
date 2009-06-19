//======================================================================
/**
 * @file	fieldmap_ctrl_grid.h
 * @brief	フィールドマップ　コントロール　グリッド処理
 * @author	kagaya
 * @data	09.04.22
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_player_grid.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
///FIELDMAP_CTRL_GRID
typedef struct _TAG_FIELDMAP_CTRL_GRID FIELDMAP_CTRL_GRID;

//======================================================================
//	extern
//======================================================================
extern const DEPEND_FUNCTIONS FieldMapCtrl_GridFunctions;

extern FIELD_PLAYER_GRID * FIELDMAP_CTRL_GRID_GetFieldPlayerGrid(
    FIELDMAP_CTRL_GRID *gridWork );
