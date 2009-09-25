//======================================================================
/**
 * @file  scrcmd.c
 * @brief  �X�N���v�g�R�}���h�p�֐�
 * @author  Satoshi Nohara
 * @date  05.08.04
 *
 * 05.04.26 Hiroyuki Nakamura
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"

#include "scrcmd.h"
#include "scrcmd_work.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field_sound.h"
#include "field_player.h"

#include "tr_tool/tr_tool.h"


#include "scrcmd_trainer.h"
#include "scrcmd_sound.h"
#include "scrcmd_musical.h"
#include "scrcmd_word.h"
#include "scrcmd_item.h"
#include "scrcmd_mapchange.h"
#include "scrcmd_menuwin.h"
#include "scrcmd_fldmmdl.h"
#include "scrcmd_gym.h"
#include "scrcmd_pokemon.h"
#include "scrcmd_screeneffects.h"
#include "scrcmd_enviroments.h"


#include "../../../resource/fldmapdata/script/usescript.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// ���W�X�^��r�̌��ʒ�`
//--------------------------------------------------------------
enum {
  MINUS_RESULT = 0,  //��r���ʂ��}�C�i�X
  EQUAL_RESULT,    //��r���ʂ��C�R�[��
  PLUS_RESULT      //��r���ʂ��v���X
};


//======================================================================
//  proto
//======================================================================
//parts
static u16 getZoneID( SCRCMD_WORK *work );

//data
const u32 ScriptCmdMax;
const VMCMD_FUNC ScriptCmdTbl[];

static const u8 ConditionTable[6][3];

//======================================================================
//  ��{�V�X�e������
//======================================================================
//--------------------------------------------------------------
/**
 * �m�n�o���߁i�Ȃɂ����Ȃ��j
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdNop( VMHANDLE *core, void *wk )
{
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �������Ȃ��i�f�o�b�K�ň����|���邽�߂̖��߁j
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdDummy( VMHANDLE *core, void *wk )
{
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�̏I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdEnd( VMHANDLE *core, void *wk )
{
  VM_End( core );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �E�F�C�g���� �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitTime(VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u32 * counter = &core->vm_register[0];

  (*counter)--;
  if( *counter == 0 ){ 
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTimeWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 num        = VMGetU16( core );
  
  //���z�}�V���̔ėp���W�X�^�ɃE�F�C�g���i�[
  core->vm_register[0] = num;
  VMCMD_SetWait( core, EvWaitTime );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�R�[��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 * @li  EVCMD_CALL
 *  �\�L�F  EVCMD_CALL  CallOffset(s16)
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdGlobalCall( VMHANDLE *core, void *wk )
{
  s32  pos = (s32)VMGetU32(core);
  VMCMD_Call( core, (VM_CODE *)(core->adrs+pos) );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g���^�[��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdRet( VMHANDLE *core, void *wk )
{
  VMCMD_Return( core );
  return VMCMD_RESULT_CONTINUE;
}

#if 0
static VMCMD_RESULT EvCmdDebugWatch(VMHANDLE * core, void *wk )
{
  u16 value = SCRCMD_GetVMWorkValue(core);
  KAGAYA_Printf("SCR WORK: %d\n", value);
  return VMCMD_RESULT_CONTINUE;
}
#endif

//======================================================================
//  �f�[�^���[�h�E�X�g�A�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^��1byte�̒l���i�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegValue( VMHANDLE *core, void *wk )
{
  u8  r = VMGetU8( core );
  core->vm_register[r] = VMGetU8( core );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^��4byte�̒l���i�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegWData( VMHANDLE *core, void *wk )
{
  u8  r;
  u32  wdata;
  
  r = VMGetU8( core );
  wdata = VMGetU32( core );
  core->vm_register[r] = wdata;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�ɃA�h���X���i�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegAdrs( VMHANDLE *core, void *wk )
{
  u8  r;
  VM_CODE * adrs;

  r = VMGetU8( core );
  adrs = (VM_CODE *)VMGetU32( core );
  core->vm_register[r] = *adrs;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ɒl����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadAdrsValue( VMHANDLE *core, void *wk )
{
  VM_CODE * adrs;
  u8  r;

  adrs = (VM_CODE *)VMGetU32( core );
  r = VMGetU8( core );
  *adrs = r;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ɉ��z�}�V���̔ėp���W�X�^�̒l����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadAdrsReg( VMHANDLE *core, void *wk )
{
  VM_CODE * adrs;
  u8  r;
  
  adrs = (VM_CODE *)VMGetU32( core );
  r = VMGetU8( core);
  *adrs = core->vm_register[r];
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�̒l��ėp���W�X�^�ɃR�s�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadRegReg( VMHANDLE *core, void *wk )
{
  u8  r1, r2;

  r1 = VMGetU8( core );
  r2 = VMGetU8( core );
  core->vm_register[r1] = core->vm_register[r2];
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ɃA�h���X�̒��g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadAdrsAdrs( VMHANDLE *core, void *wk )
{
  VM_CODE * adr1;
  VM_CODE * adr2;

  adr1 = (VM_CODE *)VMGetU32( core );
  adr2 = (VM_CODE *)VMGetU32( core );
  *adr1 = * adr2;
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  ��r����
//======================================================================
//--------------------------------------------------------------
/**
 * �Q�̒l���r
 * @param  r1    �l�P
 * @param  r2    �l�Q
 * @retval  "r1 < r2 : MISUS_RESULT"
 * @retval  "r1 = r2 : EQUAL_RESULT"
 * @retval  "r1 > r2 : PLUS_RESULT"
 */
