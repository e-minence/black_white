//=============================================================================
/**
 * @file	net.c
 * @brief	GFL�ʐM���C�u�����[
 * @author	GAME FREAK Inc.
 * @date    2006.12.14
 */
//=============================================================================

#include "gflib.h"

#include "device/wh_config.h"

#include "net_def.h"
#include "device/net_wireless.h"
#include "device/dwc_rapcommon.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "wm_icon.h"

#include "device/dwc_rap.h"


#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "tool/net_tool.h"

// �ʐM�Ŏg�p����CreateHEAP��

#define _HEAPSIZE_NET              (0xE000)          ///< NET�֘A�̃������̈�
#define _HEAPSIZE_WIFI             (MYDWC_HEAPSIZE+0xA000)  ///< DWC���g�p����̈�

/// @brief  �ʐM�V�X�e���Ǘ��\����
struct _GFL_NETSYS{
    GFLNetInitializeStruct aNetInit;  ///< �������\���̂̃R�s�[
    GFL_NETHANDLE* pNetHandle[GFL_NET_HANDLE_MAX]; ///< �ʐM�n���h��
    GFL_NETHANDLE* pNetHandleInit;  ///< �������p�n���h��
    GFL_NETWL* pNetWL;   ///<  ���C�����X�}�l�[�W���[�|�C���^
};

static GFL_NETSYS* _pNetSys = NULL; // �ʐM�̃������[���Ǘ�����static�ϐ�

// �R���|�[�l���g�]���I���̊m�F�p  �C�N�j���[�����R���|�[�l���g�������ړ�������Ƃ��͂�����ړ�
static volatile u8   startCheck;	

//==============================================================================
/**
 * @brief       netHandle���Ǘ��}�l�[�W���[�ɒǉ�
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @param       pHandle   �ʐM�n���h��
 * @return      �ǉ������ԍ�
 */
//==============================================================================
static int _addNetHandle(GFL_NETSYS* pNet, GFL_NETHANDLE* pHandle)
{
    int i;

    for(i = 0;i < GFL_NET_HANDLE_MAX; i++){
        if(pNet->pNetHandle[i] == NULL){
            pNet->pNetHandle[i] = pHandle;
            return i;
        }
    }
    OS_TPanic("no handle");
    return 0;
}

//==============================================================================
/**
 * @brief       netHandle�̔ԍ��𓾂�
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @param       pHandle   �ʐM�n���h��
 * @return      �ǉ������ԍ�
 */
//==============================================================================
static int _numNetHandle(GFL_NETSYS* pNet, GFL_NETHANDLE* pHandle)
{
    int i;

    for(i = 0;i < GFL_NET_HANDLE_MAX; i++){
        if(pNet->pNetHandle[i] == pHandle){
            return i;
        }
    }
    return -1;
}

//==============================================================================
/**
 * @brief       netHandle�̒��g������
 * @param       pNetHandle   �ʐM�n���h��
 * @return      �ǉ������ԍ�
 */
//==============================================================================
static void _deleteHandle(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle->pParent){
        GFL_HEAP_FreeMemory(pNetHandle->pParent);
    }
    GFL_NET_TOOL_End(pNetHandle->pTool);
    GFL_HEAP_FreeMemory(pNetHandle);
}

//==============================================================================
/**
 * @brief       netHandle������ ����J�֐�
 * @param       pHandle   �ʐM�n���h��
 * @return      none
 */
//==============================================================================
void GFI_NET_DeleteNetHandle(GFL_NETHANDLE* pHandle)
{
    int i;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    for(i = 0;i < GFL_NET_HANDLE_MAX;i++){
        if(pNet->pNetHandle[i]==pHandle){
            if(pNet->pNetHandleInit == pHandle){
                pNet->pNetHandleInit = NULL;
            }
            _deleteHandle(pNet->pNetHandle[i]);
            pNet->pNetHandle[i] = NULL;
        }
    }
}

//==============================================================================
/**
 * @brief       netHandle��S�ď���
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @return      none
 */
