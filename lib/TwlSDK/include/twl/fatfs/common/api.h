/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FATFS - include
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

#ifndef NITRO_FATFS_API_H_
#define NITRO_FATFS_API_H_


#include <twl/fatfs/common/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FATFS_Init

  Description:  FATFS���C�u������������

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM9
void FATFS_Init(void);
#else
BOOL FATFS_Init(u32 dma1, u32 dma2, u32 priority);
#endif

/*---------------------------------------------------------------------------*
  Name:         FATFSi_IsInitialized

  Description:  FATFS���C�u�������������ς݂��ǂ�����Ԃ�

  Arguments:    None

  Returns:      �������ς݂Ȃ�TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFSi_IsInitialized(void);

/*---------------------------------------------------------------------------*
  Name:         FATFSi_GetArcnameList

  Description:  �A�N�Z�X�\�ȃA�[�J�C�u���̈ꗗ���擾�B

  Arguments:    None

  Returns:      "\0"�ŋ�؂��I�[��"\0\0"�ŗ^����ꂽ�A�[�J�C�u���ꗗ�B
 *---------------------------------------------------------------------------*/
const char* FATFSi_GetArcnameList(void);

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetLastError

  Description:  �Ō�ɔ��s�������N�G�X�g�̌��ʂ��擾�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
u32 FATFS_GetLastError(void);

/*---------------------------------------------------------------------------*
  Name:         FATFS_RegisterResultBuffer

  Description:  �Ăяo�����X���b�h�Ɋ֘A�t����R�}���h���ʃo�b�t�@��o�^�B

  Arguments:    buffer : FATFSResultBuffer�\���́B
                enable : �o�^�Ȃ�TRUE, �����Ȃ�FALSE�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FATFS_RegisterResultBuffer(FATFSResultBuffer *buffer, BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         FATFS_MountDrive

  Description:  �w���NAND�p�[�e�B�V�������h���C�u�Ƀ}�E���g

  Arguments:    name : �h���C�u������ ("A"-"Z")
                partition : NAND���̃p�[�e�B�V�����ԍ�

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_MountDrive(const char *name, FATFSMediaType media, u32 partition);

/*---------------------------------------------------------------------------*
  Name:         FATFS_MountNAND

  Description:  �w���NAND�p�[�e�B�V�������h���C�u�Ƀ}�E���g

  Arguments:    name : �h���C�u������ ("A"-"Z")
                partition : NAND���̃p�[�e�B�V�����ԍ�

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FATFS_MountNAND(const char *name, u32 partition)
{
    return FATFS_MountDrive(name, FATFS_MEDIA_TYPE_NAND, partition);
}

/*---------------------------------------------------------------------------*
  Name:         FATFS_UnmountDrive

  Description:  �w��̃h���C�u���A���}�E���g

  Arguments:    name : �h���C�u������ ("A"-"Z")

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_UnmountDrive(const char *name);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SetDefaultDrive

  Description:  �f�t�H���g�̃h���C�u��I��

  Arguments:    path : �h���C�u�����܂񂾃p�X��

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SetDefaultDrive(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FATFS_FormatDrive

  Description:  �w��̃p�X���w���h���C�u��������

  Arguments:    path : �h���C�u�����܂񂾃p�X��

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_FormatDrive(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FATFSi_FormatDriveEx

  Description:  �w��̃p�X���w���h���C�u�S�̂܂��̓��f�B�A�S�̂�������

  Arguments:    path        : �h���C�u�����܂񂾃p�X��
                formatMedia : ���f�B�A�S�̂���������Ȃ�TRUE

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFSi_FormatDriveEx(const char *path, BOOL formatMedia);

/*---------------------------------------------------------------------------*
  Name:         FATFSi_FormatMedia

  Description:  �w��̃p�X���w�����f�B�A�S�̂�������

  Arguments:    path : �h���C�u�����܂񂾃p�X��

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FATFSi_FormatMedia(const char *path)
{
    return FATFSi_FormatDriveEx(path, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         FATFS_CheckDisk

  Description:  �f�B�X�N�̓��e���`�F�b�N���A�K�v�Ȃ�C������

  Arguments:    name : �h���C�u������ ("A"-"Z")
                info : ���ʂ��i�[����FATFSDiskInfo�\����
                verbose : �`�F�b�N���e���f�o�b�O�o�͂���ꍇ��TRUE
                fixProblems : ���o���ꂽFAT�ُ̈�����̏�ŏC������ꍇ��TRUE
                writeChains : �Ǘ������`�F�[������t�@�C���𕜌�����ꍇ��TRUE
                              (fixProblems���L���̏ꍇ�ɂ̂ݗL��)

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CheckDisk(const char *name, FATFSDiskInfo *info, BOOL verbose, BOOL fixProblems, BOOL writeChains);

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetDriveResource

  Description:  �f�B�X�N�̋󂫗e�ʂ��擾

  Arguments:    path     : �h���C�u�����܂񂾃p�X��
                resource : ���ʂ��i�[����FATFSDriveResource�\����

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_GetDriveResource(const char *path, FATFSDriveResource *resource);

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetDiskSpace

  Description:  �f�B�X�N�̋󂫗e�ʂ��擾

  Arguments:    name : �h���C�u������ ("A"-"Z")
                totalBlocks : ���v�u���b�N�����擾����|�C���^�܂���NULL
                freeBlocks : �󂫃u���b�N�����擾����|�C���^�܂���NULL

  Returns:      ��������΋󂫗e�ʂ������o�C�g���A���s�����-1
 *---------------------------------------------------------------------------*/
