/*---------------------------------------------------------------------------*
  Project:  TwlSDK - snd - include
  File:     sndex_common.h

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
#ifndef TWL_SND_COMMON_SNDEX_COMMON_H_
#define TWL_SND_COMMON_SNDEX_COMMON_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
/* PXI �R�}���h�t�H�[�}�b�g */
#define     SNDEX_PXI_COMMAND_MASK              0x00ff0000
#define     SNDEX_PXI_COMMAND_SHIFT             16
#define     SNDEX_PXI_RESULT_MASK               0x0000ff00
#define     SNDEX_PXI_RESULT_SHIFT              8
#define     SNDEX_PXI_PARAMETER_MASK            0x000000ff
#define     SNDEX_PXI_PARAMETER_SHIFT           0
#define     SNDEX_PXI_PARAMETER_MASK_IIR        0x0000ffff
#define     SNDEX_PXI_PARAMETER_SHIFT_IIR       0

// ���ʂ̐ݒ�E�擾�Ɋւ���p�����[�^�̃}�X�N�ꗗ
#define     SNDEX_PXI_PARAMETER_MASK_VOL_VALUE  0x1f	// �X�s�[�J�[����
#define     SNDEX_PXI_PARAMETER_MASK_VOL_KEEP   0x80	// �ύX�\�񂳂ꂽ�X�s�[�J�[���ʂۗ̕��l
#define     SNDEX_PXI_PARAMETER_SHIFT_VOL_KEEP  7
#define     SNDEX_PXI_PARAMETER_MASK_VOL_8LV    0x40	// �X�s�[�J�[���ʁi8�i�K�j
#define     SNDEX_PXI_PARAMETER_SHIFT_VOL_8LV   6

/* PXI �R�}���h��� */
#define     SNDEX_PXI_COMMAND_GET_SMIX_MUTE           0x01    // ������Ԏ擾
#define     SNDEX_PXI_COMMAND_GET_SMIX_FREQ           0x02    // I2S ���g���擾
#define     SNDEX_PXI_COMMAND_GET_SMIX_DSP            0x03    // CPU/DSP �o�͍�����擾
#define     SNDEX_PXI_COMMAND_GET_VOLUME              0x04    // ���ʎ擾
#define     SNDEX_PXI_COMMAND_GET_SND_DEVICE          0x05    // �����o�̓f�o�C�X�ݒ�擾
#define     SNDEX_PXI_COMMAND_SET_SMIX_MUTE           0x81    // ������ԕύX
#define     SNDEX_PXI_COMMAND_SET_SMIX_FREQ           0x82    // I2S ���g���ύX
#define     SNDEX_PXI_COMMAND_SET_SMIX_DSP            0x83    // CPU/DSP �o�͍�����ύX
#define     SNDEX_PXI_COMMAND_SET_VOLUME              0x84    // ���ʕύX
#define     SNDEX_PXI_COMMAND_SET_SND_DEVICE          0x85    // �����o�̓f�o�C�X�ݒ�ύX
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER_TARGET    0xc1    // IIR�t�B���^��������Ώۂ̒ʒm
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N0  0xc2    // IIR�t�B���^�p�����[�^�̒ʒm
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N1  0xc3    // IIR�t�B���^�p�����[�^�̒ʒm
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_N2  0xc4    // IIR�t�B���^�p�����[�^�̒ʒm
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D1  0xc5    // IIR�t�B���^�p�����[�^�̒ʒm
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER_PARAM_D2  0xc6    // IIR�t�B���^�p�����[�^�̒ʒm
#define     SNDEX_PXI_COMMAND_SET_IIRFILTER           0xc7    // IIR�t�B���^�̓o�^
#define     SNDEX_PXI_COMMAND_PRE_PROC_SHUTTER        0xfb    // �V���b�^�[���Đ��O����
#define     SNDEX_PXI_COMMAND_POST_PROC_SHUTTER       0xfc    // �V���b�^�[���Đ��㏈��
#define     SNDEX_PXI_COMMAND_PRESS_VOLSWITCH         0xfd    // ���ʒ����{�^�������̒ʒm
#define     SNDEX_PXI_COMMAND_HP_CONNECT              0xfe    // �w�b�h�t�H���ڑ��L��

/* PXI �R�}���h�������ʎ�� */
#define     SNDEX_PXI_RESULT_SUCCESS            0x00    // ����
#define     SNDEX_PXI_RESULT_INVALID_COMMAND    0x01    // �s���ȃR�}���h
#define     SNDEX_PXI_RESULT_INVALID_PARAM      0x02    // �s���ȃp�����[�^
#define     SNDEX_PXI_RESULT_EXCLUSIVE          0x03    // �r�����䒆
#define     SNDEX_PXI_RESULT_ILLEGAL_STATE      0x04    // �����ł��Ȃ���� (�r������ȊO)
#define     SNDEX_PXI_RESULT_DEVICE_ERROR       0x05    // �f�o�C�X����Ɏ��s
#define     SNDEX_PXI_RESULT_FATAL_ERROR        0xff    // ���������Ȃ��v���I�ȃG���[

/* �����o�̓~���[�g�ݒ�l�ꗗ */
#define     SNDEX_SMIX_MUTE_OFF                 0x00    // ����
#define     SNDEX_SMIX_MUTE_ON                  0x01    // ����

/* I2S ���g���ݒ�l�ꗗ */
#define     SNDEX_SMIX_FREQ_32730               0x00    // 32.73 kHz
#define     SNDEX_SMIX_FREQ_47610               0x01    // 47.61 kHz

/* �����o�̓f�o�C�X�ݒ�l�ꗗ */
#define     SNDEX_MCU_DEVICE_AUTO               0x00    // �󋵂ɉ����ďo�͐�������I��
#define     SNDEX_MCU_DEVICE_SPEAKER            0x01    // �X�s�[�J�ɂ̂ݏo��
#define     SNDEX_MCU_DEVICE_HEADPHONE          0x02    // �w�b�h�t�H���ɂ̂ݏo��
#define     SNDEX_MCU_DEVICE_BOTH               0x03    // �X�s�[�J�E�w�b�h�t�H���̗����ɏo��

/* ���������X�s�[�J�o�͐ݒ�l�ꗗ */
#define     SNDEX_FORCEOUT_OFF                  0x00    // �ʏ���
#define     SNDEX_FORCEOUT_CAMERA_SHUTTER_ON    0x01    // �J�����B�e�p�����o�͏��

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   // extern "C"
#endif
#endif  // TWL_SND_COMMON_SNDEX_COMMON_H_
