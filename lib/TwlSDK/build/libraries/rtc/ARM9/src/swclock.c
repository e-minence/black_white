/*---------------------------------------------------------------------------*
  Project:  TwlSDK - RTC - libraries
  File:     swclock.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date: 2008-01-11#$
  $Rev: 9449 $
  $Author: tokunaga_eiji $
 *---------------------------------------------------------------------------*/

#include <nitro/os.h>
#include <nitro/rtc.h>
#include <nitro/spi/ARM9/pm.h>

/*---------------------------------------------------------------------------*
    �ÓI�ϐ���`
 *---------------------------------------------------------------------------*/
static u16       rtcSWClockInitialized;       // �`�b�N�������m�F�t���O
static OSTick    rtcSWClockBootTicks;         // �u�[�g�� RTC �̃`�b�N���Z�l
static RTCResult rtcLastResultOfSyncSWClock;  // �Ō�̓������� RTCResult
static PMSleepCallbackInfo rtcSWClockSleepCbInfo; // �X���[�v���A���R�[���o�b�N���

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/

static void RtcGetDateTimeExFromSWClock(RTCDate * date, RTCTimeEx *time);
static void RtcSleepCallbackForSyncSWClock(void* args);

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
RTCResult RTC_InitSWClock(void)
{
    SDK_ASSERT(OS_IsTickAvailable());
    
    // ���ɏ���������Ă���ꍇ�͉��������� RTC_RESULT_SUCCESS ��Ԃ�
    if(rtcSWClockInitialized)
    {
        return RTC_RESULT_SUCCESS;
    }
    
    // �\�t�g�E�F�A�N���b�N�l�����݂� RTC �l�Ɠ�������B
    (void) RTC_SyncSWClock();

    // �X���[�v���A���R�[���o�b�N��o�^����B
    PM_SetSleepCallbackInfo(&rtcSWClockSleepCbInfo, RtcSleepCallbackForSyncSWClock, NULL);
    PM_AppendPostSleepCallback(&rtcSWClockSleepCbInfo);
    
    rtcSWClockInitialized = TRUE;

    return rtcLastResultOfSyncSWClock;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetSWClockTicks

  Description:  ���݂̃`�b�N�l�ƃu�[�g�� RTC �̃`�b�N���Z�l�����v���� 2000�N1��
                1���ߑO0��0��0�b���猻�ݎ����܂ł̎��Ԃ��`�b�N���Z�l�ŕԂ��B

  Arguments:    None

  Returns:      OSTick - 2000�N1��1���ߑO0��0��0�b���猻�ݎ����܂Ń`�b�N���Z����
                         �Ō�̓����Ɏ��s���Ă���ꍇ�� 0
 *---------------------------------------------------------------------------*/
OSTick RTC_GetSWClockTick(void)
{
    if(rtcLastResultOfSyncSWClock == RTC_RESULT_SUCCESS)
    {
        return OS_GetTick() + rtcSWClockBootTicks;
    }
    else
    {
        return 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetLastSyncSWClockResult

  Description:  �Ō�̃\�t�g�E�F�A�N���b�N�������ɌĂяo���ꂽ RTC_GetDateTime
                �̌��ʂ�Ԃ��B

  Arguments:    None

  Returns:      RTCResult - �������ɌĂяo���ꂽ RTC_GetDateTime �̌���
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetLastSyncSWClockResult(void)
{
    return rtcLastResultOfSyncSWClock;
}


/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeExFromSWClock

  Description:  CPU �`�b�N�𗘗p����\�t�g�E�F�A�N���b�N����~���b�P�ʂ܂ł�
                ���t�E�����f�[�^��ǂݏo���B

  Arguments:    date      - ���t�f�[�^���i�[����o�b�t�@���w��B
                time      - �����f�[�^���i�[����o�b�t�@���w��B

  Returns:      RTCResult - �Ō�̓������ɕۑ����ꂽ RTCResult ��Ԃ�
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeExFromSWClock(RTCDate *date, RTCTimeEx *time)
{
    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(time);

    RtcGetDateTimeExFromSWClock(date, time);
    
    return rtcLastResultOfSyncSWClock;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SyncSWClock

  Description:  CPU �`�b�N�𗘗p����\�t�g�E�F�A�N���b�N�� RTC �ɓ�������B
                �������ɌĂяo���ꂽ RTC_GetDateTime �̌��ʂ��ۑ�����A
                RTC_GetDateTimeExFromSWClock �̖߂�l�Ƃ��ĕԂ����B

  Arguments:    None

  Returns:      RTCResult - �f�o�C�X����̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
RTCResult RTC_SyncSWClock(void)
{   
    RTCDate currentDate;
    RTCTime currentTime;
    
    // RTC_GetDateTime �̌��ʂ�ۑ�����B
    rtcLastResultOfSyncSWClock = RTC_GetDateTime(&currentDate, &currentTime);
    // ���݂� RTC ���`�b�N���Z���A���� CPU �`�b�N�Ƃ̍���������āA�u�[�g�� RTC �̃`�b�N���Z�l��ۑ�����B    
    rtcSWClockBootTicks = OS_SecondsToTicks(RTC_ConvertDateTimeToSecond(&currentDate, &currentTime)) - OS_GetTick();

    return rtcLastResultOfSyncSWClock;
}

/*---------------------------------------------------------------------------*
  Name:         RtcGetDateTimeExFromSWClock

  Description:  ���݂̃`�b�N�ƃ\�t�g�E�F�A�N���b�N�� RTC �ɓ����������ɕۑ���
                �ꂽ�`�b�N�̍�������A���t�Ǝ����f�[�^���v�Z����B

  Arguments:    date      - buffer for storing RTCDate
                time      - buffer for storing RTCTimeEx

  Returns:      None
 *---------------------------------------------------------------------------*/
static void RtcGetDateTimeExFromSWClock(RTCDate * date, RTCTimeEx *time)
{
    OSTick currentTicks;
    s64 currentSWClockSeconds;

    currentTicks = RTC_GetSWClockTick();
    currentSWClockSeconds = (s64) OS_TicksToSeconds(currentTicks);
        
    // �~���b���̂��������݂̕b���� RTCDate �� RTCTime �ɕϊ�����B
    RTC_ConvertSecondToDateTime(date, (RTCTime*)time, currentSWClockSeconds);

    // �~���b��ʌɉ�����
    time->millisecond = (u32) (OS_TicksToMilliSeconds(currentTicks) % 1000);  
}

/*---------------------------------------------------------------------------*
  Name:         RtcSleepCallbackForSyncSWClock

  Description:  RTC_InitSWClock �Ăяo�����ɓo�^�����X���[�v�R�[���o�b�N�B
                �X���[�v���A���Ƀ\�t�g�E�F�A�N���b�N�� RTC �ɓ���������B

  Arguments:    args     - ���p����Ȃ�����

  Returns:      None
 *---------------------------------------------------------------------------*/
#define RTC_SWCLOCK_SYNC_RETRY_INTERVAL   1   // �P�� milliseconds
static void RtcSleepCallbackForSyncSWClock(void* args)
{
#pragma unused(args)
    for (;;)
    {
        (void) RTC_SyncSWClock();

        // RTC �����̃X���b�h�������� ARM7 �ŏ��������APXI FIFO �� FULL �̎���
        // RTC_SWCLOCK_SYNC_RETRY_INTERVAL �~���b�҂��ă��g���C
        if(rtcLastResultOfSyncSWClock != RTC_RESULT_BUSY &&
           rtcLastResultOfSyncSWClock != RTC_RESULT_SEND_ERROR)
        {
            break;
        }

        OS_TWarning("RTC_SyncSWClock() failed at sleep callback. Retry... \n");
        OS_Sleep(RTC_SWCLOCK_SYNC_RETRY_INTERVAL);
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
