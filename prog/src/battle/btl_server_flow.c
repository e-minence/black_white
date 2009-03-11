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
#include "waza_tool/wazadata.h"


#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_server_local.h"
#include "btl_server.h"

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

}FlowFlag;



typedef struct {

	WazaID	waza;						///< �o�������UID

	u8		attackPokeID;				///< �U�����|�PID
	u8		defencePokeID;			///< �󂯑��|�PID�i�P�́j
	BtlExPos	defencePokePos;	///< �󂯑��|�P�ʒuID�i�͈͑I���j

	u8		wazaExecuteFlag;	///< ���U���s�ł������t���O
	u8		wazaHitRatio;			///< ���U�����m��
//	u8		attackHitRank;
//	u8		defenceHitRank;
	u8		wazaPower;				///< ���U�З�
	u8		wazaDamageType;		///< �_���[�W�^�C�v�i�����E����j
	u8		criticalRank;			///< �N���e�B�J����
	u8		criticalFlag;			///< �N���e�B�J���t���O
	u8		wazaPokeType;			///< ���U�̃|�P�^�C�v
	u8		typeAff;					///< ���U�Ǝ󂯑��|�P�̑���
	PokeTypePair	attackerPokeType;	///< �U�����|�P�^�C�v
	PokeTypePair	defenderPokeType;	///< �󂯑��|�P�^�C�v
	u16		attackerPower;		///< �U�����̔\�͒l�i��������or�Ƃ������j
	u16		defenderGuard;		///< �h�䑤�̔\�͒l�i�ڂ�����or�Ƃ��ڂ��j
	u16		damageDenom;			///< �_���[�W�v�Z�p�̕ꐔ
	u32		rawDamage;				///< �^�C�v�v�Z�O�̑f�_���[�W
	u32		realDamage;				///< �^�C�v�v�Z��̎��_���[�W
	u32		refrectDamage;		///< �U�����̔����_���[�W
	fx32	typeMatchRatio;		///< �U�����|�P�ƃ��U�̃^�C�v��v�ɂ��_���[�W�{��(1.0 or 1.5�j

	u8		wazaFailReason;		///< ���U���s���R
	u8		confDamage;				///< �����񎩖Ń_���[�W

	u8		hitCountMax;			///< �ő�q�b�g��
	u8		hitCountReal;			///< ���ۃq�b�g��

	u8		decPP;						///< ����PP�l

	PokeSick	sick;					///< ��Ԉُ�
	int				sickDamage;		///< ��Ԉُ�_���[�W�l
	WazaSick	add_sick;
	u8				add_sick_turn;

}FIGHT_EVENT_PARAM;

typedef struct {

	BTL_POKEPARAM*  inPokeParam;
	BTL_POKEPARAM*  outPokeParam;

}CHANGE_EVENT_PARAM;


/**
 *	�A�N�V�����D�揇�L�^�\����
 */
typedef struct {
	u8  clientID;		///< �N���C�A���gID
	u8  pokeIdx;		///< ���̃N���C�A���g�́A���̖ځH 0�`
}ACTION_ORDER_WORK;


// �P�x�̃��U�����Ń_���[�W���󂯂��|�P�����̏����L�����Ă���
typedef struct {

	BTL_POKEPARAM*	pp[ BTL_POS_MAX ];
	u8							count;

}TARGET_POKE_REC;

struct _BTL_SVFLOW_WORK {

	BTL_SERVER*             server;
	const BTL_MAIN_MODULE*  mainModule;
	BTL_POKE_CONTAINER*			pokeCon;
	BTL_SERVER_CMD_QUE*			que;
	u8                      numClient;

	ACTION_ORDER_WORK		actOrder[ BTL_POS_MAX ];
	TARGET_POKE_REC			damagedPokemon;
	TARGET_POKE_REC			targetPokemon;

	FIGHT_EVENT_PARAM		fightEventParams;
	CHANGE_EVENT_PARAM	changeEventParams;

	u8					flowFlags[ FLOWFLG_BYTE_MAX ];

};

//--------------------------------------------------------------
/**
 *	�퓬�ɏo�Ă���|�P�����f�[�^�ɏ��ԂɃA�N�Z�X���鏈���̃��[�N
 */
//--------------------------------------------------------------
typedef struct {
	u8 clientIdx;
	u8 pokeIdx;
	u8 endFlag;
}FRONT_POKE_SEEK_WORK;


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static u8 sortClientAction( BTL_SVFLOW_WORK* wk, ACTION_ORDER_WORK* order );
static u8 countAlivePokemon( BTL_SVFLOW_WORK* wk );
static inline void FlowFlg_Set( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_Clear( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline BOOL FlowFlg_Get( BTL_SVFLOW_WORK* wk, FlowFlag flg );
static inline void FlowFlg_ClearAll( BTL_SVFLOW_WORK* wk );
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk );
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SVFLOW_WORK* wk, BTL_POKEPARAM** bpp );
static inline void TargetPokeRec_Clear( TARGET_POKE_REC* rec );
static inline void TargetPokeRec_Add( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp );
static inline u32 TargetPokeRec_GetCount( const TARGET_POKE_REC* rec );
static inline BTL_POKEPARAM* TargetPokeRec_Get( TARGET_POKE_REC* rec, u32 idx );
static inline int TargetPokeRec_SeekFriendIdx( const TARGET_POKE_REC* rec, const BTL_POKEPARAM* pp, u32 start_idx );
static void scput_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scput_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx );
static void scput_Fight( BTL_SVFLOW_WORK* wk, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action );
static void cof_put_wazafail_msg_cmd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaFailReason );
static u8 svflowsub_register_waza_targets( BTL_SVFLOW_WORK* wk, BtlPokePos atPos, WazaID waza, const BTL_ACTION_PARAM* action, TARGET_POKE_REC* rec );
static void scput_Fight_Damage( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza, SVCL_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action );
static BTL_POKEPARAM* svflowsub_get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* svflowsub_get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
static void svflowsub_check_and_make_dead_command( BTL_SVFLOW_WORK* server, BTL_POKEPARAM* poke );
static void scput_Fight_Damage_Single( BTL_SVFLOW_WORK* server, FIGHT_EVENT_PARAM* fep, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scput_Fight_Damage_Enemy2(
	 BTL_SVFLOW_WORK* server, FIGHT_EVENT_PARAM* fep, WazaID waza, 
	 BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void scput_Fight_Damage_OtherAll(
	 BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza,
	 BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec );
static void svflowsub_damage_singular( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
			BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, fx32 targetDmgRatio );
static void svflowsub_damage_enemy_all( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender1, BTL_POKEPARAM* defender2, WazaID waza, fx32 targetDmgRatio );
static void svflowsub_damage_act_singular(
		BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
		WazaID waza,   BtlTypeAff aff,   fx32 targetDmgRatio );
static void svflowsub_damage_act_enemy_all( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker,  BTL_POKEPARAM* def1,  BTL_POKEPARAM* def2,
		BtlTypeAff aff1,  BtlTypeAff aff2,  WazaID waza,
		fx32 targetDmgRatio );
static void svflowsub_damage_act_enemy_all_atonce( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2,
		BtlTypeAff aff, WazaID waza, fx32 targetDmgRatio );
static void scput_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static void scput_Fight_Damage_After_Shrink( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets );
static BOOL svflowsub_hitcheck_singular( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza );
static u16 svflowsub_damage_calc_core( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff typeAff,
		fx32 targetDmgRatio );
static void svflowsub_set_waza_effect( BTL_SVFLOW_WORK* wk, u8 atPokeID, u8 defPokeID, WazaID waza );
static void scput_Fight_AddSick( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza, BTL_POKEPARAM* attacker );
static void scput_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action );
static void scEvent_addSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker, WazaSick sick, BOOL fAlmost );
static void scput_TurnCheck( BTL_SVFLOW_WORK* wk );
static void scput_turncheck_sick( BTL_SVFLOW_WORK* wk );
static void scput_turncheck_weather( BTL_SVFLOW_WORK* wk );
static inline int roundValue( int val, int min, int max );
static inline int minValue( int val, int min );
static inline BOOL perOccur( u8 per );
static void initFightEventParams( FIGHT_EVENT_PARAM* fep );
static BOOL scEvent_CheckConf( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker );
static SV_WazaFailReason scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza );
static BOOL scEvent_checkHit( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BtlTypeAff scEvent_checkAffinity( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_decrementPP( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, u8 wazaIdx );
static u8 scEvent_getHitRatio( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza );
static u8 scEvent_GetHitCount( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, u8 hitCountMax );
static u8 scEvent_getCriticalRank( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza );
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza );
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza );
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender, WazaID waza );
static u16 scEvent_getDamageDenom( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep );
static PokeType scEvent_getWazaPokeType( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza );
static PokeTypePair scEvent_getAttackerPokeType( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker );
static PokeTypePair scEvent_getDefenderPokeType( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender );
static void scEvent_MemberOut( BTL_SVFLOW_WORK* wk, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx );
static void scEvent_MemberIn( BTL_SVFLOW_WORK* wk, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scEvent_PokeComp( BTL_SVFLOW_WORK* wk );
static void scEvent_RankDown( BTL_SVFLOW_WORK* wk, u8 pokeID, BppValueID statusType, u8 volume, BOOL fRandom );
static void scEvent_CheckShrink( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, u8 per );




BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem( 
	BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
	BTL_SERVER_CMD_QUE* que, u32 numClient, HEAPID heapID )
{
	BTL_SVFLOW_WORK* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SVFLOW_WORK) );

	wk->server = server;
	wk->pokeCon = pokeCon;
	wk->mainModule = mainModule;
	wk->numClient = numClient;
	wk->que = que;

	return wk;
}


