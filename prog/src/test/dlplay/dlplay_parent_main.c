//============================================================================================
/**
 * @file	dlplay_parent_main.c
 * @brief	�_�E�����[�h�v���C �q�@�z�M����
 * @author	ariizumi
 * @date	2008.10.8
 */
//============================================================================================
#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "system/main.h"
#include "arc_def.h"
#include "multiboot/comm/mbp.h"

#include "test/ariizumi/ari_debug.h"
#include "dlplay_parent_sample.h"
#include "dlplay_parent_main.h"
#include "dlplay_func.h"

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
	u16 subSeq_;
	
	DLPLAY_MSG_SYS *msgSys_;
};

//======================================================================
//	enum
//======================================================================

//============================================================================================
//	proto
//============================================================================================
DLPLAY_SEND_DATA* DLPlaySend_Init( int heapID );
void	DLPlaySend_Term( DLPLAY_SEND_DATA *dlData );
void	DLPlaySend_StartMBP( DLPLAY_SEND_DATA *dlData );

void DLPlaySend_SetMessageSystem( DLPLAY_MSG_SYS *msgSys , DLPLAY_SEND_DATA *dlData );
BOOL DLPlaySend_MBPLoop( DLPLAY_SEND_DATA *dlData );
static void OnPreSendMBVolat(u32 ggid);
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


	dlData->subSeq_ = 0xFFFF;
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
	GFL_HEAP_FreeMemory( dlData );
}

void DLPlaySend_SetMessageSystem( DLPLAY_MSG_SYS *msgSys , DLPLAY_SEND_DATA *dlData )
{
	dlData->msgSys_ = msgSys;
}


void	DLPlaySend_StartMBP( DLPLAY_SEND_DATA *dlData )
{
	MBP_Init( WH_GGID , 127 );	
}

BOOL DLPlaySend_MBPLoop( DLPLAY_SEND_DATA *dlData )
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

			//OS_WaitVBlankIntr();
			return TRUE;
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
	}

	return FALSE;
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
