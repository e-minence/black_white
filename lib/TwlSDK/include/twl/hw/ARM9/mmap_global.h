/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - twl - HW - ARM9
  File:     mmap_global.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef TWL_HW_ARM9_MMAP_GLOBAL_H_
#define TWL_HW_ARM9_MMAP_GLOBAL_H_
#ifdef  __cplusplus
extern  "C" {
#endif
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    GLOBAL MEMORY MAP
 *---------------------------------------------------------------------------*/

//----------------------------- ITCM
#define HW_ITCM_IMAGE           0x01000000
#define HW_ITCM                 0x01ff8000
#define HW_ITCM_SIZE            0x8000
#define HW_ITCM_END             (HW_ITCM + HW_ITCM_SIZE)

//----------------------------- DTCM
#ifndef SDK_ASM
#include    <nitro/types.h>
extern  u32 SDK_AUTOLOAD_DTCM_START[];
#define HW_DTCM                 ((u32)SDK_AUTOLOAD_DTCM_START)
#else
.extern SDK_AUTOLOAD_DTCM_START
#define HW_DTCM                 SDK_AUTOLOAD_DTCM_START
#endif
#define HW_DTCM_SIZE            0x4000
#define HW_DTCM_END             (HW_DTCM + HW_DTCM_SIZE)

//----------------------------- MAIN on NITRO hardware
#define HW_MAIN_MEM             0x02000000
#define HW_MAIN_MEM_SIZE        0x00400000
#define HW_MAIN_MEM_EX_SIZE     0x00800000
#define HW_MAIN_MEM_END         (HW_MAIN_MEM + HW_MAIN_MEM_SIZE)
#define HW_MAIN_MEM_EX_END      (HW_MAIN_MEM + HW_MAIN_MEM_EX_SIZE)

//----------------------------- MAIN on TWL hardware
#define HW_TWL_MAIN_MEM_SIZE        0x01000000
#define HW_TWL_MAIN_MEM_EX_SIZE     0x01000000

#define HW_TWL_MAIN_MEM             0x02000000
#define HW_TWL_MAIN_MEM_END         (HW_TWL_MAIN_MEM + HW_TWL_MAIN_MEM_SIZE)
#define HW_TWL_MAIN_MEM_IMAGE       0x0c000000
#define HW_TWL_MAIN_MEM_IMAGE_END   (HW_TWL_MAIN_MEM_IMAGE + HW_TWL_MAIN_MEM_SIZE)
#define HW_TWL_MAIN_MEM_EX          0x0d000000
#define HW_TWL_MAIN_MEM_EX_END      (HW_TWL_MAIN_MEM_EX + HW_TWL_MAIN_MEM_EX_SIZE)

//----------------------------- WRAM_AREA
#define HW_WRAM_AREA            0x03000000
#define HW_WRAM_AREA_HALF       (HW_WRAM_AREA+(HW_WRAM_AREA_SIZE/2))
#define HW_WRAM_AREA_END        0x04000000
#define HW_WRAM_AREA_SIZE       (HW_WRAM_AREA_END-HW_WRAM_AREA)

//----------------------------- Shared WRAMs
#define HW_WRAM_BASE            0x03000000
#define HW_WRAM_0_SIZE          0x4000
#define HW_WRAM_1_SIZE          0x4000
#define HW_WRAM_SIZE            (HW_WRAM_0_SIZE + HW_WRAM_1_SIZE)
#ifdef	SDK_BB
#define HW_WRAM_A_SIZE          0x20000
#define HW_WRAM_B_SIZE          0x20000
#define HW_WRAM_C_SIZE          0x20000
#else
#define HW_WRAM_A_SIZE          0x40000
#define HW_WRAM_B_SIZE          0x40000
#define HW_WRAM_C_SIZE          0x40000
#endif

#define HW_WRAM_0               (HW_WRAM_BASE + HW_WRAM_A_SIZE)
#define HW_WRAM_0_END           (HW_WRAM_0 + HW_WRAM_0_SIZE)
#define HW_WRAM_1               HW_WRAM_0_END
#define HW_WRAM_1_END           (HW_WRAM_1 + HW_WRAM_1_SIZE)
#define HW_WRAM                 HW_WRAM_0
#define HW_WRAM_END             HW_WRAM_1_END

#define HW_WRAM_B_OFFSET            0x0740000
#define HW_WRAM_C_OFFSET            0x0700000
#define HW_WRAM_B                   (HW_WRAM_BASE + HW_WRAM_B_OFFSET)
#ifdef SDK_BB
#define HW_WRAM_B_END               (HW_WRAM_B + (HW_WRAM_B_SIZE * 2))
#else
#define HW_WRAM_B_END               (HW_WRAM_B + HW_WRAM_B_SIZE)
#endif
#define HW_WRAM_C                   (HW_WRAM_BASE + HW_WRAM_C_OFFSET)
#ifdef SDK_BB
#define HW_WRAM_C_END               (HW_WRAM_C + (HW_WRAM_C_SIZE * 2))
#else
#define HW_WRAM_C_END               (HW_WRAM_C + HW_WRAM_C_SIZE)
#endif
#define HW_WRAM_B_OR_C_MIRROR       HW_WRAM_B_END
#ifdef SDK_BB
#define HW_WRAM_B_OR_C_MIRROR_END   (HW_WRAM_B_OR_C_MIRROR + (HW_WRAM_B_SIZE * 2)) // HW_WRAM_B_SIZE==HW_WRAM_C_SIZE
#else
#define HW_WRAM_B_OR_C_MIRROR_END   (HW_WRAM_B_OR_C_MIRROR + HW_WRAM_B_SIZE) // HW_WRAM_B_SIZE==HW_WRAM_C_SIZE
#endif

//----------------------------- IOs
#define HW_IOREG                0x04000000
#define HW_IOREG_END            0x05000000
#define HW_REG_BASE             HW_IOREG        // alias

//----------------------------- VRAMs
#define HW_BG_PLTT              0x05000000
#define HW_BG_PLTT_END          0x05000200
#define HW_BG_PLTT_SIZE         (HW_BG_PLTT_END - HW_BG_PLTT)
#define HW_OBJ_PLTT             0x05000200
#define HW_OBJ_PLTT_END         0x05000400
#define HW_OBJ_PLTT_SIZE        (HW_OBJ_PLTT_END - HW_OBJ_PLTT)
#define HW_PLTT                 HW_BG_PLTT
#define HW_PLTT_END             HW_OBJ_PLTT_END
#define HW_PLTT_SIZE            (HW_PLTT_END - HW_PLTT)
#define HW_DB_BG_PLTT           0x05000400
#define HW_DB_BG_PLTT_END       0x05000600
#define HW_DB_BG_PLTT_SIZE      (HW_DB_BG_PLTT_END - HW_DB_BG_PLTT)
#define HW_DB_OBJ_PLTT          0x05000600
#define HW_DB_OBJ_PLTT_END      0x05000800
#define HW_DB_OBJ_PLTT_SIZE     (HW_DB_OBJ_PLTT_END - HW_DB_OBJ_PLTT)
#define HW_DB_PLTT              HW_DB_BG_PLTT
#define HW_DB_PLTT_END          HW_DB_OBJ_PLTT_END
#define HW_DB_PLTT_SIZE         (HW_DB_PLTT_END - HW_DB_PLTT)
#define HW_BG_VRAM              0x06000000
#define HW_BG_VRAM_END          0x06080000
#define HW_BG_VRAM_SIZE         (HW_BG_VRAM_END - HW_BG_VRAM)
#define HW_DB_BG_VRAM           0x06200000
#define HW_DB_BG_VRAM_END       0x06220000
#define HW_DB_BG_VRAM_SIZE      (HW_DB_BG_VRAM_END - HW_DB_BG_VRAM)
#define HW_OBJ_VRAM             0x06400000
#define HW_OBJ_VRAM_END         0x06440000
#define HW_OBJ_VRAM_SIZE        (HW_OBJ_VRAM_END - HW_OBJ_VRAM)
#define HW_DB_OBJ_VRAM          0x06600000
#define HW_DB_OBJ_VRAM_END      0x06620000
#define HW_DB_OBJ_VRAM_SIZE     (HW_DB_OBJ_VRAM_END - HW_DB_OBJ_VRAM)
#define HW_LCDC_VRAM            0x06800000
#define HW_LCDC_VRAM_END        0x068A4000
#define HW_LCDC_VRAM_SIZE       (HW_LCDC_VRAM_END - HW_LCDC_VRAM)
#define HW_OAM                  0x07000000
#define HW_OAM_END              0x07000400
#define HW_OAM_SIZE             (HW_OAM_END - HW_OAM)
#define HW_DB_OAM               0x07000400
#define HW_DB_OAM_END           0x07000800
#define HW_DB_OAM_SIZE          (HW_DB_OAM_END - HW_DB_OAM)

//----------------------------- Cartridge Bus
#define HW_CTRDG_ROM            0x08000000
#define HW_CTRDG_ROM_END        0x0a000000
#define HW_CTRDG_RAM            0x0a000000
#define HW_CTRDG_RAM_END        0x0a010000

//----------------------------- System ROM
#define HW_BIOS_SIZE            0x00002000
#define HW_BIOS                 0xffff0000
#define HW_BIOS_END             (HW_BIOS + HW_BIOS_SIZE)
#define HW_TWL_BIOS_SIZE        0x00010000
#define HW_TWL_BIOS             HW_BIOS
#define HW_TWL_BIOS_END         (HW_TWL_BIOS + HW_TWL_BIOS_SIZE)

#define HW_RESET_VECTOR         HW_BIOS

//----------------------------- for request to ARM7
#define HW_EXT_WRAM_ARM7        0x06000000
#define HW_EXT_WRAM_ARM7_END    0x06040000
#define HW_EXT_WRAM_ARM7_SIZE   (HW_EXT_WRAM_ARM7_END-HW_EXT_WRAM_ARM7)

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
}   /* extern "C" */
#endif
#endif  /* TWL_HW_ARM9_MMAP_GLOBAL_H_ */
