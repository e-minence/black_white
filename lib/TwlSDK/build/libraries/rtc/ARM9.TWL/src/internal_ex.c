/*---------------------------------------------------------------------------*
  Project:  TwlSDK - RTC - libraries
  File:     internal_ex.c

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

#include    <twl/rtc/common/fifo_ex.h>
#include    <nitro/pxi.h>
#include	"private.h"

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static BOOL     RtcexSendPxiCommand(u32 command);

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_ReadRawCounterAsync

  Description:  �����H�̃A�b�v�J�E���^���W�X�^�ݒ�l��񓯊��Ŏ擾����B
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                �f�[�^��OS_GetSystemWork()->real_time_clock�Ɋi�[�����B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL
RTCEXi_ReadRawCounterAsync(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_READ_COUNTER);
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_ReadRawFoutAsync

  Description:  �����H��FOUT1/FOUT2���W�X�^�ݒ�l��񓯊��Ŏ擾����B
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                �f�[�^��OS_GetSystemWork()->real_time_clock�Ɋi�[�����B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL RTCEXi_ReadRawFoutAsync(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_READ_FOUT);
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_WriteRawFoutAsync

  Description:  �����H��FOUT1/FOUT2�ݒ�l��񓯊��Ńf�o�C�X�ɏ������ށB
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                OS_GetSystemWork()->real_time_clock�ɂ���f�[�^���������܂��B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL RTCEXi_WriteRawFoutAsync(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_WRITE_FOUT);
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_ReadRawAlarmEx1Async

  Description:  �����H�̃A���[���g���P���W�X�^�ݒ�l��񓯊��Ŏ擾����B
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                �f�[�^��OS_GetSystemWork()->real_time_clock�Ɋi�[�����B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL RTCEXi_ReadRawAlarmEx1Async(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_READ_ALARM_EX1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_WriteRawAlarmEx1Async

  Description:  �����H�̃A���[���g���P���W�X�^��񓯊��Ńf�o�C�X�ɏ������ށB
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                OS_GetSystemWork()->real_time_clock�ɂ���f�[�^���������܂��B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL RTCEXi_WriteRawAlarmEx1Async(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_WRITE_ALARM_EX1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_ReadRawAlarmEx2Async

  Description:  �����H�̃A���[���g���Q���W�X�^�ݒ�l��񓯊��Ŏ擾����B
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                �f�[�^��OS_GetSystemWork()->real_time_clock�Ɋi�[�����B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL RTCEXi_ReadRawAlarmEx2Async(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_READ_ALARM_EX2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCEXi_WriteRawAlarmEx2Async

  Description:  �����H�̃A���[���g���Q���W�X�^��񓯊��Ńf�o�C�X�ɏ������ށB
                ARM7����̉�����PXI�R�[���o�b�N(tag:RTC)�ɂĒʒm�����B
                OS_GetSystemWork()->real_time_clock�ɂ���f�[�^���������܂��B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL RTCEXi_WriteRawAlarmEx2Async(void)
{
    return RtcexSendPxiCommand(RTC_PXI_COMMAND_WRITE_ALARM_EX2);
}

/*---------------------------------------------------------------------------*
  Name:         RtcexSendPxiCommand

  Description:  �w��R�}���h��PXI�o�R��ARM7�ɑ��M����B

  Arguments:    None.

  Returns:      BOOL     - PXI�ɑ΂��đ��M�����������ꍇTRUE���A
                           PXI�ɂ�鑗�M�Ɏ��s�����ꍇFALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL RtcexSendPxiCommand(u32 command)
{
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_RTC,
                               ((command << RTC_PXI_COMMAND_SHIFT) & RTC_PXI_COMMAND_MASK), 0))
    {
        return FALSE;
    }
    return TRUE;
}
