//=============================================================================================
/**
 * @file  btl_server_flow.c
 * @brief ポケモンWB バトルシステム サーバコマンド生成処理
 * @author  taya
 *
 * @date  2009.03.06  作成
 */
//=============================================================================================
#include <gflib.h>
#include "sound\pm_sndsys.h"
#include "waza_tool\wazadata.h"
#include "waza_tool\wazano_def.h"
#include "item\item.h"

#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_server_local.h"
#include "btl_server.h"

#include "handler\hand_tokusei.h"
#include "handler\hand_item.h"
#include "handler\hand_waza.h"
#include "handler\hand_side.h"

#include "btl_server_flow.h"

//--------------------------------------------------------
/**
 *    サーバフロー処理フラグ
 */
//--------------------------------------------------------
enum {
  HANDLER_EXHIBISION_WORK_TOTALSIZE = 512,    ///< ハンドラ反応情報を格納するワークのトータルサイズ
};

//--------------------------------------------------------
/**
 *    サーバフロー処理フラグ
 */
//--------------------------------------------------------
typedef enum {

  FLOWFLG_SET_WAZAEFFECT,

  FLOWFLG_MAX,
  FLOWFLG_BYTE_MAX = ((FLOWFLG_MAX/4)+((FLOWFLG_MAX&3)!=0)*4),

  EVENT_WORK_DEPTH = 16,
  EVENT_WORK_TOTAL_SIZE = 512,

  STRID_NULL = 0xffff,

}FlowFlag;

/**
 *  ふきとばし系のワザ処理パターン
 */
typedef enum {
  PUSHOUT_EFFECT_ESCAPE,    ///< 逃げたのと同じ（戦闘を終了する）
  PUSHOUT_EFFECT_CHANGE,    ///< 相手を入れ替えさせる（手持ちがいなければ失敗する）
  PUSHOUT_EFFECT_MUSTFAIL,  ///< 必ず失敗する
}PushOutEffect;


/**
 *  アクション優先順記録構造体
 */
typedef struct {
  u8  clientID;   ///< クライアントID
  u8  pokeIdx;    ///< そのクライアントの、何体目？ 0〜
}ACTION_ORDER_WORK;


/**
 * 対象となるポケモンパラメータを記録しておくためのワーク
 */
typedef struct {

  BTL_POKEPARAM*  pp[ BTL_POS_MAX ];
  u16             damage[ BTL_POS_MAX ];
  u8              count;
  u8              idx;

}TARGET_POKE_REC;

/**
 *  戦闘に出ているポケモンデータに順番にアクセスする処理のワーク
 */
typedef struct {
  u8 clientIdx;
  u8 pokeIdx;
  u8 endFlag;
}FRONT_POKE_SEEK_WORK;

/**
 *  イベント - ハンドラ間 連絡ワーク領域
 */

typedef struct {

  u8    work[ EVENT_WORK_TOTAL_SIZE ];
  u16   size[ EVENT_WORK_DEPTH ];
  u32   sp;

}BTL_EVENT_WORK_STACK;


/**
 *  ハンドラ反応保存用ワークマネージャ
 */
typedef struct {

  u16  stack_ptr;
  u16  read_ptr;

  u8   workBuffer[ HANDLER_EXHIBISION_WORK_TOTALSIZE ];

}HANDLER_EXHIBISION_MANAGER;

/**
 *  メッセージ表示処理コールバック関数型
 */
typedef void (*pMsgCallback)( BTL_SVFLOW_WORK*, const BTL_POKEPARAM*, void* );


/**
 *  メッセージ表示処理コールバック用パラメータ
 */
typedef struct {
  pMsgCallback    func;
  void*           arg;
}MSG_CALLBACK_PARAM;

//-----------------------------------------------------
/**
 *  メインワーク
 */
//-----------------------------------------------------
struct _BTL_SVFLOW_WORK {

  BTL_SERVER*             server;
  const BTL_MAIN_MODULE*  mainModule;
  BTL_POKE_CONTAINER*     pokeCon;
  BTL_SERVER_CMD_QUE*     que;
  u32                     turnCount;
  SvflowResult            flowResult;
  HEAPID  heapID;

  u8      numClient;
  u8      wazaEff_EnableFlag;
  u8      wazaEff_TargetPokeID;
  u8      pokeDeadFlag[ BTL_POKEID_MAX ];


  ACTION_ORDER_WORK   actOrder[ BTL_POS_MAX ];
  TARGET_POKE_REC     targetPokemon;
  TARGET_POKE_REC     targetSubPokemon;
  TARGET_POKE_REC     damagedPokemon;
  SVFL_WAZAPARAM      wazaParam;
  BTL_ACTION_PARAM    prevActionParam[ BTL_POS_MAX ];

  BTL_EVENT_WORK_STACK        eventWork;
  HANDLER_EXHIBISION_MANAGER  HEManager;
  BPP_WAZADMG_REC             wazaDamageRec;

  u8          flowFlags[ FLOWFLG_BYTE_MAX ];
};



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void clear_poke_actflags( BTL_SVFLOW_WORK* wk );
static void scproc_SetFlyingFlag( BTL_SVFLOW_WORK* wk );
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order );
static u8 countAlivePokemon( BTL_SVFLOW_WORK* wk );
static inline void FlowFlg_Set( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_Clear( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline BOOL FlowFlg_Get( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_ClearAll( BTL_SVFLOW_WORK* wk );
static void wazaEff_SetTarget( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target );
static void wazaEff_SetNoTarget( BTL_SVFLOW_WORK* wk );
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk );
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk, BTL_POKEPARAM** bpp );
static inline void TargetPokeRec_Clear( TARGET_POKE_REC* rec );
static inline void TargetPokeRec_AddWithDamage( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp, u16 damage );
static inline void TargetPokeRec_Add( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp );
static inline void TargetPokeRec_GetStart( TARGET_POKE_REC* rec );
static inline BTL_POKEPARAM* TargetPokeRec_GetNext( TARGET_POKE_REC* rec );
static inline void TargetPokeRec_Remove( TARGET_POKE_REC* rec, BTL_POKEPARAM* bpp );
static inline u32 TargetPokeRec_GetDamage( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* bpp );
static inline BTL_POKEPARAM* TargetPokeRec_Get( const TARGET_POKE_REC* rec, u32 idx );
static inline int TargetPokeRec_SeekFriendIdx( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, u32 start_idx );
static inline u32 TargetPokeRec_GetCount( const TARGET_POKE_REC* rec );
static u32 TargetPokeRec_CopyFriends( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst );
static u32 TargetPokeRec_CopyEnemys( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst );
static void TargetPokeRec_RemoveDeadPokemon( TARGET_POKE_REC* rec );
static BOOL scput_Nigeru( BTL_SVFLOW_WORK* wk, u8 clientID, u8 pokeIdx );
static void scproc_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fBtlStart );
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk );
static void scproc_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx );
static void scproc_Fight( BTL_SVFLOW_WORK* wk, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action );
static void scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, const BTL_ACTION_PARAM* action_src );
static u8 flowsub_registerWazaTargets( BTL_SVFLOW_WORK* wk, BtlPokePos atPos, WazaID waza, const BTL_ACTION_PARAM* action, TARGET_POKE_REC* rec );
static BOOL IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_WazaEffective( BTL_SVFLOW_WORK* wk, WazaID waza, u8 atkPokeID, u8 defPokeID, u32 que_reserve_pos );
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos atPos, WazaID waza, BTL_ACTION_PARAM* action );
static BOOL scproc_Fight_CheckWazaExecuteFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckConf( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static void scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scproc_WazaExecuteFailed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause fail_cause );
static BOOL scEvent_CheckMamoruBreak( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
static void scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, TARGET_POKE_REC* rec );
static void scproc_Fight_Damage( BTL_SVFLOW_WORK* wk, WazaID waza,
   BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker,
  TARGET_POKE_REC* targets, fx32 dmg_ratio );
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
      BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza,
      fx32 targetDmgRatio, BTL_EVWK_CHECK_AFFINITY* evwk );
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    WazaID waza,   BTL_EVWK_CHECK_AFFINITY* evwkAff,   fx32 targetDmgRatio );
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, BTL_EVWK_CHECK_AFFINITY** evwkAry, WazaID waza, fx32 dmg_ratio );
static void wazaDmgRec_Add( const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, SVFL_WAZAPARAM* wazaParam, u16 damage );
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, WazaID waza,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scproc_WazaDamageReaction_Sick( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scproc_WazaDamageReaction_Rank( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scproc_WazaDamageReaction_Damage( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static BOOL scproc_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, MSG_CALLBACK_PARAM* msgCallbackParam );
static BOOL addsick_core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost  );
static BtlAddSickFailCode addsick_check_fail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT sickCont );
static BOOL scEvent_AddSick_IsMustFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick );
static BOOL scEvent_AddSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick );
static u16 scEvent_AddSick_Fix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont );
static void scEvent_AddSick_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick );
static void scproc_UseItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_EatNutsAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_UseItem_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static void scPut_RemoveItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scEvent_GetItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_After_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BOOL svflowsub_hitcheck_singular( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza );
static u16 scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag );
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage );
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, u16 strID );
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost );
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u16 itemID, BOOL fAlmost, BOOL fStdMsg );
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker );
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void svflowsub_MakeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
    WazaSick sick, BPP_SICK_CONT contParam, BOOL fAlmost );
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk );
static u8 get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk );
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void scproc_Fight_Weather( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, const BTL_ACTION_PARAM* action );
static BOOL scproc_WeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );
static void scput_Fight_Others( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scput_Fight_Others_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID );
static void scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void handex_addsick_msg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, void* arg );
static void scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_messageSet( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_messageStd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_sideEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BtlEventType event_type );
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, BtlEventType event_type );
static void scproc_turncheck_sick( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg );
static void scproc_turncheck_weather( BTL_SVFLOW_WORK* wk );
static int scEvent_CalcWeatherDamage( BTL_SVFLOW_WORK* wk, BtlWeather weather, BTL_POKEPARAM* bpp, u8 *tok_cause_flg );
static inline int roundValue( int val, int min, int max );
static inline int roundMin( int val, int min );
static inline int roundMax( int val, int max );
static inline BOOL perOccur( u8 per );
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack );
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size );
static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs );
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, u8 clientID, u8 pokeIdx );
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage );
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick );
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static WazaSick scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWazaSickEx exSickCode );
static void scPut_CureSickMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick, u16 itemID );
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx exCode );
static int getCureSickStrID( WazaSick sick, BOOL fUseItem );
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
static void scPut_ResetSameWazaCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza );
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scput_DmgToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BTL_EVWK_DMG_TO_RECOVER* evwk );
static void scPut_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke );
static void scPut_WazaEffectOn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza );
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage,
  BOOL critical_flag, BOOL plural_hit_flag );
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, u32 poke_cnt, BtlTypeAff aff,
  BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag );
static void scPut_Koraeru( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void scPut_WazaDamageAffMsg( BTL_SVFLOW_WORK* wk, BtlTypeAff aff );
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage );
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume );
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, BOOL fStdMsg );
static void scPut_Ichigeki( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target );
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value );
static void scPut_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 effType );
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont );
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick );
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID );
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID, u32 argCnt, const int* args );
static void scPut_DecrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol );
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, u16 itemID );
static void scPut_AddSickDefaultMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaSick sickID, BPP_SICK_CONT sickCont );
static void scPut_ActFlag_Set( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppActFlag flagID );
static void scPut_ActFlag_Clear( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_SetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_ResetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_SetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static void scPut_ResetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static void scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8* spePriA, u8* spePriB );
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_CheckWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param );
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause );
static void scEvent_WazaExecuteFix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static u8 scEvent_GetWazaTarget( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
static void scEvent_WazaExe_NoEffect( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static BOOL scEvent_CheckTameTurnSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scEvent_checkHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckPokeHideAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IsExcuseCalcHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckNotEffect_byType( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, u8 lv,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, u16* damage );
static void scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff aff,
  u32 damage, BOOL criticalFlag );
static BtlTypeAff scEvent_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender,
  PokeType waza_type, BTL_EVWK_CHECK_AFFINITY* evwk );
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u16 wazaIdx, TARGET_POKE_REC* rec );
static BOOL scEvent_DecrementPP_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaIdx );
static BOOL scEvent_CheckPluralHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8* hitCount );
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage );
static BOOL scEvent_SimpleDamage_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag );
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  WazaID waza, SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag );
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type );
static PokeTypePair scEvent_getDefenderPokeType( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scEvent_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx );
static void scEvent_MemberIn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaID waza );
static WazaSick scEvent_CheckAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont );
static void scEvent_AfterDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, WazaID waza );
static void scEvent_MakePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
  PokeSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static void scEvent_MakeWazaSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scEvent_GetWazaRankEffectValue( BTL_SVFLOW_WORK* wk, WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume );
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume );
static void scEvent_RankEffect_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume );
static int scEvent_CalcDrainVolume( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, int damage );
static void scEvent_CheckSpecialDrain( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, u32 total_damage );
static BOOL scEvent_CheckChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8* turn );
static void scEvent_AfterChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather );
static BOOL scEvent_CheckItemUseEnable( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static u32 scEvent_CalcRecoverHP( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp );
static void Hem_Init( HANDLER_EXHIBISION_MANAGER* wk );
static u16 Hem_GetStackPtr( const HANDLER_EXHIBISION_MANAGER* wk );
static u32 Hem_PushState( HANDLER_EXHIBISION_MANAGER* wk );
static void Hem_PopState( HANDLER_EXHIBISION_MANAGER* wk, u32 state );
static BTL_HANDEX_PARAM_HEADER* Hem_ReadWork( HANDLER_EXHIBISION_MANAGER* wk );
static BTL_HANDEX_PARAM_HEADER* Hem_PushWork( HANDLER_EXHIBISION_MANAGER* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );



BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, u32 numClient, HEAPID heapID )
{
  BTL_SVFLOW_WORK* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SVFLOW_WORK) );

  wk->server = server;
  wk->pokeCon = pokeCon;
  wk->mainModule = mainModule;
  wk->numClient = numClient;
  wk->turnCount = 0;
  wk->flowResult = SVFLOW_RESULT_DEFAULT;
  wk->que = que;
  wk->heapID = heapID;

  GFL_STD_MemClear( wk->pokeDeadFlag, sizeof(wk->pokeDeadFlag) );

  Hem_Init( &wk->HEManager );

  eventWork_Init( &wk->eventWork );

  BTL_HANDLER_SIDE_InitSystem();

  return wk;
}


void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk )
{
  GFL_HEAP_FreeMemory( wk );
}


SvflowResult BTL_SVFLOW_Start_AfterPokemonIn( BTL_SVFLOW_WORK* wk )
{
  SVCL_WORK* cw;
  u32 i, posIdx;

  SCQUE_Init( wk->que );

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, i );
    if( cw == NULL ){ continue; }

    for(posIdx=0; posIdx<cw->numCoverPos; ++posIdx)
    {
      if( !BTL_POKEPARAM_IsDead(cw->member[posIdx]) )
      {
        // @@@ 先々は、戦闘開始時のMemberInと途中入れ替えのMemberInでは演出を別ける必要アリ。
        BTL_Printf("client(%d, posIdx=%d poke...In!\n", i, posIdx);
        scproc_MemberIn( wk, i, posIdx, posIdx, TRUE );
      }
    }
  }

  scproc_AfterMemberIn( wk );

  // @@@ しぬこともあるかも
  return SVFLOW_RESULT_DEFAULT;
}

SvflowResult BTL_SVFLOW_Start( BTL_SVFLOW_WORK* wk )
{
  BTL_POKEPARAM* bpp;
  SVCL_WORK* clwk;
  u8 numActPoke, alivePokeBefore, alivePokeAfter, clientID, pokeIdx, i;

  SCQUE_Init( wk->que );

  FlowFlg_ClearAll( wk );
  BTL_EVENT_StartTurn();

  wk->flowResult =  SVFLOW_RESULT_DEFAULT;

  alivePokeBefore = countAlivePokemon( wk );
  clear_poke_actflags( wk );

  scproc_SetFlyingFlag( wk );

  numActPoke = sortClientAction( wk, wk->actOrder );
  for(i=0; i<numActPoke; i++)
  {
    clientID = wk->actOrder[i].clientID;
    pokeIdx  = wk->actOrder[i].pokeIdx;
    clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
    bpp = clwk->frontMember[ pokeIdx ];

    if( !BTL_POKEPARAM_IsDead(bpp) )
    {
      const BTL_ACTION_PARAM* action = BTL_SVCL_GetPokeAction( clwk, pokeIdx );

      BTL_Printf("Client(%d) の %d 番目のポケモンのアクション\n", clientID, pokeIdx);
      switch( action->gen.cmd ){
      case BTL_ACTION_FIGHT:
        BTL_Printf("【たたかう】を処理。ワザ[%d]を、位置%d番の相手に。\n", action->fight.waza, action->fight.targetPos);
        scproc_Fight( wk, clientID, pokeIdx, action );
        break;
      case BTL_ACTION_ITEM:
        BTL_Printf("【どうぐ】を処理。アイテム%dを、%d番の相手に。\n", action->item.number, action->item.targetIdx);
        break;
      case BTL_ACTION_CHANGE:
        BTL_Printf("【ポケモン】を処理。位置%d <- ポケ%d \n", action->change.posIdx, action->change.memberIdx);
        scproc_MemberOut( wk, clientID, action->change.posIdx );
        scproc_MemberIn( wk, clientID, action->change.posIdx, action->change.memberIdx, FALSE );
        scproc_AfterMemberIn( wk );
        break;
      case BTL_ACTION_ESCAPE:
        BTL_Printf("【にげる】を処理。\n");
        if( scput_Nigeru( wk, clientID, pokeIdx ) )
        {
          PMSND_PlaySE( SEQ_SE_NIGERU );
          SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeSuccess );
          wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
        } else {
          SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeFail);
        }
        break;
      case BTL_ACTION_SKIP:
        BTL_Printf("処理をスキップ\n");
        scPut_CantAction( wk, clientID, pokeIdx );
        break;
      case BTL_ACTION_NULL:
        BTL_Printf("不明な処理\n");
        scPut_CantAction( wk, clientID, pokeIdx );
        break;
      }

      BTL_POKEPARAM_SetTurnFlag( bpp, BPP_TURNFLG_ACTION_DONE );

      if( wk->flowResult !=  SVFLOW_RESULT_DEFAULT )
      {
        return wk->flowResult;
      }
    }
  }

  // ターンチェック処理
  scproc_TurnCheck( wk );

  alivePokeAfter = countAlivePokemon( wk );

  // 死んだポケモンがいる場合の処理
  BTL_Printf( "ポケモン数 %d -> %d ...\n", alivePokeBefore, alivePokeAfter );
  if( alivePokeBefore > alivePokeAfter )
  {
    return SVFLOW_RESULT_POKE_CHANGE;
  }

  return SVFLOW_RESULT_DEFAULT;
}
static void clear_poke_actflags( BTL_SVFLOW_WORK* wk )
{
  BTL_POKEPARAM* bpp;
  FRONT_POKE_SEEK_WORK fps;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    scPut_ActFlag_Clear( wk, bpp );
  }
}

//--------------------------------------------------------------------------
/**
 * １ターン分サーバコマンド生成（ポケモン選択後）
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_StartAfterPokeSelect( BTL_SVFLOW_WORK* wk )
{
  const BTL_ACTION_PARAM* action;
  SVCL_WORK* clwk;
  u16 clientID, posIdx;
  int i, j, actionCnt;

  BTL_Printf("ひんしポケモン入れ替え選択後のサーバーコマンド生成\n");

  SCQUE_Init( wk->que );
  scproc_SetFlyingFlag( wk );

  for(i=0; i<wk->numClient; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    actionCnt = BTL_SVCL_GetNumActPoke( clwk );

    for(j=0; j<actionCnt; ++j)
    {
      action = BTL_SVCL_GetPokeAction( clwk, j );
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      BTL_Printf("クライアント(%d)のポケモン(位置%d) を、%d番目のポケといれかえる\n",
            i, action->change.posIdx, action->change.memberIdx );

      scproc_MemberIn( wk, i, action->change.posIdx, action->change.memberIdx, FALSE );
    }
  }
  scproc_AfterMemberIn( wk );

  {
    // @@@ いずれ「まきびし」とかで死ぬこともある
    return SVFLOW_RESULT_DEFAULT;
  }
}

//----------------------------------------------------------------------------------
/**
 * 場にいる全てのポケモンに可能かつ必要なら飛行フラグをセット
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_SetFlyingFlag( BTL_SVFLOW_WORK* wk )
{
  if( !BTL_FIELD_CheckState(BTL_FLDSTATE_GRAVITY) )
  {
    FRONT_POKE_SEEK_WORK  fps;
    BTL_POKEPARAM* bpp;

    FRONT_POKE_SEEK_InitWork( &fps, wk );
    while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
    {
      if( BTL_POKEPARAM_IsMatchType(bpp, POKETYPE_HIKOU)
      ||  (BTL_POKEPARAM_GetValue(bpp, BPP_TOKUSEI) == POKETOKUSEI_FUYUU)
      ||  (BTL_POKEPARAM_CheckSick(bpp, WAZASICK_FLYING))
      ){
        scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_FLYING );
      }
    }
  }
}

//--------------------------------------------------------------------------
/**
 * クライアントのアクションをチェックし、処理順序を確定
 *
 * @param   server
 * @param   order   処理する順番にクライアントIDを並べ直して格納するための配列
 *
 */