//--------------------------------------------------------------
static u8 EvCmdCmpMain( u32 r1, u32 r2 )
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
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpRegReg( VMHANDLE *core, void *wk )
{
  u8  r1, r2;

  r1 = core->vm_register[ VMGetU8(core) ];
  r2 = core->vm_register[ VMGetU8(core) ];
  core->cmp_flag = EvCmdCmpMain( r1, r2 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�ƒl���r
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpRegValue( VMHANDLE *core, void *wk )
{
  u8  r1, r2;

  r1 = core->vm_register[ VMGetU8(core) ];
  r2 = VMGetU8(core);
  core->cmp_flag = EvCmdCmpMain( r1, r2 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���z�}�V���̔ėp���W�X�^�ƃA�h���X�̒��g���r
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpRegAdrs( VMHANDLE *core, void *wk )
{
  u8  r1, r2;

  r1 = core->vm_register[ VMGetU8(core) ];
  r2 = *(VM_CODE *)VMGetU32(core);
  core->cmp_flag = EvCmdCmpMain( r1, r2 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�Ɖ��z�}�V���̔ėp���W�X�^���r
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpAdrsReg( VMHANDLE *core, void *wk )
{
  u8  r1, r2;

  r1 = *(VM_CODE *)VMGetU32(core);
  r2 = core->vm_register[ VMGetU8(core) ];
  core->cmp_flag = EvCmdCmpMain(r1,r2);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ƒl���r
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpAdrsValue(VMHANDLE * core, void *wk)
{
  u8 r1,r2;

  r1 = *(VM_CODE *)VMGetU32(core);
  r2 = VMGetU8(core);
  core->cmp_flag = EvCmdCmpMain(r1,r2);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �A�h���X�̒��g�ƃA�h���X�̒��g���r
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpAdrsAdrs(VMHANDLE * core, void *wk)
{
  u8 r1,r2;

  r1 = *(VM_CODE *)VMGetU32(core);
  r2 = *(VM_CODE *)VMGetU32(core);
  core->cmp_flag = EvCmdCmpMain(r1,r2);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�ƒl���r
 *
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 *
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpWkValue( VMHANDLE *core, void *wk )
{
  u16 *p;
  u16  r1, r2;
  
  p = SCRCMD_GetVMWork( core, wk );
  r1 = *p;
  r2 = VMGetU16( core );
  core->cmp_flag = EvCmdCmpMain( r1, r2 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�ƃ��[�N���r
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpWkWk( VMHANDLE *core, void *wk )
{
  u16 * wk1;
  u16 * wk2;

  wk1 = SCRCMD_GetVMWork( core, wk );
  wk2 = SCRCMD_GetVMWork( core, wk );
  core->cmp_flag = EvCmdCmpMain( *wk1, *wk2 );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPushValue( VMHANDLE *core, void *wk )
{
  u32 value;
  value = VMGetU16(core);
  VMCMD_Push( core, value );
  TAMADA_Printf("SCRCMD:PUSH_VALUE:%d\n", value);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPushWork( VMHANDLE *core, void *wk )
{
  u32 work_value;
  work_value = SCRCMD_GetVMWorkValue( core, wk );
  VMCMD_Push( core, work_value );
  TAMADA_Printf("SCRCMD:PUSH_WORK:%d\n", work_value);
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPopWork( VMHANDLE *core, void *wk )
{
  u32 value;
  u16 * work;
  value = VMCMD_Pop( core );
  work = SCRCMD_GetVMWork( core, wk );
  *work = value;
  TAMADA_Printf("SCRCMD:POP_WORK:%d\n", value);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPop( VMHANDLE *core, void *wk )
{
  VMCMD_Pop( core );
  TAMADA_Printf("SCRCMD:POP\n");

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCalcAdd( VMHANDLE * core, void *wk )
{
  u32 val1,val2;
  val2 = VMCMD_Pop( core );
  val1 = VMCMD_Pop( core );
  VMCMD_Push( core, val1 + val2 );
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCalcSub( VMHANDLE * core, void *wk )
{
  u32 val1,val2;
  val2 = VMCMD_Pop( core );
  val1 = VMCMD_Pop( core );
  VMCMD_Push( core, val1 - val2 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCalcMul( VMHANDLE * core, void *wk )
{
  u32 val1,val2;
  val2 = VMCMD_Pop( core );
  val1 = VMCMD_Pop( core );
  VMCMD_Push( core, val1 * val2 );
  return VMCMD_RESULT_CONTINUE;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCalcDiv( VMHANDLE * core, void *wk )
{
  u32 val1,val2;
  val2 = VMCMD_Pop( core );
  val1 = VMCMD_Pop( core );
  VMCMD_Push( core, val1 / val2 );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPushFlagValue( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  //u16  flag = VMGetU16( core );
  //�t���OID�̓��[�NID��薾�炩�ɏ������̂ŁA���Ή�
  u16 flag = SCRCMD_GetVMWorkValue( core, work );
  BOOL value  = EVENTWORK_CheckEventFlag( evwork, flag );
  VMCMD_Push( core, value );
  TAMADA_Printf("SCRCMD:Push Flag Value(id=%d, value=%d)\n", flag, value );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ���菈��
 *
 * �X�^�b�N��̓�̒l��POP���āA�w������ł̔�r���s��
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCmpStack( VMHANDLE * core, void *wk )
{
  u16  cond;
  u32 val1, val2, result;
  val2 = VMCMD_Pop( core );
  val1 = VMCMD_Pop( core );
  //core->cmp_flag = EvCmdCmpMain(val1 , val2);
  cond   = VMGetU16(core);

  switch (cond)
  {
  case CMPID_LT:
    result = (val1 < val2);
    break;
  case CMPID_EQ:
    result = (val1 == val2);
    break;
  case CMPID_GT:
    result = (val1 > val2);
    break;
  case CMPID_LT_EQ:
    result = (val1 <= val2);
    break;
  case CMPID_GT_EQ:
    result = (val1 >= val2);
    break;
  case CMPID_NE:   
    result = (val1 != val2);
    break;
  case CMPID_OR:
    result = ( (val1 == TRUE) || (val2 == TRUE) );
    break;
  case CMPID_AND:
    result = ((val1 == TRUE) && (val2 == TRUE));
    break;
  default:
    result = 0;
  }
  VMCMD_Push( core, result );
  TAMADA_Printf("SCRCMD:CMP_STACK:v1(%08x),v2(%08x),cond(%d),result(%d)\n",
      val1, val2, cond, result);
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  ���z�}�V���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���z�}�V���ǉ�(�؂�ւ��͂����A����œ��삵�܂��I)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdVMMachineAdd( VMHANDLE *core, void *wk )
{
  u16 scr_id;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  
  scr_id = VMGetU16(core);

  //���z�}�V���ǉ�
  SCRIPT_AddVMachine( sc, getZoneID(work), scr_id, VMHANDLE_SUB1 );
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���[�J���X�N���v�g���E�F�C�g��Ԃɂ��ċ��ʃX�N���v�g�𓮍� �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvChangeCommonScrWait(VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  if ( SCRIPT_GetVMExists( sc, VMHANDLE_SUB1 ) != TRUE) {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���[�J���X�N���v�g���E�F�C�g��Ԃɂ��āA���ʃX�N���v�g�𓮍삳���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdChangeCommonScr( VMHANDLE *core, void *wk )
{
  u16 scr_id;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  scr_id = VMGetU16(core);

  //���ʃX�N���v�g�ȊO�ɂ��؂�ւ��\�ɂȂ��Ă���I
  //���z�}�V���ǉ�
  SCRIPT_AddVMachine( sc, getZoneID(work), scr_id, VMHANDLE_SUB1 );

  VMCMD_SetWait( core, EvChangeCommonScrWait );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ���ʃX�N���v�g���I�����āA���[�J���X�N���v�g���ĊJ�����܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdChangeLocalScr( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  VM_End( core );
  return VMCMD_RESULT_SUSPEND;
  //return VMCMD_RESULT_CONTINUE;  //���ӁI�@���̌��"END"�ɍs���悤�ɂ���
}

//======================================================================
//  ���򖽗�
//======================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�g�W�����v
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 * @li  EVCMD_JUMP
 *  �\�L�F  EVCMD_JUMP  JumpOffset(s16)
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdJump( VMHANDLE *core, void *wk )
{
  s32  pos;
  pos = (s32)VMGetU32(core);
  VMCMD_Jump( core, (VM_CODE *)(core->adrs+pos) );  //JUMP
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�����W�����v
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdIfJump( VMHANDLE *core, void *wk )
{
  u8  r;
  s32  pos;

  r   = VMGetU8(core);
  pos = (s32)VMGetU32(core);

  if ( r == CMPID_GET)
  {
    if (VMCMD_Pop( core ) != TRUE)
    {
      VMCMD_Jump( core, (VM_CODE *)(core->adrs+pos) );
    }
  }
  else if( ConditionTable[r][core->cmp_flag] == TRUE ){
    VMCMD_Jump( core, (VM_CODE *)(core->adrs+pos) );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�����R�[��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdIfCall( VMHANDLE *core, void *wk )  
{
  u8  r;
  s32  pos;

  r   = VMGetU8(core);
  pos = (s32)VMGetU32(core);

  if( ConditionTable[r][core->cmp_flag] == TRUE ){
    VMCMD_Call( core, (VM_CODE *)(core->adrs+pos) );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �b���|���Ώ�OBJID��r�W�����v
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdObjIDJump( VMHANDLE *core, void *wk )
{
  GF_ASSERT( 0 && "WB���Ή�\n" );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �b���|���Ώ�BG��r�W�����v
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdBgIDJump( VMHANDLE *core, void *wk )
{
  GF_ASSERT( 0 && "WB���Ή�\n" );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�N�����̎�l���̌�����r�W�����v
 * (���݂̌����ł͂Ȃ��̂Œ��ӁI)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdPlayerDirJump( VMHANDLE *core, void *wk )
{
  GF_ASSERT( 0 && "WB���Ή�\n" );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  �C�x���g���[�N�֘A
//======================================================================
//--------------------------------------------------------------
/*
 * �t���O�̃Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdFlagSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  u16  flag = VMGetU16( core );
  EVENTWORK_SetEventFlag( evwork, flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t���O�̃��Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdFlagReset( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  u16  flag = VMGetU16( core );
  EVENTWORK_ResetEventFlag( evwork, flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t���O�̃`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdFlagCheck( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  u16  flag = VMGetU16( core );
  core->cmp_flag = EVENTWORK_CheckEventFlag( evwork, flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�̒l���t���O�i���o�[�Ƃ��ăt���O�`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdFlagCheckWk( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  u16 *flag_work = SCRCMD_GetVMWork( core, work );
  u16 *ret_work  = SCRCMD_GetVMWork( core, work );
  *ret_work = EVENTWORK_CheckEventFlag( evwork, *flag_work );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�̒l���t���O�i���o�[�Ƃ��ăt���O�Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdFlagSetWk( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
  u16 *flag_work = SCRCMD_GetVMWork( core, work );
  EVENTWORK_SetEventFlag( evwork, *flag_work );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  ���[�N����֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���[�N�ɒl�𑫂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdWkAdd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 *work_val;

  work_val = SCRCMD_GetVMWork( core, work );
  *work_val += SCRCMD_GetVMWorkValue( core, work );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N����l������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdWkSub( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 * work_val;
  work_val = SCRCMD_GetVMWork( core, work );
  *work_val -= SCRCMD_GetVMWorkValue( core, work );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�ɒl���i�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadWkValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 *work_val;
  work_val = SCRCMD_GetVMWork( core, work );
  *work_val = VMGetU16( core );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�Ƀ��[�N�̒l���i�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadWkWk( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 * wk1;
  u16 * wk2;
  wk1 = SCRCMD_GetVMWork( core, work );
  wk2 = SCRCMD_GetVMWork( core, work );
  *wk1 = *wk2;
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���[�N�ɒl�����[�N�̒l���i�[
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLoadWkWkValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  u16 * work_val;
  work_val = SCRCMD_GetVMWork( core, work );
  *work_val = SCRCMD_GetVMWorkValue( core, work );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
/// �t�B�[���h�C�x���g���ʏ���
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�C�x���g�J�n���̋��ʏ����܂Ƃ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCommonProcFieldEventStart( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  
  //�S���샂�f���|�[�Y
  return( SCRCMD_SUB_ObjPauseAll(core,work) );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�C�x���g�I�����̋��ʏ����܂Ƃ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdCommonProcFieldEventEnd( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  
  //�T�E���h�J���Y����
  {
    GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
    FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
    FIELD_SOUND_ForcePopBGM( fsnd );
  }
  
  //���샂�f���|�[�Y����
  SCRCMD_SUB_PauseClearAll( work );

  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  �L�[���͊֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[�E�F�C�g �E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitABKey( VMHANDLE * core, void *wk )
{
  int trg = GFL_UI_KEY_GetTrg();
  
  if( trg & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * �L�[�E�F�C�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdABKeyWait( VMHANDLE * core, void *wk )
{
  VMCMD_SetWait( core, EvWaitABKey );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �E�F�C�g���[�`���F�Ō�̃L�[�҂�
 * @retval  TRUE=�I��
 * @retval  FALSE=�҂���Ԃ̂܂�
 */
//--------------------------------------------------------------
static BOOL evWaitLastKey( VMHANDLE * core, void *wk )
{
  enum {
    WAIT_AB_KEY = PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL,
    WAIT_PAD_KEY = PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT,
    WAIT_LAST_KEY = WAIT_AB_KEY | WAIT_PAD_KEY 
  };
  
  u32 trg = GFL_UI_KEY_GetTrg();
  if( (trg & WAIT_LAST_KEY) != 0)
  {
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 * �X�N���v�g�R�}���h�F�Ō�̃L�[�E�F�C�g
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdLastKeyWait( VMHANDLE * core, void *wk )
{
  VMCMD_SetWait( core, evWaitLastKey );
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
// �g���[�i�[�t���O�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �g���[�i�[�t���O�̃Z�b�g(�g���[�i�[ID��n��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTrainerFlagSet( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  u16 flag = SCRCMD_GetVMWorkValue(core,work);  //�g���[�i�[ID��n���I�@���[�N�i���o�[��n���̂̓_���I
  SCRIPT_SetEventFlagTrainer( ev, flag );
  KAGAYA_Printf( "�g���[�i�[�t���O�Z�b�g ID=%d", flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�t���O�̃��Z�b�g(�g���[�i�[ID��n��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTrainerFlagReset( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  u16 flag = SCRCMD_GetVMWorkValue(core,work);  //�g���[�i�[ID��n���I�@���[�N�i���o�[��n���̂̓_���I
  SCRIPT_ResetEventFlagTrainer( ev, flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�t���O�̃`�F�b�N(�g���[�i�[ID��n��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
static VMCMD_RESULT EvCmdTrainerFlagCheck( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  EVENTWORK *ev = GAMEDATA_GetEventWork( gdata );
  u16 flag = SCRCMD_GetVMWorkValue(core,work);  //�g���[�i�[ID��n���I ���[�N�i���o�[��n���̂̓_���I
  core->cmp_flag = SCRIPT_CheckEventFlagTrainer( ev, flag );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//  ���̑�
//======================================================================




//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * ���[�N���擾����B
 * ����2�o�C�g�����[�N���w�肷��ID�Ƃ݂Ȃ��āA���[�N�ւ̃|�C���^���擾����
 * @param  core  ���z�}�V�����䃏�[�N�ւ̃|�C���^
 * @retval  u16 *  ���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
u16 * SCRCMD_GetVMWork( VMHANDLE *core, SCRCMD_WORK *work )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 work_id = VMGetU16(core);
  TAMADA_Printf("WORKID:%04x\n", work_id);
  return SCRIPT_GetEventWork( sc, gdata, work_id );
}

//--------------------------------------------------------------
/**
 * ���[�N����l���擾����B
 * ����2�o�C�g��SVWK_START�i0x4000�ȉ��j�ł���Βl�Ƃ��Ď󂯎��B
 * ����ȏ�̏ꍇ�̓��[�N���w�肷��ID�Ƃ݂Ȃ��āA���̃��[�N����l���擾����
 * @param  core  ���z�}�V�����䃏�[�N�ւ̃|�C���^
 * @retval  u16    �l
 */
//--------------------------------------------------------------
u16 SCRCMD_GetVMWorkValue( VMHANDLE * core, SCRCMD_WORK *work )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 work_id = VMGetU16(core);
  TAMADA_Printf("WORKID:%04x\n", work_id);
  return SCRIPT_GetEventWorkValue( sc, gdata, work_id );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 getZoneID( SCRCMD_WORK *work )
{
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  u16 zone_id = PLAYERWORK_getZoneID( player );
  return zone_id;
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
///  �X�N���v�g�R�}���h�e�[�u��
//--------------------------------------------------------------
const VMCMD_FUNC ScriptCmdTbl[] = {
  //��{�V�X�e������
  EvCmdNop,        
  EvCmdDummy,        
  EvCmdEnd,
  EvCmdTimeWait,
  EvCmdGlobalCall,
  EvCmdRet,
  
  EvCmdPushValue,
  EvCmdPushWork,
  EvCmdPopWork,
  EvCmdPop,
  EvCmdCalcAdd,
  EvCmdCalcSub,
  EvCmdCalcMul,
  EvCmdCalcDiv,
  EvCmdPushFlagValue,
  EvCmdCmpStack,

  //�f�[�^���[�h�E�X�g�A�֘A
  EvCmdLoadRegValue,
  EvCmdLoadRegWData,
  EvCmdLoadRegAdrs,
  EvCmdLoadAdrsValue,
  EvCmdLoadAdrsReg,
  EvCmdLoadRegReg,
  EvCmdLoadAdrsAdrs,
  
  //��r����
  EvCmdCmpRegReg,
  EvCmdCmpRegValue,
  EvCmdCmpRegAdrs,    
  EvCmdCmpAdrsReg,    
  EvCmdCmpAdrsValue,
  EvCmdCmpAdrsAdrs,
  EvCmdCmpWkValue,
  EvCmdCmpWkWk,
  
  //���z�}�V���֘A
  EvCmdVMMachineAdd,
  EvCmdChangeCommonScr,
  EvCmdChangeLocalScr,
  
  //���򖽗�
  EvCmdJump,
  EvCmdIfJump,
  EvCmdIfCall,
  EvCmdObjIDJump,
  EvCmdBgIDJump,
  EvCmdPlayerDirJump,
    
  //�C�x���g���[�N�֘A
  EvCmdFlagSet,
  EvCmdFlagReset,
  EvCmdFlagCheck,
  EvCmdFlagCheckWk,
  EvCmdFlagSetWk,
  
  //���[�N����֘A
  EvCmdWkAdd,
  EvCmdWkSub,
  EvCmdLoadWkValue,
  EvCmdLoadWkWk,
  EvCmdLoadWkWkValue,
  
  //�t�B�[���h�C�x���g���ʏ���
  EvCmdCommonProcFieldEventStart,
  EvCmdCommonProcFieldEventEnd,

  //�L�[���͊֘A
  EvCmdABKeyWait,
  EvCmdLastKeyWait,
  
  //��b�E�B���h�E
  EvCmdTalkMsg,
  EvCmdTalkMsgAllPut,
  EvCmdTalkWinOpen,
  EvCmdTalkWinClose,
  
  //�����o���E�B���h�E
  EvCmdBalloonWinWrite,
  EvCmdBalloonWinTalkWrite,
  EvCmdBalloonWinClose,
  
  //���샂�f��
  EvCmdObjAnime,
  EvCmdObjAnimeWait,
  EvCmdMoveCodeGet,
  EvCmdObjPosGet,
  EvCmdPlayerPosGet,
  EvCmdObjAdd,
  EvCmdObjDel,
  EvCmdObjAddEvent,
  EvCmdObjDelEvent,
  EvCmdObjPosChange,
  
  //���샂�f���@�C�x���g�֘A
  EvCmdObjTurn,
  
  //�͂��A�������@����
  EvCmdYesNoWin,
  
  //WORDSET�֘A
  EvCmdPlayerName,
  EvCmdItemName,
  EvCmdItemWazaName,
  EvCmdWazaName,
  EvCmdPocketName,
  
  //scrcmd_trainer.c �����g���[�i�[�֘A
  EvCmdEyeTrainerMoveSet,
  EvCmdEyeTrainerMoveSingle,
  EvCmdEyeTrainerMoveDouble,
  EvCmdEyeTrainerTypeGet,
  EvCmdEyeTrainerIdGet,
  
  //scrcmd_trainer.c �g���[�i�[�o�g���֘A
  EvCmdTrainerIdGet,
  EvCmdTrainerBattleSet,
  EvCmdTrainerMultiBattleSet,
  EvCmdTrainerMessageSet,
  EvCmdTrainerTalkTypeGet,
  EvCmdRevengeTrainerTalkTypeGet,
  EvCmdTrainerTypeGet,
  EvCmdTrainerBgmSet,
  EvCmdTrainerLose,
  EvCmdTrainerLoseCheck,
  EvCmdSeacretPokeRetryCheck,
  EvCmdHaifuPokeRetryCheck,
  EvCmd2vs2BattleCheck,
  EvCmdDebugBattleSet,
  EvCmdBattleResultGet,
  
  //�g���[�i�[�t���O�֘A
  EvCmdTrainerFlagSet,
  EvCmdTrainerFlagReset,
  EvCmdTrainerFlagCheck,
  
  //BGM
  EvCmdBgmPlay,
  EvCmdBgmStop,
  EvCmdBgmPlayerPause,
  EvCmdBgmPlayCheck,
  EvCmdBgmFadeOut,
  EvCmdBgmFadeIn,
  EvCmdBgmNowMapPlay,
  
  //SE
  EvCmdSePlay,
  EvCmdSeStop,
  EvCmdSeWait,
  
  //ME
  EvCmdMePlay,
  EvCmdMeWait,
  
  //���j���[
  EvCmdBmpMenuInit,
  EvCmdBmpMenuInitEx,
  EvCmdBmpMenuMakeList,
  EvCmdBmpMenuStart,
  
  //��ʃt�F�[�h
  EvCmdDispFadeStart,
  EvCmdDispFadeCheck,
  
  //�A�C�e���֘A
  EvCmdAddItem,
  EvCmdSubItem,
  EvCmdAddItemChk,
  EvCmdCheckItem,
  EvCmdGetItemNum,
  EvCmdWazaMachineItemNoCheck,
  EvCmdGetPocketID,
  
  //�~���[�W�J���֘A
  EvCmdMusicalCall,
  
  //�}�b�v�J��
  EvCmdMapChangeBySandStream,
  EvCmdMapChange,

  //�W���M�~�b�N�֘A
  EvCmdGymElec_PushSw,

  //���̑�
  EvCmdChangeLangID,
  EvCmdGetRand,
  EvCmdGetNowMsgArcID,
  EvCmdCheckTemotiPokerus,
  EvCmdGetTimeZone,
  EvCmdGetTrainerCardRank,
  EvCmdPokemonRecover,
  EvCmdPokecenRecoverAnime,
  EvCmdGetLangID,
  EvCmdCheckPokemonHP,      //�|�P�����֘A�F�莝��HP�̃`�F�b�N

  //����
  EvCmdVoicePlay,
  EvCmdVoiceWait,
};


//--------------------------------------------------------------
///  �X�N���v�g�R�}���h�̍ő吔
//--------------------------------------------------------------
const u32 ScriptCmdMax = NELEMS(ScriptCmdTbl);

//--------------------------------------------------------------
///  ��������p�e�[�u��
//--------------------------------------------------------------
static const u8 ConditionTable[6][3] =
{
//    MINUS  EQUAL  PLUS
  { TRUE,   FALSE, FALSE },  // LT
  { FALSE, TRUE,  FALSE },  // EQ
  { FALSE, FALSE, TRUE  },  // GT
  { TRUE,  TRUE,  FALSE },  // LE
  { FALSE, TRUE,  TRUE  },  // GE
  { TRUE,  FALSE, TRUE  }    // NE
};