//==============================================================================
static void _deleteAllNetHandle(GFL_NETSYS* pNet)
{
    int i;

    for(i = 0;i < GFL_NET_HANDLE_MAX;i++){
        if(pNet->pNetHandle[i]!=NULL){
            _deleteHandle(pNet->pNetHandle[i]);
            pNet->pNetHandle[i] = NULL;
        }
    }
}

//==============================================================================
/**
 * @brief       id�ɉ�����netHandle���o��  ����J�֐�
 * @param       netID    id
 * @return      netHandle
 */
//==============================================================================
GFL_NETHANDLE* GFL_NET_GetNetHandle(int netID)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    return pNet->pNetHandle[netID];
}

//==============================================================================
/**
 * @brief       netHandle ��state�n���h���𓾂� ����J�֐�
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @return      PTRStateFunc
 */
//==============================================================================
PTRStateFunc GFL_NET_GetStateFunc(GFL_NETHANDLE* pHandle)
{
    return pHandle->state;
}

//==============================================================================
/**
 * @brief       �����̃l�S�V�G�[�V����������ł��邩�ǂ���
 * @param[in]   pHandle   �ʐM�n���h��
 * @return      ����ł���ꍇTRUE   �܂��̏ꍇFALSE
 */
//==============================================================================
BOOL GFL_NET_IsNegotiation(GFL_NETHANDLE* pHandle)
{
    if(pHandle){
        return (pHandle->negotiation == _NEGOTIATION_OK);
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief       �l�S�V�G�[�V�����J�n��e�ɑ��M����
 * @param[in]   pHandle   �ʐM�n���h��
 * @return      ���M�ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_RequestNegotiation(GFL_NETHANDLE* pHandle)
{
    u8 id = GFL_NET_SystemGetCurrentID();
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
//    id = id * 8 + _numNetHandle(pNet, pHandle);
    pHandle->creatureNo = id;

    if(GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID())){
        return GFL_NET_SendData(pHandle, GFL_NET_CMD_NEGOTIATION, &id);
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief       tool��work�|�C���^�𓾂� ����J�֐�
 * @param[in]   pHandle  �ʐM�n���h��
 * @return      NET_TOOLSYS�|�C���^
 */
//==============================================================================
NET_TOOLSYS* _NETHANDLE_GetTOOLSYS(GFL_NETHANDLE* pHandle)
{
    return pHandle->pTool;
}

//==============================================================================
/**
 * @brief    �ʐM��boot��������
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
void GFL_NET_Init(const GFLNetInitializeStruct* pNetInit)
{
    // �ʐM�q�[�v�쐬
    GFL_HEAP_CreateHeap( pNetInit->baseHeapID, pNetInit->netHeapID, _HEAPSIZE_NET );
    if(pNetInit->bWiFi){
        GFL_HEAP_CreateHeap( pNetInit->baseHeapID, pNetInit->wifiHeapID, _HEAPSIZE_WIFI);
    }
    {
        GFL_NETSYS* pNet = GFL_HEAP_AllocMemory(pNetInit->netHeapID, sizeof(GFL_NETSYS));
        _pNetSys = pNet;

        GFL_STD_MemClear(pNet, sizeof(GFL_NETSYS));

        NET_PRINT("size %d addr %x",sizeof(GFLNetInitializeStruct),(u32)&pNet->aNetInit);

        GFL_STD_MemCopy(pNetInit, &pNet->aNetInit, sizeof(GFLNetInitializeStruct));
        if(pNetInit->bNetwork){
            GFL_NETHANDLE* pNetHandle = GFL_NET_CreateHandle();
            GFL_NET_StateDeviceInitialize(pNetHandle);
            pNet->pNetHandleInit = pNetHandle;
        }

        GFL_NET_COMMAND_Init( pNetInit->recvFuncTable, pNetInit->recvFuncTableNum, pNetInit->pWork);
    }
    WirelessIconEasy(pNetInit->bWiFi, pNetInit->netHeapID);
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

    if(pNet->pNetHandleInit!=NULL){
        return FALSE;
    }
    return TRUE;
}

//==============================================================================
/**
 * @brief  �ʐM�I��
 * @retval  TRUE  �I�����܂���
 * @retval  FALSE �I�����܂��� ���Ԃ��󂯂Ă������Ă�ł�������
 */
//==============================================================================
BOOL GFL_NET_Exit(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    HEAPID netHeapID = pNet->aNetInit.netHeapID;
    HEAPID wifiHeapID = pNet->aNetInit.wifiHeapID;
    BOOL bWiFi = pNet->aNetInit.bWiFi;

    if(GFL_NET_SystemIsInitialize()){
        return FALSE;
    }
    WirelessIconEasyEnd();
    _deleteAllNetHandle(pNet);
    GFL_HEAP_FreeMemory(pNet);
    _pNetSys = NULL;

    if(bWiFi){
        GFL_HEAP_DeleteHeap(wifiHeapID);
    }
    GFL_HEAP_DeleteHeap(netHeapID);
    return TRUE;
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
void GFL_NET_AddBeaconServiceID(GFL_NETHANDLE* pNet, GameServiceID gsid)
{
}

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
 * @return  none
 */
//==============================================================================

void GFL_NET_Main(void)
{
    int i;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet==NULL){
        return;
    }
    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        if(pNet->pNetHandle[i]!=NULL){
            GFL_NET_StateMainProc(pNet->pNetHandle[i]);  // ���̓�����handle���������Ƃ�����
        }
        if(pNet->pNetHandle[i]!=NULL){
            GFL_NET_TOOL_TimingSyncSend(pNet->pNetHandle[i]);
            GFL_NET_StateTransmissonMain(pNet->pNetHandle[i]);
        }
    }
    GFL_NET_SystemUpdateData();
    
}

//==============================================================================
/**
 * @brief   �ʐM�n���h�������   �ʐM��P�ʂ̃��[�N�G���A�̎�
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
GFL_NETHANDLE* GFL_NET_CreateHandle(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    GFL_NETHANDLE* pHandle = GFL_HEAP_AllocMemory(pNet->aNetInit.netHeapID, sizeof(GFL_NETHANDLE));
    GFL_STD_MemClear(pHandle, sizeof(GFL_NETHANDLE));
    _addNetHandle(pNet, pHandle);
    pHandle->pTool = GFL_NET_TOOL_Init(pNet->aNetInit.netHeapID, pNet->aNetInit.maxConnectNum);
    return pHandle;
}


//==============================================================================
/**
 * @brief   �q�@�ɂȂ�w�肵���e�@�ɐڑ�����
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
void GFL_NET_InitClientAndConnectToParent(GFL_NETHANDLE* pHandle,u8* macAddress)
{
    GFL_STD_MemCopy(macAddress, pHandle->aMacAddress, sizeof(pHandle->aMacAddress));
    GFL_NET_StateConnectMacAddress(pHandle, TRUE);
}

//==============================================================================
/**
 * @brief   �w�肵���e�@�ɐڑ�����
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @param   macAddress     �}�b�N�A�h���X�̃o�b�t�@
 * @return  none
 */
//==============================================================================
void GFL_NET_ConnectToParent(GFL_NETHANDLE* pHandle,u8* macAddress)
{
    GFL_STD_MemCopy(macAddress, pHandle->aMacAddress, sizeof(pHandle->aMacAddress));
    GFL_NET_StateConnectMacAddress(pHandle, FALSE);
}

//==============================================================================
/**
 * @brief   �q�@�ɂȂ�r�[�R�����W�߂�
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================
void GFL_NET_StartBeaconScan(GFL_NETHANDLE* pHandle)
{
    GFL_NET_StateBeaconScan(pHandle);
}

//==============================================================================
/**
 * @brief    �e�@�ɂȂ�҂��󂯂�
 * @param    GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
void GFL_NET_InitServer(GFL_NETHANDLE* pHandle)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

#if GFL_NET_WIFI
    if(pNet->aNetInit.bWiFi){
        GFL_NET_StateConnectWifiParent(pHandle, pNet->aNetInit.netHeapID);
    }
    else{
        GFL_NET_StateConnectParent(pHandle, pNet->aNetInit.netHeapID);
    }
#else
    GFL_NET_StateConnectParent(pHandle, pNet->aNetInit.netHeapID);
#endif
}

//==============================================================================
/**
 * @brief    �e�@�q�@���J��Ԃ��A�N�ł������̂Őڑ�����
 * @param    GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return   none
 */
//==============================================================================
void GFL_NET_ChangeoverConnect(GFL_NETHANDLE* pHandle)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GFL_NET_StateChangeoverConnect(pHandle, pNet->aNetInit.netHeapID);
}

//==============================================================================
/**
 * @brief    Wi-Fi���r�[�֐ڑ�����
 * @param    GFL_NETHANDLE        �ʐM�n���h���̃|�C���^
 * @param    GFL_WIFI_FRIENDLIST  �t�����h���X�g�\���̂̃|�C���^
 * @return   none
 */
//==============================================================================

void GFL_NET_WifiLogin(GFL_NETHANDLE* pHandle)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    GFL_NET_StateWifiEnterLogin(pHandle,
                                pNet->aNetInit.netHeapID, pNet->aNetInit.wifiHeapID);


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
    return pNetHandle->creatureNo;
}

//==============================================================================
/**
 * @brief ���݂̐ڑ��䐔�𓾂�
 * @retval  int  �ڑ���
 */
//==============================================================================
int GFL_NET_GetConnectNum(void)
{
    return GFL_NET_SystemGetConnectNum();
}

//==============================================================================
/**
 * @brief ID�̒ʐM�Ɛڑ����Ă��邩�ǂ�����Ԃ�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @param   NetID id            �l�b�gID
 * @retval  BOOL  �ڑ����Ă�����TRUE
 */
//==============================================================================
BOOL GFL_NET_IsConnectMember(GFL_NETHANDLE* pNet,NetID id)
{
    return pNet->negotiationID[id];
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����ςݐl���𓾂�
 * @param[in,out]   NetHandle* pNet     �ʐM�n���h���̃|�C���^
 * @return   �l��
 */
//==============================================================================
int GFL_NET_GetNegotiationConnectNum(GFL_NETHANDLE* pNet)
{
    int j=0,i;

    for(i = 0; i < GFL_NET_HANDLE_MAX ; i++){
        if( pNet->negotiationID[i] ){
            j++;
        }
    }
    return j;
}

//==============================================================================
/**
 * @brief �ʐM�ؒf����
 * @retval  none
 */
//==============================================================================
void GFL_NET_Disconnect(void)
{
    int i,userNo = -1;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    if(pNet){
        for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
            if(pNet->pNetHandle[i]){
                //            GFL_NET_SendData(pNet->pNetHandle[i],GFL_NET_CMD_EXIT,NULL); ///�I����S���ɑ��M
                userNo = i;
            }
        }
    }
    if(userNo != -1){
        GFL_NET_StateExit(pNet->pNetHandle[userNo]);
    }
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

        
        WirelessIconEasy(pNet->aNetInit.bWiFi, pNet->aNetInit.netHeapID);
    }
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
    return TRUE;
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
                                  FALSE, pNet->creatureNo ,GFL_NET_SENDID_ALLUSER);
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
 * @retval  TRUE   ��������
 * @retval  FALSE  ���s�̏ꍇ
 */
