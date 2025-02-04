
//------------------------------------------------------------------
/**
 * @brief   親機が自分のレギュレーション検査 WIFIP2PMATCH_PLAYERMACHINE_INIT1
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

  switch(gamemode){
  case WIFI_GAME_VCT:
    {
      GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
//      GFL_NET_DWC_StartVChat();
      _vctcheckCommon( wk );

      _changeBGMVol(wk, 0);
      
      WifiP2PMatchMessagePrintDirect(wk, msg_wifilobby_1015, FALSE);
      wk->VChatModeOff = FALSE;
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
      return seq;
    }
    break;
  case WIFI_GAME_TVT:
    {
      wk->endSeq = gamemode;
      return SEQ_OUT;            //終了シーケンスへ
    }
    break;
  case WIFI_GAME_TRADE:
    {
      _vctcheckCommon( wk );
      wk->endSeq = gamemode;
      return SEQ_OUT;            //終了シーケンスへ
    }
    break;
  }

  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),FALSE);  //接続禁止
  if(GFL_NET_IsParentMachine()){
    if(POKE_REG_OK!=_CheckRegulation_Temoti(wk->pRegulation, wk->pGameData, &fail_bit )){
      if(POKE_REG_OK!=_CheckRegulation_BBox(wk->pRegulation, wk->bb_party, &fail_bit )){
        // 選ぶ事ができない
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
        _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT);
        return seq;
      }
    }

  //  command = WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE;  //ここからDIRECTとおなじ  レギュレーションを送るところ
  //  GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,1,&command);
    
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE);
  }
  else{
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_DIRECTS, WB_NET_WIFICLUB );
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT_ST);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   レギュレーションがあわなかった WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT
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
//    command = WIFIP2PMATCH_PLAYERDIRECT_END;
//    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND, 1, &command);
    wk->command = WIFIP2PMATCH_PLAYERDIRECT_END;
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND);
  }
  return seq;
 
}


//------------------------------------------------------------------
/**
 * @brief   指定モード終了 WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _playerMachineBattleDecide( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 fail_bit;
  u32 regulation = _createRegulationType(wk, REG_CREATE_TYPE_SELECT);
  
  if(!_regulationCheck(wk)){    // 選ぶ事ができない
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT );
    return seq;
  }

  {
    u16 gamemode;
    u32 modetbl[]={
      WIFI_GAME_BATTLE_SINGLE_ALL,      // シングルバトル
      WIFI_GAME_BATTLE_SINGLE_FLAT,      // シングルバトル
      WIFI_GAME_BATTLE_DOUBLE_ALL,      // ダブル
      WIFI_GAME_BATTLE_DOUBLE_FLAT,      // ダブル
      WIFI_GAME_BATTLE_TRIPLE_ALL,      // トリプル
      WIFI_GAME_BATTLE_TRIPLE_FLAT,      // トリプル
      WIFI_GAME_BATTLE_ROTATION_ALL,      // ローテーション
      WIFI_GAME_BATTLE_ROTATION_FLAT,      // ローテーション
    };
    gamemode = modetbl[wk->battleMode*2+wk->battleRule];
    _myStatusChange(wk, WIFI_STATUS_RECRUIT ,gamemode);
    WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    _CHANGESTATE(wk,WIFIP2PMATCH_MODE_FRIENDLIST_MW);
//    _CHANGESTATE(wk,  WIFIP2PMATCH_MODE_FRIENDLIST_MW0);
  //  WifiP2PMatch_CommWifiBattleStart( wk, -1 );
    FriendRequestWaitOn( wk, TRUE );
  }
  return seq;

}


//msg_wifilobby_1012


//TRUE
#if defined(SDK_TWL)
//restrictPhoto = OS_IsRestrictPhotoExchange();
#endif

