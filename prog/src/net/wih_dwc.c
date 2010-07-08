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
//  u16 timer;
//  u16 level;
} _BEACONCATCH_STR;

struct _WIH_DWC_WORK {
  _BEACONCATCH_STR aBeaconCatch[_ALLBEACON_MAX];
  void* ScanMemory;
  NCFGConfigEx* cfg;
 // BOOL bIrc;
  int timer;
  u16 bStop;
  u16 levelTimer;
	u16 AllBeaconNum;
  u16 level;
  u16 levelGame;
  u16 loopkeep;
  int timeWIFI;
  int timeZONE;
  int timeFREE;
  int timeLOCK;
};

#define _DEFAULT_FRAME (60*10)
#define _SHORT_FRAME (40*10)

static void _SetScanBeaconData(WMBssDesc* pBss,void* pWork,u16 level);
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
  }
}


/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

static void _SetScanBeaconData(WMBssDesc* pBss,void* pWork,u16 level)
{
  int i;

  if(_localWork->AllBeaconNum==0){
    return;
  }
  if(_localWork->loopkeep >= _localWork->AllBeaconNum){
    return;
  }
  if(_localWork->loopkeep >= _ALLBEACON_MAX){
    return;
  }
  {
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[ _localWork->loopkeep ];
    GFL_STD_MemCopy(pBss, &pS->sBssDesc, sizeof(WMBssDesc));
  }
  _localWork->loopkeep++;
}



/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

