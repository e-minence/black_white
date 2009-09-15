/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_backup.c

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


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_spi.h"


#ifndef SDK_ARM9
SDK_ERROR("this code is only for ARM9"!
#endif // SDK_ARM9


/*---------------------------------------------------------------------------*/
/* constants */

#include <nitro/version_begin.h>
SDK_DEFINE_MIDDLEWARE(cardi_backup_assert, "NINTENDO", "BACKUP");
#include <nitro/version_end.h>
#define SDK_USING_BACKUP() SDK_USING_MIDDLEWARE(cardi_backup_assert)


/*---------------------------------------------------------------------------*/
/* variables */

/* �Ō�ɓ]�������o�b�N�A�b�v�y�[�W�̃L���b�V�� */
static u8 CARDi_backup_cache_page_buf[256] ATTRIBUTE_ALIGN(32);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnFifoRecv

  Description:  PXI FIFO ���[�h��M�R�[���o�b�N

  Arguments:    tag        PXI �^�O (��� PXI_FIFO_TAG_FS)
                data       ��M�f�[�^
                err        �G���[�r�b�g (���d�l�ɂ�����)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(data)
    if ((tag == PXI_FIFO_TAG_FS) && err)
    {
        CARDiCommon *const p = &cardi_common;
        /* ARM7 ����̉�������M���Ċ�����ʒm���� */
        SDK_ASSERT(data == CARD_REQ_ACK);
        p->flag &= ~CARD_STAT_WAITFOR7ACK;
        OS_WakeupThreadDirect(p->current_thread_9);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_Request

  Description:  ARM9 ���� ARM7 �փ��N�G�X�g���M���Ċ������u���b�L���O�B
                ���ʂ�CARD_RESULT_SUCCESS�łȂ��Ȃ�w��񐔂܂ł͍Ď��s����B
                (����̃o�X�̃��b�N����у^�X�N�X���b�h�̔r�������
                 ���̊֐��̌Ăяo�����ŕۏ؂���)
                ���̃R�}���h���ŌJ��Ԃ��Ăяo�����T�u�����B

  Arguments:    req_type     �R�}���h���N�G�X�g�̎��
                retry_max    ���g���C�ő��

  Returns:      ���ʂ�CARD_RESULT_SUCCESS�ł����TRUE
 *---------------------------------------------------------------------------*/
static BOOL CARDi_Request(CARDiCommon *p, int req_type, int retry_count)
{
    // PXI ���������Ȃ炱���Ŏ��s
    if ((p->flag & CARD_STAT_INIT_CMD) == 0)
    {
        p->flag |= CARD_STAT_INIT_CMD;
        while (!PXI_IsCallbackReady(PXI_FIFO_TAG_FS, PXI_PROC_ARM7))
        {
            OS_SpinWait(100);
        }
        // �ŏ��̃R�}���h "INIT" �𑗐M (�ċA)
        (void)CARDi_Request(p, CARD_REQ_INIT, 1);
    }
    // �ݒ肵�����L���������t���b�V��
    DC_FlushRange(p->cmd, sizeof(*p->cmd));
    DC_WaitWriteBufferEmpty();

    // �R�}���h�������󂯂邽�߂ɃX���b�h�|�C���^��o�^�B
    p->current_thread_9 = OS_GetCurrentThread();

    do
    {
        // �R�}���h���N�G�X�g���M�B
        p->flag |= CARD_STAT_WAITFOR7ACK;
        CARDi_SendPxi((u32)req_type);
        // �㑱����������Βǉ����M
        switch (req_type)
        {
        case CARD_REQ_INIT:
            CARDi_SendPxi((u32)p->cmd);
            break;
        }
        {
            // ARM7���烊�N�G�X�g������ҋ@�B
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while ((p->flag & CARD_STAT_WAITFOR7ACK) != 0)
            {
                OS_SleepThread(NULL);
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
        DC_InvalidateRange(p->cmd, sizeof(*p->cmd));
    }
    while ((p->cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));

    return (p->cmd->result == CARD_RESULT_SUCCESS);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommandCore

  Description:  �f�[�^��]������R�}���h���N�G�X�g�̏����{�́B
                �����I���邢�͔񓯊��I�ɌĂяo�����B

  Arguments:    p          ���C�u�����̃��[�N�o�b�t�@ (�����̂��߂Ɉ����n��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_RequestStreamCommandCore(CARDiCommon * p)
{
    const int req_type = p->req_type;
    const int req_mode = p->req_mode;
    const int retry_count = p->req_retry;
    u32     size = sizeof(CARDi_backup_cache_page_buf);

    SDK_USING_BACKUP();

    /* �y�[�W�܂��̓Z�N�^�P�ʂŃ��N�G�X�g */
    if (req_type == CARD_REQ_ERASE_SECTOR_BACKUP)
    {
        size = CARD_GetBackupSectorSize();
    }
    else if (req_type == CARD_REQ_ERASE_SUBSECTOR_BACKUP)
    {
        size = cardi_common.cmd->spec.subsect_size;
    }
    do
    {
        const u32 len = (size < p->len) ? size : p->len;
        p->cmd->len = len;

        /* �L�����Z���v��������΂����Œ��~ */
        if ((p->flag & CARD_STAT_CANCEL) != 0)
        {
            p->flag &= ~CARD_STAT_CANCEL;
            p->cmd->result = CARD_RESULT_CANCELED;
            break;
        }
        switch (req_mode)
        {
        case CARD_REQUEST_MODE_RECV:
            /* ��M�n�R�}���h�Ȃ�o�b�t�@�𖳌��� */
            DC_InvalidateRange(CARDi_backup_cache_page_buf, len);
            p->cmd->src = (u32)p->src;
            p->cmd->dst = (u32)CARDi_backup_cache_page_buf;
            break;
        case CARD_REQUEST_MODE_SEND:
        case CARD_REQUEST_MODE_SEND_VERIFY:
            /* ���M�n�R�}���h�Ȃ�f�[�^���e���|�����o�b�t�@�փR�s�[ */
            MI_CpuCopy8((const void *)p->src, CARDi_backup_cache_page_buf, len);
            DC_FlushRange(CARDi_backup_cache_page_buf, len);
            DC_WaitWriteBufferEmpty();
            p->cmd->src = (u32)CARDi_backup_cache_page_buf;
            p->cmd->dst = (u32)p->dst;
            break;
        case CARD_REQUEST_MODE_SPECIAL:
            /* �o�b�t�@����͕s�v */
            p->cmd->src = (u32)p->src;
            p->cmd->dst = (u32)p->dst;
            break;
        }
        /* ���N�G�X�g���M */
        if (!CARDi_Request(p, req_type, retry_count))
        {
            break;
        }
        /* �w�肪����΂���ɓ����ݒ�Ńx���t�@�C�v�� */
        if (req_mode == CARD_REQUEST_MODE_SEND_VERIFY)
        {
            if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
            {
                break;
            }
        }
        else if (req_mode == CARD_REQUEST_MODE_RECV)
        {
            /* �L���b�V������R�s�[ */
            MI_CpuCopy8(CARDi_backup_cache_page_buf, (void *)p->dst, len);
        }
        p->src += len;
        p->dst += len;
        p->len -= len;
    }
    while (p->len > 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestWriteSectorCommandCore

  Description:  �Z�N�^���� + �v���O�����̃��N�G�X�g�̏����{�́B
                �����I���邢�͔񓯊��I�ɌĂяo�����B

  Arguments:    p          ���C�u�����̃��[�N�o�b�t�@ (�����̂��߂Ɉ����n��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_RequestWriteSectorCommandCore(CARDiCommon * p)
{
    const u32 sector_size = CARD_GetBackupSectorSize();
    SDK_USING_BACKUP();

    /* �����͈͂��Z�N�^�P�ʂ̐����{�łȂ���Ύ��s��Ԃ� */
    if ((((u32)p->dst | p->len) & (sector_size - 1)) != 0)
    {
        p->flag &= ~CARD_STAT_CANCEL;
        p->cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else
    {
        /* �Z�N�^�P�ʏ��� */
        for (; p->len > 0; p->len -= sector_size)
        {
            u32     len = sector_size;
            /* �L�����Z���v��������΂����Œ��~ */
            if ((p->flag & CARD_STAT_CANCEL) != 0)
            {
                p->flag &= ~CARD_STAT_CANCEL;
                p->cmd->result = CARD_RESULT_CANCELED;
                break;
            }
            /* �Z�N�^���� */
            p->cmd->dst = (u32)p->dst;
            p->cmd->len = len;
            if (!CARDi_Request(p, CARD_REQ_ERASE_SECTOR_BACKUP, 1))
            {
                break;
            }
            while (len > 0)
            {
                const u32 page = sizeof(CARDi_backup_cache_page_buf);
                /* �L�����Z���v��������΂����Œ��~ */
                if ((p->flag & CARD_STAT_CANCEL) != 0)
                {
                    p->flag &= ~CARD_STAT_CANCEL;
                    p->cmd->result = CARD_RESULT_CANCELED;
                    break;
                }
                /* �v���O���� */
                MI_CpuCopy8((const void *)p->src, CARDi_backup_cache_page_buf, page);
                DC_FlushRange(CARDi_backup_cache_page_buf, page);
                DC_WaitWriteBufferEmpty();
                p->cmd->src = (u32)CARDi_backup_cache_page_buf;
                p->cmd->dst = (u32)p->dst;
                p->cmd->len = page;
                if (!CARDi_Request(p, CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX))
                {
                    break;
                }
                /* �K�v�Ȃ�x���t�@�C */
                if (p->req_mode == CARD_REQUEST_MODE_SEND_VERIFY)
                {
                    if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
                    {
                        break;
                    }
                }
                p->src += page;
                p->dst += page;
                len -= page;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_AccessStatusCore

  Description:  �X�e�[�^�X���W�X�^�ւ̃A�N�Z�X�����{�́B
                �����I���邢�͔񓯊��I�ɌĂяo�����B

  Arguments:    p          ���C�u�����̃��[�N�o�b�t�@ (�����̂��߂Ɉ����n��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_AccessStatusCore(CARDiCommon *p)
{
    CARDRequest command = (CARDRequest)CARDi_backup_cache_page_buf[1];
    DC_FlushRange(CARDi_backup_cache_page_buf, 1);
    p->cmd->src = (u32)CARDi_backup_cache_page_buf;
    p->cmd->dst = (u32)CARDi_backup_cache_page_buf;
    (void)CARDi_Request(p, command, 1);
    DC_InvalidateRange(CARDi_backup_cache_page_buf, 1);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_IdentifyBackupCore2

  Description:  �f�o�C�X�^�C�v����.

  Arguments:    type              ���肷��f�o�C�X�^�C�v.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_IdentifyBackupCore2(CARDBackupType type)
{
    /*
     * �擾�����p�����[�^�� CARDiCommandArg �֕ۑ�.
     * ����͍ŏI�I�Ƀe�[�u����p�~���邱�ƂŊ�������.
     */
    {
        CARDiCommandArg *const p = cardi_common.cmd;

        /* �܂��S�p�����[�^���N���A���� NOT_USE ��Ԃɂ��� */
        MI_CpuFill8(&p->spec, 0, sizeof(p->spec));
        p->type = type;
        p->spec.caps = (CARD_BACKUP_CAPS_AVAILABLE | CARD_BACKUP_CAPS_READ_STATUS);
        if (type != CARD_BACKUP_TYPE_NOT_USE)
        {
            /*
             * �f�o�C�X�^�C�v, ���e��, �x���_�� type ����擾�\.
             * �x���_�ԍ���, ����^�C�v�ŕ������[�J�[���̗p����
             * ���s��Ȃǂ̗��R�ŋ�ʂ���K�v���������ꍇ�ȊO 0.
             */
            const u32 size = (u32)(1 << ((type >> CARD_BACKUP_TYPE_SIZEBIT_SHIFT) &
                                         CARD_BACKUP_TYPE_SIZEBIT_MASK));
            const int device =
                ((type >> CARD_BACKUP_TYPE_DEVICE_SHIFT) & CARD_BACKUP_TYPE_DEVICE_MASK);
            const int vender =
                ((type >> CARD_BACKUP_TYPE_VENDER_SHIFT) & CARD_BACKUP_TYPE_VENDER_MASK);

            p->spec.total_size = size;
            /* �X�e�[�^�X���W�X�^�̕␳���s�v�Ȃ� 0xFF. (���ꂪ�ʏ�) */
            p->spec.initial_status = 0xFF;
            if (device == CARD_BACKUP_TYPE_DEVICE_EEPROM)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x000200:        // CARD_BACKUP_TYPE_EEPROM_4KBITS
                    p->spec.page_size = 0x10;
                    p->spec.addr_width = 1;
                    p->spec.program_page = 5;
                    p->spec.initial_status = 0xF0;
                    break;
                case 0x002000:        // CARD_BACKUP_TYPE_EEPROM_64KBITS
                    p->spec.page_size = 0x0020;
                    p->spec.addr_width = 2;
                    p->spec.program_page = 5;
                    p->spec.initial_status = 0x00;
                    break;
                case 0x010000:        // CARD_BACKUP_TYPE_EEPROM_512KBITS
                    p->spec.page_size = 0x0080;
                    p->spec.addr_width = 2;
                    p->spec.program_page = 10;
                    p->spec.initial_status = 0x00;
                    break;
				case 0x020000:	      // CARD_BACKUP_TYPE_EEPROM_1MBITS
					p->spec.page_size = 0x0100;
					p->spec.addr_width = 3;
					p->spec.program_page = 5;
                    p->spec.initial_status = 0x00;
					break;
                }
                p->spec.sect_size = p->spec.page_size;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
            }
            else if (device == CARD_BACKUP_TYPE_DEVICE_FLASH)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x040000:        // CARD_BACKUP_TYPE_FLASH_2MBITS
                case 0x080000:        // CARD_BACKUP_TYPE_FLASH_4MBITS
                case 0x100000:        // CARD_BACKUP_TYPE_FLASH_8MBITS
                    p->spec.write_page = 25;
                    p->spec.write_page_total = 300;
                    p->spec.erase_page = 300;
                    p->spec.erase_sector = 5000;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                    break;
                case 0x200000:        // CARD_BACKUP_TYPE_FLASH_16MBITS
                    p->spec.write_page = 23;
                    p->spec.write_page_total = 300;
                    p->spec.erase_sector = 500;
                    p->spec.erase_sector_total = 5000;
                    p->spec.erase_chip = 10000;
                    p->spec.erase_chip_total = 60000;
                    p->spec.initial_status = 0x00;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    break;
                case 0x400000:        // CARD_BACKUP_TYPE_FLASH_32MBITS
                    p->spec.erase_sector = 600;
                    p->spec.erase_sector_total = 3000;
                    p->spec.erase_subsector = 70;
                    p->spec.erase_subsector_total = 150;
                    p->spec.erase_chip = 23000;
                    p->spec.erase_chip_total = 800000;
                    p->spec.initial_status = 0x00;
                    p->spec.subsect_size = 0x1000;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SUBSECTOR;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    break;
                case 0x800000:
                    if (vender == 0)  // CARD_BACKUP_TYPE_FLASH_64MBITS
                    {
                        p->spec.erase_sector = 1000;
                        p->spec.erase_sector_total = 3000;
                        p->spec.erase_chip = 68000;
                        p->spec.erase_chip_total = 160000;
                        p->spec.initial_status = 0x00;
                        p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                        p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    }
                    else if (vender == 1)   // CARD_BACKUP_TYPE_FLASH_64MBITS_EX
                    {
                        p->spec.erase_sector = 1000;
                        p->spec.erase_sector_total = 3000;
                        p->spec.erase_chip = 68000;
                        p->spec.erase_chip_total = 160000;
                        p->spec.initial_status = 0x84;
                        p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                        p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                    }
                    break;
                }
                p->spec.sect_size = 0x010000;
                p->spec.page_size = 0x0100;
                p->spec.addr_width = 3;
                p->spec.program_page = 5;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SECTOR;
            }
            else if (device == CARD_BACKUP_TYPE_DEVICE_FRAM)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x002000:        // #CARD_BACKUP_TYPE_FRAM_64KBITS
                case 0x008000:        // #CARD_BACKUP_TYPE_FRAM_256KBITS
                    break;
                }
                p->spec.page_size = size;
                p->spec.sect_size = size;
                p->spec.addr_width = 2;
                p->spec.initial_status = 0x00;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
            }
            else
            {
              invalid_type:
                p->type = CARD_BACKUP_TYPE_NOT_USE;
                p->spec.total_size = 0;
                cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
                return;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_IdentifyBackupCore

  Description:  �f�o�C�X�^�C�v���菈���{�́B
                �����I���邢�͔񓯊��I�ɌĂяo�����B

  Arguments:    p          ���C�u�����̃��[�N�o�b�t�@ (�����̂��߂Ɉ����n��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_IdentifyBackupCore(CARDiCommon * p)
{
    (void)CARDi_Request(p, CARD_REQ_IDENTIFY, 1);
    /*
     * �擪 1 BYTE �̃��[�h�R�}���h�𔭍s���Č��ʒl���擾����.
     * �ڐG�s��, �j��, �����̂����ꂩ�ł���Βl�Ɋւ�炸 TIMEOUT ���Ԃ�.
     * (TIMEOUT ��, �f�o�C�X��ʂɊւ�炸 Read-Status �R�}���h�Ŕ��f�\)
     */
    p->cmd->src = 0;
    p->cmd->dst = (u32)CARDi_backup_cache_page_buf;
    p->cmd->len = 1;
    (void)CARDi_Request(p, CARD_REQ_READ_BACKUP, 1);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_BeginBackupCommand

  Description:  �o�b�N�A�b�v����R�}���h�̊J�n����

  Arguments:    callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_BeginBackupCommand(MIDmaCallback callback, void *arg)
{
    SDK_USING_BACKUP();
    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
                  "[CARD] current locking target is not backup.");
    CARD_CheckEnabled();
    if ((CARDi_GetAccessLevel() & CARD_ACCESS_LEVEL_BACKUP) == 0)
    {
        OS_TPanic("this program cannot access CARD-backup!");
    }
    (void)CARDi_WaitForTask(&cardi_common, TRUE, callback, arg);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommand

  Description:  �f�[�^��]������R�}���h�̃��N�G�X�g�𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE
                req_type   �R�}���h���N�G�X�g�^�C�v
                req_retry  �R�}���h���N�G�X�g�̎��s�����g���C�ő��
                req_mode   �R�}���h���N�G�X�g�̓��샂�[�h

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
BOOL CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                MIDmaCallback callback, void *arg, BOOL is_async,
                                CARDRequest req_type, int req_retry, CARDRequestMode req_mode)
{
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);

    CARDi_BeginBackupCommand(callback, arg);

    {
        CARDiCommon *p = &cardi_common;
        p->src = src;
        p->dst = dst;
        p->len = len;
        p->req_type = req_type;
        p->req_retry = req_retry;
        p->req_mode = req_mode;
    }

    return CARDi_ExecuteOldTypeTask(CARDi_RequestStreamCommandCore, is_async);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_AccessStatus

  Description:  �X�e�[�^�X���[�h�܂��̓��C�g (�e�X�g�p)

  Arguments:    command    CARD_REQ_READ_STATUS�܂���CARD_REQ_WRITE_STATUS
                value      CARD_REQ_WRITE_STATUS�ł���Ώ������ޒl

  Returns:      ���������0�ȏ�̒l�A���s����Ε��̒l
 *---------------------------------------------------------------------------*/
int CARDi_AccessStatus(CARDRequest command, u8 value)
{
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);

    CARDi_BeginBackupCommand(NULL, NULL);

    // �^�X�N���������Ƀe���|�����o�b�t�@���g�p�B
    CARDi_backup_cache_page_buf[0] = value;
    CARDi_backup_cache_page_buf[1] = (u8)command;

    return CARDi_ExecuteOldTypeTask(CARDi_AccessStatusCore, FALSE) ?
           CARDi_backup_cache_page_buf[0] : -1;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestWriteSectorCommand

  Description:  �Z�N�^���� + �v���O�����̃��N�G�X�g�𔭍s

  Arguments:    src        �]�����������A�h���X
                dst        �]����I�t�Z�b�g
                len        �]���T�C�Y
                verify     �x���t�@�C���s���ꍇ�� TRUE
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
BOOL CARDi_RequestWriteSectorCommand(u32 src, u32 dst, u32 len, BOOL verify,
                                     MIDmaCallback callback, void *arg, BOOL is_async)
{
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);

    CARDi_BeginBackupCommand(callback, arg);

    {
        CARDiCommon *p = &cardi_common;
        p->src = src;
        p->dst = dst;
        p->len = len;
        p->req_mode = verify ? CARD_REQUEST_MODE_SEND_VERIFY : CARD_REQUEST_MODE_SEND;
    }

    return CARDi_ExecuteOldTypeTask(CARDi_RequestWriteSectorCommandCore, is_async);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_IdentifyBackup

  Description:  �J�[�h�̃o�b�N�A�b�v�f�o�C�X��ʂ��w��

  Arguments:    type        CARDBackupType�^�̃f�o�C�X���

  Returns:      �ǂݏo���e�X�g�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL CARD_IdentifyBackup(CARDBackupType type)
{
    if (type == CARD_BACKUP_TYPE_NOT_USE)
    {
        OS_TPanic("cannot specify CARD_BACKUP_TYPE_NOT_USE.");
    }

    CARDi_BeginBackupCommand(NULL, NULL);

    CARDi_IdentifyBackupCore2(type);

    return CARDi_ExecuteOldTypeTask(CARDi_IdentifyBackupCore, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCurrentBackupType

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X��ʂ��擾

  Arguments:    None.

  Returns:      ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X���
 *---------------------------------------------------------------------------*/
CARDBackupType CARD_GetCurrentBackupType(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->type;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupTotalSize

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X�̑S�̃T�C�Y���擾

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�̑S�̃T�C�Y
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupTotalSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.total_size;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupSectorSize

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X�̃Z�N�^�T�C�Y���擾

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�̃Z�N�^�T�C�Y
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupSectorSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.sect_size;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupPageSize

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X�̃y�[�W�T�C�Y���擾

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�̃y�[�W�T�C�Y
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupPageSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.page_size;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WaitBackupAsync

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐�����������܂őҋ@

  Arguments:    None.

  Returns:      �Ō�ɌĂяo�����o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐���
                CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
BOOL CARD_WaitBackupAsync(void)
{
    return CARDi_WaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐�����������������

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐����������Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL CARD_TryWaitBackupAsync(void)
{
    return CARDi_TryWaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARD_CancelBackupAsync

  Description:  �������̃o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐��ɒ��~��v������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_CancelBackupAsync(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    cardi_common.flag |= CARD_STAT_CANCEL;
    (void)OS_RestoreInterrupts(bak_cpsr);
}
