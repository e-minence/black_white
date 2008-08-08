/*---------------------------------------------------------------------------*
  Project:  NitroSDK - IO Register List - 
  File:     nitro/hw/ARM9/ioreg_G2S.h

  Copyright 2003-2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
//
//  I was generated automatically, don't edit me directly!!!
//
#ifndef NITRO_HW_ARM9_IOREG_G2S_H_
#define NITRO_HW_ARM9_IOREG_G2S_H_

#ifndef SDK_ASM
#include <nitro/types.h>
#include <nitro/hw/ARM9/mmap_global.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Definition of Register offsets, addresses and variables.
 */


/* DB_BG0CNT */

#define REG_DB_BG0CNT_OFFSET                               0x1008
#define REG_DB_BG0CNT_ADDR                                 (HW_REG_BASE + REG_DB_BG0CNT_OFFSET)
#define reg_G2S_DB_BG0CNT                                  (*( REGType16v *) REG_DB_BG0CNT_ADDR)

/* DB_BG1CNT */

#define REG_DB_BG1CNT_OFFSET                               0x100a
#define REG_DB_BG1CNT_ADDR                                 (HW_REG_BASE + REG_DB_BG1CNT_OFFSET)
#define reg_G2S_DB_BG1CNT                                  (*( REGType16v *) REG_DB_BG1CNT_ADDR)

/* DB_BG2CNT */

#define REG_DB_BG2CNT_OFFSET                               0x100c
#define REG_DB_BG2CNT_ADDR                                 (HW_REG_BASE + REG_DB_BG2CNT_OFFSET)
#define reg_G2S_DB_BG2CNT                                  (*( REGType16v *) REG_DB_BG2CNT_ADDR)

/* DB_BG3CNT */

#define REG_DB_BG3CNT_OFFSET                               0x100e
#define REG_DB_BG3CNT_ADDR                                 (HW_REG_BASE + REG_DB_BG3CNT_OFFSET)
#define reg_G2S_DB_BG3CNT                                  (*( REGType16v *) REG_DB_BG3CNT_ADDR)

/* DB_BG0OFS */

#define REG_DB_BG0OFS_OFFSET                               0x1010
#define REG_DB_BG0OFS_ADDR                                 (HW_REG_BASE + REG_DB_BG0OFS_OFFSET)
#define reg_G2S_DB_BG0OFS                                  (*( REGType32v *) REG_DB_BG0OFS_ADDR)

/* DB_BG0HOFS */

#define REG_DB_BG0HOFS_OFFSET                              0x1010
#define REG_DB_BG0HOFS_ADDR                                (HW_REG_BASE + REG_DB_BG0HOFS_OFFSET)
#define reg_G2S_DB_BG0HOFS                                 (*( REGType16v *) REG_DB_BG0HOFS_ADDR)

/* DB_BG0VOFS */

#define REG_DB_BG0VOFS_OFFSET                              0x1012
#define REG_DB_BG0VOFS_ADDR                                (HW_REG_BASE + REG_DB_BG0VOFS_OFFSET)
#define reg_G2S_DB_BG0VOFS                                 (*( REGType16v *) REG_DB_BG0VOFS_ADDR)

/* DB_BG1OFS */

#define REG_DB_BG1OFS_OFFSET                               0x1014
#define REG_DB_BG1OFS_ADDR                                 (HW_REG_BASE + REG_DB_BG1OFS_OFFSET)
#define reg_G2S_DB_BG1OFS                                  (*( REGType32v *) REG_DB_BG1OFS_ADDR)

/* DB_BG1HOFS */

#define REG_DB_BG1HOFS_OFFSET                              0x1014
#define REG_DB_BG1HOFS_ADDR                                (HW_REG_BASE + REG_DB_BG1HOFS_OFFSET)
#define reg_G2S_DB_BG1HOFS                                 (*( REGType16v *) REG_DB_BG1HOFS_ADDR)

/* DB_BG1VOFS */

#define REG_DB_BG1VOFS_OFFSET                              0x1016
#define REG_DB_BG1VOFS_ADDR                                (HW_REG_BASE + REG_DB_BG1VOFS_OFFSET)
#define reg_G2S_DB_BG1VOFS                                 (*( REGType16v *) REG_DB_BG1VOFS_ADDR)

/* DB_BG2OFS */

#define REG_DB_BG2OFS_OFFSET                               0x1018
#define REG_DB_BG2OFS_ADDR                                 (HW_REG_BASE + REG_DB_BG2OFS_OFFSET)
#define reg_G2S_DB_BG2OFS                                  (*( REGType32v *) REG_DB_BG2OFS_ADDR)

/* DB_BG2HOFS */

#define REG_DB_BG2HOFS_OFFSET                              0x1018
#define REG_DB_BG2HOFS_ADDR                                (HW_REG_BASE + REG_DB_BG2HOFS_OFFSET)
#define reg_G2S_DB_BG2HOFS                                 (*( REGType16v *) REG_DB_BG2HOFS_ADDR)

/* DB_BG2VOFS */

#define REG_DB_BG2VOFS_OFFSET                              0x101a
#define REG_DB_BG2VOFS_ADDR                                (HW_REG_BASE + REG_DB_BG2VOFS_OFFSET)
#define reg_G2S_DB_BG2VOFS                                 (*( REGType16v *) REG_DB_BG2VOFS_ADDR)

/* DB_BG3OFS */

#define REG_DB_BG3OFS_OFFSET                               0x101c
#define REG_DB_BG3OFS_ADDR                                 (HW_REG_BASE + REG_DB_BG3OFS_OFFSET)
#define reg_G2S_DB_BG3OFS                                  (*( REGType32v *) REG_DB_BG3OFS_ADDR)

/* DB_BG3HOFS */

#define REG_DB_BG3HOFS_OFFSET                              0x101c
#define REG_DB_BG3HOFS_ADDR                                (HW_REG_BASE + REG_DB_BG3HOFS_OFFSET)
#define reg_G2S_DB_BG3HOFS                                 (*( REGType16v *) REG_DB_BG3HOFS_ADDR)

/* DB_BG3VOFS */

#define REG_DB_BG3VOFS_OFFSET                              0x101e
#define REG_DB_BG3VOFS_ADDR                                (HW_REG_BASE + REG_DB_BG3VOFS_OFFSET)
#define reg_G2S_DB_BG3VOFS                                 (*( REGType16v *) REG_DB_BG3VOFS_ADDR)

/* DB_BG2PA */

#define REG_DB_BG2PA_OFFSET                                0x1020
#define REG_DB_BG2PA_ADDR                                  (HW_REG_BASE + REG_DB_BG2PA_OFFSET)
#define reg_G2S_DB_BG2PA                                   (*( REGType16v *) REG_DB_BG2PA_ADDR)

/* DB_BG2PB */

