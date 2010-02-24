//======================================================================
/**
 * @file	fldeff_hide.h
 * @brief	���샂�f���B�ꖪ
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
///	HIDE_TYPE �B�ꖪ���
//--------------------------------------------------------------
typedef enum
{
	HIDE_GRASS,
	HIDE_GROUND,
  HIDE_MAX,
  
  //�ȉ�wb�ł͎g�p���Ȃ����A�ߋ�����ڐA��������֐��͑��݂���ׁA�u������
  HIDE_SNOW = HIDE_GRASS,
  HIDE_SAND = HIDE_GROUND,
}HIDE_TYPE;

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern void * FLDEFF_HIDE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID );
extern void FLDEFF_HIDE_Delete( FLDEFF_CTRL *fectrl, void *work );

extern FLDEFF_TASK * FLDEFF_HIDE_SetMMdl(
    FLDEFF_CTRL *fectrl, MMDL *mmdl, HIDE_TYPE type );
extern void FLDEFF_HIDE_SetAnime( FLDEFF_TASK *task );
extern BOOL FLDEFF_HIDE_CheckAnime( FLDEFF_TASK *task );
