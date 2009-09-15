/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - relayStation-1
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
    �q�@���[�h100%�ɂȂ�A���p���Ɠ�������ƂȂ�܂��B
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <nitro/wxc.h>
#include "user.h"
#include "common.h"
#include "dispfunc.h"


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void ModeSelect(void);          // �e�@/�q�@ �I�����
static void ModeError(void);           // �G���[�\�����
static void VBlankIntr(void);          // V�u�����N�����݃n���h��

// �ėp�T�u���[�`��
static void InitializeAllocateSystem(void);

static void Menu(void);

/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static s32 gFrame;                     // �t���[���J�E���^

/*---------------------------------------------------------------------------*
    �O���ϐ���`
 *---------------------------------------------------------------------------*/
int passby_endflg;
int menu_flg;

BOOL keep_flg;

BOOL passby_ggid[MENU_MAX];     // ���ꂿ����������GGID

u16 keyData;

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  �������y�у��C�����[�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    int i;
    
    // �e�평����
    OS_Init();
    OS_InitTick();

    OS_InitAlarm();
    
    FX_Init();

    {                                  /* �`��ݒ菉���� */
        GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
        MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
        (void)GX_DisableBankForLCDC();
        MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
        MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
        MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
        MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);
        BgInitForPrintStr();
        GX_SetBankForOBJ(GX_VRAM_OBJ_128_B);
        GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
        GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
        GXS_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
        GX_LoadOBJ(sampleCharData, 0, sizeof(sampleCharData));
        GX_LoadOBJPltt(samplePlttData, 0, sizeof(samplePlttData));
        MI_DmaFill32(3, oamBak, 0xc0, sizeof(oamBak));
    }

    /* V�u�����N�����ݒ� */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrqMask(OS_IE_FIFO_RECV);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);

    // RTC������
    RTC_Init();

    /* �\���J�n */
    GX_DispOn();
    GXS_DispOn();
    G2_SetBG0Offset(0, 0);
    G2S_SetBG0Offset(0, 0);

    /* ���������蓖�ď����� */
    InitializeAllocateSystem();

    // �f�o�b�O������o��
    OS_Printf("ARM9: WM simple demo started.");
    
    
    passby_endflg = 0;
    menu_flg = MENU_MIN;
    keep_flg = TRUE;
    
    // ���ꂿ�����\��GGID�͏����l�ł͑S�ĂƂ���
    for(i = 0; i < MENU_MAX; i++)
    {
        passby_ggid[i] = TRUE;
    }
    
    /* ���C�����[�v */
    while(1)
    {
        OS_WaitVBlankIntr();
        
        keyData = ReadKeySetTrigger(PAD_Read());
        
        /* �\���N���A */
        BgClear();
        
        // ���ꂿ�����ʐM�ݒ���
        Menu();

        // ���ꂿ�����ʐM�Ɉڍs
        if(menu_flg == 9)
        {
            int i;
            
            // �I�����ꂽGGID�̂ݏ�����.
            for(i = 0; i < MENU_MAX; i++)
            {
                if(passby_ggid[i])
                {
                    /* ���ꂿ�����ʐM�̏�����, �o�^, �N�� */
                    User_Init();
                    break;
                }
            }
            if(i >= MENU_MAX)
            {
                /* �G���[���b�Z�[�W�\���J�n */
                BgClear();
                BgPutString(5, 19, WHITE, "Set at least one GGID");
                passby_endflg = 4;
            }

            /* �\���N���A */
            BgClear();

            /* ���ꂿ�����ʐM���C�����[�v */
            // �X�^�[�g�{�^���������܂ł͒��p���ɂȂ葱����
            for (gFrame = 0; passby_endflg != 3; gFrame++)
            {
                OS_WaitVBlankIntr();

                keyData = ReadKeySetTrigger(PAD_Read());
                
                /* �ʐM��Ԃɂ�菈����U�蕪�� */
                switch (WXC_GetStateCode())
                {
                case WXC_STATE_END:
                    ModeSelect();
                    break;
                case WXC_STATE_ENDING:
                    break;
                case WXC_STATE_ACTIVE:
                    if(WXC_IsParentMode() == TRUE)
                    {
                        BgPutString(8, 2, WHITE, "Now parent...");
                    }
                    else
                    {
                        BgPutString(8, 2, WHITE, "Now child...");
                    }
                    break;
                }
                
                BgPutString(5, 19, WHITE, "Push START to Reset");
                
                if (keyData & PAD_BUTTON_START)
                {
                    passby_endflg = 3;
                    /* �\���N���A */
                    BgClear();
                }
                // �u�u�����N�҂�
                OS_WaitVBlankIntr();
            }
            
            // WXC_RegisterCommonData()���Ă΂�Ă���ꍇ�͂����ŏI��.
            if(passby_endflg != 4)
            {
                (void)WXC_End();
            }
            
            while( WXC_GetStateCode() != WXC_STATE_END )
            {
                ;
            }
            // A�{�^���҂�
            while(1)
            {
                keyData = ReadKeySetTrigger(PAD_Read());
                
                BgPutString(3, 1, WHITE, "Push A to Reset");
                
                if (keyData & PAD_BUTTON_A)
                {
                    passby_endflg = 0;
                    menu_flg = MENU_MIN;
                    break;
                }
                OS_WaitVBlankIntr();
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         Menu

  Description:  ���ꂿ�����̐ݒ��ʂł̏����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
 static void Menu(void)
{
    int i;
    enum
    { DISP_OX = 5, DISP_OY = 5 };
    
    /* �J�[�\���ԍ��ύX */
    if (keyData & PAD_KEY_DOWN)
    {
        menu_flg++;
        if(menu_flg > MENU_MAX)
        {
            menu_flg = MENU_MIN;
        }
    }
    else if (keyData & PAD_KEY_UP)
    {
        menu_flg--;
        if(menu_flg < MENU_MIN)
        {
            menu_flg = MENU_MAX;
        }
    }
    else if (keyData & (PAD_BUTTON_A | PAD_BUTTON_START))
    {
        menu_flg = 9;
    }

    /* �\���J�n */
    BgPutString((s16)(DISP_OX + 3), (s16)(DISP_OY - 3), WHITE, "Main Menu");

    for(i = 0; i < MENU_MAX-1; i++)
    {
        BgPrintf((s16)(DISP_OX), (s16)(DISP_OY + i), WHITE,
                 "GGID No.%01d     %s", (i+1), (passby_ggid[i]) ? "[Yes]" : "[No ]");
    }
    
    BgPrintf((s16)(DISP_OX), (s16)(DISP_OY + i), WHITE,
                 "RelayMode     %s", (keep_flg) ? "[KEEP ]" : "[THROW]");
    
    /* �J�[�\���\�� */
    BgPutChar((s16)(DISP_OX - 2), (s16)(DISP_OY + menu_flg-1), RED, '*');
    /* A�{�^���҂� */
    BgPutString(DISP_OX, DISP_OY + 14, WHITE, " Push A or START Button");


    if (keyData & (PAD_KEY_RIGHT | PAD_KEY_LEFT))
    {
		if(menu_flg < MENU_MAX)
		{
            passby_ggid[menu_flg-1] ^= TRUE;
        }
        else
        {
			keep_flg ^= TRUE;
	    }
    }
}
 
/*---------------------------------------------------------------------------*
  Name:         ModeSelect

  Description:  �e�@/�q�@ �I����ʂł̏����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeSelect(void)
{
    BgPutString(3, 1, WHITE, "Push A to start");

    if (keyData & PAD_BUTTON_A)
    {
        User_Init();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ModeError

  Description:  �G���[�\����ʂł̏����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ModeError(void)
{
    BgPutString(5, 0, WHITE, "======= ERROR! =======");
    BgPutString(5, 1, WHITE, " Fatal error occured.");
    BgPutString(5, 2, WHITE, "Please reboot program.");
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  �u�u�����N�����݃n���h��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    //---- �n�`�l�̍X�V
    DC_FlushRange(oamBak, sizeof(oamBak));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    MI_DmaCopy32(3, oamBak, (void *)HW_OAM, sizeof(oamBak));

    //---- BG�X�N���[���X�V
    BgScrSetVblank();

    //(void)rand();

    //---- ���荞�݃`�F�b�N�t���O
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
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


/*----------------------------------------------------------------------------*

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
