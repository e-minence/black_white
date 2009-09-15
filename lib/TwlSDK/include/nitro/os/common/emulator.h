/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     emulator.h

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

#ifndef NITRO_OS_EMULATOR_H_
#define NITRO_OS_EMULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

//---- value of 'console not yet detect'
#define OSi_CONSOLE_NOT_DETECT    0xffffffff

//---- running program environment
#define OS_CONSOLE_MASK           0xff000000
#define OS_CONSOLE_TWL            0x01000000 // classified by OS_CONSOLE_TWLTYPE_xxx
#define OS_CONSOLE_TWLDEBUGGER    0x02000000 // classified by OS_CONSOLE_BOARD_xxx
#define OS_CONSOLE_RESERVE1       0x04000000
#define OS_CONSOLE_RESERVE2       0x08000000
#define OS_CONSOLE_ENSATA         0x10000000
#define OS_CONSOLE_ISEMULATOR     0x20000000
#define OS_CONSOLE_ISDEBUGGER     0x40000000
#define OS_CONSOLE_NITRO          0x80000000

//----  classified by the running environment
#define OS_CONSOLE_BOARD_MASK     0x00300000
#define OS_CONSOLE_BOARD_A9_A7    0x00100000
#define OS_CONSOLE_BOARD_A9       0x00200000

#define OS_CONSOLE_TWLTYPE_MASK   0x00c00000
#define OS_CONSOLE_TWLTYPE_RETAIL 0x00800000
#define OS_CONSOLE_TWLTYPE_DEV    0x00400000

#define OS_CONSOLE_EVALUATE       0x00080000

//---- device
#define OS_CONSOLE_DEV_MASK       0x000fc000
#define OS_CONSOLE_DEV_CARTRIDGE  0x00010000
#define OS_CONSOLE_DEV_CARD       0x00020000
#define OS_CONSOLE_DEV_NAND       0x00040000
#define OS_CONSOLE_DEV_SDCARD     0x00080000
#define OS_CONSOLE_DEV_MEMORY     0x00008000
#define OS_CONSOLE_DEV_DOWNLOAD   0x00004000

//---- main memory size
#define OS_CONSOLE_SIZE_MASK      0x0000000f
#define OS_CONSOLE_SIZE_4MB       0x00000001
#define OS_CONSOLE_SIZE_8MB       0x00000002
#define OS_CONSOLE_SIZE_16MB      0x00000004
#define OS_CONSOLE_SIZE_32MB      0x00000008


//---- for ARM7 memory checker in crt0
#define OS_CHIPTYPE_DEBUGGER_ADDR   (HW_SYS_CONF_BUF+4)
#define OS_CHIPTYPE_DEBUGGER_SIZE	1
#define OS_CHIPTYPE_DEBUGGER_MASK   0x3

#define OS_CHIPTYPE_JTAG_ADDR       (HW_SYS_CONF_BUF+5)
#define OS_CHIPTYPE_JTAG_SIZE		1
#define OS_CHIPTYPE_JTAG_MASK       0x2

#define OS_CHIPTYPE_SMX_ADDR        (HW_MMEMCHECKER_SUB)
#define OS_CHIPTYPE_SMX_SIZE        2
#define OS_CHIPTYPE_SMX_MASK        0x8000

typedef enum
{
    OS_CHIPTYPE_TWL        = 0,
    OS_CHIPTYPE_DEBUGGER_1 = 1,
    OS_CHIPTYPE_DEBUGGER_2 = 2,
    OS_CHIPTYPE_EVALUATE   = 3
}
OSChipType;


/*---------------------------------------------------------------------------*
  Name:         OS_GetConsoleType

  Description:  Get console type value
                This function returns a fixed value on FINALROM

  Arguments:    None

  Returns:		console type value
 *---------------------------------------------------------------------------*/
u32 OS_GetConsoleType(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetRunningConsoleType

  Description:  Get console type value
                This function returns a true value in spite of on FINALROM

  Arguments:    None

  Returns:      console type value.
 *---------------------------------------------------------------------------*/
u32 OS_GetRunningConsoleType(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsRunOnEmulator

  Description:  Detect software emulator Ensata

  Arguments:    None

  Returns:      TRUE  : running on Ensata
                FALSE : not running on Ensata
 *---------------------------------------------------------------------------*/
BOOL OS_IsRunOnEmulator(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsRunOnDebugger

  Description:  Detect running on debugger.

  Arguments:    None

  Returns:      TRUE  : running on debugger
                FALSE : not running on debugger
 *---------------------------------------------------------------------------*/
BOOL OS_IsRunOnDebugger(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsRunOnTWL

  Description:  check running platform

                This function is used in only Nitro-TWL hybrid mode.
                (In Nitro mode and TWL limited mode, treated as constant)

  Arguments:    None

  Returns:      TRUE  : running on TWL
                FALSE : running on NITRO
 *---------------------------------------------------------------------------*/
#ifndef SDK_TWL
#define OS_IsRunOnTwl()			  (FALSE)
#else
#ifdef SDK_TWLLTD
#define OS_IsRunOnTwl()			  (TRUE)
#else
BOOL OS_IsRunOnTwl(void);
#endif
#endif

//----------------------------------------------------------------
//---- for debug
#ifndef SDK_FINALROM
void OS_SetConsoleType(u32 type);
#else
#define OS_SetConsoleType(x)  ((void)0)
#endif

//---- for internal use
u32 OSi_DetectDebugger(void);
u32 OSi_DetectEmulator(void);

#ifndef SDK_TWLLTD
BOOL OSi_IsNitroModeOnTwl(void);
#else
#define OSi_IsNitroModeOnTwl()    (FALSE)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_EMULATOR_H_ */
#endif
