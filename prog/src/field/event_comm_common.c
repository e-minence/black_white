//==============================================================================
/**
 * @file    event_comm_common.c
 * @brief   �~�b�V�����F�b�������A�b���������C�x���g���ʏ���
 * @author  matsuda
 * @date    2010.02.19(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"

#include "script.h"
#include "event_fieldtalk.h"

#include "print/wordset.h"
#include "field/field_comm/intrude_work.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_comm_command.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_monolith.h"
#include "msg/msg_mission_msg.h"
#include "field/field_comm/intrude_battle.h"
#include "field/event_fieldmap_control.h" //EVENT_FieldSubProc
#include "item/itemsym.h"
#include "event_intrude.h"
#include "event_comm_common.h"
#include "event_mission_normal.h"
#include "event_mission_talked.h"
#include "event_mission_talk.h"
#include "event_mission_battle.h"
#include "event_mission_help.h"
#include "event_mission_shop.h"
#include "event_mission_secretitem.h"
#include "event_comm_notarget.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	
//--------------------------------------------------------------
typedef struct{
  COMMTALK_COMMON_EVENT_WORK ccew;    //���ʃC�x���g���[�N
	u8 first_talk_seq;
	u8 padding[3];
}EVENT_COMM_COMMON;



//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _CommCommon_Init(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, COMMTALK_COMMON_EVENT_WORK *ccew, FIELDMAP_WORK *fieldWork, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id);
static GMEVENT_RESULT EventCommCommonTalk( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT EventCommCommonTalked( GMEVENT *event, int *seq, void *wk );
static void _EventChangeTalk(GMEVENT *event, EVENT_COMM_COMMON *talk, INTRUDE_COMM_SYS_PTR intcomm);
static void _EventChangeTalked(GMEVENT *event, EVENT_COMM_COMMON *talk, INTRUDE_COMM_SYS_PTR intcomm);
static INTRUDE_TALK_TYPE _IntrudeTalkTypeJudge(EVENT_COMM_COMMON *talk, INTRUDE_COMM_SYS_PTR intcomm);


//==============================================================================
//  �f�[�^
//==============================================================================
///�b���������F�C�x���g�؂�ւ��e�[�u��
static const EVENT_COMM_FUNC EventCommFuncTalkTbl[] = {
  EVENTCHANGE_CommMissionBattle_MtoT_Talk,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_N
  EVENTCHANGE_CommMissionBattle_TtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_T_to_M
  EVENTCHANGE_CommMissionNormal_Talk,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_T_to_N

  EVENTCHANGE_CommMissionTalked_MtoT_Talk,  //INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_N
  EVENTCHANGE_CommMissionTalked_TtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_SKILL_T_to_M
  EVENTCHANGE_CommMissionNormal_Talk,  //INTRUDE_TALK_TYPE_MISSION_SKILL_T_to_N

  EVENTCHANGE_CommMissionTalk_MtoT_Talk,  //INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_N
  EVENTCHANGE_CommMissionTalk_TtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_BASIC_T_to_M
  EVENTCHANGE_CommMissionNormal_Talk,  //INTRUDE_TALK_TYPE_MISSION_BASIC_T_to_N

  EVENTCHANGE_CommMissionShop_MtoT_Talk,//INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talk,   //INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,   //INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_N
  EVENTCHANGE_CommMissionShop_TtoM_Talk,//INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_T_to_M
  EVENTCHANGE_CommMissionNormal_Talk,   //INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_T_to_N

  EVENTCHANGE_CommMissionItem_MtoT_Talk,  //INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_N
  EVENTCHANGE_CommMissionItem_TtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_ITEM_T_to_M
  EVENTCHANGE_CommMissionNormal_Talk,  //INTRUDE_TALK_TYPE_MISSION_ITEM_T_to_N

  EVENTCHANGE_CommMissionHelp_MtoT_Talk,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_N
  EVENTCHANGE_CommMissionHelp_TtoM_Talk,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_T_to_M
  EVENTCHANGE_CommMissionNormal_Talk,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_T_to_N
};

///�b��������ꂽ�F�C�x���g�؂�ւ��e�[�u��
static const EVENT_COMM_FUNC EventCommFuncTalkedTbl[] = {
  EVENTCHANGE_CommMissionBattle_TtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_N
  EVENTCHANGE_CommMissionBattle_MtoT_Talked,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_T_to_M
  EVENTCHANGE_CommMissionNormal_Talked,  //INTRUDE_TALK_TYPE_MISSION_VICTORY_T_to_N

  EVENTCHANGE_CommMissionTalked_TtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_N
  EVENTCHANGE_CommMissionTalked_MtoT_Talked,  //INTRUDE_TALK_TYPE_MISSION_SKILL_T_to_M
  EVENTCHANGE_CommMissionNormal_Talked,  //INTRUDE_TALK_TYPE_MISSION_SKILL_T_to_N

  EVENTCHANGE_CommMissionTalk_TtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_N
  EVENTCHANGE_CommMissionTalk_MtoT_Talked,  //INTRUDE_TALK_TYPE_MISSION_BASIC_T_to_M
  EVENTCHANGE_CommMissionNormal_Talked,  //INTRUDE_TALK_TYPE_MISSION_BASIC_T_to_N

  EVENTCHANGE_CommMissionShop_TtoM_Talked,//INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talked,   //INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,   //INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_N
  EVENTCHANGE_CommMissionShop_MtoT_Talked,//INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_T_to_M
  EVENTCHANGE_CommMissionNormal_Talked,   //INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_T_to_N

  EVENTCHANGE_CommMissionItem_TtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_N
  EVENTCHANGE_CommMissionItem_MtoT_Talked,  //INTRUDE_TALK_TYPE_MISSION_ITEM_T_to_M
  EVENTCHANGE_CommMissionNormal_Talked,  //INTRUDE_TALK_TYPE_MISSION_ITEM_T_to_N

  EVENTCHANGE_CommMissionHelp_TtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_T
  EVENTCHANGE_CommMissionEtc_MtoM_Talked,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_M
  EVENTCHANGE_CommMissionEtc_MtoN_M,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_N
  EVENTCHANGE_CommMissionHelp_MtoT_Talked,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_T_to_M
  EVENTCHANGE_CommMissionNormal_Talked,  //INTRUDE_TALK_TYPE_MISSION_PERSONALITY_T_to_N
};





//==================================================================
/**
 * �ʐM�v���C���[�b�������C�x���g�i��������󋵖��ɃC�x���g���}�����ꂵ�Ă���)
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   intcomm		
 * @param   fmmdl_player		
 * @param   talk_net_id		
 * @param   heap_id		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommCommon_Talk(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id)
{
	EVENT_COMM_COMMON *talk;
	GMEVENT *event = NULL;
	
 	event = GMEVENT_Create(
    		gsys, NULL,	EventCommCommonTalk, sizeof(EVENT_COMM_COMMON) );
  
	talk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( talk, sizeof(EVENT_COMM_COMMON) );
	
	//���ʃC�x���g����������
	_CommCommon_Init(gsys, intcomm, &talk->ccew, fieldWork, fmmdl_player, talk_net_id, heap_id);
  Intrude_InitTalkWork(intcomm, talk_net_id);

  //��b�^�C�v����
  talk->ccew.intrude_talk_type = _IntrudeTalkTypeJudge(talk, intcomm);

	return( event );
}

//==================================================================
/**
 * �ʐM�v���C���[�b��������ꂽ�C�x���g�i��������󋵖��ɃC�x���g���}�����ꂵ�Ă���)
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   intcomm		
 * @param   fmmdl_player		
 * @param   talk_net_id		
 * @param   heap_id		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommCommon_Talked(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id)
{
	EVENT_COMM_COMMON *talk;
	GMEVENT *event = NULL;
	
 	event = GMEVENT_Create(
    		gsys, NULL,	EventCommCommonTalked, sizeof(EVENT_COMM_COMMON) );
  
	talk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( talk, sizeof(EVENT_COMM_COMMON) );
	
	//���ʃC�x���g����������
	_CommCommon_Init(gsys, intcomm, &talk->ccew, fieldWork, fmmdl_player, talk_net_id, heap_id);

  //����̑��M�f�[�^�ɂ���ĉ�b�^�C�v����
	talk->ccew.intrude_talk_type = intcomm->recv_talk_first_attack.talk_type;
	talk->ccew.mdata = intcomm->recv_talk_first_attack.mdata;
  
	return( event );
}

//--------------------------------------------------------------
/**
 * �C�x���g����Init����
 *
 * @param   intcomm		
 * @param   ccew		
 * @param   fieldWork		
 * @param   fmmdl_player		
 * @param   talk_net_id		
 * @param   heap_id		
 */
