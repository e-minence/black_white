/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - spi - tp-2
  File:     main.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-08#$
  $Rev: 10700 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/

#include    <nitro.h>

#define     CHATTER_RANGE           20
#define     CHATTER_RETRY           5

#define     SAMPLING_FREQUENCE      4  // Touch panel sampling frequence per frame
#define     SAMPLING_BUFSIZE        (SAMPLING_FREQUENCE + 1)    // AutoSampling buffer size
#define     SAMPLING_START_VCOUNT   0  // base vcount value in auto sampling.

/*---------------------------------------------------------------------------*
    Prototype definition
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void);
static void KeyInit(void);
static void KeyRead(void);
static inline void PutDot(u16 x, u16 y, u16 col);
static void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 col);
static void HandDraw(TPData *buf, u32 size);


// Key State
static struct
{
    u16     con;
    u16     trg;
}
keys;


/*---------------------------------------------------------------------------*
    Static variables definition
 *---------------------------------------------------------------------------*/
static TPData gTpBuf[SAMPLING_BUFSIZE];


/*---------------------------------------------------------------------------*
  Name:         PutDot

  Description:  Draw a Dot in VRAM for LCDC.

  Arguments:    x   - position X.
                y   - position Y.
                col - DotColor.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PutDot(u16 x, u16 y, u16 col)
{
    *(u16 *)(HW_LCDC_VRAM_C + y * 256 * 2 + x * 2) = col;
}


/*---------------------------------------------------------------------------*
  Name:         DrawLine

  Description:  Draw a line VRAM for LCDC.

  Arguments:    x1   - X of begin.
                y1   - Y of begin.
                x2   - X of end.
                y2   - Y of end.
                col -  Lines' Color.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 col)
{
    u16     width, height;
    u16     i, min, max, base;
    u16     px, py;

    width = (u16)((x1 >= x2) ? (x1 - x2) : (x2 - x1));  // width  = | x1 - x2 |
    height = (u16)((y1 >= y2) ? (y1 - y2) : (y2 - y1)); // height = | y1 - y2 |


    if (width > height)
    {
        /* if (X size > Y size) draw a Dot per Xdot */
        if (x1 >= x2)
        {
            min = x2;
            max = x1;
            base = y2;
        }
        else
        {
            min = x1;
            max = x2;
            base = y1;
        }

        for (i = min; i <= max; i++)
        {
            px = i;
            py = (u16)(((px - min) * (y2 - y1)) / (x2 - x1) + base);
            PutDot(px, py, col);
        }

    }
    else
    {
        /* if (X size <= Y size) draw a Dot per Ydot */
        if (y1 >= y2)
        {
            min = y2;
            max = y1;
            base = x2;
        }
        else
        {
            min = y1;
            max = y2;
            base = x1;
        }

        for (i = min; i <= max; i++)
        {
            py = i;
            px = (u16)((py - min) * (x2 - x1) / (y2 - y1) + base);
            PutDot(px, py, col);
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         HandDraw

  Description:  �����T���v�����O�œ���ꂽ�S�Ă̐ڐG�_���W��\������B

  Arguments:    buf  - a pointer of Auto sampling buffer.
                size - frequence of auto sampling.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void HandDraw(TPData *buf, u32 size)
{
    s32     i, idx;
    s32     last_idx;
    TPData  sample;
    static TPData tpLast = { 0, 0, 0, 0 };

    // buf[LatestIndex + 1] �̃f�[�^�͎����T���v�����O�����ŏ㏑�������\��������B
    last_idx = TP_GetLatestIndexInAuto();

    for (i = 0; i < size; i++)
    {
        idx = last_idx - (s32)size + i + 1;
        if (idx < 0)
        {
            idx += SAMPLING_BUFSIZE;
        }

        if (!buf[idx].touch)
        {
            tpLast.touch = 0;
            continue;
        }
        if (buf[idx].validity != 0)
        {
            continue;
        }

        TP_GetCalibratedPoint(&sample, &buf[idx]);
        OS_Printf("( %d, %d ) -> ( %d, %d )\n", buf[idx].x, buf[idx].y, sample.x, sample.y);
        if (!tpLast.touch)
        {
            PutDot(sample.x, sample.y, 0);
        }
        else
        {
            DrawLine(tpLast.x, tpLast.y, sample.x, sample.y, 0);
        }
        tpLast = sample;
    }
}


/*---------------------------------------------------------------------------*
  Name:         DisplayInit

  Description:  Graphics Initialization

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DisplayInit()
{

    GX_Init();
    FX_Init();

    GX_DispOff();
    GXS_DispOff();

    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);         // to generate VBlank interrupt request
    (void)OS_EnableIrq();


    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);

    MI_CpuFillFast((void *)HW_OAM, 192, HW_OAM_SIZE);   // clear OAM
    MI_CpuClearFast((void *)HW_PLTT, HW_PLTT_SIZE);     // clear the standard palette

    MI_CpuFillFast((void *)HW_DB_OAM, 192, HW_DB_OAM_SIZE);     // clear OAM
    MI_CpuClearFast((void *)HW_DB_PLTT, HW_DB_PLTT_SIZE);       // clear the standard palette
    MI_DmaFill32(3, (void *)HW_LCDC_VRAM_C, 0x7FFF7FFF, 256 * 192 * sizeof(u16));


    GX_SetBankForOBJ(GX_VRAM_OBJ_256_AB);       // Set VRAM-A,B for OBJ

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS,    // 2D / 3D Mode
                       GX_BGMODE_0,    // BGMODE 0
                       GX_BG0_AS_2D);  // Set BG0 as 2D

    GX_SetVisiblePlane(GX_PLANEMASK_OBJ);       // make OBJ visible
    GX_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_1D_128K);   // 2D mapping OBJ

    OS_WaitVBlankIntr();               // Waiting the end of VBlank interrupt
    GX_DispOn();

}





/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  Initialization and main loop

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{

    TPCalibrateParam calibrate;

    // Initialization
    KeyInit();
    OS_Init();
    TP_Init();

    // get CalibrateParameter from FlashMemory
    if (!TP_GetUserInfo(&calibrate))
    {
        OS_Panic("FATAL ERROR: can't get UserInfo\n");
    }
    else
    {
        OS_Printf("Get Calibration Parameter from NVRAM\n");
    }

    TP_SetCalibrateParam(&calibrate);

    DisplayInit();

    // unnecessary called. 
    // be able to use default parameter (retry,range).
#if 0
    // Send parameter of revision noise.
    if (TP_RequestSetStability(3, 15) != 0)
    {
        OS_Panic("SetStability request err!\n");
    }
#endif

    GX_SetGraphicsMode(GX_DISPMODE_VRAM_C,      // VRAM mode.
                       (GXBGMode)0,    // dummy
                       (GXBG0As)0);    // dummy

    // send auto sampling start request.
    if (TP_RequestAutoSamplingStart(SAMPLING_START_VCOUNT, SAMPLING_FREQUENCE,
                                    gTpBuf, SAMPLING_BUFSIZE))
    {
        OS_Panic("auto sampling start reqeuest err!\n");
    }
    OS_Printf("Start auto sampling\n");

    // �L�[���͏��擾�̋�Ăяo��(IPL �ł� A �{�^�������΍�)
    KeyRead();

    while (TRUE)
    {
        KeyRead();

        // Draw

        // If START button pressed, clear screen.
        if (keys.trg & PAD_BUTTON_START)
        {
            MI_DmaFill32(3, (void *)HW_LCDC_VRAM_C, 0x7FFF7FFF, 256 * 192 * sizeof(u16));
        }

        // �����T���v�����O�œ���ꂽ���ʂ�\������B
        HandDraw(gTpBuf, SAMPLING_FREQUENCE);

        // Wait V-Blank interrupt
        OS_WaitVBlankIntr();
    }
}


/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V-Blank function

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{

    // Set IRQ check flag
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


/*---------------------------------------------------------------------------*
  Name:         KeyRead

  Description:  Read Pad Keys.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyRead(void)
{
    u16     r = PAD_Read();

    keys.trg = (u16)(~keys.con & r);
    keys.con = r;
}

/*---------------------------------------------------------------------------*
  Name:         KeyInit

  Description:  Initialize Pad Keys.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void KeyInit(void)
{

    keys.trg = 0;
    keys.con = 0;

}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
