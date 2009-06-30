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

#include "poke_tool\pokeparty.h"

#include "battle\battle.h"
#include "btl_common.h"
#include "btl_adapter.h"
#include "btl_action.h"
#include "btl_field.h"
#include "btl_pokeselect.h"
#include "btl_server_cmd.h"
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

  ClientSubProc  subProc;
  int            subSeq;

  const void*    returnDataPtr;
  u32            returnDataSize;

  const BTL_PARTY*  myParty;
  u8                frontPokeEmpty[ BTL_POSIDX_MAX ];       ///< 担当している戦闘位置にもう出せない時にTRUEにする
  u8                numCoverPos;  ///< 担当する戦闘ポケモン数
  u8                procPokeIdx;  ///< 処理中ポケモンインデックス
  BtlPokePos        basePos;      ///< 戦闘ポケモンの位置ID

  BTL_ACTION_PARAM     actionParam[ BTL_POSIDX_MAX ];
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



};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static ClientSubProc getSubProc( BTL_CLIENT* wk, BtlAdapterCmd cmd );
static BOOL SubProc_UI_NotifyPokeData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_NotifyPokeData( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_Initialize( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_Initialize( BTL_CLIENT* wk, int *seq );
static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq );
static BOOL is_action_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
static BOOL is_waza_unselectable( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* action );
static void setWaruagakiAction( BTL_ACTION_PARAM* dst, BTL_CLIENT* wk, const BTL_POKEPARAM* bpp );
static BOOL is_unselectable_waza( BTL_CLIENT* wk, const BTL_POKEPARAM* bpp, WazaID waza, STR_PARAM* strParam );
static BtlCantEscapeCode is_prohibit_escape( BTL_CLIENT* wk, u8* pokeID );
static BOOL SubProc_AI_SelectAction( BTL_CLIENT* wk, int* seq );
static u8 calc_empty_pos( BTL_CLIENT* wk, u8* posIdxList );
static void abandon_cover_pos( BTL_CLIENT* wk, u8 numPos );
static u8 calc_puttable_pokemons( BTL_CLIENT* wk, u8* list );
static u8 calc_front_dead_pokemons( BTL_CLIENT* wk, u8* list );
static void setup_pokesel_param_change( BTL_CLIENT* wk, BTL_POKESELECT_PARAM* param );
static void setup_pokesel_param_dead( BTL_CLIENT* wk, u8 numSelect, BTL_POKESELECT_PARAM* param );
static void store_pokesel_to_action( BTL_CLIENT* wk, const BTL_POKESELECT_RESULT* res );
static BOOL SubProc_UI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_SelectPokemon( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_AI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL SubProc_UI_ServerCmd( BTL_CLIENT* wk, int* seq );
static BOOL scProc_ACT_MemberOut( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_MemberIn( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Std( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Set( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_MSG_Waza( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaEffect( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaEffectEx( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg_Dbl( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaDmg_Plural( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WazaIchigeki( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_ConfDamage( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Dead( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankDown( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_RankUp( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SickSet( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SickDamage( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherDmg( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherStart( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_WeatherEnd( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_SimpleHP( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Kinomi( BTL_CLIENT* wk, int* seq, const int* args );
static BOOL scProc_ACT_Kill( BTL_CLIENT* wk, int* seq, const int* args );
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
  GFL_NETHANDLE* netHandle, u16 clientID, u16 numCoverPos, BtlThinkerType clientType, HEAPID heapID )
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
  for(i=0; i<NELEMS(wk->frontPokeEmpty); i++)
  {
    wk->frontPokeEmpty[i] = FALSE;
  }
  wk->cmdQue = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER_CMD_QUE) );

  wk->myState = 0;
  wk->commWaitInfoOn = FALSE;

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
    if( wk->subProc(wk, &wk->subSeq) )
    {
      BTL_ADAPTER_ReturnCmd( wk->adapter, wk->returnDataPtr, wk->returnDataSize );
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
    { BTL_ACMD_WAIT_INITIALIZE, { SubProc_UI_Initialize,    SubProc_AI_Initialize } },
    { BTL_ACMD_SELECT_ACTION,   { SubProc_UI_SelectAction,  SubProc_AI_SelectAction } },
    { BTL_ACMD_SELECT_POKEMON,  { SubProc_UI_SelectPokemon, SubProc_AI_SelectPokemon } },
    { BTL_ACMD_SERVER_CMD,      { SubProc_UI_ServerCmd,     SubProc_AI_ServerCmd } },

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
static BOOL SubProc_UI_Initialize( BTL_CLIENT* wk, int* seq )
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

static BOOL SubProc_AI_Initialize( BTL_CLIENT* wk, int *seq )
{

  return TRUE;
}

//--------------
static BOOL SubProc_UI_SelectAction( BTL_CLIENT* wk, int* seq )
{
  enum {
    SEQ_INIT = 0,
    SEQ_CHECK_UNSEL_ACTION,
    SEQ_SELECT_ACTION,
    SEQ_CHECK_ACTION,
    SEQ_SELECT_POKEMON,
    SEQ_CHECK_DONE,
    SEQ_CHECK_ESCAPE,
    SEQ_CHECK_FIGHT,
    SEQ_SELECT_WAZA_START,
    SEQ_SELECT_WAZA_WAIT,
    SEQ_CHECK_WAZA_TARGET,
    SEQ_SELECT_TARGET_START,
    SEQ_SELECT_TARGET_WAIT,
    SEQ_WAIT_MSG,
    SEQ_CANT_ESCAPE,
    SEQ_RETURN_START,
    SEQ_RETURN_COMM_WAIT,
    SEQ_WAIT_CAMERA_EFFECT,
  };

  switch( *seq ){
  case SEQ_INIT:
    setup_pokesel_param_change( wk, &wk->pokeSelParam );
    wk->procPokeIdx = 0;
    (*seq) = SEQ_CHECK_UNSEL_ACTION;
    /* fallthru */
  case SEQ_CHECK_UNSEL_ACTION:
    wk->procPoke = BTL_POKECON_GetClientPokeData( wk->pokeCon, wk->myID, wk->procPokeIdx );
    wk->procAction = &wk->actionParam[ wk->procPokeIdx ];
    if( is_action_unselectable(wk, wk->procPoke,  wk->procAction) ){
      BTL_Printf("アクション選択(%d体目）スキップします\n", wk->procPokeIdx );
      (*seq) = SEQ_CHECK_DONE;
      break;
    }
    (*seq) = SEQ_SELECT_ACTION;
    /* fallthru */
  case SEQ_SELECT_ACTION:
    BTL_Printf("アクション選択(%d体目=ID:%d）開始します\n", wk->procPokeIdx, BTL_POKEPARAM_GetID(wk->procPoke));
    BTLV_UI_SelectAction_Start( wk->viewCore, wk->procPoke, wk->procAction );
    (*seq) = SEQ_CHECK_ACTION;
    break;
  case SEQ_CHECK_ACTION:
    {
      BtlAction action = BTLV_UI_SelectAction_Wait( wk->viewCore );

      switch( action ){
      // 入れ替えポケモン選択の場合はまだアクションパラメータが不十分->ポケモン選択へ
      case BTL_ACTION_CHANGE:
        BTLV_StartPokeSelect( wk->viewCore, &wk->pokeSelParam, &wk->pokeSelResult );
        (*seq) = SEQ_SELECT_POKEMON;
        break;
      // 「にげる」を選んだら、反応の条件分岐へ
      case BTL_ACTION_ESCAPE:
        BTL_Printf("「にげる」を選びました\n");
        (*seq) = SEQ_CHECK_ESCAPE;
        break;
      // 「たたかう」を選んだら、各種条件分岐
      case BTL_ACTION_FIGHT:
        (*seq) = SEQ_CHECK_FIGHT;
        break;
      }
    }
    break;

  // 「たたかう」選択直後
  case SEQ_CHECK_FIGHT:
    if( is_waza_unselectable( wk, wk->procPoke, wk->procAction ) ){
      (*seq) = SEQ_CHECK_DONE;
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
      if( BTL_ACTION_GetAction(wk->procAction) == BTL_ACTION_NULL ){
        (*seq) = SEQ_SELECT_ACTION;
      }
      else{
        if( is_unselectable_waza(wk, wk->procPoke, wk->actionParam[wk->procPokeIdx].fight.waza, &wk->strParam) )
        {
          if( wk->strParam.stdFlag ){
            BTL_Printf(" STR_STD, ID=%d\n", wk->strParam.strID);
            BTLV_StartMsgStd( wk->viewCore, wk->strParam.strID, wk->strParam.args );
          }else{
            BTL_Printf(" STR_SET, ID=%d\n", wk->strParam.strID);
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
    if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE ){
      (*seq) = SEQ_CHECK_DONE;
    }else{
      (*seq) = SEQ_SELECT_TARGET_START;
    }
    break;

  case SEQ_SELECT_TARGET_START:
    BTLV_UI_SelectTarget_Start( wk->viewCore, wk->procPoke, wk->procAction );
    (*seq) = SEQ_SELECT_TARGET_WAIT;
    break;

  case SEQ_SELECT_TARGET_WAIT:
    if( BTLV_UI_SelectTarget_Wait(wk->viewCore) ){
      (*seq) = SEQ_CHECK_DONE;
    }
    break;

  case SEQ_WAIT_MSG:
    if( BTLV_WaitMsg(wk->viewCore) ){
      BTLV_UI_Cleanup( wk->viewCore );
      (*seq) = SEQ_SELECT_ACTION;
    }
    break;

  // 入れ替えポケモン選択後
  case SEQ_SELECT_POKEMON:
    if( BTLV_WaitPokeSelect(wk->viewCore) )
    {
      u8 idx = BTL_POKESELECT_RESULT_GetLast( &wk->pokeSelResult );
      BTL_ACTION_SetChangeParam( &wk->actionParam[wk->procPokeIdx], wk->procPokeIdx, idx );
      BTL_POKESELECT_PARAM_SetProhibit( &wk->pokeSelParam, BTL_POKESEL_CANT_SELECTED, idx );
      (*seq) =  SEQ_CHECK_DONE;
    }
    break;

  case SEQ_CHECK_DONE:
    BTLV_UI_Cleanup( wk->viewCore );
    wk->procPokeIdx++;
    if( wk->procPokeIdx >= wk->numCoverPos )
    {
      wk->returnDataPtr = &(wk->actionParam[0]);
      wk->returnDataSize = sizeof(wk->actionParam[0]) * wk->numCoverPos;
      (*seq) = SEQ_RETURN_START;
    }
    else{
      (*seq) = SEQ_CHECK_UNSEL_ACTION;
    }
    break;

  case SEQ_CHECK_ESCAPE:
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
            BTL_Printf("逃げコマンドのハズ=%d\n", wk->procAction->gen.cmd);
            (*seq) = SEQ_RETURN_START;
          }
          else
          {
            int args[2];
            args[0] = pokeID;
            args[1] = code;
            BTLV_StartMsgSet( wk->viewCore, BTL_STRID_SET_CantEscTok, args );
            (*seq) = SEQ_CANT_ESCAPE;
          }
        }
        break;

      case BTL_ESCAPE_MODE_NG:
        BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_EscapeCant, NULL );
        (*seq) = SEQ_CANT_ESCAPE;
        break;
      }
    }
    break;

  case SEQ_CANT_ESCAPE:
    if( BTLV_WaitMsg(wk->viewCore) )
    {
      (*seq) = SEQ_SELECT_ACTION;
    }
    break;

  case SEQ_RETURN_START:
    BTLV_EFFECT_Stop();
    BTLV_EFFECT_Add( BTLEFF_CAMERA_INIT );
    if( BTL_MAIN_GetCommMode(wk->mainModule) != BTL_COMM_NONE )
    {
      BTL_Printf("通信中なので待機中メッセージだすよ\n");
      wk->commWaitInfoOn = TRUE;
      BTLV_StartCommWait( wk->viewCore );
      (*seq) = SEQ_RETURN_COMM_WAIT;
    }
    else
    {
      BTL_Printf("通信中じゃないなので待機中メッセージだしません\n");
      (*seq) = SEQ_WAIT_CAMERA_EFFECT;
    }
    break;

  case SEQ_RETURN_COMM_WAIT:
    if( BTLV_WaitCommWait(wk->viewCore) )
    {
      (*seq) = SEQ_WAIT_CAMERA_EFFECT;
    }
    break;

  case SEQ_WAIT_CAMERA_EFFECT:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      return TRUE;
    }
    break;
  }

  return FALSE;
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
  if( BTL_POKEPARAM_IsDead(wk->procPoke) )
  {
    BTL_ACTION_SetNULL( action );
    return TRUE;
  }
  // アクション選択できない（攻撃の反動など）場合はスキップ
  if( BTL_POKEPARAM_GetActFlag(wk->procPoke, BPP_ACTFLG_CANT_ACTION) )
  {
    BTL_ACTION_SetSkip( action );
    return TRUE;
  }
  // ワザロック状態（前回ワザをそのまま使う）は勝手にワザ選択処理してスキップ
  if( BTL_POKEPARAM_CheckSick(wk->procPoke, WAZASICK_WAZALOCK_HARD) )
  {
    WazaID waza = BTL_POKEPARAM_GetPrevWazaNumber( wk->procPoke );
    BtlPokePos pos = BTL_POKEPARAM_GetPrevTargetPos( wk->procPoke );
    u8 waza_idx = BTL_POKEPARAM_GetWazaIdx( wk->procPoke, waza );
    if( BTL_POKEPARAM_GetPP(wk->procPoke, waza_idx) ){
      BTL_ACTION_SetFightParam( action, waza, pos );
    }else{
      // PPゼロならわるあがき
      setWaruagakiAction( action, wk, wk->procPoke );
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
  if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_TYOUHATSU) )
  {
    int i, cnt, max = BTL_POKEPARAM_GetWazaCount( bpp );
    for(i=0; i<max; ++i){
      if( BTL_POKEPARAM_GetPP(bpp, i) && WAZADATA_IsDamage(BTL_POKEPARAM_GetWazaNumber(bpp, i)) ){
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
  BtlPokePos myPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, BTL_POKEPARAM_GetID(bpp) );
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
  if( BTL_POKEPARAM_GetContFlag(bpp, BPP_CONTFLG_KODAWARI_LOCK) )
  {
    if( waza != BTL_POKEPARAM_GetPrevWazaNumber(bpp) ){
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_STD_KodawariLock;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BTL_POKEPARAM_GetID( bpp );
        strParam->args[1] = BTL_POKEPARAM_GetItem( bpp );
        strParam->args[2] = waza;
      }
      return TRUE;
    }
  }

  // ワザロック効果（前回と同じワザしか出せない）
  if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_WAZALOCK) )
  {
    if( waza != BTL_POKEPARAM_GetPrevWazaNumber(bpp) ){
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_STD_WazaLock;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BTL_POKEPARAM_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

  // ちょうはつ状態（ダメージワザしか選べない）
  if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_TYOUHATSU) )
  {
    if( !WAZADATA_IsDamage(waza) )
    {
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_SET_ChouhatuWarn;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BTL_POKEPARAM_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

  // いちゃもん状態（２ターン続けて同じワザを選べない）
  if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_ICHAMON) )
  {
    if( BTL_POKEPARAM_GetPrevWazaNumber(bpp) == waza )
    {
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_SET_IchamonWarn;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BTL_POKEPARAM_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }

  // かなしばり状態（かなしばり直前に出していたワザを選べない）
  if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KANASIBARI) )
  {
    u8 idx = BTL_POKEPARAM_GetSickParam( bpp, WAZASICK_KANASIBARI );
    if( BTL_POKEPARAM_GetWazaIdx(bpp, waza) == idx )
    {
      if( strParam != NULL )
      {
        strParam->strID = BTL_STRID_SET_KanasibariWarn;
        strParam->stdFlag = FALSE;
        strParam->args[0] = BTL_POKEPARAM_GetID( bpp );
        strParam->args[1] = waza;
      }
      return TRUE;
    }
  }


  // @@@ ふういんとかの処理
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
  u8 i;

  for(i=0; i<wk->numCoverPos; ++i)
  {
    pp = BTL_CLIENT_GetFrontPokeData( wk, i );
    if( !BTL_POKEPARAM_IsDead(pp) )
    {
      u8 wazaCount, wazaIdx, mypos, targetPos;

      if( BTL_POKEPARAM_CheckSick(pp, WAZASICK_WAZALOCK)
      ||  BTL_POKEPARAM_CheckSick(pp, WAZASICK_WAZALOCK_HARD)
      ){
        WazaID waza = BTL_POKEPARAM_GetPrevWazaNumber( pp );
        BtlPokePos pos = BTL_POKEPARAM_GetPrevTargetPos( pp );
        BTL_ACTION_SetFightParam( &wk->actionParam[i], waza, pos );
        continue;
      }

      wazaCount = BTL_POKEPARAM_GetWazaCount( pp );
      {
        u8 usableWazaIdx[ PTL_WAZA_MAX ];
        WazaID waza;
        u8 j, cnt=0;
        for(j=0, cnt=0; j<wazaCount; ++j){
          if( BTL_POKEPARAM_GetPP(pp, j) )
          {
            waza = BTL_POKEPARAM_GetWazaNumber( pp, j );
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
        mypos = BTL_MAIN_GetClientPokePos( wk->mainModule, wk->myID, i );
        aliveCnt = 0;
        for(j=0; j<CHECK_MAX; ++j)
        {
          p = BTL_MAIN_GetOpponentPokePos( wk->mainModule, mypos, j );
          targetPoke = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, p );
          if( !BTL_POKEPARAM_IsDead(targetPoke) )
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
        WazaID waza = BTL_POKEPARAM_GetWazaNumber( pp, wazaIdx );
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
// 自分のポケモンが死んでいたら次を選択する処理
//---------------------------------------------------

// 瀕死になって、次のポケモンを出さなければならない担当位置数を返す
static u8 calc_empty_pos( BTL_CLIENT* wk, u8* posIdxList )
{
  u8 cnt, i;
  for(i=0, cnt=0; i<wk->numCoverPos; ++i)
  {
    if( wk->frontPokeEmpty[i] == FALSE )
    {
      const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( wk->myParty, i );
      if( BTL_POKEPARAM_IsDead(bpp) )
      {
        if( posIdxList )
        {
          posIdxList[cnt] = i;
        }
        ++cnt;
      }
    }
  }
  return cnt;
}

// 担当位置を放棄する  numPos : 放棄する位置の数
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
 * 今、場に出ているポケモン以外で、生きているポケモンの数を返す
 *
 * @param   wk
 * @param   list    [out] 場に出ている以外で生きているポケモンのパーティ内インデックスを格納する配列
 *
 * @retval  u8    場に出ている以外で生きているポケモンの数
 */
//--------------------------------------------------------------------------
static u8 calc_puttable_pokemons( BTL_CLIENT* wk, u8* list )
{
  u8 cnt, numMembers, i, j;

  numMembers = BTL_PARTY_GetMemberCount( wk->myParty );
  for(i=wk->numCoverPos, cnt=0; i<numMembers; i++)
  {
    {
      const BTL_POKEPARAM* pp = BTL_PARTY_GetMemberDataConst(wk->myParty, i);
      if( !BTL_POKEPARAM_IsDead(pp) )
      {
        if( list )
        {
          list[cnt] = i;
        }
        cnt++;
      }
    }
  }
  return cnt;
}
//--------------------------------------------------------------------------
/**
 * 今、場に出ているポケモンで、死んでいるポケモンの数を返す
 *
 * @param   wk
 * @param   list    [out] 場に出ていて死んでるポケモンのパーティ内インデックスを格納する配列
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------
static u8 calc_front_dead_pokemons( BTL_CLIENT* wk, u8* list )
{
  const BTL_POKEPARAM* pp;
  u8 cnt, numMembers, i;

  numMembers = BTL_PARTY_GetMemberCount( wk->myParty );
  for(i=0, cnt=0; i<wk->numCoverPos; ++i)
  {
    pp = BTL_PARTY_GetMemberDataConst(wk->myParty, i);
    if( BTL_POKEPARAM_IsDead(pp) )
    {
      list[cnt++] = i;
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
  u8 i, selIdx;
  u8 deadList[ BTL_PARTY_MEMBER_MAX ];

  calc_front_dead_pokemons( wk, deadList );

  for(i=0; i<res->cnt; i++)
  {
    selIdx = res->selIdx[i];
    BTL_Printf("ポケモン入れ替え %d体目 <-> %d体目\n", deadList[i], selIdx );
    BTL_ACTION_SetChangeParam( &wk->actionParam[i], deadList[i], selIdx );
  }

  wk->returnDataPtr = &(wk->actionParam[0]);
  wk->returnDataSize = sizeof(wk->actionParam[0]) * res->cnt;
}

static BOOL SubProc_UI_SelectPokemon( BTL_CLIENT* wk, int* seq )
{
  switch( *seq ){
  case 0:
    {
      u8 numEmpty;
      numEmpty = calc_empty_pos( wk, NULL );
      if( numEmpty )
      {
        u8 numPuttable = calc_puttable_pokemons( wk, NULL );
        if( numPuttable )
        {
          u8 numSelect = numEmpty;

          // 生きてるポケが出さなければいけない数に足りない場合、そこを諦める
          if( numSelect > numPuttable )
          {
            abandon_cover_pos( wk, numSelect - numPuttable );
            numSelect = numPuttable;
          }

          BTL_Printf("myID=%d 戦闘ポケが死んだのでポケモン%d体選択\n", wk->myID, numSelect);
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
  u8 numEmpty;
  u8 emptyPosList[ BTL_POSIDX_MAX ];
  u8 puttableList[ BTL_PARTY_MEMBER_MAX ];
  numEmpty = calc_empty_pos( wk, emptyPosList );
  if( numEmpty )
  {
    u8 numPuttable;

    numPuttable = calc_puttable_pokemons( wk, puttableList );
    if( numPuttable )
    {
      u8 numSelect = numEmpty;
      u8 i;

      // 生きてるポケが出さなければいけない数に足りない場合、そこを諦める
      if( numSelect > numPuttable )
      {
        abandon_cover_pos( wk, numSelect - numPuttable );
        numSelect = numPuttable;
      }
      BTL_Printf("myID=%d 戦闘ポケが死んだのでポケモン%d体選択\n", wk->myID, numSelect);
      for(i=0; i<numSelect; i++)
      {
        BTL_ACTION_SetChangeParam( &wk->actionParam[i], emptyPosList[i], puttableList[i] );
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
    { SC_MSG_SET,               scProc_MSG_Set            },
    { SC_MSG_WAZA,              scProc_MSG_Waza           },
    { SC_ACT_WAZA_EFFECT,       scProc_ACT_WazaEffect     },
    { SC_ACT_WAZA_EFFECT_EX,    scProc_ACT_WazaEffectEx   },
    { SC_ACT_WAZA_DMG,          scProc_ACT_WazaDmg        },
    { SC_ACT_WAZA_DMG_DBL,      scProc_ACT_WazaDmg_Dbl    },
    { SC_ACT_WAZA_DMG_PLURAL,   scProc_ACT_WazaDmg_Plural },
    { SC_ACT_WAZA_ICHIGEKI,     scProc_ACT_WazaIchigeki   },
    { SC_ACT_CONF_DMG,          scProc_ACT_ConfDamage     },
    { SC_ACT_DEAD,              scProc_ACT_Dead           },
    { SC_ACT_MEMBER_OUT,        scProc_ACT_MemberOut      },
    { SC_ACT_MEMBER_IN,         scProc_ACT_MemberIn       },
    { SC_ACT_RANKUP,            scProc_ACT_RankUp         },
    { SC_ACT_RANKDOWN,          scProc_ACT_RankDown       },
    { SC_ACT_SICK_SET,          scProc_ACT_SickSet        },
    { SC_ACT_SICK_DMG,          scProc_ACT_SickDamage     },
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
    { SC_ACT_KILL,              scProc_ACT_Kill           },
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
      BTL_Printf("サーバーコマンド読み終わりましたよっと\n");
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
      u8 clientID = args[0];
      u8 memberIdx = args[1];

      BTLV_ACT_MemberOut_Start( wk->viewCore, clientID, memberIdx );
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

    defPokePos  = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
    affinity  = args[1];
    damage    = args[2];

    BTLV_ACT_DamageEffectSingle_Start( wk->viewCore, defPokePos, damage, affinity );
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
 * 【アクション】２体同時ダメージ処理
 */
static BOOL scProc_ACT_WazaDmg_Dbl( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ) {
  case 0:
  {
    WazaID waza;
    u8  defPokePos1, defPokePos2, aff;
    u16 damage1, damage2;

    defPokePos1 = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
    defPokePos2 = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[1] );
    aff       = args[2];
    damage1   = args[3];
    damage2   = args[4];

    BTLV_ACT_DamageEffectDouble_Start( wk->viewCore, defPokePos1, defPokePos2, aff );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_DamageEffectDouble_Wait(wk->viewCore) )
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
    u16 poke_cnt = args[0];
    BtlTypeAffAbout aff_about = BTL_CALC_TypeAffAbout( args[1] );
    u8 pokeID[ BTL_POS_MAX ];
    u8 i;
    for(i=0; i<poke_cnt; ++i){
      pokeID[i] = SCQUE_READ_ArgOnly( wk->cmdQue );
    }
    BTLV_ACT_DamageEffectPlural_Start( wk->viewCore, poke_cnt, aff_about, pokeID );
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
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
    BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
    (*seq)++;
  }
  break;

  case 1:
    if( BTLV_ACT_SimpleHPEffect_Wait(wk->viewCore) )
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
      BTLV_StartDeadAct( wk->viewCore, pos );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_WaitDeadAct( wk->viewCore ) )
    {
      BTLV_StartMsgStd( wk->viewCore, BTL_STRID_STD_Ichigeki, args );
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
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
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
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
      BTLV_StartDeadAct( wk->viewCore, pos );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_WaitDeadAct(wk->viewCore) )
    {
      BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
      BTL_POKEPARM_DeadClear( pp );
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
 *  【アクション】ターンチェックによる状態異常ダメージ処理
 */
static BOOL scProc_ACT_SickDamage( BTL_CLIENT* wk, int* seq, const int* args )
{
  switch( *seq ){
  case 0:
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
      WazaSick sick = args[1];
      int damage = args[2];
      u16 msgID;

      switch( sick ){
      default:
        GF_ASSERT_MSG(0, "poke[%d], Illegal sick ID:%d\n", args[0], sick);
        /* fallthru */
      case WAZASICK_DOKU:   msgID = BTL_STRID_SET_DokuDamage; break;
        break;
      case WAZASICK_YAKEDO: msgID = BTL_STRID_SET_YakedoDamage; break;
        break;
      case WAZASICK_AKUMU:  msgID = BTL_STRID_SET_AkumuDamage; break;
      }

      BTLV_StartMsgSet( wk->viewCore, msgID, args );  // この先ではargs[0]しか参照しないハズ…
      BTLV_ACT_SimpleHPEffect_Start( wk->viewCore, pos );
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
        pokePos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, pokeID );
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
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
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
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, pokeID );

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
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, pokeID );
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
static BOOL scProc_ACT_TraceTokusei( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, pokeID );

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
      BTL_POKEPARAM_ChangeTokusei( bpp, args[2] );
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
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
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
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, args[0] );
  BTLV_QuitTokWin( wk->viewCore, pos );
  return TRUE;
}

static BOOL scProc_OP_HpMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_HpMinus( pp, args[1] );
  return TRUE;
}


static BOOL scProc_OP_HpPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_HpPlus( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_PPMinus( BTL_CLIENT* wk, int* seq, const int* args )
{
  u8 pokeID = args[0];
  u8 wazaIdx = args[1];
  u8 value = args[2];
  u8 pokePos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, pokeID );

  BTL_POKEPARAM* pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, pokePos );
  BTL_POKEPARAM_PPMinus( pp, wazaIdx, value );
  BTL_Printf("ポケモンのPP 減らします pokeID=%d, waza=%d, val=%d\n", pokeID, wazaIdx, value);

  return TRUE;
}
static BOOL scProc_OP_HpZero( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_HpZero( pp );
  return TRUE;
}
static BOOL scProc_OP_PPPlus( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, args[0] );
  BTL_POKEPARAM_PPPlus( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankUp( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_RankUp( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankDown( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_RankDown( pp, args[1], args[2] );
  return TRUE;
}
static BOOL scProc_OP_RankSet5( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_RankSet( pp, BPP_ATTACK,      args[1] );
  BTL_POKEPARAM_RankSet( pp, BPP_DEFENCE,     args[2] );
  BTL_POKEPARAM_RankSet( pp, BPP_SP_ATTACK,   args[3] );
  BTL_POKEPARAM_RankSet( pp, BPP_SP_DEFENCE,  args[4] );
  BTL_POKEPARAM_RankSet( pp, BPP_AGILITY,     args[5] );
  return TRUE;
}
static BOOL scProc_OP_SickSet( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BPP_SICK_CONT cont;
  cont.raw = args[2];
  BTL_POKEPARAM_SetWazaSick( pp, args[1], cont );
  return TRUE;
}
static BOOL scProc_OP_CurePokeSick( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_CurePokeSick( pp );
  return TRUE;
}
static BOOL scProc_OP_CureWazaSick( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_CureWazaSick( pp, args[1] );
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
      BTL_PARTY_SwapMembers( party, posIdx, memberIdx );
    }
    BTL_Printf("メンバーイン 位置 %d <- %d にいたポケ\n", posIdx, memberIdx);
    bpp = BTL_PARTY_GetMemberData( party, posIdx );
    BTL_POKEPARAM_SetAppearTurn( bpp, args[3] );
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
  BTL_POKEPARAM_ChangePokeType( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ChangePokeForm( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_ChangeForm( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_WSTurnCheck( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_WazaSick_TurnCheck( pp );
  return TRUE;
}
static BOOL scProc_OP_RemoveItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_RemoveItem( pp );
  return TRUE;
}
static BOOL scProc_OP_UpdateUseWaza( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_UpdateUsedWazaNumber( pp, args[2], args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetUsedWaza( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_ResetUsedWazaNumber( pp );
  return TRUE;
}
static BOOL scProc_OP_SetContFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_SetContFlag( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetContFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_ResetContFlag( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_SetTurnFlag( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ResetTurnFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_ForceOffTurnFlag( pp, args[1] );
  return TRUE;
}

static BOOL scProc_OP_SetActFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_SetActFlag( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_ClearActFlag( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_ClearActFlag( pp );
  return TRUE;
}
static BOOL scProc_OP_ChangeTokusei( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_ChangeTokusei( pp, args[1] );
  return TRUE;
}
static BOOL scProc_OP_SetItem( BTL_CLIENT* wk, int* seq, const int* args )
{
  BTL_POKEPARAM* pp = BTL_POKECON_GetPokeParam( wk->pokeCon, args[0] );
  BTL_POKEPARAM_SetItem( pp, args[1] );
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
        u8 clientID = BTL_MAIN_PokeIDtoClientID( wk->mainModule, i );
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
  if( BTL_FIELD_CheckState(BTL_FLDSTATE_GRAVITY) )
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
    if( !BTL_POKEPARAM_IsDead(bpp) )
    {
      if( BTL_POKEPARAM_GetValue(bpp, BPP_TOKUSEI) == tokusei ){ ++cnt; }
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
    if( !BTL_POKEPARAM_IsDead(bpp) )
    {
      if( BTL_POKEPARAM_IsMatchType(bpp, type) ){ ++cnt; }
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
 * 現在、行動選択処理中のポケモンデータを取得
 *
 * @param   client
 *
 * @retval  const BTL_POKEPARAM*
 */
//=============================================================================================
const BTL_POKEPARAM* BTL_CLIENT_GetProcPokeData( const BTL_CLIENT* client )
{
  return BTL_CLIENT_GetFrontPokeData( client, client->procPokeIdx );
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
//=============================================================================================
/**
 * 戦闘に出ているポケモンのポケモンデータを取得
 *
 * @param   client
 * @param   posIdx
 *
 * @retval  const BTL_POKEPARAM*
 */
//=============================================================================================
const BTL_POKEPARAM* BTL_CLIENT_GetFrontPokeData( const BTL_CLIENT* client, u8 posIdx )
{
  GF_ASSERT_MSG(posIdx<client->numCoverPos, "posIdx=%d, numCoverPos=%d", posIdx, client->numCoverPos);
  return BTL_PARTY_GetMemberDataConst( client->myParty, posIdx );
}






