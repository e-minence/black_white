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
#include "item\itemtype_def.h"

#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_server_local.h"
#include "btl_server.h"
#include "btl_sick.h"
#include "btl_pospoke_state.h"
#include "btl_shooter.h"

#include "handler\hand_tokusei.h"
#include "handler\hand_item.h"
#include "handler\hand_waza.h"
#include "handler\hand_side.h"
#include "handler\hand_pos.h"

#include "btl_server_flow.h"

#define SOGA_DEBUG  //暫定でsogaがデバッグした箇所

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

  FLOWFLG_FIRST_FIGHT,
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
 *  他ワザ呼び出しパラメータ
 */
typedef struct {

  WazaID      wazaID;
  BtlPokePos  targetPos;

}REQWAZA_WORK;

/**
 *  アクション優先順記録構造体
 */
typedef struct {

  BTL_POKEPARAM      *bpp;
  BTL_ACTION_PARAM   action;
  REQWAZA_WORK       reqWaza;
  u32                priority;

  u8  clientID;   ///< クライアントID
  u8  pokeIdx;    ///< そのクライアントの、何体目？ 0〜

  u8                 fDone;
  u8                 fIntr;

}ACTION_ORDER_WORK;


/**
 * 対象となるポケモンパラメータを記録しておくためのワーク
 */
typedef struct {

  BTL_POKEPARAM*  pp[ BTL_POS_MAX ];
  u16             damage[ BTL_POS_MAX ];
  u8              count;
  u8              countMax;
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

/**
 *  経験値計算用ワーク
 */
typedef struct {
  u8   fBonus;    ///< 多めにもらえたフラグ
  u32  exp;       ///< もらえる経験値
}CALC_EXP_WORK;


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
  BtlBagMode              bagMode;
  HEAPID  heapID;

  u8      numClient;
  u8      numActOrder;
  u8      numEndActOrder;
  u8      wazaEff_EnableFlag;
  u8      wazaEff_TargetPokeID;
  u8      escapeClientID;
  u8      getPokePos;
  u8      numDeadPoke;
  u8      numRelivePoke;
  u8      relivedPokeID[ BTL_POKEID_MAX ];
  u8      pokeDeadFlag[ BTL_POKEID_MAX ];


  ACTION_ORDER_WORK   actOrder[ BTL_POS_MAX ];
  TARGET_POKE_REC     targetPokemon;
  TARGET_POKE_REC     targetSubPokemon;
  TARGET_POKE_REC     damagedPokemon;
  SVFL_WAZAPARAM      wazaParam;
  BTL_POSPOKE_WORK    pospokeWork;
  CALC_EXP_WORK       calcExpWork[ BTL_PARTY_MEMBER_MAX ];

  BTL_EVENT_WORK_STACK        eventWork;
  HANDLER_EXHIBISION_MANAGER  HEManager;
  BPP_WAZADMG_REC             wazaDamageRec;
  WazaID                      prevExeWaza;

  u8          flowFlags[ FLOWFLG_BYTE_MAX ];
  u8          handlerTmpWork[ EVENT_HANDLER_TMP_WORK_SIZE ];
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL reqChangePokeForServer( BTL_SVFLOW_WORK* wk );
static void ActOrder_Proc( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* actOrder );
static void scproc_BeforeFirstFight( BTL_SVFLOW_WORK* wk );
static void clear_poke_actflags( BTL_SVFLOW_WORK* wk );
static void scproc_SetFlyingFlag( BTL_SVFLOW_WORK* wk );
static void reqWazaUseForCalcActOrder( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order );
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order, u32 orderMax );
static inline WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk );
static ACTION_ORDER_WORK* SearchActOrderByPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID );
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
static inline u32 TargetPokeRec_GetCountMax( const TARGET_POKE_REC* rec );
static u32 TargetPokeRec_CopyFriends( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst );
static u32 TargetPokeRec_CopyEnemys( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst );
static void TargetPokeRec_RemoveDeadPokemon( TARGET_POKE_REC* rec );
static void scproc_Move( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scproc_NigeruCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_SkipNigeruCalc( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_SkipNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_NigeruExMessage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scproc_NigeruCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fBtlStart );
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk );
static void scPut_MemberOutMessage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_MemberOut( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scproc_TrainerItem_Root( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx );
static void scproc_TrainerItem_BallRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static u8 ItemEff_SleevRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_PoisonRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_YakedoRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_KooriRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_MahiRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_KonranRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_MeromeroRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_EffectGuard( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_Relive( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_DefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_SPAttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_SPDefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AgilityRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_HitRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_CriticalUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_PP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AllPP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_HP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_Common_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID );
static u8 ItemEff_Common_Rank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BppValueID rankType );
static u8 ShooterEff_ItemCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_SkillCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_ItemDrop( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_FlatCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static void scproc_MemberChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u8 nextPokeIdx );
static BOOL scproc_MemberOut( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke );
static void scproc_Fight( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, REQWAZA_WORK* reqWaza, BTL_ACTION_PARAM* action );
static u8 scproc_Check_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  TARGET_POKE_REC* rec, BTL_HANDEX_STR_PARAMS* strParam );
static BOOL scEvent_CheckWazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  TARGET_POKE_REC* targetRec, u8* robberPokeID, u8* robTargetPokeID, BTL_HANDEX_STR_PARAMS* strParam );
static BOOL scproc_Fight_CheckReqWazaFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID orgWazaID );
static void scPut_ReqWazaEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos );
static void scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, TARGET_POKE_REC* targetRec );
static u8 flowsub_registerWazaTargets( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* rec );
static u8 registerTarget_single( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, TARGET_POKE_REC* rec );
static u8 registerTarget_double( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, TARGET_POKE_REC* rec );
static u8 registerTarget_triple( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, TARGET_POKE_REC* rec );
static BOOL IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void flowsub_checkWazaAvoid( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_WazaEffective( BTL_SVFLOW_WORK* wk, WazaID waza, u8 atkPokeID, u8 defPokeID, u32 que_reserve_pos );
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckWazaExecuteFail_1st( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckWazaExecuteFail_2nd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scproc_Fight_CheckConf( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static BOOL scproc_Fight_CheckMeroMero( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static void scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scproc_WazaExecuteFailed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause fail_cause );
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
static void scproc_decrementPPUsedWaza( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, TARGET_POKE_REC* rec );
static BOOL scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume );
static void scproc_Fight_Damage_Root( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
   BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_ToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_Determine( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* targets );
static void scEvent_WazaDamageDetermine( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  TARGET_POKE_REC* targets, fx32 dmg_ratio );
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
      BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
      fx32 targetDmgRatio, BtlTypeAff affinity );
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    const SVFL_WAZAPARAM* wazaParam,   BtlTypeAff affinity,   fx32 targetDmgRatio );
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, BtlTypeAff* affAry, const SVFL_WAZAPARAM* wazaParam, fx32 dmg_ratio );
static void wazaDmgRec_Add( const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, u16 damage );
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, BOOL critical_flag );
static void scproc_CheckItemReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
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
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BOOL scproc_AddShrinkCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaID waza, u32 per );
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BOOL scproc_DrainCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP );
static u16 scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag );
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage );
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, const BTL_HANDEX_STR_PARAMS* str );
static BOOL scproc_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost );
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u16 itemID, BOOL fAlmost, BOOL fStdMsg );
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker );
static BOOL scproc_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP );
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk );
static u8 get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk );
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void scproc_Fight_Weather( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker );
static BOOL scproc_WeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );
static void scput_Fight_Uncategory( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scput_Fight_Uncategory_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scput_Fight_Uncategory_Hensin( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scproc_Migawari_Create( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scproc_Migawari_Damage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 damage );
static void scproc_Migawari_CheckNoEffect( BTL_SVFLOW_WORK* wk, SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, TARGET_POKE_REC* rec );
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BtlEventType event_type );
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, BtlEventType event_type );
static void scproc_countup_shooter_energy( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_sick( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg );
static void scproc_turncheck_weather( BTL_SVFLOW_WORK* wk );
static void scproc_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke );
static void scproc_GetExp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* deadPoke );
static void getexp_calc( BTL_SVFLOW_WORK* wk, const BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result );
static u32 getexp_calc_adjust_level( u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv );
static void getexp_make_cmd( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const CALC_EXP_WORK* calcExp );
static void scproc_ClearPokeDependEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* poke );
static inline int roundValue( int val, int min, int max );
static inline int roundMin( int val, int min );
static inline int roundMax( int val, int max );
static inline BOOL perOccur( u8 per );
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack );
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size );
static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs );
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_MeromeroAct( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage );
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick );
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static WazaSick scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWazaSickEx exSickCode, BPP_SICK_CONT* oldCont );
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx exCode );
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
static void scPut_ResetSameWazaCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza );
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scput_DmgToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BTL_EVWK_DMG_TO_RECOVER* evwk );
static void scPut_WazaEffectOn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza );
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage, BOOL critical_flag, BOOL plural_hit_flag );
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  u32 poke_cnt, BtlTypeAff aff, BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag );
static void scPut_Koraeru( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BppKoraeruCause cause );
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
static void scPut_Message_SetExPoke( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID, u32 argCnt, const int* args );
static void scPut_Message_StdEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args );
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args );
static void scPut_DecrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol );
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, u16 itemID );
static void scPut_EatNutsAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_RemoveItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_ActFlag_Set( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppActFlag flagID );
static void scPut_ActFlag_Clear( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_SetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_ResetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_SetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static void scPut_ResetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static BOOL scEvent_MemberChangeIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* outPoke );
static BOOL scEvent_GetReqWazaForCalcActOrder( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID orgWazaID, REQWAZA_WORK* reqWaza );
static BOOL scEvent_GetReqWazaParam( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza );
static void scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8* spePriA, u8* spePriB );
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static void scEvent_BeforeFight( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza );
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckWazaMsgCustom( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, WazaID actWazaID, BTL_HANDEX_STR_PARAMS* str );
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_GetWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param );
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause );
static BOOL scEvent_WazaExecuteFix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, TARGET_POKE_REC* rec );
static u8 scEvent_GetWazaTargetIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_CheckMamoruBreak( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8 targetCount, const u8* targetPokeID );
static void scEvent_WazaExe_NoEffect( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scEvent_WazaExe_Done( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static BOOL scEvent_CheckTameTurnSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scEvent_checkHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckPokeHideAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IsExcuseCalcHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckNotEffect_byType( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, u8 lv,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam );
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage );
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, u16* damage );
static void scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, BOOL criticalFlag );
static void scEvent_CheckItemReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_ItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_ItemEquipTmp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BtlTypeAff scEvent_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* defender, PokeType waza_type );
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* rec );
static BOOL scEvent_DecrementPP_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaIdx );
static BOOL scEvent_CheckPluralHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8* hitCount );
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage );
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag );
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag );
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type );
static PokeTypePair scEvent_getDefenderPokeType( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scEvent_MemberIn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static u32 scEvent_GetWazaShrinkPer( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* target, WazaID waza, u32 per );
static void scEvent_FailShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target );
static WazaSick scEvent_CheckAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont );
static void scEvent_WazaSickCont( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT* sickCont );
static void scEvent_AfterDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, WazaID waza );
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scEvent_GetWazaRankEffectValue( BTL_SVFLOW_WORK* wk, WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume );
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume );
static void scEvent_RankEffect_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume );
static int scEvent_RecalcDrainVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u32 volume );
static void scEvent_CheckSpecialDrain( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, u32 total_damage );
static BOOL scEvent_CheckChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8* turn );
static void scEvent_AfterChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather );
static int scEvent_CalcWeatherDamage( BTL_SVFLOW_WORK* wk, BtlWeather weather, BTL_POKEPARAM* bpp, u8 *tok_cause_flg );
static u32 scEvent_CalcRecoverHP( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp );
static BOOL scEventSetItem( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void Hem_Init( HANDLER_EXHIBISION_MANAGER* wk );
static u32 Hem_PushState( HANDLER_EXHIBISION_MANAGER* wk );
static void Hem_PopState( HANDLER_EXHIBISION_MANAGER* wk, u32 state );
static u16 Hem_GetStackPtr( const HANDLER_EXHIBISION_MANAGER* wk );
static BTL_HANDEX_PARAM_HEADER* Hem_ReadWork( HANDLER_EXHIBISION_MANAGER* wk );
static BTL_HANDEX_PARAM_HEADER* Hem_PushWork( HANDLER_EXHIBISION_MANAGER* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );
static void relivePokeRec_Init( BTL_SVFLOW_WORK* wk );
static void relivePokeRec_Add( BTL_SVFLOW_WORK* wk, u8 pokeID );
static BOOL relivePokeRec_CheckNecessaryPokeIn( BTL_SVFLOW_WORK* wk );
static BOOL scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID );
static u8 scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_drain( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void handex_addsick_msg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, void* arg );
static u8 scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_setRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_message( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sideEffectRemove( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_addFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void handexSub_putString( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_STR_PARAMS* strParam );
static u8 scproc_HandEx_removeFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_PosEffAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_equipItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_consumeItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_swapItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void handexSub_itemSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static u8 scproc_HandEx_updateWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_counter( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_delayWazaDamage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_quitBattle( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeMember( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_batonTouch( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_addShrink( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_relive( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );


BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, u32 numClient, BtlBagMode bagMode, HEAPID heapID )
{
  BTL_SVFLOW_WORK* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SVFLOW_WORK) );

  wk->server = server;
  wk->pokeCon = pokeCon;
  wk->mainModule = mainModule;
  wk->numClient = numClient;
  wk->numActOrder = 0;
  wk->turnCount = 0;
  wk->flowResult = SVFLOW_RESULT_DEFAULT;
  wk->que = que;
  wk->heapID = heapID;
  wk->prevExeWaza = WAZANO_NULL;
  wk->bagMode = bagMode;
  wk->escapeClientID = BTL_CLIENTID_NULL;
  wk->getPokePos = BTL_POS_NULL;
  {
    BtlRule rule = BTL_MAIN_GetRule( mainModule );
    BTL_POSPOKE_InitWork( &wk->pospokeWork, wk->mainModule, wk->pokeCon, rule );
  }

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
      if( !BPP_IsDead( BTL_PARTY_GetMemberData(cw->party, posIdx)) )
      {
        BTL_Printf("client(%d, posIdx=%d poke...In!\n", i, posIdx);
        scproc_MemberIn( wk, i, posIdx, posIdx, TRUE );
      }
    }
  }

  scproc_AfterMemberIn( wk );

  // @@@ しぬこともあるかも?
  return SVFLOW_RESULT_DEFAULT;
}

//--------------------------------------------------------------------------
/**
 * サーバコマンド生成（通常の１ターン分コマンド生成）
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_Start( BTL_SVFLOW_WORK* wk )
{
  u8 numActPoke, alivePokeBefore, alivePokeAfter;
  u8 i;

  SCQUE_Init( wk->que );
  relivePokeRec_Init( wk );
  wk->numActOrder = 0;
  wk->numDeadPoke = 0;


  FlowFlg_ClearAll( wk );
  BTL_EVENT_StartTurn();
  wk->flowResult =  SVFLOW_RESULT_DEFAULT;

  clear_poke_actflags( wk );
  scproc_SetFlyingFlag( wk );

  BTL_SERVER_InitChangePokemonReq( wk->server );

  wk->numActOrder = sortClientAction( wk, wk->actOrder, NELEMS(wk->actOrder) );
  wk->numEndActOrder = 0;
  for(i=0; i<wk->numActOrder; i++)
  {
    ActOrder_Proc( wk, &wk->actOrder[i] );

    if( wk->flowResult !=  SVFLOW_RESULT_DEFAULT ){
      wk->numEndActOrder = i+1;
      break;
    }
  }

  // 全アクション処理し終えた
  if( i == wk->numActOrder )
  {
    wk->numEndActOrder = wk->numActOrder;

    // ターンチェック処理
    scproc_TurnCheck( wk );

    // 死亡・生き返りなどでポケ交換の必要があるかチェック
    if( relivePokeRec_CheckNecessaryPokeIn(wk)
    ||  (wk->numDeadPoke != 0)
    ){
      BTL_Printf("新ポケ入場の必要あります\n");
      reqChangePokeForServer( wk );
      return SVFLOW_RESULT_POKE_IN_REQ;
    }
    return SVFLOW_RESULT_DEFAULT;
  }
  // まだアクションが残っているが入れ替え・逃げなど発生でサーバーへ返す
  else
  {
    // @@@ トンボがえりで…殴られた方が死んで終了の場合は引っ込む処理要らないねぇ。
    reqChangePokeForServer( wk );
    return wk->flowResult;
  }
}
//--------------------------------------------------------------------------
/**
 * サーバコマンド生成（ポケモン死亡・生き返りによるターン最初の入場処理）
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_StartAfterPokeIn( BTL_SVFLOW_WORK* wk )
{
  const BTL_ACTION_PARAM* action;
  SVCL_WORK* clwk;
  u16 clientID, posIdx;
  int i, j, actionCnt;

  BTL_Printf("空き位置にポケモン投入後のサーバーコマンド生成\n");

  SCQUE_Init( wk->que );
  scproc_SetFlyingFlag( wk );
  BTL_SERVER_InitChangePokemonReq( wk->server );

  wk->numDeadPoke = 0;
  wk->flowResult =  SVFLOW_RESULT_DEFAULT;

  wk->numActOrder = sortClientAction( wk, wk->actOrder, NELEMS(wk->actOrder) );
  wk->numEndActOrder = 0;
  for(i=0; i<wk->numActOrder; i++)
  {
    action = &wk->actOrder[i].action;
    if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
    if( action->change.depleteFlag ){ continue; }

    BTL_Printf("クライアント_%d の位置_%d へ、%d番目のポケを出す\n",
          i, action->change.posIdx, action->change.memberIdx );
    scproc_MemberIn( wk, i, action->change.posIdx, action->change.memberIdx, FALSE );
  }

  scproc_AfterMemberIn( wk );

  return ( wk->numDeadPoke == 0)?  SVFLOW_RESULT_DEFAULT : SVFLOW_RESULT_POKE_IN_REQ;
}
//--------------------------------------------------------------------------
/**
 * サーバコマンド生成（ターン途中のポケモン入れ替え選択後）
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_StartAfterPokeChange( BTL_SVFLOW_WORK* wk )
{
  const BTL_ACTION_PARAM* action;
  SVCL_WORK* clwk;
  u16 clientID, posIdx;
  int i, j, actionCnt;

  BTL_Printf("ひんしポケモン入れ替え選択後のサーバーコマンド生成\n");

  SCQUE_Init( wk->que );
  scproc_SetFlyingFlag( wk );

  wk->flowResult =  SVFLOW_RESULT_DEFAULT;
  BTL_SERVER_InitChangePokemonReq( wk->server );

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk == NULL ){
      continue;
    }
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

  {
    scproc_AfterMemberIn( wk );
    #if 0
    if( fNewPokeIn )
    {
      // @@@ この辺は BTL_SVFLOW_Start と一緒なのでひとまとめにする
      for(i=wk->numEndActOrder; i<wk->numActOrder; i++)
      {
        ActOrder_Proc( wk, &wk->actOrder[i] );
        if( wk->flowResult !=  SVFLOW_RESULT_DEFAULT ){
          wk->numEndActOrder = i+1;
          break;
        }
      }
      if( i==wk->numActOrder )
      {
        BTL_Printf("全クライアントの処理を最後まで行った\n");
        // 死んだポケモンがいる場合の処理
        if( reqChangePokeForServer(wk) ){
          BTL_Printf("…しんだポケがいる\n");
          return SVFLOW_RESULT_POKE_CHANGE;
        }
      }
      else{
        reqChangePokeForServer( wk );
        BTL_Printf("全クライアントの処理を最後まで行えない ... result=%d\n", wk->flowResult);
        return wk->flowResult;
      }
    }
    #endif
  }

  return SVFLOW_RESULT_DEFAULT;
}


//----------------------------------------------------------------------------------
/**
 * ローテーションコマンド生成（ローテーションバトルのみ）
 *
 * @param   wk
 * @param   clientID
 * @param   dir
 */
//----------------------------------------------------------------------------------
void BTL_SVFLOW_CreateRotationCommand( BTL_SVFLOW_WORK* wk, u8 clientID, BtlRotateDir dir )
{
  BTL_Printf("クライアント[%d]は回転方向 %d へローテ\n", clientID, dir);
  SCQUE_PUT_ACT_Rotation( wk->que, clientID, dir );

  if( dir != BTL_ROTATEDIR_STAY )
  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    BTL_POKEPARAM *outPoke, *inPoke;
    BTL_PARTY_RotateMembers( party, dir, &outPoke, &inPoke );
    if( !BPP_IsDead(outPoke) ){
      BTL_HANDLER_TOKUSEI_Remove( outPoke );
    }
    if( !BPP_IsDead(inPoke) ){
      BTL_HANDLER_TOKUSEI_Add( inPoke );
    }
    BTL_POSPOKE_Rotate( &wk->pospokeWork, dir, clientID, inPoke, wk->pokeCon );
  }
}

//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 *
 * @retval  BOOL  誰かが死んで交換する必要ある場合はTRUE
 */
//----------------------------------------------------------------------------------
static BOOL reqChangePokeForServer( BTL_SVFLOW_WORK* wk )
{
  u8 posAry[ BTL_POSIDX_MAX ];
  u8 empty_pos_cnt, clientID, i;
  u8 result = FALSE;

  SVCL_WORK* clwk;
  for(clientID=0; clientID<BTL_CLIENT_MAX; ++clientID)
  {
    empty_pos_cnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, clientID, posAry );
    BTL_Printf( "クライアント[%d]   空いている位置の数=%d\n", clientID, empty_pos_cnt );
    if( empty_pos_cnt )
    {
      // 空いている位置を全てサーバへ通知
      BTL_Printf( "  空いてる位置の数=%d  pos=", empty_pos_cnt );
      for(i=0; i<empty_pos_cnt; ++i)
      {
        BTL_SERVER_RequestChangePokemon( wk->server, posAry[i] );
        BTL_PrintfSimple( "%d,", posAry[i] );
      }
      BTL_PrintfSimple( "\n" );
      result = TRUE;
    }
  }
  return result;
}

//=============================================================================================
/**
 * 逃げたクライアントIDを取得
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_SVFLOW_GetEscapeClientID( const BTL_SVFLOW_WORK* wk )
{
  return wk->escapeClientID;
}

//=============================================================================================
/**
 * 捕獲したポケモンの位置を取得
 *
 * @param   wk
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_SVFLOW_GetCapturedPokePos( const BTL_SVFLOW_WORK* wk )
{
  return wk->getPokePos;
}


//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   actOrder
 */
