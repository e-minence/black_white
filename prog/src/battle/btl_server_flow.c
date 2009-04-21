//=============================================================================================
/**
 * @file	btl_server_flow.c
 * @brief	�|�P����WB �o�g���V�X�e��	�T�[�o�R�}���h��������
 * @author	taya
 *
 * @date	2009.03.06	�쐬
 */
//=============================================================================================
#include <gflib.h>
#include "waza_tool\wazadata.h"
#include "waza_tool\wazano_def.h"

#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_server_local.h"
#include "btl_server.h"

#include "handler\hand_tokusei.h"
#include "handler\hand_item.h"

#include "btl_server_flow.h"


//--------------------------------------------------------
/**
 *		�T�[�o�t���[�����t���O
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
 *	�ӂ��Ƃ΂��n�̃��U�����p�^�[��
 */
typedef enum {
	PUSHOUT_EFFECT_ESCAPE,		///< �������̂Ɠ����i�퓬���I������j
	PUSHOUT_EFFECT_CHANGE,		///< ��������ւ�������i�莝�������Ȃ���Ύ��s����j
	PUSHOUT_EFFECT_MUSTFAIL,	///< �K�����s����
}PushOutEffect;


/**
 *	�A�N�V�����D�揇�L�^�\����
 */
typedef struct {
	u8  clientID;		///< �N���C�A���gID
	u8  pokeIdx;		///< ���̃N���C�A���g�́A���̖ځH 0�`
}ACTION_ORDER_WORK;


/**
 * �ΏۂƂȂ�|�P�����p�����[�^���L�^���Ă������߂̃��[�N
 */
typedef struct {

	BTL_POKEPARAM*	pp[ BTL_POS_MAX ];
	u16							damage[ BTL_POS_MAX ];
	u8							count;
	u8							idx;

}TARGET_POKE_REC;

/**
 *	�퓬�ɏo�Ă���|�P�����f�[�^�ɏ��ԂɃA�N�Z�X���鏈���̃��[�N
 */
typedef struct {
	u8 clientIdx;
	u8 pokeIdx;
	u8 endFlag;
}FRONT_POKE_SEEK_WORK;


typedef struct {

	u8		work[ EVENT_WORK_TOTAL_SIZE ];
	u16		size[ EVENT_WORK_DEPTH ];
	u8		sp;

}BTL_EVENT_WORK_STACK;

//-----------------------------------------------------
/**
 *	���C�����[�N
 */
//-----------------------------------------------------
struct _BTL_SVFLOW_WORK {

	BTL_SERVER*             server;
	const BTL_MAIN_MODULE*  mainModule;
	BTL_POKE_CONTAINER*			pokeCon;
	BTL_SERVER_CMD_QUE*			que;
	u32											turnCount;
	SvflowResult						flowResult;
	u8                      numClient;
	u8			wazaEff_EnableFlag;
	u8			wazaEff_TargetPokeID;

	ACTION_ORDER_WORK		actOrder[ BTL_POS_MAX ];
	TARGET_POKE_REC			targetPokemon;
	TARGET_POKE_REC			targetSubPokemon;
	TARGET_POKE_REC			damagedPokemon;
	SVFL_WAZAPARAM			wazaParam;

	BTL_EVENT_WORK_STACK	eventWork;

	u8					flowFlags[ FLOWFLG_BYTE_MAX ];
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
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
static inline void TargetPokeRec_Remove( TARGET_POKE_REC* rec, BTL_POKEPARAM* bpp );
static inline BTL_POKEPARAM* TargetPokeRec_GetNext( TARGET_POKE_REC* rec );
static inline void TargetPokeRec_GetStart( TARGET_POKE_REC* rec );
static inline BTL_POKEPARAM* TargetPokeRec_Get( const TARGET_POKE_REC* rec, u32 idx );
static inline u32 TargetPokeRec_GetDamage( const TARGET_POKE_REC* rec, u32 idx );
static inline int TargetPokeRec_SeekFriendIdx( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, u32 start_idx );
static inline u32 TargetPokeRec_GetCount( const TARGET_POKE_REC* rec );
static u32 TargetPokeRec_CopyFriends( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst );
static u32 TargetPokeRec_CopyEnemys( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, TARGET_POKE_REC* dst );
static void TargetPokeRec_RemoveDeadPokemon( TARGET_POKE_REC* rec );
static BOOL scput_Nigeru( BTL_SVFLOW_WORK* wk, u8 clientID, u8 pokeIdx );
static void scproc_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 pokeIdx, BOOL fBtlStart );
static void scproc_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx );
static void scproc_Fight( BTL_SVFLOW_WORK* wk, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action );
static u8 flowsub_registerWazaTargets( BTL_SVFLOW_WORK* wk, BtlPokePos atPos, WazaID waza, const BTL_ACTION_PARAM* action, TARGET_POKE_REC* rec );
static void flowsub_checkNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BTL_POKEPARAM* svflowsub_get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* svflowsub_get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
static void scproc_Fight_Damage( BTL_SVFLOW_WORK* wk, WazaID waza, SVCL_WORK* atClient,
	 BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scproc_Fight_Damage_side( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker,
	TARGET_POKE_REC* targets, fx32 dmg_ratio );
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
			BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, fx32 targetDmgRatio );
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
		BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
		WazaID waza,   BtlTypeAff aff,   fx32 targetDmgRatio );
static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk, 
		BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, const u8* aff, WazaID waza, fx32 dmg_ratio );
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, WazaID waza,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scproc_WazaDamageReaction_Sick( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scproc_WazaDamageReaction_Rank( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scproc_WazaDamageReaction_Damage( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk );
static BOOL scproc_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
	WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost );
static BOOL addsick_core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
	WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, BOOL* fCarry, BOOL* fItemResponce );
static BOOL scEvent_AddSick_IsMustFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick );
static void scEvent_AddSick_Check( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
		WazaSick sick, BTL_EVWK_ADDSICK* evwk );
static void scproc_UseItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scput_Fight_Damage_After_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BOOL svflowsub_hitcheck_singular( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza );
static u16 scEvent_CalcDamage( BTL_SVFLOW_WORK* wk, 
		const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza,
		BtlTypeAff typeAff, fx32 targetDmgRatio, BOOL criticalFlag );
static void scproc_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage );
static BOOL scproc_SimpleDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage, u16 strID );
static BOOL scEvent_CheckEnableSimpleDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u32 damage );
static BOOL scEvent_SimpleDamage_Reaction( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scproc_Fight_Damage_AddEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target );
static void scproc_Fight_SimpleEffect( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost );
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, BOOL fAlmost );
static void scproc_Fight_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage );
static void scproc_Fight_Ichigeki( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void svflowsub_MakeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker,
		WazaSick sick, BPP_SICK_CONT contParam, BOOL fAlmost );
static void scproc_Fight_PushOut( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static PushOutEffect check_pushout_effect( BTL_SVFLOW_WORK* wk );
static u8 get_pushout_nextpoke_idx( BTL_SVFLOW_WORK* wk, const SVCL_WORK* clwk );
static BOOL scEvent_CheckPushOutFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target );
static void scproc_Fight_Weather( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, const BTL_ACTION_PARAM* action );
static void scput_Fight_Others( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scput_Fight_Others_SkillSwap( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scput_TurnCheck( BTL_SVFLOW_WORK* wk );
static void scput_turncheck_sick( BTL_SVFLOW_WORK* wk );
static void scput_turncheck_weather( BTL_SVFLOW_WORK* wk );
static int scEvent_CalcWeatherDamage( BTL_SVFLOW_WORK* wk, BtlWeather weather, BTL_POKEPARAM* bpp, u8 *tok_cause_flg );
static inline int roundValue( int val, int min, int max );
static inline int roundMin( int val, int min );
static inline int roundMax( int val, int max );
static inline BOOL perOccur( u8 per );
static void eventWork_Init( BTL_EVENT_WORK_STACK* stack );
static void* eventWork_Push( BTL_EVENT_WORK_STACK* stack, u32 size );
static void eventWork_Pop( BTL_EVENT_WORK_STACK* stack, void* adrs );
static void scPut_ConfCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_ConfDamage( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u32 damage );
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scPut_WazaExecuteFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause );
static void scPut_WazaFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza );
static void scPut_WazaAvoid( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza );
static void scput_WazaNoEffect( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scput_DmgToRecover( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, BTL_EVWK_DMG_TO_RECOVER* evwk );
static void scPut_CheckDeadCmd( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* poke );
static void scPut_WazaEffectOn( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, WazaID waza );
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage, BOOL critical_flag );
static void scPut_WazaDamagePlural( BTL_SVFLOW_WORK* wk, u32 poke_cnt, BtlTypeAff aff, 
	BTL_POKEPARAM** bpp, const u16* damage, const u8* critical_flag );
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage );
static void scPut_TokWin_In( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_TokWin_Out( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );
static void scPut_RankEffectLimit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaRankEffect effect, int volume );
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume );
static void scPut_RankEffectFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
	WazaRankEffect effect, const BTL_EVWK_CHECK_RANKEFF* evwk );
