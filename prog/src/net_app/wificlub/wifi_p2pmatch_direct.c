




//------------------------------------------------------------------
/**
 * @brief   �܂��q�@���\�����݂��󂯂邩�ǂ����I�� WIFIP2PMATCH_PLAYERDIRECT_INIT1
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectInit1( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno,gmmidx;
  u16 friendNo,status,gamemode;
  WIFI_STATUS* p_status;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;

  wk->pParentWork->btalk = TRUE;  //�_�C���N�g

  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),FALSE);  //�ڑ��֎~
  if(!GFL_NET_IsParentMachine()){
    _friendNameExpand(wk,  wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_083, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT2);
  }
  else{
    _friendNameExpand(wk,  wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1018, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �܂��q�@���\�����݂��󂯂邩�ǂ����I�� WIFIP2PMATCH_PLAYERDIRECT_INIT2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectInit2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }

  _yenowinCreateM2(wk);

  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT3);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �܂��q�@���\�����݂��󂯂邩�ǂ����I�� WIFIP2PMATCH_PLAYERDIRECT_INIT3
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectInit3( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret;
  u8 command;

  GFL_FONTSYS_SetDefaultColor();
  ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }
  else if(ret == 0){ // �͂���I�������ꍇ
    command = WIFIP2PMATCH_PLAYERDIRECT_INIT5;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                     1, &command);
  }
  else{  // ��������I�������ꍇ
    command = WIFIP2PMATCH_PLAYERDIRECT_END;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                     1, &command);
  }
  EndMessageWindowOff(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_INIT5
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectInit5( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno,gmmidx;
  u16 friendNo,status,gamemode;
  WIFI_STATUS* p_status;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;

  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(),FALSE);  //�ڑ��֎~
  if(GFL_NET_IsParentMachine()){
    _friendNameExpand(wk,  wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_084, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT6);
  }
  else{
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_073, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_INIT6
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectInit6( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  _ParentModeSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_INIT7);
  return seq;
}





//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_INIT7
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectInit7( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret = BMPMENULIST_NULL;
  int command;

  PRINT_UTIL_Trans( &wk->SysMsgPrintUtil, wk->SysMsgQue );
  ret = BmpMenuList_Main(wk->sublw);

  if(BMPMENULIST_NULL!=ret){
    EndMessageWindowOff(wk);
  }
  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    command = WIFIP2PMATCH_PLAYERDIRECT_END;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND, 1, &command);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
    break;
  case WIFI_GAME_VCT:
    if(WIFI_STATUS_GetVChatStatus( wk->pMatch )==FALSE){
      
      return seq;
    }
    else{
      command = WIFIP2PMATCH_PLAYERDIRECT_VCT;
      GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                       1, &command);
      _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
    }
    break;
  case WIFI_GAME_TVT:
    if(WIFI_STATUS_GetVChatStatus( wk->pMatch )==FALSE){
      
      return seq;
    }
    else{
      command=WIFIP2PMATCH_PLAYERDIRECT_TVT;
      GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                       1, &command);
      _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
    }
    break;
  case WIFI_GAME_TRADE:
    {
      if(FALSE == _tradeNumCheck(wk)){
        WifiP2PMatchMessagePrint(wk, msg_wifilobby_1013, FALSE);
        _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_RETURN);
      }
      else{
        command = WIFIP2PMATCH_PLAYERDIRECT_TRADE;
        GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                         1, &command);
        _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
      }
    }
    break;
  case WIFI_GAME_BATTLE_SINGLE_ALL:
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE1);
    break;
  }
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[_MENUTYPE_GAME]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_RETURN
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectReturn( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_INIT5);
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �q�@���Ȃɂ��邩�ǂ����I�� 
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectSubMain( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno,gmmidx;
  u16 friendNo,status,gamemode;
  u32 way;
  int message[]={msg_wifilobby_085,msg_wifilobby_086,msg_wifilobby_087};

  if(!GFL_NET_IsParentMachine()){
    _friendNameExpand(wk,  wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, message[  wk->directmode - WIFIP2PMATCH_PLAYERDIRECT_VCT ], FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_SUB1);
  }
  else{
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �q�@��VCT���邩�ǂ����I�� WIFIP2PMATCH_PLAYERDIRECT_VCT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectVCT( WIFIP2PMATCH_WORK *wk, int seq )
{
  wk->directmode = WIFIP2PMATCH_PLAYERDIRECT_VCT;
  return _playerDirectSubMain(wk,seq);
}

//------------------------------------------------------------------
/**
 * @brief   �q�@��TVT���邩�ǂ����I�� WIFIP2PMATCH_PLAYERDIRECT_TVT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectTVT( WIFIP2PMATCH_WORK *wk, int seq )
{
  wk->directmode = WIFIP2PMATCH_PLAYERDIRECT_TVT;
  return _playerDirectSubMain(wk,seq);
}


//------------------------------------------------------------------
/**
 * @brief   �q�@��TRADE���邩�ǂ����I�� WIFIP2PMATCH_PLAYERDIRECT_TRADE
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectTrade( WIFIP2PMATCH_WORK *wk, int seq )
{
  wk->directmode = WIFIP2PMATCH_PLAYERDIRECT_TRADE;
  return _playerDirectSubMain(wk,seq);
}



//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_SUB1
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectSub1( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  _yenowinCreateM2(wk);
  _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_SUB2);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_SUB2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectSub2( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret;
  u8 command;
  GFL_FONTSYS_SetDefaultColor();
  ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }
  else if(ret == 0){ // �͂���I�������ꍇ
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_SUBSTARTCALL);
  }
  else{  // ��������I�������ꍇ
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_ENDCALL);
  }
  EndMessageWindowOff(wk);

 // _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_SUBSTARTCALL
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectSubstartCall( WIFIP2PMATCH_WORK *wk, int seq )
{
  u8 command;
  
  command = WIFIP2PMATCH_PLAYERDIRECT_SUBSTART;
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND, 1, &command)){
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_ENDCALL
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectEndCall( WIFIP2PMATCH_WORK *wk, int seq )
{
  u8 command;
  
  command = WIFIP2PMATCH_PLAYERDIRECT_END;
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND, 1, &command)){
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * @brief   VCT�J�n   WIFIP2PMATCH_PLAYERDIRECT_SUBSTART
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectSubStart( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status,gamemode;


  WIFI_STATUS_ResetVChatMac(wk->pMatch);
  status = WIFI_STATUS_PLAYING;

  switch(wk->directmode){
  case WIFIP2PMATCH_PLAYERDIRECT_VCT:
    gamemode = WIFI_GAME_VCT;
    break;
  case WIFIP2PMATCH_PLAYERDIRECT_TVT:
    gamemode = WIFI_GAME_TVT;
    break;
  case WIFIP2PMATCH_PLAYERDIRECT_TRADE:
    gamemode = WIFI_GAME_TRADE;
    break;
  }
  _myStatusChange(wk, status,gamemode);  // �ڑ����ɂȂ�
  wk->pParentWork->btalk = TRUE;  //�_�C���N�g

  switch(wk->directmode){
  case WIFIP2PMATCH_PLAYERDIRECT_VCT:
    {
      GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
      GFL_NET_DWC_StartVChat();
      _CHANGESTATE(wk,WIFIP2PMATCH_MODE_VCT_CONNECT);
    }
    break;
  case WIFIP2PMATCH_PLAYERDIRECT_TVT:
  case WIFIP2PMATCH_PLAYERDIRECT_TRADE:
    {
      WIFI_STATUS* p_status;
      EndMessageWindowOff(wk);

      wk->endSeq = gamemode;

      return SEQ_OUT;            //�I���V�[�P���X��
    }
    break;
  }
  return seq;
}




//------------------------------------------------------------------
/**
 * @brief   �o�g���̃��M�����[�V�����I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE1
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattle1( WIFIP2PMATCH_WORK *wk, int seq )
{

  _battleCustomSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE2);
  return seq;
}





//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattle2( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret = BMPMENULIST_NULL;
  int command;

  PRINT_UTIL_Trans( &wk->SysMsgPrintUtil, wk->SysMsgQue );
  ret = BmpMenuList_Main(wk->sublw);

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    if(wk->pParentWork->btalk){  //��b�̗���
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_084, FALSE);
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_INIT6);
    }
    else{  //�f���̗���
      _CHANGESTATE(wk, WIFIP2PMATCH_MODE_SELECT_INIT);
    }
    break;
  default:
    if(wk->pParentWork->btalk){  //��b�̗���
      _CHANGESTATE(wk, ret);
    }
    else{  //�f���̗���
      if(WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE==ret){
        _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE);
      }
      else{
        _CHANGESTATE(wk, ret);
      }
    }
    break;
  }
  EndMessageWindowOff(wk);
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[_MENUTYPE_BATTLE_CUSTOM]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleMode( WIFIP2PMATCH_WORK *wk, int seq )
{

  _battleModeSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE2);
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleMode2( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret = BMPMENULIST_NULL;
  int command;

  PRINT_UTIL_Trans( &wk->SysMsgPrintUtil, wk->SysMsgQue );
  ret = BmpMenuList_Main(wk->sublw);

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    break;
  default:
    wk->battleMode = ret;
    break;
  }
  _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE1);
  EndMessageWindowOff(wk);
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL,  &wk->singleCur[_MENUTYPE_BATTLE_MODE]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleRule( WIFIP2PMATCH_WORK *wk, int seq )
{
  _battleRuleSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE2);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleRule2( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret = BMPMENULIST_NULL;
  int command;

  PRINT_UTIL_Trans( &wk->SysMsgPrintUtil, wk->SysMsgQue );
  ret = BmpMenuList_Main(wk->sublw);

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    break;
  default:
    wk->battleRule = ret;
    break;
  }
  _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE1);
  EndMessageWindowOff(wk);
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[_MENUTYPE_BATTLE_RULE]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleShooter( WIFIP2PMATCH_WORK *wk, int seq )
{
  _battleShooterSelectMenu(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER2);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �e�@���������邩�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleShooter2( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret = BMPMENULIST_NULL;
  int command;

  PRINT_UTIL_Trans( &wk->SysMsgPrintUtil, wk->SysMsgQue );
  ret = BmpMenuList_Main(wk->sublw);

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    break;
  default:
    wk->battleShooter = 1-ret;
    break;
  }
  _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE1);
  EndMessageWindowOff(wk);
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[_MENUTYPE_BATTLE_SHOOTER]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;
}



//--------------------------------------------------------------
/**
 * �莝���|�P�����̃��M�����[�V�����`�F�b�N
 *
 * @param   REGULATION
 * @param   GAMEDATA
 * @param   fail_bit		
 * @retval  POKE_REG_RETURN_ENUM		
 */
