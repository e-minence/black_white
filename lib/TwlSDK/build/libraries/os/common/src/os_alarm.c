/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_alarm.c

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
#include <nitro/os.h>

//---- timer control setting for alarm
#define OSi_ALARM_TIMERCONTROL      ( REG_OS_TM0CNT_H_E_MASK | REG_OS_TM0CNT_H_I_MASK | OS_TIMER_PRESCALER_64 )

//---- timer number alarm uses
#define OSi_ALARM_TIMER    OS_TIMER_1

//---- timer interrupt mask (must be same number with OSi_ALARM_TIMER)
#define OSi_ALARM_IE_TIMER OS_IE_TIMER1

//---- flag for initialization alarm
static u16 OSi_UseAlarm = FALSE;

//---- alarm queue
static struct OSiAlarmQueue OSi_AlarmQueue;


u16     OSi_IsTimerReserved(int timerNum);
void    OSi_SetTimerReserved(int timerNum);
void    OSi_UnsetTimerReserved(int timerNum);

static void OSi_SetTimer(OSAlarm *alarm);
static void OSi_InsertAlarm(OSAlarm *alarm, OSTick fire);

static void OSi_AlarmHandler(void *arg);
static void OSi_ArrangeTimer(void);


/*---------------------------------------------------------------------------*
  Name:         OSi_SetTimer

  Description:  set Timer

  Arguments:    alarm       pointer to alarm to set timer

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_SetTimer(OSAlarm *alarm)
{
    s64     delta;
    OSTick  tick = OS_GetTick();
    u16     timerCount;

    //---- let timer be disable
    OS_SetTimerControl(OSi_ALARM_TIMER, 0);

    delta = (s64)(alarm->fire - tick);

    //---- set interrupt callback
    OSi_EnterTimerCallback(OSi_ALARM_TIMER, OSi_AlarmHandler, NULL);


    if (delta < 0)
    {
        timerCount = (u16)~1;
    }
    else if (delta < 0x10000)
    {
        timerCount = (u16)(~delta);
    }
    else
    {
        timerCount = 0;
    }

//OS_TPrintf( "**OSi_SetTimer  alarm=%x, fire=%llx  time=%llx delta=%lld timeCount=%x \n", alarm, alarm->fire, time, delta, (int)timerCount );

    //---- set count and let timer be enable
    OS_SetTimerCount((OSTimer)OSi_ALARM_TIMER, timerCount);
    OS_SetTimerControl(OSi_ALARM_TIMER, (u16)OSi_ALARM_TIMERCONTROL);

    //---- TIMER IRQ Enable
    (void)OS_EnableIrqMask(OSi_ALARM_IE_TIMER);
}

/*---------------------------------------------------------------------------*
  Name:         OS_InitAlarm

  Description:  Initialize alarm system

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_InitAlarm(void)
{
    if (!OSi_UseAlarm)
    {
        OSi_UseAlarm = TRUE;

        //---- check if tick system is available
        SDK_TASSERTMSG(OS_IsTickAvailable(), "OS_InitAlarm: alarm system needs of tick system.");

        //---- OS reserves OSi_ALARM_TIMER
        SDK_ASSERT(!OSi_IsTimerReserved(OSi_ALARM_TIMER));
        OSi_SetTimerReserved(OSi_ALARM_TIMER);

        //---- clear alarm list
        OSi_AlarmQueue.head = NULL;
        OSi_AlarmQueue.tail = NULL;

        //---- TIMER IRQ Disable
        (void)OS_DisableIrqMask(OSi_ALARM_IE_TIMER);
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_EndAlarm

  Description:  end alarm system

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_EndAlarm(void)
{
    OSIntrMode enabled;

    SDK_ASSERT(OSi_UseAlarm);
    enabled = OS_DisableInterrupts();

    //---- check if any alarm exists
    if (OSi_UseAlarm)
    {
        SDK_TASSERTMSG(!OSi_AlarmQueue.head,
                      "OS_EndAlarm: Cannot end alarm system while using alarm.");

        //---- unset timer reservation by OS
        SDK_ASSERT(OSi_IsTimerReserved(OSi_ALARM_TIMER));
        OSi_UnsetTimerReserved(OSi_ALARM_TIMER);

        OSi_UseAlarm = FALSE;
    }

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         OS_IsAlarmAvailable

  Description:  check alarm system is available

  Arguments:    None

  Returns:      if available, TRUE.
 *---------------------------------------------------------------------------*/
BOOL OS_IsAlarmAvailable(void)
{
    return OSi_UseAlarm;
}


