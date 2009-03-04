//=============================================================================================
/**
 * @file	btl_server.c
 * @brief	�|�P����WB �o�g���V�X�e��	�T�[�o���W���[��
 * @author	taya
 *
 * @date	2008.09.06	�쐬
 */
//=============================================================================================
#include <gflib.h>
#include "waza_tool/wazadata.h"

#include "btl_common.h"
#include "btl_client.h"
#include "btl_adapter.h"
#include "btl_action.h"
#include "btl_pokeparam.h"
#include "btl_calc.h"
#include "btl_event.h"
#include "btl_util.h"
#include "btl_string.h"
#include "handler/hand_tokusei.h"

#include "btl_server_cmd.h"
#include "btl_server_local.h"
#include "btl_server.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	SERVER_CMD_SIZE_MAX = 1024,
	CLIENT_DISABLE_ID = 0xff,
};

enum {
	QUITSTEP_NONE = 0,		///
	QUITSTEP_REQ,
	QUITSTEP_CORE,
};


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


//--------------------------------------------------------------
/**
 *	�T�[�o���C�����[�v�֐��^  - �o�g���I�����̂� TRUE ��Ԃ� -
 */
//--------------------------------------------------------------
typedef BOOL (*ServerMainProc)(BTL_SERVER*, int*);



typedef struct {

	BTL_ADAPTER*		adapter;
	BTL_PARTY*			party;
	BTL_POKEPARAM*	member[ TEMOTI_POKEMAX ];
	BTL_POKEPARAM*	frontMember[ BTL_POSIDX_MAX ];
	u8				memberCount;
	u8				numCoverPos;
	u8				isLocalClient;
	u8				myID;

}CLIENT_WORK;

/**
 *	�A�N�V�����D�揇�L�^�\����
 */
typedef struct {
	u8  clientID;		///< �N���C�A���gID
	u8  pokeIdx;		///< ���̃N���C�A���g�́A���̖ځH 0�`
}ACTION_ORDER_WORK;


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
	u8		confFlag;					///< �����񎩖Ńt���O
	u8		confDamage;				///< �����񎩖Ń_���[�W

	u8		hitCountMax;			///< �ő�q�b�g��
	u8		hitCountReal;			///< ���ۃq�b�g��

	u8		decPP;						///< ����PP�l

	PokeSick	sick;					///< ��Ԉُ�
	int				sickDamage;		///< ��Ԉُ�_���[�W�l

}FIGHT_EVENT_PARAM;

typedef struct {

	BTL_POKEPARAM*  inPokeParam;
	BTL_POKEPARAM*  outPokeParam;

}CHANGE_EVENT_PARAM;

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


//--------------------------------------------------------------
/**
 *	
 */
//--------------------------------------------------------------
struct _BTL_SERVER {
	BTL_MAIN_MODULE*	mainModule;
	CLIENT_WORK				client[ BTL_CLIENT_MAX ];
//	u8					actionOrder[ BTL_POS_MAX ];
	ACTION_ORDER_WORK	actOrder[ BTL_POS_MAX ];
	u8					numActPokemon;
	u8					numClient;
	u8					pokeDeadFlag;
	u8					quitStep;
	u8					wazaEffectPuttedFlag;

	ServerMainProc		mainProc;
	int					seq;

	FIGHT_EVENT_PARAM		fightEventParams;
	CHANGE_EVENT_PARAM	changeEventParams;

	BTL_SERVER_CMD_QUE	queBody;
	BTL_SERVER_CMD_QUE*	que;

	int					eventArgs[ BTL_EVARG_MAX ];

