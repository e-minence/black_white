//======================================================================
/**
 * @file  scrcmd_startmenu.c
 * @brief  �X�^�[�g���j���[�֘A
 * @author  Nozomu Satio
 *
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

#include "scrcmd_startmenu.h"

//--------------------------------------------------------------
/**
 * �X�^�[�g���j���[���ڊJ���t���O�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetStartMenuFlg( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*   work     = (SCRCMD_WORK*)wk;
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( wk );
  
  u16 *type = SCRCMD_GetVMWorkValue( core, work );
  u16 *flg = SCRCMD_GetVMWork( core, work );
  {
    MISC *misc = GAMEDATA_GetMiscWork(gdata);
    (*flg) = MISC_GetStartMenuFlag( misc, type );
  }

  return VMCMD_RESULT_CONTINUE;
}

