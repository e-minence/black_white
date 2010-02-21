//==============================================================================
/**
 * @file    scrcmd_intrude_field.c
 * @brief   �X�N���v�g�R�}���h�F�N��  ��fieldmap�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.12.03(��)
 */
//==============================================================================
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

#include "scrcmd_intrude.h"

#include "field_comm\intrude_types.h"
#include "field_comm\intrude_main.h"
#include "field_comm\intrude_comm_command.h"
#include "field/field_comm/intrude_minimono.h"
#include "field/field_comm/intrude_monolith.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "field/field_comm/intrude_field.h"
#include "field\field_comm\intrude_work.h"
#include "field/field_comm/intrude_mission_field.h"

#include "message.naix"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_msg.h"

#include "event_intrude.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�p���XIN���̋����ϑ��C�x���g �Ǘ����[�N
typedef struct{
  INTRUDE_EVENT_DISGUISE_WORK ev_diswork;
}PALACE_IN_DISGUISE_WORK;

///MissionStartWait����\����
typedef struct{
  INTRUDE_EVENT_MSGWORK iem;
}EVENT_MISSION_START_WAIT;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT * EVENT_Intrude_MissionChoiceListReq(GAMESYS_WORK * gsys);
static GMEVENT_RESULT _event_MissionChoiceListReq( GMEVENT * event, int * seq, void * work );

static GMEVENT * EVENT_Intrude_PalaceInDisguise(GAMESYS_WORK * gsys);
static GMEVENT_RESULT _event_PalaceInDisguise( GMEVENT * event, int * seq, void * work );

static GMEVENT_RESULT _event_MissionStartWait( GMEVENT * event, int * seq, void * work );
static void _event_MissionStartWait_Destructor(EVENT_MISSION_START_WAIT *emsw);




//==================================================================
/**
 * �~�b�V�����I����⃊�X�g���擾
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * �ȉ��̏������s���܂��B
 * �P�D�e�Ƀ~�b�V�����I����⃊�X�g�����N�G�X�g
 * �Q�D�~�b�V�����I����⃊�X�g��M�҂�
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeMissionChoiceListReq( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  
  SCRIPT_CallEvent( scw, EVENT_Intrude_MissionChoiceListReq(gsys) );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�~�b�V�����I����⃊�X�g���擾
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_Intrude_MissionChoiceListReq(GAMESYS_WORK * gsys)
{
  GMEVENT *event;
  
  event = GMEVENT_Create( gsys, NULL, _event_MissionChoiceListReq, 0 );
  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�����֐��F�~�b�V�����I����⃊�X�g���擾
 *
 * @param   event		
 * @param   seq		
 * @param   work		
 *
 * @retval  GMEVENT_RESULT		
 * 
 * �ȉ��̏������s���܂��B
 * �P�D�e�Ƀ~�b�V�����I����⃊�X�g�����N�G�X�g
 * �Q�D�~�b�V�����I����⃊�X�g��M�҂�
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _event_MissionChoiceListReq( GMEVENT * event, int * seq, void * work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm= GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  enum{
    SEQ_INIT,
    SEQ_LIST_REQ,
    SEQ_LIST_RECEIVE_WAIT,
  };
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    OS_TPrintf("intcomm=NULL!! �~�b�V�����I����⃊�X�g�͎擾�ł��Ȃ�\n");
    return GMEVENT_RES_FINISH;
  }
  
  switch( *seq ){
  case SEQ_INIT:
    MISSION_Init_List(&intcomm->mission); //�茳�̃��X�g�͏�����
    if(GFL_NET_GetConnectNum() <= 1){
      OS_TPrintf("�~�b�V�������X�g�F��l�̂��߁A��M�͂��Ȃ�\n");
      return GMEVENT_RES_FINISH;  //������l�̎��͂��̂܂�FINISH
    }
    (*seq)++;
    //break;
  case SEQ_LIST_REQ:
    if(IntrudeSend_MissionListReq(intcomm, Intrude_GetPalaceArea(intcomm)) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_LIST_RECEIVE_WAIT:
    if(MISSION_MissionList_CheckOcc(
        &intcomm->mission.list[Intrude_GetPalaceArea(intcomm)]) == TRUE){
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * ���@�̕ϑ����Ă���OBJCODE���擾
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * �ϑ����Ă��Ȃ����́u�O�v���Ԃ�܂�
 */