//----------------------------------------------------------------------------------
static void ActOrder_Proc( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* actOrder )
{
  if( actOrder->fDone == FALSE )
  {
    BTL_POKEPARAM* bpp = actOrder->bpp;

    if( !BPP_IsDead(bpp) )
    {
      BTL_ACTION_PARAM action = actOrder->action;

      switch( action.gen.cmd ){
      case BTL_ACTION_FIGHT:
        if( !FlowFlg_Get(wk, FLOWFLG_FIRST_FIGHT) ){
          scproc_BeforeFirstFight( wk );
          FlowFlg_Set( wk, FLOWFLG_FIRST_FIGHT );
        }
        BTL_Printf("【たたかう】を処理。ワザ[%d]を、位置[%d]の相手に。\n", action.fight.waza, action.fight.targetPos);
        scproc_Fight( wk, bpp, &actOrder->reqWaza, &action );
        if( wk->flowResult == SVFLOW_RESULT_BTL_QUIT ){
          PMSND_PlaySE( SEQ_SE_NIGERU );
        }
        break;
      case BTL_ACTION_ITEM:
        BTL_Printf("【どうぐ】を処理。アイテム%dを、%d番の相手に。\n", action.item.number, action.item.targetIdx);
        scproc_TrainerItem_Root( wk, bpp, action.item.number, action.item.param, action.item.targetIdx );
        break;
      case BTL_ACTION_CHANGE:
        BTL_Printf("【ポケモン】を処理。位置%d <- ポケ%d \n", action.change.posIdx, action.change.memberIdx);
        scproc_MemberChange( wk, bpp, action.change.memberIdx );
        break;
      case BTL_ACTION_ESCAPE:
        BTL_Printf("【にげる】を処理。\n");
        if( scproc_NigeruCmd( wk, bpp ) )
        {
          PMSND_PlaySE( SEQ_SE_NIGERU );
          wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
          wk->escapeClientID = actOrder->clientID;
          BTL_Printf("クライアント[%d]が逃げて終了\n");
        } else {
          SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeFail );
        }
        break;
      case BTL_ACTION_MOVE:
        scproc_Move( wk, bpp );
        break;
      case BTL_ACTION_SKIP:
        BTL_Printf("処理をスキップ\n");
        scPut_CantAction( wk, bpp );
        break;
      case BTL_ACTION_NULL:
        BTL_Printf("不明な処理\n");
        scPut_CantAction( wk, bpp );
        break;
      }

      BPP_TURNFLAG_Set( bpp, BPP_TURNFLG_ACTION_DONE );
      actOrder->fDone = TRUE;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ターン最初の「たたかう」コマンド処理前に実行する処理
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_BeforeFirstFight( BTL_SVFLOW_WORK* wk )
{
  u32 hem_state, i;
  WazaID waza;

  hem_state = Hem_PushState( &wk->HEManager );
  for(i=0; i<wk->numActOrder; ++i)
  {
    waza = ActOrder_GetWazaID( &wk->actOrder[i] );
    if( waza != WAZANO_NULL )
    {
      if( BTL_HANDLER_Waza_Add(wk->actOrder[i].bpp, waza) )
      {
        scEvent_BeforeFight(wk, wk->actOrder[i].bpp, waza);
        BTL_HANDLER_Waza_Remove( wk->actOrder[i].bpp, waza );
      }
    }
  }
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
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
//----------------------------------------------------------------------------------
/**
 * 場にいる全てのポケモンに可能かつ必要なら飛行フラグをセット
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_SetFlyingFlag( BTL_SVFLOW_WORK* wk )
{
  if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) )
  {
    FRONT_POKE_SEEK_WORK  fps;
    BTL_POKEPARAM* bpp;

    FRONT_POKE_SEEK_InitWork( &fps, wk );
    while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
    {
      if( BPP_IsMatchType(bpp, POKETYPE_HIKOU)
      ||  (BPP_GetValue(bpp, BPP_TOKUSEI) == POKETOKUSEI_FUYUU)
      ||  (BPP_CheckSick(bpp, WAZASICK_FLYING))
      ){
        scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_FLYING );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 他ワザ呼び出しを行うワザで、アクション処理順序計算にも呼び出し後のワザを適用する場合、
 * 呼び出すワザID等をワークに格納する
 *
 * @param   flowWk
 * @param   order
 */
//----------------------------------------------------------------------------------
static void reqWazaUseForCalcActOrder( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order )
{
  order->reqWaza.wazaID = WAZANO_NULL;
  order->reqWaza.targetPos = BTL_POS_NULL;

  if( order->action.gen.cmd == BTL_ACTION_FIGHT )
  {
    WazaID orgWazaID = order->action.fight.waza;
    if( BTL_HANDLER_Waza_Add(order->bpp, orgWazaID) )
    {
      scEvent_GetReqWazaForCalcActOrder( wk, order->bpp, orgWazaID, &order->reqWaza );
      BTL_HANDLER_Waza_Remove( order->bpp, orgWazaID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 *
 * ポケモンごとのアクションをチェックし、処理順序を確定
 *
 * @param   server
 * @param   order       処理する順番にクライアントIDを並べ直して格納するための配列
 * @param   orderMax    配列 order の要素数
 *
 * @retval  u8    処理するポケモン数
 */
//----------------------------------------------------------------------------------
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order, u32 orderMax )
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
  u16 agility;
  u8  action, actionPri, wazaPri, spPri_A, spPri_B;
  u8  i, j, p, numPoke;

// 全ポケモンの行動内容をワークに格納
  for(i=0, p=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk == NULL ){ continue; }

    numPoke = BTL_SVCL_GetNumActPoke( clwk );
    for(j=0; j<numPoke; j++)
    {
      order[p].bpp = BTL_PARTY_GetMemberData( clwk->party, j );
      order[p].action = *BTL_SVCL_GetPokeAction( clwk, j );
      order[p].clientID = i;
      order[p].pokeIdx = j;
      order[p].fDone = FALSE;
      order[p].fIntr = FALSE;
      reqWazaUseForCalcActOrder( wk, &order[p] );
      ++p;
    }
  }
// 各ポケモンの行動プライオリティ値を生成
  numPoke = p;
  for(i=0; i<numPoke; ++i)
  {
    bpp = order[i].bpp;
    actParam = &(order[i].action);

    // 行動による優先順（優先度高いほど数値大）
    switch( actParam->gen.cmd ){
    case BTL_ACTION_ESCAPE: actionPri = 5; break;
    case BTL_ACTION_ITEM:   actionPri = 4; break;
    case BTL_ACTION_CHANGE: actionPri = 3; break;
    case BTL_ACTION_SKIP:   actionPri = 2; break;
    case BTL_ACTION_MOVE:   actionPri = 1; break;
    case BTL_ACTION_FIGHT:  actionPri = 0; break;
    case BTL_ACTION_NULL: continue;
    default:
      GF_ASSERT(0);
      actionPri = 0;
      break;
    }
    // ワザによる優先順
    if( actParam->gen.cmd == BTL_ACTION_FIGHT ){
      WazaID  waza = ActOrder_GetWazaID( &order[i] );
      wazaPri = WAZADATA_GetPriority( waza ) - WAZAPRI_MIN;
      // アイテム装備など、特殊な優先フラグ
      scEvent_CheckSpecialActPriority( wk, bpp, &spPri_A, &spPri_B );
    }else{
      wazaPri = 0;
      spPri_A = BTL_SPPRI_A_DEFAULT;
      spPri_B = BTL_SPPRI_B_DEFAULT;
    }
    // すばやさ
    agility = scEvent_CalcAgility( wk, bpp );
    BPP_SetActionAgility( (BTL_POKEPARAM*)bpp, agility );

    BTL_Printf("行動プライオリティ決定！ Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
        i, j, actionPri, wazaPri, agility );

    // プライオリティ値とクライアントIDを対にして配列に保存
    order[i].priority = MakePriValue( actionPri, spPri_A, wazaPri, spPri_B, agility );
  }
// プライオリティ値によるソート
  for(i=0; i<numPoke; i++){
    for(j=i+1; j<numPoke; j++){
      if( order[i].priority < order[j].priority ){
        ACTION_ORDER_WORK tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
      }
    }
  }
// 全く同じプライオリティ値があったらランダムシャッフル
// @@@ 未実装

//

  return p;
}
// アクション内容から、優先順位計算に使用するワザIDを取得（ワザ使わない場合はWAZANO_NULL）
static inline WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk )
{
  if( wk->action.gen.cmd == BTL_ACTION_FIGHT )
  {
    return (wk->reqWaza.wazaID == WAZANO_NULL)? wk->action.fight.waza : wk->reqWaza.wazaID;
  }
  return WAZANO_NULL;
}
static ACTION_ORDER_WORK* SearchActOrderByPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i){
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      return &(wk->actOrder[i]);
    }
  }
  return NULL;
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
  wk->wazaEff_TargetPokeID = BPP_GetID( target );
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
        if( !BPP_IsDead( BTL_PARTY_GetMemberDataConst(cw->party, j)) )
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

    *bpp = BTL_PARTY_GetMemberData( cw->party, fpsw->pokeIdx );
    fpsw->pokeIdx++;

    while( fpsw->clientIdx < BTL_CLIENT_MAX )
    {
      cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, fpsw->clientIdx );
      if( cw )
      {
        while( fpsw->pokeIdx < cw->numCoverPos )
        {
          nextPoke = BTL_PARTY_GetMemberData( cw->party, fpsw->pokeIdx );
          if( !BPP_IsDead(nextPoke) )
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
  rec->countMax = 0;
}
static inline void TargetPokeRec_AddWithDamage( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp, u16 damage )
{
  if( rec->count < NELEMS(rec->pp) )
  {
    rec->pp[ rec->count ] = pp;
    rec->damage[ rec->count ] = damage;
    rec->count++;
    if( rec->count > rec->countMax ){
      rec->countMax = rec->count;
    }
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

  pokeID1 = BPP_GetID( pp );
  for(i=start_idx; i<rec->count; ++i)
  {
    pokeID2 = BPP_GetID( rec->pp[i] );
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
// このターンに記録されたポケモン総数を返す
static inline u32 TargetPokeRec_GetCountMax( const TARGET_POKE_REC* rec )
{
  return rec->countMax;
}
// 指定ポケモンと同チームのポケモンのみを別ワークにコピー
// return : コピーしたポケモン数
static u32 TargetPokeRec_CopyFriends( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst )
{
  BTL_POKEPARAM* bpp;
  u32 max, i;
  u8 ID1, ID2;

  TargetPokeRec_Clear( dst );
  ID1 = BPP_GetID( pp );

  max = TargetPokeRec_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    bpp = TargetPokeRec_Get( rec, i );
    ID2 = BPP_GetID( bpp );
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
  ID1 = BPP_GetID( pp );

  max = TargetPokeRec_GetCount( rec );
  for(i=0; i<max; ++i)
  {
    bpp = TargetPokeRec_Get( rec, i );
    ID2 = BPP_GetID( bpp );
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
    if( BPP_IsDead(bpp) ){
      TargetPokeRec_Remove( rec, bpp );
    }
  }
}


//======================================================================================================
// サーバーフロー処理
//======================================================================================================

//-----------------------------------------------------------------------------------
// サーバーフロー：「ムーブ」
//-----------------------------------------------------------------------------------
static void scproc_Move( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_PARTY* party;
  u8 clientID;
  u8 pokeID;
  int posIdx;

  pokeID = BPP_GetID( bpp );
  clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
  posIdx = BTL_PARTY_FindMember( party, bpp );
  if( (posIdx == 0) || (posIdx == 2) )
  {
    BtlPokePos pos1, pos2;

    pos1 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );
    pos2 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, 1 );
    BTL_POSPOKE_Swap( &wk->pospokeWork, pos1, pos2 );

    BTL_PARTY_SwapMembers( party, posIdx, 1 );
    SCQUE_PUT_ACT_MemberMove( wk->que, clientID, posIdx );
  }else{
    GF_ASSERT_MSG(0, "clientID=%d, pokeID=%d, posIdx=%d\n", clientID, pokeID, posIdx);
  }
}
//-----------------------------------------------------------------------------------
// サーバーフロー：「にげる」
//-----------------------------------------------------------------------------------
static BOOL scproc_NigeruCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BOOL fSkipNigeruCalc = scEvent_SkipNigeruCalc( wk, bpp );

  #ifdef PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
    return TRUE;
  }
  #endif

  if( !fSkipNigeruCalc )
  {
    // @@@ 本来はココで逃げる計算
    // @@@ 今はすばやさ計算をせず誰でも逃げられるようにしている
    // return FALSE;
  }

  return scproc_NigeruCore( wk, bpp );
}
//----------------------------------------------------------------------------------
/**
 * [Event] 逃げる時の確率計算処理をスキップするか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    スキップするならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipNigeruCalc( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKIP_NIGERU_CALC );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 逃げる封じのチェック処理をスキップするか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    スキップするならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKIP_NIGERU_FORBID );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 逃げる封じチェック処理
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  逃げる封じが発動していたらTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NIGERU_FORBID );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 逃げることが成功したときに特殊メッセージを表示するか判定
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    特殊メッセージを表示するならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_NigeruExMessage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NIGERU_EXMSG );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}


static BOOL scproc_NigeruCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  // 逃げ禁止チェック
  if( !scEvent_SkipNigeruForbid(wk, bpp) )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    BOOL fForbid = scEvent_CheckNigeruForbid( wk, bpp );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
    if( fForbid ){
      return FALSE;
    }
  }

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    if( scEvent_NigeruExMessage(wk, bpp) ){
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    }else{
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeSuccess );
    }
    Hem_PopState( &wk->HEManager, hem_state );
  }

  return TRUE;
}

//-----------------------------------------------------------------------------------
// サーバーフロー：メンバーを場に登場させる
//-----------------------------------------------------------------------------------
static void scproc_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fBtlStart )
{
  SVCL_WORK* clwk;
//  BTL_PARTY* party;
  BTL_POKEPARAM* bpp;

//  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  GF_ASSERT(posIdx < clwk->numCoverPos);

  if( posIdx != next_poke_idx ){
    BTL_Printf("サーバ側：%d <-> %d ポケ入れ替え\n", posIdx, next_poke_idx);
//    BTL_PARTY_SwapMembers( party, posIdx, next_poke_idx );
    BTL_PARTY_SwapMembers( clwk->party, posIdx, next_poke_idx );
  }
  bpp = BTL_PARTY_GetMemberData( clwk->party, posIdx );

  BTL_HANDLER_TOKUSEI_Add( bpp );
  BTL_HANDLER_ITEM_Add( bpp );
  BPP_CONTFLAG_Set( bpp, BPP_CONTFLG_MEMBERIN_EFFECT );
  BPP_SetAppearTurn( bpp, wk->turnCount );
  BPP_Clear_ForIn( bpp );

  SCQUE_PUT_OP_MemberIn( wk->que, clientID, posIdx, next_poke_idx, wk->turnCount );
  if( !fBtlStart ){
    SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, next_poke_idx );
  }

  {
    BtlPokePos  pos = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );
    BTL_POSPOKE_PokeIn( &wk->pospokeWork, pos, BPP_GetID(bpp), wk->pokeCon );
  }
}
//----------------------------------------------------------------------------------
/**
 * サーバーフロー：新しく場に入場したポケモン処理
 *
 * @param   wk
 *
 */
//----------------------------------------------------------------------------------
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
//  BOOL fExistNewPoke = FALSE;

  u32 hem_state = Hem_PushState( &wk->HEManager );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_MEMBERIN_EFFECT) )
    {
      BTL_Printf(" After MemberIn pokeID=%d\n", BPP_GetID(bpp) );
      scEvent_MemberIn( wk, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BPP_CONTFLAG_Clear( bpp, BPP_CONTFLG_MEMBERIN_EFFECT );
//      fExistNewPoke = TRUE;
    }
  }
  Hem_PopState( &wk->HEManager, hem_state );

//  return fExistNewPoke;
}
//----------------------------------------------------------------------------------
/**
 * [Put] メンバー交替時の「○○ もどれ！」などのメッセージ表示コマンド
 *
 * @param   wk
 * @param   bpp   引っ込めるポケモンパラメータ
 */
//----------------------------------------------------------------------------------
static void scPut_MemberOutMessage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  // @@@ ホントは状況に応じてメッセージ別ける
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_MemberOut1, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Put] メンバー交替時にポケモンを引っ込める処理コマンド
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scPut_MemberOut( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  SCQUE_PUT_ACT_MemberOut( wk->que, pos );
}
static void scPut_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
  SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, nextPokeIdx );
}
//-----------------------------------------------------------------------------------
// サーバーフロー：アイテム使用
//-----------------------------------------------------------------------------------
static void scproc_TrainerItem_Root( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx )
{
  typedef u8 (*pItemEffFunc)( BTL_SVFLOW_WORK*, BTL_POKEPARAM*, u16, int, u8 );

  static const struct {
    u16            effect;
    pItemEffFunc   func;
  }ItemParallelEffectTbl[] = {
    { ITEM_PRM_SLEEP_RCV,     ItemEff_SleevRcv      },   // 眠り回復
    { ITEM_PRM_POISON_RCV,    ItemEff_PoisonRcv     },   // 毒回復
    { ITEM_PRM_BURN_RCV,      ItemEff_YakedoRcv     },   // 火傷回復
    { ITEM_PRM_ICE_RCV,       ItemEff_KooriRcv      },   // 氷回復
    { ITEM_PRM_PARALYZE_RCV,  ItemEff_MahiRcv       },   // 麻痺回復
    { ITEM_PRM_PANIC_RCV,     ItemEff_KonranRcv     },   // 混乱回復
    { ITEM_PRM_MEROMERO_RCV,  ItemEff_MeromeroRcv   },   // メロメロ回復
    { ITEM_PRM_ABILITY_GUARD, ItemEff_EffectGuard   },   // 能力ガード
    { ITEM_PRM_DEATH_RCV,     ItemEff_Relive        },   // 瀕死回復
    { ITEM_PRM_ATTACK_UP,     ItemEff_AttackRank    },   // 攻撃力アップ
    { ITEM_PRM_DEFENCE_UP,    ItemEff_DefenceRank   },   // 防御力アップ
    { ITEM_PRM_SP_ATTACK_UP,  ItemEff_SPAttackRank  },   // 特攻アップ
    { ITEM_PRM_SP_DEFENCE_UP, ItemEff_SPDefenceRank },   // 特防アップ
    { ITEM_PRM_AGILITY_UP,    ItemEff_AgilityRank   },   // 素早さアップ
    { ITEM_PRM_HIT_UP,        ItemEff_HitRank       },   // 命中率アップ
    { ITEM_PRM_CRITICAL_UP,   ItemEff_CriticalUp    },   // クリティカル率アップ
    { ITEM_PRM_PP_RCV,        ItemEff_PP_Rcv        },   // PP回復
    { ITEM_PRM_ALL_PP_RCV,    ItemEff_AllPP_Rcv     },   // PP回復（全ての技）
    { ITEM_PRM_HP_RCV,        ItemEff_HP_Rcv        },   // HP回復
  };

  static const struct {
    u16           itemID;
    pItemEffFunc  func;
  }ShooterItemParam[] = {
    { ITEM_AITEMUKOORU,     ShooterEff_ItemCall  },
    { ITEM_SUKIRUKOORU,     ShooterEff_SkillCall },
    { ITEM_AITEMUDOROPPU,   ShooterEff_ItemDrop  },
    { ITEM_HURATTOKOORU,    ShooterEff_FlatCall  },
  };

  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );
  BTL_POKEPARAM* target = NULL;
  int i, itemParam;
  u32 hem_state;

  // ○○は××を使った！
  {
    int args[2];
    args[0] = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );
    args[1] = itemID;
    if( wk->bagMode != BBAG_MODE_SHOOTER ){
      scPut_Message_StdEx( wk, BTL_STRID_STD_UseItem_Self, 2, args );
    }else{
//      scPut_Message_StdEx( wk, BTL_STRID_STD_UseItem_Shooter, 2, args );
      SCQUE_PUT_MSG_STD_SE( wk->que, BTL_STRID_STD_UseItem_Shooter, SEQ_SE_SHOOTER, args[0], args[1] );
    }
  }

  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_ITEM_TYPE) == ITEMTYPE_BALL )
  {
    scproc_TrainerItem_BallRoot( wk, bpp, itemID );
    return;
  }

  // @@@ targetIdx = 自パーティの何番目のポケモンに使うか？という意味だが、マルチでも有効だと話が違ってきます
  if( targetIdx != BTL_PARTY_MEMBER_MAX ){
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    target = BTL_PARTY_GetMemberData( party, targetIdx );
  }

  // シューター独自のアイテム処理
  for(i=0; i<NELEMS(ShooterItemParam); ++i)
  {
    if( itemID == ShooterItemParam[i].itemID ){
      ShooterItemParam[i].func( wk, target, itemID, 0, actParam );
      return;
    }
  }

  // 通常のアイテム処理
  hem_state = Hem_PushState( &wk->HEManager );
  for(i=0; i<NELEMS(ItemParallelEffectTbl); ++i)
  {
    itemParam = BTL_CALC_ITEM_GetParam( itemID, ItemParallelEffectTbl[i].effect );
    if( itemParam ){
      if( !ItemParallelEffectTbl[i].func(wk, target, itemID, itemParam, actParam) )
      {
        scPut_Message_StdEx( wk, BTL_STRID_STD_UseItem_NoEffect, 0, NULL );
      }
    }
  }
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}

//----------------------------------------------------------------------------------
/**
 * ボール使用ルート
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 */
//----------------------------------------------------------------------------------
static void scproc_TrainerItem_BallRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID )
{
  if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD )
  {
    BTL_POKEPARAM* targetPoke;
    BtlPokePos  targetPos = BTL_POS_NULL;

    {
      BtlExPos exPos;
      u8 posAry[ BTL_POSIDX_MAX ];
      u8 basePos;
      u8 posCnt, i;

      basePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(bpp) );

      exPos = EXPOS_MAKE( BTL_EXPOS_ENEMY_ALL, basePos );
      posCnt = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, posAry );

      for(i=0; i<posCnt; ++i)
      {
        targetPoke = BTL_POKECON_GetFrontPokeData( wk->pokeCon, posAry[i] );
        if( !BPP_IsDead(targetPoke) )
        {
          targetPos = posAry[i];
          break;
        }
      }
    }

    if( targetPos != BTL_POS_NULL )
    {
      u8 yure_cnt, fSuccess;

      // @todo ここで捕獲成否チェック＆失敗なら揺れ数を計算する。今は適当。
      if( itemID == ITEM_MASUTAABOORU ){
        yure_cnt = 3;
        fSuccess = TRUE;
      }else{
        fSuccess = GFL_STD_MtRand(100) < 75;
        if( fSuccess ){
          yure_cnt = 3;
        }else{
          yure_cnt = GFL_STD_MtRand( 4 );
        }
      }

      SCQUE_PUT_ACT_BallThrow( wk->que, targetPos, yure_cnt, fSuccess );
      if( fSuccess ){
        wk->flowResult = SVFLOW_RESULT_POKE_GET;
        wk->getPokePos = targetPos;
      }
    }
  }
}

