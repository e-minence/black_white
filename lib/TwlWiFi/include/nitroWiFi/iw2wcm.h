/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - WCM - include
  File:     iw2wcm.h

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
#ifndef NITROWIFI_IW2WCM_H_
#define NITROWIFI_IW2WCM_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    定数
 *---------------------------------------------------------------------------*/
#define IW_ESSID_SIZE           WCM_ESSID_SIZE
#define IW_BSSID_SIZE           WCM_BSSID_SIZE

#define IW_BSSBS_SIZE           WCM_APLIST_SIZE
#define IW_BSSEX_SIZE           WCM_APLIST_EX_SIZE
#define IW_BSS_SIZE             WCM_APLIST_BLOCK_SIZE
#define IW_WEPBS_SIZE           WCM_WEP_SIZE
#define IW_WEPEX_SIZE           WCM_WEP_EX_SIZE
#define IW_WEP_SIZE             WCM_WEP_STRUCT_SIZE
#define IW_WMDCFRECVWRK_SIZE    WCM_DCF_RECV_EXCESS_SIZE
#define IW_WMDCFSENDWRK_SIZE    WCM_DCF_SEND_EXCESS_SIZE
#define IW_WMDCFRECV_SIZE       WCM_DCF_RECV_BUF_SIZE
#define IW_WMDCFSEND_SIZE       WCM_DCF_SEND_BUF_SIZE

#define IW_BUFFER_SIZE          WCM_ETC_BUF_SIZE
#define IW_WORK_SIZE            WCM_WORK_SIZE

enum
{
    IW_PHASE_NULL       = WCM_PHASE_NULL,
    IW_PHASE_WAIT       = WCM_PHASE_WAIT,
    IW_PHASE_WAIT_IDLE  = WCM_PHASE_WAIT_TO_IDLE,
    IW_PHASE_IDLE       = WCM_PHASE_IDLE,
    IW_PHASE_IDLE_WAIT  = WCM_PHASE_IDLE_TO_WAIT,
    IW_PHASE_IDLE_SCAN  = WCM_PHASE_IDLE_TO_SEARCH,
    IW_PHASE_SCAN       = WCM_PHASE_SEARCH,
    IW_PHASE_SCAN_IDLE  = WCM_PHASE_SEARCH_TO_IDLE,
    IW_PHASE_IDLE_LINK  = WCM_PHASE_IDLE_TO_DCF,
    IW_PHASE_LINK       = WCM_PHASE_DCF,
    IW_PHASE_LINK_IDLE  = WCM_PHASE_DCF_TO_IDLE,
    IW_PHASE_FATALERROR = WCM_PHASE_FATAL_ERROR,
    IW_PHASE__
};

enum
{
    IW_RESULT_SUCCESS       = WCM_RESULT_SUCCESS,
    IW_RESULT_FAILURE       = WCM_RESULT_FAILURE,
    IW_RESULT_PROGRESS      = WCM_RESULT_PROGRESS,
    IW_RESULT_ACCEPT        = WCM_RESULT_ACCEPT,
    IW_RESULT_REJECT        = WCM_RESULT_REJECT,
    IW_RESULT_WMDISABLE     = WCM_RESULT_WMDISABLE,
    IW_RESULT_MEMORYERROR   = WCM_RESULT_NOT_ENOUGH_MEM,
    IW_RESULT_FATALERROR    = WCM_RESULT_FATAL_ERROR,
    IW_RESULT__
};

enum
{
    IW_NOTIFY_COMMON        = WCM_NOTIFY_COMMON,
    IW_NOTIFY_STARTUP       = WCM_NOTIFY_STARTUP,
    IW_NOTIFY_CLEANUP       = WCM_NOTIFY_CLEANUP,
    IW_NOTIFY_SEARCH_START  = WCM_NOTIFY_BEGIN_SEARCH,
    IW_NOTIFY_SEARCH_END    = WCM_NOTIFY_END_SEARCH,
    IW_NOTIFY_CONNECT       = WCM_NOTIFY_CONNECT,
    IW_NOTIFY_DISCONNECT    = WCM_NOTIFY_DISCONNECT,
    IW_NOTIFY_FOUND         = WCM_NOTIFY_FOUND_AP,
    IW_NOTIFY_BEACON_LOST,
    IW_NOTIFY_FATALERROR,
    IW_NOTIFY__
};

