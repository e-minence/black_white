/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - dsp - common
  File:     byteaccess.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-03#$
  $Rev: 10855 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/
#ifndef TWL_DSP_BYTEACCESS_H_
#define TWL_DSP_BYTEACCESS_H_


#ifdef SDK_TWL
#include <twl/types.h>
#include <twl/os.h>
#else
#include <dsp/types.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

// �v���Z�b�T�ԒʐM�̃f�[�^���L�Ɏg�p����^��`�B
typedef u16 DSPAddr;        // DSP���ŃA�h���X��\������^ (2�o�C�g1���[�h)
typedef u16 DSPWord;        // DSP���ŃT�C�Y��\������^ (2�o�C�g1���[�h)
typedef u16 DSPByte;        // DSP���Ńo�C�g�P�ʂ�\������^ (1�o�C�g1���[�h)
typedef u32 DSPWord32;      // DSP���ŃT�C�Y��\������^ (2�o�C�g1���[�h)
typedef u32 DSPByte32;      // DSP���Ńo�C�g�P�ʂ�\������^ (1�o�C�g1���[�h)
typedef u32 DSPAddrInARM;   // DSP���A�h���X���o�C�g�P�ʂɕϊ������^

// �����I�Ȍ^�ϊ��}�N���B
#define DSP_ADDR_TO_ARM(address)    (u32)((address) << 1)
#define DSP_ADDR_TO_DSP(address)    (u16)((u32)(address) >> 1)
#define DSP_WORD_TO_ARM(word)       (u16)((word) << 1)
#define DSP_WORD_TO_DSP(word)       (u16)((word) >> 1)
#define DSP_WORD_TO_ARM32(word)     (u32)((word) << 1)
#define DSP_WORD_TO_DSP32(word)     (u32)((word) >> 1)
#define DSP_32BIT_TO_ARM(value)     (u32)(((u32)(value) >> 16) | ((u32)(value) << 16))
#define DSP_32BIT_TO_DSP(value)     (u32)(((u32)(value) >> 16) | ((u32)(value) << 16))
#ifdef SDK_TWL
#define DSP_BYTE_TO_UNIT(byte)      (u16)(byte)
#define DSP_UNIT_TO_BYTE(unit)      (u16)(unit)
#else
#define DSP_BYTE_TO_UNIT(byte)      (u16)((byte) >> 1)
#define DSP_UNIT_TO_BYTE(unit)      (u16)((unit) << 1)
#endif

// �l�C�e�B�u��sizeof(char)�̃T�C�Y�B (DSP���ł�2, ARM���ł�1)
#define DSP_WORD_UNIT       (3 - sizeof(DSPWord))


/*---------------------------------------------------------------------------*/
/* functions */

#ifdef SDK_TWL

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadWord

  Description:  DSP�f�[�^��������Ԃ���1���[�h(�����P�ʂ�16�r�b�g)�ǂݏo���B

  Arguments:    offset : �]����DSP�A�h���X (���[�h�P��)

  Returns:      �ǂݏo����16�r�b�g�f�[�^�B
 *---------------------------------------------------------------------------*/
SDK_INLINE u16 DSP_LoadWord(DSPAddr offset)
{
    u16     value;
    OSIntrMode  cpsr = OS_DisableInterrupts();
    DSP_RecvFifo(DSP_FIFO_MEMSEL_DATA, &value, offset, DSP_WORD_TO_DSP(sizeof(u16)));
    reg_DSP_PCFG &= ~(REG_DSP_PCFG_DRS_MASK | REG_DSP_PCFG_AIM_MASK);
    (void)OS_RestoreInterrupts(cpsr);
    return value;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_StoreWord

  Description:  DSP�f�[�^��������Ԃ�1���[�h(�����P�ʂ�16�r�b�g)�ǂݏo���������݁B

  Arguments:    offset : �]����DSP�A�h���X (���[�h�P��)
                value  : �������ރ��[�h�l

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_StoreWord(DSPAddr offset, u16 value)
{
    OSIntrMode  cpsr = OS_DisableInterrupts();
    DSP_SendFifo(DSP_FIFO_MEMSEL_DATA, offset, &value, DSP_WORD_TO_DSP(sizeof(u16)));
//    reg_DSP_PCFG &= ~(REG_DSP_PCFG_DRS_MASK | REG_DSP_PCFG_AIM_MASK);
    (void)OS_RestoreInterrupts(cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_Load8

  Description:  DSP�f�[�^��������Ԃ���8�r�b�g�ǂݏo���B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)

  Returns:      �ǂݏo����8�r�b�g�f�[�^�B
 *---------------------------------------------------------------------------*/
SDK_INLINE u8 DSP_Load8(DSPAddrInARM offset)
{
    return (u8)(DSP_LoadWord(DSP_WORD_TO_DSP(offset)) >> ((offset & 1) << 3));
}

/*---------------------------------------------------------------------------*
  Name:         DSP_Load16

  Description:  DSP�f�[�^��������Ԃ���16�r�b�g�ǂݏo���B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)

  Returns:      �ǂݏo����16�r�b�g�f�[�^�B
 *---------------------------------------------------------------------------*/
u16     DSP_Load16(DSPAddrInARM offset);

/*---------------------------------------------------------------------------*
  Name:         DSP_Load32

  Description:  DSP�f�[�^��������Ԃ���32�r�b�g�ǂݏo���B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)

  Returns:      �ǂݏo����32�r�b�g�f�[�^�B
 *---------------------------------------------------------------------------*/
u32     DSP_Load32(DSPAddrInARM offset);

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadData

  Description:  DSP�f�[�^��������Ԃ���C�Ӓ��̃f�[�^��ǂݏo���B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)
                buffer : �]����o�b�t�@
                length : �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    DSP_LoadData(DSPAddrInARM offset, void *buffer, u32 length);

/*---------------------------------------------------------------------------*
  Name:         DSP_Store8

  Description:  DSP�f�[�^��������Ԃ�8�r�b�g�������݁B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)
                value  : ��������8�r�b�g�l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    DSP_Store8(DSPAddrInARM offset, u8 value);

/*---------------------------------------------------------------------------*
  Name:         DSP_Store16

  Description:  DSP�f�[�^��������Ԃ�16�r�b�g�������݁B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)
                value  : ��������16�r�b�g�l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    DSP_Store16(DSPAddrInARM offset, u16 value);

/*---------------------------------------------------------------------------*
  Name:         DSP_Store32

  Description:  DSP�f�[�^��������Ԃ�32�r�b�g�������݁B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)
                value  : ��������32�r�b�g�l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    DSP_Store32(DSPAddrInARM offset, u32 value);

/*---------------------------------------------------------------------------*
  Name:         DSP_StoreData

  Description:  DSP�f�[�^��������Ԃ֔C�Ӓ��̃f�[�^���������݁B

  Arguments:    offset : �]����DSP�A�h���X (�o�C�g�P��)
                buffer : �]�����o�b�t�@
                length : �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    DSP_StoreData(DSPAddrInARM offset, const void *buffer, u32 length);


#endif // SDK_TWL


/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_BYTEACCESS_H_ */
