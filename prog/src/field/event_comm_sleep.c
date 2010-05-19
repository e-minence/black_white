//==============================================================================
/**
 * @file    event_comm_sleep.c
 * @brief   �~�b�V�����F�X���[�v���Ă���^�[�Q�b�g�ɘb��������
 * @author  matsuda
 * @date    2010.05.19(��)
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
#include "event_comm_sleep.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"
#include "../../../resource/fldmapdata/script/item_get_scr_def.h" //SCRID_�`

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	EVENT_MISSION_SLEEP
//--------------------------------------------------------------
typedef struct
{
  COMMTALK_COMMON_EVENT_WORK ccew;
	BOOL error;
	u16 msg_id;
}EVENT_MISSION_SLEEP;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _CommonEventChangeSet(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew, const u16 *msg_tbl);
static GMEVENT_RESULT CommMissionSleep_CommonEvent( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  �f�[�^
//==============================================================================
///�W����Ă���^�[�Q�b�g�ƃ~�b�V�����Q���҂̉�b
ALIGN4 static const u16 Talk_Sleep[TALK_TYPE_MAX] = {
  mis_sleep_01_m1,
  mis_sleep_01_m2,
  mis_sleep_01_m3,
  mis_sleep_01_m4,
  mis_sleep_01_m5,
};


//--------------------------------------------------------------
/**
 * �C�x���g�؂�ւ��F�W����Ă���^�[�Q�b�g�ɘb��������
 *
 * @param   event		
 * @param   ccew		
 */
//--------------------------------------------------------------
void EVENTCHANGE_CommMissionSleep_MtoT_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
 	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  EVENT_MISSION_SLEEP *talk;

  intcomm = Intrude_Check_CommConnect(game_comm);
  
  GMEVENT_Change(event, CommMissionSleep_CommonEvent, sizeof(EVENT_MISSION_SLEEP));
	talk = GMEVENT_GetEventWork( event );
  GFL_STD_MemCopy(ccew, &talk->ccew, sizeof(COMMTALK_COMMON_EVENT_WORK));
	
  if(intcomm != NULL){  //���̃^�C�~���O��intcomm��NULL�̏ꍇ�͖������ꉞ
  	talk->msg_id = Talk_Sleep[MISSION_FIELD_GetTalkType(intcomm, ccew->talk_netid)];
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
static GMEVENT_RESULT CommMissionSleep_CommonEvent( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_SLEEP *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
	enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_ITEM_GET_CALL,
    SEQ_ITEM_GET_EVENT_WAIT,
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
  	break;
  case SEQ_ITEM_GET_CALL:
    //�O���C�x���g���N������א�s���č폜
    IntrudeEventPrint_ExitFieldMsg(&talk->ccew.iem);

    {
      GMEVENT *item_event;
      SCRIPT_WORK* sc;
      
      item_event = SCRIPT_SetEventScript( gsys, SCRID_ITEM_EVENT_PROG_CALL, NULL, HEAPID_PROC );
      sc = SCRIPT_GetScriptWorkFromEvent( item_event );
      SCRIPT_SetScriptWorkParam( sc, ITEM_DERUDAMA, 1, 0, 0 );

      GMEVENT_CallEvent(event, item_event);
    }
    (*seq)++;
    break;
  case SEQ_ITEM_GET_EVENT_WAIT:
    *seq = SEQ_END;
    break;
  case SEQ_END:
  	//����Finish����
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);

  	//�p���X�֖߂����
    GMEVENT_ChangeEvent(event, EVENT_IntrudeForceWarpMyPalace(gsys, MISSION_FORCEWARP_MSGID_NULL));
    return GMEVENT_RES_CONTINUE;
  }
	return GMEVENT_RES_CONTINUE;
}

