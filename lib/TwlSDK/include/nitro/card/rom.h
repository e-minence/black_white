/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     rom.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-05#$
  $Rev: 9518 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_CARD_ROM_H_
#define NITRO_CARD_ROM_H_


#include <nitro/card/types.h>

#include <nitro/mi/dma.h>
#include <nitro/mi/exMemory.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomHeader

  Description:  ���ݑ}������Ă���J�[�h��ROM�w�b�_�����擾

  Arguments:    None.

  Returns:      CARDRomHeader�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
const u8 *CARD_GetRomHeader(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetOwnRomHeader

  Description:  ���ݎ��s���Ă���v���O������ROM�w�b�_�����擾�B

  Arguments:    None.

  Returns:      CARDRomHeader�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
const CARDRomHeader *CARD_GetOwnRomHeader(void);

#ifdef SDK_TWL

/*---------------------------------------------------------------------------*
  Name:         CARD_GetOwnRomHeaderTWL

  Description:  ���ݎ��s���Ă���v���O������TWL-ROM�w�b�_�����擾�B

  Arguments:    None.

  Returns:      CARDRomHeaderTWL�\���̂ւ̃|�C���^
 *---------------------------------------------------------------------------*/
const CARDRomHeaderTWL *CARD_GetOwnRomHeaderTWL(void);

#endif // SDK_TWL

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomRegionFNT

  Description:  ROM �w�b�_�� FNT �̈�����擾����

  Arguments:    None.

  Returns:      ROM �w�b�_�� FNT �̈��� �ւ̃|�C���^
 *---------------------------------------------------------------------------*/
SDK_INLINE const CARDRomRegion *CARD_GetRomRegionFNT(void)
{
    const CARDRomHeader *header = CARD_GetOwnRomHeader();
    return &header->fnt;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomRegionFAT

  Description:  ROM �w�b�_�� FAT �̈�����擾����

  Arguments:    None.

  Returns:      ROM �w�b�_�� FAT �̈��� �ւ̃|�C���^
 *---------------------------------------------------------------------------*/
SDK_INLINE const CARDRomRegion *CARD_GetRomRegionFAT(void)
{
    const CARDRomHeader *header = CARD_GetOwnRomHeader();
    return &header->fat;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomRegionOVT

  Description:  ROM �w�b�_�� OVT �̈�����擾����

  Arguments:    None.

  Returns:      ROM �w�b�_�� OVT �̈��� �ւ̃|�C���^
 *---------------------------------------------------------------------------*/
SDK_INLINE const CARDRomRegion *CARD_GetRomRegionOVT(MIProcessor target)
{
    const CARDRomHeader *header = CARD_GetOwnRomHeader();
    return (target == MI_PROCESSOR_ARM9) ? &header->main_ovt : &header->sub_ovt;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_LockRom

  Description:  ROM�A�N�Z�X�̂��߂ɃJ�[�h�o�X�����b�N����

  Arguments:    lock id.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_LockRom(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockRom

  Description:  ���b�N�����J�[�h�o�X���������

  Arguments:    lock id which is used by CARD_LockRom().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_UnlockRom(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitRomAsync

  Description:  ROM�A�N�Z�X�֐�����������������

  Arguments:    None.

  Returns:      ROM�A�N�Z�X�֐����������Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARD_TryWaitRomAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_WaitRomAsync

  Description:  ROM�A�N�Z�X�֐�����������܂őҋ@

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_WaitRomAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRom

  Description:  ROM ���[�h�̊�{�֐�

  Arguments:    dma        �g�p���� DMA �`�����l��
                src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊����[�h���w�肷��Ȃ� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadRom(u32 dma, const void *src, void *dst, u32 len,
                      MIDmaCallback callback, void *arg, BOOL is_async);

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadRomAsync

  Description:  �񓯊� ROM ���[�h

  Arguments:    dma        �g�p���� DMA �`�����l��
                src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ReadRomAsync(u32 dma, const void *src, void *dst, u32 len,
                                  MIDmaCallback callback, void *arg)
{
    CARDi_ReadRom(dma, src, dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadRom

  Description:  ���� ROM ���[�h

  Arguments:    dma        �g�p���� DMA �`�����l��
                src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ReadRom(u32 dma, const void *src, void *dst, u32 len)
{
    CARDi_ReadRom(dma, src, dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCacheFlushThreshold

  Description:  �L���b�V���������𕔕��I�ɍs�����S�̂֍s������臒l���擾

  Arguments:    icache            ���߃L���b�V���̖�����臒l���i�[����|�C���^
                                  NULL�ł���Ζ��������
                dcache            �f�[�^�L���b�V���̖�����臒l���i�[����|�C���^
                                  NULL�ł���Ζ��������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_GetCacheFlushThreshold(u32 *icache, u32 *dcache);

/*---------------------------------------------------------------------------*
  Name:         CARD_SetCacheFlushThreshold

  Description:  �L���b�V���������𕔕��I�ɍs�����S�̂֍s������臒l��ݒ�

  Arguments:    icache            ���߃L���b�V���̖�����臒l
                dcache            �f�[�^�L���b�V���̖�����臒l

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_SetCacheFlushThreshold(u32 icache, u32 dcache);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCacheFlushFlag

  Description:  �L���b�V���������������I�ɍs�����ǂ����̐ݒ�t���O���擾�B

  Arguments:    icache            ���߃L���b�V���̎����������t���O���i�[����|�C���^
                                  NULL�ł���Ζ��������
                dcache            �f�[�^�L���b�V���̎����������t���O���i�[����|�C���^
                                  NULL�ł���Ζ��������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void     CARD_GetCacheFlushFlag(BOOL *icache, BOOL *dcache);

/*---------------------------------------------------------------------------*
  Name:         CARD_SetCacheFlushFlag

  Description:  �L���b�V���������������I�ɍs�����ǂ�����ݒ�B
                �f�t�H���g�ł͖��߃L���b�V����FALSE�Ńf�[�^�L���b�V����TRUE�B

  Arguments:    icache            ���߃L���b�V���̎�����������L���ɂ���Ȃ�TRUE
                dcache            �f�[�^�L���b�V���̎�����������L���ɂ���Ȃ�TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void     CARD_SetCacheFlushFlag(BOOL icache, BOOL dcache);


/*---------------------------------------------------------------------------*
 * internal functions
 *---------------------------------------------------------------------------*/

u32     CARDi_ReadRomID(void);
void    CARDi_RefreshRom(u32 warn_mask);
BOOL    CARDi_IsTwlRom(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetOwnSignature

  Description:  �������g��DS�_�E�����[�h�v���C�����f�[�^���擾�B

  Arguments:    None.

  Returns:      �������g��DS�_�E�����[�h�v���C�����f�[�^�B
 *---------------------------------------------------------------------------*/
const u8* CARDi_GetOwnSignature(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetOwnSignature

  Description:  �������g��DS�_�E�����[�h�v���C�����f�[�^��ݒ�B
                ��J�[�h�u�[�g���ɏ�ʃ��C�u��������Ăяo���B

  Arguments:    DS�_�E�����[�h�v���C�����f�[�^

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_SetOwnSignature(const void *signature);


#ifdef __cplusplus
} /* extern "C" */
#endif


/* NITRO_CARD_ROM_H_ */
#endif
