//======================================================================
/**
 * @file	irc_battle_subproc.h
 * @brief	赤外線バトル リスト・ステータス呼び出し
          ->WIFI_MATCHへもってきてPROC化
          ->WIFI_CLUBへもってきた
 * @author	ariizumi -> nagihashi -> ariizumi
 * @data	10/02/26
 *
 */
//======================================================================
#include <gflib.h>

#include "app/pokelist.h"
#include "app/p_status.h"
#include "app/pokelist/plist_comm.h"
#include "savedata/mystatus.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/net_err.h"
#include "net/dwc_error.h"
#include "net/network_define.h"

#include "wifi_p2p_subproc.h"
#include "poke_tool/poke_regulation.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define


//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  IBSS_INIT_POKELIST,
  IBSS_MAIN_POKELIST,
  IBSS_EXIT_POKELIST,
  IBSS_INIT_POKESTATUS,
  IBSS_MAIN_POKESTATUS,
  IBSS_EXIT_POKESTATUS,
  _POKEMONPARTY_SET,
  _POKEMONPARTY_SEND,
  _POKEMONPARTY_SEND2,
}WIFICLUB_BATTLE_SUBPROC_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  WIFICLUB_BATTLE_SUBPROC_STATE state;
  
  //バトルボックスの対応も考え一応pokepartyはコメント
  //POKEPARTY *pokeParty;
  
  PLIST_DATA plData;
  PSTATUS_DATA psData;
  
  u8 timeCnt;
  u32 befVCount;
  
  GFL_PROCSYS *procSys;

  WIFICLUB_BATTLE_SUBPROC_PARAM  param;
  HEAPID  heapId;
}WIFICLUB_BATTLE_SUBPROC_WORK;



//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void WIFICLUB_BATTLE_SUBPROC_InitListData( WIFICLUB_BATTLE_SUBPROC_PARAM *p_param, WIFICLUB_BATTLE_SUBPROC_WORK *procWork , PLIST_DATA *plData );
static void WIFICLUB_BATTLE_SUBPROC_InitStatusData( WIFICLUB_BATTLE_SUBPROC_PARAM *p_param, WIFICLUB_BATTLE_SUBPROC_WORK *procWork , PSTATUS_DATA *psData );

static GFL_PROC_RESULT WIFICLUB_BATTLE_SUB_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFICLUB_BATTLE_SUB_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFICLUB_BATTLE_SUB_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static void _recvPartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static u8* _netPartyGet(int netID, void* pWk, int size);



FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(poke_status);


//======================================================================
//	テーブル
//======================================================================


///通信コマンド
typedef enum {
  _NETCMD_PARTY = GFL_NET_CMD_WIFIP2P_SUB,
} _WIFIP2P_SENDCMD;

typedef enum {
  _TIMING_PARTYSEND =  20,
} _WIFIP2P_TIMING;


///通信コマンドテーブル
static const NetRecvFuncTable _PacketTbl[] = {
  {_recvPartyData,   _netPartyGet},    ///_NETCMD_PARTY
};



const GFL_PROC_DATA WifiClubBattle_Sub_ProcData =
{ 
  WIFICLUB_BATTLE_SUB_PROC_Init,
  WIFICLUB_BATTLE_SUB_PROC_Main,
  WIFICLUB_BATTLE_SUB_PROC_Exit,
};



//------------------------------------------------------------------------------
/**
 * @brief   パーティーの受信バッファを返す
 * @retval  none
 */
//------------------------------------------------------------------------------
static u8* _netPartyGet(int netID, void* pWk, int size)
{
  WIFICLUB_BATTLE_SUBPROC_WORK *pWork = pWk;
  return (u8*)pWork->param.netParty[netID];
}




