//==============================================================================
/**
 * @file    event_mission_normal.c
 * @brief   �~�b�V�����F���̑���b
 * @author  matsuda
 * @date    2010.04.19(��)
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
#include "field/field_comm/intrude_battle.h"
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
#include "event_comm_notarget.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_NOTARGET
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
	u16 msg_id;

	INTRUDE_TALK_STATUS answer_status;
	INTRUDE_BATTLE_PARENT ibp;
}EVENT_MISSION_NOTARGET;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _CommonEventChangeSet(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew, const u16 *msg_tbl);
static GMEVENT_RESULT CommMissionEtc_CommonEvent( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  �f�[�^
//==============================================================================
///�퓬��Ԃ̑���ɘb��������(�~�b�V�������s�ґ�)
ALIGN4 static const u16 Talk_M_Battle[TALK_TYPE_MAX] = {
  mis_btl_01_m1,
  mis_btl_01_m2,
  mis_btl_01_m3,
  mis_btl_01_m4,
  mis_btl_01_m5,
};

///�b���������o���Ȃ�����ɘb��������(�~�b�V�������s�ґ�)
ALIGN4 static const u16 Talk_M_NotTalk[TALK_TYPE_MAX] = {
  mis_std_01_m1,
  mis_std_01_m2,
  mis_std_01_m3,
  mis_std_01_m4,
  mis_std_01_m5,
};

///�~�b�V�����Q���ғ��m�̘b������(�b����������)
ALIGN4 static const u16 Talk_MissionMission[MISSION_TYPE_MAX][TALK_TYPE_MAX] = {
  {//MISSION_TYPE_VICTORY
    mis_m03_06_m1,
    mis_m03_06_m2,
    mis_m03_06_m3,
    mis_m03_06_m4,
    mis_m03_06_m5,
  },
  {//MISSION_TYPE_SKILL
    mis_m01_04_m1,
    mis_m01_04_m2,
    mis_m01_04_m3,
    mis_m01_04_m4,
    mis_m01_04_m5,
  },
  {//MISSION_TYPE_BASIC
    mis_m02_04_m1,
    mis_m02_04_m2,
    mis_m02_04_m3,
    mis_m02_04_m4,
    mis_m02_04_m5,
  },
  {//MISSION_TYPE_ATTRIBUTE
    mis_m05_01_m1,
    mis_m05_01_m2,
    mis_m05_01_m3,
    mis_m05_01_m4,
    mis_m05_01_m5,
  },
  {//MISSION_TYPE_ITEM
    mis_m06_06_m1,
    mis_m06_06_m2,
    mis_m06_06_m3,
    mis_m06_06_m4,
    mis_m06_06_m5,
  },
  {//MISSION_TYPE_PERSONALITY
    mis_m04_05_m1,
    mis_m04_05_m2,
    mis_m04_05_m3,
    mis_m04_05_m4,
    mis_m04_05_m5,
  },
};

///�~�b�V�����Q���ғ��m�̘b������(�b����������)
ALIGN4 static const u16 Talked_MissionMission[MISSION_TYPE_MAX][TALK_TYPE_MAX] = {
  {//MISSION_TYPE_VICTORY
    mis_m03_06_t1,
    mis_m03_06_t2,
    mis_m03_06_t3,
    mis_m03_06_t4,
    mis_m03_06_t5,
  },
  {//MISSION_TYPE_SKILL
    mis_m01_04_t1,
    mis_m01_04_t2,
    mis_m01_04_t3,
    mis_m01_04_t4,
    mis_m01_04_t5,
  },
  {//MISSION_TYPE_BASIC
    mis_m02_04_t1,
    mis_m02_04_t2,
    mis_m02_04_t3,
    mis_m02_04_t4,
    mis_m02_04_t5,
  },
  {//MISSION_TYPE_ATTRIBUTE
    mis_m05_02_t1,
    mis_m05_02_t2,
    mis_m05_02_t3,
    mis_m05_02_t4,
    mis_m05_02_t5,
  },
  {//MISSION_TYPE_ITEM
    mis_m06_06_t1,
    mis_m06_06_t2,
    mis_m06_06_t3,
    mis_m06_06_t4,
    mis_m06_06_t5,
  },
  {//MISSION_TYPE_PERSONALITY
    mis_m04_05_t1,
    mis_m04_05_t2,
    mis_m04_05_t3,
    mis_m04_05_t4,
    mis_m04_05_t5,
  },
};

///�T�ώ҂ƃ~�b�V�����Q���҂̉�b(�~�b�V�����Q���ґ�)
ALIGN4 static const u16 Talk_M_Boukansya[TALK_TYPE_MAX] = {
  mis_nom_01_m1,
  mis_nom_01_m2,
  mis_nom_01_m3,
  mis_nom_01_m4,
  mis_nom_01_m5,
};

///�T�ώ҂ƃ~�b�V�����Q���҂̉�b(�T�ώґ�)
ALIGN4 static const u16 Talk_N_Boukansya[TALK_TYPE_MAX] = {
  mis_nom_01_t1,
  mis_nom_01_t2,
  mis_nom_01_t3,
  mis_nom_01_t4,
  mis_nom_01_t5,
};


//--------------------------------------------------------------
/**
 * �C�x���g�؂�ւ��F���ʏ���
 *
 * @param   event		
 * @param   ccew		
 * @param   msg_tbl		���b�Z�[�WID�̃e�[�u��
 */
