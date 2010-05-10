//==============================================================================
/**
 * @file    event_mission_normal.c
 * @brief   �~�b�V�����F�ʏ��b
 * @author  matsuda
 * @date    2010.02.21(��)
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
#include "field/field_comm/bingo_system.h"
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
#include "event_mission_normal.h"

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

	INTRUDE_TALK_STATUS answer_status;
	INTRUDE_BATTLE_PARENT ibp;
}EVENT_MISSION_NORMAL;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommMissionNormal_Talk( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�ʏ��b  (�b��������)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionNormal_Talk(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENT_MISSION_NORMAL *talk;
  
  GMEVENT_Change(event, CommMissionNormal_Talk, sizeof(EVENT_MISSION_NORMAL));
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
static GMEVENT_RESULT CommMissionNormal_Talk( GMEVENT *event, int *seq, void *wk )
{
	EVENT_MISSION_NORMAL *talk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  enum{
    SEQ_MSG_INIT,
    SEQ_MSG_WAIT,
    SEQ_LAST_KEY,
    SEQ_FINISH,
  };

  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    //intcomm�Ɉˑ����Ă��Ȃ��̂Ō���G���[�`�F�b�N����K�v�͖���
  }
	
	switch( *seq ){
  case SEQ_MSG_INIT:
    {
      //intcomm�Ɉˑ����Ȃ��ōςނ悤��GAMEDATA���璼�ڈ�������
      MYSTATUS *target_myst = GAMEDATA_GetMyStatusPlayer(gamedata, talk->ccew.talk_netid);
      u32 msg_id;
      
      WORDSET_RegisterPlayerName( talk->ccew.iem.wordset, 0, target_myst );
      if(MyStatus_GetMySex(target_myst) == PM_MALE){
        msg_id = msg_intrude_normal_talk_m;
      }
      else{
        msg_id = msg_intrude_normal_talk_w;
      }
      IntrudeEventPrint_StartStream(&talk->ccew.iem, msg_id);
    }
		(*seq)++;
		break;
  case SEQ_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->ccew.iem) == TRUE){
			(*seq)++;
		}
		break;
  case SEQ_LAST_KEY:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      (*seq)++;
    }
    break;
	  
	case SEQ_FINISH:
  	EVENT_CommCommon_Finish(intcomm, &talk->ccew);
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �C�x���g�؂�ւ��F�ʏ��b  (�b��������ꂽ)
 *
 * @param   event		
 * @param   ccew		
 */
//==================================================================
void EVENTCHANGE_CommMissionNormal_Talked(GMEVENT *event, const COMMTALK_COMMON_EVENT_WORK *ccew)
{
  EVENTCHANGE_CommMissionNormal_Talk(event, ccew);
}

