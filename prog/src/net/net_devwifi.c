//=============================================================================
/**
 * @file	net_devwifi.c
 * @bfief	WIFI�ʐM�h���C�o�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	08/12/22
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "dwc_rap.h"
#include "dwc_rapinitialize.h"
#include "dwc_rapcommon.h"
#include "dwc_rapfriend.h"

extern GFLNetDevTable *NET_GetWifiDeviceTable(void);

static void _DevBootFunc(HEAPID heapID, NetErrorFunc errorFunc);
static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork);
static int _DevStartFunc(NetDevEndCallback callback);
static int _DevMainFunc(u16 bitmap);
static BOOL _DevEndFunc(BOOL bForce,NetDevEndCallback callback);
static BOOL _DevExitFunc(NetDevEndCallback callback);
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit,NetDevEndCallback callback);
static BOOL _DevWifiConnectFunc(int index, int maxnum, BOOL bVCT);
static BOOL _DevModeDisconnectFunc(BOOL bForce, NetDevEndCallback callback);
static BOOL _DevSendDataFunc(void* data,int size, int no,NetDevEndCallback callback);
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback);
static BOOL _DevIsStartFunc(void);
static BOOL _DevIsConnectFunc(void);
static BOOL _DevIsIdleFunc(void); ///<�A�C�h����Ԃ��ǂ���
static u32 _DevGetBitmapFunc(void);
static u32 _DevGetCurrentIDFunc(void);
static int _DevGetIconLevelFunc(void);
static int _DevGetError(void);  ///< �G���[�𓾂�
static void _DevSetNoChildErrorSet(BOOL bOn);
static BOOL _DevIsConnectable(int index);
static BOOL _DevIsVChat(void);
static BOOL _DevIsNewPlayer(void);


//--------------------------------------------
// �\����
//--------------------------------------------

static GFLNetDevTable netDevTbl={
    _DevBootFunc,
    _DevInitFunc,
    _DevStartFunc,
    _DevMainFunc,
    _DevEndFunc,
    _DevExitFunc,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    _DevChangeOverModeConnectFunc,
    _DevWifiConnectFunc,
    _DevModeDisconnectFunc,
    NULL,
    NULL,
    NULL,
    _DevSendDataFunc,
    _DevSetRecvCallbackFunc,
    _DevIsStartFunc,
    _DevIsConnectFunc,
    _DevIsIdleFunc,
    _DevGetBitmapFunc,
    _DevGetCurrentIDFunc,
    _DevGetIconLevelFunc,
    _DevGetError,
    _DevSetNoChildErrorSet,
    _DevIsConnectable,
    _DevIsVChat,
    _DevIsNewPlayer,
    NULL, //_DevIrcMoveFunc
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
 * @brief   DS�N�����ɍs������
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _DevBootFunc(HEAPID heapID, NetErrorFunc errorFunc)
{
    GFL_NET_WifiStart( heapID, errorFunc );
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�o�C�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork)
{
    if(_pWork){
        return FALSE;  //���łɏ������ς�
    }
    _pWork = GFL_HEAP_AllocClearMemory(heapID, sizeof(NetWork));
    _pWork->pNet = pNet;
    return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ�  WIFI�ڑ��܂ł��
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevStartFunc(NetDevEndCallback callback)
{
    int ret = mydwc_startConnect( GFI_NET_GetMyDWCUserData() ,GFI_NET_GetMyDWCFriendData());
    if(callback)
        callback(TRUE);
    //    mydwc_setFetalErrorCallback(CommFatalErrorFunc);   //@@OO �G���[�����ǉ��K�v 07/02/22
    return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���C�����[�v
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _DevMainFunc(u16 bitmap)
{
    return mydwc_stepmatch(bitmap);
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM����U�I������
 * @param   bForce �����I���������ꍇTRUE
 * @retval  TRUE ���� FALSE ���s���Ăё�����K�v������
 */
