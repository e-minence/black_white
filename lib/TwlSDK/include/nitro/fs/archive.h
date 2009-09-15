/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     archive.h

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


#ifndef NITRO_FS_ARCHIVE_H_
#define NITRO_FS_ARCHIVE_H_


#include <nitro/fs/types.h>
#include <nitro/fs/romfat.h>
#include <nitro/os/common/thread.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

// �A�[�J�C�u�R�}���h�C���^�t�F�[�X
typedef struct FSArchiveInterface
{
    // ���d�l�ƌ݊����̂���R�}���h
    FSResult (*ReadFile)(struct FSArchive*, struct FSFile*, void *buffer, u32 *length);
    FSResult (*WriteFile)(struct FSArchive*, struct FSFile*, const void *buffer, u32 *length);
    FSResult (*SeekDirectory)(struct FSArchive*, struct FSFile*, u32 id, u32 position);
    FSResult (*ReadDirectory)(struct FSArchive*, struct FSFile*, FSDirectoryEntryInfo *info);
    FSResult (*FindPath)(struct FSArchive*, u32 base_dir_id, const char *path, u32 *target_id, BOOL target_is_directory);
    FSResult (*GetPath)(struct FSArchive*, struct FSFile*, BOOL is_directory, char *buffer, u32 *length);
    FSResult (*OpenFileFast)(struct FSArchive*, struct FSFile*, u32 id, u32 mode);
    FSResult (*OpenFileDirect)(struct FSArchive*, struct FSFile*, u32 top, u32 bottom, u32 *id);
    FSResult (*CloseFile)(struct FSArchive*, struct FSFile*);
    void (*Activate)(struct FSArchive*);
    void (*Idle)(struct FSArchive*);
    void (*Suspend)(struct FSArchive*);
    void (*Resume)(struct FSArchive*);
    // ���d�l�ƌ݊��������邪�R�}���h�ł͂Ȃ���������
    FSResult (*OpenFile)(struct FSArchive*, struct FSFile*, u32 base_dir_id, const char *path, u32 mode);
    FSResult (*SeekFile)(struct FSArchive*, struct FSFile*, int *offset, FSSeekFileMode from);
    FSResult (*GetFileLength)(struct FSArchive*, struct FSFile*, u32 *length);
    FSResult (*GetFilePosition)(struct FSArchive*, struct FSFile*, u32 *position);
    // �V�d�l�Ŋg�����ꂽ�R�}���h
    void (*Mount)(struct FSArchive*);
    void (*Unmount)(struct FSArchive*);
    FSResult (*GetArchiveCaps)(struct FSArchive*, u32 *caps);
    FSResult (*CreateFile)(struct FSArchive*, u32 baseid, const char *relpath, u32 permit);
    FSResult (*DeleteFile)(struct FSArchive*, u32 baseid, const char *relpath);
    FSResult (*RenameFile)(struct FSArchive*, u32 baseid_src, const char *relpath_src, u32 baseid_dst, const char *relpath_dst);
    FSResult (*GetPathInfo)(struct FSArchive*, u32 baseid, const char *relpath, FSPathInfo *info);
    FSResult (*SetPathInfo)(struct FSArchive*, u32 baseid, const char *relpath, FSPathInfo *info);
    FSResult (*CreateDirectory)(struct FSArchive*, u32 baseid, const char *relpath, u32 permit);
    FSResult (*DeleteDirectory)(struct FSArchive*, u32 baseid, const char *relpath);
    FSResult (*RenameDirectory)(struct FSArchive*, u32 baseid, const char *relpath_src, u32 baseid_dst, const char *relpath_dst);
    FSResult (*GetArchiveResource)(struct FSArchive*, FSArchiveResource *resource);
    void   *unused_29;
    FSResult (*FlushFile)(struct FSArchive*, struct FSFile*);
    FSResult (*SetFileLength)(struct FSArchive*, struct FSFile*, u32 length);
    FSResult (*OpenDirectory)(struct FSArchive*, struct FSFile*, u32 base_dir_id, const char *path, u32 mode);
    FSResult (*CloseDirectory)(struct FSArchive*, struct FSFile*);
    FSResult (*SetSeekCache)(struct FSArchive*, struct FSFile*, void* buf, u32 buf_size);
    // �����g�����̗\��B
    u8      reserved[116];
}
FSArchiveInterface;

SDK_COMPILER_ASSERT(sizeof(FSArchiveInterface) == 256);