#define REG_DB_BG2PB_OFFSET                                0x1022
#define REG_DB_BG2PB_ADDR                                  (HW_REG_BASE + REG_DB_BG2PB_OFFSET)
#define reg_G2S_DB_BG2PB                                   (*( REGType16v *) REG_DB_BG2PB_ADDR)

/* DB_BG2PC */

#define REG_DB_BG2PC_OFFSET                                0x1024
#define REG_DB_BG2PC_ADDR                                  (HW_REG_BASE + REG_DB_BG2PC_OFFSET)
#define reg_G2S_DB_BG2PC                                   (*( REGType16v *) REG_DB_BG2PC_ADDR)

/* DB_BG2PD */

#define REG_DB_BG2PD_OFFSET                                0x1026
#define REG_DB_BG2PD_ADDR                                  (HW_REG_BASE + REG_DB_BG2PD_OFFSET)
#define reg_G2S_DB_BG2PD                                   (*( REGType16v *) REG_DB_BG2PD_ADDR)

/* DB_BG2X */

#define REG_DB_BG2X_OFFSET                                 0x1028
#define REG_DB_BG2X_ADDR                                   (HW_REG_BASE + REG_DB_BG2X_OFFSET)
#define reg_G2S_DB_BG2X                                    (*( REGType32v *) REG_DB_BG2X_ADDR)

/* DB_BG2Y */

#define REG_DB_BG2Y_OFFSET                                 0x102c
#define REG_DB_BG2Y_ADDR                                   (HW_REG_BASE + REG_DB_BG2Y_OFFSET)
#define reg_G2S_DB_BG2Y                                    (*( REGType32v *) REG_DB_BG2Y_ADDR)

/* DB_BG3PA */

#define REG_DB_BG3PA_OFFSET                                0x1030
#define REG_DB_BG3PA_ADDR                                  (HW_REG_BASE + REG_DB_BG3PA_OFFSET)
#define reg_G2S_DB_BG3PA                                   (*( REGType16v *) REG_DB_BG3PA_ADDR)

/* DB_BG3PB */

#define REG_DB_BG3PB_OFFSET                                0x1032
#define REG_DB_BG3PB_ADDR                                  (HW_REG_BASE + REG_DB_BG3PB_OFFSET)
#define reg_G2S_DB_BG3PB                                   (*( REGType16v *) REG_DB_BG3PB_ADDR)

/* DB_BG3PC */

#define REG_DB_BG3PC_OFFSET                                0x1034
#define REG_DB_BG3PC_ADDR                                  (HW_REG_BASE + REG_DB_BG3PC_OFFSET)
#define reg_G2S_DB_BG3PC                                   (*( REGType16v *) REG_DB_BG3PC_ADDR)

/* DB_BG3PD */

#define REG_DB_BG3PD_OFFSET                                0x1036
#define REG_DB_BG3PD_ADDR                                  (HW_REG_BASE + REG_DB_BG3PD_OFFSET)
#define reg_G2S_DB_BG3PD                                   (*( REGType16v *) REG_DB_BG3PD_ADDR)

/* DB_BG3X */

#define REG_DB_BG3X_OFFSET                                 0x1038
#define REG_DB_BG3X_ADDR                                   (HW_REG_BASE + REG_DB_BG3X_OFFSET)
#define reg_G2S_DB_BG3X                                    (*( REGType32v *) REG_DB_BG3X_ADDR)

/* DB_BG3Y */

#define REG_DB_BG3Y_OFFSET                                 0x103c
#define REG_DB_BG3Y_ADDR                                   (HW_REG_BASE + REG_DB_BG3Y_OFFSET)
#define reg_G2S_DB_BG3Y                                    (*( REGType32v *) REG_DB_BG3Y_ADDR)

/* DB_WIN0H */

#define REG_DB_WIN0H_OFFSET                                0x1040
#define REG_DB_WIN0H_ADDR                                  (HW_REG_BASE + REG_DB_WIN0H_OFFSET)
#define reg_G2S_DB_WIN0H                                   (*( REGType16v *) REG_DB_WIN0H_ADDR)

/* DB_WIN1H */

#define REG_DB_WIN1H_OFFSET                                0x1042
#define REG_DB_WIN1H_ADDR                                  (HW_REG_BASE + REG_DB_WIN1H_OFFSET)
#define reg_G2S_DB_WIN1H                                   (*( REGType16v *) REG_DB_WIN1H_ADDR)

/* DB_WIN0V */

#define REG_DB_WIN0V_OFFSET                                0x1044
#define REG_DB_WIN0V_ADDR                                  (HW_REG_BASE + REG_DB_WIN0V_OFFSET)
#define reg_G2S_DB_WIN0V                                   (*( REGType16v *) REG_DB_WIN0V_ADDR)

/* DB_WIN1V */

#define REG_DB_WIN1V_OFFSET                                0x1046
#define REG_DB_WIN1V_ADDR                                  (HW_REG_BASE + REG_DB_WIN1V_OFFSET)
#define reg_G2S_DB_WIN1V                                   (*( REGType16v *) REG_DB_WIN1V_ADDR)

/* DB_WININ */

#define REG_DB_WININ_OFFSET                                0x1048
#define REG_DB_WININ_ADDR                                  (HW_REG_BASE + REG_DB_WININ_OFFSET)
#define reg_G2S_DB_WININ                                   (*( REGType16v *) REG_DB_WININ_ADDR)

/* DB_WINOUT */

#define REG_DB_WINOUT_OFFSET                               0x104a
#define REG_DB_WINOUT_ADDR                                 (HW_REG_BASE + REG_DB_WINOUT_OFFSET)
#define reg_G2S_DB_WINOUT                                  (*( REGType16v *) REG_DB_WINOUT_ADDR)

/* DB_MOSAIC */

#define REG_DB_MOSAIC_OFFSET                               0x104c
#define REG_DB_MOSAIC_ADDR                                 (HW_REG_BASE + REG_DB_MOSAIC_OFFSET)
#define reg_G2S_DB_MOSAIC                                  (*( REGType16v *) REG_DB_MOSAIC_ADDR)

/* DB_BLDCNT */

#define REG_DB_BLDCNT_OFFSET                               0x1050
#define REG_DB_BLDCNT_ADDR                                 (HW_REG_BASE + REG_DB_BLDCNT_OFFSET)
#define reg_G2S_DB_BLDCNT                                  (*( REGType16v *) REG_DB_BLDCNT_ADDR)

/* DB_BLDALPHA */

#define REG_DB_BLDALPHA_OFFSET                             0x1052
#define REG_DB_BLDALPHA_ADDR                               (HW_REG_BASE + REG_DB_BLDALPHA_OFFSET)
#define reg_G2S_DB_BLDALPHA                                (*( REGType16v *) REG_DB_BLDALPHA_ADDR)

/* DB_BLDY */

