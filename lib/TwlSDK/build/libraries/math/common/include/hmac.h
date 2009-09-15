/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MATH - include
  File:     dgt.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_LIBRARIES_MATH_COMMON_HMAC_H_
#define NITRO_LIBRARIES_MATH_COMMON_HMAC_H_

#include <nitro/types.h>

typedef struct MATHiHMACFuncs {
    const u32 dlength;
    const u32 blength;
    void *context;
    u8 *hash_buf;
    void (*HashReset)(void*);
    void (*HashSetSource)(void*, const void*, u32);
    void (*HashGetDigest)(void*, void*);
} MATHiHMACFuncs;


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
                               MATHiHMACFuncs *funcs);

#endif  // ifndef NITRO_LIBRARIES_MATH_COMMON_HMAC_H_
