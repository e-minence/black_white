/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - snd
  File:     sndex.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-13#$
  $Rev: 10889 $
  $Author: nishimoto_takashi $
 *---------------------------------------------------------------------------*/

#include    <twl/os.h>
#include    <twl/snd/ARM9/sndex.h>
#include    <nitro/pxi.h>
#include    <twl/os/common/codecmode.h>


/*---------------------------------------------------------------------------*
    �}�N����`
 *---------------------------------------------------------------------------*/
#ifdef  SDK_DEBUG
#define     SNDEXi_Warning          OS_TWarning
#else
#define     SNDEXi_Warning(...)     ((void)0)
#endif

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/

/* �񓯊������p���[�N�\���� */
typedef struct SNDEXWork
{
    SNDEXCallback   callback;
    void*           cbArg;
    void*           dest;
    u8              command;
    u8              padding[3];

} SNDEXWork;

/* ������� */
typedef enum SNDEXState
{
    SNDEX_STATE_BEFORE_INIT       =   0,
    SNDEX_STATE_INITIALIZING      =   1,
    SNDEX_STATE_INITIALIZED       =   2,
    SNDEX_STATE_LOCKED            =   3,
    SNDEX_STATE_PLAY_SHUTTER      =   4,        // �V���b�^�[���Đ����i Pre,PostProcessForShutterSound �ȊO�� SNDEX API ��r������j
    SNDEX_STATE_POST_PROC_SHUTTER =   5,        // �V���b�^�[���Đ��̌㏈�����i�S�Ă� SNDEX API ��r������j
    SNDEX_STATE_MAX

} SNDEXState;

/* �����o�̓f�o�C�X�ݒ� */
typedef enum SNDEXDevice
{
    SNDEX_DEVICE_AUTO       =   0,
    SNDEX_DEVICE_SPEAKER    =   1,
    SNDEX_DEVICE_HEADPHONE  =   2,
    SNDEX_DEVICE_BOTH       =   3,
    SNDEX_DEVICE_MAX

} SNDEXDevice;

/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/
static volatile SNDEXState   sndexState  =   SNDEX_STATE_BEFORE_INIT;
static SNDEXWork    sndexWork;
static SNDEXVolumeSwitchCallbackInfo SNDEXi_VolumeSwitchCallbackInfo = {NULL, NULL, NULL, 0};

PMSleepCallbackInfo sleepCallbackInfo;
PMExitCallbackInfo  exitCallbackInfo;
static volatile BOOL         isIirFilterSetting = FALSE;     // ���ł� SetIirFilterAsync �̎��s���Ȃ� TRUE
static volatile BOOL         isLockSpi = FALSE;              // SNDEX_SetIirFilter[Async] �̒��ŁASPI �̔r�����䒆���ǂ���

static volatile BOOL         isPlayShutter = FALSE;          // �V���b�^�[���Đ��������Ȃ�� TRUE
static volatile BOOL         isStoreVolume = FALSE;          // SNDEXi_SetIgnoreHWVolume �ɂ���ĕۑ�����Ă��鉹�ʒl�����݂��邩�ǂ���
static          u8           storeVolume   = 0;              // SNDEXi_SetIgnoreHWVolume �ɂ���Ĉꎞ�I�ɕύX�����O�̉��ʒl

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/
static void         CommonCallback  (PXIFifoTag tag, u32 data, BOOL err);
static void         SyncCallback    (SNDEXResult result, void* arg);
static BOOL         SendCommand     (u8 command, u8 param);
static BOOL         SendCommandEx   (u8 command, u16 param);
static SNDEXResult  CheckState       (void);
static void         ReplyCallback   (SNDEXResult result);
static void         SetSndexWork    (SNDEXCallback cb, void* cbarg, void* dst, u8 command);

static void         SndexSleepAndExitCallback    (void *arg);

static void         WaitIirFilterSetting   (void);
static void         WaitResetSoundCallback (SNDEXResult result, void* arg);
static void         ResetTempVolume        (void);

