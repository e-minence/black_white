//==============================================================================
/**
 * @file    event_mission_secretitem.c
 * @brief   ミッション：道具を隠せ
 * @author  matsuda
 * @date    2010.02.23(火)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"

#include "script.h"
#include "event_fieldtalk.h"

#include "print/wordset.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_comm_command.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "field\field_comm\intrude_mission_field.h"
#include "field\field_comm\intrude_work.h"
#include "field\field_comm\intrude_snd_def.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_monolith.h"
#include "msg/msg_mission_msg.h"
#include "field/field_comm/intrude_battle.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "item/itemsym.h"
#include "event_intrude.h"
#include "event_comm_common.h"
#include "event_mission_secretitem.h"
#include "event_mission_secretitem_ex.h"
#include "event_comm_result.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"
#include "field/fieldmap_call.h"  //FIELDMAP_IsReady



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_ITEM
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
}EVENT_MISSION_ITEM;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static GMEVENT_RESULT CommMissionItem_MtoT_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionItem_TtoM_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionItem_MtoT_Talked( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionItem_TtoM_Talked( GMEVENT *event, int *seq, void *wk );

static GMEVENT_RESULT Intrude_SecretItemArrivalManager( GMEVENT *event, int *seq, void *wk );
static GMEVENT* EVENT_Intrude_SecretItemArrival(GAMESYS_WORK *gsys, const MISSION_DATA *mdata );
static GMEVENT_RESULT Intrude_SecretItemArrivalEvent( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  データ
//==============================================================================
static const struct{
  u16 talk_m_to_t[TALK_TYPE_MAX];       ///<自分がミッション実行者でターゲットに話しかけた
  u16 talked_m_to_t[TALK_TYPE_MAX];     ///<自分がミッション実行者でターゲットから話しかけられた

  u16 talk_t_to_m[TALK_TYPE_MAX];       ///<自分がターゲットでミッション実行者に話しかけた
  u16 talked_t_to_m[TALK_TYPE_MAX];     ///<自分がターゲットでミッション実行者から話しかけられた
  
  u16 arrival[TALK_TYPE_MAX];           ///<自分がミッション実行者で特定の場所に到着
}MissionSecretItemMsgID = {
  { //自分がミッション実行者でターゲットに話しかけた
    mis_m05_01_m1,
    mis_m05_01_m2,
    mis_m05_01_m3,
    mis_m05_01_m4,
    mis_m05_01_m5,
  },
  { //自分がミッション実行者でターゲットから話しかけられた
    mis_m05_02_m1,
    mis_m05_02_m2,
    mis_m05_02_m3,
    mis_m05_02_m4,
    mis_m05_02_m5,
  },

  { //自分がターゲットでミッション実行者に話しかけた
    mis_m05_02_t1,
    mis_m05_02_t2,
    mis_m05_02_t3,
    mis_m05_02_t4,
    mis_m05_02_t5,
  },
  { //自分がターゲットでミッション実行者から話しかけられた
    mis_m05_01_t1,
    mis_m05_01_t2,
    mis_m05_01_t3,
    mis_m05_01_t4,
    mis_m05_01_t5,
  },

  { //自分がミッション実行者で特定の場所に到着
    mis_m05_03_m1,
    mis_m05_03_m2,
    mis_m05_03_m3,
    mis_m05_03_m4,
    mis_m05_03_m5,
  },
};



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * イベント切り替え：話し掛けろ  (自分がミッション実行者でターゲットに話しかけた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionItem_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_ITEM *talk;
  
  GMEVENT_Change(event, CommMissionItem_MtoT_Talk, sizeof(EVENT_MISSION_ITEM));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けろ (自分がミッション実行者でターゲットに話しかけた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionItem_MtoT_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_ITEM *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(0){    //intcomm不使用の為、エラーチェックの必要無し   intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_intrude_004);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, &talk->ccew.talk_myst );
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionSecretItemMsgID.talk_m_to_t[talk->ccew.disguise_talk_type]);
		(*seq)++;
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
 * イベント切り替え：話し掛けろ  (自分がターゲットでミッション実行者に話しかけた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionItem_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_ITEM *talk;

  GMEVENT_Change(event, CommMissionItem_TtoM_Talk, sizeof(EVENT_MISSION_ITEM));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けろ (自分がターゲットでミッション実行者に話しかけた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionItem_TtoM_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_ITEM *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(0){    //intcomm不使用の為、エラーチェックの必要無し     intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_intrude_004);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, &talk->ccew.talk_myst );
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionSecretItemMsgID.talk_t_to_m[talk->ccew.disguise_talk_type]);
		(*seq)++;
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
 * イベント切り替え：話し掛けろ (自分がミッション実行者でターゲットから話しかけられた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionItem_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_ITEM *talk;

  GMEVENT_Change(event, CommMissionItem_MtoT_Talked, sizeof(EVENT_MISSION_ITEM));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けろ (自分がミッション実行者でターゲットから話しかけられた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionItem_MtoT_Talked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_ITEM *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(0){    //intcomm不使用の為、エラーチェックの必要無し   intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_intrude_004);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, &talk->ccew.talk_myst );
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionSecretItemMsgID.talked_m_to_t[talk->ccew.disguise_talk_type]);
		(*seq)++;
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
  	
  	//ミッション失敗：パレスへ戻される
  	if(intcomm != NULL){
      PMSND_PlaySE( INTSE_WARP );
      GMEVENT_ChangeEvent(event, EVENT_IntrudeWarpPalace(gsys));
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
 * イベント切り替え：話し掛けろ (自分がターゲットでミッション実行者から話しかけられた)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionItem_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_ITEM *talk;

  GMEVENT_Change(event, CommMissionItem_TtoM_Talked, sizeof(EVENT_MISSION_ITEM));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * ミッション：話し掛けろ (自分がターゲットでミッション実行者から話しかけられた)
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionItem_TtoM_Talked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_ITEM *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(0){    //intcomm不使用の為、エラーチェックの必要無し   intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_intrude_004);
      *seq = SEQ_LAST_MSG_WAIT;
      talk->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      talk->ccew.iem.wordset, 0, &talk->ccew.talk_myst );
    IntrudeEventPrint_StartStream(&talk->ccew.iem, 
      MissionSecretItemMsgID.talked_t_to_m[talk->ccew.disguise_talk_type]);
		(*seq)++;
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
///目的場所に到達した時のイベント
typedef struct{
  INTRUDE_EVENT_MSGWORK iem;
  INTRUDE_SECRET_ITEM_SAVE itemdata;
  u16 target_netid;
  u16 padding;
  BOOL error;
  BOOL success;
  MISSION_ACHIEVE achieve_result;
}EVENT_ARRIVAL_WORK;

///目的場所に到達した時のイベント管理
typedef struct{
  MISSION_DATA mdata;
}EVENT_ARRIVAL_MANAGER;


//---------------------------------------------------------------------------------
/**
 * @brief ミッション：道具を隠せ：目的地に到着し、隠すイベント
 *
 * @param gsys
 * 
 * @return イベント
 */
