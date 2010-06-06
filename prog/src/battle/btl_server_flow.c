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
#include "item\itemtype_def.h"
#include "poke_tool\shinka_check.h"
#include "gamesystem\g_power.h"

#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_sick.h"
#include "btl_pospoke_state.h"
#include "btl_pokeset.h"
#include "btl_shooter.h"
#include "btl_calc.h"
#include "btl_tables.h"
#include "btlv\btlv_effect.h"

#include "handler\hand_tokusei.h"
#include "handler\hand_item.h"
#include "handler\hand_waza.h"
#include "handler\hand_side.h"
#include "handler\hand_pos.h"

#include "btl_server_local.h"
#include "btl_server.h"
#include "btl_server_flow_sub.h"
#include "btl_hem.h"

#include "btl_server_flow.h"
#include "btl_server_flow_local.h"



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void clearWorks( BTL_SVFLOW_WORK* wk );
static void clearPokeCantActionFlag( BTL_SVFLOW_WORK* wk );
static u32 ActOrderProc_Main( BTL_SVFLOW_WORK* wk, u32 startOrderIdx );
static BOOL ActOrderProc_OnlyPokeIn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );
static BOOL scproc_CheckShowdown( BTL_SVFLOW_WORK* wk );
static BOOL CheckPlayerSideAlive( BTL_SVFLOW_WORK* wk );
static void scproc_countup_shooter_energy( BTL_SVFLOW_WORK* wk );
static BOOL reqChangePokeForServer( BTL_SVFLOW_WORK* wk, CLIENTID_REC* rec );
static void scproc_BeforeFirstFight( BTL_SVFLOW_WORK* wk );
static BOOL scproc_CheckFloating( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckFloating( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static inline u32 ActPri_Make( u8 actPri, u8 wazaPri, u8 spPri, u16 agility );
static inline u8 ActPri_GetWazaPri( u32 priValue );
static inline u8 ActPri_GetSpPri( u32 priValue );
static inline u32 ActPri_SetSpPri( u32 priValue, u8 spPri );
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction, ACTION_ORDER_WORK* order, u32 orderMax );
static void sortActionSub( ACTION_ORDER_WORK* order, u32 numOrder );
static u8 scEvent_GetWazaPriority( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp );
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BOOL fTrickRoomEnable );
static inline WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk );
static ACTION_ORDER_WORK* ActOrderTool_SearchByPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID );
static ACTION_ORDER_WORK* ActOrderTool_SearchByWazaID( BTL_SVFLOW_WORK* wk, WazaID waza, u8 startIndex );
static ACTION_ORDER_WORK* ActOrderTool_SearchForCombiWaza( BTL_SVFLOW_WORK* wk, WazaID waza, u8 pokeID, BtlPokePos targetPos );
static u8 ActOrderTool_GetIndex( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder );
static int ActOrderTool_Intr( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder, u32 intrIndex );
static void ActOrderTool_SendToLast( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder );
static void ActOrder_Proc( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* actOrder );
static void scproc_ActStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlAction actionCmd );
static void scEvent_ActProcStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlAction actionCmd );
static void scEvent_ActProcEnd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static SabotageType CheckSabotageType( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL ActOrder_IntrProc( BTL_SVFLOW_WORK* wk, u8 intrPokeID );
static BOOL ActOrder_IntrReserve( BTL_SVFLOW_WORK* wk, u8 intrPokeID );
static BOOL ActOrder_IntrReserveByWaza( BTL_SVFLOW_WORK* wk, WazaID waza );
static BOOL ActOrder_SendLast( BTL_SVFLOW_WORK* wk, u8 pokeID );
static void ActOrder_ForceDone( BTL_SVFLOW_WORK* wk, u8 pokeID );
static inline void FlowFlg_Set( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_Clear( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline BOOL FlowFlg_Get( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_ClearAll( BTL_SVFLOW_WORK* wk );
static void wazaEffCtrl_Init( WAZAEFF_CTRL* ctrl );
static void wazaEffCtrl_Setup( WAZAEFF_CTRL* ctrl, BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec );
static inline void wazaEffCtrl_SetEnable( WAZAEFF_CTRL* ctrl );
static inline BOOL wazaEffCtrl_IsEnable( const WAZAEFF_CTRL* ctrl );
static inline void wazaEffCtrl_SetEffectIndex( WAZAEFF_CTRL* ctrl, u8 index );
static inline BOOL IsBppExist( const BTL_POKEPARAM* bpp );
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk );
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk, BTL_POKEPARAM** bpp );
static void scproc_Move( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_MoveCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx1, u8 posIdx2, BOOL fActCmd );
static BOOL scproc_NigeruCmd_Root( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scproc_NigeruCmdSub( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_SkipNigeruCalc( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scproc_NigeruCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BOOL fForceNigeru );
static BOOL scEvent_SkipNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scEvent_NigeruExMessage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scproc_MemberInCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scproc_MemberInForChange( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fPutMsg );
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk );
static void scPut_MemberOutMessage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_MemberChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u8 nextPokeIdx );
static BOOL scproc_MemberOutForChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, BOOL fIntrDisable );
static void scproc_MemberOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u16 effectNo );
static void scEvent_MemberOutFixed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke );
static void scproc_Fight( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_ACTION_PARAM* action );
static void scproc_MagicCoat_Root( BTL_SVFLOW_WORK* wk, WazaID actWaza );
static void wazaRobParam_Init( WAZA_ROB_PARAM* param );
static void wazaRobParam_Add( WAZA_ROB_PARAM* param, u8 robberPokeID, u8 targetPokeID, BtlPokePos targetPos );
static void scproc_WazaRobRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID actWaza, BTL_POKESET* defaultTarget );
static void scEvent_CheckCombiWazaExe( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scproc_WazaExe_Decide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, BtlEventType evType );
static void scEvent_WazaExeDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, BtlEventType evType  );
static BOOL scproc_Fight_CheckCombiWazaReady( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos );
static BOOL scproc_Fight_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos );
static BOOL scEvent_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BtlPokePos targetPos );
static void scproc_Fight_DecideDelayWaza( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static void scEvent_DecideDelayWaza( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static void scproc_StartWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_StartWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scproc_EndWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_EndWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza );
static void scEvent_WazaReflect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza );
static BOOL scproc_Check_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* rec, WAZA_ROB_PARAM* robParam );
static BOOL scEvent_CheckWazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* targetRec, u8* robberPokeID, u8* robTargetPokeID );
static BOOL scproc_Fight_CheckReqWazaFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID orgWazaID );
static void scPut_ReqWazaEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos );
static void scproc_WazaExeRecordUpdate( BTL_SVFLOW_WORK* wk, WazaID waza );
static BOOL scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* targetRec );
static void scproc_MigawariExclude( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
    const BTL_POKEPARAM* attacker, BTL_POKESET* target, BOOL fDamage );
static BOOL scEvent_CheckMigawariExclude( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL fDamage );
static void scproc_WazaExe_Effective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scproc_WazaExe_NotEffective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scproc_WazaExe_Done( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
static void scEvent_WazaExeEnd_Common( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza, BtlEventType eventID );
static BOOL IsMustHit( const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void flowsub_checkWazaAffineNoEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets, BTL_DMGAFF_REC* affRec );
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static BOOL scproc_checkNoEffect_sub( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, BtlEventType eventID );
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BtlEventType eventID,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* customMsg, BOOL* fNoReaction );
static void flowsub_checkWazaAvoid( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static BOOL scEvent_SkipAvoidCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BOOL IsTripleFarPos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza );
static BOOL scEvent_CheckHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam );
static void scPut_WazaEffect( BTL_SVFLOW_WORK* wk, WazaID waza, WAZAEFF_CTRL* effCtrl, u32 que_reserve_pos );
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza );
static BOOL scproc_TameStartTurn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_POKESET* targetRec, WazaID waza );
static void scproc_TameLockClear( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static BOOL scproc_FreeFall_Start( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL* fFailMsgDisped );
static void scproc_FreeFall_CheckRelease( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BOOL fReleaseProc );
static BOOL scproc_Fight_CheckWazaExecuteFail_1st( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u8 fWazaLock );
static BOOL scproc_Fight_CheckWazaExecuteFail_2nd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BOOL fWazaLock );
static BOOL scEvent_ExeFailThrew( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlEventType eventID );
static BOOL scproc_Fight_CheckConf( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static BOOL scproc_Fight_CheckMeroMero( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static BOOL scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static void scproc_WazaExecuteFailed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, SV_WazaFailCause fail_cause );
static void scPut_WazaExecuteFailMsg( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
static void scPut_SetBppCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppCounter counterID, u8 value );
static void scproc_decrementPPUsedWaza( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID waza, u8 wazaIdx, BTL_POKESET* rec );
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  u8 wazaIdx, WazaID waza, BTL_POKESET* rec );
static BOOL scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume );
static void scPut_DecrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, u8 vol );
static void scproc_Fight_Damage_Root( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
   BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_DMGAFF_REC* affRec, BOOL fDelayAttack );
static void BTL_CALCDAMAGE_Set( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKESET* target,
  const SVFL_WAZAPARAM* wazaParam, const BTL_DMGAFF_REC* affRec, fx32 dmg_ratio, BTL_CALC_DAMAGE_REC* dmgRec );
static u32 BTL_CALCDAMAGE_GetCount( const BTL_CALC_DAMAGE_REC* dmgRec );
static u32 BTL_CALCDAMAGE_GetMigawariHitPoke( BTL_SVFLOW_WORK* wk, const BTL_CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, u8* criticalFlagAry );
static u32 BTL_CALCDAMAGE_GetAllHitPoke( BTL_SVFLOW_WORK* wk, const BTL_CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry );
static u32 BTL_CALCDAMAGE_GetRealHitPoke( BTL_SVFLOW_WORK* wk, const BTL_CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, u8* criticalFlagAry, u8* koraeCauseAry );
static u32 scproc_Fight_Damage_SingleCount( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_DMGAFF_REC* affRec, BOOL fDelayAttack );
static u32 scproc_Fight_Damage_PluralCount( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_DMGAFF_REC* affRec );
static BOOL HITCHECK_IsFirstTime( const HITCHECK_PARAM* param );
static BOOL HITCHECK_IsPluralHitWaza( const HITCHECK_PARAM* param );
static BOOL HITCHECK_CheckWazaEffectPuttable( HITCHECK_PARAM* param );
static void HITCHECK_SetPluralHitAffinity( HITCHECK_PARAM* param, BtlTypeAff affinity );
static u32 scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  BTL_POKESET* targets, const BTL_CALC_DAMAGE_REC* dmgRec, HITCHECK_PARAM* hitCheckParam, fx32 dmg_ratio, FLAG_SET flagSet );
static u32 scproc_Fight_damage_side_core( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_CALC_DAMAGE_REC* dmgRec,
    const SVFL_WAZAPARAM* wazaParam, HITCHECK_PARAM* hitCheckParam, fx32 dmg_ratio, FLAG_SET flagSet );
static void scproc_Fight_Damage_ToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* targets );
static void scPut_DamageAff( BTL_SVFLOW_WORK* wk, BtlTypeAff affinity );
static u32 MarumeDamage( const BTL_POKEPARAM* bpp, u32 damage );
static void scproc_Fight_Damage_Determine( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static void scEvent_WazaDamageDetermine( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BOOL fWazaDamage, u16* damage );
static void scproc_Koraeru( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void scEvent_KoraeruExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void scproc_PrevWazaDamage( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry );
static void scEvent_PrevWazaDamage( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry );
static void wazaDmgRec_Add( BTL_SVFLOW_WORK* wk, BtlPokePos atkPos, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, u16 damage );
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage, BOOL fMigawriHit );
static void scproc_WazaDamageSideAfter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, u32 damage );
static void scEvent_WazaDamageSideAfter( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u32 damage );
static void scproc_CheckItemReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlItemReaction reactionType );
static void scEvent_CheckItemReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlItemReaction reactionType );
static void scproc_Fight_DamageProcStart( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
static void scproc_Fight_DamageProcEnd( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets, u32 dmgTotal, BOOL fDelayAttack );
static void scproc_CheckShrink( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender );
static void scproc_Fight_Damage_KoriCure( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static BOOL scproc_AddShrinkCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, u32 per );
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_Damage_Drain( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
static BOOL scproc_DrainCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP );
static void scEvent_DamageProcStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
static void scEvent_DamageProcEnd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BTL_POKESET* targets,
  const SVFL_WAZAPARAM* wazaParam, BOOL fDelayAttack );
static BOOL scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag, BOOL fSimurationMode, u16* dstDamage );
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage );
static BOOL scproc_SimpleDamage_CheckEnable( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage );
static BOOL scproc_SimpleDamage_Core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, BTL_HANDEX_STR_PARAMS* str );
static BOOL scproc_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckItemEquipFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID );
static void scproc_ConsumeItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scEvent_ConsumeItem( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID );
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static WazaSick scEvent_CheckWazaAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont );
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static WazaSick scEvent_DecideSpecialWazaSick( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str );
static void scEvent_GetWazaSickAddStr( BTL_SVFLOW_WORK* wk, WazaSick sick,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str );
static void scEvent_WazaSickCont( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT* sickCont );
static BOOL scproc_AddSickRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, BOOL fDefaultMsgEnable );
static BOOL scproc_AddSickCheckFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BtlSickOverWriteMode overWriteMode, BOOL fDispFailResult );
static BtlAddSickFailCode addsick_check_fail_std( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT sickCont, BtlSickOverWriteMode overWriteMode );
static void scproc_AddSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fDefaultMsgEnable, const BTL_HANDEX_STR_PARAMS* exStr );
static BtlWeather scEvent_GetWeather( BTL_SVFLOW_WORK* wk );
static BOOL scEvent_WazaSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaSick sick  );
static void scEvent_AddSick_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick );
static void scEvent_PokeSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker,
  PokeSick sick, BPP_SICK_CONT sickCont );
static void scEvent_WazaSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick );
static void scEvent_IekiFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target );
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost );
static void scEvent_GetWazaRankEffectValue( BTL_SVFLOW_WORK* wk, WazaID waza, u32 idx,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaRankEffect* effect, int* volume );
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, u8 atkPokeID, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u8 wazaUsePokeID, u16 itemID, BOOL fAlmost, BOOL fStdMsg );
static int scEvent_RankValueChange( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect rankType,
  u8 wazaUsePokeID, u16 itemID, int volume );
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP, BOOL fDispFailMsg );
static BOOL scproc_RecoverHP_CheckFailBase( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scproc_RecoverHP_CheckFailSP( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fDispFailMsg );
static void scproc_RecoverHP_Core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP );
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scproc_Ichigeki_Succeed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, const SVFL_WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout );
static void scproc_Ichigeki_Korae( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAffAbout affAbout, BppKoraeruCause korae_cause, u16 damage );
static void scproc_Ichigeki_Migawari( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, const SVFL_WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout );
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static BOOL scproc_PushOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  BOOL fForceChange, BOOL* fFailMsgDisped, u16 effectNo, BOOL fIgnoreLevel, BTL_HANDEX_STR_PARAMS* succeedMsg );
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk );
static int get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk );
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void scput_Fight_FieldEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker );
static BOOL scproc_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );
static BOOL scproc_ChangeWeatherCheck( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );
static void scproc_ChangeWeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );
static void scproc_ChangeWeatherAfter( BTL_SVFLOW_WORK* wk, BtlWeather weather );
static u8 scEvent_WazaWeatherTurnUp( BTL_SVFLOW_WORK* wk, BtlWeather weather, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8* turn );
static BOOL scproc_FieldEffectCore( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect, BPP_SICK_CONT contParam, BOOL fAddDependTry );
static void scEvent_FieldEffectCall( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scput_Fight_Uncategory( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets );
static void scput_Fight_Uncategory_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec );
static void scproc_Migawari_Create( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static u16 scproc_Migawari_Damage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 damage,
  BtlTypeAff aff, u8 fCritical, const SVFL_WAZAPARAM* wazaParam );
static void scproc_Migawari_Delete( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, BOOL critical_flag, BOOL fMigawari );
static void scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, BOOL fCritical, BOOL fMigawari );
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, BTL_POKESET* targets, BOOL* fFailMsgEnable );
static void StoreFrontPokeOrderAgility( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet );
static BOOL scproc_TurnCheck( BTL_SVFLOW_WORK* wk );
static void scproc_CheckResetMove( BTL_SVFLOW_WORK* wk );
static void   scproc_turncheck_CommSupport( BTL_SVFLOW_WORK* wk );
static BOOL scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet, BtlEventType event_type );
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlEventType event_type );
static BOOL scproc_turncheck_sick( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet );
static u32 scEvent_SickDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage );
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk );
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg );
static void scPut_SideEffectOffMsg( BTL_SVFLOW_WORK* wk, BtlSideEffect sideEffect, BtlSide side );
static void scproc_turncheck_field( BTL_SVFLOW_WORK* wk );
static void turncheck_field_callback( BtlFieldEffect effect, void* arg );
static void scproc_FieldEff_End( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect );
static BOOL scproc_turncheck_weather( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet );
static int scEvent_CheckWeatherReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlWeather weather, u32 damage );
static void scPut_WeatherDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWeather weather, int damage );
static BOOL scproc_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke );
static u32 checkExistEnemyMaxLevel( BTL_SVFLOW_WORK* wk );
static void scproc_ClearPokeDependEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke );
static void scEvent_BeforeDead( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BOOL scproc_CheckExpGet( BTL_SVFLOW_WORK* wk );
static BOOL scproc_GetExp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* deadPoke );
static BOOL getexp_make_cmd( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const CALC_EXP_WORK* calcExp );
static BOOL scproc_TameHideCancel( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag hideContFlag );
static inline int roundValue( int val, int min, int max );
static inline int roundMin( int val, int min );
static inline int roundMax( int val, int max );
static inline BOOL perOccur( BTL_SVFLOW_WORK* wk, u8 per );
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack );
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size );
static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs );
static void scPut_EffectByPokePos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 effectNo );
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_MeromeroAct( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage );
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick, BOOL fStdMsg );
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT* oldCont );
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx* exCode );
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza );
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scput_WazaNoEffectIchigeki( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag, BOOL fTargetPlural );
static void scPut_WazaAffinityMsg( BTL_SVFLOW_WORK* wk, u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp,  BOOL fTargetPlural );
static void scPut_CriticalMsg( BTL_SVFLOW_WORK* wk, u32 poke_cnt, BTL_POKEPARAM** bpp, const u8* critical_flg, BOOL fTargetPlural );
static void scPut_Koraeru( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BppKoraeruCause cause );
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume );
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, u16 itemID, BOOL fStdMsg );
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value, BOOL fEffectEnable );
static void scPut_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 effType );
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont );
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick );
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID );
static void scPut_Message_StdEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args );
static void scPut_Message_SetEx( BTL_SVFLOW_WORK* wk, u16 strID, u32 argCnt, const int* args );
static void scPut_DecreaseHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 value );
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, BOOL fOrgWaza );
static void scPut_UseItemAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_ConsumeItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_SetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_ResetContFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag flag );
static void scPut_SetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static void scPut_ResetTurnFlag( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppTurnFlag flag );
static void scPut_IllusionSet( BTL_SVFLOW_WORK* wk, CLIENTID_REC* rec );
static u32 scEvent_MemberChangeIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* outPoke );
static BOOL scEvent_GetReqWazaParam( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza );
static u8 scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static void scEvent_SpecialActPriorityWorked( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static void scEvent_BeforeFight( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza );
static u32 scproc_CalcConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckWazaMsgCustom( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, WazaID actWazaID, BTL_HANDEX_STR_PARAMS* str );
static void scEvent_GetWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param );
static void scEvent_CheckWazaExeFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp,
  WazaID waza, SV_WazaFailCause cause );
static BOOL scEvent_WazaExecuteStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* rec );
static BOOL scEvent_CheckMamoruBreak( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8 targetCount, const u8* targetPokeID );
static BOOL scEvent_CheckTameTurnSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza, u8* hideTargetPokeID, BOOL* fFailMsgDisped );
static void scEvent_TameSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec, WazaID waza );
static BppContFlag CheckPokeHideState( const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckPokeHideAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IsCalcHitCancel( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam );
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage, BOOL* fMustEnable );
static void scEvent_ItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_ItemEquipTmp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u8 atkPokeID );
static BtlTypeAff scProc_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BOOL fNoEffectMsg );
static void scproc_WazaNoEffectByFlying( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scproc_ViewEffect( BTL_SVFLOW_WORK* wk, u16 effectNo, BtlPokePos pos_from, BtlPokePos pos_to, BOOL fQueResereved, u32 reservedPos );
static void scEvent_WazaNoEffectByFlying( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static BtlTypeAff scEvent_CheckDamageAffinity( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, PokeType waza_type, PokeType poke_type );
static BOOL scEvent_DecrementPP_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaIdx );
static BOOL scEvent_HitCheckParam( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, HITCHECK_PARAM* param );
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam );
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage );
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag );
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag );
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type );
static void scEvent_AfterMemberIn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static u32 scEvent_GetWazaShrinkPer( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, u32 per );
static void scEvent_FailShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target );
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, u8 wazaUsePokeID, int volume );
static void scEvent_RankEffect_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, u8 atkPokeID,
  const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume );
static void scEvent_WazaRankEffectFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaID wazaID, WazaRankEffect effectID, int volume );
static u16 scEvent_RecalcDrainVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u16 volume );
static void scEvent_AfterChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather );
static u32 scEvent_CalcRecoverHP( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp );
static BOOL scEvent_CheckItemSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID );
static void scEvent_ItemSetDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 nextItemID );
static void scEvent_ItemSetFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scEvent_ChangeTokuseiBefore( BTL_SVFLOW_WORK* wk, u8 pokeID, u16 prev_tokuseiID, u16 next_tokuseiID );
static void scEvent_ChangeTokuseiAfter( BTL_SVFLOW_WORK* wk, u8 pokeID );
static void scEvent_CheckSideEffectParam( BTL_SVFLOW_WORK* wk, u8 userPokeID, BtlSideEffect effect, BtlSide side, BPP_SICK_CONT* cont );
static void AffCounter_Clear( WAZA_AFF_COUNTER* cnt );
static void AffCounter_CountUp( WAZA_AFF_COUNTER* cnt, BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BtlTypeAff affinity );
static void relivePokeRec_Init( BTL_SVFLOW_WORK* wk );
static void relivePokeRec_Add( BTL_SVFLOW_WORK* wk, u8 pokeID );
static BOOL relivePokeRec_CheckNecessaryPokeIn( BTL_SVFLOW_WORK* wk );
static void PSetStack_Init( BTL_SVFLOW_WORK* wk );
static void PSetStack_Push( BTL_SVFLOW_WORK* wk );
static void PSetStack_Pop( BTL_SVFLOW_WORK* wk );
static void psetstack_setup( BTL_SVFLOW_WORK* wk, u32 sp, BOOL fClear );
static HandExResult scproc_HandEx_Result( BTL_SVFLOW_WORK* wk );
static void HandEx_Exe( BTL_SVFLOW_WORK* wk, BTL_HANDEX_PARAM_HEADER* handEx_header );
static u8 scproc_HandEx_UseItemAct( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_ItemEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_drain( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_cureSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID );
static u8 scproc_HandEx_setRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setStatus( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_message( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetTurnFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_resetContFlag( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sideEffectAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sideEffectRemove( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_addFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_removeFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeWeather( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static BOOL handexSub_putString( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_STR_PARAMS* strParam );
static u8 scproc_HandEx_PosEffAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_swapItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_EquipItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_ItemSP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_consumeItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static void scproc_ItemChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static u8 scproc_HandEx_updateWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_counter( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_delayWazaDamage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_quitBattle( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeMember( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_batonTouch( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_addShrink( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_relive( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setWeight( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_pushOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_intrPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_intrWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_sendLast( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_swapPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_hensin( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_fakeBreak( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_juryokuCheck( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_TameHideCancel( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_effectByPos( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_changeForm( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setWazaEffectIndex( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );
static u8 scproc_HandEx_setWazaEffectEnable( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header );




BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, BtlBagMode bagMode, HEAPID heapID )
{
  BTL_SVFLOW_WORK* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SVFLOW_WORK) );

  wk->server = server;
  wk->pokeCon = pokeCon;
  wk->mainModule = mainModule;
  wk->numActOrder = 0;
  wk->turnCount = 0;
  wk->flowResult = SVFLOW_RESULT_DEFAULT;
  wk->que = que;
  wk->heapID = heapID;
  wk->bagMode = bagMode;
  wk->sinkaArcHandle = SHINKA_GetArcHandle( heapID );

  clearWorks( wk );
  BTL_FIELD_Init( BTL_WEATHER_NONE );

  return wk;
}


void BTL_SVFLOW_ResetSystem( BTL_SVFLOW_WORK* wk )
{
  clearWorks( wk );
  BTL_FIELD_Init( BTL_WEATHER_NONE );
}

static void clearWorks( BTL_SVFLOW_WORK* wk )
{
  BTL_WAZAREC_Init( &wk->wazaRec );
  BTL_DEADREC_Init( &wk->deadRec );
  {
    BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
    BTL_POSPOKE_InitWork( &wk->pospokeWork, wk->mainModule, wk->pokeCon, rule );
  }

  GFL_STD_MemClear( wk->pokeDeadFlag, sizeof(wk->pokeDeadFlag) );
  GFL_STD_MemClear( wk->pokeInFlag, sizeof(wk->pokeInFlag) );
  GFL_STD_MemClear( wk->memberChangeCount, sizeof(wk->memberChangeCount) );

  AffCounter_Clear( &wk->affCounter );
  PSetStack_Init( wk );
  BTL_ESCAPEINFO_Clear( &wk->escInfo );

  BTL_Hem_Init( &wk->HEManager );
  eventWork_Init( &wk->eventWork );
  BTL_HANDLER_SIDE_InitSystem();

  wk->nigeruCount = 0;
  wk->wazaEffIdx = 0;
  wk->fMemberOutIntr = FALSE;
  wk->fWinBGMPlayWild = FALSE;
  wk->fEscMsgDisped = FALSE;
  wk->fMemberOutIntr = FALSE;
  wk->cmdBuildStep = 0;
  wk->thruDeadMsgPokeID = BTL_POKEID_NULL;
  wk->prevExeWaza = WAZANO_NULL;
  wk->getPokePos = BTL_POS_NULL;
  wk->simulationCounter = 0;
  wk->turnCheckStep = 0;
}

void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk )
{
  GFL_ARC_CloseDataHandle( wk->sinkaArcHandle );
  GFL_HEAP_FreeMemory( wk );
}




//--------------------------------------------------------------------------
/**
 * �T�[�o�R�}���h�����i�o�g���J�n����j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//--------------------------------------------------------------------------
SvflowResult BTL_SVFLOW_StartBtlIn( BTL_SVFLOW_WORK* wk )
{
  SVCL_WORK* cw;
  u32 i, posIdx;

  SCQUE_Init( wk->que );

  {
    const BTL_FIELD_SITUATION* fSit = BTL_MAIN_GetFieldSituation( wk->mainModule );
    if( fSit->weather != BTL_WEATHER_NONE )
    {
      scproc_ChangeWeather( wk, fSit->weather, BTL_WEATHER_TURN_PERMANENT );
    }
  }

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, i );
    if( cw == NULL ){ continue; }

    for(posIdx=0; posIdx<cw->numCoverPos; ++posIdx)
    {
      BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberData(cw->party, posIdx);
      if( (bpp!=NULL) && !BPP_IsDead(bpp) )
      {
        scproc_MemberInCore( wk, i, posIdx, posIdx );
      }
    }
  }

  scproc_AfterMemberIn( wk );

  return SVFLOW_RESULT_DEFAULT;
}

void BTL_SVFLOW_StartTurn_Boot( BTL_SVFLOW_WORK* wk )
{
  wk->cmdBuildStep = 0;
  wk->numEndActOrder = 0;
}

//=============================================================================================
/**
 * �T�[�o�R�}���h�����i�ʏ�̂P�^�[�����R�}���h�����j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//=============================================================================================
SvflowResult BTL_SVFLOW_StartTurn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction )
{
  wk->flowResult =  SVFLOW_RESULT_DEFAULT;
  SCQUE_Init( wk->que );

  if( wk->cmdBuildStep == 0 )
  {
    relivePokeRec_Init( wk );
    BTL_DEADREC_StartTurn( &wk->deadRec );
    FlowFlg_ClearAll( wk );
    BTL_EVENT_StartTurn();

    BTL_SERVER_InitChangePokemonReq( wk->server );
    BTL_EVENTVAR_CheckStackCleared();

    clearPokeCantActionFlag( wk );

    wk->numActOrder = sortClientAction( wk, clientAction, wk->actOrder, NELEMS(wk->actOrder) );
    wk->cmdBuildStep = 1;
  }

  wk->numEndActOrder = ActOrderProc_Main( wk, wk->numEndActOrder );

  BTL_N_Printf( DBGSTR_SVFL_TurnStart_Result, wk->numEndActOrder, wk->numActOrder );

  return wk->flowResult;
}
/**
 *  �S�|�P�����̔����œ����Ȃ���ԃt���O���N���A
 */
static void clearPokeCantActionFlag( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_CANT_ACTION) ){
      scPut_ResetContFlag( wk, bpp, BPP_CONTFLG_CANT_ACTION );
    }
  }
}
/**
 *
 */
void BTL_SVFLOW_ContinueAfterPokeChange_Boot( BTL_SVFLOW_WORK* wk )
{
  wk->cmdBuildStep = 0;
}
//=============================================================================================
/**
 * �T�[�o�R�}���h�����i�^�[���r���̃|�P��������ւ��I����A�p���j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//=============================================================================================
SvflowResult BTL_SVFLOW_ContinueAfterPokeChange( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction )
{
  wk->flowResult = SVFLOW_RESULT_DEFAULT;
  SCQUE_Init( wk->que );

  if( wk->cmdBuildStep == 0 )
  {
    BTL_N_Printf( DBGSTR_SVFL_StartAfterPokeChange );

    BTL_SERVER_InitChangePokemonReq( wk->server );
    BTL_EVENTVAR_CheckStackCleared();

    wk->actOrderStep = 0;
    wk->cmdBuildStep = 1;

    if( ActOrderProc_OnlyPokeIn(wk, clientAction) ){
      wk->flowResult = SVFLOW_RESULT_LEVELUP;
      return wk->flowResult;
    }
  }

  wk->numEndActOrder = ActOrderProc_Main( wk, wk->numEndActOrder );
  return wk->flowResult;
}
void BTL_SVFLOW_StartAfterPokeIn_Boot( BTL_SVFLOW_WORK* wk )
{
  wk->cmdBuildStep = 0;
}

//=============================================================================================
/**
 * �T�[�o�R�}���h�����i�|�P�������S�E�����Ԃ�ɂ��^�[���ŏ��̓��ꏈ���j
 *
 * @param   wk
 *
 * @retval  SvflowResult
 */
//=============================================================================================
SvflowResult BTL_SVFLOW_StartAfterPokeIn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction )
{
  const BTL_ACTION_PARAM* action;
  u32 i;
  u8 numDeadPoke;

  wk->flowResult =  SVFLOW_RESULT_DEFAULT;
  SCQUE_Init( wk->que );

  if( wk->cmdBuildStep == 0 )
  {
    BTL_SERVER_InitChangePokemonReq( wk->server );
    BTL_EVENTVAR_CheckStackCleared();

    BTL_N_Printf( DBGSTR_SVFL_StartAfterPokeIn );

    numDeadPoke = BTL_DEADREC_GetCount( &wk->deadRec, 0 );

    wk->numActOrder = sortClientAction( wk, clientAction, wk->actOrder, NELEMS(wk->actOrder) );
    wk->numEndActOrder = 0;

    // �܂��́u�|�P�����v�R�}���h����̓���ւ�����
    for(i=0; i<wk->numActOrder; i++)
    {
      action = &wk->actOrder[i].action;
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      if( !BPP_IsDead(wk->actOrder[i].bpp) )
      {
        BTL_N_Printf( DBGSTR_SVFL_AfterPokeIn_Alive,
                wk->actOrder[i].clientID, action->change.posIdx, action->change.memberIdx );

        scproc_MemberChange( wk, wk->actOrder[i].bpp, action->change.memberIdx );
        wk->actOrder[i].fDone = TRUE;
      }
    }
    // �����ċ󂫂𖄂߂�J��o��
    for(i=0; i<wk->numActOrder; i++)
    {
      action = &wk->actOrder[i].action;
      if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action->change.depleteFlag ){ continue; }

      if( BPP_IsDead(wk->actOrder[i].bpp) )
      {
        BTL_N_Printf( DBGSTR_SVFL_AfterPokeIn_Dead,
                action->change.posIdx, action->change.memberIdx );

        scproc_MemberInForChange( wk, wk->actOrder[i].clientID, action->change.posIdx, action->change.memberIdx, TRUE );
        wk->actOrder[i].fDone = TRUE;
      }
    }
    // �V���[�e�[�V�����o�g���ł͋󂫂𖄂߂郍�[�e�[�V��������������
    #ifdef ROTATION_NEW_SYSTEM
    if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION )
    {
      for(i=0; i<wk->numActOrder; i++)
      {
        action = &wk->actOrder[i].action;
        if( action->gen.cmd != BTL_ACTION_ROTATION ){ continue; }
        if( action->change.depleteFlag ){ continue; }

        ActOrder_Proc( wk, &wk->actOrder[i] );
      }
    }
    #endif

    wk->cmdBuildStep = 1;

  }

  scproc_AfterMemberIn( wk );
  scproc_CheckExpGet( wk );
  reqChangePokeForServer( wk, &wk->clientIDRec );

  {
    u8 numDeadPokeAfter = BTL_DEADREC_GetCount( &wk->deadRec, 0 );
    if( numDeadPoke == numDeadPokeAfter ){
      return SVFLOW_RESULT_DEFAULT;
    }else{
      if( scproc_CheckShowdown(wk) == FALSE)
      {
        scPut_IllusionSet( wk, &wk->clientIDRec );
        return SVFLOW_RESULT_POKE_COVER;
      }
      else
      {
        return SVFLOW_RESULT_BTL_SHOWDOWN;
      }
    }
  }
}


//----------------------------------------------------------------------------------
/**
 * �R�}���h�����R�A�i�I���A�N�V�����z�񂩂�j
 *
 * @param   wk
 * @param   startOrderIdx   �J�n�A�N�V����Index
 *
 * @retval  u32   �������I�����A�N�V������
 */
//----------------------------------------------------------------------------------
static u32 ActOrderProc_Main( BTL_SVFLOW_WORK* wk, u32 startOrderIdx )
{
  u32 i;
  u8 fExpGet, fShowDown;

  BTL_N_Printf( DBGSTR_SVFL_ActOrderMainStart, startOrderIdx );

  for(i=startOrderIdx; i<wk->numActOrder; i++)
  {
    ActOrder_Proc( wk, &wk->actOrder[i] );

    // ���x���A�b�v�R�}���h����
    fExpGet = scproc_CheckExpGet( wk );

    // �����i�唚���ȂǓ����S�ł̃P�[�X�́A���S���R�[�h������Ή�������񂶂��Ǝv���Ă�j
    fShowDown = scproc_CheckShowdown( wk );

    if( fShowDown ){
      wk->flowResult = SVFLOW_RESULT_BTL_SHOWDOWN;
      return i+1;
    }

    if( wk->flowResult == SVFLOW_RESULT_POKE_GET ){
      return i+1;
    }

    if( wk->flowResult == SVFLOW_RESULT_POKE_CHANGE ){
      // �r����サ�悤�Ƃ��Ă�Ȃ玀�S�|�P�̕��̓��N�G�X�g�o�����Ⴞ��
      return i+1;
    }

    if( fExpGet ){
      wk->flowResult = SVFLOW_RESULT_LEVELUP;
      return i+1;
    }

    // @todo ������m�肵����{���́u�ɂ���v�R�}���h�������������ׂ�
    if( wk->flowResult == SVFLOW_RESULT_BTL_QUIT ){
      continue;
    }

    GF_ASSERT_MSG( wk->flowResult==SVFLOW_RESULT_DEFAULT, "result=%d\n", wk->flowResult );
    BTL_N_Printf( DBGSTR_SVFL_QueWritePtr, wk->que->writePtr );
  }

  // �^�[���`�F�b�N
  if( wk->flowResult == SVFLOW_RESULT_DEFAULT )
  {
    u8 numDeadPoke;

    // �^�[���`�F�b�N����
    fExpGet = scproc_TurnCheck( wk );
    fShowDown = scproc_CheckShowdown( wk );

    if( fShowDown ){
      wk->flowResult = SVFLOW_RESULT_BTL_SHOWDOWN;
      return wk->numActOrder;
    }
    if( fExpGet ){
      wk->flowResult = SVFLOW_RESULT_LEVELUP;
      return wk->numActOrder;
    }

    numDeadPoke = BTL_DEADREC_GetCount( &wk->deadRec, 0 );
    // ���S�E�����Ԃ�ȂǂŃ|�P�����̕K�v�����邩�`�F�b�N
    if( relivePokeRec_CheckNecessaryPokeIn(wk)
    ||  (numDeadPoke != 0)
    ){
      reqChangePokeForServer( wk, &wk->clientIDRec );
      scPut_IllusionSet( wk, &wk->clientIDRec );

      wk->flowResult = SVFLOW_RESULT_POKE_COVER;
      return wk->numActOrder;
    }

    wk->flowResult = SVFLOW_RESULT_DEFAULT;
  }

  return wk->numActOrder;
}
//----------------------------------------------------------------------------------
/**
 * �R�}���h�����i�|�P��������A�N�V�����̂ݏ����j
 *
 * @param   wk
 *
 * @retval  BOOL  �����̏����Ń|�P���������S->�o���l�擾�Ń��x���A�b�v�����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL ActOrderProc_OnlyPokeIn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction )
{
  SVCL_WORK* clwk;
  BTL_ACTION_PARAM action;
  u32 i, j, actionCnt;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk == NULL ){
      continue;
    }

    actionCnt = BTL_SVCL_ACTION_GetNumActPoke( clientAction, i );
    for(j=0; j<actionCnt; ++j)
    {
      action = BTL_SVCL_ACTION_Get( clientAction, i, j );
      if( action.gen.cmd != BTL_ACTION_CHANGE ){ continue; }
      if( action.change.depleteFlag ){ continue; }

      BTL_N_Printf( DBGSTR_SVFL_PokeChangeRootInfo,
            i, action.change.posIdx, action.change.memberIdx );

      scproc_MemberInForChange( wk, i, action.change.posIdx, action.change.memberIdx, TRUE );
    }
  }
  scproc_AfterMemberIn( wk );
  return scproc_CheckExpGet( wk );
//  return scproc_CheckShowdown( wk );
}
//----------------------------------------------------------------------------------
/**
 * ���݂̃p�[�e�B�f�[�^��r���A����������������
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_CheckShowdown( BTL_SVFLOW_WORK* wk )
{
  u8  pokeExist[2];
  u32 i;

  pokeExist[0] = pokeExist[1] = FALSE;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( BTL_MAIN_IsExistClient(wk->mainModule, i) )
    {
      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, i );
      u8 side = BTL_MAIN_GetClientSide( wk->mainModule, i );
      u8 aliveCnt = BTL_PARTY_GetAliveMemberCount( party );
      if( aliveCnt )
      {
        BTL_N_Printf( DBGSTR_SVFL_ClientPokeStillAlive, i, side, aliveCnt );
        pokeExist[ side ] = TRUE;
      }
      else{
        BTL_N_Printf( DBGSTR_SVFL_ClientPokeDeadAll, i, side);
      }
    }
  }

  if( (pokeExist[0] == FALSE) || (pokeExist[1] == FALSE) ){
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL CheckPlayerSideAlive( BTL_SVFLOW_WORK* wk )
{
  u32 i;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( BTL_MAIN_IsExistClient(wk->mainModule, i) )
    {
      u8 side = BTL_MAIN_GetClientSide( wk->mainModule, i );
      u8 playerSide = BTL_MAIN_GetClientSide( wk->mainModule, BTL_MAIN_GetPlayerClientID(wk->mainModule) );

      if( side == playerSide )
      {
        BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, i );
        if( BTL_PARTY_GetAliveMemberCount(party) )
        {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

//=============================================================================================
/**
 * �T�[�o�R�}���h�����i���̃|�P�������o�� or �ɂ��� -> �ɂ����I�񂾎��̏����j
 *
 * @param   wk
 *
 * @retval  BOOL    �����邱�Ƃ��o������TRUE
 */
//=============================================================================================
BOOL BTL_SVFLOW_MakePlayerEscapeCommand( BTL_SVFLOW_WORK* wk )
{
  u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
  BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, 0 );

  SCQUE_Init( wk->que );

  if( scproc_NigeruCmd_Root(wk, bpp) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//=============================================================================================
/**
 * �V���[�^�[�`���[�W�R�}���h����
 *
 * @param   wk
 *
 * @retval  BOOL    �R�}���h����������TRUE
 */
//=============================================================================================
BOOL BTL_SVFLOW_MakeShooterChargeCommand( BTL_SVFLOW_WORK* wk )
{
  SCQUE_Init( wk->que );

  // �V���[�^�[�̃G�l���M�[�`���[�W
  scproc_countup_shooter_energy( wk );

  return wk->que->writePtr != 0;
}
//----------------------------------------------------------------------------------
/**
 * �V���[�^�[�̃G�l���M�[�`���[�W�v�Z���e�N���C�A���g�ɒʒm
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_countup_shooter_energy( BTL_SVFLOW_WORK* wk )
{
  if( wk->bagMode == BBAG_MODE_SHOOTER )
  {
    u32 i;
    for(i=0; i<BTL_CLIENT_MAX; ++i)
    {
      if( !BTL_SERVER_IsClientEnable(wk->server, i) ){
        continue;
      }
      // �g���v���o�g���ȊO�͖��^�[���P���Œ�
      if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_TRIPLE ){
        SCQUE_PUT_OP_ShooterCharge( wk->que, i, 1 );
      // �g���v���o�g���̏ꍇ�͎����̋󂫈ʒu�{�P
      }else{
        u8 emptyCnt;
        u8 emptyPos[ BTL_POSIDX_MAX ];

        emptyCnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, i, emptyPos );
        SCQUE_PUT_OP_ShooterCharge( wk->que, i, emptyCnt+1 );
      }
    }
  }
}

//=============================================================================================
/**
 * ���[�e�[�V�����R�}���h�����i���[�e�[�V�����o�g���̂݁j
 *
 * @param   wk
 * @param   clientID
 * @param   dir
 */
//=============================================================================================
void BTL_SVFLOW_MakeRotationCommand( BTL_SVFLOW_WORK* wk, u8 clientID, BtlRotateDir dir )
{
  BTL_Printf("�N���C�A���g[%d]�͉�]���� %d �փ��[�e\n", clientID, dir);

  if( (dir != BTL_ROTATEDIR_STAY)
  &&  (dir != BTL_ROTATEDIR_NONE)
  ){
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    BTL_POKEPARAM *outPoke, *inPoke;

    SCQUE_PUT_ACT_Rotation( wk->que, clientID, dir );

    BTL_PARTY_RotateMembers( party, dir, &outPoke, &inPoke );
    if( !BPP_IsDead(outPoke) ){
      scproc_ClearPokeDependEffect( wk, outPoke );
    }
    if( !BPP_IsDead(inPoke) ){
      BTL_HANDLER_TOKUSEI_Add( inPoke );
      BTL_HANDLER_ITEM_Add( inPoke );
    }
    BTL_POSPOKE_Rotate( &wk->pospokeWork, dir, clientID, inPoke, wk->pokeCon );
  }
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o�ɑ΂��A�e�N���C�A���g�̋󂫈ʒu��ʒm
 *
 * @param   wk
 *
 * @retval  BOOL  �N��������ł��Č�������K�v����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL reqChangePokeForServer( BTL_SVFLOW_WORK* wk, CLIENTID_REC* rec )
{
  u8 posAry[ BTL_POSIDX_MAX ];
  u8 empty_pos_cnt, clientID, i;
  u8 result = FALSE;

  rec->count = 0;

  for(clientID=0; clientID<BTL_CLIENT_MAX; ++clientID)
  {
    empty_pos_cnt = BTL_POSPOKE_GetClientEmptyPos( &wk->pospokeWork, clientID, posAry );
    if( empty_pos_cnt )
    {
      // �󂢂Ă���ʒu��S�ăT�[�o�֒ʒm
      BTL_N_Printf( DBGSTR_SVFL_ChangePokeReqInfo, clientID, empty_pos_cnt );
      for(i=0; i<empty_pos_cnt; ++i)
      {
        BTL_SERVER_RequestChangePokemon( wk->server, posAry[i] );
        BTL_N_PrintfSimple( DBGSTR_csv, posAry[i] );
      }
      BTL_N_PrintfSimple( DBGSTR_LF );

      rec->clientID[ rec->count++ ] = clientID;
      result = TRUE;
    }
  }
  return result;
}
//=============================================================================================
/**
 * �������N���C�A���gID���擾
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
const BTL_ESCAPEINFO* BTL_SVFLOW_GetEscapeInfoPointer( const BTL_SVFLOW_WORK* wk )
{
  return &(wk->escInfo);
}
//=============================================================================================
/**
 * �ߊl�����|�P�����̈ʒu���擾
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
 * [Proc] �^�[���ŏ��́u���������v�R�}���h�����O�Ɏ��s���鏈��
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_BeforeFirstFight( BTL_SVFLOW_WORK* wk )
{
  u32 hem_state, i;
  WazaID waza;

  hem_state = BTL_Hem_PushState( &wk->HEManager );
  for(i=0; i<wk->numActOrder; ++i)
  {
    waza = ActOrder_GetWazaID( &wk->actOrder[i] );
    if( waza != WAZANO_NULL )
    {
      if( BTL_HANDLER_Waza_Add(wk->actOrder[i].bpp, waza) )
      {
        scEvent_BeforeFight(wk, wk->actOrder[i].bpp, waza );
        BTL_HANDLER_Waza_Remove( wk->actOrder[i].bpp, waza );
      }
    }
  }
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
static inline u32 ActPri_Make( u8 actPri, u8 wazaPri, u8 spPri, u16 agility )
{
  /*
    �s���D�揇  ...  3 BIT
    ���U�D�揇  ...  6 BIT
    ����D�揇  ...  3 BIT
    �f����      ... 16 BIT
  */

  return ( ((actPri&0x07)<<25) | ((wazaPri&0x3f)<<19) | ((spPri&0x07)<<16) | (agility&0xffff) );
}
static inline u8 ActPri_GetWazaPri( u32 priValue )
{
  return ((priValue >>19) & 0x3f);
}
static inline u8 ActPri_GetSpPri( u32 priValue )
{
  return ((priValue >> 16) & 0x07);
}
static inline u32 ActPri_SetSpPri( u32 priValue, u8 spPri )
{
  priValue &= ~(0x07 << 16);
  priValue |= ((spPri & 0x07) << 16);
  return priValue;
}

//----------------------------------------------------------------------------------
/**
 * �|�P�������Ƃ̃A�N�V�������`�F�b�N���A�����������m��
 *
 * @param   wk
 * @param   clientAction    �N���C�A���g�A�N�V�����f�[�^�n���h��
 * @param   order           �������鏇�ԂɃN���C�A���gID����ג����Ċi�[���邽�߂̔z��
 * @param   orderMax        �z�� order �̗v�f��
 *
 * @retval  u8    ��������|�P������
 */
//----------------------------------------------------------------------------------
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction, ACTION_ORDER_WORK* order, u32 orderMax )
{
  SVCL_WORK* clwk;
  const BTL_ACTION_PARAM* actParam;
  const BTL_POKEPARAM* bpp;
  u32   hem_state;
  u16 agility;
  u8  actionPri, wazaPri;
  u8  i, j, p, numAction, pokeIdx, action;

// �S�|�P�����̍s�����e�����[�N�Ɋi�[
  for(i=0, p=0; i<BTL_CLIENT_MAX; ++i)
  {
    clwk = BTL_SERVER_GetClientWork( wk->server, i );
    if( clwk == NULL ){ continue; }

    numAction = BTL_SVCL_ACTION_GetNumActPoke( clientAction, i );
    pokeIdx = 0;
    BTL_N_Printf( DBGSTR_SVFL_ActionSortInfo, i, numAction );
    for(j=0; j<numAction; j++)
    {
      order[p].action = BTL_SVCL_ACTION_Get( clientAction, i, j );
      if( BTL_ACTION_IsDeplete(&order[p].action) ){
        break;
      }

      action = BTL_ACTION_GetAction( &order[p].action );

      switch( action ){
      case BTL_ACTION_ROTATION:
        {
          pokeIdx = BTL_MAINUTIL_GetRotateInPosIdx( order[p].action.rotation.dir );
          order[p].bpp = BTL_PARTY_GetMemberData( clwk->party, pokeIdx );
          BTL_N_Printf ( DBGSTR_SVFL_ActionSortRotation, pokeIdx );
        }
        break;

      case BTL_ACTION_CHANGE:
        order[p].bpp = BTL_PARTY_GetMemberData( clwk->party, order[p].action.change.posIdx );
        ++pokeIdx;
        break;

      default:
        order[p].bpp = BTL_PARTY_GetMemberData( clwk->party, pokeIdx );
        BTL_N_Printf ( DBGSTR_SVFL_ActionSortGen, pokeIdx, BPP_GetID(order[p].bpp), action );
        ++pokeIdx;
        break;
      }

      order[p].clientID = i;
      order[p].fDone = FALSE;
      order[p].fIntrCheck = FALSE;
      ++p;
    }
  }

// �e�|�P�����̍s���v���C�I���e�B�l�𐶐�
  numAction = p;
  for(i=0; i<numAction; ++i)
  {
    hem_state = BTL_Hem_PushState( &wk->HEManager );
    bpp = order[i].bpp;
    actParam = &(order[i].action);

    // �s���ɂ��D�揇�i�D��x�����قǐ��l��j
    switch( actParam->gen.cmd ){
    case BTL_ACTION_ESCAPE:
      if( (BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD)
      &&  (order[i].clientID == BTL_CLIENT_ENEMY1)
      ){
        // �쐶�|�P�����́u�ɂ���v�́u���������v�Ɠ����v���C�I���e�B�Ɂi�ړ��|�P�p�j
        actionPri = 0;
      }else{
        actionPri = 4;
      }
      break;
    case BTL_ACTION_CHANGE:   actionPri = 3; break;
    case BTL_ACTION_ITEM:     actionPri = 2; break;
    case BTL_ACTION_ROTATION: actionPri = 1; break;
    case BTL_ACTION_SKIP:     actionPri = 0; break;
    case BTL_ACTION_MOVE:     actionPri = 0; break;
    case BTL_ACTION_FIGHT:    actionPri = 0; break;
    case BTL_ACTION_NULL: continue;
    case BTL_ACTION_RECPLAY_TIMEOVER: continue;
    default:
      GF_ASSERT(0);
      actionPri = 0;
      break;
    }
    // �u���������v�ꍇ�̓��U�ɂ��D�揇�A�A�C�e�������ɂ��D��t���O��S�Č���
    if( actParam->gen.cmd == BTL_ACTION_FIGHT )
    {
      WazaID  waza;
      waza = ActOrder_GetWazaID( &order[i] );
      BTL_Printf("�|�P[%d]�̃��U�D��`�F�b�N .. waza=%d\n", BPP_GetID(bpp), waza);
      wazaPri = scEvent_GetWazaPriority( wk, waza, bpp );

    // �u���[�u�v�܂��́u�͂�ǂ��œ����Ȃ��v�ꍇ�A���U�D��x�t���b�g�Ōv�Z
    }
    else if( (actParam->gen.cmd == BTL_ACTION_MOVE) || (actParam->gen.cmd == BTL_ACTION_SKIP) )
    {
      wazaPri = 0 - WAZAPRI_MIN;

    // ����ȊO�͍s���D�揇�݂̂Ŕ���
    }else{
      wazaPri = 0;
    }

    // ���΂₳
    agility = scEvent_CalcAgility( wk, bpp, TRUE );

    BTL_Printf("�s���v���C�I���e�B����I Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
        i, j, actionPri, wazaPri, agility );

    BTL_Hem_PopState( &wk->HEManager, hem_state );

    // �v���C�I���e�B�l�ƃN���C�A���gID��΂ɂ��Ĕz��ɕۑ�
    order[i].priority = ActPri_Make( actionPri, wazaPri, BTL_SPPRI_DEFAULT, agility );
  }

  // �v���C�I���e�B�l�ɂ��\�[�g
  sortActionSub( order, numAction );

  // ���̎��_�ł̏����������[�N�ɋL������
  for(i=0; i<numAction; ++i){
    order[i].defaultIdx = i;
  }

  // �v���C�I���e�B����C�x���g�Ăяo��
  for(i=0; i<numAction; ++i)
  {
    if( (order[i].action.gen.cmd == BTL_ACTION_FIGHT)
    ||  (order[i].action.gen.cmd == BTL_ACTION_MOVE)
    ){
      u8 spPri = scEvent_CheckSpecialActPriority( wk, order[i].bpp );
      order[i].priority = ActPri_SetSpPri( order[i].priority, spPri );
    }
  }

  // �ēx�A�v���C�I���e�B�l�ɂ��\�[�g
  sortActionSub( order, numAction );

  for(i=0; i<numAction; ++i)
  {
    if( ActPri_GetSpPri(order[i].priority) == BTL_SPPRI_HIGH )
    {
      for(j=i+1; j<numAction; ++j)
      {
        // ������荂�D��x�������|�P�����������̉��ɂ�����A
        // �v���C�I���e�B������ʔ����C�x���g�Ăяo��
        if( order[i].defaultIdx > order[j].defaultIdx )
        {
          u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
          scEvent_SpecialActPriorityWorked( wk, order[i].bpp );
          BTL_Hem_PopState( &wk->HEManager, hem_state );
          break;
        }
      }
    }
  }

  return p;
}
//----------------------------------------------------------------------------------
/**
 * �A�N�V�������e���[�N���v���C�I���e�B�l�~���Ƀ\�[�g����
 *
 * @param   order
 * @param   numOrder
 */
//----------------------------------------------------------------------------------
static void sortActionSub( ACTION_ORDER_WORK* order, u32 numOrder )
{
  ACTION_ORDER_WORK tmp;
  u32 i, j;

  for(i=0; i<numOrder; ++i)
  {
    for(j=i+1; j<numOrder; ++j)
    {
      if( order[i].priority > order[j].priority ){
        continue;
      }
      if( order[i].priority == order[j].priority ){ // �S�������v���C�I���e�B�l���������烉���_��
        if( BTL_CALC_GetRand(2) == 0 ){
          continue;
        }
      }

      tmp = order[i];
      order[i] = order[j];
      order[j] = tmp;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�v���C�I���e�B�擾
 * �����U�f�[�^�ɐݒ肳��Ă��� min �` max �̒l�ł͂Ȃ��A0�I���W���̒l�ɒ��������́B
 *
 * @param   wk
 * @param   waza
 * @param   bpp
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaPriority( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* bpp )
{
  u8 pri = WAZADATA_GetParam( waza, WAZAPARAM_PRIORITY ) - WAZAPRI_MIN;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_PRI, pri );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_GET_WAZA_PRI );
    pri = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_PRI );
  BTL_EVENTVAR_Pop();

  return pri;
}
//--------------------------------------------------------------------------
/**
 * [Event] ���΂₳�擾
 *
 * @param   wk
 * @param   attacker
 * @param   fTrickRoomEnable
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BOOL fTrickRoomEnable )
{
  u32 agi = BPP_GetValue( attacker, BPP_AGILITY );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_AGILITY, agi );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE ); // �܂Ѓ`�F�b�N�t���O�Ƃ��Ďg���Ă���
    BTL_EVENTVAR_SetValue( BTL_EVAR_TRICK_FLAG, FALSE );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1f), FX32_CONST(32) );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_AGILITY );

    agi = BTL_EVENTVAR_GetValue( BTL_EVAR_AGILITY );
    {
      fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      agi = BTL_CALC_MulRatio( agi, ratio );
    }
    if( BPP_GetPokeSick(attacker) == POKESICK_MAHI )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
        agi = (agi * BTL_MAHI_AGILITY_RATIO) / 100;
      }
    }
    if( agi > BTL_CALC_AGILITY_MAX ){
      agi = BTL_CALC_AGILITY_MAX;
    }

    // �g���b�N���[���łЂ����肩����
    if( fTrickRoomEnable )
    {
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_TRICK_FLAG) ){
        agi = BTL_CALC_AGILITY_MAX - agi;
      }
    }

  BTL_EVENTVAR_Pop();

  return agi;
}

// �A�N�V�������e����A�D�揇�ʌv�Z�Ɏg�p���郏�UID���擾�i���U�g��Ȃ��ꍇ��WAZANO_NULL�j
static inline WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk )
{
  if( wk->action.gen.cmd == BTL_ACTION_FIGHT )
  {
    return wk->action.fight.waza;
  }
  return WAZANO_NULL;
}
// �w��|�P�����̃A�N�V�������e�f�[�^�|�C���^���擾
static ACTION_ORDER_WORK* ActOrderTool_SearchByPokeID( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i){
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      return &(wk->actOrder[i]);
    }
  }
  return NULL;
}
// �����s�̎w�胏�U�g�p�A�N�V�������e�f�[�^�|�C���^���擾
static ACTION_ORDER_WORK* ActOrderTool_SearchByWazaID( BTL_SVFLOW_WORK* wk, WazaID waza, u8 startIndex )
{
  u32 i;
  for(i=startIndex; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fDone == FALSE )
    {
      if( (BTL_ACTION_GetAction(&wk->actOrder[i].action) == BTL_ACTION_FIGHT)
      &&  (BTL_ACTION_GetWazaID(&wk->actOrder[i].action) == waza)
      ){
        return &wk->actOrder[i];
      }
    }
  }
  return NULL;
}
//----------------------------------------------------------------------------------
/**
 * ���̃��U�ΏۂƂȂ�A�N�V�������e�f�[�^�|�C���^���擾�i���݂��Ȃ��ꍇNULL�j
 *
 * @param   wk
 * @param   waza        �Ώۃ��UID
 * @param   pokeID      ��ɍ��̃��U��ł|�PID�i���̃|�P�̖����݂̂��ΏۂƂȂ�j
 * @param   targetPos   ��ɍ��̃��U��ł|�P���_���Ă���ʒu�i���ʒu��_�����U�̂ݑΏۂƂȂ�j
 *
 * @retval  ACTION_ORDER_WORK*
 */
//----------------------------------------------------------------------------------
static ACTION_ORDER_WORK* ActOrderTool_SearchForCombiWaza( BTL_SVFLOW_WORK* wk, WazaID waza, u8 pokeID, BtlPokePos targetPos )
{
  ACTION_ORDER_WORK* p = ActOrderTool_SearchByWazaID( wk, waza, 0 );
  while( p != NULL )
  {
    if( BTL_MAINUTIL_IsFriendPokeID(pokeID, BPP_GetID(p->bpp)) )
    {
      return p;
    }

    {
      u8 idx = ActOrderTool_GetIndex( wk, p );
      p = ActOrderTool_SearchByWazaID( wk, waza, idx + 1 );
    }
  }
  return NULL;
}
//----------------------------------------------------------------------------------
/**
 * �A�N�V�������e�f�[�^�|�C���^���玩�g��Index���擾
 *
 * @param   wk
 * @param   actOrder
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 ActOrderTool_GetIndex( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder )
{
  int i;
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( &(wk->actOrder[i]) == actOrder ){
      return i;
    }
  }
  GF_ASSERT(0);
  return wk->numActOrder;
}
//----------------------------------------------------------------------------------
/**
 * �w��A�N�V�������A�w��index�ȍ~�̖����s��̐擪�Ɋ��荞�܂���
 *
 * @param   wk
 * @param   actOrder
 * @param   intrIndex   ���荞�݋�����ŏ���index�i����ȍ~�Ɋ��荞�݋��j
 *
 * @retval  int   ���荞��Index �i���荞�߂Ȃ����-1)
 */
//----------------------------------------------------------------------------------
static int ActOrderTool_Intr( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder, u32 intrIndex )
{
  int prevOrder, topOrder, i;

  // �������g�̌��X��Index�ƁA�����s��̐擪Index���T�[�`
  prevOrder = topOrder = -1;
  for(i=intrIndex; i<wk->numActOrder; ++i)
  {
    if( &(wk->actOrder[i]) == actOrder ){
      prevOrder = i;
      break;
    }
  }
  for(i=intrIndex; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fDone == FALSE){
      topOrder = i;
      break;
    }
  }

  // �ǂ�����L���l���������g�������s�̐擪����̏��ԂȂ���בւ�
  if( (prevOrder>=0) && (topOrder>=0)
  &&  (prevOrder > topOrder)
  ){
    wk->actOrderTmp = *actOrder;
    while( prevOrder > topOrder ){
      wk->actOrder[prevOrder] = wk->actOrder[prevOrder-1];
      --prevOrder;
    }
    wk->actOrder[topOrder] = wk->actOrderTmp;
    return topOrder;
  }

  return -1;
}
//----------------------------------------------------------------------------------
/**
 * �w��A�N�V�������A�Ō�ɉ�
 *
 * @param   wk
 * @param   actOrder
 */
//----------------------------------------------------------------------------------
static void ActOrderTool_SendToLast( BTL_SVFLOW_WORK* wk, const ACTION_ORDER_WORK* actOrder )
{
  // �������g�̌��X��Index
  int prevOrder = -1;
  int i;

  for(i=0; i<wk->numActOrder; ++i)
  {
    if( &(wk->actOrder[i]) == actOrder ){
      prevOrder = i;
      break;
    }
  }

  if( prevOrder >= 0 )
  {
    wk->actOrderTmp = *actOrder;
    for(i=prevOrder; i<(wk->numActOrder-1); ++i)
    {
      wk->actOrder[i] = wk->actOrder[i+1];
    }
    wk->actOrder[i] = wk->actOrderTmp;
  }
}

//----------------------------------------------------------------------------------
/**
 *  �A�N�V�������e�̎��s
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
    BTL_ACTION_PARAM action = actOrder->action;

    PSetStack_Push( wk );
    actOrder->fDone = TRUE;
    BTL_N_Printf( DBGSTR_SVFL_ActOrderStart, BPP_GetID(bpp), BPP_GetMonsNo(bpp), actOrder );

    do {
      // ���ɒN���̓������m�肵�Ă��鎞�A�����R�}���h�����ȊO�͎��s���Ȃ�
      if( (wk->flowResult == SVFLOW_RESULT_BTL_QUIT)
      &&  (action.gen.cmd != BTL_ACTION_ESCAPE)
      ){
        break;
      }
      // ����ł�����s���Ȃ�
      if( BPP_IsDead(bpp) ){
        break;
      }
      // ���[�e�[�V�����ȊO�ŁA��ɂ��Ȃ��|�P�͎��s���Ȃ�
      if( (action.gen.cmd != BTL_ACTION_ROTATION)
      &&  (!BTL_POSPOKE_IsExist(&wk->pospokeWork, BPP_GetID(bpp)))
      ){
        break;
      }
      // �t���[�t�H�[����Ԃ̃|�P�͎��s���Ȃ�
      if( BPP_CheckSick(bpp, WAZASICK_FREEFALL) ){
        break;
      }

      scproc_ActStart( wk, bpp, actOrder->action.gen.cmd );

      switch( action.gen.cmd ){
      #ifdef ROTATION_NEW_SYSTEM
      case BTL_ACTION_ROTATION:
        if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION ){
          BTL_N_Printf( DBGSTR_SVFL_ActOrder_Rotation, actOrder->clientID, action.rotation.dir);
          BTL_SVFLOW_MakeRotationCommand( wk, actOrder->clientID, action.rotation.dir );
        }
        else{
          GF_ASSERT(0);
        }
        break;
      #endif
      case BTL_ACTION_FIGHT:
        if( !FlowFlg_Get(wk, FLOWFLG_FIRST_FIGHT) ){
          scproc_BeforeFirstFight( wk );
          FlowFlg_Set( wk, FLOWFLG_FIRST_FIGHT );
        }
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Fight, BPP_GetID(bpp), action.fight.waza, action.fight.targetPos);
        wk->currentSabotageType = CheckSabotageType( wk, bpp );
        scproc_Fight( wk, bpp, &action );
        break;
      case BTL_ACTION_ITEM:
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Item, action.item.number, action.item.targetIdx);
//        scproc_TrainerItem_Root( wk, bpp, action.item.number, action.item.param, action.item.targetIdx );
        {
          TrItemResult result = BTL_SVFSUB_TrainerItemProc( wk, bpp, action.item.number,
                    action.item.param, action.item.targetIdx );

          if( result == TRITEM_RESULT_ESCAPE )
          {
            if( scproc_NigeruCore(wk, bpp, TRUE) ){
              wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
            }
          }
        }
        break;
      case BTL_ACTION_CHANGE:
        BTL_Printf( DBGSTR_SVFL_ActOrder_Change, action.change.posIdx, action.change.memberIdx);
        scproc_MemberChange( wk, bpp, action.change.memberIdx );
        scproc_AfterMemberIn( wk );
        break;
      case BTL_ACTION_ESCAPE:
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Escape );
        if( scproc_NigeruCmd_Root( wk, bpp ) ){
          wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
        }
        break;
      case BTL_ACTION_RECPLAY_TIMEOVER:
        wk->flowResult = SVFLOW_RESULT_BTL_SHOWDOWN;
        break;
      case BTL_ACTION_MOVE:
        scproc_Move( wk, bpp );
        break;
      case BTL_ACTION_SKIP:
        scPut_CantAction( wk, bpp );
        break;
      case BTL_ACTION_NULL:
        BTL_N_Printf( DBGSTR_SVFL_ActOrder_Dead );
  //        scPut_CantAction( wk, bpp );
         break;
      }

      if( (action.gen.cmd == BTL_ACTION_FIGHT) || (action.gen.cmd == BTL_ACTION_ITEM) ){
        BPP_TURNFLAG_Set( bpp, BPP_TURNFLG_ACTION_DONE );
        scPut_SetContFlag( wk, bpp, BPP_CONTFLG_ACTION_DONE );
      }

      {
        u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
        scEvent_ActProcEnd( wk, bpp );
//        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        BTL_Hem_PopState( &wk->HEManager, hem_state );
      }

    }while(0);

    PSetStack_Pop( wk );
    BTL_N_Printf( DBGSTR_SVFL_ActOrderEnd, BPP_GetID(bpp), BPP_GetMonsNo(bpp), actOrder );
  }
}
//----------------------------------------------------------------------------------
/**
 * �A�N�V�������s�J�n�ʒm����
 *
 * @param   wk
 * @param   bpp
 * @param   actionCmd
 */
//----------------------------------------------------------------------------------
static void scproc_ActStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlAction actionCmd )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_ActProcStart( wk, bpp, actionCmd );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �A�N�V�������s�J�n
 *
 * @param   wk
 * @param   bpp
 * @param   actionCmd
 */
//----------------------------------------------------------------------------------
static void scEvent_ActProcStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlAction actionCmd )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ACTION, actionCmd );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ACTPROC_START );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �A�N�V�������s�̏I��
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_ActProcEnd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ACTPROC_END );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * ���ߖ����`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  SabotageType
 */
//----------------------------------------------------------------------------------
static SabotageType CheckSabotageType( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor(wk->mainModule);

  if( ((competitor == BTL_COMPETITOR_TRAINER) || (competitor == BTL_COMPETITOR_WILD))
  &&  (BTL_MAINUTIL_PokeIDtoClientID(BPP_GetID(bpp)) == (BTL_MAIN_GetPlayerClientID(wk->mainModule)))
  ){
    const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );
    const MYSTATUS* status = BTL_MAIN_GetPlayerStatus( wk->mainModule );
    if( !PP_IsMatchOya(pp, status) )
    {
      u8 badgeCnt = BTL_MAIN_GetPlayerBadgeCount( wk->mainModule );
      if( badgeCnt < 8 )
      {
        u16 lv_poke = BPP_GetValue( bpp, BPP_LEVEL );
        u16 lv_border = (1 + badgeCnt) * 10;
        u16 rand;

        if( lv_poke <= lv_border ){
          return SABOTAGE_NONE;
        }
        rand = BTL_CALC_GetRand( (lv_poke + lv_border + 1) );
        if( rand < lv_border ){
          return SABOTAGE_NONE;
        }

        if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
          return SABOTAGE_DONT_ANY;
        }

        rand = BTL_CALC_GetRand( (lv_poke + lv_border + 1) );
        if( rand < lv_border ){
          if( BPP_WAZA_GetUsableCount(bpp) > 1 ){
            return SABOTAGE_OTHER_WAZA;
          }
        }

        rand = BTL_CALC_GetRand( 256 );
        if( rand < (lv_poke - lv_border) )
        {
          if( !scEvent_WazaSick_CheckFail(wk, bpp, bpp, POKESICK_NEMURI) ){
            return SABOTAGE_GO_SLEEP;
          }
        }

        rand -= (lv_poke - lv_border);
        if( rand < (lv_poke - lv_border) ){
          return SABOTAGE_CONFUSE;
        }

        return SABOTAGE_DONT_ANY;
      }
    }
  }
  return SABOTAGE_NONE;

}

//--------------------------------------------------------------
/**
 *  �A�N�V�����̊��荞�ݎ��s�i�|�P����ID�w��j
 *  �����荞��ő����Ɏ��s�i����́g���������h�ɂ̂ݎg�p�j
 */
//--------------------------------------------------------------
static BOOL ActOrder_IntrProc( BTL_SVFLOW_WORK* wk, u8 intrPokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID( wk, intrPokeID );
  if( actOrder && (actOrder->fDone == FALSE) ){
    BTL_N_Printf( DBGSTR_SVFL_ActIntr, actOrder, BPP_GetID(actOrder->bpp));
    ActOrder_Proc( wk, actOrder );
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 *  �A�N�V�����̊��荞�ݗ\��i�|�P����ID�w��j
 *  �����荞��ŁA���ݏ������Ă���A�N�V�����̒���Ɏ��s
 * �i����́g�������ɂǂ����h�ƍ��̃��U�ɂ̂ݎg�p�j
 */
//--------------------------------------------------------------
static BOOL ActOrder_IntrReserve( BTL_SVFLOW_WORK* wk, u8 intrPokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID( wk, intrPokeID );
  if( actOrder && (actOrder->fDone == FALSE) ){
    if( ActOrderTool_Intr(wk, actOrder, 0) >= 0 ){
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 *  �A�N�V�����̊��荞�ݎ��s�i�g�p���U�w��j
 *  �����荞��ŁA���ݏ������Ă���A�N�V�����̒���Ɏ��s�i����́g��񂵂傤�h�ɂ̂ݎg�p�j
 */
//--------------------------------------------------------------
static BOOL ActOrder_IntrReserveByWaza( BTL_SVFLOW_WORK* wk, WazaID waza )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByWazaID( wk, waza, 0 );
  int intrIdx = 0;
  BOOL result = FALSE;
  while( actOrder )
  {
    intrIdx = ActOrderTool_Intr( wk, actOrder, intrIdx );
    if( intrIdx >= 0 ){
      actOrder = ActOrderTool_SearchByWazaID( wk, waza, ++intrIdx );
      result = TRUE;
    }else{
      break;
    }
  }
  return result;
}
//--------------------------------------------------------------
/**
 *  �A�N�V�������Ō�ɉ񂷁i�|�P����ID�w��j
 */
//--------------------------------------------------------------
static BOOL ActOrder_SendLast( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID( wk, pokeID );
  if( actOrder && (actOrder->fDone == FALSE) ){
    ActOrderTool_SendToLast( wk, actOrder );
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
/**
 *  �����s�A�N�V���������s�ς݂ɂ���i�|�P����ID�w��j
 */
//--------------------------------------------------------------
static void ActOrder_ForceDone( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  ACTION_ORDER_WORK* actOrder = ActOrderTool_SearchByPokeID( wk, pokeID );
  if( actOrder )
  {
    actOrder->fDone = TRUE;
  }

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

/**
 *  ���U�G�t�F�N�g�����Ǘ��F������
 */
static void wazaEffCtrl_Init( WAZAEFF_CTRL* ctrl )
{
  ctrl->attackerPos = BTL_POS_NULL;
  ctrl->targetPos = BTL_POS_NULL;
  ctrl->effectIndex = 0;
  ctrl->fEnable = FALSE;
}
/**
 *  ���U�G�t�F�N�g�����Ǘ��F��{�p�����[�^�ݒ�
 */
static void wazaEffCtrl_Setup( WAZAEFF_CTRL* ctrl, BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec )
{
  u32 target_max = BTL_POKESET_GetCountMax( rec );

  ctrl->attackerPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, BPP_GetID(attacker) );
  ctrl->targetPos = BTL_POS_NULL;
  ctrl->fEnable = FALSE;

  // �^�[�Q�b�g���P�̂Ȃ疾�m�ȃ^�[�Q�b�g�ʒu�����L�^
  if( target_max == 1 )
  {
    if( BTL_POKESET_GetCount(rec) )
    {
      const BTL_POKEPARAM* bpp = BTL_POKESET_Get( rec, 0 );
      ctrl->targetPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, BPP_GetID(bpp) );
    }
  }
}

/**
 *  ���U�G�t�F�N�g�����Ǘ��F�����m��
 */
static inline void wazaEffCtrl_SetEnable( WAZAEFF_CTRL* ctrl )
{
  if( ctrl->fEnable == FALSE )
  {
    ctrl->fEnable = TRUE;
  }
}
/**
 *  ���U�G�t�F�N�g�����Ǘ��F�����m�肷�邩�`�F�b�N
 */
static inline BOOL wazaEffCtrl_IsEnable( const WAZAEFF_CTRL* ctrl )
{
  return ctrl->fEnable;
}
static inline void wazaEffCtrl_SetEffectIndex( WAZAEFF_CTRL* ctrl, u8 index )
{
  ctrl->effectIndex = index;
}

/**
 *  �|�P�������݁������`�F�b�N�ꎞ�֐�
 */
static inline BOOL IsBppExist( const BTL_POKEPARAM* bpp )
{
  if( bpp != NULL ){
    return !BPP_IsDead(bpp);
  }
  return FALSE;
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
        if( IsBppExist( BTL_PARTY_GetMemberDataConst(cw->party, j)) )
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
          if( IsBppExist(nextPoke) )
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
//======================================================================================================
// �T�[�o�[�t���[����
//======================================================================================================

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���[�u�v
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
    scproc_MoveCore( wk, clientID, posIdx, 1, TRUE );
  }
  else
  {
    GF_ASSERT_MSG(0, "clientID=%d, pokeID=%d, posIdx=%d\n", clientID, pokeID, posIdx);
  }
}
/**
 *
 */
static void scproc_MoveCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx1, u8 posIdx2, BOOL fActCmd )
{
  BtlPokePos pos1, pos2;

  pos1 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx1 );
  pos2 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx2 );

  BTL_POSPOKE_Swap( &wk->pospokeWork, pos1, pos2 );
  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    BTL_PARTY_SwapMembers( party, posIdx1, posIdx2 );
  }

  if( fActCmd ){
    SCQUE_PUT_ACT_MemberMove( wk->que, clientID, pos1, pos2 );
  }
}

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u�ɂ���v
//-----------------------------------------------------------------------------------
/**
 * �����鏈�����[�g
 *
 *  @retval ������ꂽ��TRUE, �������s��FALSE
 */
static BOOL scproc_NigeruCmd_Root( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );
  u8 playerClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );

  BOOL result = scproc_NigeruCmdSub( wk, bpp );

  if( result )
  {
    if( !BTL_MAINUTIL_IsFriendClientID(clientID, playerClientID) ){
      BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_NIGERARETA );
    }
    return TRUE;
  }
  else
  {
    if( clientID == playerClientID ){
      BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_NIGERU_SIPPAI );
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeFail );
    }else{
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_EscapeFailed, BPP_GetID(bpp) );
    }
  }

  if( clientID == playerClientID )
  {
    if( ++(wk->nigeruCount) > NIGERU_COUNT_MAX ){
      wk->nigeruCount = NIGERU_COUNT_MAX;
    }
  }

  return result;
}
/**
 * ������R�}���h�I������������
 *
 *  @retval ������ꂽ��TRUE, �������s��FALSE
 */
static BOOL scproc_NigeruCmdSub( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BOOL fSkipNigeruCalc = TRUE;
  BOOL fForceNigeru = TRUE;

  if( BTL_MAIN_GetEscapeMode(wk->mainModule) == BTL_ESCAPE_MODE_WILD )
  {
    u8 escClientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );
    if( escClientID == BTL_MAIN_GetPlayerClientID(wk->mainModule) ){
      fSkipNigeruCalc = scEvent_SkipNigeruCalc( wk, bpp );
      fForceNigeru = FALSE;
    }
  }

  #ifdef PM_DEBUG
  // L or R �������ςȂ��ŋ����I�ɓ�����
  if( ( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ) || ( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ) ){
    fForceNigeru = TRUE;
    fSkipNigeruCalc = TRUE;
  }
  #endif

  if( !fSkipNigeruCalc )
  {
    const BTL_POKEPARAM* bppEnemy = BTL_POKECON_GetClientPokeDataConst( wk->pokeCon, BTL_CLIENTID_SA_ENEMY1, 0 );
    if( bppEnemy )
    {
      u16 myAgi = BPP_GetValue_Base( bpp, BPP_AGILITY );
      u16 enemyAgi = BPP_GetValue_Base( bppEnemy, BPP_AGILITY );

      if( myAgi <= enemyAgi )
      {
        u32 value = ((myAgi << FX32_SHIFT) / enemyAgi * 128) >> FX32_SHIFT;
        value += (wk->nigeruCount * 30 );
        if( BTL_CALC_GetRand(256) >= value ){
          return FALSE;
        }
      }
    }
  }

  return scproc_NigeruCore( wk, bpp, fForceNigeru );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����鎞�̊m���v�Z�������X�L�b�v���邩����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �X�L�b�v����Ȃ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipNigeruCalc( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  if( !BPP_IsDead(bpp) )
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
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * �����鏈���R�A
 *
 * @param   wk
 * @param   bpp
 * @param   fNigeruCmd
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_NigeruCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BOOL fForceNigeru )
{
  u8 escapeClientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );

  // �쐶��͓����锻��
  if( BTL_MAIN_GetEscapeMode(wk->mainModule) == BTL_ESCAPE_MODE_WILD )
  {
    u8 playerClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
    u8 fEscapeEnemy = ( !BTL_MAINUTIL_IsFriendClientID( escapeClientID, playerClientID) );

    // �����I�ɓ������ꍇ�̓`�F�b�N���s��Ȃ�
    if( !fForceNigeru )
    {
      // �����֎~�`�F�b�N
      if( !BPP_IsDead(bpp) )
      {
        if( !scEvent_SkipNigeruForbid(wk, bpp) )
        {
          u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
          BOOL fForbid = scEvent_CheckNigeruForbid( wk, bpp );
          BTL_Hem_PopState( &wk->HEManager, hem_state );
          if( fForbid ){
            return FALSE;
          }
        }
      }
    }

    // �����܂ŗ����瓦���m��
    if( !(wk->fEscMsgDisped) )
    {
      // ����ȓ������b�Z�[�W�`�F�b�N
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      BOOL fSpMsgDisped = scEvent_NigeruExMessage( wk, bpp );

      // �����Ȃ���ΕW�����b�Z�[�W
      if( !fSpMsgDisped )
      {
        if( fEscapeEnemy ){
          SCQUE_PUT_MSG_STD_SE( wk->que, BTL_STRID_STD_EscapeWild, SEQ_SE_NIGERU, BPP_GetID(bpp) );
        }else{
          SCQUE_PUT_MSG_STD_SE( wk->que, BTL_STRID_STD_EscapeSuccess, SEQ_SE_NIGERU );
        }
      }
      BTL_Hem_PopState( &wk->HEManager, hem_state );

      wk->fEscMsgDisped = TRUE;
    }

  }

  // �T�u�E�F�C��E�ʐM�ΐ�Ȃǂ͖������ɓ�����
  BTL_ESCAPEINFO_Add( &wk->escInfo, escapeClientID );
  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����镕���̃`�F�b�N�������X�L�b�v���邩����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �X�L�b�v����Ȃ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKIP_NIGERU_FORBID );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����镕���`�F�b�N����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  �����镕�����������Ă�����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckNigeruForbid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BOOL result = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
    BTL_SICKEVENT_CheckEscapeForbit( wk, bpp );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NIGERU_FORBID );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����邱�Ƃ����������Ƃ��ɓ��ꃁ�b�Z�[�W��\�����邩����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    ���ꃁ�b�Z�[�W��\������Ȃ�TRUE
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


//----------------------------------------------------------------------------------
/**
 * �����o�[����R�A
 *
 * @param   wk
 * @param   clientID      �N���C�A���gID
 * @param   posIdx        ����ʒu index
 * @param   nextPokeIdx   ���ꂳ����|�P�����̌��p�[�e�B�� index
 */
//----------------------------------------------------------------------------------
static void scproc_MemberInCore( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
  BTL_PARTY* party;
  BTL_POKEPARAM* bpp;
  u8 pokeID;

  party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );

  {
    SCQUE_PUT_OP_SetIllusionForParty( wk->que, clientID );
    BTL_MAIN_SetIllusionForParty(  wk->mainModule, party );
  }

  if( posIdx != nextPokeIdx ){
    // �N���C�A���g�ւ̒ʒm�� SCQUE_PUT_OP_MemberIn
    BTL_PARTY_SwapMembers( party, posIdx, nextPokeIdx );
  }
  bpp = BTL_PARTY_GetMemberData( party, posIdx );
  pokeID = BPP_GetID( bpp );

  BTL_MAIN_RegisterZukanSeeFlag( wk->mainModule, clientID, bpp );

  BTL_HANDLER_TOKUSEI_Add( bpp );
  BTL_HANDLER_ITEM_Add( bpp );
  BPP_SetAppearTurn( bpp, wk->turnCount );
  BPP_Clear_ForIn( bpp );
  wk->pokeInFlag[ pokeID ] = TRUE;

  SCQUE_PUT_OP_MemberIn( wk->que, clientID, posIdx, nextPokeIdx, wk->turnCount );
  {
    BtlPokePos  pos = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx );
    BTL_POSPOKE_PokeIn( &wk->pospokeWork, pos, BPP_GetID(bpp), wk->pokeCon );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����o�[����ւ�
 *
 * @param   wk
 * @param   clientID
 * @param   posIdx
 * @param   next_poke_idx
 * @param   fPutMsg
 */
//----------------------------------------------------------------------------------
static void scproc_MemberInForChange( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 next_poke_idx, BOOL fPutMsg )
{
  scproc_MemberInCore( wk, clientID, posIdx, next_poke_idx );
  SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, next_poke_idx, fPutMsg );
}


//----------------------------------------------------------------------------------
/**
 * �T�[�o�[�t���[�F�V������ɓ��ꂵ���|�P��������
 *
 * @param   wk
 *
 */
//----------------------------------------------------------------------------------
static void scproc_AfterMemberIn( BTL_SVFLOW_WORK* wk )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKESET* pokeSet;
  BTL_POKEPARAM* bpp;
  u32 hem_state;
  u8  pokeID;

  pokeSet = &wk->pokesetMemberInProc;
  BTL_POKESET_Clear( pokeSet );
  FRONT_POKE_SEEK_InitWork( &fps, wk );

  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) )
  {
    {
      u8 pokeID = BPP_GetID( bpp );
      if( wk->pokeInFlag[ pokeID ] )
      {
        BTL_POKESET_Add( pokeSet, bpp );
        wk->pokeInFlag[ pokeID ] = FALSE;
      }
    }
  }
  BTL_POKESET_SortByAgility( pokeSet, wk );


  BTL_POKESET_SeekStart( pokeSet );
  while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
  {
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_AfterMemberIn( wk, bpp );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] �����o�[��֎��́u���� ���ǂ�I�v�Ȃǂ̃��b�Z�[�W�\���R�}���h
 *
 * @param   wk
 * @param   bpp   �������߂�|�P�����p�����[�^
 */
//----------------------------------------------------------------------------------
static void scPut_MemberOutMessage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

  SCQUE_PUT_ACT_MemberOutMsg( wk->que, clientID, pokeID );
}
//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�����o�[���
//-----------------------------------------------------------------------------------
static void scproc_MemberChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u8 nextPokeIdx )
{
  scPut_MemberOutMessage( wk, outPoke );

  if( scproc_MemberOutForChange(wk, outPoke, FALSE) )
  {
    BtlPokePos pos;
    u8 clientID, posIdx;
    u8 outPokeID = BPP_GetID( outPoke );

    pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, outPokeID );
    BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, pos, &clientID, &posIdx );

    if( wk->memberChangeCount[clientID] < MEMBER_CHANGE_COUNT_MAX ){
      wk->memberChangeCount[clientID]++;
    }

    scproc_MemberInForChange( wk, clientID, posIdx, nextPokeIdx, TRUE );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����o�[���ꂩ��ޏꂳ����i����ւ������O�j
 *
 * @param   wk
 * @param   bpp           �ޏꂳ����|�P�����p�����[�^
 * @param   fIntrDisable  ���荞�݃A�N�V�����i���������Ȃǁj���֎~����t���O
 *
 * @retval  BOOL    �ޏ�ł�����TRUE�^���荞�܂�Ď��񂾂肵����FALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_MemberOutForChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, BOOL fIntrDisable )
{
  // ���荞�݃`�F�b�N
  if( !fIntrDisable )
  {
    u32 intrPokeCount = scEvent_MemberChangeIntr( wk, outPoke );
    if( intrPokeCount )
    {
      u32 i;
      wk->fMemberOutIntr = TRUE;
      for(i=0; i<intrPokeCount; ++i)
      {
        ActOrder_IntrProc( wk, wk->MemberOutIntrPokeID[i] );
        if( BPP_IsDead(outPoke) ){
          break;
        }
      }
      wk->fMemberOutIntr = FALSE;
    }
  }

  if( !BPP_IsDead(outPoke) )
  {
    scproc_MemberOutCore( wk, outPoke, BTLEFF_POKEMON_MODOSU );
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �����o�[�ޏ�i����ւ��E������΂��E���S�j���ʏ���
 *
 * @param   wk
 * @param   outPoke
 * @param   effectNo   �G�t�F�N�g�i���o�[(0=�G�t�F�N�g�Ȃ��j
 *
 */
//----------------------------------------------------------------------------------
static void scproc_MemberOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke, u16 effectNo )
{
  u8 pokeID = BPP_GetID( outPoke );


  {
    BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
    if( pos != BTL_POS_NULL )
    {
      SCQUE_PUT_ACT_MemberOut( wk->que, pos, effectNo );
    }
  }

  ActOrder_ForceDone( wk, pokeID );

  scproc_ClearPokeDependEffect( wk, outPoke );
  BPP_Clear_ForOut( outPoke );
  SCQUE_PUT_OP_OutClear( wk->que, BPP_GetID(outPoke) );

  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    scEvent_MemberOutFixed( wk, outPoke );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }

  BTL_POSPOKE_PokeOut( &wk->pospokeWork, pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����o�[�ޏ�̊m��
 *
 * @param   wk
 * @param   outPoke
 */
//----------------------------------------------------------------------------------
static void scEvent_MemberOutFixed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* outPoke )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(outPoke) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_OUT_FIXED );
  BTL_EVENTVAR_Pop();
}

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v���[�g
//-----------------------------------------------------------------------------------
static void scproc_Fight( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_ACTION_PARAM* action )
{
  REQWAZA_WORK  reqWaza;
  WazaID  orgWaza, actWaza;
  BtlPokePos  orgTargetPos, actTargetPos;
  BppContFlag  tameFlag;
  FLAG_SET  wazaFlags;
  u8 fWazaEnable, fWazaLock, fReqWaza, fPPDecrement, orgWazaIdx;

  tameFlag = CheckPokeHideState( attacker );

  wazaEffCtrl_Init( wk->wazaEffCtrl );
  wazaRobParam_Init( wk->magicCoatParam );

  wazaFlags.raw = 0;
  reqWaza.wazaID = WAZANO_NULL;
  reqWaza.targetPos = BTL_POS_NULL;

  /* �A���R�[����ԂňႤ���U��I��ł����ꍇ�͋����������� */
  if( BPP_CheckSick(attacker, WAZASICK_ENCORE) ){
    WazaID encoreWaza = BPP_SICKCONT_GetParam( BPP_GetSickCont(attacker, WAZASICK_ENCORE) );
    if( (encoreWaza != action->fight.waza)
    &&  (BPP_WAZA_GetPP_ByNumber(attacker, encoreWaza))
    ){
      action->fight.waza = encoreWaza;
    }
  }

  orgWaza = action->fight.waza;
  orgWazaIdx = BPP_WAZA_SearchIdx( attacker, orgWaza );
  actWaza = orgWaza;
  orgTargetPos = action->fight.targetPos;
  actTargetPos = orgTargetPos;
  fWazaEnable = FALSE;
  fPPDecrement = FALSE;
  fWazaLock = BPP_CheckSick(attacker, WAZASICK_WAZALOCK) || BPP_CheckSick(attacker, WAZASICK_TAMELOCK);

  BTL_HANDLER_Waza_Add( attacker, orgWaza );
  scproc_StartWazaSeq( wk, attacker, orgWaza );

  do {

    // ���U�o�����s����P�i�|�P�����n��Ԉُ큕������A���������A�Ђ�݁j
    if( scproc_Fight_CheckWazaExecuteFail_1st(wk, attacker, orgWaza, fWazaLock ) ){ break; }

    // �h�����U�Ăяo���p�����[�^�`�F�b�N�����s����
    if( !scEvent_GetReqWazaParam(wk, attacker, orgWaza, orgTargetPos, &reqWaza) ){
      scPut_WazaMsg( wk, attacker, orgWaza );
      scproc_WazaExecuteFailed( wk, attacker, orgWaza, SV_WAZAFAIL_OTHER );
      break;
    }

    // �h�����U�Ăяo������ꍇ�A���b�Z�[�W�o�́����U�o���m��C�x���g
    fReqWaza = ( reqWaza.wazaID != WAZANO_NULL );
    if( fReqWaza )
    {
      scPut_WazaMsg( wk, attacker, orgWaza );
      scPut_ReqWazaEffect( wk, attacker, orgWaza, orgTargetPos );
      BTL_HANDLER_Waza_Add( attacker, reqWaza.wazaID );
      actWaza = reqWaza.wazaID;
      actTargetPos = reqWaza.targetPos;
    }else{
      actWaza = orgWaza;
      actTargetPos = orgTargetPos;
    }

    // ���U�p�����[�^�m��
    scEvent_GetWazaParam( wk, orgWaza, attacker, wk->wazaParamOrg );
    if( fReqWaza )
    {
      scEvent_GetWazaParam( wk, actWaza, attacker, wk->wazaParam );
      wk->wazaParam->fReqWaza = TRUE;
    }
    else{
      *(wk->wazaParam) = *(wk->wazaParamOrg);
    }

    // ���U�Ώۂ����[�N�Ɏ擾
    BTL_POKESET_Clear( wk->psetTargetOrg );
    BTL_SVFSUB_RegisterTargets( wk, attacker, actTargetPos, wk->wazaParam, wk->psetTargetOrg );
    BTL_POKESET_RemoveDeadPoke( wk->psetTargetOrg );
    BTL_POKESET_Copy( wk->psetTargetOrg, wk->psetTarget );

    // �g�������U��PP���炷�i�O�^�[�����烍�b�N����Ă���ꍇ�͌��炳�Ȃ��j
    if( (!fWazaLock) )
    {
      if( orgWazaIdx != PTL_WAZA_MAX )
      {
        scproc_decrementPPUsedWaza( wk, attacker, orgWaza, orgWazaIdx, wk->psetTargetOrg );
        fPPDecrement = TRUE;
      }
    }

    // ���U���b�Z�[�W�o�͊m��
    scproc_WazaExe_Decide( wk, attacker, wk->wazaParamOrg, BTL_EVENT_WAZA_CALL_DECIDE );

    // ���̃��U�i�㔭�j�̔����`�F�b�N
    scEvent_CheckCombiWazaExe( wk, attacker, actWaza );

    // ���U���b�Z�[�W�o��
    if( scEvent_CheckWazaMsgCustom(wk, attacker, orgWaza, actWaza, &wk->strParam) ){
      // �����U�Ăяo�����ȂǁA���U�����b�Z�[�W���J�X�^�}�C�Y�����ꍇ
      handexSub_putString( wk, &wk->strParam );
      HANDEX_STR_Clear( &wk->strParam );
    }else{
      scPut_WazaMsg( wk, attacker, actWaza );
    }

    // ���U�o�����s����Q�i�o�o������j
    if( scproc_Fight_CheckWazaExecuteFail_2nd(wk, attacker, actWaza, fWazaLock )){
       break;
    }

    // �x���������U�̏�������
    if( scproc_Fight_CheckDelayWazaSet(wk, attacker, actWaza, actTargetPos) ){ break; }

    // ���̃��U�i�攭�j�̏�������
    if( scproc_Fight_CheckCombiWazaReady(wk, attacker, actWaza, actTargetPos) ){ break; }

    // �����܂ŗ����烏�U���o�邱�Ƃ͊m��
    BTL_WAZAREC_Add( &wk->wazaRec, actWaza, wk->turnCount, BPP_GetID(attacker) );
    scproc_WazaExe_Decide( wk, attacker, wk->wazaParam, BTL_EVENT_WAZA_EXE_DECIDE );

    // ���U�����蔻��
    if( scproc_Check_WazaRob(wk, attacker, actWaza, wk->psetTarget, wk->wazaRobParam) )
    {
      BTL_N_Printf( DBGSTR_SVFL_YokodoriDetermine, BPP_GetID(attacker), actWaza );
      scproc_WazaRobRoot( wk, attacker, actWaza, wk->psetTarget );
    }
    // ������ꂸ�ʏ폈��
    else
    {
      fWazaEnable = scproc_Fight_WazaExe( wk, attacker, actWaza, wk->psetTarget );
    }

    wk->prevExeWaza = actWaza;

  }while(0);

  // ���U�v���Z�X�C������
  BPP_TURNFLAG_Set( attacker, BPP_TURNFLG_WAZAPROC_DONE );

  // PP������烏�U�g�p�L�^���X�V
  if( fPPDecrement )
  {
    BPP_UpdateWazaProcResult( attacker, actTargetPos, fWazaEnable, wk->wazaParam->wazaType, actWaza, orgWaza );
    SCQUE_PUT_OP_UpdateWazaProcResult( wk->que, BPP_GetID(attacker), actTargetPos, fWazaEnable,
                            wk->wazaParam->wazaType, actWaza, orgWaza );
  }

  // ���߃��U����^�[���̎��s�ɑΏ�
  if( (tameFlag != BPP_CONTFLG_NULL) && (CheckPokeHideState(attacker) != BPP_CONTFLG_NULL) ){
    scproc_TameLockClear( wk, attacker );
  }
  if( BPP_TURNFLAG_Get(attacker, BPP_TURNFLG_TAMEHIDE_OFF )
  ){
    SCQUE_PUT_ACT_TameWazaHide( wk->que, BPP_GetID(attacker), FALSE );
    scproc_FreeFall_CheckRelease( wk, attacker, FALSE );
  }

  scproc_EndWazaSeq( wk, attacker, actWaza );

  if( reqWaza.wazaID != WAZANO_NULL ){
    BTL_HANDLER_Waza_Remove( attacker, reqWaza.wazaID );
  }
  BTL_HANDLER_Waza_Remove( attacker, orgWaza );

  scproc_MagicCoat_Root( wk, actWaza );
}
//----------------------------------------------------------------------------------
/**
 * �}�W�b�N�R�[�g�������[�g
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void scproc_MagicCoat_Root( BTL_SVFLOW_WORK* wk, WazaID actWaza )
{
  BTL_POKEPARAM *robPoke, *targetPoke;
  u32 i;

  BTL_N_Printf( DBGSTR_SVFL_MagicCoatStart, wk->magicCoatParam->robberCount);

  for(i=0; i<wk->magicCoatParam->robberCount; ++i)
  {
    robPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, wk->magicCoatParam->robberPokeID[i] );
    targetPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, wk->magicCoatParam->targetPokeID[i] );

    BTL_N_Printf( DBGSTR_SVFL_MagicCoatReflect,
      wk->magicCoatParam->robberPokeID[i],
      wk->magicCoatParam->targetPokeID[i],
      wk->magicCoatParam->targetPos[i] );

    if( BPP_IsFightEnable(robPoke) )
    {
      // ���˕Ԃ��m��C�x���g
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_WazaReflect( wk, robPoke, targetPoke, actWaza );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );

      // ���̌�A���˕Ԃ�����
      // ���U�p�����[�^�����ւ�
      scEvent_GetWazaParam( wk, actWaza, robPoke, wk->wazaParam );
      wk->wazaParam->fMagicCoat = TRUE;
      BTL_SVFSUB_RegisterTargets( wk, robPoke, wk->magicCoatParam->targetPos[i], wk->wazaParam, wk->psetRobTarget );

      BTL_HANDLER_Waza_Add( robPoke, actWaza );
      scproc_Fight_WazaExe( wk, robPoke, actWaza, wk->psetRobTarget );
//      scproc_EndWazaSeq( wk, robPoke, actWaza );

      BTL_HANDLER_Waza_RemoveForce( robPoke, actWaza );
    }
  }
}


static void wazaRobParam_Init( WAZA_ROB_PARAM* param )
{
  u32 i;
  param->robberCount = 0;
  for(i=0; i<NELEMS(param->robberPokeID); ++i){
    param->robberPokeID[i] = BTL_POKEID_NULL;
  }
}
static void wazaRobParam_Add( WAZA_ROB_PARAM* param, u8 robberPokeID, u8 targetPokeID, BtlPokePos targetPos )
{
  if( param->robberCount < (NELEMS(param->robberPokeID)) )
  {
    param->robberPokeID[ param->robberCount ] = robberPokeID;
    param->targetPokeID[ param->robberCount ] = targetPokeID;
    param->targetPos[ param->robberCount ] = targetPos;
    param->robberCount++;
  }
}

//----------------------------------------------------------------------------------
/**
 * ���U�����菈�����[�g
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   defaultTarget
 */
//----------------------------------------------------------------------------------
static void scproc_WazaRobRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID actWaza, BTL_POKESET* defaultTarget )
{
  BtlPokePos robTargetPos;

  // �N������������
  if( wk->wazaRobParam->robberCount )
  {
    u8 robPokeID = wk->wazaRobParam->robberPokeID[ 0 ];
    BTL_POKEPARAM* robPoke = BTL_POKECON_GetPokeParam( wk->pokeCon, robPokeID );

    // ������i���������|�P����̂ɂȂ�A�w�肳�ꂽ�^�[�Q�b�g�Ƀ��U���o���j
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_WazaRob( wk, robPoke, attacker, actWaza );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );

      // ���U�p�����[�^�����ւ�
      scEvent_GetWazaParam( wk, actWaza, robPoke, wk->wazaParam );
      BTL_SVFSUB_RegisterTargets( wk, robPoke, wk->wazaRobParam->targetPos[0], wk->wazaParam, wk->psetRobTarget );

      BTL_N_Printf( DBGSTR_SVFL_YokodoriExe, robPokeID, wk->wazaRobParam->targetPos[0] );

      BTL_HANDLER_Waza_Add( robPoke, actWaza );
      scproc_Fight_WazaExe( wk, robPoke, actWaza, wk->psetRobTarget );
      BTL_HANDLER_Waza_RemoveForce( robPoke, actWaza );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���̃��U�̐����`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckCombiWazaExe( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_COMBIWAZA_CHECK );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ���U�����m��C�x���g�Ăяo������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_Decide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, BtlEventType evType )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  scEvent_WazaExeDecide( wk, attacker, wazaParam, evType );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�����m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExeDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, BtlEventType evType  )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENT_CallHandlers( wk, evType );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * �K�v�Ȃ�A���̃��U�̔��������������s��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckCombiWazaReady( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos )
{
  static const WazaID CombiWazaTbl[] = {
    WAZANO_HONOONOTIKAI,  WAZANO_MIZUNOTIKAI,  WAZANO_KUSANOTIKAI,
  };
  u32 i;

  for(i=0; i<NELEMS(CombiWazaTbl); ++i)
  {
    if( CombiWazaTbl[i] == waza ){
      BTL_N_Printf( DBGSTR_SVFL_CombiWazaCheck, waza);
      break;
    }
  }
  if( (i != NELEMS(CombiWazaTbl))
  &&  (!BPP_TURNFLAG_Get(attacker, BPP_TURNFLG_COMBIWAZA_READY))
  &&  (!BPP_CombiWaza_IsSetParam(attacker))
  ){
    enum {
      COMBI_MAX = BTL_POSIDX_MAX - 1,   // ���̃��U���ꏏ�Ɍ����Ă����_����̍ő�|�P��
    };
    ACTION_ORDER_WORK*  orderWork[ COMBI_MAX ];
    u32 orderCnt = 0;
    u8 pokeID = BPP_GetID( attacker );

    for(i=0; i<NELEMS(CombiWazaTbl); ++i)
    {
      if( CombiWazaTbl[i] != waza )
      {
        orderWork[ orderCnt ] = ActOrderTool_SearchForCombiWaza( wk, CombiWazaTbl[i], pokeID, targetPos );
        if( orderWork[ orderCnt ] != NULL ){
          BTL_N_Printf( DBGSTR_SVFL_CombiWazaFound,
                    targetPos, CombiWazaTbl[i], BPP_GetID(orderWork[orderCnt]->bpp) );
          if( ++orderCnt >= COMBI_MAX ){ break; }
        }
      }
    }

    if( orderCnt )
    {
      BTL_POKEPARAM* combiPoke;
      u8 idx, idxMin, combiPokeID;

      idxMin = BTL_POS_MAX;
      combiPoke = orderWork[ 0 ]->bpp;

      for(i=0; i<orderCnt; ++i)
      {
        idx = ActOrderTool_GetIndex( wk, orderWork[i] );
        if( idx < idxMin ){
          idxMin = idx;
          combiPoke = orderWork[i]->bpp;
        }
      }

      combiPokeID = BPP_GetID( combiPoke );
      BTL_N_Printf( DBGSTR_SVFL_CombiDecide, pokeID, combiPokeID );

      BPP_TURNFLAG_Set( attacker, BPP_TURNFLG_COMBIWAZA_READY );
      BPP_CombiWaza_SetParam( combiPoke, pokeID, waza );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_CombiWazaReady, pokeID, combiPokeID );

      ActOrder_IntrReserve( wk, combiPokeID );
      return TRUE;
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �K�v�Ȃ�A�x���������U�̏����������s���B
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 *
 * @retval  BOOL      �x���������U�����������s�����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlPokePos targetPos )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  BOOL result = scEvent_CheckDelayWazaSet( wk, attacker, targetPos );

  if( result )
  {
    BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
    if( scproc_HandEx_Result(wk) == HandExResult_Enable )
    {
      SCQUE_PUT_ACT_WazaEffect( wk->que, atPos, targetPos, waza, BTLV_WAZAEFF_DELAY_START );
      scproc_Fight_DecideDelayWaza( wk, attacker );
    }
    else{
      scPut_WazaFail( wk, attacker, waza );
    }
  }

  BTL_Hem_PopState( &wk->HEManager, hem_state );
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �x�����U�����`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   targetPos
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckDelayWazaSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BtlPokePos targetPos )
{
  BOOL result;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEPOS, targetPos );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_DELAY_WAZA );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return result;
}
//----------------------------------------------------------------------------------
/**
 * �x���������U�̎��s�m��i���U�G�t�F�N�g�\����j
 *
 * @param   wk
 * @param   attacker
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_DecideDelayWaza( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

    scEvent_DecideDelayWaza( wk, attacker );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �x�����U�����m��
 *
 * @param   wk
 * @param   attacker
 */
//----------------------------------------------------------------------------------
static void scEvent_DecideDelayWaza( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECIDE_DELAY_WAZA );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * ���U�V�[�P���X�J�n����
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_StartWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_StartWazaSeq( wk, attacker, waza );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�V�[�P���X�J�n
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_StartWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_START );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ���U�V�[�P���X�I������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_EndWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_EndWazaSeq( wk, attacker, waza );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�V�[�P���X�I��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_EndWazaSeq( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_END );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U������m��
 *
 * @param   wk
 * @param   robPoke       ����������
 * @param   orgAtkPoke    ���X�A���U���o������
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(robPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(orgAtkPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_ROB );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �}�W�b�N�R�[�g���˕Ԃ��m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaReflect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* robPoke, const BTL_POKEPARAM* orgAtkPoke, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(robPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(orgAtkPoke) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASEQ_REFRECT );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * �킴������`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   rec       [io] �{���̃^�[�Q�b�g�Q�B���������|�P�͂������珜�O�����B
 * @param   targetPos [out] ���������ꍇ�A���̑Ώۈʒu��ێ�
 *
 * @retval  u8    ���������|�P������ID�^���������|�P�����Ȃ��ꍇ��BTL_POKEID_NULL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Check_WazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* rec, WAZA_ROB_PARAM* robParam )
{
  u8 robberPokeID = BTL_POKEID_NULL;
  u8 robTargetPokeID = BTL_POKEID_NULL;

  scEvent_CheckWazaRob( wk, attacker, waza, rec, &robberPokeID, &robTargetPokeID );
  if( robberPokeID != BTL_POKEID_NULL )
  {
    if( robTargetPokeID != BTL_POKEID_NULL ){
      robParam->targetPos[0] = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, robTargetPokeID );
    }else{
      robParam->targetPos[0] = BTL_POS_NULL;
    }

    BTL_N_Printf( DBGSTR_SVFL_YokodoriInfo, robTargetPokeID, robParam->targetPos[0]);

    robParam->robberPokeID[0] = robberPokeID;
    robParam->robberCount = 1;

    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����ȊO�̃|�P���o�������U�������锻��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   targetPos
 * @param   robberPokeID    [out] ���������|�P����ID
 * @param   robTargetPokeID [out] ���������ꍇ�̑Ώۃ|�P����ID
 * @param   strParam        [out] ���������ꍇ�̃��b�Z�[�W�p�����[�^
 *
 * @retval  BOOL    ����������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckWazaRob( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza,
  BTL_POKESET* targetRec, u8* robberPokeID, u8* robTargetPokeID )
{
  u32 targetCnt = BTL_POKESET_GetCount( targetRec );
  u32 i;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i)
      {
        bpp = BTL_POKESET_Get( targetRec, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID, BTL_POKEID_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEID_NULL );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_WAZA_ROB );

    *robberPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID );
    *robTargetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );

  BTL_EVENTVAR_Pop();

  return (*robberPokeID) != BTL_POKEID_NULL;
}

// �����U�Ăяo�����郏�U�ŁA�����s����ꍇ�̂�TRUE
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
// �����U�Ăяo�����郏�U�̃G�t�F�N�g�𔭓�
static void scPut_ReqWazaEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, BtlPokePos targetPos )
{
  BtlPokePos  atkPos;
  u8 pokeID = BPP_GetID( bpp );

  atkPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );

  SCQUE_PUT_ACT_WazaEffect( wk->que, atkPos, targetPos, waza, 0 );
}

//----------------------------------------------------------------------------------
/**
 * ���U�o�����R�[�h�X�V
 *
 * @param   wk
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExeRecordUpdate( BTL_SVFLOW_WORK* wk, WazaID waza )
{
  if( waza == WAZANO_HANERU ){
    BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_WAZA_HANERU );
    return;
  }
  if( waza == WAZANO_WARUAGAKI ){
    BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_WAZA_WARUAGAKI );
    return;
  }
}
//----------------------------------------------------------------------------------
/**
 * �u���������v-> ���U�o�������ȍ~�̏���
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL  ���U�������ɏI������ꍇFALSE�^����ȊO��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_WazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* targetRec )
{
  WazaCategory  category = WAZADATA_GetCategory( waza );
  u8 pokeID = BPP_GetID( attacker );
  u8 fEnable = TRUE;
  u8 fDamage = FALSE;

  u16  que_reserve_pos;

  BTL_N_Printf( DBGSTR_SVFL_WazaExeStart, BPP_GetID(attacker), waza );

  // ���R�[�h�f�[�^�X�V
  scproc_WazaExeRecordUpdate( wk, waza );

  // �P�^�[�����߃��U�̔����`�F�b�N
  if( scproc_Fight_TameWazaExe(wk, attacker, targetRec, waza) ){
    return TRUE;
  }

  // �_���[�W�󂯃|�P�������[�N���N���A���Ă���
  BTL_POKESET_Clear( wk->psetDamaged );

  // ���U�G�t�F�N�g�R�}���h�����p�Ƀo�b�t�@�̈��\�񂵂Ă���
  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );
  wazaEffCtrl_Setup( wk->wazaEffCtrl, wk, attacker, targetRec );

  // ���U�o�������J�n�C�x���g
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    BOOL fQuit = scEvent_WazaExecuteStart( wk, attacker, waza, targetRec );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
    if( fQuit ){
      // ����A����fQuit�́u���܂�v�ł������p�B
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
      return TRUE;
    }
  }

  // ����ł�|�P�����͏��O
  BTL_POKESET_RemoveDeadPoke( targetRec );
  // �ŏ��͋����^�[�Q�b�g���c���Ă��Ȃ�->���܂����܂�Ȃ������A�I��
  if( BTL_POKESET_IsRemovedAll(targetRec) ){
    scPut_WazaFail( wk, attacker, waza );
    fEnable = FALSE;
  }

  if( fEnable )
  {
    BOOL fMigawariHit = FALSE;

    switch( category ){
    case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
    case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
    case WAZADATA_CATEGORY_DAMAGE_EFFECT:
    case WAZADATA_CATEGORY_DAMAGE_SICK:
    case WAZADATA_CATEGORY_DRAIN:
      fDamage = TRUE;
    }

    // �^�C�v�ɂ�閳�����`�F�b�N
    if( fDamage || (category == WAZADATA_CATEGORY_ICHIGEKI) ){
      flowsub_checkWazaAffineNoEffect( wk, wk->wazaParam, attacker, targetRec, &wk->dmgAffRec );
      fMigawariHit = TRUE;
    }

    // �Ώۂ��Ƃ̖����`�F�b�N->����`�F�b�N�i�����\���͂��̐�ɔC����j
    flowsub_checkNotEffect( wk, wk->wazaParam, attacker, targetRec );
    if( category != WAZADATA_CATEGORY_ICHIGEKI )  // �ꌂ���U�͓Ǝ��̉�𔻒���s���̂�
    {
      flowsub_checkWazaAvoid( wk, wk->wazaParam, attacker, targetRec );
    }

    // �ŏ��͋����^�[�Q�b�g���c���Ă��Ȃ� -> �����C�x���g�Ăяo����I��
    if( BTL_POKESET_IsRemovedAll(targetRec) )
    {
      scproc_WazaExe_NotEffective( wk, pokeID, waza );
      fEnable = FALSE;
    }
    else
    {
      // �݂���菜�O�`�F�b�N
      scproc_MigawariExclude( wk, wk->wazaParam, attacker, targetRec, fMigawariHit );
      // �^�[�Q�b�g���c���Ă��Ȃ� -> �����C�x���g�Ăяo����I��
      if( BTL_POKESET_IsRemovedAll(targetRec) )
      {
        scPut_WazaFail( wk, attacker, waza );
        scproc_WazaExe_NotEffective( wk, pokeID, waza );
        fEnable = FALSE;
      }
    }
  }

  if( fEnable )
  {
    BTL_Printf("���U=%d, �J�e�S��=%d\n", wk->wazaParam.wazaID, category );

    if( fDamage ){
      scproc_Fight_Damage_Root( wk, wk->wazaParam, attacker, targetRec, &wk->dmgAffRec, FALSE );
    }
    else{

      switch( category ){
      case WAZADATA_CATEGORY_SIMPLE_EFFECT:
        scproc_Fight_SimpleEffect( wk, wk->wazaParam, attacker, targetRec );
        break;
      case WAZADATA_CATEGORY_SIMPLE_SICK:
        scproc_Fight_SimpleSick( wk, waza, attacker, targetRec );
        break;
      case WAZADATA_CATEGORY_EFFECT_SICK:
        scproc_Fight_EffectSick( wk, wk->wazaParam, attacker, targetRec );
        break;
      case WAZADATA_CATEGORY_ICHIGEKI:
        scproc_Fight_Ichigeki( wk, wk->wazaParam, attacker, targetRec );
        break;
      case WAZADATA_CATEGORY_PUSHOUT:
        scproc_Fight_PushOut( wk, waza, attacker, targetRec );
        break;
      case WAZADATA_CATEGORY_SIMPLE_RECOVER:
        scproc_Fight_SimpleRecover( wk, waza, attacker, targetRec );
        break;
      case WAZADATA_CATEGORY_FIELD_EFFECT:
        scput_Fight_FieldEffect( wk, wk->wazaParam, attacker );
        break;
      case WAZADATA_CATEGORY_SIDE_EFFECT:
      case WAZADATA_CATEGORY_OTHERS:
        scput_Fight_Uncategory( wk, wk->wazaParam, attacker, targetRec );
        break;
      default:
        SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
        break;
      }
    }

    // ���U���ʂ��聨�G�t�F�N�g�R�}���h�����Ȃǂ�
    if( wazaEffCtrl_IsEnable(wk->wazaEffCtrl) )
    {
      scPut_WazaEffect( wk, waza, wk->wazaEffCtrl, que_reserve_pos );
      BTL_WAZAREC_SetEffectiveLast( &wk->wazaRec );

      // �����œ����Ȃ��Ȃ鏈��
      if( !BPP_IsDead(attacker) ){
        if( WAZADATA_GetFlag(waza, WAZAFLAG_Tire) ){
          scPut_SetContFlag( wk, attacker, BPP_CONTFLG_CANT_ACTION );
        }
      }

      scproc_WazaExe_Effective( wk, pokeID, waza );

    }else{
      scproc_WazaExe_NotEffective( wk, pokeID, waza );
      fEnable = FALSE;
    }


  }
  else{
    // �����ł��G�t�F�N�g�̂ݔ�������i���΂��E�����΂��͂Ή��j
    if( wazaEffCtrl_IsEnable(wk->wazaEffCtrl) ){
      scPut_WazaEffect( wk, waza, wk->wazaEffCtrl, que_reserve_pos );
    }
  }

  scproc_WazaExe_Done( wk, pokeID, waza );
  scproc_CheckDeadCmd( wk, attacker );

  return fEnable;
}
//----------------------------------------------------------------------------------
/**
 * �݂���蒆�̃|�P�������^�[�Q�b�g���珜�O
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   target
 * @param   fDamage
 */
//----------------------------------------------------------------------------------
static void scproc_MigawariExclude( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
    const BTL_POKEPARAM* attacker, BTL_POKESET* target, BOOL fDamage )
{
  BTL_POKEPARAM* bpp;

  BTL_POKESET_SeekStart( target );
  while( (bpp = BTL_POKESET_SeekNext(target) ) != NULL )
  {
    if( (attacker != bpp)
    &&  (BPP_MIGAWARI_IsExist(bpp))
    ){
      if( (!fDamage) && (WAZADATA_GetFlag(wazaParam->wazaID, WAZAFLAG_MigawariThru)==FALSE) )
      {
//      if( scEvent_CheckMigawariExclude(wk, attacker, bpp, wazaParam->wazaID, fDamage) ){
        BTL_POKESET_Remove( target, bpp );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �݂���蒆�|�P���������U�Ώۂ��珜�O���邩�`�F�b�N
 *
 * @param   wk
 * @param   attacker    �U���|�P
 * @param   defender    �^�[�Q�b�g�|�P�i�݂���蒆�j
 * @param   fDamage     �_���[�W��^���郏�U���H
 *
 * @retval  BOOL    ���O����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckMigawariExclude( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL fDamage )
{
  BOOL fExclude = (!fDamage); // �f�t�H���g�ł̓_���[�W���U�Ȃ珜�O�����A����ȊO�͏��O����

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_NOEFFECT_FLAG, fExclude );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MIGAWARI_EXCLUDE );
    fExclude = BTL_EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
  BTL_EVENTVAR_Pop();

  return fExclude;
}


//----------------------------------------------------------------------------------
/**
 * ���U�o����->�L�����������̃C�x���g�Ăяo��
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_Effective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_WazaExeEnd_Common( wk, pokeID, waza, BTL_EVENT_WAZA_EXECUTE_EFFECTIVE );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * ���U�o����->�������������̃C�x���g�Ăяo��
 *
 * @param   wk
 * @param   pokeID  ���U���o�����|�P����ID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_NotEffective( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_WazaExeEnd_Common( wk, pokeID, waza, BTL_EVENT_WAZA_EXECUTE_NO_EFFECT );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );

  BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_WAZA_MUKOU );

  if( BTL_MAINUTIL_PokeIDtoClientID(pokeID) == BTL_MAIN_GetPlayerClientID(wk->mainModule) ){
    BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_WAZA_MUKOU );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���U�o���ŏI�C�x���g�Ăяo��
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scproc_WazaExe_Done( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_WazaExeEnd_Common( wk, pokeID, waza, BTL_EVENT_WAZA_EXECUTE_DONE );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�o���I������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaExeEnd_Common( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza, BtlEventType eventID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, eventID );
  BTL_EVENTVAR_Pop();
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
  if( BPP_CheckSick(attacker, WAZASICK_MUSTHIT) ){
    return TRUE;
  }
  if( BPP_CheckSick(attacker, WAZASICK_MUSTHIT_TARGET) )
  {
    u8 targetPokeID = BPP_GetSickParam( attacker, WAZASICK_MUSTHIT_TARGET );
    TAYA_Printf("pokeID=%d �́A%d �����b�N�I�����Ă�\n", BPP_GetID(attacker), targetPokeID );
    if( targetPokeID == BPP_GetID(target) ){
      return TRUE;
    }
  }

  return FALSE;
}
//--------------------------------------------------------------------------
/**
 * ���U�����O�́A�����ɂ�閳�����`�F�b�N�i�_���[�W���U�̂݁j
 *
 * @param   wk
 * @param   attacker  [in] ���U���o���|�P����
 * @param   targets   [io] ���U�Ώۃ|�P�����R���e�i�^�����ɂȂ����|�P�����̓R���e�i����폜�����
 *
 */
//--------------------------------------------------------------------------
static void flowsub_checkWazaAffineNoEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets, BTL_DMGAFF_REC* affRec )
{
  BTL_POKEPARAM* bpp;
  BtlTypeAff aff;

  BTL_DMGAFF_REC_Init( affRec );

  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets) ) != NULL )
  {
    aff = scProc_checkWazaDamageAffinity( wk, attacker, bpp, wazaParam, TRUE );
    BTL_DMGAFF_REC_Add( affRec, BPP_GetID(bpp), aff );
    if( aff == BTL_TYPEAFF_0 )
    {
      BTL_POKESET_Remove( targets, bpp );
      AffCounter_CountUp( &wk->affCounter, wk, attacker, bpp, BTL_TYPEAFF_0 );
    }
  }
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
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;

  // Lv1 �������`�F�b�N�i�U���|�P���K����ԂȂ疳�����ł��Ȃ��j
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( !IsMustHit(attacker, bpp) )
    {
      if( scproc_checkNoEffect_sub(wk, wazaParam, attacker, bpp, BTL_EVENT_NOEFFECT_CHECK_L1) ){
        BTL_POKESET_Remove( targets, bpp );
      }
    }
  }

  // Lv2 �������`�F�b�N�i�U���|�P���K����Ԃł��������j
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( scproc_checkNoEffect_sub(wk, wazaParam, attacker, bpp, BTL_EVENT_NOEFFECT_CHECK_L2) ){
      BTL_POKESET_Remove( targets, bpp );
    }
  }

  // �܂���`�F�b�N
  if( WAZADATA_GetFlag(wazaParam->wazaID, WAZAFLAG_Mamoru) )
  {
    BTL_POKESET_SeekStart( targets );
    while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
    {
      if( BPP_TURNFLAG_Get(bpp, BPP_TURNFLG_MAMORU) )
      {
        // �܂��閳��������Ȃ���΃^�[�Q�b�g���珜�O
        if( !scEvent_CheckMamoruBreak(wk, attacker, bpp, wazaParam->wazaID) ){
          BTL_POKESET_Remove( targets, bpp );
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Mamoru, BPP_GetID(bpp) );
        }
      }
    }
  }

  // Lv3 �������`�F�b�N�i�U���|�P���K����Ԃł����������u�܂���v����v���C�I���e�B�j
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( scproc_checkNoEffect_sub(wk, wazaParam, attacker, bpp, BTL_EVENT_NOEFFECT_CHECK_L3) ){
      BTL_POKESET_Remove( targets, bpp );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * ���U�����`�F�b�N�������\��
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   target
 * @param   eventID
 *
 * @retval  BOOL    ������������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_checkNoEffect_sub( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, BtlEventType eventID )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  BOOL fNoReaction = FALSE;
  BOOL fNoEffect;

  fNoEffect = scEvent_CheckNotEffect( wk, wazaParam, eventID, attacker, target, &wk->strParam, &fNoReaction );
  if( fNoEffect )
  {
    // �Ƃ肠�����������o�������̃��A�N�V����
    if( HANDEX_STR_IsEnable(&wk->strParam) ){
      handexSub_putString( wk, &wk->strParam );
      HANDEX_STR_Clear( &wk->strParam );
    }
    else{
      // HandEx�𗘗p�������A�N�V����
      if( !scproc_HandEx_Result( wk ) )
      {
        if( !fNoReaction )
        {
          // �������A�N�V�����Ȃ���΁u���ʂ��Ȃ��悤���c�v
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(target) );
        }
      }
    }
  }

  BTL_Hem_PopState( &wk->HEManager, hem_state );
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
 * @param   customMsg   �����������ꍇ�ɓ���̃��b�Z�[�W���o�͂���ꍇ�̃p�����[�^
 *
 * @retval  BOOL    �����ȏꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BtlEventType eventID,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* customMsg, BOOL* fNoReaction )
{
  BOOL fNoEffect = FALSE;
  HANDEX_STR_Clear( customMsg );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_MAGICCOAT_FLAG, wazaParam->fMagicCoat );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)customMsg );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_NOEFFECT_FLAG, fNoEffect );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, eventID );
    fNoEffect = BTL_EVENTVAR_GetValue( BTL_EVAR_NOEFFECT_FLAG );
    (*fNoReaction) = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return fNoEffect;
}

//--------------------------------------------------------------------------
/**
 * ���U�������� -> �������Ȃ������ꍇ�� targets ���珜�O
 *
 * @param   wk
 * @param   attacker  [in] ���U���o���|�P����
 * @param   targets   [io] ���U�Ώۃ|�P�����R���e�i�^�����ɂȂ����|�P�����̓R���e�i����폜�����
 *
 */
//--------------------------------------------------------------------------
static void flowsub_checkWazaAvoid( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;
  u8 pokeID[ BTL_POS_MAX ];
  u8 avoid_count = 0;

  // �Ώۂ��ŏ����烏�U��ł����|�P�����P�̂̃��U�͖������Ȃǂɂ�锻����s��Ȃ�
  if( (BTL_POKESET_GetCountMax(targets) == 1)
  &&  (BTL_POKESET_Get(targets,0) == attacker)
  ){
    return;
  }

  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( scEvent_SkipAvoidCheck(wk, attacker, bpp, wazaParam) ){
      continue;
    }
    if( IsTripleFarPos(wk, attacker, bpp, wazaParam->wazaID)
    ||  !scEvent_CheckHit(wk, attacker, bpp, wazaParam)
    ){
      pokeID[ avoid_count++ ] = BPP_GetID( bpp );
      BTL_POKESET_Remove( targets, bpp );
      scPut_WazaAvoid( wk, bpp, wazaParam->wazaID );
    }
  }
  if( avoid_count )
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    scEvent_WazaAvoid( wk, attacker, wazaParam->wazaID, avoid_count, pokeID );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U����������X�L�b�v���邩�`�F�b�N
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 *
 * @retval  BOOL    �X�L�b�v����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_SkipAvoidCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  BOOL fSkip = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SKIP_AVOID_CHECK );
    fSkip = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return fSkip;
}

//----------------------------------------------------------------------------------
/**
 * �g���v�����A������Ȃ��ʒu�̃|�P�������ǂ����𔻒�
 *
 * @param   wk
 * @param   attacker    �U������|�P
 * @param   target      �U�������|�P
 * @param   waza        ���UID
 *
 * @retval  BOOL        �g���v���œ�����Ȃ��ʒu�Ȃ�TRUE�^����ȊOFALSE
 */
//----------------------------------------------------------------------------------
static BOOL IsTripleFarPos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza )
{
  if( (BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_TRIPLE)
  &&  (WAZADATA_GetFlag(waza, WAZAFLAG_TripleFar) == FALSE)
  ){
    BtlPokePos  atkPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
    BtlPokePos  tgtPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(target) );

    if( !BTL_MAINUTIL_CheckTripleHitArea(atkPos, tgtPos) ){
      return TRUE;
    }
  }
  return FALSE;
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
 * @retval  BOOL  ������ꍇ��TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam )
{
  if( IsMustHit(attacker, defender) ){
    return TRUE;
  }

  if( scEvent_CheckPokeHideAvoid(wk, attacker, defender, wazaParam->wazaID) ){
    return FALSE;
  }

  if( scEvent_IsCalcHitCancel(wk, attacker, defender, wazaParam->wazaID) ){
    return TRUE;
  }

  if( BPP_CheckSick(defender, WAZASICK_TELEKINESIS) ){
    return TRUE;
  }

  {
    u8 wazaHitPer, fAvoidFlat;
    s8 hitRank, avoidRank, totalRank;
    u32 totalPer;
    fx32 ratio;

    wazaHitPer = scEvent_getHitPer(wk, attacker, defender, wazaParam);

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, FALSE );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HIT_RANK, BPP_GetValue(attacker, BPP_HIT_RATIO) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_AVOID_RANK, BPP_GetValue(defender, BPP_AVOID_RATIO) );
      BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );
      BTL_SICKEVENT_CheckHitRatio( wk, attacker, defender );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RANK );

      hitRank = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_RANK );
      avoidRank = BTL_EVENTVAR_GetValue( BTL_EVAR_AVOID_RANK );
      fAvoidFlat = BTL_EVENTVAR_GetValue( BTL_EVAR_FLAT_FLAG );
      // �u�݂�Ԃ�v�����ꂽ�|�P�����͉�𗦏㏸�𖳎�
      if( ((BPP_CheckSick(defender, WAZASICK_MIYABURU)) && (avoidRank > BTL_CALC_HITRATIO_MID))
      ||  (fAvoidFlat)
      ){
        avoidRank = BTL_CALC_HITRATIO_MID;
      }

      ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );

    BTL_EVENTVAR_Pop();

    totalRank = roundValue( (int)(BTL_CALC_HITRATIO_MID + hitRank - avoidRank), BTL_CALC_HITRATIO_MIN, BTL_CALC_HITRATIO_MAX );
    totalPer  = BTL_CALC_HitPer( wazaHitPer, totalRank );

    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo1, BPP_GetID(attacker), hitRank, wazaHitPer );
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo2, BPP_GetID(defender), avoidRank );
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo3, totalRank, totalPer, ratio );

    totalPer = BTL_CALC_MulRatio( totalPer, ratio );
    if( totalPer > 100 ){
      totalPer = 100;
    }
    if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_HIT100PER) ){
      totalPer = 100;
    }
    BTL_N_Printf( DBGSTR_SVFL_HitCheckInfo4, totalPer);

    return perOccur( wk, totalPer );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���U���ʂ��邱�Ƃ��m�聨�K�v�ȉ��o�R�}���h�̐���
 *
 * @param   wk
 * @param   waza
 * @param   defPokeID
 * @param   que_reserve_pos
 */
//----------------------------------------------------------------------------------
static void scPut_WazaEffect( BTL_SVFLOW_WORK* wk, WazaID waza, WAZAEFF_CTRL* effCtrl, u32 que_reserve_pos )
{
  SCQUE_PUT_ReservedPos( wk->que, que_reserve_pos, SC_ACT_WAZA_EFFECT,
          effCtrl->attackerPos, effCtrl->targetPos, waza, effCtrl->effectIndex );

  BTL_N_Printf( DBGSTR_SVFL_PutWazaEffect, que_reserve_pos, waza, effCtrl->attackerPos, effCtrl->targetPos, effCtrl->effectIndex );

  effCtrl->effectIndex = BTLV_WAZAEFF_INDEX_DEFAULT;
}
//----------------------------------------------------------------------------------
/**
 * �K�v�Ȃ�A�P�^�[�����߃��U�̗��߃^�[���������s���B
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 *
 * @retval  BOOL      ���߃^�[���������s�����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_Fight_TameWazaExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza )
{
  if( WAZADATA_GetFlag(waza, WAZAFLAG_Tame) )
  {
    if( !BPP_CheckSick(attacker, WAZASICK_TAMELOCK) )
    {
      BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

      // ���߃^�[���X�L�b�v����
      if( !scEvent_CheckTameTurnSkip(wk, attacker, waza) )
      {
        if( scproc_TameStartTurn(wk, attacker, atPos, targetRec, waza) )
        {
          BPP_SICK_CONT  sickCont = BPP_SICKCONT_MakeTurnParam( 2, waza );
          scPut_AddSick( wk, attacker, WAZASICK_TAMELOCK, sickCont );
        }
        // �X�L�b�v�ł��Ȃ����͂�����return
        return TRUE;
      }

      if( !scproc_TameStartTurn(wk, attacker, atPos, targetRec, waza) ){
        return FALSE;
      }
      {
        u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
        scEvent_TameSkip( wk, attacker, waza );
//        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        BTL_Hem_PopState( &wk->HEManager, hem_state );
      }
    }

    wazaEffCtrl_SetEffectIndex( wk->wazaEffCtrl, BTLV_WAZAEFF_TAME_RELEASE );
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_TameRelease( wk, attacker, targetRec, waza );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }
    scproc_TameLockClear( wk, attacker );
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���ߊJ�n�^�[������
 *
 * @param   wk
 * @param   attacker
 * @param   atPos
 * @param   targetRec
 * @param   waza
 */
//----------------------------------------------------------------------------------
static BOOL scproc_TameStartTurn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_POKESET* targetRec, WazaID waza )
{
  BOOL fSuccess, fFailMsgDisped = FALSE;
  u8 hideTargetPokeID = BTL_POKEID_NULL;

  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  fSuccess = scEvent_TameStart( wk, attacker, targetRec, waza, &hideTargetPokeID, &fFailMsgDisped );

  if( fSuccess )
  {
    BtlPokePos targetPos = wk->defaultTargetPos;
    if( BTL_POKESET_GetCount(targetRec) )
    {
      BTL_POKEPARAM* target = BTL_POKESET_Get( targetRec, 0 );
      targetPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, BPP_GetID(target) );
    }
    SCQUE_PUT_ACT_WazaEffect( wk->que, atPos, targetPos, waza, BTLV_WAZAEFF_TAME_START );
  }
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );

  if( fSuccess )
  {
    if( CheckPokeHideState(attacker) != BPP_CONTFLG_NULL ){
      SCQUE_PUT_ACT_TameWazaHide( wk->que, BPP_GetID(attacker), TRUE );
    }
    if( hideTargetPokeID != BTL_POKEID_NULL ){
      SCQUE_PUT_ACT_TameWazaHide( wk->que, hideTargetPokeID, TRUE );
    }
  }
  else
  {
    if( !fFailMsgDisped ){
      scPut_WazaFail( wk, attacker, waza );
    }
  }

  return fSuccess;
}
static void scproc_TameLockClear( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker )
{
  if( BPP_CheckSick(attacker, WAZASICK_TAMELOCK) )
  {
    scPut_CureSick( wk, attacker, WAZASICK_TAMELOCK, NULL );
    BPP_TURNFLAG_Set( attacker, BPP_TURNFLG_TAMEHIDE_OFF );
  }

  {
    BppContFlag contFlag = CheckPokeHideState( attacker );
    while( contFlag != BPP_CONTFLG_NULL )
    {
      scPut_ResetContFlag( wk, attacker, contFlag );
      contFlag = CheckPokeHideState( attacker );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �t���[�t�H�[�����߃^�[���J�n����
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_FreeFall_Start( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL* fFailMsgDisped )
{
  *fFailMsgDisped = FALSE;

  // �Ώۂ�����ł�E�݂����E���U���ʂŏꂩ������Ă�ꍇ�͎��s
  if( (!BPP_IsDead(target))
  &&  (!BPP_MIGAWARI_IsExist(target))
  &&  (CheckPokeHideState(target) == BPP_CONTFLG_NULL)
  ){
    // ����Ɏ���ĂȂ���ΐ���
    if( !BPP_TURNFLAG_Get(target, BPP_TURNFLG_MAMORU) )
    {
      BPP_SICK_CONT  cont;
      u8 counterValue = BPP_PokeIDtoFreeFallCounter( BPP_GetID(target) );

      scPut_SetContFlag( wk, attacker, BPP_CONTFLG_SORAWOTOBU );
      scPut_SetContFlag( wk, target, BPP_CONTFLG_SORAWOTOBU );
      scPut_SetBppCounter( wk, attacker, BPP_COUNTER_FREEFALL, counterValue );

      cont = BPP_SICKCONT_MakePoke( BPP_GetID(attacker) );
      scproc_AddSickCore( wk, target, attacker, WAZASICK_FREEFALL, cont, FALSE, NULL );

      return TRUE;
    }
    // ������ꍇ�̓��b�Z�[�W�\��
    else
    {
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Mamoru, BPP_GetID(target) );
      *fFailMsgDisped = TRUE;
    }
  }
  return FALSE;
}
/**
 *  �t���[�t�H�[���i�U�����j�ŋ󒆂ɋ����Ԃ��ǂ�������
 */
static inline BOOL checkFreeFallUsing( const BTL_POKEPARAM* bpp )
{
  u8 capturedPokeID = BPP_FreeFallCounterToPokeID( BPP_COUNTER_Get(bpp, BPP_COUNTER_FREEFALL) );
//  if( BPP_FreeFallCounterToPokeID( BPP_COUNTER_Get(bpp, BPP_COUNTER_FREEFALL) != BTL_POKEID_NULL ) ){
  TAYA_Printf("pokeID-%d, captureCheck ... %d\n", BPP_GetID(bpp), capturedPokeID);
  if( capturedPokeID != BTL_POKEID_NULL ){
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �t���[�t�H�[���Ń|�P���������񂾏�ԂȂ烊���[�X���鏈��
 *
 * @param   fldSit
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_FreeFall_CheckRelease( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BOOL fReleaseProc )
{
  u8 capturedPokeID = BPP_FreeFallCounterToPokeID( BPP_COUNTER_Get(bpp, BPP_COUNTER_FREEFALL) );
  if( capturedPokeID != BTL_POKEID_NULL )
  {
    BTL_POKEPARAM* capturedBpp = BTL_POKECON_GetPokeParam( wk->pokeCon, capturedPokeID );

    if( BPP_CheckSick(capturedBpp, WAZASICK_FREEFALL) )
    {
      scPut_CureSick( wk, capturedBpp, WAZASICK_FREEFALL, NULL );
      scPut_ResetContFlag( wk, capturedBpp, BPP_CONTFLG_SORAWOTOBU );

      if( BTL_POSPOKE_IsExist(&wk->pospokeWork, capturedPokeID) ){
        SCQUE_PUT_ACT_TameWazaHide( wk->que, capturedPokeID, FALSE );
        scPut_Message_Set( wk, capturedBpp, BTL_STRID_SET_FreeFall_End );
      }
    }

    scPut_SetBppCounter( wk, bpp, BPP_COUNTER_FREEFALL, 0 );
    if( !fReleaseProc )
    {
      if( !BPP_IsDead(bpp) ){
        SCQUE_PUT_ACT_TameWazaHide( wk->que, BPP_GetID(bpp), FALSE );
      }
    }
  }
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
static BOOL scproc_Fight_CheckWazaExecuteFail_1st( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u8 fWazaLock )
{
  SV_WazaFailCause  cause = SV_WAZAFAIL_NULL;
  PokeSick sick = POKESICK_NULL;
  BOOL fWazaMelt = FALSE;

  do {

    // ���ߖ�������
    if( wk->currentSabotageType == SABOTAGE_DONT_ANY ){
      cause = SV_WAZAFAIL_SABOTAGE;
      break;
    }
    if( wk->currentSabotageType == SABOTAGE_GO_SLEEP ){
      cause = SV_WAZAFAIL_SABOTAGE_GO_SLEEP;
      break;
    }

     // PP���[��
    if( !fWazaLock )
    {
      u8 wazaIdx = BPP_WAZA_SearchIdx( attacker, waza );
      if( (wazaIdx != PTL_WAZA_MAX)
      &&  (BPP_WAZA_GetPP(attacker, wazaIdx) == 0)
      ){
        cause = SV_WAZAFAIL_PP_ZERO;
        break;
      }
    }

    // �˂ނ�E�����蓙�̉����`�F�b�N
    fWazaMelt = scproc_PokeSickCure_WazaCheck( wk, attacker, waza );

    // �|�P�����n ��Ԉُ�ɂ�鎸�s�`�F�b�N
    sick = BPP_GetPokeSick( attacker );
    if( sick == POKESICK_NEMURI ){
      if( !scEvent_ExeFailThrew(wk, attacker, waza, SV_WAZAFAIL_NEMURI) ){
        cause = SV_WAZAFAIL_NEMURI;
        break;
      }
    }
    if( (sick == POKESICK_KOORI) && (!fWazaMelt) ){
      cause = SV_WAZAFAIL_KOORI;
      break;
    }

    // �C�x���g���s�`�F�b�N���x���P
    cause = scEvent_CheckWazaExecute( wk, attacker, waza, BTL_EVENT_WAZA_EXECUTE_CHECK_1ST );
    if( cause != SV_WAZAFAIL_NULL ){  break; }


    // �Ђ�݂ɂ�鎸�s�`�F�b�N
    if( BPP_TURNFLAG_Get(attacker, BPP_TURNFLG_SHRINK) ){
      cause = SV_WAZAFAIL_SHRINK;
      break;
    }

    // ���Ȃ��΂蔻��
    if( BPP_CheckSick(attacker, WAZASICK_KANASIBARI) )
    {
      if( waza != WAZANO_WARUAGAKI )
      {
        BPP_SICK_CONT  sickCont = BPP_GetSickCont( attacker, WAZASICK_KANASIBARI );
        if( BPP_SICKCONT_GetParam(sickCont) == waza ){
          cause = SV_WAZAFAIL_KANASIBARI;
          break;
        }
      }
    }

    // �����ӂ��ӂ����`�F�b�N
    if( BPP_CheckSick(attacker, WAZASICK_KAIHUKUHUUJI)
    &&  (WAZADATA_GetFlag(waza, WAZAFLAG_KaifukuHuuji))
    ){
      cause = SV_WAZAFAIL_KAIHUKUHUUJI;
      break;
    }

    // ���イ��傭�`�F�b�N
    if( BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU)
    &&  (WAZADATA_GetFlag(waza, WAZAFLAG_Flying))
    ){
      cause = SV_WAZAFAIL_JURYOKU;
      break;
    }

    // ��邠�����ȊO���`�F�b�N
    if( waza != WAZANO_WARUAGAKI )
    {
      // �A���R�[���ɂ�鎸�s�`�F�b�N
      if( (BPP_CheckSick(attacker, WAZASICK_ENCORE))
      &&  (BPP_GetPrevOrgWazaID(attacker) != waza)
      ){
        cause = SV_WAZAFAIL_WAZALOCK;
        break;
      }
      // ���U���b�N�i�������j�ɂ�鎸�s�`�F�b�N
      if( (BPP_CheckSick(attacker, WAZASICK_WAZALOCK))
      &&  (BPP_GetPrevWazaID(attacker) != waza)
      ){
        cause = SV_WAZAFAIL_WAZALOCK;
        break;
      }
      // ���傤�͂ɂ�鎸�s�`�F�b�N
      if( BPP_CheckSick(attacker, WAZASICK_TYOUHATSU)
      &&  !WAZADATA_IsDamage(waza)
      ){
        cause = SV_WAZAFAIL_TYOUHATSU;
        break;
      }
      // ���������ɂ�鎸�s�`�F�b�N
      // 2010.06.06  GS�܂ł̋����Ƃ��ẮA����������搧�Ŏ󂯂Ă����U�͏o����炵���̂Ń`�F�b�N���͂���
      #if 0
      if( BPP_CheckSick(attacker, WAZASICK_ICHAMON)
      &&  (BPP_GetPrevOrgWazaID(attacker) == waza)
      ){
        cause = SV_WAZAFAIL_ICHAMON;
        break;
      }
      #endif
      // �ӂ�����ɂ�鎸�s�`�F�b�N
      if( BTL_FIELD_CheckEffect(BTL_FLDEFF_FUIN) )
      {
        if( BTL_FIELD_CheckFuin(wk->pokeCon, attacker, waza) )
        {
          cause = SV_WAZAFAIL_FUUIN;
          break;
        }
      }
    }/* waza != WAZANO_WARUAGAKI */


    // �����񎩔�����
    if( scproc_Fight_CheckConf(wk, attacker) ){
      cause = SV_WAZAFAIL_KONRAN;
      break;
    }

    // �}�q����
    if( sick == POKESICK_MAHI ){
      if( BTL_CALC_IsOccurPer(BTL_MAHI_EXE_PER) ){
       cause = SV_WAZAFAIL_MAHI;
        break;
      }
    }
    // ������������
    if( scproc_Fight_CheckMeroMero(wk, attacker) ){
      cause = SV_WAZAFAIL_MEROMERO;
      break;
    }

  }while( 0 );

  if( cause != SV_WAZAFAIL_NULL ){
    BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_1, BPP_GetID(attacker), cause);
    scproc_WazaExecuteFailed( wk, attacker, waza, cause );
    return TRUE;
  }
  else
  {
    // �����Ă�̂Ɏ��s����->����G�t�F�N�g�����\������
    if( sick == POKESICK_NEMURI )
    {
      scPut_WazaExecuteFailMsg( wk, attacker, waza, SV_WAZAFAIL_NEMURI );
      scPut_EffectByPokePos( wk, attacker, BTLEFF_NEMURI );
    }
    // ���U�ł����肪�n����
    else if( fWazaMelt && (sick == POKESICK_KOORI ) ){
      scPut_CurePokeSick( wk, attacker, sick, FALSE );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriMeltWaza, BPP_GetID(attacker), waza );
    }
  }
  return FALSE;
}
static BOOL scproc_Fight_CheckWazaExecuteFail_2nd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BOOL fWazaLock )
{
  if( waza != WAZANO_WARUAGAKI )
  {
    SV_WazaFailCause  cause;

    cause = scEvent_CheckWazaExecute( wk, attacker, waza, BTL_EVENT_WAZA_EXECUTE_CHECK_2ND );
    if( cause != SV_WAZAFAIL_NULL )
    {
      BTL_N_Printf( DBGSTR_SVFL_WazaExeFail_2, BPP_GetID(attacker), cause);
      scproc_WazaExecuteFailed( wk, attacker, waza, cause );
      return TRUE;
    }
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event]  ���胏�U�o�����s�v���𖳎����邩����
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 * @param   waza
 *
 * @retval  BOOL    ��������ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_ExeFailThrew( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
{
  BOOL fThrew = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_FAIL_CAUSE, cause );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_FAIL_THREW );
    fThrew = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return fThrew;
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z���U�o�����s����
 *
 * @param   wk
 * @param   attacker    �U���|�P�����p�����[�^
 * @param   waza        �o�����Ƃ��Ă��郏�U
 *
 * @retval  SV_WazaFailCause
 */
//--------------------------------------------------------------------------
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, BtlEventType eventID )
{
  SV_WazaFailCause cause ;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NULL );
    BTL_EVENT_CallHandlers( wk, eventID );
    cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
  BTL_EVENTVAR_Pop();

  return cause;
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
  if( wk->currentSabotageType == SABOTAGE_CONFUSE ){
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_Sabotage1, BPP_GetID(attacker) );
    return TRUE;
  }

  if( BPP_CheckSick(attacker, WAZASICK_KONRAN) )
  {
    if( BPP_CheckKonranWakeUp(attacker) ){
      scPut_CureSick( wk, attacker, WAZASICK_KONRAN, NULL );
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranCure, BPP_GetID(attacker) );
      return FALSE;
    }
    scPut_ConfCheck( wk, attacker );
    return BTL_CALC_IsOccurPer( BTL_CONF_EXE_RATIO );
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���������ɂ�郏�U�������s����
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  BOOL    ���s�����ꍇTRUE
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
 * �˂ނ�E�����蓙�A�A�N�V�����I����ɉ����`�F�b�N���s������
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static BOOL scproc_PokeSickCure_WazaCheck( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
  PokeSick sick = BPP_GetPokeSick( attacker );
  BOOL f_cured = FALSE;
  BOOL f_wazaMelt = FALSE;

  switch( sick ){
  case POKESICK_NEMURI:
    if( BPP_CheckNemuriWakeUp(attacker) ){
      f_cured = TRUE;
    }
    break;
  case POKESICK_KOORI:
    if( WAZADATA_GetFlag(waza, WAZAFLAG_KooriMelt) ){
      f_wazaMelt = TRUE;
    }
    else if( BTL_CALC_IsOccurPer(BTL_KORI_MELT_PER) )
    {
      f_cured = TRUE;
    }
    break;
  }

  if( f_cured ){
    scPut_CurePokeSick( wk, attacker, sick, TRUE );
  }

  return f_wazaMelt;
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
  // ������Ȃ玩���_���[�W�`�F�b�N
  if( fail_cause == SV_WAZAFAIL_KONRAN )
  {
    scproc_CalcConfDamage( wk, attacker );
    if( scproc_CheckDeadCmd(wk, attacker) ){
      if( scproc_CheckShowdown(wk) ){
        return;
      }
    }
  }
  else
  {
    // ���s���b�Z�[�W
    scPut_WazaExecuteFailMsg( wk, attacker, waza, fail_cause );

    // ���o
    switch( fail_cause ){
    case SV_WAZAFAIL_NEMURI:     scPut_EffectByPokePos( wk, attacker, BTLEFF_NEMURI ); break;
    case SV_WAZAFAIL_MAHI:       scPut_EffectByPokePos( wk, attacker, BTLEFF_MAHI ); break;
    case SV_WAZAFAIL_KOORI:      scPut_EffectByPokePos( wk, attacker, BTLEFF_KOORI ); break;
    case SV_WAZAFAIL_MEROMERO:   scPut_EffectByPokePos( wk, attacker, BTLEFF_MEROMERO ); break;

    case SV_WAZAFAIL_SABOTAGE_GO_SLEEP:
      {
        BPP_SICK_CONT cont = BTL_CALC_MakeDefaultPokeSickCont( POKESICK_NEMURI );
        scPut_AddSick( wk, attacker, POKESICK_NEMURI, cont );
        scPut_EffectByPokePos( wk, attacker, BTLEFF_NEMURI );
      }
      break;
    }
  }

  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    scEvent_CheckWazaExeFail( wk, attacker, waza, fail_cause );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }
}
//--------------------------------------------------------------------------
/**
 * ���U�o�����s���b�Z�[�W�\��
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
  case SV_WAZAFAIL_KANASIBARI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KanasibariWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_NAMAKE:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Namake, pokeID );
    break;
  case SV_WAZAFAIL_ENCORE:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Namake, pokeID );
    break;
  case SV_WAZAFAIL_TYOUHATSU:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChouhatuWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_ICHAMON:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_IchamonWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_FUUIN:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_FuuinWarn, pokeID, waza );
    break;
  case SV_WAZAFAIL_KAIHUKUHUUJI:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KaifukuFujiExe, pokeID, waza );
    break;
  case SV_WAZAFAIL_HPFULL:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HPFull, pokeID );
    break;
  case SV_WAZAFAIL_FUMIN:
    SCQUE_PUT_TOKWIN_IN( wk->que, pokeID );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Fumin, pokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, pokeID );
    break;
  case SV_WAZAFAIL_JURYOKU:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_JyuryokuWazaFail, pokeID, waza );
    break;
  case SV_WAZAFAIL_SABOTAGE:
    if( BPP_CheckSick(bpp, WAZASICK_NEMURI) ){
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_Sabotage_Sleeping, pokeID  );
    }else{
      u8 rnd = BTL_CALC_GetRand( 4 );
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_Sabotage1 + rnd, pokeID  );
    }
    break;
  case SV_WAZAFAIL_SABOTAGE_GO_SLEEP:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_Sabotage_GoSleep, pokeID  );
    break;

  case SV_WAZAFAIL_TOKUSEI:
  case  SV_WAZAFAIL_NO_REACTION:
    // �Ƃ������̏ꍇ�A�e�n���h���ɔC����
    break;

  default:
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail, pokeID );
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] �|�P���������J�E���^�l���Z�b�g
 *
 * @param   wk
 * @param   bpp
 * @param   counterID
 * @param   value
 */
//----------------------------------------------------------------------------------
static void scPut_SetBppCounter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppCounter counterID, u8 value )
{
  SCQUE_PUT_OP_SetPokeCounter( wk->que, BPP_GetID(bpp), counterID, value );
  BPP_COUNTER_Set( bpp, counterID, value );
}

//--------------------------------------------------------------------------
/**
 * [Proc] �g�������U��PP����
 *
 * @param   wk
 * @param   attacker  �U���|�P�����p�����[�^
 * @param   wazaIdx   �g�������U�C���f�b�N�X
 * @param   rec       �_���[�W���󂯂��|�P�����p�����[�^�Q
 *
 */
//--------------------------------------------------------------------------
static void scproc_decrementPPUsedWaza( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID waza, u8 wazaIdx, BTL_POKESET* rec )
{
  u8 vol = scEvent_DecrementPPVolume( wk, attacker, wazaIdx, waza, rec );
  scproc_decrementPP( wk, attacker, wazaIdx, vol );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�g�p��̃f�N�������g����PP�l���擾
 *
 * @param   wk
 * @param   attacker  ���U�g�p�|�P����
 * @param   rec       ���U�ɂ���ă_���[�W���󂯂��|�P�����p�����[�^�Q
 *
 * @retval  u32       �f�N�������gPP�l
 */
//----------------------------------------------------------------------------------
static u32 scEvent_DecrementPPVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  u8 wazaIdx, WazaID waza, BTL_POKESET* rec )
{
  u32 volume = 1;

  BTL_EVENTVAR_Push();
    {
      u32 i = 0;
      BTL_POKEPARAM* bpp;
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, BTL_POKESET_GetCount(rec) );

      while( (bpp=BTL_POKESET_Get(rec, i)) != NULL )
      {
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
        ++i;
      }
    }
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_IDX, wazaIdx );
//    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE ); // �^���U���[�N�Ώۃt���O�Ƃ���
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP );
    volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );

  BTL_EVENTVAR_Pop();

  return volume;
}
//----------------------------------------------------------------------------------
/**
 * [Proc] ���U��PP����
 *
 * @param   wk
 * @param   bpp       �Ώۃ|�P�����p�����[�^
 * @param   wazaIdx   ���������郏�U�C���f�b�N�X
 * @param   volume    ������
 *
 * @retval  BOOL      ������������TRUE�^���S���Ȃǌ����������Ȃ�������FALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume )
{
  u8 restPP;

  restPP = BPP_WAZA_GetPP( bpp, wazaIdx );

  if( volume >= restPP ){
    volume = restPP;
  }

  if( volume )
  {
    scPut_DecrementPP( wk, bpp, wazaIdx, volume );
    return TRUE;
  }

  return FALSE;
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
  if( !BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_PP_CONST) )
  {
    u8 pokeID = BPP_GetID( attacker );

    BPP_WAZA_DecrementPP( attacker, wazaIdx, vol );
    SCQUE_PUT_OP_PPMinus( wk->que, pokeID, wazaIdx, vol );

    BPP_WAZA_SetUsedFlag( attacker, wazaIdx );
    SCQUE_PUT_OP_WazaUsed( wk->que, pokeID, wazaIdx );
  }
}

//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U
//----------------------------------------------------------------------
static void scproc_Fight_Damage_Root( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
   BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_DMGAFF_REC* affRec, BOOL fDelayAttack )
{
  u32  dmg_sum;

  FlowFlg_Clear( wk, FLOWFLG_SET_WAZAEFFECT );
  scproc_Fight_DamageProcStart( wk, attacker, wazaParam );

  // �_���[�W�󂯃|�P�������[�N���N���A���Ă���
  BTL_POKESET_Clear( wk->psetDamaged );

  scEvent_HitCheckParam( wk, attacker, wazaParam->wazaID, wk->hitCheckParam );

  dmg_sum = 0;

  if( HITCHECK_IsPluralHitWaza(wk->hitCheckParam)
  &&  (BTL_POKESET_GetCountMax(targets) == 1)
  ){
    dmg_sum = scproc_Fight_Damage_PluralCount( wk, wazaParam, attacker, targets, affRec );
  }
  else{
    dmg_sum = scproc_Fight_Damage_SingleCount( wk, wazaParam, attacker, targets, affRec, fDelayAttack );
  }

  if( dmg_sum ){
    scproc_Fight_Damage_Kickback( wk, attacker, wazaParam->wazaID, dmg_sum );
  }

  scproc_Fight_DamageProcEnd( wk, wazaParam, attacker, wk->psetDamaged, dmg_sum, fDelayAttack );
}

//----------------------------------------------------------------------------------
/**
 * �_���[�W�v�Z���ʂ����[�N�Ɏ擾
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   wazaParam
 * @param   affRec
 * @param   dmg_ratio
 * @param   dmgRec
 */
//----------------------------------------------------------------------------------
static void BTL_CALCDAMAGE_Set( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKESET* target,
  const SVFL_WAZAPARAM* wazaParam, const BTL_DMGAFF_REC* affRec, fx32 dmg_ratio, BTL_CALC_DAMAGE_REC* dmgRec )
{
  BTL_POKEPARAM* bpp;
  u32 c;
  u16 damage;

  dmgRec->real_hit_count = 0;
  dmgRec->migawari_hit_count = 0;
  c = 0;

  BTL_POKESET_SeekStart( target );
  while( (bpp = BTL_POKESET_SeekNext(target) ) != NULL )
  {
    dmgRec->record[c].pokeID = BPP_GetID( bpp );
    dmgRec->record[c].fCritical = scEvent_CheckCritical( wk, attacker, bpp, wazaParam->wazaID );
    dmgRec->record[c].affine = BTL_DMGAFF_REC_Get( affRec, dmgRec->record[c].pokeID );
    dmgRec->record[c].fFixDamage = scEvent_CalcDamage( wk, attacker, bpp, wazaParam, dmgRec->record[c].affine,
        dmg_ratio, dmgRec->record[c].fCritical, FALSE, &damage );
    dmgRec->record[c].damage = damage;

    // �Œ�_���[�W�Ȃ瑊�����{�A�N���e�B�J�������Ƃ��ď���
    if( dmgRec->record[c].fFixDamage )
    {
      dmgRec->record[c].fCritical = FALSE;
      dmgRec->record[c].affine = BTL_TYPEAFF_100;
    }

    dmgRec->record[c].fMigawari = BPP_MIGAWARI_IsExist( bpp );
    if( dmgRec->record[c].fMigawari == FALSE )
    {
      u16 damage = MarumeDamage( bpp, dmgRec->record[c].damage );
      dmgRec->record[c].koraeru = scEvent_CheckKoraeru( wk, attacker, bpp, TRUE, &damage );
      dmgRec->record[c].damage = damage;
      dmgRec->real_hit_count++;
    }
    else
    {
      dmgRec->record[c].damage = damage;
      dmgRec->record[c].koraeru = BPP_KORAE_NONE;
      dmgRec->migawari_hit_count++;
    }
//    TAYA_Printf("�_���[�W�L�^�F�|�P(%d)�ɑ΂���_���[�W=%d\n", BPP_GetID(bpp), dmgRec->record[c].damage );

    AffCounter_CountUp( &wk->affCounter, wk, attacker, bpp, dmgRec->record[c].affine );

    ++c;
  }
  dmgRec->total_hit_count = c;

  #if 0
  BTL_POKEPARAM* bpp;

  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets) ) != NULL )
  {
  }
    bpp[i] = BTL_POKESET_Get( targets, i );
    critical_flg[i] = scEvent_CheckCritical( wk, attacker, bpp[i], wazaParam->wazaID );
    fFixDamage = scEvent_CalcDamage( wk, attacker, bpp[i], wazaParam, affAry[i], dmg_ratio, critical_flg[i], TRUE, &dmg[i] );
    if( fFixDamage ){
      affAry[i] = BTL_TYPEAFF_100;
      critical_flg[i] = FALSE;
    }
    dmg_tmp = dmg[i];
    if( !BPP_MIGAWARI_IsExist(bpp[i]) ){
      dmg[i] = MarumeDamage( bpp[i], dmg[i] );
      koraeru_cause[i] = scEvent_CheckKoraeru( wk, attacker, bpp[i], TRUE, &dmg[i] );
    }
    dmg_sum += dmg[i];
    if( dmg_tmp ){
      AffCounter_CountUp( &wk->affCounter, wk, attacker, bpp[i], affAry[i] );
    }
#endif
}
//----------------------------------------------------------------------------------
/**
 * �_���[�W�v�Z���ʂɊi�[���ꂽ�^�[�Q�b�g�|�P���������擾�i���́E�݂����̍��v�j
 *
 * @param   dmgRec
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 BTL_CALCDAMAGE_GetCount( const BTL_CALC_DAMAGE_REC* dmgRec )
{
  return dmgRec->total_hit_count;
}
//----------------------------------------------------------------------------------
/**
 * �݂����q�b�g�����|�P�����̏��݂̂�z��ɏo��
 *
 * @retval  u32   �݂����q�b�g�����|�P������
 */
//----------------------------------------------------------------------------------
static u32 BTL_CALCDAMAGE_GetMigawariHitPoke( BTL_SVFLOW_WORK* wk, const BTL_CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, u8* criticalFlagAry )
{
  u32 i, c;
  for(i=0, c=0; i<rec->total_hit_count; ++i)
  {
    if( rec->record[i].fMigawari )
    {
      bppAry[c] = BTL_POKECON_GetPokeParam( wk->pokeCon, rec->record[i].pokeID );
      dmgAry[c] = rec->record[i].damage;
      affAry[c] = rec->record[i].affine;
      criticalFlagAry[c] = rec->record[i].fCritical;
      ++c;
    }
  }
  return c;
}
//----------------------------------------------------------------------------------
/**
 * �݂����E���̖�킸�q�b�g�����|�P�����̏���z��ɏo��
 *
 * @retval  u32   �q�b�g�����|�P������
 */
//----------------------------------------------------------------------------------
static u32 BTL_CALCDAMAGE_GetAllHitPoke( BTL_SVFLOW_WORK* wk, const BTL_CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry )
{
  u32 i;
  for(i=0; i<rec->total_hit_count; ++i)
  {
    bppAry[i] = BTL_POKECON_GetPokeParam( wk->pokeCon, rec->record[i].pokeID );
  }
  return rec->total_hit_count;
}
//----------------------------------------------------------------------------------
/**
 * ���̃q�b�g�����|�P�����̏��݂̂�z��ɏo��
 *
 * @retval  u32   �q�b�g�����|�P������
 */
//----------------------------------------------------------------------------------
static u32 BTL_CALCDAMAGE_GetRealHitPoke( BTL_SVFLOW_WORK* wk, const BTL_CALC_DAMAGE_REC* rec,
  BTL_POKEPARAM** bppAry, u16* dmgAry, BtlTypeAff* affAry, u8* criticalFlagAry, u8* koraeCauseAry )
{
  u32 i, c;
  for(i=0, c=0; i<rec->total_hit_count; ++i)
  {
    if( rec->record[i].fMigawari == FALSE )
    {
      bppAry[c] = BTL_POKECON_GetPokeParam( wk->pokeCon, rec->record[i].pokeID );
      dmgAry[c] = rec->record[i].damage;
      affAry[c] = rec->record[i].affine;
      criticalFlagAry[c] = rec->record[i].fCritical;
      koraeCauseAry[c] = rec->record[i].koraeru;;
      ++c;
    }
  }
  return c;
}
//----------------------------------------------------------------------------------
/**
 * �_���[�W���v���擾
 *
 * @param   rec
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 BTL_CALCDAMAGE_GetDamageSum( const BTL_CALC_DAMAGE_REC* rec )
{
  u32 i, sum = 0;
  for(i=0; i<rec->total_hit_count; ++i)
  {
    sum += rec->record[i].damage;
  }
  return sum;
}



//------------------------------------------------------------------------------
/**
* �P��q�b�g���U�i�Ώۂ͂P�̈ȏ�j
*/
//------------------------------------------------------------------------------
static u32 scproc_Fight_Damage_SingleCount( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_DMGAFF_REC* affRec, BOOL fDelayAttack )
{
  FLAG_SET flagSet;
  fx32 dmg_ratio = (BTL_POKESET_GetCount(wk->psetTargetOrg) == 1)? BTL_CALC_DMG_TARGET_RATIO_NONE : BTL_CALC_DMG_TARGET_RATIO_PLURAL;
  u32 dmg_sum = 0;


  // �����Ώۂ̃��U������
  flagSet.hitPluralPoke = (BTL_POKESET_GetCount(targets) > 1);
  flagSet.delayAttack = fDelayAttack;

  // �G�E������ʁX��Set�ɃR�s�[
  BTL_POKESET_CopyFriends( targets, attacker, wk->psetFriend );
  BTL_POKESET_CopyEnemys( targets, attacker, wk->psetEnemy );

  // �_���[�W�v�Z���ʂ����[�N�ɕۑ�
  BTL_CALCDAMAGE_Set( wk, attacker, wk->psetFriend, wazaParam, affRec, dmg_ratio, wk->calcDmgFriend );
  BTL_CALCDAMAGE_Set( wk, attacker, wk->psetEnemy, wazaParam, affRec, dmg_ratio, wk->calcDmgEnemy );

//  scproc_SingleCount_DamageDetermine( wk, attacker,

  if( BTL_POKESET_GetCount( wk->psetFriend ) )
  {
    dmg_sum += scproc_Fight_Damage_side( wk, wazaParam, attacker, wk->psetFriend, wk->calcDmgFriend,
                  wk->hitCheckParam, dmg_ratio, flagSet );
    if( dmg_sum ){
      BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_TEMOTI_MAKIZOE );
    }
  }
  if( BTL_POKESET_GetCount( wk->psetEnemy ) )
  {
    dmg_sum += scproc_Fight_Damage_side( wk, wazaParam, attacker, wk->psetEnemy, wk->calcDmgEnemy,
      wk->hitCheckParam, dmg_ratio, flagSet );
  }

  wazaEffCtrl_SetEnable( wk->wazaEffCtrl );

  return dmg_sum;
}
//------------------------------------------------------------------------------
/**
* ������q�b�g���U�i�Ώۂ͂P�̂̂݁j
*/
//------------------------------------------------------------------------------
static u32 scproc_Fight_Damage_PluralCount( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_DMGAFF_REC* affRec )
{
  BTL_POKEPARAM* bpp;
  u32 i, hitCount;
  u32 dmg_sum = 0;
  WazaSick  pokeSick;
  BtlPokePos  targetPos = BTL_POS_NULL;
  FLAG_SET flagSet;

  bpp = BTL_POKESET_Get( targets, 0 );
  wk->thruDeadMsgPokeID = BPP_GetID( bpp );

  // ������q�b�g�̓G�t�F�N�g�ɃJ�������o����ꂸ�A�Y�[���C���R�}���h�𐶐�����
  if( BTL_MAIN_IsWazaEffectEnable(wk->mainModule) )
  {
    targetPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, BPP_GetID(bpp) );
    if( targetPos != BTL_POS_NULL ){
      SCQUE_PUT_ACT_EffectByPos( wk->que, targetPos, BTLEFF_ZOOM_IN );
    }
  }

  flagSet.raw = 0;

  for(i=0, hitCount=0; i<wk->hitCheckParam->countMax; ++i)
  {
    pokeSick = BPP_GetPokeSick( attacker );

    // ���U�G�t�F�N�g�R�}���h����
    SCQUE_PUT_ACT_WazaEffect( wk->que,
        wk->wazaEffCtrl->attackerPos, wk->wazaEffCtrl->targetPos, wazaParam->wazaID, 0 );


    BTL_CALCDAMAGE_Set( wk, attacker, targets, wazaParam, affRec, BTL_CALC_DMG_TARGET_RATIO_NONE, wk->calcDmgEnemy );

    dmg_sum += scproc_Fight_Damage_side( wk, wazaParam, attacker, targets, wk->calcDmgEnemy,
                    wk->hitCheckParam, BTL_CALC_DMG_TARGET_RATIO_NONE, flagSet );
    ++hitCount;

    if( BPP_IsDead(bpp) ){ break; }
    if( BPP_IsDead(attacker) ){ break; }

    // �U���������Ƃɂ�薰���Ă��܂�����u���C�N
    if( (BPP_GetPokeSick(attacker) == POKESICK_NEMURI)
    &&  (pokeSick != POKESICK_NEMURI)
    ){
      break;
    }
    // ����q�b�g�`�F�b�N���s�����U�͊O�ꂽ��u���C�N
    if( wk->hitCheckParam->fCheckEveryTime ){
      if( !scEvent_CheckHit(wk, attacker, bpp, wazaParam) ){ break; }
    }


  }

  if( hitCount>0 ){
    scPut_DamageAff( wk, wk->hitCheckParam->pluralHitAffinity );
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_Hit_PluralTimes, hitCount );
  }

  if( BPP_IsDead(bpp) ){
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, BPP_GetID(bpp) );
  }

  // �J�����Y�[���C�������Z�b�g
  if( targetPos != BTL_POS_NULL ){
    SCQUE_PUT_ACT_EffectSimple( wk->que, BTLEFF_CAMERA_INIT );
  }

  wk->thruDeadMsgPokeID = BTL_POKEID_NULL;

  return dmg_sum;
}

/**
 *  �ŏ��̃q�b�g������
 */
static BOOL HITCHECK_IsFirstTime( const HITCHECK_PARAM* param )
{
  return param->count == 0;
}
/**
 *  ������q�b�g���郏�U������
 */
static BOOL HITCHECK_IsPluralHitWaza( const HITCHECK_PARAM* param )
{
  return param->fPluralHitWaza;
}
/**
 *
 */
static BOOL HITCHECK_CheckWazaEffectPuttable( HITCHECK_PARAM* param )
{
  if( param->fPutEffectCmd == FALSE ){
    param->fPutEffectCmd = TRUE;
    return TRUE;
  }
  return FALSE;
}
/**
 *
 */
static void HITCHECK_SetPluralHitAffinity( HITCHECK_PARAM* param, BtlTypeAff affinity )
{
  if( param->pluralHitAffinity == BTL_TYPEAFF_100 ){
    param->pluralHitAffinity = affinity;
  }
}

//----------------------------------------------------------------------------------
/**
 * �P�w�c�ɑ΂���_���[�W����
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   targets
 * @param   dmg_ratio
 *
 * @retval  u32  ���������_���[�W��
 */
//----------------------------------------------------------------------------------
static u32 scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker,
  BTL_POKESET* targets, const BTL_CALC_DAMAGE_REC* dmgRec, HITCHECK_PARAM* hitCheckParam, fx32 dmg_ratio, FLAG_SET flagSet )
{
  BtlTypeAff affAry[ BTL_POSIDX_MAX ];
  BTL_POKEPARAM* bpp;
  u8 damaged_poke_cnt;

  // �R�C�c�͌��݁u�v���[���g�v�ł����g���Ă��Ȃ�
  scproc_Fight_Damage_ToRecover( wk, attacker, wazaParam, targets );

  damaged_poke_cnt = BTL_CALCDAMAGE_GetCount( dmgRec );

  if( damaged_poke_cnt )
  {
    u32 dmg_sum = scproc_Fight_damage_side_core( wk, attacker, targets, dmgRec, wazaParam,
                  hitCheckParam, dmg_ratio, flagSet );
    scproc_WazaDamageSideAfter( wk, attacker, wazaParam, dmg_sum );
    return dmg_sum;
  }

  return 0;
}
/**
 *  �P�w�c�_���[�W�����R�A
 */
static u32 scproc_Fight_damage_side_core( BTL_SVFLOW_WORK* wk,
    BTL_POKEPARAM* attacker, BTL_POKESET* targets, const BTL_CALC_DAMAGE_REC* dmgRec,
    const SVFL_WAZAPARAM* wazaParam, HITCHECK_PARAM* hitCheckParam, fx32 dmg_ratio, FLAG_SET flagSet )
{
  static BTL_POKEPARAM *bpp[ BTL_POSIDX_MAX ];
  static u16 dmg[ BTL_POSIDX_MAX ];
  static BtlTypeAff affAry[ BTL_POSIDX_MAX ];
  static u8  critical_flg[ BTL_POSIDX_MAX ];
  static u8  koraeru_cause[ BTL_POSIDX_MAX ];

  u32 dmg_sum, dmg_tmp;
  u8  poke_cnt, fFixDamage;
  int i;
  BtlPokePos atkPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, BPP_GetID(attacker) );

  dmg_sum = BTL_CALCDAMAGE_GetDamageSum( dmgRec );

  // �g����_���[�W����
  poke_cnt = BTL_CALCDAMAGE_GetMigawariHitPoke( wk, dmgRec, bpp, dmg, affAry, critical_flg );
  for(i=0; i<poke_cnt; ++i)
  {
    if( BPP_MIGAWARI_IsExist(bpp[i]) )
    {
      u16 add_damage = scproc_Migawari_Damage( wk, attacker, bpp[i], dmg[i], affAry[i], critical_flg[i], wazaParam );
      BTL_POKESET_AddWithDamage( wk->psetDamaged, bpp[i], 0 );  // �_���[�W0�Ƃ��ċL�^
      dmg_sum -= (dmg[i] - add_damage);
    }
  }

  // �_���[�W�m��C�x���g�R�[���i������q�b�g���U�Ȃ珉��̂݁j
  if( HITCHECK_IsFirstTime(hitCheckParam) )
  {
    poke_cnt = BTL_CALCDAMAGE_GetAllHitPoke( wk, dmgRec, bpp );
    for(i=0; i<poke_cnt; ++i){
      scproc_Fight_Damage_Determine( wk, attacker, bpp[i], wazaParam );
    }
  }
  hitCheckParam->count++;

  // ���̃q�b�g���������R�s�[�i�݂���肾���������炱���Ń��^�[���j
  poke_cnt = BTL_CALCDAMAGE_GetRealHitPoke( wk, dmgRec, bpp, dmg, affAry, critical_flg, koraeru_cause );
  if( poke_cnt == 0 ){
    return dmg_sum;
  }

  // �_���[�W�R�}���h�o��
  scproc_PrevWazaDamage( wk, wazaParam, attacker, poke_cnt, bpp );
  scPut_WazaDamagePlural( wk, wazaParam, poke_cnt, affAry, bpp, dmg, critical_flg, flagSet.hitPluralPoke );

  if( !HITCHECK_IsPluralHitWaza(hitCheckParam) ){
    // �P��q�b�g�Ȃ瑊�����b�Z�[�W���\��
    scPut_WazaAffinityMsg( wk, poke_cnt, affAry, bpp, flagSet.hitPluralPoke );
  }else{
    // ������q�b�g���U�̏ꍇ�͑����l���L�^���邾��
    HITCHECK_SetPluralHitAffinity( hitCheckParam, affAry[0] );
  }
  scPut_CriticalMsg( wk, poke_cnt, bpp, critical_flg, flagSet.hitPluralPoke );

  // �_���[�W�L�^
  for(i=0; i<poke_cnt; ++i)
  {
    BTL_POKESET_AddWithDamage( wk->psetDamaged, bpp[i], dmg[i] );
    wazaDmgRec_Add( wk, atkPos, attacker, bpp[i], wazaParam, dmg[i] );
    BPP_TURNFLAG_Set( bpp[i], BPP_TURNFLG_DAMAGED );
  }

  // ���炦������
  for(i=0; i<poke_cnt; ++i)
  {
    if( koraeru_cause[i] != BPP_KORAE_NONE ){
      scproc_Koraeru( wk, bpp[i], koraeru_cause[i] );
    }
  }

  // �ǉ����ʁA���A�N�V��������
  for(i=0; i<poke_cnt; ++i)
  {
    scproc_CheckShrink( wk, wazaParam, attacker, bpp[i] );
    scproc_Damage_Drain( wk, wazaParam, attacker, bpp[i], dmg[i] );
    scproc_WazaAdditionalEffect( wk, wazaParam, attacker, bpp[i], dmg[i], FALSE );
    scproc_WazaDamageReaction( wk, attacker, bpp[i], wazaParam, affAry[i], dmg[i], critical_flg[i], FALSE );
    scproc_CheckItemReaction( wk, bpp[i], BTL_ITEMREACTION_HP );
  }

  // �Ђ񂵃`�F�b�N
  for(i=0; i<poke_cnt; ++i){
    scproc_CheckDeadCmd( wk, bpp[i] );
  }
  scproc_CheckDeadCmd( wk, attacker );

  return dmg_sum;
}
//----------------------------------------------------------------------------------
/**
 * �_���[�W�������i���񕜉��j���Ă��܂��|�P�����̏����B������A�Y���|�P������targets���珜�O
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 * @param   targets
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_Damage_ToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;

  // �񕜃`�F�b�N -> ����^�C�v�̃_���[�W�𖳌����A�񕜂��Ă��܂��|�P�� targets ���珜�O
  BTL_POKESET_SeekStart( targets );
  while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
  {
    if( scEvent_CheckDmgToRecover(wk, attacker, bpp, wazaParam) )
    {
      u32 hem_state;

      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );

      hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_DmgToRecover( wk, attacker, bpp );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

      BTL_Hem_PopState( &wk->HEManager, hem_state );
      BTL_POKESET_Remove( targets, bpp );
    }
  }
}

/**
 *  ���ʂ́������@���b�Z�[�W�o��
 */
static void scPut_DamageAff( BTL_SVFLOW_WORK* wk, BtlTypeAff affinity )
{
  if( affinity < BTL_TYPEAFF_100 ){
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_AffBad );
  }
  else if(affinity > BTL_TYPEAFF_100){
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_AffGood );
  }
}
/**
 * �ō��Ŏc��g�o�͈̔͂Ɏ��܂�悤�Ƀ_���[�W�ŏI�␳
 */
static u32 MarumeDamage( const BTL_POKEPARAM* bpp, u32 damage )
{
  u32 hp = BPP_GetValue( bpp, BPP_HP );
  if( damage > hp ){
    damage = hp;
  }
  return damage;
}

//----------------------------------------------------------------------------------
/**
 * ���U�ɂ��_���[�W��^���邱�Ƃ��m�肵���i�݂������܂ށj
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 * @param   targets
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_Damage_Determine( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageDetermine( wk, attacker, defender, wazaParam );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��_���[�W��^���邱�Ƃ��m�肵��
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaDamageDetermine( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_DETERMINE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�E���������ɂ��_���[�W���u���炦��v���ǂ����`�F�b�N
 * ���u���炦��v= HP��1�c��
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   fWazaDamage   ���U�ɂ��_���[�W�̏ꍇTRUE�^�����_���[�W�Ȃ�FALSE���w��
 * @param   damage
 *
 * @retval  BppKoraeruCause
 */
//----------------------------------------------------------------------------------
static BppKoraeruCause scEvent_CheckKoraeru( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BOOL fWazaDamage, u16* damage )
{
  if( BPP_GetValue(defender, BPP_HP) > (*damage) )
  {
    return BPP_KORAE_NONE;
  }
  else
  {
    BppKoraeruCause  cause = BPP_KORAE_NONE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_GEN_FLAG,  fWazaDamage );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_KORAERU_CAUSE, cause );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_KORAERU_CHECK );
      cause = BTL_EVENTVAR_GetValue( BTL_EVAR_KORAERU_CAUSE );
    BTL_EVENTVAR_Pop();

    if( cause != BPP_KORAE_NONE ){
      *damage = BPP_GetValue(defender, BPP_HP) - 1;
    }

    return cause;
  }
}
//----------------------------------------------------------------------------------
/**
 * �u���炦��v�����\�����A�C�e������̏���
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scproc_Koraeru( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause )
{
  u8 pokeID = BPP_GetID( bpp );

  switch( cause ){
  case BPP_KORAE_WAZA_DEFENDER:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Koraeru_Exe, pokeID );
    break;

  case BPP_KORAE_ITEM:
  default:
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_KoraeruExe( wk, bpp, cause );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���炦�铮�씭��
 *
 * @param   wk
 * @param   bpp
 * @param   cause
 */
//----------------------------------------------------------------------------------
static void scEvent_KoraeruExe( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppKoraeruCause cause )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_KORAERU_EXE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W���o����i�݂����͏��O����Ă���j
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   poke_cnt
 * @param   bppAry
 */
//----------------------------------------------------------------------------------
static void scproc_PrevWazaDamage( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  scEvent_PrevWazaDamage( wk, wazaParam, attacker, poke_cnt, bppAry );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�_���[�W���o����i�݂����͏��O����Ă���j
 *
 * @param   wk
 * @param   wazaParam
 * @param   attacker
 * @param   poke_cnt
 * @param   bppAry
 */
//----------------------------------------------------------------------------------
static void scEvent_PrevWazaDamage( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, u32 poke_cnt, BTL_POKEPARAM** bppAry )
{
  u32 i;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, poke_cnt );
    for(i=0; i<poke_cnt; ++i)
    {
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bppAry[i]) );
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_PREV_WAZA_DMG );
  BTL_EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W���R�[�h��ǉ�
 *
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   damage
 */
//----------------------------------------------------------------------------------
static void wazaDmgRec_Add( BTL_SVFLOW_WORK* wk, BtlPokePos atkPos, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, u16 damage )
{
  BPP_WAZADMG_REC rec;
  u8 atkPokeID = BPP_GetID( attacker );
  u8 defPokeID = BPP_GetID( defender );

  BPP_WAZADMG_REC_Setup( &rec, atkPokeID, atkPos, wazaParam->wazaID, wazaParam->wazaType, damage );
  BPP_WAZADMGREC_Add( defender, &rec );

  SCQUE_OP_AddWazaDmgRec( wk->que, defPokeID, atkPokeID, atkPos, wazaParam->wazaType, wazaParam->wazaID, damage );
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
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage, BOOL fMigawriHit )
{
  WazaCategory category = WAZADATA_GetCategory( wazaParam->wazaID );

  switch( category ){
  case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
    scproc_Fight_Damage_AddEffect( wk, wazaParam, attacker, attacker );
    break;
  case WAZADATA_CATEGORY_DAMAGE_EFFECT:
    if( !fMigawriHit ){
      scproc_Fight_Damage_AddEffect( wk, wazaParam, attacker, defender );
    }
    break;
  case WAZADATA_CATEGORY_DAMAGE_SICK:
    if( !fMigawriHit ){
      scproc_Fight_Damage_AddSick( wk, wazaParam, attacker, defender );
    }
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W�P��P�w�c���̏I������
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
static void scproc_WazaDamageSideAfter( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, u32 damage )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageSideAfter( wk, attacker, wazaParam, damage );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�_���[�W��^���I�������̒ǉ�����
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza
 * @param   aff
 * @param   damage
 *
 */
//--------------------------------------------------------------------------
static void scEvent_WazaDamageSideAfter( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u32 damage )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZADMG_SIDE_AFTER );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * �A�C�e�������`�F�b�N
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_CheckItemReaction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlItemReaction reactionType )
{
  if( BPP_GetItem(bpp) != ITEM_DUMMY_DATA )
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

    BTL_N_Printf( DBGSTR_SVFL_CheckItemReaction, BPP_GetID(bpp));

    scEvent_CheckItemReaction( wk, bpp, reactionType );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event]�A�C�e�������`�F�b�N
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckItemReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlItemReaction reactionType )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM_REACTION, reactionType );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_ITEM_REACTION );
  BTL_EVENTVAR_Pop();
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W���U�V�[�P���X�J�n
//------------------------------------------------------------------
static void scproc_Fight_DamageProcStart( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  scEvent_DamageProcStart( wk, attacker, wazaParam );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W���U�V�[�P���X�I��
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   wazaParam   ���U�p�����[�^
 * @param   attacker    �U���|�P
 * @param   targets     �_���[�W���󂯂��|�P�����Q�i�݂���菜���j
 * @param   dmg_sum     �݂���蕪���܂߂��_���[�W�g�[�^��
 */
//----------------------------------------------------------------------------------
static void scproc_Fight_DamageProcEnd( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets, u32 dmgTotal, BOOL fDelayAttack )
{
//  scproc_Fight_Damage_Drain( wk, wazaParam->wazaID, attacker, targets );
  scproc_Fight_Damage_KoriCure( wk, wazaParam, attacker, targets );

  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

    scEvent_DamageProcEnd( wk, attacker, targets, wazaParam, fDelayAttack );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );

    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏��� > �Ђ�݃`�F�b�N
//------------------------------------------------------------------
static void scproc_CheckShrink( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender )
{
  u32 waza_per = scEvent_GetWazaShrinkPer( wk, wazaParam->wazaID, attacker );

//  if( !BPP_TURNFLAG_Get(defender, BPP_TURNFLG_ACTION_DONE) )
  {
    scproc_AddShrinkCore( wk, defender, waza_per );
  }
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏��� > �ق̂����U�Łu������v�����鏈��
// �ǉ����ʏ����̌�ɒu�����ƁB�łȂ��Ɓu�����肪�Ƃ����v���u�₯�ǂɂȂ����v���N���Ă��܂��B
//------------------------------------------------------------------
static void scproc_Fight_Damage_KoriCure( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  if( wazaParam->wazaType == POKETYPE_HONOO )
  {
    BTL_POKEPARAM* bpp;

    BTL_POKESET_SeekStart( targets );
    while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
    {
      if( (BPP_GetPokeSick(bpp) == POKESICK_KOORI)
      &&  (!BPP_MIGAWARI_IsExist(bpp))
      ){
        scPut_CurePokeSick( wk, bpp, POKESICK_KOORI, TRUE );
      }
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �Ώۃ|�P�����ɂЂ�݌��ʂ�^����
 *
 * @param   wk
 * @param   target
 * @param   per     �m���i�p�[�Z���e�[�W�j
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_AddShrinkCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, u32 per )
{
  BOOL fShrink;


  if( BPP_TURNFLAG_Get(target, BPP_TURNFLG_MUST_SHRINK) ){
    fShrink = TRUE;
  }else{
    fShrink = scEvent_CheckShrink( wk, target, per );
  }

  if( fShrink )
  {
    BPP_TURNFLAG_Set( target, BPP_TURNFLG_SHRINK );
    return TRUE;
  }
  else if( per >= 100 )
  {
    // �m��100���Ȃ̂Ɏ��s�����猴���\����
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    scEvent_FailShrink( wk, target );
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }
  return FALSE;
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�_���[�W�ɉ�����HP�z��������
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  u32 total_damage = 0;
  u32 hem_state;

  if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_DRAIN )
  {
    BTL_POKEPARAM* bpp;
    u32 damage, recoverHP;
    u32 i;

    hem_state = BTL_Hem_PushState( &wk->HEManager );
    BTL_POKESET_SeekStart( targets );
    while( (bpp = BTL_POKESET_SeekNext(targets)) != NULL )
    {
      damage = BTL_POKESET_GetDamage( targets, bpp );
      total_damage += damage;
      recoverHP = (WAZADATA_GetParam(waza, WAZAPARAM_DAMAGE_RECOVER_RATIO) * damage) / 100;

      if( recoverHP > 0 )
      {
        if( scproc_DrainCore(wk, attacker, bpp, recoverHP) )
        {
          scPut_Message_Set( wk, bpp, BTL_STRID_SET_Drain );
        }
      }
    }
  }
}

static void scproc_Damage_Drain( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
  if( WAZADATA_GetCategory(wazaParam->wazaID) == WAZADATA_CATEGORY_DRAIN )
  {
    u32 ratio = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_DAMAGE_RECOVER_RATIO );
    u32 recoverHP = BTL_CALC_MulRatioInt( damage, ratio );

    if( scproc_DrainCore(wk, attacker, defender, recoverHP) )
    {
      scPut_Message_Set( wk, defender, BTL_STRID_SET_Drain );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * HP�z����菈���R�A
 *
 * @param   wk
 * @param   attacker    �z����鑤�|�PID
 * @param   target      �z������鑤�|�PID�iNULL�ł��F�u�˂��͂�v��傫�Ȃ˂����Ώۂɂ��邽�ߗ��p�j
 * @param   drainHP
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scproc_DrainCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 drainHP )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  BOOL result = FALSE;

  drainHP = scEvent_RecalcDrainVolume( wk, attacker, target, drainHP );
  if( drainHP > 0 )
  {
    if( !scproc_RecoverHP_CheckFailBase(wk, attacker) )
    {
      result = scproc_RecoverHP( wk, attacker, drainHP, TRUE );
    }
  }
//  else{
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
//  }
  BTL_Hem_PopState( &wk->HEManager, hem_state );
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W���U�����J�n
 *
 * @param   wk
 * @param   attacker
 * @param   targets
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_DamageProcStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  BTL_EVENTVAR_Push();

    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_START );

  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W���U�����I��
 *
 * @param   wk
 * @param   attacker
 * @param   targets
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_DamageProcEnd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, BTL_POKESET* targets,
  const SVFL_WAZAPARAM* wazaParam, BOOL fDelayAttack )
{
  u32 real_hit_cnt = 0;   // ���̂Ƀ_���[�W��^������
  u32 hit_cnt = 0;        // �݂����܂߃_���[�W��^������

  const BTL_POKEPARAM* bpp;
  u32 i, damage_sum;

  hit_cnt = BTL_POKESET_GetCount( targets );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DELAY_ATTACK_FLAG, fDelayAttack );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );

    if( hit_cnt )
    {
      u32 damage;
      for(i=0, damage_sum=0; i<hit_cnt; ++i)
      {
        bpp = BTL_POKESET_Get( targets, i );
        damage = BTL_POKESET_GetDamage( targets, bpp );
        if( damage )
        {
          damage_sum += damage;
          BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+real_hit_cnt, BPP_GetID(bpp) );
          ++real_hit_cnt;
        }
      }
    }
    if( real_hit_cnt )
    {
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, real_hit_cnt );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage_sum );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_END_HIT_REAL );
    }
  BTL_EVENTVAR_Pop();


  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DELAY_ATTACK_FLAG, fDelayAttack );

    if( hit_cnt )
    {
      for(i=0, damage_sum=0; i<hit_cnt; ++i)
      {
        bpp = BTL_POKESET_Get( targets, i );
        damage_sum += BTL_POKESET_GetDamage( targets, bpp );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, hit_cnt );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE, damage_sum );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_END_HIT );
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGEPROC_END );

  BTL_EVENTVAR_Pop();
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �������� > �_���[�W���U�n > �_���[�W�l�v�Z
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W�v�Z
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   typeAff
 * @param   targetDmgRatio
 * @param   criticalFlag
 * @param   dstDamage       [out] �v�Z����
 *
 * @retval  BOOL     �Œ�_���[�W�l�i�h��E�U�����̒l�Ɗ֌W�Ȃ��^����_���[�W�j�̏ꍇ�ATRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CalcDamage( BTL_SVFLOW_WORK* wk,
    const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag, BOOL fSimurationMode, u16* dstDamage )
{
  enum {
    PRINT_FLG = TRUE,
  };
  WazaDamageType dmgType = WAZADATA_GetDamageType( wazaParam->wazaID );
  u32  rawDamage = 0;
  BOOL fFixDamage = FALSE;

  BTL_EVENTVAR_Push();

  BTL_EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, typeAff );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
  BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, dmgType );
  BTL_EVENTVAR_SetValue( BTL_EVAR_FIX_DAMAGE, 0 );

  BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC1 );
  rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_FIX_DAMAGE );

  if( rawDamage )
  {
    fFixDamage = TRUE;
  }
  else
  {
    u32 fxDamage;
    u16 atkPower, defGuard, wazaPower;
    u8 atkLevel;
    // �e��p�����[�^����f�̃_���[�W�l�v�Z
    wazaPower = scEvent_getWazaPower( wk, attacker, defender, wazaParam );
    atkPower  = scEvent_getAttackPower( wk, attacker, defender, wazaParam, criticalFlag );
    defGuard  = scEvent_getDefenderGuard( wk, attacker, defender, wazaParam, criticalFlag );
    {
      atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
      fxDamage = BTL_CALC_DamageBase( wazaPower, atkPower, atkLevel, defGuard );

      BTL_N_PrintfEx(PRINT_FLG, DBGSTR_CALCDMG_WazaParam, wazaParam->wazaID, wazaParam->wazaType);
      BTL_N_PrintfEx(PRINT_FLG, DBGSTR_CALCDMG_BaseDamage, fxDamage);
    }
    fxDamage  = BTL_CALC_MulRatio( fxDamage, targetDmgRatio );
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RangeHosei, fxDamage);
    // �V��␳
    {
      BtlWeather weather = scEvent_GetWeather( wk );
      fx32 weatherDmgRatio = BTL_CALC_GetWeatherDmgRatio( weather, wazaParam->wazaType );
      if( weatherDmgRatio != BTL_CALC_DMG_WEATHER_RATIO_NONE )
      {
        u32 prevDmg = fxDamage;
        fxDamage = BTL_CALC_MulRatio( fxDamage, weatherDmgRatio );
        BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_WeatherHosei, weatherDmgRatio, prevDmg, fxDamage);
      }
    }
    // �f�_���[�W�l���m��
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RAW_DAMAGE,
        wazaPower, atkLevel, atkPower, defGuard, fxDamage );

    // �N���e�B�J���łQ�{
    if( criticalFlag ){
      fxDamage *= 2;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_Critical, fxDamage);
    }
    //�����_���␳(100�`85��)
    if( !BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_DMG_RAND_OFF) )
    {
      u16 ratio;
      if( !fSimurationMode ){
        ratio = 100 - BTL_CALC_GetRand( 16 );
      }else{
        ratio = 85;
      }
      fxDamage = (fxDamage * ratio) / 100;
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_RandomHosei, ratio, fxDamage);
    }
    // �^�C�v��v�␳
    if( wazaParam->wazaType != POKETYPE_NULL )
    {
      fx32 ratio = scEvent_CalcTypeMatchRatio( wk, attacker, wazaParam->wazaType );
      fxDamage = BTL_CALC_MulRatio( fxDamage, ratio );
      if( ratio != FX32_ONE ){
        BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_TypeMatchHosei, (ratio*100>>FX32_SHIFT), fxDamage);
      }
    }
    // �^�C�v�����v�Z
    fxDamage = BTL_CALC_AffDamage( fxDamage, typeAff );
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_TypeAffInfo, typeAff, fxDamage);
    // �₯�Ǖ␳
    if( (dmgType == WAZADATA_DMG_PHYSIC)
    &&  (BPP_GetPokeSick(attacker) == POKESICK_YAKEDO)
    &&  (BPP_GetValue(attacker, BPP_TOKUSEI_EFFECTIVE) != POKETOKUSEI_KONJOU)
    ){
      fxDamage = (fxDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
    }
    rawDamage = fxDamage;
    if( rawDamage == 0 ){ rawDamage = 1; }

    // �e��n���h���ɂ��{���v�Z
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.01f), FX32_CONST(32) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, rawDamage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC2 );
    {
      fx32 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
      rawDamage = BTL_CALC_MulRatio( rawDamage, ratio );
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_CALCDMG_DamageResult,
            BPP_GetID(defender), ratio, BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE ), rawDamage);
    }
  } /* if( rawDamage == 0 ) */

  BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC_END );

  BTL_EVENTVAR_Pop();

  *dstDamage = rawDamage;
  return fFixDamage;
}
//---------------------------------------------------------------------------------------------
// ���U�_���[�W������̔��������i�_���[�W�~�������j
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage )
{
  if( !BPP_IsDead(attacker) )
  {
    u32  damage;
    BOOL fMustEnable;

    damage = scEvent_CalcKickBack( wk, attacker, waza, wazaDamage, &fMustEnable );

    if( damage )
    {
      HANDEX_STR_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_ReactionDmg );
      HANDEX_STR_AddArg( &wk->strParam, BPP_GetID(attacker) );
      if( (fMustEnable)
      ||  (scproc_SimpleDamage_CheckEnable(wk, attacker, damage))
      ){
        scproc_SimpleDamage_Core( wk, attacker, damage, &wk->strParam );
      }
    }
  }
}
//---------------------------------------------------------------------------------------------
// ���U�ȊO�̃_���[�W���ʏ���
//---------------------------------------------------------------------------------------------
/**
 *  ���U�ȊO�_���[�W�F�L���`�F�b�N�i�L���Ȃ�TRUE�j
 */
static BOOL scproc_SimpleDamage_CheckEnable( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage )
{
  if( !scEvent_CheckEnableSimpleDamage(wk, bpp, damage) )
  {
    return FALSE;
  }
  return TRUE;
}
/**
 *  ���U�ȊO�_���[�W�F���s
 */
static BOOL scproc_SimpleDamage_Core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, BTL_HANDEX_STR_PARAMS* str )
{
  int value = -damage;

  if( value )
  {
    scPut_SimpleHp( wk, bpp, value, TRUE );
    BPP_TURNFLAG_Set( bpp, BPP_TURNFLG_DAMAGED );

    if( str != NULL ){
      handexSub_putString( wk, str );
      HANDEX_STR_Clear( str );
    }

    scproc_CheckItemReaction( wk, bpp, BTL_ITEMREACTION_HP );

    TAYA_Printf("BPP ID=%d, HP=%d, ADRS=%p\n", BPP_GetID(bpp), BPP_GetValue(bpp,BPP_HP), bpp);

    if( scproc_CheckDeadCmd(wk, bpp) ){
      if( scproc_CheckShowdown(wk) ){
        return TRUE;
      }
    }

    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Proc] �����A�C�e���g�p
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL  �����A�C�e���g�p������TRUE�^�����Ȃ��E�g�p�ł��Ȃ��ꍇFALSE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u32 itemID = BPP_GetItem( bpp );
  u32 hem_state_1st;
  BOOL result = FALSE;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, BPP_GetID(bpp)) )
  {
    hem_state_1st = BTL_Hem_PushState( &wk->HEManager );
    result = !scEvent_CheckItemEquipFail( wk, bpp, itemID );
    if( result )
    {
      u32 hem_state_2nd = BTL_Hem_PushStateUseItem( &wk->HEManager, itemID );
      u8 fConsume = FALSE;

      scPut_UseItemAct( wk, bpp );
      if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_ITEM_SPEND) ){
        scproc_ConsumeItem( wk, bpp );
        fConsume = TRUE;
      }
      scEvent_ItemEquip( wk, bpp );

      if( fConsume ){
        scproc_ItemChange( wk, bpp, ITEM_DUMMY_DATA );
      }

      if( scproc_HandEx_Result(wk) == HandExResult_NULL ){
        result = FALSE;
      }

      BTL_Hem_PopState( &wk->HEManager, hem_state_2nd );
    }

    BTL_Hem_PopState( &wk->HEManager, hem_state_1st );
  }
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����A�C�e���g�p�ۃ`�F�b�N
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 *
 * @retval  BOOL    ����Ȍ����Ŏg�p�ł��Ȃ�������TRUE�^����ȊOFALSE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckItemEquipFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_ITEMEQUIP_FAIL );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}
//----------------------------------------------------------------------------------
/**
 * �����A�C�e�������
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 */
//----------------------------------------------------------------------------------
static void scproc_ConsumeItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u16 itemID = BPP_GetItem( bpp );

  scPut_ConsumeItem( wk, bpp );
  scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_ITEM_CONSUMED );

  /*  ����킴�����ŗ��p���Ă����B�K�v�Ȃ��Ȃ����͂��B
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    scEvent_ConsumeItem( wk, bpp, itemID );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }
  */
}
//----------------------------------------------------------------------------------
/**
 * [Put] �����A�C�e��������������L�^
 */
//----------------------------------------------------------------------------------
static void scPut_ConsumeItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  BPP_ConsumeItem( bpp );
  SCQUE_PUT_OP_ConsumeItem( wk->que, BPP_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����A�C�e�������
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 */
//----------------------------------------------------------------------------------
static void scEvent_ConsumeItem( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEM_CONSUMED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
// �A�C�e�������������ʏ���
//----------------------------------------------------------------------------------
static void scproc_ItemChange( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 nextItemID )
{
  u8 pokeID = BPP_GetID( bpp );
  u16 prevItemID = BPP_GetItem( bpp );
  u32 hem_state;

  // �A�C�e�����������m��n���h���Ăяo��
  hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_ItemSetDecide( wk, bpp, nextItemID );
  BTL_Hem_PopState( &wk->HEManager, hem_state );

  BTL_HANDLER_ITEM_Remove( bpp );
  SCQUE_PUT_OP_SetItem( wk->que, pokeID, nextItemID );
  BPP_SetItem( bpp, nextItemID );

  // �A�C�e���������������n���h���Ăяo��
  if( nextItemID != ITEM_DUMMY_DATA ){
    BTL_HANDLER_ITEM_Add( bpp );
  }

  hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_ItemSetFixed( wk, bpp );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}


//---------------------------------------------------------------------------------------------
// �|�P�����������I�ɕm���ɂ���
//---------------------------------------------------------------------------------------------
static void scproc_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_KillPokemon( wk, bpp, 0 );
  scproc_CheckDeadCmd( wk, bpp );
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W���U�̒ǉ����ʂɂ���Ԉُ�
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  WazaSick sick;
  BPP_SICK_CONT sick_cont;
  u32 i=0;

  sick = scEvent_CheckWazaAddSick( wk, wazaParam->wazaID, attacker, target, &sick_cont );
  if( sick != WAZASICK_NULL )
  {
    if( !BPP_IsDead(target) ){
      scproc_Fight_WazaSickCore( wk,  attacker, target, wazaParam->wazaID, sick, sick_cont, FALSE );
    }
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
static WazaSick scEvent_CheckWazaAddSick( BTL_SVFLOW_WORK* wk, WazaID waza,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BPP_SICK_CONT* pSickCont )
{
  BPP_SICK_CONT sickCont;
  WazaSick sick = WAZADATA_GetParam( waza, WAZAPARAM_SICK );
  WAZA_SICKCONT_PARAM  waza_contParam = WAZADATA_GetSickCont( waza );
  u8 per = WAZADATA_GetParam( waza, WAZAPARAM_SICK_PER );
  u8 fFail = FALSE;

  BTL_CALC_WazaSickContToBppSickCont( waza_contParam, attacker, &sickCont );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );

    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_SICK_CONT, sickCont.raw );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_SICK );

    sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( sick == WAZASICK_SPECIAL_CODE ){
      sick = WAZASICK_NULL;
    }
    sickCont.raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );

  BTL_EVENTVAR_Pop();

  if( !fFail )
  {
    if( sick != WAZASICK_NULL )
    {
      if( perOccur(wk, per) ){
        *pSickCont = sickCont;
        return sick;
      }
      // �f�o�b�O�@�\�ɂ��K������
      if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_MUST_TUIKA) ){
        *pSickCont = sickCont;
        return sick;
      }
    }
  }
  return  WAZASICK_NULL;
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̏�Ԉُ�
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target;
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  BOOL fSucceed;
  u32  targetCnt;

  sick = WAZADATA_GetParam( waza, WAZAPARAM_SICK );
  contParam = WAZADATA_GetSickCont( waza );
  fSucceed = FALSE;

  targetCnt = BTL_POKESET_GetCount( targetRec );

  if( targetCnt )
  {
    BTL_POKESET_SeekStart( targetRec );
    while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
    {
      BPP_SICK_CONT sickCont;
      BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );
      if( scproc_Fight_WazaSickCore( wk, attacker, target, waza, sick, sickCont, TRUE) ){
        wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
        fSucceed = TRUE;  // �^�[�Q�b�g�����āA�P�̂ł���Ԉُ�ɂ�����ΐ���
      }
    }
  }
  else{
    // ���Ƀ^�[�Q�b�g�������Ȃ��Ă���->���������܂����܂�Ȃ�����
    scPut_WazaFail( wk, attacker, waza );
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ���Ԉُ틤��
//---------------------------------------------------------------------------------------------
static BOOL scproc_Fight_WazaSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  WazaID waza, WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
  HANDEX_STR_Clear( &wk->strParam );

  if( sick == WAZASICK_SPECIAL_CODE )
  {
    sick = scEvent_DecideSpecialWazaSick( wk, attacker, target, &wk->strParam );
    if( (sick == WAZASICK_NULL) || (sick == WAZASICK_SPECIAL_CODE) ){
      return FALSE;
    }
  }
  else{
    scEvent_GetWazaSickAddStr( wk, sick, attacker, target, &wk->strParam );
  }
  scEvent_WazaSickCont( wk, attacker, target, sick, &sickCont );

  {
    // ���ꃁ�b�Z�[�W�������p�ӂ���Ă��Ȃ���΁Ascproc_AddSick ���ŁA
    // �ł��邾���W�����b�Z�[�W���o���悤�ɂ��Ă���B
    BOOL fDefaultMsg = (HANDEX_STR_IsEnable(&wk->strParam) == FALSE );
    if( scproc_AddSickRoot(wk, target, attacker, sick, sickCont, fAlmost, fDefaultMsg) )
    {
      if( !fDefaultMsg ){
        handexSub_putString( wk, &wk->strParam );
        HANDEX_STR_Clear( &wk->strParam );
      }
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ����ꏈ���̏�Ԉُ�ID�����������b�Z�[�W�����肷��
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   str
 *
 * @retval  WazaSick
 */
//----------------------------------------------------------------------------------
static WazaSick scEvent_DecideSpecialWazaSick( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str )
{
  WazaSick sickID;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)(str) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, WAZASICK_NULL );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_SPECIAL );
    sickID = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
  BTL_EVENTVAR_Pop();
  return sickID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��ʏ�̏�Ԉُ�ɑ΂��鐬�������b�Z�[�W�����肷��
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   str
 */
//----------------------------------------------------------------------------------
static void scEvent_GetWazaSickAddStr( BTL_SVFLOW_WORK* wk, WazaSick sick,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BTL_HANDEX_STR_PARAMS* str )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)(str) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_STR );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ���Ԉُ�̌p���p�����[�^����
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   sick
 * @param   sickCont    [io]�����O�p�����[�^�^������̌��ʂ��i�[
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaSickCont( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT* sickCont )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, sickCont->raw );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_PARAM );
    sickCont->raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [proc] ��Ԉُ폈�����[�g
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fAlmost            ���s�������Ɍ������b�Z�[�W��\������
 *
 * @retval  BOOL       ���������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_AddSickRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, BOOL fDefaultMsgEnable )
{
  if( scproc_AddSickCheckFail(wk, target, attacker, sick, sickCont, BTL_SICK_OW_NONE, fAlmost) == FALSE )
  {
    scproc_AddSickCore( wk, target, attacker, sick, sickCont, fDefaultMsgEnable, NULL );
    return TRUE;
  }
  return FALSE;
}
/**
* ��Ԉُ퐬�ۃ`�F�b�N
*/
static BOOL scproc_AddSickCheckFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BtlSickOverWriteMode overWriteMode, BOOL fDispFailResult )
{
  // ��{���[���ɂ�鎸�s
  BtlAddSickFailCode fail_code = addsick_check_fail_std( wk, target, sick, sickCont, overWriteMode );
  if( fail_code != BTL_ADDSICK_FAIL_NULL )
  {
    BTL_N_Printf( DBGSTR_SVFL_AddSickFailCode, fail_code );
    if( fDispFailResult ){
      scPut_AddSickFail( wk, target, fail_code, sick );
    }
    return TRUE;
  }
  // �Ƃ������ȂǓ�������ɂ�鎸�s
  else
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

    BOOL fFail = scEvent_WazaSick_CheckFail( wk, attacker, target, sick );

    if( fFail )
    {
      if( fDispFailResult ){
        scEvent_AddSick_Failed( wk, target, attacker, sick );
//        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      }
    }

    BTL_Hem_PopState( &wk->HEManager, hem_state );
    return fFail;
  }
}
/**
 *  ��Ԉُ편�s�`�F�b�N�i��{���[���j
 */
static BtlAddSickFailCode addsick_check_fail_std( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaSick sick, BPP_SICK_CONT sickCont, BtlSickOverWriteMode overWriteMode )
{
  // ���łɓ�����Ԉُ�ɂȂ��Ă���Ȃ玸�s
  if( BPP_CheckSick(target, sick) )
  {
    if( overWriteMode != BTL_SICK_OW_STRONG ){
      return BTL_ADDSICK_FAIL_ALREADY;
    }
  }

  // ���łɊ�{��Ԉُ�ɂȂ��Ă���Ȃ�A���̊�{��Ԉُ�ɂ͂Ȃ�Ȃ�
  if( (sick < POKESICK_MAX)
  &&  (BPP_GetPokeSick(target) != POKESICK_NULL)
  ){
    if( overWriteMode == BTL_SICK_OW_NONE ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
  if( (scEvent_GetWeather(wk) == BTL_WEATHER_SHINE)
  &&  (sick == POKESICK_KOORI)
  ){
    return BTL_ADDSICK_FAIL_OTHER;
  }

  // �͂��� or �ǂ��^�C�v�́A�u�ǂ��v�ɂȂ�Ȃ�
  if( sick==WAZASICK_DOKU )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_HAGANE)
    ||  PokeTypePair_IsMatch(type, POKETYPE_DOKU)
    ){
      return BTL_ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // �ق̂��^�C�v�́A�u�₯�ǁv�ɂȂ�Ȃ�
  if( sick==WAZASICK_YAKEDO )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_HONOO) ){
      return BTL_ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // ������^�C�v�́A�u������v�ɂȂ�Ȃ�
  if( sick==WAZASICK_KOORI )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_KOORI) ){
      return BTL_ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // �����^�C�v�́A�u��ǂ肬�̃^�l�v�ɂȂ�Ȃ�
  if( sick==WAZASICK_YADORIGI )
  {
    PokeTypePair type = BPP_GetPokeType( target );
    if( PokeTypePair_IsMatch(type, POKETYPE_KUSA) ){
      return BTL_ADDSICK_FAIL_NO_EFFECT;
    }
  }

  // ��{��Ԉُ�̎��ɂ́u�����сv�ɂȂ�Ȃ�
  if( sick == WAZASICK_AKUBI )
  {
    if( BPP_GetPokeSick(target) != POKESICK_NULL ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  // �}���`�^�C�v�́u�������v�ɂȂ�Ȃ�
  if( sick == WAZASICK_IEKI )
  {
    if( BPP_GetValue(target, BPP_TOKUSEI) == POKETOKUSEI_MARUTITAIPU ){
      return BTL_ADDSICK_FAIL_OTHER;
    }
  }

  return BTL_ADDSICK_FAIL_NULL;
}
//----------------------------------------------------------------------------------
/**
 * ��Ԉُ폈���R�A�i�m���j
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 * @param   sickCont
 * @param   fDefaultMsgEnable
 */
//----------------------------------------------------------------------------------
static void scproc_AddSickCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
  WazaSick sick, BPP_SICK_CONT sickCont, BOOL fDefaultMsgEnable, const BTL_HANDEX_STR_PARAMS* exStr )
{
  scPut_AddSick( wk, target, sick, sickCont );

  // �G�t�F�N�g�̗p�ӂ���Ă���ُ�͂����\��
  switch( sick ){
  case WAZASICK_DOKU:     scPut_EffectByPokePos( wk, target, BTLEFF_DOKU ); break;
  case WAZASICK_YAKEDO:   scPut_EffectByPokePos( wk, target, BTLEFF_YAKEDO ); break;
  case WAZASICK_MAHI:     scPut_EffectByPokePos( wk, target, BTLEFF_MAHI ); break;
  case WAZASICK_KOORI:    scPut_EffectByPokePos( wk, target, BTLEFF_KOORI ); break;
  case WAZASICK_NEMURI:   scPut_EffectByPokePos( wk, target, BTLEFF_NEMURI ); break;
  case WAZASICK_KONRAN:   scPut_EffectByPokePos( wk, target, BTLEFF_KONRAN ); break;
  case WAZASICK_MEROMERO: scPut_EffectByPokePos( wk, target, BTLEFF_MEROMERO ); break;
  }

  // ��Ԉُ�m��W�����b�Z�[�W�o��
  if( fDefaultMsgEnable )
  {
    BTL_SICK_MakeDefaultMsg( sick, sickCont, target, &wk->strParam );
    handexSub_putString( wk, &wk->strParam );
    HANDEX_STR_Clear( &wk->strParam );
  }
  else if( exStr != NULL ){
    handexSub_putString( wk, exStr );
  }

  // �V�F�C�~�������h�t�H�����ɖ߂�
  if( (sick == WAZASICK_KOORI)
  &&  (BPP_GetMonsNo(target) == MONSNO_SHEIMI)
  &&  (BPP_GetValue(target,BPP_FORM) == FORMNO_SHEIMI_SKY)
  ){
    BPP_ChangeForm( target, FORMNO_SHEIMI_LAND );
    SCQUE_PUT_ACT_ChangeForm( wk->que, BPP_GetID(target), FORMNO_SHEIMI_LAND );
    scPut_Message_Set( wk, target, BTL_STRID_SET_ChangeForm );
  }

  // ��Ԉُ�m��C�x���g�R�[��
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

    if( BTL_CALC_IsBasicSickID(sick) ){
      scEvent_PokeSickFixed( wk, target, attacker, sick, sickCont );
    }else if( sick == WAZASICK_IEKI ){
      scEvent_IekiFixed( wk, target );
    }else {
      scEvent_WazaSickFixed( wk, target, attacker, sick );
    }
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }

  // �A�C�e�������C�x���g��
  scproc_CheckItemReaction( wk, target, BTL_ITEMREACTION_SICK );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �V��`�F�b�N
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//----------------------------------------------------------------------------------
static BtlWeather scEvent_GetWeather( BTL_SVFLOW_WORK* wk )
{
  BOOL fDisable = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHECK );
    fDisable = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  if( fDisable ){
    return BTL_WEATHER_NONE;
  }

  return BTL_FIELD_GetWeather();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ��Ԉُ�����s����P�[�X�̃`�F�b�N
 *
 * @param   wk
 * @param   target
 * @param   sick
 *
 * @retval  BOOL    ���s����ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_WazaSick_CheckFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaSick sick  )
{
  BOOL fFail = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_CHECKFAIL );
    fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return fFail;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ��Ԉُ편�s���m�肵��
 *
 * @param   wk
 * @param   target
 * @param   sick
 */
//----------------------------------------------------------------------------------
static void scEvent_AddSick_Failed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADDSICK_FAILED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �|�P�����n��Ԉُ�m��
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 */
//----------------------------------------------------------------------------------
static void scEvent_PokeSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker,
  PokeSick sick, BPP_SICK_CONT sickCont )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    {
      u8 atkPokeID = (attacker!=NULL)? BPP_GetID(attacker) : BTL_POKEID_NULL;
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    }
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICK_CONT, sickCont.raw );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_POKESICK_FIXED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�n��Ԉُ�m��
 *
 * @param   wk
 * @param   target
 * @param   attacker
 * @param   sick
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaSickFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, const BTL_POKEPARAM* attacker, WazaSick sick )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    {
      u8 atkPokeID = (attacker!=NULL)? BPP_GetID(attacker) : BTL_POKEID_NULL;
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    }
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sick );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZASICK_FIXED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �������ɂ������������̊m��
 *
 * @param   wk
 * @param   target
 */
//----------------------------------------------------------------------------------
static void scEvent_IekiFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENT_ForceCallHandlers( wk, BTL_EVENT_IEKI_FIXED );
  BTL_EVENTVAR_Pop();
}


//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W���U�̒ǉ����ʂɂ�郉���N����
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
static BOOL scEvent_CheckAddRankEffectOccur( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
  u8 per = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_RANKPER_1 );
  u8 failFlag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_RANK_TARGET );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  BTL_EVENTVAR_Pop();

  if( !failFlag )
  {
    if( perOccur(wk, per) ){
      return TRUE;
    }
    // �f�o�b�O�@�\�ɂ��K������
    if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_MUST_TUIKA) ){
      return TRUE;
    }
  }

  return FALSE;
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̃����N����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target;
  u32 i = 0;

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wk, wazaParam, attacker, target, TRUE) )
    {
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
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
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost )
{
  BOOL ret = FALSE;
  u32 eff_cnt, i;
  u8 atkPokeID = BPP_GetID( attacker );

  eff_cnt = WAZADATA_GetRankEffectCount( wazaParam->wazaID );
  BTL_Printf("���U:%d�̃G�t�F�N�g��=%d\n", wazaParam->wazaID, eff_cnt);
  for(i=0; i<eff_cnt; ++i)
  {
    WazaRankEffect  effect;
    int  volume;
    BOOL fEffective = FALSE;

    scEvent_GetWazaRankEffectValue( wk, wazaParam->wazaID, i, attacker, target, &effect, &volume );
    if( effect != WAZA_RANKEFF_NULL )
    {
      if( effect != WAZA_RANKEFF_MULTI5 )
      {
        fEffective = scproc_RankEffectCore( wk, atkPokeID, target, effect, volume, atkPokeID, ITEM_DUMMY_DATA, fAlmost, TRUE );
      }
      else
      {
        u8 e;
        for( e=WAZA_RANKEFF_ORIGIN; e<WAZA_RANKEFF_BASE_MAX; ++e )
        {
          if( scproc_RankEffectCore(wk, atkPokeID, target, e, volume, atkPokeID,
              ITEM_DUMMY_DATA, fAlmost, TRUE)
          ){
            fEffective = TRUE;
          }
        }
      }
      if( fEffective )
      {
        u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
        scEvent_WazaRankEffectFixed( wk, target, wazaParam->wazaID, effect, volume );
//        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        BTL_Hem_PopState( &wk->HEManager, hem_state );
        ret = TRUE;
      }
    }
  }
  return ret;
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
  BTL_Printf("���U[%d]�̃����N����[%d] type=%d, volume=%d\n", waza, idx, *effect, *volume);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, *effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, *volume );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, 1 );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_GET_RANKEFF_VALUE );
    *effect = BTL_EVENTVAR_GetValue( BTL_EVAR_STATUS_TYPE );
    *volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    {
      u8 ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      if( ratio > 1 ){
        (*volume) *= ratio;
      }
    }
  BTL_EVENTVAR_Pop();

  if( *effect == WAZA_RANKEFF_SP ){
    *effect = WAZA_RANKEFF_NULL;
  };
}
//--------------------------------------------------------------------------
/**
 * �����N�������ʂ̃R�A�i���U�ȊO��������Ăяo���j
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 * @param   wazaUsePokeID    ���U�ɂ�郉���N�����Ȃ烏�U�g�p�҃|�PID�i����ȊO�� BTL_POKEID_NULL�j
 * @param   itemID    �A�C�e���g�p�ɂ�郉���N�����Ȃ�A�C�e��ID�w��i����ȊO�� ITEM_DUMMY_DATA ���w��j
 * @param   fAlmost   ����v���Ō��ʎ��s�������A���̃��b�Z�[�W��\������
 * @param   fStdMsg   �W�����b�Z�[�W���o�͂���i�����́~�~�����������I�Ȃǁj
 *
 * @retval  BOOL    �����N�������ʂ�����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, u8 atkPokeID, BTL_POKEPARAM* target,
  WazaRankEffect effect, int volume, u8 wazaUsePokeID, u16 itemID, BOOL fAlmost, BOOL fStdMsg )
{
  volume = scEvent_RankValueChange( wk, target, effect, wazaUsePokeID, itemID, volume );
  BTL_N_Printf( DBGSTR_SVFL_RankEffCore, BPP_GetID(target), effect, volume, atkPokeID );

  // ���łɌ��E�Ȃ炻���\��
  if( !BPP_IsRankEffectValid(target, effect, volume) ){
    if( fAlmost ){
      scPut_RankEffectLimit( wk, target, effect, volume );
    }
    return FALSE;
  }

  // �^�[�Q�b�g���݂�����Ԃ̎��A�����ȊO����̍�p�͎󂯂Ȃ�
  if( BPP_MIGAWARI_IsExist(target) )
  {
    u8 targetPokeID = BPP_GetID( target );
    if( atkPokeID != targetPokeID )
    {
      if( fAlmost ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, targetPokeID );
      }
      return FALSE;
    }
  }

  {
    BOOL ret = TRUE;

    if( scEvent_CheckRankEffectSuccess(wk, target, effect, atkPokeID, volume) )
    {
      // �����܂ŗ����烉���N���ʔ���
      BTL_Printf("�����N���ʔ����Ftype=%d, volume=%d, itemID=%d\n", effect, volume, itemID );
      scPut_RankEffect( wk, target, effect, volume, itemID, fStdMsg );
      {
        u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
        scEvent_RankEffect_Fix( wk, atkPokeID, target, effect, volume );
//        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        BTL_Hem_PopState( &wk->HEManager, hem_state );
      }
    }
    else
    {
      BTL_Printf("�����N���ʎ��s\n");
      if( fAlmost )
      {
        u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
        scEvent_RankEffect_Failed( wk, target );
//        scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
        BTL_Hem_PopState( &wk->HEManager, hem_state );
      }
      ret = FALSE;
    }

    return ret;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����N���ʂ̑����l��ω�������
 *
 * @param   wk
 * @param   target
 * @param   rankType
 * @param   wazaUsePokeID
 * @param   itemID
 * @param   volume
 *
 * @retval  int   �ω���̑����l
 */
//----------------------------------------------------------------------------------
static int scEvent_RankValueChange( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect rankType,
  u8 wazaUsePokeID, u16 itemID, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, wazaUsePokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, rankType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKVALUE_CHANGE );
    volume  = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
  BTL_EVENTVAR_Pop();
  return volume;
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�����N���ʁ���Ԉُ�𓯎��ɗ^���郏�U
//---------------------------------------------------------------------------------------------
static void scproc_Fight_EffectSick( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  WazaSick sick;
  WAZA_SICKCONT_PARAM contParam;
  BPP_SICK_CONT sickCont;
  BTL_POKEPARAM* target;

  sick = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_SICK );
  contParam = WAZADATA_GetSickCont( wazaParam->wazaID );

  BTL_CALC_WazaSickContToBppSickCont( contParam, attacker, &sickCont );

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    if( scproc_WazaRankEffect_Common(wk, wazaParam, attacker, target, TRUE) ){
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
    }
    if( scproc_Fight_WazaSickCore(wk, attacker, target, wazaParam->wazaID, sick, sickCont, FALSE) ){
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
    }
  }
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�FHP��
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleRecover( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target;
  u8  pokeID;

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    pokeID = BPP_GetID( target );
    if( !scproc_RecoverHP_CheckFailBase(wk, target) )
    {
      u32 recoverHP = scEvent_CalcRecoverHP( wk, waza, target );
      if( scproc_RecoverHP(wk, target, recoverHP, TRUE) )
      {
        wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HP_Recover, pokeID );
      }
    }
    else
    {
      if( BPP_IsHPFull(target) ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_HPFull, pokeID );
      }
    }
  }
}
/**
 *  HP�񕜃��[�g
 */
static BOOL scproc_RecoverHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP, BOOL fDispFailMsg )
{
  if( !scproc_RecoverHP_CheckFailSP(wk, bpp, fDispFailMsg) )
  {
    scproc_RecoverHP_Core( wk, bpp, recoverHP );
    return TRUE;
  }
  return FALSE;
}
/**
 *  HP�񕜉ۃ`�F�b�N�i��{�����j
 */
static BOOL scproc_RecoverHP_CheckFailBase( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  if( !BPP_IsFightEnable(bpp) ){
    return TRUE;
  }
  if( BPP_IsHPFull(bpp) ){
    return TRUE;
  }
  return FALSE;
}
/**
 *  HP�񕜉ۃ`�F�b�N�i��������j
 */
static BOOL scproc_RecoverHP_CheckFailSP( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fDispFailMsg )
{
  if( BPP_CheckSick(bpp, WAZASICK_KAIHUKUHUUJI) )
  {
    if( fDispFailMsg ){
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KaifukuFujiExe, BPP_GetID(bpp) );
    }
    return TRUE;
  }
  return FALSE;
}
/**
 *  HP�񕜏����R�A
 */
static void scproc_RecoverHP_Core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 recoverHP )
{
  u8 pokeID = BPP_GetID( bpp );
  BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );
  if( pos != BTL_POS_NULL ){
    SCQUE_PUT_ACT_EffectByPos( wk->que, pos, BTLEFF_HP_RECOVER );
  }
  scPut_SimpleHp( wk, bpp, recoverHP, TRUE );
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ꌂ���U����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* target;
  u8 atkLevel, defLevel, targetPokeID;
  BtlTypeAff aff;
  u32 i = 0;

  atkLevel = BPP_GetValue( attacker, BPP_LEVEL );

  while( 1 )
  {
    target = BTL_POKESET_Get( targets, i++ );
    if( target == NULL ){ break; }
    if( BPP_IsDead(target) ){ continue; }

    targetPokeID = BPP_GetID( target );

    if( !IsMustHit(attacker, target) )
    {
      // ������ƂԂȂǂɂ��n�Y��
      if( scEvent_CheckPokeHideAvoid(wk, attacker, target, wazaParam->wazaID) ){
        scPut_WazaAvoid( wk, target, wazaParam->wazaID );
        continue;
      }
    }

    // �g���v�����u�n�ɂ��n�Y��
    if( IsTripleFarPos(wk, attacker, target, wazaParam->wazaID) ){
      scPut_WazaAvoid( wk, target, wazaParam->wazaID );
      continue;
    }

    // ������背�x���������|�P�����ɂ͖���
    defLevel = BPP_GetValue( target, BPP_LEVEL );
    if( atkLevel < defLevel ){
      scput_WazaNoEffectIchigeki( wk, target );
      continue;
    }

    // ���U�����ɂ�閳����
    aff = BTL_DMGAFF_REC_Get( &wk->dmgAffRec, targetPokeID );
    if( aff == BTL_TYPEAFF_0 ){
      scput_WazaNoEffectIchigeki( wk, target );
      continue;
    }

    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

      if( scEvent_IchigekiCheck(wk, attacker, target, wazaParam->wazaID) )
      {
        BtlPokePos atkPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, BPP_GetID(attacker) );
        u16  damage = BPP_GetValue( target, BPP_HP );
        BtlTypeAffAbout  affAbout = BTL_CALC_TypeAffAbout( aff );

        wazaEffCtrl_SetEnable( wk->wazaEffCtrl );

        if( BPP_MIGAWARI_IsExist(target) )
        {
          scproc_Ichigeki_Migawari( wk, target, wazaParam, affAbout );
        }
        else
        {
          BppKoraeruCause korae_cause = scEvent_CheckKoraeru( wk, attacker, target, TRUE, &damage );

          if( korae_cause == BPP_KORAE_NONE ){
            scproc_Ichigeki_Succeed( wk, target, wazaParam, affAbout );
          }else{
            scproc_Ichigeki_Korae( wk, target, wazaParam, affAbout, korae_cause, damage );
          }
        }

        wazaDmgRec_Add( wk, atkPos, attacker, target, wazaParam, damage );

        scproc_CheckDeadCmd( wk, target );
        scproc_WazaDamageReaction( wk, attacker, target, wazaParam, aff, damage, FALSE, FALSE );
      }
      else
      {
        if( !scproc_HandEx_Result(wk) ){
          scPut_WazaAvoid( wk, target, wazaParam->wazaID );
        }
      }
       BTL_Hem_PopState( &wk->HEManager, hem_state );
    }

  }/* while(1) */
}
//--------------------------------------------------------------------------
/**
 * �ꌂ�K�E���U����
 */
//--------------------------------------------------------------------------
static void scproc_Ichigeki_Succeed( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, const SVFL_WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout )
{
  u8 pokeID = BPP_GetID( target );

  BPP_HpZero( target );
  SCQUE_PUT_OP_HpZero( wk->que, pokeID );

  SCQUE_PUT_ACT_WazaDamage( wk->que, pokeID, affAbout, wazaParam->wazaID );
  SCQUE_PUT_ACT_WazaIchigeki( wk->que, pokeID );
}
//--------------------------------------------------------------------------
/**
 * �ꌂ�K�E���U���炦��
 */
//--------------------------------------------------------------------------
static void scproc_Ichigeki_Korae( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, const SVFL_WAZAPARAM* wazaParam,
    BtlTypeAffAbout affAbout, BppKoraeruCause korae_cause, u16 damage )
{
  scPut_DecreaseHP( wk, target, damage );

//  SCQUE_PUT_ACT_SimpleHP( wk->que, BPP_GetID(target) );
  SCQUE_PUT_ACT_WazaDamage( wk->que, BPP_GetID(target), affAbout, wazaParam->wazaID );

  scproc_Koraeru( wk, target, korae_cause );
}
//----------------------------------------------------------------------------------
/**
 * �ꌂ�K�E���U �݂����j��
 */
//----------------------------------------------------------------------------------
static void scproc_Ichigeki_Migawari( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, const SVFL_WAZAPARAM* wazaParam, BtlTypeAffAbout affAbout )
{
  u8 pokeID = BPP_GetID( target );

  SCQUE_PUT_ACT_WazaIchigeki( wk->que, pokeID );
  SCQUE_PUT_ACT_WazaDamage( wk->que, pokeID, affAbout, wazaParam->wazaID );
  scproc_Migawari_Delete( wk, target );

}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ӂ��Ƃ΂����U����
//---------------------------------------------------------------------------------------------
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM *target;
  BOOL fFailMsgDisped = FALSE;

  BTL_POKESET_SeekStart( targetRec );
  while( (target = BTL_POKESET_SeekNext(targetRec)) != NULL )
  {
    if( scproc_PushOutCore(wk, attacker, target, FALSE, &fFailMsgDisped, 0, FALSE, NULL) )
    {
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
    }
    else if( fFailMsgDisped == FALSE ){
      scPut_WazaFail( wk, attacker, waza );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �ӂ��Ƃ΂������R�A
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   fForceChange  �����I�ɓ���ւ����[�h�Ŏ��s�iFALSE�Ȃ烋�[�����ɉ����Ď������ʁj
 * @param   effectNo      �������G�t�F�N�g�i���o�[�i0 = �W���G�t�F�N�g�j
 * @param   fIgnoreLevel  ���x�����ɂ�鎸�s������s��Ȃ�
 * @param   fFailMsgDisped  [out] ����Ȏ��s���������A������\��������TRUE
 *
 * @retval  BOOL    ������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_PushOutCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target,
  BOOL fForceChange, BOOL* fFailMsgDisped, u16 effectNo, BOOL fIgnoreLevel, BTL_HANDEX_STR_PARAMS* succeedMsg )
{
  PushOutEffect   eff;

  *fFailMsgDisped = FALSE;

  if( fForceChange ){
    eff = PUSHOUT_EFFECT_CHANGE;
  }else{
    eff = check_pushout_effect( wk );
  }

  if( eff == PUSHOUT_EFFECT_MUSTFAIL ){
    return FALSE;
  }else{

    BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(target) );

    // �Ώۂ���ɂ��Ȃ���Ύ��s
    if( targetPos == BTL_POS_NULL ){
      return FALSE;
    }

    // �Ώۂ�����ł��玸�s
    if( BPP_IsDead(target) ){
      return FALSE;
    }

    // �t���[�t�H�[���ŕ߂܂�Ă�ꍇ�͎��s
    if( BPP_CheckSick(target, WAZASICK_FREEFALL) ){
      return FALSE;
    }
    // �t���[�t�H�[���ŕ߂�ł���ꍇ�����s
    if( checkFreeFallUsing(target) ){
      return FALSE;
    }

    // ����v���Ŏ��s
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      u8 fFailed = scEvent_CheckPushOutFail( wk, attacker, target );
      if( fFailed )
      {
        if( scproc_HandEx_Result(wk) != HandExResult_NULL ){
          (*fFailMsgDisped) = TRUE;
        }
      }
      BTL_Hem_PopState( &wk->HEManager, hem_state );

      if( fFailed ){
        return FALSE;
      }
    }

    // �ʏ폈��
    {
      u8 clientID, posIdx;
      BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

      // ��������ւ�����
      if( eff == PUSHOUT_EFFECT_CHANGE )
      {
        SVCL_WORK* clwk;
        int nextPokeIdx;

        clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
        nextPokeIdx = get_pushout_nextpoke_idx( wk, clwk );

        if( nextPokeIdx >= 0 )
        {
          BTL_POKEPARAM* nextPoke = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, nextPokeIdx );
          u8 nextPokeID = BPP_GetID( nextPoke );

          scproc_MemberOutCore( wk, target, effectNo );
          if( succeedMsg != NULL ){
            handexSub_putString( wk, succeedMsg );
          }
          scproc_MemberInForChange( wk, clientID, posIdx, nextPokeIdx, FALSE );

          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_PushOutChange, nextPokeID );
          scproc_AfterMemberIn( wk );
        }
        else{
          return FALSE;
        }
      }
      // �o�g�����E����
      else
      {
        u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(attacker) );

        // �Ώۂ̕������x���ゾ�����玸�s
        if( !fIgnoreLevel )
        {
          u8 atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
          u8 tgtLevel = BPP_GetValue( target, BPP_LEVEL );
          if( tgtLevel > atkLevel ){
            return FALSE;
          }
        }

        BTL_ESCAPEINFO_Add( &wk->escInfo, clientID );

        scproc_MemberOutCore( wk, target, effectNo );
        wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
      }
    }
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * �u�ӂ��Ƃ΂��v�n�̃��U���ʂ��o�g���̃��[�������画�肷��
 */
//--------------------------------------------------------------------------
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor( wk->mainModule );

  switch( rule ){
  case BTL_RULE_SINGLE:
    if( competitor == BTL_COMPETITOR_WILD ){
      return PUSHOUT_EFFECT_ESCAPE;
    }else{
      return PUSHOUT_EFFECT_CHANGE;
    }

  default:
    return PUSHOUT_EFFECT_CHANGE;
  }
}
//--------------------------------------------------------------------------
/**
 * �u�ӂ��Ƃ΂��v�n�̃��U�ŋ������ꂩ���������������A
 * ���ɏo���|�P�����̃p�[�e�B���C���f�b�N�X�������_���Ō��߂�
 * �o����|�P�������T���ɋ��Ȃ��ꍇ�A-1 ��Ԃ�
 *
 * @param   wk
 * @param   clwk
 *
 * @retval  int  ���ɏo���|�P�����̃p�[�e�B��Index : �o����|�P�����Ȃ��ꍇ -1
 */
//--------------------------------------------------------------------------
static int get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk )
{
  const BTL_POKEPARAM* bpp;
  u32 i, startIdx, count, members;
  u8 list[ BTL_PARTY_MEMBER_MAX ];

  members = BTL_PARTY_GetMemberCount( clwk->party );
  startIdx = BTL_MAIN_GetClientBenchPosIndex( wk->mainModule, clwk->myID );

  for(i=startIdx, count=0; i<members; ++i)
  {
    bpp = BTL_PARTY_GetMemberDataConst( clwk->party, i );
    if( BPP_IsFightEnable(bpp) ){
      list[count++] = i;
    }
  }

  if( count )
  {
    i = BTL_CALC_GetRand( count );
    return list[ i ];
  }

  return -1;
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
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_PUSHOUT );
    BTL_SICKEVENT_CheckPushOutFail( wk, target );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  return failFlag;
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�t�B�[���h�G�t�F�N�g����
//---------------------------------------------------------------------------------------------
static void scput_Fight_FieldEffect( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker )
{
  BtlWeather  weather = WAZADATA_GetWeather( wazaParam->wazaID );
  if( weather != BTL_WEATHER_NONE )
  {
    u8 turn_upcnt = scEvent_WazaWeatherTurnUp( wk, weather, attacker );

    if( scproc_ChangeWeather( wk, weather, BTL_WEATHER_TURN_DEFAULT+turn_upcnt ) ){
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
    }else{
      scPut_WazaFail( wk, attacker, wazaParam->wazaID );
    }
  }
  else
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    BOOL fSucceed;
    scEvent_FieldEffectCall( wk, attacker, wazaParam->wazaID );
    fSucceed = (scproc_HandEx_Result( wk ) == HandExResult_Enable);
    BTL_Hem_PopState( &wk->HEManager, hem_state );

    if( fSucceed ){
      wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
    }else{
      scPut_WazaFail( wk, attacker, wazaParam->wazaID );
    }
  }
}
/**
 *  �V��ω�����
 */
static BOOL scproc_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn )
{
  if( scproc_ChangeWeatherCheck(wk, weather, turn) )
  {
    scproc_ChangeWeatherCore( wk, weather, turn );
    return TRUE;
  }
  return FALSE;
}
/**
 *  �V��ω��ۃ`�F�b�N
 */
static BOOL scproc_ChangeWeatherCheck( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn )
{
  if( weather >= BTL_WEATHER_MAX ){
    return FALSE;
  }

  if( BTL_FIELD_GetWeather() == weather )
  {
    if( (turn != BTL_WEATHER_TURN_PERMANENT)
    ||  (BTL_FIELD_GetWeatherTurn() == BTL_WEATHER_TURN_PERMANENT)
    ){
      return FALSE;
    }
  }

  return TRUE;
}
/**
 *  �V��ω��R�A
 */
static void scproc_ChangeWeatherCore( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn )
{
  BTL_FIELD_SetWeather( weather, turn );
  SCQUE_PUT_ACT_WeatherStart( wk->que, weather );
  scproc_ChangeWeatherAfter( wk, weather );
}
/**
 *  �V��ω�����C�x���g�Ăяo��
 */
static void scproc_ChangeWeatherAfter( BTL_SVFLOW_WORK* wk, BtlWeather weather )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_AfterChangeWeather( wk, weather );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��V��ω��̃^�[���������`�F�b�N
 *
 * @param   wk
 * @param   weather
 * @param   wk
 * @param   weather
 * @param   turn
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_WazaWeatherTurnUp( BTL_SVFLOW_WORK* wk, BtlWeather weather, const BTL_POKEPARAM* attacker )
{
  u8 cnt;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, 0 );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_WEATHER_TURNCNT );
    cnt = BTL_EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
  BTL_EVENTVAR_Pop();
  return cnt;
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

  {
    BOOL fFail;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
      BTL_EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, *turn );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE );
      weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );
      *turn = BTL_EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
      fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    BTL_EVENTVAR_Pop();
    return !fFail;
  }
}
static BOOL scproc_FieldEffectCore( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect, BPP_SICK_CONT contParam, BOOL fAddDependTry )
{
  if( BTL_FIELD_AddEffect(effect, contParam) )
  {
    SCQUE_PUT_OP_AddFieldEffect( wk->que, effect, contParam.raw );
    return TRUE;
  }
  else if( fAddDependTry )
  {
    u8 dependPokeID = BPP_SICKCONT_GetPokeID( contParam );
    if( dependPokeID != BTL_POKEID_NULL )
    {
      BTL_FIELD_AddDependPoke( effect, dependPokeID );
      SCQUE_PUT_OP_AddFieldEffectDepend( wk->que, effect, dependPokeID );
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �t�B�[���h�G�t�F�N�g�ǉ������Ăяo��
 *
 * @param   wk
 * @param   attacker
 * @param   target
 *
 */
//----------------------------------------------------------------------------------
static void scEvent_FieldEffectCall( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BOOL failFlag = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_FIELD_EFFECT_CALL );
  BTL_EVENTVAR_Pop();
}

//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���ނ�����Ȃ����U
//---------------------------------------------------------------------------------------------
static void scput_Fight_Uncategory( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam, BTL_POKEPARAM* attacker, BTL_POKESET* targets )
{
  BTL_POKEPARAM* bpp;

  // �^�[�Q�b�g�͋����͂��������������̗��R�Ŏc���Ă��Ȃ��ꍇ�͏I��
  if( BTL_POKESET_IsRemovedAll(targets) ){
    return;
  }

  switch( wazaParam->wazaID ){
  case WAZANO_SUKIRUSUWAPPU:
    scput_Fight_Uncategory_SkillSwap( wk, attacker, targets );
    break;
  case WAZANO_MIGAWARI:
    scproc_Migawari_Create( wk, attacker );
    break;
  default:
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      HandExResult result = HandExResult_NULL;
      BOOL fFailMsgEnable;

      if( scEvent_UnCategoryWaza(wk, wazaParam, attacker, targets, &fFailMsgEnable) )
      {
        result = scproc_HandEx_Result( wk );
        if( result == HandExResult_Enable ){
          wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
        }
      }

      if( (result == HandExResult_NULL) || (result == HandExResult_Fail) )
      {
        if( fFailMsgEnable ){
          SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
        }
      }

      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }
    break;
  }
}
// �X�L���X���b�v
static void scput_Fight_Uncategory_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKESET* targetRec )
{
  BTL_POKEPARAM* target = BTL_POKESET_Get( targetRec, 0 );
  PokeTokusei atk_tok, tgt_tok;

  atk_tok = BPP_GetValue( attacker, BPP_TOKUSEI );
  tgt_tok = BPP_GetValue( target, BPP_TOKUSEI );

  if( (atk_tok != tgt_tok)
  &&  (!BTL_TABLES_CheckSkillSwapFailTokusei(atk_tok))
  &&  (!BTL_TABLES_CheckSkillSwapFailTokusei(tgt_tok))
  ){
    u8 atkPokeID = BPP_GetID( attacker );
    u8 tgtPokeID = BPP_GetID( target );

    wazaEffCtrl_SetEnable( wk->wazaEffCtrl );

    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiBefore( wk, atkPokeID, atk_tok, tgt_tok );
      scEvent_ChangeTokuseiBefore( wk, tgtPokeID, tgt_tok, atk_tok );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }

    BPP_ChangeTokusei( attacker, tgt_tok );
    BPP_ChangeTokusei( target, atk_tok );
    BTL_HANDLER_TOKUSEI_Swap( attacker, target );

    SCQUE_PUT_ACT_SwapTokusei( wk->que, atkPokeID, tgtPokeID, tgt_tok, atk_tok );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_SkillSwap, atkPokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, atkPokeID );
    SCQUE_PUT_TOKWIN_OUT( wk->que, tgtPokeID );


    if( atk_tok != tgt_tok )
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiAfter( wk, atkPokeID );
      scEvent_ChangeTokuseiAfter( wk, tgtPokeID );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }

    if( atk_tok == POKETOKUSEI_BUKIYOU ){
      scproc_CheckItemReaction( wk, attacker, BTL_ITEMREACTION_GEN );
    }
    if( tgt_tok == POKETOKUSEI_BUKIYOU ){
      scproc_CheckItemReaction( wk, target, BTL_ITEMREACTION_GEN );
    }
  }
  else{
    SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂���� - �쐬
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_Migawari_Create( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  if( !BPP_MIGAWARI_IsExist(bpp) )
  {
    int migawariHP = BTL_CALC_QuotMaxHP( bpp, 4 );
    if( BPP_GetValue(bpp, BPP_HP) > migawariHP )
    {
      BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, BPP_GetID(bpp) );
      if( pos != BTL_POS_NULL )
      {
        scPut_SimpleHp( wk, bpp, -migawariHP, TRUE );
        scproc_CheckItemReaction( wk, bpp, BTL_ITEMREACTION_HP );

        BPP_MIGAWARI_Create( bpp, migawariHP );
        SCQUE_PUT_OP_MigawariCreate( wk->que, BPP_GetID(bpp), migawariHP );
        SCQUE_PUT_ACT_MigawariCreate( wk->que, pos );
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MigawariAppear, BPP_GetID(bpp) );
        return;
      }
    }
    // ���s���b�Z�[�W
    scPut_Message_StdEx( wk, BTL_STRID_STD_MigawariFail, 0, NULL );
  }
  else{
    // ���łɏo�Ă������b�Z�[�W
    scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariExist );
  }
}
//----------------------------------------------------------------------------------
/**
 * �݂���� - �_���[�W����
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16 ���ۂɗ^�����_���[�W�l
 */
//----------------------------------------------------------------------------------
static u16 scproc_Migawari_Damage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, u16 damage,
  BtlTypeAff aff, u8 fCritical, const SVFL_WAZAPARAM* wazaParam )
{

  SCQUE_PUT_ACT_MigawariDamage( wk->que, BPP_GetID(target), aff, wazaParam->wazaID );
  scPut_Message_Set( wk, target, BTL_STRID_SET_MigawariDamage );
  scPut_WazaAffinityMsg( wk, 1, &aff, &target, FALSE );
  scPut_CriticalMsg( wk, 1, &target, &fCritical, FALSE );

  if( BPP_MIGAWARI_AddDamage(target, &damage) )
  {
    scproc_Migawari_Delete( wk, target );
  }

  scproc_Damage_Drain( wk, wazaParam, attacker, target, damage );
  scproc_WazaAdditionalEffect( wk, wazaParam, attacker, target, damage, TRUE );
  scproc_WazaDamageReaction( wk, attacker, target, wazaParam, aff, damage, fCritical, TRUE );

  return damage;
}
//----------------------------------------------------------------------------------
/**
 * �݂���� - �폜����
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_Migawari_Delete( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  u8 pokeID = BPP_GetID( bpp );
  BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );

  scPut_Message_Set( wk, bpp, BTL_STRID_SET_MigawariDestract );

  BPP_MIGAWARI_Delete( bpp );
  SCQUE_PUT_OP_MigawariDelete( wk->que, pokeID );
  SCQUE_PUT_ACT_MigawariDelete( wk->que, pos );
}
//----------------------------------------------------------------------------------
/**
 * ���U�_���[�W�ɑ΂��锽������
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
  const SVFL_WAZAPARAM* wazaParam, BtlTypeAff affinity, u32 damage, BOOL critical_flag, BOOL fMigawari )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );

  scEvent_WazaDamageReaction( wk, attacker, defender, wazaParam, affinity, damage, critical_flag, fMigawari );
//  scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
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
  const BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BtlTypeAff aff,
  u32 damage, BOOL fCritical, BOOL fMigawari )
{
  u8 defPokeID = BPP_GetID( defender );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, defPokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TYPEAFF, aff );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_CRITICAL_FLAG, fCritical );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_MIGAWARI_FLAG, fMigawari );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_RINPUNGUARD_FLG, FALSE );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_REACTION_PREV );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_REACTION );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_REACTION_L2 );

  BTL_EVENTVAR_Pop();
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
static BOOL scEvent_UnCategoryWaza( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  const BTL_POKEPARAM* attacker, BTL_POKESET* targets, BOOL* fFailMsgEnable )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    {
      BTL_POKEPARAM* bpp;
      u8 targetMax, targetCnt, i;

      targetMax = BTL_POKESET_GetCountMax( targets );
      targetCnt = BTL_POKESET_GetCount( targets );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, TRUE );


      BTL_N_Printf( DBGSTR_SVFL_UncategoryWazaInfo, BPP_GetID(attacker), targetCnt, targetMax);

      for(i=0; i<targetCnt; ++i){
        bpp = BTL_POKESET_Get( targets, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      if( targetMax )
      {
        if( targetCnt ){
          BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORIZE_WAZA );
        }
      }
      else{
        BTL_EVENT_CallHandlers( wk, BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET );
      }

      (*fFailMsgEnable) = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );

    }
  BTL_EVENTVAR_Pop();

  return (scproc_HandEx_Result(wk) != HandExResult_NULL);
}


//==============================================================================
// �T�[�o�[�t���[�F�^�[���`�F�b�N���[�g
//==============================================================================

/**
 *  POKESET�ɑS�퓬�|�P���i�[ -> �f�������\�[�g
 */
static void StoreFrontPokeOrderAgility( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet )
{
  FRONT_POKE_SEEK_WORK  fps;
  BTL_POKEPARAM* bpp;

  BTL_POKESET_Clear( pokeSet );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext(&fps, wk, &bpp) ){
    BTL_POKESET_Add( pokeSet, bpp );
  }
  BTL_POKESET_SortByAgility( pokeSet, wk );
}

/**
 *  @retval BOOL  ���x���A�b�v���ď����r���Ŕ�������TRUE
 */
static BOOL scproc_TurnCheck( BTL_SVFLOW_WORK* wk )
{
  // ����POKESET�ɑΏۃ|�P�������i�[����
  BTL_POKESET* pokeSet = wk->psetTarget;
  BOOL fExpGet = FALSE;

  if( wk->turnCheckStep == 0 )
  {
    // POKESET�i�[->�f�������\�[�g
    StoreFrontPokeOrderAgility( wk, pokeSet );
    wk->turnCheckStep = 1;

    scproc_turncheck_CommSupport( wk );
  }

  switch( wk->turnCheckStep ){
  case 1:
    wk->turnCheckStep++;
    SCQUE_PUT_ACT_MsgWinHide( wk->que, 0 );
    if( scproc_turncheck_weather(wk, pokeSet) ){
      fExpGet = TRUE;
      break;
    }
    if( scproc_CheckShowdown(wk) ){ return FALSE; }
    /* fallthru */
  case 2:
    wk->turnCheckStep++;
    if( scproc_turncheck_sub(wk, pokeSet, BTL_EVENT_TURNCHECK_BEGIN) ){
      fExpGet = TRUE;
      break;
    }
    if( scproc_CheckShowdown(wk) ){
      return FALSE;
     }
    /* fallthru */
  case 3:
    wk->turnCheckStep++;
    if( scproc_turncheck_sick(wk, pokeSet) ){
      fExpGet = TRUE;
      break;
    }
    if( scproc_CheckShowdown(wk) ){ return FALSE; }
    /* fallthru */

  case 4:
    wk->turnCheckStep++;
    scproc_turncheck_side( wk );
    scproc_turncheck_field( wk );
    /* fallthru */
  case 5:
    wk->turnCheckStep++;
    if( scproc_turncheck_sub(wk, pokeSet, BTL_EVENT_TURNCHECK_END) ){
      fExpGet = TRUE;
      break;
    }
    if( scproc_CheckShowdown(wk) ){ return FALSE; }
    /* fallthru */
  case 6:
    // �S�|�P�����̃^�[���t���O���N���A
    wk->turnCheckStep++;
    {
      BTL_POKEPARAM* bpp;
      BTL_POKESET_SeekStart( pokeSet );
      while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
      {
        BPP_TurnCheck( bpp );
        BPP_CombiWaza_ClearParam( bpp );
        SCQUE_PUT_OP_TurnCheck( wk->que, BPP_GetID(bpp) );
      }
    }

    // �g���v���o�g�����A���Z�b�g���[�u����
    if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_TRIPLE ){
      scproc_CheckResetMove( wk );
    }

    // �����C�x���g�t�@�N�^�[�폜
    BTL_EVENT_RemoveIsolateFactors();

    if( wk->turnCount < BTL_TURNCOUNT_MAX ){
      wk->turnCount++;
    }
    wk->turnCheckStep = 0;
    wk->simulationCounter = 0;

    OS_SetPrintOutput_Arm9( 1 );
//    TAYA_Printf( " ============= turn END =============\n" );
    OS_SetPrintOutput_Arm9( 0 );

    return FALSE;
  }

  return fExpGet;
}
/**
 *  �^�[���`�F�b�N�F�g���v�������Z�b�g���[�u����
 */
static void scproc_CheckResetMove( BTL_SVFLOW_WORK* wk )
{
  if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_TRIPLE )
  {
    u8 clientID_1 = BTL_MAIN_GetPlayerClientID( wk->mainModule );
    u8 clientID_2 = BTL_MAIN_GetOpponentClientID( wk->mainModule, clientID_1, 0 );

    BTL_PARTY* party_1 = BTL_POKECON_GetPartyData( wk->pokeCon, clientID_1 );
    BTL_PARTY* party_2 = BTL_POKECON_GetPartyData( wk->pokeCon, clientID_2 );

    if( (BTL_PARTY_GetAliveMemberCount(party_1) == 1)
    &&  (BTL_PARTY_GetAliveMemberCount(party_2) == 1)
    ){
      BTL_POKEPARAM  *bpp1 = BTL_PARTY_GetAliveTopMember( party_1 );
      BTL_POKEPARAM  *bpp2 = BTL_PARTY_GetAliveTopMember( party_2 );

      u8 pokeID_1 = BPP_GetID( bpp1 );
      u8 pokeID_2 = BPP_GetID( bpp2 );

      BtlPokePos  pos1 = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID_1 );
      BtlPokePos  pos2 = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID_2 );

      u8 posIdx1 = BTL_MAIN_BtlPosToPosIdx( wk->mainModule, pos1 );
      u8 posIdx2 = BTL_MAIN_BtlPosToPosIdx( wk->mainModule, pos2 );

      if( (posIdx1 == posIdx2) && (!BTL_MAINUTIL_IsTripleCenterPos(pos1) ) )
      {
        SCQUE_PUT_ACT_TripleResetMove( wk->que, clientID_1, posIdx1, clientID_2, posIdx2 );
        scproc_MoveCore( wk, clientID_1, posIdx1, 1, FALSE );
        scproc_MoveCore( wk, clientID_2, posIdx2, 1, FALSE );
      }
    }
  }
}

/**
 *  �^�[���`�F�b�N�F�ʐM�N���҂ɂ��T�|�[�g
 */
static void   scproc_turncheck_CommSupport( BTL_SVFLOW_WORK* wk )
{
  COMM_PLAYER_SUPPORT* supportHandle = BTL_MAIN_GetCommSupportHandle( wk->mainModule );
  if( supportHandle )
  {
    SUPPORT_TYPE support_type;

// �e�X�g�p�ɋ����n�m���������Ɏg����
//    COMM_PLAYER_SUPPORT_SetParam( supportHandle, SUPPORT_TYPE_RECOVER_HALF, BTL_MAIN_GetPlayerStatus(wk->mainModule) );

    support_type = COMM_PLAYER_SUPPORT_GetSupportType( supportHandle );
    if( support_type != SUPPORT_TYPE_NULL && support_type != SUPPORT_TYPE_USED )
    {
      u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      SVCL_WORK* clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
      u8 pokeIDAry[ BTL_POSIDX_MAX ];
      u8 cnt, i;
      BTL_POKEPARAM* bpp;

      // �����Ă�HP�����^���ł͂Ȃ��|�P�̂ݗ�
      for(i=0, cnt=0; i<clwk->numCoverPos; ++i)
      {
        bpp = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, i );
        if( IsBppExist(bpp) && !BPP_IsHPFull(bpp) )
        {
          pokeIDAry[ cnt++ ] = BPP_GetID( bpp );
        }
      }
      if( cnt )
      {
        u32 recoverHP;
        u8 pokeID = pokeIDAry[ GFL_STD_MtRand(cnt) ];
        bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
        recoverHP = BPP_GetValue( bpp, BPP_MAX_HP );
        if( support_type == SUPPORT_TYPE_RECOVER_HALF ){
          recoverHP /= 2;
        }
        if( recoverHP )
        {
          if( scproc_RecoverHP(wk, bpp, recoverHP, FALSE) )
          {
            SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_CommSupport, BTL_CLIENTID_COMM_SUPPORT, pokeID );
            COMM_PLAYER_SUPPORT_SetUsed( supportHandle );
          }
        }
      }
    }
  }

}
/**
 *  �^�[���`�F�b�N�F�ėp
 *
 * @retval  BOOL    �N�������S->���x���A�b�v���������ꍇ��TRUE
 */
static BOOL scproc_turncheck_sub( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet, BtlEventType event_type )
{
  BTL_POKEPARAM* bpp;
  BTL_POKESET_SeekStart( pokeSet );

  while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
  {
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_TurnCheck( wk, bpp, event_type );
//      scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
      BTL_Hem_PopState( &wk->HEManager, hem_state );

      scproc_CheckDeadCmd( wk, bpp );
      if( scproc_CheckShowdown(wk) ){
        break;
      }
    }
  }

  return scproc_CheckExpGet( wk );
}
static void scEvent_TurnCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlEventType event_type )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, event_type );
  BTL_EVENTVAR_Pop();
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > ��Ԉُ�
//------------------------------------------------------------------
static BOOL scproc_turncheck_sick( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet )
{
  BTL_POKEPARAM* bpp;
  u32 hem_state;

  BTL_POKESET_SeekStart( pokeSet );
  while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
  {
    hem_state = BTL_Hem_PushState( &wk->HEManager );

    BPP_WazaSick_TurnCheck( bpp, BTL_SICK_TurnCheckCallback, wk );
    SCQUE_PUT_OP_WazaSickTurnCheck( wk->que, BPP_GetID(bpp) );

//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }

  return scproc_CheckExpGet( wk );
}
//--------------------------------------------------------------------------------
/**
 * �^�[���`�F�b�N�ŏ�Ԉُ�ɂ��_���[�W���������邽�тɃR�[���o�b�N�����
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   damage
 */
//--------------------------------------------------------------------------------
void BTL_SVF_SickDamageRecall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  BOOL fEnable = FALSE;
  damage = scEvent_SickDamage( wk, bpp, sickID, damage );
  if( damage )
  {
    if( scproc_SimpleDamage_CheckEnable(wk, bpp, damage) )
    {
      fEnable = TRUE;
      switch( sickID ){
      case WAZASICK_DOKU:
        scPut_EffectByPokePos( wk, bpp, BTLEFF_DOKU );
        break;
      case WAZASICK_YAKEDO:
        scPut_EffectByPokePos( wk, bpp, BTLEFF_YAKEDO );
        break;
      case WAZASICK_NOROI:
        scPut_EffectByPokePos( wk, bpp, BTLEFF_NOROI );
        break;
      case WAZASICK_AKUMU:
        scPut_EffectByPokePos( wk, bpp, BTLEFF_AKUMU );
        break;
      }
      HANDEX_STR_Clear( &wk->strParam );
      BTL_SICK_MakeSickDamageMsg( &wk->strParam, bpp, sickID );
      scproc_SimpleDamage_Core( wk, bpp, damage, &wk->strParam );
    }
  }

  if(!fEnable){
//    scproc_HandEx_Root( wk, ITEM_DUMMY_DATA );
  }

  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ��Ԉُ�ɂ��^�[���`�F�b�N�_���[�W
 *
 * @param   wk
 * @param   bpp
 * @param   sickID
 * @param   damage
 */
//----------------------------------------------------------------------------------
static u32 scEvent_SickDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SICKID, sickID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SICK_DAMAGE );
    damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
  BTL_EVENTVAR_Pop();
  return damage;
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �T�C�h�G�t�F�N�g
//------------------------------------------------------------------
static void scproc_turncheck_side( BTL_SVFLOW_WORK* wk )
{
  BTL_HANDLER_SIDE_TurnCheck( scproc_turncheck_side_callback, wk );
}
static void scproc_turncheck_side_callback( BtlSide side, BtlSideEffect sideEffect, void* arg )
{
  BTL_SVFLOW_WORK* wk = arg;
  scPut_SideEffectOffMsg( wk, sideEffect, side );
}
static void scPut_SideEffectOffMsg( BTL_SVFLOW_WORK* wk, BtlSideEffect sideEffect, BtlSide side )
{
  int strID = -1;

  switch( sideEffect ){
  case BTL_SIDEEFF_REFRECTOR:       strID = BTL_STRID_STD_ReflectorOff; break;
  case BTL_SIDEEFF_HIKARINOKABE:    strID = BTL_STRID_STD_HikariNoKabeOff; break;
  case BTL_SIDEEFF_SINPINOMAMORI:   strID = BTL_STRID_STD_SinpiNoMamoriOff; break;
  case BTL_SIDEEFF_SIROIKIRI:       strID = BTL_STRID_STD_SiroiKiriOff; break;
  case BTL_SIDEEFF_OIKAZE:          strID = BTL_STRID_STD_OikazeOff; break;
  case BTL_SIDEEFF_OMAJINAI:        strID = BTL_STRID_STD_OmajinaiOff; break;
  case BTL_SIDEEFF_MAKIBISI:        strID = BTL_STRID_STD_MakibisiOff; break;
  case BTL_SIDEEFF_DOKUBISI:        strID = BTL_STRID_STD_DokubisiOff; break;
  case BTL_SIDEEFF_STEALTHROCK:     strID = BTL_STRID_STD_StealthRockOff; break;
  case BTL_SIDEEFF_RAINBOW:         strID = BTL_STRID_STD_RainbowOff; break;
  case BTL_SIDEEFF_BURNING:         strID = BTL_STRID_STD_BurningOff; break;
  case BTL_SIDEEFF_MOOR:            strID = BTL_STRID_STD_MoorOff; break;
  default:
    break;
  }

  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( wk->que, strID, side );
  }
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �t�B�[���h�G�t�F�N�g
//------------------------------------------------------------------
static void scproc_turncheck_field( BTL_SVFLOW_WORK* wk )
{
  BTL_FIELD_TurnCheck( turncheck_field_callback, wk );
}
static void turncheck_field_callback( BtlFieldEffect effect, void* arg )
{
  BTL_SVFLOW_WORK* wk = arg;

  scproc_FieldEff_End( wk, effect );
}

static void scproc_FieldEff_End( BTL_SVFLOW_WORK* wk, BtlFieldEffect effect )
{
  int strID = -1;
  switch( effect ){
  case BTL_FLDEFF_TRICKROOM:   strID = BTL_STRID_STD_TrickRoomOff; break; ///< �g���b�N���[��
  case BTL_FLDEFF_JURYOKU:     strID = BTL_STRID_STD_JyuryokuOff; break;  ///< ���イ��傭
  case BTL_FLDEFF_WONDERROOM:  strID = BTL_STRID_STD_WonderRoom_End; break;
  case BTL_FLDEFF_MAGICROOM:   strID = BTL_STRID_STD_MagicRoom_End; break;
  }

  if( strID >= 0 )
  {
    SCQUE_PUT_MSG_STD( wk->que, strID );
  }
  SCQUE_PUT_OP_RemoveFieldEffect( wk->que, effect );

  // �}�W�b�N���[�����؂ꂽ��A�C�e�������`�F�b�N
  if( effect == BTL_FLDEFF_MAGICROOM )
  {
    BTL_POKEPARAM* bpp;

    StoreFrontPokeOrderAgility( wk, wk->psetTmp );
    BTL_POKESET_SeekStart( wk->psetTmp );
    while( (bpp = BTL_POKESET_SeekNext(wk->psetTmp)) != NULL )
    {
      if( BPP_IsFightEnable(bpp) )
      {
        scproc_CheckItemReaction( wk, bpp, BTL_ITEMREACTION_GEN );
      }
    }
  }
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �V��
//------------------------------------------------------------------
static BOOL scproc_turncheck_weather( BTL_SVFLOW_WORK* wk, BTL_POKESET* pokeSet )
{
  BtlWeather weather = BTL_FIELD_TurnCheckWeather();
  // �^�[���`�F�b�N�ɂ��V�󂪏I�����
  if( weather != BTL_WEATHER_NONE )
  {
    SCQUE_PUT_ACT_WeatherEnd( wk->que, weather );
    scproc_ChangeWeatherAfter( wk, BTL_WEATHER_NONE );
    return FALSE;
  }
  // �V�󂪌p��
  else
  {
    BTL_POKEPARAM* bpp;

    weather = scEvent_GetWeather( wk );
    BTL_POKESET_SeekStart( pokeSet );
    while( (bpp = BTL_POKESET_SeekNext(pokeSet)) != NULL )
    {
      if( (!BPP_IsDead(bpp))
      &&  (!BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_ANAWOHORU))
      &&  (!BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_DIVING))
      ){
        u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
        int  damage = BTL_CALC_RecvWeatherDamage( bpp, weather );
        damage = scEvent_CheckWeatherReaction( wk, bpp, weather, damage );
        if( !scproc_HandEx_Result(wk) )
        {
          if( damage ){
            scPut_WeatherDamage( wk, bpp, weather, damage );
          }
        }
        BTL_Hem_PopState( &wk->HEManager, hem_state );
        scproc_CheckDeadCmd( wk, bpp );
      }
    }
    return scproc_CheckExpGet( wk );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �V�󃊃A�N�V�����`�F�b�N
 *
 * @param   wk
 * @param   weather
 */
//----------------------------------------------------------------------------------
static int scEvent_CheckWeatherReaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BtlWeather weather, u32 damage )
{
  u8  failFlag;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE,  damage );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_REACTION );
    damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();
  if( failFlag ){
    return 0;
  }
  return damage;
}
//----------------------------------------------------------------------------------
/**
 * [Put]�V��ɂ��_���[�W����
 *
 * @param   wk
 * @param   bpp
 * @param   weather
 * @param   damage
 */
//----------------------------------------------------------------------------------
static void scPut_WeatherDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BtlWeather weather, int damage )
{
  u8 pokeID = BPP_GetID( bpp );

  switch( weather ){
  case BTL_WEATHER_SAND:
    HANDEX_STR_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WeatherDmg_Sand );
    HANDEX_STR_AddArg( &wk->strParam, pokeID );
    break;
  case BTL_WEATHER_SNOW:
    HANDEX_STR_Setup( &wk->strParam, BTL_STRTYPE_SET, BTL_STRID_SET_WeatherDmg_Snow );
    HANDEX_STR_AddArg( &wk->strParam, pokeID );
    break;
  default:
    HANDEX_STR_Clear( &wk->strParam );
    break;
  }

  BTL_Printf("�_���[�W�l=%d\n", damage);
  if( damage > 0 )
  {
    if( scproc_SimpleDamage_CheckEnable(wk, bpp, damage) )
    {
      handexSub_putString( wk, &wk->strParam );
      scproc_SimpleDamage_Core( wk, bpp, damage, NULL );
    }
  }
}

//--------------------------------------------------------------------------
/**
 * �Ώۃ|�P����������ł����玀�S�������K�v�R�}���h����
 */
//--------------------------------------------------------------------------
static BOOL scproc_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke )
{
  u8 pokeID = BPP_GetID( poke );

  if( wk->pokeDeadFlag[pokeID] == 0 )
  {
    if( BPP_IsDead(poke) )
    {
      BTL_N_Printf( DBGSTR_SVFL_PokeDead, pokeID);
      wk->pokeDeadFlag[pokeID] = 1;
      BTL_DEADREC_Add( &wk->deadRec, pokeID );

      // ���X�g�̃V���E���́u�����́@�����ꂽ�v���b�Z�[�W��\�����Ȃ�
      // ������U�����󂯂Ă���Œ��̃|�P���������l
      if( ((BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_LEGEND_EX) == FALSE) ||
          (BTL_MAINUTIL_PokeIDtoClientID(pokeID) == BTL_CLIENT_PLAYER))
      &&  (wk->thruDeadMsgPokeID != pokeID)
      ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, pokeID );
      }

      SCQUE_PUT_ACT_Dead( wk->que, pokeID );
      scproc_ClearPokeDependEffect( wk, poke );
      BPP_Clear_ForDead( poke );

      // �쐶��Ńv���C���[�����Ȃ炱�̎��_��BGM�Đ��R�}���h
      if( (BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD)
      &&  (wk->fWinBGMPlayWild == FALSE)
      &&  scproc_CheckShowdown(wk)
      &&  CheckPlayerSideAlive(wk)
      ){
        u16 WinBGM = BTL_MAIN_GetWinBGMNo( wk->mainModule );
        SCQUE_PUT_ACT_PlayWinBGM( wk->que, WinBGM );
        wk->fWinBGMPlayWild = TRUE;
      }

      // �v���C���[�̃|�P���������񂾎��ɂȂ��x�v�Z
      if( BTL_MAINUTIL_PokeIDtoClientID(pokeID) == BTL_MAIN_GetPlayerClientID(wk->mainModule) )
      {
        int deadPokeLv = BPP_GetValue( poke, BPP_LEVEL );
        int enemyMaxLv = checkExistEnemyMaxLevel( wk );
        BOOL fLargeDiffLevel = ( (deadPokeLv + 30) <= enemyMaxLv );

        BTL_N_Printf( DBGSTR_SVFL_DeadDiffLevelCheck, deadPokeLv, enemyMaxLv);
        BTL_MAIN_ReflectNatsukiDead( wk->mainModule, poke, fLargeDiffLevel );
      }

      // �o���l�擾 -> �ޏ�̏��ɂ��Ȃ��ƌo���l�v�Z�ł��������Ȃ�܂�
//      scproc_GetExp( wk, poke );
      BTL_POSPOKE_PokeOut( &wk->pospokeWork, pokeID );

      {
        u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
        u8 playerClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
        if( clientID == playerClientID )
        {
          BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_TEMOTI_KIZETU );
        }
        else if( !BTL_MAINUTIL_IsFriendClientID(clientID, playerClientID) )
        {
          BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_KILL_POKE );
        }
      }

      return TRUE;
    }
  }
  else
  {
    BTL_N_Printf( DBGSTR_SVFL_DeadAlready, pokeID);
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ����T�C�h�̏�ɏo�Ă���|�P�����̓��A�ő僌�x�����擾
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static u32 checkExistEnemyMaxLevel( BTL_SVFLOW_WORK* wk )
{
  u32 maxLv = 1;
  u32 posEnd = BTL_MAIN_GetEnablePosEnd( wk->mainModule );
  u8  playerClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
  u8  clientID;
  u32 pos;

  for(pos=0; pos<=posEnd; ++pos)
  {
    clientID = BTL_MAIN_BtlPosToClientID( wk->mainModule, pos );

    if( !BTL_MAINUTIL_IsFriendClientID(playerClientID, clientID) )
    {
      u8 pokeID = BTL_POSPOKE_GetExistPokeID( &wk->pospokeWork, pos );
      if( pokeID != BTL_POKEID_NULL )
      {
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
        u32 lv = BPP_GetValue( bpp, BPP_LEVEL );
        if( lv > maxLv ){
          maxLv = lv;
        }
      }
    }
  }

  return maxLv;
}

//--------------------------------------------------------------------------
/**
 * ����|�P�����ˑ��̏�Ԉُ�E�T�C�h�G�t�F�N�g���A�e��n���h�����N���A����
 */
//--------------------------------------------------------------------------
static void scproc_ClearPokeDependEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u8 dead_pokeID = BPP_GetID( poke );

  scEvent_BeforeDead( wk, poke );

  scproc_FreeFall_CheckRelease( wk, poke, FALSE );

  BTL_HANDLER_TOKUSEI_Remove( poke );
  BTL_HANDLER_ITEM_Remove( poke );
  BTL_HANDLER_Waza_RemoveForceAll( poke );

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    BPP_CureWazaSickDependPoke( bpp, dead_pokeID );
    SCQUE_PUT_OP_CureSickDependPoke( wk->que, BPP_GetID(bpp), dead_pokeID );
  }
  BTL_FIELD_RemoveDependPokeEffect( dead_pokeID );
  SCQUE_PUT_OP_DeleteDependPokeFieldEffect( wk->que, dead_pokeID );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���S���O�ʒm
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_BeforeDead( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_NOTIFY_DEAD );
  BTL_EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------
/**
 * ���S�|�P���R�[�h�����ĕK�v�Ȃ�o���l�擾�̏������s��
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static BOOL scproc_CheckExpGet( BTL_SVFLOW_WORK* wk )
{
  BOOL result = FALSE;
  if( BTL_MAIN_IsExpSeqEnable(wk->mainModule) )
  {
    u32 i, deadPokeCnt = BTL_DEADREC_GetCount( &wk->deadRec, 0 );

    for(i=0; i<deadPokeCnt; ++i)
    {
      if( !BTL_DEADREC_GetExpCheckedFlag(&wk->deadRec, 0, i) )
      {
        u8 pokeID = BTL_DEADREC_GetPokeID( &wk->deadRec, 0, i );
        const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );

        if( scproc_GetExp(wk, bpp) ){
          result = TRUE;
        }

        BTL_DEADREC_SetExpCheckedFlag( &wk->deadRec, 0, i );
      }
    }
  }
  return result;
}
//----------------------------------------------------------------------------------
/**
 * �o���l�擾�����R�}���h����
 *
 * @param   wk
 * @param   deadPoke
 *
 * @retval  BOOL    �o���l�擾�����ɂ�背�x���A�b�v������i�擾�����𖞂����Ă�����jTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_GetExp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* deadPoke )
{
  u8 deadPokeID = BPP_GetID( deadPoke );
  if( BTL_MAINUTIL_PokeIDtoSide(deadPokeID) == BTL_SIDE_2ND )
  {
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, BTL_MAIN_GetPlayerClientID(wk->mainModule) );

    BTL_SVFSUB_CalcExp( wk, party, deadPoke, wk->calcExpWork );
    return getexp_make_cmd( wk, party, wk->calcExpWork );
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �o���l�v�Z���ʂ����ɃR�}���h����
 *
 * @param   wk
 * @param   party     �v���C���[���p�[�e�B
 * @param   calcExp   �o���l�v�Z���ʃ��[�N
 *
 * @retval BOOL  ���x���A�b�v������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL getexp_make_cmd( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const CALC_EXP_WORK* calcExp )
{
  BTL_POKEPARAM* bpp;
  u32 i;
  BOOL result = FALSE;

  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i)
  {
    if( calcExp[i].exp )
    {
      BTL_POKEPARAM* bpp;


      bpp = BTL_PARTY_GetMemberData( party, i );
      if( BPP_GetValue(bpp, BPP_LEVEL) < PTL_LEVEL_MAX )
      {
        u32 exp   = calcExp[i].exp;
        u16 strID = (calcExp[i].fBonus)? BTL_STRID_STD_GetExp_Bonus : BTL_STRID_STD_GetExp;
        u8  pokeID = BPP_GetID( bpp );

//        BTL_Printf("�o���l�͂��������b�Z�[�W :strID=%d, pokeID=%d, exp=%d\n", strID, pokeID, exp);
        SCQUE_PUT_MSG_STD( wk->que, strID, pokeID, exp );

        {
          u32 restExp = exp;
          while(1){
            if( !BPP_AddExp(bpp, &restExp, &wk->lvupInfo) ){
              break;
            }
          }
        }
        SCQUE_PUT_ACT_AddExp( wk->que, pokeID, exp );

        BTL_MAIN_RECORDDATA_Add( wk->mainModule, RECID_DAYCNT_EXP, exp );

        result = TRUE;

      }
    }
  }

  return result;
}

/**
 *  ������Ƃԏ�Ԃ��L�����Z��
 */
static BOOL scproc_TameHideCancel( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BppContFlag hideContFlag )
{
  if( BPP_CONTFLAG_Get(bpp, hideContFlag) )
  {
    u8 pokeID = BPP_GetID( bpp );

    scPut_ResetContFlag( wk, bpp, hideContFlag );
    if( BPP_CheckSick(bpp, WAZASICK_TAMELOCK) ) {
      scPut_CureSick( wk, bpp, WAZASICK_TAMELOCK, NULL );
    }
    SCQUE_PUT_ACT_TameWazaHide( wk->que, pokeID, FALSE );

    ActOrder_ForceDone( wk, pokeID );
    return TRUE;
  }
  return FALSE;
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
static inline BOOL perOccur( BTL_SVFLOW_WORK* wk, u8 per )
{
  return BTL_CALC_GetRand( 100 ) < per;
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
 * [Put] �w��|�P�����ʒu�ɃG�t�F�N�g����
 */
//--------------------------------------------------------------------------
static void scPut_EffectByPokePos( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 effectNo )
{
  u8 pokeID = BPP_GetID( bpp );
  BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );
  if( pos != BTL_POS_NULL )
  {
    SCQUE_PUT_ACT_EffectByPos( wk->que, pos, effectNo );
  }
}

//--------------------------------------------------------------------------
/**
 * [Put] �u�����́@�͂�ǂ��ł������Ȃ��v�\��
 */
//--------------------------------------------------------------------------
static void scPut_CantAction( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  scPut_Message_Set( wk, bpp, BTL_STRID_SET_CantAction );
}
//--------------------------------------------------------------------------
/**
 * �u�����͂����񂵂Ă���v�\��
 */
//--------------------------------------------------------------------------
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  scPut_EffectByPokePos( wk, bpp, BTLEFF_KONRAN );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAct, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * �u�����́����Ƀ����������v�\��
 */
//--------------------------------------------------------------------------
static void scPut_MeromeroAct( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  u8 target_pokeID = BPP_GetSickParam( bpp, WAZASICK_MEROMERO );
  scPut_EffectByPokePos( wk, bpp, BTLEFF_MEROMERO );
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MeromeroAct, BPP_GetID(bpp), target_pokeID );
}
//--------------------------------------------------------------------------
/**
 * ������ɂ�鎩���_���[�W����
 */
//--------------------------------------------------------------------------
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage )
{
  u8 pokeID = BPP_GetID( bpp );

  // ������̓}�W�b�N�K�[�h�̉e�����󂯂Ȃ��̂�SimpleHP�𒼐ڌĂ�ł���  taya
  SCQUE_PUT_ACT_ConfDamage( wk->que, pokeID );
  scPut_SimpleHp( wk, bpp, (int)(-damage), TRUE );
  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_KonranExe, pokeID );

}
//--------------------------------------------------------------------------
/**
 * ���U�o�����莞�̃|�P�����n��Ԉُ�񕜏���
 */
//--------------------------------------------------------------------------
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick, BOOL fStdMsg )
{
  u8 pokeID = BPP_GetID( bpp );

  BPP_CurePokeSick( bpp );
  SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
  SCQUE_PUT_ACT_SickIcon( wk->que, pokeID, POKESICK_NULL );

  if( fStdMsg )
  {
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
}
//--------------------------------------------------------------------------
/**
 * ���U���b�Z�[�W�\���i�����́~�~���������I�Ȃǁj
 */
//--------------------------------------------------------------------------
static void scPut_WazaMsg( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  SCQUE_PUT_MSG_WAZA( wk->que, BPP_GetID(attacker), waza );
}
//----------------------------------------------------------------------------------
/**
 * [Put]��Ԉُ�񕜏����̃R�}���h����
 *
 * @param   wk
 * @param   bpp       �Ώۃ|�P����
 * @param   sick_id   ��Ԉُ�R�[�h
 * @param   oldCont   [out] �񕜑O�̌p���p�����[�^�i�s�v�Ȃ�NULL)
 */
//----------------------------------------------------------------------------------
static void scPut_CureSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT* oldCont )
{
  if( sick != WAZASICK_NULL )
  {
    u8 pokeID = BPP_GetID( bpp );

    if( oldCont ){
      *oldCont = BPP_GetSickCont( bpp, sick );
    }

    if( sick < POKESICK_MAX )
    {
      BPP_CurePokeSick( bpp );
      SCQUE_PUT_OP_CurePokeSick( wk->que, pokeID );
      if ( BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, pokeID) != BTL_POS_NULL ){
        SCQUE_PUT_ACT_SickIcon( wk->que, pokeID, POKESICK_NULL );
      }
    }
    else
    {
      BPP_CureWazaSick( bpp, sick );
      SCQUE_PUT_OP_CureWazaSick( wk->que, pokeID, sick );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �g����Ԉُ�R�[�h���A�Y������R�[�h�ɕϊ�����
 *
 * @param   bpp
 * @param   exCode    [io]
 *
 * @retval  WazaSick    �Y���R�[�h�^�Y���R�[�h�Ȃ��̏ꍇ�AWAZASICK_NULL��Ԃ�
 */
//----------------------------------------------------------------------------------
static WazaSick trans_sick_code( const BTL_POKEPARAM* bpp, BtlWazaSickEx* exCode )
{
  WazaSick result = WAZASICK_NULL;
  PokeSick poke_sick = BPP_GetPokeSick( bpp );

  switch( (*exCode) ){
  case WAZASICK_EX_POKEFULL:
    // �|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ炻�������
    if( poke_sick != WAZASICK_NULL ){
      result = poke_sick;
    }
    *exCode = WAZASICK_NULL;
    break;

  case WAZASICK_EX_POKEFULL_PLUS:
    // �|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ炻�������
    if( poke_sick != WAZASICK_NULL ){
      result = poke_sick;
      *exCode = WAZASICK_KONRAN;
      break;
    }
    if( BPP_CheckSick(bpp, WAZASICK_KONRAN) ){
      result = WAZASICK_KONRAN;
    }
    *exCode = WAZASICK_NULL;
    break;

  case WAZASICK_EX_MENTAL:  // �����^���n�[�u
    {
      result = BTL_CALC_CheckMentalSick( bpp );
      if( result == WAZASICK_NULL ){
        *exCode = WAZASICK_NULL;
      }
      return result;
    }
    break;

  case WAZASICK_NULL:
    break;

  default:
    if( BPP_CheckSick(bpp, *exCode) ){
      result = (*exCode);
    }
    *exCode = WAZASICK_NULL;
    break;
  }

  return result;
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
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, BPP_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * �u�����ɂ͂��������Ȃ��悤���v�\��
 */
//--------------------------------------------------------------------------
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, BPP_GetID(defender) );
}
//--------------------------------------------------------------------------
/**
 * �u�����ɂ�  ���񂺂񂫂��ĂȂ��v�\��
 */
//--------------------------------------------------------------------------
static void scput_WazaNoEffectIchigeki( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender )
{
  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NotEffect_Ichigeki, BPP_GetID(defender) );
}

//--------------------------------------------------------------------------
/**
 * [Put] ���U�ɂ��_���[�W�R�}���h����
 */
//--------------------------------------------------------------------------
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, const SVFL_WAZAPARAM* wazaParam,
  u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag, BOOL fTargetPlural )
{
  BtlTypeAffAbout  affAbout;
  u8               affGoodCnt, affBadCnt;
  u32 i;

  affGoodCnt = affBadCnt = 0;

  // HP�����R�}���h���������ʉ��p�̑�������
  for(i=0; i<poke_cnt; ++i)
  {
    scPut_DecreaseHP( wk, bpp[i], damage[i] );
    if( aff[i] > BTL_TYPEAFF_100 ){
      ++affGoodCnt;
    }
    if( aff[i] < BTL_TYPEAFF_100 ){
      ++affBadCnt;
    }
  }

  // �_���[�W�G�t�F�N�g�R�}���h����
  if( affGoodCnt ){
    affAbout = BTL_TYPEAFF_ABOUT_ADVANTAGE;
  }else{
    affAbout = (affBadCnt)? BTL_TYPEAFF_ABOUT_DISADVANTAGE : BTL_TYPEAFF_ABOUT_NORMAL;
  }
  SCQUE_PUT_ACT_WazaDamagePlural( wk->que, poke_cnt, affAbout, wazaParam->wazaID );
  BTL_N_Printf( DBGSTR_SVFL_WazaDmgCmd, wazaParam->wazaID, poke_cnt );
  for(i=0; i<poke_cnt; ++i)
  {
    SCQUE_PUT_ArgOnly( wk->que, BPP_GetID(bpp[i]) );
    BTL_N_PrintfSimple( DBGSTR_csv, BPP_GetID(bpp[i]) );
  }
  BTL_N_PrintfSimple( DBGSTR_LF );
}
//--------------------------------------------------------------------------
/**
 * [Put] �������b�Z�[�W����
 */
//--------------------------------------------------------------------------
static void scPut_WazaAffinityMsg( BTL_SVFLOW_WORK* wk, u32 poke_cnt, const BtlTypeAff* aff, BTL_POKEPARAM** bpp,  BOOL fTargetPlural )
{
  u8               affGoodCnt, affBadCnt;
  u32 i;

  affGoodCnt = affBadCnt = 0;

  // ���ʉ��p�̑�������
  for(i=0; i<poke_cnt; ++i)
  {
    if( aff[i] > BTL_TYPEAFF_100 ){ ++affGoodCnt; }
    if( aff[i] < BTL_TYPEAFF_100 ){ ++affBadCnt; }
  }

  // �������b�Z�[�W�R�}���h����
  if( fTargetPlural )
  {
    u8 pokeID[ BTL_POSIDX_MAX ];
    u8 c;

    if( affGoodCnt )
    {
      for(i=0, c=0; i<poke_cnt; ++i)
      {
        if( aff[i] > BTL_TYPEAFF_100 ){
          pokeID[c++] = BPP_GetID( bpp[i] );
        }
      }
      switch( affGoodCnt ){
      case 1: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_AffGood_1, pokeID[0] ); break;
      case 2: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_AffGood_2, pokeID[0], pokeID[1] ); break;
      case 3: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_AffGood_3, pokeID[0], pokeID[1], pokeID[2] ); break;
      }
    }
    if( affBadCnt )
    {
      for(i=0, c=0; i<poke_cnt; ++i)
      {
        if( aff[i] < BTL_TYPEAFF_100 ){
          pokeID[c++] = BPP_GetID( bpp[i] );
        }
      }
      switch( affBadCnt ){
      case 1: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_AffBad_1, pokeID[0] ); break;
      case 2: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_AffBad_2, pokeID[0], pokeID[1] ); break;
      case 3: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_AffBad_3, pokeID[0], pokeID[1], pokeID[2] ); break;
      }
    }
  }
  else
  {
    if( affGoodCnt ){
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_AffGood );
    }else if( affBadCnt ){
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_AffBad );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �N���e�B�J�����b�Z�[�W�R�}���h����
 */
//--------------------------------------------------------------------------
static void scPut_CriticalMsg( BTL_SVFLOW_WORK* wk, u32 poke_cnt, BTL_POKEPARAM** bpp, const u8* critical_flg, BOOL fTargetPlural )
{
  u32 i;

  for(i=0; i<poke_cnt; ++i)
  {
    if( critical_flg[i] )
    {
      if( fTargetPlural ){
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_CriticalHit, BPP_GetID(bpp[i]) );
      }else{
        SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_CriticalHit );
      }
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
 * [Put] �Ƃ������E�B���h�E�\���J�n
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_IN( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Ƃ������E�B���h�E�����J�n
 */
//--------------------------------------------------------------------------
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_TOKWIN_OUT( wk->que, BPP_GetID(bpp) );
}
//--------------------------------------------------------------------------
/**
 * [Put] ����ȏ�A�\�̓����N��������Ȃ��i������Ȃ��j���b�Z�[�W
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
 * [Put] �����N�������ʂ�����
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
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_Rankup_ATK, pokeID, itemID, effect, volume );
      }
    }
  }
  else
  {
    volume *= -1;
    volume = BPP_RankDown( target, effect, volume );
    SCQUE_PUT_OP_RankDown( wk->que, pokeID, effect, volume );
    SCQUE_PUT_ACT_RankDown( wk->que, pokeID, effect, volume );
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankdown_ATK, pokeID, effect, volume );
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �V���v��HP�����G�t�F�N�g
 */
//--------------------------------------------------------------------------
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value, BOOL fEffectEnable )
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
    scPut_DecreaseHP( wk, bpp, value );
  }

  // ��ɂ��Ȃ��|�P���񕜂����邱�Ƃ����邽��
  if( fEffectEnable
  &&  (BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, pokeID) != BTL_POS_NULL)
  ){
    SCQUE_PUT_ACT_SimpleHP( wk->que, pokeID );
  }
  else{
    BTL_Printf("�G�t�F�N�g�L���t���O=%d, �G�t�F�N�g�𐶐����܂���\n", fEffectEnable);
  }
}
//--------------------------------------------------------------------------
/**
 * [Put] �|�P���������Ђ�
 */
//--------------------------------------------------------------------------
static void scPut_KillPokemon( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 effType )
{
  u16 minusHP = BPP_GetValue( bpp, BPP_HP );
  u8 pokeID = BPP_GetID( bpp );

  BPP_HpZero( bpp );
  SCQUE_PUT_OP_HpZero( wk->que, pokeID );
  SCQUE_PUT_ACT_Kill( wk->que, pokeID, effType );
}
//--------------------------------------------------------------------------
/**
 * [Put] ��Ԉُ퉻
 */
//--------------------------------------------------------------------------
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont )
{
  u8 pokeID = BPP_GetID( target );

  BPP_SetWazaSick( target, sick, sickCont );
  SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );

  // ��Ԉُ�A�C�R���t��
  if( sick < POKESICK_MAX ){
    if( (sick == POKESICK_DOKU) && BPP_SICKCONT_IsMoudokuCont(sickCont) ){
      sick = POKESICK_MAX;
    }
    SCQUE_PUT_ACT_SickIcon( wk->que, pokeID, sick );
  }
}
/**
 *  ��Ԉُ퉻�̎��s������\��
 */
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, BtlAddSickFailCode failCode, WazaSick sick )
{
  u8 pokeID = BPP_GetID( target );

  switch( failCode ){
  case BTL_ADDSICK_FAIL_ALREADY:
    switch( sick ){
    case WAZASICK_DOKU:   SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_DokuAlready, pokeID ); break;
    case WAZASICK_YAKEDO: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_YakedoAlready, pokeID ); break;
    case WAZASICK_MAHI:   SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MahiAlready, pokeID ); break;
    case WAZASICK_NEMURI: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriAlready, pokeID ); break;
    case WAZASICK_KOORI:  SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriAlready, pokeID ); break;
    case WAZASICK_KONRAN: SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAlready, pokeID ); break;
    default:
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
    }
    return;

  case BTL_ADDSICK_FAIL_NO_EFFECT:
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
    return;

  default:
    break;
  }

  SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Z�b�g���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------
static void scPut_Message_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID )
{
  u8 pokeID = BPP_GetID( bpp );
  SCQUE_PUT_MSG_SET( wk->que, strID, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �W�����b�Z�[�W�\���i�����g���j
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
 * [Put] �Z�b�g���b�Z�[�W�\���i�����g���j
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
 * [Put] �|�P����HP�����炷
 *
 * @param   wk
 * @param   bpp
 * @param   value
 */
//----------------------------------------------------------------------------------
static void scPut_DecreaseHP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 value )
{
  if( !BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_HP_CONST) )
  {
    BPP_HpMinus( bpp, value );
    SCQUE_PUT_OP_HpMinus( wk->que, BPP_GetID(bpp), value );
  }
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
static void scPut_RecoverPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8 wazaIdx, u8 volume, BOOL fOrgWaza )
{
  u8 pokeID = BPP_GetID( bpp );
  WazaID waza;

  if( fOrgWaza )
  {
    waza = BPP_WAZA_IncrementPP_Org( bpp, wazaIdx, volume );
    SCQUE_PUT_OP_PPPlus_Org( wk->que, pokeID, wazaIdx, volume );
  }
  else
  {
    waza = BPP_WAZA_IncrementPP( bpp, wazaIdx, volume );
    SCQUE_PUT_OP_PPPlus( wk->que, pokeID, wazaIdx, volume );
  }
}
//----------------------------------------------------------------------------------
/**
 * [Put] ���݂̂�H�ׂ�A�N�V����
 */
//----------------------------------------------------------------------------------
static void scPut_UseItemAct( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  SCQUE_PUT_ACT_KINOMI( wk->que, BPP_GetID(bpp) );
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
  BPP_CONTFLAG_Set( bpp, flag );
  SCQUE_PUT_OP_SetContFlag( wk->que, BPP_GetID(bpp), flag );
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
  BPP_CONTFLAG_Clear( bpp, flag );
  SCQUE_PUT_OP_ResetContFlag( wk->que, BPP_GetID(bpp), flag );
}
//----------------------------------------------------------------------------------
/**
 * [Put] �^�[���t���O�Z�b�g
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
 * [Put] �^�[���t���O���Z�b�g
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
//----------------------------------------------------------------------------------
/**
 * [Put] �C�����[�W�����Đݒ�
 *
 * @param   wk
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void scPut_IllusionSet( BTL_SVFLOW_WORK* wk, CLIENTID_REC* rec )
{
  u32 i;
  BTL_PARTY* party;
  for(i=0; i<rec->count; ++i)
  {
    party = BTL_POKECON_GetPartyData( wk->pokeCon, rec->clientID[i] );
    SCQUE_PUT_OP_SetIllusionForParty( wk->que, rec->clientID[i] );
    BTL_MAIN_SetIllusionForParty(  wk->mainModule, party );
  }
}

//---------------------------------------------------------------------------------------------
//
// �yEvent�z
//
// ���C�������w�ł̔���ɕK�v�Ȋe������̌�����s���B
// �n���h���ނ̌Ăяo���̓C�x���g�w����̂ݍs����B
// �|�P�����p�����[�^�̑���͍s��Ȃ��B
// �R�}���h�o�͍͂s��Ȃ��B
//
//---------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * [Event] �����o�[����ւ��O�ɓ���|�P�����̃A�N�V���������荞�܂��邩�`�F�b�N
 *
 * @param   wk
 * @param   outPoke
 *
 * @retval  u32        ���荞�݂����|�P������
 */
//----------------------------------------------------------------------------------
static u32 scEvent_MemberChangeIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* outPoke )
{
  u32 i;

  // �P�x�A�������|�P�����̑S���U�n���h����o�^���Ă���
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( (wk->actOrder[i].bpp != outPoke) && (wk->actOrder[i].fDone == FALSE) )
    {
      WazaID waza = ActOrder_GetWazaID( &wk->actOrder[i] );
      if( waza != WAZANO_NULL ){
        if( BTL_HANDLER_Waza_Add( wk->actOrder[i].bpp, waza ) ){
          wk->actOrder[i].fIntrCheck = TRUE;
        }
      }
    }
  }

  wk->MemberOutIntrPokeCount = 0;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1, BPP_GetID(outPoke) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MENBERCHANGE_INTR );
  BTL_EVENTVAR_Pop();

  // �������|�P�����̑S���U�n���h�����폜
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( wk->actOrder[i].fIntrCheck)
    {
      WazaID waza = ActOrder_GetWazaID( &wk->actOrder[i] );
      if( waza != WAZANO_NULL ){
        BTL_HANDLER_Waza_RemoveForce( wk->actOrder[i].bpp, waza );
      }
      wk->actOrder[i].fIntrCheck = FALSE;
    }
  }

  return wk->MemberOutIntrPokeCount;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���̃��U���Ăяo�����U�Ȃ�A�Ăяo�����U�̃p�����[�^���Z�b�g����
 *
 * @param   wk
 * @param   attacker
 * @param   orgWazaid
 * @param   orgTargetPos
 * @param   reqWaza         [io] ���ɃA�N�V���������������ݒ肳��Ă���
 *
 * @retval  BOOL    ���̃��U���Ăяo�����U���̂��o���̂Ɏ��s����ꍇFALSE�^����ȊOTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_GetReqWazaParam( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
  WazaID orgWazaid, BtlPokePos orgTargetPos, REQWAZA_WORK* reqWaza )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEPOS_ORG, orgTargetPos );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, WAZANO_NULL );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEPOS, BTL_POS_NULL );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_PARAM );

    reqWaza->wazaID = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
    reqWaza->targetPos = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEPOS );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );

  BTL_EVENTVAR_Pop();

  return !failFlag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �g���������h��I�񂾂Ƃ��̓���D��x�𔻒�
 *
 * @param   wk
 * @param   attacker
 *
 */
//----------------------------------------------------------------------------------
static u8 scEvent_CheckSpecialActPriority( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  u8 spPri;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SP_PRIORITY, BTL_SPPRI_DEFAULT );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_SP_PRIORITY );
    spPri = BTL_EVENTVAR_GetValue( BTL_EVAR_SP_PRIORITY );
  BTL_EVENTVAR_Pop();

  return spPri;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ����D��x�̌��ʂ���������
 *
 * @param   wk
 * @param   attacker
 */
//----------------------------------------------------------------------------------
static void scEvent_SpecialActPriorityWorked( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WORKED_SP_PRIORITY );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] �^�[���ŏ��̃��U�����V�[�P���X���O
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
 * [Event] ������_���[�W�v�Z
 *
 * @param   wk
 * @param   attacker
 *
 * @retval  u16   �_���[�W�l
 */
//--------------------------------------------------------------------------
static u32 scproc_CalcConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker )
{
  u32 atk, guard, level, ratio;
  u16 dmg;
  BppKoraeruCause  korae_cause;

  atk = BPP_GetValue( attacker, BPP_ATTACK );
  guard = BPP_GetValue( attacker, BPP_DEFENCE );
  level = BPP_GetValue( attacker, BPP_LEVEL );

  dmg = BTL_CALC_DamageBase( 40, atk, level, guard );

  ratio = 100 - BTL_CALC_GetRand( 16 );
  dmg = (dmg * ratio) / 100;
  if( dmg == 0 ){
    dmg = 1;
  }

  korae_cause = scEvent_CheckKoraeru( wk, attacker, attacker, FALSE, &dmg );
  scPut_ConfDamage( wk, attacker, dmg );

  if( korae_cause != BPP_KORAE_NONE ){
    scproc_Koraeru( wk, attacker, korae_cause );
  }
  return dmg;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����U�Ăяo�����ȂǁA�Ǝ��̃��U���b�Z�[�W���o�͂��邩����
 *
 * @param   wk
 * @param   attacker    ���U���o���|�P����
 * @param   orgWazaID   �Ăяo�����̃��UID�iex. ��т��ӂ�j
 * @param   actWazaID   �Ăяo����̃��UID
 * @param   str         [out] �Ǝ����U���b�Z�[�W�p�����[�^���i�[
 *
 * @retval  BOOL    �Ǝ����U���b�Z�[�W���o�͂���ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckWazaMsgCustom( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker,
  WazaID orgWazaID, WazaID actWazaID, BTL_HANDEX_STR_PARAMS* str )
{
  BOOL result;

  HANDEX_STR_Clear( str );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, actWazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WORK_ADRS, (int)str );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_REQWAZA_MSG );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return result;
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
static void scEvent_GetWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, WAZADATA_GetType(waza) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_USER_TYPE, BPP_GetPokeType(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE_TYPE, WAZADATA_GetDamageType(waza) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_TARGET_TYPE, WAZADATA_GetParam(waza, WAZAPARAM_TARGET) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, FALSE );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_PARAM );

    param->wazaID      = waza;
    param->wazaType    = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_TYPE );
    param->userType    = BTL_EVENTVAR_GetValue( BTL_EVAR_USER_TYPE );
    param->damageType  = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE_TYPE );
    param->targetType  = BTL_EVENTVAR_GetValue( BTL_EVAR_TARGET_TYPE );
    param->flags = 0;
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_FLAT_FLAG) ){
      param->wazaType = POKETYPE_NULL;
    }

    /*
      if( waza != WAZANO_NOROI ){
        param->targetType = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );
      }else{
        param->targetType = BTL_CALC_GetNoroiTargetType( attacker );
      }
    */

  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�������s�m��
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
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_FAIL_CAUSE, cause );
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
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_WazaExecuteStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, BTL_POKESET* rec )
{
  BOOL fQuit = FALSE;
  u32 targetCnt = BTL_POKESET_GetCount( rec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    {
      u32 i;
      const BTL_POKEPARAM* bpp;
      for(i=0; i<targetCnt; ++i){
        bpp = BTL_POKESET_Get( rec, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(bpp) );
      }
    }
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, fQuit );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXE_START );
    fQuit = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return fQuit;
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
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, result );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_MAMORU_BREAK );
    result = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return result;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U���O�ꂽ
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
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, waza );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, result );
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
static BOOL scEvent_TameStart( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* targetRec, WazaID waza, u8* hideTargetPokeID, BOOL* fFailMsgDisped )
{
  const BTL_POKEPARAM* target;
  u32 targetCnt, i;
  BOOL fFail = FALSE;

  targetCnt = BTL_POKESET_GetCount( targetRec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    if( targetCnt )
    {
      u32 i;
      for(i=0; i<targetCnt; ++i)
      {
        target = BTL_POKESET_Get( targetRec, i );
        BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(target) );
      }
    }
    else{
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1, BTL_POKEID_NULL );
    }
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_MSG_FLAG, FALSE );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_START );

    if( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) ){
      *hideTargetPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_TARGET1 );
    }else{
      *hideTargetPokeID = BTL_POKEID_NULL;
    }
    *fFailMsgDisped = BTL_EVENTVAR_GetValue( BTL_EVAR_MSG_FLAG );
    fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );

  BTL_EVENTVAR_Pop();

  return !fFail;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���߃^�[���X�L�b�v�m��
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 */
//----------------------------------------------------------------------------------
static void scEvent_TameSkip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_SKIP );
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
static void scEvent_TameRelease( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKESET* rec, WazaID waza )
{
  const BTL_POKEPARAM* target;
  u32 targetCnt, i;

  targetCnt = BTL_POKESET_GetCount( rec );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TARGET_POKECNT, targetCnt );
    for(i=0; i<targetCnt; ++i)
    {
      target = BTL_POKESET_Get( rec, i );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_TARGET1+i, BPP_GetID(target) );
    }
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TAME_RELEASE );
  BTL_EVENTVAR_Pop();
}

/**
 *  ������Ƃԏ�ԂȂǁA��ʂ�������Ă��邩�`�F�b�N
 */
static BppContFlag CheckPokeHideState( const BTL_POKEPARAM* bpp )
{
  static const BppContFlag  hideState[] = {
    BPP_CONTFLG_SORAWOTOBU, BPP_CONTFLG_ANAWOHORU, BPP_CONTFLG_DIVING, BPP_CONTFLG_SHADOWDIVE,
  };
  u32 i;

  for(i=0; i<NELEMS(hideState); ++i)
  {
    if( BPP_CONTFLAG_Get(bpp, hideState[i]) )
    {
      return hideState[ i ];
    }
  }
  return BPP_CONTFLG_NULL;
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
  BOOL avoidFlag = FALSE;

  BppContFlag  hideState = CheckPokeHideState( defender );
  if( hideState != BPP_CONTFLG_NULL )
  {
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKE_HIDE, hideState );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_AVOID_FLAG, TRUE );

      BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_POKE_HIDE );
      if( BTL_EVENTVAR_GetValue(BTL_EVAR_AVOID_FLAG) ){
        avoidFlag = TRUE;
      }
    BTL_EVENTVAR_Pop();
  }

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
static BOOL scEvent_IsCalcHitCancel( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
  BOOL result = WAZADATA_IsAlwaysHit( waza );
  if( !result )
  {
    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_HIT_CANCEL );
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
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u32 per = WAZADATA_GetParam( wazaParam->wazaID, WAZAPARAM_HITPER );
  fx32 ratio = FX32_CONST(1);

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_HIT_PER, per );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RATIO );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_HIT_PER );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  per = BTL_CALC_MulRatio( per, ratio );
  if( per > 100 ){
    per = 100;
  }

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
  u8 atkLevel = BPP_GetValue( attacker, BPP_LEVEL );
  u8 defLevel = BPP_GetValue( defender, BPP_LEVEL );

  // �U�������x�����h�䑤���x�������Ȃ��Ɏ��s
  if( atkLevel < defLevel )
  {
    return FALSE;
  }

  {
    BOOL ret = FALSE;
    BOOL fFail = FALSE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ICHIGEKI_CHECK );
      fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    BTL_EVENTVAR_Pop();

    // �Ƃ��������ɂ�鋭��������������Ύ��s
    if( fFail ){
      return FALSE;
    }

    if( IsMustHit(attacker, defender) ){
      ret = TRUE;
    }else{
      u8 per = WAZADATA_GetParam( waza, WAZAPARAM_HITPER );
      BTL_Printf("�f�t�H������=%d\n", per);
      per += (atkLevel - defLevel);
      ret = perOccur( wk, per );
      BTL_Printf("���x���␳������=%d, hit=%d\n", per, ret);
    }
    return ret;
  }
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
static BOOL scEvent_CheckDmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam )
{
  BOOL ret = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, ret );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER_CHECK );
    ret = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();
  return ret;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �_���[�W���U�񕜉�����
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 */
//----------------------------------------------------------------------------------
static void scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DMG_TO_RECOVER_FIX );
  BTL_EVENTVAR_Pop();
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
  u16  rank = WAZADATA_GetParam( waza, WAZAPARAM_CRITICAL_RANK );
  rank += BPP_GetCriticalRank( attacker );

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_RANK, rank );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CRITICAL_CHECK );
    if( !BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG ) )
    {
      if( WAZADATA_IsMustCritical(waza) ){
        flag = TRUE;
      }else{
        rank = roundMax( BTL_EVENTVAR_GetValue(BTL_EVAR_CRITICAL_RANK), BTL_CALC_CRITICAL_MAX );
        flag = BTL_CALC_CheckCritical( rank );
      }

      if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_MUST_CRITICAL) ){
        flag = TRUE;
      }

    }
    else{
      flag = FALSE;
    }
  BTL_EVENTVAR_Pop();
  return flag;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ��_���[�W��^������̔����v�Z�i�_���[�W�~�������j
 *
 * @param   wk
 * @param   attacker
 * @param   waza
 * @param   damage
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage, BOOL* fMustEnable )
{
  u8 pokeID = BPP_GetID( attacker );
  u8 ratio = WAZADATA_GetParam( waza, WAZAPARAM_DAMAGE_REACTION_RATIO );
  u8 ratio_ex = 0;
  u8 fail_flag = FALSE;
  u8 anti_fail_flag = FALSE;
  SV_ReactionType  reactionType = SV_REACTION_NONE;

  *fMustEnable = FALSE;

  if( ratio )
  {
    reactionType = SV_REACTION_DAMAGE;
  }
  else
  {
    ratio = WAZADATA_GetParam( waza, WAZAPARAM_HP_REACTION_RATIO );
    if(  ratio ){
      reactionType = SV_REACTION_HP;
    }
  }

  //�h���C���n�͔����ɂȂ�Ȃ��悤�ɂ���
  if( WAZADATA_GetCategory(waza) == WAZADATA_CATEGORY_DRAIN )
  {
    return 0;
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, pokeID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO_EX, 0 );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_KICKBACK );

    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    anti_fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
    ratio_ex = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO_EX );
  BTL_EVENTVAR_Pop();

  if( !anti_fail_flag )
  {
    if( fail_flag == TRUE ){
      return 0;
    }
  }
  ratio += ratio_ex;

  if( ratio )
  {
    if( reactionType == SV_REACTION_HP )
    {
      u32 maxHP = BPP_GetValue( attacker, BPP_MAX_HP );
      damage = roundMin( BTL_CALC_MulRatioInt(maxHP, ratio), 1 );
    }
    else{
      damage = roundMin( BTL_CALC_MulRatioInt(damage, ratio), 1 );
    }

    (*fMustEnable) = anti_fail_flag;

    return damage;
  }
  return 0;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����A�C�e���g�p
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemEquip( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_USE_ITEM );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �ꎞ�I�ɓo�^�����A�C�e���̎g�p�i���΂ނȂǁj
 *
 * @param   wk
 * @param   bpp
 * @param   evwk
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemEquipTmp( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u8 atkPokeID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    BTL_EVENT_ForceCallHandlers( wk, BTL_EVENT_USE_ITEM_TMP );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * ���U�̑����v�Z�i�_���[�W�E�ꌂ���U�̂݁j
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   wazaParam
 * @param   fNoEffectMsg    ���ʂ��������U�̏ꍇ�A������������b�Z�[�W�\�����s��
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff scProc_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk,
  BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BOOL fNoEffectMsg )
{
  BOOL fFloating = FALSE;

  // �������̍U���i��邠�����j�́A��ɓ��{�Ńq�b�g
  if( wazaParam->wazaType == POKETYPE_NULL ){
    return BTL_TYPEAFF_100;
  }

  // �ӂ䂤��Ԃ̃|�P�����ɒn�ʃ_���[�W���U�͌��ʂ��Ȃ�
  if( wazaParam->wazaType == POKETYPE_JIMEN )
  {
    fFloating = scproc_CheckFloating(wk, defender);
    if( fFloating )
    {
      if( fNoEffectMsg ){
        scproc_WazaNoEffectByFlying( wk, defender );
      }
      return BTL_TYPEAFF_0;
    }
  }

  {
    BtlTypeAff affinity;
    PokeTypePair defPokeTypePair = BPP_GetPokeType( defender );
    PokeType  defPokeType = PokeTypePair_GetType1( defPokeTypePair );

    affinity = scEvent_CheckDamageAffinity( wk, attacker, defender, wazaParam->wazaType, defPokeType );
    if( !PokeTypePair_IsPure(defPokeTypePair) )
    {
      PokeTypePair  type2 = PokeTypePair_GetType2( defPokeTypePair );
      BtlTypeAff    aff2 = scEvent_CheckDamageAffinity( wk, attacker, defender, wazaParam->wazaType, type2 );
      affinity = BTL_CALC_TypeAffMul( affinity, aff2 );
    }

    if( affinity == BTL_TYPEAFF_0 )
    {
      // �n�ʃ��U�ő��������̏ꍇ = ��s�^�C�v�Ȃ̂ŁA�ӂ䂤��ԂɂȂ��Ȃ瓙�{�q�b�g
      if( (wazaParam->wazaType == POKETYPE_JIMEN) && !(fFloating) )
      {
        TAYA_Printf("�n�ʂő������������Ǖ����ĂȂ����瓖�����\n");
        return BTL_TYPEAFF_100;
      }

      if( fNoEffectMsg && (affinity == BTL_TYPEAFF_0) )
      {
        scput_WazaNoEffect( wk, defender );
      }
    }
    return affinity;
  }
}
//----------------------------------------------------------------------------------
/**
 * �|�P�����P�̂ӂ䂤��Ԃ̃`�F�b�N
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static BOOL scproc_CheckFloating( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  // ���イ��傭�������Ă�����N�������Ȃ��̂Ń`�F�b�N���Ȃ�
  if( !BTL_FIELD_CheckEffect(BTL_FLDEFF_JURYOKU) )
  {
    if( scEvent_CheckFloating(wk, bpp) ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �ӂ䂤��ԃ`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    �L���ł����TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckFloating( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  // �����ł�"���イ��傭"�̗L�����`�F�b�N���Ȃ��B
  // �i"���イ��傭��������Ɂu���߂�ɂ������v�e�L�X�g�\���̃`�F�b�N�Ɏg�������̂Łj
  {
    u8 floatFlag = BPP_IsMatchType( bpp, POKETYPE_HIKOU );
    u8 failFlag = FALSE;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, floatFlag );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_FLYING );
      BTL_SICKEVENT_CheckFlying( wk, bpp );
      floatFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
      failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    BTL_EVENTVAR_Pop();

    if( failFlag ){
      return FALSE;
    }
    return floatFlag;
  }
}
//----------------------------------------------------------------------------------
/**
 * �ӂ䂤��ԂŒn�ʃ��U�������������̃��b�Z�[�W�\��
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scproc_WazaNoEffectByFlying( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  scEvent_WazaNoEffectByFlying( wk, bpp );

  if( scproc_HandEx_Result(wk) != HandExResult_Enable ){
    scput_WazaNoEffect( wk, bpp );
  }

  BTL_Hem_PopState( &wk->HEManager, hem_state );
}
//----------------------------------------------------------------------------------
/**
 * [Event] �ӂ䂤��Ԃɂ��n�ʃ��U����������
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaNoEffectByFlying( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_NOEFF_BY_FLYING );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�����v�Z
 *
 * @param   wk
 * @param   attacker
 * @param   defender
 * @param   waza_type
 * @param   poke_type
 *
 * @retval  BtlTypeAff
 */
//----------------------------------------------------------------------------------
static BtlTypeAff scEvent_CheckDamageAffinity( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, PokeType waza_type, PokeType poke_type )
{
  BOOL  flatFlag, flatMasterFlag;
  BtlTypeAff affinity;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKE_TYPE, poke_type );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, waza_type );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLAT_FLAG, FALSE );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FLATMASTER_FLAG, FALSE );

    BTL_SICKEVENT_CheckNotEffectByType( wk, defender );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_AFFINITY );

    flatFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FLAT_FLAG );
    flatMasterFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FLATMASTER_FLAG );
  BTL_EVENTVAR_Pop();

  if( flatMasterFlag ){
    return BTL_TYPEAFF_100;
  }

  affinity = BTL_CALC_TypeAff( waza_type, poke_type );
  if( (affinity == BTL_TYPEAFF_0) && (flatFlag) ){
    return BTL_TYPEAFF_100;
  }

  return affinity;
}


//----------------------------------------------------------------------------------
/**
 * �G�t�F�N�g����
 *
 * @param   wk
 * @param   effectNo    �G�t�F�N�g�i���o�[
 * @param   pos_from    �G�t�F�N�g�J�n�ʒu�i�s�v�Ȃ�BTL_POS_NULL)
 * @param   pos_to      �G�t�F�N�g�I���ʒu�i�s�v�Ȃ�BTL_POS_NULL)
 * @param   fQueResereved   �\��ς�Que�̈�𗘗p����t���O
 * @param   reservedPos     �\��ς�Que�ʒu
 */
//----------------------------------------------------------------------------------
static void scproc_ViewEffect( BTL_SVFLOW_WORK* wk, u16 effectNo, BtlPokePos pos_from, BtlPokePos pos_to, BOOL fQueResereved, u32 reservedPos )
{
  enum {
    EFF_SIMPLE = 0, // �ʒu�w��Ȃ�
    EFF_POS,        // �J�n�ʒu�̂ݎw��
    EFF_VECTOR,     // �J�n�E�I�[�ʒu�Ƃ��Ɏw��
  };

  int effType;

  if( pos_to != BTL_POS_NULL ){
    effType = EFF_VECTOR;
  }else if( pos_from != BTL_POS_NULL ) {
    effType = EFF_POS;
  }else{
    effType = EFF_SIMPLE;
  }

  // �\��̈�֏�������
  if( fQueResereved )
  {
    switch( effType ){
    case EFF_SIMPLE:
      SCQUE_PUT_ReservedPos( wk->que, reservedPos, SC_ACT_EFFECT_SIMPLE, effectNo );
      break;

    case EFF_POS:
      SCQUE_PUT_ReservedPos( wk->que, reservedPos, SC_ACT_EFFECT_BYPOS,
          pos_from, effectNo );
      break;

    case EFF_VECTOR:
      SCQUE_PUT_ReservedPos( wk->que, reservedPos, SC_ACT_EFFECT_BYVECTOR,
          pos_from, pos_to, effectNo );
      break;
    }
  }
  // �Ō���ɒǋL
  else
  {
    switch( effType ){
    case EFF_SIMPLE:
      SCQUE_PUT_ACT_EffectSimple( wk->que, effectNo );
      break;

    case EFF_POS:
      SCQUE_PUT_ACT_EffectByPos( wk->que, pos_from, effectNo );
      break;

    case EFF_VECTOR:
      SCQUE_PUT_ACT_EffectByVector( wk->que, pos_from, pos_to, effectNo );
      break;
    }
  }
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
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID,   BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_IDX, wazaIdx );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, flag );
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
 * @param   param            [out] �q�b�g�`�F�b�N�p�����[�^
 *
 * @retval  BOOL    ������q�b�g���郏�U�Ȃ�TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_HitCheckParam( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, HITCHECK_PARAM* param )
{
  u8 max = WAZADATA_GetParam( waza, WAZAPARAM_HITCOUNT_MAX );

  param->count = 0;
  param->pluralHitAffinity = BTL_TYPEAFF_100;

  if( max > 1 )
  {
    u8 hit_cnt = BTL_CALC_HitCountStd( max );

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_HITCOUNT_MAX, max );
      BTL_EVENTVAR_SetValue( BTL_EVAR_HITCOUNT, hit_cnt );
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );   // �ő�܂Ńq�b�g������t���O
      BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_AVOID_FLAG, FALSE ); // �P��q�b�g���Ƃɖ���������s���t���O

      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_COUNT );

      // �����I�ɍő�񐔂܂Ńq�b�g
      if( ( max <= 5)
      &&  ( BTL_EVENTVAR_GetValue(BTL_EVAR_GEN_FLAG) )
      ){
        param->countMax = max;
        param->fCheckEveryTime = FALSE;
      }
      else
      {
        param->countMax = BTL_EVENTVAR_GetValue( BTL_EVAR_HITCOUNT );
        param->fCheckEveryTime = BTL_EVENTVAR_GetValue( BTL_EVAR_AVOID_FLAG );
      }
      param->fPluralHitWaza = TRUE;

    BTL_EVENTVAR_Pop();

    return TRUE;
  }
  else
  {
    param->countMax = 1;
    param->fCheckEveryTime = FALSE;
    param->fPluralHitWaza = FALSE;
    return FALSE;
  }
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
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam )
{
  u16 power = WAZADATA_GetPower( wazaParam->wazaID );
  fx32 ratio;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_DAMAGE_TYPE, wazaParam->damageType );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_WAZA_POWER, power );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER_BASE );

    BTL_EVENTVAR_SetMulValue( BTL_EVAR_WAZA_POWER_RATIO, FX32_CONST(1.0), FX32_CONST(0.01f), FX32_CONST(512) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER );
    power = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER_RATIO );
  BTL_EVENTVAR_Pop();

  power = BTL_CALC_MulRatio( power, ratio );
  BTL_N_Printf( DBGSTR_SVFL_WazaPower, wazaParam->wazaID, power, ratio);
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
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, TRUE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SIMPLE_DAMAGE_ENABLE );
    flag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();

  return flag;
}

// �U�����\�͒l�擾
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag )
{
  BppValueID vid = (WAZADATA_GetDamageType(wazaParam->wazaID) == WAZADATA_DMG_SPECIAL)? BPP_SP_ATTACK : BPP_ATTACK;

  {
    const BTL_POKEPARAM* calc_attacker = attacker;
    fx32 ratio;
    u16 power;

    BTL_EVENTVAR_Push();
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
      BTL_EVENTVAR_SetValue( BTL_EVAR_SWAP_POKEID, BTL_POKEID_NULL );
      BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );    //
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER_PREV );

      {
        u8 swapPokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_SWAP_POKEID );
        if( swapPokeID != BTL_POKEID_NULL ){
          calc_attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, swapPokeID );
        }
      }

      // �N���e�B�J���`�F�b�N
      if( BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG ) ){
        power = BPP_GetValue_Base( calc_attacker, vid );
      }
      else
      {
        if( criticalFlag ){
          power = BPP_GetValue_Critical( calc_attacker, vid );
        }else{
          power = BPP_GetValue( calc_attacker, vid );
        }
      }

      BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
      BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
      BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, power );
      BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_ONE, FX32_CONST(0.1), FX32_CONST(32) );
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER );
      power = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
      ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
      power = BTL_CALC_MulRatio( power, ratio );
      BTL_N_Printf( DBGSTR_SVFL_AtkPower, power, criticalFlag, ratio);
    BTL_EVENTVAR_Pop();
    return power;
  }
}

// �h�䑤�\�͒l�擾
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
  const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender,
  const SVFL_WAZAPARAM* wazaParam, BOOL criticalFlag )
{
  BppValueID vid = (WAZADATA_GetDamageType(wazaParam->wazaID) == WAZADATA_DMG_SPECIAL)? BPP_SP_DEFENCE : BPP_DEFENCE;
  fx32 ratio = FX32_CONST(1);
  u16 guard;
  u8 forceFlatFlag;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VID, vid );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VID_SWAP_CNT, 0 );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, FALSE );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD_PREV );
    if( BTL_EVENTVAR_GetValue(BTL_EVAR_VID_SWAP_CNT) & 1 ){
      vid = (vid == BPP_DEFENCE)? BPP_SP_DEFENCE : BPP_DEFENCE;
    }
    forceFlatFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );
  BTL_EVENTVAR_Pop();


  if( forceFlatFlag ){
    guard = BPP_GetValue_Base( defender, vid );
  }else{
    if( criticalFlag ){
      guard = BPP_GetValue_Critical( defender, vid );
    }else{
      guard = BPP_GetValue( defender, vid );
    }
  }

  // �Ă񂱂��u���Ȃ��炵�v�̎��A����^�C�v�̂Ƃ��ڂ�1.5�{
  if( (scEvent_GetWeather(wk) == BTL_WEATHER_SAND)
  &&  (BPP_IsMatchType(defender, POKETYPE_IWA))
  &&  (vid == BPP_SP_DEFENCE)
  ){
    guard = BTL_CALC_MulRatio( guard, FX32_CONST(1.5) );
  }

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(defender) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, ratio, FX32_CONST(0.1), FX32_CONST(32) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD );
    guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  guard = BTL_CALC_MulRatio( guard, ratio );
  BTL_N_Printf( DBGSTR_SVFL_DefGuard, guard, criticalFlag, ratio);
  return guard;
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
  BOOL f_match = BPP_IsMatchType( attacker, waza_type );
  fx32 ratio = FX32_CONST(1);

  BTL_EVENTVAR_Push();
  {
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_GEN_FLAG, f_match );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TYPEMATCH_CHECK );
    f_match = BTL_EVENTVAR_GetValue( BTL_EVAR_GEN_FLAG );

    if( f_match ){
      ratio = FX32_CONST(1.5f);
    }

    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TYPEMATCH_FLAG, f_match );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TYPEMATCH_RATIO );

    ratio = (fx32)BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  }
  BTL_EVENTVAR_Pop();
  return ratio;
}
//--------------------------------------------------------------------------
/**
 * [Event] �����o�[����ɓo�ꂳ��������
 *
 * @param   wk
 * @param   bpp
 * @param   fAllIn  �S����ē��ꏈ���t���O�i�o�g���J���P��̂݁j
 *
 */
//--------------------------------------------------------------------------
static void scEvent_AfterMemberIn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_MemberInEventBegin, BPP_GetID(bpp) );
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_IN );
  BTL_EVENTVAR_Pop();
  BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_MemberInEventEnd, BPP_GetID(bpp) );
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ǉ����ʂɂ��Ђ�݊m���擾
 *
 * @param   wk
 * @param   waza
 * @param   attacker
 *
 * @retval  u32
 */
//----------------------------------------------------------------------------------
static u32 scEvent_GetWazaShrinkPer( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker )
{
  BOOL fFail = FALSE;
  u32 per = WAZADATA_GetParam( waza, WAZAPARAM_SHRINK_PER );
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fFail );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_SHRINK_PER );
    fFail = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
  BTL_EVENTVAR_Pop();
  if( fFail ){
    return 0;
  }
  return per;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �Ђ�ݔ�������
 *
 * @param   wk
 * @param   target
 * @param   waza    ���U�ǉ����ʂɂ��ꍇ���UID�i����ȊOWAZANO_NULL�j
 * @param   per     �Ђ�݊m��
 *
 * @retval  BOOL    �Ђ�ނȂ�TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckShrink( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, u32 per )
{
  BOOL fail_flag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, BPP_GetID(target) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, fail_flag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_CHECK );

    per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
    fail_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  if( fail_flag ){
    return FALSE;
  }
  else if( per > 0 )
  {
    if( BTL_SVFTOOL_GetDebugFlag(wk, BTL_DEBUGFLAG_MUST_TUIKA) ){
      return TRUE;
    }

    return perOccur( wk, per );
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �Ђ�ݎ��s�����\��
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
 * [Event] �����N�������ʂ̍ŏI���ۃ`�F�b�N
 *
 * @param   wk
 * @param   target
 * @param   effect
 * @param   volume
 *
 * @retval  BOOL      ��������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaRankEffect effect, u8 wazaUsePokeID, int volume )
{
  BOOL failFlag = FALSE;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, wazaUsePokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, effect );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failFlag );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_LAST_CHECK );
    failFlag = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return !failFlag;
//  return evwk->failFlag == FALSE;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �����N�������ʂ̎��s�m��
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
 * [Event] �����N�������ʂ̐����m��
 *
 * @param   wk
 * @param   atkPokeID   ���ʂ𔭐����������U�E�Ƃ������̎�̃|�PID�i���Ȃ��ꍇBTL_POKEID_NULL�j
 * @param   bpp
 * @param   rankType
 * @param   volume
 */
//----------------------------------------------------------------------------------
static void scEvent_RankEffect_Fix( BTL_SVFLOW_WORK* wk, u8 atkPokeID,
  const BTL_POKEPARAM* bpp, WazaRankEffect rankType, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, atkPokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, rankType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RANKEFF_FIXED );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�ɂ�郉���N�������ʂ̐����m��
 *
 * @param   wk
 * @param   target
 * @param   wazaID
 * @param   effectID
 * @param   volume
 */
//----------------------------------------------------------------------------------
static void scEvent_WazaRankEffectFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
  WazaID wazaID, WazaRankEffect effectID, int volume )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(target) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_STATUS_TYPE, effectID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_VOLUME, volume );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_RANKEFF_FIXED );
  BTL_EVENTVAR_Pop();
}

//--------------------------------------------------------------------------
/**
 * [Event] �h���C���U���̃h���C��HP�ʂ��Čv�Z
 *
 * @param   wk
 * @param   attacker
 * @param   target
 * @param   volume     �f�t�H���g�񕜗�
 *
 * @retval  int     �񕜗�
 */
//--------------------------------------------------------------------------
static u16 scEvent_RecalcDrainVolume( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, u16 volume )
{
  fx32 ratio;
  u8   targetPokeID = (target!=NULL)? BPP_GetID(target) : BTL_POKEID_NULL;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_DEF, targetPokeID );
    BTL_EVENTVAR_SetMulValue( BTL_EVAR_RATIO, FX32_CONST(1), FX32_CONST(0.1), FX32_CONST(32) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );

    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_DRAIN );
    volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
    ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
    if( volume != 0 ){
      volume = BTL_CALC_MulRatio_OverZero( volume, ratio );
    }
  BTL_EVENTVAR_Pop();

  return volume;
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
  BOOL fDisable = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WEATHER, weather );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE );
    fDisable = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
    if( !fDisable ){
      BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE_AFTER );
    }
  BTL_EVENTVAR_Pop();
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
  u32 base_ratio = WAZADATA_GetParam( waza, WAZAPARAM_HP_RECOVER_RATIO );
  fx32 ex_ratio;

  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, 0 );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_RECOVER_HP_RATIO );
    ex_ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
  BTL_EVENTVAR_Pop();

  {
    u32 maxHP = BPP_GetValue( bpp, BPP_MAX_HP );
    u32 volume;

//    TAYA_Printf("maxHP=%d, ex_ratio=%08x, baes_ratio=%08x\n", maxHP, ex_ratio, base_ratio );

    if( ex_ratio ){
      volume = BTL_CALC_MulRatio( maxHP, ex_ratio );
    }else{
      volume = BTL_CALC_MulRatioInt( maxHP, base_ratio );
    }
    if( volume == 0 ){
      volume = 1;
    }else if( volume > maxHP ){
      volume = maxHP;
    }

    return volume;
  }
}
//----------------------------------------------------------------------------------
/**
 * [Event] �A�C�e�����Z�b�g�i���邢�͏����j�����`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL    ���s����ꍇTRUE
 */
//----------------------------------------------------------------------------------
static BOOL scEvent_CheckItemSet( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 itemID )
{
  BOOL failed = FALSE;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, itemID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FAIL_FLAG, failed );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEMSET_CHECK );
    failed = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_FLAG );
  BTL_EVENTVAR_Pop();

  return failed;
}
//----------------------------------------------------------------------------------
/**
 * [Event] �A�C�e��������������
 *
 * @param   wk
 * @param   bpp
 * @param   nextItemID   ����������̃A�C�e���i���o�[
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemSetDecide( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 nextItemID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_ITEM, nextItemID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEMSET_DECIDE );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------
/**
 * [Event] �A�C�e��������������
 *
 * @param   wk
 * @param   bpp
 */
//----------------------------------------------------------------------------------
static void scEvent_ItemSetFixed( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, BPP_GetID(bpp) );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_ITEMSET_FIXED );
  BTL_EVENTVAR_Pop();
}


//----------------------------------------------------------------------------------
/**
 * [Event] �Ƃ������ύX���m��i�ύX�̒��O�j
 *
 * @param   wk
 * @param   pokeID
 * @param   tokuseiID
 */
//----------------------------------------------------------------------------------
static void scEvent_ChangeTokuseiBefore( BTL_SVFLOW_WORK* wk, u8 pokeID, u16 prev_tokuseiID, u16 next_tokuseiID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_PREV, prev_tokuseiID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_TOKUSEI_NEXT, next_tokuseiID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHANGE_TOKUSEI_BEFORE );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �Ƃ��������ύX���ꂽ
 *
 * @param   wk
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
static void scEvent_ChangeTokuseiAfter( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, pokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHANGE_TOKUSEI_AFTER );
  BTL_EVENTVAR_Pop();
}
//----------------------------------------------------------------------------------
/**
 * [Event] �T�C�h�G�t�F�N�g�p���p�����[�^����
 *
 * @param   wk
 * @param   param->effect
 * @param   param->side
 * @param   &param->cont
 */
//----------------------------------------------------------------------------------
static void scEvent_CheckSideEffectParam( BTL_SVFLOW_WORK* wk, u8 userPokeID, BtlSideEffect effect, BtlSide side, BPP_SICK_CONT* cont )
{
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID, userPokeID );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SIDE, side );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_SIDE_EFFECT, effect );
    BTL_EVENTVAR_SetValue( BTL_EVAR_SICK_CONT, cont->raw );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_CHECK_SIDEEFF_PARAM );
    cont->raw = BTL_EVENTVAR_GetValue( BTL_EVAR_SICK_CONT );
  BTL_EVENTVAR_Pop();
}

//----------------------------------------------------------------------------------------------------------
// �����q�b�g�J�E���^
//----------------------------------------------------------------------------------------------------------

/**
 *  �N���A
 */
static void AffCounter_Clear( WAZA_AFF_COUNTER* cnt )
{
  GFL_STD_MemClear( cnt, sizeof(WAZA_AFF_COUNTER) );
}

static void AffCounter_CountUp( WAZA_AFF_COUNTER* cnt, BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BtlTypeAff affinity )
{
  u8 atkPokeID = BPP_GetID( attacker );
  u8 defPokeID = BPP_GetID( defender );

  u8 atkClientID = BTL_MAINUTIL_PokeIDtoClientID( atkPokeID );
  u8 defClientID = BTL_MAINUTIL_PokeIDtoClientID( defPokeID );

  u8 fEnemyAttack = !BTL_MAINUTIL_IsFriendPokeID( atkPokeID, defPokeID );

  u16* pCnt = NULL;

  // �v���C���[���U����
  if( atkClientID == BTL_CLIENT_PLAYER )
  {
    if( fEnemyAttack )
    {
      if( affinity == BTL_TYPEAFF_0 )
      {
        pCnt = &(cnt->putVoid);
      }
      else if( affinity > BTL_TYPEAFF_100 )
      {
        pCnt = &(cnt->putAdvantage);
      }
      else if( affinity < BTL_TYPEAFF_100 )
      {
        pCnt = &(cnt->putDisadvantage);
      }
    }
  }
  // �v���C���[���h�䑤
  else if( defClientID == BTL_CLIENT_PLAYER )
  {
    if( fEnemyAttack )
    {
      if( affinity == BTL_TYPEAFF_0 )
      {
        pCnt = &(cnt->recvVoid);
      }
      else if( affinity > BTL_TYPEAFF_100 )
      {
        pCnt = &(cnt->recvAdvantage);
      }
      else if( affinity < BTL_TYPEAFF_100 )
      {
        pCnt = &(cnt->recvDisadvantage);
      }
    }
  }

  if( pCnt != NULL )
  {
    if( (*pCnt) < AFFCOUNTER_MAX ){
      ++(*pCnt);
    }
  }
}
/**
 *  �U�������J�E���^�F�u���ʂ��Ȃ������v���U���o������
 */
u16 BTL_SVF_GetTypeAffCnt_PutVoid( const BTL_SVFLOW_WORK* wk )
{
  return wk->affCounter.putVoid;
}
/**
 *  �U�������J�E���^�F�u���ʂ̓o�c�O���v���U���o������
 */
u16 BTL_SVF_GetTypeAffCnt_PutAdvantage( const BTL_SVFLOW_WORK* wk )
{
  return wk->affCounter.putAdvantage;
}
/**
 *  �U�������J�E���^�F�u���ʂ͂��܂ЂƂv���U���o������
 */
u16 BTL_SVF_GetTypeAffCnt_PutDisadvantage( const BTL_SVFLOW_WORK* wk )
{
  return wk->affCounter.putDisadvantage;
}
/**
 *  �U�������J�E���^�F�u���ʂ��Ȃ������v���U���󂯂���
 */
u16 BTL_SVF_GetTypeAffCnt_RecvVoid( const BTL_SVFLOW_WORK* wk )
{
  return wk->affCounter.recvVoid;
}
/**
 *  �U�������J�E���^�F�u���ʂ̓o�c�O���v���U���󂯂���
 */
u16 BTL_SVF_GetTypeAffCnt_RecvAdvantage( const BTL_SVFLOW_WORK* wk )
{
  return wk->affCounter.recvAdvantage;
}
/**
 *  �U�������J�E���^�F�u���ʂ͂��܂ЂƂv���U���󂯂���
 */
u16 BTL_SVF_GetTypeAffCnt_RecvDisadvantage( const BTL_SVFLOW_WORK* wk )
{
  return wk->affCounter.recvDisadvantage;
}


//----------------------------------------------------------------------------------------------------------
// �ȉ��A�n���h������̉�����M�֐��ƃ��[�e�B���e�B�Q
//----------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��]�w��ID�̃|�P�����p�����[�^��Ԃ�
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_POKEPARAM*
 */
//--------------------------------------------------------------------------------------
const BTL_POKEPARAM* BTL_SVFTOOL_GetPokeParam( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�PID�����|�P�������퓬�ɏo�Ă��邩�`�F�b�N���A�o�Ă����炻�̐퓬�ʒu��Ԃ�
 *
 * @param   server
 * @param   pokeID
 *
 * @retval  BtlPokePos    �o�Ă���ꍇ�͐퓬�ʒuID�^�o�Ă��Ȃ��ꍇ��BTL_POS_MAX
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFTOOL_GetExistFrontPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�PID�����|�P�������Ō�ɏo�Ă��邽�ʒuID��Ԃ�
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFTOOL_GetPokeLastPos( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POSPOKE_GetPokeLastPos( &wk->pospokeWork, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��ʒu�ɑ��݂��Ă���|�PID��Ԃ��i���݂��Ă��Ȃ��ꍇ�� BTL_POKEID_NULL�j
 *
 * @param   wk
 * @param   pos
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetExistPokeID( BTL_SVFLOW_WORK* wk, BtlPokePos pos )
{
  return BTL_POSPOKE_GetExistPokeID( &wk->pospokeWork, pos );
}

//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ��ɏo�Ă���S�Ă̑��葤�|�P����ID��z��Ɋi�[����
 *
 * @param   wk
 * @param   dst
 *
 * @retval  u8    ��ɏo�Ă���|�P������
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetAllOpponentFrontPokeID( BTL_SVFLOW_WORK* wk, u8 basePokeID, u8* dst )
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
 * [�n���h���p�c�[��] �w��̂Ƃ����������|�P�������퓬�ɏo�Ă��邩�`�F�b�N
 *
 * @param   wk
 * @param   tokusei
 *
 * @retval  BOOL    �o�Ă�����TRUE
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_CheckExistTokuseiPokemon( BTL_SVFLOW_WORK* wk, PokeTokusei tokusei )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    if( BPP_GetValue(bpp, BPP_TOKUSEI_EFFECTIVE) == tokusei )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 *  * [�n���h���p�c�[��] �����v�Z�V�~�����[�g���ʂ�Ԃ�
 *
 * @param   wk
 * @param   atkPokeID
 * @param   defPokeID
 * @param   waza
 *
 * @retval  BtlTypeAff
 */
//--------------------------------------------------------------------------------------
BtlTypeAff BTL_SVFTOOL_SimulationAffinity( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza )
{
  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, atkPokeID );
  BTL_POKEPARAM* defender = BTL_POKECON_GetPokeParam( wk->pokeCon, defPokeID );
  SVFL_WAZAPARAM  wazaParam;
  BtlTypeAff result;

  scEvent_GetWazaParam( wk, waza, attacker, &wazaParam );
  result = scProc_checkWazaDamageAffinity( wk, attacker, defender, &wazaParam, FALSE );

  return result;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �_���[�W�v�Z�V�~�����[�g���ʂ�Ԃ�
 *
 * @param   flowWk
 * @param   atkPokeID       �U���|�PID
 * @param   defPokeID       �h��|�PID
 * @param   waza            ���U
 * @param   fAffinity       �����v�Z���邩�iFALSE�Ȃ瓙�{�Ōv�Z�j
 * @param   fEnableRand     �����L��
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 BTL_SVFTOOL_SimulationDamage( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza, BOOL fAffinity, BOOL fEnableRand )
{
  if( ( waza != WAZANO_NULL )
  &&  ( WAZADATA_IsDamage(waza) )
  ){
    const BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, atkPokeID );
    const BTL_POKEPARAM* defender = BTL_POKECON_GetPokeParam( wk->pokeCon, defPokeID );

    BtlTypeAff  aff;
    BOOL  critical_flag;
    SVFL_WAZAPARAM  wazaParam;
    u16  damage;

    wk->simulationCounter++;

    //
    if( fAffinity ){
      aff = BTL_SVFTOOL_SimulationAffinity( wk, atkPokeID, defPokeID, waza );
    }else{
      aff = BTL_TYPEAFF_100;
    }

    scEvent_GetWazaParam( wk, waza, attacker, &wazaParam );

    scEvent_CalcDamage( wk, attacker, defender, &wazaParam, aff,
      BTL_CALC_DMG_TARGET_RATIO_NONE, FALSE, !fEnableRand, &damage );

    wk->simulationCounter--;


    return damage;
  }
  else{
    return 0;
  }
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �V�~�����[�V�����p�Ăяo�����ǂ����𔻒�
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsSimulationMode( BTL_SVFLOW_WORK* wk )
{
  return wk->simulationCounter != 0;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���N���C�A���g���S�����Ă���퓬�ʒu�̐���Ԃ�
 *
 * @param   wk
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetClientCoverPosCount( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  SVCL_WORK* clwk;
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
  return clwk->numCoverPos;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���N���C�A���g�p�[�e�B���A�T���̊J�nIndex���擾
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetMyBenchIndex( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_ROTATION ){
    return BTL_SVFTOOL_GetClientCoverPosCount( wk, pokeID );
  }
  else{
    return BTL_ROTATION_FRONTPOS_NUM + BTL_ROTATION_BACKPOS_NUM;
  }
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���N���C�A���g�p�[�e�B�ɁA�i�퓬�\�ȁj�T���̃|�P���������邩
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsExistBenchPoke( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  const BTL_PARTY* party = BTL_SVFTOOL_GetPartyData( wk, pokeID );
  u8 startIdx = BTL_SVFTOOL_GetMyBenchIndex( wk, pokeID );
  if( BTL_PARTY_GetAliveMemberCountRear(party, startIdx) ){
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���Ƀ����o�[����ւ��A�N�V���������s���悤�Ƃ��Ă���|�P���������邩����
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_ReserveMemberChangeAction( BTL_SVFLOW_WORK* wk )
{
  if( wk->fMemberChangeReserve ){
    return FALSE;
  }
  wk->fMemberChangeReserve = TRUE;
  return TRUE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �Y���ʒu�ɂ��鐶�����Ă���|�P����ID��z��Ɋi�[������Ԃ�
 *
 * @param   wk
 * @param   exPos
 * @param   dst_pokeID    [out]�i�[��z��
 *
 * @retval  u8    �i�[������
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_ExpandPokeID( BTL_SVFLOW_WORK* wk, BtlExPos exPos, u8* dst_pokeID )
{
  u8 pos[ BTL_POS_MAX ];
  BTL_POKEPARAM* bpp;
  u8 max, num, i;

  max = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, pos );

  for(i=0, num=0; i<max; ++i)
  {
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, pos[i] );
    if( IsBppExist(bpp) ){
      dst_pokeID[num++] = BPP_GetID( bpp );
    }
  }
  return num;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�P�������܂ރp�[�e�B�f�[�^���擾
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  const BTL_PARTY*
 */
//--------------------------------------------------------------------------------------
const BTL_PARTY* BTL_SVFTOOL_GetPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );
  return BTL_POKECON_GetPartyDataConst( wk->pokeCon, clientID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�P�����̐퓬�ʒu��Ԃ�
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFTOOL_PokeIDtoPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��ʒu�̃|�P����ID��Ԃ�
 *
 * @param   wk
 * @param   pokePos
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_PokePosToPokeID( BTL_SVFLOW_WORK* wk, u8 pokePos )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, pokePos );
  return BPP_GetID( bpp );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��|�P�������u��т��ӂ�v�ȂǑ��̃��U���Ăяo�����ہA�^�[�Q�b�g�������Ō��߂�
 * ���^�[�Q�b�g�I���̕K�v���������U�̏ꍇ�ABTL_POS_NULL ���Ԃ�
 *
 * @param   wk
 * @param   pokeID
 * @param   waza
 *
 * @retval  BtlPokePos
 */
//--------------------------------------------------------------------------------------
BtlPokePos BTL_SVFTOOL_ReqWazaTargetAuto( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  BtlPokePos targetPos;

  targetPos = BTL_CALC_DecideWazaTargetAuto( wk->mainModule, wk->pokeCon, bpp, waza );
  return targetPos;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���^�[���ɑI�����ꂽ�A�N�V�������e���擾
 *
 * @param   wk
 * @param   pokeID
 * @param   dst       [out]
 *
 * @retval  BOOL    �������擾�ł�����TRUE�i���^�[���A�Q�����Ă��Ȃ��|�P�Ȃǂ��w�肳�ꂽ��FALSE�j
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_GetThisTurnAction( BTL_SVFLOW_WORK* wk, u8 pokeID, BTL_ACTION_PARAM* dst )
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
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �����̃A�N�V�������S�̂̉��Ԗڂ����擾
 *
 * @param   wk
 * @param   pokeID
 * @param   myOrder       [out] �����̏���
 * @param   totalAction   [out] �S�A�N�V������
 *
 * @retval  BOOL    �������擾�ł�����TRUE�i���^�[���A�Q�����Ă��Ȃ��|�P�Ȃǂ��w�肳�ꂽ��FALSE�j
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_GetMyActionOrder( BTL_SVFLOW_WORK* wk, u8 pokeID, u8* myOrder, u8* totalAction )
{
  u32 i;
  for(i=0; i<wk->numActOrder; ++i)
  {
    if( BPP_GetID(wk->actOrder[i].bpp) == pokeID ){
      *myOrder = i;
      *totalAction = wk->numActOrder;
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���O�ɏo���ꂽ���U���擾
 *
 * @param   wk
 *
 * @retval  WazaID
 */
//--------------------------------------------------------------------------------------
WazaID BTL_SVFTOOL_GetPrevExeWaza( BTL_SVFLOW_WORK* wk )
{
  return wk->prevExeWaza;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �S�n���h�����L�̃e���|�������[�N�擪�A�h���X��Ԃ�
 *
 * @param   wk
 *
 * @retval  void*
 */
//--------------------------------------------------------------------------------------
void* BTL_SVFTOOL_GetTmpWork( BTL_SVFLOW_WORK* wk, u32 size )
{
  GF_ASSERT(size < sizeof(wk->handlerTmpWork) );
  return wk->handlerTmpWork;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �o�����U���R�[�h�̃|�C���^���擾
 *
 * @param   wk
 *
 * @retval  const BTL_WAZAREC*
 */
//--------------------------------------------------------------------------------------
const BTL_WAZAREC* BTL_SVFTOOL_GetWazaRecord( BTL_SVFLOW_WORK* wk )
{
  return &wk->wazaRec;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] Que�̈�\��
 *
 * @param   wk
 *
 * @retval  const BTL_WAZAREC*
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_ReserveQuePos( BTL_SVFLOW_WORK* wk, ServerCmd cmd )
{
  return SCQUE_RESERVE_Pos( wk->que, cmd );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���S�|�P���R�[�h�̃|�C���^���擾
 *
 * @param   wk
 *
 * @retval  const BTL_DEADREC*
 */
//--------------------------------------------------------------------------------------
const BTL_DEADREC* BTL_SVFTOOL_GetDeadRecord( BTL_SVFLOW_WORK* wk )
{
  return &wk->deadRec;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] ���݂̌o�߃^�[�������擾
 *
 * @param   wk
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_GetTurnCount( BTL_SVFLOW_WORK* wk )
{
  return wk->turnCount;
}
//--------------------------------------------------------------------------------------
/**
 * �����o�[��փJ�E���^�̒l���擾
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetPokeChangeCount( BTL_SVFLOW_WORK* wk, u8 clientID )
{
  return wk->memberChangeCount[ clientID ];
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �|�P��������ɂ��邩
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsExistPokemon( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  return BTL_POSPOKE_IsExist( &wk->pospokeWork, pokeID );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �o�g�����[���擾
 *
 * @param   wk
 *
 * @retval  BtlRule
 */
//--------------------------------------------------------------------------------------
BtlRule BTL_SVFTOOL_GetRule( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetRule( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 *  [�n���h���p�c�[��] �ΐ�҃^�C�v���擾
 *
 * @param   wk
 *
 * @retval  BtlCompetitor
 */
//--------------------------------------------------------------------------------------
BtlCompetitor BTL_SVFTOOL_GetCompetitor( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetCompetitor( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �퓬�n�`�^�C�v���擾
 *
 * @param   wk
 *
 * @retval  BtlBgType
 */
//--------------------------------------------------------------------------------------
BtlBgAttr BTL_SVFTOOL_GetLandForm( BTL_SVFLOW_WORK* wk )
{
  const BTL_FIELD_SITUATION* sit = BTL_MAIN_GetFieldSituation( wk->mainModule );
  return sit->bgAttr;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �O�q�ʒu�̐���Ԃ�
 *
 * @param   wk
 *
 * @retval  u32
 */
//--------------------------------------------------------------------------------------
u32 BTL_SVFTOOL_GetFrontPosNum( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_GetFrontPosNum( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �}���`���[�h���ǂ����`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsMultiMode( BTL_SVFLOW_WORK* wk )
{
  return BTL_MAIN_IsMultiMode( wk->mainModule );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �i���O�|�P�������ǂ����`�F�b�N
 *
 * @param   wk
 * @param   pokeID
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_CheckSinkaMae( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  u16 monsno = BPP_GetMonsNo( bpp );
  u16 i;
  for(i=SHINKA_COND_START; i<SHINKA_COND_MAX; ++i){
    if( SHINKA_GetParamByHandle(wk->sinkaArcHandle, monsno, 0, i) ){
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �f�����v�Z
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fTrickRoomEnable )
{
  return scEvent_CalcAgility( wk, bpp, fTrickRoomEnable );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �f�����v�Z->��ɏo�Ă���|�P�������ł̃����N��Ԃ�
 *
 * @param   wk
 * @param   bpp_target
 *
 * @retval  u16 �����N�i0�`�j
 */
//--------------------------------------------------------------------------------------
u16 BTL_SVFTOOL_CalcAgilityRank( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp_target, BOOL fTrickRoomEnable )
{
  FRONT_POKE_SEEK_WORK fps;
  BTL_POKEPARAM* bpp;
  u16 rank, agi, target_agi;
  u8 target_pokeID;

  target_agi = BTL_SVFTOOL_CalcAgility( wk, bpp_target, fTrickRoomEnable );
  target_pokeID = BPP_GetID( bpp_target );
  rank = 0;

  FRONT_POKE_SEEK_InitWork( &fps, wk );
  while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
  {
    if( BPP_GetID(bpp) == target_pokeID ){
      continue;
    }
    agi = BTL_SVFTOOL_CalcAgility( wk, bpp, fTrickRoomEnable );
    if( agi > target_agi ){
      ++rank;
    }
  }

  return rank;
}

//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �g�����Ă���h��ԃ`�F�b�N
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsFlyingPoke( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
  return scproc_CheckFloating( wk, bpp );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �V��擾
 *
 * @param   wk
 *
 * @retval  BtlWeather
 */
//--------------------------------------------------------------------------------------
BtlWeather BTL_SVFTOOL_GetWeather( BTL_SVFLOW_WORK* wk )
{
  return scEvent_GetWeather( wk );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �f�o�b�O�t���O�擾
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  u16
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_GetDebugFlag( BTL_SVFLOW_WORK* wk, BtlDebugFlag flag )
{
  return BTL_MAIN_GetDebugFlag( wk->mainModule, flag );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��T�C�h�G�t�F�N�g�������Ă��邩����
 *
 * @param   wk
 * @param   BtlPokePos      pos
 * @param   BtlSideEffect
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsExistSideEffect( BTL_SVFLOW_WORK* wk, BtlSide side, BtlSideEffect sideEffect )
{
  return BTL_HANDER_SIDE_IsExist( side, sideEffect );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��T�C�h�G�t�F�N�g�̃J�E���^���擾
 *
 * @param   wk
 * @param   BtlPokePos      pos
 * @param   BtlSideEffect
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
u32 BTL_SVFTOOL_GetSideEffectCount( BTL_SVFLOW_WORK* wk, BtlPokePos pos, BtlSideEffect sideEffect )
{
  BtlSide side = BTL_MAINUTIL_PosToSide( pos );
  return BTL_HANDLER_SIDE_GetAddCount( side, sideEffect );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��T�C�h�G�t�F�N�g�̃J�E���^���擾
 *
 * @param   wk
 * @param   pos
 * @param   effect
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsExistPosEffect( BTL_SVFLOW_WORK* wk, BtlPokePos pos, BtlPosEffect effect )
{
  return BTL_HANDLER_POS_IsRegistered( effect, pos );
}
//--------------------------------------------------------------------------------------
/**
 * [�n���h���p�c�[��] �w��N���C�A���g�̖����N���C�A���gID��Ԃ��i���������Ȃ����� BTL_CLIENTID_NULL�j
 *
 * @param   wk
 * @param   clientID
 *
 * @retval  u8
 */
//--------------------------------------------------------------------------------------
u8 BTL_SVFTOOL_GetFriendClientID( BTL_SVFLOW_WORK* wk, u8 clientID )
{
  return BTL_MAIN_GetFriendCleintID( wk->mainModule, clientID );
}
//--------------------------------------------------------------------------------------
/**
 * �����o�[����ւ����̊��荞�݃A�N�V�����������ł��邩�`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SVFTOOL_IsMemberOutIntr( BTL_SVFLOW_WORK* wk )
{
  return wk->fMemberOutIntr;
}
//--------------------------------------------------------------------------------------
/**
 * �����o�[����ւ����̊��荞�݃��N�G�X�g
 *
 * @param   wk
 * @param   pokeID    ���荞�݂����|�PID
 */
//--------------------------------------------------------------------------------------
void BTL_SVFTOOL_AddMemberOutIntr( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  if( wk->MemberOutIntrPokeCount < NELEMS(wk->MemberOutIntrPokeID) )
  {
    wk->MemberOutIntrPokeID[ wk->MemberOutIntrPokeCount++ ] = pokeID;
  }
}

//=============================================================================================
/**
 * [�n���h������̑���Ăяo��]
 *
 * @param   wk
 * @param   effIndex
 */
//=============================================================================================
void BTL_SVFRET_SetWazaEffectIndex( BTL_SVFLOW_WORK* wk, u8 effIndex )
{
  wazaEffCtrl_SetEffectIndex( wk->wazaEffCtrl, effIndex );
}
//=============================================================================================
/**
 * [�n���h������̃V�X�e���Ăяo��]
 *  �X�^���h�A���[���o�g���ŁA�v���C���[�|�P�����̎g�p�����n���h������Ăяo���ꂽ�ꍇ�̂�
 *  �����Â�������悹����i�l�R�ɂ��΂��p�j
 *
 * @param   wk
 * @param   volume    ��悹�z
 * @param   pokeID    ���U�g�p�|�P����ID
 *
 * @retval  BOOL      �����𖞂������i��悹�����j�ꍇ��TRUE���Ԃ�
 */
//=============================================================================================
BOOL BTL_SVFRET_AddBonusMoney( BTL_SVFLOW_WORK* wk, u32 volume, u8 pokeID )
{
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor( wk->mainModule );

  if( (competitor == BTL_COMPETITOR_WILD)
  ||  (competitor == BTL_COMPETITOR_TRAINER)
  ){
    BTL_SERVER_AddBonusMoney( wk->server, volume );
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * [�n���h������̃V�X�e���Ăяo��]
 * �X�^���h�A�����o�g���ŁA�I�����ɂ��炦�邨����{�ɂ���i���ʂ͂P�x�����j
 *
 * @param   wk
 * @param   pokeID
 */
//=============================================================================================
void BTL_SVFRET_SetMoneyDblUp( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor( wk->mainModule );
  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

  if( ((competitor == BTL_COMPETITOR_WILD)||(competitor == BTL_COMPETITOR_TRAINER))
  &&  (clientID == BTL_MAIN_GetPlayerClientID(wk->mainModule))
  ){
    BTL_SERVER_SetMoneyDblUp( wk->server );
  }
}
//=============================================================================================
/**
 * [�n���h������̑���Ăяo��]  �}�W�b�N�R�[�g���˕Ԃ��o�^
 *
 * @param   wk
 * @param   pokeID          �}�W�b�N�R�[�g���g�������̃|�P����ID
 * @param   targetPokeID    ���˕Ԃ���̃|�P����ID
 */
//=============================================================================================
void BTL_SVFRET_AddMagicCoatAction( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 targetPokeID )
{
  BtlPokePos targetPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, targetPokeID );
  wazaRobParam_Add( wk->magicCoatParam, pokeID, targetPokeID, targetPos );
}
//=============================================================================================
/**
 * [�n���h������̑���Ăяo��]  �t���[�t�H�[�����߃^�[������
 *
 * @param   wk
 * @param   atkPokeID
 * @param   targetPokeID
 *
 * @retval  BOOL    ���������TRUE
 */
//=============================================================================================
BOOL BTL_SVFRET_FreeFallStart( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 targetPokeID, BOOL* fFailMsgDisped )
{
  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, atkPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, targetPokeID );

  return scproc_FreeFall_Start( wk, attacker, target, fFailMsgDisped );
}
//=============================================================================================
/**
 * [�n���h������̑���Ăяo��]  �t���[�t�H�[������^�[������
 *
 * @param   wk
 * @param   atkPokeID
 * @param   targetPokeID
 *
 * @retval  BOOL    ���������TRUE
 */
//=============================================================================================
void BTL_SVFRET_FreeFallRelease( BTL_SVFLOW_WORK* wk, u8 atkPokeID )
{
  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, atkPokeID );

  scproc_FreeFall_CheckRelease( wk, attacker, TRUE );
}
//=============================================================================================
/**
 * [btl_server_flow_sub���瑀��Ăяo��]  �����A�C�e�����ʏ���
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_SVFRET_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
  return scproc_UseItemEquip( wk, bpp );
}
//=============================================================================================
/**
 * [btl_server_flow_sub���瑀��Ăяo��]  �����o�[����C�x���g�Ăяo��
 *
 * @param   wk
 * @param   bpp
 *
 * @retval  BOOL
 */
//=============================================================================================
void BTL_SVFRET_Event_AfterMemberIn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp )
{
  scEvent_AfterMemberIn( wk, bpp );
}

//=============================================================================================
/**
 * �퓬���ʔ��菈��
 *
 * @param   wk
 *
 * @retval  BtlResult
 */
//=============================================================================================
BtlResult BTL_SVFLOW_ChecBattleResult( BTL_SVFLOW_WORK* wk )
{
  return BTL_SVFSUB_CheckBattleResult( wk );
}


//---------------------------------------------------------------------------------------------
// ���^�[�������Ԃ�|�P���R�[�h
//---------------------------------------------------------------------------------------------

void BTL_SVFRET_AddRelivePokeRecord( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  relivePokeRec_Add( wk, pokeID );
}

// ������
static void relivePokeRec_Init( BTL_SVFLOW_WORK* wk )
{
  wk->numRelivePoke = 0;
}

// �����Ԃ�L�^�ǉ�
static void relivePokeRec_Add( BTL_SVFLOW_WORK* wk, u8 pokeID )
{
  u32 i;

  for(i=0; i<wk->numRelivePoke; ++i)
  {
    if( wk->relivedPokeID[i] == pokeID ){
      return;
    }
  }

  if( i < NELEMS(wk->relivedPokeID) )
  {
    wk->relivedPokeID[i] = pokeID;
    wk->numRelivePoke++;
  }
}

// �^�[���I�����A�|�P��������̕K�v�����邩����
static BOOL relivePokeRec_CheckNecessaryPokeIn( BTL_SVFLOW_WORK* wk )
{
  u32 i;
  u8 pos[ BTL_POSIDX_MAX ];
  u8 clientID;

  for(i=0; i<wk->numRelivePoke; ++i)
  {
    clientID = BTL_MAINUTIL_PokeIDtoClientID( wk->relivedPokeID[i] );

    BTL_N_Printf( DBGSTR_SVFL_RelivePokeStart, clientID);

    // �P���ł��󂫈ʒu������Γ��ꂳ����K�v�A��
    if( BTL_POSPOKE_GetClientEmptyPos(&wk->pospokeWork, clientID, pos) ){
      BTL_N_Printf( DBGSTR_SVFL_ReliveEndIn );
      return TRUE;
    }
    BTL_N_Printf( DBGSTR_SVFL_ReliveEndIgnore );
  }
  return FALSE;
}

/*--------------------------------------------------------------------------------------------*/
/* POKESET Stack : Action����Push,Pop���Ċe�p�r��PokeSet���ʂɗp�ӁA���p���邽�߂̎d�g��    */
/*--------------------------------------------------------------------------------------------*/
// ������
static void PSetStack_Init( BTL_SVFLOW_WORK* wk )
{
  wk->pokesetStackPtr = 0;
  psetstack_setup( wk, 0, TRUE );
}
// Push
static void PSetStack_Push( BTL_SVFLOW_WORK* wk )
{
  if( wk->pokesetStackPtr < (NELEMS(wk->pokesetUnit)-1) )
  {
    wk->pokesetStackPtr++;
    psetstack_setup( wk, wk->pokesetStackPtr, TRUE );
  }
  else{
    GF_ASSERT(0);
  }
}
// Pop
static void PSetStack_Pop( BTL_SVFLOW_WORK* wk )
{
  if( wk->pokesetStackPtr )
  {
    wk->pokesetStackPtr--;
    psetstack_setup( wk, wk->pokesetStackPtr, FALSE );
  }
  else
  {
    GF_ASSERT(0);
  }
}
// �������J�����g�v�f�N���A
static void psetstack_setup( BTL_SVFLOW_WORK* wk, u32 sp, BOOL fClear )
{
  POKESET_STACK_UNIT* unit = &(wk->pokesetUnit[ sp ]);

  wk->psetTargetOrg  = &unit->TargetOrg;
  wk->psetTarget     = &unit->Target;
  wk->psetFriend     = &unit->Friend;
  wk->psetEnemy      = &unit->Enemy;
  wk->psetDamaged    = &unit->Damaged;
  wk->psetRobTarget  = &unit->RobTarget;
  wk->psetTmp        = &unit->psetTmp;
  wk->wazaEffCtrl    = &unit->effCtrl;
  wk->wazaParam      = &unit->wazaParam;
  wk->wazaParamOrg   = &unit->wazaParamOrg;
  wk->hitCheckParam  = &unit->hitCheck;
  wk->wazaRobParam   = &unit->wazaRobParam;
  wk->magicCoatParam = &unit->magicCoatParam;
  wk->calcDmgFriend  = &unit->calcDmgFriend;
  wk->calcDmgEnemy   = &unit->calcDmgEnemy;
  wk->defaultTargetPos = unit->defaultTargetPos;
  wk->fMemberChangeReserve = unit->fMemberChangeReserve;

  if( fClear ){
    BTL_POKESET_Clear( wk->psetTargetOrg );
    BTL_POKESET_Clear( wk->psetTarget );
    BTL_POKESET_Clear( wk->psetFriend );
    BTL_POKESET_Clear( wk->psetEnemy );
    BTL_POKESET_Clear( wk->psetDamaged );
    BTL_POKESET_Clear( wk->psetRobTarget );
    BTL_POKESET_Clear( wk->psetTmp );
    wazaEffCtrl_Init( wk->wazaEffCtrl );
    GFL_STD_MemClear( wk->wazaParam, sizeof(*(wk->wazaParam)) );
    GFL_STD_MemClear( wk->wazaParamOrg, sizeof(*(wk->wazaParamOrg)) );
    GFL_STD_MemClear( wk->hitCheckParam, sizeof(*(wk->hitCheckParam)) );
    GFL_STD_MemClear( wk->wazaRobParam, sizeof(*(wk->wazaRobParam)) );
    GFL_STD_MemClear( wk->magicCoatParam, sizeof(*(wk->magicCoatParam)) );
    GFL_STD_MemClear( wk->calcDmgFriend, sizeof(*(wk->calcDmgFriend)) );
    GFL_STD_MemClear( wk->calcDmgEnemy, sizeof(*(wk->calcDmgEnemy)) );

    wk->defaultTargetPos = BTL_POS_NULL;
    wk->fMemberChangeReserve = 0;
  }
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

//=============================================================================================
/**
 * HandEx ���[�N�P���擾
 *
 * @param   wk
 * @param   eq_type
 * @param   userPokeID
 *
 * @retval  void*
 */
//=============================================================================================
void* BTL_SVF_HANDEX_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID )
{
  return BTL_Hem_PushWork( &wk->HEManager, eq_type, userPokeID );
}
void BTL_SVF_HANDEX_PushRun( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID )
{
  void* work = BTL_SVF_HANDEX_Push( wk, eq_type, userPokeID );
  BTL_SVF_HANDEX_Pop( wk, work );
}

//=============================================================================================
/**
 * HandEx ���[�N�P�����s
 *
 * @param   wk
 * @param   eq_type
 * @param   userPokeID
 *
 * @retval  void
 */
//=============================================================================================
void BTL_SVF_HANDEX_Pop( BTL_SVFLOW_WORK* wk, void* handExWork )
{
  HandEx_Exe( wk, (BTL_HANDEX_PARAM_HEADER*)handExWork );
  BTL_Hem_PopWork( &wk->HEManager, handExWork );
}


//----------------------------------------------------------------------------------
/**
 * �C�x���g���s���ʂ��擾
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static HandExResult scproc_HandEx_Result( BTL_SVFLOW_WORK* wk )
{
  if( BTL_Hem_IsUsed(&wk->HEManager) )
  {
    BOOL fSucceed = BTL_Hem_GetTotalResult( &wk->HEManager );
    return (fSucceed)? HandExResult_Enable : HandExResult_Fail;
  }

  return HandExResult_NULL;
}
HandExResult BTL_SVF_HandEx_Result( BTL_SVFLOW_WORK* wk )
{
  return scproc_HandEx_Result( wk );
}

static void HandEx_Exe( BTL_SVFLOW_WORK* wk, BTL_HANDEX_PARAM_HEADER* handEx_header )
{
  u16 useItemID = BTL_Hem_GetUseItemNo( &wk->HEManager );
  u8 fPrevSucceed;

  if( BTL_Hem_IsUsed(&wk->HEManager) ){
    fPrevSucceed = BTL_Hem_GetPrevResult(&wk->HEManager);
  }else{
    fPrevSucceed = TRUE;
  }

  if( handEx_header->failSkipFlag && (fPrevSucceed == FALSE) ){
    BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_SVFL_HandExContFail );
    return;
  }
  if( handEx_header->autoRemoveFlag ){
    const BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParamConst( wk->pokeCon, handEx_header->userPokeID );
    if( BPP_IsDead(bpp) ){
      BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_SVFL_HandExContDead );
      return;
    }
  }

  switch( handEx_header->equip ){
  case BTL_HANDEX_USE_ITEM_EFFECT:    fPrevSucceed = scproc_HandEx_UseItemAct( wk, handEx_header ); break;
  case BTL_HANDEX_TOKWIN_IN:          fPrevSucceed = scproc_HandEx_TokWinIn( wk, handEx_header ); break;
  case BTL_HANDEX_TOKWIN_OUT:         fPrevSucceed = scproc_HandEx_TokWinOut( wk, handEx_header ); break;
  case BTL_HANDEX_ITEM_EFFECT:        fPrevSucceed = scproc_HandEx_ItemEffect( wk, handEx_header ); break;
  case BTL_HANDEX_USE_ITEM:           fPrevSucceed = scproc_HandEx_useItem( wk, handEx_header ); break;
  case BTL_HANDEX_RECOVER_HP:         fPrevSucceed = scproc_HandEx_recoverHP( wk, handEx_header, useItemID ); break;
  case BTL_HANDEX_DRAIN:              fPrevSucceed = scproc_HandEx_drain( wk, handEx_header, useItemID ); break;
  case BTL_HANDEX_DAMAGE:             fPrevSucceed = scproc_HandEx_damage( wk, handEx_header ); break;
  case BTL_HANDEX_SHIFT_HP:           fPrevSucceed = scproc_HandEx_shiftHP( wk, handEx_header ); break;
  case BTL_HANDEX_RECOVER_PP:         fPrevSucceed = scproc_HandEx_recoverPP( wk, handEx_header, useItemID ); break;
  case BTL_HANDEX_DECREMENT_PP:       fPrevSucceed = scproc_HandEx_decrementPP( wk, handEx_header, useItemID ); break;
  case BTL_HANDEX_CURE_SICK:          fPrevSucceed = scproc_HandEx_cureSick( wk, handEx_header, useItemID ); break;
  case BTL_HANDEX_ADD_SICK:           fPrevSucceed = scproc_HandEx_addSick( wk, handEx_header ); break;
  case BTL_HANDEX_RANK_EFFECT:        fPrevSucceed = scproc_HandEx_rankEffect( wk, handEx_header, useItemID ); break;
  case BTL_HANDEX_SET_RANK:           fPrevSucceed = scproc_HandEx_setRank( wk, handEx_header ); break;
  case BTL_HANDEX_RECOVER_RANK:       fPrevSucceed = scproc_HandEx_recoverRank( wk, handEx_header ); break;
  case BTL_HANDEX_RESET_RANK:         fPrevSucceed = scproc_HandEx_resetRank( wk, handEx_header ); break;
  case BTL_HANDEX_SET_STATUS:         fPrevSucceed = scproc_HandEx_setStatus( wk, handEx_header ); break;
  case BTL_HANDEX_KILL:               fPrevSucceed = scproc_HandEx_kill( wk, handEx_header ); break;
  case BTL_HANDEX_CHANGE_TYPE:        fPrevSucceed = scproc_HandEx_changeType( wk, handEx_header ); break;
  case BTL_HANDEX_MESSAGE:            fPrevSucceed = scproc_HandEx_message( wk, handEx_header ); break;
  case BTL_HANDEX_SET_TURNFLAG:       fPrevSucceed = scproc_HandEx_setTurnFlag( wk, handEx_header ); break;
  case BTL_HANDEX_RESET_TURNFLAG:     fPrevSucceed = scproc_HandEx_resetTurnFlag( wk, handEx_header ); break;
  case BTL_HANDEX_SET_CONTFLAG:       fPrevSucceed = scproc_HandEx_setContFlag( wk, handEx_header ); break;
  case BTL_HANDEX_RESET_CONTFLAG:     fPrevSucceed = scproc_HandEx_resetContFlag( wk, handEx_header ); break;
  case BTL_HANDEX_SIDEEFF_ADD:        fPrevSucceed = scproc_HandEx_sideEffectAdd( wk, handEx_header ); break;
  case BTL_HANDEX_SIDEEFF_REMOVE:     fPrevSucceed = scproc_HandEx_sideEffectRemove( wk, handEx_header ); break;
  case BTL_HANDEX_ADD_FLDEFF:         fPrevSucceed = scproc_HandEx_addFieldEffect( wk, handEx_header ); break;
  case BTL_HANDEX_CHANGE_WEATHER:     fPrevSucceed = scproc_HandEx_changeWeather( wk, handEx_header ); break;
  case BTL_HANDEX_REMOVE_FLDEFF:      fPrevSucceed = scproc_HandEx_removeFieldEffect( wk, handEx_header ); break;
  case BTL_HANDEX_POSEFF_ADD:         fPrevSucceed = scproc_HandEx_PosEffAdd( wk, handEx_header ); break;
  case BTL_HANDEX_CHANGE_TOKUSEI:     fPrevSucceed = scproc_HandEx_tokuseiChange( wk, handEx_header ); break;
  case BTL_HANDEX_SET_ITEM:           fPrevSucceed = scproc_HandEx_setItem( wk, handEx_header ); break;
  case BTL_HANDEX_EQUIP_ITEM:         fPrevSucceed = scproc_HandEx_EquipItem( wk, handEx_header ); break;
  case BTL_HANDEX_ITEM_SP:            fPrevSucceed = scproc_HandEx_ItemSP( wk, handEx_header ); break;
  case BTL_HANDEX_CONSUME_ITEM:       fPrevSucceed = scproc_HandEx_consumeItem( wk, handEx_header ); break;
  case BTL_HANDEX_SWAP_ITEM:          fPrevSucceed = scproc_HandEx_swapItem( wk, handEx_header ); break;
  case BTL_HANDEX_UPDATE_WAZA:        fPrevSucceed = scproc_HandEx_updateWaza( wk, handEx_header ); break;
  case BTL_HANDEX_COUNTER:            fPrevSucceed = scproc_HandEx_counter( wk, handEx_header ); break;
  case BTL_HANDEX_DELAY_WAZADMG:      fPrevSucceed = scproc_HandEx_delayWazaDamage( wk, handEx_header ); break;
  case BTL_HANDEX_QUIT_BATTLE:        fPrevSucceed = scproc_HandEx_quitBattle( wk, handEx_header ); break;
  case BTL_HANDEX_CHANGE_MEMBER:      fPrevSucceed = scproc_HandEx_changeMember( wk, handEx_header ); break;
  case BTL_HANDEX_BATONTOUCH:         fPrevSucceed = scproc_HandEx_batonTouch( wk, handEx_header ); break;
  case BTL_HANDEX_ADD_SHRINK:         fPrevSucceed = scproc_HandEx_addShrink( wk, handEx_header ); break;
  case BTL_HANDEX_RELIVE:             fPrevSucceed = scproc_HandEx_relive( wk, handEx_header ); break;
  case BTL_HANDEX_SET_WEIGHT:         fPrevSucceed = scproc_HandEx_setWeight( wk, handEx_header ); break;
  case BTL_HANDEX_PUSHOUT:            fPrevSucceed = scproc_HandEx_pushOut( wk, handEx_header ); break;
  case BTL_HANDEX_INTR_POKE:          fPrevSucceed = scproc_HandEx_intrPoke( wk, handEx_header ); break;
  case BTL_HANDEX_INTR_WAZA:          fPrevSucceed = scproc_HandEx_intrWaza( wk, handEx_header ); break;
  case BTL_HANDEX_SEND_LAST:          fPrevSucceed = scproc_HandEx_sendLast( wk, handEx_header ); break;
  case BTL_HANDEX_SWAP_POKE:          fPrevSucceed = scproc_HandEx_swapPoke( wk, handEx_header ); break;
  case BTL_HANDEX_HENSIN:             fPrevSucceed = scproc_HandEx_hensin( wk, handEx_header ); break;
  case BTL_HANDEX_FAKE_BREAK:         fPrevSucceed = scproc_HandEx_fakeBreak( wk, handEx_header ); break;
  case BTL_HANDEX_JURYOKU_CHECK:      fPrevSucceed = scproc_HandEx_juryokuCheck( wk, handEx_header ); break;
  case BTL_HANDEX_TAMEHIDE_CANCEL:    fPrevSucceed = scproc_HandEx_TameHideCancel( wk, handEx_header ); break;
  case BTL_HANDEX_ADD_EFFECT:         fPrevSucceed = scproc_HandEx_effectByPos( wk, handEx_header ); break;
  case BTL_HANDEX_CHANGE_FORM:        fPrevSucceed = scproc_HandEx_changeForm( wk, handEx_header ); break;
  case BTL_HANDEX_SET_EFFECT_IDX:     fPrevSucceed = scproc_HandEx_setWazaEffectIndex( wk, handEx_header ); break;
  case BTL_HANDEX_WAZAEFFECT_ENABLE:  fPrevSucceed = scproc_HandEx_setWazaEffectEnable( wk, handEx_header ); break;
  default:
    GF_ASSERT_MSG(0, "illegal handEx type = %d, userPokeID=%d", handEx_header->equip, handEx_header->userPokeID);
  }

  BTL_N_PrintfEx( PRINT_CHANNEL_EVENTSYS, DBGSTR_SVFL_HandExSetResult, fPrevSucceed );
  BTL_Hem_SetResult( &wk->HEManager, fPrevSucceed );
}

/**
 * �A�C�e���g�p�G�t�F�N�g�̂ݔ���
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_UseItemAct( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param_header->userPokeID) )
  {
    BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
    scPut_UseItemAct( wk, pp_user );
    return 1;
  }
  return 0;

}
/**
 * �Ƃ������E�B���h�E�\��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_TokWinIn( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param_header->userPokeID) )
  {
    BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
    scPut_TokWin_In( wk, pp_user );
    return 1;
  }
  return 0;
}
/**
 * �Ƃ������E�B���h�E����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_TokWinOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
//  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param_header->userPokeID) )
  {
    BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
    scPut_TokWin_Out( wk, pp_user );
    return 1;
  }
  return 0;
}
/**
 * �����A�C�e���g�p�G�t�F�N�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_ItemEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BtlPokePos  pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param_header->userPokeID );
  if( pos != BTL_POS_NULL )
  {
    SCQUE_PUT_ACT_EffectByPos( wk->que, pos, BTLEFF_USE_ITEM );
    return 1;
  }
  return 0;
}
/**
 * �|�P����HP��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_recoverHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RECOVER_HP* param = (const BTL_HANDEX_PARAM_RECOVER_HP*)param_header;

  u8 result = 0;
/// ����g�p�ɂ����s������̂ŁA��ɂ��Ȃ��Ă��L��
//  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
    BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

    if( !scproc_RecoverHP_CheckFailBase(wk, pp_target) )
    {
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( wk, pp_user );
      }

      if( param->fFailCheckThru == FALSE )
      {
        result = !scproc_RecoverHP_CheckFailSP( wk, pp_target, TRUE );
      }
      else{
        result = TRUE;
      }

      if( result )
      {
        scproc_RecoverHP_Core( wk, pp_target, param->recoverHP );

        if( param->exStr.type != BTL_STRTYPE_NULL ){
            handexSub_putString( wk, &(param->exStr) );
        }else if( itemID != ITEM_DUMMY_DATA ){
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_UseItem_RecoverHP, param->pokeID, itemID );
        }
      }

      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( wk, pp_user );
      }
    }
  }
  return result;
}
/**
 * �|�P����HP�񕜁i�h���C���n�j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_drain( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_DRAIN* param = (BTL_HANDEX_PARAM_DRAIN*)param_header;
  BTL_POKEPARAM* pp_damaged = NULL;
  if( param->damagedPokeID != BTL_POKEID_NULL ){
    pp_damaged = BTL_POKECON_GetPokeParam( wk->pokeCon, param->damagedPokeID );
  }

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->recoverPokeID) )
  {
    BTL_POKEPARAM* pp_recover = BTL_POKECON_GetPokeParam( wk->pokeCon, param->recoverPokeID );

    if( !BPP_IsDead(pp_recover) )
    {
      if( scproc_DrainCore(wk, pp_recover, pp_damaged, param->recoverHP) ){
        if( param->exStr.type != BTL_STRTYPE_NULL ){
          handexSub_putString( wk, &(param->exStr) );
        }
        return 1;
      }
    }
  }
  return 0;
}
/**
 * �|�P�����Ƀ_���[�W
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_damage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_DAMAGE* param = (BTL_HANDEX_PARAM_DAMAGE*)param_header;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

    if( !BPP_IsDead(pp_target) )
    {
      if( (!(param->fAvoidHidePoke))
      ||  (CheckPokeHideState(pp_target) == BPP_CONTFLG_NULL)
      ){
        if( scproc_SimpleDamage_CheckEnable(wk, pp_target, param->damage) )
        {
          if( param->fExEffect ){
            scproc_ViewEffect( wk, param->effectNo, param->pos_from, param->pos_to, FALSE, 0 );
          }
          scproc_SimpleDamage_Core(wk, pp_target, param->damage, &param->exStr );
          return 1;
        }
      }
    }
  }
  return 0;
}
/**
 * �|�P����HP�����i�񕜁^�_���[�W�ƌ��Ȃ��Ȃ��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_shiftHP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SHIFT_HP* param = (BTL_HANDEX_PARAM_SHIFT_HP*)param_header;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i)
  {
    if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID[i]) )
    {
      BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
      if( !BPP_IsDead(pp_target) ){
        scPut_SimpleHp( wk, pp_target, param->volume[i], !param->fEffectDisable );
        scproc_CheckItemReaction( wk, pp_target, BTL_ITEMREACTION_HP );
        result = 1;
      }
    }
  }
  return result;
}
/**
 * �|�P����PP��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_recoverPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

/// ����g�p�ɂ����s������̂ŁA��ɂ��Ȃ��Ă��L��
//  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

    if( BPP_IsFightEnable(pp_target) || (param->fDeadPokeEnable) )
    {
      BOOL fOrgWaza = (!(param->fSurfacePP));
      if( !BPP_WAZA_IsPPFull(pp_target, param->wazaIdx, fOrgWaza) )
      {
        scPut_RecoverPP( wk, pp_target, param->wazaIdx, param->volume, fOrgWaza );
        handexSub_putString( wk, &param->exStr );
        return 1;
      }
    }
  }
  return 0;
}
/**
 * �|�P����PP����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_decrementPP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_PP* param = (BTL_HANDEX_PARAM_PP*)param_header;

// �唚��->����˂� ������̂ŁA��ɂ��Ȃ��Ă��L��
//  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
    if( !BPP_IsDead(target) )
    {
      if( scproc_decrementPP(wk, target, param->wazaIdx, param->volume) )
      {
        handexSub_putString( wk, &param->exStr );
        if( scEvent_DecrementPP_Reaction(wk, target, param->wazaIdx ) )
        {
          scproc_UseItemEquip( wk, target );
        }
      }
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P������Ԉُ��
 * @return ������ 1 / ���s�� 0
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
    u32 i;
    for(i=0; i<param->poke_cnt; ++i)
    {
      // ��ɂ���|�P�����Ɍ��肵�Ă��܂��Ɓu�A���}�Z���s�[�v�Ȃǂ������Ȃ�
//      if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID[i]) )
      {
        BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
        if( BPP_IsFightEnable(pp_target) )
        {
          BtlWazaSickEx exCode;
          BPP_SICK_CONT oldCont;
          WazaSick check_sick;

          exCode = param->sickCode;
          while( (check_sick = trans_sick_code(pp_target, &exCode)) != WAZASICK_NULL )
          {
            scPut_CureSick( wk, pp_target, check_sick, &oldCont );
            if( !param->fStdMsgDisable )
            {
              if( BTL_SICK_MakeDefaultCureMsg(check_sick, oldCont, pp_target, itemID, &wk->strParam) ){
                handexSub_putString( wk, &wk->strParam );
              }
            }
            else
            {
              handexSub_putString( wk, &param->exStr );
            }
            if( check_sick == WAZASICK_SASIOSAE ){
              scproc_CheckItemReaction( wk, pp_target, BTL_ITEMREACTION_GEN );
            }
            result = 1;
          }
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
 * �|�P������Ԉُ퉻
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_addSick( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_SICK* param = (BTL_HANDEX_PARAM_ADD_SICK*)param_header;
  BTL_POKEPARAM* pp_user = (param_header->userPokeID != BTL_POKEID_NULL)?
                      BTL_POKECON_GetPokeParam(wk->pokeCon, param_header->userPokeID) : NULL;

  BTL_POKEPARAM* target;
  u8 fDefaultMsg = ( (HANDEX_STR_IsEnable(&param->exStr) == FALSE) && (param->fStdMsgDisable == FALSE) );

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
    if( !BPP_IsDead(target) )
    {
      BTL_N_Printf( DBGSTR_SVFL_HandEx_AddSick, param->pokeID, param->sickID, param->fAlmost );

      if( !scproc_AddSickCheckFail(wk, target, pp_user, param->sickID, param->sickCont,
        param->overWriteMode, param->fAlmost) )
      {
        if( param->header.tokwin_flag ){
          TAYA_Printf("�Ƃ������E�B���h�E�I�� .. %p\n", param);
          scPut_TokWin_In( wk, pp_user );
        }

        scproc_AddSickCore( wk, target, pp_user, param->sickID, param->sickCont, fDefaultMsg, &param->exStr );

        TAYA_Printf("�Ƃ������E�B���h�E = %d .. %p\n", param->header.tokwin_flag, param);


        if( param->header.tokwin_flag ){
          scPut_TokWin_Out( wk, pp_user );
        }

        return 1;
      }
    }
  }

  return 0;
}
/**
 * �|�P���������N���ʁi�i�K�����w�肵�A�㏸�܂��͉��~������B����̂Ƃ��������ɂ�莸�s���邱�Ƃ����蓾��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_rankEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header, u16 itemID )
{
  const BTL_HANDEX_PARAM_RANK_EFFECT* param = (BTL_HANDEX_PARAM_RANK_EFFECT*)param_header;
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BTL_POKEPARAM* pp_target;
  u8 fEffective = FALSE;
  u8 result = 0;
  u32 i;


  for(i=0; i<param->poke_cnt; ++i)
  {
    if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID[i]) )
    {
      pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
      if( !BPP_IsDead( pp_target )
      &&  BPP_IsRankEffectValid( pp_target, param->rankType, param->rankVolume )
      ){
        fEffective = TRUE;
        break;
      }
    }
  }
  if( fEffective && param_header->tokwin_flag ){
    scPut_TokWin_In( wk, pp_user );
  }

  for(i=0; i<param->poke_cnt; ++i)
  {
    if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID[i]) )
    {
      pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
      if( !BPP_IsDead(pp_target) )
      {
        if( scproc_RankEffectCore(wk, param_header->userPokeID, pp_target, param->rankType, param->rankVolume,
          BTL_POKEID_NULL, itemID, param->fAlmost, !(param->fStdMsgDisable))
        ){
          handexSub_putString( wk, &param->exStr );
          result = 1;
        }
      }
    }
  }

  if( fEffective && param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, pp_user );
  }

  return result;
}
/**
 * �|�P���������N�����Z�b�g�i�w��l�ɋ����I�ɏ���������B���s���Ȃ��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_RANK* param = (BTL_HANDEX_PARAM_SET_RANK*)param_header;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
    if( !BPP_IsDead(pp_target) )
    {
      BPP_RankSet( pp_target, BPP_ATTACK_RANK,     param->attack );
      BPP_RankSet( pp_target, BPP_DEFENCE_RANK,    param->defence );
      BPP_RankSet( pp_target, BPP_SP_ATTACK_RANK,  param->sp_attack );
      BPP_RankSet( pp_target, BPP_SP_DEFENCE_RANK, param->sp_defence );
      BPP_RankSet( pp_target, BPP_AGILITY_RANK,    param->agility );
      BPP_RankSet( pp_target, BPP_HIT_RATIO,       param->hit_ratio );
      BPP_RankSet( pp_target, BPP_AVOID_RATIO,     param->avoid_ratio );

      SCQUE_PUT_OP_RankSet7( wk->que,
        param->pokeID, param->attack, param->defence, param->sp_attack, param->sp_defence, param->agility,
                       param->hit_ratio, param->avoid_ratio );
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P���������N�̓��A�}�C�i�X�ɂȂ��Ă�����̂̂݃t���b�g�ɖ߂�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_recoverRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RECOVER_RANK* param = (BTL_HANDEX_PARAM_RECOVER_RANK*)param_header;
  BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(pp_target) )
  {
    u8 result;
    SCQUE_PUT_OP_RankRecover( wk->que, param->pokeID );
    result = BPP_RankRecover( pp_target );
    return result;
  }
  return 0;
}
/**
 * �|�P���������N��S�ăt���b�g�ɖ߂�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_resetRank( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RESET_RANK* param = (BTL_HANDEX_PARAM_RESET_RANK*)param_header;
  BTL_POKEPARAM* pp_target;
  u32 i;
  u8 result = 0;

  for(i=0; i<param->poke_cnt; ++i)
  {
    pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID[i] );
    if( !BPP_IsDead(pp_target) ){
      SCQUE_PUT_OP_RankReset( wk->que, param->pokeID[i] );
      BPP_RankReset( pp_target );
      result = 1;
    }
  }
  return result;
}
/**
 * �|�P�����\�͒l�i�U���A�h�䓙�j���w��l�ɏ�������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setStatus( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SET_STATUS* param = (BTL_HANDEX_PARAM_SET_STATUS*)param_header;
  BTL_POKEPARAM* bpp;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
    if( param->fAttackEnable ){
      BPP_SetBaseStatus( bpp, BPP_ATTACK, param->attack );
      SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_ATTACK, param->attack );
    }
    if( param->fDefenceEnable ){
      BPP_SetBaseStatus( bpp, BPP_DEFENCE, param->defence );
      SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_DEFENCE, param->defence );
    }
    if( param->fSpAttackEnable ){
      BPP_SetBaseStatus( bpp, BPP_SP_ATTACK, param->sp_attack );
      SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_SP_ATTACK, param->sp_attack );
    }
    if( param->fSpDefenceEnable ){
      BPP_SetBaseStatus( bpp, BPP_SP_DEFENCE, param->sp_defence );
      SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_SP_DEFENCE, param->sp_defence );
    }
    if( param->fAgilityEnable ){
      BPP_SetBaseStatus( bpp, BPP_AGILITY, param->agility );
      SCQUE_PUT_OP_SetStatus( wk->que, param->pokeID, BPP_AGILITY, param->agility );
    }

    handexSub_putString( wk, &param->exStr );

    return 1;
  }
  return 0;
}
/**
 * �|�P�����������I�ɂЂ񎀂ɂ�����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_kill( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_KILL* param = (BTL_HANDEX_PARAM_KILL*)param_header;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* pp_target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
    if( !BPP_IsDead(pp_target) ){
      scproc_KillPokemon( wk, pp_target );
      handexSub_putString( wk, &param->exStr );
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P�����^�C�v�ύX
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeType( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TYPE* param= (const BTL_HANDEX_PARAM_CHANGE_TYPE*)param_header;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
    if( !BPP_IsDead(bpp) )
    {
      SCQUE_PUT_OP_ChangePokeType( wk->que, param->pokeID, param->next_type );
      BPP_ChangePokeType( bpp, param->next_type );

      if( !(param->fStdMsgDisable) )
      {
        if( PokeTypePair_IsPure( param->next_type ) ){
          PokeType type_pure = PokeTypePair_GetType1( param->next_type );
          SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_ChangePokeType, param->pokeID, type_pure );
        }
      }

      return 1;
    }
  }
  return 0;
}
/**
 * SET���b�Z�[�W�\��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_message( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_MESSAGE* param = (const BTL_HANDEX_PARAM_MESSAGE*)(param_header);
  const BTL_POKEPARAM* pp_user = NULL;
  if( param_header->userPokeID != BTL_POKEID_NULL ){
    pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  }

  if( param_header->tokwin_flag && (pp_user!=NULL) ){
    scPut_TokWin_In( wk, pp_user );
  }

  handexSub_putString( wk, &param->str );

  if( param_header->tokwin_flag && (pp_user!=NULL) ){
    scPut_TokWin_Out( wk, pp_user );
  }

  return 1;
}
/**
 * �|�P�����^�[���t���O�Z�b�g
 * @return ������ 1 / ���s�� 0
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
 * �|�P�����^�[���t���O���Z�b�g
 * @return ������ 1 / ���s�� 0
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
 * �|�P�����p���t���O�Z�b�g
 * @return ������ 1 / ���s�� 0
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
 * �|�P�����p���t���O���Z�b�g
 * @return ������ 1 / ���s�� 0
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
 * �T�C�h�G�t�F�N�g�ǉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_sideEffectAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_ADD* param = (const BTL_HANDEX_PARAM_SIDEEFF_ADD*)(param_header);
  BPP_SICK_CONT cont = param->cont;

  scEvent_CheckSideEffectParam( wk, param_header->userPokeID, param->effect, param->side, &cont );

  if( BTL_HANDLER_SIDE_Add(param->side, param->effect, cont) )
  {
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �T�C�h�G�t�F�N�g�n���h������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_sideEffectRemove( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SIDEEFF_REMOVE* param = (const BTL_HANDEX_PARAM_SIDEEFF_REMOVE*)(param_header);
  u32 i;
  u8 result = 0;
  for(i=BTL_SIDEEFF_START; i<BTL_SIDEEFF_MAX; ++i)
  {
    if( BTL_CALC_BITFLG_Check(param->flags, i) )
    {
      if( BTL_HANDLER_SIDE_Remove(param->side, i) )
      {
        scPut_SideEffectOffMsg( wk, i, param->side );
        result = 1;
      }
    }
  }
  return result;
}
/**
 * �t�B�[���h�G�t�F�N�g�n���h���ǉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_addFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_FLDEFF* param = (const BTL_HANDEX_PARAM_ADD_FLDEFF*)(param_header);

  if( scproc_FieldEffectCore(wk, param->effect, param->cont, param->fAddDependPoke) )
  {
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �t�B�[���h�G�t�F�N�g�n���h���폜
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_removeFieldEffect( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_REMOVE_FLDEFF* param = (const BTL_HANDEX_PARAM_REMOVE_FLDEFF*)(param_header);

  if( BTL_FIELD_RemoveEffect(param->effect) ){
    scproc_FieldEff_End( wk, param->effect );
    return 1;
  }
  return 0;
}

/**
 * �V��ω�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeWeather( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_WEATHER* param = (const BTL_HANDEX_PARAM_CHANGE_WEATHER*)(param_header);
  const BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  BOOL result;

  if( param->weather != BTL_WEATHER_NONE )
  {
    if( scproc_ChangeWeatherCheck(wk, param->weather, param->turn) )
    {
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( wk, pp_user );
      }
      scproc_ChangeWeatherCore( wk, param->weather, param->turn );
      result = TRUE;
      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( wk, pp_user );
      }
    }
  }
  else
  {
    BtlWeather prevWeather = BTL_FIELD_GetWeather();
    if( prevWeather != BTL_WEATHER_NONE )
    {
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( wk, pp_user );
      }

      BTL_FIELD_ClearWeather();
      SCQUE_PUT_ACT_WeatherEnd( wk->que, prevWeather );
      result = TRUE;

      if( param_header->tokwin_flag ){
        scPut_TokWin_In( wk, pp_user );
      }

    }
    else{
      result = FALSE;
    }
  }
  if( result ){
    handexSub_putString( wk, &param->exStr );
  }


  return result;
}


//----------------------------------------------------------------------------------
// �����o�͋��ʏ���
//----------------------------------------------------------------------------------
static BOOL handexSub_putString( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_STR_PARAMS* strParam )
{
  switch( strParam->type ){
  case BTL_STRTYPE_STD:
    scPut_Message_StdEx( wk, strParam->ID, strParam->argCnt, strParam->args );
    return TRUE;
  case BTL_STRTYPE_SET:
    scPut_Message_SetEx( wk, strParam->ID, strParam->argCnt, strParam->args );
    return TRUE;
  }
  return FALSE;
}

/**
 * �ʒu�G�t�F�N�g�n���h���ǉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_PosEffAdd( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_POSEFF_ADD* param = (const BTL_HANDEX_PARAM_POSEFF_ADD*)(param_header);

  if( BTL_HANDLER_POS_Add(param->effect, param->pos, param_header->userPokeID, param->param, param->param_cnt) != NULL ){
    BTL_N_Printf( DBGSTR_SVFL_PosEffAdd, param->pos, param->effect, param_header->userPokeID);
    return 1;
  }
  BTL_N_Printf( DBGSTR_SVFL_PosEffDupFail, param->pos, param->effect, param_header->userPokeID );
  return 0;
}
/**
 * �|�P�����Ƃ�������������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_tokuseiChange( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_TOKUSEI* param = (const BTL_HANDEX_PARAM_CHANGE_TOKUSEI*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  u16 prevTokusei = BPP_GetValue( bpp, BPP_TOKUSEI );

  if( BTL_TABLES_IsNeverChangeTokusei(prevTokusei) ){
    return 0;
  }

  if( ((param->fSameTokEffective) || ( param->tokuseiID != prevTokusei ))
  ){
    if( param_header->tokwin_flag ){
      SCQUE_PUT_TOKWIN_IN( wk->que, param_header->userPokeID );
    }

    SCQUE_PUT_ACT_ChangeTokusei( wk->que, param->pokeID, param->tokuseiID );
    handexSub_putString( wk, &param->exStr );

    // �Ƃ����������������O�C�x���g
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_ChangeTokuseiBefore( wk, param->pokeID, prevTokusei, param->tokuseiID );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }

    BTL_HANDLER_TOKUSEI_Remove( bpp );
    BPP_ChangeTokusei( bpp, param->tokuseiID );
    SCQUE_PUT_OP_ChangeTokusei( wk->que, param->pokeID, param->tokuseiID );
    {
      BTL_EVENT_FACTOR* p = BTL_HANDLER_TOKUSEI_Add( bpp );
    }


    SCQUE_PUT_TOKWIN_OUT( wk->que, param->pokeID );

    // �Ƃ������������������C�x���g
    if( prevTokusei != param->tokuseiID )
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_HandExTokChangeEventCall, param->pokeID );
      scEvent_ChangeTokuseiAfter( wk, param->pokeID );
      BTL_N_PrintfEx( PRINT_CHANNEL_PRESSURE, DBGSTR_SVFL_HandExTokChangeEventCallEnd );

      BTL_Hem_PopState( &wk->HEManager, hem_state );
    }

    if( param_header->tokwin_flag ){
      SCQUE_PUT_TOKWIN_OUT( wk->que, param_header->userPokeID );
    }

    if( prevTokusei == POKETOKUSEI_BUKIYOU ){
      scproc_CheckItemReaction( wk, bpp, BTL_ITEMREACTION_GEN );
    }

    return 1;
  }
  return 0;
}
/**
 * �|�P���������A�C�e����������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_ITEM* param = (const BTL_HANDEX_PARAM_SET_ITEM*)(param_header);

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  // �������g�ւ̃��N�G�X�g�łȂ���΃A�C�e���Z�b�g�̎��s�`�F�b�N
  if( param_header->userPokeID != param->pokeID )
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    u8  failed = scEvent_CheckItemSet( wk, bpp, param->itemID );
    BTL_Hem_PopState( &wk->HEManager, hem_state );

    if( failed ){
      return 0;
    }
  }

  // �����܂ŗ����琬��
  scproc_ItemChange( wk, bpp, param->itemID );

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_IN( wk->que, param_header->userPokeID );
  }

  handexSub_putString( wk, &param->exStr );

  if( param_header->tokwin_flag ){
    SCQUE_PUT_TOKWIN_OUT( wk->que, param_header->userPokeID );
  }

  // ��������N���A
  if( param->fClearConsume ){
    BPP_ClearConsumedItem( bpp );
    SCQUE_PUT_OP_ClearConsumedItem( wk->que, param->pokeID );
  }

  scproc_CheckItemReaction( wk, bpp, BTL_ITEMREACTION_GEN );
  return 1;
}
/**
 * �|�P���������A�C�e������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_swapItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SWAP_ITEM* param = (const BTL_HANDEX_PARAM_SWAP_ITEM*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* self = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );
  u16 selfItem = BPP_GetItem( self );
  u16 targetItem = BPP_GetItem( target );

  // �Ώۂ̔\�͂Ŏ��s����P�[�X���`�F�b�N
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    u8  failed = scEvent_CheckItemSet( wk, target, selfItem );
    BTL_Hem_PopState( &wk->HEManager, hem_state );
    if( failed ){
      return 0;
    }
  }

  // �����܂ŗ����琬��
  if( param_header->tokwin_flag ){
    scPut_TokWin_In( wk, self );
  }
  handexSub_putString( wk, &param->exStr );
  handexSub_putString( wk, &param->exSubStr1 );
  handexSub_putString( wk, &param->exSubStr2 );
  if( param_header->tokwin_flag ){
    scPut_TokWin_Out( wk, self );
  }

  {
    scproc_ItemChange( wk, self, targetItem );
    scproc_ItemChange( wk, target, selfItem );
  }

  scproc_CheckItemReaction( wk, self, BTL_ITEMREACTION_GEN );
  scproc_CheckItemReaction( wk, target, BTL_ITEMREACTION_GEN );

  return 1;
}
/**
 * �|�P���������A�C�e�������`�F�b�N
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_EquipItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_EQUIP_ITEM* param = (const BTL_HANDEX_PARAM_EQUIP_ITEM*)(param_header);
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  scproc_CheckItemReaction( wk, bpp, BTL_ITEMREACTION_GEN );
  return 1;
}
/**
 * �����A�C�e���g�p
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_useItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_USE_ITEM* param = (const BTL_HANDEX_PARAM_USE_ITEM*)(param_header);
  BTL_POKEPARAM* pp_user = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( !BPP_IsDead(pp_user) || param->fUseDead )
  {
    if( param->fSkipHPFull && BPP_IsHPFull(pp_user) ){
      return 0;
    }

    if( scproc_UseItemEquip(wk, pp_user) ){
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P�����ɃA�C�e�����ʂ���������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_ItemSP( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ITEM_SP* param = (BTL_HANDEX_PARAM_ITEM_SP*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    BTL_EVENT_FACTOR* factor = BTL_HANDLER_ITEM_TMP_Add( bpp, param->itemID );
    if( factor )
    {
      u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
      scEvent_ItemEquipTmp( wk, bpp, param_header->userPokeID );
      BTL_Hem_PopState( &wk->HEManager, hem_state );
      BTL_HANDLER_ITEM_TMP_Remove( factor );
      return 1;
    }
  }
  return 0;
}
/**
 * �|�P���������̃A�C�e��������b�Z�[�W�\��
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_consumeItem( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CONSUME_ITEM* param = (const BTL_HANDEX_PARAM_CONSUME_ITEM*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  scPut_UseItemAct( wk, bpp );
  handexSub_putString( wk, &param->exStr );

  scproc_ConsumeItem( wk, bpp );
  scproc_ItemChange( wk, bpp, ITEM_DUMMY_DATA );

//  scPut_ConsumeItem( wk, bpp );
//  scPut_SetTurnFlag( wk, bpp, BPP_TURNFLG_ITEM_CONSUMED );


  return 1;
}

/**
 * �|�P�����킴��������
 * @return ������ 1 / ���s�� 0
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
 * �|�P�����J�E���^�l��������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_counter( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_COUNTER* param = (const BTL_HANDEX_PARAM_COUNTER*)(param_header);

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  scPut_SetBppCounter( wk, target, param->counterID, param->value );
  return 1;
}
/**
 * ���ԍ����U�_���[�W
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_delayWazaDamage( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_DELAY_WAZADMG* param = (const BTL_HANDEX_PARAM_DELAY_WAZADMG*)param_header;

  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, param->attackerPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->targetPokeID );

  SVFL_WAZAPARAM   wazaParam;
  WAZAEFF_CTRL  ctrlBackup;
  u16  que_reserve_pos;
  u8 result;

  scEvent_GetWazaParam( wk, param->wazaID, attacker, &wazaParam );

  // ���U���b�Z�[�W�C�G�t�F�N�g�C���U�o���m��
  que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );

  // ���U�Ώۂ����[�N�Ɏ擾
  BTL_POKESET_Clear( wk->psetDamaged );
  BTL_POKESET_Clear( wk->psetTarget );
  BTL_POKESET_Add( wk->psetTarget, target );

  // ��ɂ��Ȃ��|�P�����ɂ͓�����Ȃ��i�␳�Ȃ��j
  BTL_POKESET_RemoveDeadPoke( wk->psetTarget );
  if( BTL_POKESET_IsRemovedAll(wk->psetTarget) ){
    scPut_WazaFail( wk, attacker, wazaParam.wazaID );
    return 0;
  }

  // �Ώۂ��Ƃ̖����`�F�b�N������`�F�b�N
  flowsub_checkWazaAffineNoEffect( wk, &wazaParam, attacker, wk->psetTarget, &wk->dmgAffRec );
  flowsub_checkNotEffect( wk, &wazaParam, attacker, wk->psetTarget );
  flowsub_checkWazaAvoid( wk, &wazaParam, attacker, wk->psetTarget );
  // �ŏ��͋����^�[�Q�b�g���c���Ă��Ȃ� -> ���������I��
  if( BTL_POKESET_IsRemovedAll(wk->psetTarget) ){
    return 0;
  }

  // ���U�G�t�F�N�g�Ǘ��̃o�b�N�A�b�v�����A�V�X�e��������
  ctrlBackup = *(wk->wazaEffCtrl);
  wazaEffCtrl_Init( wk->wazaEffCtrl );
  wazaEffCtrl_Setup( wk->wazaEffCtrl, wk, attacker, wk->psetTarget );

  scproc_Fight_Damage_Root( wk, &wazaParam, attacker, wk->psetTarget, &wk->dmgAffRec, TRUE );

  // ���U���ʂ���m�聨���o�\���R�}���h�����Ȃǂ�
  result = wazaEffCtrl_IsEnable( wk->wazaEffCtrl );
  if( result ){
    wazaEffCtrl_SetEffectIndex( wk->wazaEffCtrl, BTLV_WAZAEFF_DELAY_ATTACK );
    scPut_WazaEffect( wk, wazaParam.wazaID, wk->wazaEffCtrl, que_reserve_pos );
  }

  *(wk->wazaEffCtrl) = ctrlBackup;
  return result;
}
/**
 * �o�g�����E
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_quitBattle( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  if( scproc_NigeruCore(wk, bpp, TRUE) )
  {
    wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
    return 1;
  }
  return 0;
}
/**
 * �����o�[����ւ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeMember( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_CHANGE_MEMBER* param = (BTL_HANDEX_PARAM_CHANGE_MEMBER*)param_header;
  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  if( (!scproc_CheckShowdown(wk))
  &&  (!checkFreeFallUsing(bpp))
  &&  (wk->flowResult == SVFLOW_RESULT_DEFAULT)
  ){
    handexSub_putString( wk, &param->preStr );
    if( scproc_MemberOutForChange(wk, bpp, param->fIntrDisable) )
    {
      BtlPokePos pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param->pokeID );

      BTL_SERVER_RequestChangePokemon( wk->server, pos );
      handexSub_putString( wk, &param->exStr );
      wk->flowResult = SVFLOW_RESULT_POKE_CHANGE;
      return 1;
    }
  }
  return 0;
}
/**
 * �o�g���^�b�`
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_batonTouch( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_BATONTOUCH* param = (BTL_HANDEX_PARAM_BATONTOUCH*)param_header;

  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon, param->userPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->targetPokeID );

  BPP_BatonTouchParam( target, user );
  SCQUE_PUT_OP_BatonTouch( wk->que, param->userPokeID, param->targetPokeID );
  if( BPP_MIGAWARI_IsExist(user) )
  {
    BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, param->targetPokeID );
    SCQUE_PUT_ACT_MigawariCreate( wk->que, pos );
  }
  return 1;
}
/**
 * �Ђ�܂���
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_addShrink( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_ADD_SHRINK* param = (BTL_HANDEX_PARAM_ADD_SHRINK*)param_header;

  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( scproc_AddShrinkCore(wk, target, param->per) ){
    return 1;
  }
  return 0;
}
/**
 * �m�������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_relive( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_RELIVE* param = (BTL_HANDEX_PARAM_RELIVE*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BTL_N_Printf( DBGSTR_SVFL_HandExRelive, param->pokeID, param->recoverHP );
  BPP_HpPlus( target, param->recoverHP );
  SCQUE_PUT_OP_HpPlus( wk->que, param->pokeID, param->recoverHP );
  wk->pokeDeadFlag[param->pokeID] = FALSE;

  {
    BtlPokePos targetPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param->pokeID );
    if( targetPos != BTL_POS_NULL )
    {
      BTL_POSPOKE_PokeIn( &wk->pospokeWork, targetPos, param->pokeID, wk->pokeCon );
      SCQUE_PUT_ACT_RelivePoke( wk->que, param->pokeID );
      BTL_HANDLER_TOKUSEI_Add( target );
      BTL_HANDLER_ITEM_Add( target );
    }
  }

  handexSub_putString( wk, &param->exStr );
  return 1;
}
/**
 * �̏d���Z�b�g
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setWeight( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SET_WEIGHT* param = (BTL_HANDEX_PARAM_SET_WEIGHT*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );

  BPP_SetWeight( target, param->weight );
  SCQUE_PUT_OP_SetWeight( wk->que, param->pokeID, param->weight );
  handexSub_putString( wk, &param->exStr );
  return 1;
}
/**
 * �ꂩ�琁����΂�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_pushOut( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_PUSHOUT* param = (BTL_HANDEX_PARAM_PUSHOUT*)param_header;
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  BTL_POKEPARAM* attacker = BTL_POKECON_GetPokeParam( wk->pokeCon, param_header->userPokeID );

  BOOL fFailMsgDisped;

  if( scproc_PushOutCore(wk, attacker, target, param->fForceChange, &fFailMsgDisped,
        param->effectNo, param->fIgnoreLevel, &param->exStr) )
  {
    return 1;
  }
  return 0;
}
/**
 * �w��|�P�����̍s�������荞�݂�����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_intrPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_INTR_POKE* param = (BTL_HANDEX_PARAM_INTR_POKE*)param_header;

  if( ActOrder_IntrReserve(wk, param->pokeID) ){
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �w�胏�U�g�p�|�P�����̍s�������荞�݂�����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_intrWaza( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_INTR_WAZA* param = (BTL_HANDEX_PARAM_INTR_WAZA*)param_header;

  if( ActOrder_IntrReserveByWaza(wk, param->waza) ){
    return 1;
  }
  return 0;
}
/**
 * �w��|�P�����̍s�������Ō�ɉ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_sendLast( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SEND_LAST* param = (BTL_HANDEX_PARAM_SEND_LAST*)param_header;

  if( ActOrder_SendLast(wk, param->pokeID) ){
    handexSub_putString( wk, &param->exStr );
    return 1;
  }
  return 0;
}
/**
 * �w��|�P�������m�̏ꏊ����ւ�
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_swapPoke( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_SWAP_POKE* param = (BTL_HANDEX_PARAM_SWAP_POKE*)param_header;

  if( param->pokeID1 != param->pokeID2 )
  {
    u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( param->pokeID1 );
    if( clientID == BTL_MAINUTIL_PokeIDtoClientID(param->pokeID2) )
    {
      const BTL_POKEPARAM* bpp1 = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID1 );
      const BTL_POKEPARAM* bpp2 = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID2 );
      if( !BPP_IsDead(bpp1) && !BPP_IsDead(bpp2) )
      {
        const BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
        s16 posIdx1 = BTL_PARTY_FindMember( party, bpp1 );
        s16 posIdx2 = BTL_PARTY_FindMember( party, bpp2 );
        if( (posIdx1 >= 0) && (posIdx2 >= 0) )
        {
          scproc_MoveCore( wk, clientID, posIdx1, posIdx2, TRUE );
          handexSub_putString( wk, &param->exStr );
          return 1;
        }
      }
    }
  }
  return 0;
}
/**
 * �w�b�_�|�P�������w��|�P�����ɕϐg
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_hensin( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_HENSIN* param = (BTL_HANDEX_PARAM_HENSIN*)param_header;

  BTL_POKEPARAM* user = BTL_POKECON_GetPokeParam( wk->pokeCon,  param_header->userPokeID );
  BTL_POKEPARAM* target = BTL_POKECON_GetPokeParam( wk->pokeCon,  param->pokeID );

  if( (!BPP_IsFakeEnable(user))
  &&  (!BPP_IsFakeEnable(target))
  ){
//    TAYA_Printf("PokeSick=%d, line=%d\n", BPP_GetPokeSick(user), __LINE__);
    if( BPP_HENSIN_Set(user, target) )
    {
      u8 usrPokeID = BPP_GetID( user );
      u8 tgtPokeID = BPP_GetID( target );

//      TAYA_Printf("PokeSick=%d, line=%d\n", BPP_GetPokeSick(user), __LINE__);

      if( param_header->tokwin_flag ){
        scPut_TokWin_In( wk, user );
      }

      BTL_HANDLER_Waza_RemoveForceAll( user );
      BTL_HANDLER_TOKUSEI_Remove( user );
      BTL_HANDLER_TOKUSEI_Add( user );
      SCQUE_PUT_ACT_Hensin( wk->que, usrPokeID, tgtPokeID );

      handexSub_putString( wk, &param->exStr );
      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( wk, user );
      }
      return 1;
    }
  }
  return 0;
}
/**
 * �C�����[�W��������
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_fakeBreak( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  BTL_HANDEX_PARAM_HENSIN* param = (BTL_HANDEX_PARAM_HENSIN*)param_header;

  if( BTL_POSPOKE_IsExist(&wk->pospokeWork, param->pokeID) )
  {
    BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon,  param->pokeID );

    if( BPP_IsFakeEnable(bpp) )
    {
      BPP_FakeDisable( bpp );
      SCQUE_PUT_ACT_FakeDisable( wk->que, param->pokeID );
      handexSub_putString( wk, &param->exStr );
      return 1;
    }
  }
  return 0;
}
/**
 * ���イ��傭�������`�F�b�N�i����ƂԁE�_�C�r���O���ӂ䂤�t���O�𗎂Ƃ��j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_juryokuCheck( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  u8 pokeID[ BTL_POS_MAX ];
  u8 cnt, pos, fFall, i;
  BtlExPos exPos;
  BTL_POKEPARAM* bpp;

  pos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, param_header->userPokeID );
  exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ALL, pos );
  cnt = BTL_SVFTOOL_ExpandPokeID( wk, exPos, pokeID );
  for(i=0; i<cnt; ++i)
  {
    bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID[i] );
    fFall = FALSE;
    if( BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_SORAWOTOBU) )
    {
      scproc_TameHideCancel( wk, bpp, BPP_CONTFLG_SORAWOTOBU );
      fFall = TRUE;
    }
    if( scEvent_CheckFloating(wk, bpp) )
    {
      fFall = TRUE;
    }
    if( BPP_CheckSick(bpp, WAZASICK_FLYING) )
    {
      scPut_CureSick( wk, bpp, WAZASICK_FLYING, NULL );
      fFall = TRUE;
    }

    if( fFall ){
      SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_JyuryokuFall, pokeID[i] );
    }
  }

  return 1;
}
/**
 * ���߃��U������Ԃ̃L�����Z���i����ƂԁE�_�C�r���O���j
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_TameHideCancel( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_TAMEHIDE_CANCEL* param = (const BTL_HANDEX_PARAM_TAMEHIDE_CANCEL*)param_header;

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    if( scproc_TameHideCancel(wk, bpp, param->flag) )
    {
      handexSub_putString( wk, &param->exStr );
      return 1;
    }
  }
  return 0;
}
/**
 * �ʒu�w��G�t�F�N�g����
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_effectByPos( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_ADD_EFFECT* param = (const BTL_HANDEX_PARAM_ADD_EFFECT*)(param_header);

  if( param->fMsgWinVanish ){
    SCQUE_PUT_ACT_MsgWinHide( wk->que, 0 );
  }

  scproc_ViewEffect( wk, param->effectNo, param->pos_from, param->pos_to, param->fQueReserve, param->reservedQuePos );

  return 1;
}

/**
 * �t�H�����i���o�[�`�F���W
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_changeForm( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_CHANGE_FORM* param = (const BTL_HANDEX_PARAM_CHANGE_FORM*)param_header;

  BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, param->pokeID );
  if( !BPP_IsDead(bpp) )
  {
    u8 currentForm = BPP_GetValue( bpp, BPP_FORM );
    if( currentForm != param->formNo )
    {
      if( param_header->tokwin_flag ){
        scPut_TokWin_In( wk, bpp );
      }

      BPP_ChangeForm( bpp, param->formNo );
      SCQUE_PUT_ACT_ChangeForm( wk->que, param->pokeID, param->formNo );
      handexSub_putString( wk, &param->exStr );

      if( param_header->tokwin_flag ){
        scPut_TokWin_Out( wk, bpp );
      }

      return 1;
    }
  }
  return 0;
}
/**
 * ���U�G�t�F�N�g�C���f�b�N�X�ύX
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setWazaEffectIndex( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_SET_EFFECT_IDX* param = (const BTL_HANDEX_PARAM_SET_EFFECT_IDX*)param_header;

  wazaEffCtrl_SetEffectIndex( wk->wazaEffCtrl, param->effIndex );
  return 1;
}
/**
 * ���U�G�t�F�N�g�������I�ɗL���ɂ���
 * @return ������ 1 / ���s�� 0
 */
static u8 scproc_HandEx_setWazaEffectEnable( BTL_SVFLOW_WORK* wk, const BTL_HANDEX_PARAM_HEADER* param_header )
{
  const BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE* param = (const BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE*)param_header;

  wazaEffCtrl_SetEnable( wk->wazaEffCtrl );
  return 1;
}


/*

typedef struct {

  u8    type;       ///< �ق̂��A�݂��A�����Ȃǂ̃^�C�v
  u8    category;   ///< ���U�J�e�S���i���P�j
  s8    priority;   ///< �D��x
  u8    hitPer;     ///< ������
  u8    damageType; ///< �_���[�W�^�C�v�i"�Ȃ�"=0, "����"=1, "����"=2�j
  u8    power;      ///< �З͒l
  u8    critical;   ///< �N���e�B�J�������N
  u8    hitMax;     ///< �ő�q�b�g��
  u8    hitMin;     ///< �ŏ��q�b�g��
  u8    shrinkPer;  ///< �Ђ�݊m��

  u8    sickID;     ///< ��Ԉُ����
  u8    sickCont;   ///< ��Ԉُ�̌p���p�^�[��
  u8    sickTurnMin;///< ��Ԉُ�̍ŏ��^�[����
  u8    sickTurnMax;///< ��Ԉُ�̍ő�^�[����
  u8    sickPer;    ///< �ǉ����ʂɂ���Ԉُ�̔�����

  struct {
    u8 statusID;
    s8 value;
    u8 per;
  }rankEffect[ 3 ];

  s8    recoverRatio_damage;
  s8    recoverRatio_HP;

  u8    coverArea;

  u16   touchFlag : 1;      ///< �ڐG���U
  u16   tameFlag  : 1;      ///< �P�^�[������
  u16   tireFlag  : 1;      ///< �P�^�[�������œ����Ȃ�
  u16   guardFlag : 1;      ///< �u�܂���v�u�݂���v�̑Ώ�
  u16   magicCoatFlag : 1;  ///< �u�}�W�b�N�R�[�g�v�̑Ώ�
  u16   yokodoriFlag : 1;   ///< �u�悱�ǂ�v�̑Ώ�
  u16   oumuFlag  : 1;      ///< �u�I�E���������v�̑Ώ�
  u16   hitFarFlag : 1;     ///< �g���v���ŗ��ꂽ�|�P�����ɂ�������



}WAZADATA;

/*
���P
"�V���v���_���[�W"=0
"�V���v���V�b�N"
"

*/

