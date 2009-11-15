#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	dwc_raputil.c
 * @bfief	WIFI�ݒ��ʌĂяo���p
 * @author	k.ohnop
 * @date	09/01/15
 */
//=============================================================================

#pragma once

#include <gflib.h>

//==============================================================================
/**
 * WiFi�ڑ����[�e�B���e�B���Ăяo��
 * @param   none
 * @retval  none
 */
//==============================================================================


FS_EXTERN_OVERLAY(wifi_util);
extern const GFL_PROC_DATA WifiUtilProcData;
extern const GFL_PROC_DATA WifiUtilGSyncProcData;




#ifdef __cplusplus
} /* extern "C" */
#endif