//--------------------------------------------------------------------------
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order )
{
  /*
    行動優先順  ...  3 BIT
    特殊優先順A ...  3 BIT
    ワザ優先順  ...  6 BIT
    特殊優先順B ...  3 BIT
    素早さ      ... 16 BIT
  */
  #define MakePriValue( actPri, spePriA, wazaPri, spePriB, agility )  \
            ( ((actPri&0x07)<<28) | ((spePriA&0x07)<<25) | ((wazaPri&0x3f)<<19) | ((spePriB&0x07)<<16) | (agility&0xffff) )

  SVCL_WORK* clwk;
  const BTL_ACTION_PARAM* actParam;
  const BTL_POKEPARAM* bpp;
  u32 pri[ BTL_POS_MAX ];
  u16 agility;
  u8  action, actionPri, wazaPri, spPri_A, spPri_B;
  u8  i, j, p, numPoke;

// 各ポケモンの行動プライオリティ値を生成
  for(i=0, p=0; i<wk->numClient; i++)
  {
    // クライアントごとの行動ポケモン数＆パラメータをアダプタから取得
    clwk = BTL_SERVER_GetClientWork( wk->server, i );

    numPoke = BTL_SVCL_GetNumActPoke( clwk );
    for(j=0; j<numPoke; j++)
    {
      bpp = clwk->frontMember[j];
      actParam = BTL_SVCL_GetPokeAction( clwk, j );

      // 行動による優先順（優先度高いほど数値大）
      switch( actParam->gen.cmd ){
      case BTL_ACTION_ESCAPE: actionPri = 4; break;
      case BTL_ACTION_ITEM:   actionPri = 3; break;
      case BTL_ACTION_CHANGE: actionPri = 2; break;
      case BTL_ACTION_SKIP:   actionPri = 1; break;
      case BTL_ACTION_FIGHT:  actionPri = 0; break;
      case BTL_ACTION_NULL: continue;
      default:
        GF_ASSERT(0);
        actionPri = 0;
        break;
      }
      // ワザによる優先順
      if( actParam->gen.cmd == BTL_ACTION_FIGHT ){
        WazaID  w = actParam->fight.waza;
        wazaPri = WAZADATA_GetPriority( w ) - WAZAPRI_MIN;
        // アイテム装備など、特殊な優先フラグ
        scEvent_CheckSpecialActPriority( wk, bpp, &spPri_A, &spPri_B );
      }else{
        wazaPri = 0;
        spPri_A = BTL_SPPRI_A_DEFAULT;
        spPri_B = BTL_SPPRI_B_DEFAULT;
      }
      // すばやさ
      agility = scEvent_CalcAgility( wk, bpp );

      BTL_Printf("行動プライオリティ決定！ Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
          i, j, actionPri, wazaPri, agility );

      // プライオリティ値とクライアントIDを対にして配列に保存
      pri[p] = MakePriValue( actionPri, spPri_A, wazaPri, spPri_B, agility );
      order[p].clientID = i;
      order[p].pokeIdx = j;
      ++p;
    }
  }
// プライオリティ値ソートに伴ってクライアントID配列もソート
  for(i=0; i<p; i++)
  {
    for(j=i+1; j<p; j++)
    {
      if( pri[i] < pri[j] )
      {
        u32 t = pri[i];
        pri[i] = pri[j];
        pri[j] = t;

        {
          ACTION_ORDER_WORK w = order[i];
          order[i] = order[j];
          order[j] = w;
        }
      }
    }
  }
// 全く同じプライオリティ値があったらランダムシャッフル
// @@@ 未実装

  return p;
}

// 今、戦闘に出ていて生きているポケモンの数をカウント
static u8 countAlivePokemon( BTL_SVFLOW_WORK* wk )
{
  SVCL_WORK* clwk;
  const BTL_POKEPARAM* bpp;
  u16 i, j;
  u8 cnt = 0;

  for(i=0; i<wk->numClient; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    for(j=0; j<clwk->numCoverPos; ++j)
    {
      bpp = clwk->frontMember[ j ];
      if( bpp != NULL )
      {
        if( !BTL_POKEPARAM_IsDead(bpp) )
        {
          cnt++;
        }
      }
    }
  }
  return cnt;
}

//----------------------------------------------------------------------------------------------
// サーバーフローフラグ処理
//----------------------------------------------------------------------------------------------
static inline void FlowFlg_Set( BTL_SVFLOW_WORK* wk, FlowFlag flg )
{
  wk->flowFlags[ flg ] = 1;
}
static inline void FlowFlg_Clear( BTL_SVFLOW_WORK* wk, FlowFlag flg )
{
  wk->flowFlags[ flg ] = 0;
}
static inline BOOL FlowFlg_Get( BTL_SVFLOW_WORK* wk, FlowFlag flg )
{
  return wk->flowFlags[ flg ];
}
static inline void FlowFlg_ClearAll( BTL_SVFLOW_WORK* wk )
{
  int i;
  for(i=0; i<NELEMS(wk->flowFlags); ++i)
  {
    wk->flowFlags[ i ] = 0;
  }
}

static void wazaEff_SetTarget( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target )
{
  wk->wazaEff_EnableFlag = TRUE;
  wk->wazaEff_TargetPokeID = BTL_POKEPARAM_GetID( target );
}
static void wazaEff_SetNoTarget( BTL_SVFLOW_WORK* wk )
{
  wk->wazaEff_EnableFlag = TRUE;
  wk->wazaEff_TargetPokeID = BTL_POKEID_NULL;
}

//----------------------------------------------------------------------------------------------
// 戦闘に出ているポケモンデータ順番アクセス処理
//----------------------------------------------------------------------------------------------

/**
 *  順番アクセス：ワーク初期化
 */
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk )
{
  fpsw->clientIdx = 0;
  fpsw->pokeIdx = 0;
  fpsw->endFlag = TRUE;

  {
    SVCL_WORK* cw;
    u8 i, j;

    for(i=0; i<BTL_CLIENT_MAX; ++i)
    {
      cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, i );
      if( cw == NULL ){ continue; }

      for(j=0; j<cw->numCoverPos; ++j)
      {
        if( !BTL_POKEPARAM_IsDead(cw->frontMember[j]) )
        {
          fpsw->clientIdx = i;
          fpsw->pokeIdx = j;
          fpsw->endFlag = FALSE;
          return;
        }
      }
    }
  }
}
/**
 *  順番アクセス：次のポケモンデータを取得（FALSEが返ったら終了）
 */
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk, BTL_POKEPARAM** bpp )
{
  if( fpsw->endFlag )
  {
    return FALSE;
  }
  else
  {
    BTL_POKEPARAM* nextPoke = NULL;
    SVCL_WORK* cw = BTL_SERVER_GetClientWork( wk->server, fpsw->clientIdx );

    *bpp = cw->frontMember[ fpsw->pokeIdx ];
    fpsw->pokeIdx++;

    while( fpsw->clientIdx < BTL_CLIENT_MAX )
    {
      cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, fpsw->clientIdx );
      if( cw )
      {
        while( fpsw->pokeIdx < cw->numCoverPos )
        {
          nextPoke = cw->frontMember[ fpsw->pokeIdx ];
          if( !BTL_POKEPARAM_IsDead(nextPoke) )
          {
            return TRUE;
          }
          fpsw->pokeIdx++;
        }
      }
      fpsw->clientIdx++;
      fpsw->pokeIdx = 0;
    }
    fpsw->endFlag = TRUE;
    return TRUE;
  }
}
//----------------------------------------------------------------------------------------------
// 対象ポケモン記録ワーク
//----------------------------------------------------------------------------------------------
static inline void TargetPokeRec_Clear( TARGET_POKE_REC* rec )
{
  rec->count = 0;
}
static inline void TargetPokeRec_AddWithDamage( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp, u16 damage )
{
  if( rec->count < NELEMS(rec->pp) )
  {
    rec->pp[rec->count] = pp;
    rec->damage[rec->count] = damage;
    rec->count++;
  }
  else
  {
    GF_ASSERT(0);
  }
}
static inline void TargetPokeRec_Add( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp )
{
  TargetPokeRec_AddWithDamage( rec, pp, 0 );
}
static inline void TargetPokeRec_GetStart( TARGET_POKE_REC* rec )
{
  rec->idx = 0;
}
static inline BTL_POKEPARAM* TargetPokeRec_GetNext( TARGET_POKE_REC* rec )
{
  if( rec->idx < rec->count )
  {
    return rec->pp[ rec->idx++ ];
  }
  else
  {
    return NULL;
  }
}
static inline void TargetPokeRec_Remove( TARGET_POKE_REC* rec, BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<rec->count; ++i)
  {
    if( rec->pp[i] == bpp )
    {
      u32 j;
      for(j=i+1; j<rec->count; ++j)
      {
        rec->pp[j-1] = rec->pp[j];
      }
      rec->count--;
      rec->idx--;
      break;
    }
  }
}
static inline u32 TargetPokeRec_GetDamage( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* bpp )
{
  u32 i;
  for(i=0; i<rec->count; ++i)
  {
    if( rec->pp[i] == bpp )
    {
      return rec->damage[i];
    }
  }
  GF_ASSERT(0); // ポケモン見つからない
  return 0;
}
static inline BTL_POKEPARAM* TargetPokeRec_Get( const TARGET_POKE_REC* rec, u32 idx )
{
  if( idx < rec->count )
  {
    return (BTL_POKEPARAM*)(rec->pp[ idx ]);
  }
  return NULL;
}
// 指定ポケモンと同チームのポケモン位置インデックスを返す（見つからなければ-1）
static inline int TargetPokeRec_SeekFriendIdx( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, u32 start_idx )
{
  u8 pokeID1, pokeID2, i;

  pokeID1 = BTL_POKEPARAM_GetID( pp );
  for(i=start_idx; i<rec->count; ++i)
  {
    pokeID2 = BTL_POKEPARAM_GetID( rec->pp[i] );
    if( BTL_MAINUTIL_IsFriendPokeID(pokeID1, pokeID2) )
    {
      return i;
    }
  }
  return -1;
}
// ポケモン総数を返す
static inline u32 TargetPokeRec_GetCount( const TARGET_POKE_REC* rec )
{
  return rec->count;
}
// 指定ポケモンと同チームのポケモンのみを別ワークにコピー
// return : コピーしたポケモン数
static u32 TargetPokeRec_CopyFriends( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst )
{
  BTL_POKEPARAM* bpp;
  u32 max, i;
  u8 ID1, ID2;

  TargetPokeRec_Clear( dst );
  ID1 = BTL_POKEPARAM_GetID( pp );

  max = TargetPokeRec_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    bpp = TargetPokeRec_Get( rec, i );
    ID2 = BTL_POKEPARAM_GetID( bpp );
    if( BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) ){
      TargetPokeRec_Add( dst, bpp );
    }
  }
  return TargetPokeRec_GetCount( dst );
}
// 指定ポケモンと敵チームのポケモンのみを別ワークにコピー
// return : コピーしたポケモン数
static u32 TargetPokeRec_CopyEnemys( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst )
{
  BTL_POKEPARAM* bpp;
  u32 max, i;
  u8 ID1, ID2;

  TargetPokeRec_Clear( dst );
  ID1 = BTL_POKEPARAM_GetID( pp );

  max = TargetPokeRec_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    bpp = TargetPokeRec_Get( rec, i );
    ID2 = BTL_POKEPARAM_GetID( bpp );
    if( !BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) ){
      TargetPokeRec_Add( dst, bpp );
    }
  }
  return TargetPokeRec_GetCount( dst );
}
// 死んでるポケモンを削除
static void TargetPokeRec_RemoveDeadPokemon( TARGET_POKE_REC* rec )
{
  BTL_POKEPARAM* bpp;

  TargetPokeRec_GetStart( rec );

  while( (bpp = TargetPokeRec_GetNext(rec)) != NULL )
  {
    if( BTL_POKEPARAM_IsDead(bpp) ){
      TargetPokeRec_Remove( rec, bpp );
    }
  }
}


//======================================================================================================
// サーバーフロー処理
//======================================================================================================

//-----------------------------------------------------------------------------------
// サーバーフロー：「にげる」
//-----------------------------------------------------------------------------------
static BOOL scput_Nigeru( BTL_SVFLOW_WORK* wk, u8 clientID, u8 pokeIdx )
{
  SVCL_WORK* clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  BTL_POKEPARAM* bpp = clwk->member[ pokeIdx ];

  BOOL fEscape = TRUE;  // @@@ 今はすばやさ計算をせず誰でも逃げられるようにしている

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, fEscape );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_ESCAPE );
    fEscape = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  #ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
    fEscape = TRUE;
  }
  #endif

  return fEscape;
}
//-----------------------------------------------------------------------------------
// サーバーフロー：メンバーを場に登場させる
//-----------------------------------------------------------------------------------
static void scproc_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fBtlStart )
{
  SVCL_WORK* clwk;
  BTL_PARTY* party;
  BTL_POKEPARAM* bpp;

  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  GF_ASSERT(posIdx < clwk->numCoverPos);

  if( posIdx != next_poke_idx )
  {
    BTL_PARTY_SwapMembers( party, posIdx, next_poke_idx );
  }
  bpp = BTL_PARTY_GetMemberData( party, posIdx );

  clwk->frontMember[ posIdx ] = bpp;

  BTL_HANDLER_TOKUSEI_Add( bpp );
  BTL_HANDLER_ITEM_Add( bpp );
  BTL_POKEPARAM_SetContFlag( bpp, BPP_CONTFLG_MEMBERIN_EFFECT );
  BTL_POKEPARAM_SetAppearTurn( bpp, wk->turnCount );

  SCQUE_PUT_OP_MemberIn( wk->que, clientID, posIdx, next_poke_idx, wk->turnCount );
  if( !fBtlStart )
  {
    SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, next_poke_idx, wk->turnCount );
  }
}
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u32 hem_state = Hem_PushState( &wk->HEManager );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    if( BTL_POKEPARAM_GetContFlag(bpp, BPP_CONTFLG_MEMBERIN_EFFECT) )
    {
      scEvent_MemberIn( wk, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_POKEPARAM_ResetContFlag( bpp, BPP_CONTFLG_MEMBERIN_EFFECT );
    }
  }
  Hem_PopState( &wk->HEManager, hem_state );
}


//-----------------------------------------------------------------------------------
// サーバーフロー：メンバーを場から退場させる
//-----------------------------------------------------------------------------------
static void scproc_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx )
{
  scEvent_MemberOut( wk, clientID, posIdx );
}
//-----------------------------------------------------------------------------------
// サーバーフロー：「たたかう」ルート
//-----------------------------------------------------------------------------------
static void scproc_Fight( BTL_SVFLOW_WORK* wk, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action )
{
  SVCL_WORK *atClient;
  BTL_POKEPARAM  *attacker;
  WazaID  waza;
  u8 attacker_pokeID;
  u8 waza_exe_flag;

  atClient = BTL_SERVER_GetClientWork( wk->server, attackClientID );
  attacker = atClient->frontMember[posIdx];
  attacker_pokeID = BTL_POKEPARAM_GetID( attacker );

  waza = action->fight.waza;
  BTL_HANDLER_Waza_Add( attacker, waza );

  do {
    waza_exe_flag = FALSE;

    // ワザ出し失敗判定
    if( scproc_Fight_CheckWazaExecuteFail(wk, attacker, waza) ){ break; }

    // ワザ出し成功
    scproc_Fight_WazaExe( wk, attacker, waza, action );
    waza_exe_flag = TRUE;

  }while(0);

  if( waza_exe_flag == FALSE ){
    scPut_ResetSameWazaCounter( wk, attacker );
  }

  BTL_HANDLER_Waza_Remove( attacker, waza );
}

//----------------------------------------------------------------------------------
/**
 * 「たたかう」-> ワザ出し成功以降の処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, const BTL_ACTION_PARAM* action_src )
{
  WazaCategory  category = WAZADATA_GetCategory( waza );
  u8 pokeID = BTL_POKEPARAM_GetID( attacker );
  BtlPokePos    atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, pokeID );
  BTL_ACTION_PARAM  action = *action_src;
  u16  que_reserve_pos;

  scEvent_CheckWazaParam( wk, waza, attacker, &wk->wazaParam );

// @@@ ここに、対象ポケによらない無効化チェックを入れるかも…

  BTL_POKEPARAM_UpdateUsedWazaNumber( attacker, waza, action.fight.targetPos );
  SCQUE_PUT_OP_UpdateUseWaza( wk->que, pokeID, action.fight.targetPos, waza );
  BTL_POKEPARAM_SetTurnFlag( attacker, BPP_TURNFLG_WAZA_EXE );

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_WazaExecuteFix( wk, attacker, waza );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }

  if( scproc_Fight_TameWazaExe(wk, attacker, atPos, waza, &action) ){
    return;
  }

  scPut_WazaMsg( wk, attacker, waza );

  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );
  wk->wazaEff_EnableFlag = FALSE;
  wk->wazaEff_TargetPokeID = BTL_POKEID_NULL;

  // ワザ対象をワークに取得
  TargetPokeRec_Clear( &wk->damagedPokemon );
  TargetPokeRec_Clear( &wk->targetPokemon );
  flowsub_registerWazaTargets( wk, atPos, waza, &action, &wk->targetPokemon );
  flowsub_checkNotEffect( wk, waza, attacker, &wk->targetPokemon );

  switch( category ){
  case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT:
  case WAZADATA_CATEGORY_DAMAGE_SICK:
  case WAZADATA_CATEGORY_DRAIN:
    scproc_Fight_Damage( wk, waza, attacker, &wk->targetPokemon );
    break;
  case WAZADATA_CATEGORY_SIMPLE_EFFECT:
    scproc_Fight_SimpleEffect( wk, waza, attacker, &wk->targetPokemon );
    break;
  case WAZADATA_CATEGORY_SIMPLE_SICK:
    scproc_Fight_SimpleSick( wk, waza, attacker, &wk->targetPokemon );
    break;
  case WAZADATA_CATEGORY_EFFECT_SICK:
    scproc_Fight_EffectSick( wk, waza, attacker, &wk->targetPokemon );
    break;
  case WAZADATA_CATEGORY_ICHIGEKI:
    scproc_Fight_Ichigeki( wk, waza, attacker, &wk->targetPokemon );
    break;
  case WAZADATA_CATEGORY_WEATHER:
    scproc_Fight_Weather( wk, waza, attacker, &action );
    break;
  case WAZADATA_CATEGORY_PUSHOUT:
    scproc_Fight_PushOut( wk, waza, attacker, &wk->targetPokemon );
    break;
  case WAZADATA_CATEGORY_SIMPLE_RECOVER:
    scproc_Fight_SimpleRecover( wk, waza, attacker );
    break;
//  case WAZADATA_CATEGORY_BIND:
//  case WAZADATA_CATEGORY_GUARD:
//  case WAZADATA_CATEGORY_FIELD_EFFECT:
//  case WAZADATA_CATEGORY_SIDE_EFFECT:
  case WAZADATA_CATEGORY_OTHERS:
    scput_Fight_Others( wk, waza, attacker, &wk->targetPokemon );
    break;
  default:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
    break;
  }

  // ワザ効果あり確定→演出表示コマンド生成などへ
  if( wk->wazaEff_EnableFlag ){
    scproc_Fight_WazaEffective( wk, waza, pokeID, wk->wazaEff_TargetPokeID, que_reserve_pos );
  }else{
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_WazaExe_NoEffect( wk, pokeID, waza );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }

  if( WAZADATA_IsTire(waza) ){
    scPut_ActFlag_Set( wk, attacker, BPP_ACTFLG_CANT_ACTION );
  }

  if( !BTL_POKEPARAM_CheckSick(attacker, WAZASICK_WAZALOCK_HARD) )
  {
    u8 wazaIdx = BTL_POKEPARAM_GetWazaIdx( attacker, action.fight.waza );
    if( wazaIdx != PTL_WAZA_MAX ){
      scproc_decrementPP( wk, attacker, wazaIdx, &wk->damagedPokemon );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * 対象ポケモンのデータポインタを、TARGET_POKE_REC構造体にセット
 *
 * @param   wk
 * @param   atPos
 * @param   waza
 * @param   action
 * @param   rec       [out]
 *
 * @retval  u8        対象ポケモン数
 */
//--------------------------------------------------------------------------
static u8 flowsub_registerWazaTargets( BTL_SVFLOW_WORK* wk, BtlPokePos atPos, WazaID waza, const BTL_ACTION_PARAM* action, TARGET_POKE_REC* rec )
{
  WazaTarget  targetType = WAZADATA_GetTarget( waza );
  BTL_POKEPARAM* attacker = BTL_POKECON_GetFrontPokeData( wk->pokeCon, atPos );

  TargetPokeRec_Clear( rec );

  // シングル
  if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE )
  {
    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:  ///< 自分以外の１体（選択）
    case WAZA_TARGET_ENEMY_SELECT:  ///< 敵１体（選択）
    case WAZA_TARGET_ENEMY_RANDOM:  ///< 敵ランダム
    case WAZA_TARGET_ENEMY_ALL:     ///< 敵側２体
    case WAZA_TARGET_OTHER_ALL:     ///< 自分以外全部
      TAYA_Printf("ターゲットあいて：waza=%d, type=%d\n", waza, targetType);
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
      break;

    case WAZA_TARGET_USER:                ///< 自分１体のみ
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体
      TAYA_Printf("ターゲットじぶん：waza=%d, type=%d\n", waza, targetType);
      TargetPokeRec_Add( rec, attacker );
      break;

    case WAZA_TARGET_UNKNOWN:
      {
        u8 pokeID = scEvent_GetWazaTarget( wk, attacker, &wk->wazaParam );
        if( pokeID != BTL_POKEID_NULL ){
          TargetPokeRec_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, pokeID) );
          return 1;
        }else{
          return 0;
        }
      }
      break;

    default:
      return 0;
    }

    return 1;
  }
  // ダブル
  else
  {
    BTL_POKEPARAM* bpp = NULL;

    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, action->fight.targetPos );
//      TargetPokeRec_Add( rec, bpp );
      break;
    case WAZA_TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, action->fight.targetPos );
//      TargetPokeRec_Add( rec, bpp );
      break;
    case WAZA_TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
      bpp = get_opponent_pokeparam( wk, atPos, GFL_STD_MtRand(1) );
//      TargetPokeRec_Add( rec, bpp );
      break;

    case WAZA_TARGET_ENEMY_ALL:           ///< 敵側全体
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 1) );
      return 2;
    case WAZA_TARGET_OTHER_ALL:           ///< 自分以外全部
      TargetPokeRec_Add( rec, get_next_pokeparam( wk, atPos ) );
      TargetPokeRec_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
      TargetPokeRec_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
      return 3;

    case WAZA_TARGET_USER:      ///< 自分１体のみ
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
      return 1;
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, action->fight.targetPos) );
      return 1;
    case WAZA_TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
      TargetPokeRec_Add( rec, get_next_pokeparam(wk, atPos) );
      return 1;

    case WAZA_TARGET_UNKNOWN:
      {
        u8 pokeID = scEvent_GetWazaTarget( wk, attacker, &wk->wazaParam );
        if( pokeID != BTL_POKEID_NULL ){
          bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
        }
      }
      break;

    default:
      return 0;
    }

    if( bpp )
    {
      u8 targetPokeID = scEvent_GetWazaTarget( wk, attacker, &wk->wazaParam );
      if( targetPokeID != BTL_POKEID_NULL ){
        bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, targetPokeID );
      }
      TargetPokeRec_Add( rec, bpp );
      return 1;
    }
    else
    {
      return 0;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 必中状態かどうかチェック
 *
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL    必中状態ならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_MUSTHIT) ){
    return TRUE;
  }
  if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_MUSTHIT_TARGET) )
  {
    u8 targetPokeID = BTL_POKEPARAM_GetSickParam( attacker, WAZASICK_MUSTHIT_TARGET );
    if( targetPokeID == BTL_POKEPARAM_GetID(target) ){
      return TRUE;
    }
  }

  return FALSE;
}

//--------------------------------------------------------------------------
/**
 * ワザ発動前の、対象別無効化チェック
 *
 * @param   wk
 * @param   attacker  [in] ワザを出すポケモン
 * @param   targets   [io] ワザ対象ポケモンコンテナ／無効になったポケモンはコンテナから削除される
 *
 */
