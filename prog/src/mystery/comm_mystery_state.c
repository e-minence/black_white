//=============================================================================
/**
 * @file	comm_mystery_state.c
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          	�X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          	�J�n��I�����Ǘ�����
 *		��comm_field_state.c�̐^�����q
 * @author	Satoshi Mitsuhara
 * @date    	2006.05.17
 */
//=============================================================================
#include <gflib.h>
#include "system/main.h"

#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "system/wipe.h"
#include "print/global_font.h"
#include "print/wordset.h"

#include "mystery.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_mystery.h"
#include "msg/msg_wifi_system.h"

#include "savedata/save_tbl.h"
#include "savedata/mystery_data.h"
#include "savedata/mystatus.h"
#include "savedata/save_control.h"
#include "savedata/config.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"

#include "comm_mystery_state.h"
#include "comm_mystery_gift.h"

//==============================================================================
//	�^�錾
//==============================================================================
// �R�[���o�b�N�֐��̏���
typedef void (*PTRStateFunc)(void);
//==============================================================================
// ���[�N
//==============================================================================
typedef struct{
	MYSTERYGIFT_WORK *pMSys;
	MATHRandContext32 sRand; 			//< �e�q�@�l�S�V�G�[�V�����p�����L�[
	GFL_TCBL *pTcb;
	PTRStateFunc state;
	u16 timer;
	u8 bStateNoChange;
	u8 connectIndex;   				// �q�@���ڑ�����e�@��index�ԍ�
	MYSTATUS *status[SCAN_PARENT_COUNT_MAX];

	DOWNLOAD_GIFT_DATA recv_data;				// ��M�����f�[�^
	u8 recv_flag;					// ��M�����t���O
	u8 result_flag[SCAN_PARENT_COUNT_MAX];	// �����Ǝ�M���܂����Ԏ�

	//  u8 beacon_data[MYSTERY_BEACON_DATA_SIZE];	// �r�[�R���f�[�^
	
} _COMM_STATE_WORK;

static _COMM_STATE_WORK *_pCommStateM = NULL;  ///<�@���[�N�\���̂̃|�C���^

//==============================================================================
// ��`
//==============================================================================
#define _START_TIME (50)     // �J�n����
#define _CHILD_P_SEARCH_TIME (12) ///�q�@�Ƃ��Đe��T������
#define _PARENT_WAIT_TIME (40) ///�e�Ƃ��Ă̂�т�҂���
#define _FINALIZE_TIME (2)
#define _EXIT_SENDING_TIME (5)
#define _PARENT_END_TIME (2)
#define _SEND_NAME_TIME (10)
#define _PARENTSCAN_PA (3)  // �e�@�Ƃ��Č�������m����1/3

#define _TCB_COMMCHECK_PRT   (10)    ///< �t�B�[���h������ʐM�̊Ď����[�`����PRI

//==============================================================================
// static�錾
//==============================================================================
static void _commCheckFunc(GFL_TCBL *tcb, void* work);  // �X�e�[�g�����s���Ă���^�X�N
static void _mysteryParentInit(void);
static void _mysteryParentWaiting(void);

static void _mysteryChildInit(void);
static void _mysteryChildBconScanning(void);
static void _mysteryChildConnecting(void);
static void _mysteryChildSendName(void);
static void _mysteryChildWaiting(void);

void CommMysteryGiftRecvPlace(const int netID,const  int size, const void* pBuff, void* pWork , GFL_NETHANDLE *pNetHandle);
void CommMysterySendRecvResult(const int netID,const  int size, const void* pBuff, void* pWork , GFL_NETHANDLE *pNetHandle);

//�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
	/* �ӂ����Ȃ�������́@�f�[�^���M */
	{ CommMysteryGiftRecvPlace,	CommGetMysteryGiftRecvBuff },
	/* ��M�������܂����̃t���O�@�f�[�^���M */
	{ CommMysterySendRecvResult,	NULL },
};


#define   _CHANGE_STATE(state, time)  _changeState(state, time)