//--------------------------------------------------------------
static POKE_REG_RETURN_ENUM _CheckRegulation_Temoti(REGULATION* pReg,GAMEDATA* pGameData,u32 *fail_bit)
{
  POKE_REG_RETURN_ENUM reg_ret;
  
  *fail_bit = 0;
  reg_ret =
    PokeRegulationMatchLookAtPokeParty(pReg, 
                                       GAMEDATA_GetMyPokemon(pGameData), fail_bit);
  return reg_ret;
}

//--------------------------------------------------------------
/**
 * �o�g���{�b�N�X��POKEPARTY��Alloc���č쐬���܂�
 *
 * @param   unisys		
 *
 * @retval  POKEPARTY *		Alloc����POKEPARTY (�o�g���{�b�N�X�������ꍇ��NULL)
 */
//--------------------------------------------------------------
static POKEPARTY * _BBox_PokePartyAlloc(GAMEDATA* pGameData)
{
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(pGameData);
  BATTLE_BOX_SAVE *bb_save = BATTLE_BOX_SAVE_GetBattleBoxSave( sv_ctrl );
  
  if(BATTLE_BOX_SAVE_IsIn( bb_save ) == FALSE){
    return NULL;
  }
  
  return BATTLE_BOX_SAVE_MakePokeParty( bb_save, HEAPID_WIFIP2PMATCH );
}

