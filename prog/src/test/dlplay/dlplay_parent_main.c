//============================================================================================
/**
 * @file	dlplay_parent_main.c
 * @brief	�_�E�����[�h�v���C �q�@�z�M����
 * @author	ariizumi
 * @date	2008.10.8
 */
//============================================================================================
#include "string.h"
#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "system/main.h"
#include "arc_def.h"
#include "mbp.h"

#include "test/ariizumi/ari_debug.h"
#include "dlplay_parent_sample.h"
#include "dlplay_parent_main.h"
#include "dlplay_func.h"
#include "dlplay_comm_func.h"
#include "dlplay_disp_sys.h"
#include "test/performance.h"

#include "message.naix"
#include "test_graphic/d_taya.naix"

/* ���̃f���Ŏg�p���� GGID */
#define WH_GGID				 (0x3FFF21)

/* ���̃f�����_�E�����[�h������v���O������� */
const MBGameRegistry mbGameList = {
	"/dl_rom/child.srl",					  // �q�@�o�C�i���R�[�h
	(u16 *)L"MultiBootTest",		   // �Q�[����
	(u16 *)L"�}���`�u�[�g�e�X�g by Ariizumi",		// �Q�[�����e����
	"/dl_rom/icon.char",				 // �A�C�R���L�����N�^�f�[�^
	"/dl_rom/icon.plt",				  // �A�C�R���p���b�g�f�[�^
	WH_GGID,						   // GGID
	MBP_CHILD_MAX + 1,				 // �ő�v���C�l���A�e�@�̐����܂߂��l��
};


//============================================================================================
//	struct
//============================================================================================
struct _DLPLAY_SEND_DATA
{
	u8 mainSeq_;
	u16 subSeq_;
	int	currTray_;
	u8	errorState_;
	u16	waitCounter_;
	u16	waitTimer_;

	DLPLAY_COMM_DATA	*commSys_;
	DLPLAY_MSG_SYS		*msgSys_;
	DLPLAY_DISP_SYS		*dispSys_;
};

//======================================================================
//	enum
//======================================================================
enum DLPLAY_SEND_STATE
{
	DSS_INIT_COMM,
	DSS_WAIT_INIT_COMM,
	DSS_WAIT_START,
	DSS_MAIN_LOOP,

	DSS_START_SHARE,
	DSS_WAIT_CHILD,
	DSS_WAIT_START_POST_INDEX,
	DSS_WAIT_INDEX_DATA,
	DSS_SELECT_BOX_TEMP,
	DSS_SELECT_BOX_CONFIRM,
	
	DSS_SAVE_MAIN,

	DSS_ERROR_INIT,
	DSS_ERROR_LOOP,

	DSS_MAX,
};

//============================================================================================
//	proto
//============================================================================================
DLPLAY_SEND_DATA* DLPlaySend_Init( int heapID );
u8		DLPlaySend_Loop( DLPLAY_SEND_DATA *dlData );
void	DLPlaySend_Term( DLPLAY_SEND_DATA *dlData );

static BOOL DLPlaySend_MBPLoop( DLPLAY_SEND_DATA *dlData );
static void OnPreSendMBVolat(u32 ggid);
static void	DLPlaySend_SaveMain( DLPLAY_SEND_DATA *dlData );

//============================================================================================

//--------------------------------------------------------------
/**
 * ������
 * @param	heapID	�q�[�vID
 * @retval	
 */
//--------------------------------------------------------------
DLPLAY_SEND_DATA* DLPlaySend_Init( int heapID )
{
	DLPLAY_SEND_DATA *dlData;
	dlData = GFL_HEAP_AllocClearMemory( heapID , sizeof( DLPLAY_SEND_DATA ) );

	dlData->commSys_ = DLPlayComm_InitData( heapID );
	dlData->msgSys_ = DLPlayFunc_MsgInit( heapID , DLPLAY_MSG_PLANE );
	dlData->dispSys_ = DLPlayDispSys_InitSystem( heapID );
	DLPlayFunc_FontInit( ARCID_D_TAYA , NARC_d_taya_lc12_2bit_nftr ,
					ARCID_MESSAGE , NARC_message_d_dlplay_dat ,
					ARCID_D_TAYA , NARC_d_taya_default_nclr , 
					DLPLAY_FONT_MSG_PLANE , dlData->msgSys_ );

	dlData->errorState_ = DES_NONE;

	dlData->mainSeq_ = DSS_INIT_COMM;
	dlData->subSeq_ = 0xFFFF;
#if DLPLAY_FUNC_USE_PRINT
	DLPlayComm_SetMsgSys( dlData->msgSys_ , dlData->commSys_ );
	DLPlayFunc_PutString("",dlData->msgSys_);
	DLPlayFunc_PutString("System Initialize complete.",dlData->msgSys_);
#endif
	DEBUG_PerformanceSetActive( FALSE );
	DLPlayFunc_ChangeBgMsg( MSG_SEARCH_CHILD , dlData->msgSys_ );
	return dlData;
}