int FATFS_GetDiskSpace(const char *name, u32 *totalBlocks, u32 *freeBlocks);

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetFileInfo

  Description:  �t�@�C���܂��̓f�B���N�g���̏����擾

  Arguments:    path : �p�X��
                info : ���ʂ��i�[����FATFSFileInfo�\����

  Returns:      �w��̃p�X�����݂���Ώ����擾����TRUE�A���݂��Ȃ����FALSE
 *---------------------------------------------------------------------------*/
BOOL FATFS_GetFileInfo(const char *path, FATFSFileInfo *info);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SetFileInfo

  Description:  �t�@�C���܂��̓f�B���N�g���̏���ύX

  Arguments:    path : �p�X��
                info : �ύX���ׂ���񂪊i�[���ꂽFATFSFileInfo�\����

  Returns:      �w��̃p�X�����݂���Ώ���ύX����TRUE�A���݂��Ȃ����FALSE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SetFileInfo(const char *path, const FATFSFileInfo *info);

/*---------------------------------------------------------------------------*
  Name:         FATFS_CreateFile

  Description:  �w��̃p�X�Ƀt�@�C���𐶐�

  Arguments:    path : �p�X��
                trunc : ���łɑ��݂���t�@�C����j������Ȃ�TRUE
                permit : �t�@�C���̃A�N�Z�X�� (rwx){1,3}

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CreateFile(const char *path, BOOL trunc, const char *permit);

/*---------------------------------------------------------------------------*
  Name:         FATFS_DeleteFile

  Description:  �w��̃t�@�C�����폜

  Arguments:    path : �p�X��

  Returns:      �폜�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_DeleteFile(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FATFS_RenameFile

  Description:  �w��̃t�@�C��������

  Arguments:    path : �p�X��
                newpath : �V�����p�X��

  Returns:      �����ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_RenameFile(const char *path, const char *newpath);

/*---------------------------------------------------------------------------*
  Name:         FATFS_CreateDirectory

  Description:  �w��̃p�X�Ƀf�B���N�g���𐶐�

  Arguments:    path : �p�X��
                permit : �f�B���N�g���̃A�N�Z�X�� (rwx){1,3}

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CreateDirectory(const char *path, const char *permit);

/*---------------------------------------------------------------------------*
  Name:         FATFS_DeleteDirectory

  Description:  �w��̃f�B���N�g�����폜

  Arguments:    path : �p�X��

  Returns:      �폜�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_DeleteDirectory(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FATFS_RenameDirectory

  Description:  �w��̃f�B���N�g��������

  Arguments:    path : �p�X��
                newpath : �V�����p�X��

  Returns:      �����ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_RenameDirectory(const char *path, const char *newpath);

/*---------------------------------------------------------------------------*
  Name:         FATFS_OpenFile

  Description:  �w��p�X�̃t�@�C�����J��

  Arguments:    path : �p�X��
                mode : �t�@�C���̃A�N�Z�X���[�h ("r/w/r+/w+" + "b/t")

  Returns:      �J�����t�@�C���̃n���h���܂���NULL
 *---------------------------------------------------------------------------*/
FATFSFileHandle FATFS_OpenFile(const char *path, const char *mode);

/*---------------------------------------------------------------------------*
  Name:         FATFS_CloseFile

  Description:  �t�@�C�������

  Arguments:    file : �t�@�C���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CloseFile(FATFSFileHandle file);

/*---------------------------------------------------------------------------*
  Name:         FATFS_ReadFile

  Description:  �t�@�C������f�[�^��ǂݏo��

  Arguments:    file : �t�@�C���n���h��
                buffer : �ǂݏo�����f�[�^�̊i�[��
                length : �ǂݏo���T�C�Y

  Returns:      ���ۂɓǂݏo�����T�C�Y�܂���-1
 *---------------------------------------------------------------------------*/
int FATFS_ReadFile(FATFSFileHandle file, void *buffer, int length);

/*---------------------------------------------------------------------------*
  Name:         FATFS_WriteFile

  Description:  �t�@�C���փf�[�^����������

  Arguments:    file : �t�@�C���n���h��
                buffer : �������ރf�[�^�̊i�[��
                length : �������ރT�C�Y

  Returns:      ���ۂɏ������񂾃T�C�Y�܂���-1
 *---------------------------------------------------------------------------*/
int FATFS_WriteFile(FATFSFileHandle file, const void *buffer, int length);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SetSeekCache

  Description:  �����t�V�[�N�p�ɃL���b�V���o�b�t�@�����蓖��

  Arguments:    file     : �t�@�C���n���h��
                buf      : �L���b�V���o�b�t�@
                buf_size : �L���b�V���o�b�t�@�T�C�Y

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SetSeekCache(FATFSFileHandle file, void* buf, u32 buf_size);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SeekFile

  Description:  �t�@�C���|�C���^���ړ�

  Arguments:    file : �t�@�C���n���h��
                offset : �ړ���
                origin : �ړ���_

  Returns:      �ړ���̃I�t�Z�b�g�܂���-1
 *---------------------------------------------------------------------------*/
int FATFS_SeekFile(FATFSFileHandle file, int offset, FATFSSeekMode origin);

/*---------------------------------------------------------------------------*
  Name:         FATFS_FlushFile

  Description:  �t�@�C���̓��e���h���C�u�փt���b�V��

  Arguments:    file : �t�@�C���n���h��

  Returns:      ����Ƀt���b�V��������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_FlushFile(FATFSFileHandle file);

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetFileLength

  Description:  �t�@�C���T�C�Y���擾

  Arguments:    file : �t�@�C���n���h��

  Returns:      �t�@�C���T�C�Y�܂���-1
 *---------------------------------------------------------------------------*/
int FATFS_GetFileLength(FATFSFileHandle file);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SetFileLength

  Description:  �t�@�C���T�C�Y��ݒ�

  Arguments:    file : �t�@�C���n���h��

  Returns:      �ݒ肪���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SetFileLength(FATFSFileHandle file, int length);

/*---------------------------------------------------------------------------*
  Name:         FATFS_OpenDirectory

  Description:  �w��p�X�̃f�B���N�g�����J��

  Arguments:    path : �p�X��
                mode : �f�B���N�g���̃A�N�Z�X���[�h (���݂͖��������)

  Returns:      �J�����f�B���N�g���̃n���h���܂���NULL
 *---------------------------------------------------------------------------*/
FATFSDirectoryHandle FATFS_OpenDirectory(const char *path, const char *mode);

/*---------------------------------------------------------------------------*
  Name:         FATFS_CloseDirectory

  Description:  �f�B���N�g�������

  Arguments:    dir : �f�B���N�g���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CloseDirectory(FATFSDirectoryHandle dir);

/*---------------------------------------------------------------------------*
  Name:         FATFS_ReadDirectory

  Description:  �f�B���N�g�����玟�̃G���g������ǂݏo��

  Arguments:    dir : �f�B���N�g���n���h��
                info : �G���g�����̊i�[��

  Returns:      ����ɃG���g������ǂݏo������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_ReadDirectory(FATFSDirectoryHandle dir, FATFSFileInfo *info);

/*---------------------------------------------------------------------------*
  Name:         FATFS_FlushAll

  Description:  �S�Ẵh���C�u�̓��e���f�o�C�X�փt���b�V��

  Arguments:    None

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_FlushAll(void);

/*---------------------------------------------------------------------------*
  Name:         FATFS_UnmountAll

  Description:  �S�Ẵh���C�u���A���}�E���g

  Arguments:    None

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_UnmountAll(void);

/*---------------------------------------------------------------------------*
  Name:         FATFS_MountSpecial

  Description:  ����h���C�u���}�E���g

  Arguments:    param   : �}�E���g�Ώۂ��w�肷��p�����[�^
                arcname : �}�E���g����A�[�J�C�u��
                          "otherPub", "otherPrv", "share", ���w��\�B
                          NULL���w�肷��ƈȑO�̃h���C�u���A���}�E���g
                slot    : ���蓖�Ă�ꂽ�C���f�b�N�X���i�[���Ă����o�b�t�@�B
                          �}�E���g���ɐݒ肳��A�A���}�E���g���ɎQ�Ƃ����

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_MountSpecial(u64 param, const char *arcname, int *slot);

/*---------------------------------------------------------------------------*
  Name:         FATFS_FormatSpecial

  Description:  ���g�����L����������h���C�u���ď������B

  Arguments:    path : �h���C�u�����܂񂾃p�X��

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_FormatSpecial(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SetLatencyEmulation

  Description:  �򉻂����f�o�C�X�ւ̃A�N�Z�X���ɐ�����E�F�C�g���Ԃ�
                �h���C�o�w�ŋ[���I�ɍČ�������悤�w���B

  Arguments:    enable : �L���ɂ���ꍇ��TRUE�B

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SetLatencyEmulation(BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SearchWildcard

  Description:  SD�J�[�h�̏���f�B���N�g���������C���h�J�[�h�Ō����B

  Arguments:    directory : 8�����ȓ��̔C�ӂ̃f�B���N�g�����B
                            �A�v���̃��[�g�f�B���N�g��������
                            �t�@�C���������o���Ȃ��NULL�B
                prefix    : �t�@�C�����̃v���t�B�b�N�X��(1�`5����)
                suffix    : �t�@�C���̊g���q��(1�`3����)
                buffer    : �����Ƀ}�b�`���������t�@�C�����L�^����o�b�t�@�B
                            (buffer[i / 8] & (1 << (i % 8))) ���^�Ȃ��
                            "prefix{i}.suffix" �����݂��邱�Ƃ������B
                length    : buffer�̒����B

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SearchWildcard(const char* directory, const char *prefix, const char *suffix,
                          void *buffer, u32 length);

/*---------------------------------------------------------------------------*
 * Unicode�Ή���
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetDriveResourceW

  Description:  �f�B�X�N�̋󂫗e�ʂ��擾

  Arguments:    path     : �h���C�u�����܂񂾃p�X��
                resource : ���ʂ��i�[����FATFSDriveResource�\����

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_GetDriveResourceW(const u16 *path, FATFSDriveResource *resource);

/*---------------------------------------------------------------------------*
  Name:         FATFS_GetFileInfoW

  Description:  �t�@�C���܂��̓f�B���N�g���̏����擾

  Arguments:    path : �p�X��
                info : ���ʂ��i�[����FATFSFileInfoW�\����

  Returns:      �w��̃p�X�����݂���Ώ����擾����TRUE�A���݂��Ȃ����FALSE
 *---------------------------------------------------------------------------*/
BOOL FATFS_GetFileInfoW(const u16 *path, FATFSFileInfoW *info);

/*---------------------------------------------------------------------------*
  Name:         FATFS_SetFileInfoW

  Description:  �t�@�C���܂��̓f�B���N�g���̏���ύX

  Arguments:    path : �p�X��
                info : �ύX���ׂ���񂪊i�[���ꂽFATFSFileInfoW�\����

  Returns:      �w��̃p�X�����݂���Ώ���ύX����TRUE�A���݂��Ȃ����FALSE
 *---------------------------------------------------------------------------*/
BOOL FATFS_SetFileInfoW(const u16 *path, const FATFSFileInfoW *info);

/*---------------------------------------------------------------------------*
  Name:         FATFS_CreateFileW

  Description:  �w��̃p�X�Ƀt�@�C���𐶐�

  Arguments:    path : �p�X��
                trunc : ���łɑ��݂���t�@�C����j������Ȃ�TRUE
                permit : �t�@�C���̃A�N�Z�X�� (rwx){1,3}

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CreateFileW(const u16 *path, BOOL trunc, const char *permit);

/*---------------------------------------------------------------------------*
  Name:         FATFS_DeleteFileW

  Description:  �w��̃t�@�C�����폜

  Arguments:    path : �p�X��

  Returns:      �폜�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_DeleteFileW(const u16 *path);

/*---------------------------------------------------------------------------*
  Name:         FATFS_RenameFileW

  Description:  �w��̃t�@�C��������

  Arguments:    path : �p�X��
                newpath : �V�����p�X��

  Returns:      �����ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_RenameFileW(const u16 *path, const u16 *newpath);

/*---------------------------------------------------------------------------*
  Name:         FATFS_CreateDirectoryW

  Description:  �w��̃p�X�Ƀf�B���N�g���𐶐�

  Arguments:    path : �p�X��
                permit : �t�@�C���̃A�N�Z�X�� (rwx){1,3}

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_CreateDirectoryW(const u16 *path, const char *permit);

/*---------------------------------------------------------------------------*
  Name:         FATFS_DeleteDirectoryW

  Description:  �w��̃f�B���N�g�����폜

  Arguments:    path : �p�X��

  Returns:      �폜�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_DeleteDirectoryW(const u16 *path);

/*---------------------------------------------------------------------------*
  Name:         FATFS_RenameDirectoryW

  Description:  �w��̃f�B���N�g��������

  Arguments:    path : �p�X��
                newpath : �V�����p�X��

  Returns:      �����ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_RenameDirectoryW(const u16 *path, const u16 *newpath);

/*---------------------------------------------------------------------------*
  Name:         FATFS_OpenFileW

  Description:  �w��p�X�̃t�@�C�����J��

  Arguments:    path : �p�X��
                mode : �t�@�C���̃A�N�Z�X���[�h ("r/w/r+/w+" + "b/t")

  Returns:      �J�����t�@�C���̃n���h���܂���NULL
 *---------------------------------------------------------------------------*/
FATFSFileHandle FATFS_OpenFileW(const u16 *path, const char *mode);

/*---------------------------------------------------------------------------*
  Name:         FATFS_OpenDirectoryW

  Description:  �w��p�X�̃f�B���N�g�����J��

  Arguments:    path : �p�X��
                mode : �f�B���N�g���̃A�N�Z�X���[�h (���݂͖��������)

  Returns:      �J�����f�B���N�g���̃n���h���܂���NULL
 *---------------------------------------------------------------------------*/
FATFSDirectoryHandle FATFS_OpenDirectoryW(const u16 *path, const char *mode);

/*---------------------------------------------------------------------------*
  Name:         FATFS_ReadDirectoryW

  Description:  �f�B���N�g�����玟�̃G���g������ǂݏo��

  Arguments:    dir : �f�B���N�g���n���h��
                info : �G���g�����̊i�[��

  Returns:      ����ɃG���g������ǂݏo������TRUE
 *---------------------------------------------------------------------------*/
BOOL FATFS_ReadDirectoryW(FATFSDirectoryHandle dir, FATFSFileInfoW *info);


/*---------------------------------------------------------------------------*
 * internal functions
 *---------------------------------------------------------------------------*

 /*---------------------------------------------------------------------------*
  Name:         FATFSiArcnameList

  Description:  ARM9����Q�Ɖ\�ȃA�[�J�C�u���̃��X�g��ێ�����ÓI�ȃo�b�t�@�B
                ARM9/ARM7�Ƃ��ɎQ�Ɖ\�ȃ������ł���K�v������A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̕ϐ���FATFS_Init�֐��̌Ăяo�����O�ɕύX���邱�Ƃ�
                �K�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      ARM9����Q�Ɖ\�ȃA�[�J�C�u���̃��X�g��ێ�����ÓI�ȃo�b�t�@
 *---------------------------------------------------------------------------*/
extern char *FATFSiArcnameList/* [MATH_ROUNDUP(OS_MOUNT_ARCHIVE_NAME_LEN * OS_MOUNT_INFO_MAX + 1, 32)] ATTRIBUTE_ALIGN(32) */;

/*---------------------------------------------------------------------------*
  Name:         FATFSiCommandBuffer

  Description:  ���N�G�X�g���s�p�̐ÓI�ȃR�}���h�o�b�t�@�ւ̃|�C���^�B
                ARM9/ARM7�Ƃ��ɎQ�Ɖ\�ȃ������ł���K�v������A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̕ϐ���FATFS_Init�֐��̌Ăяo�����O�ɕύX���邱�Ƃ�
                �K�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FATFS_COMMAND_BUFFER_MAX�o�C�g�̐ÓI�ȃR�}���h�o�b�t�@
 *---------------------------------------------------------------------------*/
extern u8 *FATFSiCommandBuffer/* [FATFS_COMMAND_BUFFER_MAX] ATTRIBUTE_ALIGN(32)*/;

/*---------------------------------------------------------------------------*
  Name:         FATFSi_GetUnicodeConversionTable

  Description:  ARM7�ɋ��L�����邽�߂�Unicode�ϊ��e�[�u�����擾����B
                ASCII�p�X����������Ȃ����Ƃ����炩�ȃA�v���P�[�V�����ł�
                ���̊֐����I�[�o�[���C�h���Ė��������邱�Ƃɂ��
                80kB���Unicode�ϊ��e�[�u�����폜���邱�Ƃ��ł���B

  Arguments:    u2s : Unicode����Shift_JIS�ւ̕ϊ��e�[�u��
                s2u : Shift_JIS����Unicode�ւ̕ϊ��e�[�u��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FATFSi_GetUnicodeConversionTable(const u8 **u2s, const u16 **s2u);

/*---------------------------------------------------------------------------*
  Name:         FATFSi_SetNdmaParameters

  Description:  ARM7��FAT�h���C�o���g�p����NDMA�ݒ�𒼐ڕύX����B

  Arguments:    ndmaNo        : �ݒ��ύX����NDMA�`�����l�� (0-3)
                blockWord     : �u���b�N�]�����[�h�� (MI_NDMA_BWORD_1-MI_NDMA_BWORD_32768)
                intervalTimer : �C���^�[�o�� (0x0000-0xFFFF)
                prescaler     : �v���X�P�[�� (MI_NDMA_INTERVAL_PS_1-MI_NDMA_INTERVAL_PS_64)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FATFSi_SetNdmaParameters(u32 ndmaNo, u32 blockWord, u32 intervalTimer, u32 prescaler);

/*---------------------------------------------------------------------------*
  Name:         FATFSi_SetRequestBuffer

  Description:  ���ɔ��s����R�}���h��񓯊������֕ύX���邽�߂�
                ���N�G�X�g�o�b�t�@��ݒ肷��B

  Arguments:    buffer : ���N�G�X�g�Ǘ��Ɏg�p����FATFSRequestBuffer�\���́B
                         32�o�C�g�̐����{�ŋ��E�������Ă��Ȃ���΂Ȃ�Ȃ�9�B

                callback : �R�}���h�������ɌĂяo�����R�[���o�b�N�B

                userdata : ���N�G�X�g�o�b�t�@�Ɋ֘A�t����C�ӂ̃��[�U��`�l�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FATFSi_SetRequestBuffer(FATFSRequestBuffer *buffer, void (*callback)(FATFSRequestBuffer *), void *userdata);


#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_FATFS_API_H_ */
#endif