#define REG_DB_BLDY_OFFSET                                 0x1054
#define REG_DB_BLDY_ADDR                                   (HW_REG_BASE + REG_DB_BLDY_OFFSET)
#define reg_G2S_DB_BLDY                                    (*( REGType16v *) REG_DB_BLDY_ADDR)


/*
 * Definitions of Register fields
 */


/* DB_BG0CNT */

#define REG_G2S_DB_BG0CNT_SCREENSIZE_SHIFT                 14
#define REG_G2S_DB_BG0CNT_SCREENSIZE_SIZE                  2
#define REG_G2S_DB_BG0CNT_SCREENSIZE_MASK                  0xc000

#define REG_G2S_DB_BG0CNT_BGPLTTSLOT_SHIFT                 13
#define REG_G2S_DB_BG0CNT_BGPLTTSLOT_SIZE                  1
#define REG_G2S_DB_BG0CNT_BGPLTTSLOT_MASK                  0x2000

#define REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT                 8
#define REG_G2S_DB_BG0CNT_SCREENBASE_SIZE                  5
#define REG_G2S_DB_BG0CNT_SCREENBASE_MASK                  0x1f00

#define REG_G2S_DB_BG0CNT_COLORMODE_SHIFT                  7
#define REG_G2S_DB_BG0CNT_COLORMODE_SIZE                   1
#define REG_G2S_DB_BG0CNT_COLORMODE_MASK                   0x0080

#define REG_G2S_DB_BG0CNT_MOSAIC_SHIFT                     6
#define REG_G2S_DB_BG0CNT_MOSAIC_SIZE                      1
#define REG_G2S_DB_BG0CNT_MOSAIC_MASK                      0x0040

#define REG_G2S_DB_BG0CNT_CHARBASE_SHIFT                   2
#define REG_G2S_DB_BG0CNT_CHARBASE_SIZE                    4
#define REG_G2S_DB_BG0CNT_CHARBASE_MASK                    0x003c

#define REG_G2S_DB_BG0CNT_PRIORITY_SHIFT                   0
#define REG_G2S_DB_BG0CNT_PRIORITY_SIZE                    2
#define REG_G2S_DB_BG0CNT_PRIORITY_MASK                    0x0003

#ifndef SDK_ASM
#define REG_G2S_DB_BG0CNT_FIELD( screensize, bgplttslot, screenbase, colormode, mosaic, charbase, priority ) \
    (u16)( \
    ((u32)(screensize) << REG_G2S_DB_BG0CNT_SCREENSIZE_SHIFT) | \
    ((u32)(bgplttslot) << REG_G2S_DB_BG0CNT_BGPLTTSLOT_SHIFT) | \
    ((u32)(screenbase) << REG_G2S_DB_BG0CNT_SCREENBASE_SHIFT) | \
    ((u32)(colormode) << REG_G2S_DB_BG0CNT_COLORMODE_SHIFT) | \
    ((u32)(mosaic) << REG_G2S_DB_BG0CNT_MOSAIC_SHIFT) | \
    ((u32)(charbase) << REG_G2S_DB_BG0CNT_CHARBASE_SHIFT) | \
    ((u32)(priority) << REG_G2S_DB_BG0CNT_PRIORITY_SHIFT))
#endif


/* DB_BG1CNT */

#define REG_G2S_DB_BG1CNT_SCREENSIZE_SHIFT                 14
#define REG_G2S_DB_BG1CNT_SCREENSIZE_SIZE                  2
#define REG_G2S_DB_BG1CNT_SCREENSIZE_MASK                  0xc000

#define REG_G2S_DB_BG1CNT_BGPLTTSLOT_SHIFT                 13
#define REG_G2S_DB_BG1CNT_BGPLTTSLOT_SIZE                  1
#define REG_G2S_DB_BG1CNT_BGPLTTSLOT_MASK                  0x2000

#define REG_G2S_DB_BG1CNT_SCREENBASE_SHIFT                 8
#define REG_G2S_DB_BG1CNT_SCREENBASE_SIZE                  5
#define REG_G2S_DB_BG1CNT_SCREENBASE_MASK                  0x1f00

#define REG_G2S_DB_BG1CNT_COLORMODE_SHIFT                  7
#define REG_G2S_DB_BG1CNT_COLORMODE_SIZE                   1
#define REG_G2S_DB_BG1CNT_COLORMODE_MASK                   0x0080

#define REG_G2S_DB_BG1CNT_MOSAIC_SHIFT                     6
#define REG_G2S_DB_BG1CNT_MOSAIC_SIZE                      1
#define REG_G2S_DB_BG1CNT_MOSAIC_MASK                      0x0040

#define REG_G2S_DB_BG1CNT_CHARBASE_SHIFT                   2
#define REG_G2S_DB_BG1CNT_CHARBASE_SIZE                    4
#define REG_G2S_DB_BG1CNT_CHARBASE_MASK                    0x003c

#define REG_G2S_DB_BG1CNT_PRIORITY_SHIFT                   0
#define REG_G2S_DB_BG1CNT_PRIORITY_SIZE                    2
#define REG_G2S_DB_BG1CNT_PRIORITY_MASK                    0x0003

#ifndef SDK_ASM
#define REG_G2S_DB_BG1CNT_FIELD( screensize, bgplttslot, screenbase, colormode, mosaic, charbase, priority ) \
    (u16)( \
    ((u32)(screensize) << REG_G2S_DB_BG1CNT_SCREENSIZE_SHIFT) | \
    ((u32)(bgplttslot) << REG_G2S_DB_BG1CNT_BGPLTTSLOT_SHIFT) | \
    ((u32)(screenbase) << REG_G2S_DB_BG1CNT_SCREENBASE_SHIFT) | \
    ((u32)(colormode) << REG_G2S_DB_BG1CNT_COLORMODE_SHIFT) | \
    ((u32)(mosaic) << REG_G2S_DB_BG1CNT_MOSAIC_SHIFT) | \
    ((u32)(charbase) << REG_G2S_DB_BG1CNT_CHARBASE_SHIFT) | \
    ((u32)(priority) << REG_G2S_DB_BG1CNT_PRIORITY_SHIFT))
#endif


/* DB_BG2CNT */

#define REG_G2S_DB_BG2CNT_SCREENSIZE_SHIFT                 14
#define REG_G2S_DB_BG2CNT_SCREENSIZE_SIZE                  2
#define REG_G2S_DB_BG2CNT_SCREENSIZE_MASK                  0xc000

#define REG_G2S_DB_BG2CNT_AREAOVER_SHIFT                   13
#define REG_G2S_DB_BG2CNT_AREAOVER_SIZE                    1
#define REG_G2S_DB_BG2CNT_AREAOVER_MASK                    0x2000

