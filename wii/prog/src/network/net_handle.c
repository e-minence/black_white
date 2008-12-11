//=============================================================================
/**
 * @file	net_handle.c
 * @brief	GFL�ʐM���C�u�����[  �ʐM�n���h��
 * @author	GAME FREAK Inc.  k.ohno
 * @date    2008.2.26
 */
//=============================================================================


#include "net.h"
#include "net_def.h"
#include "net_local.h"
#include "net_system.h"
#include "net_command.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"

#define _NEGOTIATION_START_TIME (10)


static u16 _setNegotiationBit(u16 checkbit,const int netID,const BOOL onoff);
static BOOL _getNegotiationBit(const u16 checkbit,const int netID);


//==============================================================================
//	�\���̒�`
//==============================================================================
///�l�S�V�G�[�V�����J�n��e�ɑ��M���鎞�ɑ���f�[�^
typedef struct{
	u8 MacAddress[6];		///<���M�҂�MAC�A�h���X
	u8 netid;				///<���M�҂�netid
	u8 padding;				//�p�f�B���O
}NEGOTIATION_REQ_DATA;

///�l�S�V�G�[�V������M��e���瑗���Ă���f�[�^
typedef struct{
	u8 parent_MacAddress[6];	///<�e��MAC�A�h���X
	u8 child_MacAddress[6];		///<�l�S�V�G�[�V�����𑗐M���Ă����q��MAC�A�h���X
	u8 child_netid;				///<�l�S�V�G�[�V�����𑗐M���Ă����q��netid
    u8 bInit;                   ///<�������������ǂ���
    u8 padding[2];				//�p�f�B���O
}NEGOTIATION_RETURN_DATA;

//==============================================================================
/**
 * @brief       index����netid�ւ̌��т��ɕϊ�
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @param       pHandle   �ʐM�n���h��
 * @return      �ǉ������ԍ�
 */
//==============================================================================
static int _indexToNetID(int index)
{
    if(index >= GFL_NET_MACHINE_MAX){
        return GFL_NET_NETID_SERVER;
    }
    return index;
}

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

    if(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER) == pHandle){
        return GFL_NET_NETID_SERVER;
    }
    for(i = 0;i < GFL_NET_HANDLE_MAX; i++){
        if(GFL_NET_GetNetHandle(i) == pHandle){
            return i;
        }
    }
    GF_ASSERT(0);
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
 * @param   netID    �ʐMID 0-15 + GFL_NET_NETID_SERVER
 * @return  GFL_NETHANDLE  �ʐM�n���h���̃|�C���^
 */
//==============================================================================
GFL_NETHANDLE* GFL_NET_InitHandle(int netID)
{
    GFL_NETSYS* pNet;
    GFL_NETHANDLE* pHandle;
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

    pNet = _GFL_NET_GetNETSYS();
    pHandle = GFL_NET_GetNetHandle(netID);
    if(netID == GFL_NET_NETID_SERVER){
        int i;
        for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
            if(pHandle->negoBuff[i]!=NULL){
                GFL_HEAP_FreeMemory(pHandle->negoBuff[i]);
            }
        }
    }
    GFL_STD_MemClear(pHandle, sizeof(GFL_NETHANDLE));
    if(netID == GFL_NET_NETID_SERVER){
        int i;
        for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
            pHandle->negoBuff[i] = GFL_HEAP_AllocClearMemory(pNetIni->netHeapID ,sizeof(NEGOTIATION_RETURN_DATA));
        }
    }
    pHandle->negotiationType = _NEG_TYPE_FIRST;
    pHandle->timingSyncEnd = 0xff;
    pHandle->timingSyncMy = 0xff;
    pHandle->timingSendFlg = FALSE;
    return pHandle;
}

//==============================================================================
/**
 * @brief       netHandle��S�ď���������
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @return      none
 */
//==============================================================================
void GFI_NET_HANDLE_InitAll(GFL_NETSYS* pNet)
{
    int netID;

    for(netID = 0;netID < GFL_NET_MACHINE_MAX;netID++){
        GFL_NET_InitHandle(netID);
    }
    GFL_NET_InitHandle(GFL_NET_NETID_SERVER);
}

//==============================================================================
/**
 * @brief       netHandle��S�ď���
 * @param       pNet      �ʐM�V�X�e���Ǘ��\����
 * @return      none
 */
