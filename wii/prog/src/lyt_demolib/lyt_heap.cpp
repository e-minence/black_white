/*---------------------------------------------------------------------------*
  Project:  NW4R - LayoutLib - build - demos - lyt - demolib
  File:     pad.cpp

  Copyright (C)2006-2008 Nintendo  All Rights Reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.2 $
 *---------------------------------------------------------------------------*/

#include "lyt_demolib/lyt_heap.h"
#include <demo.h>
#include <nw4r/misc.h>
#include <nw4r/ut.h>

const u32 HEAP_SIZE_OF_SDK_DEMO = 4 * 1024 * 1024;

namespace demo
{

MEMHeapHandle       gDemoHeap1;
MEMHeapHandle       gDemoHeap2;

/*-------------------------------------------------------------------------*
  Name:         InitHeap

  Description:  �f���Ŏg�p����q�[�v�����������܂��B

  Arguments:    �Ȃ��B

  Returns:      MEM1����쐬�����g���q�[�v�̃n���h���B
 *-------------------------------------------------------------------------*/
MEMHeapHandle
InitHeap()
{
    // MEM1
    // SDK DEMO���C�u�����p�ɃA���[�i�c���A�A���[�i�̏�ʂ�胁�������m�ۂ��܂��B
    {
        const u32 arenaSize = u32(nw4r::ut::GetOffsetFromPtr(OSGetMEM1ArenaLo(), OSGetMEM1ArenaHi()));
        NW4R_ASSERT(arenaSize > HEAP_SIZE_OF_SDK_DEMO);

        const u32 heapSize = arenaSize - HEAP_SIZE_OF_SDK_DEMO;
        void *const heapMem = OSAllocFromMEM1ArenaHi(heapSize, 32);

        gDemoHeap1 = MEMCreateExpHeapEx(
                                heapMem,
                                heapSize,
                                MEM_HEAP_OPT_DEBUG_FILL);
        NW4R_ASSERT(gDemoHeap1 != MEM_HEAP_INVALID_HANDLE);

        NW4R_LOG("demo heap mem1:  %p - %p (%d bytes)\n", heapMem, reinterpret_cast<u32>(heapMem) + heapSize - 1, heapSize);
   }

    // MEM2
    {
        void *const arenaLo = OSGetMEM2ArenaLo();
        void *const arenaHi = OSGetMEM2ArenaHi();
        const u32 heapSize  = u32(nw4r::ut::GetOffsetFromPtr(arenaLo, arenaHi));

        gDemoHeap2 = MEMCreateExpHeapEx(
                                arenaLo,
                                heapSize,
                                MEM_HEAP_OPT_DEBUG_FILL );
        NW4R_ASSERT(gDemoHeap2 != MEM_HEAP_INVALID_HANDLE);

        OSSetMEM2ArenaLo(arenaHi);

        NW4R_LOG("demo heap mem2:  %p - %p (%d bytes)\n", arenaLo, reinterpret_cast<u32>(arenaLo) + heapSize - 1, heapSize);
    }

    return gDemoHeap1;
}

}   // namespace demo
