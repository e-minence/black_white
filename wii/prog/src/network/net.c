//=============================================================================
/**
 * @file	net.c
 * @brief	GFL�ʐM���C�u�����[
 * @author	GAME FREAK Inc.
 * @date    2006.12.14
 */
//=============================================================================

#include "net_def.h"
#include "net_local.h"
#include "net_system.h"
#include "net_command.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"


// �ʐM�Ŏg�p����CreateHEAP��

#define _HEAPSIZE_NET              (0xB000)          ///< NET�֘A�̃������̈�
#define _HEAPSIZE_WIFI             (MYDWC_HEAPSIZE+0xA000)  ///< DWC���g�p����̈�
#define _HEAPSIZE_IRC              (0x2000)          ///< �ԊO�����g�p����̈�@��check

static GFL_NETSYS* _pNetSys = NULL; // �ʐM�̃������[���Ǘ�����static�ϐ�

// �R���|�[�l���g�]���I���̊m�F�p  �C�N�j���[�����R���|�[�l���g�������ړ�������Ƃ��͂�����ړ�
static PTRIchneumonCallback pIchneumonCallback = NULL;
static u8 bLoadIchneumon = FALSE;

///�ʐM�G���[��ʂ�PUSH,POP���ABG1�̍��W�����ɖ߂��ׂɁABG1�̍��W����ɂ����ɋL��
static int err_bg1_x, err_bg1_y;

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
#pragma mark [> NewFuncList
//==============================================================================
/**
 * @brief �ʐM������
 * @param[in]   pNetInit  �ʐM�������\���̂̃|�C���^
 * @return none
 */
//==============================================================================
void GFL_NET_Init(const GFLNetInitializeStruct* pNetInit, NetStepEndCallback callback, void* pWork)
{
	NetStepEndCallback test = callback;	//���[�j���O�΍�
    // �ʐM�q�[�v�쐬

    {
        GFL_NETSYS* pNet = (GFL_NETSYS*)GFL_HEAP_AllocClearMemory(pNetInit->netHeapID, sizeof(GFL_NETSYS));
        _pNetSys = pNet;

        NET_PRINT("size %d addr %x\n",sizeof(GFLNetInitializeStruct),(u32)&pNet->aNetInit);

        GFL_STD_MemCopy((void*)pNetInit, &pNet->aNetInit, sizeof(GFLNetInitializeStruct));
        pNet->pWork = pWork;
        //GFL_NET_StateDeviceInitialize(pNetInit->netHeapID, callback);
        GFI_NET_HANDLE_InitAll(pNet);
        const int cokind = (int)pNetInit->gsid*0x100;
        GFL_NET_COMMAND_Init(cokind , pNetInit->recvFuncTable, pNetInit->recvFuncTableNum, pNet->pWork);
    }
    OS_TPrintf("GFL_NET_Init �ʐM�J�n\n");
}
//==============================================================================
/**
 * @brief    �ڑ��\�ȃ}�V���̑䐔�������܂ސ���Ԃ�
 * @return   �ڑ��l��
 */
//==============================================================================
int GFI_NET_GetConnectNumMax(void)
{
/*
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GF_ASSERT(pNet->aNetInit.maxConnectNum <= GFL_NET_MACHINE_MAX);
    return pNet->aNetInit.maxConnectNum;
*/
	return GFL_NET_MACHINE_MAX;
}


//==============================================================================
/**
 * @brief    ���M�ő�T�C�Y�𓾂�
 * @return   ���M�T�C�Y
 */
//==============================================================================
int GFI_NET_GetSendSizeMax(void)
{
/*
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
#ifdef PM_DEBUG
    GF_ASSERT(pNet->aNetInit.maxConnectNum > 0);
//    GF_ASSERT(pNet->aNetInit.maxSendSize < ((WM_SIZE_MP_DATA_MAX-4)/pNet->aNetInit.maxConnectNum));
#endif
    return pNet->aNetInit.maxSendSize;
*/
   	return MY_DS_LENGTH;
}
//==============================================================================
/**
 * @brief ���M�������肽���ǂ�������������i�q�@��p�֐��j
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   ����
 * @retval  FALSE  �֎~
 */
//==============================================================================
BOOL GFL_NET_IsSendEnable(GFL_NETHANDLE* pNet)
{
    return (pNet->negotiationType == _NEG_TYPE_CONNECT);
}

