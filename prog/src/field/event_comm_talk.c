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
#include "msg/msg_intrude_comm.h"	//��

#include "script.h"
#include "event_fieldtalk.h"

#include "event_comm_talk.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_comm_command.h"
#include "field/field_comm/bingo_system.h"


#include "../../../resource/fldmapdata/script/common_scr_def.h"

extern MMDLSYS * FIELDMAP_GetMMdlSys( FIELDMAP_WORK *fieldWork );

//======================================================================
//	define
//======================================================================

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
  
	FLDMSGBG *msgBG;
	GFL_MSGDATA *msgData;
	FLDMSGWIN *msgWin;
}COMMTALK_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT CommTalkEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT_RESULT CommBingoEvent( GMEVENT *event, int *seq, void *wk );



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
	GMEVENT *event;
	
	if(intcomm->intrude_status[talk_net_id].action_status == INTRUDE_ACTION_BINGO_BATTLE){
  	event = GMEVENT_Create(
  		gsys, NULL,	CommBingoEvent, sizeof(COMMTALK_EVENT_WORK) );
    Bingo_Clear_BingoBattleBeforeBuffer(Bingo_GetBingoSystemWork(intcomm));
  }
  else{
  	event = GMEVENT_Create(
  		gsys, NULL,	CommTalkEvent, sizeof(COMMTALK_EVENT_WORK) );
  }
	ftalk_wk = GMEVENT_GetEventWork( event );
	GFL_STD_MemClear( ftalk_wk, sizeof(COMMTALK_EVENT_WORK) );
	
	ftalk_wk->heapID = heap_id;
	ftalk_wk->msgBG = FIELDMAP_GetFldMsgBG( fieldWork );
	ftalk_wk->fmmdl_player = fmmdl_player;
	ftalk_wk->fmmdl_talk = CommPlayer_GetMmdl(intcomm->cps, talk_net_id);
	ftalk_wk->fieldWork = fieldWork;
	ftalk_wk->intcomm = intcomm;
	
	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
	Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_TALK);
  Intrude_InitTalkWork(intcomm, talk_net_id);
	
	return( event );
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
	
	switch( *seq ){
	case 0:	//�Ώ�MDL�̈ړ��I���҂�
		if( talk->fmmdl_talk != NULL ){
			if( MMDL_CheckStatusBitMove(talk->fmmdl_talk) == TRUE ){
				MMDL_UpdateMove( talk->fmmdl_talk );
				
				if( MMDL_CheckStatusBitMove(talk->fmmdl_talk) == TRUE ){
					break;
				}
			}
		}
		
		{
			MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( talk->fieldWork );
			MMDLSYS_PauseMoveProc( fmmdlsys );
		}
		(*seq)++;
	case 1:
		talk->msgData = FLDMSGBG_CreateMSGDATA(
			talk->msgBG, NARC_message_intrude_comm_dat );
		talk->msgWin = FLDMSGWIN_AddTalkWin( talk->msgBG, talk->msgData );
		FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_000 );
		(*seq)++;
		break;
	case 2:
	  if(IntrudeSend_Talk(talk->intcomm->talk.talk_netid) == TRUE){
      (*seq)++;
    }
	case 3:
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 4:
	  {
      INTRUDE_TALK_STATUS talk_status = Intrude_GetTalkAnswer(talk->intcomm);
      switch(talk_status){
      case INTRUDE_TALK_STATUS_OK:
        *seq = 100;
        break;
      case INTRUDE_TALK_STATUS_NG:
        *seq = 200;
        break;
      }
    }
		break;
	
  case 100:   //�Ԏ�OK
    FLDMSGWIN_ClearWindow(talk->msgWin);
		FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_003 );
		(*seq)++;
    break;
  case 101:
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 102:
		{
			int trg = GFL_UI_KEY_GetTrg();
			if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				(*seq) = 300;
			}
		}
	  break;
	  
  case 200:   //�f��ꂽ
    FLDMSGWIN_ClearWindow(talk->msgWin);
		FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_002 );
		(*seq)++;
    break;
  case 201:
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 202:
		{
			int trg = GFL_UI_KEY_GetTrg();
			if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				(*seq) = 300;
			}
		}
	  break;
	
	case 300:   //�I������
		FLDMSGWIN_Delete( talk->msgWin );
		GFL_MSG_Delete( talk->msgData );

		{
			MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( talk->fieldWork );
			MMDLSYS_ClearPauseMoveProc( fmmdlsys );
		}

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
	case 0:	//�Ώ�MDL�̈ړ��I���҂�
		if( talk->fmmdl_talk != NULL ){
			if( MMDL_CheckStatusBitMove(talk->fmmdl_talk) == TRUE ){
				MMDL_UpdateMove( talk->fmmdl_talk );
				
				if( MMDL_CheckStatusBitMove(talk->fmmdl_talk) == TRUE ){
					break;
				}
			}
		}
		
		{
			MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( talk->fieldWork );
			MMDLSYS_PauseMoveProc( fmmdlsys );
		}
		(*seq)++;
	case 1:
		talk->msgData = FLDMSGBG_CreateMSGDATA(
			talk->msgBG, NARC_message_intrude_comm_dat );
		talk->msgWin = FLDMSGWIN_AddTalkWin( talk->msgBG, talk->msgData );
		FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_bingo000 );
		(*seq)++;
		break;
	case 2:
	  if(IntrudeSend_BingoIntrusion(talk->intcomm->talk.talk_netid) == TRUE){
      (*seq)++;
    }
	case 3:
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 4:
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
    FLDMSGWIN_ClearWindow(talk->msgWin);
   	FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_bingo001 );
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
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 104:
		{
			int trg = GFL_UI_KEY_GetTrg();
			if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				(*seq) = 300;
			}
		}
	  break;
	  
  case 200:   //�f��ꂽ
    FLDMSGWIN_ClearWindow(talk->msgWin);
		FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_bingo002 );
		(*seq)++;
    break;
  case 201:
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 202:
		{
			int trg = GFL_UI_KEY_GetTrg();
			if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				(*seq) = 300;
			}
		}
	  break;
	
	case 300:   //�I������
		FLDMSGWIN_Delete( talk->msgWin );
		GFL_MSG_Delete( talk->msgData );

		{
			MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( talk->fieldWork );
			MMDLSYS_ClearPauseMoveProc( fmmdlsys );
		}

  	//�N���V�X�e���̃A�N�V�����X�e�[�^�X���X�V
  	Intrude_SetActionStatus(talk->intcomm, INTRUDE_ACTION_FIELD);
    Intrude_InitTalkWork(talk->intcomm, INTRUDE_NETID_NULL);
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

