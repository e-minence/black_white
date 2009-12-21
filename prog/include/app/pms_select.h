//============================================================================
/**
 *
 *	@file		pms_select.h
 *	@brief  �o�^�ς݊ȈՉ�b�I�����
 *	@author		hosaka genya
 *	@data		2009.10.20
 *
 */
//============================================================================
#pragma once

typedef struct _PMS_SELECT_WORK PMS_SELECT_WORK;

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//--------------------------------------------------------------
///	
//==============================================================
typedef struct {
  // [IN]
  SAVE_CONTROL_WORK* save_ctrl;
  // [OUT]
  BOOL            out_cancel_flag;  ///< �L�����Z���Ŕ��������H
  const PMS_DATA* out_pms_data;     ///< out_cancel_flag��TRUE�̏ꍇ�� NULL�BFALSE�̏ꍇ��SAVEDATA����
} PMS_SELECT_PARAM;

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================

extern const GFL_PROC_DATA ProcData_PMSSelect;

extern GFL_PROC_RESULT PMSSelect_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT PMSSelect_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT PMSSelect_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

