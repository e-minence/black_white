/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SPI - libraries
  File:     mic.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include    <nitro/spi.h>
#include    <nitro/os/common/systemWork.h>

#ifdef  SDK_TWL
#include    <twl/os/common/codecmode.h>
#include    "micex.h"
#endif

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
#ifndef SDK_TWL
// �񓯊��֐��r�������p���b�N��`
typedef enum MICLock
{
    MIC_LOCK_OFF = 0,                  // ���b�N�J�����
    MIC_LOCK_ON,                       // ���b�N�{�����
    MIC_LOCK_MAX
}
MICLock;

// ���[�N�p�\����
typedef struct MICWork
{
    MICLock lock;                      // �r�����b�N
    MICCallback callback;              // �񓯊��֐��R�[���o�b�N�ޔ�p
    void   *callbackArg;               // �R�[���o�b�N�֐��̈����ۑ��p
    MICResult commonResult;            // �񓯊��֐��̏������ʑޔ�p
    MICCallback full;                  // �T���v�����O�����R�[���o�b�N�ޔ�p
    void   *fullArg;                   // �����R�[���o�b�N�֐��̈����ۑ��p
    void   *dst_buf;                   // �P�̃T���v�����O���ʊi�[�G���A�ޔ�p

}
MICWork;
#endif

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static u16 micInitialized;             // �������m�F�t���O
static MICWork micWork;                // ���[�N�ϐ����܂Ƃ߂��\����


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void MicCommonCallback(PXIFifoTag tag, u32 data, BOOL err);
static BOOL MicDoSampling(u16 type);
static BOOL MicStartAutoSampling(void *buf, u32 size, u32 span, u8 flags);
static BOOL MicStopAutoSampling(void);
static BOOL MicAdjustAutoSampling(u32 span);
static void MicGetResultCallback(MICResult result, void *arg);
static void MicWaitBusy(void);


