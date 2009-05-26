//==============================================================================
/**
 * @file    net_exchange.c
 * @brief   Wi-Fi�L��A�}�b�`���O�ŋ��ʂ��Ďg�p��������f�[�^
 * @author  matsuda
 * @date    2009.05.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "net/net_exchange.h"
#include "savedata\wifilist.h"


//==================================================================
/**
 * Wi-Fi�̋��ʌ����f�[�^���Z�b�g����
 *
 * @param   sv		�Z�[�u�f�[�^�ւ̃|�C���^
 * @param   exc		�����
 */
//==================================================================
void NET_EXCHANGE_SetParam(SAVE_CONTROL_WORK *sv, WIFI_EXCHANGE_WORK *exc)
{
  WIFI_HISTORY* pHistry = SaveData_GetWifiHistory(sv);
  
  GFL_STD_MemClear(exc, sizeof(WIFI_EXCHANGE_WORK));
  
  GFL_NET_DWC_GetMySendedFriendCode( SaveData_GetWifiListData(sv), &exc->friendData );
  MyStatus_Copy(SaveData_GetMyStatus(sv), &exc->mystatus);
  OS_GetMacAddress(exc->mac);
  exc->my_nation = WIFIHISTORY_GetMyNation(pHistry);
  exc->my_area = WIFIHISTORY_GetMyArea(pHistry);
}

