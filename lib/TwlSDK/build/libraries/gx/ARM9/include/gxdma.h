/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - 
  File:     gxdma.h

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

#ifndef NITRO_GXDMA_H_
#define NITRO_GXDMA_H_

#include <nitro/gx/gx.h>
#include <nitro/mi.h>

#ifdef SDK_TWL
#include <twl/mi/common/dma.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static void GXi_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size);
static void GXi_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size);
static void GXi_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                               MIDmaCallback callback, void *arg);
static void GXi_WaitDma(u32 dmaNo);

#define GX_CPU_FASTER32_SIZE        48
#define GX_CPU_FASTER16_SIZE        28

//---------------------------------------------------------------------------
//   inline functions.
//---------------------------------------------------------------------------

static inline void GXi_DmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    if (dmaNo != GX_DMA_NOT_USE && size > GX_CPU_FASTER32_SIZE)
    {
#ifdef SDK_TWL
        if (dmaNo > 3)
        {
            MI_NDmaCopy(dmaNo-4, src, dest, size);
        }
        else
#endif
        {
            MI_DmaCopy32(dmaNo, src, dest, size);
        }
    }
    else
    {
        MI_CpuCopy32(src, dest, size);
    }
}

static inline void GXi_DmaCopy16(u32 dmaNo, const void *src, void *dest, u32 size)
{
    if (dmaNo != GX_DMA_NOT_USE && size > GX_CPU_FASTER16_SIZE)
    {
#ifdef SDK_TWL
        if (dmaNo > 3)
        {
            if ( (((u32)src | (u32)dest | (u32)size) & 3) == 0 )
            {
                MI_NDmaCopy(dmaNo-4, src, dest, size);
            }
            else
            {
                MI_CpuCopy16(src, dest, size);
            }
        }
        else
#endif
        {
            MI_DmaCopy16(dmaNo, src, dest, size);
        }
    }
    else
    {
        MI_CpuCopy16(src, dest, size);
    }
}

static inline void GXi_DmaCopy32Async(u32 dmaNo, const void *src, void *dest, u32 size,
                                      MIDmaCallback callback, void *arg)
{
    if (dmaNo != GX_DMA_NOT_USE)
    {
#ifdef SDK_TWL
        if (dmaNo > 3)
        {
            MI_NDmaCopyAsync(dmaNo-4, src, dest, size, callback, arg);
        }
        else
#endif
        {
            MI_DmaCopy32Async(dmaNo, src, dest, size, callback, arg);
        }
    }
    else
    {
        MI_CpuCopy32(src, dest, size);
    }
}

static inline void GXi_WaitDma(u32 dmaNo)
{
    if (dmaNo != GX_DMA_NOT_USE)
    {
#ifdef SDK_TWL
        if (dmaNo > 3)
        {
            MI_WaitNDma(dmaNo-4);
        }
        else
#endif
        {
            MI_WaitDma(dmaNo);
        }
    }
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // NITRO_GXDMA_H_
