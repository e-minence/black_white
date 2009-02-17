//==============================================================================
/**
 * @file	compati_comm.c
 * @brief	�����`�F�b�N�F�ʐM
 * @author	matsuda
 * @date	2009.02.10(��)
 */
//==============================================================================
#include <gflib.h>
#include "system\main.h"
#include "net\network_define.h"

#include "compati_types.h"
#include "compati_tool.h"
#include "compati_comm.h"


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
static void _initCallBack(void* pWork);
static void _CommSystemEndCallback(void* pWork);
static void _connectCallBack(void* pWork, int netID);
static void _endCallBack(void* pWork);
static void _RecvFirstData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvResultData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

//==============================================================================
//	�f�[�^
//==============================================================================
///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _CompatiCommPacketTbl[] = {
    {_RecvFirstData,         NULL},    ///NET_CMD_FIRST
    {_RecvResultData,          NULL},  ///NET_CMD_RESULT
};

enum{
	NET_CMD_FIRST = GFL_NET_CMD_COMPATI_CHECK,
	NET_CMD_RESULT,
};

#define _MAXNUM   (2)         // �ő�ڑ��l��
#define _MAXSIZE  (32)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��


static const GFLNetInitializeStruct aGFLNetInit = {
    _CompatiCommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CompatiCommPacketTbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    _connectCallBack,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_COMPATI_CHECK,  //GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
};


//--------------------------------------------------------------
/**
 * @brief   �ʐM������
 * @param   commsys		
 * @retval  TRUE:���������@FALSE:�����p����
 */