// アイテム効果：ねむり回復
static u8 ItemEff_SleevRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_NEMURI );
}
static u8 ItemEff_PoisonRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_DOKU );
}
static u8 ItemEff_YakedoRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_YAKEDO );
}
static u8 ItemEff_KooriRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_KOORI );
}
static u8 ItemEff_MahiRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_MAHI );
}
static u8 ItemEff_KonranRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_KONRAN );
}
static u8 ItemEff_MeromeroRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_MEROMERO );
}
static u8 ItemEff_EffectGuard( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( BPP_GetID(bpp) );
  BPP_SICK_CONT cont = BPP_SICKCONT_MakeTurn( 5 );
  if( BTL_HANDLER_SIDE_Add(side, BTL_SIDEEFF_SIROIKIRI, cont) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_MESSAGE, BTL_POKEID_NULL );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_SiroiKiri );
    HANDEX_STR_AddArg( &param->str, side );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_Relive( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BPP_IsDead(bpp))
  {
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_RELIVE* param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_RELIVE, pokeID );
    param->pokeID = pokeID;

    itemParam = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT );
    switch( itemParam ){
    case ITEM_RECOVER_HP_FULL:
      param->recoverHP = BPP_GetValue( bpp, BPP_MAX_HP ); break;
    case ITEM_RECOVER_HP_HALF:
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 2 ); break;
    default:
      param->recoverHP = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT ); break;
    }

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Relive );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    // 当ターンに生き返ったポケモンを記録
    relivePokeRec_Add( wk, pokeID );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_AttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_ATTACK );
}
static u8 ItemEff_DefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_DEFENCE );
}
static u8 ItemEff_SPAttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_SP_ATTACK );
}
static u8 ItemEff_SPDefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_SP_DEFENCE );
}
static u8 ItemEff_AgilityRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_AGILITY );
}
static u8 ItemEff_HitRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_HIT_RATIO );
}
static u8 ItemEff_CriticalUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u8 result = FALSE;
  if( !BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KIAIDAME) )
  {
    scPut_SetContFlag( wk, bpp, BPP_CONTFLG_KIAIDAME );
    result = TRUE;
  }
  if( itemParam > 1 ){
    --itemParam;
    if( BPP_AddCriticalRank(bpp, itemParam) ){
      SCQUE_PUT_OP_AddCritical( wk->que, BPP_GetID(bpp), itemParam );
      result = TRUE;
    }
  }
  if( result ){
    scPut_Message_Set( wk, bpp, BTL_STRID_SET_KiaiDame );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_PP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BPP_WAZA_GetCount(bpp) > actParam )
  {
    u8 pokeID = BPP_GetID( bpp );
    u8 volume = BPP_WAZA_GetPPShort( bpp, actParam );
    u8 ppValue = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_PP_RCV_POINT );
    if( ppValue != ITEM_RECOVER_PP_FULL ){
      if( volume > ppValue ){
        volume = ppValue;
      }
    }
    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->volume = volume;
      param->pokeID = pokeID;
      param->wazaIdx = actParam;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BPP_WAZA_GetID(bpp, actParam) );
      return 1;
    }
  }
  return 0;
}
static u8 ItemEff_AllPP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  BTL_HANDEX_PARAM_MESSAGE* msg_param;
  u32 cnt, volume, i;
  u8 pokeID = BPP_GetID( bpp );

  cnt = BPP_WAZA_GetCount( bpp );
  for(i=0; i<cnt; ++i)
  {
    volume = BPP_WAZA_GetPPShort( bpp, i );
    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->pokeID = pokeID;
      param->volume = volume;
    }
  }

  msg_param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_MESSAGE, pokeID );
  HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PP_AllRecover );
  HANDEX_STR_AddArg( &msg_param->str, pokeID );
  return 1;
}
static u8 ItemEff_HP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_DEATH_RCV) ){
    return ItemEff_Relive( wk, bpp, itemID, itemParam, actParam );
  }

  if( !BPP_IsHPFull(bpp)
  &&  !BPP_IsDead(bpp)
  ){
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->pokeID = pokeID;
    itemParam = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT );
    switch( itemParam ){
    case ITEM_RECOVER_HP_FULL:
      param->recoverHP = BPP_GetValue( bpp, BPP_MAX_HP ); break;
    case ITEM_RECOVER_HP_HALF:
      param->recoverHP = BTL_CALC_QuotMaxHP(bpp, 2 ); break;
    default:
      param->recoverHP = itemParam; break;
    }

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    return 1;
  }
  return 0;
}

/**
 *  状態異常治し系共通
 */
static u8 ItemEff_Common_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID )
{
  if( !BPP_IsDead(bpp) )
  {
    if( BPP_CheckSick(bpp, sickID) )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param;
      u8 pokeID = BPP_GetID( bpp );
      param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_CURE_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = sickID;
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  ランクアップ系共通
 */
static u8 ItemEff_Common_Rank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BppValueID rankType )
{
  u8 pokeID = BPP_GetID( bpp );

  if( (BTL_SVFLOW_CheckExistFrontPokeID(wk, pokeID) != BTL_POS_MAX)
  &&  !BPP_IsDead(bpp)
  ){
    if( BPP_IsRankEffectValid(bpp, rankType, itemParam) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = rankType;
      param->rankVolume = itemParam;
      param->fAlmost = TRUE;
      return 1;
    }
  }
  else
  {
    BTL_Printf("場に出ていないのでランク効果なし\n");
  }

  return 0;
}
/**
 *  シューター専用：アイテムコール
 */
static u8 ShooterEff_ItemCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  scproc_UseItemEquip( wk, bpp );
  // @todo 使えるアイテムが無ければ「しかし効果が無かった」とか必要？
  return TRUE;
}
/**
 *  シューター専用：スキルコール
 */
static u8 ShooterEff_SkillCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  scEvent_MemberIn( wk, bpp );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
  return TRUE;
}
/**
 *  シューター専用：アイテムドロップ
 */
static u8 ShooterEff_ItemDrop( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u16 equipItemID = BPP_GetItem( bpp );
  if( equipItemID != ITEM_DUMMY_DATA )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    {
      BTL_HANDEX_PARAM_SET_ITEM* param;
      u8 pokeID = BPP_GetID( bpp );

      param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_ItemDrop );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, equipItemID );

      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    }
    Hem_PopState( &wk->HEManager, hem_state );
    return TRUE;
  }
  return FALSE;
}
/**
 *  シューター専用：フラットコール
 */
static u8 ShooterEff_FlatCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 pokeID = BPP_GetID( bpp );

    reset_param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVFLOW_HANDLERWORK_Push( wk, BTL_HANDEX_MESSAGE, pokeID );

    reset_param->poke_cnt = 1;
    reset_param->pokeID[0] = pokeID;

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_FlatCall );
  }
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
  return TRUE;
}





//-----------------------------------------------------------------------------------
// サーバーフロー：メンバー交替
//-----------------------------------------------------------------------------------
static void scproc_MemberChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u8 nextPokeIdx )
{
  // @@@ 成否チェック ... コマンド選択時にハジくので必要ないか？

  scPut_MemberOutMessage( wk, outPoke );

  if( scproc_MemberOut(wk, outPoke) )
  {
    BtlPokePos pos;
    u8 clientID, posIdx;
    u8 outPokeID = BPP_GetID( outPoke );

    pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, outPokeID );
    BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, pos, &clientID, &posIdx );

    scproc_MemberIn( wk, clientID, posIdx, nextPokeIdx, FALSE );
    scproc_AfterMemberIn( wk );
  }
}
//----------------------------------------------------------------------------------
/**
 * メンバーを場から退場させる
 *
 * @param   wk
 * @param   bpp     退場させるポケモンパラメータ
 *
 * @retval  BOOL    退場できたらTRUE／割り込まれて死んだりしたらFALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_MemberOut( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke )
{
  // 割り込みチェック
  {
    u8 intrPokeID = scEvent_MemberChangeIntr( wk, outPoke );
    if( intrPokeID != BTL_POKEID_NULL )
    {
      ACTION_ORDER_WORK* actOrder = SearchActOrderByPokeID( wk, intrPokeID );
      if( actOrder ){
        ActOrder_Proc( wk, actOrder );
      }
    }
  }

  if( !BPP_IsDead(outPoke) )
  {
    scPut_MemberOut( wk, outPoke );

    BPP_Clear_ForOut( outPoke );
    SCQUE_PUT_OP_OutClear( wk->que, BPP_GetID(outPoke) );

    BTL_POSPOKE_PokeOut( &wk->pospokeWork, BPP_GetID(outPoke) );
    scproc_ClearPokeDependEffect( wk, outPoke );
    return TRUE;
  }
  return FALSE;
}
//-----------------------------------------------------------------------------------
// サーバーフロー：「たたかう」ルート
//-----------------------------------------------------------------------------------
static void scproc_Fight( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, REQWAZA_WORK* reqWaza, BTL_ACTION_PARAM* action )
{
  WazaID  orgWaza, actWaza;
  BtlPokePos  orgTargetPos, actTargetPos;
  BTL_HANDEX_STR_PARAMS  strParam;
  u8 waza_exe_flag, waza_lock_flag;

  orgWaza = action->fight.waza;
  orgTargetPos = action->fight.targetPos;
  waza_exe_flag = FALSE;
  waza_lock_flag = BPP_CheckSick( attacker, WAZASICK_WAZALOCK );

  BTL_HANDLER_Waza_Add( attacker, orgWaza );
  do {

    // ワザ出し失敗判定１（ポケモン系状態異常＆こんらん、メロメロ、ひるみ）
    if( scproc_Fight_CheckWazaExecuteFail_1st(wk, attacker, orgWaza) ){ break; }

    // 他ワザ呼び出しワザのパラメータチェック＆失敗判定
    if( !scEvent_GetReqWazaParam(wk, attacker, orgWaza, orgTargetPos, reqWaza) ){
      scPut_WazaMsg( wk, attacker, orgWaza );
      scproc_WazaExecuteFailed( wk, attacker, orgWaza, SV_WAZAFAIL_OTHER );
      break;
    }
    else{
      if( reqWaza->wazaID != WAZANO_NULL ){
        scPut_WazaMsg( wk, attacker, orgWaza );
        scPut_ReqWazaEffect( wk, attacker, orgWaza, orgTargetPos );
        BTL_HANDLER_Waza_Add( attacker, reqWaza->wazaID );
        actWaza = reqWaza->wazaID;
        actTargetPos = reqWaza->targetPos;
        BTL_Printf("他ワザ実行で [%d] -> [%d] が出る\n", orgWaza, actWaza);
      }else{
        actWaza = orgWaza;
        actTargetPos = orgTargetPos;
      }
    }

    // ワザパラメータ確定
    scEvent_GetWazaParam( wk, actWaza, attacker, &wk->wazaParam );

    // ワザメッセージ出力
    if( scEvent_CheckWazaMsgCustom(wk, attacker, orgWaza, actWaza, &strParam) ){
      // 他ワザ呼び出し時など、ワザ名メッセージをカスタマイズした場合
      handexSub_putString( wk, &strParam );
    }else{
      scPut_WazaMsg( wk, attacker, actWaza );
    }

    // ワザ出し失敗判定２
    if( scproc_Fight_CheckWazaExecuteFail_2nd(wk, attacker, actWaza) ){ break; }

    // ワザ対象をワークに取得
    TargetPokeRec_Clear( &wk->targetPokemon );
    flowsub_registerWazaTargets( wk, attacker, action->fight.targetPos, &wk->wazaParam, &wk->targetPokemon );

    // ワザ乗っ取られ判定
    {
      BTL_POKEPARAM* wazaUser;
      u8 robberPokeID;
      robberPokeID = scproc_Check_WazaRob(wk, attacker, actWaza, &wk->targetPokemon, &strParam);
      if( robberPokeID != BTL_POKEID_NULL )
      {
        wazaUser = BTL_POKECON_GetPokeParam( wk->pokeCon, robberPokeID );
        handexSub_putString( wk, &strParam );
      }
      else
      {
        // ここまで来たら通常ワザ出し成功
        // @@@ ここに、対象ポケによらない無効化チェックを入れるかも…
        wazaUser = attacker;
        BPP_UpdatePrevWazaID( attacker, actWaza, actTargetPos );
        SCQUE_PUT_OP_UpdateUseWaza( wk->que, BPP_GetID(attacker), actTargetPos, actWaza );
        BPP_TURNFLAG_Set( attacker, BPP_TURNFLG_WAZA_EXE );
        wk->prevExeWaza = actWaza;
        waza_exe_flag = TRUE;
      }

      scproc_Fight_WazaExe( wk, wazaUser, actWaza, &wk->targetPokemon );
    }

  }while(0);

  // 使ったワザのPP減らす（前ターンからロックされている場合は減らさない）
  if( !waza_lock_flag ){
    u8 wazaIdx = BPP_WAZA_SearchIdx( attacker, orgWaza );
    if( wazaIdx != PTL_WAZA_MAX ){
      scproc_decrementPPUsedWaza( wk, attacker, wazaIdx, &wk->damagedPokemon );
    }
  }

  if( reqWaza->wazaID != WAZANO_NULL ){
    BTL_HANDLER_Waza_Remove( attacker, reqWaza->wazaID );
  }
  BTL_HANDLER_Waza_Remove( attacker, orgWaza );

  // @@@ ワザハンドラ登録に失敗した時の処理はどーすっか
  if( waza_exe_flag == FALSE ){
    scPut_ResetSameWazaCounter( wk, attacker );
  }
}
static u8 scproc_Check_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  TARGET_POKE_REC* rec, BTL_HANDEX_STR_PARAMS* strParam )
{
  u8 targetCnt = TargetPokeRec_GetCount( rec );
  if( targetCnt <= 1 )
  {
    u8 robberPokeID = BTL_POKEID_NULL;
    u8 robTargetPokeID = BTL_POKEID_NULL;
    if( scEvent_CheckWazaRob(wk, attacker, waza, rec, &robberPokeID, &robTargetPokeID, strParam) )
    {
      TargetPokeRec_Clear( &wk->targetPokemon );
      if( robTargetPokeID != BTL_POKEID_NULL ){
        TargetPokeRec_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, robTargetPokeID) );
      }
      return robberPokeID;
    }
  }
  return BTL_POKEID_NULL;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 自分以外のポケが出したワザを乗っ取る判定
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 * @param   robberPokeID    [out] 乗っ取ったポケモンID
 * @param   robTargetPokeID [out] 乗っ取った場合の対象ポケモンID
 * @param   strParam        [out] 乗っ取った場合のメッセージパラメータ
 *
 * @retval  BOOL    乗っ取ったらTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckWazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  TARGET_POKE_REC* targetRec, u8* robberPokeID, u8* robTargetPokeID, BTL_HANDEX_STR_PARAMS* strParam )
{
  u32 targetCnt = TargetPokeRec_GetCount( targetRec );
  u32 i;

  HANDEX_STR_Setup( strParam, BTL_STRTYPE_NULL, 0 );
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEID_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEID_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)strParam );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i)
      {
        bpp = TargetPokeRec_Get( targetRec, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_WAZA_ROB );
    *robberPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
    *robTargetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BTL_EVENTVAR_Pop();

  return (*robberPokeID) != BTL_POKEID_NULL;
}

// 他ワザ呼び出しするワザで、かつ失敗する場合のみTRUE
static BOOL scproc_Fight_CheckReqWazaFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID orgWazaID )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, orgWazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_CHECKFAIL );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return failFlag;
}
// 他ワザ呼び出しするワザのエフェクトを発動
static void scPut_ReqWazaEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos )
{
  BtlPokePos  atkPos;
  u8 pokeID = BPP_GetID( bpp );

  atkPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  SCQUE_PUT_ACT_WazaEffect( wk->que, atkPos, targetPos, waza );
  BPP_UpdatePrevWazaID( bpp, waza, targetPos );
  SCQUE_PUT_OP_UpdateUseWaza( wk->que, pokeID, targetPos, waza );
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
static void scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, TARGET_POKE_REC* targetRec )
{
  WazaCategory  category = WAZADATA_GetCategory( waza );
  u8 pokeID = BPP_GetID( attacker );
  u16  que_reserve_pos;

  if( scproc_Fight_TameWazaExe(wk, attacker, waza) ){
    return;
  }

  // ダメージ受けポケモンワークをクリアしておく
  TargetPokeRec_Clear( &wk->damagedPokemon );

  // ワザエフェクトコマンド生成用にバッファ領域を予約しておく
  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );
  wk->wazaEff_EnableFlag = FALSE;
  wk->wazaEff_TargetPokeID = BTL_POKEID_NULL;
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    BOOL fQuit = scEvent_WazaExecuteFix( wk, attacker, waza, targetRec );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
    if( fQuit ){
      wazaEff_SetNoTarget( wk );
      return;
    }
  }

  // @@@単純に死んでるポケモンを除外してるが…ホントはダブルとかだと隣のポケモンに当たったりするハズ
  TargetPokeRec_RemoveDeadPokemon( targetRec );

  // 対象ごとの無効チェック＆回避チェック
  flowsub_checkNotEffect( wk, &wk->wazaParam, attacker, targetRec );
  flowsub_checkWazaAvoid( wk, waza, attacker, targetRec );

  switch( category ){
  case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
  case WAZADATA_CATEGORY_DAMAGE_EFFECT:
  case WAZADATA_CATEGORY_DAMAGE_SICK:
  case WAZADATA_CATEGORY_DRAIN:
    scproc_Fight_Damage_Root( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_SIMPLE_EFFECT:
    scproc_Migawari_CheckNoEffect( wk, &wk->wazaParam, attacker, targetRec );
    scproc_Fight_SimpleEffect( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_SIMPLE_SICK:
    scproc_Migawari_CheckNoEffect( wk, &wk->wazaParam, attacker, targetRec );
    scproc_Fight_SimpleSick( wk, waza, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_EFFECT_SICK:
    scproc_Migawari_CheckNoEffect( wk, &wk->wazaParam, attacker, targetRec );
    scproc_Fight_EffectSick( wk, &wk->wazaParam, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_ICHIGEKI:
    scproc_Fight_Ichigeki( wk, waza, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_WEATHER:
    scproc_Fight_Weather( wk, waza, attacker );
    break;
  case WAZADATA_CATEGORY_PUSHOUT:
    scproc_Fight_PushOut( wk, waza, attacker, targetRec );
    break;
  case WAZADATA_CATEGORY_SIMPLE_RECOVER:
    scproc_Fight_SimpleRecover( wk, waza, attacker );
    break;
//  case WAZADATA_CATEGORY_GUARD:
//  case WAZADATA_CATEGORY_FIELD_EFFECT:
//  case WAZADATA_CATEGORY_SIDE_EFFECT:
  case WAZADATA_CATEGORY_OTHERS:
    scput_Fight_Uncategory( wk, &wk->wazaParam, attacker, targetRec );
    break;
  default:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
    break;
  }

  // ワザ効果あり→エフェクトコマンド生成などへ
  if( wk->wazaEff_EnableFlag ){
    scproc_Fight_WazaEffective( wk, waza, pokeID, wk->wazaEff_TargetPokeID, que_reserve_pos );
  }else{
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_WazaExe_NoEffect( wk, pokeID, waza );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_WazaExe_Done( wk, pokeID, waza );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }

  scproc_CheckDeadCmd( wk, attacker );

  if( !BPP_IsDead(attacker) )
  {
    // 反動で動けなくなるワザ処理
    if( WAZADATA_IsTire(waza) ){
      scPut_ActFlag_Set( wk, attacker, BPP_ACTFLG_CANT_ACTION );
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
static u8 flowsub_registerWazaTargets( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* rec )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );

  u8 intrPokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );

  TargetPokeRec_Clear( rec );

  switch( rule ){
  case BTL_RULE_SINGLE:
  default:
    return registerTarget_single( wk, attacker, targetPos, wazaParam, intrPokeID, rec );

  case BTL_RULE_DOUBLE:
  case BTL_RULE_ROTATION:
    return registerTarget_double( wk, attacker, targetPos, wazaParam, intrPokeID, rec );

  case BTL_RULE_TRIPLE:
    return registerTarget_triple( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
  }
}
static u8 registerTarget_single( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, TARGET_POKE_REC* rec )
{
  WazaTarget  targetType = WAZADATA_GetTarget( wazaParam->wazaID );
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  switch( targetType ){
  case WAZA_TARGET_OTHER_SELECT:  ///< 自分以外の１体（選択）
  case WAZA_TARGET_ENEMY_SELECT:  ///< 敵１体（選択）
  case WAZA_TARGET_ENEMY_RANDOM:  ///< 敵ランダム
  case WAZA_TARGET_ENEMY_ALL:     ///< 敵側２体
  case WAZA_TARGET_OTHER_ALL:     ///< 自分以外全部
    if( intrPokeID == BTL_POKEID_NULL ){
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    }else{
      TargetPokeRec_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_USER:                ///< 自分１体のみ
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体
    if( intrPokeID == BTL_POKEID_NULL ){
      TargetPokeRec_Add( rec, attacker );
    }else{
      TargetPokeRec_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_ALL:
    TargetPokeRec_Add( rec, attacker );
    TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    return 2;

  case WAZA_TARGET_UNKNOWN:
    if( intrPokeID != BTL_POKEID_NULL ){
      TargetPokeRec_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
      return 1;
    }else{
      return 0;
    }
    break;

  default:
    return 0;
  }
}
static u8 registerTarget_double( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, TARGET_POKE_REC* rec )
{
  WazaTarget  targetType = WAZADATA_GetTarget( wazaParam->wazaID );
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  BTL_POKEPARAM* bpp = NULL;

  switch( targetType ){
  case WAZA_TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
    bpp = get_opponent_pokeparam( wk, atPos, GFL_STD_MtRand(1) );
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
  case WAZA_TARGET_ALL:                ///< 全部
    TargetPokeRec_Add( rec, attacker );
    TargetPokeRec_Add( rec, get_next_pokeparam( wk, atPos ) );
    TargetPokeRec_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
    TargetPokeRec_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
    return 3;

  case WAZA_TARGET_USER:      ///< 自分１体のみ
    TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
    return 1;
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
    TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;
  case WAZA_TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
    TargetPokeRec_Add( rec, get_next_pokeparam(wk, atPos) );
    return 1;

  case WAZA_TARGET_UNKNOWN:
    {
      // @@@ ココは割り込みのターゲットとは処理を別ける必要があると思う。いずれやる。
      u8 pokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );
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
    if( intrPokeID != BTL_POKEID_NULL ){
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, intrPokeID );
    }
    TargetPokeRec_Add( rec, bpp );
    return 1;
  }
  else
  {
    return 0;
  }
}
static u8 registerTarget_triple( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, TARGET_POKE_REC* rec )
{
  WazaTarget  targetType = WAZADATA_GetTarget( wazaParam->wazaID );
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
  const BTL_TRIPLE_ATTACK_AREA* area = BTL_MAINUTIL_GetTripleAttackArea( atPos );
  u32 i, cnt;

  BTL_POKEPARAM* bpp = NULL;

  switch( targetType ){
  case WAZA_TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
    {
      u8 r = GFL_STD_MtRand( area->numEnemys );
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, area->enemyPos[r] );
    }
    break;

  case WAZA_TARGET_ENEMY_ALL:           ///< 敵側全体
    for(i=0; i<area->numEnemys; ++i){
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData( wk->pokeCon, area->enemyPos[i]) );
    }
    return area->numEnemys;

  case WAZA_TARGET_OTHER_ALL:           ///< 自分以外全部
    cnt = 0;
    for(i=0; i<area->numEnemys; ++i){
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->enemyPos[i]) );
      ++cnt;
    }
    for(i=0; i<area->numFriends; ++i){
      if( area->friendPos[i] == atPos ){ continue; }
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->friendPos[i]) );
      ++cnt;
    }
    return cnt;

  case WAZA_TARGET_ALL:                ///< 全部
    cnt = 0;
    for(i=0; i<area->numEnemys; ++i){
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->enemyPos[i]) );
      ++cnt;
    }
    for(i=0; i<area->numFriends; ++i){
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->friendPos[i]) );
      ++cnt;
    }
    return cnt;

  case WAZA_TARGET_USER:      ///< 自分１体のみ
    TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
    return 1;
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
    TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;
  case WAZA_TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
    TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;

  case WAZA_TARGET_UNKNOWN:
    {
      // @@@ ココは割り込みのターゲットとは処理を別ける必要があると思う。いずれやる。
      u8 pokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );
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
    if( intrPokeID != BTL_POKEID_NULL ){
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, intrPokeID );
    }
    TargetPokeRec_Add( rec, bpp );
    return 1;
  }
  else
  {
    return 0;
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
  if( BPP_CheckSick(attacker, WAZASICK_MUSTHIT) ){
    return TRUE;
  }
  if( BPP_CheckSick(attacker, WAZASICK_MUSTHIT_TARGET) )
  {
    u8 targetPokeID = BPP_GetSickParam( attacker, WAZASICK_MUSTHIT_TARGET );
    if( targetPokeID == BPP_GetID(target) ){
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
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;

  TargetPokeRec_GetStart( targets );
  // タイプ相性による無効化チェック
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckNotEffect_byType(wk, wazaParam, attacker, bpp) )
    {
      TargetPokeRec_Remove( targets, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
    }
  }

  // Lv1 無効化チェック（攻撃ポケが必中状態なら無効化できない）
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( !IsMustHit(attacker, bpp) )
    {
      if( scEvent_CheckNotEffect(wk, wazaParam->wazaID, 0, attacker, bpp) )
      {
        TargetPokeRec_Remove( targets, bpp );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
      }
    }
  }

  // Lv2 無効化チェック（攻撃ポケが必中状態でも無効化）
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckNotEffect(wk, wazaParam->wazaID, 1, attacker, bpp) )
    {
      TargetPokeRec_Remove( targets, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
    }
  }

  // まもるチェック
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
    {
      // まもる無効化されなければターゲットから除外
      if( !scEvent_CheckMamoruBreak(wk, attacker, bpp, wazaParam->wazaID) ){
        TargetPokeRec_Remove( targets, bpp );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Mamoru, BPP_GetID(bpp) );
      }
    }
  }
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
static void flowsub_checkWazaAvoid( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  // 命中チェック -> 命中しなかった場合は targets から除外
  BTL_POKEPARAM* bpp;
  u8 pokeID[ BTL_POS_MAX ];
  u8 count = 0;

  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( !scEvent_checkHit(wk, attacker, bpp, waza) )
    {
      pokeID[count++] = BPP_GetID( bpp );
      TargetPokeRec_Remove( targets, bpp );
      scPut_WazaAvoid( wk, bpp, waza );
    }
  }
  if( count )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_WazaAvoid( wk, attacker, waza, count, pokeID );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
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

  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, atkPokeID );
  if( defPokeID != BTL_POKEID_NULL ){
    defPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, defPokeID );
  }else{
    defPos = BTL_POS_NULL;
  }

  SCQUE_PUT_ReservedPos( wk->que, que_reserve_pos, SC_ACT_WAZA_EFFECT, atPos, defPos, waza );
}
//----------------------------------------------------------------------------------
/**
 * 必要なら、１ターン溜めワザの溜めターン処理を行う。
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   action    [io] 溜めターンの場合、攻撃対象の情報をワークに保存／攻撃ターンの場合、それを取り出して格納
 *
 * @retval  BOOL      溜めターン処理を行った場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  if( WAZADATA_GetFlag(waza, WAZAFLAG_TAME) )
  {
    if( !scEvent_CheckTameTurnSkip(wk, attacker, waza) )
    {
      if( !BPP_CONTFLAG_Get(attacker, BPP_CONTFLG_TAME) )
      {
        BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
        BPP_SICK_CONT  sickCont = BTL_CALC_MakeWazaSickCont_Turn( 2 );
        scPut_SetContFlag( wk, attacker, BPP_CONTFLG_TAME );
        scPut_AddSick( wk, attacker, WAZASICK_WAZALOCK, sickCont );
        SCQUE_PUT_ACT_WazaEffectEx( wk->que, atPos, BTL_POS_NULL, waza, BTLV_WAZAEFF_TURN_TAME );
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
static BOOL scproc_Fight_CheckWazaExecuteFail_1st( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;
  PokeSick sick;

  do {
    // こんらん自爆判定
    if( scproc_Fight_CheckConf(wk, attacker) ){
      cause = SV_WAZAFAIL_KONRAN;
      break;
    }
    // メロメロ判定
    if( scproc_Fight_CheckMeroMero(wk, attacker) ){
      cause = SV_WAZAFAIL_MEROMERO;
      break;
    }
    // ねむり・こおり等の解除チェック
    scproc_PokeSickCure_WazaCheck( wk, attacker, waza );

    // ポケモン系 状態異常による失敗チェック
    sick = BPP_GetPokeSick( attacker );
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
    if( BPP_TURNFLAG_Get(attacker, BPP_TURNFLG_SHRINK) ){
      cause = SV_WAZAFAIL_SHRINK;
      break;
    }

  }while( 0 );

  if( cause != SV_WAZAFAIL_NULL ){
    BTL_Printf("ワザ出し失敗（１）  ポケID=%d, 失敗コード=%d\n", BPP_GetID(attacker), cause);
    scproc_WazaExecuteFailed( wk, attacker, waza, cause );
    return TRUE;
  }
  return FALSE;
}

static BOOL scproc_Fight_CheckWazaExecuteFail_2nd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;

  do {
    // かいふくふうじチェック
    if( BPP_CheckSick(attacker, WAZASICK_KAIHUKUHUUJI)
    &&  (WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_SIMPLE_RECOVER)
    ){
      cause = SV_WAZAFAIL_KAIHUKUHUUJI;
      break;
    }

    // その他の失敗チェック
    cause = scEvent_CheckWazaExecute( wk, attacker, waza );
  }while( 0 );

  if( cause != SV_WAZAFAIL_NULL ){
    BTL_Printf("ワザ出し失敗（２）  ポケID=%d, 失敗コード=%d\n", BPP_GetID(attacker), cause);
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
  if( BPP_CheckSick(attacker, WAZASICK_KONRAN) )
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
 * メロメロによるワザだし失敗判定
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    失敗した場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckMeroMero( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker )
{
  if( BPP_CheckSick(attacker, WAZASICK_MEROMERO) )
  {
    scPut_MeromeroAct( wk, attacker );
    if( BTL_CALC_IsOccurPer(BTL_MEROMERO_EXE_PER) )
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
  PokeSick sick = BPP_GetPokeSick( attacker );
  BOOL f_cured = FALSE;

  switch( sick ){
  case POKESICK_NEMURI:
    if( BPP_Nemuri_CheckWake(attacker) ){
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
    scproc_CheckDeadCmd( wk, attacker );
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
// 指定位置から見て対戦相手のポケモンデータを返す
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx )
{
  SVCL_WORK* clwk;
  BtlPokePos targetPos;
  u8 clientID, posIdx;

  targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, pos, pokeSideIdx );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return BTL_PARTY_GetMemberData( clwk->party, posIdx );
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

  return BTL_PARTY_GetMemberData( clwk->party, posIdx );
}
//--------------------------------------------------------------------------
/**
 * [Proc] 使ったワザのPP減少
 *
 * @param   wk
 * @param   attacker  攻撃ポケモンパラメータ
 * @param   wazaIdx   使ったワザインデックス
 * @param   rec       ダメージを受けたポケモンパラメータ群
 *
 */
