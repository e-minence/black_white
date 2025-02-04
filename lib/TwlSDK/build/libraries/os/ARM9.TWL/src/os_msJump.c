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
 定数定義
 *---------------------------------------------------------------------------*/
#define TITLE_ID_MACHINE_SETTING 0x00030015484e4241 /* HNBA、仕向地は問わない */

//============================================================================

/* 以下のコードは TWL 拡張メモリ領域に配置 */
#ifdef    SDK_TWL
#include  <twl/ltdmain_begin.h>
#endif
/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToMachineSetting

  Description:  TWL の本体設定の指定した項目にアプリジャンプする
  
  Arguments:    dest : 設定項目
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToMachineSetting(u8 dest)
{
    switch( dest )
    {
        OSTWLRegion region = OS_GetRegion();

        case  OS_TWL_MACHINE_SETTING_COUNTRY:
            /* 本体設定に国設定が存在しないリージョンでは FALSE で終了 */
            switch ( OS_GetRegion() )
            {
                case OS_TWL_REGION_AMERICA:
                case OS_TWL_REGION_EUROPE:
                case OS_TWL_REGION_AUSTRALIA:

                    break;

                case OS_TWL_REGION_JAPAN:
//                case OS_TWL_REGION_CHINA:
//                case OS_TWL_REGION_KOREA:
                /* 上記以外のリージョンも FALSE で終了 */
                default:
                    OS_TWarning("Region Error.");
                    return FALSE;
            }
            break;

    case  OS_TWL_MACHINE_SETTING_LANGUAGE:
            /* 本体設定に言語設定が存在しないリージョンでは FALSE で終了 */
            switch ( OS_GetRegion() )
            {
                case OS_TWL_REGION_AMERICA:
                case OS_TWL_REGION_EUROPE:

                    break;

                case OS_TWL_REGION_JAPAN:
                case OS_TWL_REGION_AUSTRALIA:
//                case OS_TWL_REGION_CHINA:
//                case OS_TWL_REGION_KOREA:
                /* 上記以外のリージョンも FALSE で終了 */
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
        (void)OS_DecodeDeliverArg();   //事前に DeliverArg を設定していない場合もあるので、正否に関わらず処理を続行
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

  Description:  TWL の本体設定にアプリケーションジャンプして、
                "インターネット - 利用規約" を立ち上げます。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToEulaDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_NETWORK_EULA );
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToApplicationManagerDirect

  Description:  TWL の本体設定にアプリケーションジャンプして、
                "ソフト管理" を立ち上げます。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToApplicationManagerDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_APP_MANAGER );
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToNetworkSettngDirect

  Description:  TWL の本体設定にアプリケーションジャンプして、
                "インターネット - 接続設定" を立ち上げます。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToNetworkSettngDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_NETWORK_SETTING );
}

/*---------------------------------------------------------------------------*
  Name:         OSi_JumpToCountrySettingDirect

  Description:  TWL の本体設定にアプリケーションジャンプして、
                "Country" を立ち上げます。
                ※ 日本向けの TWL では同設定が存在しないため、本体設定の先頭にジャンプします。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToCountrySettingDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_COUNTRY );
}

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToSystemUpdateDirect

  Description:  TWL の本体設定にアプリケーションジャンプして、
                "本体の更新" を立ち上げます。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
 *---------------------------------------------------------------------------*/
BOOL OSi_JumpToSystemUpdateDirect( void )
{
    return OSi_JumpToMachineSetting( OS_TWL_MACHINE_SETTING_SYSTEM_UPDATE );
}

/* 以上のコードは TWL 拡張メモリ領域に配置 */
#ifdef    SDK_TWL
#include  <twl/ltdmain_end.h>
#endif

/*---------------------------------------------------------------------------*
  Name:         OS_JumpToInternetSetting

  Description:  ハードウェアリセットを行い、TWL 本体設定のインターネット-接続設定
                にジャンプします。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
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

  Description:  ハードウェアリセットを行い、TWL 本体設定のインターネット-利用規約
                にジャンプします。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
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

  Description:  ハードウェアリセットを行い、TWL 本体設定の 無線通信 にジャンプします。
  
  Arguments:    なし
 
  Returns:      FALSE … 何らかの理由でアプリケーションジャンプに失敗
                ※ 処理が成功した場合、この関数中でリセット処理が発生するため
                   TRUE を返すことはありません。
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
