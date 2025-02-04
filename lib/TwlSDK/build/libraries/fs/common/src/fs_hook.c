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

// イベントフック管理情報
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

  Description:  システムイベント監視コールバック。

  Arguments:    userdata         任意のユーザ定義データ
                event            発生したイベント
                argument         イベントごとの引数

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_SystemEvents(void *userdata, CARDEvent event, void *argument)
{
    FSEventHookContext *context = (FSEventHookContext*)userdata;
    FSArchive  *target = NULL;
    FSEvent     fsevent = FS_EVENT_NONE;
    (void)argument;
    // システム全体のメディアイベントを各アーカイブのイベントへ変換する。
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

  Description:  アーカイブ内部イベント用フックを登録する。

  Arguments:    arcname          登録対象のアーカイブ
                hook             登録するFSEventHook構造体
                callback         イベント発生時のコールバック関数
                userdata         任意のユーザ定義データ (不要であればNULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_RegisterEventHook(const char *arcname, FSEventHook *hook, FSEventFunction callback, void *userdata)
{
    FSEventHookContext *context = FSiEventHookStaticWork;
    OSIntrMode  bak = OS_DisableInterrupts();
    FSArchive  *arc = FS_FindArchive(arcname, STD_GetStringLength(arcname));
    if (arc)
    {
        // 明示的に使用されない場合はフック機能がリンクされないように配慮。
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

  Description:  登録したアーカイブ内部イベント用フックを解除する。

  Arguments:    hook             登録したFSEventHook構造体

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