//==============================================================================
/**
 * @brief ���M�J�n
 *        �S���ɖ������ő��M  ���M�T�C�Y�͏��������̃e�[�u����������o��
 *        �f�[�^�͕K���R�s�[����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u16 sendCommand                ���M����R�}���h
 * @param[in]   u16 size                       ���M����f�[�^�T�C�Y
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
BOOL GFL_NET_SendData(GFL_NETHANDLE* pNet,const u16 sendCommand,const u16 size,const void* data)
{
    return GFL_NET_SystemSendData(sendCommand, data, size,
                                  FALSE, GFL_NET_HANDLE_GetNetHandleID(pNet) ,GFL_NET_SENDID_ALLUSER, FALSE);
}

//==============================================================================
/**
 * @brief ���M�J�n
 * @param[in,out]  pNet  �ʐM�n���h��
 * @param[in]   sendID                     ���M���� �S���֑��M����ꍇ NET_SENDID_ALLUSER
 * @param[in]   sendCommand                ���M����R�}���h
 * @param[in]   size                       ���M�f�[�^�T�C�Y
 * @param[in]   data                       ���M�f�[�^�|�C���^
 * @param[in]   bFast                      �D�揇�ʂ��������đ��M����ꍇTRUE
 * @param[in]   bRepeat                    ���̃R�}���h���L���[�ɂȂ��Ƃ��������M
 * @param[in]   bSendBuffLock              ���M�o�b�t�@���Ăԑ����ێ�����ꍇ�i�ʐM���̃�����������Ȃ��̂ő傫���f�[�^�𑗐M�ł��܂��j
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock)
{
    if((GFL_NET_IsSendEnable(pNet)==FALSE) && (sendCommand >= GFL_NET_CMD_COMMAND_MAX)){
        // �F�؂��I���Ȃ��̂ɁA���C���ȊO�̃R�}���h�𑗂邱�Ƃ͂ł��Ȃ�
        return FALSE;
    }
    if(bRepeat && GFL_NET_SystemIsSendCommand(sendCommand,GFL_NET_HANDLE_GetNetHandleID(pNet))){
        return FALSE;
    }
    return GFL_NET_SystemSendData((int)sendCommand, data, size, bFast, GFL_NET_HANDLE_GetNetHandleID(pNet) , sendID, bSendBuffLock);
}

//==============================================================================
/**
 * @brief    �ʐM�Ǘ��\���̂𓾂�  �i�ʐM�����g�p �O���Ɍ��J����Ȃ�GFL_NET_Initialize�̖߂�l�ɂ���ق��������j
 * @return   �l�b�g�V�X�e���\���̃|�C���^
 */
//==============================================================================
GFL_NETSYS* _GFL_NET_GetNETSYS(void)
{
    return _pNetSys;
}

//==============================================================================
/**
 * @brief    �ʐM�Ǘ��������\���̂𓾂�
 * @return   �l�b�g�V�X�e���\���̃|�C���^
 */
//==============================================================================
GFLNetInitializeStruct* _GFL_NET_GetNETInitStruct(void)
{
    return &_pNetSys->aNetInit;
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
#pragma mark [> OldFuncList
#if 0

//==============================================================================
/**
 * @brief    boot���̒ʐM������
 *           WIFI��IPL�����ݒ肪�N�����ɕK�v�Ȉ�
 * @param    heapID    �g�p����temp������ID
 * @param    errorFunc �G���[���ɌĂяo���֐�
 * @return   none
 */
//==============================================================================
void GFL_NET_Boot(HEAPID heapID, NetErrorFunc errorFunc)
{
#if GFL_NET_WIFI
    //WIFI��IPL�����ݒ�
    GFL_NET_WifiStart( heapID, errorFunc );
#endif

#if GFL_NET_IRC
    GFL_NET_IRC_Init();
#endif
}

//==============================================================================
/**
 * @brief �ʐM�������������������ǂ������m�F����
 * @retval TRUE   �������I��
 * @retval FALSE  ���������܂��I����Ă��Ȃ�
 */
//==============================================================================
BOOL GFL_NET_IsInit(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet==NULL){
        return FALSE;
    }
    if(pNet->aNetInit.bNetType == GFL_NET_TYPE_WIRELESS){
        if( _GFL_NET_GetNETWL()){
            return TRUE;
        }
    }
#if GFL_NET_WIFI
    else if(pNet->aNetInit.bNetType == GFL_NET_TYPE_WIFI){
        return GFL_NET_DWC_IsInit();
    }
#endif
#if GFL_NET_IRC
	else if(pNet->aNetInit.bNetType == GFL_NET_TYPE_IRC || pNet->aNetInit.bNetType == GFL_NET_TYPE_IRC_WIRELESS){
		return GFL_NET_IRC_InitializeFlagGet();
	}
#endif
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�I���R�[���o�b�N
 */
//==============================================================================
static void _netEndCallback(void* pWork)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    HEAPID netHeapID = pNet->aNetInit.netHeapID;
    HEAPID wifiHeapID = pNet->aNetInit.wifiHeapID;
    HEAPID ircHeapID = pNet->aNetInit.ircHeapID;
    int bNetType = pNet->aNetInit.bNetType;
    int index;

    if(pNet->pNetEndCallback){
        pNet->pNetEndCallback(pNet->pWork);
    }
    if(pNet->aNetInit.connectEndFunc){
        pNet->aNetInit.connectEndFunc(pNet->pWork);
    }
    GFI_NET_HANDLE_DeleteAll(pNet);
    GFL_HEAP_FreeMemory(pNet);
    _pNetSys = NULL;

    if(bNetType == GFL_NET_TYPE_WIFI){
        GFL_HEAP_DeleteHeap(wifiHeapID);
    }
    else if(bNetType == GFL_NET_TYPE_IRC){
		GFL_HEAP_DeleteHeap(ircHeapID);
	}
    GFL_HEAP_DeleteHeap(netHeapID);

#if GFL_NET_OVERLAY
    if(bNetType == GFL_NET_TYPE_WIRELESS){
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( wireless ) );
    }
#if GFL_NET_WIFI
    else if(bNetType == GFL_NET_TYPE_WIFI){
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( wifi ) );
    }