	HEAPID				heapID;
	u8						flowFlags[ FLOWFLG_BYTE_MAX ];

};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline void setup_client_members( CLIENT_WORK* client, BTL_PARTY* party, u8 numCoverPos );
static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc );
static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectPokemon( BTL_SERVER* server, int* seq );
static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd );
static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize );
static BOOL WaitAdapterCmd( BTL_SERVER* server );
static void ResetAdapterCmd( BTL_SERVER* server );
static inline void FlowFlg_Set( BTL_SERVER* server, FlowFlag flg );
static inline void FlowFlg_Clear( BTL_SERVER* server, FlowFlag flg );
static inline BOOL FlowFlg_Get( BTL_SERVER* server, FlowFlag flg );
static inline void FlowFlg_ClearAll( BTL_SERVER* server );
static u8 sortClientAction( BTL_SERVER* server, ACTION_ORDER_WORK* order );
static u8 countAlivePokemon( const BTL_SERVER* server );
static BOOL ServerFlow_Start( BTL_SERVER* server );
static BOOL ServerFlow_AfterPokeSelect( BTL_SERVER* server );
static void scput_MemberIn( BTL_SERVER* server, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scput_MemberOut( BTL_SERVER* server, u8 clientID, const BTL_ACTION_PARAM* action );
static void scput_Fight( BTL_SERVER* server, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action );
static void cof_put_wazafail_msg_cmd( BTL_SERVER* server, const BTL_POKEPARAM* attacker, u8 wazaFailReason );
static void scput_Fight_Damage(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action );
static BTL_POKEPARAM* svflowsub_get_opponent_pokeparam( BTL_SERVER* server, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* svflowsub_get_next_pokeparam( BTL_SERVER* server, BtlPokePos pos );
static void svflowsub_check_and_make_dead_command( BTL_SERVER* server, BTL_POKEPARAM* poke );
static void scput_Fight_Damage_Single(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action );
static void scput_Fight_Damage_Enemy2(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action );
static void scput_Fight_Damage_OtherAll(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action );
static void svflowsub_damage_singular( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza );
static void svflowsub_damage_enemy_all( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender1, BTL_POKEPARAM* defender2, WazaID waza );
static void svflowsub_damage_act_singular( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff aff );
static void svflowsub_damage_act_enemy_all( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2, BtlTypeAff aff1, BtlTypeAff aff2, WazaID waza );
static void svflowsub_damage_act_enemy_all_atonce( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2, BtlTypeAff aff, WazaID waza );
static BOOL svflowsub_hitcheck_singular( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza );
static u16 svflowsub_damage_calc_core( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff typeAff );
static void svflowsub_set_waza_effect( BTL_SERVER* server, u8 atPokeID, u8 defPokeID, WazaID waza );
static void scput_TurnCheck( BTL_SERVER* server );
static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SERVER* server );
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SERVER* server, BTL_POKEPARAM** bpp );
static void scput_turncheck_sick( BTL_SERVER* server );
static inline int roundValue( int val, int min, int max );
static void initFightEventParams( FIGHT_EVENT_PARAM* fep );
static BOOL scEvent_CheckConf( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient );
static u16 scEvent_CalcConfDamage( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckWazaExecute( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient );
static BOOL scEvent_checkHit( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BtlTypeAff scEvent_checkAffinity( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_decrementPP( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, u8 wazaIdx );
static u8 scEvent_getHitRatio( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza );
static u8 scEvent_GetHitCount( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, u8 hitCountMax );
static u8 scEvent_getCriticalRank( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza );
static u16 scEvent_getWazaPower( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza );
static u16 scEvent_getAttackPower( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza );
static u16 scEvent_getDefenderGuard( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender, WazaID waza );
static u16 scEvent_getDamageDenom( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep );
static PokeType scEvent_getWazaPokeType( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza );
static PokeTypePair scEvent_getAttackerPokeType( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker );
static PokeTypePair scEvent_getDefenderPokeType( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender );
static void scEvent_MemberOut( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx );
static void scEvent_MemberIn( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx, u8 nextPokeIdx );
static void scEvent_PokeComp( BTL_SERVER* server );
static void scEvent_RankDown( BTL_SERVER* server, u8 pokeID, BtlPokePos pokePos, BppValueID statusType, u8 volume );



//--------------------------------------------------------------------------------------
/**
 * �T�[�o����
 *
 * @param   mainModule		�o�g�����C�����W���[���n���h��
 * @param   heapID			�����p�q�[�vID
 *
 * @retval  BTL_SERVER*		�������ꂽ�T�[�o�n���h��
 */
//--------------------------------------------------------------------------------------
BTL_SERVER* BTL_SERVER_Create( BTL_MAIN_MODULE* mainModule, HEAPID heapID )
{
	BTL_SERVER* sv = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER) );

	sv->mainModule = mainModule;
	sv->numClient = 0;
	sv->heapID = heapID;
	sv->que = &sv->queBody;
	sv->quitStep = QUITSTEP_NONE;

	{
		int i;
		for(i=0; i<BTL_CLIENT_MAX; ++i)
		{
			sv->client[i].myID = CLIENT_DISABLE_ID;
		}
	}

	setMainProc( sv, ServerMain_WaitReady );

	return sv;
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o�폜
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_Delete( BTL_SERVER* wk )
{
	int i;
	for(i=0; i<BTL_CLIENT_MAX; ++i)
	{
		if( (wk->client[i].myID != CLIENT_DISABLE_ID)
		&&	(wk->client[i].isLocalClient == FALSE)
		){
			// ����}�V����ɂ���N���C�A���g�łȂ���΁A
			// �T�[�o���A�_�v�^��p�ӂ����n�Y�Ȃ̂Ŏ����ō폜
			BTL_ADAPTER_Delete( wk->client[i].adapter );
		}
	}
	GFL_HEAP_FreeMemory( wk );
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o�Ɠ���}�V����ɂ���N���C�A���g�Ƃ̃A�_�v�^��ڑ�����
 *
 * @param   server			�T�[�o�n���h��
 * @param   adapter			�A�_�v�^�i���ɃN���C�A���g�Ɋ֘A�t�����Ă���j
 * @param   party				�N���C�A���g�̃p�[�e�B�f�[�^
 * @param   clientID		�N���C�A���gID
 * @param   numCoverPos	�N���C�A���g���󂯎��퓬���|�P������
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_AttachLocalClient( BTL_SERVER* server, BTL_ADAPTER* adapter, BTL_PARTY* party, u8 clientID, u8 numCoverPos )
{
	GF_ASSERT(clientID < BTL_CLIENT_MAX);
	GF_ASSERT(server->client[clientID].adapter==NULL);

	{
		CLIENT_WORK* client;

		client = &server->client[ clientID ];

		client->adapter = adapter;
		client->myID = clientID;
		client->isLocalClient = TRUE;

		setup_client_members( client, party, numCoverPos );
	}

	server->numClient++;
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o�ƈقȂ�}�V����ɂ���N���C�A���g�Ƃ̃A�_�v�^�𐶐�����
 *
 * @param   server			�T�[�o�n���h��
 * @param   commMode		�ʐM���[�h
 * @param   netHandle		�l�b�g���[�N�n���h��
 * @param   clientID		�N���C�A���gID
 * @param   numCoverPos	�N���C�A���g���󂯎��퓬���|�P������
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_ReceptionNetClient( BTL_SERVER* server, BtlCommMode commMode, GFL_NETHANDLE* netHandle, BTL_PARTY* party, u8 clientID, u8 numCoverPos )
{
	GF_ASSERT(clientID < BTL_CLIENT_MAX);
	GF_ASSERT(server->client[clientID].adapter==NULL);

	{
		CLIENT_WORK* client;
		int i;

		client = &server->client[ clientID ];

		client->adapter = BTL_ADAPTER_Create( netHandle, server->heapID, clientID );
		client->myID = clientID;
		client->isLocalClient = FALSE;

		setup_client_members( client, party, numCoverPos );
	}

	server->numClient++;
}
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
BtlPokePos BTL_SERVER_CheckExistFrontPokeID( BTL_SERVER* server, u8 pokeID )
{
	int i;

	for(i=0; i<BTL_CLIENT_MAX; i++)
	{
		if( server->client[i].numCoverPos )
		{
			int p;
			for(p=0; p<server->client[i].numCoverPos; p++)
			{
				if( (server->client[i].frontMember[p] != NULL)
				&&	(BTL_POKEPARAM_GetID(server->client[i].frontMember[p]) == pokeID)
				){
					return BTL_MAIN_GetClientPokePos( server->mainModule, i, p );
				}
			}
		}
	}
	return BTL_POS_MAX;
}


//--------------------------------------------------------------------------
/**
 * �N���C�A���g���[�N�̃����o�[�f�[�^����������
 *
 * @param   client				�N���C�A���g���[�N
 * @param   party					�p�[�e�B�f�[�^
 * @param   numCoverPos		�N���C�A���g���󂯎��퓬���|�P������
 */
//--------------------------------------------------------------------------
static inline void setup_client_members( CLIENT_WORK* client, BTL_PARTY* party, u8 numCoverPos )
{
	u16 i;

	client->party = party;
	client->memberCount = BTL_PARTY_GetMemberCount( party );
	client->numCoverPos = numCoverPos;

	for(i=0; i<client->memberCount; i++)
	{
		client->member[i] = BTL_PARTY_GetMemberData( client->party, i );
	}
	for( ; i<NELEMS(client->member); i++)
	{
		client->member[i] = NULL;
	}

	{
		u16 numFrontPoke = (numCoverPos < client->memberCount)? numCoverPos : client->memberCount;
		for(i=0; i<numFrontPoke; i++)
		{
			client->frontMember[i] = client->member[i];
		}
		for( ; i<NELEMS(client->frontMember); i++)
		{
			client->frontMember[i] = NULL;
		}
	}
}


//--------------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v
 *
 * @param   sv			�T�[�o�n���h��
 *
 * @retval  BOOL		�I���� TRUE ��Ԃ�
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_Main( BTL_SERVER* sv )
{
	if( sv->quitStep != QUITSTEP_CORE )
	{
		if( sv->mainProc( sv, &sv->seq ) )
		{
			SetAdapterCmd( sv, BTL_ACMD_QUIT_BTL );
			sv->quitStep = QUITSTEP_CORE;
		}
	}
	else
	{
		WaitAdapterCmd( sv );
	}
}

static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc )
{
	sv->mainProc = mainProc;
	sv->seq = 0;
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq )
{
	switch( *seq ){
	case 0:
		BTL_EVENT_InitSystem();
		BTL_Printf("�C�j�V�����C�Y�R�}���h���s\n");
		SetAdapterCmd( server, BTL_ACMD_WAIT_INITIALIZE );
		(*seq)++;
	case 1:
		if( WaitAdapterCmd(server) )
		{
			u16 i, p;

			ResetAdapterCmd( server );

			for(i=0; i<server->numClient; i++)
			{
				for(p=0; p<server->client[i].numCoverPos; p++)
				{
					if( server->client[i].frontMember[p] != NULL )
					{
						BTL_HANDLER_TOKUSEI_Add( server->client[i].frontMember[p] );
					}
				}
			}
			setMainProc( server, ServerMain_SelectAction );
		}
		break;
	}

	return FALSE;
}

static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq )
{
	switch( *seq ){
	case 0:
		SCQUE_Init( server->que );
		scEvent_PokeComp( server );
		if( server->que->writePtr )
		{
			BTL_Printf("�Đ��R�}���h���s\n");
			SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
			(*seq) = 1;
		}
		else
		{
			(*seq)=2;
		}
		break;

	case 1:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			(*seq)++;
		}
		break;

	case 2:
		BTL_Printf("�A�N�V�����I���R�}���h���s\n");
		SetAdapterCmd( server, BTL_ACMD_SELECT_ACTION );
		(*seq)++;
		break;

	case 3:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			server->numActPokemon = sortClientAction( server, server->actOrder );
			BTL_Printf("�S�N���C�A���g�̃A�N�V�����\�[�g�����B�����A�N�V������=%d\n", server->numActPokemon);
			SCQUE_Init( server->que );
			server->pokeDeadFlag = ServerFlow_Start( server );
			if( server->pokeDeadFlag )
			{
				BTL_Printf(" **** �|�P�������ɂ܂���\n");
			}
			SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
			(*seq)++;
		}
		break;

	case 4:
		if( WaitAdapterCmd(server) )
		{
			BTL_Printf("�R�}���h�Đ������܂���\n");
			BTL_MAIN_SyncServerCalcData( server->mainModule );
			ResetAdapterCmd( server );
			if( server->quitStep )
			{
				BTL_Printf("�T�[�o�[�I��\n");
				return TRUE;
			}
			else if( server->pokeDeadFlag )
			{
				setMainProc( server, ServerMain_SelectPokemon );
			}
			else
			{
				(*seq) = 2;
			}
		}
		break;
	}
	return FALSE;
}

static BOOL ServerMain_SelectPokemon( BTL_SERVER* server, int* seq )
{
	switch( *seq ){
	case 0:
		{
			u8 loseClientCount;
			int i;

			loseClientCount = 0;

			for(i=0; i<server->numClient; i++)
			{
				if( BTL_PARTY_GetAliveMemberCount(server->client[i].party) == 0 )
				{
					loseClientCount++;
				}
			}

			// ���s
			if( loseClientCount == 0 )
			{
				BTL_Printf("�������N���C�A���g���Ȃ��̂Ŏ��̃|�P�����I����\n");
				SetAdapterCmd( server, BTL_ACMD_SELECT_POKEMON );
				(*seq)++;
			}
			// ����
			else
			{
				// ���s����
				if( loseClientCount == 1 )
				{
					server->quitStep = QUITSTEP_REQ;
				}
				// ��������
				else
				{
					server->quitStep = QUITSTEP_REQ;
				}
				return TRUE;
			}
		}
		break;

	case 1:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			SCQUE_Init( server->que );
			server->pokeDeadFlag = ServerFlow_AfterPokeSelect( server );
			BTL_DUMP_Printf( "[SV]�R�}���h���M", server->que->buffer, server->que->writePtr );
			SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );

			(*seq)++;
		}
		break;

	case 2:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			BTL_MAIN_SyncServerCalcData( server->mainModule );

			if( server->quitStep )
			{
				return TRUE;
			}
			else if( server->pokeDeadFlag )
			{
				(*seq) = 0;
			}
			else
			{
				setMainProc( server, ServerMain_SelectAction );
			}
		}
		break;
	}

	return FALSE;
}



//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd )
{
	SetAdapterCmdEx( server, cmd, NULL, 0 );
}

static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize )
{
	int i;

	BTL_ADAPTERSYS_BeginSetCmd();

	for(i=0; i<server->numClient; i++)
	{
		BTL_ADAPTER_SetCmd( server->client[i].adapter, cmd, sendData, dataSize );
	}

	BTL_ADAPTERSYS_EndSetCmd();
}

static BOOL WaitAdapterCmd( BTL_SERVER* server )
{
	int i;
	BOOL ret = TRUE;
	for(i=0; i<server->numClient; i++)
	{
		if( !BTL_ADAPTER_WaitCmd( server->client[i].adapter ) )
		{
			ret = FALSE;
		}
	}
	return ret;
}

static void ResetAdapterCmd( BTL_SERVER* server )
{
	int i;
	for(i=0; i<server->numClient; i++)
	{
		BTL_ADAPTER_ResetCmd( server->client[i].adapter );
	}
}




//----------------------------------------------------------------------------------------------
// �T�[�o�[�t���[�t���O����
//----------------------------------------------------------------------------------------------

static inline void FlowFlg_Set( BTL_SERVER* server, FlowFlag flg )
{
	server->flowFlags[ flg ] = 1;
}
static inline void FlowFlg_Clear( BTL_SERVER* server, FlowFlag flg )
{
	server->flowFlags[ flg ] = 0;
}
static inline BOOL FlowFlg_Get( BTL_SERVER* server, FlowFlag flg )
{
	return server->flowFlags[ flg ];
}
static inline void FlowFlg_ClearAll( BTL_SERVER* server )
{
	int i;
	for(i=0; i<NELEMS(server->flowFlags); ++i)
	{
		server->flowFlags[ i ] = 0;
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
static u8 sortClientAction( BTL_SERVER* server, ACTION_ORDER_WORK* order )
{
	/*
		�s���D�揇  ... 2BIT
		���U�D�揇  ... 6BIT
		�f����      ... 16BIT
	*/
	#define MakePriValue( actPri, wazaPri, agility )	\
						( ((actPri)<<22) | ((wazaPri)<<16) | (agility) )

	CLIENT_WORK* client;
	const BTL_ACTION_PARAM* actParam;
	u32 pri[ BTL_POS_MAX ];
	u16 agility;
	u8  action, actionPri, wazaPri;
	u8  i, j, p, numPoke;

// �e�N���C�A���g�̃v���C�I���e�B�l�𐶐�
	for(i=0, p=0; i<server->numClient; i++)
	{
		client = &server->client[i];

		actParam = BTL_ADAPTER_GetReturnData( client->adapter );
		numPoke = BTL_ADAPTER_GetReturnDataCount( client->adapter );

		TAYA_Printf("[SV] �A�N�V�����\�[�g, CLIENT<%d> �̃J�o�[����%d, �A��f�[�^����%d\n",
					i, client->numCoverPos, numPoke);

//		for(j=0; j<client->numCoverPos; j++)
		for(j=0; j<numPoke; j++)
		{
			// �s���ɂ��D�揇�i�D��x�����قǐ��l��j
			switch( actParam->gen.cmd ){
			case BTL_ACTION_ESCAPE:	actionPri = 3; break;
			case BTL_ACTION_ITEM:	actionPri = 2; break;
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
				WazaID  w = BTL_POKEPARAM_GetWazaNumber( client->frontMember[j], actParam->fight.wazaIdx );
				wazaPri = WAZADATA_GetPriority( w ) - WAZAPRI_MIN;
			}
			else
			{
				wazaPri = 0;
			}

			// ���΂₳
			agility = BTL_POKEPARAM_GetValue( client->frontMember[j], BPP_AGILITY );

			BTL_Printf("Client{%d-%d}'s actionPri=%d, wazaPri=%d, agility=%d\n",
					i, j, actionPri, wazaPri, agility );

			// �v���C�I���e�B�l�ƃN���C�A���gID��΂ɂ��Ĕz��ɕۑ�
			pri[p] = MakePriValue( actionPri, wazaPri, agility );
			order[p].clientID = i;
			order[p].pokeIdx = j;
			BTL_Printf("Client(%d) PriValue=0x%8x\n", i, pri[i]);

			actParam++;
			p++;
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
static u8 countAlivePokemon( const BTL_SERVER* server )
{
	const CLIENT_WORK* cl;
	u16 i, j;
	u8 cnt = 0;

	for(i=0; i<server->numClient; i++)
	{
		cl = &server->client[i];
		for(j=0; j<cl->numCoverPos; j++)
		{
			if( cl->frontMember[j] )
			{
				if( !BTL_POKEPARAM_IsDead(cl->frontMember[j]) )
				{
					cnt++;
				}
			}
		}
	}
	return cnt;
}
//--------------------------------------------------------------------------
/**
 * �P�^�[�����T�[�o�R�}���h�����i�s���I����j
 *
 * @param   server		
 *
 * @retval  BOOL		�^�[�����A�m���ɂȂ����|�P����������ꍇ��TRUE�^����ȊOFALSE
 */
//--------------------------------------------------------------------------
static BOOL ServerFlow_Start( BTL_SERVER* server )
{
	u16 clientID, pokeIdx;
	u8 numPokeBegin, numPokeAlive;
	u8 i;

	numPokeBegin = countAlivePokemon( server );
	FlowFlg_ClearAll( server );

	for(i=0; i<server->numActPokemon; i++)
	{
		clientID = server->actOrder[i].clientID;
		pokeIdx  = server->actOrder[i].pokeIdx;

		if( !BTL_POKEPARAM_IsDead(server->client[clientID].frontMember[pokeIdx]) )
		{
			const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( server->client[clientID].adapter );
			action += pokeIdx;

			BTL_Printf("Client(%d) �� ", clientID);
			switch( action->gen.cmd ){
			case BTL_ACTION_FIGHT:
				BTL_Printf("�y���������z�������B%d�Ԗڂ̃��U���A%d�Ԃ̑���ɁB\n", action->fight.wazaIdx, action->fight.targetIdx);
				scput_Fight( server, clientID, pokeIdx, action );
				break;
			case BTL_ACTION_ITEM:
				BTL_Printf("�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n", action->item.number, action->item.targetIdx);
				break;
			case BTL_ACTION_CHANGE:
				BTL_Printf("�y�|�P�����z�������B�ʒu%d <- �|�P%d \n", action->change.posIdx, action->change.memberIdx);
				scput_MemberOut( server, clientID, action );
				scput_MemberIn( server, clientID, action->change.posIdx, action->change.memberIdx );
				break;
			case BTL_ACTION_ESCAPE:
				BTL_Printf("�y�ɂ���z�������B\n");
				// @@@ ���͑����ɓ�������悤�ɂ��Ă���
				SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_EscapeSuccess, clientID );
				server->quitStep = QUITSTEP_REQ;
				return FALSE;
			}
		}
	}

	// �^�[���`�F�b�N����
	scput_TurnCheck( server );

	// ���񂾃|�P����������ꍇ�̏���
	numPokeAlive = countAlivePokemon( server );
	BTL_Printf( "�|�P������ %d -> %d ...\n", numPokeBegin, numPokeAlive );
	if( numPokeBegin > numPokeAlive )
	{
		return TRUE;
	}

	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * �P�^�[�����T�[�o�R�}���h�����i�|�P�����I����j
 *
 * @param   server		
 *
 * @retval  BOOL		�^�[�����A�m���ɂȂ����|�P����������ꍇ��TRUE�^����ȊOFALSE
 */
//--------------------------------------------------------------------------
static BOOL ServerFlow_AfterPokeSelect( BTL_SERVER* server )
{
	const BTL_ACTION_PARAM* action;
	CLIENT_WORK* client;
	u16 clientID, posIdx;
	int i, j, actionCnt;

	BTL_Printf("�Ђ񂵃|�P��������ւ��I����̃T�[�o�[�R�}���h����\n");

	for(i=0; i<server->numClient; i++)
	{
		client = &server->client[i];
		action = BTL_ADAPTER_GetReturnData( client->adapter );
		actionCnt = BTL_ADAPTER_GetReturnDataCount( client->adapter );

		for(j=0; j<actionCnt; j++, action++)
		{
			if( action->gen.cmd != BTL_ACTION_CHANGE ){ continue; }
			if( action->change.depleteFlag ){ continue; }

			BTL_Printf("�N���C�A���g(%d)�̃|�P����(�ʒu%d) ���A%d�Ԗڂ̃|�P�Ƃ��ꂩ����\n",
						i, action->change.posIdx, action->change.memberIdx );

			scput_MemberIn( server, i, action->change.posIdx, action->change.memberIdx );
		}
	}

	// @@@ ���͊m����FALSE�����A����ւ������ɂ܂��т��Ƃ��Ŏ��ʂ��Ƃ�����
	return FALSE;
}

static void scput_MemberIn( BTL_SERVER* server, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
	server->client[clientID].frontMember[posIdx] = server->client[clientID].member[ nextPokeIdx ];
	scEvent_MemberIn( server, &server->changeEventParams, clientID, posIdx, nextPokeIdx );
}
static void scput_MemberOut( BTL_SERVER* server, u8 clientID, const BTL_ACTION_PARAM* action )
{
	scEvent_MemberOut( server, &server->changeEventParams, clientID, action->change.posIdx );
}
//==============================================================================
// �T�[�o�[�t���[�F�u���������v���[�g
//==============================================================================
static void scput_Fight( BTL_SERVER* server, u8 attackClientID, u8 posIdx, const BTL_ACTION_PARAM* action )
{
	CLIENT_WORK *atClient;
	BTL_POKEPARAM  *attacker;
	FIGHT_EVENT_PARAM* fep;
	WazaID  waza;

	atClient = &server->client[ attackClientID ];
	attacker = atClient->frontMember[posIdx];

	waza = BTL_POKEPARAM_GetWazaNumber( attacker, action->fight.wazaIdx );

  fep = &server->fightEventParams;
	initFightEventParams( fep );
	fep->waza = waza;
	fep->attackPokeID = BTL_POKEPARAM_GetID( attacker );

// �����񎩔�����
	if( scEvent_CheckConf( server, fep, attacker, atClient ) )
	{
		fep->confDamage = scEvent_CalcConfDamage( server, fep, attacker );
		{
			u16 hp = BTL_POKEPARAM_GetValue( attacker, BPP_HP );
			u8 deadFlag = FALSE;
			if( fep->confDamage >= hp )
			{
				fep->confDamage = hp;
				deadFlag = TRUE;
			}
			BTL_POKEPARAM_HpMinus( attacker, fep->confDamage );
			SCQUE_PUT_OP_HpMinus( server->que, atClient->myID, fep->confDamage );
//			SCQUE_PUT_MSG_ConfOn( server->que, atClient->myID );	@@@ ���b�Z�[�W�܂��ł�
			if( deadFlag )
			{
				u8 pokeID = BTL_POKEPARAM_GetID( attacker );
				SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Dead, pokeID );
				SCQUE_PUT_ACT_Dead( server->que, pokeID );
				return;
			}
		}
	}

// ���U�o����������
	if( !scEvent_CheckWazaExecute(server, fep, attacker, atClient ) )
	{
		cof_put_wazafail_msg_cmd( server, attacker, fep->wazaFailReason );
		return;
	}
	else
	{
		WazaCategory  category;
		BtlPokePos    atPos;

		category = WAZADATA_GetCategory( waza );
		atPos = BTL_MAIN_GetClientPokePos( server->mainModule, atClient->myID, posIdx );

		BTL_Printf("�o���|�P�ʒu=%d, �o�����U=%d, �J�e�S��=%d\n", atPos, waza, category);

		SCQUE_PUT_MSG_WAZA( server->que, atPos, action->fight.wazaIdx );

		switch( category ){
		case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
		case WAZADATA_CATEGORY_DAMAGE_SICK:
		case WAZADATA_CATEGORY_DAMAGE_EFFECT:
		case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
			scput_Fight_Damage( server, fep, waza, atClient, attacker, atPos, action );
			break;

//	case WAZADATA_CATEGORY_ICHIGEKI:
//	case WAZADATA_CATEGORY_BIND:
//	case WAZADATA_CATEGORY_GUARD:
//	case WAZADATA_CATEGORY_FIELD_EFFECT:
//	case WAZADATA_CATEGORY_SIDE_EFFECT:
//	case WAZADATA_CATEGORY_OTHERS:
		default:
//		SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_WazaAvoid );
			SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_WazaFail );
			break;
		}
		scEvent_decrementPP( server, fep, attacker, action->fight.wazaIdx );
	}
}
// ���U�������s���b�Z�[�W�\���R�}���h���Z�b�g
static void cof_put_wazafail_msg_cmd( BTL_SERVER* server, const BTL_POKEPARAM* attacker, u8 wazaFailReason )
{
	u8 pokeID = BTL_POKEPARAM_GetID( attacker );

	switch( wazaFailReason ){
	case SV_WAZAFAIL_NEMURI:
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_NemuriAct, pokeID );
		break;

	case SV_WAZAFAIL_MAHI:
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_MahiAct, pokeID );
		break;

	case SV_WAZAFAIL_KOORI:
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_KoriAct, pokeID );
		break;

	default:
		SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_WazaFail, pokeID );
		break;
	}
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n
//----------------------------------------------------------------------
static void scput_Fight_Damage(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action )
{
	FlowFlg_Clear( server, FLOWFLG_SET_WAZAEFFECT );

	// �V���O��
	if( BTL_MAIN_GetRule(server->mainModule) == BTL_RULE_SINGLE )
	{
		scput_Fight_Damage_Single( server, fep, waza, atClient, attacker, atPos, action );
	}
	// �_�u��
	else
	{
		WazaTarget  targetType = WAZADATA_GetTarget( waza );

		BTL_Printf("�_�u���ł��B���U�i���o�[=%d, ���U�^�[�Q�b�g�^�C�v=%d\n", waza, targetType);

		switch( targetType ) {
		case WAZA_TARGET_SINGLE:				///< �����ȊO�̂P�́i�I���j
		case WAZA_TARGET_SINGLE_ENEMY:	///< �G�P�́i�I���j
		case WAZA_TARGET_RANDOM:				///< �G�����_��
			scput_Fight_Damage_Single( server, fep, waza, atClient, attacker, atPos, action );
			break;

		case WAZA_TARGET_ENEMY2:				///< �G���Q��
			scput_Fight_Damage_Enemy2( server, fep, waza, atClient, attacker, atPos, action );
			break;

		case WAZA_TARGET_OTHER_ALL:			///< �����ȊO�S��
			scput_Fight_Damage_OtherAll( server, fep, waza, atClient, attacker, atPos, action );
			break;

	//	case WAZA_TARGET_ONLY_USER:			///< �����P�̂̂�
	//	case WAZA_TARGET_TEAM_USER:			///< �������w�n
	//	case WAZA_TARGET_TEAM_ENEMY:		///< �G���w�n
	//	case WAZA_TARGET_FIELD:					///< ��Ɍ����i�V��n�Ȃǁj
		default:
//			GF_ASSERT_MSG(0, "illegal targetType!! waza=%d, targetType=%d", waza, targetType);
			return;
		}
	}
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��ʒu���猩�đΐ푊��̃|�P�����f�[�^��Ԃ�
//----------------------------------------------------------------------
static BTL_POKEPARAM* svflowsub_get_opponent_pokeparam( BTL_SERVER* server, BtlPokePos pos, u8 pokeSideIdx )
{
	BtlPokePos targetPos;
	u8 clientID, pokeIdx;

	targetPos = BTL_MAIN_GetOpponentPokePos( server->mainModule, pos, pokeSideIdx );
	BTL_MAIN_BtlPosToClientID_and_PokeIdx( server->mainModule, targetPos, &clientID, &pokeIdx );

	return server->client[ clientID ].frontMember[ pokeIdx ];
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��ʒu���猩�ėׂ̃|�P�����f�[�^��Ԃ�
//----------------------------------------------------------------------
static BTL_POKEPARAM* svflowsub_get_next_pokeparam( BTL_SERVER* server, BtlPokePos pos )
{
	BtlPokePos nextPos;
	u8 clientID, pokeIdx;

	nextPos = BTL_MAIN_GetNextPokePos( server->mainModule, pos );
	BTL_MAIN_BtlPosToClientID_and_PokeIdx( server->mainModule, nextPos, &clientID, &pokeIdx );

	return server->client[ clientID ].frontMember[ pokeIdx ];
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�������F�w��|�P����������ł��玀�S�����R�}���h����
//----------------------------------------------------------------------
static void svflowsub_check_and_make_dead_command( BTL_SERVER* server, BTL_POKEPARAM* poke )
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
static void scput_Fight_Damage_Single(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action )
{
//	BtlPokePos  defPos;
//	u8 defClientID;
	BTL_POKEPARAM* defender;

//	defPos = BTL_MAIN_GetClientPokePos( server->mainModule, atPos, action->fight.targetIdx );
//	BTL_MAIN_GetFrontPokeData( server->mainModule, defPos );
	defender = svflowsub_get_opponent_pokeparam( server, atPos, action->fight.targetIdx );

	svflowsub_damage_singular( server, fep, attacker, defender, waza );
}
//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n > ����Q�̑Ώ�
//----------------------------------------------------------------------
static void scput_Fight_Damage_Enemy2(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action )
{
	BTL_POKEPARAM *defpoke1, *defpoke2;

	defpoke1 = svflowsub_get_opponent_pokeparam( server, atPos, 0 );
	defpoke2 = svflowsub_get_opponent_pokeparam( server, atPos, 1 );

	svflowsub_damage_enemy_all( server, fep, attacker, defpoke1, defpoke2, waza );
}

//----------------------------------------------------------------------
// �T�[�o�[�t���[�F�u���������v> �_���[�W���U�n > �R�́i�����ȊO�j�Ώ�
//----------------------------------------------------------------------
static void scput_Fight_Damage_OtherAll(
	 BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza, CLIENT_WORK* atClient,
	 BTL_POKEPARAM* attacker, BtlPokePos atPos, const BTL_ACTION_PARAM* action )
{
	BTL_POKEPARAM *defF, *defE1, *defE2;

	defF  = svflowsub_get_next_pokeparam( server, atPos );
	defE1 = svflowsub_get_opponent_pokeparam( server, atPos, 0 );
	defE2 = svflowsub_get_opponent_pokeparam( server, atPos, 1 );

	svflowsub_damage_singular( server, fep, attacker, defF, waza );
	svflowsub_damage_enemy_all( server, fep, attacker, defE1, defE2, waza );
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i��ʕ���j
//------------------------------------------------------------------
static void svflowsub_damage_singular( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza )
{
	BtlTypeAff aff;

	if( !svflowsub_hitcheck_singular(server, fep, attacker, defender, waza) )
	{
		return;
	}

	aff = scEvent_checkAffinity( server, fep, attacker, defender, waza );

	svflowsub_damage_act_singular( server, fep, attacker, defender, waza, aff );

	svflowsub_check_and_make_dead_command( server, defender );
	svflowsub_check_and_make_dead_command( server, attacker );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F����S�̃_���[�W�����i��ʕ���j
//------------------------------------------------------------------
static void svflowsub_damage_enemy_all( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender1, BTL_POKEPARAM* defender2, WazaID waza )
{
	u8 hit1, hit2;
	u8 alive1, alive2;

	hit1 = svflowsub_hitcheck_singular(server, fep, attacker, defender1, waza);
	hit2 = svflowsub_hitcheck_singular(server, fep, attacker, defender2, waza);

	alive1 = !BTL_POKEPARAM_IsDead( defender1 );
	alive2 = !BTL_POKEPARAM_IsDead( defender2 );

	// �Q�̂Ƃ��q�b�g
	if( hit1 && hit2 )
	{
		BtlTypeAff aff1, aff2;
		u8 about1, about2;

		aff1 = scEvent_checkAffinity( server, fep, attacker, defender1, waza );
		aff2 = scEvent_checkAffinity( server, fep, attacker, defender2, waza );

		about1 = BTL_CALC_TypeAffAbout( aff1 );
		about2 = BTL_CALC_TypeAffAbout( aff2 );

		// �������ʁX�Ȃ�A�ʂ̏���
		if( about1 != about2 )
		{
			BTL_Printf("�Q�̂Ƃ��Ƀq�b�g���A�����ׂׂ�\n");
			svflowsub_damage_act_enemy_all( server, fep, attacker, defender1, defender2, aff1, aff2, waza );
		}
		// �����������Ȃ�A�̗̓o�[�𓯎��Ɍ��炷�悤�ɃR�}���h��������
		else
		{
			BTL_Printf("�Q�̂Ƃ��Ƀq�b�g���A�������ꏏ\n");
			svflowsub_damage_act_enemy_all_atonce( server, fep, attacker, defender1, defender2, aff1, waza );
		}
	}
	// �ǂ������̂݃q�b�g
	else if( hit1 != !hit2 )
	{
		BTL_POKEPARAM* defender = (hit1)? defender1 : defender2;
		BtlTypeAff aff;

		aff = scEvent_checkAffinity( server, fep, attacker, defender, waza );
		svflowsub_damage_act_singular( server, fep, attacker, defender, waza, aff );
	}
	// �Q�̂Ƃ��n�Y��
	else
	{
		// return;
	}

	// ���S�`�F�b�N�i���U��������Ȃ��Ă������ȂǂŎ��ʃP�[�X�͂���j
	if( alive1 ){
		svflowsub_check_and_make_dead_command( server, defender1 );
	}
	if( alive2 ){
		svflowsub_check_and_make_dead_command( server, defender2 );
	}
	svflowsub_check_and_make_dead_command( server, attacker );

}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̃_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_singular( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff aff )
{
	u8 pluralHit = FALSE;	// ������q�b�g�t���O
	u8 deadFlag = FALSE;
	u8 atPokeID, defPokeID;
	int i;

	fep->hitCountMax = WAZADATA_GetMaxHitCount( waza );
	if( fep->hitCountMax > 1 )
	{
		fep->hitCountReal = scEvent_GetHitCount( server, fep, fep->hitCountMax );
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
		fep->realDamage = svflowsub_damage_calc_core( server, fep, attacker, defender, waza, aff );

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
		SCQUE_PUT_OP_HpMinus( server->que, defPokeID, fep->realDamage );
		svflowsub_set_waza_effect( server, atPokeID, defPokeID, waza );
		SCQUE_PUT_ACT_WazaDamage( server->que, defPokeID, fep->typeAff, fep->realDamage );
		BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_DMG_AFTER );

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

	BTL_Printf("Damage Waza Exe %d times ... plural=%d, dead=%d\n",
			fep->hitCountReal, pluralHit, deadFlag);

	if( pluralHit )
	{
//			SCQUE_PUT_MSG_WazaHitCount( server->que, i );	// @@@ ���Ƃ�
	}
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�G�S�̂����ԂɃ_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_enemy_all( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2, BtlTypeAff aff1, BtlTypeAff aff2, WazaID waza )
{
	u8 atPokeID, defPokeID1, defPokeID2;
	u16 dmg1, dmg2;

	// �����U���̎��A�q�b�g�J�E���g�͂P�Œ�
	fep->hitCountMax = 1;
	fep->hitCountReal = 1;

	atPokeID = BTL_POKEPARAM_GetID( attacker );
	defPokeID1 = BTL_POKEPARAM_GetID( def1 );
	defPokeID2 = BTL_POKEPARAM_GetID( def2 );

	dmg1 = svflowsub_damage_calc_core( server, fep, attacker, def1, waza, aff1 );
	BTL_POKEPARAM_HpMinus( def1, dmg1 );
	SCQUE_PUT_OP_HpMinus( server->que, defPokeID1, fep->realDamage );

	dmg2 = svflowsub_damage_calc_core( server, fep, attacker, def2, waza, aff2 );
	BTL_POKEPARAM_HpMinus( def2, dmg2 );
	SCQUE_PUT_OP_HpMinus( server->que, defPokeID2, fep->realDamage );

	svflowsub_set_waza_effect( server, atPokeID, defPokeID1, waza );
	SCQUE_PUT_ACT_WazaDamage( server->que, defPokeID1, aff1, dmg1 );
	SCQUE_PUT_ACT_WazaDamage( server->que, defPokeID2, aff2, dmg2 );

	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_DMG_AFTER );
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�G�S�̂���ĂɃ_���[�W�����i���ʁj
//------------------------------------------------------------------
static void svflowsub_damage_act_enemy_all_atonce( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep,
		BTL_POKEPARAM* attacker, BTL_POKEPARAM* def1, BTL_POKEPARAM* def2, BtlTypeAff aff, WazaID waza )
{
	u8 atPokeID, defPokeID1, defPokeID2;
	u16 dmg1, dmg2;

	// �����U���̎��A�q�b�g�J�E���g�͂P�Œ�
	fep->hitCountMax = 1;
	fep->hitCountReal = 1;

	atPokeID = BTL_POKEPARAM_GetID( attacker );
	defPokeID1 = BTL_POKEPARAM_GetID( def1 );
	defPokeID2 = BTL_POKEPARAM_GetID( def2 );

	dmg1 = svflowsub_damage_calc_core( server, fep, attacker, def1, waza, aff );
	BTL_POKEPARAM_HpMinus( def1, dmg1 );
	SCQUE_PUT_OP_HpMinus( server->que, defPokeID1, dmg1 );

	dmg2 = svflowsub_damage_calc_core( server, fep, attacker, def2, waza, aff );
	BTL_POKEPARAM_HpMinus( def2, dmg2 );
	SCQUE_PUT_OP_HpMinus( server->que, defPokeID2, dmg2 );

	svflowsub_set_waza_effect( server, atPokeID, defPokeID1, waza );
	SCQUE_PUT_ACT_WazaDamageDbl( server->que, defPokeID1, defPokeID2, dmg1, dmg2, aff );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_DMG_AFTER );
}

//------------------------------------------------------------------
// �T�[�o�[�t���[�������F�P�̕��U���q�b�g�`�F�b�N
//------------------------------------------------------------------
static BOOL svflowsub_hitcheck_singular( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender, WazaID waza )
{
	u8 defPokeID = BTL_POKEPARAM_GetID( defender );

	// ���ɑΏۂ�����ł���
	if( BTL_POKEPARAM_IsDead(defender) )
	{
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_WazaAvoid, defPokeID );
		return FALSE;
	}
	// �������E��𗦂ɂ��q�b�g�`�F�b�N�ŊO�ꂽ
	if( !scEvent_checkHit(server, fep, attacker, defender, waza) )
	{
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_WazaAvoid, defPokeID );
		return FALSE;
	}
	return TRUE;
}
//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �������� > �_���[�W���U�n > �_���[�W�l�v�Z
//------------------------------------------------------------------
static u16 svflowsub_damage_calc_core( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza, BtlTypeAff typeAff )
{
	fep->wazaDamageType = WAZADATA_GetDamageType( waza );

	GF_ASSERT(fep->wazaDamageType != WAZADATA_DMG_NONE);

	fep->criticalRank = scEvent_getCriticalRank( server, fep, attacker, waza );
	fep->criticalFlag = BTL_CALC_CheckCritical( fep->criticalRank );
	fep->wazaPower = scEvent_getWazaPower( server, fep, waza );
	fep->attackerPower = scEvent_getAttackPower( server, fep, attacker, waza );
	fep->defenderGuard = scEvent_getDefenderGuard( server, fep, defender, waza );

	fep->damageDenom = scEvent_getDamageDenom( server, fep );
	{
		u8 attackerLevel = BTL_POKEPARAM_GetValue( attacker, BPP_LEVEL );
		fep->rawDamage = fep->wazaPower * fep->attackerPower * (attackerLevel*2/5+2) / fep->defenderGuard;
		fep->rawDamage /= fep->damageDenom;
		if( fep->criticalFlag )
		{
			fep->rawDamage *= 2;
		}

		BTL_Printf("�З�:%d, ��������:%d, LV:%d, �ڂ�����:%d, ����:%d ... �f�_��:%d\n",
				fep->wazaPower, fep->attackerPower, attackerLevel, fep->defenderGuard, fep->damageDenom, fep->rawDamage
		);
	}
	BTL_EVENT_CallHandlers( server, BTL_EVENT_DAMAGE_PROC1 );

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

	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_DMG_PROC2 );

	return fep->realDamage;
}
//---------------------------------------------------------------------------------------------
// �P�x�̃��U�����ɂāA�G�t�F�N�g�����R�}���h���P�񂵂��쐬���Ȃ����߂̎d�g��
//---------------------------------------------------------------------------------------------
static void svflowsub_set_waza_effect( BTL_SERVER* server, u8 atPokeID, u8 defPokeID, WazaID waza )
{
	if( FlowFlg_Get(server, FLOWFLG_SET_WAZAEFFECT ) == FALSE )
	{
		BTL_Printf("���U�G�t�F�N�g�R�}���h�������܂���, ���U�i���o%d\n", waza);
		SCQUE_PUT_ACT_WazaEffect( server->que, atPokeID, defPokeID, waza );
		FlowFlg_Set( server, FLOWFLG_SET_WAZAEFFECT );
	}
	else
	{
		BTL_Printf("���U�G�t�F�N�g�R�}���h�����ς݂Ȃ̂Ŗ���, ���U�i���o%d\n", waza);
	}
}

//==============================================================================
// �T�[�o�[�t���[�F�^�[���`�F�b�N���[�g
//==============================================================================
static void scput_TurnCheck( BTL_SERVER* server )
{
	scput_turncheck_sick( server );
//	scput_turncheck_weather( server );
}


static void FRONT_POKE_SEEK_InitWork( FRONT_POKE_SEEK_WORK* fpsw, BTL_SERVER* server )
{
	fpsw->clientIdx = 0;
	fpsw->pokeIdx = 0;
	fpsw->endFlag = TRUE;

	{
		CLIENT_WORK* cw;
		u8 i, j;

		for(i=0; i<BTL_CLIENT_MAX; ++i)
		{
			cw = &server->client[i];
			if( cw->myID == CLIENT_DISABLE_ID ){ continue; }

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
static BOOL FRONT_POKE_SEEK_GetNext( FRONT_POKE_SEEK_WORK* fpsw, BTL_SERVER* server, BTL_POKEPARAM** bpp )
{
	if( fpsw->endFlag )
	{
		return FALSE;
	}
	else
	{
		CLIENT_WORK* cw = &server->client[ fpsw->clientIdx ];
		BTL_POKEPARAM* nextPoke = NULL;

		*bpp = cw->frontMember[ fpsw->pokeIdx++ ];

		while( fpsw->clientIdx < BTL_CLIENT_MAX )
		{
			cw = &server->client[ fpsw->clientIdx ];
			while( fpsw->pokeIdx < cw->numCoverPos )
			{
				nextPoke = cw->frontMember[ fpsw->pokeIdx ];
				if( !BTL_POKEPARAM_IsDead(nextPoke) )
				{
					return TRUE;
				}
				fpsw->pokeIdx++;
			}
			fpsw->clientIdx++;
			fpsw->pokeIdx = 0;
		}
		fpsw->endFlag = TRUE;
		return TRUE;
	}
}


//------------------------------------------------------------------
// �T�[�o�[�t���[�������F �^�[���`�F�b�N > ��Ԉُ�
//------------------------------------------------------------------
static void scput_turncheck_sick( BTL_SERVER* server )
{
	FIGHT_EVENT_PARAM* fep;
	FRONT_POKE_SEEK_WORK  fpsw;
	BTL_POKEPARAM* bpp;
	u8 pokeID;

	fep = &server->fightEventParams;
	FRONT_POKE_SEEK_InitWork( &fpsw, server );

	while( FRONT_POKE_SEEK_GetNext( &fpsw, server, &bpp ) )
	{
		pokeID = BTL_POKEPARAM_GetID( bpp );

		fep->sick = BTL_POKEPARAM_GetPokeSick( bpp );
		fep->sickDamage = BTL_POKEPARAM_CalcSickDamage( bpp );
		if( fep->sick != POKESICK_NULL && fep->sickDamage )
		{
			BTL_EVENT_CallHandlers( server, BTL_EVENT_SICK_DAMAGE );
			if( fep->sickDamage > 0 )
			{
				BTL_POKEPARAM_HpMinus( bpp, fep->sickDamage );
				SCQUE_PUT_OP_HpMinus( server->que, pokeID, fep->sickDamage );
				SCQUE_PUT_SickDamage( server->que, pokeID, fep->sick, fep->sickDamage );
			}
		}
		BTL_POKEPARAM_WazaSick_TurnCheck( bpp );
		SCQUE_PUT_OP_WazaSickTurnCheck( server->que, pokeID );
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


//---------------------------------------------------------------------------------------------
// �R�}���h�o��  ��P�K�w
//---------------------------------------------------------------------------------------------

/*
	���U�����邩���� *
		�{��������
		�{������
		�{�܂�
		�{���傤�͂�
		�{�Ђ��
		�{�ӂ�����
		���o���Ȃ��ꍇ�͏I���

	���U�Ώۃ`�F�b�N
		�E�Ώۂ��|�P����      ���`��
		�E�Ώۂ��|�P�����ȊO  ���a��

	�`�E�|�P�����Ώۃ��U
		�E�Ώ۔͈̓`�F�b�N
			�{�P��
			�{�i�_�u���̂݁j�����ȊO�S��
			�{�i�_�u���̂݁j����Q��
			�{�����P��

			���Ώېl��������

			�E���U�����邩���� 
			   �E���U�I�����擾		(*)
			   �E�U�����������擾	(*)
			   �E�h�䑤��𗦎擾	(*)
			   ��������Ȃ��ꍇ�͂`�P��

			�E���U�J�e�S���`�F�b�N
				�E�_���[�W���U    ���`�Q��
				�E��_���[�W���U  ���`�R��


		�`�P�E�|�P�����Ώۃ��U�͂��ꏈ��
			�E�͂���n���h���Ăяo��(*)
			�m������������
			���I��

		�`�Q�E�_���[�W���U�q�b�g����
			�E�N���e�B�J�����擾		(*)
			�E�N���e�B�J������
			�E���U�З͒l�擾			(*)
			�E�U�����\�͒l�擾			(*)
			�E�U�������x���擾
			�E�h�䑤�\�͒l�擾			(*)
			�E�_���[�W����v�Z			(*)
			�E�N���e�B�J���␳
			�E�^�C�v�v�Z�O�_���[�W�␳	(*)
			�E�����_���␳
			�E���U�^�C�v�擾			(*)
			�E�U���|�P�^�C�v�擾		(*)
			�E�h��|�P�^�C�v�擾		(*)
			�E�^�C�v��v�␳
			�E�^�C�v�����擾
			�E�^�C�v�����␳
			�E�^�C�v�v�Z��_���[�W�␳	(*)
			�E�ŏI�␳					(*)
			�E�����v�Z					(*)
			�E�m������������P
			�E�_���[�W�󂯂��n���h��	(*)
			�E�m������������Q
			  �E�����Ȃ灨�I��
			�E�����ւ̒ǉ����ʏ���		(*)
			�E����ւ̒ǉ����ʏ���		(*)
			  �E�m���ɂȂ���   ���b�E�_���[�W�Z�ɂ��m������ ��
			  �E�m���ɂȂ�Ȃ� ���c�E

		�`�R�E��_���[�W���U�q�b�g����
			


	�a�E�|�P�����ȊO�Ώۃ��U
		�E�Ώ۔͈̓`�F�b�N
			�E�w�c
			�E��

	->tmp.c
*/

static void initFightEventParams( FIGHT_EVENT_PARAM* fep )
{
	GFL_STD_MemClear( fep, sizeof(*fep) );
}


//---------------------------------------------------------------------------------------------
// �R�}���h�o��  �C�x���g�Q
//---------------------------------------------------------------------------------------------

// �����񎩔��`�F�b�N
static BOOL scEvent_CheckConf( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient )
{
	if( BTL_POKEPARAM_CheckSick(attacker, WAZASICK_KONRAN) )
	{
//		SCQUE_PUT_MSG_ConfAnnounce( server->que, atClient->myID );	// @@@ 
		fep->confFlag = ( GFL_STD_MtRand(100) < BTL_CALC_CONF_PER );
		return fep->confFlag;
	}
	return FALSE;
}

// �����_���[�W�v�Z
static u16 scEvent_CalcConfDamage( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker )
{
	u16 dmg = BTL_POKEPARAM_GetValue(attacker, BPP_MAX_HP) / 8;

	fep->confDamage = dmg;

	BTL_EVENT_CallHandlers( server, BTL_EVENT_CALC_CONF_DAMAGE );
	return fep->confDamage;
}

// ���U�����邩����
static BOOL scEvent_CheckWazaExecute( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient )
{
	// @@@ �Ђ�݁A�܂ЁA����������
	WazaSick  sick;
	fep->wazaExecuteFlag = TRUE;
	fep->wazaFailReason = SV_WAZAFAIL_NULL;

	do {
		sick = BTL_POKEPARAM_GetPokeSick( attacker );

		switch( sick ){
		case POKESICK_NEMURI:
			if( !BTL_POKEPARAM_Nemuri_CheckWake(attacker) )
			{
				fep->wazaExecuteFlag = FALSE;
				fep->wazaFailReason = SV_WAZAFAIL_NEMURI;
			}
			break;
		case POKESICK_MAHI:
			if( GFL_STD_MtRand(100) < BTL_MAHI_EXE_RATIO )
			{
				fep->wazaExecuteFlag = FALSE;
				fep->wazaFailReason = SV_WAZAFAIL_MAHI;
			}
			break;
		case POKESICK_KOORI:
			if( GFL_STD_MtRand(100) >= BTL_KORI_MELT_RATIO )
			{
				fep->wazaExecuteFlag = FALSE;
				fep->wazaFailReason = SV_WAZAFAIL_KOORI;
			}
			else
			{
				u8 pokeID = BTL_POKEPARAM_GetID( attacker );
				BTL_POKEPARAM_SetPokeSick( attacker, POKESICK_NULL, 0 );
				SCQUE_PUT_OP_SetSick( server->que, pokeID, POKESICK_NULL, 0 );
				SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_KoriMelt, pokeID );
			}
			break;
		}

	}while(0);

	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_EXECUTE );

	return fep->wazaExecuteFlag;
}

// ���U�����邩����
static BOOL scEvent_checkHit( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	if( WAZADATA_IsAlwaysHit(waza) )
	{
		return TRUE;
	}
	else
	{
		u8 wazaHitRatio, per;
		s8 hitRank, avoidRank, totalRank;

		wazaHitRatio = scEvent_getHitRatio(server, fep, waza);

		hitRank = BTL_POKEPARAM_GetValue( attacker, BPP_HIT_RATIO );
		avoidRank = BTL_POKEPARAM_GetValue( defender, BPP_AVOID_RATIO );
		totalRank = roundValue( BTL_CALC_HITRATIO_MID + avoidRank - hitRank, BTL_CALC_HITRATIO_MIN, BTL_CALC_HITRATIO_MAX );

		per = BTL_CALC_HitPer( wazaHitRatio, totalRank );

		return ( GFL_STD_MtRand(100) < (u32)per );
	}
}

// ���U�����`�F�b�N
static BtlTypeAff scEvent_checkAffinity( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	// �^�C�v��v�␳
	fep->wazaPokeType = scEvent_getWazaPokeType( server, fep, waza );
	fep->attackerPokeType = scEvent_getAttackerPokeType( server, fep, attacker );

	if( PokeTypePair_IsMatch(fep->attackerPokeType, fep->wazaPokeType) )
	{
		fep->typeMatchRatio = FX32_CONST(1.5f);
	}
	else
	{
		fep->typeMatchRatio = FX32_CONST(1.0f);
	}
	BTL_EVENT_CallHandlers( server, BTL_EVENT_TYPEMATCH_RATIO );

	// �^�C�v�����v�Z
	fep->defenderPokeType = scEvent_getDefenderPokeType( server, fep, defender );
	fep->typeAff = BTL_CALC_TypeAff( fep->wazaPokeType, fep->defenderPokeType );

	return fep->typeAff;
}

// �g�������U��PP�f�N�������g
static void scEvent_decrementPP( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, u8 wazaIdx )
{
	fep->decPP = 1;

	BTL_EVENT_CallHandlers( server, BTL_EVENT_DECREMENT_PP );

	if( fep->decPP )
	{
		u8 pokeID = BTL_POKEPARAM_GetID( attacker );
		BTL_Printf("DECPP_ �R�}���h�Z�b�g, value=%d\n", fep->decPP);
		SCQUE_PUT_OP_PPMinus( server->que, pokeID, wazaIdx, fep->decPP );
	}

}

// ���U�I�����擾
static u8 scEvent_getHitRatio( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza )
{
	fep->wazaHitRatio = WAZADATA_GetHitRatio( waza );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_HIT_RATIO );
	return fep->wazaHitRatio;
}

// �q�b�g����ő�񐔂��擾
static u8 scEvent_GetHitCount( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, u8 hitCountMax )
{
	fep->hitCountMax = hitCountMax;
	fep->hitCountReal =  BTL_CALC_HitCountMax( hitCountMax );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_HIT_COUNT );
	return fep->hitCountReal;
}


// �N���e�B�J�������N�擾
static u8 scEvent_getCriticalRank( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza )
{
	fep->criticalRank = BTL_POKEPARAM_GetValue( attacker, BPP_CRITICAL_RATIO );
	fep->criticalRank += WAZADATA_GetCriticalRank( waza );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_CRITICAL_RANK );

	if( fep->criticalRank > BTL_CALC_CRITICAL_MAX )
	{
		fep->criticalRank = BTL_CALC_CRITICAL_MAX;
	}

	return fep->criticalRank;
}

// ���U�З͎擾
static u16 scEvent_getWazaPower( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza )
{
	fep->wazaPower = WAZADATA_GetPower( waza );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_POWER );
	return fep->wazaPower;
}

// �U�����\�͒l�擾
static u16 scEvent_getAttackPower( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, WazaID waza )
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

	BTL_EVENT_CallHandlers( server, BTL_EVENT_ATTACKER_POWER );

	return fep->attackerPower;
}

// �h�䑤�\�͒l�擾
static u16 scEvent_getDefenderGuard( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender, WazaID waza )
{
	BppValueID vid;

	switch( fep->wazaDamageType ){
	case WAZADATA_DMG_PHYSIC:	vid = BPP_DEFENCE; break;
	case WAZADATA_DMG_SPECIAL:	vid = BPP_SP_DEFENCE; break;
	default:
		GF_ASSERT(0);
		vid = BPP_ATTACK;
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

	BTL_EVENT_CallHandlers( server, BTL_EVENT_DEFENDER_GUARD );

	return fep->defenderGuard;
}

// �_���[�W�v�Z�p����擾
static u16 scEvent_getDamageDenom( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep )
{
	fep->damageDenom = 50;
	BTL_EVENT_CallHandlers( server, BTL_EVENT_DAMAGE_DENOM );
	fep->damageDenom += 2;

	return fep->damageDenom;
}

// ���U�^�C�v�擾
static PokeType scEvent_getWazaPokeType( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, WazaID waza )
{
	fep->wazaPokeType = WAZADATA_GetType( waza );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_TYPE );
	return fep->wazaPokeType;
}

// �U�����^�C�v�擾
static PokeTypePair scEvent_getAttackerPokeType( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker )
{
	fep->attackerPokeType = BTL_POKEPARAM_GetPokeType( attacker );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_ATTACKER_TYPE );
	return fep->attackerPokeType;
}

// �h�䑤�^�C�v�擾
static PokeTypePair scEvent_getDefenderPokeType( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* defender )
{
	fep->defenderPokeType = BTL_POKEPARAM_GetPokeType( defender );
	BTL_EVENT_CallHandlers( server, BTL_EVENT_DEFENDER_TYPE );
	return fep->defenderPokeType;
}

// �����o�[�o�g���ꂩ��ޏo
static void scEvent_MemberOut( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx )
{
	cep->outPokeParam = BTL_MAIN_GetClientPokeData( server->mainModule, clientID, posIdx );

	SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_MemberOut1, clientID, posIdx );

	BTL_EVENT_CallHandlers( server, BTL_EVENT_MEMBER_OUT );
	// �����Ŏ��ʂ��Ƃ�����

	if( !BTL_POKEPARAM_IsDead(cep->outPokeParam) )
	{
		SCQUE_PUT_ACT_MemberOut( server->que, clientID, posIdx );
	}
}

// �����o�[�V�K�Q��
static void scEvent_MemberIn( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 posIdx, u8 nextPokeIdx )
{
//	cep->inPokeParam = BTL_MAIN_GetPokeParam
		SCQUE_PUT_ACT_MemberIn( server->que, clientID, posIdx, nextPokeIdx );
//	SCQUE_PUT_ACT_MemberIn( server->que, clientID, memberIdx );
}

// �|�P�����o�����������
static void scEvent_PokeComp( BTL_SERVER* server )
{
	BTL_EVENT_CallHandlers( server, BTL_EVENT_MEMBER_COMP );
}

// �\�͂�������
static void scEvent_RankDown( BTL_SERVER* server, u8 pokeID, BtlPokePos pokePos, BppValueID statusType, u8 volume )
{
	server->eventArgs[ BTL_EVARG_COMMON_POKEPOS ] = pokePos;
	server->eventArgs[ BTL_EVARG_COMMON_POKEID ] = pokeID;
	server->eventArgs[ BTL_EVARG_RANKDOWN_STATUS_TYPE ] = statusType;
	server->eventArgs[ BTL_EVARG_RANKDOWN_VOLUME ] = volume;
	server->eventArgs[ BTL_EVARG_RANKDOWN_FAIL_FLAG ] = FALSE;

	BTL_EVENT_CallHandlers( server, BTL_EVENT_BEFORE_RANKDOWN );

	if( server->eventArgs[ BTL_EVARG_RANKDOWN_FAIL_FLAG ] == FALSE )
	{
		volume = server->eventArgs[ BTL_EVARG_RANKDOWN_VOLUME ];
		BTL_POKEPARAM_RankDown(
//				server->client[targetClientID].frontMember,
				BTL_MAIN_GetFrontPokeData( server->mainModule, pokePos ),
				statusType, 
				volume
		);

		SCQUE_PUT_OP_RankDown( server->que, pokePos, statusType, volume );
		SCQUE_PUT_ACT_RankDown( server->que, pokePos, statusType, volume );
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Rankdown_ATK, pokePos, statusType, volume );
	}
}

//----------------------------------------------------------------------------------------------------------
// �ȉ��A�n���h������̉�����M�֐��ƃ��[�e�B���e�B�Q
//----------------------------------------------------------------------------------------------------------

int BTL_SERVER_RECEPT_GetEventArg( const BTL_SERVER* server, u8 idx )
{
	GF_ASSERT(idx < BTL_EVARG_MAX);
	return server->eventArgs[ idx ];
}

void BTL_SERVER_RECEPT_SetEventArg( BTL_SERVER* server, u8 idx, int value )
{
	GF_ASSERT(idx < BTL_EVARG_MAX);
	server->eventArgs[ idx ] = value;
}

//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���C��
 *
 * @param   server		
 * @param   clientID		
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_TokuseiWinIn( BTL_SERVER* server, u8 clientID )
{
	SCQUE_PUT_TOKWIN_IN( server->que, clientID );
}
//=============================================================================================
/**
 * �Ƃ������E�B���h�E�\���A�E�g
 *
 * @param   server		
 * @param   pokeID		
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_TokuseiWinOut( BTL_SERVER* server, u8 clientID )
{
	SCQUE_PUT_TOKWIN_OUT( server->que, clientID );
}

void BTL_SERVER_RECTPT_SetMessage( BTL_SERVER* server, u16 msgID, BtlPokePos pokePos )
{
	SCQUE_PUT_MSG_SET( server->que, msgID, pokePos );
}

//=============================================================================================
/**
 * [�n���h����M] �X�e�[�^�X�̃����N�_�E������
 *
 * @param   server			
 * @param   exPos					�Ώۃ|�P�����ʒu
 * @param   statusType		�X�e�[�^�X�^�C�v
 * @param   volume		
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_RankDownEffect( BTL_SERVER* server, BtlExPos exPos, BppValueID statusType, u8 volume )
{
	const BTL_POKEPARAM* pp;
	u8 targetPos[ BTL_POSIDX_MAX ];
	u8 numPokemons, pokeID, i;

	numPokemons = BTL_MAIN_ExpandBtlPos( server->mainModule, exPos, targetPos );
	BTL_Printf("�����N�������ʁF�^�C�v=%d,  �Ώۃ|�P������=%d\n", statusType, numPokemons );
	for(i=0; i<numPokemons; i++)
	{
		pp = BTL_MAIN_GetFrontPokeData( server->mainModule, targetPos[i] );
		pokeID = BTL_POKEPARAM_GetID( pp );
		scEvent_RankDown( server, pokeID, targetPos[i], statusType, volume );
	}
}

//=============================================================================================
/**
 * [�n���h����M] ����PP�l�̏C��
 *
 * @param   server		
 * @param   volume		�C�����PP
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_SetDecrementPP( BTL_SERVER* server, u8 volume )
{
	server->fightEventParams.decPP = volume;
}


