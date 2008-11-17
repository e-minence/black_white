//=============================================================================================
/**
 * @file	btl_main.c
 * @brief	�|�P����WB �o�g���V�X�e�����C�����W���[��
 * @author	taya
 *
 * @date	2008.09.06	�쐬
 */
//=============================================================================================
#include <gflib.h>

#include "battle/battle.h"

#include "btl_common.h"
#include "btl_server.h"
#include "btl_client.h"
#include "btl_adapter.h"
#include "btl_pokeparam.h"
#include "btl_net.h"
#include "btl_util.h"

#include "btlv_core.h"

#include "btl_main.h"

enum {
	PARENT_HEAP_ID = 0,
	SYS_HEAP_ID = 115,
	VIEW_HEAP_ID,
	NET_HEAP_ID,

	BTL_COMMITMENT_POKE_MAX = BTL_CLIENT_MAX * TEMOTI_POKEMAX,

};



typedef BOOL (*pMainLoop)(BTL_MAIN_MODULE*);

typedef struct {
	u8	competitor;
	u8	rule;
	u8	engine;
	u8	comm_mode;
}BTL_BASIC_PARAMS;


struct _BTL_PARTY {
	BTL_POKEPARAM*  member[ TEMOTI_POKEMAX ];
	u8  memberCount;
};



struct _BTL_MAIN_MODULE {

	const BATTLE_SETUP_PARAM*	setupParam;
	BTL_BASIC_PARAMS			basicParam;

	BTLV_CORE*		viewCore;
	BTL_SERVER*		server;
	BTL_CLIENT*		client[ BTL_CLIENT_MAX ];

	// �T�[�o���v�Z���ɏ��������Ă��ǂ��ꎞ�g�p�p�����[�^�̈�ƁA
	// �T�[�o�R�}���h���󂯎�����N���C�A���g�����ۂɏ���������p�����[�^�̈�
	BTL_PARTY		party[ BTL_CLIENT_MAX ];
	BTL_POKEPARAM*	pokeParam[ BTL_COMMITMENT_POKE_MAX ];
	BTL_PARTY		partyForServerCalc[ BTL_CLIENT_MAX ];
	BTL_POKEPARAM*	pokeParamForServerCalc[ BTL_COMMITMENT_POKE_MAX ];
	u8				frontMemberIdx[ BTL_CLIENT_MAX ];

	u32				numClients;

	BTL_PROC		subProc;
	pMainLoop		mainLoop;


	HEAPID			heapID;

};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT BTL_PROC_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT BTL_PROC_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT BTL_PROC_Quit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static void setupInitializeProc( BTL_PROC* bp, BTL_MAIN_MODULE* wk, const BATTLE_SETUP_PARAM* setup_param );
static void setupPokeParams( BTL_PARTY* dstParty, BTL_POKEPARAM** dstParams, const POKEPARTY* party, u8 pokeID_Origin );
static BOOL initialize_alone_single( int* seq, void* work );
static BOOL initialize_comm_single( int* seq, void* work );
static BOOL MainLoop_StandAlone( BTL_MAIN_MODULE* wk );
static u8 expandClientID_single( const BTL_MAIN_MODULE* wk, BtlExClientType exType, u8 clientID, u8* dst );
static u8 expandClientID_double( const BTL_MAIN_MODULE* wk, BtlExClientType exType, u8 clientID, u8* dst );
static void BTL_PARTY_Initialize( BTL_PARTY* party );
static void BTL_PARTY_AddMember( BTL_PARTY* party, BTL_POKEPARAM* member );


