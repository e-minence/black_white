//==============================================================================
/**
 * @file    event_mission_talk.c
 * @brief   �~�b�V�����F�b��������
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
#include "event_mission_talk.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_TALK
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
}EVENT_MISSION_TALK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommMissionTalk_MtoT_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionTalk_TtoM_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionTalk_MtoT_Talked( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionTalk_TtoM_Talked( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  �f�[�^
//==============================================================================
static const struct{
  u16 target_talk[TALK_TYPE_MAX];           ///<�������^�[�Q�b�g�Řb��������
  
  u16 target_talked[TALK_TYPE_MAX];         ///<�������^�[�Q�b�g�Řb��������ꂽ
  u16 target_talked_item[TALK_TYPE_MAX];    ///<�������^�[�Q�b�g�ŃA�C�e�����������

  u16 mission_talked[TALK_TYPE_MAX];        ///<�������~�b�V�������s�҂Řb��������ꂽ

  u16 mission_talk[TALK_TYPE_MAX];          ///<�������~�b�V�������s�҂Řb��������
  u16 mission_talk_item[TALK_TYPE_MAX];     ///<�������~�b�V�������s�҂ŃA�C�e����������
}MissionTalkMsgID = {
  { //�������^�[�Q�b�g�Řb��������
    mis_m02_02_t1,
    mis_m02_02_t2,
    mis_m02_02_t3,
    mis_m02_02_t4,
    mis_m02_02_t5,
  },
  { //�������^�[�Q�b�g�Řb��������ꂽ
    mis_m02_01_t1,
    mis_m02_01_t2,
    mis_m02_01_t3,
    mis_m02_01_t4,
    mis_m02_01_t5,
  },
  { //�������^�[�Q�b�g�ŃA�C�e�����������
    mis_m02_03_t1,
    mis_m02_03_t2,
    mis_m02_03_t3,
    mis_m02_03_t4,
    mis_m02_03_t5,
  },
  { //�������~�b�V�������s�҂Řb��������ꂽ
    mis_m02_02_m1,
    mis_m02_02_m2,
    mis_m02_02_m3,
    mis_m02_02_m4,
    mis_m02_02_m5,
  },
  { //�������~�b�V�������s�҂Řb��������
    mis_m02_01_m1,
    mis_m02_01_m2,
    mis_m02_01_m3,
    mis_m02_01_m4,
    mis_m02_01_m5,
  },
  { //�������~�b�V�������s�҂ŃA�C�e����������
    mis_m02_03_m1,
    mis_m02_03_m2,
    mis_m02_03_m3,
    mis_m02_03_m4,
    mis_m02_03_m5,
  },
};



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�b���|����  (�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalk_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALK *talk;
  
  GMEVENT_Change(event, CommMissionTalk_MtoT_Talk, sizeof(EVENT_MISSION_TALK));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�b���|���� (�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalk_MtoT_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
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
	case SEQ_SEND_ACHIEVE:
    if(IntrudeSend_MissionAchieve(intcomm, &intcomm->mission) == TRUE){//�~�b�V�����B����e�ɓ`����
      (*seq)++;
    }
    break;
  case SEQ_RECV_WAIT:
		if(MISSION_GetAchieveAnswer(&intcomm->mission) != MISSION_ACHIEVE_NULL){
      (*seq)++;
    }
    break;
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, Intrude_GetMyStatus(intcomm, talk->ccew.talk_netid) );
    {
      const MISSION_TYPEDATA_BASIC *d_basic = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_basic->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkMsgID.mission_talk[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, 
        MissionTalkMsgID.mission_talk_item[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
  		(*seq)++;
  	}
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
 * �C�x���g�؂�ւ��F�b���|����  (�������^�[�Q�b�g�Ń~�b�V�������s�҂ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalk_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALK *talk;

  GMEVENT_Change(event, CommMissionTalk_TtoM_Talk, sizeof(EVENT_MISSION_TALK));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�b���|���� (�������^�[�Q�b�g�Ń~�b�V�������s�҂ɘb��������)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalk_TtoM_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, Intrude_GetMyStatus(intcomm, talk->ccew.talk_netid) );
    {
      const MISSION_TYPEDATA_BASIC *d_basic = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_basic->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkMsgID.target_talk[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
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


//======================================================================
//	
//======================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�b���|���� (�������~�b�V�������s�҂Ń^�[�Q�b�g����b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalk_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALK *talk;

  GMEVENT_Change(event, CommMissionTalk_MtoT_Talked, sizeof(EVENT_MISSION_TALK));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�b���|���� (�������~�b�V�������s�҂Ń^�[�Q�b�g����b��������ꂽ)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalk_MtoT_Talked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, Intrude_GetMyStatus(intcomm, talk->ccew.talk_netid) );
    {
      const MISSION_TYPEDATA_BASIC *d_basic = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_basic->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkMsgID.mission_talked[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
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
  	
  	//�~�b�V�������s�F�p���X�֖߂����
  	if(intcomm != NULL){
      GMEVENT_ChangeEvent(event, EVENT_IntrudeWarpMyPalace(gsys));
      return GMEVENT_RES_CONTINUE;
    }
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�b���|���� (�������^�[�Q�b�g�Ń~�b�V�������s�҂���b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalk_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALK *talk;

  GMEVENT_Change(event, CommMissionTalk_TtoM_Talked, sizeof(EVENT_MISSION_TALK));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�b���|���� (�������^�[�Q�b�g�Ń~�b�V�������s�҂���b��������ꂽ)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalk_TtoM_Talked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
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
      const MISSION_TYPEDATA_BASIC *d_basic = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_basic->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkMsgID.target_talked[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, 
        MissionTalkMsgID.target_talked_item[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
  		(*seq)++;
  	}
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

