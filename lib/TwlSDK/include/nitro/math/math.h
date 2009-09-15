/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MATH - include
  File:     math/math.h

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

#ifndef NITRO_MATH_MATH_H_
#define NITRO_MATH_MATH_H_

#include <nitro/misc.h>
#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         MATH_ABS

  Description:  ��Βl��Ԃ��}�N���B
                �e�����͕�����]������邽�߁A����p�ɒ��ӁB
  
  Arguments:    a

  Returns:      a < 0 �Ȃ� -a, �����Ȃ��� a
 *---------------------------------------------------------------------------*/
#define MATH_ABS(a) ( ( (a) < 0 ) ? (-(a)) : (a) )

/*---------------------------------------------------------------------------*
  Name:         MATH_IAbs

  Description:  ��Βl��Ԃ��C�����C���֐��B
                �C�����C���֐��Ƃ��Ď�������Ă���̂ŁA����p�͂Ȃ��B
  
  Arguments:    a

  Returns:      a < 0 �Ȃ� -a, �����Ȃ��� a
 *---------------------------------------------------------------------------*/
SDK_INLINE int MATH_IAbs(int a)
{
    return (a < 0) ? -a : a;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_CLAMP

  Description:  low����high�͈̔͂̒l���擾����}�N���B
                �e�����͕�����]������邽�߁A����p�ɒ��ӁB
  
  Arguments:    x     
                low   �ő�l
                hight �ŏ��l
  
  Returns:      x < low �Ȃ�� low,  x > high �Ȃ�� high �����Ȃ��� x
 *---------------------------------------------------------------------------*/
#define MATH_CLAMP(x, low, high) ( ( (x) > (high) ) ? (high) : ( ( (x) < (low) ) ? (low) : (x) ) )


/*---------------------------------------------------------------------------*
  Name:         MATH_MIN

  Description:  2�̈����̂ǂ��炩�������ق���Ԃ��B
                �}�N���Ƃ��Ď�������Ă���̂ŁA�s�������Z�q����`����Ă���
                �^�Ŏg�p�\�B�e�����͕�����]������邽�߁A����p�ɒ��ӁB

  Arguments:    a, b

  Returns:      a < b �Ȃ� a, �����Ȃ��� b
 *---------------------------------------------------------------------------*/
#define MATH_MIN(a,b) (((a) <= (b)) ? (a) : (b))


/*---------------------------------------------------------------------------*
  Name:         MATH_IMin

  Description:  2�� int �^���������̂ǂ��炩�������ق���Ԃ��B
                �C�����C���֐��Ƃ��Ď�������Ă���̂ŁA����p�͂Ȃ��B

  Arguments:    a, b - int �^����

  Returns:      a <= b �Ȃ� a, �����Ȃ��� b
 *---------------------------------------------------------------------------*/
