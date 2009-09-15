/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     rom.h

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


#ifndef NITRO_FS_ROM_H_
#define NITRO_FS_ROM_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/card/hash.h>
#include <nitro/fs/file.h>
#include <nitro/fs/archive.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* functions */
#if defined(FS_IMPLEMENT)

/*---------------------------------------------------------------------------*
  Name:         FSi_InitRomArchive

  Description:  "rom" �A�[�J�C�u��������

  Arguments:    default_dma_no   ROM�A�N�Z�X�Ɏg�p����DMA�`�����l���B
                                 0-3�͈̔͊O�ł����CPU�ŃA�N�Z�X�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_InitRomArchive(u32 default_dma_no);

/*---------------------------------------------------------------------------*
  Name:         FSi_EndRomArchive

  Description:  "rom" �A�[�J�C�u�����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_EndRomArchive(void);

/*---------------------------------------------------------------------------*
  Name:         FSi_MountSRLFile

  Description:  SRL�t�@�C���Ɋ܂܂��ROM�t�@�C���V�X�e�����}�E���g

  Arguments:    arc              �}�E���g�Ɏg�p����FSArchive�\���́B
                                 ���O�͓o�^�ς݂łȂ���΂Ȃ�Ȃ��B
                file             ���łɃI�[�v������Ă���}�E���g�Ώۃt�@�C���B
                                 �}�E���g���͂��̍\���̂�j�����Ă͂Ȃ�Ȃ��B
                hash             �n�b�V���R���e�L�X�g�\���́B

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FSi_MountSRLFile(FSArchive *arc, FSFile *file, CARDRomHashContext *hash);

/*---------------------------------------------------------------------------*
  Name:         FSi_ConvertPathToFATFS

  Description:  �w�肳�ꂽ�p�X����FATFS�`���ɒu������

  Arguments:    dst                 �i�[��o�b�t�@
                src                 �ϊ����p�X
                ignorePermission    �A�N�Z�X���𖳎����Ă悢�ꍇ��TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ConvertPathToFATFS(char *dst, const char *src, BOOL ignorePermission);

FSResult FSi_ConvertError(u32 error);

/*---------------------------------------------------------------------------*
  Name:         FSi_MountFATFS

  Description:  FATFS�C���^�t�F�[�X���t�@�C���V�X�e���Ƀ}�E���g����B

  Arguments:    index               �g�p����z��v�f
                arcname             �A�[�J�C�u��
                drivename           �h���C�u��

  Returns:      �A�[�J�C�u���������}�E���g������TRUE
 *---------------------------------------------------------------------------*/
BOOL FSi_MountFATFS(u32 index, const char *arcname, const char *drivename);

/*---------------------------------------------------------------------------*
  Name:         FSi_MountDefaultArchives

  Description:  IPL�ɗ^����ꂽ�N���������Q�Ƃ��ăf�t�H���g�A�[�J�C�u���}�E���g

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_MountDefaultArchives(void);

#else /* FS_IMPLEMENT */

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadRomDirect

  Description:  �w���ROM�A�h���X�𒼐ڃ��[�h

  Arguments:    src              �]�����I�t�Z�b�g
                dst              �]����A�h���X
                len              �]���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_ReadRomDirect(const void *src, void *dst, u32 len);

#endif


#ifdef __cplusplus
} /* extern "C" */
#endif


/*---------------------------------------------------------------------------*/


#endif /* NITRO_FS_ROM_H_ */
