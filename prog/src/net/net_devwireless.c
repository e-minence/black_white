//=============================================================================
/**
 * @file	net_devwireless.c
 * @bfief	�����ʐM�h���C�o�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	08/12/19
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "net_whpipe.h"
#include "net/wih.h"

//--------------------------------------------
//�����֐��ꗗ
//--------------------------------------------
extern GFLNetDevTable *NET_GetWirelessDeviceTable(void);
static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork);
static BOOL _DevStartFunc(NetDevEndCallback callback);
static int _DevMainFunc(u16 bitmap);
static BOOL _DevEndFunc(BOOL bForce,NetDevEndCallback callback);
static BOOL _DevExitFunc(NetDevEndCallback callback);
static BOOL _DevChildWorkInitFunc(int NetID);
static BOOL _DevParentWorkInitFunc(void);
static BOOL _DevMyBeaconSetFunc(NetDevEndCallback callback);
static BOOL _DevBeaconSearchFunc(NetDevEndCallback callback);
static void* _DevBeaconGetFunc(int index);
static u8* _DevBeaconGetMacFunc(int index);
static BOOL _DevIsBeaconChangeFunc(NetDevEndCallback callback);
static BOOL _DevResetBeaconChangeFlgFunc(NetDevEndCallback callback);
static BOOL _DevIsChangeOverTurnFunc(NetDevEndCallback callback);

static BOOL _DevSetChildConnectCallbackFunc(PTRChildConnectCallbackFunc func);  ///< �q�����Ȃ������̃R�[���o�b�N���w�肷��
static BOOL _DevParentModeConnectFunc(BOOL bChannelChange, NetDevEndCallback callback); ///<DevParentModeConnect       �e�J�n
static BOOL _DevChildModeConnectFunc(BOOL bBconInit, NetDevEndCallback callback); ///<DevChildModeConnect        �q���J�n        �r�[�R��index + macAddress  �Ȃ��Ȃ��ꍇ-1
static BOOL _DevChildModeMacConnectFunc(BOOL bBconInit,void* pMacAddr,int index,NetDevEndCallback callback,PTRPARENTFIND_CALLBACK pCallback); ///<DevChildModeConnect        �q���J�n        �r�[�R��index + macAddress  �Ȃ��Ȃ��ꍇ-1
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit,NetDevEndCallback callback); ///<DevChangeOverModeConnect   �����_���ڑ�     �Ȃ��Ȃ��ꍇ-1
static BOOL _DevModeDisconnectFunc(BOOL bForce,NetDevEndCallback callback); ///<DevModeDisconnect          �ڑ����I���
static BOOL _DevIsStepDSFunc(void);  ///< �f�[�^�V�F�A�����O�������M�o�����Ԃ�
static BOOL _DevStepDSFunc(void* pSendData);  ///< �f�[�^�V�F�A�����O�������M
static void* _DevGetSharedDataAdrFunc(int index);

static BOOL _DevSendDataFunc(void* data,int size,int no,NetDevEndCallback callback); ///<DevSendData                ���M�֐�
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback); ///<DevRecvCallback            ��M�R�[���o�b�N

static BOOL _DevIsStartFunc(void); ///<DevIsStart                 �ʐM�ڑ����Ă邩�ǂ���
static BOOL _DevIsConnectFunc(void); ///<DevIsConnect               ����Ɛڑ����Ă邩�ǂ���
static BOOL _DevIsEndFunc(void); ///<DevIsEnd                 �ʐM�I�����Ă悢���ǂ���
static BOOL _DevIsIdleFunc(void); ///<�A�C�h����Ԃ��ǂ���
static u32 _DevGetBitmapFunc(void); ///<DevGetBitmap               �ڑ���Ԃ�BITMAP�ŕԂ�
static u32 _DevGetCurrentIDFunc(void); ///<DevGetCurrentID            �����̐ڑ�ID��Ԃ�
static int _DevGetIconLevelFunc(void);

static int _DevGetError(void);  ///< �G���[�𓾂�
static void _DevSetNoChildErrorSet(BOOL bOn);
static void _DevSetClinetConnect(BOOL bEnable);
static BOOL _DevCrossScan(void);
static void _DevCrossScanChangeSpeed(int num);

//--------------------------------------------
// �\����
//--------------------------------------------

static GFLNetDevTable netDevTbl={
	NULL,
	_DevInitFunc,
	_DevStartFunc,
	_DevMainFunc,
	_DevEndFunc,
	_DevExitFunc,
	_DevChildWorkInitFunc,
	_DevParentWorkInitFunc,
	_DevMyBeaconSetFunc,
	_DevBeaconSearchFunc,
	_DevBeaconGetFunc,
	_DevBeaconGetMacFunc,
	_DevIsBeaconChangeFunc,
	_DevResetBeaconChangeFlgFunc,
	_DevIsChangeOverTurnFunc,
	_DevSetChildConnectCallbackFunc,
	_DevParentModeConnectFunc,
	_DevChildModeConnectFunc,
	_DevChildModeMacConnectFunc,
	_DevChangeOverModeConnectFunc,
	NULL,
	_DevModeDisconnectFunc,
	_DevIsStepDSFunc,
	_DevStepDSFunc,
	_DevGetSharedDataAdrFunc,
	_DevSendDataFunc,
	_DevSetRecvCallbackFunc,
	_DevIsStartFunc,
	_DevIsConnectFunc,
	_DevIsEndFunc,
	_DevIsIdleFunc,
	_DevGetBitmapFunc,
	_DevGetCurrentIDFunc,
	_DevGetIconLevelFunc,
	_DevGetError,
	_DevSetNoChildErrorSet,
	NULL, //_DevIsConnectable
	NULL, //_DevIsVChat
	NULL, //_DevIsNewPlayer
	NULL, //_DevIrcMoveFunc

	NULL, //DevIsSendDataFunc DevIsSendData; ///< �f�[�^�𑗂��Ă悢���ǂ���
	NULL, //DevGetSendTurnFunc DevGetSendTurn; ///< ���M�\�^�[���t���O���擾
	NULL, //DevIsConnectSystemFunc DevIsConnectSystem;  ///< �Đڑ����ȂǊ֌W�Ȃ��A�����ɍ��A�q�����Ă��邩
	NULL, //DevGetSendLockFlagFunc DevGetSendLockFlag;  ///< ���M���b�N�t���O���擾
	NULL, //DevConnectWorkInitFunc DevConnectWorkInit;           ///<���߂Ă̐ڑ���̃��[�N�ݒ�
	_DevSetClinetConnect, //DevSetClientConnectFunc DevSetClientConnect; ///< �q�@���Ȃ����Ă悢���ǂ����n�[�h���x���Œ���
  _DevCrossScan, //����Ⴂ�ʐM
  _DevCrossScanChangeSpeed, //����Ⴂ���x�ύX
  NULL, //DevSetTimeOutFunc

#if 0
	NULL, //DevLobbyLoginFunc DevLobbyLogin;		///<Wi-Fi�L��Ƀ��O�C��	DWC_LOBBY_Login
	NULL, //DevDebugSetRoomFunc DevDebugSetRoom;	///<�f�o�b�O�p �����f�[�^�ݒ� DWC_LOBBY_DEBUG_SetRoomData
	NULL, //DevLobbyUpdateErrorCheckFunc DevLobbyUpdateErrorCheck;	///<DWC���r�[�X�V DWC_LOBBY_UpdateErrorCheck
	NULL, //DevLobbyLoginWaitFunc DevLobbyLoginWait;	///<DWC_LOBBY_LoginWait
	NULL, //DevLobbyLogoutFunc DevLobbyLogout;	///<DWC_LOBBY_Logout
	NULL, //DevLobbyLogoutWaitFunc DevLobbyLogoutWait;	///<DWC_LOBBY_LogoutWait
	NULL, //DevLobbyMgCheckRecruitFunc DevLobbyMgCheckRecruit;  ///<DWC_LOBBY_MG_CheckRecruit
	NULL, //DevLobbyMgStartRecruitFunc DevLobbyMgStartRecruit;  ///<DWC_LOBBY_MG_StartRecruit
	NULL, //DevLobbyMgEntryFunc DevLobbyMgEntry;  ///<DWC_LOBBY_MG_Entry
	NULL, //DevLobbyMgForceEndFunc DevLobbyMgForceEnd;  ///<DWC_LOBBY_MG_ForceEnd
	NULL, //DevLobbyMgEndConnectFunc DevLobbyMgEndConnect; ///<DWC_LOBBY_MG_EndConnect
	NULL, //DevLobbyMgMyParentFunc DevLobbyMgMyParent;  ///<DWC_LOBBY_MG_MyParent
	NULL, //DevLobbyMgEndRecruitFunc DevLobbyMgEndRecruit;  ///<DWC_LOBBY_MG_EndRecruit
#endif
};

//--------------------------------------------
// �������[�N
//--------------------------------------------

typedef struct{
	GFL_NETSYS* pNet;
	NetDevEndCallback callback;
} NetWork;


static NetWork* _pWork = NULL;


//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X�e�[�u����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
GFLNetDevTable *NET_GetWirelessDeviceTable(void)
{
	return &netDevTbl;
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�o�C�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork)
{
	GFLNetInitializeStruct* initStruct;
	BOOL isScanOnly;
	if(_pWork){
		return FALSE;  //���łɏ������ς�
	}
	_pWork = GFL_HEAP_AllocClearMemory(heapID, sizeof(NetWork));
	_pWork->pNet = pNet;
	initStruct = GFL_NET_GetNETInitStruct();
	isScanOnly = (initStruct->bNetType == GFL_NET_TYPE_WIRELESS_SCANONLY ? TRUE : FALSE );
	return GFL_NET_WLInitialize(heapID, callback, pUserWork,isScanOnly);
}


//------------------------------------------------------------------------------
/**
 * @brief   �ڑ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevStartFunc(NetDevEndCallback callback)
{
	//���C�����X�ɂ͐ڑ��͑��݂��Ȃ� IDLE��ԂȂ�OK
	if(callback)
		callback(GFL_NET_WLIsStateIdle());
	return GFL_NET_WLIsStateIdle();
}

//------------------------------------------------------------------------------
/**
 * @brief   ���C�����[�v
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevMainFunc(u16 bitmap)
{
  GFI_NET_MLProcess(bitmap);
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM����U�I������
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevEndFunc(BOOL bForce, NetDevEndCallback callback)
{
	return GFL_NET_WLFinalize();
}


//------------------------------------------------------------------------------
/**
 * @brief   �������J���I�[�o�[���C�J���̃R�[���o�b�N
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _callbackExitFunc(BOOL bReset)
{
	if(_pWork->callback){
		_pWork->callback(bReset);
	}
	GFL_HEAP_FreeMemory(_pWork);
	_pWork = NULL;
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �������J���I�[�o�[���C�J��
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevExitFunc(NetDevEndCallback callback)
{
	if(_pWork){
		_pWork->callback = callback;
		return GFI_NET_WHPipeEnd(_callbackExitFunc);
	}
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@�G���A������
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevChildWorkInitFunc(int NetID)
{
	GFL_NET_WLChildBconDataInit();
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �e�@�G���A������
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevParentWorkInitFunc(void)
{
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �����r�[�R����ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevMyBeaconSetFunc(NetDevEndCallback callback)
{
	return TRUE;
}
//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R����T��
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevBeaconSearchFunc(NetDevEndCallback callback)
{
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���𓾂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void* _DevBeaconGetFunc(int index)
{
	GFLNetInitializeStruct* initStruct;


  if(index >= SCAN_PARENT_COUNT_MAX){
		return NULL;
	}
	return GFL_NET_WLGetUserBss(index);
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���𓾂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u8* _DevBeaconGetMacFunc(int index)
{
	if(index >= SCAN_PARENT_COUNT_MAX){
		return NULL;
	}
	return GFL_NET_WLGetUserMacAddress(index);
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R�����ς�������ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsBeaconChangeFunc(NetDevEndCallback callback)
{
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���𓾂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevResetBeaconChangeFlgFunc(NetDevEndCallback callback)
{
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���𓾂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsChangeOverTurnFunc(NetDevEndCallback callback)
{
	return GFL_NET_WLIsParentBeaconSent();
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�����Ȃ������̃R�[���o�b�N���w�肷��
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevSetChildConnectCallbackFunc(PTRChildConnectCallbackFunc func)
{
	WHSetConnectCallBack(func);
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �e�J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevParentModeConnectFunc(BOOL bChannelChange, NetDevEndCallback callback)
{
	return GFL_NET_WL_ParentConnect(bChannelChange);

}
//------------------------------------------------------------------------------
/**
 * @brief   �q���J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevChildModeConnectFunc(BOOL bBconInit, NetDevEndCallback callback)
{
	return GFL_NET_WLChildInit(bBconInit);
}

//------------------------------------------------------------------------------
/**
 * @brief   MAC�A�h���X�w��Ŏq���J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevChildModeMacConnectFunc(BOOL bBconInit, void* macAddress,int index, NetDevEndCallback callback,PTRPARENTFIND_CALLBACK pCallback)
{
	return GFL_NET_WLChildMacAddressConnect(bBconInit, macAddress,index, pCallback);
}


//------------------------------------------------------------------------------
/**
 * @brief   �����_���ڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit,NetDevEndCallback callback)
{
	return FALSE;
}
//------------------------------------------------------------------------------
/**
 * @brief   �ڑ����I���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevModeDisconnectFunc(BOOL bForce,NetDevEndCallback callback)
{
	return GFL_NET_WLFinalize();
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�V�F�A�����O�������M�o�����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsStepDSFunc(void)
{
	return (WH_GetSystemState() == WH_SYSSTATE_DATASHARING);
}


//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�V�F�A�����O�������M
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevStepDSFunc(void* pSendData)
{
	return WH_StepDS(pSendData);
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�𑗂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void* _DevGetSharedDataAdrFunc(int index)
{
	return WH_GetSharedDataAdr(index);
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�𑗂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevSendDataFunc(void* data,int size,int no, NetDevEndCallback callback)
{
	return GFL_NET_WL_SendData( data, size, callback);
}

//------------------------------------------------------------------------------
/**
 * @brief   ��M�R�[���o�b�N�̐ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback)
{
	GFL_NET_WLSetRecvCallback(recvCallback);
	return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�ڑ����Ă邩�ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsStartFunc(void)
{
	switch(WH_GetSystemState()){
		//  WH_SYSSTATE_STOP,                  // �������
	case WH_SYSSTATE_IDLE:                  // �ҋ@��
	case WH_SYSSTATE_SCANNING:              // �X�L������
	case WH_SYSSTATE_BUSY:                  // �ڑ���ƒ�
	case WH_SYSSTATE_CONNECTED:             // �ڑ������i���̏�ԂŒʐM�\�j
	case WH_SYSSTATE_DATASHARING:           // data-sharing�L���Őڑ�����
	case WH_SYSSTATE_KEYSHARING_NONE:       // key-sharing�L���Őڑ�����  (�g���Ă��Ȃ�)
	case WH_SYSSTATE_MEASURECHANNEL:        // �`�����l���̓d�g�g�p�����`�F�b�N
	case WH_SYSSTATE_CONNECT_FAIL:          // �e�@�ւ̐ڑ����s
	case WH_SYSSTATE_ERROR:                 // �G���[����
	case WH_SYSSTATE_FATAL:                 // FATAL�G���[����
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------
/**
 * @brief   ����Ɛڑ����Ă邩�ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsConnectFunc(void)
{
	return GFL_NET_WL_IsConnectLowDevice();
}
//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�I�����Ă悢���ǂ���
 * @retval  TRUE  �ʐM�I�����Ă悢
 */
