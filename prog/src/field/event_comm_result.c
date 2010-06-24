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
#include "event_comm_result.h"
#include "sound/pm_sndsys.h"
#include "savedata/intrude_save_field.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���ʃE�B���h�E��\�����Ă��鎞��
#define RESULT_WINDOW_PRINT_TIME      (30 * 2)


//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	COMMTALK_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	HEAPID heapID;
  
	INTRUDE_EVENT_MSGWORK iem;
	BOOL recv_achieve;
	BOOL error;
	
	INTRUDE_EVENT_DISGUISE_WORK iedw;
	COMMTALK_COMMON_EVENT_WORK ccew;
	
	MISSION_RESULT mresult;
	u16 title_msgid;
	u16 explain_msgid;
	BOOL mission_result;    ///<TRUE:�������~�b�V�����B����
	s32 point;
	u8 add_white;
	u8 add_black;
	u16 wait;
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
 * @param   heap_id		      �q�[�vID
 * @param   BOOL            TRUE:�B�����ʂ���M�Ă���̂��Ăяo�����Ŋm�F���Ă���
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommMissionResult(GAMESYS_WORK *gsys, BOOL recv_achieve)
{
	COMMTALK_EVENT_WORK *talk;
	GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
	
	event = GMEVENT_Create(
 		gsys, NULL,	CommMissionResultEvent, sizeof(COMMTALK_EVENT_WORK) );

	talk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( talk, sizeof(COMMTALK_EVENT_WORK) );
	
	talk->heapID = FIELDMAP_GetHeapID(fieldWork);
	talk->recv_achieve = recv_achieve;

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
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
	enum{
    SEQ_RESULT_RECV_WAIT,   //���ʎ�M�҂�
    SEQ_INIT,
    SEQ_RESULT_KEY_WAIT,
    SEQ_POINT_GET_CHECK,   //��V�Q�b�g������
    SEQ_POINT_GET,         //��V�Q�b�g
    SEQ_POINT_GET_MSG_WAIT,
    SEQ_LEVELUP_BLACK_MSG,
    SEQ_LEVELUP_BLACK_MSG_WAIT,
    SEQ_LEVELUP_WHITE_MSG,
    SEQ_LEVELUP_WHITE_MSG_WAIT,
    SEQ_POINT_GET_MSG_END_BUTTON_WAIT,
    SEQ_MISSION_FAIL,    //�~�b�V�������s
    SEQ_END,
  };
	
  // ��BGM��Push,Pop������ׁA�G���[�ɂ��ޏo��PushPop�̊Ԃōs��Ȃ����I�I
  
	switch( *seq ){
	case SEQ_RESULT_RECV_WAIT:  //���ʎ�M�҂�
	  {
      INTRUDE_COMM_SYS_PTR intcomm = NULL;
        
      //���ʔ��\�̎��_�ł͊��ɐؒf�����ɍs���Ă���ꍇ������̂�Check_CommConnect�ł͖���
      //GameComm���璼�ڎ擾
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION){
        intcomm = GameCommSys_GetAppWork(game_comm);
      }
      
      if(talk->recv_achieve == FALSE){  //��M�m�F���Ă��Ȃ��ꍇ�͑��I��
        //�����̏����͌��ʂ��󂯎��������ɃG���[����������Ƃ���
        //��b�������Ɍ��ʃV�[�P���X�ɗ���ׁA���̎��p�ɂ͑��I���A�Ƃ���
        if(intcomm != NULL && NetErr_App_CheckError() == NET_ERR_STATUS_NULL
            && MISSION_CheckResultMissionMine(intcomm, &intcomm->mission) == TRUE){
          //�������B���҂��������A���ʂ��󂯎��O�ɐؒf���m�Ō��ʉ�ʂ܂Ŕ�΂��ꂽ
          intcomm->achieve_not_result = TRUE;
        }
        return GMEVENT_RES_FINISH;
      }
      else if(intcomm != NULL && MISSION_CheckRecvResult(&intcomm->mission) == TRUE
          && Intrude_CheckRecvOccupyResult(intcomm) == TRUE){
        if(MISSION_CheckResultMissionMine(intcomm, &intcomm->mission) == FALSE){
          return GMEVENT_RES_FINISH;  //�B���҂łȂ��ꍇ�͂����ŏI��
        }
        
        talk->add_white = intcomm->recv_occupy_result.add_white;
        talk->add_black = intcomm->recv_occupy_result.add_black;
        IntrudeEventPrint_SetupFieldMsg(&talk->iem, gsys);

        //�C�x���g���A�G���[�ɂȂ��Ă��\��Ȃ��悤��intcomm����K�v�ȃp�����[�^�������őS�Ď擾
        //  ��BGM��Push,Pop������ׁA�r���ŃC�x���g����߂�ɂ͂�����ӂ̃P�A���K�v�ɂȂ�
        {
          const MISSION_RESULT *mresult = MISSION_GetResultData(&intcomm->mission);
          OCCUPY_INFO *my_occupy = GAMEDATA_GetMyOccupyInfo(gdata);
          
          GF_ASSERT(mresult != NULL);
          talk->mresult = *mresult;
          talk->title_msgid = msg_mistype_000 + mresult->mission_data.cdata.type;
          talk->explain_msgid = mresult->mission_data.cdata.msg_id_contents_monolith;
          
          talk->mission_result = MISSION_CheckResultMissionMine(intcomm, &intcomm->mission);
          talk->point = MISSION_GetResultPoint(intcomm, &intcomm->mission);

          OccupyInfo_LevelUpBlack(my_occupy, talk->add_black);
          OccupyInfo_LevelUpWhite(my_occupy, talk->add_white);
        }
        
        (*seq)++;
      }
      else if(intcomm == NULL || NetErr_App_CheckError()){
        return GMEVENT_RES_FINISH;  //���ʎ�M�҂��̎������G���[���N�����ꍇ�͑��I��
      }
    }
    break;
	  
  case SEQ_INIT:
    MISSIONDATA_Wordset(&talk->mresult.mission_data.cdata, 
      &talk->mresult.mission_data.target_info, talk->iem.wordset, talk->heapID);
    IntrudeEventPrint_SetupExtraMsgWin(&talk->iem, gsys, 1, 4, 32-2, 16);
    IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&talk->iem, talk->title_msgid, 8, 0);
    IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&talk->iem, talk->explain_msgid, 8, 16);
    (*seq)++;
    break;
  case SEQ_RESULT_KEY_WAIT:
    talk->wait++;
    if(talk->wait > RESULT_WINDOW_PRINT_TIME){
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
      
      { //�~�b�V�����N���A�񐔃C���N�������g
        INTRUDE_SAVE_WORK * intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( gdata ) );
        ISC_SAVE_IncMissionClearCount(intsave);
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
  case SEQ_LEVELUP_BLACK_MSG:
    if(talk->add_black > 0){
      WORDSET_RegisterNumber( talk->iem.wordset, 0, talk->add_black, 
        3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_clear_02);
      (*seq)++;
    }
    else{
      *seq = SEQ_LEVELUP_WHITE_MSG;
    }
    break;
  case SEQ_LEVELUP_BLACK_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      (*seq)++;
    }
    break;

  case SEQ_LEVELUP_WHITE_MSG:
    if(talk->add_white > 0){
      WORDSET_RegisterNumber( talk->iem.wordset, 0, talk->add_white, 
        3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      IntrudeEventPrint_StartStream(&talk->iem, msg_invasion_mission_clear_03);
      (*seq)++;
    }
    else{
      *seq = SEQ_POINT_GET_MSG_END_BUTTON_WAIT;
    }
    break;
  case SEQ_LEVELUP_WHITE_MSG_WAIT:
    if(IntrudeEventPrint_WaitStream(&talk->iem) == TRUE){
      *seq = SEQ_POINT_GET_MSG_END_BUTTON_WAIT;
    }
    break;

  case SEQ_POINT_GET_MSG_END_BUTTON_WAIT:
    GameCommInfo_MessageEntry_MissionSuccess(game_comm);
    (*seq) = SEQ_END;
    break;

  case SEQ_MISSION_FAIL:    //�~�b�V�������s
    if( PMSND_CheckPlayBGM() == FALSE ){
      GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_NONE, FSND_FADE_FAST));
      GameCommInfo_MessageEntry_MissionFail(game_comm);
      *seq = SEQ_END;
    }
    break;
    
  case SEQ_END:
  #if 0 //�~�b�V�����I�����ؒf�A�̗���ɂȂ����̂ŏ������͂��Ȃ�(����������Ǝ󒍏o����̂�)
        //2010.04.22(��)
    if(intcomm != NULL){
      MISSION_Init(&intcomm->mission);
    }
  #endif
    
    IntrudeEventPrint_ExitFieldMsg(&talk->iem);
    GMEVENT_ChangeEvent(event, EVENT_IntrudeForceWarpMyPalace(gsys, MISSION_FORCEWARP_MSGID_NULL));
  
    return GMEVENT_RES_CONTINUE;  //ChangeEvent�ŏI�����邽��FINISH���Ȃ�
  }
	return GMEVENT_RES_CONTINUE;
}