//--------------------------------------------------------------
/**
 * �J��
 * @param	heapID	�q�[�vID
 * @retval	
 */
//--------------------------------------------------------------
void	DLPlaySend_Term( DLPLAY_SEND_DATA *dlData )
{
	DLPlayDispSys_TermSystem( dlData->dispSys_ );
	DLPlayFunc_MsgTerm( dlData->msgSys_ );
}

//--------------------------------------------------------------
/**
 * ���C�����[�v
 * @param	
 * @retval	���̃p�[�g	
 */
//--------------------------------------------------------------
u8		DLPlaySend_Loop( DLPLAY_SEND_DATA *dlData )
{
	DLPlayFunc_UpdateFont( dlData->msgSys_ );
	if( dlData->errorState_ != DSS_ERROR_INIT &&
		dlData->errorState_ != DSS_ERROR_LOOP )
	{
		if( dlData->errorState_ != DES_NONE )
		{
			dlData->mainSeq_ = DSS_ERROR_INIT;
		}
		else
		if( DLPlayComm_GetPostErrorState( dlData->commSys_ ) != DES_NONE )
		{
			dlData->errorState_ = DLPlayComm_GetPostErrorState( dlData->commSys_ );
			dlData->mainSeq_ = DSS_ERROR_INIT;
		}
	}
	switch( dlData->mainSeq_ )
	{
	case DSS_INIT_COMM:
		DLPlayComm_InitSystem( dlData->commSys_ );
		dlData->mainSeq_ = DSS_WAIT_INIT_COMM;
		break;

	case DSS_WAIT_INIT_COMM:
		if( DLPlayComm_IsFinish_InitSystem( dlData->commSys_ ) ){
			DLPlayFunc_PutString("Commnicate system initialize omplete.",dlData->msgSys_);
			DLPlayFunc_PutString("Press A Button to start.",dlData->msgSys_);
			dlData->mainSeq_ = DSS_WAIT_START;
		}
		break;

	case DSS_WAIT_START:
		//if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A ){
			MBP_Init( WH_GGID , 127 );	

			dlData->mainSeq_ = DSS_MAIN_LOOP;
			DLPlayFunc_PutString( "MultiBoot is start."  ,dlData->msgSys_ );
		//}
		break;

	case DSS_MAIN_LOOP:
		DLPlaySend_MBPLoop( dlData );
		//�����̒���mainSeq��ς��Ă�
		break;

	case DSS_START_SHARE:
		DLPlayComm_InitParent( dlData->commSys_ );
		DLPlayFunc_PutString( "Wait child......",dlData->msgSys_); 
		dlData->mainSeq_ = DSS_WAIT_CHILD;
		break;

	case DSS_WAIT_CHILD:
		if ( DLPlayComm_IsConnect( dlData->commSys_ ) == TRUE ){
			DLPlayFunc_ChangeBgMsg( MSG_WAIT_CHILD_PROC , dlData->msgSys_ );
			
			DLPlayFunc_PutString( "Child connect.",dlData->msgSys_); 
			dlData->mainSeq_ = DSS_WAIT_START_POST_INDEX;
		}
		break;
	case DSS_WAIT_START_POST_INDEX:
		if ( DLPlayComm_IsStartPostIndex( dlData->commSys_ ) == TRUE ){
			DLPlayFunc_ChangeBgMsg( MSG_POST_DATA_CHILD , dlData->msgSys_ );
			
			dlData->mainSeq_ = DSS_WAIT_INDEX_DATA;
		}
		break;

	case DSS_WAIT_INDEX_DATA:
		if(	DLPlayComm_IsPostIndex( dlData->commSys_ ) == TRUE ){
			DLPlayFunc_PutString( "Post box index data complete.",dlData->msgSys_); 
			DLPlayFunc_ChangeBgMsg( MSG_SELECT_BOX , dlData->msgSys_ );
			dlData->mainSeq_ = DSS_SELECT_BOX_TEMP;
			dlData->currTray_ = 0xFF;
			//return DPM_SELECT_BOX;
		}
		break;
	case DSS_SELECT_BOX_TEMP:
		{
			u8 i;
			BOOL isUpdate = FALSE;
			if( dlData->currTray_ == 0xFF )
			{
				isUpdate = TRUE;
				dlData->currTray_ = 0;
			}
			if( GFL_UI_KEY_GetTrg() == PAD_KEY_RIGHT )
			{
				isUpdate = TRUE;
				dlData->currTray_++;
				if( dlData->currTray_ >= 18 )
				{
					dlData->currTray_ = 0;
				}
			}
			if( GFL_UI_KEY_GetTrg() == PAD_KEY_LEFT )
			{
				isUpdate = TRUE;
				dlData->currTray_--;
				if( dlData->currTray_ < 0 )
				{
					dlData->currTray_ = 17;
				}
			}
			if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
			{
				dlData->mainSeq_ = DSS_SELECT_BOX_CONFIRM;
				DLPlayFunc_ChangeBgMsg( MSG_CONFIRM , dlData->msgSys_ );
			}
			if( isUpdate == TRUE )
			{
#if 1
				DLPLAY_BOX_INDEX *boxIndex = DLPlayComm_GetBoxIndexBuff( dlData->commSys_ );
				DLPlayDispSys_DispBoxIcon( boxIndex , dlData->currTray_ , dlData->dispSys_ );
//#else 
				{
					const char sexStr[3][8] ={"M","F","?"};
					const int bi = dlData->currTray_;
					u16 i;
					int	strLen = 64;
					char str[128],w1Str[64],w2Str[64];
					DLPLAY_BOX_INDEX *boxIndex = DLPlayComm_GetBoxIndexBuff( dlData->commSys_ );
	
					DLPlayFunc_ClearString( dlData->msgSys_ );
	 
					STD_ConvertStringUnicodeToSjis( w1Str , &strLen , boxIndex->boxName_[bi] , NULL , NULL );
					w1Str[strLen] = '\0';
					sprintf(str,"BoxName[%s]",w1Str);
					DLPlayFunc_PutStringLine( 0,str,dlData->msgSys_ );
	
					for( i=0;i<30;i++ )
					{
						DLPLAY_MONS_INDEX *pokeData = &boxIndex->pokeData_[bi][i];
						if( pokeData->pokeNo_ == 0 )
						{
							sprintf(w2Str,"[---][----------]");
						}
						else
						{
							strLen = 64;
							STD_ConvertStringUnicodeToSjis( w1Str , &strLen , pokeData->name_ , NULL , NULL );
							w1Str[strLen] = '\0';
							sprintf(w2Str,"[%3d][%s:%s][%d]",pokeData->pokeNo_,w1Str,sexStr[pokeData->sex_],pokeData->lv_);
							if( pokeData->isEgg_ == 1 )
							{
								strcat( w2Str , "[E]" );
							}
							if( pokeData->rare_ == 1 )
							{
								strcat( w2Str , "[R]" );
							}
							strcat(w2Str,"      ");
						}
						if( i%2==1 )
						{
							DLPlayFunc_PutStringLineDiv( i/2+1,str,w2Str,dlData->msgSys_ );
						}
						else
						{
							strcpy(str,w2Str);
						}
					}
				}
#endif
			}
		}
		break;
		
	case DSS_SELECT_BOX_CONFIRM:
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
		{
			DLPlayComm_Send_BoxNumber( dlData->currTray_ , dlData->commSys_ );
			DLPlayFunc_ChangeBgMsg( MSG_SAVE , dlData->msgSys_ );
			dlData->mainSeq_ = DSS_SAVE_MAIN;
			dlData->subSeq_ = 0;
		}
		else if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
		{
			DLPlayFunc_ChangeBgMsg( MSG_SELECT_BOX , dlData->msgSys_ );
			dlData->mainSeq_ = DSS_SELECT_BOX_TEMP;
		}
		break;
	
	case DSS_SAVE_MAIN:
		DLPlaySend_SaveMain( dlData );
		break;

	case DSS_ERROR_INIT:
		dlData->mainSeq_ = DSS_ERROR_LOOP;
		if( dlData->errorState_ == DES_MISS_LOAD_BACKUP )
		{
			DLPlayFunc_ChangeBgMsg( MSG_MISS_LOAD_BACKUP , dlData->msgSys_ );
		}
		else
		{
			DLPlayFunc_ChangeBgMsg( MSG_ERROR , dlData->msgSys_ );
		}
		if( DLPlayComm_GetPostErrorState( dlData->commSys_ ) == DES_NONE )
		{
			//��M���ăG���[�ɗ����Ƃ��͑���Ȃ�
			DLPlayComm_Send_ErrorState( dlData->errorState_ , dlData->commSys_ );
		}
		break;

	case DSS_ERROR_LOOP:
		break;
		

	}
	DLPlayDispSys_UpdateDraw( dlData->dispSys_ );
	return DPM_SEND_IMAGE;
}