#define IW_OPTTST_CHANNEL           WCM_OPTION_TEST_CHANNEL
#define IW_OPTFLT_CHANNEL           WCM_OPTION_FILTER_CHANNEL
#define IW_OPTMSK_CHANNEL           WCM_OPTION_MASK_CHANNEL
#define IW_OPT_CHANNEL_1            WCM_OPTION_CHANNEL_1
#define IW_OPT_CHANNEL_2            WCM_OPTION_CHANNEL_2
#define IW_OPT_CHANNEL_3            WCM_OPTION_CHANNEL_3
#define IW_OPT_CHANNEL_4            WCM_OPTION_CHANNEL_4
#define IW_OPT_CHANNEL_5            WCM_OPTION_CHANNEL_5
#define IW_OPT_CHANNEL_6            WCM_OPTION_CHANNEL_6
#define IW_OPT_CHANNEL_7            WCM_OPTION_CHANNEL_7
#define IW_OPT_CHANNEL_8            WCM_OPTION_CHANNEL_8
#define IW_OPT_CHANNEL_9            WCM_OPTION_CHANNEL_9
#define IW_OPT_CHANNEL_10           WCM_OPTION_CHANNEL_10
#define IW_OPT_CHANNEL_11           WCM_OPTION_CHANNEL_11
#define IW_OPT_CHANNEL_12           WCM_OPTION_CHANNEL_12
#define IW_OPT_CHANNEL_13           WCM_OPTION_CHANNEL_13
#define IW_OPT_CHANNEL_ALL          WCM_OPTION_CHANNEL_ALL
#define IW_OPT_CHANNEL_RDC          WCM_OPTION_CHANNEL_RDC

#define IW_OPTTST_POWER             WCM_OPTION_TEST_POWER
#define IW_OPTFLT_POWER             WCM_OPTION_FILTER_POWER
#define IW_OPTMSK_POWER             WCM_OPTION_MASK_POWER
#define IW_OPT_POWER_SAVE           WCM_OPTION_POWER_SAVE
#define IW_OPT_POWER_FULL           WCM_OPTION_POWER_ACTIVE

#define IW_OPTTST_AUTHMODE          WCM_OPTION_TEST_AUTH
#define IW_OPTFLT_AUTHMODE          WCM_OPTION_FILTER_AUTH
#define IW_OPTMSK_AUTHMODE          WCM_OPTION_MASK_AUTH
#define IW_OPT_AUTHMODE_OPENSYSTEM  WCM_OPTION_AUTH_OPENSYSTEM
#define IW_OPT_AUTHMODE_SHAREDKEY   WCM_OPTION_AUTH_SHAREDKEY
#define IW_OPT_AUTHMODE_OPEN_SYSTEM WCM_OPTION_AUTH_OPENSYSTEM

#define IW_OPTTST_SCANTYPE          WCM_OPTION_TEST_SCANTYPE
#define IW_OPTFLT_SCANTYPE          WCM_OPTION_FILTER_SCANTYPE
#define IW_OPTMSK_SCANTYPE          WCM_OPTION_MASK_SCANTYPE
#define IW_OPT_SCANTYPE_PASSIVE     WCM_OPTION_SCANTYPE_PASSIVE
#define IW_OPT_SCANTYPE_ACTIVE      WCM_OPTION_SCANTYPE_ACTIVE

#define IW_Bssid_Any                WCM_Bssid_Any
#define IW_Essid_Any                WCM_Essid_Any
#define IW_BSSID_ANY                WCM_BSSID_ANY
#define IW_ESSID_ANY                WCM_ESSID_ANY

/*---------------------------------------------------------------------------*
    マクロ
 *---------------------------------------------------------------------------*/
#define IW_BOUNDUPPER4  WCM_ROUNDUP4
#define IW_BOUNDLOWER4  WCM_ROUNDDOWN4
#define IW_BOUND4       WCM_ROUNDUP4
#define IW_BOUNDUPPER32 WCM_ROUNDUP32
#define IW_BOUNDLOWER32 WCM_ROUNDDOWN32
#define IW_BOUND32      WCM_ROUNDUP32

/*---------------------------------------------------------------------------*
    構造体
 *---------------------------------------------------------------------------*/
#define IWNoticeParameter   WCMNoticeParameter
#define IWNotice            WCMNotice
#define IWNotify            WCMNotify
#define IWConfig            WCMConfig

/*---------------------------------------------------------------------------*
    関数
 *---------------------------------------------------------------------------*/
#define IW_Init         WCM_Init
#define IW_Exit         WCM_Finish
#define IW_Startup      WCM_StartupAsync
#define IW_Cleanup      WCM_CleanupAsync
#define IW_Search       WCM_SearchAsync
#define IW_Connect      WCM_ConnectAsync
#define IW_Disconnect   WCM_DisconnectAsync
#define IW_GetPhase     WCM_GetPhase
#define IW_ChangeOption WCM_UpdateOption
#define IW_LockBssDesc  WCM_LockApList
#define IW_ClearBssDesc WCM_ClearApList
#define IW_CountBssDesc WCM_CountApList
#define IW_PointBssDesc WCM_PointApList

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NITROWIFI_IW2WCM_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
