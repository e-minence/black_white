/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_hook.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-04-21#$
  $Rev: 5625 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/


#include <nitro/card/common.h>


/*---------------------------------------------------------------------------*/
/* variables */

static CARDHookContext *CARDiHookChain = NULL;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_RegisterHook

  Description:  CARD���C�u�����̓����C�x���g�t�b�N��o�^

  Arguments:    hook        �o�^�Ɏg�p����t�b�N�\����
                callback    �C�x���g�������ɌĂяo���R�[���o�b�N�֐�
                arg         �R�[���o�b�N����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_RegisterHook(CARDHookContext *hook, CARDHookFunction callback, void *arg)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    hook->callback = callback;
    hook->userdata = arg;
    hook->next = CARDiHookChain;
    CARDiHookChain = hook;
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_UnregisterHook

  Description:  CARD���C�u�����̓����C�x���g�t�b�N������

  Arguments:    hook        �o�^�Ɏg�p�����t�b�N�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_UnregisterHook(CARDHookContext *hook)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    CARDHookContext **pp;
    for (pp = &CARDiHookChain; *pp; pp = &(*pp)->next)
    {
        if (*pp == hook)
        {
            *pp = (*pp)->next;
            break;
        }
    }
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_NotifyEvent

  Description:  CARD���C�u�����̓����C�x���g��ʒm

  Arguments:    event       ���������C�x���g
                arg         �C�x���g���Ƃ̈���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_NotifyEvent(CARDEvent event, void *arg)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    CARDHookContext **pp = &CARDiHookChain;
    while (*pp)
    {
        CARDHookContext *hook = *pp;
        if (hook->callback)
        {
            (*hook->callback)(hook->userdata, event, arg);
        }
        // �R�[���o�b�N���Ńt�b�N���������P�[�X���l��
        if (*pp == hook)
        {
            pp = &(*pp)->next;
        }
    }
    (void)OS_RestoreInterrupts(bak);
}