static BOOL DLPlaySend_MBPLoop( DLPLAY_SEND_DATA *dlData )
{
	const u16 mbpState = MBP_GetState();
	BOOL isChangeState = FALSE;

	if( mbpState != dlData->subSeq_ ){
		isChangeState = TRUE;
		dlData->subSeq_ = mbpState;
	}
	//�ȉ��T���v�����p
	switch (MBP_GetState())
	{
		//-----------------------------------------
		// �A�C�h�����
	case MBP_STATE_IDLE:
		{
			u16 test = WM_GetAllowedChannel();
			ARI_TPrintf("[%d][%x]\n",test,test);
			MBP_Start(&mbGameList, 1 /*�ʐM�`�����l��sChannel*/);
			/* ���[�U��`�f�[�^�̑��M�e�X�g */
			MB_SetSendVolatCallback(OnPreSendMBVolat, MB_SEND_VOLAT_CALLBACK_TIMMING_BEFORE);
		}
		break;

		//-----------------------------------------
		// �q�@����̃G���g���[��t��
	case MBP_STATE_ENTRY:
		{
			if( isChangeState == TRUE ){
				DLPlayFunc_PutString( "Now Accepting",dlData->msgSys_); 
			}
			//BgSetMessage(PLTT_WHITE, " Now Accepting			");

			if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
			{
				// B�{�^���Ń}���`�u�[�g�L�����Z��
				MBP_Cancel();
				break;
			}

			// �G���g���[���̎q�@�����ł����݂���ΊJ�n�\�Ƃ���
			if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
				MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
				MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
			{
				//if( isChangeState == TRUE ){
					DLPlayFunc_PutString( "Find child. send RomImage start.",dlData->msgSys_); 
					DLPlayFunc_ChangeBgMsg( MSG_FIND_CHILD , dlData->msgSys_ );
				//}
				//BgSetMessage(PLTT_WHITE, " Push START Button to start   ");
				//�q�@��������Ƃ肠�����n�߂Ă��܂�
				//if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_START )
				{
					// �_�E�����[�h�J�n
					MBP_StartDownloadAll();
				}
			}
		}
		break;

		//-----------------------------------------
		// �v���O�����z�M����
	case MBP_STATE_DATASENDING:
		{

			// �S�����_�E�����[�h�������Ă���Ȃ�΃X�^�[�g�\.
			if (MBP_IsBootableAll())
			{
				// �u�[�g�J�n
				MBP_StartRebootAll();
			}
		}
		break;

		//-----------------------------------------
		// ���u�[�g����
	case MBP_STATE_REBOOTING:
		{
			if( isChangeState == TRUE ){
				DLPlayFunc_PutString( "Rebooting now",dlData->msgSys_); 
				DLPlayFunc_ChangeBgMsg( MSG_WAIT_CHILD_CONNECT , dlData->msgSys_ );
			}
			//BgSetMessage(PLTT_WHITE, " Rebooting now				");
		}
		break;

		//-----------------------------------------
		// �Đڑ�����
	case MBP_STATE_COMPLETE:
		{
			if( isChangeState == TRUE ){
				DLPlayFunc_PutString( "Reconnecting now",dlData->msgSys_); 
			}
			// �S�������ɐڑ�����������}���`�u�[�g�����͏I����
			// �ʏ�̐e�@�Ƃ��Ė������ċN������B
			//BgSetMessage(PLTT_WHITE, " Reconnecting now			 ");

			OS_WaitVBlankIntr();
			dlData->mainSeq_ = DSS_START_SHARE;
			//return DPM_DATA_SHARE;
		}
		break;

		//-----------------------------------------
		// �G���[����
	case MBP_STATE_ERROR:
		{
			// �ʐM���L�����Z������
			MBP_Cancel();
		}
		break;

		//-----------------------------------------
		// �ʐM�L�����Z��������
	case MBP_STATE_CANCEL:
		// None
		break;

		//-----------------------------------------
		// �ʐM�ُ�I��
	case MBP_STATE_STOP:
#ifdef MBP_USING_MB_EX
		/*
		switch (WH_GetSystemState())
		{
		case WH_SYSSTATE_IDLE:
			(void)WH_End();
			break;
		case WH_SYSSTATE_BUSY:
			break;
		case WH_SYSSTATE_STOP:
			return FALSE;
		default:
			OS_Panic("illegal state\n");
		}
		*/
		break;
#else
		return FALSE;
#endif
	}

	// �q�@��Ԃ�\������
	//PrintChildState();
	return TRUE;
}

