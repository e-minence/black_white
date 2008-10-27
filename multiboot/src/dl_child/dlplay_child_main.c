//======================================================================
/**
 *
 * @file	dlplay_child_main.c	
 * @brief	�_�E�����[�h�v���C�q�@
 * @author	ariizumi
 * @data	08/10/21
 */
//======================================================================
#include <gflib.h>

#include "system/main.h"

#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_comm_func.h"
#include "../../pokemon_wb/prog/src/test/dlplay/dlplay_func.h"
#include "../../pokemon_wb/prog/src/test/ariizumi/ari_debug.h"

#include "dlplay_data_main.h"

//======================================================================
//	define
//======================================================================
//BG�ʒ�`
#define DLPLAY_MSG_PLANE			(GFL_BG_FRAME3_M)
#define DLPLAY_MSG_PLANE_PRI		(0)

//======================================================================
//	enum
//======================================================================
enum DLPLAY_CHILD_STATE
{
	DCS_INIT_COMM,
	DCS_WAIT_INIT_COMM,
	DCS_WAIT_CONNECT,

	DCS_LOAD_BACKUP,
	DCS_SAVE_BACKUP,
	DCS_MAX,
	
};


//======================================================================
//	typedef struct
//======================================================================
typedef struct
{
	int	heapID_;

	u8	mainSeq_;
	u8	parentMacAddress_[WM_SIZE_BSSID];

	DLPLAY_COMM_DATA *commSys_;
	DLPLAY_MSG_SYS	 *msgSys_;
	DLPLAY_DATA_DATA *dataSys_;
}DLPLAY_CHILD_DATA;
//======================================================================
//	proto
//======================================================================

DLPLAY_CHILD_DATA *childData;


void	DLPlayChild_SetProc(void);
void	DLPlayChild_InitBg(void);

//============================================================================================
//
//
//	�v���Z�X�R���g���[��
//
//
//============================================================================================
static const	GFL_PROC_DATA DLPlayChild_ProcData;

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�ݒ�
 */
//------------------------------------------------------------------
void	DLPlayChild_SetProc(void)
{
	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &DLPlayChild_ProcData, NULL);
}

