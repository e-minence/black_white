//=============================================================================
/**
 * @file	net_queue.c
 * @brief	�i���M�j�L���[�̎d�g�݂��Ǘ�����֐�
 * @author	katsumi ohno
 * @date	06/01/29
 */
//=============================================================================

#include "gflib.h"
#include "../net_def.h"
#include "../net_command.h"
#include "net_ring_buff.h"
#include "net_queue.h"

//==============================================================================
/**
 * @brief   �󂢂Ă���L���[��Ԃ�
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  SEND_QUEUE  �L���[CELL
 */
//==============================================================================

static SEND_QUEUE* _freeQueue(SEND_QUEUE_MANAGER* pQueueMgr)
{
    SEND_QUEUE* pFree = pQueueMgr->heapTop;
    int i;

    for(i = 0;i < pQueueMgr->max; i++){
        if(pFree->command == GFL_NET_CMD_FREE){
            return pFree;
        }
        pFree++;
    }
    return NULL;   // �R�}���h������
}

//==============================================================================
/**
 * @brief   �L���[�ɉ��������Ă��邩�ǂ������m�F����
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  TRUE   ��
 * @retval  FALSE  �����Ă���
 */
//==============================================================================

BOOL GFL_NET_QueueIsEmpty(SEND_QUEUE_MANAGER* pQueueMgr)
{
    return (0 == GFL_NET_QueueGetNowNum(pQueueMgr));
}

//==============================================================================
/**
 * @brief   �L���[�̐��𓾂�
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @return  �g�p���Ă���L���[�̐�
 */
//==============================================================================

int GFL_NET_QueueGetNowNum(SEND_QUEUE_MANAGER* pQueueMgr)
{
    SEND_QUEUE* pFree = pQueueMgr->heapTop;
    int i,j = 0;

    for(i = 0;i < pQueueMgr->max; i++){
        if(pFree->command != GFL_NET_CMD_FREE){
            j++;
        }
        pFree++;
    }
    return j;
}

//==============================================================================
/**
 * @brief   �L���[������
 * @param   pTerm   ����L���[�Ǘ�
 * @retval  TRUE   ������
 * @retval  FALSE  �������Ƃ��ł��Ȃ�����
 */
//==============================================================================