//==================================================================
VMCMD_RESULT EvCmdGetDisguiseCode( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL || intcomm->intrude_status_mine.disguise_no == DISGUISE_NO_NULL){
    *ret_wk = 0;
  }
  else{
    *ret_wk = intcomm->intrude_status_mine.disguise_no;
  }
  OS_TPrintf("get disguise = %d\n", *ret_wk);
  return VMCMD_RESULT_CONTINUE;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �p���XIN���̋����ϑ��C�x���g
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdPalaceInDisguise( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  
  SCRIPT_CallEvent( scw, EVENT_Intrude_PalaceInDisguise(gsys) );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�p���XIN���̋����ϑ��C�x���g
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_Intrude_PalaceInDisguise(GAMESYS_WORK * gsys)
{
  GMEVENT *event;
  PALACE_IN_DISGUISE_WORK *pidw;
  
  event = GMEVENT_Create( gsys, NULL, _event_PalaceInDisguise, sizeof(PALACE_IN_DISGUISE_WORK) );
  pidw = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear(pidw, sizeof(PALACE_IN_DISGUISE_WORK));
  
  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�����֐��F�p���XIN���̋����ϑ��C�x���g
 *
 * @param   event		
 * @param   seq		
 * @param   work		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _event_PalaceInDisguise( GMEVENT * event, int * seq, void * work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm= GAMESYSTEM_GetGameCommSysPtr(gsys);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  PALACE_IN_DISGUISE_WORK *pidw = work;
  INTRUDE_COMM_SYS_PTR intcomm;
  enum{
    SEQ_INIT,
    SEQ_MAIN,
  };
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
  //�V�[�P���X�̗���K��NULL�`�F�b�N�̕K�v�Ȃ�
  //  return GMEVENT_RES_FINISH;
  }
  
  switch( *seq ){
  case SEQ_INIT:
    {
      const MYSTATUS *myst = GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(gsys) );
      u16 disguise_code;
      u8 disguise_type, disguise_sex;
      Intrude_GetNormalDisguiseObjCode(myst, &disguise_code, &disguise_type, &disguise_sex);
      IntrudeEvent_Sub_DisguiseEffectSetup(
        &pidw->ev_diswork, gsys, fieldWork, disguise_code, disguise_type, disguise_sex);
    }
    (*seq)++;
    break;
  case SEQ_MAIN:
    if(IntrudeEvent_Sub_DisguiseEffectMain(&pidw->ev_diswork, intcomm) == TRUE){
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �~�b�V�����G���g���[���Ă��邩���擾
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * TRUE:�G���g���[���Ă���@FALSE:�G���g���[���Ă��Ȃ�
 */
//==================================================================
VMCMD_RESULT EvCmdGetMissionEntry( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm != NULL && MISSION_GetMissionEntry(&intcomm->mission) == TRUE){
    *ret_wk = TRUE;
  }
  else{
    *ret_wk = FALSE;
  }
  OS_TPrintf("get mission entry = %d\n", *ret_wk);
  return VMCMD_RESULT_CONTINUE;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �~�b�V�����G���g���[��A�J�n�܂őҋ@
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 * �ȉ��̏������s���܂��B
 * �P�D�~�b�V�������J�n�����܂ő҂�
 * �Q�D�ϐg�C�x���g�ֈڍs
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeMissionStartWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  
  SCRIPT_CallEvent( scw, EVENT_Intrude_MissionStartWait(gsys) );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�~�b�V�����G���g���[��A�J�n�܂őҋ@
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
GMEVENT * EVENT_Intrude_MissionStartWait(GAMESYS_WORK * gsys)
{
  GMEVENT *event;
  EVENT_MISSION_START_WAIT *emsw;
  
  event = GMEVENT_Create( gsys, NULL, _event_MissionStartWait, sizeof(EVENT_MISSION_START_WAIT) );
  emsw = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear(emsw, sizeof(EVENT_MISSION_START_WAIT));
  
  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�����֐��F�~�b�V�����G���g���[��A�J�n�܂őҋ@
 *
 * @param   event		
 * @param   seq		
 * @param   work		
 *
 * @retval  GMEVENT_RESULT		
 * 
 * �ȉ��̏������s���܂��B
 * �P�D�~�b�V�������J�n�����܂ő҂�
 * �Q�D�ϐg�C�x���g�ֈڍs
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _event_MissionStartWait( GMEVENT * event, int * seq, void * work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  EVENT_MISSION_START_WAIT *emsw = work;
  INTRUDE_COMM_SYS_PTR intcomm;
  enum{
    SEQ_INIT,
    SEQ_MISSION_START_WAIT,
    SEQ_DISGUISE_EVENT_CHANGE,
    SEQ_FINISH,
  };
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL && (*seq) > SEQ_INIT && (*seq) < SEQ_FINISH){
    *seq = SEQ_FINISH;
  }
  
  switch( *seq ){
  case SEQ_INIT:
    IntrudeEventPrint_SetupFieldMsg(&emsw->iem, gsys);
    if(MISSION_FIELD_CheckStatus(&intcomm->mission) == MISSION_STATUS_EXE){
      *seq = SEQ_DISGUISE_EVENT_CHANGE; //���Ƀ~�b�V�������n�܂��Ă���ꍇ�͂����ɕϐg�C�x���g��
    }
    else{
      IntrudeEventPrint_Print(&emsw->iem, msg_invasion_mission_sys005, 0, 0);
      (*seq)++;
    }
    break;
  case SEQ_MISSION_START_WAIT:
    {
      MISSION_STATUS mst = MISSION_FIELD_CheckStatus(&intcomm->mission);
      switch(mst){
      case MISSION_STATUS_ENTRY:
      case MISSION_STATUS_READY:
        break;
      case MISSION_STATUS_EXE:
      default:  //�~�b�V�����Q�����M���M���������ꍇ�ȂǂŊ��Ƀ~�b�V�������I�����Ă���ꍇ������
        (*seq)++;
        break;
      }
    }
    break;
  case SEQ_DISGUISE_EVENT_CHANGE:
    {
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
      GMEVENT *next_event = EVENT_Disguise(gsys, fieldWork, intcomm, HEAPID_FIELDMAP);
      
      _event_MissionStartWait_Destructor(emsw);
      GMEVENT_ChangeEvent(event, next_event);
      return GMEVENT_RES_CONTINUE;  //ChangeEvent�ł�FINISH�����Ă͂����Ȃ�
    }
    break;
    
  case SEQ_FINISH:
    _event_MissionStartWait_Destructor(emsw);
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �������
 *
 * @param   emsw		
 *
 * �r����ChangeEvent�AFINISH�ŕ��򂷂�̂ŏI���������܂Ƃ߂Ă���
 */
//--------------------------------------------------------------
static void _event_MissionStartWait_Destructor(EVENT_MISSION_START_WAIT *emsw)
{
  IntrudeEventPrint_ExitFieldMsg(&emsw->iem);
}
