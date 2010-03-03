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
#include "net/ctvt_beacon.h"

#define _ALLBEACON_MAX (4)

#define WCM_NETWORK_CAPABILITY_PRIVACY          0x0010
#define _IRC_CATCH_LOOP (3)  //�ԊO�����E���Ă���A�����ɂ���܂ł̉�

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
//�ʐM�̊��ɂ�����镔���Ȃ̂Ń��[�J���ɂ�����������ł��Q�Ƃł���`�ɂ��Ă���
static WIH_DWC_WORK* _localWork=NULL;  
static NCFGConfigEx* _localcfg=NULL;

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

  pWork->cfg = _localcfg;

  GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_irc ) );

  
#if 0 //�ԊO���͏�ɓ_�łɎd�l�ύX 2010.01.08
  IRC_Init(12);  //@todo 12
#endif
  
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

#if 0 //�ԊO���͏�ɓ_�łɎd�l�ύX 2010.01.08
  IRC_Shutdown();
#endif
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
#if 0 //�ԊO���͏�ɓ_�łɎd�l�ύX 2010.01.08
    IRC_Init(12);  //@todo 12
#endif
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
#if 0 //�ԊO���͏�ɓ_�łɎd�l�ύX 2010.01.08
    IRC_Shutdown();
#endif
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
#if 0 //�ԊO���͏�ɓ_�łɎd�l�ύX 2010.01.08

  {
    int size = IRCi_Read(_localWork->buff);
    if(size>0){
      _localWork->bIrc = TRUE;
      _localWork->timer = _IRC_CATCH_LOOP;
    }
  }
  IRC_Move();
#endif
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
  GAME_COMM_STATUS_BIT bit  = WIH_DWC_GetAllBeaconTypeBit();

  //�ŏ��Ɍ��������r�b�g��D��x�������Ɣ��f���A�Ԃ�
  for( i = 0; i < GAME_COMM_STATUS_MAX; i++ )
  { 
    if( (bit >> i) & 1 )
    { 
      return i;
    }
  }

  return GAME_COMM_STATUS_NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���̃^�C�v�𓾂�  �r�b�g�Ȃ̂ŁA�����̏�Ԃ��擾�\
 * @retval  GAME_COMM_STATUS_BIT
 */
//------------------------------------------------------------------------------
GAME_COMM_STATUS_BIT WIH_DWC_GetAllBeaconTypeBit(void)
{ 
  int i;
  GAME_COMM_STATUS_BIT retcode = 0;
  const GFLNetInitializeStruct *aNetStruct = GFL_NET_GetNETInitStruct();

  if(_localWork==NULL){
    return retcode;
  }

#if 0 //�ԊO���͏�ɓ_�łɎd�l�ύX 2010.01.08
  if( _localWork->bIrc == TRUE){
    if(_localWork->timer>0){
      _localWork->timer--;
    }
    else{
      _localWork->bIrc = FALSE;
    }
    retcode |= GAME_COMM_STATUS_BIT_IRC;
  }
#else
  retcode |= GAME_COMM_STATUS_BIT_IRC;
#endif
  
  if(-1 != WIH_DWC_TVTCallCheck()){  //TVT�͍ŗD��
    retcode |= GAME_COMM_STATUS_BIT_WIRELESS_TR;
  }
  if(aNetStruct->gsid == WB_NET_PALACE_SERVICEID){  //�p���X�łȂ����Ă��锻��
    if(GFL_NET_GetConnectNum()>1){
      retcode |= GAME_COMM_STATUS_BIT_WIRELESS;
    }
  }
  for( i=0;i < aNetStruct->maxBeaconNum;i++ ){
    if( GFL_NET_GetBeaconData( i ) != NULL ){
      GameServiceID id = GFL_NET_WLGetGameServiceID(i);
      NAGI_Printf( "ID%d idx%d\n", id, i );
      switch(id){
      case WB_NET_UNION:
        retcode |= GAME_COMM_STATUS_BIT_WIRELESS_UN;
        break;
      case WB_NET_MYSTERY:
        retcode |= GAME_COMM_STATUS_BIT_WIRELESS_FU;
        break;
      default:
        break;
      }
    }
  }
  //NAGI_Printf( "ret %d\n", retcode );
  
  for(i=0;i < _localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    if(pS->timer == 0){
      continue;
    }
    {
      WMBssDesc* bss = &pS->sBssDesc;
    
      DWCApInfo ap;

      if(_scanAPReserveed(bss)){
        retcode |= GAME_COMM_STATUS_BIT_WIFI;
      }
      else if(_scanFreeSpot(bss)){
        retcode |= GAME_COMM_STATUS_BIT_WIFI_ZONE;
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
          retcode |= GAME_COMM_STATUS_BIT_WIFI;
          break;
        case DWC_APINFO_TYPE_SHOP://         :  �j���e���h�[Wi-Fi�X�e�[�V����
        case DWC_APINFO_TYPE_FREESPOT://     :  FREESPOT�i�t���[�X�|�b�g�j
        case DWC_APINFO_TYPE_WAYPORT://      :  Wayport(�k�ăz�b�g�X�|�b�g)�����݂͎g�p�ł��܂���
        case DWC_APINFO_TYPE_NINTENDOZONE:// : Nintendo Zone
          retcode |= GAME_COMM_STATUS_BIT_WIFI_ZONE;
          break;
        }
      }
    }
  }
  for(i=0;i < _localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    if(pS->timer == 0){
      continue;
    }
    if(!_scanPrivacy(&pS->sBssDesc)){
      retcode |= GAME_COMM_STATUS_BIT_WIFI_FREE;
    }
  }
  if(_localWork->AllBeaconNum>0){
    retcode |= GAME_COMM_STATUS_BIT_WIFI_LOCK;
  }
  //NAGI_Printf( "ret %d\n", retcode );
  return retcode;
}


