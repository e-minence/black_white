/*---------------------------------------------------------------------------*
  Project:  TwlSDK - libraries - OS
  File:     os_irqHandler.c

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
#include        <nitro/code32.h>
#include        <nitro/types.h>
#include        <nitro/os/common/interrupt.h>
#include        <nitro/os/common/thread.h>
#include        <nitro/os/common/system.h>

#ifdef SDK_NITRO
# ifdef SDK_ARM9
# include        <nitro/hw/ARM9/mmap_global.h>
# include        <nitro/hw/ARM9/ioreg_OS.h>
# else  // SDK_ARM9
# include        <nitro/hw/ARM7/mmap_global.h>
# include        <nitro/hw/ARM7/ioreg_OS.h>
# endif // SDK_ARM9
#else
# ifdef SDK_ARM9
# include        <twl/hw/ARM9/mmap_global.h>
# include        <twl/hw/ARM9/ioreg_OS.h>
# else  // SDK_ARM9
# include        <twl/hw/ARM7/mmap_global.h>
# include        <twl/hw/ARM7/ioreg_OS.h>
# endif // SDK_ARM9
#endif


#ifdef      SDK_ARM9
#include    <nitro/dtcm_begin.h>
#endif

//---- thread queue for interrupt
OSThreadQueue OSi_IrqThreadQueue = { NULL, NULL };

#ifdef      SDK_ARM9
#include    <nitro/dtcm_end.h>
#include    <nitro/itcm_begin.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_IrqHandler

  Description:  IRQ handler. call handler according to OS_InterruptTable

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
asm void OS_IrqHandler( void )
{
        stmfd   sp!, { lr }                     // save LR

        // get IE address
        mov     r12,      #HW_REG_BASE
        add     r12, r12, #REG_IE_OFFSET        // r12: REG_IE address

        // get IME
        ldr     r1, [ r12, #REG_IME_ADDR - REG_IE_ADDR ]  // r1: IME

        // if IME==0 then return (without changing IF)
        cmp     r1, #0
        ldmeqfd sp!, { pc }

        // get IE&IF
        ldmia   r12, { r1-r2 }                  // r1: IE, r2: IF
        ands    r1, r1, r2                      // r1: IE & IF

        // if IE&IF==0 then return (without changing IF)
        ldmeqfd sp!, { pc }


#if     defined(SDK_ARM9) && !defined(SDK_CWBUG_PROC_OPT)
        //--------------------------------------------------
        // IRQ HANDLING CODE for ARCHITECTURE VERSION 5
        //--------------------------------------------------
        
        // get lowest 1 bit
        mov     r3, #1<<31
@1:     clz     r0, r1                  // count zero of high bit
        bics    r1, r1, r3, LSR r0
        bne     @1

        // clear IF
        mov     r1, r3, LSR r0
        str     r1, [ r12, #REG_IF_ADDR - REG_IE_ADDR ]

        rsbs    r0, r0, #31

#else //defined(SDK_ARM9) && !defined(SDK_CWBUG_PROC_OPT)
        //--------------------------------------------------
        // IRQ HANDLING CODE for ARCHITECTURE VERSION 4
        //--------------------------------------------------
        mov     r3, #1
        mov     r0, #0
@1:     ands    r2, r1, r3, LSL r0              // count zero of high bit
        addeq   r0, r0, #1
        beq     @1

        // clear IF
        str     r2, [ r12, #REG_IF_ADDR - REG_IE_ADDR ]
#endif //defined(SDK_ARM9) && !defined(SDK_CWBUG_PROC_OPT)

        // get jump vector
#ifdef  SDK_DEBUG
        cmp     r0, #OS_IRQ_TABLE_MAX
@2:     bge     @2                              // Error Trap
#endif//SDK_DEBUG
        ldr     r1, =OS_IRQTable
        ldr     r0, [ r1, r0, LSL #2 ]
        
        ldr     lr, =OS_IrqHandler_ThreadSwitch
        bx      r0      // set return address for thread rescheduling
}



/*---------------------------------------------------------------------------*
  Name:         OS_IRQHandler_ThreadSwitch

  Description:  ���荞�ݕ��򏈗��i�e�[�u�� OS_InterruptTable �����j

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
asm void OS_IrqHandler_ThreadSwitch(void)
{
        //--------------------------------------------------
        // wakeup threads in OSi_IrqThreadQueue
        //--------------------------------------------------
           ldr             r12, =OSi_IrqThreadQueue
           mov             r3,  #0                      // avoid stall
           ldr             r12, [r12, #OSThreadQueue.head] // r12 = OSi_IrqThreadQueue.head
           mov             r2,  #OS_THREAD_STATE_READY  // avoid stall
           cmp             r12, #0

           beq             @thread_switch                                  // if r12 == 0 exit

@1:        str             r2,  [r12, #OSThread.state]
           str             r3,  [r12, #OSThread.queue]
           str             r3,  [r12, #OSThread.link.prev]
           ldr             r0,  [r12, #OSThread.link.next]
           str             r3,  [r12, #OSThread.link.next]
           mov             r12,  r0

           cmp             r12, #0
           bne             @1

           ldr             r12, =OSi_IrqThreadQueue
           str             r3, [r12, #OSThreadQueue.head]  // clear OSi_IrqThreadQueue.head
           str             r3, [r12, #OSThreadQueue.tail]  // clear OSi_IrqThreadQueue.tail

           ldr             r12, =OSi_ThreadInfo                    // need to do scheduling
           mov             r1, #1
           strh            r1, [ r12, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING ]


@thread_switch:
        //--------------------------------------------------
        // THREAD SWITCH
        //--------------------------------------------------
        // pseudo code
        //
        // if ( isNeedRescheduling == FALSE ) return;
        // isNeedRescheduling = FALSE;
        //
        // // OS_SelectThread
        // OSThread* t = OSi_ThreadInfo.list;
        // while( t && ! OS_IsThreadRunnable( t ) ){ t = t->next; }
        // return t;
        //
        // select:
        // current = CurrentThread;
        // if ( next == current ) return;
        // CurrentThread = next;
        // OS_SaveContext( current );
        // OS_LoadContext( next );
        // 
        
        // [[[ new OS_SelectThread ]]]
        ldr     r12, =OSi_ThreadInfo
        ldrh    r1, [ r12, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING ]
        cmp     r1, #0
        ldreq   pc, [ sp ], #4          // return if OSi_IsNeedResceduling == 0
        
        mov     r1, #0
        strh    r1, [ r12, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING ]

        // ---- OS_SelectThread (disable FIQ to support IS-Debugger snooping thread information)
        mov     r3, #HW_PSR_IRQ_MODE|HW_PSR_FIQ_DISABLE|HW_PSR_IRQ_DISABLE|HW_PSR_ARM_STATE
        msr     cpsr_c, r3

        add     r2, r12, #OS_THREADINFO_OFFSET_LIST // r2 = &OSi_ThreadInfo.list
        ldr     r1, [r2]                            // r1 = *r2 = TopOfList
@11:
        cmp     r1, #0
        ldrneh  r0, [ r1, #OS_THREAD_OFFSET_STATE ] // r0 = t->state
        cmpne   r0, #OS_THREAD_STATE_READY          
        ldrne   r1, [ r1, #OS_THREAD_OFFSET_NEXT ]
        bne     @11

        cmp     r1, #0
        bne     @12

_dont_switched_:
        mov     r3, #HW_PSR_IRQ_MODE|HW_PSR_IRQ_DISABLE|HW_PSR_ARM_STATE
        msr     cpsr_c, r3

        ldr     pc, [ sp ], #4          // return to irq master handler
        // not reach here


@12:
        // ---- OS_GetCurrentThread
        ldr     r0, [ r12, #OS_THREADINFO_OFFSET_CURRENT ]
        cmp     r1, r0
        beq     _dont_switched_         // return if no thread switching

        // call thread switch callback (need to save register r0, r1, r12)
        ldr     r3, [ r12, #OS_THREADINFO_OFFSET_SWITCHCALLBACK ]
        cmp     r3, #0
        beq     @13                     // skip calling callback when callback == 0
        stmfd   sp!, { r0, r1, r12 }
        mov     lr, pc
        bx      r3
        ldmfd   sp!, { r0, r1, r12 }

@13:
        // ---- OS_SetCurrentThread
        str     r1, [ r12, #OS_THREADINFO_OFFSET_CURRENT ]
        
        // ---- OS_SaveContext 
        // r0=currentThread  r1=nextThread
        // stack=Lo[LR,R0,R1,R2,R3,R12,LR]Hi
        mrs     r2, SPSR
        str     r2, [ r0, #OS_THREAD_OFFSET_CONTEXT ]!  // *r0=context:CPSR
        
#if defined(SDK_ARM9)
        // first, save CP context
        stmfd   sp!, { r0, r1 }
        add     r0, r0, #OS_THREAD_OFFSET_CONTEXT
        add     r0, r0, #OS_CONTEXT_CP_CONTEXT
        ldr     r1, =CP_SaveContext
        blx     r1
        ldmfd   sp!, { r0, r1 }
#endif

        ldmib   sp!, { r2,r3 }          // Get R0,R1    // *sp=stack:R1
        stmib   r0!, { r2,r3 }          // Put R0,R1    // *r0=context:R1
        
        ldmib   sp!, { r2,r3,r12,r14 }  // Get R2,R3,R12,LR // *sp=stack:LR
        stmib   r0, { r2-r14        }^  // Put R2-R14^  // *r0=context:R14
        add     r0, r0, #0x34           // 0x34 is size of r2-r14(13 resisters)
        stmib   r0!, { r14           }  // Put R14_irq  // *r0=context:R15+4
#ifdef  SDK_CONTEXT_HAS_SP_SVC
        mov     r3, #HW_PSR_SVC_MODE|HW_PSR_FIQ_DISABLE|HW_PSR_IRQ_DISABLE|HW_PSR_ARM_STATE
        msr     cpsr_c, r3
        stmib   r0!, { sp }
#endif

        // ---- OS_LoadContext
#if defined(SDK_ARM9)
        // first, load CP context
        stmfd   sp!, { r1 }
        add     r0, r1, #OS_THREAD_OFFSET_CONTEXT
        add     r0, r0, #OS_CONTEXT_CP_CONTEXT
        ldr     r1, =CPi_RestoreContext
        blx     r1
        
        ldmfd   sp!, { r1 }

#endif // if defined(SDK_ARM9)


#ifdef  SDK_CONTEXT_HAS_SP_SVC
        ldr     sp, [ r1, #OS_THREAD_OFFSET_CONTEXT+OS_CONTEXT_SP_SVC ]
        mov     r3, #HW_PSR_IRQ_MODE|HW_PSR_FIQ_DISABLE|HW_PSR_IRQ_DISABLE|HW_PSR_ARM_STATE
        msr     cpsr_c, r3
#endif

        ldr     r2, [ r1, #OS_THREAD_OFFSET_CONTEXT ]!  // *r1=context:CPSR
        msr     SPSR, r2                                // Put SPSR
        
        ldr     r14, [ r1, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_CPSR ]   // Get R15
        ldmib   r1, { r0-r14 }^         // Get R0-R14^  // *r1=over written
        nop
        stmda   sp!, { r0-r3,r12,r14 }  // Put R0-R3,R12,LR / *sp=stack:LR
        
        ldmfd   sp!, { pc }             // return to irq master handler
}

#ifdef      SDK_ARM9
#include    <nitro/itcm_end.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_WaitIrq

  Description:  wait specified interrupt
                the difference between OS_WaitIrq and OS_WaitInterrupt,
                in waiting interrupt
                OS_WaitIrq does switch thread,
                OS_WaitInterrupt doesn't switch thread.
                OS_WaitIrq wait by using OS_SleepThread() with threadQueue,
                OS_WaitInterrupt wait by using OS_Halt().
                if SDK_NO_THREAD defined, 2 functions become same.
  
  Arguments:    clear       TRUE if want to clear interrupt flag before wait
                            FALSE if not
                irqFlags    bit of interrupts to wait for

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_WaitIrq(BOOL clear, OSIrqMask irqFlags)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- clear interrupt check flags (if needed)
    if (clear)
    {
        (void)OS_ClearIrqCheckFlag(irqFlags);
    }

    (void)OS_RestoreInterrupts(enabled);

    //---- sleep till requied interrupts
    while (!(OS_GetIrqCheckFlag() & irqFlags))
    {
        OS_SleepThread(&OSi_IrqThreadQueue);
    }
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
void OS_WaitIrqEx(BOOL clear, OSIrqMask irqFlags)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    //---- clear interrupt check flags (if needed)
    if (clear)
    {
        (void)OS_ClearIrqCheckFlagEx(irqFlags);
    }

    (void)OS_RestoreInterrupts(enabled);

    //---- sleep till requied interrupts
    while (!(OS_GetIrqCheckFlagEx() & irqFlags))
    {
        OS_SleepThread(&OSi_IrqThreadQueue);
    }
}
#endif // defined(SDK_TWL) && defined(SDK_ARM7)

/*---------------------------------------------------------------------------*
  Name:         OS_WaitAnyIrq

  Description:  wait any interrupt
  
  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_WaitAnyIrq(void)
{
    OS_SleepThread(&OSi_IrqThreadQueue);
}
#if defined(SDK_TWL) && defined(SDK_ARM7)
inline void OS_WaitAnyIrqEx(void)
{
	OS_WaitAnyIrq();
}
#endif // defined(SDK_TWL) && defined(SDK_ARM7)
