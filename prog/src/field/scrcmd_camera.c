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

//static GMEVENT_RESULT WaitTraceStopEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitTraceStopForCamMvEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitCamMovEvt( GMEVENT* event, int* seq, void* work );
static void EndCamera(FIELD_CAMERA *camera);

typedef struct CAM_PARAM_tag
{
  u16 Pitch;
  u16 Yaw;
  fx32 Dist;
  VecFx32 Pos;
  FLD_CAM_MV_PARAM_CHK Chk;
}CAM_PARAM;

// �\���̂��z��̃T�C�Y�ƂȂ��Ă��邩�`�F�b�N
#ifdef PM_DEBUG
#ifdef _NITRO
SDK_COMPILER_ASSERT(sizeof(CAM_PARAM) == 44);
#endif
#endif		//PM_DEBUG


//--------------------------------------------------------------
/**
 * �J�����I������
 * @param   camera   �J�����|�C���^
 * @param   chk     �`�F�b�N�\����
 * @retval none
 */
//--------------------------------------------------------------
static void EndCamera(FIELD_CAMERA *camera)
{
  //���A�p�����[�^���N���A����
  FIELD_CAMERA_ClearRecvCamParam(camera);
  if ( FIELD_CAMERA_CheckTraceSys(camera) )
  {
    //�J�����g���[�X�ĊJ
    FIELD_CAMERA_RestartTrace(camera);
  }
  //�I���`�F�b�N�t���O���I�t
  SCREND_CHK_SetBitOff(SCREND_CHK_CAMERA);
}

//--------------------------------------------------------------
/**
 *�X�N���v�g�I�����J�����I���`�F�b�N
 * @param   end_chk     �`�F�b�N�\����
 * @param   seq     �T�u�V�[�P���T
 * @retval  BOOL    TRUE�Ń`�F�b�N�I��
 */
//--------------------------------------------------------------
BOOL SCREND_CheckEndCamera(SCREND_CHECK *end_check , int *seq)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(end_check->gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  EndCamera(camera);

  return  TRUE;
}

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
 
  //�I���`�F�b�N�t���O���I��
  SCREND_CHK_SetBitOn(SCREND_CHK_CAMERA);
  
  if ( FIELD_CAMERA_CheckTraceSys(camera) )
  {
    //�J�����g���[�X���~���郊�N�G�X�g���o��
    FIELD_CAMERA_StopTraceRequest(camera);
    //�J�����g���[�X��������~����̂�҂C�x���g���R�[��
    {
      GMEVENT *call_event;
      SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
      call_event = GMEVENT_Create( gsys, NULL, WaitTraceStopForCamMvEvt, 0 );
      SCRIPT_CallEvent( sc, call_event );
    }
  }
  else
  {
    //���A�p�����[�^���Z�b�g����
    FIELD_CAMERA_SetRecvCamParam(camera);
  }
  
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

  EndCamera(camera);

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
    param.Core.Distance = VMGetU32( core );
    param.Core.TrgtPos.x = VMGetU32( core );
    param.Core.TrgtPos.y = VMGetU32( core );
    param.Core.TrgtPos.z = VMGetU32( core );
    param.Chk.Shift = FALSE;
    param.Chk.Pitch = TRUE;
    param.Chk.Yaw = TRUE;
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
 * �J�����̈ړ�(ID�w��^)
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_MoveByID( VMHANDLE *core, void *wk )
{

  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  CAM_PARAM cam_param;
  u16 param_id;
  u16 frame;
  param_id = VMGetU16( core );
  frame = VMGetU16( core );
  
  //�A�[�J�C�u���烊�\�[�X��`�����[�h
  GFL_ARC_LoadData(&cam_param, ARCID_SCR_CAM_PRM, param_id);
  
  {
    FLD_CAM_MV_PARAM param;
    param.Core.AnglePitch = cam_param.Pitch;
    param.Core.AngleYaw = cam_param.Yaw;
    param.Core.Distance = cam_param.Dist;
    param.Core.TrgtPos = cam_param.Pos;
    param.Chk = cam_param.Chk;

    NOZOMU_Printf("cam_prm = %d,%d,%d\n",cam_param.Pitch, cam_param.Yaw, cam_param.Dist);

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
  {
    FLD_CAM_MV_PARAM_CHK chk;
    chk.Shift = TRUE;
    chk.Pitch = TRUE;
    chk.Yaw = TRUE;
    chk.Dist = TRUE;
    chk.Fovy = TRUE;
    chk.Pos = TRUE;
    FIELD_CAMERA_RecvLinerParam(camera, &chk, frame);
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �f�t�H���g�J�����ւ̕��A�ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCamera_DefaultCamMove( VMHANDLE *core, void *wk )
{
  u16 frame;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  frame = VMGetU16( core );  
  FIELD_CAMERA_RecvLinerParamDefault( camera, frame);

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

#if 0
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
#endif
//--------------------------------------------------------------
/**
 * �J�����g���[�X������~�҂��C�x���g
 * @note    �g���[�X�I�����ɁA���A�p�p�����[�^���Z�b�g����
 * @param   event�@�@�@       �C�x���g�|�C���^
 * @param   seq�@�@�@�@       �V�[�P���X
 * @param   work�@�@�@�@      ���[�N�|�C���^
 * @return  GMEVENT_RESULT    GMEVENT_RES_FINISH�ŃC�x���g�I��
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WaitTraceStopForCamMvEvt( GMEVENT* event, int* seq, void* work )
{
  BOOL rc;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  
  rc = FIELD_CAMERA_CheckTrace(camera);

  if (!rc){
    //���A�p�����[�^���Z�b�g����
    FIELD_CAMERA_SetRecvCamParam(camera);
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
