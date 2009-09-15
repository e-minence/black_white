/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_task.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_LIBRARIES_CARD_TASK_H__
#define NITRO_LIBRARIES_CARD_TASK_H__


#include <nitro/os.h>


/*---------------------------------------------------------------------------*
 * �D��x���^�X�N�̃L���[���Ǘ����郆�[�e�B���e�B�B
 * CARD���C�u���������ł̂ݎg�p���Ă��邪�P�̂Ƃ��Đ؂�o���\�B
 *---------------------------------------------------------------------------*/


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */


// ���[�J�X���b�h�Ƀo�b�N�O���E���h�ŏ���������^�X�N�\���́B
struct CARDTask;
typedef void (*CARDTaskFunction)(struct CARDTask *);

typedef struct CARDTask
{
    struct CARDTask    *next;
    u32                 priority;
    void               *userdata;
    CARDTaskFunction    function;
    CARDTaskFunction    callback;
}
CARDTask;

// �^�X�N�L���[�\���́B
typedef struct CARDTaskQueue
{
    CARDTask * volatile list;
    OSThreadQueue       workers[1];
    u32                 quit:1;
    u32                 dummy:31;
}
CARDTaskQueue;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitTaskQueue

  Description:  �^�X�N�L���[�\���̂��������B

  Arguments:    queue  : CARDTaskQueue�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitTaskQueue(CARDTaskQueue *queue);

/*---------------------------------------------------------------------------*
  Name:         CARDi_QuitTaskQueue

  Description:  �^�X�N�L���[���Ď�����S�Ẵ��[�J�X���b�h���I����QUIT��Ԃɂ���B
                �^�X�N�L���[�̏�Ԃ͂��̂܂܎c��A�������̃^�X�N�͊��������B

  Arguments:    queue : CARDTaskQueue�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_QuitTaskQueue(CARDTaskQueue *queue);

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
                    CARDTaskFunction function, CARDTaskFunction callback);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProcessTask

  Description:  �^�X�N�����̏�Ŏ��s���邩�^�X�N�L���[�֒ǉ��B

  Arguments:    queue          : CARDTaskQueue�\���́B
                task           : ���s�܂��̓L���[�֒ǉ�����^�X�N�B
                blocking       : �L���[�֒ǉ��������̏�Ŏ��s����Ȃ�TRUE�B
                                 �L���[�֒ǉ����ă��[�J�X���b�h�֔C����Ȃ�FALSE�B
                changePriority : �^�X�N�ɐݒ肳�ꂽ�D��x�Ŏ��s����Ȃ�TRUE�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ProcessTask(CARDTaskQueue *queue, CARDTask *task, BOOL blocking, BOOL changePriority);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReceiveTask

  Description:  �^�X�N�L���[���玟�̃^�X�N���擾�B

  Arguments:    queue    : CARDTaskQueue�\���́B
                blocking : �L���[����̏ꍇ�Ƀu���b�N����Ȃ�TRUE�B

  Returns:      �擾�ł����V�K�^�X�N�B
                ��u���b�L���O���L���[���󂩁A�܂��̓L���[��QUIT��ԂȂ�NULL�B
 *---------------------------------------------------------------------------*/
CARDTask* CARDi_ReceiveTask(CARDTaskQueue *queue, BOOL blocking);

/*---------------------------------------------------------------------------*
  Name:         CARDi_TaskWorkerProcedure

  Description:  �^�X�N�L���[���Ď��������郏�[�J�X���b�h�̃v���V�[�W���B
                ���̃v���V�[�W�����w�肵�Đ��������X���b�h��
                CARDi_QuitTask�֐��𔭍s����܂Ń^�X�N��������������B
                �P��̃^�X�N�L���[�ɑ΂��ă��[�J�X���b�h�𕡐��������Ă��悢�B

  Arguments:    arg : �Ď����ׂ�CARDTaskQueue�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_TaskWorkerProcedure(void *arg);


/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_LIBRARIES_CARD_TASK_H__
