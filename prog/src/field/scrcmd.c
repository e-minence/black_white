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
#include "scrcmd.h"
#include "scrcmd_work.h"

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

static VMCMD_RESULT EvCmdTalkWinOpen( VMHANDLE *core, void *wk );
static VMCMD_RESULT EvCmdTalkWinClose( VMHANDLE *core, void *wk );

//static VMCMD_RESULT EvCmdFldTalkWindow( VMHANDLE *core, void *wk );

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
 * @brief	�C�����C���֐��F���[�N���擾����
 * @param	core	���z�}�V�����䃏�[�N�ւ̃|�C���^
 * @return	u16 *	���[�N�ւ̃|�C���^
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
 * @brief	�C�����C���֐��F���[�N����l���擾����
 * @param	core	���z�}�V�����䃏�[�N�ւ̃|�C���^
 * @return	u16		�l
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
//	��{�V�X�e������
//======================================================================
//--------------------------------------------------------------
/**
 * �m�n�o���߁i�Ȃɂ����Ȃ��j
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdNop( VMHANDLE *core, void *wk )
{
	return 0;
}

//--------------------------------------------------------------
/**
 * �������Ȃ��i�f�o�b�K�ň����|���邽�߂̖��߁j
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdDummy( VMHANDLE *core, void *wk )
{
	return 0;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�̏I��
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"1"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *  
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	r1		�l�P
 * @param	r2		�l�Q
 *
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"1"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"1"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 *
 * @li	EVCMD_JUMP
 *
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 *
 * @li	EVCMD_CALL
 *
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
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
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	���1
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
//--------------------------------------------------------------
/**
 * �o�^���ꂽ�P����g���ĕ�����W�J�@���b�Z�[�W�\��(MSG_ALLPUT��)
 *
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 *
 * @return	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkMsgAllPut( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	u8 msg_id = VMGetU8(core);
	FLDMSGWIN *msgWin;
	
	msgWin = SCRCMD_WORK_GetFldMsgWin( work );
	FLDMSGWIN_Print( msgWin, 0, 0, msg_id );
	return 0;
}

//======================================================================
//	�t�B�[���h�@��b�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�\��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkWinOpen( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc;
	SCRIPT_FLDPARAM *fparam;
	GFL_MSGDATA *msgData;
	FLDMSGWIN *msgWin;
	u8 *win_open_flag;
	
	sc = SCRCMD_WORK_GetScriptWork( work );
	fparam = SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_FLDPARAM );
	msgData = SCRCMD_WORK_GetMsgData( work );
	msgWin = FLDMSGWIN_AddTalkWin( fparam->msgBG, msgData );
	SCRCMD_WORK_SetFldMsgWin( work, msgWin );

	win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
	*win_open_flag = TRUE;	//ON;
	return 0;
}

//--------------------------------------------------------------
/**
 * ��b�E�B���h�E�����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"0"
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTalkWinClose( VMHANDLE *core, void *wk )
{
	SCRCMD_WORK *work = wk;
	SCRIPT_WORK *sc;
	FLDMSGWIN *msgWin;
	u8 *win_open_flag;
	
	msgWin = SCRCMD_WORK_GetFldMsgWin( work );
	FLDMSGWIN_Delete( msgWin );
	
	sc = SCRCMD_WORK_GetScriptWork( work );
	win_open_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_OPEN_FLAG );
	*win_open_flag = FALSE;	//OFF;
	return 0;
}

//======================================================================
//	���샂�f��	
//======================================================================

