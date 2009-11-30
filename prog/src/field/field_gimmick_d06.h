//======================================================================
/**
 * @file	field_gimmick_d06.h
 * @brief	�t�B�[���h�M�~�b�N�@D06�d�C���A
 * @authaor	kagaya
 * @date	2008.12.11
 */
//======================================================================
#pragma once

#include <gflib.h>
#include "system/gfl_use.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void D06_GIMMICK_Setup( FIELDMAP_WORK *fieldmap );
extern void D06_GIMMICK_End( FIELDMAP_WORK *fieldmap );
extern void D06_GIMMICK_Move( FIELDMAP_WORK *fieldmap );

extern GMEVENT * D06_GIMMICK_CheckPushEvent(
    FIELDMAP_WORK *fieldmap, u16 dir );
