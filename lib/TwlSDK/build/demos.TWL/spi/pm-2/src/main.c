/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos.TWL - SPI - pm-2
  File:     main.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-13#$
  $Rev: 0 $
  $Author: yada $
 *---------------------------------------------------------------------------*/
#include  <nitro.h>
#include  "font.h"
#include  "screen.h"

#define EXIT_COUNT 0xA0

//---- for RTC
static RTCTime myCurrentTime;

//---- vcount
static u32 myVCount;

//---- auto exit flag
static BOOL isAutoExit = FALSE;

//---- exit callback info 
static PMExitCallbackInfo exitCallbackInfo;

//---- whether exit callback was called
static BOOL isExit = FALSE;

//---- exit delay counter
static int  exitCounter;

static void myInit(void);
static void myVBlankIntr(void);
static void myExitCallback(void *arg);

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  main

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    u16     preButton;
    u16     button;
    u16     trigger;

    //---------------- initialize
    myInit();
    RTC_Init();

    if ( OS_IsRunOnTwl() )
    {
        PM_SetAutoExit( isAutoExit );
    }
    else
    {
        OS_Printf("\nThis demo is only for TWL mode.");
        OS_Exit(0);
    }

    OS_Printf("[ARM9] set exit callback\n");
    PM_SetExitCallbackInfo( &exitCallbackInfo, myExitCallback, (void*)100 );
    PM_AppendPreExitCallback( &exitCallbackInfo );

    preButton = PAD_Read();            // dummy read for pressing 'A' on IPL

    //---------------- main loop
    while (TRUE)
    {
        button = PAD_Read();
        trigger = (u16)((button ^ preButton) & button);
        preButton = button;

        //---- vblank count
        myVCount = OS_GetVBlankCount();

        //---- counter
        if ( isExit )
        {
            if ( -- exitCounter < 0 )
            {
                PM_ReadyToExit();
            }
        }

        //---- clear screen buffer
        ClearScreen();

        //---- display key description
        PrintString(3, 4, 15, "A : toggle Auto Exit mode");
        PrintString(3, 6, 15, "X : do hardware reset");

        //---- display time
        if (RTC_GetTime(&myCurrentTime) == 0 /*no error */ )
        {
            PrintString(5, 20, 8, "%02d:%02d:%02d", myCurrentTime.hour, myCurrentTime.minute, myCurrentTime.second);
        }

        //---- display AutoExit mode
        {
            BOOL a = PM_GetAutoExit();
            PrintString(8, 16, 15, "AUTO EXIT : %s", a? "TRUE": "FALSE" );
        }

        //---- display v-counter
        PrintString(18, 20, 4, "%08X", myVCount);

        //---- display count
        if ( isExit )
        {
            static int blinkCount = 0;
            if ( (++ blinkCount) & 0x4 )
            {
                PrintString(12, 12, 1, "NOW EXIT...");
            }
            PrintString(15, 10, 4, "%X", exitCounter / 0x10);
        }

        //---- push A to switch top backlight on/off
        if (trigger & PAD_BUTTON_A)
        {
            isAutoExit = (isAutoExit == FALSE);
            PM_SetAutoExit( isAutoExit );
        }

        //---- push X to do hardware reset
        if (trigger & PAD_BUTTON_X)
        {
            (void)OS_RebootSystem();
            // Do not call PM_ForceToResetHardware() directly.
        }

        OS_WaitVBlankIntr();
    }
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

    //---- FileSytem init
    FS_Init(FS_DMA_NOT_USE);

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

//================================================================
//----------------------------------------------------------------
//  myExitCallback
//
void myExitCallback(void *arg)
{
#ifdef SDK_FINALROM
#pragma unused( arg )
#endif
    OS_Printf("exit callback. arg=%d factor=%d\n", arg, PM_GetExitFactor() );

    exitCounter = EXIT_COUNT;
    isExit = TRUE;
}
