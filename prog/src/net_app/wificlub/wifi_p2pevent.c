//============================================================================================
/**
 * @file  wifi_p2pevent.c
 * @bfief WIFIP2P�C�x���g����
 * @author  k.ohno
 * @date  06.04.14
 */
//============================================================================================

#include <gflib.h>
#include "pm_define.h"
#include "net/network_define.h"

#include "gamesystem/game_event.h"
#include "gamesystem/btl_setup.h"

#include "net_app/wificlub/wifi_p2pmatch.h"
//#include "net_app/wificlub/wifi_p2pmatchfour.h"
#include "net_app/pokemontrade.h"
#include "wifi_p2pmatch_local.h"

#include "net/dwc_rap.h"
#include "net/dwc_raputil.h"
#include "net/dwc_rapcommon.h"
#include "savedata/wifilist.h"
#include "system/main.h"
#include "battle/battle.h"  //BTL_RULE_SINGLE
#include "sound/pm_sndsys.h"

#include "field/event_wificlub.h"

#include "net_app/comm_tvt_sys.h"  //TV�g�����V�[�o
#include "wifi_status.h"
#include "net_app/wifi_login.h"

#include "app/pokelist.h"
#include "include/demo/comm_btl_demo.h"
#include "field/event_battle_call.h"

//���X�g�̃X�e�[�^�X�̃v���b�N
#include "wifi_p2p_subproc.h"

//#include "net_app/balloon.h"

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GFL_PROC_RESULT WifiClubProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT WifiClubProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT WifiClubProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

//==============================================================================
//  �f�[�^
//==============================================================================
const GFL_PROC_DATA WifiClubProcData = {
  WifiClubProcInit,
  WifiClubProcMain,
  WifiClubProcEnd,
};


FS_EXTERN_OVERLAY(wifi2dmap);
FS_EXTERN_OVERLAY(battle);
FS_EXTERN_OVERLAY(wificlub);
FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(pokemon_trade);
FS_EXTERN_OVERLAY(wificlub_subproc);
#define _LOCALMATCHNO (100)
//----------------------------------------------------------------
// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------


typedef struct{
  //PLIST_DATA PokeList;
  WIFICLUB_BATTLE_SUBPROC_PARAM subProcParam;
  COMM_TVT_INIT_WORK aTVT;
  WIFIP2PMATCH_PROC_PARAM* pMatchParam;
  POKEMONTRADE_PARAM aPokeTr;
  GAMEDATA* pGameData;
  WIFI_LIST* pWifiList;
  GAMESYS_WORK * gsys;
  WIFILOGIN_PARAM     login;
  BATTLE_SETUP_PARAM para;
  COMM_BTL_DEMO_PARAM demo_prm;
  COMM_BATTLE_CALL_PROC_PARAM prm;
  POKEPARTY* pPokeParty;   //�o�g���ɓn��PokeParty
  int seq;
  u16* ret;
  u8 bSingle;
  u8 dummy;
}EV_P2PEVENT_WORK;

enum{
  P2P_INIT,
  P2P_LOGIN,
  P2P_MATCH_BOARD,
  P2P_SELECT,
  P2P_BATTLE,
  P2P_BATTLE2,
  P2P_TIMING_SYNC_CALL_BATTLE,
  P2P_BATTLE_START,
  P2P_BATTLE_END,
  P2P_TRADE,
  P2P_TRADE_MAIN,
  P2P_TRADE_END,
  P2P_EVOLUTION,
  P2P_EVOLUTION_END,
  P2P_TVT,
  P2P_TVT_END,
  P2P_EXIT,
  P2P_FREE,
  P2P_SETEND,

  P2P_NOP
};



#define _BLOCK (0)

//-------------------------------------
#if _BLOCK
/// �o�g�����[���̎����̂��߃v���Z�X�ύX�֐����쐬
static void P2P_FourWaitInit( EV_P2PEVENT_WORK* p_wk, GMEVENT* fsys, u32 heapID, u32 type );
static u32 P2P_FourWaitEnd( EV_P2PEVENT_WORK* p_wk );
#else
static void P2P_FourWaitInit( EV_P2PEVENT_WORK* p_wk, GMEVENT* fsys, u32 heapID, u32 type ){}
static u32 P2P_FourWaitEnd( EV_P2PEVENT_WORK* p_wk ){ return P2P_MATCH_BOARD; }
#endif

static void _pokmeonListWorkFree(EV_P2PEVENT_WORK* ep2p);

