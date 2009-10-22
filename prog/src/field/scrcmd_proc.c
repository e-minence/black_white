//======================================================================
/**
 * @file    scrcmd_proc.c
 * @brief   �X�N���v�g�R�}���h�p�֐��@�e��A�v���Ăяo���n(�풓)
 * @author  Miyuki Iwasawa 
 * @date    09.10.22
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
//#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field_sound.h"
#include "field_player.h"
#include "event_fieldmap_control.h"

#include "scrcmd_proc.h"
#include "app/bag.h"
#include "../../../resource/fldmapdata/script/usescript.h"

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldOpen( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldOpen( gsys ));
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�}�b�v�v���Z�X�j��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldClose(gsys, GAMESYSTEM_GetFieldMapWork(gsys)));
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�(���[�N��������L��)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 */
//--------------------------------------------------------------
BOOL SCMD_WaitSubProcEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

	if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  SCRIPT_FreeSubProcWorkPointer(sc);

  return TRUE;
}
//--------------------------------------------------------------
/**
 * @brief �T�u�v���Z�X�̏I����҂��܂�(���[�N�����������)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  TRUE    �I��
 * @retval  FALSE   �҂�
 *
 * ��������ő҂����ꍇ�A�����I�� EvCmdFreeSubProcWork( VMHANDLE *core, void *wk ) ���g���ĉ�����邱��
 */
//--------------------------------------------------------------
BOOL SCMD_WaitSubProcEndNonFree( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

	if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief �ėp�T�u�v���Z�X���[�N����R�}���h
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFreeSubProcWork( VMHANDLE *core, void *wk )
{
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( (SCRCMD_WORK*)wk );
  SCRIPT_FreeSubProcWorkPointer(sc);
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X���Ăяo���܂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ���T�u�v���Z�X���[�N�̉���������s��Ȃ��̂ŁA�K�������I�� EvCmdBagProcWorkFree()���Ăяo������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallBagProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** sub_wk = SCRIPT_SetSubProcWorkPointerAdrs( sc );

  u16 mode = SCRCMD_GetVMWorkValue( core, work );

  if( mode == SCR_BAG_MODE_SELL ){
    mode = BAG_MODE_SELL;
  }else{
    mode = BAG_MODE_FIELD;
  }
  *sub_wk = (void*)BAG_CreateParam( SCRCMD_WORK_GetGameData( work ), NULL, mode );
  
  GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(bag), &ItemMenuProcData, *sub_wk ); 
 
  VMCMD_SetWait( core, SCMD_WaitSubProcEndNonFree );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�v���Z�X�̃��^�[���R�[�h���擾���܂�
 * @param   core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 *
 * ��EvCmdFreeSubProcWork() �̑O�ɌĂяo������
 *
 * �o�b�O�v���Z�X�̏I�����[�h�ƑI���A�C�e��No��Ԃ��܂�
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBagProcResult( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  BAG_PARAM* bp = (BAG_PARAM*)(SCRIPT_SetSubProcWorkPointer( sc ));

  u16* ret_mode = SCRCMD_GetVMWork( core, work );
  u16* ret_item = SCRCMD_GetVMWork( core, work );

  if(bp->next_proc == BAG_NEXTPROC_EXIT ){
    *ret_mode = SCR_PROC_RETMODE_EXIT;
  }else{
    *ret_mode = SCR_PROC_RETMODE_NORMAL;
  }
  *ret_item = bp->ret_item;
  return VMCMD_RESULT_SUSPEND;
}


