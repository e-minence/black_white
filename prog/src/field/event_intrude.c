//==============================================================================
/**
 * @file    event_intrude.c
 * @brief   �N���F�C�x���g
 * @author  matsuda
 * @date    2009.12.15(��)
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
#include "event_intrude.h"
#include "field_comm\intrude_field.h"

#include "../../../resource/fldmapdata/script/common_scr_def.h"

#include "poke_tool/status_rcv.h"


//==============================================================================
//  �萔��`
//==============================================================================
enum{
  DISGUISE_ANM_WAIT_MIN = 1,
  DISGUISE_ANM_WAIT_MAX = 15,
};


//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	DISGUISE_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	HEAPID heapID;
	FIELDMAP_WORK *fieldWork;
  
	INTRUDE_EVENT_MSGWORK iem;
	
	MISSION_DATA mdata;
	INTRUDE_EVENT_DISGUISE_WORK iedw;
	
	u8 mission_view;
	u8 padding[3];
}DISGUISE_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT DisguiseEvent( GMEVENT *event, int *seq, void *wk );


//======================================================================
//	�C�x���g�F�v���C���[�ϐg�C�x���g
//======================================================================
//==================================================================
/**
 * �v���C���[�ϐg�C�x���g�N��
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
GMEVENT * EVENT_Disguise(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, HEAPID heap_id)
{
	DISGUISE_EVENT_WORK *dis_wk;
	GMEVENT *event = NULL;
  const MISSION_DATA *mdata = MISSION_GetRecvData(&intcomm->mission);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
	
	event = GMEVENT_Create( gsys, NULL, DisguiseEvent, sizeof(DISGUISE_EVENT_WORK) );
  
	dis_wk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( dis_wk, sizeof(DISGUISE_EVENT_WORK) );
	
	dis_wk->heapID = heap_id;
	dis_wk->fieldWork = fieldWork;
	dis_wk->mdata = *mdata;
#if 0
	if(mdata->accept_netid != GAMEDATA_GetIntrudeMyID(gamedata)){
    //�󒍂����̂������Ŗ����Ȃ�A�܂��~�b�V�������e��\��
    dis_wk->mission_view = TRUE;
  }
#else //�N�ł��\���ɕύX�@2010.02.01(��)
  dis_wk->mission_view = TRUE;
#endif

  IntrudeEventPrint_SetupFieldMsg(&dis_wk->iem, gsys);

	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_EFFECT);

	return( event );
}

//--------------------------------------------------------------
/**
 * �v���C���[�ϐg�C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event dis_wk
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DisguiseEvent( GMEVENT *event, int *seq, void *wk )
{
	DISGUISE_EVENT_WORK *dis_wk = wk;
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	INTRUDE_COMM_SYS_PTR intcomm;
  enum{
  	SEQ_MISSION_VIEW,
    SEQ_MISSION_VIEW_WAIT,
    SEQ_DISGUISE_START,
    SEQ_DISGUISE_MAIN,
  	SEQ_FINISH,
  };
	
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(intcomm == NULL){
    if((*seq) == SEQ_MISSION_VIEW){ //�N���e�B�J���ȃ^�C�~���O������FINISH�֔�΂�
      *seq = SEQ_FINISH;
    }
  }

	switch( *seq ){
	case SEQ_MISSION_VIEW:
	  if(dis_wk->mission_view == FALSE){
      *seq = SEQ_DISGUISE_START;
      break;
    }
    
    IntrudeEventPrint_SetupExtraMsgWin(&dis_wk->iem, gsys, 1, 1, 32-2, 16);
    
    MISSIONDATA_Wordset(intcomm, &dis_wk->mdata, dis_wk->iem.wordset, dis_wk->heapID);
    {
      u16 explain_msgid, title_msgid;
      
      title_msgid = msg_mistype_000 + dis_wk->mdata.cdata.type;
      explain_msgid = dis_wk->mdata.cdata.msg_id_contents_monolith;
      
      IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&dis_wk->iem, title_msgid, 8, 0);
      IntrudeEventPrint_PrintExtraMsgWin_MissionMono(&dis_wk->iem, explain_msgid, 8, 16);
    }
    
    (*seq)++;
    break;
  case SEQ_MISSION_VIEW_WAIT:
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)){
      IntrudeEventPrint_ExitExtraMsgWin(&dis_wk->iem);
      (*seq)++;
    }
    break;
  
  case SEQ_DISGUISE_START:
    {
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
      
      IntrudeEvent_Sub_DisguiseEffectSetup(&dis_wk->iedw, gsys, dis_wk->fieldWork, 
        dis_wk->mdata.cdata.obj_id[GAMEDATA_GetIntrudeMyID(gamedata)]);
      (*seq)++;
    }
    break;
  case SEQ_DISGUISE_MAIN:
    if(IntrudeEvent_Sub_DisguiseEffectMain(&dis_wk->iedw, intcomm) == TRUE){
      (*seq)++;
    }
    break;
	  
	case SEQ_FINISH:   //�I������
    IntrudeEventPrint_ExitFieldMsg(&dis_wk->iem);

  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	if(intcomm != NULL){
    	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_FIELD);
    }
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

//==================================================================
/**
 * �ϑ����o�F�Z�b�g�A�b�v
 *
 * @param   iedw		
 * @param   fieldWork		
 * @param   disguise_code		�ϑ�����OBJCODE
 *                  (DISGUISE_NO_NULL�̏ꍇ�͒ʏ�̎p�ADISGUISE_NO_NORMAL�̏ꍇ�̓p���X���̕W���p)
 */
