/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     context.h

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

#ifndef NITRO_OS_CONTEXT_H_
#define NITRO_OS_CONTEXT_H_

#ifdef SDK_ARM9
#include <nitro/cp/context.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
#define SDK_CONTEXT_HAS_SP_SVC

//----------------------------------------------------------------------------

#define OS_CONTEXT_CPSR      0
#define OS_CONTEXT_R0        4
#define OS_CONTEXT_R1        8
#define OS_CONTEXT_R2       12
#define OS_CONTEXT_R3       16
#define OS_CONTEXT_R4       20
#define OS_CONTEXT_R5       24
#define OS_CONTEXT_R6       28
#define OS_CONTEXT_R7       32
#define OS_CONTEXT_R8       36
#define OS_CONTEXT_R9       40
#define OS_CONTEXT_R10      44
#define OS_CONTEXT_R11      48
#define OS_CONTEXT_R12      52
#define OS_CONTEXT_R13      56
#define OS_CONTEXT_R14      60
#define OS_CONTEXT_PC_PLUS4 64         // Should be set to PC+4 for interrupt handling

#define OS_CONTEXT_SP       OS_CONTEXT_R13
#define OS_CONTEXT_LR       OS_CONTEXT_R14

#ifdef  SDK_CONTEXT_HAS_SP_SVC
#define OS_CONTEXT_R13_SVC  68
#define OS_CONTEXT_SP_SVC   OS_CONTEXT_R13_SVC
#endif

#ifdef SDK_ARM9
#ifdef SDK_CONTEXT_HAS_SP_SVC
#define OS_CONTEXT_CP_CONTEXT 72
#else
#define OS_CONTEXT_CP_CONTEXT 68
#endif
#endif


typedef struct OSContext
{
    u32     cpsr;
    u32     r[13];
    u32     sp;
    u32     lr;
    u32     pc_plus4;
#ifdef  SDK_CONTEXT_HAS_SP_SVC
    u32     sp_svc;
#endif
#ifdef  SDK_ARM9
    CPContext cp_context;
#endif
}
OSContext;


//----------------------------------------------------------------------------

void    OS_InitContext(OSContext *context, u32 newpc, u32 newsp);
BOOL    OS_SaveContext(OSContext *context);
void    OS_LoadContext(OSContext *context);
void    OS_DumpContext(OSContext *context);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_CONTEXT_H_ */
#endif
