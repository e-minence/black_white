/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_common.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_LIBRARIES_CARD_COMMON_H__
#define NITRO_LIBRARIES_CARD_COMMON_H__


#include <nitro/card/common.h>
#include <nitro/card/backup.h>


#include "../include/card_utility.h"
#include "../include/card_task.h"
#include "../include/card_command.h"



#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// CARD �������
enum
{
    // �������ς݁B
    CARD_STAT_INIT = (1 << 0),

    // �R�}���hPXI�������ς݁B (ARM9)
    CARD_STAT_INIT_CMD = (1 << 1),

    // �R�}���h�𔭍s���Ċ������Ă��Ȃ���ԁB(ARM9)
    // WaitForTask()�̏����O�����̏�ԂɊ܂܂��B
    CARD_STAT_BUSY = (1 << 2),

    // �ڊǂ��ꂽ�^�X�N�������ԁB(ARM9/ARM7)
    CARD_STAT_TASK = (1 << 3),

    // ARM7����̃��N�G�X�g�����ʒm�҂��B (ARM9)
    CARD_STAT_WAITFOR7ACK = (1 << 5),

    // �L�����Z���v�����B
    CARD_STAT_CANCEL = (1 << 6)
};

#define CARD_UNSYNCHRONIZED_BUFFER  (void*)0x80000000

typedef enum
{
    CARD_TARGET_NONE,
    CARD_TARGET_ROM,
    CARD_TARGET_BACKUP,
    CARD_TARGET_RW
}
CARDTargetMode;

typedef u32 CARDAccessLevel;
#define CARD_ACCESS_LEVEL_NONE      0
#define CARD_ACCESS_LEVEL_BACKUP    1
#define CARD_ACCESS_LEVEL_ROM       2
#define CARD_ACCESS_LEVEL_FULL      3


/*---------------------------------------------------------------------------*/
/* declarations */

typedef s32 CARDiOwner;

// CARD ���ʃp�����[�^
typedef struct CARDiCommon
{
    // ���N�G�X�g�ʐM�p���L�������B
    CARDiCommandArg *cmd;

    // ��ԃt���O�B
    volatile u32    flag;

    // �^�X�N�̃f�t�H���g�D��x�B
    u32             priority;

#if	defined(SDK_ARM9)
    // ROM���[�h���̃L���b�V���S�̖�����臒l�B
    // ���T�C�Y������ꍇ��FlushRange���FlushAll�̕����������B
    u32     flush_threshold_ic;
    u32     flush_threshold_dc;
#endif

    // �J�[�h�A�N�Z�X���Ǘ��B
    // �v���Z�b�T���̃J�[�h/�o�b�N�A�b�v�r�������B
    // �����CARD�A�N�Z�X���g�p���镡���̔񓯊��֐�(Rom&Backup)��
    // ����X���b�h����Ăяo�����ꍇ�����邽�߂ɕK�v�B
    //
    // OSMutex ���X���b�h�ɕt���ĉ��̂ɑ΂��Ă���� lock-ID �ɕt���ĉ��B
    volatile CARDiOwner lock_owner;    // ==s32 with Error status
    volatile int        lock_ref;
    OSThreadQueue       lock_queue[1];
    CARDTargetMode      lock_target;

    // �^�X�N�X���b�h���B
    struct
    {
        OSThread    context[1];
        u8          stack[0x400];
    }
    thread;

#if defined(SDK_ARM7)
    // �V�`���̃^�X�N�v���V�[�W���B
    CARDTask        task[1];
    CARDTaskQueue   task_q[1];
    // ARM9���疽�߂���Ă���ŐV�̃R�}���h�B
    int             command;
    u8              padding[20];
#else
    // ���`���̃^�X�N�v���V�[�W���B
    void          (*task_func) (struct CARDiCommon *);
    // ���[�U�R�[���o�b�N�ƈ����B
    MIDmaCallback   callback;
    void           *callback_arg;
    // �^�X�N�����҂��X���b�h�B
    OSThreadQueue   busy_q[1];
    // API�^�X�N�p�����[�^�B
    u32                     src;
    u32                     dst;
    u32                     len;
    u32                     dma;
    const CARDDmaInterface *DmaCall;
    // �o�b�N�A�b�v�R�}���h�Ǘ����B
    CARDRequest             req_type;
    int                     req_retry;
    CARDRequestMode         req_mode;
    OSThread               *current_thread_9;
//    u8                      padding[32];
#endif

}
CARDiCommon;


