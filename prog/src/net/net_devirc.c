//=============================================================================
/**
 * @file	net_devirc.c
 * @bfief	IRC�ʐM�h���C�o�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	08/12/22
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"

//--------------------------------------------
//�����֐��ꗗ
//--------------------------------------------
extern GFLNetDevTable *NET_GetIrcDeviceTable(void);
static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork);
static BOOL _DevStartFunc(NetDevEndCallback callback);
static int _DevMainFunc(u16 bitmap);
static BOOL _DevEndFunc(BOOL bForce,NetDevEndCallback callback);
static BOOL _DevExitFunc(NetDevEndCallback callback);
static BOOL _DevChildWorkInitFunc(int NetID);
static BOOL _DevParentWorkInitFunc(void);
static BOOL _DevIsChangeOverTurnFunc(NetDevEndCallback callback);

static BOOL _DevSetChildConnectCallbackFunc(PTRChildConnectCallbackFunc func);  ///< �q�����Ȃ������̃R�[���o�b�N���w�肷��
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit,NetDevEndCallback callback); ///<DevChangeOverModeConnect   �����_���ڑ�     �Ȃ��Ȃ��ꍇ-1
static BOOL _DevModeDisconnectFunc(BOOL bForce,NetDevEndCallback callback); ///<DevModeDisconnect          �ڑ����I���
static BOOL _DevIsStepDSFunc(void);  ///< �f�[�^�V�F�A�����O�������M�o�����Ԃ�
static BOOL _DevStepDSFunc(void* pSendData);  ///< �f�[�^�V�F�A�����O�������M
static void* _DevGetSharedDataAdrFunc(int index);

static BOOL _DevSendDataFunc(void* data,int size,int no,NetDevEndCallback callback); ///<DevSendData                ���M�֐�
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback); ///<DevRecvCallback            ��M�R�[���o�b�N

static BOOL _DevIsStartFunc(void); ///<DevIsStart                 �ʐM�ڑ����Ă邩�ǂ���
static BOOL _DevIsConnectFunc(void); ///<DevIsConnect               ����Ɛڑ����Ă邩�ǂ���
static BOOL _DevIsIdleFunc(void); ///<�A�C�h����Ԃ��ǂ���
static u32 _DevGetBitmapFunc(void); ///<DevGetBitmap               �ڑ���Ԃ�BITMAP�ŕԂ�
static u32 _DevGetCurrentIDFunc(void); ///<DevGetCurrentID            �����̐ڑ�ID��Ԃ�
static int _DevGetErrorFunc(void);


static void _DevSetNoChildErrorSet(BOOL bOn);
static void _DevIrcMoveFunc(void);
static BOOL _DevIsSendDataFunc(void);
static BOOL _DevGetSendTurnFunc(void);
static BOOL _DevIsConnectSystemFunc(void);
static BOOL _DevGetSendLockFlagFunc(void);
static void _DevConnectWorkInitFunc(void);



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
    NULL, //_DevChildWorkInitFunc,
    NULL, //_DevParentWorkInitFunc,
    NULL, //_DevMyBeaconSetFunc,
    NULL,
    NULL, //_DevBeaconGetFunc,
    NULL, //_DevBeaconGetMacFunc,
    NULL, //_DevIsBeaconChangeFunc,
    NULL, //_DevResetBeaconChangeFlgFunc,
    NULL, //_DevIsChangeOverTurnFunc,
    NULL, //_DevSetChildConnectCallbackFunc,
    NULL,
    NULL,
    NULL,
    NULL, //_DevChangeOverModeConnectFunc,
    NULL,
    NULL, //_DevModeDisconnectFunc,
    NULL, //_DevIsStepDSFunc,
    NULL, //_DevStepDSFunc,
    NULL, //_DevGetSharedDataAdrFunc,
    _DevSendDataFunc,
    _DevSetRecvCallbackFunc,
    _DevIsStartFunc,
    _DevIsConnectFunc,
    _DevIsIdleFunc,
    NULL, //_DevGetBitmapFunc,
    _DevGetCurrentIDFunc,
    NULL,
    _DevGetErrorFunc, //_DevGetError,
    NULL, //_DevSetNoChildErrorSet,
    NULL, //_DevIsConnectable
    NULL, //_DevIsVChat
    NULL, //_DevIsNewPlayer
    _DevIrcMoveFunc,
    _DevIsSendDataFunc,
    _DevGetSendTurnFunc,
    _DevIsConnectSystemFunc,
    _DevGetSendLockFlagFunc,
    _DevConnectWorkInitFunc,
};

//------------------------------------------------------------------------------
/**
 * @brief   ������
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork)
{
    GFL_NET_IRC_Init((GFL_NET_GetNETInitStruct())->irc_timeout);
    GFL_NET_IRC_InitializeFlagSet();
    return TRUE;
}


static BOOL _DevStartFunc(NetDevEndCallback callback)
{
	OS_TPrintf("IRC_Connect���s\n");
    IRC_Connect();
    return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���C������
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevMainFunc(u16 bitmap)
{
    GFL_NET_IRC_CommandContinue();
    return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�I������
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevEndFunc(BOOL bForce,NetDevEndCallback callback)
{
    //�ԊO���ɂ͐ؒf�������Ȃ�
    return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �I������
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevExitFunc(NetDevEndCallback callback)
{
    GFL_NET_IRC_Exit();
    return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �X�^�[�g��Ԃ��ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevIsStartFunc(void)
{
    return GFL_NET_IRC_InitializeFlagGet();
}

static BOOL _DevSendDataFunc(void* data,int size,int no,NetDevEndCallback callback)
{
    GFL_NET_IRC_Send(data, size, 0);
    if(callback){
        callback( TRUE );
    }
    return TRUE;
}

static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback)
{
    GFL_NET_IRC_RecieveFuncSet(recvCallback);
    return TRUE;
}


static u32 _DevGetCurrentIDFunc(void) ///<DevGetCurrentID            �����̐ڑ�ID��Ԃ�
{
    if(GFL_NET_IRC_IsConnect()){
        return GFL_NET_IRC_System_GetCurrentAid();
    }
    return GFL_NET_NO_PARENTMACHINE;
}

static int _DevGetErrorFunc(void)
{
	return GFL_NET_IRC_ErrorCheck();
}

static void _DevIrcMoveFunc(void)
{
    GFL_NET_IRC_Move();
}

static BOOL _DevIsSendDataFunc(void)
{
    return GFL_NET_IRC_SendCheck();
}


static BOOL _DevGetSendTurnFunc(void)
{
    return GFL_NET_IRC_SendTurnGet();
}

static BOOL _DevIsConnectSystemFunc(void)
{
    return GFL_NET_IRC_IsConnectSystem();
}

static BOOL _DevIsConnectFunc(void)
{
    return GFL_NET_IRC_IsConnect();
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�h����Ԃ��ǂ���  IRC�ɂ͑ҋ@��Ԃ��Ȃ��̂� START�Ɠ����ɂ��Ă���
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevIsIdleFunc(void)
{
    return GFL_NET_IRC_InitializeFlagGet();
}


static BOOL _DevGetSendLockFlagFunc(void)
{
    return GFL_NET_IRC_SendLockFlagGet();
}

static void _DevConnectWorkInitFunc(void)
{
    GFL_NET_IRC_FirstConnect();
}


//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X�e�[�u����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
GFLNetDevTable *NET_GetIrcDeviceTable(void)
{
    return &netDevTbl;
}

