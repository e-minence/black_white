//======================================================================
/*
 * @file	field_comm_event.c
 * @brief	�ʐM��b����
 * @author	ariizumi
 * @data	08/11/19
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "app/trainer_card.h"
#include "field_comm_def.h"
#include "field_comm_main.h"
#include "field_comm_menu.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "field_comm_event.h"
#include "field/event_fieldmap_control.h"	//EVENT_FieldSubProc

#include "msg/msg_d_field.h"

//======================================================================
//	define
//======================================================================

#define TALK_SEQ_COMMON_START (0x40)
//======================================================================
//	enum
//======================================================================
//�b�������鑤�V�[�P���X
enum TALK_PARENT_SEQ
{
	TPS_WAIT_CAN_TALK = 0,	//��b�ł��邩�̖߂�҂�
	TPS_START_TALK,		//��b�J�n

	TPS_SELECT_ACTION_INIT,	//������H�I����
	TPS_SELECT_ACTION_LOOP,	//������H�I����
	TPS_SELECT_ACTION_TERM,
	TPS_WAIT_ACTION_ACCEPT,	//�I�������A�N�V�����̉۔���

	//�ȉ����ʕ������A���ʕ�������̖߂�p
	//�q�Ƌ��ʔԍ��ɂ��邱�ƁI
	TPS_LABEL_START_COMMON = TALK_SEQ_COMMON_START,
	
	TPS_SEND_SYNC_START_ACTION,	//�s���J�n�O�̓���
	TPS_WAIT_SYNC_START_ACTION,	
	TPS_GET_ACTION_DATA_INIT,	//�傫���f�[�^�̂����
	TPS_GET_ACTION_DATA_LOOP,
	TPS_GET_ACTION_DATA_TERM,
	
	TPS_PROC_CHANGE_INIT,		//�p�[�g�ύX����
	TPS_PROC_CHANGE_MAIN,

	TPS_WAIT_RETURN_PROC,
	TPS_RETURN_PROC,	//�p�[�g�ύX����A���Ă�������

	TPS_END_BUTTON_WAIT,
	TPS_END_TALK_INIT,
	TPS_END_TALK,

	TPS_LABEL_END_COMMON,
};
//�b���������鑤�V�[�P���X
enum TALK_CHILD_SEQ
{
	TCS_START_TALK_INIT = 0,
	TCS_START_TALK ,

	TCS_WAIT_SELECT_ACTION,	//�e�I��҂�
	TCS_ACTION_RETURN_LOOP,	//�I���̕Ԏ�
	TCS_ACTION_RETURN_TERM,

	//�ȉ����ʕ������A���ʕ�������̖߂�p
	//�e�Ƌ��ʔԍ��ɂ��邱�ƁI
	
	TCS_LABEL_START_COMMON = TALK_SEQ_COMMON_START,

	TCS_SEND_SYNC_START_ACTION,	//�s���J�n�O�̓���
	TCS_WAIT_SYNC_START_ACTION,	
	TCS_GET_ACTION_DATA_INIT,
	TCS_GET_ACTION_DATA_LOOP,
	TCS_GET_ACTION_DATA_TERM,

	TCS_PROC_CHANGE_INIT,		//�p�[�g�ύX����
	TCS_PROC_CHANGE_MAIN,

	TCS_WAIT_RETURN_PROC,
	TCS_RETURN_PROC,	//�p�[�g�ύX����A���Ă�������

	TCS_END_BUTTON_WAIT,
	TCS_END_TALK_INIT,
	TCS_END_TALK,

	TCS_LABEL_END_COMMON,
};

//======================================================================
//	typedef struct
//======================================================================
struct _FIELD_COMM_EVENT
{
	FIELD_COMM_MAIN *commSys_;
	FIELD_MAIN_WORK *fieldWork_;
	GAMESYS_WORK	*gameSys_;
	GMEVENT			*event_;

	u8	subSeq_;
	F_COMM_ACTION_LIST selectAction_;	//�I�΂ꂽ�s��
	BOOL	isDoAction_;				//�s�����s����?
	BOOL	isInitMenu_;				//�t�B�[���h��Proc��(�����ɂ̓��b�Z�[�W�̏����p
};
//���[�U�[�f�[�^�������p�֐��^�C�v
typedef void(FIELD_COMM_EVENT_USERDATA_INIT_FUNC)( FIELD_COMM_MAIN *commSys );

//======================================================================
//	proto
//======================================================================
const int FIELD_COMM_EVENT_GetWorkSize(void);
void FIELD_COMM_EVENT_SetWorkData( FIELD_COMM_MAIN *commSys , FIELD_MAIN_WORK *fieldWork , FIELD_COMM_EVENT *commEvent );

GMEVENT* FIELD_COMM_EVENT_Start_StartCommNormal( GAMESYS_WORK *gameSys , FIELD_COMM_MAIN *commSys );
GMEVENT_RESULT FIELD_COMM_EVENT_StartCommNormal( GMEVENT *event , int *seq , void *work );
GMEVENT* FIELD_COMM_EVENT_Start_StartCommInvasion( GAMESYS_WORK *gameSys , FIELD_COMM_MAIN *commSys );
GMEVENT_RESULT FIELD_COMM_EVENT_StartCommInvasion( GMEVENT *event , int *seq , void *work );

GMEVENT* FIELD_COMM_EVENT_StartTalk( GAMESYS_WORK *gameSys , FIELD_MAIN_WORK *fieldWork , FIELD_COMM_MAIN *commSys );
static GMEVENT_RESULT FIELD_COMM_EVENT_TalkEvent( GMEVENT *event , int *seq , void *work );
GMEVENT* FIELD_COMM_EVENT_StartTalkPartner( GAMESYS_WORK *gameSys ,FIELD_MAIN_WORK *fieldWork , FIELD_COMM_MAIN *commSys );
static GMEVENT_RESULT FIELD_COMM_EVENT_TalkEventPartner( GMEVENT *event , int *seq , void *work );
static GMEVENT_RESULT FIELD_COMM_EVENT_TalkCommonEvent( GMEVENT *event , int *seq , FIELD_COMM_EVENT *evtWork , BOOL isParent );

static	const BOOL	FIELD_COMM_EVENT_ChangePartFunc( FIELD_COMM_EVENT *evtWork );

static void FIELD_COMM_EVENT_InitUserData_TrainerCard( FIELD_COMM_MAIN *commSys );
static void FIELD_COMM_EVENT_InitUserData_Battle( FIELD_COMM_MAIN *commSys );

extern const int	FIELD_COMM_MAIN_GetWorkSize(void);
extern FIELD_COMM_FUNC* FIELD_COMM_MAIN_GetCommFuncWork( FIELD_COMM_MAIN *commSys );
extern FIELD_COMM_MENU** FIELD_COMM_MAIN_GetCommMenuWork( FIELD_COMM_MAIN *commSys );
extern const HEAPID FIELD_COMM_MAIN_GetHeapID( FIELD_COMM_MAIN *commSys );

const int FIELD_COMM_EVENT_GetWorkSize(void)
{
	return sizeof(FIELD_COMM_EVENT);
}
void FIELD_COMM_EVENT_SetWorkData( FIELD_COMM_MAIN *commSys , FIELD_MAIN_WORK *fieldWork , FIELD_COMM_EVENT *commEvent )
{
	commEvent->commSys_ = commSys;
	commEvent->fieldWork_ = fieldWork;
}

//--------------------------------------------------------------
//	�ʐM�J�n�C�x���g	
//--------------------------------------------------------------
GMEVENT* FIELD_COMM_EVENT_Start_StartCommNormal( GAMESYS_WORK *gameSys , FIELD_COMM_MAIN *commSys )
{
	GF_ASSERT(NULL);
	return NULL;
}
GMEVENT_RESULT FIELD_COMM_EVENT_StartCommNormal( GMEVENT *event , int *seq , void *work )
{
	FIELD_COMM_EVENT *evtWork = work;
	FIELD_COMM_MAIN *commSys = evtWork->commSys_;
	switch( *seq )
	{
	case 0:
		FIELD_COMM_MAIN_InitStartCommMenu( commSys, FIELDMAP_GetFldMsgBG(evtWork->fieldWork_) );
		*seq += 1;
		break;
	case 1:
		if( FIELD_COMM_MAIN_LoopStartCommMenu( commSys ) == TRUE ){
			*seq += 1;
		}
		break;
	case 2:
		FIELD_COMM_MAIN_TermStartCommMenu( commSys );
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
//	�N���J�n�C�x���g	
//--------------------------------------------------------------
GMEVENT* FIELD_COMM_EVENT_Start_StartCommInvasion( GAMESYS_WORK *gameSys , FIELD_COMM_MAIN *commSys )
{
	GF_ASSERT(NULL);
	return NULL;
}
GMEVENT_RESULT FIELD_COMM_EVENT_StartCommInvasion( GMEVENT *event , int *seq , void *work )
{
	FIELD_COMM_EVENT *evtWork = work;
	FIELD_COMM_MAIN *commSys = evtWork->commSys_;
	switch( *seq )
	{
	case 0:
		FIELD_COMM_MAIN_InitStartInvasionMenu( commSys, FIELDMAP_GetFldMsgBG(evtWork->fieldWork_) );
		*seq += 1;
		break;
	case 1:
		if( FIELD_COMM_MAIN_LoopStartInvasionMenu( commSys ) == TRUE ){
			*seq += 1;
		}
		break;
	case 2:
		FIELD_COMM_MAIN_TermStartInvasionMenu( commSys );
		*seq += 1;
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------
//	�b��������C�x���g�J�n	
//--------------------------------------------------------------
GMEVENT* FIELD_COMM_EVENT_StartTalk( GAMESYS_WORK *gameSys ,FIELD_MAIN_WORK *fieldWork , FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_EVENT *evtWork;
	GMEVENT *event;
	event = GMEVENT_Create(gameSys, NULL, FIELD_COMM_EVENT_TalkEvent, sizeof(FIELD_COMM_EVENT));
	evtWork = GMEVENT_GetEventWork(event);
	evtWork->event_ = event;
	evtWork->commSys_ = commSys;
	evtWork->gameSys_ = gameSys;
	evtWork->fieldWork_ = fieldWork;
	evtWork->isInitMenu_ = FALSE;

	//�ŏ��̃p�P�b�g���M�����łɂ��Ă��܂�
	FIELD_COMM_MAIN_StartTalk( commSys );
	return event;
}

//--------------------------------------------------------------
//	�b��������C�x���g�X�V
//--------------------------------------------------------------
static GMEVENT_RESULT FIELD_COMM_EVENT_TalkEvent( GMEVENT *event , int *seq , void *work )
{
	FIELD_COMM_EVENT *evtWork = work;
	FIELD_COMM_MAIN *commSys = evtWork->commSys_;
	FIELD_COMM_FUNC *commFunc = FIELD_COMM_MAIN_GetCommFuncWork( commSys );
	//�������Ő������Ă��̂Ń|�C���^�̃|�C���^�E�E�E
	FIELD_COMM_MENU **commMenu = FIELD_COMM_MAIN_GetCommMenuWork( commSys );
	const F_COMM_TALK_STATE talkState = FIELD_COMM_DATA_GetTalkState( FCD_SELF_INDEX );
	
	//�ی�
	GF_ASSERT_MSG( TPS_LABEL_END_COMMON == TCS_LABEL_END_COMMON , "TALK_xx_SEQ ����Ă�\n");
	
	if( evtWork->isInitMenu_ == TRUE )
	{
		FIELD_COMM_MENU_UpdateMessageWindow( *commMenu );
	}
	switch( *seq )
	{
	case TPS_WAIT_CAN_TALK:
		if( talkState == FCTS_UNPOSSIBLE )
		{
			//�L�����Z�����ꂽ
			FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_NONE );
			return GMEVENT_RES_FINISH;
		}
		else if( talkState == FCTS_ACCEPT )
		{
			//OK
			FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_TALKING );
			
			*commMenu = FIELD_COMM_MENU_InitCommMenu( FIELD_COMM_MAIN_GetHeapID( commSys ), FIELDMAP_GetFldMsgBG(evtWork->fieldWork_) );
			FIELD_COMM_MENU_InitBG_MsgPlane( *commMenu );
			FIELD_COMM_MENU_OpenMessageWindow( FCM_BGPLANE_MSG_WINDOW , *commMenu );
			FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR02 , *commMenu );
			evtWork->isInitMenu_ = TRUE;

			*seq = TPS_START_TALK;
		}
		break;
		//�ڑ��m����̏���
	case TPS_START_TALK:
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
		{
			//�s���I����
			*seq = TPS_SELECT_ACTION_INIT;
			//�I���҂���
//			FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR04 , *commMenu );
//			FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_WAIT_END );
//			*seq += 1;
		}
		break;
	case TPS_SELECT_ACTION_INIT:
		FIELD_COMM_MENU_InitBG_MenuPlane( *commMenu );
		FIELD_COMM_MENU_OpenActionList( FCM_BGPLANE_YESNO_WINDOW , *commMenu );
		FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR08 , *commMenu );
		*seq = TPS_SELECT_ACTION_LOOP;
		break;
		
	case TPS_SELECT_ACTION_LOOP:
		{
			const F_COMM_ACTION_LIST ret = FIELD_COMM_MENU_UpdateActionList( *commMenu );
			switch( ret )
			{
			case FCAL_SELECT:
				//�I��
				break;
			case FCAL_TRAINERCARD:
			case FCAL_BATTLE:
			case FCAL_END:
				evtWork->selectAction_ = ret;
				{
					u8 partnerID;
					FIELD_COMM_FUNC_GetTalkParterData_ID( &partnerID , commFunc ); 
					FIELD_COMM_FUNC_Send_CommonFlg( FCCF_ACTION_SELECT , evtWork->selectAction_ , partnerID );
					*seq = TPS_SELECT_ACTION_TERM;
				}
				break;
			}
		}
		break;
	
	case TPS_SELECT_ACTION_TERM:
		FIELD_COMM_MENU_CloseActionList( *commMenu );
		FIELD_COMM_MENU_TermBG_MenuPlane( *commMenu );
		if( evtWork->selectAction_ == FCAL_END )
		{
			*seq = TPS_END_TALK_INIT;
		}
		else
		{
			*seq = TPS_WAIT_ACTION_ACCEPT;
			FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR07 , *commMenu );
		}
		break;

	case TPS_WAIT_ACTION_ACCEPT:
		{
			const BOOL ret = FIELD_COMM_FUNC_IsActionReturn( commFunc );
			if( ret == TRUE )
			{
				const BOOL isDo = FIELD_COMM_FUNC_GetActionReturn( commFunc );
				if( isDo == TRUE )
				{
					FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR10 , *commMenu );
					*seq = TPS_SEND_SYNC_START_ACTION;
				}
				else
				{
					FIELD_COMM_FUNC_InitCommData_StartTalk( commFunc );
					FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR11 , *commMenu );
					*seq = TPS_START_TALK;
				}
			}
		}
		break;
	
	default:
		//���ʕ����Ƃ��ď���
		return FIELD_COMM_EVENT_TalkCommonEvent( event , seq , evtWork , FALSE );
		break;
	}
	//OS_TPrintf("TALK!\n");
	//return GMEVENT_RES_FINISH;
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
//	�b����������C�x���g�J�n	
//--------------------------------------------------------------
GMEVENT* FIELD_COMM_EVENT_StartTalkPartner( GAMESYS_WORK *gameSys , FIELD_MAIN_WORK *fieldWork ,FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_EVENT *evtWork;
	GMEVENT *event;
	event = GMEVENT_Create(gameSys, NULL, FIELD_COMM_EVENT_TalkEventPartner, sizeof(FIELD_COMM_EVENT));
	evtWork = GMEVENT_GetEventWork(event);
	evtWork->event_ = event;
	evtWork->commSys_ = commSys;
	evtWork->gameSys_ = gameSys;
	evtWork->fieldWork_ = fieldWork;
	evtWork->isInitMenu_ = FALSE;

	FIELD_COMM_MAIN_StartTalkPartner( commSys );
	return event;
}

//--------------------------------------------------------------
//	�b����������C�x���g	
//--------------------------------------------------------------
static GMEVENT_RESULT FIELD_COMM_EVENT_TalkEventPartner( GMEVENT *event , int *seq , void *work )
{
	FIELD_COMM_EVENT *evtWork = work;
	FIELD_COMM_MAIN *commSys = evtWork->commSys_;
	FIELD_COMM_FUNC *commFunc = FIELD_COMM_MAIN_GetCommFuncWork( commSys );
	//�������Ő������Ă��̂Ń|�C���^�̃|�C���^�E�E�E
	FIELD_COMM_MENU **commMenu = FIELD_COMM_MAIN_GetCommMenuWork( commSys );
	
	if( evtWork->isInitMenu_ == TRUE )
	{
		FIELD_COMM_MENU_UpdateMessageWindow( *commMenu );
	}
	switch( *seq )
	{
	case TCS_START_TALK_INIT:
		//OK
		FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_TALKING );
		
		*commMenu = FIELD_COMM_MENU_InitCommMenu( FIELD_COMM_MAIN_GetHeapID( commSys ), FIELDMAP_GetFldMsgBG(evtWork->fieldWork_) );
		FIELD_COMM_MENU_InitBG_MsgPlane( *commMenu );
		FIELD_COMM_MENU_OpenMessageWindow( FCM_BGPLANE_MSG_WINDOW , *commMenu );
		FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR03 , *commMenu );
		evtWork->isInitMenu_ = TRUE;
		*seq = TCS_START_TALK;
		break;

		//�ڑ��m����̏���
	case TCS_START_TALK:
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
		{
			*seq = TCS_WAIT_SELECT_ACTION;
			FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR07 , *commMenu );

			//�I���҂���
			//FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR04 , *commMenu );
			//FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_WAIT_END );
			//*seq += 1;
		}
		break;

	case TCS_WAIT_SELECT_ACTION:	//�e�I��҂�
		{
			const F_COMM_ACTION_LIST selAct = FIELD_COMM_FUNC_GetSelectAction( commFunc );
			if( selAct == FCAL_END )
			{
				FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR09 , *commMenu );
				*seq = TCS_END_BUTTON_WAIT;
			}
			else
			if( selAct != FCAL_SELECT )
			{
				//�A�N�V�����ɑ΂���Ԏ��̃��b�Z�[�W��ID
				static const u16 actionReturnMsgID[FCAL_MAX] =
				{
					DEBUG_FIELD_C_STR05,	//�J�[�h
					DEBUG_FIELD_C_STR06,	//�o�g��
					DEBUG_FIELD_C_STR09,	//�I��
				};
				evtWork->selectAction_ = selAct;
				FIELD_COMM_MENU_InitBG_MenuPlane( *commMenu );
				FIELD_COMM_MENU_OpenYesNoMenu( FCM_BGPLANE_YESNO_WINDOW , *commMenu );
				FIELD_COMM_MENU_SetMessage( actionReturnMsgID[selAct] , *commMenu );
				*seq = TCS_ACTION_RETURN_LOOP;
			}
		}
		break;

	case TCS_ACTION_RETURN_LOOP:	//�I���̕Ԏ�
		{
			const u8 ret = FIELD_COMM_MENU_UpdateYesNoMenu( *commMenu );
			if( ret != YNR_WAIT )
			{
				u8 partnerID;
				FIELD_COMM_FUNC_GetTalkParterData_ID( &partnerID , commFunc ); 
				if( ret == YNR_YES )
				{
					FIELD_COMM_FUNC_Send_CommonFlg( FCCF_ACTION_RETURN , TRUE , partnerID );
					evtWork->isDoAction_ = TRUE;
				}
				else
				{
					FIELD_COMM_FUNC_Send_CommonFlg( FCCF_ACTION_RETURN , FALSE , partnerID );
					evtWork->isDoAction_ = FALSE;
				}
				FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR10 , *commMenu );
				*seq = TCS_ACTION_RETURN_TERM;
			}
		}
		break;

	case TCS_ACTION_RETURN_TERM:
		FIELD_COMM_MENU_CloseYesNoMenu( *commMenu );
		FIELD_COMM_MENU_TermBG_MenuPlane( *commMenu );
		if( evtWork->isDoAction_ == TRUE )
		{
			*seq = TCS_SEND_SYNC_START_ACTION;
		}
		else
		{
			*seq = TCS_WAIT_SELECT_ACTION;
			FIELD_COMM_FUNC_InitCommData_StartTalk( commFunc );
			FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR07 , *commMenu );
		}
		break;

	default:
		//���ʕ����Ƃ��ď���
		return FIELD_COMM_EVENT_TalkCommonEvent( event , seq , evtWork , FALSE );
		break;
	}
	//OS_TPrintf("TALKED!\n");
	//return GMEVENT_RES_FINISH;
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
//	�b���E�b���������鋤�ʃC�x���g
//	seq�̒l�� TALK_SEQ_COMMON_START(0x40) ����̋��ʂ̒l�ɂ��邱�ƁI
//--------------------------------------------------------------
//�s���ɑΉ��������[�U�[�f�[�^�������֐��Q
FIELD_COMM_EVENT_USERDATA_INIT_FUNC *userDataInitFunc[FCAL_MAX] =
{
	FIELD_COMM_EVENT_InitUserData_TrainerCard,
	FIELD_COMM_EVENT_InitUserData_Battle,
	NULL,
};
static GMEVENT_RESULT FIELD_COMM_EVENT_TalkCommonEvent( GMEVENT *event , int *seq , FIELD_COMM_EVENT *evtWork , BOOL isParent )
{
	FIELD_COMM_MAIN *commSys = evtWork->commSys_;
	FIELD_COMM_FUNC *commFunc = FIELD_COMM_MAIN_GetCommFuncWork( commSys );
	//�������Ő������Ă��̂Ń|�C���^�̃|�C���^�E�E�E
	FIELD_COMM_MENU **commMenu = FIELD_COMM_MAIN_GetCommMenuWork( commSys );
	GF_ASSERT( *seq >= TALK_SEQ_COMMON_START );
	switch( *seq )
	{
	case TPS_SEND_SYNC_START_ACTION:	//�s���J�n�O�̓���+�f�[�^������
		userDataInitFunc[evtWork->selectAction_](commSys);

		FIELD_COMM_FUNC_Send_SyncCommand( FCST_START_INIT_ACTION , commFunc );
		*seq = TPS_WAIT_SYNC_START_ACTION;
		break;
	case TPS_WAIT_SYNC_START_ACTION:
		{
			const u8 talkMemberBit = FIELD_COMM_FUNC_GetBit_TalkMember( commFunc );
			if( FIELD_COMM_FUNC_CheckSyncFinish( FCST_START_INIT_ACTION , talkMemberBit , commFunc ) == TRUE )
			{
				*seq = TPS_GET_ACTION_DATA_INIT;
			}
		}
		break;

	case TPS_GET_ACTION_DATA_INIT:	//���[�U�[�f�[�^�̂����
		{
			const F_COMM_USERDATA_TYPE type = FIELD_COMM_DATA_GetUserDataType_From_Action( evtWork->selectAction_ );
			u8 partnerID;
			FIELD_COMM_FUNC_GetTalkParterData_ID( &partnerID , commFunc );

			if( FIELD_COMM_FUNC_Send_UserData( type , partnerID ) == TRUE )
			{
				FIELD_COMM_FUNC_ResetGetUserData( commFunc );
				*seq = TPS_GET_ACTION_DATA_LOOP;
			}
		}
		break;

	case TPS_GET_ACTION_DATA_LOOP:
		if( FIELD_COMM_FUNC_IsGetUserData( commFunc ) == TRUE )
		{
			*seq = TPS_GET_ACTION_DATA_TERM;
		}
		break;

	case TPS_GET_ACTION_DATA_TERM:
		FIELD_COMM_MENU_CloseMessageWindow( *commMenu );
		FIELD_COMM_MENU_TermBG_MsgPlane( *commMenu );
		FIELD_COMM_MENU_TermCommMenu( *commMenu );
		evtWork->isInitMenu_ = FALSE;

		FIELD_COMM_FUNC_Send_SyncCommand( FCST_START_CHANGE_PROC , commFunc );
		*seq = TPS_PROC_CHANGE_INIT;
		break;

	case TPS_PROC_CHANGE_INIT:
		{
			const u8 talkMemberBit = FIELD_COMM_FUNC_GetBit_TalkMember( commFunc );
			if( FIELD_COMM_FUNC_CheckSyncFinish( FCST_START_CHANGE_PROC , talkMemberBit , commFunc ) )
			{
				*seq = TPS_PROC_CHANGE_MAIN;
				evtWork->subSeq_ = 0;
			}
		}
		break;

	case TPS_PROC_CHANGE_MAIN:
		{
			const BOOL ret = FIELD_COMM_EVENT_ChangePartFunc( evtWork );
			if( ret == TRUE )
			{
				*commMenu = FIELD_COMM_MENU_InitCommMenu( FIELD_COMM_MAIN_GetHeapID( commSys ), FIELDMAP_GetFldMsgBG(evtWork->fieldWork_) );
				FIELD_COMM_MENU_InitBG_MsgPlane( *commMenu );
				FIELD_COMM_MENU_OpenMessageWindow( FCM_BGPLANE_MSG_WINDOW , *commMenu );
				FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR10 , *commMenu );
				evtWork->isInitMenu_ = TRUE;

				//���[�U�[�f�[�^�̊J��
				FIELD_COMM_DATA_DeleteUserData();
				
				FIELD_COMM_FUNC_Send_SyncCommand( FCST_WAIT_RETURN_PROC , commFunc );
				*seq = TPS_WAIT_RETURN_PROC;
			}
		}
		break;

	case TPS_WAIT_RETURN_PROC:
		{
			const u8 talkMemberBit = FIELD_COMM_FUNC_GetBit_TalkMember( commFunc );
			if( FIELD_COMM_FUNC_CheckSyncFinish( FCST_WAIT_RETURN_PROC , talkMemberBit , commFunc ) == TRUE )
			{	
				*seq = TPS_RETURN_PROC;
			}
		}
		break;
	case TPS_RETURN_PROC:	//�p�[�g�ύX����A���Ă�������
		FIELD_COMM_FUNC_InitCommData_StartTalk( commFunc );
		if( isParent == TRUE )
		{
			//�e�@�s���I����
			*seq = TPS_SELECT_ACTION_INIT;
		}
		else
		{
			//�q�@�s���I��҂�
			*seq = TCS_WAIT_SELECT_ACTION;
			FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR07 , *commMenu );
		}
		break;

	case TPS_END_BUTTON_WAIT:
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
		{
			*seq = TPS_END_TALK_INIT;
		}
		break;
	case TPS_END_TALK_INIT:
		FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_WAIT_END );
		FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR04 , *commMenu );
		*seq = TPS_END_TALK;
		break;
	case TPS_END_TALK:
		{
			u8 partnerID;
			FIELD_COMM_FUNC_GetTalkParterData_ID( &partnerID , commFunc );
			if( FIELD_COMM_DATA_GetTalkState( partnerID ) == FCTS_WAIT_END )
			{
				FIELD_COMM_MENU_CloseMessageWindow( *commMenu );
				FIELD_COMM_MENU_TermBG_MsgPlane( *commMenu );
				FIELD_COMM_MENU_TermCommMenu( *commMenu );
				FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_NONE );
				evtWork->isInitMenu_ = FALSE;
				return GMEVENT_RES_FINISH;
			}
		}
		break;
	}
	return GMEVENT_RES_CONTINUE;
}
//--------------------------------------------------------------
//	�p�[�g�؂�ւ�
//--------------------------------------------------------------
extern const GFL_PROC_DATA DebugSogabeMainProcData;
static	const BOOL	FIELD_COMM_EVENT_ChangePartFunc( FIELD_COMM_EVENT *evtWork )
{
	switch( evtWork->subSeq_ )
	{
	case 0:
		{
			GMEVENT * newEvent;
			if( evtWork->selectAction_ == FCAL_TRAINERCARD )
			{
				newEvent = EVENT_FieldSubProc(evtWork->gameSys_, evtWork->fieldWork_,
							TRCARD_OVERLAY_ID, &TrCardSysCommProcData, 
							FIELD_COMM_DATA_GetPartnerUserData(FCUT_TRAINERCARD) );
			}
			else if( evtWork->selectAction_ == FCAL_BATTLE )
			{
				newEvent = EVENT_FieldSubProc(evtWork->gameSys_, evtWork->fieldWork_,
							NO_OVERLAY_ID, &DebugSogabeMainProcData, 
							NULL );
			}
			GMEVENT_CallEvent( evtWork->event_, newEvent);
			evtWork->subSeq_++;
		}
		break;
	case 1:
//		if( GFL_FADE_CheckFade() == FALSE )
		{
			evtWork->subSeq_++;
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//���[�U�[�f�[�^�������p�֐�(��
static void FIELD_COMM_EVENT_InitUserData_TrainerCard( FIELD_COMM_MAIN *commSys )
{
	STRBUF *workStr;
	TR_CARD_DATA *cardData;
	FIELD_COMM_DATA_CreateUserData( FCUT_TRAINERCARD );
	cardData = FIELD_COMM_DATA_GetSelfUserData( FCUT_TRAINERCARD );
	
	TRAINERCARD_GetSelfData( cardData , TRUE );
}
static void FIELD_COMM_EVENT_InitUserData_Battle( FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_DATA_CreateUserData( FCUT_BATTLE );
}

