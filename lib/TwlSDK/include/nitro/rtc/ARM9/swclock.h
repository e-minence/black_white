/*---------------------------------------------------------------------------*
  Project:  TwlSDK - RTC - include
  File:     swclock.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date$
  $Rev$
  $Author: tokunaga_eiji $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_RTC_ARM9_SWCLOCK_H_
#define NITRO_RTC_ARM9_SWCLOCK_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include    <nitro/types.h>

/*---------------------------------------------------------------------------*
  Name:         RTC_InitSWClock

  Description:  �\�t�g�E�F�A�N���b�N������������B�\�t�g�E�F�A�N���b�N�l������
                �� RTC �l�Ɠ������A�X���[�v���A���R�[���o�b�N�� RTC �l�ƃ\�t�g
                �E�F�A�N���b�N���ē�������֐���ǉ�����B
                RTC_GetDateTimeExFromSWClock �֐������ RTC_SyncSWClock �֐���
                �ĂԑO�ɌĂ�ł����K�v������B

  Arguments:    None

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_InitSWClock(void);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetSWClockTick

  Description:  ���݂̃`�b�N�l�ƃu�[�g�� RTC �̃`�b�N���Z�l�����v���� 2000�N1��
                1��0��00�����猻�ݎ����܂ł̎��Ԃ��`�b�N���Z�l�ŕԂ��B

  Arguments:    None

  Returns:      OSTick - 2000�N1��1��0��00�����猻�ݎ����܂ł̎��Ԃ̃`�b�N���Z�l
 *---------------------------------------------------------------------------*/
OSTick RTC_GetSWClockTick(void);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetSyncSWClockResult

  Description:  �Ō�̃\�t�g�E�F�A�N���b�N�������ɌĂяo���ꂽ RTC_GetDateTime
                �̌��ʂ�Ԃ��B

  Arguments:    None

  Returns:      RTCResult - �������ɌĂяo���ꂽ RTC_GetDateTime �̌���
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetLastSyncSWClockResult(void);

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeExFromSWClock

  Description:  CPU �`�b�N�𗘗p����\�t�g�E�F�A�N���b�N����~���b�P�ʂ܂ł�
                ���t�E�����f�[�^��ǂݏo���B

  Arguments:    date      - ���t�f�[�^���i�[����o�b�t�@���w��B
                time      - �����f�[�^���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �Ō�̓������ɕۑ����ꂽ RTCResult ��Ԃ�
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeExFromSWClock(RTCDate *date, RTCTimeEx *time);

/*---------------------------------------------------------------------------*
  Name:         RTC_SyncSWClock

  Description:  CPU �`�b�N�𗘗p����\�t�g�E�F�A�N���b�N�� RTC �ɓ�������B

  Arguments:    None

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SyncSWClock(void);

/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_RTC_ARM9_SWCLOCK_H_ */


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
