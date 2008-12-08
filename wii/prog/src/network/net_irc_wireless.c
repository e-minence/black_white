//==============================================================================
/**
 * @file	net_irc_wiress.c
 * @brief	�ԊO���ʐM�Ń}�b�`���O��A���C�����X�ʐM�ֈڍs
 * @author	matsuda
 * @date	2008.11.11(��)
 */
//==============================================================================
#include "gflib.h"

#include "ui/ui_def.h"
#include "net_def.h"
#include "net_local.h"
#include "device/net_whpipe.h"
#include "device/dwc_rapcommon.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "wm_icon.h"
#include "device/dwc_rap.h"


#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"
#include "net_irc.h"
#include "net_irc_wireless.h"


//==============================================================================
//	�\���̒�`
//==============================================================================
///�ԊO�����C�����X�Ǘ����[�N
typedef struct{
	GFLNetInitializeStruct aNetInit;
	void *user_pWork;
	
	NetConnectEndFunc user_connectEndFunc;		///<�ʐM�ؒf���ɌĂ΂��֐�(���[�U�[�ޔ�)
	
	NetStepEndCallback connect_callback;///<�S�Ă̐ڑ�������ɌĂ΂��R�[���o�b�N(���[�U�[�w��)
	
	BOOL wireless_shift;	///<TRUE:���C�����X�ʐM�ֈڍs��
	
	BOOL irc_matching;		///<TRUE:�ԊO���}�b�`���O����
	BOOL irc_shutdown;		///<TRUE:�ԊO���ؒf����
	int net_system_id;		///<�ԊO���}�b�`���O�������̎����̃}�V����NetID
	
	u8 parent_MacAddress[6];	///<�e��Mac�A�h���X
	u8 padding[2];
	
	int seq;
	int timer;
}NET_IW_SYS;

//==============================================================================
//	���[�J���ϐ�
//==============================================================================
static NET_IW_SYS *NetIwSys = NULL;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void Local_IrcConnectCallback(void *pWork);
static void Local_IrcConnectEndFunc(void *pWork);



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ԊO�����C�����X�ڍs�V�X�e�����쐬
 *
 * @param   pNetInit		
 * @param   callback		
 * @param   pWork			
 */
