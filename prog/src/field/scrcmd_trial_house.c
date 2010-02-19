//======================================================================
/**
 * @file  scrcmd_trial_house.c
 * @brief  �X�N���v�g�R�}���h�F�g���C�A���n�E�X
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
#include "scrcmd_trial_house.h"
#include "trial_house.h"

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_Start( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );

  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  if ( *ptr == NULL ) *ptr = TRIAL_HOUSE_Start();
  else GF_ASSERT_MSG(0,"TRIAL_HOUSE_ALLOC_ERROR");

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X�I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_End( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);
  TRIAL_HOUSE_End(ptr);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X �v���C���[�h�̃Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SetPlayMode( VMHANDLE *core, void *wk )
{
  u16 play_mode;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  play_mode = SCRCMD_GetVMWorkValue( core, work );  
  TRIAL_HOUSE_SetPlayMode(*ptr, play_mode );
  return VMCMD_RESULT_CONTINUE;

}

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X �|�P�����I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SelectPoke( VMHANDLE *core, void *wk )
{
  u16 *ret;
  u16 btl_type;
  u16 party_type;
  GMEVENT* event;

  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  ret = SCRCMD_GetVMWork( core, work );
  btl_type = SCRCMD_GetVMWorkValue( core, work );
  party_type = SCRCMD_GetVMWorkValue( core, work );
  
  event = TRIAL_HOUSE_CreatePokeSelEvt(gsys, *ptr, btl_type, party_type, ret );

  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X �g���[�i�[�Z�b�g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SetTrainer( VMHANDLE *core, void *wk )
{
  u16 *obj_id;
  u16 btl_count;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  btl_count = SCRCMD_GetVMWorkValue( core, work );

  *obj_id = TRIAL_HOUSE_MakeTrainer( *ptr, btl_count );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X �퓬�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_CallBattle( VMHANDLE *core, void *wk )
{
  GMEVENT* event;

  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  event = TRIAL_HOUSE_CreateBtlEvt(gsys, *ptr);

  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}


