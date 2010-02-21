//==============================================================================
/**
 * @file    event_mission_talked.c
 * @brief   ミッション：話しかけられろ
 * @author  matsuda
 * @date    2010.02.19(金)
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
#include "event_mission_talked.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_TALKED
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
}EVENT_MISSION_TALKED;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static GMEVENT_RESULT CommMissionTalked_MtoT_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionTalked_TtoM_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionTalked_MtoT_Talked( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionTalked_TtoM_Talked( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  データ
//==============================================================================
static const struct{
  u16 target_talk[TALK_TYPE_MAX];           ///<自分がターゲットで話しかけた
  u16 target_talk_item[TALK_TYPE_MAX];      ///<自分がターゲットでアイテムをもらった
  
  u16 target_talked[TALK_TYPE_MAX];         ///<自分がターゲットで話しかけられた

  u16 mission_talked[TALK_TYPE_MAX];        ///<自分がミッション実行者で話しかけられた
  u16 mission_talked_item[TALK_TYPE_MAX];   ///<自分がミッション実行者でアイテムをあげた
  
  u16 mission_talk[TALK_TYPE_MAX];          ///<自分がミッション実行者で話しかけた
}MissionTalkedMsgID = {
  { //自分がターゲットで話しかけた
    mis_m01_02_t1,
    mis_m01_02_t2,
    mis_m01_02_t3,
    mis_m01_02_t4,
    mis_m01_02_t5,
  },
  { //自分がターゲットでアイテムをもらった
    mis_m01_03_t1,
    mis_m01_03_t2,
    mis_m01_03_t3,
    mis_m01_03_t4,
    mis_m01_03_t5,
  },
  { //自分がターゲットで話しかけられた
    mis_m01_01_t1,
    mis_m01_01_t2,
    mis_m01_01_t3,
    mis_m01_01_t4,
    mis_m01_01_t5,
  },
  { //自分がミッション実行者で話しかけられた
    mis_m01_01_m1,
    mis_m01_01_m2,
    mis_m01_01_m3,
    mis_m01_01_m4,
    mis_m01_01_m5,
  },
  { //自分がミッション実行者でアイテムをあげた
    mis_m01_03_m1,
    mis_m01_03_m2,
    mis_m01_03_m3,
    mis_m01_03_m4,
    mis_m01_03_m5,
  },
  { //自分がミッション実行者で話しかけた
    mis_m01_02_m1,
    mis_m01_02_m2,
    mis_m01_02_m3,
    mis_m01_02_m4,
    mis_m01_02_m5,
  },
};



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * イベント切り替え：話し掛けられろ  (自分がミッション実行者でターゲットに話しかけた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalked_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALKED *talk;

  GMEVENT_Change(event, CommMissionTalked_MtoT_Talk, sizeof(EVENT_MISSION_TALKED));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けられろ (自分がミッション実行者でターゲットに話しかけた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalked_MtoT_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALKED *talk = wk;
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
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_MSG_END_BUTTON_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, Intrude_GetMyStatus(intcomm, talk->ccew.talk_netid) );
    {
      const MISSION_TYPEDATA_SKILL *d_skill = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_skill->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkedMsgID.mission_talk[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
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
  	//共通Finish処理
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
 * イベント切り替え：話し掛けられろ  (自分がターゲットでミッション実行者に話しかけた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalked_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALKED *talk;

  GMEVENT_Change(event, CommMissionTalked_TtoM_Talk, sizeof(EVENT_MISSION_TALKED));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けられろ (自分がターゲットでミッション実行者に話しかけた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalked_TtoM_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALKED *talk = wk;
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
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
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
      const MISSION_TYPEDATA_SKILL *d_skill = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_skill->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkedMsgID.target_talk[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, 
        MissionTalkedMsgID.target_talk_item[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
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
  	//共通Finish処理
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
 * イベント切り替え：話し掛けられろ (自分がミッション実行者でターゲットから話しかけられた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalked_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALKED *talk;

  GMEVENT_Change(event, CommMissionTalked_MtoT_Talked, sizeof(EVENT_MISSION_TALKED));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けられろ (自分がミッション実行者でターゲットから話しかけられた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalked_MtoT_Talked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALKED *talk = wk;
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
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
	case SEQ_SEND_ACHIEVE:
    if(IntrudeSend_MissionAchieve(intcomm, &intcomm->mission) == TRUE){//ミッション達成を親に伝える
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
      const MISSION_TYPEDATA_SKILL *d_skill = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_skill->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkedMsgID.mission_talked[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, 
        MissionTalkedMsgID.mission_talked_item[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
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
  	//共通Finish処理
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
 * イベント切り替え：話し掛けられろ (自分がターゲットでミッション実行者から話しかけられた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionTalked_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_TALKED *talk;

  GMEVENT_Change(event, CommMissionTalked_TtoM_Talked, sizeof(EVENT_MISSION_TALKED));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けられろ (自分がターゲットでミッション実行者から話しかけられた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionTalked_TtoM_Talked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_TALKED *talk = wk;
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
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
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
      const MISSION_TYPEDATA_SKILL *d_skill = (void*)talk->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( talk->ccew.iem.wordset, 1, d_skill->item );
    }
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionTalkedMsgID.target_talked[MISSION_FIELD_GetTalkType(intcomm, talk->ccew.talk_netid)]);
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
  	//共通Finish処理
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}
