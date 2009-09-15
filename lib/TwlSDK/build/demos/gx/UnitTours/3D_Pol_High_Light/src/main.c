/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/3D_Pol_High_Light
  File:     main.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// A sample for high-light shading
//
// Display two chairs.
// The left one is in modulation mode, and the right one is in high-light mode.
//
// HOWTO:
// 1. Switch to high light shading mode by G3X_SetShading(GX_SHADING_HIGHLIGHT).
// 2. Set up the high-light(toon) table by G3X_SetToonTable.
// 3. Speciy GX_POLYGON_MODE_TOON by G3X_PolygonAttr.
//---------------------------------------------------------------------------

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif
#include "DEMO.h"
#include "data.h"

GXRgb   Toon_Table[32] = {
    GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), GX_RGB(0, 0, 0),
    GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), GX_RGB(0, 0, 0),
    GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), GX_RGB(0, 0, 0), GX_RGB(0, 0, 0),
    GX_RGB(4, 4, 4), GX_RGB(4, 4, 4), GX_RGB(4, 4, 4), GX_RGB(4, 4, 4),
    GX_RGB(16, 16, 16), GX_RGB(16, 16, 16), GX_RGB(16, 16, 16), GX_RGB(16, 16, 16),
    GX_RGB(19, 19, 19), GX_RGB(22, 22, 22), GX_RGB(25, 25, 25), GX_RGB(28, 28, 28),
    GX_RGB(29, 29, 29), GX_RGB(30, 30, 30), GX_RGB(31, 31, 31), GX_RGB(31, 31, 31),
    GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), GX_RGB(31, 31, 31)
};


