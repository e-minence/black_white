//=============================================================================
/**
 * @file	comm_tool.c
 * @brief	�ʐM���g�����ėp�֐�
 * @author	Katsumi Ohno
 * @date    2006.11.15
 */
//=============================================================================

#include "gflib.h"



//==============================================================================
// static�錾
//==============================================================================



typedef struct{
    u8 no;
    u8 listNo;
}_ListResult;

typedef struct _NET_TOOLSYS_t{
    u8* timingSyncBuff;   ///< �ʐM����̓����R�}���h�ԍ�
    u8 timingSyncEnd;     ///< �����R�}���h�p
    u8 timingSendE;       ///< ���������ǂ���
    u8 timingSyncMy;      ///< ������������NO
    u8 timingSendM;       ///< ���������ǂ���
};

//==============================================================================
/**
 * @brief   �l�b�g���[�N�c�[��������
 * @param   heapID    �q�[�v�m�ۂ��s��ID
 * @param   num       �������l��
 * @return  NET_TOOLSYS  �l�b�g���[�N�c�[���V�X�e�����[�N
 */
//==============================================================================

NET_TOOLSYS* GFL_NET_TOOL_sysInit(const int heapID, const int num)
{
    int i;
    NET_TOOLSYS* pCT;

    pCT = GFL_HEAP_AllocMemory(heapID, sizeof(NET_TOOLSYS));
    pCT->timingSync = GFL_HEAP_AllocMemory(heapID, num);
    MI_CpuFill8(pCT->timingSync, 0xff , num);
    pCT->timingSyncEnd = 0xff;
    pCT->timingSyncMy = 0xff;
    pCT->timingSend = FALSE;
    return pCT;
}

//==============================================================================
/**
 * @brief   �l�b�g���[�N�c�[���I��
 * @param   pCT    �l�b�g���[�N�c�[���V�X�e�����[�N
 * @return  none
 */
//==============================================================================

void GFL_NET_TOOL_sysEnd(NET_TOOLSYS* pCT)
{
    GFL_HEAP_FreeMemory(pCT->timingSync);
    GFL_HEAP_FreeMemory(pCT);
}

//==============================================================================
/**
 * @brief  �^�C�~���O�R�}���h����M����   CS_TIMING_SYNC
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================

void GFL_NET_TOOL_RecvTimingSync(const int netID, const int size, const void* pData,
                                 void* pWork, GFL_NETHandle* pNet)
{
    u8* pBuff = pData;
    u8 syncNo = pBuff[0];
    u8 sendBuff[2];
    int i;
    NET_TOOLSYS* _pCT = _NET_GetTOOLSYS(pNet);

    if(GFL_NET_GetCurrentID(pNet) == COMM_PARENT_ID){
        sendBuff[0] = netID;
        sendBuff[1] = syncNo;
        OHNO_PRINT("������M %d %d\n",netID,syncNo);
        pCT->timingSync[netID] = syncNo;     // �����R�}���h�p
        for(i = 0; i < COMM_MACHINE_MAX; i++){
            if(CommIsConnect(pNet, i)){
                if(syncNo != pCT->timingSync[i]){
                    // �������Ă��Ȃ�
                    return;
                }
            }
        }
        pCT->timingSendE = TRUE;
    }
}


//==============================================================================
/**
 * �^�C�~���O�R�}���hEND����M����   CS_TIMING_SYNC_END
 * @param[in]   netID  �ʐM���M��ID
 * @param[in]   size   ��M�f�[�^�T�C�Y
 * @param[in]   pData  ��M�f�[�^�|�C���^
 * @param[in,out]   pWork  �g�p�҂��K�v��work�̃|�C���^
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @retval      none
 */
//==============================================================================

void CommRecvTimingSyncEnd(const int netID, const int size, const void* pData,
                                 void* pWork, GFL_NETHandle* pNet)
{
    u8* pBuff = pData;
    u8 syncNo = pBuff[0];
    NET_TOOLSYS* _pCT = _NET_GetTOOLSYS(pNet);

    NET_PRINT("�S�������R�}���h��M %d %d\n", netID, syncNo);
    _pCT->timingSyncEnd = syncNo;     // �����R�}���h�p
}

