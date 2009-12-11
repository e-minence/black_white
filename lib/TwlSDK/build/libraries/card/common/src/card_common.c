/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_common.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-12-08#$
  $Rev: 11236 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/


#include "../include/card_common.h"
#include "../include/card_event.h"
#include "../include/card_spi.h"
#include <nitro/card/rom.h>


/*---------------------------------------------------------------------------*/
/* variables */

CARDiCommon cardi_common ATTRIBUTE_ALIGN(32);
static CARDiCommandArg cardi_arg ATTRIBUTE_ALIGN(32);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_LockResource

  Description:  ���\�[�X�r���̃��b�N

  Arguments:    owner      ���b�N�̏��L�҂����� lock-ID
                target     ���b�N���ׂ��J�[�h�o�X��̃��\�[�X�^�[�Q�b�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_LockResource(CARDiOwner owner, CARDTargetMode target)
{
    CARDiCommon *const p = &cardi_common;
    OSIntrMode bak_psr = OS_DisableInterrupts();
    if (p->lock_owner == owner)
    {
        if (p->lock_target != target)
        {
            OS_TPanic("card-lock : can not reuse same ID for locking without unlocking!");
        }
    }
    else
    {
        while (p->lock_owner != OS_LOCK_ID_ERROR)
        {
            OS_SleepThread(p->lock_queue);
        }
        p->lock_owner = owner;
        p->lock_target = target;
    }
    ++p->lock_ref;
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_UnlockResource

  Description:  ���\�[�X�r���̃A�����b�N

  Arguments:    owner      ���b�N�̏��L�҂����� lock-ID
                target     �A�����b�N���ׂ��J�[�h�o�X��̃��\�[�X�^�[�Q�b�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_UnlockResource(CARDiOwner owner, CARDTargetMode target)
{
    CARDiCommon *p = &cardi_common;
    OSIntrMode bak_psr = OS_DisableInterrupts();
    if ((p->lock_owner != owner) || !p->lock_ref)
    {
        OS_TPanic("card-unlock : specified ID for unlocking is not locking one!");
    }
    else
    {
        if (p->lock_target != target)
        {
            OS_TPanic("card-unlock : locking target and unlocking one are different!");
        }
        if (!--p->lock_ref)
        {
            p->lock_owner = OS_LOCK_ID_ERROR;
            p->lock_target = CARD_TARGET_NONE;
            OS_WakeupThread(p->lock_queue);
        }
    }
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetAccessLevel

  Description:  ������Ă���ROM�A�N�Z�X���x�����擾

  Arguments:    None.

  Returns:      ������Ă���ROM�A�N�Z�X���x��
 *---------------------------------------------------------------------------*/
CARDAccessLevel CARDi_GetAccessLevel(void)
{
    CARDAccessLevel level = CARD_ACCESS_LEVEL_NONE;
    if (OS_GetBootType() == OS_BOOTTYPE_ROM)
    {
        level = CARD_ACCESS_LEVEL_FULL;
    }
    else if (!OS_IsRunOnTwl())
    {
//        level = CARD_ACCESS_LEVEL_BACKUP;
        level = CARD_ACCESS_LEVEL_FULL;    // �_�E�����[�h�A�v������ ROM �ɃA�N�Z�X�ł���悤��
    }
#ifdef SDK_TWL
    else
    {
        const CARDRomHeaderTWL *header = CARD_GetOwnRomHeaderTWL();
        BOOL                    backupPowerOn = FALSE;
        if (header->access_control.game_card_nitro_mode)
        {
            level |= CARD_ACCESS_LEVEL_ROM;
            backupPowerOn = TRUE;
        }
        else if (header->access_control.game_card_on)
        {
            backupPowerOn = TRUE;
        }
        if (backupPowerOn)
        {
            if (header->access_control.backup_access_read)
            {
                level |= CARD_ACCESS_LEVEL_BACKUP_R;
            }
            if (header->access_control.backup_access_write)
            {
                level |= CARD_ACCESS_LEVEL_BACKUP_W;
            }
        }
    }
#endif
    return level;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitAsync

  Description:  �񓯊�������҂�

  Arguments:    None.

  Returns:      �ŐV�̏������ʂ�CARD_RESULT_SUCCESS�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL CARDi_WaitAsync(void)
{
    SDK_ASSERT(CARD_IsAvailable());
    return CARDi_WaitForTask(&cardi_common, FALSE, NULL, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryWaitAsync

  Description:  �񓯊������ҋ@�����s���Đ��ۂɊւ�炸�������ɐ����Ԃ�

  Arguments:    None.

  Returns:      �ŐV�̔񓯊��������������Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL CARDi_TryWaitAsync(void)
{
    CARDiCommon *const p = &cardi_common;
    SDK_ASSERT(CARD_IsAvailable());

    return !(p->flag & CARD_STAT_BUSY);
}

void CARDi_InitResourceLock(void)
{
    CARDiCommon *p = &cardi_common;
    p->lock_owner = OS_LOCK_ID_ERROR;
    p->lock_ref = 0;
    p->lock_target = CARD_TARGET_NONE;
    OS_InitThreadQueue(p->lock_queue);
}






void CARDi_InitCommand(void)
{
    CARDiCommon *p = &cardi_common;

#if defined(SDK_ARM9)
    p->cmd = &cardi_arg;
    MI_CpuFillFast(&cardi_arg, 0x00, sizeof(cardi_arg));
    DC_FlushRange(&cardi_arg, sizeof(cardi_arg));
#else
    p->cmd = CARD_UNSYNCHRONIZED_BUFFER;
#endif

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_FS, CARDi_OnFifoRecv);
}