typedef struct {
  u8 bSingle;
  u16 kind;
} NextMatchKindTbl;

NextMatchKindTbl aNextMatchKindTbl[] = {
  { 0, P2P_EXIT},
  { 0, P2P_EXIT},
  { 0, P2P_EXIT},
  { 0, P2P_EXIT},
  { 0, P2P_TVT}, //WIFI_P2PMATCH_TRADE:   // TV�g�����V�[�o
  { 0, P2P_TRADE}, //WIFI_P2PMATCH_TRADE:   // �|�P�����g���[�h�Ăяo��
  { WIFI_GAME_BATTLE_SINGLE_ALL, P2P_BATTLE},   //�o�g��
  { WIFI_GAME_BATTLE_SINGLE_FLAT, P2P_BATTLE}, 
  { WIFI_GAME_BATTLE_DOUBLE_ALL, P2P_BATTLE}, 
  { WIFI_GAME_BATTLE_DOUBLE_FLAT, P2P_BATTLE}, 
  { WIFI_GAME_BATTLE_TRIPLE_ALL, P2P_BATTLE}, 
  { WIFI_GAME_BATTLE_TRIPLE_FLAT, P2P_BATTLE}, 
  { WIFI_GAME_BATTLE_ROTATION_ALL, P2P_BATTLE}, 
  { WIFI_GAME_BATTLE_ROTATION_FLAT, P2P_BATTLE}, 
  { 0, P2P_EXIT},   // �ʐM�ؒf���Ă܂��B�I�����܂��B
};


