/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_thread.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL
#include <twl/memorymap.h>
#else
#include <nitro/memorymap.h>
#endif

#include <nitro/os.h>
#include <nitro/mi.h>

//---------------------------------------------------------------------------

extern unsigned long SDK_SYS_STACKSIZE[];
extern unsigned long SDK_IRQ_STACKSIZE[];
#define OSi_SYS_STACKSIZE             ((s32)SDK_SYS_STACKSIZE)
#define OSi_IRQ_STACKSIZE             ((s32)SDK_IRQ_STACKSIZE)

//---- Stack CheckNumber
#ifdef SDK_ARM9
#  define  OSi_STACK_CHECKNUM_BOTTOM     0xfddb597dUL
#  define  OSi_STACK_CHECKNUM_TOP        0x7bf9dd5bUL
#  define  OSi_STACK_CHECKNUM_WARN       0x597dfbd9UL
#else
#  define  OSi_STACK_CHECKNUM_BOTTOM     0xd73bfdf7UL
#  define  OSi_STACK_CHECKNUM_TOP        0xfbdd37bbUL
#  define  OSi_STACK_CHECKNUM_WARN       0xbdf7db3dUL
#endif

//---- defs for launcher thread stack
#ifdef SDK_ARM9
extern void SDK_SECTION_ARENA_DTCM_START(void);
void    SDK_AUTOLOAD_DTCM_END(void);   // defined in LCF
#  define  OSi_LAUNCHER_STACK_LO_DEFAULT    SDK_SECTION_ARENA_DTCM_START
#  define  OSi_LAUNCHER_STACK_HI_MAX        (HW_DTCM_SVC_STACK - OSi_IRQ_STACKSIZE)
#  define  OSi_LAUNCHER_STACK_BOTTOM        (HW_DTCM_SVC_STACK - OSi_IRQ_STACKSIZE)
#else
#  define  OSi_LAUNCHER_STACK_LO_DEFAULT    HW_WRAM
#  define  OSi_LAUNCHER_STACK_HI_MAX        (HW_PRV_WRAM_IRQ_STACK_END - OSi_IRQ_STACKSIZE)
#  define  OSi_LAUNCHER_STACK_BOTTOM        (HW_PRV_WRAM_IRQ_STACK_END - OSi_IRQ_STACKSIZE)
#endif

//---- defs for idle thread
// for checkNumber and SVC stack (if defined)
#ifdef SDK_CONTEXT_HAS_SP_SVC
#define OSi_IDLE_CHECKNUM_SIZE  ( sizeof(u32)*2 + HW_SVC_STACK_SIZE )
#else
#define OSi_IDLE_CHECKNUM_SIZE  ( sizeof(u32)*2 )
#endif
// for SVC to stack out registers
#ifdef SDK_ARM9
#define OSi_IDLE_SVC_SIZE  ( sizeof(u32)*32 )   // arm9 svc stacks 14 words and makes in 8byte-alignment
#else
#define OSi_IDLE_SVC_SIZE  ( sizeof(u32)*16 )   // arm7 svc stacks 14 words
#endif
// stack size of idle thread
#define OSi_IDLE_THREAD_STACK_SIZE    ( OSi_IDLE_CHECKNUM_SIZE + OSi_IDLE_SVC_SIZE )

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
//---- threads
OSThread OSi_LauncherThread;
OSThread OSi_IdleThread;

//---- thread information
OSThreadInfo OSi_ThreadInfo;

//---- current thread pointer (for quick reference)
OSThread **OSi_CurrentThreadPtr;
#define       OSi_GetCurrentThread()    (*OSi_CurrentThreadPtr)

//---- thread initialization flag
BOOL    OSi_IsThreadInitialized = FALSE;

//---- idle thread stack
u32     OSi_IdleThreadStack[OSi_IDLE_THREAD_STACK_SIZE / sizeof(u32)];

//---- system callback in switch thread ( maybe for profile )
void   *OSi_SystemCallbackInSwitchThread = NULL;

//---- reschedule counter. if value>=0, do not reschedule.
u32     OSi_RescheduleCount = 0;
#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif

#ifdef SDK_THREAD_INFINITY
//---- thread id count
static int OSi_ThreadIdCount = 0;
#endif

void   *OSi_StackForDestructor = NULL;


#ifndef SDK_THREAD_INFINITY
static int OSi_SearchFreeEntry(void);
#endif
static void OSi_CancelThreadAlarmForSleep(OSThread *thread);
static void OSi_InsertThreadToList(OSThread *thread);
static void OSi_RemoveThreadFromList(OSThread *thread);
static void OSi_SleepAlarmCallback(void *arg);
static void OSi_IdleThreadProc(void *);
void    OSi_SetSystemCallbackInSwitchThread(void *callback);

static void OSi_ExitThread_ArgSpecified(OSThread *thread, void *arg);
static void OSi_ExitThread(void *arg);
static void OSi_ExitThread_Destroy(void);

#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_begin.h>
#endif
static void OSi_RescheduleThread(void);

#ifdef SDK_THREAD_INFINITY
/*---------------------------------------------------------------------------*
  Name:         OSi_GetUnusedThreadId

  Description:  get unused thread id

  Arguments:    None

  Returns:      thread id (0-0x7fffffff) which is never used.
 *---------------------------------------------------------------------------*/
static int OSi_GetUnusedThreadId(void)
{
    ++OSi_ThreadIdCount;
    SDK_ASSERT(OSi_ThreadIdCount > 0); // overflow check
    return OSi_ThreadIdCount;
}
#endif /* ; */

/*---------------------------------------------------------------------------*
  Name:         OSi_InsertLinkToQueue

  Description:  insert thread to thread queue

  Arguments:    queue  : thread queue
  				thread : thread to insert

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_InsertLinkToQueue(OSThreadQueue *queue, OSThread *thread)
{
    OSThread *next = queue->head;

    while (next && next->priority <= thread->priority)
    {
        if (next == thread)
        {
            return;
        }
        next = next->link.next;
    }

    if (!next)
    {
        OSThread *prev = queue->tail;

        if (!prev)
        {
            queue->head = thread;
        }
        else
        {
            prev->link.next = thread;
        }

        thread->link.prev = prev;
        thread->link.next = NULL;
        queue->tail = thread;
    }
    else
    {
        OSThread *prev = next->link.prev;

        if (!prev)
        {
            queue->head = thread;
        }
        else
        {
            prev->link.next = thread;
        }

        thread->link.prev = prev;
        thread->link.next = next;
        next->link.prev = thread;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_RemoveLinkFromQueue

  Description:  remove head thread from thread queue

  Arguments:    queue  : thread queue

  Returns:      thread pointer which is removed
 *---------------------------------------------------------------------------*/
