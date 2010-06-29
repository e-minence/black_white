//======================================================================
/**
 * @file  scrcmd_gym.c
 * @brief  �X�N���v�g�R�}���h�F�W���֘A
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

#include "scrcmd_gym.h"

#include "gym_init.h"
#include "gym_elec.h"
#include "gym_normal.h"
#include "gym_anti.h"
#include "gym_insect.h"
#include "gym_ground.h"
#include "gym_ground_ent.h"
#include "gym_ice.h"
#include "gym_dragon.h"

//--�d�C--
//--------------------------------------------------------------
/**
 * �d�C�W���X�C�b�`����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymElec_PushSw( VMHANDLE *core, void *wk )
{
  u8 sw_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  sw_idx = VMGetU16( core );

  call_event = GYM_ELEC_ChangePoint(gsys, sw_idx);
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
 * �d�C�W���J�v�Z���g���[�i�[����������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymElec_SetTrEncFlg( VMHANDLE *core, void *wk )
{
  u8 cap_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  cap_idx = VMGetU16( core );
  GYM_ELEC_SetTrEncFlg(gsys, cap_idx);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �d�C�W�����r�d������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymElec_InitSe( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  GYM_ELEC_InitSe( gsys );

  return VMCMD_RESULT_CONTINUE;
}

//--�m�[�}��--
//--------------------------------------------------------------
/**
 * �m�[�}���W���M�~�b�N���b�N����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymNormal_Unrock( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);

  GYM_NORMAL_Unrock(fieldWork);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �m�[�}���W���M�~�b�N���b�N�����`�F�b�N
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymNormal_CheckUnrock( VMHANDLE *core, void *wk )
{
  u16 *ret;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);

  ret = SCRCMD_GetVMWork( core, work );

  if ( GYM_NORMAL_CheckRock(fieldWork) ){
    *ret = 1;
  }else{
    *ret = 0;
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �m�[�}���W���M�~�b�N�{�I�ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymNormal_MoveWall( VMHANDLE *core, void *wk )
{
  u8 wall_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  wall_idx = VMGetU16( core );

  call_event = GYM_NORMAL_MoveWall(gsys, wall_idx);
  if (call_event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }

  SCRIPT_CallEvent( sc, call_event );
  
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;

}

//--�A���`--
//--------------------------------------------------------------
/**
 * �A���`�W���M�~�b�N�@���X�C�b�`�I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymAnti_SwOn( VMHANDLE *core, void *wk )
{
  u8 sw_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  sw_idx = VMGetU16( core );
  call_event = GYM_ANTI_SwOnOff(gsys, sw_idx, 1);
  
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
 * �A���`�W���M�~�b�N�@���X�C�b�`�I�t
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymAnti_SwOff( VMHANDLE *core, void *wk )
{
  u8 sw_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  sw_idx = VMGetU16( core );
  call_event = GYM_ANTI_SwOnOff(gsys, sw_idx, 0);
  
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
 * �A���`�W���M�~�b�N�@�h�A�I�[�v��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymAnti_OpenDoor( VMHANDLE *core, void *wk )
{
  u8 door_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  door_idx = VMGetU16( core );
  call_event = GYM_ANTI_OpenDoor(gsys, door_idx);
  if (call_event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }

  SCRIPT_CallEvent( sc, call_event );  
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;

}

//--��--
//--------------------------------------------------------------
/**
 * ���W���M�~�b�N�@���X�C�b�`�I��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymInsect_SwOn( VMHANDLE *core, void *wk )
{
  u16 *sw_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  sw_idx = SCRCMD_GetVMWork( core, work );
  call_event = GYM_INSECT_CreateSwitchEvt(gsys, *sw_idx);
  
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
 * ���W���M�~�b�N�@�|�[���A�j���N��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymInsect_PoleOn( VMHANDLE *core, void *wk )
{
  u16 *pl_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  pl_idx = SCRCMD_GetVMWork( core, work );
  call_event = GYM_INSECT_CreatePoleEvt(gsys, *pl_idx);
  
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
 * ���W���M�~�b�N�@�g���[�i�[�g���b�v�N��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymInsect_TrTrapOn( VMHANDLE *core, void *wk )
{
  u16 tr_evt_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  tr_evt_idx = VMGetU16( core );

  GF_ASSERT( tr_evt_idx < 2 );
  call_event = GYM_INSECT_CreateTrTrapEvt(gsys, tr_evt_idx);
  
  if (call_event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }

  SCRIPT_CallEvent( sc, call_event );  
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}

//--�n��--
//--------------------------------------------------------------
/**
 * �n�ʃW���M�~�b�N�@���t�g�ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymGround_MvLift( VMHANDLE *core, void *wk )
{
  u16 lift_idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  lift_idx = VMGetU16( core );

  call_event = GYM_GROUND_CreateLiftMoveEvt(gsys, lift_idx);
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
 * �n�ʃW���M�~�b�N�@�o�����t�g�ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymGround_ExitLift( VMHANDLE *core, void *wk )
{
  u16 exit;
  BOOL exit_flg;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  exit = VMGetU16( core );

  if (exit) exit_flg = TRUE;
  else exit_flg = FALSE;

  call_event = GYM_GROUND_CreateExitLiftMoveEvt(gsys, exit_flg);
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
 * �n�ʃW���G���g�����X�M�~�b�N�@�o�����t�g�ړ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymGroundEnt_ExitLift( VMHANDLE *core, void *wk )
{
  u16 exit;
  BOOL exit_flg;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  exit = VMGetU16( core );

  if (exit) exit_flg = TRUE;
  else exit_flg = FALSE;

  call_event = GYM_GROUND_ENT_CreateExitLiftMoveEvt(gsys, exit_flg);
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
 * �n�ʃW�� ������ʒu�Ƀ��t�g�Ǝ��@���Z�b�g�@
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymGround_SetEnterPos( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  GYM_GROUND_SetEnterPos(gsys);
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �n�ʃW�� ������ʒu�Ƀ��t�g�Ǝ��@���Z�b�g�@
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymGroundEnt_SetEnterPos( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  GYM_GROUND_ENT_SetEnterPos(gsys);
  return VMCMD_RESULT_CONTINUE;
}



//--�X--
//--------------------------------------------------------------
/**
 * �X�W���M�~�b�N�@�X�C�b�`�A�j���N��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymIce_SwAnm( VMHANDLE *core, void *wk )
{
  u16 idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  idx = VMGetU16( core );
  call_event = GYM_ICE_CreateSwEvt(gsys, idx);
  
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
 * �X�W���M�~�b�N�@�ǃA�j���N��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymIce_WallAnm( VMHANDLE *core, void *wk )
{
  u16 idx;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  idx = VMGetU16( core );
  call_event = GYM_ICE_CreateWallEvt(gsys, idx);
  
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
 * �X�W���M�~�b�N�@���[���؂�ւ�
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymIce_ChgRail( VMHANDLE *core, void *wk )
{
  u16 idx;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  idx = VMGetU16( core );
  GYM_ICE_ChangeRailMap(gsys, idx);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �h���S���W���M�~�b�N�@�M�~�b�N�C�x���g�R�[��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymDragon_CallGmk( VMHANDLE *core, void *wk )
{
  u16 head;
  u16 arm;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  head = VMGetU16( core );
  arm = VMGetU16( core );

  call_event = GYM_DRAGON_CreateGmkEvt(gsys, head, arm);

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
 * �h���S���W���M�~�b�N�@���@��э~��C�x���g
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymDragon_JumpDown( VMHANDLE *core, void *wk )
{
  u16 dir;
  
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  dir = VMGetU16( core );
  
  call_event = GYM_DRAGON_CreateJumpDownEvt(gsys, dir);

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
 * �h���S���W���M�~�b�N�@�r�M�~�b�N�C�x���g�R�[��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymDragon_MoveArm( VMHANDLE *core, void *wk )
{
  u16 head,arm;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  head = SCRCMD_GetVMWorkValue( core, work );
  arm = SCRCMD_GetVMWorkValue( core, work );

  call_event = GYM_DRAGON_CreateArmMoveEvt(gsys, head, arm);

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
 * �h���S���W���M�~�b�N�@��M�~�b�N�C�x���g�R�[��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGymDragon_MoveHead( VMHANDLE *core, void *wk )
{
  u16 head;
  GMEVENT *call_event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  head = SCRCMD_GetVMWorkValue( core, work );

  call_event = GYM_DRAGON_CreateHeadMoveEvt(gsys, head);

  if (call_event == NULL){
    GF_ASSERT(0);
    return VMCMD_RESULT_SUSPEND;
  }

  SCRIPT_CallEvent( sc, call_event );
  //�C�x���g�R�[������̂ŁA��x�����Ԃ�
  return VMCMD_RESULT_SUSPEND;
}
