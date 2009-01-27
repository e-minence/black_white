//======================================================================
/**
 *
 * @file	event_fieldtalk.c
 * @brief	�t�B�[���h�b���|���C�x���g
 * @author	kagaya
 * @data	2009.01.22
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "event_fieldtalk.h"

#include "message.naix"
#include "msg/msg_d_field.h"	//��

extern FLDMMDLSYS * FIELDMAP_GetFldMMdlSys( FIELD_MAIN_WORK *fieldWork );

//======================================================================
//	define
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	FLDTALK_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
	HEAPID heapID;
	u32 scr_id;
	FLDMMDL *fmmdl_player;
	FLDMMDL *fmmdl_talk;
	FIELD_MAIN_WORK *fieldWork;

	FLDMSGBG *msgBG;
	GFL_MSGDATA *msgData;
	FLDMSGWIN *msgWin;
}FLDTALK_EVENT_WORK;

//======================================================================
//	proto
//======================================================================
static GMEVENT_RESULT FldTalkEvent( GMEVENT *event, int *seq, void *wk );

//======================================================================
//	�C�x���g�F�t�B�[���h�b���|���C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�b���|���C�x���g�N��
 * @param	GAMESYS_WORK
 * @param	fieldWork		FIELD_MAIN_WORK
 * @param	scr_id			�X�N���v�gID
 * @param	fmmdl_player	���@���샂�f��
 * @param	fmmdl_talk		�b���|���Ώۓ��샂�f��
 * @param	heapID		HEAPID
 * @retval	GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_FieldTalk(
		GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, u32 scr_id,
		FLDMMDL *fmmdl_player, FLDMMDL *fmmdl_talk, HEAPID heapID )
{
	FLDTALK_EVENT_WORK *ftalk_wk;
	GMEVENT *event;
	
	event = GMEVENT_Create(
		gsys, NULL,	FldTalkEvent, sizeof(FLDTALK_EVENT_WORK) );
	ftalk_wk = GMEVENT_GetEventWork( event );
	MI_CpuClear8( ftalk_wk, sizeof(FLDTALK_EVENT_WORK) );
	
	ftalk_wk->scr_id = scr_id;
	ftalk_wk->heapID = heapID;
	ftalk_wk->msgBG = FIELDMAP_GetFLDMSGBG( fieldWork );
	ftalk_wk->fmmdl_player = fmmdl_player;
	ftalk_wk->fmmdl_talk = fmmdl_talk;
	ftalk_wk->fieldWork = fieldWork;
	
	return( event );
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�b���|���C�x���g
 * @param	event	GMEVENT
 * @param	seq		�V�[�P���X
 * @param	wk		event work
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FldTalkEvent( GMEVENT *event, int *seq, void *wk )
{
	FLDTALK_EVENT_WORK *work = wk;
	
	switch( (*seq) ){
	case 0:	//�Ώ�MDL�̈ړ��I���҂�
		if( work->fmmdl_talk != NULL ){
			if( FLDMMDL_StatusBitCheck_Move(work->fmmdl_talk) == TRUE ){
				FLDMMDL_Move( work->fmmdl_talk );
				
				if( FLDMMDL_StatusBitCheck_Move(work->fmmdl_talk) == TRUE ){
					break;
				}
			}
		}
		
		{
			FLDMMDLSYS *fmmdlsys = FIELDMAP_GetFldMMdlSys( work->fieldWork );
			FLDMMDLSYS_MovePauseAll( fmmdlsys );
		}
		(*seq)++;
	case 1:
		work->msgData = FLDMSGBG_CreateMSGDATA(
			work->msgBG, NARC_message_d_field_dat );
		work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
		FLDMSGWIN_Print( work->msgWin, 0, 0, DEBUG_FIELD_STR01 );
		(*seq)++;
		break;
	case 2:
		if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
			(*seq)++;
		}
		break;
	case 3:
		{
			int trg = GFL_UI_KEY_GetTrg();
			if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				(*seq)++;
			}
		}
		break;
	case 4:
		FLDMSGWIN_Delete( work->msgWin );
		GFL_MSG_Delete( work->msgData );

		{
			FLDMMDLSYS *fmmdlsys = FIELDMAP_GetFldMMdlSys( work->fieldWork );
			FLDMMDLSYS_MovePauseAllClear( fmmdlsys );
		}
		return( GMEVENT_RES_FINISH );
	}
	
	return( GMEVENT_RES_CONTINUE );
}

