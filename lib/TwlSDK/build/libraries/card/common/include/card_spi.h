/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_spi.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_LIBRARIES_CARD_SPI_H__
#define NITRO_LIBRARIES_CARD_SPI_H__


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

#define	CSPI_CONTINUOUS_ON	0x0040
#define	CSPI_CONTINUOUS_OFF	0x0000

/* �R�}���h */
#define COMM_WRITE_STATUS	0x01   /* �X�e�[�^�X���C�g. */
#define COMM_PROGRAM_PAGE	0x02   /* �y�[�W�v���O����. */
#define COMM_READ_ARRAY		0x03   /* ���[�h. */
#define COMM_WRITE_DISABLE	0x04   /* ���C�g�f�B�Z�[�u��. (���g�p) */
#define COMM_READ_STATUS	0x05   /* �X�e�[�^�X���[�h. */
#define COMM_WRITE_ENABLE	0x06   /* ���C�g�C�l�[�u��. */

/* FLASH */
#define COMM_PAGE_WRITE		0x0A
#define COMM_PAGE_ERASE		0xDB
#define COMM_SECTOR_ERASE	0xD8
#define COMM_SUBSECTOR_ERASE	0x20
#define COMM_CHIP_ERASE		0xC7
#define CARDFLASH_READ_BYTES_FAST	0x0B    /* ���g�p */
#define CARDFLASH_DEEP_POWDOWN		0xB9    /* ���g�p */
#define CARDFLASH_WAKEUP			0xAB    /* ���g�p */

/* �X�e�[�^�X���W�X�^�̃r�b�g */
#define	COMM_STATUS_WIP_BIT		0x01    /* WriteInProgress (bufy) */
#define	COMM_STATUS_WEL_BIT		0x02    /* WriteEnableLatch */
#define	COMM_STATUS_BP0_BIT		0x04
#define	COMM_STATUS_BP1_BIT		0x08
#define	COMM_STATUS_BP2_BIT		0x10
#define COMM_STATUS_WPBEN_BIT	0x80


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitStatusRegister

  Description:  FRAM/EEPROM �̏ꍇ�͋N�����ɃX�e�[�^�X���W�X�^�̕␳�������s���B
                (FRAM�͓d���������Ƀ��C�g�v���e�N�g�̕ω����N���肤�邽��)
                (EEPROM�͔[�����ɏ����l�s�������肤�邽��)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_InitStatusRegister(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandReadStatus

  Description:  ���[�h�X�e�[�^�X

  Arguments:    None

  Returns:      �X�e�[�^�X�l
 *---------------------------------------------------------------------------*/
u8 CARDi_CommandReadStatus(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackupCore

  Description:  �f�o�C�X�ւ̃��[�h�R�}���h�S��

  Arguments:    src               �ǂݏo�����̃f�o�C�X�I�t�Z�b�g
                dst               �ǂݏo����̃������A�h���X
                len               �ǂݏo���T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadBackupCore(u32 src, void *dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackupCore

  Description:  �f�o�C�X�ւ̃v���O����(����������������)�R�}���h�S��

  Arguments:    dst               �������ݐ�̃f�o�C�X�I�t�Z�b�g
                src               �������݌��̃������A�h���X
                len               �������݃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ProgramBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackupCore

  Description:  �f�o�C�X�ւ̃��C�g(���� + �v���O����)�R�}���h�S��

  Arguments:    dst               �������ݐ�̃f�o�C�X�I�t�Z�b�g
                src               �������݌��̃������A�h���X
                len               �������݃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_WriteBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackupCore

  Description:  �f�o�C�X�ւ̃x���t�@�C(���ۂ̓��[�h + ��r����)�R�}���h�S��

  Arguments:    dst               ��r��̃f�o�C�X�I�t�Z�b�g
                src               ��r���̃������A�h���X
                len               ��r�T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_VerifyBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSectorCore

  Description:  �f�o�C�X�ւ̃Z�N�^�����R�}���h�S��

  Arguments:    dst               ������̃f�o�C�X�I�t�Z�b�g
                len               �����T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseBackupSectorCore(u32 dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSubSectorCore

  Description:  �f�o�C�X�ւ̃T�u�Z�N�^�����R�}���h�S��.

  Arguments:    dst               ������̃f�o�C�X�I�t�Z�b�g.
                len               �����T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseBackupSubSectorCore(u32 dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseChipCore

  Description:  �f�o�C�X�ւ̃`�b�v�����R�}���h�S��

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseChipCore(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetWriteProtectCore

  Description:  �f�o�C�X�ւ̃��C�g�v���e�N�g�R�}���h�S��

  Arguments:    stat              �ݒ肷��v���e�N�g�t���O

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_SetWriteProtectCore(u16 stat);


#ifdef __cplusplus
} // extern "C"
#endif



#endif // NITRO_LIBRARIES_CARD_SPI_H__
