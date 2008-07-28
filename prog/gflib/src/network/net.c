//=============================================================================
/**
 * @file	net.c
 * @brief	GFL�ʐM���C�u�����[
 * @author	GAME FREAK Inc.
 * @date    2006.12.14
 */
//=============================================================================

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

#define  GFL_NET_OVERLAY  (0)




#if GFL_NET_WIFI
FS_EXTERN_OVERLAY(wifi);
#endif
FS_EXTERN_OVERLAY(wireless);


// �ʐM�Ŏg�p����CreateHEAP��

#define _HEAPSIZE_NET              (0xE000)          ///< NET�֘A�̃������̈�
#define _HEAPSIZE_WIFI             (MYDWC_HEAPSIZE+0xA000)  ///< DWC���g�p����̈�

static GFL_NETSYS* _pNetSys = NULL; // �ʐM�̃������[���Ǘ�����static�ϐ�

// �R���|�[�l���g�]���I���̊m�F�p  �C�N�j���[�����R���|�[�l���g�������ړ�������Ƃ��͂�����ړ�
static PTRIchneumonCallback pIchneumonCallback = NULL;
static u8 bLoadIchneumon = FALSE;

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
}

//==============================================================================
/**
 * @brief �ʐM������
 * @param[in]   pNetInit  �ʐM�������\���̂̃|�C���^
 * @return none
 */
//==============================================================================
void GFL_NET_Init(const GFLNetInitializeStruct* pNetInit,NetStepEndCallback callback)
{
    int index;
    // �ʐM�q�[�v�쐬

#if GFL_NET_OVERLAY
    if(!pNetInit->bWiFi){
        GFL_OVERLAY_Load( FS_OVERLAY_ID( wireless ) );
    }
#if GFL_NET_WIFI
    else{
        GFL_OVERLAY_Load( FS_OVERLAY_ID( wifi ) );
    }
#endif
#endif
    
    GFL_HEAP_CreateHeap( pNetInit->baseHeapID, pNetInit->netHeapID, _HEAPSIZE_NET );
    if(pNetInit->bWiFi){
        GFL_HEAP_CreateHeap( pNetInit->baseHeapID, pNetInit->wifiHeapID, _HEAPSIZE_WIFI);
    }
    {
        GFL_NETSYS* pNet = GFL_HEAP_AllocClearMemory(pNetInit->netHeapID, sizeof(GFL_NETSYS));
        _pNetSys = pNet;

        NET_PRINT("size %d addr %x\n",sizeof(GFLNetInitializeStruct),(u32)&pNet->aNetInit);

        GFL_STD_MemCopy(pNetInit, &pNet->aNetInit, sizeof(GFLNetInitializeStruct));
        GFL_NET_StateDeviceInitialize(pNetInit->netHeapID, callback);
      //  for(index = 0; index < pNet->aNetInit.maxConnectNum; index++){
        //    _pNetSys->pKey[index] = GFL_UI_KEY_Init(pNet->aNetInit.netHeapID);
       // }

        GFL_NET_COMMAND_Init( pNetInit->recvFuncTable, pNetInit->recvFuncTableNum, pNetInit->pWork);
    }
    GFL_NET_WirelessIconEasyXY(pNetInit->iconX,pNetInit->iconY,pNetInit->bWiFi, pNetInit->netHeapID);
    NET_PRINT("GFL_NET_Init\n");
}

//==============================================================================
/**
 * @brief �ʐM�������������������ǂ������m�F����  ���̂Ƃ���Ichneumon�Ɠ���
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
    if(!pNet->aNetInit.bWiFi){
        if( _GFL_NET_GetNETWL()){
            return TRUE;
        }
    }
#if GFL_NET_WIFI
    else{
        return GFL_NET_DWC_IsInit();
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
    BOOL bWiFi = pNet->aNetInit.bWiFi;
    int index;

    pNet->pNetEndCallback(pNet->aNetInit.pWork);
    
//    for(index = 0; index < pNet->aNetInit.maxConnectNum;index++){
  //      GFL_HEAP_FreeMemory(pNet->pKey[index]);
    //}
    GFL_NET_HANDLE_DeleteAll(pNet);
    GFL_HEAP_FreeMemory(pNet);
    _pNetSys = NULL;

    if(bWiFi){
        GFL_HEAP_DeleteHeap(wifiHeapID);
    }
    GFL_HEAP_DeleteHeap(netHeapID);

#if GFL_NET_OVERLAY
    if(!bWiFi){
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( wireless ) );
    }
#if GFL_NET_WIFI
    else{
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( wifi ) );
    }
#endif
#endif
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
    pNet->pNetEndCallback = netEndCallback;
    GFL_NET_StateExit(_netEndCallback);
    GFL_NET_WirelessIconEasyEnd();
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
 * @return  �r�[�R���f�[�^�̐擪�|�C���^ �Ȃ����NULL
 */