static void scPut_Ichigeki( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target );
static void scPut_SimpleHp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, int value );
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont );
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick );
static void scPut_Messae_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID );
static u16 scEvent_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckConf( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
	WazaID waza, u8* curedFlag );
static void scEvent_CheckWazaParam( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, SVFL_WAZAPARAM* param );
static BOOL scEvent_checkHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_IchigekiCheck( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckNotEffect_byType( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_CheckNotEffect( BTL_SVFLOW_WORK* wk, WazaID waza, u8 lv,
		const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
static BOOL scEvent_DmgToRecover( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, WazaID waza, BTL_EVWK_DMG_TO_RECOVER* evwk );
static BOOL scEvent_CheckCritical( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff aff,
	u32 damage, BOOL criticalFlag, BTL_EVWK_DAMAGE_REACTION* evwk );
static void scEvent_WazaDamageEffect_Double( BTL_SVFLOW_WORK* wk,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2, WazaID waza, BtlTypeAff aff,
	u32 dmg1, u32 dmg2 );
static BtlTypeAff scEvent_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, PokeType waza_type );
static void scEvent_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, TARGET_POKE_REC* rec );
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckPluralHit( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u8* hitCount );
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
	const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag );
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk,
	const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag );
static fx32 scEvent_CalcTypeMatchRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, PokeType waza_type );
static PokeTypePair scEvent_getDefenderPokeType( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender );
static void scEvent_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx );
static void scEvent_MemberIn( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp );
static void scEvent_AddShrink( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker, u8 per );
static BOOL scEvent_CheckAddSickPer( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender );
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
static BOOL scEvent_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn );




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
				scproc_MemberIn( wk, i, posIdx, posIdx, TRUE );
			}
		}
	}

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

	numActPoke = sortClientAction( wk, wk->actOrder );
	for(i=0; i<numActPoke; i++)
	{
		clientID = wk->actOrder[i].clientID;
		pokeIdx  = wk->actOrder[i].pokeIdx;
		clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
		bpp = BTL_SVCL_GetFrontPokeData( clwk, pokeIdx );

		if( !BTL_POKEPARAM_IsDead(bpp) )
		{
			const BTL_ACTION_PARAM* action = BTL_SVCL_GetPokeAction( clwk, pokeIdx );

			BTL_POKEPARAM_SetTurnFlag( bpp, BPP_TURNFLG_ACTION_EXE );

			BTL_Printf("Client(%d) �� %d �Ԗڂ̃|�P�����̃A�N�V����\n", clientID, pokeIdx);
			switch( action->gen.cmd ){
			case BTL_ACTION_FIGHT:
				BTL_Printf("�y���������z�������B%d�Ԗڂ̃��U���A%d�Ԃ̑���ɁB\n", action->fight.wazaIdx, action->fight.targetPos);
				scproc_Fight( wk, clientID, pokeIdx, action );
				break;
			case BTL_ACTION_ITEM:
				BTL_Printf("�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n", action->item.number, action->item.targetIdx);
				break;
			case BTL_ACTION_CHANGE:
				BTL_Printf("�y�|�P�����z�������B�ʒu%d <- �|�P%d \n", action->change.posIdx, action->change.memberIdx);
				scproc_MemberOut( wk, clientID, action->change.posIdx );
				scproc_MemberIn( wk, clientID, action->change.posIdx, action->change.memberIdx, FALSE );
				break;
			case BTL_ACTION_ESCAPE:
				BTL_Printf("�y�ɂ���z�������B\n");
				if( scput_Nigeru( wk, clientID, pokeIdx ) ){
					SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeSuccess );
					wk->flowResult = SVFLOW_RESULT_BTL_QUIT;
				}	else {
					SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeFail);
				}
				break;
			}

			if( wk->flowResult !=  SVFLOW_RESULT_DEFAULT )
			{
				return wk->flowResult;
			}
		}
	}

	// �^�[���`�F�b�N����
	scput_TurnCheck( wk );

	alivePokeAfter = countAlivePokemon( wk );

	// ���񂾃|�P����������ꍇ�̏���
	BTL_Printf( "�|�P������ %d -> %d ...\n", alivePokeBefore, alivePokeAfter );
	if( alivePokeBefore > alivePokeAfter )
	{
		return SVFLOW_RESULT_POKE_CHANGE;
	}

	return SVFLOW_RESULT_DEFAULT;
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
 * @param   order		�������鏇�ԂɃN���C�A���gID����ג����Ċi�[���邽�߂̔z��
 *
 */
//--------------------------------------------------------------------------
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order )
{
	/*
		�s���D�揇  ... 2BIT
		���U�D�揇  ... 6BIT
		�f����      ... 16BIT
	*/
	#define MakePriValue( actPri, wazaPri, agility )	\
						( ((actPri)<<22) | ((wazaPri)<<16) | (agility) )

	SVCL_WORK* clwk;
	const BTL_ACTION_PARAM* actParam;
	const BTL_POKEPARAM* bpp;
	u32 pri[ BTL_POS_MAX ];
	u16 agility;
	u8  action, actionPri, wazaPri;
	u8  i, j, p, numPoke;

// �e�|�P�����̍s���v���C�I���e�B�l�𐶐�
	for(i=0, p=0; i<wk->numClient; i++)
	{
		// �N���C�A���g���Ƃ̍s���|�P���������p�����[�^���A�_�v�^����擾
		clwk = BTL_SERVER_GetClientWork( wk->server, i );

		numPoke = BTL_SVCL_GetNumActPoke( clwk );
		for(j=0; j<numPoke; j++)
		{
			bpp = BTL_SVCL_GetFrontPokeData( clwk, j );
			actParam = BTL_SVCL_GetPokeAction( clwk, j );

			// �s���ɂ��D�揇�i�D��x�����قǐ��l��j
			switch( actParam->gen.cmd ){
			case BTL_ACTION_ESCAPE:	actionPri = 3; break;
			case BTL_ACTION_ITEM:		actionPri = 2; break;
			case BTL_ACTION_CHANGE:	actionPri = 1; break;
			case BTL_ACTION_FIGHT:	actionPri = 0; break;
			case BTL_ACTION_NULL:	continue;
			default:
				GF_ASSERT(0);
				actionPri = 0;
				break;
			}
			// ���U�ɂ��D�揇
			if( actParam->gen.cmd == BTL_ACTION_FIGHT )
			{
				WazaID  w = BTL_POKEPARAM_GetWazaNumber( bpp, actParam->fight.wazaIdx );
				wazaPri = WAZADATA_GetPriority( w ) - WAZAPRI_MIN;
			}
			else
			{
				wazaPri = 0;
			}
			// ���΂₳
			agility = scEvent_CalcAgility( wk, bpp );

			BTL_Printf("�s���v���C�I���e�B����I Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
					i, j, actionPri, wazaPri, agility );

			// �v���C�I���e�B�l�ƃN���C�A���gID��΂ɂ��Ĕz��ɕۑ�
			pri[p] = MakePriValue( actionPri, wazaPri, agility );
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
			bpp = BTL_SVCL_GetFrontPokeData( clwk, j );
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
 *	���ԃA�N�Z�X�F���[�N������
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
 *	���ԃA�N�Z�X�F���̃|�P�����f�[�^���擾�iFALSE���Ԃ�����I���j
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
static inline void TargetPokeRec_GetStart( TARGET_POKE_REC* rec )
{
	rec->idx = 0;
}
static inline BTL_POKEPARAM* TargetPokeRec_Get( const TARGET_POKE_REC* rec, u32 idx )
{
	if( idx < rec->count )
	{
		return (BTL_POKEPARAM*)(rec->pp[ idx ]);
	}
	return NULL;
}
static inline u32 TargetPokeRec_GetDamage( const TARGET_POKE_REC* rec, u32 idx )
{
	GF_ASSERT(idx < rec->count);
	return rec->damage[ idx ];
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
		if( BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) )
		{
			TargetPokeRec_Add( dst, bpp );
		}
	}
	return TargetPokeRec_GetCount( dst );
}
// �w��|�P�����ƓG�`�[���̃|�P�����݂̂�ʃ��[�N�ɃR�s�[
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
		if( !BTL_MAINUTIL_IsFriendPokeID(ID1, ID2) )
		{
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

	// �^�C�v�����ɂ�閳�����`�F�b�N
	while( (bpp = TargetPokeRec_GetNext(rec)) != NULL )
	{
		if( BTL_POKEPARAM_IsDead(bpp) )
		{
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

	BOOL fEscape = TRUE;	// @@@ ���͂��΂₳�v�Z�������N�ł���������悤�ɂ��Ă���

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
static void scproc_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 pokeIdx, BOOL fBtlStart )
{
	SVCL_WORK* clwk;
	BTL_PARTY* party;
	BTL_POKEPARAM* bpp;

	party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
	clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

	GF_ASSERT(pokeIdx < clwk->numCoverPos);

	if( posIdx != pokeIdx )
	{
		BTL_PARTY_SwapMembers( party, posIdx, pokeIdx );
	}
	bpp = BTL_PARTY_GetMemberData( party, posIdx );

	clwk->frontMember[ posIdx ] = bpp;

	BTL_HANDLER_TOKUSEI_Add( bpp );
	BTL_HANDLER_ITEM_Add( bpp );
	BTL_POKEPARAM_SetAppearTurn( bpp, wk->turnCount );

	SCQUE_PUT_OP_MemberIn( wk->que, clientID, posIdx, pokeIdx, wk->turnCount );
	if( !fBtlStart )
	{
		SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, pokeIdx, wk->turnCount );
	}

	scEvent_MemberIn( wk, bpp );
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

	atClient = BTL_SERVER_GetClientWork( wk->server, attackClientID );
	attacker = atClient->frontMember[posIdx];
	attacker_pokeID = BTL_POKEPARAM_GetID( attacker );

	waza = BTL_POKEPARAM_GetWazaNumber( attacker, action->fight.wazaIdx );

// �����񎩔�����
	if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KONRAN) )
	{
		scPut_ConfCheck( wk, attacker );
		if( scEvent_CheckConf(wk, attacker ) )
		{
			u16 conf_dmg = scEvent_CalcConfDamage( wk, attacker );
			scPut_ConfDamage( wk, attacker, conf_dmg );
			return;
		}
	}
// ���U�o����������
	{
		SV_WazaFailCause  fail_cause;
		u8 cured_flag;

		fail_cause = scEvent_CheckWazaExecute( wk, attacker, waza, &cured_flag );
		if( cured_flag )
		{
			scPut_CurePokeSick( wk, attacker );
		}
		if( fail_cause != SV_WAZAFAIL_NULL )
		{
			scPut_WazaExecuteFail( wk, attacker, waza, fail_cause );
			return;
		}
	}
// ���U���o�����Ƃɐ���
	{
		WazaCategory  category = WAZADATA_GetCategory( waza );
		BtlPokePos    atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, attacker_pokeID );
		u16  que_reserve_pos;
		BTL_Printf("�o���|�P�ʒu=%d, �o�����U=%d, �J�e�S��=%d\n", atPos, waza, category);

		TargetPokeRec_Clear( &wk->damagedPokemon );
		TargetPokeRec_Clear( &wk->targetPokemon );

		scEvent_CheckWazaParam( wk, waza, attacker, &wk->wazaParam );
		SCQUE_PUT_MSG_WAZA( wk->que, attacker_pokeID, action->fight.wazaIdx );
		que_reserve_pos = SCQUE_RESERVE_Pos( wk->que, SC_ACT_WAZA_EFFECT );
		wk->wazaEff_EnableFlag = FALSE;
		wk->wazaEff_TargetPokeID = BTL_POKEID_NULL;

		// @@@ �����ɁA�Ώۃ|�P�ɂ��Ȃ��������`�F�b�N�����邩���c
		flowsub_registerWazaTargets( wk, atPos, waza, action, &wk->targetPokemon );
		flowsub_checkNotEffect( wk, waza, attacker, &wk->targetPokemon );

		switch( category ){
		case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
		case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
		case WAZADATA_CATEGORY_DAMAGE_EFFECT:
		case WAZADATA_CATEGORY_DAMAGE_SICK:
		case WAZADATA_CATEGORY_DRAIN:
			scproc_Fight_Damage( wk, waza, atClient, attacker, &wk->targetPokemon );
			break;
		case WAZADATA_CATEGORY_SIMPLE_EFFECT:
			scproc_Fight_SimpleEffect( wk, waza, attacker, &wk->targetPokemon );
			break;
		case WAZADATA_CATEGORY_SIMPLE_SICK:
			scproc_Fight_SimpleSick( wk, waza, attacker, &wk->targetPokemon );
			break;
		case WAZADATA_CATEGORY_ICHIGEKI:
			scproc_Fight_Ichigeki( wk, waza, attacker, &wk->targetPokemon );
			break;
		case WAZADATA_CATEGORY_WEATHER:
			scproc_Fight_Weather( wk, waza, attacker, action );
			break;
		case WAZADATA_CATEGORY_PUSHOUT:
			scproc_Fight_PushOut( wk, waza, attacker, &wk->targetPokemon );
			break;
//	case WAZADATA_CATEGORY_BIND:
//	case WAZADATA_CATEGORY_GUARD:
//	case WAZADATA_CATEGORY_FIELD_EFFECT:
//	case WAZADATA_CATEGORY_SIDE_EFFECT:
		case WAZADATA_CATEGORY_OTHERS:
			scput_Fight_Others( wk, waza, attacker, &wk->targetPokemon );
			break;
		default:
//		SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaAvoid );
			SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
			break;
		}

		if( wk->wazaEff_EnableFlag )
		{
			u8 atPokeID = BTL_POKEPARAM_GetID( attacker );
			SCQUE_PUT_ReservedPos( wk->que, que_reserve_pos, SC_ACT_WAZA_EFFECT, atPokeID, wk->wazaEff_TargetPokeID, waza );
		}

		scEvent_decrementPP( wk, attacker, action->fight.wazaIdx, &wk->damagedPokemon );
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
 * @param   rec				[out]
 *	
 * @retval  u8				�Ώۃ|�P������
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
		case WAZA_TARGET_OTHER_SELECT:	///< �����ȊO�̂P�́i�I���j
		case WAZA_TARGET_ENEMY_SELECT:	///< �G�P�́i�I���j
		case WAZA_TARGET_ENEMY_RANDOM:	///< �G�����_��
		case WAZA_TARGET_ENEMY_ALL:			///< �G���Q��
		case WAZA_TARGET_OTHER_ALL:			///< �����ȊO�S��
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam(wk, atPos, 0) );
			break;

		case WAZA_TARGET_USER:			///< �����P�̂̂�
		case WAZA_TARGET_FRIEND_USER_SELECT:	///< �������܂ޖ����P��
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
		case WAZA_TARGET_OTHER_SELECT:				///< �����ȊO�̂P�́i�I���j
			bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, action->fight.targetPos );
			TargetPokeRec_Add( rec, bpp );
			return 1;
		case WAZA_TARGET_ENEMY_SELECT:				///< �G�P�́i�I���j
			bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, action->fight.targetPos );
			TargetPokeRec_Add( rec, bpp );
			return 1;
		case WAZA_TARGET_ENEMY_RANDOM:				///< �G�����_��
			bpp = svflowsub_get_opponent_pokeparam( wk, atPos, GFL_STD_MtRand(1) );
			TargetPokeRec_Add( rec, bpp );
			return 1;

		case WAZA_TARGET_ENEMY_ALL:						///< �G���S��
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam(wk, atPos, 0) );
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam(wk, atPos, 1) );
			return 2;

		case WAZA_TARGET_OTHER_ALL:						///< �����ȊO�S��
			TargetPokeRec_Add( rec, svflowsub_get_next_pokeparam( wk, atPos ) );
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam( wk, atPos, 0 ) );
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam( wk, atPos, 1 ) );
			return 3;

		case WAZA_TARGET_USER:			///< �����P�̂̂�
			TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
			return 1;
		case WAZA_TARGET_FRIEND_USER_SELECT:	///< �������܂ޖ����P�́i�I���j
			TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, action->fight.targetPos) );
			return 1;
		case WAZA_TARGET_FRIEND_SELECT:				///< �����ȊO�̖����P��
			TargetPokeRec_Add( rec, svflowsub_get_next_pokeparam(wk, atPos) );
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
//--------------------------------------------------------------------------
/**
 * ���U�����O�́A�Ώەʖ������`�F�b�N
 *
 * @param   wk				
 * @param   attacker	[in] ���U���o���|�P����
 * @param   targets		[io] ���U�Ώۃ|�P�����R���e�i�^�����ɂȂ����|�P�����̓R���e�i����폜�����
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

	// Lv1 �������`�F�b�N�i�U���|�P���K����ԂȂ畉����j
	if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_MUSTHIT) )
	{
		TargetPokeRec_GetStart( targets );
		while( (bpp = TargetPokeRec_GetNext(targets)) != NULL )
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
}

//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��ʒu���猩�đΐ푊��̃|�P�����f�[�^��Ԃ�
//----------------------------------------------------------------------
static BTL_POKEPARAM* svflowsub_get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx )
{
	SVCL_WORK* clwk;
	BtlPokePos targetPos;
	u8 clientID, posIdx;

	targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, pos, pokeSideIdx );
	BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

	clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

	return clwk->frontMember[ posIdx ];
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��ʒu���猩�ėׂ̃|�P�����f�[�^��Ԃ�
//----------------------------------------------------------------------
static BTL_POKEPARAM* svflowsub_get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos )
{
	SVCL_WORK* clwk;
	BtlPokePos nextPos;
	u8 clientID, posIdx;

	nextPos = BTL_MAIN_GetNextPokePos( wk->mainModule, pos );
	BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, nextPos, &clientID, &posIdx );
	clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

	return clwk->frontMember[ posIdx ];
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n
//----------------------------------------------------------------------
static void scproc_Fight_Damage( BTL_SVFLOW_WORK* wk, WazaID waza, SVCL_WORK* atClient,
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
		bpp = TargetPokeRec_GetNext( targets );
		scproc_Fight_Damage_side_single( wk, attacker, bpp, waza, dmg_ratio );
	}
	else if( poke_cnt > 1 )
	{
		u8 aff[ BTL_POSIDX_MAX ];
		u8 critical[ BTL_POSIDX_MAX ];
		u8 f_same_aff;
		PokeType waza_type;
		u32 i, j;

		f_same_aff = TRUE;
		waza_type = WAZADATA_GetType( waza );
		for(i=0; i<poke_cnt; ++i)
		{
			bpp = TargetPokeRec_Get( targets, i );
			aff[i] = scEvent_checkWazaDamageAffinity( wk, bpp, waza_type );
			for(j=0; j<i; ++j){
				if( BTL_CALC_TypeAffAbout(aff[i]) != BTL_CALC_TypeAffAbout(aff[j]) ){
					f_same_aff = FALSE;
				}
			}
		}
		// �������P�̂ł��ʂ̃��c������Ȃ�ʂ̏���
		if( !f_same_aff )
		{
			for(i=0; i<poke_cnt; ++i){
				bpp = TargetPokeRec_Get( targets, i );
				scproc_Fight_Damage_side_single( wk, attacker, bpp, waza, dmg_ratio );
			}
		}
		// ������v�Ȃ�A�̗̓o�[�𓯎��Ɍ��炷�悤�ɃR�}���h��������
		else
		{
			scproc_Fight_damage_side_plural( wk, attacker, targets, aff, waza, dmg_ratio );
		}
	}
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i��ʕ���j
//------------------------------------------------------------------
static void scproc_Fight_Damage_side_single( BTL_SVFLOW_WORK* wk,
			BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, fx32 targetDmgRatio )
{
	BtlTypeAff aff = scEvent_checkWazaDamageAffinity( wk, defender, wk->wazaParam.wazaType );

	svflowsub_damage_act_singular( wk, attacker, defender, waza, aff, targetDmgRatio );

	scPut_CheckDeadCmd( wk, defender );
	scPut_CheckDeadCmd( wk, attacker );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_singular(  BTL_SVFLOW_WORK* wk,
		BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
		WazaID waza,   BtlTypeAff aff,   fx32 targetDmgRatio )
{
	u8 plural_flag = FALSE;	// ������q�b�g�t���O
	u8 deadFlag = FALSE;
	u8 atPokeID, defPokeID;
	u8 hit_count, critical_flag, item_use_flag;
	u32 damage;
	int i;
	BTL_EVWK_DAMAGE_REACTION* evwk;

	evwk = eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_DAMAGE_REACTION) );
	plural_flag = scEvent_CheckPluralHit( wk, attacker, waza, &hit_count );
	BTL_Printf("�O�̂��ߕ\�� : �q�b�g��=%d\n", hit_count);

	atPokeID = BTL_POKEPARAM_GetID( attacker );
	defPokeID = BTL_POKEPARAM_GetID( defender );

	wazaEff_SetTarget( wk, defender );

	for(i=0; i<hit_count; ++i)
	{
		critical_flag = scEvent_CheckCritical( wk, attacker, defender, waza );
		damage = scEvent_CalcDamage( wk, attacker, defender, waza, aff, targetDmgRatio, critical_flag );

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

		scPut_WazaDamageSingle( wk, defender, aff, damage, critical_flag );
		scproc_WazaAdditionalEffect( wk, waza, attacker, defender, damage );

		item_use_flag = scEvent_WazaDamageReaction( wk, attacker, defender, waza, aff, damage, critical_flag, evwk );
		if( item_use_flag ){
			scproc_UseItem( wk, defender );
		}
		scproc_WazaDamageReaction( wk, evwk );

		scproc_Damage_Kickback( wk, attacker, waza, damage );

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

	TargetPokeRec_AddWithDamage( &wk->damagedPokemon, defender, damage );

	if( plural_flag )
	{
		// @@@ �u�����񂠂������I�v���b�Z�[�W
//			SCQUE_PUT_MSG_WazaHitCount( wk->que, i );	// @@@ ���Ƃ�
	}

	eventWork_Pop( &wk->eventWork, evwk );
}

static void scproc_Fight_damage_side_plural( BTL_SVFLOW_WORK* wk, 
		BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets, const u8* aff, WazaID waza, fx32 dmg_ratio )
{
	BTL_POKEPARAM *bpp[ BTL_POSIDX_MAX ];
	u16 dmg[ BTL_POSIDX_MAX ];
	u8  critical_flg[ BTL_POSIDX_MAX ];
	u8 i, poke_cnt;

	poke_cnt = TargetPokeRec_GetCount( targets );
	GF_ASSERT(poke_cnt < BTL_POSIDX_MAX);

	wazaEff_SetNoTarget( wk );

	for(i=0; i<poke_cnt; ++i)
	{
		bpp[i] = TargetPokeRec_Get( targets, i );
		critical_flg[i] = scEvent_CheckCritical( wk, attacker, bpp[i], waza );
		dmg[i] = scEvent_CalcDamage( wk, attacker, bpp[i], waza, aff[i], dmg_ratio, critical_flg[i] );
		TargetPokeRec_AddWithDamage( &wk->damagedPokemon, bpp[i], dmg[i] );
	}

	scPut_WazaDamagePlural( wk, poke_cnt, aff[0], bpp, dmg, critical_flg );

	for(i=0; i<poke_cnt; ++i)
	{
		scproc_WazaAdditionalEffect( wk, waza, attacker, bpp[i], dmg[i] );
	}

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
 * @param   waza				�g�p���U
 * @param   attacker		�U���|�P�����p�����[�^
 * @param   defender		�h��|�P�����p�����[�^
 * @param   damage			�^�����_���[�W
 *
 */
