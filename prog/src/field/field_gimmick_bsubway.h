//======================================================================
/**
 * @file	field_gimmick_bsubway.h
 * @brief	�t�B�[���h�M�~�b�N�@bsubway�d�C���A
 * @authaor	kagaya
 * @date	2008.12.11
 */
//======================================================================
#pragma once

#include <gflib.h>
#include "system/gfl_use.h"

#include "fldeff_btrain.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void BSUBWAY_GIMMICK_Setup( FIELDMAP_WORK *fieldmap );
extern void BSUBWAY_GIMMICK_End( FIELDMAP_WORK *fieldmap );
extern void BSUBWAY_GIMMICK_Move( FIELDMAP_WORK *fieldmap );

extern void BSUBWAY_GIMMICK_SetTrain(
    FIELDMAP_WORK *fieldmap, FLDEFF_BTRAIN_TYPE type, const VecFx32 *pos );
extern FLDEFF_TASK * BSUBWAY_GIMMICK_GetTrainTask( FIELDMAP_WORK *fieldmap );

extern void BSUBWAY_GIMMICK_StopTrainShake( FIELDMAP_WORK *fieldmap );
