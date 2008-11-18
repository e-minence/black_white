//======================================================================
/**
 * @file	field_comm_main.c
 * @brief	�t�B�[���h�ʐM�̏����̊O�ƒ����Ȃ��֐��Ƃ��Q
 * @author	ariizumi
 * @data	08/11/11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/gamesystem.h"
#include "field_comm_main.h"
#include "field_comm_menu.h"
#include "field_comm_func.h"
#include "field_comm_data.h"

#include "msg/msg_d_field.h"

//======================================================================
//	define
//======================================================================
#define BGPLANE_YESNO_WINDOW (GFL_BG_FRAME1_M)	//�g�p�t���[��(DEBUG_BGFRAME_MENU 
#define BGPLANE_MSG_WINDOW (GFL_BG_FRAME2_M)	//�g�p�t���[��(DEBUG_BGFRAME_MENU 

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================
struct _FIELD_COMM_MAIN
{
	u8	menuSeq_;
	HEAPID	heapID_;
	FIELD_COMM_MENU *commMenu_;
	FIELD_COMM_FUNC *commFunc_;
};

//======================================================================
//	proto
//======================================================================

FIELD_COMM_MAIN* FIELD_COMM_MAIN_InitSystem( HEAPID heapID , HEAPID commHeapID );
void	FIELD_COMM_MAIN_TermSystem( FIELD_COMM_MAIN *commSys , BOOL isTermAll );
void	FIELD_COMM_MAIN_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork , 
				GAMESYS_WORK *gameSys , PC_ACTCONT *pcActor , FIELD_COMM_MAIN *commSys );

//�ڑ��J�n�p���j���[����
//�J�n��
void	FIELD_COMM_MAIN_InitStartCommMenu( FIELD_COMM_MAIN *commSys );
void	FIELD_COMM_MAIN_TermStartCommMenu( FIELD_COMM_MAIN *commSys );
const BOOL	FIELD_COMM_MAIN_LoopStartCommMenu( FIELD_COMM_MAIN *commSys );
//���̎�
void	FIELD_COMM_MAIN_InitStartInvasionMenu( FIELD_COMM_MAIN *commSys );
void	FIELD_COMM_MAIN_TermStartInvasionMenu( FIELD_COMM_MAIN *commSys );
const BOOL	FIELD_COMM_MAIN_LoopStartInvasionMenu( FIELD_COMM_MAIN *commSys );

//--------------------------------------------------------------
//	�t�B�[���h�ʐM�V�X�e��������
//	@param	commHeapID �ʐM�p�ɏ풓����q�[�vID
//			�ʐM���L���ȊԒ��J������Ȃ�Heap���w�肵�Ă�������
//--------------------------------------------------------------
FIELD_COMM_MAIN* FIELD_COMM_MAIN_InitSystem( HEAPID heapID , HEAPID commHeapID )
{
	FIELD_COMM_MAIN *commSys;
	commSys = GFL_HEAP_AllocMemory( heapID , sizeof(FIELD_COMM_MAIN) );
	commSys->heapID_ = heapID;
	commSys->commFunc_ = FIELD_COMM_FUNC_InitSystem( heapID );
	
	FIELD_COMM_DATA_InitSystem( commHeapID );
	return commSys;
}

//--------------------------------------------------------------
// �t�B�[���h�ʐM�V�X�e���J��
// @param isTermAll TRUE�Ńf�[�^�̈�̃q�[�v���J��
//--------------------------------------------------------------
void FIELD_COMM_MAIN_TermSystem( FIELD_COMM_MAIN *commSys , BOOL isTermAll )
{
	if( isTermAll == TRUE )
	{
		FIELD_COMM_DATA_TermSystem();
	}
	FIELD_COMM_FUNC_TermSystem( commSys->commFunc_ );
	GFL_HEAP_FreeMemory( commSys );
}

//--------------------------------------------------------------
// �t�B�[���h�ʐM�V�X�e���X�V
//	@param	FIELD_MAIN_WORK �t�B�[���h���[�N
//	@param	GAMESYS_WORK	�Q�[���V�X�e�����[�N(PLAYER_WORK�擾�p
//	@param	PC_ACTCONT		�v���C���[�A�N�^�[(�v���C���[���l�擾�p
//	@param	FIELD_COMM_MAIN	�ʐM���[�N
//	�����̃L�����̐��l���擾���ĒʐM�p�ɕۑ�
//	���L�����̏����擾���A�ʐM����ݒ�
//--------------------------------------------------------------
void	FIELD_COMM_MAIN_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork , 
				GAMESYS_WORK *gameSys , PC_ACTCONT *pcActor , FIELD_COMM_MAIN *commSys )
{
	if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commFunc_ ) == TRUE )
	{
		u8 i;
		FIELD_COMM_FUNC_UpdateSystem( commSys->commFunc_ );
		if( FIELD_COMM_FUNC_GetMemberNum( commSys->commFunc_ ) > 1 )
		{
			ZONEID zoneID;
			VecFx32 pos;
			u16 dir;
			PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
			//���L�������W���X�V
			zoneID = PLAYERWORK_getZoneID( plWork );
			GetPlayerActTrans( pcActor , &pos );
			//GetPlayerActDirection( pcActor , &dir );
			dir = FieldMainGrid_GetPlayerDir( fieldWork );
			FIELD_COMM_DATA_SetSelfData_Pos( &zoneID , &pos , &dir );
	
			FIELD_COMM_FUNC_Send_SelfData( commSys->commFunc_ );
		}
			//�͂����f�[�^�̃`�F�b�N
		for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
		{
			if( i != FIELD_COMM_FUNC_GetSelfIndex(commSys->commFunc_) &&
				FIELD_COMM_DATA_GetCharaData_IsValid(i) == TRUE )
			{
				//�L���ȃf�[�^�������Ă���
				GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
				PLAYER_WORK *setPlWork = GAMEDATA_GetPlayerWork( gameData , i+1 );	//0�ɂ͎����������Ă��邩��
				PLAYER_WORK *charaWork = FIELD_COMM_DATA_GetCharaData_PlayerWork(i);
				GFL_STD_MemCopy( (void*)charaWork , (void*)setPlWork , sizeof(PLAYER_WORK) );
				if( FIELD_COMM_DATA_GetCharaData_IsExist(i) == FALSE )
				{
					//���������ȃL�����Ȃ̂ŁA����������
					FieldMain_AddCommActor( fieldWork , setPlWork );
					FIELD_COMM_DATA_SetCharaData_IsExist(i,TRUE);
				}
				
				FIELD_COMM_DATA_SetCharaData_IsValid(i,FALSE);
			}
		}
	}
}

//--------------------------------------------------------------
// �ʐM�J�n���j���[������
//--------------------------------------------------------------
void	FIELD_COMM_MAIN_InitStartCommMenu( FIELD_COMM_MAIN *commSys )
{
	commSys->commMenu_ = FIELD_COMM_MENU_InitCommMenu( commSys->heapID_ );
	FIELD_COMM_MENU_OpenMessageWindow( BGPLANE_MSG_WINDOW , commSys->commMenu_ );
	FIELD_COMM_MENU_OpenYesNoMenu( BGPLANE_YESNO_WINDOW , commSys->commMenu_ );
	FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR00 , commSys->commMenu_ );
	commSys->menuSeq_ = 0;
}

//--------------------------------------------------------------
//	�ʐM�J�n���j���[ �J�� 
//--------------------------------------------------------------
void	FIELD_COMM_MAIN_TermStartCommMenu( FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_MENU_CloseMessageWindow( commSys->commMenu_ );
	FIELD_COMM_MENU_TermCommMenu( commSys->commMenu_ );
}

//--------------------------------------------------------------
// �ʐM�J�n���j���[�X�V
//--------------------------------------------------------------
const BOOL	FIELD_COMM_MAIN_LoopStartCommMenu( FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_MENU_UpdateMessageWindow( commSys->commMenu_ );
	switch( commSys->menuSeq_ )
	{
	case 0:
		{
			const u8 ret = FIELD_COMM_MENU_UpdateYesNoMenu( commSys->commMenu_ );
			if( ret == YNR_YES ){
				FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
				commSys->menuSeq_++;
			}
			else if( ret == YNR_NO ){
				FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
				return (TRUE);
			}
		}
		break;
	case 1:
		//���������̂Ƃ���������������
		if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commFunc_ ) == FALSE ){
			FIELD_COMM_FUNC_InitCommSystem( commSys->commFunc_ );
		}
		commSys->menuSeq_++;
		break;
	case 2:
		if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commFunc_ ) == TRUE ){
			FIELD_COMM_FUNC_StartCommWait( commSys->commFunc_ );
			commSys->menuSeq_++;
			return (TRUE);
		}
		break;
	}
	return (FALSE);
}

//--------------------------------------------------------------
// �N���J�n���j���[ ������
//--------------------------------------------------------------
void	FIELD_COMM_MAIN_InitStartInvasionMenu( FIELD_COMM_MAIN *commSys )
{
	commSys->commMenu_ = FIELD_COMM_MENU_InitCommMenu( commSys->heapID_ );
	FIELD_COMM_MENU_OpenMessageWindow( BGPLANE_MSG_WINDOW , commSys->commMenu_ );
	FIELD_COMM_MENU_OpenYesNoMenu( BGPLANE_YESNO_WINDOW , commSys->commMenu_ );
	FIELD_COMM_MENU_SetMessage( DEBUG_FIELD_C_STR01 , commSys->commMenu_ );
	commSys->menuSeq_ = 0;
}

//--------------------------------------------------------------
// �N���J�n���j���[�J��
//--------------------------------------------------------------
void	FIELD_COMM_MAIN_TermStartInvasionMenu( FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_MENU_CloseMessageWindow( commSys->commMenu_ );
	FIELD_COMM_MENU_TermCommMenu( commSys->commMenu_ );
}

//--------------------------------------------------------------
// �N���J�n���j���[�X�V
//--------------------------------------------------------------
const BOOL	FIELD_COMM_MAIN_LoopStartInvasionMenu( FIELD_COMM_MAIN *commSys )
{
	FIELD_COMM_MENU_UpdateMessageWindow( commSys->commMenu_ );
	switch( commSys->menuSeq_ )
	{
	case 0:
		{
			const u8 ret = FIELD_COMM_MENU_UpdateYesNoMenu( commSys->commMenu_ );
			if( ret == YNR_YES ){
				FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
				commSys->menuSeq_++;
			}
			else if( ret == YNR_NO ){
				FIELD_COMM_MENU_CloseYesNoMenu( commSys->commMenu_ );
				return (TRUE);
			}
		}
		break;
	case 1:
		//���������̂Ƃ���������������
		if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commFunc_ ) == FALSE ){
			FIELD_COMM_FUNC_InitCommSystem( commSys->commFunc_ );
		}
		commSys->menuSeq_++;
		break;
	case 2:
		if( FIELD_COMM_FUNC_IsFinishInitCommSystem( commSys->commFunc_ ) == TRUE ){
			FIELD_COMM_FUNC_StartCommSearch( commSys->commFunc_ );
			commSys->menuSeq_++;
			return (TRUE);
		}
		break;
	}
	return (FALSE);
}



