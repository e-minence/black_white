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

#include "net.h"
#include "net_def.h"
#include "device/net_wireless.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "tool/net_tool.h"


/// @brief  �ʐM�V�X�e���Ǘ��\����
struct _GFL_NETSYS{
    GFLNetInitializeStruct aNetInit;  ///< �������\���̂̃R�s�[
    GFL_NETHANDLE* pNetHandle[GFL_NET_HANDLE_MAX]; ///< �ʐM�n���h��
    GFL_NETWL* pNetWL;   ///<  ���C�����X�}�l�[�W���[�|�C���^
    int heapID;  ///< �ʐM�̊m�ۃ������[�̏ꏊ
};

static GFL_NETSYS* _pNetSys = NULL; // �ʐM�̃������[���Ǘ�����static�ϐ�


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
            GFL_HEAP_FreeMemory(pNet->pNetHandle[i]->pTool);
            GFL_HEAP_FreeMemory(pNet->pNetHandle[i]);
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
            GFL_HEAP_FreeMemory(pNet->pNetHandle[i]->pTool);
            GFL_HEAP_FreeMemory(pNet->pNetHandle[i]);
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
BOOL GFL_NET_NegotiationRequest(GFL_NETHANDLE* pHandle)
{
    u8 id = GFL_NET_SystemGetCurrentID();
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    id = id * 8 + _numNetHandle(pNet, pHandle);
    pHandle->creatureNo = id;
    return GFL_NET_SendData(pHandle, GFL_NET_CMD_NEGOTIATION, &id);
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
 * @brief �ʐM������
 * @param[in]   pNetInit  �ʐM�������\���̂̃|�C���^
 * @param[in]   netID     �l�b�gID
 * @return none
 */
//==============================================================================
void GFL_NET_sysInit(const GFLNetInitializeStruct* pNetInit,int heapID)
{
    GFL_NETSYS* pNet = GFL_HEAP_AllocMemory(heapID, sizeof(GFL_NETSYS));
    _pNetSys = pNet;

    GFL_STD_MemClear(pNet, sizeof(GFL_NETSYS));

    OS_TPrintf("size %d addr %x",sizeof(GFLNetInitializeStruct),(u32)&pNet->aNetInit);

    GFL_STD_MemCopy(pNetInit, &pNet->aNetInit, sizeof(GFLNetInitializeStruct));
    pNet->heapID = heapID;
    if(pNetInit->bNetwork){
        GFL_NETHANDLE* pNetHandle = GFL_NET_CreateHandle();
        GFL_NET_StateDeviceInitialize(pNetHandle);
    }

    GFL_NET_CommandInitialize( pNetInit->recvFuncTable, pNetInit->recvFuncTableNum, pNetInit->pWork);
    
}

//==============================================================================
/**
 * @brief  �ʐM�I��
 * @retval  TRUE  �I�����܂���
 * @retval  FALSE �I�����܂��� ���Ԃ��󂯂Ă������Ă�ł�������
 */
//==============================================================================
BOOL GFL_NET_sysExit(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    _deleteAllNetHandle(pNet);
    GFL_HEAP_FreeMemory(pNet);
    _pNetSys = NULL;
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

void GFL_NET_sysMain(void)
{
    int i;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        if(pNet->pNetHandle[i]!=NULL){
            GFL_NET_StateMainProc(pNet->pNetHandle[i]);
        }
        if(pNet->pNetHandle[i]!=NULL){
            GFL_NET_ToolTimingSyncSend(pNet->pNetHandle[i]);
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

    GFL_NETHANDLE* pHandle = GFL_HEAP_AllocMemory(pNet->heapID, sizeof(GFL_NETHANDLE));
    GFL_STD_MemClear(pHandle, sizeof(GFL_NETHANDLE));
    _addNetHandle(pNet, pHandle);
    pHandle->pTool = GFL_NET_Tool_sysInit(pNet->heapID, pNet->aNetInit.maxConnectNum);
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
void GFL_NET_ClientConnectTo(GFL_NETHANDLE* pHandle,u8* macAddress)
{
    GFL_STD_MemCopy(macAddress, pHandle->aMacAddress, sizeof(pHandle->aMacAddress));
    GFL_NET_StateConnectMacAddress(pHandle);
}

//==============================================================================
/**
 * @brief   �q�@�ɂȂ�r�[�R�����W�߂�
 * @param   GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 * @return  none
 */
//==============================================================================
void GFL_NET_ClientConnect(GFL_NETHANDLE* pHandle)
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
void GFL_NET_ServerConnect(GFL_NETHANDLE* pHandle)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GFL_NET_StateConnectParent(pHandle, pNet->heapID);
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
 * @brief �ʐM�ؒf����
 * @retval  none
 */
//==============================================================================
void GFL_NET_Disconnect(void)
{
    int i,userNo = 0;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        if(pNet->pNetHandle[i]){
//            GFL_NET_SendData(pNet->pNetHandle[i],GFL_NET_CMD_EXIT,NULL); ///�I����S���ɑ��M
            userNo = i;
        }
    }
    GFL_NET_StateExit(pNet->pNetHandle[userNo]);
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
                                  FALSE, pNet->creatureNo ,NET_SENDID_ALLUSER);
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
    GFL_NET_ToolTimingSyncStart(pNet,no);
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
    return GFL_NET_ToolIsTimingSync(pNet, no);
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