//==============================================================================
BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand, const u32 size,const void* data, const BOOL bFast, const BOOL bRepeat)
{
    if(bRepeat && !GFL_NET_SystemIsSendCommand(sendCommand,pNet->creatureNo)){
        return GFL_NET_SystemSendData(sendCommand, data, size, bFast, pNet->creatureNo ,sendID);
    }
    else if(bRepeat){
        return FALSE;
    }
    return GFL_NET_SystemSendData(sendCommand, data, size, bFast, pNet->creatureNo ,sendID);
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
    return TRUE;
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
    GFL_NET_TOOL_TimingSyncStart(pNet,no);
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
    return GFL_NET_TOOL_IsTimingSync(pNet, no);
}

//==============================================================================
/**
 * @brief     DS���[�h�ɕύX����
 * @param     NetHandle* pNet  �ʐM�n���h��
 * @return    none
 */
//==============================================================================
void GFL_NET_ChangeDsMode(GFL_NETHANDLE* pNet)
{
    u8 bDSMode = TRUE;
    GFL_NET_SendData(pNet, GFL_NET_CMD_DSMP_CHANGE, &bDSMode);
}

//==============================================================================
/**
 * @brief     MP���[�h�ɕύX����
 * @param     NetHandle* pNet  �ʐM�n���h��
 * @return    none
 */