/*---------------------------------------------------------------------------*
  Name:         MIC_Init

  Description:  �}�C�N���C�u����������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MIC_Init(void)
{
    // ���������m�F
    if (micInitialized)
    {
        return;
    }
    micInitialized = 1;

    // ���[�N�p�ϐ�������
    micWork.lock = MIC_LOCK_OFF;
    micWork.callback = NULL;

    // ARM7��MIC���C�u�������J�n�����܂ő҂�
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_MIC, PXI_PROC_ARM7))
    {
    }

    // shared�̈�̍ŐV�T���v�����O���ʊi�[�A�h���X���N���A
    OS_GetSystemWork()->mic_last_address = 0;

    // PXI�R�[���o�b�N�֐���ݒ�
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_MIC, MicCommonCallback);
}

/*---------------------------------------------------------------------------*
  Name:         MIC_DoSamplingAsync

  Description:  �}�C�N�����񓯊��ɃT���v�����O����B

  Arguments:    type      - �T���v�����O��ʂ��w��B
                buf       - �T���v�����O�f�[�^���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_DoSamplingAsync(MICSamplingType type, void *buf, MICCallback callback, void *arg)
{
    OSIntrMode enabled;
    u16     wtype;

    SDK_NULL_ASSERT(buf);
    SDK_NULL_ASSERT(callback);

    // �p�����[�^�m�F
    if (type >= MIC_SAMPLING_TYPE_MAX)
    {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    switch (type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_8BIT;
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_12BIT;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_S8BIT;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_S12BIT;
        break;
    default:
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    // ���b�N�m�F
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // �T���v�����O���s�R�}���h�𑗐M
    micWork.callback = callback;
    micWork.callbackArg = arg;
    micWork.dst_buf = buf;
    if (MicDoSampling(wtype))
    {
        return MIC_RESULT_SUCCESS;
    }
    micWork.lock = MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_DoSampling

  Description:  �}�C�N�����T���v�����O����B

  Arguments:    type      - �T���v�����O��ʂ��w��B
                buf       - �T���v�����O�f�[�^���i�[����o�b�t�@���w��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_DoSampling(MICSamplingType type, void *buf)
{
    micWork.commonResult = MIC_DoSamplingAsync(type, buf, MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StartAutoSamplingAsync

  Description:  �}�C�N�̎����T���v�����O��񓯊��ɊJ�n����B

  Arguments:    param        - �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B
                callback     - �񓯊����������������ۂɌĂяo���֐����w��B
                arg          - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult    - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StartAutoSamplingAsync(const MICAutoParam *param, MICCallback callback, void *arg)
{
    OSIntrMode enabled;
    u8      flags;

    SDK_NULL_ASSERT(callback);
    SDK_NULL_ASSERT(param->buffer);

    // �p�����[�^�m�F
    {
        // �o�b�t�@32�o�C�g�A���C��
        if ((u32)(param->buffer) & 0x01f)
        {
#ifdef  SDK_DEBUG
            OS_TWarning("Parameter param->buffer must be 32-byte aligned.\n");
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // �o�b�t�@�T�C�Y�A���C��
        if (param->size & 0x01f)
        {
#ifdef  SDK_DEBUG
            OS_TWarning("Parameter param->size must be a multiple of 32-byte.\n");
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // �o�b�t�@�T�C�Y
        if (param->size <= 0)
        {
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // �T���v�����O����
        if (param->rate < MIC_SAMPLING_RATE_LIMIT)
        {
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // AD�ϊ��r�b�g��
        switch (param->type)
        {
        case MIC_SAMPLING_TYPE_8BIT:
            flags = SPI_MIC_SAMPLING_TYPE_8BIT;
            break;
        case MIC_SAMPLING_TYPE_12BIT:
            flags = SPI_MIC_SAMPLING_TYPE_12BIT;
            break;
        case MIC_SAMPLING_TYPE_SIGNED_8BIT:
            flags = SPI_MIC_SAMPLING_TYPE_S8BIT;
            break;
        case MIC_SAMPLING_TYPE_SIGNED_12BIT:
            flags = SPI_MIC_SAMPLING_TYPE_S12BIT;
            break;
        case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
            flags = (SPI_MIC_SAMPLING_TYPE_12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
            break;
        case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
            flags = (SPI_MIC_SAMPLING_TYPE_S12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
            break;
        default:
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // ���[�v��
        if (param->loop_enable)
        {
            flags = (u8)(flags | SPI_MIC_SAMPLING_TYPE_LOOP_ON);
        }
        else
        {
            flags = (u8)(flags | SPI_MIC_SAMPLING_TYPE_LOOP_OFF);
        }
        // �g���p�␳�t���O�͌��� OFF �Œ�
        flags = (u8)(flags | SPI_MIC_SAMPLING_TYPE_CORRECT_OFF);
    }

    // ���b�N�m�F
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // �����T���v�����O�J�n�R�}���h�𑗐M
    micWork.callback = callback;
    micWork.callbackArg = arg;
    micWork.full = param->full_callback;
    micWork.fullArg = param->full_arg;
    if (MicStartAutoSampling(param->buffer, param->size, param->rate, flags))
    {
        return MIC_RESULT_SUCCESS;
    }
    micWork.lock = MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StartAutoSampling

  Description:  �}�C�N�̎����T���v�����O���J�n����B

  Arguments:    param     - �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StartAutoSampling(const MICAutoParam *param)
{
    micWork.commonResult = MIC_StartAutoSamplingAsync(param, MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StopAutoSamplingAsync

  Description:  �}�C�N�̎����T���v�����O��񓯊��ɒ�~����B

  Arguments:    callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StopAutoSamplingAsync(MICCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(callback);

    // ���b�N�m�F
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // �����T���v�����O��~�R�}���h�𑗐M
    micWork.callback = callback;
    micWork.callbackArg = arg;
    if (MicStopAutoSampling())
    {
        return MIC_RESULT_SUCCESS;
    }
    micWork.lock = MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StopAutoSampling

  Description:  �}�C�N�̎����T���v�����O���~����B
                �����T���v�����O�J�n���Ƀ��[�v�w�肵�Ȃ������ꍇ�̓o�b�t�@��
                ���܂������_�Ŏ����I�ɃT���v�����O�͒�~�����B

  Arguments:    None.

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StopAutoSampling(void)
{
    micWork.commonResult = MIC_StopAutoSamplingAsync(MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustAutoSamplingAsync

  Description:  �}�C�N�̎����T���v�����O�ɂ�����T���v�����O���[�g��񓯊���
                ��������B

  Arguments:    rate         - �T���v�����O���[�g���w��B
                callback     - �񓯊����������������ۂɌĂяo���֐����w��B
                arg          - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult    - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_AdjustAutoSamplingAsync(u32 rate, MICCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(callback);

    // �p�����[�^�m�F
    if (rate < MIC_SAMPLING_RATE_LIMIT)
    {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    // ���b�N�m�F
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // �����T���v�����O�����R�}���h�𑗐M
    micWork.callback = callback;
    micWork.callbackArg = arg;
    if (MicAdjustAutoSampling(rate))
    {
        return MIC_RESULT_SUCCESS;
    }
    micWork.lock = MIC_LOCK_OFF;
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustAutoSampling

  Description:  �}�C�N�̎����T���v�����O�ɂ�����T���v�����O���[�g�𒲐�����B

  Arguments:    rate         - �T���v�����O���[�g���w��B

  Returns:      MICResult    - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_AdjustAutoSampling(u32 rate)
{
    micWork.commonResult = MIC_AdjustAutoSamplingAsync(rate, MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_GetLastSamplingAddress

  Description:  �}�C�N�̍ŐV�T���v�����O���ʊi�[�A�h���X���擾����B

  Arguments:    None.

  Returns:      void* - �T���v�����O���ʊi�[�A�h���X��Ԃ��B
                        �܂��T���v�����O���Ă��Ȃ��ꍇ��NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
void   *MIC_GetLastSamplingAddress(void)
{
    return (void *)(OS_GetSystemWork()->mic_last_address);
}

#ifdef  SDK_TWL
/*---------------------------------------------------------------------------*
  Name:         MIC_StartLimitedSamplingAsync

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O��񓯊��ɊJ�n
                ����B�n�[�h�E�F�A�ɂ��T���v�����O���s�����ߐ��m�Ȏ��g���ł�
                �T���v�����O��Ⴂ CPU ���ׂōs�����Ƃ��ł��邪�A�T���v�����O
                ���[�g�̓n�[�h�E�F�A�̃T�|�[�g���郌�[�g�Ɍ��肳���B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O�J�n�֐��ő�ւ����B

  Arguments:    param   -   �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B
                callback -  �񓯊����������������ۂɌĂяo���֐����w��B
                arg     -   �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MIC_StartLimitedSamplingAsync(const MICAutoParam* param, MICCallback callback, void* arg)
{
    return ((OSi_IsCodecTwlMode() == TRUE) ?
            MICEXi_StartLimitedSamplingAsync(param, callback, arg) :
            MIC_StartAutoSamplingAsync(param, callback, arg));
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StopLimitedSamplingAsync

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O��񓯊��ɒ�~
                ����B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O��~�֐��ő�ւ����B

  Arguments:    callback -  �񓯊����������������ۂɌĂяo���֐����w��B
                arg     -   �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MIC_StopLimitedSamplingAsync(MICCallback callback, void* arg)
{
    return ((OSi_IsCodecTwlMode() == TRUE) ?
            MICEXi_StopLimitedSamplingAsync(callback, arg) :
            MIC_StopAutoSamplingAsync(callback, arg));
}

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustLimitedSamplingAsync

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O�ɂ�����T���v
                �����O���[�g��񓯊��ɒ�������B
                �񓯊��֐��Ȃ̂ŁA���ۂ̏������ʂ̓R�[���o�b�N���ɓn�����B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O�ɂ�����T���v�����O���[�g�����֐��ő�ւ����B

  Arguments:    rate    -   �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B
                callback -  �񓯊����������������ۂɌĂяo���֐����w��B
                arg     -   �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MIC_AdjustLimitedSamplingAsync(u32 rate, MICCallback callback, void* arg)
{
    return ((OSi_IsCodecTwlMode() == TRUE) ?
            MICEXi_AdjustLimitedSamplingAsync(rate, callback, arg) :
            MIC_AdjustAutoSamplingAsync(rate, callback, arg));
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StartLimitedSampling

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O���J�n����B
                �n�[�h�E�F�A�ɂ��T���v�����O���s�����ߐ��m�Ȏ��g���ł̃T���v
                �����O��Ⴂ CPU ���ׂōs�����Ƃ��ł��邪�A�T���v�����O���[�g
                �̓n�[�h�E�F�A�̃T�|�[�g���郌�[�g�Ɍ��肳���B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O�J�n�֐��ő�ւ����B

  Arguments:    param   -   �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MIC_StartLimitedSampling(const MICAutoParam* param)
{
    return ((OSi_IsCodecTwlMode() == TRUE) ?
            MICEXi_StartLimitedSampling(param) :
            MIC_StartAutoSampling(param));
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StopLimitedSampling

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O���~����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O��~�֐��ő�ւ����B

  Arguments:    None.

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MIC_StopLimitedSampling(void)
{
    return ((OSi_IsCodecTwlMode() == TRUE) ?
            MICEXi_StopLimitedSampling() :
            MIC_StopAutoSampling());
}

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustLimitedSampling

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O�ɂ�����T���v
                �����O���[�g�𒲐�����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O�ɂ�����T���v�����O���[�g�����֐��ő�ւ����B

  Arguments:    rate    -   �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult
MIC_AdjustLimitedSampling(u32 rate)
{
    return ((OSi_IsCodecTwlMode() == TRUE) ?
            MICEXi_AdjustLimitedSampling(rate) :
            MIC_AdjustAutoSampling(rate));
}
#endif

/*---------------------------------------------------------------------------*
  Name:         MicCommonCallback

  Description:  �񓯊�MIC�֐��p�̋��ʃR�[���o�b�N�֐��B

  Arguments:    tag -  PXI tag which show message type.
                data - message from ARM7.
                err -  PXI transfer error flag.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MicCommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag )

    u16     command;
    u16     pxiresult;
    MICResult result;
    MICCallback cb;

    // PXI�ʐM�G���[���m�F
    if (err)
    {
        if (micWork.lock != MIC_LOCK_OFF)
        {
            micWork.lock = MIC_LOCK_OFF;
        }
        if (micWork.callback)
        {
            cb = micWork.callback;
            micWork.callback = NULL;
            cb(MIC_RESULT_FATAL_ERROR, micWork.callbackArg);
        }
    }

    // ��M�f�[�^�����
    command = (u16)((data & SPI_PXI_RESULT_COMMAND_MASK) >> SPI_PXI_RESULT_COMMAND_SHIFT);
    pxiresult = (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);

    // �������ʂ��m�F
    switch (pxiresult)
    {
    case SPI_PXI_RESULT_SUCCESS:
        result = MIC_RESULT_SUCCESS;
        break;
    case SPI_PXI_RESULT_INVALID_COMMAND:
        result = MIC_RESULT_INVALID_COMMAND;
        break;
    case SPI_PXI_RESULT_INVALID_PARAMETER:
        result = MIC_RESULT_ILLEGAL_PARAMETER;
        break;
    case SPI_PXI_RESULT_ILLEGAL_STATUS:
        result = MIC_RESULT_ILLEGAL_STATUS;
        break;
    case SPI_PXI_RESULT_EXCLUSIVE:
        result = MIC_RESULT_BUSY;
        break;
    default:
        result = MIC_RESULT_FATAL_ERROR;
    }

    // �����ΏۃR�}���h���m�F
    if (command == SPI_PXI_COMMAND_MIC_BUFFER_FULL)
    {
        // �o�b�t�@�t���ʒm���R�[���o�b�N
        if (micWork.full)
        {
            micWork.full(result, micWork.fullArg);
        }
    }
    else
    {
        if (command == SPI_PXI_COMMAND_MIC_SAMPLING)
        {
            // �T���v�����O���ʂ��o�b�t�@�Ɋi�[
            if (micWork.dst_buf)
            {
                *(u16 *)(micWork.dst_buf) = OS_GetSystemWork()->mic_sampling_data;
            }
        }
        // �r�����b�N�J��
        if (micWork.lock != MIC_LOCK_OFF)
        {
            micWork.lock = MIC_LOCK_OFF;
        }
        // �������ʂ��R�[���o�b�N
        if (micWork.callback)
        {
            cb = micWork.callback;
            micWork.callback = NULL;
            cb(result, micWork.callbackArg);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MicDoSampling

  Description:  �}�C�N�����T���v�����O����R�}���h��ARM7�ɑ��M����B

  Arguments:    type - �T���v�����O�^�C�v
                ( 0: 8�r�b�g , 1: 12�r�b�g , 2: signed 8�r�b�g , 3: signed 12�r�b�g )

  Returns:      BOOL - ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                       ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL MicDoSampling(u16 type)
{
    // PXI�p�P�b�g[ 0 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_SAMPLING << 8) | (u32)type, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicStartAutoSampling

  Description:  �}�C�N�̎����T���v�����O�J�n�R�}���h��ARM7�ɑ��M����B

  Arguments:    buf   - �T���v�����O�����f�[�^���i�[����o�b�t�@�̃A�h���X�B
                size  - �o�b�t�@�̃T�C�Y�B�o�C�g�P�ʂŎw��B
                span  - �T���v�����O�Ԋu(ARM7��CPU�N���b�N�Ŏw��)�B
                        �^�C�}�[�̐�����A16�r�b�g�~1or64or256or1024 �̐���
                        �������m�ɂ͐ݒ�ł��Ȃ��B�[���r�b�g�͐؂�̂Ă���B
                flags - AD�ϊ��̃r�b�g���A���[�v�ہA�␳�ۂ��w��B

  Returns:      BOOL - ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                       ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL MicStartAutoSampling(void *buf, u32 size, u32 span, u8 flags)
{
    // PXI�p�P�b�g[ 0 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_AUTO_ON << 8) | (u32)flags, 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 1 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (1 << SPI_PXI_INDEX_SHIFT) | ((u32)buf >> 16), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 2 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (2 << SPI_PXI_INDEX_SHIFT) | ((u32)buf & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 3 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (3 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 4 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (4 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 5 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (5 << SPI_PXI_INDEX_SHIFT) | (span >> 16), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 6 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_END_BIT |
                               (6 << SPI_PXI_INDEX_SHIFT) | (span & 0x0000ffff), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicStopAutoSampling

  Description:  �}�C�N�̎����T���v�����O��~�R�}���h��ARM7�ɑ��M����B

  Arguments:    None.

  Returns:      BOOL - ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                       ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL MicStopAutoSampling(void)
{
    // PXI�p�P�b�g[ 0 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicAdjustAutoSampling

  Description:  �}�C�N�̎����T���v�����O���[�g�ύX�R�}���h��ARM7�ɑ��M����B

  Arguments:    span - �T���v�����O�Ԋu�� ARM7 �� CPU �N���b�N�P�ʂŎw��B

  Returns:      BOOL - ���߂�PXI�o�R�Ő���ɑ��M�ł����ꍇTRUE�A
                       ���s�����ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL MicAdjustAutoSampling(u32 span)
{
    // PXI�p�P�b�g[ 0 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_AUTO_ADJUST << 8), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 1 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (1 << SPI_PXI_INDEX_SHIFT) | (span >> 16), 0))
    {
        return FALSE;
    }

    // PXI�p�P�b�g[ 2 ]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (span & 0x0000ffff), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicGetResultCallback

  Description:  �񓯊������̊������ɌĂяo����A�����ϐ��̏������ʂ��X�V����B

  Arguments:    result - �񓯊��֐��̏������ʁB
                arg    - �g�p���Ȃ��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MicGetResultCallback(MICResult result, void *arg)
{
#pragma unused( arg )

    micWork.commonResult = result;
}

#include    <nitro/code32.h>
/*---------------------------------------------------------------------------*
  Name:         MicWaitBusy

  Description:  MIC�̔񓯊����������b�N����Ă���ԑ҂B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void
MicWaitBusy( void )
{
    ldr     r12,    =micWork.lock
loop:
    ldr     r0,     [ r12,  #0 ]
    cmp     r0,     #MIC_LOCK_ON
    beq     loop
    bx      lr
}
#include    <nitro/codereset.h>

#ifdef  SDK_TWL
/*---------------------------------------------------------------------------*
  Name:         MICi_GetSysWork

  Description:  MIC ���C�u��������p�\���̂��擾����B

  Arguments:    None.

  Returns:      MICWork*    -   �\���̂ւ̃|�C���^��Ԃ��B
 *---------------------------------------------------------------------------*/
MICWork*
MICi_GetSysWork(void)
{
    return &micWork;
}
#endif

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