//==================================================================
void IntrudeEvent_Sub_DisguiseEffectSetup(INTRUDE_EVENT_DISGUISE_WORK *iedw, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 disguise_code)
{
  GFL_STD_MemClear(iedw, sizeof(INTRUDE_EVENT_DISGUISE_WORK));
  iedw->gsys = gsys;
  iedw->fieldWork = fieldWork;
  iedw->disguise_code =disguise_code;
  iedw->wait_max = DISGUISE_ANM_WAIT_MAX;
  iedw->wait = iedw->wait_max;
}

//==================================================================
/**
 * �ϑ����o�F���C���V�[�P���X
 *
 * @param   iedw		
 * @param   intcomm ������NULL�`�F�b�N�͂���ׁANULL�ł����Ă����̊֐����Ăё����Ă�������
 *
 * @retval  BOOL		TRUE:�V�[�P���X�I���@FALSE:�V�[�P���X�p����
 */
//==================================================================
BOOL IntrudeEvent_Sub_DisguiseEffectMain(INTRUDE_EVENT_DISGUISE_WORK *iedw, INTRUDE_COMM_SYS_PTR intcomm)
{
  switch(iedw->seq){
  case 0:
    iedw->wait_max = DISGUISE_ANM_WAIT_MAX;
    iedw->wait = iedw->wait_max;
    iedw->seq++;
    break;
  case 1:
    {
      FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(iedw->fieldWork);
      MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);
      static const u8 player_anm_tbl[] = {
        AC_DIR_U, AC_DIR_L, AC_DIR_D, AC_DIR_R,
      };
      
      if(MMDL_CheckPossibleAcmd(player_mmdl) == TRUE){
        if(iedw->wait_max >= DISGUISE_ANM_WAIT_MAX 
            && iedw->loop > 0
            && player_anm_tbl[iedw->anm_no] == AC_DIR_R){
          iedw->seq++;
          break;
        }
        MMDL_SetAcmd(player_mmdl, player_anm_tbl[iedw->anm_no]);
        iedw->anm_no = (iedw->anm_no + 1) % NELEMS(player_anm_tbl);
        iedw->wait = iedw->wait_max;
        if(iedw->loop == 0){
          iedw->wait_max--;
          if(iedw->wait_max <= DISGUISE_ANM_WAIT_MIN){
            GAMEDATA *gamedata = GAMESYSTEM_GetGameData(iedw->gsys);
            if(intcomm != NULL){
              IntrudeField_PlayerDisguise(intcomm, iedw->gsys, iedw->disguise_code);
            }
            iedw->loop++;
          }
        }
        else{
          if(iedw->wait_max < DISGUISE_ANM_WAIT_MAX){
            iedw->wait_max++;
          }
        }
      }
    }
    break;
  default:
    return TRUE;
  }
  
  return FALSE;
}
