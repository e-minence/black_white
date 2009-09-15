/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - dsp
  File:     dsp_if.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef TWL_DSP_IF_H_
#define TWL_DSP_IF_H_

#include <twl/types.h>
#include <twl/hw/ARM9/ioreg_DSP.h>
#include <nitro/os/common/emulator.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
typedef enum
{
    DSP_FIFO_MEMSEL_DATA    = (0x0 << REG_DSP_PCFG_MEMSEL_SHIFT),
    DSP_FIFO_MEMSEL_MMIO    = (0x1 << REG_DSP_PCFG_MEMSEL_SHIFT),
    DSP_FIFO_MEMSEL_PROGRAM = (0x5 << REG_DSP_PCFG_MEMSEL_SHIFT)
}
DSPFifoMemSel;

typedef enum
{
    DSP_FIFO_RECV_2B            = (0x0 << REG_DSP_PCFG_DRS_SHIFT),
    DSP_FIFO_RECV_16B           = (0x1 << REG_DSP_PCFG_DRS_SHIFT),
    DSP_FIFO_RECV_32B           = (0x2 << REG_DSP_PCFG_DRS_SHIFT),
    DSP_FIFO_RECV_CONTINUOUS    = (0x3 << REG_DSP_PCFG_DRS_SHIFT)
}
DSPFifoRecvLength;

typedef enum
{
    DSP_FIFO_INTR_SEND_EMPTY        = REG_DSP_PCFG_WFEIE_MASK,
    DSP_FIFO_INTR_SEND_FULL         = REG_DSP_PCFG_WFFIE_MASK,
    DSP_FIFO_INTR_RECV_NOT_EMPTY    = REG_DSP_PCFG_RFNEIE_MASK,
    DSP_FIFO_INTR_RECV_FULL         = REG_DSP_PCFG_RFFIE_MASK
}
DSPFifoIntr;

// for complex API
typedef enum
{
    DSP_FIFO_FLAG_SRC_INC   = (0UL << 0),
    DSP_FIFO_FLAG_SRC_FIX   = (1UL << 0),

    DSP_FIFO_FLAG_DEST_INC  = (0UL << 1),
    DSP_FIFO_FLAG_DEST_FIX  = (1UL << 1),

    DSP_FIFO_FLAG_RECV_UNIT_CONTINUOUS  = (0UL << 8),
    DSP_FIFO_FLAG_RECV_UNIT_2B          = (1UL << 8),
    DSP_FIFO_FLAG_RECV_UNIT_16B         = (2UL << 8),
    DSP_FIFO_FLAG_RECV_UNIT_32B         = (3UL << 8),
    DSP_FIFO_FLAG_RECV_MASK             = (3UL << 8)
}
DSPFifoFlag;

// ���d�l�̃��[�h���������΂炭�̊ԃT�|�[�g����B
#define DSP_SUPPORT_OBSOLETE_LOADER

/*---------------------------------------------------------------------------*
    �\���̒�`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSP_PowerOn

  Description:  DSP�̓d�����I���ɂ��āA���Z�b�g��Ԃɂ��܂��B
                DSP���N��������ɂ� DSP_ResetOff() ���Ăяo���K�v������܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_PowerOnCore(void);
SDK_INLINE void DSP_PowerOn(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_PowerOnCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_PowerOff

  Description:  DSP�̓d�����I�t�ɂ��܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_PowerOffCore(void);
SDK_INLINE void DSP_PowerOff(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_PowerOffCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetOn

  Description:  DSP�����Z�b�g���܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetOnCore(void);
SDK_INLINE void DSP_ResetOn(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_ResetOnCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetOff

  Description:  ���Z�b�g��Ԃ���DSP���N�����܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetOffCore(void);
SDK_INLINE void DSP_ResetOff(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_ResetOffCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetOffEx

  Description:  ���Z�b�g��Ԃ���DSP���N�����܂��B

  Arguments:    bitmap : DSP����ARM9�ւ̊��荞�݂�������Z�}�t�H�A���v���C���W�X�^�̃r�b�g�}�b�v�B

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetOffExCore(u16 bitmap);
SDK_INLINE void DSP_ResetOffEx(u16 bitmap)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_ResetOffExCore(bitmap);
    }
}


/*---------------------------------------------------------------------------*
  Name:         DSP_ResetInterface

  Description:  ���W�X�^�̏��������s���܂��B
                DSP�����Z�b�g�����ꍇ�ɌĂяo���K�v������܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetInterfaceCore(void);
SDK_INLINE void DSP_ResetInterface(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_ResetInterfaceCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_EnableRecvDataInterrupt

  Description:  �w�肵�����v���C���W�X�^�̊��荞�݂�L���ɂ��܂��B

  Arguments:    dataNo : ���v���C���W�X�^�ԍ� (0-2)

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_EnableRecvDataInterruptCore(u32 dataNo);
SDK_INLINE void DSP_EnableRecvDataInterrupt(u32 dataNo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_EnableRecvDataInterruptCore(dataNo);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_DisableRecvDataInterrupt

  Description:  �w�肵�����v���C���W�X�^�̊��荞�݂𖳌��ɂ��܂��B

  Arguments:    dataNo : ���v���C���W�X�^�̔ԍ� (0-2)

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_DisableRecvDataInterruptCore(u32 dataNo);
SDK_INLINE void DSP_DisableRecvDataInterrupt(u32 dataNo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_DisableRecvDataInterruptCore(dataNo);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SendDataIsEmpty

  Description:  �w�肵���R�}���h���W�X�^�̃f�[�^��DSP����M�ς݂��`�F�b�N���܂��B

  Arguments:    dataNo : �R�}���h���W�X�^�̔ԍ� (0-2)

  Returns:      DSP�����Ƀf�[�^����M���Ă���� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_SendDataIsEmptyCore(u32 dataNo);
SDK_INLINE BOOL DSP_SendDataIsEmpty(u32 dataNo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_SendDataIsEmptyCore(dataNo);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_RecvDataIsReady

  Description:  �w�肵�����v���C���W�X�^��DSP����f�[�^�����M���ꂽ���`�F�b�N���܂��B

  Arguments:    dataNo : ���v���C���W�X�^�̔ԍ� (0-2)

  Returns:      DSP���f�[�^�𑗐M���Ă���� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_RecvDataIsReadyCore(u32 dataNo);
SDK_INLINE BOOL DSP_RecvDataIsReady(u32 dataNo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_RecvDataIsReadyCore(dataNo);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SendData

  Description:  DSP�Ƀf�[�^�𑗐M���܂��B

  Arguments:    dataNo : �R�}���h���W�X�^�̔ԍ� (0-2)
                data   : ���M����f�[�^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SendDataCore(u32 dataNo, u16 data);
SDK_INLINE void DSP_SendData(u32 dataNo, u16 data)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_SendDataCore(dataNo, data);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_RecvData

  Description:  DSP����̃f�[�^����M���܂��B

  Arguments:    dataNo : ���v���C���W�X�^�̔ԍ� (0-2)

  Returns:      ��M�����f�[�^
 *---------------------------------------------------------------------------*/