#define REG_G2S_DB_BG2CNT_SCREENBASE_SHIFT                 8
#define REG_G2S_DB_BG2CNT_SCREENBASE_SIZE                  5
#define REG_G2S_DB_BG2CNT_SCREENBASE_MASK                  0x1f00

#define REG_G2S_DB_BG2CNT_COLORMODE_SHIFT                  7
#define REG_G2S_DB_BG2CNT_COLORMODE_SIZE                   1
#define REG_G2S_DB_BG2CNT_COLORMODE_MASK                   0x0080

#define REG_G2S_DB_BG2CNT_MOSAIC_SHIFT                     6
#define REG_G2S_DB_BG2CNT_MOSAIC_SIZE                      1
#define REG_G2S_DB_BG2CNT_MOSAIC_MASK                      0x0040

#define REG_G2S_DB_BG2CNT_CHARBASE_SHIFT                   2
#define REG_G2S_DB_BG2CNT_CHARBASE_SIZE                    4
#define REG_G2S_DB_BG2CNT_CHARBASE_MASK                    0x003c

#define REG_G2S_DB_BG2CNT_PRIORITY_SHIFT                   0
#define REG_G2S_DB_BG2CNT_PRIORITY_SIZE                    2
#define REG_G2S_DB_BG2CNT_PRIORITY_MASK                    0x0003

#ifndef SDK_ASM
#define REG_G2S_DB_BG2CNT_FIELD( screensize, areaover, screenbase, colormode, mosaic, charbase, priority ) \
    (u16)( \
    ((u32)(screensize) << REG_G2S_DB_BG2CNT_SCREENSIZE_SHIFT) | \
    ((u32)(areaover) << REG_G2S_DB_BG2CNT_AREAOVER_SHIFT) | \
    ((u32)(screenbase) << REG_G2S_DB_BG2CNT_SCREENBASE_SHIFT) | \
    ((u32)(colormode) << REG_G2S_DB_BG2CNT_COLORMODE_SHIFT) | \
    ((u32)(mosaic) << REG_G2S_DB_BG2CNT_MOSAIC_SHIFT) | \
    ((u32)(charbase) << REG_G2S_DB_BG2CNT_CHARBASE_SHIFT) | \
    ((u32)(priority) << REG_G2S_DB_BG2CNT_PRIORITY_SHIFT))
#endif


/* DB_BG3CNT */

#define REG_G2S_DB_BG3CNT_SCREENSIZE_SHIFT                 14
#define REG_G2S_DB_BG3CNT_SCREENSIZE_SIZE                  2
#define REG_G2S_DB_BG3CNT_SCREENSIZE_MASK                  0xc000

#define REG_G2S_DB_BG3CNT_AREAOVER_SHIFT                   13
#define REG_G2S_DB_BG3CNT_AREAOVER_SIZE                    1
#define REG_G2S_DB_BG3CNT_AREAOVER_MASK                    0x2000

#define REG_G2S_DB_BG3CNT_SCREENBASE_SHIFT                 8
#define REG_G2S_DB_BG3CNT_SCREENBASE_SIZE                  5
#define REG_G2S_DB_BG3CNT_SCREENBASE_MASK                  0x1f00

#define REG_G2S_DB_BG3CNT_COLORMODE_SHIFT                  7
#define REG_G2S_DB_BG3CNT_COLORMODE_SIZE                   1
#define REG_G2S_DB_BG3CNT_COLORMODE_MASK                   0x0080

#define REG_G2S_DB_BG3CNT_MOSAIC_SHIFT                     6
#define REG_G2S_DB_BG3CNT_MOSAIC_SIZE                      1
#define REG_G2S_DB_BG3CNT_MOSAIC_MASK                      0x0040

#define REG_G2S_DB_BG3CNT_CHARBASE_SHIFT                   2
#define REG_G2S_DB_BG3CNT_CHARBASE_SIZE                    4
#define REG_G2S_DB_BG3CNT_CHARBASE_MASK                    0x003c

#define REG_G2S_DB_BG3CNT_PRIORITY_SHIFT                   0
#define REG_G2S_DB_BG3CNT_PRIORITY_SIZE                    2
#define REG_G2S_DB_BG3CNT_PRIORITY_MASK                    0x0003

#ifndef SDK_ASM
#define REG_G2S_DB_BG3CNT_FIELD( screensize, areaover, screenbase, colormode, mosaic, charbase, priority ) \
    (u16)( \
    ((u32)(screensize) << REG_G2S_DB_BG3CNT_SCREENSIZE_SHIFT) | \
    ((u32)(areaover) << REG_G2S_DB_BG3CNT_AREAOVER_SHIFT) | \
    ((u32)(screenbase) << REG_G2S_DB_BG3CNT_SCREENBASE_SHIFT) | \
    ((u32)(colormode) << REG_G2S_DB_BG3CNT_COLORMODE_SHIFT) | \
    ((u32)(mosaic) << REG_G2S_DB_BG3CNT_MOSAIC_SHIFT) | \
    ((u32)(charbase) << REG_G2S_DB_BG3CNT_CHARBASE_SHIFT) | \
    ((u32)(priority) << REG_G2S_DB_BG3CNT_PRIORITY_SHIFT))
#endif


/* DB_BG0OFS */

#define REG_G2S_DB_BG0OFS_VOFFSET_SHIFT                    16
#define REG_G2S_DB_BG0OFS_VOFFSET_SIZE                     9
#define REG_G2S_DB_BG0OFS_VOFFSET_MASK                     0x01ff0000

#define REG_G2S_DB_BG0OFS_HOFFSET_SHIFT                    0
#define REG_G2S_DB_BG0OFS_HOFFSET_SIZE                     9
#define REG_G2S_DB_BG0OFS_HOFFSET_MASK                     0x000001ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG0OFS_FIELD( voffset, hoffset ) \
    (u32)( \
    ((u32)(voffset) << REG_G2S_DB_BG0OFS_VOFFSET_SHIFT) | \
    ((u32)(hoffset) << REG_G2S_DB_BG0OFS_HOFFSET_SHIFT))
#endif


/* DB_BG0HOFS */

#define REG_G2S_DB_BG0HOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG0HOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG0HOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG0HOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG0HOFS_OFFSET_SHIFT))
#endif


/* DB_BG0VOFS */

#define REG_G2S_DB_BG0VOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG0VOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG0VOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG0VOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG0VOFS_OFFSET_SHIFT))
#endif


/* DB_BG1OFS */

#define REG_G2S_DB_BG1OFS_VOFFSET_SHIFT                    16
#define REG_G2S_DB_BG1OFS_VOFFSET_SIZE                     9
#define REG_G2S_DB_BG1OFS_VOFFSET_MASK                     0x01ff0000

