//======================================================================
/**
 * @file	scrcmd.h
 * @bfief	�X�N���v�g�R�}���h�p�֐�
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

//======================================================================
//	��`
//======================================================================
//#define EVCMD_MAX	(565)		//scrcmd.c ScriptCmdTbl�̗v�f��
#define SCRCMD_PL_NULL //��������`

//======================================================================
//	�O���[�o���ϐ�
//======================================================================
extern const VMCMD_FUNC ScriptCmdTbl[];
extern const u32 ScriptCmdMax;

//======================================================================
//	extern�錾
//======================================================================
#ifndef SCRCMD_PL_NULL
extern BOOL EvCmdWaitSubProcEnd( VM_MACHINE * core );
extern BOOL EvWaitSubProcAndFree( VM_MACHINE * core );
#endif

#endif	/* SCRCMD_H */