//--------------------------------------------------------------------------
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;

  TargetPokeRec_GetStart( targets );
  // タイプ相性による無効化チェック
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckNotEffect_byType(wk, waza, attacker, bpp) )
    {
      TargetPokeRec_Remove( targets, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(bpp) );
    }
  }

  // Lv1 無効化チェック（攻撃ポケが必中状態なら無効化できない）
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( !IsMustHit(attacker, bpp) )
    {
      if( scEvent_CheckNotEffect(wk, waza, 0, attacker, bpp) )
      {
        TargetPokeRec_Remove( targets, bpp );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(bpp) );
      }
    }
  }

  // Lv2 無効化チェック（攻撃ポケが必中状態でも無効化）
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckNotEffect(wk, waza, 1, attacker, bpp) )
    {
      TargetPokeRec_Remove( targets, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(bpp) );
    }
  }

  // まもるチェック
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_MAMORU) )
    {
      // まもる無効化されなければターゲットから除外
      if( !scEvent_CheckMamoruBreak(wk, attacker, bpp, waza) ){
        TargetPokeRec_Remove( targets, bpp );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Mamoru, BTL_POKEPARAM_GetID(bpp) );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ワザ効果あることが確定→必要な演出コマンドの生成
 *
 * @param   wk
 * @param   waza
 * @param   atkPokeID
 * @param   defPokeID
 * @param   que_reserve_pos
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_WazaEffective( BTL_SVFLOW_WORK* wk, WazaID waza, u8 atkPokeID, u8 defPokeID, u32 que_reserve_pos )
{
  BtlPokePos  atPos, defPos;

  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, atkPokeID );
  defPos = ( defPokeID != BTL_POKEID_NULL )?
    BTL_MAIN_PokeIDtoPokePos( wk->mainModule, defPokeID ) : BTL_POS_NULL;

  SCQUE_PUT_ReservedPos( wk->que, que_reserve_pos, SC_ACT_WAZA_EFFECT, atPos, defPos, waza );
}
//----------------------------------------------------------------------------------
/**
 * 必要なら、１ターン溜めワザの溜めターン処理を行う。
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   action    [io]
 *
 * @retval  BOOL      溜めターン処理を行った場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos atPos, WazaID waza, BTL_ACTION_PARAM* action )
{
  if( WAZADATA_GetFlag(waza, WAZAFLAG_TAME) )
  {
    if( !scEvent_CheckTameTurnSkip(wk, attacker, waza) )
    {
      if( !BTL_POKEPARAM_GetContFlag(attacker, BPP_CONTFLG_TAME) )
      {
        u8 pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, BTL_POKEPARAM_GetID(attacker) );
        BPP_SICK_CONT  sickCont = BTL_CALC_MakeWazaSickCont_Turn( 2 );
        scPut_SetContFlag( wk, attacker, BPP_CONTFLG_TAME );
        scPut_AddSick( wk, attacker, WAZASICK_WAZALOCK_HARD, sickCont );
        SCQUE_PUT_ACT_WazaEffectEx( wk->que, pos, BTL_POS_NULL, waza, BTLV_WAZAEFF_TURN_TAME );
        wk->prevActionParam[ atPos ] = *action;
        {
          u32 hem_state = Hem_PushState( &wk->HEManager );
          scEvent_TameStart( wk, attacker, waza );
          scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
          Hem_PopState( &wk->HEManager, hem_state );
        }
        return TRUE;
      }
      else
      {
        *action = wk->prevActionParam[ atPos ];
        scPut_ResetContFlag( wk, attacker, BPP_CONTFLG_TAME );
        {
          u32 hem_state = Hem_PushState( &wk->HEManager );
          scEvent_TameRelease( wk, attacker, waza );
          scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
          Hem_PopState( &wk->HEManager, hem_state );
        }
      }
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ワザ出し失敗チェック
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  SV_WazaFailCause    失敗した場合失敗コード／成功の場合 SV_WAZAFAIL_NULL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckWazaExecuteFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;

  do {
    // こんらん自爆判定
    if( scproc_Fight_CheckConf(wk, attacker) ){
      cause = SV_WAZAFAIL_KONRAN;
      break;
    }

    // ねむり・こおり等の解除チェック
    scproc_PokeSickCure_WazaCheck( wk, attacker, waza );

    // その他の失敗チェック
    cause = scEvent_CheckWazaExecute( wk, attacker, waza );

  }while( 0 );


  if( cause != SV_WAZAFAIL_NULL ){
    BTL_Printf("ポケID=%d, 失敗コード=%dですよ\n", BTL_POKEPARAM_GetID(attacker), cause);
    scproc_WazaExecuteFailed( wk, attacker, waza, cause );
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * こんらん自爆チェック＆処理
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    自爆した場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckConf( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker )
{
  if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KONRAN) )
  {
    scPut_ConfCheck( wk, attacker );
    if( BTL_CALC_IsOccurPer(BTL_CONF_EXE_RATIO) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ねむり・こおり等、アクション選択後に解除チェックを行う処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  PokeSick sick = BTL_POKEPARAM_GetPokeSick( attacker );
  BOOL f_cured = FALSE;

  switch( sick ){
  case POKESICK_NEMURI:
    if( BTL_POKEPARAM_Nemuri_CheckWake(attacker) ){
      f_cured = TRUE;
    }
    break;
  case POKESICK_KOORI:
    if( WAZADATA_IsImage(waza, WAZA_IMG_HEAT)
    ||  BTL_CALC_IsOccurPer( BTL_KORI_MELT_PER )
    ){
      f_cured = TRUE;
    }
    break;
  }

  if( f_cured ){
    scPut_CurePokeSick( wk, attacker, sick );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ワザ出し失敗時処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   fail_cause
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExecuteFailed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause fail_cause )
{
  if( fail_cause == SV_WAZAFAIL_KONRAN ){
    u16 conf_dmg = scEvent_CalcConfDamage( wk, attacker );
    scPut_ConfDamage( wk, attacker, conf_dmg );
  }else{
    scPut_WazaExecuteFailMsg( wk, attacker, waza, fail_cause );
  }

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_CheckWazaExeFail( wk, attacker, waza, fail_cause );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] まもる・みきりの無効化チェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL    無効化する場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckMamoruBreak( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_MAMORU_BREAK );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
// 指定位置から見て対戦相手のポケモンデータを返す
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx )
{
  SVCL_WORK* clwk;
  BtlPokePos targetPos;
  u8 clientID, posIdx;

  targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, pos, pokeSideIdx );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return clwk->frontMember[ posIdx ];
}
// 指定位置から見て隣のポケモンデータを返す
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos )
{
  SVCL_WORK* clwk;
  BtlPokePos nextPos;
  u8 clientID, posIdx;

  nextPos = BTL_MAIN_GetNextPokePos( wk->mainModule, pos );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, nextPos, &clientID, &posIdx );
  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return clwk->frontMember[ posIdx ];
}
//--------------------------------------------------------------------------
/**
 * [Proc] 使ったワザのPPデクリメント
 *
 * @param   wk
 * @param   attacker  攻撃ポケモンパラメータ
 * @param   wazaIdx   使ったワザインデックス
 * @param   rec       ダメージを受けたポケモンパラメータ群
 *
 */
//--------------------------------------------------------------------------
static void scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, TARGET_POKE_REC* rec )
{
  u8 vol = scEvent_DecrementPPVolume( wk, attacker, wazaIdx, rec );
  if( vol )
  {
    scPut_DecrementPP( wk, attacker, wazaIdx, vol );

    if( scEvent_DecrementPP_Reaction(wk, attacker, wazaIdx) )
    {
      scproc_UseItem( wk, attacker );
    }
  }
}
//----------------------------------------------------------------------
// サーバーフロー：「たたかう」> ダメージワザ系
//----------------------------------------------------------------------
static void scproc_Fight_Damage( BTL_SVFLOW_WORK* wk, WazaID waza,
   BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  u8 target_cnt;
  fx32 dmg_ratio;

  FlowFlg_Clear( wk, FLOWFLG_SET_WAZAEFFECT );

  target_cnt = TargetPokeRec_GetCount( targets );
  dmg_ratio = (target_cnt == 1)? BTL_CALC_DMG_TARGET_RATIO_NONE : BTL_CALC_DMG_TARGET_RATIO_PLURAL;

// 味方に対するダメージ
  if( TargetPokeRec_CopyFriends(targets, attacker, &wk->targetSubPokemon) )
  {
    TargetPokeRec_RemoveDeadPokemon( &wk->targetSubPokemon );
    scproc_Fight_Damage_side( wk, waza, attacker, &wk->targetSubPokemon, dmg_ratio );
  }
// 敵に対するダメージ
  if( TargetPokeRec_CopyEnemys(targets, attacker, &wk->targetSubPokemon) )
  {
    // @@@ 本当は死んでたら別ターゲットにしないとダメかも
    TargetPokeRec_RemoveDeadPokemon( &wk->targetSubPokemon );
    scproc_Fight_Damage_side( wk, waza, attacker, &wk->targetSubPokemon, dmg_ratio );
  }

  scproc_Fight_Damage_After( wk, waza, attacker, &wk->damagedPokemon );
}
static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker,
  TARGET_POKE_REC* targets, fx32 dmg_ratio )
{
  BTL_POKEPARAM* bpp;
  u8 poke_cnt;

  // 命中チェック -> 命中しなかった場合は targets から除外
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( !scEvent_checkHit(wk, attacker, bpp, waza) )
    {
      scPut_WazaAvoid( wk, bpp, waza );
      TargetPokeRec_Remove( targets, bpp );
    }
  }
  // 回復チェック -> 特定タイプのダメージを無効化、回復してしまうポケを targets から除外
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckDmgToRecover(wk, attacker, bpp, waza) )
    {
      u32 hem_state;

      wazaEff_SetTarget( wk, bpp );
      hem_state = Hem_PushState( &wk->HEManager );
//        scput_DmgToRecover( wk, bpp, ewk );
      scEvent_DmgToRecover( wk, attacker, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

      Hem_PopState( &wk->HEManager, hem_state );
      TargetPokeRec_Remove( targets, bpp );
    }
  }

  TargetPokeRec_GetStart( targets );
  poke_cnt = TargetPokeRec_GetCount( targets );
  if( poke_cnt == 1 )
  {
    BTL_EVWK_CHECK_AFFINITY* evwk;
    bpp = TargetPokeRec_GetNext( targets );
    evwk = eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_CHECK_AFFINITY) );
    scEvent_checkWazaDamageAffinity( wk, bpp, wk->wazaParam.wazaType, evwk );
    scproc_Fight_Damage_side_single( wk, attacker, bpp, waza, dmg_ratio, evwk );
    eventWork_Pop( &wk->eventWork, evwk );
  }
  else if( poke_cnt > 1 )
  {
    BTL_EVWK_CHECK_AFFINITY* evwkAry[ BTL_POSIDX_MAX ];
    u8 f_same_aff;
    int i, j;

    f_same_aff = TRUE;
    for(i=0; i<poke_cnt; ++i)
    {
      bpp = TargetPokeRec_Get( targets, i );
      evwkAry[i] = eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_CHECK_AFFINITY) );
      scEvent_checkWazaDamageAffinity( wk, bpp, wk->wazaParam.wazaType, evwkAry[i] );
    }

  // 相性が別のヤツがいる or 相性がハンドラによって変化された場合は個別の処理
    for(i=0; i<poke_cnt; ++i)
    {
      if( evwkAry[i]->weakedByItem ){
        f_same_aff = FALSE;
        break;
      }
      for(j=0; j<i; ++j){
        if( BTL_CALC_TypeAffAbout(evwkAry[i]->aff) != BTL_CALC_TypeAffAbout(evwkAry[j]->aff) ){
          f_same_aff = FALSE;
          i = poke_cnt;
          break;
        }
      }
    }
    // 相性が１体でも別のヤツがいるなら個別の処理
    if( !f_same_aff )
    {
      for(i=0; i<poke_cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        scproc_Fight_Damage_side_single( wk, attacker, bpp, waza, dmg_ratio, evwkAry[i] );
      }
    }
    // 相性が全てのターゲットで一致なら、体力バーを同時に減らすようにコマンド生成する
    else
    {
      scproc_Fight_damage_side_plural( wk, attacker, targets, evwkAry, waza, dmg_ratio );
    }

    for(i=poke_cnt-1; i>=0; --i){
      eventWork_Pop( &wk->eventWork, evwkAry[i] );
    }
  }
}
//------------------------------------------------------------------
// サーバーフロー下請け：単体ダメージ処理（上位分岐）
//------------------------------------------------------------------
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
      BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza,
      fx32 targetDmgRatio, BTL_EVWK_CHECK_AFFINITY* evwk )
{
  svflowsub_damage_act_singular( wk, attacker, defender, waza, evwk, targetDmgRatio );

  scPut_CheckDeadCmd( wk, defender );
  scPut_CheckDeadCmd( wk, attacker );
}
//------------------------------------------------------------------
// サーバーフロー下請け：単体ダメージ処理（下位）
//------------------------------------------------------------------
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    WazaID waza,   BTL_EVWK_CHECK_AFFINITY* evwkAff,   fx32 targetDmgRatio )
{
  u8 plural_flag = FALSE; // 複数回ヒットフラグ
  u8 deadFlag = FALSE;
  u8 hit_count, critical_flag, item_use_flag, koraeru_cause;
  u16 damage;
  u32 damage_sum;
  int i;

  plural_flag = scEvent_CheckPluralHit( wk, attacker, waza, &hit_count );

  wazaEff_SetTarget( wk, defender );
  damage_sum = 0;

  for(i=0; i<hit_count; ++i)
  {
    critical_flag = scEvent_CheckCritical( wk, attacker, defender, waza );
    damage = scEvent_CalcDamage( wk, attacker, defender, waza, evwkAff->aff, targetDmgRatio, critical_flag );

    // デバッグを簡単にするため必ず大ダメージにする措置
    #ifdef PM_DEBUG
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    {
      u8 pokeID = BTL_POKEPARAM_GetID(defender);
      if( pokeID >= BTL_PARTY_MEMBER_MAX )
      {
        damage = 999;
      }
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      u8 pokeID = BTL_POKEPARAM_GetID(defender);
      if( pokeID < BTL_PARTY_MEMBER_MAX )
      {
        damage = 999;
      }
    }
    #endif

    koraeru_cause = scEvent_CheckKoraeru( wk, attacker, defender, &damage );
    scPut_WazaDamageSingle( wk, defender, evwkAff->aff, damage, critical_flag, plural_flag );
    if( koraeru_cause != BPP_KORAE_NONE ){
      scPut_Koraeru( wk, defender, koraeru_cause );
    }
    damage_sum += damage;
    scproc_WazaAdditionalEffect( wk, waza, attacker, defender, damage );

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );

      scEvent_WazaDamageReaction( wk, attacker, defender, waza, evwkAff->aff, damage, critical_flag );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

      Hem_PopState( &wk->HEManager, hem_state );
    }

    scproc_Fight_Damage_Kickback( wk, attacker, waza, damage );

    if( BTL_POKEPARAM_IsDead(defender) ){
      deadFlag = TRUE;
    }
    if( BTL_POKEPARAM_IsDead(attacker) ){
      deadFlag = TRUE;
    }
    if( deadFlag ){ break; }
  }

  TargetPokeRec_AddWithDamage( &wk->damagedPokemon, defender, damage_sum );
  if( damage_sum ){
    wazaDmgRec_Add( attacker, defender, &wk->wazaParam, damage_sum );
    BTL_POKEPARAM_SetTurnFlag( defender, BPP_TURNFLG_DAMAGED );
  }

  if( plural_flag )
  {
    // @@@ 「●●回あたった！」メッセージ
//    scPut_WazaDamageAffMsg( wk, evwkAff );
//    SCQUE_PUT_MSG_WazaHitCount( wk->que, i ); // @@@ あとで
  }
}
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, BTL_EVWK_CHECK_AFFINITY** evwkAry, WazaID waza, fx32 dmg_ratio )
{
  BTL_POKEPARAM *bpp[ BTL_POSIDX_MAX ];
  u16 dmg[ BTL_POSIDX_MAX ];
  u32 dmg_sum;
  u8  critical_flg[ BTL_POSIDX_MAX ];
  u8  koraeru_cause[ BTL_POSIDX_MAX ];
  u8 i, poke_cnt;

  poke_cnt = TargetPokeRec_GetCount( targets );
  GF_ASSERT(poke_cnt < BTL_POSIDX_MAX);

  wazaEff_SetNoTarget( wk );
  dmg_sum = 0;
  for(i=0; i<poke_cnt; ++i)
  {
    bpp[i] = TargetPokeRec_Get( targets, i );
    critical_flg[i] = scEvent_CheckCritical( wk, attacker, bpp[i], waza );
    dmg[i] = scEvent_CalcDamage( wk, attacker, bpp[i], waza, evwkAry[i]->aff, dmg_ratio, critical_flg[i] );
    koraeru_cause[i] = scEvent_CheckKoraeru( wk, attacker, bpp[i], &dmg[i] );
    dmg_sum += dmg[i];
    TargetPokeRec_AddWithDamage( &wk->damagedPokemon, bpp[i], dmg[i] );
    if( dmg[i] ){
      wazaDmgRec_Add( attacker, bpp[i], &wk->wazaParam, dmg[i] );
      BTL_POKEPARAM_SetTurnFlag( bpp[i], BPP_TURNFLG_DAMAGED );
    }
  }

  scPut_WazaDamagePlural( wk, poke_cnt, evwkAry[0]->aff, bpp, dmg, critical_flg );
  for(i=0; i<poke_cnt; ++i)
  {
    if( koraeru_cause[i] != BPP_KORAE_NONE ){
      scPut_Koraeru( wk, bpp[i], koraeru_cause[i] );
    }
  }

  for(i=0; i<poke_cnt; ++i){
    scproc_WazaAdditionalEffect( wk, waza, attacker, bpp[i], dmg[i] );
  }

  for(i=0; i<poke_cnt; ++i)
  {
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );

      scEvent_WazaDamageReaction( wk, attacker, bpp[i], waza,
        evwkAry[i]->aff, dmg[i], critical_flg[i] );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

      Hem_PopState( &wk->HEManager, hem_state );
    }
  }

  scproc_Fight_Damage_Kickback( wk, attacker, waza, dmg_sum );

  for(i=0; i<poke_cnt; ++i)
  {
    scPut_CheckDeadCmd( wk, bpp[i] );
  }
  scPut_CheckDeadCmd( wk, attacker );
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージレコードを追加
 *
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   damage
 */
//----------------------------------------------------------------------------------
static void wazaDmgRec_Add( const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, SVFL_WAZAPARAM* wazaParam, u16 damage )
{
  BPP_WAZADMG_REC rec;
  BPP_WAZADMG_REC_Setup( &rec, BTL_POKEPARAM_GetID(attacker), wazaParam->wazaID, wazaParam->wazaType, damage );
  BTL_POKEPARAM_WAZADMG_REC_Add( defender, &rec );
}

//--------------------------------------------------------------------------
/**
 * ワザダメージ後の追加効果処理
 *
 * @param   wk
 * @param   waza        使用ワザ
 * @param   attacker    攻撃ポケモンパラメータ
 * @param   defender    防御ポケモンパラメータ
 * @param   damage      与えたダメージ
 *
 */
//--------------------------------------------------------------------------
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, WazaID waza,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
  WazaCategory category = WAZADATA_GetCategory( waza );

  if( !BTL_POKEPARAM_IsDead(defender) )
  {
    switch( category ){
    case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
      scproc_Fight_Damage_AddEffect( wk, waza, attacker, attacker );
      break;
    case WAZADATA_CATEGORY_DAMAGE_EFFECT:
      scproc_Fight_Damage_AddEffect( wk, waza, attacker, defender );
      break;
    case WAZADATA_CATEGORY_DAMAGE_SICK:
      scproc_Fight_Damage_AddSick( wk, waza, attacker, defender );
      break;
    }
  }
}


//--------------------------------------------------------------------------
/**
 * ワザによるダメージ後の各種効果
 */
//--------------------------------------------------------------------------
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk )
{
  switch( evwk->reaction ){
  case BTL_EV_AFTER_DAMAGED_REACTION_SICK:        ///< 対象ポケモンを状態異常にする
    scproc_WazaDamageReaction_Sick( wk, evwk );
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_RANK:        ///< 対象ポケモンのランク増減効果
    scproc_WazaDamageReaction_Rank( wk, evwk );
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE:      ///< 対象ポケモンにダメージを与える
    scproc_WazaDamageReaction_Damage( wk, evwk );
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE:    ///< 対象ポケモンのタイプ変更
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_NONE:
    break;
  default:
    GF_ASSERT(0);
    break;
  }
}

static void scproc_WazaDamageReaction_Sick( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk )
{
  BTL_POKEPARAM* targetPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, evwk->targetPokeID );
  BTL_POKEPARAM* damagedPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, evwk->damagedPokeID );

  if( !scEvent_AddSick_IsMustFail(wk, targetPoke, evwk->sick) )
  {
    u16 sc_reserved_pos;
    u8  add_succeed_flag;
    u8  sc_reserved_flag = FALSE;

    if( evwk->tokFlag )
    {
      sc_reserved_pos = SCQUE_RESERVE_Pos( wk->que, SC_TOKWIN_IN );
      sc_reserved_flag = TRUE;
    }

    add_succeed_flag = scproc_AddSick( wk, targetPoke, damagedPoke, evwk->sick, evwk->sickCont, FALSE, NULL );

    if( add_succeed_flag )
    {
      if( sc_reserved_flag )
      {
        SCQUE_PUT_ReservedPos( wk->que, sc_reserved_pos, SC_TOKWIN_IN, evwk->damagedPokeID );
        SCQUE_PUT_TOKWIN_OUT( wk->que, evwk->damagedPokeID );
      }
    }
  }
}

static void scproc_WazaDamageReaction_Rank( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk )
{
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, evwk->targetPokeID );
  u16 sc_reserved_pos;
  u8 sc_reserved_flag;
  u8 succeed;

  if( evwk->tokFlag ){
    sc_reserved_pos = SCQUE_RESERVE_Pos( wk->que, SC_TOKWIN_IN );
    sc_reserved_flag = TRUE;
  }

  succeed = scproc_RankEffectCore( wk, target, evwk->rankType, evwk->rankVolume, ITEM_DUMMY_DATA, TRUE, TRUE );

  if( sc_reserved_flag && succeed ){
    SCQUE_PUT_ReservedPos( wk->que, sc_reserved_pos, SC_TOKWIN_IN, evwk->damagedPokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, evwk->damagedPokeID );
  }
}

static void scproc_WazaDamageReaction_Damage( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk )
{
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, evwk->targetPokeID );
  u16 sc_reserved_pos;
  u8 sc_reserved_flag;
  u8 succeed;

  if( evwk->tokFlag ){
    sc_reserved_pos = SCQUE_RESERVE_Pos( wk->que, SC_TOKWIN_IN );
    sc_reserved_flag = TRUE;
  }

  succeed = scproc_SimpleDamage( wk, target, evwk->damage, STRID_NULL );

  if( succeed ){
    SCQUE_PUT_ReservedPos( wk->que, sc_reserved_pos, SC_TOKWIN_IN, evwk->damagedPokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, evwk->damagedPokeID );
  }
}