/*---------------------------------------------------------------------------*
  Name:         OS_CreateAlarm

  Description:  Create alarm

  Arguments:    alarm       pointer to alarm to be initialized

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CreateAlarm(OSAlarm *alarm)
{
    SDK_ASSERT(OSi_UseAlarm);
    SDK_ASSERT(alarm);

    alarm->handler = 0;
    alarm->tag = 0;
}


/*---------------------------------------------------------------------------*
  Name:         OSi_InsertAlarm

  Description:  Insert alarm. Needs to be called interrupts disabled.

  Arguments:    alarm       pointer to alarm to be set
                fire        tick to fire (only for one shot alarm)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_InsertAlarm(OSAlarm *alarm, OSTick fire)
{
    OSAlarm *prev;
    OSAlarm *next;

    //---- caluculate next fire for periodic alarm
    if (alarm->period > 0)
    {
        OSTick  tick = OS_GetTick();

        fire = alarm->start;
        if (alarm->start < tick)
        {
            fire += alarm->period * ((tick - alarm->start) / alarm->period + 1);
        }
    }

    //---- set tick to fire
    alarm->fire = fire;

    //---- insert to list
    for (next = OSi_AlarmQueue.head; next; next = next->next)
    {
//        if ( next->fire <= fire )
        if ((s64)(fire - next->fire) >= 0)
        {
            continue;
        }

        //---- insert alarm before 'next'
        alarm->prev = next->prev;
        next->prev = alarm;
        alarm->next = next;
        prev = alarm->prev;
        if (prev)
        {
            prev->next = alarm;
        }
        else
        {
            OSi_AlarmQueue.head = alarm;
            OSi_SetTimer(alarm);
        }

        return;
    }

    //---- insert alarm after tail
    alarm->next = 0;
    prev = OSi_AlarmQueue.tail;
    OSi_AlarmQueue.tail = alarm;
    alarm->prev = prev;
    if (prev)
    {
        prev->next = alarm;
    }
    else
    {
        OSi_AlarmQueue.head = OSi_AlarmQueue.tail = alarm;
        OSi_SetTimer(alarm);
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_SetAlarm

  Description:  Set alarm as a relative tick

  Arguments:    alarm       pointer to alarm to be set
                tick        ticks to count before firing
                handler     alarm handler to be called
                arg         argument of handler

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetAlarm(OSAlarm *alarm, OSTick tick, OSAlarmHandler handler, void *arg)
{
    OSIntrMode enabled;

    //OS_TPrintf( "**OS_SetAlarm e=%x alarm=%x tick=%x  handler%x\n", enabled, alarm, tick, handler );
    SDK_ASSERT(OSi_UseAlarm);
    SDK_TASSERTMSG(handler, "OS_SetAlarm: handler must not be NULL.");
    if (!alarm || alarm->handler)
    {
#ifndef SDK_FINALROM
        OS_TPanic("alarm could be already used.");
#else
        OS_TPanic("");
#endif
    }

    enabled = OS_DisableInterrupts();

    //---- clear periodic info
    alarm->period = 0;

    //---- set handler
    alarm->handler = handler;
    alarm->arg = arg;

    //---- insert alarm
    OSi_InsertAlarm(alarm, OS_GetTick() + tick);

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetPeriodicAlarm

  Description:  set periodic alarm 

  Arguments:    alarm          pointer to alarm to be set
                start          origin of the period in absolute tick
                period         ticks to count for each period
                handler        alarm handler to be called
                arg            argument of handler

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetPeriodicAlarm(OSAlarm *alarm, OSTick start, OSTick period, OSAlarmHandler handler,
                         void *arg)
{
    u32     enabled;

    //OS_TPrintf( "**SetPeriodicAlarm s=%llx p=%llx\n", start, period );
    SDK_ASSERT(OSi_UseAlarm);
    SDK_TASSERTMSG(handler, "OS_SetPeriodicAlarm: handler must not be NULL\n");
    SDK_TASSERTMSG(period > 0, "OS_SetPeriodicAlarm: bad period specified.");
    if (!alarm || alarm->handler)
    {
#ifndef SDK_FINALROM
        OS_TPanic("alarm could be already used.");
#else
        OS_TPanic("");
#endif
    }

    enabled = OS_DisableInterrupts();

    //---- set periodic info
    alarm->period = period;
    alarm->start = start;

    //---- set handler
    alarm->handler = handler;
    alarm->arg = arg;

    //---- insert periodic alarm
    OSi_InsertAlarm(alarm, 0);

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OS_CancelAlarm

  Description:  Cancel alarm

  Arguments:    alarm       pointer to alarm to be canceled

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CancelAlarm(OSAlarm *alarm)
{
    OSAlarm *next;
    u32     enabled;

    SDK_ASSERT(OSi_UseAlarm);
    SDK_ASSERT(alarm);

    enabled = OS_DisableInterrupts();

    if (alarm->handler == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);
        return;
    }

    //---- remove alarm 
    next = alarm->next;
    if (next == NULL)
    {
        OSi_AlarmQueue.tail = alarm->prev;
    }
    else
    {
        next->prev = alarm->prev;
    }

    if (alarm->prev)
    {
        alarm->prev->next = next;
    }
    else
    {
        OSi_AlarmQueue.head = next;
        if (next)
        {
            OSi_SetTimer(next);
        }
    }

    alarm->handler = NULL;
    alarm->period = 0;                 // not periodic alarm

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         OSi_AlarmHandler

  Description:  handler timer interrupt

  Arguments:    arg         dummy

  Returns:      None.
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm void OSi_AlarmHandler( void* arg )
{
    stmfd     sp!, {r0, lr} /* コールスタックを 8 バイト整合 */
    bl        OSi_ArrangeTimer
    ldmfd     sp!, {r0, lr} /* コールスタックを 8 バイト整合 */
    bx        lr
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         OSi_ArrangeTimer

  Description:  handler timer interrupt. called from OSi_AlarmHandler

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void OSi_ArrangeTimer(void)
{
    OSTick  tick;
    OSAlarm *alarm;
    OSAlarm *next;
    OSAlarmHandler handler;

    //---- let timer be disable
    OS_SetTimerControl(OSi_ALARM_TIMER, 0);

    //---- To be timer-irq Disable
    (void)OS_DisableIrqMask(OSi_ALARM_IE_TIMER);

    //---- set check flag timer interrupt
    OS_SetIrqCheckFlag(OSi_ALARM_IE_TIMER);


    tick = OS_GetTick();
    alarm = OSi_AlarmQueue.head;

    //OS_TPrintf( "**Arrange  alarm=%x  time=%llx  file=%llx\n", alarm, time, alarm->fire );

    //---- no alarm
    if (alarm == NULL)
    {
        return;
    }

    //---- not reach to time of top alarm
    if (tick < alarm->fire)
    {
        OSi_SetTimer(alarm);
        return;
    }

    //---- move next alarm to top
    next = alarm->next;
    OSi_AlarmQueue.head = next;

    if (next == NULL)
    {
        OSi_AlarmQueue.tail = NULL;
    }
    else
    {
        next->prev = NULL;
    }

    //---- call user alarm handler
    handler = alarm->handler;

    if (alarm->period == 0)
    {
        alarm->handler = NULL;
    }

    if (handler)
    {
        (handler) (alarm->arg);
    }

    //---- if alarm is periodic, re-inter to list
    if (alarm->period > 0)
    {
        alarm->handler = handler;
        OSi_InsertAlarm(alarm, 0);
    }

    //---- set timer
    if (OSi_AlarmQueue.head)
    {
        OSi_SetTimer(OSi_AlarmQueue.head);
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_SetAlarmTag

  Description:  set tag which is used OS_CancelAlarms

  Arguments:    alarm        alarm to be set tag
                tag          tagNo

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_SetAlarmTag(OSAlarm *alarm, u32 tag)
{
    SDK_ASSERT(OSi_UseAlarm);
    SDK_ASSERT(alarm);
    SDK_TASSERTMSG(tag > 0, "OS_SetAlarmTag: Tag must be >0.");

    alarm->tag = tag;
}


/*---------------------------------------------------------------------------*
  Name:         OS_CancelAlarms

  Description:  cancel alarms which have specified tag

  Arguments:    tag          tagNo. to be cancelled. not 0

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CancelAlarms(u32 tag)
{
    u32     enabled;
    OSAlarm *alarm;
    OSAlarm *next;

    SDK_ASSERT(OSi_UseAlarm);
    SDK_TASSERTMSG(tag > 0, "OSCancelAlarms: Tag must be >0.");

    if (tag == 0)
    {
        return;
    }

    enabled = OS_DisableInterrupts();

    for (alarm = OSi_AlarmQueue.head, next = alarm ? alarm->next : NULL;
         alarm; alarm = next, next = alarm ? alarm->next : NULL)
    {
        if (alarm->tag == tag)
        {
            //---- cancel alarm
            OS_CancelAlarm(alarm);
        }
    }

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         OS_CancelAllAlarms

  Description:  cancel all alarms

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_CancelAllAlarms(void)
{
    u32     enabled;
    OSAlarm *alarm;
    OSAlarm *next;

    SDK_ASSERT(OSi_UseAlarm);
    enabled = OS_DisableInterrupts();

    for (alarm = OSi_AlarmQueue.head, next = alarm ? alarm->next : NULL;
         alarm; alarm = next, next = alarm ? alarm->next : NULL)
    {
        //---- cancel alarm
        OS_CancelAlarm(alarm);
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         OSi_GetAlarmQueue

  Description:  get alarm queue

  Arguments:    None

  Returns:      alarm queue.
 *---------------------------------------------------------------------------*/
struct OSiAlarmQueue *OSi_GetAlarmQueue(void)
{
    return &OSi_AlarmQueue;
}

//================================================================================
//        FOR DEBUG
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetNumberOfAlarm

  Description:  get number of alarm

  Arguments:    None

  Returns:      number of alarm
 *---------------------------------------------------------------------------*/
int OS_GetNumberOfAlarm(void)
{
    OSIntrMode enabled = OS_DisableInterrupts();
	OSAlarm* p = OSi_AlarmQueue.head;
	int num = 0;

	while(p)
	{
		num ++;
		p = p->next;
	}

    (void)OS_RestoreInterrupts(enabled);
	return num;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetAlarmResource

  Description:  store resources of alarm to specified pointer

  Arguments:    resource       pointer to store alarm resources

  Returns:      TRUE  ... success (always return this now)
                FALSE ... fail
 *---------------------------------------------------------------------------*/
BOOL    OS_GetAlarmResource(OSAlarmResource *resource)
{
	resource->num = OS_GetNumberOfAlarm();

	return TRUE;
}

