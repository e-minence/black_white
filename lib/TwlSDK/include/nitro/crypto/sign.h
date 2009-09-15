/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include
  File:     crypto/sign.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CRYPTO_SIGN_H_
#define NITRO_CRYPTO_SIGN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_VerifySignatureWithHash

  Description:  �n�b�V���l����d�q���������؂���

  Arguments:    hash_ptr - �n�b�V���l�ւ̃|�C���^
                sign_ptr - �d�q�����ւ̃|�C���^
                mod_ptr - ���J���ւ̃|�C���^
  Returns:      �F�؂ł����� TRUE
                �F�؂ł��Ȃ������� FALSE
 *---------------------------------------------------------------------------*/
int CRYPTO_VerifySignatureWithHash(
    const void* hash_ptr,
    const void* sign_ptr,
    const void* mod_ptr
);

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_VerifySignature

  Description:  �f�[�^����d�q���������؂���

  Arguments:    data_ptr - �f�[�^�ւ̃|�C���^
                data_len - �f�[�^�̃T�C�Y
                sign_ptr - �d�q�����ւ̃|�C���^
                mod_ptr - ���J���ւ̃|�C���^

  Returns:      �F�؂ł����� TRUE
                �F�؂ł��Ȃ������� FALSE
 *---------------------------------------------------------------------------*/
int CRYPTO_VerifySignature(
    const void* data_ptr,
    int   data_len,
    const void* sign_ptr,
    const void* mod_ptr
);


/*---------------------------------------------------------------------------*
  Name:         CRYPTO_SIGN_GetModulus

  Description:  ���J����modulus�ւ̃|�C���^��Ԃ�
                (�Ԃ��Ă����|�C���^�ւ̃A�h���X���ȉ���API�̈��� mod_ptr �ɓn��
                   CRYPTO_VerifySignature, CRYPTO_VerifySignatureWithHash )

  Arguments:    pub_ptr - ���J���ւ̃|�C���^
  Returns:      ���������ꍇ�� modulus �ւ̃A�h���X���Ԃ�
                ���s�����ꍇ��NULL���Ԃ�
 *---------------------------------------------------------------------------*/
void *CRYPTO_SIGN_GetModulus(
	const void* pub_ptr
);


#ifdef __cplusplus
}
#endif

#endif //_NITRO_CRYPTO_SIGN_H_
