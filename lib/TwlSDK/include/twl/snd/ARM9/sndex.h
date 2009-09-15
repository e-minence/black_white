/*---------------------------------------------------------------------------*
  Project:  TwlSDK - snd - include
  File:     sndex.h

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef TWL_SND_ARM9_SNDEX_H_
#define TWL_SND_ARM9_SNDEX_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

#include <twl/snd/common/sndex_common.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
/* �������� */
typedef enum SNDEXResult
{
    SNDEX_RESULT_SUCCESS        =   0,
    SNDEX_RESULT_BEFORE_INIT    =   1,
    SNDEX_RESULT_INVALID_PARAM  =   2,
    SNDEX_RESULT_EXCLUSIVE      =   3,
    SNDEX_RESULT_ILLEGAL_STATE  =   4,
    SNDEX_RESULT_PXI_SEND_ERROR =   5,
    SNDEX_RESULT_DEVICE_ERROR   =   6,
    SNDEX_RESULT_FATAL_ERROR    =   7,
    SNDEX_RESULT_MAX

} SNDEXResult;

/* �����ݒ� */
typedef enum SNDEXMute
{
    SNDEX_MUTE_OFF  =   0,
    SNDEX_MUTE_ON   =   1,
    SNDEX_MUTE_MAX

} SNDEXMute;

/* I2S ���g���ݒ� */
typedef enum SNDEXFrequency
{
    SNDEX_FREQUENCY_32730   =   0,
    SNDEX_FREQUENCY_47610   =   1,
    SNDEX_FREQUENCY_MAX

} SNDEXFrequency;

/* IIR�t�B���^�p�����[�^�\���� */
typedef struct _SNDEXIirFilterParam
{
    u16 n0;
    u16 n1;
    u16 n2;
    u16 d1;
    u16 d2;
} SNDEXIirFilterParam;

/* IIR �t�B���^�̃^�[�Q�b�g�񋓌^ */
typedef enum _SNDEXIirFilterTarget
{
    SNDEX_IIR_FILTER_ADC_1 = 0,
    SNDEX_IIR_FILTER_ADC_2,
    SNDEX_IIR_FILTER_ADC_3,
    SNDEX_IIR_FILTER_ADC_4,
    SNDEX_IIR_FILTER_ADC_5,
    SNDEX_IIR_FILTER_DAC_LEFT_1,
    SNDEX_IIR_FILTER_DAC_LEFT_2,
    SNDEX_IIR_FILTER_DAC_LEFT_3,
    SNDEX_IIR_FILTER_DAC_LEFT_4,
    SNDEX_IIR_FILTER_DAC_LEFT_5,
    SNDEX_IIR_FILTER_DAC_RIGHT_1,
    SNDEX_IIR_FILTER_DAC_RIGHT_2,
    SNDEX_IIR_FILTER_DAC_RIGHT_3,
    SNDEX_IIR_FILTER_DAC_RIGHT_4,
    SNDEX_IIR_FILTER_DAC_RIGHT_5,
    SNDEX_IIR_FILTER_DAC_BOTH_1,
    SNDEX_IIR_FILTER_DAC_BOTH_2,
    SNDEX_IIR_FILTER_DAC_BOTH_3,
    SNDEX_IIR_FILTER_DAC_BOTH_4,
    SNDEX_IIR_FILTER_DAC_BOTH_5,
    SNDEX_IIR_FILTER_TARGET_MAX
} SNDEXIirFilterTarget;

/* �w�b�h�t�H���ڑ��L�� */
typedef enum SNDEXHeadphone
{
    SNDEX_HEADPHONE_UNCONNECT   =   0,
    SNDEX_HEADPHONE_CONNECT     =   1,
    SNDEX_HEADPHONE_MAX
} SNDEXHeadphone;

/* DSP ������ݒ� */
#define SNDEX_DSP_MIX_RATE_MIN  0
#define SNDEX_DSP_MIX_RATE_MAX  8

/* ���ʐݒ� */
#define SNDEX_VOLUME_MIN        0
#define SNDEX_VOLUME_MAX        7
#define SNDEX_VOLUME_MAX_EX     31

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
/* �R�[���o�b�N�֐��^��` */
typedef void    (*SNDEXCallback)    (SNDEXResult result, void* arg);

