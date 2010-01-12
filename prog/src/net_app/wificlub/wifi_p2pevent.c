//============================================================================================
/**
 * @file  wifi_p2pevent.c
 * @bfief WIFIP2P�C�x���g����
 * @author  k.ohno
 * @date  06.04.14
 */
//============================================================================================

#include <gflib.h>
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
FS_EXTERN_OVERLAY(pokemon_trade);
#define _LOCALMATCHNO (100)
//----------------------------------------------------------------
// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------


typedef struct{
  COMM_TVT_INIT_WORK aTVT;
  WIFIP2PMATCH_PROC_PARAM* pMatchParam;
  POKEMONTRADE_PARAM aPokeTr;
  GAMEDATA* pGameData;
  WIFI_LIST* pWifiList;
  GAMESYS_WORK * gsys;
  WIFILOGIN_PARAM     login;
  BATTLE_SETUP_PARAM para;
  int seq;
  u16* ret;
  //u8 lvLimit;
  u8 bSingle;
  void* work;   // wifiPofin�p���[�N
  u32 vchat;
}EV_P2PEVENT_WORK;

enum{
  P2P_INIT,
  P2P_LOGIN,
  P2P_MATCH_BOARD,
  P2P_SELECT,
  P2P_BATTLE,
  P2P_TIMING_SYNC_CALL_BATTLE,
  P2P_BATTLE_START,
  P2P_BATTLE_END,
  P2P_TRADE,
  P2P_TRADE_END,
  P2P_TVT,
  P2P_TVT_END,
  P2P_EXIT,
  P2P_FREE,
  P2P_SETEND,

  P2P_NOP
};


// WiFiP2PMatrch �S�l�ڑ���ʐl��������`
//static const u8 sc_P2P_FOUR_MATCH_MAX[ WFP2PMF_TYPE_NUM ] = {
//  3, 4, 4, 4
//};


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


typedef struct {
//  u8 lvLimit;
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


static void _battleSetting(EVENT_WIFICLUB_WORK* pClub,int gamemode)
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(pClub->gsys);
  
  switch(gamemode){
  case WIFI_GAME_BATTLE_SINGLE_ALL:
  case WIFI_GAME_BATTLE_SINGLE_FLAT:
    BTL_SETUP_Single_Comm(
      &pClub->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  case WIFI_GAME_BATTLE_DOUBLE_ALL:
  case WIFI_GAME_BATTLE_DOUBLE_FLAT:
    BTL_SETUP_Double_Comm( &pClub->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  case WIFI_GAME_BATTLE_TRIPLE_ALL:
  case WIFI_GAME_BATTLE_TRIPLE_FLAT:
    BTL_SETUP_Triple_Comm( &pClub->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  case WIFI_GAME_BATTLE_ROTATION_ALL:
  case WIFI_GAME_BATTLE_ROTATION_FLAT:
    BTL_SETUP_Rotation_Comm( &pClub->para , gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
    break;
  }
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
    }
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &ep2p->login);
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
    WIFI_STATUS_SetMyMac(ep2p->pMatchParam->pMatch);
    GFL_PROC_SysCallProc(FS_OVERLAY_ID(wifi2dmap), &WifiP2PMatchProcData, ep2p->pMatchParam);
    ep2p->seq ++;
    break;
  case P2P_SELECT:
    if(GFL_NET_IsInit()){
      if( mydwc_checkMyGSID() ){  // �R�[�h�擾�ɐ���
        // �����ƒu���Ȃ���
        //            SysFlag_WifiUseSet(SaveData_GetEventWork(fsys->savedata));
      }
    }
    ep2p->seq = aNextMatchKindTbl[ep2p->pMatchParam->seq].kind;
    NET_PRINT("P2P_SELECT %d %d\n",ep2p->seq,ep2p->pMatchParam->seq);
    //ep2p->lvLimit = aNextMatchKindTbl[ep2p->pMatchParam->seq].lvLimit;
    ep2p->bSingle = aNextMatchKindTbl[ep2p->pMatchParam->seq].bSingle;
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(wificlub));
/*
    switch(ep2p->pMatchParam->seq){
    case WIFI_P2PMATCH_DPW_END:  //DPW�ւ����ꍇ
      *(ep2p->ret) = 1;
      ep2p->seq = P2P_SETEND;
      break;
    }
   */
    break;
  case P2P_BATTLE:
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO);
      ep2p->seq++;
    }
    break;
  case P2P_TIMING_SYNC_CALL_BATTLE:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO)){
      ep2p->seq++;
    }
    break;
  case P2P_BATTLE_START:

    _battleSetting(pClub, ep2p->pMatchParam->seq);
