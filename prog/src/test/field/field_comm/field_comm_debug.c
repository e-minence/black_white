//======================================================================
/**
 * @file	field_comm_debug.c
 * @brief	�t�B�[���h�ʐM�f�o�b�O�@�q���j���[
 * @author	ariizumi
 * @data	08/11/21
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "../event_debug_menu.h"
#include "field_comm_debug.h"
#include "field_comm_event.h"

#include "arc_def.h"
#include "message.naix"
#include "msg/msg_d_field.h"
//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
typedef const BOOL(*FIELD_COMM_DEBUG_CALLBACK)(FIELD_COMM_DEBUG_WORK* work);

struct _FIELD_COMM_DEBUG_WORK
{
	HEAPID heapID_;
	FIELD_MAIN_WORK *fieldWork_;
	GMEVENT *event_;

	BMP_MENULIST_DATA	*menuListData_;
	DMENU_COMMON_WORK	*debMenuWork_;

	FIELD_COMM_DEBUG_CALLBACK callback_;
};

//======================================================================
//	proto
//======================================================================
const int FIELD_COMM_DEBUG_GetWorkSize(void);
void	FIELD_COMM_DEBUG_InitWork( const HEAPID heapID , FIELD_MAIN_WORK *fieldWork , GMEVENT *event , FIELD_COMM_DEBUG_WORK *commDeb );
GMEVENT_RESULT FIELD_COMM_DEBUG_CommDebugMenu( GMEVENT *event , int *seq , void *work );

static	const BOOL	FIELD_COMM_DEBUG_MenuCallback_StartComm( FIELD_COMM_DEBUG_WORK *work );
static	const BOOL	FIELD_COMM_DEBUG_MenuCallback_StartInvasion(FIELD_COMM_DEBUG_WORK *work  );
//--------------------------------------------------------------
//	���[�N�T�C�Y�擾
//--------------------------------------------------------------
const int FIELD_COMM_DEBUG_GetWorkSize(void)
{
	return sizeof( FIELD_COMM_DEBUG_WORK );
}
//--------------------------------------------------------------
//	���[�N������
//--------------------------------------------------------------
void	FIELD_COMM_DEBUG_InitWork( const HEAPID heapID , FIELD_MAIN_WORK *fieldWork , GMEVENT *event , FIELD_COMM_DEBUG_WORK *commDeb )
{
	commDeb->heapID_ = heapID;
	commDeb->fieldWork_ = fieldWork;
	commDeb->event_ = event;
}

//--------------------------------------------------------------
//	�C�x���g���C��
//--------------------------------------------------------------
///	���j���[�w�b�_�[
static const BMPMENULIST_HEADER FieldCommDebugMenuHeader =
{
	NULL,	//�\�������f�[�^�|�C���^
	NULL,	//�J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
	NULL,	//���\�����Ƃ̃R�[���o�b�N�֐�
	NULL,	//GFL_BMPWIN

	1,		//���X�g���ڐ�
	12,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	11,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	BMPMENULIST_LRKEY_SKIP,	//�y�[�W�X�L�b�v�^�C�v
	0,		//�����w��(�{����u8�����ǂ���Ȃɍ��Ȃ��Ǝv���̂�)
	0,		//�a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
	
	NULL,	//���[�N�|�C���^
	
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	
	NULL,	//�\���Ɏg�p���郁�b�Z�[�W�o�b�t�@
	NULL,	//�\���Ɏg�p����v�����g���[�e�B���e�B
	NULL,	//�\���Ɏg�p����v�����g�L���[
	NULL,	//�\���Ɏg�p����t�H���g�n���h��
};

GMEVENT_RESULT FIELD_COMM_DEBUG_CommDebugMenu( GMEVENT *event , int *seq , void *work )
{
	FIELD_COMM_DEBUG_WORK *commDeb = work;
	switch( *seq )
	{
	case 0:
		//���j���[�̐���
		{
			static const u8 itemNum = 2;
			static const u16 itemMsgList[itemNum] = 
			{
				DEBUG_FIELD_C_CHOICE00 ,
				DEBUG_FIELD_C_CHOICE01 ,
			};
			static const void* itemCallProc[itemNum] =
			{
				FIELD_COMM_DEBUG_MenuCallback_StartComm,
				FIELD_COMM_DEBUG_MenuCallback_StartInvasion,
			};
			u8 i;
			GFL_MSGDATA	*msgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL ,
					ARCID_MESSAGE , NARC_message_d_field_dat , commDeb->heapID_ );
			BMPMENULIST_HEADER menuHeader = FieldCommDebugMenuHeader;

			commDeb->debMenuWork_ = GFL_HEAP_AllocMemory( commDeb->heapID_ , DebugMenu_GetWorkSize() );
			commDeb->menuListData_ =
				BmpMenuWork_ListCreate( itemNum, commDeb->heapID_ );
			
			for( i=0;i<itemNum;i++ )
			{
				BmpMenuWork_ListAddArchiveString( 
						commDeb->menuListData_ , msgData ,
						itemMsgList[i] , (u32)itemCallProc[i] , commDeb->heapID_ );
			}


			DebugMenu_InitCommonMenu(
				commDeb->debMenuWork_,
				&menuHeader, commDeb->menuListData_, NULL, itemNum,
				11, 16, NARC_message_d_field_dat, commDeb->heapID_ );

			GFL_MSG_Delete( msgData );
		}
		*seq += 1;
		break;
	case 1:
		{
			const u32 ret = DebugMenu_ProcCommonMenu( commDeb->debMenuWork_ );
			switch( ret )
			{
			case BMPMENU_NULL:
				break;
			
			case BMPMENU_CANCEL:
				DebugMenu_DeleteCommonMenu( commDeb->debMenuWork_ );
				GFL_HEAP_FreeMemory( commDeb->debMenuWork_ );
				return GMEVENT_RES_FINISH;
				break;

			default:
				commDeb->callback_ = (FIELD_COMM_DEBUG_CALLBACK)ret;
				*seq += 1;
				break;
			}
		}
		break;
	case 2:
		BmpMenuWork_ListDelete( commDeb->menuListData_ );
		DebugMenu_DeleteCommonMenu( commDeb->debMenuWork_ );
		GFL_HEAP_FreeMemory( commDeb->debMenuWork_ );
		if( commDeb->callback_(commDeb) == TRUE )
			return GMEVENT_RES_CONTINUE;
		else
			return GMEVENT_RES_FINISH;
		break;
	}
	//return GMEVENT_RES_FINISH;
	return GMEVENT_RES_CONTINUE;
}

static	const BOOL	FIELD_COMM_DEBUG_MenuCallback_StartComm( FIELD_COMM_DEBUG_WORK *work )
{
	GMEVENT *event = work->event_;
	FIELD_COMM_MAIN *commSys = FieldMain_GetCommSys(work->fieldWork_);
	FIELD_COMM_EVENT *eveWork;
	
	GMEVENT_Change( event , FIELD_COMM_EVENT_StartCommNormal , FIELD_COMM_EVENT_GetWorkSize() );
			
	eveWork = GMEVENT_GetEventWork(event);
	FIELD_COMM_EVENT_SetWorkData( commSys , eveWork );
	return TRUE;
}
static	const BOOL	FIELD_COMM_DEBUG_MenuCallback_StartInvasion( FIELD_COMM_DEBUG_WORK *work )
{
	GMEVENT *event = work->event_;
	FIELD_COMM_MAIN *commSys = FieldMain_GetCommSys(work->fieldWork_);
	FIELD_COMM_EVENT *eveWork;
	
	GMEVENT_Change( event , FIELD_COMM_EVENT_StartCommInvasion , FIELD_COMM_EVENT_GetWorkSize() );
	
	eveWork = GMEVENT_GetEventWork(event);
	FIELD_COMM_EVENT_SetWorkData( commSys , eveWork );
	return TRUE;
}

