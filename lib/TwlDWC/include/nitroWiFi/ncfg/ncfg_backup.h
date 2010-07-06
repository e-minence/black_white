/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NCFG - include
  File:     ncfg.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef TWLWIFI_NCFG_BACKUP_H_
#define TWLWIFI_NCFG_BACKUP_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitro/types.h>

/*---------------------------------------------------------------------------*
    �萔 ��`
 *---------------------------------------------------------------------------*/
typedef enum
{
    NCFG_RESULT_SUCCESS        = 0,
    NCFG_RESULT_FAILURE        = -1,

    NCFG_RESULT_INIT_OK          =      0,     // ����I��
    NCFG_RESULT_INIT_OK_ERASE    = -10002,     // �f�[�^��������������I��
    NCFG_RESULT_INIT_OK_INIT     = -10003,     // ���[�UID����������������I��
    NCFG_RESULT_INIT_ERROR_WRITE = -10000,     // �o�b�N�A�b�v�������������݃G���[
    NCFG_RESULT_INIT_ERROR_READ  = -10001      // �o�b�N�A�b�v�������ǂݍ��݃G���[
} NCFGResultCode;

#define NCFG_SLOT_EX_MASK 0x0010
typedef enum
{
    NCFG_SLOT_1    = 0,
    NCFG_SLOT_2    = 1,
    NCFG_SLOT_3    = 2,
    NCFG_SLOT_RESERVED = 3,
    NCFG_SLOT_EX_1 = (NCFG_SLOT_EX_MASK | 1),
    NCFG_SLOT_EX_2 = (NCFG_SLOT_EX_MASK | 2),
    NCFG_SLOT_EX_3 = (NCFG_SLOT_EX_MASK | 3)
} NCFGSlotIndex;

#define NCFG_SLOT_SIZE 256
#define NCFG_SLOT_EX_SIZE 512

#define NCFG_NVRAM_WRITE_RETRY_MAX 3

typedef enum
{
    NCFG_FORMAT_VERSION_UNKNOWN = 0x00,
    NCFG_FORMAT_VERSION_NITRO   = 0x10,
    NCFG_FORMAT_VERSION_TWL     = 0x20
} NCFGFormatVersion;


/*---------------------------------------------------------------------------*
    �}�N�� ��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �\���� ��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �֐� ��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         NCFGi_InitBackupMemory

  Description:  NVRAM �Ƃ̂��Ƃ�̏��������s��
                �֘A����֐����Ăяo�����ۂɎ����I�ɌĂ΂��̂�
                �����I�ȌĂяo���͕s�v

  Arguments:    �Ȃ�

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_SUCCESS, NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 NCFGi_InitBackupMemory(void);

/*---------------------------------------------------------------------------*
  Name:         NCFG_GetFormatVersion

  Description:  NVRAM �Ɋi�[����Ă���l�b�g���[�N�ݒ�̃t�H�[�}�b�g��
                �o�[�W�������擾����B

  Arguments:    �Ȃ�

  Returns:      u8      -   �l�b�g���[�N�ݒ�̃t�H�[�}�b�g�o�[�W����
                            ��ʃj�u�������W���[�o�[�W����
                            ���ʃj�u�����}�C�i�[�o�[�W����
                            �G���[�̏ꍇ�� 0
 *---------------------------------------------------------------------------*/
u8 NCFG_GetFormatVersion(void);

/*---------------------------------------------------------------------------*
  Name:         NCFG_ReadBackupMemory

  Description:  NVRAM ���琶�̐ݒ�f�[�^��ǂݍ���
                �ǂݍ��ރf�[�^�͋����I�ɃL���b�V�������������̂Œ���

  Arguments:    buf   - �f�[�^�i�[�o�b�t�@ (�L���b�V���Z�[�t�ł���K�v������)
                size  - �f�[�^�i�[�o�b�t�@�̃T�C�Y
                index - �ǂݍ��ރf�[�^�̔ԍ�

  Returns:      s32     -   �ǂݍ��񂾃f�[�^�̃T�C�Y�i�񕉂̐����l�j���A
                            �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 NCFG_ReadBackupMemory(void* buf, u32 size, s32 index);

/*---------------------------------------------------------------------------*
  Name:         NCFG_WriteBackupMemory

  Description:  NVRAM �ɐ��̐ݒ�f�[�^����������
                �������ރf�[�^�͋����I�ɃL���b�V�������������̂Œ���

  Arguments:    index - �������ރf�[�^�̔ԍ�
                buf   - �f�[�^�i�[�o�b�t�@
                size  - �f�[�^�i�[�o�b�t�@�̃T�C�Y

  Returns:      s32     -   �������񂾃f�[�^�̃T�C�Y�i�񕉂̐����l�j���A
                            �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 NCFG_WriteBackupMemory(s32 index, const void* buf, u32 size);


/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* TWLWIFI_NCFG_BACKUP_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