//------------------------------------------------------------------
/**
 * @brief		������
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DLPlayChild_ProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	u8	parentMacAddress[ WM_SIZE_BSSID ];
	WMBssDesc desc;		//�e�@���
	//�e�@�̏����擾���Ă݂�
	if( MB_IsMultiBootChild() == FALSE ){
#if DEB_ARI
		//����04080852��MacAddress(��ł�)
		desc.ssid[0] = 0x00;
		desc.ssid[1] = 0x09;
		desc.ssid[2] = 0xBF;
		desc.ssid[3] = 0x08;
		desc.ssid[4] = 0x23;
		desc.ssid[5] = 0x0D;
#else
		GF_ASSERT("This DS is not multiboot child!!\n");
#endif
	}
	else{
		MB_ReadMultiBootParentBssDesc( &desc , 32 , 32 , FALSE , FALSE );
	}

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ARIIZUMI_DEBUG, 0x200000 );

	DLPlayChild_InitBg();


	childData = GFL_HEAP_AllocClearMemory( HEAPID_ARIIZUMI_DEBUG, sizeof( DLPLAY_CHILD_DATA ) );

	childData->heapID_ = HEAPID_ARIIZUMI_DEBUG;
	childData->mainSeq_ = DCS_INIT_COMM;

	childData->msgSys_	= DLPlayFunc_MsgInit( childData->heapID_ , DLPLAY_MSG_PLANE );	 
	DLPlayFunc_PutString("",childData->msgSys_);
	childData->commSys_ = DLPlayComm_InitData( childData->heapID_ );
	childData->dataSys_ = DLPlayData_InitSystem( childData->heapID_ , childData->msgSys_ );

	GFL_STD_MemCopy( (void*)&desc.bssid , (void*)childData->parentMacAddress_ , WM_SIZE_BSSID );

	DLPlayFunc_PutString("System Initialize complete.",childData->msgSys_);

#if DEB_ARI
	{
		char str[128];
		sprintf(str,"Parent MacAddress is [%02x:%02x:%02x:%02x:%02x:%02x]"
					,desc.bssid[0]	,desc.bssid[1]	,desc.bssid[2]
					,desc.bssid[3]	,desc.bssid[4]	,desc.bssid[5]	);
		DLPlayFunc_PutString( str , childData->msgSys_ );
	}

	childData->mainSeq_ = DCS_LOAD_BACKUP;
	DLPlayFunc_PutString("Start backup test mode.",childData->msgSys_);

#endif
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DLPlayChild_ProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	switch( childData->mainSeq_ )
	{
	case DCS_INIT_COMM:
		DLPlayComm_InitSystem( childData->commSys_ );
		childData->mainSeq_ = DCS_WAIT_INIT_COMM;
		break;

	case DCS_WAIT_INIT_COMM:
		if( DLPlayComm_IsFinish_InitSystem( childData->commSys_ ) ){
			DLPlayComm_InitChild( childData->commSys_ , childData->parentMacAddress_ );
			DLPlayFunc_PutString("Commnicate system initialize complete.",childData->msgSys_);
			DLPlayFunc_PutString("Try connect parent.",childData->msgSys_);
			childData->mainSeq_ = DCS_WAIT_CONNECT;
		}
		break;
	case DCS_WAIT_CONNECT:
		if( DLPlayComm_IsFinish_ConnectParent( childData->commSys_ ) == TRUE ){
			DLPlayFunc_PutString("Succsess connect parent!",childData->msgSys_);
			childData->mainSeq_ = DCS_MAX;
			
			DLPlayComm_Send_ConnectSign(childData->commSys_);
		}
		break;
	case DCS_MAX:
			//�f�[�^���M�e�X�g
			if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_Y )
			{
				DLPlayComm_Send_ConnectSign(childData->commSys_);
			}
			if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_X )
			{
				u8 i=0;
				DLPLAY_LARGE_PACKET pak;
#if 1
				sprintf(pak.data_[i++],"���̃f�[�^�̓e�X�g�f�[�^�ł��B");
				sprintf(pak.data_[i++],"���w�V�Q�[���f�U�C���x��ǂ�ŁA�ǂ������Ƃ���͂ǂ��ł����H");
				sprintf(pak.data_[i++],"�@�F�X�ȃQ�[�����ʔ����Ǝv���闝�R���A�v�f���Ƃɕ������ď����Ă���Ƃ���B");
				sprintf(pak.data_[i++],"�@�����悤�ȃQ�[���ł��A�Ȃ���̂ق��ɏo�����̂��ʔ����Ȃ��̂��H��Ԑ�����");
				sprintf(pak.data_[i++],"������̂��Ƃ����^�₪�������̂ł����A�w�V�Q�[���f�U�C���x�Ŋi���Q�[����");
				sprintf(pak.data_[i++],"�V���[�e�B���O�ŋ�̗�������ďo���Ă���̂ŁA�ƂĂ��[���ł����B");
				sprintf(pak.data_[i++],"���w�V�Q�[���f�U�C���x��ǂ�ŁA�s���������Ƃ���͂ǂ��ł����H");
				sprintf(pak.data_[i++],"�@�Q�l�ɂ��Ă���Q�[���A�s�ꂪ�Â��Ƃ���B");
				sprintf(pak.data_[i++],"�@�\�N�ȏ���O�̖{�Ȃ̂Ŏd���Ȃ��Ƃ͎v�����A��͂荡�Ɣ�ׂ�ƃQ�[���@�̃X");
				sprintf(pak.data_[i++],"�y�b�N���s��̉��l�ς��Ⴄ�Ǝv���̂ŁA���̃Q�[���E�s��ł������������e�̖{");
				sprintf(pak.data_[i++],"��ǂ݂����Ǝv���B");
				sprintf(pak.data_[i++],"���w�V�Q�[���f�U�C���x����ǂݎ���Q�[���t���[�N�炵���͉����Ǝv���܂����H");
				sprintf(pak.data_[i++],"�@�����̃Q�[����͕킷��̂ł͂Ȃ��A�����ʔ������Ƃ��Q�[���Ƃ����`�ɂ���");
				sprintf(pak.data_[i++],"�\�����鎖�B");
				sprintf(pak.data_[i++],"�������ɂƂ��āA�֐S�̍������ЂƂ́A�ǂ̂悤�Ȃ��̂ł����H");
				sprintf(pak.data_[i++],"�@��̎����g�R�g���ǋ����Ă���{�B");
#else
				for( i=0;i<TEST_DATA_LINE;i++ )
				{
					u16 j;
					for( j=0;j<TEST_DATA_NUM;j++ )
					{
						pak.data_[i][j] =('A'+i);
					}
					pak.data_[i][TEST_DATA_NUM-1] = '\0';
				}
#endif
				DLPlayComm_Send_LargeData( &pak , childData->commSys_ );
			}

		break;

	case DCS_LOAD_BACKUP:
		{
			const BOOL ret = DLPlayData_LoadDataFirst( childData->dataSys_ );
			if( ret == TRUE ){ childData->mainSeq_ = DCS_SAVE_BACKUP; }
		}
		break;
	case DCS_SAVE_BACKUP:
		DLPlayData_SaveData( childData->dataSys_ );
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief		�I��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DLPlayChild_ProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA DLPlayChild_ProcData = {
	DLPlayChild_ProcInit,
	DLPlayChild_ProcMain,
	DLPlayChild_ProcEnd,
};


//------------------------------------------------------------------
//BG������
//------------------------------------------------------------------
//------------------------------------------------------------------
//	VRAM�p��`
//------------------------------------------------------------------
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	
static const GFL_BG_BGCNT_HEADER bgCont3 = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

void	DLPlayChild_InitBg(void)
{
	//VRAM�n������
	//BG�V�X�e���N��
	GFL_BG_Init( HEAPID_ARIIZUMI_DEBUG );

	//VRAM�ݒ�
	GX_SetBankForBG( GX_VRAM_BG_128_A );
	GX_SetBankForOBJ( GX_VRAM_OBJ_128_B );

	//BG���[�h�ݒ�
	GFL_BG_SetBGMode( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl( DLPLAY_MSG_PLANE, &bgCont3, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( DLPLAY_MSG_PLANE, DLPLAY_MSG_PLANE_PRI );
	GFL_BG_SetVisible( DLPLAY_MSG_PLANE, VISIBLE_ON );

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_Init( HEAPID_ARIIZUMI_DEBUG );

}