#define REG_G2S_DB_BG1OFS_HOFFSET_SHIFT                    0
#define REG_G2S_DB_BG1OFS_HOFFSET_SIZE                     9
#define REG_G2S_DB_BG1OFS_HOFFSET_MASK                     0x000001ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG1OFS_FIELD( voffset, hoffset ) \
    (u32)( \
    ((u32)(voffset) << REG_G2S_DB_BG1OFS_VOFFSET_SHIFT) | \
    ((u32)(hoffset) << REG_G2S_DB_BG1OFS_HOFFSET_SHIFT))
#endif


/* DB_BG1HOFS */

#define REG_G2S_DB_BG1HOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG1HOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG1HOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG1HOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG1HOFS_OFFSET_SHIFT))
#endif


/* DB_BG1VOFS */

#define REG_G2S_DB_BG1VOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG1VOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG1VOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG1VOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG1VOFS_OFFSET_SHIFT))
#endif


/* DB_BG2OFS */

#define REG_G2S_DB_BG2OFS_VOFFSET_SHIFT                    16
#define REG_G2S_DB_BG2OFS_VOFFSET_SIZE                     9
#define REG_G2S_DB_BG2OFS_VOFFSET_MASK                     0x01ff0000

#define REG_G2S_DB_BG2OFS_HOFFSET_SHIFT                    0
#define REG_G2S_DB_BG2OFS_HOFFSET_SIZE                     9
#define REG_G2S_DB_BG2OFS_HOFFSET_MASK                     0x000001ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2OFS_FIELD( voffset, hoffset ) \
    (u32)( \
    ((u32)(voffset) << REG_G2S_DB_BG2OFS_VOFFSET_SHIFT) | \
    ((u32)(hoffset) << REG_G2S_DB_BG2OFS_HOFFSET_SHIFT))
#endif


/* DB_BG2HOFS */

#define REG_G2S_DB_BG2HOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG2HOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG2HOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2HOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG2HOFS_OFFSET_SHIFT))
#endif


/* DB_BG2VOFS */

#define REG_G2S_DB_BG2VOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG2VOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG2VOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2VOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG2VOFS_OFFSET_SHIFT))
#endif


/* DB_BG3OFS */

#define REG_G2S_DB_BG3OFS_VOFFSET_SHIFT                    16
#define REG_G2S_DB_BG3OFS_VOFFSET_SIZE                     9
#define REG_G2S_DB_BG3OFS_VOFFSET_MASK                     0x01ff0000

#define REG_G2S_DB_BG3OFS_HOFFSET_SHIFT                    0
#define REG_G2S_DB_BG3OFS_HOFFSET_SIZE                     9
#define REG_G2S_DB_BG3OFS_HOFFSET_MASK                     0x000001ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3OFS_FIELD( voffset, hoffset ) \
    (u32)( \
    ((u32)(voffset) << REG_G2S_DB_BG3OFS_VOFFSET_SHIFT) | \
    ((u32)(hoffset) << REG_G2S_DB_BG3OFS_HOFFSET_SHIFT))
#endif


/* DB_BG3HOFS */

#define REG_G2S_DB_BG3HOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG3HOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG3HOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3HOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG3HOFS_OFFSET_SHIFT))
#endif


/* DB_BG3VOFS */

#define REG_G2S_DB_BG3VOFS_OFFSET_SHIFT                    0
#define REG_G2S_DB_BG3VOFS_OFFSET_SIZE                     9
#define REG_G2S_DB_BG3VOFS_OFFSET_MASK                     0x01ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3VOFS_FIELD( offset ) \
    (u16)( \
    ((u32)(offset) << REG_G2S_DB_BG3VOFS_OFFSET_SHIFT))
#endif


/* DB_BG2PA */

#define REG_G2S_DB_BG2PA_S_SHIFT                           15
#define REG_G2S_DB_BG2PA_S_SIZE                            1
#define REG_G2S_DB_BG2PA_S_MASK                            0x8000

#define REG_G2S_DB_BG2PA_INTEGER_DX_SHIFT                  8
#define REG_G2S_DB_BG2PA_INTEGER_DX_SIZE                   7
#define REG_G2S_DB_BG2PA_INTEGER_DX_MASK                   0x7f00

#define REG_G2S_DB_BG2PA_DECIMAL_DX_SHIFT                  0
#define REG_G2S_DB_BG2PA_DECIMAL_DX_SIZE                   8
#define REG_G2S_DB_BG2PA_DECIMAL_DX_MASK                   0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2PA_FIELD( s, integer_dx, decimal_dx ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG2PA_S_SHIFT) | \
    ((u32)(integer_dx) << REG_G2S_DB_BG2PA_INTEGER_DX_SHIFT) | \
    ((u32)(decimal_dx) << REG_G2S_DB_BG2PA_DECIMAL_DX_SHIFT))
#endif


/* DB_BG2PB */

#define REG_G2S_DB_BG2PB_S_SHIFT                           15
#define REG_G2S_DB_BG2PB_S_SIZE                            1
#define REG_G2S_DB_BG2PB_S_MASK                            0x8000

#define REG_G2S_DB_BG2PB_INTEGER_DMX_SHIFT                 8
#define REG_G2S_DB_BG2PB_INTEGER_DMX_SIZE                  7
#define REG_G2S_DB_BG2PB_INTEGER_DMX_MASK                  0x7f00

#define REG_G2S_DB_BG2PB_DECIMAL_DMX_SHIFT                 0
#define REG_G2S_DB_BG2PB_DECIMAL_DMX_SIZE                  8
#define REG_G2S_DB_BG2PB_DECIMAL_DMX_MASK                  0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2PB_FIELD( s, integer_dmx, decimal_dmx ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG2PB_S_SHIFT) | \
    ((u32)(integer_dmx) << REG_G2S_DB_BG2PB_INTEGER_DMX_SHIFT) | \
    ((u32)(decimal_dmx) << REG_G2S_DB_BG2PB_DECIMAL_DMX_SHIFT))
#endif


/* DB_BG2PC */

#define REG_G2S_DB_BG2PC_S_SHIFT                           15
#define REG_G2S_DB_BG2PC_S_SIZE                            1
#define REG_G2S_DB_BG2PC_S_MASK                            0x8000

#define REG_G2S_DB_BG2PC_INTEGER_DY_SHIFT                  8
#define REG_G2S_DB_BG2PC_INTEGER_DY_SIZE                   7
#define REG_G2S_DB_BG2PC_INTEGER_DY_MASK                   0x7f00

#define REG_G2S_DB_BG2PC_DECIMAL_DY_SHIFT                  0
#define REG_G2S_DB_BG2PC_DECIMAL_DY_SIZE                   8
#define REG_G2S_DB_BG2PC_DECIMAL_DY_MASK                   0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2PC_FIELD( s, integer_dy, decimal_dy ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG2PC_S_SHIFT) | \
    ((u32)(integer_dy) << REG_G2S_DB_BG2PC_INTEGER_DY_SHIFT) | \
    ((u32)(decimal_dy) << REG_G2S_DB_BG2PC_DECIMAL_DY_SHIFT))