void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk )
{
	GFL_HEAP_FreeMemory( wk );
}


BOOL BTL_SVFLOW_Start_AfterPokemonIn( BTL_SVFLOW_WORK* wk )
{
	SCQUE_Init( wk->que );

	scEvent_PokeComp( wk );

	// @@@ ���ʂ��Ƃ����邩��
	return FALSE;
}

BOOL BTL_SVFLOW_Start( BTL_SVFLOW_WORK* wk )
{
	const BTL_POKEPARAM* bpp;
	SVCL_WORK* clwk;
	u8 numActPoke, alivePokeBefore, alivePokeAfter, clientID, pokeIdx, i;

	SCQUE_Init( wk->que );
	FlowFlg_ClearAll( wk );

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

			BTL_Printf("Client(%d) �� ", clientID);
			switch( action->gen.cmd ){
			case BTL_ACTION_FIGHT:
				BTL_Printf("�y���������z�������B%d�Ԗڂ̃��U���A%d�Ԃ̑���ɁB\n", action->fight.wazaIdx, action->fight.targetIdx);
				scput_Fight( wk, clientID, pokeIdx, action );
				break;
			case BTL_ACTION_ITEM:
				BTL_Printf("�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n", action->item.number, action->item.targetIdx);
				break;
			case BTL_ACTION_CHANGE:
				BTL_Printf("�y�|�P�����z�������B�ʒu%d <- �|�P%d \n", action->change.posIdx, action->change.memberIdx);
				scput_MemberOut( wk, clientID, action->change.memberIdx );
				scput_MemberIn( wk, clientID, action->change.posIdx, action->change.memberIdx );
				break;
			case BTL_ACTION_ESCAPE:
				BTL_Printf("�y�ɂ���z�������B\n");
				// @@@ ���͑����ɓ�������悤�ɂ��Ă���
				SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_EscapeSuccess, clientID );

				// @@@ ����͌����Aserver�{�̂ōs������
		//				server->quitStep = QUITSTEP_REQ;
				return FALSE;
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
		return TRUE;
	}

	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * �P�^�[�����T�[�o�R�}���h�����i�|�P�����I����j
 *
 * @param   wk		
 *
 * @retval  BOOL		�^�[�����A�m���ɂȂ����|�P����������ꍇ��TRUE�^����ȊOFALSE
 */
//--------------------------------------------------------------------------
BOOL BTL_SVFLOW_StartAfterPokeSelect( BTL_SVFLOW_WORK* wk )
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

			scput_MemberIn( wk, i, action->change.posIdx, action->change.memberIdx );
		}
	}

	// @@@ ���͊m����FALSE�����A����ւ������ɂ܂��т��Ƃ��Ŏ��ʂ��Ƃ�����
	return FALSE;
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
			agility = BTL_POKEPARAM_GetValue( bpp, BPP_AGILITY );

			BTL_Printf("Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
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
// �_���[�W���󂯂��|�P�����L�^���[�N����
//----------------------------------------------------------------------------------------------

static inline void TargetPokeRec_Clear( TARGET_POKE_REC* rec )
{
	rec->count = 0;
}
static inline void TargetPokeRec_Add( TARGET_POKE_REC* rec, BTL_POKEPARAM* pp )
{
	if( rec->count < NELEMS(rec->pp) )
	{
		rec->pp[rec->count++] = pp;
	}
	else
	{
		GF_ASSERT(0);
	}
}
static inline u32 TargetPokeRec_GetCount( const TARGET_POKE_REC* rec )
{
	return rec->count;
}
static inline BTL_POKEPARAM* TargetPokeRec_Get( TARGET_POKE_REC* rec, u32 idx )
{
	if( idx < rec->count )
	{
		return rec->pp[ idx ];
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

//======================================================================================================
// �T�[�o�[�t���[����
//======================================================================================================

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�t�����g�����o�[����ւ�
//-----------------------------------------------------------------------------------
static void scput_MemberIn( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
	SVCL_WORK* clwk = BTL_SERVER_GetClientWork( wk->server, clientID );
	clwk->frontMember[ posIdx ] = clwk->member[ nextPokeIdx ];
	scEvent_MemberIn( wk, &wk->changeEventParams, clientID, posIdx, nextPokeIdx );
}

static void scput_MemberOut( BTL_SVFLOW_WORK* wk, u8 clientID, u8 posIdx )
{
	scEvent_MemberOut( wk, &wk->changeEventParams, clientID, posIdx );
}

//-----------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v���[�g
//-----------------------------------------------------------------------------------
static void scput_Fight( BTL_SVFLOW_WORK* wk, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action )
{
	SVCL_WORK *atClient;
	BTL_POKEPARAM  *attacker;
	FIGHT_EVENT_PARAM* fep;
	WazaID  waza;
	u8 attacker_pokeID;

	atClient = BTL_SERVER_GetClientWork( wk->server, attackClientID );
	attacker = atClient->frontMember[posIdx];
	attacker_pokeID = BTL_POKEPARAM_GetID( attacker );

	waza = BTL_POKEPARAM_GetWazaNumber( attacker, action->fight.wazaIdx );

  fep = &wk->fightEventParams;
	initFightEventParams( fep );
	fep->waza = waza;

// �����񎩔�����
	if( scEvent_CheckConf( wk, attacker ) )
	{
		u16 conf_dmg = scEvent_CalcConfDamage( wk, attacker );

		BTL_POKEPARAM_HpMinus( attacker, conf_dmg );
		SCQUE_PUT_OP_HpMinus( wk->que, atClient->myID, conf_dmg );
		SCQUE_PUT_ACT_ConfDamage( wk->que, BTL_POKEPARAM_GetID(attacker) );
		if( BTL_POKEPARAM_IsDead(attacker) )
		{
			SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Dead, attacker_pokeID );
			SCQUE_PUT_ACT_Dead( wk->que, attacker_pokeID );
			return;
		}
	}
// ���U�o����������
	{
		SV_WazaFailReason  reason = scEvent_CheckWazaExecute( wk, attacker, waza );
		if( reason != SV_WAZAFAIL_NULL )
		{
			cof_put_wazafail_msg_cmd( wk, attacker, reason );
			return;
		}
	}
// ���U���o�����Ƃɐ���
	{
		WazaCategory  category = WAZADATA_GetCategory( waza );
		BtlPokePos    atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, attacker_pokeID );
		BTL_Printf("�o���|�P�ʒu=%d, �o�����U=%d, �J�e�S��=%d\n", atPos, waza, category);

		SCQUE_PUT_MSG_WAZA( wk->que, atPos, action->fight.wazaIdx );

		TargetPokeRec_Clear( &wk->damagedPokemon );

		switch( category ){
		case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
		case WAZADATA_CATEGORY_DAMAGE_EFFECT:
		case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
			scput_Fight_Damage( wk, fep, waza, atClient, attacker, atPos, action );
			break;
		case WAZADATA_CATEGORY_DAMAGE_SICK:
			scput_Fight_Damage( wk, fep, waza, atClient, attacker, atPos, action );
			scput_Fight_AddSick( wk, fep, waza, attacker );
			break;
		case WAZADATA_CATEGORY_SIMPLE_SICK:
			scput_Fight_SimpleSick( wk, waza, attacker, atPos, action );
			break;

//	case WAZADATA_CATEGORY_ICHIGEKI:
//	case WAZADATA_CATEGORY_BIND:
//	case WAZADATA_CATEGORY_GUARD:
//	case WAZADATA_CATEGORY_FIELD_EFFECT:
//	case WAZADATA_CATEGORY_SIDE_EFFECT:
//	case WAZADATA_CATEGORY_OTHERS:
		default:
//		SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaAvoid );
			SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail );
			break;
		}
		scEvent_decrementPP( wk, fep, attacker, action->fight.wazaIdx );
	}
}
// ���U�������s���b�Z�[�W�\���R�}���h���Z�b�g
static void cof_put_wazafail_msg_cmd( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, u8 wazaFailReason )
{
	u8 pokeID = BTL_POKEPARAM_GetID( attacker );

	switch( wazaFailReason ){
	case SV_WAZAFAIL_NEMURI:
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriAct, pokeID );
		break;

	case SV_WAZAFAIL_MAHI:
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_MahiAct, pokeID );
		break;

	case SV_WAZAFAIL_KOORI:
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriAct, pokeID );
		break;

	default:
		SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_WazaFail, pokeID );
		break;
	}
}