//------------------------------------------------------------------------------
/**
 * @brief   通信コールバックポケパーティー _NETCMD_PARTY
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _recvPartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  WIFICLUB_BATTLE_SUBPROC_WORK *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;//自分のは今は受け取らない
  }
  //入っている
}


//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
static GFL_PROC_RESULT WIFICLUB_BATTLE_SUB_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  u8 i;
  WIFICLUB_BATTLE_SUBPROC_WORK   *procWork;
  WIFICLUB_BATTLE_SUBPROC_PARAM  *p_param  = p_param_adrs;
  
  //ヒープ作成
	//GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFICLUB_BATTLE_SUB, 0x30000 );

  procWork = GFL_PROC_AllocWork( p_proc, sizeof(WIFICLUB_BATTLE_SUBPROC_WORK), HEAPID_PROC );
  GFL_STD_MemClear( procWork, sizeof(WIFICLUB_BATTLE_SUBPROC_WORK) );

  procWork->param   = *p_param;
  procWork->heapId  = HEAPID_PROC;
  
  procWork->procSys = GFL_PROC_LOCAL_boot( procWork->heapId );

  /*
  procWork->pokeParty = PokeParty_AllocPartyWork( procWork->heapId );
  for( i=0;i<TEMOTI_POKEMAX;i++ )
  {
    if( p_param->ppp[i] )
    { 
      if( PPP_Get( p_param->ppp[i] , ID_PARA_poke_exist , NULL ) )
      {
        POKEMON_PARAM *pp = PP_CreateByPPP( p_param->ppp[i] , procWork->heapId );
        PokeParty_Add( procWork->pokeParty , pp );
        GFL_HEAP_FreeMemory( pp );
      }
    }
  }
  */
  GFL_NET_AddCommandTable(GFL_NET_CMD_WIFIP2P_SUB,_PacketTbl,NELEMS(_PacketTbl), procWork);

  WIFICLUB_BATTLE_SUBPROC_InitListData( p_param, procWork , &procWork->plData );
  WIFICLUB_BATTLE_SUBPROC_InitStatusData( p_param, procWork , &procWork->psData );
  
  procWork->timeCnt = 0;
  procWork->befVCount = OS_GetVBlankCount();
  procWork->state = IBSS_INIT_POKELIST;

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
static GFL_PROC_RESULT WIFICLUB_BATTLE_SUB_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{

  WIFICLUB_BATTLE_SUBPROC_WORK   *procWork = p_wk_adrs;
  WIFICLUB_BATTLE_SUBPROC_PARAM  *p_param  = p_param_adrs;


  //パーティーのセット
  /*
  for( i=0;i<TEMOTI_POKEMAX;i++ )
  {
    const u8 num = procWork->plData.in_num[i];
    if( num > 0 )
    {
      POKEMON_PARAM *pp = PP_CreateByPPP( procWork->param.ppp[num-1] , procWork->heapId );
      PokeParty_Add( p_param->p_party, pp );
      GFL_HEAP_FreeMemory( pp );
    }
  }
  

  GFL_HEAP_FreeMemory( procWork->pokeParty );
  */
  PLIST_COMM_ExitComm( &procWork->plData );
  GFL_OVERLAY_Unload(FS_OVERLAY_ID(pokelist_comm));
  
  GFL_PROC_LOCAL_Exit( procWork->procSys );
	GFL_PROC_FreeWork( p_proc );

	//ヒープ破棄
	//GFL_HEAP_DeleteHeap( HEAPID_WIFICLUB_BATTLE_SUB );

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
static GFL_PROC_RESULT WIFICLUB_BATTLE_SUB_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  WIFICLUB_BATTLE_SUBPROC_WORK   *procWork = p_wk_adrs;
  WIFICLUB_BATTLE_SUBPROC_PARAM  *p_param  = p_param_adrs;

  //制限時間処理
  if( procWork->plData.use_tile_limit == TRUE &&
      procWork->plData.time_limit > 0 )
  {
    const u32 vCount = OS_GetVBlankCount();
    procWork->timeCnt += vCount - procWork->befVCount;
    procWork->befVCount = vCount;
    if( procWork->timeCnt > 60 )
    {
      procWork->plData.time_limit--;
      procWork->timeCnt -= 60;
    }
  }

  //List内で制御するのでコメントアウト Ari100310
  //procWork->plData.comm_selected_num  = p_param->comm_selected_num;
  PLIST_COMM_UpdateComm( &procWork->plData );
  
  
  switch( procWork->state )
  {
  case IBSS_INIT_POKELIST:
    GFL_OVERLAY_Load( FS_OVERLAY_ID(pokelist) );
    GFL_PROC_LOCAL_CallProc( procWork->procSys , NO_OVERLAY_ID , &PokeList_ProcData , &procWork->plData );
    
    procWork->state = IBSS_MAIN_POKELIST;
    break;

  case IBSS_MAIN_POKELIST:
    if( GFL_PROC_LOCAL_Main( procWork->procSys ) == GFL_PROC_MAIN_NULL )
    {
      procWork->state = IBSS_EXIT_POKELIST;
    }
    break;

  case IBSS_EXIT_POKELIST:
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(pokelist) );
    if( procWork->plData.ret_mode == PL_RET_STATUS )
    {
      procWork->psData.pos = procWork->plData.ret_sel;
      procWork->state = IBSS_INIT_POKESTATUS;
    }
    else
    {
      p_param->result = WIFICLUB_BATTLE_SUBPROC_RESULT_SUCCESS;
      procWork->state = _POKEMONPARTY_SET;
    }
    break;
  case _POKEMONPARTY_SET:
    {
      u8 i;
      for( i=0;i<TEMOTI_POKEMAX;i++ )
      {
        const u8 num = procWork->plData.in_num[i];
        if( num > 0 )
        {
          POKEMON_PARAM *pp = PokeParty_GetMemberPointer( p_param->selfPokeParty , num-1 );
          PokeParty_Add( p_param->p_party, pp );
        }
      }
    }
    procWork->state = _POKEMONPARTY_SEND;
    break;
  case _POKEMONPARTY_SEND:
    if(GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(),GFL_NET_SENDID_ALLUSER,
                           _NETCMD_PARTY,PokeParty_GetWorkSize() , p_param->p_party, FALSE,FALSE,TRUE )){
      GFL_NET_HANDLE_TimeSyncStart( GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_PARTYSEND, WB_NET_WIFIP2P_SUB);
      procWork->state = _POKEMONPARTY_SEND2;
    }
    break;
  case _POKEMONPARTY_SEND2:
    if( GFL_NET_HANDLE_IsTimeSync( GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_PARTYSEND, WB_NET_WIFIP2P_SUB)){

      OS_TPrintf("PokeParty_GetBattlePokeNum %d \n" ,PokeParty_GetBattlePokeNum(procWork->param.netParty[0]));
      OS_TPrintf("PokeParty_GetBattlePokeNum %d \n" ,PokeParty_GetBattlePokeNum(procWork->param.netParty[1]));

      return GFL_PROC_RES_FINISH;
    }
    break;
  case IBSS_INIT_POKESTATUS:
    GFL_OVERLAY_Load( FS_OVERLAY_ID(poke_status) );
    GFL_PROC_LOCAL_CallProc( procWork->procSys , NO_OVERLAY_ID , &PokeStatus_ProcData , &procWork->psData );
    
    procWork->state = IBSS_MAIN_POKESTATUS;
    break;

  case IBSS_MAIN_POKESTATUS:
    if( procWork->plData.use_tile_limit == TRUE &&
        procWork->plData.time_limit == 0 )
    {
      procWork->psData.isExitRequest = TRUE;
    }
    if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
    {
      procWork->psData.isExitRequest = TRUE;
    }

    if( GFL_PROC_LOCAL_Main( procWork->procSys ) == GFL_PROC_MAIN_NULL )
    {
      procWork->state = IBSS_EXIT_POKESTATUS;
    }
    break;

  case IBSS_EXIT_POKESTATUS:
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(poke_status) );
    procWork->plData.ret_sel = procWork->psData.pos;
    procWork->state = IBSS_INIT_POKELIST;
    break;
  }

  //エラー処理ここで起きたら復帰が難しいので切断
  if( GFL_NET_IsInit() )
  { 
    if( procWork->state != IBSS_MAIN_POKELIST && 
        procWork->state != IBSS_EXIT_POKELIST &&
        procWork->state != IBSS_MAIN_POKESTATUS && 
        procWork->state != IBSS_EXIT_POKESTATUS )
    {
      if( GFL_NET_DWC_ERROR_ReqErrorDisp(TRUE,TRUE) != GFL_NET_DWC_ERROR_RESULT_NONE )
      { 
        p_param->result = WIFICLUB_BATTLE_SUBPROC_RESULT_ERROR_NEXT_LOGIN;
        return GFL_PROC_RES_FINISH;
      }
    }
  }


  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