//--------------------------------------------------------------------------
static void scproc_WazaAdditionalEffect( BTL_SVFLOW_WORK* wk, WazaID waza,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
	WazaCategory category = WAZADATA_GetCategory( waza );

	if( category == WAZADATA_CATEGORY_DRAIN )
	{
		scproc_Fight_Drain( wk, waza, attacker, defender, damage );
	}
	else
	{
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
}


//--------------------------------------------------------------------------
/**
 * ���U�ɂ��_���[�W��̊e�����
 */
//--------------------------------------------------------------------------
static void scproc_WazaDamageReaction( BTL_SVFLOW_WORK* wk, BTL_EVWK_DAMAGE_REACTION* evwk )
{
	switch( evwk->reaction ){
	case BTL_EV_AFTER_DAMAGED_REACTION_SICK:				///< �Ώۃ|�P��������Ԉُ�ɂ���
		scproc_WazaDamageReaction_Sick( wk, evwk );
		break;
	case BTL_EV_AFTER_DAMAGED_REACTION_RANK:				///< �Ώۃ|�P�����̃����N��������
		scproc_WazaDamageReaction_Rank( wk, evwk );
		break;
	case BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE:			///< �Ώۃ|�P�����Ƀ_���[�W��^����
		scproc_WazaDamageReaction_Damage( wk, evwk );
		break;
	case BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE:		///< �Ώۃ|�P�����̃^�C�v�ύX
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

		add_succeed_flag = scproc_AddSick( wk, targetPoke, damagedPoke, evwk->sick, evwk->sickCont, FALSE );

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

	succeed = scproc_RankEffectCore( wk, target, evwk->rankType, evwk->rankVolume, TRUE );

	if( succeed ){
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
 * @param   fAlmost			
 *
 * @retval  BOOL				��Ԉُ�ɂł����ꍇ TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
	WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost )
{
	BOOL fItemUseTgt, fCarry, fSucceed;

	fSucceed = addsick_core( wk, target, attacker, sick, sickCont, fAlmost, &fCarry, &fItemUseTgt );
	if( fCarry )
	{
		BOOL fItemUseAtk;

		addsick_core( wk, attacker, target, sick, sickCont, TRUE, &fCarry, &fItemUseAtk );
		if( fItemUseAtk )
		{
			scproc_UseItem( wk, attacker );
		}
	}

	if( fItemUseTgt )
	{
		scproc_UseItem( wk, target );
	}

	return fSucceed;
}
//--------------------------------------------------------------------------
/**
 * ��Ԉُ�R�A����
 *
 * @param   wk					
 * @param   target						��Ԉُ�����炤�|�P
 * @param   sick							��Ԉُ�ID
 * @param   sickCont					��Ԉُ�p���p�����[�^
 * @param   fAlmost						�قڊm��t���O
 * @param   fCarry						[out]��������ꍇ�A����ɂ������ُ�������Ȃ�TRUE
 * @param   fItemResponce			[out]��������ꍇ�A�A�C�e���g�p����������Ȃ�TRUE
 *
 * @retval  BOOL		��Ԉُ�ɂ����ꍇ�ATRUE
 */
//--------------------------------------------------------------------------
static BOOL addsick_core( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker,
	WazaSick sick, BPP_SICK_CONT sickCont, BOOL fAlmost, BOOL* fCarry, BOOL* fItemResponce )
{
	BTL_EVWK_ADDSICK* evwk = eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_ADDSICK) );
	BOOL succeed = FALSE;

	scEvent_AddSick_Check( wk, target, sick, evwk );
	*fItemResponce = FALSE;
	*fCarry = FALSE;

	if( evwk->reaction != BTL_EV_SICK_REACTION_DISCARD )
	{
		scPut_AddSick( wk, target, sick, sickCont );
		*fItemResponce = evwk->fItemResponce;
		*fCarry = (evwk->reaction == BTL_EV_SICK_REACTION_REFRECT);
		succeed = TRUE;
	}
	else
	{
		BOOL fTokWin = (fAlmost && evwk->fDiscardByTokusei);
		if( fTokWin ){
			scPut_TokWin_In( wk, target );
		}

		scPut_AddSickFail( wk, target, evwk->discardSickType );

		if( fTokWin ){
			scPut_TokWin_Out( wk, target );
		}
	}

	eventWork_Pop( &wk->eventWork, evwk );
	return succeed;
}

/*
typedef struct {

	BtlEvSickReaction		reaction;
	WazaSick						discardSickType;
	u8									fDiscardByTokusei;
	u8									discardPokeID;
	u8									fItemResponce;

}BTL_EVWK_ADDSICK;
*/
static BOOL scEvent_AddSick_IsMustFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
	// �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
	if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
	&&  (sick == POKESICK_KOORI )
	){
		return TRUE;
	}
	// ���łɃ|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ�A�V���Ƀ|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
	if( BTL_POKEPARAM_GetPokeSick(target) != POKESICK_NULL )
	{
		return TRUE;
	}
	return FALSE;
}
static void scEvent_AddSick_Check( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
		WazaSick sick, BTL_EVWK_ADDSICK* evwk )
{
	BtlEventType event = (sick < POKESICK_MAX)? BTL_EVENT_MAKE_POKESICK : BTL_EVENT_MAKE_WAZASICK;

	evwk->reaction = BTL_EV_SICK_REACTION_NONE;
	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
		BTL_EVENT_CallHandlers( wk, event );
	BTL_EVENTVAR_Pop();
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�A�C�e���g�p
//------------------------------------------------------------------
static void scproc_UseItem( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
	// @@@ ������
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏���
//------------------------------------------------------------------
static void scproc_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
	scput_Fight_Damage_After_Shrink( wk, waza, attacker, targets );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏��� > �Ђ�݃`�F�b�N
//------------------------------------------------------------------
static void scput_Fight_Damage_After_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
{
	u32 per = WAZADATA_GetShrinkPer( waza );
	if( per )
	{
		BTL_POKEPARAM* bpp;
		u32 cnt, i=0;

		while( (bpp = TargetPokeRec_Get(targets, i++)) != NULL )
		{
			if( !BTL_POKEPARAM_GetTurnFlag(bpp, BPP_TURNFLG_ACTION_EXE) )
			{
				scEvent_AddShrink( wk, bpp, attacker, per );
			}
		}
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
		wazaPower = scEvent_getWazaPower( wk, attacker, defender, waza );
		atkPower  = scEvent_getAttackPower( wk, attacker, defender, waza, criticalFlag );
		defGuard  = scEvent_getDefenderGuard( wk, attacker, defender, waza, criticalFlag );
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
	if(	(WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC)
	&&	(BTL_POKEPARAM_GetPokeSick(attacker) == POKESICK_YAKEDO)
	){
		rawDamage = (rawDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
	}

	BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, rawDamage );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC2 );
	rawDamage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
	// �Œ�A�P�̓_���[�W��^����
	if( rawDamage == 0 ){ rawDamage = 1; }

	BTL_EVENTVAR_Pop();
	return rawDamage;
}
//---------------------------------------------------------------------------------------------
// ���U�_���[�W������̔�������
//---------------------------------------------------------------------------------------------
static void scproc_Damage_Kickback( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza, u32 wazaDamage )
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
			scPut_Messae_Set( wk, bpp, strID );
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
//--------------------------------------------------------------------------
/**
 * [Event] ���U�ȊO�̃_���[�W�L���^�����`�F�b�N
 *
 * @param   wk		
 * @param   bpp		
 * @param   damage	�_���[�W��
 *
 * @retval  BOOL		�L���Ȃ�TRUE
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
 * @retval  BOOL		�A�C�e���g�p����ꍇ�ATRUE
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


//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ǉ����ʂɂ���Ԉُ�
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Damage_AddSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target )
{
	WAZA_SICKCONT_PARAM  waza_contParam;
	WazaSick sick;
	u32 i=0;

	sick = WAZADATA_GetSick( waza, &waza_contParam );

	if(	(!BTL_POKEPARAM_IsDead(target))
	&&	(!scEvent_AddSick_IsMustFail(wk, target, sick))
	){
		if( scEvent_CheckAddSickPer(wk, waza, attacker, target) )
		{
			BPP_SICK_CONT sickCont;
			BTL_CALC_WazaSickContToBppSickCont( waza_contParam, attacker, &sickCont );
			scproc_AddSick( wk, target, attacker, sick, sickCont, FALSE );
		}
	}
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̏�Ԉُ�
//---------------------------------------------------------------------------------------------
static void scproc_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
	BTL_POKEPARAM* target;
	WAZA_SICKCONT_PARAM waza_contParam;
	WazaSick sick;
	u32 i = 0;

	sick = WAZADATA_GetSick( waza, &waza_contParam );

	while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
	{
		if( scEvent_AddSick_IsMustFail(wk, target, sick) ){
			TargetPokeRec_Remove( targetRec, target );
		}
	}

	if( TargetPokeRec_GetCount(targetRec) )
	{
		TargetPokeRec_GetStart( targetRec );
		while( (target = TargetPokeRec_GetNext(targetRec)) != NULL )
		{
			if( scEvent_checkHit(wk, attacker, target, waza) )
			{
				BPP_SICK_CONT sickCont;
				BTL_CALC_WazaSickContToBppSickCont( waza_contParam, attacker, &sickCont );
				if( scproc_AddSick( wk, target, attacker, sick, sickCont, TRUE ) )
				{
					wazaEff_SetTarget( wk, target );
				}
				else
				{
					// @@@ ���ʂ��Ȃ����R������˂΂Ȃ��c
				}
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
 * @retval  BOOL		�����N�������ʂ�����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_WazaRankEffect_Common( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* target, BOOL fAlmost )
{
	BOOL ret = FALSE;
	u32 eff_cnt, i;

	eff_cnt = WAZADATA_GetRankEffectCount( waza );
	for(i=0; i<eff_cnt; ++i)
	{
		WazaRankEffect  effect;
		int  volume;

		scEvent_GetWazaRankEffectValue( wk, waza, i, attacker, target, &effect, &volume );
		if( scproc_RankEffectCore(wk, target, effect, volume, fAlmost) )
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
 * @param   fAlmost		
 *
 * @retval  BOOL		�����N�������ʂ�����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scproc_RankEffectCore( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume, BOOL fAlmost )
{
	if( !BTL_POKEPARAM_IsRankEffectValid(target, effect, volume) )
	{
		if( fAlmost )
		{
			scPut_RankEffectLimit( wk, target, effect, volume );
			return FALSE;
		}
	}

	{
		BTL_EVWK_CHECK_RANKEFF* evwk = (BTL_EVWK_CHECK_RANKEFF*)eventWork_Push( &wk->eventWork, sizeof(BTL_EVWK_CHECK_RANKEFF) );
		BOOL ret = TRUE;

		if( scEvent_CheckRankEffectSuccess(wk, target, effect, volume, evwk) )
		{
			BTL_Printf("�����N���ʁFtype=%d, volume=%d\n", effect, volume );
			scPut_RankEffect( wk, target, effect, volume );
		}
		else
		{
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
// �T�[�o�[�t���[�F���U�_���[�W�ɉ�����HP�z��������
//---------------------------------------------------------------------------------------------
static void scproc_Fight_Drain( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, u32 damage )
{
	BTL_POKEPARAM* target;
	int value;
	u32 i;
	u8 atkPokeID;

	atkPokeID = BTL_POKEPARAM_GetID( attacker );

	value = scEvent_CalcDrainVolume( wk, waza, attacker, defender, damage );
	if( value > 0 )
	{
		scPut_SimpleHp( wk, attacker, value );
	}
	else
	{
		scproc_SimpleDamage( wk, attacker, -value, STRID_NULL );
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
 * @retval  BOOL	�K�����s����P�[�X��TRUE
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
 * @retval  BOOL			���s�Ȃ�TRUE
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

	if( scEvent_ChangeWeather( wk, weather, BTL_WEATHER_TURN_DEFAULT ) )
	{
		scPut_WazaEffectOn( wk, attacker, attacker, waza );
		SCQUE_PUT_ACT_WeatherStart( wk->que, weather );
	}
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
		SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
		break;
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


//==============================================================================
// �T�[�o�[�t���[�F�^�[���`�F�b�N���[�g
//==============================================================================
static void scput_TurnCheck( BTL_SVFLOW_WORK* wk )
{
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_TURNCHECK_BEGIN );

	scput_turncheck_sick( wk );
	scput_turncheck_weather( wk );

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_TURNCHECK_END );

	// �S�|�P�����̃^�[���t���O���N���A
	{
		SVCL_WORK* cw;
		u32 i, j;
		for(i=0; i<BTL_CLIENT_MAX; ++i)
		{
			cw = BTL_SERVER_GetClientWorkIfEnable( wk->server, i );
			if( cw == NULL ){ continue; }
			for(j=0; j<cw->memberCount; ++j)
			{
				BTL_POKEPARAM_ClearTurnFlag( cw->member[j] );
			}
		}
	}

	if( wk->turnCount < BTL_TURNCOUNT_MAX ){
		wk->turnCount++;
	}
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > ��Ԉُ�
//------------------------------------------------------------------
static void scput_turncheck_sick( BTL_SVFLOW_WORK* wk )
{
	FRONT_POKE_SEEK_WORK  fpsw;
	BTL_POKEPARAM* bpp;
	WazaSick sick;
	int damage;
	u8 pokeID;

	BTL_EVENTVAR_Push();

	FRONT_POKE_SEEK_InitWork( &fpsw, wk );

	while( FRONT_POKE_SEEK_GetNext( &fpsw, wk, &bpp ) )
	{
		pokeID = BTL_POKEPARAM_GetID( bpp );
		sick = BTL_POKEPARAM_GetPokeSick( bpp );
		damage = BTL_POKEPARAM_CalcSickDamage( bpp );
		if( sick != POKESICK_NULL && (damage!=0) )
		{
			BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
			BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, damage );
			BTL_EVENT_CallHandlers( wk, BTL_EVENT_SICK_DAMAGE );
			damage = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
			sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
			if( sick != POKESICK_NULL )
			{
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

	BTL_EVENTVAR_Pop();
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > �V��
//------------------------------------------------------------------
static void scput_turncheck_weather( BTL_SVFLOW_WORK* wk )
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
	BTL_Printf( "�V��_���[�W�`�F�b�N����\n");
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
//	���l�֘A�֐��Q
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
//	�C�x���g���[�N�m��
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
			GF_ASSERT(0);	// �Ō�Ɋm�ۂ��ꂽ���[�N�ȊO�̗̈悪��ɉ������悤�Ƃ��Ă���
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
	SCQUE_PUT_ACT_ConfDamage( wk->que, pokeID );

	scPut_CheckDeadCmd( wk, bpp );
}
//--------------------------------------------------------------------------
/**
 * ���U�o�����莞�̃|�P�����n��Ԉُ�񕜏���
 */
//--------------------------------------------------------------------------
static void scPut_CurePokeSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp )
{
	u8 pokeID = BTL_POKEPARAM_GetID( bpp );
	PokeSick sick = BTL_POKEPARAM_GetPokeSick( bpp );

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
		GF_ASSERT_MSG(0, "illegal sick ID=%d\n", sick);
	}
}
//--------------------------------------------------------------------------
/**
 * ���U�o�����s���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------
static void scPut_WazaExecuteFail( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaID waza, SV_WazaFailCause cause )
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
	default:
		SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail, pokeID );
		break;
	}
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
		SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_SET_NoEffect, pokeID );
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
	if( !BTL_POKEPARAM_GetContFlag(poke, BPP_CONTFLG_DEAD_IGNORE) )
	{
		if( BTL_POKEPARAM_IsDead(poke) )
		{
			u8 pokeID = BTL_POKEPARAM_GetID( poke );

			BTL_HANDLER_TOKUSEI_Remove( poke );

			BTL_POKEPARAM_SetContFlag( poke, BPP_CONTFLG_DEAD_IGNORE );
			BTL_POKEPARAM_SetTurnFlag( poke, BPP_TURNFLG_DEAD );

			SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, pokeID );
			SCQUE_PUT_ACT_Dead( wk->que, pokeID );
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
static void scPut_WazaDamageSingle( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* defender, BtlTypeAff aff, u32 damage, BOOL critical_flag )
{
	u8 pokeID = BTL_POKEPARAM_GetID( defender );

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
//--------------------------------------------------------------------------
/**
 * [Put] ���U�ɂ��_���[�W��^������̔����v�Z
 */
//--------------------------------------------------------------------------
static u32 scEvent_CalcKickBack( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, WazaID waza, u32 damage )
{
	u8 ratio = WAZADATA_GetReactionRatio( waza );
	u8 pokeID = BTL_POKEPARAM_GetID( attacker );

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_RATIO, ratio );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, pokeID );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_REACTION );
		ratio = BTL_EVENTVAR_GetValue( BTL_EVAR_RATIO );
	BTL_EVENTVAR_Pop();

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
static void scPut_RankEffect( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaRankEffect effect, int volume )
{
	u8 pokeID = BTL_POKEPARAM_GetID( target );

	if( volume > 0 )
	{
		volume = BTL_POKEPARAM_RankUp( target, effect, volume );
		SCQUE_PUT_OP_RankUp( wk->que, pokeID, effect, volume );
		SCQUE_PUT_ACT_RankUp( wk->que, pokeID, effect, volume );
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankup_ATK, pokeID, effect, volume );
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
		case WAZA_RANKEFF_ATTACK:	strID = BTL_STRID_SET_RankdownFail_ATK; break;
		case WAZA_RANKEFF_HIT:		strID = BTL_STRID_SET_RankdownFail_HIT; break;
		}
	}
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
 * [Put] ��Ԉُ퉻
 */
//--------------------------------------------------------------------------
static void scPut_AddSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, WazaSick sick, BPP_SICK_CONT sickCont )
{
	u8 pokeID = BTL_POKEPARAM_GetID( target );

	BTL_POKEPARAM_SetWazaSick( target, sick, sickCont );
	SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, sickCont.raw );
	SCQUE_PUT_ACT_SickSet( wk->que, pokeID, sick );
}
static void scPut_AddSickFail( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target, WazaSick sick )
{
	u8 pokeID = BTL_POKEPARAM_GetID( target );
	SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NoEffect, pokeID );
}
//--------------------------------------------------------------------------
/**
 * [Put] �Z�b�g���b�Z�[�W�\��
 */
//--------------------------------------------------------------------------
static void scPut_Messae_Set( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, u16 strID )
{
	SCQUE_PUT_MSG_SET( wk->que, strID, BTL_POKEPARAM_GetID(bpp) );
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
	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_AGILITY, agi );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, TRUE );	// �܂Ж����t���O�Ƃ��Ďg���Ă���
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
	return agi;
}
//--------------------------------------------------------------------------
/**
 * [Event] �����񎩔�����
 *
 * @param   wk		
 * @param   attacker		
 *
 * @retval  BOOL		����������TRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckConf( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
	if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KONRAN) )
	{
		return BTL_CALC_IsOccurPer( BTL_CONF_EXE_RATIO );
	}
	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * [Event] ������_���[�W�v�Z
 *
 * @param   wk		
 * @param   attacker		
 *
 * @retval  u16		�_���[�W�l
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
 * @param   attacker		�U���|�P�����p�����[�^
 * @param   waza				�o�����Ƃ��Ă��郏�U
 * @param   curedFlag		[out] �˂ނ�E�܂ЁE������ ���񕜂����ꍇTRUE�^����ȊOFALSE������
 *
 * @retval  SV_WazaFailCause		
 */
//--------------------------------------------------------------------------
static SV_WazaFailCause scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker,
	WazaID waza, u8* curedFlag )
{
	*curedFlag = FALSE;

	BTL_EVENTVAR_Push();

	BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NULL );
	BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
	do {
		WazaSick sick = BTL_POKEPARAM_GetPokeSick( attacker );
		u8 pokeID = BTL_POKEPARAM_GetID( attacker );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );

		// ��Ԉُ�ɂ�鎸�s�`�F�b�N
		switch( sick ){
		case POKESICK_NEMURI:
			if( !BTL_POKEPARAM_Nemuri_CheckWake(attacker) ){
				BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_NEMURI );
			}else{
				*curedFlag = TRUE;
			}
			break;
		case POKESICK_MAHI:
			if( GFL_STD_MtRand(100) < BTL_MAHI_EXE_RATIO )
			{
				BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_MAHI );
			}
			break;
		case POKESICK_KOORI:
			if( GFL_STD_MtRand(100) >= BTL_KORI_MELT_RATIO ){
				BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_KOORI );
			}else{
				*curedFlag = TRUE;
			}
			break;
		}
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_CAUSE) != SV_WAZAFAIL_NULL )
		{
			break;
		}
		// �Ђ�݂ɂ�鎸�s�`�F�b�N
		if( BTL_POKEPARAM_GetTurnFlag(attacker, BPP_TURNFLG_SHRINK) )
		{
			BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_CAUSE, SV_WAZAFAIL_SHRINK );
			break;
		}

	}while(0);

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE );
	{
		SV_WazaFailCause cause = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_CAUSE );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE_FIX );
		BTL_EVENTVAR_Pop();
		return cause;
	}
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
	if( WAZADATA_IsAlwaysHit(waza) )
	{
		return TRUE;
	}
	if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_MUSTHIT) )
	{
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
			if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_MUSTHIT) ){
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
 * @retval  BOOL		�����ȏꍇTRUE
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
 * @retval  BOOL		�����ȏꍇTRUE
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
 * @param   evwk					[out] �����ڍ׏��
 *
 * @retval  BOOL		�A�C�e�������t���O�iTRUE�Ȃ�A�C�e�����g���j
 */
//--------------------------------------------------------------------------
static BOOL scEvent_WazaDamageReaction( BTL_SVFLOW_WORK* wk,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff aff,
	u32 damage, BOOL criticalFlag, BTL_EVWK_DAMAGE_REACTION* evwk )
{
	u8 defPokeID = BTL_POKEPARAM_GetID( defender );
	u8 item_use_flag = FALSE;

	evwk->reaction = BTL_EV_AFTER_DAMAGED_REACTION_NONE;
	evwk->damagedPokeID = defPokeID;

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defPokeID );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_TYPE, wk->wazaParam.wazaType );
		BTL_EVENTVAR_SetValue( BTL_EVAR_CRITICAL_FLAG, criticalFlag );
		BTL_EVENTVAR_SetValue( BTL_EVAR_ITEMUSE_FLAG, item_use_flag );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
		item_use_flag = BTL_EVENTVAR_GetValue( BTL_EVAR_ITEMUSE_FLAG );
	BTL_EVENTVAR_Pop();

	return item_use_flag;
}
//--------------------------------------------------------------------------
/**
 * [Event] �Q�̓������U�_���[�W�A�N�V��������
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------
static void scEvent_WazaDamageEffect_Double( BTL_SVFLOW_WORK* wk,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2, WazaID waza, BtlTypeAff aff,
	u32 dmg1, u32 dmg2 )
{
	u8 defPokeID1 = BTL_POKEPARAM_GetID( def1 );
	u8 defPokeID2 = BTL_POKEPARAM_GetID( def2 );

	SCQUE_PUT_ACT_WazaDamageDbl( wk->que, defPokeID1, defPokeID2, dmg1, dmg2, aff );

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
		BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );

		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defPokeID1 );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defPokeID2 );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );

	BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] ���U�̑����v�Z
 *
 * @param   wk		
 * @param   defender		
 * @param   waza_type		
 *
 * @retval  BtlTypeAff		
 */