SDK_INLINE int MATH_IMin(int a, int b)
{
    return (a <= b) ? a : b;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_MAX

  Description:  2�̈����̂ǂ��炩�傫���ق���Ԃ��B
                �}�N���Ƃ��Ď�������Ă���̂ŁA�s�������Z�q����`����Ă���
                �^�Ŏg�p�\�B�e�����͕�����]������邽�߁A����p�ɒ��ӁB

  Arguments:    a, b

  Returns:      a >= b �Ȃ� a, �����Ȃ��� b
 *---------------------------------------------------------------------------*/
#define MATH_MAX(a,b) (((a) >= (b)) ? (a) : (b))

/*---------------------------------------------------------------------------*
  Name:         MATH_IMax

  Description:  2�� int �^���������̂ǂ��炩�傫���ق���Ԃ��B
                �C�����C���֐��Ƃ��Ď�������Ă���̂ŁA����p�͂Ȃ��B

  Arguments:    a, b - int �^����

  Returns:      a >= b �Ȃ� a, �����Ȃ��� b
 *---------------------------------------------------------------------------*/
SDK_INLINE int MATH_IMax(int a, int b)
{
    return (a >= b) ? a : b;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_DIVUP

  Description:  base �Ŋ���[����؂�グ��}�N���B

  Arguments:    x:      ���l
                base:   2 �ׂ̂���̊

  Returns:      x �� base �Ŋ���[����؂�グ����
 *---------------------------------------------------------------------------*/
#define MATH_DIVUP(x, base) (((x) + ((base)-1)) / (base))

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDUP

  Description:  �؂�グ�����l��Ԃ��}�N���B
  
  Arguments:    x
                base - 2 �ׂ̂���̊

  Returns:      x �ȏ�̍ŏ��� base �̔{��
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDUP(x, base) (((x) + ((base)-1)) & ~((base)-1))

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDDOWN

  Description:  �؂艺�������l��Ԃ��}�N���B
  
  Arguments:    x
                base - 2 �ׂ̂���̊

  Returns:      x �ȉ��̍ő�� base �̔{��
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDDOWN(x, base) ((x) & ~((base)-1))

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDUP32

  Description:  32�̔{���ɐ؂�グ�����l��Ԃ��}�N���B
  
  Arguments:    x

  Returns:      x �ȏ�̍ŏ���32�̔{��
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDUP32(x) MATH_ROUNDUP(x, 32)

/*---------------------------------------------------------------------------*
  Name:         MATH_ROUNDDOWN32

  Description:  32�̔{���ɐ؂艺�������l��Ԃ��}�N���B
  
  Arguments:    x

  Returns:      x �ȉ��̍ő��32�̔{��
 *---------------------------------------------------------------------------*/
#define MATH_ROUNDDOWN32(x) MATH_ROUNDDOWN(x, 32)

/*---------------------------------------------------------------------------*
  Name:         MATH_CountLeadingZeros

  Description:  2�i��32�r�b�g�\���ŏ�ʉ��r�b�g��0�������߂�B
                ARM9 �� ARM �R�[�h�ł� 1 ���߁B

  Arguments:    x

  Returns:      ��ʂ���A������0�̃r�b�g��
 *---------------------------------------------------------------------------*/
u32     MATH_CountLeadingZerosFunc(u32 x);

#if !defined(PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32)

/* clz is available ARM mode only */
#ifdef  SDK_ARM9
#if     defined(SDK_CW) || defined(SDK_RX) || defined(__MWERKS__)
#pragma thumb off
SDK_INLINE u32 MATH_CountLeadingZerosInline(u32 x)
{
    asm
    {
    clz     x, x}
    return  x;
}

#pragma thumb reset
#elif   defined(SDK_ADS)
TO BE   DEFINED
#elif   defined(SDK_GCC)
TO BE   DEFINED
#endif
#endif
#endif /* PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32 */

#ifndef MATH_CountLeadingZeros
#if       defined(PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32)
#define MATH_CountLeadingZeros(x) PLATFORM_INTRINSIC_FUNCTION_BIT_CLZ32(x)
#elif     defined(SDK_ARM9) && defined(SDK_CODE_ARM)
#define MATH_CountLeadingZeros(x) MATH_CountLeadingZerosInline(x)
#else                                  // not (ARM9 && CODE_ARM)
#define MATH_CountLeadingZeros(x) MATH_CountLeadingZerosFunc(x)
#endif                                 // ARM9 && CODE_ARM
#endif

/*---------------------------------------------------------------------------*
  Name:         MATH_CLZ

  Description:  MATH_CountLeadingZeros �̕ʖ�
                2�i��32�r�b�g�\���ŏ�ʉ��r�b�g��0�������߂�B
                ARM9 �� ARM �R�[�h�ł� 1 ���߁B

  Arguments:    x

  Returns:      ��ʂ���A������0�̃r�b�g��
 *---------------------------------------------------------------------------*/
#define MATH_CLZ(x) MATH_CountLeadingZeros(x)
/*---------------------------------------------------------------------------*
  Name:         MATH_ILog2

  Description:  u32 �̈��� x �� 2 ���Ƃ����ΐ� log2(x) �̐������������߂�B
                �������Ax == 0 �̏ꍇ�͓��ʂ� -1 ��Ԃ��B
                ARM9 �� ARM �R�[�h�ł� CLZ ���߂��g�� 2 ���߂ƂȂ�B

  Arguments:    x - u32

  Returns:      log2(x) when x > 0, or -1 when x == 0
 *---------------------------------------------------------------------------*/
        SDK_INLINE int MATH_ILog2(u32 x)
{
    return (int)(31 - MATH_CountLeadingZeros(x));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CountPopulation

  Description:  2�i��32�r�b�g�\����1�̃r�b�g�������߂�

  Arguments:    x

  Returns:      2�i�\����1�ƂȂ�r�b�g��
 *---------------------------------------------------------------------------*/
u8      MATH_CountPopulation(u32 x);


/*---------------------------------------------------------------------------*
  Name:         MATH_CountTrailingZeros

  Description:  2�i��32�r�b�g�\���ŉ��ʉ��r�b�g��0�������߂�B
                MATH_CountLeadingZeros �֐����g�p����B

  Arguments:    x             ����Ɏg�p���� u32 �̒l

  Returns:      ���ʂ���A������0�̃r�b�g��
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 MATH_CountTrailingZeros(u32 x)
{
    return (u32)(32 - MATH_CountLeadingZeros((u32)(~x & (x - 1))));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CTZ

  Description:  MATH_CountTrailingZeros �̕ʖ�
                2�i��32�r�b�g�\���ŉ��ʉ��r�b�g��0�������߂�B
                MATH_CountLeadingZeros �֐����g�p����B

  Arguments:    x             ����Ɏg�p���� u32 �̒l

  Returns:      ���ʂ���A������0�̃r�b�g��
 *---------------------------------------------------------------------------*/
#define MATH_CTZ(x) MATH_CountTrailingZeros(x)

/*---------------------------------------------------------------------------*
  Name:         MATH_GetLeastSignificantBit

  Description:  2�i��32�r�b�g�\����1�ɂȂ��Ă���ŉ��ʃr�b�g�����߂�B

  Arguments:    x             ����Ɏg�p���� u32 �̒l

  Returns:      1�ɂȂ��Ă���ŉ��ʃr�b�g������킷u32 �̒l
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 MATH_GetLeastSignificantBit(u32 x)
{
    return (u32)(x & -(s32)x);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_LSB

  Description:  MATH_GetLeastSignificantBit �̕ʖ�
                2�i��32�r�b�g�\����1�ɂȂ��Ă���ŉ��ʃr�b�g�����߂�B

  Arguments:    x             ����Ɏg�p���� u32 �̒l

  Returns:      1�ɂȂ��Ă���ŉ��ʃr�b�g������킷u32 �̒l
 *---------------------------------------------------------------------------*/
#define MATH_LSB(x) MATH_GetLeastSignificantBit(x)

/*---------------------------------------------------------------------------*
  Name:         MATH_GetMostSignificantBit

  Description:  2�i��32�r�b�g�\����1�ɂȂ��Ă���ŏ�ʃr�b�g�����߂�B

  Arguments:    x             ����Ɏg�p���� u32 �̒l

  Returns:      1�ɂȂ��Ă���ŏ�ʃr�b�g������킷u32 �̒l
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 MATH_GetMostSignificantBit(u32 x)
{
    return (u32)(x & ((s32)0x80000000 >> MATH_CountLeadingZeros(x)));
}

/*---------------------------------------------------------------------------*
  Name:         MATH_MSB

  Description:  MATH_GetMostSignificantBit �̕ʖ�
                2�i��32�r�b�g�\����1�ɂȂ��Ă���ŏ�ʃr�b�g�����߂�B

  Arguments:    x             ����Ɏg�p���� u32 �̒l

  Returns:      1�ɂȂ��Ă���ŏ�ʃr�b�g������킷u32 �̒l
 *---------------------------------------------------------------------------*/
#define MATH_MSB(x) MATH_GetMostSignificantBit(x)


#ifdef __cplusplus
}/* extern "C" */
#endif

/* NITRO_MATH_MATH_H_ */
#endif
