/*---------------------------------------------------------------------------*
  Project:  TwlSDK - FS - libraries
  File:     fs_archive_fatfs.c

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

#ifdef SDK_TWL

#include <nitro/fs.h>
#include <nitro/os/ARM9/tcm.h>
#include <nitro/os/ARM9/cache.h>
#include <nitro/math/math.h>
#include <nitro/std/string.h>
#include <nitro/gx/gx_vramcnt.h>
#include <nitro/spi/ARM9/pm.h>
#include <twl/fatfs.h>
#include <twl/mi/common/sharedWram.h>

#include "../include/util.h"
#include "../include/command.h"
#include "../include/rom.h"


#if defined(FS_IMPLEMENT)


#include <twl/ltdmain_begin.h>


/*---------------------------------------------------------------------------*/
/* constants */

// �p�X�̃G���g�����Ɏg�p�ł��Ȃ������Q�B
extern const u16  *FSiPathInvalidCharactersW;
const u16  *FSiPathInvalidCharactersW = L":*?\"<>|";


/*---------------------------------------------------------------------------*/
/* variables */

static int                      FSiSwitchableWramSlots = 0;

static FSFATFSArchiveContext    FSiFATFSDriveDefault[FS_MOUNTDRIVE_MAX] ATTRIBUTE_ALIGN(32);
static FATFSRequestBuffer       FSiFATFSAsyncRequestDefault[FS_MOUNTDRIVE_MAX] ATTRIBUTE_ALIGN(32);
static u8                       FSiTemporaryBufferDefault[FS_TEMPORARY_BUFFER_MAX] ATTRIBUTE_ALIGN(32);


/*---------------------------------------------------------------------------*/
/* function */

/*---------------------------------------------------------------------------*
 * �J�X�^�}�C�Y�Ɋւ������
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         FSiFATFSDrive

  Description:  FAT�x�[�X�̃A�[�J�C�u���Ǘ�����R���e�L�X�g�z��B
                �ʏ��FS_MOUNTDRIVE_MAX�����̔z��T�C�Y��K�v�Ƃ��A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                �I�[�o�[���C�h����FSi_SetupFATBuffers�֐��̌Ăяo��������
                ���̕ϐ���ύX���ēK�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FSFATFSArchiveContext�\����FS_MOUNTDRIVE_MAX���̃o�b�t�@
 *---------------------------------------------------------------------------*/
FSFATFSArchiveContext *FSiFATFSDrive = NULL;

/*---------------------------------------------------------------------------*
  Name:         FSiFATFSAsyncRequest

  Description:  FAT�x�[�X�̃A�[�J�C�u�Ŕ񓯊������Ɏg�p����R�}���h�o�b�t�@�B
                �ʏ��FS_MOUNTDRIVE_MAX�����̔z��T�C�Y��K�v�Ƃ��A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                �I�[�o�[���C�h����FSi_SetupFATBuffers�֐��̌Ăяo��������
                ���̕ϐ���ύX���ēK�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FS_TEMPORARY_BUFFER_MAX�o�C�g�̐ÓI�ȃR�}���h�o�b�t�@
 *---------------------------------------------------------------------------*/
FATFSRequestBuffer *FSiFATFSAsyncRequest = NULL;

/*---------------------------------------------------------------------------*
  Name:         FSiTemporaryBuffer

  Description:  Read/Write�R�}���h���s�p�̐ÓI�Ȉꎞ�o�b�t�@�ւ̃|�C���^�B
                ARM9/ARM7�Ƃ��ɎQ�Ɖ\�ȃ������ł���K�v������A
                �f�t�H���g�ł�LTDMAIN�Z�O�����g���̐ÓI�ϐ����g�p�����B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̕ϐ���FS_Init�֐��̌Ăяo�����O�ɕύX���邱�Ƃ�
                �K�؂ȃo�b�t�@��ݒ肷�邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      FS_TEMPORARY_BUFFER_MAX�o�C�g�̐ÓI�ȃR�}���h�o�b�t�@
 *---------------------------------------------------------------------------*/
u8 *FSiTemporaryBuffer = NULL;

/*---------------------------------------------------------------------------*
  Name:         FSi_SetupFATBuffers

  Description:  FAT�x�[�X�̃A�[�J�C�u�ɕK�v�Ȋe��o�b�t�@���������B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̊֐����I�[�o�[���C�h���Ċe��o�b�t�@��Ǝ��ɐݒ肵
                ���v�������T�C�Y��K�v�Œ���ɗ}�~���邱�Ƃ��ł���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void FSi_SetupFATBuffers(void)
{
    FSiFATFSDrive = FSiFATFSDriveDefault;
    FSiFATFSAsyncRequest = FSiFATFSAsyncRequestDefault;
    FSiTemporaryBuffer = FSiTemporaryBufferDefault;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_IsValidAddressForARM7

  Description:  ARM7����A�N�Z�X�\�ȃo�b�t�@�����肷��B
                ����ȃ������z�u�ō\�z���ꂽ�A�v���P�[�V�����ł�
                ���̊֐����I�[�o�[���C�h���邱�Ƃ�
                �K�؂Ȕ��胋�[�`�����Ē�`���邱�Ƃ��ł���B

  Arguments:    buffer              ���肷�ׂ��o�b�t�@
                length              �o�b�t�@��

  Returns:      ARM7����A�N�Z�X�\�ȃo�b�t�@�ł����TRUE
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL BOOL FSi_IsValidAddressForARM7(const void *buffer, u32 length)
 __attribute__((never_inline))
{
    u32     addr = (u32)buffer;
    u32     dtcm = OS_GetDTCMAddress();
    if ((addr + length > dtcm) && (addr < dtcm + HW_DTCM_SIZE))
    {
        return FALSE;
    }
    if ((addr >= HW_TWL_MAIN_MEM) && (addr + length <= HW_TWL_MAIN_MEM_END))
    {
        return TRUE;
    }
    if ((addr >= HW_EXT_WRAM_ARM7) && (addr + length <= GX_GetSizeOfARM7()))
    {
        return TRUE;
    }
    // �X���b�g���E���܂����Ȃ��؂�ւ��\��WRAM�Ȃ狖�B
    if ((addr >= HW_WRAM_AREA) && (addr <= HW_WRAM_AREA_END) &&
        (MATH_ROUNDDOWN(addr, MI_WRAM_B_SLOT_SIZE) + MI_WRAM_B_SLOT_SIZE ==
         MATH_ROUNDUP(addr + length, MI_WRAM_B_SLOT_SIZE)))
    {
        MIWramPos   type;
        int         slot = MIi_AddressToWramSlot(buffer, &type);
        if (slot >= 0)
        {
            if ((((FSiSwitchableWramSlots >> ((type == MI_WRAM_B) ? 0 : 8)) & (1 << slot)) != 0) &&
                // ����ɐ؂�ւ��ĕK��������������Ƃ͌���Ȃ��ꍇ�ɂ̓A�v���P�[�V�������ۏ؂���B
                ((MI_GetWramBankMaster(type, slot)  == MI_WRAM_ARM7) ||
                 (MI_SwitchWramSlot(type, slot, MI_WRAM_SIZE_32KB,
                                    MI_GetWramBankMaster(type, slot), MI_WRAM_ARM7) >= 0)))
            {
                
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SetSwitchableWramSlots

  Description:  �󋵂ɉ�����FS���C�u������ARM7�֐؂�ւ��Ă悢WRAM�X���b�g���w��B

  Arguments:    bitsB               WRAM-B�X���b�g�̃r�b�g�W��
                bitsC               WRAM-C�X���b�g�̃r�b�g�W��

  Returns:      None
 *---------------------------------------------------------------------------*/
