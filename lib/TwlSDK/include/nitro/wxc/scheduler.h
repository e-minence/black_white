/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     scheduler.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_WXC_SCHEDULER_H_
#define	NITRO_WXC_SCHEDULER_H_


#include <nitro.h>


/*****************************************************************************/
/* constant */

/* �e�q�ϓ��p�^�[�� */
#define WXC_SCHEDULER_PATTERN_MAX   4
#define WXC_SCHEDULER_SEQ_MAX       4


/*****************************************************************************/
/* declaration */

/* �e�@���[�h,�q�@���[�h�̐؂�ւ����Ǘ�����\���� */
typedef struct WXCScheduler
{
    /* ���݂̐؂�ւ��V�[�P���X */
    int     seq;
    int     pattern;
    int     start;
    BOOL    child_mode;
    /* �؂�ւ��e�[�u�� (TRUE �͐e�@, FALSE �͎q�@) */
    BOOL    table[WXC_SCHEDULER_PATTERN_MAX][WXC_SCHEDULER_SEQ_MAX];
}
WXCScheduler;


/*****************************************************************************/
/* function */

#ifdef __cplusplus
extern "C"
{
#endif


/*---------------------------------------------------------------------------*
  Name:         WXCi_InitScheduler

  Description:  ���[�h�؂�ւ��X�P�W���[����������.

  Arguments:    p             WXCScheduler �\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXCi_InitScheduler(WXCScheduler * p);

/*---------------------------------------------------------------------------*
  Name:         WXCi_SetChildMode

  Description:  �X�P�W���[�����q�@���Œ�œ��삷��悤�ݒ�.

  Arguments:    p             WXCScheduler �\����
                enable        �q�@���ł����N�������Ȃ��ꍇ�� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXCi_SetChildMode(WXCScheduler * p, BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         WXCi_UpdateScheduler

  Description:  ���[�h�؂�ւ��X�P�W���[�����X�V.

  Arguments:    p             WXCScheduler �\����

  Returns:      ���ݐe�@���[�h�Ȃ� TRUE ��, �q�@���[�h�Ȃ� FALSE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL    WXCi_UpdateScheduler(WXCScheduler * p);


#ifdef __cplusplus
}
#endif


#endif /* NITRO_WXC_SCHEDULER_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