//==============================================================================
u8* GFL_NET_GetBeaconMacAddress(int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return NULL;
    }
    return GFL_NET_WLGetUserMacAddress(index);
}

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
 * @param   @@OO �G���h�R�[���o�b�N���K�v
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
    if(pNet->aNetInit.bWiFi){
        GFL_NET_StateConnectWifiParent(pNet->aNetInit.netHeapID);
    }
    else{
        GFL_NET_StateConnectParent(pNet->aNetInit.netHeapID);
    }
#else
    GFL_NET_StateConnectParent(pNet->aNetInit.netHeapID);
#endif
}


//==============================================================================
/**
 * @brief    �e�@�ɂȂ�
 * @param    GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
void GFL_NET_CreateServer(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GFL_NET_StateCreateParent(pNet->aNetInit.netHeapID);
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
    GFL_NET_StateChangeoverConnect(pNet->aNetInit.netHeapID, callback);
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
    if(pNet){
        GFL_NET_WirelessIconEasyXY( pNet->aNetInit.iconX, pNet->aNetInit.iconY, pNet->aNetInit.bWiFi, pNet->aNetInit.netHeapID);
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

    if(GFL_NET_SystemGetCurrentID() == GFL_NET_PARENT_NETID){
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
    if(GFL_NET_HANDLE_GetNetHandleID(pNetHandle) == GFL_NET_PARENT_NETID){
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
 * @param[in]   void* data                     ���M�f�[�^�|�C���^
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
BOOL GFL_NET_SendData(GFL_NETHANDLE* pNet,const u16 sendCommand,const void* data)
{
    return GFL_NET_SystemSendData(sendCommand, data, 0,
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
BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u8 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock)
{
    if((GFL_NET_IsSendEnable(pNet)==FALSE) && (sendCommand >= GFL_NET_CMD_COMMAND_MAX)){
        // �F�؂��I���Ȃ��̂ɁA���C���ȊO�̃R�}���h�𑗂邱�Ƃ͂ł��Ȃ�
        return FALSE;
    }
    if(bRepeat && GFL_NET_SystemIsSendCommand(sendCommand,GFL_NET_HANDLE_GetNetHandleID(pNet))){
        return FALSE;
    }
    return GFL_NET_SystemSendData(sendCommand, data, size, bFast, GFL_NET_HANDLE_GetNetHandleID(pNet) , sendID, bSendBuffLock);
}

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

    bReturn = GFL_NET_SendData(pNetHandle, GFL_NET_CMD_DSMP_CHANGE, &bDSMode);
    if(FALSE == bReturn){
        if(func){
            func(pNet->aNetInit.pWork, FALSE);
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
    pNet->aNetInit.pWork = pWork;
}


#endif // GFL_NET_WIFI

//==============================================================================
/**
 * @brief    �ڑ��\�ȃ}�V���̑䐔�������܂ސ���Ԃ�
 * @return   �ڑ��l��
 */
//==============================================================================
int GFI_NET_GetConnectNumMax(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GF_ASSERT(pNet->aNetInit.maxConnectNum <= GFL_NET_MACHINE_MAX);
    return pNet->aNetInit.maxConnectNum;
}


//==============================================================================
/**
 * @brief    ���M�ő�T�C�Y�𓾂�
 * @return   ���M�T�C�Y
 */
//==============================================================================
int GFI_NET_GetSendSizeMax(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
#ifdef PM_DEBUG
    GF_ASSERT(pNet->aNetInit.maxConnectNum > 0);
//    GF_ASSERT(pNet->aNetInit.maxSendSize < ((WM_SIZE_MP_DATA_MAX-4)/pNet->aNetInit.maxConnectNum));
#endif
    return pNet->aNetInit.maxSendSize;
}

//==============================================================================
/**
 * @brief    GFI_NET_AutoParentConnectFunc
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   DWCUserData�̃|�C���^
 */
//==============================================================================

void GFI_NET_AutoParentConnectFunc(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet->aNetInit.autoParentConnect!=NULL){
        pNet->aNetInit.autoParentConnect(pNet->aNetInit.pWork);
    }
}

//==============================================================================
/**
 * @brief    FatalError���s
 * @param    pNetHandle  �l�b�g�n���h��
 * @param    errorNo     �G���[�ԍ�
 * @return   DWCUserData�̃|�C���^
 */
//==============================================================================

void GFI_NET_FatalErrorFunc(int errorNo)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(pNet->aNetInit.errorFunc != NULL){
        pNet->aNetInit.errorFunc(GFL_NET_HANDLE_GetCurrentHandle(), errorNo, pNet->aNetInit.pWork);
    }
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