SDK_COMPILER_ASSERT(sizeof(CARDiCommon) % 32 == 0);


/*---------------------------------------------------------------------------*/
/* variables */

extern CARDiCommon cardi_common;
extern u32  cardi_rom_base;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_ExecuteOldTypeTask

  Description:  �񓯊��Ȃ烏�[�J�X���b�h�֓����ē����Ȃ炱�̏�Ŏ��s�B
                (���łɃ^�X�N�X���b�h��CARDi_WaitForTask()�ɂ���Ĕr������
                 ����Ă��邱�Ƃ͂��̊֐��̌Ăяo�����ŕۏ؂���)

  Arguments:    task       �ݒ肷��^�X�N�֐�
                async      �񓯊���������]����Ȃ�TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL CARDi_ExecuteOldTypeTask(void (*task) (CARDiCommon *), BOOL async);

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
BOOL CARDi_WaitForTask(CARDiCommon *p, BOOL restart, MIDmaCallback callback, void *callback_arg);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EndTask

  Description:  �^�X�N�I����ʒm���ă^�X�N�X���b�h�̎g�p���������

  Arguments:    p            ���C�u�����̃��[�N�o�b�t�@ (�����̂��߂Ɉ����n��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EndTask(CARDiCommon *p);

void CARDi_OldTypeTaskThread(void *arg);


/*---------------------------------------------------------------------------*
  Name:         CARDi_GetTargetMode

  Description:  CARD �o�X�̌��݂̃��b�N�^�[�Q�b�g���擾

  Arguments:    None.

  Returns:      CARDTargetMode �Ŏ������3�̏�Ԃ̂����ꂩ
 *---------------------------------------------------------------------------*/
SDK_INLINE CARDTargetMode CARDi_GetTargetMode(void)
{
    return cardi_common.lock_target;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_LockResource

  Description:  ���\�[�X�r���̃��b�N

  Arguments:    owner      ���b�N�̏��L�҂����� lock-ID
                target     ���b�N���ׂ��J�[�h�o�X��̃��\�[�X�^�[�Q�b�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_LockResource(CARDiOwner owner, CARDTargetMode target);

/*---------------------------------------------------------------------------*
  Name:         CARDi_UnlockResource

  Description:  ���\�[�X�r���̃A�����b�N

  Arguments:    owner      ���b�N�̏��L�҂����� lock-ID
                target     �A�����b�N���ׂ��J�[�h�o�X��̃��\�[�X�^�[�Q�b�g

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_UnlockResource(CARDiOwner owner, CARDTargetMode target);

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetAccessLevel

  Description:  ������Ă���ROM�A�N�Z�X���x�����擾

  Arguments:    None.

  Returns:      ������Ă���ROM�A�N�Z�X���x��
 *---------------------------------------------------------------------------*/
CARDAccessLevel CARDi_GetAccessLevel(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitAsync

  Description:  �񓯊�������҂�

  Arguments:    None.

  Returns:      �ŐV�̏������ʂ�CARD_RESULT_SUCCESS�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARDi_WaitAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryWaitAsync

  Description:  �񓯊������ҋ@�����s���Đ��ۂɊւ�炸�������ɐ����Ԃ�

  Arguments:    None.

  Returns:      �ŐV�̔񓯊��������������Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARDi_TryWaitAsync(void);

void CARDi_InitResourceLock(void);



void CARDi_InitCommand(void);


#ifdef __cplusplus
} // extern "C"
#endif



#endif // NITRO_LIBRARIES_CARD_COMMON_H__
