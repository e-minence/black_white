//==============================================================================
/**
 * @file    event_mission_shop.c
 * @brief   �~�b�V�����F����𔄂�I
 * @author  matsuda
 * @date    2010.03.18(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"
#include "system/main.h"

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
#include "msg/msg_invasion.h"
#include "msg/msg_mission_monolith.h"
#include "msg/msg_mission_msg.h"
#include "field/field_comm/intrude_battle.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "item/itemsym.h"
#include "event_intrude.h"
#include "event_comm_common.h"
#include "event_mission_shop.h"
#include "event_comm_result.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_SHOP
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
	BOOL success;
	BOOL cancel;
}EVENT_MISSION_SHOP;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommMissionShop_MtoT_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionShop_TtoM_Talk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionShop_MtoT_Talked( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionShop_TtoM_Talked( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  �f�[�^
//==============================================================================
static const struct{
  u16 target_first[TALK_TYPE_MAX];          ///<�������^�[�Q�b�g�ōŏ��̉�b
  u16 target_second[TALK_TYPE_MAX];         ///<�������^�[�Q�b�g�ōŏ��̉�b���̂Q
  u16 target_decline[TALK_TYPE_MAX];        ///<�������^�[�Q�b�g�Ŕ�����f����
  u16 target_short_of_money[TALK_TYPE_MAX]; ///<�������^�[�Q�b�g�ł���������Ȃ�
  u16 target_item_full[TALK_TYPE_MAX];      ///<�������^�[�Q�b�g�œ�������ς�
  u16 target_concluded[TALK_TYPE_MAX];      ///<�������^�[�Q�b�g�Ŕ�������

  u16 mission_first[TALK_TYPE_MAX];          ///<�������~�b�V�������s�҂ōŏ��̉�b
  u16 mission_decline[TALK_TYPE_MAX];        ///<�������~�b�V�������s�҂Ŕ�����f��ꂽ
  u16 mission_short_of_money[TALK_TYPE_MAX]; ///<�������~�b�V�������s�҂ő��肪����������Ȃ�
  u16 mission_item_full[TALK_TYPE_MAX];      ///<�������~�b�V�������s�҂ő��肪��������ς�
  u16 mission_concluded[TALK_TYPE_MAX];      ///<�������~�b�V�������s�҂Ŕ�������
}MissionShopMsgID = {
  { //�������^�[�Q�b�g�ōŏ��̉�b
    mis_m06_01_t1,
    mis_m06_01_t2,
    mis_m06_01_t3,
    mis_m06_01_t4,
    mis_m06_01_t5,
  },
  { //�������^�[�Q�b�g�ōŏ��̉�b�@���̂Q
    mis_m06_01_t1,
    mis_m06_01_t2,
    mis_m06_01_t3,
    mis_m06_01_t4_2,
    mis_m06_01_t5_2,
  },
  { //�������^�[�Q�b�g�Ŕ�����f����
    mis_m06_03_t1,
    mis_m06_03_t2,
    mis_m06_03_t3,
    mis_m06_03_t4,
    mis_m06_03_t5,
  },
  { //�������^�[�Q�b�g�ł���������Ȃ�
    mis_m06_04_t1,
    mis_m06_04_t2,
    mis_m06_04_t3,
    mis_m06_04_t4,
    mis_m06_04_t5,
  },
  { //�������^�[�Q�b�g�œ�������ς�
    mis_m06_05_t1,
    mis_m06_05_t2,
    mis_m06_05_t3,
    mis_m06_05_t4,
    mis_m06_05_t5,
  },
  { //�������^�[�Q�b�g�Ŕ�������
    mis_m06_02_t1,
    mis_m06_02_t2,
    mis_m06_02_t3,
    mis_m06_02_t4,
    mis_m06_02_t5,
  },

  { //�������~�b�V�������s�҂ōŏ��̉�b
    mis_m06_01_m1,
    mis_m06_01_m2,
    mis_m06_01_m3,
    mis_m06_01_m4,
    mis_m06_01_m5,
  },
  { //�������~�b�V�������s�҂Ŕ�����f��ꂽ
    mis_m06_03_m1,
    mis_m06_03_m2,
    mis_m06_03_m3,
    mis_m06_03_m4,
    mis_m06_03_m5,
  },
  { //�������~�b�V�������s�҂ő��肪����������Ȃ�
    mis_m06_04_m1,
    mis_m06_04_m2,
    mis_m06_04_m3,
    mis_m06_04_m4,
    mis_m06_04_m5,
  },
  { //�������~�b�V�������s�҂ő��肪��������ς�
    mis_m06_05_m1,
    mis_m06_05_m2,
    mis_m06_05_m3,
    mis_m06_05_m4,
    mis_m06_05_m5,
  },
  { //�������~�b�V�������s�҂Ŕ�������
    mis_m06_02_m1,
    mis_m06_02_m2,
    mis_m06_02_m3,
    mis_m06_02_m4,
    mis_m06_02_m5,
  },
};



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F����𔄂�  (�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionShop_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_SHOP *shop;
  
  GMEVENT_Change(event, CommMissionShop_MtoT_Talk, sizeof(EVENT_MISSION_SHOP));
	shop = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &shop->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
  {
    MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)shop->ccew.mdata.cdata.data;
    if(Intrude_CheckNG_Item(d_attr->item_no) == FALSE){
      d_attr->item_no = ITEM_KIZUGUSURI;
    }
  }
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F����𔄂� (�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event shop
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionShop_MtoT_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_SHOP *shop = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_SHOP_OK,
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,    //�ʐM�͂����܂ŁB�������牺�͒ʐM�s�v
    SEQ_SHOP_NG,
    SEQ_NG_SHORT_OF_MONEY,
    SEQ_NG_ITEM_FULL,
    SEQ_TALK_CANCEL,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL && (*seq) <= SEQ_RECV_WAIT){
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&shop->ccew.iem, msg_intrude_004);
      *seq = SEQ_LAST_MSG_WAIT;
      shop->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      shop->ccew.iem.wordset, 0, &shop->ccew.talk_myst );
    {
      const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)shop->ccew.mdata.cdata.data;
      WORDSET_RegisterItemName( shop->ccew.iem.wordset, 1, d_attr->item_no );
      WORDSET_RegisterNumber( shop->ccew.iem.wordset, 2, d_attr->price, 5, 
        STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    }
    WORDSET_RegisterPlayerName( 
      shop->ccew.iem.wordset, 3, GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(gsys) ) );
    IntrudeEventPrint_StartStream(&shop->ccew.iem, 
      MissionShopMsgID.mission_first[shop->ccew.disguise_talk_type]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == TRUE){
      INTRUDE_TALK_STATUS talk_status = Intrude_GetTalkAnswer(intcomm);
      OS_TPrintf("menu_answer status = %d\n", talk_status);
      switch(talk_status){
      case INTRUDE_TALK_STATUS_SHOP_OK:
        *seq = SEQ_SHOP_OK;
        break;
      case INTRUDE_TALK_STATUS_SHOP_NG:
      case INTRUDE_TALK_STATUS_CANCEL:
        *seq = SEQ_SHOP_NG;
        break;
      case INTRUDE_TALK_STATUS_SHOP_SHOT_OF_MONEY:
        *seq = SEQ_NG_SHORT_OF_MONEY;
        break;
      case INTRUDE_TALK_STATUS_SHOP_ITEM_FULL:
        *seq = SEQ_NG_ITEM_FULL;
        break;
      default:  //�܂��Ԏ������Ă��Ȃ�
        if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
          if(IntrudeSend_TalkCancel(intcomm->talk.talk_netid, intcomm->talk.talk_rand) == TRUE){
            Intrude_TalkRandClose(intcomm);
            shop->cancel = TRUE;
            *seq = SEQ_TALK_CANCEL;
          }
        }
        break;
      }
    }
    break;

  case SEQ_SHOP_OK:
    IntrudeEventPrint_StartStream(&shop->ccew.iem, 
      MissionShopMsgID.mission_concluded[shop->ccew.disguise_talk_type]);
		(*seq)++;
		break;
	case SEQ_SEND_ACHIEVE:
    if(IntrudeSend_MissionAchieve(intcomm, &intcomm->mission) == TRUE){//�~�b�V�����B����e�ɓ`����
      (*seq)++;
    }
    break;
  case SEQ_RECV_WAIT:
		if(MISSION_GetAchieveAnswer(intcomm, &intcomm->mission) != MISSION_ACHIEVE_NULL){
      shop->success = TRUE;
      (*seq) = SEQ_LAST_MSG_WAIT;
    }
    break;
  
  case SEQ_SHOP_NG:
    IntrudeEventPrint_StartStream(&shop->ccew.iem, 
      MissionShopMsgID.mission_decline[shop->ccew.disguise_talk_type]);
    (*seq) = SEQ_LAST_MSG_WAIT;
		break;
  case SEQ_NG_SHORT_OF_MONEY:
    IntrudeEventPrint_StartStream(&shop->ccew.iem, 
      MissionShopMsgID.mission_short_of_money[shop->ccew.disguise_talk_type]);
    (*seq) = SEQ_LAST_MSG_WAIT;
		break;
  case SEQ_NG_ITEM_FULL:
    IntrudeEventPrint_StartStream(&shop->ccew.iem, 
      MissionShopMsgID.mission_item_full[shop->ccew.disguise_talk_type]);
    (*seq) = SEQ_LAST_MSG_WAIT;
		break;
  case SEQ_TALK_CANCEL:
    (*seq) = SEQ_END;
		break;
  
  case SEQ_LAST_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == TRUE){
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
  	EVENT_CommCommon_Finish(intcomm, &shop->ccew);

    if(shop->success == TRUE){
      GMEVENT_ChangeEvent(event, EVENT_CommMissionResult(gsys, shop->success));
      return GMEVENT_RES_CONTINUE;  //ChangeEvent�ŏI�����邽��FINISH���Ȃ�
    }
    else if(shop->cancel == FALSE){
      GMEVENT_ChangeEvent(
        event, EVENT_IntrudeForceWarpMyPalace(gsys, MISSION_FORCEWARP_MSGID_SHOP_NG));
      return GMEVENT_RES_CONTINUE;  //ChangeEvent�ŏI�����邽��FINISH���Ȃ�
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
 * �C�x���g�؂�ւ��F����𔄂�  (�������^�[�Q�b�g�Ń~�b�V�������s�҂ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionShop_TtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_SHOP *shop;

  GMEVENT_Change(event, CommMissionShop_TtoM_Talk, sizeof(EVENT_MISSION_SHOP));
	shop = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &shop->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F����𔄂� (�������^�[�Q�b�g�Ń~�b�V�������s�҂ɘb��������)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event shop
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionShop_TtoM_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_SHOP *shop = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)shop->ccew.mdata.cdata.data;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_SECOND,
    SEQ_BATTLE_YESNO_SELECT,
    SEQ_CONCLUDED,
    SEQ_NG_SHORT_OF_MONEY,
    SEQ_NG_ITEM_FULL,
    SEQ_NG,
    SEQ_LAST_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_LAST_MSG_WAIT){
      IntrudeEventPrint_StartStream(&shop->ccew.iem, msg_intrude_004);
      *seq = SEQ_LAST_MSG_WAIT;
      shop->error = TRUE;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    WORDSET_RegisterPlayerName( 
      shop->ccew.iem.wordset, 0, &shop->ccew.talk_myst );
    WORDSET_RegisterItemName( shop->ccew.iem.wordset, 1, d_attr->item_no );
    WORDSET_RegisterNumber( shop->ccew.iem.wordset, 2, d_attr->price, 5, 
      STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterPlayerName( 
      shop->ccew.iem.wordset, 3, GAMEDATA_GetMyStatus( GAMESYSTEM_GetGameData(gsys) ) );
    IntrudeEventPrint_StartStream(&shop->ccew.iem, 
      MissionShopMsgID.target_first[shop->ccew.disguise_talk_type]);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == TRUE){
      IntrudeEventPrint_StartStream(&shop->ccew.iem, 
        MissionShopMsgID.target_second[shop->ccew.disguise_talk_type]);
      (*seq)++;
    }
    break;
  case SEQ_MSG_SECOND:
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == TRUE){
      IntrudeEventPrint_SetupYesNo(&shop->ccew.iem, gsys);
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_YESNO_SELECT:
    {
      FLDMENUFUNC_YESNO yesno = IntrudeEventPrint_SelectYesNo(&shop->ccew.iem);
      BOOL ng_end = FALSE;
      
      switch(yesno){
      case FLDMENUFUNC_YESNO_YES:
        IntrudeEventPrint_ExitMenu(&shop->ccew.iem);

        //�����͑���Ă��邩
        if(d_attr->price > MISC_GetGold( GAMEDATA_GetMiscWork(gamedata) )){
          ng_end = TRUE;
          *seq = SEQ_NG_SHORT_OF_MONEY;
          break;
        }
        //����ɋ󂫂͂��邩
        if(MYITEM_AddCheck( 
            GAMEDATA_GetMyItem(gamedata), d_attr->item_no, 1, HEAPID_FIELDMAP ) == FALSE){
          ng_end = TRUE;
          *seq = SEQ_NG_ITEM_FULL;
          break;
        }
        *seq = SEQ_CONCLUDED;
        break;
      case FLDMENUFUNC_YESNO_NO:
        IntrudeEventPrint_ExitMenu(&shop->ccew.iem);
        ng_end = TRUE;
        *seq = SEQ_NG;
        break;
      }
      
      if(ng_end == TRUE){
        //���s�Ȃ̂Ń~�b�V��������蒼��
        MISSION_LIST_Create_Type(GAMEDATA_GetMyOccupyInfo(gamedata), MISSION_TYPE_ATTRIBUTE);
      }
    }
    break;
    
  case SEQ_CONCLUDED:
    if(IntrudeSend_TalkAnswer(intcomm, intcomm->talk.talk_netid, 
        INTRUDE_TALK_STATUS_SHOP_OK, intcomm->talk.talk_rand) == TRUE){
      MISC_SubGold( GAMEDATA_GetMiscWork(gamedata), d_attr->price );
      MYITEM_AddItem(GAMEDATA_GetMyItem(gamedata), d_attr->item_no, 1, HEAPID_FIELDMAP);
      
      IntrudeEventPrint_StartStream(&shop->ccew.iem, 
        MissionShopMsgID.target_concluded[shop->ccew.disguise_talk_type]);
      (*seq) = SEQ_LAST_MSG_WAIT;
    }
    break;

  case SEQ_NG_SHORT_OF_MONEY:
    if(IntrudeSend_TalkAnswer(intcomm, intcomm->talk.talk_netid, 
        INTRUDE_TALK_STATUS_SHOP_SHOT_OF_MONEY, intcomm->talk.talk_rand) == TRUE){
      IntrudeEventPrint_StartStream(&shop->ccew.iem, 
        MissionShopMsgID.target_short_of_money[shop->ccew.disguise_talk_type]);
      (*seq) = SEQ_LAST_MSG_WAIT;
    }
    break;

  case SEQ_NG_ITEM_FULL:
    if(IntrudeSend_TalkAnswer(intcomm, intcomm->talk.talk_netid, 
        INTRUDE_TALK_STATUS_SHOP_ITEM_FULL, intcomm->talk.talk_rand) == TRUE){
      IntrudeEventPrint_StartStream(&shop->ccew.iem, 
        MissionShopMsgID.target_item_full[shop->ccew.disguise_talk_type]);
      (*seq) = SEQ_LAST_MSG_WAIT;
    }
    break;

  case SEQ_NG:
    if(IntrudeSend_TalkAnswer(intcomm, intcomm->talk.talk_netid, 
        INTRUDE_TALK_STATUS_SHOP_NG, intcomm->talk.talk_rand) == TRUE){
      IntrudeEventPrint_StartStream(&shop->ccew.iem, 
        MissionShopMsgID.target_decline[shop->ccew.disguise_talk_type]);
      (*seq) = SEQ_LAST_MSG_WAIT;
    }
    break;

  case SEQ_LAST_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&shop->ccew.iem) == TRUE){
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
  	EVENT_CommCommon_Finish(intcomm, &shop->ccew);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}


//======================================================================
//	
//======================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F����𔄂� (�������~�b�V�������s�҂Ń^�[�Q�b�g����b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionShop_MtoT_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENTCHANGE_CommMissionShop_MtoT_Talk(event, ccew); //�C�x���g�͑S���ꏏ
}




//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F����𔄂� (�������^�[�Q�b�g�Ń~�b�V�������s�҂���b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionShop_TtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENTCHANGE_CommMissionShop_TtoM_Talk(event, ccew); //�C�x���g�͑S���ꏏ
}