//--------------------------------------------------------------------------
/**
 * [proc] 状態異常共通処理
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fAlmost            失敗した時に原因メッセージを表示する
 * @param   msgCallbackParam   成功時、メッセージ表示コマンド生成するためのコールバック。
 *                             デフォルトメッセージで良ければNULL。
 *
 * @retval  BOOL       成功した場合TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, MSG_CALLBACK_PARAM* msgCallbackParam )
{
  BOOL fSucceed;

  fSucceed = addsick_core( wk, target, attacker, sick, sickCont, fAlmost );
  if( fSucceed )
  {
    if( msgCallbackParam == NULL ){
      scPut_AddSickDefaultMsg( wk, target, sick, sickCont );
    }else{
      msgCallbackParam->func( wk, target, msgCallbackParam->arg );
    }

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      u16 itemID = scEvent_AddSick_Fix( wk, target, attacker, sick, sickCont );
      scproc_HandEx_Root( wk, itemID );
      Hem_PopState( &wk->HEManager, hem_state );
    }
  }

  return fSucceed;
}
//--------------------------------------------------------------------------
/**
 * 状態異常コア部分
 *
 * @param   wk
 * @param   target            状態異常をくらうポケ
 * @param   sick              状態異常ID
 * @param   sickCont          状態異常継続パラメータ
 * @param   fAlmost           ほぼ確定フラグ
 *
 * @retval  BOOL    状態異常にした場合、TRUE
 */
//--------------------------------------------------------------------------
static BOOL addsick_core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost  )
{
  BtlAddSickFailCode  fail_code;

  fail_code = addsick_check_fail( wk, target, sick, sickCont );

  if( fail_code != BTL_ADDSICK_FAIL_NULL )
  {
    if( fAlmost ){
      scPut_AddSickFail( wk, target, fail_code, sick );
    }
    return FALSE;
  }
  else
  {
    BOOL failFlag = scEvent_AddSick_CheckFail( wk, target, sick );

    if( !failFlag ){
      scPut_AddSick( wk, target, sick, sickCont );
    }
    else if( fAlmost )
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_AddSick_Failed( wk, target, sick );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
//    scPut_AddSickFail( wk, target, BTL_ADDSICK_FAIL_NULL, evwk->discardSickType );
    }
    return !failFlag;
  }
}
static BtlAddSickFailCode addsick_check_fail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT sickCont )
{
  // てんこう「はれ」の時に「こおり」にはならない
  if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI)
  ){
    return BTL_ADDSICK_FAIL_OTHER;
  }
  // すでに混乱なら混乱にならないとか…
  if( BTL_POKEPARAM_CheckSick(target, sick) ){
    return BTL_ADDSICK_FAIL_ALREADY;
  }

  // すでにポケモン系状態異常になっているなら、新たにポケモン系状態異常にはならない
  if( sick < POKESICK_MAX )
  {
    if( BTL_POKEPARAM_GetPokeSick(target) != POKESICK_NULL ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // @@@ すでに混乱なら混乱にならないとか… 失敗コードを返す必要があるかも
  if( BTL_POKEPARAM_CheckSick(target, sick) ){
    return BTL_ADDSICK_FAIL_ALREADY;
  }

  return BTL_ADDSICK_FAIL_NULL;
}
static BOOL scEvent_AddSick_IsMustFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
  // てんこう「はれ」の時に「こおり」にはならない
  if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI )
  ){
    return TRUE;
  }
  // すでにポケモン系状態異常になっているなら、新たにポケモン系状態異常にはならない
  if( (BTL_POKEPARAM_GetPokeSick(target) != POKESICK_NULL)
  &&  (sick < POKESICK_MAX)
  ){
    return TRUE;
  }

  // @@@ すでに混乱なら混乱にならないとか… 失敗コードを返す必要があるかも
  if( BTL_POKEPARAM_CheckSick(target, sick) ){
    return TRUE;
  }

  return FALSE;
}
static BOOL scEvent_AddSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_CHECKFAIL );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}
static u16 scEvent_AddSick_Fix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont )
{
  u16 itemID = ITEM_DUMMY_DATA;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    if( attacker ){
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    }else{
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEID_NULL );
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont.raw );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ITEM, itemID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_FIX );
    itemID = BTL_EVENTVAR_GetValue( BTL_EVAR_ITEM );
  BTL_EVENTVAR_Pop();

  return itemID;
}
static void scEvent_AddSick_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_FAILED );
  BTL_EVENTVAR_Pop();
}

//------------------------------------------------------------------
// サーバーフロー：アイテム使用
//------------------------------------------------------------------
static void scproc_UseItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  if( scEvent_CheckItemUseEnable(wk, bpp) )
  {
    u32 itemID = BTL_POKEPARAM_GetItem( bpp );
    u32 hem_state = Hem_PushState( &wk->HEManager );

    scEvent_GetItemEquip( wk, bpp );

    if( ITEM_CheckNuts(itemID) ){
      scPut_EatNutsAct( wk, bpp );
    }

    scproc_HandEx_Root( wk, itemID );
    Hem_PopState( &wk->HEManager, hem_state );

    if( ITEM_CheckNuts(itemID) ){
      scPut_RemoveItem( wk, bpp );
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * [Put] きのみを食べるアクション
 */
//----------------------------------------------------------------------------------
static void scPut_EatNutsAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_ACT_KINOMI( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Put] アイテム使用によるHP回復メッセージ
 */
//----------------------------------------------------------------------------------
static void scPut_UseItem_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID )
{
  u16 strID = BTL_STRID_SET_UseItem_RecoverHP;
  if( itemID == ITEM_TABENOKOSI ){
    strID = BTL_STRID_SET_UseItem_RecoverLittle;
  }
  SCQUE_PUT_MSG_SET( wk->que, strID, BTL_POKEPARAM_GetID(bpp), itemID );
}
//----------------------------------------------------------------------------------
/**
 * [Put] 所持アイテムを削除
 */
//----------------------------------------------------------------------------------
static void scPut_RemoveItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_HANDLER_ITEM_Remove( bpp );
  BTL_POKEPARAM_RemoveItem( bpp );
  SCQUE_PUT_OP_RemoveItem( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Event] アイテム使用効果の取得
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_GetItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_USE_ITEM );
  BTL_EVENTVAR_Pop();
}

//------------------------------------------------------------------
// サーバーフロー：ダメージ受け後の処理
//------------------------------------------------------------------
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  scproc_Fight_Damage_Drain( wk, waza, attacker, targets );
  scproc_Fight_Damage_After_Shrink( wk, waza, attacker, targets );

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );

    scEvent_AfterDamage( wk, attacker, targets, waza );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//------------------------------------------------------------------
// サーバーフロー：ダメージ受け後の処理 > ひるみチェック
//------------------------------------------------------------------
static void scproc_Fight_Damage_After_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;
  u32 cnt, i=0;

  while( (bpp = TargetPokeRec_Get(targets, i++)) != NULL )
  {
    if( !BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_ACTION_DONE) )
    {
      if( scEvent_CheckShrink(wk, bpp, attacker, waza) )
      {
        BTL_POKEPARAM_SetTurnFlag( bpp, BPP_TURNFLG_SHRINK );
      }
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザダメージに応じたHP吸い取り効果
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  u32 total_damage = 0;
  u32 hem_state;

  if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_DRAIN )
  {
    BTL_POKEPARAM* bpp;
    u32 damage;
    u32 i;
    u32 total_recover = 0;

    hem_state = Hem_PushState( &wk->HEManager );
    TargetPokeRec_GetStart( targets );
    while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
    {
      damage = TargetPokeRec_GetDamage( targets, bpp );
      total_damage += damage;
      damage = scEvent_CalcDrainVolume( wk, waza, attacker, bpp, damage );
      if( damage == 0 ){
        continue;
      }else{
        total_recover += damage;
      }
    }

    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

    if( total_recover && (!BTL_POKEPARAM_IsDead(attacker)) )
    {
      scPut_SimpleHp( wk, attacker, total_recover );
      scPut_Message_Set( wk, attacker, BTL_STRID_SET_Drain );
    }
  }

  {
    hem_state = Hem_PushState( &wk->HEManager );
    scEvent_CheckSpecialDrain( wk, waza, attacker, total_damage );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
}

//------------------------------------------------------------------
// サーバーフロー下請け：１体分攻撃ヒットチェック
//------------------------------------------------------------------
static BOOL svflowsub_hitcheck_singular( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza )
{
  u8 defPokeID = BTL_POKEPARAM_GetID( defender );

  // 既に対象が死んでいる
  if( BTL_POKEPARAM_IsDead(defender) )
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, defPokeID );
    return FALSE;
  }
  // 命中率・回避率によるヒットチェックで外れた
  if( !scEvent_checkHit(wk, attacker, defender, waza) )
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, defPokeID );
    return FALSE;
  }
  return TRUE;
}
//------------------------------------------------------------------
// サーバーフロー下請け： たたかう > ダメージワザ系 > ダメージ値計算
//------------------------------------------------------------------
static u16 scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag )
{
  WazaDamageType dmgType = WAZADATA_GetDamageType( waza );
  u32 rawDamage = 0;


  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_TYPEAFF, typeAff );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
  BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE_TYPE, dmgType );

  {
    u16 atkPower, defGuard, wazaPower;

    // 各種パラメータから素のダメージ値計算
    wazaPower = scEvent_getWazaPower( wk, attacker, defender, waza, &wk->wazaParam );
    atkPower  = scEvent_getAttackPower( wk, attacker, defender, waza, criticalFlag );
    defGuard  = scEvent_getDefenderGuard( wk, attacker, defender, waza, &wk->wazaParam, criticalFlag );
    {
      u8 atkLevel = BTL_POKEPARAM_GetValue( attacker, BPP_LEVEL );
      rawDamage = wazaPower * atkPower * (atkLevel*2/5+2) / defGuard / 50;
    }
    rawDamage = (rawDamage * targetDmgRatio) >> FX32_SHIFT;
    // 天候補正
    {
      fx32 weatherDmgRatio = BTL_FIELD_GetWeatherDmgRatio( waza );
      if( weatherDmgRatio != BTL_CALC_DMG_WEATHER_RATIO_NONE )
      {
        u32 prevDmg = rawDamage;
        rawDamage = (rawDamage * weatherDmgRatio) >> FX32_SHIFT;
        BTL_Printf("天候による補正が発生, 補正率=%08x, dmg=%d->%d\n", weatherDmgRatio, prevDmg, rawDamage);
      }
    }
    // 素ダメージ値を確定
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, rawDamage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC1 );
    rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    BTL_Printf("威力:%d, こうげき:%d, ぼうぎょ:%d,  ... 素ダメ:%d\n",
        wazaPower, atkPower, defGuard, rawDamage );

    // クリティカルで２倍
    if( criticalFlag )
    {
      rawDamage *= 2;
    }
  }
  //ランダム補正
  {
    u16 ratio = 100 - GFL_STD_MtRand(16);
    rawDamage = (rawDamage * ratio) / 100;
    BTL_Printf("ランダム補正:%d%%  -> 素ダメ=%d\n", ratio, rawDamage);
  }
  // タイプ一致補正
  {
    fx32 ratio = scEvent_CalcTypeMatchRatio( wk, attacker, wk->wazaParam.wazaType );
    rawDamage = (rawDamage * ratio) >> FX32_SHIFT;
  }
  // タイプ相性計算
  rawDamage = BTL_CALC_AffDamage( rawDamage, typeAff );
  BTL_Printf("タイプ相性:%02d -> ダメージ値：%d\n", typeAff, rawDamage);
  // やけど補正
  if( (dmgType == WAZADATA_DMG_PHYSIC)
  &&  (BTL_POKEPARAM_GetPokeSick(attacker) == POKESICK_YAKEDO)
  ){
    rawDamage = (rawDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
  }

  if( rawDamage == 0 ){ rawDamage = 1; }

  BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );

  BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, rawDamage );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC2 );

  {
    fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    rawDamage = (rawDamage * ratio) >> FX32_SHIFT;
    BTL_Printf("ダメ受けポケモン=%d, ratio=%08x, Damage=%d -> %d\n",
          BTL_POKEPARAM_GetID(defender), ratio, BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE ), rawDamage);
  }

  // 最高で残りＨＰの範囲に収まるように最終補正
  if( rawDamage > BTL_POKEPARAM_GetValue(defender, BPP_HP) ){
    rawDamage = BTL_POKEPARAM_GetValue( defender, BPP_HP );
  }

  BTL_EVENTVAR_Pop();

  return rawDamage;
}
//---------------------------------------------------------------------------------------------
// ワザダメージ処理後の反動処理
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage )
{
  u32 damage = scEvent_CalcKickBack( wk, attacker, waza, wazaDamage );
  if( damage )
  {
    scproc_SimpleDamage( wk, attacker, damage, BTL_STRID_SET_ReactionDmg );
  }
}
//---------------------------------------------------------------------------------------------
// ワザ以外のダメージ共通処理
//---------------------------------------------------------------------------------------------
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, u16 strID )
{
  if( scEvent_CheckEnableSimpleDamage(wk, bpp, damage) )
  {
    int value = -damage;

    if( value )
    {
      scPut_SimpleHp( wk, bpp, value );
      BTL_POKEPARAM_SetTurnFlag( bpp, BPP_TURNFLG_DAMAGED );

      if( strID != STRID_NULL ){
        scPut_Message_Set( wk, bpp, strID );
      }

      if( scEvent_SimpleDamage_Reaction(wk, bpp) ){
        scproc_UseItem( wk, bpp );
      }

      scPut_CheckDeadCmd( wk, bpp );

      return TRUE;
    }
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------------
// ポケモンを強制的に瀕死にする
//---------------------------------------------------------------------------------------------
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_KillPokemon( wk, bpp, 0 );
  scPut_CheckDeadCmd( wk, bpp );
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：追加効果による状態異常
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  WazaSick sick;
  BPP_SICK_CONT sick_cont;
  u32 i=0;

  sick = scEvent_CheckAddSick( wk, waza, attacker, target, &sick_cont );
  if( sick != WAZASICK_NULL )
  {
    if( (!BTL_POKEPARAM_IsDead(target))
    &&  (!scEvent_AddSick_IsMustFail(wk, target, sick))
    ){
      scproc_Fight_WazaSickCore( wk,  attacker, target, waza, sick, sick_cont, FALSE );
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる直接の状態異常
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  BTL_POKEPARAM* target;
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  u32 i = 0;

  sick = WAZADATA_GetSick( waza );
  contParam = WAZADATA_GetSickCont( waza );

  if( TargetPokeRec_GetCount(targetRec) )
  {
    TargetPokeRec_GetStart( targetRec );
    while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
    {
      if( scEvent_checkHit(wk, attacker, target, waza) )
      {
        BPP_SICK_CONT sickCont;
        BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );
        scproc_Fight_WazaSickCore( wk, attacker, target, waza, sick, sickCont, TRUE );
      }
      else
      {
        scPut_WazaAvoid( wk, target, waza );
      }
    }
  }
  else
  {
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる状態異常共通
//---------------------------------------------------------------------------------------------
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
  if( scEvent_AddSick_IsMustFail(wk, target, sick) ){
    scPut_WazaFail( wk, attacker, waza );
    return FALSE;
  }

  return scproc_AddSick( wk, target, attacker, sick, sickCont, fAlmost, NULL );
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：追加効果によるランク効果
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  if( !BTL_POKEPARAM_IsDead(target) )
  {
    if(   scEvent_CheckAddRankEffectOccur(wk, waza, attacker, target) )
    {
      scproc_WazaRankEffect_Common( wk, waza, attacker, target, FALSE );
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる直接のランク効果
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  BTL_POKEPARAM* target;
  u32 i = 0;

  TargetPokeRec_GetStart( targetRec );
  while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
  {
    if( scEvent_checkHit(wk, attacker, target, waza) )
    {
      if( scproc_WazaRankEffect_Common(wk, waza, attacker, target, TRUE) )
      {
        wazaEff_SetTarget( wk, target );
      }
    }
    else
    {
      scPut_WazaAvoid( wk, target, waza );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * ワザによる（直接・追加共通）ランク増減効果の処理
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL    ランク増減効果が発生したらTRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost )
{
  BOOL ret = FALSE;
  u32 eff_cnt, i;

  eff_cnt = WAZADATA_GetRankEffectCount( waza );
  BTL_Printf("ランク効果の種類=%d\n", eff_cnt);
  for(i=0; i<eff_cnt; ++i)
  {
    WazaRankEffect  effect;
    int  volume;

    scEvent_GetWazaRankEffectValue( wk, waza, i, attacker, target, &effect, &volume );
    BTL_Printf("ランク効果 %d, vol=%d\n", effect, volume );
    if( scproc_RankEffectCore(wk, target, effect, volume, ITEM_DUMMY_DATA, fAlmost, TRUE) )
    {
      ret = TRUE;
    }
  }
  return ret;
}
//--------------------------------------------------------------------------
/**
 * ランク増減効果のコア（ワザ以外もこれを呼び出す）
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   itemID    アイテム使用によるランク増減ならアイテムID指定（それ以外は ITEM_DUMMY_DATA を指定）
 * @param   fAlmost   特殊要因で効果失敗した時、そのメッセージを表示する
 * @param   fStdMsg   標準メッセージを出力する（○○は××があがった！など）
 *
 * @retval  BOOL    ランク増減効果が発生したらTRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u16 itemID, BOOL fAlmost, BOOL fStdMsg )
{
  if( !BTL_POKEPARAM_IsRankEffectValid(target, effect, volume) ){
    if( fAlmost ){
      scPut_RankEffectLimit( wk, target, effect, volume );
      return FALSE;
    }
  }

  {
    BOOL ret = TRUE;

    if( scEvent_CheckRankEffectSuccess(wk, target, effect, volume) )
    {
      BTL_Printf("ランク効果発生：type=%d, volume=%d\n", effect, volume );
      scPut_RankEffect( wk, target, effect, volume, itemID, fStdMsg );
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        scEvent_RankEffect_Fix( wk, target, effect, volume );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
      }
    }
    else
    {
      BTL_Printf("ランク効果失敗した\n");
      if( fAlmost )
      {
        u32 hem_state = Hem_PushState( &wk->HEManager );
        scEvent_RankEffect_Failed( wk, target );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
      }
      ret = FALSE;
    }

    return ret;
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ランク効果＆状態異常を同時に与えるワザ
//---------------------------------------------------------------------------------------------
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  BPP_SICK_CONT sickCont;
  BTL_POKEPARAM* target;
  u32 i = 0;

  sick = WAZADATA_GetSick( waza );
  contParam = WAZADATA_GetSickCont( waza );
  BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );

  TargetPokeRec_GetStart( targetRec );
  while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
  {
    if( scEvent_checkHit(wk, attacker, target, waza) )
    {
      if( scproc_WazaRankEffect_Common(wk, waza, attacker, target, TRUE) ){
        wazaEff_SetTarget( wk, target );
      }
      if( scproc_Fight_WazaSickCore(wk, attacker, target, waza, sick, sickCont, TRUE) ){
        wazaEff_SetTarget( wk, target );
      }
    }
    else
    {
      scPut_WazaAvoid( wk, target, waza );
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：HP回復
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker )
{
  if( !BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KAIHUKUHUUJI) )
  {
    u32 recoverHP = scEvent_CalcRecoverHP( wk, waza, attacker );
    scPut_SimpleHp( wk, attacker, recoverHP );
  }
  // かいふくふうじで失敗
  else
  {
    u8 pokeID = BTL_POKEPARAM_GetID( attacker );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KaifukuFuji, pokeID, waza );
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：一撃ワザ処理
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* target;
  WazaSick sick;
  u32 i = 0;

  while( 1 )
  {
    target = TargetPokeRec_Get( targets, i++ );
    if( target == NULL ){ break; }
    if( !BTL_POKEPARAM_IsDead(target) )
    {
      u8 atkLevel = BTL_POKEPARAM_GetValue( attacker, BPP_LEVEL );
      u8 defLevel = BTL_POKEPARAM_GetValue( target, BPP_LEVEL );
      u8 targetPokeID = BTL_POKEPARAM_GetID( target );
      if( atkLevel < defLevel )
      {
        scput_WazaNoEffect( wk, target );
      }
      else
      {
        if( scEvent_IchigekiCheck(wk, attacker, target, waza) )
        {
          wazaEff_SetTarget( wk, target );
          scPut_Ichigeki( wk, target );
        }
        else
        {
          scPut_WazaAvoid( wk, target, waza );
        }
      }
    }
  }
}

// ポケモン系・ワザ系による状態異常化イベントの呼び分け
static void svflowsub_MakeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
    WazaSick sick, BPP_SICK_CONT contParam, BOOL fAlmost )
{
  if( sick < POKESICK_MAX ){
    scEvent_MakePokeSick( wk, receiver, attacker, sick, contParam, fAlmost );
  }else{
    scEvent_MakeWazaSick( wk, receiver, attacker, sick, contParam, fAlmost );
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ふきとばしワザ処理
//---------------------------------------------------------------------------------------------
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  PushOutEffect eff = check_pushout_effect( wk );

  if( eff == PUSHOUT_EFFECT_MUSTFAIL ){
    scPut_WazaFail( wk, attacker, waza );
  }else{

    BTL_POKEPARAM* target = TargetPokeRec_Get( targetRec, 0 );

    if( BTL_POKEPARAM_IsDead(target) ){
      scPut_WazaFail( wk, attacker, waza );
      return;
    }
    if( scEvent_CheckPushOutFail(wk, attacker, target) ){
      return;
    }
    if( !scEvent_checkHit(wk, attacker, target, waza) ){
      scPut_WazaAvoid( wk, attacker, waza );
      return;
    }

    {
      BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, BTL_POKEPARAM_GetID(target) );
      u8 clientID, posIdx;
      BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

      if( eff == PUSHOUT_EFFECT_CHANGE )
      {
        SVCL_WORK* clwk;
        clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
        if( BTL_PARTY_GetMemberCount(clwk->party) <= clwk->numCoverPos )
        {
          scPut_WazaFail( wk, attacker, waza );
          return;
        }
        else
        {
          u8 nextPokeIdx = get_pushout_nextpoke_idx( wk, clwk );
          SCQUE_PUT_ACT_MemberOut( wk->que, clientID, posIdx );
          scproc_MemberIn( wk, clientID, posIdx, nextPokeIdx, FALSE );
          scproc_AfterMemberIn( wk );
        }
      }
      else
      {
        SCQUE_PUT_ACT_MemberOut( wk->que, clientID, posIdx );
        wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
      }
    }
  }
}
//--------------------------------------------------------------------------
/**
 * 「ふきとばし」系のワザ効果をバトルのルール等から判定する
 *
 * @retval  BOOL  必ず失敗するケースでTRUE
 */
//--------------------------------------------------------------------------
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor( wk->mainModule );

  switch( rule ){
  case BTL_COMPETITOR_WILD:
    return (competitor == BTL_COMPETITOR_WILD)? PUSHOUT_EFFECT_ESCAPE : PUSHOUT_EFFECT_MUSTFAIL;

  default:
    return PUSHOUT_EFFECT_CHANGE;
  }
}
//--------------------------------------------------------------------------
/**
 * 「ふきとばし」系のワザで強制入れかえが発生した時、
 * 次に出すポケモンのパーティ内インデックスをランダムで決める
 *
 * @param   wk
 * @param   clwk
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------
static u8 get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk )
{
  const BTL_POKEPARAM* bpp;
  u32 i, count, members;
  u8 list[ BTL_PARTY_MEMBER_MAX ];

  members = BTL_PARTY_GetMemberCount( clwk->party );
  for(i=clwk->numCoverPos, count=0; i<members; ++i)
  {
    bpp = BTL_PARTY_GetMemberDataConst( clwk->party, i );
    if( !BTL_POKEPARAM_IsDead(bpp) ){
      list[count++] = i;
    }
  }
  GF_ASSERT( count );
  return clwk->numCoverPos + GFL_STD_MtRand( count );
}

//--------------------------------------------------------------------------
/**
 * [Event] ふきとばしワザ失敗チェック
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL      失敗ならTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_PUSHOUT );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}

//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる天候の変化
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Weather( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, const BTL_ACTION_PARAM* action )
{
  BtlWeather  weather = WAZADATA_GetWeather( waza );

  if( scproc_WeatherCore( wk, weather, BTL_WEATHER_TURN_DEFAULT ) ){
    wazaEff_SetNoTarget( wk );
  }
}
static BOOL scproc_WeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn )
{
  if( scEvent_CheckChangeWeather(wk, weather, &turn) )
  {
    BTL_FIELD_SetWeather( weather, turn );
    SCQUE_PUT_ACT_WeatherStart( wk->que, weather );

    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_AfterChangeWeather( wk, weather );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
    }
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：分類しきれないワザ
//---------------------------------------------------------------------------------------------
static void scput_Fight_Others( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;

  // 命中チェック -> 命中しなかった場合は targets から除外
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( !scEvent_checkHit(wk, attacker, bpp, waza) )
    {
      scPut_WazaAvoid( wk, bpp, waza );
      TargetPokeRec_Remove( targets, bpp );
    }
  }


  switch( waza ){
  case WAZANO_SUKIRUSUWAPPU:
    scput_Fight_Others_SkillSwap( wk, attacker, targets );
    break;
  default:
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      if( scEvent_UnCategoryWaza(wk, waza, attacker, targets) ){
        wazaEff_SetNoTarget( wk );
        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        Hem_PopState( &wk->HEManager, hem_state );
      }
      else{
        SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
      }
    }
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] 未分類ワザ処理コール
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   targets
 *
 * @retval  BOOL      反応したワザハンドラがあればTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  u16 sp_before = Hem_GetStackPtr( &wk->HEManager );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    {
      BTL_POKEPARAM* bpp;
      u8 cnt, i;
      cnt = TargetPokeRec_GetCount( targets );
      BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, cnt );
      for(i=0; i<cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BTL_POKEPARAM_GetID(bpp) );
      }
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
      if( cnt ){
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORY_WAZA );
      }else{
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORY_WAZA_NO_TARGET );
      }
    }
  BTL_EVENTVAR_Pop();

  {
    u16 sp_after = Hem_GetStackPtr( &wk->HEManager );
    return sp_after != sp_before;
  }
}
// スキルスワップ
static void scput_Fight_Others_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  BTL_POKEPARAM* target = TargetPokeRec_Get( targetRec, 0 );
  PokeTokusei atk_tok, tgt_tok;

  atk_tok = BTL_POKEPARAM_GetValue( attacker, BPP_TOKUSEI );
  tgt_tok = BTL_POKEPARAM_GetValue( target, BPP_TOKUSEI );

  if( (!BTL_CALC_TOK_CheckCant_Swap(atk_tok)) && (!BTL_CALC_TOK_CheckCant_Swap(tgt_tok)) )
  {
    u8 atkPokeID = BTL_POKEPARAM_GetID( attacker );
    u8 tgtPokeID = BTL_POKEPARAM_GetID( target );
    scPut_WazaEffectOn( wk, attacker, target, WAZANO_SUKIRUSUWAPPU );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_SkillSwap, atkPokeID );

    BTL_POKEPARAM_ChangeTokusei( attacker, tgt_tok );
    BTL_POKEPARAM_ChangeTokusei( target, atk_tok );
    BTL_HANDLER_TOKUSEI_Swap( attacker, target );

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, atkPokeID );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, tgtPokeID );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKILL_SWAP );
    BTL_EVENTVAR_Pop();
  }
  else
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_STD_WazaFail );
  }
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * ハンドラ反応スタック処理ルート
 *
 * @param   wk
 * @param   useItemID   アイテム使用による呼び出しの場合、そのアイテムID／それ以外、ITEM_DUMMY_DATA
 */
//----------------------------------------------------------------------------------
static void scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID )
{
  BTL_HANDEX_PARAM_HEADER* handEx_header;

  while( (handEx_header = Hem_ReadWork(&wk->HEManager)) != NULL )
  {
    BTL_Printf("ProcHandEX : ADRS=0x%p, type=%d\n", handEx_header, handEx_header->equip);
    switch( handEx_header->equip ){
    case BTL_HANDEX_TOKWIN_IN:      scproc_HandEx_TokWinIn( wk, handEx_header ); break;
    case BTL_HANDEX_TOKWIN_OUT:     scproc_HandEx_TokWinOut( wk, handEx_header ); break;
    case BTL_HANDEX_USE_ITEM:       scproc_HandEx_useItem( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_HP:     scproc_HandEx_recoverHP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DAMAGE:         scproc_HandEx_damage( wk, handEx_header ); break;
    case BTL_HANDEX_SHIFT_HP:       scproc_HandEx_shiftHP( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_PP:     scproc_HandEx_recoverPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DECREMENT_PP:   scproc_HandEx_decrementPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_CURE_SICK:      scproc_HandEx_cureSick( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_ADD_SICK:       scproc_HandEx_addSick( wk, handEx_header ); break;
    case BTL_HANDEX_RANK_EFFECT:    scproc_HandEx_rankEffect( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_RECOVER_RANK:   scproc_HandEx_recoverRank( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_RANK:     scproc_HandEx_resetRank( wk, handEx_header ); break;
    case BTL_HANDEX_KILL:           scproc_HandEx_kill( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_TYPE:    scproc_HandEx_changeType( wk, handEx_header ); break;
    case BTL_HANDEX_MESSAGE_SET:    scproc_HandEx_messageSet( wk, handEx_header ); break;
    case BTL_HANDEX_MESSAGE_STD:    scproc_HandEx_messageStd( wk, handEx_header ); break;
    case BTL_HANDEX_SET_TURNFLAG:   scproc_HandEx_setTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_TURNFLAG: scproc_HandEx_resetTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SET_CONTFLAG:   scproc_HandEx_setContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_CONTFLAG: scproc_HandEx_resetContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SIDE_EFFECT:    scproc_HandEx_sideEffect( wk, handEx_header ); break;
    case BTL_HANDEX_TOKUSEI_CHANGE: scproc_HandEx_tokuseiChange( wk, handEx_header ); break;
    default:
      GF_ASSERT_MSG(0, "illegal handEx type = %d, userPokeID=%d", handEx_header->equip, handEx_header->userPokeID);
    }
  }
}
static void scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_TokWin_In( wk, pp_user );
}
static void scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_TokWin_Out( wk, pp_user );
}

static void scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scproc_UseItem( wk, pp_user );
}
static void scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RECOVER_HP* param = (BTL_HANDEX_PARAM_RECOVER_HP*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( !BTL_POKEPARAM_IsHPFull(pp_target) )
  {
    if( param_header->tokwin_flag ){
      scPut_TokWin_In( wk, pp_user );
    }

    scPut_SimpleHp( wk, pp_target, param->recoverHP );
    if( itemID != ITEM_DUMMY_DATA ){
      scPut_UseItem_RecoverHP( wk, pp_target, itemID );
    }

    if( param_header->tokwin_flag ){
      scPut_TokWin_Out( wk, pp_user );
    }
  }
}
static void scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DAMAGE* param = (BTL_HANDEX_PARAM_DAMAGE*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u16 strID = (param->fSucceedStrEx)? param->succeedStrID : STRID_NULL;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );

    scproc_SimpleDamage( wk, pp_target, param->damage[i], strID );
  }
}
static void scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SHIFT_HP* param = (BTL_HANDEX_PARAM_SHIFT_HP*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    scPut_SimpleHp( wk, pp_target, param->volume[i] );
  }
}


static void scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  if( !BTL_POKEPARAM_IsPPFull(pp_target, param->wazaIdx) ){
    scPut_RecoverPP( wk, pp_target, param->wazaIdx, param->volume, itemID );
  }

  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }
}

static void scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;

  BTL_POKEPARAM* targetPokemon = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  u8 restPP = BTL_POKEPARAM_GetPP( targetPokemon, param->wazaIdx );
  u8 volume = ( param->volume < restPP )? param->volume : restPP;

  scPut_DecrementPP( wk, targetPokemon, param->wazaIdx, volume );
}

static void scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_CURE_SICK* param = (BTL_HANDEX_PARAM_CURE_SICK*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  {
    BTL_POKEPARAM* pp_target;
    WazaSick cured_sick;
    u32 i;
    for(i=0; i<param->poke_cnt; ++i){
      pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
      cured_sick = scPut_CureSick( wk, pp_target, param->sickCode );
      if( !param->fStdMsgDisable ){
        scPut_CureSickMsg( wk, pp_target, cured_sick, itemID );
      }
    }
  }

  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }
}
static void scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = (BTL_HANDEX_PARAM_ADD_SICK*)param_header;
  BTL_POKEPARAM* pp_user = (param_header->userPokeID != BTL_POKEID_NULL)?
                      BTL_POKECON_GetPokeParam(wk->pokeCon, param_header->userPokeID) : NULL;

  BTL_POKEPARAM* pp_target;
  MSG_CALLBACK_PARAM  cbParam;
  u32 i;

  cbParam.func = handex_addsick_msg;
  cbParam.arg = (void*)param;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    scproc_AddSick( wk, pp_target, pp_user, param->sickID, param->sickCont, param->fAlmost, &cbParam );
  }
}
// 状態異常成功時のメッセージ表示コールバック
static void handex_addsick_msg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, void* arg )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = arg;

  if( param->fExMsg ){
    scPut_Message_SetEx( wk, target, param->exStrID, param->exStrArgCnt, param->exStrArgs );
  }else{
    scPut_AddSickDefaultMsg( wk, target, param->sickID, param->sickCont );
  }
}

