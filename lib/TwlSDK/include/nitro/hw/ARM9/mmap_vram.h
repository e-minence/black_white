/*---------------------------------------------------------------------------*
  Project:  TwlSDK - HW - include
  File:     mmap_vram.h

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
#ifndef NITRO_HW_MMAP_VRAM
#define NITRO_HW_MMAP_VRAM

#ifdef __cplusplus
extern "C" {
#endif

#define HW_VRAM_A_SIZE          0x20000 // VRAM-A
#define HW_VRAM_B_SIZE          0x20000 //      B
#define HW_VRAM_C_SIZE          0x20000 //      C
#define HW_VRAM_D_SIZE          0x20000 //      D
#define HW_VRAM_E_SIZE          0x10000 //      E
#define HW_VRAM_F_SIZE          0x4000 //      F
#define HW_VRAM_G_SIZE          0x4000 //      G
#define HW_VRAM_H_SIZE          0x8000 //      H
#define HW_VRAM_I_SIZE          0x4000 //      I

#define HW_LCDC_VRAM_A          (HW_LCDC_VRAM)
#define HW_LCDC_VRAM_B          (HW_LCDC_VRAM_A + HW_VRAM_A_SIZE)
#define HW_LCDC_VRAM_C          (HW_LCDC_VRAM_B + HW_VRAM_B_SIZE)
#define HW_LCDC_VRAM_D          (HW_LCDC_VRAM_C + HW_VRAM_C_SIZE)
#define HW_LCDC_VRAM_E          (HW_LCDC_VRAM_D + HW_VRAM_D_SIZE)
#define HW_LCDC_VRAM_F          (HW_LCDC_VRAM_E + HW_VRAM_E_SIZE)
#define HW_LCDC_VRAM_G          (HW_LCDC_VRAM_F + HW_VRAM_F_SIZE)
#define HW_LCDC_VRAM_H          (HW_LCDC_VRAM_G + HW_VRAM_G_SIZE)
#define HW_LCDC_VRAM_I          (HW_LCDC_VRAM_H + HW_VRAM_H_SIZE)

#ifdef __cplusplus
} /* extern "C" */
#endif

/* #ifndef NITRO_HW_MMAP_VRAM */
#endif
