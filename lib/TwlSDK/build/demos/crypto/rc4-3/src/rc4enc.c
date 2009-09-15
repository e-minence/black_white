/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CRYPTO - demos
  File:     rc4enc.c

  Copyright 2006-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-20#$
  $Rev: 9005 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Pseudo Random Number Generator
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/crypto.h>

#include "prng.h"
#include "rc4enc.h"

/*---------------------------------------------------------------------------*
    �ϐ���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         InitRC4Encoder

  Description:  RC4 �A���S���Y���ňÍ������s�����߂̏��������s���B

  Arguments:    context - RC4 �̌����Ȃǂ����߂�R���e�L�X�g�\����
                key     - 12�o�C�g�̌��f�[�^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void InitRC4Encoder(
    RC4EncoderContext* context,
    const void* key
)
{
    MI_CpuClear8(context, sizeof(RC4EncoderContext));

    // ���̐ݒ�
    MI_CpuCopy8(key, context->key, RC4ENC_USER_KEY_LENGTH);
}

/*---------------------------------------------------------------------------*
  Name:         EncodeRC4

  Description:  RC4 �A���S���Y���ňÍ������s���B

  Arguments:    context - RC4 �̌����Ȃǂ����߂��R���e�L�X�g�\����
                in      - ���̓f�[�^
                in_len  - �f�[�^��
                out     - �o�̓f�[�^
                out_len - �o�̓o�b�t�@�Ƃ��Ċm�ۂ��Ă��钷��

  Returns:      ����������o�͒�, ���s������ 0
 *---------------------------------------------------------------------------*/
u32 EncodeRC4(
    RC4EncoderContext* context,
    const void* in,
    u32 in_len,
    void* out,
    u32 out_len
)
{
    u8 digest[MATH_SHA1_DIGEST_SIZE];
    u32 iv;
    u8* out_ptr = (u8*)out;

    if ((out_len < in_len)
        ||
        (out_len - in_len < RC4ENC_ADDITIONAL_SIZE))
    {
        // �o�̓o�b�t�@������Ȃ�
        return 0;
    }
    
    // �����_���� IV (Initialization Vector) �̍쐬
    GetRandomBytes((u8*)(&iv), sizeof(iv));

    // TODO: �l�b�g���[�N�o�C�g�I�[�_�[�Ή��̂��߂� Endian �ϊ�
    MI_CpuCopy8(&iv, out_ptr, sizeof(iv));
    out_ptr += sizeof(iv);

    // �w�肳�ꂽ���� IV ���������āA����g�p���錮���쐬
    MI_CpuCopy8(&iv, &context->key[RC4ENC_USER_KEY_LENGTH], sizeof(iv));
    CRYPTO_RC4Init(&context->rc4_context, context->key, sizeof(context->key));

    // ���̓f�[�^�̈Í���
    CRYPTO_RC4Encrypt(&context->rc4_context, in, in_len, out_ptr);
    out_ptr += in_len;

    // ���̓f�[�^�̃��b�Z�[�W�_�C�W�F�X�g�l�̌v�Z
    MATH_CalcSHA1(digest, in, in_len);

    // �_�C�W�F�X�g�l�̈Í���
    CRYPTO_RC4Encrypt(&context->rc4_context, digest, MATH_SHA1_DIGEST_SIZE, out_ptr);
    out_ptr += sizeof(digest);

    return (u32)(out_ptr - out);
}

/*---------------------------------------------------------------------------*
  Name:         InitRC4Decoder

  Description:  RC4 �A���S���Y���ŕ������s�����߂̏��������s���B

  Arguments:    context - RC4 �̌����Ȃǂ����߂�R���e�L�X�g�\����
                key     - 12�o�C�g�̌��f�[�^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void InitRC4Decoder(
    RC4DecoderContext* context,
    const void* key
)
{
    MI_CpuClear8(context, sizeof(RC4DecoderContext));

    // ���̐ݒ�
    MI_CpuCopy8(key, context->key, RC4ENC_USER_KEY_LENGTH);
}

/*---------------------------------------------------------------------------*
  Name:         DecodeRC4

  Description:  RC4 �A���S���Y���ŕ������s���B
                ���̍ہA�f�[�^�̉��σ`�F�b�N���s���A���ς���Ă����玸�s����B

  Arguments:    context - RC4 �̌����Ȃǂ����߂��R���e�L�X�g�\����
                in      - ���̓f�[�^
                in_len  - �f�[�^��
                out     - �o�̓f�[�^
                out_len - �o�̓o�b�t�@�Ƃ��Ċm�ۂ��Ă��钷��

  Returns:      ����������o�͒�, ���s������ 0
 *---------------------------------------------------------------------------*/
u32 DecodeRC4(
    RC4DecoderContext* context,
    const void* in,
    u32 in_len,
    void* out,
    u32 out_len
)
{
    u8 digest[MATH_SHA1_DIGEST_SIZE];
    u8 decrypted_digest[MATH_SHA1_DIGEST_SIZE];
    u32 iv;
    u8* in_ptr = (u8*)in;
    u32 data_len = in_len - RC4ENC_ADDITIONAL_SIZE;

    if ((in_len < RC4ENC_ADDITIONAL_SIZE)
        ||
        (out_len < data_len))
    {
        // �o�̓o�b�t�@������Ȃ�
        return 0;
    }
    
    // IV �̎擾
    // TODO: �l�b�g���[�N�o�C�g�I�[�_�[�Ή��̂��߂� Endian �ϊ�
    MI_CpuCopy8(in_ptr, &iv, sizeof(iv));
    in_ptr += sizeof(iv);

    // ����g�p���錮�̍쐬
    MI_CpuCopy8(&iv, &context->key[RC4ENC_USER_KEY_LENGTH], sizeof(iv));
    CRYPTO_RC4Init(&context->rc4_context, context->key, sizeof(context->key));

    // ���̓f�[�^�̕���
    CRYPTO_RC4Decrypt(&context->rc4_context, in_ptr, data_len, out);
    in_ptr += data_len;

    // �����f�[�^�̃��b�Z�[�W�_�C�W�F�X�g�l�̌v�Z
    MATH_CalcSHA1(digest, out, data_len);

    // �_�C�W�F�X�g�l�̕���
    CRYPTO_RC4Decrypt(&context->rc4_context, in_ptr, MATH_SHA1_DIGEST_SIZE, decrypted_digest);
    in_ptr += data_len;

    // �_�C�W�F�X�g�l���������l�ɂȂ��Ă��邩������
    {
        int i;
        for (i = 0; i < MATH_SHA1_DIGEST_SIZE; i++)
        {
            if (digest[i] != decrypted_digest[i])
            {
                // ���؎��s
                return 0;
            }
        }
    }

    return data_len;
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