static void scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RANK_EFFECT* param = (BTL_HANDEX_PARAM_RANK_EFFECT*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BTL_POKEPARAM* pp_target;
  u32 i;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    scproc_RankEffectCore( wk, pp_target, param->rankType, param->rankVolume, itemID, param->fAlmost, !(param->fStdMsgDisable) );
  }
}
static void scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RECOVER_RANK* param = (BTL_HANDEX_PARAM_RECOVER_RANK*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BTL_POKEPARAM_RankRecover( pp_user );
}
static void scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RESET_RANK* param = (BTL_HANDEX_PARAM_RESET_RANK*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    BTL_POKEPARAM_RankReset( pp_target );
  }
}

static void scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_KILL* param = (BTL_HANDEX_PARAM_KILL*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  scproc_KillPokemon( wk, pp_target );
}
static void scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TYPE* param= (const BTL_HANDEX_PARAM_CHANGE_TYPE*)param_header;

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  PokeType type_pure = PokeTypePair_GetType1( param->next_type );

  SCQUE_PUT_OP_ChangePokeType( wk->que, param->pokeID, param->next_type );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChangePokeType, param->pokeID, type_pure );
  BTL_POKEPARAM_ChangePokeType( bpp, param->next_type );
}
static void scproc_HandEx_messageSet( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_MESSAGE* param = (const BTL_HANDEX_PARAM_MESSAGE*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  scPut_Message_SetEx( wk, bpp, param->strID, param->arg_cnt, param->args );
}

static void scproc_HandEx_messageStd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_MESSAGE* param = (const BTL_HANDEX_PARAM_MESSAGE*)(param_header);

  switch( param->arg_cnt ){
  case 0:
    SCQUE_PUT_MSG_STD( wk->que, param->strID );
    break;
  case 1:
    SCQUE_PUT_MSG_STD( wk->que, param->strID, param->args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_STD( wk->que, param->strID, param->args[0], param->args[1] );
    break;
  }
}
static void scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BTL_POKEPARAM_SetTurnFlag( bpp, param->flag );
}
static void scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BTL_POKEPARAM_ForceOffTurnFlag( bpp, param->flag );
}
static void scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  scPut_SetContFlag( wk, bpp, param->flag );
}
static void scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  scPut_ResetContFlag( wk, bpp, param->flag );
}
static void scproc_HandEx_sideEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDE_EFFECT* param = (const BTL_HANDEX_PARAM_SIDE_EFFECT*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
}
static void scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TOKUSEI_CHANGE* param = (const BTL_HANDEX_PARAM_TOKUSEI_CHANGE*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BTL_HANDLER_TOKUSEI_Remove( bpp );
  SCQUE_PUT_OP_ChangeTokusei( wk->que, param->pokeID, param->tokuseiID );
  BTL_POKEPARAM_ChangeTokusei( bpp, param->tokuseiID );

  if( param->tokuseiID != POKETOKUSEI_NULL )
  {
    BTL_HANDLER_TOKUSEI_Add( bpp );
  }
}



//==============================================================================
// サーバーフロー：ターンチェックルート
//==============================================================================
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk )
{
  scproc_turncheck_sub( wk, BTL_EVENT_TURNCHECK_BEGIN );
  scproc_turncheck_sick( wk );
  scproc_turncheck_side( wk );
  scproc_turncheck_weather( wk );
  scproc_turncheck_sub( wk, BTL_EVENT_TURNCHECK_END );


  // 全ポケモンのターンフラグをクリア
  {
    FRONT_POKE_SEEK_WORK  fps;
    BTL_POKEPARAM* bpp;

    FRONT_POKE_SEEK_InitWork( &fps, wk );
    while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) ){
      BTL_POKEPARAM_TurnCheck( bpp );
    }
  }

  if( wk->turnCount < BTL_TURNCOUNT_MAX ){
    wk->turnCount++;
  }
}
static void scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BtlEventType event_type )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  scEvent_TurnCheck( wk, event_type );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, BtlEventType event_type )
{
  BTL_EVENT_CallHandlers( wk, event_type );
}

//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > 状態異常
//------------------------------------------------------------------
static void scproc_turncheck_sick( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK  fpsw;
  BTL_POKEPARAM* bpp;
  WazaSick sick;
  int damage;
  u8 pokeID;

  FRONT_POKE_SEEK_InitWork( &fpsw, wk );

  while( FRONT_POKE_SEEK_GetNext( &fpsw, wk, &bpp ) )
  {
    pokeID = BTL_POKEPARAM_GetID( bpp );
    for( sick=WAZASICK_ORIGIN; sick<WAZASICK_MAX; ++sick )
    {
      damage = BTL_POKEPARAM_CalcSickDamage( bpp, sick );
      if( damage )
      {
        BTL_EVENTVAR_Push();
          BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
          BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
          BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
          BTL_EVENT_CallHandlers( wk, BTL_EVENT_SICK_DAMAGE );
          damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
        BTL_EVENTVAR_Pop();
        if( damage > 0 )
        {
          BTL_Printf("ポケ[%d]が状態異常(%d)で、%dのダメージ発生\n", pokeID, sick, damage );
          BTL_POKEPARAM_HpMinus( bpp, damage );
          SCQUE_PUT_OP_HpMinus( wk->que, pokeID, damage );
          SCQUE_PUT_SickDamage( wk->que, pokeID, sick, damage );
        }
        else if( damage < 0 )
        {
          int recover = -damage;
          BTL_POKEPARAM_HpPlus( bpp, recover );
          SCQUE_PUT_OP_HpPlus( wk->que, pokeID, recover );
          SCQUE_PUT_ACT_SimpleHP( wk->que, pokeID );
        }
      }
    }
    BTL_POKEPARAM_WazaSick_TurnCheck( bpp );
    SCQUE_PUT_OP_WazaSickTurnCheck( wk->que, pokeID );
    scPut_CheckDeadCmd( wk, bpp );
  }
}
//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > サイドエフェクト
//------------------------------------------------------------------
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk )
{
  BTL_HANDLER_SIDE_TurnCheck( scproc_turncheck_side_callback, wk );
}
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg )
{
  BTL_SVFLOW_WORK* wk = arg;
  int strID = -1;
  switch( sideEffect ){
  case BTL_SIDEEFF_REFRECTOR:     strID = BTL_STRID_STD_ReflectorOff; break;
  case BTL_SIDEEFF_HIKARINOKABE:  strID = BTL_STRID_STD_HikariNoKabeOff; break;
  case BTL_SIDEEFF_SINPINOMAMORI: strID = BTL_STRID_STD_SinpiNoMamoriOff; break;
  case BTL_SIDEEFF_SIROIKIRI:     strID = BTL_STRID_STD_SiroiKiriOff; break;
  case BTL_SIDEEFF_OIKAZE:        strID = BTL_STRID_STD_OikazeOff; break;
  case BTL_SIDEEFF_OMAJINAI:      strID = BTL_STRID_STD_OmajinaiOff; break;
  }
  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( wk->que, strID, side );
  }
}

//------------------------------------------------------------------
// サーバーフロー下請け： ターンチェック > 天候
//------------------------------------------------------------------
static void scproc_turncheck_weather( BTL_SVFLOW_WORK* wk )
{
  BtlWeather weather = BTL_FIELD_TurnCheckWeather();
  if( weather != BTL_WEATHER_NONE )
  {
    SCQUE_PUT_ACT_WeatherEnd( wk->que, weather );
    return;
  }
  else
  {
    FRONT_POKE_SEEK_WORK  fps;
    u8   pokeID[ BTL_POS_MAX ];
    u8   tok_pokeID[ BTL_POS_MAX ];
    s16  damage[ BTL_POS_MAX ];
    BTL_POKEPARAM* bpp;
    s16 dmg_tmp;
    u8 poke_cnt = 0;
    u8 tok_poke_cnt = 0;
    u8 tok_flg;

    weather = BTL_FIELD_GetWeather();
    FRONT_POKE_SEEK_InitWork( &fps, wk );
    while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
    {
      if( !BTL_POKEPARAM_IsDead(bpp) )
      {
        dmg_tmp = scEvent_CalcWeatherDamage( wk, weather, bpp, &tok_flg );
        if( dmg_tmp )
        {
          if( tok_flg )
          {
            tok_pokeID[ tok_poke_cnt++ ] = BTL_POKEPARAM_GetID( bpp );
          }
          damage[ poke_cnt ] = dmg_tmp;
          pokeID[ poke_cnt ] = BTL_POKEPARAM_GetID( bpp );
          ++poke_cnt;
        }
      }
    }
    // ダメージ反映コマンド発行の前後に、とくせいによるケースはウィンドウ表示コマンド
    if( poke_cnt )
    {
      u8 i;
      for(i=0; i<tok_poke_cnt; ++i)
      {
        SCQUE_PUT_TOKWIN_IN( wk->que, tok_pokeID[i] );
      }

      for(i=0; i<poke_cnt; ++i){
        bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID[i] );
        if( damage[i] >= 0 ){
          BTL_POKEPARAM_HpMinus( bpp, damage[i] );
          SCQUE_PUT_OP_HpMinus( wk->que, pokeID[i], damage[i] );
        }else{
          BTL_POKEPARAM_HpPlus( bpp, -damage[i] );
          SCQUE_PUT_OP_HpPlus( wk->que, pokeID[i], -damage[i] );
        }
      }
      SCQUE_PUT_ACT_WeatherDamage( wk->que, weather, poke_cnt );
      for(i=0; i<poke_cnt; ++i)
      {
        SCQUE_PUT_ArgOnly( wk->que, pokeID[i] );
      }

      for(i=0; i<tok_poke_cnt; ++i)
      {
        SCQUE_PUT_TOKWIN_OUT( wk->que, tok_pokeID[i] );
      }
      // 死亡チェック
      for(i=0; i<poke_cnt; ++i)
      {
        scPut_CheckDeadCmd( wk, BTL_POKECON_GetPokeParam(wk->pokeCon, pokeID[i]) );
      }
    }
  }
}
static int scEvent_CalcWeatherDamage( BTL_SVFLOW_WORK* wk, BtlWeather weather, BTL_POKEPARAM* bpp, u8 *tok_cause_flg )
{
  int default_damage = BTL_CALC_RecvWeatherDamage( bpp, weather );
  int damage;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE,  default_damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_WEATHER_DAMAGE );
    damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    *tok_cause_flg = ( default_damage != damage );
  BTL_EVENTVAR_Pop();
  return damage;
}

//---------------------------------------------------------------------------------------------
//  数値関連関数群
//---------------------------------------------------------------------------------------------
static inline int roundValue( int val, int min, int max )
{
  if( val < min ){ val = min; }
  if( val > max ){ val = max; }
  return val;
}
static inline int roundMin( int val, int min )
{
  if( val < min ){ val = min; }
  return val;
}
static inline int roundMax( int val, int max )
{
  if( val > max ){ val = max; }
  return val;
}
static inline BOOL perOccur( u8 per )
{
  return GFL_STD_MtRand(100) < per;
}
//---------------------------------------------------------------------------------------------
//  イベントワーク確保
//---------------------------------------------------------------------------------------------
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack )
{
  stack->sp = 0;
}
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size )
{
  GF_ASSERT( stack->sp < EVENT_WORK_DEPTH );

  while( size % 4 ){ ++size; }

  {
    u32 usedSize;
    u32 i;

    for(i=0, usedSize=0; i<stack->sp; ++i)
    {
      usedSize += stack->size[ i ];
    }

    if( (usedSize + size) < sizeof(stack->work) )
    {
      void* adrs;
      adrs = &stack->work[ usedSize ];
      stack->size[ stack->sp ] = size;
      stack->sp++;
      return adrs;
    }
    else
    {
      GF_ASSERT(0);
      return NULL;
    }
  }
}

static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs )
{
  GF_ASSERT( stack->sp );

  stack->sp--;

  {
    u32 used_size, i;

    for(i=0, used_size=0; i<stack->sp; ++i)
    {
      used_size += stack->size[ i ];
    }

    if( adrs != &(stack->work[used_size]) )
    {
      GF_ASSERT(0); // 最後に確保されたワーク以外の領域が先に解放されようとしている
    }
  }
}
//---------------------------------------------------------------------------------------------
//
// コマンド出力層
// サーバーコマンドをバッファに生成 ＆ 必要ならそれに伴うポケモンパラメータ操作を行う
// イベント層の関数呼び出しは行われない
//
//---------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * [Put] 「○○は　はんどうでうごけない」表示
 */
