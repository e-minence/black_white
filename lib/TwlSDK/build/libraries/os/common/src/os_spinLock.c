/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS -
  File:     os_spinLock.c

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
#include <nitro.h>


void    _ISDbgLib_Initialize(void);
void    _ISDbgLib_AllocateEmualtor(void);
void    _ISDbgLib_FreeEmulator(void);
void    _ISTDbgLib_Initialize(void);
void    _ISTDbgLib_AllocateEmualtor(void);
void    _ISTDbgLib_FreeEmulator(void);

s32     OS_LockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_UnlockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void));
s32     OS_TryLockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*crtlFuncp) (void));

static s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                            BOOL disableFiq);
static s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                              BOOL disableFIQ);
static s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                               BOOL disableFIQ);

static void OSi_AllocateCartridgeBus(void);
static void OSi_FreeCartridgeBus(void);

static void OSi_AllocateCardBus(void);
static void OSi_FreeCardBus(void);

static void OSi_WaitByLoop(void);


#ifdef  SDK_ARM9
#define OSi_ASSERT_ID( id )       SDK_ASSERTMSG( id >= OS_MAINP_LOCK_ID_START && id <= OS_MAINP_SYSTEM_LOCK_ID, \
                             "lock ID %d is out of bounds", id )
#else
#define OSi_ASSERT_ID( id )       SDK_ASSERTMSG( id >= OS_SUBP_LOCK_ID_START && id <= OS_SUBP_SYSTEM_LOCK_ID, \
                             "lock ID %d is out of bounds", id )
#endif


#define OSi_LOCKID_INITIAL_FLAG_0     0xffffffff
#define OSi_LOCKID_INITIAL_FLAG_1     0xffff0000


#ifdef SDK_ARM9
#define OSi_ANYP_LOCK_ID_FLAG  HW_LOCK_ID_FLAG_MAIN
#define OSi_ANYP_LOCK_ID_START OS_MAINP_LOCK_ID_START
#else
#define OSi_ANYP_LOCK_ID_FLAG  HW_LOCK_ID_FLAG_SUB
#define OSi_ANYP_LOCK_ID_START OS_SUBP_LOCK_ID_START
#endif

