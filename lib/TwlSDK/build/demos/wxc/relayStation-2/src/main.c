/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - relayStation-2
  File:     main.c

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


/*---------------------------------------------------------------------------*
    �����ɂ�邷�ꂿ�����ʐM���s���T���v���ł��B
    �q�@���[�h100%�ɂȂ�A���p���Ƃ��Ă̓���ƂȂ�܂��B
    GGID ���ЂƂ����Ή���, �o�^�f�[�^�̈����𓮓I�ɐݒ�ł���_��
    relayStation-1 �ƈقȂ�܂��B

    [�e���[�h�̐���]

    BROADCAST : ���݂̓o�^�ς݃f�[�^���J��Ԃ��g�p��������.
                (�����f�[�^���������ɑ��B���邱�ƂɂȂ�܂�)

    RELAY     : �����Ŏ�M�����f�[�^��V���ȑ��M�f�[�^�Ƃ��čēo�^��������.
                (�{���́u���p���v�̊�{�I�ȓ��삪����ɂ�����܂�)

    END       : �����������̂��I������.
                (���݌������Ȃ炻�̊����܂ł͏������p�����܂�)

 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/wxc.h>

#include "user.h"
#include "dispfunc.h"


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WaitNextFrame

  Description:  ���̃t���[����҂�, ���񉟉����ꂽ�L�[�����o

  Arguments:    None.

  Returns:      ���񉟉����ꂽ�L�[�̃r�b�g�W��.
 *---------------------------------------------------------------------------*/
static u16 WaitNextFrame(void)
{
    static u16 cont = 0xffff;          /* IPL ���ł� A �{�^�������΍� */
    u16     keyset, trigger;

    /* ���̃t���[����҂� */
    OS_WaitVBlankIntr();
    /* ���񉟉����ꂽ�L�[�����o */
    keyset = PAD_Read();
    trigger = (u16)(keyset & (keyset ^ cont));
    cont = keyset;

    return trigger;
}

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem

  Description:  ���C����������̃A���[�i�ɂă����������ăV�X�e��������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitializeAllocateSystem(void)
{
    void   *tempLo;
    OSHeapHandle hh;

    OS_Printf(" arena lo = %08x\n", OS_GetMainArenaLo());
    OS_Printf(" arena hi = %08x\n", OS_GetMainArenaHi());

    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃n���h��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    /* BG�X�N���[���X�V */
    BgScrSetVblank();
    /* ���荞�݃`�F�b�N�t���O */
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    int     cursor;

    /* �e�평���� */
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    RTC_Init();

    /* �`��ݒ菉���� */
    {
        GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
        MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
        (void)GX_DisableBankForLCDC();
        MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
        MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
        MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
        MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
        BgInitForPrintStr();
        GX_SetVisiblePlane(GX_PLANEMASK_BG0);
        GXS_SetVisiblePlane(GX_PLANEMASK_BG0);
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
    G2S_SetBG0Offset(0, 0);

    /* ���������蓖�ď����� */
    InitializeAllocateSystem();

    /* �T���v���f���J�n */
    OS_TPrintf("relayStation-2 demo started.\n");

    /* �f���N���ƃ��Z�b�g�̖������[�v */
    for (;;)
    {
        /* A �{�^�������҂� */
        while ((WaitNextFrame() & PAD_BUTTON_A) == 0)
        {
            BgClear();
            BgPutString(3, 1, WHITE, "Press A to start");
        }

        /* WXC �����������ă��[�U�ݒ� (������Ԃ̓u���[�h�L���X�g) */
        station_is_relay = FALSE;
        cursor = 0;
        User_Init();

        /* ���ꂿ�����ʐM���C�����[�v */
        while (WXC_GetStateCode() != WXC_STATE_END)
        {
            u16     key = WaitNextFrame();
            enum
            { MODE_BROADCAST, MODE_RELAY, MODE_STOP, MODE_MAX };

            /* ��ʕ\�� */
            {
                const char *(mode[MODE_MAX]) =
                {
                "BROADCAST", "RELAY", "STOP",};
                const char *status = "";
                char    tmp[32];
                u8      mac[6];
                int     i;

                BgClear();
                /* ���݂� WXC �X�e�[�^�X */
                switch (WXC_GetStateCode())
                {
                case WXC_STATE_END:
                    status = "WXC_STATE_END";
                    break;
                case WXC_STATE_ENDING:
                    status = "WXC_STATE_ENDING";
                    break;
                case WXC_STATE_READY:
                    status = "WXC_STATE_READY";
                    break;
                case WXC_STATE_ACTIVE:
                    status = "WXC_STATE_ACTIVE";
                    break;
                default:
                    (void)OS_SPrintf(tmp, "?(%d)", WXC_GetStateCode());
                    status = tmp;
                    break;
                }
                BgPrintf(2, 2, WHITE, "status = %s", status);
                /* ���j���[�I�� */
                BgPrintf(4, 5 + (station_is_relay ? 1 : 0), WHITE, "*");
                for (i = 0; i < MODE_MAX; ++i)
                {
                    BgPrintf(3, 5 + i, WHITE, "%c", (cursor == i) ? '>' : ' ');
                    BgPrintf(6, 5 + i, (cursor == i) ? YELLOW : WHITE, "%s", mode[i]);
                }
                /* ���ݓo�^����Ă���f�[�^ */
                BgPrintf(0, 12, GREEN, "------- CURRENT DATA -------");
                BgPrintf(2, 14, WHITE, "MAC = ");
                for (i = 0; i < 6; ++i)
                {
                    BgPrintf(8 + i * 3, 14, WHITE, "%02X", send_data_owner[i]);
                }
                BgPrintf(2, 16, WHITE, "DATA = ");
                for (i = 0; i < 8; ++i)
                {
                    BgPrintf(9 + i * 3, 16, WHITE, "%02X", send_data[i]);
                }
                /* �Q�l�܂łɎ����� MAC �A�h���X */
                BgPrintf(2, 10, WHITE, "OWN MAC = ");
                OS_GetMacAddress(mac);
                for (i = 0; i < 6; ++i)
                {
                    BgPrintf(12 + i * 3, 10, WHITE, "%02X", mac[i]);
                }
            }

            /* �L�[���� */
            if (((key & PAD_KEY_DOWN) != 0) && (++cursor >= MODE_MAX))
            {
                cursor -= MODE_MAX;
            }
            if (((key & PAD_KEY_UP) != 0) && (--cursor < 0))
            {
                cursor += MODE_MAX;
            }
            if ((key & PAD_BUTTON_A) != 0)
            {
                switch (cursor)
                {
                case MODE_BROADCAST:
                    station_is_relay = FALSE;
                    break;
                case MODE_RELAY:
                    station_is_relay = TRUE;
                    break;
                case MODE_STOP:
                    (void)WXC_End();
                    break;
                }
            }
        }
    }
}


/*----------------------------------------------------------------------------*

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