//---------------------------------------------------------------------------------
GMEVENT* EVENT_Intrude_SecretItemArrivalEvent(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, const MISSION_DATA *mdata )
{
  GMEVENT* event;
  EVENT_ARRIVAL_MANAGER* manage;
  
  event = GMEVENT_Create(gsys, NULL, 
    Intrude_SecretItemArrivalManager, sizeof(EVENT_ARRIVAL_MANAGER));
  manage = GMEVENT_GetEventWork( event );
  
  manage->mdata = *mdata;

  return event;
}

//---------------------------------------------------------------------------------
/**
 * @brief ミッション：道具を隠せ：目的地に到着し、隠すイベント
 *
 * @param gsys
 * 
 * @return イベント
 */
//---------------------------------------------------------------------------------
static GMEVENT_RESULT Intrude_SecretItemArrivalManager( GMEVENT *event, int *seq, void *wk )
{
  EVENT_ARRIVAL_MANAGER* manage = wk;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  
  switch(*seq){
  case 0:
    MMDLSYS_PauseMoveProc( FIELDMAP_GetMMdlSys( fieldWork ) );

    GMEVENT_CallEvent(event, EVENT_Intrude_SecretItemArrival(gsys, &manage->mdata));
    (*seq)++;
    break;
  case 1:
    if(fieldWork != NULL && FIELDMAP_IsReady(fieldWork) == TRUE){
   	  MMDLSYS_ClearPauseMoveProc(FIELDMAP_GetMMdlSys(fieldWork));
      return GMEVENT_RES_FINISH;
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief ミッション：道具を隠せ：目的地に到着し、隠すイベント
 *
 * @param gsys
 * 
 * @return イベント
 */
//---------------------------------------------------------------------------------
static GMEVENT* EVENT_Intrude_SecretItemArrival(GAMESYS_WORK *gsys, const MISSION_DATA *mdata )
{
  GMEVENT* event;
  EVENT_ARRIVAL_WORK* work;
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData( gsys );
  const MISSION_TYPEDATA_ITEM *d_item = (void*)mdata->cdata.data;
  const MYSTATUS *myst = GAMEDATA_GetMyStatus(gamedata);
  
  event = GMEVENT_Create(gsys, NULL, Intrude_SecretItemArrivalEvent, sizeof(EVENT_ARRIVAL_WORK));
  work = GMEVENT_GetEventWork( event );
  
  MyStatus_CopyNameStrCode( myst, work->itemdata.name, PERSON_NAME_SIZE + EOM_SIZE );
  work->itemdata.item = d_item->item_no;
  work->itemdata.tbl_no = d_item->secret_pos_tblno;
  
  work->target_netid = mdata->target_info.net_id;


  IntrudeEventPrint_SetupFieldMsg(&work->iem, gsys);

  return event;
}

//--------------------------------------------------------------
/**
 * ミッション：目的地に到着し、隠すイベント
 * @param	event	GMEVENT
 * @param	seq		シーケンス
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT Intrude_SecretItemArrivalEvent( GMEVENT *event, int *seq, void *wk )
{
	EVENT_ARRIVAL_WORK *work = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
	enum{
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,
    SEQ_MSG_PRINT,
    SEQ_ITEMDATA_SEND,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };

  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&work->iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //メッセージ描画中は待つ
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&work->iem, msg_intrude_006);
      *seq = SEQ_LAST_MSG_WAIT;
      work->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }
	
	switch( *seq ){
	case SEQ_SEND_ACHIEVE:
    if(IntrudeSend_MissionAchieve(intcomm, &intcomm->mission) == TRUE){//ミッション達成を親に伝える
      (*seq)++;
    }
    break;
  case SEQ_RECV_WAIT:
    work->achieve_result = MISSION_GetAchieveAnswer(intcomm, &intcomm->mission);
    if(work->achieve_result != MISSION_ACHIEVE_NULL){
      (*seq)++;
    }
    break;
  case SEQ_MSG_PRINT:
    WORDSET_RegisterPlayerName( work->iem.wordset, 0, GAMEDATA_GetMyStatus(gamedata) );
    WORDSET_RegisterItemName( work->iem.wordset, 1, work->itemdata.item );
    {
      u32 msg_id;
      
      if(work->achieve_result == MISSION_ACHIEVE_OK){
        msg_id = MissionSecretItemMsgID.arrival[MISSION_FIELD_GetTalkType(intcomm, GAMEDATA_GetIntrudeMyID(gamedata))];
        work->success = TRUE;
      }
      else{
        msg_id = msg_intrude_005;
      }
      IntrudeEventPrint_StartStream(&work->iem, msg_id);
    }
    (*seq)++;
    break;
  case SEQ_ITEMDATA_SEND:
    if(work->success == TRUE){
      if(IntrudeSend_SecretItem(work->target_netid, &work->itemdata) == TRUE){
        OS_TPrintf("隠しアイテム送信成功 target_netid=%d\n", work->target_netid);
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;

  case SEQ_LAST_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&work->iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
    IntrudeEventPrint_ExitFieldMsg(&work->iem);

    GMEVENT_ChangeEvent(event, EVENT_CommMissionResult(gsys, work->success));
    return GMEVENT_RES_CONTINUE;  //ChangeEventで終了するためFINISHしない
  }
	return GMEVENT_RES_CONTINUE;
}