void FSi_SetSwitchableWramSlots(int bitsB, int bitsC)
{
    FSiSwitchableWramSlots = (bitsB << 0) | (bitsC << 8);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_AllocUnicodeFullPath

  Description:  �A�[�J�C�u���Ƒ��΃p�X��A�������t���p�X���擾�B

  Arguments:    context             FSFATFSArchiveContext�\����
                relpath             �A�[�J�C�u�ȉ��̑��΃p�X

  Returns:      �A�[�J�C�u������n�܂�Unicode�̃t���p�X���i�[�����o�b�t�@�B
 *---------------------------------------------------------------------------*/
static u16* FSi_AllocUnicodeFullPath(FSFATFSArchiveContext *context, const char *relpath)
{
    u16        *dst = FSi_GetUnicodeBuffer(NULL);
    {
        int     pos = 0;
        int     dstlen;
        // �܂��A�[�J�C�u���Ƒ��΃p�X��A���B
        dstlen = FS_ARCHIVE_FULLPATH_MAX - pos - 2;
        (void)FSi_ConvertStringSjisToUnicode(&dst[pos], &dstlen, FS_GetArchiveName(context->arc), NULL, NULL);
        pos += dstlen;
        dst[pos++] = L':';
        dst[pos++] = L'/';
        dstlen = FS_ARCHIVE_FULLPATH_MAX - pos;
        (void)FSi_ConvertStringSjisToUnicode(&dst[pos], &dstlen, relpath, NULL, NULL);
        pos += dstlen;
        // �I�[�ɕt�^���ꂽ'/'������Ώ����B
        if ((pos > 0) && ((dst[pos - 1] == L'\\') || (dst[pos - 1] == L'/')))
        {
            --pos;
        }
        dst[pos] = L'\0';
    }
    return dst;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_DostimeToFstime

  Description:  FAT��DOS�^�C���X�^���v��FSDateTime�`���ɕϊ��B

  Arguments:    dst                 �ϊ����ʂ��i�[����FSDateTime�\����
                src                 �ϊ����ƂȂ�DOS�^�C���X�^���v

  Returns:      None
 *---------------------------------------------------------------------------*/
static void FSi_DostimeToFstime(FSDateTime *dst, u32 src)
{
    dst->year = FATFS_DOSTIME_TO_YEAR(src);
    dst->month = FATFS_DOSTIME_TO_MON(src);
    dst->day = FATFS_DOSTIME_TO_MDAY(src);
    dst->hour = FATFS_DOSTIME_TO_HOUR(src);
    dst->minute = FATFS_DOSTIME_TO_MIN(src);
    dst->second = FATFS_DOSTIME_TO_SEC(src);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FstimeToDostime

  Description:  FSDateTime��FAT��DOS�^�C���X�^���v�`���ɕϊ��B

  Arguments:    dst                 �ϊ����ʂ��i�[����DOS�^�C���X�^���v
                src                 �ϊ����ƂȂ�FSDateTime�\����

  Returns:      None
 *---------------------------------------------------------------------------*/
static void FSi_FstimeToDostime(u32 *dst, const FSDateTime *src)
{
    *dst = FATFS_DATETIME_TO_DOSTIME(src->year, src->month, src->day,
                                     src->hour, src->minute, src->second);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_CheckFstime

  Description:  FSDateTime���`�F�b�N����

  Arguments:    src                 FSDateTime�\����

  Returns:      TRUE / FALSE
 *---------------------------------------------------------------------------*/
static BOOL FSi_CheckFstime( const FSDateTime *fstime)
{
    if( fstime->month / 13 != 0) { return FALSE;}
//    if( fstime->day   / 32 != 0) { return FALSE;}
    if( fstime->hour  / 24 != 0) { return FALSE;}
    if( fstime->minute/ 60 != 0) { return FALSE;}
    if( fstime->second/ 61 != 0) { return FALSE;}
    return( TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetUnicodeSpanExcluding

  Description:  �w�肳�ꂽ�����Q���܂܂Ȃ������񒷂��擾�B

  Arguments:    src : �����Ώە�����
                pattern : �������ׂ������Q

  Returns:      �w�肳�ꂽ�����Q���܂܂Ȃ�������
 *---------------------------------------------------------------------------*/
static int FSi_GetUnicodeSpanExcluding(const u16 *src, const u16 *pattern)
{
    int     pos = 0;
    BOOL    found = FALSE;
    for (; src[pos]; ++pos)
    {
        int     i;
        for (i = 0; pattern[i]; ++i)
        {
            if (src[pos] == pattern[i])
            {
                found = TRUE;
                break;
            }
        }
        if (found)
        {
            break;
        }
    }
    return pos;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_UsingInvalidCharacterW

  Description:  �p�X���Ƃ��ĕs�K�؂ȕ������g�p���Ă��邩����B
                ������ "arcname:/<***>" �̌`���͓���p�X�Ƃ��ċ��B

  Arguments:    path : �����ΏۂƂȂ�t���p�X������

  Returns:      �s�K�؂ȕ������g�p���Ă�����TRUE
 *---------------------------------------------------------------------------*/
static BOOL FSi_UsingInvalidCharacterW(const u16 *path)
{
    BOOL        retval = FALSE;
    const u16  *list = FSiPathInvalidCharactersW;
    if (list && *list)
    {
        BOOL    foundLT = FALSE;
        int     pos = 0;
        // �A�[�J�C�u�����X�L�b�v�B
        while (path[pos] && (path[pos] != L':'))
        {
            ++pos;
        }
        // �A�[�J�C�u��؂���X�L�b�v�B
        pos += (path[pos] == L':');
        pos += (path[pos] == L'/');
        // ����p�X���l�����֎~�����̌����B
        if (path[pos] == L'<')
        {
            foundLT = TRUE;
            ++pos;
        }
        // �֎~�������g�p���Ă��Ȃ����m�F�B
        while (path[pos])
        {
            pos += FSi_GetUnicodeSpanExcluding(&path[pos], list);
            if (path[pos])
            {
                if (foundLT && (path[pos] == L'>') && (path[pos + 1] == L'\0'))
                {
                    foundLT = FALSE;
                    pos += 1;
                }
                else
                {
                    retval = TRUE;
                    break;
                }
            }
        }
        retval |= foundLT;
        // �֎~�������g�p���Ă�����x���B
        if (retval)
        {
            static BOOL logOnce = FALSE;
            if (!logOnce)
            {
                OS_TWarning("specified path includes invalid character '%c'\n", (char)path[pos]);
                logOnce = TRUE;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ConvertError

  Description:  FATFS���C�u�����̃G���[�R�[�h��FS���C�u�����̃G���[�R�[�h�֕ϊ��B

  Arguments:    error               FATFS���C�u�����̃G���[�R�[�h

  Returns:      FS���C�u�����̃G���[�R�[�h
 *---------------------------------------------------------------------------*/
FSResult FSi_ConvertError(u32 error)
{
    if (error == FATFS_RESULT_SUCCESS)
    {
        return FS_RESULT_SUCCESS;
    }
    else if (error == FATFS_RESULT_BUSY)
    {
        return FS_RESULT_BUSY;
    }
    else if (error == FATFS_RESULT_FAILURE)
    {
        return FS_RESULT_FAILURE;
    }
    else if (error == FATFS_RESULT_UNSUPPORTED)
    {
        return FS_RESULT_UNSUPPORTED;
    }
    else if (error == FATFS_RESULT_INVALIDPARAM)
    {
        return FS_RESULT_INVALID_PARAMETER;
    }
    else if (error == FATFS_RESULT_ALREADYDONE)
    {
        return FS_RESULT_ALREADY_DONE;
    }
    else if (error == FATFS_RESULT_PERMISSIONDENIDED)
    {
        return FS_RESULT_PERMISSION_DENIED;
    }
    else if (error == FATFS_RESULT_NOMORERESOURCE)
    {
        return FS_RESULT_NO_MORE_RESOURCE;
    }
    else if (error == FATFS_RESULT_MEDIAFATAL)
    {
        return FS_RESULT_MEDIA_FATAL;
    }
    else if (error == FATFS_RESULT_NOENTRY)
    {
        return FS_RESULT_NO_ENTRY;
    }
    else if (error == FATFS_RESULT_MEDIANOTHING)
    {
        return FS_RESULT_MEDIA_NOTHING;
    }
    else if (error == FATFS_RESULT_MEDIAUNKNOWN)
    {
        return FS_RESULT_MEDIA_UNKNOWN;
    }
    else if (error == FATFS_RESULT_BADFORMAT)
    {
        return FS_RESULT_BAD_FORMAT;
    }
    else if (error == FATFS_RESULT_CANCELED)
    {
        return FS_RESULT_CANCELED;
    }
    else
    {
        return FS_RESULT_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFSAsyncDone

  Description:  FATFS�񓯊����������ʒm

  Arguments:    buffer           ���N�G�X�g�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_FATFSAsyncDone(FATFSRequestBuffer *buffer)
{
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)buffer->userdata;
    FS_NotifyArchiveAsyncEnd(context->arc, FSi_ConvertError(buffer->header.result));
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_GetArchiveCaps

  Description:  FS_COMMAND_GETARCHIVECAPS�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                caps                �f�o�C�X�\�̓t���O�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_GetArchiveCaps(FSArchive *arc, u32 *caps)
{
    (void)arc;
    *caps = FS_ARCHIVE_CAPS_UNICODE;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_GetPathInfo

  Description:  FS_COMMAND_GETPATHINFO�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                relpath             �p�X
                info                �t�@�C�����̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_GetPathInfo(FSArchive *arc, u32 baseid, const char *relpath, FSPathInfo *info)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    FATFSFileInfoW          stat[1];
    u16                    *path = FSi_AllocUnicodeFullPath(context, relpath);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    (void)baseid;
    if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_GetFileInfoW(path, stat))
    {
        info->attributes = stat->attributes;
        if ((stat->attributes & FATFS_ATTRIBUTE_DOS_DIRECTORY) != 0)
        {
            info->attributes |= FS_ATTRIBUTE_IS_DIRECTORY;
        }
        info->filesize = stat->length;
        FSi_DostimeToFstime(&info->atime, stat->dos_atime);
        FSi_DostimeToFstime(&info->mtime, stat->dos_mtime);
        FSi_DostimeToFstime(&info->ctime, stat->dos_ctime);
        info->id = FS_INVALID_FILE_ID;
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_SetPathInfo

  Description:  FS_COMMAND_SETPATHINFO�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                relpath             �p�X
                info                �t�@�C�����̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_SetPathInfo(FSArchive *arc, u32 baseid, const char *relpath, FSPathInfo *info)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    FATFSFileInfoW          stat[1];
    u16                    *path = FSi_AllocUnicodeFullPath(context, relpath);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    (void)baseid;
    FSi_FstimeToDostime(&stat->dos_atime, &info->atime);
    FSi_FstimeToDostime(&stat->dos_mtime, &info->mtime);
    FSi_FstimeToDostime(&stat->dos_ctime, &info->ctime);
    stat->attributes = (info->attributes & (FATFS_ATTRIBUTE_DOS_MASK | FATFS_PROPERTY_CTRL_MASK));
    info->attributes &= ~FATFS_PROPERTY_CTRL_MASK;
    if (((stat->attributes & FATFS_PROPERTY_CTRL_MASK) != 0) && !FSi_CheckFstime(&info->mtime))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_SetFileInfoW(path, stat))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_CreateFile

  Description:  FATFS_COMMAND_CREATE_FILE�R�}���h

  Arguments:    

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_CreateFile(FSArchive *arc, u32 baseid, const char *relpath, u32 permit)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    char                    permitstring[16];
    char                   *s = permitstring;
    BOOL                    tranc = TRUE;
    u16                    *path = FSi_AllocUnicodeFullPath(context, relpath);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if ((permit & FS_PERMIT_R) != 0)
    {
        *s++ = 'r';
    }
    if ((permit & FS_PERMIT_W) != 0)
    {
        *s++ = 'w';
    }
    *s++ = '\0';
    if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_CreateFileW(path, tranc, permitstring))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_DeleteFile

  Description:  FATFS_COMMAND_DELETE_FILE�R�}���h

  Arguments:    

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_DeleteFile(FSArchive *arc, u32 baseid, const char *relpath)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    u16                    *path = FSi_AllocUnicodeFullPath(context, relpath);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_DeleteFileW(path))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_RenameFile

  Description:  FATFS_COMMAND_RENAME_FILE�R�}���h

  Arguments:    

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_RenameFile(FSArchive *arc, u32 baseid_src, const char *relpath_src, u32 baseid_dst, const char *relpath_dst)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    u16                    *src = FSi_AllocUnicodeFullPath(context, relpath_src);
    u16                    *dst = FSi_AllocUnicodeFullPath(context, relpath_dst);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FSi_UsingInvalidCharacterW(src))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FSi_UsingInvalidCharacterW(dst))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_RenameFileW(src, dst))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(src);
    FSi_ReleaseUnicodeBuffer(dst);
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid_src;
    (void)baseid_dst;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_CreateDirectory

  Description:  FATFS_COMMAND_CREATE_DIRECTORY�R�}���h

  Arguments:    

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_CreateDirectory(FSArchive *arc, u32 baseid, const char *relpath, u32 permit)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    char                    permitstring[16];
    char                   *s = permitstring;
    u16                    *path = FSi_AllocUnicodeFullPath(context, relpath);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if ((permit & FS_PERMIT_R) != 0)
    {
        *s++ = 'r';
    }
    if ((permit & FS_PERMIT_W) != 0)
    {
        *s++ = 'w';
    }
    *s++ = '\0';
    if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_CreateDirectoryW(path, permitstring))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_DeleteDirectory

  Description:  FATFS_COMMAND_DELETE_DIRECTORY�R�}���h

  Arguments:    

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_DeleteDirectory(FSArchive *arc, u32 baseid, const char *relpath)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    u16                    *path = FSi_AllocUnicodeFullPath(context, relpath);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_DeleteDirectoryW(path))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_RenameDirectory

  Description:  FATFS_COMMAND_RENAME_DIRECTORY�R�}���h

  Arguments:    

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_RenameDirectory(FSArchive *arc, u32 baseid_src, const char *relpath_src, u32 baseid_dst, const char *relpath_dst)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    u16                    *src = FSi_AllocUnicodeFullPath(context, relpath_src);
    u16                    *dst = FSi_AllocUnicodeFullPath(context, relpath_dst);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FSi_UsingInvalidCharacterW(src))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FSi_UsingInvalidCharacterW(dst))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_RenameDirectoryW(src, dst))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(src);
    FSi_ReleaseUnicodeBuffer(dst);
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid_src;
    (void)baseid_dst;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_GetArchiveResource

  Description:  FS_COMMAND_GETARCHIVERESOURCE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                resource            ���\�[�X���̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_GetArchiveResource(FSArchive *arc, FSArchiveResource *resource)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    u16                    *path = FSi_AllocUnicodeFullPath(context, "/");
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FSi_UsingInvalidCharacterW(path))
    {
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if (FATFS_GetDriveResourceW(path, context->resource))
    {
        resource->totalSize = context->resource->totalSize;
        resource->availableSize = context->resource->availableSize;
        resource->maxFileHandles = context->resource->maxFileHandles;
        resource->currentFileHandles = context->resource->currentFileHandles;
        resource->maxDirectoryHandles = context->resource->maxDirectoryHandles;
        resource->currentDirectoryHandles = context->resource->currentDirectoryHandles;
        // for FAT archives.
        resource->bytesPerSector = context->resource->bytesPerSector;
        resource->sectorsPerCluster = context->resource->sectorsPerCluster;
        resource->totalClusters = context->resource->totalClusters;
        resource->availableClusters = context->resource->availableClusters;
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FSi_ReleaseUnicodeBuffer(path);
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_OpenFile

  Description:  FS_COMMAND_OPENFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                baseid              ��_�f�B���N�g�� (���[�g�Ȃ�0)
                path                �t�@�C���p�X
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_OpenFile(FSArchive *arc, FSFile *file, u32 baseid, const char *path, u32 mode)
{
    FSResult                result = FS_RESULT_ERROR;
    FATFSFileHandle handle = FATFS_INVALID_HANDLE;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    char                    modestring[16];
    char                   *s = modestring;
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if ((mode & FS_FILEMODE_R) != 0)
    {
        *s++ = 'r';
        if ((mode & FS_FILEMODE_W) != 0)
        {
            *s++ = '+';
        }
    }
    else if ((mode & FS_FILEMODE_W) != 0)
    {
        *s++ = 'w';
    }
    if ((mode & FS_FILEMODE_L) != 0)
    {
        *s++ = 'l';
    }
    *s++ = '\0';
    {
        const u16  *unipath = NULL;
        u16        *fpath = NULL;
        // Unicode�ł̍\���̂�n����Ă����Unicode�̂܂܊i�[�B
        if ((file->stat & FS_FILE_STATUS_UNICODE_MODE) != 0)
        {
            unipath = (const u16 *)path;
        }
        // �����łȂ����Shift_JIS�łƂ��Ċi�[�B
        // (����������͎b��I�Ȏ����ł���A�����I�ɂ�
        //  CAPS_UNICODE��錾�����A�[�J�C�u��Unicode�ňȊO�͓n����Ȃ�)
        else
        {
            fpath = FSi_AllocUnicodeFullPath(context, path);
            unipath = fpath;
        }
        if (FSi_UsingInvalidCharacterW(unipath))
        {
            result = FS_RESULT_INVALID_PARAMETER;
        }
        else
        {
            handle = FATFS_OpenFileW(unipath, modestring);
            if (handle != FATFS_INVALID_HANDLE)
            {
                FS_SetFileHandle(file, arc, (void*)handle);
                result = FS_RESULT_SUCCESS;
            }
            else
            {
                result = FSi_ConvertError(tls->result);
            }
        }
        FSi_ReleaseUnicodeBuffer(fpath);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_CloseFile

  Description:  FS_COMMAND_CLOSEFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_CloseFile(FSArchive *arc, FSFile *file)
{
    FSResult        result = FS_RESULT_ERROR;
    FATFSFileHandle handle = (FATFSFileHandle)FS_GetFileUserData(file);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FATFS_CloseFile(handle))
    {
        FS_DetachHandle(file);
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_ReadFile

  Description:  FS_COMMAND_READFILE�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                buffer           �]���惁����
                length           �]���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_ReadFile(FSArchive *arc, FSFile *file, void *buffer, u32 *length)
{
    FSResult                result = FS_RESULT_SUCCESS;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    FATFSFileHandle         handle = (FATFSFileHandle)FS_GetFileUserData(file);
    u32                     rest = *length;
    BOOL                    async = ((file->stat & FS_FILE_STATUS_BLOCKING) == 0);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    while (rest > 0)
    {
        void   *dst = buffer;
        u32     len = rest;
        int     read;
        // ARM7�����ڃA�N�Z�X�ł��Ȃ����������w�肳�ꂽ��ꎞ�o�b�t�@�ő�ցB
        if (!FSi_IsValidAddressForARM7(buffer, rest))
        {
            dst = context->tmpbuf;
            len = (u32)MATH_MIN(len, FS_TMPBUF_LENGTH);
        }
        // 32�o�C�g�������Ă��Ȃ����������w�肳�ꂽ��ꎞ�o�b�t�@�ő�ցB
        else if ((((u32)buffer | len) & 31) != 0)
        {
            // ���̓]���ŏI���̂Ȃ����Ȓ����͖����B
            if (len <= FS_TMPBUF_LENGTH)
            {
                dst = context->tmpbuf;
                len = (u32)MATH_MIN(len, FS_TMPBUF_LENGTH);
            }
            // �o�b�t�@��[���s�����Ȃ�Ύ��̓]���Ő�������悤�����B
            else if (((u32)buffer & 31) != 0)
            {
                dst = context->tmpbuf;
                len = MATH_ROUNDUP((u32)buffer, 32) - (u32)buffer;
            }
            // �]���T�C�Y�����̕s�����Ȃ�ΏI�[��؂�l�߂Ē��ړ]���B
            else
            {
                len = MATH_ROUNDDOWN(len, 32);
            }
        }
        // �ꎞ�o�b�t�@���g�p���Ȃ��Ƃ����C���������Ȃ��
        // �_�[�e�B�ȃL���b�V�������C�g�o�b�N���Ă����B
        if ((dst == buffer) &&
            (((u32)dst >= HW_TWL_MAIN_MEM) && ((u32)dst + len <= HW_TWL_MAIN_MEM_END)))
        {
            DC_FlushRange(dst, len);
        }
        // �񓯊����������߂��Ă��Ď��ۂɔ񓯊������\�ł����
        // �����Ń��N�G�X�g�o�b�t�@��ݒ肷��B
        async &= ((dst == buffer) && (len == rest));
        if (async)
        {
            FATFSi_SetRequestBuffer(&FSiFATFSAsyncRequest[context - FSiFATFSDrive], FSi_FATFSAsyncDone, context);
        }
        // ���ۂ̌Ăяo��
        read = FATFS_ReadFile(handle, dst, (int)len);
        if (async)
        {
            rest -= len;
            result = FS_RESULT_PROC_ASYNC;
            break;
        }
        // �ꎞ�o�b�t�@�ő�ւ����Ȃ炱���ŃR�s�[
        if ((dst != buffer) && (read > 0))
        {
            DC_InvalidateRange(dst, (u32)read);
            MI_CpuCopy8(dst, buffer, (u32)read);
        }
        buffer = (u8 *)buffer + read;
        rest -= read;
        if (read != len)
        {
            result = FSi_ConvertError(tls->result);
            break;
        }
    }
    *length -= rest;
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_WriteFile

  Description:  FS_COMMAND_WRITEFILE�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                buffer           �]����������
                length           �]���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_WriteFile(FSArchive *arc, FSFile *file, const void *buffer, u32 *length)
{
    FSResult                result = FS_RESULT_SUCCESS;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    FATFSFileHandle         handle = (FATFSFileHandle)FS_GetFileUserData(file);
    u32                     rest = *length;
    BOOL                    async = ((file->stat & FS_FILE_STATUS_BLOCKING) == 0);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    while (rest > 0)
    {
        // ARM7�����ڃA�N�Z�X�ł��Ȃ����������w�肳�ꂽ��ꎞ�o�b�t�@�ő�ցB
        const void *dst = buffer;
        u32         len = rest;
        int         written;
        if (!FSi_IsValidAddressForARM7(buffer, rest) || ((((u32)buffer | len) & 31) != 0))
        {
            dst = context->tmpbuf;
            len = (u32)MATH_MIN(len, FS_TMPBUF_LENGTH);
        }
        // 32�o�C�g�������Ă��Ȃ����������w�肳�ꂽ��ꎞ�o�b�t�@�ő�ցB
        else if ((((u32)buffer | len) & 31) != 0)
        {
            // ���̓]���ŏI���̂Ȃ����Ȓ����͖����B
            if (len <= FS_TMPBUF_LENGTH)
            {
                dst = context->tmpbuf;
                len = (u32)MATH_MIN(len, FS_TMPBUF_LENGTH);
            }
            // �o�b�t�@��[���s�����Ȃ�Ύ��̓]���Ő�������悤�����B
            else if (((u32)buffer & 31) != 0)
            {
                dst = context->tmpbuf;
                len = MATH_ROUNDUP((u32)buffer, 32) - (u32)buffer;
            }
            // �]���T�C�Y�����̕s�����Ȃ�ΏI�[��؂�l�߂Ē��ړ]���B
            else
            {
                len = MATH_ROUNDDOWN(len, 32);
            }
        }
        // �ꎞ�o�b�t�@���g�p����Ȃ珑�����ނׂ��f�[�^���R�s�[�B
        if (dst != buffer)
        {
            MI_CpuCopy8(buffer, context->tmpbuf, len);
        }
        // �o�b�t�@�����C���������Ȃ�ARM7���猩����悤���C�g�o�b�N���K�v�B
        if (((u32)dst >= HW_TWL_MAIN_MEM) && ((u32)dst + len <= HW_TWL_MAIN_MEM_END))
        {
            DC_StoreRange(dst, len);
        }
        // �񓯊����������߂��Ă��Ď��ۂɔ񓯊������\�ł����
        // �����Ń��N�G�X�g�o�b�t�@��ݒ肷��B
        async &= ((dst == buffer) && (len == rest));
        if (async)
        {
            FATFSi_SetRequestBuffer(&FSiFATFSAsyncRequest[context - FSiFATFSDrive], FSi_FATFSAsyncDone, context);
        }
        // ���ۂ̌Ăяo��
        written = FATFS_WriteFile(handle, dst, (int)len);
        if (async)
        {
            rest -= len;
            result = FS_RESULT_PROC_ASYNC;
            break;
        }
        buffer = (u8 *)buffer + written;
        rest -= written;
        if (written != len)
        {
            result = FSi_ConvertError(tls->result);
            break;
        }
    }
    *length -= rest;
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_SetSeekCache

  Description:  FS_COMMAND_SETSEEKCACHE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                buf                 �L���b�V���o�b�t�@
                buf_size            �L���b�V���o�b�t�@�T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_SetSeekCache(FSArchive *arc, FSFile *file, void* buf, u32 buf_size)
{
    FSResult        result = FS_RESULT_ERROR;
    FATFSFileHandle handle = (FATFSFileHandle)FS_GetFileUserData(file);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    
    if ((buf != NULL) &&
        (((u32)buf < HW_TWL_MAIN_MEM) || (((u32)buf + buf_size) > HW_TWL_MAIN_MEM_END)))
    {
        // ���C����������̃o�b�t�@�ł͂Ȃ��ꍇ�̓p�����[�^�G���[
        result = FS_RESULT_INVALID_PARAMETER;
    }
    else if ((buf != NULL) && (buf_size < 32))
    {
        // ARM7�Ƃ̃��������L�ɃT�C�Y������Ȃ��ꍇ�͎��s
        result = FS_RESULT_FAILURE;
    }
    else if ((((u32)buf & 31) != 0) && ((buf_size - (32 - ((u32)buf & 31))) < 32))
    {
        // 32�o�C�g������ɃT�C�Y������Ȃ��ꍇ�����s
        result = FS_RESULT_FAILURE;
    }
    else
    {
        void   *cache;
        u32     cache_size;
        
        if (buf != NULL) {
            // �{�֐��Ŏg�p����O�ɃL���b�V���ɂ̂��Ă����\�������邽�߁A�t���b�V��
            DC_FlushRange(buf, buf_size);
        }
        
        // ARM7�Ƌ��L�ł���悤��32�o�C�g���������̈�𔲂��o��
        if (((u32)buf & 31) != 0) {
            cache = (void *)((u32)buf + (32 - ((u32)buf & 31)));    // 32�o�C�g���E
            cache_size = buf_size - (32 - ((u32)buf & 31));
        } else {
            cache = buf;
            cache_size = buf_size;
        }
        cache_size = cache_size & ~31;                              // 32�o�C�g�؂艺��

        if( FATFS_SetSeekCache(handle, cache, cache_size) != FALSE)
        {
            result = FS_RESULT_SUCCESS;
        }
        else
        {
            result = FSi_ConvertError(tls->result);
        }
    }
    (void)arc;
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_SeekFile

  Description:  FS_COMMAND_SEEKFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                offset              �ړ��ʂ���шړ���̈ʒu
                from                �V�[�N�N�_

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_SeekFile(FSArchive *arc, FSFile *file, int *offset, FSSeekFileMode from)
{
    FSResult        result = FS_RESULT_ERROR;
    FATFSFileHandle handle = (FATFSFileHandle)FS_GetFileUserData(file);
    FATFSSeekMode   mode = FATFS_SEEK_CUR;
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (from == FS_SEEK_SET)
    {
        mode = FATFS_SEEK_SET;
    }
    else if (from == FS_SEEK_CUR)
    {
        mode = FATFS_SEEK_CUR;
    }
    else if (from == FS_SEEK_END)
    {
        mode = FATFS_SEEK_END;
    }
    *offset = FATFS_SeekFile(handle, *offset, mode);
    if (*offset >= 0)
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_GetFileLength

  Description:  FS_COMMAND_GETFILELENGTH�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                length              �擾�����T�C�Y�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_GetFileLength(FSArchive *arc, FSFile *file, u32 *length)
{
    FSResult        result = FS_RESULT_ERROR;
    FATFSFileHandle handle = (FATFSFileHandle)FS_GetFileUserData(file);
    int             n = FATFS_GetFileLength(handle);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (n >= 0)
    {
        *length = (u32)n;
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_SetFileLength

  Description:  FS_COMMAND_SETFILELENGTH�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                length              �ݒ肷��t�@�C���T�C�Y

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_SetFileLength(FSArchive *arc, FSFile *file, u32 length)
{
    FSResult        result = FS_RESULT_ERROR;
    FATFSFileHandle handle = (FATFSFileHandle)FS_GetFileUserData(file);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FATFS_SetFileLength(handle, (int)length))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_GetFilePosition

  Description:  FS_COMMAND_GETFILEPOSITION�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                position            �擾�����ʒu�̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_GetFilePosition(FSArchive *arc, FSFile *file, u32 *position)
{
    *position = 0;
    return FSi_FATFS_SeekFile(arc, file, (int*)position, FS_SEEK_CUR);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_FlushFile

  Description:  FS_COMMAND_FLUSHFILE�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_FlushFile(FSArchive *arc, FSFile *file)
{
    FSResult        result = FS_RESULT_ERROR;
    FATFSFileHandle handle = (FATFSFileHandle)FS_GetFileUserData(file);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FATFS_FlushFile(handle))
    {
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_OpenDirectory

  Description:  FS_COMMAND_OPENDIRECTORY�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��
                baseid              ��_�f�B���N�g��ID (���[�g�Ȃ�0)
                path                �p�X
                mode                �A�N�Z�X���[�h

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_OpenDirectory(FSArchive *arc, FSFile *file, u32 baseid, const char *path, u32 mode)
{
    FSResult                result = FS_RESULT_ERROR;
    FSFATFSArchiveContext  *context = (FSFATFSArchiveContext *)FS_GetArchiveUserData(arc);
    FATFSDirectoryHandle    handle = FATFS_INVALID_HANDLE;
    char                    modestring[16];
    char                   *s = modestring;
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if ((mode & FS_FILEMODE_R) != 0)
    {
        *s++ = 'r';
    }
    if ((mode & FS_FILEMODE_W) != 0)
    {
        *s++ = 'w';
    }
    if ((mode & FS_DIRMODE_SHORTNAME_ONLY) != 0)
    {
        *s++ = 's';
    }
    *s++ = '\0';
    {
        u16    *unipath = NULL;
        u16    *fpath = NULL;
        // Unicode�ł̍\���̂�n����Ă����Unicode�̂܂܊i�[�B
        if ((file->stat & FS_FILE_STATUS_UNICODE_MODE) != 0)
        {
            unipath = (u16 *)path;
        }
        // �����łȂ����Shift_JIS�łƂ��Ċi�[�B
        // (����������͎b��I�Ȏ����ł���A�����I�ɂ�
        //  CAPS_UNICODE��錾�����A�[�J�C�u��Unicode�ňȊO�͓n����Ȃ�)
        else
        {
            fpath = FSi_AllocUnicodeFullPath(context, path);
            unipath = fpath;
        }
        // �I�[�̓���ȃ��C���h�J�[�h�w��𖳌����B
        // �p�X��FS���C�u��������ɐ��K�����ēn���Ă��邽��
        // �o�b�t�@�̓��e�͕ύX�\�ł��邱�Ƃ��ۏ؂���Ă���B
        if (*unipath)
        {
            int     pos;
            for (pos = 0; unipath[pos]; ++pos)
            {
            }
            for (--pos; (pos > 0) && (unipath[pos] == L'*'); --pos)
            {
            }
            if (unipath[pos] != L'/')
            {
                unipath[++pos] = L'/';
            }
            unipath[++pos] = L'*';
            unipath[++pos] = L'\0';
        }
        handle = FATFS_OpenDirectoryW(unipath, modestring);
        if (handle != FATFS_INVALID_HANDLE)
        {
            FS_SetDirectoryHandle(file, arc, (void*)handle);
            result = FS_RESULT_SUCCESS;
        }
        else
        {
            result = FSi_ConvertError(tls->result);
        }
        FSi_ReleaseUnicodeBuffer(fpath);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)baseid;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_CloseDirectory

  Description:  FS_COMMAND_CLOSEDIRECTORY�R�}���h

  Arguments:    arc                 �Ăяo�����A�[�J�C�u
                file                �Ώۃt�@�C��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_CloseDirectory(FSArchive *arc, FSFile *file)
{
    FSResult                result = FS_RESULT_ERROR;
    FATFSDirectoryHandle    handle = (FATFSDirectoryHandle)FS_GetFileUserData(file);
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FATFS_CloseDirectory(handle))
    {
        FS_DetachHandle(file);
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FATFS_ReadDirectory

  Description:  FS_COMMAND_READDIR�R�}���h

  Arguments:    arc              �Ăяo�����A�[�J�C�u
                file             �Ώۃt�@�C��
                info             ���̊i�[��

  Returns:      �R�}���h�̏�������
 *---------------------------------------------------------------------------*/
static FSResult FSi_FATFS_ReadDirectory(FSArchive *arc, FSFile *file, FSDirectoryEntryInfo *info)
{
    FSResult                result = FS_RESULT_ERROR;
    FATFSDirectoryHandle    handle = (FATFSDirectoryHandle)FS_GetFileUserData(file);
    FATFSFileInfoW          tmp[1];
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (FATFS_ReadDirectoryW(handle, tmp))
    {
        // Unicode�ł̍\���̂�n����Ă����Unicode�̂܂܊i�[�B
        if ((file->stat & FS_FILE_STATUS_UNICODE_MODE) != 0)
        {
            FSDirectoryEntryInfoW  *infow = (FSDirectoryEntryInfoW *)info;
            infow->shortname_length = (u32)STD_GetStringLength(tmp->shortname);
            (void)STD_CopyLString(infow->shortname, tmp->shortname, sizeof(infow->shortname));
            {
                int     n;
                for (n = 0; (n < FS_ENTRY_LONGNAME_MAX - 1) && tmp->longname[n]; ++n)
                {
                    infow->longname[n] = tmp->longname[n];
                }
                infow->longname[n] = L'\0';
                infow->longname_length = (u32)n;
            }
            infow->attributes = tmp->attributes;
            if ((tmp->attributes & FATFS_ATTRIBUTE_DOS_DIRECTORY) != 0)
            {
                infow->attributes |= FS_ATTRIBUTE_IS_DIRECTORY;
            }
            infow->filesize = tmp->length;
            FSi_DostimeToFstime(&infow->atime, tmp->dos_atime);
            FSi_DostimeToFstime(&infow->mtime, tmp->dos_mtime);
            FSi_DostimeToFstime(&infow->ctime, tmp->dos_ctime);
        }
        // �����łȂ����Shift_JIS�łƂ��Ċi�[�B
        // (����������͎b��I�Ȏ����ł���A�����I�ɂ�
        //  CAPS_UNICODE��錾�����A�[�J�C�u��Unicode�ňȊO�͓n����Ȃ�)
        else
        {
            FSDirectoryEntryInfo   *infoa = (FSDirectoryEntryInfo *)info;
            infoa->shortname_length = (u32)STD_GetStringLength(tmp->shortname);
            (void)STD_CopyLString(infoa->shortname, tmp->shortname, sizeof(infoa->shortname));
            {
                int     dstlen = sizeof(infoa->longname) - 1;
                int     srclen = 0;
                (void)FSi_ConvertStringUnicodeToSjis(infoa->longname, &dstlen, tmp->longname, NULL, NULL);
                infoa->longname[dstlen] = L'\0';
                infoa->longname_length = (u32)dstlen;
            }
            infoa->attributes = tmp->attributes;
            if ((tmp->attributes & FATFS_ATTRIBUTE_DOS_DIRECTORY) != 0)
            {
                infoa->attributes |= FS_ATTRIBUTE_IS_DIRECTORY;
            }
            infoa->filesize = tmp->length;
            FSi_DostimeToFstime(&infoa->atime, tmp->dos_atime);
            FSi_DostimeToFstime(&infoa->mtime, tmp->dos_mtime);
            FSi_DostimeToFstime(&infoa->ctime, tmp->dos_ctime);
        }
        result = FS_RESULT_SUCCESS;
    }
    else
    {
        result = FSi_ConvertError(tls->result);
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    (void)arc;
    return result;
}

static PMSleepCallbackInfo  FSiPreSleepInfo;
static PMExitCallbackInfo   FSiPostExitInfo;
static void FSi_SleepProcedure(void *arg)
{
    (void)arg;
    (void)FATFS_FlushAll();
}

static void FSi_ShutdownProcedure(void *arg)
{
    (void)arg;
    (void)FATFS_UnmountAll();
}

/*---------------------------------------------------------------------------*
  Name:         FSi_MountFATFSArchive

  Description:  FATFS�A�[�J�C�u���}�E���g

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_MountFATFSArchive(FSFATFSArchiveContext *context, const char *arcname, u8 *tmpbuf)
{
    static const FSArchiveInterface FSiArchiveFATFSInterface =
    {
        // ���d�l�ƌ݊����̂���R�}���h
        FSi_FATFS_ReadFile,
        FSi_FATFS_WriteFile,
        NULL,               // SeekDirectory
        FSi_FATFS_ReadDirectory,
        NULL,               // FindPath
        NULL,               // GetPath
        NULL,               // OpenFileFast
        NULL,               // OpenFileDirect
        FSi_FATFS_CloseFile,
        NULL,               // Activate
        NULL,               // Idle
        NULL,               // Suspend
        NULL,               // Resume
        // ���d�l�ƌ݊��������邪�R�}���h�ł͂Ȃ���������
        FSi_FATFS_OpenFile,
        FSi_FATFS_SeekFile,
        FSi_FATFS_GetFileLength,
        FSi_FATFS_GetFilePosition,
        // �V�d�l�Ŋg�����ꂽ�R�}���h (NULL�Ȃ�UNSUPPORTED)
        NULL,               // Mount
        NULL,               // Unmount
        FSi_FATFS_GetArchiveCaps,
        FSi_FATFS_CreateFile,
        FSi_FATFS_DeleteFile,
        FSi_FATFS_RenameFile,
        FSi_FATFS_GetPathInfo,
        FSi_FATFS_SetPathInfo,
        FSi_FATFS_CreateDirectory,
        FSi_FATFS_DeleteDirectory,
        FSi_FATFS_RenameDirectory,
        FSi_FATFS_GetArchiveResource,
        NULL,               // 29UL
        FSi_FATFS_FlushFile,
        FSi_FATFS_SetFileLength,
        FSi_FATFS_OpenDirectory,
        FSi_FATFS_CloseDirectory,
        FSi_FATFS_SetSeekCache,
    };
    u32     arcnamelen = (u32)STD_GetStringLength(arcname);
    context->tmpbuf = tmpbuf;
    FS_InitArchive(context->arc);
    if (FS_RegisterArchiveName(context->arc, arcname, arcnamelen))
    {
        (void)FS_MountArchive(context->arc, context, &FSiArchiveFATFSInterface, 0);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_MountDefaultArchives

  Description:  IPL�ɗ^����ꂽ�N���������Q�Ƃ��ăf�t�H���g�A�[�J�C�u���}�E���g

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_MountDefaultArchives(void)
{
    // FATFS���e��̕������f�B�A���}�E���g����܂ő҂B
    FATFS_Init();
    FSi_SetupFATBuffers();
    // �X���[�v�O�ƃV���b�g�_�E���O�Ƀt���b�V�����邽�߂̃R�[���o�b�N��ݒ�B
    FSiPreSleepInfo.callback = FSi_SleepProcedure;
    FSiPostExitInfo.callback = FSi_ShutdownProcedure;
    PMi_InsertPreSleepCallbackEx(&FSiPreSleepInfo, PM_CALLBACK_PRIORITY_FS);
    PMi_InsertPostExitCallbackEx(&FSiPostExitInfo, PM_CALLBACK_PRIORITY_FS);
    // ���ۂɃA�N�Z�X���������_���h���C�u�̂�FS�փ}�E���g����B
    if (FSiFATFSDrive && FSiTemporaryBuffer)
    {
        int                 index = 0;
        static const int    max = FS_MOUNTDRIVE_MAX;
        const char         *arcname = FATFSi_GetArcnameList();
        while ((index < max) && *arcname)
        {
            FSi_MountFATFSArchive(&FSiFATFSDrive[index], arcname, &FSiTemporaryBuffer[FS_TMPBUF_LENGTH * index]);
            arcname += STD_GetStringLength(arcname) + 1;
            ++index;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_MountSpecialArchive

  Description:  ����ȃA�[�J�C�u�𒼐ڃ}�E���g

  Arguments:    param   : �}�E���g�Ώۂ��w�肷��p�����[�^
                arcname : �}�E���g����A�[�J�C�u��
                          "otherPub"�܂���"otherPrv"���w��\��
                          NULL���w�肷��ƈȑO�̃A�[�J�C�u���A���}�E���g
                pWork   : �}�E���g�Ɏg�p���郏�[�N�̈�B
                          �}�E���g����Ă���ԕێ������K�v������܂��B

  Returns:      ���������������FS_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
FSResult FSi_MountSpecialArchive(u64 param, const char *arcname, FSFATFSArchiveWork* pWork)
{
    FSResult    result = FS_RESULT_ERROR;
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (!FATFS_MountSpecial(param, arcname, &pWork->slot))
    {
        result = FSi_ConvertError(tls->result);
    }
    else
    {
        if (arcname && *arcname)
        {
            FSi_MountFATFSArchive(&pWork->context, arcname, pWork->tmpbuf);
        }
        else
        {
            (void)FS_UnmountArchive(pWork->context.arc);
            (void)FS_ReleaseArchiveName(pWork->context.arc);
        }
        result = FS_RESULT_SUCCESS;
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FormatSpecialArchive

  Description:  �ȉ��̏����𖞂�������ȃA�[�J�C�u�̓��e���t�H�[�}�b�g�B
                  - ���݂��łɃ}�E���g����Ă���B
                  - ���g�����L�������B (dataPub, dataPrv, share*)

  Arguments:    path : �A�[�J�C�u�����܂񂾃p�X��

  Returns:      ���������������FS_RESULT_SUCCESS
 *---------------------------------------------------------------------------*/
FSResult FSi_FormatSpecialArchive(const char *path)
{
    FSResult    result = FS_RESULT_ERROR;
    FATFSResultBuffer       tls[1];
    FATFS_RegisterResultBuffer(tls, TRUE);
    if (!FATFS_FormatSpecial(path))
    {
        result = FSi_ConvertError(tls->result);
    }
    else
    {
        result = FS_RESULT_SUCCESS;
    }
    FATFS_RegisterResultBuffer(tls, FALSE);
    return result;
}


#include <twl/ltdmain_end.h>


#endif /* FS_IMPLEMENT */

#endif /* SDK_TWL */

/*---------------------------------------------------------------------------*
  Name:         FS_ForceToEnableLatencyEmulation

  Description:  �򉻂���NAND�f�o�C�X�ւ̃A�N�Z�X���ɐ�����
                �����_���ȃE�F�C�g���Ԃ��h���C�o�ŋ[���I�ɍČ�����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_ForceToEnableLatencyEmulation(void)
{
#if defined(SDK_TWL)
    if (OS_IsRunOnTwl())
    {
        (void)FATFS_SetLatencyEmulation(TRUE);
    }
#endif
}

