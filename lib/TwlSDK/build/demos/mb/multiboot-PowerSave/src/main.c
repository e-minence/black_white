/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot-PowerSave
  File:     main.c

  Copyright 2006-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/


#ifdef SDK_TWL
#include	<twl.h>
#else
#include	<nitro.h>
#endif


#include "common.h"
#include "dispfunc.h"

/*
 * �ȓd�̓��[�h�𖾎��I�ɐݒ�ύX����DS�_�E�����[�h�e�@�̃T���v��.
 *
 * MB���C�u�����̃T���v���̓��C�����X�@�\���g�����߁A
 * �����ʐM��(�������邢�͗L��)�ǂ����̊J���@�ނ𕡐��K�v�Ƃ��܂��B
 * $TwlSDK/bin/ARM9-TS/Rom/ �f�B���N�g���ɂ���
 * mb_child_NITRO.srl, mb_child_TWL.srl �v���O�������ŏI���@�ɂ�����DS�_�E�����[�h�q�@�Ɠ�����
 * �@�\��񋟂���T���v���ɂȂ��Ă��܂��̂ŁA
 * ���̃o�C�i�����T���v���v���O�����Ɠ������@�ő��̋@�ނɓǂݍ��݁A
 * �ꏏ�Ɏ��s���Ă��������B
 */


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �`��X�V�p��V�u�����N���荞�݃n���h��.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    BgUpdate();
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �A�v���P�[�V�������C���G���g��.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    /* OS ������ */
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    FX_Init();

    /* GX ������ */
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    /* ���������蓖�Ă̏����� */
    {
        OSHeapHandle hh;
        static const u32 MAIN_HEAP_SIZE = 0x80000;
        void   *heapStart;
        void   *nstart = OS_InitAlloc(OS_ARENA_MAIN,
                                      OS_GetMainArenaLo(), OS_GetMainArenaHi(), 16);
        OS_SetMainArenaLo(nstart);
        heapStart = OS_AllocFromMainArenaLo(MAIN_HEAP_SIZE, 32);
        hh = OS_CreateHeap(OS_ARENA_MAIN, heapStart, (void *)((u32)heapStart + MAIN_HEAP_SIZE));
        if (hh < 0)
        {
            OS_Panic("ARM9: Fail to create heap...\n");
        }
        (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    }
    
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    
    /* FS ������ */
    FS_Init(FS_DMA_NOT_USE);
    {
        u32     need_size = FS_GetTableSize();
        void   *p_table = OS_Alloc(need_size);
        SDK_ASSERT(p_table != NULL);
        (void)FS_LoadTable(p_table, need_size);
    }

    /* �`��ݒ菉���� */
    {
        GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
        MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
        (void)GX_DisableBankForLCDC();
        MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
        MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
        MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
        MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
        BgInit();
    }

    /* V�u�����N�����ݒ� */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
//    (void)OS_EnableIrq();
//    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    /* ���C�����[�v */
    for (;;)
    {
        MenuMode();
        ParentMode();
    }

}