//--------------------------------------------------------------------------
static void scproc_decrementPPUsedWaza( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, TARGET_POKE_REC* rec )
{
  u8 vol = scEvent_DecrementPPVolume( wk, attacker, rec );
  scproc_decrementPP( wk, attacker, wazaIdx, vol );
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ワザのPP減少
 *
 * @param   wk
 * @param   bpp       対象ポケモンパラメータ
 * @param   wazaIdx   減少させるワザインデックス
 * @param   volume    減少量
 *
 * @retval  BOOL      減少させたらTRUE／死亡時など減少させられなかったらFALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume )
{
  if( !BPP_IsDead(bpp) )
  {
    u8 restPP = BPP_WAZA_GetPP( bpp, wazaIdx );
    if( volume > restPP ){
      volume = restPP;
    }

    if( volume )
    {
      scPut_DecrementPP( wk, bpp, wazaIdx, volume );
      if( scEvent_DecrementPP_Reaction(wk, bpp, wazaIdx) ){
        scproc_UseItemEquip( wk, bpp );
      }
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------
// サーバーフロー：「たたかう」> ダメージワザ系
//----------------------------------------------------------------------
static void scproc_Fight_Damage_Root( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
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
    scproc_Fight_Damage_side( wk, wazaParam, attacker, &wk->targetSubPokemon, dmg_ratio );
  }
// 敵に対するダメージ
  if( TargetPokeRec_CopyEnemys(targets, attacker, &wk->targetSubPokemon) )
  {
    // @@@ 本当は死んでたら別ターゲットにしないとダメかも
    TargetPokeRec_RemoveDeadPokemon( &wk->targetSubPokemon );
    scproc_Fight_Damage_side( wk, wazaParam, attacker, &wk->targetSubPokemon, dmg_ratio );
  }

  scproc_Fight_Damage_After( wk, wazaParam->wazaID, attacker, &wk->damagedPokemon );
}
static void scproc_Fight_Damage_ToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;

  // 回復チェック -> 特定タイプのダメージを無効化、回復してしまうポケを targets から除外
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckDmgToRecover(wk, attacker, bpp, wazaParam) )
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
}
static void scproc_Fight_Damage_Determine( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* targets )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageDetermine( wk, attacker, wazaParam, targets );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

  Hem_PopState( &wk->HEManager, hem_state );
}

static void scEvent_WazaDamageDetermine( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, TARGET_POKE_REC* targets )
{
  u8 target_cnt = TargetPokeRec_GetCount( targets );

  if( target_cnt )
  {
    const BTL_POKEPARAM* bpp;
    u32 i;
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, target_cnt );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      for(i=0; i<target_cnt; ++i)
      {
        bpp = TargetPokeRec_Get( targets, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_DETERMINE );
    BTL_EVENTVAR_Pop();
  }
}
static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  TARGET_POKE_REC* targets, fx32 dmg_ratio )
{
  BTL_POKEPARAM* bpp;
  u8 poke_cnt;

  scproc_Fight_Damage_ToRecover( wk, attacker, wazaParam, targets );
  scproc_Fight_Damage_Determine( wk, attacker, wazaParam, targets );

  TargetPokeRec_GetStart( targets );
  poke_cnt = TargetPokeRec_GetCount( targets );
  if( poke_cnt == 1 )
  {
    BtlTypeAff aff;
    bpp = TargetPokeRec_GetNext( targets );
    aff = scEvent_checkWazaDamageAffinity( wk, bpp, wazaParam->wazaType );
    scproc_Fight_Damage_side_single( wk, attacker, bpp, wazaParam, dmg_ratio, aff );
  }
  else if( poke_cnt > 1 )
  {
    BtlTypeAff affAry[ BTL_POSIDX_MAX ];
    u8 f_same_aff;
    int i, j;

    f_same_aff = TRUE;
    for(i=0; i<poke_cnt; ++i)
    {
      bpp = TargetPokeRec_Get( targets, i );
      // １体でもみがわりがいる場合は個別の処理
      if( BPP_MIGAWARI_IsExist(bpp) ){
        f_same_aff = FALSE;
      }
      affAry[i] = scEvent_checkWazaDamageAffinity( wk, bpp, wazaParam->wazaType );
    }

  // 相性が別々でも個別の処理
    if( f_same_aff )
    {
      for(i=0; i<poke_cnt; ++i){
        for(j=0; j<i; ++j){
          if( BTL_CALC_TypeAffAbout(affAry[i]) != BTL_CALC_TypeAffAbout(affAry[i]) ){
            f_same_aff = FALSE;
            i = poke_cnt; // for loopOut
            break;
          }
        }
      }
    }
    // 相性が１体でも別のヤツがいるなら個別の処理
    if( !f_same_aff ){
      for(i=0; i<poke_cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        scproc_Fight_Damage_side_single( wk, attacker, bpp, wazaParam, dmg_ratio, affAry[i] );
      }
    }
    // 相性が全てのターゲットで一致なら、体力バーを同時に減らすようにコマンド生成する
    else{
      scproc_Fight_damage_side_plural( wk, attacker, targets, affAry, wazaParam, dmg_ratio );
    }
  }
}
//------------------------------------------------------------------
// サーバーフロー下請け：単体ダメージ処理（上位分岐）
//------------------------------------------------------------------
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
      BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
      fx32 targetDmgRatio, BtlTypeAff affinity )
{
  svflowsub_damage_act_singular( wk, attacker, defender, wazaParam, affinity, targetDmgRatio );

  scproc_CheckDeadCmd( wk, defender );
  scproc_CheckDeadCmd( wk, attacker );
}
//------------------------------------------------------------------
// サーバーフロー下請け：単体ダメージ処理（下位）
//------------------------------------------------------------------
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    const SVFL_WAZAPARAM* wazaParam,   BtlTypeAff affinity,   fx32 targetDmgRatio )
{
  u8 plural_flag = FALSE; // 複数回ヒットフラグ
  u8 deadFlag = FALSE;
  u8 hit_count, critical_flag;
  u16 damage;
  u32 damage_sum;
  int i;

  plural_flag = scEvent_CheckPluralHit( wk, attacker, wazaParam->wazaID, &hit_count );

  wazaEff_SetTarget( wk, defender );
  damage_sum = 0;

  for(i=0; i<hit_count; ++i)
  {
    critical_flag = scEvent_CheckCritical( wk, attacker, defender, wazaParam->wazaID );
    damage = scEvent_CalcDamage( wk, attacker, defender, wazaParam,
        affinity, targetDmgRatio, critical_flag );

    // デバッグを簡単にするため必ず大ダメージにする措置
    #ifdef PM_DEBUG
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    {
      u8 pokeID = BPP_GetID(defender);
      if( pokeID >= BTL_PARTY_MEMBER_MAX )
      {
        damage = 999;
      }
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      u8 pokeID = BPP_GetID(defender);
      if( pokeID < BTL_PARTY_MEMBER_MAX )
      {
        damage = 999;
      }
    }
    #endif

    if( BPP_MIGAWARI_IsExist(defender) ){
      scproc_Migawari_Damage( wk, defender, damage );
    }else{
      u8 koraeru_cause = scEvent_CheckKoraeru( wk, attacker, defender, &damage );
      scPut_WazaDamageSingle( wk, wazaParam, defender, affinity, damage, critical_flag, plural_flag );
      if( koraeru_cause != BPP_KORAE_NONE ){
        scPut_Koraeru( wk, defender, koraeru_cause );
      }
      damage_sum += damage;
      scproc_WazaAdditionalEffect( wk, wazaParam, attacker, defender, damage );
      scproc_WazaDamageReaction( wk, attacker, defender, wazaParam, affinity, damage, critical_flag );
      scproc_CheckItemReaction( wk, defender );
      scproc_Fight_Damage_Kickback( wk, attacker, wazaParam->wazaID, damage );
    }

    if( BPP_IsDead(defender) ){
      deadFlag = TRUE;
    }
    if( BPP_IsDead(attacker) ){
      deadFlag = TRUE;
    }
    if( deadFlag ){ break; }
  }

  TargetPokeRec_AddWithDamage( &wk->damagedPokemon, defender, damage_sum );
  if( damage_sum ){
    wazaDmgRec_Add( attacker, defender, wazaParam, damage_sum );
    BPP_TURNFLAG_Set( defender, BPP_TURNFLG_DAMAGED );
  }

  if( plural_flag )
  {
    // @@@ 「●●回あたった！」メッセージ
//    SCQUE_PUT_MSG_WazaHitCount( wk->que, i ); // @@@ あとで
  }
}
//
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, BtlTypeAff* affAry, const SVFL_WAZAPARAM* wazaParam, fx32 dmg_ratio )
{
  BTL_POKEPARAM *bpp[ BTL_POSIDX_MAX ];
  u16 dmg[ BTL_POSIDX_MAX ];
  u32 dmg_sum;
  u8  critical_flg[ BTL_POSIDX_MAX ];
  u8  koraeru_cause[ BTL_POSIDX_MAX ];
  u8 i, poke_cnt;

  poke_cnt = TargetPokeRec_GetCount( targets );
  GF_ASSERT(poke_cnt <= BTL_POSIDX_MAX);

  wazaEff_SetNoTarget( wk );

  dmg_sum = 0;
  for(i=0; i<poke_cnt; ++i)
  {
    bpp[i] = TargetPokeRec_Get( targets, i );
    critical_flg[i] = scEvent_CheckCritical( wk, attacker, bpp[i], wazaParam->wazaID );
    dmg[i] = scEvent_CalcDamage( wk, attacker, bpp[i], wazaParam, affAry[i], dmg_ratio, critical_flg[i] );
    koraeru_cause[i] = scEvent_CheckKoraeru( wk, attacker, bpp[i], &dmg[i] );
    dmg_sum += dmg[i];
    if( dmg[i] ){
      TargetPokeRec_AddWithDamage( &wk->damagedPokemon, bpp[i], dmg[i] );
      wazaDmgRec_Add( attacker, bpp[i], wazaParam, dmg[i] );
      BPP_TURNFLAG_Set( bpp[i], BPP_TURNFLG_DAMAGED );
    }
  }

  scPut_WazaDamagePlural( wk, wazaParam, poke_cnt, affAry[0], bpp, dmg, critical_flg );
  for(i=0; i<poke_cnt; ++i)
  {
    if( koraeru_cause[i] != BPP_KORAE_NONE ){
      scPut_Koraeru( wk, bpp[i], koraeru_cause[i] );
    }
  }

  for(i=0; i<poke_cnt; ++i){
    scproc_WazaAdditionalEffect( wk, wazaParam, attacker, bpp[i], dmg[i] );
    scproc_WazaDamageReaction( wk, attacker, bpp[i], wazaParam, affAry[i], dmg[i], critical_flg[i] );
    scproc_CheckItemReaction( wk, bpp[i] );
  }

  scproc_Fight_Damage_Kickback( wk, attacker, wazaParam->wazaID, dmg_sum );

  for(i=0; i<poke_cnt; ++i)
  {
    scproc_CheckDeadCmd( wk, bpp[i] );
  }
  scproc_CheckDeadCmd( wk, attacker );
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
static void wazaDmgRec_Add( const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, u16 damage )
{
  BPP_WAZADMG_REC rec;
  BPP_WAZADMG_REC_Setup( &rec, BPP_GetID(attacker), wazaParam->wazaID, wazaParam->wazaType, damage );
  BPP_WAZADMGREC_Add( defender, &rec );
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
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
  WazaCategory category = WAZADATA_GetCategory( wazaParam->wazaID );

  if( !BPP_IsDead(defender) )
  {
    switch( category ){
    case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
      scproc_Fight_Damage_AddEffect( wk, wazaParam, attacker, attacker );
      break;
    case WAZADATA_CATEGORY_DAMAGE_EFFECT:
      scproc_Fight_Damage_AddEffect( wk, wazaParam, attacker, defender );
      break;
    case WAZADATA_CATEGORY_DAMAGE_SICK:
      scproc_Fight_Damage_AddSick( wk, wazaParam, attacker, defender );
      break;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ワザダメージに対する反応処理
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   affinity
 * @param   damage
 * @param   critical_flag
 */
//----------------------------------------------------------------------------------
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, BOOL critical_flag )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageReaction( wk, attacker, defender, wazaParam, affinity, damage, critical_flag );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * アイテム反応チェック
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_CheckItemReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_CheckItemReaction( wk, bpp );
  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );
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
    BTL_SICK_AddProc( wk, target, sick );

    if( msgCallbackParam == NULL ){
      BTL_HANDEX_STR_PARAMS   str;
      BTL_SICK_MakeDefaultMsg( sick, sickCont, target, &str );
      handexSub_putString( wk, &str );
    }else{
      msgCallbackParam->func( wk, target, msgCallbackParam->arg );
    }

    scproc_CheckItemReaction( wk, target );
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
  if( BPP_CheckSick(target, sick) ){
    return BTL_ADDSICK_FAIL_ALREADY;
  }

  // すでにポケモン系状態異常になっているなら、新たにポケモン系状態異常にはならない
  if( sick < POKESICK_MAX )
  {
    if( BPP_GetPokeSick(target) != POKESICK_NULL ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // @@@ すでに混乱なら混乱にならないとか… 失敗コードを返す必要があるかも
  if( BPP_CheckSick(target, sick) ){
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
  if( (BPP_GetPokeSick(target) != POKESICK_NULL)
  &&  (sick < POKESICK_MAX)
  ){
    return TRUE;
  }

  // @@@ すでに混乱なら混乱にならないとか… 失敗コードを返す必要があるかも
  if( BPP_CheckSick(target, sick) ){
    return TRUE;
  }

  return FALSE;
}
static BOOL scEvent_AddSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    if( attacker ){
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_FAILED );
  BTL_EVENTVAR_Pop();
}
//------------------------------------------------------------------
// サーバーフロー：ダメージ受け後の処理
//------------------------------------------------------------------
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  scproc_Fight_Damage_Drain( wk, waza, attacker, targets );
  scproc_Fight_Damage_Shrink( wk, waza, attacker, targets );

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
static void scproc_Fight_Damage_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  u32 waza_per = scEvent_GetWazaShrinkPer( wk, waza, attacker );
  BTL_POKEPARAM* bpp;
  u32 cnt, per, i=0;

  while( (bpp = TargetPokeRec_Get(targets, i++)) != NULL )
  {
    if( !BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_ACTION_DONE) )
    {
      if( BPP_TURNFLAG_Get( bpp, BPP_TURNFLG_MUST_SHRINK ) ){
        per = 100;
      }else{
        per = waza_per;
      }
      if( per ){
        scproc_AddShrinkCore( wk, bpp, waza, per );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 対象ポケモンにひるみ効果を与える
 *
 * @param   wk
 * @param   target
 * @param   waza    ワザ追加効果によるひるみの場合、ワザナンバー（それ以外 WAZANO_NULL）
 * @param   per     確率（パーセンテージ）
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_AddShrinkCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaID waza, u32 per )
{
  if( scEvent_CheckShrink(wk, target, waza, per) )
  {
    BPP_TURNFLAG_Set( target, BPP_TURNFLG_SHRINK );
    return TRUE;
  }
  else if( per >= 100 )
  {
    // 確率100％なのに失敗したら原因表示へ
    u32 hem_state = Hem_PushState( &wk->HEManager );
    scEvent_FailShrink( wk, target );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
  return FALSE;
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
    u32 damage, recoverHP;
    u32 i;

    hem_state = Hem_PushState( &wk->HEManager );
    TargetPokeRec_GetStart( targets );
    while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
    {
      damage = TargetPokeRec_GetDamage( targets, bpp );
      total_damage += damage;
      recoverHP = (WAZADATA_GetDrainRatio(waza) * damage) / 100;
      //最低でも1は回復するようにする
      if( recoverHP == 0 ) recoverHP = 1;

      if( scproc_DrainCore(wk, attacker, bpp, recoverHP) )
      {
        scPut_Message_Set( wk, bpp, BTL_STRID_SET_Drain );
      }
    }
  }

  {
    hem_state = Hem_PushState( &wk->HEManager );
    scEvent_CheckSpecialDrain( wk, waza, attacker, total_damage );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
  }
}
//
static BOOL scproc_DrainCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP )
{
  u32 hem_state = Hem_PushState( &wk->HEManager );
  BOOL result = FALSE;

  drainHP = scEvent_RecalcDrainVolume( wk, attacker, target, drainHP );
  if( drainHP && !BPP_IsDead(attacker) )
  {
    if( scproc_RecoverHP(wk, attacker, drainHP) ){
      result = TRUE;
    }
  }

  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  Hem_PopState( &wk->HEManager, hem_state );

  return result;
}


//------------------------------------------------------------------
// サーバーフロー下請け： たたかう > ダメージワザ系 > ダメージ値計算
//------------------------------------------------------------------
static u16 scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag )
{
  WazaDamageType dmgType = WAZADATA_GetDamageType( wazaParam->wazaID );
  u32 rawDamage = 0;

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_TYPEAFF, typeAff );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
  BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
  BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE_TYPE, dmgType );

  {
    u16 atkPower, defGuard, wazaPower;

    // 各種パラメータから素のダメージ値計算
    wazaPower = scEvent_getWazaPower( wk, attacker, defender, wazaParam );
    atkPower  = scEvent_getAttackPower( wk, attacker, defender, wazaParam->wazaID, criticalFlag );
    defGuard  = scEvent_getDefenderGuard( wk, attacker, defender, wazaParam, criticalFlag );
    {
      u8 atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
      rawDamage = wazaPower * atkPower * (atkLevel*2/5+2) / defGuard / 50;
    }
    rawDamage = (rawDamage * targetDmgRatio) >> FX32_SHIFT;
    // 天候補正
    {
      fx32 weatherDmgRatio = BTL_FIELD_GetWeatherDmgRatio( wazaParam->wazaID );
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
    fx32 ratio = scEvent_CalcTypeMatchRatio( wk, attacker, wazaParam->wazaType );
    rawDamage = (rawDamage * ratio) >> FX32_SHIFT;
    if( ratio != FX32_ONE ){
      BTL_Printf("タイプ一致補正:%d%%  -> 素ダメ=%d\n", (ratio*100>>FX32_SHIFT), rawDamage);
    }
  }
  // タイプ相性計算
  rawDamage = BTL_CALC_AffDamage( rawDamage, typeAff );
  BTL_Printf("タイプ相性:%02d -> ダメージ値：%d\n", typeAff, rawDamage);
  // やけど補正
  if( (dmgType == WAZADATA_DMG_PHYSIC)
  &&  (BPP_GetPokeSick(attacker) == POKESICK_YAKEDO)
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
          BPP_GetID(defender), ratio, BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE ), rawDamage);
  }

  // 最高で残りＨＰの範囲に収まるように最終補正
  if( rawDamage > BPP_GetValue(defender, BPP_HP) ){
    rawDamage = BPP_GetValue( defender, BPP_HP );
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
    BTL_HANDEX_STR_PARAMS  str;
    HANDEX_STR_Setup( &str, BTL_STRTYPE_SET, BTL_STRID_SET_ReactionDmg );
    HANDEX_STR_AddArg( &str, BPP_GetID(attacker) );
    scproc_SimpleDamage( wk, attacker, damage, &str );
  }
}
//---------------------------------------------------------------------------------------------
// ワザ以外のダメージ共通処理
//---------------------------------------------------------------------------------------------
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, const BTL_HANDEX_STR_PARAMS* str )
{
  if( scEvent_CheckEnableSimpleDamage(wk, bpp, damage) )
  {
    int value = -damage;

    if( value )
    {
      if( str != NULL ){
        handexSub_putString( wk, str );
      }

      scPut_SimpleHp( wk, bpp, value );
      BPP_TURNFLAG_Set( bpp, BPP_TURNFLG_DAMAGED );

      scproc_CheckItemReaction( wk, bpp );
      scproc_CheckDeadCmd( wk, bpp );

      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Proc] 装備アイテム使用
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  特殊要因により装備アイテムの使用が出来なければFALSE／それ以外TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u32 itemID = BPP_GetItem( bpp );
  u32 hem_state = Hem_PushState( &wk->HEManager );

  scEvent_ItemEquip( wk, bpp );

  if( ITEM_CheckNuts(itemID) ){
    scPut_EatNutsAct( wk, bpp );
  }

  scproc_HandEx_Root( wk, itemID );
  Hem_PopState( &wk->HEManager, hem_state );

  if( ITEM_CheckNuts(itemID) ){
    scPut_RemoveItem( wk, bpp );
  }
  return TRUE;
}
//---------------------------------------------------------------------------------------------
// ポケモンを強制的に瀕死にする
//---------------------------------------------------------------------------------------------
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_KillPokemon( wk, bpp, 0 );
  scproc_CheckDeadCmd( wk, bpp );
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ダメージワザの追加効果による状態異常
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  WazaSick sick;
  BPP_SICK_CONT sick_cont;
  u32 i=0;

  sick = scEvent_CheckAddSick( wk, wazaParam->wazaID, attacker, target, &sick_cont );
  if( sick != WAZASICK_NULL )
  {
    if( (!BPP_IsDead(target))
    &&  (!scEvent_AddSick_IsMustFail(wk, target, sick))
    ){
      scproc_Fight_WazaSickCore( wk,  attacker, target, wazaParam->wazaID, sick, sick_cont, FALSE );
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
      BPP_SICK_CONT sickCont;
      BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );
      scproc_Fight_WazaSickCore( wk, attacker, target, waza, sick, sickCont, TRUE );
    }
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

  scEvent_WazaSickCont( wk, attacker, target, sick, &sickCont );

  return scproc_AddSick( wk, target, attacker, sick, sickCont, fAlmost, NULL );
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ダメージワザの追加効果によるランク効果
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  if( !BPP_IsDead(target) )
  {
    if( scEvent_CheckAddRankEffectOccur(wk, wazaParam, attacker, target) )
    {
      scproc_WazaRankEffect_Common( wk, wazaParam, attacker, target, FALSE );
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる直接のランク効果
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  BTL_POKEPARAM* target;
  u32 i = 0;

  TargetPokeRec_GetStart( targetRec );
  while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wk, wazaParam, attacker, target, TRUE) )
    {
      wazaEff_SetTarget( wk, target );
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
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost )
{
  BOOL ret = FALSE;
  u32 eff_cnt, i;

  eff_cnt = WAZADATA_GetRankEffectCount( wazaParam->wazaID );
  for(i=0; i<eff_cnt; ++i)
  {
    WazaRankEffect  effect;
    int  volume;

    scEvent_GetWazaRankEffectValue( wk, wazaParam->wazaID, i, attacker, target, &effect, &volume );
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
  if( !BPP_IsRankEffectValid(target, effect, volume) ){
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
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  BPP_SICK_CONT sickCont;
  BTL_POKEPARAM* target;
  u32 i = 0;

  sick = WAZADATA_GetSick( wazaParam->wazaID );
  contParam = WAZADATA_GetSickCont( wazaParam->wazaID );
  BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );

  TargetPokeRec_GetStart( targetRec );
  while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wk, wazaParam, attacker, target, TRUE) ){
      wazaEff_SetTarget( wk, target );
    }
    if( scproc_Fight_WazaSickCore(wk, attacker, target, wazaParam->wazaID, sick, sickCont, TRUE) ){
      wazaEff_SetTarget( wk, target );
    }
  }
}
//---------------------------------------------------------------------------------------------
// サーバーフロー：HP回復
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker )
{
  u32 recoverHP = scEvent_CalcRecoverHP( wk, waza, attacker );

  if( scproc_RecoverHP(wk, attacker, recoverHP) ){
    u8 pokeID = BPP_GetID( attacker );
    wazaEff_SetTarget( wk, attacker );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HP_Recover, pokeID );
  }
}
static BOOL scproc_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP )
{
  if( !BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI)
  &&  !BPP_IsDead(bpp)
  ){
    scPut_SimpleHp( wk, bpp, recoverHP );
    return TRUE;
  }
  return FALSE;
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
    if( !BPP_IsDead(target) )
    {
      u8 atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
      u8 defLevel = BPP_GetValue( target, BPP_LEVEL );
      u8 targetPokeID = BPP_GetID( target );
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
    BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(target) );

    if( BPP_IsDead(target) ){
      scPut_WazaFail( wk, attacker, waza );
      return;
    }
    if( scEvent_CheckPushOutFail(wk, attacker, target) ){
      return;
    }

    {
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
          SCQUE_PUT_ACT_MemberOut( wk->que, targetPos );
          scproc_MemberIn( wk, clientID, posIdx, nextPokeIdx, FALSE );
          scproc_AfterMemberIn( wk );
        }
      }
      else
      {
        SCQUE_PUT_ACT_MemberOut( wk->que, targetPos );
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
  case BTL_RULE_SINGLE:
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
    if( !BPP_IsDead(bpp) ){
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_PUSHOUT );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}