//--------------------------------------------------------------------------
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, u8 clientID, u8 pokeIdx )
{
  SVCL_WORK* clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  BTL_POKEPARAM* bpp = clwk->member[ pokeIdx ];

  scPut_Message_Set( wk, bpp, BTL_STRID_SET_CantAction );
}
//--------------------------------------------------------------------------
/**
 * 「○○はこんらんしている」表示
 */
//--------------------------------------------------------------------------
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAct, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * こんらんによる自爆ダメージ処理
 */
//--------------------------------------------------------------------------
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  BTL_POKEPARAM_HpMinus( bpp, damage );
  SCQUE_PUT_OP_HpMinus( wk->que, pokeID, damage );
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_KonranExe, pokeID );
  SCQUE_PUT_ACT_ConfDamage( wk->que, pokeID );

  scPut_CheckDeadCmd( wk, bpp );
}
//--------------------------------------------------------------------------
/**
 * ワザ出し判定時のポケモン系状態異常回復処理
 */
//--------------------------------------------------------------------------
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  BTL_POKEPARAM_CurePokeSick( bpp );
  SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );

  switch( sick ){
  case POKESICK_NEMURI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriWake, pokeID );
    break;
  case POKESICK_KOORI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriMelt, pokeID );
    break;
  default:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_PokeSick_Cure, pokeID );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * ワザメッセージ表示（○○の××こうげき！など）
 */
//--------------------------------------------------------------------------
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_WAZA( wk->que, BTL_POKEPARAM_GetID(attacker), waza );
}
//----------------------------------------------------------------------------------
/**
 * [Put]状態異常回復処理のコマンド生成
 *
 * @param   wk
 * @param   bpp       対象ポケモン
 * @param   sick_id   状態異常コード
 * @param   itemID  アイテム使用による回復ならアイテムID／それ以外は ITEM_DUMMY_DATA
 */
//----------------------------------------------------------------------------------
static WazaSick scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWazaSickEx exSickCode )
{
  WazaSick sick = trans_sick_code( bpp, exSickCode );

  if( sick != WAZASICK_NULL )
  {
    u8 pokeID = BTL_POKEPARAM_GetID( bpp );

    if( sick < POKESICK_MAX ){
      BTL_POKEPARAM_CurePokeSick( bpp );
      SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
    }else{
      BTL_POKEPARAM_CureWazaSick( bpp, sick );
      SCQUE_PUT_OP_CureWazaSick( wk->que, pokeID, sick );
    }
  }
  return sick;
}
//----------------------------------------------------------------------------------
/**
 * [Put]状態異常回復処理の標準メッセージ出力コマンド生成
 *
 * @param   wk
 * @param   bpp
 * @param   sick
 * @param   itemID
 */
//----------------------------------------------------------------------------------
static void scPut_CureSickMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick, u16 itemID )
{
  BOOL fUseItem = (itemID != ITEM_DUMMY_DATA);
  int strID = getCureSickStrID( sick, fUseItem );
  if( strID >= 0 )
  {
    u8 pokeID = BTL_POKEPARAM_GetID( bpp );
    if( fUseItem ){
      SCQUE_PUT_MSG_SET( wk->que, strID, pokeID, itemID );
    }else{
      SCQUE_PUT_MSG_SET( wk->que, strID, pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 拡張状態異常コードを、該当するコードに変換する
 *
 * @param   bpp       対象ポケモン
 * @param   exCode
 *
 * @retval  WazaSick  該当コード／該当コードなしの場合、WAZASICK_NULLを返す
 */
//----------------------------------------------------------------------------------
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx exCode )
{
  WazaSick result = WAZASICK_NULL;
  PokeSick poke_sick = BTL_POKEPARAM_GetPokeSick( bpp );

  // 拡張コード変換
  if( (exCode == WAZASICK_EX_POKEFULL) || (exCode == WAZASICK_EX_POKEFULL_PLUS) )
  {
    // ポケモン系状態異常になっているならそれを治す
    if( poke_sick != WAZASICK_NULL ){
      result = poke_sick;
    }
    // それ以外は混乱のみ治す
    else{
      if( (exCode == WAZASICK_EX_POKEFULL_PLUS)
      &&  (BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KONRAN))
      ){
        result = WAZASICK_KONRAN;
      }
    }
  }
  // 通常コード時は罹患していないポケ系状態異常を指定されたらNULLを返す
  else{
    result = exCode;

    if( (poke_sick != POKESICK_NULL)
    &&  (result < POKESICK_MAX)
    &&  (result != poke_sick)
    ){
      result = WAZASICK_NULL;
    }
  }

  return result;
}
//----------------------------------------------------------------------------------
/**
 * 状態異常回復時のメッセージID取得
 *
 * @param   sick        状態異常コード
 * @param   fUseItem    道具の使用による回復ならTRUE
 *
 * @retval  int   メッセージID（該当メッセージが存在しない場合 -1）
 */
//----------------------------------------------------------------------------------
static int getCureSickStrID( WazaSick sick, BOOL fUseItem )
{
  static const struct {
    WazaSick  sick;
    s16       strID_notItem;
    s16       strID_useItem;
  }dispatchTbl[] = {
    { WAZASICK_DOKU,      BTL_STRID_SET_PokeSick_Cure,    BTL_STRID_SET_UseItem_CureDoku    },
    { POKESICK_YAKEDO,    BTL_STRID_SET_PokeSick_Cure,    BTL_STRID_SET_UseItem_CureYakedo  },
    { WAZASICK_NEMURI,    BTL_STRID_SET_NemuriWake,       BTL_STRID_SET_UseItem_CureNemuri  },
    { WAZASICK_KOORI,     BTL_STRID_SET_KoriMelt,         BTL_STRID_SET_UseItem_CureKoori   },
    { WAZASICK_MAHI,      BTL_STRID_SET_MahiCure,         BTL_STRID_SET_UseItem_CureMahi    },

    { WAZASICK_KONRAN,    -1,                             BTL_STRID_SET_UseItem_CureKonran  },
    { WAZASICK_MEROMERO,  -1,                             BTL_STRID_SET_UseItem_CureMero    },
  };

  u32 i;

  for(i=0; i<NELEMS(dispatchTbl); ++i){
    if( dispatchTbl[i].sick == sick ){
      return (fUseItem)? dispatchTbl[i].strID_useItem : dispatchTbl[i].strID_notItem;
    }
  }

  return -1;
}

//--------------------------------------------------------------------------
/**
 * ワザ出し失敗メッセージ表示
 */
//--------------------------------------------------------------------------
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  switch( cause ){
  case SV_WAZAFAIL_NEMURI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriAct, pokeID );
    break;
  case SV_WAZAFAIL_MAHI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MahiAct, pokeID );
    break;
  case SV_WAZAFAIL_KOORI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriAct, pokeID );
    break;
  case SV_WAZAFAIL_SHRINK:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ShrinkExe, pokeID );
    break;
  case SV_WAZAFAIL_NAMAKE:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Namake, pokeID );
    break;
  case SV_WAZAFAIL_TYOUHATSU:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChouhatuWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_ICHAMON:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_IchamonWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_KAIHUKUHUUJI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KaifukuFuji, pokeID, waza );
    break;
  case SV_WAZAFAIL_HPFULL:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HPFull, pokeID );
    break;
  case SV_WAZAFAIL_FUMIN:
    SCQUE_PUT_TOKWIN_IN( wk->que, pokeID );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Fumin, pokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, pokeID );
    break;
  default:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail, pokeID );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * 連続ワザ成功カウンタをクリア
 */
//--------------------------------------------------------------------------
static void scPut_ResetSameWazaCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_POKEPARAM_ResetUsedWazaNumber( bpp );
  SCQUE_PUT_OP_ResetUsedWazaCounter( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * 「しかしうまくきまらなかった」表示
 */
//--------------------------------------------------------------------------
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
}
//--------------------------------------------------------------------------
/**
 * 「○○には当たらなかった」表示
 */
//--------------------------------------------------------------------------
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, BTL_POKEPARAM_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * 「○○にはこうかがないようだ」表示
 */
//--------------------------------------------------------------------------
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * [Put] ダメージ受け->回復化処理
 */
//--------------------------------------------------------------------------
static void scput_DmgToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BTL_EVWK_DMG_TO_RECOVER* evwk )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  if( evwk->tokFlag ){
    SCQUE_PUT_TOKWIN_IN( wk->que, pokeID );
  }

  if( evwk->recoverHP != 0 )
  {
    wazaEff_SetTarget( wk, bpp );
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_SET_HP_Recover, pokeID );
    BTL_POKEPARAM_HpPlus( bpp, evwk->recoverHP );
    SCQUE_PUT_OP_HpPlus( wk->que, pokeID, evwk->recoverHP );
    SCQUE_PUT_ACT_SimpleHP( wk->que, evwk->pokeID );
  }
  else
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
  }

  if( evwk->tokFlag ){
    SCQUE_PUT_TOKWIN_OUT( wk->que, evwk->pokeID );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] 対象ポケモンが死んでいたら死亡処理＆必要コマンド生成
 */
//--------------------------------------------------------------------------
static void scPut_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke )
{
  u8 pokeID = BTL_POKEPARAM_GetID( poke );

  if( wk->pokeDeadFlag[pokeID] == 0 )
  {
    if( BTL_POKEPARAM_IsDead(poke) )
    {
      wk->pokeDeadFlag[pokeID] = 1;

      BTL_HANDLER_TOKUSEI_Remove( poke );
      BTL_HANDLER_ITEM_Remove( poke );
      BTL_HANDLER_Waza_RemoveForceAll( poke );
//      BTL_HANDLER_Waza_RemoveForce( poke );

      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, pokeID );
      SCQUE_PUT_ACT_Dead( wk->que, pokeID );
      BTL_POKEPARM_DeadClear( poke );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ワザエフェクト発動
 */
//--------------------------------------------------------------------------
static void scPut_WazaEffectOn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza )
{
  // １つのワザ処理ごとに１回しか作らない
  if( FlowFlg_Get(wk, FLOWFLG_SET_WAZAEFFECT ) == FALSE )
  {
    u8 atPokeID = BTL_POKEPARAM_GetID( attacker );
    u8 defPokeID = BTL_POKEPARAM_GetID( target );
    BTL_Printf("ワザエフェクトコマンド生成しますよ, ワザナンバ%d\n", waza);
    SCQUE_PUT_ACT_WazaEffect( wk->que, atPokeID, defPokeID, waza );
    FlowFlg_Set( wk, FLOWFLG_SET_WAZAEFFECT );
  }
  else
  {
    BTL_Printf("ワザエフェクトコマンド生成済みなので無視, ワザナンバ%d\n", waza);
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ワザによるダメージ（単体）
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage,
  BOOL critical_flag, BOOL plural_hit_flag )
{
  u8 pokeID = BTL_POKEPARAM_GetID( defender );

  // 複数回ヒットは最後に１回だけ「効果は○○だ！」を出すため、ここでは無効化
  if( plural_hit_flag ){
    aff = BTL_TYPEAFF_100;
  }

  BTL_POKEPARAM_HpMinus( defender, damage );
  SCQUE_PUT_OP_HpMinus( wk->que, pokeID, damage );
  SCQUE_PUT_ACT_WazaDamage( wk->que, pokeID, aff, damage );
  if( critical_flag )
  {
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_CriticalHit );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ワザによるダメージ（複数体）
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, u32 poke_cnt, BtlTypeAff aff,
  BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag )
{
  u32 i;

  for(i=0; i<poke_cnt; ++i)
  {
    BTL_POKEPARAM_HpMinus( bpp[i], damage[i] );
    SCQUE_PUT_OP_HpMinus( wk->que, BTL_POKEPARAM_GetID(bpp[i]), damage[i] );
  }

  SCQUE_PUT_ACT_WazaDamagePlural( wk->que, poke_cnt, aff );
  for(i=0; i<poke_cnt; ++i)
  {
    SCQUE_PUT_ArgOnly( wk->que, BTL_POKEPARAM_GetID(bpp[i]) );
  }
  for(i=0; i<poke_cnt; ++i)
  {
    if( critical_flag[i] ){
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_CriticalHit, BTL_POKEPARAM_GetID(bpp[i]) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] ワザダメージを「こらえた」時のメッセージ表示
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scPut_Koraeru( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BppKoraeruCause cause )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  switch( cause ){
  case BPP_KORAE_WAZA_DEFENDER:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Koraeru_Exe, pokeID );
    break;
  case BPP_KORAE_ITEM:
    {
      u16 itemID = BTL_POKEPARAM_GetItem( bpp );
      if( itemID ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoraeItem, pokeID, itemID );
      }
    }
  }
}
static void scPut_WazaDamageAffMsg( BTL_SVFLOW_WORK* wk, BtlTypeAff aff )
{
//  scPut_WazaDamageSingle( wk, defender, aff, damage, critical_flag );
}
//--------------------------------------------------------------------------
/**
 * [Put] ワザによるダメージを与えた後の反動計算
 */
//--------------------------------------------------------------------------
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage )
{
  u8 pokeID = BTL_POKEPARAM_GetID( attacker );
  u8 ratio = WAZADATA_GetReactionRatio( waza );
  u8 ratio_ex = 0;
  u8 fail_flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO_EX, ratio_ex );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, pokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_KICKBACK );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    ratio_ex = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO_EX );
  BTL_EVENTVAR_Pop();

  if( fail_flag )
  {
    ratio = 0;
  }
  ratio += ratio_ex;

  if( ratio )
  {
    damage = roundMin( (damage*ratio)/100, 1 );
    return damage;
  }
  return 0;
}
//--------------------------------------------------------------------------
/**
 * [Put] とくせいウィンドウ表示開始
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_IN( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] とくせいウィンドウ消去開始
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_OUT( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] これ以上、能力ランクがあがらない（さがらない）メッセージ
 */
//--------------------------------------------------------------------------
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume )
{
  u8 pokeID = BTL_POKEPARAM_GetID( target );

  if( volume > 0 ){
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_RankupMax_ATK, pokeID, effect );
  }else{
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_RankdownMin_ATK, pokeID, effect );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ランク増減効果が発生
 */
//--------------------------------------------------------------------------
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, BOOL fStdMsg )
{
  u8 pokeID = BTL_POKEPARAM_GetID( target );

  if( volume > 0 )
  {
    volume = BTL_POKEPARAM_RankUp( target, effect, volume );
    SCQUE_PUT_OP_RankUp( wk->que, pokeID, effect, volume );
    SCQUE_PUT_ACT_RankUp( wk->que, pokeID, effect, volume );
    if( fStdMsg )
    {
      if( itemID == ITEM_DUMMY_DATA ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankup_ATK, pokeID, effect, volume );
      }else{
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_Rankup_ATK, pokeID, effect, volume, itemID );
      }
    }
  }
  else
  {
    volume *= -1;
    volume = BTL_POKEPARAM_RankDown( target, effect, volume );
    BTL_Printf(" RANK DOWN vol=%d\n", volume);
    SCQUE_PUT_OP_RankDown( wk->que, pokeID, effect, volume );
    SCQUE_PUT_ACT_RankDown( wk->que, pokeID, effect, volume );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankdown_ATK, pokeID, effect, volume );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] 一撃必殺ワザ成功
 */
//--------------------------------------------------------------------------
static void scPut_Ichigeki( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target )
{
  u8 pokeID = BTL_POKEPARAM_GetID( target );
  BTL_POKEPARAM_HpZero( target );
  SCQUE_PUT_OP_HpZero( wk->que, pokeID );
  SCQUE_PUT_ACT_WazaIchigeki( wk->que, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] シンプルHP増減エフェクト
 */
//--------------------------------------------------------------------------
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  if( value > 0 )
  {
    BTL_POKEPARAM_HpPlus( bpp, value );
    SCQUE_PUT_OP_HpPlus( wk->que, pokeID, value );
  }
  else if( value < 0 )
  {
    value *= -1;
    BTL_POKEPARAM_HpMinus( bpp, value );
    SCQUE_PUT_OP_HpMinus( wk->que, pokeID, value );
  }
  SCQUE_PUT_ACT_SimpleHP( wk->que, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] シンプルHP増減エフェクト
 */
//--------------------------------------------------------------------------
static void scPut_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 effType )
{
  u16 minusHP = BTL_POKEPARAM_GetValue( bpp, BPP_HP );
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  BTL_POKEPARAM_HpMinus( bpp, minusHP );
  SCQUE_PUT_OP_HpMinus( wk->que, pokeID, minusHP );
  SCQUE_PUT_ACT_Kill( wk->que, pokeID, effType );
}
//--------------------------------------------------------------------------
/**
 * [Put] 状態異常化
 */
//--------------------------------------------------------------------------
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont )
{
  u8 pokeID = BTL_POKEPARAM_GetID( target );

  BTL_POKEPARAM_SetWazaSick( target, sick, sickCont );
  SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );
//  SCQUE_PUT_ACT_SickSet( wk->que, pokeID, sick );
}
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick )
{
  u8 pokeID = BTL_POKEPARAM_GetID( target );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示
 */
//--------------------------------------------------------------------------
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );
  SCQUE_PUT_MSG_SET( wk->que, strID, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示（引数拡張）
 */
//--------------------------------------------------------------------------
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID, u32 argCnt, const int* args )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );
  switch( argCnt ){
  case 1:
  default:
    SCQUE_PUT_MSG_SET( wk->que, strID, pokeID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_SET( wk->que, strID, pokeID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_SET( wk->que, strID, pokeID, args[0], args[1], args[2] );
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] 使ったワザのPPデクリメント
 *
 * @param   wk
 * @param   attacker
 * @param   wazaIdx
 * @param   vol
 */
//----------------------------------------------------------------------------------
static void scPut_DecrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol )
{
  u8 pokeID = BTL_POKEPARAM_GetID( attacker );
  BTL_POKEPARAM_PPMinus( attacker, wazaIdx, vol );
  SCQUE_PUT_OP_PPMinus( wk->que, pokeID, wazaIdx, vol );
}
//----------------------------------------------------------------------------------
/**
 * [Put] PP回復
 *
 * @param   wk
 * @param   bpp
 * @param   wazaIdx
 * @param   volume
 * @param   itemID    アイテムを使った効果の場合はアイテムID
 */
//----------------------------------------------------------------------------------
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, u16 itemID )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  BTL_POKEPARAM_PPPlus( bpp, wazaIdx, volume );
  SCQUE_PUT_OP_PPPlus( wk->que, pokeID, wazaIdx, volume );
  if( itemID != ITEM_DUMMY_DATA ){
    WazaID waza = BTL_POKEPARAM_GetWazaNumber( bpp, wazaIdx );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_RecoverPP, pokeID, itemID, waza );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] 状態異常メッセージセット
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   sickCont
 */
//----------------------------------------------------------------------------------
static void scPut_AddSickDefaultMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaSick sickID, BPP_SICK_CONT sickCont )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );
  u16 msgID;
  switch( sickID ){
  case WAZASICK_YAKEDO: msgID = BTL_STRID_SET_YakedoGet; break;
  case WAZASICK_MAHI:   msgID = BTL_STRID_SET_MahiGet; break;
  case WAZASICK_KOORI:  msgID = BTL_STRID_SET_KoriGet; break;
  case WAZASICK_NEMURI: msgID = BTL_STRID_SET_NemuriGet; break;
  case WAZASICK_KONRAN: msgID = BTL_STRID_SET_KonranGet; break;
  case WAZASICK_AKUMU:  msgID = BTL_STRID_SET_AkumuGet;  break;
  case WAZASICK_DOKU:
    if( BPP_SICKCONT_IsMoudokuCont(sickCont) ){
      msgID = BTL_STRID_SET_MoudokuGet;
    }else{
      msgID = BTL_STRID_SET_DokuGet;
    }
    break;
  default:
    return;
  }

  SCQUE_PUT_MSG_SET( wk->que, msgID, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Put]アクション毎フラグセット
 *
 * @param   wk
 * @param   bpp
 * @param   flagID
 */
//----------------------------------------------------------------------------------
static void scPut_ActFlag_Set( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppActFlag flagID )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );
  BTL_POKEPARAM_SetActFlag( bpp, flagID );
  SCQUE_PUT_OP_SetActFlag( wk->que, pokeID, flagID );
}
//----------------------------------------------------------------------------------
/**
 * [Put]アクション毎フラグクリア
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scPut_ActFlag_Clear( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );
  BTL_POKEPARAM_ClearActFlag( bpp );
  SCQUE_PUT_OP_ClearActFlag( wk->que, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Put] 継続フラグセット
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_SetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag )
{
  BTL_POKEPARAM_SetContFlag( bpp, flag );
  SCQUE_PUT_OP_SetContFlag( wk->que, BTL_POKEPARAM_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] 継続フラグクリア
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_ResetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag )
{
  BTL_POKEPARAM_ResetContFlag( bpp, flag );
  SCQUE_PUT_OP_ResetContFlag( wk->que, BTL_POKEPARAM_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] ターンフラグセット
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_SetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag )
{
  BTL_POKEPARAM_SetTurnFlag( bpp, flag );
  SCQUE_PUT_OP_SetTurnFlag( wk->que, BTL_POKEPARAM_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] ターンフラグセット
 *
 * @param   wk
 * @param   bpp
 * @param   flag
 */
//----------------------------------------------------------------------------------
static void scPut_ResetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag )
{
  BTL_POKEPARAM_ForceOffTurnFlag( bpp, flag );
  SCQUE_PUT_OP_ResetTurnFlag( wk->que, BTL_POKEPARAM_GetID(bpp), flag );
}

//---------------------------------------------------------------------------------------------
//
// 【イベント層】
//
// メイン処理層での判定に必要な各種条件の決定を行う。
// ハンドラ類の呼び出しはイベント層からのみ行われる。
// ポケモンパラメータの操作は行わない。
// コマンド出力は行わない。
//
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * [Event] “たたかう”を選んだときの特殊優先度を判定
 *
 * @param   wk
 * @param   attacker
 * @param   spePriA     [out] 特殊優先度Aを受け取る（ワザより高い判定レベル）
 * @param   spePriB     [out] 特殊優先度Bを受け取る（ワザより低い判定レベル）
 *
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8* spePriA, u8* spePriB )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SP_PRIORITY_A, BTL_SPPRI_A_DEFAULT );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SP_PRIORITY_B, BTL_SPPRI_B_DEFAULT );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_SP_PRIORITY );
    *spePriA = BTL_EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY_A );
    *spePriB = BTL_EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY_B );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] すばやさ取得
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  u16 agi = BTL_POKEPARAM_GetValue( attacker, BPP_AGILITY );
  BTL_Printf("素の素早さを表示する poke=%d, agi=%d\n", BTL_POKEPARAM_GetID(attacker), agi);
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE ); // まひチェックフラグとして使っている
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1f), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_AGILITY );
    agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    {
      fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      agi = (agi * ratio) >> FX32_SHIFT;
    }
    if( BTL_POKEPARAM_GetPokeSick(attacker) == POKESICK_MAHI )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
      {
        agi = (agi * BTL_MAHI_AGILITY_RATIO) / 100;
      }
    }
  BTL_EVENTVAR_Pop();
  BTL_Printf("返ってきた素早さは %d だった\n", agi);
  return agi;
}
//--------------------------------------------------------------------------
/**
 * [Event] こんらんダメージ計算
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16   ダメージ値
 */