typedef struct FSArchive
{
// private:

    // �A�[�J�C�u�����ʂ����ӂ̖��O�B
    // �p����1�`3�����ő召��������ʂ��Ȃ��B
    union
    {
        char    ptr[FS_ARCHIVE_NAME_LEN_MAX + 1];
        u32     pack;
    }
    name;
    struct FSArchive   *next;       // �A�[�J�C�u�o�^���X�g
    struct FSFile      *list;       // �����҂��̃R�}���h���X�g
    OSThreadQueue       queue;      // �C�x���g�ҋ@�p�̔ėp�L���[
    u32                 flag;       // ������ԃt���O (FS_ARCHIVE_FLAG_*)
    FSCommandType       command;    // �ŐV�̃R�}���h
    FSResult            result;     // �ŐV�̏�������
    void               *userdata;   // ���[�U��`�|�C���^
    const FSArchiveInterface *vtbl; // �R�}���h�C���^�t�F�[�X

    union
    {
        // ���[�U��`�̃A�[�J�C�u�Ŏ��R�ɗ��p�ł�����
        u8      reserved2[52];
        // ROMFAT�^�A�[�J�C�u�Ŏg�p������ (�݊����̂��߂ɕێ�)
        struct  FS_ROMFAT_CONTEXT_DEFINITION();
    };
}
FSArchive;