static void _battleSetting(EVENT_WIFICLUB_WORK* pClub,EV_P2PEVENT_WORK * ep2p,int gamemode)
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(pClub->gsys);
  
  switch(gamemode){
  case WIFI_GAME_BATTLE_SINGLE_ALL:
  case WIFI_GAME_BATTLE_SINGLE_FLAT:
    BTL_SETUP_Single_Comm(
      &ep2p->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  case WIFI_GAME_BATTLE_DOUBLE_ALL:
  case WIFI_GAME_BATTLE_DOUBLE_FLAT:
    BTL_SETUP_Double_Comm( &ep2p->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  case WIFI_GAME_BATTLE_TRIPLE_ALL:
  case WIFI_GAME_BATTLE_TRIPLE_FLAT:
    BTL_SETUP_Triple_Comm( &ep2p->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  case WIFI_GAME_BATTLE_ROTATION_ALL:
  case WIFI_GAME_BATTLE_ROTATION_FLAT:
    BTL_SETUP_Rotation_Comm( &ep2p->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  }
  BATTLE_PARAM_SetPokeParty( &ep2p->para, ep2p->pPokeParty, BTL_CLIENT_PLAYER );

}

//==============================================================================
//  WIFI�N���u����|�P�������X�g�p���[�N�쐬
//==============================================================================

static void _pokeListWorkMake(EV_P2PEVENT_WORK * ep2p,GAMEDATA* pGameData,u32 gamemode)
{
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  MYSTATUS* pTarget = GAMEDATA_GetMyStatusPlayer(pGameData,1-my_net_id);
  WIFICLUB_BATTLE_SUBPROC_PARAM *initWork = &ep2p->subProcParam;
  
  initWork->regulation = ep2p->pMatchParam->pRegulation;
  initWork->selfPokeParty = ep2p->pMatchParam->pPokeParty[my_net_id];
  initWork->enemyName = MyStatus_GetMyName(pTarget);
  initWork->enemySex = MyStatus_GetMySex(pTarget);
  initWork->enemyPokeParty = ep2p->pMatchParam->pPokeParty[1-my_net_id];
  initWork->gameData = pGameData;
  initWork->comm_selected_num = 0;
  
  //�ȉ�[out]
  PokeParty_InitWork(ep2p->pPokeParty);
  initWork->p_party = ep2p->pPokeParty;

/*
  PLIST_DATA* plist = &ep2p->PokeList;
  
  plist->reg = ep2p->pMatchParam->pRegulation;
  plist->pp = ep2p->pMatchParam->pPokeParty[my_net_id];
  plist->mode = PL_MODE_BATTLE_WIFI;
  plist->myitem = GAMEDATA_GetMyItem(pGameData);
  plist->cfg = SaveData_GetConfig(GAMEDATA_GetSaveControlWork(pGameData));
  plist->is_disp_party = TRUE;
  plist->use_tile_limit = FALSE;
  plist->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].pp = ep2p->pMatchParam->pPokeParty[1-my_net_id];
  plist->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].name = MyStatus_GetMyName(pTarget);
  plist->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].sex = MyStatus_GetMySex(pTarget);
  plist->comm_type = PL_COMM_SINGLE;

  switch(gamemode){
  case WIFI_GAME_BATTLE_SINGLE_ALL:
  case WIFI_GAME_BATTLE_SINGLE_FLAT:
    plist->type = PL_TYPE_SINGLE;
    break;
  case WIFI_GAME_BATTLE_DOUBLE_ALL:
  case WIFI_GAME_BATTLE_DOUBLE_FLAT:
    plist->type = PL_TYPE_DOUBLE;
    break;
  case WIFI_GAME_BATTLE_TRIPLE_ALL:
  case WIFI_GAME_BATTLE_TRIPLE_FLAT:
    plist->type = PL_TYPE_TRIPLE;
    break;
  case WIFI_GAME_BATTLE_ROTATION_ALL:
  case WIFI_GAME_BATTLE_ROTATION_FLAT:
    plist->type = PL_TYPE_ROTATION;
    break;
  }
*/
}

//==============================================================================
//  �|�P�������X�g����o�g���p�|�P�p�[�e�B�[�쐬
//==============================================================================
static void _pokeListWorkOut(EV_P2PEVENT_WORK * ep2p,GAMEDATA* pGameData,u32 gamemode)
{
/*
  int entry_no;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  MYSTATUS* pTarget = GAMEDATA_GetMyStatusPlayer(pGameData,1-my_net_id);
  PLIST_DATA* plist = &ep2p->PokeList;

  PokeParty_InitWork(ep2p->pPokeParty);

  for(entry_no = 0; entry_no < TEMOTI_POKEMAX; entry_no++){
    if(plist->in_num[entry_no] == 0){
      break;
    }
    PokeParty_Add(ep2p->pPokeParty, 
                  PokeParty_GetMemberPointer(plist->pp, plist->in_num[entry_no] - 1));
  }
*/
}



//==============================================================================
//  WIFI�ʐM�����
//==============================================================================
static GFL_PROC_RESULT WifiClubProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  int len;
  EVENT_WIFICLUB_WORK* pClub = (void*)pwk;
  EV_P2PEVENT_WORK * ep2p = pClub->pWork;
  GAMESYS_WORK * gsys = pClub->gsys;

  switch (ep2p->seq) {
  case P2P_INIT:
    ep2p->seq = P2P_LOGIN;
    { //WIFI�����̐ݒ�
      GFL_STD_MemClear( &ep2p->login, sizeof(WIFILOGIN_PARAM) );
      ep2p->login.gamedata = GAMESYSTEM_GetGameData(pClub->gsys);
      ep2p->login.display = WIFILOGIN_DISPLAY_UP;
      ep2p->login.bg = WIFILOGIN_BG_NORMAL;
      ep2p->login.nsid = WB_NET_WIFICLUB;
    }
    GMEVENT_CallProc(pClub->event, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &ep2p->login);
//    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &ep2p->login);
   // GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &ep2p->login);
    break;
  case P2P_LOGIN:
    if( ep2p->login.result == WIFILOGIN_RESULT_LOGIN ){
        ep2p->seq = P2P_MATCH_BOARD;
      }
      else{ 
        ep2p->seq = P2P_EXIT;
      }
    break;
  case P2P_MATCH_BOARD:
    GFL_OVERLAY_Load(FS_OVERLAY_ID(wificlub));
    GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

    WIFI_STATUS_SetMyMac(ep2p->pMatchParam->pMatch);

    ep2p->pMatchParam->pPokeParty[0] = PokeParty_AllocPartyWork(GFL_HEAPID_APP);   //���݂���Party����M
    ep2p->pMatchParam->pPokeParty[1] = PokeParty_AllocPartyWork(GFL_HEAPID_APP);   //���݂���Party����M
    ep2p->pMatchParam->pRegulation = Regulation_AllocWork(GFL_HEAPID_APP);

    GMEVENT_CallProc(pClub->event,FS_OVERLAY_ID(wifi2dmap), &WifiP2PMatchProcData, ep2p->pMatchParam);
//    GFL_PROC_SysCallProc(FS_OVERLAY_ID(wifi2dmap), &WifiP2PMatchProcData, ep2p->pMatchParam);
    ep2p->seq ++;
    break;
  case P2P_SELECT:
    ep2p->seq = aNextMatchKindTbl[ep2p->pMatchParam->seq].kind;
    NET_PRINT("P2P_SELECT %d %d\n",ep2p->seq,ep2p->pMatchParam->seq);
    if(P2P_BATTLE != ep2p->seq){  // �o�g���ȊO�͂���Ȃ�
      _pokmeonListWorkFree(ep2p);      // �|�P�������X�g���I�������v��Ȃ�
    }
    ep2p->bSingle = aNextMatchKindTbl[ep2p->pMatchParam->seq].bSingle;
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(ui_common));
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(wificlub));
    break;
  case P2P_BATTLE:
    {
      _pokeListWorkMake(ep2p,GAMESYSTEM_GetGameData(pClub->gsys),ep2p->seq );
      //GFL_PROC_SysCallProc(FS_OVERLAY_ID(pokelist), &PokeList_ProcData, &ep2p->PokeList);
      //GMEVENT_CallProc(pClub->event,FS_OVERLAY_ID(pokelist), &PokeList_ProcData, &ep2p->PokeList);
      GMEVENT_CallProc(pClub->event,FS_OVERLAY_ID(wificlub_subproc), &WifiClubBattle_Sub_ProcData, &ep2p->subProcParam);
      ep2p->seq++;
    }
    break;
  case P2P_BATTLE2:
    {
      _pokeListWorkOut(ep2p,GAMESYSTEM_GetGameData(pClub->gsys),ep2p->seq );
      
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO,WB_NET_WIFICLUB);
      ep2p->seq++;
    }
    break;
  case P2P_TIMING_SYNC_CALL_BATTLE:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO, WB_NET_WIFICLUB)){
      ep2p->seq++;
    }
    break;
  case P2P_BATTLE_START:

    _battleSetting(pClub, ep2p ,ep2p->pMatchParam->seq);
    PMSND_PlayBGM(ep2p->para.musicDefault);

    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
