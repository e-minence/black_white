//======================================================================
/**
 * @file	fldeff_splash.h
 * @brief	���򖗃G�t�F�N�g
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
extern void * FLDEFF_SPLASH_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_SPLASH_Delete( FLDEFF_CTRL *fectrl, void *work );
extern void FLDEFF_SPLASH_SetMMdl( FLDEFF_CTRL *fectrl, const MMDL *mmdl );

extern void * FLDEFF_SHOAL_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_SHOAL_Delete( FLDEFF_CTRL *fectrl, void *work );
extern void FLDEFF_SHOAL_SetMMdl( FLDEFF_CTRL *fectrl, const MMDL *mmdl );
