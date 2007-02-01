//=============================================================================
/**
 * @file	net_tool.c
 * @brief	�ʐM���g�����ėp�֐�
 * @author	GAME FREAK Inc.
 * @date    2006.11.15
 */
//=============================================================================

#include "gflib.h"

#include "../net_def.h"
#include "net_tool.h"
#include "net_command.h"
#include "../net_system.h"

//==============================================================================
// static�錾
//==============================================================================

struct _NET_TOOLSYS_t{
    u8* timingSyncBuff;   ///< �ʐM����̓����R�}���h�ԍ�
    u8 timingSyncEnd;     ///< �����R�}���h�p
    u8 timingSendE;       ///< ���������ǂ���
    u8 timingSyncMy;      ///< ������������NO
    u8 timingSendM;       ///< ���������ǂ���
} ;

//==============================================================================
/**
 * @brief   �l�b�g���[�N�c�[��������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @param   num       �������l��
 * @return  NET_TOOLSYS  �l�b�g���[�N�c�[���V�X�e�����[�N
 */
//==============================================================================

NET_TOOLSYS* GFL_NET_Tool_sysInit(const int heapID, const int num)
{
    int i;
    NET_TOOLSYS* pCT;

    pCT = GFL_HEAP_AllocMemory(heapID, sizeof(NET_TOOLSYS));
    pCT->timingSyncBuff = GFL_HEAP_AllocMemory(heapID, num);
    MI_CpuFill8(pCT->timingSyncBuff, 0xff , num);
    pCT->timingSyncEnd = 0xff;
    pCT->timingSyncMy = 0xff;
    pCT->timingSendE = FALSE;
    pCT->timingSendM = FALSE;
    return pCT;
}

//==============================================================================
/**
 * @brief   �l�b�g���[�N�c�[���I��
 * @param   pCT    �l�b�g���[�N�c�[���V�X�e�����[�N
 * @return  none
 */
//==============================================================================

void GFL_NET_Tool_sysEnd(NET_TOOLSYS* pCT)
{
    if(pCT){
        GFL_HEAP_FreeMemory(pCT->timingSyncBuff);
        GFL_HEAP_FreeMemory(pCT);
    }
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

void GFL_NET_ToolRecvTimingSync(const int netID, const int size, const void* pData,
                                void* pWork, GFL_NETHANDLE* pNet)
{
    const u8* pBuff = (const u8*)pData;
    u8 syncNo = pBuff[0];
    u8 sendBuff[2];
    int i;
    NET_TOOLSYS* pCT = _NETHANDLE_GetTOOLSYS(pNet);

    if(pNet->pParent){
        sendBuff[0] = netID;
        sendBuff[1] = syncNo;
        NET_PRINT("������M %d %d\n",netID,syncNo);
        pCT->timingSyncBuff[netID] = syncNo;     // �����R�}���h�p
        for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
            if(GFL_NET_IsConnectMember(pNet, i)){
                NET_PRINT("�����o�m�F%d\n",i);
                if(syncNo != pCT->timingSyncBuff[i]){
                    NET_PRINT("�����o�m�F �������ĂȂ�%d %d %d\n", i, syncNo, pCT->timingSyncBuff[i]);
                    // �������Ă��Ȃ�
                    pCT->timingSendE = FALSE;
                    return;
                }
            }
        }
        NET_PRINT("�����o�m�F����\n");
        pCT->timingSyncMy = syncNo;
        pCT->timingSendE = TRUE;
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

void GFL_NET_ToolRecvTimingSyncEnd(const int netID, const int size, const void* pData,
                                 void* pWork, GFL_NETHANDLE* pNet)
{
    const u8* pBuff = (const u8*)pData;
    u8 syncNo = pBuff[0];
    NET_TOOLSYS* _pCT = _NETHANDLE_GetTOOLSYS(pNet);

    NET_PRINT("�S�������R�}���h��M %d %d\n", netID, syncNo);
    _pCT->timingSyncEnd = syncNo;     // �����R�}���h�p
}

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_ToolTimingSyncStart(GFL_NETHANDLE* pNet, const u8 no)
{
    NET_TOOLSYS* _pCT = _NETHANDLE_GetTOOLSYS(pNet);

    NET_PRINT("�R�}���h���s %d \n",no);
    _pCT->timingSyncMy = no;
    _pCT->timingSendM = TRUE;
}

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h�𑗐M
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_ToolTimingSyncSend(GFL_NETHANDLE* pNet)
{
    NET_TOOLSYS* pCT = _NETHANDLE_GetTOOLSYS(pNet);

    if(FALSE == GFL_NET_IsSendEnable(pNet)){
        return;
    }
    
    if(pCT->timingSendM){
        if(GFL_NET_SendData(pNet, GFL_NET_CMD_TIMING_SYNC, &pCT->timingSyncMy)){
            pCT->timingSendM = FALSE;
        }
    }
    if(pCT->timingSendE){
        if(GFL_NET_SendData(pNet, GFL_NET_CMD_TIMING_SYNC_END, &pCT->timingSyncMy)){
            NET_PRINT("�����I���𑗐M\n");
            pCT->timingSendE = FALSE;
        }
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

BOOL GFL_NET_ToolIsTimingSync(GFL_NETHANDLE* pNet, const u8 no)
{
    NET_TOOLSYS* pCT = _NETHANDLE_GetTOOLSYS(pNet);

    NET_PRINT("�m�F %d\n",pCT->timingSyncEnd);
    if(pCT->timingSyncEnd == no){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   no   �͂��ԍ�
 * @retval  �͂��Ă�����TRUE
 */
//==============================================================================

int GFL_NET_ToolGetTimingSyncNo(GFL_NETHANDLE* pNet, int netID)
{
    NET_TOOLSYS* pCT = _NETHANDLE_GetTOOLSYS(pNet);
    return pCT->timingSyncBuff[netID];
}

