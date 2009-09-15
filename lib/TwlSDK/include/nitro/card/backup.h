/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     backup.h

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
#ifndef NITRO_CARD_BACKUP_H_
#define NITRO_CARD_BACKUP_H_


#include <nitro/card/types.h>

#include <nitro/mi/dma.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// �o�b�N�A�b�v�f�o�C�X�^�C�v
#define	CARD_BACKUP_TYPE_DEVICE_SHIFT	0
#define	CARD_BACKUP_TYPE_DEVICE_MASK	0xFF
#define	CARD_BACKUP_TYPE_DEVICE_EEPROM	1
#define	CARD_BACKUP_TYPE_DEVICE_FLASH	2
#define	CARD_BACKUP_TYPE_DEVICE_FRAM	3
#define	CARD_BACKUP_TYPE_SIZEBIT_SHIFT	8
#define	CARD_BACKUP_TYPE_SIZEBIT_MASK	0xFF
#define	CARD_BACKUP_TYPE_VENDER_SHIFT	16
#define	CARD_BACKUP_TYPE_VENDER_MASK	0xFF
#define	CARD_BACKUP_TYPE_DEFINE(type, size, vender)	\
	(((CARD_BACKUP_TYPE_DEVICE_ ## type) << CARD_BACKUP_TYPE_DEVICE_SHIFT) |	\
	((size) << CARD_BACKUP_TYPE_SIZEBIT_SHIFT) |	\
	((vender) << CARD_BACKUP_TYPE_VENDER_SHIFT))

typedef enum CARDBackupType
{
    CARD_BACKUP_TYPE_EEPROM_4KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 9, 0),
    CARD_BACKUP_TYPE_EEPROM_64KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 13, 0),
    CARD_BACKUP_TYPE_EEPROM_512KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 16, 0),
    CARD_BACKUP_TYPE_EEPROM_1MBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 17, 0),
    CARD_BACKUP_TYPE_FLASH_2MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 18, 0),
    CARD_BACKUP_TYPE_FLASH_4MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 19, 0),
    CARD_BACKUP_TYPE_FLASH_8MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 20, 0),
    CARD_BACKUP_TYPE_FLASH_16MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 21, 0),
    CARD_BACKUP_TYPE_FLASH_64MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 23, 0),
    CARD_BACKUP_TYPE_FRAM_256KBITS = CARD_BACKUP_TYPE_DEFINE(FRAM, 15, 0),
    CARD_BACKUP_TYPE_NOT_USE = 0
}
CARDBackupType;

#define CARD_BACKUP_TYPE_FLASH_64MBITS_EX (CARDBackupType)CARD_BACKUP_TYPE_DEFINE(FLASH, 23, 1)


// (���C�u���������Ŏg�p)

// �R���|�[�l���g����� ensata �G�~�����[�^�ւ� PXI �R�}���h���N�G�X�g
typedef enum CARDRequest
{
    CARD_REQ_INIT = 0,                 /* initialize (setting from ARM9) */
    CARD_REQ_ACK,                      /* request done (acknowledge from ARM7) */
    CARD_REQ_IDENTIFY,                 /* CARD_IdentifyBackup */
    CARD_REQ_READ_ID,                  /* CARD_ReadRomID (TEG && ARM9) */
    CARD_REQ_READ_ROM,                 /* CARD_ReadRom (TEG && ARM9) */
    CARD_REQ_WRITE_ROM,                /* (reserved) */
    CARD_REQ_READ_BACKUP,              /* CARD_ReadBackup */
    CARD_REQ_WRITE_BACKUP,             /* CARD_WriteBackup */
    CARD_REQ_PROGRAM_BACKUP,           /* CARD_ProgramBackup */
    CARD_REQ_VERIFY_BACKUP,            /* CARD_VerifyBackup */
    CARD_REQ_ERASE_PAGE_BACKUP,        /* CARD_EraseBackupPage */
    CARD_REQ_ERASE_SECTOR_BACKUP,      /* CARD_EraseBackupSector */
    CARD_REQ_ERASE_CHIP_BACKUP,        /* CARD_EraseBackupChip */
    CARD_REQ_READ_STATUS,              /* CARD_ReadStatus */
    CARD_REQ_WRITE_STATUS,             /* CARD_WriteStatus */
    CARD_REQ_ERASE_SUBSECTOR_BACKUP,   /* CARD_EraseBackupSubSector */
    CARD_REQ_MAX
}
CARDRequest;

