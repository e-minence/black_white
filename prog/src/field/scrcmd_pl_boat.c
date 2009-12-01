//======================================================================
/**
 * @file  scrcmd_pl_boat.c
 * @brief  �X�N���v�g�R�}���h�F�V���D�֘A
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

#include "field_sound.h"

#include "scrcmd_pl_boat.h"

#include "pleasure_boat.h"

#include "pl_boat_def.h"

//--------------------------------------------------------------
/**
 * �V���D���[�h�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlBoat_Start( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );

  PL_BOAT_WORK_PTR *ptr = GAMEDATA_GetPlBoatWorkPtr(gamedata);

  if ( *ptr == NULL ) *ptr = PL_BOAT_Init();
  else GF_ASSERT_MSG(0,"PL_BOAT_ALLOC_ERROR");
   
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �V���D���[�h�I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlBoat_End( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  
  PL_BOAT_WORK_PTR *ptr = GAMEDATA_GetPlBoatWorkPtr(gamedata);
  PL_BOAT_End(ptr);
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �D���g���[�i�[�����擾
 * @param   core          ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlBoat_GetTrNum( VMHANDLE *core, void *wk )
{
  u16 type;
  u16 *ret;

  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  PL_BOAT_WORK_PTR *ptr = GAMEDATA_GetPlBoatWorkPtr(gamedata);

  type = VMGetU16( core );
  ret = SCRCMD_GetVMWork( core, work );

  *ret = PL_BOAT_GetTrNum(*ptr, type);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �D�����ԋ������Z
 * @param   core          ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlBoat_AddTime( VMHANDLE *core, void *wk )
{
  u16 add_time;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  PL_BOAT_WORK_PTR *ptr = GAMEDATA_GetPlBoatWorkPtr(gamedata);

  add_time = VMGetU16( core );

  PL_BOAT_AddTimeEvt(*ptr, add_time);

  return VMCMD_RESULT_CONTINUE;
}



