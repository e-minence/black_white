/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MI - include
  File:     card.h

  Copyright 2004-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_CARD_H_
#define NITRO_MI_CARD_H_


#ifdef __cplusplus
extern "C" {
#endif

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
#ifdef __DUMMY________
#include <nitro/os.h>
#include <nitro/card.h>


/******************************************************************************
 * �b�`�q�c�A�N�Z�X
 *
 *	������ Lock / Unlock ������ōs�������̒P���� CARD API ���b�p�[�ł�.
 *	CARD API �ւ̈ڍs�ɔ���, �����͔p�~�����\��ł�.
 *
 ******************************************************************************/


// MI-CARD �֐��p�� LockID ���g�p���� CARD �m�� / ���.
void    MIi_LockCard(void);
void    MIi_UnlockCard(void);

// �J�[�h ID �̓ǂݏo��. (����)
static inline u32 MI_ReadCardID(void)
{
    u32     ret;
    MIi_LockCard();
    ret = CARDi_ReadRomID();
    MIi_UnlockCard();
    return ret;
}

// �J�[�h�ǂݍ���. (����)
static inline void MIi_ReadCard(u32 dmaNo, const void *src, void *dst, u32 size)
{
    MIi_LockCard();
    /*
     * �ᐅ���� CARD_ReadRom �ŉ\�Ȍ���p�t�H�[�}���X��ۏ؂��邽��,
     * ���� / �񓯊��ɂ�����炸���荞�݂��g�p����悤�ɕύX.
     * ���������̊֐������荞�݋֎~�̂܂܎g�p����Ă����������̂�,
     * MI �ɂ��Ă͓����łŖ������� CPU �]�����g�p����悤�ύX.
     * (�}���`�X���b�h�łȂ���Ό����͈ꏏ)
     */
    (void)dmaNo;
    CARD_ReadRom((MI_DMA_MAX_NUM + 1), src, dst, size);
    MIi_UnlockCard();
}

// �J�[�h�f�[�^�ǂݍ���. (�񓯊�)
void    MIi_ReadCardAsync(u32 dmaNo, const void *srcp, void *dstp, u32 size,
                          MIDmaCallback callback, void *arg);

// �J�[�h�f�[�^�񓯊��ǂݍ��݊����̊m�F.
static inline BOOL MIi_TryWaitCard(void)
{
    return CARD_TryWaitRomAsync();
}

// �J�[�h�f�[�^�񓯊��ǂݍ��ݏI���҂�.
static inline void MIi_WaitCard(void)
{
    CARD_WaitRomAsync();
}

#endif
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz


#ifdef __cplusplus
} /* extern "C" */
#endif


/* NITRO_MI_CARD_H_ */
#endif
