#if 0
/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MI
  File:     mi_card.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/


#include <nitro/os.h>
#include <nitro/mi/card.h>


/********************************************************************/
/* �ϐ� */

// �J�[�h�r�������p�ϐ�.
typedef struct
{
    BOOL    is_init;                   // �������ς�.
    u16     lock_id;                   //���b�N�p ID.
    u16     padding;
    MIDmaCallback dma_callback;
}
MIi_CardParam;

static MIi_CardParam mii_card_param;


/********************************************************************/
/* �֐� */

// MI �o�R CARD �A�N�Z�X�̏�����.
static void MIi_InitCard(void)
{
    MIi_CardParam *const p = &mii_card_param;
    OSIntrMode bak_psr = OS_DisableInterrupts();

    if (!p->is_init)
    {
        s32     lock_id = OS_GetLockID();

#ifndef	SDK_FINALROM
        if (lock_id < 0)
        {
            OS_Panic("Invalid lock ID.");
        }
#endif
        p->is_init = TRUE;
        p->lock_id = (u16)lock_id;
    }
    (void)OS_RestoreInterrupts(bak_psr);
}

// �v���Z�b�T�r���Ńf�o�C�X���m��.
void MIi_LockCard(void)
{
    // ���֐����l��, �K�v�Ȃ炱���ŏ�����.
    MIi_InitCard();
    CARD_LockRom(mii_card_param.lock_id);
}

// �v���Z�b�T�r���Ńf�o�C�X�����.
void MIi_UnlockCard(void)
{
    SDK_ASSERT(mii_card_param.is_init);
    CARD_UnlockRom(mii_card_param.lock_id);
}

// ���[�U�R�[���o�b�N�̑O�� MI ���g�̃��b�N���͂���.
static void MIi_OnAsyncEnd(void *arg)
{
    MIi_UnlockCard();
    {
        MIDmaCallback func = mii_card_param.dma_callback;
        mii_card_param.dma_callback = NULL;
        if (func)
            (*func) (arg);
    }
}

// �J�[�h�ǂݍ���. (�񓯊�)
void MIi_ReadCardAsync(u32 dmaNo, const void *src, void *dst, u32 size,
                       MIDmaCallback callback, void *arg)
{
    MIi_LockCard();
    mii_card_param.dma_callback = callback;
    (void)CARD_ReadRomAsync(dmaNo, src, dst, size, MIi_OnAsyncEnd, arg);
}

#endif //#if 0
