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

#include "scrcmd_work.h"

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
//	�X�N���v�g�R�}���h���g�p����̂ɕK�v�ȃC�����C���֐���`
//======================================================================
//--------------------------------------------------------------
/**
 * �C�����C���֐��F���[�N���擾����
 * @param	core	���z�}�V�����䃏�[�N�ւ̃|�C���^
 * @retval	u16 *	���[�N�ւ̃|�C���^
 *
 * ����2�o�C�g�����[�N���w�肷��ID�Ƃ݂Ȃ��āA���[�N�ւ̃|�C���^���擾����
 */
//--------------------------------------------------------------
static inline u16 * VMGetWork(VMHANDLE *core, SCRCMD_WORK *work )
{
	GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	return SCRIPT_GetEventWork( sc, gdata, VMGetU16(core) );
}

//--------------------------------------------------------------
/**
 * �C�����C���֐��F���[�N����l���擾����
 * @param	core	���z�}�V�����䃏�[�N�ւ̃|�C���^
 * @retval	u16		�l
 *
 * ����2�o�C�g��SVWK_START�i0x4000�ȉ��j�ł���Βl�Ƃ��Ď󂯎��B
 * ����ȏ�̏ꍇ�̓��[�N���w�肷��ID�Ƃ݂Ȃ��āA���̃��[�N����l���擾����
 */
//--------------------------------------------------------------
static inline u16 VMGetWorkValue(VMHANDLE * core, SCRCMD_WORK *work)
{
	GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	return SCRIPT_GetEventWorkValue( sc, gdata, VMGetU16(core) );
}

//======================================================================
//	extern�錾
//======================================================================
#ifndef SCRCMD_PL_NULL
extern BOOL EvCmdWaitSubProcEnd( VM_MACHINE * core );
extern BOOL EvWaitSubProcAndFree( VM_MACHINE * core );
#endif

#endif	/* SCRCMD_H */