#endif
#endif

    OS_TPrintf("Net�I��\n");
}

//==============================================================================
/**
 * @brief   �ʐM�I��
 * @param   netEndCallback    �ʐM���I�������ۂɌĂ΂��R�[���o�b�N�ł�
 */
//==============================================================================
void GFL_NET_Exit(NetEndCallback netEndCallback)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    
    GFL_NET_IW_SystemExit();
    if(pNet){
        pNet->pNetEndCallback = netEndCallback;
        GFL_NET_StateExit(_netEndCallback);
        GFL_NET_WirelessIconEasyEnd();
    }
}

//-----�r�[�R���֘A
//==============================================================================
/**
 * @brief �r�[�R�����󂯎��T�[�r�X��ǉ��w��  �i�������̃T�[�r�X�̃r�[�R�����E���������Ɂj
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   GameServiceID gsid  �Q�[���T�[�r�XID  �ʐM�̎��
 * @retval  none
 */
//==============================================================================
//void GFL_NET_AddBeaconServiceID(GFL_NETHANDLE* pNet, GameServiceID gsid)
//{
//}

//==============================================================================
/**
 * @brief   ��M�r�[�R���f�[�^�𓾂�
 * @param   index  �r�[�R���o�b�t�@index
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
void* GFL_NET_GetBeaconData(int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return NULL;
    }
    return GFL_NET_WLGetUserBss(index);
}

//==============================================================================
/**
 * @brief   ��M�r�[�R���ɑΉ�����MAC�A�h���X�𓾂�
 * @param   index  �r�[�R���o�b�t�@index
 * @return  MACADDRESS�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
u8* GFL_NET_GetBeaconMacAddress(int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return NULL;
    }
    return GFL_NET_WLGetUserMacAddress(index);
}

//==============================================================================
/**
 * @brief   �ڑ������������N���C�A���g��infomation�f�[�^�𓾂�
 * @param   index  �ڑ�����
 * @return  infomation�f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
#if 0
void* GFL_NET_GetUserInfomationData(int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return NULL;
    }
    return NULL;
//    return GFL_NET_WLGetUserBss(index);
}
#endif

//==============================================================================
/**
 * @brief   �ڑ������������N���C�A���g��MAC�A�h���X�𓾂�
 * @param   index  �ڑ�����
 * @return  MACADDRESS�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
#if 0
u8* GFL_NET_GetUserInfomationMacAddress(int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return NULL;
    }
    //    return GFL_NET_WLGetUserMacAddress(index);
    return NULL;
}
#endif
//--------�ڑ��E�ؒf

//==============================================================================
/**
 * @brief   �ʐM�̃��C�����s�֐�
 * @retval  TRUE  �������s���Ă悢�ꍇ
 * @retval  FALSE ���������Ă��Ȃ��̂ŏ������s��Ȃ��ꍇ
 */
//==============================================================================

BOOL GFL_NET_Main(void)
{
    int i;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    
    GFL_NET_IW_Main();
    if(pNet==NULL){
        return TRUE;
    }
    GFL_NET_WirelessIconEasyFunc();
    GFL_NET_StateMainProc();
    GFL_NET_HANDLE_MainProc();
    GFL_NET_SystemUpdateData();
    return GFL_NET_SystemCheckDataSharing();
}


//==============================================================================
/**
 * @brief   �q�@�ɂȂ�w�肵���e�@�ɐڑ�����
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
void GFL_NET_InitClientAndConnectToParent(u8* macAddress)
{
    GFL_NET_StateConnectMacAddress(macAddress,TRUE);
}

//==============================================================================
/**
 * @brief   �w�肵���e�@�ɐڑ�����
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
void GFL_NET_ConnectToParent(u8* macAddress)
{
    GFL_NET_StateConnectMacAddress(macAddress, FALSE);
}

//==============================================================================
/**
 * @brief   �q�@�ɂȂ�r�[�R�����W�߂�
 * @return  none
 */