//---------------------------------------------------------------------------------------------
// サーバーフロー：ワザによる天候の変化
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Weather( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker )
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
static void scput_Fight_Uncategory( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;

  // ターゲットは居たはずだがモロモロの理由で残っていない場合は終了
  if( (TargetPokeRec_GetCountMax(targets) > 0)
  &&  (TargetPokeRec_GetCount(targets) == 0)
  ){
    return;
  }

  switch( wazaParam->wazaID ){
  case WAZANO_SUKIRUSUWAPPU:
    scput_Fight_Uncategory_SkillSwap( wk, attacker, targets );
    break;
  case WAZANO_HENSIN:
    scput_Fight_Uncategory_Hensin( wk, attacker, targets );
    break;
  case WAZANO_MIGAWARI:
    scproc_Migawari_Create( wk, attacker );
    break;
  default:
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      if( scEvent_UnCategoryWaza(wk, wazaParam, attacker, targets) ){
        if( scproc_HandEx_Root( wk, ITEM_DUMMY_DATA ) ){
          wazaEff_SetNoTarget( wk );
        }
      }
      Hem_PopState( &wk->HEManager, hem_state );
    }
    break;
  }
}
// スキルスワップ
static void scput_Fight_Uncategory_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  BTL_POKEPARAM* target = TargetPokeRec_Get( targetRec, 0 );
  PokeTokusei atk_tok, tgt_tok;

  atk_tok = BPP_GetValue( attacker, BPP_TOKUSEI );
  tgt_tok = BPP_GetValue( target, BPP_TOKUSEI );

  if( (!BTL_CALC_TOK_CheckCant_Swap(atk_tok)) && (!BTL_CALC_TOK_CheckCant_Swap(tgt_tok)) )
  {
    u8 atkPokeID = BPP_GetID( attacker );
    u8 tgtPokeID = BPP_GetID( target );
    scPut_WazaEffectOn( wk, attacker, target, WAZANO_SUKIRUSUWAPPU );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_SkillSwap, atkPokeID );

    BPP_ChangeTokusei( attacker, tgt_tok );
    BPP_ChangeTokusei( target, atk_tok );
    BTL_HANDLER_TOKUSEI_Swap( attacker, target );

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, atkPokeID );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, tgtPokeID );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKILL_SWAP );
    BTL_EVENTVAR_Pop();
  }
}
// へんしん
static void scput_Fight_Uncategory_Hensin( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
  BTL_POKEPARAM* target = TargetPokeRec_Get( targetRec, 0 );

  if( BPP_HENSIN_Set(attacker, target) )
  {
    u8 atkPokeID = BPP_GetID( attacker );
    u8 tgtPokeID = BPP_GetID( target );

    scPut_WazaEffectOn( wk, attacker, target, WAZANO_HENSIN );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Hensin, atkPokeID, tgtPokeID );

    BTL_HANDLER_Waza_RemoveForceAll( attacker );
    BTL_HANDLER_TOKUSEI_Remove( attacker );
    BTL_HANDLER_TOKUSEI_Add( attacker );

    SCQUE_PUT_OP_Hensin( wk->que, atkPokeID, tgtPokeID );
  }
}

//----------------------------------------------------------------------------------
/**
 * みがわり - 作成
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_Migawari_Create( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  if( !BPP_MIGAWARI_IsExist(bpp) ){
    int migawariHP = BTL_CALC_QuotMaxHP( bpp, 4 );
    if( BPP_GetValue(bpp, BPP_HP) > migawariHP )
    {
      scPut_SimpleHp( wk, bpp, -migawariHP );
      scproc_CheckItemReaction( wk, bpp );

      BPP_MIGAWARI_Create( bpp, migawariHP );
      SCQUE_PUT_OP_MigawariCreate( wk->que, BPP_GetID(bpp), migawariHP );
      // @@@ ここでたぶん身代わり作成エフェクトコマンド
    }
    else{
      scPut_Message_StdEx( wk, BTL_STRID_STD_MigawariFail, 0, NULL );
    }
  }
  else{
    // すでに出ていた
    scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariExist );
  }
}
//----------------------------------------------------------------------------------
/**
 * みがわり - ダメージ処理
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  みがわりが消えたらTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Migawari_Damage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 damage )
{
  scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariDamage );
  if( BPP_MIGAWARI_AddDamage(bpp, damage) )
  {
     SCQUE_PUT_OP_MigawariDelete( wk->que, BPP_GetID(bpp) );
     // @@@ ここでたぶん身代わり削除エフェクトコマンド
     scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariDestract );
     return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * みがわり - ダメージワザ以外の無効化判定
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker    攻撃ポケモンデータ
 * @param   rec         [io] ワザ対象ポケデータ群。無効化されたら該当ポケモンデータは除去される。
 */
//----------------------------------------------------------------------------------
static void scproc_Migawari_CheckNoEffect( BTL_SVFLOW_WORK* wk, SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, TARGET_POKE_REC* rec )
{
  BTL_POKEPARAM* bpp;

  TargetPokeRec_GetStart( rec );
  while( (bpp = TargetPokeRec_GetNext(rec)) != NULL )
  {
    if( (bpp != attacker)
    &&  (BPP_MIGAWARI_IsExist(bpp))
    ){
      TargetPokeRec_Remove( rec, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(bpp) );
    }
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
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  u16 sp = Hem_GetStackPtr( &wk->HEManager );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    {
      BTL_POKEPARAM* bpp;
      u8 cnt, i;
      cnt = TargetPokeRec_GetCount( targets );
      BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, cnt );
      for(i=0; i<cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      if( cnt ){
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORIZE_WAZA );
      }else{
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET );
      }
    }
  BTL_EVENTVAR_Pop();

  return (sp != Hem_GetStackPtr(&wk->HEManager));
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
      BPP_TurnCheck( bpp );
    }
  }

  // シューターのエネルギーチャージ
  scproc_countup_shooter_energy( wk );

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

//----------------------------------------------------------------------------------
/**
 * シューターのエネルギーチャージ計算＆各クライアントに通知
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_countup_shooter_energy( BTL_SVFLOW_WORK* wk )
{
  if( wk->bagMode == BBAG_MODE_SHOOTER )
  {
    u32 i;
    for(i=0; i<wk->numClient; ++i)
    {
      if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_TRIPLE ){
//        SCQUE_PUT_OP_ShooterCharge( wk->que, i, 1 );
        SCQUE_PUT_OP_ShooterCharge( wk->que, i, 13 );  // @@@ テスト用措置
      }else{
        u8 myClientID, opponentClientID, emptyCnt, fEnable, p;
        u8 emptyPos[ BTL_POSIDX_MAX ];

        myClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
        opponentClientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, myClientID, 0 );

        fEnable = TRUE;
        emptyCnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, myClientID, emptyPos );
        for(p=0; p<emptyCnt; ++p)
        {
          if( BTL_MAINUTIL_IsTripleCenterPos(emptyPos[p]) ){
            fEnable = FALSE;  // 自分の中央が空いていたらチャージされない
            break;
          }
        }
        if( fEnable )
        {
          u8 value = 1 + emptyCnt;  // 基本は自分の空き位置 + 1
          emptyCnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, opponentClientID, emptyPos );
          for(p=0; p<emptyCnt; ++p){
            if( BTL_MAINUTIL_IsTripleCenterPos(emptyPos[p]) ){
              value *= 2;   // 相手の中央が空いていたら２倍
              break;
            }
          }
          SCQUE_PUT_OP_ShooterCharge( wk->que, i, value );
        }
      }
    }
  }
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
  u32 hem_state;
  u8 pokeID;

  FRONT_POKE_SEEK_InitWork( &fpsw, wk );

  while( FRONT_POKE_SEEK_GetNext( &fpsw, wk, &bpp ) )
  {
    hem_state = Hem_PushState( &wk->HEManager );

    BPP_WazaSick_TurnCheck( bpp, BTL_SICK_TurnCheckCallback, wk );
    SCQUE_PUT_OP_WazaSickTurnCheck( wk->que, BPP_GetID(bpp) );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

    Hem_PopState( &wk->HEManager, hem_state );
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
      if( !BPP_IsDead(bpp) )
      {
        dmg_tmp = scEvent_CalcWeatherDamage( wk, weather, bpp, &tok_flg );
        if( dmg_tmp )
        {
          if( tok_flg ){
            tok_pokeID[ tok_poke_cnt++ ] = BPP_GetID( bpp );
          }
          damage[ poke_cnt ] = dmg_tmp;
          pokeID[ poke_cnt ] = BPP_GetID( bpp );
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
          BPP_HpMinus( bpp, damage[i] );
          SCQUE_PUT_OP_HpMinus( wk->que, pokeID[i], damage[i] );
        }else{
          BPP_HpPlus( bpp, -damage[i] );
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
        scproc_CheckDeadCmd( wk, BTL_POKECON_GetPokeParam(wk->pokeCon, pokeID[i]) );
      }
    }
  }
}
//--------------------------------------------------------------------------
/**
 * 対象ポケモンが死んでいたら死亡処理＆必要コマンド生成
 */
//--------------------------------------------------------------------------
static void scproc_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke )
{
  u8 pokeID = BPP_GetID( poke );

  if( wk->pokeDeadFlag[pokeID] == 0 )
  {
    if( BPP_IsDead(poke) )
    {
      BTL_Printf("ポケ[ID=%d]しにます\n", pokeID);
      wk->pokeDeadFlag[pokeID] = 1;
      wk->numDeadPoke++;

      // @@@ みがわり出てたら画面から消すコマンド生成？

      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, pokeID );
      SCQUE_PUT_ACT_Dead( wk->que, pokeID );
      scproc_GetExp( wk, poke );

      BPP_Clear_ForDead( poke );
      scproc_ClearPokeDependEffect( wk, poke );
      BTL_POSPOKE_PokeOut( &wk->pospokeWork, pokeID );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 経験値取得処理コマンド生成
 *
 * @param   wk
 * @param   deadPoke    死んだポケモン
 */
//----------------------------------------------------------------------------------
static void scproc_GetExp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* deadPoke )
{
  BtlCompetitor  competitor = BTL_MAIN_GetCompetitor( wk->mainModule );
  if( (competitor == BTL_COMPETITOR_WILD)
  ||  (competitor == BTL_COMPETITOR_TRAINER)
  ){
    u8 deadPokeID = BPP_GetID( deadPoke );
    if( BTL_MAINUTIL_PokeIDtoSide(deadPokeID) == BTL_SIDE_2ND )
    {
      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, BTL_MAIN_GetPlayerClientID(wk->mainModule) );

      BTL_Printf("経験値処理へ来ました\n");

      getexp_calc( wk, party, deadPoke, wk->calcExpWork );
      getexp_make_cmd( wk, party, wk->calcExpWork );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 経験値計算結果を専用ワークに出力
 *
 * @param   wk
 * @param   party     プレイヤー側パーティ
 * @param   deadPoke  しんだポケモン
 * @param   result    [out] 計算結果格納先
 */
//----------------------------------------------------------------------------------
static void getexp_calc( BTL_SVFLOW_WORK* wk, const BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result )
{
  u32 baseExp = BTL_CALC_CalcBaseExp( deadPoke );
  u16 memberCount  = BTL_PARTY_GetMemberCount( party );
  u16 gakusyuCount = 0;

  const BTL_POKEPARAM* bpp;
  u16 i;

  // ワーククリア
  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i){
    result[i].exp = 0;
  }

  // がくしゅうそうち分の経験値計算
  for(i=0; i<memberCount; ++i)
  {
    // 生きていてがくしゅうそうちを装備しているポケモンが対象
    bpp = BTL_PARTY_GetMemberDataConst( party, i );
    if( !BPP_IsDead(bpp)
    &&  (BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX)
    &&  (BPP_GetItem(bpp) == ITEM_GAKUSYUUSOUTI)
    ){
      ++gakusyuCount;
    }
  }
  if( gakusyuCount )
  {
    u32 gakusyuExp = baseExp / 2;
    baseExp -= gakusyuExp;

    for(i=0; i<memberCount; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp)
      &&  (BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX)
      &&  (BPP_GetItem(bpp) == ITEM_GAKUSYUUSOUTI)
      ){
        result[i].exp = gakusyuExp / gakusyuCount;
      }
    }
  }

  BTL_Printf("基本となる経験値=%d\n", baseExp );


  // 対戦経験値取得
  {
    u8 confrontCnt = BPP_CONFRONT_REC_GetCount( deadPoke );
    u8 aliveCnt = 0;
    u8 pokeID;

    for(i=0; i<confrontCnt; ++i)
    {
      pokeID = BPP_CONFRONT_REC_GetPokeID( deadPoke, i );
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      if( !BPP_IsDead(bpp) ){
        ++aliveCnt;
      }
    }
    BTL_Printf("死亡ポケが対面した相手ポケの数=%d, その内、生きてる数=%d\n", confrontCnt, aliveCnt);

    for(i=0; i<memberCount; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp) && (BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX) )
      {
        u8 j;
        pokeID = BPP_GetID( bpp );
        for(j=0; j<confrontCnt; ++j)
        {
          if( BPP_CONFRONT_REC_GetPokeID(deadPoke, j) == pokeID ){
            u32 addExp = (baseExp / aliveCnt );
            BTL_Printf("メンバーIdx[%d]のポケに経験値%dを分配\n", i, (baseExp/aliveCnt));
            result[i].exp += (baseExp / aliveCnt);
          }
        }
      }
    }
  }

  // ボーナス計算
  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    if( result[i].exp )
    {
      const MYSTATUS* status = BTL_MAIN_GetPlayerStatus( wk->mainModule );
      const POKEMON_PARAM* pp;
      u16 myLv, enemyLv;
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      pp = BPP_GetSrcData( bpp );

      // 倒したポケとのレベル差によって経験値が増減する（WBから追加された仕様）
      myLv = BPP_GetValue( bpp, BPP_LEVEL );
      enemyLv = BPP_GetValue( deadPoke, BPP_LEVEL );
      result[i].exp = getexp_calc_adjust_level( result[i].exp, myLv, enemyLv );

#ifndef SOGA_DEBUG
      //現状ポケモンに正しいIDと親の名前がはいっていないため、常に「おおめに」になってしまうため、
      //不本意ながら10末Verではこの処理を外しておく
      // 他人が親ならボーナス
      if( !PP_IsMatchOya(pp, status) )
      {
        // 外国の親ならx1.7，そうじゃなければx1.5
        fx32 ratio;
        ratio = ( PP_Get(pp, ID_PARA_country_code, NULL) != MyStatus_GetRegionCode(status) )?
            FX32_CONST(1.7f) : FX32_CONST(1.5f);

        result[i].exp = BTL_CALC_MulRatio( result[i].exp, ratio );
        result[i].fBonus = TRUE;
      }
#endif
      // しあわせタマゴ持ってたらボーナス x1.5
      if( BPP_GetItem(bpp) == ITEM_SIAWASETAMAGO ){
        result[i].exp = BTL_CALC_MulRatio( result[i].exp, FX32_CONST(1.5f) );
        result[i].fBonus = TRUE;
      }

      BTL_Printf("メンバーIdx[%d]のポケに対し、最終経験値=%d\n", i, result[i].exp);
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * 倒されたポケモンと経験値を取得するポケモンのレベル差に応じて経験値を補正する（ＷＢより）
 *
 * @param   base_exp      基本経験値（GSまでの仕様による計算結果）
 * @param   getpoke_lv    経験値を取得するポケモンのレベル
 * @param   deadpoke_lv   倒されたポケモンのレベル
 *
 * @retval  u32   補正後経験値
 */
//----------------------------------------------------------------------------------
static u32 getexp_calc_adjust_level( u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv )
{
  fx32 denom, numer, ratio;
  u32  denom_int, numer_int, result;

  numer_int = deadpoke_lv * 2 + 10;
  denom_int = deadpoke_lv + getpoke_lv + 10;

  numer  = ( (numer_int * numer_int) * FX_Sqrt(FX32_CONST(numer_int)) );
  denom  = ( (denom_int * denom_int) * FX_Sqrt(FX32_CONST(denom_int)) );
  ratio = FX_Div( numer, denom );

  result = ((base_exp * ratio) >> FX32_SHIFT) + 1;

  BTL_Printf( "自分Lv=%d, 敵Lv=%d, 基本経験値=%d -> 補正後経験値=%d\n",
      getpoke_lv, deadpoke_lv, base_exp, result );

  return result;
}

//----------------------------------------------------------------------------------
/**
 * 経験値計算結果を元にコマンド生成
 *
 * @param   wk
 * @param   party     プレイヤー側パーティ
 * @param   calcExp   経験値計算結果ワーク
 */
//----------------------------------------------------------------------------------
static void getexp_make_cmd( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const CALC_EXP_WORK* calcExp )
{
  BTL_POKEPARAM* bpp;
  u32 i;

  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    if( calcExp[i].exp )
    {
      BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberData( party, i );
      u32 exp   = calcExp[i].exp;
      u16 strID = (calcExp[i].fBonus)? BTL_STRID_STD_GetExp_Bonus : BTL_STRID_STD_GetExp;
      u8  pokeID = BPP_GetID( bpp );
      BTL_LEVELUP_INFO  info;

      BTL_Printf("経験値はいったメッセージ :strID=%d, pokeID=%d, exp=%d\n", strID, pokeID, exp);
      SCQUE_PUT_MSG_STD( wk->que, strID, pokeID, exp );

      while( exp )
      {
        if( BPP_AddExp(bpp, &exp, &info) )
        {
          BTL_Printf("レベルアップする経験値: exp=%d\n", exp);
          SCQUE_PUT_ACT_AddExpLevelup( wk->que, pokeID,
            info.level, info.hp, info.atk, info.def, info.sp_atk, info.sp_def, info.agi );
        }
        else
        {
          BTL_Printf("レベルアップしない経験値増: exp=%d\n", exp);
          SCQUE_PUT_ACT_AddExp( wk->que, pokeID, exp );
          break;
        }
      }
    }
  }
}

