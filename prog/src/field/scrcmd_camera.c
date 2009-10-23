//======================================================================
/**
 * @file  scrcmd_camera.c
 * @brief  �J��������X�N���v�g�R�}���h�p�֐�
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

#include "scrcmd_camera.h"

static GMEVENT_RESULT WaitTraceStopEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitCamMovEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * �J�����ړ��J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_Start( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  //���A�p�����[�^���Z�b�g����
  FIELD_CAMERA_SetRecvCamParam(camera);
  //�J�����g���[�X���~���郊�N�G�X�g���o��
  FIELD_CAMERA_StopTraceRequest(camera);
  //�J�����g���[�X��������~����̂�҂C�x���g���R�[��
  {
    GMEVENT *call_event;
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    call_event = GMEVENT_Create( gsys, NULL, WaitTraceStopEvt, 0 );
    SCRIPT_CallEvent( sc, call_event );
  }
#if 0
  //�I���`�F�b�N�t���O���I��
  {
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    SCREND_CHECK_WK *chk = SCRIPT_GetScrEndChkWkPtr( sc );
    chk->CameraEndCheck = 1;
  }
#endif
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �J�����ړ��I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_End( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  //���A�p�����[�^���N���A����
  FIELD_CAMERA_ClearRecvCamParam(camera);
  //�J�����g���[�X�ĊJ
  FIELD_CAMERA_RestartTrace(camera);
#if 0
  //�I���`�F�b�N�t���O���I�t
  {
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    SCREND_CHECK_WK *chk = SCRIPT_GetScrEndChkWkPtr( sc );
    chk->CameraEndCheck = 0;
  }
#endif
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �Ώە��ƃJ�����̃o�C���h������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_Purge( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  FIELD_CAMERA_FreeTarget(camera);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �Ώە��ƃJ�������o�C���h����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_Bind( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  FIELD_CAMERA_BindDefaultTarget(camera);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �J�����̈ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_Move( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );


  {
    FLD_CAM_MV_PARAM param;
    u16 frame;
    param.Core.AnglePitch = VMGetU16( core );
    param.Core.AngleYaw = VMGetU16( core );
    param.Core.Distance = VMGetU16( core );
    param.Core.TrgtPos.x = VMGetU32( core );
    param.Core.TrgtPos.y = VMGetU32( core );
    param.Core.TrgtPos.z = VMGetU32( core );
    param.Chk.Shift = FALSE;
    param.Chk.Angle = TRUE;
    param.Chk.Dist = TRUE;
    param.Chk.Fovy = FALSE;
    param.Chk.Pos = TRUE;
    frame = VMGetU16( core );
    FIELD_CAMERA_SetLinerParam(camera, &param, frame);
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �J�����̕��A�ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_RecoverMove( VMHANDLE *core, void *wk )
{
  u16 frame;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  frame = VMGetU16( core );

  FIELD_CAMERA_RecvLinerParam(camera, frame);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �J�����ړ��I���҂�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_WaitMove( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  //�J�����ړ��I���҂�������C�x���g���R�[��
  {
    GMEVENT *call_event;
    SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
    GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
    call_event = GMEVENT_Create( gsys, NULL, WaitCamMovEvt, 0 );
    SCRIPT_CallEvent( sc, call_event );
  }

  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �J�����g���[�X������~�҂��C�x���g
 * @param   event�@�@�@       �C�x���g�|�C���^
 * @param   seq�@�@�@�@       �V�[�P���X
 * @param   work�@�@�@�@      ���[�N�|�C���^
 * @return  GMEVENT_RESULT    GMEVENT_RES_FINISH�ŃC�x���g�I��
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WaitTraceStopEvt( GMEVENT* event, int* seq, void* work )
{
  BOOL rc;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  
  rc = FIELD_CAMERA_CheckTrace(camera);

  if (!rc){
    //��~
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �J�����ړ��I���҂��C�x���g
 * @param   event�@�@�@       �C�x���g�|�C���^
 * @param   seq�@�@�@�@       �V�[�P���X
 * @param   work�@�@�@�@      ���[�N�|�C���^
 * @return  GMEVENT_RESULT    GMEVENT_RES_FINISH�ŃC�x���g�I��
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WaitCamMovEvt( GMEVENT* event, int* seq, void* work )
{
  BOOL rc;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  rc = FIELD_CAMERA_CheckMvFunc(camera);

  if (!rc){
    //�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}