//==============================================================================
void GFL_NET_StartBeaconScan(void)
{
    GFL_NET_StateBeaconScan();
}

//==============================================================================
/**
 * @brief    �e�@�ɂȂ�҂��󂯂�
 * @return   none
 */
//==============================================================================
void GFL_NET_InitServer(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

#if GFL_NET_WIFI
    if(pNet->aNetInit.bNetType == GFL_NET_TYPE_WIFI){
        GFL_NET_StateConnectWifiParent(pNet->aNetInit.netHeapID);
    }
    else if(aNet->aNetInit.bNetType == GFL_NET_TYPE_WIRELESS){
        GFL_NET_StateConnectParent(pNet->aNetInit.netHeapID);
    }
#else
    GFL_NET_StateConnectParent(pNet->aNetInit.netHeapID);
#endif
}

//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ��A�N�ł������̂Őڑ�����
 * @param    none
 * @return   none
 */
//==============================================================================
void GFL_NET_ChangeoverConnect(NetStepEndCallback callback)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(GFL_NET_IW_SystemCheck() == TRUE){
		callback = GFL_NET_IW_ConnectCallbackSetting(callback);
	}
    GFL_NET_StateChangeover(pNet->aNetInit.netHeapID, callback, TRUE);
}

//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ�����
 * @param    none
 * @return   none
 */
//==============================================================================
void GFL_NET_Changeover(NetStepEndCallback callback)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(GFL_NET_IW_SystemCheck() == TRUE){
		callback = GFL_NET_IW_ConnectCallbackSetting(callback);
	}
    GFL_NET_StateChangeover(pNet->aNetInit.netHeapID, callback, FALSE);
}

//==============================================================================
/**
 * @brief    Wi-Fi���r�[�֐ڑ�����
 * @param    GFL_NETHANDLE        �ʐM�n���h���̃|�C���^
 * @param    GFL_WIFI_FRIENDLIST  �t�����h���X�g�\���̂̃|�C���^
 * @return   none
 */
//==============================================================================

void GFL_NET_WifiLogin(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    GFL_NET_StateWifiEnterLogin(pNet->aNetInit.netHeapID, pNet->aNetInit.wifiHeapID);
}

//==============================================================================
/**
 * @brief ���̃n���h���̒ʐM�ԍ��𓾂�
 * @param[in,out]  NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @retval  NetID     �ʐMID
 */
//==============================================================================
NetID GFL_NET_GetNetID(GFL_NETHANDLE* pNetHandle)
{
    return GFL_NET_HANDLE_GetNetHandleID(pNetHandle);
}

//==============================================================================
/**
 * @brief ���݂̐ڑ��䐔�𓾂�
 * @retval  int  �ڑ���
 */
//==============================================================================
int GFL_NET_GetConnectNum(void)
{
    return GFL_NET_HANDLE_GetNumNegotiation();
}

//==============================================================================
/**
 * @brief ID�̒ʐM�Ɛڑ����Ă��邩�ǂ�����Ԃ�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetID id            �l�b�gID
 * @retval  BOOL  �ڑ����Ă�����TRUE
 */
//==============================================================================
BOOL GFL_NET_IsConnectMember(NetID id)
{
    return GFL_NET_HANDLE_IsNegotiation(GFL_NET_GetNetHandle(id));
}

//==============================================================================
/**
 * @brief �V�K�ڑ��֎~������ݒ�
 * @param   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   BOOL bEnable     TRUE=�ڑ����� FALSE=�֎~
 * @retval  none
 */
//==============================================================================
void GFL_NET_SetClientConnect(GFL_NETHANDLE* pNet,BOOL bEnable)
{
}

//==============================================================================
/**
 * @brief   ���Z�b�g�ł����Ԃ��ǂ���
 * @retval  TRUE�Ȃ烊�Z�b�g�\
 */
//==============================================================================