//--------------------------------------------------------------------------
/**
 * 特定ポケモン依存の状態異常・サイドエフェクト等、各種ハンドラをクリアする
 */
//--------------------------------------------------------------------------
static void scproc_ClearPokeDependEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* poke )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 dead_pokeID = BPP_GetID( poke );

  BTL_HANDLER_TOKUSEI_Remove( poke );
  BTL_HANDLER_ITEM_Remove( poke );
  BTL_HANDLER_Waza_RemoveForceAll( poke );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    BPP_CureWazaSickDependPoke( bpp, dead_pokeID );
  }
  BTL_FIELD_RemoveDependPokeEffect( dead_pokeID );
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
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_Message_Set( wk, bpp, BTL_STRID_SET_CantAction );
}
//--------------------------------------------------------------------------
/**
 * 「○○はこんらんしている」表示
 */
//--------------------------------------------------------------------------
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAct, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * 「○○は△△にメロメロだ」表示
 */
//--------------------------------------------------------------------------
static void scPut_MeromeroAct( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  u8 target_pokeID = BPP_GetSickParam( bpp, WAZASICK_MEROMERO );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MeromeroAct, BPP_GetID(bpp), target_pokeID );
}
//--------------------------------------------------------------------------
/**
 * こんらんによる自爆ダメージ処理
 */
//--------------------------------------------------------------------------
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage )
{
  u8 pokeID = BPP_GetID( bpp );

  BPP_HpMinus( bpp, damage );
  SCQUE_PUT_OP_HpMinus( wk->que, pokeID, damage );
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_KonranExe, pokeID );
  SCQUE_PUT_ACT_ConfDamage( wk->que, pokeID );
}
//--------------------------------------------------------------------------
/**
 * ワザ出し判定時のポケモン系状態異常回復処理
 */
//--------------------------------------------------------------------------
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick )
{
  u8 pokeID = BPP_GetID( bpp );

  BPP_CurePokeSick( bpp );
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
  SCQUE_PUT_MSG_WAZA( wk->que, BPP_GetID(attacker), waza );
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
static WazaSick scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWazaSickEx exSickCode, BPP_SICK_CONT* oldCont )
{
  WazaSick sick = trans_sick_code( bpp, exSickCode );

  if( sick != WAZASICK_NULL )
  {
    u8 pokeID = BPP_GetID( bpp );

    *oldCont = BPP_GetSickCont( bpp, sick );

    if( sick < POKESICK_MAX ){
      BPP_CurePokeSick( bpp );
      SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
    }else{
      BPP_CureWazaSick( bpp, sick );
      SCQUE_PUT_OP_CureWazaSick( wk->que, pokeID, sick );
    }
  }
  return sick;
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
  PokeSick poke_sick = BPP_GetPokeSick( bpp );

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
      &&  (BPP_CheckSick(bpp, WAZASICK_KONRAN))
      ){
        result = WAZASICK_KONRAN;
      }
    }
  }
  // 通常コード時は罹患していない状態異常を指定されたらNULLを返す
  else{
    if( BPP_CheckSick(bpp, exCode) ){
      result = exCode;
    }else{
      result = WAZASICK_NULL;
    }
  }

  return result;
}
//--------------------------------------------------------------------------
/**
 * ワザ出し失敗メッセージ表示
 */
//--------------------------------------------------------------------------
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
{
  u8 pokeID = BPP_GetID( bpp );

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
  case SV_WAZAFAIL_MEROMERO:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MeromeroExe, pokeID );
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
  BPP_ResetWazaContConter( bpp );
  SCQUE_PUT_OP_ResetUsedWazaCounter( wk->que, BPP_GetID(bpp) );
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
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, BPP_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * 「○○にはこうかがないようだ」表示
 */
//--------------------------------------------------------------------------
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * [Put] ダメージ受け->回復化処理
 */
//--------------------------------------------------------------------------
static void scput_DmgToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BTL_EVWK_DMG_TO_RECOVER* evwk )
{
  u8 pokeID = BPP_GetID( bpp );

  if( evwk->tokFlag ){
    SCQUE_PUT_TOKWIN_IN( wk->que, pokeID );
  }

  if( evwk->recoverHP != 0 )
  {
    wazaEff_SetTarget( wk, bpp );
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_SET_HP_Recover, pokeID );
    BPP_HpPlus( bpp, evwk->recoverHP );
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
 * [Put] ワザエフェクト発動
 */
//--------------------------------------------------------------------------
static void scPut_WazaEffectOn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza )
{
  // １つのワザ処理ごとに１回しか作らない
  if( FlowFlg_Get(wk, FLOWFLG_SET_WAZAEFFECT ) == FALSE )
  {
    u8 atkPokePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
    u8 defPokePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(target) );

    BTL_Printf("ワザエフェクトコマンド生成します, ワザナンバ%d, 位置%d->%d\n", waza, atkPokePos, defPokePos);
    SCQUE_PUT_ACT_WazaEffect( wk->que, atkPokePos, defPokePos, waza );
    FlowFlg_Set( wk, FLOWFLG_SET_WAZAEFFECT );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ワザによるダメージ（単体）
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage, BOOL critical_flag, BOOL plural_hit_flag )
{
  u8 pokeID = BPP_GetID( defender );

  // 複数回ヒットは最後に１回だけ「効果は○○だ！」を出すため、ここでは無効化
  if( plural_hit_flag ){
    aff = BTL_TYPEAFF_100;
  }

  BPP_HpMinus( defender, damage );
  SCQUE_PUT_OP_HpMinus( wk->que, pokeID, damage );
  SCQUE_PUT_ACT_WazaDamage( wk->que, pokeID, aff, damage, wazaParam->wazaID );
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
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  u32 poke_cnt, BtlTypeAff aff, BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag )
{
  u32 i;

  for(i=0; i<poke_cnt; ++i)
  {
    BPP_HpMinus( bpp[i], damage[i] );
    SCQUE_PUT_OP_HpMinus( wk->que, BPP_GetID(bpp[i]), damage[i] );
  }

  SCQUE_PUT_ACT_WazaDamagePlural( wk->que, poke_cnt, aff, wazaParam->wazaID );
  for(i=0; i<poke_cnt; ++i)
  {
    SCQUE_PUT_ArgOnly( wk->que, BPP_GetID(bpp[i]) );
  }
  for(i=0; i<poke_cnt; ++i)
  {
    if( critical_flag[i] ){
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_CriticalHit, BPP_GetID(bpp[i]) );
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
  u8 pokeID = BPP_GetID( bpp );

  switch( cause ){
  case BPP_KORAE_WAZA_DEFENDER:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Koraeru_Exe, pokeID );
    break;
  case BPP_KORAE_ITEM:
    {
      u16 itemID = BPP_GetItem( bpp );
      if( itemID ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoraeItem, pokeID, itemID );
      }
    }
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] とくせいウィンドウ表示開始
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_IN( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] とくせいウィンドウ消去開始
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_OUT( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] これ以上、能力ランクがあがらない（さがらない）メッセージ
 */
//--------------------------------------------------------------------------
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume )
{
  u8 pokeID = BPP_GetID( target );

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
  u8 pokeID = BPP_GetID( target );

  if( volume > 0 )
  {
    volume = BPP_RankUp( target, effect, volume );
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
    volume = BPP_RankDown( target, effect, volume );
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
  u8 pokeID = BPP_GetID( target );
  BPP_HpZero( target );
  SCQUE_PUT_OP_HpZero( wk->que, pokeID );
  SCQUE_PUT_ACT_WazaIchigeki( wk->que, pokeID );
#ifdef SOGA_DEBUG
  //一撃必殺で倒しても戦闘終了しないので、この処理が必要？
  BTL_POSPOKE_PokeOut( &wk->pospokeWork, BPP_GetID(target) );
#endif
}
//--------------------------------------------------------------------------
/**
 * [Put] シンプルHP増減エフェクト
 */
//--------------------------------------------------------------------------
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value )
{
  u8 pokeID = BPP_GetID( bpp );

  if( value > 0 )
  {
    BPP_HpPlus( bpp, value );
    SCQUE_PUT_OP_HpPlus( wk->que, pokeID, value );
  }
  else if( value < 0 )
  {
    value *= -1;
    BPP_HpMinus( bpp, value );
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
  u16 minusHP = BPP_GetValue( bpp, BPP_HP );
  u8 pokeID = BPP_GetID( bpp );

  BPP_HpMinus( bpp, minusHP );
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
  u8 pokeID = BPP_GetID( target );

  BPP_SetWazaSick( target, sick, sickCont );
  SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );
}
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick )
{
  u8 pokeID = BPP_GetID( target );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示
 */
//--------------------------------------------------------------------------
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID )
{
  u8 pokeID = BPP_GetID( bpp );
  SCQUE_PUT_MSG_SET( wk->que, strID, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示（引数拡張）
 */
//--------------------------------------------------------------------------
static void scPut_Message_SetExPoke( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID, u32 argCnt, const int* args )
{
  u8 pokeID = BPP_GetID( bpp );
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
//--------------------------------------------------------------------------
/**
 * [Put] 標準メッセージ表示（引数拡張）
 */
//--------------------------------------------------------------------------
static void scPut_Message_StdEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_STD( wk->que, strID );
    break;
  case 1:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_STD( wk->que, strID, args[0], args[1], args[2], args[3] );
    break;
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] セットメッセージ表示（引数拡張）
 */
//--------------------------------------------------------------------------
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args )
{
  switch( argCnt ){
  case 0:
  default:
    SCQUE_PUT_MSG_SET( wk->que, strID );
    break;
  case 1:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0] );
    break;
  case 2:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0], args[1] );
    break;
  case 3:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0], args[1], args[2] );
    break;
  case 4:
    SCQUE_PUT_MSG_SET( wk->que, strID, args[0], args[1], args[2], args[3] );
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
  u8 pokeID = BPP_GetID( attacker );
  BPP_PPMinus( attacker, wazaIdx, vol );
  BPP_WAZA_SetUsedFlag( attacker, wazaIdx );
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
  u8 pokeID = BPP_GetID( bpp );

  BPP_PPPlus( bpp, wazaIdx, volume );
  SCQUE_PUT_OP_PPPlus( wk->que, pokeID, wazaIdx, volume );
  if( itemID != ITEM_DUMMY_DATA ){
    WazaID waza = BPP_WAZA_GetID( bpp, wazaIdx );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_RecoverPP, pokeID, itemID, waza );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] きのみを食べるアクション
 */
//----------------------------------------------------------------------------------
static void scPut_EatNutsAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_ACT_KINOMI( wk->que, BPP_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Put] 所持アイテムを削除
 */
//----------------------------------------------------------------------------------
static void scPut_RemoveItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_HANDLER_ITEM_Remove( bpp );
  BPP_RemoveItem( bpp );
  SCQUE_PUT_OP_RemoveItem( wk->que, BPP_GetID(bpp) );
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
  u8 pokeID = BPP_GetID( bpp );
  BPP_ACTFLAG_Set( bpp, flagID );
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
  u8 pokeID = BPP_GetID( bpp );
  BPP_ACTFLAG_Clear( bpp );
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
  BPP_CONTFLAG_Set( bpp, flag );
  SCQUE_PUT_OP_SetContFlag( wk->que, BPP_GetID(bpp), flag );
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
  BPP_CONTFLAG_Clear( bpp, flag );
  SCQUE_PUT_OP_ResetContFlag( wk->que, BPP_GetID(bpp), flag );
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
  BPP_TURNFLAG_Set( bpp, flag );
  SCQUE_PUT_OP_SetTurnFlag( wk->que, BPP_GetID(bpp), flag );
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
  BPP_TURNFLAG_ForceOff( bpp, flag );
  SCQUE_PUT_OP_ResetTurnFlag( wk->que, BPP_GetID(bpp), flag );
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
 * [Event] メンバー入れ替え前に特定ポケモンのアクションを割り込ませるかチェック
 *
 * @param   wk
 * @param   outPoke
 *
 * @retval  u8        割り込む場合、該当ポケモンID／それ以外 BTL_POKEID_NULL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_MemberChangeIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* outPoke )
{
  u8 intrPokeID = BTL_POKEID_NULL;
  u32 i;

  // １度、未処理ポケモンの全ワザハンドラを登録しておく
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( (wk->actOrder[i].bpp != outPoke) && (wk->actOrder[i].fDone == FALSE) )
    {
      WazaID waza = ActOrder_GetWazaID( &wk->actOrder[i] );
      if( waza != WAZANO_NULL ){
        BTL_HANDLER_Waza_Add( wk->actOrder[i].bpp, waza );
        wk->actOrder[i].fIntr = TRUE;
      }
    }
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1, BPP_GetID(outPoke) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, intrPokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MENBERCHANGE_INTR );
    intrPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_ATK );
  BTL_EVENTVAR_Push();

  // 未処理ポケモンの全ワザハンドラを削除
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fIntr)
    {
      WazaID waza = ActOrder_GetWazaID( &wk->actOrder[i] );
      if( waza != WAZANO_NULL ){
        BTL_HANDLER_Waza_RemoveForce( wk->actOrder[i].bpp, waza );
      }
    }
  }

  return intrPokeID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 他のワザを呼び出すワザが、アクション処理順計算を呼び出し後のワザで行う場合、そのパラメータ取得
 *
 * @param   wk
 * @param   bpp
 * @param   orgWazaID   呼び出し元のワザ（ゆびをふる等）
 * @param   reqWaza     [out] 呼び出すワザのパラメータを格納する
 *
 * @retval  BOOL    呼び出すワザのパラメータが設定されていればTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_GetReqWazaForCalcActOrder( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID orgWazaID, REQWAZA_WORK* reqWaza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID,  BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID,  WAZANO_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEPOS, BTL_POS_NULL );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_FOR_ACT_ORDER );
    reqWaza->wazaID = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    reqWaza->targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    if( reqWaza->wazaID != WAZANO_NULL )
    {
      BTL_Printf("他ワザ呼び出し [%d] --> [%d], target=%d\n", orgWazaID, reqWaza->wazaID, reqWaza->targetPos);
    }
  BTL_EVENTVAR_Pop();
  return (reqWaza->wazaID != WAZANO_NULL);
}
//----------------------------------------------------------------------------------
/**
 * [Event] 他のワザを呼び出すワザなら、呼び出すワザのパラメータをセットする
 *
 * @param   wk
 * @param   attacker
 * @param   orgWazaid
 * @param   orgTargetPos
 * @param   reqWaza         [io] 既にアクション処理順序が設定されている
 *
 * @retval  BOOL    他のワザを呼び出すワザ自体を出すのに失敗する場合FALSE／それ以外TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_GetReqWazaParam( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, reqWaza->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEPOS, BTL_POS_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEPOS_ORG, orgTargetPos );
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_PARAM );
    reqWaza->wazaID = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    reqWaza->targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return !failFlag;
}
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
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
  u16 agi = BPP_GetValue( attacker, BPP_AGILITY );
  BTL_Printf("素の素早さを表示する poke=%d, agi=%d\n", BPP_GetID(attacker), agi);
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE ); // まひチェックフラグとして使っている
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1f), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_AGILITY );
    agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    {
      fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      agi = (agi * ratio) >> FX32_SHIFT;
      BTL_Printf("  ハンドラ処理後の素早さ=%d, 倍率=%08x, 結果=%d\n",
          BTL_EVENTVAR_GetValue(BTL_EVAR_AGILITY), ratio, agi);
    }
    if( BPP_GetPokeSick(attacker) == POKESICK_MAHI )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
        agi = (agi * BTL_MAHI_AGILITY_RATIO) / 100;
        BTL_Printf("    さらにマヒで%d\n", agi);
      }
    }
  BTL_EVENTVAR_Pop();

  return agi;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ターン最初のワザ処理シーケンス直前
 *
 * @param   wk
 * @param   bpp
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_BeforeFight( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_BEFORE_FIGHT );
  BTL_EVENTVAR_Pop();
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
  u16 dmg = roundMin( BPP_GetValue(attacker, BPP_MAX_HP)/8, 1 );

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_CONF_DMG, dmg );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_CONF_DAMAGE );
  dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_CONF_DMG );

  BTL_EVENTVAR_Pop();
  return dmg;
}
//----------------------------------------------------------------------------------
/**
 * [Event] 他ワザ呼び出し時など、独自のワザメッセージを出力するか判定
 *
 * @param   wk
 * @param   attacker    ワザを出すポケモン
 * @param   orgWazaID   呼び出し元のワザID（ex. ゆびをふる）
 * @param   actWazaID   呼び出し後のワザID
 * @param   str         [out] 独自ワザメッセージパラメータを格納
 *
 * @retval  BOOL    独自ワザメッセージを出力する場合はTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckWazaMsgCustom( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, WazaID actWazaID, BTL_HANDEX_STR_PARAMS* str )
{
  BOOL result;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, actWazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)str );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_MSG );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return result;
}
//--------------------------------------------------------------------------
/**
 * 【Event】ワザ出し失敗判定
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
  WazaSick sick = BPP_GetPokeSick( attacker );
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;
  u8 pokeID = BPP_GetID( attacker );

  do {

    // ワザロックによる失敗チェック
    if( (BPP_CheckSick(attacker, WAZASICK_ENCORE)
       ||  BPP_CheckSick(attacker, WAZASICK_WAZALOCK) )
    &&  (BPP_GetPrevWazaID(attacker) != waza)
    ){
      cause = SV_WAZAFAIL_WAZALOCK;
      break;
    }
    // ちょうはつによる失敗チェック
    if( BPP_CheckSick(attacker, WAZASICK_TYOUHATSU)
    &&  !WAZADATA_IsDamage(waza)
    ){
      cause = SV_WAZAFAIL_TYOUHATSU;
      break;
    }
    // いちゃもんによる失敗チェック
    if( BPP_CheckSick(attacker, WAZASICK_ICHAMON)
    &&  (BPP_GetPrevWazaID(attacker) == waza)
    ){
      cause = SV_WAZAFAIL_ICHAMON;
      break;
    }
    // ふういんによる失敗チェック
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
    {
      u8 dependPokeID = BTL_FIELD_GetDependPokeID( BTL_FLDEFF_FUIN );
      if( BPP_GetID(attacker) != dependPokeID )
      {
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, dependPokeID );
        if( BPP_WAZA_SearchIdx(bpp, waza) != PTL_WAZA_MAX ){
          cause = SV_WAZAFAIL_FUUIN;
          break;
        }
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
static void scEvent_GetWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, WAZADATA_GetType(waza) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_USER_TYPE, BPP_GetPokeType(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_PARAM );
    param->wazaID = waza;
    param->wazaType = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    param->userType = BTL_EVENTVAR_GetValue( BTL_EVAR_USER_TYPE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザだし失敗チェック
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
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
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_WazaExecuteFix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, TARGET_POKE_REC* rec )
{
  BOOL fQuit = FALSE;
  u32 targetCnt = TargetPokeRec_GetCount( rec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      u32 i;
      const BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i){
        bpp = TargetPokeRec_Get( rec, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, fQuit );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_FIX );
    fQuit = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return fQuit;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザターゲット割り込み
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaTargetIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u8 pokeID = BTL_POKEID_NULL;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECIDE_TARGET );
    pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BTL_EVENTVAR_Pop();
  return pokeID;
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_MAMORU_BREAK );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザが外れた
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetCount
 * @param   targetPokeID
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8 targetCount, const u8* targetPokeID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, targetCount );
    {
      u32 i;
      for(i=0; i<targetCount; ++i){
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, targetPokeID[i] );
      }
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_AVOID );
  BTL_EVENTVAR_Pop();
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
 * [Event] ワザ出しシーケンス終了（当たった外れた＆効果の有無は関係なし）
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExe_Done( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_DONE );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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

      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HIT_RANK, BPP_GetValue(attacker, BPP_HIT_RATIO) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_RANK, BPP_GetValue(defender, BPP_HIT_RATIO) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    for(i=0; i<NELEMS(hideState); ++i)
    {
      if( BPP_CONTFLAG_Get(defender, hideState[i]) )
      {
        BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_FLAG, TRUE );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKE_HIDE, hideState[i] );
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_POKE_HIDE );
        if( BTL_EVENTVAR_GetValue(BTL_EVAR_AVOID_FLAG) ){
          BTL_Printf("ポケ[%d], 状態[%d] なのでワザあたりません\n", BPP_GetID(defender), hideState[i]);
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
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
  u32 per = WAZADATA_GetHitRatio( waza );
  fx32 ratio = FX32_CONST(1);

  BTL_EVENTVAR_Push();
    if( BTL_FIELD_GetWeather() == BTL_WEATHER_MIST )
    {
      per = BTL_CALC_MulRatio( per, BTL_CALC_WEATHER_MIST_HITRATIO );
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_HIT_PER, per );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RATIO );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_PER );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  per = (per * ratio) >> FX32_SHIFT;
  if( per > 100 ){
    per = 100;
  }

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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ICHIGEKI_CHECK );
    if( !BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_FLAG) )
    {
      // とくせい等による強制無効化を受けない場合、必中有効。
      if( IsMustHit(attacker, defender) ){
        ret = TRUE;
      }else{
        u8 per = WAZADATA_GetHitRatio( waza );
        u8 atLevel = BPP_GetValue( attacker, BPP_LEVEL );
        u8 defLevel = BPP_GetValue( defender, BPP_LEVEL );
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
static BOOL scEvent_CheckNotEffect_byType( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  PokeTypePair def_type = BPP_GetPokeType( defender );
  BOOL fNoEffect = FALSE;

  if( (BTL_CALC_TypeAff( wazaParam->wazaType, def_type ) == BTL_TYPEAFF_0)
  &&  (WAZADATA_IsDamage(wazaParam->wazaID) || (WAZADATA_GetCategory(wazaParam->wazaID) == WAZADATA_CATEGORY_ICHIGEKI))
  ){
    fNoEffect = TRUE;
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_NOEFFECT_FLAG, fNoEffect );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NOEFFECT_TYPE_CHECK );
    BTL_SICK_CheckNotEffectByType( wk, defender );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
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
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam )
{
  BOOL ret = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
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
  u16  rank = WAZADATA_GetCriticalRank( waza );
  rank += BPP_GetCriticalRank( attacker );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
 * [Event] ワザによるダメージを与えた後の反動計算
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   damage
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage )
{
  u8 pokeID = BPP_GetID( attacker );
  u8 ratio = WAZADATA_GetReactionRatio( waza );
  u8 ratio_ex = 0;
  u8 fail_flag = FALSE;

#ifdef SOGA_DEBUG
  //ドレイン系は反動にならないようにする
  if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_DRAIN )
  {
    return 0;
  }
#endif

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
  if( BPP_GetValue(defender, BPP_HP) > (*damage) )
  {
    return BPP_KORAE_NONE;
  }
  else
  {
    BppKoraeruCause  cause = BPP_KORAE_NONE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_KORAERU_CAUSE, cause );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_KORAERU_CHECK );
      cause = BTL_EVENTVAR_GetValue( BTL_EVAR_KORAERU_CAUSE );
    BTL_EVENTVAR_Pop();

    if( cause != BPP_KORAE_NONE ){
      *damage = BPP_GetValue(defender, BPP_HP) - 1;
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
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, BOOL criticalFlag )
{
  u8 defPokeID = BPP_GetID( defender );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defPokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TYPEAFF, aff );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event]アイテム反応チェック
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckItemReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_ITEM_REACTION );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 装備アイテム使用
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_USE_ITEM );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] 一時的に登録したアイテムの使用（ついばむなど）
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemEquipTmp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_USE_ITEM_TMP );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザの相性計算
 *
 * @param   wk
 * @param   defender
 * @param   waza_type
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff scEvent_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* defender, PokeType waza_type )
{
  PokeTypePair defPokeType = scEvent_getDefenderPokeType( wk, defender );
  BtlTypeAff affinity = BTL_CALC_TypeAff( waza_type, defPokeType );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, waza_type );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TYPEAFF, affinity );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_AFFINITY );
    BTL_SICK_CheckDamageAffinity( wk, defender );
    affinity = BTL_EVENTVAR_GetValue( BTL_EVAR_TYPEAFF );
  BTL_EVENTVAR_Pop();

  return affinity;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ使用後のデクリメントするPP値を取得
 *
 * @param   wk
 * @param   attacker  ワザ使用ポケモン
 * @param   rec       ワザによってダメージを受けたポケモンパラメータ群
 *
 * @retval  u32       デクリメントPP値
 */
