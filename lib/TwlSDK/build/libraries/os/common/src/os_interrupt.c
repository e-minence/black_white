/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - OS
  File:     os_interrupt.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-06#$
  $Rev: 9229 $
  $Author: yada $

 *---------------------------------------------------------------------------*/
#include <nitro/hw/common/armArch.h>

#ifdef SDK_NITRO
#include        <nitro/os/common/interrupt.h>
#else
#include        <twl/os/common/interrupt.h>
#endif

#include <nitro/os/common/system.h>

/*---------------------------------------------------------------------------*
  Name:         OS_InitIrqTable

  Description:  initialize IRQ handler table

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
extern OSThreadQueue OSi_IrqThreadQueue;

void OS_InitIrqTable(void)
{
    //---- initialize thread queue for irq
    OS_InitThreadQueue(&OSi_IrqThreadQueue);

#ifdef SDK_ARM7
    //---- clear VBlank counter
    OSi_SetVBlankCount(0);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqFunction

  Description:  set IRQ handler function

  Arguments:    intrBit         IRQ mask bit
                function        funtion to set

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetIrqFunction(OSIrqMask intrBit, OSIrqFunction function)
{
    int     i;
    OSIrqCallbackInfo *info;

    for (i = 0; i < OS_IRQ_TABLE_MAX; i++)
    {
        if (intrBit & 1)
        {
            info = NULL;

            // ---- if dma(0-3)
            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT)
            {
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT];
            }
#ifdef SDK_TWL
			// ---- if new dma(0-3)
			else if (REG_OS_IE_ND0_SHIFT <= i && i <= REG_OS_IE_ND3_SHIFT)
			{
				info = &OSi_IrqCallbackInfo[i - REG_OS_IE_ND0_SHIFT + OSi_IRQCALLBACK_NO_NDMA0];
			}
#endif
            // ---- if timer(0-3)
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)
            {
                info = &OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT + OSi_IRQCALLBACK_NO_TIMER0];
            }
#ifdef SDK_ARM7
            // ---- if vblank
            else if (REG_OS_IE_VB_SHIFT == i)
            {
                info = &OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK];
            }
#endif
            //---- other interrupt
            else
            {
                OS_IRQTable[i] = function;
            }

            if (info)
            {
                info->func = (void (*)(void *))function;
                info->arg = 0;
                info->enable = TRUE;
            }

        }
        intrBit >>= 1;
    }
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
void OS_SetIrqFunctionEx(OSIrqMask intrBit, OSIrqFunction function)
{
    int     		   i;
    OSIrqCallbackInfo *info = NULL;

    for (i = 0; i < OS_IRQ_TABLE2_MAX; i++)
    {
        if (intrBit & 1)
        {
            info = NULL;
			OS_IRQTable2[i] = function;

            if (info)
            {
                info->func = (void (*)(void *))function;
                info->arg = 0;
                info->enable = TRUE;
            }

        }
        intrBit >>= 1;
    }
}
#endif // defined(SDK_TWL) && defined(SDK_ARM7)

/*---------------------------------------------------------------------------*
  Name:         OS_GetIrqFunction

  Description:  get IRQ handler function

  Arguments:    intrBit         IRQ mask bit

  Returns:      None
 *---------------------------------------------------------------------------*/
OSIrqFunction OS_GetIrqFunction(OSIrqMask intrBit)
{
    int     i;
    OSIrqFunction *funcPtr = &OS_IRQTable[0];

    for (i = 0; i < OS_IRQ_TABLE_MAX; i++)
    {
        if (intrBit & 1)
        {
            //---- if dma(0-3)
            if (REG_OS_IE_D0_SHIFT <= i && i <= REG_OS_IE_D3_SHIFT)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT].func;
            }
#ifdef TWL_SDK
            //---- if ndma(0-3)
            else if (REG_OS_IE_ND0_SHIFT <= i && i <= REG_OS_IE_ND3_SHIFT)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[i - REG_OS_IE_D0_SHIFT +
                                                           OSi_IRQCALLBACK_NO_NDMA0].func;
            }
