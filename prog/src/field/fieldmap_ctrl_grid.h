//======================================================================
/**
 * @file	fieldmap_ctrl_grid.h
 * @brief	�t�B�[���h�}�b�v�@�R���g���[���@�O���b�h����
 * @author	kagaya
 * @date	09.04.22
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"

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

extern void FIELDMAP_CTRL_GRID_SetPlayerPause(
    FIELDMAP_WORK *fieldMap, BOOL flag );


