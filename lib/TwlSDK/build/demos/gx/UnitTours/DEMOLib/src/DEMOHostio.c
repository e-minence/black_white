/*---------------------------------------------------------------------------*
  Project:  TwlSDK - GX - demos - UnitTours/DEMOLib
  File:     DEMOHostio.c

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
#include "DEMOHostio.h"

// �R���p�C�����ƃ����N���̗�����IS-TWL-DEBUGGER�̃��C�u���������݂�
// �����s����IS-TWL-DEBUGGER�ł������ꍇ�̂�HostI/O�����p�\�ł���_�ɒ��ӁB
// ���Ƃ��΁ADEMOLib���C�u�����Ƃ�����g���A�v���P�[�V������ʁX�Ƀr���h��
// �ǂ��炩�̊���IS-TWL-DEBUGGER�\�t�g�E�F�A���C���X�g�[������Ă��Ȃ��Ȃ�
// ���̃��W���[���͉��������x�����b�Z�[�W���o�͂���R�[�h�����𐶐�����B
#ifdef SDK_LINK_ISTD
#include <istdhio.h>
#include <istdfio.h>
#include <istdbglibpriv.h>

/*---------------------------------------------------------------------------*/
/* variables */

#define DEMO_HIO_HANDLE_MAX   8

// IS-TWL-DEBUGGER HostI/O ���b�p�[�A�[�J�C�u
typedef struct DEMOHIOContext
{
    FSArchive       arc[1];
    char            basepath[FS_ENTRY_LONGNAME_MAX];
    char            fullpath[2][FS_ENTRY_LONGNAME_MAX];
    ISTDFIOFile     file[DEMO_HIO_HANDLE_MAX];
    ISTDFIOFindData ffdata[DEMO_HIO_HANDLE_MAX];
    int             busyfiles;
    int             validfiles;
}
DEMOHIOContext;

static DEMOHIOContext DEMOiHIODrive[1] ATTRIBUTE_ALIGN(32);


/*---------------------------------------------------------------------------*/
/* function */