//--------------------------------------------------------------
static void _CommonEventChangeSet(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew, const u16 *msg_tbl)
{
 	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  EVENT_MISSION_NOTARGET *talk;

  intcomm = Intrude_Check_CommConnect(game_comm);
  
  GMEVENT_Change(event, CommMissionEtc_CommonEvent, sizeof(EVENT_MISSION_NOTARGET));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
	
  if(intcomm != NULL){  //���̃^�C�~���O��intcomm��NULL�̏ꍇ�͖������ꉞ
  	talk->msg_id = msg_tbl[MISSION_FIELD_GetTalkType(intcomm, ccew->talk_netid)];
    WORDSET_RegisterPlayerName( //�C�x���g����intcomm�Ɉˑ����Ȃ��ėǂ��悤�ɂ����ŃZ�b�g
      ccew->iem.wordset, 0, Intrude_GetMyStatus(intcomm, ccew->talk_netid) );
  }
  else{
    GF_ASSERT(0);
  }
}

//--------------------------------------------------------------
/**
 * �~�b�V�����F���ʏ����Ftalk->msg_id�ɐݒ肳��Ă��郁�b�Z�[�W���o�͂��ďI��
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event talk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT CommMissionEtc_CommonEvent( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_NOTARGET *talk = wk;
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
    //intcomm�Ɉˑ����Ă��Ȃ��̂Ń`�F�b�N����K�v�͂Ȃ�
  }

	switch( *seq ){
  case SEQ_MSG_INIT:
    IntrudeEventPrint_StartStream(&talk->ccew.iem, talk->msg_id);
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
  		(*seq)++;
  	}
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
//  �퓬��Ԃ̑���ɘb��������
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�퓬��Ԃ̑���ɘb��������(�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionEtc_MtoT_Battle_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  _CommonEventChangeSet(event, ccew, Talk_M_Battle);
}

//==================================================================
/**
 * �C�x���g�؂�ւ��F�b���������o���Ȃ���Ԃ̑���ɘb��������(�������~�b�V�������s�҂Ń^�[�Q�b�g�ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionEtc_MtoT_NotTalk_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  _CommonEventChangeSet(event, ccew, Talk_M_NotTalk);
}

//==================================================================
/**
 * �C�x���g�؂�ւ��F�~�b�V�����Q���ғ��m�̘b������(�������~�b�V�������s�҂Ń~�b�V�������s�҂ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionEtc_MtoM_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  MISSION_TYPE mission_type = ccew->mdata.cdata.type;
  
  if(mission_type >= MISSION_TYPE_MAX){
    mission_type = 0;
  }
  _CommonEventChangeSet(event, ccew, Talk_MissionMission[mission_type]);
}

//==================================================================
/**
 * �C�x���g�؂�ւ��F�~�b�V�����Q���ғ��m�̘b������(�������~�b�V�������s�҂Ń~�b�V�������s�҂ɘb��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionEtc_MtoM_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  MISSION_TYPE mission_type = ccew->mdata.cdata.type;
  
  if(mission_type >= MISSION_TYPE_MAX){
    mission_type = 0;
  }
  _CommonEventChangeSet(event, ccew, Talked_MissionMission[mission_type]);
}

//==================================================================
/**
 * �C�x���g�؂�ւ��F�T�ώ҂��~�b�V�����Q���҂ɘb��������(�������~�b�V�������s�҂ŖT�ώ҂ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionEtc_MtoN_M(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  _CommonEventChangeSet(event, ccew, Talk_M_Boukansya);
}

//==================================================================
/**
 * �C�x���g�؂�ւ��F�T�ώ҂��~�b�V�����Q���҂ɘb��������(�������T�ώ҂Ń~�b�V�������s�҂ɘb��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionEtc_MtoN_N(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  _CommonEventChangeSet(event, ccew, Talk_N_Boukansya);
}
