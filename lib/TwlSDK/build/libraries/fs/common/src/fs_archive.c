/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     fs_archive.c

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


#include <nitro/fs.h>

#include <nitro/mi/memory.h>
#include <nitro/std.h>


#include "../include/util.h"
#include "../include/command.h"


#if defined(FS_IMPLEMENT)


/*---------------------------------------------------------------------------*/
/* constants */

// �����A�[�J�C�u��������I�ɃT�|�[�g����ꍇ�͗L���ɂ���
#define FS_SUPPORT_LONG_ARCNAME


/*---------------------------------------------------------------------------*/
/* variables */

// �t�@�C���V�X�e���ɓo�^�ς݂̃A�[�J�C�u���X�g
static FSArchive   *arc_list = NULL;

// �J�����g�f�B���N�g��
static FSDirPos     current_dir_pos;
static char         current_dir_path[FS_ENTRY_LONGNAME_MAX];

#if defined(FS_SUPPORT_LONG_ARCNAME)
// �����A�[�J�C�u��������I�ɃT�|�[�g���邽�߂̐ÓI�o�b�t�@
#define FS_LONG_ARCNAME_LENGTH_MAX  15
#define FS_LONG_ARCNAME_TABLE_MAX   16
static char FSiLongNameTable[FS_LONG_ARCNAME_TABLE_MAX][FS_LONG_ARCNAME_LENGTH_MAX + 1];
#endif


/*---------------------------------------------------------------------------*/
/* functions */

FSArchive* FSi_GetArchiveChain(void)
{
    return arc_list;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsEventCommand

  Description:  �R�}���h���C�x���g�ʒm������

  Arguments:    command          �R�}���h���

  Returns:      �R�}���h���C�x���g�ʒm�Ȃ�TRUE
 *---------------------------------------------------------------------------*/
static BOOL FSi_IsEventCommand(FSCommandType command)
{
    return
        ((command == FS_COMMAND_ACTIVATE) ||
        (command == FS_COMMAND_IDLE) ||
        (command == FS_COMMAND_SUSPEND) ||
        (command == FS_COMMAND_RESUME) ||
        (command == FS_COMMAND_MOUNT) ||
        (command == FS_COMMAND_UNMOUNT) ||
        (command == FS_COMMAND_INVALID));
}

