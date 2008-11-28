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
#include "btl_server_cmd.h"
#include "btl_util.h"
#include "btl_string.h"
#include "handler/hand_tokusei.h"

#include "btl_server.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	SERVER_CMD_SIZE_MAX = 1024,
};

//--------------------------------------------------------------
/**
 *	�T�[�o���C�����[�v�֐��^  - �o�g���I�����̂� TRUE ��Ԃ� -
 */
//--------------------------------------------------------------
typedef BOOL (*ServerMainProc)(BTL_SERVER*, int*);



typedef struct {

	BTL_ADAPTER*	adapter;
	BTL_PARTY*		party;
	BTL_POKEPARAM*	member[ TEMOTI_POKEMAX ];
	BTL_POKEPARAM*	frontMember;
	u8				frontMemberIdx;
	u8				memberCount;
	u8				myID;

}CLIENT_WORK;


typedef struct {

	WazaID	waza;				///< �o�������UID

	u8		attackPokeID;		///< �U�����|�PID
	u8		defencePokeID;		///< �󂯑��|�PID

	u8		wazaExecuteFlag;	///< ���U���s�ł������t���O
	u8		wazaHitRatio;		///< ���U�����m��
//	u8		attackHitRank;
//	u8		defenceHitRank;
	u8		wazaPower;			///< ���U�З�
	u8		wazaDamageType;		///< �_���[�W�^�C�v�i�����E����j
	u8		criticalRank;		///< �N���e�B�J����
	u8		criticalFlag;		///< �N���e�B�J���t���O
	u8		wazaPokeType;		///< ���U�̃|�P�^�C�v
	u8		typeAff;			///< ���U�Ǝ󂯑��|�P�̑���
	PokeTypePair	attackerPokeType;	///< �U�����|�P�^�C�v
	PokeTypePair	defenderPokeType;	///< �󂯑��|�P�^�C�v
	u16		attackerPower;		///< �U�����̔\�͒l�i��������or�Ƃ������j
	u16		defenderGuard;		///< �h�䑤�̔\�͒l�i�ڂ�����or�Ƃ��ڂ��j
	u16		damageDenom;		///< �_���[�W�v�Z�p�̕ꐔ
	u32		rawDamage;			///< �^�C�v�v�Z�O�̑f�_���[�W
	u32		realDamage;			///< �^�C�v�v�Z��̎��_���[�W
	u32		refrectDamage;		///< �U�����̔����_���[�W
	fx32	typeMatchRatio;		///< �U�����|�P�ƃ��U�̃^�C�v��v�ɂ��_���[�W�{��(1.0 or 1.5�j

	u8		wazaFailReason;		///< ���U���s���R
	u8		confFlag;			///< �����񎩖Ńt���O
	u8		confDamage;			///< �����񎩖Ń_���[�W

	u8		hitCountMax;		///< �ő�q�b�g��
	u8		hitCountReal;		///< ���ۃq�b�g��

}FIGHT_EVENT_PARAM;

typedef struct {

	BTL_POKEPARAM*  inPokeParam;
	BTL_POKEPARAM*  outPokeParam;

}CHANGE_EVENT_PARAM;


//--------------------------------------------------------------
/**
 *	
 */
//--------------------------------------------------------------
struct _BTL_SERVER {
	BTL_MAIN_MODULE*	mainModule;
	CLIENT_WORK			client[ BTL_CLIENT_MAX ];
	u8					actionOrder[ BTL_CLIENT_MAX ];
	u8					numClient;
	u8					pokeDeadFlag;
	u8					endFlag;

	ServerMainProc		mainProc;
	int					seq;

	FIGHT_EVENT_PARAM	fightEventParams;
	CHANGE_EVENT_PARAM	changeEventParams;

	BTL_SERVER_CMD_QUE	queBody;
	BTL_SERVER_CMD_QUE*	que;

	int					eventArgs[ BTL_EVARG_MAX ];