//--------------------------------------------------------------
static void _CommCommon_Init(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, COMMTALK_COMMON_EVENT_WORK *ccew, FIELDMAP_WORK *fieldWork, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id)
{
	//���ʃC�x���g���[�N�Z�b�g�A�b�v
	ccew->heapID = heap_id;
	ccew->fmmdl_player = fmmdl_player;
	ccew->fmmdl_talk = CommPlayer_GetMmdl(intcomm->cps, talk_net_id);
	ccew->fieldWork = fieldWork;
	ccew->talk_netid = talk_net_id;
	
	if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    GFL_STD_MemCopy(MISSION_GetRecvData(&intcomm->mission), &ccew->mdata, sizeof(MISSION_DATA));
  }
  
  IntrudeEventPrint_SetupFieldMsg(&ccew->iem, gsys);

	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_TALK);
}

//==================================================================
/**
 * �C�x���g����Finish����
 *
 * @param   intcomm		
 */
//==================================================================
void EVENT_CommCommon_Finish(INTRUDE_COMM_SYS_PTR intcomm, COMMTALK_COMMON_EVENT_WORK *ccew)
{
  IntrudeEventPrint_ExitFieldMsg(&ccew->iem);
	if(intcomm != NULL){
  	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(intcomm, INTRUDE_NETID_NULL);
  }
}


