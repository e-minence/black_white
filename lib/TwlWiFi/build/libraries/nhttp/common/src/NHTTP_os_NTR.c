/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NHTTP - libraries
  File:     NHTTP_os_NTR.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include "nhttp.h"
#include "NHTTPlib_int.h"
#include "d_nhttp_common.h"
#include "NHTTP_nonport.h"
#include "NHTTP_thread.h"

extern void NHTTPi_InitMutex(NHTTPMutex* mutex_p)
{
    OS_InitMutex(mutex_p);
}

extern void NHTTPi_LockMutex(NHTTPMutex* mutex_p)
{
    OS_LockMutex(mutex_p);
}

extern void NHTTPi_UnlockMutex(NHTTPMutex* mutex_p)
{
    OS_UnlockMutex(mutex_p);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_createCommThread
  Description:  �X���b�h�쐬
  Returns:      TRUE	�����ɐ�������
                FALSE	�����Ɏ��s����
 *-------------------------------------------------------------------------*/
extern BOOL NHTTPi_createCommThread(NHTTPThreadInfo* threadInfo_p, u32 priority, u32* stack_p)
{
    if (NHTTPi_IsCreateCommThreadMessageQueue(threadInfo_p) == FALSE)
    {
        OS_InitMessageQueue(&(threadInfo_p->commThreadMessageQueue), threadInfo_p->commThreadMessageArray, MSGARRAY);
        NHTTPi_IsCreateCommThreadMessageQueueOn(threadInfo_p);
    }

    OS_CreateThread(&(threadInfo_p->commThread),
                    NHTTPi_CommThreadProc,
                    NULL,
                    &stack_p[NHTTP_COMMTHREAD_STACK/sizeof(u32)],
                    NHTTP_COMMTHREAD_STACK,
                    priority);
    OS_WakeupThreadDirect(&(threadInfo_p->commThread));
    return TRUE;
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_destroyCommThread
  
  Description:  �X���b�h���I�������č폜
 *-------------------------------------------------------------------------*/
extern void NHTTPi_destroyCommThread(NHTTPThreadInfo* threadInfo_p, NHTTPBgnEndInfo* bgnEndInfo_p)
{
    bgnEndInfo_p->isThreadEnd = TRUE;
    NHTTPi_kickCommThread(threadInfo_p);
    
    OS_JoinThread(&(threadInfo_p->commThread));
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_idleCommThread
  
  Description:  kick�����܂ŃX���b�h���x�~
 *-------------------------------------------------------------------------*/
extern void NHTTPi_idleCommThread(NHTTPThreadInfo* threadInfo_p)
{
    OSMessage msg;
    OS_ReceiveMessage(&(threadInfo_p->commThreadMessageQueue), &msg, OS_MESSAGE_BLOCK);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_kickCommThread
  
  Description:  �x�~���Ă���X���b�h���N����
 *-------------------------------------------------------------------------*/
extern void NHTTPi_kickCommThread(NHTTPThreadInfo* threadInfo_p)
{
    OS_SendMessage(&(threadInfo_p->commThreadMessageQueue), NULL, OS_MESSAGE_NOBLOCK);
}

/*-------------------------------------------------------------------------*
  Name:         NHTTPi_CheckCurrentThread

  Description:  ���ݏ������̃X���b�h�� NHTTP�X���b�h���`�F�b�N���܂��D
                check�Ŏw�肵�������ƍ���Ȃ��ꍇ
                �x�������o�͂��������~���܂��D
                
  
  Arguments:    handle  - �ڑ��n���h��
                buf     - ��M�o�b�t�@
                length  - �o�b�t�@�T�C�Y
                
  Returns:      ��M�o�b�t�@���Z�b�g�ł���� 0, �����łȂ���� -1
 *-------------------------------------------------------------------------*/
extern void NHTTPi_CheckCurrentThread(NHTTPThreadInfo* threadInfo_p, NHTTPThreadIs is)
{
    OSThread*	thread_p = OS_GetCurrentThread();
    OSThread*	commThread_p = &(threadInfo_p->commThread);

    if ((thread_p != NULL) &&
        (((is == NHTTP_CURRENT_THREAD_IS_NHTTP_THREAD) &&
          (thread_p != commThread_p)) ||
         ((is != NHTTP_CURRENT_THREAD_IS_NHTTP_THREAD) &&
          (thread_p == commThread_p))))
    {
        OS_TPrintf("%s:illegal thread\n",__func__);
        OS_TPanic("halt\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         NHTTPi_CommThreadProc
  Description:  HTTP�ʐM�X���b�h
                NHTTPi_Startup() ���� NHTTPi_CleanupAsync() �܂ő��݂���
  Arguments:    param
 *---------------------------------------------------------------------------*/
extern void NHTTPi_CommThreadProc(void* param)
{
    (void)param;
    NHTTPi_CommThreadProcMain();
}