//==============================================================================
void GFI_NET_HANDLE_DeleteAll(GFL_NETSYS* pNet)
{
    GFL_NETHANDLE* pHandle;
    int i;

    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        _deleteHandle(GFL_NET_GetNetHandle(i));
    }
    pHandle=GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);
    for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
        if(pHandle->negoBuff[i]!=NULL){
            GFL_HEAP_FreeMemory(pHandle->negoBuff[i]);
            pHandle->negoBuff[i]=NULL;
        }
    }
    _deleteHandle(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER));
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
    if(GFL_NET_NETID_SERVER == netID){
        return &pNet->aNetHandle[GFL_NET_HANDLE_MAX-1];
    }
    GF_ASSERT(netID < GFL_NET_HANDLE_MAX);
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
    return GFL_NET_GetNetHandle(GFL_NET_SystemGetCurrentID());
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
 * @brief       �l�S�V�G�[�V�������������Ă���̂����l�� SERVER�͐����Ȃ�
 * @return      ����ł���l�� 
 */
//==============================================================================
int GFL_NET_HANDLE_GetNumNegotiation(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    int i,num=0;

    if(pNet){
        for(i = 0;i < GFL_NET_MACHINE_MAX;i++){
            if(GFL_NET_HANDLE_IsNegotiation(GFL_NET_GetNetHandle(i))){
                num++;
            }
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
			NEGOTIATION_REQ_DATA send_data;
			
            OS_TPrintf("GFL_NET_CMD_NEGOTIATION %d\n",id);
            OS_GetMacAddress(send_data.MacAddress);
            send_data.netid = id;
            return GFL_NET_SendData(pHandle, GFL_NET_CMD_NEGOTIATION, sizeof(NEGOTIATION_REQ_DATA), &send_data); //NETID�𑗂�
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

    if(GFL_NET_HANDLE_GetNetHandleID(pNet) == GFL_NET_NETID_SERVER){
        pNet->timingR[netID] = syncNo;       ///< �q�@����̎󂯎��L�^

        NET_PRINT("������M %d %d \n",netID,syncNo);

        for(i = 0; i < GFL_NET_MACHINE_MAX; i++){  // �q�@�̕����̃n���h��������
            if(GFL_NET_HANDLE_IsNegotiation(GFL_NET_GetNetHandle(i))){
                if(pNet->timingR[i] != syncNo){
                    NET_PRINT("�������ĂȂ�%d %d\n", i, syncNo);
                    pNet->timingSendFlg = FALSE;
                    return;
                }
            }
            else if(GFL_NET_SystemIsConnect(i) == TRUE){
				OS_TPrintf("�l�S�V�G�[�V�������ς�ł��Ȃ� %d %d\n", i, syncNo);
				return;
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

    NET_PRINT("�S�������R�}���h��M %d %d %d\n", netID, syncNo,GFL_NET_HANDLE_GetNetHandleID(pNet));
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
    if(GFL_NET_HANDLE_GetNetHandleID(pNet) == GFL_NET_NETID_SERVER){
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
 * @brief   �e����q�@�l�����̃l�S�V�G�[�V������Ԃ�
 * @retval  none
 */
//==============================================================================

static BOOL _sendParentNegotiation(GFL_NETHANDLE* pNetHandle)
{
    int j;

    for(j = 0;j < GFL_NET_MACHINE_MAX; j++){
        NEGOTIATION_RETURN_DATA* send_data = (NEGOTIATION_RETURN_DATA*)pNetHandle->negoBuff[j];
        if(send_data->bInit){
            if(_getNegotiationBit(pNetHandle->negotiationBit, j) == FALSE){
                if(GFL_NET_SendDataEx(pNetHandle,GFL_NET_SENDID_ALLUSER,
                                      GFL_NET_CMD_NEGOTIATION_RETURN,
                                      sizeof(NEGOTIATION_RETURN_DATA),pNetHandle->negoBuff[j],
                                      TRUE, FALSE, TRUE)){
                    _setNegotiationBit(pNetHandle->negotiationBit, j, TRUE);
                }
                else{
                    return FALSE;
                }
            }
        }
    }
    pNetHandle->negotiationBit = 0;
    return TRUE;
}

//==============================================================================
/**
 * @brief   �n���h�������̃��C��
 * @retval  none
 */
//==============================================================================

void GFL_NET_HANDLE_MainProc(void)
{
    int i,j, num=0, netID;
    GFL_NETHANDLE* pNetHandle;
    
    for(i = 0;i < GFL_NET_HANDLE_MAX;i++){
        netID = _indexToNetID(i);
        pNetHandle = GFL_NET_GetNetHandle(netID);
        if(FALSE == GFL_NET_IsSendEnable(pNetHandle)){
            continue;
        }
        if(!pNetHandle->timingSendFlg){
            continue;
        }
        if(GFL_NET_HANDLE_GetNetHandleID(pNetHandle) == GFL_NET_NETID_SERVER){
            if(GFL_NET_SendData(pNetHandle, GFL_NET_CMD_TIMING_SYNC_END, 1, &pNetHandle->timingSyncMy)){
                NET_PRINT("�����I���𑗐M\n");
                pNetHandle->timingSendFlg = FALSE;
            }
        }
        else{
            if(GFL_NET_SendData(pNetHandle, GFL_NET_CMD_TIMING_SYNC, 1, &pNetHandle->timingSyncMy)){
                NET_PRINT("TIMING_SYNC %d\n",netID);
                pNetHandle->timingSendFlg = FALSE;
            }
        }
        if(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER) == pNetHandle){
            NET_PRINT("OYA \n");

        }
    }
    if(GFL_NET_SystemGetCurrentID()==GFL_NET_NO_PARENTMACHINE){
        pNetHandle = GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);

        for(j = 0;j < GFL_NET_MACHINE_MAX; j++){
            NEGOTIATION_RETURN_DATA* send_data = (NEGOTIATION_RETURN_DATA*)pNetHandle->negoBuff[j];
            if(send_data->bInit){
                if(_getNegotiationBit(pNetHandle->negotiationReturn,j) == FALSE){
                    if(_sendParentNegotiation(pNetHandle)){
                        pNetHandle->negotiationReturn = _setNegotiationBit(pNetHandle->negotiationReturn, j, TRUE);
                    }
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
    const NEGOTIATION_REQ_DATA *recv_data = (NEGOTIATION_REQ_DATA*)pData;
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
    NEGOTIATION_RETURN_DATA* send_data;

    
    if(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER) == pNetHandle){
        send_data=(NEGOTIATION_RETURN_DATA*)pNetHandle->negoBuff[netID];
        if(!send_data->bInit){
            send_data->bInit=TRUE;
            GFL_STD_MemCopy(recv_data->MacAddress, send_data->child_MacAddress, sizeof(recv_data->MacAddress));
            NET_PRINT("RecvNegotiation %d %d\n",netID, recv_data->netid);
            send_data->child_netid = recv_data->netid;
            OS_GetMacAddress(send_data->parent_MacAddress);
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
    const u8* pMsg = (u8*)pData;
	const NEGOTIATION_RETURN_DATA *recv_data = (NEGOTIATION_RETURN_DATA*)pData;
    GFLNetInitializeStruct* pNetInit = _GFL_NET_GetNETInitStruct();
	
    if(GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER) == pNetHandle){
        pNetHandle->negotiationType = _NEG_TYPE_CONNECT;
        return;
    }
    else{



    #if 0	//��check�@�����������������̂ŕK�����s����B�Ƃ肠�����O�̂ɖ߂�
		u8 mac_address[6];
		
        OS_GetMacAddress(mac_address);
		if(recv_data->netid == GFL_NET_HANDLE_GetNetHandleID(GFL_NET_HANDLE_GetCurrentHandle())
				&& GFL_STD_MemComp(recv_data->MacAddress, mac_address, 6) == 0){
	        pNetHandle = GFL_NET_GetNetHandle(recv_data->netid);
    	    pNetHandle->negotiationType = _NEG_TYPE_CONNECT;
        	NET_PRINT("�ڑ��F�� OK %d\n",recv_data->netid);
        }
        else{
			GF_ASSERT(0 && "�F�؎��s");
		}
	#else
        pNetHandle = GFL_NET_GetNetHandle(recv_data->child_netid);
        pNetHandle->negotiationType = _NEG_TYPE_CONNECT;
        NET_PRINT("�ڑ��F�� OK %d\n",recv_data->child_netid);
	#endif

        if(pNetInit->connectNegotiationFunc){  // �R�[���o�b�N�Ăяo��
            pNetInit->connectNegotiationFunc(pWork, recv_data->child_netid);
        }
	//	GFL_NET_IW_ParentMacAddressSet(recv_data->parent_MacAddress);
    }
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�����̑���
 * @param   pNetHandle  �n���h��
 * @param   netID    �ʐMID
 * @param   onoff    ONOFF
 * @retval  none
 */
//==============================================================================

static u16 _setNegotiationBit(u16 negotiationBit,const int netID,const BOOL onoff)
{
    GF_ASSERT(netID!=GFL_NET_NETID_SERVER);
    {
        u8 bit = 0x01 << netID;
        if(onoff){
            negotiationBit |= bit;
        }
        else{
            negotiationBit ^= bit;
        }
    }
    return negotiationBit;
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�����̊m�F
 * @param   pNetHandle  �n���h��
 * @param   netID    �ʐMID
 * @retval  ON�Ȃ�TRUE
 */
//==============================================================================

static BOOL _getNegotiationBit(u16 checkbit,const int netID)
{
    GF_ASSERT(netID!=GFL_NET_NETID_SERVER);
    {
        u8 bit = 0x01 << netID;
        if( checkbit & bit){
            return TRUE;
        }
    }
    return FALSE;
}