// NITRO-SDK�Ƃ̌݊������d�����ē��ʂ̓T�C�Y�ύX����
SDK_COMPILER_ASSERT(sizeof(FSArchive) == 92);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FS_FindArchive

  Description:  �A�[�J�C�u������������B
                ��v���閼�O��������� NULL ��Ԃ��B

  Arguments:    name             ��������A�[�J�C�u���̕�����
                name_len         name �̕�����

  Returns:      �������Č��������A�[�J�C�u�̃|�C���^�� NULL
 *---------------------------------------------------------------------------*/
FSArchive *FS_FindArchive(const char *name, int name_len);

/*---------------------------------------------------------------------------*
  Name:         FS_NormalizePath

  Description:  �w��̃p�X���ɃJ�����g�f�B���N�g�����l������
                (�A�[�J�C�u�|�C���^) + (��f�B���N�g��ID) + (�p�X��)
                �̌`���֐��K������B

  Arguments:    path        ���K������Ă��Ȃ��p�X������
                baseid      ��f�B���N�g��ID�̊i�[��܂���NULL
                relpath     �ϊ���̃p�X���i�[��܂���NULL

  Returns:      �A�[�J�C�u�|�C���^�܂���NULL
 *---------------------------------------------------------------------------*/
FSArchive* FS_NormalizePath(const char *path, u32 *baseid, char *relpath);

/*---------------------------------------------------------------------------*
  Name:         FS_GetCurrentDirectory

  Description:  ���݂̃f�B���N�g�����p�X���Ŏ擾�B

  Arguments:    None.

  Returns:      ���݂̃f�B���N�g����\��������
 *---------------------------------------------------------------------------*/
const char *FS_GetCurrentDirectory(void);

/*---------------------------------------------------------------------------*
  Name:         FS_GetLastArchiveCommand

  Description:  �A�[�J�C�u�����������ŐV�̃R�}���h���擾�B

  Arguments:    arc              FSArchive�\����

  Returns:      �A�[�J�C�u�����������ŐV�̃R�}���h�̌��ʒl
 *---------------------------------------------------------------------------*/
SDK_INLINE FSCommandType FS_GetLastArchiveCommand(const FSArchive *arc)
{
    return arc->command;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveResultCode

  Description:  �w�肵���A�[�J�C�u�̍ŐV�̃G���[�R�[�h���擾�B

  Arguments:    path_or_archive  �Ώۂ̃A�[�J�C�u���w��
                                 FSArchive�\���̂܂��̓p�X������

  Returns:      �w�肳�ꂽ�A�[�J�C�u�̍ŐV�̃G���[�R�[�h�B
                �Y������A�[�J�C�u�����݂��Ȃ��ꍇ��FS_RESULT_ERROR
 *---------------------------------------------------------------------------*/
FSResult FS_GetArchiveResultCode(const void *path_or_archive);

/*---------------------------------------------------------------------------*
  Name:         FSi_EndArchive

  Description:  �S�ẴA�[�J�C�u���I�������ĉ��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_EndArchive(void);

/*---------------------------------------------------------------------------*
  Name:         FS_InitArchive

  Description:  �A�[�J�C�u�\���̂�������

  Arguments:    arc              ����������A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_InitArchive(FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveName

  Description:  �A�[�J�C�u�����擾

  Arguments:    arc              ���O���擾����A�[�J�C�u

  Returns:      �t�@�C���V�X�e���ɓo�^���ꂽ�A�[�J�C�u��
 *---------------------------------------------------------------------------*/
const char *FS_GetArchiveName(const FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveLoaded

  Description:  �A�[�J�C�u�����݃t�@�C���V�X�e���Ƀ��[�h�ς݂�����

  Arguments:    arc              ���肷��A�[�J�C�u

  Returns:      �t�@�C���V�X�e���Ƀ��[�h�ς݂Ȃ� TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsArchiveLoaded(volatile const FSArchive *arc)
{
    return ((arc->flag & FS_ARCHIVE_FLAG_LOADED) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveSuspended

  Description:  �A�[�J�C�u�����݃T�X�y���h��������

  Arguments:    arc              ���肷��A�[�J�C�u

  Returns:      �T�X�y���h���Ȃ� TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL FS_IsArchiveSuspended(volatile const FSArchive *arc)
{
    return ((arc->flag & FS_ARCHIVE_FLAG_SUSPEND) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveUserData

  Description:  �A�[�J�C�u�Ɋ֘A�t����ꂽ���[�U��`�|�C���^���擾

  Arguments:    arc              FSArchive�\����

  Returns:      ���[�U��`�|�C���^
 *---------------------------------------------------------------------------*/
SDK_INLINE void* FS_GetArchiveUserData(const FSArchive *arc)
{
    return arc->userdata;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RegisterArchiveName

  Description:  �A�[�J�C�u�����t�@�C���V�X�e���֓o�^���֘A�t����B
                �A�[�J�C�u���̂͂܂��t�@�C���V�X�e���Ƀ��[�h����Ȃ��B
                �A�[�J�C�u�� "rom" �̓t�@�C���V�X�e���ɗ\��ς݁B

  Arguments:    arc              ���O���֘A�t����A�[�J�C�u
                name             �o�^���閼�O�̕�����
                name_len         name �̕�����

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL    FS_RegisterArchiveName(FSArchive *arc, const char *name, u32 name_len);

/*---------------------------------------------------------------------------*
  Name:         FS_ReleaseArchiveName

  Description:  �o�^�ς݂̃A�[�J�C�u�����������

  Arguments:    arc              ���O���������A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_ReleaseArchiveName(FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FS_MountArchive

  Description:  �A�[�J�C�u���}�E���g����

  Arguments:    arc              �}�E���g����A�[�J�C�u
                userdata         �A�[�J�C�u�Ɋ֘A�t���郆�[�U��`�ϐ�
                vtbl             �R�}���h�C���^�t�F�[�X
                reserved         �����̂��߂̗\�� (���0���w��)


  Returns:      �}�E���g�����ɐ��������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_MountArchive(FSArchive *arc, void *userdata,
                        const FSArchiveInterface *vtbl, u32 reserved);

/*---------------------------------------------------------------------------*
  Name:         FS_UnmountArchive

  Description:  �A�[�J�C�u���A���}�E���g����

  Arguments:    arc              �A���}�E���g����A�[�J�C�u

  Returns:      �A���}�E���g�����ɐ��������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_UnmountArchive(FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FS_SuspendArchive

  Description:  �A�[�J�C�u�̏����@�\���̂��~����B
                ���ݎ��s���̏����������, ���̊�����ҋ@�B

  Arguments:    arc              ��~����A�[�J�C�u

  Returns:      �Ăяo���ȑO�ɃT�X�y���h��ԂłȂ���� TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_SuspendArchive(FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FS_ResumeArchive

  Description:  ��~���Ă����A�[�J�C�u�̏������ĊJ

  Arguments:    arc              �ĊJ����A�[�J�C�u

  Returns:      �Ăяo���ȑO�ɃT�X�y���h��ԂłȂ���� TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_ResumeArchive(FSArchive *arc);

/*---------------------------------------------------------------------------*
  Name:         FS_NotifyArchiveAsyncEnd

  Description:  �񓯊��Ŏ��s���Ă����A�[�J�C�u�����̊�����ʒm���邽�߂�
                �A�[�J�C�u����������Ăяo���B

  Arguments:    arc              ������ʒm����A�[�J�C�u
                ret              ��������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FS_NotifyArchiveAsyncEnd(FSArchive *arc, FSResult ret);


/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_ARCHIVE_H_ */
