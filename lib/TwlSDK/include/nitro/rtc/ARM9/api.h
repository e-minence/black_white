/*---------------------------------------------------------------------------*
  Project:  TwlSDK - RTC - include
  File:     api.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev:$
  $Author:$
 *---------------------------------------------------------------------------*/

#ifndef NITRO_RTC_ARM9_API_H_
#define NITRO_RTC_ARM9_API_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/rtc/common/type.h>
#include    <nitro/rtc/common/fifo.h>
#include    <nitro/pxi.h>


/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

// �j����`
typedef enum RTCWeek
{
    RTC_WEEK_SUNDAY = 0,               // ���j��
    RTC_WEEK_MONDAY,                   // ���j��
    RTC_WEEK_TUESDAY,                  // �Ηj��
    RTC_WEEK_WEDNESDAY,                // ���j��
    RTC_WEEK_THURSDAY,                 // �ؗj��
    RTC_WEEK_FRIDAY,                   // ���j��
    RTC_WEEK_SATURDAY,                 // �y�j��
    RTC_WEEK_MAX
}
RTCWeek;

// �A���[���`�����l����`
typedef enum RTCAlarmChan
{
    RTC_ALARM_CHAN_1 = 0,              // �����݃`�����l���P
    RTC_ALARM_CHAN_2,                  // �����݃`�����l���Q
    RTC_ALARM_CHAN_MAX
}
RTCAlarmChan;

// �A���[����Ԓ�`
typedef enum RTCAlarmStatus
{
    RTC_ALARM_STATUS_OFF = 0,          // �����݋֎~���
    RTC_ALARM_STATUS_ON,               // �����݋����
    RTC_ALARM_STATUS_MAX
}
RTCAlarmStatus;

// �A���[���L���t���O��`
#define     RTC_ALARM_ENABLE_NONE       0x0000  // �L���t���O�Ȃ�
#define     RTC_ALARM_ENABLE_WEEK       0x0001  // �j���ݒ�������ݔ��f�Ɏg�p
#define     RTC_ALARM_ENABLE_HOUR       0x0002  // ���Ԑݒ�������ݔ��f�Ɏg�p
#define     RTC_ALARM_ENABLE_MINUTE     0x0004  // ���ݒ�������ݔ��f�Ɏg�p
#define     RTC_ALARM_ENABLE_ALL        ( RTC_ALARM_ENABLE_WEEK | RTC_ALARM_ENABLE_HOUR | RTC_ALARM_ENABLE_MINUTE )

// �������ʒ�`
typedef enum RTCResult
{
    RTC_RESULT_SUCCESS = 0,
    RTC_RESULT_BUSY,
    RTC_RESULT_ILLEGAL_PARAMETER,
    RTC_RESULT_SEND_ERROR,
    RTC_RESULT_INVALID_COMMAND,
    RTC_RESULT_ILLEGAL_STATUS,
    RTC_RESULT_FATAL_ERROR,
    RTC_RESULT_MAX
}
RTCResult;


/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
// �R�[���o�b�N�֐��^��`
typedef void (*RTCCallback) (RTCResult result, void *arg);
typedef void (*RTCInterrupt) (void);

// ���t�\����
typedef struct RTCDate
{
    u32     year;                      // �N ( 0 ~ 99 )
    u32     month;                     // �� ( 1 ~ 12 )
    u32     day;                       // �� ( 1 ~ 31 )
    RTCWeek week;                      // �j��

}
RTCDate;

// �����\����
typedef struct RTCTime
{
    u32     hour;                      // �� ( 0 ~ 23 )
    u32     minute;                    // �� ( 0 ~ 59 )
    u32     second;                    // �b ( 0 ~ 59 )

}
RTCTime;

// �~���b�t�B�[���h���ǉ����ꂽ�����\����
typedef struct RTCTimeEx
{
    u32     hour;                      // �� ( 0 ~ 23 )
    u32     minute;                    // �� ( 0 ~ 59 )
    u32     second;                    // �b ( 0 ~ 59 )
    u32     millisecond;               // �~���b ( 0 ~ 999)
}
RTCTimeEx;

