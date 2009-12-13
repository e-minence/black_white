//==============================================================================
/**
 * @file    event_comm_talk.c
 * @brief   �ʐM�v���C���[�ւ̘b�������C�x���g
 * @author  matsuda
 * @date    2009.10.11(��)
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
#include "msg/msg_invasion.h"
#include "msg/msg_mission_monolith.h"
#include "field/field_comm/intrude_battle.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "item/itemsym.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"


//======================================================================
//	define
//======================================================================
typedef enum{
  FIRST_TALK_RET_NULL,      ///<�V�[�P���X���s��
  FIRST_TALK_RET_OK,        ///<OK�ŏI��
  FIRST_TALK_RET_NG,        ///<NG�ŏI��
  FIRST_TALK_RET_CANCEL,    ///<�L�����Z������
}FIRST_TALK_RET;

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	COMMTALK_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	HEAPID heapID;
	u32 scr_id;
	MMDL *fmmdl_player;
	MMDL *fmmdl_talk;
	FIELDMAP_WORK *fieldWork;
  INTRUDE_COMM_SYS_PTR intcomm;
  
	INTRUDE_EVENT_MSGWORK iem;
	BOOL error;
	
	INTRUDE_TALK_STATUS answer_status;
	
	INTRUDE_BATTLE_PARENT ibp;
	u32 talk_netid;
	
	u8 first_talk_seq;
	u8 padding[3];
}COMMTALK_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommTalkEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommBingoEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionAchieveEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionItemEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionBasicEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommMissionResultEvent( GMEVENT *event, int *seq, void *wk );

//==============================================================================
//  �f�[�^
//==============================================================================
///�~�b�V��������n���̃��b�Z�[�WID ��TALK_TYPE��
const u16 MissionItemMsgID[] = {
  msg_talk_item_m_000,          //TALK_TYPE_NORMAL
  msg_talk_item_m_000,          //TALK_TYPE_MAN
  msg_talk_item_w_000,          //TALK_TYPE_WOMAN
  msg_talk_item_pika_000,       //TALK_TYPE_PIKA
};

///�~�b�V����Basic�̃��b�Z�[�WID ��TALK_TYPE��
const u16 MissionBasicMsgID[] = {
  msg_talk_life_m_000,          //TALK_TYPE_NORMAL
  msg_talk_life_m_000,          //TALK_TYPE_MAN
  msg_talk_life_w_000,          //TALK_TYPE_WOMAN
  msg_talk_life_pika_000,       //TALK_TYPE_PIKA
};


//======================================================================
//	�C�x���g�F�t�B�[���h�b���|���C�x���g
//======================================================================
//==================================================================
/**
 * �ʐM�v���C���[�b�������C�x���g�N��
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   intcomm		      �N���V�X�e�����[�N�ւ̃|�C���^
 * @param   fmmdl_player		���@���샂�f��
 * @param   talk_net_id		  �b�������Ώۂ�NetID
 * @param   heap_id		      �q�[�vID
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommTalk(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id)
{
	COMMTALK_EVENT_WORK *ftalk_wk;
	GMEVENT *event = NULL;
	
	if(MISSION_CheckMissionTargetNetID(&intcomm->mission, talk_net_id) == TRUE){
    MISSION_TYPE mission_type = MISSION_GetMissionType(&intcomm->mission);
	  switch(mission_type){
	  case MISSION_TYPE_ITEM:
    default:
    	event = GMEVENT_Create(
    		gsys, NULL,	CommMissionItemEvent, sizeof(COMMTALK_EVENT_WORK) );
      break;
    case MISSION_TYPE_BASIC:
      if(intcomm->intrude_status[talk_net_id].action_status == INTRUDE_ACTION_BATTLE){
      	event = GMEVENT_Create(
      		gsys, NULL,	CommMissionBasicEvent, sizeof(COMMTALK_EVENT_WORK) );
      }
      break;
    }
  }
	
	if(event == NULL){
  	if(intcomm->intrude_status[talk_net_id].action_status == INTRUDE_ACTION_BINGO_BATTLE){
    	event = GMEVENT_Create(
    		gsys, NULL,	CommBingoEvent, sizeof(COMMTALK_EVENT_WORK) );
      Bingo_Clear_BingoBattleBeforeBuffer(Bingo_GetBingoSystemWork(intcomm));
    }
    else{
    	event = GMEVENT_Create(
    		gsys, NULL,	CommTalkEvent, sizeof(COMMTALK_EVENT_WORK) );
    }
  }
  
	ftalk_wk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( ftalk_wk, sizeof(COMMTALK_EVENT_WORK) );
	
	ftalk_wk->heapID = heap_id;
	ftalk_wk->fmmdl_player = fmmdl_player;
	ftalk_wk->fmmdl_talk = CommPlayer_GetMmdl(intcomm->cps, talk_net_id);
	ftalk_wk->fieldWork = fieldWork;
	ftalk_wk->intcomm = intcomm;
	ftalk_wk->talk_netid = talk_net_id;
	
  IntrudeEventPrint_SetupFieldMsg(&ftalk_wk->iem, gsys);

	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_TALK);
  Intrude_InitTalkWork(intcomm, talk_net_id);

	return( event );
}

//--------------------------------------------------------------
/**
 * ��b���n�߂鎞�̍ŏ��̑���̃��A�N�V�����҂��Ȃǂ̏������ꊇ���čs��
 *
 * @param   iem		
 * @param   seq		
 *
 * @retval  FIRST_TALK_RET		
 */