// �R�}���h���N�G�X�g�̓���^�C�v
typedef enum CARDRequestMode
{
    CARD_REQUEST_MODE_RECV,            /* �f�[�^��M */
    CARD_REQUEST_MODE_SEND,            /* �f�[�^���M (�P���̃x���t�@�C���܂�) */
    CARD_REQUEST_MODE_SEND_VERIFY,     /* �f�[�^���M + �x���t�@�C */
    CARD_REQUEST_MODE_SPECIAL          /* �Z�N�^�����Ȃǂ̓��ꑀ�� */
}
CARDRequestMode;

// ���g���C�ő��
#define	CARD_RETRY_COUNT_MAX	10

// PXI protocol definition
#define CARD_PXI_COMMAND_MASK              0x0000003f   // command part
#define CARD_PXI_COMMAND_SHIFT             0
#define CARD_PXI_COMMAND_PARAM_MASK        0x01ffffc0   // parameter part
#define CARD_PXI_COMMAND_PARAM_SHIFT       6

//---- PXI command
#define CARD_PXI_COMMAND_TERMINATE         0x0001       // arm9->arm7 terminate command
#define CARD_PXI_COMMAND_PULLED_OUT        0x0011       // arm7->arm9 pulled out message
#define CARD_PXI_COMMAND_RESET_SLOT        0x0002       // arm7->arm9 reset-slot message


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_IdentifyBackup

  Description:  �J�[�h�̃o�b�N�A�b�v�f�o�C�X��ʂ��w��

  Arguments:    type        CARDBackupType�^�̃f�o�C�X���

  Returns:      �ǂݏo���e�X�g�ɐ���������TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARD_IdentifyBackup(CARDBackupType type);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetCurrentBackupType

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X��ʂ��擾

  Arguments:    None.

  Returns:      ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X���
 *---------------------------------------------------------------------------*/
CARDBackupType CARD_GetCurrentBackupType(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupTotalSize

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X�̑S�̃T�C�Y���擾

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�̑S�̃T�C�Y
 *---------------------------------------------------------------------------*/
u32     CARD_GetBackupTotalSize(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupSectorSize

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X�̃Z�N�^�T�C�Y���擾

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�̃Z�N�^�T�C�Y
 *---------------------------------------------------------------------------*/
u32     CARD_GetBackupSectorSize(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupPageSize

  Description:  ���ݎw�肵�Ă���o�b�N�A�b�v�f�o�C�X�̃y�[�W�T�C�Y���擾

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�̃y�[�W�T�C�Y
 *---------------------------------------------------------------------------*/
u32     CARD_GetBackupPageSize(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_LockBackup

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�̂��߂ɃJ�[�h�o�X�����b�N����

  Arguments:    lock_id     OS_GetLockID�֐��Ŋm�ۂ����r������pID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_LockBackup(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockBackup

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�̂��߂Ƀ��b�N�����J�[�h�o�X���������

  Arguments:    lock_id     CARD_LockBackup�֐��Ŏg�p�����r������pID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_UnlockBackup(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐�����������������

  Arguments:    None.

  Returns:      �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐����������Ă����TRUE
 *---------------------------------------------------------------------------*/
BOOL    CARD_TryWaitBackupAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_WaitBackupAsync

  Description:  �o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐�����������܂őҋ@

  Arguments:    None.

  Returns:      �Ō�ɌĂяo�����o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐���
                CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
BOOL    CARD_WaitBackupAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_CancelBackupAsync

  Description:  �������̃o�b�N�A�b�v�f�o�C�X�A�N�Z�X�֐��ɒ��~��v������

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_CancelBackupAsync(void);


// internal chip command as Serial-Bus-Interface

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
BOOL    CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async,
                                   CARDRequest req_type, int req_retry, CARDRequestMode req_mode);

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
BOOL    CARDi_RequestWriteSectorCommand(u32 src, u32 dst, u32 len, BOOL verify,
                                        MIDmaCallback callback, void *arg, BOOL is_async);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackup

  Description:  �`�b�v�R�}���h "read" �𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ReadBackup(u32 src, void *dst, u32 len,
                                 MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len,
                                      callback, arg, is_async,
                                      CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackup

  Description:  �`�b�v�R�}���h "program" ���g�p����ǂݍ��݃R�}���h�𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ProgramBackup(u32 dst, const void *src, u32 len,
                                    MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackup

  Description:  �`�b�v�R�}���h "write" ���g�p���鏑�����݃R�}���h�𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteBackup(u32 dst, const void *src, u32 len,
                                  MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackup

  Description:  �`�b�v�R�}���h "read" ���g�p����x���t�@�C�R�}���h�𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_VerifyBackup(u32 dst, const void *src, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_VERIFY_BACKUP, 1, CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramAndVerifyBackup

  Description:  �`�b�v�R�}���h "program" ���g�p����v���O���� + �x���t�@�C�R�}���h�𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len,
                                             MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteAndVerifyBackup

  Description:  �`�b�v�R�}���h "write" ���g�p����v���O���� + �x���t�@�C�R�}���h�𔭍s

  Arguments:    src        �]�����I�t�Z�b�g�܂��̓������A�h���X
                dst        �]����I�t�Z�b�g�܂��̓������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteAndVerifyBackup(u32 dst, const void *src, u32 len,
                                           MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSector

  Description:  �`�b�v�R�}���h "erase sector" ���g�p����Z�N�^�����R�}���h�𔭍s

  Arguments:    dst        ������I�t�Z�b�g
                len        �����T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupSector(u32 dst, u32 len,
                                        MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSubSector

  Description:  �`�b�v�R�}���h "erase subsector" ���g�p����Z�N�^�����R�}���h�𔭍s.

  Arguments:    dst        ������I�t�Z�b�g
                len        �����T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ���������������� TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupSubSector(u32 dst, u32 len,
                                           MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SUBSECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupChip

  Description:  �`�b�v�R�}���h "erase chip" ���g�p����Z�N�^�����R�}���h�𔭍s

  Arguments:    callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)
                is_async   �񓯊�������w�肳��Ă���� TRUE

  Returns:      ����������������TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupChip(MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, 0, 0, callback, arg, is_async,
                                      CARD_REQ_ERASE_CHIP_BACKUP, 1, CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadBackupAsync

  Description:  �񓯊��o�b�N�A�b�v���[�h (�`�b�v�R�}���h "read" �ɑ���)

  Arguments:    src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ReadBackupAsync(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_ReadBackup(src, dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadBackup

  Description:  �����o�b�N�A�b�v���[�h (�`�b�v�R�}���h "read" �ɑ���)

  Arguments:    src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ReadBackup(u32 src, void *dst, u32 len)
{
    return CARDi_ReadBackup(src, dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramBackupAsync

  Description:  �񓯊��o�b�N�A�b�v�v���O���� (�`�b�v�R�}���h "program" �ɑ���)

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ProgramBackupAsync(u32 dst, const void *src, u32 len,
                                        MIDmaCallback callback, void *arg)
{
    (void)CARDi_ProgramBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramBackup

  Description:  �����o�b�N�A�b�v�v���O���� (�`�b�v�R�}���h "program" �ɑ���)

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ProgramBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_ProgramBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupAsync

  Description:  �񓯊��o�b�N�A�b�v���C�g (�`�b�v�R�}���h "write" �ɑ���)

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteBackupAsync(u32 dst, const void *src, u32 len,
                                      MIDmaCallback callback, void *arg)
{
    (void)CARDi_WriteBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackup

  Description:  �����o�b�N�A�b�v���C�g (�`�b�v�R�}���h "write" �ɑ���)

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_WriteBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyBackupAsync

  Description:  �񓯊��o�b�N�A�b�v�x���t�@�C (�`�b�v�R�}���h "read" �ɑ���)

  Arguments:    src        ��r���I�t�Z�b�g
                dst        ��r�惁�����A�h���X
                len        ��r�T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_VerifyBackupAsync(u32 dst, const void *src, u32 len,
                                       MIDmaCallback callback, void *arg)
{
    (void)CARDi_VerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyBackup

  Description:  �����o�b�N�A�b�v�x���t�@�C (�`�b�v�R�}���h "read" �ɑ���)

  Arguments:    src        ��r���I�t�Z�b�g
                dst        ��r�惁�����A�h���X
                len        ��r�T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_VerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_VerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramAndVerifyBackupAsync

  Description:  �񓯊��o�b�N�A�b�v�v���O���� + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ProgramAndVerifyBackupAsync(u32 dst, const void *src, u32 len,
                                                 MIDmaCallback callback, void *arg)
{
    (void)CARDi_ProgramAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramAndVerifyBackup

  Description:  �����o�b�N�A�b�v�v���O���� + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_ProgramAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupAsync

  Description:  �񓯊��o�b�N�A�b�v���C�g + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteAndVerifyBackupAsync(u32 dst, const void *src, u32 len,
                                               MIDmaCallback callback, void *arg)
{
    (void)CARDi_WriteAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackup

  Description:  �����o�b�N�A�b�v���C�g + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteAndVerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_WriteAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSectorAsync

  Description:  �񓯊��Z�N�^����

  Arguments:    dst        ������I�t�Z�b�g
                len        �����T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupSectorAsync(u32 dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupSector(dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSector

  Description:  �����Z�N�^����

  Arguments:    dst        ������I�t�Z�b�g
                len        �����T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupSector(u32 dst, u32 len)
{
    return CARDi_EraseBackupSector(dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSubSectorAsync

  Description:  �񓯊��T�u�Z�N�^����.

  Arguments:    dst        ������I�t�Z�b�g
                len        �����T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupSubSectorAsync(u32 dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupSubSector(dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSubSector

  Description:  �����T�u�Z�N�^����.

  Arguments:    dst        ������I�t�Z�b�g
                len        �����T�C�Y

  Returns:      CARD_RESULT_SUCCESS �Ŋ�������� TRUE, ����ȊO�� FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupSubSector(u32 dst, u32 len)
{
    return CARDi_EraseBackupSubSector(dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupChipAsync

  Description:  �񓯊��`�b�v����

  Arguments:    callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupChipAsync(MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupChip(callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupChip

  Description:  �����Z�N�^����

  Arguments:    None.

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupChip(void)
{
    return CARDi_EraseBackupChip(NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupSectorAsync

  Description:  �Z�N�^�P�ʏ��� + �Z�N�^�P�ʃv���O����

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteBackupSectorAsync(u32 dst, const void *src, u32 len,
                                            MIDmaCallback callback, void *arg)
{
    (void)CARDi_RequestWriteSectorCommand((u32)src, dst, len, FALSE, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupSector

  Description:  �Z�N�^�P�ʏ��� + �Z�N�^�P�ʃv���O����

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteBackupSector(u32 dst, const void *src, u32 len)
{
    return CARDi_RequestWriteSectorCommand((u32)src, dst, len, FALSE, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupSectorAsync

  Description:  �Z�N�^�P�ʏ��� + �Z�N�^�P�ʃv���O���� + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteAndVerifyBackupSectorAsync(u32 dst, const void *src, u32 len,
                                                     MIDmaCallback callback, void *arg)
{
    (void)CARDi_RequestWriteSectorCommand((u32)src, dst, len, TRUE, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupSector

  Description:  �Z�N�^�P�ʏ��� + �Z�N�^�P�ʃv���O���� + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteAndVerifyBackupSector(u32 dst, const void *src, u32 len)
{
    return CARDi_RequestWriteSectorCommand((u32)src, dst, len, TRUE, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_AccessStatus

  Description:  �X�e�[�^�X���[�h�܂��̓��C�g (�e�X�g�p)

  Arguments:    command    CARD_REQ_READ_STATUS�܂���CARD_REQ_WRITE_STATUS
                value      CARD_REQ_WRITE_STATUS�ł���Ώ������ޒl

  Returns:      ���������0�ȏ�̒l�A���s����Ε��̒l
 *---------------------------------------------------------------------------*/
int CARDi_AccessStatus(CARDRequest command, u8 value);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadStatus

  Description:  �X�e�[�^�X���[�h (�e�X�g�p)

  Arguments:    None.

  Returns:      ���������0�ȏ�̒l�A���s����Ε��̒l
 *---------------------------------------------------------------------------*/
SDK_INLINE int CARDi_ReadStatus(void)
{
    return CARDi_AccessStatus(CARD_REQ_READ_STATUS, 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteStatus

  Description:  �X�e�[�^�X���C�g (�e�X�g�p)

  Arguments:    value      �������ޒl

  Returns:      CARD_RESULT_SUCCESS �Ŋ�������� TRUE, ����ȊO�� FALSE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteStatus(u8 value)
{
    return (CARDi_AccessStatus(CARD_REQ_WRITE_STATUS, value) >= 0);
}


#ifdef __cplusplus
}
#endif // extern "C"


#endif // NITRO_CARD_BACKUP_H_