//======================================================================
//                      SYNC
//======================================================================
#ifdef SDK_TWL
/*---------------------------------------------------------------------------*
  Name:         OSi_SyncWithOtherProc

  Description:  sync with other processor.

  Arguments:    type    : OSi_SYNCTYPE_SENDER/RECVER
                syncBuf : work area. use 4byte.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_SyncWithOtherProc( int type, void* syncBuf )
{
    vu8* ptr1     = (vu8*)syncBuf;
    vu8* ptr2     = (vu8*)syncBuf +1;
    vu8* pfinish  = (vu8*)syncBuf +2;
    vu8* pconf    = (vu8*)syncBuf +3;

    if ( type == OSi_SYNCTYPE_SENDER )
    {
        int n=0;
        *pfinish = FALSE;
        do
        {
            *ptr1 = (u8)( 0x80 | (n&0xf) );
            while ( *ptr1 != *ptr2 && *pfinish == FALSE )
            {
                OSi_WaitByLoop();
            }
            n ++;
        } while( *pfinish == FALSE );
        *pconf = TRUE;
    }
    else
    {
        int sum = 0;
        *ptr2 = 0;
        while( sum < 0x300 )
        {
            if ( *ptr2 != *ptr1 )
            {
                *ptr2 = *ptr1;
                sum += *ptr2;
            }
            else
            {
                OSi_WaitByLoop();
            }
        }
        *pconf   = FALSE;
        *pfinish = TRUE;
        while( *pconf == FALSE )
        {
            OSi_WaitByLoop();
        }
    }
}
#endif

//======================================================================
//                      DUMMY LOOP
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_WaitByLoop

  Description:  wait by for() loop

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void OSi_WaitByLoop(void)
{
    SVC_WaitByLoop(0x1000 / 4);
}

//======================================================================
//                      INITIALIZE
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_InitLock

  Description:  �V�X�e���̃��b�N�ϐ��⋤�L���\�[�X�ւ̃A�N�Z�X��
                �̐ݒ�����������܂��B

                �J�[�g���b�W�̔r������̂��߂̗̈�̓N���A����܂��B
                (���̗̈���f�o�b�K���g�p���邽��)

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void OS_InitLock(void)
{
    static BOOL isInitialized = FALSE;
#ifdef SDK_NITRO
    OSLockWord *lockp;
#endif

    if (isInitialized)
    {
        return;                        // do it only once
    }
    isInitialized = TRUE;

//---------------- NITRO
#ifdef SDK_NITRO
    lockp = (OSLockWord *)HW_INIT_LOCK_BUF;

#ifdef  SDK_ARM9
    {
        //
        // Code for MAIN PROCESSOR
        //

        lockp->lockFlag = 0;
        (void)OS_LockByWord(OS_MAINP_SYSTEM_LOCK_ID - 1, lockp, NULL);

        // �T�u�v���Z�b�T�ɂ�鋤�L���\�[�X���g�p�`�F�b�N
        while (lockp->extension != 0)
        {
            OSi_WaitByLoop();
        }

        // ���b�NID�J�E���^�p�t���O ������
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[0] = OSi_LOCKID_INITIAL_FLAG_0;
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[1] = OSi_LOCKID_INITIAL_FLAG_1;

        // ���b�N�o�b�t�@ �N���A�i�J�[�g���b�W�̈�ȊO�j
        MI_CpuClear32((void *)HW_SHARED_LOCK_BUF, HW_CTRDG_LOCK_BUF - HW_SHARED_LOCK_BUF);

        // NITRO �J�[�h�A�N�Z�X�� �� �T�u�v���Z�b�T
        MIi_SetCardProcessor(MI_PROCESSOR_ARM7);

        // �J�[�g���b�W�A�N�Z�X��   �� �T�u�v���Z�b�T
        MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM7);

#ifndef SDK_FINALROM
		{
			u32 type = OS_GetConsoleType();
			if ( type & OS_CONSOLE_TWLDEBUGGER )
			{
                _ISTDbgLib_Initialize();
            }
            else if ( type & (OS_CONSOLE_ISDEBUGGER | OS_CONSOLE_NITRO) )
            {
                _ISDbgLib_Initialize();
            }
        }
#endif

        (void)OS_UnlockByWord(OS_MAINP_SYSTEM_LOCK_ID - 1, lockp, NULL);
        (void)OS_LockByWord(OS_MAINP_SYSTEM_LOCK_ID, lockp, NULL);
    }

#else  // SDK_ARM7
    {
        //
        // Code for SUB PROCESSOR
        //

        lockp->extension = 0;
        while (lockp->ownerID != OS_MAINP_SYSTEM_LOCK_ID)
        {
            OSi_WaitByLoop();
        }

#ifndef SDK_FINALROM
        // [TODO]
        // �����R���|�[�l���g����ꂽ�ꍇ�ɂ́A
        // _ISTDbgLib_Initialize() ���ĂԃR���|�[�l���g���쐬����K�v������B
		{
			u32 type = OSi_DetectDebugger();
			if ( type & OS_CONSOLE_TWLDEBUGGER )
			{
				_ISTDbgLib_Initialize();
			}
			else if ( type & OS_CONSOLE_ISDEBUGGER )
			{
				_ISDbgLib_Initialize();
			}
        }
#endif

        // ���b�NID�J�E���^�p�t���O ������
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[0] = OSi_LOCKID_INITIAL_FLAG_0;
        ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[1] = OSi_LOCKID_INITIAL_FLAG_1;

        // ���C���v���Z�b�T�Ƃ̏������I���̓���
        lockp->extension = OS_SUBP_SYSTEM_LOCK_ID;
    }
#endif

//---------------- TWL
#else
#ifdef SDK_ARM9
    // ���b�NID�J�E���^�p�t���O ������
    ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[0] = OSi_LOCKID_INITIAL_FLAG_0;
    ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[1] = OSi_LOCKID_INITIAL_FLAG_1;

    // ���b�N�o�b�t�@ �N���A�i�J�[�g���b�W�̈�ȊO�j
    MI_CpuClear32((void *)HW_SHARED_LOCK_BUF, HW_CTRDG_LOCK_BUF - HW_SHARED_LOCK_BUF);

    // NITRO �J�[�h�A�N�Z�X�� �� �T�u�v���Z�b�T
    MIi_SetCardProcessor(MI_PROCESSOR_ARM7);

    // �J�[�g���b�W�A�N�Z�X��   �� �T�u�v���Z�b�T
    MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM7);

    //---- sync with ARM7
    OSi_SyncWithOtherProc( OSi_SYNCTYPE_SENDER, (void*)HW_INIT_LOCK_BUF );
    OSi_SyncWithOtherProc( OSi_SYNCTYPE_RECVER, (void*)HW_INIT_LOCK_BUF );

#ifndef SDK_FINALROM
	{
		u32 type = OSi_DetectDebugger();
		if ( type & OS_CONSOLE_TWLDEBUGGER )
		{
			_ISTDbgLib_Initialize();
		}
		else if ( type & OS_CONSOLE_ISDEBUGGER )
		{
			_ISDbgLib_Initialize();
		}
	}
#endif

    //lockp->lockFlag = 0;
    //(void)OS_LockByWord(OS_MAINP_SYSTEM_LOCK_ID, lockp, NULL);

#else  // SDK_ARM7
    // ���b�NID�J�E���^�p�t���O ������
    ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[0] = OSi_LOCKID_INITIAL_FLAG_0;
    ((u32 *)OSi_ANYP_LOCK_ID_FLAG)[1] = OSi_LOCKID_INITIAL_FLAG_1;

    //---- sync with ARM9
    OSi_SyncWithOtherProc( OSi_SYNCTYPE_RECVER, (void*)HW_INIT_LOCK_BUF );
    OSi_SyncWithOtherProc( OSi_SYNCTYPE_SENDER, (void*)HW_INIT_LOCK_BUF );

#ifndef SDK_FINALROM
	{
		u32 type = OSi_DetectDebugger();
		if ( type & OS_CONSOLE_TWLDEBUGGER )
		{
			_ISTDbgLib_Initialize();
		}
		else if ( type & OS_CONSOLE_ISDEBUGGER )
		{
			_ISDbgLib_Initialize();
		}
	}
#endif

#endif // SDK_ARM7

#endif // SDK_TWL
}

//======================================================================
//                      LOCK
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DoLockByWord

  Description:  do spinlock. keep to try till success

  Arguments:    lockID           lock ID
                lockp            pointer to lock variable
                ctrlFuncp        function
                disableFiq       whether do disable fiq

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
static s32 OSi_DoLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                            BOOL disableFiq)
{
    s32     lastLockFlag;

    while ((lastLockFlag =
            OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, disableFiq)) > OS_LOCK_SUCCESS)
    {
        OSi_WaitByLoop();
    }

    return lastLockFlag;
}

/*---------------------------------------------------------------------------*
  Name:         OS_LockByWord

  Description:  do spinlock. keep to try till success.

  Arguments:    lockID           lock ID
                lockp            pointer to lock variable
                ctrlFuncp        function

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoLockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_LockByWord_IrqAndFiq

  Description:  do spinlock. keep to try till success.
                disable irq and fiq

  Arguments:    lockID           lock ID
                lockp            pointer to lock variable
                ctrlFuncp        function

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoLockByWord(lockID, lockp, ctrlFuncp, TRUE);
}


//======================================================================
//                      UNLOCK
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DoUnlockByWord

  Description:  do unlock lock.

  Arguments:    lockID           lock ID
                lockp            pointer to unlock variable
                ctrlFuncp        function
                disableFiq       whether do disable fiq

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
static s32 OSi_DoUnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                              BOOL disableFIQ)
{
    OSIntrMode lastInterrupts;

    OSi_ASSERT_ID(lockID);

    if (lockID != lockp->ownerID)
    {
        return OS_UNLOCK_ERROR;
    }

    //---- Disable irq/fiq or irq
    lastInterrupts = (disableFIQ) ? OS_DisableInterrupts_IrqAndFiq() : OS_DisableInterrupts();

    lockp->ownerID = 0;
    if (ctrlFuncp)
    {
        ctrlFuncp();
    }
    lockp->lockFlag = 0;

    //---- Restore irq/fiq or irq
    if (disableFIQ)
    {
        (void)OS_RestoreInterrupts_IrqAndFiq(lastInterrupts);
    }
    else
    {
        (void)OS_RestoreInterrupts(lastInterrupts);
    }

    return OS_UNLOCK_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockByWord

  Description:  do unlock lock.
                disable irq and fiq

  Arguments:    lockID           lock ID
                lockp            pointer to unlock variable
                ctrlFuncp        function

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
s32 OS_UnLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockByWord_IrqAndFiq

  Description:  do unlock lock.

  Arguments:    lockID           lock ID
                lockp            pointer to unlock variable
                ctrlFuncp        function

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoUnlockByWord(lockID, lockp, ctrlFuncp, TRUE);
}


//======================================================================
//                      TRY LOCK
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OSi_DoTryLockByWord

  Description:  try to lock spinlock only once.

  Arguments:    lockID           lock ID
                lockp            pointer to trying to lock variable
                ctrlFuncp        function
                disableFiq       whether do disable fiq

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
static s32 OSi_DoTryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void),
                               BOOL disableFIQ)
{
    s32     lastLockFlag;
    OSIntrMode lastInterrupts;

    OSi_ASSERT_ID(lockID);

    //---- Disable irq/fiq or irq
    lastInterrupts = (disableFIQ) ? OS_DisableInterrupts_IrqAndFiq() : OS_DisableInterrupts();

    lastLockFlag = (s32)MI_SwapWord(lockID, &lockp->lockFlag);

    if (lastLockFlag == OS_LOCK_SUCCESS)
    {
        if (ctrlFuncp)
        {
            ctrlFuncp();
        }
        lockp->ownerID = lockID;
    }

    //---- Restore irq/fiq or irq
    if (disableFIQ)
    {
        (void)OS_RestoreInterrupts_IrqAndFiq(lastInterrupts);
    }
    else
    {
        (void)OS_RestoreInterrupts(lastInterrupts);
    }

    return lastLockFlag;
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockByWord

  Description:  try to lock spinlock only once.

  Arguments:    lockID           lock ID
                lockp            pointer to trying to lock variable
                ctrlFuncp        function

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockByWord(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockByWord_IrqAndFiq

  Description:  try to lock spinlock only once.

  Arguments:    lockID           lock ID
                lockp            pointer to trying to lock variable
                ctrlFuncp        function

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockByWord_IrqAndFiq(u16 lockID, OSLockWord *lockp, void (*ctrlFuncp) (void))
{
    return OSi_DoTryLockByWord(lockID, lockp, ctrlFuncp, TRUE);
}


//======================================================================
//                      CARTRIDGE
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_LockCartridge

  Description:  lock cartridge

  Arguments:    lockID           lock ID

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockCartridge(u16 lockID)
{
    s32     lastLockFlag;

    OSi_ASSERT_ID(lockID);

    lastLockFlag =
        OSi_DoLockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_AllocateCartridgeBus, TRUE);

#ifndef SDK_FINALROM
	{
		u32 type = OSi_DetectDebugger();
		if ( type & OS_CONSOLE_TWLDEBUGGER )
		{
			_ISTDbgLib_AllocateEmualtor();
		}
		else if ( type & OS_CONSOLE_ISDEBUGGER )
		{
			_ISDbgLib_AllocateEmualtor();
		}
    }
#endif

    return lastLockFlag;
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockCartridge

  Description:  unlock cartridge

  Arguments:    lockID           lock ID

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockCartridge(u16 lockID)
{
    s32     lastLockFlag;

    OSi_ASSERT_ID(lockID);

#ifndef SDK_FINALROM
	{
		u32 type = OSi_DetectDebugger();
		if ( type & OS_CONSOLE_TWLDEBUGGER )
		{
			_ISTDbgLib_FreeEmulator();
		}
		else if ( type & OS_CONSOLE_ISDEBUGGER )
		{		
			_ISDbgLib_FreeEmulator();
		}
	}
#endif

    lastLockFlag =
        OSi_DoUnlockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_FreeCartridgeBus, TRUE);

    return lastLockFlag;
}

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
asm s32 OS_UnLockCartridge( u16 lockID )
{
  ldr  r1, =OS_UnlockCartridge
  bx   r1
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockCartridge

  Description:  try to lock cartridge

  Arguments:    lockID           lock ID

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockCartridge(u16 lockID)
{
    s32     lastLockFlag;

    lastLockFlag =
        OSi_DoTryLockByWord(lockID, (OSLockWord *)HW_CTRDG_LOCK_BUF, OSi_AllocateCartridgeBus,
                            TRUE);

#ifndef SDK_FINALROM
	if ( lastLockFlag == OS_LOCK_SUCCESS )
	{
		u32 type = OSi_DetectDebugger();
		if ( type & OS_CONSOLE_TWLDEBUGGER )
		{
			_ISTDbgLib_AllocateEmualtor();
		}
		else if ( type & OS_CONSOLE_ISDEBUGGER )
		{
			_ISDbgLib_AllocateEmualtor();
		}
    }
#endif

    return lastLockFlag;
}

//----------------
static void OSi_AllocateCartridgeBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM9);       // Cartridge for MAIN
#endif
}

//----------------
static void OSi_FreeCartridgeBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCartridgeProcessor(MI_PROCESSOR_ARM7);       // Cartridge for SUB
#endif
}

//======================================================================
//                      CARD
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_LockCard

  Description:  lock card

  Arguments:    lockID           lock ID

  Returns:      OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_LockCard(u16 lockID)
{
    OSi_ASSERT_ID(lockID);

    return OS_LockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_AllocateCardBus);
}

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockCard

  Description:  unlock card

  Arguments:    lockID           lock ID

  Returns:      OS_UNLOCK_SUCCESS   success
                OS_UNLOCK_ERROR     error of unlocking lock
 *---------------------------------------------------------------------------*/
