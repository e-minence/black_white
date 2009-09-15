/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - include
  File:     api.h

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
#if	!defined(NITRO_FS_API_H_)
#define NITRO_FS_API_H_


#include <nitro/fs/archive.h>
#include <nitro/fs/file.h>
#include <nitro/fs/romfat.h>
#include <nitro/fs/overlay.h>
#include <nitro/std.h>

#ifdef SDK_TWL
#include <twl/fatfs.h>
#endif // SDK_TWL

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FS_Init

  Description:  FS���C�u������������

  Arguments:    default_dma_no   �K�v�ɉ����ē����Ŏg�p����DMA�ԍ��B (0-3)
                                 �V�KDMA���g�p����ꍇ��MI_DMA_USING_NEW�������A
                                 DMA���g�p�����Ȃ��ꍇ��MI_DMA_NOT_USE���w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_Init(u32 default_dma_no);

/*---------------------------------------------------------------------------*
  Name:         FS_InitFatDriver

  Description:  FAT�h���C�o���������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if defined(SDK_TWL)
void FS_InitFatDriver(void);
#endif

/*---------------------------------------------------------------------------*
  Name:         FS_IsAvailable

  Description:  FS���C�u�������������ς݂�����

  Arguments:    None.

  Returns:      FS_Init�֐������łɌĂяo����Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_IsAvailable(void);

/*---------------------------------------------------------------------------*
  Name:         FS_End

  Description:  �S�ẴA�[�J�C�u���A���}�E���g����FS���C�u�������I������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_End(void);

/*---------------------------------------------------------------------------*
  Name:         FS_GetDefaultDMA

  Description:  ���ݐݒ肳��Ă���FS���C�u�����pDMA�`�����l�����擾

  Arguments:    None.

  Returns:      current DMA channel.
 *---------------------------------------------------------------------------*/
u32     FS_GetDefaultDMA(void);

/*---------------------------------------------------------------------------*
  Name:         FS_SetDefaultDMA

  Description:  FS���C�u�����pDMA�`�����l����ݒ肷��

  Arguments:    dma_no      �K�v�ɉ����ē����Ŏg�p����DMA�ԍ��B (0-3)
                            �V�KDMA���g�p����ꍇ��MI_DMA_USING_NEW�������A
                            DMA���g�p�����Ȃ��ꍇ��MI_DMA_NOT_USE���w�肷��B

  Returns:      �ȑO�ɐݒ肳��Ă���DMA�`�����l��
 *---------------------------------------------------------------------------*/
u32     FS_SetDefaultDMA(u32 dma_no);

/*---------------------------------------------------------------------------*
  Name:         FS_SetCurrentDirectory

  Description:  �J�����g�f�B���N�g����ύX����

  Arguments:    path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_SetCurrentDirectory(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileFromMemory

  Description:  �������̈���}�b�v�����t�@�C�����ꎞ�I�ɐ���

  Arguments:    file             �t�@�C���n���h�����i�[����FSFile�\����
                buf              READ ����� WRITE �̑ΏۂƂȂ郁����
                size             buf �̃o�C�g�T�C�Y

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_CreateFileFromMemory(FSFile *file, void *buf, u32 size);

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileFromRom

  Description:  �w��� CARD-ROM �̈���}�b�v�����t�@�C�����ꎞ�I�ɐ���

  Arguments:    file             �t�@�C���n���h�����i�[���� FSFile �\����
                offset           READ �̑ΏۂƂȂ� CARD-ROM �̈�擪�I�t�Z�b�g
                size             �Ώۗ̈�� offset ����̃o�C�g�T�C�Y

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_CreateFileFromRom(FSFile *file, u32 offset, u32 size);

/*---------------------------------------------------------------------------*
  Name:         FS_OpenTopLevelDirectory

  Description:  �A�[�J�C�u����񋓂ł������ȍŏ�ʃf�B���N�g�����J��

  Arguments:    dir              �f�B���N�g���n���h�����i�[���� FSFile �\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_OpenTopLevelDirectory(FSFile *dir);

/*---------------------------------------------------------------------------*
  Name:         FS_TryLoadTable

  Description:  "rom"�A�[�J�C�u�̃e�[�u���e�[�^���v�����[�h����

  Arguments:    mem              �e�[�u���f�[�^�̊i�[��o�b�t�@
                                 NULl�Ȃ�P�ɃT�C�Y�������v�Z���ĕԂ�
                size             mem�̃T�C�Y

  Returns:      "rom"�A�[�J�C�u��FAT��FNT���v�����[�h����̂ɕK�v�ȃo�C�g��
 *---------------------------------------------------------------------------*/
u32     FS_TryLoadTable(void *mem, u32 size);