//NET_Init�p�\���̂ɓn���@�\�֐�
BOOL CommMysteryBeaconCheck(GameServiceID GameServiceID1, GameServiceID GameServiceID2);
void* CommMysteryGetBeaconData(void* pWork);
int CommMysteryGetBeaconSize(void* pWork);

//	�r�[�R����r�֐�
BOOL CommMysteryBeaconCheck(GameServiceID GameServiceID1, GameServiceID GameServiceID2)
{
	return TRUE;
}


//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̏���������
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _commStateInitialize(MYSTERYGIFT_WORK *pMSys)
{
	void* pWork;

	if(_pCommStateM != NULL)		// ���łɓ��쒆�̏ꍇ�K�v�Ȃ�
		return;

	_pCommStateM = (_COMM_STATE_WORK*)GFL_HEAP_AllocMemory(HEAPID_MYSTERYGIFT, sizeof(_COMM_STATE_WORK));
	MI_CpuFill8(_pCommStateM, 0, sizeof(_COMM_STATE_WORK));
	_pCommStateM->timer = _START_TIME;
	_pCommStateM->pTcb = GFL_TCBL_Create(pMSys->tcblSys , _commCheckFunc, 0, _TCB_COMMCHECK_PRT);
	_pCommStateM->pMSys = pMSys;

	{
	CommMysteryInitNetLib( (void*)_pCommStateM );
	}


#if 0//OLD
	CommCommandMysteryInitialize((void *)pMSys);
	// ������
	_pCommStateM = (_COMM_STATE_WORK*)GFL_HEAP_AllocMemory(HEAPID_MYSTERYGIFT, sizeof(_COMM_STATE_WORK));
	MI_CpuFill8(_pCommStateM, 0, sizeof(_COMM_STATE_WORK));
	_pCommStateM->timer = _START_TIME;
	_pCommStateM->pTcb = GFL_TCB_AddTask(pMSys->tcblSys , _commCheckFunc, NULL, _TCB_COMMCHECK_PRT);
	_pCommStateM->pMSys = pMSys;
	CommRandSeedInitialize(&_pCommStateM->sRand);
#endif
}

//���̏ꏊ�Ń��C�u������������������K�v���o�Ă����̂�
void CommMysteryInitNetLib(void* pWork)
{
	GFLNetInitializeStruct mysteryGiftNetInit = {
		_CommPacketTbl,		//NetSamplePacketTbl,  // ��M�֐��e�[�u��
		CM_COMMAND_MAX,		// ��M�e�[�u���v�f��
    NULL,		///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,		///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,	// ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
		NULL,	// ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
		NULL,	//FIELD_COMM_FUNC_GetBeaconData,			// �r�[�R���f�[�^�擾�֐�  
		NULL,	//FIELD_COMM_FUNC_GetBeaconSize,			// �r�[�R���f�[�^�T�C�Y�擾�֐� 
		CommMysteryBeaconCheck,//FIELD_COMM_FUNC_CheckConnectService,	// �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
		NULL,//FIELD_COMM_FUNC_ErrorCallBack,			// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
    NULL,  //FatalError
    NULL,//FIELD_COMM_FUNC_DisconnectCallBack,	// �ʐM�ؒf���ɌĂ΂��֐�(�I����
		NULL,	// �I�[�g�ڑ��Őe�ɂȂ����ꍇ
		NULL,		///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
		NULL,		///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
		NULL,		///< wifi�t�����h���X�g�폜�R�[���o�b�N
		NULL,		///< DWC�`���̗F�B���X�g	
		NULL,		///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
		0,			///< DWC�̃q�[�v�T�C�Y
		TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
		0x333,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
		GFL_HEAPID_APP,  //���ɂȂ�heapid
		HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
		HEAPID_WIFI,  //wifi�p��create�����HEAPID
		HEAPID_NETWORK,	//
		GFL_WICON_POSX,GFL_WICON_POSY,	// �ʐM�A�C�R��XY�ʒu
		4,//_MAXNUM,	//�ő�ڑ��l��
		48,//_MAXSIZE,	//�ő呗�M�o�C�g��
		4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
		TRUE,		// CRC�v�Z
		FALSE,		// MP�ʐM���e�q�^�ʐM���[�h���ǂ���
		GFL_NET_TYPE_WIRELESS,		//�ʐM�^�C�v�̎w��
		TRUE,		// �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
		WB_NET_FIELDMOVE_SERVICEID,	//GameServiceID
		IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
    0,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
	};
	
	GFL_NET_Init( &mysteryGiftNetInit , NULL , pWork ); 

}


