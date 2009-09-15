/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS - include
  File:     ownerInfoEx.h

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

#ifndef TWL_OS_COMMON_OWNERINFO_EX_H_
#define TWL_OS_COMMON_OWNERINFO_EX_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include    <twl/types.h>
#include    <twl/spec.h>
#include    <nitro/std.h>
#ifndef SDK_TWL
#include    <nitro/hw/common/mmap_shared.h>
#else
#include    <twl/hw/common/mmap_shared.h>
#endif

#include <nitro/os/common/ownerInfo.h>


/*---------------------------------------------------------------------------*
    �萔
 *---------------------------------------------------------------------------*/
// ���[�W�����R�[�h
typedef enum OSTWLRegionCode
{
    OS_TWL_REGION_JAPAN     = 0,
    OS_TWL_REGION_AMERICA   = 1,
    OS_TWL_REGION_EUROPE    = 2,
    OS_TWL_REGION_AUSTRALIA = 3,
    OS_TWL_REGION_CHINA     = 4,
    OS_TWL_REGION_KOREA     = 5,
    OS_TWL_REGION_MAX
} OSTWLRegion;

#define OS_TWL_HWINFO_SERIALNO_LEN_MAX        15                                // �{�̃V���A��No.��Max(�I�[�t���Ȃ̂ŁA14bytes�܂Ŋg����)
#define OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN   16                                // �ڍs�\�ȃ��j�[�NID
#define OS_TWL_NICKNAME_LENGTH                OS_OWNERINFO_NICKNAME_MAX         // �j�b�N�l�[����
#define OS_TWL_COMMENT_LENGTH                 OS_OWNERINFO_COMMENT_MAX          // �R�����g��
#define OS_TWL_PCTL_PASSWORD_LENGTH           4                                 // �Ïؔԍ��̌���
#define OS_TWL_PCTL_SECRET_ANSWER_LENGTH_MAX  64                                // �閧�̎���̉񓚁AUTF-16�ōő� MAX ����


/*---------------------------------------------------------------------------*
    �\����
 *---------------------------------------------------------------------------*/

// TWL�I�[�i���
typedef struct OSOwnerInfoEx
{
    u8      language;                  // ����R�[�h
    u8      favoriteColor;             // ���C�ɓ���̐F ( 0 �` 15 )
    OSBirthday birthday;               // �a����
    u16     nickName[OS_OWNERINFO_NICKNAME_MAX + 1];
    // �j�b�N�l�[�� (Unicode(UTF16)�ōő�10�����A�I�[�R�[�h�Ȃ�)
    u16     nickNameLength;            // �j�b�N�l�[��������( 0 �` 10 )
    u16     comment[OS_OWNERINFO_COMMENT_MAX + 1];
    // �R�����g (Unicode(UTF16)�ōő�26�����A�I�[�R�[�h�Ȃ�)
    u16     commentLength;             // �R�����g������( 0 �` 26 )
    // TWL�݂̂Ŏ擾�\�ȃf�[�^
    u8      country;                   // ������ђn��R�[�h
    u8      padding;

}
OSOwnerInfoEx;


// �y�A�����^���R���g���[��
// �R���c��
typedef enum OSTWLRatingOgn
{
    OS_TWL_PCTL_OGN_CERO        =   0,  // CERO                         // ���{
    OS_TWL_PCTL_OGN_ESRB        =   1,  // ESRB                         // �k��
    OS_TWL_PCTL_OGN_RESERVED2   =   2,  // BBFC [obsolete]
    OS_TWL_PCTL_OGN_USK         =   3,  // USK                          // �h�C�c
    OS_TWL_PCTL_OGN_PEGI_GEN    =   4,  // PEGI general                 // ���B
    OS_TWL_PCTL_OGN_RESERVED5   =   5,  // PEGI Finland [obsolete]
    OS_TWL_PCTL_OGN_PEGI_PRT    =   6,  // PEGI Portugal                // �|���g�K��
    OS_TWL_PCTL_OGN_PEGI_BBFC   =   7,  // PEGI and BBFC Great Briten   // �C�M���X
    OS_TWL_PCTL_OGN_AGCB        =   8,  // AGCB                         // �I�[�X�g�����A�A�j���[�W�[�����h
    OS_TWL_PCTL_OGN_GRB         =   9,  // GRB                          // �؍�

    OS_TWL_PCTL_OGN_MAX         =   16

} OSTWLRatingOgn;