//--------------------------------------------------------------------------
/**
 * ���U�Ώۃ|�P�����̃f�[�^�|�C���^���ATARGET_POKE_REC�\���̂ɃZ�b�g
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
static u8 svflowsub_register_waza_targets( BTL_SVFLOW_WORK* wk, BtlPokePos atPos, WazaID waza, const BTL_ACTION_PARAM* action, TARGET_POKE_REC* rec )
{
	WazaTarget  targetType = WAZADATA_GetTarget( waza );
	BTL_POKEPARAM* bpp;

	TargetPokeRec_Clear( rec );

	// �V���O��
	if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE )
	{
		switch( targetType ){
		case WAZA_TARGET_SINGLE:				///< �����ȊO�̂P�́i�I���j
		case WAZA_TARGET_SINGLE_ENEMY:	///< �G�P�́i�I���j
		case WAZA_TARGET_RANDOM:				///< �G�����_��
		case WAZA_TARGET_ENEMY2:				///< �G���Q��
		case WAZA_TARGET_OTHER_ALL:			///< �����ȊO�S��
			bpp = svflowsub_get_opponent_pokeparam( wk, atPos, 0 );
			TargetPokeRec_Add( rec, bpp );
			break;

		case WAZA_TARGET_ONLY_USER:			///< �����P�̂̂�
		case WAZA_TARGET_SINGLE_FRIEND:	///< �������܂ޖ����P��
			bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, atPos );
			TargetPokeRec_Add( rec, bpp );
			break;

		default:
			return 0;
		}
		return 1;
	}
	// �_�u��
	else
	{
		switch( targetType ){
		case WAZA_TARGET_SINGLE:				///< �����ȊO�̂P�́i�I���j
			if( action->fight.targetIdx < 2 ){
				bpp = svflowsub_get_opponent_pokeparam( wk, atPos, action->fight.targetIdx );
			}else{
				bpp = svflowsub_get_next_pokeparam( wk, atPos );
			}
			TargetPokeRec_Add( rec, bpp );
			return 1;
		case WAZA_TARGET_SINGLE_ENEMY:	///< �G�P�́i�I���j
			bpp = svflowsub_get_opponent_pokeparam( wk, atPos, action->fight.targetIdx );
			TargetPokeRec_Add( rec, bpp );
			return 1;
		case WAZA_TARGET_RANDOM:				///< �G�����_��
			bpp = svflowsub_get_opponent_pokeparam( wk, atPos, GFL_STD_MtRand(1) );
			TargetPokeRec_Add( rec, bpp );
			return 1;

		case WAZA_TARGET_ENEMY2:				///< �G���Q��
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam(wk, atPos, 0) );
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam(wk, atPos, 1) );
			return 2;

		case WAZA_TARGET_OTHER_ALL:			///< �����ȊO�S��
			TargetPokeRec_Add( rec, svflowsub_get_next_pokeparam( wk, atPos ) );
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam( wk, atPos, 0 ) );
			TargetPokeRec_Add( rec, svflowsub_get_opponent_pokeparam( wk, atPos, 1 ) );
			return 3;

		case WAZA_TARGET_ONLY_USER:			///< �����P�̂̂�
			TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
			return 1;
		case WAZA_TARGET_SINGLE_FRIEND:	///< �������܂ޖ����P�́i�I���j
			// @@@ ����ł͂��߂����Ƃ肠����
			TargetPokeRec_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, atPos) );
			return 1;
		case WAZA_TARGET_OTHER_FRIEND:	///< �����ȊO�̖����P��
			TargetPokeRec_Add( rec, svflowsub_get_next_pokeparam( wk, atPos ) );
			return 1;
		default:
			return 0;
		}
	}
}

//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n
//----------------------------------------------------------------------
static void scput_Fight_Damage( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza, SVCL_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action )
{
	u8 targetCnt;

	FlowFlg_Clear( wk, FLOWFLG_SET_WAZAEFFECT );

	targetCnt = svflowsub_register_waza_targets( wk, atPos, waza, action, &wk->targetPokemon );

	// �V���O��
	if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_SINGLE )
	{
		scput_Fight_Damage_Single( wk, fep, waza, attacker, &wk->targetPokemon );
	}
	// �_�u��
	else
	{
		{
			WazaTarget  type = WAZADATA_GetTarget( waza );
			BTL_Printf("�_�u���ł��B���U�i���o�[=%d, ���U�^�[�Q�b�g�^�C�v=%d\n", waza, type);
		}
		switch( targetCnt ) {
		case 1:
			scput_Fight_Damage_Single( wk, fep, waza, attacker, &wk->targetPokemon );
			break;
		case 2:				///< �G���Q��
			scput_Fight_Damage_Enemy2( wk, fep, waza, attacker, &wk->targetPokemon );
			break;
		case 3:			///< �����ȊO�S��
			scput_Fight_Damage_OtherAll( wk, fep, waza, attacker, &wk->targetPokemon );
			break;
		default:
//			GF_ASSERT_MSG(0, "illegal targetType!! waza=%d, targetType=%d", waza, targetType);
			return;
		}
	}
	scput_Fight_Damage_After( wk, waza, attacker, &wk->damagedPokemon );
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��ʒu���猩�đΐ푊��̃|�P�����f�[�^��Ԃ�
//----------------------------------------------------------------------
static BTL_POKEPARAM* svflowsub_get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx )
{
	SVCL_WORK* clwk;
	BtlPokePos targetPos;
	u8 clientID, pokeIdx;

	targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, pos, pokeSideIdx );
	BTL_MAIN_BtlPosToClientID_and_PokeIdx( wk->mainModule, targetPos, &clientID, &pokeIdx );

	clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

	return clwk->frontMember[ pokeIdx ];
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��ʒu���猩�ėׂ̃|�P�����f�[�^��Ԃ�
//----------------------------------------------------------------------
static BTL_POKEPARAM* svflowsub_get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos )
{
	SVCL_WORK* clwk;
	BtlPokePos nextPos;
	u8 clientID, pokeIdx;

	nextPos = BTL_MAIN_GetNextPokePos( wk->mainModule, pos );
	BTL_MAIN_BtlPosToClientID_and_PokeIdx( wk->mainModule, nextPos, &clientID, &pokeIdx );
	clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

	return clwk->frontMember[ pokeIdx ];
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��|�P����������ł��玀�S�����R�}���h����
//----------------------------------------------------------------------
static void svflowsub_check_and_make_dead_command( BTL_SVFLOW_WORK* server, BTL_POKEPARAM* poke )
{
	if( BTL_POKEPARAM_IsDead(poke) )
	{
		u8 pokeID = BTL_POKEPARAM_GetID( poke );
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Dead, pokeID );
		SCQUE_PUT_ACT_Dead( server->que, pokeID );
	}
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n > �P�̑I��
//----------------------------------------------------------------------
static void scput_Fight_Damage_Single( BTL_SVFLOW_WORK* server, FIGHT_EVENT_PARAM* fep, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
	BTL_POKEPARAM* defender;

	defender = TargetPokeRec_Get( targetRec, 0 );
	GF_ASSERT(defender!=NULL);

	svflowsub_damage_singular( server, fep, attacker, defender, waza, BTL_CALC_DMG_TARGET_RATIO_NONE );
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n > ����Q�̑Ώ�
//----------------------------------------------------------------------
static void scput_Fight_Damage_Enemy2(
	 BTL_SVFLOW_WORK* server, FIGHT_EVENT_PARAM* fep, WazaID waza, 
	 BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
	BTL_POKEPARAM *defpoke1, *defpoke2;
	fx32 targetDmgRatio = BTL_CALC_DMG_TARGET_RATIO_PLURAL;

	defpoke1 = TargetPokeRec_Get( targetRec, 0 );
	defpoke2 = TargetPokeRec_Get( targetRec, 1 );
	GF_ASSERT(defpoke1!=NULL);
	GF_ASSERT(defpoke2!=NULL);

	if( BTL_POKEPARAM_IsDead(defpoke1) || BTL_POKEPARAM_IsDead(defpoke2) )
	{
		targetDmgRatio = BTL_CALC_DMG_TARGET_RATIO_NONE;
	}

	svflowsub_damage_enemy_all( server, fep, attacker, defpoke1, defpoke2, waza, targetDmgRatio );
}

//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n > �R�́i�����ȊO�j�Ώ�
//----------------------------------------------------------------------
static void scput_Fight_Damage_OtherAll(
	 BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza,
	 BTL_POKEPARAM* attacker, TARGET_POKE_REC* targetRec )
{
	BTL_POKEPARAM *defF, *defE1, *defE2;
	fx32 targetDmgRatio;

	defF  = TargetPokeRec_Get( targetRec, 0 );
	defE1 = TargetPokeRec_Get( targetRec, 1 );
	defE2 = TargetPokeRec_Get( targetRec, 2 );

	GF_ASSERT(defF);
	GF_ASSERT(defE1);
	GF_ASSERT(defE2);

	targetDmgRatio = BTL_CALC_DMG_TARGET_RATIO_PLURAL;
	{
		u32 alive_targets = 0;
		if( !BTL_POKEPARAM_IsDead(defF) ){ ++alive_targets; }
		if( !BTL_POKEPARAM_IsDead(defE1) ){ ++alive_targets; }
		if( !BTL_POKEPARAM_IsDead(defE2) ){ ++alive_targets; }
		if( alive_targets < 2 )
		{
			targetDmgRatio = BTL_CALC_DMG_TARGET_RATIO_NONE;
		}
	}

	svflowsub_damage_singular( wk, fep, attacker, defF, waza, targetDmgRatio );
	svflowsub_damage_enemy_all( wk, fep, attacker, defE1, defE2, waza, targetDmgRatio );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i��ʕ���j
//------------------------------------------------------------------
static void svflowsub_damage_singular( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
			BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, fx32 targetDmgRatio )
{
	BtlTypeAff aff;

	if( !svflowsub_hitcheck_singular(wk, fep, attacker, defender, waza) )
	{
		return;
	}

	aff = scEvent_checkAffinity( wk, fep, attacker, defender, waza );

	svflowsub_damage_act_singular( wk, fep, attacker, defender, waza, aff, targetDmgRatio );

	svflowsub_check_and_make_dead_command( wk, defender );
	svflowsub_check_and_make_dead_command( wk, attacker );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F����S�̃_���[�W�����i��ʕ���j
//------------------------------------------------------------------
static void svflowsub_damage_enemy_all( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
	BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender1, BTL_POKEPARAM* defender2, WazaID waza, fx32 targetDmgRatio )
{
	u8 hit1, hit2;
	u8 alive1, alive2;

	hit1 = svflowsub_hitcheck_singular(wk, fep, attacker, defender1, waza);
	hit2 = svflowsub_hitcheck_singular(wk, fep, attacker, defender2, waza);

	alive1 = !BTL_POKEPARAM_IsDead( defender1 );
	alive2 = !BTL_POKEPARAM_IsDead( defender2 );

	// �Q�̂Ƃ��q�b�g
	if( hit1 && hit2 )
	{
		BtlTypeAff aff1, aff2;
		u8 about1, about2;

		aff1 = scEvent_checkAffinity( wk, fep, attacker, defender1, waza );
		aff2 = scEvent_checkAffinity( wk, fep, attacker, defender2, waza );

		about1 = BTL_CALC_TypeAffAbout( aff1 );
		about2 = BTL_CALC_TypeAffAbout( aff2 );

		// �������ʁX�Ȃ�A�ʂ̏���
		if( about1 != about2 )
		{
			BTL_Printf("�Q�̂Ƃ��Ƀq�b�g���A�����ׂׂ�\n");
			svflowsub_damage_act_enemy_all( wk, fep, attacker, defender1, defender2, aff1, aff2, waza, targetDmgRatio );
		}
		// �����������Ȃ�A�̗̓o�[�𓯎��Ɍ��炷�悤�ɃR�}���h��������
		else
		{
			BTL_Printf("�Q�̂Ƃ��Ƀq�b�g���A�������ꏏ\n");
			svflowsub_damage_act_enemy_all_atonce( wk, fep, attacker, defender1, defender2, aff1, waza, targetDmgRatio );
		}
	}
	// �ǂ������̂݃q�b�g
	else if( hit1 || hit2 )
	{
		BTL_POKEPARAM* defender = (hit1)? defender1 : defender2;
		BtlTypeAff aff;

		aff = scEvent_checkAffinity( wk, fep, attacker, defender, waza );
		svflowsub_damage_act_singular( wk, fep, attacker, defender, waza, aff, targetDmgRatio );
	}
	// �Q�̂Ƃ��n�Y��
	else
	{
		// return;
	}

	// ���S�`�F�b�N�i���U��������Ȃ��Ă������ȂǂŎ��ʃP�[�X�͂���j
	if( alive1 ){
		svflowsub_check_and_make_dead_command( wk, defender1 );
	}
	if( alive2 ){
		svflowsub_check_and_make_dead_command( wk, defender2 );
	}
	svflowsub_check_and_make_dead_command( wk, attacker );

}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_singular(
		BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker,  BTL_POKEPARAM* defender,
		WazaID waza,   BtlTypeAff aff,   fx32 targetDmgRatio )
{
	u8 pluralHit = FALSE;	// ������q�b�g�t���O
	u8 deadFlag = FALSE;
	u8 atPokeID, defPokeID;
	int i;

	fep->hitCountMax = WAZADATA_GetMaxHitCount( waza );
	if( fep->hitCountMax > 1 )
	{
		fep->hitCountReal = scEvent_GetHitCount( wk, fep, fep->hitCountMax );
		pluralHit = TRUE;
	}
	else
	{
		fep->hitCountReal = 1;
	}

	atPokeID = BTL_POKEPARAM_GetID( attacker );
	defPokeID = BTL_POKEPARAM_GetID( defender );

	for(i=0; i<fep->hitCountReal; ++i)
	{
		fep->realDamage = svflowsub_damage_calc_core( wk, fep, attacker, defender, waza, aff, targetDmgRatio );

		// �f�o�b�O���ȒP�ɂ��邽�ߕK����_���[�W�ɂ���[�u
		#ifdef PM_DEBUG
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
		{
			u8 pokeID = BTL_POKEPARAM_GetID(defender);
			if( pokeID >= BTL_PARTY_MEMBER_MAX )
			{
				fep->realDamage = 999;
			}
		}
		if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
		{
			u8 pokeID = BTL_POKEPARAM_GetID(defender);
			if( pokeID < BTL_PARTY_MEMBER_MAX )
			{
				fep->realDamage = 999;
			}
		}
		#endif

		BTL_POKEPARAM_HpMinus( defender, fep->realDamage );
		SCQUE_PUT_OP_HpMinus( wk->que, defPokeID, fep->realDamage );
		svflowsub_set_waza_effect( wk, atPokeID, defPokeID, waza );
		SCQUE_PUT_ACT_WazaDamage( wk->que, defPokeID, fep->typeAff, fep->realDamage );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );

		BTL_Printf(" Waza Aff=%d, Damage=%d\n", fep->typeAff, fep->realDamage );

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

	TargetPokeRec_Add( &wk->damagedPokemon, defender );

	BTL_Printf("Damage Waza Exe %d times ... plural=%d, dead=%d\n",
			fep->hitCountReal, pluralHit, deadFlag);

	if( pluralHit )
	{
//			SCQUE_PUT_MSG_WazaHitCount( wk->que, i );	// @@@ ���Ƃ�
	}
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�G�S�̂����ԂɃ_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_enemy_all( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker,  BTL_POKEPARAM* def1,  BTL_POKEPARAM* def2,
		BtlTypeAff aff1,  BtlTypeAff aff2,  WazaID waza,
		fx32 targetDmgRatio )
{
	u8 atPokeID, defPokeID1, defPokeID2;
	u16 dmg1, dmg2;

	// �����U���̎��A�q�b�g�J�E���g�͂P�Œ�
	fep->hitCountMax = 1;
	fep->hitCountReal = 1;

	atPokeID = BTL_POKEPARAM_GetID( attacker );
	defPokeID1 = BTL_POKEPARAM_GetID( def1 );
	defPokeID2 = BTL_POKEPARAM_GetID( def2 );

	dmg1 = svflowsub_damage_calc_core( wk, fep, attacker, def1, waza, aff1, targetDmgRatio );
	BTL_POKEPARAM_HpMinus( def1, dmg1 );
	SCQUE_PUT_OP_HpMinus( wk->que, defPokeID1, fep->realDamage );
	TargetPokeRec_Add( &wk->damagedPokemon, def1 );

	dmg2 = svflowsub_damage_calc_core( wk, fep, attacker, def2, waza, aff2, targetDmgRatio );
	BTL_POKEPARAM_HpMinus( def2, dmg2 );
	SCQUE_PUT_OP_HpMinus( wk->que, defPokeID2, fep->realDamage );
	TargetPokeRec_Add( &wk->damagedPokemon, def2 );

	svflowsub_set_waza_effect( wk, atPokeID, defPokeID1, waza );
	SCQUE_PUT_ACT_WazaDamage( wk->que, defPokeID1, aff1, dmg1 );
	SCQUE_PUT_ACT_WazaDamage( wk->que, defPokeID2, aff2, dmg2 );

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�G�S�̂���ĂɃ_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_enemy_all_atonce( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2,
		BtlTypeAff aff, WazaID waza, fx32 targetDmgRatio )
{
	u8 atPokeID, defPokeID1, defPokeID2;
	u16 dmg1, dmg2;

	// �����U���̎��A�q�b�g�J�E���g�͂P�Œ�
	fep->hitCountMax = 1;
	fep->hitCountReal = 1;

	atPokeID = BTL_POKEPARAM_GetID( attacker );
	defPokeID1 = BTL_POKEPARAM_GetID( def1 );
	defPokeID2 = BTL_POKEPARAM_GetID( def2 );

	dmg1 = svflowsub_damage_calc_core( wk, fep, attacker, def1, waza, aff, targetDmgRatio );
	BTL_POKEPARAM_HpMinus( def1, dmg1 );
	SCQUE_PUT_OP_HpMinus( wk->que, defPokeID1, dmg1 );
	TargetPokeRec_Add( &wk->damagedPokemon, def1 );

	dmg2 = svflowsub_damage_calc_core( wk, fep, attacker, def2, waza, aff, targetDmgRatio );
	BTL_POKEPARAM_HpMinus( def2, dmg2 );
	SCQUE_PUT_OP_HpMinus( wk->que, defPokeID2, dmg2 );
	TargetPokeRec_Add( &wk->damagedPokemon, def2 );

	svflowsub_set_waza_effect( wk, atPokeID, defPokeID1, waza );
	SCQUE_PUT_ACT_WazaDamageDbl( wk->que, defPokeID1, defPokeID2, dmg1, dmg2, aff );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_AFTER );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�F�_���[�W�󂯌�̏���
//------------------------------------------------------------------
static void scput_Fight_Damage_After( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, TARGET_POKE_REC* targets )
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
			scEvent_CheckShrink( wk, bpp, per );
		}
	}
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̕��U���q�b�g�`�F�b�N
//------------------------------------------------------------------
static BOOL svflowsub_hitcheck_singular( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza )
{
	u8 defPokeID = BTL_POKEPARAM_GetID( defender );

	// ���ɑΏۂ�����ł���
	if( BTL_POKEPARAM_IsDead(defender) )
	{
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, defPokeID );
		return FALSE;
	}
	// �������E��𗦂ɂ��q�b�g�`�F�b�N�ŊO�ꂽ
	if( !scEvent_checkHit(wk, fep, attacker, defender, waza) )
	{
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_WazaAvoid, defPokeID );
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �������� > �_���[�W���U�n > �_���[�W�l�v�Z
//------------------------------------------------------------------
static u16 svflowsub_damage_calc_core( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep,
		const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff typeAff,
		fx32 targetDmgRatio )
{
	fep->wazaDamageType = WAZADATA_GetDamageType( waza );

	GF_ASSERT(fep->wazaDamageType != WAZADATA_DMG_NONE);

	fep->criticalRank = scEvent_getCriticalRank( wk, fep, attacker, waza );
	fep->criticalFlag = BTL_CALC_CheckCritical( fep->criticalRank );
	fep->wazaPower = scEvent_getWazaPower( wk, fep, waza );
	fep->attackerPower = scEvent_getAttackPower( wk, fep, attacker, waza );
	fep->defenderGuard = scEvent_getDefenderGuard( wk, fep, defender, waza );

	fep->damageDenom = scEvent_getDamageDenom( wk, fep );
	{
		u8 attackerLevel = BTL_POKEPARAM_GetValue( attacker, BPP_LEVEL );
		fep->rawDamage = fep->wazaPower * fep->attackerPower * (attackerLevel*2/5+2) / fep->defenderGuard;
		fep->rawDamage /= fep->damageDenom;
		fep->rawDamage = (fep->rawDamage * targetDmgRatio) >> FX32_SHIFT;
		{
			fx32 weatherDmgRatio = BTL_FIELD_GetWeatherDmgRatio( waza );
			if( weatherDmgRatio != BTL_CALC_DMG_WEATHER_RATIO_NONE )
			{
				u32 prevDmg = fep->rawDamage;
				fep->rawDamage = (fep->rawDamage * weatherDmgRatio) >> FX32_SHIFT;
				BTL_Printf("�V��ɂ��␳������, �␳��=%08x, dmg=%d->%d\n", weatherDmgRatio, prevDmg, fep->rawDamage);
			}
		}
		if( fep->criticalFlag )
		{
			fep->rawDamage *= 2;
		}

		BTL_Printf("�З�:%d, ��������:%d, LV:%d, �ڂ�����:%d, ����:%d ... �f�_��:%d\n",
				fep->wazaPower, fep->attackerPower, attackerLevel, fep->defenderGuard, fep->damageDenom, fep->rawDamage
		);
	}
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGE_PROC1 );

	//�����_���␳
	{
		u16 ratio = 100 - GFL_STD_MtRand(16);
		fep->rawDamage = (fep->rawDamage * ratio) / 100;
		BTL_Printf("�����_���␳:%d%%  -> �f�_��=%d\n", ratio, fep->rawDamage);
	}

	// �^�C�v��v�␳
	fep->rawDamage = (fep->rawDamage * fep->typeMatchRatio) >> FX32_SHIFT;
	BTL_Printf("�^�C�v��v�␳:%08x  -> �f�_��=%d\n", fep->typeMatchRatio, fep->rawDamage);

	// �^�C�v�����v�Z
	fep->realDamage = BTL_CALC_AffDamage( fep->rawDamage, typeAff );

	// �₯�Ǖ␳
	if( fep->wazaDamageType == WAZADATA_DMG_PHYSIC )
	{
		if( BTL_POKEPARAM_GetPokeSick(attacker) == POKESICK_YAKEDO )
		{
			fep->realDamage = (fep->realDamage * BTL_YAKEDO_DAMAGE_RATIO) / 100;
		}
	}

	// �����U���␳

	// �Œ�A�P�̓_���[�W��^����
	if( fep->realDamage == 0 )
	{
		fep->realDamage = 1;
	}

	BTL_Printf("�^�C�v����:%02d -> �_���[�W�l�F%d\n", typeAff, fep->realDamage);

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_DMG_PROC2 );

	return fep->realDamage;
}
//---------------------------------------------------------------------------------------------
// �P�x�̃��U�����ɂāA�G�t�F�N�g�����R�}���h���P�񂵂��쐬���Ȃ����߂̎d�g��
//---------------------------------------------------------------------------------------------
static void svflowsub_set_waza_effect( BTL_SVFLOW_WORK* wk, u8 atPokeID, u8 defPokeID, WazaID waza )
{
	if( FlowFlg_Get(wk, FLOWFLG_SET_WAZAEFFECT ) == FALSE )
	{
		BTL_Printf("���U�G�t�F�N�g�R�}���h�������܂���, ���U�i���o%d\n", waza);
		SCQUE_PUT_ACT_WazaEffect( wk->que, atPokeID, defPokeID, waza );
		FlowFlg_Set( wk, FLOWFLG_SET_WAZAEFFECT );
	}
	else
	{
		BTL_Printf("���U�G�t�F�N�g�R�}���h�����ς݂Ȃ̂Ŗ���, ���U�i���o%d\n", waza);
	}
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F�ǉ����ʂɂ���Ԉُ�
//---------------------------------------------------------------------------------------------
static void scput_Fight_AddSick( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza, BTL_POKEPARAM* attacker )
{
	BTL_POKEPARAM* receiver;
	WazaSick sick;
	u32 per, i=0;

	sick = WAZADATA_GetSick( waza );
	per = WAZADATA_GetSickPer( waza );

	while( 1 )
	{
		receiver = TargetPokeRec_Get( &wk->damagedPokemon, i );
		if( receiver == NULL ){ break; }
		{
			if( per >= GFL_STD_MtRand(100) )
			{
				scEvent_addSick( wk, receiver, attacker, sick, (per>=100) );
			}
		}
		++i;
	}
}
//---------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�F���U�ɂ�钼�ڂ̏�Ԉُ�
//---------------------------------------------------------------------------------------------
static void scput_Fight_SimpleSick( BTL_SVFLOW_WORK* wk, WazaID waza, BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action )
{
	BTL_POKEPARAM* target;
	WazaSick sick;
	u32 per, i = 0;

	svflowsub_register_waza_targets( wk, atPos, waza, action, &wk->targetPokemon );

	sick = WAZADATA_GetSick( waza );
	per = WAZADATA_GetHitRatio( waza );

	while( 1 )
	{
		target = TargetPokeRec_Get( &wk->targetPokemon, i++ );
		if( target )
		{
			scEvent_addSick( wk, target, attacker, sick, (per>=100) );
		}
		else
		{
			break;
		}
	}
}

//--------------------------------------------------------------------------
/**
 * �yEvent�z��Ԉُ폈��
 *
 * @param   wk					
 * @param   receiver		��Ԉُ���󂯂�Ώۂ̃|�P�����p�����[�^
 * @param   attacker		��Ԉُ�
 * @param   sick				��Ԉُ�ID
 * @param   fAlmost			�قڊm��t���O�i���j
 *
 * ���قڊm��t���O�Ƃ�
 *	�v���C���[���قڊm���ɏ�Ԉُ�ɂȂ�͂��A�Ǝv���󋵂�TRUE�ɂ���B
 *	�ǉ����ʂȂ���ʊm��100�A���U���ڌ��ʂȂ烏�U������100�̏ꍇ�B�Ƃ��������ʂ̏ꍇ�͏��TRUE�B
 *	���ꂪTRUE�̏ꍇ�A��Ԉُ�𖳌�������Ƃ����������������ꍇ�ɂ��̂��Ƃ�\�����邽�߂Ɏg���B
 */