//==============================================================================
/**
 * �^�C�~���O�R�}���h�𔭍s����
 * @param[in,out]   pNet  �ʐM�n���h���̃|�C���^
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================

void CommTimingSyncStart(GFL_NETHandle* pNet, u8 no)
{
    NET_TOOLSYS* _pCT = _NET_GetTOOLSYS(pNet);

    NET_PRINT("�R�}���h���s %d \n",no);
    _pCT->timingSyncMy = no;
    _pCT->timingSendM = TRUE;
}

//==============================================================================
/**
 * �^�C�~���O�R�}���h�𑗐M
 * @param   no   �^�C�~���O��肽���ԍ�
 * @retval  none
 */
//==============================================================================


#ifdef PM_DEBUG
extern int DebugCommGetNum(int id);
static u8 _keytrg = 0;
#endif

void CommTimingSyncSend(GFL_NETHandle* pNet)
{
    NET_TOOLSYS* _pCT = _NET_GetTOOLSYS(pNet);

    if(pNet->pCT){
        if(pNet->pCT->timingSendM){
            if(CommSendFixSizeData(CS_TIMING_SYNC, &_pCT->timingSyncMy)){
                _pCT->timingSendM = FALSE;
            }
        }
        if(pNet->pCT->timingSendE){
            if(CommSendFixSizeData(CS_TIMING_SYNC_END, &_pCT->timingSyncMy)){
                _pCT->timingSendE = FALSE;
            }
        }

        GFL_NET_SendData(pNet, CS_TIMING_SYNC_END, &syncNo);


    }

}

//==============================================================================
/**
 * �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   no   �͂��ԍ�
 * @retval  �͂��Ă�����TRUE
 */
//==============================================================================

BOOL CommIsTimingSync(GFL_NETHandle* pNet, u8 no)
{
    NET_TOOLSYS* _pCT = _NET_GetTOOLSYS(pNet);

    if(_pCT==NULL){
        return TRUE;  // �ʐM���ĂȂ��ꍇ�������Ă���Ƃ݂Ȃ�
    }
    if(_pCT->timingSyncEnd == no){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * �^�C�~���O�R�}���h���͂������ǂ������m�F����
 * @param   no   �͂��ԍ�
 * @retval  �͂��Ă�����TRUE
 */
//==============================================================================

int CommGetTimingSyncNo(int netID)
{
    return _pCT->timingSync[netID];
}

//------------------------------------------------------
/**
 * @brief   �I�����ʂ���M����֐�
 * @param   �R�[���o�b�N����
 * @retval  none
 */
//------------------------------------------------------

void CommRecvListNo(int netID, int size, void* pData, void* pWork)
{
    _ListResult* pRet = pData;

    _pCT->listNo[netID].no = pRet->no;
    _pCT->listNo[netID].listNo = pRet->listNo;
}

//------------------------------------------------------
/**
 * @brief   �I�����ʂ�ʐM����R�}���h�T�C�Y
 * @param   no        �I�����j�[�N�ԍ�
 * @param   listNo    �I������  0-255
 * @retval  none
 */
//------------------------------------------------------

int CommGetListNoSize(void)
{
    return sizeof(_ListResult);
}

//------------------------------------------------------
/**
 * @brief   �I�����ʂ�ʐM����
 * @param   no        �I�����j�[�N�ԍ�
 * @param   listNo    �I������  0-255
 * @retval  none
 */
//------------------------------------------------------

void CommListSet(u8 no,u8 listNo)
{
    _ListResult ret;

    ret.no = no;
    ret.listNo = listNo;
    CommSendFixSizeData(CS_LIST_NO,&ret);
}

//------------------------------------------------------
/**
 * @brief   �I�����ʂ��󂯎��
 * @param   recvNetID   �ǂ̐l�̃f�[�^��҂��󂯂�̂�
 * @param   pBuff       ��M�o�b�t�@
 * @retval  �I������ �������� -1
 */
//------------------------------------------------------

int CommListGet(int netID, u8 no)
{
    if(!_pCT){
        return INVALID_LIST_NO;
    }
    if(_pCT->listNo[netID].no == no){
        return _pCT->listNo[netID].listNo;
    }
    return INVALID_LIST_NO;
}


//------------------------------------------------------
/**
 * @brief   �I�����ʂ�����
 * @param   none
 * @retval  none
 */
//------------------------------------------------------

void CommListReset(void)
{
    int i;

    for(i = 0;i < COMM_MACHINE_MAX;i++){
        MI_CpuFill8(&_pCT->listNo[i], 0, sizeof(_ListResult));
    }
}

