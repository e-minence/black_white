/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot
  File:     main.c

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

//--------------------------------------------------------------------
// �}���`�u�[�g���̃`�����l������T���v��
// 
// �d�g�g�p���̌v���͖���IDLE��Ԃł̂ݎg�p�\�Ȃ��߁A
// �}���`�u�[�g���ɂ�MB�J�n�O�ɑO�����Ƃ��ă`�����l���̌��������K�v��
// ����܂��B
//
// ���̃T���v����MBM_MeasureChannel()�́A�����p���[OFF��ԂŃR�[������ƁA
// �����ōœK�ȃ`�����l���𒲍����Ė����p���[OFF��Ԃɖ߂��Ă���
// �R�[���o�b�N�Ō��ʂ�Ԃ��Ƃ�������ɂȂ��Ă��܂��B
// 
// (��NITRO-SDK2.0rc2�Œǉ����ꂽMB_StartParentFromIdle,
//    MB_EndToIdle�𗘗p������@������܂�)
//--------------------------------------------------------------------


#include <nitro.h>

#include "mb_measure_channel.h"


static void my_callback(MBMCallback * cb);
static vu16 channel_setting = 0;
static int prog_state;


/******************************************************************************/
/* function */

/* �u�u�����N���荞�ݏ��� */
static void VBlankIntr(void)
{
    //---- ���荞�݃`�F�b�N�t���O
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/* ���C�� */
void NitroMain()
{
    /* OS ������ */
    OS_Init();
    OS_InitThread();
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();

    /* GX ������ */
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    {                                  /* ���������蓖�Ă̏����� */
        OSHeapHandle heapHandle;       // Heap�n���h��
        enum
        { MAIN_HEAP_SIZE = 0x80000 };
        void   *heapStart, *nstart;
        nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 16);
        OS_SetMainArenaLo(nstart);
        heapStart = OS_AllocFromMainArenaLo((u32)MAIN_HEAP_SIZE, 32);
        heapHandle =
            OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
        if (heapHandle < 0)
        {
            OS_Panic("ARM9: Fail to create heap...\n");
        }
        heapHandle = OS_SetCurrentHeap(OS_ARENA_MAIN, heapHandle);
    }

    /* V�u�����N�����ݒ� */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* �\���J�n */
    GX_DispOn();
    GXS_DispOn();
    G2_SetBG0Offset(0, 0);

    channel_setting = 0;

    {
        void   *wm_sys_buf_ptr = OS_Alloc(WM_SYSTEM_BUF_SIZE);
        MBM_MeasureChannel((u8 *)wm_sys_buf_ptr, my_callback);
        while (channel_setting == 0)
        {
            OS_WaitVBlankIntr();
        }
        OS_Free(wm_sys_buf_ptr);
    }

    /* ���C�����[�v */
    for (;;)
    {
        OS_WaitVBlankIntr();
    }
}


static void my_callback(MBMCallback * cb)
{
    channel_setting = 1;
    if (cb->errcode == MBM_MEASURE_SUCCESS)
    {
        OS_TPrintf("measure channel = %d\n", cb->channel);
    }
    else
    {
        OS_TPrintf("get measure channel fail\n");
    }
}