//--------------------------------------------------------------------------
static BtlTypeAff scEvent_checkWazaDamageAffinity( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* defender, PokeType waza_type )
{
	PokeTypePair defPokeType = scEvent_getDefenderPokeType( wk, defender );
	return BTL_CALC_TypeAff( waza_type, defPokeType );;
}
//--------------------------------------------------------------------------
/**
 * [Event] �g�������U��PP�f�N�������g
 *
 * @param   wk				
 * @param   attacker	�U���|�P�����p�����[�^
 * @param   wazaIdx		�g�������U�C���f�b�N�X
 * @param   rec				�_���[�W���󂯂��|�P�����p�����[�^�Q
 *
 */
//--------------------------------------------------------------------------
static void scEvent_decrementPP( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, u8 wazaIdx, TARGET_POKE_REC* rec )
{
	BTL_EVENTVAR_Push();

	BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, 1 );
	{
		u8 i = 0;
		BTL_POKEPARAM* bpp;
		BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGED_POKECNT, TargetPokeRec_GetCount(rec) );
		while( (bpp=TargetPokeRec_Get(rec, i)) != NULL )
		{
			BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DAMAGED1+i, BTL_POKEPARAM_GetID(bpp) );
			++i;
		}
	}
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP );
	{
		u8 volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
		if( volume )
		{
			u8 pokeID = BTL_POKEPARAM_GetID( attacker );
			SCQUE_PUT_OP_PPMinus( wk->que, pokeID, wazaIdx, volume );
			BTL_POKEPARAM_PPMinus( attacker, wazaIdx, volume );
		}
	}
	BTL_EVENTVAR_Pop();
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
 * @retval  u8		�I���� (0-100)
 */