#if 0
    GAMESYSTEM_CallProc(ep2p->gsys, NO_OVERLAY_ID, &BtlProcData, &ep2p->para);
#else
    {
      int i;

      ep2p->demo_prm.type = COMM_BTL_DEMO_TYPE_NORMAL_START;

      for( i=0;i<2;i++){
        ep2p->demo_prm.trainer_data[i].mystatus = GAMEDATA_GetMyStatusPlayer( GAMESYSTEM_GetGameData( gsys ),i );
        ep2p->demo_prm.trainer_data[i].party = ep2p->pMatchParam->pPokeParty[i];
      }
      {
        ep2p->prm.gdata = GAMESYSTEM_GetGameData(pClub->gsys);
        ep2p->prm.btl_setup_prm = &ep2p->para;
        ep2p->prm.demo_prm = &ep2p->demo_prm;
        GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
        GMEVENT_CallProc(pClub->event, NO_OVERLAY_ID, &CommBattleCommProcData, &ep2p->prm);
      }
    }
#endif
    //        GFL_PROC_SysCallProc(NO_OVERLAY_ID, GMEVENT_Sub_BattleProc, battle_param);
    ep2p->seq++;
    break;
  case P2P_BATTLE_END:

    
    _pokmeonListWorkFree(ep2p);      // 

    ep2p->seq = P2P_MATCH_BOARD;
//    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    break;
  case P2P_TRADE:
    ep2p->aPokeTr.ret = POKEMONTRADE_MOVE_START;
    ep2p->seq++;
    break;
  case P2P_TRADE_MAIN:
    ep2p->aPokeTr.gamedata = GAMESYSTEM_GetGameData(pClub->gsys);

    GMEVENT_CallProc(pClub->event,FS_OVERLAY_ID(pokemon_trade), &PokemonTradeClubProcData, &ep2p->aPokeTr);
   // GFL_PROC_SysCallProc(FS_OVERLAY_ID(pokemon_trade), &PokemonTradeClubProcData, &ep2p->aPokeTr);
    ep2p->seq++;
    break;
  case P2P_TRADE_END:
    if(ep2p->aPokeTr.ret == POKEMONTRADE_MOVE_EVOLUTION){
      ep2p->seq = P2P_EVOLUTION;
    }
    else{
      ep2p->seq = P2P_MATCH_BOARD;
    }
    break;
  case P2P_EVOLUTION:
    GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
    ep2p->aPokeTr.shinka_param = SHINKADEMO_AllocParam( HEAPID_PROC, GAMESYSTEM_GetGameData(pClub->gsys),
                                               ep2p->aPokeTr.pParty,
                                               ep2p->aPokeTr.after_mons_no,
                                               0, ep2p->aPokeTr.cond, TRUE );
    GMEVENT_CallProc(pClub->event, NO_OVERLAY_ID, &ShinkaDemoProcData, ep2p->aPokeTr.shinka_param );
