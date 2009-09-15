/*---------------------------------------------------------------------------*
  Project:  TwlSDK - library - dsp
  File:     dsp_if.c

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
#include <twl.h>
#include <twl/dsp.h>

#include <dsp_coff.h>
#include "dsp_process.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

#define reg_CFG_DSP_RST             *(vu8*)REG_RST_ADDR

#define REG_DSP_PCFG_RRIE_MASK      (REG_DSP_PCFG_PRIE0_MASK | REG_DSP_PCFG_PRIE1_MASK | REG_DSP_PCFG_PRIE2_MASK)
#define REG_DSP_PCFG_RRIE_SHIFT     REG_DSP_PCFG_PRIE0_SHIFT
#define REG_DSP_PSTS_RCOMIM_SHIFT   REG_DSP_PSTS_RCOMIM0_SHIFT
#define REG_DSP_PSTS_RRI_SHIFT      REG_DSP_PSTS_RRI0_SHIFT


#define DSP_DPRINTF(...)    ((void) 0)
//#define DSP_DPRINTF OS_TPrintf

/*---------------------------------------------------------------------------*
    �^��`
 *---------------------------------------------------------------------------*/
typedef struct DSPData
{
    u16 send;
    u16 reserve1;
    u16 recv;
    u16 reserve2;
}
DSPData;


/*---------------------------------------------------------------------------*
    �ÓI�ϐ���`
 *---------------------------------------------------------------------------*/
static volatile DSPData *const dspData = (DSPData*)REG_COM0_ADDR;

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSP_PowerOn

  Description:  DSP�̓d�����I���ɂ��āA���Z�b�g��Ԃɂ��܂��B
                DSP���N��������ɂ� DSP_ResetOff() ���Ăяo���K�v������܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_PowerOnCore(void)  // DSP_Init
{
    SCFG_ResetDSP();                                // DSP�u���b�N�̃��Z�b�g�m�F
    SCFG_SupplyClockToDSP(TRUE);                    // DSP�u���b�N�̓d��On
    OS_SpinWaitSysCycles(2);                        // wait 8 cycle @ 134MHz
    SCFG_ReleaseResetDSP();                         // DSP�u���b�N�̃��Z�b�g����
    DSP_ResetOnCore();                                  // DSP�R�A�̃��Z�b�g�ݒ�
}

