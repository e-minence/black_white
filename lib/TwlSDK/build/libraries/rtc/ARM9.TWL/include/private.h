/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - rtc
  File:     private.h

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
#ifndef TWL_LIBRARIES_RTC_ARM9_PRIVATE_H_
#define TWL_LIBRARIES_RTC_ARM9_PRIVATE_H_
#ifdef  __cplusplus
extern  "C" {
#endif

#include <nitro/rtc/ARM9/api.h>

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
/* �񓯊��֐��r�������p���b�N��` */
typedef enum RTCLock
{
    RTC_LOCK_OFF = 0,               /* ���b�N�J����� */
    RTC_LOCK_ON,                    /* ���b�N�{����� */
    RTC_LOCK_MAX
}
RTCLock;

/* �A���R�}���h���M���܂ޏ����p�̃V�[�P���X��` */
typedef enum RTCSequence
{
    RTC_SEQ_GET_DATE = 0,           /* ���t�擾�V�[�P���X */
    RTC_SEQ_GET_TIME,               /* �����擾�V�[�P���X */
    RTC_SEQ_GET_DATETIME,           /* ���t�E�����擾�V�[�P���X */
    RTC_SEQ_SET_DATE,               /* ���t�ݒ�V�[�P���X */
    RTC_SEQ_SET_TIME,               /* �����ݒ�V�[�P���X */
    RTC_SEQ_SET_DATETIME,           /* ���t�E�����ݒ�V�[�P���X */
    RTC_SEQ_GET_ALARM1_STATUS,      /* �A���[���P��Ԏ擾�V�[�P���X */
    RTC_SEQ_GET_ALARM2_STATUS,      /* �A���[���Q��Ԏ擾�V�[�P���X */
    RTC_SEQ_GET_ALARM_PARAM,        /* �A���[���ݒ�l�擾�V�[�P���X */
    RTC_SEQ_SET_ALARM1_STATUS,      /* �A���[���P��ԕύX�V�[�P���X */
    RTC_SEQ_SET_ALARM2_STATUS,      /* �A���[���Q��ԕύX�V�[�P���X */
    RTC_SEQ_SET_ALARM1_PARAM,       /* �A���[���P�ݒ�l�ύX�V�[�P���X */
    RTC_SEQ_SET_ALARM2_PARAM,       /* �A���[���Q�ݒ�l�ύX�V�[�P���X */
    RTC_SEQ_SET_HOUR_FORMAT,        /* ���ԕ\�L�t�H�[�}�b�g�ύX�V�[�P���X */
    RTC_SEQ_SET_REG_STATUS2,        /* �X�e�[�^�X�Q���W�X�^�������݃V�[�P���X */
    RTC_SEQ_SET_REG_ADJUST,         /* �A�W���X�g���W�X�^�������݃V�[�P���X */
    RTC_SEQ_GET_COUNTER,            /* �A�b�v�J�E���^�̒l�擾�V�[�P���X */
    RTC_SEQ_GET_FOUT,               /* FOUT�l�擾�V�[�P���X */
    RTC_SEQ_SET_FOUT,               /* FOUT�l�ύX�V�[�P���X */
    RTC_SEQ_MAX
}
RTCSequence;

/* ���[�N�p�\���� */
typedef struct RTCWork
{
    u32     lock;                   /* �r�����b�N */
    RTCCallback callback;           /* �񓯊��֐��R�[���o�b�N�ޔ�p */
    void   *buffer[2];              /* �񓯊��֐��p�����[�^�ޔ�p */
    void   *callbackArg;            /* �R�[���o�b�N�֐��̈����ۑ��p */
    u32     sequence;               /* �A���������[�h�Ǘ��p */
    u32     index;                  /* �A�������̏󋵊Ǘ��p */
    RTCInterrupt interrupt;         /* �A���[���ʒm���̌Ăяo���֐��ޔ�p */
    RTCResult commonResult;         /* �񓯊��֐��̏������ʑޔ�p */

}
RTCWork;


/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/
RTCWork*    RTCi_GetSysWork(void);
RTCResult   RTCEXi_CommonCallback(void);

RTCResult   RTCEXi_GetCounterAsync(u32* count, RTCCallback callback, void* arg);
RTCResult   RTCEXi_GetCounter(u32* count);
RTCResult   RTCEXi_GetFoutAsync(u16* fout, RTCCallback callback, void* arg);
RTCResult   RTCEXi_GetFout(u16* fout);
RTCResult   RTCEXi_SetFoutAsync(const u16* fout, RTCCallback callback, void* arg);
RTCResult   RTCEXi_SetFout(const u16* fout);

BOOL    RTCEXi_ReadRawCounterAsync(void);
BOOL    RTCEXi_ReadRawFoutAsync(void);
BOOL    RTCEXi_WriteRawFoutAsync(void);
BOOL    RTCEXi_ReadRawAlarmEx1Async(void);
BOOL    RTCEXi_WriteRawAlarmEx1Async(void);
BOOL    RTCEXi_ReadRawAlarmEx2Async(void);
BOOL    RTCEXi_WriteRawAlarmEx2Async(void);


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_LIBRARIES_RTC_ARM9_PRIVATE_H_ */
