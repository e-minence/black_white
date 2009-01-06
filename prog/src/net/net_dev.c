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
#define SUPPORT_WIFI_  (1)   // WIFI�ʐM
#define SUPPORT_WIRELESS_  (1)   // ���C�����X�ʐM
#define SUPPORT_IRC_  (1)   // IRC�ʐM
#define SUPPORT_OVERLAY_ (1)  //�I�[�o�[���C���s���ꍇ


extern GFLNetDevTable *NET_GetIrcDeviceTable(void);
extern GFLNetDevTable *NET_GetWirelessDeviceTable(void);
extern GFLNetDevTable *NET_GetWifiDeviceTable(void);


FS_EXTERN_OVERLAY(dev_wifi);
FS_EXTERN_OVERLAY(dev_wireless);
FS_EXTERN_OVERLAY(dev_irc);


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
      case GFL_NET_TYPE_WIFI:
#if SUPPORT_OVERLAY_
        GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifi ) );
#endif
        return NET_GetWifiDeviceTable();
#endif
#if SUPPORT_WIRELESS_
      case GFL_NET_TYPE_WIRELESS:
#if SUPPORT_OVERLAY_
        GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wireless ) );
#endif
        return NET_GetWirelessDeviceTable();
#endif
#if SUPPORT_IRC_
      case GFL_NET_TYPE_IRC:
#if SUPPORT_OVERLAY_
        GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_irc ) );
#endif
        return NET_GetIrcDeviceTable();
#endif
      default:
        GF_ASSERT(0);
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
      case GFL_NET_TYPE_WIFI:
#if SUPPORT_OVERLAY_
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wifi ) );
#endif
        break;
#endif
#if SUPPORT_WIRELESS_
      case GFL_NET_TYPE_WIRELESS:
#if SUPPORT_OVERLAY_
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_wireless ) );
#endif
        break;
#endif
#if SUPPORT_IRC_
      case GFL_NET_TYPE_IRC:
#if SUPPORT_OVERLAY_
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_irc ) );
#endif
        break;
#endif
      default:
        GF_ASSERT(0);
    }
}

