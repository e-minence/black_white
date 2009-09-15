/*---------------------------------------------------------------------------*
  Project:  TwlSDK - rtc - include
  File:     type_ex.h

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
#ifndef TWL_RTC_COMMON_TYPE_EX_H_
#define TWL_RTC_COMMON_TYPE_EX_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

#include	<twl/types.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
/* FOUT�f���[�e�B�[��` */
#define     RTC_FOUT_DUTY_1HZ         	0x0001
#define     RTC_FOUT_DUTY_2HZ           0x0002
#define     RTC_FOUT_DUTY_4HZ           0x0004
#define     RTC_FOUT_DUTY_8HZ           0x0008
#define     RTC_FOUT_DUTY_16HZ          0x0010
#define     RTC_FOUT_DUTY_32HZ          0x0020
#define     RTC_FOUT_DUTY_64HZ          0x0040
#define     RTC_FOUT_DUTY_128HZ         0x0080
#define     RTC_FOUT_DUTY_256HZ         0x0100
#define     RTC_FOUT_DUTY_512HZ         0x0200
#define     RTC_FOUT_DUTY_1KHZ          0x0400
#define     RTC_FOUT_DUTY_2KHZ          0x0800
#define     RTC_FOUT_DUTY_4KHZ          0x1000
#define     RTC_FOUT_DUTY_8KHZ          0x2000
#define     RTC_FOUT_DUTY_16KHZ         0x4000
#define     RTC_FOUT_DUTY_32KHZ         0x8000

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
/* �A�b�v�J�E���^�\���� */
typedef union RTCRawCounter
{
    struct
    {
        u32 count:24;   // big endian
        u32 dummy:8;
    };

    u8  bytes[4];
}
RTCRawCounter;

/* FOUT�\���� */
typedef union RTCRawFout
{
    struct
    {
        u16 fout;
        u16 dummy0;
    };
    struct
    {
        u8  fout2;
        u8  fout1;
        u16 dummy1;
    };
}
RTCRawFout;

/* �A���[���g���\���� */
typedef union
{
    u32     year:8;            	/* �N( 00 ~ 99 ) */
    u32     month:5;            /* ��( 01 ~ 12 ) */
    u32     dummy0:1;
    u32     ye:1;               /* �N�ݒ�L���t���O */
    u32     me:1;               /* ���ݒ�L���t���O */
    u32     day:6;              /* ��( 01 ~ 31 ) ��/�[�N�ɂ�����͕ϓ� */
    u32     dummy1:1;
    u32     de:1;               /* ���ݒ�L���t���O */
    u32     dummy2:8;
}
RTCRawAlarmEx;

/* ARM9 <-> ARM7 �󂯓n���f�[�^�\����( 8�o�C�g���p�� ) */
typedef union RTCRawDataEx
{
    struct
    {
        RTCRawAlarmEx alarmex;      // �A���[��( 1 or 2 )�g�����W�X�^�ݒ�
        union
        {
            RTCRawCounter counter;  // �A�b�v�J�E���^
            RTCRawFout fout;        // FOUT1/FOUT2���W�X�^�ݒ�
        };
    }
    a;

    u32     words[2];              	// 4�o�C�g�A�N�Z�X�p
    u16     halfs[4];               // 2�o�C�g�A�N�Z�X�p
    u8      bytes[8];               // �o�C�g�A�N�Z�X�p
}
RTCRawDataEx;


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_RTC_COMMON_TYPE_EX_H_ */
