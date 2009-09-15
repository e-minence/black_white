/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     ownerInfoEx_private.h

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef TWL_OS_COMMON_OWNERINFO_PRIVATE_H_
#define TWL_OS_COMMON_OWNERINFO_PRIVATE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
    ��ʂ̊J���҂��ӎ����Ȃ��Ă悢�錾��ownerInfoEx.h���番��
    �����ɒ�`����Ă�����̂̓A�v�����璼�ڎg�p���Ȃ��ł�������
 *---------------------------------------------------------------------------*/

/*===========================================================================*/

#include    <twl/types.h>
#include    <twl/spec.h>
#ifndef SDK_TWL
#include    <nitro/hw/common/mmap_shared.h>
#else
#include    <twl/hw/common/mmap_shared.h>
#endif

#include <twl/os/common/ownerInfoEx.h>


/*---------------------------------------------------------------------------*
    ���C���������Ɋi�[�����`��
 *---------------------------------------------------------------------------*/


// TWL���t�f�[�^�ƌ݊����̂���\����
typedef struct OSTWLDate{
    u8              month;                      // ��:01�`12
    u8              day;                        // ��:01�`31
} OSTWLDate;          // 2byte

// TWL�I�[�i�[���ƌ݊����̂���\����
typedef struct OSTWLOwnerInfo
{
    u8              userColor : 4;              // �D���ȐF
    u8              rsv : 4;                    // �\��B
    u8              pad;                        // �p�f�B���O
    OSTWLDate       birthday;                   // ���N����
    u16             nickname[ OS_TWL_NICKNAME_LENGTH + 1 ];   // �j�b�N�l�[���i�I�[����j
    u16             comment[ OS_TWL_COMMENT_LENGTH + 1 ];     // �R�����g�i�I�[����j
} OSTWLOwnerInfo;     // 80  bytes


// TWL�ݒ�f�[�^�ƌ݊����̂���\����
typedef struct OSTWLSettingsData
{
    union {
        struct {
            u32     isFinishedInitialSetting : 1;       // ����ݒ�I���H
            u32     isFinishedInitialSetting_Launcher : 1;  // �����`���[�̏���ݒ�I���H
            u32     isSetLanguage : 1;                  // ����R�[�h�ݒ�ς݁H
            u32     isAvailableWireless : 1;            // �������W���[����RF���j�b�g�̗L�����^������
            u32     rsv : 20;                           // �\��
            u32     isAgreeEULAFlagList : 8;            // EULA���Ӄt���O���X�g
            // WiFi�ݒ�͕ʃf�[�^�Ȃ̂ŁA�����ɐݒ�ς݃t���O�͗p�ӂ��Ȃ��B
        };
        u32         raw;
    } flags;
    u8                      rsv[ 1 ];               // �\��
    u8                      country;                // ���R�[�h
    u8                      language;               // ����(NTR�Ƃ̈Ⴂ�́A�f�[�^�T�C�Y8bit)
    u8                      rtcLastSetYear;         // RTC�̑O��ݒ�N
    s64                     rtcOffset;              // RTC�ݒ莞�̃I�t�Z�b�g�l�i���[�U�[��RTC�ݒ��ύX����x�ɂ��̒l�ɉ����đ������܂��B�j
                                                    //   16bytes �����܂ł̃p�����[�^�T�C�Y
    u8                      agreeEulaVersion[ 8 ];  //    8bytes ���ӂ���EULA�̃o�[�W����
    u8                      pad1[2];
    u8                      pad2[6];                //    6bytes
    u8                      pad3[16];               //   16bytes
    u8                      pad4[20];               //   20bytes
    OSTWLOwnerInfo          owner;                  //   80bytes �I�[�i�[���
    OSTWLParentalControl    parentalControl;        //  148bytes �y�A�����^���R���g���[�����
} OSTWLSettingsData;  // 296bytes


// TWL_HW�m�[�}�����ݒ�f�[�^�ƌ݊����̂���\����
typedef struct OSTWLHWNormalInfo
{
    u8              rtcAdjust;                                  // RTC�����l
    u8              rsv[ 3 ];
    u8              movableUniqueID[ OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // �ڍs�\�ȃ��j�[�NID
} OSTWLHWNormalInfo;  // 20byte


// TWL_HW�Z�L���A���ݒ�f�[�^�ƌ݊����̂���\����
typedef struct OSTWLHWSecureInfo
{
    u32             validLanguageBitmap;                            // �{�̂ŗL���Ȍ���R�[�h���r�b�g��ŕ\��
    struct {
        u8          forceDisableWireless :1;
        u8          :7;
    }flags;
    u8              pad[ 3 ];
    u8              region;                                         // ���[�W����
    u8              serialNo[ OS_TWL_HWINFO_SERIALNO_LEN_MAX ];   // �V���A��No.�i�I�[�t��ASCII������j
} OSTWLHWSecureInfo;  // 24bytes


typedef struct OSTWLWirelessFirmwareData
{
    u8              data;
    u8              rsv[ 3 ];
} OSTWLWirelessFirmwareData;

/*---------------------------------------------------------------------------*
    �֐��錾
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         OS_GetWirelessWirmwareData

  Description:  �����t�@�[���E�F�A�p�f�[�^�ւ̃|�C���^���擾����B

  Arguments:    None.
  Returns:      �����t�@�[���E�F�A�p�f�[�^�ւ̃|�C���^��Ԃ��B
                TWL��̓���łȂ��Ƃ���NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
OSTWLWirelessFirmwareData *OS_GetWirelessFirmwareData(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetValidLanguageBitmap

  Description:  �Ή�����r�b�g�}�b�v���擾����B

  Arguments:    None.
  Returns:      �Ή�����r�b�g�}�b�v��Ԃ��B
                TWL��̓���łȂ��Ƃ���"0"��Ԃ��B
 *---------------------------------------------------------------------------*/
u32 OS_GetValidLanguageBitmap(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetSerialNo

  Description:  �V���A���i���o���擾����B

  Arguments:    serialNo - �i�[��B
                           �I�[�������܂߂�OS_TWL_HWINFO_SERIALNO_LEN_MAX�o�C�g�i�[�����B
                           TWL��̓���łȂ��Ƃ����0�Ŗ��߂���B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_GetSerialNo(u8 *serialNo);


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* TWL_OS_COMMON_OWNERINFO_EX_PRIVATE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
