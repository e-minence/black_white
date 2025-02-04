//======================================================================
/**
 * @file	fldeff_shadow.h
 * @brief	フィールドOBJ影
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_SHADOW_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_SHADOW_Delete( FLDEFF_CTRL *fectrl, void *work );
extern void FLDEFF_SHADOW_SetGlobalScale(
    FLDEFF_CTRL *fectrl, const VecFx32 *scale );

extern void FLDEFF_SHADOW_SetMMdl( MMDL *fmmdl, FLDEFF_CTRL *fectrl );