//    GFL_PROC_SysCallProc( NO_OVERLAY_ID, &ShinkaDemoProcData, ep2p->aPokeTr.shinka_param );
    ep2p->seq = P2P_EVOLUTION_END;
    break;
  case P2P_EVOLUTION_END:
    SHINKADEMO_FreeParam( ep2p->aPokeTr.shinka_param );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
    ep2p->aPokeTr.ret = POKEMONTRADE_MOVE_EVOLUTION;
    ep2p->seq = P2P_TRADE_MAIN;
    break;
  case P2P_TVT:
    ep2p->aTVT.gameData = GAMESYSTEM_GetGameData(ep2p->gsys);
    ep2p->aTVT.mode = CTM_WIFI;
    GMEVENT_CallProc(pClub->event, FS_OVERLAY_ID(comm_tvt), &COMM_TVT_ProcData, &ep2p->aTVT);
 //   GFL_PROC_SysCallProc(FS_OVERLAY_ID(comm_tvt), &COMM_TVT_ProcData, &ep2p->aTVT);
    ep2p->seq++;
    break;
  case P2P_TVT_END:
    ep2p->seq = P2P_MATCH_BOARD;
    break;

  case P2P_EXIT:
  case P2P_SETEND:
  case P2P_FREE:
    return TRUE;
  default:
    return TRUE;
  }
  return FALSE;
}


static void _pokmeonListWorkFree(EV_P2PEVENT_WORK* ep2p)
{
  GFL_HEAP_FreeMemory(ep2p->pMatchParam->pPokeParty[0]);
  GFL_HEAP_FreeMemory(ep2p->pMatchParam->pPokeParty[1]);
  GFL_HEAP_FreeMemory(ep2p->pMatchParam->pRegulation);
}


static GFL_PROC_RESULT WifiClubProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_WIFICLUB_WORK* pClub = pwk;
  EV_P2PEVENT_WORK* ep2p = GFL_PROC_AllocWork(proc, sizeof(EV_P2PEVENT_WORK),GetHeapLowID(HEAPID_PROC));

  GFL_STD_MemClear(ep2p, sizeof(EV_P2PEVENT_WORK));
  ep2p->pMatchParam = GFL_HEAP_AllocClearMemory(GetHeapLowID(HEAPID_PROC), sizeof(WIFIP2PMATCH_PROC_PARAM));
  ep2p->pMatchParam->pMatch = GFL_HEAP_AllocClearMemory(GetHeapLowID(HEAPID_PROC), sizeof( WIFI_STATUS ));
  ep2p->pMatchParam->pGameData = GAMESYSTEM_GetGameData(pClub->gsys);
  ep2p->pMatchParam->pSaveData = GAMEDATA_GetSaveControlWork(ep2p->pMatchParam->pGameData);
  ep2p->pMatchParam->vchatMain = TRUE;

  ep2p->pGameData =  ep2p->pMatchParam->pGameData;
  ep2p->pWifiList = GAMEDATA_GetWiFiList(ep2p->pGameData);
  ep2p->pMatchParam->seq = WIFI_GAME_NONE;
  ep2p->gsys = pClub->gsys;
  ep2p->pPokeParty = PokeParty_AllocPartyWork(GetHeapLowID(HEAPID_PROC));
  ep2p->aPokeTr.pParty = ep2p->pPokeParty; // ���������g��

  pClub->pWork = ep2p;

  // �T�E���h�e�X�g
  // �a�f�l�ꎞ��~���ޔ�
  PMSND_PauseBGM(TRUE);
  PMSND_PushBGM();


  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT WifiClubProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_WIFICLUB_WORK* pClub = pwk;
  EV_P2PEVENT_WORK* ep2p = pClub->pWork;

  GFL_HEAP_FreeMemory(ep2p->pPokeParty);
  GFL_HEAP_FreeMemory(ep2p->pMatchParam->pMatch);
  GFL_HEAP_FreeMemory(ep2p->pMatchParam);
  GFL_PROC_FreeWork(proc);

  // �T�E���h�e�X�g
  // �a�f�l���o�����ĊJ
  PMSND_PopBGM();
  PMSND_PauseBGM(FALSE);
  PMSND_FadeInBGM(60);


  return GFL_PROC_RES_FINISH;
}