	HEAPID				heapID;



};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static BOOL callMainProc( BTL_SERVER* sv );
static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc );
static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectPokemon( BTL_SERVER* server, int* seq );
static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd );
static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize );
static BOOL WaitAdapterCmd( BTL_SERVER* server );
static void ResetAdapterCmd( BTL_SERVER* server );
static void sortClientAction( BTL_SERVER* server, u8* order );
static u8 countAlivePokemon( const BTL_SERVER* server );
static BOOL createServerCommand( BTL_SERVER* server );
static BOOL createServerCommandPokeSelect( BTL_SERVER* server );
static void scput_ChangePokemon( BTL_SERVER* server, u8 clientID, u8 pokeIdx );
static void scput_Fight( BTL_SERVER* server, u8 attackClientID, const BTL_ACTION_PARAM* action );
static inline int roundValue( int val, int min, int max );
static void initFightEventParams( FIGHT_EVENT_PARAM* fep );
static void scput_FightRoot( BTL_SERVER* server, const CLIENT_WORK* atClient, const BTL_ACTION_PARAM* action, WazaID waza );
static void scPut_FightSingleDmg( BTL_SERVER* server,FIGHT_EVENT_PARAM* fep,
	 const CLIENT_WORK* atClient, const CLIENT_WORK* defClient,
	 BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
	 WazaID waza, u8 wazaIdx );
