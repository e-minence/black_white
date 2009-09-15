/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CRYPTO - demos
  File:     rc4enc.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: rc4enc.h,v $
  Revision 1.3  2006/03/08 09:15:35  seiki_masashi
  �R�����g�̏C��

  Revision 1.2  2006/03/08 09:14:44  seiki_masashi
  �R�����g�̏C��

  Revision 1.1  2006/03/08 08:53:41  seiki_masashi
  rc4-3 �f���̒ǉ�

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef RC4ENC_H_
#define RC4ENC_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <nitro/types.h>

#define RC4ENC_USER_KEY_LENGTH  12 // ���[�U���w�肷�錮��
#define RC4ENC_TOTAL_KEY_LENGTH 16 // �ŏI�I�Ȍ���

// �G���R�[�h���ɒǉ��ŕK�v�ɂȂ�T�C�Y (24�o�C�g)
#define RC4ENC_ADDITIONAL_SIZE (sizeof(u32) /* iv */ + MATH_SHA1_DIGEST_SIZE /* message digest */)

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/
typedef struct RC4EncoderContext
{
    CRYPTORC4Context rc4_context;
    u8 key[RC4ENC_TOTAL_KEY_LENGTH];
} RC4EncoderContext;

typedef struct RC4DecoderContext
{
    CRYPTORC4Context rc4_context;
    u8 key[RC4ENC_TOTAL_KEY_LENGTH];
} RC4DecoderContext;

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
);

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
);

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
);

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
);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif  /* RC4ENC_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