#endif


/* DB_BG2PD */

#define REG_G2S_DB_BG2PD_S_SHIFT                           15
#define REG_G2S_DB_BG2PD_S_SIZE                            1
#define REG_G2S_DB_BG2PD_S_MASK                            0x8000

#define REG_G2S_DB_BG2PD_INTEGER_DMY_SHIFT                 8
#define REG_G2S_DB_BG2PD_INTEGER_DMY_SIZE                  7
#define REG_G2S_DB_BG2PD_INTEGER_DMY_MASK                  0x7f00

#define REG_G2S_DB_BG2PD_DECIMAL_DMY_SHIFT                 0
#define REG_G2S_DB_BG2PD_DECIMAL_DMY_SIZE                  8
#define REG_G2S_DB_BG2PD_DECIMAL_DMY_MASK                  0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2PD_FIELD( s, integer_dmy, decimal_dmy ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG2PD_S_SHIFT) | \
    ((u32)(integer_dmy) << REG_G2S_DB_BG2PD_INTEGER_DMY_SHIFT) | \
    ((u32)(decimal_dmy) << REG_G2S_DB_BG2PD_DECIMAL_DMY_SHIFT))
#endif


/* DB_BG2X */

#define REG_G2S_DB_BG2X_S_SHIFT                            27
#define REG_G2S_DB_BG2X_S_SIZE                             1
#define REG_G2S_DB_BG2X_S_MASK                             0x08000000

#define REG_G2S_DB_BG2X_INTEGER_SX_SHIFT                   8
#define REG_G2S_DB_BG2X_INTEGER_SX_SIZE                    19
#define REG_G2S_DB_BG2X_INTEGER_SX_MASK                    0x07ffff00

#define REG_G2S_DB_BG2X_DECIMAL_SX_SHIFT                   0
#define REG_G2S_DB_BG2X_DECIMAL_SX_SIZE                    8
#define REG_G2S_DB_BG2X_DECIMAL_SX_MASK                    0x000000ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2X_FIELD( s, integer_sx, decimal_sx ) \
    (u32)( \
    ((u32)(s) << REG_G2S_DB_BG2X_S_SHIFT) | \
    ((u32)(integer_sx) << REG_G2S_DB_BG2X_INTEGER_SX_SHIFT) | \
    ((u32)(decimal_sx) << REG_G2S_DB_BG2X_DECIMAL_SX_SHIFT))
#endif


/* DB_BG2Y */

#define REG_G2S_DB_BG2Y_S_SHIFT                            27
#define REG_G2S_DB_BG2Y_S_SIZE                             1
#define REG_G2S_DB_BG2Y_S_MASK                             0x08000000

#define REG_G2S_DB_BG2Y_INTEGER_SY_SHIFT                   8
#define REG_G2S_DB_BG2Y_INTEGER_SY_SIZE                    19
#define REG_G2S_DB_BG2Y_INTEGER_SY_MASK                    0x07ffff00

#define REG_G2S_DB_BG2Y_DECIMAL_SY_SHIFT                   0
#define REG_G2S_DB_BG2Y_DECIMAL_SY_SIZE                    8
#define REG_G2S_DB_BG2Y_DECIMAL_SY_MASK                    0x000000ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG2Y_FIELD( s, integer_sy, decimal_sy ) \
    (u32)( \
    ((u32)(s) << REG_G2S_DB_BG2Y_S_SHIFT) | \
    ((u32)(integer_sy) << REG_G2S_DB_BG2Y_INTEGER_SY_SHIFT) | \
    ((u32)(decimal_sy) << REG_G2S_DB_BG2Y_DECIMAL_SY_SHIFT))
#endif


/* DB_BG3PA */

#define REG_G2S_DB_BG3PA_S_SHIFT                           15
#define REG_G2S_DB_BG3PA_S_SIZE                            1
#define REG_G2S_DB_BG3PA_S_MASK                            0x8000

#define REG_G2S_DB_BG3PA_INTEGER_DX_SHIFT                  8
#define REG_G2S_DB_BG3PA_INTEGER_DX_SIZE                   7
#define REG_G2S_DB_BG3PA_INTEGER_DX_MASK                   0x7f00

#define REG_G2S_DB_BG3PA_DECIMAL_DX_SHIFT                  0
#define REG_G2S_DB_BG3PA_DECIMAL_DX_SIZE                   8
#define REG_G2S_DB_BG3PA_DECIMAL_DX_MASK                   0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3PA_FIELD( s, integer_dx, decimal_dx ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG3PA_S_SHIFT) | \
    ((u32)(integer_dx) << REG_G2S_DB_BG3PA_INTEGER_DX_SHIFT) | \
    ((u32)(decimal_dx) << REG_G2S_DB_BG3PA_DECIMAL_DX_SHIFT))
#endif


/* DB_BG3PB */

#define REG_G2S_DB_BG3PB_S_SHIFT                           15
#define REG_G2S_DB_BG3PB_S_SIZE                            1
#define REG_G2S_DB_BG3PB_S_MASK                            0x8000

#define REG_G2S_DB_BG3PB_INTEGER_DMX_SHIFT                 8
#define REG_G2S_DB_BG3PB_INTEGER_DMX_SIZE                  7
#define REG_G2S_DB_BG3PB_INTEGER_DMX_MASK                  0x7f00

#define REG_G2S_DB_BG3PB_DECIMAL_DMX_SHIFT                 0
#define REG_G2S_DB_BG3PB_DECIMAL_DMX_SIZE                  8
#define REG_G2S_DB_BG3PB_DECIMAL_DMX_MASK                  0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3PB_FIELD( s, integer_dmx, decimal_dmx ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG3PB_S_SHIFT) | \
    ((u32)(integer_dmx) << REG_G2S_DB_BG3PB_INTEGER_DMX_SHIFT) | \
    ((u32)(decimal_dmx) << REG_G2S_DB_BG3PB_DECIMAL_DMX_SHIFT))
#endif


/* DB_BG3PC */

#define REG_G2S_DB_BG3PC_S_SHIFT                           15
#define REG_G2S_DB_BG3PC_S_SIZE                            1
#define REG_G2S_DB_BG3PC_S_MASK                            0x8000

#define REG_G2S_DB_BG3PC_INTEGER_DY_SHIFT                  8
#define REG_G2S_DB_BG3PC_INTEGER_DY_SIZE                   7
#define REG_G2S_DB_BG3PC_INTEGER_DY_MASK                   0x7f00

