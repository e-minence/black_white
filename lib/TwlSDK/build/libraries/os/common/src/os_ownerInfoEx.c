/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     ownerInfoEx.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-13#$
  $Rev: 9302 $
  $Author: hirose_kazuki $
 *---------------------------------------------------------------------------*/

#include    <nitro/os/common/ownerInfo.h>
#include    <nitro/os/common/systemWork.h>
#include    <nitro/os/common/emulator.h>
#include    <twl/os/common/ownerInfoEx.h>
#include    <twl/os/common/ownerInfoEx_private.h>
#include    <twl/hw/common/mmap_parameter.h>

#ifdef SDK_ARM9
#include    <twl/hw/ARM9/mmap_main.h>
#else //SDK_ARM7
#include    <twl/hw/ARM7/mmap_main.h>
#endif

/*---------------------------------------------------------------------------*
    �萔
 *---------------------------------------------------------------------------*/

//
// �I�[�i�[���̊e�l�̓��C���������̌Œ�A�h���X�Ƀ����`���ɂ���ēǂݏo����Ă���
#define  OS_ADDR_TWL_SETTINGSDATA   ( (OSTWLSettingsData *)HW_PARAM_TWL_SETTINGS_DATA )
#define  OS_ADDR_TWL_HWNORMALINFO   ( (OSTWLHWNormalInfo *)HW_PARAM_TWL_HW_NORMAL_INFO )


/*---------------------------------------------------------------------------*
    �֐���`
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
void OS_GetOwnerInfoEx(OSOwnerInfoEx *info)
{
    NVRAMConfig *src;
    OSTWLSettingsData  *twlSettings;

    // NITRO�Ɠ�������NITRO�̗̈悩��擾����(����ăR�s�y)
    src = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
    //info->language = (u8)(src->ncd.option.language);          // ����R�[�h��TWL���[�h��NITRO���[�h�ŏꍇ����
    info->favoriteColor = (u8)(src->ncd.owner.favoriteColor);
    info->birthday.month = (u8)(src->ncd.owner.birthday.month);
    info->birthday.day = (u8)(src->ncd.owner.birthday.day);
    info->nickNameLength = (u16)(src->ncd.owner.nickname.length);
    info->commentLength = (u16)(src->ncd.owner.comment.length);
    MI_CpuCopy16(src->ncd.owner.nickname.str,
                 info->nickName, OS_OWNERINFO_NICKNAME_MAX * sizeof(u16));
    MI_CpuCopy16(src->ncd.owner.comment.str, info->comment, OS_OWNERINFO_COMMENT_MAX * sizeof(u16));
    info->nickName[OS_OWNERINFO_NICKNAME_MAX] = 0;
    info->comment[OS_OWNERINFO_COMMENT_MAX] = 0;

    // TWL���̎擾
    if( OS_IsRunOnTwl() )
    {
        twlSettings   = (OSTWLSettingsData*) OS_ADDR_TWL_SETTINGSDATA ;
        info->country = twlSettings->country;
        info->language = twlSettings->language;
    }
    else
    {
        info->country = 0;
        info->language = (u8)(src->ncd.option.language);
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsAvailableWireless

  Description:  �������W���[����RF���j�b�g�̗L�����^�����������擾����B

  Arguments:    None.

  Returns:      �L���̂Ƃ�TRUE/�����̂Ƃ�FALSE�B
                TWL��̓���łȂ��Ƃ����TRUE�B
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static BOOL OS_IsAvailableWireless_ltdmain(void);
static BOOL OS_IsAvailableWireless_ltdmain(void)
{
    OSTWLSettingsData *p;
    p = (OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA;
    return (p->flags.isAvailableWireless == 0x1);
}
#include <twl/ltdmain_end.h>
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BOOL OS_IsAvailableWireless(void)
{
    BOOL result;
    if( OS_IsRunOnTwl() )
    {
#ifdef SDK_TWL
        result = OS_IsAvailableWireless_ltdmain();
#endif
    }
    else
    {
        result = TRUE;
    }
    return (result);
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsAgreeEULA

  Description:  EULA���Ӎς݂��ǂ������擾����B

  Arguments:    None.

  Returns:      BOOL - EULA���Ӎς݂̎�TRUE�A�񓯈ӂ̎�FALSE�B
                TWL��̓���łȂ��Ƃ����FALSE�B
 *---------------------------------------------------------------------------*/