static BOOL sc_fight_layer1_calc_damage( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static u16 sc_fight_layer1_single_dmg( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void sc_fight_layer1_single_not_dmg( BTL_SERVER* server, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static BOOL scEvent_CheckConf( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient );
static u16 scEvent_CalcConfDamage( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker );
static BOOL scEvent_CheckWazaExecute( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient );
static BOOL scEvent_checkHit( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
static void scEvent_checkAffinity( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza );
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
static void scEvent_MemberOut( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID );
static void scEvent_MemberIn( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 memberIdx );
static void scEvent_PokeComp( BTL_SERVER* server );
static void scEvent_RankDown( BTL_SERVER* server, u8 targetClientID, BppValueID statusType, u8 volume );
static inline u8 pokeID_to_clientID( const BTL_SERVER* sv, u8 pokeID );



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
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------
/**
 * �T�[�o�Ɠ���}�V����ɂ���N���C�A���g�Ƃ̃A�_�v�^��ڑ�����
 *
 * @param   server			�T�[�o�n���h��
 * @param   adapter			�A�_�v�^�i���ɃN���C�A���g�Ɋ֘A�t�����Ă���j
 * @param   clientID		�N���C�A���gID
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_AttachLocalClient( BTL_SERVER* server, BTL_ADAPTER* adapter, BTL_PARTY* party, u16 clientID )
{
	GF_ASSERT(server->client[clientID].adapter==NULL);

	{
		CLIENT_WORK* client;
		int i;

		client = &server->client[ clientID ];

		client->adapter = adapter;
		client->party = party;
		client->memberCount = BTL_PARTY_GetMemberCount( client->party );
		for(i=0; i<client->memberCount; i++)
		{
			client->member[i] = BTL_PARTY_GetMemberData( client->party, i );
		}
		client->frontMemberIdx = 0;
		client->frontMember = client->member[0];
		client->myID = clientID;
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
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_ReceptionNetClient( BTL_SERVER* server, BtlCommMode commMode, GFL_NETHANDLE* netHandle, u16 clientID )
{
	GF_ASSERT(server->client[clientID].adapter==NULL);

	server->client[clientID].adapter = BTL_ADAPTER_Create( commMode, netHandle, server->heapID, clientID );
}



//--------------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v
 *
 * @param   server		�T�[�o�n���h��
 *
 * @retval  BOOL		�I���� TRUE ��Ԃ�
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SERVER_Main( BTL_SERVER* server )
{
	return callMainProc( server );
}

static BOOL callMainProc( BTL_SERVER* sv )
{
	return sv->mainProc( sv, &sv->seq );
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
		SetAdapterCmd( server, BTL_ACMD_NOTIFY_POKEDATA );
		(*seq)++;
		break;
	case 1:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			SetAdapterCmd( server, BTL_ACMD_WAIT_INITIALIZE );
			(*seq)++;
		}
		break;
	case 2:
		if( WaitAdapterCmd(server) )
		{
			int i;

			ResetAdapterCmd( server );

			for(i=0; i<server->numClient; i++)
			{
				BTL_HANDLER_TOKUSEI_Add( server->client[i].frontMember );
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
		BTL_Printf("Start to Action Select\n");
		SetAdapterCmd( server, BTL_ACMD_SELECT_ACTION );
		server->endFlag = FALSE;
		(*seq)++;
		break;

	case 3:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			sortClientAction( server, server->actionOrder );
			SCQUE_Init( server->que );
			server->pokeDeadFlag = createServerCommand( server );
			if( server->pokeDeadFlag )
			{
				BTL_Printf("�|�P�������ɂ܂���\n");
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
			if( server->endFlag )
			{
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
					server->endFlag = TRUE;
				}
				// ��������
				else
				{
					server->endFlag = TRUE;
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
			server->pokeDeadFlag = createServerCommandPokeSelect( server );
			SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
			(*seq)++;
		}
		break;

	case 2:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			if( server->endFlag )
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
	for(i=0; i<server->numClient; i++)
	{
		BTL_ADAPTER_SetCmd( server->client[i].adapter, cmd, sendData, dataSize );
	}
}

static BOOL WaitAdapterCmd( BTL_SERVER* server )
{
	int i;
	for(i=0; i<server->numClient; i++)
	{
		if( !BTL_ADAPTER_WaitCmd( server->client[i].adapter ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

static void ResetAdapterCmd( BTL_SERVER* server )
{
	int i;
	for(i=0; i<server->numClient; i++)
	{
		BTL_ADAPTER_ResetCmd( server->client[i].adapter );
	}
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * �N���C�A���g�̃A�N�V�������`�F�b�N���A�����������m��
 *
 * @param   server		
 * @param   order		�������鏇�ԂɃN���C�A���gID����ג����Ċi�[���邽�߂̔z��
 *
 */
//--------------------------------------------------------------------------
static void sortClientAction( BTL_SERVER* server, u8* order )
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
	u32 pri[ BTL_CLIENT_MAX ];
	u16 agility;
	u8  action, actionPri, wazaPri;
	u8  i, j;

// �e�N���C�A���g�̃v���C�I���e�B�l�𐶐�
	for(i=0; i<server->numClient; i++)
	{
		client = &server->client[i];
		// �s���ɂ��D�揇�i�D��x�����قǐ��l��j
		actParam = BTL_ADAPTER_GetReturnData( client->adapter );
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
			WazaID  w = BTL_POKEPARAM_GetWazaNumber( client->frontMember, actParam->fight.wazaIdx );
			wazaPri = WAZADATA_GetPriority( w ) - WAZAPRI_MIN;
		}
		else
		{
			wazaPri = 0;
		}

		// ���΂₳
		agility = BTL_POKEPARAM_GetValue( client->frontMember, BPP_AGILITY );

		BTL_Printf("[SV] Client[%d]'s actionPri=%d, wazaPri=%d, agility=%d\n",
				i, actionPri, wazaPri, agility );

		// �v���C�I���e�B�l�ƃN���C�A���gID��΂ɂ��Ĕz��ɕۑ�
		pri[i] = MakePriValue( actionPri, wazaPri, agility );
		order[i] = i;
		BTL_Printf("[SV] Client[%d] PriValue=%8x\n", i, pri[i]);
	}
 
// �v���C�I���e�B�l�\�[�g�ɔ����ăN���C�A���gID�z����\�[�g
	for(i=0; i<server->numClient; i++)
	{
		for(j=i+1; j<server->numClient; j++)
		{
			if( pri[i] < pri[j] )
			{
				u32 t = pri[i];
				pri[i] = pri[j];
				pri[j] = t;

				t = order[i];
				order[i] = order[j];
				order[j] = t;
			}
		}
	}

// �S�������v���C�I���e�B�l���������烉���_���V���b�t��
// @@@ ������


}

static u8 countAlivePokemon( const BTL_SERVER* server )
{
	u8 cnt = 0;
	int i;
	for(i=0; i<server->numClient; i++)
	{
		if( BTL_POKEPARAM_GetValue( server->client[i].frontMember, BPP_HP ) )
		{
			cnt++;
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
static BOOL createServerCommand( BTL_SERVER* server )
{
	int i, clientID;
	u8 numPokeBegin, numPokeAlive;

	numPokeBegin = countAlivePokemon( server );

	for(i=0; i<server->numClient; i++)
	{
		clientID = server->actionOrder[ i ];

		if( BTL_POKEPARAM_GetValue(server->client[clientID].frontMember, BPP_HP) )
		{
			const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( server->client[clientID].adapter );
			BTL_Printf("Client[%d] �� ", clientID);
			switch( action->gen.cmd ){
			case BTL_ACTION_FIGHT:
				BTL_Printf("�y���������z�������B%d�Ԗڂ̃��U���A%d�Ԃ̑���ɁB\n", action->fight.wazaIdx, action->fight.targetIdx);
				scput_Fight( server, clientID, action );
				break;
			case BTL_ACTION_ITEM:
				BTL_Printf("�y�ǂ����z�������B�A�C�e��%d���A%d�Ԃ̑���ɁB\n", action->item.number, action->item.targetIdx);
				break;
			case BTL_ACTION_CHANGE:
				BTL_Printf("�y�|�P�����z�������B%d�Ԃ̑���ƁB\n", action->change.memberIdx);
				scput_ChangePokemon( server, clientID, action->change.memberIdx );
				break;
			case BTL_ACTION_ESCAPE:
				BTL_Printf("�y�ɂ���z�������B\n");
				SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_EscapeSuccess, clientID );
				server->endFlag = TRUE;
				break;
			}
		}
	}

	// ���񂾃|�P����������ꍇ�̏���
	numPokeAlive = countAlivePokemon( server );
	if( numPokeBegin > numPokeAlive )
	{
		return TRUE;
	}

	return FALSE;
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
static BOOL createServerCommandPokeSelect( BTL_SERVER* server )
{
	int i;

	for(i=0; i<server->numClient; i++)
	{
		if( BTL_POKEPARAM_GetValue(server->client[i].frontMember, BPP_HP) == 0 )
		{
			const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( server->client[i].adapter );

			GF_ASSERT(action->gen.cmd == BTL_ACTION_CHANGE);

			scput_ChangePokemon( server, i, action->change.memberIdx );
		}
	}

	// @@@ ���͊m����FALSE�����A����ւ������ɂ܂��т��Ƃ��Ŏ��ʂ��Ƃ�����
	return FALSE;
}

static void scput_ChangePokemon( BTL_SERVER* server, u8 clientID, u8 pokeIdx )
{
	server->client[clientID].frontMemberIdx = pokeIdx;
	server->client[clientID].frontMember = server->client[clientID].member[ pokeIdx ];
	scEvent_MemberIn( server, &server->changeEventParams, clientID, pokeIdx );
}




static void scput_Fight( BTL_SERVER* server, u8 attackClientID, const BTL_ACTION_PARAM* action )
{
	const CLIENT_WORK *atClient;
	const BTL_POKEPARAM  *attacker, *defender;
	WazaID  waza;

	atClient = &server->client[ attackClientID ];
	waza = BTL_POKEPARAM_GetWazaNumber( atClient->frontMember, action->fight.wazaIdx );

	switch( WAZADATA_GetCategory(waza) ){
	case WAZADATA_CATEGORY_SIMPLE_DAMAGE:
	case WAZADATA_CATEGORY_SIMPLE_SICK:
	case WAZADATA_CATEGORY_SIMPLE_EFFECT:
	case WAZADATA_CATEGORY_SIMPLE_RECOVER:
	case WAZADATA_CATEGORY_DAMAGE_SICK:
	case WAZADATA_CATEGORY_DAMAGE_EFFECT:
	case WAZADATA_CATEGORY_DAMAGE_EFFECT_USER:
	case WAZADATA_CATEGORY_DAMAGE_RECOVER:
	case WAZADATA_CATEGORY_ICHIGEKI:
	case WAZADATA_CATEGORY_BIND:
	case WAZADATA_CATEGORY_GUARD:
	case WAZADATA_CATEGORY_WEATHER:
	case WAZADATA_CATEGORY_OTHERS:
	default:
		initFightEventParams( &server->fightEventParams );
		scput_FightRoot( server, atClient, action, waza );
//		sc_fight_layer1_single( server, atClient, defClient, action, waza );
		break;

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

//
static void scput_FightRoot( BTL_SERVER* server, const CLIENT_WORK* atClient, const BTL_ACTION_PARAM* action, WazaID waza )
{
	FIGHT_EVENT_PARAM* fep = &server->fightEventParams;
	BTL_POKEPARAM* attacker = atClient->frontMember;

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
				SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Dead, atClient->myID );
				SCQUE_PUT_ACT_Dead( server->que, atClient->myID );
				return;
			}
		}
	}

	// ���U�o����������
	if( !scEvent_CheckWazaExecute(server, fep, attacker, atClient ) )
	{
//		SCQUE_PUT_MSG_WazaFail( server->que, atClient->myID, fep->wazaFailReason );
		SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_WazaAvoid );
		return;
	}
	else
	{
		CLIENT_WORK* defClient;
		BTL_POKEPARAM* defender;
		u8 defClientID;

		defClientID = BTL_MAIN_GetOpponentClientID( server->mainModule, atClient->myID, action->fight.targetIdx );
		defClient = &server->client[ defClientID ];
		defender = defClient->frontMember;

		BTL_Printf("SetWazaExeData atClientID=%d, defClientID=%d, waza=%d\n",
			atClient->myID, defClientID, BTL_POKEPARAM_GetWazaNumber(attacker, action->fight.wazaIdx)
		);
		SCQUE_PUT_DATA_WazaExe( server->que, atClient->myID, action->fight.wazaIdx, 1, defClient->myID, 0, 0 );
		SCQUE_PUT_MSG_WAZA( server->que, atClient->myID, action->fight.wazaIdx );

		scPut_FightSingleDmg( server, fep, atClient, defClient, attacker, defender, waza, action->fight.wazaIdx );
	}
}

// �V���O�����P�K�w
static void scPut_FightSingleDmg( BTL_SERVER* server,FIGHT_EVENT_PARAM* fep,
	 const CLIENT_WORK* atClient, const CLIENT_WORK* defClient,
	 BTL_POKEPARAM* attacker, BTL_POKEPARAM* defender,
	 WazaID waza, u8 wazaIdx )
{
// �q�b�g�`�F�b�N
	// �͂��ꂽ
	if( !scEvent_checkHit(server, fep, attacker, defender, waza) )
	{
		SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_WazaAvoid );
		return;
	}
	// ��������
	else
	{
		// �_���[�W���U
		if( WAZADATA_IsDamage(waza) )
		{
			u8 pluralHit, deadFlag, i;

			scEvent_checkAffinity( server, fep, attacker, defender, waza );

			fep->hitCountMax = WAZADATA_GetMaxHitCount( waza );
			if( fep->hitCountMax > 1 )
			{
				fep->hitCountReal = scEvent_GetHitCount( server, fep, fep->hitCountMax );
				pluralHit = TRUE;
			}
			else
			{
				fep->hitCountReal = 1;
				pluralHit = FALSE;
			}

			deadFlag = FALSE;
			for(i=0; i<fep->hitCountReal; i++)
			{
				fep->realDamage = sc_fight_layer1_single_dmg( server, fep, attacker, defender, waza );
//				scEvent_WazaDamage( server, fep, attacker, defender , waza );

				// �f�o�b�O���ȒP�ɂ��邽�ߕK����_���[�W�ɂ���[�u
				#ifdef PM_DEBUG
				if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
				{
					if( !(defClient->myID & 1) )
					{
						fep->realDamage = 999;
					}
				}
				#endif

				BTL_POKEPARAM_HpMinus( defender, fep->realDamage );
				SCQUE_PUT_OP_HpMinus( server->que, defClient->myID, fep->realDamage );
				SCQUE_PUT_ACT_WazaDamage( server->que, atClient->myID, defClient->myID, fep->realDamage, wazaIdx, fep->typeAff );
				BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_DMG_AFTER );

				BTL_Printf("[SV] Waza Aff=%d, Damage=%d\n", fep->typeAff, fep->realDamage );

				if( BTL_POKEPARAM_GetValue(defender, BPP_HP) == 0 )
				{
					SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Dead, defClient->myID );
					SCQUE_PUT_ACT_Dead( server->que, defClient->myID );
					deadFlag = TRUE;
				}
				if( BTL_POKEPARAM_GetValue(attacker, BPP_HP) == 0 )
				{
					SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Dead, atClient->myID );
					SCQUE_PUT_ACT_Dead( server->que, atClient->myID );
					deadFlag = TRUE;
				}
				if( deadFlag ){ break; }
			}

			BTL_Printf("Damage Waza Exe %d times ... plural=%d, dead=%d\n",
					fep->hitCountReal, pluralHit, deadFlag);

			if( pluralHit )
			{
//				SCQUE_PUT_MSG_WazaHitCount( server->que, i );	// @@@ ���Ƃ�
			}
		}
		// ��_���[�W���U
		else
		{
			SCQUE_PUT_MSG_STD( server->que, BTL_STRID_STD_WazaAvoid );
			return;
		}
	}
}