//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================
static void _changeState(PTRStateFunc state, int time)
{
	_pCommStateM->state = state;
	_pCommStateM->timer = time;
}


//==============================================================================
/**
 * �e�@�Ƃ��ď��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryParentInit(void)
{
	int i;
	//  MYSTERYGIFT_WORK *wk;
	
	// �܂��͎����Ɛڑ�������
	//TODO:�K�v�H
//	if(!CommIsConnect(CommGetCurrentID()))
//		return;

	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++)
		_pCommStateM->result_flag[i] = FALSE;
	// �e�@�Ƃ��Ă̏��������������̂ŁA�r�[�R������Y�t����
	//  wk = PROC_GetWork(MyseryGiftGetProcp());
	// �����̃f�[�^�͑���˂�
	//FIXME �K�X�u������
//	CommInfoSendPokeData();
	_CHANGE_STATE(_mysteryParentWaiting, 0);
}


//==============================================================================
/**
 * �e�@�Ƃ��đҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryParentWaiting(void)
{
	int i;
	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
//		const GFL_NETHANDLE *handle = GFL_NET_HANDLE_Get(i);
			//FIXME const�Ή���
//		if( _pCommStateM->status[i] == NULL && GFL_NET_HANDLE_IsNegotiation(handle) == TRUE)
			{
			//FIXME ���O�Ŏ�M�֐����v��
			//_pCommStateM->status[i] = CommInfoGetMyStatus(i);
			if(_pCommStateM->status[i])
			{
#if 0//def DEBUG_ONLY_FOR_mituhara
	OS_Printf("�q�@ %d ����ԐM������܂����I\n", i);
#endif
			}
		}
	}
}


//==============================================================================
/**
 * �q�@�̏�����
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryChildInit(void)
{
	if(GFL_NET_IsInit() == FALSE){
		return;
	}
	GFL_NET_StartBeaconScan();
	_CHANGE_STATE(_mysteryChildBconScanning, 0);
}


//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�r�[�R�����W��
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryChildBconScanning(void)
{
	//  CommParentBconCheck();
}


//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�ɋ����炢��
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryChildConnecting(void)
{
//	CommStateConnectBattleChild(_pCommStateM->connectIndex);
	u8* macAdr = GFL_NET_GetBeaconMacAddress(_pCommStateM->connectIndex);
	GFL_NET_ConnectToParent( macAdr );
	_CHANGE_STATE(_mysteryChildSendName, 0);
}


//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�ɏ��𑗐M
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryChildSendName(void)
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
//	if(!CommIsBattleConnectingState())
	if( GFL_NET_HANDLE_IsNegotiation(selfHandle) )
		return;

	_pCommStateM->recv_flag = 0;
	//FIXME �K�X�u������
//	CommInfoSendPokeData();
	GFL_NET_TimingSyncStart(selfHandle,MYSTERYGIFT_SYNC_CODE);
	_CHANGE_STATE(_mysteryChildWaiting, 0);
}


//==============================================================================
/**
 * �q�@�ҋ@���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _mysteryChildWaiting(void)
{
	//Exit���󂯎������q�@�ؒf
}




//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̏���
 * @param
 * @retval  none
 */
//==============================================================================
void _commCheckFunc(GFL_TCBL *tcb, void* work)
{
	if(_pCommStateM==NULL){
		GFL_TCBL_Delete(tcb);
	} else {
		if(_pCommStateM->state != NULL){
			PTRStateFunc state = _pCommStateM->state;
			if(!_pCommStateM->bStateNoChange){
	state();
			}
		}
	}
}