//	リスト初期化データ作成
//--------------------------------------------------------------
static void WIFICLUB_BATTLE_SUBPROC_InitListData( WIFICLUB_BATTLE_SUBPROC_PARAM *p_param, WIFICLUB_BATTLE_SUBPROC_WORK *procWork , PLIST_DATA *plData )
{
  u8 i;
  REGULATION *reg = p_param->regulation;
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork(p_param->gameData);
  
  //plData->pp = procWork->pokeParty;
  plData->pp = p_param->selfPokeParty;

  //レギュレーションにそったレベル補正を行います
  PokeRegulation_ModifyLevelPokeParty( reg, plData->pp );

  plData->myitem = NULL;
  plData->mailblock = NULL;
  plData->cfg = SaveData_GetConfig(svWork);
  plData->tvwk = NULL;
  plData->reg = reg;
  
  plData->zone_id = 0;
  plData->mode = PL_MODE_BATTLE_WIFI;
  switch( reg->BATTLE_TYPE )
  {
  case REGULATION_BATTLE_SINGLE:   ///<シングル
    plData->type = PL_TYPE_SINGLE;
    plData->comm_type = PL_COMM_SINGLE;
    break;
  case REGULATION_BATTLE_DOUBLE:   ///<ダブル
    plData->type = PL_TYPE_DOUBLE;
    plData->comm_type = PL_COMM_SINGLE;
    break;
  case REGULATION_BATTLE_TRIPLE:   ///<トリプル
    plData->type = PL_TYPE_DOUBLE;
    plData->comm_type = PL_COMM_SINGLE;
    break;
  case REGULATION_BATTLE_ROTATION:   ///<ローテーション
    plData->type = PL_TYPE_DOUBLE;
    plData->comm_type = PL_COMM_SINGLE;
    break;
  case REGULATION_BATTLE_MULTI:   ///<マルチ
    plData->type = PL_TYPE_MULTI;
    plData->comm_type = PL_COMM_MULTI;
    break;
  }
  plData->ret_sel = PL_SEL_POS_POKE1;
  plData->ret_mode = PL_RET_NORMAL;

  plData->item = 0;
  plData->waza = 0;
  plData->waza_pos = 0;
  
  for( i=0;i<6;i++ )
  {
    plData->in_num[i] = 0;
  }
  plData->lv_cnt = 0;
  plData->after_mons = 0;
  plData->shinka_cond = 0;
  
  plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].pp = p_param->enemyPokeParty;
  plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].name = p_param->enemyName;
  plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].sex = p_param->enemySex;
  plData->is_disp_party = reg->SHOW_POKE;
  if( reg->SHOW_POKE_TIME != 0 )
  {
    plData->use_tile_limit = TRUE;
    plData->time_limit = reg->SHOW_POKE_TIME;
  }
  else
  {
    plData->use_tile_limit = FALSE;
    plData->time_limit = 0;
  }
  plData->comm_selected_num = 0;
  GFL_OVERLAY_Load(FS_OVERLAY_ID(pokelist_comm));
  PLIST_COMM_InitComm( plData );
}