//--------------------------------------------------------------------------
static void scEvent_addSick( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* receiver, BTL_POKEPARAM* attacker, WazaSick sick, BOOL fAlmost )
{
	// �Ă񂱂��u�͂�v�̎��Ɂu������v�ɂ͂Ȃ�Ȃ�
	if( (BTL_FIELD_GetWeather() == BTL_WEATHER_SHINE)
	&&  (sick == WAZASICK_KOORI )
	){
		sick = WAZASICK_NULL;
	}
	// ���łɃ|�P�����n��Ԉُ�ɂȂ��Ă���Ȃ�A�V���Ƀ|�P�����n��Ԉُ�ɂ͂Ȃ�Ȃ�
	if( (sick < POKESICK_MAX) && (BTL_POKEPARAM_GetPokeSick(receiver) != POKESICK_NULL) )
	{
		sick = WAZASICK_NULL;
	}

	if( sick != WAZASICK_NULL )
	{
		BTL_EVENTVAR_Push();
		BTL_EVENTVAR_SetValue( BTL_EVAR_SICKID, sick );
		BTL_EVENTVAR_SetValue( BTL_EVAR_TURN_COUNT, BTL_CALC_DecideSickTurn(sick) );
		BTL_EVENTVAR_SetValue( BTL_EVAR_ALMOST_FLAG, fAlmost );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_ADD_SICK );
		sick = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
		if( sick != WAZASICK_NULL )
		{
			u8 pokeID = BTL_POKEPARAM_GetID( receiver );
			u8 turn = BTL_EVENTVAR_GetValue( BTL_EVAR_TURN_COUNT );
			BTL_POKEPARAM_SetWazaSick( receiver, sick, turn );
			SCQUE_PUT_OP_SetSick( wk->que, pokeID, sick, turn );
			SCQUE_PUT_ACT_SickSet( wk->que, pokeID, sick );
		}
		BTL_EVENTVAR_Pop();
	}
}


