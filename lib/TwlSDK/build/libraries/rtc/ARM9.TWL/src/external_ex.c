/*---------------------------------------------------------------------------*
  Project:  TwlSDK - RTC - libraries
  File:     external_ex.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include    <nitro/os.h>
#include    <nitro/rtc.h>
#include    <twl/rtc/common/type_ex.h>
#include    <twl/rtc/ARM9/api_ex.h>
#include    "private.h"

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void     RtcexGetResultCallback(RTCResult result, void* arg);
static void     RtcexWaitBusy(u32* lock);

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_GetCounterAsync

  Description:  RTC����A�b�v�J�E���^�̒l��񓯊��œǂݏo���B

  Arguments:    count     - �A�b�v�J�E���^�̒l���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_GetCounterAsync(u32* count, RTCCallback callback, void* arg)
{
    OSIntrMode  enabled;
    RTCWork*    w   =   RTCi_GetSysWork();

    SDK_NULL_ASSERT(count);
    SDK_NULL_ASSERT(callback);

    /* ���b�N�m�F */
    enabled =   OS_DisableInterrupts();
    if (w->lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    w->lock =   RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    /* �A�b�v�J�E���^�ǂݏo���R�}���h�𑗐M */
    w->sequence =   RTC_SEQ_GET_COUNTER;
    w->index    =   0;
    w->buffer[0]    =   (void*)count;
    w->callback =   callback;
    w->callbackArg  =   arg;
    if (RTCEXi_ReadRawCounterAsync() == TRUE)
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        w->lock =   RTC_LOCK_OFF;
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_GetCounter

  Description:  RTC����A�b�v�J�E���^�̒l��ǂݏo���B

  Arguments:    count     - �A�b�v�J�E���^�̒l���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_GetCounter(u32* count)
{
    RTCWork*    w   =   RTCi_GetSysWork();

    w->commonResult =   RTCEXi_GetCounterAsync(count, RtcexGetResultCallback, NULL);
    if (w->commonResult == RTC_RESULT_SUCCESS)
    {
        RtcexWaitBusy(&(w->lock));
    }
    return w->commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_GetFoutAsync

  Description:  RTC����FOUT�̐ݒ�l��񓯊��œǂݏo���B

  Arguments:    fout      - FOUT�̐ݒ�l���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_GetFoutAsync(u16* fout, RTCCallback callback, void* arg)
{
    OSIntrMode  enabled;
    RTCWork*    w   =   RTCi_GetSysWork();

    SDK_NULL_ASSERT(fout);
    SDK_NULL_ASSERT(callback);

    /* ���b�N�m�F */
    enabled =   OS_DisableInterrupts();
    if (w->lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    w->lock =   RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    /* FOUT�ݒ�l�ǂݏo���R�}���h�𑗐M */
    w->sequence =   RTC_SEQ_GET_FOUT;
    w->index    =   0;
    w->buffer[0]    =   (void*)fout;
    w->callback =   callback;
    w->callbackArg  =   arg;
    if (RTCEXi_ReadRawFoutAsync() == TRUE)
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        w->lock =   RTC_LOCK_OFF;
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_GetFout

  Description:  RTC����FOUT�̐ݒ�l��񓯊��œǂݏo���B

  Arguments:    fout      - FOUT�̐ݒ�l���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_GetFout(u16* fout)
{
    RTCWork*    w   =   RTCi_GetSysWork();

    w->commonResult =   RTCEXi_GetFoutAsync(fout, RtcexGetResultCallback, NULL);
    if (w->commonResult == RTC_RESULT_SUCCESS)
    {
        RtcexWaitBusy(&(w->lock));
    }
    return w->commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_SetFoutAsync

  Description:  RTC��FOUT�ݒ�l��񓯊��ŏ������ށB

  Arguments:    fout      - FOUT�ݒ�l���i�[����Ă���o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_SetFoutAsync(const u16* fout, RTCCallback callback, void* arg)
{
    OSIntrMode  enabled;
    RTCWork*    w   =   RTCi_GetSysWork();

    SDK_NULL_ASSERT(fout);
    SDK_NULL_ASSERT(callback);

    /* ���b�N�m�F */
    enabled =   OS_DisableInterrupts();
    if (w->lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    w->lock =   RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    /* �ݒ肷��f�[�^��ҏW */
    ((RTCRawDataEx*)(OS_GetSystemWork()->real_time_clock))->a.fout.fout =   *fout;

    /* FOUT�ݒ�l�������݃R�}���h�𑗐M */
    w->sequence =   RTC_SEQ_SET_FOUT;
    w->index    =   0;
    w->callback =   callback;
    w->callbackArg  =   arg;
    if (RTCEXi_WriteRawFoutAsync() == TRUE)
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        w->lock =   RTC_LOCK_OFF;
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_SetFout

  Description:  RTC��FOUT�ݒ�l���������ށB

  Arguments:    fout      - FOUT�ݒ�l���i�[����Ă���o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_SetFout(const u16* fout)
{
    RTCWork*    w   =   RTCi_GetSysWork();

    w->commonResult =   RTCEXi_SetFoutAsync(fout, RtcexGetResultCallback, NULL);
    if (w->commonResult == RTC_RESULT_SUCCESS)
    {
        RtcexWaitBusy(&(w->lock));
    }
    return w->commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_CommonCallback

  Description:  �񓯊�RTC�֐��p�̒ǉ��R�}���h�ɑ΂��鋤�ʃR�[���o�b�N�֐��B

  Arguments:    tag -  PXI tag which show message type.
                data - message from ARM7.
                err -  PXI transfer error flag.

  Returns:      RTCResult   -   �񓯊��f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult
RTCEXi_CommonCallback(void)
{
    RTCWork*    w   =   RTCi_GetSysWork();
    RTCResult   result  =   RTC_RESULT_SUCCESS;

    /* ������Ԃɉ����Ċe�펖�㏈�� */
    switch(w->sequence)
    {
    /* �A�b�v�J�E���^�̒l�擾�V�[�P���X */
    case RTC_SEQ_GET_COUNTER:
        {
            u32*    pDst    =   (u32*)(w->buffer[0]);
            RTCRawCounter*  pSrc    =
                &(((RTCRawDataEx*)(OS_GetSystemWork()->real_time_clock))->a.counter);
            /* �G���f�B�A���ύX */
            *pDst   =   (u32)(pSrc->bytes[0] | (pSrc->bytes[1] << 8) | (pSrc->bytes[2] << 16));
        }
        break;
    /* FOUT�ݒ�l�擾�V�[�P���X */
    case RTC_SEQ_GET_FOUT:
        {
            u16*    pDst    =   (u16*)(w->buffer[0]);
            RTCRawFout* pSrc    =
                &(((RTCRawDataEx*)(OS_GetSystemWork()->real_time_clock))->a.fout);
            *pDst   =   (u16)(pSrc->fout);
        }
        break;
    /* FOUT�ݒ�l�ύX�V�[�P���X */
    case RTC_SEQ_SET_FOUT:
        /* ���ɏ����Ȃ� */
        break;
    default:
        /* ���̑��s���ȃV�[�P���X */
        w->index    =   0;
        result  =   RTC_RESULT_INVALID_COMMAND;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         RtcexGetResultCallback

  Description:  �񓯊������̊������ɌĂяo����A�����ϐ��̏������ʂ��X�V����B

  Arguments:    result - �񓯊��֐��̏������ʁB
                arg    - �g�p���Ȃ��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
RtcexGetResultCallback(RTCResult result, void* arg)
{
#pragma unused(arg)

    RTCi_GetSysWork()->commonResult =   result;
}

#include    <nitro/code32.h>
/*---------------------------------------------------------------------------*
  Name:         RtcWaitBusy

  Description:  RTC�̔񓯊����������b�N����Ă���ԑ҂B

  Arguments:    r0  -   ���C�u�������p���[�N���̃��b�N���䃁���o�ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void
RtcexWaitBusy(u32* lock)
{
loop:
    ldr     r1, [r0]
    cmp     r1, #RTC_LOCK_ON
    beq     loop
    bx      lr
}
#include    <nitro/codereset.h>