s32 OS_UnlockCard(u16 lockID)
{
    OSi_ASSERT_ID(lockID);

    return OS_UnlockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_FreeCardBus);
}

//---- for compatibility to old name ('UnLock' <-> 'Unlock')
asm s32 OS_UnLockCard( u16 lockID )
{
  ldr  r1, =OS_UnlockCard
  bx   r1
}

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockCard

  Description:  try to lock card

  Arguments:    lockID           lock ID

  Returns:      >0 value          previous locked id
                OS_LOCK_SUCCESS   success to lock
 *---------------------------------------------------------------------------*/
s32 OS_TryLockCard(u16 lockID)
{
    return OS_TryLockByWord(lockID, (OSLockWord *)HW_CARD_LOCK_BUF, OSi_AllocateCardBus);
}

//----------------
static void OSi_AllocateCardBus(void)
{
#ifdef  SDK_ARM9
#ifdef  SDK_TWL
    // preset reset flag with status of disable interrupts in OSi_DoTryLockByWord
    if ( ((reg_MI_MC & REG_MI_MC_SL1_MODE_MASK) >> REG_MI_MC_SL1_MODE_SHIFT) == 0x2 )
    {
        reg_MI_MCCNT1 |= REG_MI_MCCNT1_RESB_MASK;
    }
#endif
    MIi_SetCardProcessor(MI_PROCESSOR_ARM9);    // Card for MAIN
#endif
}

