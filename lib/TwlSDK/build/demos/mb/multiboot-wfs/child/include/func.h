/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot-wfs
  File:     func.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef __NITROSDK_DEMO_MB_MULTIBOOT_WFS_FUNC_H
#define __NITROSDK_DEMO_MB_MULTIBOOT_WFS_FUNC_H

#ifdef SDK_TWL
#include	<twl.h>
#else
#include	<nitro.h>
#endif


/******************************************************************************/
/* macro */

/*
 * �I�[�o�[���C���W���[���T�C�Y���璷�ɑ���������ړI�̃}�N��.
 * volatile int v; OVERLAY_CODE_BY_10000(++v);
 * ���̂悤�Ɏg�p���܂�.
 * �r���h���Ԃ�(���ɍœK������)�ɒ[�ɒ����Ȃ�_�ɒ��ӂ��Ă�������.
 */
#define	OVERLAY_CODE_BY_10(expr)		(expr), (expr), (expr), (expr), (expr), (expr), (expr), (expr), (expr), (expr)
#define	OVERLAY_CODE_BY_100(expr)		OVERLAY_CODE_BY_10(OVERLAY_CODE_BY_10(expr))
#define	OVERLAY_CODE_BY_1000(expr)		OVERLAY_CODE_BY_10(OVERLAY_CODE_BY_100(expr))
#define	OVERLAY_CODE_BY_10000(expr)		OVERLAY_CODE_BY_10(OVERLAY_CODE_BY_1000(expr))
#define	OVERLAY_CODE_BY_100000(expr)	OVERLAY_CODE_BY_10(OVERLAY_CODE_BY_10000(expr))
#define	OVERLAY_CODE_BY_1000000(expr)	OVERLAY_CODE_BY_10(OVERLAY_CODE_BY_100000(expr))
#define	OVERLAY_CODE_BY_10000000(expr)	OVERLAY_CODE_BY_10(OVERLAY_CODE_BY_1000000(expr))


/******************************************************************************/
/* function */

#if	defined(__cplusplus)
extern  "C"
{
#endif


/* �I�[�o�[���C�������Ă��邱�Ƃ��m�F���邽�߂̕�������擾���܂� */
    void    func_1(char *dst);
    void    func_2(char *dst);
    void    func_3(char *dst);


#if	defined(__cplusplus)
}                                      /* extern "C" */
#endif


/******************************************************************************/


#endif                                 /* __NITROSDK_DEMO_FS_OVERLAY_FUNC_H */
