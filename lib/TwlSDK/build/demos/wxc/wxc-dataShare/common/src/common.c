/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - wxc-dataShare
  File:     common.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-15#$
  $Rev: 2414 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "common.h"
#include "disp.h"

void VBlankIntr(void);

/*---------------------------------------------------------------------------*
  Name:         CommonInit

  Description:  共通初期化関数

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CommonInit(void)
{
    /* OS 初期化 */
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();

    /* GX 初期化 */
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* Vブランク割込設定 */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)GX_VBlankIntr(TRUE);

    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    {                                  /* FS 初期化 */
        static u32 fs_tablework[0x100 / 4];
        FS_Init(FS_DMA_NOT_USE);
        (void)FS_LoadTable(fs_tablework, sizeof(fs_tablework));
    }
}


/*---------------------------------------------------------------------------*
  Name:         InitAllocateSystem

  Description:  メインメモリ上のアリーナにてメモリ割当てシステムを初期化する。

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void InitAllocateSystem(void)
{
    void *tempLo;
    OSHeapHandle hh;

    // OS_Initは呼ばれているという前提
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}