BOOL GFL_NET_IsResetEnable(void)
{
    if(_pNetSys==NULL){
        return TRUE;
    }
    if(GFL_NET_WLIsConnectStalth()){ // �ʐM�I��
        return TRUE;
    }
    if(!GFL_NET_WLIsConnect()){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�A�C�R�����ēx�����[�h���܂�
 * @retval  none
 */
//==============================================================================

void GFL_NET_ReloadIcon(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GFLNetInitializeStruct* pNetInit = _GFL_NET_GetNETInitStruct();

    if(pNet && pNetInit->bNetType != GFL_NET_TYPE_IRC){
        GFL_NET_WirelessIconEasyXY( pNet->aNetInit.iconX, pNet->aNetInit.iconY, pNet->aNetInit.bNetType, pNet->aNetInit.netHeapID);
    }
}


//==============================================================================
/**
 * @brief   �e�@�}�V�����ǂ���
 * @param   void
 * @retval  TRUE   �e�@
 * @retval  FALSE  �q�@
 */
//==============================================================================

BOOL GFL_NET_IsParentMachine(void)
{
    BOOL ret = FALSE;

    if(GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE){
        ret = TRUE;
    }
    return ret;
}

//==============================================================================
/**
 * @brief   �e�@�̃n���h�����ǂ���
 * @param   void
 * @retval  TRUE   �e�@
 * @retval  FALSE  �q�@
 */
//==============================================================================
BOOL GFL_NET_IsParentHandle(GFL_NETHANDLE* pNetHandle)
{
    if(GFL_NET_HANDLE_GetNetHandleID(pNetHandle) == GFL_NET_NETID_SERVER){
        return TRUE;
    }
    return FALSE;
}


//--------���M

//==============================================================================
/**
 * @brief ���M�J�n���q�@�ɓ`����i�e�@��p�֐��j
 *        ���̊֐��ȍ~�ݒ肵�����M�f�[�^���q�@�Ŏg�p�����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   NetID id            �q�@ID
 * @return  none
 */
//==============================================================================
void GFL_NET_SendEnableCommand(GFL_NETHANDLE* pNet,const NetID id)
{}


//==============================================================================
/**
 * @brief ���M�f�[�^���������ǂ���
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @retval  TRUE   �����ꍇ
 * @retval  FALSE  �c���Ă���ꍇ
 */
//==============================================================================
BOOL GFL_NET_IsEmptySendData(GFL_NETHANDLE* pNet)
{
    return GFL_NET_SystemIsEmptyQueue();
}



//--------���̑��A�c�[����
//==============================================================================
/**
 * @brief �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   u8 no   �^�C�~���O��肽���ԍ�
 * @return      none
 */
//==============================================================================
void GFL_NET_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no)
{
    GFL_NET_HANDLE_TimingSyncStart(pNet,no);
}

//==============================================================================
/**
 * @brief �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param[in,out]   NetHandle* pNet  �ʐM�n���h��
 * @param[in]   no   �͂��ԍ�
 * @retval  TRUE    �^�C�~���O�����v
 * @retval  FALSE   �^�C�~���O��������Ă��Ȃ�
 */
//==============================================================================
BOOL GFL_NET_IsTimingSync(GFL_NETHANDLE* pNet, const u8 no)
{
    return GFL_NET_HANDLE_IsTimingSync(pNet, no);
}


//==============================================================================
/**
 * @brief     ���[�h�ύX�֐�
 * @param     NetHandle* pNet  �ʐM�n���h��
 * @return    none
 */
//==============================================================================
static BOOL _changeModeRequest(GFL_NETHANDLE* pNetHandle, NetModeChangeFunc func,BOOL bDSMode)
{
    BOOL bReturn;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    bReturn = GFL_NET_SendData(pNetHandle, GFL_NET_CMD_DSMP_CHANGE,1, &bDSMode);
    if(FALSE == bReturn){
        if(func){
            func(pNet->pWork, FALSE);
        }
    }
    else{
        pNet->pNetModeChangeFunc = func;
    }
    return bReturn;
}

//==============================================================================
/**
 * @brief     DS���[�h�ɕύX����
 * @param     NetHandle* pNet    �ʐM�n���h��
 * @param     NetModeChangeFunc  �ύX�������ɌĂ΂��R�[���o�b�N
 * @return    TRUE ����   FALSE ���s
 */
//==============================================================================
BOOL GFL_NET_ChangeDsMode(GFL_NETHANDLE* pNet, NetModeChangeFunc func)
{
    return _changeModeRequest(pNet, func, TRUE);
}

//==============================================================================
/**
 * @brief     MP���[�h�ɕύX����
 * @param     NetHandle* pNet    �ʐM�n���h��
 * @param     NetModeChangeFunc  �ύX�������ɌĂ΂��R�[���o�b�N
 * @return    TRUE ����   FALSE ���s
 */
//==============================================================================
BOOL GFL_NET_ChangeMpMode(GFL_NETHANDLE* pNet, NetModeChangeFunc func)
{
    return _changeModeRequest(pNet, func, FALSE);
}


#if GFL_NET_WIFI //wifi

//==============================================================================
/**
 * @brief    DWC�̃��[�U�[�f�[�^�𓾂�
 * @return   DWCUserData�̃|�C���^
 */
//==============================================================================

DWCUserData* GFI_NET_GetMyDWCUserData(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    return pNet->aNetInit.myUserData;
}

//==============================================================================
/**
 * @brief    DWC�̗F�B�f�[�^�𓾂�
 * @return   DWCFriendData�̃|�C���^
 */
//==============================================================================

DWCFriendData* GFI_NET_GetMyDWCFriendData(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    return pNet->aNetInit.keyList;
}

//==============================================================================
/**
 * @brief    DWC�̗F�B�f�[�^�{���𓾂�
 * @return   DWCFriendData�̖{��
 */
//==============================================================================
int GFI_NET_GetFriendNumMax(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GF_ASSERT(pNet->aNetInit.maxBeaconNum < DWC_ACC_USERDATA_BUFSIZE);
    return pNet->aNetInit.maxBeaconNum;
}


//==============================================================================
/**
 * @brief    DWCUser�f�[�^���Z�[�u���Ăق������̃R�[���o�b�N�֐����Ăяo��
 * @return   none
 */
//==============================================================================
void GFI_NET_NetWifiSaveUserDataFunc(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet->aNetInit.wifiSaveFunc){
        pNet->aNetInit.wifiSaveFunc();
    }
}

//==============================================================================
/**
 * @brief    �F�B�f�[�^���}�[�W���邽�߂̊֐����Ăяo��
 * @param    deletedIndex  ������t�����h�R�[�h
 * @param    srcIndex      ���ɂȂ�t�����h�R�[�h
 * @return   none
 */
//==============================================================================
void GFI_NET_NetWifiMargeFrinedDataFunc(int deletedIndex,int srcIndex)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet->aNetInit.wifiMargeFunc){
        pNet->aNetInit.wifiMargeFunc(deletedIndex, srcIndex);
    }
}