//==============================================================================
/**
 * �u�ӂ����Ȃ�������́v�̐e�Ƃ��Ă̒ʐM�����J�n
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================
void CommMysteryStateEnterGiftParent(MYSTERYGIFT_WORK *pMSys, SAVE_CONTROL_WORK *sv, int serviceNo)
{
	//TODO �K�v�H
//	if(CommIsInitialize())
//		return;      // �Ȃ����Ă���ꍇ���͏��O����

//	CommStateEnterMysteryParent(sv, serviceNo);
	_commStateInitialize(pMSys);
	GFL_NET_InitServer();
	_CHANGE_STATE(_mysteryParentInit, 0);
}

//==============================================================================
/**
 * ���������������q�̐���Ԃ�
 * @param   none
 * @retval  none
 */
//==============================================================================
int CommMysteryGetCommChild(void)
{
	int i, max;
	for(max = 0, i = 1; i < SCAN_PARENT_COUNT_MAX; i++)
		if(_pCommStateM->status[i])	max++;
	return max;
}

//==============================================================================
/**
 * �e�@�F�q�����Ă���q�@�փf�[�^�𑗂�
 * @param   none
 * @retval  none
 */
//==============================================================================
void CommMysterySendGiftDataParent(const void *p, int size)
{
//	CommSendHugeData_ServerSide(CM_GIFT_DATA, p, size);
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	GFL_NET_SendData( selfHandle , CM_GIFT_DATA , size , p );
}










//==============================================================================
/**
 * �u�ӂ����Ȃ�������́v�̎q�Ƃ��Ă̒ʐM�����J�n
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================
void CommMysteryStateEnterGiftChild(MYSTERYGIFT_WORK *pMSys, int serviceNo)
{
	SAVE_CONTROL_WORK *sv;
	//TODO �K�v�H
//	if(CommIsInitialize())
//		return;      // �Ȃ����Ă���ꍇ���͏��O����

	// �ʐM�q�[�v�쐬
//	sv = ((MAINWORK *)PROC_GetParentWork(MyseryGiftGetProcp()))->savedata;
	sv = SaveControl_GetPointer();
//	CommStateEnterMysteryChild(sv, serviceNo);
	_commStateInitialize(pMSys);
	_CHANGE_STATE(_mysteryChildInit, 0);
}


//==============================================================================
/**
 * �u�ӂ����Ȃ�������́v�̎q�Ƃ��Ă̒ʐM�����J�n
 * @param   connectIndex �ڑ�����e�@��Index
 * @retval  none
 */