//------------------------------------------------------------------------------
static BOOL _DevEndFunc(BOOL bForce, NetDevEndCallback callback)
{
    if(mydwc_disconnect( !bForce ) ){
        if(mydwc_returnLobby()){
            return TRUE;
        }
    }
    else{
        mydwc_stepmatch(bForce);
    }
    return FALSE;
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
        mydwc_Logout();  // �ؒf
        if(callback){
            callback(TRUE);
        }
        GFL_HEAP_FreeMemory(_pWork);
        _pWork = NULL;
    }
    return TRUE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �����_���ڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit, NetDevEndCallback callback)
{
    return GFL_NET_DWC_StartMatch( keyStr, numEntry,  bParent, timelimit );
}

//------------------------------------------------------------------------------
/**
 * @brief   WIFI�F�l�w��Őڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevWifiConnectFunc(int index, int maxnum, BOOL bVCT)
{
    return GFL_NET_DWC_StartGame(index ,maxnum, bVCT );
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ����I���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevModeDisconnectFunc(BOOL bForce, NetDevEndCallback callback)
{
    if(mydwc_disconnect( !bForce )){
        mydwc_returnLobby();
        return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�𑗂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevSendDataFunc(void* data, int size, int no, NetDevEndCallback callback)
{
    if(GFL_NET_DWC_SendToOther(data, size)){
        if(callback){
            callback(TRUE);
        }
        return TRUE;
    }
    return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ��M�R�[���o�b�N�̐ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback)
{
    mydwc_setReceiver(recvCallback,recvCallback);
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
    return GFL_NET_DWC_IsInit();
}

//------------------------------------------------------------------------------
/**
 * @brief   ����Ɛڑ����Ă邩�ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsConnectFunc(void)
{
    return (MYDWC_NONE_AID != GFL_NET_DWC_GetAid());
}

//------------------------------------------------------------------------------
/**
 * @brief   �������Ă��Ȃ��@���̏�Ԃɂ������Ԃ��ǂ����H
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsIdleFunc(void)
{
    return mydwc_IsLogin();
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ���Ԃ�BITMAP�ŕԂ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u32 _DevGetBitmapFunc(void)
{
    return DWC_GetAIDBitmap();
}



//------------------------------------------------------------------------------
/**
 * @brief   ������NetID��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u32 _DevGetCurrentIDFunc(void)
{
    return GFL_NET_DWC_GetAid();
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�R�����x����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevGetIconLevelFunc(void)
{
    return (WM_LINK_LEVEL_3 - DWC_GetLinkLevel());
}

//------------------------------------------------------------------------------
/**
 * @brief   �G���[�ԍ���Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevGetError(void)  ///< �G���[�𓾂�
{
	int errorCode;
	DWCErrorType myErrorType;
	int ret;

    ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
    
    if(ret == DWC_ERROR_NONE){
        return 1;
    }
    return errorCode;  //WIFI�ł͂O�Ԃ��A�邱�Ƃ����邽��
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _DevSetNoChildErrorSet(BOOL bOn)
{
//    GFL_NET_WLSetDisconnectOtherError(bOn);
}


//------------------------------------------------------------------------------
/**
 * @brief   ���肪�Ȃ��ł悢��Ԃ��ǂ��� (�ق�friendWifi�p)
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevIsConnectable(int index)
{
    if( mydwc_getFriendStatus(index) != DWC_STATUS_MATCH_SC_SV ){
        return FALSE;
    }
    //    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
    return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�C�X�`���b�g��Ԃ��ǂ��� (�ق�friendWifi�p)
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevIsVChat(void)
{
    return mydwc_IsVChat();
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ����Ă������ǂ�����Ԃ��܂�
 * @retval  TRUE�c�ڑ��J�n�Ȃ̂ŃL�[������u���b�N   FALSE�c
 */
//------------------------------------------------------------------------------
static BOOL _DevIsNewPlayer(void)
{
    return GFL_NET_DWC_IsNewPlayer();
}


//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X�e�[�u����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
GFLNetDevTable *NET_GetWifiDeviceTable(void)
{
    return &netDevTbl;
}

