//======================================================================
/**
 * @file  scrcmd_faceup.c
 * @brief  �X�N���v�g�R�}���h�F��A�b�v�֘A
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

#include "fld_faceup.h"
#include "scrcmd_faceup.h"

FS_EXTERN_OVERLAY(fld_faceup);

static GMEVENT_RESULT EndEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * ��A�b�v�J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFaceup_Start( VMHANDLE *core, void *wk )
{
  u8 back_idx;
  u8 char_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  back_idx = VMGetU16( core );
  char_idx = VMGetU16( core );

  //�I���`�F�b�N�t���O���I��
  SCREND_CHK_SetBitOn(SCREND_CHK_FACEUP);

  GFL_OVERLAY_Load( FS_OVERLAY_ID(fld_faceup) );		//�I�[�o�[���C���[�h

  call_event = FLD_FACEUP_Start(back_idx, char_idx, gsys);
  if (call_event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }

  SCRIPT_CallEvent( sc, call_event );
  
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ��A�b�v�I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFaceup_End( VMHANDLE *core, void *wk )
{
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  call_event = GMEVENT_Create( gsys, NULL, EndEvt, 0 );
  if (call_event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }

  SCRIPT_CallEvent( sc, call_event );

  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * ��A�b�v��ύX
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFaceup_Change( VMHANDLE *core, void *wk )
{
  u8 char_idx;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);

  char_idx = VMGetU16( core );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *�X�N���v�g�I���� ��A�b�v�I���`�F�b�N
 * @param   end_chk     �`�F�b�N�\����
 * @param   seq     �T�u�V�[�P���T
 * @retval  BOOL    TRUE�Ń`�F�b�N�I��
 */
//--------------------------------------------------------------
BOOL SCREND_CheckEndFaceup(SCREND_CHECK *end_check , int *seq)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(end_check->gsys);
  FLD_FACEUP_Release( fieldWork );
  //�I���`�F�b�N�t���O���I�t
  SCREND_CHK_SetBitOff(SCREND_CHK_FACEUP);
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(fld_faceup));		//�I�[�o�[���C�A�����[�h

  return  TRUE;
}

//--------------------------------------------------------------
/**
 * �I���Ď��C�x���g
 * @param     event	            �C�x���g�|�C���^
 * @param     seq               �V�[�P���T
 * @param     work              ���[�N�|�C���^
 * @return    GMEVENT_RESULT   �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EndEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  switch(*seq){
  case 0:
    {
      GMEVENT *call_event;
      //�C�x���g�R�[��
      call_event = FLD_FACEUP_End(gsys);
      GMEVENT_CallEvent(event, call_event);
    }
    (*seq)++;
    break;
  case 1:
    //�I���`�F�b�N�t���O���I�t
    SCREND_CHK_SetBitOff(SCREND_CHK_FACEUP);
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(fld_faceup));		//�I�[�o�[���C�A�����[�h
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}