//--------------------------------------------------------------
//	ステータス初期化データ作成
//--------------------------------------------------------------
static void WIFICLUB_BATTLE_SUBPROC_InitStatusData( WIFICLUB_BATTLE_SUBPROC_PARAM *p_param, WIFICLUB_BATTLE_SUBPROC_WORK *procWork , PSTATUS_DATA *psData )
{
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork(p_param->gameData);
  MYSTATUS *myStatus = GAMEDATA_GetMyStatus( p_param->gameData );
  ZUKAN_SAVEDATA *zukanSave = GAMEDATA_GetZukanSave( p_param->gameData );

  //psData->ppd = procWork->pokeParty;
  psData->ppd = p_param->selfPokeParty;
  psData->game_data = p_param->gameData;
  psData->cfg = SaveData_GetConfig(svWork);

  //psData->player_name = MyStatus_GetMyName( myStatus );
  //psData->player_id = MyStatus_GetID( myStatus );
  //psData->player_sex = MyStatus_GetMySex( myStatus );
  
  psData->ppt = PST_PP_TYPE_POKEPARTY;
  psData->mode = PST_MODE_NO_WAZACHG;
  //psData->max = PokeParty_GetPokeCount(procWork->pokeParty);
  psData->max = PokeParty_GetPokeCount(p_param->selfPokeParty);
  psData->pos = 0;
  psData->page = PPT_INFO;
  
  psData->ret_sel = 0;
  psData->ret_mode = 0;
  psData->waza = 0;

  psData->zukan_mode = ZUKANSAVE_GetZenkokuZukanFlag( zukanSave );
  
  psData->canExitButton = FALSE;
  psData->isExitRequest = FALSE;
}
