/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include
  File:     crypto/prng.h

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CRYPTO_PRNG_H_
#define NITRO_CRYPTO_PRNG_H_

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    �^��`
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    ��l�\���̐錾
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
    �֐��錾
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_PRNG_GatherEntropy

  Description:  ���������̂��߂̃G���g���s�[�쐬

  Arguments:    �Ȃ�

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_PRNG_GatherEntropy( void );

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_PRNG_GenerateRandom

  Description:  �����𐶐�����

  Arguments:    randomBytes:  �����o�̓o�b�t�@
                size:         �o�̓T�C�Y

  Returns:      0 - ����, ����ȊO - ���s
 *---------------------------------------------------------------------------*/
s32 CRYPTO_PRNG_GenerateRandom( u8 *randomBytes, u32 size );


/* for internal use */



#ifdef __cplusplus
}
#endif

#endif //NITRO_CRYPTO_PRNG_H_
