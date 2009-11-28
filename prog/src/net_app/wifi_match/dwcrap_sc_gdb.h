//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dwcrap_sc_gdb.h
 *	@brief  DWC���v�E�������f�[�^�x�[�X���b�v���[�`��
 *	@author	Toru=Nagihashi
 *	@date		2009.11.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "sc/dwc_sc.h"
#include "gdb/dwc_gdb.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�󂯎��\����
//=====================================
typedef struct 
{
  u16 win;
  u16 lose;
  u32 match_cnt;
  u32 rate;
} DWC_GDB_PARAM;



//-------------------------------------
///	���[�N
//=====================================
typedef struct _DWCRAP_SC_GDB_WORK DWCRAP_SC_GDB_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	���[�N�쐬
//=====================================
extern DWCRAP_SC_GDB_WORK * DWCRAP_SC_GDB_Init( HEAPID heapID );
extern void DWCRAP_SC_GDB_Exit( DWCRAP_SC_GDB_WORK *p_wk );

//-------------------------------------
///	���v�E�����֌W�iSC�j
//=====================================
extern void DWCRAP_SC_Start( DWCRAP_SC_GDB_WORK *p_wk );
extern BOOL DWCRAP_SC_Process( DWCRAP_SC_GDB_WORK *p_wk, const DWCUserData *cp_user_data, DWCScResult *p_result );

//-------------------------------------
///	�f�[�^�x�[�X�֌W�iGDB)
//=====================================
extern void DWCRAP_GDB_Start( DWCRAP_SC_GDB_WORK *p_wk );
extern BOOL DWCRAP_GDB_Process( DWCRAP_SC_GDB_WORK *p_wk, const DWCUserData *cp_user_data, DWCGdbError *p_result );
