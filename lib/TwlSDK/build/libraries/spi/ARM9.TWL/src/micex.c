/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - mic
  File:     micex.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include    <nitro/os.h>
#include    <nitro/spi.h>
#include    "micex.h"

#include    <twl/spi/common/mic_common.h>

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void     MicexSyncCallback(MICResult result, void* arg);
static BOOL     MicexStartLimitedSampling(void* buf, u32 size, u32 rate, u8 flags);
static BOOL     MicexStopLimitedSampling(void);
static BOOL     MicexAdjustLimitedSampling(u32 rate);

/*---------------------------------------------------------------------------*
  Name:         MICEXi_StartLimitedSampling
  Description:  �T���v�����O���[�g����̃}�C�N�����T���v�����O���J�n����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
  Arguments:    param   -   �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B
  Returns:      MICResult - �����I�ȏ������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MICEXi_StartLimitedSampling(const MICAutoParam* param)
{
    MICResult       result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ���荞�݃n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
#ifdef  SDK_DEBUG
        OS_TWarning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
        return MIC_RESULT_ILLEGAL_STATUS;
    }

    /* �����擾�p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   MICEXi_StartLimitedSamplingAsync(param, MicexSyncCallback, (void*)(&msgQ));
    if (result == MIC_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         MICEXi_StartLimitedSamplingAsync
  Description:  �T���v�����O���[�g����̃}�C�N�����T���v�����O���J�n����B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
  Arguments:    param   -   �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B
                callback -  �񓯊������������ɌĂяo���R�[���o�b�N�֐����w��B
                arg     -   �R�[���o�b�N�֐��ɓn���p�����[�^���w��B
  Returns:      MICResult - �񓯊������J�n�����̌��ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MICEXi_StartLimitedSamplingAsync(const MICAutoParam* param, MICCallback callback, void* arg)
{
    OSIntrMode  e;
    MICWork*    w   =   MICi_GetSysWork();
    u8          flags;

    SDK_NULL_ASSERT(callback);
    SDK_NULL_ASSERT(param->buffer);

    /* �p�����[�^�m�F */
    {
        /* �o�b�t�@�T�C�Y */
        if (param->size <= 0)
        {
#ifdef  SDK_DEBUG
            OS_TWarning("%s: Illegal desination buffer size. (%d)\n", __FUNCTION__, param->size);
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        /* �o�b�t�@ 32 �o�C�g�A���C�� */
        if (((u32)(param->buffer) % HW_CACHE_LINE_SIZE != 0) || ((param->size % HW_CACHE_LINE_SIZE) != 0))
        {
#ifdef  SDK_DEBUG
            OS_TWarning("%s: Destination buffer (%p - %p) is not aligned on %d bytes boundary.\n",
                    __FUNCTION__, param->buffer, (void*)((u32)param->buffer + param->size), HW_CACHE_LINE_SIZE);
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        /* AD �ϊ��r�b�g�� */
        switch (param->type)
        {
        case MIC_SAMPLING_TYPE_8BIT:
            flags   =   SPI_MIC_SAMPLING_TYPE_8BIT;
            break;
        case MIC_SAMPLING_TYPE_12BIT:
            flags   =   SPI_MIC_SAMPLING_TYPE_12BIT;
            break;
        case MIC_SAMPLING_TYPE_SIGNED_8BIT:
            flags   =   SPI_MIC_SAMPLING_TYPE_S8BIT;
            break;
        case MIC_SAMPLING_TYPE_SIGNED_12BIT:
            flags   =   SPI_MIC_SAMPLING_TYPE_S12BIT;
            break;
        case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
            flags   =   (SPI_MIC_SAMPLING_TYPE_12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
            break;
        case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
            flags   =   (SPI_MIC_SAMPLING_TYPE_S12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
            break;
        default:
#ifdef  SDK_DEBUG
            OS_TWarning("%s: Illegal sampling type. (%d)\n", __FUNCTION__, param->type);
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        /* �T���v�����O���� */
        switch (param->rate)
        {
        case MIC_SAMPLING_RATE_32730:   case MIC_SAMPLING_RATE_16360:
        case MIC_SAMPLING_RATE_10910:   case MIC_SAMPLING_RATE_8180:
        case MIC_SAMPLING_RATE_47610:   case MIC_SAMPLING_RATE_23810:
        case MIC_SAMPLING_RATE_15870:   case MIC_SAMPLING_RATE_11900:
            break;
        default:
#ifdef  SDK_TWL
            OS_TWarning("%s: Illegal sampling rate. (%d)\n", __FUNCTION__, param->rate);
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        /* ���[�v�� */
        if (param->loop_enable)
        {
            flags   =   (u8)((flags & ~SPI_MIC_SAMPLING_TYPE_LOOP_MASK) | SPI_MIC_SAMPLING_TYPE_LOOP_ON);
        }
        else
        {
            flags   =   (u8)((flags & ~SPI_MIC_SAMPLING_TYPE_LOOP_MASK) | SPI_MIC_SAMPLING_TYPE_LOOP_OFF);
        }
        /* �g���p�␳�t���O */
        flags   =   (u8)((flags & ~SPI_MIC_SAMPLING_TYPE_CORRECT_MASK) | SPI_MIC_SAMPLING_TYPE_CORRECT_OFF);
    }

    /* �r�����b�N�m�F */
    e   =   OS_DisableInterrupts();
    if (w->lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(e);
        return MIC_RESULT_BUSY;
    }
    w->lock =   MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(e);

    /* �����t�������T���v�����O�J�n�R�}���h�𑗐M */
    w->callback =   callback;
    w->callbackArg  =   arg;
    w->full =   param->full_callback;
    w->fullArg  =   param->full_arg;
    if (MicexStartLimitedSampling(param->buffer, param->size, param->rate, flags))
    {
        return MIC_RESULT_SUCCESS;
    }
    w->lock =   MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MICEXi_StopLimitedSampling
  Description:  �T���v�����O���[�g����̃}�C�N�����T���v�����O���~����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
  Arguments:    None.
  Returns:      MICResult - �����I�ȏ������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MICEXi_StopLimitedSampling(void)
{
    MICResult       result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ���荞�݃n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
#ifdef  SDK_DEBUG
        OS_TWarning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
        return MIC_RESULT_ILLEGAL_STATUS;
    }

    /* �����擾�p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   MICEXi_StopLimitedSamplingAsync(MicexSyncCallback, (void*)(&msgQ));
    if (result == MIC_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         MICEXi_StopLimitedSamplingAsync
  Description:  �T���v�����O���[�g����̃}�C�N�����T���v�����O���~����B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
  Arguments:    callback -  �񓯊������������ɌĂяo���R�[���o�b�N�֐����w��B
                arg     -   �R�[���o�b�N�֐��ɓn���p�����[�^���w��B
  Returns:      MICResult - �񓯊������J�n�����̌��ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MICEXi_StopLimitedSamplingAsync(MICCallback callback, void* arg)
{
    OSIntrMode  e;
    MICWork*    w   =   MICi_GetSysWork();

    SDK_NULL_ASSERT(callback);

    /* �r�����b�N�m�F */
    e   =   OS_DisableInterrupts();
    if (w->lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(e);
        return MIC_RESULT_BUSY;
    }
    w->lock =   MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(e);

    /* �����t�������T���v�����O��~�R�}���h�𑗐M */
    w->callback =   callback;
    w->callbackArg  =   arg;
    if (MicexStopLimitedSampling())
    {
        return MIC_RESULT_SUCCESS;
    }
    w->lock =   MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MICEXi_AdjustLimitedSampling
  Description:  �T���v�����O���[�g����̃}�C�N�����T���v�����O�ɂ�����
                �T���v�����O�Ԋu�𒲐�����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
  Arguments:    rate    -   �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B
  Returns:      MICResult - �����I�ȏ������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MICEXi_AdjustLimitedSampling(u32 rate)
{
    MICResult       result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ���荞�݃n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
#ifdef  SDK_DEBUG
        OS_TWarning("%s: Could not process in exception handler.\n", __FUNCTION__);
#endif
        return MIC_RESULT_ILLEGAL_STATUS;
    }

    /* �����擾�p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   MICEXi_AdjustLimitedSamplingAsync(rate, MicexSyncCallback, (void*)(&msgQ));
    if (result == MIC_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         MICEXi_AdjustLimitedSamplingAsync
  Description:  �T���v�����O���[�g����̃}�C�N�����T���v�����O�ɂ�����
                �T���v�����O�Ԋu�𒲐�����B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
  Arguments:    rate    -   �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B
                callback -  �񓯊������������ɌĂяo���R�[���o�b�N�֐����w��B
                arg     -   �R�[���o�b�N�֐��ɓn���p�����[�^���w��B
  Returns:      MICResult - �񓯊������J�n�����̌��ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MICEXi_AdjustLimitedSamplingAsync(u32 rate, MICCallback callback, void* arg)
{
    OSIntrMode  e;
    MICWork*    w   =   MICi_GetSysWork();

    SDK_NULL_ASSERT(callback);

    /* �p�����[�^�m�F */
    switch (rate)
    {
    case MIC_SAMPLING_RATE_32730:   case MIC_SAMPLING_RATE_16360:
    case MIC_SAMPLING_RATE_10910:   case MIC_SAMPLING_RATE_8180:
    case MIC_SAMPLING_RATE_47610:   case MIC_SAMPLING_RATE_23810:
    case MIC_SAMPLING_RATE_15870:   case MIC_SAMPLING_RATE_11900:
        break;
    default:
#ifdef  SDK_DEBUG
        OS_TWarning("%s: Illegal sampling rate. (%d)\n", __FUNCTION__, rate);
#endif
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    /* �r�����b�N�m�F */
    e   =   OS_DisableInterrupts();
    if (w->lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(e);
        return MIC_RESULT_BUSY;
    }
    w->lock =   MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(e);

    /* �����t�������T���v�����O�����R�}���h�𑗐M */
    w->callback =   callback;
    w->callbackArg  =   arg;
    if (MicexAdjustLimitedSampling(rate))
    {
        return MIC_RESULT_SUCCESS;
    }
    w->lock =   MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MicexSyncCallback
  Description:  �����֐����p�̃R�[���o�b�N�֐��B�x�~���Ă���X���b�h�ɔ񓯊�
                �������ʂ𑗂邱�Ƃœ���\��Ԃɕ��A������B
  Arguments:    result  -   �}�C�N����̔񓯊��������ʂ��n�����B
                arg     -   �����֐��Ăяo���X���b�h����M�ҋ@���Ă��郁�b�Z�[�W
                            �L���[�ւ̃|�C���^���n�����B
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
MicexSyncCallback(MICResult result, void* arg)
{
    SDK_NULL_ASSERT(arg);

    (void)OS_SendMessage((OSMessageQueue*)arg, (OSMessage)result, OS_MESSAGE_NOBLOCK);
}

/*---------------------------------------------------------------------------*
  Name:         MicexStartLimitedSampling
  Description:  �����t�������T���v�����O�J�n�R�}���h�� ARM7 �ɔ��s����B
  Arguments:    buf     -   �T���v�����O�����f�[�^���i�[����o�b�t�@�̃A�h���X�B
                size    -   �o�b�t�@�̃T�C�Y���o�C�g�P�ʂŎw��B
                rate    -   �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B
                flags   -   AD �ϊ��̃r�b�g���A���[�v�ہA�␳�ۂ��w��B
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇ TRUE�A
                            ���s�����ꍇ FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
MicexStartLimitedSampling(void* buf, u32 size, u32 rate, u8 flags)
{
    /* PXI�p�P�b�g[0] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            SPI_PXI_START_BIT | (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_LTDAUTO_ON << 8) | (u32)flags, 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[1] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            (1 << SPI_PXI_INDEX_SHIFT) | ((u32)buf >> 16), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[2] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            (2 << SPI_PXI_INDEX_SHIFT) | ((u32)buf & 0x0000ffff), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[3] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            (3 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[4] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            (4 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[5] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            (5 << SPI_PXI_INDEX_SHIFT) | (rate >> 16), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[6] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            SPI_PXI_END_BIT | (6 << SPI_PXI_INDEX_SHIFT) | (rate & 0x0000ffff), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicexStopLimitedSampling
  Description:  �����t�������T���v�����O��~�R�}���h�� ARM7 �ɔ��s����B
  Arguments:    None.
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇ TRUE�A
                            ���s�����ꍇ FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
MicexStopLimitedSampling(void)
{
    /* PXI�p�P�b�g[0] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            SPI_PXI_START_BIT | SPI_PXI_END_BIT | (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_LTDAUTO_OFF << 8), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicexAdjustLimitedSampling
  Description:  �����t�������T���v�����O���[�g�ύX�R�}���h�� ARM7 �ɔ��s����B
  Arguments:    rate    -   �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B
  Returns:      BOOL    -   ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇ TRUE�A
                            ���s�����ꍇ FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
MicexAdjustLimitedSampling(u32 rate)
{
    /* PXI�p�P�b�g[0] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            SPI_PXI_START_BIT | (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_LTDAUTO_ADJUST << 8), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[1] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            (1 << SPI_PXI_INDEX_SHIFT) | (rate >> 16), 0))
    {
        return FALSE;
    }

    /* PXI�p�P�b�g[2] */
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
            SPI_PXI_END_BIT | (2 << SPI_PXI_INDEX_SHIFT) | (rate & 0x0000ffff), 0))
    {
        return FALSE;
    }
    return TRUE;
}
