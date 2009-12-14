//=============================================================================
/**
 * @file	  wih_dwc.c
 * @brief	  �r�[�R����͂��s���AAP�̔��ʂ��s��
 * @author	k.ohno
 * @date    2009.11.15
 */
//=============================================================================

#include <gflib.h>
#include <nitro/irc.h>
#include "net/network_define.h"
#include "net/wih_dwc.h"
#include "nitroWiFi/ncfg.h"
#include "nitroWiFi/wcm.h"
#include "net/net_whpipe.h"

#define _ALLBEACON_MAX (4)

#define WCM_NETWORK_CAPABILITY_PRIVACY          0x0010

typedef struct{
	WMBssDesc sBssDesc;
  int timer;
} _BEACONCATCH_STR;

struct _WIH_DWC_WORK {
  _BEACONCATCH_STR aBeaconCatch[_ALLBEACON_MAX];
  void* ScanMemory;
  NCFGConfigEx* cfg;
  BOOL bIrc;
  int timer;
  BOOL bStop;
	u16 AllBeaconNum;
  u8 buff[200];  //�ԊO����M���s�����o�b�t�@
  
};


static void _SetScanBeaconData(WMBssDesc* pBss);
static WIH_DWC_WORK* _localWork=NULL;
FS_EXTERN_OVERLAY(dev_irc);


/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

WIH_DWC_WORK* WIH_DWC_AllBeaconStart(int num, HEAPID id)
{
  int i;
  WIH_DWC_WORK* pWork;
  
  GF_ASSERT(num <= _ALLBEACON_MAX);
  GF_ASSERT(_localWork==NULL);
  
  pWork = GFL_NET_Align32Alloc(id , sizeof(WIH_DWC_WORK));
  pWork->AllBeaconNum = num;

  pWork->ScanMemory =GFL_NET_Align32Alloc(id ,DWC_GetParseWMBssDescWorkSize());
  WHSetWIHDWCBeaconGetFunc(_SetScanBeaconData);
  _localWork=pWork;

  pWork->cfg =GFL_NET_Align32Alloc(id ,sizeof(NCFGConfigEx));
  NCFG_ReadConfig(&_localWork->cfg->compat, NULL);

// NCFG_ReadConfigEx(NCFGConfigEx* configEx, void* work); //@todo

  GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_irc ) );

  
  IRC_Init(12);  //@todo 12
  
  return(pWork);
}

/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

void WIH_DWC_AllBeaconEnd(WIH_DWC_WORK* pWork)
{
  int i;
  GF_ASSERT(_localWork);

  WHSetWIHDWCBeaconGetFunc(NULL);

  IRC_Shutdown();
  GFL_NET_Align32Free(pWork->cfg);
  GFL_NET_Align32Free(pWork->ScanMemory);
  GFL_NET_Align32Free(pWork);
  _localWork=NULL;
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( dev_irc ) );
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R�����W���~����
 */
//------------------------------------------------------------------------------