static GFL_PROC_RESULT BTL_PROC_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	switch( *seq ){
	case 0:
		{
			BTL_MAIN_MODULE* wk;
			const BATTLE_SETUP_PARAM* setup_param = pwk;

			GFL_HEAP_CreateHeap( PARENT_HEAP_ID, SYS_HEAP_ID, 0x68000 );
			GFL_HEAP_CreateHeap( PARENT_HEAP_ID, NET_HEAP_ID, 0x8000 );

			wk = GFL_PROC_AllocWork( proc, SYS_HEAP_ID, sizeof(BTL_MAIN_MODULE) );
			wk->heapID = SYS_HEAP_ID;
			wk->setupParam = setup_param;

			BTL_NET_InitSystem( setup_param->netHandle, NET_HEAP_ID );
			BTL_ADAPTER_InitSystem();

			setupInitializeProc( &wk->subProc, wk, setup_param );
			(*seq)++;
		}
		break;

	case 1:
		{
			BTL_MAIN_MODULE* wk = mywk;
			if( BTL_UTIL_CallProc(&wk->subProc) )
			{
				return GFL_PROC_RES_FINISH;
			}
		}
		break;

	}

	return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT BTL_PROC_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	BTL_MAIN_MODULE* wk = mywk;


	if( wk->mainLoop( wk ) )
	{
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;

	#if 0
	BTL_MAIN_MODULE* wk = GFL_HEAP_AllocMemory( 0, sizeof(BTL_MAIN_MODULE) );


	// 
	// �K�v�ȃp�����[�^���
	// 

	

	while( 1 )
	{
		if( wk->mainLoop( wk ) )
		{
			break;
		}
	}

	return 0;
	return GFL_PROC_RES_FINISH;
	#endif
}

static GFL_PROC_RESULT BTL_PROC_Quit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	BTL_Printf("btl quit called\n");
	GFL_HEAP_DeleteHeap( SYS_HEAP_ID );
	return GFL_PROC_RES_FINISH;
}

//======================================================================================================
//======================================================================================================


static void setupInitializeProc( BTL_PROC* bp, BTL_MAIN_MODULE* wk, const BATTLE_SETUP_PARAM* setup_param )
{
	// @@@ �{���� setup_param ���Q�Ƃ��Ċe�평�����������[�`�������肷��
	if( setup_param->commMode == BTL_COMM_NONE )
	{
		BTL_UTIL_SetupProc( bp, wk, initialize_alone_single, NULL );
	}
	else
	{
		BTL_UTIL_SetupProc( bp, wk, initialize_comm_single, NULL );
	}
}

//--------------------------------------------------------------------------
/**
 * �o�g���p�|�P�����p�����[�^���p�[�e�B�f�[�^�𐶐�
 *
 * @param   dstParty		[out] �p�[�e�B�f�[�^������A�h���X
 * @param   dstParams		[out] �|�P�����p�����[�^������z��A�h���X
 * @param   party			[in] �������̃t�B�[���h�p�|�P�����p�[�e�B�f�[�^
 *
 */
//--------------------------------------------------------------------------
static void setupPokeParams( BTL_PARTY* dstParty, BTL_POKEPARAM** dstParams, const POKEPARTY* party, u8 pokeID_Origin )
{
	int i, max = PokeParty_GetPokeCount( party );

	BTL_PARTY_Initialize( dstParty );
	for(i=0; i<max; i++)
	{
		dstParams[i] = BTL_POKEPARAM_Create( PokeParty_GetMemberPointer(party, i), pokeID_Origin+i, SYS_HEAP_ID );
		BTL_PARTY_AddMember( dstParty, dstParams[i] );
	}
}

//--------------------------------------------------------------------------
/**
 * �X�^���h�A���[���^�V���O���o�g��
 */