#define SNDEXVolumeSwitchCallbackInfo SNDEXWork

/*---------------------------------------------------------------------------*
    ����J�֐���`
 *---------------------------------------------------------------------------*/
void    SNDEXi_Init (void);

SNDEXResult SNDEXi_GetMuteAsync            (SNDEXMute* mute, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_GetMute                 (SNDEXMute* mute);
SNDEXResult SNDEXi_GetI2SFrequencyAsync    (SNDEXFrequency* freq, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_GetI2SFrequency         (SNDEXFrequency* freq);
SNDEXResult SNDEXi_GetDSPMixRateAsync      (u8* rate, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_GetDSPMixRate           (u8* rate);
SNDEXResult SNDEXi_GetVolumeAsync          (u8* volume, SNDEXCallback callback, void* arg, BOOL eightlv, BOOL keep);
SNDEXResult SNDEXi_GetVolume               (u8* volume, BOOL eightlv, BOOL keep);
SNDEXResult SNDEXi_GetVolumeExAsync        (u8* volume, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_GetVolumeEx             (u8* volume);
SNDEXResult SNDEXi_GetCurrentVolumeExAsync (u8* volume, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_GetCurrentVolumeEx      (u8* volume);

SNDEXResult SNDEXi_SetMuteAsync         (SNDEXMute mute, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_SetMute              (SNDEXMute mute);
SNDEXResult SNDEXi_SetI2SFrequencyAsync (SNDEXFrequency freq, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_SetI2SFrequency      (SNDEXFrequency freq);
SNDEXResult SNDEXi_SetDSPMixRateAsync   (u8 rate, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_SetDSPMixRate        (u8 rate);
SNDEXResult SNDEXi_SetVolumeAsync       (u8 volume, SNDEXCallback callback, void* arg, BOOL eightlv);
SNDEXResult SNDEXi_SetVolume            (u8 volume, BOOL eightlv);
SNDEXResult SNDEXi_SetVolumeExAsync     (u8 volume, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_SetVolumeEx          (u8 volume);

SNDEXResult SNDEXi_SetIirFilterAsync    (SNDEXIirFilterTarget target, const SNDEXIirFilterParam* pParam, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_SetIirFilter         (SNDEXIirFilterTarget target, const SNDEXIirFilterParam* pParam);

SNDEXResult SNDEXi_IsConnectedHeadphoneAsync (SNDEXHeadphone *hp, SNDEXCallback callback, void* arg);
SNDEXResult SNDEXi_IsConnectedHeadphone      (SNDEXHeadphone *hp);

void        SNDEXi_SetVolumeSwitchCallback   (SNDEXCallback callback, void* arg);

SNDEXResult SNDEXi_SetIgnoreHWVolume       (u8 volume, BOOL eightlv);
SNDEXResult SNDEXi_ResetIgnoreHWVolume     (void);

SNDEXResult SNDEXi_PreProcessForShutterSound  (void);
SNDEXResult SNDEXi_PostProcessForShutterSound (SNDEXCallback callback, void* arg);

/*---------------------------------------------------------------------------*
  Name:         SNDEX_Init

  Description:  �T�E���h�g���@�\���C�u����������������B

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void
SNDEX_Init (void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        SNDEXi_Init();
    }
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetMuteAsync

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
static inline SNDEXResult
SNDEX_GetMuteAsync (SNDEXMute* mute, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetMuteAsync(mute, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetMute

  Description:  �����o�͂̏����ݒ��Ԃ��擾����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    mute    -   �����ݒ��Ԃ��擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_GetMute (SNDEXMute* mute)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetMute(mute) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetI2SFrequencyAsync

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
static inline SNDEXResult
SNDEX_GetI2SFrequencyAsync (SNDEXFrequency* freq, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetI2SFrequencyAsync(freq, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetI2SFrequency

  Description:  I2S ���g�������擾����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    freq    -   ���g�������擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_GetI2SFrequency (SNDEXFrequency* freq)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetI2SFrequency(freq) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetDSPMixRateAsync

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
static inline SNDEXResult
SNDEX_GetDSPMixRateAsync (u8* rate, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetDSPMixRateAsync(rate, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetDSPMixRate

  Description:  CPU �� DSP �̉����o�͍���������擾����B
                ������͍ŏ�: 0 (DSP 100%), �ő�: 8 (CPU 100%)�B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    rate    -   ����������擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_GetDSPMixRate (u8* rate)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetDSPMixRate(rate) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetVolumeAsync

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
static inline SNDEXResult
SNDEX_GetVolumeAsync (u8* volume, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetVolumeAsync(volume, callback, arg, TRUE, TRUE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetCurrentVolumeAsync

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
                                �������A���̊֐��Ŏ擾�ł��鉹�ʂ͎��ۂ̐ݒ�l�ł���A
                                SNDEX_SetVolume[Async] �ɂ�肢����X�V�����ۗ��l�ł͂Ȃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_GetCurrentVolumeAsync (u8* volume, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetVolumeAsync(volume, callback, arg, TRUE, FALSE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetVolume

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʏ����擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_GetVolume (u8* volume)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetVolume(volume, TRUE, TRUE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_GetCurrentVolume

  Description:  �����o�͉��ʏ����擾����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʏ����擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
                                �������A���̊֐��Ŏ擾�ł��鉹�ʂ͎��ۂ̐ݒ�l�ł���A
                                SNDEX_SetVolume[Async] �ɂ�肢����X�V�����ۗ��l�ł͂Ȃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_GetCurrentVolume (u8* volume)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_GetVolume(volume, TRUE, FALSE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetMuteAsync

  Description:  �����o�͂̏����ݒ��Ԃ�ύX����B

  Arguments:    mute        -   �����ݒ��Ԃ��w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetMuteAsync (SNDEXMute mute, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetMuteAsync(mute, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetMute

  Description:  �����o�͂̏����ݒ��Ԃ�ύX����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    mute    -   �����ݒ��Ԃ��w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetMute (SNDEXMute mute)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetMute(mute) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetI2SFrequencyAsync

  Description:  I2S ���g����ύX����B

  Arguments:    freq        -   ���g�����w�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetI2SFrequencyAsync (SNDEXFrequency freq, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetI2SFrequencyAsync(freq, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetI2SFrequency

  Description:  I2S ���g����ύX����B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    freq    -   ���g�����w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetI2SFrequency (SNDEXFrequency freq)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetI2SFrequency(freq) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetDSPMixRateAsync

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
static inline SNDEXResult
SNDEX_SetDSPMixRateAsync (u8 rate, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetDSPMixRateAsync(rate, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetDSPMixRate

  Description:  CPU �� DSP �̉����o�͍������ύX����B
                ������͍ŏ�: 0 (DSP 100%), �ő�: 8 (CPU 100%)�B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    rate    -   ������� 0 ���� 8 �̐��l�Ŏw�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetDSPMixRate (u8 rate)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetDSPMixRate(rate) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetVolumeAsync

  Description:  �����o�͉��ʂ�ύX����B���ʂ͍ŏ�: 0, �ő�: 31 �B

  Arguments:    volume      -   ���ʂ� 0 ���� 31 �̐��l�Ŏw�肷��B
                callback    -   �񓯊������������Ɍ��ʂ�ʒm����R�[���o�b�N
                                �֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetVolumeAsync (u8 volume, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetVolumeAsync(volume, callback, arg, TRUE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetVolume

  Description:  �����o�͉��ʂ�ύX����B���ʂ͍ŏ�: 0, �ő�: 31 �B
                �����֐��Ȃ̂ŁA�����݃n���h��������̌Ăяo���͋֎~�B

  Arguments:    volume  -   ���ʂ� 0 ���� 31 �̐��l�Ŏw�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetVolume (u8 volume)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetVolume(volume, TRUE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetIirFilterAsync

  Description:  IIR�t�B���^�[(Biquad)�̃p�����[�^�ݒ��񓯊��ōs���܂��B

  Arguments:    target   : IIR�t�B���^��������Ώ� SNDEXIirFilterTarget�񋓑�
                pParam   : IIR�t�B���^�̌W�� SNDEXIirFilterParam�\����
                callback : IIR�t�B���^�ݒ��ɌĂ΂��R�[���o�b�N�֐�
                arg      : �R�[���o�b�N�֐��֓n������

  Returns:      ����I������ SNDEX_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetIirFilterAsync(SNDEXIirFilterTarget target, const SNDEXIirFilterParam* pParam, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetIirFilterAsync(target, pParam, callback, arg) : SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetIirFilter

  Description:  IIR�t�B���^�[(Biquad)�̃p�����[�^�ݒ���s���܂��B

  Arguments:    target   : IIR�t�B���^��������Ώ� SNDEXIirFilterTarget�񋓑�
                pParam   : IIR�t�B���^�̌W�� SNDEXIirFilterParam�\����

  Returns:      ����I������ SNDEX_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetIirFilter(SNDEXIirFilterTarget target, const SNDEXIirFilterParam* pParam)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetIirFilter(target, pParam) : SNDEX_RESULT_ILLEGAL_STATE);
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
static inline SNDEXResult
SNDEX_IsConnectedHeadphoneAsync (SNDEXHeadphone *hp, SNDEXCallback callback, void* arg)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_IsConnectedHeadphoneAsync(hp, callback, arg) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_IsConnectedHeadphone

  Description:  �w�b�h�t�H���̐ڑ��̗L�����擾����B

  Arguments:    hp          -   �w�b�h�t�H���̐ڑ��󋵂��擾����o�b�t�@���w�肷��B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��BSNDEX_RESULT_SUCCESS ��Ԃ�
                                �ꍇ�́A�񓯊�����������Ɏ��ۂ̏������ʂ�
                                �R�[���o�b�N�ɓo�^�����֐��ɒʒm�����B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_IsConnectedHeadphone (SNDEXHeadphone *hp)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_IsConnectedHeadphone(hp) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetVolumeSwitchCallback

  Description:  ���ʒ����{�^���������ɌĂ΂��R�[���o�b�N�֐���ݒ肷��B

  Arguments:    callback    -   ���ʒ����{�^���������ɌĂ΂��R�[���o�b�N�֐����w�肷��B
                arg         -   �R�[���o�b�N�֐��ɓn���p�����[�^���w�肷��B

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static inline void
SNDEX_SetVolumeSwitchCallback (SNDEXCallback callback, void* arg)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        SNDEXi_SetVolumeSwitchCallback(callback, arg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_SetIgnoreHWVolume

  Description:  ���v�̃A���[�����ȂǁA���̂Ƃ��̖{�̉��ʂ̒l�ɂƂ��ꂸ
                �w�肵�����ʂŉ���炷�K�v������ꍇ�A���[�U�̑���ɂ�炸
                ���ʕύX���s��˂΂Ȃ�Ȃ��B
                �{�֐��ł́A���ʕύX���s�������łȂ��A�ύX�O�̉��ʂ�ۑ�����B
                �܂��A�A�v���̈Ӑ}���Ȃ��^�C�~���O�Ŗ{�̃��Z�b�g�E�V���b�g�_�E����
                �s��ꂽ�ۂɌ��̉��ʂɖ߂��I���R�[���o�b�N�֐���o�^����B
                SNDEX_ResetIgnoreHWVolume ���ĂԂ��ƂŁA���̉��ʂɖ߂��Ƌ���
                �o�^�����I���R�[���o�b�N�֐����폜����B

  Arguments:    volume      -   �ύX���������ʒl�B

  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_SetIgnoreHWVolume (u8 volume)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_SetIgnoreHWVolume(volume, TRUE) :
            SNDEX_RESULT_ILLEGAL_STATE);
}

/*---------------------------------------------------------------------------*
  Name:         SNDEX_ResetIgnoreHWVolume

  Description:  SNDEX_SetIgnoreHWVolume �ŕύX�������ʂ��A�ύX�O�̉��ʂ֖߂��B
                �����ɓo�^���Ă����I���R�[���o�b�N�֐����폜����B

  Arguments:    �Ȃ�
  
  Returns:      SNDEXResult -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
static inline SNDEXResult
SNDEX_ResetIgnoreHWVolume (void)
{
    return (SNDEXResult)((OS_IsRunOnTwl() == TRUE) ?
            SNDEXi_ResetIgnoreHWVolume() :
            SNDEX_RESULT_ILLEGAL_STATE);
}
/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   // extern "C"
#endif
#endif  // TWL_SND_ARM9_SNDEX_H_
