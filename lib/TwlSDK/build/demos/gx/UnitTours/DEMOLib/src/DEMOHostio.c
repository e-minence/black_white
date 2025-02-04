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

// コンパイル環境とリンク環境の両方にIS-TWL-DEBUGGERのライブラリが存在し
// かつ実行環境がIS-TWL-DEBUGGERであった場合のみHostI/Oが利用可能である点に注意。
// たとえば、DEMOLibライブラリとそれを使うアプリケーションを別々にビルドし
// どちらかの環境にIS-TWL-DEBUGGERソフトウェアがインストールされていないなら
// このモジュールは何もせず警告メッセージを出力するコードだけを生成する。
#ifdef SDK_LINK_ISTD
#include <istdhio.h>
#include <istdfio.h>
#include <istdbglibpriv.h>

/*---------------------------------------------------------------------------*/
/* variables */

#define DEMO_HIO_HANDLE_MAX   8

// IS-TWL-DEBUGGER HostI/O ラッパーアーカイブ
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

// リンク環境にIS-TWL-DEBUGGERソフトウェアがインストールされていなければ
// たとえ実行環境がIS-TWL-DEBUGGERでもHostI/Oを利用することはできない。
// DEMOLibを使用するがHostI/Oは使用しないプログラムが
// リンクエラーを起こさないようのダミー関数を用意しておく。
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

  Description:  HIOのファイル構造体を確保

  Arguments:    context             DEMOHIOContextコンテキスト

  Returns:      ファイル構造体またはNULL
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

  Description:  HIOのファイル構造体を解放

  Arguments:    context             DEMOHIOContextコンテキスト
                file                ファイル構造体

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

  Description:  FSのパス名からHIOのフルパス名へ変換

  Arguments:    context             DEMOHIOContextコンテキスト
                dst                 フルパス格納先
                relpath             ルートからの相対パス

  Returns:      コマンドの処理結果
 *---------------------------------------------------------------------------*/
