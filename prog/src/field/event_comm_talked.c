//==============================================================================
/**
 * @file    event_comm_talked.c
 * @brief   �ʐM�v���C���[����b��������ꂽ
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

#include "event_comm_talked.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_comm_command.h"


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
static GMEVENT_RESULT CommWasTalkedTo( GMEVENT *event, int *seq, void *wk );



//======================================================================
//	�C�x���g�F�t�B�[���h�b���|���C�x���g
//======================================================================
//==================================================================
/**
 * �ʐM�v���C���[����b��������ꂽ�C�x���g�N��
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   intcomm		      �N���V�X�e�����[�N�ւ̃|�C���^
 * @param   fmmdl_player		���@���샂�f��
 * @param   talk_net_id		  �b�������Ă����Ώۂ�NetID
 * @param   heap_id		      �q�[�vID
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommWasTalkedTo(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id)
{
	COMMTALK_EVENT_WORK *ftalk_wk;
	GMEVENT *event;
	
	event = GMEVENT_Create(
		gsys, NULL,	CommWasTalkedTo, sizeof(COMMTALK_EVENT_WORK) );
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
static GMEVENT_RESULT CommWasTalkedTo( GMEVENT *event, int *seq, void *wk )
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
		FLDMSGWIN_Print( talk->msgWin, 0, 0, msg_intrude_001 );
		(*seq)++;
		break;
	case 2:
	  if(IntrudeSend_TalkAnswer(
	      talk->intcomm, talk->intcomm->talk.talk_netid, talk->intcomm->talk.talk_status) == TRUE){
      (*seq)++;
    }
    break;
  case 3:
		if( FLDMSGWIN_CheckPrintTrans(talk->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 4:
		{
			int trg = GFL_UI_KEY_GetTrg();
			if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				(*seq)++;
			}
		}
		break;
	case 5:
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

