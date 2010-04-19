//======================================================================
/**
 * @file  scrcmd_leg_gmk.c
 * @brief  �`���|�P�M�~�b�N�X�N���v�g
 * @author  Saito
 */
//======================================================================
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
#include "scrcmd_leg_gmk.h"

#include "legend_gmk.h"

//--------------------------------------------------------------
/**
 * �M�~�b�N�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdStartLegGmk( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  event = LEGEND_GMK_Start( gsys );

  if (event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}
