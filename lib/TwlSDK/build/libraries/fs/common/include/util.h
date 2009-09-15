/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     util.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-25#$
  $Rev: 8648 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_UTIL_H_)
#define NITRO_FS_UTIL_H_

#include <nitro/misc.h>
#include <nitro/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// ARM7 ����t�@�C���V�X�e�����g�p�������ꍇ�͂��̒�`��L���ɂ���
// #define SDK_ARM7FS

// �t�@�C���V�X�e���̃t���Z�b�g���܂ނׂ����̂ݒ�`�����
#if	!defined(SDK_ARM7) || defined(SDK_ARM7FS)
#define	FS_IMPLEMENT
#endif


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FSi_IsSlash

  Description:  �����֐�.
                �f�B���N�g����؂蕶��������.

  Arguments:    c                ���肷�镶��.

  Returns:      �f�B���N�g����؂蕶���Ȃ� TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FSi_IsSlash(u32 c)
{
    return (c == '/') || (c == '\\');
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IncrementSjisPosition

  Description:  Shift_JIS������̎Q�ƈʒu��1�������i�߂�B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos��1�����i�߂��Q�ƈʒu�B
 *---------------------------------------------------------------------------*/
SDK_INLINE int FSi_IncrementSjisPosition(const char *str, int pos)
{
    return pos + 1 + STD_IsSjisLeadByte(str[pos]);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementSjisPosition

  Description:  Shift_JIS������̎Q�ƈʒu��1�������߂��B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos��1�����߂����Q�ƈʒu��-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementSjisPosition(const char *str, int pos);

/*---------------------------------------------------------------------------*
  Name:         FSi_IncrementSjisPositionToSlash

  Description:  Shift_JIS������̎Q�ƈʒu��
                �f�B���N�g����؂蕶�����I�[�܂Ői�߂�B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos�ȍ~�Ɍ����ŏ��̃f�B���N�g����؂肩�I�[�̈ʒu�B
 *---------------------------------------------------------------------------*/
int FSi_IncrementSjisPositionToSlash(const char *str, int pos);

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementSjisPositionToSlash

  Description:  Shift_JIS������̎Q�ƈʒu��
                �f�B���N�g����؂蕶�����擪�܂Ŗ߂��B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos�����Ɍ����ŏ��̃f�B���N�g����؂肩-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementSjisPositionToSlash(const char *str, int pos);

/*---------------------------------------------------------------------------*
  Name:         FSi_TrimSjisTrailingSlash

  Description:  Shift_JIS������̏I�[���f�B���N�g����؂蕶���ł���Ώ����B

  Arguments:    str              Shift_JIS������B

  Returns:      �����񒷁B
 *---------------------------------------------------------------------------*/
int FSi_TrimSjisTrailingSlash(char *str);

/*---------------------------------------------------------------------------*
  Name:         FSi_StrNICmp

  Description:  �召��������ʂ����Ɏw��o�C�g�������������r�B
                �I�[��NUL���l�����Ȃ��_�ɒ��ӁB

  Arguments:    str1             ��r��������
                str2             ��r�敶����
                len              ��r�o�C�g��

  Returns:      (str1 - str2) �̔�r����
 *---------------------------------------------------------------------------*/
SDK_INLINE int FSi_StrNICmp(const char *str1, const char *str2, u32 len)
{
    int     retval = 0;
    int     i;
    for (i = 0; i < len; ++i)
    {
        u32     c = (u8)(str1[i] - 'A');
        u32     d = (u8)(str2[i] - 'A');
        if (c <= 'Z' - 'A')
        {
            c += 'a' - 'A';
        }
        if (d <= 'Z' - 'A')
        {
            d += 'a' - 'A';
        }
        retval = (int)(c - d);
        if (retval != 0)
        {
            break;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsUnicodeSlash

  Description:  Unicode�̃p�X��؂蕶��������B

  Arguments:    c             Unicode1����
                
  Returns:      L'/'(0x2F)��L'\\'(0x5C)�Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FSi_IsUnicodeSlash(u16 c)
{
    return (c == L'/') || (c == L'\\');
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementUnicodePosition

  Description:  Unicode������̎Q�ƈʒu��1�������߂��B

  Arguments:    str              Unicode������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos��1�����߂����Q�ƈʒu��-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementUnicodePosition(const u16 *str, int pos);

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementUnicodePositionToSlash

  Description:  Unicode������̎Q�ƈʒu��
                �f�B���N�g����؂蕶�����擪�܂Ŗ߂��B

  Arguments:    str              Unicode������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos�����Ɍ����ŏ��̃f�B���N�g����؂肩-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementUnicodePositionToSlash(const u16 *str, int pos);

/*---------------------------------------------------------------------------*
  Name:         FSi_WaitConditionOn

  Description:  ����̃r�b�g��1�ɂȂ�܂ŃX���[�v

  Arguments:    flags            �Ď�����r�b�g�W��
                bits             1�ɂȂ�ׂ��r�b�g
                queue            �X���[�v�p�̃L���[�܂���NULL

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void FSi_WaitConditionOn(u32 *flags, u32 bits, OSThreadQueue *queue)
{
    OSIntrMode bak = OS_DisableInterrupts();
    while ((*flags & bits) == 0)
    {
        OS_SleepThread(queue);
    }
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WaitConditionOff

  Description:  ����̃r�b�g��0�ɂȂ�܂ŃX���[�v

  Arguments:    flags            �Ď�����r�b�g�W��
                bits             0�ɂȂ�ׂ��r�b�g
                queue            �X���[�v�p�̃L���[�܂���NULL

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void FSi_WaitConditionOff(u32 *flags, u32 bits, OSThreadQueue *queue)
{
    OSIntrMode bak = OS_DisableInterrupts();
    while ((*flags & bits) != 0)
    {
        OS_SleepThread(queue);
    }
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetFileLengthIfProc

  Description:  �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���΃T�C�Y���擾

  Arguments:    file             �t�@�C���n���h��
                length           �T�C�Y�̊i�[��

  Returns:      �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���΂��̃T�C�Y
 *---------------------------------------------------------------------------*/
BOOL FSi_GetFileLengthIfProc(FSFile *file, u32 *length);

/*---------------------------------------------------------------------------*
  Name:         FSi_GetFilePositionIfProc

  Description:  �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���Ό��݈ʒu���擾

  Arguments:    file             �t�@�C���n���h��
                length           �T�C�Y�̊i�[��

  Returns:      �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���΂��̌��݈ʒu
 *---------------------------------------------------------------------------*/
BOOL FSi_GetFilePositionIfProc(FSFile *file, u32 *length);


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_UTIL_H_ */