//--------------------------------------------------------------------------
static u8 scEvent_getHitPer( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	u8 per = WAZADATA_GetHitRatio( waza );

	BTL_EVENTVAR_Push();
		if( BTL_FIELD_GetWeather() == BTL_WEATHER_MIST )
		{
			per = (per * BTL_CALC_WEATHER_MIST_HITRATIO) >> FX32_SHIFT;
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
 * [Event] ������q�b�g���U�̃`�F�b�N����
 *
 * @param   wk					
 * @param   attacker		
 * @param   waza		
 * @param   hitCount		[out] ���s����q�b�g�񐔁i�����_���j
 *
 * @retval  BOOL		������q�b�g���郏�U�Ȃ�TRUE
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

// ���U�З͎擾
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	u16 power = WAZADATA_GetPower( waza );

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, power );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER );
		power = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
	BTL_EVENTVAR_Pop();

	return power;
}


// �U�����\�͒l�擾
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk,
	const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag )
{
	BppValueID vid;

	switch( WAZADATA_GetDamageType(waza) ){
	case WAZADATA_DMG_PHYSIC:		vid = BPP_ATTACK; break;
	case WAZADATA_DMG_SPECIAL:	vid = BPP_SP_ATTACK; break;
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
			BTL_EVENTVAR_SetValue( BTL_EVAR_GEN_FLAG, FALSE );		// 
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
	const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BOOL criticalFlag )
{
	BppValueID vid;

	switch( WAZADATA_GetDamageType(waza) ){
	case WAZADATA_DMG_PHYSIC:		vid = BPP_DEFENCE; break;
	case WAZADATA_DMG_SPECIAL:	vid = BPP_SP_DEFENCE; break;
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
			if(	(BTL_FIELD_GetWeather() == BTL_WEATHER_SAND)
			&&	(BTL_POKEPARAM_IsMatchType(defender, POKETYPE_IWA))
			&&	(vid == BPP_SP_DEFENCE)
			){
				guard = (guard * 192) / 128;
			}

			BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );
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
 * @retval  fx32		�␳��
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


//--------------------------------------------------------------------------
/**
 * [Event] ���U�ǉ����ʂɂ��Ώۃ|�P�������Ђ�܂���
 *
 * @param   wk					
 * @param   target			�Ώۃ|�P�����f�[�^
 * @param   attacker		�U�������|�P�����f�[�^
 * @param   per					�Ђ�݊m��
 *
 */
//--------------------------------------------------------------------------
static void scEvent_AddShrink( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, BTL_POKEPARAM* attacker, u8 per )
{
	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_CHECK );
		per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
	BTL_EVENTVAR_Pop();

	if( perOccur(per) )
	{
		BTL_POKEPARAM_SetTurnFlag( target, BPP_TURNFLG_SHRINK );
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
 *
 * @retval  BOOL		��������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckAddSickPer( BTL_SVFLOW_WORK* wk, WazaID waza, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender )
{
	WAZA_SICKCONT_PARAM  waza_contParam;
	WazaSick sick = WAZADATA_GetSick( waza, &waza_contParam );
	u8 per = WAZADATA_GetSickPer( waza );

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(defender) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_SICK );
		per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
	BTL_EVENTVAR_Pop();

	return  perOccur(per);
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z����|�P�������|�P�����n�̏�Ԉُ�ɂ���
 *
 * @param   wk					
 * @param   receiver		��Ԉُ���󂯂�Ώۂ̃|�P�����p�����[�^
 * @param   attacker		��Ԉُ��^���鑤�̃|�P�����p�����[�^
 * @param   sick				��Ԉُ�ID
 * @param   fAlmost			�قڊm��t���O�i���j
 *
 * ���قڊm��t���O�Ƃ�
 *	�v���C���[���قڊm���ɏ�Ԉُ�ɂȂ�͂��A�Ǝv���󋵂�TRUE�ɂ���B
 *  ���U���ڌ��ʂȂ���TRUE�B�ǉ����ʂȂ���ʊm��100�̎���TRUE�B�Ƃ��������ʂ̏ꍇ�͏��FALSE�B
 *	���ꂪTRUE�̏ꍇ�A��Ԉُ�𖳌�������Ƃ����������������ꍇ�ɂ��̂��Ƃ�\�����邽�߂Ɏg���B
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
 * @param   receiver		��Ԉُ���󂯂�Ώۂ̃|�P�����p�����[�^
 * @param   attacker		��Ԉُ��^���鑤�̃|�P�����p�����[�^
 * @param   sick				��Ԉُ�ID
 * @param   fAlmost			�قڊm��t���O�i���j
 *
 * ���قڊm��t���O�Ƃ�
 *	�v���C���[���قڊm���ɏ�Ԉُ�ɂȂ�͂��A�Ǝv���󋵂�TRUE�ɂ���B
 *  ���U���ڌ��ʂȂ���TRUE�B�ǉ����ʂȂ���ʊm��100�̎���TRUE�B�Ƃ��������ʂ̏ꍇ�͏��FALSE�B
 *	���ꂪTRUE�̏ꍇ�A��Ԉُ�𖳌�������Ƃ����������������ꍇ�ɂ��̂��Ƃ�\�����邽�߂Ɏg���B
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
//			SCQUE_PUT_ACT_SickSet( wk->que, pokeID, sick );
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
 * @retval  BOOL		����������TRUE
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
 * @param   effect		[out] ���ʎ��
 * @param   volume		[out] ���ʒl
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
 * @param   evwk			[out]
 *
 * @retval  BOOL			��������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_CheckRankEffectSuccess( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* target,
	WazaRankEffect effect, int volume, BTL_EVWK_CHECK_RANKEFF* evwk )
{
	evwk->failFlag = FALSE;
	evwk->failTokuseiFlag = FALSE;
	evwk->failSpecificType = WAZA_RANKEFF_NULL;

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(target) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, effect );
		BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
		BTL_EVENTVAR_SetValue( BTL_EVAR_WORK_ADRS, (int)evwk );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_GET_RANKEFF_VALUE );
	BTL_EVENTVAR_Pop();

	return evwk->failFlag == FALSE;
}//--------------------------------------------------------------------------
/**
 * [Event] �h���C���U���̃h���C��HP�ʂ��v�Z
 *
 * @param   wk		
 * @param   waza		
 * @param   attacker		
 * @param   target		
 * @param   damage		�^�����_���[�W
 *
 * @retval  int			�񕜗ʁi�}�C�i�X�Ȃ�t�Ƀ_���[�W���󂯂�j
 */
//--------------------------------------------------------------------------
static int scEvent_CalcDrainVolume( BTL_SVFLOW_WORK* wk, WazaID waza,
	const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* target, int damage )
{
	int volume = damage * WAZADATA_GetDrainRatio(waza) / 100;

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_ATK, BTL_POKEPARAM_GetID(attacker) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, BTL_POKEPARAM_GetID(target) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_CALC_DRAIN_VOLUME );
		volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
	BTL_EVENTVAR_Pop();
	return volume;
}
//--------------------------------------------------------------------------
/**
 * �yEvent�z�V��̕ω�
 *
 * @param   wk			
 * @param   weather	�V��
 * @param   turn		�p���^�[����
 *
 * @retval  BOOL		�ω������ꍇTRUE
 */
//--------------------------------------------------------------------------
static BOOL scEvent_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather, u8 turn )
{
	GF_ASSERT(weather != BTL_WEATHER_NONE);

	BTL_Printf("�V��ω��C�x���g���� ... �V��->%d\n", weather);

	BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_WEATHER, weather );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE );
		weather = BTL_EVENTVAR_GetValue( BTL_EVAR_WEATHER );
		if( weather != BTL_WEATHER_NONE )
		{
			BTL_Printf("   �V�� -> %d �֕ω�\n", weather);
			BTL_FIELD_SetWeather( weather, turn );
			BTL_EVENT_CallHandlers( wk, BTL_EVENT_WEATHER_CHANGE_AFTER );
		}
		else
		{
			BTL_Printf("   �V��ω� �ł������I\n");
		}
	BTL_EVENTVAR_Pop();
	return (weather != BTL_WEATHER_NONE);
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
 * @retval  BtlPokePos		�o�Ă���ꍇ�͐퓬�ʒuID�^�o�Ă��Ȃ��ꍇ��BTL_POS_MAX
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
				&&	(BTL_POKEPARAM_GetID(clwk->frontMember[p]) == pokeID)
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
 * @retval  u8		��ɏo�Ă���|�P������
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
 * @retval  u8		��ɏo�Ă���|�P������
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
 * @retval  BOOL		�o�Ă�����TRUE
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
 * [�n���h����M] �X�e�[�^�X�̃����N�_�E������
 *
 * @param   wk			
 * @param   exPos					�Ώۃ|�P�����ʒu
 * @param   statusType		�X�e�[�^�X�^�C�v
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
			scproc_RankEffectCore( wk, pp, statusType, volume, fAlmost );
		}
	}
}
//=============================================================================================
/**
 * [�n���h����M] �X�e�[�^�X�̃����N�A�b�v����
 *
 * @param   wk			
 * @param   exPos					�Ώۃ|�P�����ʒu
 * @param   statusType		�X�e�[�^�X�^�C�v
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
			scproc_RankEffectCore( wk, pp, statusType, volume, fAlmost );
		}
	}
}
//=============================================================================================
/**
 * [�n���h����M] �V���v��HP��������
 *
 * @param   wk						
 * @param   					�Ώۃ|�P�����ʒu
 * @param   statusType		�X�e�[�^�X�^�C�v
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
 * @param   targetPokeID	���炤����̃|�P����ID
 * @param   attackPokeID	����킹�鑤�́i�Ƃ����������Ȃǁj�|�P����ID
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
 * @param   weather		�ω���̓V��w��^�V����t���b�g�ɂ������ꍇ�� BTL_WEATHER_NONE
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_ChangeWeather( BTL_SVFLOW_WORK* wk, BtlWeather weather )
{
	if( weather != BTL_WEATHER_NONE )
	{
		if( scEvent_ChangeWeather( wk, weather, BTL_WEATHER_TURN_PERMANENT ) )
		{
			SCQUE_PUT_ACT_WeatherStart( wk->que, weather );
		}
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
 * [�n���h����M] �|�P�����̃^�C�v�ω��i�����^�C�v�̂݁j
 *
 * @param   wk			
 * @param   pokeID	
 * @param   type		
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_ChangePokeType( BTL_SVFLOW_WORK* wk, u8 pokeID, PokeType type )
{
	BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
	SCQUE_PUT_OP_ChangePokeType( wk->que, pokeID, type );
	BTL_POKEPARAM_ChangePokeType( bpp, PokeTypePair_MakePure(type) );
}
//=============================================================================================
/**
 * [�n���h����M] �|�P�����̃t�H�����ω�
 *
 * @param   wk			
 * @param   pokeID	
 * @param   formNo	
 *
 */
//=============================================================================================
void BTL_SVFLOW_RECEPT_ChangePokeForm( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 formNo )
{
	BTL_POKEPARAM* bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
	SCQUE_PUT_OP_ChangePokeForm( wk->que, pokeID, formNo );
	BTL_POKEPARAM_ChangeForm( bpp, formNo );
}
//=============================================================================================
/**
 * [�n���h����M] ���������֎~�R�[�h�̒ǉ���S�N���C�A���g�ɒʒm
 *
 * @param   wk						
 * @param   clientID			�֎~�R�[�h�𔭍s�����|�P����ID
 * @param   code					�֎~�R�[�h
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
 * @param   clientID			�֎~�R�[�h�𔭍s�����|�P����ID
 * @param   code					�֎~�R�[�h
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
 * @param   pokeID				�g���[�X�g�����|�P����ID
 * @param   targetPokeID	�g���[�X����鑤�|�P����ID
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

