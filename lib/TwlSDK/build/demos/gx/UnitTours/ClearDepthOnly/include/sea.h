/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tests - GX
  File:     sea.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef _SEA_H_
#define _SEA_H_

#ifdef SDK_TWL
#include <twl.h>
#else
#include <nitro.h>
#endif

/* �֎q���f�� */
extern const u32 isu1[];
extern const u32 isu2[];

/* �֎q�T�C�Y */
extern const u32 isu1_size;
extern const u32 isu2_size;

/* �֎q�e�N�X�`�� */
extern const u16 tex_isu1[];
extern const u16 tex_isu2[];

/* �֎q�p���b�g */
extern const u16 pal_isu1[];
extern const u16 pal_isu2[];

/* �֎q�e�N�X�`���y�уp���b�g�̃T�C�Y */
extern const u32 tex_isu1_size;
extern const u32 tex_isu2_size;

extern const u32 pal_isu1_size;
extern const u32 pal_isu2_size;

/* �C�X�N���[�� */

extern const unsigned int d_natsunoumi_sscDT[32 * 32 / 2];
extern const unsigned int d_natsunoumi_sclDT[8 * 16];
extern const unsigned int d_natsunoumi_schDT[16 * 736];

#endif /* _SEA_H_ */
