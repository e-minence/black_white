/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_dma.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/card_utility.h"


/*---------------------------------------------------------------------------*/
/* constants */

static const CARDDmaInterface CARDiDmaUsingFormer =
{
    MIi_CardDmaCopy32,
    MI_StopDma,
};
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static CARDDmaInterface CARDiDmaUsingNew =
{
    MI_Card_A_NDmaCopy,
    MI_StopNDma,
};
#include <twl/ltdmain_end.h>
#endif

/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetDmaInterface

  Description:  DMA�`�����l������K�؂ɔ��肵��
                �V�������ꂩ��DMA�R�}���h�C���^�t�F�[�X���擾�B

  Arguments:    channel : DMA�`�����l���B
                          0����MI_DMA_MAX_NUM�܂ł͈̔͂ł���A
                          MI_DMA_USING_NEW���L���Ȃ�V�KDMA�ł��邱�Ƃ������B

  Returns:      �Y������V�������ꂩ��DMA�C���^�t�F�[�X�B
 *---------------------------------------------------------------------------*/
const CARDDmaInterface* CARDi_GetDmaInterface(u32 channel)
{
    const CARDDmaInterface *retval = NULL;
    // �L���ȃ`�����l���͈͂Ȃ�DMA���g�p�B
    BOOL    isNew = ((channel & MI_DMA_USING_NEW) != 0);
    channel &= ~MI_DMA_USING_NEW;
    if (channel <= MI_DMA_MAX_NUM)
    {
        // �]����DMA�B
        if (!isNew)
        {
            retval = &CARDiDmaUsingFormer;
        }
        // �V�KDMA��TWL���œ��삵�Ă���ꍇ�̂ݎg�p�\�B
        else if (!OS_IsRunOnTwl())
        {
            OS_TPanic("NDMA can use only TWL!");
        }
#ifdef SDK_TWL
        else
        {
            retval = &CARDiDmaUsingNew;
        }
#endif
    }
    return retval;
}

#ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         CARDi_ICInvalidateSmart

  Description:  臒l�����Ƃ�IC_InvalidateAll�֐���IC_InvalidateRange�֐���I���B

  Arguments:    buffer     invalidate���ׂ��o�b�t�@
                length     invalidate���ׂ��T�C�Y
                threshold  ������؂�ւ���臒l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ICInvalidateSmart(void *buffer, u32 length, u32 threshold)
{
    if (length >= threshold)
    {
        IC_InvalidateAll();
    }
    else
    {
        IC_InvalidateRange((void *)buffer, length);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_DCInvalidateSmart

  Description:  臒l�����Ƃ�DC_FlushAll�֐���DC_InvalidateRange�֐���I���B
                �o�b�t�@���L���b�V�����C�����E�������Ă��Ȃ��ꍇ��
                ��[����яI�[����Store����������ōs���B

  Arguments:    buffer     invalidate���ׂ��o�b�t�@
                length     invalidate���ׂ��T�C�Y
                threshold  ������؂�ւ���臒l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_DCInvalidateSmart(void *buffer, u32 length, u32 threshold)
{
    if (length >= threshold)
    {
        DC_FlushAll();
    }
    else
    {
        u32     len = length;
        u32     pos = (u32)buffer;
        u32     mod = (pos & (HW_CACHE_LINE_SIZE - 1));
        if (mod)
        {
            pos -= mod;
            DC_StoreRange((void *)(pos), HW_CACHE_LINE_SIZE);
            DC_StoreRange((void *)(pos + length), HW_CACHE_LINE_SIZE);
            length += HW_CACHE_LINE_SIZE;
        }
        DC_InvalidateRange((void *)pos, length);
        DC_WaitWriteBufferEmpty();
    }
}

#endif // SDK_ARM9