//==============================================================================
// �T�[�o�[�t���[�F�^�[���`�F�b�N���[�g
//==============================================================================
static void scput_TurnCheck( BTL_SVFLOW_WORK* wk )
{
	scput_turncheck_sick( wk );
	scput_turncheck_weather( wk );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > ��Ԉُ�
//------------------------------------------------------------------
static void scput_turncheck_sick( BTL_SVFLOW_WORK* wk )
{
	FIGHT_EVENT_PARAM* fep;
	FRONT_POKE_SEEK_WORK  fpsw;
	BTL_POKEPARAM* bpp;
	u8 pokeID;

	fep = &wk->fightEventParams;
	FRONT_POKE_SEEK_InitWork( &fpsw, wk );

	while( FRONT_POKE_SEEK_GetNext( &fpsw, wk, &bpp ) )
	{
		pokeID = BTL_POKEPARAM_GetID( bpp );

		fep->sick = BTL_POKEPARAM_GetPokeSick( bpp );
		fep->sickDamage = BTL_POKEPARAM_CalcSickDamage( bpp );
		if( fep->sick != POKESICK_NULL && fep->sickDamage )
		{
			BTL_EVENT_CallHandlers( wk, BTL_EVENT_SICK_DAMAGE );
			if( fep->sickDamage > 0 )
			{
				BTL_POKEPARAM_HpMinus( bpp, fep->sickDamage );
				SCQUE_PUT_OP_HpMinus( wk->que, pokeID, fep->sickDamage );
				SCQUE_PUT_SickDamage( wk->que, pokeID, fep->sick, fep->sickDamage );
			}
		}
		BTL_POKEPARAM_WazaSick_TurnCheck( bpp );
		SCQUE_PUT_OP_WazaSickTurnCheck( wk->que, pokeID );
		svflowsub_check_and_make_dead_command( wk, bpp );
	}
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

	weather = BTL_FIELD_GetWeather();
	if( (weather == BTL_WEATHER_SAND)
	||	(weather == BTL_WEATHER_SNOW)
	){
		FRONT_POKE_SEEK_WORK  fps;
		u8 pokeID[ BTL_POS_MAX ];
		BTL_POKEPARAM* bpp;
		u8 poke_cnt = 0, i;

		FRONT_POKE_SEEK_InitWork( &fps, wk );
		while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
		{
			if( !BTL_POKEPARAM_IsDead(bpp) )
			{
				// ����ł��炸�A�m�[�_���^�C�v�ł��Ȃ�=�_���[�W�󂯂�̊m��
				if( BTL_CALC_CheckRecvWeatherDamage(bpp, weather) )
				{
					u16 dmg;

					dmg = BTL_POKEPARAM_GetValue( bpp, BPP_MAX_HP ) / 16;
					if( dmg == 0 ){ dmg = 1; }
					pokeID[ poke_cnt ] = BTL_POKEPARAM_GetID( bpp );

					BTL_POKEPARAM_HpMinus( bpp, dmg );
					SCQUE_PUT_OP_HpMinus( wk->que, pokeID[ poke_cnt ], dmg );
					poke_cnt++;
				}
			}
		}/* while( FRONT_POKE_SEEK_GetNext(...) */

		// �V��_���[�W�R�}���h�i�����όj
		SCQUE_PUT_ACT_WeatherDamage( wk->que, weather, poke_cnt );
		for(i=0; i<poke_cnt; ++i)
		{
			SCQUE_PUT_ArgOnly( wk->que, pokeID[i] );
		}
		// ���S�`�F�b�N
		FRONT_POKE_SEEK_InitWork( &fps, wk );
		while( FRONT_POKE_SEEK_GetNext( &fps, wk, &bpp ) )
		{
			for(i=0; i<poke_cnt; ++i)
			{
				if( pokeID[i] == BTL_POKEPARAM_GetID(bpp) )
				{
					svflowsub_check_and_make_dead_command( wk, bpp );
					break;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------------------
static inline int roundValue( int val, int min, int max )
{
	if( val < min ){ val = min; }
	if( val > max ){ val = max; }
	return val;
}
static inline int minValue( int val, int min )
{
	if( val < min ){ val = min; }
	return val;
}
static inline BOOL perOccur( u8 per )
{
	return GFL_STD_MtRand(100) < per;
}

static void initFightEventParams( FIGHT_EVENT_PARAM* fep )
{
	GFL_STD_MemClear( fep, sizeof(*fep) );
}


//---------------------------------------------------------------------------------------------
// �R�}���h�o��  �C�x���g�Q
//---------------------------------------------------------------------------------------------

// �����񎩔��`�F�b�N
static BOOL scEvent_CheckConf( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
	if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KONRAN) )
	{
		u8 pokeID = BTL_POKEPARAM_GetID( attacker );
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KonranAct, pokeID );
		{
			BOOL  fexe = ( GFL_STD_MtRand(100) < BTL_CONF_EXE_RATIO );
			return fexe;
		}
	}
	return FALSE;
}

// �����_���[�W�v�Z
static u16 scEvent_CalcConfDamage( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker )
{
	u16 dmg = minValue( BTL_POKEPARAM_GetValue(attacker, BPP_MAX_HP)/8, 1 );

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
 * @param   attacker	�U���|�P�����p�����[�^
 * @param   waza			�o�����Ƃ��Ă��郏�U
 *
 * @retval  SV_WazaFailReason		���s���R�R�[�h�i���s���Ȃ�������SV_WAZAFAIL_NULL�j
 */
//--------------------------------------------------------------------------
static SV_WazaFailReason scEvent_CheckWazaExecute( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, WazaID waza )
{
	BTL_EVENTVAR_Push();
	BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_REASON, SV_WAZAFAIL_NULL );
	BTL_EVENTVAR_SetValue( BTL_EVAR_WAZAID, waza );

	do {
		WazaSick sick = BTL_POKEPARAM_GetPokeSick( attacker );

		// @@@ �Ђ�݁A�����������̃`�F�b�N���܂��ł��i���������͑��肪�m�肵�Ă��炩�j
		switch( sick ){
		case POKESICK_NEMURI:
			if( !BTL_POKEPARAM_Nemuri_CheckWake(attacker) ){
				BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_REASON, SV_WAZAFAIL_NEMURI );
			}else{
				u8 pokeID = BTL_POKEPARAM_GetID( attacker );
				BTL_POKEPARAM_SetPokeSick( attacker, POKESICK_NULL, 0 );
				SCQUE_PUT_OP_SetSick( wk->que, pokeID, POKESICK_NULL, 0 );
				SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_NemuriWake, pokeID );
			}
			break;
		case POKESICK_MAHI:
			if( GFL_STD_MtRand(100) < BTL_MAHI_EXE_RATIO )
			{
				BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_REASON, SV_WAZAFAIL_MAHI );
			}
			break;
		case POKESICK_KOORI:
			if( GFL_STD_MtRand(100) >= BTL_KORI_MELT_RATIO ){
				BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_REASON, SV_WAZAFAIL_KOORI );
			}else{
				u8 pokeID = BTL_POKEPARAM_GetID( attacker );
				BTL_POKEPARAM_SetPokeSick( attacker, POKESICK_NULL, 0 );
				SCQUE_PUT_OP_SetSick( wk->que, pokeID, POKESICK_NULL, 0 );
				SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KoriMelt, pokeID );
			}
			break;
		}

	}while(0);

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_EXECUTE );
	{
		SV_WazaFailReason reason = BTL_EVENTVAR_GetValue( BTL_EVAR_FAIL_REASON );
		BTL_EVENTVAR_Pop();
		return reason;
	}
}