static void _ResetScanBeaconData(void)
{
  _localWork->loopkeep=0;
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
    if(_localWork->timeWIFI>0){
      _localWork->timeWIFI--;
    }
    if(_localWork->timeZONE>0){
      _localWork->timeZONE--;
    }
    if(_localWork->timeFREE>0){
      _localWork->timeFREE--;
    }
    if(_localWork->timeLOCK>0){
      _localWork->timeLOCK--;
    }
  }
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
    int len = GFL_STD_StrLen(_localWork->cfg->slot[i].ap.ssid[0]);
    if(len > 0){
     // NET_PRINT("SSIDCHK %s\n",pChk->ssid);
      if(0==GFL_STD_MemComp(_localWork->cfg->slot[i].ap.ssid[0], pChk->ssid, len)){
      //  NET_PRINT("SSIDFIND %s\n",pChk->ssid);
        return TRUE;
      }
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

#if 0
/*---------------------------------------------------------------------*
  Name:         WMSP_GetRssi8

  Description:  WL��RSSI�̃t�H�[�}�b�g����8�r�b�g�l�ɕϊ�����B

  Arguments:    rssi - WL�t�H�[�}�b�g��RSSI�B

  Returns:      8�r�b�g�l�ɕϊ����ꂽRSSI��Ԃ��B
 *--------------------------------------------------------------------*/
static u8 WMSP_GetRssi8(u8 rssi)
{
	if (rssi & 0x0002)
	{
		return (u8)(rssi >> 2);
	}
	return (u8)((rssi >> 2) + 25);
}
#endif


//------------------------------------------------------------------------------
/**
 * @brief   �Z�L�����e�B�[�r�b�g�����邩�ǂ���
 * @param   ��������r�[�R��
 * @retval  ��������TRUE
 */
//------------------------------------------------------------------------------

static BOOL _scanPrivacy( WMBssDesc* pChk )
{

  if((pChk->capaInfo & WCM_NETWORK_CAPABILITY_PRIVACY)){
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
GAME_COMM_STATUS WIH_DWC_GetAllBeaconType(WIFI_LIST * list)
{
  int i;
  GAME_COMM_STATUS_BIT bit  = WIH_DWC_GetAllBeaconTypeBit(list);

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
GAME_COMM_STATUS_BIT WIH_DWC_GetAllBeaconTypeBit(WIFI_LIST * list)
{ 
  int i;
  GAME_COMM_STATUS_BIT retcode = 0;
  u32 getBIT=0;
  const GFLNetInitializeStruct *aNetStruct = GFL_NET_GetNETInitStruct();

  if(_localWork==NULL){
    return retcode;
  }
  
  if(-1 != WIH_DWC_TVTCallCheck(list)){  //TVT�͍ŗD��
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
      if((id >= WB_NET_UNION) && (id <=WB_NET_UNION_GURUGURU)){
        retcode |= GAME_COMM_STATUS_BIT_WIRELESS_UN;
      }
      else if(id == WB_NET_MYSTERY){
        retcode |= GAME_COMM_STATUS_BIT_WIRELESS_FU;
      }
    }
  }
  //NAGI_Printf( "ret %d\n", retcode );
  
  for(i=0;i < _localWork->loopkeep;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    {
      WMBssDesc* bss = &pS->sBssDesc;
      DWCApInfo ap;

      if(_scanAPReserveed(bss)){
        _localWork->timeWIFI = _DEFAULT_FRAME;
      }
      else if(_scanFreeSpot(bss)){
        _localWork->timeZONE = _DEFAULT_FRAME;
      }
      else if(DWC_ParseWMBssDesc(_localWork->ScanMemory,bss,&ap)){
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
          _localWork->timeWIFI = _DEFAULT_FRAME;
          break;
        case DWC_APINFO_TYPE_SHOP://         :  �j���e���h�[Wi-Fi�X�e�[�V����
        case DWC_APINFO_TYPE_FREESPOT://     :  FREESPOT�i�t���[�X�|�b�g�j
        case DWC_APINFO_TYPE_WAYPORT://      :  Wayport(�k�ăz�b�g�X�|�b�g)�����݂͎g�p�ł��܂���
        case DWC_APINFO_TYPE_NINTENDOZONE:// : Nintendo Zone
          _localWork->timeZONE = _DEFAULT_FRAME;
          break;
        }
      }
      else{
        if(pS->sBssDesc.ssidLength!=0){
          if(!_scanPrivacy(&pS->sBssDesc)){
            _localWork->timeFREE = _SHORT_FRAME;
          }
          else{
            _localWork->timeLOCK = _SHORT_FRAME;
          }
        }
      }
    }
  }
  if(_localWork->timeWIFI > 0){
    retcode |= GAME_COMM_STATUS_BIT_WIFI;
  }
  if(_localWork->timeZONE > 0){
    retcode |= GAME_COMM_STATUS_BIT_WIFI_ZONE;
  }
  if(_localWork->timeFREE > 0){
    retcode |= GAME_COMM_STATUS_BIT_WIFI_FREE;
  }
  if(_localWork->timeLOCK > 0){
    retcode |= GAME_COMM_STATUS_BIT_WIFI_LOCK;
  }

  if( _localWork->levelTimer > 0){
    _localWork->levelTimer--;
  }
  if(retcode==0){
    _localWork->level=0;
  }
  else if(_localWork->levelGame!=0){
    if(_localWork->levelTimer == 0){
      _localWork->level = _localWork->levelGame;
      _localWork->levelTimer = _SHORT_FRAME;
      _localWork->levelGame = 0;
    }
  }
  _localWork->loopkeep=0;
  retcode |= GAME_COMM_STATUS_BIT_IRC;
  return retcode;
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
int WIH_DWC_TVTCallCheck(WIFI_LIST * list)
{
  u8 selfMacAdr[6];
  int i,num;

  if(GFL_NET_IsInit()){
    GFLNetInitializeStruct* pIn = GFL_NET_GetNETInitStruct();

    num = pIn->maxBeaconNum;
    OS_GetMacAddress( selfMacAdr );

    for( i = 0; i < SCAN_PARENT_COUNT_MAX; i++ ){
      int x = GFL_NET_WLGetUserGameServiceId( i );
      if(WB_NET_COMM_TVT == x){
        if( CTVT_BCON_CheckCallSelf( GFL_NET_GetBeaconData(i) , list , selfMacAdr )){
          return i;
        }
      }
    }
  }
  return -1;
}

//------------------------------------------------------------------------------
/**
 * @brief   �E�����r�[�R���̋��x�̍ő���l�i�K�ɂ��Ă�����
 * @retval  ���x
 */
//------------------------------------------------------------------------------

int WIH_DWC_GetBeaconRssiMax(void)
{
  if(_localWork){
    return _localWork->level;
  }
  return 0;
}


//------------------------------------------------------------------------------
/**
 * @brief   ���x���Z�b�g
 * @retval  ���x
 */
//------------------------------------------------------------------------------
void WIH_DWC_SetLevel(u8 level)
{
  if(_localWork){
    if(_localWork->levelGame < level && level < 4){
      _localWork->levelGame = level;
    }
  }
}

void WIH_DWC_ResetLevel(void)
{
 // if(_localWork){
//    _localWork->levelGame = 0;
//  }
}
