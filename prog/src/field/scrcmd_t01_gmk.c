//======================================================================
/**
 * @file  scrcmd_t01_gmk.c
 * @brief  �X�N���v�g�R�}���h�F�s01�M�~�b�N
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_fieldmap_control.h"

#include "scrcmd_t01_gmk.h"
#include "field_gimmick_t01.h"

//--------------------------------------------------------------
/**
 * �M�~�b�N�\��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdT01Gmk_Disp( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  T01_GIMMICK_StartDisp( gsys );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �M�~�b�N����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdT01Gmk_Start( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  T01_GIMMICK_Start( fieldWork );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �M�~�b�N�I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdT01Gmk_WaitEnd( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  event = T01_GIMMICK_CreateEndChkEvt( gsys );
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}



