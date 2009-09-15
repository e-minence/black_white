/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SPI - include
  File:     api.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_SPI_ARM9_MIC_H_
#define NITRO_SPI_ARM9_MIC_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/spi/common/type.h>
#include    <nitro/pxi.h>


/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
// �������ʒ�`
typedef enum MICResult
{
    MIC_RESULT_SUCCESS = 0,            // ����
    MIC_RESULT_BUSY,                   // �r�����䒆
    MIC_RESULT_ILLEGAL_PARAMETER,      // �s���ȃp�����[�^
    MIC_RESULT_SEND_ERROR,             // PXI�ɂ�鑗�M�Ɏ��s
    MIC_RESULT_INVALID_COMMAND,        // �s���ȃR�}���h
    MIC_RESULT_ILLEGAL_STATUS,         // �������s�s�\�ȏ��
    MIC_RESULT_FATAL_ERROR,            // ��L�ȊO�̃G���[
    MIC_RESULT_MAX
}
MICResult;

// �T���v�����O��ʒ�`
typedef enum MICSamplingType
{
    MIC_SAMPLING_TYPE_8BIT = 0,        //  8�r�b�g�T���v�����O
    MIC_SAMPLING_TYPE_12BIT,           // 12�r�b�g�T���v�����O
    MIC_SAMPLING_TYPE_SIGNED_8BIT,     // �����t�� 8�r�b�g�T���v�����O
    MIC_SAMPLING_TYPE_SIGNED_12BIT,    // �����t��12�r�b�g�T���v�����O
    MIC_SAMPLING_TYPE_12BIT_FILTER_OFF,
    MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF,
    MIC_SAMPLING_TYPE_MAX
}
MICSamplingType;

// ��\�I�ȃT���v�����O������ARM7�̃N���b�N���Œ�`
typedef enum MICSamplingRate
{
    MIC_SAMPLING_RATE_8K = (HW_CPU_CLOCK_ARM7 / 8000),  // ��  8.0   kHz
    MIC_SAMPLING_RATE_11K = (HW_CPU_CLOCK_ARM7 / 11025),        // �� 11.025 kHz
    MIC_SAMPLING_RATE_16K = (HW_CPU_CLOCK_ARM7 / 16000),        // �� 16.0   kHz
    MIC_SAMPLING_RATE_22K = (HW_CPU_CLOCK_ARM7 / 22050),        // �� 22.05  kHz
    MIC_SAMPLING_RATE_32K = (HW_CPU_CLOCK_ARM7 / 32000),        // �� 32.0   kHz
    MIC_SAMPLING_RATE_LIMIT = 1024
}
MICSamplingRate;

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// �R�[���o�b�N�֐��^��`
typedef void (*MICCallback) (MICResult result, void *arg);

// �I�[�g�T���v�����O�p�ݒ��`
typedef struct MICAutoParam
{
    MICSamplingType type;              // �T���v�����O���
    void   *buffer;                    // ���ʊi�[�o�b�t�@�ւ̃|�C���^
    u32     size;                      // �o�b�t�@�T�C�Y
    u32     rate;                      // �T���v�����O����( ARM7�̃N���b�N�� )
    BOOL    loop_enable;               // �o�b�t�@�t�����̃��[�v��
    MICCallback full_callback;         // �o�b�t�@�t�����̃R�[���o�b�N
    void   *full_arg;                  // ��L�R�[���o�b�N�Ɏw�肷�����

}
MICAutoParam;


/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         MIC_Init

  Description:  �}�C�N���C�u����������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MIC_Init(void);

/*---------------------------------------------------------------------------*
  Name:         MIC_DoSamplingAsync

  Description:  �}�C�N�����񓯊��ɃT���v�����O����B

  Arguments:    type      - �T���v�����O��ʂ��w��B
                buf       - �T���v�����O�f�[�^���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_DoSamplingAsync(MICSamplingType type, void *buf, MICCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         MIC_StartAutoSamplingAsync

  Description:  �}�C�N�̎����T���v�����O��񓯊��ɊJ�n����B

  Arguments:    param     - �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StartAutoSamplingAsync(const MICAutoParam *param, MICCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         MIC_StopAutoSamplingAsync

  Description:  �}�C�N�̎����T���v�����O��񓯊��ɒ�~����B

  Arguments:    callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StopAutoSamplingAsync(MICCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustAutoSamplingAsync

  Description:  �}�C�N�̎����T���v�����O�ɂ�����T���v�����O���[�g��񓯊���
                ��������B

  Arguments:    rate         - �T���v�����O���[�g���w��B
                callback     - �񓯊����������������ۂɌĂяo���֐����w��B
                arg          - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      MICResult    - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_AdjustAutoSamplingAsync(u32 rate, MICCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         MIC_GetLastSamplingAddress

  Description:  �}�C�N�̍ŐV�T���v�����O���ʊi�[�A�h���X���擾����B

  Arguments:    None.

  Returns:      void* - �T���v�����O���ʊi�[�A�h���X��Ԃ��B
                        �܂��T���v�����O���Ă��Ȃ��ꍇ��NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
void   *MIC_GetLastSamplingAddress(void);

/*---------------------------------------------------------------------------*
  Name:         MIC_DoSampling

  Description:  �}�C�N�����T���v�����O����B

  Arguments:    type      - �T���v�����O��ʂ��w��B
                buf       - �T���v�����O�f�[�^���i�[����o�b�t�@���w��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_DoSampling(MICSamplingType type, void *buf);

/*---------------------------------------------------------------------------*
  Name:         MIC_StartAutoSampling

  Description:  �}�C�N�̎����T���v�����O���J�n����B

  Arguments:    param     - �����T���v�����O�ݒ���\���̂ւ̃|�C���^�Ŏw��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StartAutoSampling(const MICAutoParam *param);

/*---------------------------------------------------------------------------*
  Name:         MIC_StopAutoSampling

  Description:  �}�C�N�̎����T���v�����O���~����B
                �����T���v�����O�J�n���Ƀ��[�v�w�肵�Ȃ������ꍇ�̓o�b�t�@��
                ���܂������_�Ŏ����I�ɃT���v�����O�͒�~�����B

  Arguments:    None.

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_StopAutoSampling(void);

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustAutoSampling

  Description:  �}�C�N�̎����T���v�����O�ɂ�����T���v�����O���[�g�𒲐�����B

  Arguments:    rate      - �T���v�����O���[�g���w��B

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult MIC_AdjustAutoSampling(u32 rate);

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
MICResult   MIC_StartLimitedSamplingAsync(const MICAutoParam* param, MICCallback callback, void* arg);

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
MICResult   MIC_StopLimitedSamplingAsync(MICCallback callback, void* arg);

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
MICResult   MIC_AdjustLimitedSamplingAsync(u32 rate, MICCallback callback, void* arg);

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
MICResult   MIC_StartLimitedSampling(const MICAutoParam* param);

/*---------------------------------------------------------------------------*
  Name:         MIC_StopLimitedSampling

  Description:  �T���v�����O���[�g����̃}�C�N�̎����T���v�����O���~����B
                �����֐��Ȃ̂ŁA���荞�݃n���h��������̌Ăяo���͋֎~�B
                �Ȃ��ANITRO ��ŌĂяo�����ꍇ�ɂ͊����� CPU �ɂ�鎩���T���v
                �����O��~�֐��ő�ւ����B

  Arguments:    None.

  Returns:      MICResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
MICResult   MIC_StopLimitedSampling(void);

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
MICResult   MIC_AdjustLimitedSampling(u32 rate);

#endif

/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_RTC_ARM9_API_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