//--------------------------------------------------------------------------
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  u16 dmg = roundMin( BTL_POKEPARAM_GetValue(attacker, BPP_MAX_HP)/8, 1 );

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_CONF_DMG, dmg );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_CONF_DAMAGE );
  dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_CONF_DMG );

  BTL_EVENTVAR_Pop();
  return dmg;
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザ出し失敗判定 ＆ ねむり・まひ・こおりの回復チェック
 *
 * @param   wk
 * @param   attacker    攻撃ポケモンパラメータ
 * @param   waza        出そうとしているワザ
 *
 * @retval  SV_WazaFailCause
 */
//--------------------------------------------------------------------------
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  WazaSick sick = BTL_POKEPARAM_GetPokeSick( attacker );
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;
  u8 pokeID = BTL_POKEPARAM_GetID( attacker );

  do {

    // 状態異常による失敗チェック
    switch( sick ){
    case POKESICK_NEMURI:
      cause = SV_WAZAFAIL_NEMURI;
      break;
    case POKESICK_MAHI:
      if( BTL_CALC_IsOccurPer(BTL_MAHI_EXE_PER) ){
       cause = SV_WAZAFAIL_MAHI;
      }
      break;
    case POKESICK_KOORI:
      cause = SV_WAZAFAIL_KOORI;
      break;
    }
    if( cause != SV_WAZAFAIL_NULL ){  break; }

    // ひるみによる失敗チェック
    if( BTL_POKEPARAM_GetTurnFlag(attacker, BPP_TURNFLG_SHRINK) ){
      cause = SV_WAZAFAIL_SHRINK;
      break;
    }
    // ワザロックによる失敗チェック
    if( (BTL_POKEPARAM_CheckSick(attacker, WAZASICK_WAZALOCK)
       ||  BTL_POKEPARAM_CheckSick(attacker, WAZASICK_WAZALOCK_HARD) )
    &&  (BTL_POKEPARAM_GetPrevWazaNumber(attacker) != waza)
    ){
      cause = SV_WAZAFAIL_WAZALOCK;
      break;
    }
    // ちょうはつによる失敗チェック
    if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_TYOUHATSU)
    &&  !WAZADATA_IsDamage(waza)
    ){
      cause = SV_WAZAFAIL_TYOUHATSU;
      break;
    }
    // いちゃもんによる失敗チェック
    if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_ICHAMON)
    &&  (BTL_POKEPARAM_GetPrevWazaNumber(attacker) == waza)
    ){
      cause = SV_WAZAFAIL_ICHAMON;
      break;
    }
    // ふういんによる失敗チェック
    if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_FUUIN) )
    {
      u8 targetPokeID = BTL_POKEPARAM_GetSickParam( attacker, WAZASICK_FUUIN );
      BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, targetPokeID );
      if( BTL_POKEPARAM_GetWazaIdx(target, waza) != PTL_WAZA_MAX ){
        cause = SV_WAZAFAIL_FUUIN;
        break;
      }
    }

  }while(0);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, cause );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_CHECK );
    cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
  BTL_EVENTVAR_Pop();
  return cause;

}
//--------------------------------------------------------------------------
/**
 * [Event] 発動するワザのパラメータを構造体にセット
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   [out] param
 *
 */
//--------------------------------------------------------------------------
static void scEvent_CheckWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, WAZADATA_GetType(waza) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_USER_TYPE, BTL_POKEPARAM_GetPokeType(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_PARAM );
    param->wazaID = waza;
    param->wazaType = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    param->userType = BTL_EVENTVAR_GetValue( BTL_EVAR_USER_TYPE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザだし失敗時の反応チェック
 *
 * @param   wk
 * @param   bpp
 * @param   waza
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, cause );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_FAIL );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ出し成功確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExecuteFix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
//    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_FIX );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザターゲット決定
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaTarget( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u8 pokeID = BTL_POKEID_NULL;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECIDE_TARGET );
    pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BTL_EVENTVAR_Pop();
  return pokeID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザは出したが効果が無かった
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExe_NoEffect( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_NO_EFFECT );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターンスキップ判定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL    溜めターンスキップする場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckTameTurnSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_TAMETURN_SKIP );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターン発動確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_START );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 溜めターン発動確定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_RELEASE );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] 出したワザが対象に当たるか判定（一撃必殺以外のポケモン対象ワザ）
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_checkHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  if( IsMustHit(attacker, defender) ){
    return TRUE;
  }

  if( scEvent_CheckPokeHideAvoid(wk, attacker, defender, waza) ){
    return FALSE;
  }

  if( scEvent_IsExcuseCalcHit(wk, attacker, defender, waza) ){
    return TRUE;
  }

  {
    u8 wazaHitRatio, per;
    s8 hitRank, avoidRank, totalRank;

    wazaHitRatio = scEvent_getHitPer(wk, attacker, defender, waza);

    BTL_EVENTVAR_Push();

      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HIT_RANK, BTL_POKEPARAM_GetValue(attacker, BPP_HIT_RATIO) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_RANK, BTL_POKEPARAM_GetValue(defender, BPP_HIT_RATIO) );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RANK );

      hitRank = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_RANK );
      avoidRank = BTL_EVENTVAR_GetValue( BTL_EVAR_AVOID_RANK );

    BTL_EVENTVAR_Pop();

    totalRank = roundValue( BTL_CALC_HITRATIO_MID + avoidRank - hitRank, BTL_CALC_HITRATIO_MIN, BTL_CALC_HITRATIO_MAX );
    per = BTL_CALC_HitPer( wazaHitRatio, totalRank );

    return perOccur( per );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] そらをとぶ、ダイビング等、場から隠れているポケモンへのヒットチェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL    隠れているためヒットしない場合にTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckPokeHideAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  static const BppContFlag  hideState[] = {
    BPP_CONTFLG_SORAWOTOBU, BPP_CONTFLG_ANAWOHORU, BPP_CONTFLG_DIVING,
  };

  BOOL avoidFlag = FALSE;
  u32 i;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    for(i=0; i<NELEMS(hideState); ++i)
    {
      if( BTL_POKEPARAM_GetContFlag(defender, hideState[i]) )
      {
        BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_FLAG, TRUE );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKE_HIDE, hideState[i] );
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_POKE_HIDE );
        if( BTL_EVENTVAR_GetValue(BTL_EVAR_AVOID_FLAG) ){
          BTL_Printf("ポケ[%d], 状態[%d] なのでワザあたりません\n", BTL_POKEPARAM_GetID(defender), hideState[i]);
          avoidFlag = TRUE;
          break;
        }
      }
    }
  BTL_EVENTVAR_Pop();

  return avoidFlag;

}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザの命中チェック時、確率計算をスキップして命中させるかチェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL        確率計算をスキップする場合はTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_IsExcuseCalcHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL result = WAZADATA_IsAlwaysHit( waza );
  if( !result )
  {
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_EXCUSE_CALC_HIT );
      result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    BTL_EVENTVAR_Pop();
  }
  return result;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ的中率(0-100)取得
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  u8    的中率 (0-100)
 */
//--------------------------------------------------------------------------
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  u8 per = WAZADATA_GetHitRatio( waza );

  BTL_EVENTVAR_Push();
    if( BTL_FIELD_GetWeather() == BTL_WEATHER_MIST )
    {
      per = BTL_CALC_MulRatio( per, BTL_CALC_WEATHER_MIST_HITRATIO );
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RATIO );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  return per;
}
//--------------------------------------------------------------------------
/**
 * [Event] 出したワザが対象に当たるか判定（一撃必殺ワザ）
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL ret = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ICHIGEKI_CHECK );
    if( !BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_FLAG) )
    {
      // とくせい等による強制無効化を受けない場合、必中有効。
      if( IsMustHit(attacker, defender) ){
        ret = TRUE;
      }else{
        u8 per = WAZADATA_GetHitRatio( waza );
        u8 atLevel = BTL_POKEPARAM_GetValue( attacker, BPP_LEVEL );
        u8 defLevel = BTL_POKEPARAM_GetValue( defender, BPP_LEVEL );
        if( atLevel > defLevel )
        {
          per += (atLevel - defLevel);
          ret = perOccur( per );
        }
      }
    }
  BTL_EVENTVAR_Pop();
  return ret;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ無効チェック（タイプ相性による無効化）
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 *
 * @retval  BOOL    無効な場合TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckNotEffect_byType( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  PokeType waza_type = wk->wazaParam.wazaType;
  PokeTypePair def_type = BTL_POKEPARAM_GetPokeType( defender );
  BOOL fNoEffect = (BTL_CALC_TypeAff( waza_type, def_type ) == BTL_TYPEAFF_0);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_NOEFFECT_FLAG, fNoEffect );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NOEFFECT_TYPE_CHECK );
    fNoEffect = BTL_EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
  BTL_EVENTVAR_Pop();

  return fNoEffect;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ無効チェック（とくせい等、個別ケースによる無効化）
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 *
 * @retval  BOOL    無効な場合TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, u8 lv,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  BOOL fNoEffect = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_NOEFFECT_FLAG, fNoEffect );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NOEFFECT_CHECK_L1+lv );
    fNoEffect = BTL_EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
  BTL_EVENTVAR_Pop();

  return fNoEffect;
}
//--------------------------------------------------------------------------
/**
 * [Event] 特定タイプのダメージワザで回復してしまうポケの判定
 *
 * @param   wk
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL ret = FALSE;

/*
  evwk->pokeID = BTL_POKEPARAM_GetID( defender );
  evwk->recoverHP = 0;
  evwk->tokFlag = FALSE;
  evwk->rankEffect = WAZA_RANKEFF_NULL;
  evwk->rankVolume = 0;
*/
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
//    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wk->wazaParam.wazaType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, ret );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER_CHECK );
    ret = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return ret;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ダメージワザ回復化決定
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 */
//----------------------------------------------------------------------------------
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER_FIX );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] クリティカルヒット発生チェック
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL flag = FALSE;
  u16  rank = BTL_POKEPARAM_GetValue( attacker, BPP_CRITICAL_RATIO );
  rank += WAZADATA_GetCriticalRank( waza );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_RANK, rank );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CRITICAL_CHECK );
    if( !BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG ) )
    {
      rank = roundMax( BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_RANK), BTL_CALC_CRITICAL_MAX );
      flag = BTL_CALC_CheckCritical( rank );
    }
  BTL_EVENTVAR_Pop();
  return flag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによるダメージを「こらえる」かどうかチェック
 * ※「こらえる」= HPが1残る
 *
 * @param   wk
 * @param   defender    ワザダメージをくらうポケ
 * @param   damage      [io] ダメージ量（こらえた場合、HPが１残るように補正される）
 *
 * @retval  BppKoraeruCause   「こらえる」理由／こらえない場合は BPP_KORAE_NONE
 */
//----------------------------------------------------------------------------------
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, u16* damage )
{
  if( BTL_POKEPARAM_GetValue(defender, BPP_HP) > (*damage) )
  {
    return BPP_KORAE_NONE;
  }
  else
  {
    BppKoraeruCause  cause = BPP_KORAE_NONE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_KORAERU_CAUSE, cause );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_KORAERU_CHECK );
      cause = BTL_EVENTVAR_GetValue( BTL_EVAR_KORAERU_CAUSE );
    BTL_EVENTVAR_Pop();

    if( cause != BPP_KORAE_NONE ){
      *damage = BTL_POKEPARAM_GetValue(defender, BPP_HP) - 1;
    }

    return cause;
  }
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザダメージ受けた後の反応処理
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   aff
 * @param   damage
 * @param   criticalFlag
 *
 */
//--------------------------------------------------------------------------
static void scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff aff,
  u32 damage, BOOL criticalFlag )
{
  u8 defPokeID = BTL_POKEPARAM_GetID( defender );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defPokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TYPEAFF, aff );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wk->wazaParam.wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
//    BTL_EVENTVAR_SetValue( BTL_EVAR_ITEMUSE_FLAG, item_use_flag );
//    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザの相性計算
 *
 * @param   wk
 * @param   defender
 * @param   waza_type
 * @param   evwk        [out] 計算結果
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff scEvent_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender,
  PokeType waza_type, BTL_EVWK_CHECK_AFFINITY* evwk )
{
  PokeTypePair defPokeType = scEvent_getDefenderPokeType( wk, defender );
  evwk->aff = BTL_CALC_TypeAff( waza_type, defPokeType );
  evwk->weakedByItem = FALSE;
  evwk->weakReserveByItem = FALSE;
  evwk->weakedItemID = ITEM_DUMMY_DATA;


  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, waza_type );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)(evwk) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_AFFINITY );
  BTL_EVENTVAR_Pop();

  return evwk->aff;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ使用後のデクリメントするPP値を取得
 *
 * @param   wk
 * @param   attacker  ワザ使用ポケモン
 * @param   wazaIdx   使用ワザインデックス
 * @param   rec       ワザによってダメージを受けたポケモンパラメータ群
 *
 * @retval  u32       デクリメントPP値
 */
//----------------------------------------------------------------------------------
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u16 wazaIdx, TARGET_POKE_REC* rec )
{
  u32 volume = 1;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    {
      u8 i = 0;
      BTL_POKEPARAM* bpp;
      BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, TargetPokeRec_GetCount(rec) );
      while( (bpp=TargetPokeRec_Get(rec, i)) != NULL )
      {
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BTL_POKEPARAM_GetID(bpp) );
        ++i;
      }
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP_VOLUME );
    volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();

  return volume;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ使用後、PPをデクリメントした後の反応
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    アイテム使用反応があればTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_DecrementPP_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaIdx )
{
  BOOL flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, flag );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID,   BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_IDX, wazaIdx );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP_DONE );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return flag;
}
//--------------------------------------------------------------------------
/**
 * [Event] 複数回ヒットワザのチェック処理
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   hitCount    [out] 実行するヒット回数（ランダム）
 *
 * @retval  BOOL    複数回ヒットするワザならTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckPluralHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8* hitCount )
{
  u8 max = WAZADATA_GetMaxHitCount( waza );

  if( max > 1 )
  {
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HITCOUNT_MAX, max );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HITCOUNT, BTL_CALC_HitCountMax(max) );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_COUNT );
      *hitCount = BTL_EVENTVAR_GetValue( BTL_EVAR_HITCOUNT );
    BTL_EVENTVAR_Pop();
    return TRUE;
  }
  else
  {
    *hitCount = 1;
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ワザ威力取得
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   wazaParam
 *
 * @retval  u16   ワザ威力値
 */
//----------------------------------------------------------------------------------
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, SVFL_WAZAPARAM* wazaParam )
{
  u16 power = WAZADATA_GetPower( waza );
  fx32 ratio;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, power );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.0), FX32_CONST(0.1f), FX32_CONST(512) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER );
    power = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER_RATIO );
  BTL_EVENTVAR_Pop();

  power = ( power * ratio ) >> FX32_SHIFT;
  BTL_Printf("威力は%dだーよ\n", power);
  return power;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ以外のダメージ有効／無効チェック
 *
 * @param   wk
 * @param   bpp
 * @param   damage  ダメージ量
 *
 * @retval  BOOL    有効ならTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage )
{
  BOOL flag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SIMPLE_DAMAGE_ENABLE );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return flag;
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザ以外のダメージ後、反応チェック（現状、アイテム使用のみ）
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    アイテム使用する場合、TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_SimpleDamage_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL flag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ITEMUSE_FLAG, TRUE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SIMPLE_DAMAGE_REACTION );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_ITEMUSE_FLAG );
  BTL_EVENTVAR_Pop();

  return flag;
}



// 攻撃側能力値取得
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag )
{
  BppValueID vid;

  switch( WAZADATA_GetDamageType(waza) ){
  case WAZADATA_DMG_PHYSIC:   vid = BPP_ATTACK; break;
  case WAZADATA_DMG_SPECIAL:  vid = BPP_SP_ATTACK; break;
  default:
    GF_ASSERT(0);
    vid = BPP_ATTACK;
    break;
  }

  {
    u16 power;
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );    //
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER_PREV );
      if( BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG ) )
      {
        BTL_POKEPARAM_GetValue_Base( attacker, vid );
      }
      else
      {
        if( criticalFlag ){
          power = BTL_POKEPARAM_GetValue_Critical( attacker, vid );
        }else{
          power = BTL_POKEPARAM_GetValue( attacker, vid );
        }
      }
      BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, power );
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER );
      power = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
    BTL_EVENTVAR_Pop();
    return power;
  }
}

// 防御側能力値取得
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  WazaID waza, SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag )
{
  BppValueID vid;

  switch( WAZADATA_GetDamageType(waza) ){
  case WAZADATA_DMG_PHYSIC:   vid = BPP_DEFENCE; break;
  case WAZADATA_DMG_SPECIAL:  vid = BPP_SP_DEFENCE; break;
  default:
    GF_ASSERT(0);
    vid = BPP_DEFENCE;
    break;
  }

  {
    u16 guard;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD_PREV );

      if( BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG) )
      {
        guard = BTL_POKEPARAM_GetValue_Base( defender, vid );
      }
      else
      {
        if( criticalFlag ){
          guard = BTL_POKEPARAM_GetValue_Critical( defender, vid );
        }else{
          guard = BTL_POKEPARAM_GetValue( defender, vid );
        }
      }

      // てんこう「すなあらし」の時、いわタイプのとくぼう1.5倍
      if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SAND)
      &&  (BTL_POKEPARAM_IsMatchType(defender, POKETYPE_IWA))
      &&  (vid == BPP_SP_DEFENCE)
      ){
        guard = (guard * 192) / 128;
      }

      BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD );
      guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );

    BTL_EVENTVAR_Pop();
    return guard;
  }
}
//--------------------------------------------------------------------------
/**
 * [Event] 攻撃ポケとワザのタイプ一致補正率を計算
 *
 * @param   wk
 * @param   attacker
 * @param   waza_type
 *
 * @retval  fx32    補正率
 */
//--------------------------------------------------------------------------
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type )
{
  BOOL f_match = BTL_POKEPARAM_IsMatchType( attacker, waza_type );
  fx32 ratio = (f_match)? FX32_CONST(1.5f) : FX32_CONST(1);

  BTL_EVENTVAR_Push();
  {
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, f_match );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TYPEMATCH_RATIO );
    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  }
  BTL_EVENTVAR_Pop();
  return ratio;
}
// 防御側タイプ取得
static PokeTypePair scEvent_getDefenderPokeType( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_POKE_TYPE, BTL_POKEPARAM_GetPokeType(defender) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(defender) );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_TYPE );

  {
    PokeTypePair type = BTL_EVENTVAR_GetValue( BTL_EVAR_POKE_TYPE );
    BTL_EVENTVAR_Pop();
    return type;
  }
}

// メンバーバトル場から退出
static void scEvent_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, posIdx );
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_OUT );
    // ここで死ぬこともある
    if( !BTL_POKEPARAM_IsDead(bpp) )
    {
      SCQUE_PUT_ACT_MemberOut( wk->que, clientID, posIdx );
    }
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_MemberOut1, pokeID );
  BTL_EVENTVAR_Pop();

  BTL_HANDLER_TOKUSEI_Remove( bpp );
}
//--------------------------------------------------------------------------
/**
 * [Event] メンバーを場に登場させる
 *
 * @param   wk
 * @param   bpp
 *
 */
//--------------------------------------------------------------------------
static void scEvent_MemberIn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_IN );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ追加効果により対象ポケモンをひるませるか判定
 *
 * @param   wk
 * @param   target    対象ポケモンデータ
 * @param   attacker  攻撃ポケモンデータ
 * @param   waza      使ったワザ
 *
 * @retval  BOOL    ひるませる場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaID waza )
{
  u32 per = WAZADATA_GetShrinkPer( waza );
  BOOL fail_flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, fail_flag );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_CHECK );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  if( fail_flag ){
    return FALSE;
  }else{
    return perOccur(per);
  }
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザ追加効果による状態異常の発生チェック
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 * @param   [out] pSickCont   継続効果
 *
 * @retval  WazaSick          状態異常ID
 */
//--------------------------------------------------------------------------
static WazaSick scEvent_CheckAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont )
{
  BPP_SICK_CONT sickCont;
  WazaSick sick = WAZADATA_GetSick( waza );
  WAZA_SICKCONT_PARAM  waza_contParam = WAZADATA_GetSickCont( waza );
  u8 per = WAZADATA_GetSickPer( waza );

  BTL_CALC_WazaSickContToBppSickCont( waza_contParam, attacker, &sickCont );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont.raw );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_SICK );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    sickCont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();

  if( sick != WAZASICK_NULL )
  {
    if( perOccur(per) ){
      *pSickCont = sickCont;
      return sick;
    }
  }
  return  WAZASICK_NULL;
}
//----------------------------------------------------------------------------------
/**
 * ダメージワザ処理後
 *
 * @param   wk
 * @param   attacker
 * @param   targets
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_AfterDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    {
      u32 i, cnt = TargetPokeRec_GetCount( targets );
      const BTL_POKEPARAM* bpp;
      BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, cnt );
      for(i=0; i<cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BTL_POKEPARAM_GetID(bpp) );
      }
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_AFTER_DAMAGE );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * 【Event】特定ポケモンをポケモン系の状態異常にする
 *
 * @param   wk
 * @param   receiver    状態異常を受ける対象のポケモンパラメータ
 * @param   attacker    状態異常を与える側のポケモンパラメータ
 * @param   sick        状態異常ID
 * @param   fAlmost     ほぼ確定フラグ（※）
 *
 * ※ほぼ確定フラグとは
 *  プレイヤーがほぼ確実に状態異常になるはず、と思う状況でTRUEにする。
 *  ワザ直接効果なら常にTRUE。追加効果なら効果確率100の時にTRUE。とくせい効果の場合は常にFALSE。
 *  これがTRUEの場合、状態異常を無効化するとくせいが発動した場合にそのことを表示するために使う。
 */
//--------------------------------------------------------------------------
static void scEvent_MakePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
  PokeSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
  // てんこう「はれ」の時に「こおり」にはならない
  if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI )
  ){
    return;
  }
  // すでにポケモン系状態異常になっているなら、新たにポケモン系状態異常にはならない
  if( BTL_POKEPARAM_GetPokeSick(receiver) != POKESICK_NULL )
  {
    return;
  }

  {
    BTL_EVENTVAR_Push();
      if( attacker )
      {
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
      }
      else
      {
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEID_NULL );
      }
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(receiver) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
      BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont.raw );
      BTL_EVENTVAR_SetValue( BTL_EVAR_ALMOST_FLAG, fAlmost );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_MAKE_POKESICK );
      sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
      if( sick != WAZASICK_NULL )
      {
        u8 pokeID = BTL_POKEPARAM_GetID( receiver );
        sickCont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
        BTL_POKEPARAM_SetWazaSick( receiver, sick, sickCont );
        SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );
        SCQUE_PUT_ACT_SickSet( wk->que, pokeID, sick );
      }
    BTL_EVENTVAR_Pop();
  }
}
//--------------------------------------------------------------------------
/**
 * 【Event】特定ポケモンをワザ系の状態異常にする
 *
 * @param   wk
 * @param   receiver    状態異常を受ける対象のポケモンパラメータ
 * @param   attacker    状態異常を与える側のポケモンパラメータ
 * @param   sick        状態異常ID
 * @param   fAlmost     ほぼ確定フラグ（※）
 *
 * ※ほぼ確定フラグとは
 *  プレイヤーがほぼ確実に状態異常になるはず、と思う状況でTRUEにする。
 *  ワザ直接効果なら常にTRUE。追加効果なら効果確率100の時にTRUE。とくせい効果の場合は常にFALSE。
 *  これがTRUEの場合、状態異常を無効化するとくせいが発動した場合にそのことを表示するために使う。
 */
