



/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

void WH_AllBeaconStart(int num)
{
  int i;
  GF_ASSERT( _pWmInfo->ScanMemory==NULL);
  GF_ASSERT(num <= _ALLBEACON_MAX);
  _pWmInfo->AllBeaconNum = num;
  for(i=0;i<num;i++){
    _pWmInfo->pBeaconCatch[i] = GFL_NET_Align32Alloc(_pWmInfo->heapID , sizeof(_BEACONCATCH_STR));
  }
  _pWmInfo->ScanMemory =GFL_NET_Align32Alloc(_pWmInfo->heapID ,DWC_GetParseWMBssDescWorkSize());

}

/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

void WH_AllBeaconEnd(void)
{
  int i;

  GF_ASSERT( _pWmInfo->ScanMemory);
  for(i=0;i<_pWmInfo->AllBeaconNum;i++){
    GFL_NET_Align32Free(_pWmInfo->pBeaconCatch[i]);
  }
  GFL_NET_Align32Free(_pWmInfo->ScanMemory);
  _pWmInfo->ScanMemory=NULL;
  _pWmInfo->AllBeaconNum = 0;
}

/*---------------------------------------------------------------------------*
  Name:         WHSetAllBeaconFlg
  Description:  �r�[�R���Ȃ牽�ł����W���ĕ\������CGEAR�p
  Arguments:    bFlg  ON�Ȃ���W
 *---------------------------------------------------------------------------*/

static void _SetScanBeaconData(WMBssDesc* pBss)
{
  int i;

  if(_pWmInfo->AllBeaconNum==0){
    return;
  }
  
  for(i=0;i<_pWmInfo->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = _pWmInfo->pBeaconCatch[i];
      if(GFL_STD_MemComp(&pS->sBssDesc, pBss, sizeof(WMBssDesc))==0){ //��v
        pS->timer = DEFAULT_TIMEOUT_FRAME;
        return;
      }
  }
  for(i=0;i<_pWmInfo->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = _pWmInfo->pBeaconCatch[i];
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

void WH_MainLoopScanBeaconData(void)
{
  int i;

  if(_pWmInfo && _pWmInfo->pBeaconCatch){
    for(i=0;i<_pWmInfo->AllBeaconNum;i++){
      _BEACONCATCH_STR* pS = _pWmInfo->pBeaconCatch[i];
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

GAME_COMM_STATUS WH_GetAllBeaconType(void)
{
  int i;
  int retcode = GAME_COMM_STATUS_NULL;

  for(i=0;i < _pWmInfo->AllBeaconNum;i++){
    _BEACONCATCH_STR* pS = _pWmInfo->pBeaconCatch[i];
    if(pS->timer == 0){
      continue;
    }
    {
      WMBssDesc* bss = &pS->sBssDesc;
    
      DWCApInfo ap;

      if(DWC_ParseWMBssDesc(_pWmInfo->ScanMemory,bss,&ap)){
        OS_TPrintf("DWCApInfo %d\n",ap.aptype);
        switch(ap.aptype){
        case DWC_APINFO_TYPE_USER0:
        case DWC_APINFO_TYPE_USER1:
        case DWC_APINFO_TYPE_USER2:
        case DWC_APINFO_TYPE_USER3:
        case DWC_APINFO_TYPE_USER4:
        case DWC_APINFO_TYPE_USER5:
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