void WIH_DWC_Stop(void)
{
  if(_localWork){
    _localWork->bStop=TRUE;
    IRC_Init(12);  //@todo 12
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R�����W���ĊJ����
 */
//------------------------------------------------------------------------------

void WIH_DWC_Restart(void)
{
  if(_localWork){
    _localWork->bStop=FALSE;
    IRC_Shutdown();
  }
}


/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

static void _SetScanBeaconData(WMBssDesc* pBss)
{
  int i;

  if(_localWork->AllBeaconNum==0){
    return;
  }
  
  for(i=0;i<_localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
      if(GFL_STD_MemComp(&pS->sBssDesc, pBss, sizeof(WMBssDesc))==0){ //��v
        pS->timer = DEFAULT_TIMEOUT_FRAME;
        return;
      }
  }
  for(i=0;i<_localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    if(pS->timer==0){
      GFL_STD_MemCopy(pBss, &pS->sBssDesc, sizeof(WMBssDesc));
      pS->timer = DEFAULT_TIMEOUT_FRAME;
      return;
    }
  }
}

/*---------------------------------------------------------------------------*
  Name:         WH_MainLoopScanBeaconData
  Description:  CGEAR�p�Ȃ�ł��r�[�R�����WMAIN
 *---------------------------------------------------------------------------*/

void WIH_DWC_MainLoopScanBeaconData(void)
{
  int i;

  if(_localWork && _localWork->bStop){
    return;
  }
  
  if(_localWork){
    for(i=0;i<_localWork->AllBeaconNum;i++){
      _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
      if(pS->timer > 0){
        pS->timer--;
      }
    }
  }
  {
    int size = IRCi_Read(_localWork->buff);
    if(size>0){
      _localWork->bIrc = TRUE;
      _localWork->timer = 60;
    }
  }
  IRC_Move();
}

//------------------------------------------------------------------------------
/**
 * @brief   �{��AP���X�g�ɂ���r�[�R�����ǂ���
 * @param   ��������r�[�R��
 * @retval  ��������TRUE
 */
//------------------------------------------------------------------------------

static BOOL _scanAPReserveed( WMBssDesc* pChk )
{
  int i;



  for(i = 0;i < 3;i++){
    if(GFL_STD_MemComp(_localWork->cfg->slot[i].ap.ssid,pChk->ssid,WM_SIZE_SSID)){
      
 //     NET_PRINT("SSIDFIND %s\n",pChk->ssid);
      
      return TRUE;
    }
  }
  return FALSE;
}

/*
�@FREESPOT�ɂ��܂��ẮA������񋟂����֐��ł͌��m�ł��܂���̂ŁA�ȉ�
�̕��@�ł̌��o�����肢�v���܂��B

�EFREESPOT�ɂ��ẮASSID��"FREESPOT"��8�����ł��邱�Ƃ��`�F�b�N
�EWMBssDesc��capaInfo��bit4��0�ł��邱��(��)���`�F�b�N
*/
//------------------------------------------------------------------------------
/**
 * @brief   FREESPOT(TM)���ǂ���
 * @param   ��������r�[�R��
 * @retval  ��������TRUE
 */
//------------------------------------------------------------------------------

static BOOL _scanFreeSpot( WMBssDesc* pChk )
{

  if(pChk->ssidLength==8){
    if(GFL_STD_MemComp("FREESPOT",pChk->ssid,8)){
      if(!(pChk->capaInfo & WCM_NETWORK_CAPABILITY_PRIVACY)){
        return TRUE;
      }
    }
  }
  return FALSE;

}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�L�����e�B�[�r�b�g�����邩�ǂ���
 * @param   ��������r�[�R��
 * @retval  ��������TRUE
 */
//------------------------------------------------------------------------------

static BOOL _scanPrivacy( WMBssDesc* pChk )
{

  if(!(pChk->capaInfo & WCM_NETWORK_CAPABILITY_PRIVACY)){
    return TRUE;
  }
  return FALSE;

}


//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���̃^�C�v�𓾂�  ���ԂŃX�L�����D�揇�ʂ�ς���
 * @param   �r�[�R����
 * @param   �q�[�vID
 * @retval  GAME_COMM_STATUS
 */
//------------------------------------------------------------------------------

GAME_COMM_STATUS WIH_DWC_GetAllBeaconType(void)
{
  int i;
  int retcode = GAME_COMM_STATUS_NULL;
  const GFLNetInitializeStruct *aNetStruct = GFL_NET_GetNETInitStruct();

  if(_localWork==NULL){
    return retcode;
  }

  if( _localWork->bIrc == TRUE){
    if(_localWork->timer>0){
      _localWork->timer--;
    }
    else{
      _localWork->bIrc = FALSE;
    }
    return GAME_COMM_STATUS_IRC;
  }
  
  for( i=0;i < aNetStruct->maxBeaconNum;i++ ){
    if( GFL_NET_GetBeaconData( i ) != NULL ){
      GameServiceID id = GFL_NET_WLGetGameServiceID(i);
      switch(id){
      case WB_NET_UNION:
        return GAME_COMM_STATUS_WIRELESS_UN;
      case WB_NET_FIELDMOVE_SERVICEID:
      case WB_NET_PALACE_SERVICEID:
        return GAME_COMM_STATUS_WIRELESS;
      case WB_NET_NOP_SERVICEID:
        break;
      default:
        return GAME_COMM_STATUS_WIRELESS_FU;  //@todo ���̑��͕s�v�c�ɂ��Ă���
      }
    }
  }
  
  for(i=0;i < _localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    if(pS->timer == 0){
      continue;
    }
    {
      WMBssDesc* bss = &pS->sBssDesc;
    
      DWCApInfo ap;

      if(_scanAPReserveed(bss)){
        retcode = GAME_COMM_STATUS_WIFI;
      }
      else if(_scanFreeSpot(bss)){
        retcode = GAME_COMM_STATUS_WIFI_ZONE;
      }
      else if(DWC_ParseWMBssDesc(_localWork->ScanMemory,bss,&ap)){
        OS_TPrintf("DWCApInfo %d\n",ap.aptype);
        switch(ap.aptype){
        case DWC_APINFO_TYPE_USER0:
        case DWC_APINFO_TYPE_USER1:
        case DWC_APINFO_TYPE_USER2:
#ifdef SDK_TWL
        case DWC_APINFO_TYPE_USER3:
        case DWC_APINFO_TYPE_USER4:
        case DWC_APINFO_TYPE_USER5:
#endif //SDK_TWL
        case DWC_APINFO_TYPE_USB://          :  �j���e���h�[Wi-Fi USB�R�l�N�^
          retcode = GAME_COMM_STATUS_WIFI;
          break;
        case DWC_APINFO_TYPE_SHOP://         :  �j���e���h�[Wi-Fi�X�e�[�V����
        case DWC_APINFO_TYPE_FREESPOT://     :  FREESPOT�i�t���[�X�|�b�g�j
        case DWC_APINFO_TYPE_WAYPORT://      :  Wayport(�k�ăz�b�g�X�|�b�g)�����݂͎g�p�ł��܂���
        case DWC_APINFO_TYPE_NINTENDOZONE:// : Nintendo Zone
          retcode = GAME_COMM_STATUS_WIFI_ZONE;
          break;
        }
      }
    }
  }
  if(retcode!=GAME_COMM_STATUS_NULL){
    return retcode;
  }

  for(i=0;i < _localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    if(pS->timer == 0){
      continue;
    }
    if(!_scanPrivacy(&pS->sBssDesc)){
      return GAME_COMM_STATUS_WIFI_FREE;
    }
  }
  if(_localWork->AllBeaconNum>0){
    return GAME_COMM_STATUS_WIFI_LOCK;
  }
  ///@todo �Z�L�����e�B�[���������Ă��邩�ǂ���
  return retcode;
}