//--------------------------------------------------------------
BOOL CompatiComm_Init(COMPATI_CONNECT_SYS *commsys, u32 irc_timeout)
{
	switch(commsys->seq){
	case 0:
		{
			GFLNetInitializeStruct net_ini = aGFLNetInit;
			
			net_ini.irc_timeout = irc_timeout;
			if(irc_timeout == IRC_TIMEOUT_STANDARD){
//				net_ini.bNetType = GFL_NET_TYPE_IRC;
			}
			GFL_NET_Init(&net_ini, _initCallBack, commsys);	//�ʐM������
		}
		commsys->seq++;
		break;
	case 1:
		if(GFL_NET_IsInit() == TRUE){	//�������I���҂�
			commsys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�ڑ�
 * @param   commsys		
 * @retval  TRUE:���������@FALSE:�����p����
 */
//--------------------------------------------------------------
BOOL CompatiComm_Connect(COMPATI_CONNECT_SYS *commsys)
{
	switch(commsys->seq){
	case 0:
		GFL_NET_ChangeoverConnect(NULL); // �����ڑ�
		commsys->seq++;
		break;
	case 1:
		//�����ڑ��҂�
		if(commsys->connect_ok == TRUE){
			OS_TPrintf("�ڑ�����\n");
			commsys->seq++;
		}
		break;
	case 2:		//�^�C�~���O�R�}���h���s
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15);
		commsys->seq++;
		break;
	case 3:
		if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),15) == TRUE){
			OS_TPrintf("�^�C�~���O��萬��\n");
			OS_TPrintf("�ڑ��l�� = %d\n", GFL_NET_GetConnectNum());
			commsys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�ؒf
 * @param   commsys		
 * @retval  TRUE:���������@FALSE:�����p����
 */
//--------------------------------------------------------------
BOOL CompatiComm_Shoutdown(COMPATI_CONNECT_SYS *commsys)
{
	switch(commsys->seq){
	case 0:	//�ʐM�I��
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL)){
			commsys->seq++;
		}
		break;
	case 1:	//�ʐM�I���҂�
		if(commsys->connect_ok == FALSE){
			commsys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�V�X�e���I��
 *
 * @param   commsys		
 *
 * @retval  TRUE:���������@FALSE:�����p����
 */
//--------------------------------------------------------------
BOOL CompatiComm_Exit(COMPATI_CONNECT_SYS *commsys)
{
#if 0
	return TRUE;
#else
	switch(commsys->seq){
	case 0:
		GFL_NET_Exit(_CommSystemEndCallback);
		commsys->seq++;
		break;
	case 1:
		if(commsys->lib_finish == TRUE){
			commsys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�F�ŏ��̃f�[�^����M
 * @param   commsys		
 * @retval  TRUE:���������@FALSE:�����p����
 */
//--------------------------------------------------------------
BOOL CompatiComm_FirstSend(COMPATI_CONNECT_SYS *commsys)
{
	switch(commsys->seq){
	case 0:
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
				NET_CMD_FIRST, sizeof(CCNET_FIRST_PARAM), commsys->send_first_param)){
			OS_TPrintf("�ŏ��̃f�[�^���M����\n");
			commsys->seq++;
		}
		break;
	case 1:	//����̃f�[�^�������Ă���̂�҂�
		if(commsys->receive_ok == TRUE){
			OS_TPrintf("�ŏ��̃f�[�^��M����\n");
			commsys->receive_ok = FALSE;
			commsys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�F�Ō�̃f�[�^����M
 * @param   commsys		
 * @retval  TRUE:���������@FALSE:�����p����
 */
//--------------------------------------------------------------
BOOL CompatiComm_ResultSend(COMPATI_CONNECT_SYS *commsys)
{
	switch(commsys->seq){
	case 0:
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
				NET_CMD_RESULT, sizeof(CCNET_RESULT_PARAM), commsys->send_result_param)){
			OS_TPrintf("�Ō�̃f�[�^���M����\n");
			commsys->seq++;
		}
		break;
	case 1:	//����̃f�[�^�������Ă���̂�҂�
		if(commsys->receive_ok == TRUE){
			OS_TPrintf("�Ō�̃f�[�^��M����\n");
			commsys->receive_ok = FALSE;
			commsys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}


//==============================================================================
//	
//==============================================================================
typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_COMPATI_CHECK };

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void* _netBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _initCallBack(void* pWork)
{
	OS_TPrintf("�����������R�[���o�b�N���Ăяo���ꂽ\n");
	return;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�V�X�e���I���R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _CommSystemEndCallback(void* pWork)
{
	COMPATI_CONNECT_SYS *commsys = pWork;
	
	OS_TPrintf("�ʐM�V�X�e���I���R�[���o�b�N���Ăяo���ꂽ\n");
	commsys->lib_finish = TRUE;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWork, int netID)
{
	COMPATI_CONNECT_SYS *commsys = pWork;
	u32 temp;
	
    OS_TPrintf("�l�S�V�G�[�V�������� netID = %d\n", netID);
	commsys->connect_bit |= 1 << netID;
	temp = commsys->connect_bit;
	if(MATH_CountPopulation(temp) >= 2){
		OS_TPrintf("�S���̃l�S�V�G�[�V�������� �l��bit=%d\n", commsys->connect_bit);
		commsys->connect_ok = TRUE;
	}
}

static void _endCallBack(void* pWork)
{
	COMPATI_CONNECT_SYS *commsys = pWork;

    OS_TPrintf("endCallBack�I��\n");
    commsys->connect_ok = FALSE;
    commsys->connect_bit = 0;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ړ��R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvFirstData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMPATI_CONNECT_SYS *commsys = pWork;
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if(netID == GFL_NET_SystemGetCurrentID()){
		return;	//�����̃f�[�^�͖���
	}

	GFL_STD_MemCopy(pData, commsys->receive_first_param, size);
	commsys->receive_ok = TRUE;
	OS_TPrintf("�ŏ��̃f�[�^��M�R�[���o�b�N netID = %d\n", netID);
}

//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------
static void _RecvResultData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	COMPATI_CONNECT_SYS *commsys = pWork;
	
	if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if(netID == GFL_NET_SystemGetCurrentID()){
		return;	//�����̃f�[�^�͖���
	}

	GFL_STD_MemCopy(pData, commsys->receive_result_param, size);
	commsys->receive_ok = TRUE;
	OS_TPrintf("�Ō�̃f�[�^��M�R�[���o�b�N netID = %d\n", netID);
}