// 
static BOOL sc_fight_layer1_calc_damage( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	if( WAZADATA_IsDamage(waza) )
	{
		sc_fight_layer1_single_dmg( server, fep, attacker, defender, waza );
		return TRUE;
	}
	else
	{
		sc_fight_layer1_single_not_dmg( server, attacker, defender, waza );
		return FALSE;
	}
}

// �_���[�W���U�q�b�g
static u16 sc_fight_layer1_single_dmg( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
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

		BTL_Printf("[SV WAZA] �З�:%d, ��������:%d, LV:%d, �ڂ�����:%d, ����:%d ... �f�_��:%d\n",
				fep->wazaPower, fep->attackerPower, attackerLevel, fep->defenderGuard, fep->damageDenom, fep->rawDamage
		);

	}
	BTL_EVENT_CallHandlers( server, BTL_EVENT_DAMAGE_PROC1 );


	//�����_���␳
	{
		u16 ratio = 100 - GFL_STD_MtRand(16);
		fep->rawDamage = (fep->rawDamage * ratio) / 100;
		BTL_Printf("[SV WAZA] �����_���␳:%d%%  -> �f�_��=%d\n", ratio, fep->rawDamage);
	}

	// �^�C�v��v�␳
	fep->rawDamage = (fep->rawDamage * fep->typeMatchRatio) >> FX32_SHIFT;
	BTL_Printf("[SV WAZA] �^�C�v��v�␳:%08x  -> �f�_��=%d\n", fep->typeMatchRatio, fep->rawDamage);

	// �^�C�v�����v�Z
	fep->realDamage = BTL_CALC_AffDamage( fep->rawDamage, fep->typeAff );

	// �Œ�A�P�̓_���[�W��^����
	if( fep->realDamage == 0 )
	{
		fep->realDamage = 1;
	}


	BTL_Printf("[SV WAZA] �^�C�v����:%02d -> �_���[�W�l�F%d\n", fep->typeAff, fep->realDamage);

	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_DMG_PROC2 );

	return fep->realDamage;

}
// ��_���[�W���U�q�b�g
static void sc_fight_layer1_single_not_dmg( BTL_SERVER* server, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	
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
static BOOL scEvent_CheckWazaExecute( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const CLIENT_WORK* atClient )
{
	// @@@ �Ђ�݁A�܂ЁA����������
	fep->wazaExecuteFlag = TRUE;
	fep->wazaFailReason = 0;

	BTL_EVENT_CallHandlers( server, BTL_EVENT_WAZA_EXECUTE );
	return fep->wazaExecuteFlag;
}

// ���U�����邩����
static BOOL scEvent_checkHit( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
{
	if( WAZADATA_IsAleadyHit(waza) )
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
static void scEvent_checkAffinity( BTL_SERVER* server, FIGHT_EVENT_PARAM* fep, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, WazaID waza )
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
static void scEvent_MemberOut( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID )
{
	
}

// �����o�[�V�K�Q��
static void scEvent_MemberIn( BTL_SERVER* server, CHANGE_EVENT_PARAM* cep, u8 clientID, u8 memberIdx )
{
//	cep->inPokeParam = BTL_MAIN_GetPokeParam
	SCQUE_PUT_DATA_MemberIn( server->que, clientID, memberIdx );
//	SCQUE_PUT_ACT_MemberIn( server->que, clientID, memberIdx );
}

// �|�P�����o�����������
static void scEvent_PokeComp( BTL_SERVER* server )
{
	BTL_EVENT_CallHandlers( server, BTL_EVENT_MEMBER_COMP );
}

// �\�͂�������
static void scEvent_RankDown( BTL_SERVER* server, u8 targetClientID, BppValueID statusType, u8 volume )
{
	server->eventArgs[ BTL_EVARG_COMMON_CLIENT_ID ] = targetClientID;
	server->eventArgs[ BTL_EVARG_RANKDOWN_STATUS_TYPE ] = statusType;
	server->eventArgs[ BTL_EVARG_RANKDOWN_VOLUME ] = volume;
	server->eventArgs[ BTL_EVARG_RANKDOWN_FAIL_FLAG ] = FALSE;

	BTL_EVENT_CallHandlers( server, BTL_EVENT_BEFORE_RANKDOWN );

	if( server->eventArgs[ BTL_EVARG_RANKDOWN_FAIL_FLAG ] == FALSE )
	{
		volume = server->eventArgs[ BTL_EVARG_RANKDOWN_VOLUME ];
		BTL_POKEPARAM_RankDown(
				server->client[targetClientID].frontMember,
				statusType, 
				volume
		);

		SCQUE_PUT_OP_RankDown( server->que, targetClientID, statusType, volume );
		SCQUE_PUT_ACT_RankDown( server->que, targetClientID, statusType, volume );
		SCQUE_PUT_MSG_SET( server->que, BTL_STRID_SET_Rankdown_ATK, targetClientID, statusType, volume );
	}
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

static inline u8 pokeID_to_clientID( const BTL_SERVER* sv, u8 pokeID )
{
	int i;
	for(i=0; i<sv->numClient; i++)
	{
		if( BTL_POKEPARAM_GetID(sv->client[i].frontMember) == pokeID )
		{
			return i;
		}
	}
	GF_ASSERT(0);
	return 0;
}

u8 BTL_SERVER_RECEPT_PokeIDtoClientID( const BTL_SERVER* sv, u8 pokeID )
{
	return pokeID_to_clientID( sv, pokeID );
}

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

void BTL_SERVER_RECTPT_SetMessage( BTL_SERVER* server, u16 msgID, u8 clientID )
{
	SCQUE_PUT_Msg( server->que, msgID, clientID );
}

//=============================================================================================
/**
 * �X�e�[�^�X�̃����N�_�E������
 *
 * @param   server			
 * @param   exID			�ΏۃN���C�A���gID
 * @param   statusType		�X�e�[�^�X�^�C�v
 * @param   volume		
 *
 */
//=============================================================================================
void BTL_SERVER_RECEPT_RankDownEffect( BTL_SERVER* server, BtlExClientID exID, BppValueID statusType, u8 volume )
{
	u8 clientID[ BTL_CLIENT_MAX ];
	u8 numClients, i;

	numClients = BTL_MAIN_ExpandClientID( server->mainModule, exID, clientID );
	BTL_Printf("�����N�������ʁF�^�C�v=%d,  �ΏۃN���C�A���g��=%d\n", statusType, numClients);
	for(i=0; i<numClients; i++)
	{
		scEvent_RankDown( server, clientID[i], statusType, volume );
	}
}