// ���U�����邩����
static BOOL scEvent_checkHit( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	if( WAZADATA_IsAlwaysHit(waza) )
	{
		return TRUE;
	}
	else
	{
		u8 wazaHitRatio, per;
		s8 hitRank, avoidRank, totalRank;

		wazaHitRatio = scEvent_getHitRatio(wk, fep, waza);

		hitRank = BTL_POKEPARAM_GetValue( attacker, BPP_HIT_RATIO );
		avoidRank = BTL_POKEPARAM_GetValue( defender, BPP_AVOID_RATIO );
		totalRank = roundValue( BTL_CALC_HITRATIO_MID + avoidRank - hitRank, BTL_CALC_HITRATIO_MIN, BTL_CALC_HITRATIO_MAX );

		per = BTL_CALC_HitPer( wazaHitRatio, totalRank );

		return ( GFL_STD_MtRand(100) < (u32)per );
	}
}

// ���U�����`�F�b�N
static BtlTypeAff scEvent_checkAffinity( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	// �^�C�v��v�␳
	fep->wazaPokeType = scEvent_getWazaPokeType( wk, fep, waza );
	fep->attackerPokeType = scEvent_getAttackerPokeType( wk, fep, attacker );

	if( PokeTypePair_IsMatch(fep->attackerPokeType, fep->wazaPokeType) )
	{
		fep->typeMatchRatio = FX32_CONST(1.5f);
	}
	else
	{
		fep->typeMatchRatio = FX32_CONST(1.0f);
	}
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_TYPEMATCH_RATIO );

	// �^�C�v�����v�Z
	fep->defenderPokeType = scEvent_getDefenderPokeType( wk, fep, defender );
	fep->typeAff = BTL_CALC_TypeAff( fep->wazaPokeType, fep->defenderPokeType );

	return fep->typeAff;
}

