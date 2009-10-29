/*
 *  @file scrcmd_encount.c
 *  @brief  �X�N���v�g�@�쐶��G���J�E���g��
 *  @author Miyuki Iwasawa
 *  @date   09.10.28
 */

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_battle.h"

#include "scrcmd_encount.h"

//--------------------------------------------------------------
/**
 * �쐶�퓬�Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildBattleSet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	
	u16 mons_no = SCRCMD_GetVMWorkValue( core, work );
	u16 mons_lv = SCRCMD_GetVMWorkValue( core, work );
  u16 flags = SCRCMD_GetVMWorkValue( core, work );
  
  {
    GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
    SCRIPT_FLDPARAM * fparam = SCRIPT_GetFieldParam( sc );
    GMEVENT *ev_battle =
      (GMEVENT*)FIELD_ENCOUNT_SetWildEncount(
          FIELDMAP_GetEncount( fparam->fieldMap ), mons_no, mons_lv, flags );
    SCRIPT_CallEvent( sc, ev_battle );
  }
	return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �쐶�폟��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	VMCMD_RESULT_SUSPEND
 *
 * @todo
 * ����͂��ꂪ�Ă΂�Ȃ��Ă����퓮�삵�Ă���B
 * �������ɃX�N���v�g����OBJ����Ȃǂ��s���A���̌�t�F�[�h�C������B
 * ���̍ۂ̃t�F�[�h�C�������Ȃǂ����������\��B
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildWin( VMHANDLE * core, void *wk )
{
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �쐶�퓬 �Đ�R�[�h�`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWildBattleRetryCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  BtlResult res = GAMEDATA_GetLastBattleResult(gdata);
	
	u16* ret_wk = SCRCMD_GetVMWork( core, work );
  *ret_wk = FIELD_BATTLE_GetWildBattleRevengeCode( res );
	return VMCMD_RESULT_CONTINUE;
}

