/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_api.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_event.h"
#include "../include/card_rom.h"


/*---------------------------------------------------------------------------*/
/* variables */

static BOOL CARDi_EnableFlag = FALSE;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_LockBusCondition

  Description:   �C�x���g���X�i�[�\���̂̃C�x���g������ҋ@�B

  Arguments:    userdata : ���b�NID

  Returns:      None.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_LockBusCondition(void *userdata)
{
    u16     lockID = *(const u16 *)userdata;
    return (OS_TryLockCard(lockID) == OS_LOCK_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_Init

  Description:  DARD���C�u������������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_Init(void)
{
    CARDiCommon *p = &cardi_common;

    if (!p->flag)
    {
        p->flag = CARD_STAT_INIT;

#if !defined(SDK_SMALL_BUILD) && defined(SDK_ARM9)
        // ��J�[�h�u�[�g���Ɋi�[�����CARD-ROM�w�b�_����
        // �J�[�h�u�[�g���ɋ�f�[�^�Ȃ̂ł���𐳂����ޔ��B
        if (OS_GetBootType() == OS_BOOTTYPE_ROM)
        {
            MI_CpuCopy8((const void *)HW_ROM_HEADER_BUF, (void *)HW_CARD_ROM_HEADER,
                        HW_CARD_ROM_HEADER_SIZE);
        }
#endif // !defined(SDK_SMALL_BUILD) && defined(SDK_ARM9)

#if defined(SDK_ARM9)
        // ���C�u�����̓����ϐ����������B
        p->src = 0;
        p->dst = 0;
        p->len = 0;
        p->dma = MI_DMA_NOT_USE;
        p->DmaCall = NULL;
        p->flush_threshold_ic = 0x400;
        p->flush_threshold_dc = 0x2400;
#endif
        cardi_rom_base = 0;
        p->priority = CARD_THREAD_PRIORITY_DEFAULT;

        // ���\�[�X���b�N�̏������B
        CARDi_InitResourceLock();

        // �^�X�N�X���b�h���N���B
#if defined(SDK_ARM9)
        // ARM9���͌݊����̂��ߋ��`���̑҂��󂯃X���b�h���̗p�B
        p->callback = NULL;
        p->callback_arg = NULL;
        OS_InitThreadQueue(p->busy_q);
        OS_CreateThread(p->thread.context,
                        CARDi_OldTypeTaskThread, NULL,
                        p->thread.stack + sizeof(p->thread.stack),
                        sizeof(p->thread.stack), p->priority);
        OS_WakeupThreadDirect(p->thread.context);
#else // defined(SDK_ARM9)
        // ARM7���͐V�`���̈��肵���҂��󂯃X���b�h���̗p�B
        CARDi_InitTaskQueue(p->task_q);
        OS_CreateThread(p->thread.context,
                        CARDi_TaskWorkerProcedure, p->task_q,
                        p->thread.stack + sizeof(p->thread.stack),
                        sizeof(p->thread.stack), p->priority);
        OS_WakeupThreadDirect(p->thread.context);
#endif

        // �R�}���h�ʐM�V�X�e�����������B
        CARDi_InitCommand();

        // ROM�h���C�o���������B
        CARDi_InitRom();

        // �J�[�h�u�[�g���̂ݖ������ɃJ�[�h�A�N�Z�X�����B
        // ����ȊO�̋N�����[�h�ŃJ�[�h�A�N�Z�X����]����ꍇ�A
        // �K�C�h���C���ɋL�ڂ��ꂽ����̏����ɏ]�������
        // �����I��CARD_Enable�֐����Ăяo���K�v������B
        if (OS_GetBootType() == OS_BOOTTYPE_ROM)
        {
            CARD_Enable(TRUE);
        }

#if !defined(SDK_SMALL_BUILD)
        //---- for detect pulled out card
        CARD_InitPulledOutCallback();
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_IsAvailable

  Description:  CARD���C�u���������p�\������

  Arguments:    None.

  Returns:      CARD_Init�֐������łɌĂяo����Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL CARD_IsAvailable(void)
{
    CARDiCommon *const p = &cardi_common;
    return (p->flag != 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_IsEnabled

  Description:  �J�[�h�ɃA�N�Z�X�\������

  Arguments:    None.

  Returns:      �J�[�h�ɃA�N�Z�X���錠���������TRUE
 *---------------------------------------------------------------------------*/
BOOL CARD_IsEnabled(void)
{
    return CARDi_EnableFlag;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_CheckEnabled

  Description:  �J�[�h�ɃA�N�Z�X���錠�������邩���肵�A�Ȃ���΋�����~

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_CheckEnabled(void)
{
    if (!CARD_IsEnabled())
    {
        OS_TPanic("NITRO-CARD permission denied");
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_Enable

  Description:  �J�[�h�ɃA�N�Z�X���錠����ݒ�

  Arguments:    enable      �A�N�Z�X���錠���������TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_Enable(BOOL enable)
{
    CARDi_EnableFlag = enable;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetThreadPriority

  Description:  ���C�u���������̔񓯊������p�X���b�h�̗D��x���擾

  Arguments:    None.

  Returns:      �����X���b�h�̗D��x
 *---------------------------------------------------------------------------*/
u32 CARD_GetThreadPriority(void)
{
    CARDiCommon *const p = &cardi_common;
    SDK_ASSERT(CARD_IsAvailable());

    return p->priority;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_SetThreadPriority

  Description:  ���C�u���������̔񓯊������p�X���b�h�̗D��x��ݒ�

  Arguments:    None.

  Returns:      �ݒ�O�̓����X���b�h�̗D��x
 *---------------------------------------------------------------------------*/
u32 CARD_SetThreadPriority(u32 prior)
{
    CARDiCommon *const p = &cardi_common;
    SDK_ASSERT(CARD_IsAvailable());

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        u32     ret = p->priority;
        SDK_ASSERT((prior >= OS_THREAD_PRIORITY_MIN) && (prior <= OS_THREAD_PRIORITY_MAX));
        p->priority = prior;
        (void)OS_SetThreadPriority(p->thread.context, p->priority);
        (void)OS_RestoreInterrupts(bak_psr);
        return ret;
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetResultCode

  Description:  �Ō�ɌĂяo����CARD�֐��̌��ʂ��擾

  Arguments:    None.

  Returns:      �Ō�ɌĂяo����CARD�֐��̌���
 *---------------------------------------------------------------------------*/
CARDResult CARD_GetResultCode(void)
{
    CARDiCommon *const p = &cardi_common;
    SDK_ASSERT(CARD_IsAvailable());

    return p->cmd->result;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomHeader

  Description:  ���ݑ}������Ă���J�[�h��ROM�w�b�_�����擾

  Arguments:    None.

  Returns:      CARDRomHeader�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
const u8 *CARD_GetRomHeader(void)
{
    return (const u8 *)HW_CARD_ROM_HEADER;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetOwnRomHeader

  Description:  ���ݎ��s���Ă���v���O������ROM�w�b�_�����擾�B

  Arguments:    None.

  Returns:      CARDRomHeader�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
const CARDRomHeader *CARD_GetOwnRomHeader(void)
{
    return (const CARDRomHeader *)HW_ROM_HEADER_BUF;
}

#if defined(SDK_TWL)

/*---------------------------------------------------------------------------*
  Name:         CARD_GetOwnRomHeaderTWL

  Description:  ���ݎ��s���Ă���v���O������TWL-ROM�w�b�_�����擾�B

  Arguments:    None.

  Returns:      CARDRomHeaderTWL�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
const CARDRomHeaderTWL *CARD_GetOwnRomHeaderTWL(void)
{
    return (const CARDRomHeaderTWL *)HW_TWL_ROM_HEADER_BUF;
}

#endif // SDK_TWL

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCacheFlushThreshold

  Description:  �L���b�V���������𕔕��I�ɍs�����S�̂֍s������臒l���擾

  Arguments:    icache            ���߃L���b�V���̖�����臒l���i�[����|�C���^
                                  NULL�ł���Ζ��������
                dcache            �f�[�^�L���b�V���̖�����臒l���i�[����|�C���^
                                  NULL�ł���Ζ��������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_GetCacheFlushThreshold(u32 *icache, u32 *dcache)
{
#if defined(SDK_ARM9)
    SDK_ASSERT(CARD_IsAvailable());
    if (icache)
    {
        *icache = cardi_common.flush_threshold_ic;
    }
    if (dcache)
    {
        *dcache = cardi_common.flush_threshold_dc;
    }
#else
    (void)icache;
    (void)dcache;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CARD_SetCacheFlushThreshold

  Description:  �L���b�V���������𕔕��I�ɍs�����S�̂֍s������臒l��ݒ�

  Arguments:    icache            ���߃L���b�V���̖�����臒l
                dcache            �f�[�^�L���b�V���̖�����臒l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_SetCacheFlushThreshold(u32 icache, u32 dcache)
{
#if defined(SDK_ARM9)
    SDK_ASSERT(CARD_IsAvailable());
    cardi_common.flush_threshold_ic = icache;
    cardi_common.flush_threshold_dc = dcache;
#else
    (void)icache;
    (void)dcache;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CARD_LockRom

  Description:  ROM�A�N�Z�X�̂��߂ɃJ�[�h�o�X�����b�N����

  Arguments:    lock id.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_LockRom(u16 lock_id)
{
    SDK_ASSERT(CARD_IsAvailable());

    /* ���\�[�X�����b�N */
    CARDi_LockResource(lock_id, CARD_TARGET_ROM);
    /* �J�[�h�o�X�����b�N */
    {
        // ARM7�����b�N���Ă���ꍇ��CPU�������ԃu���b�L���O���Ă��܂�����
        // �����Ƃ�������V�J�E���g�A���[�����g�p���ăX���[�v����B
#define CARD_USING_SLEEPY_LOCK
#ifdef CARD_USING_SLEEPY_LOCK
        CARDEventListener   el[1];
        CARDi_InitEventListener(el);
        CARDi_SetEventListener(el, CARDi_LockBusCondition, &lock_id);
        CARDi_WaitForEvent(el);
#else
        // ���쌟�؂��ς߂Εs�v�B
        (void)OS_LockCard(lock_id);
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockRom

  Description:  ���b�N�����J�[�h�o�X���������

  Arguments:    lock id which is used by CARD_LockRom().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_UnlockRom(u16 lock_id)
{
    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(cardi_common.lock_target == CARD_TARGET_ROM);

    /* �J�[�h�o�X���A�����b�N */
    {
        (void)OS_UnlockCard(lock_id);
    }
    /* ���\�[�X���A�����b�N */
    CARDi_UnlockResource(lock_id, CARD_TARGET_ROM);

}

/*---------------------------------------------------------------------------*
  Name:         CARD_LockBackup

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�̂��߂ɃJ�[�h�o�X�����b�N����

  Arguments:    lock_id     OS_GetLockID�֐��Ŋm�ۂ����r������pID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_LockBackup(u16 lock_id)
{
    SDK_ASSERT(CARD_IsAvailable());

    /* �v���Z�b�T����ROM/�o�b�N�A�b�v�r������� */
    CARDi_LockResource(lock_id, CARD_TARGET_BACKUP);
    /* ��� ARM7 ���A�N�Z�X */
#if defined(SDK_ARM7)
    (void)OS_LockCard(lock_id);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockBackup

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�̂��߂Ƀ��b�N�����J�[�h�o�X���������

  Arguments:    lock_id     CARD_LockBackup�֐��Ŏg�p�����r������pID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_UnlockBackup(u16 lock_id)
{
    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(cardi_common.lock_target == CARD_TARGET_BACKUP);

#if defined(SDK_ARM9)
    /* �񓯊������̎��s���Ƀo�X��������悤�Ƃ�����u���b�L���O */
    if (!CARD_TryWaitBackupAsync())
    {
        OS_TWarning("called CARD_UnlockBackup() during backup asynchronous operation. (force to wait)\n");
        (void)CARD_WaitBackupAsync();
    }
#endif // defined(SDK_ARM9)

    /* ��� ARM7 ���A�N�Z�X */
#if defined(SDK_ARM7)
    (void)OS_UnlockCard(lock_id);
#endif
    /* ���\�[�X���A�����b�N */
    CARDi_UnlockResource(lock_id, CARD_TARGET_BACKUP);
}