//    pClub->para.rule = BTL_RULE_SINGLE;
//    pClub->para.netHandle = GFL_NET_HANDLE_GetCurrentHandle();
//    pClub->para.commPos = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
    PMSND_PlayBGM(pClub->para.musicDefault);

    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
    GAMESYSTEM_CallProc(ep2p->gsys, NO_OVERLAY_ID, &BtlProcData, &pClub->para);
    //        GFL_PROC_SysCallProc(NO_OVERLAY_ID, GMEVENT_Sub_BattleProc, battle_param);
    ep2p->seq++;
    break;
  case P2P_BATTLE_END:
    ep2p->seq = P2P_MATCH_BOARD;
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    break;
  case P2P_TRADE:
    ep2p->aPokeTr.gamedata = GAMESYSTEM_GetGameData(pClub->gsys);
    ep2p->aPokeTr.type = POKEMONTRADE_WIFICLUB;
    GFL_PROC_SysCallProc(FS_OVERLAY_ID(pokemon_trade), &PokemonTradeProcData, &ep2p->aPokeTr);
    ep2p->seq++;
    break;
  case P2P_TRADE_END:
    ep2p->seq = P2P_MATCH_BOARD;
    break;
/*
�����l�ł��A�L��ł��B

�ʐMTVT�̌Ăяo���ł��B
�w�b�_�t�@�C�� include/net_app/comm_tvt_sys.h
extern GFL_PROC_DATA COMM_TVT_ProcData;

COMM_TVT_INIT_WORK �� mode �� CTM_WIFI ��n���ċN�����Ă��������B
��낵�����肢�������܂��B
*/


  case P2P_TVT:
    ep2p->aTVT.gameData = GAMESYSTEM_GetGameData(ep2p->gsys);
    ep2p->aTVT.mode = CTM_WIFI;
    GFL_PROC_SysCallProc(FS_OVERLAY_ID(comm_tvt), &COMM_TVT_ProcData, &ep2p->aTVT);
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

static GFL_PROC_RESULT WifiClubProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_WIFICLUB_WORK* pClub = pwk;
  EV_P2PEVENT_WORK* ep2p = GFL_PROC_AllocWork(proc, sizeof(EV_P2PEVENT_WORK),GetHeapLowID(HEAPID_PROC));

  GFL_STD_MemClear(ep2p, sizeof(EV_P2PEVENT_WORK));
  ep2p->pMatchParam = GFL_HEAP_AllocClearMemory(GetHeapLowID(HEAPID_PROC), sizeof(WIFIP2PMATCH_PROC_PARAM));
  ep2p->pMatchParam->pMatch = GFL_HEAP_AllocClearMemory(GetHeapLowID(HEAPID_PROC), sizeof( WIFI_STATUS ));
  ep2p->pMatchParam->pSaveData = pClub->ctrl;
  ep2p->pMatchParam->pGameData = GAMESYSTEM_GetGameData(pClub->gsys);
//  NET_PRINT("%x %x\n",(int)ep2p->pMatchParam->pMatch,(int)pClub->ctrl);

  ep2p->pGameData =  GAMESYSTEM_GetGameData(pClub->gsys);
  ep2p->pWifiList = GAMEDATA_GetWiFiList(ep2p->pGameData);
  ep2p->pMatchParam->seq = P2P_INIT;
  ep2p->gsys = pClub->gsys;

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




