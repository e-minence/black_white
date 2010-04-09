//======================================================================
/**
 * @file  scrcmd_d12_gmk.c
 * @brief  �X�N���v�g�R�}���h�FD12�M�~�b�N
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

#include "scrcmd_d12_gmk.h"
#include "field_gimmick_d12.h"

FS_EXTERN_OVERLAY(fld_faceup);

static GMEVENT_RESULT EndEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * ��A�b�v�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdD12Gmk_Vanish( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  D12_GIMMICK_Vanish( gsys );

  return VMCMD_RESULT_CONTINUE;
}


