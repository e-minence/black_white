//======================================================================
/**
 * @file	scrcmd.h
 * @brief	�X�N���v�g�R�}���h�p�֐�
 * @author	Satoshi Nohara
 * @date	05.08.08
 *
 * 05.04.26 Hiroyuki Nakamura
 */
//======================================================================
#ifndef SCRCMD_H
#define SCRCMD_H

#include <gflib.h>
#include "system/vm.h"

#include "scrcmd_work.h"

//======================================================================
//	��`
//======================================================================
//#define EVCMD_MAX	(565)		//scrcmd.c ScriptCmdTbl�̗v�f��
#define SCRCMD_PL_NULL //��������`

//--------------------------------------------------------------
/// �X�N���v�g�C�x���g�@�L�[�V���{��
//--------------------------------------------------------------
enum
{
  EVENT_WAIT_AB_KEY = PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL,
  EVENT_WAIT_PAD_KEY = PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT,
  EVENT_WAIT_LAST_KEY = EVENT_WAIT_AB_KEY|EVENT_WAIT_PAD_KEY 
};

//======================================================================
//	�O���[�o���ϐ�
//======================================================================
extern const VMCMD_FUNC ScriptCmdTbl[];
extern const u32 ScriptCmdMax;

//======================================================================
//	extern�錾
//======================================================================
extern u16 * SCRCMD_GetVMWork( VMHANDLE *core, SCRCMD_WORK *work );
extern u16 SCRCMD_GetVMWorkValue( VMHANDLE * core, SCRCMD_WORK *work );


#ifndef SCRCMD_PL_NULL
extern BOOL EvCmdWaitSubProcEnd( VM_MACHINE * core );
extern BOOL EvWaitSubProcAndFree( VM_MACHINE * core );
#endif

extern VMCMD_RESULT EvCmdDebugPrintWk( VMHANDLE * core, void *wk );

#endif	/* SCRCMD_H */