/*---------------------------------------------------------------------------*
  Name:         FSi_EndCommand

  Description:  �R�}���h���������ҋ@�X���b�h������Ε��A

  Arguments:    file             ���������R�}���h
                ret              �R�}���h�̌��ʒl

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_EndCommand(FSFile *file, FSResult ret)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    // �R�}���h�����X�g����폜
    FSArchive *const arc = file->arc;
    if (arc)
    {
        FSFile    **pp = &arc->list;
        for (; *pp; pp = &(*pp)->next)
        {
            if (*pp == file)
            {
                *pp = file->next;
                break;
            }
        }
        file->next = NULL;
    }
    // ���ʂ��i�[
    {
        FSCommandType   command = FSi_GetCurrentCommand(file);
        if (!FSi_IsEventCommand(command))
        {
            arc->command = command;
            arc->result = ret;
        }
        file->error = ret;
        file->stat &= ~(FS_FILE_STATUS_CANCEL | FS_FILE_STATUS_BUSY |
                        FS_FILE_STATUS_BLOCKING | FS_FILE_STATUS_OPERATING |
                        FS_FILE_STATUS_ASYNC_DONE | FS_FILE_STATUS_UNICODE_MODE);
    }
    // �ҋ@���̃X���b�h������Βʒm
    OS_WakeupThread(file->queue);
    (void)OS_RestoreInterrupts(bak_psr);
}


// �v���V�[�W���̂��߂Ɉꎞ�I�Ɉȉ���2�����J�B
// �v���V�[�W������ċA�I�ɌĂяo���Ă���y�͑S�������R�}���h�Ȃ̂ŁA
// ����vtbl�o�R�ŌĂяo����FSi_WaitForArchiveCompletion()���邾���ł��ǂ��B

FSResult FSi_WaitForArchiveCompletion(FSFile *file, FSResult result)
{
    if (result == FS_RESULT_PROC_ASYNC)
    {
        FSi_WaitConditionOn(&file->stat, FS_FILE_STATUS_ASYNC_DONE, file->queue);
        file->stat &= ~FS_FILE_STATUS_ASYNC_DONE;
        result = file->error;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_InvokeCommand

  Description:  �A�[�J�C�u�ɃR�}���h�𔭍s����B
                �R�}���h���X�g�̊Ǘ��͂��̊֐��Ăяo���̑O�ɉ������Ă���B

  Arguments:    file             �������i�[����FSFile�\����
                command          �R�}���hID

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_InvokeCommand(FSFile *file, FSCommandType command)
{
    FSResult            result = FS_RESULT_UNSUPPORTED;
    FSArchive   * const arc = file->arc;

    {
        const void   *(*table) = (const void*)arc->vtbl;
        // �͈͊O�̖���`�R�}���h
        if ((command < 0) || (command >= FS_COMMAND_MAX))
        {
            OS_TWarning("undefined command (%d)\n", command);
            result = FS_RESULT_UNSUPPORTED;
        }
        // �h���C�o���Ŏ�������������R�}���h�łȂ���Έ����𕜌����ČĂяo��
        else if (table[command] == NULL)
        {
            result = FS_RESULT_UNSUPPORTED;
        }
        else
        {
#define FS_DECLARE_ARGUMENT_(type)                              \
            type *arg = (type*) file->argument;                 \
            (void)arg

#define FS_INVOKE_METHOD_(command, ...)                         \
            do                                                  \
            {                                                   \
                FS_DECLARE_ARGUMENT_(FSArgumentFor ## command); \
                result = arc->vtbl->command(__VA_ARGS__);       \
            }                                                   \
            while(0)
#define FS_NOTIFY_EVENT_(command, ...)                          \
            do                                                  \
            {                                                   \
                FS_DECLARE_ARGUMENT_(FSArgumentFor ## command); \
                (void)arc->vtbl->command(__VA_ARGS__);          \
                return FS_RESULT_SUCCESS;                       \
            }                                                   \
            while(0)
            switch (command)
            {
            case FS_COMMAND_READFILE:
                FS_INVOKE_METHOD_(ReadFile, arc, file, arg->buffer, &arg->length);
                break;
            case FS_COMMAND_WRITEFILE:
                FS_INVOKE_METHOD_(WriteFile, arc, file, arg->buffer, &arg->length);
                break;
            case FS_COMMAND_SEEKDIR:
                FS_INVOKE_METHOD_(SeekDirectory, arc, file, arg->id, arg->position);
                break;
            case FS_COMMAND_READDIR:
                FS_INVOKE_METHOD_(ReadDirectory, arc, file, arg->info);
                break;
            case FS_COMMAND_FINDPATH:
                FS_INVOKE_METHOD_(FindPath, arc, arg->baseid, arg->relpath, &arg->target_id, arg->target_is_directory);
                break;
            case FS_COMMAND_GETPATH:
                FS_INVOKE_METHOD_(GetPath, arc, file, arg->is_directory, arg->buffer, &arg->length);
                break;
            case FS_COMMAND_OPENFILEFAST:
                FS_INVOKE_METHOD_(OpenFileFast, arc, file, arg->id, arg->mode);
                break;
            case FS_COMMAND_OPENFILEDIRECT:
                FS_INVOKE_METHOD_(OpenFileDirect, arc, file, arg->top, arg->bottom, &arg->id);
                break;
            case FS_COMMAND_CLOSEFILE:
                FS_INVOKE_METHOD_(CloseFile, arc, file);
                break;
            case FS_COMMAND_ACTIVATE:
                FS_NOTIFY_EVENT_(Activate, arc);
                break;
            case FS_COMMAND_IDLE:
                FS_NOTIFY_EVENT_(Idle, arc);
                break;
            case FS_COMMAND_SUSPEND:
                FS_NOTIFY_EVENT_(Suspend, arc);
                break;
            case FS_COMMAND_RESUME:
                FS_NOTIFY_EVENT_(Resume, arc);
                break;
            case FS_COMMAND_OPENFILE:
                FS_INVOKE_METHOD_(OpenFile, arc, file, arg->baseid, arg->relpath, arg->mode);
                break;
            case FS_COMMAND_SEEKFILE:
                FS_INVOKE_METHOD_(SeekFile, arc, file, &arg->offset, arg->from);
                break;
            case FS_COMMAND_GETFILELENGTH:
                FS_INVOKE_METHOD_(GetFileLength, arc, file, &arg->length);
                break;
            case FS_COMMAND_GETFILEPOSITION:
                FS_INVOKE_METHOD_(GetFilePosition, arc, file, &arg->position);
                break;
                // ��������͊g���R�}���h
            case FS_COMMAND_MOUNT:
                FS_NOTIFY_EVENT_(Mount, arc);
                break;
            case FS_COMMAND_UNMOUNT:
                FS_NOTIFY_EVENT_(Unmount, arc);
                break;
            case FS_COMMAND_GETARCHIVECAPS:
                FS_INVOKE_METHOD_(GetArchiveCaps, arc, &arg->caps);
                break;
            case FS_COMMAND_CREATEFILE:
                FS_INVOKE_METHOD_(CreateFile, arc, arg->baseid, arg->relpath, arg->permit);
                break;
            case FS_COMMAND_DELETEFILE:
                FS_INVOKE_METHOD_(DeleteFile, arc, arg->baseid, arg->relpath);
                break;
            case FS_COMMAND_RENAMEFILE:
                FS_INVOKE_METHOD_(RenameFile, arc, arg->baseid_src, arg->relpath_src, arg->baseid_dst, arg->relpath_dst);
                break;
            case FS_COMMAND_GETPATHINFO:
                FS_INVOKE_METHOD_(GetPathInfo, arc, arg->baseid, arg->relpath, arg->info);
                break;
            case FS_COMMAND_SETPATHINFO:
                FS_INVOKE_METHOD_(SetPathInfo, arc, arg->baseid, arg->relpath, arg->info);
                break;
            case FS_COMMAND_CREATEDIRECTORY:
                FS_INVOKE_METHOD_(CreateDirectory, arc, arg->baseid, arg->relpath, arg->permit);
                break;
            case FS_COMMAND_DELETEDIRECTORY:
                FS_INVOKE_METHOD_(DeleteDirectory, arc, arg->baseid, arg->relpath);
                break;
            case FS_COMMAND_RENAMEDIRECTORY:
                FS_INVOKE_METHOD_(RenameDirectory, arc, arg->baseid_src, arg->relpath_src, arg->baseid_dst, arg->relpath_dst);
                break;
            case FS_COMMAND_GETARCHIVERESOURCE:
                FS_INVOKE_METHOD_(GetArchiveResource, arc, arg->resource);
                break;
            case FS_COMMAND_FLUSHFILE:
                FS_INVOKE_METHOD_(FlushFile, arc, file);
                break;
            case FS_COMMAND_SETFILELENGTH:
                FS_INVOKE_METHOD_(SetFileLength, arc, file, arg->length);
                break;
            case FS_COMMAND_OPENDIRECTORY:
                FS_INVOKE_METHOD_(OpenDirectory, arc, file, arg->baseid, arg->relpath, arg->mode);
                break;
            case FS_COMMAND_CLOSEDIRECTORY:
                FS_INVOKE_METHOD_(CloseDirectory, arc, file);
                break;
            case FS_COMMAND_SETSEEKCACHE:
                FS_INVOKE_METHOD_(SetSeekCache, arc, file, arg->buf, arg->buf_size);
                break;
            default:
                result = FS_RESULT_UNSUPPORTED;
            }
#undef FS_DECLARE_ARGUMENT_
#undef FS_INVOKE_METHOD_
#undef FS_NOTIFY_EVENT_
        }
    }
    // �C�x���g�ʒm�łȂ��ʏ�̃R�}���h�Ȃ甭�s���̏����𖞂����悤�ɕԂ��B
    if (!FSi_IsEventCommand(command))
    {
        // �p�X�����ԈႦ��UNSUPPORTED�Ƃ����P�[�X�������̂ŁA
        // ���̉\��������Όx���o�͂����Ă����B
        if (result == FS_RESULT_UNSUPPORTED)
        {
            OS_TWarning("archive \"%s:\" cannot support command %d.\n", FS_GetArchiveName(arc), command);
        }
        // �K�v�Ȃ�u���b�L���O
        if ((file->stat & FS_FILE_STATUS_BLOCKING) != 0)
        {
            result = FSi_WaitForArchiveCompletion(file, result);
        }
        // �N���u���b�L���O���Ȃ��܂܊��������ꍇ�͂����ŉ��
        else if (result != FS_RESULT_PROC_ASYNC)
        {
            FSi_EndCommand(file, result);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_NextCommand

  Description:  �A�[�J�C�u�����ɏ������ׂ��R�}���h��I������B
                �񓯊��R�}���h���I�����ꂽ�炻�̃|�C���^��Ԃ��B
                NULL �ȊO���Ԃ��ꂽ�ꍇ�͌Ăяo�����ŏ�������K�v������B

  Arguments:    arc              ���̃R�}���h���擾����A�[�J�C�u
                owner            �Ăяo���������łɃR�}���h���s���������Ă����TRUE

  Returns:      ���̏�ŏ�����K�v�Ƃ��鎟�̃R�}���h
 *---------------------------------------------------------------------------*/