// �A���[���p�����[�^�\����
typedef struct RTCAlarmParam
{
    RTCWeek week;                      // �j��
    u32     hour;                      // �� ( 0 ~ 23 )
    u32     minute;                    // �� ( 0 ~ 59 )
    u32     enable;                    // �A���[���L���t���O( RTC_ALARM_ENABLE_* )

}
RTCAlarmParam;


/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         RTC_Init

  Description:  RTC���C�u����������������B
       Notice:  �R���|�[�l���g���̏������ɂ���ēd�������`�F�b�N���s����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTC_Init(void);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDate

  Description:  RTC������t�f�[�^��ǂݏo���B

  Arguments:    date      - ���t�f�[�^���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDate(RTCDate *date);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTime

  Description:  RTC���玞���f�[�^��ǂݏo���B

  Arguments:    time      - �����f�[�^���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTime(RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTime

  Description:  RTC������t�E�����f�[�^��ǂݏo���B

  Arguments:    date      - ���t�f�[�^���i�[����o�b�t�@���w��B
                time      - �����f�[�^���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTime(RTCDate *date, RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDate

  Description:  RTC�ɓ��t�f�[�^���������ށB

  Arguments:    date      - ���t�f�[�^���i�[����Ă���o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDate(const RTCDate *date);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTime

  Description:  RTC�Ɏ����f�[�^���������ށB

  Arguments:    time      - �����f�[�^���i�[����Ă���o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTime(const RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTime

  Description:  RTC�ɓ��t�E�����f�[�^���������ށB

  Arguments:    date      - ���t�f�[�^���i�[����Ă���o�b�t�@���w��B
                time      - �����f�[�^���i�[����Ă���o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateTime(const RTCDate *date, const RTCTime *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatus

  Description:  RTC����A���[����ON/OFF��Ԃ�ǂݏo���B

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                status    - �A���[����Ԃ��i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmStatus(RTCAlarmChan chan, RTCAlarmStatus *status);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParam

  Description:  RTC����A���[���̐ݒ�l��ǂݏo���B

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                param     - �A���[���ݒ�l���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmParam(RTCAlarmChan chan, RTCAlarmParam *param);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatus

  Description:  RTC�ɃA���[����Ԃ��������ށB

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                status    - �A���[����Ԃ��i�[����Ă���o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmStatus(RTCAlarmChan chan, const RTCAlarmStatus *status);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParam

  Description:  RTC�ɃA���[���ݒ�l���������ށB

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                param     - �A���[���ݒ�l���i�[����Ă���o�b�t�@���w��B

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmParam(RTCAlarmChan chan, const RTCAlarmParam *param);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateAsync

  Description:  RTC������t�f�[�^��񓯊��œǂݏo���B

  Arguments:    date      - ���t�f�[�^���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateAsync(RTCDate *date, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTimeAsync

  Description:  RTC���玞���f�[�^��񓯊��œǂݏo���B

  Arguments:    time      - �����f�[�^���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTimeAsync(RTCTime *time, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeAsync

  Description:  RTC������t�E�����f�[�^��񓯊��œǂݏo���B

  Arguments:    date      - ���t�f�[�^���i�[����o�b�t�@���w��B
                time      - �����f�[�^���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeAsync(RTCDate *date, RTCTime *time, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateAsync

  Description:  RTC�ɓ��t�f�[�^��񓯊��ŏ������ށB

  Arguments:    date      - ���t�f�[�^���i�[����Ă���o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateAsync(const RTCDate *date, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTimeAsync

  Description:  RTC�Ɏ����f�[�^��񓯊��ŏ������ށB

  Arguments:    time      - �����f�[�^���i�[����Ă���o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTimeAsync(const RTCTime *time, RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTimeAsync

  Description:  RTC�ɓ��t�E�����f�[�^��񓯊��ŏ������ށB

  Arguments:    date      - ���t�f�[�^���i�[����Ă���o�b�t�@���w��B
                time      - �����f�[�^���i�[����Ă���o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateTimeAsync(const RTCDate *date, const RTCTime *time, RTCCallback callback,
                               void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatusAsync

  Description:  RTC����A���[����ON/OFF��Ԃ�񓯊��œǂݏo���B

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                status    - �A���[����Ԃ��i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmStatusAsync(RTCAlarmChan chan, RTCAlarmStatus *status, RTCCallback callback,
                                  void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParamAsync

  Description:  RTC����A���[���̐ݒ�l��񓯊��œǂݏo���B

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                param     - �A���[���ݒ�l���i�[����o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmParamAsync(RTCAlarmChan chan, RTCAlarmParam *param, RTCCallback callback,
                                 void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmInterrupt

  Description:  �A���[�������ݔ������̃R�[���o�b�N�֐���ݒ肷��B

  Arguments:    interrupt - �R�[���o�b�N�֐����w��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    RTC_SetAlarmInterrupt(RTCInterrupt interrupt);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatusAsync

  Description:  RTC�ɃA���[����Ԃ�񓯊��ŏ������ށB

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                status    - �A���[����Ԃ��i�[����Ă���o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmStatusAsync(RTCAlarmChan chan, const RTCAlarmStatus *status,
                                  RTCCallback callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParamAsync

  Description:  RTC�ɃA���[���ݒ��񓯊��ŏ������ށB
       Notice:  RTC�̃A���[����Ԃ�ON�ɂȂ��Ă��Ȃ��ꍇ�A�f�o�C�X���Ń��C�g��
                �󂯕t���Ă���Ȃ��̂ŁA�������݂͎��s����B

  Arguments:    chan      - �A���[���̃`�����l�����w��B
                param     - �A���[���ݒ�l���i�[����Ă���o�b�t�@���w��B
                callback  - �񓯊����������������ۂɌĂяo���֐����w��B
                arg       - �R�[���o�b�N�֐��Ăяo�����̈������w��B

  Returns:      RTCResult - �񓯊��f�o�C�X����J�n�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmParamAsync(RTCAlarmChan chan, const RTCAlarmParam *param,
                                 RTCCallback callback, void *arg);


/*---------------------------------------------------------------------------*
    ����J�֐���`
 *---------------------------------------------------------------------------*/
BOOL    RTCi_ResetAsync(void);
BOOL    RTCi_SetHourFormatAsync(void);
BOOL    RTCi_ReadRawDateTimeAsync(void);
BOOL    RTCi_WriteRawDateTimeAsync(void);
BOOL    RTCi_ReadRawDateAsync(void);
BOOL    RTCi_WriteRawDateAsync(void);
BOOL    RTCi_ReadRawTimeAsync(void);
BOOL    RTCi_WriteRawTimeAsync(void);
BOOL    RTCi_ReadRawPulseAsync(void);
BOOL    RTCi_WriteRawPulseAsync(void);
BOOL    RTCi_ReadRawAlarm1Async(void);
BOOL    RTCi_WriteRawAlarm1Async(void);
BOOL    RTCi_ReadRawAlarm2Async(void);
BOOL    RTCi_WriteRawAlarm2Async(void);
BOOL    RTCi_ReadRawStatus1Async(void);
BOOL    RTCi_WriteRawStatus1Async(void);
BOOL    RTCi_ReadRawStatus2Async(void);
BOOL    RTCi_WriteRawStatus2Async(void);
BOOL    RTCi_ReadRawAdjustAsync(void);
BOOL    RTCi_WriteRawAdjustAsync(void);
BOOL    RTCi_ReadRawFreeAsync(void);
BOOL    RTCi_WriteRawFreeAsync(void);
RTCResult RTCi_SetRegStatus2Async(const RTCRawStatus2 *status2, RTCCallback callback, void *arg);
RTCResult RTCi_SetRegAdjustAsync(const RTCRawAdjust *adjust, RTCCallback callback, void *arg);
RTCResult RTCi_SetRegAdjust(const RTCRawAdjust *Adjust);
RTCResult RTCi_SetRegStatus2(const RTCRawStatus2 *status2);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_RTC_ARM9_API_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