/*---------------------------------------------------------------------------*
  Name:         FS_GetTableSize

  Description:  "ROM"�A�[�J�C�u�̃e�[�u���T�C�Y���擾

  Arguments:    None.

  Returns:      "rom"�A�[�J�C�u��FAT��FNT���v�����[�h����̂ɕK�v�ȃo�C�g��
 *---------------------------------------------------------------------------*/
SDK_INLINE u32 FS_GetTableSize(void)
{
    return FS_TryLoadTable(NULL, 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadTable

  Description:  "rom"�A�[�J�C�u�̃e�[�u���e�[�^���v�����[�h����

  Arguments:    mem              �e�[�u���f�[�^�̊i�[��o�b�t�@
                size             mem�̃T�C�Y

  Returns:      �v�����[�h�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_LoadTable(void *mem, u32 size)
{
    return (FS_TryLoadTable(mem, size) <= size);
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadTable

  Description:  "rom"�A�[�J�C�u�Ƀv�����[�h�����e�[�u�����������

  Arguments:    None.

  Returns:      �e�[�u���̂��߂Ɋ��蓖�Ă��Ă����o�b�t�@�ւ̃|�C���^
 *---------------------------------------------------------------------------*/
SDK_INLINE void *FS_UnloadTable(void)
{
    return FS_UnloadArchiveTables(FS_FindArchive("rom", 3));
}

/*---------------------------------------------------------------------------*
  Name:         FS_ForceToEnableLatencyEmulation

  Description:  �򉻂���NAND�f�o�C�X�ւ̃A�N�Z�X���ɐ�����
                �����_���ȃE�F�C�g���Ԃ��h���C�o�ŋ[���I�ɍČ�����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_ForceToEnableLatencyEmulation(void);


/*---------------------------------------------------------------------------*
 * internal functions
 *---------------------------------------------------------------------------*/

#define FS_TMPBUF_LENGTH        2048
#define FS_MOUNTDRIVE_MAX       OS_MOUNT_INFO_MAX
#define FS_TEMPORARY_BUFFER_MAX (FS_TMPBUF_LENGTH * FS_MOUNTDRIVE_MAX)

#ifdef SDK_TWL
typedef struct FSFATFSArchiveContext
{
    FSArchive       arc[1];
    char            fullpath[2][FATFS_PATH_MAX];
    u8             *tmpbuf;
    FATFSDriveResource  resource[1];
}
FSFATFSArchiveContext;

typedef struct FSFATFSArchiveWork
{
    u8 tmpbuf[FS_TMPBUF_LENGTH];
    FSFATFSArchiveContext context;
    int slot;
}
FSFATFSArchiveWork;


/*---------------------------------------------------------------------------*
  Name:         FSiFATFSDrive

  Description:  FAT�x�[�X�̃A�[�J�C�u���Ǘ�����R���e�L�X�g�z��B
                �ʏ��FS_MOUNTDRIVE_MAX�����̔z��T�C�Y��K�v�Ƃ��A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                �I�[�o�[���C�h����FSi_SetupFATBuffers�֐��̌Ăяo��������
                ���̕ϐ���ύX���ēK�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FSFATFSArchiveContext�\����FS_MOUNTDRIVE_MAX���̃o�b�t�@
 *---------------------------------------------------------------------------*/
extern FSFATFSArchiveContext *FSiFATFSDrive;

/*---------------------------------------------------------------------------*
  Name:         FSiFATFSAsyncRequest

  Description:  FAT�x�[�X�̃A�[�J�C�u�Ŕ񓯊������Ɏg�p����R�}���h�o�b�t�@�B
                �ʏ��FS_MOUNTDRIVE_MAX�����̔z��T�C�Y��K�v�Ƃ��A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                �I�[�o�[���C�h����FSi_SetupFATBuffers�֐��̌Ăяo��������
                ���̕ϐ���ύX���ēK�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FS_TEMPORARY_BUFFER_MAX�o�C�g�̐ÓI�ȃR�}���h�o�b�t�@
 *---------------------------------------------------------------------------*/
extern FATFSRequestBuffer *FSiFATFSAsyncRequest;

/*---------------------------------------------------------------------------*
  Name:         FSi_MountSpecialArchive

  Description:  ����ȃA�[�J�C�u�𒼐ڃ}�E���g

  Arguments:    param   : �}�E���g�Ώۂ��w�肷��p�����[�^
                arcname : �}�E���g����A�[�J�C�u��
                          "otherPub"�܂���"otherPrv"���w��\��
                          NULL���w�肷��ƈȑO�̃A�[�J�C�u���A���}�E���g
                pWork   : �}�E���g�Ɏg�p���郏�[�N�̈�B
                          �}�E���g����Ă���ԕێ������K�v������܂��B

  Returns:      ���������������FS_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
FSResult FSi_MountSpecialArchive(u64 param, const char *arcname, FSFATFSArchiveWork* pWork);

/*---------------------------------------------------------------------------*
  Name:         FSi_FormatSpecialArchive

  Description:  �ȉ��̏����𖞂�������ȃA�[�J�C�u�̓��e���t�H�[�}�b�g�B
                  - ���݂��łɃ}�E���g����Ă���B
                  - ���g�����L�������B (dataPub, dataPrv, share*)

  Arguments:    path : �A�[�J�C�u�����܂񂾃p�X��

  Returns:      ���������������FS_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
FSResult FSi_FormatSpecialArchive(const char *path);

#endif // SDK_TWL

/*---------------------------------------------------------------------------*
  Name:         FSiTemporaryBuffer

  Description:  Read/Write�R�}���h���s�p�̐ÓI�Ȉꎞ�o�b�t�@�ւ̃|�C���^�B
                ARM9/ARM7�Ƃ��ɎQ�Ɖ\�ȃ������ł���K�v������A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̕ϐ���FS_Init�֐��̌Ăяo�����O�ɕύX���邱�Ƃ�
                �K�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FS_TEMPORARY_BUFFER_MAX�o�C�g�̐ÓI�ȃR�}���h�o�b�t�@
 *---------------------------------------------------------------------------*/
extern u8 *FSiTemporaryBuffer/* [FS_TEMPORARY_BUFFER_MAX] ATTRIBUTE_ALIGN(32)*/;

/*---------------------------------------------------------------------------*
  Name:         FSi_SetupFATBuffers

  Description:  FAT�x�[�X�̃A�[�J�C�u�ɕK�v�Ȋe��o�b�t�@���������B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̊֐����I�[�o�[���C�h���Ċe��o�b�t�@��Ǝ��ɐݒ肵
                ���v�������T�C�Y��K�v�Œ���ɗ}�~���邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_SetupFATBuffers(void);

/*---------------------------------------------------------------------------*
  Name:         FSi_OverrideRomArchive

  Description:  �K�v�ɉ�����ROM�A�[�J�C�u�̓��e��u��������B

  Arguments:    arc              �}�E���g���ׂ�ROM�A�[�J�C�u�\����

  Returns:      �J�[�h��ɂ���W����ROM�A�[�J�C�u���g�p����
                �ʂ̎����ɒu��������ׂ��ł����TRUE��Ԃ��K�v������B
 *---------------------------------------------------------------------------*/
BOOL FSi_OverrideRomArchive(FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FSi_IsValidAddressForARM7

  Description:  ARM7����A�N�Z�X�\�ȃo�b�t�@�����肷��B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̊֐����I�[�o�[���C�h���邱�Ƃ�
                �K�؂Ȕ��胋�[�`�����Ē�`���邱�Ƃ��ł���B

  Arguments:    buffer              ���肷�ׂ��o�b�t�@
                length              �o�b�t�@��

  Returns:      ARM7����A�N�Z�X�\�ȃo�b�t�@�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL FSi_IsValidAddressForARM7(const void *buffer, u32 length);

/*---------------------------------------------------------------------------*
  Name:         FSi_SetSwitchableWramSlots

  Description:  �󋵂ɉ�����FS���C�u������ARM7�֐؂�ւ��Ă悢WRAM�X���b�g���w��B

  Arguments:    bitsB               WRAM-B�X���b�g�̃r�b�g�W��
                bitsC               WRAM-C�X���b�g�̃r�b�g�W��

  Returns:      None
 *---------------------------------------------------------------------------*/
void FSi_SetSwitchableWramSlots(int bitsB, int bitsC);

/*---------------------------------------------------------------------------*
  Name:         FSi_UnmountRomAndCloseNANDSRL

  Description:  NAND�A�v���P�[�V�����p��ROM�A�[�J�C�u�𖳌�����SRL�t�@�C�������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_UnmountRomAndCloseNANDSRL(void);

/*---------------------------------------------------------------------------*
  Name:         FSi_ConvertStringSjisToUnicode

  Description:  ShiftJIS�������Unicode������ɕϊ��B
                �����p�X�������炩��ASCII�݂̂ł���ꍇ�Ȃ�
                Unicode��ShiftJIS�̑��ݕϊ����ȗ����ł���ꍇ��
                ���̊֐����I�[�o�[���C�h���邱�Ƃɂ����
                STD���C�u�����̕W�������������N�����̂�h�����Ƃ��ł���B

  Arguments:    dst               �ϊ���o�b�t�@.
                                  NULL ���w�肷��Ɗi�[�����͖��������.
                dst_len           �ϊ���o�b�t�@�̍ő啶�������i�[���ēn��,
                                  ���ۂɊi�[���ꂽ���������󂯎��|�C���^.
                                  NULL ��^�����ꍇ�͖��������.
                src               �ϊ����o�b�t�@.
                src_len           �ϊ����ׂ��ő啶�������i�[���ēn��,
                                  ���ۂɕϊ����ꂽ���������󂯎��|�C���^.
                                  ���̎w�����������I�[�̈ʒu���D�悳���.
                                  ���̒l���i�[���ēn���� NULL ��^�����ꍇ��
                                  �I�[�ʒu�܂ł̕��������w�肵���Ƃ݂Ȃ����.
                callback          �ϊ��ł��Ȃ����������ꂽ���ɌĂ΂��R�[���o�b�N.
                                  NULL���w�肵���ꍇ, �ϊ��ł��Ȃ������̈ʒu��
                                  �ϊ��������I������.

  Returns:      �ϊ������̌���.
 *---------------------------------------------------------------------------*/
STDResult FSi_ConvertStringSjisToUnicode(u16 *dst, int *dst_len,
                                         const char *src, int *src_len,
                                         STDConvertUnicodeCallback callback);

/*---------------------------------------------------------------------------*
  Name:         FSi_ConvertStringUnicodeToSjis

  Description:  Unicode�������ShiftJIS������ɕϊ��B
                �����p�X�������炩��ASCII�݂̂ł���ꍇ�Ȃ�
                Unicode��ShiftJIS�̑��ݕϊ����ȗ����ł���ꍇ��
                ���̊֐����I�[�o�[���C�h���邱�Ƃɂ����
                STD���C�u�����̕W�������������N�����̂�h�����Ƃ��ł���B

  Arguments:    dst               �ϊ���o�b�t�@.
                                  NULL ���w�肷��Ɗi�[�����͖��������.
                dst_len           �ϊ���o�b�t�@�̍ő啶�������i�[���ēn��,
                                  ���ۂɊi�[���ꂽ���������󂯎��|�C���^.
                                  NULL ��^�����ꍇ�͖��������.
                src               �ϊ����o�b�t�@.
                src_len           �ϊ����ׂ��ő啶�������i�[���ēn��,
                                  ���ۂɕϊ����ꂽ���������󂯎��|�C���^.
                                  ���̎w�����������I�[�̈ʒu���D�悳���.
                                  ���̒l���i�[���ēn���� NULL ��^�����ꍇ��
                                  �I�[�ʒu�܂ł̕��������w�肵���Ƃ݂Ȃ����.
                callback          �ϊ��ł��Ȃ����������ꂽ���ɌĂ΂��R�[���o�b�N.
                                  NULL���w�肵���ꍇ, �ϊ��ł��Ȃ������̈ʒu��
                                  �ϊ��������I������.

  Returns:      �ϊ������̌���.
 *---------------------------------------------------------------------------*/
STDResult FSi_ConvertStringUnicodeToSjis(char *dst, int *dst_len,
                                         const u16 *src, int *src_len,
                                         STDConvertSjisCallback callback);

/*---------------------------------------------------------------------------*
  Name:         FSi_GetUnicodeBuffer

  Description:  Unicode�ϊ��p�̈ꎞ�o�b�t�@���擾�B
                FS���C�u������Shift_JIS��ϊ����邽�߂Ɏg�p�����B

  Arguments:    src : Unicode�ϊ��̕K�v��Shift_JIS������܂���NULL

  Returns:      �K�v�ɉ�����UTF16-LE�֕ϊ����ꂽ������o�b�t�@
 *---------------------------------------------------------------------------*/
u16* FSi_GetUnicodeBuffer(const char *src);

/*---------------------------------------------------------------------------*
  Name:         FSi_ReleaseUnicodeBuffer

  Description:  Unicode�ϊ��p�̈ꎞ�o�b�t�@������B

  Arguments:    buf : FSi_GetUnicodeBuffer()�Ŋm�ۂ����o�b�t�@

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ReleaseUnicodeBuffer(const void *buf);


/*---------------------------------------------------------------------------*
 * obsolete functions
 *---------------------------------------------------------------------------*/

#define	FS_DMA_NOT_USE	MI_DMA_NOT_USE

/*---------------------------------------------------------------------------*
  Name:         FS_ChangeDir

  Description:  �J�����g�f�B���N�g����ύX����

  Arguments:    path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_ChangeDir(const char *path);


#ifdef SDK_ARM7
#define FS_CreateReadServerThread(priority) (void)CARD_SetThreadPriority(priority)
#endif // SDK_ARM7


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_API_H_ */
