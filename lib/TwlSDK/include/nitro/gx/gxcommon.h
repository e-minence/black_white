/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - 
  File:     gxcommon.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_GXCOMMON_H_
#define NITRO_GXCOMMON_H_

#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))

#include <nitro/misc.h>
#include <nitro/memorymap.h>

#endif

#include <nitro/types.h>
#include <nitro/fx/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

#define GX_DEFAULT_DMAID (3)
#define GX_DMA_NOT_USE   ((u32)~0)

#define GX_DMAID         (GXi_DmaId)   // remain for compatibility
extern u32 GXi_DmaId;

// x1B5G5R5 format
typedef u16 GXRgb;
#define GXRGB_ASSERT(gxrgb)       SDK_ASSERT(((gxrgb) & 0x8000) == 0)
#define GX_RGB_R_SHIFT            (0)
#define GX_RGB_R_MASK             (0x001f)
#define GX_RGB_G_SHIFT            (5)
#define GX_RGB_G_MASK             (0x03e0)
#define GX_RGB_B_SHIFT            (10)
#define GX_RGB_B_MASK             (0x7c00)
#define GX_RGB(r, g, b)           ((GXRgb)(((r) << GX_RGB_R_SHIFT) | \
                                           ((g) << GX_RGB_G_SHIFT) | \
                                           ((b) << GX_RGB_B_SHIFT)))

// A1B5G5R5 format
typedef u16 GXRgba;
#define GX_RGBA_R_SHIFT           (0)
#define GX_RGBA_R_MASK            (0x001f)
#define GX_RGBA_G_SHIFT           (5)
#define GX_RGBA_G_MASK            (0x03e0)
#define GX_RGBA_B_SHIFT           (10)
#define GX_RGBA_B_MASK            (0x7c00)
#define GX_RGBA_A_SHIFT           (15)
#define GX_RGBA_A_MASK            (0x8000)
#define GX_RGBA(r, g, b, a)       ((GXRgba)(((r) << GX_RGBA_R_SHIFT) | \
                                            ((g) << GX_RGBA_G_SHIFT) | \
                                            ((b) << GX_RGBA_B_SHIFT) | \
                                            ((a) << GX_RGBA_A_SHIFT)))


#define GX_POLYGONID_ASSERT(x)    SDK_ASSERT((x) >= 0 && (x) <= 63)
#define GX_ALPHA_ASSERT(x)        SDK_ASSERT((x) >= 0 && (x) <= 31)
#define GX_DEPTH_ASSERT(x)        SDK_ASSERT((x) >= 0 && (x) <= 0x7fff)


//----------------------------------------------------------------------------
// Make VecFx10(normal vector/light vector)
// x : fx32/16, y : fx32/16, z : fx32/16
// -FX32_ONE(-FX16_ONE) < x,y,z < FX32_ONE(FX16_ONE)
//----------------------------------------------------------------------------
#define GX_VECFX10(x, y, z) \
    ((VecFx10)(((((x) >> (FX32_DEC_SIZE - GX_FX10_DEC_SIZE)) & GX_FX10_MASK) << GX_VEC_FX10_X_SHIFT) | \
               ((((y) >> (FX32_DEC_SIZE - GX_FX10_DEC_SIZE)) & GX_FX10_MASK) << GX_VEC_FX10_Y_SHIFT) | \
               ((((z) >> (FX32_DEC_SIZE - GX_FX10_DEC_SIZE)) & GX_FX10_MASK) << GX_VEC_FX10_Z_SHIFT)))

//----------------------------------------------------------------------------
// type for 3D vector of s0.9
//----------------------------------------------------------------------------
typedef u32 VecFx10;
#define GX_FX10_SHIFT             9
#define GX_FX10_INT_SIZE          0
#define GX_FX10_DEC_SIZE          9

#define GX_FX10_INT_MASK          0x0000
#define GX_FX10_DEC_MASK          0x01ff
#define GX_FX10_SIGN_MASK         0x0200
#define GX_FX10_MASK              (GX_FX10_INT_MASK | GX_FX10_DEC_MASK | GX_FX10_SIGN_MASK)

#define GX_FX16_FX10_MAX          (fx16)(0x0fff)
#define GX_FX16_FX10_MIN          (fx16)(0xf000)

#define GX_FX32_FX10_MAX          (fx32)(0x00000fff);
#define GX_FX32_FX10_MIN          (fx32)(0xfffff000);

#define GX_VEC_FX10_X_SHIFT       0
#define GX_VEC_FX10_Y_SHIFT       10
#define GX_VEC_FX10_Z_SHIFT       20

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
