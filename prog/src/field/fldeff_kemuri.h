//======================================================================
/**
 * @file	fldeff_kemuri.h
 * @brief	�y���G�t�F�N�g
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
extern void * FLDEFF_KEMURI_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_KEMURI_Delete( FLDEFF_CTRL *fectrl, void *work );

extern void FLDEFF_KEMURI_SetFldMMdl( FLDMMDL *fmmdl, FLDEFF_CTRL *fectrl );