/*---------------------------------------------------------------------------*
  Name:         DSP_PowerOff

  Description:  DSP�̓d�����I�t�ɂ��܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_PowerOffCore(void) // DSP_End
{
    SCFG_ResetDSP();                                // DSP�u���b�N�̃��Z�b�g�ݒ�
    SCFG_SupplyClockToDSP(FALSE);                   // DSP�u���b�N�̓d��Off
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetOn

  Description:  DSP�����Z�b�g���܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetOnCore(void)
{
    if ((reg_DSP_PCFG & REG_DSP_PCFG_DSPR_MASK) == 0)
    {
        reg_DSP_PCFG |= REG_DSP_PCFG_DSPR_MASK;
        while ( reg_DSP_PSTS & REG_DSP_PSTS_PRST_MASK )
        {
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetOff

  Description:  ���Z�b�g��Ԃ���DSP���N�����܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetOffCore(void)
{
    while ( reg_DSP_PSTS & REG_DSP_PSTS_PRST_MASK )
    {
    }
    DSP_ResetInterfaceCore();   // DSP-A9IF�̏�����
    reg_DSP_PCFG &= ~REG_DSP_PCFG_DSPR_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetOffEx

  Description:  ���Z�b�g��Ԃ���DSP���N�����܂��B

  Arguments:    bitmap : DSP����ARM9�ւ̊��荞�݂�������Z�}�t�H�A���v���C���W�X�^�̃r�b�g�}�b�v�B

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetOffExCore(u16 bitmap)
{
    SDK_ASSERT(bitmap >= 0 && bitmap <= 7);
    
    while ( reg_DSP_PSTS & REG_DSP_PSTS_PRST_MASK )
    {
    }
    DSP_ResetInterfaceCore();   // DSP-A9IF�̏�����
    reg_DSP_PCFG |= (bitmap) << REG_DSP_PCFG_RRIE_SHIFT;
    reg_DSP_PCFG &= ~REG_DSP_PCFG_DSPR_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ResetInterface

  Description:  ���W�X�^�̏��������s���܂��B
                DSP�����Z�b�g�����ꍇ�ɌĂяo���K�v������܂��B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ResetInterfaceCore(void)
{
    if (reg_DSP_PCFG & REG_DSP_PCFG_DSPR_MASK)
    {
        u16 dummy;
        reg_DSP_PCFG &= ~REG_DSP_PCFG_RRIE_MASK;
        reg_DSP_PSEM = 0;
        reg_DSP_PCLEAR = 0xFFFF;
        dummy = dspData[0].recv;
        dummy = dspData[1].recv;
        dummy = dspData[2].recv;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_EnableRecvDataInterrupt

  Description:  �w�肵�����v���C���W�X�^�̊��荞�݂�L���ɂ��܂��B

  Arguments:    dataNo : ���v���C���W�X�^�ԍ� (0-2)

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_EnableRecvDataInterruptCore(u32 dataNo)
{
    SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
    reg_DSP_PCFG |= (1 << dataNo) << REG_DSP_PCFG_RRIE_SHIFT;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_DisableRecvDataInterrupt

  Description:  �w�肵�����v���C���W�X�^�̊��荞�݂𖳌��ɂ��܂��B

  Arguments:    dataNo : ���v���C���W�X�^�̔ԍ� (0-2)

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_DisableRecvDataInterruptCore(u32 dataNo)
{
    SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
    reg_DSP_PCFG &= ~((1 << dataNo) << REG_DSP_PCFG_RRIE_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SendDataIsEmpty

  Description:  �w�肵���R�}���h���W�X�^�̃f�[�^��DSP����M�ς݂��`�F�b�N���܂��B

  Arguments:    dataNo : �R�}���h���W�X�^�̔ԍ� (0-2)

  Returns:      DSP�����Ƀf�[�^����M���Ă���� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_SendDataIsEmptyCore(u32 dataNo)
{
    SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
    return (reg_DSP_PSTS & ((1 << dataNo) << REG_DSP_PSTS_RCOMIM_SHIFT)) ? FALSE : TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_RecvDataIsReady

  Description:  �w�肵�����v���C���W�X�^��DSP����f�[�^�����M���ꂽ���`�F�b�N���܂��B

  Arguments:    dataNo : ���v���C���W�X�^�̔ԍ� (0-2)

  Returns:      DSP���f�[�^�𑗐M���Ă���� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_RecvDataIsReadyCore(u32 dataNo)
{
    SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
    return (reg_DSP_PSTS & ((1 << dataNo) << REG_DSP_PSTS_RRI_SHIFT)) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SendData

  Description:  DSP�Ƀf�[�^�𑗐M���܂��B

  Arguments:    dataNo : �R�}���h���W�X�^�̔ԍ� (0-2)
                data   : ���M����f�[�^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SendDataCore(u32 dataNo, u16 data)
{
    SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
    while (DSP_SendDataIsEmptyCore(dataNo) == FALSE)
    {
    }
    dspData[dataNo].send = data;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_RecvData

  Description:  DSP����̃f�[�^����M���܂��B

  Arguments:    dataNo : ���v���C���W�X�^�̔ԍ� (0-2)

  Returns:      ��M�����f�[�^
 *---------------------------------------------------------------------------*/
