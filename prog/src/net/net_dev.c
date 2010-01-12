//=============================================================================
/**
 * @file	net_dev.c
 * @bfief	�ʐM�f�o�C�X�I��
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	08/12/19
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"

// �ʐMGFLIB�͒ʐM�f�o�C�X�������Ă��܂���
// �����GFLIB�Ɏ����Ă��܂��ƁA�]�v�Ƀ��C�u�����T�C�Y�𑝂₵�Ă��܂��ׂł�
// ���̃t�@�C���̓I�[�o�[���C���Ȃ��ł��������B
// �܂��A���̊֐���GFL_NET_Init�ŏ���ɌĂ΂�܂��̂ŁA�K���錾���Ă��������B


//�Ή�����ʐM��1 ���Ȃ��ꍇ0
#ifndef MULTI_BOOT_MAKE

#define SUPPORT_WIFI_  (1)   // WIFI�ʐM
#define SUPPORT_WIRELESS_  (1)   // ���C�����X�ʐM
#define SUPPORT_IRC_  (1)   // IRC�ʐM
#define SUPPORT_OVERLAY_ (1)  //�I�[�o�[���C���s���ꍇ

#else

#define SUPPORT_WIFI_  (0)   // WIFI�ʐM
#define SUPPORT_WIRELESS_  (1)   // ���C�����X�ʐM
#define SUPPORT_IRC_  (0)   // IRC�ʐM
#define SUPPORT_OVERLAY_ (0)  //�I�[�o�[���C���s���ꍇ

#endif //MULTI_BOOT_MAKE


extern GFLNetDevTable *NET_GetIrcDeviceTable(void);
extern GFLNetDevTable *NET_GetWirelessDeviceTable(void);
extern GFLNetDevTable *NET_GetWifiDeviceTable(void);

//SRCS_OVERLAY_NOTWIFI

FS_EXTERN_OVERLAY(notwifi);
FS_EXTERN_OVERLAY(dev_wifilib);
FS_EXTERN_OVERLAY(dev_wifi);
FS_EXTERN_OVERLAY(dev_wireless);
FS_EXTERN_OVERLAY(dev_irc);
FS_EXTERN_OVERLAY(wifilobby_common);
FS_EXTERN_OVERLAY(dpw_common);


//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X�����[�h����
 * @param   deviceNo  �f�o�C�X�ԍ�
 * @retval  GFLNetDevTable*
 */
//------------------------------------------------------------------------------
GFLNetDevTable* NET_DeviceLoad(int deviceNo)
{
  switch(deviceNo){
#if SUPPORT_WIFI_	
	case GFL_NET_TYPE_WIFI_GTS:
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( notwifi ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifi ) );
		GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common) );
    return NET_GetWifiDeviceTable();
  case GFL_NET_TYPE_WIFI:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( notwifi ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifi ) );
#endif
    return NET_GetWifiDeviceTable();
#endif
#if SUPPORT_WIFI_
  case GFL_NET_TYPE_WIFI_LOBBY:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( notwifi ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifi ) );
    //        GFL_OVERLAY_Load( FS_OVERLAY_ID( wifilobby_common ) );
#endif
    return NET_GetWifiDeviceTable();
#endif
#if SUPPORT_WIRELESS_
  case GFL_NET_TYPE_WIRELESS:
  case GFL_NET_TYPE_WIRELESS_SCANONLY:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wireless ) );
#endif
    return NET_GetWirelessDeviceTable();
#endif
#if SUPPORT_IRC_
  case GFL_NET_TYPE_IRC:
  case GFL_NET_TYPE_IRC_WIRELESS:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_irc ) );
#endif
    return NET_GetIrcDeviceTable();
#endif
  default:
    //        GF_ASSERT(0);
    break;
  }
  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X���A�����[�h����
 * @param   deviceNo  �f�o�C�X�ԍ�
 * @retval  none
 */
//------------------------------------------------------------------------------
void NET_DeviceUnload(int deviceNo)
{
  switch(deviceNo){
#if SUPPORT_WIFI_
	case GFL_NET_TYPE_WIFI_GTS:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dpw_common ) );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifi ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( notwifi ) );
#endif
		break;
  case GFL_NET_TYPE_WIFI:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifi ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( notwifi ) );
#endif
    break;
#endif	//SUPPORT_WIFI_
#if SUPPORT_WIFI_
  case GFL_NET_TYPE_WIFI_LOBBY:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifi ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( notwifi ) );
    //        GFL_OVERLAY_Unload( FS_OVERLAY_ID( wifilobby_common ) );
#endif
    break;
#endif
#if SUPPORT_WIRELESS_
  case GFL_NET_TYPE_WIRELESS:
  case GFL_NET_TYPE_WIRELESS_SCANONLY:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wireless ) );
#endif
    break;
#endif
#if SUPPORT_IRC_
  case GFL_NET_TYPE_IRC:
  case GFL_NET_TYPE_IRC_WIRELESS:
#if SUPPORT_OVERLAY_
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_irc ) );
#endif
    break;
#endif
  default:
    //        GF_ASSERT(0);
    break;
  }
}