//--------------------------------------------------------------
/**
 * �o�g���{�b�N�X��POKEPARTY��Free���܂�
 * @param   party		
 */
//--------------------------------------------------------------
static void _BBox_PokePartyFree(POKEPARTY *party)
{
  GF_ASSERT(party != NULL);
  GFL_HEAP_FreeMemory(party);
}


//--------------------------------------------------------------
/**
 * �o�g���{�b�N�X�̃��M�����[�V�����`�F�b�N
 *
 * @param   unisys		
 * @param   fail_bit		
 *
 * @retval  POKE_REG_RETURN_ENUM		
 *
 * unisys->alloc.regulation�Ƀ��M�����[�V�����f�[�^�����[�h����Ă���K�v������܂�
 */
//--------------------------------------------------------------
static POKE_REG_RETURN_ENUM _CheckRegulation_BBox(REGULATION* pReg,GAMEDATA* pGameData, u32 *fail_bit)
{
  POKE_REG_RETURN_ENUM reg_ret=POKE_REG_NUM_FAILED;
  POKEPARTY *bb_party = _BBox_PokePartyAlloc(pGameData);
  
  *fail_bit = 0;
  
  if(bb_party != NULL){
    reg_ret = PokeRegulationMatchLookAtPokeParty(
      pReg, bb_party, fail_bit);
    _BBox_PokePartyFree(bb_party);
  }
  else{ //�o�g���{�b�N�X�̃Z�[�u�f�[�^�����݂��Ȃ�
    *fail_bit = 0xffffffff;
  }
  return reg_ret;
}