// �g�������U��PP�f�N�������g
static void scEvent_decrementPP( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, u8 wazaIdx )
{
	fep->decPP = 1;

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECREMENT_PP );

	if( fep->decPP )
	{
		u8 pokeID = BTL_POKEPARAM_GetID( attacker );
		BTL_Printf("DECPP_ �R�}���h�Z�b�g, value=%d\n", fep->decPP);
		SCQUE_PUT_OP_PPMinus( wk->que, pokeID, wazaIdx, fep->decPP );
	}

}

// ���U�I�����擾(0-100)
static u8 scEvent_getHitRatio( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza )
{
	fep->wazaHitRatio = WAZADATA_GetHitRatio( waza );
	if( BTL_FIELD_GetWeather() == BTL_WEATHER_MIST )
	{
		fep->wazaHitRatio = (fep->wazaHitRatio * BTL_CALC_WEATHER_MIST_HITRATIO) >> FX32_SHIFT;
	}
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_RATIO );
	return fep->wazaHitRatio;
}

// �q�b�g����ő�񐔂��擾
static u8 scEvent_GetHitCount( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, u8 hitCountMax )
{
	fep->hitCountMax = hitCountMax;
	fep->hitCountReal =  BTL_CALC_HitCountMax( hitCountMax );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_HIT_COUNT );
	return fep->hitCountReal;
}


// �N���e�B�J�������N�擾
static u8 scEvent_getCriticalRank( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza )
{
	fep->criticalRank = BTL_POKEPARAM_GetValue( attacker, BPP_CRITICAL_RATIO );
	fep->criticalRank += WAZADATA_GetCriticalRank( waza );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_CRITICAL_RANK );

	if( fep->criticalRank > BTL_CALC_CRITICAL_MAX )
	{
		fep->criticalRank = BTL_CALC_CRITICAL_MAX;
	}

	return fep->criticalRank;
}

// ���U�З͎擾
static u16 scEvent_getWazaPower( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza )
{
	fep->wazaPower = WAZADATA_GetPower( waza );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_POWER );
	return fep->wazaPower;
}

// �U�����\�͒l�擾
static u16 scEvent_getAttackPower( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza )
{
	BppValueID vid;

	switch( fep->wazaDamageType ){
	case WAZADATA_DMG_PHYSIC:	vid = BPP_ATTACK; break;
	case WAZADATA_DMG_SPECIAL:	vid = BPP_SP_ATTACK; break;
	default:
		GF_ASSERT(0);
		vid = BPP_ATTACK;
		break;
	}

	if( fep->criticalFlag )
	{
		fep->attackerPower = BTL_POKEPARAM_GetValueCriticalHit( attacker, vid );
	}
	else
	{
		fep->attackerPower = BTL_POKEPARAM_GetValue( attacker, vid );
	}

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_POWER );

	return fep->attackerPower;
}

// �h�䑤�\�͒l�擾
static u16 scEvent_getDefenderGuard( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender, WazaID waza )
{
	BppValueID vid;

	switch( fep->wazaDamageType ){
	case WAZADATA_DMG_PHYSIC:	vid = BPP_DEFENCE; break;
	case WAZADATA_DMG_SPECIAL:	vid = BPP_SP_DEFENCE; break;
	default:
		GF_ASSERT(0);
		vid = BPP_DEFENCE;
		break;
	}

	if( fep->criticalFlag )
	{
		fep->defenderGuard = BTL_POKEPARAM_GetValueCriticalHit( defender, vid );
	}
	else
	{
		fep->defenderGuard = BTL_POKEPARAM_GetValue( defender, vid );
	}

	if(	(BTL_FIELD_GetWeather() == BTL_WEATHER_SAND)
	&&	(BTL_POKEPARAM_IsMatchType(defender, POKETYPE_IWA))
	&&	(vid == BPP_SP_DEFENCE)
	){
		fep->defenderGuard = (fep->defenderGuard * 192) / 128;
	}

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_GUARD );

	return fep->defenderGuard;
}