u16 DSP_RecvDataCore(u32 dataNo)
{
    SDK_ASSERT(dataNo >= 0 && dataNo <= 2);
    while (DSP_RecvDataIsReadyCore(dataNo) == FALSE)
    {
    }
    return dspData[dataNo].recv;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_EnableFifoInterrupt

  Description:  FIFO�̊��荞�݂�L���ɂ��܂��B

  Arguments:    type : FIFO�̊��荞�ݗv��

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_EnableFifoInterruptCore(DSPFifoIntr type)
{
    reg_DSP_PCFG |= type;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_DisableFifoInterrupt

  Description:  FIFO�̊��荞�݂𖳌��ɂ��܂��B

  Arguments:    type : FIFO�̊��荞�ݗv��

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_DisableFifoInterruptCore(DSPFifoIntr type)
{
    reg_DSP_PCFG &= ~type;
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
void DSP_SendFifoExCore(DSPFifoMemSel memsel, u16 dest, const u16 *src, int size, u16 flags)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    u16 incmode = (u16)((flags & DSP_FIFO_FLAG_DEST_FIX) ? 0 : REG_DSP_PCFG_AIM_MASK);

    reg_DSP_PADR = dest;
    reg_DSP_PCFG = (u16)((reg_DSP_PCFG & ~(REG_DSP_PCFG_MEMSEL_MASK|REG_DSP_PCFG_AIM_MASK))
                        | memsel | incmode);

    if (flags & DSP_FIFO_FLAG_SRC_FIX)
    {
        while (size-- > 0)
        {
            while (reg_DSP_PSTS & REG_DSP_PSTS_WFFI_MASK)
            {
            }
            reg_DSP_PDATA = *src;
        }
    }
    else
    {
        while (size-- > 0)
        {
            while (reg_DSP_PSTS & REG_DSP_PSTS_WFFI_MASK)
            {
            }
            reg_DSP_PDATA = *src++;
        }
    }
    (void)OS_RestoreInterrupts(bak);
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
void DSP_RecvFifoExCore(DSPFifoMemSel memsel, u16* dest, u16 src, int size, u16 flags)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    DSPFifoRecvLength len;
    u16 incmode = (u16)((flags & DSP_FIFO_FLAG_SRC_FIX) ? 0 : REG_DSP_PCFG_AIM_MASK);

    SDK_ASSERT(memsel != DSP_FIFO_MEMSEL_PROGRAM);

    switch (flags & DSP_FIFO_FLAG_RECV_MASK)
    {
    case DSP_FIFO_FLAG_RECV_UNIT_2B:
        len = DSP_FIFO_RECV_2B;
        size = 1;
        break;
    case DSP_FIFO_FLAG_RECV_UNIT_16B:
        len = DSP_FIFO_RECV_16B;
        size = 8;
        break;
    case DSP_FIFO_FLAG_RECV_UNIT_32B:
        len = DSP_FIFO_RECV_32B;
        size = 16;
        break;
    default:
        len = DSP_FIFO_RECV_CONTINUOUS;
        break;
    }

    reg_DSP_PADR = src;
    reg_DSP_PCFG = (u16)((reg_DSP_PCFG & ~(REG_DSP_PCFG_MEMSEL_MASK|REG_DSP_PCFG_DRS_MASK|REG_DSP_PCFG_AIM_MASK))
                        | memsel | len | incmode | REG_DSP_PCFG_RS_MASK);

    if (flags & DSP_FIFO_FLAG_DEST_FIX)
    {
        while (size-- > 0)
        {
            while ((reg_DSP_PSTS & REG_DSP_PSTS_RFNEI_MASK) == 0)
            {
            }
            *dest = reg_DSP_PDATA;
        }
    }
    else
    {
        while (size-- > 0)
        {
            while ((reg_DSP_PSTS & REG_DSP_PSTS_RFNEI_MASK) == 0)
            {
            }
            *dest++ = reg_DSP_PDATA;
        }
    }
    reg_DSP_PCFG &= ~REG_DSP_PCFG_RS_MASK;
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SetCommandReg

  Description:  �R�}���h���W�X�^�ɒl���������݂܂��B

  Arguments:    regNo : �R�}���h���W�X�^�ԍ� (0-2)
                data  : �f�[�^

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SetCommandRegCore(u32 regNo, u16 data)
{
    SDK_ASSERT(regNo >= 0 && regNo <= 2);
    dspData[regNo].send = data;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetReplyReg

  Description:  ���v���C���W�X�^����l��ǂݍ��݂܂��B

  Arguments:    regNo : ���v���C���W�X�^�ԍ� (0-2)

  Returns:      �ǂݍ��񂾃f�[�^
 *---------------------------------------------------------------------------*/
u16 DSP_GetReplyRegCore(u32 regNo)
{
    SDK_ASSERT(regNo >= 0 && regNo <= 2);
    return dspData[regNo].recv;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SetSemaphore

  Description:  ARM����DSP�ւ̒ʒm�p�Z�}�t�H���W�X�^�ɒl���������݂܂��B

  Arguments:    mask : �Z�b�g����l

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_SetSemaphoreCore(u16 mask)
{
    reg_DSP_PSEM = mask;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetSemaphore

  Description:  DSP����ARM�ւ̒ʒm�p�Z�}�t�H���W�X�^�̒l��ǂݍ��݂܂��B

  Arguments:    �Ȃ�

  Returns:      DSP���Z�}�t�H�ɏ������񂾒l
 *---------------------------------------------------------------------------*/
u16 DSP_GetSemaphoreCore(void)
{
    return reg_DSP_SEM;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ClearSemaphore

  Description:  DSP����ARM�ւ̒ʒm�p�Z�}�t�H���W�X�^�̒l���N���A���܂��B

  Arguments:    mask : �N���A����r�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_ClearSemaphoreCore(u16 mask)
{
    reg_DSP_PCLEAR = mask;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_MaskSemaphore

  Description:  ���荞�݂𖳌��ɂ���DSP����ARM�ւ̒ʒm�p�Z�}�t�H���W�X�^�̃r�b�g��ݒ肵�܂��B

  Arguments:    mask : ���荞�݂𖳌��ɂ���r�b�g

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSP_MaskSemaphoreCore(u16 mask)
{
    reg_DSP_PMASK = mask;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_CheckSemaphoreRequest

  Description:  �Z�}�t�H���W�X�^�ɂ�銄�荞�ݗv�������邩�`�F�b�N���܂��B

  Arguments:    �Ȃ�

  Returns:      �v��������ꍇ�� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_CheckSemaphoreRequestCore(void)
{
    return (reg_DSP_PSTS & REG_DSP_PSTS_PSEMI_MASK) >> REG_DSP_PSTS_PSEMI_SHIFT;
}


#if defined(DSP_SUPPORT_OBSOLETE_LOADER)
/*---------------------------------------------------------------------------*
 * �ȉ��͌��ݎg�p����Ă��Ȃ��Ǝv����p�~���C���^�t�F�[�X�B
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * �ȉ��̓X�g���[�g�}�b�s���O�����ɂ��Â��C���^�t�F�[�X�B
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSPi_MapProcessSlotAsStraight

  Description:  �Z�O�����g�ԍ���WRAM�X���b�g�ԍ�����v����悤�X���b�g�}�b�v���������B
                (���̕�����Ex���t���Ȃ������̃��[�h�֐��ł̂ݎg�p�����)

  Arguments:    context : DSPProcessContext�\���́B
                slotB   : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC   : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static BOOL DSPi_MapProcessSlotAsStraight(DSPProcessContext *context, int slotB, int slotC)
{
    int     segment;
    for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment)
    {
        if (context->segmentCode & (1 << segment) != 0)
        {
            int     slot = segment;
            if ((slotB & (1 << slot)) == 0)
            {
                return FALSE;
            }
            context->slotOfSegmentCode[segment] = slot;
        }
    }
    for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment)
    {
        if (context->segmentData & (1 << segment) != 0)
        {
            int     slot = segment;
            if ((slotC & (1 << slot)) == 0)
            {
                return FALSE;
            }
            context->slotOfSegmentData[segment] = slot;
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadFileAuto

  Description:  COFF�`����DSP�v���O���������[�h���ĕK�v��WRAM��DSP�Ɋ��蓖�Ă�

  Arguments:    image : COFF�t�@�C��

  Returns:      �����Ȃ�� TRUE
 *---------------------------------------------------------------------------*/
BOOL DSP_LoadFileAutoCore(const void *image)
{
    // �ꎞ�I�Ƀ������t�@�C���֕ϊ��B
    FSFile  memfile[1];
    if(DSPi_CreateMemoryFile(memfile, image))
    {
        DSPProcessContext   context[1];
        DSP_InitProcessContext(context, NULL);
        return DSP_StartupProcess(context, memfile, 0xFF, 0xFF, DSPi_MapProcessSlotAsStraight);
    }
    return FALSE;
}


#endif

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