const BOOL WIH_DWC_IsEnableBeaconData( const u8 idx )
{
  if( _localWork->aBeaconCatch[idx].timer > 0 )
  {
    return TRUE;
  }
  return FALSE;
}

const WMBssDesc* WIH_DWC_GetBeaconData( const u8 idx )
{
  return &_localWork->aBeaconCatch[idx].sBssDesc;
}


//------------------------------------------------------------------------------
/**
 * @brief   AP���𓾂�
 * @retval  GAME_COMM_STATUS
 */
//------------------------------------------------------------------------------
void WIH_DWC_CreateCFG(HEAPID id)
{
  GF_ASSERT(_localcfg==NULL);

  _localcfg = GFL_NET_Align32Alloc(id ,sizeof(NCFGConfigEx));
}




//------------------------------------------------------------------------------
/**
 * @brief   AP���J��
 * @retval  none
 */
//------------------------------------------------------------------------------
void WIH_DWC_DeleteCFG(void)
{
  GF_ASSERT(_localcfg);
  GFL_NET_Align32Free(_localcfg);
  _localcfg = NULL;
}
//------------------------------------------------------------------------------
/**
 * @brief   AP���ēǂݍ���
 * @retval  void
 */
//------------------------------------------------------------------------------
void WIH_DWC_ReloadCFG(void)
{
  GF_ASSERT(_localcfg);
  NCFG_ReadConfig(&_localcfg->compat, NULL);
}

//------------------------------------------------------------------------------
/**
 * @brief   TV�g�����V�[�o�[�ɌĂ΂�Ă��邩�ǂ���
 * @retval  �Ă΂�Ă���beaconindex��Ԃ� �Ă΂�Ė����ꍇ-1
 */
//------------------------------------------------------------------------------
int WIH_DWC_TVTCallCheck(void)
{
  u8 selfMacAdr[6];
  int i,num;

  if(GFL_NET_IsInit()){
    GFLNetInitializeStruct* pIn = GFL_NET_GetNETInitStruct();

    num = pIn->maxBeaconNum;
    OS_GetMacAddress( selfMacAdr );

    for( i = 0; i < num; i++ ){
      int x = GFL_NET_WLGetUserGameServiceId( i );
      if(WB_NET_COMM_TVT == x){
        if( CTVT_BCON_CheckCallSelf( GFL_NET_GetBeaconData(i) , selfMacAdr )){
        return i;
        }
      }
    }
  }
  return -1;
}
