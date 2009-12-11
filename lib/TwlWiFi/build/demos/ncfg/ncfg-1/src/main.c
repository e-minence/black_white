/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NCFG - demos - ncfg-1
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    NVRAM ��̃l�b�g���[�N�ݒ�����̒l�ŏ��������Ȃ���A
    ���̌��ʂ��_���v����T���v���v���O�����ł��B
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi.h>
#include <nitroWiFi/ncfg.h>

static void MainLoop(void);
static void VBlankIntr(void);

static void ReadNCFG(s32 index);
static void WriteNCFG(s32 index);

void NitroMain()
{
    OS_Init();
    FX_Init();
    GX_Init();

    GX_DispOff();
    GXS_DispOff();

    // V �u�����N���荞�ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);

    // ���荞�݋���
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    // ��ʕ\���J�n
    OS_WaitVBlankIntr();
    GX_DispOn();
    GXS_DispOn();

    /* �L�[���͏��擾�̋�Ăяo��(IPL �ł� A �{�^�������΍�) */
    (void)PAD_Read();

    MainLoop();

    OS_Terminate();
}

static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static void MainLoop(void)
{
    ReadNCFG(NCFG_SLOT_1);
    ReadNCFG(NCFG_SLOT_2);
    ReadNCFG(NCFG_SLOT_3);
    ReadNCFG(NCFG_SLOT_RESERVED);
    WriteNCFG(NCFG_SLOT_1);
    WriteNCFG(NCFG_SLOT_2);
    WriteNCFG(NCFG_SLOT_3);
    WriteNCFG(NCFG_SLOT_RESERVED);
    ReadNCFG(NCFG_SLOT_1);
    ReadNCFG(NCFG_SLOT_2);
    ReadNCFG(NCFG_SLOT_3);
    ReadNCFG(NCFG_SLOT_RESERVED);

    ReadNCFG(NCFG_SLOT_EX_1);
    ReadNCFG(NCFG_SLOT_EX_2);
    ReadNCFG(NCFG_SLOT_EX_3);
    WriteNCFG(NCFG_SLOT_EX_1);
    WriteNCFG(NCFG_SLOT_EX_2);
    WriteNCFG(NCFG_SLOT_EX_3);
    ReadNCFG(NCFG_SLOT_EX_1);
    ReadNCFG(NCFG_SLOT_EX_2);
    ReadNCFG(NCFG_SLOT_EX_3);
}

static void ReadNCFG(s32 index)
{
    static u8 buf[512];
    s32 result;
    result = NCFG_ReadBackupMemory(buf, sizeof(buf), index);
    OS_TPrintf("NCFG_ReadBackupMemory(%d): %d\n", index, result);
    if ( result >= 0 )
    {
        OS_TPrintfEx("% *.16b", result, buf);
        OS_TPrintf("\n");
    }
}

static void WriteNCFG(s32 index)
{
    static u8 buf[512];
    s32 result;

    MI_CpuFill8(buf, (u8)0 /*index*/, sizeof(buf));

    result = NCFG_WriteBackupMemory(index, buf, sizeof(buf));
    OS_TPrintf("NCFG_WriteBackupMemory(%d): %d\n", index, result);
}
