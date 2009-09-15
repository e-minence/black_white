/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MATH - 
  File:     checksum.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-07#$
  $Rev: 9262 $
  $Author:$
 *---------------------------------------------------------------------------*/

#include <nitro/math/checksum.h>

/*---------------------------------------------------------------------------*
  Name:         MATHi_Checksum16Update

  Description:  16bit �� Checksum �l�����߂�B

  Arguments:    input   ���̓f�[�^�ւ̃|�C���^
                length  ���̓f�[�^��

  Returns:      �v�Z����.
 *---------------------------------------------------------------------------*/
static inline u32 MATHi_Checksum16Update(const void *input, u32 length)
{
    u32     sum;
    u32     n;
    BOOL    fSwap = FALSE;

    sum = 0;

    if (((u32)input) & 1)
    {
        sum += (*((u8 *)input) << 8); // BIGENDIAN
        input = (u8 *)input + 1;
        length--;
        fSwap = TRUE;
    }

    while ((length >> 17) > 0)
    {
        length -= (1 << 17);
        for (n = (1 << 16); n > 0; n--)
        {
            sum += *(u16 *)input;
            input = (u16 *)input + 1;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum = (u16)((sum >> 16) + sum);
    }

    for (n = (length >> 1); n > 0; n--)
    {
        sum += *(u16 *)input;
        input = (u16 *)input + 1;
    }

    if (length & 1)
    {
        sum += *((u8 *)input);
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = (sum >> 16) + (sum & 0xffff);

    if (fSwap)
    {
        sum = ((sum << 24) | (sum << 8)) >> 16;
    }

    return sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum16Update

  Description:  16bit �� Checksum �l��^�����f�[�^�ōX�V����B

  Arguments:    context MATHChecksum16Context �\����
                input   ���̓f�[�^�ւ̃|�C���^
                length  ���̓f�[�^��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_Checksum16Update(MATHChecksum16Context * context, const void *input, u32 length)
{
    u32     sum;

    sum = MATHi_Checksum16Update(input, length);

    sum += *context;
    sum += (sum >> 16);

    *context = (u16)sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcChecksum16

  Description:  16bit �� Checksum ���v�Z����B
  
  Arguments:    data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
  
  Returns:      Checksum �l.
 *---------------------------------------------------------------------------*/
u16 MATH_CalcChecksum16(const void *data, u32 dataLength)
{
    u32     sum;

    sum = MATHi_Checksum16Update(data, dataLength);

    return (u16)(~sum);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8Update

  Description:  8bit �� Checksum �l�����߂�B

  Arguments:    input   ���̓f�[�^�ւ̃|�C���^
                length  ���̓f�[�^��

  Returns:      �v�Z����.
 *---------------------------------------------------------------------------*/
static inline u32 MATHi_Checksum8Update(const void *input, u32 length)
{
    u32     sum;
    u32     n;

    sum = 0;

    if (((u32)input) & 1)
    {
        sum += *(u8 *)input;
        input = (u8 *)input + 1;
        length--;
    }

    while ((length >> 17) > 0)
    {
        length -= (1 << 17);
        for (n = (1 << 16); n > 0; n--)
        {
            sum += *(u16 *)input;
            input = (u16 *)input + 1;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum = (u16)(sum + (sum >> 16));
    }

    for (n = (length >> 1); n > 0; n--)
    {
        sum += *(u16 *)input;
        input = (u16 *)input + 1;
    }

    if (length & 1)
    {
        sum += *((u8 *)input);
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (u16)sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8Update

  Description:  8bit �� Checksum �l��^�����f�[�^�ōX�V����B

  Arguments:    context MATHChecksum8Context �\����
                input   ���̓f�[�^�ւ̃|�C���^
                length  ���̓f�[�^��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_Checksum8Update(MATHChecksum8Context * context, const void *input, u32 length)
{
    u32     sum;

    sum = MATHi_Checksum8Update(input, length);

    sum += *context;
    sum += (sum >> 16);

    *context = (u16)sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcChecksum8

  Description:  16bit �� Checksum ���v�Z����B
  
  Arguments:    data    ���̓f�[�^�̃|�C���^
                dataLength  ���̓f�[�^��
  
  Returns:      Checksum �l.
 *---------------------------------------------------------------------------*/
u8 MATH_CalcChecksum8(const void *data, u32 dataLength)
{
    u32     sum;

    sum = MATHi_Checksum8Update(data, dataLength);

    sum = (sum >> 8) + (sum & 0xff);
    sum += (sum >> 8);

    return (u8)(~sum);
}