#define REG_G2S_DB_BG3PC_DECIMAL_DY_SHIFT                  0
#define REG_G2S_DB_BG3PC_DECIMAL_DY_SIZE                   8
#define REG_G2S_DB_BG3PC_DECIMAL_DY_MASK                   0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3PC_FIELD( s, integer_dy, decimal_dy ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG3PC_S_SHIFT) | \
    ((u32)(integer_dy) << REG_G2S_DB_BG3PC_INTEGER_DY_SHIFT) | \
    ((u32)(decimal_dy) << REG_G2S_DB_BG3PC_DECIMAL_DY_SHIFT))
#endif


/* DB_BG3PD */

#define REG_G2S_DB_BG3PD_S_SHIFT                           15
#define REG_G2S_DB_BG3PD_S_SIZE                            1
#define REG_G2S_DB_BG3PD_S_MASK                            0x8000

#define REG_G2S_DB_BG3PD_INTEGER_DMY_SHIFT                 8
#define REG_G2S_DB_BG3PD_INTEGER_DMY_SIZE                  7
#define REG_G2S_DB_BG3PD_INTEGER_DMY_MASK                  0x7f00

#define REG_G2S_DB_BG3PD_DECIMAL_DMY_SHIFT                 0
#define REG_G2S_DB_BG3PD_DECIMAL_DMY_SIZE                  8
#define REG_G2S_DB_BG3PD_DECIMAL_DMY_MASK                  0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3PD_FIELD( s, integer_dmy, decimal_dmy ) \
    (u16)( \
    ((u32)(s) << REG_G2S_DB_BG3PD_S_SHIFT) | \
    ((u32)(integer_dmy) << REG_G2S_DB_BG3PD_INTEGER_DMY_SHIFT) | \
    ((u32)(decimal_dmy) << REG_G2S_DB_BG3PD_DECIMAL_DMY_SHIFT))
#endif


/* DB_BG3X */

#define REG_G2S_DB_BG3X_S_SHIFT                            27
#define REG_G2S_DB_BG3X_S_SIZE                             1
#define REG_G2S_DB_BG3X_S_MASK                             0x08000000

#define REG_G2S_DB_BG3X_INTEGER_SX_SHIFT                   8
#define REG_G2S_DB_BG3X_INTEGER_SX_SIZE                    19
#define REG_G2S_DB_BG3X_INTEGER_SX_MASK                    0x07ffff00

#define REG_G2S_DB_BG3X_DECIMAL_SX_SHIFT                   0
#define REG_G2S_DB_BG3X_DECIMAL_SX_SIZE                    8
#define REG_G2S_DB_BG3X_DECIMAL_SX_MASK                    0x000000ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3X_FIELD( s, integer_sx, decimal_sx ) \
    (u32)( \
    ((u32)(s) << REG_G2S_DB_BG3X_S_SHIFT) | \
    ((u32)(integer_sx) << REG_G2S_DB_BG3X_INTEGER_SX_SHIFT) | \
    ((u32)(decimal_sx) << REG_G2S_DB_BG3X_DECIMAL_SX_SHIFT))
#endif


/* DB_BG3Y */

#define REG_G2S_DB_BG3Y_S_SHIFT                            27
#define REG_G2S_DB_BG3Y_S_SIZE                             1
#define REG_G2S_DB_BG3Y_S_MASK                             0x08000000

#define REG_G2S_DB_BG3Y_INTEGER_SY_SHIFT                   8
#define REG_G2S_DB_BG3Y_INTEGER_SY_SIZE                    19
#define REG_G2S_DB_BG3Y_INTEGER_SY_MASK                    0x07ffff00

#define REG_G2S_DB_BG3Y_DECIMAL_SY_SHIFT                   0
#define REG_G2S_DB_BG3Y_DECIMAL_SY_SIZE                    8
#define REG_G2S_DB_BG3Y_DECIMAL_SY_MASK                    0x000000ff

#ifndef SDK_ASM
#define REG_G2S_DB_BG3Y_FIELD( s, integer_sy, decimal_sy ) \
    (u32)( \
    ((u32)(s) << REG_G2S_DB_BG3Y_S_SHIFT) | \
    ((u32)(integer_sy) << REG_G2S_DB_BG3Y_INTEGER_SY_SHIFT) | \
    ((u32)(decimal_sy) << REG_G2S_DB_BG3Y_DECIMAL_SY_SHIFT))
#endif


/* DB_WIN0H */

#define REG_G2S_DB_WIN0H_LEFTX_SHIFT                       8
#define REG_G2S_DB_WIN0H_LEFTX_SIZE                        8
#define REG_G2S_DB_WIN0H_LEFTX_MASK                        0xff00

#define REG_G2S_DB_WIN0H_RIGHTX_SHIFT                      0
#define REG_G2S_DB_WIN0H_RIGHTX_SIZE                       8
#define REG_G2S_DB_WIN0H_RIGHTX_MASK                       0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_WIN0H_FIELD( leftx, rightx ) \
    (u16)( \
    ((u32)(leftx) << REG_G2S_DB_WIN0H_LEFTX_SHIFT) | \
    ((u32)(rightx) << REG_G2S_DB_WIN0H_RIGHTX_SHIFT))
#endif


/* DB_WIN1H */

#define REG_G2S_DB_WIN1H_LEFTX_SHIFT                       8
#define REG_G2S_DB_WIN1H_LEFTX_SIZE                        8
#define REG_G2S_DB_WIN1H_LEFTX_MASK                        0xff00

#define REG_G2S_DB_WIN1H_RIGHTX_SHIFT                      0
#define REG_G2S_DB_WIN1H_RIGHTX_SIZE                       8
#define REG_G2S_DB_WIN1H_RIGHTX_MASK                       0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_WIN1H_FIELD( leftx, rightx ) \
    (u16)( \
    ((u32)(leftx) << REG_G2S_DB_WIN1H_LEFTX_SHIFT) | \
    ((u32)(rightx) << REG_G2S_DB_WIN1H_RIGHTX_SHIFT))
#endif


/* DB_WIN0V */

#define REG_G2S_DB_WIN0V_UPY_SHIFT                         8
#define REG_G2S_DB_WIN0V_UPY_SIZE                          8
#define REG_G2S_DB_WIN0V_UPY_MASK                          0xff00

#define REG_G2S_DB_WIN0V_DOWNY_SHIFT                       0
#define REG_G2S_DB_WIN0V_DOWNY_SIZE                        8
#define REG_G2S_DB_WIN0V_DOWNY_MASK                        0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_WIN0V_FIELD( upy, downy ) \
    (u16)( \
    ((u32)(upy) << REG_G2S_DB_WIN0V_UPY_SHIFT) | \
    ((u32)(downy) << REG_G2S_DB_WIN0V_DOWNY_SHIFT))
#endif


/* DB_WIN1V */

#define REG_G2S_DB_WIN1V_UPY_SHIFT                         8
#define REG_G2S_DB_WIN1V_UPY_SIZE                          8
#define REG_G2S_DB_WIN1V_UPY_MASK                          0xff00