//==============================================================================
void CommMysteryStateConnectGiftChild(int connectIndex)
{
	_pCommStateM->connectIndex = connectIndex;
	_CHANGE_STATE(_mysteryChildConnecting, 0);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�L���ȃr�[�R�������󂯎�������Ԃ�
 * @param	NONE
 * @return	0�`15: �󂯎���� -1: �󂯎��Ȃ�����
 *		���󂯎�����ꍇ��beacon_data�ɏ�񂪃R�s�[�����
*/
//--------------------------------------------------------------------------------------------
int CommMysteryCheckParentBeacon(MYSTERYGIFT_WORK *wk)
{
	int i;
	GIFT_BEACON *p;

	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++)
	{
		p = (GIFT_BEACON *)GFL_NET_GetBeaconData(i);
		if(p)
		{
			
			if(p->event_id)
			{
#if 0
	GFL_STD_MemCopy(p, &wk->gift_data.gd2.b, sizeof(GIFT_DATA2_B));
#endif
	GFL_STD_MemCopy(p, &wk->gift_data.beacon, sizeof(GIFT_BEACON));
#if 0//def DEBUG_ONLY_FOR_mituhara
	OS_Printf("�r�[�R���f�[�^�� %d �Ɏ󂯎��܂����I\n", i);
	OS_Printf("event_id = %d\n", p->event_id);
	OS_Printf("have_card = %d\n", p->have_card);
#endif
	return i;
			}
		}
	}
	return -1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�����ƃf�[�^���󂯎�ꂽ����e�@�֓`�B
 * @param	NONE
 * @return	NONE
 */
//--------------------------------------------------------------------------------------------
void CommMysteryResultRecvData(void)
{
	//CommSendData(CM_RECV_RESULT, NULL, 0);
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	GFL_NET_SendData( selfHandle , CM_RECV_RESULT , 0 , NULL );
}


//==============================================================================
/**
 * ��������́@�̃f�[�^���󂯎�����ۂɌĂ΂��R�[���o�b�N
 * @param   netID    ���M���Ă���ID
 * @param   size     �����Ă����f�[�^�T�C�Y
 * @param   pData    ��������̂ւ̃f�[�^�|�C���^
 * @retval  none
 */
//==============================================================================
void CommMysteryGiftRecvPlace(const int netID, const int size, const void* pData, void* pWork , GFL_NETHANDLE *pNetHandle)
{
	/* �m��Ȃ����肩��f�[�^�������Ă����疳�� */
	if(_pCommStateM->connectIndex != netID)
		return;
	_pCommStateM->recv_flag = 1;
#if 0//def DEBUG_ONLY_FOR_mituhara
	OS_Printf("��������̂̃f�[�^���󂯎��܂����I\n");
#endif
}

//==============================================================================
/**
 * @brief	��������̂̃f�[�^�������Ă������Ԃ�
 * @param	NONE
 * @return	TRUE: �����Ă��� : FALSE: �����Ă��Ă��Ȃ�
 */
//==============================================================================
int CommMysteryCheckRecvData(void)
{
	return _pCommStateM->recv_flag;
}

//==============================================================================
/**
 * ��������́@�̃f�[�^�T�C�Y��Ԃ�
 * @param   none
 * @retval  �f�[�^�T�C�Y
 */
//==============================================================================
int CommMysteryGetRecvPlaceSize(void)
{
	return sizeof(GIFT_DATA);
}

//==============================================================================
/**
 * ��������́@���i�[����|�C���^��Ԃ�
 * @param   none
 * @retval  �|�C���^
 */
//==============================================================================
u8* CommGetMysteryGiftRecvBuff( int netID, void* pWork, int size)
{
	return (u8 *)&_pCommStateM->recv_data;
}

//==============================================================================
/**
 * ��������́@�������Ă������Ƃ𑗂�
 * @param   none
 * @retval  �|�C���^
 */
//==============================================================================
void CommMysterySendRecvResult(const int netID,const  int size, const void* pBuff, void* pWork , GFL_NETHANDLE *pNetHandle)
{
#if 0//def DEBUG_ONLY_FOR_mituhara
	OS_Printf("%d %d %08X %08X\n", netID, size, pBuff, pWork);
#endif

	_pCommStateM->result_flag[netID] = TRUE;
}

//==============================================================================
/**
 * �ڑ�����Ă��邷�ׂĂ̎q�@����Ԏ����Ԃ��Ă������Ԃ�
 * @param   none
 * @retval  TRUE: �Ԃ��Ă��� : FALSE: �Ԃ��Ă��Ă��Ȃ�
 */
//==============================================================================
int CommMysteryGiftGetRecvCheck(void)
{
	int i;
	for(i = 1; i < SCAN_PARENT_COUNT_MAX; i++){
		if(/*CommInfoGetMyStatus(i) && */_pCommStateM->status[i] && _pCommStateM->result_flag[i] != TRUE)
			return FALSE;
	}
	return TRUE;
}


//==============================================================================
/**
 * �ʐM���I��������
 * @param   none
 * @retval  none
 */
//==============================================================================
void CommMysteryExitGift(void)
{
	GFL_HEAP_FreeMemory(_pCommStateM);
	_pCommStateM = NULL;
	GFL_NET_Exit(NULL);
//	CommStateExitBattle();
}

/*  */
