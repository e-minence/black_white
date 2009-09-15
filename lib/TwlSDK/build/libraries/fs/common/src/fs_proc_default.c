/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     fs_romfat_default.c

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
#include <nitro/math/math.h>
#include <nitro/std.h>


#include "../include/util.h"
#include "../include/command.h"
#include "../include/rom.h"


#if defined(FS_IMPLEMENT)


/*---------------------------------------------------------------------------*/
/* declarations */

// �����n�R�}���h�̃u���b�L���O�ǂݍ��ݍ\����
typedef struct FSiSyncReadParam
{
    FSArchive  *arc;
    u32         pos;
}
FSiSyncReadParam;

// �r�b�g�ƃA���C�������g�̃}�N��
#define	BIT_MASK(n)	((1 << (n)) - 1)


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         FSi_SeekAndReadSRL

  Description:  ROM�n�b�V���R���e�L�X�g����Ăяo�����SRL�A�N�Z�X�R�[���o�b�N�B

  Arguments:    userdata         �C�ӂ̃��[�U��`�f�[�^
                buffer           �]����o�b�t�@
                offset           �]����ROM�I�t�Z�b�g
                length           �]���T�C�Y

  Returns:      �]���T�C�Y
 *---------------------------------------------------------------------------*/
static int FSi_SeekAndReadSRL(void *userdata, void *buffer, u32 offset, u32 length)
{
    FSFile     *file = (FSFile*)userdata;
    if (file)
    {
        (void)FS_SeekFile(file, (int)offset, FS_SEEK_SET);
        (void)FS_ReadFile(file, buffer, (int)length);
    }
    return (int)length;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_TranslateCommand

  Description:  ���[�U�v���V�[�W���܂��̓f�t�H���g�������Ăяo�����ʂ�Ԃ��B
                �����n�R�}���h���񓯊��I������Ԃ����ꍇ�͓����Ŋ�����҂B
                �񓯊��n�R�}���h���񓯊��I������Ԃ����ꍇ�͂��̂܂ܕԂ��B

  Arguments:    p_file           �R�}���h�����̊i�[���ꂽFSFile�\����
                command          �R�}���hID
                block            �u���b�L���O����Ȃ�TRUE

  Returns:      �R�}���h�̏�������.
 *---------------------------------------------------------------------------*/
static FSResult FSi_TranslateCommand(FSFile *p_file, FSCommandType command, BOOL block);

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadTable

  Description:  �����n�R�}���h���œ������[�h�����s

  Arguments:    p                �������[�h�\����
                dst              �ǂݍ��݃f�[�^�i�[��o�b�t�@
                len              �ǂݍ��݃o�C�g��

  Returns:      �������[�h�̌���
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadTable(FSiSyncReadParam * p, void *dst, u32 len)
{
    FSResult                result;
    FSArchive       * const arc = p->arc;
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);


    if (context->load_mem)
    {
        MI_CpuCopy8((const void *)p->pos, dst, len);
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = (*context->read_func) (arc, dst, p->pos, len);
        result = FSi_WaitForArchiveCompletion(arc->list, result);
    }
    p->pos += len;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SeekDirDirect

  Description:  �w��f�B���N�g���̐擪�֒��ڈړ�

  Arguments:    file             �f�B���N�g�����X�g���i�[����|�C���^
                id               �f�B���N�g��ID

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_SeekDirDirect(FSFile *file, u32 id)
{
    file->arg.seekdir.pos.arc = file->arc;
    file->arg.seekdir.pos.own_id = (u16)id;
    file->arg.seekdir.pos.index = 0;
    file->arg.seekdir.pos.pos = 0;
    (void)FSi_TranslateCommand(file, FS_COMMAND_SEEKDIR, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SeekDirDefault

  Description:  SEEKDIR�R�}���h�̃f�t�H���g����

  Arguments:    file             �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_SeekDirDefault(FSFile *file)
{
    FSResult                result;
    FSArchive       * const arc = file->arc;
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);

    const FSDirPos  * const arg = &file->arg.seekdir.pos;
    FSArchiveFNT            fnt;
    FSiSyncReadParam        param;
    param.arc = arc;
    param.pos = context->fnt + arg->own_id * sizeof(fnt);
    result = FSi_ReadTable(&param, &fnt, sizeof(fnt));
    if (result == FS_RESULT_SUCCESS)
    {
        file->prop.dir.pos = *arg;
        if ((arg->index == 0) && (arg->pos == 0))
        {
            file->prop.dir.pos.index = fnt.index;
            file->prop.dir.pos.pos = context->fnt + fnt.start;
        }
        file->prop.dir.parent = (u32)(fnt.parent & BIT_MASK(12));
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadDirDefault

  Description:  READDIR�R�}���h�̃f�t�H���g����

  Arguments:    file             �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadDirDefault(FSFile *file)
{
    FSResult                result;
    FSDirEntry             *entry = file->arg.readdir.p_entry;
    u8                      len;

    FSiSyncReadParam        param;
    param.arc = file->arc;
    param.pos = file->prop.dir.pos.pos;

    result = FSi_ReadTable(&param, &len, sizeof(len));

    if (result == FS_RESULT_SUCCESS)
    {
        entry->name_len = (u32)(len & BIT_MASK(7));
        entry->is_directory = (u32)((len >> 7) & 1);
        if (entry->name_len == 0)
        {
            result = FS_RESULT_FAILURE;
        }
        else
        {
            if (!file->arg.readdir.skip_string)
            {
                result = FSi_ReadTable(&param, entry->name, entry->name_len);
                if (result != FS_RESULT_SUCCESS)
                {
                    return result;
                }
                entry->name[entry->name_len] = '\0';
            }
            else
            {
                param.pos += entry->name_len;
            }
            if (!entry->is_directory)
            {
                entry->file_id.arc = file->arc;
                entry->file_id.file_id = file->prop.dir.pos.index;
                ++file->prop.dir.pos.index;
            }
            else
            {
                u16     id;
                result = FSi_ReadTable(&param, &id, sizeof(id));
                if (result == FS_RESULT_SUCCESS)
                {
                    entry->dir_id.arc = file->arc;
                    entry->dir_id.own_id = (u16)(id & BIT_MASK(12));
                    entry->dir_id.index = 0;
                    entry->dir_id.pos = 0;
                }
            }
            if (result == FS_RESULT_SUCCESS)
            {
                file->prop.dir.pos.pos = param.pos;
            }
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FindPathDefault

  Description:  FINDPATH�R�}���h�̃f�t�H���g����
                (SEEKDIR, READDIR �R�}���h���g�p����)

  Arguments:    p_dir            �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_FindPathDefault(FSFile *p_dir)
{
    const char *path = p_dir->arg.findpath.path;
    const BOOL  is_dir = p_dir->arg.findpath.find_directory;

    // �w��̃f�B���N�g������Ƀp�X������
    p_dir->arg.seekdir.pos = p_dir->arg.findpath.pos;
    (void)FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR, TRUE);
    for (; *path; path += (*path ? 1 : 0))
    {
        // �����ΏۂƂȂ�G���g�����𒊏o
        int     name_len = FSi_IncrementSjisPositionToSlash(path, 0);
        u32     is_directory = ((path[name_len] != '\0') || is_dir);
        // "//" �Ȃǂ̕s���ȃf�B���N�g�������o�����玸�s
        if (name_len == 0)
        {
            return FS_RESULT_INVALID_PARAMETER;
        }
        // ����ȃf�B���N�g���w��
        else if (*path == '.')
        {
            // "." �̓J�����g�f�B���N�g��
            if (name_len == 1)
            {
                path += 1;
                continue;
            }
            // ".." �͐e�f�B���N�g��
            else if ((name_len == 2) & (path[1] == '.'))
            {
                if (p_dir->prop.dir.pos.own_id != 0)
                {
                    FSi_SeekDirDirect(p_dir, p_dir->prop.dir.parent);
                }
                path += 2;
                continue;
            }
        }
        else if (*path == '*')
        {
            // "*" �̓��C���h�J�[�h�w�� (�A�[�J�C�u�v���V�[�W���ł͂���𖳎�����)
            break;
        }
        // ��������G���g���������o�����玸�s
        if (name_len > FS_FILE_NAME_MAX)
        {
            return FS_RESULT_NO_ENTRY;
        }
        // �G���g����񋓂��ď�����r
        else
        {
            FSDirEntry etr;
            p_dir->arg.readdir.p_entry = &etr;
            p_dir->arg.readdir.skip_string = FALSE;
            for (;;)
            {
                // �I�[�ɒB���Ă��Y���G���g�������݂��Ȃ������玸�s
                if (FSi_TranslateCommand(p_dir, FS_COMMAND_READDIR, TRUE) != FS_RESULT_SUCCESS)
                {
                    return FS_RESULT_NO_ENTRY;
                }
                // ��v���Ȃ���Ζ���
                if ((is_directory == etr.is_directory) &&
                    (name_len == etr.name_len) && (FSi_StrNICmp(path, etr.name, (u32)name_len) == 0))
                {
                    // �f�B���N�g���Ȃ�Ăт��̐擪�ֈړ�
                    if (is_directory)
                    {
                        path += name_len;
                        p_dir->arg.seekdir.pos = etr.dir_id;
                        (void)FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR, TRUE);
                        break;
                    }
                    // �t�@�C���ł���A�f�B���N�g�������҂��Ă���Ύ��s
                    else if (is_dir)
                    {
                        return FS_RESULT_NO_ENTRY;
                    }
                    // �t�@�C���𔭌�
                    else
                    {
                        *p_dir->arg.findpath.result.file = etr.file_id;
                        return FS_RESULT_SUCCESS;
                    }
                }
            }
        }
    }
    // �t�@�C���𔭌��ł��Ȃ���Ύ��s
    if (!is_dir)
    {
        return FS_RESULT_NO_ENTRY;
    }
    else
    {
        *p_dir->arg.findpath.result.dir = p_dir->prop.dir.pos;
        return FS_RESULT_SUCCESS;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetPathDefault

  Description:  GETPATH �R�}���h�̃f�t�H���g����
                (SEEKDIR, READDIR �R�}���h���g�p����)

  Arguments:    file             �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_GetPathDefault(FSFile *file)
{
    FSResult            result;
    FSArchive   * const arc = file->arc;
    FSGetPathInfo      *p_info = &file->arg.getpath;
    FSDirEntry          entry;
    u32                 dir_id;
    u32                 file_id;

    enum
    { INVALID_ID = 0x10000 };

    FSFile              tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;

    if (FS_IsDir(file))
    {
        dir_id = file->prop.dir.pos.own_id;
        file_id = INVALID_ID;
    }
    else
    {
        file_id = file->prop.file.own_id;
        {
            // ���[�g����S�f�B���N�g��������
            u32     pos = 0;
            u32     num_dir = 0;
            dir_id = INVALID_ID;
            do
            {
                FSi_SeekDirDirect(tmp, pos);
                // ����͑S�f�B���N�g�������擾
                if (!pos)
                {
                    num_dir = tmp->prop.dir.parent;
                }
                tmp->arg.readdir.p_entry = &entry;
                tmp->arg.readdir.skip_string = TRUE;
                while (FSi_TranslateCommand(tmp, FS_COMMAND_READDIR, TRUE) == FS_RESULT_SUCCESS)
                {
                    if (!entry.is_directory && (entry.file_id.file_id == file_id))
                    {
                        dir_id = tmp->prop.dir.pos.own_id;
                        break;
                    }
                }
            }
            while ((dir_id == INVALID_ID) && (++pos < num_dir));
        }
    }

    // �Y������f�B���N�g����������Ȃ����FALSE
    if (dir_id == INVALID_ID)
    {
        p_info->total_len = 0;
        result = FS_RESULT_NO_ENTRY;
    }
    else
    {
        // �p�X���𖢌v���Ȃ獡���x�v��
        {
            u32         id = dir_id;
            // �܂�"�A�[�J�C�u��:/" �����Z
            const char *arcname = FS_GetArchiveName(arc);
            u32         len = (u32)STD_GetStringLength(arcname);
            len += 2;
            // ��f�B���N�g���ֈړ�
            FSi_SeekDirDirect(tmp, id);
            // �K�v�Ȃ�t�@�C���������Z (���łɎ擾�ς�)
            if (file_id != INVALID_ID)
            {
                len += entry.name_len;
            }
            // ���������̂ڂ��Ď��g�̖��O�����Z
            if (id != 0)
            {
                do
                {
                    // �e�Ɉړ����Ď��g������
                    FSi_SeekDirDirect(tmp, tmp->prop.dir.parent);
                    tmp->arg.readdir.p_entry = &entry;
                    tmp->arg.readdir.skip_string = TRUE;
                    while (FSi_TranslateCommand(tmp, FS_COMMAND_READDIR, TRUE) == FS_RESULT_SUCCESS)
                    {
                        if (entry.is_directory && (entry.dir_id.own_id == id))
                        {
                            len += entry.name_len + 1;
                            break;
                        }
                    }
                    id = tmp->prop.dir.pos.own_id;
                }
                while (id != 0);
            }
            // ����v�Z�ς݂̃f�[�^�͕ۑ����Ă���
            p_info->total_len = (u16)(len + 1);
            p_info->dir_id = (u16)dir_id;
        }

        // �p�X���̌v�����ړI�Ȃ獡��͐���
        if (!p_info->buf)
        {
            result = FS_RESULT_SUCCESS;
        }
        // �o�b�t�@�����s���Ȃ獡��͎��s
        else if (p_info->buf_len < p_info->total_len)
        {
            result = FS_RESULT_NO_MORE_RESOURCE;
        }
        // ��������I�[����i�[
        else
        {
            u8         *dst = p_info->buf;
            u32         total = p_info->total_len;
            u32         pos = 0;
            u32         id = dir_id;
            // �܂�"�A�[�J�C�u��:/" �����Z
            const char *arcname = FS_GetArchiveName(arc);
            u32         len = (u32)STD_GetStringLength(arcname);
            MI_CpuCopy8(arcname, dst + pos, len);
            pos += len;
            MI_CpuCopy8(":/", dst + pos, 2);
            pos += 2;
            // ��f�B���N�g���ֈړ�
            FSi_SeekDirDirect(tmp, id);
            // �K�v�Ȃ�t�@�C���������Z
            if (file_id != INVALID_ID)
            {
                tmp->arg.readdir.p_entry = &entry;
                tmp->arg.readdir.skip_string = FALSE;
                while (FSi_TranslateCommand(tmp, FS_COMMAND_READDIR, TRUE) == FS_RESULT_SUCCESS)
                {
                    if (!entry.is_directory && (entry.file_id.file_id == file_id))
                    {
                        break;
                    }
                }
                len = entry.name_len + 1;
                MI_CpuCopy8(entry.name, dst + total - len, len);
                total -= len;
            }
            else
            {
                // �f�B���N�g���Ȃ�I�[�����t��
                dst[total - 1] = '\0';
                total -= 1;
            }
            // ���������̂ڂ��Ď��g�̖��O�����Z
            if (id != 0)
            {
                do
                {
                    // �e�Ɉړ����Ď��g������
                    FSi_SeekDirDirect(tmp, tmp->prop.dir.parent);
                    tmp->arg.readdir.p_entry = &entry;
                    tmp->arg.readdir.skip_string = FALSE;
                    // "/" ��t��
                    dst[total - 1] = '/';
                    total -= 1;
                    // �e�̎q(�ȑO�̎��g)������
                    while (FSi_TranslateCommand(tmp, FS_COMMAND_READDIR, TRUE) == FS_RESULT_SUCCESS)
                    {
                        if (entry.is_directory && (entry.dir_id.own_id == id))
                        {
                            u32     len = entry.name_len;
                            MI_CpuCopy8(entry.name, dst + total - len, len);
                            total -= len;
                            break;
                        }
                    }
                    id = tmp->prop.dir.pos.own_id;
                }
                while (id != 0);
            }
            SDK_ASSERT(pos == total);
        }
        result = FS_RESULT_SUCCESS;
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_OpenFileFastDefault

  Description:  OPENFILEFAST�R�}���h�̃f�t�H���g����
                (OPENFILEDIRECT �R�}���h���g�p����)

  Arguments:    p_file           �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_OpenFileFastDefault(FSFile *p_file)
{
    FSResult                result;
    FSArchive       * const p_arc = p_file->arc;
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(p_arc);
    const FSFileID         *p_id = &p_file->arg.openfilefast.id;
    const u32               index = p_id->file_id;

    {
        // FAT �A�N�Z�X�̈�̐���������
        u32     pos = (u32)(index * sizeof(FSArchiveFAT));
        if (pos >= context->fat_size)
        {
            result = FS_RESULT_NO_ENTRY;
        }
        else
        {
            // FAT ����w��t�@�C���̃C���[�W�̈���擾
            FSArchiveFAT fat;
            FSiSyncReadParam param;
            param.arc = p_arc;
            param.pos = context->fat + pos;
            result = FSi_ReadTable(&param, &fat, sizeof(fat));
            if (result == FS_RESULT_SUCCESS)
            {
                // ���ڃI�[�v��
                p_file->arg.openfiledirect.top = fat.top;
                p_file->arg.openfiledirect.bottom = fat.bottom;
                p_file->arg.openfiledirect.index = index;
                result = FSi_TranslateCommand(p_file, FS_COMMAND_OPENFILEDIRECT, TRUE);
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_OpenFileDirectDefault

  Description:  OPENFILEDIRECT�R�}���h�̃f�t�H���g����

  Arguments:    p_file           �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_OpenFileDirectDefault(FSFile *p_file)
{
    p_file->prop.file.top = p_file->arg.openfiledirect.top;
    p_file->prop.file.pos = p_file->arg.openfiledirect.top;
    p_file->prop.file.bottom = p_file->arg.openfiledirect.bottom;
    p_file->prop.file.own_id = p_file->arg.openfiledirect.index;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadFileDefault

  Description:  READFILE�R�}���h�̃f�t�H���g����

  Arguments:    file             �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadFileDefault(FSFile *file)
{
    FSArchive       * const arc = file->arc;
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    const u32               pos = file->prop.file.pos;
    const u32               len = file->arg.readfile.len;
    void            * const dst = file->arg.readfile.dst;
    file->prop.file.pos += len;
    return (*context->read_func) (arc, dst, pos, len);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WriteFileDefault

  Description:  WRITEFILE�R�}���h�̃f�t�H���g����

  Arguments:    file             �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_WriteFileDefault(FSFile *file)
{
    FSArchive       * const arc = file->arc;
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    const u32               pos = file->prop.file.pos;
    const u32               len = file->arg.writefile.len;
    const void      * const src = file->arg.writefile.src;
    file->prop.file.pos += len;
    return (*context->write_func) (arc, src, pos, len);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IgnoredCommand

  Description:  �f�t�H���g�ł͉������Ȃ��R�}���h�̎���

  Arguments:    file             �R�}���h����������n���h��

  Returns:      �R�}���h���ʒl
 *---------------------------------------------------------------------------*/
static FSResult FSi_IgnoredCommand(FSFile *file)
{
    (void)file;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_TranslateCommand

  Description:  ���[�U�v���V�[�W���܂��̓f�t�H���g�������Ăяo���B
                �����n�R�}���h���񓯊��I������Ԃ����ꍇ�͓����Ŋ�����҂B
                �񓯊��n�R�}���h���񓯊��I������Ԃ����ꍇ�͂��̂܂ܕԂ��B

  Arguments:    file             �������i�[����FSFile�\����
                command          �R�}���h ID
                block            �u���b�L���O���K�v�Ȃ�TRUE

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
FSResult FSi_TranslateCommand(FSFile *file, FSCommandType command, BOOL block)
{
    FSResult                result = FS_RESULT_PROC_DEFAULT;
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(file->arc);
    // �v���V�[�W�����Ή����Ă���R�}���h�Ȃ���s������
    if ((context->proc_flag & (1 << command)) != 0)
    {
        result = (*context->proc) (file, command);
        switch (result)
        {
        case FS_RESULT_SUCCESS:
        case FS_RESULT_FAILURE:
        case FS_RESULT_UNSUPPORTED:
            break;
        case FS_RESULT_PROC_ASYNC:
            // �񓯊������̈����͂��̌�ōׂ�������
            break;
        case FS_RESULT_PROC_UNKNOWN:
            // ���m�̃R�}���h�Ȃ̂ō���ȍ~�̓f�t�H���g�����ɐ؂�ւ���
            result = FS_RESULT_PROC_DEFAULT;
            context->proc_flag &= ~(1 << command);
            break;
        }
    }
    // �v���V�[�W���ŏ�������Ȃ������Ȃ�f�t�H���g�̏������Ăяo��
    if (result == FS_RESULT_PROC_DEFAULT)
    {
        static FSResult (*const (default_table[])) (FSFile *) =
        {
            FSi_ReadFileDefault,
            FSi_WriteFileDefault,
            FSi_SeekDirDefault,
            FSi_ReadDirDefault,
            FSi_FindPathDefault,
            FSi_GetPathDefault,
            FSi_OpenFileFastDefault,
            FSi_OpenFileDirectDefault,
            FSi_IgnoredCommand,
            FSi_IgnoredCommand,
            FSi_IgnoredCommand,
            FSi_IgnoredCommand,
            FSi_IgnoredCommand,
        };
        if (command < sizeof(default_table) / sizeof(*default_table))
        {
            result = (*default_table[command])(file);
        }
        else
        {
            result = FS_RESULT_UNSUPPORTED;
        }
    }
    // �K�v�Ȃ炱�̏�Ńu���b�L���O
    if (block)
    {
        result = FSi_WaitForArchiveCompletion(file, result);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_ReadFile

  Description:  FS_COMMAND_READFILE�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                buffer           �]���惁����
                length           �]���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_ReadFile(FSArchive *arc, FSFile *file, void *buffer, u32 *length)
{
    FSROMFATProperty   *prop = (FSROMFATProperty*)FS_GetFileUserData(file);
    const u32 pos = prop->file.pos;
    const u32 rest = (u32)(prop->file.bottom - pos);
    const u32 org = *length;
    if (*length > rest)
    {
        *length = rest;
    }
    file->arg.readfile.dst = buffer;
    file->arg.readfile.len_org = org;
    file->arg.readfile.len = *length;
    (void)arc;
    return FSi_TranslateCommand(file, FS_COMMAND_READFILE, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_WriteFile

  Description:  FS_COMMAND_WRITEFILE�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                buffer           �]����������
                length           �]���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_WriteFile(FSArchive *arc, FSFile *file, const void *buffer, u32 *length)
{
    FSROMFATProperty   *prop = (FSROMFATProperty*)FS_GetFileUserData(file);
    const u32 pos = prop->file.pos;
    const u32 rest = (u32)(prop->file.bottom - pos);
    const u32 org = *length;
    if (*length > rest)
    {
        *length = rest;
    }
    file->arg.writefile.src = buffer;
    file->arg.writefile.len_org = org;
    file->arg.writefile.len = *length;
    (void)arc;
    return FSi_TranslateCommand(file, FS_COMMAND_WRITEFILE, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_SeekDirectory

  Description:  FS_COMMAND_SEEKDIR�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                id               ��ӂȃf�B���N�g��ID
                position         �񋓏󋵂�����

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_SeekDirectory(FSArchive *arc, FSFile *file, u32 id, u32 position)
{
    FSResult                result;
    FSROMFATCommandInfo    *arg = &file->arg;
    file->arc = arc;
    arg->seekdir.pos.arc = arc;
    arg->seekdir.pos.own_id = (u16)(id >> 0);
    arg->seekdir.pos.index = (u16)(id >> 16);
    arg->seekdir.pos.pos = position;
    result = FSi_TranslateCommand(file, FS_COMMAND_SEEKDIR, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        FS_SetDirectoryHandle(file, arc, &file->prop);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_ReadDirectory

  Description:  FS_COMMAND_READDIR�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                info             ���̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_ReadDirectory(FSArchive *arc, FSFile *file, FSDirectoryEntryInfo *info)
{
    FSResult                result;
    FSDirEntry              entry[1];
    FSROMFATCommandInfo    *arg = &file->arg;
    arg->readdir.p_entry = entry;
    arg->readdir.skip_string = FALSE;
    result = FSi_TranslateCommand(file, FS_COMMAND_READDIR, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        info->shortname_length = 0;
        info->longname_length = entry->name_len;
        MI_CpuCopy8(entry->name, info->longname, info->longname_length);
        info->longname[info->longname_length] = '\0';
        if (entry->is_directory)
        {
            info->attributes = FS_ATTRIBUTE_IS_DIRECTORY;
            info->id = (u32)((entry->dir_id.own_id << 0) | (entry->dir_id.index << 16));
            info->filesize = 0;
        }
        else
        {
            info->attributes = 0;
            info->id = entry->file_id.file_id;
            info->filesize = 0;
            // �L���ȃt�@�C��ID�Ȃ�t�@�C���T�C�Y����FAT����擾����B
            {
                FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
                u32                     pos = (u32)(info->id * sizeof(FSArchiveFAT));
                if (pos < context->fat_size)
                {
                    FSArchiveFAT        fat;
                    FSiSyncReadParam    param;
                    param.arc = arc;
                    param.pos = context->fat + pos;
                    if (FSi_ReadTable(&param, &fat, sizeof(fat)) == FS_RESULT_SUCCESS)
                    {
                        info->filesize = (u32)(fat.bottom - fat.top);
                        // NTR���[�h�œ��쒆�Ȃ�TWL��p�̈悪�ǂݏo���Ȃ����Ƃ��t���O�ŋL�^����B
                        if (FSi_IsUnreadableRomOffset(arc, fat.top))
                        {
                            info->attributes |= FS_ATTRIBUTE_IS_OFFLINE;
                        }
                    }
                }
            }
        }
        info->mtime.year = 0;
        info->mtime.month = 0;
        info->mtime.day = 0;
        info->mtime.hour = 0;
        info->mtime.minute = 0;
        info->mtime.second = 0;
    }
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_FindPath

  Description:  FS_COMMAND_FINDPATH�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                base_dir_id         ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                path                �����p�X
                target_id           ID�i�[��
                target_is_directory �f�B���N�g�����ǂ����̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_FindPath(FSArchive *arc, u32 base_dir_id, const char *path, u32 *target_id, BOOL target_is_directory)
{
    FSResult        result;
    union
    {
        FSFileID    file;
        FSDirPos    dir;
    }
    id;
    FSFile  tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;
    tmp->arg.findpath.pos.arc = arc;
    tmp->arg.findpath.pos.own_id = (u16)(base_dir_id >> 0);
    tmp->arg.findpath.pos.index = 0;
    tmp->arg.findpath.pos.pos = 0;
    tmp->arg.findpath.path = path;
    tmp->arg.findpath.find_directory = target_is_directory;
    if (target_is_directory)
    {
        tmp->arg.findpath.result.dir = &id.dir;
    }
    else
    {
        tmp->arg.findpath.result.file = &id.file;
    }
    result = FSi_TranslateCommand(tmp, FS_COMMAND_FINDPATH, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        if (target_is_directory)
        {
            *target_id = id.dir.own_id;
        }
        else
        {
            *target_id = id.file.file_id;
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_GetPath

  Description:  FS_COMMAND_GETPATH�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                is_directory        file���t�@�C���Ȃ�FALSE�A�f�B���N�g���Ȃ�TRUE
                buffer              �p�X�i�[��
                length              �o�b�t�@�T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_GetPath(FSArchive *arc, FSFile *file, BOOL is_directory, char *buffer, u32 *length)
{
    FSResult                result;
    FSROMFATCommandInfo    *arg = &file->arg;
    arg->getpath.total_len = 0;
    arg->getpath.dir_id = 0;
    arg->getpath.buf = (u8 *)buffer;
    arg->getpath.buf_len = *length;
    result = FSi_TranslateCommand(file, FS_COMMAND_GETPATH, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        *length = arg->getpath.buf_len;
    }
    (void)arc;
    (void)is_directory;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_OpenFileFast

  Description:  FS_COMMAND_OPENFILEFAST�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                id                  �t�@�C��ID
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_OpenFileFast(FSArchive *arc, FSFile *file, u32 id, u32 mode)
{
    FSResult                result;
    FSROMFATCommandInfo    *arg = &file->arg;
    arg->openfilefast.id.arc = arc;
    arg->openfilefast.id.file_id = id;
    result = FSi_TranslateCommand(file, FS_COMMAND_OPENFILEFAST, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        FS_SetFileHandle(file, arc, &file->prop);
    }
    (void)mode;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_OpenFileDirect

  Description:  FS_COMMAND_OPENFILEDIRECT�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                top                 �t�@�C���擪�I�t�Z�b�g
                bottom              �t�@�C���I�[�I�t�Z�b�g
                id                  �v������t�@�C��ID����ь��ʂ̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_OpenFileDirect(FSArchive *arc, FSFile *file, u32 top, u32 bottom, u32 *id)
{
    FSResult                result;
    FSROMFATCommandInfo    *arg = &file->arg;
    arg->openfiledirect.top = top;
    arg->openfiledirect.bottom = bottom;
    arg->openfiledirect.index = *id;
    result = FSi_TranslateCommand(file, FS_COMMAND_OPENFILEDIRECT, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        FS_SetFileHandle(file, arc, &file->prop);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_CloseFile

  Description:  FS_COMMAND_CLOSEFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_CloseFile(FSArchive *arc, FSFile *file)
{
    FSResult            result;
    result = FSi_TranslateCommand(file, FS_COMMAND_CLOSEFILE, TRUE);
    FS_DetachHandle(file);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_Activate

  Description:  FS_COMMAND_ACTIVATE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ROMFAT_Activate(FSArchive* arc)
{
    FSFile  tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;
    (void)FSi_TranslateCommand(tmp, FS_COMMAND_ACTIVATE, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_Idle

  Description:  FS_COMMAND_ACTIVATE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ROMFAT_Idle(FSArchive* arc)
{
    FSFile  tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;
    (void)FSi_TranslateCommand(tmp, FS_COMMAND_IDLE, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_Suspend

  Description:  FS_COMMAND_SUSPEND�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ROMFAT_Suspend(FSArchive* arc)
{
    FSFile  tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;
    (void)FSi_TranslateCommand(tmp, FS_COMMAND_SUSPEND, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_Resume

  Description:  FS_COMMAND_RESUME�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ROMFAT_Resume(FSArchive* arc)
{
    FSFile  tmp[1];
    FS_InitFile(tmp);
    tmp->arc = arc;
    (void)FSi_TranslateCommand(tmp, FS_COMMAND_RESUME, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_OpenFile

  Description:  FS_COMMAND_OPENFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                baseid              ��_�f�B���N�g�� (���[�g�Ȃ�0)
                path                �t�@�C���p�X
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_OpenFile(FSArchive *arc, FSFile *file, u32 baseid, const char *path, u32 mode)
{
    FSResult    result;
    u32         fileid;
    result = FSi_ROMFAT_FindPath(arc, baseid, path, &fileid, FALSE);
    if (result == FS_RESULT_SUCCESS)
    {
        result = FSi_ROMFAT_OpenFileFast(arc, file, fileid, mode);
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_SeekFile

  Description:  FS_COMMAND_SEEKFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                offset              �ړ��ʂ���шړ���̈ʒu
                from                �V�[�N�N�_

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_SeekFile(FSArchive *arc, FSFile *file, int *offset, FSSeekFileMode from)
{
    FSROMFATProperty   *prop = (FSROMFATProperty*)FS_GetFileUserData(file);
    int pos = *offset;
    switch (from)
    {
    case FS_SEEK_SET:
        pos += prop->file.top;
        break;
    case FS_SEEK_CUR:
    default:
        pos += prop->file.pos;
        break;
    case FS_SEEK_END:
        pos += prop->file.bottom;
        break;
    }
    // �͈͊O�ւ̃V�[�N�����͎��s�Ƃ݂Ȃ��B
    if ((pos < (int)prop->file.top) || (pos > (int)prop->file.bottom))
    {
        return FS_RESULT_INVALID_PARAMETER;
    }
    else
    {
        prop->file.pos = (u32)pos;
        *offset = pos;
        (void)arc;
        return FS_RESULT_SUCCESS;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_GetFileLength

  Description:  FS_COMMAND_GETFILELENGTH�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                length              �擾�����T�C�Y�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_GetFileLength(FSArchive *arc, FSFile *file, u32 *length)
{
    FSROMFATProperty   *prop = (FSROMFATProperty*)FS_GetFileUserData(file);
    *length = prop->file.bottom - prop->file.top;
    (void)arc;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_GetFilePosition

  Description:  FS_COMMAND_GETFILEPOSITION�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                length              �擾�����ʒu�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_GetFilePosition(FSArchive *arc, FSFile *file, u32 *position)
{
    FSROMFATProperty   *prop = (FSROMFATProperty*)FS_GetFileUserData(file);
    *position = prop->file.pos - prop->file.top;
    (void)arc;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_Unmount

  Description:  FS_COMMAND_UNMOUNT�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_ROMFAT_Unmount(FSArchive *arc)
{
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    if (FS_IsArchiveTableLoaded(arc))
    {
        OS_TWarning("memory may leak. preloaded-table of archive \"%s\" (0x%08X)",
                    FS_GetArchiveName(arc), context->load_mem);
    }
    context->base = 0;
    context->fat = 0;
    context->fat_size = 0;
    context->fnt = 0;
    context->fnt_size = 0;
    context->fat_bak = 0;
    context->fnt_bak = 0;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_GetArchiveCaps

  Description:  FS_COMMAND_GETARCHIVECAPS�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                caps                �f�o�C�X�\�̓t���O�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_GetArchiveCaps(FSArchive *arc, u32 *caps)
{
    (void)arc;
    *caps = 0;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_OpenDirectory

  Description:  FS_COMMAND_OPENDIRECTORY�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                path                �p�X
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_OpenDirectory(FSArchive *arc, FSFile *file, u32 baseid, const char *path, u32 mode)
{
    FSResult    result = FS_RESULT_ERROR;
    u32         id = 0;
    result = FSi_ROMFAT_FindPath(arc, baseid, path, &id, TRUE);
    if (result == FS_RESULT_SUCCESS)
    {
        result = FSi_ROMFAT_SeekDirectory(arc, file, id, 0);
    }
    (void)mode;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_CloseDirectory

  Description:  FS_COMMAND_CLOSEDIRECTORY�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_CloseDirectory(FSArchive *arc, FSFile *file)
{
    FS_DetachHandle(file);
    (void)arc;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_GetPathInfo

  Description:  FS_COMMAND_GETPATHINFO�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                path                �p�X
                info                �t�@�C�����̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_GetPathInfo(FSArchive *arc, u32 baseid, const char *path, FSPathInfo *info)
{
    FSResult    result = FS_RESULT_ERROR;
    u32         id = 0;
    MI_CpuFill8(info, 0x00, sizeof(*info));
    // ���Ȃ�����������̂Ŋ֐���ʓr�؂�o�����ق����ǂ�
    if (FSi_ROMFAT_FindPath(arc, baseid, path, &id, TRUE) == FS_RESULT_SUCCESS)
    {
        info->attributes = FS_ATTRIBUTE_IS_DIRECTORY;
        info->id = id;
        result = FS_RESULT_SUCCESS;
    }
    else if (FSi_ROMFAT_FindPath(arc, baseid, path, &id, FALSE) == FS_RESULT_SUCCESS)
    {
        info->attributes = 0;
        info->id = id;
        info->filesize = 0;
        // �L���ȃt�@�C��ID�Ȃ�t�@�C���T�C�Y����FAT����擾����B
        {
            FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
            u32                     pos = (u32)(id * sizeof(FSArchiveFAT));
            if (pos < context->fat_size)
            {
                FSArchiveFAT        fat;
                FSiSyncReadParam    param;
                param.arc = arc;
                param.pos = context->fat + pos;
                if (FSi_ReadTable(&param, &fat, sizeof(fat)) == FS_RESULT_SUCCESS)
                {
                    info->filesize = (u32)(fat.bottom - fat.top);
                    // NTR���[�h�œ��쒆�Ȃ�TWL��p�̈悪�ǂݏo���Ȃ����Ƃ��t���O�ŋL�^����B
                    if (FSi_IsUnreadableRomOffset(arc, fat.top))
                    {
                        info->attributes |= FS_ATTRIBUTE_IS_OFFLINE;
                    }
                }
            }
        }
        result = FS_RESULT_SUCCESS;
    }
    // NitroROM�t�H�[�}�b�g�̃A�[�J�C�u�̓f�t�H���g�ŏ������݋֎~�B
    info->attributes |= FS_ATTRIBUTE_IS_PROTECTED;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ROMFAT_GetPathInfo

  Description:  FS_COMMAND_GETARCHIVERESOURCE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                resource            ���\�[�X���̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_ROMFAT_GetArchiveResource(FSArchive *arc, FSArchiveResource *resource)
{
    const CARDRomHeader    *header = (const CARDRomHeader *)CARD_GetRomHeader();
    resource->bytesPerSector = 0;
    resource->sectorsPerCluster = 0;
    resource->totalClusters = 0;
    resource->availableClusters = 0;
    resource->totalSize = header->rom_size;
    resource->availableSize = 0;
    resource->maxFileHandles = 0x7FFFFFFF;
    resource->currentFileHandles = 0;
    resource->maxDirectoryHandles = 0x7FFFFFFF;
    resource->currentDirectoryHandles = 0;
    (void)arc;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadSRLCallback

  Description:  SRL�t�@�C���ǂݍ��݃R�[���o�b�N

  Arguments:    p_arc            FSArchive�\����
                dst              �]����
                src              �]����
                len              �]���T�C�Y

  Returns:      �ǂݍ��ݏ����̌���
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadSRLCallback(FSArchive *arc, void *buffer, u32 offset, u32 length)
{
    CARDRomHashContext *hash = (CARDRomHashContext*)FS_GetArchiveBase(arc);
    CARD_ReadRomHashImage(hash, buffer, offset, length);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SRLArchiveProc

  Description:  SRL�t�@�C���A�[�J�C�u�̃v���V�[�W��

  Arguments:    file             �R�}���h�����i�[����FSFile�\����
                cmd              �R�}���h�^�C�v

  Returns:      �R�}���h��������
 *---------------------------------------------------------------------------*/
static FSResult FSi_SRLArchiveProc(FSFile *file, FSCommandType cmd)
{
    (void)file;
    switch (cmd)
    {
    case FS_COMMAND_WRITEFILE:
        return FS_RESULT_UNSUPPORTED;
    default:
        return FS_RESULT_PROC_UNKNOWN;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_MountSRLFile

  Description:  SRL�t�@�C���Ɋ܂܂��ROM�t�@�C���V�X�e�����}�E���g

  Arguments:    arc              �}�E���g�Ɏg�p����FSArchive�\���́B
                                 ���O�͓o�^�ς݂łȂ���΂Ȃ�Ȃ��B
                file             ���łɃI�[�v������Ă���}�E���g�Ώۃt�@�C���B
                                 �}�E���g���͂��̍\���̂�j�����Ă͂Ȃ�Ȃ��B
                hash             �n�b�V���R���e�L�X�g�\���́B

  Returns:      ���������������TRUE
 *---------------------------------------------------------------------------*/
BOOL FSi_MountSRLFile(FSArchive *arc, FSFile *file, CARDRomHashContext *hash)
{
    BOOL                    retval = FALSE;
    static CARDRomHeaderTWL header[1] ATTRIBUTE_ALIGN(32);
    if (file &&
        (FS_SeekFileToBegin(file) &&
        (FS_ReadFile(file, header, sizeof(header)) == sizeof(header))))
    {
        // �Z�L�����e�B�̂���NAND�A�v���̓n�b�V���e�[�u���K�{�Ƃ���B
        // �����I�ɁuNITRO�݊����[�h�����t�@�C���V�X�e�����g�p�������v
        // �Ƃ���ROM���o�ꂵ���炻�̎��ɉ^�p���j����������B
        if ((((const u8 *)header)[0x1C] & 0x01) != 0)
        {
            FSResult (*proc)(FSFile*, FSCommandType) = FSi_SRLArchiveProc;
            FSResult (*read)(FSArchive*, void*, u32, u32) = FSi_ReadSRLCallback;
            FSResult (*write)(FSArchive*, const void*, u32, u32) = NULL;
            FS_SetArchiveProc(arc, proc, FS_ARCHIVE_PROC_WRITEFILE);
            // arc -> hash -> file �̏��ɃA�N�Z�X�`�F�[�����`�������B
            if (FS_LoadArchive(arc, (u32)hash,
                               header->ntr.fat.offset, header->ntr.fat.length,
                               header->ntr.fnt.offset, header->ntr.fnt.length,
                               read, write))
            {
                // �V�X�e�����[�N�̈��ROM�w�b�_�ƈقȂ���e�������ꍇ
                // CARDi_InitRom�֐��Ŏ��O�Ɋm�ۂ��Ă����o�b�t�@�͖��ʂɂȂ�B
                extern u8  *CARDiHashBufferAddress;
                extern u32  CARDiHashBufferLength;
                u32         len = CARD_CalcRomHashBufferLength(header);
                if (len > CARDiHashBufferLength)
                {
                    u8     *lo = (u8 *)MATH_ROUNDUP((u32)OS_GetMainArenaLo(), 32);
                    u8     *hi = (u8 *)MATH_ROUNDDOWN((u32)OS_GetMainArenaHi(), 32);
                    if (&lo[len] > hi)
                    {
                        OS_TPanic("cannot allocate memory for ROM-hash from ARENA");
                    }
                    else
                    {
                        OS_SetMainArenaLo(&lo[len]);
                        CARDiHashBufferAddress = lo;
                        CARDiHashBufferLength = len;
                    }
                }
                CARD_InitRomHashContext(hash, header,
                                        CARDiHashBufferAddress, CARDiHashBufferLength,
                                        FSi_SeekAndReadSRL, NULL, file);
                // �����o�b�t�@���������Ďg�p����Ȃ��悤�|�C���^��j���B
                CARDiHashBufferAddress = NULL;
                CARDiHashBufferLength = 0;
                retval = TRUE;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadMemCallback

  Description:  �f�t�H���g�̃��������[�h�R�[���o�b�N�B

  Arguments:    p_arc            ���삷��A�[�J�C�u
                dst              �ǂݍ��ރ������̊i�[��
                pos              �ǂݍ��݈ʒu
                size             �ǂݍ��݃T�C�Y

  Returns:      ��� FS_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadMemCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
    MI_CpuCopy8((const void *)FS_GetArchiveOffset(p_arc, pos), dst, size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WriteMemCallback

  Description:  �f�t�H���g�̃��������C�g�R�[���o�b�N

  Arguments:    p_arc            ���삷��A�[�J�C�u
                dst              �������ރ������̎Q�Ɛ�
                pos              �������݈ʒu
                size             �������݃T�C�Y

  Returns:      ��� FS_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
static FSResult FSi_WriteMemCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size)
{
    MI_CpuCopy8(src, (void *)FS_GetArchiveOffset(p_arc, pos), size);
    return FS_RESULT_SUCCESS;
}

static const FSArchiveInterface FSiArchiveProcInterface =
{
    // ���d�l�ƌ݊����̂���R�}���h
    FSi_ROMFAT_ReadFile,
    FSi_ROMFAT_WriteFile,
    FSi_ROMFAT_SeekDirectory,
    FSi_ROMFAT_ReadDirectory,
    FSi_ROMFAT_FindPath,
    FSi_ROMFAT_GetPath,
    FSi_ROMFAT_OpenFileFast,
    FSi_ROMFAT_OpenFileDirect,
    FSi_ROMFAT_CloseFile,
    FSi_ROMFAT_Activate,
    FSi_ROMFAT_Idle,
    FSi_ROMFAT_Suspend,
    FSi_ROMFAT_Resume,
    // ���d�l�ƌ݊��������邪�R�}���h�ł͂Ȃ���������
    FSi_ROMFAT_OpenFile,
    FSi_ROMFAT_SeekFile,
    FSi_ROMFAT_GetFileLength,
    FSi_ROMFAT_GetFilePosition,
    // �V�d�l�Ŋg�����ꂽ�R�}���h (NULL�Ȃ�UNSUPPORTED)
    NULL,               // Mount
    FSi_ROMFAT_Unmount,
    FSi_ROMFAT_GetArchiveCaps,
    NULL,               // CreateFile
    NULL,               // DeleteFile
    NULL,               // RenameFile
    FSi_ROMFAT_GetPathInfo,
    NULL,               // SetFileInfo
    NULL,               // CreateDirectory
    NULL,               // DeleteDirectory
    NULL,               // RenameDirectory
    FSi_ROMFAT_GetArchiveResource,
    NULL,               // 29UL
    NULL,               // FlushFile
    NULL,               // SetFileLength
    FSi_ROMFAT_OpenDirectory,
    FSi_ROMFAT_CloseDirectory,
};

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchive

  Description:  �A�[�J�C�u���t�@�C���V�X�e���Ƀ��[�h����B
                ���łɃA�[�J�C�u���X�g�ɖ��O���o�^����Ă���K�v������B

  Arguments:    arc              ���[�h����A�[�J�C�u
                base             �Ǝ��Ɏg�p�\�ȔC�ӂ� u32 �l
                fat              FAT �e�[�u���̐擪�I�t�Z�b�g
                fat_size         FAT �e�[�u���̃T�C�Y
                fnt              FNT �e�[�u���̐擪�I�t�Z�b�g
                fnt_size         FNT �e�[�u���̃T�C�Y
                read_func        ���[�h�A�N�Z�X�R�[���o�b�N
                write_func       ���C�g�A�N�Z�X�R�[���o�b�N

  Returns:      �A�[�J�C�u�����������[�h������ TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_LoadArchive(FSArchive *arc, u32 base,
                    u32 fat, u32 fat_size,
                    u32 fnt, u32 fnt_size,
                    FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func)
{
    BOOL    retval = FALSE;
    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(arc);
    SDK_ASSERT(!FS_IsArchiveLoaded(arc));

    // �A�[�J�C�u�p�̏���������
    {
        FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)arc->reserved2;
        context->base = base;
        context->fat_size = fat_size;
        context->fat = fat;
        context->fat_bak = fat;
        context->fnt_size = fnt_size;
        context->fnt = fnt;
        context->fnt_bak = fnt;
        context->read_func = read_func ? read_func : FSi_ReadMemCallback;
        context->write_func = write_func ? write_func : FSi_WriteMemCallback;
        context->load_mem = NULL;
        return FS_MountArchive(arc, context, &FSiArchiveProcInterface, 0);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchive

  Description:  �A�[�J�C�u���t�@�C���V�X�e������A�����[�h����B
                ���ݏ������̃^�X�N���S�Ċ�������܂Ńu���b�L���O����B

  Arguments:    arc              �A�����[�h����A�[�J�C�u

  Returns:      �A�[�J�C�u���������A�����[�h������ TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_UnloadArchive(FSArchive *arc)
{
    return FS_UnmountArchive(arc);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetFileLengthIfProc

  Description:  �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���΃T�C�Y���擾

  Arguments:    file             �t�@�C���n���h��
                length           �T�C�Y�̊i�[��

  Returns:      �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���΂��̃T�C�Y
 *---------------------------------------------------------------------------*/
BOOL FSi_GetFileLengthIfProc(FSFile *file, u32 *length)
{
    return (file->arc->vtbl == &FSiArchiveProcInterface) &&
           (FSi_ROMFAT_GetFileLength(file->arc, file, length) == FS_RESULT_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetFilePositionIfProc

  Description:  �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���Ό��݈ʒu���擾

  Arguments:    file             �t�@�C���n���h��
                length           �T�C�Y�̊i�[��

  Returns:      �w�肳�ꂽ�t�@�C�����A�[�J�C�u�v���V�[�W���ł���΂��̌��݈ʒu
 *---------------------------------------------------------------------------*/
BOOL FSi_GetFilePositionIfProc(FSFile *file, u32 *length)
{
    return (file->arc->vtbl == &FSiArchiveProcInterface) &&
           (FSi_ROMFAT_GetFilePosition(file->arc, file, length) == FS_RESULT_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetArchiveProc

  Description:  �A�[�J�C�u�̃��[�U�v���V�[�W����ݒ肷��B
                proc == NULL �܂��� flags = 0 �Ȃ�
                �P�Ƀ��[�U�v���V�[�W���𖳌��ɂ���B

  Arguments:    arc              ���[�U�v���V�[�W����ݒ肷��A�[�J�C�u
                proc             ���[�U�v���V�[�W��
                flags            �v���V�[�W���փt�b�N����R�}���h�̃r�b�g�W��

  Returns:      ��ɍ��v�̃e�[�u���T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
void FS_SetArchiveProc(FSArchive *arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags)
{
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)arc->reserved2;
    if (!flags)
    {
        proc = NULL;
    }
    else if (!proc)
    {
        flags = 0;
    }
    context->proc = proc;
    context->proc_flag = flags;
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchiveTables

  Description:  �A�[�J�C�u�� FAT + FNT ����������Ƀv�����[�h����B
                �w��T�C�Y�ȓ��̏ꍇ�̂ݓǂݍ��݂����s��, �K�v�T�C�Y��Ԃ��B

  Arguments:    p_arc            �e�[�u�����v�����[�h����A�[�J�C�u
                p_mem            �e�[�u���f�[�^�̊i�[��o�b�t�@
                max_size         p_mem �̃T�C�Y

  Returns:      ���, ���v�̃e�[�u���T�C�Y��Ԃ�
 *---------------------------------------------------------------------------*/
u32 FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size)
{
    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(p_arc);

    // ���łɃ��[�h�ς݂̃e�[�u��������΃A�����[�h����B
    if ((p_mem != NULL) && FS_IsArchiveTableLoaded(p_arc))
    {
        (void)FS_UnloadArchiveTables(p_arc);
    }

    {
        FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(p_arc);
        // �v�����[�h�T�C�Y�͑O�� 32 BYTE �A���C��
        u32     total_size = (u32)((context->fat_size + context->fnt_size + 32 + 31) & ~31);
        if (total_size <= max_size)
        {
            // �T�C�Y���[���Ȃ烁�����փ��[�h
            u8     *p_cache = (u8 *)(((u32)p_mem + 31) & ~31);
            FSFile  tmp;
            FS_InitFile(&tmp);
            // �e�[�u���̓��[�h�ł��Ȃ����Ƃ�����B
            // ���̏ꍇ��, ���X�e�[�u���ɃA�N�Z�X�ł��Ȃ��̂ŉ������Ȃ��B
            if (FS_OpenFileDirect(&tmp, p_arc, context->fat, context->fat + context->fat_size, (u32)~0))
            {
                if (FS_ReadFile(&tmp, p_cache, (s32)context->fat_size) < 0)
                {
                    MI_CpuFill8(p_cache, 0x00, context->fat_size);
                }
                (void)FS_CloseFile(&tmp);
            }
            context->fat = (u32)p_cache;
            p_cache += context->fat_size;
            if (FS_OpenFileDirect(&tmp, p_arc, context->fnt, context->fnt + context->fnt_size, (u32)~0))
            {
                if (FS_ReadFile(&tmp, p_cache, (s32)context->fnt_size) < 0)
                {
                    MI_CpuFill8(p_cache, 0x00, context->fnt_size);
                }
                (void)FS_CloseFile(&tmp);
            }
            context->fnt = (u32)p_cache;
            // �ȍ~�̓e�[�u�����[�h�n�Ńv�����[�h�������������B
            context->load_mem = p_mem;
            p_arc->flag |= FS_ARCHIVE_FLAG_TABLE_LOAD;
        }
        return total_size;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchiveTables

  Description:  �A�[�J�C�u�̃v�����[�h�����������

  Arguments:    arc              �v�����[�h���������������A�[�J�C�u

  Returns:      �v�����[�h�������Ƃ��ă��[�U����^�����Ă����o�b�t�@
 *---------------------------------------------------------------------------*/
void   *FS_UnloadArchiveTables(FSArchive *arc)
{
    void   *retval = NULL;

    SDK_ASSERT(FS_IsAvailable());
    SDK_NULL_ASSERT(arc);

    if (FS_IsArchiveLoaded(arc))
    {
        FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
        BOOL    bak_stat = FS_SuspendArchive(arc);
        if (FS_IsArchiveTableLoaded(arc))
        {
            arc->flag &= ~FS_ARCHIVE_FLAG_TABLE_LOAD;
            retval = context->load_mem;
            context->fat = context->fat_bak;
            context->fnt = context->fnt_bak;
            context->load_mem = NULL;
        }
        if (bak_stat)
        {
            (void)FS_ResumeArchive(arc);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveBase

  Description:  ROMFAT�^�A�[�J�C�u�̃x�[�X�I�t�Z�b�g���擾

  Arguments:    arc              ROMFAT�^�A�[�J�C�u

  Returns:      ROMFAT�^�A�[�J�C�u�̃x�[�X�I�t�Z�b�g
 *---------------------------------------------------------------------------*/
u32 FS_GetArchiveBase(const struct FSArchive *arc)
{
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    return context->base;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveFAT

  Description:  ROMFAT�^�A�[�J�C�u��FAT�I�t�Z�b�g���擾

  Arguments:    arc              ROMFAT�^�A�[�J�C�u

  Returns:      ROMFAT�^�A�[�J�C�u��FAT�I�t�Z�b�g
 *---------------------------------------------------------------------------*/
u32 FS_GetArchiveFAT(const struct FSArchive *arc)
{
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    return context->fat;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveFNT

  Description:  ROMFAT�^�A�[�J�C�u��FNT�I�t�Z�b�g���擾

  Arguments:    arc              ROMFAT�^�A�[�J�C�u

  Returns:      ROMFAT�^�A�[�J�C�u��FNT�I�t�Z�b�g
 *---------------------------------------------------------------------------*/
u32 FS_GetArchiveFNT(const struct FSArchive *arc)
{
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    return context->fnt;
}

/* �A�[�J�C�u�̃x�[�X����̎w��ʒu�I�t�Z�b�g���擾 */
/*---------------------------------------------------------------------------*
  Name:         FS_GetArchiveOffset

  Description:  ROMFAT�^�A�[�J�C�u�̃x�[�X����̎w��ʒu�I�t�Z�b�g���擾

  Arguments:    arc              ROMFAT�^�A�[�J�C�u

  Returns:      �x�[�X�����Z�����w��ʒu�I�t�Z�b�g
 *---------------------------------------------------------------------------*/
u32 FS_GetArchiveOffset(const struct FSArchive *arc, u32 pos)
{
    FSROMFATArchiveContext *context = (FSROMFATArchiveContext*)FS_GetArchiveUserData(arc);
    return context->base + pos;
}

/*---------------------------------------------------------------------------*
  Name:         FS_IsArchiveTableLoaded

  Description:  ROMFAT�^�A�[�J�C�u�����݃e�[�u�����v�����[�h�ς݂�����

  Arguments:    arc              ROMFAT�^�A�[�J�C�u

  Returns:      �e�[�u�����v�����[�h�ς݂Ȃ�TRUE
 *---------------------------------------------------------------------------*/
BOOL FS_IsArchiveTableLoaded(volatile const struct FSArchive *arc)
{
    return ((arc->flag & FS_ARCHIVE_FLAG_TABLE_LOAD) != 0);
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileImageTop

  Description:  ROMFAT�^�A�[�J�C�u����I�[�v�������t�@�C����
                �擪�I�t�Z�b�g���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �A�[�J�C�u��ł̃t�@�C���擪�I�t�Z�b�g
 *---------------------------------------------------------------------------*/
u32 FS_GetFileImageTop(const struct FSFile *file)
{
    return file->prop.file.top;
}

/*---------------------------------------------------------------------------*
  Name:         FS_GetFileImageBottom

  Description:  ROMFAT�^�A�[�J�C�u����I�[�v�������t�@�C����
                �I�[�I�t�Z�b�g���擾

  Arguments:    file        �t�@�C���n���h��

  Returns:      �A�[�J�C�u��ł̃t�@�C���I�[�I�t�Z�b�g
 *---------------------------------------------------------------------------*/
u32 FS_GetFileImageBottom(const struct FSFile *file)
{
    return file->prop.file.bottom;
}


#endif /* FS_IMPLEMENT */
