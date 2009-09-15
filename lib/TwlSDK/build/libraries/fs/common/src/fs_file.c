/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     fs_file.c

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


#include <nitro/types.h>
#include <nitro/misc.h>
#include <nitro/mi.h>
#include <nitro/os.h>
#include <nitro/pxi.h>
#include <nitro/std/string.h>
#include <nitro/std/unicode.h>
#include <nitro/math/math.h>

#include <nitro/fs.h>

#include "../include/rom.h"
#include "../include/util.h"
#include "../include/command.h"


#define FS_DEBUG_TRACE(...) (void)0
//#define FS_DEBUG_TRACE  OS_TPrintf


/*---------------------------------------------------------------------------*/
/* functions */

#if defined(FS_IMPLEMENT)

/*---------------------------------------------------------------------------*
  Name:         FSi_IsValidTransferRegion

  Description:  Read/Write����Ɏg�p����o�b�t�@���댯�Ȕ͈͂łȂ����ȈՔ���B

  Arguments:    buffer : �]���ΏۂƂȂ�o�b�t�@�B
                length : �]���T�C�Y�B

  Returns:      pos��1�����߂����Q�ƈʒu��-1�B
 *---------------------------------------------------------------------------*/
static BOOL FSi_IsValidTransferRegion(const void *buffer, s32 length)
{
    BOOL    retval = FALSE;
    if (buffer == NULL)
    {
        OS_TWarning("specified transfer buffer is NULL.\n");
    }
    else if (((u32)buffer >= HW_IOREG) && ((u32)buffer < HW_IOREG_END))
    {
        OS_TWarning("specified transfer buffer is in I/O register %08X. (seems to be dangerous)\n", buffer);
    }
    else if (length < 0)
    {
        OS_TWarning("specified transfer size is minus. (%d)\n", length);
    }
    else
    {
#if !defined(SDK_TWL)
        s32     mainmem_size = HW_MAIN_MEM_EX_SIZE;
#else
        s32     mainmem_size = OS_IsRunOnTwl() ? HW_TWL_MAIN_MEM_EX_SIZE : HW_MAIN_MEM_EX_SIZE;
#endif
        if (length > mainmem_size)
        {
            OS_TWarning("specified transfer size is over mainmemory-size. (%d)\n", length);
        }
        else
        {
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementSjisPosition

  Description:  Shift_JIS������̎Q�ƈʒu��1�������߂��B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos��1�����߂����Q�ƈʒu��-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementSjisPosition(const char *str, int pos)
{
    // �܂�1�o�C�g�������͊m���ɖ߂�B
    int     prev = --pos;
    // Shift_JIS�̕�����؂�ƂȂ�̂̓V���O���o�C�g���㑱�o�C�g�ł���
    // ��s�o�C�g�ƌ㑱�o�C�g���}�b�s���O���ꕔ���L���Ă��邽�߁A
    // ��������s�o�C�g�Ɍ����邤���͕�����ʂ��m�肵�Ȃ��̂ł���ɖ߂�B
    for (; (prev > 0) && STD_IsSjisLeadByte(str[prev - 1]); --prev)
    {
    }
    // "������b"�̂悤�ɂ����܂��Ȍ㑱�o�C�g�����S�p����������ł�����
    // 2�̔{�������]���ɖ߂��Ă��邽�߂���𖳎�����B(2�̏�]�����)
    return pos - ((pos - prev) & 1);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IncrementSjisPositionToSlash

  Description:  Shift_JIS������̎Q�ƈʒu��
                �f�B���N�g����؂蕶�����I�[�܂Ői�߂�B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos�ȍ~�Ɍ����ŏ��̃f�B���N�g����؂肩�I�[�̈ʒu�B
 *---------------------------------------------------------------------------*/
int FSi_IncrementSjisPositionToSlash(const char *str, int pos)
{
    while (str[pos] && !FSi_IsSlash((u8)str[pos]))
    {
        pos = FSi_IncrementSjisPosition(str, pos);
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementSjisPositionToSlash

  Description:  Shift_JIS������̎Q�ƈʒu��
                �f�B���N�g����؂蕶�����擪�܂Ŗ߂��B

  Arguments:    str              Shift_JIS������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos�����Ɍ����ŏ��̃f�B���N�g����؂肩-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementSjisPositionToSlash(const char *str, int pos)
{
    for (;;)
    {
        pos = FSi_DecrementSjisPosition(str, pos);
        if ((pos < 0) || FSi_IsSlash((u8)str[pos]))
        {
            break;
        }
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_TrimSjisTrailingSlash

  Description:  Shift_JIS������̏I�[���f�B���N�g����؂蕶���ł���Ώ����B

  Arguments:    str              Shift_JIS������B

  Returns:      �����񒷁B
 *---------------------------------------------------------------------------*/
int FSi_TrimSjisTrailingSlash(char *str)
{
    int     length = STD_GetStringLength(str);
    int     lastpos = FSi_DecrementSjisPosition(str, length);
    if ((lastpos >= 0) && FSi_IsSlash((u8)str[lastpos]))
    {
        length = lastpos;
        str[length] = '\0';
    }
    return length;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementUnicodePosition

  Description:  Unicode������̎Q�ƈʒu��1�������߂��B

  Arguments:    str              Unicode������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos��1�����߂����Q�ƈʒu��-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementUnicodePosition(const u16 *str, int pos)
{
    // �܂�1�����������͊m���ɖ߂�B
    int     prev = --pos;
    // �L���ȃT���Q�[�g�y�A�������ꍇ�͂���1�����߂�B
    if ((pos > 0) &&
        ((str[pos - 1] >= 0xD800) && (str[pos - 1] <= 0xDC00)) &&
        ((str[pos - 0] >= 0xDC00) && (str[pos - 0] <= 0xE000)))
    {
        --pos;
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DecrementUnicodePositionToSlash

  Description:  Unicode������̎Q�ƈʒu��
                �f�B���N�g����؂蕶�����擪�܂Ŗ߂��B

  Arguments:    str              Unicode������̐擪���w���|�C���^�B
                pos              ���݂̕�����Q�ƈʒu�B(�o�C�g�P��)

  Returns:      pos�����Ɍ����ŏ��̃f�B���N�g����؂肩-1�B
 *---------------------------------------------------------------------------*/
int FSi_DecrementUnicodePositionToSlash(const u16 *str, int pos)
{
    for (;;)
    {
        pos = FSi_DecrementUnicodePosition(str, pos);
        if ((pos < 0) || FSi_IsUnicodeSlash(str[pos]))
        {
            break;
        }
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         FS_InitFile

  Description:  FSFile�\���̂�������

  Arguments:    file        FSFile�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_InitFile(FSFile *file)
{
    SDK_NULL_ASSERT(file);
    {
        file->arc = NULL;
        file->userdata = NULL;
        file->next = NULL;
        OS_InitThreadQueue(file->queue);
        file->stat = 0;
        file->stat |= (FS_COMMAND_INVALID << FS_FILE_STATUS_CMD_SHIFT);
        file->argument = NULL;
        file->error = FS_RESULT_SUCCESS;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_CancelFile

  Description:  �񓯊��R�}���h�̃L�����Z����v������

  Arguments:    file        �t�@�C���n���h��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_CancelFile(FSFile *file)
{
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsAvailable());
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        if (FS_IsBusy(file))
        {
            file->stat |= FS_FILE_STATUS_CANCEL;
            file->arc->flag |= FS_ARCHIVE_FLAG_CANCELING;
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFile

  Description:  �t�@�C���𐶐�����

  Arguments:    path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      �t�@�C��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CreateFile(const char *path, u32 permit)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile                  file[1];
            FSArgumentForCreateFile arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->permit = permit;
            retval = FSi_SendCommand(file, FS_COMMAND_CREATEFILE, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteFile

  Description:  �t�@�C�����폜����

  Arguments:    path        �p�X��

  Returns:      �t�@�C��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_DeleteFile(const char *path)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile                  file[1];
            FSArgumentForDeleteFile arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            retval = FSi_SendCommand(file, FS_COMMAND_DELETEFILE, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RenameFile

  Description:  �t�@�C������ύX����

  Arguments:    src         �ϊ����̃t�@�C����
                dst         �ϊ���̃t�@�C����

  Returns:      �t�@�C����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RenameFile(const char *src, const char *dst)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s->%s)\n", __FUNCTION__, src, dst);
    SDK_NULL_ASSERT(src);
    SDK_NULL_ASSERT(dst);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath_src[FS_ARCHIVE_FULLPATH_MAX + 1];
        char        relpath_dst[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid_src = 0;
        u32         baseid_dst = 0;
        FSArchive  *arc_src = FS_NormalizePath(src, &baseid_src, relpath_src);
        FSArchive  *arc_dst = FS_NormalizePath(dst, &baseid_dst, relpath_dst);
        if (arc_src != arc_dst)
        {
            OS_TWarning("cannot rename between defferent archives.\n");
        }
        else
        {
            FSFile                  file[1];
            FSArgumentForRenameFile arg[1];
            FS_InitFile(file);
            file->arc = arc_src;
            file->argument = arg;
            arg->baseid_src = baseid_src;
            arg->relpath_src = relpath_src;
            arg->baseid_dst = baseid_dst;
            arg->relpath_dst = relpath_dst;
            retval = FSi_SendCommand(file, FS_COMMAND_RENAMEFILE, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathInfo

  Description:  �t�@�C���܂��̓f�B���N�g���̏����擾����

  Arguments:    path        �p�X��
                info        ���̊i�[��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
BOOL FS_GetPathInfo(const char *path, FSPathInfo *info)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(path);
    SDK_NULL_ASSERT(info);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile                      file[1];
            FSArgumentForGetPathInfo    arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->info = info;
            retval = FSi_SendCommand(file, FS_COMMAND_GETPATHINFO, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetPathInfo

  Description:  �t�@�C������ݒ肷��

  Arguments:    path        �p�X��
                info        ���̊i�[��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
BOOL FS_SetPathInfo(const char *path, const FSPathInfo *info)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(path);
    SDK_NULL_ASSERT(info);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile                      file[1];
            FSArgumentForSetPathInfo    arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->info = (FSPathInfo*)info; //info->attributes��FATFS_PROPERTY_CTRL_MASK�𗎂Ƃ�����
            retval = FSi_SendCommand(file, FS_COMMAND_SETPATHINFO, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateDirectory

  Description:  �f�B���N�g���𐶐�����

  Arguments:    path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      �f�B���N�g��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CreateDirectory(const char *path, u32 permit)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile                          file[1];
            FSArgumentForCreateDirectory    arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->permit = permit;
            retval = FSi_SendCommand(file, FS_COMMAND_CREATEDIRECTORY, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteDirectory

  Description:  �f�B���N�g�����폜����

  Arguments:    path        �p�X��

  Returns:      �f�B���N�g��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_DeleteDirectory(const char *path)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile                          file[1];
            FSArgumentForDeleteDirectory    arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            retval = FSi_SendCommand(file, FS_COMMAND_DELETEDIRECTORY, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RenameDirectory

  Description:  �f�B���N�g������ύX����

  Arguments:    src         �ϊ����̃f�B���N�g����
                dst         �ϊ���̃f�B���N�g����

  Returns:      �f�B���N�g����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RenameDirectory(const char *src, const char *dst)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s->%s)\n", __FUNCTION__, src, dst);
    SDK_NULL_ASSERT(src);
    SDK_NULL_ASSERT(dst);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath_src[FS_ARCHIVE_FULLPATH_MAX + 1];
        char        relpath_dst[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid_src = 0;
        u32         baseid_dst = 0;
        FSArchive  *arc_src = FS_NormalizePath(src, &baseid_src, relpath_src);
        FSArchive  *arc_dst = FS_NormalizePath(dst, &baseid_dst, relpath_dst);
        if (arc_src != arc_dst)
        {
            OS_TWarning("cannot rename between defferent archives.\n");
        }
        else
        {
            FSFile                          file[1];
            FSArgumentForRenameDirectory    arg[1];
            FS_InitFile(file);
            file->arc = arc_src;
            file->argument = arg;
            arg->baseid_src = baseid_src;
            arg->relpath_src = relpath_src;
            arg->baseid_dst = baseid_dst;
            arg->relpath_dst = relpath_dst;
            retval = FSi_SendCommand(file, FS_COMMAND_RENAMEDIRECTORY, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetFullPath

  Description:  �w�肳�ꂽ�p�X���t���p�X�֕ϊ�����B

  Arguments:    dst         ����ꂽ�t���p�X���i�[����o�b�t�@�B
                            FS_ARCHIVE_FULLPATH_MAX+1�o�C�g�ȏ�łȂ���΂Ȃ�Ȃ��B
                path        �t�@�C���܂��̓f�B���N�g���̃p�X���B

  Returns:      �t���p�X������Ɏ擾�ł����TRUE
 *---------------------------------------------------------------------------*/
static BOOL FSi_GetFullPath(char *dst, const char *path)
{
    FSArchive  *arc = FS_NormalizePath(path, NULL, dst);
    if (arc)
    {
        const char *arcname = FS_GetArchiveName(arc);
        int     m = STD_GetStringLength(arcname);
        int     n = STD_GetStringLength(dst);
        (void)STD_MoveMemory(&dst[m + 2], &dst[0], (u32)n + 1);
        (void)STD_MoveMemory(&dst[0], arcname, (u32)m);
        dst[m + 0] = ':';
        dst[m + 1] = '/';
    }
    return (arc != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ComplementDirectory

  Description:  �w�肳�ꂽ�p�X�Ɏ���S�Ă̐e�f�B���N�g���̑��݂��m�F���A
                ���݂��Ȃ��K�w�͎����I�ɐ������ĕ⊮����B

  Arguments:    path        �t�@�C���܂��̓f�B���N�g���̃p�X���B
                            ������1�K�w��܂ł̃f�B���N�g����⊮����B
                autogen     �����������ꂽ�ŏ�ʃf�B���N�g�����L�^����o�b�t�@�B
                            FS_ARCHIVE_FULLPATH_MAX+1�o�C�g�ȏ�łȂ���΂Ȃ�Ȃ��B
                            ���ʂ̐��ۂɂ�����炸autogen�ɂ͌��ʂ��Ԃ���A
                            �󕶎���ł���΂��łɑS�đ��݂��Ă������Ƃ������B

  Returns:      �f�B���N�g��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
static BOOL FSi_ComplementDirectory(const char *path, char *autogen)
{
    BOOL    retval = FALSE;
    int     root = 0;
    // ��������t���p�X���ɐ��K���B(�X�^�b�N�ߖ�̂���autogen�𗬗p)
    char   *tmppath = autogen;
    if (FSi_GetFullPath(tmppath, path))
    {
        int     length = STD_GetStringLength(tmppath);
        if (length > 0)
        {
            int     pos = 0;
            FS_DEBUG_TRACE("  trying to complete \"%s\"\n", tmppath);
            // �I�[��"/"�͏����B
            length = FSi_TrimSjisTrailingSlash(tmppath);
            // �ŉ��w�̃G���g���͖����B
            length = FSi_DecrementSjisPositionToSlash(tmppath, length);
            // ���݂���Ő[�̊K�w��T���B
            for (pos = length; pos >= 0;)
            {
                FSPathInfo  info[1];
                BOOL        exists;
                tmppath[pos] = '\0';
                exists = FS_GetPathInfo(tmppath, info);
                FS_DEBUG_TRACE("    - \"%s\" is%s existent (result:%d)\n", tmppath, exists ? "" : " not",
                               FS_GetArchiveResultCode(tmppath));
                tmppath[pos] = '/';
                // �G���g�������݂��Ȃ���΂���ɏ�ʂ̊K�w�ցB
                if (!exists)
                {
                    pos = FSi_DecrementSjisPositionToSlash(tmppath, pos);
                }
                // �������݂��Ă���΂ЂƂ܂������ŒT���I���B
                else
                {
                    // �����̃t�@�C�������݂��Ă���ꍇ�͊m���Ɏ��s�B
                    if ((info->attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0)
                    {
                        pos = -1;
                    }
                    // �����̃f�B���N�g�������݂��Ă���΂��̉��w��������������B
                    else
                    {
                        ++pos;
                    }
                    break;
                }
            }
            // ��_�����߂�ꂽ�Ȃ�΂���ȍ~�̊K�w���J��Ԃ������B
            if (pos >= 0)
            {
                for (;;)
                {
                    // �I�[�ɒB�����琬���B
                    if (pos >= length)
                    {
                        retval = TRUE;
                        break;
                    }
                    else
                    {
                        pos = FSi_IncrementSjisPositionToSlash(tmppath, pos);
                        tmppath[pos] = '\0';
                        if (!FS_CreateDirectory(tmppath, FS_PERMIT_R | FS_PERMIT_W))
                        {
                            break;
                        }
                        else
                        {
                            // �ŏ�ʂ̊K�w�͋L�����Ă����B
                            if (root == 0)
                            {
                                FS_DEBUG_TRACE("    - we have created \"%s\" as root\n", tmppath);
                                root = pos;
                            }
                            tmppath[pos++] = '/';
                        }
                    }
                }
            }
        }
    }
    // �����������ꂽ�ŏ�ʃf�B���N�g�����L�^�B
    autogen[root] = '\0';
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăt�@�C���𐶐�����

  Arguments:    path        �p�X��
                permit      �A�N�Z�X���[�h

  Returns:      �t�@�C��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CreateFileAuto(const char *path, u32 permit)
{
    BOOL    result = FALSE;
    char    autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    if (FSi_ComplementDirectory(path, autogen))
    {
        result = FS_CreateFile(path, permit);
        if (!result)
        {
            (void)FS_DeleteDirectoryAuto(autogen);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteFileAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăt�@�C�����폜����

  Arguments:    path        �p�X��

  Returns:      �t�@�C��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_DeleteFileAuto(const char *path)
{
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    // �����̓��ꐫ�̂��߂ɑ��݂��Ă��邪���ۂɂ͕⊮�������s�v�B
    return FS_DeleteFile(path);
}

/*---------------------------------------------------------------------------*
  Name:         FS_RenameFileAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăt�@�C������ύX����

  Arguments:    src         �ϊ����̃t�@�C����
                dst         �ϊ���̃t�@�C����

  Returns:      �t�@�C����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RenameFileAuto(const char *src, const char *dst)
{
    BOOL    result = FALSE;
    char    autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
    FS_DEBUG_TRACE( "%s(%s->%s)\n", __FUNCTION__);
    if (FSi_ComplementDirectory(dst, autogen))
    {
        result = FS_RenameFile(src, dst);
        if (!result)
        {
            (void)FS_DeleteDirectoryAuto(autogen);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateDirectoryAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g�����܂߂ăf�B���N�g���𐶐�����

  Arguments:    path        ��������f�B���N�g����
                permit      �A�N�Z�X���[�h

  Returns:      �f�B���N�g��������ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CreateDirectoryAuto(const char *path, u32 permit)
{
    BOOL    result = FALSE;
    char    autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    if (FSi_ComplementDirectory(path, autogen))
    {
        result = FS_CreateDirectory(path, permit);
        if (!result)
        {
            (void)FS_DeleteDirectoryAuto(autogen);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FS_DeleteDirectoryAuto

  Description:  �f�B���N�g�����ċA�I�ɍ폜����

  Arguments:    path        �p�X��

  Returns:      �f�B���N�g��������ɍ폜������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_DeleteDirectoryAuto(const char *path)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s(%s)\n", __FUNCTION__, path);
    if (path && *path)
    {
        char    tmppath[FS_ARCHIVE_FULLPATH_MAX + 1];
        if (FSi_GetFullPath(tmppath, path))
        {
            int     pos;
            BOOL    mayBeEmpty;
            int     length = FSi_TrimSjisTrailingSlash(tmppath);
            FS_DEBUG_TRACE("  trying to force-delete \"%s\"\n", tmppath);
            mayBeEmpty = TRUE;
            for (pos = 0; pos >= 0;)
            {
                BOOL    failure = FALSE;
                // �܂����ڂ��̃f�B���N�g�����폜���Ă݂Đ�������Ȃ�1�K�w��ցB
                tmppath[length + pos] = '\0';
                if (mayBeEmpty && (FS_DeleteDirectory(tmppath) ||
                    (FS_GetArchiveResultCode(tmppath) == FS_RESULT_ALREADY_DONE)))
                {
                    FS_DEBUG_TRACE("  -> succeeded to delete \"%s\"\n", tmppath);
                    pos = FSi_DecrementSjisPositionToSlash(&tmppath[length], pos);
                }
                else
                {
                    // �f�B���N�g�����J����悤�Ȃ�S�G���g����񋓁B
                    FSFile  dir[1];
                    FS_InitFile(dir);
                    if (!FS_OpenDirectory(dir, tmppath, FS_FILEMODE_R))
                    {
                        FS_DEBUG_TRACE("  -> failed to delete & open \"%s\"\n", tmppath);
                        failure = TRUE;
                    }
                    else
                    {
                        FSDirectoryEntryInfo    info[1];
                        tmppath[length + pos] = '/';
                        mayBeEmpty = TRUE;
                        while (FS_ReadDirectory(dir, info))
                        {
                            (void)STD_CopyString(&tmppath[length + pos + 1], info->longname);
                            // �t�@�C�������݂���΍폜�B
                            if ((info->attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0)
                            {
                                if (!FS_DeleteFile(tmppath))
                                {
                                    FS_DEBUG_TRACE("  -> failed to delete file \"%s\"\n", tmppath);
                                    failure = TRUE;
                                    break;
                                }
                                FS_DEBUG_TRACE("  -> succeeded to delete \"%s\"\n", tmppath);
                            }
                            // "." �܂��� ".." �Ȃ疳���B
                            else if ((STD_CompareString(info->longname, ".") == 0) ||
                                     (STD_CompareString(info->longname, "..") == 0))
                            {
                            }
                            // ��łȂ��f�B���N�g�������݂���΂���ɉ��w�ֈړ��B
                            else if (!FS_DeleteDirectory(tmppath))
                            {
                                pos += 1 + STD_GetStringLength(info->longname);
                                mayBeEmpty = FALSE;
                                break;
                            }
                        }
                        (void)FS_CloseDirectory(dir);
                    }
                }
                // �������ׂ����삳�����s�����ꍇ�͏����𒆎~�B(ALREADY_DONE�Ȃ�)
                if (failure)
                {
                    break;
                }
            }
            retval = (pos < 0);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RenameDirectoryAuto

  Description:  �K�v�Ȓ��ԃf�B���N�g���������������ăf�B���N�g������ύX����

  Arguments:    src         �ϊ����̃f�B���N�g����
                dst         �ϊ���̃f�B���N�g����

  Returns:      �f�B���N�g����������ɕύX������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RenameDirectoryAuto(const char *src, const char *dst)
{
    BOOL    result = FALSE;
    char    autogen[FS_ARCHIVE_FULLPATH_MAX + 1];
    FS_DEBUG_TRACE( "%s(%s->%s)\n", __FUNCTION__, src, dst);
    if (FSi_ComplementDirectory(dst, autogen))
    {
        result = FS_RenameDirectory(src, dst);
        if (!result)
        {
            (void)FS_DeleteDirectoryAuto(autogen);
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveResource

  Description:  �w�肵���A�[�J�C�u�̃��\�[�X�����擾����

  Arguments:    path        �A�[�J�C�u�����ł���p�X��
                resource    �擾�������\�[�X���̊i�[��

  Returns:      ���\�[�X��񂪐���Ɏ擾�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_GetArchiveResource(const char *path, FSArchiveResource *resource)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArchive  *arc = FS_NormalizePath(path, NULL, NULL);
        if (arc)
        {
            FSFile                          file[1];
            FSArgumentForGetArchiveResource arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->resource = resource;
            retval = FSi_SendCommand(file, FS_COMMAND_GETARCHIVERESOURCE, TRUE);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetSpaceToCreateDirectoryEntries

  Description:  �t�@�C���̐����Ɠ����ɐ��������f�B���N�g���G���g���̗e�ʂ����ς���
                (�p�X���ɑ��݂���f�B���N�g�����V�K�ɍ쐬���邱�Ƃ�z�肷��)

  Arguments:    path                �����������t�@�C���̃p�X���B
                bytesPerCluster     �t�@�C���V�X�e����̃N���X�^������̃o�C�g���B

  Returns:      �e��
 *---------------------------------------------------------------------------*/
u32 FSi_GetSpaceToCreateDirectoryEntries(const char *path, u32 bytesPerCluster)
{
    static const u32    bytesPerEntry = 32UL;
    static const u32    longnamePerEntry = 13UL;
    // �t���p�X�Ȃ�X�L�[�����������e�G���g�����ʂɔ���B
    const char         *root = STD_SearchString(path, ":");
    const char         *current = (root != NULL) ? (root + 1) : path;
    u32                 totalBytes = 0;
    u32                 restBytesInCluster = 0;
    current += (*current == '/');
    while (*current)
    {
        BOOL    isShortName = FALSE;
        u32     entries = 0;
        // �G���g�����̒������Z�o�B
        u32     len = (u32)FSi_IncrementSjisPositionToSlash(current, 0);
        // 8.3�`���ŕ\���ł���G���g�������ǂ�������B
#if 0
        // (TWL�ō̗p���Ă���FAT�h���C�o�͏�ɒ����t�@�C������ۑ�����̂�
        //  ���ۂɂ͂����܂Ō����Ȕ��菈���͕K�v�Ȃ�����)
        {
            static const char  *alnum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            static const char  *special = "!#$%&'()*+-<>?@^_`{}~";
            if ((len <= 8 + 1 + 3) && STD_SearchChar(alnum, current[0]))
            {
                u32     namelen = 0;
                u32     extlen = 0;
                u32     scanned = 0;
                for (; namelen < len; ++namelen)
                {
                    char    c = current[scanned + namelen];
                    if (!STD_SearchChar(alnum, c) && !STD_SearchChar(special, c))
                    {
                        break;
                    }
                }
                scanned += namelen;
                if ((scanned < len) && (current[scanned] == '.'))
                {
                    ++scanned;
                    for (; scanned + extlen < len; ++extlen)
                    {
                        char    c = current[scanned + extlen];
                        if (!STD_SearchChar(alnum, c) && !STD_SearchChar(special, c))
                        {
                            break;
                        }
                    }
                    scanned += extlen;
                }
                if ((scanned == len) && (namelen <= 8) && (extlen <= 3))
                {
                    isShortName = TRUE;
                }
            }
        }
#endif
        // 8.3�`���łȂ���Β����t�@�C�����̂��߂ɒǉ��G���g�����K�v�B
        if (!isShortName)
        {
            entries += ((len + longnamePerEntry - 1UL) / longnamePerEntry);
        }
        // ������ɂ���1�G���g�����͏�ɏ�����Ƃ݂Ȃ��B
        entries += 1;
        current += len;
        // ���łɎ��������������f�B���N�g���̃N���X�^�]���ő���Ȃ��Ȃ�
        // ���ߕ��̃G���g�����N���X�^�P�ʂŏ����B
        {
            int     over = (int)(entries * bytesPerEntry - restBytesInCluster);
            if (over > 0)
            {
                totalBytes += MATH_ROUNDUP(over, bytesPerCluster);
            }
        }
        // ����ɉ��w������Ȃ�f�B���N�g���Ƃ���1�N���X�^����A
        // ���̂��� "." ".." ��2�G���g�������������T�C�Y���]���ƂȂ�B
        if (*current != '\0')
        {
            current += 1;
            totalBytes += bytesPerCluster;
            restBytesInCluster = bytesPerCluster - (2 * bytesPerEntry);
        }
    }
    return totalBytes;
}

/*---------------------------------------------------------------------------*
  Name:         FS_HasEnoughSpaceToCreateFile

  Description:  �w�肵���p�X���ƃT�C�Y�����t�@�C�������ݐ����\�����肷��

  Arguments:    resource    ���O��FS_GetArchiveResource�֐��Ŏ擾�����A�[�J�C�u���B
                            �֐�����������ƁA�t�@�C��������镪�����e�T�C�Y����������B
                path        �����������t�@�C���̃p�X���B
                size        �����������t�@�C���̃T�C�Y�B

  Returns:      �w�肵���p�X���ƃT�C�Y�����t�@�C���������\�ł����TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_HasEnoughSpaceToCreateFile(FSArchiveResource *resource, const char *path, u32 size)
{
    BOOL    retval = FALSE;
    u32 bytesPerCluster = resource->bytesPerSector * resource->sectorsPerCluster;
    if (bytesPerCluster != 0)
    {
        u32     needbytes = (FSi_GetSpaceToCreateDirectoryEntries(path, bytesPerCluster) +
                             MATH_ROUNDUP(size, bytesPerCluster));
        u32     needclusters = needbytes / bytesPerCluster;
        if (needclusters <= resource->availableClusters)
        {
            resource->availableClusters -= needclusters;
            resource->availableSize -= needbytes;
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveReady

  Description:  �w�肵���A�[�J�C�u�����ݎg�p�\�����肷��

  Arguments:    path        "�A�[�J�C�u��:"�Ŏn�܂��΃p�X

  Returns:      �w�肵���A�[�J�C�u�����ݎg�p�\�ł����TRUE�B
                �X���b�g�ɑ}������Ă��Ȃ�SD�������J�[�h�A�[�J�C�u��
                �܂��C���|�[�g����Ă��Ȃ��Z�[�u�f�[�^�A�[�J�C�u�Ȃ�FALSE�B
 *---------------------------------------------------------------------------*/
BOOL FS_IsArchiveReady(const char *path)
{
    FSArchiveResource   resource[1];
    return FS_GetArchiveResource(path, resource);
}

/*---------------------------------------------------------------------------*
  Name:         FS_FlushFile

  Description:  �t�@�C���̕ύX���e���f�o�C�X�֔��f����

  Arguments:    file        �t�@�C���n���h��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
FSResult FS_FlushFile(FSFile *file)
{
    FSResult    retval = FS_RESULT_ERROR;
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsFile(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        (void)FSi_SendCommand(file, FS_COMMAND_FLUSHFILE, TRUE);
        retval = FS_GetResultCode(file);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetFileLength

  Description:  �t�@�C���T�C�Y��ύX����

  Arguments:    file        �t�@�C���n���h��
                length      �ύX��̃T�C�Y

  Returns:      ��������
 *---------------------------------------------------------------------------*/
FSResult FS_SetFileLength(FSFile *file, u32 length)
{
    FSResult    retval = FS_RESULT_ERROR;
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsFile(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForSetFileLength  arg[1];
        file->argument = arg;
        arg->length = length;
        (void)FSi_SendCommand(file, FS_COMMAND_SETFILELENGTH, TRUE);
        retval = FS_GetResultCode(file);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathName

  Description:  �w�肵���n���h���̃p�X�����擾����

  Arguments:    file        �t�@�C���܂��̓f�B���N�g��
                buffer      �p�X�i�[��
                length      �o�b�t�@�T�C�Y

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_GetPathName(FSFile *file, char *buffer, u32 length)
{
    BOOL    retval = FALSE;
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file) || FS_IsDir(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForGetPath    arg[1];
        file->argument = arg;
        arg->is_directory = FS_IsDir(file);
        arg->buffer = buffer;
        arg->length = length;
        retval = FSi_SendCommand(file, FS_COMMAND_GETPATH, TRUE);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPathLength

  Description:  �w�肵���t�@�C���܂��̓f�B���N�g���̃t���p�X���̒������擾

  Arguments:    file        �t�@�C���܂��̓f�B���N�g���n���h��

  Returns:      ��������ΏI�[��'\0'���܂߂��p�X���̒����A���s�����-1
 *---------------------------------------------------------------------------*/
s32 FS_GetPathLength(FSFile *file)
{
    s32     retval = -1;
    if (FS_GetPathName(file, NULL, 0))
    {
        retval = file->arg.getpath.total_len;
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ConvertPathToFileID

  Description:  �p�X������t�@�C��ID���擾����

  Arguments:    p_fileid    FSFileID�̊i�[��
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_ConvertPathToFileID(FSFileID *p_fileid, const char *path)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(p_fileid);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSFile      file[1];
            FSArgumentForFindPath   arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->target_is_directory = FALSE;
            if (FSi_SendCommand(file, FS_COMMAND_FINDPATH, TRUE))
            {
                p_fileid->arc = arc;
                p_fileid->file_id = arg->target_id;
                retval = TRUE;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileDirect

  Description:  �A�[�J�C�u���̗̈�𒼐ڎw�肵�ăt�@�C�����J��

  Arguments:    file         �n���h������ێ�����FSFile
                arc          �}�b�v���ɂȂ�A�[�J�C�u
                image_top    �t�@�C���C���[�W�擪�̃I�t�Z�b�g
                image_bottom �t�@�C���C���[�W�I�[�̃I�t�Z�b�g
                id           �C�ӂɎw�肷��t�@�C��ID

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileDirect(FSFile *file, FSArchive *arc,
                       u32 image_top, u32 image_bottom, u32 id)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(file);
    SDK_NULL_ASSERT(arc);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(!FS_IsFile(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForOpenFileDirect arg[1];
        file->arc = arc;
        file->argument = arg;
        arg->id = id;
        arg->top = image_top;
        arg->bottom = image_bottom;
        arg->mode = 0;
        retval = FSi_SendCommand(file, FS_COMMAND_OPENFILEDIRECT, TRUE);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileFast

  Description:  ID���w�肵�ăt�@�C�����J��

  Arguments:    file         �n���h������ێ�����FSFile
                id           �J���ׂ��t�@�C�����w��FSFileID

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileFast(FSFile *file, FSFileID id)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(!FS_IsFile(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    if (id.arc)
    {
        FSArgumentForOpenFileFast   arg[1];
        file->arc = id.arc;
        file->argument = arg;
        arg->id = id.file_id;
        arg->mode = 0;
        retval = FSi_SendCommand(file, FS_COMMAND_OPENFILEFAST, TRUE);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileEx

  Description:  �p�X�����w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileEx(FSFile *file, const char *path, u32 mode)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

    // FS_FILEMODE_L �Ɋւ���_���`�F�b�N�B
    // (creation���[�h�ŊJ���T�C�Y����������Ӗ�������)
    if (((mode & FS_FILEMODE_L) != 0) &&
        ((mode & FS_FILEMODE_RW) == FS_FILEMODE_W))
    {
        OS_TWarning("\"FS_FILEMODE_WL\" seems useless.\n"
                    "(this means creating empty file and prohibiting any modifications)");
    }
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSArgumentForOpenFile   arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->mode = mode;
            if (FSi_SendCommand(file, FS_COMMAND_OPENFILE, TRUE))
            {
                retval = TRUE;
            }
            else
            {
                file->arc = NULL;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_CloseFile

  Description:  �t�@�C�������

  Arguments:    file        �t�@�C���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CloseFile(FSFile *file)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        retval = FSi_SendCommand(file, FS_COMMAND_CLOSEFILE, TRUE);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetSeekCacheSize

  Description:  �����t�V�[�N�p�̃t���L���b�V���ɕK�v�ȃo�b�t�@�T�C�Y�����߂�

  Arguments:    path

  Returns:      ��������΃T�C�Y�A���s�����0
 *---------------------------------------------------------------------------*/
u32 FS_GetSeekCacheSize(const char *path)
{
    u32         retval = 0;
    // �t�@�C�������݂���Ȃ�T�C�Y���擾����B
    FSPathInfo  info;
    if (FS_GetPathInfo(path, &info) &&
        ((info.attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0))
    {
        // �Y������A�[�J�C�u��FAT�����擾�B
        FSArchiveResource   resource;
        if (FS_GetArchiveResource(path, &resource))
        {
            // ���ۂ�FAT�x�[�X�̃A�[�J�C�u�ł���΃L���b�V���T�C�Y���Z�o�B
            u32     bytesPerCluster = resource.sectorsPerCluster * resource.bytesPerSector;
            if (bytesPerCluster != 0)
            {
                static const u32    fatBits = 32;
                retval = (u32)((info.filesize + bytesPerCluster - 1) / bytesPerCluster) * ((fatBits + 4) / 8);
				// �o�b�t�@�O����L���b�V�����C���ɐ��������邽�ߗ]����ǉ��B
                retval += (u32)(HW_CACHE_LINE_SIZE * 2);
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetSeekCache

  Description:  �����t�V�[�N�p�̃L���b�V���o�b�t�@�����蓖�Ă�

  Arguments:    file        �t�@�C���n���h��
                buf         �L���b�V���o�b�t�@
                buf_size    �L���b�V���o�b�t�@�T�C�Y

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_SetSeekCache(FSFile *file, void* buf, u32 buf_size)
{
    FSArgumentForSetSeekCache    arg[1];
    BOOL     retval = FALSE;
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file));

    file->argument = arg;
    arg->buf      = buf;
    arg->buf_size = buf_size;
    retval = FSi_SendCommand(file, FS_COMMAND_SETSEEKCACHE, TRUE);

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileLength

  Description:  �t�@�C���T�C�Y���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���T�C�Y
 *---------------------------------------------------------------------------*/
u32 FS_GetFileLength(FSFile *file)
{
    u32     retval = 0;
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file));
    // �A�[�J�C�u�v���V�[�W���̏ꍇ�͂��̏�Œ��ڎQ�Ɖ\�B
    if (!FSi_GetFileLengthIfProc(file, &retval))
    {
        FSArgumentForGetFileLength    arg[1];
        file->argument = arg;
        arg->length = 0;
        if (FSi_SendCommand(file, FS_COMMAND_GETFILELENGTH, TRUE))
        {
            retval = arg->length;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetFilePosition

  Description:  �t�@�C���|�C���^�̌��݈ʒu���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���|�C���^�̌��݈ʒu
 *---------------------------------------------------------------------------*/
u32 FS_GetFilePosition(FSFile *file)
{
    u32     retval = 0;
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file));
    // �A�[�J�C�u�v���V�[�W���̏ꍇ�͂��̏�Œ��ڎQ�Ɖ\�B
    if (!FSi_GetFilePositionIfProc(file, &retval))
    {
        FSArgumentForGetFilePosition    arg[1];
        file->argument = arg;
        arg->position = 0;
        if (FSi_SendCommand(file, FS_COMMAND_GETFILEPOSITION, TRUE))
        {
            retval = arg->position;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SeekFile

  Description:  �t�@�C���|�C���^���ړ�

  Arguments:    file        �t�@�C���n���h��
                offset      �ړ���
                origin      �ړ��N�_

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_SeekFile(FSFile *file, s32 offset, FSSeekFileMode origin)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file));
    {
        FSArgumentForSeekFile   arg[1];
        file->argument = arg;
        arg->offset = (int)offset;
        arg->from = origin;
        retval = FSi_SendCommand(file, FS_COMMAND_SEEKFILE, TRUE);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadFile

  Description:  �t�@�C������f�[�^��ǂݏo��

  Arguments:    file        �t�@�C���n���h��
                buffer      �]����o�b�t�@
                length      �ǂݏo���T�C�Y

  Returns:      ��������Ύ��ۂɓǂݏo�����T�C�Y�A���s�����-1
 *---------------------------------------------------------------------------*/
s32 FS_ReadFile(FSFile *file, void *buffer, s32 length)
{
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FSi_IsValidTransferRegion(buffer, length));
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file) && !FS_IsBusy(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForReadFile   arg[1];
        file->argument = arg;
        arg->buffer = buffer;
        arg->length = (u32)length;
        if (FSi_SendCommand(file, FS_COMMAND_READFILE, TRUE))
        {
            length = (s32)arg->length;
        }
        else
        {
            if( file->error == FS_RESULT_INVALID_PARAMETER) {
                length = -1; //�����������[�h���Ȃ������ꍇ
            }else{
                length = (s32)arg->length; //���[�h�����݂��ꍇ��-1�ȏ�̒l�������Ă���
            }
        }
    }
    return length;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadFileAsync

  Description:  �t�@�C������f�[�^��񓯊��I�ɓǂݏo��

  Arguments:    file        �t�@�C���n���h��
                buffer      �]����o�b�t�@
                length      �ǂݏo���T�C�Y

  Returns:      ��������ΒP��length�Ɠ����l�A���s�����-1
 *---------------------------------------------------------------------------*/
s32 FS_ReadFileAsync(FSFile *file, void *buffer, s32 length)
{
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FSi_IsValidTransferRegion(buffer, length));
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file) && !FS_IsBusy(file));
    // �A�[�J�C�u�v���V�[�W���̏ꍇ�͂��̏�ŃT�C�Y�␳
    {
        u32     end, pos;
        if (FSi_GetFilePositionIfProc(file, &pos) &&
            FSi_GetFileLengthIfProc(file, &end) &&
            (pos + length > end))
        {
            length = (s32)(end - pos);
        }
    }
    {
        FSArgumentForReadFile  *arg = (FSArgumentForReadFile*)file->reserved2;
        file->argument = arg;
        arg->buffer = buffer;
        arg->length = (u32)length;
        (void)FSi_SendCommand(file, FS_COMMAND_READFILE, FALSE);
    }
    return length;
}

/*---------------------------------------------------------------------------*
  Name:         FS_WriteFile

  Description:  �t�@�C���փf�[�^����������

  Arguments:    file        �t�@�C���n���h��
                buffer      �]�����o�b�t�@
                length      �������݃T�C�Y

  Returns:      ��������Ύ��ۂɏ������񂾃T�C�Y�A���s�����-1
 *---------------------------------------------------------------------------*/
s32 FS_WriteFile(FSFile *file, const void *buffer, s32 length)
{
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FSi_IsValidTransferRegion(buffer, length));
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file) && !FS_IsBusy(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForWriteFile  arg[1];
        file->argument = arg;
        arg->buffer = buffer;
        arg->length = (u32)length;
        if (FSi_SendCommand(file, FS_COMMAND_WRITEFILE, TRUE))
        {
            length = (s32)arg->length;
        }
        else
        {
            if( file->error == FS_RESULT_INVALID_PARAMETER) {
                length = -1; //�����������C�g���Ȃ������ꍇ
            }else{
                length = (s32)arg->length; //���C�g�����݂��ꍇ��-1�ȏ�̒l�������Ă���
            }
        }
    }
    return length;
}

/*---------------------------------------------------------------------------*
  Name:         FS_WriteFileAsync

  Description:  �t�@�C���փf�[�^��񓯊��I�ɏ�������

  Arguments:    file        �t�@�C���n���h��
                buffer      �]�����o�b�t�@
                length      �������݃T�C�Y

  Returns:      ��������ΒP��length�Ɠ����l�A���s�����-1
 *---------------------------------------------------------------------------*/
s32 FS_WriteFileAsync(FSFile *file, const void *buffer, s32 length)
{
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FSi_IsValidTransferRegion(buffer, length));
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsFile(file) && !FS_IsBusy(file));
    // �A�[�J�C�u�v���V�[�W���̏ꍇ�͂��̏�ŃT�C�Y�␳
    {
        u32     end, pos;
        if (FSi_GetFilePositionIfProc(file, &pos) &&
            FSi_GetFileLengthIfProc(file, &end) &&
            (pos + length > end))
        {
            length = (s32)(end - pos);
        }
    }
    {
        FSArgumentForWriteFile *arg = (FSArgumentForWriteFile*)file->reserved2;
        file->argument = arg;
        arg->buffer = buffer;
        arg->length = (u32)length;
        (void)FSi_SendCommand(file, FS_COMMAND_WRITEFILE, FALSE);
    }
    return length;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenDirectory

  Description:  �f�B���N�g���n���h�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenDirectory(FSFile *file, const char *path, u32 mode)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        char        relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FS_NormalizePath(path, &baseid, relpath);
        if (arc)
        {
            FSArgumentForOpenDirectory  arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = relpath;
            arg->mode = mode;
            if (FSi_SendCommand(file, FS_COMMAND_OPENDIRECTORY, TRUE))
            {
                retval = TRUE;
            }
            else
            {
                file->arc = NULL;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_CloseDirectory

  Description:  �f�B���N�g���n���h�������

  Arguments:    file        FSFile�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_CloseDirectory(FSFile *file)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsDir(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        if (FSi_SendCommand(file, FS_COMMAND_CLOSEDIRECTORY, TRUE))
        {
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDirectory

  Description:  �f�B���N�g���̃G���g����1�����ǂݐi�߂�

  Arguments:    file        FSFile�\����
                info        FSDirectoryEntryInfo�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_ReadDirectory(FSFile *file, FSDirectoryEntryInfo *info)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(file);
    SDK_NULL_ASSERT(info);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsDir(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForReadDirectory  arg[1];
        file->argument = arg;
        arg->info = info;
        MI_CpuFill8(info, 0x00, sizeof(info));
        info->id = FS_INVALID_FILE_ID;
        if (FSi_SendCommand(file, FS_COMMAND_READDIR, TRUE))
        {
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_SeekDir

  Description:  �f�B���N�g���ʒu���w�肵�ĊJ��

  Arguments:    file        FSFile�\����
                pos         FS_ReadDir��FS_TellDir�Ŏ擾�����f�B���N�g���ʒu

  Returns:      ����������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_SeekDir(FSFile *file, const FSDirPos *pos)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(file);
    SDK_NULL_ASSERT(pos);
    SDK_NULL_ASSERT(pos->arc);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArgumentForSeekDirectory  arg[1];
        arg->id = (u32)((pos->own_id << 0) | (pos->index << 16));
        arg->position = pos->pos;
        file->arc = pos->arc;
        file->argument = arg;
        if (FSi_SendCommand(file, FS_COMMAND_SEEKDIR, TRUE))
        {
            file->stat |= FS_FILE_STATUS_IS_DIR;
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_TellDir

  Description:  �f�B���N�g���n���h�����猻�݂̃f�B���N�g���ʒu���擾

  Arguments:    dir         �f�B���N�g���n���h��
                pos         �f�B���N�g���ʒu�̊i�[��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_TellDir(const FSFile *dir, FSDirPos *pos)
{
    BOOL        retval = FALSE;
    SDK_NULL_ASSERT(dir);
    SDK_NULL_ASSERT(pos);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsDir(dir));
    {
        *pos = dir->prop.dir.pos;
        retval = TRUE;
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RewindDir

  Description:  �f�B���N�g���n���h���̗񋓈ʒu��擪�֖߂�

  Arguments:    dir         �f�B���N�g���n���h��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_RewindDir(FSFile *dir)
{
    BOOL        retval = FALSE;
    SDK_NULL_ASSERT(dir);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsDir(dir));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

    {
        FSDirPos pos;
        pos.arc = dir->arc;
        pos.own_id = dir->prop.dir.pos.own_id;
        pos.pos = 0;
        pos.index = 0;
        retval = FS_SeekDir(dir, &pos);
    }
    return retval;
}


/*---------------------------------------------------------------------------*
 * Unicode support
 *---------------------------------------------------------------------------*/

enum
{
    FS_UNICODE_CONVSRC_ASCII,
    FS_UNICODE_CONVSRC_SHIFT_JIS,
    FS_UNICODE_CONVSRC_UNICODE
};

/*---------------------------------------------------------------------------*
  Name:         FSi_CopySafeUnicodeString

  Description:   �o�b�t�@�T�C�Y���m�F���������Unicode�Ƃ��ăR�s�[�B

  Arguments:    dst           �]����o�b�t�@
                dstlen        �]����T�C�Y
                src           �]�����o�b�t�@
                srclen        �]���敶���T�C�Y
                srctype       �]�����̕����Z�b�g
                stickyFailure �]�����̐؂�̂Ă��N��������FALSE
                
  Returns:      ���ۂɊi�[���ꂽ�������B
 *---------------------------------------------------------------------------*/
static int FSi_CopySafeUnicodeString(u16 *dst, int dstlen,
                                     const void *srcptr, int srclen,
                                     int srctype, BOOL *stickyFailure)
{
    int     srcpos = 0;
    int     dstpos = 0;
    if (srctype == FS_UNICODE_CONVSRC_ASCII)
    {
        const char *src = (const char *)srcptr;
        int     n = (dstlen - 1 < srclen) ? (dstlen - 1) : srclen;
        while ((dstpos < n) && src[srcpos])
        {
            dst[dstpos++] = (u8)src[srcpos++];
        }
        if ((srcpos < srclen) && src[srcpos])
        {
            *stickyFailure = TRUE;
        }
    }
    else if (srctype == FS_UNICODE_CONVSRC_UNICODE)
    {
        const u16 *src = (const u16 *)srcptr;
        int     n = (dstlen - 1 < srclen) ? (dstlen - 1) : srclen;
        while ((dstpos < n) && src[srcpos])
        {
            dst[dstpos++] = src[srcpos++];
        }
        if ((srcpos < srclen) && src[srcpos])
        {
            *stickyFailure = TRUE;
        }
    }
    else if (srctype == FS_UNICODE_CONVSRC_SHIFT_JIS)
    {
        const char *src = (const char *)srcptr;
        srcpos = srclen;
        dstpos = dstlen - 1;
        (void)FSi_ConvertStringSjisToUnicode(dst, &dstpos, src, &srcpos, NULL);
        if ((srcpos < srclen) && src[srcpos])
        {
            *stickyFailure = TRUE;
        }
    }
    dst[dstpos] = L'\0';
    return dstpos;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_NormalizePathWtoW

  Description:  Unicode�p�X���A�[�J�C�u���܂Ŋ܂�Unicode�t���p�X�֕ϊ��B

  Arguments:    path        ���K������Ă��Ȃ��p�X������
                baseid      ��f�B���N�g��ID�̊i�[��܂���NULL
                relpath     �ϊ���̃p�X���i�[��܂���NULL

  Returns:      �A�[�J�C�u�|�C���^�܂���NULL
 *---------------------------------------------------------------------------*/
FSArchive* FSi_NormalizePathWtoW(const u16 *path, u32*baseid, u16 *relpath);
FSArchive* FSi_NormalizePathWtoW(const u16 *path, u32*baseid, u16 *relpath)
{
    FSArchive  *arc = NULL;
    int         pathlen = 0;
    int         pathmax = FS_ARCHIVE_FULLPATH_MAX + 1;
    BOOL        stickyFailure = FALSE;
    // �܂��R�}���h�ΏۂƂȂ�A�[�J�C�u�����B
    // �w�肳�ꂽUnicode�p�X����΃p�X�Ȃ�A�[�J�C�u���擾�B
    BOOL        absolute = FALSE;
    int         arcnameLen;
    for (arcnameLen = 0; arcnameLen < FS_ARCHIVE_NAME_LONG_MAX + 1; ++arcnameLen)
    {
        if (path[arcnameLen] == L'\0')
        {
            break;
        }
        else if (FSi_IsUnicodeSlash(path[arcnameLen]))
        {
            break;
        }
        else if (path[arcnameLen] == L':')
        {
            char    arcname[FS_ARCHIVE_NAME_LONG_MAX + 1];
            int     j;
            for (j = 0; j < arcnameLen; ++j)
            {
                arcname[j] = (char)path[j];
            }
            arcname[arcnameLen] = '\0';
            arc = FS_FindArchive(arcname, arcnameLen);
            break;
        }
    }
    if (arc)
    {
        absolute = TRUE;
        *baseid = 0;
    }
    else
    {
        arc = FS_NormalizePath("", baseid, NULL);
    }
    if (arc)
    {
        // �A�[�J�C�u��Unicode�Ή��\�łȂ���΂����Ŏ��s�B
        u32     caps = 0;
        (void)arc->vtbl->GetArchiveCaps(arc, &caps);
        if ((caps & FS_ARCHIVE_CAPS_UNICODE) == 0)
        {
            arc = NULL;
        }
        else
        {
            // �擪�ɃA�[�J�C�u�����i�[�B
            pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                 FS_GetArchiveName(arc), FS_ARCHIVE_NAME_LONG_MAX,
                                                 FS_UNICODE_CONVSRC_ASCII, &stickyFailure);
            pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                 L":", 1,
                                                 FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
            // ��΃p�X�Ȃ烋�[�g�ȉ������̂܂ܘA���B
            if (absolute)
            {
                path += arcnameLen + 1 + FSi_IsUnicodeSlash(path[arcnameLen + 1]);
            }
            // �J�����g���[�g�Ȃ烋�[�g�ȉ��𒼐ژA���B
            else if (FSi_IsUnicodeSlash(*path))
            {
                path += 1;
            }
            // �J�����g�f�B���N�g���Ȃ�Shift_JIS->Unicode�ϊ����ĘA���B
            else
            {
                pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                     L"/", 1,
                                                     FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
                pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                     FS_GetCurrentDirectory(), FS_ENTRY_LONGNAME_MAX,
                                                     FS_UNICODE_CONVSRC_SHIFT_JIS, &stickyFailure);
            }
            // �c��̕�����A���B
            pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                 L"/", 1,
                                                 FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
            {
                // ����G���g�����ɒ��ӂ����΃p�X�𐳋K���B
                int     curlen = 0;
                while (!stickyFailure)
                {
                    u16     c = path[curlen];
                    if ((c != L'\0') && !FSi_IsUnicodeSlash(c))
                    {
                        curlen += 1;
                    }
                    else
                    {
                        // ��f�B���N�g���͖����B
                        if (curlen == 0)
                        {
                        }
                        // "." (�J�����g�f�B���N�g��)�͖����B
                        else if ((curlen == 1) && (path[0] == L'.'))
                        {
                        }
                        // ".." (�e�f�B���N�g��)�̓��[�g������Ƃ���1�K�w�オ��B
                        else if ((curlen == 2) && (path[0] == '.') && (path[1] == '.'))
                        {
                            if ((pathlen > 2) && (relpath[pathlen - 2] != L':'))
                            {
                                --pathlen;
                                pathlen = FSi_DecrementUnicodePositionToSlash(relpath, pathlen) + 1;
                            }
                        }
                        // ����ȊO�̓G���g���ǉ��B
                        else
                        {
                            pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                                 path, curlen,
                                                                 FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
                            if (c != L'\0')
                            {
                                pathlen += FSi_CopySafeUnicodeString(&relpath[pathlen], pathmax - pathlen,
                                                                     L"/", 1,
                                                                     FS_UNICODE_CONVSRC_UNICODE, &stickyFailure);
                            }
                        }
                        if (c == L'\0')
                        {
                            break;
                        }
                        path += curlen + 1;
                        curlen = 0;
                    }
                }
            }
            relpath[pathlen] = L'\0';
        }
    }
    return stickyFailure ? NULL : arc;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFileExW

  Description:  �p�X�����w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFileExW(FSFile *file, const u16 *path, u32 mode)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(file);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);

    // FS_FILEMODE_L �Ɋւ���_���`�F�b�N�B
    // (creation���[�h�ŊJ���T�C�Y����������Ӗ�������)
    if (((mode & FS_FILEMODE_L) != 0) &&
        ((mode & FS_FILEMODE_RW) == FS_FILEMODE_W))
    {
        OS_TWarning("\"FS_FILEMODE_WL\" seems useless.\n"
                    "(this means creating empty file and prohibiting any modifications)");
    }
    {
        u16         relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FSi_NormalizePathWtoW(path, &baseid, relpath);
        // �����_�ł�Unicode���g�p�\�ȍŏ��̕ύX�ɂƂǂ߂邽�߂�
        // ROM�Ȃ�Unicode��Ή��̃A�[�J�C�u�ł�Unsupported��Ԃ��B
        if (!arc)
        {
            file->error = FS_RESULT_UNSUPPORTED;
        }
        else
        {
            FSArgumentForOpenFile   arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = (char*)relpath;
            arg->mode = mode;
            file->stat |= FS_FILE_STATUS_UNICODE_MODE;
            if (FSi_SendCommand(file, FS_COMMAND_OPENFILE, TRUE))
            {
                retval = TRUE;
            }
            else
            {
                file->arc = NULL;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenDirectoryW

  Description:  �f�B���N�g���n���h�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��
                mode        �A�N�Z�X���[�h

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenDirectoryW(FSFile *file, const u16 *path, u32 mode)
{
    BOOL    retval = FALSE;
    FS_DEBUG_TRACE( "%s\n", __FUNCTION__);
    SDK_NULL_ASSERT(path);
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        u16         relpath[FS_ARCHIVE_FULLPATH_MAX + 1];
        u32         baseid = 0;
        FSArchive  *arc = FSi_NormalizePathWtoW(path, &baseid, relpath);
        // �����_�ł�Unicode���g�p�\�ȍŏ��̕ύX�ɂƂǂ߂邽�߂�
        // ROM�Ȃ�Unicode��Ή��̃A�[�J�C�u�ł�Unsupported��Ԃ��B
        if (!arc)
        {
            file->error = FS_RESULT_UNSUPPORTED;
        }
        else
        {
            FSArgumentForOpenDirectory  arg[1];
            FS_InitFile(file);
            file->arc = arc;
            file->argument = arg;
            arg->baseid = baseid;
            arg->relpath = (char*)relpath;
            arg->mode = mode;
            file->stat |= FS_FILE_STATUS_UNICODE_MODE;
            if (FSi_SendCommand(file, FS_COMMAND_OPENDIRECTORY, TRUE))
            {
                retval = TRUE;
            }
            else
            {
                file->arc = NULL;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDirectoryW

  Description:  �f�B���N�g���̃G���g����1�����ǂݐi�߂�

  Arguments:    file        FSFile�\����
                info        FSDirectoryEntryInfo�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_ReadDirectoryW(FSFile *file, FSDirectoryEntryInfoW *info)
{
    BOOL    retval = FALSE;
    SDK_NULL_ASSERT(file);
    SDK_NULL_ASSERT(info);
    SDK_ASSERT(FS_IsAvailable());
    SDK_ASSERT(FS_IsDir(file));
    SDK_ASSERT(OS_GetProcMode() != OS_PROCMODE_IRQ);
    {
        FSArchive  *arc = file->arc;
        // ���݂�Unicode���g�p�ł���ŏ��̕ύX�ɂƂǂ߂邽�߁A
        // ROM�Ȃ�Unicode��Ή��̃A�[�J�C�u�ł�Unsupported��Ԃ��B
        u32     caps = 0;
        (void)arc->vtbl->GetArchiveCaps(arc, &caps);
        if ((caps & FS_ARCHIVE_CAPS_UNICODE) == 0)
        {
            file->error = FS_RESULT_UNSUPPORTED;
        }
        else
        {
            FSArgumentForReadDirectory  arg[1];
            file->argument = arg;
            arg->info = (FSDirectoryEntryInfo*)info;
            MI_CpuFill8(info, 0x00, sizeof(info));
            info->id = FS_INVALID_FILE_ID;
            file->stat |= FS_FILE_STATUS_UNICODE_MODE;
            if (FSi_SendCommand(file, FS_COMMAND_READDIR, TRUE))
            {
                retval = TRUE;
            }
        }
    }
    return retval;
}


/*---------------------------------------------------------------------------*
 * obsolete functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         FSi_ConvertToDirEntry

  Description:  FSDirectoryEntryInfo�\���̂���FSDirEntry�\���̂ւ̕ϊ�

  Arguments:    entry       �ϊ����FSDirEntry�\����
                info        �ϊ�����FSDirectoryEntryInfo�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
static void FSi_ConvertToDirEntry(FSDirEntry *entry, const FSDirectoryEntryInfo *info)
{
    entry->name_len = info->longname_length;
    if (entry->name_len > sizeof(entry->name) - 1)
    {
        entry->name_len = sizeof(entry->name) - 1;
    }
    MI_CpuCopy8(info->longname, entry->name, entry->name_len);
    entry->name[entry->name_len] = '\0';
    if (info->id == FS_INVALID_FILE_ID)
    {
        entry->is_directory = FALSE;
        entry->file_id.file_id = FS_INVALID_FILE_ID;
    }
    else if ((info->attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0)
    {
        entry->is_directory = TRUE;
        entry->dir_id.own_id = (u16)(info->id >> 0);
        entry->dir_id.index = (u16)(info->id >> 16);
        entry->dir_id.pos = 0;
    }
    else
    {
        entry->is_directory = FALSE;
        entry->file_id.file_id = info->id;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_OpenFile

  Description:  �p�X�����w�肵�ăt�@�C�����J��

  Arguments:    file        FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_OpenFile(FSFile *file, const char *path)
{
    return FS_OpenFileEx(file, path, FS_FILEMODE_R);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetLength

  Description:  �t�@�C���T�C�Y���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���T�C�Y
 *---------------------------------------------------------------------------*/
u32 FS_GetLength(FSFile *file)
{
    return FS_GetFileLength(file);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetPosition

  Description:  �t�@�C���|�C���^�̌��݈ʒu���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �t�@�C���|�C���^�̌��݈ʒu
 *---------------------------------------------------------------------------*/
u32 FS_GetPosition(FSFile *file)
{
    return FS_GetFilePosition(file);
}

/*---------------------------------------------------------------------------*
  Name:         FS_FindDir

  Description:  �f�B���N�g���n���h�����J��

  Arguments:    dir         FSFile�\����
                path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_FindDir(FSFile *dir, const char *path)
{
    return FS_OpenDirectory(dir, path, FS_FILEMODE_R);
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReadDir

  Description:  �f�B���N�g���̃G���g����1�����ǂݐi�߂�

  Arguments:    file        FSFile�\����
                entry       FSDirEntry�\����

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_ReadDir(FSFile *file, FSDirEntry *entry)
{
    BOOL                    retval = FALSE;
    FSDirectoryEntryInfo    info[1];
    if (FS_ReadDirectory(file, info))
    {
        FSi_ConvertToDirEntry(entry, info);
        retval = TRUE;
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_ChangeDir

  Description:  �J�����g�f�B���N�g����ύX����

  Arguments:    path        �p�X��

  Returns:      ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    FS_ChangeDir(const char *path)
{
    return FS_SetCurrentDirectory(path);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileInfo

  Description:  �t�@�C�������擾����

  Arguments:    path        �p�X��
                info        ���̊i�[��

  Returns:      ��������
 *---------------------------------------------------------------------------*/
FSResult FS_GetFileInfo(const char *path, FSFileInfo *info)
{
    return FS_GetPathInfo(path, info) ? FS_RESULT_SUCCESS : FS_GetArchiveResultCode(path);
}


#endif /* FS_IMPLEMENT */

// �ȉ���FS���C�u�����ȊO�ł��g�p�����̂�FS_IMPLEMENT�̑ΏۊO�B
// ARM7�ł�Unicode�Ή����K�v�ɂȂ�̂�TWL���쎞�݂̂Ȃ̂Ŋg���������֔z�u�B
#if defined(SDK_TWL) && defined(SDK_ARM7)
#include <twl/ltdmain_begin.h>
#endif

static const int        FSiUnicodeBufferQueueMax = 4;
static OSMessageQueue   FSiUnicodeBufferQueue[1];
static OSMessage        FSiUnicodeBufferQueueArray[FSiUnicodeBufferQueueMax];
static BOOL             FSiUnicodeBufferQueueInitialized = FALSE;
static u16              FSiUnicodeBufferTable[FSiUnicodeBufferQueueMax][FS_ARCHIVE_FULLPATH_MAX + 1];

/*---------------------------------------------------------------------------*
  Name:         FSi_GetUnicodeBuffer

  Description:  Unicode�ϊ��p�̈ꎞ�o�b�t�@���擾�B
                FS���C�u������Shift_JIS��ϊ����邽�߂Ɏg�p�����B

  Arguments:    src : Unicode�ϊ��̕K�v��Shift_JIS������܂���NULL

  Returns:      �K�v�ɉ�����UTF16-LE�֕ϊ����ꂽ������o�b�t�@
 *---------------------------------------------------------------------------*/
u16* FSi_GetUnicodeBuffer(const char *src)
{
    u16        *retval = NULL;
    // ����Ăяo�����Ƀo�b�t�@�����b�Z�[�W�L���[�֒ǉ��B
    OSIntrMode  bak = OS_DisableInterrupts();
    if (!FSiUnicodeBufferQueueInitialized)
    {
        int     i;
        FSiUnicodeBufferQueueInitialized = TRUE;
        OS_InitMessageQueue(FSiUnicodeBufferQueue, FSiUnicodeBufferQueueArray, 4);
        for (i = 0; i < FSiUnicodeBufferQueueMax; ++i)
        {
            (void)OS_SendMessage(FSiUnicodeBufferQueue, FSiUnicodeBufferTable[i], OS_MESSAGE_BLOCK);
        }
    }
    (void)OS_RestoreInterrupts(bak);
    // ���b�Z�[�W�L���[����o�b�t�@���m�ہB(������΂����Ńu���b�L���O)
    (void)OS_ReceiveMessage(FSiUnicodeBufferQueue, (OSMessage*)&retval, OS_MESSAGE_BLOCK);
    if (src)
    {
        int     dstlen = FS_ARCHIVE_FULLPATH_MAX;
        (void)FSi_ConvertStringSjisToUnicode(retval, &dstlen, src, NULL, NULL);
        retval[dstlen] = L'\0';
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReleaseUnicodeBuffer

  Description:  Unicode�ϊ��p�̈ꎞ�o�b�t�@������B

  Arguments:    buf : FSi_GetUnicodeBuffer()�Ŋm�ۂ����o�b�t�@

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ReleaseUnicodeBuffer(const void *buf)
{
    if (buf)
    {
        // �g�p�����o�b�t�@�����b�Z�[�W�L���[�֕ԋp�B
        (void)OS_SendMessage(FSiUnicodeBufferQueue, (OSMessage)buf, OS_MESSAGE_BLOCK);
    }
}

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
SDK_WEAK_SYMBOL
STDResult FSi_ConvertStringSjisToUnicode(u16 *dst, int *dst_len,
                                         const char *src, int *src_len,
                                         STDConvertUnicodeCallback callback)
 __attribute__((never_inline))
{
    return STD_ConvertStringSjisToUnicode(dst, dst_len, src, src_len, callback);
}

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
SDK_WEAK_SYMBOL
STDResult FSi_ConvertStringUnicodeToSjis(char *dst, int *dst_len,
                                         const u16 *src, int *src_len,
                                         STDConvertSjisCallback callback)
 __attribute__((never_inline))
{
    return STD_ConvertStringUnicodeToSjis(dst, dst_len, src, src_len, callback);
}

#if defined(SDK_TWL) && defined(SDK_ARM7)
#include <twl/ltdmain_end.h>
#endif
