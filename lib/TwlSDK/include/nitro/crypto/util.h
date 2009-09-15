/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include
  File:     crypto/util.h

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

#ifndef NITRO_CRYPTO_UTIL_H_
#define NITRO_CRYPTO_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_SetAllocator

  Description:  CRYPTO ���C�u�����Ŏg�p���郁�����Ǘ��֐���o�^����
                �ߋ��@��Ƃ̌݊��̂��߂Ɏc�����֐�

  Arguments:    alloc - �������m�ۊ֐��ւ̃|�C���^
                free  - ����������֐��ւ̃|�C���^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
#define CRYPTO_SetAllocator(alloc, free)    CRYPTO_SetMemAllocator(alloc, free, NULL)


/*---------------------------------------------------------------------------*
  Name:         CRYPTO_SetMemAllocator

  Description:  CRYPTO ���C�u�����Ŏg�p���郁�����Ǘ��֐���o�^����

  Arguments:    alloc   - �������m�ۊ֐��ւ̃|�C���^
                free    - ����������֐��ւ̃|�C���^
                realloc - �������T�C�Y�ύX�֐��ւ̃|�C���^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void CRYPTO_SetMemAllocator(
    void* (*alloc) (u32),
    void  (*free) (void*),
    void* (*realloc) (void*,u32,u32)
);



#define BER_INTEGER              2
#define BER_BIT_STRING           3
#define BER_OCTET_STRING         4
#define BER_NULL                 5
#define BER_OBJECT               6
#define BER_SEQUENCE            16
#define BER_CONSTRUCTED       0x20

/*---------------------------------------------------------------------------*
  Name:         CRYPTO_DerSkip

  Description:  ASN.1�I�u�W�F�N�g(DER)�̂����^�O�t�B�[���h�ƒ����t�B�[���h��
                �X�L�b�v����

  Arguments:    datap - �o�b�t�@�ւ̃|�C���^
                dlenp - �o�b�t�@��
                type  - �^�O�ԍ�
                lenp  - �����t�B�[���h�̒l

  Returns:      1 : �X�L�b�v�ɐ���
                0 : �X�L�b�v�Ɏ��s
 *---------------------------------------------------------------------------*/
int
CRYPTO_DerSkip(
    unsigned char **datap,
    unsigned int   *dlenp,
    unsigned char   type,
    unsigned int   *lenp
);

#ifdef __cplusplus
}
#endif

#endif //_NITRO_CRYPTO_UTIL_H_