//--------------------------------------------------------------
const GFLNetInitializeStruct * GFL_NET_IW_SystemCreate(const GFLNetInitializeStruct* pNetInit, void* pWork)
{
	NET_IW_SYS *iw;
	
	GF_ASSERT(NetIwSys == NULL || NetIwSys->wireless_shift == TRUE);
	
	if(NetIwSys != NULL && NetIwSys->wireless_shift == TRUE){
		return pNetInit;	//�ԊO�����烏�C�����X�ʐM�ֈڍs���B���ɃV�X�e���͍쐬�ς�
	}
	
    iw = GFL_HEAP_AllocClearMemoryLo(pNetInit->baseHeapID, sizeof(NET_IW_SYS));
    iw->aNetInit = *pNetInit;
    iw->user_pWork = pWork;
    iw->user_connectEndFunc = pNetInit->connectEndFunc;
    
    //�ŏ��͐ԊO���ʐM�Őڑ�����悤��GFLNetInitializeStruct������������
    iw->aNetInit.bNetType = GFL_NET_TYPE_IRC;
	iw->aNetInit.connectEndFunc = Local_IrcConnectEndFunc;
	
    NetIwSys = iw;
    return &NetIwSys->aNetInit;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO�����C�����X�ڍs�V�X�e����j��
 */
//--------------------------------------------------------------
void GFL_NET_IW_SystemExit(void)
{
	if(NetIwSys != NULL && NetIwSys->wireless_shift == FALSE){
		GFL_HEAP_FreeMemory(NetIwSys);
		NetIwSys = NULL;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO�����C�����X�ڍs�V�X�e�����쐬����Ă��邩�m�F
 * @retval  TRUE:�쐬����Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IW_SystemCheck(void)
{
	if(NetIwSys != NULL){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ڑ������C�����X�ʐM�ڍs�����A�܂őS�Ă��I��������Ăяo���R�[���o�b�N�֐���o�^
 *
 * @param   callback		�R�[���o�b�N�֐��ւ̃|�C���^
 *
 * @retval  �ŏ��̐ԊO���}�b�`���O�������ɌĂ΂������̃R�[���o�b�N�֐��ւ̃|�C���^
 */
//--------------------------------------------------------------
NetStepEndCallback GFL_NET_IW_ConnectCallbackSetting(NetStepEndCallback callback)
{
	if(NetIwSys != NULL){
		NetIwSys->connect_callback = callback;
		return Local_IrcConnectCallback;
	}
	return callback;
}

//--------------------------------------------------------------
/**
 * @brief   �e��MAC�A�h���X��ۑ�����
 *
 * @param   mac_address		�e��MAC�A�h���X
 */
//--------------------------------------------------------------
void GFL_NET_IW_ParentMacAddressSet(const u8 *mac_address)
{
	if(NetIwSys != NULL){
		GFL_STD_MemCopy(mac_address, NetIwSys->parent_MacAddress, 6);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���Ń}�b�`���O������ɌĂ΂��R�[���o�b�N�֐�
 * @param   pWork		���[�U�[��GFL_NET_Init�Ŏw�肵�Ă��郏�[�N�ւ̃|�C���^
 * 						���ׁ̈A���̊֐����ł͎g�p���Ȃ�
 */
//--------------------------------------------------------------
static void Local_IrcConnectCallback(void *pWork)
{
	OS_TPrintf("IW:�ԊO���}�b�`���O����\n");
	NetIwSys->irc_matching = TRUE;
	NetIwSys->net_system_id = GFL_NET_SystemGetCurrentID();	//�ԊO����ؒf����O��ID�擾
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO����ؒf������Ă΂��R�[���o�b�N�֐�
 * @param   pWork		���[�U�[��GFL_NET_Init�Ŏw�肵�Ă��郏�[�N�ւ̃|�C���^
 * 						���ׁ̈A���̊֐����ł͎g�p���Ȃ�
 */
//--------------------------------------------------------------
static void Local_IrcConnectEndFunc(void *pWork)
{
	OS_TPrintf("IW:�ԊO���ؒf\n");
	NetIwSys->irc_shutdown = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO�����烏�C�����X�ʐM�ւ̈ڍs�A���C���v���Z�X
 */
//--------------------------------------------------------------
void GFL_NET_IW_Main(void)
{
	NET_IW_SYS *iw;
	
	if(NetIwSys == NULL){
		return;
	}
	
	iw = NetIwSys;
	
	switch(iw->seq){
	case 0:
		//�ԊO���}�b�`���O��҂��āA���̌�ؒf���N�G�X�g�𑗐M
		if(iw->irc_matching == TRUE){
			NetIwSys->wireless_shift = TRUE;	//���[�N���������Ȃ��悤�ɁB
			if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL)){
				iw->seq++;
			}
		}
		break;
	case 1:
	#if 0	//��check�@����GFL_NET_CMD_EXIT_REQ�Őؒf�R�[���o�b�N�͌Ă΂�Ȃ��̂ŁA
			//�b��I�Ƀ^�C�}�[�Őؒf�������ɂ���
		//�ԊO���ؒf��҂��A���C�����X�ʐM���n�߂�
		if(iw->irc_shutdown == TRUE){
	#else
		iw->timer++;
		if(iw->timer > 60){
			iw->timer = 0;
	#endif
			iw->aNetInit.bNetType = GFL_NET_TYPE_WIRELESS;
			iw->aNetInit.connectEndFunc = iw->user_connectEndFunc;
			GFL_NET_Init(&iw->aNetInit, NULL, iw->user_pWork);
			NetIwSys->wireless_shift = FALSE;	//���C�����X�̏����ݒ�͒ʂ����̂ŁB
			iw->seq++;
		}
		break;
	case 2:	//�������҂�
		if(GFL_NET_IsInit() == TRUE){
			if(iw->net_system_id == GFL_NET_NO_PARENTMACHINE){
				GFL_NET_InitServer();
				OS_TPrintf("IW:�e�@�ɂȂ��ă��C�����X�J�n\n");
			}
			else{
				GFL_NET_InitClientAndConnectToParent(iw->parent_MacAddress);
				OS_TPrintf("IW:�q�@�ɂȂ��ă��C�����X�J�n\n");
			}
			iw->seq++;
		}
		break;
	case 3:	//���C�����X�ڑ��҂�
		//��check�@����ڑ����������f�����i���Ȃ��̂ŁA�b��I�Ƀ^�C�}�[�Ōq���������ɂ���
		iw->timer++;
		if(iw->timer > 60*3){
			iw->timer = 0;
			//�l�S�V�G�[�V�����J�n
			OS_TPrintf("IW:�l�S�V�G�[�V�������M\n");
			if(GFL_NET_HANDLE_RequestNegotiation() == TRUE){
				iw->seq++;
			}
		}
		break;
	case 4:	//�l�S�V�G�[�V���������҂�
		if(GFL_NET_HANDLE_GetNumNegotiation() >= 2){	//IRC����̈ڍs�Ȃ̂�2�l�Œ�
			OS_TPrintf("IW:�l�S�V�G�[�V����������\n");
	        if(iw->connect_callback){
				iw->connect_callback(iw->user_pWork);
			}
			GFL_NET_IW_SystemExit();
			return;
		}
		break;
	}
}

