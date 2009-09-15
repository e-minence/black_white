/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     command.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-08#$
  $Rev: 9544 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_COMMAND_H_)
#define NITRO_FS_COMMAND_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/archive.h>
#include <nitro/fs/file.h>
#include <nitro/fs/romfat.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

// �������ꎞ�I�Ƀ^�X�N���X�g�֕ۑ����邽�߂̃p�b�N�\����

typedef struct FSArgumentForReadFile
{
    void       *buffer;
    u32         length;
}
FSArgumentForReadFile;

typedef struct FSArgumentForWriteFile
{
    const void *buffer;
    u32         length;
}
FSArgumentForWriteFile;

typedef struct FSArgumentForSeekDirectory
{
    u32         id;
    u32         position;
}
FSArgumentForSeekDirectory;

typedef struct FSArgumentForReadDirectory
{
    FSDirectoryEntryInfo   *info;
}
FSArgumentForReadDirectory;

typedef struct FSArgumentForFindPath
{
    u32                 baseid;
    const char         *relpath;
    u32                 target_id;
    BOOL                target_is_directory;
}
FSArgumentForFindPath;

typedef struct FSArgumentForGetPath
{
    BOOL                is_directory;
    char               *buffer;
    u32                 length;
}
FSArgumentForGetPath;

typedef struct FSArgumentForOpenFileFast
{
    u32                 id;
    u32                 mode;
}
FSArgumentForOpenFileFast;

typedef struct FSArgumentForOpenFileDirect
{
    u32                 id; // in : requested-id
    u32                 top;
    u32                 bottom;
    u32                 mode;
}
FSArgumentForOpenFileDirect;

typedef void FSArgumentForCloseFile;
typedef void FSArgumentForActivate;
typedef void FSArgumentForIdle;
typedef void FSArgumentForSuspend;
typedef void FSArgumentForResume;

typedef struct FSArgumentForOpenFile
{
    u32             baseid;
    const char     *relpath;
    u32             mode;
}
FSArgumentForOpenFile;

typedef struct FSArgumentForSetSeekCache
{
    void           *buf;
    u32             buf_size;
}
FSArgumentForSetSeekCache;

typedef struct FSArgumentForSeekFile
{
    int             offset;
    FSSeekFileMode  from;
}
FSArgumentForSeekFile;

typedef struct FSArgumentForGetFileLength
{
    u32             length;
}
FSArgumentForGetFileLength;

typedef struct FSArgumentForGetFilePosition
{
    u32             position;
}
FSArgumentForGetFilePosition;

typedef struct FSArgumentForGetArchiveCaps
{
    u32             caps;
}
FSArgumentForGetArchiveCaps;

typedef void FSArgumentForMount;
typedef void FSArgumentForUnmount;

typedef struct FSArgumentForCreateFile
{
    u32         baseid;
    const char *relpath;
    u32         permit;
}
FSArgumentForCreateFile;

typedef struct FSArgumentForDeleteFile
{
    u32         baseid;
    const char *relpath;
}
FSArgumentForDeleteFile;

typedef struct FSArgumentForRenameFile
{
    u32         baseid_src;
    const char *relpath_src;
    u32         baseid_dst;
    const char *relpath_dst;
}
FSArgumentForRenameFile;

typedef struct FSArgumentForGetPathInfo
{
    u32         baseid;
    const char *relpath;
    FSPathInfo *info;
}
FSArgumentForGetPathInfo;

typedef struct FSArgumentForSetPathInfo
{
    u32         baseid;
    const char *relpath;
    FSPathInfo *info;
}
FSArgumentForSetPathInfo;

typedef FSArgumentForCreateFile     FSArgumentForCreateDirectory;
typedef FSArgumentForDeleteFile     FSArgumentForDeleteDirectory;
typedef FSArgumentForRenameFile     FSArgumentForRenameDirectory;

typedef struct FSArgumentForGetArchiveResource
{
    FSArchiveResource  *resource;
}
FSArgumentForGetArchiveResource;

typedef void FSArgumentForFlushFile;

typedef struct FSArgumentForSetFileLength
{
    u32             length;
}
FSArgumentForSetFileLength;

typedef FSArgumentForOpenFile   FSArgumentForOpenDirectory;
typedef FSArgumentForCloseFile  FSArgumentForCloseDirectory;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FSi_SendCommand

  Description:  �A�[�J�C�u�փR�}���h�𔭍s����B
                �N���^�C�~���O�𒲐����A�����n�Ȃ炱���Ńu���b�L���O�B

  Arguments:    p_file           �R�}���h�����̊i�[���ꂽFSFile�\����
                command          �R�}���hID
                blocking         �u���b�L���O����Ȃ�TRUE

  Returns:      �R�}���h����������� TRUE
 *---------------------------------------------------------------------------*/
BOOL    FSi_SendCommand(FSFile *p_file, FSCommandType command, BOOL blocking);

/*---------------------------------------------------------------------------*
  Name:         FSi_GetCurrentCommand

  Description:  ���ݏ������Ă���R�}���h���擾�B

  Arguments:    file             �R�}���h�����̊i�[���ꂽFSFile�\����

  Returns:      ���ݏ������Ă���R�}���h
 *---------------------------------------------------------------------------*/
SDK_INLINE FSCommandType FSi_GetCurrentCommand(const FSFile *file)
{
    return (FSCommandType)((file->stat >> FS_FILE_STATUS_CMD_SHIFT) & FS_FILE_STATUS_CMD_MASK);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WaitForArchiveCompletion

  Description:  �A�[�J�C�u���񓯊����������s���Ȃ炻�̊�����҂B
                ����R�}���h���ʂ̒�ʃR�}���h�Ɉˑ����Ă���ꍇ��
                �v���V�[�W���̓����ŌĂяo�����B

  Arguments:    file             �R�}���h�����̊i�[���ꂽFSFile�\����
                result           �A�[�J�C�u����̕Ԃ�l
                                 ���ꂪFS_RESULT_PROC_ASYNC�̏ꍇ�̂�
                                 ���ۂɃu���b�L���O����B

  Returns:      ���ۂ̏�������
 *---------------------------------------------------------------------------*/
FSResult FSi_WaitForArchiveCompletion(FSFile *file, FSResult result);

/*---------------------------------------------------------------------------*
  Name:         FSi_GetArchiveChain

  Description:  �o�^����Ă���A�[�J�C�u�̐擪���擾�B

  Arguments:    None.

  Returns:      �o�^����Ă���A�[�J�C�u�̐擪
 *---------------------------------------------------------------------------*/
FSArchive* FSi_GetArchiveChain(void);

/*---------------------------------------------------------------------------*
  Name:         FSi_IsUnreadableRomOffset

  Description:  �w���ROM�I�t�Z�b�g�����݂̊��œǂ߂Ȃ��͈͂ɂ��邩����B

  Arguments:    arc                 �Ăяo�����A�[�J�C�u�B
                offset              ���肷��ROM�I�t�Z�b�g�B

  Returns:      �w���ROM�I�t�Z�b�g�����݂̊��œǂ߂Ȃ����TRUE�B
 *---------------------------------------------------------------------------*/
BOOL FSi_IsUnreadableRomOffset(FSArchive *arc, u32 offset);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* NITRO_FS_COMMAND_H_ */
