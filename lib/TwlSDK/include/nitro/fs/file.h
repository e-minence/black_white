/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - include
  File:     file.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-05#$
  $Rev: 9524 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_FILE_H_)
#define NITRO_FS_FILE_H_


#include <nitro/fs/archive.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

// �t�@�C���\����
typedef struct FSFile
{
// private:
    struct FSFile              *next;
    void                       *userdata;
    struct FSArchive           *arc;
    u32                         stat;
    void                       *argument;
    FSResult                    error;
    OSThreadQueue               queue[1];

    union
    {
        u8                      reserved1[16];
        FSROMFATProperty        prop;
    };
    union
    {
        u8                      reserved2[24];
        FSROMFATCommandInfo     arg;
    };
}
FSFile;


SDK_COMPILER_ASSERT(sizeof(FSFile) == 72);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FS_InitFile

  Description:  FSFile�\���̂�������

  Arguments:    file        FSFile�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_InitFile(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_GetAttachedArchive

  Description:  �t�@�C����f�B���N�g���Ɋ֘A�t����ꂽ�A�[�J�C�u���擾

  Arguments:    file        �t�@�C���n���h���܂��̓f�B���N�g���n���h��

  Returns:      �n���h����ێ����Ă���΂��̃A�[�J�C�u�A�����łȂ����NULL
 *---------------------------------------------------------------------------*/
SDK_INLINE FSArchive *FS_GetAttachedArchive(const FSFile *file)
{
    return file->arc;
}
/*---------------------------------------------------------------------------*
  Name:         FS_IsBusy

  Description:  �t�@�C�����R�}���h������������

  Arguments:    file        �R�}���h�𔭍s����FSFile�\����

  Returns:      �R�}���h�������Ȃ�TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsBusy(volatile const FSFile *file)
{
    return ((file->stat & FS_FILE_STATUS_BUSY) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsCanceling

  Description:  �t�@�C�����R�}���h�L�����Z����������

  Arguments:    file        �R�}���h�𔭍s����FSFile�\����

  Returns:      �R�}���h�L�����Z�����Ȃ�TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsCanceling(volatile const FSFile *file)
{
    return ((file->stat & FS_FILE_STATUS_CANCEL) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsSucceeded

  Description:  �ŐV�̃R�}���h���������Đ�������������

  Arguments:    file        �R�}���h�𔭍s����FSFile�\����

  Returns:      �R�}���h�����������ł����TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsSucceeded(volatile const FSFile *file)
{
    return (file->error == FS_RESULT_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsFile

  Description:  �I�[�v�����ꂽ�t�@�C���n���h��������

  Arguments:    file        FSFile�\����

  Returns:      �t�@�C���n���h����ێ����Ă����TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsFile(volatile const FSFile *file)
{
    return ((file->stat & FS_FILE_STATUS_IS_FILE) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsDir

  Description:  �I�[�v�����ꂽ�f�B���N�g���n���h��������

  Arguments:    file        FSFile�\����

  Returns:      �f�B���N�g���n���h����ێ����Ă����TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsDir(volatile const FSFile *file)
{
    return ((file->stat & FS_FILE_STATUS_IS_DIR) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetResultCode

  Description:  �ŐV�̃R�}���h�̌��ʒl���擾

  Arguments:    file        FSFile�\����

  Returns:      �ŐV�̃R�}���h�̌��ʒl
 *---------------------------------------------------------------------------*/
SDK_INLINE FSResult FS_GetResultCode(volatile const FSFile *file)
{
    return file->error;
}

/*---------------------------------------------------------------------------*
  Name:         FS_WaitAsync

  Description:  �񓯊��R�}���h�̊����܂Ńu���b�L���O����

  Arguments:    file        �t�@�C���n���h��

  Returns:      �R�}���h�����������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_WaitAsync(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_CancelFile

  Description:  �񓯊��R�}���h�̃L�����Z����v������

  Arguments:    file        �t�@�C���n���h��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_CancelFile(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_SetFileHandle

  Description:  �t�@�C���\���̂��t�@�C���n���h���Ƃ��ď������B
                ���̊֐���OpenFile�R�}���h�ɑΉ����邽�߂�
                �A�[�J�C�u����������Ăяo���B

  Arguments:    file             FSFile�\����
                arc              �֘A�t����A�[�J�C�u
                userdata         �t�@�C���n���h������ێ�����
                                 �C�ӂ̃A�[�J�C�u��`�f�[�^

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void FS_SetFileHandle(FSFile *file, FSArchive *arc, void *userdata)
{
    file->stat |= FS_FILE_STATUS_IS_FILE;
    file->stat &= ~FS_FILE_STATUS_IS_DIR;
    file->arc = arc;
    file->userdata = userdata;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetDirectoryHandle

  Description:  �t�@�C���\���̂��f�B���N�g���n���h���Ƃ��ď������B
                ���̊֐���OpenDirectory�R�}���h�ɑΉ����邽�߂�
                �A�[�J�C�u����������Ăяo���B

  Arguments:    file             FSFile�\����
                arc              �֘A�t����A�[�J�C�u
                userdata         �f�B���N�g���n���h������ێ�����
                                 �C�ӂ̃A�[�J�C�u��`�f�[�^

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void FS_SetDirectoryHandle(FSFile *file, FSArchive *arc, void *userdata)
{
    file->stat |= FS_FILE_STATUS_IS_DIR;
    file->stat &= ~FS_FILE_STATUS_IS_FILE;
    file->arc = arc;
    file->userdata = userdata;
}

/*---------------------------------------------------------------------------*
  Name:         FS_DetachHandle

  Description:  �t�@�C���\���̂𖳌��ȃn���h���Ƃ��ď������B
                ���̊֐���CloseFile�����CloseDirectory�R�}���h�ɑΉ����邽�߂�
                �A�[�J�C�u����������Ăяo���B

  Arguments:    file             FSFile�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void FS_DetachHandle(FSFile *file)
{
    file->userdata = NULL;
    file->stat &= ~(FS_FILE_STATUS_IS_FILE | FS_FILE_STATUS_IS_DIR);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileUserData

  Description:  �t�@�C���\���̂Ɋ֘A�t����ꂽ�A�[�J�C�u��`�f�[�^���擾�B
                �A�[�J�C�u����������Ăяo���B

  Arguments:    file             FSFile�\����

  Returns:      �t�@�C���\���̂Ɋ֘A�t����ꂽ�A�[�J�C�u��`�f�[�^
 *---------------------------------------------------------------------------*/
SDK_INLINE void* FS_GetFileUserData(const FSFile *file)
{
    return file->userdata;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathName

  Description:  �w�肵���t�@�C���܂��̓f�B���N�g���̃t���p�X�����擾

  Arguments:    file        �t�@�C���܂��̓f�B���N�g���n���h��
                buffer      �p�X���̊i�[��
                length      �o�b�t�@�T�C�Y

  Returns:      �I�[��'\0'���܂߂Đ������擾�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_GetPathName(FSFile *file, char *buffer, u32 length);

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathLength

  Description:  �w�肵���t�@�C���܂��̓f�B���N�g���̃t���p�X���̒������擾

  Arguments:    file        �t�@�C���܂��̓f�B���N�g���n���h��

  Returns:      ��������ΏI�[��'\0'���܂߂��p�X���̒����A���s�����-1
 *---------------------------------------------------------------------------*/
s32     FS_GetPathLength(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFile

  Description:  �t�@�C���𐶐�����

  Arguments:    path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      �t�@�C��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_CreateFile(const char *path, u32 permit);

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteFile

  Description:  �t�@�C�����폜����

  Arguments:    path        �p�X��

  Returns:      �t�@�C��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_DeleteFile(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_RenameFile

  Description:  �t�@�C������ύX����

  Arguments:    src         �ϊ����̃t�@�C����
                dst         �ϊ���̃t�@�C����

  Returns:      �t�@�C����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_RenameFile(const char *src, const char *dst);

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathInfo

  Description:  �t�@�C���܂��̓f�B���N�g���̏����擾����

  Arguments:    path        �p�X��
                info        ���̊i�[��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
BOOL    FS_GetPathInfo(const char *path, FSPathInfo *info);

/*---------------------------------------------------------------------------*
  Name:         FS_SetPathInfo

  Description:  �t�@�C���܂��̓f�B���N�g���̏���ݒ肷��

  Arguments:    path        �p�X��
                info        ���̊i�[��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
BOOL    FS_SetPathInfo(const char *path, const FSPathInfo *info);

/*---------------------------------------------------------------------------*
  Name:         FS_CreateDirectory

  Description:  �f�B���N�g���𐶐�����

  Arguments:    path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      �f�B���N�g��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_CreateDirectory(const char *path, u32 permit);

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteDirectory

  Description:  �f�B���N�g�����폜����

  Arguments:    path        �p�X��

  Returns:      �f�B���N�g��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_DeleteDirectory(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_RenameDirectory

  Description:  �f�B���N�g������ύX����

  Arguments:    src         �ϊ����̃f�B���N�g����
                dst         �ϊ���̃f�B���N�g����

  Returns:      �f�B���N�g����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_RenameDirectory(const char *src, const char *dst);

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăt�@�C���𐶐�����

  Arguments:    path        �p�X��
                permit      �A�N�Z�X���[�h

  Returns:      �t�@�C��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CreateFileAuto(const char *path, u32 permit);

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteFileAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăt�@�C�����폜����

  Arguments:    path        �p�X��

  Returns:      �t�@�C��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_DeleteFileAuto(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_RenameFileAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăt�@�C������ύX����

  Arguments:    src         �ϊ����̃t�@�C����
                dst         �ϊ���̃t�@�C����

  Returns:      �t�@�C����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RenameFileAuto(const char *src, const char *dst);

/*---------------------------------------------------------------------------*
  Name:         FS_CreateDirectoryAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăf�B���N�g���𐶐�����

  Arguments:    path        ��������f�B���N�g����
                permit      �A�N�Z�X���[�h

  Returns:      �f�B���N�g��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CreateDirectoryAuto(const char *path, u32 permit);

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteDirectoryAuto

  Description:  �f�B���N�g�����ċA�I�ɍ폜����

  Arguments:    path        �p�X��

  Returns:      �f�B���N�g��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_DeleteDirectoryAuto(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_RenameDirectoryAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g���������������ăf�B���N�g������ύX����

  Arguments:    src         �ϊ����̃f�B���N�g����
                dst         �ϊ���̃f�B���N�g����

  Returns:      �f�B���N�g����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RenameDirectoryAuto(const char *src, const char *dst);

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveResource

  Description:  �w�肵���A�[�J�C�u�̃��\�[�X�����擾����

  Arguments:    path        �A�[�J�C�u�����ł���p�X��
                resource    �擾�������\�[�X���̊i�[��

  Returns:      ���\�[�X��񂪐���Ɏ擾�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_GetArchiveResource(const char *path, FSArchiveResource *resource);

/*---------------------------------------------------------------------------*
  Name:         FSi_GetSpaceToCreateDirectoryEntries

  Description:  �t�@�C���̐����Ɠ����ɐ��������f�B���N�g���G���g���̊i�[�ɕK�v�ȗe�ʂ����ς���
                (�p�X���ɑ��݂���f�B���N�g�����V�K�ɍ쐬���邱�Ƃ�z�肷��)

  Arguments:    path                �����������t�@�C���̃p�X���B
                bytesPerCluster     �t�@�C���V�X�e����̃N���X�^������̃o�C�g���B

  Returns:      �f�B���N�g���G���g���̊i�[�ɕK�v�ȗe��
 *---------------------------------------------------------------------------*/
u32  FSi_GetSpaceToCreateDirectoryEntries(const char *path, const u32 bytesPerCluster);

/*---------------------------------------------------------------------------*
  Name:         FS_HasEnoughSpaceToCreateFile

  Description:  �w�肵���p�X���ƃT�C�Y�����t�@�C�������ݐ����\�����肷��

  Arguments:    resource    ���O��FS_GetArchiveResource�֐��Ŏ擾�����A�[�J�C�u���B
                            �֐�����������ƁA�t�@�C��������镪�����e�T�C�Y����������B
                path        �����������t�@�C���̃p�X���B
                size        �����������t�@�C���̃T�C�Y�B

  Returns:      �w�肵���p�X���ƃT�C�Y�����t�@�C���������\�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_HasEnoughSpaceToCreateFile(FSArchiveResource *resource, const char *path, u32 size);

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveReady

  Description:  �w�肵���A�[�J�C�u�����ݎg�p�\�����肷��

  Arguments:    path        "�A�[�J�C�u��:" �Ŏn�܂��΃p�X

  Returns:      �w�肵���A�[�J�C�u�����ݎg�p�\�ł����TRUE�B
                �X���b�g�ɑ}������Ă��Ȃ�SD�������J�[�h�A�[�J�C�u��
                �܂��C���|�[�g����Ă��Ȃ��Z�[�u�f�[�^�A�[�J�C�u�Ȃǂ�FALSE�B
 *---------------------------------------------------------------------------*/
BOOL FS_IsArchiveReady(const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileEx

  Description:  �p�X�����w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileEx(FSFile *file, const char *path, u32 mode);

/*---------------------------------------------------------------------------*
  Name:         FS_ConvertPathToFileID

  Description:  �p�X������t�@�C��ID���擾����

  Arguments:    p_fileid    FSFileID�̊i�[��
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_ConvertPathToFileID(FSFileID *p_fileid, const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileFast

  Description:  �t�@�C��ID���w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                fileid      FS_ReadDir�֐��Ȃǂœ�����t�@�C��ID

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_OpenFileFast(FSFile *file, FSFileID fileid);

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileDirect

  Description:  �A�[�J�C�u���̃I�t�Z�b�g�𒼐ڃ}�b�s���O���ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                arc         �t�@�C�����}�b�v����A�[�J�C�u
                top         �t�@�C���C���[�W�̐擪�I�t�Z�b�g
                bottom      �t�@�C���C���[�W�̏I�[�I�t�Z�b�g
                fileid      ���蓖�Ă�C�ӂ̃t�@�C��ID

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_OpenFileDirect(FSFile *file, FSArchive *arc,
                          u32 top, u32 bottom, u32 fileid);

/*---------------------------------------------------------------------------*
  Name:         FS_CloseFile

  Description:  �t�@�C���n���h�������

  Arguments:    file        FSFile�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_CloseFile(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileLength

  Description:  �t�@�C���T�C�Y���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���T�C�Y
 *---------------------------------------------------------------------------*/
u32     FS_GetFileLength(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_SetFileLength

  Description:  �t�@�C���T�C�Y��ύX����

  Arguments:    file        �t�@�C���n���h��
                length      �ύX��̃T�C�Y

  Returns:      ��������
 *---------------------------------------------------------------------------*/
FSResult FS_SetFileLength(FSFile *file, u32 length);

/*---------------------------------------------------------------------------*
  Name:         FS_GetFilePosition

  Description:  �t�@�C���|�C���^�̌��݈ʒu���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���|�C���^�̌��݈ʒu
 *---------------------------------------------------------------------------*/
u32     FS_GetFilePosition(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_GetSeekCacheSize

  Description:  �����t�V�[�N�p�̃t���L���b�V���ɕK�v�ȃo�b�t�@�T�C�Y�����߂�

  Arguments:    path

  Returns:      ��������΃T�C�Y�A���s�����0
 *---------------------------------------------------------------------------*/
u32 FS_GetSeekCacheSize( const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_SetSeekCache

  Description:  �����t�V�[�N�p�̃L���b�V���o�b�t�@�����蓖�Ă�

  Arguments:    file        �t�@�C���n���h��
                buf         �L���b�V���o�b�t�@
                buf_size    �L���b�V���o�b�t�@�T�C�Y

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_SetSeekCache(FSFile *file, void* buf, u32 buf_size);

/*---------------------------------------------------------------------------*
  Name:         FS_SeekFile

  Description:  �t�@�C���|�C���^���ړ�����

  Arguments:    file        �t�@�C���n���h��
                offset      �ړ���
                origin      �ړ��̋N�_

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_SeekFile(FSFile *file, s32 offset, FSSeekFileMode origin);

/*---------------------------------------------------------------------------*
  Name:         FS_SeekFileToBegin

  Description:  �t�@�C���|�C���^���t�@�C���擪�܂ňړ�����

  Arguments:    file        �t�@�C���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_SeekFileToBegin(FSFile *file)
{
    return FS_SeekFile(file, 0, FS_SEEK_SET);
}

/*---------------------------------------------------------------------------*
  Name:         FS_SeekFileToEnd

  Description:  �t�@�C���|�C���^���t�@�C���I�[�܂ňړ�����

  Arguments:    file        �t�@�C���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_SeekFileToEnd(FSFile *file)
{
    return FS_SeekFile(file, 0, FS_SEEK_END);
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadFile

  Description:  �t�@�C������f�[�^��ǂݏo��

  Arguments:    file        �t�@�C���n���h��
                buffer      �]����o�b�t�@
                length      �ǂݏo���T�C�Y

  Returns:      ��������Ύ��ۂɓǂݏo�����T�C�Y�A���s�����-1
 *---------------------------------------------------------------------------*/
s32     FS_ReadFile(FSFile *file, void *buffer, s32 length);

/*---------------------------------------------------------------------------*
  Name:         FS_ReadFileAsync

  Description:  �t�@�C������f�[�^��񓯊��I�ɓǂݏo��

  Arguments:    file        �t�@�C���n���h��
                buffer      �]����o�b�t�@
                length      �ǂݏo���T�C�Y

  Returns:      ��������ΒP��length�Ɠ����l�A���s�����-1
 *---------------------------------------------------------------------------*/
s32     FS_ReadFileAsync(FSFile *file, void *buffer, s32 length);

/*---------------------------------------------------------------------------*
  Name:         FS_WriteFile

  Description:  �t�@�C���փf�[�^����������

  Arguments:    file        �t�@�C���n���h��
                buffer      �]�����o�b�t�@
                length      �������݃T�C�Y

  Returns:      ��������Ύ��ۂɏ������񂾃T�C�Y�A���s�����-1
 *---------------------------------------------------------------------------*/
s32     FS_WriteFile(FSFile *file, const void *buffer, s32 length);

/*---------------------------------------------------------------------------*
  Name:         FS_WriteFileAsync

  Description:  �t�@�C���փf�[�^��񓯊��I�ɏ�������

  Arguments:    file        �t�@�C���n���h��
                buffer      �]�����o�b�t�@
                length      �������݃T�C�Y

  Returns:      ��������ΒP��length�Ɠ����l�A���s�����-1
 *---------------------------------------------------------------------------*/
s32     FS_WriteFileAsync(FSFile *file, const void *buffer, s32 length);

/*---------------------------------------------------------------------------*
  Name:         FS_FlushFile

  Description:  �t�@�C���̕ύX���e���f�o�C�X�֔��f����

  Arguments:    file        �t�@�C���n���h��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
FSResult FS_FlushFile(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_OpenDirectory

  Description:  �f�B���N�g���n���h�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_OpenDirectory(FSFile *file, const char *path, u32 mode);

/*---------------------------------------------------------------------------*
  Name:         FS_CloseDirectory

  Description:  �f�B���N�g���n���h�������

  Arguments:    file        FSFile�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_CloseDirectory(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDirectory

  Description:  �f�B���N�g���̃G���g����1�����ǂݐi�߂�

  Arguments:    file        FSFile�\����
                info        FSDirectoryEntryInfo�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_ReadDirectory(FSFile *file, FSDirectoryEntryInfo *info);

/*---------------------------------------------------------------------------*
  Name:         FS_TellDir

  Description:  �f�B���N�g���n���h�����猻�݂̃f�B���N�g���ʒu���擾

  Arguments:    dir         �f�B���N�g���n���h��
                pos         �f�B���N�g���ʒu�̊i�[��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_TellDir(const FSFile *dir, FSDirPos *pos);

/*---------------------------------------------------------------------------*
  Name:         FS_SeekDir

  Description:  �f�B���N�g���n���h�����f�B���N�g���ʒu�Ŏw�肵�ĊJ��

  Arguments:    dir         �f�B���N�g���n���h��
                pos         �f�B���N�g���ʒu

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_SeekDir(FSFile *p_dir, const FSDirPos *p_pos);

/*---------------------------------------------------------------------------*
  Name:         FS_RewindDir

  Description:  �f�B���N�g���n���h���̗񋓈ʒu��擪�֖߂�

  Arguments:    dir         �f�B���N�g���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_RewindDir(FSFile *dir);


/*---------------------------------------------------------------------------*
 * Unicode support
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileExW

  Description:  �p�X�����w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileExW(FSFile *file, const u16 *path, u32 mode);

/*---------------------------------------------------------------------------*
  Name:         FS_OpenDirectoryW

  Description:  �f�B���N�g���n���h�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenDirectoryW(FSFile *file, const u16 *path, u32 mode);

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDirectoryW

  Description:  �f�B���N�g���̃G���g����1�����ǂݐi�߂�

  Arguments:    file        FSFile�\����
                info        FSDirectoryEntryInfo�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_ReadDirectoryW(FSFile *file, FSDirectoryEntryInfoW *info);


/*---------------------------------------------------------------------------*
 * obsolete functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFile

  Description:  �p�X�����w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_OpenFile(FSFile *file, const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_GetLength

  Description:  �t�@�C���T�C�Y���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���T�C�Y
 *---------------------------------------------------------------------------*/
u32     FS_GetLength(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_GetPosition

  Description:  �t�@�C���|�C���^�̌��݈ʒu���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���|�C���^�̌��݈ʒu
 *---------------------------------------------------------------------------*/
u32     FS_GetPosition(FSFile *file);

/*---------------------------------------------------------------------------*
  Name:         FS_FindDir

  Description:  �f�B���N�g���n���h�����J��

  Arguments:    dir         FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_FindDir(FSFile *dir, const char *path);

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDir

  Description:  �f�B���N�g���n���h���̃G���g������1�����ǂݐi�߂�

  Arguments:    dir         �f�B���N�g���n���h��
                entry       �G���g�����̊i�[��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_ReadDir(FSFile *dir, FSDirEntry *entry);

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileInfo

  Description:  �t�@�C���܂��̓f�B���N�g���̏����擾����

  Arguments:    path        �p�X��
                info        ���̊i�[��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
typedef FSPathInfo  FSFileInfo;
FSResult FS_GetFileInfo(const char *path, FSFileInfo *info);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_FILE_H_ */
