/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_rom.h

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
#ifndef NITRO_LIBRARIES_CARD_ROM_H__
#define NITRO_LIBRARIES_CARD_ROM_H__


#include <nitro.h>
#include "../include/card_common.h"


// #define SDK_ARM7_READROM_SUPPORT


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

typedef struct CARDTransferInfo
{
    u32     command;
    void  (*callback)(void *userdata);
    void   *userdata;
    u32     src;
    u32     dst;
    u32     len;
    u32     work;
}
CARDTransferInfo;

typedef void (*CARDTransferCallbackFunction)(void *userdata);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomWithCPU

  Description:  CPU���g�p����ROM�]���B
                �L���b�V����y�[�W�P�ʂ̐������l������K�v�͖�����
                �]�������܂Ŋ֐����u���b�L���O����_�ɒ��ӁB

  Arguments:    userdata          (���̃R�[���o�b�N�Ƃ��Ďg�p���邽�߂̃_�~�[)
                buffer            �]����o�b�t�@
                offset            �]����ROM�I�t�Z�b�g
                length            �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
int CARDi_ReadRomWithCPU(void *userdata, void *buffer, u32 offset, u32 length);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomWithDMA

  Description:  DMA���g�p����ROM�]���B
                �ŏ��̃y�[�W�̓]�����J�n������֐��͂������ɐ����Ԃ��B

  Arguments:    info              �]�����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadRomWithDMA(CARDTransferInfo *info);

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitRom

  Description:  ROM�A�N�Z�X�Ǘ������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitRom(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_CheckPulledOutCore

  Description:  �J�[�h�������o�֐��̃��C�������B
                �J�[�h�o�X�̓��b�N����Ă���K�v������B

  Arguments:    id            �J�[�h����ǂݏo���ꂽ ROM-ID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_CheckPulledOutCore(u32 id);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomIDCore

  Description:  �J�[�h ID �̓ǂݏo��

  Arguments:    None.

  Returns:      �J�[�h ID
 *---------------------------------------------------------------------------*/
u32     CARDi_ReadRomIDCore(void);

#ifdef SDK_ARM7_READROM_SUPPORT

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomCore

  Description:  ARM7 ����̃J�[�h�A�N�Z�X

  Arguments:    src        �]�����I�t�Z�b�g
                src        �]�����������A�h���X
                src        �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadRomCore(const void *src, void *dst, u32 len);

#endif // SDK_ARM7_READROM_SUPPORT

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomStatusCore

  Description:  �J�[�h�X�e�[�^�X�̓ǂݏo���B
                ���t���b�V���Ή�ROM���o���̂ݔ��s�B
                �Ή�ROM�𓋍ڂ���NITRO�A�v���P�[�V�����ł��K�v�ɂȂ�B

  Arguments:    None.

  Returns:      �J�[�h�X�e�[�^�X
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomStatusCore(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_RefreshRomCore

  Description:  �J�[�hROM�o�b�h�u���b�N�̃��t���b�V���B
                �Y��ROM�𓋍ڂ���NITRO�A�v���P�[�V�����ł��K�v�ɂȂ�B
                �J�[�h�փR�}���h������I�ɔ��s���邾���Ȃ̂Ń��C�e���V�ݒ�͖����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_RefreshRomCore(void);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_LIBRARIES_CARD_ROM_H__