#endif
            //---- if timer(0-3)
            else if (REG_OS_IE_T0_SHIFT <= i && i <= REG_OS_IE_T3_SHIFT)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[i - REG_OS_IE_T0_SHIFT +
                                                           OSi_IRQCALLBACK_NO_TIMER0].func;
            }
#ifdef SDK_ARM7
            else if (REG_OS_IE_VB_SHIFT == i)
            {
                return (void (*)(void))OSi_IrqCallbackInfo[OSi_IRQCALLBACK_NO_VBLANK].func;
            }
#endif

            //---- other interrupt
            return *funcPtr;
        }
        intrBit >>= 1;
        funcPtr++;
    }
    return 0;
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqFunction OS_GetIrqFunctionEx(OSIrqMask intrBit)
{
    int     i;
    OSIrqFunction *funcPtr = &OS_IRQTable2[0]; // skip IE part

    for (i = 0; i < OS_IRQ_TABLE2_MAX; i++)
    {
        if (intrBit & 1)
        {
            return *funcPtr;
        }
        intrBit >>= 1;
        funcPtr++;
    }
    return 0;
}
#endif // defined(SDK_TWL) && defined(SDK_ARM7)


/*---------------------------------------------------------------------------*
  Name:         OSi_EnterDmaCallback

  Description:  enter dma callback handler and arg

  Arguments:    dmaNo       dma number to set callback
                callback    callback for dma interrupt
                arg         its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_EnterDmaCallback(u32 dmaNo, void (*callback) (void *), void *arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_D0_SHIFT + dmaNo));

    //---- enter callback
    OSi_IrqCallbackInfo[dmaNo].func = callback;
    OSi_IrqCallbackInfo[dmaNo].arg = arg;

    //---- remember IRQ mask bit
    OSi_IrqCallbackInfo[dmaNo].enable = OS_EnableIrqMask(imask) & imask;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_EnterNDmaCallback

  Description:  enter new dma callback handler and arg

  Arguments:    dmaNo       dma number to set callback
                callback    callback for dma interrupt
                arg         its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static void OSi_EnterNDmaCallback_ltdmain(u32 dmaNo, void (*callback) (void *), void *arg);
static void OSi_EnterNDmaCallback_ltdmain(u32 dmaNo, void (*callback) (void *), void *arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_ND0_SHIFT + dmaNo));

    //---- enter callback
    OSi_IrqCallbackInfo[dmaNo + OSi_IRQCALLBACK_NO_NDMA0].func = callback;
    OSi_IrqCallbackInfo[dmaNo + OSi_IRQCALLBACK_NO_NDMA0].arg = arg;

    //---- remember IRQ mask bit
    OSi_IrqCallbackInfo[dmaNo + OSi_IRQCALLBACK_NO_NDMA0].enable = OS_EnableIrqMask(imask) & imask;
}
#include <twl/ltdmain_end.h>
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void OSi_EnterNDmaCallback(u32 dmaNo, void (*callback) (void *), void *arg)
{
    OSi_EnterNDmaCallback_ltdmain(dmaNo, callback, arg);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OSi_EnterTimerCallback

  Description:  enter timer callback handler and arg

  Arguments:    timerNo     timer number to set callback
                callback    callback for timer interrupt
                arg         its argument

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_EnterTimerCallback(u32 timerNo, void (*callback) (void *), void *arg)
{
    OSIrqMask imask = (1UL << (REG_OS_IE_T0_SHIFT + timerNo));

    //---- enter callback
    OSi_IrqCallbackInfo[timerNo + OSi_IRQCALLBACK_NO_TIMER0].func = callback;
    OSi_IrqCallbackInfo[timerNo + OSi_IRQCALLBACK_NO_TIMER0].arg = arg;

    //---- remember IRQ mask bit (force to be ENABLE)
    (void)OS_EnableIrqMask(imask);
    OSi_IrqCallbackInfo[timerNo + OSi_IRQCALLBACK_NO_TIMER0].enable = TRUE;
}

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
//================================================================================
//         IrqMask
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqMask

  Description:  set irq factor

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_SetIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}


#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqMask OS_SetIrqMaskEx(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE2;
    reg_OS_IE2 = (u16)intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_EnableIrqMask

  Description:  set specified irq factor

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_EnableIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = prep | intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqMask OS_EnableIrqMaskEx(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE2;
    reg_OS_IE2 = (u16)(prep | intr);
    (void)OS_RestoreIrq(ime);
    return prep;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_DisableIrqMask

  Description:  unset specified irq factor

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_DisableIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE;
    reg_OS_IE = prep & ~intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if defined(SDK_TWL) & defined(SDK_ARM7)
OSIrqMask OS_DisableIrqMaskEx(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IE2;
    reg_OS_IE2 = (u16)(prep & ~intr);
    (void)OS_RestoreIrq(ime);
    return prep;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_ResetRequestIrqMask

  Description:  reset IF bit
                (setting bit causes to clear bit for interrupt)

  Arguments:    intr        irq factor

  Returns:      previous factors
 *---------------------------------------------------------------------------*/