BOOL OS_IsAgreeEULA(void)
{
    OSTWLSettingsData *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA;
        return (p->flags.isAgreeEULAFlagList & 0x01) ? TRUE : FALSE;
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetAgreeEULAVersion

  Description:  ���ӂ���EULA�̃o�[�W�������擾����B

  Arguments:    None

  Returns:      agreedEulaVersion - ���ӂ���EULA�o�[�W�����i0-255�̐����j
 *---------------------------------------------------------------------------*/
u8 OS_GetAgreedEULAVersion( void )
{
    OSTWLSettingsData *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA;
        return p->agreeEulaVersion[ 0 ];
    }
    else
    {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetROMHeaderEULAVersion

  Description:  �A�v���P�[�V������ROM�w�b�_�ɓo�^����Ă���EULA�̃o�[�W�������擾����B

  Arguments:    None

  Returns:      cardEulaVersion - �A�v���P�[�V������ROM�w�b�_�ɓo�^����Ă���EULA�o�[�W�����i0-255�̐����j
 *---------------------------------------------------------------------------*/
u8 OS_GetROMHeaderEULAVersion( void )
{
    if( OS_IsRunOnTwl() )
    {
        // ROM�w�b�_�� agree_EULA_version ���n�[�h�R�[�f�B���O
        return *(u8 *)( HW_TWL_ROM_HEADER_BUF + 0x020e );
    }
    else
    {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetParentalControlInfoPtr

  Description:  �y�A�����^���R���g���[�����ւ̃|�C���^���擾����B

  Arguments:    None.

  Returns:      �|�C���^��Ԃ��B
                TWL��̓���łȂ��Ƃ���NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
const OSTWLParentalControl* OS_GetParentalControlInfoPtr(void)
{
    OSTWLSettingsData *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA;
        return &(p->parentalControl);
    }
    else
    {
        return NULL;
    }
}

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
/* �ȉ��̃R�[�h�� TWL �g���������̈�ɔz�u */
#ifdef    SDK_TWL
#include  <twl/ltdmain_begin.h>
#endif
BOOL OSi_IsParentalControledAppCore(u8* appRatingInfo);

BOOL
OSi_IsParentalControledAppCore(u8* appRatingInfo)
{
    OSTWLParentalControl*   p   =   &(((OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA)->parentalControl);
    
    if (p->flags.isSetParentalControl)
    {
        if (p->ogn < OS_TWL_PCTL_OGN_MAX)
        {
            if (appRatingInfo == NULL)
            {
                /* ��������A�v���P�[�V�����̃��[�e�B���O��񂪕s�� */
                OS_TWarning("Invalid pointer to Application rating information.\n");
                return TRUE;
            }
            else
            {
                if ((appRatingInfo[p->ogn] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) == 0)
                {
                    /* �A�v���P�[�V�������ɊY������R���c�̂ł̃��[�e�B���O��񂪒�`����Ă��Ȃ� */
                    OS_TWarning("Application doesn't have rating information for the organization.\n");
                    return TRUE;
                }
                else
                {
                    if ((appRatingInfo[p->ogn] & OS_TWL_PCTL_OGNINFO_ALWAYS_MASK) != 0)
                    {
                        return TRUE;
                    }
                    else
                    {
                        if ((appRatingInfo[p->ogn] & OS_TWL_PCTL_OGNINFO_AGE_MASK) > p->ratingAge)
                        {
                            return TRUE;
                        }
                    }
                }
            }
        }
        else
        {
            /* �{�̐ݒ���̐R���c�̎w�肪�z��O */
            OS_TWarning("Invalid rating organization index (%d) in LCFG.\n", p->ogn);
        }
    }
    return FALSE;
}
/* �ȏ�̃R�[�h�� TWL �g���������̈�ɔz�u */
#ifdef    SDK_TWL
#include  <twl/ltdmain_end.h>
#endif

BOOL
OS_IsParentalControledApp(u8* appRatingInfo)
{
    if (OS_IsRunOnTwl() == TRUE)
    {
        return OSi_IsParentalControledAppCore(appRatingInfo);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetMovableUniqueID

  Description:  �{�̊Ԃňڍs�\�ȃ��j�[�NID(HW���Ɋ܂܂��)���擾����B

  Arguments:    pUniqueID - �i�[��(OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN�o�C�g�i�[�����)�B
                         TWL��̓���łȂ��Ƃ����0�Ŗ��߂���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OS_GetMovableUniqueID(u8 *pUniqueID)
{
    OSTWLHWNormalInfo *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLHWNormalInfo*)OS_ADDR_TWL_HWNORMALINFO;
        MI_CpuCopy8( p->movableUniqueID, pUniqueID, OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN*sizeof(u8) );
    }
    else
    {
        MI_CpuFill8( pUniqueID, 0, OS_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN*sizeof(u8) );
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetValidLanguageBitmap

  Description:  �Ή�����r�b�g�}�b�v���擾����B

  Arguments:    None.
  Returns:      �Ή�����r�b�g�}�b�v��Ԃ��B
                TWL��̓���łȂ��Ƃ���"0"��Ԃ��B
 *---------------------------------------------------------------------------*/
u32 OS_GetValidLanguageBitmap( void )
{
    OSTWLHWSecureInfo *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLHWSecureInfo*)HW_HW_SECURE_INFO;
        return p->validLanguageBitmap;
    }
    else
    {
        return 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetUniqueIDPtr

  Description:  �{�̊Ԃňڍs�\�ȃ��j�[�NID(HW���Ɋ܂܂��)�ւ̃|�C���^���擾����B

  Arguments:    None.

  Returns:      �|�C���^��Ԃ��B
                TWL��̓���łȂ��Ƃ���NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
const u8* OS_GetMovableUniqueIDPtr(void)
{
    OSTWLHWNormalInfo *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLHWNormalInfo*)OS_ADDR_TWL_HWNORMALINFO;
        return (p->movableUniqueID);
    }
    else
    {
        return NULL;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsForceDisableWireless

  Description:  �����������I�ɖ��������邩�ǂ����̏��B
                (HW�Z�L���A���Ɋ܂܂��)���擾����B

  Arguments:    None.

  Returns:      �����I�ɖ���������Ƃ�TRUE/����������Ȃ��Ƃ�FALSE�B
                TWL��̓���łȂ��Ƃ����FALSE�B
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static BOOL OS_IsForceDisableWireless_ltdmain(void);
static BOOL OS_IsForceDisableWireless_ltdmain(void)
{
    OSTWLHWSecureInfo *p;
    p = (OSTWLHWSecureInfo*)HW_HW_SECURE_INFO;
    return (p->flags.forceDisableWireless == 0x1);
}
#include <twl/ltdmain_end.h>
#endif

BOOL OS_IsForceDisableWireless(void)
{
    BOOL  result;
    if( OS_IsRunOnTwl() )
    {
#ifdef SDK_TWL
        result = OS_IsForceDisableWireless_ltdmain();
#endif
    }
    else
    {
        result = FALSE;
    }
    return (result);
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetRegion

  Description:  ���[�W���������擾����B

  Arguments:    None.

  Returns:      ���[�W�����ԍ���Ԃ��B
                TWL��̓���łȂ��Ƃ����0��Ԃ��B
 *---------------------------------------------------------------------------*/
OSTWLRegion OS_GetRegion(void)
{
    OSTWLRegion result;
    OSTWLHWSecureInfo *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLHWSecureInfo*)HW_HW_SECURE_INFO;
        result = (OSTWLRegion)(p->region);
    }
    else
    {
        result = (OSTWLRegion)0;
    }
    return (result);
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetSerialNo

  Description:  �V���A���i���o���擾����B

  Arguments:    serialNo - �i�[��B
                           �I�[�������܂߂�OS_TWL_HWINFO_SERIALNO_LEN_MAX�o�C�g�i�[�����B
                           TWL��̓���łȂ��Ƃ����0�Ŗ��߂���B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OS_GetSerialNo(u8 *serialNo)
{
    OSTWLHWSecureInfo *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLHWSecureInfo*)HW_HW_SECURE_INFO;
        MI_CpuCopy8( p->serialNo, serialNo, OS_TWL_HWINFO_SERIALNO_LEN_MAX*sizeof(u8) );
    }
    else
    {
        MI_CpuFill8( serialNo, 0, OS_TWL_HWINFO_SERIALNO_LEN_MAX*sizeof(u8) );
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetWirelessFirmwareData

  Description:  �����t�@�[���E�F�A�p�f�[�^�ւ̃|�C���^���擾����B

  Arguments:    None.
  Returns:      �����t�@�[���E�F�A�p�f�[�^�ւ̃|�C���^
 *---------------------------------------------------------------------------*/
OSTWLWirelessFirmwareData *OS_GetWirelessFirmwareData(void)
{
    if( OS_IsRunOnTwl() )
    {
        return (OSTWLWirelessFirmwareData*)HW_PARAM_WIRELESS_FIRMWARE_DATA;
    }
    else
    {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  Name:         OS_GetRegionCodeA3

  Description:  EC / NUP �Ŏg�����[�W�����R�[�h�ɑΉ����镶������擾����B

  Arguments:    region -   ���[�W�����R�[�h

  Returns:      ���[�W�����R�[�h�ɑΉ����镶����ւ̃|�C���^��Ԃ��B
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL
#include <twl/ltdmain_begin.h>
static const char* OSi_GetRegionCodeA3_ltdmain(OSTWLRegion region);
static const char* OSi_GetRegionCodeA3_ltdmain(OSTWLRegion region)
{
    const char* regionList[] =
    {
        "JPN",  // OS_TWL_REGION_JAPAN     = 0,  // NCL
        "USA",  // OS_TWL_REGION_AMERICA   = 1,  // NOA
        "EUR",  // OS_TWL_REGION_EUROPE    = 2,  // NOE
        "AUS",  // OS_TWL_REGION_AUSTRALIA = 3,  // NAL
        "CHN",  // OS_TWL_REGION_CHINA     = 4,  // IQue
        "KOR"   // OS_TWL_REGION_KOREA     = 5,  // NOK
    };
    
    if (region >= sizeof(regionList)/sizeof(regionList[0]))
    {
        OS_TWarning("Invalide region code.(%d)", region);
        return NULL;
    }
    return regionList[region];
}
#include <twl/ltdmain_end.h>
#endif /* SDK_TWL */

const char* OS_GetRegionCodeA3(OSTWLRegion region)
{
    if( OS_IsRunOnTwl() )
    {
#ifdef SDK_TWL
        return OSi_GetRegionCodeA3_ltdmain(region);
#else /* SDK_TWL */
        return NULL;
#endif /* SDK_TWL */
    }
    else
    {
        return NULL;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetISOCountryCodeA2

  Description:  TWL�Ǝ��̍�����ђn��R�[�h�ԍ��� ISO 3166-1 alpha-2 �ɕϊ�����B

  Arguments:    twlCountry -   ������ђn��R�[�h

  Returns:      ������ђn��R�[�h�ɑΉ����镶����ւ̃|�C���^��Ԃ��B
                TWL��̓���łȂ��Ƃ�NULL��Ԃ��B
 *---------------------------------------------------------------------------*/
extern const int    OSi_CountryCodeListNumEntries;
extern const char*  OSi_CountryCodeList[];

const char* OS_GetISOCountryCodeA2(u8 twlCountry)
{
    const char* cca2 = NULL;
    if( OS_IsRunOnTwl() )
    {
#ifdef SDK_TWL
        if (twlCountry < OSi_CountryCodeListNumEntries)
        {
            cca2 = OSi_CountryCodeList[twlCountry];
        }
#ifndef SDK_FINALROM
        if (!cca2)
        {
            OS_TWarning("Invalid country code(%d)\n", twlCountry);
        }
#endif /* SDK_FINALROM */
#endif /* SDK_TWL */
        return cca2;
    }
    else  // TWL��̓���łȂ��Ƃ�
    {
        return NULL;
    }
}

#undef OS_TWL_COUNTRY_NAME_MAX

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