// �y�A�����^���R���g���[�����
typedef struct OSTWLParentalControl
{
    struct {
        u32             isSetParentalControl : 1;   // �y�A�����^���R���g���[���ݒ�ς݂��H
        u32             pictoChat :1;               // �s�N�g�`���b�g���N���������邩�H
        u32             dsDownload :1;              // DS�_�E�����[�h�v���C���N���������邩�H
        u32             browser :1;                 // �t���u���E�U���N���������邩�H
        u32             prepaidPoint :1;            // �|�C���g���g�p�������邩�H
        u32             photoExchange : 1;          // �ʐ^�̌����𐧌����邩�H
        u32             ugc : 1;                    // ���[�U�[�쐬�R���e���c�̐��������邩�H
        u32             rsv :25;
    } flags;
    u8                  rsv1[ 3 ];
    u8                  ogn;                        // �R���c��
    u8                  ratingAge;                  // ���[�e�B���O�i�N��j�l
    u8                  secretQuestionID;           // �閧�̎��╶ ID
    u8                  secretAnswerLength;         // �閧�̎���ւ̉񓚕�����
    u8                  rsv[ 2 ];
    char                password[ OS_TWL_PCTL_PASSWORD_LENGTH + 1 ];   // �Ïؔԍ��A�I�[�R�[�h����
    u16                 secretAnswer[ OS_TWL_PCTL_SECRET_ANSWER_LENGTH_MAX + 1 ];  // UTF16,�閧�̎���ւ̉񓚁A�I�[�R�[�h����
} OSTWLParentalControl;   //  148bytes

// �A�v���P�[�V�������̃��[�e�B���O���t�H�[�}�b�g
#define OS_TWL_PCTL_OGNINFO_ENABLE_MASK     0x80
#define OS_TWL_PCTL_OGNINFO_ALWAYS_MASK     0x40
#define OS_TWL_PCTL_OGNINFO_AGE_MASK        0x1f


/*---------------------------------------------------------------------------*
    �֐��錾
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         OS_GetOwnerInfoEx

  Description:  �I�[�i�[�����擾����BTWL�ŁB
                NITRO�ɂ����݂������NITRO�łƓ����ꏊ����擾����B
                ����R�[�h(laungage)��TWL��NITRO�ňقȂ�\�������邽��
                TWL���̏�񂩂�擾����B

  Arguments:    info    -   �I�[�i�[�����擾����o�b�t�@�ւ̃|�C���^�B
                            ���̃o�b�t�@�Ƀf�[�^���������܂��B
                            (*)TWL�̏���TWL���[�h�ł����擾�ł��Ȃ��B
                               (TWL��̓���łȂ��Ƃ����0�ƂȂ�B)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_GetOwnerInfoEx(OSOwnerInfoEx *info);

/*---------------------------------------------------------------------------*
  Name:         OS_IsAvailableWireless

  Description:  �������W���[����RF���j�b�g�̗L�����^�����������擾����B

  Arguments:    None.

  Returns:      �L���̂Ƃ�TRUE/�����̂Ƃ�FALSE�B
                TWL��̓���łȂ��Ƃ����TRUE�B
 *---------------------------------------------------------------------------*/
