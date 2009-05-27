//======================================================================
/**
 * @file	fldeff_shadow.c
 * @brief	�t�B�[���hOBJ�e
 * @author	kagaya
 * @data	05.07.13
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

extern void FLDEFF_SHADOW_SetFldMMdl( FLDMMDL *fmmdl, FLDEFF_CTRL *fectrl );
