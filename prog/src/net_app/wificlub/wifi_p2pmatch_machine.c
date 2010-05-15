
//------------------------------------------------------------------
/**
 * @brief   �e�@�������̃��M�����[�V�������� WIFIP2PMATCH_PLAYERMACHINE_INIT1
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerMachineInit1( WIFIP2PMATCH_WORK *wk, int seq )
{
  u8 command;
  int gmmno,gmmidx;
  u16 friendNo,status;
  WIFI_STATUS* p_status;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;
  u32 fail_bit;
  u16 gamemode = _WifiMyGameModeGet( wk, wk->pMatch );

  wk->pParentWork->btalk = FALSE;  // 
  switch(gamemode){
  case WIFI_GAME_VCT:
    {
      GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
//      GFL_NET_DWC_StartVChat();
      _vctcheckCommon( wk );

      _changeBGMVol(wk, 0);
      
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_1015, FALSE);
      wk->VChatModeOff = FALSE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
      return seq;
    }
    break;
  case WIFI_GAME_TVT:
    {
      wk->endSeq = gamemode;
      return SEQ_OUT;            //�I���V�[�P���X��
    }
    break;
  case WIFI_GAME_TRADE:
    {
      _vctcheckCommon( wk );
      wk->endSeq = gamemode;
      return SEQ_OUT;            //�I���V�[�P���X��
    }
    break;
  }

  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),FALSE);  //�ڑ��֎~
  if(GFL_NET_IsParentMachine()){
    if(POKE_REG_OK!=_CheckRegulation_Temoti(wk->pRegulation, wk->pGameData, &fail_bit )){
      if(POKE_REG_OK!=_CheckRegulation_BBox(wk->pRegulation, wk->bb_party, &fail_bit )){
        // �I�Ԏ����ł��Ȃ�
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
        _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT);
        return seq;
      }
    }

  //  command = WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE;  //��������DIRECT�Ƃ��Ȃ�  ���M�����[�V�����𑗂�Ƃ���
  //  GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,1,&command);
    
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE);
  }
  else{
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   ���M�����[�V����������Ȃ����� WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _playerMachineNoregParent( WIFIP2PMATCH_WORK *wk, int seq )
{
  u16 command;

  if(!WifiP2PMatchMessageEndCheck(wk)){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg()){
    EndMessageWindowOff(wk);
    command = WIFIP2PMATCH_PLAYERDIRECT_END;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND, 1, &command);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
 
}


//------------------------------------------------------------------
/**
 * @brief   �w�胂�[�h�I�� WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _playerMachineBattleDecide( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 fail_bit;
  u32 regulation = _createRegulation(wk);
  
  if(POKE_REG_OK!=_CheckRegulation_Temoti(wk->pRegulation, wk->pGameData, &fail_bit )){
    if(POKE_REG_OK!=_CheckRegulation_BBox(wk->pRegulation, wk->bb_party, &fail_bit )){
      // �I�Ԏ����ł��Ȃ�
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
      _CHANGESTATE(wk, WIFIP2PMATCH_MESSAGEEND_RETURNLIST);
//      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT);
      return seq;
    }
  }

  {
    u16 gamemode;
    u32 modetbl[]={
      WIFI_GAME_BATTLE_SINGLE_ALL,      // �V���O���o�g��
      WIFI_GAME_BATTLE_SINGLE_FLAT,      // �V���O���o�g��
      WIFI_GAME_BATTLE_DOUBLE_ALL,      // �_�u��
      WIFI_GAME_BATTLE_DOUBLE_FLAT,      // �_�u��
      WIFI_GAME_BATTLE_TRIPLE_ALL,      // �g���v��
      WIFI_GAME_BATTLE_TRIPLE_FLAT,      // �g���v��
      WIFI_GAME_BATTLE_ROTATION_ALL,      // ���[�e�[�V����
      WIFI_GAME_BATTLE_ROTATION_FLAT,      // ���[�e�[�V����
    };
    gamemode = modetbl[wk->battleMode*2+wk->battleRule];
    _myStatusChange(wk, WIFI_STATUS_RECRUIT ,gamemode);
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST);

    WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    FriendRequestWaitOn( wk, TRUE );
  }
  return seq;

}


//msg_wifilobby_1012


//TRUE
#if defined(SDK_TWL)
//restrictPhoto = OS_IsRestrictPhotoExchange();
#endif