BOOL    OS_IsAvailableWireless(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsAgreeEULA

  Description:  ���̖{�̂�EULA���Ӎς݂��ǂ������擾����B

  Arguments:    None.

  Returns:      BOOL - EULA���Ӎς݂̎�TRUE�A�񓯈ӂ̎�FALSE�B
                TWL��̓���łȂ��Ƃ����FALSE�B
 *---------------------------------------------------------------------------*/
BOOL    OS_IsAgreeEULA(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetAgreeEULAVersion

  Description:  ���̖{�̂œ��ӂ���EULA�̃o�[�W�������擾����B

  Arguments:    None

  Returns:      agreedEulaVersion - ���ӂ���EULA�o�[�W�����i0-255�̐����j
 *---------------------------------------------------------------------------*/
u8    OS_GetAgreedEULAVersion( void );

/*---------------------------------------------------------------------------*
  Name:         OS_GetROMHeaderEULAVersion

  Description:  �A�v���P�[�V������ROM�w�b�_�ɓo�^����Ă���EULA�̃o�[�W�������擾����B

  Arguments:    None

  Returns:      cardEulaVersion - �A�v���P�[�V������ROM�w�b�_�ɓo�^����Ă���EULA�o�[�W�����i0-255�̐����j
 *---------------------------------------------------------------------------*/
u8    OS_GetROMHeaderEULAVersion( void );

/*---------------------------------------------------------------------------*
  Name:         OS_GetParentalControlPtr

  Description:  �y�A�����^���R���g���[�����ւ̃|�C���^���擾����B

  Arguments:    None.

  Returns:      �|�C���^��Ԃ��B
                TWL��̓���łȂ��Ƃ���NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
const OSTWLParentalControl* OS_GetParentalControlInfoPtr(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsParentalControledApp

  Description:  �A�v���P�[�V�����̋N���Ƀy�A�����^���R���g���[���������������
                ���ł��邩�ǂ������肷��B

  Arguments:    appRatingInfo   -   �A�v���P�[�V������ ROM �w�b�_���̃I�t�Z�b�g
                                    0x2f0 �ʒu�ɖ��ߍ��܂�Ă��� 16 �o�C�g�̃�
                                    �[�e�B���O���ւ̃|�C���^���w�肷��B

  Returns:      �y�A�����^���R���g���[�������������ăp�X���[�h���͂𑣂��ׂ���
                ���� TRUE ��Ԃ��B���������ɋN�����Ă悢�A�v���P�[�V�����ł���
                �ꍇ�� FALSE ��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL    OS_IsParentalControledApp(u8* appRatingInfo);

/*---------------------------------------------------------------------------*
  Name:         OS_GetMovableUniqueID

  Description:  �{�̊Ԃňڍs�\�ȃ��j�[�NID(HW���Ɋ܂܂��)���擾����B

  Arguments:    pUniqueID - �i�[��(OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN�o�C�g�i�[�����)�B
                         TWL��̓���łȂ��Ƃ����0�Ŗ��߂���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_GetMovableUniqueID(u8 *pUniqueID);

/*---------------------------------------------------------------------------*
  Name:         OS_GetUniqueIDPtr

  Description:  �{�̊Ԃňڍs�\�ȃ��j�[�NID(HW���Ɋ܂܂��)�ւ̃|�C���^���擾����B

  Arguments:    None.

  Returns:      �|�C���^��Ԃ��B
                TWL��̓���łȂ��Ƃ���NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
const u8* OS_GetMovableUniqueIDPtr(void);

/*---------------------------------------------------------------------------*
  Name:         OS_IsForceDisableWireless

  Description:  �����������I�ɖ��������邩�ǂ����̏��B
                (HW�Z�L���A���Ɋ܂܂��)���擾����B

  Arguments:    None.

  Returns:      �����I�ɖ���������Ƃ�TRUE/����������Ȃ��Ƃ�FALSE�B
                TWL��̓���łȂ��Ƃ����FALSE�B
 *---------------------------------------------------------------------------*/
BOOL    OS_IsForceDisableWireless(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetRegion

  Description:  ���[�W���������擾����B

  Arguments:    None.

  Returns:      ���[�W�����ԍ���Ԃ��B
                TWL��̓���łȂ��Ƃ����0��Ԃ��B
 *---------------------------------------------------------------------------*/
OSTWLRegion  OS_GetRegion(void);

/*---------------------------------------------------------------------------*
  Name:         OS_GetRegionCodeA3

  Description:  ���[�W�����R�[�h�ɑΉ����镶������擾����B

  Arguments:    region -   ���[�W�����R�[�h

  Returns:      ���[�W�����R�[�h�ɑΉ����镶����ւ̃|�C���^��Ԃ��B
 *---------------------------------------------------------------------------*/
const char* OS_GetRegionCodeA3(OSTWLRegion region);

/*---------------------------------------------------------------------------*
  Name:         OS_GetISOCountryCodeA2

  Description:  TWL�Ǝ��̍�����ђn��R�[�h�ԍ��� ISO 3166-1 alpha-2 �ɕϊ�����B

  Arguments:    twlCountry -   ������ђn��R�[�h

  Returns:      ������ђn��R�[�h�ɑΉ����镶����ւ̃|�C���^��Ԃ��B
 *---------------------------------------------------------------------------*/
const char* OS_GetISOCountryCodeA2(u8 twlCountry);

/*---------------------------------------------------------------------------*
  Name:         OS_IsRestrictPictoChatBoot

  Description:  �y�A�����^���R���g���[���̐ݒ�̂����A
                �s�N�g�`���b�g�̋N���������������Ă��邩�ǂ����𔻒肷��B

  Arguments:    None.

  Returns:      �������������Ă���ꍇ�� TRUE, �������Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_IsRestrictPictoChatBoot( void )
{
    return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl && (BOOL)OS_GetParentalControlInfoPtr()->flags.pictoChat;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsRestrictDSDownloadBoot

  Description:  �y�A�����^���R���g���[���̐ݒ�̂����A
                DS �_�E�����[�h�v���C�̋N���������������Ă��邩�ǂ����𔻒肷��B

  Arguments:    None.

  Returns:      �������������Ă���ꍇ�� TRUE�A�������Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_IsRestrictDSDownloadBoot( void )
{
    return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl && (BOOL)OS_GetParentalControlInfoPtr()->flags.dsDownload;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsRestrictBrowserBoot

  Description:  �y�A�����^���R���g���[���̐ݒ�̂����A
                DSi �u���E�U�[�̋N���������������Ă��邩�ǂ����𔻒肷��B

  Arguments:    None.

  Returns:      �������������Ă���ꍇ�� TRUE, �������Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_IsRestrictBrowserBoot( void )
{
    return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl && (BOOL)OS_GetParentalControlInfoPtr()->flags.browser;
}


/*---------------------------------------------------------------------------*
  Name:         OS_IsRestrictPrepaidPoint

  Description:  �y�A�����^���R���g���[���̐ݒ�̂����A
                �j���e���h�[ DSi �|�C���g�̎d�l�Ɋւ��鐧�����������Ă��邩�ǂ����𔻒肷��B

  Arguments:    None.

  Returns:      �������������Ă���ꍇ�� TRUE, �������Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_IsRestrictPrepaidPoint( void )
{
    return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl && (BOOL)OS_GetParentalControlInfoPtr()->flags.prepaidPoint;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsRestrictPhotoExchange

  Description:  �y�A�����^���R���g���[���̐ݒ�̂����A
                �ʐ^�f�[�^�̌����Ɋւ��鐧�����������Ă��邩�ǂ����𔻒肷��B

  Arguments:    None.

  Returns:      �������������Ă���ꍇ�� TRUE, �������Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_IsRestrictPhotoExchange( void )
{
    return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl && (BOOL)OS_GetParentalControlInfoPtr()->flags.photoExchange;
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsRestrictUGC

  Description:  �y�A�����^���R���g���[���̐ݒ�̂����A�ꕔ�̃��[�U�[�쐬�R���e���c��
                ����M�Ɋւ��鐧�����������Ă��邩�ǂ����𔻒肷��B

  Arguments:    None.

  Returns:      �������������Ă���ꍇ�� TRUE, �������Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_IsRestrictUGC( void )
{
    return (BOOL)OS_GetParentalControlInfoPtr()->flags.isSetParentalControl && (BOOL)OS_GetParentalControlInfoPtr()->flags.ugc;
}

/*---------------------------------------------------------------------------*
  Name:         OS_CheckParentalControlPassword

  Description:  �^����ꂽ�����񂪁A�y�A�����^���R���g���[���̉����p�X���[�h��
                ��v���邩�ǂ����𔻒肷��B

  Arguments:    password - ���肳��镶����.

  Returns:      ��v���Ă����ꍇ�� TRUE, ��v���Ă��Ȃ��ꍇ�� FALSE�B
 *---------------------------------------------------------------------------*/
static inline BOOL OS_CheckParentalControlPassword( const char *password )
{
    return password && (STD_CompareLString(password, OS_GetParentalControlInfoPtr()->password, OS_TWL_PCTL_PASSWORD_LENGTH) == 0);
}

/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* TWL_OS_COMMON_OWNERINFO_EX_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
