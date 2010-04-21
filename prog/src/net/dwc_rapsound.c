//=============================================================================
/**
 * @file	  dwc_rapsound.c
 * @bfief	  VCHAT�ׂ̈̉��ʊǗ�
 * @author	k.ohno
 * @date	  10/04/21
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"

#include "net/dwc_rap.h"
#include "dwc_rapfriend.h"
#include "savedata/wifilist.h"
#include "sound/pm_sndsys.h"



//------------------------------------------------------------------
/**
 * @brief   WIFIVCT�p���b�v�a�f�l�{�����[���ύX  VCT���͕ύX���󂯕t���Ȃ�
 */ 
//------------------------------------------------------------------
void GFL_NET_DWC_PMSND_ChangeBGMVolume( u16 trackBit, int volume )
{
  if(GFL_NET_IsWifiConnect()){  //WIFI�ڑ���
    if(GFL_NET_DWC_IsVChat()){
      return;
    }
  }
  PMSND_ChangeBGMVolume( trackBit, volume );
}

