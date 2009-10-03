//======================================================================
/**
 * @file	fldeff_gyoe.h
 * @brief	�t�B�[���hOBJ �т�����}�[�N
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
//--------------------------------------------------------------
/// �т�����}�[�N���
//--------------------------------------------------------------
typedef enum
{
  FLDEFF_GYOETYPE_GYOE = 0,
//  FLDEFF_GYOETYPE_SISEN,
}FLDEFF_GYOETYPE;

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_GYOE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_GYOE_Delete( FLDEFF_CTRL *fectrl, void *work );

extern FLDEFF_TASK * FLDEFF_GYOE_SetMMdl( FLDEFF_CTRL *fectrl,
    const MMDL *mmdl, FLDEFF_GYOETYPE type, BOOL se_play );
extern FLDEFF_TASK * FLDEFF_GYOE_SetMMdlNonDepend( FLDEFF_CTRL *fectrl,
    const MMDL *mmdl, FLDEFF_GYOETYPE type, BOOL se_play );
extern BOOL FLDEFF_GYOE_CheckEnd( FLDEFF_TASK *task );