OSIrqMask OS_ResetRequestIrqMask(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IF;
    reg_OS_IF = intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
OSIrqMask OS_ResetRequestIrqMaskEx(OSIrqMask intr)
{
    BOOL    ime = OS_DisableIrq();     // IME disable
    OSIrqMask prep = reg_OS_IF2;
    reg_OS_IF2 = (u16)intr;
    (void)OS_RestoreIrq(ime);
    return prep;
}
#endif

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif



//================================================================================
//              IRQ STACK CHECKER
//================================================================================
extern unsigned long SDK_IRQ_STACKSIZE[];
#define OSi_IRQ_STACKSIZE               ((int)SDK_IRQ_STACKSIZE)

//---- defs for irq stack
#ifdef SDK_ARM9
#  define  OSi_IRQ_STACK_TOP                (HW_DTCM_SVC_STACK - ((s32)SDK_IRQ_STACKSIZE))
#  define  OSi_IRQ_STACK_BOTTOM             HW_DTCM_SVC_STACK
#else
#  define  OSi_IRQ_STACK_TOP                (HW_PRV_WRAM_IRQ_STACK_END - ((s32)SDK_IRQ_STACKSIZE))
#  define  OSi_IRQ_STACK_BOTTOM             HW_PRV_WRAM_IRQ_STACK_END
#endif

//---- Stack CheckNumber
#ifdef SDK_ARM9
#  define  OSi_IRQ_STACK_CHECKNUM_BOTTOM     0xfddb597dUL
#  define  OSi_IRQ_STACK_CHECKNUM_TOP        0x7bf9dd5bUL
#  define  OSi_IRQ_STACK_CHECKNUM_WARN       0x597dfbd9UL
#else
#  define  OSi_IRQ_STACK_CHECKNUM_BOTTOM     0xd73bfdf7UL
#  define  OSi_IRQ_STACK_CHECKNUM_TOP        0xfbdd37bbUL
#  define  OSi_IRQ_STACK_CHECKNUM_WARN       0xbdf7db3dUL
#endif

static u32 OSi_IrqStackWarningOffset = 0;

/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqStackChecker

  Description:  set irq stack check number to irq stack

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetIrqStackChecker(void)
{
    *(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32)) = OSi_IRQ_STACK_CHECKNUM_BOTTOM;
    *(u32 *)(OSi_IRQ_STACK_TOP) = OSi_IRQ_STACK_CHECKNUM_TOP;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetIrqStackWarningOffset

  Description:  Set warning level for irq stack

  Arguments:    offset : offset from stack top. must be multiple of 4

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetIrqStackWarningOffset(u32 offset)
{
    SDK_ASSERTMSG((offset & 3) == 0, "Offset must be aligned by 4");
    SDK_ASSERTMSG(offset > 0, "Cannot set warning level to stack top.");
    SDK_ASSERTMSG(offset < ((u32)SDK_IRQ_STACKSIZE), "Cannot set warning level over stack bottom.");

    //---- remember warning offset
    OSi_IrqStackWarningOffset = offset;

    //---- set Stack CheckNum
    if (offset != 0)
    {
        *(u32 *)(OSi_IRQ_STACK_TOP + offset) = OSi_IRQ_STACK_CHECKNUM_WARN;
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetIrqStackStatus

  Description:  check irq stack. check each CheckNUM.
                return result.

  Arguments:    None

  Returns:      0 (OS_STACK_NO_ERROR)        no error
                OS_STACK_OVERFLOW            overflow
                OS_STACK_ABOUT_TO_OVERFLOW   about to overflow
                OS_STACK_UNDERFLOW           underflow
 *---------------------------------------------------------------------------*/
OSStackStatus OS_GetIrqStackStatus(void)
{
    //OS_Printf("CHECK OF %x\n", (*(u32 *)(OSi_IRQ_STACK_TOP) ) );
    //OS_Printf("CHECK AO %x\n", (*(u32 *)(OSi_IRQ_STACK_TOP + OSi_IrqStackWarningOffset) ) );
    //OS_Printf("CHECK UF %x\n", (*(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32))) );

    //OS_Printf(" - OF %x\n", OSi_IRQ_STACK_CHECKNUM_TOP);
    //OS_Printf(" - AO %x\n", OSi_IRQ_STACK_CHECKNUM_WARN);
    //OS_Printf(" - UF %x\n", OSi_IRQ_STACK_CHECKNUM_BOTTOM);

    //---- Check if overflow
    if (*(u32 *)(OSi_IRQ_STACK_TOP) != OSi_IRQ_STACK_CHECKNUM_TOP)
    {
        return OS_STACK_OVERFLOW;
    }
    //---- Check if about to overflow
    else if (OSi_IrqStackWarningOffset
             && *(u32 *)(OSi_IRQ_STACK_TOP + OSi_IrqStackWarningOffset) !=
             OSi_IRQ_STACK_CHECKNUM_WARN)
    {
        return OS_STACK_ABOUT_TO_OVERFLOW;
    }
    //---- Check if underFlow
    else if (*(u32 *)(OSi_IRQ_STACK_BOTTOM - sizeof(u32)) != OSi_IRQ_STACK_CHECKNUM_BOTTOM)
    {
        return OS_STACK_UNDERFLOW;
    }
    //---- No Error, return.
    else
    {
        return OS_STACK_NO_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_CheckIrqStack

  Description:  check irq stack. check each CheckNUM.
                if changed, display warning and halt.

  Arguments:    file     file name displayed when stack overflow
                line     line number displayed when stack overflow

  Returns:      None
                ( if error occurred, never return )
 *---------------------------------------------------------------------------*/
static char *OSi_CheckIrqStack_mesg[] = {
    "overflow", "about to overflow", "underflow"
};

#ifndef SDK_FINALROM
#ifndef SDK_NO_MESSAGE
void OSi_CheckIrqStack(char *file, int line)
{
    OSStackStatus st = OS_GetIrqStackStatus();

    if (st == OS_STACK_NO_ERROR)
    {
        return;
    }

    OSi_Panic(file, line, "irq stack %s.\nirq stack area: %08x-%08x, warning offset: %x",
              OSi_CheckIrqStack_mesg[(int)st - 1],
              OSi_IRQ_STACK_TOP, OSi_IRQ_STACK_BOTTOM, OSi_IrqStackWarningOffset);
    // Never return
}
#endif
#endif
