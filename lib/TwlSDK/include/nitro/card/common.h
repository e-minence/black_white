/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     common.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_CARD_COMMON_H_
#define NITRO_CARD_COMMON_H_


#include <nitro/card/types.h>

#include <nitro/memorymap.h>
#include <nitro/mi/dma.h>
#include <nitro/os.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// �J�[�h�X���b�h�̃f�t�H���g�D�惌�x��
#define	CARD_THREAD_PRIORITY_DEFAULT	4


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_Init

  Description:  DARD���C�u������������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_Init(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_IsAvailable

  Description:  CARD���C�u���������p�\������

  Arguments:    None.

  Returns:      CARD_Init�֐������łɌĂяo����Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARD_IsAvailable(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_IsEnabled

  Description:  �J�[�h�ɃA�N�Z�X�\������

  Arguments:    None.

  Returns:      �J�[�h�ɃA�N�Z�X���錠���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARD_IsEnabled(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_CheckEnabled

  Description:  �J�[�h�ɃA�N�Z�X���錠�������邩���肵�A�Ȃ���΋�����~

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_CheckEnabled(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_Enable

  Description:  �J�[�h�ɃA�N�Z�X���錠����ݒ�

  Arguments:    enable      �A�N�Z�X���錠���������TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_Enable(BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetThreadPriority

  Description:  ���C�u���������̔񓯊������p�X���b�h�̗D��x���擾

  Arguments:    None.

  Returns:      �����X���b�h�̗D��x
 *---------------------------------------------------------------------------*/
u32     CARD_GetThreadPriority(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_SetThreadPriority

  Description:  ���C�u���������̔񓯊������p�X���b�h�̗D��x��ݒ�

  Arguments:    None.

  Returns:      �ݒ�O�̓����X���b�h�̗D��x
 *---------------------------------------------------------------------------*/
u32     CARD_SetThreadPriority(u32 prior);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetResultCode

  Description:  �Ō�ɌĂяo����CARD�֐��̌��ʂ��擾

  Arguments:    None.

  Returns:      �Ō�ɌĂяo����CARD�֐��̌���
 *---------------------------------------------------------------------------*/
CARDResult CARD_GetResultCode(void);


/*---------------------------------------------------------------------------*
 * for internal use
 *---------------------------------------------------------------------------*/

// CARD���C�u���������C�x���g
typedef u32 CARDEvent;
#define CARD_EVENT_PULLEDOUT  0x00000001
#define CARD_EVENT_SLOTRESET  0x00000002

// �C�x���g�t�b�N�R�[���o�b�N�ƃt�b�N�o�^�\����
typedef void (*CARDHookFunction)(void*, CARDEvent, void*);

typedef struct CARDHookContext
{
    struct CARDHookContext *next;
    void                   *userdata;
    CARDHookFunction        callback;
}
CARDHookContext;

/*---------------------------------------------------------------------------*
  Name:         CARDi_RegisterHook

  Description:  CARD���C�u�����̓����C�x���g�t�b�N��o�^

  Arguments:    hook        �o�^�Ɏg�p����t�b�N�\����
                callback    �C�x���g�������ɌĂяo���R�[���o�b�N�֐�
                arg         �R�[���o�b�N����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_RegisterHook(CARDHookContext *hook, CARDHookFunction callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         CARDi_UnregisterHook

  Description:  CARD���C�u�����̓����C�x���g�t�b�N������

  Arguments:    hook        �o�^�Ɏg�p�����t�b�N�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_UnregisterHook(CARDHookContext *hook);

/*---------------------------------------------------------------------------*
  Name:         CARDi_NotifyEvent

  Description:  CARD���C�u�����̓����C�x���g��S�Ẵt�b�N�ɒʒm

  Arguments:    event       ���������C�x���g
                arg         �C�x���g���Ƃ̈���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_NotifyEvent(CARDEvent event, void *arg);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_CARD_COMMON_H_
