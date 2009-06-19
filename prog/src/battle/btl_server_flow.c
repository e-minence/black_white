//=============================================================================================
/**
 * @file  btl_server_flow.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h��������
 * @author  taya
 *
 * @date  2009.03.06  �쐬
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

#include "btl_server_flow.h"

//--------------------------------------------------------
/**
 *    �T�[�o�t���[�����t���O
 */
//--------------------------------------------------------
enum {
  HANDLER_EXHIBISION_WORK_TOTALSIZE = 512,    ///< �n���h�����������i�[���郏�[�N�̃g�[�^���T�C�Y
};

//--------------------------------------------------------
/**
 *    �T�[�o�t���[�����t���O
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
 *  �ӂ��Ƃ΂��n�̃��U�����p�^�[��
 */
typedef enum {
  PUSHOUT_EFFECT_ESCAPE,    ///< �������̂Ɠ����i�퓬���I������j
  PUSHOUT_EFFECT_CHANGE,    ///< ��������ւ�������i�莝�������Ȃ���Ύ��s����j
  PUSHOUT_EFFECT_MUSTFAIL,  ///< �K�����s����
}PushOutEffect;


/**
 *  �A�N�V�����D�揇�L�^�\����
 */
typedef struct {
  u8  clientID;   ///< �N���C�A���gID
  u8  pokeIdx;    ///< ���̃N���C�A���g�́A���̖ځH 0�`
}ACTION_ORDER_WORK;


/**
 * �ΏۂƂȂ�|�P�����p�����[�^���L�^���Ă������߂̃��[�N
 */
typedef struct {

  BTL_POKEPARAM*  pp[ BTL_POS_MAX ];
  u16             damage[ BTL_POS_MAX ];
  u8              count;
  u8              idx;

}TARGET_POKE_REC;

/**
 *  �퓬�ɏo�Ă���|�P�����f�[�^�ɏ��ԂɃA�N�Z�X���鏈���̃��[�N
 */
typedef struct {
  u8 clientIdx;
  u8 pokeIdx;
  u8 endFlag;
}FRONT_POKE_SEEK_WORK;

/**
 *  �C�x���g - �n���h���� �A�����[�N�̈�
 */

typedef struct {

  u8    work[ EVENT_WORK_TOTAL_SIZE ];
  u16   size[ EVENT_WORK_DEPTH ];
  u32   sp;

}BTL_EVENT_WORK_STACK;


/**
 *  �n���h�������ۑ��p���[�N�}�l�[�W��
 */
typedef struct {

  u16  stack_ptr;
  u16  read_ptr;

  u8   workBuffer[ HANDLER_EXHIBISION_WORK_TOTALSIZE ];

}HANDLER_EXHIBISION_MANAGER;

/**
 *  ���b�Z�[�W�\�������R�[���o�b�N�֐��^
 */
typedef void (*pMsgCallback)( BTL_SVFLOW_WORK*, const BTL_POKEPARAM*, void* );


/**
 *  ���b�Z�[�W�\�������R�[���o�b�N�p�p�����[�^
 */
typedef struct {
  pMsgCallback    func;
  void*           arg;
}MSG_CALLBACK_PARAM;

//-----------------------------------------------------
/**
 *  ���C�����[�N
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

  u8          flowFlags[ FLOWFLG_BYTE_MAX ];
};



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void clear_poke_actflags( BTL_SVFLOW_WORK* wk );
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
static void scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void handex_addsick_msg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, void* arg );
static void scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static void scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_messageSet( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_messageStd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BtlEventType event_type );
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, BtlEventType event_type );
static void scproc_turncheck_sick( BTL_SVFLOW_WORK* wk );
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
static void scPut_RankEffectFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, const BTL_EVWK_CHECK_RANKEFF* evwk );
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
static void scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8* spePriA, u8* spePriB );
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_CheckWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param );
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause );
static void scEvent_WazaExecuteFix( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
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
static BOOL scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza, BTL_EVWK_DMG_TO_RECOVER* evwk );
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
  WazaRankEffect effect, int volume, BTL_EVWK_CHECK_RANKEFF* evwk );
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
        // @@@ ��X�́A�퓬�J�n����MemberIn�Ɠr������ւ���MemberIn�ł͉��o��ʂ���K�v�A���B
        BTL_Printf("client(%d, posIdx=%d poke...In!\n", i, posIdx);
        scproc_MemberIn( wk, i, posIdx, posIdx, TRUE );
      }
    }
  }

  scproc_AfterMemberIn( wk );

  // @@@ ���ʂ��Ƃ����邩��
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

      BTL_Printf("Client(%d) �� %d �Ԗڂ̃|�P�����̃A�N�V����\n", clientID, pokeIdx);
      switch( action->gen.cmd ){
      case BTL_ACTION_FIGHT:
        BTL_Printf("�y���������z�������B���U[%d]���A�ʒu%d�Ԃ̑���ɁB\n", action->fight.waza, action->fight.targetPos);
        scproc_Fight( wk, clientID, pokeIdx, action );
        break;
      case BTL_ACTION_ITEM:
        BTL_Printf("�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n", action->item.number, action->item.targetIdx);
        break;
      case BTL_ACTION_CHANGE:
        BTL_Printf("�y�|�P�����z�������B�ʒu%d <- �|�P%d \n", action->change.posIdx, action->change.memberIdx);
        scproc_MemberOut( wk, clientID, action->change.posIdx );
        scproc_MemberIn( wk, clientID, action->change.posIdx, action->change.memberIdx, FALSE );
        scproc_AfterMemberIn( wk );
        break;
      case BTL_ACTION_ESCAPE:
        BTL_Printf("�y�ɂ���z�������B\n");
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
        BTL_Printf("�������X�L�b�v\n");
        scPut_CantAction( wk, clientID, pokeIdx );
        break;
      case BTL_ACTION_NULL:
        BTL_Printf("�s���ȏ���\n");
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

  // �^�[���`�F�b�N����
  scproc_TurnCheck( wk );

  alivePokeAfter = countAlivePokemon( wk );

  // ���񂾃|�P����������ꍇ�̏���
  BTL_Printf( "�|�P������ %d -> %d ...\n", alivePokeBefore, alivePokeAfter );
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
 * �P�^�[�����T�[�o�R�}���h�����i�|�P�����I����j
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

  BTL_Printf("�Ђ񂵃|�P��������ւ��I����̃T�[�o�[�R�}���h����\n");

  SCQUE_Init( wk->que );

  for(i=0; i<wk->numClient; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    actionCnt = BTL_SVCL_GetNumActPoke( clwk );

    for(j=0; j<actionCnt; ++j)
    {
      action = BTL_SVCL_GetPokeAction( clwk, j );
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      BTL_Printf("�N���C�A���g(%d)�̃|�P����(�ʒu%d) ���A%d�Ԗڂ̃|�P�Ƃ��ꂩ����\n",
            i, action->change.posIdx, action->change.memberIdx );

      scproc_MemberIn( wk, i, action->change.posIdx, action->change.memberIdx, FALSE );
    }
  }
  scproc_AfterMemberIn( wk );

  {
    // @@@ ������u�܂��т��v�Ƃ��Ŏ��ʂ��Ƃ�����
    return SVFLOW_RESULT_DEFAULT;
  }
}

//--------------------------------------------------------------------------
/**
 * �N���C�A���g�̃A�N�V�������`�F�b�N���A�����������m��
 *
 * @param   server
 * @param   order   �������鏇�ԂɃN���C�A���gID����ג����Ċi�[���邽�߂̔z��
 *
 */