static FSFile *FSi_NextCommand(FSArchive *arc, BOOL owner)
{
    FSFile  *next = NULL;
    // �܂��S�ẴR�}���h�̃L�����Z���v�����`�F�b�N
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        if ((arc->flag & FS_ARCHIVE_FLAG_CANCELING) != 0)
        {
            FSFile *p = arc->list;
            arc->flag &= ~FS_ARCHIVE_FLAG_CANCELING;
            while (p != NULL)
            {
                FSFile *q = p->next;
                if (FS_IsCanceling(p))
                {
                    FSi_EndCommand(p, FS_RESULT_CANCELED);
                    if (!q)
                    {
                        q = arc->list;
                    }
                }
                p = q;
            }
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
    // ���̃R�}���h�����s�\������
    {
        OSIntrMode  bak_psr = OS_DisableInterrupts();
        if (((arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) == 0) &&
            ((arc->flag & FS_ARCHIVE_FLAG_SUSPEND) == 0) && arc->list)
        {
            // �A�[�J�C�u����~���Ă����炱���ŋN��
            const BOOL  is_started = owner && ((arc->flag & FS_ARCHIVE_FLAG_RUNNING) == 0);
            if (is_started)
            {
                arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
            }
            (void)OS_RestoreInterrupts(bak_psr);
            if (is_started)
            {
                (void)FSi_InvokeCommand(arc->list, FS_COMMAND_ACTIVATE);
            }
            bak_psr = OS_DisableInterrupts();
            // ���X�g�擪�̃R�}���h�����s�����l��
            if (owner || is_started)
            {
                next = arc->list;
                next->stat |= FS_FILE_STATUS_OPERATING;
            }
            // �R�}���h���s�����u���b�L���O���Ȃ���s�����Ϗ�
            if (owner && ((next->stat & FS_FILE_STATUS_BLOCKING) != 0))
            {
                OS_WakeupThread(next->queue);
                next = NULL;
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
        // �T�X�y���h�܂��̓A�C�h�����
        else
        {
            // ���s���������Ă����Ȃ��~���ăA�C�h����Ԃ�
            if (owner)
            {
                if ((arc->flag & FS_ARCHIVE_FLAG_RUNNING) != 0)
                {
                    FSFile  tmp;
                    FS_InitFile(&tmp);
                    tmp.arc = arc;
                    arc->flag &= ~FS_ARCHIVE_FLAG_RUNNING;
                    (void)FSi_InvokeCommand(&tmp, FS_COMMAND_IDLE);
                }
                // �T�X�y���h���s���ł���΃T�X�y���h�֐��̌Ăяo�����֒ʒm
                if ((arc->flag & FS_ARCHIVE_FLAG_SUSPENDING) != 0)
                {
                    arc->flag &= ~FS_ARCHIVE_FLAG_SUSPENDING;
                    arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
                    OS_WakeupThread(&arc->queue);
                }
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
    }
    return next;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteAsyncCommand

  Description:  �񓯊��n�R�}���h�̎��s�B
                �ŏ��� 1 ��̓��[�U�X���b�h���犄�荞�݋��ŌĂ΂��B
                �A�[�J�C�u�������I�ɓ��삷����肱���ŃR�}���h�������J��Ԃ�
                1 ��ł��񓯊������ɂȂ�Α����� NotifyAsyncEnd() �ōs���B

                ����ăA�[�J�C�u���������� / �񓯊��Ő؂�ւ��ꍇ��
                NotifyAsyncEnd() �̌Ăяo�����ɒ��ӂ���K�v������B

  Arguments:    file             ���s����񓯊��R�}���h���i�[���� FSFile �\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ExecuteAsyncCommand(FSFile *file)
{
    FSArchive   * const arc = file->arc;
    while (file)
    {
        // �Ăяo�����������҂��Ȃ炻����N�����ď�����C����
        {
            OSIntrMode bak_psr = OS_DisableInterrupts();
            file->stat |= FS_FILE_STATUS_OPERATING;
            if ((file->stat & FS_FILE_STATUS_BLOCKING) != 0)
            {
                OS_WakeupThread(file->queue);
                file = NULL;
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
        if (!file)
        {
            break;
        }
        // �������񓯊��Ȃ�ꎞ�I��
        else if (FSi_InvokeCommand(file, FSi_GetCurrentCommand(file)) == FS_RESULT_PROC_ASYNC)
        {
            break;
        }
        // ���ʂ����������Ȃ炱���ő�����I��
        else
        {
            file = FSi_NextCommand(arc, TRUE);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteSyncCommand

  Description:  �u���b�L���O�����R�}���h�̎��s�B

  Arguments:    file             ���s����R�}���h���i�[����FSFile�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ExecuteSyncCommand(FSFile *file)
{
    FSArchive   * const arc = file->arc;
    FSResult            result;
    // �K�v�Ȃ珇�Ԃ�҂��Ă���R�}���h����������
    FSi_WaitConditionOn(&file->stat, FS_FILE_STATUS_OPERATING, file->queue);
    result = FSi_InvokeCommand(file, FSi_GetCurrentCommand(file));
    FSi_EndCommand(file, result);
    // �����ŏ������ׂ��񓯊��^�̃R�}���h������΂����Ɏ��s
    file = FSi_NextCommand(arc, TRUE);
    if (file)
    {
        FSi_ExecuteAsyncCommand(file);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SendCommand

  Description:  �A�[�J�C�u�փR�}���h�𔭍s����B
                �N���^�C�~���O�̒����ƂƂ���, �����n�Ȃ炱���Ńu���b�L���O�B

  Arguments:    file             �R�}���h�������w�肳�ꂽ FSFile �\����
                command          �R�}���h ID
                blocking         �u���b�L���O����Ȃ�TRUE

  Returns:      �R�}���h����������� TRUE
 *---------------------------------------------------------------------------*/
BOOL FSi_SendCommand(FSFile *file, FSCommandType command, BOOL blocking)
{
    BOOL                retval = FALSE;
    FSArchive   * const arc = file->arc;
    BOOL                owner = FALSE;

    if (FS_IsBusy(file))
    {
        OS_TPanic("specified file is now still proceccing previous command!");
    }
    if (!arc)
    {
        OS_TWarning("specified handle is not related by any archive\n");
        file->error = FS_RESULT_INVALID_PARAMETER;
        return FALSE;
    }

    // �R�}���h�̏�����
    file->error = FS_RESULT_BUSY;
    file->stat &= ~(FS_FILE_STATUS_CMD_MASK << FS_FILE_STATUS_CMD_SHIFT);
    file->stat |= (command << FS_FILE_STATUS_CMD_SHIFT);
    file->stat |= FS_FILE_STATUS_BUSY;
    file->next = NULL;
    if (blocking)
    {
        file->stat |= FS_FILE_STATUS_BLOCKING;
    }
    // �A�����[�h���Ȃ珈�����L�����Z�����A�����łȂ���΃��X�g�I�[�ɒǉ�
    {
        OSIntrMode          bak_psr = OS_DisableInterrupts();
        if ((arc->flag & FS_ARCHIVE_FLAG_UNLOADING) != 0)
        {
            FSi_EndCommand(file, FS_RESULT_CANCELED);
        }
        else
        {
            FSFile    **pp;
            for (pp = &arc->list; *pp; pp = &(*pp)->next)
            {
            }
            *pp = file;
        }
        owner = (arc->list == file) && ((arc->flag & FS_ARCHIVE_FLAG_RUNNING) == 0);
        (void)OS_RestoreInterrupts(bak_psr);
    }
    // ���X�g�ɒǉ����ꂽ�Ȃ�R�}���h���s�����`�F�b�N
    if (file->error != FS_RESULT_CANCELED)
    {
        // �����ŏ������ׂ��R�}���h������΂����Ɏ��s
        FSFile *next = FSi_NextCommand(arc, owner);
        // �K�v�Ȃ炱���Ńu���b�L���O
        if (blocking)
        {
            FSi_ExecuteSyncCommand(file);
            retval = FS_IsSucceeded(file);
        }
        // ���X�g�ɒǉ����ꂽ�̂ł�����N���ɏ��������
        else
        {
            if (next != NULL)
            {
                FSi_ExecuteAsyncCommand(next);
            }
            retval = TRUE;
        }
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_EndArchive

  Description:  �S�ẴA�[�J�C�u���I�������ĉ��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_EndArchive(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    while (arc_list)
    {
        FSArchive *p_arc = arc_list;
        arc_list = arc_list->next;
        (void)FS_UnloadArchive(p_arc);
        FS_ReleaseArchiveName(p_arc);
    }
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         FS_FindArchive

  Description:  �A�[�J�C�u������������B
                ��v���閼�O��������� NULL ��Ԃ��B

  Arguments:    name             ��������A�[�J�C�u���̕�����
                name_len         name �̕�����

  Returns:      �������Č��������A�[�J�C�u�̃|�C���^�� NULL.
 *---------------------------------------------------------------------------*/
FSArchive *FS_FindArchive(const char *name, int name_len)
{
    OSIntrMode  bak_psr = OS_DisableInterrupts();
    FSArchive  *arc = arc_list;
    for (; arc; arc = arc->next)
    {
        if (FS_IsArchiveLoaded(arc))
        {
            const char *arcname = FS_GetArchiveName(arc);
            if ((STD_CompareNString(arcname, name, name_len) == 0) && (arcname[name_len] == '\0'))
            {
                break;
            }
        }
    }
    (void)OS_RestoreInterrupts(bak_psr);
    return arc;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveResultCode

  Description:  �w�肵���A�[�J�C�u�̍ŐV�̃G���[�R�[�h���擾�B

  Arguments:    path_or_archive  �Ώۂ̃A�[�J�C�u���w��
                                 FSArchive�\���̂܂��̓p�X������

  Returns:      �w�肳�ꂽ�A�[�J�C�u�̍ŐV�̃G���[�R�[�h�B
                �Y������A�[�J�C�u�����݂��Ȃ��ꍇ��FS_RESULT_ERROR
 *---------------------------------------------------------------------------*/
FSResult FS_GetArchiveResultCode(const void *path_or_archive)
{
    OSIntrMode  bak_psr = OS_DisableInterrupts();
    FSArchive  *arc = arc_list;
    while (arc && (arc != (const FSArchive *)path_or_archive))
    {
        arc = arc->next;
    }
    if (!arc)
    {
        arc = FS_NormalizePath((const char *)path_or_archive, NULL, NULL);
    }
    (void)OS_RestoreInterrupts(bak_psr);
    return arc ? arc->result : FS_RESULT_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetCurrentDirectoryPath

  Description:  ���݂̃f�B���N�g�����p�X���Ŏ擾�B

  Arguments:    None.

  Returns:      ���݂̃f�B���N�g����\��������
 *---------------------------------------------------------------------------*/
const char *FS_GetCurrentDirectory(void)
{
    return current_dir_path;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetCurrentDirectory

  Description:  �J�����g�f�B���N�g����ύX����

  Arguments:    path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_SetCurrentDirectory(const char *path)
{
    BOOL        retval = FALSE;
    FSArchive  *arc = NULL;
    u32         baseid = 0;
    char        relpath[FS_ENTRY_LONGNAME_MAX];

    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

    // ���K�����ꂽ�p�X�����擾
    arc = FS_NormalizePath(path, &baseid, relpath);
    if (arc)
    {
        // ������ɂ���ݒ�͐���
        current_dir_pos.arc = arc;
        current_dir_pos.own_id = 0;
        current_dir_pos.index = 0;
        current_dir_pos.pos = 0;
        (void)STD_CopyLString(current_dir_path, relpath, sizeof(current_dir_path));
        // �\�Ȃ�f�B���N�g��ID�̎擾�����s
        if (arc->vtbl->FindPath != NULL)
        {
            FSFile                  dir[1];
            FSArgumentForFindPath   arg[1];
            FS_InitFile(dir);
            dir->arc = arc;
            dir->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->target_is_directory = TRUE;
            if (FSi_SendCommand(dir, FS_COMMAND_FINDPATH, TRUE))
            {
                current_dir_pos.own_id = (u16)arg->target_id;
                (void)STD_CopyLString(current_dir_path, relpath, sizeof(current_dir_path));
            }
        }
        retval = TRUE;
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_CopySafeString

  Description:  �o�b�t�@�T�C�Y���m�F����������R�s�[�B

  Arguments:    dst           �]����o�b�t�@
                dstlen        �]����T�C�Y
                src           �]�����o�b�t�@
                srclen        �]����T�C�Y
                stickyFailure �]�����̐؂�̂Ă��N��������FALSE
                
  Returns:      �A�[�J�C�u�|�C���^�܂���NULL
 *---------------------------------------------------------------------------*/
static int FSi_CopySafeString(char *dst, int dstlen, const char *src, int srclen, BOOL *stickyFailure)
{
    int     i;
    int     n = (dstlen - 1 < srclen) ? (dstlen - 1) : srclen;
    for (i = 0; (i < n) && src[i]; ++i)
    {
        dst[i] = src[i];
    }
    if ((i < srclen) && src[i])
    {
        *stickyFailure = TRUE;
    }
    dst[i] = '\0';
    return i;
}

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
FSArchive* FS_NormalizePath(const char *path, u32 *baseid, char *relpath)
{
    FSArchive  *arc = NULL;
    int         pathlen = 0;
    int         pathmax = FS_ENTRY_LONGNAME_MAX;
    BOOL        stickyFailure = FALSE;
    // �J�����g�f�B���N�g�����w�肳��Ă��Ȃ��ꍇ�͍ĕ]��
    if (current_dir_pos.arc == NULL)
    {
        current_dir_pos.arc = arc_list;
        current_dir_pos.own_id = 0;
        current_dir_pos.pos = 0;
        current_dir_pos.index = 0;
        current_dir_path[0] = '\0';
    }
    // "/path" �̏ꍇ�͌��݂̃A�[�J�C�u�̃��[�g����_�Ƃ���
    if (FSi_IsSlash((u8)*path))
    {
        arc = current_dir_pos.arc;
        ++path;
        if (baseid)
        {
            *baseid = 0;
        }
    }
    else
    {
        int     i;
        for (i = 0; ; i = FSi_IncrementSjisPosition(path, i))
        {
            u32     c = (u8)path[i];
            // "(path/)*path" �̏ꍇ�͌��݈ʒu����̑��΃p�X
            if (!c || FSi_IsSlash(c))
            {
                arc = current_dir_pos.arc;
                if (baseid)
                {
                    *baseid = current_dir_pos.own_id;
                }
                if(relpath)
                {
                    // �f�B���N�g�����p�X�������ŕێ�����Ă���Ȃ�A��
                    if ((current_dir_pos.own_id == 0) && (current_dir_path[0] != '\0'))
                    {
                        pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                                      current_dir_path, FS_ENTRY_LONGNAME_MAX, &stickyFailure);
                        pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                                      "/", 1, &stickyFailure);
                    }
                }
                break;
            }
            // "arc:/path" �̏ꍇ�̓t���p�X
            else if (c == ':')
            {
                arc = FS_FindArchive(path, i);
                if (!arc)
                {
                    OS_Warning("archive \"%*s\" is not found.", i, path);
                }
                path += i + 1;
                if (FSi_IsSlash((u8)*path))
                {
                    ++path;
                }
                if (baseid)
                {
                    *baseid = 0;
                }
                break;
            }
        }
    }
    if(relpath)
    {
        // ����G���g�����ɒ��ӂ����΃p�X�𐳋K���B
        int     curlen = 0;
        while (!stickyFailure)
        {
            char    c = path[curlen];
            if ((c != '\0') && !FSi_IsSlash((u8)c))
            {
                curlen += STD_IsSjisCharacter(&path[curlen]) ? 2 : 1;
            }
            else
            {
                // ��f�B���N�g���͖����B
                if (curlen == 0)
                {
                }
                // "." (�J�����g�f�B���N�g��)�͖����B
                else if ((curlen == 1) && (path[0] == '.'))
                {
                }
                // ".." (�e�f�B���N�g��)�̓��[�g������Ƃ���1�K�w�オ��B
                else if ((curlen == 2) && (path[0] == '.') && (path[1] == '.'))
                {
                    if (pathlen > 0)
                    {
                        --pathlen;
                    }
                    pathlen = FSi_DecrementSjisPositionToSlash(relpath, pathlen) + 1;
                }
                // ����ȊO�̓G���g���ǉ��B
                else
                {
                    pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                                  path, curlen, &stickyFailure);
                    if (c != '\0')
                    {
                        pathlen += FSi_CopySafeString(&relpath[pathlen], pathmax - pathlen,
                                                      "/", 1, &stickyFailure);
                    }
                }
                if (c == '\0')
                {
                    break;
                }
                path += curlen + 1;
                curlen = 0;
            }
        }
        relpath[pathlen] = '\0';
        pathlen = FSi_TrimSjisTrailingSlash(relpath);
    }
    return stickyFailure ? NULL : arc;
}

/*---------------------------------------------------------------------------*
  Name:         FS_InitArchive

  Description:  �A�[�J�C�u�\���̂�������

  Arguments:    p_arc            ����������A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_InitArchive(FSArchive *p_arc)
{
    SDK_NULL_ASSERT(p_arc);
    MI_CpuClear8(p_arc, sizeof(FSArchive));
    OS_InitThreadQueue(&p_arc->queue);
}

/*---------------------------------------------------------------------------*
  Name:         FS_RegisterArchiveName

  Description:  �A�[�J�C�u�����t�@�C���V�X�e���֓o�^��, �֘A�t����B
                �A�[�J�C�u���̂͂܂��t�@�C���V�X�e���Ƀ��[�h����Ȃ��B
                �A�[�J�C�u�� "rom" �̓t�@�C���V�X�e���ɗ\��ς݁B

  Arguments:    p_arc            ���O���֘A�t����A�[�J�C�u
                name             �o�^���閼�O�̕�����
                name_len         name �̕�����

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL FS_RegisterArchiveName(FSArchive *p_arc, const char *name, u32 name_len)
{
    BOOL    retval = FALSE;

    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(p_arc);
    SDK_NULL_ASSERT(name);

    {
        OSIntrMode bak_intr = OS_DisableInterrupts();
        if (!FS_FindArchive(name, (s32)name_len))
        {
            // ���X�g�̏I�[�ɒǉ�
            FSArchive **pp;
            for (pp = &arc_list; *pp; pp = &(*pp)->next)
            {
            }
            *pp = p_arc;
            // ���݂̎d�l�ł̓A�[�J�C�u�͍ő�3����
            if (name_len <= FS_ARCHIVE_NAME_LEN_MAX)
            {
                p_arc->name.pack = 0;
                (void)STD_CopyLString(p_arc->name.ptr, name, (int)(name_len + 1));
            }
            else
            {
#if defined(FS_SUPPORT_LONG_ARCNAME)
            // �A�[�J�C�u���̕�����������g������@�\�B
            // FSArchive�\���̂̃T�C�Y��ύX���Ȃ��őΉ����悤�Ƃ����
            // ���̂悤�ɊO���̃����������蓖�Ă�K�v������B
                if (name_len <= FS_LONG_ARCNAME_LENGTH_MAX)
                {
                    int i;
                    for (i = 0; ; ++i)
                    {
                        if (i >= FS_LONG_ARCNAME_TABLE_MAX)
                        {
                            OS_TPanic("failed to allocate memory for long archive-name(%.*s)!", name_len, name);
                        }
                        else if (FSiLongNameTable[i][0] == '\0')
                        {
                            (void)STD_CopyLString(FSiLongNameTable[i], name, (int)(name_len + 1));
                            p_arc->name.pack = (u32)FSiLongNameTable[i];
                            break;
                        }
                    }
                }
#endif
                // ��������A�[�J�C�u���͓o�^�ł��Ȃ��B
                else
                {
                    OS_TPanic("too long archive-name(%.*s)!", name_len, name);
                }
            }
            p_arc->flag |= FS_ARCHIVE_FLAG_REGISTER;
            retval = TRUE;
        }
        (void)OS_RestoreInterrupts(bak_intr);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReleaseArchiveName

  Description:  �o�^�ς݂̃A�[�J�C�u�����������B
                �t�@�C���V�X�e������A�����[�h����Ă���K�v������B

  Arguments:    p_arc            ���O���������A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_ReleaseArchiveName(FSArchive *p_arc)
{
    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(p_arc);

	if(p_arc == arc_list)
    {
        OS_TPanic("[file-system] cannot modify \"rom\" archive.\n");
    }

    if (p_arc->name.pack)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        // ���X�g����ؒf
        FSArchive **pp;
        for (pp = &arc_list; *pp; pp = &(*pp)->next)
        {
            if(*pp == p_arc)
            {
                *pp = (*pp)->next;
                break;
            }
        }
#if defined(FS_SUPPORT_LONG_ARCNAME)
        // �����A�[�J�C�u���ł���΃o�b�t�@���V�X�e���։��
        if (p_arc->name.ptr[3] != '\0')
        {
            ((char *)p_arc->name.pack)[0] = '\0';
        }
#endif
        p_arc->name.pack = 0;
        p_arc->next = NULL;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_REGISTER;
        // �J�����g�A�[�J�C�u�Ȃ����
        if (current_dir_pos.arc == p_arc)
        {
            current_dir_pos.arc = NULL;
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveName

  Description:  �A�[�J�C�u�����擾

  Arguments:    p_arc            ���O���擾����A�[�J�C�u

  Returns:      �t�@�C���V�X�e���ɓo�^���ꂽ�A�[�J�C�u��
 *---------------------------------------------------------------------------*/
const char *FS_GetArchiveName(const FSArchive *arc)
{
#if defined(FS_SUPPORT_LONG_ARCNAME)
    return (arc->name.ptr[3] != '\0') ? (const char *)arc->name.pack : arc->name.ptr;
#else
    return arc->name.ptr;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         FS_MountArchive

  Description:  �A�[�J�C�u���}�E���g����B

  Arguments:    arc              �}�E���g����A�[�J�C�u
                userdata         �A�[�J�C�u�Ɋ֘A�t���郆�[�U��`�ϐ�
                vtbl             �R�}���h�C���^�t�F�[�X
                reserved         �����̂��߂̗\�� (���0���w��)


  Returns:      �}�E���g�����ɐ��������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_MountArchive(FSArchive *arc, void *userdata,
                     const FSArchiveInterface *vtbl, u32 reserved)
{
    (void)reserved;
    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(arc);
    SDK_ASSERT(!FS_IsArchiveLoaded(arc));
    // �V�d�l�ŃA�[�J�C�u��������
    arc->userdata = userdata;
    arc->vtbl = vtbl;
    // �}�E���g�ʒm�C�x���g���s
    {
        FSFile  tmp[1];
        FS_InitFile(tmp);
        tmp->arc = arc;
        (void)FSi_InvokeCommand(tmp, FS_COMMAND_MOUNT);
    }
    arc->flag |= FS_ARCHIVE_FLAG_LOADED;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnmountArchive

  Description:  �A�[�J�C�u���A���}�E���g����B

  Arguments:    arc              �A���}�E���g����A�[�J�C�u

  Returns:      �A���}�E���g�����ɐ��������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_UnmountArchive(FSArchive *arc)
{
    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(arc);

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        // ���[�h����Ă��Ȃ��Ȃ疳��
        if (FS_IsArchiveLoaded(arc))
        {
            // �S�Ă̑ҋ@���R�}���h���L�����Z��
            {
                BOOL    bak_state = FS_SuspendArchive(arc);
                FSFile *file = arc->list;
                arc->flag |= FS_ARCHIVE_FLAG_UNLOADING;
                while (file)
                {
                    FSFile *next = file->next;
                    FSi_EndCommand(file, FS_RESULT_CANCELED);
                    file = next;
                }
                arc->list = NULL;
                if (bak_state)
                {
                    (void)FS_ResumeArchive(arc);
                }
            }
            // �A���}�E���g�ʒm�C�x���g���s
            {
                FSFile  tmp[1];
                FS_InitFile(tmp);
                tmp->arc = arc;
                (void)FSi_InvokeCommand(tmp, FS_COMMAND_UNMOUNT);
            }
            arc->flag &= ~(FS_ARCHIVE_FLAG_CANCELING |
                           FS_ARCHIVE_FLAG_LOADED | FS_ARCHIVE_FLAG_UNLOADING);
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
    return TRUE;

}

/*---------------------------------------------------------------------------*
  Name:         FS_SuspendArchive

  Description:  �A�[�J�C�u�̏����@�\���̂��~����B
                ���ݎ��s���̏���������΂��̊�����ҋ@�B

  Arguments:    p_arc            ��~����A�[�J�C�u

  Returns:      �Ăяo���ȑO�ɃT�X�y���h��ԂłȂ���� TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_SuspendArchive(FSArchive *p_arc)
{
    BOOL    retval = FALSE;

    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(p_arc);

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        retval = !FS_IsArchiveSuspended(p_arc);
        if (retval)
        {
            if ((p_arc->flag & FS_ARCHIVE_FLAG_RUNNING) == 0)
            {
                p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
            }
            else
            {
                p_arc->flag |= FS_ARCHIVE_FLAG_SUSPENDING;
                FSi_WaitConditionOff(&p_arc->flag, FS_ARCHIVE_FLAG_SUSPENDING, &p_arc->queue);
            }
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ResumeArchive

  Description:  ��~���Ă����A�[�J�C�u�̏������ĊJ

  Arguments:    arc              �ĊJ����A�[�J�C�u

  Returns:      �Ăяo���ȑO�ɃT�X�y���h��ԂłȂ���� TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_ResumeArchive(FSArchive *arc)
{
    BOOL    retval;
    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(arc);
    {
        OSIntrMode bak_irq = OS_DisableInterrupts();
        retval = !FS_IsArchiveSuspended(arc);
        if (!retval)
        {
            arc->flag &= ~FS_ARCHIVE_FLAG_SUSPEND;
        }
        (void)OS_RestoreInterrupts(bak_irq);
    }
    {
        FSFile *file = NULL;
        file = FSi_NextCommand(arc, TRUE);
        if (file)
        {
            FSi_ExecuteAsyncCommand(file);
        }
    }
    return retval;
}


/*---------------------------------------------------------------------------*
  Name:         FS_NotifyArchiveAsyncEnd

  Description:  �񓯊��Ŏ��s���Ă����A�[�J�C�u�����̊�����ʒm���邽�߂�
                �A�[�J�C�u����������Ăяo���B

  Arguments:    arc              ������ʒm����A�[�J�C�u
                ret              ��������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_NotifyArchiveAsyncEnd(FSArchive *arc, FSResult ret)
{
    FSFile     *file = arc->list;
    if ((file->stat & FS_FILE_STATUS_BLOCKING) != 0)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        file->stat |= FS_FILE_STATUS_ASYNC_DONE;
        file->error = ret;
        OS_WakeupThread(file->queue);
        (void)OS_RestoreInterrupts(bak_psr);
    }
    else
    {
        FSi_EndCommand(file, ret);
        file = FSi_NextCommand(arc, TRUE);
        if (file)
        {
            FSi_ExecuteAsyncCommand(file);
        }
    }
}


/*---------------------------------------------------------------------------*
  Name:         FS_WaitAsync

  Description:  wait for end of asynchronous function and return result.

  Arguments:    file to wait

  Returns:      if succeeded, True.
 *---------------------------------------------------------------------------*/
BOOL FS_WaitAsync(FSFile *file)
{
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

    {
        BOOL    is_owner = FALSE;
        OSIntrMode bak_psr = OS_DisableInterrupts();
        if (FS_IsBusy(file))
        {
            // �������̔񓯊����[�h�Ȃ炱���ŏ����������󂯂�
            is_owner = !(file->stat & (FS_FILE_STATUS_BLOCKING | FS_FILE_STATUS_OPERATING));
            if (is_owner)
            {
                file->stat |= FS_FILE_STATUS_BLOCKING;
            }
        }
        (void)OS_RestoreInterrupts(bak_psr);
        if (is_owner)
        {
            FSi_ExecuteSyncCommand(file);
        }
        else
        {
            FSi_WaitConditionOff(&file->stat, FS_FILE_STATUS_BUSY, file->queue);
        }
    }

    return FS_IsSucceeded(file);
}


#endif /* FS_IMPLEMENT */
