/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/2D_Oam_4
  File:     main.c

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
//---------------------------------------------------------------------------
// A sample that flips OBJs:
//
// USAGE:
//   A: Flips horizontally
//   B: Flips vertically
//
// HOWTO:
// 1. Set up the character/palette/attribute data same as 2D_Oam_1.
// 2. Flip horizontally/vertically by setting corresponding fields.
//---------------------------------------------------------------------------
#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#include "DEMO.h"
#include "data.h"

static GXOamAttr sOamBak[128];         // Buffer for OAM

#ifdef SDK_TWL
void TwlMain(void)
#else
void NitroMain(void)
#endif
{
    int     flip_H = 0, flip_V = 0;

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and make OBJ visible.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayOBJOnly();

    //---------------------------------------------------------------------------
    // Transmitting the character data and the palette data
    //---------------------------------------------------------------------------
    GX_LoadOBJ(d_64_256_obj_schDT, 0, sizeof(d_64_256_obj_schDT));
    GX_LoadOBJPltt(d_64_256_obj_sclDT, 0, sizeof(d_64_256_obj_sclDT));

    MI_DmaFill32(3, sOamBak, 192, sizeof(sOamBak));     // OAM�o�b�t�@���N���A�[

    G2_SetOBJAttr(&sOamBak[0],         // a pointer to the attributes
                  0,                   // x
                  0,                   // y
                  0,                   // priority
                  GX_OAM_MODE_NORMAL,  // OBJ mode
                  FALSE,               // mosaic
                  GX_OAM_EFFECT_NONE,  // flip/affine/no display/affine(double)
                  GX_OAM_SHAPE_64x64,  // size and shape
                  GX_OAM_COLORMODE_256, // OBJ character data are in 256 colors format
                  0,                   // character name
                  0,                   // color param
                  0                    // affine param
        );

    DEMOStartDisplay();
    //---------------------------------------------------------------------------
    // Main Loop
    //---------------------------------------------------------------------------
    while (1)
    {
        DEMOReadKey();

        if (DEMO_IS_TRIG(PAD_BUTTON_A))
        {
            ++flip_H;
            flip_H %= 2;
        }
        if (DEMO_IS_TRIG(PAD_BUTTON_B))
        {
            ++flip_V;
            flip_V %= 2;
        }

        {
            GXOamEffect effect = (GXOamEffect)0;
            if (flip_H)
                effect |= GX_OAM_EFFECT_FLIP_H;
            if (flip_V)
                effect |= GX_OAM_EFFECT_FLIP_V;

#ifdef SDK_AUTOTEST
            effect = (GXOamEffect)(GX_OAM_EFFECT_FLIP_H | GX_OAM_EFFECT_FLIP_V);        // default params for testing.
#endif //SDK_AUTOTEST


            //---------------------------------------------------------------------------
            // G2_SetOBJEffect:
            // it gives effects such as flip/scale and rotation.
            //---------------------------------------------------------------------------
            G2_SetOBJEffect(&sOamBak[0], effect, 0      // select affine params if GX_OAM_EFFECT_AFFINE or GX_OAM_EFFECT_AFFINE_DOUBLE selected
                );
        }

        // Store the data onto the main memory, and invalidate the cache.
        DC_FlushRange(sOamBak, sizeof(sOamBak));
        /* DMA�����IO���W�X�^�փA�N�Z�X����̂ŃL���b�V���� Wait �͕s�v */
        // DC_WaitWriteBufferEmpty();

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x9975CFB4);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of VBlank interrupt

        GX_LoadOAM(sOamBak, 0, sizeof(sOamBak));
    }
}

//---------------------------------------------------------------------------
// VBlank interrupt function:
//
// Interrupt handlers are registered on the interrupt table by OS_SetIRQFunction.
// OS_EnableIrqMask selects IRQ interrupts to enable, and
// OS_EnableIrq enables IRQ interrupts.
// Notice that you have to call 'OS_SetIrqCheckFlag' to check a VBlank interrupt.
//---------------------------------------------------------------------------
void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK); // checking VBlank interrupt
}
