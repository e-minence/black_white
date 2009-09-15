/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_command.h

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-04-21#$
  $Rev: 5625 $
  $Author: yosizaki $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_LIBRARIES_CARD_COMMAND_H__
#define NITRO_LIBRARIES_CARD_COMMAND_H__


#include <nitro/card/common.h>
#include <nitro/card/backup.h>

#include <nitro/pxi.h>
#include <nitro/os.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// �o�b�N�A�b�v�f�o�C�X�@�\ (�����g�p)
#define CARD_BACKUP_CAPS_AVAILABLE          (u32)(CARD_BACKUP_CAPS_READ - 1)
#define CARD_BACKUP_CAPS_READ               (u32)(1 << CARD_REQ_READ_BACKUP)
#define CARD_BACKUP_CAPS_WRITE              (u32)(1 << CARD_REQ_WRITE_BACKUP)
#define CARD_BACKUP_CAPS_PROGRAM            (u32)(1 << CARD_REQ_PROGRAM_BACKUP)
#define CARD_BACKUP_CAPS_VERIFY             (u32)(1 << CARD_REQ_VERIFY_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_PAGE         (u32)(1 << CARD_REQ_ERASE_PAGE_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_SECTOR       (u32)(1 << CARD_REQ_ERASE_SECTOR_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_CHIP         (u32)(1 << CARD_REQ_ERASE_CHIP_BACKUP)
#define CARD_BACKUP_CAPS_READ_STATUS        (u32)(1 << CARD_REQ_READ_STATUS)
#define CARD_BACKUP_CAPS_WRITE_STATUS       (u32)(1 << CARD_REQ_WRITE_STATUS)
#define CARD_BACKUP_CAPS_ERASE_SUBSECTOR    (u32)(1 << CARD_REQ_ERASE_SUBSECTOR_BACKUP)


/*---------------------------------------------------------------------------*/
/* declarations */

// PXI �R�}���h���N�G�X�g����M�p�����[�^.
typedef struct CARDiCommandArg
{
    CARDResult result;                 /* �߂�l */
    CARDBackupType type;               /* �f�o�C�X�^�C�v */
    u32     id;                        /* �J�[�h ID */
    u32     src;                       /* �]���� */
    u32     dst;                       /* �]���� */
    u32     len;                       /* �]���� */

    struct
    {
        /* memory property */
        u32     total_size;            /* memory capacity (BYTE) */
        u32     sect_size;             /* write-sector (BYTE) */
        u32     subsect_size;          /* write-sector (BYTE) */
        u32     page_size;             /* write-page (BYTE) */
        u32     addr_width;            /* address width on command (BYTE) */
        /* maximun wait-time (as LCD-V-COUNT tick) */
        u32     program_page;
        u32     write_page;
        u32     write_page_total;
        u32     erase_chip;
        u32     erase_chip_total;
        u32     erase_sector;
        u32     erase_sector_total;
        u32     erase_subsector;
        u32     erase_subsector_total;
        u32     erase_page;
        /* initial parameter of status register */
        u8      initial_status;
        u8      padding1[3];
        u32     caps;
        u8      padding2[4];
    }
    spec;

}
CARDiCommandArg;

SDK_COMPILER_ASSERT(sizeof(CARDiCommandArg) % 32 == 0);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_SendPxi

  Description:  PXI FIFO ���[�h���M

  Arguments:    data       ���M�f�[�^

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_SendPxi(u32 data)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_FS, data, TRUE) < 0)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnFifoRecv

  Description:  PXI FIFO ���[�h��M�R�[���o�b�N

  Arguments:    tag        PXI �^�O (��� PXI_FIFO_TAG_FS)
                data       ��M�f�[�^
                err        �G���[�r�b�g (���d�l�ɂ�����)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_LIBRARIES_CARD_COMMAND_H__
