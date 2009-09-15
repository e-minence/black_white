/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - libraries -
  File:     wxc_scheduler.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-16#$
  $Rev: 9661 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/scheduler.h>


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WXCi_InitScheduler

  Description:  ���[�h�؂�ւ��X�P�W���[����������.

  Arguments:    p             WXCScheduler �\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXCi_InitScheduler(WXCScheduler * p)
{
    /*
     * �o���I�ɓK�؂Ǝv����f�t�H���g�̃X�P�W���[���ŏ�����.
     * �f�o�b�O�p�r�ȊO�ł͊�{�I�ɂ��̐ݒ��ύX���Ȃ�.
     */
    static const BOOL default_table[WXC_SCHEDULER_PATTERN_MAX][WXC_SCHEDULER_SEQ_MAX] = {
        {TRUE, FALSE, TRUE, TRUE},
        {FALSE, TRUE, TRUE, TRUE},
        {FALSE, TRUE, TRUE, TRUE},
        {TRUE, FALSE, TRUE, TRUE},
    };
    p->seq = (int)((OS_GetTick() >> 0) % WXC_SCHEDULER_SEQ_MAX);
    p->pattern = (int)((OS_GetTick() >> 2) % WXC_SCHEDULER_PATTERN_MAX);
    p->start = 0;
    p->child_mode = FALSE;
    MI_CpuCopy32(default_table, p->table, sizeof(default_table));
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_SetChildMode

  Description:  �X�P�W���[�����q�@���Œ�œ��삷��悤�ݒ�.

  Arguments:    p             WXCScheduler �\����
                enable        �q�@���ł����N�������Ȃ��ꍇ�� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXCi_SetChildMode(WXCScheduler * p, BOOL enable)
{
    p->child_mode = enable;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_UpdateScheduler

  Description:  ���[�h�؂�ւ��X�P�W���[�����X�V.

  Arguments:    p             WXCScheduler �\����

  Returns:      ���ݐe�@���[�h�Ȃ� TRUE ��, �q�@���[�h�Ȃ� FALSE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL WXCi_UpdateScheduler(WXCScheduler * p)
{
    if (++p->seq >= WXC_SCHEDULER_SEQ_MAX)
    {
        p->seq = 0;
        if (++p->pattern >= WXC_SCHEDULER_PATTERN_MAX)
        {
            p->pattern = 0;
        }
        if (p->pattern == p->start)
        {
            /* �����̂��� */
            p->start = (int)(OS_GetTick() % WXC_SCHEDULER_PATTERN_MAX);
            p->pattern = p->start;
        }
    }
    return p->table[p->pattern][p->seq] && !p->child_mode;
}