//----------------------------------------------------------------------------------
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* rec )
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
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID,   BPP_GetID(attacker) );
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
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
 * @param   attackerBTL_HANDEX_PARAM_SET_ITEM
 * @param   defender
 * @param   waza
 * @param   wazaParam
 *
 * @retval  u16   ワザ威力値
 */
//----------------------------------------------------------------------------------
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u16 power = WAZADATA_GetPower( wazaParam->wazaID );
  fx32 ratio;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SIMPLE_DAMAGE_ENABLE );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
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
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );    //
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER_PREV );
      if( BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG ) )
      {
        BPP_GetValue_Base( attacker, vid );
      }
      else
      {
        if( criticalFlag ){
          power = BPP_GetValue_Critical( attacker, vid );
        }else{
          power = BPP_GetValue( attacker, vid );
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
  const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag )
{
  BppValueID vid;

  switch( WAZADATA_GetDamageType(wazaParam->wazaID) ){
  case WAZADATA_DMG_PHYSIC:   vid = BPP_DEFENCE; break;
  case WAZADATA_DMG_SPECIAL:  vid = BPP_SP_DEFENCE; break;
  default:
    GF_ASSERT(0);
    vid = BPP_DEFENCE;
    break;
  }

  {
    u16 guard;
    fx32 ratio = FX32_CONST(1);

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD_PREV );

      if( BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG) )
      {
        guard = BPP_GetValue_Base( defender, vid );
      }
      else
      {
        if( criticalFlag ){
          guard = BPP_GetValue_Critical( defender, vid );
        }else{
          guard = BPP_GetValue( defender, vid );
        }
      }

      // てんこう「すなあらし」の時、いわタイプのとくぼう1.5倍
      if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SAND)
      &&  (BPP_IsMatchType(defender, POKETYPE_IWA))
      &&  (vid == BPP_SP_DEFENCE)
      ){
        guard = (guard * 192) / 128;
      }

      BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
      BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, ratio, FX32_CONST(0.1), FX32_CONST(32) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD );
      guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
      ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    BTL_EVENTVAR_Pop();

    guard = (guard * ratio) >> FX32_SHIFT;
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
  BOOL f_match = BPP_IsMatchType( attacker, waza_type );
  fx32 ratio = (f_match)? FX32_CONST(1.5f) : FX32_CONST(1);

  BTL_EVENTVAR_Push();
  {
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
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

  BTL_EVENTVAR_SetValue( BTL_EVAR_POKE_TYPE, BPP_GetPokeType(defender) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(defender) );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_TYPE );

  {
    PokeTypePair type = BTL_EVENTVAR_GetValue( BTL_EVAR_POKE_TYPE );
    BTL_EVENTVAR_Pop();
    return type;
  }
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_IN );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ワザ追加効果によるひるみ確率取得
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_GetWazaShrinkPer( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker )
{
  u32 per = WAZADATA_GetShrinkPer( waza );
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_SHRINK_PER );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  BTL_EVENTVAR_Pop();
  return per;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ひるみ発生判定
 *
 * @param   wk
 * @param   target
 * @param   waza    ワザ追加効果による場合ワザID（それ以外WAZANO_NULL）
 * @param   per     ひるみ確率
 *
 * @retval  BOOL    ひるむならTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* target, WazaID waza, u32 per )
{
  BOOL fail_flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fail_flag );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
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
//----------------------------------------------------------------------------------
/**
 * [Event] ひるみ失敗原因表示
 *
 * @param   wk
 * @param   target
 */
//----------------------------------------------------------------------------------
static void scEvent_FailShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_FAIL );
  BTL_EVENTVAR_Pop();
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
 * [Event] ワザによる状態異常の継続パラメータ調整
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   sick
 * @param   sickCont    [io]調整前パラメータ／調整後の結果も格納
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaSickCont( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT* sickCont )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont->raw );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_PARAM );
    sickCont->raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    {
      u32 i, cnt = TargetPokeRec_GetCount( targets );
      const BTL_POKEPARAM* bpp;
      BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_POKECNT, cnt );
      for(i=0; i<cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_END );
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
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  u8 per = WAZADATA_GetRankEffPer( wazaParam->wazaID );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(target) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, rankType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_FIXED );
  BTL_EVENTVAR_Pop();
}

//--------------------------------------------------------------------------
/**
 * [Event] ドレイン攻撃のドレインHP量を再計算
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   volume     デフォルト回復量
 *
 * @retval  int     回復量
 */
//--------------------------------------------------------------------------
static int scEvent_RecalcDrainVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u32 volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
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
//----------------------------------------------------------------------------------
/**
 * 【Event】天候ダメージ計算
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
static int scEvent_CalcWeatherDamage( BTL_SVFLOW_WORK* wk, BtlWeather weather, BTL_POKEPARAM* bpp, u8 *tok_cause_flg )
{
  int default_damage = BTL_CALC_RecvWeatherDamage( bpp, weather );
  int damage;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE,  default_damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_WEATHER_DAMAGE );
    damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    *tok_cause_flg = ( default_damage != damage );
  BTL_EVENTVAR_Pop();
  return damage;
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RECOVER_HP_RATIO );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  {
    u32 volume = (BPP_GetValue(bpp, BPP_MAX_HP) * ratio) / 100;
    return volume;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] アイテムを強制的にセット（あるいは消去）される
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    失敗する場合TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEventSetItem( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL failed = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failed );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SET_ITEM_BEFORE );
    failed = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return failed;
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
  return BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );
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
    dst[ cnt++ ] = BPP_GetID( bpp );
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
    if( !BTL_MAINUTIL_IsFriendPokeID( basePokeID, BPP_GetID(bpp) ) )
    {
      dst[ cnt++ ] = BPP_GetID( bpp );
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
    if( BPP_GetValue(bpp, BPP_TOKUSEI) == tokusei )
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
    if( !BPP_IsDead(bpp) ){
      dst_pokeID[num++] = BPP_GetID( bpp );
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
    if( !BPP_IsDead(pp) )
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
    if( !BPP_IsDead(pp) )
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
      BPP_HpPlus( bpp, value );
      SCQUE_PUT_OP_HpPlus( wk->que, pokeID, value );
    }
    else
    {
      value *= -1;
      BPP_HpMinus( bpp, value );
      SCQUE_PUT_OP_HpMinus( wk->que, pokeID, value );
    }
    SCQUE_PUT_ACT_SimpleHP( wk->que, pokeID );
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
  if( !BPP_IsDead(bpp) )
  {
    BPP_CurePokeSick( bpp );
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
  if( !BPP_IsDead(bpp) )
  {
    BPP_CureWazaSick( bpp, sick );
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
 *
 *
 * @param   wk
 *
 * @retval  BtlCompetitor
 */
//=============================================================================================
BtlCompetitor BTL_SVFLOW_GetCompetitor( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetCompetitor( wk->mainModule );
}
//=============================================================================================
/**
 * ゲーム内戦闘であり、かつプレイヤー側のポケモンであるかどうか判定
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL    そうだったらTRUE
 */
//=============================================================================================
BOOL BTL_SVFLOW_IsStandAlonePlayerSide( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  if( (BTL_MAIN_GetCommMode(wk->mainModule) == BTL_COMM_NONE)
  &&  (BTL_MAINUTIL_PokeIDtoClientID(pokeID) == BTL_STANDALONE_PLAYER_CLIENT_ID)
  ){
    return TRUE;
  }
  return FALSE;
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
  PokeTokusei tok = BPP_GetValue( bppTgt, BPP_TOKUSEI );

  BTL_Printf("トレースで書き換えるとくせい=%d\n", tok);
  BPP_ChangeTokusei( bpp, tok );
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

//=============================================================================================
/**
 * ダメージ計算シミュレート結果を返す
 *
 * @param   flowWk
 * @param   atkPokeID       攻撃ポケID
 * @param   defPokeID       防御ポケID
 * @param   waza            ワザ
 * @param   fAffinity       相性計算するか（FALSEなら等倍で計算）
 * @param   fCriticalCheck  クリティカル判定するか
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_SVFLOW_SimulationDamage( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza, BOOL fAffinity, BOOL fCriticalCheck )
{
  const BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, atkPokeID );
  const BTL_POKEPARAM* defender = BTL_POKECON_GetPokeParam( wk->pokeCon, defPokeID );

  BtlTypeAff  aff;
  BOOL critical_flag;
  SVFL_WAZAPARAM wazaParam;

  //
  if( fAffinity ){
    aff = BTL_CALC_TypeAff( WAZADATA_GetType(waza), BPP_GetPokeType(defender) );
  }else{
    aff = BTL_TYPEAFF_100;
  }

  if( fCriticalCheck ){
    critical_flag = scEvent_CheckCritical( wk, attacker, defender, waza );
  }else{
    critical_flag = FALSE;
  }

  scEvent_GetWazaParam( wk, waza, attacker, &wazaParam );

  return scEvent_CalcDamage( wk, attacker, defender, &wazaParam, aff, BTL_CALC_DMG_TARGET_RATIO_NONE, critical_flag );
}
//=============================================================================================
/**
 * 戦闘地形タイプを取得
 *
 * @param   wk
 *
 * @retval  BtlLandForm
 */
//=============================================================================================
BtlLandForm BTL_SVFLOW_GetLandForm( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetLandForm( wk->mainModule );
}
//=============================================================================================
/**
 * 自クライアントが担当している戦闘位置の数を返す
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_SVFLOW_GetClientCoverPosCount( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  SVCL_WORK* clwk;
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  return clwk->numCoverPos;
}
//=============================================================================================
/**
 * 指定ポケモンを含むパーティデータを取得
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_PARTY*
 */
//=============================================================================================
const BTL_PARTY* BTL_SVFLOW_GetPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
  return BTL_POKECON_GetPartyDataConst( wk->pokeCon, clientID );
}
//=============================================================================================
/**
 * 指定ポケモンの戦闘位置を返す
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_SVFLOW_PokeIDtoPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
}
//=============================================================================================
/**
 * 指定ポケモンが「ゆびをふる」など他のワザを呼び出した際、ターゲットを自動で決める
 * ※ターゲット選択の必要が無いワザの場合、BTL_POS_NULL が返る
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 *
 * @retval  BtlPokePos
 */
//=============================================================================================
BtlPokePos BTL_SVFLOW_ReqWazaTargetAuto( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
  BtlPokePos targetPos = BTL_POS_NULL;
  BtlPokePos myPos = BTL_SVFLOW_PokeIDtoPokePos( wk, pokeID );
  WazaTarget  targetType = WAZADATA_GetTarget( waza );

  // シングル
  if( rule == BTL_RULE_SINGLE )
  {
    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:  ///< 自分以外の１体（選択）
    case WAZA_TARGET_ENEMY_SELECT:  ///< 敵１体（選択）
    case WAZA_TARGET_ENEMY_RANDOM:  ///< 敵ランダム
    case WAZA_TARGET_ENEMY_ALL:     ///< 敵側２体
    case WAZA_TARGET_OTHER_ALL:     ///< 自分以外全部
      targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, myPos, 0 );
      break;

    case WAZA_TARGET_USER:                ///< 自分１体のみ
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体
      targetPos = myPos;
      break;

    case WAZA_TARGET_ALL:
    case WAZA_TARGET_UNKNOWN:
    default:
      break;
    }
  }
  // ダブル
  else
  {
    u8 rnd;

    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    case WAZA_TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
    case WAZA_TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
      rnd = GFL_STD_MtRand(2);
      targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, myPos, rnd );
      break;

    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
      rnd = GFL_STD_MtRand(2);
      if( rnd ){
        targetPos = myPos;
      }else{
        targetPos = BTL_MAIN_GetNextPokePos( wk->mainModule, myPos );
      }
      break;

    case WAZA_TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
      targetPos = BTL_MAIN_GetNextPokePos( wk->mainModule, myPos );
      break;

    case WAZA_TARGET_USER:      ///< 自分１体のみ
      targetPos = myPos;
      break;

    case WAZA_TARGET_ENEMY_ALL:           ///< 敵側全体
    case WAZA_TARGET_OTHER_ALL:           ///< 自分以外全部
    case WAZA_TARGET_ALL:                 ///< 全部
    case WAZA_TARGET_UNKNOWN:
    default:
      break;
    }
  }
  return targetPos;
}
//=============================================================================================
/**
 * 指定位置のポケモンIDを返す
 *
 * @param   wk
 * @param   pokePos
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTL_SVFLOW_PokePosToPokeID( BTL_SVFLOW_WORK* wk, u8 pokePos )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, pokePos );
  return BPP_GetID( bpp );
}
//=============================================================================================
/**
 * 現ターンに選択されたアクション内容を取得
 *
 * @param   wk
 * @param   pokeID
 * @param   dst       [out]
 *
 * @retval  BOOL    正しく取得できたらTRUE（現ターン、参加していないポケなどが指定されたらFALSE）
 */
//=============================================================================================
BOOL BTL_SVFLOW_GetThisTurnAction( BTL_SVFLOW_WORK* wk, u8 pokeID, BTL_ACTION_PARAM* dst )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      *dst = wk->actOrder[i].action;
      return TRUE;
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * 直前に出されたワザを取得
 *
 * @param   wk
 *
 * @retval  WazaID
 */
//=============================================================================================
WazaID BTL_SVFLOW_GetPrevExeWaza( BTL_SVFLOW_WORK* wk )
{
  return wk->prevExeWaza;
}
//=============================================================================================
/**
 * 全ハンドラ共有のテンポラリワーク先頭アドレスを返す
 *
 * @param   wk
 *
 * @retval  void*
 */
//=============================================================================================
void* BTL_SVFLOW_GetHandlerTmpWork( BTL_SVFLOW_WORK* wk )
{
  return wk->handlerTmpWork;
}


//=============================================================================================
/**
 *
 *
 * @param   wk
 * @param   volume
 */
//=============================================================================================
void BTL_SVFLOW_AddBonusMoney( BTL_SVFLOW_WORK* wk, u32 volume )
{
  BTL_SERVER_AddBonusMoney( wk->server, volume );
}

//--------------------------------------------------------------------------------------------------------
// HandEx
//--------------------------------------------------------------------------------------------------------

static void Hem_Init( HANDLER_EXHIBISION_MANAGER* wk )
{
  wk->stack_ptr = 0;
  wk->read_ptr = 0;
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
static u16 Hem_GetStackPtr( const HANDLER_EXHIBISION_MANAGER* wk )
{
  return wk->stack_ptr;
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
    { BTL_HANDEX_USE_ITEM,        sizeof(BTL_HANDEX_PARAM_USE_ITEM)        },
    { BTL_HANDEX_RECOVER_HP,      sizeof(BTL_HANDEX_PARAM_RECOVER_HP)      },
    { BTL_HANDEX_DRAIN,           sizeof(BTL_HANDEX_PARAM_DRAIN)           },
    { BTL_HANDEX_SHIFT_HP,        sizeof(BTL_HANDEX_PARAM_SHIFT_HP)        },
    { BTL_HANDEX_RECOVER_PP,      sizeof(BTL_HANDEX_PARAM_PP)              },
    { BTL_HANDEX_DECREMENT_PP,    sizeof(BTL_HANDEX_PARAM_PP)              },
    { BTL_HANDEX_CURE_SICK,       sizeof(BTL_HANDEX_PARAM_CURE_SICK)       },
    { BTL_HANDEX_ADD_SICK,        sizeof(BTL_HANDEX_PARAM_ADD_SICK)        },
    { BTL_HANDEX_RANK_EFFECT,     sizeof(BTL_HANDEX_PARAM_RANK_EFFECT)     },
    { BTL_HANDEX_SET_RANK,        sizeof(BTL_HANDEX_PARAM_SET_RANK)        },
    { BTL_HANDEX_RECOVER_RANK,    sizeof(BTL_HANDEX_PARAM_RECOVER_RANK)    },
    { BTL_HANDEX_RESET_RANK,      sizeof(BTL_HANDEX_PARAM_RESET_RANK)      },
    { BTL_HANDEX_DAMAGE,          sizeof(BTL_HANDEX_PARAM_DAMAGE)          },
    { BTL_HANDEX_KILL,            sizeof(BTL_HANDEX_PARAM_KILL)            },
    { BTL_HANDEX_CHANGE_TYPE,     sizeof(BTL_HANDEX_PARAM_CHANGE_TYPE)     },
    { BTL_HANDEX_MESSAGE,         sizeof(BTL_HANDEX_PARAM_MESSAGE)         },
    { BTL_HANDEX_TOKWIN_IN,       sizeof(BTL_HANDEX_PARAM_HEADER)          },
    { BTL_HANDEX_TOKWIN_OUT,      sizeof(BTL_HANDEX_PARAM_HEADER)          },
    { BTL_HANDEX_SET_TURNFLAG,    sizeof(BTL_HANDEX_PARAM_TURNFLAG)        },
    { BTL_HANDEX_RESET_TURNFLAG,  sizeof(BTL_HANDEX_PARAM_TURNFLAG)        },
    { BTL_HANDEX_SET_CONTFLAG,    sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)    },
    { BTL_HANDEX_RESET_CONTFLAG,  sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG)    },
    { BTL_HANDEX_SIDEEFF_REMOVE,  sizeof(BTL_HANDEX_PARAM_SIDEEFF_REMOVE)  },
    { BTL_HANDEX_ADD_FLDEFF,      sizeof(BTL_HANDEX_PARAM_ADD_FLDEFF)      },
    { BTL_HANDEX_REMOVE_FLDEFF,   sizeof(BTL_HANDEX_PARAM_REMOVE_FLDEFF)   },
    { BTL_HANDEX_POSEFF_ADD,      sizeof(BTL_HANDEX_PARAM_POSEFF_ADD)      },
    { BTL_HANDEX_CHANGE_TOKUSEI,  sizeof(BTL_HANDEX_PARAM_CHANGE_TOKUSEI)  },
    { BTL_HANDEX_SET_ITEM,        sizeof(BTL_HANDEX_PARAM_SET_ITEM)        },
    { BTL_HANDEX_EQUIP_ITEM,      sizeof(BTL_HANDEX_PARAM_EQUIP_ITEM)      },
    { BTL_HANDEX_CONSUME_ITEM,    sizeof(BTL_HANDEX_PARAM_HEADER)          },
    { BTL_HANDEX_SWAP_ITEM,       sizeof(BTL_HANDEX_PARAM_SWAP_ITEM)       },
    { BTL_HANDEX_UPDATE_WAZA,     sizeof(BTL_HANDEX_PARAM_UPDATE_WAZA)     },
    { BTL_HANDEX_COUNTER,         sizeof(BTL_HANDEX_PARAM_COUNTER)         },
    { BTL_HANDEX_DELAY_WAZADMG,   sizeof(BTL_HANDEX_PARAM_DELAY_WAZADMG)   },
    { BTL_HANDEX_QUIT_BATTLE,     sizeof(BTL_HANDEX_PARAM_HEADER)          },
    { BTL_HANDEX_CHANGE_MEMBER,   sizeof(BTL_HANDEX_PARAM_CHANGE_MEMBER)   },
    { BTL_HANDEX_BATONTOUCH,      sizeof(BTL_HANDEX_PARAM_BATONTOUCH)      },
    { BTL_HANDEX_ADD_SHRINK,      sizeof(BTL_HANDEX_PARAM_ADD_SHRINK)      },
    { BTL_HANDEX_RELIVE,          sizeof(BTL_HANDEX_PARAM_RELIVE)          },
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
//---------------------------------------------------------------------------------------------
// 当ターン生き返りポケレコード
//---------------------------------------------------------------------------------------------

// 初期化
static void relivePokeRec_Init( BTL_SVFLOW_WORK* wk )
{
  wk->numRelivePoke = 0;
}

// 生き返り記録追加
static void relivePokeRec_Add( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<wk->numRelivePoke; ++i)
  {
    if( wk->relivedPokeID[i] == pokeID ){
      return;
    }
  }

  if( i < NELEMS(wk->relivedPokeID) ){
    wk->relivedPokeID[i] = pokeID;
  }
}

