//======================================================================
/**
 * @file  scrcmd_fldci.c
 * @brief  �t�B�[���h�J�b�g�C���X�N���v�g�R�}���h�p�֐�
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

#include "scrcmd_fldci.h"
#include "fld3d_ci.h"

//--------------------------------------------------------------
/**
 * �t�B�[���h�J�b�g�C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFldCi_CallCutin( VMHANDLE *core, void *wk )
{
  u16 cutin_no;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD3D_CI_PTR ptr = FIELDMAP_GetFld3dCiPtr(fieldWork);

  cutin_no = VMGetU16( core );

  //�J�b�g�C���C�x���g���R�[��
  {
    GMEVENT *call_event;
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    call_event = FLD3D_CI_CreateCutInEvt(gsys, ptr, cutin_no);
    SCRIPT_CallEvent( sc, call_event );
  }
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �|�P�����J�b�g�C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFldCi_CallPokeCutin( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*    work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD3D_CI_PTR ptr = FIELDMAP_GetFld3dCiPtr(fieldWork);

  u16 pos = SCRCMD_GetVMWorkValue( core, work );
  
  //�J�b�g�C���C�x���g���R�[��
  {
    GMEVENT *call_event;
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    call_event = FLD3D_CI_CreatePokeCutInEvtTemoti( gsys, ptr, pos );
    SCRIPT_CallEvent( sc, call_event );
  }
  
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * NPC����ԃJ�b�g�C��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFldCi_CallNpcFlyCutin( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*    work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD3D_CI_PTR ptr = FIELDMAP_GetFld3dCiPtr(fieldWork);

  u16 obj_id = SCRCMD_GetVMWorkValue( core, work );
  
  //�J�b�g�C���C�x���g���R�[��
  {
    GMEVENT *call_event;
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    call_event = FLD3D_CI_CreateNpcFlyCutInEvt( gsys, ptr, obj_id );
    SCRIPT_CallEvent( sc, call_event );
  }
  
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}


