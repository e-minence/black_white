/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MATH - libraries
  File:     dgt_hmac.c

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

#include <nitro/math/dgt.h>
#include "hmac.h"

/*  TWLSDK �� WPA �̂��߂� ARM7 ���� LTDWRAM �ɔz�u */
#ifdef SDK_TWLHYB
#ifdef SDK_ARM7
#include <twl/ltdwram_begin.h>
#endif
#endif

/*---------------------------------------------------------------------------*
  Name:         MATHi_CalcHMAC

  Description:  HMAC��p����MAC�l���v�Z����.

  Arguments:    mac:            �ړI��MAC�l���i�[�����
                message:        ���b�Z�[�W�f�[�^
                message_length: ���b�Z�[�W�f�[�^�̒���
                key:            ��
                key_length:     ���̒���
                funcs:          HMAC���p����n�b�V���֐��̍\����

  Returns:      None
 *---------------------------------------------------------------------------*/
void MATHi_CalcHMAC(void *mac, const void *message, u32 message_length,
                               const void *key,     u32 key_length,
                               MATHiHMACFuncs *funcs)
{
    int i;
    u8 new_key[MATH_HASH_BLOCK_SIZE];
    u8 *use_key;
    u32 use_key_length;
    u8 ipad_key[MATH_HASH_BLOCK_SIZE];
    u8 opad_key[MATH_HASH_BLOCK_SIZE];
    
    /* ����Ȃ��v�f������Ώ����͂����Ȃ�Ȃ�. */
    if((mac == NULL)||(message == NULL)||(message_length == 0)||(key == NULL)||
       (key_length == 0)||(funcs == NULL))
    {
        return;
    }
    
    /* �����u���b�N�����������ꍇ�A�n�b�V���l�����Ƃ���. */
    if(key_length > funcs->blength)
    {
        funcs->HashReset(funcs->context);
        funcs->HashSetSource(funcs->context, key, key_length);
        funcs->HashGetDigest(funcs->context, new_key);
        use_key = new_key;
        use_key_length = funcs->dlength;
    }
    else
    {
        use_key = (u8*)key;
        use_key_length = key_length;
    }
    
    /* ����ipad��XOR */
    for(i = 0; i < use_key_length; i++)
    {
        ipad_key[i] = (u8)(use_key[i] ^ 0x36);
    }
    /* ���̃p�f�B���O������ipad��XOR */
    for(; i < funcs->blength; i++)
    {
        ipad_key[i] = 0x00 ^ 0x36;
    }
    
    /* ���b�Z�[�W�Ƃ̌����ƃn�b�V���l�̌v�Z */
    funcs->HashReset(funcs->context);
    funcs->HashSetSource(funcs->context, ipad_key, funcs->blength);
    funcs->HashSetSource(funcs->context, message, message_length);
    funcs->HashGetDigest(funcs->context, funcs->hash_buf);

    /* ����opad��XOR */
    for(i = 0; i < use_key_length; i++)
    {
        opad_key[i] = (u8)(use_key[i] ^ 0x5c);
    }
    /* ���̃p�f�B���O������ipad��XOR */
    for(; i < funcs->blength; i++)
    {
        opad_key[i] = 0x00 ^ 0x5c;
    }
    
    /* �n�b�V���l�Ƃ̌����ƃn�b�V���l�̌v�Z */
    funcs->HashReset(funcs->context);
    funcs->HashSetSource(funcs->context, opad_key, funcs->blength);
    funcs->HashSetSource(funcs->context, funcs->hash_buf, funcs->dlength);
    funcs->HashGetDigest(funcs->context, mac);
}

#ifdef SDK_TWLHYB
#ifdef SDK_ARM7
#include <twl/ltdwram_end.h>
#endif
#endif