//--------------------------------------------------------------
static FIRST_TALK_RET _FirstTalkSeq(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_EVENT_MSGWORK *iem, u8 *seq)
{
  enum{
    SEQ_FIRST_TALK,
    SEQ_TALK_SEND,
    SEQ_TALK_WAIT,
    SEQ_TALK_RECV_WAIT,
    SEQ_TALK_NG,
    SEQ_TALK_NG_MSGWAIT,
    SEQ_TALK_NG_LAST,
    SEQ_END_OK,
    SEQ_END_NG,
    SEQ_END_CANCEL,
  };
  
  switch(*seq){
	case SEQ_FIRST_TALK:
    IntrudeEventPrint_StartStream(iem, msg_intrude_000);
		(*seq)++;
		break;
	case SEQ_TALK_SEND:
	  if(IntrudeSend_Talk(intcomm, intcomm->talk.talk_netid) == TRUE){
      (*seq)++;
    }
    break;
	case SEQ_TALK_WAIT:
    if(IntrudeEventPrint_WaitStream(iem) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_TALK_RECV_WAIT:
	  {
      INTRUDE_TALK_STATUS talk_status = Intrude_GetTalkAnswer(intcomm);
      switch(talk_status){
      case INTRUDE_TALK_STATUS_OK:
        *seq = SEQ_END_OK;
        break;
      case INTRUDE_TALK_STATUS_NG:
      case INTRUDE_TALK_STATUS_CANCEL:
        *seq = SEQ_TALK_NG;
        break;
      default:  //�܂��Ԏ������Ă��Ȃ�
        if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
          if(IntrudeSend_TalkCancel(intcomm->talk.talk_netid) == TRUE){
            *seq = SEQ_END_CANCEL;
          }
        }
        break;
      }
    }
		break;

  case SEQ_TALK_NG:   //�f��ꂽ
    IntrudeEventPrint_StartStream(iem, msg_intrude_002);
		(*seq)++;
    break;
  case SEQ_TALK_NG_MSGWAIT:
    if(IntrudeEventPrint_WaitStream(iem) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_TALK_NG_LAST:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
			(*seq) = SEQ_END_NG;
		}
	  break;
	
	case SEQ_END_OK:
	  return FIRST_TALK_RET_OK;
	case SEQ_END_NG:
	  return FIRST_TALK_RET_NG;
	case SEQ_END_CANCEL:
	  return FIRST_TALK_RET_CANCEL;
  default:
    GF_ASSERT(0);
    break;
  }
  
  return FIRST_TALK_RET_NULL;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�b���|���C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommTalkEvent( GMEVENT *event, int *seq, void *wk )
{
	COMMTALK_EVENT_WORK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  enum{
    SEQ_FIRST_TALK,
    SEQ_TALK_OK,
    SEQ_TALK_OK_MSGWAIT,
    SEQ_MENU_INIT,
    SEQ_MENU_WAIT,
    SEQ_MENU_SELECT_SEND,
    SEQ_MENU_SELECT_ANSWER_WAIT,
    SEQ_TALK_NG,
    SEQ_TALK_NG_MSGWAIT,
    SEQ_TALK_NG_LAST,
    SEQ_BATTLE_START,
    SEQ_ITEM_INIT,
    SEQ_ITEM_MSG_WAIT,
    SEQ_ITEM_LAST_WAIT,
    SEQ_TALK_CANCEL,
    SEQ_FINISH,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
  #if 0 //��check�@��ō쐬
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->iem, msg_talk_cancel);
      *seq = SEQ_MSG_WAIT;
      talk->error = TRUE;
    }
  #endif
  }

	switch( *seq ){
	case SEQ_FIRST_TALK:
	  {
      FIRST_TALK_RET first_ret = _FirstTalkSeq(intcomm, &talk->iem, &talk->first_talk_seq);
      switch(first_ret){
      case FIRST_TALK_RET_OK:
        *seq = SEQ_TALK_OK;
        break;
      case FIRST_TALK_RET_NG:
        *seq = SEQ_FINISH;
        break;
      case FIRST_TALK_RET_CANCEL:
        *seq = SEQ_TALK_CANCEL;
        break;
      }
    }
    break;

  case SEQ_TALK_OK:   //�Ԏ�OK
    IntrudeEventPrint_StartStream(&talk->iem, msg_intrude_003);
		(*seq)++;
    break;
  case SEQ_TALK_OK_MSGWAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_MENU_INIT:
	  IntrudeEventPrint_SetupMainMenu(&talk->iem, gsys);
	  (*seq)++;
	  break;
	case SEQ_MENU_WAIT:
	  {
      u32 menu_ret = IntrudeEventPrint_SelectMenu(&talk->iem);
      switch(menu_ret){
      case FLDMENUFUNC_NULL:
        break;
      case FLDMENUFUNC_CANCEL:
      case INTRUDE_TALK_STATUS_CANCEL:
        talk->answer_status = INTRUDE_TALK_STATUS_CANCEL;
        IntrudeEventPrint_ExitMenu(&talk->iem);
        (*seq)++;
        break;
      default:
        talk->answer_status = menu_ret;
        IntrudeEventPrint_ExitMenu(&talk->iem);
        IntrudeEventPrint_StartStream(&talk->iem, msg_talk_battle_002);
        (*seq)++;
        break;
      }
    }
    break;
  case SEQ_MENU_SELECT_SEND:  //���j���[�I�����ʂ𑗐M
    if(IntrudeSend_TalkAnswer(
        talk->intcomm, talk->intcomm->talk.talk_netid, talk->answer_status) == TRUE){
      if(talk->answer_status == INTRUDE_TALK_STATUS_CANCEL){
        *seq = SEQ_TALK_CANCEL;
      }
      else{
        (*seq)++;
      }
    }
	  break;
	case SEQ_MENU_SELECT_ANSWER_WAIT: //���j���[�I�����ʂ̕Ԏ��҂�
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      INTRUDE_TALK_STATUS talk_status = Intrude_GetTalkAnswer(talk->intcomm);
      OS_TPrintf("menu_answer status = %d\n", talk_status);
      switch(talk_status){
      case INTRUDE_TALK_STATUS_OK:
        switch(talk->answer_status){
        case INTRUDE_TALK_STATUS_BATTLE:
          *seq = SEQ_BATTLE_START;
          break;
        case INTRUDE_TALK_STATUS_ITEM:
          *seq = SEQ_ITEM_INIT;
          break;
        default:
          GF_ASSERT(0);
          break;
        }
        break;
      case INTRUDE_TALK_STATUS_NG:
      case INTRUDE_TALK_STATUS_CANCEL:
        *seq = SEQ_TALK_NG;
        break;
      default:  //�܂��Ԏ������Ă��Ȃ�
        if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
          if(IntrudeSend_TalkCancel(talk->intcomm->talk.talk_netid) == TRUE){
            *seq = SEQ_TALK_CANCEL;
          }
        }
        break;
      }
    }
		break;
	  
  case SEQ_TALK_NG:   //�f��ꂽ
    IntrudeEventPrint_StartStream(&talk->iem, msg_intrude_002);
		(*seq)++;
    break;
  case SEQ_TALK_NG_MSGWAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
			(*seq)++;
		}
		break;
	case SEQ_TALK_NG_LAST:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
			(*seq) = SEQ_FINISH;
		}
	  break;
	
	case SEQ_BATTLE_START:
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);
    {
      GMEVENT *child_event;
      
      talk->ibp.gsys = gsys;
      talk->ibp.target_netid = talk->talk_netid;
      child_event = EVENT_FieldSubProc(
        gsys, talk->fieldWork, NO_OVERLAY_ID, &IntrudeBattleProcData, &talk->ibp);
      GMEVENT_CallEvent(event, child_event);
    }
	  *seq = SEQ_FINISH;
	  break;
	
	case SEQ_ITEM_INIT:
    {
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
      MYSTATUS *target_myst = GAMEDATA_GetMyStatusPlayer(gamedata, talk->talk_netid);

      WORDSET_RegisterPlayerName( talk->iem.wordset, 0, target_myst );
      WORDSET_RegisterItemName( talk->iem.wordset, 1, ITEM_KIZUGUSURI );
      IntrudeEventPrint_StartStream(&talk->iem, msg_talk_item_002);
    }
    *seq = SEQ_ITEM_MSG_WAIT;
    break;
  case SEQ_ITEM_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_ITEM_LAST_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_FINISH;
    }
    break;
	  
	case SEQ_TALK_CANCEL:   //��b�L�����Z��
	  *seq = SEQ_FINISH;
	  break;
	  
	case SEQ_FINISH:   //�I������
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);

  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	Intrude_SetActionStatus(talk->intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(talk->intcomm, INTRUDE_NETID_NULL);
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �r���S�����b���|���C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommBingoEvent( GMEVENT *event, int *seq, void *wk )
{
	COMMTALK_EVENT_WORK *talk = wk;
	
	switch( *seq ){
	case 0:
    IntrudeEventPrint_StartStream(&talk->iem, msg_intrude_bingo000);
		(*seq)++;
		break;
	case 1:
	  if(IntrudeSend_BingoIntrusion(talk->intcomm->talk.talk_netid) == TRUE){
      (*seq)++;
    }
	case 2:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
			(*seq)++;
		}
		break;
	case 3:
	  {
      BINGO_INTRUSION_ANSWER answer = Bingo_GetBingoIntrusionAnswer(talk->intcomm);
      switch(answer){
      case BINGO_INTRUSION_ANSWER_OK:
        *seq = 100;
        break;
      case BINGO_INTRUSION_ANSWER_NG:
        *seq = 200;
        break;
      }
    }
		break;
	
  case 100:   //�Ԏ�OK
    IntrudeEventPrint_StartStream(&talk->iem, msg_intrude_bingo001);
	  (*seq)++;
    break;
  case 101:   //�����p�p�����[�^�v��
    if(IntrudeSend_ReqBingoBattleIntrusionParam(talk->intcomm->talk.talk_netid) == TRUE){
      (*seq)++;
    }
    break;
  case 102:   //�����p�p�����[�^��M�҂�
    if(Bingo_GetBingoIntrusionParam(talk->intcomm) == TRUE){
      (*seq)++;
    }
    break;
  case 103:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
			(*seq)++;
		}
		break;
	case 104:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
			(*seq) = 300;
		}
	  break;
	  
  case 200:   //�f��ꂽ
    IntrudeEventPrint_StartStream(&talk->iem, msg_intrude_bingo002);
		(*seq)++;
    break;
  case 201:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
			(*seq)++;
		}
		break;
	case 202:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
			(*seq) = 300;
		}
	  break;
	
	case 300:   //�I������
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);

  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	Intrude_SetActionStatus(talk->intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(talk->intcomm, INTRUDE_NETID_NULL);
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �~�b�V�����B���b���|���C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionAchieveEvent( GMEVENT *event, int *seq, void *wk )
{
	COMMTALK_EVENT_WORK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_INIT,
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_sys002);
      *seq = SEQ_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_INIT:
    (*seq)++;
    break;
	case SEQ_SEND_ACHIEVE:
    if(IntrudeSend_MissionAchieve(intcomm, &intcomm->mission) == TRUE){//�~�b�V�����B����e�ɓ`����
      (*seq)++;
    }
    break;
  case SEQ_RECV_WAIT:
		if(MISSION_CheckRecvResult(&intcomm->mission) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_MSG_INIT:
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
      MYSTATUS *target_myst = GAMEDATA_GetMyStatusPlayer(gdata,intcomm->mission.data.target_netid);
      MYSTATUS *mine_myst = GAMEDATA_GetMyStatus(gdata);
      int my_netid = GAMEDATA_GetIntrudeMyID(gdata);
      u16 msg_id;
      
      msg_id = MISSION_GetAchieveMsgID(&intcomm->mission, my_netid);

      WORDSET_RegisterPlayerName( talk->iem.wordset, 0, target_myst );
      WORDSET_RegisterPlayerName( talk->iem.wordset, 1, mine_myst );

      IntrudeEventPrint_StartStream(&talk->iem, msg_id);
    }
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);
    if(intcomm != NULL){
      if(MISSION_AddPoint(intcomm, &intcomm->mission) == TRUE){
        intcomm->send_occupy = TRUE;
      }
      MISSION_Init(&intcomm->mission);
    }

  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	Intrude_SetActionStatus(talk->intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(talk->intcomm, INTRUDE_NETID_NULL);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F����F�b���|���C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionItemEvent( GMEVENT *event, int *seq, void *wk )
{
	COMMTALK_EVENT_WORK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_FIRST_TALK,
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_sys002);
      *seq = SEQ_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
	case SEQ_FIRST_TALK:
	  {
      FIRST_TALK_RET first_ret = _FirstTalkSeq(intcomm, &talk->iem, &talk->first_talk_seq);
      switch(first_ret){
      case FIRST_TALK_RET_OK:
        *seq = SEQ_SEND_ACHIEVE;
        break;
      case FIRST_TALK_RET_NG:
        *seq = SEQ_END;
        break;
      case FIRST_TALK_RET_CANCEL:
        *seq = SEQ_END;
        break;
      }
    }
    break;

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
    MISSIONDATA_Wordset(intcomm,
       MISSION_GetRecvData(&intcomm->mission), talk->iem.wordset, talk->heapID);
    IntrudeEventPrint_StartStream(&talk->iem, MissionItemMsgID[0] + 0);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);
  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	Intrude_SetActionStatus(talk->intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(talk->intcomm, INTRUDE_NETID_NULL);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F�퓬���񕜁F�b���|���C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionBasicEvent( GMEVENT *event, int *seq, void *wk )
{
	COMMTALK_EVENT_WORK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_INIT,
    SEQ_SEND_PLAYER_SUPPORT,
    SEQ_SEND_ACHIEVE,
    SEQ_RECV_WAIT,
    SEQ_MSG_WAIT,
    SEQ_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) < SEQ_MSG_WAIT){
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_sys002);
      *seq = SEQ_MSG_WAIT;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_INIT:
    MISSIONDATA_Wordset(intcomm,
       MISSION_GetRecvData(&intcomm->mission), talk->iem.wordset, talk->heapID);
    IntrudeEventPrint_StartStream(&talk->iem, MissionBasicMsgID[0] + 0);
    (*seq)++;
    break;
	case SEQ_SEND_PLAYER_SUPPORT:
	  if(IntrudeSend_PlayerSupport(intcomm, talk->talk_netid, SUPPORT_TYPE_RECOVER_FULL) == TRUE){
      (*seq)++;
    }
    break;
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
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      *seq = SEQ_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      *seq = SEQ_END;
    }
    break;
  case SEQ_END:
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);
  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	Intrude_SetActionStatus(talk->intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(talk->intcomm, INTRUDE_NETID_NULL);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}