//==============================================================================
void GFL_NET_ChangeMpMode(GFL_NETHANDLE* pNet)
{
    u8 bDSMode = FALSE;
    GFL_NET_SendData(pNet, GFL_NET_CMD_DSMP_CHANGE, &bDSMode);
}

//==============================================================================
/**
 * @brief     ���荞�ݒ��ɍs�����������s
 * @param     none
 * @return    none
 */
//==============================================================================
void GFL_NET_VBlankFunc(void)
{
    WirelessIconEasyFunc();
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
BOOL GFL_NET_IsWifiLobby(GFL_NETHANDLE* pNetHandle)
{
    if(2 > GFL_NET_StateWifiIsMatched(pNetHandle)){
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
BOOL GFL_NET_StartRandomMatch(GFL_NETHANDLE* pNetHandle)
{
    return GFL_NET_StateStartWifiRandomMatch( pNetHandle );
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
    GF_ASSERT(pNet->aNetInit.maxConnectNum < GFL_NET_MACHINE_MAX);
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
    GF_ASSERT(pNet->aNetInit.maxSendSize < ((WM_SIZE_MP_DATA_MAX-4)/pNet->aNetInit.maxConnectNum));
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

void GFI_NET_FatalErrorFunc(GFL_NETHANDLE* pNetHandle,int errorNo)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(pNet->aNetInit.errorFunc != NULL){
        pNet->aNetInit.errorFunc(pNetHandle, errorNo);
    }
}

//==============================================================================
/**
 * @brief    SSID�𓾂�֐�
 * @return   SSID
 */
//==============================================================================

u8* GFI_NET_GetSSID(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    if(pNet->aNetInit.getSSID != NULL){
        return pNet->aNetInit.getSSID();
    }
    return NULL;
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
void GFL_NET_SetAutoErrorCheck(GFL_NETHANDLE* pNetHandle,BOOL bAuto)
{
    GFL_NET_STATE_SetAutoErrorCheck(pNetHandle, bAuto);
}

//==============================================================================
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ肷��
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================
void GFL_NET_SetNoChildErrorCheck(GFL_NETHANDLE* pNetHandle,BOOL bFlg)
{
    GFL_NET_STATE_SetNoChildErrorCheck(pNetHandle, bFlg);
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
    if (result != WVR_RESULT_SUCCESS) {
        OS_TPanic("WVR_StartUpAsync error.[%08xh]\n", result);
    }
    else{
		NET_PRINT("WVR Trans VRAM-D.\n");
	}
    startCheck = 2;
}

//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����݂̂�����������
 * @return   none
 */
//==============================================================================

void GFL_NET_InitIchneumon(void)
{
    int ans;
    //************************************


    GFL_UI_SleepDisable(GFL_UI_SLEEP_NET);  // �X���[�v�֎~
    // �������C�u�����쓮�J�n
	// �C�N�j���[�����R���|�[�l���g��VRAM-D�ɓ]������
    startCheck = 1;
    ans = WVR_StartUpAsync(GX_VRAM_ARM7_128_D, _startUpCallback, NULL);
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
 * @brief    �C�N�j���[�������C�u�����̏��������I��������ǂ���
 * @retval   TREU   �I��
 * @retval   FALSE  �܂��I����Ă��Ȃ�
 */
//==============================================================================

BOOL GFL_NET_IsInitIchneumon(void)
{
    return (startCheck==2);
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
    startCheck = 0;
    GFL_UI_SleepEnable(GFL_UI_SLEEP_NET);  // �X���[�v����
}

//==============================================================================
/**
 * @brief    �C�N�j���[�������C�u�����̏I�������点��
 * @retval   none
 */
//==============================================================================


void GFL_NET_ExitIchneumon(void)
{
    NET_PRINT("VRAMD Finalize\n");
    WVR_TerminateAsync(_endCallback,NULL);  // �C�N�j���[�����ؒf
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