//--------------------------------------------------------------------------
static BOOL initialize_alone_single( int* seq, void* work )
{
	// server*1, client*2
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	// �o�g���p�|�P�����p�����[�^���p�[�e�B�f�[�^�𐶐�
	setupPokeParams( &wk->party[0], &wk->pokeParam[0], sp->partyPlayer, 0 );
	setupPokeParams( &wk->party[1], &wk->pokeParam[TEMOTI_POKEMAX], sp->partyEnemy1, TEMOTI_POKEMAX );

	setupPokeParams( &wk->partyForServerCalc[0], &wk->pokeParamForServerCalc[0], sp->partyPlayer, 0 );
	setupPokeParams( &wk->partyForServerCalc[1], &wk->pokeParamForServerCalc[TEMOTI_POKEMAX], sp->partyEnemy1, TEMOTI_POKEMAX );

	wk->frontMemberIdx[0] = 0;
	wk->frontMemberIdx[1] = 0;

	// Server �쐬
	wk->server = BTL_SERVER_Create( wk, wk->heapID );

	// Client �쐬
	wk->client[0] = BTL_CLIENT_Create( wk, BTL_COMM_NONE, sp->netHandle, 0, BTL_THINKER_UI, wk->heapID );
	wk->client[1] = BTL_CLIENT_Create( wk, BTL_COMM_NONE, sp->netHandle, 1, BTL_THINKER_AI, wk->heapID );
	wk->numClients = 2;

	// Server �� Client ��ڑ�
	BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[0]), &wk->partyForServerCalc[0], 0 );
	BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[1]), &wk->partyForServerCalc[1], 1 );

	// �`��G���W������
	wk->viewCore = BTLV_Create( wk, wk->client[0], VIEW_HEAP_ID );

	// �v���C���[�N���C�A���g�ɕ`��G���W�����֘A�t����
	BTL_CLIENT_AttachViewCore( wk->client[0], wk->viewCore );

	wk->mainLoop = MainLoop_StandAlone;

	return TRUE;
}

//--------------------------------------------------------------------------
/**
 * �ʐM�^�V���O���o�g��
 */
//--------------------------------------------------------------------------
static BOOL initialize_comm_single( int* seq, void* work )
{
//	BTL_MAIN_MODULE* wk = work;
//	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	switch( *seq ){
	case 0:
		BTL_NET_StartCommand( BTL_NETCMD_SUPPLY_POKEDATA );
		(*seq)++;
		break;
	case 1:
		if( BTL_NET_WaitCommand() )
		{
			// @@@ ���Ƃ܂킵
			return TRUE;
		}
		break;
	}
	return FALSE;
}


//======================================================================================================
//======================================================================================================


static BOOL MainLoop_StandAlone( BTL_MAIN_MODULE* wk )
{
	int i;

	if( BTL_SERVER_Main( wk->server ) )
	{
		BTL_Printf("server end msg recved\n");
		return TRUE;
	}

	for(i=0; i<2; i++)
	{
		BTL_CLIENT_Main( wk->client[i] );
	}

	return FALSE;
}

//--------------------------------------------------------------
/**
 *	Proc Data
 */
//--------------------------------------------------------------
const GFL_PROC_DATA BtlProcData = {
	BTL_PROC_Init,
	BTL_PROC_Main,
	BTL_PROC_Quit,
};





//======================================================================================================
// �N���C�A���g������̖₢���킹�Ή�
//======================================================================================================


//=============================================================================================
/**
 * �퓬�̃��[���w��q��Ԃ�
 *
 * @param   wk		
 *
 * @retval  BtlRule		
 */
//=============================================================================================
BtlRule BTL_MAIN_GetRule( const BTL_MAIN_MODULE* wk )
{
	return wk->setupParam->rule;
}


//=============================================================================================
/**
 * ����N���C�A���gID�w��q���A���ۂ̑ΏۃN���C�A���gID�ɕϊ�
 *
 * @param   wk						���C�����W���[���n���h��
 * @param   exID					����N���C�A���gID�w��q
 * @param   dst						[out] �ΏۃN���C�A���gID���i�[����o�b�t�@
 *
 * @retval  u8		�ΏۃN���C�A���g��
 */
