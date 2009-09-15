/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     eeprom.h

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
#ifndef NITRO_CARD_EEPROM_H_
#define NITRO_CARD_EEPROM_H_


#include <nitro/card/backup.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_IsBackupEeprom

  Description:  ���ݐݒ肳��Ă���o�b�N�A�b�v�f�o�C�X��ʂ�EEPROM������

  Arguments:    None.

  Returns:      ���ݐݒ肳��Ă���o�b�N�A�b�v�f�o�C�X��ʂ�EEPROM�Ȃ�TRUE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_IsBackupEeprom(void)
{
    const CARDBackupType t = CARD_GetCurrentBackupType();
    return (((t >> CARD_BACKUP_TYPE_DEVICE_SHIFT) &
              CARD_BACKUP_TYPE_DEVICE_MASK) == CARD_BACKUP_TYPE_DEVICE_EEPROM);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadEeprom

  Description:  ����EEPROM�ǂݍ��� (�`�b�v�R�}���h "read" �ɑ���)

  Arguments:    src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ReadEeprom(u32 src, void *dst, u32 len)
{
    return CARD_ReadBackup(src, dst, len);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadEepromAsync

  Description:  �񓯊�EEPROM�ǂݍ��� (�`�b�v�R�}���h "read" �ɑ���)

  Arguments:    src        �]�����I�t�Z�b�g
                dst        �]���惁�����A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ReadEepromAsync(u32 src, void *dst, u32 len,
                                     MIDmaCallback callback, void *arg)
{
    CARD_ReadBackupAsync(src, dst, len, callback, arg);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteEeprom

  Description:  ����EEPROM�������� (�`�b�v�R�}���h "program" �ɑ���)

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteEeprom(u32 dst, const void *src, u32 len)
{
    return CARD_ProgramBackup(dst, src, len);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteFlashAsync

  Description:  �񓯊�EEPROM�������� (�`�b�v�R�}���h "write" �ɑ���)

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteEepromAsync(u32 dst, const void *src, u32 len,
                                      MIDmaCallback callback, void *arg)
{
    CARD_ProgramBackupAsync(dst, src, len, callback, arg);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyEeprom

  Description:  ����EEPROM�x���t�@�C

  Arguments:    dst        ��r��I�t�Z�b�g
                src        ��r���������A�h���X
                len        ��r�T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_VerifyEeprom(u32 dst, const void *src, u32 len)
{
    return CARD_VerifyBackup(dst, src, len);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyEepromAsync

  Description:  �񓯊�EEPROM�x���t�@�C

  Arguments:    dst        ��r��I�t�Z�b�g
                src        ��r���������A�h���X
                len        ��r�T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_VerifyEepromAsync(u32 dst, const void *src, u32 len,
                                       MIDmaCallback callback, void *arg)
{
    CARD_VerifyBackupAsync(dst, src, len, callback, arg);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyEeprom

  Description:  ����EEPROM�������� + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y

  Returns:      CARD_RESULT_SUCCESS�Ŋ��������TRUE�A����ȊO��FALSE
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteAndVerifyEeprom(u32 dst, const void *src, u32 len)
{
    return CARD_ProgramAndVerifyBackup(dst, src, len);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyEepromAsync

  Description:  �񓯊�EEPROM�������� + �x���t�@�C

  Arguments:    dst        �]����I�t�Z�b�g
                src        �]�����������A�h���X
                len        �]���T�C�Y
                callback   �����R�[���o�b�N (�s�g�p�Ȃ� NULL)
                arg        �����R�[���o�b�N�̈��� (�s�g�p�Ȃ疳�������)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteAndVerifyEepromAsync(u32 dst, const void *src, u32 len,
                                               MIDmaCallback callback, void *arg)
{
    CARD_ProgramAndVerifyBackupAsync(dst, src, len, callback, arg);
}


#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_CARD_EEPROM_H_
