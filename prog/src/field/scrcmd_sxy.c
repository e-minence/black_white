//======================================================================
/**
 * @file	scrcmd_trainer.c
 * @brief	�X�N���v�g�R�}���h�F�g���[�i�[�֘A
 * @author	Satoshi Nohara
 * @date	06.06.26
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

#include "field/eventdata_system.h"

#include "scrcmd_sxy.h"
#include "field/field_const.h"

//======================================================================
//	�R�}���h
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMoveBgEventPos( VMHANDLE * core, void * wk )
{
  u16 id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gy = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  
  SCRCMD_WORK *work = wk;
  GAMEDATA *       gamedata = SCRCMD_WORK_GetGameData( work );
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );

  EVENTDATA_MoveBGData( evdata, id, gx, gy, gz );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMoveExitEventPos( VMHANDLE * core, void * wk )
{
  u16 id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gy = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  
  SCRCMD_WORK *work = wk;
  GAMEDATA *       gamedata = SCRCMD_WORK_GetGameData( work );
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );

  EVENTDATA_MoveConnectData( evdata, id, gx, gy, gz );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMoveNPCDataPos( VMHANDLE * core, void * wk )
{
  u16 id = SCRCMD_GetVMWorkValue( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gy = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  s32 y;
  
  SCRCMD_WORK *work = wk;
  GAMEDATA *       gamedata = SCRCMD_WORK_GetGameData( work );
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );

  y = FIELD_CONST_GRID_FX32_SIZE * gy;
  EVENTDATA_MoveNPCData( evdata, id, gx, y, gz );
  return VMCMD_RESULT_CONTINUE;
}

