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
#include "event_intrude_subscreen.h"
#include "fieldmap/zone_id.h"
#include "field/zonedata.h"


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

///�~�b�V�������X�g��M����\����
typedef struct{
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  WORDSET *wordset;
  u16 *ret_wk;      ///<���ʑ����@TRUE:����I���@FALSE:�G���[�B���m���X��ʂ��J���Ă͂����Ȃ�
}EVENT_MISSION_CHOICE_LIST;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT * EVENT_Intrude_MissionChoiceListReq(GAMESYS_WORK * gsys, u16 *ret_wk);
static GMEVENT_RESULT _event_MissionChoiceListReq( GMEVENT * event, int * seq, void * work );

static GMEVENT * EVENT_Intrude_PalaceInDisguise(GAMESYS_WORK * gsys);
static GMEVENT_RESULT _event_PalaceInDisguise( GMEVENT * event, int * seq, void * work );

static GMEVENT_RESULT _event_MissionStartWait( GMEVENT * event, int * seq, void * work );
static void _event_MissionStartWait_Destructor(EVENT_MISSION_START_WAIT *emsw);

static GMEVENT_RESULT _event_MissionStartWaitWarp( GMEVENT * event, int * seq, void * work );




//==================================================================
/**
 * ���m���X�p�Ƀ~�b�V�����I����⃊�X�g���擾
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
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  
  SCRIPT_CallEvent( scw, EVENT_Intrude_MissionChoiceListReq(gsys, ret_wk) );
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���m���X�p�Ƀ~�b�V�����I����⃊�X�g���擾
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_Intrude_MissionChoiceListReq(GAMESYS_WORK * gsys, u16 *ret_wk)
{
  GMEVENT *event;
  EVENT_MISSION_CHOICE_LIST *emcl;
  
  event = GMEVENT_Create( gsys, NULL, _event_MissionChoiceListReq, sizeof(EVENT_MISSION_CHOICE_LIST) );
  emcl = GMEVENT_GetEventWork(event);
  emcl->ret_wk = ret_wk;
  *(emcl->ret_wk) = FALSE;
  
  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�����֐��F���m���X�p�Ƀ~�b�V�����I����⃊�X�g���擾
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
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  GAME_COMM_SYS_PTR game_comm= GAMESYSTEM_GetGameCommSysPtr(gsys);
  EVENT_MISSION_CHOICE_LIST *emcl = work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  BOOL my_palace = FALSE;
  enum{
    SEQ_INIT,
    SEQ_LIST_REQ,
    SEQ_LIST_RECEIVE_WAIT,
    SEQ_MONOLITH_STATUS_REQ,
    SEQ_MONOLITH_STATUS_RECEIVE_WAIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_LAST_KEY_WAIT,
    SEQ_FINISH,
  };
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL && (*seq) > SEQ_INIT && (*seq) < SEQ_MSG_WAIT){
    OS_TPrintf("intcomm=NULL!! �~�b�V�����I����⃊�X�g�͎擾�ł��Ȃ�\n");
    *seq = SEQ_MSG_WAIT;
  }
  
  switch( *seq ){
  case SEQ_INIT:
	  if(NetErr_App_CheckError()){
      *seq = SEQ_FINISH;  //�G���[���������Ă��鎞�̓��b�Z�[�W���o�����ɏI��
      break;
    }

    if(intcomm != NULL){
      MISSION_Init_List(&intcomm->mission); //�茳�̃��X�g�͏�����
      IntrudeField_MonolithStatus_Init(&intcomm->monolith_status);
    }
    
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
      emcl->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      emcl->msgWin = FLDMSGWIN_AddTalkWin( msgBG, emcl->msgData );
      emcl->wordset = WORDSET_Create(HEAPID_PROC);
    }

	  if(intcomm == NULL || Intrude_GetPalaceArea(intcomm) == GAMEDATA_GetIntrudeMyID(gamedata)){
      FLDMSGWIN_Print( emcl->msgWin, 0, 0, plc_mono_01 );
      my_palace = TRUE;
    }
    else{
      STRBUF *expand_buf = GFL_STR_CreateBuffer(256, HEAPID_FIELDMAP);
      STRBUF *src_buf = GFL_MSG_CreateString( emcl->msgData, plc_mono_02 );
      
      WORDSET_RegisterPlayerName( 
        emcl->wordset, 0, Intrude_GetMyStatus( intcomm, Intrude_GetPalaceArea(intcomm) ) );
      WORDSET_ExpandStr(emcl->wordset, expand_buf, src_buf);
      FLDMSGWIN_PrintStrBuf( emcl->msgWin, 0, 0, expand_buf );
      
      GFL_STR_DeleteBuffer(src_buf);
      GFL_STR_DeleteBuffer(expand_buf);
    }

    if(GFL_NET_GetConnectNum() <= 1){
      OS_TPrintf("�~�b�V�������X�g�F��l�̂��߁A��M�͂��Ȃ�\n");
      if(my_palace == FALSE){
        *(emcl->ret_wk) = FALSE;  //���l�̃��m���X�ɂ���̂Ɏ�����l
      }
      else{
        *(emcl->ret_wk) = TRUE;
      }
      *seq = SEQ_MSG_WAIT;
      break;
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
      (*seq)++;
    }
    break;
  case SEQ_MONOLITH_STATUS_REQ:
    if(IntrudeSend_MonolithStatusReq(intcomm, Intrude_GetPalaceArea(intcomm)) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_MONOLITH_STATUS_RECEIVE_WAIT:
    if(IntrudeField_MonolithStatus_CheckOcc(&intcomm->monolith_status) == TRUE){
      *(emcl->ret_wk) = TRUE;
      *seq = SEQ_MSG_WAIT;
    }
    break;
  
  case SEQ_MSG_WAIT:
    if( FLDMSGWIN_CheckPrintTrans(emcl->msgWin) == TRUE ){
      (*seq)++;
    }
    break;
  case SEQ_MSG_LAST_KEY_WAIT:
    if( GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY ){
      WORDSET_Delete(emcl->wordset);
      FLDMSGWIN_Delete( emcl->msgWin );
      GFL_MSG_Delete( emcl->msgData );
      (*seq)++;
    }
    break;
  case SEQ_FINISH:
    return GMEVENT_RES_FINISH;
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
  if(intcomm != NULL && intcomm->mission_start_event_ended == FALSE && MISSION_GetMissionEntry(&intcomm->mission) == TRUE){
    *ret_wk = TRUE;
  }
  else{
    *ret_wk = FALSE;
  }
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
  if(intcomm == NULL && (*seq) < SEQ_FINISH){
    *seq = SEQ_FINISH;
  }
  
  switch( *seq ){
  case SEQ_INIT:
    intcomm->mission_start_event_ended = TRUE;
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


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �~�b�V�����r���Q���F�~�b�V�����N�����Ă���p���X�փ��[�v���A�J�n�܂őҋ@
 *
 * @param   gsys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
GMEVENT * EVENT_Intrude_MissionStartWait_Warp(GAMESYS_WORK * gsys)
{
  GMEVENT *event;
  
  event = GMEVENT_Create( gsys, NULL, _event_MissionStartWaitWarp, 0 );
  
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
static GMEVENT_RESULT _event_MissionStartWaitWarp( GMEVENT * event, int * seq, void * work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  enum{
    SEQ_WARP,
    SEQ_MISSION_START_WAIT,
    SEQ_FINISH,
  };
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  
  switch( *seq ){
  case SEQ_WARP:
    {
      GMEVENT *child_event;
      NetID warp_netid;
      if(intcomm == NULL){
        warp_netid = GAMEDATA_GetIntrudeMyID(gamedata);
      }
      else{
        MISSION_SetMissionEntry(intcomm, &intcomm->mission);
        warp_netid = MISSION_GetMissionTargetNetID(intcomm, &intcomm->mission);
      }
      //GMEVENT_CallEvent(event, EVENT_IntrudeWarpPalace_NetID(gsys, warp_netid));
      child_event = EVENT_IntrudeTownWarp(gsys, fieldWork, ZONE_ID_PALACE01, TRUE);
      if(child_event == NULL){
        return GMEVENT_RES_FINISH;
      }
      GMEVENT_CallEvent(event, child_event);
    }
    (*seq)++;
    break;
  case SEQ_MISSION_START_WAIT:
    //�]�[��ID���p���X�łȂ��ꍇ�̓��[�v���s
    if(ZONEDATA_IsPalace(PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gsys) )) == TRUE){
      GMEVENT_CallEvent(event, EVENT_Intrude_MissionStartWait(gsys));
    }
    (*seq)++;
    break;
  case SEQ_FINISH:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * ������������p���X�G���A���A�����̃p���X�G���A����_(0)�Ƃ����ꍇ�A
 * ���ڂ̃p���X�G���A�ɂ��邩���擾����
 * 
 * �ʐMID�ƃC�R�[���ł͂Ȃ��A�I�t�Z�b�g�ł��鎖�ɒ���
 *   ��0�̏ꍇ�͎����̃p���X�G���A�ɂ���
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeGetPalaceAreaOffset( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  INTRUDE_COMM_SYS_PTR intcomm;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL || Intrude_GetPalaceArea(intcomm) == GAMEDATA_GetIntrudeMyID(gamedata)){
    *ret_wk = 0;
  }
  else{
    int area_offset = Intrude_GetPalaceArea(intcomm) - GAMEDATA_GetIntrudeMyID(gamedata);
    if(area_offset < 0){
      area_offset += FIELD_COMM_MEMBER_MAX;
    }
    *ret_wk = area_offset;
  }
  return VMCMD_RESULT_CONTINUE;
}

//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * G�p���[�������������Ă��邩�擾
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * TRUE:�����������Ă���
 * FALSE:�����������Ă��Ȃ�
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeCheckEqpGPower( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
	u16 *ret_wk		= SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  INTRUDE_SAVE_WORK * intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( gamedata ) );
  GPOWER_ID gpower_id;
  
  gpower_id = ISC_SAVE_GetGPowerID(intsave);
  if(gpower_id != GPOWER_ID_NULL){
    *ret_wk = TRUE;
  }
  else{
    *ret_wk = FALSE;
  }
  return VMCMD_RESULT_CONTINUE;
}