//=============================================================================================
u8 BTL_MAIN_ExpandClientID( const BTL_MAIN_MODULE* wk, BtlExClientID exID, u8* dst )
{
	u8 exType = EXID_GET_TYPE( exID );
	u8 clientID = EXID_GET_CLIENTID( exID );

	if( exType == BTL_EXCLIENT_DEFAULT )
	{
		dst[0] = clientID;
		return 1;
	}

	switch( wk->setupParam->rule ){
	case BTL_RULE_SINGLE:
		return expandClientID_single( wk, exType, clientID, dst );
	case BTL_RULE_DOUBLE:
	default:
		return expandClientID_double( wk, exType, clientID, dst );
	}
}
// �V���O���p
static u8 expandClientID_single( const BTL_MAIN_MODULE* wk, BtlExClientType exType, u8 clientID, u8* dst )
{
	switch( exType ){
	default:
		GF_ASSERT(0);
		/* fallthru */
	case BTL_EXCLIENT_ENEMY_ALL:
	case BTL_EXCLIENT_ENEMY_RANDOM:
	case BTL_EXCLIENT_WITHOUT_ME:
		dst[0] = BTL_MAIN_GetOpponentClientID( wk, clientID, 0 );
		break;
	}
	return 1;
}
// �_�u���p
static u8 expandClientID_double( const BTL_MAIN_MODULE* wk, BtlExClientType exType, u8 clientID, u8* dst )
{
	switch( exType ){
	default:
		GF_ASSERT(0);
		/* fallthru */
	case BTL_EXCLIENT_ENEMY_ALL:
		dst[0] = BTL_MAIN_GetOpponentClientID( wk, clientID, 0 );
		dst[1] = BTL_MAIN_GetOpponentClientID( wk, clientID, 1 );
		return 2;
	case BTL_EXCLIENT_ENEMY_RANDOM:
		dst[0] = BTL_MAIN_GetOpponentClientID( wk, clientID, GFL_STD_MtRand(1) );
		return 1;
	case BTL_EXCLIENT_WITHOUT_ME:
		dst[0] = BTL_MAIN_GetOpponentClientID( wk, clientID, 0 );
		dst[1] = BTL_MAIN_GetOpponentClientID( wk, clientID, 1 );
		dst[2] = BTL_MAIN_GetFriendClientID( wk, clientID, 1 );
		return 3;
	}
}

//=============================================================================================
/**
 * ������N���C�A���gID��Ԃ�
 *
 * @param   wk				
 * @param   clientID		
 * @param   idx				
 *
 * @retval  u8				
 */
//=============================================================================================
u8 BTL_MAIN_GetOpponentClientID( const BTL_MAIN_MODULE* wk, u8 clientID, u8 idx )
{
	switch( wk->setupParam->rule ){
	case BTL_RULE_SINGLE:
		GF_ASSERT(idx==0);
		GF_ASSERT(clientID<2);
		return !clientID;

	case BTL_RULE_DOUBLE:
		GF_ASSERT(idx<2);
		GF_ASSERT(clientID<4);
		return !(clientID&1) + (idx*2);

	default:
		GF_ASSERT(0);
		return !(clientID&1);
	}
}
//=============================================================================================
/**
 * �����N���C�A���gID��Ԃ�
 *
 * @param   wk			
 * @param   cliendID	
 * @param   idx			
 *
 * @retval  u8			
 */
//=============================================================================================
u8 BTL_MAIN_GetFriendClientID( const BTL_MAIN_MODULE* wk, u8 clientID, u8 idx )
{
	switch( wk->setupParam->rule ){
	case BTL_RULE_SINGLE:
		GF_ASSERT(idx==0);
		GF_ASSERT(clientID<2);
		return clientID;

	case BTL_RULE_DOUBLE:
		GF_ASSERT(idx<2);
		GF_ASSERT(clientID<4);
		return (clientID&1) + (idx*2);

	default:
		GF_ASSERT(0);
		return (clientID&1);
	}
}




