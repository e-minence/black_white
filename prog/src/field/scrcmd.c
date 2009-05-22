//======================================================================
/**
 * @file	scrcmd.c
 * @bfief	�X�N���v�g�R�}���h�p�֐�
 * @author	Satoshi Nohara
 * @date	05.08.04
 *
 * 05.04.26 Hiroyuki Nakamura
 */
//======================================================================
#include <nitro/code16.h> 
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_def.h"

#include "scrcmd.h"
#include "scrcmd_work.h"

#include "msgdata.h"

//======================================================================
//	��`
//======================================================================
typedef u16 (* pMultiFunc)();

//���W�X�^��r�̌��ʒ�`
enum {
	MINUS_RESULT = 0,	//��r���ʂ��}�C�i�X
	EQUAL_RESULT,		//��r���ʂ��C�R�[��
	PLUS_RESULT			//��r���ʂ��v���X
};

//======================================================================
//	�v���g�^�C�v�錾
//======================================================================
static VMCMD_RESULT EvCmdNop( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdDummy( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdEnd( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdTimeWait( VMHANDLE * core, void *wk );
static BOOL EvWaitTime(VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdLoadRegValue( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdLoadRegWData( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdLoadRegAdrs( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdLoadAdrsValue( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdLoadAdrsReg( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdLoadRegReg( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdLoadAdrsAdrs( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdCmpMain( u16 r1, u16 r2 );
static VMCMD_RESULT EvCmdCmpRegReg( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpRegValue( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpRegAdrs( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpAdrsReg( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpAdrsValue(VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpAdrsAdrs(VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpWkValue( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdCmpWkWk( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdVMMachineAdd( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdChangeCommonScr( VMHANDLE * core, void *wk );
static BOOL EvChangeCommonScrWait(VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdChangeLocalScr( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdGlobalJump( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdObjIDJump( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdBgIDJump( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdPlayerDirJump( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdGlobalCall( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdRet( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdIfJump( VMHANDLE * core, void *wk );
static VMCMD_RESULT EvCmdIfCall(VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdABKeyWait( VMHANDLE * core, void *wk );
static BOOL EvWaitABKey( VMHANDLE * core, void *wk );

static VMCMD_RESULT EvCmdTalkMsgAllPut( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdChangeLangID( VMHANDLE *core, void *wk );

static VMCMD_RESULT EvCmdTalkWinOpen( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdTalkWinClose( VMHANDLE *core, void *wk );

static VMCMD_RESULT EvCmdObjAnime( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdObjAnimeWait( VMHANDLE * core, void *wk );
static BOOL EvObjAnimeWait(VMHANDLE * core, void *wk);

static VMCMD_RESULT EvCmdObjPauseAll( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdTalkObjPauseAll( VMHANDLE *core, void *wk );
static BOOL EvWaitTalkObj( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdObjPauseClearAll( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdObjTurn( VMHANDLE *core, void *wk );

static VMCMD_RESULT EvCmdYesNoWin( VMHANDLE *core, void *wk );
static BOOL EvYesNoWinSelect( VMHANDLE *core, void *wk );

//======================================================================
//	�O���[�o���ϐ�
//======================================================================
//--------------------------------------------------------------
///	��������p�e�[�u��
//--------------------------------------------------------------
static const u8 ConditionTable[6][3] =
{
//	  MINUS  EQUAL  PLUS
	{ TRUE,	 FALSE, FALSE },	// LT
	{ FALSE, TRUE,  FALSE },	// EQ
	{ FALSE, FALSE, TRUE  },	// GT
	{ TRUE,  TRUE,  FALSE },	// LE
	{ FALSE, TRUE,  TRUE  },	// GE
	{ TRUE,  FALSE, TRUE  }		// NE
};

//--------------------------------------------------------------
///	�X�N���v�g�R�}���h�e�[�u��
//--------------------------------------------------------------
const VMCMD_FUNC ScriptCmdTbl[] = {
	EvCmdNop,				
	EvCmdDummy,				
	
	EvCmdEnd,			
	EvCmdTimeWait,
	
	EvCmdLoadRegValue,
	EvCmdLoadRegWData,
	EvCmdLoadRegAdrs,
	EvCmdLoadAdrsValue,
	EvCmdLoadAdrsReg,
	EvCmdLoadRegReg,
	EvCmdLoadAdrsAdrs,
	
	EvCmdCmpRegReg,
	EvCmdCmpRegValue,
	EvCmdCmpRegAdrs,		
	EvCmdCmpAdrsReg,		
	EvCmdCmpAdrsValue,
	EvCmdCmpAdrsAdrs,
	EvCmdCmpWkValue,
	EvCmdCmpWkWk,
	
	EvCmdVMMachineAdd,
	EvCmdChangeCommonScr,
	EvCmdChangeLocalScr,
	
	EvCmdGlobalJump,
	EvCmdObjIDJump,
	EvCmdBgIDJump,
	EvCmdPlayerDirJump,
	
	EvCmdGlobalCall,
	EvCmdRet,
	
	EvCmdIfJump,
	EvCmdIfCall,

	EvCmdABKeyWait,
	
	EvCmdTalkMsgAllPut,
	
	EvCmdTalkWinOpen,
	EvCmdTalkWinClose,
	
	EvCmdObjAnime,
	EvCmdObjAnimeWait,
	
	EvCmdObjPauseAll,
	EvCmdTalkObjPauseAll,
	EvCmdObjPauseClearAll,
	EvCmdObjTurn,
  
  EvCmdYesNoWin,

  EvCmdChangeLangID,
};

//--------------------------------------------------------------
///	�X�N���v�g�R�}���h�̍ő吔
//--------------------------------------------------------------
const u32 ScriptCmdMax = NELEMS(ScriptCmdTbl);

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
///	���샂�f���Ď��Ɏg�p����C�����C���֐���`
//======================================================================
#define	PLAYER_BIT			(1<<0)
#define	PLAYER_PAIR_BIT		(1<<1)
#define	OTHER_BIT			(1<<2)
#define	OTHER_PAIR_BIT		(1<<3)

static u8 step_watch_bit;

static inline void InitStepWatchBit(void)
{
	step_watch_bit = 0;
}

static inline BOOL CheckStepWatchBit(int mask)
{
	return (step_watch_bit & mask) != 0;
}

static inline void SetStepWatchBit(int mask)
{
	step_watch_bit |= mask;
}

static inline void ResetStepWatchBit(int mask)
{
	step_watch_bit &= (0xff ^ mask);
}

//======================================================================
//	��{�V�X�e������
//======================================================================
//--------------------------------------------------------------
/**
 * �m�n�o���߁i�Ȃɂ����Ȃ��j
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdNop( VMHANDLE *core, void *wk )
{
	return 0;
}

//--------------------------------------------------------------
/**
 * �������Ȃ��i�f�o�b�K�ň����|���邽�߂̖��߁j
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdDummy( VMHANDLE *core, void *wk )
{
	return 0;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�̏I��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdEnd( VMHANDLE *core, void *wk )
{
	VM_End( core );
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �E�F�C�g����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"1"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTimeWait( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 num				= VMGetU16( core );
	u16 wk_id			= VMGetU16( core );
	u16 *ret_wk			= SCRIPT_GetEventWork( sc, gdata, wk_id );
	
	*ret_wk = num;
	
	//���z�}�V���̔ėp���W�X�^�Ƀ��[�N��ID���i�[
	core->vm_register[0] = wk_id;
	VMCMD_SetWait( core, EvWaitTime );
	return 1;
}

//return 1 = �I��
static BOOL EvWaitTime(VMHANDLE * core, void *wk )
{
	SCRCMD_WORK *work = wk;
	GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, core->vm_register[0] ); //���ӁI

	(*ret_wk)--;
	if( *ret_wk == 0 ){ 
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
#ifndef SCRCMD_PL_NULL
static VMCMD_RESULT EvCmdDebugWatch(VMHANDLE * core, void *wk )
{
	u16 value = VMGetWorkValue(core);
	OS_Printf("SCR WORK: %d\n", value);
	return 0;
}
#endif

//======================================================================
//	�f�[�^���[�h�E�X�g�A�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^��1byte�̒l���i�[
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegValue( VMHANDLE *core, void *wk )
{
	u8	r = VMGetU8( core );
	core->vm_register[r] = VMGetU8( core );
	return 0;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^��4byte�̒l���i�[
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegWData( VMHANDLE *core, void *wk )
{
	u8	r;
	u32	wdata;
	
	r = VMGetU8( core );
	wdata = VMGetU32( core );
	core->vm_register[r] = wdata;
	return 0;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�ɃA�h���X���i�[
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegAdrs( VMHANDLE *core, void *wk )
{
	u8	r;
	VM_CODE * adrs;

	r = VMGetU8( core );
	adrs = (VM_CODE *)VMGetU32( core );
	core->vm_register[r] = *adrs;
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ɒl����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadAdrsValue( VMHANDLE *core, void *wk )
{
	VM_CODE * adrs;
	u8	r;

	adrs = (VM_CODE *)VMGetU32( core );
	r = VMGetU8( core );
	*adrs = r;
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ɉ��z�}�V���̔ėp���W�X�^�̒l����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadAdrsReg( VMHANDLE *core, void *wk )
{
	VM_CODE * adrs;
	u8	r;

	adrs = (VM_CODE *)VMGetU32( core );
	r = VMGetU8( core) ;
	*adrs = core->vm_register[r];
	return 0;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�̒l��ėp���W�X�^�ɃR�s�[
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegReg( VMHANDLE *core, void *wk )
{
	u8	r1, r2;

	r1 = VMGetU8( core );
	r2 = VMGetU8( core );
	core->vm_register[r1] = core->vm_register[r2];
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ɃA�h���X�̒��g����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadAdrsAdrs( VMHANDLE *core, void *wk )
{
	VM_CODE * adr1;
	VM_CODE * adr2;

	adr1 = (VM_CODE *)VMGetU32( core );
	adr2 = (VM_CODE *)VMGetU32( core );
	*adr1 = * adr2;
	return 0;
}

//======================================================================
//	��r����
//======================================================================
//--------------------------------------------------------------
/**
 * �Q�̒l���r
 * @param	r1		�l�P
 * @param	r2		�l�Q
 * @retval	"r1 < r2 : MISUS_RESULT"
 * @retval	"r1 = r2 : EQUAL_RESULT"
 * @retval	"r1 > r2 : PLUS_RESULT"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpMain( u16 r1, u16 r2 )
{
	if( r1 < r2 ){
		return MINUS_RESULT;
	}else if( r1 == r2 ){
		return EQUAL_RESULT;
	}
	return PLUS_RESULT;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpRegReg( VMHANDLE *core, void *wk )
{
	u8	r1, r2;

	r1 = core->vm_register[ VMGetU8(core) ];
	r2 = core->vm_register[ VMGetU8(core) ];
	core->cmp_flag = EvCmdCmpMain( r1, r2 );
	return 0;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�ƒl���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpRegValue( VMHANDLE *core, void *wk )
{
	u8	r1, r2;

	r1 = core->vm_register[ VMGetU8(core) ];
	r2 = VMGetU8(core);
	core->cmp_flag = EvCmdCmpMain( r1, r2 );
	return 0;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�ƃA�h���X�̒��g���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpRegAdrs( VMHANDLE *core, void *wk )
{
	u8	r1, r2;

	r1 = core->vm_register[ VMGetU8(core) ];
	r2 = *(VM_CODE *)VMGetU32(core);
	core->cmp_flag = EvCmdCmpMain( r1, r2 );
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�Ɖ��z�}�V���̔ėp���W�X�^���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpAdrsReg( VMHANDLE *core, void *wk )
{
	u8	r1, r2;

	r1 = *(VM_CODE *)VMGetU32(core);
	r2 = core->vm_register[ VMGetU8(core) ];
	core->cmp_flag = EvCmdCmpMain(r1,r2);
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ƒl���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpAdrsValue(VMHANDLE * core, void *wk)
{
	u8 r1,r2;

	r1 = *(VM_CODE *)VMGetU32(core);
	r2 = VMGetU8(core);
	core->cmp_flag = EvCmdCmpMain(r1,r2);
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ƃA�h���X�̒��g���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpAdrsAdrs(VMHANDLE * core, void *wk)
{
	u8 r1,r2;

	r1 = *(VM_CODE *)VMGetU32(core);
	r2 = *(VM_CODE *)VMGetU32(core);
	core->cmp_flag = EvCmdCmpMain(r1,r2);
	return 0;
}

//--------------------------------------------------------------
/**
 * ���[�N�ƒl���r
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpWkValue( VMHANDLE *core, void *wk )
{
	u16 *p;
	u16	r1, r2;

	p = VMGetWork( core, wk );
	r1 = *p;
	r2 = VMGetU16( core );
	core->cmp_flag = EvCmdCmpMain( r1, r2 );
	return 0;
}

//--------------------------------------------------------------
/**
 * ���[�N�ƃ��[�N���r
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpWkWk( VMHANDLE *core, void *wk )
{
	u16 * wk1;
	u16 * wk2;

	wk1 = VMGetWork( core, wk );
	wk2 = VMGetWork( core, wk );
	core->cmp_flag = EvCmdCmpMain( *wk1, *wk2 );
	return 0;
}

//======================================================================
//	���z�}�V���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���z�}�V���ǉ�(�؂�ւ��͂����A����œ��삵�܂��I)
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"1"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdVMMachineAdd( VMHANDLE *core, void *wk )
{
	u16 id;
	SCRCMD_WORK *work = wk;
	GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	
	u8 *vm_machine_count = SCRIPT_GetMemberWork( sc, ID_EVSCR_VMHANDLE_COUNT );
	VMHANDLE **vm = SCRIPT_GetMemberWork( sc, ID_EVSCR_VM_SUB1 );

	id = VMGetU16(core);

	//���z�}�V���ǉ�
	//*vm = VMMachineAdd(fsys, id, &ScriptCmdTbl[0], &ScriptCmdTbl[EVCMD_MAX]);
	*vm = SCRIPT_AddVMachine( gsys, sc, SCRCMD_WORK_GetHeapID(work), id );
	(*vm_machine_count)++;

	//�C�x���g�Ɛ؂藣����TCB����ɂ��邩���H
	//*vm = VMMachineAddTCB( fsys, id, &ScriptCmdTbl[0], &ScriptCmdTbl[EVCMD_MAX] );
	return 1;
}

//--------------------------------------------------------------
/**
 * ���[�J���X�N���v�g���E�F�C�g��Ԃɂ��āA���ʃX�N���v�g�𓮍삳���܂�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"1"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdChangeCommonScr( VMHANDLE *core, void *wk )
{
	u16 scr_id;
	SCRCMD_WORK *work = wk;
	GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u8* common_scr_flag = SCRIPT_GetMemberWork(sc, ID_EVSCR_COMMON_SCR_FLAG);
	u8 *vm_machine_count = SCRIPT_GetMemberWork( sc, ID_EVSCR_VMHANDLE_COUNT );
	VMHANDLE **vm = SCRIPT_GetMemberWork( sc, ID_EVSCR_VM_SUB1 );

	scr_id = VMGetU16(core);

	//���ʃX�N���v�g�ȊO�ɂ��؂�ւ��\�ɂȂ��Ă���I

	//���ʃX�N���v�g�؂�ւ��t���OON
	*common_scr_flag = 1;

	//���z�}�V���ǉ�
	//*vm = VMMachineAdd(fsys, scr_id, &ScriptCmdTbl[0], &ScriptCmdTbl[EVCMD_MAX]);
	*vm = SCRIPT_AddVMachine( gsys, sc, SCRCMD_WORK_GetHeapID(work), scr_id );
	(*vm_machine_count)++;

	VMCMD_SetWait( core, EvChangeCommonScrWait );
	return 1;
}

//return 1 = �I��
static BOOL EvChangeCommonScrWait(VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u8* common_scr_flag = SCRIPT_GetMemberWork(sc,ID_EVSCR_COMMON_SCR_FLAG);

	if( *common_scr_flag == 0 ){
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * ���ʃX�N���v�g���I�����āA���[�J���X�N���v�g���ĊJ�����܂�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdChangeLocalScr( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u8* common_scr_flag = SCRIPT_GetMemberWork(sc, ID_EVSCR_COMMON_SCR_FLAG);

	//���ʃX�N���v�g�؂�ւ��t���OOFF
	*common_scr_flag = 0;

	//VM_End( core );
	return 0;	//���ӁI�@���̌��"END"�ɍs���悤�ɂ���
}

//======================================================================
//	���򖽗�
//======================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�g�W�����v
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 * @li	EVCMD_JUMP
 *	�\�L�F	EVCMD_JUMP	JumpOffset(s16)
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdGlobalJump( VMHANDLE *core, void *wk )
{
	s32	pos;
	pos = (s32)VMGetU32(core);
	VMCMD_Jump( core, (VM_CODE *)(core->adrs+pos) );	//JUMP
	return 0;
}

//--------------------------------------------------------------
/**
 * �b���|���Ώ�OBJID��r�W�����v
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjIDJump( VMHANDLE *core, void *wk )
{
	u8 id;
	s32	pos;
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	FLDMMDL **fldobj;
	
	//�b���|���Ώ�OBJ
	fldobj = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
	
	//��r����l
	id = VMGetU8(core);
	
	//��ѐ�
	pos = (s32)VMGetU32(core);
	
	//�b���|���Ώ�OBJ�ƁA��r����l��������
	if( FLDMMDL_GetOBJID(*fldobj) == id ){
		VMCMD_Jump( core, (VM_CODE *)(core->adrs+pos) );	//JUMP
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * �b���|���Ώ�BG��r�W�����v
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdBgIDJump( VMHANDLE *core, void *wk )
{
#ifndef SCRCMD_PL_NULL
	u8 id;
	s32	pos;
	//u32 *targetbg;
	u32 targetbg;

	//�b���|���Ώ�BG
	targetbg = Event_GetTargetBg( core->event_work );

	//��r����l
	id = VMGetU8(core);
	
	//��ѐ�
	pos = (s32)VMGetU32(core);

	//�b���|���Ώ�BG�ƁA��r����l��������
	//if( *targetbg == id ){
	if( targetbg == id ){
		VMJump( core, (VM_CODE *)(core->adrs+pos) );	//JUMP
	}
	return 0;
#else
	GF_ASSERT( 0 && "WB���Ή�\n" );
	return 0;
#endif
}

//--------------------------------------------------------------
/**
 * �C�x���g�N�����̎�l���̌�����r�W�����v
 * (���݂̌����ł͂Ȃ��̂Œ��ӁI)
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPlayerDirJump( VMHANDLE *core, void *wk )
{
#ifndef SCRCMD_PL_NULL
	u8 dir;
	s32	pos;
	int* player_dir;
	FIELDSYS_WORK* fsys = core->fsys;

	player_dir = GetEvScriptWorkMemberAdrs( fsys, ID_EVSCR_PLAYER_DIR );

	//��r����l
	dir = VMGetU8(core);
	
	//��ѐ�
	pos = (s32)VMGetU32(core);

	if( *player_dir == dir ){
		VMJump( core, (VM_CODE *)(core->adrs+pos) );	//JUMP
	}
	return 0;
#else
	GF_ASSERT( 0 && "WB���Ή�\n" );
	return 0;
#endif
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�R�[��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 * @li	EVCMD_CALL
 *	�\�L�F	EVCMD_CALL	CallOffset(s16)
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdGlobalCall( VMHANDLE *core, void *wk )
{
	s32	pos = (s32)VMGetU32(core);
	VMCMD_Call( core, (VM_CODE *)(core->adrs+pos) );
	return 0;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g���^�[��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdRet( VMHANDLE *core, void *wk )
{
	VMCMD_Return( core );
	return 0;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�����W�����v
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdIfJump( VMHANDLE *core, void *wk )
{
	u8	r;
	s32	pos;

	r   = VMGetU8(core);
	pos = (s32)VMGetU32(core);

	if( ConditionTable[r][core->cmp_flag] == TRUE ){
		VMCMD_Jump( core, (VM_CODE *)(core->adrs+pos) );
	}
	return 0;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�����R�[��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdIfCall( VMHANDLE *core, void *wk )	
{
	u8	r;
	s32	pos;

	r   = VMGetU8(core);
	pos = (s32)VMGetU32(core);

	if( ConditionTable[r][core->cmp_flag] == TRUE ){
		VMCMD_Call( core, (VM_CODE *)(core->adrs+pos) );
	}
	return 0;
}

//======================================================================
//	�L�[���͊֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[�E�F�C�g
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	���1
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdABKeyWait( VMHANDLE * core, void *wk )
{
	VMCMD_SetWait( core, EvWaitABKey );
	return VMCMD_RESULT_SUSPEND;
}

//return 1 = �I��
static BOOL EvWaitABKey( VMHANDLE * core, void *wk )
{
	int trg = GFL_UI_KEY_GetTrg();
	
	if( trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
		return 1;
	}

	return 0;
}

//======================================================================
//	�t�B�[���h�@���b�Z�[�W�\��
//======================================================================
static BOOL TalkMsgWait( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	FLDMSGWIN_STREAM *msgWin;
  msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
  
  if( FLDMSGWIN_STREAM_Print(msgWin) == TRUE ){
    return( 1 );
  }
  
  return( 0 );
}

//--------------------------------------------------------------
/**
 * �o�^���ꂽ�P����g���ĕ�����W�J�@���b�Z�[�W�\��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	���1
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkMsg( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	u8 msg_id = VMGetU8(core);
	FLDMSGWIN_STREAM *msgWin;
  
	msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
	FLDMSGWIN_STREAM_PrintStart( msgWin, 0, 0, msg_id );
  VMCMD_SetWait( core, TalkMsgWait );
	return 1;
}

//--------------------------------------------------------------
/**
 * �o�^���ꂽ�P����g���ĕ�����W�J�@���b�Z�[�W�\��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkMsgAllPut( VMHANDLE *core, void *wk )
{
#if 0
	SCRCMD_WORK *work = wk;
	u8 msg_id = VMGetU8(core);
	FLDMSGWIN_STREAM *msgWin;
	
	msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
	FLDMSGWIN_STREAM_PrintStart( msgWin, 0, 0, msg_id );
	return 0;
#else
  return( EvCmdTalkMsg(core,wk) );
#endif
}

//--------------------------------------------------------------
/**
 * ����ID��؂�ւ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdChangeLangID( VMHANDLE *core, void *wk )
{
  u8 id = GFL_MSGSYS_GetLangID();
  if( id == 0 ){ //�Ђ炪��
    id = 1;
  }else if( id == 1 ){ //����
    id = 0;
  }
  GFL_MSGSYS_SetLangID( id );
  return 0;
}

//======================================================================
//	�t�B�[���h�@��b�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkWinOpen( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc;
	SCRIPT_FLDPARAM *fparam;
	GFL_MSGDATA *msgData;
	FLDMSGWIN_STREAM *msgWin;
	u8 *win_open_flag;
	
	sc = SCRCMD_WORK_GetScriptWork( work );
	fparam = SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_FLDPARAM );
	msgData = SCRCMD_WORK_GetMsgData( work );
	msgWin = FLDMSGWIN_STREAM_AddTalkWin( fparam->msgBG, msgData );
	SCRCMD_WORK_SetFldMsgWinStream( work, msgWin );

	win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
	*win_open_flag = TRUE;	//ON;
	return 0;
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkWinClose( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc;
	FLDMSGWIN_STREAM *msgWin;
	u8 *win_open_flag;
	
	msgWin = SCRCMD_WORK_GetFldMsgWinStream( work );
	FLDMSGWIN_STREAM_Delete( msgWin );
	
	sc = SCRCMD_WORK_GetScriptWork( work );
	win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
	*win_open_flag = FALSE;	//OFF;
	return 0;
}

//======================================================================
//	���샂�f��	
//======================================================================
static FLDMMDL * FieldObjPtrGetByObjId( SCRCMD_WORK *work, u16 obj_id );
static void EvAnmSetTCB(
	SCRCMD_WORK *work, GFL_TCB *anm_tcb, FLDMMDL_ACMD_LIST *list );

//--------------------------------------------------------------
/**
 * �A�j���[�V����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjAnime( VMHANDLE *core, void *wk )
{
	u8 *num;
	VM_CODE *p;
	GFL_TCB *anm_tcb;
	FLDMMDL *fmmdl; //�Ώۂ̃t�B�[���hOBJ�̃|�C���^
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 obj_id = VMGetWorkValue(core,work); //obj ID
	u32 pos = VMGetU32(core); //list pos
	
	fmmdl = FieldObjPtrGetByObjId( work, obj_id );
	
	//�G���[�`�F�b�N
	if( fmmdl == NULL ){
		OS_Printf( "obj_id = %d\n", obj_id );
		GF_ASSERT( (0) && "�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾���s�I" );
		return 0;				//08.06.12 �v���`�i�Œǉ�
	}
	
	//�A�j���[�V�����R�}���h���X�g�Z�b�g
	p = (VM_CODE*)(core->adrs+pos);
	anm_tcb = FLDMMDL_SetAcmdList( fmmdl, (FLDMMDL_ACMD_LIST*)p );
	
	//�A�j���[�V�����̐��𑫂�
	num = SCRIPT_GetMemberWork( sc, ID_EVSCR_ANMCOUNT );
	(*num)++;
	
	//TCB�Z�b�g
	EvAnmSetTCB( work, anm_tcb, NULL );
	return 0;
}

//--------------------------------------------------------------
/**
 * �A�j���[�V�����I���҂�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"1"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjAnimeWait( VMHANDLE * core, void *wk )
{
	VMCMD_SetWait( core, EvObjAnimeWait );
	return 1;
}

//return 1 = �I��
static BOOL EvObjAnimeWait(VMHANDLE * core, void *wk )
{
	SCRCMD_WORK *work = wk;
	
	if( SCRCMD_WORK_CheckFldMMdlAnmTCB(work) == FALSE ){
		return 1;
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 * FIELD_OBJ_PTR���擾
 * @param	fsys	FIELDSYS_WORK�^�̃|�C���^
 * @param	obj_id	OBJID
 * @retval	"FIELD_OBJ_PTR"
 */
//--------------------------------------------------------------
static FLDMMDL * FieldObjPtrGetByObjId( SCRCMD_WORK *work, u16 obj_id )
{
	FLDMMDL *dummy;
	FLDMMDL *fmmdl;
	FLDMMDLSYS *fmmdlsys;
	
	fmmdlsys = SCRCMD_WORK_GetFldMMdlSys( work );
	
	//�A�����OBJ����ID���n���ꂽ��
	if( obj_id == SCR_OBJID_MV_PAIR ){
		fmmdl = FLDMMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
	//�����_�~�[OBJ����ID���n���ꂽ��
	}else if( obj_id == SCR_OBJID_DUMMY ){
		SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
		dummy = SCRIPT_GetMemberWork( sc, ID_EVSCR_DUMMY_OBJ );
	//�Ώۂ̃t�B�[���hOBJ�̃|�C���^�擾
	}else{
		fmmdl = FLDMMDLSYS_SearchOBJID( fmmdlsys, obj_id );
	}
	
	return fmmdl;
}

//--------------------------------------------------------------
/**
 * �A�j���I���Ď�TCB �Z�b�g
 * @param	fsys	FIELDSYS_WORK�^�̃|�C���^
 * @param	anm_tcb	TCB_PTR�^
 * @retval	none
 */
//--------------------------------------------------------------
static void EvAnmSetTCB(
	SCRCMD_WORK *work, GFL_TCB *anm_tcb, FLDMMDL_ACMD_LIST *list )
{
#if 0
	EV_ANM_WORK* wk = NULL;
	wk = sys_AllocMemory(HEAPID_FIELD, sizeof(EV_ANM_WORK));

	if( wk == NULL ){
		GF_ASSERT( (0) && "scrcmd.c �������m�ێ��s�I" );
		return;
	}

	wk->fsys	= fsys;
	wk->anm_tcb	= anm_tcb;
	wk->list	= list;
	wk->tcb		= TCB_Add( EvAnmMainTCB, wk, 0 );
	return;
#else
	SCRCMD_WORK_SetFldMMdlAnmTCB( work, anm_tcb );
#endif
}

//======================================================================
//	���샂�f���@�C�x���g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���@FLDMMDL�擾�@��
 * @param
 * @retval
 */
//--------------------------------------------------------------
static FLDMMDL * PlayerGetFldMMdl( SCRCMD_WORK *work )
{
	FLDMMDLSYS *fmmdlsys = SCRCMD_WORK_GetFldMMdlSys( work );
	FLDMMDL *fmmdl = FLDMMDLSYS_SearchOBJID( fmmdlsys, FLDMMDL_ID_PLAYER );
	return( fmmdl );
}

//--------------------------------------------------------------
/**
 * �SOBJ�����~
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	0
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjPauseAll( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc;
	FLDMMDL **fmmdl;
	
	sc = SCRCMD_WORK_GetScriptWork( work );
	fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
	
	if( (*fmmdl) == NULL ){
		FLDMMDLSYS *fmmdlsys;
		fmmdlsys = SCRCMD_WORK_GetFldMMdlSys( work );
		FLDMMDLSYS_PauseMoveProc( fmmdlsys );
		
		#ifndef SCRCMD_PL_NULL
		//08.06.18
		//�b�������̑Ώۂ����Ȃ�BG��POS�̎�
		//�A�����OBJ�̈ړ����쒆���̃`�F�b�N�����Ă��Ȃ�
		//
		//�ӂꂠ���L��ȂǂŁA�A�����OBJ�ɑ΂��āA
		//�X�N���v�g�ŃA�j���𔭍s����ƁA
		//�A�j�����s��ꂸ�I���҂��ɂ����Ȃ��Ń��[�v���Ă��܂�
    
		{
			FIELD_OBJ_PTR player_pair =
				FieldOBJSys_MoveCodeSearch( fsys->fldobjsys, MV_PAIR );
			//�y�A�����݂��Ă���
			if (player_pair) {
				//�A������t���O�������Ă��āA�ړ����쒆�Ȃ�
				if( SysFlag_PairCheck(
					SaveData_GetEventWork(fsys->savedata)) == 1
					&& FieldOBJ_StatusBitCheck_Move(player_pair) != 0) {
					
					//�y�A�̓���|�[�Y����
					FieldOBJ_MovePauseClear( player_pair );
					
					//�ړ�����̏I���҂����Z�b�g
					VM_SetWait( core, EvWaitPairObj );
					return 1;
				}
			}
		}
		#endif
	}else{
		EvCmdTalkObjPauseAll( core, wk );
	}
	
	return 1;
}

//--------------------------------------------------------------
/**
 * ��b�C�x���g�p�SOBJ�����~
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"1"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkObjPauseAll( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	FLDMMDLSYS *fmmdlsys = SCRCMD_WORK_GetFldMMdlSys( work );
	FLDMMDL **fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
	FLDMMDL *player = PlayerGetFldMMdl( work );
	FLDMMDL *player_pair = FLDMMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
#ifndef SCRCMD_PL_NULL
	FLDMMDL *other_pair = FieldOBJ_MovePairSearch(*fldobj);
#else
	FLDMMDL *other_pair = NULL;
#endif

	InitStepWatchBit();
	FLDMMDLSYS_PauseMoveProc( fmmdlsys );
	
	if( FLDMMDL_CheckEndAcmd(player) == FALSE ){
		SetStepWatchBit(PLAYER_BIT);
		FLDMMDL_OffStatusBitMoveProcPause( player );
	}
	
	if( FLDMMDL_CheckStatusBitMove(*fmmdl) == TRUE ){
		SetStepWatchBit(OTHER_BIT);
		FLDMMDL_OffStatusBitMoveProcPause( *fmmdl );
	}
	
	if( player_pair ){
		#ifndef SCRCMD_PL_NULL
		if( SysFlag_PairCheck(SaveData_GetEventWork(fsys->savedata)) == 1
				&& FieldOBJ_StatusBitCheck_Move(player_pair) != 0) {
			SetStepWatchBit(PLAYER_PAIR_BIT);
			FieldOBJ_MovePauseClear( player_pair );
		}
		#else
		GF_ASSERT( 0 );
		#endif
	}
	
	if( other_pair ){
		if( FLDMMDL_CheckStatusBitMove(other_pair) == TRUE ){
			SetStepWatchBit(OTHER_PAIR_BIT);
			FLDMMDL_OffStatusBitMoveProcPause( other_pair );
		}
	}
	
	VMCMD_SetWait( core, EvWaitTalkObj );
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief	OBJ����I���҂�
 */
//--------------------------------------------------------------
static BOOL EvWaitTalkObj( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	FLDMMDL **fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
	FLDMMDL *player = PlayerGetFldMMdl( work );
	FLDMMDLSYS *fmmdlsys = SCRCMD_WORK_GetFldMMdlSys( work );

	//���@�����~�`�F�b�N
	if( CheckStepWatchBit(PLAYER_BIT) &&
		FLDMMDL_CheckEndAcmd(player) == TRUE ){
		FLDMMDL_OnStatusBitMoveProcPause( player );
		ResetStepWatchBit(PLAYER_BIT);
	}
	
	//�b�������Ώۓ����~�`�F�b�N
	if( CheckStepWatchBit(OTHER_BIT) &&
		FLDMMDL_CheckStatusBitMove(*fmmdl) == FALSE ){
		FLDMMDL_OnStatusBitMoveProcPause( *fmmdl );
		ResetStepWatchBit(OTHER_BIT);
	}
	
	//���@�̘A����������~�`�F�b�N
	if( CheckStepWatchBit(PLAYER_PAIR_BIT) ){
		FLDMMDL *player_pair = FLDMMDLSYS_SearchMoveCode( fmmdlsys, MV_PAIR );
		
		if( FLDMMDL_CheckStatusBitMove(player_pair) == FALSE ){
			FLDMMDL_OnStatusBitMoveProcPause( player_pair );
			ResetStepWatchBit(PLAYER_PAIR_BIT);
		}
	}
	
	//�b�������Ώۂ̘A����������~�`�F�b�N
	if( CheckStepWatchBit(OTHER_PAIR_BIT) ){
		#ifndef SCRCMD_PL_NULL
		FLDMMDL *other_pair = FieldOBJ_MovePairSearch(*fldobj);
		if (FieldOBJ_StatusBitCheck_Move(other_pair) == 0) {
			FieldOBJ_MovePause(other_pair);
			ResetStepWatchBit(OTHER_PAIR_BIT);
		}
		#else
		GF_ASSERT( 0 );
		#endif
	}
	
	if( step_watch_bit == 0 ){
		return 1;
	}
	
	return 0;
}

//--------------------------------------------------------------
/**
 * �SOBJ����ĊJ
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"1"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjPauseClearAll( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	FLDMMDLSYS *fmmdlsys = SCRCMD_WORK_GetFldMMdlSys( work );
	FLDMMDLSYS_ClearPauseMoveProc( fmmdlsys );
	return 1;
}

//--------------------------------------------------------------
/**
 * �b��������OBJ���@�����ւ̐U�����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjTurn( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	FLDMMDL *jiki,**fmmdl;
	u16 dir;
	
	jiki = PlayerGetFldMMdl( work );
	dir = FLDMMDL_GetDirDisp( jiki );
	
	dir = FLDMMDL_TOOL_FlipDir( dir );
	fmmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );
	
	if( (*fmmdl) == NULL ){
		return 0;
	}
	
	FLDMMDL_SetDirDisp( *fmmdl, dir );
	return 0;
}

//======================================================================
//  �͂��A�������@����
//======================================================================
//--------------------------------------------------------------
/**
 * �u�͂��E�������v����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @retval 1
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdYesNoWin( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_FLDPARAM );
  FLDMENUFUNC **mw	= SCRIPT_GetMemberWork( sc, ID_EVSCR_MENUWORK );
	u16 wk_id			= VMGetU16( core );
  
  *mw = FLDMENUFUNC_AddYesNoMenu( fparam->msgBG, 0 );
  core->vm_register[0] = wk_id;
   
  VMCMD_SetWait( core, EvYesNoWinSelect );
  return 1;
}

static BOOL EvYesNoWinSelect( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FLDMENUFUNC **mw = SCRIPT_GetMemberWork( sc, ID_EVSCR_MENUWORK );
  u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, core->vm_register[0] );
  FLDMENUFUNC_YESNO ret = FLDMENUFUNC_ProcYesNoMenu( *mw );
  
  if( ret == FLDMENUFUNC_YESNO_NULL ){
    return 0;
  }
  
  if( ret == FLDMENUFUNC_YESNO_YES ){
    *ret_wk = 0;
  }else{
    *ret_wk = 1;
  }
  
  FLDMENUFUNC_DeleteMenu( *mw );
  return 1;
}

