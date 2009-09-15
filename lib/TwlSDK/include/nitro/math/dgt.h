/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MATH - include
  File:     math/dgt.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev:$
  $Author:$
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MATH_DGT_H_
#define NITRO_MATH_DGT_H_

#ifndef SDK_WIN32
#include <nitro/misc.h>
#endif
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Constant definition
//----------------------------------------------------------------------------

// �_�C�W�F�X�g��
#define MATH_MD5_DIGEST_SIZE    (128/8)     // 128bit
#define MATH_SHA1_DIGEST_SIZE   (160/8)     // 160bit
#define MATH_SHA256_DIGEST_SIZE 32          // 256bit

// �ő�̃_�C�W�F�X�g��
#define MATH_HASH_DIGEST_SIZE_MAX  MATH_SHA1_DIGEST_SIZE

// �����u���b�N��
#define MATH_HASH_BLOCK_SIZE    (512/8)
#define MATH_MD5_BLOCK_SIZE     MATH_HASH_BLOCK_SIZE     // 512bit
#define MATH_SHA1_BLOCK_SIZE    MATH_HASH_BLOCK_SIZE     // 512bit

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

typedef struct MATHMD5Context
{
    union
    {
        struct
        {
            unsigned long a, b, c, d;
        };
        unsigned long state[4];
    };
    unsigned long long length;
    union
    {
        unsigned long buffer32[16];
        unsigned char buffer8[64];
    };
} MATHMD5Context;

typedef struct MATHSHA1Context
{
    u32     h[5];                        /* H0,H1,H2,H3,H4 */
    u8      block[MATH_SHA1_BLOCK_SIZE]; /* current message block */
    u32     pool;                        /* message length in 'block' */
    u32     blocks_low;                  /* total blocks (in bytes) */
    u32     blocks_high;
}
MATHSHA1Context;

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

/*****************************************************************************/
/* MD5                                                                       */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_MD5Init

  Description:  MD5 �l�����߂邽�߂Ɏg�� MATHMD5Context �\���̂�����������B
  
  Arguments:    context MATHMD5Context �\����
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_MD5Init(MATHMD5Context * context);

/*---------------------------------------------------------------------------*
  Name:         MATH_MD5Update

  Description:  MD5 �l��^�����f�[�^�ōX�V����B
  
  Arguments:    context MATHMD5Context �\����
                input   ���̓f�[�^�ւ̃|�C���^
                length  ���̓f�[�^��
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_MD5Update(MATHMD5Context * context, const void *input, u32 length);

/*---------------------------------------------------------------------------*
  Name:         MATH_MD5GetHash

  Description:  �ŏI�I�� MD5 �l�𓾂�B
  
  Arguments:    context MATHMD5Context �\����
                digest  MD5 �l���i�[����ꏊ�ւ̃|�C���^
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_MD5GetHash(MATHMD5Context * context, void *digest);

// ���ʌ݊��̂���
static inline void MATH_MD5GetDigest(MATHMD5Context * context, void *digest)
{
    MATH_MD5GetHash(context, digest);
}


/*****************************************************************************/
/* SHA-1                                                                     */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_SHA1Init

  Description:  SHA1 �l�����߂邽�߂Ɏg�� MATHSHA1Context �\���̂�����������B
  
  Arguments:    context MATHSHA1Context �\����
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_SHA1Init(MATHSHA1Context * context);

/*---------------------------------------------------------------------------*
  Name:         MATH_SHA1Update

  Description:  SHA1 �l��^�����f�[�^�ōX�V����B
  
  Arguments:    context MATHSHA1Context �\����
                input   ���̓f�[�^�ւ̃|�C���^
                length  ���̓f�[�^��
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_SHA1Update(MATHSHA1Context * context, const void *input, u32 length);

/*---------------------------------------------------------------------------*
  Name:         MATH_SHA1GetHash

  Description:  �ŏI�I�� SHA1 �l�𓾂�B
  
  Arguments:    context MATHSHA1Context �\����
                digest  SHA1 �l���i�[����ꏊ�ւ̃|�C���^
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_SHA1GetHash(MATHSHA1Context * context, void *digest);

// ���ʌ݊��̂���
static inline void MATH_SHA1GetDigest(MATHSHA1Context * context, void *digest)
{
    MATH_SHA1GetHash(context, digest);
}


/*****************************************************************************/
/* SHA256                                                                     */
/*****************************************************************************/
#define MATHSHA256_CBLOCK	64
#define MATHSHA256_LBLOCK	16
#define MATHSHA256_BLOCK	16
#define MATHSHA256_LAST_BLOCK  56
#define MATHSHA256_LENGTH_BLOCK 8
#define MATHSHA256_DIGEST_LENGTH 32

typedef struct MATHSHA256Context MATHSHA256Context;
typedef void (MATHSHA256_BLOCK_FUNC)(MATHSHA256Context *c, u32 *W, int num); 

struct MATHSHA256Context
{
    u32 h[8];
    u32 Nl,Nh;
    u8 data[MATHSHA256_CBLOCK];
    int num;
};

void MATH_SHA256Init(MATHSHA256Context *c);
void MATH_SHA256Update(MATHSHA256Context *c, const void* data, u32 len);
void MATH_SHA256GetHash(MATHSHA256Context *c, void* digest);
void MATH_CalcSHA256(void* digest, const void* data, u32 dataLength);


/*****************************************************************************/
/* ���[�e�B���e�B�֐�                                                        */
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcMD5

  Description:  MD5 ���v�Z����B
  
  Arguments:    digest  MD5 �l���i�[����ꏊ�ւ̃|�C���^
                data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_CalcMD5(void *digest, const void *data, u32 dataLength);

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcSHA1

  Description:  SHA-1 ���v�Z����B
  
  Arguments:    digest  SHA-1 �l���i�[����ꏊ�ւ̃|�C���^
                data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void    MATH_CalcSHA1(void *digest, const void *data, u32 dataLength);


/*---------------------------------------------------------------------------*
  Name:         MATH_CalcHMACMD5

  Description:  HMAC-MD5 ���v�Z����B
  
  Arguments:    digest  HMAC-MD5 �l���i�[����ꏊ�ւ̃|�C���^
                data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
                key     ���̃|�C���^
                keyLength   ���̒���
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATH_CalcHMACMD5(void *digest, const void *data, u32 dataLength, const void *key, u32 keyLength);

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcHMACSHA1

  Description:  HMAC-SHA-1 ���v�Z����B
  
  Arguments:    digest  HMAC-SHA-1 �l���i�[����ꏊ�ւ̃|�C���^
                data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
                key     ���̃|�C���^
                keyLength   ���̒���
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATH_CalcHMACSHA1(void *digest, const void *data, u32 dataLength, const void *key, u32 keyLength);

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcHMACSHA256

  Description:  HMAC-SHA-256 ���v�Z����B
  
  Arguments:    digest  HMAC-SHA-256 �l���i�[����ꏊ�ւ̃|�C���^
                data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
                key     ���̃|�C���^
                keyLength   ���̒���
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATH_CalcHMACSHA256(void *digest, const void *data, u32 dataLength, const void *key, u32 keyLength);

int MATHi_SetOverlayTableMode( int flag );

#ifdef __cplusplus
}/* extern "C" */
#endif

/* NITRO_MATH_DGT_H_ */
#endif