static OSThread *OSi_RemoveLinkFromQueue(OSThreadQueue *queue)
{
    OSThread *t = queue->head;

    if (t)
    {
        OSThread *next = t->link.next;

        queue->head = next;

        if (next)
        {
            next->link.prev = NULL;
        }
        else
        {
            queue->tail = NULL;
            t->queue = NULL;
        }
    }

    return t;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_RemoveSpecifiedLinkFromQueue

  Description:  remove specified thread from thread queue

  Arguments:    queue  : thread queue

  Returns:      thread pointer which is removed
 *---------------------------------------------------------------------------*/
static OSThread *OSi_RemoveSpecifiedLinkFromQueue(OSThreadQueue *queue, OSThread *thread)
{
    OSThread *t = queue->head;
    OSThread *next;
    OSThread *prev;

    while (t)
    {
        next = t->link.next;

        if (t == thread)
        {
            prev = t->link.prev;

            //---- whether if head link
            if (queue->head == t)
            {
                queue->head = next;
            }
            else
            {
                prev->link.next = next;
            }

            //---- whether if tail link
            if (queue->tail == t)
            {
                queue->tail = prev;
            }
            else
            {
                next->link.prev = prev;
            }

            break;
        }

        t = next;
    }

    return t;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_RemoveMutexLinkFromQueue

  Description:  remove mutex from mutex queue

  Arguments:    queue  : mutex queue

  Returns:      mutex pointer which is removed
 *---------------------------------------------------------------------------*/
OSMutex *OSi_RemoveMutexLinkFromQueue(OSMutexQueue * queue)
{
    OSMutex *t = queue->head;

    if (t)
    {
        OSMutex *next = t->link.next;

        queue->head = next;

        if (next)
        {
            next->link.prev = NULL;
        }
        else
        {
            queue->tail = NULL;
        }
    }

    return t;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_SetSystemCallbackInSwitchThread

  Description:  set system callback in switching thread

  Arguments:    callback

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_SetSystemCallbackInSwitchThread(void *callback)
{
    OSi_SystemCallbackInSwitchThread = callback;
}

#ifndef SDK_THREAD_INFINITY
/*---------------------------------------------------------------------------*
  Name:         OSi_SearchFreeEntry

  Description:  search free thread entry area

  Arguments:    None

  Returns:      0 - OS_THREAD_MAX_NUM-1  ... entry index
                -1                       ... not found
 *---------------------------------------------------------------------------*/
static int OSi_SearchFreeEntry(void)
{
    int     i;

    for (i = 0; i < OS_THREAD_MAX_NUM; i++)
    {
        if (!OSi_ThreadInfo.entry[i])
        {
            return i;
        }
    }
    return -1;                         // not found
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OSi_InsertThreadToList

  Description:  insert thread to thread list which is arranged by priority

  Arguments:    pointer of thread

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_InsertThreadToList(OSThread *thread)
{
    OSThread *t = OSi_ThreadInfo.list;
    OSThread *pre = NULL;

    while (t && t->priority < thread->priority)
    {
        pre = t;
        t = t->next;
    }

    if (!pre)
    {
        thread->next = OSi_ThreadInfo.list;
        OSi_ThreadInfo.list = thread;
    }
    else
    {
        thread->next = pre->next;
        pre->next = thread;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_RemoveThreadFromList

  Description:  remove thread from thread list

  Arguments:    pointer of thread

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_RemoveThreadFromList(OSThread *thread)
{
    OSThread *t = OSi_ThreadInfo.list;
    OSThread *pre = NULL;

    while (t && t != thread)
    {
        pre = t;
        t = t->next;
    }

    SDK_TASSERTMSG(t, "Cannot remove thread from list.");

    if (!pre)
    {
        OSi_ThreadInfo.list = thread->next;
    }
    else
    {
        pre->next = thread->next;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_RescheduleThread

  Description:  Switch to the runnable thread highest priority
                without interrupts disabled.

  Arguments:    None

  Returns:      None or Never return
 *---------------------------------------------------------------------------*/
static void OSi_RescheduleThread(void)
{
    //---- if scheduler is set to be disabled, do nothing.
    if (OSi_RescheduleCount <= 0)
    {
        OSThreadInfo *info = &OSi_ThreadInfo;
        if (info->irqDepth > 0 || OS_GetProcMode() == OS_PROCMODE_IRQ)
        {
            // If in IRQ, do rescheduling at end of IRQ handler
            info->isNeedRescheduling = TRUE;
        }
        else
        {
            OSThread *currentThread, *nextThread;
            currentThread = OSi_GetCurrentThread();
            nextThread = OS_SelectThread();

            if (currentThread == nextThread || !nextThread)     // maybe nextThread != NULL
            {
                return;                // Don't have to switch the current context
            }

            if (currentThread->state != OS_THREAD_STATE_TERMINATED
                && OS_SaveContext(&currentThread->context))
            {
                return;                // Return if go back via OS_LoadContext
            }

            //---- call thread switch callback for system
            if (OSi_SystemCallbackInSwitchThread)
            {
                ((OSSwitchThreadCallback)OSi_SystemCallbackInSwitchThread) (currentThread,
                                                                            nextThread);
            }

            //---- call thread switch callback for user
            if (info->switchCallback)
            {
                ((OSSwitchThreadCallback)info->switchCallback) (currentThread, nextThread);
            }

            OS_SetCurrentThread(nextThread);

            OS_LoadContext(&nextThread->context);
            // Never reached
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_InitThread

  Description:  Initialize thread system

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_InitThread(void)
{
    void   *stackLo;
#ifndef SDK_THREAD_INFINITY
    int     i;
#endif

    if (OSi_IsThreadInitialized)
    {
        return;
    }
    OSi_IsThreadInitialized = TRUE;

#ifndef SDK_THREAD_INFINITY
    //---- Set thread info
    for (i = 0; i < OS_THREAD_MAX_NUM; i++)
    {
        OSi_ThreadInfo.entry[i] = NULL;
    }
#endif

    //---- set pointer to current thread buffer
    //     (for quick reference)
    OSi_CurrentThreadPtr = &(OSi_ThreadInfo.current);

    //---- Setup launcher thread
    OSi_LauncherThread.priority = OS_THREAD_LAUNCHER_PRIORITY;
    OSi_LauncherThread.id = 0;
    OSi_LauncherThread.state = OS_THREAD_STATE_READY;
    OSi_LauncherThread.next = NULL;

    //---- clear profile pointer
    OSi_LauncherThread.profiler = NULL;

    //---- clear thread entry and listPtr
#ifndef SDK_THREAD_INFINITY
    OSi_ThreadInfo.entry[0] = &OSi_LauncherThread;
#endif
    OSi_ThreadInfo.list = &OSi_LauncherThread;

    //---- let launch thread be current
    OS_SetCurrentThread(&OSi_LauncherThread);

    //---- StackLo
    stackLo = (OSi_SYS_STACKSIZE <= 0) ?
        (void *)((u32)OSi_LAUNCHER_STACK_LO_DEFAULT - OSi_SYS_STACKSIZE) :
        (void *)((u32)OSi_LAUNCHER_STACK_HI_MAX - OSi_SYS_STACKSIZE);
    SDK_ASSERT((u32)OSi_LAUNCHER_STACK_LO_DEFAULT <= (u32)stackLo
               && (u32)stackLo <= (u32)OSi_LAUNCHER_STACK_HI_MAX);

    //---- set Stack Bottom & Top
    OSi_LauncherThread.stackBottom = (u32)OSi_LAUNCHER_STACK_BOTTOM;
    OSi_LauncherThread.stackTop = (u32)stackLo;
    OSi_LauncherThread.stackWarningOffset = 0;

    //---- Set Stack CheckNumber
    *(u32 *)(OSi_LauncherThread.stackBottom - sizeof(u32)*2) = OSi_STACK_CHECKNUM_BOTTOM;
    *(u32 *)OSi_LauncherThread.stackTop = OSi_STACK_CHECKNUM_TOP;

    //---- clear join queue
    OS_InitThreadQueue(&OSi_LauncherThread.joinQueue);

#ifndef SDK_THREAD_INFINITY
    //---- max number of thread
    OSi_ThreadInfo.max_entry = OS_THREAD_MAX_NUM;
#endif

    //---- around IRQ
    OSi_ThreadInfo.isNeedRescheduling = FALSE;
    OSi_ThreadInfo.irqDepth = 0;
    SDK_TASSERTMSG(OSi_IRQ_STACKSIZE > 0, "IRQ STACKSIZE must be >0");

    //---- store thread info pointer
#ifdef SDK_ARM9
    OS_GetSystemWork()->threadinfo_mainp = &OSi_ThreadInfo;
#else
    OS_GetSystemWork()->threadinfo_subp = &OSi_ThreadInfo;
#endif

    //---- set thread switch callback
    (void)OS_SetSwitchThreadCallback(NULL);

    //---- idle thread
    OS_CreateThread(&OSi_IdleThread,
                    OSi_IdleThreadProc,
                    (void *)NULL,
                    OSi_IdleThreadStack + OSi_IDLE_THREAD_STACK_SIZE / sizeof(u32),
                    OSi_IDLE_THREAD_STACK_SIZE,
                    OS_THREAD_PRIORITY_MAX /*pseudo. change at next line. */ );
    OSi_IdleThread.priority = OS_THREAD_PRIORITY_MAX + 1;       // lower priority than the lowest (=OS_THREAD_PRIORITY_MAX)
    OSi_IdleThread.state = OS_THREAD_STATE_READY;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsThreadAvailable

  Description:  check if thread system is available

  Arguments:    None

  Returns:      TRUE if available, FALSE if not
 *---------------------------------------------------------------------------*/
asm BOOL OS_IsThreadAvailable( void )
{
    ldr      r0, =OSi_IsThreadInitialized
    ldr      r0, [r0, #0]
    bx       lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_CreateThread

  Description:  Creates a new thread

  Arguments:    thread      pointer of thread structure
                func        function to start thread
                arg         argument for func
                stack       stack bottom address
                stackSize   stack size (byte. must be aligned by 4)
                prio        thread priority

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_CreateThread(OSThread *thread,
                     void (*func) (void *), void *arg, void *stack, u32 stackSize, u32 prio)
{
#define STACK_ALIGN 4

    OSIntrMode enable;
    int     index;

    SDK_TASSERTMSG(OSi_GetCurrentThread(), "thread system were not initialized");
    SDK_TASSERTMSG(OS_THREAD_PRIORITY_MIN <= prio
                  && prio <= OS_THREAD_PRIORITY_MAX, "invalid priority");
    SDK_TASSERTMSG(stackSize % STACK_ALIGN == 0, "stack size must be aligned by %d", STACK_ALIGN);
    SDK_TASSERTMSG((u32)stack % STACK_ALIGN == 0, "stack must be aligned by %d", STACK_ALIGN);

    enable = OS_DisableInterrupts();

#ifndef SDK_THREAD_INFINITY
    //---- search free entry
    if ((index = OSi_SearchFreeEntry()) < 0)
    {
        SDK_TASSERTMSG(index >= 0, "OS_CreateThread: thread entry not allocated");
        (void)OS_RestoreInterrupts(enable);
        return;
    }
#else
    index = OSi_GetUnusedThreadId();
#endif

    //---- setup thread
    thread->priority = prio;
    thread->id = (u32)index;
    thread->state = OS_THREAD_STATE_WAITING;

    //---- clear profile pointer
    thread->profiler = NULL;

    //---- set thread entry and listPtr
#ifndef SDK_THREAD_INFINITY
    OSi_ThreadInfo.entry[index] = thread;
#endif
    OSi_InsertThreadToList(thread);

    //---- set Stack Bottom & Top
    thread->stackBottom = (u32)stack;
    thread->stackTop = (u32)stack - stackSize;
    thread->stackWarningOffset = 0;

    //---- Set Stack CheckNumber
    *(u32 *)(thread->stackBottom - sizeof(u32)*2) = OSi_STACK_CHECKNUM_BOTTOM; // minus for stack CheckNumber and padding
    *(u32 *)thread->stackTop = OSi_STACK_CHECKNUM_TOP;

    //---- clear join queue
    OS_InitThreadQueue(&thread->joinQueue);

    //---- Init context
    OS_InitContext(&thread->context, (u32)func, (u32)stack - sizeof(u32)*2); // minus for stack CheckNumber and padding

    thread->context.r[0] = (u32)arg;   // argument for func
    thread->context.lr = (u32)OS_ExitThread;

    //---- clear Stack (except check code (=sizeof(u32)*2) and padding(sizeof(u32))
    MI_CpuClear32((void *)((u32)stack - stackSize + sizeof(u32)), stackSize - sizeof(u32) * 2 - sizeof(u32) );

    //---- clear mutex
    thread->mutex = NULL;
#ifndef SDK_THREAD_INFINITY
    thread->mutexQueueHead = NULL;
    thread->mutexQueueTail = NULL;
#else
    thread->mutexQueue.head = NULL;
    thread->mutexQueue.tail = NULL;
#endif

    //---- clear destructor
#ifdef SDK_THREAD_INFINITY
    OS_SetThreadDestructor(thread, NULL);
#endif

    //---- clear queue
#ifdef SDK_THREAD_INFINITY
    thread->queue = NULL;
    thread->link.prev = thread->link.next = NULL;

    //---- clear specific member
    MI_CpuClear32(&thread->specific[0], sizeof(void *) * OS_THREAD_SPECIFIC_MAX);
#endif

    //---- clear alarm pointer for sleep
    thread->alarmForSleep = NULL;

    (void)OS_RestoreInterrupts(enable);
}


/*---------------------------------------------------------------------------*
  Name:         OS_ExitThread

  Description:  Terminates the current thread

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void OS_ExitThread(void)
{
    (void)OS_DisableInterrupts();

#ifdef  SDK_THREAD_INFINITY
    OSi_ExitThread_ArgSpecified(OS_GetCurrentThread(), 0);
#else
    OSi_ExitThread_Destroy();
#endif
}


#ifdef  SDK_THREAD_INFINITY
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// �f�X�g���N�^�X�^�b�N���l������ OSi_ExitThread
// �����݋֎~��ԂŌĂяo������
static void OSi_ExitThread_ArgSpecified(OSThread *thread, void *arg)
{
    if (OSi_StackForDestructor)
    {
        OS_InitContext(&thread->context, (u32)OSi_ExitThread, (u32)OSi_StackForDestructor);
        thread->context.r[0] = (u32)arg;
        thread->context.cpsr |= HW_PSR_IRQ_DISABLE;
        thread->state = OS_THREAD_STATE_READY;
        OS_LoadContext(&thread->context);
        // Never Returns
    }
    else
    {
        OSi_ExitThread(arg);
        // Never Returns
    }
}
#endif


#ifdef	SDK_THREAD_INFINITY
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// OS_KillThread �ɂ����ĎQ�Ƃ����R���e�L�X�g�X�C�b�`��
// ���荞�݋֎~��ԂŌĂяo������
static void OSi_ExitThread(void *arg)
{
    OSThread *currentThread = OSi_GetCurrentThread();
    OSThreadDestructor destructor;

    SDK_ASSERT(currentThread);

    // �f�X�g���N�^�̏���
    destructor = currentThread->destructor;
    if (destructor)
    {
        currentThread->destructor = NULL;
        destructor(arg);
        (void)OS_DisableInterrupts();  // �Ăъ��荞�݋֎~��
    }

    OSi_ExitThread_Destroy();          // Never return
}
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// OS_DestroyThread ����J�����g�X���b�h�� destroy ����Ƃ��ɌĂяo�����
// ���荞�݋֎~��ԂŌĂяo������
static void OSi_ExitThread_Destroy(void)
{
    OSThread *currentThread = OSi_GetCurrentThread();
    SDK_ASSERT(currentThread);

#ifdef SDK_THREAD_INFINITY
    (void)OS_DisableScheduler();
#endif

#ifndef SDK_THREAD_INFINITY
    //---- current thread check
    SDK_ASSERT(OSi_ThreadInfo.entry[currentThread->id] == currentThread);
#endif

    //---- Release all the locked mutexes by current thread
    OSi_UnlockAllMutex(currentThread);


    //---- remove from thread queue list
    if (currentThread->queue)
    {
        (void)OSi_RemoveSpecifiedLinkFromQueue(currentThread->queue, currentThread);
    }

    //---- remove from thread list
    OSi_RemoveThreadFromList(currentThread);

    //---- delete this thread
#ifndef SDK_THREAD_INFINITY
    OSi_ThreadInfo.entry[currentThread->id] = NULL;
#endif
    currentThread->state = OS_THREAD_STATE_TERMINATED;

    //---- wakeup thread waiting currentThread terminated
#ifndef SDK_THREAD_INFINITY
    OS_WakeupThread(&currentThread->joinQueue); // possible to never return
#else
    OS_WakeupThread(&currentThread->joinQueue);
#endif

#ifdef SDK_THREAD_INFINITY
    (void)OS_EnableScheduler();
#endif

    OS_RescheduleThread();             // never return

    OS_Terminate();
}

// TEST ---------------------------------------------------------------------
#ifdef NITRO_UTEST_H__
static vu32 exitThreadStatus = 0;

static void exitThreadFunc(void *arg)
{
    exitThreadStatus = 1;
    (void)arg;
}

void    UTEST_OS_ExitThread(void);
void UTEST_OS_ExitThread(void)
{
    OSThread thread;
    u32     stack[1024];

    OS_Init();
    OS_InitThread();

    OS_CreateThread(&thread,
                    exitThreadFunc,
                    NULL,
                    stack + 1024, sizeof(stack), OS_GetThreadPriority(OS_GetCurrentThread()) - 1);
    OS_WakeupThreadDirect(&thread);
    OS_JoinThread(&thread);
    UT_AssertEq(exitThreadStatus, 1);
}
#endif  // ifdef NITRO_UTEST_H__

/*---------------------------------------------------------------------------*
  Name:         OS_DestroyThread

  Description:  destroy specified thread.

  Arguments:    thread: thread to be destroyed

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_DestroyThread(OSThread *thread)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    SDK_ASSERT(thread);

    //---- check current thread
    if (OS_GetCurrentThread() == thread)
    {
        OSi_ExitThread_Destroy();
        // not reached
    }

#ifdef SDK_THREAD_INFINITY
    (void)OS_DisableScheduler();
#endif

    //---- Release all the locked mutexed by specified thread
    OSi_UnlockAllMutex(thread);

    //---- cancel alarm for sleep
    OSi_CancelThreadAlarmForSleep(thread);

    //---- remove from thread queue list
    if (thread->queue)
    {
        (void)OSi_RemoveSpecifiedLinkFromQueue(thread->queue, thread);
    }

    //---- remove from thread list
    OSi_RemoveThreadFromList(thread);

    //---- delete thread
#ifndef SDK_THREAD_INFINITY
    OSi_ThreadInfo.entry[thread->id] = NULL;
#endif
    thread->state = OS_THREAD_STATE_TERMINATED;

    //---- wakeup thread waiting this thread terminated
    OS_WakeupThread(&thread->joinQueue);

#ifdef SDK_THREAD_INFINITY
    (void)OS_EnableScheduler();
#endif
    (void)OS_RestoreInterrupts(enabled);

    OS_RescheduleThread();
}

/*---------------------------------------------------------------------------*
  Name:         OS_KillThread

  Description:  switch PC to thread destructor to finalize thread

  Arguments:    thread : thread to wait to finish
                flag   : argument for destructor

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef SDK_THREAD_INFINITY
// TESTEE
static inline void OSi_KillThreadWithPriority(OSThread *thread, void *arg, u32 prio);
static inline void OSi_KillThreadWithPriority(OSThread *thread, void *arg, u32 prio)
{
    SDK_ASSERT(thread);

    {
        u32     stack = (OSi_StackForDestructor) ? (u32)OSi_StackForDestructor : thread->stackBottom - sizeof(u32)*2; // minus for stack CheckNumber and padding

        OS_InitContext(&thread->context, (u32)OSi_ExitThread, stack);
    }
    thread->context.r[0] = (u32)arg;
    thread->context.cpsr |= HW_PSR_IRQ_DISABLE; // �f�X�g���N�^���ł� IRQ ���荞�݂��֎~����

    thread->state = OS_THREAD_STATE_READY;

    (void)OS_DisableScheduler();
    (void)OS_SetThreadPriority(thread, prio);
    (void)OS_EnableScheduler();
}

// BODY
void OS_KillThread(OSThread *thread, void *arg)
{
    OS_KillThreadWithPriority(thread, arg, OS_GetThreadPriority(thread));
}

void OS_KillThreadWithPriority(OSThread *thread, void *arg, u32 prio)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    if (thread == OS_GetCurrentThread())
    {
        OSi_ExitThread_ArgSpecified(thread, arg);
        // Never returns
    }

    //---- cancel alarm for sleep
    OSi_CancelThreadAlarmForSleep(thread);

    OSi_KillThreadWithPriority(thread, arg, prio);

    OSi_RescheduleThread();
    (void)OS_RestoreInterrupts(enabled);
}

// TEST ---------------------------------------------------------------------
#ifdef NITRO_UTEST_H__
// test1
void    UTEST_OS_KillThread_1(void);
void UTEST_OS_KillThread_1(void)
{
    OSThread thread;
    OSThread *t = &thread;
    u32     flag;

    OS_Init();
    OS_InitThread();

    t->stackBottom = 0x6789abcd;
    t->state = OS_THREAD_STATE_TERMINATED;
    OSi_KillThreadWithPriority(t, (void *)0x12345678, 16);

    // �R���e�N�X�g���̎�v���W�X�^�`�F�b�N
    UT_AssertEq(t->context.pc_plus4, (u32)OSi_ExitThread + 4);  // OS_ExitThread
    UT_AssertEq(t->context.r[0], (u32)0x12345678);      // arg
    flag = ((u32)OS_ExitThread & 1) ? (u32)HW_PSR_THUMB_STATE : (u32)HW_PSR_ARM_STATE;
    UT_AssertEq(t->context.cpsr, (u32)HW_PSR_IRQ_DISABLE | HW_PSR_SYS_MODE | flag);
    UT_AssertEq(t->context.sp, (u32)0x6789abcd - HW_SVC_STACK_SIZE);

    // �X�e�[�g�`�F�b�N
    UT_AssertEq(t->state, OS_THREAD_STATE_READY);
    UT_AssertAsserted(OSi_KillThreadWithPriority(0, 0, 16));
}

// test2
static vu32 killThreadStatus = 0;

static void killThreadDtor(void *arg)
{
    killThreadStatus = 666;
    (void)arg;
}

static void killThreadFunc(void *arg)
{
    OS_SetThreadDestructor(OS_GetCurrentThread(), killThreadDtor);

    killThreadStatus = 1;
    while (1)
    {
        OS_SleepThread(NULL);
        killThreadStatus++;
    }
    (void)arg;
}

void    UTEST_OS_KillThread_2(void);
void UTEST_OS_KillThread_2(void)
{
    OSThread thread;
    u32     stack[1024];

    OS_Init();
    OS_InitThread();

    OS_CreateThread(&thread,
                    killThreadFunc,
                    NULL,
                    stack + 1024, sizeof(stack), OS_GetThreadPriority(OS_GetCurrentThread()) - 1);
    UT_AssertEq(killThreadStatus, 0);

    OS_WakeupThreadDirect(&thread);
    UT_AssertEq(killThreadStatus, 1);
    UT_AssertEq(thread.destructor, killThreadDtor);

    OS_WakeupThreadDirect(&thread);
    UT_AssertEq(killThreadStatus, 2);

    OS_KillThread(&thread, 0);
    OS_JoinThread(&thread);
    UT_AssertEq(killThreadStatus, 666);
}
#endif  // ifdef NITRO_UTEST_H__
#endif  // ifdef SDK_THREAD_INFINITY

/*---------------------------------------------------------------------------*
  Name:         OSi_CancelThreadAlarmForSleep

  Description:  Cancel alarm used to sleep thread.
                If sleeping alarm is not set, do nothing.

  Arguments:    thread : thread to calcel alarm

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_CancelThreadAlarmForSleep(OSThread *thread)
{
    OSAlarm *alarm = thread->alarmForSleep;

    if (alarm)
    {
        OS_CancelAlarm(alarm);
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_JoinThread

  Description:  wait for specified thread to terminated

  Arguments:    thread : thread to wait to finish

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_JoinThread(OSThread *thread)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    SDK_ASSERT(thread);

    //---- skip if thread is terminated already
    while(thread->state != OS_THREAD_STATE_TERMINATED)
    {
        OS_SleepThread(&thread->joinQueue);
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsThreadTerminated

  Description:  check thread status whether it's terminated

  Arguments:    thread : pointer to thread to be examined

  Returns:      TRUE if the thread is terminated. FALSE if not
 *---------------------------------------------------------------------------*/
BOOL OS_IsThreadTerminated(const OSThread *thread)
{
	SDK_ASSERT(thread);
    return (thread->state == OS_THREAD_STATE_TERMINATED) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetThreadStatus

  Description:  get thread status

  Arguments:    thread : pointer to thread

  Returns:      
 *---------------------------------------------------------------------------*/
OSThreadState  OS_GetThreadStatus(const OSThread *thread)
{
	SDK_ASSERT(thread);
	return thread->state;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SleepThreadDirect

  Description:  Gets the thread into sleep status directly

  Arguments:    thread            thread to sleep
                queue             waiting list queue (or NULL)

  Returns:      none
 *---------------------------------------------------------------------------*/
void OS_SleepThreadDirect(OSThread *thread, OSThreadQueue *queue)
{
    SDK_ASSERT(thread);
    SDK_ASSERT(thread->state != OS_THREAD_STATE_TERMINATED);

	{
        OSIntrMode bak_intr = OS_DisableInterrupts();

		if ( thread->state == OS_THREAD_STATE_READY )
		{
			if (queue)
			{
#ifndef SDK_THREAD_INFINITY
				*queue |= (OSThreadQueue)(1UL << thread->id);
#else
				thread->queue = queue;
				OSi_InsertLinkToQueue(queue, thread);
#endif
			}
			thread->state = OS_THREAD_STATE_WAITING;
			OSi_RescheduleThread();
		}
        (void)OS_RestoreInterrupts(bak_intr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_SleepThread

  Description:  Gets the current thread into sleep status

  Arguments:    waiting list queue

  Returns:      none
 *---------------------------------------------------------------------------*/
void OS_SleepThread(OSThreadQueue *queue)
{
    OSIntrMode enable;
    OSThread *currentThread;

    enable = OS_DisableInterrupts();
#ifndef SDK_THREAD_INFINITY
    {
        currentThread = OSi_GetCurrentThread();
        SDK_ASSERT(currentThread);

        if (queue)
        {
            *queue |= (OSThreadQueue)(1UL << currentThread->id);
        }

        currentThread->state = OS_THREAD_STATE_WAITING;
        OSi_RescheduleThread();
    }
#else
    {
        currentThread = OSi_GetCurrentThread();
        SDK_ASSERT(currentThread);

        if (queue)
        {
            currentThread->queue = queue;
            OSi_InsertLinkToQueue(queue, currentThread);
        }

        currentThread->state = OS_THREAD_STATE_WAITING;
        OSi_RescheduleThread();
    }
#endif
    (void)OS_RestoreInterrupts(enable);
}

/*---------------------------------------------------------------------------*
  Name:         OS_WakeupThread

  Description:  Gets the threads out of sleep status

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void OS_WakeupThread(OSThreadQueue *queue)
{
    OSIntrMode enable;
#ifndef SDK_THREAD_INFINITY
    u32     mask;
#else
    BOOL    isNeedRescheduling = FALSE;
#endif
    SDK_ASSERT(queue);
    enable = OS_DisableInterrupts();

#ifndef SDK_THREAD_INFINITY
    mask = (u32)*queue;
    if (mask)
    {
        //---- wakeup threads
        OSThread *t = OSi_ThreadInfo.list;
        while (t)
        {
            if (mask & (1UL << t->id))
            {
                t->state = OS_THREAD_STATE_READY;
            }

            t = t->next;
        }

        OS_InitThreadQueue(queue);
        OSi_RescheduleThread();
    }
#else
    if (queue->head)
    {
        while (queue->head)
        {
            OSThread *thread = OSi_RemoveLinkFromQueue(queue);

            thread->state = OS_THREAD_STATE_READY;
            thread->queue = NULL;
            thread->link.prev = thread->link.next = NULL;
        }

        OS_InitThreadQueue(queue);
        OSi_RescheduleThread();
    }
#endif

    (void)OS_RestoreInterrupts(enable);
}


/*---------------------------------------------------------------------------*
  Name:         OS_WakeupThreadDirect

  Description:  Gets the threads out of sleep status directly

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void OS_WakeupThreadDirect(OSThread *thread)
{
    OSIntrMode enable;

    SDK_ASSERT(thread);
    SDK_ASSERT(thread->state != OS_THREAD_STATE_TERMINATED);
    enable = OS_DisableInterrupts();
    {
        thread->state = OS_THREAD_STATE_READY;
        OSi_RescheduleThread();

    }
    (void)OS_RestoreInterrupts(enable);
}


/*---------------------------------------------------------------------------*
  Name:         OS_SelectThread

  Description:  Select the runnable thread highest priority

  Arguments:    None

  Returns:      Pointer to thread must run
                NULL if no thread candidate to run
 *---------------------------------------------------------------------------*/
OSThread *OS_SelectThread(void)
{
    OSThread *t = OSi_ThreadInfo.list;

    while (t && !OS_IsThreadRunnable(t))
    {
        t = t->next;
    }

    return t;
}

/*---------------------------------------------------------------------------*
  Name:         OS_RescheduleThread

  Description:  Switch to the runnable thread highest priority

  Arguments:    None

  Returns:      None or Never return
 *---------------------------------------------------------------------------*/
void OS_RescheduleThread(void)
{
    OSIntrMode bak_intr = OS_DisableInterrupts();
    OSi_RescheduleThread();
    (void)OS_RestoreInterrupts(bak_intr);
}

/*---------------------------------------------------------------------------*
  Name:         OS_YieldThread

  Description:  do thread rescheduling. current thread relinquish CPU
                to give chance of running to other threads which has same
                priority.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_YieldThread(void)
{
    OSThread *current = OS_GetCurrentThread();
    OSThread *pre = NULL;
    OSThread *lastThread = NULL;
    int     samePriorityThread = 0;
    OSIntrMode enable = OS_DisableInterrupts();

    {
        OSThread *t = OSi_ThreadInfo.list;
        OSThread *tPre = NULL;

        while (t)
        {
            if (t == current)
            {
                pre = tPre;
            }

            if (current->priority == t->priority)
            {
                lastThread = t;
                samePriorityThread++;
            }

            tPre = t;
            t = t->next;
        }
    }

    //---- no thread of same priority with current  or  needless to arrange list
    if (samePriorityThread <= 1 || lastThread == current)
    {
        (void)OS_RestoreInterrupts(enable);
        return;
    }

    //---- remove thread from list
    if (!pre)
    {
        OSi_ThreadInfo.list = current->next;
    }
    else
    {
        pre->next = current->next;
    }

    //---- insert thread after 'lastThread'
    current->next = lastThread->next;
    lastThread->next = current;

    //---- re-schedule
    OSi_RescheduleThread();

    (void)OS_RestoreInterrupts(enable);
}


/*---------------------------------------------------------------------------*
  Name:         OS_DumpThreadList

  Description:  Dump thread list

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_DumpThreadList(void)
{
#ifndef SDK_FINALROM
#ifndef SDK_THREAD_INFINITY
    int     i;
#endif

    OS_TPrintf("thread list top %08x\n", OSi_ThreadInfo.list);

#ifndef SDK_THREAD_INFINITY
    OS_TPrintf("No:  address  prio     next\n");
    for (i = 0; i < OS_THREAD_MAX_NUM; i++)
    {
        OSThread *thread = OSi_ThreadInfo.entry[i];
        OS_TPrintf("%02d: %08x %5d %08x\n", i, thread, (thread) ? thread->priority : 0,
                  (thread) ? thread->next : 0);
    }
#else
    //    OS_TPrintf("Id:  address  prio     next\n");
    OS_TPrintf("Id:  address  prio     next st  queue.h  queue.t   link.p   link.n\n");
    {
        OSThread *thread = OSi_ThreadInfo.list;
        while (thread)
        {
            //                  OS_TPrintf("%02d: %08x %5d %08x\n", thread->id, thread, thread->priority, thread->next );
            OS_TPrintf("%02d: %08x %5d %08x  %d %8x %8x %8x %8x\n", thread->id, thread,
                      thread->priority, thread->next, thread->state,
                      (thread->queue) ? thread->queue->head : (OSThread *)1,
                      (thread->queue) ? thread->queue->tail : (OSThread *)1, thread->link.prev,
                      thread->link.next);
            thread = thread->next;

        }
    }
#endif
#endif
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetNumberOfThread

  Description:  get number of thread whch exists in system

  Arguments:    None

  Returns:      number of thread which exists in system
 *---------------------------------------------------------------------------*/
int OS_GetNumberOfThread(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    int     threads = 0;

#ifndef SDK_THREAD_INFINITY
    int     i;

    for (i = 0; i < OS_THREAD_MAX_NUM; i++)
    {
        if (OSi_ThreadInfo.entry[i])
        {
            threads++;
        }
    }
#else
    OSThread *thread = OSi_ThreadInfo.list;
    while (thread)
    {
        threads++;
        thread = thread->next;
    }
#endif

    (void)OS_RestoreInterrupts(enabled);
    return threads;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetStackStatus

  Description:  check thread stack. check each CheckNUM.
                return result.

  Arguments:    thread   thread checked

  Returns:      0 (OS_STACK_NO_ERROR)        no error
                OS_STACK_OVERFLOW            overflow
                OS_STACK_ABOUT_TO_OVERFLOW   about to overflow
                OS_STACK_UNDERFLOW           underflow
 *---------------------------------------------------------------------------*/
OSStackStatus OS_GetStackStatus(const OSThread *thread)
{
    //---- Check if overflow
    if (*(u32 *)(thread->stackTop) != OSi_STACK_CHECKNUM_TOP)
    {
        return OS_STACK_OVERFLOW;
    }
    //---- Check if about to overflow
    else if (thread->stackWarningOffset
             && *(u32 *)(thread->stackTop + thread->stackWarningOffset) != OSi_STACK_CHECKNUM_WARN)
    {
        return OS_STACK_ABOUT_TO_OVERFLOW;
    }
    //---- Check if underFlow
    else if (*(u32 *)(thread->stackBottom - sizeof(u32)*2) != OSi_STACK_CHECKNUM_BOTTOM)
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
  Name:         OSi_CheckStack

  Description:  check thread stack. check each CheckNUM.
                if changed, display warning and halt.

  Arguments:    file     file name displayed when stack overflow
                line     line number displayed when stack overflow
                thread   thread checked

  Returns:      None
                ( if error occurred, never return )
 *---------------------------------------------------------------------------*/
static char *OSi_CheckStack_mesg[] = {
    "overflow", "about to overflow", "underflow"
};

#ifndef SDK_FINALROM
#ifndef SDK_NO_MESSAGE
void OSi_CheckStack(const char *file, int line, const OSThread *thread)
{
    OSStackStatus st;

    if ((st = OS_GetStackStatus(thread)) == OS_STACK_NO_ERROR)
    {
        return;
    }

    OSi_Panic(file, line, " stack %x(id:%d) %s.\nstack area: %08x-%08x, warning offset: %x",
              thread,
              thread->id,
              OSi_CheckStack_mesg[(int)st - 1],
              thread->stackTop, thread->stackBottom, thread->stackWarningOffset);
    // Never return
}
#endif
#endif

/*---------------------------------------------------------------------------*
  Name:         OSi_GetSystemStackPointer

  Description:  Get system mode stack pointer at svc/irq mode

  Arguments:    None

  Returns:      Stack Pointer
 *---------------------------------------------------------------------------*/
static u32 OSi_SystemStackBuffer;

#include  <nitro/code32.h>
asm u32 OSi_GetSystemStackPointer( void )
{
    ldr   r0, =OSi_SystemStackBuffer
    stmia r0, { sp }^
    ldr   r0, [ r0 ]
    bx    lr
}
#include  <nitro/codereset.h>

/*---------------------------------------------------------------------------*
  Name:         OSi_GetCurrentStackPointer

  Description:  Get current mode stack pointer

  Arguments:    None

  Returns:      Stack Pointer
 *---------------------------------------------------------------------------*/
asm u32 OSi_GetCurrentStackPointer( void )
{
    mov   r0, sp
    bx    lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetThreadStackWarningOffset

  Description:  Set warning level for stack checker

  Arguments:    thread     thread to set
                offset     offset from stack top. must be multiple of 4

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetThreadStackWarningOffset(OSThread *thread, u32 offset)
{
    SDK_TASSERTMSG((offset & 3) == 0, "Offset must be aligned by 4");
    SDK_TASSERTMSG(OS_GetThreadContext(thread)->sp > thread->stackTop + offset,
                  "Cannot set warning level below current sp.");

    //---- remember warning offset
    thread->stackWarningOffset = offset;

    //---- set Stack CheckNum
    if (offset != 0)
    {
        *(u32 *)(thread->stackTop + offset) = OSi_STACK_CHECKNUM_WARN;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetThreadPriority

  Description:  change priority of thread

  Arguments:    thread     thread to set priority
                prio       new priority to be set

  Returns:      TRUE if success
 *---------------------------------------------------------------------------*/
BOOL OS_SetThreadPriority(OSThread *thread, u32 prio)
{
    OSThread *t = OSi_ThreadInfo.list;
    OSThread *pre = NULL;
    OSIntrMode enable;

    SDK_TASSERTMSG(OS_THREAD_PRIORITY_MIN <= prio
                  && prio <= OS_THREAD_PRIORITY_MAX, "invalid priority");
    SDK_TASSERTMSG(thread != &OSi_IdleThread, "cannot change idle thread priority.");

    enable = OS_DisableInterrupts();

    while (t && t != thread)
    {
        pre = t;
        t = t->next;
    }

    //---- thread not found or thread is idle
    if (!t || t == &OSi_IdleThread)
    {
        (void)OS_RestoreInterrupts(enable);
        return FALSE;
    }

    if (t->priority != prio)
    {
        //---- remove thread from list
        if (!pre)
        {
            OSi_ThreadInfo.list = thread->next;
        }
        else
        {
            pre->next = thread->next;
        }

        //---- set priority and insert proper position
        thread->priority = prio;
        OSi_InsertThreadToList(thread);

        //---- re-schedule
        OSi_RescheduleThread();
    }

    (void)OS_RestoreInterrupts(enable);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetThreadPriority

  Description:  get priority of thread

  Arguments:    thread     thread to get priority

  Returns:      priority
 *---------------------------------------------------------------------------*/
u32 OS_GetThreadPriority(const OSThread *thread)
{
    SDK_TASSERTMSG(thread, "OS_GetThreadPriority: bad thread");

    return thread->priority;
}


/*---------------------------------------------------------------------------*
  Name:         OS_Sleep

  Description:  sleep specified period

  Arguments:    msec      sleeping period. ( milliseconds )

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_Sleep(u32 msec)
{
    OSAlarm alarm;

    SDK_TASSERTMSG(OS_IsTickAvailable()
                  && OS_IsAlarmAvailable(), "OS_Sleep: need to start Tick and Alarm beforehand.");
    SDK_TASSERTMSG(OSi_IsThreadInitialized, "OS_Sleep: thread system not initialized.");

    OS_CreateAlarm(&alarm);
    {
        OSThread *volatile p_thread = OSi_GetCurrentThread();
        OSIntrMode bak_cpsr = OS_DisableInterrupts();

        // ---- remember alarm
        p_thread->alarmForSleep = &alarm;

        OS_SetAlarm(&alarm, OS_MilliSecondsToTicks(msec), &OSi_SleepAlarmCallback,
                    (void *)&p_thread);
        while (p_thread != NULL)
        {
            OS_SleepThread(NULL);
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

//---------------- callback to wakeup sleeping thread
static void OSi_SleepAlarmCallback(void *arg)
{
    OSThread **pp_thread = (OSThread **)arg;
    OSThread *p_thread = *pp_thread;
    *pp_thread = NULL;

    //---- clear remembrance of alarm
    p_thread->alarmForSleep = NULL;

    OS_WakeupThreadDirect(p_thread);
}


/*---------------------------------------------------------------------------*
  Name:         OS_SetSwitchThreadCallback

  Description:  set callback called at switching thread

  Arguments:    callback      callback function

  Returns:      previous callback function before set callback now
 *---------------------------------------------------------------------------*/
OSSwitchThreadCallback OS_SetSwitchThreadCallback(OSSwitchThreadCallback callback)
{
    OSSwitchThreadCallback prev;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    prev = OSi_ThreadInfo.switchCallback;
    OSi_ThreadInfo.switchCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
    return prev;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_IdleThreadProc

  Description:  procedure of idle thread which system creates

  Arguments:    None

  Returns:      None (never return)
 *---------------------------------------------------------------------------*/
static void OSi_IdleThreadProc(void *)
{
    (void)OS_EnableInterrupts();
    while (1)
    {
        OS_Halt();
    }
    // never return
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetIdleThread

  Description:  get pointer to idle thread structure

  Arguments:    None

  Returns:      pointer to idle thread structure
 *---------------------------------------------------------------------------*/
OSThread *OSi_GetIdleThread(void)
{
    OSThread *t = NULL;
    if (OSi_IsThreadInitialized)
    {
        t = &OSi_IdleThread;
    }
    return t;
}

/*---------------------------------------------------------------------------*
  Name:         OS_DisableScheduler

  Description:  disable scheduler

  Arguments:    None

  Returns:      Previous scheduler suspend count.
                Suspended if value >= 0.
 *---------------------------------------------------------------------------*/
u32 OS_DisableScheduler(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    u32     count;

    if (OSi_RescheduleCount < (u32)(0 - 1) /* u32 max value -1 */ )
    {
        count = OSi_RescheduleCount++;
    }
    (void)OS_RestoreInterrupts(enabled);

    return count;
}

/*---------------------------------------------------------------------------*
  Name:         OS_EnableScheduler

  Description:  enable scheduler

  Arguments:    None

  Returns:      Previous scheduler suspend count.
                Suspended if value >= 0.
 *---------------------------------------------------------------------------*/
u32 OS_EnableScheduler(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
    u32     count = 0;

    if (OSi_RescheduleCount > 0)
    {
        count = OSi_RescheduleCount--;
    }
    (void)OS_RestoreInterrupts(enabled);

    return count;
}

#ifdef SDK_THREAD_INFINITY
/*---------------------------------------------------------------------------*
  Name:         OS_GetThread

  Description:  Gets pointer to thread which id is specified

  Arguments:    id : thread id to get thread

  Returns:      pointer to thread which id is specified
 *---------------------------------------------------------------------------*/
OSThread *OS_GetThread(u32 id)
{
    OSThread *retval = NULL;
    OSThread *t = OSi_ThreadInfo.list;

    while (t)
    {
        if (t->id == id)
        {
            retval = t;
            break;
        }

        t = t->next;
    }

    return retval;
}
#endif


#ifdef SDK_THREAD_INFINITY
/*---------------------------------------------------------------------------*
  Name:         OS_SetThreadDestructor

  Description:  set thread destructor, which is called when that thread exits.

  Arguments:    thread : thread pointer
  				dtor   : destructor function

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetThreadDestructor(OSThread *thread, OSThreadDestructor dtor)
{
    SDK_ASSERT(thread);
    thread->destructor = dtor;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetThreadDestructor

  Description:  get thread destructor which is set

  Arguments:    thread : thread pointer

  Returns:      destructor function
 *---------------------------------------------------------------------------*/
OSThreadDestructor OS_GetThreadDestructor(const OSThread *thread)
{
    SDK_ASSERT(thread);
    return thread->destructor;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetThreadParameter

  Description:  set user parameter which is allowed to use freely.

  Arguments:    thread : thread pointer
				parameter : user parameter

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetThreadParameter(OSThread *thread, void *parameter)
{
    SDK_ASSERT(thread);
    thread->userParameter = parameter;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetThreadParameter

  Description:  get user parameter which is set

  Arguments:    thread : thread pointer

  Returns:      user parameter which is set
 *---------------------------------------------------------------------------*/
void   *OS_GetThreadParameter(const OSThread *thread)
{
    SDK_ASSERT(thread);
    return thread->userParameter;
}



/*---------------------------------------------------------------------------*
  Name:         OSi_SetSystemErrno

  Description:  set system error number.

  Arguments:    thread : thread to set error number
  				errno  : error number to set

  Returns:      None
 *---------------------------------------------------------------------------*/
void OSi_SetSystemErrno(OSThread *thread, int errno)
{
    SDK_ASSERT(thread);
    thread->systemErrno = errno;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetSystemErrno

  Description:  get system error number.

  Arguments:    thread : thread to set error number

  Returns:      error number
 *---------------------------------------------------------------------------*/
int OSi_GetSystemErrno(const OSThread *thread)
{
    SDK_ASSERT(thread);
    return thread->systemErrno;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetErrno

  Description:  get system error number.

  Arguments:    None.

  Returns:      error number
 *---------------------------------------------------------------------------*/
int OS_GetErrno(void)
{
    OSThread *thread = OSi_GetCurrentThread();
    return OSi_GetSystemErrno(thread);
}
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_IsThreadInList

  Description:  check if the specified thread is in the thread list

  Arguments:    thread : thread

  Returns:      TRUE if thread is in the thread list
 *---------------------------------------------------------------------------*/
BOOL OS_IsThreadInList(const OSThread *thread)
{
    BOOL    r = FALSE;
    OSThread *t = OSi_ThreadInfo.list;
    OSIntrMode enabled = OS_DisableInterrupts();

    while (t)
    {
        if (t == thread)
        {
            r = TRUE;
            break;
        }

        t = t->next;
    }

    (void)OS_RestoreInterrupts(enabled);
    return r;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetThreadDestructorStack

  Description:  specify stack area to call thread destructor

  Arguments:    stack       stack bottom address
                stackSize   stack size (byte. must be aligned by 4)

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetThreadDestructorStack(void *stack)
{
    SDK_ASSERT(stack);
    SDK_ASSERT((u32)stack % STACK_ALIGN == 0);

    OSi_StackForDestructor = stack;
}

//================================================================================
//    for DEBUG
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetThreadResource

  Description:  store resources of thread to specified pointer

  Arguments:    resource       pointer to store thread resources

  Returns:      TRUE  ... success (always return this now)
                FALSE ... fail
 *---------------------------------------------------------------------------*/
BOOL OS_GetThreadResource(OSThreadResource *resource)
{
	resource->num = OS_GetNumberOfThread();

	return TRUE;
}


#if defined(SDK_TCM_APPLY) && defined(SDK_ARM9)
#include    <nitro/itcm_end.h>
#endif
