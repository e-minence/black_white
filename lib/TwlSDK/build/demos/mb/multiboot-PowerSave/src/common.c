/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot-PowerSave
  File:     common.c

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/


#ifdef SDK_TWL
#include	<twl.h>
#else
#include	<nitro.h>
#endif
#include "common.h"


/*****************************************************************************/
/* variable */

BOOL    g_power_save_mode;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         ReadKeySetTrigger

  Description:  �L�[�g���K���o.

  Arguments:    None.

  Returns:      �O��Ăяo�����ɉ�����ԂłȂ�, ����
                ����Ăяo�����ɉ�����Ԃł������L�[���͂̃r�b�g�W��.
 *---------------------------------------------------------------------------*/
u16 ReadKeySetTrigger(void)
{
    /* IPL ���ł� A �{�^�������΍� */
    static u16 cont = 0xffff;
    u16     keyset = PAD_Read();
    u16     trigger = (u16)(keyset & (keyset ^ cont));
    cont = keyset;
    return trigger;
}