//----------------
static void OSi_FreeCardBus(void)
{
#ifdef  SDK_ARM9
    MIi_SetCardProcessor(MI_PROCESSOR_ARM7);    // Card for SUB
#endif
}


//======================================================================
//                      READ OWNER
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_ReadOwnerOfLockWord

  Description:  read owner id of lock

       �E���W���[��ID ����0 �̏ꍇ�͂��̎��_�łǂ���̃v���Z�b�T����
         ���L���������Ă���̂����m�F�ł��܂��B
       �E���L���\�[�X�̏ꍇ�́u���C���v���Z�b�T�������L���������Ă����ԁv
         �݂̂����荞�݂��֎~���邱�Ƃɂ���Ĉێ����邱�Ƃ��ł��܂��B
         ���̑��̏�Ԃ̓T�u�v���Z�b�T���ω������Ă��܂��\��������܂��B
       �E���L���W���[��ID �� 0 �ł����Ă����b�N�ϐ�����������Ă���Ƃ͌���܂���B

  Arguments:    lockp            pointer to lock

  Returns:      owner id
 *---------------------------------------------------------------------------*/
u16 OS_ReadOwnerOfLockWord(OSLockWord *lockp)
{
    return lockp->ownerID;
}

//======================================================================
//                     LOCK ID
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_GetLockID

  Description:  get lock ID

  Arguments:    None.

  Returns:      OS_LOCK_ID_ERROR, if fail to get ID

                if ARM9
                   0x40�`0x6f       lockID
                else if ARM7
                   0x80�`0xaf       lockID
                endif

                ID��48��ނ܂ł������蓖�Ă邱�Ƃ��ł��܂���B
                ���W���[�����ɂĕ����̃��b�N�ϐ��𐧌䂷��ꍇ��
                �ł��邾��1��ID���g�p����悤�ɂ��ĉ������B
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm s32 OS_GetLockID( void )
{
    //---- �t���O�̑O32�r�b�g�ɗ����Ă���t���O(��ID)�����邩
    ldr    r3, =OSi_ANYP_LOCK_ID_FLAG
    ldr    r1, [r3, #0]

#ifdef SDK_ARM9
    clz    r2, r1
#else
    mov    r2, #0
    mov    r0, #0x80000000
_lp1:
    tst    r1, r0
    bne    _ex1
    add    r2, r2, #1
    cmp    r2, #32
    beq    _ex1

    mov    r0, r0, lsr #1
    b      _lp1
 _ex1:
#endif
    cmp    r2, #32

    //---- ��ID������Ƃ�
    movne  r0, #OSi_ANYP_LOCK_ID_START
    bne    _1

    //---- ���323�r�b�g�ɗ����Ă���t���O(��ID)�����邩
    add    r3, r3, #4
    ldr    r1, [r3, #0]
#ifdef SDK_ARM9
    clz    r2, r1
#else
    mov    r2, #0
    mov    r0, #0x80000000
_lp2:
    tst    r1, r0
    bne    _ex2
    add    r2, r2, #1
    cmp    r2, #32
    beq    _ex2

    mov    r0, r0, lsr #1
    b      _lp2
 _ex2:
#endif
    cmp    r2, #32

    //---- ��ID���Ȃ�
    ldr    r0, =OS_LOCK_ID_ERROR
    bxeq   lr

    //---- ��ID������Ƃ�
    mov    r0, #OSi_ANYP_LOCK_ID_START+32

_1:
    add    r0, r0, r2
    mov    r1, #0x80000000
    mov    r1, r1, lsr r2

    ldr    r2, [r3, #0]
    bic    r2, r2, r1
    str    r2, [r3, #0]

    bx     lr
}

/*---------------------------------------------------------------------------*
  Name:         OS_ReleaseLockID

  Description:  release lock ID

  Arguments:    id to tend to release

  Returns:      None.
 *---------------------------------------------------------------------------*/
asm void OS_ReleaseLockID( register u16 lockID )
{
#pragma unused( lockID )

    ldr    r3, =OSi_ANYP_LOCK_ID_FLAG

    cmp    r0, #OSi_ANYP_LOCK_ID_START+32
    addpl  r3, r3, #4

    subpl  r0, r0, #OSi_ANYP_LOCK_ID_START+32
    submi  r0, r0, #OSi_ANYP_LOCK_ID_START

    mov r1, #0x80000000
    mov r1, r1, lsr r0

    ldr    r2, [r3, #0]
    orr    r2, r2, r1
    str    r2, [r3, #0]

    bx     lr
}
#include <nitro/codereset.h>
