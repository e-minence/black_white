/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - CTRDG - pullout-1
  File:     main.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: $
  $Rev: $
  $Author: $
 *---------------------------------------------------------------------------*/
#include  <nitro.h>
#include  "font.h"
#include  "screen.h"

static void myForceTerminate(void);

static void myInit(void);
static void myVBlankIntr(void);

static BOOL myPulledOutCallback_CARTRIDGE(void);
static BOOL myPulledOutCallback_CARD(void);
static BOOL myPulledOutCallback_dummy(void);

typedef enum
{
    MY_TERMINATE_NONE = 0,
    MY_TERMINATE_CARTRIDGE = 1,
    MY_TERMINATE_CARD = 2
}
MyTerminateFlag;

MyTerminateFlag terminateFlag = MY_TERMINATE_NONE;

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    BOOL    isCartridgeExist;

    u16     preButton;
    u16     button;
    u16     trigger;

    //---------------- initialize
    myInit();

    preButton = PAD_Read();            // dummy read for pressing 'A' on IPL

    //---- set pullout callback
    CTRDG_SetPulledOutCallback(myPulledOutCallback_CARTRIDGE);
    CARD_SetPulledOutCallback(myPulledOutCallback_CARD);

    //---- check if cartridge exist
    isCartridgeExist = CTRDG_IsExisting();

    //---------------- main loop
    while (TRUE)
    {
        button = PAD_Read();
        trigger = (u16)((button ^ preButton) & button);
        preButton = button;

        //---- clear screen buffer
        ClearScreen();

        //---- display key description
        PrintString(3, 2, 15, "push START to sleep");
        PrintString(3, 3, 15, "till push SELECT");
        if (isCartridgeExist)
        {
            PrintString(3, 4, 15, "or pull out cartridge");
            PrintString(3, 5, 15, "or pull out card.");
        }
        else
        {
            PrintString(3, 4, 15, "or pull out card.");
        }

        //---- display time
        {
            static RTCTime myCurrentTime;
            static int myResult;
            myResult = RTC_GetTime(&myCurrentTime);
            if (myResult == 0 /*no error */ )
            {
                PrintString(5, 20, 6, "%02d:%02d:%02d",
                            myCurrentTime.hour, myCurrentTime.minute, myCurrentTime.second);
            }
        }

        //---- display counter
        PrintString(18, 20, 4, "%08X", OS_GetVBlankCount());

        //---- push START to sleep
        if (trigger & PAD_BUTTON_START)
        {
            PM_GoSleepMode(PM_TRIGGER_CARD | PM_TRIGGER_CARTRIDGE | PM_TRIGGER_KEY,
                           PM_PAD_LOGIC_AND, PAD_BUTTON_SELECT);
        }

        //---- close cover to sleep
        if (PAD_DetectFold() == TRUE)
        {
            PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD | PM_TRIGGER_CARTRIDGE, 0, 0);
        }

        //---- terminate program if need
        if (terminateFlag != MY_TERMINATE_NONE)
        {
            myForceTerminate();
        }

        OS_WaitVBlankIntr();
    }
}


//----------------------------------------------------------------
//  myForceTerminate
//
void myForceTerminate(void)
{
    int     n;

    //---- set callback to do nothing, because anyway do termination soon.
    CTRDG_SetPulledOutCallback(myPulledOutCallback_dummy);
    CARD_SetPulledOutCallback(myPulledOutCallback_dummy);

    for (n = 0; n < 10; n++)
    {
        ClearScreen();
        if (terminateFlag == MY_TERMINATE_CARTRIDGE)
        {
            PrintString(4, 10, 1, "DON'T PULL OUT CARTRIDGE!");
        }
        else
        {
            PrintString(6, 10, 1, "DON'T PULL OUT CARD!");
        }

        OS_WaitVBlankIntr();
    }

    //---- terminate
    if (terminateFlag == MY_TERMINATE_CARTRIDGE)
    {
        CTRDG_TerminateForPulledOut();
    }
    else
    {
        CARD_TerminateForPulledOut();
    }

    // ** never reach this line **
}

//----------------------------------------------------------------
//  myInit
//
void myInit(void)
{
    //---- init
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    RTC_Init();
    FX_Init();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

    //---- init displaying
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);
    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);

    //---- setting 2D for top screen
    GX_SetBankForBG(GX_VRAM_BG_128_A);

    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256,
                     GX_BG_COLORMODE_16,
                     GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_SetBG0Priority(0);
    G2_BG0Mosaic(FALSE);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0);

    GX_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GX_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));



    //---- setting 2D for bottom screen
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);

    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256,
                      GX_BG_COLORMODE_16,
                      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2S_SetBG0Priority(0);
    G2S_BG0Mosaic(FALSE);
    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_SetVisiblePlane(GX_PLANEMASK_BG0);

    GXS_LoadBG0Char(d_CharData, 0, sizeof(d_CharData));
    GXS_LoadBGPltt(d_PaletteData, 0, sizeof(d_PaletteData));


    //---- screen
    MI_CpuFillFast((void *)gScreen, 0, sizeof(gScreen));
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));
    GXS_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    //---- init interrupt
    OS_SetIrqFunction(OS_IE_V_BLANK, myVBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);

    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    //---- start displaying
    GX_DispOn();
    GXS_DispOn();
}

//----------------------------------------------------------------
//  myVBlankIntr
//             vblank interrupt handler
//
static void myVBlankIntr(void)
{
    //---- upload pseudo screen to VRAM
    DC_FlushRange(gScreen, sizeof(gScreen));
    /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
    // DC_WaitWriteBufferEmpty();
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));
    GXS_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

//----------------------------------------------------------------
//  myPulledOutCallback_CARTRIDGE
//
static BOOL myPulledOutCallback_CARTRIDGE(void)
{
    terminateFlag = MY_TERMINATE_CARTRIDGE;
    return FALSE;                      //---- means not to terminate immediately by system
}

//----------------------------------------------------------------
//  myPulledOutCallback_CARD
//
static BOOL myPulledOutCallback_CARD(void)
{
    terminateFlag = MY_TERMINATE_CARD;
    return FALSE;                      //---- means not to terminate immediately by system
}

//----------------------------------------------------------------
//  myPulledOutCallback_dummy
//
static BOOL myPulledOutCallback_dummy(void)
{
    //---- do nothing
    return FALSE;
}