//==================================================================
/**
 * �~�b�V�������ʒʒm�C�x���g�N��(�������B���ł͂Ȃ��ʐM���肪�B���������ʂ��͂���)
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   intcomm		      �N���V�X�e�����[�N�ւ̃|�C���^
 * @param   fmmdl_player		���@���샂�f��
 * @param   heap_id		      �q�[�vID
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommMissionResult(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, HEAPID heap_id)
{
	COMMTALK_EVENT_WORK *ftalk_wk;
	GMEVENT *event;
	
	event = GMEVENT_Create(
 		gsys, NULL,	CommMissionResultEvent, sizeof(COMMTALK_EVENT_WORK) );

	ftalk_wk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( ftalk_wk, sizeof(COMMTALK_EVENT_WORK) );
	
	ftalk_wk->heapID = heap_id;
	ftalk_wk->fmmdl_player = fmmdl_player;
	ftalk_wk->fieldWork = fieldWork;
	ftalk_wk->intcomm = intcomm;
	
  IntrudeEventPrint_SetupFieldMsg(&ftalk_wk->iem, gsys);

	return( event );
}

//--------------------------------------------------------------
/**
 * �~�b�V�������ʒʒm�C�x���g(�������B���ł͂Ȃ��ʐM���肪�B���������ʂ��͂���)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionResultEvent( GMEVENT *event, int *seq, void *wk )
{
	COMMTALK_EVENT_WORK *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_INIT,
    SEQ_KEY_WAIT,
    SEQ_POINT_GET_CHECK,
    SEQ_POINT_GET,
    SEQ_POINT_GET_MSG_WAIT,
    SEQ_POINT_GET_MSG_END_BUTTON_WAIT,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) < SEQ_POINT_GET_MSG_WAIT){
      *seq = SEQ_END;
      talk->error = TRUE;
    }
  }

	switch( *seq ){
  case SEQ_INIT:
    IntrudeEventPrint_SetupExtraMsgWin(&talk->iem, gsys, 1, 1, 32-2, 16);
    
    MISSIONDATA_Wordset(intcomm,
       &(MISSION_GetResultData(&intcomm->mission))->mission_data, talk->iem.wordset, talk->heapID);
    {
      u16 explain_msgid, title_msgid;
      const MISSION_RESULT *mresult = MISSION_GetResultData(&intcomm->mission);
      
      title_msgid = msg_mistype_000 + mresult->mission_data.cdata.type;
      explain_msgid = mresult->mission_data.cdata.msg_id_contents_monolith;
      
      IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&talk->iem, title_msgid, 8, 0);
      IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&talk->iem, explain_msgid, 8, 16);
      if(MISSION_CheckResultMissionMine(intcomm, &intcomm->mission) == TRUE){ //����
        IntrudeEventPrint_Print(&talk->iem, msg_invasion_mission_sys004, 0, 0);
      }
      else{ //���s
        IntrudeEventPrint_Print(&talk->iem, msg_invasion_mission_sys002, 0, 0);
      }
    }
    (*seq)++;
    break;
  case SEQ_KEY_WAIT:
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)){
      IntrudeEventPrint_ExitExtraMsgWin(&talk->iem);
      (*seq) = SEQ_POINT_GET_CHECK;
    }
    break;

  case SEQ_POINT_GET_CHECK:   //��V�Q�b�g������
    if(MISSION_CheckResultMissionMine(intcomm, &intcomm->mission) == TRUE){
      *seq = SEQ_POINT_GET;
    }
    else{
      *seq = SEQ_END;
    }
    break;
  case SEQ_POINT_GET:         //��V�Q�b�g
    WORDSET_RegisterNumber( talk->iem.wordset, 0, 
      MISSION_GetResultPoint(intcomm, &intcomm->mission), 
      3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_sys003);
    (*seq)++;
    break;
  case SEQ_POINT_GET_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      *seq = SEQ_POINT_GET_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_POINT_GET_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      if(MISSION_AddPoint(intcomm, &intcomm->mission) == TRUE){
        intcomm->send_occupy = TRUE;
      }
      (*seq) = SEQ_END;
    }
    break;

  case SEQ_END:
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);

  #if 0
    //��check�@�~�b�V������������Ȃ��̂ŁA�����őS��
    if(intcomm->mission.data.target_netid == GAMEDATA_GetIntrudeMyID(gdata)){
      STATUS_RCV_PokeParty_RecoverAll(GAMEDATA_GetMyPokemon(gdata));
    }
  #endif
    MISSION_Init(&intcomm->mission);

    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

