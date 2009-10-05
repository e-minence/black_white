/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - libraries
  File:     wm_ds.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void WmDataSharingSetDataCallback(void *callback);
static void WmDataSharingReceiveCallback_Parent(void *callback);
static void WmDataSharingReceiveCallback_Child(void *callback);
static void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, u16 aid, u16 *data);
static void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, BOOL delayed);
static u16 *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, u32 aidBitmap, u16 *receiveBuf,
                                   u32 aid);

//// data-sharing �Ŏg�p���̃|�[�g�ԍ��ꗗ
//static u16 WmDataSharingPortBitmap = 0;

static inline u16 WmDataSharingGetNextIndex(u32 index)
{
    return (u16)((index + 1) % WM_DS_DATASET_NUM);
}

static inline u16 WmDataSharingGetPrevIndex(u32 index)
{
    return (u16)((index + WM_DS_DATASET_NUM - 1) % WM_DS_DATASET_NUM);
}


/*---------------------------------------------------------------------------*
  Name:         WM_StartDataSharing

  Description:  �f�[�^�V�F�A�����O�@�\��L���ɂ���B

  Arguments:    dsInfo      -   WMDataSharingInfo �\����
                port        -   �g�p���� port �ԍ�
                aidBitmap   -   �f�[�^�V�F�A���s������� AID �̃r�b�g�}�b�v
                dataLength  -   ���L�������̃f�[�^�� (�����o�C�g���w�肷�邱��)
                doubleMode  -   ���t���[�� WM_StepDataSharing ����ꍇ�� TRUE ���w��

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartDataSharing(WMDataSharingInfo *dsInfo, u16 port, u16 aidBitmap, u16 dataLength,
                    BOOL doubleMode)
{
    WMErrCode result;
    int     aid;
    u16     connectedAIDs = 0x0001;

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // �p�����[�^�`�F�b�N
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (port >= WM_NUM_OF_PORT)
    {
        WM_WARNING("Parameter \"port\" must be less than %d.\n", WM_NUM_OF_PORT);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)dsInfo & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"dsInfo\" is not 32-byte aligned.\n");
    }
    if (aidBitmap == 0)
    {
        WM_WARNING("Parameter \"aidBitmap\" must not be 0.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // �K�v�ȏ����擾
    aid = WM_GetAID();
    if (aid == 0)
    {
        connectedAIDs = WM_GetConnectedAIDs();
    }

    // �f�[�^�V�F�A�����O����ϐ���������
    MI_CpuClearFast(dsInfo, sizeof(WMDataSharingInfo));
    dsInfo->writeIndex = 0;
    dsInfo->sendIndex = 0;
    dsInfo->readIndex = 0;
    dsInfo->dataLength = dataLength;
    dsInfo->port = port;
    dsInfo->aidBitmap = 0;
    dsInfo->doubleMode = (u16)((doubleMode) ? TRUE : FALSE);

    aidBitmap |= (1 << aid);           // �����̕��𑫂�
    dsInfo->aidBitmap = aidBitmap;

    {
        u16     count = MATH_CountPopulation(aidBitmap);
        dsInfo->stationNumber = count;
        dsInfo->dataSetLength = (u16)(dataLength * count);

        if (dsInfo->dataSetLength > WM_DS_DATA_SIZE)
        {
            // ���v�̃f�[�^�ʂ����e�l�𒴂��Ă���
            dsInfo->aidBitmap = 0;
            WM_WARNING("Total size of sharing data must be less than or equal to %d bytes.\n",
                       WM_DS_DATA_SIZE);
            return WM_ERRCODE_INVALID_PARAM;
        }
        dsInfo->dataSetLength += 4;    // aidBitmap, receivedBitmap
    }

    dsInfo->state = WM_DS_STATE_START;

    if (aid == 0)
    {
        // �e�@�̊J�n����
        int     i;

        for (i = 0; i < WM_DS_DATASET_NUM; i++)
        {
            dsInfo->ds[i].aidBitmap = (u16)(dsInfo->aidBitmap & (connectedAIDs | 0x0001));
        }

        (void)WM_SetPortCallback(port, WmDataSharingReceiveCallback_Parent, (void *)dsInfo);

        // �N���p�̋�f�[�^�𑗐M
        for (i = 0; i < ((dsInfo->doubleMode == TRUE) ? 2 : 1); i++)
        {
            int     res;

            dsInfo->writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, (u16 *)&dsInfo->ds[i],
                                     dsInfo->dataSetLength,
                                     (u16)(dsInfo->aidBitmap & connectedAIDs), dsInfo->port,
                                     WM_PRIORITY_HIGH);
            if (res == WM_ERRCODE_NO_CHILD)
            {
                dsInfo->seqNum[i] = 0xffff;
                dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            }
            else
            {
                if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
                {
                    WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                    dsInfo->state = WM_DS_STATE_ERROR;
                    return WM_ERRCODE_FAILED;
                }
            }
        }
    }
    else
    {
        // �q�@�̊J�n����
        dsInfo->sendIndex = (u16)(WM_DS_DATASET_NUM - 1);
        (void)WM_SetPortCallback(port, WmDataSharingReceiveCallback_Child, (void *)dsInfo);
    }

    return WM_ERRCODE_SUCCESS;         // �����I��
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndDataSharing

  Description:  �f�[�^�V�F�A�����O�@�\�𖳌��ɂ���B

  Arguments:    dsInfo      -   WMDataSharingInfo �\����

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDataSharing(WMDataSharingInfo *dsInfo)
{
    // �p�����[�^�`�F�b�N
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // �f�[�^�V�F�A�����O��ԃ`�F�b�N
    if (dsInfo->aidBitmap == 0)
    {
        WM_WARNING("It is not data-sharing mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    (void)WM_SetPortCallback(dsInfo->port, NULL, NULL);
    dsInfo->aidBitmap = 0;
    dsInfo->state = WM_DS_STATE_READY;

    return WM_ERRCODE_SUCCESS;         // �����I��
}

/*---------------------------------------------------------------------------*
  Name:         WM_StepDataSharing

  Description:  �f�[�^�V�F�A�����O�̓�������i�߂�

  Arguments:    dsInfo      -   WMDataSharingInfo �\����
                sendData    -   ���L���������M�f�[�^
                receiveData -   ��M�������L�f�[�^

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B
                    *_SUCCESS    : ���L�f�[�^����M����
                    *_NO_DATASET : �܂����̋��L�f�[�^�����Ă��Ȃ�
 *---------------------------------------------------------------------------*/