static void	DLPlaySend_SaveMain( DLPLAY_SEND_DATA *dlData )
{
	switch( dlData->subSeq_ )
	{
	case 0:
		//�Z�[�u�O����
		DLPlayFunc_ClearString( dlData->msgSys_ );
		DLPlayFunc_PutString("Save Ready.",dlData->msgSys_);
		dlData->subSeq_++;
		break;
	case 1:
		//�Z�[�u�J�n
		DLPlayFunc_PutString("Save Start.",dlData->msgSys_);
		dlData->waitTimer_ = 600;
		DLPlayFunc_InitCounter( &dlData->waitCounter_ );
		dlData->subSeq_++;
		break;
	case 2:
		//�Z�[�u�����҂�
		if( DLPlayFunc_UpdateCounter( &dlData->waitCounter_ , dlData->waitTimer_ ) == TRUE )
		{
			DLPlayFunc_PutString("Save end. Wait child.",dlData->msgSys_);
			DLPlayComm_Send_CommonFlg( DC_FLG_FINISH_SAVE1_PARENT , 0 , dlData->commSys_ );
			dlData->subSeq_++;
		}
		break;
	case 3:
		//���������Z�[�u�҂��ɂȂ���
		if( DLPlayComm_IsFinishSaveFlg( DC_FLG_FINISH_SAVE1_CHILD , dlData->commSys_ ) == TRUE )
		{
			u16 waitTime = GFL_STD_MtRand(60)+30;
			DLPlayFunc_PutString("Send flg last save wait.",dlData->msgSys_);
			DLPlayComm_Send_CommonFlg( DC_FLG_PERMIT_LASTBIT_SAVE , waitTime , dlData->commSys_ );
			dlData->subSeq_++;
		}
		break;
	case 4:
		if( DLPlayComm_IsFinishSaveFlg( DC_FLG_PERMIT_LASTBIT_SAVE , dlData->commSys_ ) > 0 )
		{
			DLPlayFunc_PutString("Start last save wait.",dlData->msgSys_);
			dlData->waitTimer_ = DLPlayComm_IsFinishSaveFlg( DC_FLG_PERMIT_LASTBIT_SAVE , dlData->commSys_ );
			DLPlayFunc_InitCounter( &dlData->waitCounter_ );
			dlData->subSeq_++;
		}
		break;
	case 5:
		if( DLPlayFunc_UpdateCounter( &dlData->waitCounter_ , dlData->waitTimer_ ) == TRUE )
		{
			//�ŏI�r�b�g�Z�[�u
			DLPlayFunc_PutString("Last save.",dlData->msgSys_);
			dlData->subSeq_++;
		}
		break;
	case 6:
		DLPlayFunc_PutString("Save Complete!!.",dlData->msgSys_);
		DLPlayFunc_ChangeBgMsg( MSG_SAVE_END , dlData->msgSys_ );
		dlData->mainSeq_ = DSS_MAX;
		break;
	}
}

