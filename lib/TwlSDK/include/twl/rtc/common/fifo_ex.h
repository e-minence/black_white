/*---------------------------------------------------------------------------*
  Project:  TwlSDK - rtc - include
  File:     fifo_ex.h

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
#ifndef TWL_RTC_COMMON_FIFO_EX_H_
#define TWL_RTC_COMMON_FIFO_EX_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
/* �ǉ� PXI �R�}���h��` */
#define RTC_PXI_COMMAND_READ_COUNTER        0x50        /* �A�b�v�J�E���^�ǂݏo�� */
#define RTC_PXI_COMMAND_READ_FOUT           0x51        /* FOUT�ݒ�l�ǂݏo�� */
#define RTC_PXI_COMMAND_READ_ALARM_EX1      0x52        /* �A���[���P�ݒ�l�ǂݏo�� (�g����) */
#define RTC_PXI_COMMAND_READ_ALARM_EX2      0x53        /* �A���[���Q�ݒ�l�ǂݏo�� (�g����) */
#define RTC_PXI_COMMAND_WRITE_FOUT          0x61        /* FOUT�ݒ�l�������� */
#define RTC_PXI_COMMAND_WRITE_ALARM_EX1     0x62        /* �A���[���P�ݒ�l�������� (�g����) */
#define RTC_PXI_COMMAND_WRITE_ALARM_EX2     0x63        /* �A���[���Q�ݒ�l�������� (�g����) */


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_RTC_COMMON_FIFO_EX_H_ */