// �_���[�W�v�Z�p����擾
static u16 scEvent_getDamageDenom( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep )
{
	fep->damageDenom = 50;
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_DAMAGE_DENOM );
	fep->damageDenom += 2;

	return fep->damageDenom;
}

// ���U�^�C�v�擾
static PokeType scEvent_getWazaPokeType( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, WazaID waza )
{
	fep->wazaPokeType = WAZADATA_GetType( waza );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_WAZA_TYPE );
	return fep->wazaPokeType;
}

// �U�����^�C�v�擾
static PokeTypePair scEvent_getAttackerPokeType( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker )
{
	fep->attackerPokeType = BTL_POKEPARAM_GetPokeType( attacker );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_ATTACKER_TYPE );
	return fep->attackerPokeType;
}

// �h�䑤�^�C�v�擾
static PokeTypePair scEvent_getDefenderPokeType( BTL_SVFLOW_WORK* wk, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender )
{
	fep->defenderPokeType = BTL_POKEPARAM_GetPokeType( defender );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_DEFENDER_TYPE );
	return fep->defenderPokeType;
}

// �����o�[�o�g���ꂩ��ޏo
static void scEvent_MemberOut( BTL_SVFLOW_WORK* wk, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx )
{
	cep->outPokeParam = BTL_POKECON_GetClientPokeData( wk->pokeCon, clientID, posIdx );

	SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_MemberOut1, clientID, posIdx );

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_OUT );
	// �����Ŏ��ʂ��Ƃ�����

	if( !BTL_POKEPARAM_IsDead(cep->outPokeParam) )
	{
		SCQUE_PUT_ACT_MemberOut( wk->que, clientID, posIdx );
	}
}

// �����o�[�V�K�Q��
static void scEvent_MemberIn( BTL_SVFLOW_WORK* wk, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
//	cep->inPokeParam = BTL_MAIN_GetPokeParam
		SCQUE_PUT_ACT_MemberIn( wk->que, clientID, posIdx, nextPokeIdx );
//	SCQUE_PUT_ACT_MemberIn( wk->que, clientID, memberIdx );
}

// �|�P�����o�����������
static void scEvent_PokeComp( BTL_SVFLOW_WORK* wk )
{
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_MEMBER_COMP );
}

//--------------------------------------------------------------------------
/**
 * [Event] �Ώۃ|�P�����̔\�̓����N��������
 *
 * @param   wk					
 * @param   pokeID			�Ώۃ|�P������ID
 * @param   pokePos			
 * @param   statusType	
 * @param   volume			
 * @param   fRandom			�ǉ����ʂȂǗ����ۂɂ���Ĕ��������ꍇ��TRUE
 *
 */
//--------------------------------------------------------------------------
static void scEvent_RankDown( BTL_SVFLOW_WORK* wk, u8 pokeID, BppValueID statusType, u8 volume, BOOL fRandom )
{
	BTL_EVENTVAR_Push();

	BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
	BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, statusType );
	BTL_EVENTVAR_SetValue( BTL_EVAR_RANDOM_FLAG, fRandom );

	BTL_EVENTVAR_SetValue( BTL_EVAL_VOLUME, volume );
	BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );

	BTL_EVENT_CallHandlers( wk, BTL_EVENT_BEFORE_RANKDOWN );

	if( BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_FLAG) == FALSE )
	{
		volume = BTL_EVENTVAR_GetValue( BTL_EVAL_VOLUME );
		BTL_POKEPARAM_RankDown(
				BTL_POKECON_GetPokeParam(wk->pokeCon, pokeID),
				statusType,
				volume
		);
		SCQUE_PUT_OP_RankDown( wk->que, pokeID, statusType, volume );
		SCQUE_PUT_ACT_RankDown( wk->que, pokeID, statusType, volume );
		SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankdown_ATK, pokeID, statusType, volume );
	}

	BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] �Ώۃ|�P�����̔\�̓����N���グ��
 *
 * @param   wk		
 * @param   pp		
 * @param   statusType		
 * @param   volume		
 * @param   fRandom			�ǉ����ʂȂǗ����ۂɂ���Ĕ��������ꍇ��TRUE
 *
 */
//--------------------------------------------------------------------------
static void scEvent_RankUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* pp, BppValueID statusType, u8 volume, BOOL fRandom )
{
	BTL_EVENTVAR_Push();

	{
		u8 pokeID = BTL_POKEPARAM_GetID( pp );

		BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, pokeID );
		BTL_EVENTVAR_SetValue( BTL_EVAR_STATUS_TYPE, statusType );
		BTL_EVENTVAR_SetValue( BTL_EVAR_RANDOM_FLAG, fRandom );

		BTL_EVENTVAR_SetValue( BTL_EVAL_VOLUME, volume );
		BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, FALSE );

		BTL_EVENT_CallHandlers( wk, BTL_EVENT_BEFORE_RANKDOWN );

		if( BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_FLAG) == FALSE )
		{
			volume = BTL_EVENTVAR_GetValue( BTL_EVAL_VOLUME );
			BTL_POKEPARAM_RankUp( pp, statusType, volume );
			SCQUE_PUT_OP_RankUp( wk->que, pokeID, statusType, volume );
			SCQUE_PUT_ACT_RankUp( wk->que, pokeID, statusType, volume );
			SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_Rankup_ATK, pokeID, statusType, volume );
		}
	}

	BTL_EVENTVAR_Pop();
}
//--------------------------------------------------------------------------
/**
 * [Event] �Ώۃ|�P�������Ђ�܂���
 *
 * @param   wk					
 * @param   target			�Ώۃ|�P�����f�[�^
 * @param   per					�Ђ�݊m��
 *
 */
//--------------------------------------------------------------------------
static void scEvent_CheckShrink( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* target, u8 per )
{
	BTL_EVENTVAR_Push();

	BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, per );
	BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID, BTL_POKEPARAM_GetID(target) );
	BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_CHECK );
	per = BTL_EVENTVAR_GetValue( BTL_EVAR_ADD_PER );
	if( perOccur(per) )
	{
		BTL_POKEPARAM_SetShrink( target );
		BTL_EVENT_CallHandlers( wk, BTL_EVENT_SHRINK_FIX );
	}
	BTL_EVENTVAR_Pop();
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

//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���C��
 *
 * @param   wk		
 * @param   clientID		
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_TokuseiWinIn( BTL_SVFLOW_WORK* wk, BtlPokePos pokePos )
{
	SCQUE_PUT_TOKWIN_IN( wk->que, pokePos );
}
//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���A�E�g
 *
 * @param   wk		
 * @param   pokeID		
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_TokuseiWinOut( BTL_SVFLOW_WORK* wk, BtlPokePos pokePos )
{
	SCQUE_PUT_TOKWIN_OUT( wk->que, pokePos );
}

void BTL_SERVER_RECTPT_SetMessage( BTL_SVFLOW_WORK* wk, u16 msgID, BtlPokePos pokePos )
{
	SCQUE_PUT_MSG_SET( wk->que, msgID, pokePos );
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
void BTL_SERVER_RECEPT_RankDownEffect( BTL_SVFLOW_WORK* wk, BtlExPos exPos, BppValueID statusType, u8 volume )
{
	const BTL_POKEPARAM* pp;
	u8 targetPos[ BTL_POSIDX_MAX ];
	u8 numPokemons, pokeID, i;

	numPokemons = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, targetPos );
	BTL_Printf("�����N�������ʁF�^�C�v=%d,  �Ώۃ|�P������=%d\n", statusType, numPokemons );
	for(i=0; i<numPokemons; ++i)
	{
		pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos[i] );
		pokeID = BTL_POKEPARAM_GetID( pp );
		scEvent_RankDown( wk, pokeID, statusType, volume, FALSE );
	}
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
void BTL_SERVER_RECEPT_RankUpEffect( BTL_SVFLOW_WORK* wk, BtlExPos exPos, BppValueID statusType, u8 volume )
{
	BTL_POKEPARAM* pp;
	u8 targetPos[ BTL_POSIDX_MAX ];
	u8 numPokemons, i;

	numPokemons = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, targetPos );
	BTL_Printf("�����N�������ʁF�^�C�v=%d,  �Ώۃ|�P������=%d\n", statusType, numPokemons );
	for(i=0; i<numPokemons; ++i)
	{
		pp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos[i] );
		scEvent_RankUp( wk, pp, statusType, volume, FALSE );
	}
}




//=============================================================================================
/**
 * [�n���h����M] ����PP�l�̏C��
 *
 * @param   wk		
 * @param   volume		�C�����PP
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_SetDecrementPP( BTL_SVFLOW_WORK* wk, u8 volume )
{
	wk->fightEventParams.decPP = volume;
}




