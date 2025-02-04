/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WFS - libraries
  File:     wfs_archive.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/wfs/client.h>


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         WFSi_ArchiveReadCallback

  Description:  子機WFSアーカイブのリードアクセスコールバック.

  Arguments:    archive          FSArchive構造体.
                buffer           転送先.
                offset           転送元.
                length           転送サイズ.

  Returns:      処理結果.
 *---------------------------------------------------------------------------*/
static FSResult WFSi_ArchiveReadCallback(FSArchive *archive, void *buffer, u32 offset, u32 length)
{
    FSResult    result = FS_RESULT_ERROR;
    WFSClientContext * const context = (WFSClientContext*)FS_GetArchiveBase(archive);
    const WFSTableFormat * const table = WFS_GetTableFormat(context);
    if (table)
    {
        MI_CpuCopy8(&table->buffer[offset], buffer, length);
        result = FS_RESULT_SUCCESS;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_ArchiveReadDoneCallback

  Description:  子機WFSアーカイブのリード完了コールバック.

  Arguments:    context          WFSClientContext構造体.
                succeeded        読み込みに成功したらTRUE, 失敗したらFALSE.
                arg              コールバックに指定された引数.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_ArchiveReadDoneCallback(WFSClientContext *context, BOOL succeeded, void *arg)
{
    FSFile * const      file = (FSFile*)arg;
    FSResult            result = FS_RESULT_ERROR;
    if (succeeded)
    {
        file->prop.file.pos += file->arg.readfile.len;
        result = FS_RESULT_SUCCESS;
    }
    FS_NotifyArchiveAsyncEnd(FS_GetAttachedArchive(file), result);
    (void)context;
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_RomArchiveProc

  Description:  子機ROMアーカイブのユーザプロシージャ.

  Arguments:    file             FSFile構造体.
                command          コマンド.

  Returns:      処理結果.
 *---------------------------------------------------------------------------*/
static FSResult WFSi_RomArchiveProc(FSFile *file, FSCommandType command)
{
    FSResult    result = FS_RESULT_ERROR;
    switch (command)
    {
    case FS_COMMAND_READFILE:
        {
            void   *buffer = file->arg.readfile.dst;
            u32     offset = file->prop.file.pos;
            u32     length = file->arg.readfile.len;
            if (length == 0)
            {
                result = FS_RESULT_SUCCESS;
            }
            else
            {
                FSArchive * const archive = FS_GetAttachedArchive(file);
                WFSClientContext * const context = (WFSClientContext*)FS_GetArchiveBase(archive);
                const WFSTableFormat * const table = WFS_GetTableFormat(context);
                if (table != NULL)
                {
                    WFS_RequestClientRead(context, buffer, offset, length,
                                          WFSi_ArchiveReadDoneCallback, file);
                    result = FS_RESULT_PROC_ASYNC;
                }
            }
        }
        break;
    case FS_COMMAND_WRITEFILE:
        result = FS_RESULT_UNSUPPORTED;
        break;
    default:
        result = FS_RESULT_PROC_UNKNOWN;
        break;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_EmptyArchiveProc

  Description:  WFS_EndClient()呼び出し後の空のプロシージャ。

  Arguments:    file             FSFile構造体
                command          コマンド

  Returns:      処理結果
 *---------------------------------------------------------------------------*/
static FSResult WFSi_EmptyArchiveProc(FSFile *file, FSCommandType command)
{
    FSResult    result = FS_RESULT_PROC_UNKNOWN;
    // 無視して問題ないコマンド以外は警告メッセージを出力してエラーとする。
    if ((command != FS_COMMAND_CLOSEFILE) &&
        ((command < FS_COMMAND_STATUS_BEGIN) || (command >= FS_COMMAND_STATUS_END)))
    {
        static BOOL once = FALSE;
        if (!once)
        {
            once = TRUE;
            OS_TWarning("WFS-Client has been finalized. (all the commands will fail)\n");
        }
        result = FS_RESULT_ERROR;
    }
    (void)file;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WFSi_RestoreRomArchive

  Description:  WFS_EndClient()呼び出し時のアンマウントコールバック。

  Arguments:    context          WFSClientContext構造体

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WFSi_RestoreRomArchive(WFSClientContext *context)
{
    FSArchive *archive = FS_FindArchive("rom", 3);
    (void)FS_SuspendArchive(archive);
    FS_SetArchiveProc(archive, WFSi_EmptyArchiveProc, (u32)FS_ARCHIVE_PROC_ALL);
    (void)FS_ResumeArchive(archive);
    (void)context;
}

/*---------------------------------------------------------------------------*
  Name:         WFS_ReplaceRomArchive

  Description:  WFSのアーカイブをマウント.

  Arguments:    context          WFSClientContext構造体.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WFS_ReplaceRomArchive(WFSClientContext *context)
{
    const WFSTableFormat * const table = WFS_GetTableFormat(context);
    if (table != NULL)
    {
        FSArchive *archive = FS_FindArchive("rom", 3);
        if (FS_IsArchiveLoaded(archive))
        {
            (void)FS_UnloadArchive(archive);
        }
        FS_SetArchiveProc(archive, WFSi_RomArchiveProc, (u32)FS_ARCHIVE_PROC_ALL);
        context->unmount_callback = WFSi_RestoreRomArchive;
        (void)FS_LoadArchive(archive, (u32)context,
                             table->region[WFS_TABLE_REGION_FAT].offset,
                             table->region[WFS_TABLE_REGION_FAT].length,
                             table->region[WFS_TABLE_REGION_FNT].offset,
                             table->region[WFS_TABLE_REGION_FNT].length,
                             WFSi_ArchiveReadCallback, NULL);
        FS_AttachOverlayTable(MI_PROCESSOR_ARM9,
                              table->buffer +
                              table->region[WFS_TABLE_REGION_OV9].offset,
                              table->region[WFS_TABLE_REGION_OV9].length);
        FS_AttachOverlayTable(MI_PROCESSOR_ARM7,
                              table->buffer +
                              table->region[WFS_TABLE_REGION_OV7].offset,
                              table->region[WFS_TABLE_REGION_OV7].length);
    }
}


/*---------------------------------------------------------------------------*
  $Log: wfs_archive.c,v $
  Revision 1.1  2007/06/11 06:38:39  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
