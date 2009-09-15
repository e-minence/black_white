/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_task.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-21#$
  $Rev: 9385 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/card_task.h"


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitTaskQueue

  Description:  �^�X�N�L���[�\���̂��������B

  Arguments:    queue  : CARDTaskQueue�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitTaskQueue(CARDTaskQueue *queue)
{
    queue->list = NULL;
    queue->quit = FALSE;
    OS_InitThreadQueue(queue->workers);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_QuitTaskQueue

  Description:  �^�X�N�L���[���Ď�����S�ẴX���b�h���I���B

  Arguments:    queue : CARDTaskQueue�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_QuitTaskQueue(CARDTaskQueue *queue)
{
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    queue->quit = TRUE;
    OS_WakeupThread(queue->workers);
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitTask

  Description:  �^�X�N�\���̂��������B

  Arguments:    task     : CARDTask�\���́B
                priority : �X���b�h�D��x�B
                userdata : �^�X�N�Ɋ֘A�t����C�ӂ̃��[�U��`�f�[�^�B
                function : ���s������^�X�N�̊֐��|�C���^�B
                callback : �^�X�N������ɌĂяo���R�[���o�b�N�B (�s�v�Ȃ�NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitTask(CARDTask *task, u32 priority, void *userdata,
                    CARDTaskFunction function, CARDTaskFunction callback)
{
    task->next = NULL;
    task->priority = priority;
    task->userdata = userdata;
    task->function = function;
    task->callback = callback;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProcessTask

  Description:  �^�X�N�L���[�̏I�[�Ƀ^�X�N�\���̂�ǉ����邩���̏�Ŏ��s�B

  Arguments:    queue          : CARDTaskQueue�\���́B
                task           : �ǉ��܂��͎��s����^�X�N�B
                blocking       : �u���b�L���O����Ȃ�TRUE�B
                                 ���̏ꍇ�A�L���[�ւ͒ǉ��������̏�Ŏ��s����B
                changePriority : �^�X�N�ɐݒ肳�ꂽ�D��x�ɏ]���Ȃ�TRUE�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ProcessTask(CARDTaskQueue *queue, CARDTask *task, BOOL blocking, BOOL changePriority)
{
    // ��u���b�L���O����Ȃ�L���[�I�[�֒ǉ��B
    if (!blocking)
    {
        OSIntrMode  bak_cpsr = OS_DisableInterrupts();
        CARDTask  **pp = (CARDTask **)&queue->list;
        for(; *pp; pp = &(*pp)->next)
        {
        }
        *pp = task;
        // ��̃L���[�ɑ΂���ŏ��̃^�X�N�������烏�[�J�X���b�h�֒ʒm�B
        if (pp == &queue->list)
        {
            OS_WakeupThread(queue->workers);
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
    // �u���b�L���O����Ȃ炱�̏�Ń^�X�N�ƃR�[���o�b�N�����s�B
    else
    {
        // �K�v�Ȃ�^�X�N�ɐݒ肳�ꂽ�D��x�̂��Ƃœ��삷��B
        OSThread   *curth = OS_GetCurrentThread();
        u32         prio = 0;
        if (changePriority)
        {
            prio = OS_GetThreadPriority(curth);
            (void)OS_SetThreadPriority(curth, task->priority);
        }
        if (task->function)
        {
            (*task->function)(task);
        }
        // �R�[���o�b�N���Ŏ��̃^�X�N��ݒ肳��Ă��ǂ��悤�ɂ���B
        if (task->callback)
        {
            (*task->callback)(task);
        }
        // �ꎞ�I�ɕύX�����D��x�𕜋A����B
        if (changePriority)
        {
            (void)OS_SetThreadPriority(curth, prio);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReceiveTask

  Description:  �^�X�N���X�g���玟�̃^�X�N���擾�B

  Arguments:    queue    : CARDTaskQueue�\���́B
                blocking : ���݂��Ȃ��ꍇ�Ƀu���b�L���O����Ȃ�TRUE�B

  Returns:      �擾�����V�K�^�X�N�܂���NULL
 *---------------------------------------------------------------------------*/
CARDTask* CARDi_ReceiveTask(CARDTaskQueue *queue, BOOL blocking)
{
    CARDTask   *retval = NULL;
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    while (!queue->quit)
    {
        retval = queue->list;
        if ((retval != NULL) || !blocking)
        {
            break;
        }
        OS_SleepThread(queue->workers);
    }
    if (retval)
    {
        queue->list = retval->next;
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TaskWorkerProcedure

  Description:  �^�X�N���X�g�������������郏�[�J�X���b�h�̃v���V�[�W���B

  Arguments:    arg : CARDTaskQueue�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_TaskWorkerProcedure(void *arg)
{
    CARDTaskQueue  *queue = (CARDTaskQueue*)arg;
    // �ǂ̂悤�ȗD��x�̃^�X�N���󂯕t���邽�߁A�ҋ@���͍ō����x���B
    (void)OS_SetThreadPriority(OS_GetCurrentThread(), 0);
    for (;;)
    {
        // �L���[���Ď����Ď��̃^�X�N�����܂őҋ@�B
        CARDTask   *task = CARDi_ReceiveTask(queue, TRUE);
        // QUIT��ԂȂ�u���b�L���O���[�h�ł�NULL���Ԃ����B
        if (!task)
        {
            break;
        }
        // �w�肳�ꂽ�D��x�̂��ƂɃ^�X�N�ƃR�[���o�b�N�����s�B
        CARDi_ProcessTask(queue, task, TRUE, TRUE);
    }
}





#if defined(SDK_ARM9)
/*---------------------------------------------------------------------------*
 * �Â��^�X�N�X���b�h����
 *---------------------------------------------------------------------------*/

#include "../include/card_common.h"

/*---------------------------------------------------------------------------*
  Name:         CARDi_ExecuteOldTypeTask

  Description:  �񓯊��Ȃ烏�[�J�X���b�h�֓����ē����Ȃ炱�̏�Ŏ��s�B
                (���łɃ^�X�N�X���b�h��CARDi_WaitForTask()�ɂ���Ĕr������
                 ����Ă��邱�Ƃ͂��̊֐��̌Ăяo�����ŕۏ؂���)

  Arguments:    task       �ݒ肷��^�X�N�֐�
                async      �񓯊���������]����Ȃ�TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL CARDi_ExecuteOldTypeTask(void (*task) (CARDiCommon *), BOOL async)
{
    CARDiCommon *p = &cardi_common;
    if (async)
    {
        // �܂��X���[�v���̃^�X�N�X���b�h�D��x��ύX�B
        (void)OS_SetThreadPriority(p->thread.context, p->priority);
        // ����������^�X�N��ݒ肵, �X���b�h���N�����B
        p->task_func = task;
        p->flag |= CARD_STAT_TASK;
        OS_WakeupThreadDirect(p->thread.context);
    }
    else
    {
        (*task)(p);
        CARDi_EndTask(p);
    }
    return async ? TRUE : (p->cmd->result == CARD_RESULT_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_OldTypeTaskThread

  Description:  �^�X�N�X���b�h�̃��C���֐�

  Arguments:    arg          �s�g�p

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_OldTypeTaskThread(void *arg)
{
    CARDiCommon *p = &cardi_common;
    (void)arg;
    for (;;)
    {
        // ���̏�����҂B
        OSIntrMode bak_psr = OS_DisableInterrupts();
        for (;;)
        {
            if ((p->flag & CARD_STAT_TASK) != 0)
            {
                break;
            }
            OS_SleepThread(NULL);
        }
        (void)OS_RestoreInterrupts(bak_psr);
        // ���N�G�X�g�����B
        (void)CARDi_ExecuteOldTypeTask(p->task_func, FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitForTask

  Description:  �^�X�N�X���b�h�̎g�p�������擾�ł���܂ő҂�
                (����̃o�X�̃��b�N�͂��̊֐��̌Ăяo�����ŕۏ؂���)

  Arguments:    p            ���C�u�����̃��[�N�o�b�t�@
                restart      ����Ɏ��̃^�X�N���J�n������Ȃ�TRUE
                callback     �A�N�Z�X�I����̃R�[���o�b�N�֐�
                callback_arg �R�[���o�b�N�֐��̈���

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL CARDi_WaitForTask(CARDiCommon *p, BOOL restart, MIDmaCallback callback, void *callback_arg)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    // ���݂̏�������������܂őҋ@�B
    while ((p->flag & CARD_STAT_BUSY) != 0)
    {
        OS_SleepThread(p->busy_q);
    }
    // �����Ɏ��̃R�}���h�����s����Ȃ炱�̏�ōĂя������ցB
    if (restart)
    {
        p->flag |= CARD_STAT_BUSY;
        p->callback = callback;
        p->callback_arg = callback_arg;
    }
    (void)OS_RestoreInterrupts(bak_psr);
    return (p->cmd->result == CARD_RESULT_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EndTask

  Description:  �^�X�N�I����ʒm���ă^�X�N�X���b�h�̎g�p���������

  Arguments:    p            ���C�u�����̃��[�N�o�b�t�@ (�����̂��߂Ɉ����n��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EndTask(CARDiCommon *p)
{
    MIDmaCallback   callback = p->callback;
    void           *userdata = p->callback_arg;

    OSIntrMode bak_psr = OS_DisableInterrupts();
    // ������Ԃɂ��đҋ@���̃X���b�h�֒ʒm�B
    p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK | CARD_STAT_CANCEL);
    OS_WakeupThread(p->busy_q);
    (void)OS_RestoreInterrupts(bak_psr);
    // �K�v�Ȃ�R�[���o�b�N���Ăяo���B
    if (callback)
    {
        (*callback) (userdata);
    }
}

#endif // defined(SDK_ARM9)