//--------------------------------------------------------------------------
static void scEvent_MakeWazaSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
  BTL_EVENTVAR_Push();
    if( attacker )
    {
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    }
    else
    {
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEID_NULL );
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(receiver) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont.raw );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ALMOST_FLAG, fAlmost );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MAKE_WAZASICK );
    sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    if( sick != WAZASICK_NULL )
    {
      u8 pokeID = BTL_POKEPARAM_GetID( receiver );
      sickCont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
      BTL_POKEPARAM_SetWazaSick( receiver, sick, sickCont );
      SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );
//      SCQUE_PUT_ACT_SickSet( wk->que, pokeID, sick );
    }
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザによる追加ランク効果の発生チェック
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   waza
 *
 * @retval  BOOL    発生したらTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  u8 per = WAZADATA_GetRankEffPer( waza );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_RANK_TARGET );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  BTL_EVENTVAR_Pop();

  return perOccur(per);
}
//--------------------------------------------------------------------------
/**
 * [Event] ワザによる（直接・追加）ランク増減効果を取得
 *
 * @param   wk
 * @param   waza
 * @param   idx
 * @param   attacker
 * @param   target
 * @param   effect    [out] 効果種類
 * @param   volume    [out] 効果値
 *
 */
//--------------------------------------------------------------------------
static void scEvent_GetWazaRankEffectValue( BTL_SVFLOW_WORK* wk, WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume )
{
  *effect = WAZADATA_GetRankEffect( waza, idx, volume );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, *effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, *volume );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_GET_RANKEFF_VALUE );
    *volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] ランク増減効果の最終成否チェック
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 *
 * @retval  BOOL      成功する場合TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_LAST_CHECK );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return !failFlag;
//  return evwk->failFlag == FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ランク増減効果の失敗確定
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_RankEffect_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_FAILED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ランク増減効果の成功確定
 *
 * @param   wk
 * @param   bpp
 * @param   rankType
 * @param   volume
 */
//----------------------------------------------------------------------------------
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, rankType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_FIXED );
  BTL_EVENTVAR_Pop();
}

//--------------------------------------------------------------------------
/**
 * [Event] ドレイン攻撃のドレインHP量を計算
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   target
 * @param   damage    与えたダメージ
 *
 * @retval  int     回復量
 */
//--------------------------------------------------------------------------
static int scEvent_CalcDrainVolume( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, int damage )
{
  u32 volume = damage * WAZADATA_GetDrainRatio(waza) / 100;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_DRAIN );
    volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();
  return volume;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 特殊ドレイン
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   total_damage
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckSpecialDrain( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, u32 total_damage )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, total_damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_SPECIAL_DRAIN );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * 【Event】天候の変化チェック
 *
 * @param   wk
 * @param   weather 天候
 * @param   turn    [io]継続ターン数
 *
 * @retval  BOOL    変化する場合はTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8* turn )
{
  GF_ASSERT(weather != BTL_WEATHER_NONE);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, *turn );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE );
    weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );
    *turn = BTL_EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
  BTL_EVENTVAR_Pop();
  return (weather != BTL_WEATHER_NONE);
}
//----------------------------------------------------------------------------------
/**
 * 【Event】天候の変化後イベント
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
static void scEvent_AfterChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE_AFTER );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] アイテム使用可否チェック
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    TRUEで使用可能
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckItemUseEnable( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  if( BTL_POKEPARAM_GetItem(bpp) != ITEM_DUMMY_DATA )
  {
    BOOL flag = TRUE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, flag );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_USE_ITEM_ENABLE );
      flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    BTL_EVENTVAR_Pop();
    return flag;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザによるHP回復量計算
 *
 * @param   wk
 * @param   waza
 * @param   bpp
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_CalcRecoverHP( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp )
{
  u32 ratio = WAZADATA_GetRecoverHPRatio( waza );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RECOVER_HP_RATIO );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  {
    u32 volume = (BTL_POKEPARAM_GetValue(bpp, BPP_MAX_HP) * ratio) / 100;
    return volume;
  }
}


//----------------------------------------------------------------------------------------------------------
// 以下、ハンドラからの応答受信関数とユーティリティ群
//----------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
/**
 * 指定ポケIDを持つポケモンが戦闘に出ているかチェックし、出ていたらその戦闘位置を返す
 *
 * @param   server
 * @param   pokeID
 *
 * @retval  BtlPokePos    出ている場合は戦闘位置ID／出ていない場合はBTL_POS_MAX
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFLOW_CheckExistFrontPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  SVCL_WORK* clwk;
  int i;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk->numCoverPos )
    {
      int p;
      for(p=0; p<clwk->numCoverPos; ++p)
      {
        if( (clwk->frontMember[p] != NULL)
        &&  (BTL_POKEPARAM_GetID(clwk->frontMember[p]) == pokeID)
        ){
          return BTL_MAIN_GetClientPokePos( wk->mainModule, i, p );
        }
      }
    }
  }
  return BTL_POS_MAX;
}
//--------------------------------------------------------------------------------------
/**
 * 場に出ている全ポケモンのIDを配列に格納する
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    場に出ているポケモン数
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFLOW_RECEPT_GetAllFrontPokeID( BTL_SVFLOW_WORK* wk, u8* dst )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 cnt = 0;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    dst[ cnt++ ] = BTL_POKEPARAM_GetID( bpp );
  }
  return cnt;
}
//--------------------------------------------------------------------------------------
/**
 * 場に出ている全ての相手側ポケモンIDを配列に格納する
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    場に出ているポケモン数
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFLOW_RECEPT_GetAllOpponentFrontPokeID( BTL_SVFLOW_WORK* wk, u8 basePokeID, u8* dst )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 cnt = 0;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    if( !BTL_MAINUTIL_IsFriendPokeID( basePokeID, BTL_POKEPARAM_GetID(bpp) ) )
    {
      dst[ cnt++ ] = BTL_POKEPARAM_GetID( bpp );
    }
  }
  return cnt;
}

//--------------------------------------------------------------------------------------
/**
 * 指定のとくせいを持つポケモンが戦闘に出ているかチェック
 *
 * @param   wk
 * @param   tokusei
 *
 * @retval  BOOL    出ていたらTRUE
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFLOW_RECEPT_CheckExistTokuseiPokemon( BTL_SVFLOW_WORK* wk, PokeTokusei tokusei )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    if( BTL_POKEPARAM_GetValue(bpp, BPP_TOKUSEI) == tokusei )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * 指定IDのポケモンパラメータを返す
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_POKEPARAM*
 */
//--------------------------------------------------------------------------------------
const BTL_POKEPARAM* BTL_SVFLOW_RECEPT_GetPokeParam( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
}
//=============================================================================================
/**
 * 現在の経過ターン数を返す
 *
 * @param   wk
 *
 * @retval  u16
 */
//=============================================================================================
u16 BTL_SVFLOW_GetTurnCount( BTL_SVFLOW_WORK* wk )
{
  return wk->turnCount;
}
//=============================================================================================
/**
 * [ハンドラ受信] とくせいウィンドウ表示イン
 *
 * @param   wk
 * @param   pokeID
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_TokuseiWinIn( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  SCQUE_PUT_TOKWIN_IN( wk->que, pokeID );
}
//=============================================================================================
/**
 * [ハンドラ受信] とくせいウィンドウ表示アウト
 *
 * @param   wk
 * @param   pokeID
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_TokuseiWinOut( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  SCQUE_PUT_TOKWIN_OUT( wk->que, pokeID );
}

void BTL_SERVER_RECTPT_StdMessage( BTL_SVFLOW_WORK* wk, u16 msgID )
{
  SCQUE_PUT_MSG_STD( wk->que, msgID );
}
void BTL_SERVER_RECTPT_SetMessage( BTL_SVFLOW_WORK* wk, u16 msgID, u8 pokeID )
{
  SCQUE_PUT_MSG_SET( wk->que, msgID, pokeID );
}
void BTL_SERVER_RECTPT_SetMessageEx( BTL_SVFLOW_WORK* wk, u16 msgID, u8 pokeID, int arg )
{
  SCQUE_PUT_MSG_SET( wk->que, msgID, pokeID, arg );
}

//=============================================================================================
/**
 * 該当位置にいる生存しているポケモンIDを配列に格納＆数を返す
 *
 * @param   wk
 * @param   exPos
 * @param   dst_pokeID    [out]格納先配列
 *
 * @retval  u8    格納した数
 */
//=============================================================================================
u8 BTL_SERVERFLOW_RECEPT_GetTargetPokeID( BTL_SVFLOW_WORK* wk, BtlExPos exPos, u8* dst_pokeID )
{
  u8 pos[ BTL_POSIDX_MAX ];
  BTL_POKEPARAM* bpp;
  u8 max, num, i;

  max = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, pos );
  for(i=0, num=0; i<max; ++i)
  {
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, pos[i] );
    if( !BTL_POKEPARAM_IsDead(bpp) ){
      dst_pokeID[num++] = BTL_POKEPARAM_GetID( bpp );
    }
  }
  return num;
}
//=============================================================================================
/**
 * [ハンドラ受信] ステータスのランクダウン効果
 *
 * @param   wk
 * @param   exPos         対象ポケモン位置
 * @param   statusType    ステータスタイプ
 * @param   volume
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_RankDownEffect( BTL_SVFLOW_WORK* wk, BtlExPos exPos, BppValueID statusType, u8 val, BOOL fAlmost )
{
  BTL_POKEPARAM* pp;
  u8 targetPos[ BTL_POSIDX_MAX ];
  u8 numPokemons, i;
  int volume = val * -1;

  numPokemons = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, targetPos );
  for(i=0; i<numPokemons; ++i)
  {
    pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos[i] );
    if( !BTL_POKEPARAM_IsDead(pp) )
    {
      scproc_RankEffectCore( wk, pp, statusType, volume, ITEM_DUMMY_DATA, fAlmost, TRUE );
    }
  }
}
//=============================================================================================
/**
 * [ハンドラ受信] ステータスのランクアップ効果
 *
 * @param   wk
 * @param   exPos         対象ポケモン位置
 * @param   statusType    ステータスタイプ
 * @param   volume
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_RankUpEffect( BTL_SVFLOW_WORK* wk, BtlExPos exPos, BppValueID statusType, u8 volume, BOOL fAlmost )
{
  BTL_POKEPARAM* pp;
  u8 targetPos[ BTL_POSIDX_MAX ];
  u8 numPokemons, i;

  numPokemons = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, targetPos );
  BTL_Printf("ランクあげ効果：タイプ=%d,  対象ポケモン数=%d\n", statusType, numPokemons );
  for(i=0; i<numPokemons; ++i)
  {
    pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos[i] );
    if( !BTL_POKEPARAM_IsDead(pp) )
    {
      scproc_RankEffectCore( wk, pp, statusType, volume, ITEM_DUMMY_DATA, fAlmost, TRUE );
    }
  }
}
//=============================================================================================
/**
 * [ハンドラ受信] シンプルHP増減効果
 *
 * @param   wk
 * @param             対象ポケモン位置
 * @param   statusType    ステータスタイプ
 * @param   volume
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_HP_Add( BTL_SVFLOW_WORK* wk, u8 pokeID, int value )
{
  if( value )
  {
    BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
    if( value > 0 )
    {
      BTL_POKEPARAM_HpPlus( bpp, value );
      SCQUE_PUT_OP_HpPlus( wk->que, pokeID, value );
    }
    else
    {
      value *= -1;
      BTL_POKEPARAM_HpMinus( bpp, value );
      SCQUE_PUT_OP_HpMinus( wk->que, pokeID, value );
    }
    SCQUE_PUT_ACT_SimpleHP( wk->que, pokeID );
  }
}
//=============================================================================================
/**
 * [ハンドラ受信] 状態異常をくらわす処理
 *
 * @param   wk
 * @param   targetPokeID  くらう相手のポケモンID
 * @param   attackPokeID  くらわせる側の（とくせい持ちなど）ポケモンID
 * @param   sick
 * @param   fAlmost
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_AddSick( BTL_SVFLOW_WORK* wk, u8 targetPokeID, u8 attackPokeID,
  WazaSick sick, BPP_SICK_CONT contParam, BOOL fAlmost )
{
  BTL_POKEPARAM* receiver = BTL_POKECON_GetPokeParam( wk->pokeCon, targetPokeID );
  if( !BTL_POKEPARAM_IsDead(receiver) )
  {
    BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, attackPokeID );
    svflowsub_MakeSick( wk, receiver, attacker, sick, contParam, fAlmost );
  }
}
//=============================================================================================
/**
 * [ハンドラ受信] ポケモン系状態異常の回復処理
 *
 * @param   wk
 * @param   pokeID
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_CurePokeSick( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  if( !BTL_POKEPARAM_IsDead(bpp) )
  {
    BTL_POKEPARAM_CurePokeSick( bpp );
    SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
  }
}
//=============================================================================================
/**
 * [ハンドラ受信] ワザ系状態異常の回復処理
 *
 * @param   wk
 * @param   pokeID
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_CureWazaSick( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaSick sick )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  if( !BTL_POKEPARAM_IsDead(bpp) )
  {
    BTL_POKEPARAM_CureWazaSick( bpp, sick );
    SCQUE_PUT_OP_CureWazaSick( wk->que, pokeID, sick );
  }
}
//=============================================================================================
/**
 * [ハンドラ受信] 天候の変化効果
 *
 * @param   wk
 * @param   weather   変化後の天候指定／天候をフラットにしたい場合は BTL_WEATHER_NONE
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather )
{
  if( weather != BTL_WEATHER_NONE )
  {
    scproc_WeatherCore( wk, weather, BTL_WEATHER_TURN_PERMANENT );
  }
  else
  {
    BtlWeather prevWeather = BTL_FIELD_GetWeather();
    if( prevWeather != BTL_WEATHER_NONE )
    {
      BTL_FIELD_ClearWeather();
      SCQUE_PUT_ACT_WeatherEnd( wk->que, prevWeather );
    }
    else
    {
      GF_ASSERT(0);
    }
  }
}

//=============================================================================================
/**
 * [ハンドラ受信] バトルルール取得
 *
 * @param   wk
 *
 * @retval  BtlRule
 */
//=============================================================================================
BtlRule BTL_SVFLOW_GetRule( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetRule( wk->mainModule );
}
//=============================================================================================
/**
 * [ハンドラ受信] 逃げ交換禁止コードの追加を全クライアントに通知
 *
 * @param   wk
 * @param   clientID      禁止コードを発行したポケモンID
 * @param   code          禁止コード
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_CantEscapeAdd( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code )
{
  SCQUE_PUT_OP_CantEscape_Add( wk->que, pokeID, code );
}
//=============================================================================================
/**
 * [ハンドラ受信] 逃げ交換禁止コードの削除を全クライアントに通知
 *
 * @param   wk
 * @param   clientID      禁止コードを発行したポケモンID
 * @param   code          禁止コード
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_CantEscapeSub( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code )
{
  SCQUE_PUT_OP_CantEscape_Sub( wk->que, pokeID, code );
}
//=============================================================================================
/**
 * [ハンドラ受信] とくせい「トレース」処理
 *
 * @param   wk
 * @param   pokeID        トレース使う側ポケモンID
 * @param   targetPokeID  トレースされる側ポケモンID
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_TraceTokusei( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 targetPokeID )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  BTL_POKEPARAM* bppTgt = BTL_POKECON_GetPokeParam( wk->pokeCon, targetPokeID );
  PokeTokusei tok = BTL_POKEPARAM_GetValue( bppTgt, BPP_TOKUSEI );

  BTL_Printf("トレースで書き換えるとくせい=%d\n", tok);
  BTL_POKEPARAM_ChangeTokusei( bpp, tok );
  SCQUE_PUT_ACT_TokTrace( wk->que, pokeID, targetPokeID, tok );
  BTL_HANDLER_TOKUSEI_Add( bpp );
}

//=============================================================================================
/**
 * ハンドラ使用ヒープID取得
 *
 * @param   wk
 *
 * @retval  HEAPID
 */
//=============================================================================================
HEAPID BTL_SVFLOW_RECEPT_GetHeapID( BTL_SVFLOW_WORK* wk )
{
  return wk->heapID;
}


static void Hem_Init( HANDLER_EXHIBISION_MANAGER* wk )
{
  wk->stack_ptr = 0;
  wk->read_ptr = 0;
}

static u16 Hem_GetStackPtr( const HANDLER_EXHIBISION_MANAGER* wk )
{
  return wk->stack_ptr;
}

static u32 Hem_PushState( HANDLER_EXHIBISION_MANAGER* wk )
{
  u32 state = (wk->stack_ptr<<16) | wk->read_ptr;
  BTL_Printf(" *HEM-Push* sp=%d, rp=%d, next_rp=%d\n", wk->stack_ptr, wk->read_ptr, wk->stack_ptr);
  wk->read_ptr = wk->stack_ptr;
  return state;
}
static void Hem_PopState( HANDLER_EXHIBISION_MANAGER* wk, u32 state )
{
  wk->stack_ptr = (state >> 16) & 0xffff;;
  wk->read_ptr  = state & 0xffff;;
  BTL_Printf(" *HEM-Pop* sp=%d, rp=%d\n", wk->stack_ptr, wk->read_ptr);
}

static BTL_HANDEX_PARAM_HEADER* Hem_ReadWork( HANDLER_EXHIBISION_MANAGER* wk )
{
  if( wk->read_ptr < wk->stack_ptr )
  {
    BTL_HANDEX_PARAM_HEADER* header = (BTL_HANDEX_PARAM_HEADER*)(&wk->workBuffer[wk->read_ptr]);
    wk->read_ptr += header->size;

    GF_ASSERT( header->equip < BTL_HANDEX_MAX );
    GF_ASSERT( wk->read_ptr <= wk->stack_ptr );

    return header;
  }
  else
  {
    return NULL;
  }
}

static BTL_HANDEX_PARAM_HEADER* Hem_PushWork( HANDLER_EXHIBISION_MANAGER* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID )
{
  static const struct {
    u8 eq_type;
    u8 size;
  }work_size_table[] = {
    { BTL_HANDEX_USE_ITEM,       sizeof(BTL_HANDEX_PARAM_USE_ITEM)        },
    { BTL_HANDEX_RECOVER_HP,     sizeof(BTL_HANDEX_PARAM_RECOVER_HP)      },
    { BTL_HANDEX_SHIFT_HP,       sizeof(BTL_HANDEX_PARAM_SHIFT_HP)        },
    { BTL_HANDEX_RECOVER_PP,     sizeof(BTL_HANDEX_PARAM_PP)              },
    { BTL_HANDEX_DECREMENT_PP,   sizeof(BTL_HANDEX_PARAM_PP)              },
    { BTL_HANDEX_CURE_SICK,      sizeof(BTL_HANDEX_PARAM_CURE_SICK)       },
    { BTL_HANDEX_ADD_SICK,       sizeof(BTL_HANDEX_PARAM_ADD_SICK)        },
    { BTL_HANDEX_RANK_EFFECT,    sizeof(BTL_HANDEX_PARAM_RANK_EFFECT)     },
    { BTL_HANDEX_RECOVER_RANK,   sizeof(BTL_HANDEX_PARAM_RECOVER_RANK)    },
    { BTL_HANDEX_RESET_RANK,     sizeof(BTL_HANDEX_PARAM_RESET_RANK)      },
    { BTL_HANDEX_DAMAGE,         sizeof(BTL_HANDEX_PARAM_DAMAGE)          },
    { BTL_HANDEX_KILL,           sizeof(BTL_HANDEX_PARAM_KILL)            },
    { BTL_HANDEX_CHANGE_TYPE,    sizeof(BTL_HANDEX_PARAM_CHANGE_TYPE)     },
    { BTL_HANDEX_MESSAGE_STD,    sizeof(BTL_HANDEX_PARAM_MESSAGE)         },
    { BTL_HANDEX_MESSAGE_SET,    sizeof(BTL_HANDEX_PARAM_MESSAGE)         },
    { BTL_HANDEX_TOKWIN_IN,      sizeof(BTL_HANDEX_PARAM_HEADER)          },
    { BTL_HANDEX_TOKWIN_OUT,     sizeof(BTL_HANDEX_PARAM_HEADER)          },
    { BTL_HANDEX_SET_TURNFLAG,   sizeof(BTL_HANDEX_PARAM_TURNFLAG)        },
    { BTL_HANDEX_RESET_TURNFLAG, sizeof(BTL_HANDEX_PARAM_TURNFLAG)        },
    { BTL_HANDEX_SET_CONTFLAG,   sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)    },
    { BTL_HANDEX_RESET_CONTFLAG, sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)    },
    { BTL_HANDEX_SIDE_EFFECT,    sizeof(BTL_HANDEX_PARAM_SIDE_EFFECT)     },
    { BTL_HANDEX_TOKUSEI_CHANGE, sizeof(BTL_HANDEX_PARAM_TOKUSEI_CHANGE)  },
  };
  u32 size, i;

  for(i=0, size=0; i<NELEMS(work_size_table); ++i)
  {
    if( work_size_table[i].eq_type == eq_type ){
      size = work_size_table[i].size;
      break;
    }
  }

  if( size )
  {
    while( size % 4 ){ ++size; }
    if( (wk->stack_ptr + size) <= sizeof(wk->workBuffer) )
    {
      BTL_HANDEX_PARAM_HEADER* header = (BTL_HANDEX_PARAM_HEADER*) &(wk->workBuffer[wk->stack_ptr]);

      for(i=0; i<size; ++i){
        wk->workBuffer[ wk->stack_ptr + i ] = 0;
      }
      header->equip = eq_type;
      header->size = size;
      header->userPokeID = userPokeID;
      header->tokwin_flag = 0;
      wk->stack_ptr += size;
      BTL_Printf("Get Hem Work: ADRS=0x%p, type=%d, size=%d, pokeID=%d\n", header, header->equip, size, userPokeID);
      return header;
    }
    else
    {
      GF_ASSERT_MSG(0, "stack over flow ... eq_type=%d, pokeID=%d", eq_type, userPokeID);
    }
  }
  else
  {
    GF_ASSERT_MSG(0, "illegal eq_type = %d", eq_type);
  }
  return NULL;
}


void* BTL_SVFLOW_HANDLERWORK_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID )
{
  return Hem_PushWork( &wk->HEManager, eq_type, userPokeID );
}