// �����N����IS-TWL-DEBUGGER�\�t�g�E�F�A���C���X�g�[������Ă��Ȃ����
// ���Ƃ����s����IS-TWL-DEBUGGER�ł�HostI/O�𗘗p���邱�Ƃ͂ł��Ȃ��B
// DEMOLib���g�p���邪HostI/O�͎g�p���Ȃ��v���O������
// �����N�G���[���N�����Ȃ��悤�̃_�~�[�֐���p�ӂ��Ă����B
SDK_WEAK_SYMBOL void ISTDHIOInit(void) __attribute__((never_inline))
{
}
SDK_WEAK_SYMBOL BOOL ISTDHIOOpen(u32) __attribute__((never_inline))
{
    OS_TWarning("cannot use HostI/O on this link-time environment.\n");
    return FALSE;
}
SDK_WEAK_SYMBOL u32 ISTDFIOOpen(ISTDFIOFile*, const char*, u32) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOClose(ISTDFIOFile*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIORead(ISTDFIOFile*, void*, u32, u32*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
#if !defined(ISTDDBGLIB_VERSION) || (ISTDDBGLIB_VERSION < 8)
SDK_WEAK_SYMBOL u32 ISTDFIOWrite(ISTDFIOFile*, void*, u32) __attribute__((never_inline))
#else
SDK_WEAK_SYMBOL u32 ISTDFIOWrite(ISTDFIOFile*, const void*, u32) __attribute__((never_inline))
#endif
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOSeek(ISTDFIOFile*, s32, u32, u32*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOLength(ISTDFIOFile*, u32*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOFindFirst(ISTDFIOFile*, ISTDFIOFindData*, const char*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOFindNext(ISTDFIOFile*, ISTDFIOFindData*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOFindClose(ISTDFIOFile*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIORemove(const char*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIORename(const char*, const char*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOMkDir(const char*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIOGetAttribute(const char*, CFIOAtrb*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}
SDK_WEAK_SYMBOL u32 ISTDFIORmDir(const char*) __attribute__((never_inline))
{
    return ISTDFIO_FILEIO_ERROR_NOTCONNECT;
}


/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_AllocFile

  Description:  HIO�̃t�@�C���\���̂��m��

  Arguments:    context             DEMOHIOContext�R���e�L�X�g

  Returns:      �t�@�C���\���̂܂���NULL
 *---------------------------------------------------------------------------*/
static ISTDFIOFile* DEMOi_HIO_AllocFile(DEMOHIOContext *context)
{
    ISTDFIOFile    *file = NULL;
    u32     index = MATH_CTZ((u32)~context->busyfiles);
    if (index < DEMO_HIO_HANDLE_MAX)
    {
        context->busyfiles |= (1 << index);
        file = &context->file[index];
    }
    return file;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_FreeFile

  Description:  HIO�̃t�@�C���\���̂����

  Arguments:    context             DEMOHIOContext�R���e�L�X�g
                file                �t�@�C���\����

  Returns:      None
 *---------------------------------------------------------------------------*/
static void DEMOi_HIO_FreeFile(DEMOHIOContext *context, ISTDFIOFile *file)
{
    int     index = (context->file - file);
    context->busyfiles &= ~(1 << index);
    context->validfiles &= ~(1 << index);
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_CreateFullPath

  Description:  FS�̃p�X������HIO�̃t���p�X���֕ϊ�

  Arguments:    context             DEMOHIOContext�R���e�L�X�g
                dst                 �t���p�X�i�[��
                relpath             ���[�g����̑��΃p�X

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static void DEMOi_HIO_CreateFullPath(DEMOHIOContext *context, char *dst, const char *relpath)
{
    // �p�X����":"���܂܂�Ă���΂��̂܂܎g�p
    if (STD_SearchString(relpath, ":") != NULL)
    {
        (void)STD_TSPrintf(dst, "%s", relpath);
    }
    // �����łȂ���΃��[�g�f�B���N�g���Ƒ��΃p�X��A��
    else
    {
        (void)STD_TSPrintf(dst, "%s/%s", context->basepath, relpath);
    }
    // �I�[�ɕt�^���ꂽ'/'������Ώ���
    {
        int    length = STD_GetStringLength(dst);
        if ((--length >= 0) && ((dst[length] == '\\') || (dst[length] == '/')))
        {
            dst[length] = '\0';
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_ConvertError

  Description:  HIO�̃G���[�R�[�h��FSREsult�֕ϊ�

  Arguments:    error               �G���[�R�[�h

  Returns:      FSREsult�ɕϊ����ꂽ�G���[�R�[�h
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_ConvertError(u32 error)
{
    if (error == ISTDFIO_FILEIO_ERROR_SUCCESS)
    {
        return FS_RESULT_SUCCESS;
    }
    else if (error == ISTDFIO_FILEIO_ERROR_COMERROR)
    {
        return FS_RESULT_ERROR;
    }
    else if (error == ISTDFIO_FILEIO_ERROR_NOTCONNECT)
    {
        return FS_RESULT_ERROR;
    }
    else if (error == ISTDFIO_FILEIO_ERROR_SERVERERROR)
    {
        return FS_RESULT_ERROR;
    }
    else if (error == ISTDFIO_FILEIO_ERROR_NOMOREFILES)
    {
        return FS_RESULT_FAILURE;
    }
    else if (error == ISTDFIO_FILEIO_ERROR_FILENOTFOUND)
    {
        return FS_RESULT_ERROR;
    }
    else if (error == ISTDFIO_FILEIO_ERROR_PATHTOOLONG)
    {
        return FS_RESULT_INVALID_PARAMETER;
    }
    else
    {
        return FS_RESULT_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_ConvertTime

  Description:  HIO�̎����\���̂�FSDateTime�֕ϊ�

  Arguments:    dst                 �ϊ����FSDateTime
                src                 �ϊ�����CFIODateTime

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DEMOi_HIO_ConvertTime(FSDateTime *dst, const CFIODateTime *src)
{
    dst->year = src->m_nYear;
    dst->month = src->m_nMonth;
    dst->day = src->m_nDay;
    dst->hour = src->m_nHour;
    dst->minute = src->m_nMinute;
    dst->second = src->m_nSecond;
}

/*---------------------------------------------------------------------------*
 * �ȉ��AFS�R�}���h�C���^�t�F�[�X
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_GetArchiveCaps

  Description:  FS_COMMAND_GETARCHIVECAPS�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                caps                �f�o�C�X�\�̓t���O�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_GetArchiveCaps(FSArchive *arc, u32 *caps)
{
    (void)arc;
    *caps = 0;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_GetPathInfo

  Description:  FS_COMMAND_GETPATHINFO�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                relpath             �p�X
                info                �t�@�C�����̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_GetPathInfo(FSArchive *arc, u32 baseid, const char *relpath, FSPathInfo *info)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    CFIOAtrb        stat[1];
    (void)baseid;
    DEMOi_HIO_CreateFullPath(context, tmppath, relpath);
    result = DEMOi_HIO_ConvertError(ISTDFIOGetAttribute(tmppath, stat));
    if (result == FS_RESULT_ERROR)
    {
        info->attributes = (stat->m_nAtrb & FS_ATTRIBUTE_DOS_MASK);
        if ((stat->m_nAtrb & ISTDFIO_FILEIO_ATTRIBUTE_DIRECTORY) != 0)
        {
            info->attributes |= FS_ATTRIBUTE_IS_DIRECTORY;
        }
//        info->filesize = stat->m_nSize;
        DEMOi_HIO_ConvertTime(&info->atime, &stat->m_atime);
        DEMOi_HIO_ConvertTime(&info->mtime, &stat->m_mtime);
        DEMOi_HIO_ConvertTime(&info->ctime, &stat->m_ctime);
        info->id = FS_INVALID_FILE_ID;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_CreateFile

  Description:  FS_COMMAND_CREATE_FILE�R�}���h

  Arguments:

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_CreateFile(FSArchive *arc, u32 baseid, const char *relpath, u32 permit)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    // �t�@�C��������Open�ő�p�B
    ISTDFIOFile    *file = DEMOi_HIO_AllocFile(context);
    (void)baseid;
    if (!file)
    {
        result = FS_RESULT_NO_MORE_RESOURCE;
    }
    else
    {
        u32     flags = 0;
        flags |= ((permit & FS_PERMIT_R) != 0) ? ISTDFIO_FILEIO_FLAG_READ : 0;
        flags |= ((permit & FS_PERMIT_W) != 0) ? ISTDFIO_FILEIO_FLAG_WRITE : 0;
        flags |= ISTDFIO_FILEIO_FLAG_FORCE;
        DEMOi_HIO_CreateFullPath(context, tmppath, relpath);
        result = DEMOi_HIO_ConvertError(ISTDFIOOpen(file, tmppath, flags));
        if (result == FS_RESULT_SUCCESS)
        {
            (void)ISTDFIOClose(file);
        }
        DEMOi_HIO_FreeFile(context, file);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_DeleteFile

  Description:  FS_COMMAND_DELETE_FILE�R�}���h

  Arguments:

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_DeleteFile(FSArchive *arc, u32 baseid, const char *relpath)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    (void)baseid;
    DEMOi_HIO_CreateFullPath(context, tmppath, relpath);
    result = DEMOi_HIO_ConvertError(ISTDFIORemove(tmppath));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_RenameFileOrDirectory

  Description:  FS_COMMAND_RENAME_FILE�R�}���h�܂���FS_COMMAND_RENAME_DIRECTORY�R�}���h

  Arguments:

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_RenameFileOrDirectory(FSArchive *arc,
                                              u32 baseid_src, const char *relpath_src,
                                              u32 baseid_dst, const char *relpath_dst)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath1 = context->fullpath[0];
    char           *tmppath2 = context->fullpath[1];
    (void)baseid_src;
    (void)baseid_dst;
    DEMOi_HIO_CreateFullPath(context, tmppath1, relpath_src);
    DEMOi_HIO_CreateFullPath(context, tmppath2, relpath_dst);
    result = DEMOi_HIO_ConvertError(ISTDFIORename(tmppath1, tmppath2));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_CreateDirectory

  Description:  FS_COMMAND_CREATE_DIRECTORY�R�}���h

  Arguments:

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_CreateDirectory(FSArchive *arc, u32 baseid, const char *relpath, u32 permit)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    (void)baseid;
    (void)permit;
    DEMOi_HIO_CreateFullPath(context, tmppath, relpath);
    result = DEMOi_HIO_ConvertError(ISTDFIOMkDir(tmppath));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_DeleteDirectory

  Description:  FS_COMMAND_DELETE_DIRECTORY�R�}���h

  Arguments:

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_DeleteDirectory(FSArchive *arc, u32 baseid, const char *relpath)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    (void)baseid;
    DEMOi_HIO_CreateFullPath(context, tmppath, relpath);
    result = DEMOi_HIO_ConvertError(ISTDFIORmDir(tmppath));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_OpenFile

  Description:  FS_COMMAND_OPENFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                baseid              ��_�f�B���N�g�� (���[�g�Ȃ�0)
                path                �t�@�C���p�X
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_OpenFile(FSArchive *arc, FSFile *file, u32 baseid, const char *path, u32 mode)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    ISTDFIOFile    *handle = DEMOi_HIO_AllocFile(context);
    (void)baseid;
    if (!handle)
    {
        result = FS_RESULT_NO_MORE_RESOURCE;
    }
    else
    {
        u32     flags = 0;
        flags |= ((mode & FS_FILEMODE_R) != 0) ? ISTDFIO_FILEIO_FLAG_READ : 0;
        flags |= ((mode & FS_FILEMODE_W) != 0) ? ISTDFIO_FILEIO_FLAG_WRITE : 0;
        DEMOi_HIO_CreateFullPath(context, tmppath, path);
        result = DEMOi_HIO_ConvertError(ISTDFIOOpen(handle, tmppath, flags));
        if (result == FS_RESULT_SUCCESS)
        {
            FS_SetFileHandle(file, arc, (void *)handle);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_CloseFile

  Description:  FS_COMMAND_CLOSEFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_CloseFile(FSArchive *arc, FSFile *file)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    result = DEMOi_HIO_ConvertError(ISTDFIOClose(handle));
    DEMOi_HIO_FreeFile(context, handle);
    FS_DetachHandle(file);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_ReadFile

  Description:  FS_COMMAND_READFILE�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                buffer           �]���惁����
                length           �]���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_ReadFile(FSArchive *arc, FSFile *file, void *buffer, u32 *length)
{
    FSResult        result = FS_RESULT_ERROR;
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    (void)arc;
    result = DEMOi_HIO_ConvertError(ISTDFIORead(handle, buffer, *length, length));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_WriteFile

  Description:  FS_COMMAND_WRITEFILE�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                buffer           �]����������
                length           �]���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_WriteFile(FSArchive *arc, FSFile *file, const void *buffer, u32 *length)
{
    FSResult        result = FS_RESULT_ERROR;
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    (void)arc;
    result = DEMOi_HIO_ConvertError(ISTDFIOWrite(handle, (void *)buffer, *length));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_SeekFile

  Description:  FS_COMMAND_SEEKFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                offset              �ړ��ʂ���шړ���̈ʒu
                from                �V�[�N�N�_

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_SeekFile(FSArchive *arc, FSFile *file, int *offset, FSSeekFileMode from)
{
    FSResult        result = FS_RESULT_ERROR;
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    u32             mode = ISTDFIO_FILEIO_SEEK_CURRENT;
    (void)arc;
    if (from == FS_SEEK_SET)
    {
        mode = ISTDFIO_FILEIO_SEEK_BEGIN;
    }
    else if (from == FS_SEEK_CUR)
    {
        mode = ISTDFIO_FILEIO_SEEK_CURRENT;
    }
    else if (from == FS_SEEK_END)
    {
        mode = ISTDFIO_FILEIO_SEEK_END;
    }
    result = DEMOi_HIO_ConvertError(ISTDFIOSeek(handle, (s32)*offset, mode, (u32*)offset));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_GetFileLength

  Description:  FS_COMMAND_GETFILELENGTH�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                length              �擾�����T�C�Y�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_GetFileLength(FSArchive *arc, FSFile *file, u32 *length)
{
    FSResult        result = FS_RESULT_ERROR;
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    (void)arc;
    result = DEMOi_HIO_ConvertError(ISTDFIOLength(handle, length));
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_GetFilePosition

  Description:  FS_COMMAND_GETFILEPOSITION�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                position            �擾�����ʒu�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_GetFilePosition(FSArchive *arc, FSFile *file, u32 *position)
{
    *position = 0;
    return DEMOi_HIO_SeekFile(arc, file, (int *)position, FS_SEEK_CUR);
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_OpenDirectory

  Description:  FS_COMMAND_OPENDIRECTORY�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                path                �p�X
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_OpenDirectory(FSArchive *arc, FSFile *file, u32 baseid,
                                      const char *path, u32 mode)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    ISTDFIOFile    *handle = DEMOi_HIO_AllocFile(context);
    (void)baseid;
    if (!handle)
    {
        result = FS_RESULT_NO_MORE_RESOURCE;
    }
    else
    {
        int                 index = handle - context->file;
        ISTDFIOFindData    *data = &context->ffdata[index];
        u32     flags = 0;
        flags |= ((mode & FS_FILEMODE_R) != 0) ? ISTDFIO_FILEIO_FLAG_READ : 0;
        flags |= ((mode & FS_FILEMODE_W) != 0) ? ISTDFIO_FILEIO_FLAG_WRITE : 0;
        DEMOi_HIO_CreateFullPath(context, tmppath, path);
        // ����ȃ��C���h�J�[�h�w��𖳌���
        if (*tmppath)
        {
            char  *s = tmppath + STD_GetStringLength(tmppath);
            while (*--s == '*')
            {
            }
            if (*s != '/')
            {
                *++s = '/';
            }
            *++s = '*';
            *++s = '\0';
        }
        result = DEMOi_HIO_ConvertError(ISTDFIOFindFirst(handle, data, tmppath));
        if (result == FS_RESULT_SUCCESS)
        {
            context->validfiles |= (1 << index);
            FS_SetDirectoryHandle(file, arc, (void *)handle);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_CloseDirectory

  Description:  FS_COMMAND_CLOSEDIRECTORY�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_CloseDirectory(FSArchive *arc, FSFile *file)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    result = DEMOi_HIO_ConvertError(ISTDFIOFindClose(handle));
    DEMOi_HIO_FreeFile(context, handle);
    FS_DetachHandle(file);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_ReadDirectory

  Description:  FS_COMMAND_READDIR�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                info             ���̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_ReadDirectory(FSArchive * arc, FSFile * file,
                                        FSDirectoryEntryInfo * info)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    ISTDFIOFile    *handle = (ISTDFIOFile *)FS_GetFileUserData(file);
    int             index = handle - context->file;
    if ((context->validfiles & (1 << index)) == 0)
    {
        result = FS_RESULT_FAILURE;
    }
    else
    {
        ISTDFIOFindData    *data = &context->ffdata[index];
        info->shortname_length = 0;
        info->shortname[0] = 0;
        info->longname_length = (u32)STD_GetStringLength(data->m_bufName);
        (void)STD_CopyLString(info->longname, data->m_bufName, sizeof(info->longname));
        info->attributes = (data->m_nAttrib & FS_ATTRIBUTE_DOS_MASK);
        if ((data->m_nAttrib & ISTDFIO_FILEIO_ATTRIBUTE_DIRECTORY) != 0)
        {
            info->attributes |= FS_ATTRIBUTE_IS_DIRECTORY;
        }
        info->filesize = data->m_nSize;
//        DEMOi_HIO_ConvertTime(&info->atime, data->m_atime);
//        DEMOi_HIO_ConvertTime(&info->mtime, data->m_mtime);
//        DEMOi_HIO_ConvertTime(&info->ctime, data->m_ctime);
        result = FS_RESULT_SUCCESS;
        if (ISTDFIOFindNext(handle, data) != ISTDFIO_FILEIO_ERROR_SUCCESS)
        {
            context->validfiles &= ~(1 << index);
        }
    }
    return result;
}

#endif // SDK_LINK_ISTD



/*---------------------------------------------------------------------------*
  Name:         DEMOMountHostIO

  Description:  IS-TWL-DEBUGGER��HostI/O�t�@�C���V�X�e����
                "hostio:" �Ƃ������O��FS�Ƀ}�E���g����B

  Arguments:    basepath    ��ƂȂ�f�o�b�O�z�X�g���̃��[�g�f�B���N�g���B
                            FS�̃p�X���� "hostio:/relpath" �Ǝw�肵���ꍇ
                            �z�X�g���� "basepath/relpath" ���Ӗ�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOMountHostIO(const char *basepath)
{

#ifndef SDK_LINK_ISTD

    // �R���p�C������IS-TWL-DEBUGGER�\�t�g�E�F�A���C���X�g�[������Ă��Ȃ����
    // HostI/O�A�N�Z�XAPI�̒�`���Q�Ƃ��邱�Ƃ͂ł��Ȃ��B
    (void)basepath;
    OS_TWarning("cannot use HostI/O on this compile-time environment.\n");

#else // SDK_LINK_ISTD

    // ���s����IS-TWL-DEBUGGER�\�t�g�E�F�A�łȂ����
    // HostI/O�@�\�����ۂɗ��p���邱�Ƃ͂ł��Ȃ��B
    if ((OS_GetConsoleType() & OS_CONSOLE_TWLDEBUGGER) == 0)
    {
        OS_TWarning("cannot use HostI/O on this run-time environment.\n");
    }

    // �����N����IS-TWL-DEBUGGER�\�t�g�E�F�A���C���X�g�[������Ă��Ȃ����
    // ���Ƃ����s����IS-TWL-DEBUGGER�ł�HostI/O�𗘗p���邱�Ƃ͂ł��Ȃ��B
    else if (ISTDHIOInit(), !ISTDHIOOpen(ISTDHIODEVMASK_AUTOSELECT))
    {
        OS_TWarning("failed to open HostI/O device.\n");
    }

    // �R���p�C�����ƃ����N���̗�����IS-TWL-DEBUGGER�̃��C�u���������݂�
    // �����s����IS-TWL-DEBUGGER�ł������ꍇ�̂�HostI/O�����p�\�B
    else
    {
        static const FSArchiveInterface FSiArchiveHIOInterface =
        {
            // ���d�l�ƌ݊����̂���R�}���h
            DEMOi_HIO_ReadFile,
            DEMOi_HIO_WriteFile,
            NULL,               // SeekDirectory
            DEMOi_HIO_ReadDirectory,
            NULL,               // FindPath
            NULL,               // GetPath
            NULL,               // OpenFileFast
            NULL,               // OpenFileDirect
            DEMOi_HIO_CloseFile,
            NULL,               // Activate
            NULL,               // Idle
            NULL,               // Suspend
            NULL,               // Resume
            // ���d�l�ƌ݊��������邪�R�}���h�ł͂Ȃ���������
            DEMOi_HIO_OpenFile,
            DEMOi_HIO_SeekFile,
            DEMOi_HIO_GetFileLength,
            DEMOi_HIO_GetFilePosition,
            // �V�d�l�Ŋg�����ꂽ�R�}���h
            NULL,               // Mount
            NULL,               // Unmount
            DEMOi_HIO_GetArchiveCaps,
            DEMOi_HIO_CreateFile,
            DEMOi_HIO_DeleteFile,
            DEMOi_HIO_RenameFileOrDirectory,
            DEMOi_HIO_GetPathInfo,
            NULL,               // SetFileInfo
            DEMOi_HIO_CreateDirectory,
            DEMOi_HIO_DeleteDirectory,
            DEMOi_HIO_RenameFileOrDirectory,
            NULL,               // GetArchiveResource
            NULL,               // 29UL
            NULL,               // FlushFile
            NULL,               // SetFileLength
            DEMOi_HIO_OpenDirectory,
            DEMOi_HIO_CloseDirectory,
        };
        DEMOiHIODrive->busyfiles = 0;
        DEMOiHIODrive->validfiles = 0;
        (void)STD_CopyString(DEMOiHIODrive->basepath, basepath);
        if (FS_RegisterArchiveName(DEMOiHIODrive->arc, "hostio", 6))
        {
            (void)FS_MountArchive(DEMOiHIODrive->arc, DEMOiHIODrive, &FSiArchiveHIOInterface, 0);
        }
    }

#endif // SDK_LINK_ISTD
}