static BOOL _deleteQueue(SEND_TERMINATOR* pTerm)
{
    if(pTerm->pTop != NULL){
        if(pTerm->pTop->next!= NULL){
            pTerm->pTop = pTerm->pTop->next;
            pTerm->pTop->prev = NULL;
        }
        else{
            pTerm->pTop = NULL;
            pTerm->pLast = NULL;
        }
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   ���M�f�[�^�̈�o�C�g���o�b�t�@�ɓ����
 * @param   pSendBuff   ���M�o�b�t�@�Ǘ��\���̃|�C���^
 * @param   byte        �Z�b�g����f�[�^
 * @return  none
 */
//==============================================================================

static void _setSendData(SEND_BUFF_DATA* pSendBuff ,u8 byte)
{
    *pSendBuff->pData = byte;
    pSendBuff->pData++;
    pSendBuff->size--;
}

//==============================================================================
/**
 * @brief   ���M�f�[�^�̃w�b�_�[�������o�b�t�@�ɓ����
 * @param   pQueue  �L���[�̃|�C���^
 * @param   pSendBuff   ���M�o�b�t�@�Ǘ��\���̃|�C���^
 * @retval  FALSE  �~����
 * @retval  TRUE   ����Ȃ�����
 */
//==============================================================================

static BOOL _dataHeadSet(SEND_QUEUE* pQueue, SEND_BUFF_DATA* pSendBuff)
{
    int cs = GFL_NET_CommandGetPacketSize(pQueue->command);

    if( cs == GFL_NET_COMMAND_SIZE_VARIABLE ){
        if(pSendBuff->size < 3){
            pQueue->bHeadSet = FALSE;
            return TRUE;
        }
    }
    else{
        if(pSendBuff->size < 1){
            pQueue->bHeadSet = FALSE;
            return TRUE;
        }
    }
    _setSendData(pSendBuff,pQueue->command);
    if(cs == GFL_NET_COMMAND_SIZE_VARIABLE){
        _setSendData(pSendBuff,(u8)((pQueue->size >> 8)  & 0xff));
        _setSendData(pSendBuff,(u8)(pQueue->size & 0xff));
    }
    else{
        pQueue->size = (u16)cs;
    }
    pQueue->bHeadSet = TRUE;
    return FALSE;
}

//==============================================================================
/**
 * @brief   �L���[���瑗�M�o�b�t�@�Ɉڂ�
 * @param   pQueue  �L���[
 * @param   pSendBuff   ���M�o�b�t�@
 * @param   pSendRing   ���M�����O�o�b�t�@
 * @param   bNextPlus   �f�[�^�����Ƀw�b�_�[�������܂߂邩�ǂ���
 * @retval  FALSE �w�b�_�[������Ȃ������ꍇ
 * @retval  TRUE  �͂�����
 */
//==============================================================================

static BOOL _dataCopyQueue(SEND_QUEUE* pQueue, SEND_BUFF_DATA* pSendBuff,
                           RingBuffWork* pSendRing, BOOL bNextPlus)
{
    int i;
    int size;
    int cs = GFL_NET_CommandGetPacketSize(pQueue->command);
    if( cs == GFL_NET_COMMAND_SIZE_VARIABLE){
        size = 3;
    }
    else{
        size = 1;
    }
    if((pSendBuff->size < (pQueue->size + size)) && (!bNextPlus)){  // �L���[�̂ق����ł����ꍇ
        return FALSE;
    }
    if(pQueue->bHeadSet != TRUE){
        if(_dataHeadSet(pQueue, pSendBuff)){
            return FALSE;
        }
    }
//    if((pSendBuff->size < pQueue->size) && (!bNextPlus)){  // �L���[�̂ق����ł����ꍇ
//        return FALSE;  // �O�Ɉړ�
//    }
    if(pSendBuff->size < pQueue->size){  // �L���[�̂ق����ł����ꍇ
        if(pQueue->bRing){
            GFL_NET_RingGets(pSendRing, pSendBuff->pData, pSendBuff->size);
        }
        else{
          //  NET_PRINT("-----%x-%x--%d\n",(int)pQueue->pData, (int)pSendBuff->pData , pSendBuff->size);
//            MI_CpuCopy8( pQueue->pData, pSendBuff->pData, pSendBuff->size);
            
            for(i = 0;i < pSendBuff->size;i++){
                pSendBuff->pData[i] = pQueue->pData[i];
            }
        }
//        NET_PRINT("-----%d-%d--\n",pQueue->command, pQueue->size);
        
        pQueue->pData += pSendBuff->size;  // �A�h���X��i�߂�
        pQueue->size -= pSendBuff->size;  // ���M�T�C�Y���炷
        pSendBuff->size = -1;  // �c���-1�ɂ���
        return TRUE;
    }
    // �����������͏������ꍇ
    if(pQueue->bRing){
        GFL_NET_RingGets(pSendRing, pSendBuff->pData, pQueue->size);
    }
    else{
        MI_CpuCopy8( pQueue->pData, pSendBuff->pData, pQueue->size);
    }
    pSendBuff->pData += pQueue->size;  // �A�h���X��i�߂�
    pSendBuff->size -= pQueue->size; // �T�C�Y�͌��炷
    return TRUE;
}


//==============================================================================
/**
 * @brief   �L���[��~����
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @param   command   ���M�R�}���h
 * @param   pDataArea  ���M�f�[�^
 * @param   size    �T�C�Y
 * @param   bFast  �D��x�������f�[�^?
 * @param   bSave  �ۑ����邩�ǂ���
 * @retval  TRUE �~����
 * @retval  FALSE �L���[�ɓ���Ȃ�����
 */
//==============================================================================

BOOL GFL_NET_QueuePut(SEND_QUEUE_MANAGER* pQueueMgr,int command, u8* pDataArea,
                      int size, BOOL bFast, BOOL bSave)
{
    SEND_QUEUE* pLast;
    SEND_QUEUE* pFree = _freeQueue(pQueueMgr);
    SEND_TERMINATOR* pTerm;
    int cSize;

    bFast = TRUE;
    if(pFree== NULL){
        NET_PRINT("---�L���[������\n");
        return FALSE;
    }
    
    GF_ASSERT(size < 65534 && "65534�ȏ�͕���");
    cSize = GFL_NET_CommandGetPacketSize(command);

    if(GFL_NET_COMMAND_SIZE_VARIABLE == cSize){
        cSize = size;
    }
    if(bSave){
        int rest = GFL_NET_RingDataRestSize(pQueueMgr->pSendRing);
        if((cSize+3) >= rest){  // ���M�o�b�t�@���I�[�o�[���Ă��܂�
            NET_PRINT("���M�o�b�t�@�I�[�o�[ com = %d size = %d / %d\n", command, cSize, rest);
            return FALSE;
        }
        GFL_NET_RingPuts(pQueueMgr->pSendRing, pDataArea, cSize);
        GFL_NET_RingEndChange(pQueueMgr->pSendRing);
        pFree->bRing = TRUE;
    }
    pFree->size = cSize;
    pFree->command = command;
    pFree->pData = pDataArea;
    if(bFast == TRUE){
        pTerm = &pQueueMgr->fast;
    }
    else{
        pTerm = &pQueueMgr->stock;
    }
    if(pTerm->pLast==NULL){
        pTerm->pLast = pFree;
        pTerm->pTop = pFree;
    }
    else{
        pTerm->pLast->next = pFree;
        pFree->prev = pTerm->pLast;
        pTerm->pLast = pFree;
    }
    return TRUE;
}

//==============================================================================
/**
 * @brief   �ŐV�̃L���[���o��
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  �L���[
 */
//==============================================================================

static SEND_QUEUE* _queueGet(SEND_QUEUE_MANAGER* pQueueMgr)
{
    SEND_TERMINATOR* pTerm;

    if(pQueueMgr->pNow != NULL){
        return pQueueMgr->pNow;
    }
    if(pQueueMgr->fast.pTop != NULL){
        return pQueueMgr->fast.pTop;
    }
    if(pQueueMgr->stock.pTop != NULL){
        return pQueueMgr->stock.pTop;
    }
    return NULL;
}


//==============================================================================
/**
 * @brief   �L���[�����莟�ɐi�߂�
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  none
 */
//==============================================================================

static void _queueNext(SEND_QUEUE_MANAGER* pQueueMgr)
{
    SEND_TERMINATOR* pTerm;

    if(pQueueMgr->pNow != NULL){
        pQueueMgr->pNow = NULL;
    }
    else{
        if(!_deleteQueue( &pQueueMgr->fast )){
            BOOL ret = _deleteQueue( &pQueueMgr->stock );
            GF_ASSERT(ret);
        }
    }
}

//==============================================================================
/**
 * @brief   �w��o�C�g���o�b�t�@�ɓ����
 * @param   pQueueMgr  �L���[�}�l�[�W���[�̃|�C���^
 * @param   pSendBuff  ���M�o�b�t�@�\���̂̃|�C���^
 * @param   bNextPlus   �f�[�^�����Ƀw�b�_�[�������܂߂邩�ǂ���
 * @retval  TRUE  �����f�[�^���Ȃ��ꍇ
 * @retval  FALSE �f�[�^���A�����Ă���ꍇ
 */
//==============================================================================

BOOL GFL_NET_QueueGetData(SEND_QUEUE_MANAGER* pQueueMgr, SEND_BUFF_DATA *pSendBuff, BOOL bNextPlus)
{
    int ret;
    int i;
    int bNextPlusFirst = TRUE;

    while(pSendBuff->size > 0){
        SEND_QUEUE* pQueue = _queueGet(pQueueMgr);
        if(NULL == pQueue){
            break;
        }
        _queueNext(pQueueMgr);
        // �w�b�_�[���M�ł��Ȃ�
        if(!_dataCopyQueue(pQueue, pSendBuff, pQueueMgr->pSendRing, bNextPlusFirst)){
            pQueueMgr->pNow = pQueue;  // ���̍ŏ��ő��M�ł���悤�ɃZ�b�g
            break;
        }
        if( -1 == pSendBuff->size ){  // �f�[�^���l�܂��Ă���ꍇ
            pQueueMgr->pNow = pQueue;  // ���̍ŏ��ő��M�ł���悤�ɃZ�b�g
            return FALSE;
        }
        else{
            MI_CpuFill8(pQueue, 0, sizeof(SEND_QUEUE));  // �L���[������
        }
        bNextPlusFirst = bNextPlus;
    }
    for(i = 0 ; i < pSendBuff->size; i++){
        *pSendBuff->pData = GFL_NET_CMD_NONE;
        pSendBuff->pData++;
    }
    return TRUE;
}



//==============================================================================
/**
 * @brief   �L���[MANAGER�̏�����
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @param   queueMax  �L���[��
 * @param   pSendRing ���f�[�^��ۑ�����ꍇ�̃����O�o�b�t�@���[�N
 * @param   heapid    �������[�m��ID
 * @retval  none
 */
//==============================================================================

void GFL_NET_QueueManagerInitialize(SEND_QUEUE_MANAGER* pQueueMgr, int queueMax, RingBuffWork* pSendRing, int heapid)
{
    MI_CpuFill8(pQueueMgr, 0 ,sizeof(SEND_QUEUE_MANAGER));
    pQueueMgr->heapTop = GFL_HEAP_AllocMemory(heapid,
                                         sizeof(SEND_QUEUE)*queueMax);
    MI_CpuFill8(pQueueMgr->heapTop, 0 ,sizeof(SEND_QUEUE)*queueMax);
    pQueueMgr->max = queueMax;
    pQueueMgr->pSendRing = pSendRing;
}


//==============================================================================
/**
 * @brief   �L���[MANAGER�̃��Z�b�g
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  none
 */
//==============================================================================

void GFL_NET_QueueManagerReset(SEND_QUEUE_MANAGER* pQueueMgr)
{
    MI_CpuFill8(pQueueMgr->heapTop, 0 ,sizeof(SEND_QUEUE) * pQueueMgr->max);
    pQueueMgr->fast.pTop = NULL;     // �������鑗�M�L���[
    pQueueMgr->fast.pLast = NULL;     // �������鑗�M�L���[
    pQueueMgr->stock.pTop = NULL;    // ��ő���΂����L���[
    pQueueMgr->stock.pLast = NULL;    // ��ő���΂����L���[
    pQueueMgr->pNow = NULL;   // �������Ă���Œ��̃L���[
}

//==============================================================================
/**
 * @brief   �L���[MANAGER�̏I��
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  none
 */
//==============================================================================

void GFL_NET_QueueManagerFinalize(SEND_QUEUE_MANAGER* pQueueMgr)
{
    GFL_HEAP_FreeMemory( pQueueMgr->heapTop );
}

//==============================================================================
/**
 * @brief   �L���[�����݂��邩�ǂ���
 * @param   pQueueMgr  �L���[�}�l�[�W���[�̃|�C���^
 * @param   command    ���ׂ�R�}���h
 * @retval  TRUE ����
 * @retval  FALSE �Ȃ�
 */
//==============================================================================

BOOL GFL_NET_QueueIsCommand(SEND_QUEUE_MANAGER* pQueueMgr, int command)
{
    int i;
    SEND_QUEUE* pNext = pQueueMgr->heapTop;

    for(i = 0; i < pQueueMgr->max; i++){
        if(pNext->command == command){
            return TRUE;
        }
        pNext++;
    }
    return FALSE;
}


//==============================================================================
/**
 * @brief   �L���[MANAGER�̃e�X�g
 * @param   none
 * @retval  none
 */
//==============================================================================
#ifdef PM_DEBUG

void GFL_NET_QueueDebugTest(void)
{
    SEND_QUEUE_MANAGER mgr;
    u8 data1[30];
    u8 data2[22];
    SEND_BUFF_DATA buffData;
    u8 dummy[100];
    u8 ringbuff[50];
    int i;
    RingBuffWork sendRing;

    MI_CpuFill8(data1,2,30);
    MI_CpuFill8(data2,4,22);
    MI_CpuFill8(ringbuff,3,50);
    GFL_NET_RingInitialize(&sendRing, ringbuff, 50);

    // �R�{�L���[�쐬
    GFL_NET_QueueManagerInitialize(&mgr,3,&sendRing);

    // ----- �e�X�g�P
    GFL_NET_QueuePut(&mgr, GFL_NET_CMD_DEBUG_VARIABLE, data1, 30, TRUE, TRUE);

    for(i = 0; i < 50; i++){
        if(i < 30){
            GF_ASSERT(ringbuff[i] == 2);
        }
        else{
            GF_ASSERT(ringbuff[i] == 3);
        }
    }

    
    GF_ASSERT(mgr.fast.pTop != NULL);
    GF_ASSERT(mgr.fast.pLast != NULL);

    buffData.size = 25;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(!GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size==-1);
    for(i = 0; i < 100; i++){
        if(i == 0){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_DEBUG_VARIABLE);
        }
        else if(i == 1){
            GF_ASSERT(dummy[i] == 0 && "i=1");
        }
        else if(i == 2){
            GF_ASSERT(dummy[i] == 30 && "i=2");
        }
        else if(i < 25){
            GF_ASSERT(dummy[i] == 2 && "i<25");
        }
        else{
            GF_ASSERT(dummy[i] == 0xff && "i>=25");
        }
    }
    buffData.size = 25;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size == (25-8));

    GF_ASSERT(mgr.pNow == NULL);
    GF_ASSERT(mgr.fast.pTop == NULL);
    GF_ASSERT(mgr.fast.pLast == NULL);

    for(i = 0; i < 100; i++){
        if(i < 8){
            GF_ASSERT(dummy[i] == 2 && "i < 5");
        }
        else if(i < 25){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_NONE && "GFL_NET_CMD_NONE");
        }
        else{
            GF_ASSERT(dummy[i] == 0xff && "i >= 5");
        }
    }
    buffData.size = 25;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size == (25));

    GF_ASSERT(mgr.fast.pTop == NULL);
    GF_ASSERT(mgr.fast.pLast == NULL);

    for(i = 0; i < 100; i++){
        if(i < 25){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_NONE && "i<25 GFL_NET_CMD_NONE");
        }
        else{
            GF_ASSERT(dummy[i] == 0xff && "i  all");
        }
    }

    // �e�X�g�Q
    GF_ASSERT(GFL_NET_QueuePut(&mgr, GFL_NET_CMD_DEBUG_VARIABLE, data1, 10, TRUE, FALSE));

    GF_ASSERT(mgr.fast.pTop != NULL);
    GF_ASSERT(mgr.fast.pLast != NULL);
    GF_ASSERT(mgr.fast.pLast == mgr.fast.pTop);

    GF_ASSERT(GFL_NET_QueuePut(&mgr, GFL_NET_CMD_DEBUG_VARIABLE, data2, 10, TRUE, FALSE));

    GF_ASSERT(mgr.fast.pTop != NULL);
    GF_ASSERT(mgr.fast.pTop->next != NULL);
    GF_ASSERT(mgr.fast.pTop->next->next == NULL);


    GF_ASSERT(GFL_NET_QueuePut(&mgr, GFL_NET_CMD_DEBUG_VARIABLE, data1, 10, TRUE, FALSE));

    GF_ASSERT(mgr.fast.pTop != NULL);
    GF_ASSERT(mgr.fast.pTop->next != NULL);
    GF_ASSERT(mgr.fast.pTop->next->next != NULL);

    // ����̓L���[�ɓ���Ȃ�
//    GF_ASSERT(!GFL_NET_QueuePut(&mgr, GFL_NET_CMD_DEBUG_VARIABLE, data1, 10, TRUE, TRUE));


    buffData.size = 12;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(!GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size == -1);

    GF_ASSERT(mgr.fast.pTop != NULL);
    GF_ASSERT(mgr.fast.pLast != NULL);

    for(i = 0; i < 100; i++){
        if(i == 0){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_DEBUG_VARIABLE);
        }
        else if(i == 1){
            GF_ASSERT(dummy[i] == 0);
        }
        else if(i == 2){
            GF_ASSERT(dummy[i] == 10);
        }
        else if(i < 12){
            GF_ASSERT(dummy[i] == 2);
        }
        else{
            GF_ASSERT(dummy[i] == 0xff);
        }
    }
    buffData.size = 12;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(!GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size == -1);

    GF_ASSERT(mgr.pNow != NULL);
    GF_ASSERT(mgr.fast.pTop != NULL);
    GF_ASSERT(mgr.fast.pLast != NULL);

    for(i = 0; i < 100; i++){
        if(i == 0){
            GF_ASSERT(dummy[i] == 2);
        }
        else if(i == 1){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_DEBUG_VARIABLE);
        }
        else if(i == 2){
            GF_ASSERT(dummy[i] == 0);
        }
        else if(i == 3){
            GF_ASSERT(dummy[i] == 10);
        }
        else if(i < 12){
            GF_ASSERT(dummy[i] == 4);
        }
        else{
            GF_ASSERT(dummy[i] == 0xff);
        }
    }

    // �킴�Ə������ǂݍ��ނƁA���������������Ă���
    buffData.size = 4;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size == 2);

    GF_ASSERT(mgr.pNow != NULL);
    GF_ASSERT(mgr.fast.pTop == NULL);
    GF_ASSERT(mgr.fast.pLast == NULL);

    for(i = 0; i < 100; i++){
        if(i < 2){
            GF_ASSERT(dummy[i] == 4);
        }
        else if(i < 4){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_NONE);
        }
        else{
            GF_ASSERT(dummy[i] == 0xff);
        }
    }

    /// �Ō�͂R�Ԗڂ̃R�}���h������������Ă���
    buffData.size = 4;
    buffData.pData = dummy;
    MI_CpuFill8(dummy, 0xff, 100);
    GF_ASSERT(!GFL_NET_QueueGetData(&mgr, &buffData, TRUE));
    GF_ASSERT(buffData.size == -1);

    GF_ASSERT(mgr.pNow != NULL);
    GF_ASSERT(mgr.fast.pTop == NULL);
    GF_ASSERT(mgr.fast.pLast == NULL);

    for(i = 0; i < 100; i++){
        if(i == 0){
            GF_ASSERT(dummy[i] == GFL_NET_CMD_DEBUG_VARIABLE);
        }
        else if(i == 1){
            GF_ASSERT(dummy[i] == 0);
        }
        else if(i == 2){
            GF_ASSERT(dummy[i] == 10);
        }
        else if(i < 4){
            GF_ASSERT(dummy[i] == 2);
        }
        else{
            GF_ASSERT(dummy[i] == 0xff);
        }
    }
    GFL_NET_QueueManagerFinalize(&mgr);

//   GF_ASSERT(0 && "ok");  //�e�X�g����
}

#endif

