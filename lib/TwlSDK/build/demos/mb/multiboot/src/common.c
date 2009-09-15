/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot
  File:     common.c

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

#ifdef SDK_TWL
#include	<twl.h>
#else
#include	<nitro.h>
#endif


#include "common.h"


/*
 * ���̃f���S�̂Ŏg�p���鋤�ʋ@�\.
 */


/******************************************************************************/
/* constant */

/* ���̃f�����_�E�����[�h������v���O�������̔z�� */
const MBGameRegistry mbGameList[GAME_PROG_MAX] = {
    {
     "/em.srl",                        // �}���`�u�[�g�q�@�o�C�i���̃t�@�C���p�X
     L"edgeDemo",                      // �Q�[����
     L"edgemarking demo\ntesttesttest", // �Q�[�����e����
     "/data/icon.char",                // �A�C�R���L�����N�^�f�[�^�̃t�@�C���p�X
     "/data/icon.plt",                 // �A�C�R���p���b�g�f�[�^�̃t�@�C���p�X
     0x12123434,                       // GameGroupID(GGID)
     16,                               // �ő�v���C�l��
     },
    {
     "/pol_toon.srl",
     L"PolToon",
     L"toon rendering",
     "/data/icon.char",
     "/data/icon.plt",
     0x56567878,
     8,
     },
};


/******************************************************************************/
/* variable */

/* MB ���C�u�����֊��蓖�Ă郏�[�N�̈� */
u32     cwork[MB_SYSTEM_BUF_SIZE / sizeof(u32)];

/* ���̃f���̃Q�[���V�[�P���X��� */
u8      prog_state;


/******************************************************************************/
/* function */

/* �L�[�g���K���o */
u16 ReadKeySetTrigger(u16 keyset)
{
    static u16 cont = 0xffff;          /* IPL ���ł� A �{�^�������΍� */
    u16     trigger = (u16)(keyset & (keyset ^ cont));
    cont = keyset;
    return trigger;
}

/* min - max �͈̔͂�val�l�� offset�����[�e�[�V���� */
BOOL RotateU8(u8 *val, u8 min, u8 max, s8 offset)
{
    int     lVal = (int)*val - min;
    int     div = max - min + 1;

    if (div == 0)
        return FALSE;

    lVal = (lVal + offset + div) % div + min;

    *val = (u8)lVal;
    return TRUE;
}