//==============================================================================
/**
 * @brief    WIFI���r�[�֐ڑ����Ă��邩�ǂ���
 * @param    NetHandle* pNet  �ʐM�n���h��
 * @retval   TRUE   ���r�[�ڑ�
 * @retval   FALSE   �܂��ڑ����Ă��Ȃ�
 */
//==============================================================================
BOOL GFL_NET_IsWifiLobby(void)
{
    if(2 > GFL_NET_StateWifiIsMatched()){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   ���[�U�[�f�[�^������������
 * @param   DWCUserData  ���[�U�[�f�[�^�\����
 * @retval  none
 */
//==============================================================================
void GFL_NET_WIFI_InitUserData(DWCUserData *pUserdata)
{
    mydwc_createUserData( pUserdata );
}

//==============================================================================
/**
 * @brief   WIFI�ڑ��v�����������ǂ���
 * @retval  TRUE  ����
 * @retval  FALSE ���Ȃ�
 */
//==============================================================================
BOOL GFL_NET_WIFI_IsNewPlayer(void)
{
    return mydwc_IsNewPlayer();
}

//==============================================================================
/**
 * @brief   �����I��WIFI�R�l�N�V�����ԍ��𓾂�
 * @retval  -1    �R�l�N�V�����������ĂȂ�
 * @retval  0�ȏ� �R�l�N�V������ID
 */
//==============================================================================
int GFL_NET_WIFI_GetLocalConnectNo(void)
{
    return mydwc_getaid();
}

//==============================================================================
/**
 * @brief   �����_���}�b�`�ڑ����J�n����
 * @param   pNetHandle  �l�b�g�n���h��
 * @retval  TRUE  ����
 * @retval  FALSE ���s
 */
//==============================================================================
BOOL GFL_NET_StartRandomMatch(void)
{
    return GFL_NET_StateStartWifiRandomMatch();
}

//==============================================================================
/**
 * @brief   �ʐM�𓯊����񓯊����ɂ��肩����
 * @param   TRUE  ����    FALSE �񓯊�
 * @return  �Ȃ�
 */
//==============================================================================
void GFL_NET_SetWifiBothNet(BOOL flag)
{
    GFL_NET_SystemSetWifiBothNet(flag);
}

//==============================================================================
/**
 * @brief   ���[�U�[�̎g�p���[�N���Đݒ肷��
 * @param   ���[�N�̃|�C���^
 * @return  �Ȃ�
 */
//==============================================================================

void GFL_NET_SetUserWork(void* pWork)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    pNet->pWork = pWork;
}


#endif // GFL_NET_WIFI

//==============================================================================
/**
 * @brief    GFI_NET_AutoParentConnectFunc
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   none
 */
//==============================================================================

void GFI_NET_AutoParentConnectFunc(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet->aNetInit.autoParentConnect!=NULL){
        pNet->aNetInit.autoParentConnect(pNet->pWork);
    }
}

//==============================================================================
/**
 * @brief    FatalError���s
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   none
 */
//==============================================================================

void GFI_NET_FatalErrorFunc(int errorNo)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(pNet->aNetInit.errorFunc != NULL){
        pNet->aNetInit.errorFunc(GFL_NET_HANDLE_GetCurrentHandle(), errorNo, pNet->pWork);
    }
}


//==============================================================================
/**
 * @brief    �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ʐM���I��������K�v������
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   none
 */
//==============================================================================

void GFI_NET_ErrorFunc(int errorNo)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(pNet->aNetInit.errorFunc != NULL){
        pNet->aNetInit.errorFunc(GFL_NET_HANDLE_GetCurrentHandle(), errorNo, pNet->pWork);
    }
}

//==============================================================================
/**
 * @brief    WL�ʐM�Ǘ��\���̂𓾂�  �i�ʐM�����g�p ���荞�ݗp�j
 * @return   �l�b�g�V�X�e���\���̃|�C���^
 */
//==============================================================================

GFL_NETWL* _GFL_NET_GetNETWL(void)
{
    return _pNetSys->pNetWL;
}

//==============================================================================
/**
 * @brief    WL�ʐM�Ǘ��\���̂��Z�b�g����
 * @param    WL�ʐM�Ǘ��\����
 * @return   none
 */
//==============================================================================

void _GFL_NET_SetNETWL(GFL_NETWL* pWL)
{
    _pNetSys->pNetWL = pWL;
}

//==============================================================================
/**
 * @brief   �����I�ɃG���[���o���s�����ǂ���
 *          (TRUE�̏ꍇ�G���[���݂���ƃu���[�X�N���[���{�ċN���ɂȂ�)
 * @param   bAuto  TRUE�Ō����J�n FALSE�ŃG���[�������s��Ȃ�
 * @retval  none
 */
//==============================================================================
void GFL_NET_SetAutoErrorCheck(BOOL bAuto)
{
    GFL_NET_STATE_SetAutoErrorCheck(bAuto);
}

//==============================================================================
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ肷��
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================
void GFL_NET_SetNoChildErrorCheck(BOOL bFlg)
{
    GFL_NET_STATE_SetNoChildErrorCheck(bFlg);
}


//------------------------------------------------------------------
/**
 * @brief   �����쓮���䃉�C�u�����̔񓯊��I�ȏ����I�����ʒm�����R�[���o�b�N�֐��B
 * @param   arg		WVR_StartUpAsync�R�[�����Ɏw�肵�������B���g�p�B
 * @param   result	�񓯊��֐��̏������ʁB
 * @retval  none		
 */
//------------------------------------------------------------------
static void _startUpCallback(void *arg, WVRResult result)
{
    BOOL bResult = TRUE;
    if (result != WVR_RESULT_SUCCESS) {
        GF_ASSERT_MSG(0,"WVR_StartUpAsync error.[%08xh]\n", result);
        bResult = FALSE;
    }
    else{
        bLoadIchneumon = TRUE;
		NET_PRINT("WVR Trans VRAM-D.\n");
	}
    if(pIchneumonCallback){
        pIchneumonCallback(arg, bResult);
    }
}

//------------------------------------------------------------------
/**
 * @brief   �����쓮���䃉�C�u�����̔񓯊��I�ȏ����I�����ʒm�����R�[���o�b�N�֐��B
 * @param   arg		WVR_StartUpAsync�R�[�����Ɏw�肵�������B���g�p�B
 * @param   result	�񓯊��֐��̏������ʁB
 * @retval  none		
 */
//------------------------------------------------------------------
static void _endCallback(void *arg, WVRResult result)
{
    BOOL bResult = TRUE;
    if (result != WVR_RESULT_SUCCESS) {
        GF_ASSERT_MSG(0,"WVR_StartUpAsync error.[%08xh]\n", result);
        bResult = FALSE;
    }
    if(pIchneumonCallback){
        pIchneumonCallback(arg, bResult);
    }
    GFL_UI_SleepEnable(GFL_UI_SLEEP_NET);  // �X���[�v����
}

//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����݂̂�����������
 * @param    callback �I�������Ă΂��R�[���o�b�N
 * @param    pWork    �Ăԑ��̃��[�N
 * @return   none
 */
//==============================================================================

void GFL_NET_InitIchneumon(PTRIchneumonCallback callback,void* pWork)
{
    int ans;
    //************************************
    GFL_UI_SleepDisable(GFL_UI_SLEEP_NET);  // �X���[�v�֎~
    GF_ASSERT(callback);
    pIchneumonCallback = callback;
    // �������C�u�����쓮�J�n
	// �C�N�j���[�����R���|�[�l���g��VRAM-D�ɓ]������
    ans = WVR_StartUpAsync(GX_VRAM_ARM7_128_D, _startUpCallback, pWork);
    if (WVR_RESULT_OPERATING != ans) {
        //�C�N�j���[�������g�p����O�� VRAMD��disable�ɂ���K�v������
        // �����łȂ��Ƃ����ɂ���
        OS_TPanic("WVR_StartUpAsync failed. %d\n",ans);
    }
    else{
        NET_PRINT("WVRStart\n");
    }
}

//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����̏I�������点��
 * @retval   none
 */
//==============================================================================


void GFL_NET_ExitIchneumon(PTRIchneumonCallback callback,void* pWork)
{
    NET_PRINT("VRAMD Finalize\n");
    GF_ASSERT(callback);
    bLoadIchneumon = FALSE;
    pIchneumonCallback = callback;
    WVR_TerminateAsync(_endCallback,NULL);  // �C�N�j���[�����ؒf
}

//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����������Ă��邩�ǂ����𓾂�
 * @retval   TRUE �Ȃ瓮�쒆
 */
//==============================================================================

BOOL GFL_NET_IsIchneumon(void)
{
    return bLoadIchneumon;
}


//==============================================================================
/**
 * @brief    �f�o�b�O�p �������̒��𕪐͂���
 * @param    data  ������
 * @param    size  �����T�C�Y
 * @return   none
 */
//==============================================================================
#if 1
void debugcheck(u32* data,int size )
{
    int i;
    u8* p=(u8*)data;

    for(i = 0 ; i < size ; i++){
        if((*p == 0xfc) && (*p+1 == 0x0f) && (*p+2 == 0x00) && (*p+3 == 0x10)){
            OS_Panic("check");
        }
        p++;
    }
}
#endif 


/*-------------------------------------------------------------------------*
 * Name        : GFL_NET_Align32Alloc
 * Description : �������m�ۊ֐�
 * Arguments   : name  - �m�ۂ��閼�O
 *             : size  - �m�ۂ���T�C�Y
 *             : align - �m�ۂ���A���C�����g
 * Returns     : *void - �m�ۂ����|�C���^
 *
 * HEAP��4byte���E�ŕԂ��Ă��邽�߁A����𖳗����32byte���E��
 * �A���C�����g�������B
 * �Ԃ��A�h���X-4��4byte�Ɍ���alloc�����A�h���X��ۑ����Ă����āA
 * free����ۂɂ͂��̒l���Q�Ƃ��ăt���[����悤�ɂ��Ă���
 *-------------------------------------------------------------------------*/
void* GFL_NET_Align32Alloc( HEAPID id, u32 size )
{
  void *ptr;
  u32 *base;

  //  OS_TPrintf("org: %5d -> new: ", size);
  // �T�C�Y��32�o�C�g�A���C�����g���������Z����
  size = (((size + sizeof(void *)) + 32) & ~31) + 32;
  //  OS_TPrintf("%5d\n", size);
  // �������m��
  ptr = GFL_HEAP_AllocClearMemory(id, size);
  // ptr��32�o�C�g���E�ɕύX��base�֒�`
  base = (u32 *)(((u32)ptr + 32) & ~31);
  // ����-4�o�C�g�ꏊ��free����ۂ̃A�h���X����
  base--;
  *base = (u32)ptr;
  //  OS_TPrintf("Alloc: %08X [%08X], %d [%2d]\n", ptr, base, size, align);
  // �A�v���֕Ԃ��l��32�o�C�g���E
  base++;
  return base;
}


/*-------------------------------------------------------------------------*
 * Name        : GFL_NET_Align32Free
 * Description : �������J���֐�
 * Arguments   : *ptr - ������郁�����̃|�C���^
 * Returns     : None.
 *-------------------------------------------------------------------------*/
void GFL_NET_Align32Free( void* ptr )
{
  u32 *base;

  if(!ptr) return;
  base = (u32 *)((u32)ptr - 4);
  //  OS_Printf("Free: %08X [%08X]\n", ptr, *base);
  GFL_HEAP_FreeMemory((void *)*base);
}


//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʂ�PUSH,POP���ABG1�̍��W�����ɖ߂��ׂɁABG1�̍��W����ɂ����ɋL��
 *
 * @param   x		X���W
 * @param   y		Y���W
 */
//--------------------------------------------------------------
void GFL_NET_BG1PosSet(int x, int y)
{
	err_bg1_x = x;
	err_bg1_y = y;
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�G���[��ʂ�PUSH,POP���A���ɖ߂�BG1�̍��W���擾����
 *
 * @param   x		X���W�����
 * @param   y		Y���W�����
 */
//--------------------------------------------------------------
void GFL_NET_BG1PosGet(int *x, int *y)
{
	*x = err_bg1_x;
	*y = err_bg1_y;
}
#endif