static u32 _regtable[]={
    REG_FREE_SINGLE, REG_FREE_DOUBLE, REG_FREE_TRIPLE, REG_FREE_ROTATION,
    REG_FLAT_SINGLE,REG_FLAT_DOUBLE,REG_FLAT_TRIPLE,REG_FLAT_ROTATION
    };


static u32 _createRegulation(WIFIP2PMATCH_WORK *wk)
{
  u32 regulation;

  NET_PRINT("regcr %d %d %d\n",wk->battleMode,wk->battleRule,wk->battleShooter);
  regulation = _regtable[wk->battleMode + (wk->battleRule*4)];
  if(wk->pRegulation){
    GFL_HEAP_FreeMemory((void*)wk->pRegulation);
  }
  wk->pRegulation = (REGULATION*)PokeRegulation_LoadDataAlloc(regulation, HEAPID_WIFIP2PMATCH);
  if(wk->battleShooter){
    regulation = regulation | 0x80000000;
  }
  return regulation;
}

static void _convertRegulation(WIFIP2PMATCH_WORK *wk,u32 regulation)
{
  int i;
  if(regulation | 0x80000000){
    wk->battleShooter = 1;
  }
  else{
    wk->battleShooter = 0;
  }
  regulation = (regulation & 0xfffffff);
  for(i=0;i<elementof(_regtable);i++){
    if(_regtable[i] == regulation){
      wk->battleMode = i % 4;
      wk->battleRule = i / 4;
    }
  }
  NET_PRINT("regch %d %d %d\n",wk->battleMode,wk->battleRule,wk->battleShooter);
}

