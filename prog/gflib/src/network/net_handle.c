//=============================================================================
/**
 * @file	net_handle.c
 * @brief	GFL�ʐM���C�u�����[  �ʐM�n���h��
 * @author	GAME FREAK Inc.  k.ohno
 * @date    2008.2.26
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

#define _NEGOTIATION_START_TIME (10)

//==============================================================================
/**
 * @brief       netHandle�̔ԍ��𓾂�
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @param       pHandle   �ʐM�n���h��
 * @return      �ǉ������ԍ�
 */
//==============================================================================
int GFL_NET_HANDLE_GetNetHandleID(GFL_NETHANDLE* pHandle)
{
    int i;
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

    for(i = 0;i < GFL_NET_HANDLE_MAX; i++){
        if(&pNet->aNetHandle[i] == pHandle){
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
    GFL_STD_MemClear(pNetHandle, sizeof(GFL_NETHANDLE));
}

//==============================================================================
/**
 * @brief   �ʐM�n���h��������������
 * @param   netID    �ʐMID
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
GFL_NETHANDLE* GFL_NET_InitHandle(int netID)
{
    GFL_NETSYS* pNet;
    GFL_NETHANDLE* pHandle;
    GF_ASSERT(netID < GFL_NET_MACHINE_MAX);

    pNet = _GFL_NET_GetNETSYS();
    pHandle = &pNet->aNetHandle[netID+1];  //�n���h���̂O�͐e 1-�͎q

    GFL_STD_MemClear(pHandle, sizeof(GFL_NETHANDLE));
    pHandle->negotiationType = _NEG_TYPE_FIRST;
    pHandle->timingSyncEnd = 0xff;
    pHandle->timingSyncMy = 0xff;
    pHandle->timingSendFlg = FALSE;
    return pHandle;
}

//==============================================================================
/**
 * @brief       netHandle��S�ď���
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @return      none
 */
//==============================================================================
void GFL_NET_HANDLE_DeleteAll(GFL_NETSYS* pNet)
{
    int i;

    for(i = 0;i < GFL_NET_HANDLE_MAX;i++){
        _deleteHandle(&pNet->aNetHandle[i]);
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
    return &pNet->aNetHandle[netID];
}

//==============================================================================
/**
 * @brief   �����̃}�V���̃n���h�����A��
 * @retval  GFL_NETHANDLE
 */
//==============================================================================

GFL_NETHANDLE* GFL_NET_HANDLE_GetCurrentHandle(void)
{
    return GFL_NET_GetNetHandle(GFL_NET_SystemGetCurrentID()+1);
}

//==============================================================================
/**
 * @brief       �����̃l�S�V�G�[�V����������ł��邩�ǂ���
 * @param[in]   pHandle   �ʐM�n���h��
 * @return      ����ł���ꍇTRUE   �܂��̏ꍇFALSE
 */
//==============================================================================
BOOL GFL_NET_HANDLE_IsNegotiation(GFL_NETHANDLE* pHandle)
{
    if(pHandle){
        return (pHandle->negotiationType == _NEG_TYPE_CONNECT);
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief       �l�S�V�G�[�V�������������Ă���̂����l��
 * @return      ����ł���l�� 
 */
//==============================================================================
int GFL_NET_HANDLE_GetNumNegotiation(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    int i,num=0;

    for(i = 1;i < GFL_NET_HANDLE_MAX;i++){
        if(GFL_NET_HANDLE_IsNegotiation(&pNet->aNetHandle[i])){
            num++;
        }
    }
    return num;
}

//==============================================================================
/**
 * @brief       �l�S�V�G�[�V�����J�n��e�ɑ��M����
 * @param[in]   pHandle   �ʐM�n���h��
 * @return      ���M�ɐ���������TRUE  ���s�����������񑗐M
 */
//==============================================================================
BOOL GFL_NET_HANDLE_RequestNegotiation(void)
{
    GFL_NETHANDLE* pHandle = GFL_NET_HANDLE_GetCurrentHandle();
    u8 id = GFL_NET_HANDLE_GetNetHandleID(pHandle);
    GFL_NETSYS* pNetSys = _GFL_NET_GetNETSYS();

    if(GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID())){  // �����I�Ȑڑ�
        if(pHandle->negotiationType == _NEG_TYPE_SEND){
            OS_TPrintf("GFL_NET_CMD_NEGOTIATION %d\n",id);
            return GFL_NET_SendData(pHandle, GFL_NET_CMD_NEGOTIATION, &id); //NETID�𑗂�
        }
        else if(pHandle->negotiationType < _NEG_TYPE_SEND){
            pHandle->negotiationType++;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief  �^�C�~���O�R�}���h����M����   GFL_NET_CMD_TIMING_SYNC
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================

void GFL_NET_HANDLE_RecvTimingSync(const int netID, const int size, const void* pData,
                                void* pWork, GFL_NETHANDLE* pNet)
{
    const u8* pBuff = (const u8*)pData;
    u8 syncNo = pBuff[0];
    int i;

    if(GFL_NET_HANDLE_GetNetHandleID(pNet) == GFL_NET_PARENT_NETID){
        pNet->timing[netID] = syncNo;       ///< �q�@����̎󂯎��L�^

        NET_PRINT("������M %d %d \n",netID,syncNo);

        for(i = 1; i < GFL_NET_HANDLE_MAX; i++){
            if(GFL_NET_HANDLE_IsNegotiation(GFL_NET_GetNetHandle(i))){
                if(pNet->timing[i] != syncNo){
                    NET_PRINT("�������ĂȂ�%d %d\n", i, syncNo);
                    pNet->timingSendFlg = FALSE;
                    return;
                }
            }
        }
        NET_PRINT("�����o�m�F����\n");
        pNet->timingSendFlg = TRUE;
        pNet->timingSyncMy = syncNo;
    }
}


//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���hEND����M����   GFL_NET_CMD_TIMING_SYNC_END
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================

void GFL_NET_HANDLE_RecvTimingSyncEnd(const int netID, const int size, const void* pData,
                                 void* pWork, GFL_NETHANDLE* pNet)
{
    const u8* pBuff = (const u8*)pData;
    u8 syncNo = pBuff[0];

    NET_PRINT("�S�������R�}���h��M %d %d\n", netID, syncNo);
    pNet->timingSyncEnd = syncNo;     // �����R�}���h�p
}

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_HANDLE_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no)
{
    NET_PRINT("SyncStart %d \n",no);
    pNet->timingSyncMy = no;
    if(GFL_NET_HANDLE_GetNetHandleID(pNet) == GFL_NET_PARENT_NETID){
        pNet->timingSendFlg = FALSE;  // �e�݂͂�Ȃ���W�܂��Ă��瑗�M
    }
    else{
        pNet->timingSendFlg = TRUE;  // �q�͂����ɑ��M
    }
}

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   pNet   �l�b�g���[�N�n���h��
 * @param   no     ��������ԍ�
 * @retval  TRUE   �͂��Ă���
 * @retval  FALSE  �͂��Ă��Ȃ�
 */
//==============================================================================

BOOL GFL_NET_HANDLE_IsTimingSync(const GFL_NETHANDLE* pNet, const u8 no)
{
    if(pNet->timingSyncEnd == no){
        NET_PRINT("�m�F %d\n",pNet->timingSyncEnd);
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �n���h�������̃��C��
 * @retval  none
 */
//==============================================================================

void GFL_NET_HANDLE_MainProc(void)
{
    int i,num=0;
    GFL_NETHANDLE* pNet;
    
    for(i = 0;i < GFL_NET_HANDLE_MAX;i++){
        pNet = GFL_NET_GetNetHandle(i);
        if(FALSE == GFL_NET_IsSendEnable(pNet)){
            continue;
        }
        if(pNet->timingSendFlg){
            if(i == GFL_NET_PARENT_NETID){
                if(GFL_NET_SendData(pNet, GFL_NET_CMD_TIMING_SYNC_END, &pNet->timingSyncMy)){
                    NET_PRINT("�����I���𑗐M\n");
                    pNet->timingSendFlg = FALSE;
                 }
            }
            else{
                if(GFL_NET_SendData(pNet, GFL_NET_CMD_TIMING_SYNC, &pNet->timingSyncMy)){
                    NET_PRINT("TIMING_SYNC %d\n",i);
                    pNet->timingSendFlg = FALSE;
                }
            }
        }
    }
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N GFL_NET_CMD_NEGOTIATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void GFL_NET_HANDLE_RecvNegotiation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    // �e�@���󂯎���ăt���O�𗧂Ă�
    u8 retCmd[2];
    const u8 *pGet = (const u8*)pData;
    int i;


    NET_PRINT("RecvNegotiation %d \n",netID);
    
    retCmd[1] = pGet[0];
    retCmd[0] = netID;

    pNetHandle->negotiationSendFlg = FALSE;
    if(GFL_NET_GetNetHandle(GFL_NET_PARENT_NETID) == pNetHandle){
        if(pNetHandle->negotiationBit == 0){
            GFL_NET_SendData(pNetHandle, GFL_NET_CMD_NEGOTIATION_RETURN, retCmd);
        }
    }
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N GFL_NET_CMD_NEGOTIATION_RETURN
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void GFL_NET_HANDLE_RecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pMsg = pData;

    if(GFL_NET_GetNetHandle(GFL_NET_PARENT_NETID) == pNetHandle){
        pNetHandle->negotiationType = _NEG_TYPE_CONNECT;
        return;
    }
    else{
        pNetHandle = GFL_NET_GetNetHandle(pMsg[0]);
        pNetHandle->negotiationType = _NEG_TYPE_CONNECT;
        OS_TPrintf("�ڑ��F�� OK %d\n",pMsg[0]);
    }
}