/*---------------------------------------------------------------------------*
  Name:		 OnPreSendMBVolat

  Description:  �e�@��MB�r�[�R���𑗐M����O�ɒʒm�����R�[���o�b�N�֐�

  Arguments:	ggid	���M����Q�[������GGID.

  Returns:	  None.
 *---------------------------------------------------------------------------*/
static void OnPreSendMBVolat(u32 ggid)
{
	/*
	 * �����̃Q�[����o�^���X�̃Q�[�����Ƃɐݒ�l��؂�ւ���ꍇ��
	 * �ȉ��̂悤�Ɉ����� ggid �ŃQ�[�����𔻒肵�܂�.
	 */
	if (ggid == mbGameList.ggid)
	{
		/*
		 * ���M�\�ȃ��[�U��`�f�[�^�͍ő� 8 BYTE �ł�.
		 * ���̗�ł̓C���N�������^���� 64bit �l�𑗐M���Ă��܂�.
		 * ���I�ɍX�V���ꂽ�f�[�^���e���A�q�@���ł͕K��������������
		 * ��M�ł���킯�ł͂Ȃ��Ƃ����_�ɒ��ӂ��Ă�������.
		 */
		static u64 count ATTRIBUTE_ALIGN(8);
		SDK_COMPILER_ASSERT(sizeof(count) <= MB_USER_VOLAT_DATA_SIZE);
		SDK_COMPILER_ASSERT(MB_USER_VOLAT_DATA_SIZE == 8);
		++count;
		MB_SetUserVolatData(ggid, (u8 *)&count, sizeof(count));
	}
}
