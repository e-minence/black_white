/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_utility.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_LIBRARIES_CARD_UTILITY_H__
#define NITRO_LIBRARIES_CARD_UTILITY_H__


#include <nitro/types.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

// �V����DMA��؂�ւ��邽�߂̃R�}���h�C���^�t�F�[�X
typedef struct CARDDmaInterface
{
    void  (*Recv)(u32 channel, const void *src, void *dst, u32 len);
    void  (*Stop)(u32 channel);
}
CARDDmaInterface;


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
const CARDDmaInterface* CARDi_GetDmaInterface(u32 channel);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ICInvalidateSmart

  Description:  臒l�����Ƃ�IC_InvalidateAll�֐���IC_InvalidateRange�֐���I���B

  Arguments:    buffer     invalidate���ׂ��o�b�t�@
                length     invalidate���ׂ��T�C�Y
                threshold  ������؂�ւ���臒l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ICInvalidateSmart(void *buffer, u32 length, u32 threshold);

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
void CARDi_DCInvalidateSmart(void *buffer, u32 length, u32 threshold);


#ifdef __cplusplus
} // extern "C"
#endif



#endif // NITRO_LIBRARIES_CARD_UTILITY_H__