//--------------------------------------------------------------------------
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order )
{
  /*
    �s���D�揇  ...  3 BIT
    ����D�揇A ...  3 BIT
    ���U�D�揇  ...  6 BIT
    ����D�揇B ...  3 BIT
    �f����      ... 16 BIT
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

// �e�|�P�����̍s���v���C�I���e�B�l�𐶐�
  for(i=0, p=0; i<wk->numClient; i++)
  {
    // �N���C�A���g���Ƃ̍s���|�P���������p�����[�^���A�_�v�^����擾
    clwk = BTL_SERVER_GetClientWork( wk->server, i );

    numPoke = BTL_SVCL_GetNumActPoke( clwk );
    for(j=0; j<numPoke; j++)
    {
      bpp = clwk->frontMember[j];
      actParam = BTL_SVCL_GetPokeAction( clwk, j );

      // �s���ɂ��D�揇�i�D��x�����قǐ��l��j
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
      // ���U�ɂ��D�揇
      if( actParam->gen.cmd == BTL_ACTION_FIGHT ){
        WazaID  w = actParam->fight.waza;
        wazaPri = WAZADATA_GetPriority( w ) - WAZAPRI_MIN;
        // �A�C�e�������ȂǁA����ȗD��t���O
        scEvent_CheckSpecialActPriority( wk, bpp, &spPri_A, &spPri_B );
      }else{
        wazaPri = 0;
        spPri_A = BTL_SPPRI_A_DEFAULT;
        spPri_B = BTL_SPPRI_B_DEFAULT;
      }
      // ���΂₳
      agility = scEvent_CalcAgility( wk, bpp );

      BTL_Printf("�s���v���C�I���e�B����I Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
          i, j, actionPri, wazaPri, agility );

      // �v���C�I���e�B�l�ƃN���C�A���gID��΂ɂ��Ĕz��ɕۑ�
      pri[p] = MakePriValue( actionPri, spPri_A, wazaPri, spPri_B, agility );
      order[p].clientID = i;
      order[p].pokeIdx = j;
      ++p;
    }
  }
// �v���C�I���e�B�l�\�[�g�ɔ����ăN���C�A���gID�z����\�[�g
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
// �S�������v���C�I���e�B�l���������烉���_���V���b�t��
// @@@ ������

  return p;
}

// ���A�퓬�ɏo�Ă��Đ����Ă���|�P�����̐����J�E���g
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
// �T�[�o�[�t���[�t���O����
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
// �퓬�ɏo�Ă���|�P�����f�[�^���ԃA�N�Z�X����
//----------------------------------------------------------------------------------------------

/**
 *  ���ԃA�N�Z�X�F���[�N������
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
        if( !BTL_POKEPARAM_IsDead(cw->member[j]) )
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
 *  ���ԃA�N�Z�X�F���̃|�P�����f�[�^���擾�iFALSE���Ԃ�����I���j
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

    *bpp = cw->member[ fpsw->pokeIdx ];
    fpsw->pokeIdx++;

    while( fpsw->clientIdx < BTL_CLIENT_MAX )
    {
      cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, fpsw->clientIdx );
      if( cw )
      {
        while( fpsw->pokeIdx < cw->numCoverPos )
        {
          nextPoke = cw->member[ fpsw->pokeIdx ];
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
// �Ώۃ|�P�����L�^���[�N
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
  GF_ASSERT(0); // �|�P����������Ȃ�
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
// �w��|�P�����Ɠ��`�[���̃|�P�����ʒu�C���f�b�N�X��Ԃ��i������Ȃ����-1�j
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
// �|�P����������Ԃ�
static inline u32 TargetPokeRec_GetCount( const TARGET_POKE_REC* rec )
{
  return rec->count;
}
// �w��|�P�����Ɠ��`�[���̃|�P�����݂̂�ʃ��[�N�ɃR�s�[
// return : �R�s�[�����|�P������
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
// �w��|�P�����ƓG�`�[���̃|�P�����݂̂�ʃ��[�N�ɃR�s�[
// return : �R�s�[�����|�P������
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
// ����ł�|�P�������폜
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
// �T�[�o�[�t���[����
//======================================================================================================

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u�ɂ���v
//-----------------------------------------------------------------------------------
static BOOL scput_Nigeru( BTL_SVFLOW_WORK* wk, u8 clientID, u8 pokeIdx )
{
  SVCL_WORK* clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  BTL_POKEPARAM* bpp = clwk->member[ pokeIdx ];

  BOOL fEscape = TRUE;  // @@@ ���͂��΂₳�v�Z�������N�ł���������悤�ɂ��Ă���

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
// �T�[�o�[�t���[�F�����o�[����ɓo�ꂳ����
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
// �T�[�o�[�t���[�F�����o�[���ꂩ��ޏꂳ����
//-----------------------------------------------------------------------------------
static void scproc_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx )
{
  scEvent_MemberOut( wk, clientID, posIdx );
}
//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v���[�g
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

    // ���U�o�����s����
    if( scproc_Fight_CheckWazaExecuteFail(wk, attacker, waza) ){ break; }

    // ���U�o������
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
 * �u���������v-> ���U�o�������ȍ~�̏���
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

// @@@ �����ɁA�Ώۃ|�P�ɂ��Ȃ��������`�F�b�N�����邩���c

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

  // ���U���ʂ���m�聨���o�\���R�}���h�����Ȃǂ�
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
 * �Ώۃ|�P�����̃f�[�^�|�C���^���ATARGET_POKE_REC�\���̂ɃZ�b�g
 *
 * @param   wk
 * @param   atPos
 * @param   waza
 * @param   action
 * @param   rec       [out]
 *
 * @retval  u8        �Ώۃ|�P������
 */
//--------------------------------------------------------------------------
static u8 flowsub_registerWazaTargets( BTL_SVFLOW_WORK* wk, BtlPokePos atPos, WazaID waza, const BTL_ACTION_PARAM* action, TARGET_POKE_REC* rec )
{
  WazaTarget  targetType = WAZADATA_GetTarget( waza );
  BTL_POKEPARAM* attacker = BTL_POKECON_GetFrontPokeData( wk->pokeCon, atPos );

  TargetPokeRec_Clear( rec );

  // �V���O��
  if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE )
  {
    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:  ///< �����ȊO�̂P�́i�I���j
    case WAZA_TARGET_ENEMY_SELECT:  ///< �G�P�́i�I���j
    case WAZA_TARGET_ENEMY_RANDOM:  ///< �G�����_��
    case WAZA_TARGET_ENEMY_ALL:     ///< �G���Q��
    case WAZA_TARGET_OTHER_ALL:     ///< �����ȊO�S��
      TAYA_Printf("�^�[�Q�b�g�����āFwaza=%d, type=%d\n", waza, targetType);
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
      break;

    case WAZA_TARGET_USER:      ///< �����P�̂̂�
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P��
      TAYA_Printf("�^�[�Q�b�g���Ԃ�Fwaza=%d, type=%d\n", waza, targetType);
      TargetPokeRec_Add( rec, attacker );
      break;

    default:
      return 0;
    }

    return 1;
  }
  // �_�u��
  else
  {
    BTL_POKEPARAM* bpp = NULL;

    switch( targetType ){
    case WAZA_TARGET_OTHER_SELECT:        ///< �����ȊO�̂P�́i�I���j
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, action->fight.targetPos );
      TargetPokeRec_Add( rec, bpp );
      return 1;
    case WAZA_TARGET_ENEMY_SELECT:        ///< �G�P�́i�I���j
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, action->fight.targetPos );
      TargetPokeRec_Add( rec, bpp );
      return 1;
    case WAZA_TARGET_ENEMY_RANDOM:        ///< �G�����_��
      bpp = get_opponent_pokeparam( wk, atPos, GFL_STD_MtRand(1) );
      TargetPokeRec_Add( rec, bpp );
      return 1;

    case WAZA_TARGET_ENEMY_ALL:           ///< �G���S��
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
      TargetPokeRec_Add( rec, get_opponent_pokeparam(wk, atPos, 1) );
      return 2;

    case WAZA_TARGET_OTHER_ALL:           ///< �����ȊO�S��
      TargetPokeRec_Add( rec, get_next_pokeparam( wk, atPos ) );
      TargetPokeRec_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
      TargetPokeRec_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
      return 3;

    case WAZA_TARGET_USER:      ///< �����P�̂̂�
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
      return 1;
    case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P�́i�I���j
      TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, action->fight.targetPos) );
      return 1;
    case WAZA_TARGET_FRIEND_SELECT:       ///< �����ȊO�̖����P��
      TargetPokeRec_Add( rec, get_next_pokeparam(wk, atPos) );
      return 1;
    default:
      return 0;
    }

    if( bpp )
    {
      u8 targetPokeID = BTL_POKEID_NULL;
      BTL_EVENTVAR_Push();
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
        BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wk->wazaParam.wazaType );
        BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEID_NULL );
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECIDE_TARGET );
        targetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
      BTL_EVENTVAR_Pop();
      if( targetPokeID != BTL_POKEID_NULL )
      {
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
 * �K����Ԃ��ǂ����`�F�b�N
 *
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL    �K����ԂȂ�TRUE
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
 * ���U�����O�́A�Ώەʖ������`�F�b�N
 *
 * @param   wk
 * @param   attacker  [in] ���U���o���|�P����
 * @param   targets   [io] ���U�Ώۃ|�P�����R���e�i�^�����ɂȂ����|�P�����̓R���e�i����폜�����
 *
 */
//--------------------------------------------------------------------------
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  BTL_POKEPARAM* bpp;

  TargetPokeRec_GetStart( targets );
  // �^�C�v�����ɂ�閳�����`�F�b�N
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckNotEffect_byType(wk, waza, attacker, bpp) )
    {
      TargetPokeRec_Remove( targets, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(bpp) );
    }
  }

  // Lv1 �������`�F�b�N�i�U���|�P���K����ԂȂ疳�����ł��Ȃ��j
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

  // Lv2 �������`�F�b�N�i�U���|�P���K����Ԃł��������j
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( scEvent_CheckNotEffect(wk, waza, 1, attacker, bpp) )
    {
      TargetPokeRec_Remove( targets, bpp );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(bpp) );
    }
  }

  // �܂���`�F�b�N
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_MAMORU) )
    {
      // �܂��閳��������Ȃ���΃^�[�Q�b�g���珜�O
      if( !scEvent_CheckMamoruBreak(wk, attacker, bpp, waza) ){
        TargetPokeRec_Remove( targets, bpp );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Mamoru, BTL_POKEPARAM_GetID(bpp) );
      }
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���U���ʂ��邱�Ƃ��m�聨�K�v�ȉ��o�R�}���h�̐���
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
 * �K�v�Ȃ�A�P�^�[�����߃��U�̗��߃^�[���������s���B
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   action    [io]
 *
 * @retval  BOOL      ���߃^�[���������s�����ꍇTRUE
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
 * ���U�o�����s�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  SV_WazaFailCause    ���s�����ꍇ���s�R�[�h�^�����̏ꍇ SV_WAZAFAIL_NULL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckWazaExecuteFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;

  do {
    // �����񎩔�����
    if( scproc_Fight_CheckConf(wk, attacker) ){
      cause = SV_WAZAFAIL_KONRAN;
      break;
    }

    // �˂ނ�E�����蓙�̉����`�F�b�N
    scproc_PokeSickCure_WazaCheck( wk, attacker, waza );

    // ���̑��̎��s�`�F�b�N
    cause = scEvent_CheckWazaExecute( wk, attacker, waza );

  }while( 0 );


  if( cause != SV_WAZAFAIL_NULL ){
    BTL_Printf("�|�PID=%d, ���s�R�[�h=%d�ł���\n", BTL_POKEPARAM_GetID(attacker), cause);
    scproc_WazaExecuteFailed( wk, attacker, waza, cause );
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����񎩔��`�F�b�N������
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    ���������ꍇTRUE
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
 * �˂ނ�E�����蓙�A�A�N�V�����I����ɉ����`�F�b�N���s������
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
 * [Proc] ���U�o�����s������
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
 * [Event] �܂���E�݂���̖������`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL    ����������ꍇTRUE
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
// �w��ʒu���猩�đΐ푊��̃|�P�����f�[�^��Ԃ�
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
// �w��ʒu���猩�ėׂ̃|�P�����f�[�^��Ԃ�
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
 * [Proc] �g�������U��PP�f�N�������g
 *
 * @param   wk
 * @param   attacker  �U���|�P�����p�����[�^
 * @param   wazaIdx   �g�������U�C���f�b�N�X
 * @param   rec       �_���[�W���󂯂��|�P�����p�����[�^�Q
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
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n
//----------------------------------------------------------------------
static void scproc_Fight_Damage( BTL_SVFLOW_WORK* wk, WazaID waza,
   BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
  u8 target_cnt;
  fx32 dmg_ratio;

  FlowFlg_Clear( wk, FLOWFLG_SET_WAZAEFFECT );

  target_cnt = TargetPokeRec_GetCount( targets );
  dmg_ratio = (target_cnt == 1)? BTL_CALC_DMG_TARGET_RATIO_NONE : BTL_CALC_DMG_TARGET_RATIO_PLURAL;

// �����ɑ΂���_���[�W
  if( TargetPokeRec_CopyFriends(targets, attacker, &wk->targetSubPokemon) )
  {
    TargetPokeRec_RemoveDeadPokemon( &wk->targetSubPokemon );
    scproc_Fight_Damage_side( wk, waza, attacker, &wk->targetSubPokemon, dmg_ratio );
  }
// �G�ɑ΂���_���[�W
  if( TargetPokeRec_CopyEnemys(targets, attacker, &wk->targetSubPokemon) )
  {
    // @@@ �{���͎���ł���ʃ^�[�Q�b�g�ɂ��Ȃ��ƃ_������
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

  // �����`�F�b�N -> �������Ȃ������ꍇ�� targets ���珜�O
  TargetPokeRec_GetStart( targets );
  while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
  {
    if( !scEvent_checkHit(wk, attacker, bpp, waza) )
    {
      scPut_WazaAvoid( wk, bpp, waza );
      TargetPokeRec_Remove( targets, bpp );
    }
  }
  // �񕜃`�F�b�N -> ����^�C�v�̃_���[�W�𖳌����A�񕜂��Ă��܂��|�P�� targets ���珜�O
  {
    BTL_EVWK_DMG_TO_RECOVER* ewk = eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_DMG_TO_RECOVER) );
    TargetPokeRec_GetStart( targets );
    while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
    {
      if( scEvent_DmgToRecover(wk, bpp, waza, ewk) )
      {
        scput_DmgToRecover( wk, bpp, ewk );
        TargetPokeRec_Remove( targets, bpp );
      }
    }
    eventWork_Pop( &wk->eventWork, ewk );
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

  // �������ʂ̃��c������ or �������n���h���ɂ���ĕω����ꂽ�ꍇ�͌ʂ̏���
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
    // �������P�̂ł��ʂ̃��c������Ȃ�ʂ̏���
    if( !f_same_aff )
    {
      for(i=0; i<poke_cnt; ++i){
        bpp = TargetPokeRec_Get( targets, i );
        scproc_Fight_Damage_side_single( wk, attacker, bpp, waza, dmg_ratio, evwkAry[i] );
      }
    }
    // �������S�Ẵ^�[�Q�b�g�ň�v�Ȃ�A�̗̓o�[�𓯎��Ɍ��炷�悤�ɃR�}���h��������
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
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i��ʕ���j
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
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
    WazaID waza,   BTL_EVWK_CHECK_AFFINITY* evwkAff,   fx32 targetDmgRatio )
{
  u8 plural_flag = FALSE; // ������q�b�g�t���O
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

    // �f�o�b�O���ȒP�ɂ��邽�ߕK����_���[�W�ɂ���[�u
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

    if( BTL_POKEPARAM_IsDead(defender) )
    {
      deadFlag = TRUE;
    }
    if( BTL_POKEPARAM_IsDead(attacker) )
    {
      deadFlag = TRUE;
    }
    if( deadFlag ){ break; }
  }

  TargetPokeRec_AddWithDamage( &wk->damagedPokemon, defender, damage_sum );

  if( plural_flag )
  {
    // @@@ �u�����񂠂������I�v���b�Z�[�W
//    scPut_WazaDamageAffMsg( wk, evwkAff );
//    SCQUE_PUT_MSG_WazaHitCount( wk->que, i ); // @@@ ���Ƃ�
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
//--------------------------------------------------------------------------
/**
 * ���U�_���[�W��̒ǉ����ʏ���
 *
 * @param   wk
 * @param   waza        �g�p���U
 * @param   attacker    �U���|�P�����p�����[�^
 * @param   defender    �h��|�P�����p�����[�^
 * @param   damage      �^�����_���[�W
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
 * ���U�ɂ��_���[�W��̊e�����
 */
//--------------------------------------------------------------------------
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk )
{
  switch( evwk->reaction ){
  case BTL_EV_AFTER_DAMAGED_REACTION_SICK:        ///< �Ώۃ|�P��������Ԉُ�ɂ���
    scproc_WazaDamageReaction_Sick( wk, evwk );
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_RANK:        ///< �Ώۃ|�P�����̃����N��������
    scproc_WazaDamageReaction_Rank( wk, evwk );
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE:      ///< �Ώۃ|�P�����Ƀ_���[�W��^����
    scproc_WazaDamageReaction_Damage( wk, evwk );
    break;
  case BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE:    ///< �Ώۃ|�P�����̃^�C�v�ύX
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
 * [proc] ��Ԉُ틤�ʏ���
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fAlmost            ���s�������Ɍ������b�Z�[�W��\������
 * @param   msgCallbackParam   �������A���b�Z�[�W�\���R�}���h�������邽�߂̃R�[���o�b�N�B
 *                             �f�t�H���g���b�Z�[�W�ŗǂ����NULL�B
 *
 * @retval  BOOL       ���������ꍇTRUE
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
 * ��Ԉُ�R�A����
 *
 * @param   wk
 * @param   target            ��Ԉُ�����炤�|�P
 * @param   sick              ��Ԉُ�ID
 * @param   sickCont          ��Ԉُ�p���p�����[�^
 * @param   fAlmost           �قڊm��t���O
 *
 * @retval  BOOL    ��Ԉُ�ɂ����ꍇ�ATRUE
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
  // �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
  if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI)
  ){
    return BTL_ADDSICK_FAIL_OTHER;
  }
  // ���łɍ����Ȃ獬���ɂȂ�Ȃ��Ƃ��c
  if( BTL_POKEPARAM_CheckSick(target, sick) ){
    return BTL_ADDSICK_FAIL_ALREADY;
  }

  // ���łɃ|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ�A�V���Ƀ|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
  if( sick < POKESICK_MAX )
  {
    if( BTL_POKEPARAM_GetPokeSick(target) != POKESICK_NULL ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // @@@ ���łɍ����Ȃ獬���ɂȂ�Ȃ��Ƃ��c ���s�R�[�h��Ԃ��K�v�����邩��
  if( BTL_POKEPARAM_CheckSick(target, sick) ){
    return BTL_ADDSICK_FAIL_ALREADY;
  }

  return BTL_ADDSICK_FAIL_NULL;
}
static BOOL scEvent_AddSick_IsMustFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
  // �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
  if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI )
  ){
    return TRUE;
  }
  // ���łɃ|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ�A�V���Ƀ|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
  if( (BTL_POKEPARAM_GetPokeSick(target) != POKESICK_NULL)
  &&  (sick < POKESICK_MAX)
  ){
    return TRUE;
  }

  // @@@ ���łɍ����Ȃ獬���ɂȂ�Ȃ��Ƃ��c ���s�R�[�h��Ԃ��K�v�����邩��
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, failFlag );
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
// �T�[�o�[�t���[�F�A�C�e���g�p
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
 * [Put] ���݂̂�H�ׂ�A�N�V����
 */
//----------------------------------------------------------------------------------
static void scPut_EatNutsAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_ACT_KINOMI( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �A�C�e���g�p�ɂ��HP�񕜃��b�Z�[�W
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
 * [Put] �����A�C�e�����폜
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
 * [Event] �A�C�e���g�p���ʂ̎擾
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
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏���
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
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏��� > �Ђ�݃`�F�b�N
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
// �T�[�o�[�t���[�F���U�_���[�W�ɉ�����HP�z��������
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
// �T�[�o�[�t���[�������F�P�̕��U���q�b�g�`�F�b�N
//------------------------------------------------------------------
static BOOL svflowsub_hitcheck_singular( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza )
{
  u8 defPokeID = BTL_POKEPARAM_GetID( defender );

  // ���ɑΏۂ�����ł���
  if( BTL_POKEPARAM_IsDead(defender) )
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, defPokeID );
    return FALSE;
  }
  // �������E��𗦂ɂ��q�b�g�`�F�b�N�ŊO�ꂽ
  if( !scEvent_checkHit(wk, attacker, defender, waza) )
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, defPokeID );
    return FALSE;
  }
  return TRUE;
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �������� > �_���[�W���U�n > �_���[�W�l�v�Z
//------------------------------------------------------------------
static u16 scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag )
{
  u32 rawDamage = 0;

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetValue( BTL_EVAR_TYPEAFF, typeAff );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
  BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );

  {
    u16 atkPower, defGuard, wazaPower;

    // �e��p�����[�^����f�̃_���[�W�l�v�Z
    wazaPower = scEvent_getWazaPower( wk, attacker, defender, waza, &wk->wazaParam );
    atkPower  = scEvent_getAttackPower( wk, attacker, defender, waza, criticalFlag );
    defGuard  = scEvent_getDefenderGuard( wk, attacker, defender, waza, &wk->wazaParam, criticalFlag );
    {
      u8 atkLevel = BTL_POKEPARAM_GetValue( attacker, BPP_LEVEL );
      rawDamage = wazaPower * atkPower * (atkLevel*2/5+2) / defGuard / 50;
    }
    rawDamage = (rawDamage * targetDmgRatio) >> FX32_SHIFT;
    // �V��␳
    {
      fx32 weatherDmgRatio = BTL_FIELD_GetWeatherDmgRatio( waza );
      if( weatherDmgRatio != BTL_CALC_DMG_WEATHER_RATIO_NONE )
      {
        u32 prevDmg = rawDamage;
        rawDamage = (rawDamage * weatherDmgRatio) >> FX32_SHIFT;
        BTL_Printf("�V��ɂ��␳������, �␳��=%08x, dmg=%d->%d\n", weatherDmgRatio, prevDmg, rawDamage);
      }
    }
    // �N���e�B�J���łQ�{
    if( criticalFlag )
    {
      rawDamage *= 2;
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, rawDamage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGE_PROC1 );
    rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    BTL_Printf("�З�:%d, ��������:%d, �ڂ�����:%d,  ... �f�_��:%d\n",
        wazaPower, atkPower, defGuard, rawDamage );
  }
  //�����_���␳
  {
    u16 ratio = 100 - GFL_STD_MtRand(16);
    rawDamage = (rawDamage * ratio) / 100;
    BTL_Printf("�����_���␳:%d%%  -> �f�_��=%d\n", ratio, rawDamage);
  }
  // �^�C�v��v�␳
  {
    fx32 ratio = scEvent_CalcTypeMatchRatio( wk, attacker, wk->wazaParam.wazaType );
    rawDamage = (rawDamage * ratio) >> FX32_SHIFT;
  }
  // �^�C�v�����v�Z
  rawDamage = BTL_CALC_AffDamage( rawDamage, typeAff );
  BTL_Printf("�^�C�v����:%02d -> �_���[�W�l�F%d\n", typeAff, rawDamage);
  // �₯�Ǖ␳
  if( (WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC)
  &&  (BTL_POKEPARAM_GetPokeSick(attacker) == POKESICK_YAKEDO)
  ){
    rawDamage = (rawDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
  }

  if( rawDamage == 0 ){ rawDamage = 1; }

  BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1f), FX32_CONST(32) );

  BTL_EVENTVAR_RewriteValue( BTL_EVAR_DAMAGE, rawDamage );
  BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC2 );

  {
    fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    rawDamage = (rawDamage * ratio) >> FX32_SHIFT;
  }

  // �ō��Ŏc��g�o�͈̔͂Ɏ��܂�悤�ɍŏI�␳
  if( rawDamage > BTL_POKEPARAM_GetValue(defender, BPP_HP) ){
    rawDamage = BTL_POKEPARAM_GetValue( defender, BPP_HP );
  }

  BTL_EVENTVAR_Pop();

  return rawDamage;
}
//---------------------------------------------------------------------------------------------
// ���U�_���[�W������̔�������
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
// ���U�ȊO�̃_���[�W���ʏ���
//---------------------------------------------------------------------------------------------
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, u16 strID )
{
  if( scEvent_CheckEnableSimpleDamage(wk, bpp, damage) )
  {
    int value = -damage;

    scPut_SimpleHp( wk, bpp, value );

    if( strID != STRID_NULL )
    {
      scPut_Message_Set( wk, bpp, strID );
    }

    if( scEvent_SimpleDamage_Reaction(wk, bpp) )
    {
      scproc_UseItem( wk, bpp );
    }

    scPut_CheckDeadCmd( wk, bpp );

    // @@@ ����������TRUE�i���͏��TRUE�ɂ��Ă���j
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------------------------
// �|�P�����������I�ɕm���ɂ���
//---------------------------------------------------------------------------------------------
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_KillPokemon( wk, bpp, 0 );
  scPut_CheckDeadCmd( wk, bpp );
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ǉ����ʂɂ���Ԉُ�
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
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̏�Ԉُ�
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
// �T�[�o�[�t���[�F���U�ɂ���Ԉُ틤��
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
// �T�[�o�[�t���[�F�ǉ����ʂɂ�郉���N����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  if( !BTL_POKEPARAM_IsDead(target) )
  {
    if( scEvent_CheckAddRankEffectOccur(wk, waza, attacker, target) )
    {
      scproc_WazaRankEffect_Common( wk, waza, attacker, target, FALSE );
    }
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̃����N����
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
 * ���U�ɂ��i���ځE�ǉ����ʁj�����N�������ʂ̏���
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL    �����N�������ʂ�����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost )
{
  BOOL ret = FALSE;
  u32 eff_cnt, i;

  eff_cnt = WAZADATA_GetRankEffectCount( waza );
  BTL_Printf("�����N���ʂ̎��=%d\n", eff_cnt);
  for(i=0; i<eff_cnt; ++i)
  {
    WazaRankEffect  effect;
    int  volume;

    scEvent_GetWazaRankEffectValue( wk, waza, i, attacker, target, &effect, &volume );
    BTL_Printf("�����N���� %d, vol=%d\n", effect, volume );
    if( scproc_RankEffectCore(wk, target, effect, volume, ITEM_DUMMY_DATA, fAlmost, TRUE) )
    {
      ret = TRUE;
    }
  }
  return ret;
}
//--------------------------------------------------------------------------
/**
 * �����N�������ʂ̃R�A�i���U�ȊO��������Ăяo���j
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   itemID    �A�C�e���g�p�ɂ�郉���N�����Ȃ�A�C�e��ID�w��i����ȊO�� ITEM_DUMMY_DATA ���w��j
 * @param   fAlmost   ����v���Ō��ʎ��s�������A���̃��b�Z�[�W��\������
 * @param   fStdMsg   �W�����b�Z�[�W���o�͂���i�����́~�~�����������I�Ȃǁj
 *
 * @retval  BOOL    �����N�������ʂ�����������TRUE
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
    BTL_EVWK_CHECK_RANKEFF* evwk = (BTL_EVWK_CHECK_RANKEFF*)eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_CHECK_RANKEFF) );
    BOOL ret = TRUE;

    if( scEvent_CheckRankEffectSuccess(wk, target, effect, volume, evwk) )
    {
      BTL_Printf("�����N���ʔ����Ftype=%d, volume=%d\n", effect, volume );
      scPut_RankEffect( wk, target, effect, volume, itemID, fStdMsg );
      if( evwk->useItemFlag )
      {
        scproc_UseItem( wk, target );
      }
    }
    else
    {
      BTL_Printf("�����N���ʎ��s����\n");
      if( fAlmost )
      {
        if( evwk->failTokuseiFlag ){
          scPut_TokWin_In( wk, target );
        }
        scPut_RankEffectFail( wk, target, effect, evwk );
        if( evwk->failTokuseiFlag ){
          scPut_TokWin_Out( wk, target );
        }
      }
      ret = FALSE;
    }

    eventWork_Pop( &wk->eventWork, evwk );
    return ret;
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�����N���ʁ���Ԉُ�𓯎��ɗ^���郏�U
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
// �T�[�o�[�t���[�FHP��
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker )
{
  if( !BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KAIHUKUHUUJI) )
  {
    u32 recoverHP = scEvent_CalcRecoverHP( wk, waza, attacker );
    scPut_SimpleHp( wk, attacker, recoverHP );
  }
  // �����ӂ��ӂ����Ŏ��s
  else
  {
    u8 pokeID = BTL_POKEPARAM_GetID( attacker );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KaifukuFuji, pokeID, waza );
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ꌂ���U����
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

// �|�P�����n�E���U�n�ɂ���Ԉُ퉻�C�x���g�̌Ăѕ���
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
// �T�[�o�[�t���[�F�ӂ��Ƃ΂����U����
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
 * �u�ӂ��Ƃ΂��v�n�̃��U���ʂ��o�g���̃��[�������画�肷��
 *
 * @retval  BOOL  �K�����s����P�[�X��TRUE
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
 * �u�ӂ��Ƃ΂��v�n�̃��U�ŋ������ꂩ���������������A
 * ���ɏo���|�P�����̃p�[�e�B���C���f�b�N�X�������_���Ō��߂�
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
 * [Event] �ӂ��Ƃ΂����U���s�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL      ���s�Ȃ�TRUE
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
// �T�[�o�[�t���[�F���U�ɂ��V��̕ω�
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
// �T�[�o�[�t���[�F���ނ�����Ȃ����U
//---------------------------------------------------------------------------------------------
static void scput_Fight_Others( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
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
 * [Event] �����ރ��U�����R�[��
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   targets
 *
 * @retval  BOOL      �����������U�n���h���������TRUE
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
// �X�L���X���b�v
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
 * �n���h�������X�^�b�N�������[�g
 *
 * @param   wk
 * @param   useItemID   �A�C�e���g�p�ɂ��Ăяo���̏ꍇ�A���̃A�C�e��ID�^����ȊO�AITEM_DUMMY_DATA
 */
//----------------------------------------------------------------------------------
static void scproc_HandEx_Root( BTL_SVFLOW_WORK* wk, u16 useItemID )
{
  BTL_HANDEX_PARAM_HEADER* handEx_header;

  while( (handEx_header = Hem_ReadWork(&wk->HEManager)) != NULL )
  {
    BTL_Printf("ProcHandEX : ADRS=0x%p, type=%d\n", handEx_header, handEx_header->equip);
    switch( handEx_header->equip ){
    case BTL_HANDEX_TOKWIN_IN:     scproc_HandEx_TokWinIn( wk, handEx_header ); break;
    case BTL_HANDEX_TOKWIN_OUT:    scproc_HandEx_TokWinOut( wk, handEx_header ); break;
    case BTL_HANDEX_USE_ITEM:      scproc_HandEx_useItem( wk, handEx_header ); break;
    case BTL_HANDEX_RECOVER_HP:    scproc_HandEx_recoverHP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_RECOVER_PP:    scproc_HandEx_recoverPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_DECREMENT_PP:  scproc_HandEx_decrementPP( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_CURE_SICK:     scproc_HandEx_cureSick( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_ADD_SICK:      scproc_HandEx_addSick( wk, handEx_header ); break;
    case BTL_HANDEX_RANK_EFFECT:   scproc_HandEx_rankEffect( wk, handEx_header, useItemID ); break;
    case BTL_HANDEX_RECOVER_RANK:  scproc_HandEx_recoverRank( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_RANK:    scproc_HandEx_resetRank( wk, handEx_header ); break;
    case BTL_HANDEX_DAMAGE:        scproc_HandEx_damage( wk, handEx_header ); break;
    case BTL_HANDEX_KILL:          scproc_HandEx_kill( wk, handEx_header ); break;
    case BTL_HANDEX_CHANGE_TYPE:   scproc_HandEx_changeType( wk, handEx_header ); break;
    case BTL_HANDEX_MESSAGE_SET:   scproc_HandEx_messageSet( wk, handEx_header ); break;
    case BTL_HANDEX_MESSAGE_STD:   scproc_HandEx_messageStd( wk, handEx_header ); break;
    case BTL_HANDEX_SET_TURNFLAG:  scproc_HandEx_setTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_TURNFLAG:scproc_HandEx_resetTurnFlag( wk, handEx_header ); break;
    case BTL_HANDEX_SET_CONTFLAG:  scproc_HandEx_setContFlag( wk, handEx_header ); break;
    case BTL_HANDEX_RESET_CONTFLAG:scproc_HandEx_resetContFlag( wk, handEx_header ); break;
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
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
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
// ��Ԉُ퐬�����̃��b�Z�[�W�\���R�[���o�b�N
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
static void scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DAMAGE* param = (BTL_HANDEX_PARAM_DAMAGE*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BTL_POKEPARAM* pp_target;
  u32 i;

  for(i=0; i<param->poke_cnt; ++i){
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    scproc_SimpleDamage( wk, pp_target, param->damage[i], STRID_NULL );
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
//  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
//  scPut_Message_SetEx( wk, bpp, param->arg_cnt, param->args );
  SCQUE_PUT_MSG_STD( wk->que, param->strID );
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

//==============================================================================
// �T�[�o�[�t���[�F�^�[���`�F�b�N���[�g
//==============================================================================
static void scproc_TurnCheck( BTL_SVFLOW_WORK* wk )
{
  scproc_turncheck_sub( wk, BTL_EVENT_TURNCHECK_BEGIN );
  scproc_turncheck_sick( wk );
  scproc_turncheck_weather( wk );
  scproc_turncheck_sub( wk, BTL_EVENT_TURNCHECK_END );

  // �S�|�P�����̃^�[���t���O���N���A
  {
    FRONT_POKE_SEEK_WORK  fps;
    BTL_POKEPARAM* bpp;

    FRONT_POKE_SEEK_InitWork( &fps, wk );
    while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) ){
      BTL_POKEPARAM_ClearTurnFlag( bpp );
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
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > ��Ԉُ�
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
          BTL_Printf("�|�P[%d]����Ԉُ�(%d)�ŁA%d�̃_���[�W����\n", pokeID, sick, damage );
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
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �V��
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
    // �_���[�W���f�R�}���h���s�̑O��ɁA�Ƃ������ɂ��P�[�X�̓E�B���h�E�\���R�}���h
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
      // ���S�`�F�b�N
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
//  ���l�֘A�֐��Q
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
//  �C�x���g���[�N�m��
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
      GF_ASSERT(0); // �Ō�Ɋm�ۂ��ꂽ���[�N�ȊO�̗̈悪��ɉ������悤�Ƃ��Ă���
    }
  }
}
//---------------------------------------------------------------------------------------------
//
// �R�}���h�o�͑w
// �T�[�o�[�R�}���h���o�b�t�@�ɐ��� �� �K�v�Ȃ炻��ɔ����|�P�����p�����[�^������s��
// �C�x���g�w�̊֐��Ăяo���͍s���Ȃ�
//
//---------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * [Put] �u�����́@�͂�ǂ��ł������Ȃ��v�\��
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
 * �u�����͂����񂵂Ă���v�\��
 */
//--------------------------------------------------------------------------
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAct, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * ������ɂ�鎩���_���[�W����
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
 * ���U�o�����莞�̃|�P�����n��Ԉُ�񕜏���
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
 * ���U���b�Z�[�W�\���i�����́~�~���������I�Ȃǁj
 */
//--------------------------------------------------------------------------
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_WAZA( wk->que, BTL_POKEPARAM_GetID(attacker), waza );
}
//----------------------------------------------------------------------------------
/**
 * [Put]��Ԉُ�񕜏����̃R�}���h����
 *
 * @param   wk
 * @param   bpp       �Ώۃ|�P����
 * @param   sick_id   ��Ԉُ�R�[�h
 * @param   itemID  �A�C�e���g�p�ɂ��񕜂Ȃ�A�C�e��ID�^����ȊO�� ITEM_DUMMY_DATA
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
 * [Put]��Ԉُ�񕜏����̕W�����b�Z�[�W�o�̓R�}���h����
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
 * �g����Ԉُ�R�[�h���A�Y������R�[�h�ɕϊ�����
 *
 * @param   bpp       �Ώۃ|�P����
 * @param   exCode
 *
 * @retval  WazaSick  �Y���R�[�h�^�Y���R�[�h�Ȃ��̏ꍇ�AWAZASICK_NULL��Ԃ�
 */
//----------------------------------------------------------------------------------
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx exCode )
{
  WazaSick result = WAZASICK_NULL;
  PokeSick poke_sick = BTL_POKEPARAM_GetPokeSick( bpp );

  // �g���R�[�h�ϊ�
  if( (exCode == WAZASICK_EX_POKEFULL) || (exCode == WAZASICK_EX_POKEFULL_PLUS) )
  {
    // �|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ炻�������
    if( poke_sick != WAZASICK_NULL ){
      result = poke_sick;
    }
    // ����ȊO�͍����̂ݎ���
    else{
      if( (exCode == WAZASICK_EX_POKEFULL_PLUS)
      &&  (BTL_POKEPARAM_CheckSick(bpp, WAZASICK_KONRAN))
      ){
        result = WAZASICK_KONRAN;
      }
    }
  }
  // �ʏ�R�[�h���͜늳���Ă��Ȃ��|�P�n��Ԉُ���w�肳�ꂽ��NULL��Ԃ�
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
 * ��Ԉُ�񕜎��̃��b�Z�[�WID�擾
 *
 * @param   sick        ��Ԉُ�R�[�h
 * @param   fUseItem    ����̎g�p�ɂ��񕜂Ȃ�TRUE
 *
 * @retval  int   ���b�Z�[�WID�i�Y�����b�Z�[�W�����݂��Ȃ��ꍇ -1�j
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
 * ���U�o�����s���b�Z�[�W�\��
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
 * �A�����U�����J�E���^���N���A
 */
//--------------------------------------------------------------------------
static void scPut_ResetSameWazaCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BTL_POKEPARAM_ResetUsedWazaNumber( bpp );
  SCQUE_PUT_OP_ResetUsedWazaCounter( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * �u���������܂����܂�Ȃ������v�\��
 */
//--------------------------------------------------------------------------
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
}
//--------------------------------------------------------------------------
/**
 * �u�����ɂ͓�����Ȃ������v�\��
 */
//--------------------------------------------------------------------------
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, BTL_POKEPARAM_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * �u�����ɂ͂��������Ȃ��悤���v�\��
 */
//--------------------------------------------------------------------------
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BTL_POKEPARAM_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * [Put] �_���[�W��->�񕜉�����
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
 * [Put] �Ώۃ|�P����������ł����玀�S�������K�v�R�}���h����
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
 * [Put] ���U�G�t�F�N�g����
 */
//--------------------------------------------------------------------------
static void scPut_WazaEffectOn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza )
{
  // �P�̃��U�������ƂɂP�񂵂����Ȃ�
  if( FlowFlg_Get(wk, FLOWFLG_SET_WAZAEFFECT ) == FALSE )
  {
    u8 atPokeID = BTL_POKEPARAM_GetID( attacker );
    u8 defPokeID = BTL_POKEPARAM_GetID( target );
    BTL_Printf("���U�G�t�F�N�g�R�}���h�������܂���, ���U�i���o%d\n", waza);
    SCQUE_PUT_ACT_WazaEffect( wk->que, atPokeID, defPokeID, waza );
    FlowFlg_Set( wk, FLOWFLG_SET_WAZAEFFECT );
  }
  else
  {
    BTL_Printf("���U�G�t�F�N�g�R�}���h�����ς݂Ȃ̂Ŗ���, ���U�i���o%d\n", waza);
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] ���U�ɂ��_���[�W�i�P�́j
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage,
  BOOL critical_flag, BOOL plural_hit_flag )
{
  u8 pokeID = BTL_POKEPARAM_GetID( defender );

  // ������q�b�g�͍Ō�ɂP�񂾂��u���ʂ́������I�v���o�����߁A�����ł͖�����
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
 * [Put] ���U�ɂ��_���[�W�i�����́j
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
 * [Put] ���U�_���[�W���u���炦���v���̃��b�Z�[�W�\��
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
 * [Put] ���U�ɂ��_���[�W��^������̔����v�Z
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
 * [Put] �Ƃ������E�B���h�E�\���J�n
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_IN( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Ƃ������E�B���h�E�����J�n
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_OUT( wk->que, BTL_POKEPARAM_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] ����ȏ�A�\�̓����N��������Ȃ��i������Ȃ��j���b�Z�[�W
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
 * [Put] �����N�������ʂ�����
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
 * [Put] �����N�������ʎ��s���b�Z�[�W
 */
//--------------------------------------------------------------------------
static void scPut_RankEffectFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, const BTL_EVWK_CHECK_RANKEFF* evwk )
{
  u8 pokeID = BTL_POKEPARAM_GetID( target );
  u16 strID = BTL_STRID_SET_RankdownFail;

  if( effect == evwk->failSpecificType )
  {
    switch( effect ){
    case WAZA_RANKEFF_ATTACK: strID = BTL_STRID_SET_RankdownFail_ATK; break;
    case WAZA_RANKEFF_HIT:    strID = BTL_STRID_SET_RankdownFail_HIT; break;
    }
  }
  BTL_Printf("�����N���ʎ��s���b�Z�[�W=%d\n", strID);
  BTL_SERVER_RECTPT_SetMessage( wk, strID, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �ꌂ�K�E���U����
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
 * [Put] �V���v��HP�����G�t�F�N�g
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
 * [Put] �V���v��HP�����G�t�F�N�g
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
 * [Put] ��Ԉُ퉻
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
 * [Put] �Z�b�g���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID )
{
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );
  SCQUE_PUT_MSG_SET( wk->que, strID, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Z�b�g���b�Z�[�W�\���i�����g���j
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
 * [Put] �g�������U��PP�f�N�������g
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
 * [Put] PP��
 *
 * @param   wk
 * @param   bpp
 * @param   wazaIdx
 * @param   volume
 * @param   itemID    �A�C�e�����g�������ʂ̏ꍇ�̓A�C�e��ID
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
 * [Put] ��Ԉُ탁�b�Z�[�W�Z�b�g
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
  case WAZASICK_DOKU:   msgID = BTL_STRID_SET_DokuGet; break;
  case WAZASICK_YAKEDO: msgID = BTL_STRID_SET_YakedoGet; break;
  case WAZASICK_MAHI:   msgID = BTL_STRID_SET_MahiGet; break;
  case WAZASICK_KOORI:  msgID = BTL_STRID_SET_KoriGet; break;
  case WAZASICK_NEMURI: msgID = BTL_STRID_SET_NemuriGet; break;
  case WAZASICK_KONRAN: msgID = BTL_STRID_SET_KonranGet; break;
  case WAZASICK_AKUMU:  msgID = BTL_STRID_SET_AkumuGet;  break;
  default:
    return;
  }

  SCQUE_PUT_MSG_SET( wk->que, msgID, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Put]�A�N�V�������t���O�Z�b�g
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
 * [Put]�A�N�V�������t���O�N���A
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
 * [Put] �p���t���O�Z�b�g
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
 * [Put] �p���t���O�N���A
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

//---------------------------------------------------------------------------------------------
//
// �y�C�x���g�w�z
//
// ���C�������w�ł̔���ɕK�v�Ȋe������̌�����s���B
// �n���h���ނ̌Ăяo���̓C�x���g�w����̂ݍs����B
// �|�P�����p�����[�^�̑���͍s��Ȃ��B
// �R�}���h�o�͍͂s��Ȃ��B
//
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * [Event] �g���������h��I�񂾂Ƃ��̓���D��x�𔻒�
 *
 * @param   wk
 * @param   attacker
 * @param   spePriA     [out] ����D��xA���󂯎��i���U��荂�����背�x���j
 * @param   spePriB     [out] ����D��xB���󂯎��i���U���Ⴂ���背�x���j
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
 * [Event] ���΂₳�擾
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
  BTL_Printf("�f�̑f������\������ poke=%d, agi=%d\n", BTL_POKEPARAM_GetID(attacker), agi);
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE ); // �܂Ѓ`�F�b�N�t���O�Ƃ��Ďg���Ă���
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_AGILITY );
    agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    if( BTL_POKEPARAM_GetPokeSick(attacker) == POKESICK_MAHI )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
      {
        agi = (agi * BTL_MAHI_AGILITY_RATIO) / 100;
      }
    }
  BTL_EVENTVAR_Pop();
  BTL_Printf("�Ԃ��Ă����f������ %d ������\n", agi);
  return agi;
}
//--------------------------------------------------------------------------
/**
 * [Event] ������_���[�W�v�Z
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16   �_���[�W�l
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
 * �yEvent�z���U�o�����s���� �� �˂ނ�E�܂ЁE������̉񕜃`�F�b�N
 *
 * @param   wk
 * @param   attacker    �U���|�P�����p�����[�^
 * @param   waza        �o�����Ƃ��Ă��郏�U
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

    // ��Ԉُ�ɂ�鎸�s�`�F�b�N
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

    // �Ђ�݂ɂ�鎸�s�`�F�b�N
    if( BTL_POKEPARAM_GetTurnFlag(attacker, BPP_TURNFLG_SHRINK) ){
      cause = SV_WAZAFAIL_SHRINK;
      break;
    }
    // ���U���b�N�ɂ�鎸�s�`�F�b�N
    if( (BTL_POKEPARAM_CheckSick(attacker, WAZASICK_WAZALOCK)
       ||  BTL_POKEPARAM_CheckSick(attacker, WAZASICK_WAZALOCK_HARD) )
    &&  (BTL_POKEPARAM_GetPrevWazaNumber(attacker) != waza)
    ){
      cause = SV_WAZAFAIL_WAZALOCK;
      break;
    }
    // ���傤�͂ɂ�鎸�s�`�F�b�N
    if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_TYOUHATSU)
    &&  !WAZADATA_IsDamage(waza)
    ){
      cause = SV_WAZAFAIL_TYOUHATSU;
      break;
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
 * [Event] �������郏�U�̃p�����[�^���\���̂ɃZ�b�g
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
    param->wazaType = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    param->userType = BTL_EVENTVAR_GetValue( BTL_EVAR_USER_TYPE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�������s���̔����`�F�b�N
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
 * [Event] ���U�o�������m��
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
 * [Event] ���U�͏o���������ʂ���������
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
 * [Event] ���߃^�[���X�L�b�v����
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL    ���߃^�[���X�L�b�v����ꍇTRUE
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
 * [Event] ���߃^�[�������m��
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
 * [Event] ���߃^�[�������m��
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
 * [Event] �o�������U���Ώۂɓ����邩����i�ꌂ�K�E�ȊO�̃|�P�����Ώۃ��U�j
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
 * [Event] ������ƂԁA�_�C�r���O���A�ꂩ��B��Ă���|�P�����ւ̃q�b�g�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL    �B��Ă��邽�߃q�b�g���Ȃ��ꍇ��TRUE
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
          BTL_Printf("�|�P[%d], ���[%d] �Ȃ̂Ń��U������܂���\n", BTL_POKEPARAM_GetID(defender), hideState[i]);
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
 * [Event] ���U�̖����`�F�b�N���A�m���v�Z���X�L�b�v���Ė��������邩�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL        �m���v�Z���X�L�b�v����ꍇ��TRUE
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
 * [Event] ���U�I����(0-100)�擾
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 *
 * @retval  u8    �I���� (0-100)
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
 * [Event] �o�������U���Ώۂɓ����邩����i�ꌂ�K�E���U�j
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
      // �Ƃ��������ɂ�鋭�����������󂯂Ȃ��ꍇ�A�K���L���B
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
 * [Event] ���U�����`�F�b�N�i�^�C�v�����ɂ�閳�����j
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 *
 * @retval  BOOL    �����ȏꍇTRUE
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
 * [Event] ���U�����`�F�b�N�i�Ƃ��������A�ʃP�[�X�ɂ�閳�����j
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 *
 * @retval  BOOL    �����ȏꍇTRUE
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
 * [Event] ����^�C�v�̃_���[�W���U�ŉ񕜂��Ă��܂��|�P�̔���
 *
 * @param   wk
 * @param   defender
 * @param   waza
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza, BTL_EVWK_DMG_TO_RECOVER* evwk )
{
  BOOL ret = FALSE;

  evwk->pokeID = BTL_POKEPARAM_GetID( defender );
  evwk->recoverHP = 0;
  evwk->tokFlag = FALSE;
  evwk->rankEffect = WAZA_RANKEFF_NULL;
  evwk->rankVolume = 0;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, evwk->pokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, ret );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER );
    ret = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return ret;
}
//--------------------------------------------------------------------------
/**
 * [Event] �N���e�B�J���q�b�g�����`�F�b�N
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
 * [Event] ���U�ɂ��_���[�W���u���炦��v���ǂ����`�F�b�N
 * ���u���炦��v= HP��1�c��
 *
 * @param   wk
 * @param   defender    ���U�_���[�W�����炤�|�P
 * @param   damage      [io] �_���[�W�ʁi���炦���ꍇ�AHP���P�c��悤�ɕ␳�����j
 *
 * @retval  BppKoraeruCause   �u���炦��v���R�^���炦�Ȃ��ꍇ�� BPP_KORAE_NONE
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
 * [Event] ���U�_���[�W�󂯂���̔�������
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
 * [Event] ���U�̑����v�Z
 *
 * @param   wk
 * @param   defender
 * @param   waza_type
 * @param   evwk        [out] �v�Z����
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
 * [Event] ���U�g�p��̃f�N�������g����PP�l���擾
 *
 * @param   wk
 * @param   attacker  ���U�g�p�|�P����
 * @param   wazaIdx   �g�p���U�C���f�b�N�X
 * @param   rec       ���U�ɂ���ă_���[�W���󂯂��|�P�����p�����[�^�Q
 *
 * @retval  u32       �f�N�������gPP�l
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
 * [Event] ���U�g�p��APP���f�N�������g������̔���
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    �A�C�e���g�p�����������TRUE
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
 * [Event] ������q�b�g���U�̃`�F�b�N����
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   hitCount    [out] ���s����q�b�g�񐔁i�����_���j
 *
 * @retval  BOOL    ������q�b�g���郏�U�Ȃ�TRUE
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
 * [Event] ���U�З͎擾
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   wazaParam
 *
 * @retval  u16   ���U�З͒l
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
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.0), FX32_CONST(0.1f), FX32_CONST(128) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER );
    power = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER_RATIO );
  BTL_EVENTVAR_Pop();

  power = ( power * ratio ) >> FX32_SHIFT;
  BTL_Printf("�З͂�%d���[��\n", power);
  return power;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�ȊO�̃_���[�W�L���^�����`�F�b�N
 *
 * @param   wk
 * @param   bpp
 * @param   damage  �_���[�W��
 *
 * @retval  BOOL    �L���Ȃ�TRUE
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
 * [Event] ���U�ȊO�̃_���[�W��A�����`�F�b�N�i����A�A�C�e���g�p�̂݁j
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �A�C�e���g�p����ꍇ�ATRUE
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



// �U�����\�͒l�擾
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

// �h�䑤�\�͒l�擾
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

      // �Ă񂱂��u���Ȃ��炵�v�̎��A����^�C�v�̂Ƃ��ڂ�1.5�{
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
 * [Event] �U���|�P�ƃ��U�̃^�C�v��v�␳�����v�Z
 *
 * @param   wk
 * @param   attacker
 * @param   waza_type
 *
 * @retval  fx32    �␳��
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
// �h�䑤�^�C�v�擾
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

// �����o�[�o�g���ꂩ��ޏo
static void scEvent_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, posIdx );
  u8 pokeID = BTL_POKEPARAM_GetID( bpp );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_OUT );
    // �����Ŏ��ʂ��Ƃ�����
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
 * [Event] �����o�[����ɓo�ꂳ����
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
 * [Event] ���U�ǉ����ʂɂ��Ώۃ|�P�������Ђ�܂��邩����
 *
 * @param   wk
 * @param   target    �Ώۃ|�P�����f�[�^
 * @param   attacker  �U���|�P�����f�[�^
 * @param   waza      �g�������U
 *
 * @retval  BOOL    �Ђ�܂���ꍇTRUE
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
 * �yEvent�z���U�ǉ����ʂɂ���Ԉُ�̔����`�F�b�N
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   defender
 * @param   [out] pSickCont   �p������
 *
 * @retval  WazaSick          ��Ԉُ�ID
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
 * �_���[�W���U������
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
 * �yEvent�z����|�P�������|�P�����n�̏�Ԉُ�ɂ���
 *
 * @param   wk
 * @param   receiver    ��Ԉُ���󂯂�Ώۂ̃|�P�����p�����[�^
 * @param   attacker    ��Ԉُ��^���鑤�̃|�P�����p�����[�^
 * @param   sick        ��Ԉُ�ID
 * @param   fAlmost     �قڊm��t���O�i���j
 *
 * ���قڊm��t���O�Ƃ�
 *  �v���C���[���قڊm���ɏ�Ԉُ�ɂȂ�͂��A�Ǝv���󋵂�TRUE�ɂ���B
 *  ���U���ڌ��ʂȂ���TRUE�B�ǉ����ʂȂ���ʊm��100�̎���TRUE�B�Ƃ��������ʂ̏ꍇ�͏��FALSE�B
 *  ���ꂪTRUE�̏ꍇ�A��Ԉُ�𖳌�������Ƃ����������������ꍇ�ɂ��̂��Ƃ�\�����邽�߂Ɏg���B
 */
//--------------------------------------------------------------------------
static void scEvent_MakePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
  PokeSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
  // �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
  if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI )
  ){
    return;
  }
  // ���łɃ|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ�A�V���Ƀ|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
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
 * �yEvent�z����|�P���������U�n�̏�Ԉُ�ɂ���
 *
 * @param   wk
 * @param   receiver    ��Ԉُ���󂯂�Ώۂ̃|�P�����p�����[�^
 * @param   attacker    ��Ԉُ��^���鑤�̃|�P�����p�����[�^
 * @param   sick        ��Ԉُ�ID
 * @param   fAlmost     �قڊm��t���O�i���j
 *
 * ���قڊm��t���O�Ƃ�
 *  �v���C���[���قڊm���ɏ�Ԉُ�ɂȂ�͂��A�Ǝv���󋵂�TRUE�ɂ���B
 *  ���U���ڌ��ʂȂ���TRUE�B�ǉ����ʂȂ���ʊm��100�̎���TRUE�B�Ƃ��������ʂ̏ꍇ�͏��FALSE�B
 *  ���ꂪTRUE�̏ꍇ�A��Ԉُ�𖳌�������Ƃ����������������ꍇ�ɂ��̂��Ƃ�\�����邽�߂Ɏg���B
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
 * �yEvent�z���U�ɂ��ǉ������N���ʂ̔����`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   waza
 *
 * @retval  BOOL    ����������TRUE
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
 * [Event] ���U�ɂ��i���ځE�ǉ��j�����N�������ʂ��擾
 *
 * @param   wk
 * @param   waza
 * @param   idx
 * @param   attacker
 * @param   target
 * @param   effect    [out] ���ʎ��
 * @param   volume    [out] ���ʒl
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
 * [Event] �����N�������ʂ̍ŏI���ۃ`�F�b�N
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   evwk      [out]
 *
 * @retval  BOOL      ��������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, BTL_EVWK_CHECK_RANKEFF* evwk )
{
  evwk->failFlag = FALSE;
  evwk->failTokuseiFlag = FALSE;
  evwk->failSpecificType = WAZA_RANKEFF_NULL;
  evwk->useItemFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_LAST_CHECK );
  BTL_EVENTVAR_Pop();

  return evwk->failFlag == FALSE;
}
//--------------------------------------------------------------------------
/**
 * [Event] �h���C���U���̃h���C��HP�ʂ��v�Z
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 * @param   target
 * @param   damage    �^�����_���[�W
 *
 * @retval  int     �񕜗�
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
 * [Event] ����h���C��
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
 * �yEvent�z�V��̕ω��`�F�b�N
 *
 * @param   wk
 * @param   weather �V��
 * @param   turn    [io]�p���^�[����
 *
 * @retval  BOOL    �ω�����ꍇ��TRUE
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
 * �yEvent�z�V��̕ω���C�x���g
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
 * [Event] �A�C�e���g�p�ۃ`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    TRUE�Ŏg�p�\
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
 * [Event] ���U�ɂ��HP�񕜗ʌv�Z
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
// �ȉ��A�n���h������̉�����M�֐��ƃ��[�e�B���e�B�Q
//----------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
/**
 * �w��|�PID�����|�P�������퓬�ɏo�Ă��邩�`�F�b�N���A�o�Ă����炻�̐퓬�ʒu��Ԃ�
 *
 * @param   server
 * @param   pokeID
 *
 * @retval  BtlPokePos    �o�Ă���ꍇ�͐퓬�ʒuID�^�o�Ă��Ȃ��ꍇ��BTL_POS_MAX
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
 * ��ɏo�Ă���S�|�P������ID��z��Ɋi�[����
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    ��ɏo�Ă���|�P������
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
 * ��ɏo�Ă���S�Ă̑��葤�|�P����ID��z��Ɋi�[����
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    ��ɏo�Ă���|�P������
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
 * �w��̂Ƃ����������|�P�������퓬�ɏo�Ă��邩�`�F�b�N
 *
 * @param   wk
 * @param   tokusei
 *
 * @retval  BOOL    �o�Ă�����TRUE
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
 * �w��ID�̃|�P�����p�����[�^��Ԃ�
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
 * ���݂̌o�߃^�[������Ԃ�
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
 * [�n���h����M] �Ƃ������E�B���h�E�\���C��
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
 * [�n���h����M] �Ƃ������E�B���h�E�\���A�E�g
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
 * �Y���ʒu�ɂ��鐶�����Ă���|�P����ID��z��Ɋi�[������Ԃ�
 *
 * @param   wk
 * @param   exPos
 * @param   dst_pokeID    [out]�i�[��z��
 *
 * @retval  u8    �i�[������
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
 * [�n���h����M] �X�e�[�^�X�̃����N�_�E������
 *
 * @param   wk
 * @param   exPos         �Ώۃ|�P�����ʒu
 * @param   statusType    �X�e�[�^�X�^�C�v
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
 * [�n���h����M] �X�e�[�^�X�̃����N�A�b�v����
 *
 * @param   wk
 * @param   exPos         �Ώۃ|�P�����ʒu
 * @param   statusType    �X�e�[�^�X�^�C�v
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
  BTL_Printf("�����N�������ʁF�^�C�v=%d,  �Ώۃ|�P������=%d\n", statusType, numPokemons );
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
 * [�n���h����M] �V���v��HP��������
 *
 * @param   wk
 * @param             �Ώۃ|�P�����ʒu
 * @param   statusType    �X�e�[�^�X�^�C�v
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
 * [�n���h����M] ��Ԉُ������킷����
 *
 * @param   wk
 * @param   targetPokeID  ���炤����̃|�P����ID
 * @param   attackPokeID  ����킹�鑤�́i�Ƃ����������Ȃǁj�|�P����ID
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
 * [�n���h����M] �|�P�����n��Ԉُ�̉񕜏���
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
 * [�n���h����M] ���U�n��Ԉُ�̉񕜏���
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
 * [�n���h����M] �V��̕ω�����
 *
 * @param   wk
 * @param   weather   �ω���̓V��w��^�V����t���b�g�ɂ������ꍇ�� BTL_WEATHER_NONE
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
 * [�n���h����M] ���������֎~�R�[�h�̒ǉ���S�N���C�A���g�ɒʒm
 *
 * @param   wk
 * @param   clientID      �֎~�R�[�h�𔭍s�����|�P����ID
 * @param   code          �֎~�R�[�h
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_CantEscapeAdd( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code )
{
  SCQUE_PUT_OP_CantEscape_Add( wk->que, pokeID, code );
}
//=============================================================================================
/**
 * [�n���h����M] ���������֎~�R�[�h�̍폜��S�N���C�A���g�ɒʒm
 *
 * @param   wk
 * @param   clientID      �֎~�R�[�h�𔭍s�����|�P����ID
 * @param   code          �֎~�R�[�h
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_CantEscapeSub( BTL_SVFLOW_WORK* wk, u8 pokeID, BtlCantEscapeCode code )
{
  SCQUE_PUT_OP_CantEscape_Sub( wk->que, pokeID, code );
}
//=============================================================================================
/**
 * [�n���h����M] �Ƃ������u�g���[�X�v����
 *
 * @param   wk
 * @param   pokeID        �g���[�X�g�����|�P����ID
 * @param   targetPokeID  �g���[�X����鑤�|�P����ID
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_TraceTokusei( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 targetPokeID )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  BTL_POKEPARAM* bppTgt = BTL_POKECON_GetPokeParam( wk->pokeCon, targetPokeID );
  PokeTokusei tok = BTL_POKEPARAM_GetValue( bppTgt, BPP_TOKUSEI );

  BTL_Printf("�g���[�X�ŏ���������Ƃ�����=%d\n", tok);
  BTL_POKEPARAM_ChangeTokusei( bpp, tok );
  SCQUE_PUT_ACT_TokTrace( wk->que, pokeID, targetPokeID, tok );
  BTL_HANDLER_TOKUSEI_Add( bpp );
}

//=============================================================================================
/**
 * �n���h���g�p�q�[�vID�擾
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
    { BTL_HANDEX_USE_ITEM,       sizeof(BTL_HANDEX_PARAM_USE_ITEM)     },
    { BTL_HANDEX_RECOVER_HP,     sizeof(BTL_HANDEX_PARAM_RECOVER_HP)   },
    { BTL_HANDEX_RECOVER_PP,     sizeof(BTL_HANDEX_PARAM_PP)           },
    { BTL_HANDEX_DECREMENT_PP,   sizeof(BTL_HANDEX_PARAM_PP)           },
    { BTL_HANDEX_CURE_SICK,      sizeof(BTL_HANDEX_PARAM_CURE_SICK)    },
    { BTL_HANDEX_ADD_SICK,       sizeof(BTL_HANDEX_PARAM_ADD_SICK)     },
    { BTL_HANDEX_RANK_EFFECT,    sizeof(BTL_HANDEX_PARAM_RANK_EFFECT)  },
    { BTL_HANDEX_RECOVER_RANK,   sizeof(BTL_HANDEX_PARAM_RECOVER_RANK) },
    { BTL_HANDEX_RESET_RANK,     sizeof(BTL_HANDEX_PARAM_RESET_RANK)   },
    { BTL_HANDEX_DAMAGE,         sizeof(BTL_HANDEX_PARAM_DAMAGE)       },
    { BTL_HANDEX_KILL,           sizeof(BTL_HANDEX_PARAM_KILL)         },
    { BTL_HANDEX_CHANGE_TYPE,    sizeof(BTL_HANDEX_PARAM_CHANGE_TYPE)  },
    { BTL_HANDEX_MESSAGE_STD,    sizeof(BTL_HANDEX_PARAM_MESSAGE)      },
    { BTL_HANDEX_MESSAGE_SET,    sizeof(BTL_HANDEX_PARAM_MESSAGE)      },
    { BTL_HANDEX_TOKWIN_IN,      sizeof(BTL_HANDEX_PARAM_HEADER)       },
    { BTL_HANDEX_TOKWIN_OUT,     sizeof(BTL_HANDEX_PARAM_HEADER)       },
    { BTL_HANDEX_SET_TURNFLAG,   sizeof(BTL_HANDEX_PARAM_TURNFLAG)     },
    { BTL_HANDEX_RESET_TURNFLAG, sizeof(BTL_HANDEX_PARAM_TURNFLAG)     },
    { BTL_HANDEX_SET_CONTFLAG,   sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG) },
    { BTL_HANDEX_RESET_CONTFLAG, sizeof(BTL_HANDEX_PARAM_SET_CONTFLAG) },
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

