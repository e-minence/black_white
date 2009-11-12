//=============================================================================================
/**
 * @file  btl_client.c
 * @brief ポケモンWB バトルシステム クライアントモジュール
 * @author  taya
 *
 * @date  2008.09.06  作成
 */
//=============================================================================================
#include <gflib.h>

#include "sound\pm_sndsys.h"
#include "poke_tool\pokeparty.h"

#include "battle\battle.h"
#include "btl_common.h"
#include "btl_adapter.h"
#include "btl_action.h"
#include "btl_field.h"
#include "btl_pokeselect.h"
#include "btl_server_cmd.h"
#include "app\b_bag.h"
#include "btlv\btlv_core.h"


#include "btl_client.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/

enum {
  CANTESC_COUNT_MAX = 8,    ///< とりあえず少なめギリギリなとこでASSERTかけてみる

  EMPTY_POS_NONE = -1,
};

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*ClientSubProc)( BTL_CLIENT*, int* );
typedef BOOL (*ServerCmdProc)( BTL_CLIENT*, int*, const int* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/
/**
 *  逃げ交換禁止コード管理用構造体
 */
typedef struct {
  u8  counter[ BTL_CANTESC_MAX ][ BTL_POKEID_MAX ];
}CANT_ESC_CONTROL;

/**
 *  文字表示用パラメータ
 */
typedef struct {
  u16  strID;
  u16  stdFlag;
  int  args[ BTL_STR_ARG_MAX ];
}STR_PARAM;

//--------------------------------------------------------------
/**
 *  クライアントモジュール構造定義
 */
//--------------------------------------------------------------
struct _BTL_CLIENT {

  BTL_MAIN_MODULE*      mainModule;
  BTL_POKE_CONTAINER*   pokeCon;
  const BTL_POKEPARAM*  procPoke;
  BTL_ACTION_PARAM*     procAction;

  BTL_ADAPTER*  adapter;
  BTLV_CORE*    viewCore;
  STR_PARAM     strParam;
  BtlRotateDir    prevRotateDir;

  ClientSubProc  subProc;
  int            subSeq;

  ClientSubProc   selActProc;
  int             selActSeq;

  const void*    returnDataPtr;
  u32            returnDataSize;

  const BTL_PARTY*  myParty;
  u8                frontPokeEmpty[ BTL_POSIDX_MAX ];       ///< 担当している戦闘位置にもう出せない時にTRUEにする
  u8                numCoverPos;    ///< 担当する戦闘ポケモン数
  u8                procPokeIdx;    ///< 処理中ポケモンインデックス
  u8                checkedPokeCnt; ///< アクション指示したポケモン数（スキップ状態を勘案）
  BtlPokePos        basePos;        ///< 戦闘ポケモンの位置ID

  BTL_ACTION_PARAM     actionParam[ BTL_POSIDX_MAX ];
  u8                   shooterCost[ BTL_POSIDX_MAX ];
  BTL_SERVER_CMD_QUE*  cmdQue;
  int                  cmdArgs[ BTL_SERVERCMD_ARG_MAX ];
  ServerCmd            serverCmd;
  ServerCmdProc        scProc;
  int                  scSeq;


  BTL_POKESELECT_PARAM    pokeSelParam;
  BTL_POKESELECT_RESULT   pokeSelResult;
  CANT_ESC_CONTROL        cantEscCtrl;


  u8  myID;
  u8  myType;
  u8  myState;
  u8  commWaitInfoOn;
  u8  bagMode;
  u8  shooterEnergy;
  u8  escapeClientID;

  u8          myChangePokeCnt;
  BtlPokePos  myChangePokePos[ BTL_POSIDX_MAX ];

};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd );
static BOOL SubProc_UI_NotifyPokeData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_NotifyPokeData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_Setup( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_Setup( BTL_CLIENT* wk, int *seq );
static BOOL SubProc_UI_SelectRotation( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectRotation( BTL_CLIENT* wk, int* seq );
static BtlRotateDir _testAI_dir( BtlRotateDir prevDir );
static void SelActProc_Set( BTL_CLIENT* wk, ClientSubProc proc );
static BOOL SelActProc_Call( BTL_CLIENT* wk );
static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq );
static BOOL selact_Start( BTL_CLIENT* wk, int* seq );
static BOOL selact_Root( BTL_CLIENT* wk, int* seq );
static BOOL selact_Fight( BTL_CLIENT* wk, int* seq );
static BOOL selact_SelectChangePokemon( BTL_CLIENT* wk, int* seq );
static BOOL selact_Item( BTL_CLIENT* wk, int* seq );
static BOOL selact_Escape( BTL_CLIENT* wk, int* seq );
static BOOL selact_CheckFinish( BTL_CLIENT* wk, int* seq );
static BOOL selact_Finish( BTL_CLIENT* wk, int* seq );
static void shooterCost_Init( BTL_CLIENT* wk );
static void shooterCost_Save( BTL_CLIENT* wk, u8 procPokeIdx, u8 cost );
static u8 shooterCost_Get( BTL_CLIENT* wk, u8 procPokeIdx );
static u8 shooterCost_GetSum( BTL_CLIENT* wk );
static BOOL is_action_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
static BOOL is_waza_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
static void setWaruagakiAction( BTL_ACTION_PARAM* dst, BTL_CLIENT* wk, const BTL_POKEPARAM* bpp );
static BOOL is_unselectable_waza( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, WazaID waza, STR_PARAM* strParam );
static BtlCantEscapeCode is_prohibit_escape( BTL_CLIENT* wk, u8* pokeID );
static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq );
static void abandon_cover_pos( BTL_CLIENT* wk, u8 numPos );
static u8 calcPuttablePokemons( BTL_CLIENT* wk, u8* list );
static u8 calc_front_dead_pokemons( BTL_CLIENT* wk, u8* list );
static void setup_pokesel_param_change( BTL_CLIENT* wk, BTL_POKESELECT_PARAM* param );
static void setup_pokesel_param_dead( BTL_CLIENT* wk, u8 numSelect, BTL_POKESELECT_PARAM* param );
static void store_pokesel_to_action( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res );
static u8 storeMyChangePokePos( BTL_CLIENT* wk, BtlPokePos* myCoverPos );
static BOOL SubProc_UI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL scProc_ACT_MemberOut( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_MemberIn( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_StdSE( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaEffect( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaEffectEx( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg_Plural( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaIchigeki( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ConfDamage( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankDown( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankUp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SickSet( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherStart( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherEnd( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SimpleHP( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Kinomi( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Kill( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Move( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Exp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ExpLvup( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_BallThrow( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Rotation( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_TraceTokusei( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_TOKWIN_In( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_TOKWIN_Out( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_HpZero( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankUp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RankSet5( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_AddCritical( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SickSet( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_CurePokeSick( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_CureWazaSick( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_MemberIn( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_EscapeCodeAdd( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_EscapeCodeSub( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ChangePokeType( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ChangePokeForm( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_WSTurnCheck( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RemoveItem( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_UpdateUseWaza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ResetUsedWaza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetContFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ResetContFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ResetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetActFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ClearActFlag( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetItem( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_UpdateWazaNumber( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_Hensin( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_OutClear( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_AddFldEff( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_RemoveFldEff( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_SetPokeCounter( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_BatonTouch( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_MigawariCreate( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_MigawariDelete( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_OP_ShooterCharge( BTL_CLIENT* wk, int* seq, const int* args );
static void cec_addCode( CANT_ESC_CONTROL* ctrl, u8 pokeID, BtlCantEscapeCode code );
static void cec_subCode( CANT_ESC_CONTROL* ctrl, u8 pokeID, BtlCantEscapeCode code );
static u8 cec_isEnable( CANT_ESC_CONTROL* ctrl, BtlCantEscapeCode code, BTL_CLIENT* wk );
static BOOL _cec_check_kagefumi( BTL_CLIENT* wk );
static BOOL _cec_check_arijigoku( BTL_CLIENT* wk );
static BOOL _cec_check_jiryoku( BTL_CLIENT* wk );
static u8 countFrontPokeTokusei( BTL_CLIENT* wk, PokeTokusei tokusei );
static u8 countFrontPokeType( BTL_CLIENT* wk, PokeType type );



BTL_CLIENT* BTL_CLIENT_Create(
  BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokecon, BtlCommMode commMode,
  GFL_NETHANDLE* netHandle, u16 clientID, u16 numCoverPos, BtlThinkerType clientType, BtlBagMode bagMode,
  HEAPID heapID )
{
  BTL_CLIENT* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_CLIENT) );
  int i;

  wk->myID = clientID;
  wk->myType = clientType;
  wk->adapter = BTL_ADAPTER_Create( netHandle, heapID, clientID );
  wk->myParty = BTL_POKECON_GetPartyDataConst( pokecon, clientID );
  wk->mainModule = mainModule;
  wk->pokeCon = pokecon;
  wk->numCoverPos = numCoverPos;
  wk->procPokeIdx = 0;
  wk->basePos = clientID;
  wk->prevRotateDir = BTL_ROTATEDIR_NONE;

  for(i=0; i<NELEMS(wk->frontPokeEmpty); ++i){
    wk->frontPokeEmpty[i] = FALSE;
  }
  wk->cmdQue = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER_CMD_QUE) );

  wk->myState = 0;
  wk->commWaitInfoOn = FALSE;
  wk->shooterEnergy = 0;
  wk->bagMode = bagMode;
  wk->escapeClientID = BTL_CLIENTID_NULL;

  return wk;
}

void BTL_CLIENT_Delete( BTL_CLIENT* wk )
{
  GFL_HEAP_FreeMemory( wk->cmdQue );
  BTL_ADAPTER_Delete( wk->adapter );

  GFL_HEAP_FreeMemory( wk );
}

//=============================================================================================
/**
 * 描画処理モジュールをアタッチ（UIクライアントのみ）
 *
 * @param   wk
 * @param   viewCore
 *
 */
//=============================================================================================
void BTL_CLIENT_AttachViewCore( BTL_CLIENT* wk, BTLV_CORE* viewCore )
{
  wk->viewCore = viewCore;
}


BTL_ADAPTER* BTL_CLIENT_GetAdapter( BTL_CLIENT* wk )
{
  GF_ASSERT(wk);
  return wk->adapter;
}


BOOL BTL_CLIENT_Main( BTL_CLIENT* wk )
{
  switch( wk->myState ){
  case 0:
    {
      BtlAdapterCmd  cmd = BTL_ADAPTER_RecvCmd(wk->adapter);
      if( cmd == BTL_ACMD_QUIT_BTL )
      {
        const u32* p;
        BTL_ADAPTER_GetRecvData( wk->adapter, (const void**)&p );
        wk->escapeClientID = *p;
        return TRUE;
      }
      if( cmd != BTL_ACMD_NONE )
      {
        wk->subProc = getSubProc( wk, cmd );
        wk->subSeq = 0;
        wk->myState = 1;
      }
    }
    break;

  case 1:
    if( wk->subProc(wk, &wk->subSeq) ){
      BTL_Printf("ID[%d], 返信開始へ\n", wk->myID );
      wk->myState++;
    }
    break;

  case 2:
    if( BTL_ADAPTER_ReturnCmd(wk->adapter, wk->returnDataPtr, wk->returnDataSize) ){
      wk->myState = 0;
      BTL_Printf("ID[%d], 返信しました\n", wk->myID );
    }
    break;
  }
  return FALSE;
}
static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd )
{
  static const struct {
    BtlAdapterCmd   cmd;
    ClientSubProc   procs[ BTL_THINKER_TYPE_MAX ];
  }procTbl[] = {

    { BTL_ACMD_NOTIFY_POKEDATA, { SubProc_UI_NotifyPokeData,SubProc_AI_NotifyPokeData }, },
    { BTL_ACMD_WAIT_SETUP,      { SubProc_UI_Setup,          SubProc_AI_Setup } },
    { BTL_ACMD_SELECT_ROTATION, { SubProc_UI_SelectRotation, SubProc_AI_SelectRotation } },
    { BTL_ACMD_SELECT_ACTION,   { SubProc_UI_SelectAction,   SubProc_AI_SelectAction } },
    { BTL_ACMD_SELECT_POKEMON,  { SubProc_UI_SelectPokemon,  SubProc_AI_SelectPokemon } },
    { BTL_ACMD_SERVER_CMD,      { SubProc_UI_ServerCmd,      SubProc_AI_ServerCmd } },

  };

  int i;

  for(i=0; i<NELEMS(procTbl); i++)
  {
    if( procTbl[i].cmd == cmd )
    {
      return procTbl[i].procs[ wk->myType ];
    }
  }

  GF_ASSERT(0);
  return NULL;
}

//=============================================================================================
/**
 * 逃げたクライアントのIDを取得（サーバコマンドを介して送られてくる）
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_CLIENT_GetEscapeClientID( const BTL_CLIENT* wk )
{
  return wk->escapeClientID;
}
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------


static BOOL SubProc_UI_NotifyPokeData( BTL_CLIENT* wk, int* seq )
{
//  wk->returnDataPtr =
  return TRUE;
}
static BOOL SubProc_AI_NotifyPokeData( BTL_CLIENT* wk, int* seq )
{
  return TRUE;
}
static BOOL SubProc_UI_Setup( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_StartCommand( wk->viewCore, BTLV_CMD_SETUP );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitCommand(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}

static BOOL SubProc_AI_Setup( BTL_CLIENT* wk, int *seq )
{

  return TRUE;
}



//------------------------------------------------------------------------------------------------------
// ローテーション選択
//------------------------------------------------------------------------------------------------------

static BOOL SubProc_UI_SelectRotation( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      BTLV_UI_SelectRotation_Start( wk->viewCore, wk->prevRotateDir );
      (*seq)++;
    }
    break;

  case 1:
    {
      BtlRotateDir  dir;
      if( BTLV_UI_SelectRotation_Wait(wk->viewCore, &dir) ){
        // @todo 今は適当AIと同じルーチンで決定
        wk->prevRotateDir = _testAI_dir( wk->prevRotateDir );
        wk->returnDataPtr = &wk->prevRotateDir;
        wk->returnDataSize = sizeof(wk->prevRotateDir);
        return TRUE;
      }
    }
    break;
  }

  return FALSE;
}

static BOOL SubProc_AI_SelectRotation( BTL_CLIENT* wk, int* seq )
{
  // @todo AI用。今は仮動作
  wk->prevRotateDir = _testAI_dir( wk->prevRotateDir );
  wk->returnDataPtr = &wk->prevRotateDir;
  wk->returnDataSize = sizeof(wk->prevRotateDir);

  return TRUE;
}
static BtlRotateDir _testAI_dir( BtlRotateDir prevDir )
{
  u32 rnd = GFL_STD_MtRand( 100 );

  if( prevDir == BTL_ROTATEDIR_NONE )
  {
    if( rnd < 30 ){
      prevDir = BTL_ROTATEDIR_L;
    }else if( rnd < 60 ){
      prevDir = BTL_ROTATEDIR_R;
    }else{
      prevDir = BTL_ROTATEDIR_STAY;
    }
  }
  else
  {
    switch( prevDir ){
    case BTL_ROTATEDIR_STAY:
    default:
      prevDir = (rnd < 50)? BTL_ROTATEDIR_R : BTL_ROTATEDIR_L;
      break;

    case BTL_ROTATEDIR_L:
      prevDir = (rnd < 40)? BTL_ROTATEDIR_R : BTL_ROTATEDIR_STAY;
      break;

    case BTL_ROTATEDIR_R:
      prevDir = (rnd < 40)? BTL_ROTATEDIR_L : BTL_ROTATEDIR_STAY;
      break;
    }
  }

  return prevDir;
}


//--------------

static void SelActProc_Set( BTL_CLIENT* wk, ClientSubProc proc )
{
  wk->selActProc = proc;
  wk->selActSeq = 0;
}
static BOOL SelActProc_Call( BTL_CLIENT* wk )
{
  return wk->selActProc( wk, &wk->selActSeq );
}

static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    SelActProc_Set( wk, selact_Start );
    (*seq)++;
    break;

  case 1:
    if( SelActProc_Call(wk) ){
      return TRUE;
    }
    break;
  }

  return FALSE;
}

//----------------------------------------------------------------------
/**
 *  アクション選択ワーク初期化
 */
//----------------------------------------------------------------------
static BOOL selact_Start( BTL_CLIENT* wk, int* seq )
{
  wk->procPokeIdx = 0;
  wk->checkedPokeCnt = 0;

  setup_pokesel_param_change( wk, &wk->pokeSelParam );
  shooterCost_Init( wk );

  SelActProc_Set( wk, selact_Root );
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  アクション選択ルート
 */
//----------------------------------------------------------------------
static BOOL selact_Root( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    wk->procPoke = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, wk->procPokeIdx );
    wk->procAction = &wk->actionParam[ wk->procPokeIdx ];
    if( is_action_unselectable(wk, wk->procPoke,  wk->procAction) ){
      BTL_Printf("アクション選択(%d体目）スキップします\n", wk->procPokeIdx );
      SelActProc_Set( wk, selact_CheckFinish );
    }else{
      (*seq)++;
    }
    break;

  case 1:
    BTL_Printf("アクション選択(%d体目=ID:%d）開始します\n", wk->procPokeIdx, BPP_GetID(wk->procPoke));
    BTLV_UI_SelectAction_Start( wk->viewCore, wk->procPoke, (wk->procPokeIdx!=0), wk->procAction );
    (*seq)++;
    break;

  case 2:
    switch( BTLV_UI_SelectAction_Wait(wk->viewCore) ){

    // 入れ替えポケモン選択の場合はまだアクションパラメータが不十分->ポケモン選択へ
    case BTL_ACTION_CHANGE:
      shooterCost_Save( wk, wk->procPokeIdx, 0 );
      SelActProc_Set( wk, selact_SelectChangePokemon );
      break;

    // 「たたかう」を選んだ
    case BTL_ACTION_FIGHT:
      shooterCost_Save( wk, wk->procPokeIdx, 0 );
      SelActProc_Set( wk, selact_Fight );
      break;

    // 「どうぐ」を選んだ
    case BTL_ACTION_ITEM:
//      (*seq) = SEQ_CHECK_ITEM;
      SelActProc_Set( wk, selact_Item );
      break;

    // 「にげる」or「もどる」
    case BTL_ACTION_ESCAPE:
      // 先頭のポケなら「にげる」として処理
      // （先頭ポケが行動選択不可のケースもあるので checkedPokeCnt で判定）
      if( wk->checkedPokeCnt == 0 ){
        BTL_Printf("「にげる」を選びました\n");
        shooterCost_Save( wk, wk->procPokeIdx, 0 );
        SelActProc_Set( wk, selact_Escape );
//        (*seq) = SEQ_CHECK_ESCAPE;
      // ２体目以降は「もどる」として処理
      }else{
        BTL_POKEPARAM* bpp;
        while( wk->procPokeIdx )
        {
          wk->procPokeIdx--;
          bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, wk->procPokeIdx );
          if( !is_action_unselectable(wk, bpp, NULL) )
          {
            wk->checkedPokeCnt--;
            wk->shooterEnergy += shooterCost_Get( wk, wk->procPokeIdx );
            SelActProc_Set( wk, selact_Root );
//          (*seq) = SEQ_CHECK_UNSEL_ACTION;
            return FALSE;
          }
        }
        GF_ASSERT(0);
        break;
      }
      break;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  「たたかう」選択後の分岐
 */
//----------------------------------------------------------------------
static BOOL selact_Fight( BTL_CLIENT* wk, int* seq )
{
  enum {
    SEQ_START = 0,
    SEQ_SELECT_WAZA_START,
    SEQ_SELECT_WAZA_WAIT,
    SEQ_CHECK_WAZA_TARGET,
    SEQ_SELECT_TARGET_START,
    SEQ_SELECT_TARGET_WAIT,
    SEQ_WAIT_MSG,
  };

  switch( *seq ){
  case 0:
    if( is_waza_unselectable( wk, wk->procPoke, wk->procAction ) ){
      SelActProc_Set( wk, selact_CheckFinish );
    }else{
      (*seq) = SEQ_SELECT_WAZA_START;
    }
    break;

  case SEQ_SELECT_WAZA_START:
    BTLV_UI_SelectWaza_Start( wk->viewCore, wk->procPoke, wk->procAction );
    (*seq) = SEQ_SELECT_WAZA_WAIT;
    break;

  case SEQ_SELECT_WAZA_WAIT:
    if( BTLV_UI_SelectWaza_Wait(wk->viewCore) )
    {
      BtlAction action = BTL_ACTION_GetAction( wk->procAction );

      if( action == BTL_ACTION_NULL ){
        SelActProc_Set( wk, selact_Root );
      }
      else if( action == BTL_ACTION_MOVE ){
        SelActProc_Set( wk, selact_CheckFinish );
      }else
      {
        if( is_unselectable_waza(wk, wk->procPoke, wk->actionParam[wk->procPokeIdx].fight.waza, &wk->strParam) )
        {
          if( wk->strParam.stdFlag ){
            BTLV_StartMsgStd( wk->viewCore, wk->strParam.strID, wk->strParam.args );
          }else{
            BTLV_StartMsgSet( wk->viewCore, wk->strParam.strID, wk->strParam.args );
          }
          (*seq) = SEQ_WAIT_MSG;
        }
        else{
          (*seq) = SEQ_CHECK_WAZA_TARGET;
        }
      }
    }
    break;

  case SEQ_CHECK_WAZA_TARGET:
    // シングルなら対象選択なし
    {
      BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
      if( BTL_RULE_IsNeedSelectTarget(rule) ){
        (*seq) = SEQ_SELECT_TARGET_START;
      }else{
        SelActProc_Set( wk, selact_CheckFinish );
      }
    }
    break;

  case SEQ_SELECT_TARGET_START:
    BTLV_UI_SelectTarget_Start( wk->viewCore, wk->procPoke, wk->procAction );
    (*seq) = SEQ_SELECT_TARGET_WAIT;
    break;

  case SEQ_SELECT_TARGET_WAIT:
    {
      BtlvResult result = BTLV_UI_SelectTarget_Wait( wk->viewCore );
      if( result == BTLV_RESULT_DONE ){
        BTL_Printf("選択した対象ポケPos=%d\n", wk->procAction->fight.targetPos);
        SelActProc_Set( wk, selact_CheckFinish );
      }else if( result == BTLV_RESULT_CANCEL ){
        (*seq) = SEQ_SELECT_WAZA_START;
      }
    }
    break;

  case SEQ_WAIT_MSG:
    if( BTLV_WaitMsg(wk->viewCore) ){
      BTLV_UI_Restart( wk->viewCore );
      SelActProc_Set( wk, selact_Root );
    }
    break;
  }

  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  入れ替えポケモン選択
 */
//----------------------------------------------------------------------
static BOOL selact_SelectChangePokemon( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, &wk->pokeSelResult );
    (*seq)++;
    break;

  case 1:
    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      if( BTL_POKESELECT_IsDone( &wk->pokeSelResult ) )
      {
        u8 idx = BTL_POKESELECT_RESULT_GetLast( &wk->pokeSelResult );
        BTL_Printf("選んだポケidx=%d\n", idx);
        if( idx < BTL_PARTY_MEMBER_MAX ){
          BTL_ACTION_SetChangeParam( &wk->actionParam[wk->procPokeIdx], wk->procPokeIdx, idx );
          SelActProc_Set( wk, selact_CheckFinish );
          break;
        }
      }

      SelActProc_Set( wk, selact_Root );
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  「どうぐ」選択
 */
//----------------------------------------------------------------------
static BOOL selact_Item( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      u8 cost_sum = shooterCost_GetSum( wk );
      BTLV_ITEMSELECT_Start( wk->viewCore, wk->bagMode, wk->shooterEnergy, cost_sum );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_ITEMSELECT_Wait(wk->viewCore) )
    {
      u16 itemID, targetIdx;
      itemID = BTLV_ITEMSELECT_GetItemID( wk->viewCore );
      targetIdx = BTLV_ITEMSELECT_GetTargetIdx( wk->viewCore );
      if( (itemID != ITEM_DUMMY_DATA) && (targetIdx != BPL_SEL_EXIT) )
      {
        u8 cost = BTLV_ITEMSELECT_GetCost( wk->viewCore );
        if( wk->shooterEnergy >= cost ){
          wk->shooterEnergy -= cost;
        }else{
          GF_ASSERT_MSG(0, "Energy=%d, item=%d, cost=%d\n", wk->shooterEnergy, itemID, cost );
          wk->shooterEnergy = 0;
          cost = wk->shooterEnergy;
        }
        shooterCost_Save( wk, wk->procPokeIdx, cost );
        BTL_ACTION_SetItemParam( wk->procAction, itemID, targetIdx );
        SelActProc_Set( wk, selact_CheckFinish );
      }else{
//      (*seq)=SEQ_SELECT_ACTION;
        SelActProc_Set( wk, selact_Root );
      }
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  「にげる」選択
 */
//----------------------------------------------------------------------
static BOOL selact_Escape( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
  {
    // 戦闘モード等による禁止チェック（トレーナー戦など）
    BtlEscapeMode esc = BTL_MAIN_GetEscapeMode( wk->mainModule );
    switch( esc ){
    case BTL_ESCAPE_MODE_OK:
    default:
      {
        BtlCantEscapeCode  code;
        u8 pokeID;
        code = is_prohibit_escape( wk, &pokeID );
        // とくせい、ワザ効果等による禁止チェック
        if( code == BTL_CANTESC_NULL )
        {
          wk->returnDataPtr = wk->procAction;
          wk->returnDataSize = sizeof(wk->actionParam[0]);
          SelActProc_Set( wk, selact_Finish );
        }
        else
        {
          int args[2];
          args[0] = pokeID;
          args[1] = code;
          BTLV_StartMsgSet( wk->viewCore, BTL_STRID_SET_CantEscTok, args );
          (*seq) = 1;
        }
      }
      break;

    case BTL_ESCAPE_MODE_NG:
      BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_EscapeCant, NULL );
      (*seq) = 1;
      break;
    }
  }
  break;

  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      //(*seq) = SEQ_SELECT_ACTION;
      SelActProc_Set( wk, selact_Root );
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  全アクション選択終了チェック
 */
//----------------------------------------------------------------------
static BOOL selact_CheckFinish( BTL_CLIENT* wk, int* seq )
{
  BTLV_UI_Restart( wk->viewCore );
  wk->procPokeIdx++;
  wk->checkedPokeCnt++;
  if( wk->procPokeIdx >= wk->numCoverPos )
  {
    BTL_Printf("カバー位置数(%d)終了、アクション送信へ\n", wk->numCoverPos);
    wk->returnDataPtr = &(wk->actionParam[0]);
    wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;
    SelActProc_Set( wk, selact_Finish );
  }
  else{
    SelActProc_Set( wk, selact_Root );
  }

  return FALSE;
}
//----------------------------------------------------------------------
/**
 *  全アクション選択終了->サーバ返信へ
 */
//----------------------------------------------------------------------
static BOOL selact_Finish( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    BTLV_EFFECT_Stop();
    BTLV_EFFECT_Add( BTLEFF_CAMERA_INIT );
    if( BTL_MAIN_GetCommMode(wk->mainModule) != BTL_COMM_NONE )
    {
      BTL_Printf("通信中なので待機中メッセージだす\n");
      wk->commWaitInfoOn = TRUE;
      BTLV_StartCommWait( wk->viewCore );
      (*seq)++;
    }
    else
    {
      BTL_Printf("通信中じゃないので待機中メッセージだしません\n");
      (*seq)+=2;
    }
    break;

  case 1:
    if( BTLV_WaitCommWait(wk->viewCore) ){
      (*seq)++;
    }
    break;

  case 2:
    if( !BTLV_EFFECT_CheckExecute() ){
      (*seq)++;
      return TRUE;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}


/*============================================================================================*/
/* シューターコスト計算用ワーク                                                               */
/*============================================================================================*/
// 初期化
static void shooterCost_Init( BTL_CLIENT* wk )
{
  GFL_STD_MemClear( wk->shooterCost, sizeof(wk->shooterCost) );
}
// 使用アイテムコストを記録
static void shooterCost_Save( BTL_CLIENT* wk, u8 procPokeIdx, u8 cost )
{
  wk->shooterCost[ procPokeIdx ] = cost;
}
static u8 shooterCost_Get( BTL_CLIENT* wk, u8 procPokeIdx )
{
  return wk->shooterCost[ procPokeIdx ];
}
// 使用予定の合計値を取得
static u8 shooterCost_GetSum( BTL_CLIENT* wk )
{
  u32 i, sum = 0;
  for(i=0; i<wk->procPokeIdx; ++i){
    sum += wk->shooterCost[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------
/**
 * アクション選択不可状態のポケモンかどうか判定
 * 不可状態であれば、アクション内容を自動生成する
 *
 * @param   wk
 * @param   bpp       判定対象ポケモンパラメータ
 * @param   action    [out] アクション内容生成先
 *
 * @retval  BOOL      不可状態ならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_action_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action )
{
  // 死んでたらNULLデータを返す
  if( BPP_IsDead(wk->procPoke) )
  {
    if( action ){
      BTL_ACTION_SetNULL( action );
    }
    return TRUE;
  }
  // アクション選択できない（攻撃の反動など）場合はスキップ
  if( BPP_GetActFlag(wk->procPoke, BPP_ACTFLG_CANT_ACTION) )
  {
    if( action ){
      BTL_ACTION_SetSkip( action );
    }
    return TRUE;
  }
  // ワザロック状態（前回ワザをそのまま使う）は勝手にワザ選択処理してスキップ
  if( BPP_CheckSick(wk->procPoke, WAZASICK_WAZALOCK) )
  {
    WazaID waza = BPP_GetPrevWazaID( wk->procPoke );
    BtlPokePos pos = BPP_GetPrevTargetPos( wk->procPoke );
    u8 waza_idx;
    waza_idx = BPP_WAZA_SearchIdx( wk->procPoke, waza );
    BTL_Printf("ワザロック：前回使ったワザは %d, idx=%d, targetPos=%d\n", waza, waza_idx, pos);
    if( BPP_WAZA_GetPP(wk->procPoke, waza_idx) ){
      if( action ){
        BTL_ACTION_SetFightParam( action, waza, pos );
      }
    }else{
      // PPゼロならわるあがき
      if( action ){
        setWaruagakiAction( action, wk, wk->procPoke );
      }
    }
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ワザ選択不可状態のポケモンかどうか判定
 * ワザ選択不可状態であれば、アクション内容を自動生成する
 *
 * @param   wk
 * @param   bpp       判定対象ポケモンパラメータ
 * @param   action    [out] アクション内容生成先
 *
 * @retval  BOOL      ワザ選択不可状態ならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_waza_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action )
{
  // ちょうはつ状態（ダメージワザしか選べない）は、選べるワザを持っていなければ「わるあがき」セットでスキップ
  if( BPP_CheckSick(bpp, WAZASICK_TYOUHATSU) )
  {
    int i, cnt, max = BPP_WAZA_GetCount( bpp );
    for(i=0; i<max; ++i){
      if( BPP_WAZA_GetPP(bpp, i) && WAZADATA_IsDamage(BPP_WAZA_GetID(bpp, i)) ){
        break;
      }
    }
    if( i == max ){
      setWaruagakiAction( action, wk, bpp );
      return TRUE;
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * 「わるあがき」用アクションパラメータセット
 *
 * @param   dst
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void setWaruagakiAction( BTL_ACTION_PARAM* dst, BTL_CLIENT* wk, const BTL_POKEPARAM* bpp )
{
  BtlPokePos myPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(bpp) );
  BTL_ACTION_SetFightParam( dst, WAZANO_WARUAGAKI, myPos );
}
//----------------------------------------------------------------------------------
/**
 * 使用できないワザが選ばれたかどうか判定
 *
 * @param   wk
 * @param   bpp       対象ポケモン
 * @param   waza      ワザ
 * @param   strParam  [out]警告メッセージパラメータ出力先
 *
 * @retval  BOOL    使用できないワザの場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL is_unselectable_waza( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, WazaID waza, STR_PARAM* strParam )
{
  // こだわりアイテム効果（最初に使ったワザしか選べない）
  if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KODAWARI_LOCK) )
  {
    if( waza != BPP_GetPrevWazaID(bpp) ){
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_STD_KodawariLock;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BPP_GetID( bpp );
        strParam->args[1] = BPP_GetItem( bpp );
        strParam->args[2] = waza;
      }
      return TRUE;
    }
  }

  // ワザロック効果（前回と同じワザしか出せない）
  if( BPP_CheckSick(bpp, WAZASICK_ENCORE) )
  {
    if( waza != BPP_GetPrevWazaID(bpp) ){
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_STD_WazaLock;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BPP_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

  // ちょうはつ状態（ダメージワザしか選べない）
  if( BPP_CheckSick(bpp, WAZASICK_TYOUHATSU) )
  {
    if( !WAZADATA_IsDamage(waza) )
    {
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_SET_ChouhatuWarn;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BPP_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

  // いちゃもん状態（２ターン続けて同じワザを選べない）
  if( BPP_CheckSick(bpp, WAZASICK_ICHAMON) )
  {
    if( BPP_GetPrevWazaID(bpp) == waza )
    {
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_SET_IchamonWarn;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BPP_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

  // かなしばり状態（かなしばり直前に出していたワザを選べない）
  if( BPP_CheckSick(bpp, WAZASICK_KANASIBARI) )
  {
    u8 idx = BPP_GetSickParam( bpp, WAZASICK_KANASIBARI );
    if( BPP_WAZA_SearchIdx(bpp, waza) == idx )
    {
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_SET_KanasibariWarn;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BPP_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

// ふういんチェック（ふういんをかけたポケが持ってるワザを出せない）
  if( BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
  {
    u8 fuinPokeID = BTL_FIELD_GetDependPokeID( BTL_FLDEFF_FUIN );
    u8 myPokeID = BPP_GetID( bpp );
    BTL_Printf("ふういん実施中です\n");
    if( myPokeID != fuinPokeID )
    {
      const BTL_POKEPARAM* fuinPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, fuinPokeID );
      BTL_Printf("自分はふういんポケじゃありません\n");
      if( BPP_WAZA_SearchIdx(fuinPoke, waza) != PTL_WAZA_MAX )
      {
        BTL_Printf("そのワザ(%d)はふういんポケが持ってますので使えません\n", waza);
        if( strParam != NULL )
        {
          strParam->strID = BTL_STRID_SET_FuuinWarn;
          strParam->stdFlag = FALSE;
          strParam->args[0] = myPokeID;
          strParam->args[1] = waza;
        }
        return TRUE;
      }
    }
  }
  return FALSE;
}


//--------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BtlCantEscapeCode is_prohibit_escape( BTL_CLIENT* wk, u8* pokeID )
{
  BtlCantEscapeCode  code;

  for(code=0; code<BTL_CANTESC_MAX; ++code)
  {
    *pokeID = cec_isEnable(&wk->cantEscCtrl, code, wk);
    if( *pokeID != BTL_POKEID_NULL )
    {
      return code;
    }
  }

  return BTL_CANTESC_NULL;
}


static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq )
{
  // @@@ この関数は現在しぬほど適当に作られている。taya
  const BTL_POKEPARAM* pp;
  u8 i = 0;

  for(i=0; i<wk->numCoverPos; ++i)
  {
    pp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
    if( !BPP_IsDead(pp) )
    {
      u8 wazaCount, wazaIdx, mypos, targetPos;

      mypos = BTL_MAIN_GetClientPokePos( wk->mainModule, wk->myID, i );

      if( BPP_CheckSick(pp, WAZASICK_ENCORE)
      ||  BPP_CheckSick(pp, WAZASICK_WAZALOCK)
      ){
        WazaID waza = BPP_GetPrevWazaID( pp );
        BtlPokePos pos = BPP_GetPrevTargetPos( pp );
        BTL_ACTION_SetFightParam( &wk->actionParam[i], waza, pos );
        continue;
      }

      wazaCount = BPP_WAZA_GetCount( pp );
      {
        u8 usableWazaIdx[ PTL_WAZA_MAX ];
        WazaID waza;
        u8 j, cnt=0;
        for(j=0, cnt=0; j<wazaCount; ++j){
          if( BPP_WAZA_GetPP(pp, j) )
          {
            waza = BPP_WAZA_GetID( pp, j );
            if( !is_unselectable_waza(wk, pp, waza, NULL) ){
              usableWazaIdx[cnt++] = j;
            }
          }
        }
        if( cnt ){
          cnt = GFL_STD_MtRand( cnt );
          wazaIdx = usableWazaIdx[ cnt ];
          targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, mypos, 0 );
        }else{
          setWaruagakiAction( &wk->actionParam[i], wk, pp );
          continue;
        }
      }

      // シングルでなければ、対象をランダムで決定する処理
      if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_SINGLE )
      {
        enum {
          CHECK_MAX = 2,
        };
        const BTL_POKEPARAM* targetPoke;
        u8 j, p, aliveCnt;
        u8 alivePokePos[ CHECK_MAX ];
        aliveCnt = 0;
        for(j=0; j<CHECK_MAX; ++j)
        {
          p = BTL_MAIN_GetOpponentPokePos( wk->mainModule, mypos, j );
          targetPoke = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, p );
          if( !BPP_IsDead(targetPoke) )
          {
            alivePokePos[ aliveCnt++ ] = p;
          }
        }
        if( aliveCnt )
        {
          u8 rndIdx = GFL_STD_MtRand(aliveCnt);
          targetPos = alivePokePos[ rndIdx ];
        }
      }
      {
        WazaID waza = BPP_WAZA_GetID( pp, wazaIdx );
        BTL_Printf("ワシAIデス. ワシの位置=%d, 狙う位置=%dデス\n", mypos, targetPos);
        BTL_ACTION_SetFightParam( &wk->actionParam[i], waza, targetPos );
      }
    }
    else
    {
      BTL_ACTION_SetNULL( &wk->actionParam[i] );
    }
  }
  wk->returnDataPtr = &(wk->actionParam[0]);
  wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;

  return TRUE;
}
//---------------------------------------------------
// 担当位置を放棄する  numPos : 放棄する位置の数
//---------------------------------------------------
static void abandon_cover_pos( BTL_CLIENT* wk, u8 numPos )
{
  u8 i = wk->numCoverPos - 1;
  while( numPos )
  {
    if( wk->frontPokeEmpty[i] == FALSE )
    {
      wk->frontPokeEmpty[i] =  TRUE;
      numPos--;
    }
    if( i ) {
      i--;
    } else {
      break;
    }
  }
}

//
//--------------------------------------------------------------------------
/**
 * 死亡時の選択可能ポケモン（場に出ているポケモン以外で、生きているポケモン）の数
 *
 * @param   wk
 * @param   list    [out] 選択可能ポケモンのパーティ内インデックスを格納する配列
 *
 * @retval  u8    選択可能ポケモンの数
 */
//--------------------------------------------------------------------------
static u8 calcPuttablePokemons( BTL_CLIENT* wk, u8* list )
{
  u8 cnt, numMembers, numFront, i, j;

  numMembers = BTL_PARTY_GetMemberCount( wk->myParty );
  numFront = BTL_RULE_HandPokeIndex( BTL_MAIN_GetRule(wk->mainModule), wk->numCoverPos );

  for(i=numFront, cnt=0; i<numMembers; i++)
  {
    {
      const BTL_POKEPARAM* pp = BTL_PARTY_GetMemberDataConst(wk->myParty, i);
      if( !BPP_IsDead(pp) )
      {
        if( list ){
          list[cnt] = i;
        }
        ++cnt;
      }
    }
  }
  return cnt;
}
//--------------------------------------------------------------------------
/**
 * ポケモン選択画面用パラメータセット（アクション「ポケモン」で入れ替える時）
 *
 * @param   wk          クライアントモジュールハンドラ
 * @param   param       [out] 選択画面用パラメータを格納する
 *
 */
//--------------------------------------------------------------------------
static void setup_pokesel_param_change( BTL_CLIENT* wk, BTL_POKESELECT_PARAM* param )
{
  BTL_POKESELECT_PARAM_Init( param, wk->myParty, 1, TRUE );
  BTL_POKESELECT_PARAM_SetProhibitFighting( param, wk->numCoverPos );
}
//--------------------------------------------------------------------------
/**
 * ポケモン選択画面用パラメータセット（自分のポケモンが瀕死になったとき）
 *
 * @param   wk          クライアントモジュールハンドラ
 * @param   numSelect   選択しなければいけない数
 * @param   param       [out] 選択画面用パラメータを格納する
 *
 */
//--------------------------------------------------------------------------
static void setup_pokesel_param_dead( BTL_CLIENT* wk, u8 numSelect, BTL_POKESELECT_PARAM* param )
{
  BTL_POKESELECT_PARAM_Init( param, wk->myParty, numSelect, TRUE );
  BTL_POKESELECT_PARAM_SetProhibitFighting( param, wk->numCoverPos );
}

// ポケモン選択画面結果 -> 決定アクションパラメータに変換
static void store_pokesel_to_action( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res )
{
  // res->cnt=選択されたポケモン数,  wk->myChangePokeCnt=自分が担当する入れ替えリクエストポケモン数
  GF_ASSERT_MSG(res->cnt <= wk->myChangePokeCnt, "selCnt=%d, changePokeCnt=%d\n", res->cnt, wk->myChangePokeCnt);

  {
    u8 clientID, posIdx, selIdx, i;

    for(i=0; i<res->cnt; i++)
    {
      selIdx = res->selIdx[i];
      BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, wk->myChangePokePos[i], &clientID, &posIdx );
      BTL_Printf("ポケモン入れ替え %d体目 <-> %d体目\n", posIdx, selIdx );
      BTL_ACTION_SetChangeParam( &wk->actionParam[i], posIdx, selIdx );
    }
    wk->returnDataPtr = &(wk->actionParam[0]);
    wk->returnDataSize = sizeof(wk->actionParam[0]) * res->cnt;
  }
}

//----------------------------------------------------------------------------------
/**
 * サーバから送られてきた交替リクエストポケモン位置から、自分の担当する位置のみを抜き出してバッファに格納
 *
 * @param   wk
 * @param   myCoverPos      [out] changePokePos中、自分が担当する位置を格納するバッファ
 *
 * @retval  u8    myCoverPos配列に格納した、自分が担当する位置の数.
 */
//----------------------------------------------------------------------------------
static u8 storeMyChangePokePos( BTL_CLIENT* wk, BtlPokePos* myCoverPos )
{
  u8 numChangePoke;
  const u8 *changePokePos;
  u32 i, cnt;

  numChangePoke = BTL_ADAPTER_GetRecvData( wk->adapter, (const void*)&changePokePos );
  BTL_Printf(" 全Client, 選択すべきポケモン数=%d\n　位置=", numChangePoke);
  for(i=0; i<numChangePoke; ++i){
    BTL_PrintfSimple("%d,", changePokePos[i]);
  }
  BTL_PrintfSimple("\n");

  for(i=0, cnt=0; i<numChangePoke; ++i)
  {
    if( BTL_MAIN_BtlPosToClientID(wk->mainModule, changePokePos[i]) == wk->myID ){
      myCoverPos[ cnt++ ] = changePokePos[ i ];
    }
  }
  BTL_Printf(" 自分[%d]が選択すべきポケモン数=%d\n", wk->myID, cnt);

  return cnt;
}

static BOOL SubProc_UI_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      wk->myChangePokeCnt = storeMyChangePokePos( wk, wk->myChangePokePos );

      if( wk->myChangePokeCnt )
      {
        u8 numPuttable = calcPuttablePokemons( wk, NULL );
        if( numPuttable )
        {
          u8 numSelect = wk->myChangePokeCnt;

          // 生きてるポケが出さなければいけない数に足りない場合、そこを諦める
          if( numSelect > numPuttable )
          {
            abandon_cover_pos( wk, numSelect - numPuttable );
            numSelect = numPuttable;
          }

          BTL_Printf("myID=%d 戦闘ポケ位置が死んだのでポケモン%d体選択\n", wk->myID, numSelect);
          setup_pokesel_param_dead( wk, numSelect, &wk->pokeSelParam );
          BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, &wk->pokeSelResult );
          (*seq)++;
        }
        else
        {
          BTL_Printf("myID=%d もう戦えるポケモンいないことを返信\n", wk->myID);
          BTL_ACTION_SetChangeDepleteParam( &wk->actionParam[0] );
          wk->returnDataPtr = &(wk->actionParam[0]);
          wk->returnDataSize = sizeof(wk->actionParam[0]);
          return TRUE;
        }
      }
      else
      {
          BTL_Printf("myID=%d 誰も死んでないので選ぶ必要なし\n", wk->myID);
          BTL_ACTION_SetNULL( &wk->actionParam[0] );
          wk->returnDataPtr = &(wk->actionParam[0]);
          wk->returnDataSize = sizeof(wk->actionParam[0]);
          return TRUE;
      }
    }
    break;

  case 1:
    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      store_pokesel_to_action( wk, &wk->pokeSelResult );
      return TRUE;
    }
    break;
  }

  return FALSE;
}
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
  wk->myChangePokeCnt = storeMyChangePokePos( wk, wk->myChangePokePos );
  if( wk->myChangePokeCnt )
  {
    u8 puttableList[ BTL_PARTY_MEMBER_MAX ];
    u8 numPuttable;

    numPuttable = calcPuttablePokemons( wk, puttableList );
    if( numPuttable )
    {
      u8 numSelect = wk->myChangePokeCnt;
      u8 posIdx, clientID, i;

      // 生きてるポケが出さなければいけない数に足りない場合、そこを諦める
      if( numSelect > numPuttable )
      {
        abandon_cover_pos( wk, numSelect - numPuttable );
        numSelect = numPuttable;
      }
      BTL_Printf("myID=%d 戦闘ポケが死んだのでポケモン%d体選択\n", wk->myID, numSelect);
      for(i=0; i<numSelect; i++)
      {
        BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, wk->myChangePokePos[i], &clientID, &posIdx );
        BTL_ACTION_SetChangeParam( &wk->actionParam[i], posIdx, puttableList[i] );
        BTL_Printf(" %d番目を新たに出します\n", puttableList[i] );
      }
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]) * numSelect;
    }
    else
    {
      BTL_Printf("myID=%d もう戦えるポケモンいないことを返信\n", wk->myID);
      BTL_ACTION_SetChangeDepleteParam( &wk->actionParam[0] );
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]);
    }
  }
  else
  {
      BTL_Printf("myID=%d 誰も死んでないので選ぶ必要なし\n", wk->myID);
      BTL_ACTION_SetNULL( &wk->actionParam[0] );
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]);
  }
  return TRUE;
}

//---------------------------------------------------
// サーバコマンド処理
//---------------------------------------------------
static BOOL SubProc_AI_ServerCmd( BTL_CLIENT* wk, int* seq )
{
  // AIは何もしちゃダメ
  return TRUE;
}

static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq )
{
  static const struct {
    u32       cmd;
    ServerCmdProc proc;
  }scprocTbl[] = {
    { SC_MSG_STD,               scProc_MSG_Std            },
    { SC_MSG_STD_SE,            scProc_MSG_StdSE          },
    { SC_MSG_SET,               scProc_MSG_Set            },
    { SC_MSG_WAZA,              scProc_MSG_Waza           },
    { SC_ACT_WAZA_EFFECT,       scProc_ACT_WazaEffect     },
    { SC_ACT_WAZA_EFFECT_EX,    scProc_ACT_WazaEffectEx   },
    { SC_ACT_WAZA_DMG,          scProc_ACT_WazaDmg        },
    { SC_ACT_WAZA_DMG_PLURAL,   scProc_ACT_WazaDmg_Plural },
    { SC_ACT_WAZA_ICHIGEKI,     scProc_ACT_WazaIchigeki   },
    { SC_ACT_CONF_DMG,          scProc_ACT_ConfDamage     },
    { SC_ACT_DEAD,              scProc_ACT_Dead           },
    { SC_ACT_MEMBER_OUT,        scProc_ACT_MemberOut      },
    { SC_ACT_MEMBER_IN,         scProc_ACT_MemberIn       },
    { SC_ACT_RANKUP,            scProc_ACT_RankUp         },
    { SC_ACT_RANKDOWN,          scProc_ACT_RankDown       },
    { SC_ACT_WEATHER_DMG,       scProc_ACT_WeatherDmg     },
    { SC_ACT_WEATHER_START,     scProc_ACT_WeatherStart   },
    { SC_ACT_WEATHER_END,       scProc_ACT_WeatherEnd     },
    { SC_ACT_SIMPLE_HP,         scProc_ACT_SimpleHP       },
    { SC_ACT_TRACE_TOKUSEI,     scProc_ACT_TraceTokusei   },
    { SC_ACT_KINOMI,            scProc_ACT_Kinomi         },
    { SC_TOKWIN_IN,             scProc_TOKWIN_In          },
    { SC_TOKWIN_OUT,            scProc_TOKWIN_Out         },
    { SC_OP_HP_MINUS,           scProc_OP_HpMinus         },
    { SC_OP_HP_PLUS,            scProc_OP_HpPlus          },
    { SC_OP_HP_ZERO,            scProc_OP_HpZero          },
    { SC_OP_PP_MINUS,           scProc_OP_PPMinus         },
    { SC_OP_PP_PLUS,            scProc_OP_PPPlus          },
    { SC_OP_RANK_UP,            scProc_OP_RankUp          },
    { SC_OP_RANK_DOWN,          scProc_OP_RankDown        },
    { SC_OP_RANK_SET5,          scProc_OP_RankSet5        },
    { SC_OP_ADD_CRITICAL,       scProc_OP_AddCritical     },
    { SC_OP_SICK_SET,           scProc_OP_SickSet         },
    { SC_OP_CURE_POKESICK,      scProc_OP_CurePokeSick    },
    { SC_OP_CURE_WAZASICK,      scProc_OP_CureWazaSick    },
    { SC_OP_MEMBER_IN,          scProc_OP_MemberIn        },
    { SC_OP_CANTESCAPE_ADD,     scProc_OP_EscapeCodeAdd   },
    { SC_OP_CANTESCAPE_SUB,     scProc_OP_EscapeCodeSub   },
    { SC_OP_CHANGE_POKETYPE,    scProc_OP_ChangePokeType  },
    { SC_OP_CHANGE_POKEFORM,    scProc_OP_ChangePokeForm  },
    { SC_OP_WAZASICK_TURNCHECK, scProc_OP_WSTurnCheck     },
    { SC_OP_REMOVE_ITEM,        scProc_OP_RemoveItem      },
    { SC_OP_UPDATE_USE_WAZA,    scProc_OP_UpdateUseWaza   },
    { SC_OP_RESET_USED_WAZA,    scProc_OP_ResetUsedWaza   },
    { SC_OP_SET_CONTFLAG,       scProc_OP_SetContFlag     },
    { SC_OP_RESET_CONTFLAG,     scProc_OP_ResetContFlag   },
    { SC_OP_SET_ACTFLAG,        scProc_OP_SetActFlag      },
    { SC_OP_CLEAR_ACTFLAG,      scProc_OP_ClearActFlag    },
    { SC_OP_SET_TURNFLAG,       scProc_OP_SetTurnFlag     },
    { SC_OP_RESET_TURNFLAG,     scProc_OP_ResetTurnFlag   },
    { SC_OP_CHANGE_TOKUSEI,     scProc_OP_ChangeTokusei   },
    { SC_OP_SET_ITEM,           scProc_OP_SetItem         },
    { SC_OP_UPDATE_WAZANUMBER,  scProc_OP_UpdateWazaNumber},
    { SC_OP_HENSIN,             scProc_OP_Hensin          },
    { SC_OP_OUTCLEAR,           scProc_OP_OutClear        },
    { SC_OP_ADD_FLDEFF,         scProc_OP_AddFldEff       },
    { SC_OP_REMOVE_FLDEFF,      scProc_OP_RemoveFldEff    },
    { SC_OP_SET_POKE_COUNTER,   scProc_OP_SetPokeCounter  },
    { SC_OP_BATONTOUCH,         scProc_OP_BatonTouch      },
    { SC_OP_MIGAWARI_CREATE,    scProc_OP_MigawariCreate  },
    { SC_OP_MIGAWARI_DELETE,    scProc_OP_MigawariDelete  },
    { SC_OP_SHOOTER_CHARGE,     scProc_OP_ShooterCharge   },
    { SC_ACT_KILL,              scProc_ACT_Kill           },
    { SC_ACT_MOVE,              scProc_ACT_Move           },
    { SC_ACT_EXP,               scProc_ACT_Exp            },
    { SC_ACT_EXP_LVUP,          scProc_ACT_ExpLvup        },
    { SC_ACT_BALL_THROW,        scProc_ACT_BallThrow      },
    { SC_ACT_ROTATION,          scProc_ACT_Rotation       },
  };

restart:

  switch( *seq ){
  case 0:
    {
      u16 cmdSize;
      const void* cmdBuf;

      cmdSize = BTL_ADAPTER_GetRecvData( wk->adapter, &cmdBuf );
      SCQUE_Setup( wk->cmdQue, cmdBuf, cmdSize );

      if( wk->commWaitInfoOn )
      {
        wk->commWaitInfoOn = FALSE;
        BTLV_ResetCommWaitInfo( wk->viewCore );
      }
      (*seq)++;
    }
    /* fallthru */
  case 1:
    if( SCQUE_IsFinishRead(wk->cmdQue) )
    {
      BTL_Printf("サーバーコマンド読み終わりました\n");
      return TRUE;
    }
    (*seq)++;
    /* fallthru */
  case 2:
    {
      int i;

      wk->serverCmd = SCQUE_Read( wk->cmdQue, wk->cmdArgs );

      for(i=0; i<NELEMS(scprocTbl); i++)
      {
        if( scprocTbl[i].cmd == wk->serverCmd ){ break; }
      }

      if( i == NELEMS(scprocTbl) )
      {
        BTL_Printf("用意されていないコマンドNo[%d]！\n", wk->serverCmd);
        (*seq)=1;
        goto restart;
//        return TRUE;
      }

      BTL_Printf( "serverCmd=%d\n", wk->serverCmd );
      wk->scProc = scprocTbl[i].proc;
      wk->scSeq = 0;
      (*seq)++;
    }
    /* fallthru */
  case 3:
    if( wk->scProc(wk, &wk->scSeq, wk->cmdArgs) )
    {
      (*seq) = 1;
      goto restart;
    }
    break;
  }

  return FALSE;
}

static BOOL scProc_ACT_MemberOut( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlvMcssPos  vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[0] );
      BTLV_ACT_MemberOut_Start( wk->viewCore, vpos );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_ACT_MemberOut_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
static BOOL scProc_ACT_MemberIn( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 clientID = wk->cmdArgs[0];
      u8 posIdx = wk->cmdArgs[1];
      u8 memberIdx = wk->cmdArgs[2];
      BtlPokePos  pokePos = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );

      BTL_Printf("メンバーIN ACT client=%d, posIdx=%d, pos=%d, memberIdx=%d\n",
          clientID, posIdx, pokePos, memberIdx );

      BTLV_StartMemberChangeAct( wk->viewCore, pokePos, clientID, posIdx );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMemberChangeAct(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}

static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    BTLV_StartMsgStd( wk->viewCore, args[0], &args[1] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
static BOOL scProc_MSG_StdSE( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    BTLV_StartMsgStd( wk->viewCore, args[0], &args[2] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      PMSND_PlaySE( args[1] );
    }
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}


static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    BTLV_StartMsgSet( wk->viewCore, args[0], &args[1] );
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      // args[0] = pokeID, args[1] = wazaID
      BTLV_StartMsgWaza( wk->viewCore, args[0], args[1] );
    }
    (*seq)++;
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

static BOOL scProc_ACT_WazaEffect( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    WazaID waza;
    u8 atPokePos, defPokePos;
    const BTL_PARTY* party;
    const BTL_POKEPARAM* poke;

    atPokePos  = args[0];
    defPokePos = args[1];
    waza       = args[2];
    poke = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, atPokePos );
    BTLV_ACT_WazaEffect_Start( wk->viewCore, atPokePos, defPokePos, waza, BTLV_WAZAEFF_TURN_DEFAULT, 0 );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_WazaEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }

  return FALSE;
}
static BOOL scProc_ACT_WazaEffectEx( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    WazaID waza;
    u8 atPokePos, defPokePos, turnType;
    const BTL_PARTY* party;
    const BTL_POKEPARAM* poke;

    atPokePos  = args[0];
    defPokePos = args[1];
    waza       = args[2];
    turnType   = args[3];
    poke = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, atPokePos );
    BTLV_ACT_WazaEffect_Start( wk->viewCore, atPokePos, defPokePos, waza, turnType, 0 );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_WazaEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }

  return FALSE;
}




static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    WazaID waza;
    u8 atPokePos, defPokePos, affinity;
    u16 damage;
    const BTL_PARTY* party;
    const BTL_POKEPARAM* poke;

    defPokePos  = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
    affinity  = args[1];
    damage    = args[2];
    waza      = args[3];

    BTLV_ACT_DamageEffectSingle_Start( wk->viewCore, waza, defPokePos, damage, affinity );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_DamageEffectSingle_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * 【アクション】複数体一斉ダメージ処理
 */
static BOOL scProc_ACT_WazaDmg_Plural( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    u16               poke_cnt  = args[0];
    BtlTypeAffAbout   aff_about = args[1];
    WazaID            waza      = args[2];

    u8 pokeID[ BTL_POS_MAX ];
    u8 i;
    for(i=0; i<poke_cnt; ++i){
      pokeID[i] = SCQUE_READ_ArgOnly( wk->cmdQue );
    }
    BTLV_ACT_DamageEffectPlural_Start( wk->viewCore, poke_cnt, aff_about, pokeID, waza );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_DamageEffectPlural_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * 【アクション】一撃必殺ワザ処理
 */
static BOOL scProc_ACT_WazaIchigeki( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
    BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_StartDeadAct( wk->viewCore, pos );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_WaitDeadAct( wk->viewCore ) )
    {
      BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_Ichigeki, args );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * 【アクション】こんらん自爆ダメージ
 */
static BOOL scProc_ACT_ConfDamage( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * 【アクション】ポケモンひんし
 */
static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_StartDeadAct( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitDeadAct(wk->viewCore) )
    {
      BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
      BPP_Clear_ForDead( pp );
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * 【アクション】能力ランクダウン
 */
static BOOL scProc_ACT_RankDown( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTLV_StartRankDownEffect( wk->viewCore, args[0], args[1] );
  return TRUE;
}
/**
 * 【アクション】能力ランクアップ
 */
static BOOL scProc_ACT_RankUp( BTL_CLIENT* wk, int* seq, const int* args )
{
  // @@@ まだです
//  BTLV_StartRankDownEffect( wk->viewCore, args[0], args[1] );
  return TRUE;
}
/**
 *  【アクション】状態異常にさせられた時の処理
 */
static BOOL scProc_ACT_SickSet( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 pokeID = args[0];
      WazaSick sick = args[1];
      u16 msgID;

      switch( sick ){
      case WAZASICK_DOKU:   msgID = BTL_STRID_SET_DokuGet; break;
      case WAZASICK_YAKEDO: msgID = BTL_STRID_SET_YakedoGet; break;
      case WAZASICK_MAHI:   msgID = BTL_STRID_SET_MahiGet; break;
      case WAZASICK_KOORI:  msgID = BTL_STRID_SET_KoriGet; break;
      case WAZASICK_NEMURI: msgID = BTL_STRID_SET_NemuriGet; break;
      case WAZASICK_KONRAN: msgID = BTL_STRID_SET_KonranGet; break;
      default:
        GF_ASSERT(0);
        return TRUE;
      }
      BTLV_StartMsgSet( wk->viewCore, msgID, args );  // この先ではargs[0]しか参照しないハズ…
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 *  【アクション】天候による一斉ダメージ処理
 */
static BOOL scProc_ACT_WeatherDmg( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlWeather weather = args[0];
      u8 pokeCnt = args[1];
      u16 msgID;
      u8 pokeID, pokePos, i;

      switch( weather ){
      case BTL_WEATHER_SAND:
        BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_SandAttack, NULL );
        break;
      case BTL_WEATHER_SNOW:
        BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_SnowAttack, NULL );
        break;
      default:
        break;
      }

      for(i=0; i<pokeCnt; ++i)
      {
        pokeID = SCQUE_READ_ArgOnly( wk->cmdQue );
        pokePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
        BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pokePos );
      }
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore)
    &&  BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore)
    ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  天候変化開始
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_WeatherStart( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlWeather weather = args[0];
      u16  msgID;
      switch( weather ){
      case BTL_WEATHER_SHINE: msgID = BTL_STRID_STD_ShineStart; break;
      case BTL_WEATHER_RAIN:  msgID = BTL_STRID_STD_RainStart; break;
      case BTL_WEATHER_SAND:  msgID = BTL_STRID_STD_StormStart; break;
      case BTL_WEATHER_SNOW:  msgID = BTL_STRID_STD_SnowStart; break;
      case BTL_WEATHER_MIST:  msgID = BTL_STRID_STD_MistStart; break;
      default:
        GF_ASSERT(0); // おかしな天候ID
        return TRUE;
      }
      BTLV_StartMsgStd( wk->viewCore, msgID, NULL );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  ターンチェックによる天候の終了処理
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_WeatherEnd( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlWeather weather = args[0];
      u16  msgID;
      switch( weather ){
      case BTL_WEATHER_SHINE: msgID = BTL_STRID_STD_ShineEnd; break;
      case BTL_WEATHER_RAIN:  msgID = BTL_STRID_STD_RainEnd; break;
      case BTL_WEATHER_SAND:  msgID = BTL_STRID_STD_StormEnd; break;
      case BTL_WEATHER_SNOW:  msgID = BTL_STRID_STD_SnowEnd; break;
      case BTL_WEATHER_MIST:  msgID = BTL_STRID_STD_MistEnd; break;
      default:
        GF_ASSERT(0); // おかしな天候ID
        return TRUE;
      }
      BTLV_StartMsgStd( wk->viewCore, msgID, NULL );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  シンプルなHPゲージ増減処理
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_SimpleHP( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  とくせい「トレース」の発動処理
 *  args .. [0]:トレース持ちのポケID  [1]:コピー対象のポケID  [2]:コピーするとくせい
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Kinomi( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  switch( *seq ){
  case 0:
    {
      BTLV_KinomiAct_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_KinomiAct_Wait( wk->viewCore, pos ) )
    {
      return TRUE;
    }
    break;
  }

  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  強制ひんし演出
 *  args .. [0]:対象ポケID  [1]:エフェクトタイプ
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Kill( BTL_CLIENT* wk, int* seq, const int* args )
{
  // @@@ 今は単純なHP減エフェクトで代用
  u8 pokeID = args[0];
  u8 effType = args[1];

  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  ムーブ処理
 *  args .. [0]:対象クライアントID  [1]:対象ポケモンIndex
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Move( BTL_CLIENT* wk, int* seq, const int* args )
{

  switch( *seq ){
  case 0:
    {
      u8 clientID = args[0];
      u8 posIdx = args[1];
      u8 pos1, pos2, vpos1, vpos2;

      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
      BTL_PARTY_SwapMembers( party, posIdx, 1 );

      pos1 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );
      pos2 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, 1 );
      vpos1 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos1 );
      vpos2 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos2 );

      BTL_Printf("ムーブ: %d <-> %d\n", pos1, pos2 );

      BTLV_ACT_MoveMember_Start( wk->viewCore, clientID, vpos1, vpos2, posIdx, 1 );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_ACT_MoveMember_Wait(wk->viewCore) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  経験値加算処理
 *  args .. [0]:対象ポケモンID  [1]:経験値加算分
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Exp( BTL_CLIENT* wk, int* seq, const int* args )
{

  switch( *seq ){
  case 0:
    {
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
      BPP_ReflectExpAdd( bpp );

      if( BTL_MAIN_CheckFrontPoke(wk->mainModule, wk->pokeCon, args[0]) )
      {
        u8 pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
        u8 vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );

        BTLV_EFFECT_CalcGaugeEXP( vpos, args[1] );
        (*seq)++;
      }
      else{
        return TRUE;
      }
    }
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecuteGauge() ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  経験値加算処理（レベルアップをともなう）
 *  args .. [0]:対象ポケモンID  [1]:level, [2]:hp, [3]:atk, [4]:def, [5]:sp_atk, [6]:sp_def, [7]:agi
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_ExpLvup( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
      BTL_Printf(" GetExp with Levelup ... level=%d ->", BPP_GetValue(bpp, BPP_LEVEL) );
      BPP_ReflectLevelup( bpp, args[1], args[2], args[2], args[4], args[5], args[6], args[7] );
      BTL_PrintfSimple(" %d\n", BPP_GetValue(bpp, BPP_LEVEL));
      BPP_HpPlus( bpp, args[2] );

      if( BTL_MAIN_CheckFrontPoke(wk->mainModule, wk->pokeCon, args[0]) )
      {
        u8 pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
        u8 vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );

        BTLV_EFFECT_CalcGaugeEXPLevelUp( vpos, bpp );
        (*seq)++;
      }
      else{
        (*seq)+=2;
      }
    }
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecuteGauge() )
    {
      (*seq)++;
    }
    break;
  case 2:
      {
        BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );

        wk->strParam.args[0] = BPP_GetID( bpp );
        wk->strParam.args[1] = args[1];

        BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_LevelUp, wk->strParam.args );
        (*seq)++;
      }
      break;
  case 3:
    if( BTLV_IsJustDoneMsg(wk->viewCore) ){
      PMSND_PlayBGM( WAVE_ME_WB_LVUP );
    }
    if( BTLV_WaitMsg(wk->viewCore) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  モンスターボール投げつけ
 *  args .. [0]:対象ポケ位置  [1]:ゆれ回数  [2]:捕獲成功フラグ [3]:ボールのアイテムナンバー
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_BallThrow( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, args[0] );
      BTLV_EFFECT_BallThrow( vpos, args[3], args[1], args[2] );
      (*seq)++;
    }
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      u16 strID;

      // 捕獲成功メッセージ
      if( args[2] )
      {
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, args[0] );
        wk->strParam.args[0] = BPP_GetID( bpp );
        strID = BTL_STRID_STD_BallThrowS;
      }
      // 捕獲失敗メッセージ
      else
      {
        static const u16 strTbl[] = {
          BTL_STRID_STD_BallThrow0, BTL_STRID_STD_BallThrow1, BTL_STRID_STD_BallThrow2, BTL_STRID_STD_BallThrow3,
        };

        if( args[1] < NELEMS(strTbl) ){
          strID = strTbl[ args[1] ];
        }else{
          strID = strTbl[ 0 ];
        }
      }
      BTLV_StartMsgStd( wk->viewCore, strID, wk->strParam.args );
      (*seq)++;
    }
    break;
  case 2:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      if( args[2] ){
        PMSND_PlayBGM( SEQ_ME_POKEGET );
        (*seq)++;
      }else{
        (*seq) += 2;
      }
    }
    break;
  case 3:
    if( !PMSND_CheckPlayBGM() ){
      (*seq)++;
    }
    break;
  default:
      return TRUE;
  }
  return FALSE;
}

//---------------------------------------------------------------------------------------
/**
 *  ローテーション動作
 *  args .. [0]:対象クライアントID  [1]:ローテーション方向 (BtlRotateDir)
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_Rotation( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      u8 clientID = args[0];
      BtlRotateDir dir = args[1];
      BTL_PARTY*  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );

      BTL_PARTY_RotateMembers( party, dir, NULL, NULL );
      BTLV_RotationMember_Start( wk->viewCore, clientID, dir );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_RotationMember_Wait(wk->viewCore) )
    {
      BtlRotateDir dir = args[1];
      if( dir == BTL_ROTATEDIR_STAY ){
        return TRUE;
      }
      else
      {
        u8  clientID = args[0];
        u8  fNPC = (BTL_MAIN_GetClientTrainerID(wk->mainModule, clientID) != TRID_NULL);
        u16 strID;

        if( dir == BTL_ROTATEDIR_L ){
          strID = (fNPC)? BTL_STRID_STD_RotateL_NPC : BTL_STRID_STD_RotateL_Player;
        }else{
          strID = (fNPC)? BTL_STRID_STD_RotateR_NPC : BTL_STRID_STD_RotateR_Player;
        }

        BTLV_StartMsgStd( wk->viewCore, strID, args );
        (*seq)++;
      }
    }
    break;

  case 2:
    if( BTLV_WaitMsg(wk->viewCore) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  とくせい「トレース」の発動処理
 *  args .. [0]:トレース持ちのポケID  [1]:コピー対象のポケID  [2]:コピーするとくせい
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_ACT_TraceTokusei( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  switch( *seq ){
  case 0:
    {
      BTLV_StartTokWin( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_StartTokWinWait( wk->viewCore, pos ) )
    {
      BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      BPP_ChangeTokusei( bpp, args[2] );
      BTL_Printf("トレースでとくせい変更->%d\n", args[2]);
      BTLV_TokWin_Renew_Start( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 2:
    if( BTLV_TokWin_Renew_Wait( wk->viewCore, pos ) )
    {
      BTLV_StartMsgSet( wk->viewCore, BTL_STRID_SET_Trace, args );
      (*seq)++;
    }
    break;
  case 3:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      BTLV_QuitTokWin( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 4:
    if( BTLV_QuitTokWinWait(wk->viewCore, pos) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------
/**
 *  とくせいウィンドウ表示オン
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_TOKWIN_In( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
  BTLV_StartTokWin( wk->viewCore, pos );
  return TRUE;
}
//---------------------------------------------------------------------------------------
/**
 *  とくせいウィンドウ表示オフ
 */
//---------------------------------------------------------------------------------------
static BOOL scProc_TOKWIN_Out( BTL_CLIENT* wk, int* seq, const int* args )
{
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, args[0] );
  BTLV_QuitTokWin( wk->viewCore, pos );
  return TRUE;
}

static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_HpMinus( pp, args[1] );
  return TRUE;
}


static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_HpPlus( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  u8 wazaIdx = args[1];
  u8 value = args[2];
  u8 pokePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  BTL_POKEPARAM* pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, pokePos );
  BPP_PPMinus( pp, wazaIdx, value );
  BTL_Printf("ポケモンのPP 減らします pokeID=%d, waza=%d, val=%d\n", pokeID, wazaIdx, value);

  return TRUE;
}
static BOOL scProc_OP_HpZero( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_HpZero( pp );
  return TRUE;
}
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, args[0] );
  BPP_PPPlus( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankUp( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankUp( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankDown( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankSet5( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RankSet( pp, BPP_ATTACK,      args[1] );
  BPP_RankSet( pp, BPP_DEFENCE,     args[2] );
  BPP_RankSet( pp, BPP_SP_ATTACK,   args[3] );
  BPP_RankSet( pp, BPP_SP_DEFENCE,  args[4] );
  BPP_RankSet( pp, BPP_AGILITY,     args[5] );
  return TRUE;
}
static BOOL scProc_OP_AddCritical( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_AddCriticalRank( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SickSet( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SICK_CONT cont;
  cont.raw = args[2];
  BPP_SetWazaSick( pp, args[1], cont );
  return TRUE;
}
static BOOL scProc_OP_CurePokeSick( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CurePokeSick( pp );
  return TRUE;
}
static BOOL scProc_OP_CureWazaSick( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CureWazaSick( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_MemberIn( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 clientID = wk->cmdArgs[0];
  u8 posIdx = wk->cmdArgs[1];
  u8 memberIdx = wk->cmdArgs[2];

  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );

    BTL_POKEPARAM* bpp;
    if( posIdx != memberIdx ){
      BTL_Printf("クライアント側：%d <-> %d ポケ入れ替え\n", posIdx, memberIdx);
      BTL_PARTY_SwapMembers( party, posIdx, memberIdx );
    }
    BTL_Printf("メンバーイン 位置 %d <- %d にいたポケ\n", posIdx, memberIdx);
    bpp = BTL_PARTY_GetMemberData( party, posIdx );
    BPP_SetAppearTurn( bpp, args[3] );
    BPP_Clear_ForIn( bpp );
  }
  return TRUE;
}
static BOOL scProc_OP_EscapeCodeAdd( BTL_CLIENT* wk, int* seq, const int* args )
{
  cec_addCode( &wk->cantEscCtrl, args[0], args[1] );
  return TRUE;
}
static BOOL scProc_OP_EscapeCodeSub( BTL_CLIENT* wk, int* seq, const int* args )
{
  cec_subCode( &wk->cantEscCtrl, args[0], args[1] );
  return TRUE;
}
static BOOL scProc_OP_ChangePokeType( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ChangePokeType( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ChangePokeForm( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ChangeForm( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_WSTurnCheck( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_WazaSick_TurnCheck( bpp, NULL, NULL );
  return TRUE;
}
static BOOL scProc_OP_RemoveItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_RemoveItem( pp );
  return TRUE;
}
static BOOL scProc_OP_UpdateUseWaza( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_UpdatePrevWazaID( pp, args[2], args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetUsedWaza( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ResetWazaContConter( pp );
  return TRUE;
}
static BOOL scProc_OP_SetContFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CONTFLAG_Set( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetContFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_CONTFLAG_Clear( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_TURNFLAG_Set( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_TURNFLAG_ForceOff( pp, args[1] );
  return TRUE;
}

static BOOL scProc_OP_SetActFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ACTFLAG_Set( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ClearActFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ACTFLAG_Clear( pp );
  return TRUE;
}
static BOOL scProc_OP_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_ChangeTokusei( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SetItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SetItem( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_UpdateWazaNumber( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  // サーバコマンド送信時の都合で引数がヘンな並びになってる…
  BPP_WAZA_UpdateID( pp, args[1], args[4], args[2], args[3] );
  return TRUE;
}
static BOOL scProc_OP_Hensin( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* atkPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM* tgtPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, args[1] );

  BPP_HENSIN_Set( atkPoke, tgtPoke );
  return TRUE;
}
static BOOL scProc_OP_OutClear( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_Clear_ForOut( pp );
  return TRUE;
}
static BOOL scProc_OP_AddFldEff( BTL_CLIENT* wk, int* seq, const int* args )
{
  // @@@ サーバマシンなら既に天候を操作しているハズなので行わない
  //     ただしこの措置は全マシンにサーバ計算機能が乗るようになったらハズすと思う
  //     （というかこのコマンド自体、要らなくなるハズ）-> でもサーババージョン違ったら問題あるか
  if( !BTL_MAIN_IsServerMachine(wk->mainModule) ){
    BPP_SICK_CONT  cont;
    cont.raw = args[1];
    BTL_FIELD_AddEffect( args[0], cont );
  }
  return TRUE;
}
static BOOL scProc_OP_RemoveFldEff( BTL_CLIENT* wk, int* seq, const int* args )
{
  // @@@ サーバマシンなら既に天候を操作しているハズなので行わない
  //     ただしこの措置は全マシンにサーバ計算機能が乗るようになったらハズすと思う
  //     （というかこのコマンド自体、要らなくなるハズ）-> でもサーババージョン違ったら問題あるか
  if( !BTL_MAIN_IsServerMachine(wk->mainModule) ){
    BTL_FIELD_RemoveEffect( args[0] );
  }
  return TRUE;
}
static BOOL scProc_OP_SetPokeCounter( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_COUNTER_Set( bpp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_BatonTouch( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, args[1] );
  BPP_BatonTouchParam( target, user );
  return TRUE;
}
static BOOL scProc_OP_MigawariCreate( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_MIGAWARI_Create( bpp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_MigawariDelete( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_MIGAWARI_Delete( bpp );
  return TRUE;
}
static BOOL scProc_OP_ShooterCharge( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 clientID = args[0];
  u8 increment = args[1];

  if( clientID == wk->myID )
  {
    wk->shooterEnergy += increment;
    if( wk->shooterEnergy > BTL_SHOOTER_ENERGY_MAX ){
      wk->shooterEnergy = BTL_SHOOTER_ENERGY_MAX;
    }
  }
  return TRUE;
}




//------------------------------------------------------------------------------------------------------
// 逃げ交換禁止管理ワーク
//------------------------------------------------------------------------------------------------------

static void cec_addCode( CANT_ESC_CONTROL* ctrl, u8 pokeID, BtlCantEscapeCode code )
{
  GF_ASSERT(code < BTL_CANTESC_MAX);
  GF_ASSERT(pokeID < BTL_POKEID_MAX);

  if( ctrl->counter[ code ][ pokeID ] < CANTESC_COUNT_MAX )
  {
    ctrl->counter[ code ][ pokeID ]++;
  }
  else{
    GF_ASSERT(0);
  }
}
static void cec_subCode( CANT_ESC_CONTROL* ctrl, u8 pokeID, BtlCantEscapeCode code )
{
  GF_ASSERT(code < BTL_CANTESC_MAX);
  GF_ASSERT(pokeID < BTL_POKEID_MAX);

  if( ctrl->counter[ code ][ pokeID ] )
  {
    ctrl->counter[ code ][ pokeID ]--;
  }
  else{
    GF_ASSERT(0);
  }
}

static u8 cec_isEnable( CANT_ESC_CONTROL* ctrl, BtlCantEscapeCode code, BTL_CLIENT* wk )
{
  GF_ASSERT(code < BTL_CANTESC_MAX);
  {
    int i;
    for(i=0; i<BTL_POKEID_MAX; ++i)
    {
      if( ctrl->counter[code][i] )
      {
        u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( i );
        if( !BTL_MAIN_IsOpponentClientID(wk->mainModule, wk->myID, clientID) ){ continue; }
        switch( code ){
        case BTL_CANTESC_KAGEFUMI:  if( _cec_check_kagefumi(wk) ){ return i; }
        case BTL_CANTESC_ARIJIGOKU: if( _cec_check_arijigoku(wk) ){ return i; }
        case BTL_CANTESC_JIRYOKU:   if( _cec_check_jiryoku(wk) ){ return i; }
        }
      }
    }
  }
  return BTL_POKEID_NULL;
}
// とくせい「かげふみ」が自分に効くかチェック
static BOOL _cec_check_kagefumi( BTL_CLIENT* wk )
{
  return TRUE;
}
// とくせい「ありじごく」が自分に効くかチェック
static BOOL _cec_check_arijigoku( BTL_CLIENT* wk )
{
  if( BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) )
  {
    return TRUE;
  }
  else if (
      (countFrontPokeTokusei(wk, POKETOKUSEI_FUYUU) == 0)
  &&  (countFrontPokeType(wk, POKETYPE_HIKOU) == 0)
  ){
    return TRUE;
  }
  return FALSE;
}
// とくせい「じりょく」が自分に効くかチェック
static BOOL _cec_check_jiryoku( BTL_CLIENT* wk )
{
  return countFrontPokeType( wk, POKETYPE_HAGANE );
}

//------------------------------------------------------------------------------------------------------
// 戦闘中ポケモンの所有とくせい・タイプ等チェック
//------------------------------------------------------------------------------------------------------
static u8 countFrontPokeTokusei( BTL_CLIENT* wk, PokeTokusei tokusei )
{
  const BTL_POKEPARAM* bpp;
  int i, cnt;
  for(i=0, cnt=0; i<wk->numCoverPos; ++i){
    bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
    if( !BPP_IsDead(bpp) )
    {
      if( BPP_GetValue(bpp, BPP_TOKUSEI) == tokusei ){ ++cnt; }
    }
  }
  return cnt;
}
static u8 countFrontPokeType( BTL_CLIENT* wk, PokeType type )
{
  const BTL_POKEPARAM* bpp;
  int i, cnt;
  for(i=0, cnt=0; i<wk->numCoverPos; ++i){
    bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
    if( !BPP_IsDead(bpp) )
    {
      if( BPP_IsMatchType(bpp, type) ){ ++cnt; }
    }
  }
  return cnt;
}


//------------------------------------------------------------------------------------------------------
// 外部モジュールからの情報取得処理
//------------------------------------------------------------------------------------------------------
u8 BTL_CLIENT_GetClientID( const BTL_CLIENT* client )
{
  return client->myID;
}

const BTL_PARTY* BTL_CLIENT_GetParty( const BTL_CLIENT* client )
{
  return client->myParty;
}

//=============================================================================================
/**
 * 現在、行動選択処理中のポケモン位置IDを取得
 *
 * @param   client
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_CLIENT_GetProcPokePos( const BTL_CLIENT* client )
{
  return client->basePos + client->procPokeIdx*2;
}





