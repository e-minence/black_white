/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SPI - TP
  File:     tp.h
  
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

#ifndef NITRO_SPI_ARM9_TP_H_
#define NITRO_SPI_ARM9_TP_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif


#define     TP_SAMPLING_FREQUENCY_MAX   16      // �T���v�����O�p�x�̌��x

// �^�b�`�p�l���̐ڐG����
#define     TP_TOUCH_OFF                0       // �G��Ă��Ȃ�
#define     TP_TOUCH_ON                 1       // �G��Ă���

// �^�b�`�p�l���̃f�[�^�L��������
#define     TP_VALIDITY_VALID           0       // �L��
#define     TP_VALIDITY_INVALID_X       1       // X���W�������ȃf�[�^
#define     TP_VALIDITY_INVALID_Y       2       // Y���W�������ȃf�[�^
#define     TP_VALIDITY_INVALID_XY      (TP_VALIDITY_INVALID_X | TP_VALIDITY_INVALID_Y) // XY���W���ɖ����ȃf�[�^

// �^�b�`�p�l���֔��s���閽�ߎ��
typedef enum
{
    TP_REQUEST_COMMAND_SAMPLING = 0x0, // �T���v�����O���P����s
    TP_REQUEST_COMMAND_AUTO_ON = 0x1,  // �I�[�g�T���v�����O�J�n
    TP_REQUEST_COMMAND_AUTO_OFF = 0x2, // �I�[�g�T���v�����O��~
    TP_REQUEST_COMMAND_SET_STABILITY = 0x3,     // �`���^�����O�΍��l�̐ݒ�
    TP_REQUEST_COMMAND_AUTO_SAMPLING = 0x10     // �I�[�g�T���v�����O�̌��ʎ�M
}
TPRequestCommand;

// �^�b�`�p�l���ւ̃R�}���h�t���O
typedef enum
{
    TP_REQUEST_COMMAND_FLAG_SAMPLING = 1 << TP_REQUEST_COMMAND_SAMPLING,        // �T���v�����O���P����s
    TP_REQUEST_COMMAND_FLAG_AUTO_ON = 1 << TP_REQUEST_COMMAND_AUTO_ON,  // �I�[�g�T���v�����O�J�n
    TP_REQUEST_COMMAND_FLAG_AUTO_OFF = 1 << TP_REQUEST_COMMAND_AUTO_OFF,        // �I�[�g�T���v�����O��~
    TP_REQUEST_COMMAND_FLAG_SET_STABILITY = 1 << TP_REQUEST_COMMAND_SET_STABILITY       // �`���^�����O�΍��l�̐ݒ�
}
TPRequestCommandFlag;

// �^�b�`�p�l������̌��ʎ��
typedef enum
{
    TP_RESULT_SUCCESS = 0,             // ����
    TP_RESULT_INVALID_PARAMETER,       // �p�����[�^���ُ�
    TP_RESULT_ILLEGAL_STATUS,          // ���߂��󂯕t�����Ȃ����
    TP_RESULT_EXCLUSIVE,               // SPI�f�o�C�X���r�W�[��
    TP_RESULT_PXI_BUSY                 // ARM7�Ƃ�PXI�ʐM���r�W�[��
}
TPRequestResult;

// �^�b�`�p�l���̏��
typedef enum
{
    TP_STATE_READY = 0,                // ���f�B���
    TP_STATE_SAMPLING,                 // �T���v�����O���N�G�X�g��
    TP_STATE_AUTO_SAMPLING,            // �I�[�g�T���v�����O��
    TP_STATE_AUTO_WAIT_END             // �I�[�g�T���v�����O�I���҂�
}
TPState;

/*---------------------------------------------------------------------------*
    Structures definition
 *---------------------------------------------------------------------------*/

// �^�b�`�p�l�����͍\����
typedef struct
{
    u16     x;                         // x���W( 0 �` 4095 )
    u16     y;                         // y���W( 0 �` 4095 )
    u16     touch;                     // �ڐG����
    u16     validity;                  // �L��������
}
TPData;


#define TP_CALIBRATE_DOT_SCALE_SHIFT        8   // X�h�b�g�T�C�Y�̐��x
#define TP_CALIBRATE_ORIGIN_SCALE_SHIFT     2   // ���_���W�̐��x

// �^�b�`�p�l���L�����u���[�V�����f�[�^
typedef struct NvTpData
{
    s16     x0;                        // X���_���W
    s16     y0;                        // Y���_���W
    s16     xDotSize;                  // X�h�b�g�T�C�Y
    s16     yDotSize;                  // Y�h�b�g�T�C�Y
}
TPCalibrateParam;                      // 8byte


// ���[�U�R�[���o�b�N�֐�
typedef void (*TPRecvCallback) (TPRequestCommand command, TPRequestResult result, u16 index);

/*===========================================================================*
    Function definition
 *===========================================================================*/
void    TP_Init(void);
void    TP_SetCallback(TPRecvCallback callback);
void    TP_SetCalibrateParam(const TPCalibrateParam *param);

void    TP_RequestSamplingAsync(void);
u32     TP_WaitRawResult(TPData *result);
u32     TP_WaitCalibratedResult(TPData *result);
u32     TP_GetCalibratedResult(TPData *result);
void    TP_RequestAutoSamplingStartAsync(u16 vcount, u16 frequence, TPData samplingBufs[],
                                         u16 bufSize);