#ifdef SDK_TWL
void TwlMain(void)
#else
void NitroMain(void)
#endif
{
    u16     Rotate = 0;                // for rotating cubes(0-65535)
    u32     myTexAddr = 0x1000;        // texture image address in texture image slots
    u32     myTexPlttAddr = 0x1000;    // texture palette address in texture palette slots

    //---------------------------------------------------------------------------
    // Initialize:
    // They enable IRQ interrupts, initialize VRAM, and set BG #0 for 3D mode.
    //---------------------------------------------------------------------------
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplay3D();

    //---------------------------------------------------------------------------
    // Specify high-light mode
    //---------------------------------------------------------------------------
    G3X_SetShading(GX_SHADING_HIGHLIGHT);

    //---------------------------------------------------------------------------
    // Download the textures:
    //
    // Transfer the texture data on the main memory to the texture image slots.
    //---------------------------------------------------------------------------
    GX_BeginLoadTex();                 // map the texture image slots onto LCDC address space
    {
        GX_LoadTex((void *)&tex_isu1[0],        // a pointer to the texture data on the main memory(4 bytes aligned)
                   myTexAddr,          // an offset address in the texture image slots
                   tex_isu1_size       // the size of the texture(s)(in bytes)
            );
        GX_LoadTex((void *)&tex_isu2[0],        // a pointer to the texture data on the main memory(4 bytes aligned)
                   myTexAddr + tex_isu1_size,   // an offset address in the texture image slots
                   tex_isu2_size       // the size of the texture(s)(in bytes)
            );
    }
    GX_EndLoadTex();                   // restore the texture image slots

    GX_BeginLoadTexPltt();             // map the texture palette slots onto LCDC address space
    {
        GX_LoadTexPltt((void *)&pal_isu1[0],    // a pointer to the texture data on the main memory(4 bytes aligned)
                       myTexPlttAddr,  // an offset address in the texture palette slots
                       pal_isu1_size   // the size of the texture palette(s)(in bytes)
            );
        GX_LoadTexPltt((void *)&pal_isu2[0],    // a pointer to the texture data on the main memory(4 bytes aligned)
                       myTexPlttAddr + pal_isu1_size,   // an offset address in the texture palette slots
                       pal_isu2_size   // the size of the texture palette(s)(in bytes)
            );
    }
    GX_EndLoadTexPltt();               // restore the texture palette slots


    //---------------------------------------------------------------------------
    // Set up the high light(toon) table
    //---------------------------------------------------------------------------
    G3X_SetToonTable(Toon_Table);

    DEMOStartDisplay();
    while (1)
    {
        G3X_Reset();
        Rotate += 256;

        //---------------------------------------------------------------------------
        // Set up camera matrix
        //---------------------------------------------------------------------------
        {
            VecFx32 Eye = { 0, 0, FX32_ONE };   // Eye Position
            VecFx32 at = { 0, 0, 0 };  // Viewpoint
            VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

            G3_LookAt(&Eye, &vUp, &at, NULL);
        }

        //---------------------------------------------------------------------------
        // Set up light colors and direction.
        // Notice that light vector is transformed by the current vector matrix
        // immediately after LightVector command is issued.
        //
        // GX_LIGHTID_0: white, downward
        //---------------------------------------------------------------------------
        G3_LightVector(GX_LIGHTID_0, FX32_SQRT1_2, -FX32_SQRT1_2, 0);
        G3_LightColor(GX_LIGHTID_0, GX_RGB(16, 16, 16));

        G3_Translate(0, 0, -5 * FX32_ONE);

        G3_PushMtx();

        //---------------------------------------------------------------------------
        //
        // Left chair (in modulation mode)
        //
        //---------------------------------------------------------------------------
        G3_PushMtx();

        {
            // Rotate and translate the chair
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);

            G3_Translate(-2 * FX32_ONE, 0, 0);
            G3_RotZ(s, c);
            G3_RotY(s, c);
            G3_RotX(s, c);
        }

        G3_TexImageParam(GX_TEXFMT_PLTT16,      // 16 colors palette texture
                         GX_TEXGEN_NONE,        // no texgen
                         GX_TEXSIZE_S256,       // 256 pixels
                         GX_TEXSIZE_T64,        // 64 pixels
                         GX_TEXREPEAT_NONE,     // no repeat
                         GX_TEXFLIP_NONE,       // no flip
                         GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
                         myTexAddr     // the offset of the texture image
            );

        G3_TexPlttBase(myTexPlttAddr,  // the offset of the texture palette
                       GX_TEXFMT_PLTT16 // 16 colors palette texture
            );

        // Set the material color( diffuse, ambient , specular ) as basic white 
        DEMO_Set3DDefaultMaterial(TRUE, TRUE);
        DEMO_Set3DDefaultShininessTable();

        G3_PolygonAttr(GX_LIGHTMASK_0, // Light #0 is on
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_TRIANGLES);
        {
            MI_SendGXCommand(3, isu1, isu1_size);
        }
        G3_End();

        G3_TexImageParam(GX_TEXFMT_PLTT16,      // 16 colors palette texture
                         GX_TEXGEN_NONE,        // no texgen
                         GX_TEXSIZE_S128,       // 128 pixels
                         GX_TEXSIZE_T128,       // 128 pixels
                         GX_TEXREPEAT_NONE,     // no repeat
                         GX_TEXFLIP_NONE,       // no flip
                         GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
                         myTexAddr + tex_isu1_size      // the offset of the texture image
            );

        G3_TexPlttBase(myTexPlttAddr + pal_isu1_size,   // the offset of the texture palette
                       GX_TEXFMT_PLTT16 // 16 colors palette texture
            );

        // Set the material color( diffuse, ambient , specular ) as basic white 
        DEMO_Set3DDefaultMaterial(TRUE, TRUE);
        DEMO_Set3DDefaultShininessTable();


        G3_PolygonAttr(GX_LIGHTMASK_0, // Light #0 is on
                       GX_POLYGONMODE_MODULATE, // modulation mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_TRIANGLES);
        {
            MI_SendGXCommand(3, isu2, isu2_size);
        }
        G3_End();

        G3_PopMtx(1);

        //---------------------------------------------------------------------------
        //
        // Right chair (in high-light mode)
        //
        //---------------------------------------------------------------------------
        G3_PushMtx();

        {
            fx16    s = FX_SinIdx(Rotate);
            fx16    c = FX_CosIdx(Rotate);

            G3_Translate(2 * FX32_ONE, 0, 0);
            G3_RotZ(s, c);
            G3_RotY(s, c);
            G3_RotX(s, c);
        }

        G3_TexImageParam(GX_TEXFMT_PLTT16,      // 16 colors palette texture
                         GX_TEXGEN_NONE,        // no texgen
                         GX_TEXSIZE_S256,       // 256 pixels
                         GX_TEXSIZE_T64,        // 64 pixels
                         GX_TEXREPEAT_NONE,     // no repeat
                         GX_TEXFLIP_NONE,       // no flip
                         GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
                         myTexAddr     // the offset of the texture image
            );

        G3_TexPlttBase(myTexPlttAddr,  // the offset of the texture palette
                       GX_TEXFMT_PLTT16 // 16 colors palette texture
            );

        // Set the material color( diffuse, ambient , specular ) as basic white 
        DEMO_Set3DDefaultMaterial(TRUE, TRUE);
        DEMO_Set3DDefaultShininessTable();

        G3_PolygonAttr(GX_LIGHTMASK_0, // Light #0 is on
                       GX_POLYGONMODE_TOON,     // toon/high-light mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_TRIANGLES);
        {
            MI_SendGXCommand(3, isu1, isu1_size);
        }
        G3_End();

        G3_TexImageParam(GX_TEXFMT_PLTT16,      // 16 colors palette texture
                         GX_TEXGEN_NONE,        // no texgen
                         GX_TEXSIZE_S128,       // 128 pixels
                         GX_TEXSIZE_T128,       // 128 pixels
                         GX_TEXREPEAT_NONE,     // no repeat
                         GX_TEXFLIP_NONE,       // no flip
                         GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
                         myTexAddr + tex_isu1_size      // the offset of the texture image
            );

        G3_TexPlttBase(myTexPlttAddr + pal_isu1_size,   // the offset of the texture palette
                       GX_TEXFMT_PLTT16 // 16 colors palette texture
            );

        // Set the material color( diffuse, ambient , specular ) as basic white 
        DEMO_Set3DDefaultMaterial(TRUE, TRUE);
        DEMO_Set3DDefaultShininessTable();

        G3_PolygonAttr(GX_LIGHTMASK_0, // Light #0 is on
                       GX_POLYGONMODE_TOON,     // TOON / HIGHLIGHT mode
                       GX_CULL_BACK,   // cull back
                       0,              // polygon ID(0 - 63)
                       31,             // alpha(0 - 31)
                       0               // OR of GXPolygonAttrMisc's value
            );

        G3_Begin(GX_BEGIN_TRIANGLES);
        {
            MI_SendGXCommand(3, isu2, isu2_size);
        }
        G3_End();

        G3_PopMtx(1);

        G3_PopMtx(1);

        // swapping the polygon list RAM, the vertex RAM, etc.
        G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);

#ifdef SDK_AUTOTEST
        GX_SetBankForLCDC(GX_VRAM_LCDC_C);
        EXT_TestSetVRAMForScreenShot(GX_VRAM_LCDC_C);
        EXT_TestScreenShot(100, 0x76068B4B);
        EXT_TestTickCounter();
#endif //SDK_AUTOTEST

        OS_WaitVBlankIntr();           // Waiting the end of VBlank interrupt
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
