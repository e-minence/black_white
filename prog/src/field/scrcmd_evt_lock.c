//======================================================================
/**
 * @file  scrcmd_evt_lock.c
 * @brief  �X�N���v�g�R�}���h�F�z�z�n�C�x���g���b�N�`�F�b�N
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_evt_lock.h"

#include "evt_lock.h"

//--------------------------------------------------------------
/**
 * �C�x���g���b�N�`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckEvtLock( VMHANDLE *core, void *wk )
{
  MISC *misc;
  MYSTATUS *my;
  SCRCMD_WORK *work = wk;
  u16 lock_no = SCRCMD_GetVMWorkValue( core, work );
  u16 *ret = SCRCMD_GetVMWork( core, work );
  {
    GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
    GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
    misc = GAMEDATA_GetMiscWork(gdata);
    my = GAMEDATA_GetMyStatus(gdata);
  }

  *ret = EVTROCK_ChekcEvtRock( misc, lock_no, my );
  
  return VMCMD_RESULT_CONTINUE;
}