// ターン終了時、ポケモン入場の必要があるか判定
static BOOL relivePokeRec_CheckNecessaryPokeIn( BTL_SVFLOW_WORK* wk )
{
  u32 i;
  u8 pos[ BTL_POSIDX_MAX ];
  u8 clientID;
  for(i=0; i<wk->numRelivePoke; ++i)
  {
    clientID = BTL_MAINUTIL_PokeIDtoClientID( wk->relivedPokeID[i] );
    // １個所でも空き位置があれば入場させる必要アリ
    if( BTL_POSPOKE_GetClientEmptyPos(&wk->pospokeWork, clientID, pos) ){
      return TRUE;
    }
  }
  return FALSE;
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
static BOOL scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID )
{
  BTL_HANDEX_PARAM_HEADER* handEx_header;
  u8 fSucceed, fPrevSucceed;

  fSucceed = FALSE;
  fPrevSucceed = TRUE;

  while( (handEx_header = Hem_ReadWork(&wk->HEManager)) != NULL )
  {
    BTL_Printf("ProcHandEX : ADRS=0x%p, type=%d\n", handEx_header, handEx_header->equip);
    if( handEx_header->failSkipFlag && (fPrevSucceed == FALSE) ){
      continue;
    }
    switch( handEx_header->equip ){
    case BTL_HANDEX_USE_ITEM:       fPrevSucceed = scproc_HandEx_useItem( wk, handEx_header ); break;
    case BTL_HANDEX_TOKWIN_IN:      fPrevSucceed = scproc_HandEx_TokWinIn( wk, handEx_header ); break;
    case BTL_HANDEX_TOKWIN_OUT:     fPrevSucceed = scproc_HandEx_TokWinOut( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_HP:     fPrevSucceed = scproc_HandEx_recoverHP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DRAIN:          fPrevSucceed = scproc_HandEx_drain( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DAMAGE:         fPrevSucceed = scproc_HandEx_damage( wk, handEx_header ); break;
    case BTL_HANDEX_SHIFT_HP:       fPrevSucceed = scproc_HandEx_shiftHP( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_PP:     fPrevSucceed = scproc_HandEx_recoverPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DECREMENT_PP:   fPrevSucceed = scproc_HandEx_decrementPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_CURE_SICK:      fPrevSucceed = scproc_HandEx_cureSick( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_ADD_SICK:       fPrevSucceed = scproc_HandEx_addSick( wk, handEx_header ); break;
    case BTL_HANDEX_RANK_EFFECT:    fPrevSucceed = scproc_HandEx_rankEffect( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_SET_RANK:       fPrevSucceed = scproc_HandEx_setRank( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_RANK:   fPrevSucceed = scproc_HandEx_recoverRank( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_RANK:     fPrevSucceed = scproc_HandEx_resetRank( wk, handEx_header ); break;
    case BTL_HANDEX_KILL:           fPrevSucceed = scproc_HandEx_kill( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_TYPE:    fPrevSucceed = scproc_HandEx_changeType( wk, handEx_header ); break;
    case BTL_HANDEX_MESSAGE:        fPrevSucceed = scproc_HandEx_message( wk, handEx_header ); break;
    case BTL_HANDEX_SET_TURNFLAG:   fPrevSucceed = scproc_HandEx_setTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_TURNFLAG: fPrevSucceed = scproc_HandEx_resetTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SET_CONTFLAG:   fPrevSucceed = scproc_HandEx_setContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_CONTFLAG: fPrevSucceed = scproc_HandEx_resetContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SIDEEFF_REMOVE: fPrevSucceed = scproc_HandEx_sideEffectRemove( wk, handEx_header ); break;
    case BTL_HANDEX_ADD_FLDEFF:     fPrevSucceed = scproc_HandEx_addFieldEffect( wk, handEx_header ); break;
    case BTL_HANDEX_REMOVE_FLDEFF:  fPrevSucceed = scproc_HandEx_removeFieldEffect( wk, handEx_header ); break;
    case BTL_HANDEX_POSEFF_ADD:     fPrevSucceed = scproc_HandEx_PosEffAdd( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_TOKUSEI: fPrevSucceed = scproc_HandEx_tokuseiChange( wk, handEx_header ); break;
    case BTL_HANDEX_SET_ITEM:       fPrevSucceed = scproc_HandEx_setItem( wk, handEx_header ); break;
    case BTL_HANDEX_EQUIP_ITEM:     fPrevSucceed = scproc_HandEx_equipItem( wk, handEx_header ); break;
    case BTL_HANDEX_CONSUME_ITEM:   fPrevSucceed = scproc_HandEx_consumeItem( wk, handEx_header ); break;
    case BTL_HANDEX_SWAP_ITEM:      fPrevSucceed = scproc_HandEx_swapItem( wk, handEx_header ); break;
    case BTL_HANDEX_UPDATE_WAZA:    fPrevSucceed = scproc_HandEx_updateWaza( wk, handEx_header ); break;
    case BTL_HANDEX_COUNTER:        fPrevSucceed = scproc_HandEx_counter( wk, handEx_header ); break;
    case BTL_HANDEX_DELAY_WAZADMG:  fPrevSucceed = scproc_HandEx_delayWazaDamage( wk, handEx_header ); break;
    case BTL_HANDEX_QUIT_BATTLE:    fPrevSucceed = scproc_HandEx_quitBattle( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_MEMBER:  fPrevSucceed = scproc_HandEx_changeMember( wk, handEx_header ); break;
    case BTL_HANDEX_BATONTOUCH:     fPrevSucceed = scproc_HandEx_batonTouch( wk, handEx_header ); break;
    case BTL_HANDEX_ADD_SHRINK:     fPrevSucceed = scproc_HandEx_addShrink( wk, handEx_header ); break;
    case BTL_HANDEX_RELIVE:         fPrevSucceed = scproc_HandEx_relive( wk, handEx_header ); break;
    default:
      GF_ASSERT_MSG(0, "illegal handEx type = %d, userPokeID=%d", handEx_header->equip, handEx_header->userPokeID);
    }
    if( fPrevSucceed ){
      fSucceed = TRUE;
    }
  }
  return fSucceed;
}
/**
 * とくせいウィンドウ表示
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_TokWin_In( wk, pp_user );
  return 1;
}
/**
 * とくせいウィンドウ消去
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_TokWin_Out( wk, pp_user );
  return 1;
}
/**
 * 装備アイテム使用
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  if( !BPP_IsDead(pp_user) )
  {
    if( scproc_UseItemEquip(wk, pp_user) ){
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモンHP回復
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RECOVER_HP* param = (BTL_HANDEX_PARAM_RECOVER_HP*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  u8 result = 0;

  if( !BPP_IsDead(pp_target)
  &&  !BPP_IsHPFull(pp_target)
  ){
    if( param_header->tokwin_flag ){
      scPut_TokWin_In( wk, pp_user );
    }

    if( scproc_RecoverHP(wk, pp_target, param->recoverHP) ){
      if( param->exStr.type != BTL_STRTYPE_NULL ){
        handexSub_putString( wk, &(param->exStr) );
      }
      result = 1;
    }

    if( param_header->tokwin_flag ){
      scPut_TokWin_Out( wk, pp_user );
    }
  }
  return result;
}
/**
 * ポケモンHP回復（ドレイン系）
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_drain( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_DRAIN* param = (BTL_HANDEX_PARAM_DRAIN*)param_header;
  BTL_POKEPARAM* pp_recover = BTL_POKECON_GetPokeParam( wk->pokeCon, param->recoverPokeID );
  BTL_POKEPARAM* pp_damaged = BTL_POKECON_GetPokeParam( wk->pokeCon, param->damagedPokeID );

  if( !BPP_IsDead(pp_recover)
  &&  !BPP_IsHPFull(pp_recover)
  ){
    if( scproc_DrainCore(wk, pp_recover, pp_damaged, param->recoverHP) ){
      if( param->exStr.type != BTL_STRTYPE_NULL ){
        handexSub_putString( wk, &(param->exStr) );
      }
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモンにダメージ
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DAMAGE* param = (BTL_HANDEX_PARAM_DAMAGE*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) )
    {
      if( scproc_SimpleDamage(wk, pp_target, param->damage[i], &param->exStr) ){
        result = 1;
      }
    }
  }

  return result;
}
/**
 * ポケモンHP増減（回復／ダメージと見なさない）
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SHIFT_HP* param = (BTL_HANDEX_PARAM_SHIFT_HP*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) ){
      scPut_SimpleHp( wk, pp_target, param->volume[i] );
      scproc_CheckItemReaction( wk, pp_target );
      result = 1;
    }
  }
  return result;
}
/**
 * ポケモンPP回復
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( !BPP_IsDead(pp_target) )
  {
    if( !BPP_IsPPFull(pp_target, param->wazaIdx) ){
      scPut_RecoverPP( wk, pp_target, param->wazaIdx, param->volume, itemID );
      handexSub_putString( wk, &param->exStr );
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモンPP減少
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;

  BTL_POKEPARAM* targetPokemon = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_decrementPP(wk, targetPokemon, param->wazaIdx, param->volume) ){
    return 1;
  }
  return 0;
}
/**
 * ポケモン状態異常回復
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_CURE_SICK* param = (BTL_HANDEX_PARAM_CURE_SICK*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  u8 result = 0;

  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  {
    BTL_POKEPARAM* pp_target;
    WazaSick cured_sick;
    u32 i;
    for(i=0; i<param->poke_cnt; ++i){
      pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
      if( !BPP_IsDead(pp_target) )
      {
        BPP_SICK_CONT oldCont;
        cured_sick = scPut_CureSick( wk, pp_target, param->sickCode, &oldCont );
        if( cured_sick != WAZASICK_NULL )
        {
          if( !param->fStdMsgDisable )
          {
            BTL_HANDEX_STR_PARAMS  str;
            if( BTL_SICK_MakeDefaultCureMsg(cured_sick, oldCont, pp_target, itemID, &str) ){
              handexSub_putString( wk, &str );
            }
          }
          else if( param->fExMsg ){
            scPut_Message_Set( wk, pp_target, param->exStrID );
          }
          result = 1;
        }
      }
    }
  }

  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }

  return result;
}
/**
 * ポケモン状態異常化
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = (BTL_HANDEX_PARAM_ADD_SICK*)param_header;
  BTL_POKEPARAM* pp_user = (param_header->userPokeID != BTL_POKEID_NULL)?
                      BTL_POKECON_GetPokeParam(wk->pokeCon, param_header->userPokeID) : NULL;

  BTL_POKEPARAM* pp_target;
  MSG_CALLBACK_PARAM  cbParam;
  u32 i;
  u8 result = 0;

  cbParam.func = handex_addsick_msg;
  cbParam.arg = (void*)param;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) )
    {
      if( scproc_AddSick( wk, pp_target, pp_user, param->sickID, param->sickCont, param->fAlmost, &cbParam ) ){
        result = 1;
      }
    }
  }
  return result;
}
//----------------------------------------------------------------------------------
// 状態異常成功時のメッセージ表示コールバック
//----------------------------------------------------------------------------------
static void handex_addsick_msg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, void* arg )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = arg;

  if( param->exStr.type != BTL_STRTYPE_NULL ){
    handexSub_putString( wk, &param->exStr );
  }else{
    BTL_HANDEX_STR_PARAMS  str;
    BTL_SICK_MakeDefaultMsg( param->sickID, param->sickCont, target, &str );
    handexSub_putString( wk, &param->exStr );
  }
}
/**
 * ポケモンランク効果（段階数を指定し、上昇または下降させる。相手のとくせい等により失敗することがあり得る）
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RANK_EFFECT* param = (BTL_HANDEX_PARAM_RANK_EFFECT*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) )
    {
      if( scproc_RankEffectCore(wk, pp_target, param->rankType, param->rankVolume,
        itemID, param->fAlmost, !(param->fStdMsgDisable))
      ){
        result = 1;
      }
    }
  }
  return result;
}
/**
 * ポケモンランク強制セット（指定値に強制的に書き換える。失敗しない）
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_setRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_RANK* param = (BTL_HANDEX_PARAM_SET_RANK*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) )
  {
    BPP_RankSet( pp_target, BPP_ATTACK,     param->attack );
    BPP_RankSet( pp_target, BPP_DEFENCE,    param->defence );
    BPP_RankSet( pp_target, BPP_SP_ATTACK,  param->sp_attack );
    BPP_RankSet( pp_target, BPP_SP_DEFENCE, param->sp_defence );
    BPP_RankSet( pp_target, BPP_AGILITY,    param->agility );
    SCQUE_PUT_OP_RankSet5( wk->que,
      param->pokeID, param->attack, param->defence, param->sp_attack, param->sp_defence, param->agility );
    return 1;
  }
  return 0;
}
/**
 * ポケモンランクの内、マイナスになっているもののみフラットに戻す
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RECOVER_RANK* param = (BTL_HANDEX_PARAM_RECOVER_RANK*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) )
  {
    BPP_RankRecover( pp_target );
    return 1;
  }
  return 0;
}
/**
 * ポケモンランクを全てフラットに戻す
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RESET_RANK* param = (BTL_HANDEX_PARAM_RESET_RANK*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) ){
      BPP_RankReset( pp_target );
      result = 1;
    }
  }
  return result;
}
/**
 * ポケモンランクを全てフラットに戻す
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_KILL* param = (BTL_HANDEX_PARAM_KILL*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) ){
    scproc_KillPokemon( wk, pp_target );
    return 1;
  }
  return 0;
}
/**
 * ポケモンタイプ変更
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TYPE* param= (const BTL_HANDEX_PARAM_CHANGE_TYPE*)param_header;

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    PokeType type_pure = PokeTypePair_GetType1( param->next_type );

    SCQUE_PUT_OP_ChangePokeType( wk->que, param->pokeID, param->next_type );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChangePokeType, param->pokeID, type_pure );
    BPP_ChangePokeType( bpp, param->next_type );

    return 1;
  }
  return 0;
}
/**
 * SETメッセージ表示
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_message( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_MESSAGE* param = (const BTL_HANDEX_PARAM_MESSAGE*)(param_header);
  handexSub_putString( wk, &param->str );
  return 1;
}
/**
 * ポケモンターンフラグセット
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    BPP_TURNFLAG_Set( bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモンターンフラグリセット
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TURNFLAG* param = (const BTL_HANDEX_PARAM_TURNFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    BPP_TURNFLAG_ForceOff( bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモン継続フラグセット
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    scPut_SetContFlag( wk, bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * ポケモン継続フラグリセット
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_CONTFLAG* param = (const BTL_HANDEX_PARAM_SET_CONTFLAG*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( !BPP_IsDead(bpp) ){
    scPut_ResetContFlag( wk, bpp, param->flag );
    return 1;
  }
  return 0;
}
/**
 * サイドエフェクトハンドラ消去
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_sideEffectRemove( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param = (const BTL_HANDEX_PARAM_SIDEEFF_REMOVE*)(param_header);
  u32 i;
  u8 result = 0;
  for(i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
  {
    if( BTL_CALC_BITFLG_Check(param->flags, i) ){
      if( BTL_HANDLER_SIDE_Remove(param->side, i) ){
        if( param->fExMsg )
        {
          WazaID waza = BTL_CALC_SideEffectIDtoWazaID( i );
          SCQUE_PUT_MSG_SET( wk->que, param->exStrID, param_header->userPokeID, waza );
        }
        result = 1;
      }
    }
  }
  return result;
}
/**
 * フィールドエフェクトハンドラ追加
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_addFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_FLDEFF* param = (const BTL_HANDEX_PARAM_ADD_FLDEFF*)(param_header);

  if( BTL_FIELD_AddEffect(param->effect, param->cont) ){
    SCQUE_PUT_OP_AddFieldEffect( wk->que, param->effect, param->cont.raw );
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
//----------------------------------------------------------------------------------
// 文字出力共通処理
//----------------------------------------------------------------------------------
static void handexSub_putString( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_STR_PARAMS* strParam )
{
  switch( strParam->type ){
  case BTL_STRTYPE_STD:
    scPut_Message_StdEx( wk, strParam->ID, strParam->argCnt, strParam->args );
    break;
  case BTL_STRTYPE_SET:
    scPut_Message_SetEx( wk, strParam->ID, strParam->argCnt, strParam->args );
    break;
  }
}

/**
 * フィールドエフェクトハンドラ削除
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_removeFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_REMOVE_FLDEFF* param = (const BTL_HANDEX_PARAM_REMOVE_FLDEFF*)(param_header);

  BTL_FIELD_RemoveEffect( param->effect );
  SCQUE_PUT_OP_RemoveFieldEffect( wk->que, param->effect );
  return 1;
}
/**
 * 位置エフェクトハンドラ追加
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_PosEffAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_POSEFF_ADD* param = (const BTL_HANDEX_PARAM_POSEFF_ADD*)(param_header);

  if( BTL_HANDLER_POS_Add(param->effect, param->pos, param_header->userPokeID, param->param, param->param_cnt) != NULL ){
    return 1;
  }
  return 0;
}
/**
 * ポケモンとくせい書き換え
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = (const BTL_HANDEX_PARAM_CHANGE_TOKUSEI*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BTL_HANDLER_TOKUSEI_Remove( bpp );
  SCQUE_PUT_OP_ChangeTokusei( wk->que, param->pokeID, param->tokuseiID );
  BPP_ChangeTokusei( bpp, param->tokuseiID );

  if( param->tokuseiID != POKETOKUSEI_NULL )
  {
    BTL_HANDLER_TOKUSEI_Add( bpp );
  }
  return 1;
}
/**
 * ポケモン装備アイテム書き換え
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_setItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_ITEM* param = (const BTL_HANDEX_PARAM_SET_ITEM*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( param_header->userPokeID != param->pokeID )
  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    u8  failed = scEventSetItem( wk, bpp );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );

    if( failed ){
      return 0;
    }
  }

  handexSub_itemSet( wk, bpp, param->itemID );
  if( param->exStr.type != BTL_STRTYPE_NULL ){
    BTL_Printf("アイテム書き換え成功メッセージ\n");
    handexSub_putString( wk, &param->exStr );
  }
  scproc_CheckItemReaction( wk, bpp );
  return 1;
}
/**
 * ポケモンにアイテム効果を強制発動
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_equipItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_EQUIP_ITEM* param = (BTL_HANDEX_PARAM_EQUIP_ITEM*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    BTL_EVENT_FACTOR* factor = BTL_HANDLER_ITEM_TMP_Add( bpp, param->itemID );
    if( factor )
    {
      u32 hem_state = Hem_PushState( &wk->HEManager );
      scEvent_ItemEquipTmp( wk, bpp );
      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      Hem_PopState( &wk->HEManager, hem_state );
      BTL_HANDLER_ITEM_TMP_Remove( factor );
      return 1;
    }
  }
  return 0;
}
/**
 * ポケモン自分のアイテム消費
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_consumeItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  scPut_RemoveItem( wk, bpp );
  return 1;
}
/**
 * ポケモン装備アイテム交換
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_swapItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SWAP_ITEM* param = (const BTL_HANDEX_PARAM_SWAP_ITEM*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* self = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  {
    u32 hem_state = Hem_PushState( &wk->HEManager );
    u8  failed = scEventSetItem( wk, target );
    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    Hem_PopState( &wk->HEManager, hem_state );
    if( failed ){
      return 0;
    }
  }

  {
    u16 selfItem = BPP_GetItem( self );
    u16 targetItem = BPP_GetItem( target );

    handexSub_itemSet( wk, self, targetItem );
    handexSub_itemSet( wk, target, selfItem );
  }

  if( param->fSucceedMsg ){
    scPut_Message_SetExPoke( wk, self, param->succeedStrID, param->succeedStrArgCnt, param->succeedStrArgs );
  }
  scproc_CheckItemReaction( wk, self );
  scproc_CheckItemReaction( wk, target );

  return 1;
}
//----------------------------------------------------------------------------------
// アイテム書き換え共通処理
//----------------------------------------------------------------------------------
static void handexSub_itemSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID )
{
  u8 pokeID = BPP_GetID( bpp );

  BTL_HANDLER_ITEM_Remove( bpp );
  SCQUE_PUT_OP_SetItem( wk->que, pokeID, itemID );
  BPP_SetItem( bpp, itemID );

  if( itemID != ITEM_DUMMY_DATA ){
    BTL_HANDLER_ITEM_Add( bpp );
  }
}
/**
 * ポケモンわざ書き換え
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_updateWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_UPDATE_WAZA* param = (const BTL_HANDEX_PARAM_UPDATE_WAZA*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  SCQUE_PUT_OP_UpdateWaza( wk->que, param->pokeID, param->wazaIdx, param->waza, param->ppMax, param->fPermanent );
  BPP_WAZA_UpdateID( target, param->wazaIdx, param->waza, param->ppMax, param->fPermanent );
  return 1;
}
/**
 * ポケモンカウンタ値書き換え
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_counter( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_COUNTER* param = (const BTL_HANDEX_PARAM_COUNTER*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  SCQUE_PUT_OP_SetPokeCounter( wk->que, param->pokeID, param->counterID, param->value );
  BPP_COUNTER_Set( target, param->counterID, param->value );
  return 1;
}
/**
 * 時間差ワザダメージ
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_delayWazaDamage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_DELAY_WAZADMG* param = (const BTL_HANDEX_PARAM_DELAY_WAZADMG*)param_header;

  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, param->attackerPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->targetPokeID );

  SVFL_WAZAPARAM   wazaParam;
  u16  que_reserve_pos;

  scEvent_GetWazaParam( wk, param->wazaID, attacker, &wazaParam );

  // ワザメッセージ，エフェクト，ワザ出し確定
  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );
  wk->wazaEff_EnableFlag = FALSE;
  wk->wazaEff_TargetPokeID = BTL_POKEID_NULL;

  // ワザ対象をワークに取得
  TargetPokeRec_Clear( &wk->damagedPokemon );
  TargetPokeRec_Clear( &wk->targetPokemon );
  TargetPokeRec_Add( &wk->targetPokemon, target );

  // @@@単純に死んでるポケモンを除外してるが…ホントはダブルとかだと隣のポケモンに当たったりするハズ
  TargetPokeRec_RemoveDeadPokemon( &wk->targetPokemon );

  // 対象ごとの無効チェック＆回避チェック
  flowsub_checkNotEffect( wk, &wazaParam, attacker, &wk->targetPokemon );
  flowsub_checkWazaAvoid( wk, wazaParam.wazaID, attacker, &wk->targetPokemon );

  scproc_Fight_Damage_Root( wk, &wk->wazaParam, attacker, &wk->targetPokemon );

  // ワザ効果あり確定→演出表示コマンド生成などへ
  if( wk->wazaEff_EnableFlag ){
    scproc_Fight_WazaEffective( wk, wazaParam.wazaID, param->attackerPokeID, wk->wazaEff_TargetPokeID, que_reserve_pos );
    return 1;
  }

  return 0;
}
/**
 * バトル離脱
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_quitBattle( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  if( scproc_NigeruCore(wk, bpp ) ){
    wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
    return 1;
  }
  return 0;
}
/**
 * メンバー入れ替え
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_changeMember( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_CHANGE_MEMBER* param = (BTL_HANDEX_PARAM_CHANGE_MEMBER*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_MemberOut(wk, bpp) )
  {
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param->pokeID );
    BTL_SERVER_RequestChangePokemon( wk->server, pos );
    handexSub_putString( wk, &param->exStr );
    wk->flowResult = SVFLOW_RESULT_POKE_CHANGE;
    return 1;
  }
  return 0;
}
/**
 * バトンタッチ
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_batonTouch( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_BATONTOUCH* param = (BTL_HANDEX_PARAM_BATONTOUCH*)param_header;

  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon, param->userPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->targetPokeID );

  BPP_BatonTouchParam( target, user );
  SCQUE_PUT_OP_BatonTouch( wk->que, param->userPokeID, param->targetPokeID );

  return 1;
}
/**
 * ひるませる
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_addShrink( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ADD_SHRINK* param = (BTL_HANDEX_PARAM_ADD_SHRINK*)param_header;

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_AddShrinkCore(wk, target, WAZANO_NULL, param->per) ){
    return 1;
  }
  return 0;
}
/**
 * 瀕死から回復
 * @return 成功時 1 / 失敗時 0
 */
static u8 scproc_HandEx_relive( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RELIVE* param = (BTL_HANDEX_PARAM_RELIVE*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BPP_HpPlus( target, param->recoverHP );
  SCQUE_PUT_OP_HpPlus( wk->que, param->pokeID, param->recoverHP );
  wk->pokeDeadFlag[param->pokeID] = FALSE;
  BTL_Printf("回復HP量=%d ... %d\n", param->recoverHP, BPP_GetValue(target,BPP_HP));
  handexSub_putString( wk, &param->exStr );
  return 1;
}