//------------------------------------------------------------------------------
static BOOL _DevIsEndFunc(void)
{
	return !GFL_NET_WLIsInitialize();
}


//------------------------------------------------------------------------------
/**
 * @brief   �������Ă��Ȃ��@���̏�Ԃɂ������Ԃ��ǂ����H
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsIdleFunc(void)
{
	switch(WH_GetSystemState()){
	case WH_SYSSTATE_IDLE:
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ���Ԃ�BITMAP�ŕԂ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u32 _DevGetBitmapFunc(void)
{
	return GFL_NET_WL_GetBitmap();
}
//------------------------------------------------------------------------------
/**
 * @brief   ������NetID��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u32 _DevGetCurrentIDFunc(void)
{
	return GFL_NET_WL_GetCurrentAid();
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�R�����x����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevGetIconLevelFunc(void)
{
	return (WM_LINK_LEVEL_3 - WM_GetLinkLevel());
}

//------------------------------------------------------------------------------
/**
 * @brief   �G���[�ԍ���Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevGetError(void)  ///< �G���[�𓾂�
{
	int err = WH_GetLastError();

  if(err == WM_ERRCODE_NO_DATASET){
    err = WM_ERRCODE_SUCCESS;
  }
  if(GFL_NET_WLIsError() && (err == 0)){
		err = WH_ERRCODE_DISCONNECTED;
	}
	return err;
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _DevSetNoChildErrorSet(BOOL bOn)
{
	GFL_NET_WLSetDisconnectOtherError(bOn);
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@���Ȃ����Ă悢���ǂ����n�[�h���x���Œ�������
 * @param   bEnable TRUE=����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _DevSetClinetConnect(BOOL bEnable)
{
	GFL_NET_WHPipeSetClientConnect(bEnable);
}

//------------------------------------------------------------------------------
/**
 * @brief   ����Ⴂ�ʐM�J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevCrossScan(void)
{
  return GFL_NET_WLCrossoverInit();
}


//------------------------------------------------------------------------------
/**
 * @brief   ����Ⴂ�ʐM���x�ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _DevCrossScanChangeSpeed(int num)
{
  GFL_NET_WLChangeScanSpeed(num);
}



