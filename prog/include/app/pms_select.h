//============================================================================
/**
 *
 *	@file		pms_select.h
 *	@brief  o^ΟέΘΥοbIπζΚ
 *	@author		hosaka genya
 *	@data		2009.10.20
 *
 */
//============================================================================
#pragma once

typedef struct _PMS_SELECT_WORK PMS_SELECT_WORK;

//=============================================================================
/**
 *								θθ`
 */
//=============================================================================

//=============================================================================
/**
 *								\’Μθ`
 */
//=============================================================================
//--------------------------------------------------------------
///	
//==============================================================
typedef struct {
  // [IN]
  SAVE_CONTROL_WORK* save_ctrl;
  // [OUT]
} PMS_SELECT_PARAM;

//=============================================================================
/**
 *								EXTERNιΎ
 */
//=============================================================================

extern const GFL_PROC_DATA ProcData_PMSSelect;

extern GFL_PROC_RESULT PMSSelect_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT PMSSelect_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT PMSSelect_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

