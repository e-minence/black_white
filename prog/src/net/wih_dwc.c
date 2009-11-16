//=============================================================================
/**
 * @file	  wih_dwc.c
 * @brief	  �r�[�R����͂��s���AAP�̔��ʂ��s��
 * @author	k.ohno
 * @date    2009.11.15
 */
//=============================================================================

#include "wih_dwc.h"

#define _ALLBEACON_MAX (4)

typedef struct{
	WMBssDesc sBssDesc;
  int timer;
} _BEACONCATCH_STR;

struct _WIH_DWC_WORK {
  _BEACONCATCH_STR aBeaconCatch[_ALLBEACON_MAX];
  void* ScanMemory;
	u16 AllBeaconNum;
};


static void _SetScanBeaconData(WMBssDesc* pBss);
static WIH_DWC_WORK* _localWork=NULL;

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

  if(_localWork){
    for(i=0;i<_localWork->AllBeaconNum;i++){
      _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
      if(pS->timer > 0){
        pS->timer--;
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���̃^�C�v�𓾂�
 * @param   �r�[�R����
 * @param   �q�[�vID
 * @retval  GAME_COMM_STATUS
 */
//------------------------------------------------------------------------------

GAME_COMM_STATUS WIH_DWC_GetAllBeaconType(void)
{
  int i;
  int retcode = GAME_COMM_STATUS_NULL;

  if(_localWork==NULL){
    return retcode;
  }
  
  for(i=0;i < _localWork->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = &_localWork->aBeaconCatch[i];
    if(pS->timer == 0){
      continue;
    }
    {
      WMBssDesc* bss = &pS->sBssDesc;
    
      DWCApInfo ap;

      if(DWC_ParseWMBssDesc(_localWork->ScanMemory,bss,&ap)){
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
        if(retcode!=GAME_COMM_STATUS_NULL){
          return retcode;
        }
      }
    }
  }
  ///@todo �Z�L�����e�B�[���������Ă��邩�ǂ���
  return retcode;
}

