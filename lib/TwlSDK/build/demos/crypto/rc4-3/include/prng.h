/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CRYPTO - demos
  File:     prng.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prng.h,v $
  Revision 1.1  2006/03/08 08:53:41  seiki_masashi
  rc4-3 �f���̒ǉ�

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef PRNG_H_
#define PRNG_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/types.h>

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         AddRandomSeed

  Description:  PRNG �ɗ����̎��ǉ�����

  Arguments:    random_seed - �ǉ����闐���̎�ւ̃|�C���^
                len - �ǉ����闐���̎�̒���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void AddRandomSeed(u8* random_seed, u32 len);

/*---------------------------------------------------------------------------*
  Name:         ChurnRandomPool

  Description:  PRNG �Ɍ��݂̃V�X�e����Ԃ𗐐��̎�Ƃ��Ēǉ�����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ChurnRandomPool(void);

/*---------------------------------------------------------------------------*
  Name:         GetRandomBytes

  Description:  PRNG ���痐������肷��

  Arguments:    buffer - �������i�[����o�b�t�@�ւ̃|�C���^
                len - �擾�����������̃o�C�g��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void GetRandomBytes(u8* buffer, u32 len);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif  /* PRNG_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
