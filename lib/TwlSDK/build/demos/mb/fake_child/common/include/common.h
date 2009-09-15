/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - fake_child
  File:     common.h

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
*---------------------------------------------------------------------------*/
#ifndef MB_DEMO_COMMON_H_
#define MB_DEMO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SDK_TWL
#include	<twl.h>
#else
#include	<nitro.h>
#endif

//============================================================================
//  �֐��錾
//============================================================================

void    CommonInit();
void    ReadKey(void);
u16     GetPressKey(void);
u16     GetTrigKey(void);
void    InitAllocateSystem(void);

/*---------------------------------------------------------------------------*
  Name:         IS_PAD_PRESS

  Description:  �L�[����

  Arguments:    ���肷��L�[�t���O

  Returns:      �w�肵���L�[����������Ă���ꍇ�� TRUE
                ����Ă��Ȃ��ꍇ�� FALSE
 *---------------------------------------------------------------------------*/
static inline BOOL IS_PAD_PRESS(u16 flag)
{
    return (GetPressKey() & flag) == flag;
}

/*---------------------------------------------------------------------------*
  Name:         IS_PAD_TRIGGER

  Description:  �L�[�g���K����

  Arguments:    ���肷��L�[�t���O

  Returns:      �w�肵���L�[�̃g���K�������Ă���ꍇ�� TRUE
                �����Ă��Ȃ��ꍇ�� FALSE
 *---------------------------------------------------------------------------*/
static inline BOOL IS_PAD_TRIGGER(u16 flag)
{
    return (GetTrigKey() & flag) == flag;
}


#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // MB_DEMO_COMMON_H_