//=============================================================================================
/**
 * �w��N���C�A���g�̃p�[�e�B�f�[�^��Ԃ�
 *
 * @param   wk		
 * @param   clientID		
 *
 * @retval  const POKEPARTY*		
 */
//=============================================================================================
const BTL_PARTY* BTL_MAIN_GetPartyDataConst( const BTL_MAIN_MODULE* wk, u32 clientID )
{
	return &wk->party[ clientID ];
}
BTL_PARTY* BTL_MAIN_GetPartyData( BTL_MAIN_MODULE* wk, u32 clientID )
{
	return &wk->party[ clientID ];
}

BTL_POKEPARAM* BTL_MAIN_GetFrontPokeData( BTL_MAIN_MODULE* wk, u8 clientID )
{
	BTL_PARTY* party = BTL_MAIN_GetPartyData( wk, clientID );
	return BTL_PARTY_GetMemberData( party, wk->frontMemberIdx[clientID] );
}

const BTL_POKEPARAM* BTL_MAIN_GetFrontPokeDataConst( const BTL_MAIN_MODULE* wk, u8 clientID )
{
	const BTL_PARTY* party = BTL_MAIN_GetPartyDataConst( wk, clientID );
	return BTL_PARTY_GetMemberDataConst( party, wk->frontMemberIdx[clientID] );
}

void BTL_MAIN_SetFrontMemberIdx( BTL_MAIN_MODULE* wk, u8 clientID, u8 memberIdx )
{
	wk->frontMemberIdx[ clientID ] = memberIdx;
}


u8 BTL_MAIN_GetFrontMemberIdx( const BTL_MAIN_MODULE* wk, u8 clientID )
{
	return wk->frontMemberIdx[ clientID ];
}


static void BTL_PARTY_Initialize( BTL_PARTY* party )
{
	int i;
	party->memberCount = 0;
	for(i=0; i<TEMOTI_POKEMAX; i++)
	{
		party->member[i] = NULL;
	}
}

static void BTL_PARTY_AddMember( BTL_PARTY* party, BTL_POKEPARAM* member )
{
	GF_ASSERT(party->memberCount < TEMOTI_POKEMAX);
	party->member[party->memberCount++] = member;
}

u8 BTL_PARTY_GetMemberCount( const BTL_PARTY* party )
{
	return party->memberCount;
}

u8 BTL_PARTY_GetAliveMemberCount( const BTL_PARTY* party )
{
	int i, cnt;
	for(i=0, cnt=0; i<party->memberCount; i++)
	{
		if( BTL_POKEPARAM_GetValue(party->member[i], BPP_HP) )
		{
			cnt++;
		}
	}
	return cnt;
}


BTL_POKEPARAM* BTL_PARTY_GetMemberData( BTL_PARTY* party, u8 idx )
{
	return party->member[ idx ];
}
const BTL_POKEPARAM* BTL_PARTY_GetMemberDataConst( const BTL_PARTY* party, u8 idx )
{
	return party->member[ idx ];
}

const BTL_POKEPARAM* BTL_MAIN_GetPokeParam( const BTL_MAIN_MODULE* wk, u8 pokeID )
{
	GF_ASSERT(pokeID<BTL_COMMITMENT_POKE_MAX);
	GF_ASSERT(wk->pokeParam[pokeID]);

	return wk->pokeParam[ pokeID ];
}



//=============================================================================================
/**
 * �N���C�A���g�v�Z���ʊ�����̃f�[�^���A�T�[�o�v�Z�p�f�[�^�ɏ㏑������
 *
 * @param   wk		
 *
 */
//=============================================================================================
void BTL_MAIN_SyncServerCalcData( BTL_MAIN_MODULE* wk )
{
	GFL_STD_MemCopy32( wk->pokeParam, wk->pokeParamForServerCalc, sizeof(wk->pokeParamForServerCalc) );
}