SNDEXResult SNDEXi_GetDeviceAsync       (SNDEXDevice* device, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_GetDevice            (SNDEXDevice* device);
SNDEXResult SNDEXi_SetDeviceAsync       (SNDEXDevice device, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_SetDevice            (SNDEXDevice device);

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_Init

  Description:  �T�E���h�g���@�\���C�u����������������B

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void
SNDEXi_Init (void)
{
    OSIntrMode  e   =   OS_DisableInterrupts();

    /* �������ς݊m�F */
    if (sndexState != SNDEX_STATE_BEFORE_INIT)
    {
        (void)OS_RestoreInterrupts(e);
        SNDEXi_Warning("%s: Already initialized.\n", __FUNCTION__);
        return;
    }
    sndexState  =   SNDEX_STATE_INITIALIZING;
    (void)OS_RestoreInterrupts(e);

    /* �ϐ������� */
    SetSndexWork(NULL, NULL, NULL, 0);

    /* ARM7 �� SNDEX ���C�u�������J�n�����܂őҋ@ */
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_SNDEX, PXI_PROC_ARM7))
    {
        SVC_WaitByLoop(10);
    }
    /* PXI �R�[���o�b�N�֐���ݒ� */
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_SNDEX, CommonCallback);
    
    /* �n�[�h�E�F�A���Z�b�g�A�V���b�g�_�E������ �㏈���R�[���o�b�N�֐���ݒ�*/
    PM_SetExitCallbackInfo(&exitCallbackInfo, SndexSleepAndExitCallback, NULL);
    PMi_InsertPostExitCallbackEx(&exitCallbackInfo, PM_CALLBACK_PRIORITY_SNDEX);
    
    /* �X���[�v�O�̃R�[���o�b�N�o�^ */
    PM_SetSleepCallbackInfo(&sleepCallbackInfo, SndexSleepAndExitCallback, NULL);
    PMi_InsertPreSleepCallbackEx(&sleepCallbackInfo, PM_CALLBACK_PRIORITY_SNDEX);

    /* ���������� */
    sndexState  =   SNDEX_STATE_INITIALIZED;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetMuteAsync

  Description:  �����o�͂̏����ݒ��Ԃ�񓯊��Ɏ擾����B
                ���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    mute        -   �����ݒ��Ԃ��擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetMuteAsync (SNDEXMute* mute, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����Ȃ� */
    
    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, (void*)mute, SNDEX_PXI_COMMAND_GET_SMIX_MUTE);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetMute

  Description:  �����o�͂̏����ݒ��Ԃ��擾����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    mute    -   �����ݒ��Ԃ��擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetMute (SNDEXMute* mute)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_GetMuteAsync(mute, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetI2SFrequencyAsync

  Description:  I2S ���g������񓯊��Ɏ擾����B
                ���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    freq        -   ���g�������擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetI2SFrequencyAsync (SNDEXFrequency* freq, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����Ȃ� */

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, (void*)freq, SNDEX_PXI_COMMAND_GET_SMIX_FREQ);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetI2SFrequency

  Description:  I2S ���g�������擾����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    freq    -   ���g�������擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetI2SFrequency(SNDEXFrequency* freq)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_GetI2SFrequencyAsync(freq, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetDSPMixRateAsync

  Description:  CPU �� DSP �̉����o�͍���������擾����B
                ������͍ŏ�: 0 (DSP 100%), �ő�: 8 (CPU 100%)�B
                ���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    rate        -   ����������擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetDSPMixRateAsync (u8* rate, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����Ȃ� */

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, (void*)rate, SNDEX_PXI_COMMAND_GET_SMIX_DSP);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetDSPMixRate

  Description:  CPU �� DSP �̉����o�͍���������擾����B
                ������͍ŏ�: 0 (DSP 100%), �ő�: 8 (CPU 100%)�B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    rate    -   ����������擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetDSPMixRate (u8* rate)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_GetDSPMixRateAsync(rate, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetVolumeAsync

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 7 �B
                ���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    volume      -   ���ʏ����擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B
                eightlv     -   ���ʂ� 8�i�K�Ŏ擾����B
                keep        -   �ύX�\�񂳂�Ă��鉹�ʂ��擾����B�\�񂪖����ꍇ�͌��݂̒l���擾����B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetVolumeAsync (u8* volume, SNDEXCallback callback, void* arg, BOOL eightlv, BOOL keep)
{
    /* �p�����[�^�����Ȃ� */

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, (void*)volume, SNDEX_PXI_COMMAND_GET_VOLUME);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command,
        (u8)((keep << SNDEX_PXI_PARAMETER_SHIFT_VOL_KEEP) | (eightlv << SNDEX_PXI_PARAMETER_SHIFT_VOL_8LV))))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetVolume

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 7 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʏ����擾����o�b�t�@���w�肷��B
                eightlv -   ���ʂ� 8�i�K�Ŏ擾����B
                keep    -   �ύX�\�񂳂�Ă��鉹�ʂ��擾����B�\�񂪖����ꍇ�͌��݂̒l���擾����B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetVolume (u8* volume, BOOL eightlv, BOOL keep)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_GetVolumeAsync(volume, SyncCallback, (void*)(&msgQ), eightlv, keep);
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetVolumeExAsync

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                ���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    volume      -   ���ʏ����擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetVolumeExAsync (u8* volume, SNDEXCallback callback, void* arg)
{
    if (!OS_IsRunOnTwl())
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    return SNDEXi_GetVolumeAsync(volume, callback, arg, FALSE, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetVolumeEx

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʏ����擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetVolumeEx (u8* volume)
{
    if (!OS_IsRunOnTwl())
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    return SNDEXi_GetVolume(volume, FALSE, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetCurrentVolumeEx[Async]

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                �����֐��̏ꍇ�A�����݃n���h��������̌Ăяo���͋֎~�B
                �񓯊��֐��̏ꍇ�A���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    volume   -   ���ʏ����擾����o�b�t�@���w�肷��B
               [callback -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N�֐����w�肷��B]
               [arg      -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B]

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetCurrentVolumeExAsync (u8* volume, SNDEXCallback callback, void* arg)
{
    if (!OS_IsRunOnTwl())
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    return SNDEXi_GetVolumeAsync(volume, callback, arg, FALSE, FALSE);
}

SNDEXResult
SNDEXi_GetCurrentVolumeEx (u8* volume)
{
    if (!OS_IsRunOnTwl())
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    return SNDEXi_GetVolume(volume, FALSE, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetDeviceAsync

  Description:  �����o�̓f�o�C�X�ݒ�����擾����B
                ���ʎ擾�p�o�b�t�@�ɂ͊֐��Ăяo���Ƃ͔񓯊��Ɍ��ʂ��������܂�
                �邽�߁A�񓯊���������������܂ő��̗p�r�Ɏg�p���ׂ��łȂ��B

  Arguments:    device      -   �o�̓f�o�C�X�����擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetDeviceAsync (SNDEXDevice* device, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����Ȃ� */

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, (void*)device, SNDEX_PXI_COMMAND_GET_SND_DEVICE);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_GetDevice

  Description:  �����o�̓f�o�C�X�ݒ�����擾����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    device  -   �o�̓f�o�C�X�����擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_GetDevice (SNDEXDevice* device)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_GetDeviceAsync(device, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetMuteAsync

  Description:  �����o�͂̏����ݒ��Ԃ�ύX����B

  Arguments:    mute        -   �����ݒ��Ԃ��w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetMuteAsync (SNDEXMute mute, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����m�F */
    if (mute >= SNDEX_MUTE_MAX)
    {
        SNDEXi_Warning("%s: Invalid parameter (mute: %d)\n", __FUNCTION__, mute);
        return SNDEX_RESULT_INVALID_PARAM;
    }

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SMIX_MUTE);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, (u8)mute))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetMute

  Description:  �����o�͂̏����ݒ��Ԃ�ύX����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    mute    -   �����ݒ��Ԃ��w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetMute (SNDEXMute mute)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_SetMuteAsync(mute, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetI2SFrequencyAsync

  Description:  I2S ���g����ύX����B

  Arguments:    freq        -   ���g�����w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetI2SFrequencyAsync (SNDEXFrequency freq, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����m�F */
    if (freq >= SNDEX_FREQUENCY_MAX)
    {
        SNDEXi_Warning("%s: Invalid parameter (freq: %d)\n", __FUNCTION__, freq);
        return SNDEX_RESULT_INVALID_PARAM;
    }

    /* CODEC ���샂�[�h���m�F */
    if (OSi_IsCodecTwlMode() == FALSE)
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SMIX_FREQ);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, (u8)freq))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetI2SFrequency

  Description:  I2S ���g����ύX����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    freq    -   ���g�����w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetI2SFrequency (SNDEXFrequency freq)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_SetI2SFrequencyAsync(freq, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetDSPMixRateAsync

  Description:  CPU �� DSP �̉����o�͍������ύX����B
                ������͍ŏ�: 0 (DSP 100%), �ő�: 8 (CPU 100%)�B

  Arguments:    rate        -   ������� 0 ���� 8 �̐��l�Ŏw�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetDSPMixRateAsync (u8 rate, SNDEXCallback callback, void* arg)
{
    /* CODEC ���샂�[�h���m�F */
    if (OSi_IsCodecTwlMode() == FALSE)
    {
        SNDEXi_Warning("%s: Illegal state\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    /* �p�����[�^�����m�F */
    if (rate > SNDEX_DSP_MIX_RATE_MAX)
    {
        SNDEXi_Warning("%s: Invalid parameter (rate: %u)\n", __FUNCTION__, rate);
        return SNDEX_RESULT_INVALID_PARAM;
    }

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SMIX_DSP);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, (u8)rate))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetDSPMixRate

  Description:  CPU �� DSP �̉����o�͍������ύX����B
                ������͍ŏ�: 0 (DSP 100%), �ő�: 8 (CPU 100%)�B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    rate    -   ������� 0 ���� 8 �̐��l�Ŏw�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetDSPMixRate (u8 rate)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_SetDSPMixRateAsync(rate, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetVolumeAsync

  Description:  �����o�͉��ʂ�ύX����B���ʂ͍ŏ�: 0, �ő�: 7 �B

  Arguments:    volume      -   ���ʂ� 0 ���� 7 �̐��l�Ŏw�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B
                eightlv     -   ���ʂ� 8�i�K�Őݒ肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetVolumeAsync (u8 volume, SNDEXCallback callback, void* arg, BOOL eightlv)
{
    /* �p�����[�^�����m�F */
    if (volume > (eightlv ? SNDEX_VOLUME_MAX : SNDEX_VOLUME_MAX_EX))
    {
        SNDEXi_Warning("%s: Invalid parameter (volume: %u)\n", __FUNCTION__, volume);
        return SNDEX_RESULT_INVALID_PARAM;
    }

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_VOLUME);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 
        (u8)((eightlv << SNDEX_PXI_PARAMETER_SHIFT_VOL_8LV) | (SNDEX_PXI_PARAMETER_MASK_VOL_VALUE & volume))))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetVolume

  Description:  �����o�͉��ʂ�ύX����B���ʂ͍ŏ�: 0, �ő�: 7 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʂ� 0 ���� 7 �̐��l�Ŏw�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetVolume (u8 volume, BOOL eightlv)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_SetVolumeAsync(volume, SyncCallback, (void*)(&msgQ), eightlv);
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetVolumeExAsync

  Description:  �����o�͉��ʂ�ύX����B���ʂ͍ŏ�: 0, �ő�: 31 �B

  Arguments:    volume      -   ���ʂ� 0 ���� 31 �̐��l�Ŏw�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetVolumeExAsync (u8 volume, SNDEXCallback callback, void* arg)
{
    if (!OS_IsRunOnTwl())
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    return SNDEXi_SetVolumeAsync(volume, callback, arg, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetVolumeEx

  Description:  �����o�͉��ʂ�ύX����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʂ� 0 ���� 31 �̐��l�Ŏw�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetVolumeEx (u8 volume)
{
    if (!OS_IsRunOnTwl())
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    return SNDEXi_SetVolume(volume, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetDeviceAsync

  Description:  �����o�̓f�o�C�X�ݒ��ύX����B

  Arguments:    device      -   �o�̓f�o�C�X�ݒ���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetDeviceAsync (SNDEXDevice device, SNDEXCallback callback, void* arg)
{
    /* �p�����[�^�����m�F */
    if (device >= SNDEX_DEVICE_MAX)
    {
        SNDEXi_Warning("%s: Invalid parameter (device: %d)\n", __FUNCTION__, device);
        return SNDEX_RESULT_INVALID_PARAM;
    }

    /* CODEC ���샂�[�h���m�F */
    if (OSi_IsCodecTwlMode() == FALSE)
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_SND_DEVICE);

    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, (u8)device))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetDevice

  Description:  �����o�̓f�o�C�X�ݒ��ύX����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    device  -   �o�̓f�o�C�X�ݒ���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetDevice (SNDEXDevice device)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_SetDeviceAsync(device, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetIirFilterAsync

  Description:  IIR�t�B���^�[(Biquad)�̃p�����[�^�ݒ��񓯊��ōs���܂��B

  !! CAUTION !!: ARM7�ւP�x��PXI���M�ł̓p�����[�^��S�ēn�����Ƃ��ł��Ȃ��̂�
                 ������A�����M���Ă���B�����A���ő��M���邾���ł́A�O�ɑ�����
                 �p�����[�^�̏������I����ĂȂ����Ƃ����邽�߁APXI���M�̑O��
                 �����̏I����҂��Ƃɂ��Ă���B

  Arguments:    target : 
                pParam :

  Returns:      None
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetIirFilterAsync(SNDEXIirFilterTarget target, const SNDEXIirFilterParam* pParam, SNDEXCallback callback, void* arg)
{
    // ���� SetIirFilterAsync �̏������Ȃ�΃G���[��Ԃ�
    if (isIirFilterSetting)
    {
        return SNDEX_RESULT_EXCLUSIVE;
    }
    isIirFilterSetting = TRUE;
    
    /* CODEC ���샂�[�h���m�F */
    if (OSi_IsCodecTwlMode() == FALSE)
    {
        SNDEXi_Warning("%s: Illegal state\n", __FUNCTION__);
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            isIirFilterSetting = FALSE;
            return result;
        }
    }
    
  /* �^�[�Q�b�g */
    /* �񓯊������o�^ */
    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_TARGET);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, (u16)target))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    // CheckState() �ɂ�� Warning �}���̂��߂ɋ󃋁[�v���񂵂ăR�[���o�b�N��҂�
    OS_SpinWait(67 * 1000); //�� 1ms
    
    // �O�� PXI���M�ɂ�鏈�����I���܂ő҂�
    while ( CheckState() != SNDEX_RESULT_SUCCESS )
    {
        OS_SpinWait(67 * 1000); //�� 1ms
    }
    
  /* �t�B���^�p�����[�^ */
    // n0
    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N0);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, pParam->n0))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    OS_SpinWait(67 * 1000); //�� 1ms

    // �O�� PXI���M�ɂ�鏈�����I���܂ő҂�
    while ( CheckState() != SNDEX_RESULT_SUCCESS )
    {
        OS_SpinWait(67 * 1000); //�� 1ms
    }

    // n1
    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N1);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, pParam->n1))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    OS_SpinWait(67 * 1000); //�� 1ms
    
    // �O�� PXI���M�ɂ�鏈�����I���܂ő҂�
    while ( CheckState() != SNDEX_RESULT_SUCCESS )
    {
        OS_SpinWait(67 * 1000); //�� 1ms
    }
    
    // n2
    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N2);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, pParam->n2))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    OS_SpinWait(67 * 1000); //�� 1ms
    
    // �O�� PXI���M�ɂ�鏈�����I���܂ő҂�
    while ( CheckState() != SNDEX_RESULT_SUCCESS )
    {
        OS_SpinWait(67 * 1000); //�� 1ms
    }
    
    // d1
    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D1);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, pParam->d1))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    OS_SpinWait(67 * 1000); //�� 1ms
    
    // �O�� PXI���M�ɂ�鏈�����I���܂ő҂�
    while ( CheckState() != SNDEX_RESULT_SUCCESS )
    {
        OS_SpinWait(67 * 1000); //�� 1ms
    }
    
    // d2
    SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D2);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, pParam->d2))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    OS_SpinWait(67 * 1000); //�� 1ms
    
    // �O�� PXI���M�ɂ�鏈�����I���܂ő҂�
    while ( CheckState() != SNDEX_RESULT_SUCCESS )
    {
        OS_SpinWait(67 * 1000); //�� 1ms
    }
    
  /* �^�[�Q�b�g�A�t�B���^�p�����[�^���Z�b�g */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_SET_IIRFILTER);
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommandEx(sndexWork.command, 0))
    {
        isIirFilterSetting = FALSE;
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetIirFilter

  Description:  IIR�t�B���^�[(Biquad)�̃p�����[�^�ݒ���s���܂��B

  Arguments:    target : 
                pParam :

  Returns:      None
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_SetIirFilter(SNDEXIirFilterTarget target, const SNDEXIirFilterParam* pParam)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);
    
    result = SNDEXi_SetIirFilterAsync(target, pParam, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    
    // SetIirFilter �ɂ�� SPI_Lock ���I������Ƃ݂Ȃ��ăt���O�����낷
    isLockSpi = FALSE;
    
    isIirFilterSetting = FALSE;
    
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_IsConnectedHeadphoneAsync

  Description:  �w�b�h�t�H���̐ڑ��̗L�����擾����B

  Arguments:    hp          -   �w�b�h�t�H���̐ڑ��󋵂��擾����o�b�t�@���w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_IsConnectedHeadphoneAsync(SNDEXHeadphone *hp, SNDEXCallback callback, void* arg)
{
    /* ��Ԋm�F */
    {
        SNDEXResult result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }

    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, (void*)hp, SNDEX_PXI_COMMAND_HP_CONNECT);
    
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_IsConnectedHeadphone

  Description:  �w�b�h�t�H���̐ڑ��̗L�����擾����B�B

  Arguments:    hp          -   �w�b�h�t�H���̐ڑ��󋵂��擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_IsConnectedHeadphone(SNDEXHeadphone *hp)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];

    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }

    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);

    /* �񓯊��֐������s���A�R�[���o�b�N��ҋ@ */
    result  =   SNDEXi_IsConnectedHeadphoneAsync(hp, SyncCallback, (void*)(&msgQ));
    if (result == SNDEX_RESULT_SUCCESS)
    {
        (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetVolumeSwitchCallback

  Description:  ���ʒ����{�^���������ɌĂ΂��R�[���o�b�N�֐���ݒ肷��B

  Arguments:    callback    -   ���ʒ����{�^���������ɌĂ΂��R�[���o�b�N�֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void SNDEXi_SetVolumeSwitchCallback(SNDEXCallback callback, void* arg)
{
    SNDEXi_VolumeSwitchCallbackInfo.callback = callback;
    SNDEXi_VolumeSwitchCallbackInfo.cbArg = arg;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_SetIgnoreHWVolume

  Description:  ���v�̃A���[�����ȂǁA���̂Ƃ��̖{�̉��ʂ̒l�ɂƂ��ꂸ
                �w�肵�����ʂŉ���炷�K�v������ꍇ�A���[�U�̑���ɂ�炸
                ���ʕύX���s��˂΂Ȃ�Ȃ��B
                �{�֐��ł́A���ʕύX���s�������łȂ��A�ύX�O�̉��ʂ�ۑ�����B

  Arguments:    volume      -   �ύX���������ʒl�B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B�����ŌĂ�ł��� SNDEX API �̌��ʁB
 *---------------------------------------------------------------------------*/
SNDEXResult SNDEXi_SetIgnoreHWVolume(u8 volume, BOOL eightlv)
{
    SNDEXResult result;
    
    // �O�� SetIgnoreHWVolume ���Ă�ł��� ResetIgnoreHWVolume �����s����Ă��Ȃ��ꍇ��
    // ���ʒl��ۑ����Ȃ�
    if (!isStoreVolume)
    {
        result = SNDEXi_GetVolumeEx(&storeVolume);
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }
    
    // �w�肵�����ʂ֕ύX
    result = eightlv ? SNDEX_SetVolume(volume) : SNDEXi_SetVolumeEx(volume);
    if (result != SNDEX_RESULT_SUCCESS)
    {
        return result;
    }
    
    isStoreVolume = TRUE;
    return result;      // SNDEX_RESULT_SUCCESS
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_ResetIgnoreHWVolume

  Description:  SNDEX_SetIgnoreHWVolume �ŕύX�������ʂ��A�ύX�O�̉��ʂ֖߂��B

  Arguments:    �Ȃ�
  
  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult SNDEXi_ResetIgnoreHWVolume(void)
{
    SNDEXResult result;
    if ((result = SNDEXi_SetVolumeEx(storeVolume)) != SNDEX_RESULT_SUCCESS)
    {
        return result;
    }
    
    isStoreVolume = FALSE;
    return result;          // SNDEX_RESULT_SUCCESS
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_PreProcessForShutterSound

  Description:  �V���b�^�[���Đ��̂��߂ɕK�v�ȑO�������܂Ƃ߂��֐��B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    �Ȃ�

  Returns:      ��������� SNDEX_RESULT_SUCCESS ��Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_PreProcessForShutterSound  (void)
{
    SNDEXResult     result;
    OSMessageQueue  msgQ;
    OSMessage       msg[1];
    
    /* CODEC ���샂�[�h���m�F */
    if (OSi_IsCodecTwlMode() == FALSE)
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    /* ��O�n���h������̌Ăяo���͋֎~ */
    if (OS_GetCurrentThread() == NULL)
    {
        SNDEXi_Warning("%s: Syncronous API could not process in exception handler.\n", __FUNCTION__);
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    /* ���� SNDEXi_PreProcessForShutterSound() ���Ă΂�Ă����ԂȂ� CheckState �̓X���[ */
    if (sndexState != SNDEX_STATE_PLAY_SHUTTER)
    {
        /* ��Ԋm�F */
        result  =   CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return result;
        }
    }
    else if (sndexState == SNDEX_STATE_POST_PROC_SHUTTER)
    {
        return SNDEX_RESULT_EXCLUSIVE;
    }
    
    /* �����p���b�Z�[�W�L���[�������� */
    OS_InitMessageQueue(&msgQ, msg, 1);
    
    
    /* �����o�^ */
    SetSndexWork(SyncCallback, (void*)(&msgQ), NULL, SNDEX_PXI_COMMAND_PRE_PROC_SHUTTER);
    
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }

    /* ���v���C�҂� */
    (void)OS_ReceiveMessage(&msgQ, (OSMessage*)(&result), OS_MESSAGE_BLOCK);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SNDEXi_PostProcessCallbackForShutterSound

  Description:  �V���b�^�[���Đ��̂��߂̌㏈�������s����B
                ���荞�݂̒��Ŏ��s����邽�߁A�񓯊��Ŏ��s����B

  Arguments:    callback    -   �㏈���I�����ɌĂ΂��R�[���o�b�N�֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      ��������� SNDEX_RESULT_SUCCESS ��Ԃ��B
 *---------------------------------------------------------------------------*/
SNDEXResult
SNDEXi_PostProcessForShutterSound (SNDEXCallback callback, void* arg)
{
    /* CODEC ���샂�[�h���m�F */
    // ���̊֐����Ă΂��Ƃ��́ASNDEXi_PreProcessCallbackForShutterSound �̎��s�オ�O��
    if (OSi_IsCodecTwlMode() == FALSE || sndexState != SNDEX_STATE_PLAY_SHUTTER)
    {
        return SNDEX_RESULT_ILLEGAL_STATE;
    }
    
    /* �X�e�[�g�� �㏈�����ɑJ�ځi�D��x�ő�j */
    sndexState = SNDEX_STATE_POST_PROC_SHUTTER;
    
    /* �񓯊������o�^ */
    SetSndexWork(callback, arg, NULL, SNDEX_PXI_COMMAND_POST_PROC_SHUTTER);
    
    /* ARM7 �փR�}���h���s */
    if (FALSE == SendCommand(sndexWork.command, 0))
    {
        return SNDEX_RESULT_PXI_SEND_ERROR;
    }
    
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         CommonCallback

  Description:  �T�E���h�g���@�\�Ɋւ��� PXI FIFO ��M�R�[���o�b�N�֐��B

  Arguments:    tag     -   PXI ���b�Z�[�W��ʁB
                data    -   ARM7 ����̃��b�Z�[�W���e�B
                err     -   PXI �]���G���[�t���O�B

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
CommonCallback (PXIFifoTag tag, u32 data, BOOL err)
{
    u8      command =   (u8)((data & SNDEX_PXI_COMMAND_MASK) >> SNDEX_PXI_COMMAND_SHIFT);
    u8      result  =   (u8)((data & SNDEX_PXI_RESULT_MASK) >> SNDEX_PXI_RESULT_SHIFT);
    u8      param   =   (u8)((data & SNDEX_PXI_PARAMETER_MASK) >> SNDEX_PXI_PARAMETER_SHIFT);
    
    /* ARM7 �����M�����R�}���h���{�̃{�����[���X�C�b�`�����ʒm�Ȃ烍�b�N�����݂� */
    if (command == SNDEX_PXI_COMMAND_PRESS_VOLSWITCH)
    {
        SNDEXResult result = CheckState();
        if (result != SNDEX_RESULT_SUCCESS)
        {
            return;
        }
        // sndexWork �\���̂��Z�b�g����
        SetSndexWork(NULL, NULL, NULL, SNDEX_PXI_COMMAND_PRESS_VOLSWITCH);
    }
    
    /* ��Ԋm�F */
    if ((tag != PXI_FIFO_TAG_SNDEX) || (err == TRUE)
            || (sndexState != SNDEX_STATE_LOCKED && sndexState != SNDEX_STATE_PLAY_SHUTTER && sndexState != SNDEX_STATE_POST_PROC_SHUTTER)
            || (sndexWork.command != command))
    {
        /* ���W�b�N��͒ʂ蓾�Ȃ����A�O�̂��ߌx�����o�� */
        OS_TWarning("SNDEX: Library state is inconsistent.\n");
        ReplyCallback(SNDEX_RESULT_FATAL_ERROR);
        return;
    }

    /* �������ʉ�� */
    switch (result)
    {
    case SNDEX_PXI_RESULT_SUCCESS:
        /* �R�}���h�ɉ����Ċ������� */
        switch (command)
        {
        case SNDEX_PXI_COMMAND_GET_SMIX_MUTE:
            if (sndexWork.dest != NULL)
            {
                *((SNDEXMute*)sndexWork.dest)   = (SNDEXMute)param;
            }
            break;
        case SNDEX_PXI_COMMAND_GET_SMIX_FREQ:
            if (sndexWork.dest != NULL)
            {
                *((SNDEXFrequency*)sndexWork.dest)  =   (SNDEXFrequency)param;
            }
            break;
        case SNDEX_PXI_COMMAND_GET_SMIX_DSP:
        case SNDEX_PXI_COMMAND_GET_VOLUME:
            if (sndexWork.dest != NULL)
            {
                *((u8*)sndexWork.dest)  =   param;
            }
            break;
        case SNDEX_PXI_COMMAND_GET_SND_DEVICE:
            if (sndexWork.dest != NULL)
            {
                *((SNDEXDevice*)sndexWork.dest) =   (SNDEXDevice)param;
            }
            break;
        case SNDEX_PXI_COMMAND_PRESS_VOLSWITCH:
            /* ARM7����̉��ʒ����{�^�������ʒm */
            if (SNDEXi_VolumeSwitchCallbackInfo.callback != NULL)
            {
                (SNDEXi_VolumeSwitchCallbackInfo.callback)((SNDEXResult)result, SNDEXi_VolumeSwitchCallbackInfo.cbArg);
            }
            break;
        case SNDEX_PXI_COMMAND_HP_CONNECT:
            if (sndexWork.dest != NULL)
            {
                *((SNDEXHeadphone*)sndexWork.dest) =   (SNDEXHeadphone)param;
            }
            break;
        }
        ReplyCallback(SNDEX_RESULT_SUCCESS);
        break;
    case SNDEX_PXI_RESULT_INVALID_PARAM:
        ReplyCallback(SNDEX_RESULT_INVALID_PARAM);
        break;
    case SNDEX_PXI_RESULT_EXCLUSIVE:
        ReplyCallback(SNDEX_RESULT_EXCLUSIVE);
        break;
    case SNDEX_PXI_RESULT_ILLEGAL_STATE:
        ReplyCallback(SNDEX_RESULT_ILLEGAL_STATE);
        break;
    case SNDEX_PXI_RESULT_DEVICE_ERROR:
        if (command == SNDEX_PXI_COMMAND_GET_VOLUME)
        {
            if (sndexWork.dest != NULL)
            {
                *((u8*)sndexWork.dest)  =   SNDEX_VOLUME_MIN;
            }
        }
        ReplyCallback(SNDEX_RESULT_DEVICE_ERROR);
        break;
    case SNDEX_PXI_RESULT_INVALID_COMMAND:
    default:
        ReplyCallback(SNDEX_RESULT_FATAL_ERROR);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         SyncCallback

  Description:  �����֐�����p�̋��ʃR�[���o�b�N�֐��B

  Arguments:    result  -   �񓯊������̏������ʁB
                arg     -   �����֐�����M��҂��ău���b�N���Ă��郁�b�Z�[�W
                            �L���[�̐���\���̂ւ̃|�C���^�B

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
SyncCallback (SNDEXResult result, void* arg)
{
    SDK_NULL_ASSERT(arg);

    if (FALSE == OS_SendMessage((OSMessageQueue*)arg, (OSMessage)result, OS_MESSAGE_NOBLOCK))
    {
        /* ���W�b�N��͒ʂ蓾�Ȃ����A�O�̂��ߌx�����o�� */
        OS_TWarning("SNDEX: Temporary message queue is full.\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         SendCommand

  Description:  PXI �o�R�� ARM7 �ɃT�E���h�g���@�\����R�}���h�𔭍s����B

  Arguments:    command -   �R�}���h��ʁB
                param   -   �R�}���h�ɕt������p�����[�^�B

  Returns:      BOOL    -   PXI ���M�ɐ��������ꍇ�� TRUE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
SendCommand (u8 command, u8 param)
{
    OSIntrMode  e   =   OS_DisableInterrupts();
    u32     packet  =   (u32)(((command << SNDEX_PXI_COMMAND_SHIFT) & SNDEX_PXI_COMMAND_MASK)
                        | ((param << SNDEX_PXI_PARAMETER_SHIFT) & SNDEX_PXI_PARAMETER_MASK));

    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_SNDEX, packet, 0))
    {
        /* ���M�Ɏ��s�����ꍇ�́A�r����������� */
        sndexState  =   SNDEX_STATE_INITIALIZED;
        (void)OS_RestoreInterrupts(e);
        SNDEXi_Warning("SNDEX: Failed to send PXI command.\n");
        return FALSE;
    }
    
    (void)OS_RestoreInterrupts(e);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SendCommand

  Description:  PXI �o�R�� ARM7 �ɃT�E���h�g���@�\����R�}���h�𔭍s����B

  Arguments:    command -   �R�}���h��ʁB
                param   -   �R�}���h�ɕt������p�����[�^(IirFilter �̃p�����[�^�̂��߂� 16�r�b�g�ɂ��Ă���j

  Returns:      BOOL    -   PXI ���M�ɐ��������ꍇ�� TRUE ��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL
SendCommandEx (u8 command, u16 param)
{
    OSIntrMode  e   =   OS_DisableInterrupts();
    u32     packet  =   (u32)(((command << SNDEX_PXI_COMMAND_SHIFT) & SNDEX_PXI_COMMAND_MASK)
                        | ((param << SNDEX_PXI_PARAMETER_SHIFT) & SNDEX_PXI_PARAMETER_MASK_IIR));

    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_SNDEX, packet, 0))
    {
        /* ���M�Ɏ��s�����ꍇ�́A�r����������� */
        sndexState  =  SNDEX_STATE_INITIALIZED;
        (void)OS_RestoreInterrupts(e);
        SNDEXi_Warning("SNDEX: Failed to send PXI command.\n");
        return FALSE;
    }
    if( command == SNDEX_PXI_COMMAND_SET_IIRFILTER )
    {
        isLockSpi = TRUE;
    }
    
    (void)OS_RestoreInterrupts(e);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CheckState

  Description:  ������Ԃ��񓯊������J�n�ł����Ԃł��邩�𒲂ׁA
                �ł���Ȃ�� SNDEX ���C�u���������b�N����B

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static SNDEXResult
CheckState (void)
{
    OSIntrMode  e   =   OS_DisableInterrupts();

    /* ������Ԋm�F */
    switch (sndexState)
    {
    case SNDEX_STATE_BEFORE_INIT:
    case SNDEX_STATE_INITIALIZING:
        (void)OS_RestoreInterrupts(e);
        SNDEXi_Warning("SNDEX: Library is not initialized yet.\n");
        return SNDEX_RESULT_BEFORE_INIT;
    case SNDEX_STATE_LOCKED:
    case SNDEX_STATE_PLAY_SHUTTER:
    case SNDEX_STATE_POST_PROC_SHUTTER:
        (void)OS_RestoreInterrupts(e);
        SNDEXi_Warning("SNDEX: Another request is in progress.\n");
        return SNDEX_RESULT_EXCLUSIVE;
    }

    /* �r�����䒆��ԂɈڍs */
    sndexState  =   SNDEX_STATE_LOCKED;
    (void)OS_RestoreInterrupts(e);
    return SNDEX_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         ReplyCallback

  Description:  �񓯊������̊�����ʒm����B������Ԃ̍X�V�A�r����Ԃ̉�����
                ���킹�čs���B

  Arguments:    result  -   �R�[���o�b�N�֐��ɒʒm����񓯊��������ʁB

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
ReplyCallback (SNDEXResult result)
{
    OSIntrMode      e           =   OS_DisableInterrupts();
    SNDEXCallback   callback    =   sndexWork.callback;
    void*           cbArg       =   sndexWork.cbArg;
    u8              command     =   sndexWork.command;
    
    // SetIirFilterAsync �ɂ�� SPI_Lock ���I������Ƃ݂Ȃ��ăt���O�����낷
    if (sndexWork.command == SNDEX_PXI_COMMAND_SET_IIRFILTER)
    {
        isLockSpi = FALSE;
        isIirFilterSetting = FALSE;
    }
    
    /* ��ԃ��Z�b�g */
    SetSndexWork(NULL, NULL, NULL, 0);
    // �V���b�^�[���Đ��O���������s����ƁA�㏈���̎��s�܂Ń��b�N���������Ȃ�
    if (command == SNDEX_PXI_COMMAND_PRE_PROC_SHUTTER)
    {
        // �V���b�^�[���Đ��㏈�����s�� SNDEXi_PostProcessForShutterSound() �ȊO��
        // ���b�N��Ԃƌ��Ȃ��Ĕr�������B
        sndexState          =   SNDEX_STATE_PLAY_SHUTTER;
    }
    else
    {
        sndexState          =   SNDEX_STATE_INITIALIZED;
    }
    (void)OS_RestoreInterrupts(e);

    /* �o�^����Ă���R�[���o�b�N�֐��Ăяo�� */
    if (callback != NULL)
    {
        callback(result, cbArg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SetSndexWork

  Description:  sndexWork �Ƀp�����[�^���Z�b�g

  Arguments:    cb : �R�[���o�b�N�֐��ւ̃|�C���^
                cbarg : cb �֓n������
                dst : �R�}���h�̏������ʂ��i�[����ϐ��ւ̃|�C���^
                command : ARM7 �֑��M����R�}���h

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
SetSndexWork    (SNDEXCallback cb, void* cbarg, void* dst, u8 command)
{
    sndexWork.callback = cb;
    sndexWork.cbArg = cbarg;
    sndexWork.dest = dst;
    sndexWork.command = command;
}

/*---------------------------------------------------------------------------*
  Name:         SndexSleepAndExitCallback

  Description:  �n�[�h�E�F�A���Z�b�g�E�V���b�g�_�E���A�܂��̓X���[�v����
                �Ă΂��R�[���o�b�N�֐�

  Arguments:    arg

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
SndexSleepAndExitCallback (void *arg)
{
#pragma unused( arg )
    // IIR �t�B���^�������s���́ASNDEX ���ő��� SNDEX API ���r������Ă��܂�����
    // �I����K����ɑ҂���
    WaitIirFilterSetting();
    
    ResetTempVolume();
}

/*---------------------------------------------------------------------------*
  Name:         WaitIirFilterSetting

  Description:  SNDEX_SetIirFilter[Async] ���I������܂ő҂�

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
WaitIirFilterSetting (void)
{
    // SNDEX_SetIirFilter[Async]���ōs���Ă��� SPI_Lock ���I������܂ő҂�������
    while (isLockSpi)
    {
        OS_SpinWait(67 * 1000); //�� 1ms
        PXIi_HandlerRecvFifoNotEmpty();
    }
}

static void 
WaitResetSoundCallback(SNDEXResult result, void* arg)
{
    static u32 i = 0;    // ���g���C��5��܂łƂ���
#pragma unused(arg)
    if (result != SNDEX_RESULT_SUCCESS && i < 5)
    {
        (void)SNDEXi_SetVolumeExAsync(storeVolume, WaitResetSoundCallback, NULL);
        i++;
        return;
    }
    isStoreVolume = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         ResetTempVolume

  Description:  SNDEXi_SetIgnoreHWVolume �ňꎞ�ۑ����ꂽ���ʂ֖߂�

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
ResetTempVolume (void)
{
    if (isStoreVolume)
    {
        static i = 0;    // ���g���C��5��܂łƂ���
        // SNDEXi_SetIgnoreHWVolume �ňꎞ�ۑ�����Ă������ʂɖ߂�
        // �񓯊��֐��̎��s����������܂ŌJ��Ԃ�
        // ���ʕύX�̐��ۂɂ��Ẵ��g���C�̓R�[���o�b�N���ōs��
        while( SNDEX_RESULT_SUCCESS != SNDEXi_SetVolumeExAsync(storeVolume, WaitResetSoundCallback, NULL) && i < 5)
        {
            i++;
        }
        while (isStoreVolume)
        {
            OS_SpinWait(67 * 1000); //�� 1ms
            PXIi_HandlerRecvFifoNotEmpty();
        }
    }
}

