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
#include "event_comm_result.h"
#include "sound/pm_sndsys.h"
#include "savedata/intrude_save_field.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"



//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	COMMTALK_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	HEAPID heapID;
	FIELDMAP_WORK *fieldWork;
  
	INTRUDE_EVENT_MSGWORK iem;
	BOOL error;
	
	INTRUDE_EVENT_DISGUISE_WORK iedw;
	COMMTALK_COMMON_EVENT_WORK ccew;
	
	MISSION_RESULT mresult;
	u16 title_msgid;
	u16 explain_msgid;
	BOOL mission_result;    ///<TRUE:�������~�b�V�����B����
	s32 point;
}COMMTALK_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommMissionResultEvent( GMEVENT *event, int *seq, void *wk );



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
	COMMTALK_EVENT_WORK *talk;
	GMEVENT *event;
	
	event = GMEVENT_Create(
 		gsys, NULL,	CommMissionResultEvent, sizeof(COMMTALK_EVENT_WORK) );

	talk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( talk, sizeof(COMMTALK_EVENT_WORK) );
	
	talk->heapID = heap_id;
	talk->fieldWork = fieldWork;
	
  IntrudeEventPrint_SetupFieldMsg(&talk->iem, gsys);

  //�C�x���g���A�G���[�ɂȂ��Ă��\��Ȃ��悤��intcomm����K�v�ȃp�����[�^�������őS�Ď擾
  //  ��BGM��Push,Pop������ׁA�r���ŃC�x���g����߂�ɂ͂�����ӂ̃P�A���K�v�ɂȂ�
  {
    const MISSION_RESULT *mresult = MISSION_GetResultData(&intcomm->mission);
    
    GF_ASSERT(mresult != NULL);
    talk->mresult = *mresult;
    talk->title_msgid = msg_mistype_000 + mresult->mission_data.cdata.type;
    talk->explain_msgid = mresult->mission_data.cdata.msg_id_contents_monolith;
    
    talk->mission_result = MISSION_CheckResultMissionMine(intcomm, &intcomm->mission);
    talk->point = MISSION_GetResultPoint(intcomm, &intcomm->mission);
  }
  
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
    SEQ_RESULT_KEY_WAIT,
    SEQ_POINT_GET_CHECK,   //��V�Q�b�g������
    SEQ_POINT_GET,         //��V�Q�b�g
    SEQ_POINT_GET_MSG_WAIT,
    SEQ_LEVELUP_MSG,
    SEQ_LEVELUP_MSG_WAIT,
    SEQ_POINT_GET_MSG_END_BUTTON_WAIT,
    SEQ_MISSION_FAIL,    //�~�b�V�������s
    SEQ_PALACE_WARP,
    SEQ_DISGUISE_START,  //�ϑ��߂�
    SEQ_DISGUISE_MAIN,
    SEQ_END,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  // ��BGM��Push,Pop������ׁA�r���ŃC�x���g�͒�~���Ȃ�
  //   ���ׁ̈Aintcomm��NULL���ǂ����́A���̎g�p�|�C���g�ł��ꂼ��NULL��������Ďg�p���邱�ƁI�I
  //   ��check�@�}�X�^�[�A�b�v�O��intcomm�g�p�ӏ���NULL�`�F�b�N���Ă��邩��������

	switch( *seq ){
  case SEQ_INIT:
    MISSIONDATA_Wordset(&talk->mresult.mission_data.cdata, 
      &talk->mresult.mission_data.target_info, talk->iem.wordset, talk->heapID);
    IntrudeEventPrint_SetupExtraMsgWin(&talk->iem, gsys, 1, 4, 32-2, 16);
    IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&talk->iem, talk->title_msgid, 8, 0);
    IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&talk->iem, talk->explain_msgid, 8, 16);
    (*seq)++;
    break;
  case SEQ_RESULT_KEY_WAIT:
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)){
      IntrudeEventPrint_ExitExtraMsgWin(&talk->iem);
      (*seq) = SEQ_POINT_GET_CHECK;
    }
    break;

  case SEQ_POINT_GET_CHECK:   //��V�Q�b�g������
    if(talk->mission_result == TRUE){   //����
      IntrudeEventPrint_Print(&talk->iem, msg_invasion_mission_sys004, 0, 0);
      GMEVENT_CallEvent(event, EVENT_FSND_PushPlayJingleBGM(gsys, SEQ_ME_MISSION_CLEAR ));
      *seq = SEQ_POINT_GET;
    }
    else{   //���s
      IntrudeEventPrint_Print(&talk->iem, msg_invasion_mission_sys002, 0, 0);
      GMEVENT_CallEvent(event, EVENT_FSND_PushPlayJingleBGM(gsys, SEQ_ME_MISSION_FAILED ));
      *seq = SEQ_MISSION_FAIL;
    }
    break;
  case SEQ_POINT_GET:         //��V�Q�b�g
    if( PMSND_CheckPlayBGM() == FALSE ){
      GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_NONE, FSND_FADE_FAST));

      { //�p���X��
        MYITEM_PTR myitem = GAMEDATA_GetMyItem(gdata);
        u16 now_num = MYITEM_GetItemNum( myitem, ITEM_DERUDAMA, talk->heapID);
        int add_num = talk->point;
        
        if(MYITEM_GetItemMax( ITEM_DERUDAMA ) < now_num + add_num){
          add_num = ITEM_DERUDAMA - now_num;
        }
        MYITEM_AddItem(myitem, ITEM_DERUDAMA, add_num, talk->heapID);
      }

      WORDSET_RegisterNumber( talk->iem.wordset, 0, talk->point, 
        3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_sys003);

      (*seq)++;
    }
    break;
  case SEQ_POINT_GET_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_LEVELUP_MSG:
    WORDSET_RegisterNumber( talk->iem.wordset, 0, MISSION_ACHIEVE_ADD_LEVEL, 
      3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    if(talk->mresult.mission_data.monolith_type == MONOLITH_TYPE_BLACK){
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_clear_03);
    }
    else{
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_clear_02);
    }
    (*seq)++;
    break;
  case SEQ_LEVELUP_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      *seq = SEQ_POINT_GET_MSG_END_BUTTON_WAIT;
    }
    break;

  case SEQ_POINT_GET_MSG_END_BUTTON_WAIT:
    if(IntrudeEventPrint_LastKeyWait() == TRUE){
      GameCommInfo_MessageEntry_MissionSuccess(game_comm);
      (*seq) = SEQ_PALACE_WARP;
    }
    break;

  case SEQ_MISSION_FAIL:    //�~�b�V�������s
    if( PMSND_CheckPlayBGM() == FALSE ){
      GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_NONE, FSND_FADE_FAST));
      GameCommInfo_MessageEntry_MissionFail(game_comm);
      *seq = SEQ_PALACE_WARP;
    }
    break;
    
  case SEQ_PALACE_WARP:
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);

    GMEVENT_CallEvent(event, EVENT_IntrudeWarpPalace(gsys));
    (*seq) = SEQ_DISGUISE_START;
    break;
    
  case SEQ_DISGUISE_START:  //�ϑ��߂�
    IntrudeEvent_Sub_DisguiseEffectSetup(&talk->iedw, gsys, talk->fieldWork, 
      DISGUISE_NO_NORMAL, 0, 0);
    (*seq)++;
    break;
  case SEQ_DISGUISE_MAIN:
    if(IntrudeEvent_Sub_DisguiseEffectMain(&talk->iedw, intcomm) == TRUE){
      (*seq)++;
    }
    break;

  case SEQ_END:
  #if 0
    //��check�@�~�b�V������������Ȃ��̂ŁA�����őS��
    if(intcomm->mission.data.target_info.net_id == GAMEDATA_GetIntrudeMyID(gdata)){
      STATUS_RCV_PokeParty_RecoverAll(GAMEDATA_GetMyPokemon(gdata));
    }
  #endif
    if(intcomm != NULL){
      MISSION_Init(&intcomm->mission);
    }

    return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