void    TP_RequestAutoSamplingStopAsync(void);
void    TP_RequestSetStabilityAsync(u8 retry, u16 range);
void    TP_WaitBusy(TPRequestCommandFlag command_flgs);
void    TP_WaitAllBusy(void);
u32     TP_CheckBusy(TPRequestCommandFlag command_flgs);
u32     TP_CheckError(TPRequestCommandFlag command);

void    TP_GetLatestRawPointInAuto(TPData *result);
void    TP_GetLatestCalibratedPointInAuto(TPData *result);
u16     TP_GetLatestIndexInAuto(void);
u32     TP_CalcCalibrateParam(TPCalibrateParam *calibrate,
                              u16 raw_x1, u16 raw_y1,
                              u16 dx1, u16 dy1, u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2);
BOOL    TP_GetUserInfo(TPCalibrateParam *calibrate);
void    TP_GetCalibratedPoint(TPData *disp, const TPData *raw);
void    TP_GetUnCalibratedPoint(u16 *raw_x, u16 *raw_y, u16 dx, u16 dy);

/*---------------------------------------------------------------------------*
    Inline function definition
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         TP_RequestSetStability

  Description:  �^�b�`�p�l���̃`���^�����O�΍�p�����[�^��ݒ肷��B
                �l�����肷��܂ł̃��g���C�T���v�����O�񐔂ƁA
                �l�����肵���Ɣ��肷�邽�߂̃����W��ݒ�B

  Arguments:    retry -  ���̈����͓����ł͎g�p����Ă��܂���B
                range -  �l�����肵�����ǂ����𔻒肷�邽�߂̃����W.
                         (�͈�:0�`255, �f�t�H���g�l:20)
                
  Returns:      u32  - ���N�G�X�g����������� 0
                       ���s���Ă���� 0�ȊO.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestSetStability(u8 retry, u16 range)
{
    TP_RequestSetStabilityAsync(retry, range);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SET_STABILITY);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_SET_STABILITY);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStart

  Description:  ARM7�ɑ΂��ă^�b�`�p�l���l�̃I�[�g�T���v�����O�J�n�v�����o���B
                1�t���[����frequence��̃f�[�^���ϓ��ȊԊu�ŃT���v�����O����A
                ���ʂ�samplingBus�Ŏw�肵���z��֊i�[����B

  Arguments:    vcount       - �I�[�g�T���v�����O���s����ƂȂ�VCOUNT�l��ݒ�B
                frequence    - �P�t���[���ɉ���̃T���v�����O���s�����̐ݒ�B
                samplingBufs - �I�[�g�T���v�����O�����f�[�^���i�[����̈��ݒ�B
                               �Œ�ł�frequence�̑傫�����̗̈�p�ӂ���K�v��
                               ����B

  Returns:      u32  - ���N�G�X�g����������� 0
                       ���s���Ă���� 0�ȊO.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestAutoSamplingStart(u16 vcount, u16 frequence, TPData samplingBufs[],
                                              u16 bufSize)
{
    TP_RequestAutoSamplingStartAsync(vcount, frequence, samplingBufs, bufSize);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_ON);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStop

  Description:  ARM7�ɑ΂��ă^�b�`�p�l���l�̃I�[�g�T���v�����O��~�v�����o���B

  Arguments:    None.

  Returns:      u32  - ���N�G�X�g����������� 0
                       ���s���Ă���� 0�ȊO.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestAutoSamplingStop(void)
{
    TP_RequestAutoSamplingStopAsync();
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);
    return TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestRawSampling

  Description:  ARM7�ɑ΂��ă^�b�`�p�l���l��v�����A�L�����u���[�g����Ă��Ȃ�
                ���̒l��ǂݏo���B
                �����œ�����l�̓`���^�����O�΍�ς݁B
                �I�[�g�T���v�����O���͎g�p�ł��Ȃ��B
                
  Arguments:    None.

  Returns:      result - �^�b�`�p�l���l���擾���邽�߂̕ϐ��ւ̃|�C���^�B
                         x,y���W�̓L�����u���[�g����Ă��Ȃ��l(0�`4095)���Ƃ�B
                
                u32    - ���N�G�X�g����������� 0
                         ���s���Ă���� 0�ȊO.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestRawSampling(TPData *result)
{
    TP_RequestSamplingAsync();
    return TP_WaitRawResult(result);
}

/*---------------------------------------------------------------------------*
  Name:         TP_RequestCalibratedSampling

  Description:  ARM7�ɑ΂��ă^�b�`�p�l���l��v�����A�L�����u���[�g���ꂽ
                ��ʍ��W�ɑΉ������l��ǂݏo���B
                �����œ�����l�̓`���^�����O�΍�ς݁B
                �I�[�g�T���v�����O���͎g�p�ł��Ȃ��B
                
  Arguments:    None.

  Returns:      result - �^�b�`�p�l���l���擾���邽�߂̕ϐ��ւ̃|�C���^�B
                         x,y���W�͉�ʍ��W�ɑΉ������l���Ƃ�B
                         �������L�����u���[�V�����p�����[�^���ݒ肳��Ă��Ȃ��ꍇ
                         �ɂ�(0�`4095)�̃^�b�`�p�l���l��������B
                
                u32    - ���N�G�X�g����������� 0
                         ���s���Ă���� 0�ȊO.
 *---------------------------------------------------------------------------*/
static inline u32 TP_RequestCalibratedSampling(TPData *result)
{
    TP_RequestSamplingAsync();
    return TP_WaitCalibratedResult(result);
}



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/* NITRO_SPI_ARM9_TP_H_ */