static void DEMOi_HIO_CreateFullPath(DEMOHIOContext *context, char *dst, const char *relpath)
{
    // パス名に":"が含まれていればそのまま使用
    if (STD_SearchString(relpath, ":") != NULL)
    {
        (void)STD_TSPrintf(dst, "%s", relpath);
    }
    // そうでなければルートディレクトリと相対パスを連結
    else
    {
        (void)STD_TSPrintf(dst, "%s/%s", context->basepath, relpath);
    }
    // 終端に付与された'/'があれば除去
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

  Description:  HIOのエラーコードをFSREsultへ変換

  Arguments:    error               エラーコード

  Returns:      FSREsultに変換されたエラーコード
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

  Description:  HIOの時刻構造体をFSDateTimeへ変換

  Arguments:    dst                 変換先のFSDateTime
                src                 変換元のCFIODateTime

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
 * 以下、FSコマンドインタフェース
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_GetArchiveCaps

  Description:  FS_COMMAND_GETARCHIVECAPSコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                caps                デバイス能力フラグの格納先

  Returns:      コマンドの処理結果
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_GetArchiveCaps(FSArchive *arc, u32 *caps)
{
    (void)arc;
    *caps = 0;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_GetPathInfo

  Description:  FS_COMMAND_GETPATHINFOコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                baseid              基点ディレクトリID (ルートなら0)
                relpath             パス
                info                ファイル情報の格納先

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_CREATE_FILEコマンド

  Arguments:

  Returns:      コマンドの処理結果
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_CreateFile(FSArchive *arc, u32 baseid, const char *relpath, u32 permit)
{
    FSResult        result = FS_RESULT_ERROR;
    DEMOHIOContext   *context = (DEMOHIOContext *)FS_GetArchiveUserData(arc);
    char           *tmppath = context->fullpath[0];
    // ファイル生成はOpenで代用。
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

  Description:  FS_COMMAND_DELETE_FILEコマンド

  Arguments:

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_RENAME_FILEコマンドまたはFS_COMMAND_RENAME_DIRECTORYコマンド

  Arguments:

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_CREATE_DIRECTORYコマンド

  Arguments:

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_DELETE_DIRECTORYコマンド

  Arguments:

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_OPENFILEコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル
                baseid              基点ディレクトリ (ルートなら0)
                path                ファイルパス
                mode                アクセスモード

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_CLOSEFILEコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_READFILEコマンド

  Arguments:    arc              呼び出し元アーカイブ
                file             対象ファイル
                buffer           転送先メモリ
                length           転送サイズ

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_WRITEFILEコマンド

  Arguments:    arc              呼び出し元アーカイブ
                file             対象ファイル
                buffer           転送元メモリ
                length           転送サイズ

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_SEEKFILEコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル
                offset              移動量および移動後の位置
                from                シーク起点

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_GETFILELENGTHコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル
                length              取得したサイズの格納先

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_GETFILEPOSITIONコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル
                position            取得した位置の格納先

  Returns:      コマンドの処理結果
 *---------------------------------------------------------------------------*/
static FSResult DEMOi_HIO_GetFilePosition(FSArchive *arc, FSFile *file, u32 *position)
{
    *position = 0;
    return DEMOi_HIO_SeekFile(arc, file, (int *)position, FS_SEEK_CUR);
}

/*---------------------------------------------------------------------------*
  Name:         DEMOi_HIO_OpenDirectory

  Description:  FS_COMMAND_OPENDIRECTORYコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル
                baseid              基点ディレクトリID (ルートなら0)
                path                パス
                mode                アクセスモード

  Returns:      コマンドの処理結果
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
        // 特殊なワイルドカード指定を無効化
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

  Description:  FS_COMMAND_CLOSEDIRECTORYコマンド

  Arguments:    arc                 呼び出し元アーカイブ
                file                対象ファイル

  Returns:      コマンドの処理結果
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

  Description:  FS_COMMAND_READDIRコマンド

  Arguments:    arc              呼び出し元アーカイブ
                file             対象ファイル
                info             情報の格納先

  Returns:      コマンドの処理結果
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

  Description:  IS-TWL-DEBUGGERのHostI/Oファイルシステムを
                "hostio:" という名前でFSにマウントする。

  Arguments:    basepath    基準となるデバッグホスト側のルートディレクトリ。
                            FSのパス名に "hostio:/relpath" と指定した場合
                            ホスト側の "basepath/relpath" を意味する。

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DEMOMountHostIO(const char *basepath)
{

#ifndef SDK_LINK_ISTD

    // コンパイル環境にIS-TWL-DEBUGGERソフトウェアがインストールされていなければ
    // HostI/OアクセスAPIの定義を参照することはできない。
    (void)basepath;
    OS_TWarning("cannot use HostI/O on this compile-time environment.\n");

#else // SDK_LINK_ISTD

    // 実行環境がIS-TWL-DEBUGGERソフトウェアでなければ
    // HostI/O機能を実際に利用することはできない。
    if ((OS_GetConsoleType() & OS_CONSOLE_TWLDEBUGGER) == 0)
    {
        OS_TWarning("cannot use HostI/O on this run-time environment.\n");
    }

    // リンク環境にIS-TWL-DEBUGGERソフトウェアがインストールされていなければ
    // たとえ実行環境がIS-TWL-DEBUGGERでもHostI/Oを利用することはできない。
    else if (ISTDHIOInit(), !ISTDHIOOpen(ISTDHIODEVMASK_AUTOSELECT))
    {
        OS_TWarning("failed to open HostI/O device.\n");
    }

    // コンパイル環境とリンク環境の両方にIS-TWL-DEBUGGERのライブラリが存在し
    // かつ実行環境がIS-TWL-DEBUGGERであった場合のみHostI/Oが利用可能。
    else
    {
        static const FSArchiveInterface FSiArchiveHIOInterface =
        {
            // 旧仕様と互換性のあるコマンド
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
            // 旧仕様と互換性があるがコマンドではなかったもの
            DEMOi_HIO_OpenFile,
            DEMOi_HIO_SeekFile,
            DEMOi_HIO_GetFileLength,
            DEMOi_HIO_GetFilePosition,
            // 新仕様で拡張されたコマンド
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