//--------------------------------------------------------------
/**
 * �ʐM�v���C���[�b�������C�x���g�i��������󋵖��ɃC�x���g���}�����ꂵ�Ă���)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EventCommCommonTalk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_COMM_COMMON *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_INIT,
    SEQ_FIRST_TALK,
    SEQ_TALK_OK,
    SEQ_TALK_CANCEL,
    SEQ_FINISH,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) <= SEQ_TALK_OK){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_FINISH;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
	case SEQ_INIT:
	  {
      MISSION_TYPE mission_type = MISSION_GetMissionType(&intcomm->mission);
      if(intcomm->intrude_status[talk->ccew.talk_netid].action_status != INTRUDE_ACTION_FIELD){
        //�b�����������Ԃł͂Ȃ����߁A����Ƃ̉�b�̐������m�F�����ɐ�p�C�x���g���N��
        COMMTALK_COMMON_EVENT_WORK *temp_ccew;
        
        //EventChange�Ń��[�N����������̂ŁA�����n���ׁA�e���|�����ɃR�s�[����
        temp_ccew = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(talk->ccew.heapID), sizeof(COMMTALK_COMMON_EVENT_WORK));
        GFL_STD_MemCopy(&talk->ccew, temp_ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));

        if(intcomm->intrude_status[talk->ccew.talk_netid].action_status == INTRUDE_ACTION_BATTLE){
          if(mission_type == MISSION_TYPE_PERSONALITY){
            //�퓬�菕��
            EVENTCHANGE_CommMissionHelp_TtoM_Battle(event, temp_ccew);
          }
          else{
            //�퓬��������b���������Ȃ�
            EVENTCHANGE_CommMissionEtc_MtoT_Battle_Talk(event, temp_ccew);
          }
        }
        else{ //�퓬�ȊO�̘b�����������Ԃł͂Ȃ�
          EVENTCHANGE_CommMissionEtc_MtoT_NotTalk_Talk(event, temp_ccew);
        }

        GFL_HEAP_FreeMemory(temp_ccew);
        break;
      }
    }
    (*seq)++;
    break;
	case SEQ_FIRST_TALK:
	  {
      FIRST_TALK_RET first_ret = EVENT_INTRUDE_FirstTalkSeq(intcomm, &talk->ccew, &talk->first_talk_seq);
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

  case SEQ_TALK_OK:   //�e�C�x���g�֎}������
    _EventChangeTalk(event, talk, intcomm);
    return GMEVENT_RES_CONTINUE;
    
	case SEQ_TALK_CANCEL:   //��b�L�����Z��
	  *seq = SEQ_FINISH;
	  break;
	case SEQ_FINISH:   //�I������
  	//����Finish����
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �ʐM�v���C���[�b��������ꂽ�C�x���g�i��������󋵖��ɃC�x���g���}�����ꂵ�Ă���)
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EventCommCommonTalked( GMEVENT *event, int *seq, void *wk )
{
	EVENT_COMM_COMMON *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_INIT,
    SEQ_PLAYER_DIR_CHANGE,
    SEQ_PLAYER_DIR_CHANGE_WAIT,
    SEQ_ANSWER_SEND_WAIT,
    SEQ_FINISH,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == FALSE){
      return GMEVENT_RES_CONTINUE;  //���b�Z�[�W�`�撆�͑҂�
    }
    if((*seq) < SEQ_FINISH){
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_invasion_mission_sys002);
      *seq = SEQ_FINISH;
      return GMEVENT_RES_CONTINUE;
    }
  }

	switch( *seq ){
	case SEQ_INIT:    //�Ԏ���Ԃ�
	  if(IntrudeSend_TalkAnswer(
        intcomm, intcomm->talk.talk_netid, intcomm->talk.talk_status) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_PLAYER_DIR_CHANGE: //�b����������ɐU�����
    if(MMDL_CheckPossibleAcmd(talk->ccew.fmmdl_player) == TRUE){
      u16 target_dir, anmcmd;
      
      target_dir = intcomm->intrude_status[talk->ccew.talk_netid].player_pack.dir;
      if(target_dir == DIR_LEFT){
        anmcmd = AC_DIR_R;
      }
      else if(target_dir == DIR_RIGHT){
        anmcmd = AC_DIR_L;
      }
      else if(target_dir == DIR_DOWN){
        anmcmd = AC_DIR_U;
      }
      else{
        anmcmd = AC_DIR_D;
      }
      MMDL_SetAcmd(talk->ccew.fmmdl_player, anmcmd);
      (*seq)++;
    }
    break;
  case SEQ_PLAYER_DIR_CHANGE_WAIT: //�U������I���҂�
    if(MMDL_EndAcmd(talk->ccew.fmmdl_player) == TRUE){ //�A�j���R�}���h�I���҂�
	    (*seq)++;
  	}
	  break;
  case SEQ_ANSWER_SEND_WAIT:   //�e�C�x���g�֎}������
    _EventChangeTalked(event, talk, intcomm);
  	return GMEVENT_RES_CONTINUE;
    
	case SEQ_FINISH:   //�I������
  	//����Finish����
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);
    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �b���������F�C�x���g�؂�ւ�
 *
 * @param   event		
 * @param   talk		
 * @param   intcomm		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
static void _EventChangeTalk(GMEVENT *event, EVENT_COMM_COMMON *talk, INTRUDE_COMM_SYS_PTR intcomm)
{
  COMMTALK_COMMON_EVENT_WORK *temp_ccew;
  
  //EventChange�Ń��[�N����������̂ŁA�����n���ׁA�e���|�����ɃR�s�[����
  temp_ccew = GFL_HEAP_AllocClearMemory(
    GFL_HEAP_LOWID(talk->ccew.heapID), sizeof(COMMTALK_COMMON_EVENT_WORK));
  GFL_STD_MemCopy(&talk->ccew, temp_ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));


  OS_TPrintf("intrude_talk_type = %d\n", talk->ccew.intrude_talk_type);
  if(talk->ccew.intrude_talk_type == INTRUDE_TALK_TYPE_NORMAL){  //�ʏ��b
    EVENTCHANGE_CommMissionNormal_Talk(event, temp_ccew);
  }
  else if(talk->ccew.intrude_talk_type == INTRUDE_TALK_TYPE_MISSION_N_to_M){  //���ʁF�T�ώҁ��~�b�V�������{��
    EVENTCHANGE_CommMissionEtc_MtoN_N(event, temp_ccew);
  }
  else if(talk->ccew.intrude_talk_type == INTRUDE_TALK_TYPE_MISSION_N_to_T){  //���ʁF�T�ώҁ��^�[�Q�b�g
    EVENTCHANGE_CommMissionNormal_Talk(event, temp_ccew);
  }
  else if(talk->ccew.intrude_talk_type >= INTRUDE_TALK_TYPE_MISSION_OFFSET_START){
    GF_ASSERT(EventCommFuncTalkTbl[talk->ccew.intrude_talk_type - INTRUDE_TALK_TYPE_MISSION_VICTORY_START] != NULL);
    EventCommFuncTalkTbl[talk->ccew.intrude_talk_type - INTRUDE_TALK_TYPE_MISSION_VICTORY_START](event, temp_ccew);
  }
  else{
    GF_ASSERT(0);
    //�ʏ��b���N�����Ă���
    EVENTCHANGE_CommMissionNormal_Talk(event, temp_ccew);
  }


  GFL_HEAP_FreeMemory(temp_ccew);
}

//--------------------------------------------------------------
/**
 * �b��������ꂽ�F�C�x���g�؂�ւ�
 *
 * @param   event		
 * @param   talk		
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void _EventChangeTalked(GMEVENT *event, EVENT_COMM_COMMON *talk, INTRUDE_COMM_SYS_PTR intcomm)
{
  COMMTALK_COMMON_EVENT_WORK *temp_ccew;
  
  //EventChange�Ń��[�N����������̂ŁA�����n���ׁA�e���|�����ɃR�s�[����
  temp_ccew = GFL_HEAP_AllocClearMemory(
    GFL_HEAP_LOWID(talk->ccew.heapID), sizeof(COMMTALK_COMMON_EVENT_WORK));
  GFL_STD_MemCopy(&talk->ccew, temp_ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));


  OS_TPrintf("intrude_talk_type = %d\n", talk->ccew.intrude_talk_type);
  if(talk->ccew.intrude_talk_type == INTRUDE_TALK_TYPE_NORMAL){  //�ʏ��b
    EVENTCHANGE_CommMissionNormal_Talked(event, temp_ccew);
  }
  else if(talk->ccew.intrude_talk_type == INTRUDE_TALK_TYPE_MISSION_N_to_M){  //���ʁF�T�ώҁ��~�b�V�������{��
    EVENTCHANGE_CommMissionEtc_MtoN_N(event, temp_ccew);
  }
  else if(talk->ccew.intrude_talk_type == INTRUDE_TALK_TYPE_MISSION_N_to_T){  //���ʁF�T�ώҁ��^�[�Q�b�g
    EVENTCHANGE_CommMissionNormal_Talked(event, temp_ccew);
  }
  else if(talk->ccew.intrude_talk_type >= INTRUDE_TALK_TYPE_MISSION_OFFSET_START){
    GF_ASSERT(EventCommFuncTalkedTbl[talk->ccew.intrude_talk_type - INTRUDE_TALK_TYPE_MISSION_VICTORY_START] != NULL);
    EventCommFuncTalkedTbl[talk->ccew.intrude_talk_type - INTRUDE_TALK_TYPE_MISSION_VICTORY_START](event, temp_ccew);
  }
  else{
    GF_ASSERT(0);
    //�ʏ��b���N�����Ă���
    EVENTCHANGE_CommMissionNormal_Talked(event, temp_ccew);
  }


  GFL_HEAP_FreeMemory(temp_ccew);
}

//--------------------------------------------------------------
/**
 * �b�������^�C�v�����肷��
 *
 * @param   talk		
 * @param   intcomm		
 */