WMErrCode WM_StepDataSharing(WMDataSharingInfo *dsInfo, const u16 *sendData, WMDataSet *receiveData)
{
    WMErrCode result;
    u16     aid;
    u16     connectedAIDs;
    u16     state;

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // �p�����[�^�`�F�b�N
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (sendData == NULL)
    {
        WM_WARNING("Parameter \"sendData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (receiveData == NULL)
    {
        WM_WARNING("Parameter \"receiveData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // �K�v�ȏ����擾
    aid = WM_GetAID();
    if (aid == 0)
    {
        connectedAIDs = WM_GetConnectedAIDs();
    }

    // �f�[�^�V�F�A�����O��ԃ`�F�b�N
    state = dsInfo->state;
    if (state == WM_DS_STATE_ERROR)
    {
        WM_WARNING("An error occurred during data-sharing.\n");
        return WM_ERRCODE_FAILED;
    }
    if (state != WM_DS_STATE_START && state != WM_DS_STATE_RETRY_SEND)
    {
        WM_WARNING("It is not data-sharing mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    result = WM_ERRCODE_NO_DATASET;

    if (aid == 0)
    {
        // �e�@�Ȃ�
        BOOL    sendFlag = FALSE;
        BOOL    delayed = FALSE;

        if (state == WM_DS_STATE_RETRY_SEND)
        {
            // �O�� SEND_QUEUE_FULL �ɂ��A�đ��M
            int     res;
            int     oldWI;
            dsInfo->state = WM_DS_STATE_START;

            WM_DLOG_DATASHARING("send queue was full. do retry.");

            oldWI = WmDataSharingGetPrevIndex(dsInfo->writeIndex);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo,
                                     (u16 *)&dsInfo->ds[oldWI], dsInfo->dataSetLength,
                                     (u16)(dsInfo->aidBitmap & connectedAIDs), dsInfo->port,
                                     WM_PRIORITY_HIGH);
            if (res == WM_ERRCODE_NO_CHILD)
            {
                WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d\n",
                                     dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex);
                dsInfo->seqNum[oldWI] = 0xffff;
                dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            }
            else
            {
                if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
                {
                    WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                    dsInfo->state = WM_DS_STATE_ERROR;
                    return WM_ERRCODE_FAILED;
                }
            }
        }

        // �e�@�� DataSet �𑗂�I�������g�p�\�ɂȂ�̂� sendIndex ���ǂݏo������E�����߂�
        if (dsInfo->readIndex != dsInfo->sendIndex)
        {
            // �ǂݏo������
            WM_DLOGF_DATASHARING("read ds   : write: %d, read: %d, send: %d, seq#: %d",
                                 dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex,
                                 dsInfo->seqNum[dsInfo->readIndex]);

            dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;  // ���M�f�[�^���� aidBitmap �̍ŉ��ʃr�b�g�͒x���t���O
            MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
            dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
            dsInfo->readIndex = WmDataSharingGetNextIndex(dsInfo->readIndex);

            sendFlag = TRUE;
            result = WM_ERRCODE_SUCCESS;
            if (dsInfo->doubleMode == FALSE && connectedAIDs != 0
                && dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0x0001)
            {
                // �e�@���f�[�^���Z�b�g����΂����ɑ��M�\ �� �e�@�݂̂��t���[������
                delayed = TRUE;
            }
            else
            {
                delayed = FALSE;
            }
        }

        // ���M�o�b�t�@�ɑS�@�������Ă����� DataSet �𑗐M
        WmDataSharingSendDataSet(dsInfo, FALSE);

        if (sendFlag)
        {
            // �e�@�͖����ő��M�������Ɏ����̃o�b�t�@�ɓ����
            WM_DLOGF_DATASHARING("send data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            WmDataSharingReceiveData(dsInfo, 0, (u16 *)sendData);       // ��ł̘A���I�ȏ����������ʓ|�Ȃ̂� const ���O��

            if (dsInfo->doubleMode == FALSE)
            {
                // ���t���[���� Step �łȂ��ꍇ�̂݁A
                // ���̃^�C�~���O�ō� GF �ł̑��M�p�� DataSet ��p�ӂ��Ă����K�v������B
                // ���M�o�b�t�@�ɑS�@�������Ă����� DataSet �𑗐M
                WmDataSharingSendDataSet(dsInfo, delayed);
            }
        }
    }
    else
    {
        // �q�@�Ȃ�
        BOOL    sendFlag = FALSE;

        if (state == WM_DS_STATE_RETRY_SEND)
        {
            // �O�� SEND_QUEUE_FULL �ɂ��A�đ��M
            sendFlag = TRUE;
            dsInfo->state = WM_DS_STATE_START;
            WM_DLOG_DATASHARING("send queue was full. do retry.");
        }
        else
        {
            // �q�@�� DataSet ����M�����炷���Ɏg�p�\�Ȃ̂� writeIndex ���ǂݏo������E�����߂�
            if (dsInfo->readIndex != dsInfo->writeIndex)
            {
                // �x���t���O���Q�Ă�����A1�t���[�������I�ɒx�点��
                // ���M�f�[�^���� aidBitmap �̍ŉ��ʃr�b�g�͒x���t���O
                if (!(dsInfo->ds[dsInfo->readIndex].aidBitmap & 0x0001))
                {
                    dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;
                }
                else
                {
                    // �ǂݏo������
                    WM_DLOGF_DATASHARING("read ds   : write: %d, read: %d, send: %d, seq#: %d",
                                         dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex,
                                         dsInfo->seqNum[dsInfo->readIndex]);
                    MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
                    dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
                    dsInfo->readIndex = WmDataSharingGetNextIndex(dsInfo->readIndex);

                    sendFlag = TRUE;
                    result = WM_ERRCODE_SUCCESS;
                }
            }
        }

        if (sendFlag)
        {
            // �q�@�͂��̂܂ܑ��M
            int     res;
            // dsInfo->ds �̈ꕔ�𑗐M�o�b�t�@�Ƃ��Ďg�킹�Ă��炤
            u16    *buf = (u16 *)(((u8 *)&dsInfo->ds[dsInfo->sendIndex]) + 32); // 32-byte align ����Ă���K�v������

            WM_DLOGF_DATASHARING("send data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            MI_CpuCopy16(sendData, buf, dsInfo->dataLength);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, buf, dsInfo->dataLength,
                                     dsInfo->aidBitmap, dsInfo->port, WM_PRIORITY_HIGH);
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            if (res != WM_ERRCODE_OPERATING && res != WM_ERRCODE_SUCCESS)
            {
                WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                dsInfo->state = WM_DS_STATE_ERROR;
                result = WM_ERRCODE_FAILED;
            }
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingSetDataCallback

  Description:  ���M�����R�[���o�b�N�B

  Arguments:    callback - �R�[���o�b�N�\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingSetDataCallback(void *callback)
{
    WMArm9Buf *p = WMi_GetSystemWork();
    WMPortSendCallback *cb_Port = (WMPortSendCallback *)callback;
    WMDataSharingInfo *dsInfo;
    u16     aid;

    // WM_EndDataSharing() ���Ă΂�ADSInfo �������ȏ�ԂŌĂяo����Ă��Ȃ������m�F
    dsInfo = (WMDataSharingInfo *)(p->portCallbackArgument[cb_Port->port]);
    if ((p->portCallbackTable[cb_Port->port] != WmDataSharingReceiveCallback_Parent
         && p->portCallbackTable[cb_Port->port] != WmDataSharingReceiveCallback_Child)
        || dsInfo == NULL || dsInfo != (WMDataSharingInfo *)(cb_Port->arg))
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    aid = WM_GetAID();

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        // ���M����
        if (aid == 0)
        {
            // �e�@������
            WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            dsInfo->seqNum[dsInfo->sendIndex] = (u16)(cb_Port->seqNo >> 1);
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
        }
        else
        {
            // �q�@������
            WM_DLOGF_DATASHARING("sent data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);
        }
    }
    else
    {
        if (cb_Port->errcode == WM_ERRCODE_SEND_QUEUE_FULL)
        {
            // ���M�L���[�������ς��������̂ōđ�����
            if (aid != 0)
            {
                // �q�@�̏ꍇ�AsendIndex ����߂�
                dsInfo->sendIndex = WmDataSharingGetPrevIndex(dsInfo->sendIndex);
            }
            dsInfo->state = WM_DS_STATE_RETRY_SEND;
            WM_DLOG_DATASHARING("send queue is full. will retry.");
        }
        else
        {
            WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n",
                       cb_Port->errcode);
            dsInfo->state = WM_DS_STATE_ERROR;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveCallback_Parent

  Description:  �e�@�� port ��M�R�[���o�b�N�B

  Arguments:    callback - �R�[���o�b�N�\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveCallback_Parent(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)cb_Port->arg;

    if (dsInfo == NULL)
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb_Port->state)
        {
        case WM_STATECODE_PORT_RECV:
            // ��M�����q�@�̃f�[�^���o�b�t�@�Ɋi�[����
            WmDataSharingReceiveData(dsInfo, cb_Port->aid, cb_Port->data);
            WmDataSharingSendDataSet(dsInfo, FALSE);
            break;

        case WM_STATECODE_CONNECTED:
            WmDataSharingSendDataSet(dsInfo, FALSE);    // �K�v�͂Ȃ��͂�
            break;

        case WM_STATECODE_DISCONNECTED:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            {
                u32     aidBit;
                u32     writeIndex;
                OSIntrMode enabled;
                aidBit = 1U << cb_Port->aid;
                enabled = OS_DisableInterrupts();
                writeIndex = dsInfo->writeIndex;
                dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
                if (dsInfo->doubleMode == TRUE)
                {
                    dsInfo->ds[WmDataSharingGetNextIndex(writeIndex)].aidBitmap &= ~aidBit;
                }
                (void)OS_RestoreInterrupts(enabled);
                WmDataSharingSendDataSet(dsInfo, FALSE);
                if (dsInfo->doubleMode == TRUE)
                {
                    WmDataSharingSendDataSet(dsInfo, FALSE);
                }
            }
            break;

        case WM_STATECODE_PORT_INIT:
            break;
        }
    }
    else
    {
        WM_WARNING("An unknown receiving error occured during data-sharing. errcode=%x\n",
                   cb_Port->errcode);
        dsInfo->state = WM_DS_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveCallback_Child

  Description:  �q�@�� port ��M�R�[���o�b�N�B

  Arguments:    callback - �R�[���o�b�N�\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveCallback_Child(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)cb_Port->arg;

    if (dsInfo == NULL)
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb_Port->state)
        {
        case WM_STATECODE_PORT_RECV:
            {
                u32     length;
                u32     aid;
                u32     aidBitmap;
                WMDataSet *dataSet;

                WM_DLOGF_DATASHARING("recv ds   : write: %d, read: %d, send: %d",
                                     dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex);

                // DataSet ��ۑ�����
                dataSet = (WMDataSet *)(cb_Port->data);
                aidBitmap = dataSet->aidBitmap;
                length = cb_Port->length;
                aid = WM_GetAID();

                if (length != dsInfo->dataSetLength)
                {
                    WM_WARNING("received DataSharing data size(%d) != dsInfo->dataSetLength(%d)\n",
                               length, dsInfo->dataSetLength);
                    if (length > sizeof(WMDataSet))
                    {
                        // �o�b�t�@�𒴂���قǒ�������f�[�^����M������A�␳����B
                        WM_WARNING("received DataSharing data exceeds sizeof(WMDataSet)\n");
                        length = sizeof(WMDataSet);
                    }
                }
#ifdef SDK_DEBUG
                if (aidBitmap & ~(dsInfo->aidBitmap))
                {
                    // �q�@���őz�肵�Ă��� aidBitmap ��葽��
                    WM_WARNING("received aidBitmap(%x) has too many members.\n", aidBitmap);
                }
#endif
                if (length >= 4 && (aidBitmap & (1 << aid)))
                {
                    MI_CpuCopy16(dataSet, &dsInfo->ds[dsInfo->writeIndex], length);
                    dsInfo->seqNum[dsInfo->writeIndex] = (u16)(cb_Port->seqNo >> 1);
                    dsInfo->writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
                }
            }
            break;
        case WM_STATECODE_PORT_INIT:
        case WM_STATECODE_CONNECTED:
        case WM_STATECODE_DISCONNECTED:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            break;
        }
    }
    else
    {
        WM_WARNING("An unknown receiving error occured during data-sharing. errcode=%x\n",
                   cb_Port->errcode);
        dsInfo->state = WM_DS_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveData

  Description:  �e�@���e�@�f�[�^����M�����̂Ŋi�[����B

  Arguments:    dsInfo - WMDataSharingInfo �\���́B
                aid    - �f�[�^����M�����[����AID�B
                data   - ��M�����f�[�^�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, u16 aid, u16 *data)
{
    u16     aidBit = (u16)(1 << aid);

    WM_DLOGF_DATASHARING("recv data%d: write: %d, read: %d, send: %d", aid, dsInfo->writeIndex,
                         dsInfo->readIndex, dsInfo->sendIndex);

    // �����Ώۂ����m�F
    if (dsInfo->aidBitmap & aidBit)
    {
        u16    *buf;
        u16     writeIndex;
        OSIntrMode enabled;

        if (!(dsInfo->ds[dsInfo->writeIndex].aidBitmap & aidBit))
        {
            if (dsInfo->doubleMode == TRUE)
            {
                WM_DLOGF_DATASHARING("[DS] received two DS packets from aid %d", aid);

                writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
                if (!(dsInfo->ds[writeIndex].aidBitmap & aidBit))
                {
                    // 2�܂Ńo�b�t�@�ɒ��߂邪�A����ȏ�͎̂Ă�
                    OS_TWarning("received too many DataSharing packets from aid %d. discarded.\n",
                               aid);
                    return;
                }
            }
            else
            {
                // doubleMode �ł͂Ȃ���΁A�o�b�t�@�ɒ��߂�̂�1�܂�
                OS_TWarning("received too many DataSharing packets from aid %d. discarded.\n", aid);
                return;
            }
        }
        else
        {
            writeIndex = dsInfo->writeIndex;
        }

        buf = WmGetSharedDataAddress(dsInfo, dsInfo->aidBitmap, dsInfo->ds[writeIndex].data, aid);
        if (data != NULL)
        {
            MI_CpuCopy16(data, buf, dsInfo->dataLength);
        }
        else
        {
            MI_CpuClear16(buf, dsInfo->dataLength);
        }

        enabled = OS_DisableInterrupts();
        // ����M�t���O��Q������
        dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
        // ��M�ς݃t���O�𗧂�����
        dsInfo->ds[writeIndex].receivedBitmap |= aidBit;
        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingSendDataSet

  Description:  �e�@���e�@�f�[�^�̎�M���m�F�����̂��A�f�[�^�Z�b�g�𑗐M����B

  Arguments:    dsInfo - WMDataSharingInfo �\���́B
                delayed - �t���[���x���ԂɂȂ��Ă����� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, BOOL delayed)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    // �������S�@����M���I�������
    // �i��M�o�b�t�@���� ds[].aidBitmap �͖���M�t���O�̖�ڂ��ʂ����j
    if (dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0)
    {
        u16     newWI, oldWI, resetWI;
        WMErrCode res;
        u16     connectedAIDs;

        WM_DLOGF_DATASHARING("send ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                             dsInfo->readIndex, dsInfo->sendIndex);

        connectedAIDs = WM_GetConnectedAIDs();
        oldWI = dsInfo->writeIndex;
        newWI = WmDataSharingGetNextIndex(oldWI);
        if (dsInfo->doubleMode == TRUE)
        {
            resetWI = WmDataSharingGetNextIndex(newWI);
        }
        else
        {
            resetWI = newWI;
        }
        SDK_ASSERT(newWI != dsInfo->readIndex && resetWI != dsInfo->readIndex);
//        SDK_ASSERT( dsInfo->sendIndex == dsInfo->writeIndex);
        MI_CpuClear16(&dsInfo->ds[resetWI], sizeof(WMDataSet));
        dsInfo->ds[resetWI].aidBitmap = (u16)(dsInfo->aidBitmap & (connectedAIDs | 0x0001));
        dsInfo->writeIndex = newWI;
        dsInfo->ds[oldWI].aidBitmap = dsInfo->aidBitmap;        // ���M����ɂ������� aidBitmap �ɖ{���̒l������
        if (delayed == TRUE)
        {
            // aidBitmap �̍ŉ��ʃr�b�g�͒x���t���O
            dsInfo->ds[oldWI].aidBitmap &= ~0x0001;
        }
        (void)OS_RestoreInterrupts(enabled);
        res =
            WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, (u16 *)&dsInfo->ds[oldWI],
                                 dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & connectedAIDs),
                                 dsInfo->port, WM_PRIORITY_HIGH);
        if (res == WM_ERRCODE_NO_CHILD)
        {
            WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            dsInfo->seqNum[oldWI] = 0xffff;
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
        }
        else
        {
            if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
            {
                WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                dsInfo->state = WM_DS_STATE_ERROR;
            }
        }
    }
    else
    {
        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetSharedDataAddress

  Description:  �f�[�^�V�F�A�����O�̎�M�f�[�^���̓���� AID �̃A�h���X�𓾂�

  Arguments:    dsInfo      -   WMDataSharingInfo �\����
                receiveData -   ��M�������L�f�[�^
                aid         -   AID�B

  Returns:      u16*        -   ��M�f�[�^�̃A�h���X�B���݂��Ȃ���� NULL ��Ԃ��B
 *---------------------------------------------------------------------------*/
u16    *WM_GetSharedDataAddress(WMDataSharingInfo *dsInfo, WMDataSet *receiveData, u16 aid)
{
    u32     aidBitmap;
    u32     receivedBitmap;
    u32     aidBit = (1U << aid);

    // �p�����[�^�`�F�b�N
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return NULL;
    }

    if (receiveData == NULL)
    {
        WM_WARNING("Parameter \"receiveData\" must not be NULL.\n");
        return NULL;
    }

    aidBitmap = receiveData->aidBitmap;
    receivedBitmap = receiveData->receivedBitmap;
    
    if (!(aidBitmap & aidBit))
    {
//        WM_WARNING("Parameter \"aid\" must be a member of \"receiveData->aidBitmap\".\n");
        return NULL;
    }

    if (!(receivedBitmap & aidBit))
    {
        // �f�[�^����M�ł��Ă��Ȃ�
        return NULL;
    }

    return WmGetSharedDataAddress(dsInfo, aidBitmap, receiveData->data, aid);
}

/*---------------------------------------------------------------------------*
  Name:         WmGetSharedDataAddress

  Description:  ��M�o�b�t�@���̓���� AID �̃A�h���X�𓾂�

  Arguments:    dsInfo      -   WMDataSharingInfo �\����
                aidBitmap   -   �f�[�^�Ɋ܂܂�Ă���ʐM����
                receiveBuf  -   ��M�o�b�t�@
                aid         -   AID�B
                                aidBitmap & (1<<aid) �͌Ăяo���O�Ɋm�F���邱�ƁB

  Returns:      u16*        -   ��M�f�[�^�̃A�h���X�B
 *---------------------------------------------------------------------------*/
u16    *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, u32 aidBitmap, u16 *receiveBuf, u32 aid)
{
    u32     mask;
    u32     count;
    u32     offset;

    // aidBitmap �̉����� aid �r�b�g���� 1 �̌����J�E���g����
    mask = (0x0001U << aid) - 1U;
    aidBitmap &= mask;
    count = MATH_CountPopulation(aidBitmap);
    offset = dsInfo->dataLength * count;

    return (u16 *)(((u8 *)receiveBuf) + offset);
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