#define REG_G2S_DB_WIN1V_DOWNY_SHIFT                       0
#define REG_G2S_DB_WIN1V_DOWNY_SIZE                        8
#define REG_G2S_DB_WIN1V_DOWNY_MASK                        0x00ff

#ifndef SDK_ASM
#define REG_G2S_DB_WIN1V_FIELD( upy, downy ) \
    (u16)( \
    ((u32)(upy) << REG_G2S_DB_WIN1V_UPY_SHIFT) | \
    ((u32)(downy) << REG_G2S_DB_WIN1V_DOWNY_SHIFT))
#endif


/* DB_WININ */

#define REG_G2S_DB_WININ_WIN1IN_SHIFT                      8
#define REG_G2S_DB_WININ_WIN1IN_SIZE                       6
#define REG_G2S_DB_WININ_WIN1IN_MASK                       0x3f00

#define REG_G2S_DB_WININ_WIN0IN_SHIFT                      0
#define REG_G2S_DB_WININ_WIN0IN_SIZE                       6
#define REG_G2S_DB_WININ_WIN0IN_MASK                       0x003f

#ifndef SDK_ASM
#define REG_G2S_DB_WININ_FIELD( win1in, win0in ) \
    (u16)( \
    ((u32)(win1in) << REG_G2S_DB_WININ_WIN1IN_SHIFT) | \
    ((u32)(win0in) << REG_G2S_DB_WININ_WIN0IN_SHIFT))
#endif


/* DB_WINOUT */

#define REG_G2S_DB_WINOUT_OBJWININ_SHIFT                   8
#define REG_G2S_DB_WINOUT_OBJWININ_SIZE                    6
#define REG_G2S_DB_WINOUT_OBJWININ_MASK                    0x3f00

#define REG_G2S_DB_WINOUT_WINOUT_SHIFT                     0
#define REG_G2S_DB_WINOUT_WINOUT_SIZE                      6
#define REG_G2S_DB_WINOUT_WINOUT_MASK                      0x003f

#ifndef SDK_ASM
#define REG_G2S_DB_WINOUT_FIELD( objwinin, winout ) \
    (u16)( \
    ((u32)(objwinin) << REG_G2S_DB_WINOUT_OBJWININ_SHIFT) | \
    ((u32)(winout) << REG_G2S_DB_WINOUT_WINOUT_SHIFT))
#endif


/* DB_MOSAIC */

#define REG_G2S_DB_MOSAIC_OBJVSIZE_SHIFT                   12
#define REG_G2S_DB_MOSAIC_OBJVSIZE_SIZE                    4
#define REG_G2S_DB_MOSAIC_OBJVSIZE_MASK                    0xf000

#define REG_G2S_DB_MOSAIC_OBJHSIZE_SHIFT                   8
#define REG_G2S_DB_MOSAIC_OBJHSIZE_SIZE                    4
#define REG_G2S_DB_MOSAIC_OBJHSIZE_MASK                    0x0f00

#define REG_G2S_DB_MOSAIC_BGVSIZE_SHIFT                    4
#define REG_G2S_DB_MOSAIC_BGVSIZE_SIZE                     4
#define REG_G2S_DB_MOSAIC_BGVSIZE_MASK                     0x00f0

#define REG_G2S_DB_MOSAIC_BGHSIZE_SHIFT                    0
#define REG_G2S_DB_MOSAIC_BGHSIZE_SIZE                     4
#define REG_G2S_DB_MOSAIC_BGHSIZE_MASK                     0x000f

#ifndef SDK_ASM
#define REG_G2S_DB_MOSAIC_FIELD( objvsize, objhsize, bgvsize, bghsize ) \
    (u16)( \
    ((u32)(objvsize) << REG_G2S_DB_MOSAIC_OBJVSIZE_SHIFT) | \
    ((u32)(objhsize) << REG_G2S_DB_MOSAIC_OBJHSIZE_SHIFT) | \
    ((u32)(bgvsize) << REG_G2S_DB_MOSAIC_BGVSIZE_SHIFT) | \
    ((u32)(bghsize) << REG_G2S_DB_MOSAIC_BGHSIZE_SHIFT))
#endif


/* DB_BLDCNT */

#define REG_G2S_DB_BLDCNT_PLANE2_SHIFT                     8
#define REG_G2S_DB_BLDCNT_PLANE2_SIZE                      6
#define REG_G2S_DB_BLDCNT_PLANE2_MASK                      0x3f00

#define REG_G2S_DB_BLDCNT_EFFECT_SHIFT                     6
#define REG_G2S_DB_BLDCNT_EFFECT_SIZE                      2
#define REG_G2S_DB_BLDCNT_EFFECT_MASK                      0x00c0

#define REG_G2S_DB_BLDCNT_PLANE1_SHIFT                     0
#define REG_G2S_DB_BLDCNT_PLANE1_SIZE                      6
#define REG_G2S_DB_BLDCNT_PLANE1_MASK                      0x003f

#ifndef SDK_ASM
#define REG_G2S_DB_BLDCNT_FIELD( plane2, effect, plane1 ) \
    (u16)( \
    ((u32)(plane2) << REG_G2S_DB_BLDCNT_PLANE2_SHIFT) | \
    ((u32)(effect) << REG_G2S_DB_BLDCNT_EFFECT_SHIFT) | \
    ((u32)(plane1) << REG_G2S_DB_BLDCNT_PLANE1_SHIFT))
#endif


/* DB_BLDALPHA */

#define REG_G2S_DB_BLDALPHA_EVB_SHIFT                      8
#define REG_G2S_DB_BLDALPHA_EVB_SIZE                       5
#define REG_G2S_DB_BLDALPHA_EVB_MASK                       0x1f00

#define REG_G2S_DB_BLDALPHA_EVA_SHIFT                      0
#define REG_G2S_DB_BLDALPHA_EVA_SIZE                       5
#define REG_G2S_DB_BLDALPHA_EVA_MASK                       0x001f

#ifndef SDK_ASM
#define REG_G2S_DB_BLDALPHA_FIELD( evb, eva ) \
    (u16)( \
    ((u32)(evb) << REG_G2S_DB_BLDALPHA_EVB_SHIFT) | \
    ((u32)(eva) << REG_G2S_DB_BLDALPHA_EVA_SHIFT))
#endif


/* DB_BLDY */

#define REG_G2S_DB_BLDY_EVY_SHIFT                          0
#define REG_G2S_DB_BLDY_EVY_SIZE                           5
#define REG_G2S_DB_BLDY_EVY_MASK                           0x001f

#ifndef SDK_ASM
#define REG_G2S_DB_BLDY_FIELD( evy ) \
    (u16)( \
    ((u32)(evy) << REG_G2S_DB_BLDY_EVY_SHIFT))
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_HW_ARM9_IOREG_G2S_H_ */
#endif