u16 DSP_RecvDataCore(u32 dataNo);
SDK_INLINE u16 DSP_RecvData(u32 dataNo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_RecvDataCore(dataNo);
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_EnableFifoInterrupt

  Description:  FIFO�̊��荞�݂�L���ɂ��܂��B

  Arguments:    type : FIFO�̊��荞�ݗv��

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_EnableFifoInterruptCore(DSPFifoIntr type);
SDK_INLINE void DSP_EnableFifoInterrupt(DSPFifoIntr type)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_EnableFifoInterruptCore(type);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_DisableFifoInterrupt

  Description:  FIFO�̊��荞�݂𖳌��ɂ��܂��B

  Arguments:    type : FIFO�̊��荞�ݗv��

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_DisableFifoInterruptCore(DSPFifoIntr type);
SDK_INLINE void DSP_DisableFifoInterrupt(DSPFifoIntr type)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_DisableFifoInterruptCore(type);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SendFifoEx

  Description:  DSP�̃�������ԓ��Ƀf�[�^���������݂܂��B

  Arguments:    memsel : �f�[�^���������ރ��������
                dest   : �������ݐ�A�h���X (�n�[�t���[�h).
                         ��� 16 �r�b�g��ݒ肷��ꍇ�́A���O�� DMA ���W�X�^��
                         �l��ݒ肷��K�v������܂��B
                src    : �������ރf�[�^
                size   : �������ރf�[�^�̒��� (�n�[�t���[�h)
                flags  : �ǂݏo�����[�h�� DSPFifoFlag ��
                         DSP_FIFO_FLAG_RECV_UNIT_* �ȊO����I�����܂��B

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SendFifoExCore(DSPFifoMemSel memsel, u16 dest, const u16 *src, int size, u16 flags);
SDK_INLINE void DSP_SendFifoEx(DSPFifoMemSel memsel, u16 dest, const u16 *src, int size, u16 flags)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_SendFifoExCore(memsel, dest, src, size, flags);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SendFifo

  Description:  DSP�̃�������ԓ��Ƀf�[�^���������݂܂��B

  Arguments:    memsel : �f�[�^���������ރ��������
                dest   : �������ݐ�A�h���X (�n�[�t���[�h).
                         ��� 16 �r�b�g��ݒ肷��ꍇ�́A���O�� DMA ���W�X�^��
                         �l��ݒ肷��K�v������܂��B
                src    : �������ރf�[�^
                size   : �������ރf�[�^�̒��� (�n�[�t���[�h)

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static inline void DSP_SendFifo(DSPFifoMemSel memsel, u16 dest, const u16 *src, int size)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_SendFifoExCore(memsel, dest, src, size, 0);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_RecvFifoEx

  Description:  DSP�̃�������ԓ�����f�[�^��ǂݍ��݂܂��B

  Arguments:    memsel : �f�[�^��ǂݍ��ރ�������� (�v���O�����������ȊO)
                dest   : ��M��A�h���X
                src    : ��M���A�h���X (�n�[�t���[�h)
                         ��� 16 �r�b�g��ݒ肷��ꍇ�́A���O�� DMA ���W�X�^��
                         �l��ݒ肷��K�v������܂��B
                size   : �ǂݍ��ރf�[�^�̃T�C�Y (�n�[�t���[�h)
                flags  : �������݃��[�h�� DSPFifoFlag ����I�����܂��B

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_RecvFifoExCore(DSPFifoMemSel memsel, u16 *dest, u16 src, int size, u16 flags);
SDK_INLINE void DSP_RecvFifoEx(DSPFifoMemSel memsel, u16 *dest, u16 src, int size, u16 flags)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_RecvFifoExCore(memsel, dest, src, size, flags);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_RecvFifo

  Description:  DSP�̃�������ԓ�����f�[�^��ǂݍ��݂܂��B

  Arguments:    memsel : �f�[�^��ǂݍ��ރ�������� (�v���O�����������ȊO)
                dest   : ��M��A�h���X
                src    : ��M���A�h���X (�n�[�t���[�h)
                         ��� 16 �r�b�g��ݒ肷��ꍇ�́A���O�� DMA ���W�X�^��
                         �l��ݒ肷��K�v������܂��B
                size   : �ǂݍ��ރf�[�^�̃T�C�Y (�n�[�t���[�h)

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static inline void DSP_RecvFifo(DSPFifoMemSel memsel, u16* dest, u16 src, int size)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_RecvFifoExCore(memsel, dest, src, size, 0);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SetCommandReg

  Description:  �R�}���h���W�X�^�ɒl���������݂܂��B

  Arguments:    regNo : �R�}���h���W�X�^�ԍ� (0-2)
                data  : �f�[�^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SetCommandRegCore(u32 regNo, u16 data);
SDK_INLINE void DSP_SetCommandReg(u32 regNo, u16 data)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_SetCommandRegCore(regNo, data);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetReplyReg

  Description:  ���v���C���W�X�^����l��ǂݍ��݂܂��B

  Arguments:    regNo : ���v���C���W�X�^�ԍ� (0-2)

  Returns:      �ǂݍ��񂾃f�[�^
 *---------------------------------------------------------------------------*/
u16 DSP_GetReplyRegCore(u32 regNo);
SDK_INLINE u16 DSP_GetReplyReg(u32 regNo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_GetReplyRegCore(regNo);
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SetSemaphore

  Description:  ARM����DSP�ւ̒ʒm�p�Z�}�t�H���W�X�^�ɒl���������݂܂��B

  Arguments:    mask : �Z�b�g����l

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SetSemaphoreCore(u16 mask);
SDK_INLINE void DSP_SetSemaphore(u16 mask)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_SetSemaphoreCore(mask);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetSemaphore

  Description:  DSP����ARM�ւ̒ʒm�p�Z�}�t�H���W�X�^�̒l��ǂݍ��݂܂��B

  Arguments:    �Ȃ�

  Returns:      DSP���Z�}�t�H�ɏ������񂾒l
 *---------------------------------------------------------------------------*/
u16 DSP_GetSemaphoreCore(void);
SDK_INLINE u16 DSP_GetSemaphore(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_GetSemaphoreCore();
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ClearSemaphore

  Description:  DSP����ARM�ւ̒ʒm�p�Z�}�t�H���W�X�^�̒l���N���A���܂��B

  Arguments:    mask : �N���A����r�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ClearSemaphoreCore(u16 mask);
SDK_INLINE void DSP_ClearSemaphore(u16 mask)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_ClearSemaphoreCore(mask);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_MaskSemaphore

  Description:  ���荞�݂𖳌��ɂ���DSP����ARM�ւ̒ʒm�p�Z�}�t�H���W�X�^�̃r�b�g��ݒ肵�܂��B

  Arguments:    mask : ���荞�݂𖳌��ɂ���r�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_MaskSemaphoreCore(u16 mask);
SDK_INLINE void DSP_MaskSemaphore(u16 mask)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        DSP_MaskSemaphoreCore(mask);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_CheckSemaphoreRequest

  Description:  �Z�}�t�H���W�X�^�ɂ�銄�荞�ݗv�������邩�`�F�b�N���܂��B

  Arguments:    �Ȃ�

  Returns:      �v��������ꍇ�� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_CheckSemaphoreRequestCore(void);
SDK_INLINE BOOL DSP_CheckSemaphoreRequest(void)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_CheckSemaphoreRequestCore();
    }
    return FALSE;
}

#if defined(DSP_SUPPORT_OBSOLETE_LOADER)
/*---------------------------------------------------------------------------*
 * �ȉ��͌��ݎg�p����Ă��Ȃ��Ǝv����p�~���C���^�t�F�[�X�B
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadFileAuto

  Description:  COFF�`����DSP�v���O���������[�h���ĕK�v��WRAM��DSP�Ɋ��蓖�Ă�

  Arguments:    image : COFF�t�@�C��

  Returns:      �����Ȃ�� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_LoadFileAutoCore(const void *image);
SDK_INLINE BOOL DSP_LoadFileAuto(const void *image)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return DSP_LoadFileAutoCore(image);
    }
    return FALSE;
}

#endif


/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_IF_H_ */
