//==============================================================================
/**
 * @file    event_mission_battle.c
 * @brief   �~�b�V�����F�u�������ɏ����𒧂߁I�v
 * @author  matsuda
 * @date    2010.02.21(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"

#include "script.h"
#include "event_fieldtalk.h"

#include "print/wordset.h"
#include "event_comm_talk.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_comm_command.h"
#include "field/field_comm/bingo_system.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "field\field_comm\intrude_mission_field.h"
#include "field\field_comm\intrude_work.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_monolith.h"
#include "msg/msg_mission_msg.h"
#include "field/field_comm/intrude_battle.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "item/itemsym.h"
#include "event_intrude.h"
#include "event_comm_common.h"
#include "event_mission_battle.h"
#include "field/intrude_snd_def.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_BATTLE
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
	INTRUDE_BATTLE_PARENT ibp;
	int first_talked;
}EVENT_MISSION_BATTLE;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommMissionBattle_MtoT_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionBattle_TtoM_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionBattle_MtoT_Talked( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionBattle_TtoM_Talked( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  �f�[�^
//==============================================================================
static const struct{
  u16 target_talk[TALK_TYPE_MAX];           ///<�������^�[�Q�b�g�Řb��������
  u16 target_talked[TALK_TYPE_MAX];         ///<�������^�[�Q�b�g�Řb��������ꂽ

  u16 target_battle_ok[TALK_TYPE_MAX];     ///<�������^�[�Q�b�g�Ő퓬OK
  u16 target_battle_after[TALK_TYPE_MAX];  ///<�������^�[�Q�b�g�Ő퓬��

  u16 target_battle_ng[TALK_TYPE_MAX];      ///<�������^�[�Q�b�g�Ő퓬��f����

  u16 mission_talk[TALK_TYPE_MAX];          ///<�������~�b�V�������s�҂Řb��������
  u16 mission_talked[TALK_TYPE_MAX];        ///<�������~�b�V�������s�҂Řb��������ꂽ
  
  u16 mission_battle_ok[TALK_TYPE_MAX];     ///<�������~�b�V�������s�҂Ő퓬OK
  u16 mission_battle_after[TALK_TYPE_MAX];  ///<�������~�b�V�������s�҂Ő퓬��

  u16 mission_battle_ng[TALK_TYPE_MAX];     ///<�������~�b�V�������s�҂Ő퓬��f��ꂽ
}MissionBattleMsgID = {
  { //�������^�[�Q�b�g�Řb��������
    mis_m03_02_t1,
    mis_m03_02_t2,
    mis_m03_02_t3,
    mis_m03_02_t4,
    mis_m03_02_t5,
  },
  { //�������^�[�Q�b�g�Řb��������ꂽ
    mis_m03_01_t1,
    mis_m03_01_t2,
    mis_m03_01_t3,
    mis_m03_01_t4,
    mis_m03_01_t5,
  },
  { //�������^�[�Q�b�g�Ő퓬OK
    mis_m03_03_t1,
    mis_m03_03_t2,
    mis_m03_03_t3,
    mis_m03_03_t4,
    mis_m03_03_t5,
  },
  { //�������^�[�Q�b�g�Ő퓬��
    mis_m03_04_t1,
    mis_m03_04_t2,
    mis_m03_04_t3,
    mis_m03_04_t4,
    mis_m03_04_t5,
  },
  { //�������^�[�Q�b�g�Ő퓬��f����
    mis_m03_05_t1,
    mis_m03_05_t2,
    mis_m03_05_t3,
    mis_m03_05_t4,
    mis_m03_05_t5,
  },

  { //�������~�b�V�������s�҂Řb��������
    mis_m03_01_m1,
    mis_m03_01_m2,
    mis_m03_01_m3,
    mis_m03_01_m4,
    mis_m03_01_m5,
  },
  { //�������~�b�V�������s�҂Řb��������ꂽ
    mis_m03_02_m1,
    mis_m03_02_m2,
    mis_m03_02_m3,
    mis_m03_02_m4,
    mis_m03_02_m5,
  },
  { //�������~�b�V�������s�҂Ő퓬OK
    mis_m03_03_m1,
    mis_m03_03_m2,
    mis_m03_03_m3,
    mis_m03_03_m4,
    mis_m03_03_m5,
  },
  { //�������~�b�V�������s�҂Ő퓬��
    mis_m03_04_m1,
    mis_m03_04_m2,
    mis_m03_04_m3,
    mis_m03_04_m4,
    mis_m03_04_m5,
  },
  { //�������~�b�V�������s�҂Ő퓬��f��ꂽ
    mis_m03_05_m1,
    mis_m03_05_m2,
    mis_m03_05_m3,
    mis_m03_05_m4,
    mis_m03_05_m5,
  },
};



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�����𒧂�  (�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionBattle_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_BATTLE *talk;
  
  GMEVENT_Change(event, CommMissionBattle_MtoT_Talk, sizeof(EVENT_MISSION_BATTLE));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));

  talk->first_talked = FALSE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�����𒧂� (�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionBattle_MtoT_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_BATTLE *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  const MISSION_TYPEDATA_VICTORY *d_vic = (void*)talk->ccew.mdata.cdata.data;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_BATTLE_OK,
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,
    SEQ_BATTLE_START_WAIT,
    SEQ_BATTLE_START_TIMING_WAIT,
    SEQ_BATTLE_BGM,
    SEQ_ENCEFF,
    SEQ_BATTLE_PROC,
    SEQ_BATTLE_AFTER,
    SEQ_BATTLE_AFTER_NEXT,
    SEQ_BATTLE_NG,
    SEQ_TALK_CANCEL,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, Intrude_GetMyStatus(intcomm, talk->ccew.talk_netid) );
    {
      int msg_id;
      if(talk->first_talked == FALSE){
        msg_id = MissionBattleMsgID.mission_talk[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)];
      }
      else{
        msg_id = MissionBattleMsgID.mission_talked[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)];
      }
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_id);
    }
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      INTRUDE_TALK_STATUS talk_status = Intrude_GetTalkAnswer(intcomm);
      OS_TPrintf("menu_answer status = %d\n", talk_status);
      switch(talk_status){
      case INTRUDE_TALK_STATUS_BATTLE:
        *seq = SEQ_BATTLE_OK;
        break;
      case INTRUDE_TALK_STATUS_NG:
      case INTRUDE_TALK_STATUS_CANCEL:
        *seq = SEQ_BATTLE_NG;
        break;
      default:  //�܂��Ԏ������Ă��Ȃ�
        if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
          if(IntrudeSend_TalkCancel(intcomm->talk.talk_netid) == TRUE){
            *seq = SEQ_TALK_CANCEL;
          }
        }
        break;
      }
    }
    break;

  case SEQ_BATTLE_OK:
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionBattleMsgID.mission_battle_ok[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
  	(*seq) = SEQ_SEND_ACHIEVE;
		break;
	case SEQ_SEND_ACHIEVE:
    if(IntrudeSend_MissionAchieve(intcomm, &intcomm->mission) == TRUE){//�~�b�V�����B����e�ɓ`����
      (*seq)++;
    }
    break;
  case SEQ_RECV_WAIT:
		if(MISSION_GetAchieveAnswer(&intcomm->mission) != MISSION_ACHIEVE_NULL){
      (*seq) = SEQ_BATTLE_START_WAIT;
    }
    break;
  case SEQ_BATTLE_START_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      if(IntrudeSend_TargetTiming(intcomm, talk->ccew.talk_netid, INTRUDE_TIMING_MISSION_BATTLE_BEFORE) == TRUE){
        (*seq)++;
      }
    }
    break;
  case SEQ_BATTLE_START_TIMING_WAIT:
    if(Intrude_GetTargetTimingNo(intcomm, INTRUDE_TIMING_MISSION_BATTLE_BEFORE, talk->ccew.talk_netid) == TRUE){
      IntrudeEventPrint_ExitFieldMsg(&talk->ccew.iem);
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_BGM:
    // �퓬�p�a�f�l�Z�b�g
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayBattleBGM(gsys, SND_INTRUDE_BATTLE_BGM));
    (*seq)++;
    break;
  case SEQ_ENCEFF:
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(talk->ccew.fieldWork), event, ENCEFFID_TR_NORMAL);
    (*seq)++;
    break;
  case SEQ_BATTLE_PROC:
    {
      GMEVENT *child_event;
      
      talk->ibp.gsys = gsys;
      talk->ibp.target_netid = talk->ccew.talk_netid;
      talk->ibp.flat_level = d_vic->battle_level;
      child_event = EVENT_FieldSubProc(
        gsys, talk->ccew.fieldWork, NO_OVERLAY_ID, &IntrudeBattleProcData, &talk->ibp);
      GMEVENT_CallEvent(event, child_event);
    }
	  *seq = SEQ_BATTLE_AFTER;
  	break;
  case SEQ_BATTLE_AFTER:
    GMEVENT_CallEvent(event, EVENT_FSND_PopPlayBGM_fromBattle(gsys));
    (*seq)++;
    break;
  case SEQ_BATTLE_AFTER_NEXT:
    IntrudeEventPrint_SetupFieldMsg(&talk->ccew.iem, gsys);
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionBattleMsgID.mission_battle_after[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
    (*seq) = SEQ_LAST_MSG_WAIT;
    break;

  case SEQ_BATTLE_NG:
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionBattleMsgID.mission_battle_ng[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
    (*seq) = SEQ_LAST_MSG_WAIT;
    break;

	case SEQ_TALK_CANCEL:   //��b�L�����Z��
	  *seq = SEQ_END;
	  break;
    
  case SEQ_LAST_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
  	//����Finish����
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�����𒧂�  (�������^�[�Q�b�g�Ń~�b�V�������s�҂ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionBattle_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_BATTLE *talk;

  GMEVENT_Change(event, CommMissionBattle_TtoM_Talk, sizeof(EVENT_MISSION_BATTLE));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
  
  talk->first_talked = FALSE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�����𒧂� (�������^�[�Q�b�g�Ń~�b�V�������s�҂ɘb��������)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionBattle_TtoM_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_BATTLE *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  const MISSION_TYPEDATA_VICTORY *d_vic = (void*)talk->ccew.mdata.cdata.data;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_BATTLE_YESNO_SELECT,
    SEQ_BATTLE_YES,
    SEQ_BATTLE_YES_MSG,
    SEQ_BATTLE_YES_WAIT,
    SEQ_BATTLE_START_TIMING_WAIT,
    SEQ_BATTLE_BGM,
    SEQ_ENCEFF,
    SEQ_BATTLE_PROC,
    SEQ_BATTLE_AFTER,
    SEQ_BATTLE_AFTER_NEXT,
    SEQ_BATTLE_NG,
    SEQ_TALK_CANCEL,   //��b�L�����Z��
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, Intrude_GetMyStatus(intcomm, talk->ccew.talk_netid) );
    {
      int msg_id;
      if(talk->first_talked == FALSE){
        msg_id = MissionBattleMsgID.target_talk[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)];
      }
      else{
        msg_id = MissionBattleMsgID.target_talked[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)];
      }
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_id);
    }
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      IntrudeEventPrint_SetupYesNo(&talk->ccew.iem, gsys);
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_YESNO_SELECT:
    {
      FLDMENUFUNC_YESNO yesno = IntrudeEventPrint_SelectYesNo(&talk->ccew.iem);
      switch(yesno){
      case FLDMENUFUNC_YESNO_YES:
        IntrudeEventPrint_ExitMenu(&talk->ccew.iem);
        *seq = SEQ_BATTLE_YES;
        break;
      case FLDMENUFUNC_YESNO_NO:
        IntrudeEventPrint_ExitMenu(&talk->ccew.iem);
        *seq = SEQ_BATTLE_NG;
        break;
      }
    }
    break;
    
  case SEQ_BATTLE_YES:
    if(IntrudeSend_TalkAnswer(
        intcomm, intcomm->talk.talk_netid, INTRUDE_TALK_STATUS_BATTLE) == TRUE){
      *seq = SEQ_BATTLE_YES_MSG;
    }
    break;
  case SEQ_BATTLE_YES_MSG:
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionBattleMsgID.target_battle_ok[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
    (*seq)++;
    break;
  case SEQ_BATTLE_YES_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      if(IntrudeSend_TargetTiming(intcomm, talk->ccew.talk_netid, INTRUDE_TIMING_MISSION_BATTLE_BEFORE) == TRUE){
        (*seq)++;
      }
    }
    break;
  case SEQ_BATTLE_START_TIMING_WAIT:
    if(Intrude_GetTargetTimingNo(intcomm, INTRUDE_TIMING_MISSION_BATTLE_BEFORE, talk->ccew.talk_netid) == TRUE){
      IntrudeEventPrint_ExitFieldMsg(&talk->ccew.iem);
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_BGM:
    // �퓬�p�a�f�l�Z�b�g
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayBattleBGM(gsys, SND_INTRUDE_BATTLE_BGM));
    (*seq)++;
    break;
  case SEQ_ENCEFF:
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(talk->ccew.fieldWork), event, ENCEFFID_TR_NORMAL);
    (*seq)++;
    break;
  case SEQ_BATTLE_PROC:
    {
      GMEVENT *child_event;
      
      talk->ibp.gsys = gsys;
      talk->ibp.target_netid = talk->ccew.talk_netid;
      talk->ibp.flat_level = d_vic->battle_level;
      child_event = EVENT_FieldSubProc(
        gsys, talk->ccew.fieldWork, NO_OVERLAY_ID, &IntrudeBattleProcData, &talk->ibp);
      GMEVENT_CallEvent(event, child_event);
    }
	  *seq = SEQ_BATTLE_AFTER;
    break;
  case SEQ_BATTLE_AFTER:
    GMEVENT_CallEvent(event, EVENT_FSND_PopPlayBGM_fromBattle(gsys));
    (*seq)++;
    break;
  case SEQ_BATTLE_AFTER_NEXT:
    IntrudeEventPrint_SetupFieldMsg(&talk->ccew.iem, gsys);
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionBattleMsgID.target_battle_after[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
    (*seq) = SEQ_LAST_MSG_WAIT;
    break;

  case SEQ_BATTLE_NG:
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionBattleMsgID.target_battle_ng[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
    (*seq) = SEQ_LAST_MSG_WAIT;
    break;

	case SEQ_TALK_CANCEL:   //��b�L�����Z��
	  *seq = SEQ_END;
	  break;
    
  case SEQ_LAST_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
  	//����Finish����
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�����𒧂�  (�������~�b�V�������s�҂Ń^�[�Q�b�g����b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionBattle_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_BATTLE *talk;
  
  GMEVENT_Change(event, CommMissionBattle_MtoT_Talk, sizeof(EVENT_MISSION_BATTLE));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));

  talk->first_talked = TRUE;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�����𒧂�  (�������^�[�Q�b�g�Ń~�b�V�������s�҂���b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionBattle_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_BATTLE *talk;

  GMEVENT_Change(event, CommMissionBattle_TtoM_Talk, sizeof(EVENT_MISSION_BATTLE));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
  
  talk->first_talked = TRUE;
}

