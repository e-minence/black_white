/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     fs_hook.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/

#include <nitro/fs/archive.h>
#include <nitro/fs/hook.h>
#include <nitro/card/common.h>
#include <nitro/std/string.h>
#ifdef SDK_TWL
#include <twl/fatfs/common/types.h>
#endif // SDK_TWL

/*---------------------------------------------------------------------------*/
/* variables */

// �C�x���g�t�b�N�Ǘ����
typedef struct FSEventHookContext
{
    FSEventHook     *chain;
    CARDHookContext  hook[1];
}
FSEventHookContext;

static FSEventHookContext FSiEventHookStaticWork[1];


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FSi_SystemEvents

  Description:  �V�X�e���C�x���g�Ď��R�[���o�b�N�B

  Arguments:    userdata         �C�ӂ̃��[�U��`�f�[�^
                event            ���������C�x���g
                argument         �C�x���g���Ƃ̈���

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_SystemEvents(void *userdata, CARDEvent event, void *argument)
{
    FSEventHookContext *context = (FSEventHookContext*)userdata;
    FSArchive  *target = NULL;
    FSEvent     fsevent = FS_EVENT_NONE;
    (void)argument;
    // �V�X�e���S�̂̃��f�B�A�C�x���g���e�A�[�J�C�u�̃C�x���g�֕ϊ�����B
    if (event == CARD_EVENT_SLOTRESET)
    {
        target = FS_FindArchive("rom", 3);
        fsevent = FS_EVENT_MEDIA_INSERTED;
    }
    else if (event == CARD_EVENT_PULLEDOUT)
    {
        target = FS_FindArchive("rom", 3);
        fsevent = FS_EVENT_MEDIA_REMOVED;
    }
#ifdef SDK_TWL
    else if (event == FATFS_EVENT_SD_INSERTED)
    {
        target = FS_FindArchive("sdmc", 4);
        fsevent = FS_EVENT_MEDIA_INSERTED;
    }
    else if (event == FATFS_EVENT_SD_REMOVED)
    {
        target = FS_FindArchive("sdmc", 4);
        fsevent = FS_EVENT_MEDIA_REMOVED;
    }
#endif // SDK_TWL
    if (fsevent != FS_EVENT_NONE)
    {
        FSEventHook   **pp = &context->chain;
        while (*pp != NULL)
        {
            FSEventHook    *hook = *pp;
            if ((hook->arc == target) && (hook->callback != NULL))
            {
                (*hook->callback)(hook->userdata, fsevent, argument);
            }
            if (*pp == hook)
            {
                pp = &(*pp)->next;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_RegisterEventHook

  Description:  �A�[�J�C�u�����C�x���g�p�t�b�N��o�^����B

  Arguments:    arcname          �o�^�Ώۂ̃A�[�J�C�u
                hook             �o�^����FSEventHook�\����
                callback         �C�x���g�������̃R�[���o�b�N�֐�
                userdata         �C�ӂ̃��[�U��`�f�[�^ (�s�v�ł����NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_RegisterEventHook(const char *arcname, FSEventHook *hook, FSEventFunction callback, void *userdata)
{
    FSEventHookContext *context = FSiEventHookStaticWork;
    OSIntrMode  bak = OS_DisableInterrupts();
    FSArchive  *arc = FS_FindArchive(arcname, STD_GetStringLength(arcname));
    if (arc)
    {
        // �����I�Ɏg�p����Ȃ��ꍇ�̓t�b�N�@�\�������N����Ȃ��悤�ɔz���B
        if (!context->hook->callback)
        {
            CARDi_RegisterHook(context->hook, FSi_SystemEvents, context);
        }
        hook->callback = callback;
        hook->userdata = userdata;
        hook->arc = arc;
        hook->next = context->chain;
        context->chain = hook;
    }
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnregisterEventHook

  Description:  �o�^�����A�[�J�C�u�����C�x���g�p�t�b�N����������B

  Arguments:    hook             �o�^����FSEventHook�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_UnregisterEventHook(FSEventHook *hook)
{
    FSEventHookContext *context = FSiEventHookStaticWork;
    OSIntrMode      bak = OS_DisableInterrupts();
    FSEventHook   **pp;
    for (pp = &context->chain; *pp; pp = &(*pp)->next)
    {
        if (*pp == hook)
        {
            *pp = (*pp)->next;
            break;
        }
    }
    (void)OS_RestoreInterrupts(bak);
}
