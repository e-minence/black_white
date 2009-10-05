/*---------------------------------------------------------------------------*
  Project:  TwlSDK - OS
  File:     os_msJump.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/
 
#include <nitro.h>
#include <twl/os/common/msJump.h>

/*---------------------------------------------------------------------------*
 �萔��`
 *---------------------------------------------------------------------------*/
#define TITLE_ID_MACHINE_SETTING 0x00030015484e4241 /* HNBA�A�d���n�͖��Ȃ� */

//============================================================================

/* �ȉ��̃R�[�h�� TWL �g���������̈�ɔz�u */
#ifdef    SDK_TWL
#include  <twl/ltdmain_begin.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToMachineSetting

  Description:  TWL �̖{�̐ݒ�̎w�肵�����ڂɃA�v���W�����v����
  
  Arguments:    dest : �ݒ荀��
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToMachineSetting(u8 dest)
{
    switch( dest )
    {
        OSTWLRegion region = OS_GetRegion();

        case  OS_TWL_MACHINE_SETTING_COUNTRY:
            /* �{�̐ݒ�ɍ��ݒ肪���݂��Ȃ����[�W�����ł� FALSE �ŏI�� */
            switch ( OS_GetRegion() )
            {
                case OS_TWL_REGION_AMERICA:
                case OS_TWL_REGION_EUROPE:
                case OS_TWL_REGION_AUSTRALIA:

                    break;

                case OS_TWL_REGION_JAPAN:
//                case OS_TWL_REGION_CHINA:
//                case OS_TWL_REGION_KOREA:
                /* ��L�ȊO�̃��[�W������ FALSE �ŏI�� */
                default:
                    OS_TWarning("Region Error.");
                    return FALSE;
            }
            break;

    case  OS_TWL_MACHINE_SETTING_LANGUAGE:
            /* �{�̐ݒ�Ɍ���ݒ肪���݂��Ȃ����[�W�����ł� FALSE �ŏI�� */
            switch ( OS_GetRegion() )
            {
                case OS_TWL_REGION_AMERICA:
                case OS_TWL_REGION_EUROPE:

                    break;

                case OS_TWL_REGION_JAPAN:
                case OS_TWL_REGION_AUSTRALIA:
//                case OS_TWL_REGION_CHINA:
//                case OS_TWL_REGION_KOREA:
                /* ��L�ȊO�̃��[�W������ FALSE �ŏI�� */
                default:
                    OS_TWarning("Region Error.");
                    return FALSE;
            }
            break;
        
        case  OS_TWL_MACHINE_SETTING_PAGE_1:
        case  OS_TWL_MACHINE_SETTING_PAGE_2:
        case  OS_TWL_MACHINE_SETTING_PAGE_3:
        case  OS_TWL_MACHINE_SETTING_PAGE_4:
        case  OS_TWL_MACHINE_SETTING_APP_MANAGER:
        case  OS_TWL_MACHINE_SETTING_WIRELESS_SW:
        case  OS_TWL_MACHINE_SETTING_BRIGHTNESS:
        case  OS_TWL_MACHINE_SETTING_USER_INFO:
        case  OS_TWL_MACHINE_SETTING_DATE:
        case  OS_TWL_MACHINE_SETTING_TIME:
        case  OS_TWL_MACHINE_SETTING_ALARM:
        case  OS_TWL_MACHINE_SETTING_TP_CALIBRATION:
        case  OS_TWL_MACHINE_SETTING_PARENTAL_CONTROL:
        case  OS_TWL_MACHINE_SETTING_NETWORK_SETTING:
        case  OS_TWL_MACHINE_SETTING_NETWORK_EULA:
        case  OS_TWL_MACHINE_SETTING_NETWORK_OPTION:
        case  OS_TWL_MACHINE_SETTING_SYSTEM_UPDATE:
        case  OS_TWL_MACHINE_SETTING_SYSTEM_INITIALIZE:
            break;
        default:
            OS_TWarning("Unknown Destination");
            return FALSE;
    }

    /* Set Deliver Argument */
    {
        OSDeliverArgInfo argInfo;
        int result;

        OS_InitDeliverArgInfo(&argInfo, 0);
        (void)OS_DecodeDeliverArg();   //���O�� DeliverArg ��ݒ肵�Ă��Ȃ��ꍇ������̂ŁA���ۂɊւ�炸�����𑱍s
        OSi_SetDeliverArgState( OS_DELIVER_ARG_BUF_ACCESSIBLE | OS_DELIVER_ARG_BUF_WRITABLE );
        result = OS_SetSysParamToDeliverArg( (u16)dest );
        
        if(result != OS_DELIVER_ARG_SUCCESS )
        {
            OS_TWarning("Failed to Set DeliverArgument.");
            return FALSE;
        }
        result = OS_EncodeDeliverArg();
        if(result != OS_DELIVER_ARG_SUCCESS )
        {
            OS_TWarning("Failed to Encode DeliverArgument.");
            return FALSE;
        }
    }
    /* Do Application Jump */
    return OS_DoApplicationJump( TITLE_ID_MACHINE_SETTING, OS_APP_JUMP_NORMAL );


    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToEulaDirect

  Description:  TWL �̖{�̐ݒ�ɃA�v���P�[�V�����W�����v���āA
                "�C���^�[�l�b�g - ���p�K��" �𗧂��グ�܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToEulaDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_NETWORK_EULA );
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToApplicationManagerDirect

  Description:  TWL �̖{�̐ݒ�ɃA�v���P�[�V�����W�����v���āA
                "�\�t�g�Ǘ�" �𗧂��グ�܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToApplicationManagerDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_APP_MANAGER );
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToNetworkSettngDirect

  Description:  TWL �̖{�̐ݒ�ɃA�v���P�[�V�����W�����v���āA
                "�C���^�[�l�b�g - �ڑ��ݒ�" �𗧂��グ�܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToNetworkSettngDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_NETWORK_SETTING );
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToCountrySettingDirect

  Description:  TWL �̖{�̐ݒ�ɃA�v���P�[�V�����W�����v���āA
                "Country" �𗧂��グ�܂��B
                �� ���{������ TWL �ł͓��ݒ肪���݂��Ȃ����߁A�{�̐ݒ�̐擪�ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToCountrySettingDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_COUNTRY );
}

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToSystemUpdateDirect

  Description:  TWL �̖{�̐ݒ�ɃA�v���P�[�V�����W�����v���āA
                "�{�̂̍X�V" �𗧂��グ�܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToSystemUpdateDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_SYSTEM_UPDATE );
}

/* �ȏ�̃R�[�h�� TWL �g���������̈�ɔz�u */
#ifdef    SDK_TWL
#include  <twl/ltdmain_end.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToInternetSetting

  Description:  �n�[�h�E�F�A���Z�b�g���s���ATWL �{�̐ݒ�̃C���^�[�l�b�g-�ڑ��ݒ�
                �ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToInternetSetting(void)
{
    BOOL result = FALSE;
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        result = OSi_JumpToNetworkSettngDirect();
    }
    else
#endif
    {
        OS_TWarning("This Hardware don't support this funciton");
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToEULAViewer

  Description:  �n�[�h�E�F�A���Z�b�g���s���ATWL �{�̐ݒ�̃C���^�[�l�b�g-���p�K��
                �ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToEULAViewer(void)
{
    BOOL result = FALSE;
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        result = OSi_JumpToEulaDirect();
    }
    else
#endif
    {
        OS_TWarning("This Hardware don't support this funciton");
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToWirelessSetting

  Description:  �n�[�h�E�F�A���Z�b�g���s���ATWL �{�̐ݒ�� �����ʐM �ɃW�����v���܂��B
  
  Arguments:    �Ȃ�
 
  Returns:      FALSE �c ���炩�̗��R�ŃA�v���P�[�V�����W�����v�Ɏ��s
                �� ���������������ꍇ�A���̊֐����Ń��Z�b�g�������������邽��
                   TRUE ��Ԃ����Ƃ͂���܂���B
 *---------------------------------------------------------------------------*/
BOOL OS_JumpToWirelessSetting(void)
{
    BOOL result = FALSE;
#ifdef SDK_TWL
    if( OS_IsRunOnTwl() )
    {
        result = OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_WIRELESS_SW );
    }
    else
#endif
    {
        OS_TWarning("This Hardware don't support this funciton");
    }
    return result;
}
/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