//--------------------------------------------------------------
static INTRUDE_TALK_TYPE _IntrudeTalkTypeJudge(EVENT_COMM_COMMON *talk, INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork(talk->ccew.fieldWork) );
  NetID my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);
  MISSION_TYPE mission_type = MISSION_GetMissionType(&intcomm->mission);
  INTRUDE_TALK_TYPE intrude_talk_type = INTRUDE_TALK_TYPE_NORMAL;
  
  //�~�b�V�������{���ł���
  if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    //�~�b�V�������{�҂ł���
    if(MISSION_GetMissionEntry(&intcomm->mission) == TRUE){
    	//�b�����������肪�~�b�V�����^�[�Q�b�g�̑���ł���
    	if(MISSION_CheckMissionTargetNetID(&intcomm->mission, talk->ccew.talk_netid) == TRUE){
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_OFFSET_M_to_T;
      }
      //�b��������������~�b�V�������{�҂ł���
      else if(intcomm->intrude_status[talk->ccew.talk_netid].mission_entry == TRUE){
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_OFFSET_M_to_M;
      }
      else{         //�b������������͖T�ώ҂ł���
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_OFFSET_M_to_N;
      }
    }
    //�������^�[�Q�b�g�ł���
    else if(MISSION_CheckMissionTargetNetID(&intcomm->mission, my_net_id) == TRUE){
      //�b�����������肪�~�b�V�������{�҂ł���
      if(intcomm->intrude_status[talk->ccew.talk_netid].mission_entry == TRUE){
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_OFFSET_T_to_M;
      }
      else{   //�b������������͖T�ώ�
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_OFFSET_T_to_N;
      }
    }
    //�����͖T�ώ҂ł���
    else{
      //����̓~�b�V�������{�҂ł���
      if(intcomm->intrude_status[talk->ccew.talk_netid].mission_entry == TRUE){
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_N_to_M;
      }
      //����̓^�[�Q�b�g�ł���
      else if(MISSION_CheckMissionTargetNetID(&intcomm->mission, talk->ccew.talk_netid) == TRUE){
        intrude_talk_type = INTRUDE_TALK_TYPE_MISSION_N_to_T;
      }
      else{ //������T�ώ҂ł���
        intrude_talk_type = INTRUDE_TALK_TYPE_NORMAL;
      }
    }
  }
  else{ //�~�b�V�����͎��{���Ă��Ȃ�
    intrude_talk_type = INTRUDE_TALK_TYPE_NORMAL;
  }
  
  if(intrude_talk_type >= INTRUDE_TALK_TYPE_MISSION_OFFSET_START){
    switch(mission_type){ //�~�b�V�������̃I�t�Z�b�g�𑫂�����
    case MISSION_TYPE_VICTORY:
      intrude_talk_type += INTRUDE_TALK_TYPE_MISSION_VICTORY_START;
      break;
    case MISSION_TYPE_SKILL:
      intrude_talk_type += INTRUDE_TALK_TYPE_MISSION_SKILL_START;
      break;
    case MISSION_TYPE_BASIC:
      intrude_talk_type += INTRUDE_TALK_TYPE_MISSION_BASIC_START;
      break;
    case MISSION_TYPE_ATTRIBUTE:
      intrude_talk_type += INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_START;
      break;
    case MISSION_TYPE_ITEM:
      intrude_talk_type += INTRUDE_TALK_TYPE_MISSION_ITEM_START;
      break;
    case MISSION_TYPE_PERSONALITY:
      intrude_talk_type += INTRUDE_TALK_TYPE_MISSION_PERSONALITY_START;
      break;
    }
    intrude_talk_type -= INTRUDE_TALK_TYPE_MISSION_OFFSET_START;
  }
  
  return intrude_talk_type;
}