//------------------------------------------------------------------
/**
 * @brief   �e�@������I��  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _playerDirectBattleDecide( WIFIP2PMATCH_WORK *wk, int seq )
{
  u8 command;
  u32 regulation = _createRegulation(wk);
  u32 fail_bit;


  if(!_regulationCheck(wk)){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT);
  }


  GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_REGLATION,
                   sizeof(u32), &regulation);
  
  command = WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO;
  GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                   1, &command);


  _friendNameExpand(wk,  wk->friendNo - 1);
  WifiP2PMatchMessagePrint(wk, msg_wifilobby_014, FALSE);

  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   ���M�����[�V����������Ȃ����� WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _playerDirectNoregParent( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(!WifiP2PMatchMessageEndCheck(wk)){
    return seq;
  }
  if(GFL_UI_KEY_GetTrg()){
    EndMessageWindowOff(wk);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE1);
  }
  return seq;
 
}


static int _playerDirectBattleWatch( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 regulation = _createRegulation(wk);
  _Menu_RegulationSetup(wk,0,1-wk->battleShooter ,REGWIN_TYPE_RULE);
  _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH2);
  return seq;
}


static int _playerDirectBattleWatch2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_UI_KEY_GetTrg()){
    wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE1);
  }
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �q�@������ꂽ���M�����[�V�����Ńo�g���ł��邩����   WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleGO( WIFIP2PMATCH_WORK *wk, int seq )
{
//  u8 command;

  if(!GFL_NET_IsParentMachine()){
    _friendNameExpand(wk,  wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1001+wk->battleMode+wk->battleRule*4, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO2);
  }
  else{
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}

static int _playerDirectBattleGO1( WIFIP2PMATCH_WORK *wk, int seq )
{
  return seq;
}


static int _playerDirectBattleGO2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  _yenowinCreateM2(wk);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3);
  return seq;
}

static int _playerDirectBattleGO3( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret;
  u8 command;
  GFL_FONTSYS_SetDefaultColor();
  ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }
  else if(ret == 0){ // �͂���I�������ꍇ
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO4);
  }
  else{  // ��������I�������ꍇ
    command = WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                     1, &command);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1010, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  EndMessageWindowOff(wk);
  return seq;
}
  

static int _playerDirectBattleGO4( WIFIP2PMATCH_WORK *wk, int seq )
{
  u8 command;
  {  
    u32 regulation = _createRegulation(wk);
    u32 fail_bit;

    if(!_regulationCheck(wk)){        // �I�Ԏ����ł��Ȃ�
      command = WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED;
      GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                       1, &command);
      WifiP2PMatchMessagePrint(wk, msg_wifilobby_100, FALSE);
      _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
      return seq;
    }
    command = WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,1,&command);
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  return seq;
}




//------------------------------------------------------------------
/**
 * @brief   �����J�n WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleStart( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 fail_bit;
  int id=0;
  
  if(!GFL_NET_IsParentMachine()){
    id = 1;
  }
  
  if(POKE_REG_OK!=_CheckRegulation_Temoti(wk->pRegulation, wk->pGameData, &fail_bit )){
    ///�o�g���{�b�N�X�̂� �Ȃ̂ł�����
    POKEPARTY *bb_party = _BBox_PokePartyAlloc(wk->pGameData);
    PokeParty_Copy(bb_party, wk->pParentWork->pPokeParty[id]);
    GFL_HEAP_FreeMemory(bb_party);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4);
  }
  else if(POKE_REG_OK!=_CheckRegulation_BBox(wk->pRegulation, wk->pGameData, &fail_bit )){
    //�Ă����݂̂Ȃ̂ł�����
    PokeParty_Copy(GAMEDATA_GetMyPokemon(wk->pGameData), wk->pParentWork->pPokeParty[id]);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4);
  }
  else{
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1009, FALSE);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START2);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �����J�n WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleStart2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }
  _battlePokePartySelectMenu( wk );
  
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START3);
  return seq;
}
//------------------------------------------------------------------
/**
 * @brief   �����J�n WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START3
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleStart3( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret = BMPMENULIST_NULL;
  int command,id=0;
  u32 fail_bit;
  
  PRINT_UTIL_Trans( &wk->SysMsgPrintUtil, wk->SysMsgQue );
  ret = BmpMenuList_Main(wk->sublw);

  if(!GFL_NET_IsParentMachine()){
    id = 1;
  }

  switch(ret){
  case BMPMENULIST_NULL:
    return seq;
  case BMPMENULIST_CANCEL:
    GF_ASSERT(0);
    break;
  case _TEMOTI:
    PokeParty_Copy(GAMEDATA_GetMyPokemon(wk->pGameData), wk->pParentWork->pPokeParty[id]);
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4);
    break;
  case _BATTLEBOX:
    {
      POKEPARTY *bb_party = _BBox_PokePartyAlloc(wk->pGameData);
      PokeParty_Copy(bb_party, wk->pParentWork->pPokeParty[id]);
      GFL_HEAP_FreeMemory(bb_party);
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4);
    }
    break;
  }
  EndMessageWindowOff(wk);
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  BmpMenuList_Exit(wk->sublw, NULL, &wk->singleCur[_MENUTYPE_POKEPARTY]);
  BmpMenuWork_ListDelete( wk->submenulist );
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�p�[�e�B�[�]�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleStart4( WIFIP2PMATCH_WORK *wk, int seq )
{
  int id=0;
  if(!GFL_NET_IsParentMachine()){
    id = 1;
  }

  if(GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(),GFL_NET_SENDID_ALLUSER,
                        CNM_WFP2PMF_POPEPARTY, PokeParty_GetWorkSize(),
                        wk->pParentWork->pPokeParty[id], FALSE, FALSE, TRUE)){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_POKEPARTY_END,WB_NET_WIFICLUB);
      _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START5);
  }
  return seq;
}
//------------------------------------------------------------------
/**
 * @brief   �|�P�p�[�e�B�[�]��  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START5
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------


static int _playerDirectBattleStart5( WIFIP2PMATCH_WORK *wk, int seq )
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(), _TIMING_POKEPARTY_END,WB_NET_WIFICLUB )){
    _CHANGESTATE(wk, WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START6);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�p�[�e�B�[�]��  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START6
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectBattleStart6( WIFIP2PMATCH_WORK *wk, int seq )
{
  u32 status,gamemode;
  
  WIFI_STATUS_ResetVChatMac(wk->pMatch);
  status = WIFI_STATUS_PLAYING;

  NET_PRINT("regch %d %d %d\n",wk->battleMode,wk->battleRule,wk->battleShooter);

  {
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
    _myStatusChange(wk, status, gamemode);  // �ڑ����ɂȂ�
    wk->endSeq = gamemode;
  }
  {
    EndMessageWindowOff(wk);
  }
  return SEQ_OUT;            //�I���V�[�P���X��
}








//------------------------------------------------------------------
/**
 * @brief   �q�@���V�Ԃ̂��ǂ����ӎv�I��  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectFailed( WIFIP2PMATCH_WORK *wk, int seq )
{
  int gmmno,gmmidx;
  u16 friendNo,status,gamemode;
  WIFI_STATUS* p_status;
  MCR_MOVEOBJ* p_player;
  MCR_MOVEOBJ* p_npc;
  u32 way;

  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }

  if(!GFL_NET_IsParentMachine()){
    _friendNameExpand(wk,  wk->friendNo - 1);
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1011, FALSE);
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED2);
  }
  else{
    _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  }
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �q�@���V�Ԃ̂��ǂ����ӎv�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED2
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectFailed2( WIFIP2PMATCH_WORK *wk, int seq )
{
  if( !WifiP2PMatchMessageEndCheck(wk) ){
    return seq;
  }

  _yenowinCreateM2(wk);

  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED3);
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   �q�@���V�Ԃ̂��ǂ����ӎv�I�� WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED3
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectFailed3( WIFIP2PMATCH_WORK *wk, int seq )
{
  int ret;
  u8 command;
  GFL_FONTSYS_SetDefaultColor();
  ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);

  if(ret == BMPMENU_NULL){  // �܂��I��
    return seq;
  }
  else if(ret == 0){ // �͂���I�������ꍇ
    command = WIFIP2PMATCH_PLAYERDIRECT_INIT5;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                     1, &command);
  }
  else{  // ��������I�������ꍇ
    command = WIFIP2PMATCH_PLAYERDIRECT_END;
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CNM_WFP2PMF_DIRECT_COMMAND,
                     1, &command);
  }
  EndMessageWindowOff(wk);
  _CHANGESTATE(wk,WIFIP2PMATCH_PLAYERDIRECT_WAIT);
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   �w�胂�[�h�I�� WIFIP2PMATCH_PLAYERDIRECT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectWait( WIFIP2PMATCH_WORK *wk, int seq )
{
  return seq;
}



//------------------------------------------------------------------
/**
 * @brief   �w�胂�[�h�I�� WIFIP2PMATCH_PLAYERDIRECT_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static int _playerDirectEnd( WIFIP2PMATCH_WORK *wk, int seq )
{
  _myStatusChange(wk, WIFI_STATUS_WAIT,WIFI_GAME_LOGIN_WAIT);
  GFL_NET_StateWifiMatchEnd(TRUE);

  if(GFL_NET_IsParentMachine()){
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1017, FALSE);
  }
  else{
    WifiP2PMatchMessagePrint(wk, msg_wifilobby_1016, FALSE);
  }
  _CHANGESTATE(wk,WIFIP2PMATCH_MODE_DISCONNECT);
  return seq;
}



