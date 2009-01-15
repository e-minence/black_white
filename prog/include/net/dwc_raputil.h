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

#ifndef DWC_RAPUTIL_H__
#define DWC_RAPUTIL_H__

#include <gflib.h>

//==============================================================================
/**
 * WiFi�ڑ����[�e�B���e�B���Ăяo��
 * @param   none
 * @retval  none
 */
//==============================================================================

extern void mydwc_callWifiUtil( HEAPID HeapID );


FS_EXTERN_OVERLAY(wifi_util);
extern const GFL_PROC_DATA WifiUtilProcData;


#endif //DWC_RAPUTIL_H__


#ifdef __cplusplus
} /* extern "C" */
#endif
