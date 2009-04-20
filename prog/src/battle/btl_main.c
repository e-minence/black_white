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

#include "system/main.h"
#include "battle/battle.h"
#include "btlv/btlv_mcss.h"

#include "btl_common.h"
#include "btl_server.h"
#include "btl_field.h"
#include "btl_client.h"
#include "btl_adapter.h"
#include "btl_pokeparam.h"
#include "btl_net.h"
#include "btl_util.h"

#include "btlv/btlv_core.h"

#include "btl_main.h"



/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	PARENT_HEAP_ID = GFL_HEAPID_APP,

	BTL_COMMITMENT_POKE_MAX = BTL_CLIENT_MAX * TEMOTI_POKEMAX,
};

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*pMainLoop)(BTL_MAIN_MODULE*);


/*--------------------------------------------------------------------------*/
/* Tables                                                                   */
/*--------------------------------------------------------------------------*/
static const u8 ClientBasePokeID[] = {
	0,									// Client 0 �̃|�P����ID �J�n�l
	TEMOTI_POKEMAX*2,		// Client 1 �̃|�P����ID �J�n�l
	TEMOTI_POKEMAX*1,		// Client 2 �̃|�P����ID �J�n�l
	TEMOTI_POKEMAX*3,		// Client 3 �̃|�P����ID �J�n�l
};

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

struct _BTL_PARTY {
	BTL_POKEPARAM*  member[ TEMOTI_POKEMAX ];
	u8  memberCount;
};

struct _BTL_POKE_CONTAINER {
	BTL_MAIN_MODULE*	mainModule;
	BTL_PARTY					party[ BTL_CLIENT_MAX ];
	BTL_POKEPARAM*		pokeParam[ BTL_COMMITMENT_POKE_MAX ];
};

struct _BTL_MAIN_MODULE {

	const BATTLE_SETUP_PARAM*	setupParam;

	BTLV_CORE*		viewCore;
	BTL_SERVER*		server;
	BTL_CLIENT*		client[ BTL_CLIENT_MAX ];

	// �T�[�o���v�Z���ɏ��������Ă��ǂ��ꎞ�g�p�p�����[�^�̈�ƁA
	// �T�[�o�R�}���h���󂯎�����N���C�A���g�����ۂɏ���������p�����[�^�̈�
	BTL_POKE_CONTAINER		pokeconForClient;
	BTL_POKE_CONTAINER		pokeconForServer;

	u8							posCoverClientID[ BTL_POS_MAX ];

	u8				numClients;
	u8				myClientID;
	u8				myOrgPos;

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
static void setSubProcForSetup( BTL_PROC* bp, BTL_MAIN_MODULE* wk, const BATTLE_SETUP_PARAM* setup_param );
static void setSubProcForClanup( BTL_PROC* bp, BTL_MAIN_MODULE* wk, const BATTLE_SETUP_PARAM* setup_param );
static BOOL setup_alone_single( int* seq, void* work );
static BOOL cleanup_common( int* seq, void* work );
static BOOL setup_comm_single( int* seq, void* work );
static BOOL setup_alone_double( int* seq, void* work );
static BOOL cleanup_alone_double( int* seq, void* work );
static BOOL setup_comm_double( int* seq, void* work );
static BOOL MainLoop_StandAlone( BTL_MAIN_MODULE* wk );
static BOOL MainLoop_Comm_Server( BTL_MAIN_MODULE* wk );
static BOOL MainLoop_Comm_NotServer( BTL_MAIN_MODULE* wk );
static u8 expandPokePos_single( const BTL_MAIN_MODULE* wk, BtlExPos exType, u8 basePos, u8* dst );
static u8 expandPokePos_double( const BTL_MAIN_MODULE* wk, BtlExPos exType, u8 basePos, u8* dst );
static inline clientID_to_side( u8 clientID );
static inline u8 btlPos_to_clientID( const BTL_MAIN_MODULE* wk, BtlPokePos btlPos );
static inline void btlPos_to_cliendID_and_posIdx( const BTL_MAIN_MODULE* wk, BtlPokePos btlPos, u8* clientID, u8* posIdx );
static inline u8 btlPos_to_sidePosIdx( BtlPokePos pos );
static inline u8 PokeID_to_ClientID( u8 pokeID );
static void PokeCon_Init( BTL_POKE_CONTAINER* pokecon, BTL_MAIN_MODULE* mainModule );
static void PokeCon_AddParty( BTL_POKE_CONTAINER* pokecon, const POKEPARTY* party_src, u8 clientID );
static void PokeCon_Release( BTL_POKE_CONTAINER* pokecon );
static int PokeCon_FindPokemon( const BTL_POKE_CONTAINER* pokecon, u8 clientID, u8 pokeID );
static void BTL_PARTY_Initialize( BTL_PARTY* party );
static void BTL_PARTY_AddMember( BTL_PARTY* party, BTL_POKEPARAM* member );


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



static GFL_PROC_RESULT BTL_PROC_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	switch( *seq ){
	case 0:
		{
			BTL_MAIN_MODULE* wk;
			const BATTLE_SETUP_PARAM* setup_param = pwk;

			GFL_HEAP_CreateHeap( PARENT_HEAP_ID, HEAPID_BTL_SYSTEM, 0x28000 );
			GFL_HEAP_CreateHeap( PARENT_HEAP_ID, HEAPID_BTL_NET,     0x8000 );
			GFL_HEAP_CreateHeap( PARENT_HEAP_ID, HEAPID_BTL_VIEW,   0xa0000 );

			wk = GFL_PROC_AllocWork( proc, sizeof(BTL_MAIN_MODULE), HEAPID_BTL_SYSTEM );
			GFL_STD_MemClear32( wk, sizeof(BTL_MAIN_MODULE) );

			wk->heapID = HEAPID_BTL_SYSTEM;
			wk->setupParam = setup_param;

			BTL_NET_InitSystem( setup_param->netHandle, HEAPID_BTL_NET );
			BTL_ADAPTERSYS_Init( setup_param->commMode );
			BTL_FIELD_Init( BTL_WEATHER_SHINE );

			setSubProcForSetup( &wk->subProc, wk, setup_param );

//			WAZADATA_PrintDebug();
			(*seq)++;
		}
		break;

	case 1:
		{
			BTL_MAIN_MODULE* wk = mywk;
			if( BTL_UTIL_CallProc(&wk->subProc) )
			{
				BTL_Printf("Proc Init done\n");
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
		BTL_Printf("�o�g�����C���v���Z�X�I�����܂�\n");
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;

}

static GFL_PROC_RESULT BTL_PROC_Quit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	BTL_MAIN_MODULE* wk = mywk;

	switch( *seq ){
	case 0:
		BTL_Printf("�N���[���A�b�v�v���Z�X�P\n");
		setSubProcForClanup( &wk->subProc, wk, wk->setupParam );
		(*seq)++;
		break;

	case 1:
		if( BTL_UTIL_CallProc(&wk->subProc) )
		{
			BTL_Printf("�N���[���A�b�v�v���Z�X�Q�|�P\n");
			BTL_ADAPTERSYS_Quit();
			BTL_Printf("�N���[���A�b�v�v���Z�X�Q�|�Q\n");
			BTL_NET_QuitSystem();
			BTL_Printf("�N���[���A�b�v�v���Z�X�Q�|�R\n");
			(*seq)++;
		}
		break;

	case 2:
		BTL_Printf("�N���[���A�b�v�v���Z�X�R\n");
		GFL_PROC_FreeWork( proc );
		GFL_HEAP_DeleteHeap( HEAPID_BTL_VIEW );
		GFL_HEAP_DeleteHeap( HEAPID_BTL_NET );
		GFL_HEAP_DeleteHeap( HEAPID_BTL_SYSTEM );
		BTL_Printf("�N���[���A�b�v�v���Z�X�I��\n");
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}

//======================================================================================================
//======================================================================================================

// �e�탂�W���[���������֘A�t�����[�`����ݒ�
static void setSubProcForSetup( BTL_PROC* bp, BTL_MAIN_MODULE* wk, const BATTLE_SETUP_PARAM* setup_param )
{
	int i;

	// �ʒu�S���N���C�A���gID�𖳌��l�ŏ��������Ă���
	for(i=0; i<NELEMS(wk->posCoverClientID); ++i)
	{
		wk->posCoverClientID[i] = BTL_CLIENT_MAX;
	}

	// @@@ �{���� setup_param ���Q�Ƃ��Ċe�평�����������[�`�������肷��
	if( setup_param->commMode == BTL_COMM_NONE )
	{
		BTL_UTIL_SetPrintType( BTL_PRINTTYPE_STANDALONE );

		switch( setup_param->rule ){
		case BTL_RULE_SINGLE:
			BTL_UTIL_SetupProc( bp, wk, setup_alone_single, NULL );
			break;
		case BTL_RULE_DOUBLE:
			BTL_UTIL_SetupProc( bp, wk, setup_alone_double, NULL );
			break;
		default:
			GF_ASSERT(0);
			BTL_UTIL_SetupProc( bp, wk, setup_alone_single, NULL );
			break;
		}
	}
	else
	{
		switch( setup_param->rule ){
		case BTL_RULE_SINGLE:
			BTL_UTIL_SetupProc( bp, wk, setup_comm_single, NULL );
			break;
		case BTL_RULE_DOUBLE:
			BTL_UTIL_SetupProc( bp, wk, setup_comm_double, NULL );
			break;
		default:
			GF_ASSERT(0);
			BTL_UTIL_SetupProc( bp, wk, setup_alone_single, NULL );
			break;
		}
	}
}

// �e�탂�W���[��������폜�������[�`����ݒ�
static void setSubProcForClanup( BTL_PROC* bp, BTL_MAIN_MODULE* wk, const BATTLE_SETUP_PARAM* setup_param )
{
	// @@@ �{���� setup_param ���Q�Ƃ��Ċe�평�����������[�`�������肷��
	if( setup_param->commMode == BTL_COMM_NONE )
	{
		switch( setup_param->rule ){
		case BTL_RULE_SINGLE:
			BTL_UTIL_SetupProc( bp, wk, cleanup_common, NULL );
			break;
		case BTL_RULE_DOUBLE:
			BTL_UTIL_SetupProc( bp, wk, cleanup_alone_double, NULL );
			break;
		default:
			GF_ASSERT(0);
			BTL_UTIL_SetupProc( bp, wk, cleanup_common, NULL );
			break;
		}
	}
	else
	{
		switch( setup_param->rule ){
		case BTL_RULE_SINGLE:
			BTL_UTIL_SetupProc( bp, wk, cleanup_common, NULL );
			break;
		case BTL_RULE_DOUBLE:
			BTL_UTIL_SetupProc( bp, wk, cleanup_common, NULL );
			break;
		default:
			GF_ASSERT(0);
			BTL_UTIL_SetupProc( bp, wk, cleanup_common, NULL );
			break;
		}
	}
}

//--------------------------------------------------------------------------
/**
 * �X�^���h�A���[���^�V���O���o�g��
 */
//--------------------------------------------------------------------------
static BOOL setup_alone_single( int* seq, void* work )
{
	// server*1, client*2
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	// �o�g���p�|�P�����p�����[�^���p�[�e�B�f�[�^�𐶐�
	PokeCon_Init( &wk->pokeconForClient, wk );
	PokeCon_AddParty( &wk->pokeconForClient, sp->partyPlayer, 0 );
	PokeCon_AddParty( &wk->pokeconForClient, sp->partyEnemy1, 1 );

	PokeCon_Init( &wk->pokeconForServer, wk );
	PokeCon_AddParty( &wk->pokeconForServer, sp->partyPlayer, 0 );
	PokeCon_AddParty( &wk->pokeconForServer, sp->partyEnemy1, 1 );

	// Server �쐬
	wk->server = BTL_SERVER_Create( wk, &wk->pokeconForServer, wk->heapID );

	// Client �쐬
	wk->client[0] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, BTL_COMM_NONE, sp->netHandle, 0, 1, BTL_THINKER_UI, wk->heapID );
	wk->client[1] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, BTL_COMM_NONE, sp->netHandle, 1, 1, BTL_THINKER_AI, wk->heapID );
	wk->numClients = 2;
	wk->myClientID = 0;
	wk->myOrgPos = BTL_POS_1ST_0;
	wk->posCoverClientID[BTL_POS_1ST_0] = 0;
	wk->posCoverClientID[BTL_POS_2ND_0] = 1;

	// �`��G���W������
	wk->viewCore = BTLV_Create( wk, wk->client[0], &wk->pokeconForClient, HEAPID_BTL_VIEW );

	// �v���C���[�N���C�A���g�ɕ`��G���W�����֘A�t����
	BTL_CLIENT_AttachViewCore( wk->client[0], wk->viewCore );

	// Server �� Client ��ڑ�
	BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[0]), 0, 1 );
	BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[1]), 1, 1 );

	// Server �n��
	BTL_SERVER_Startup( wk->server );

	wk->mainLoop = MainLoop_StandAlone;

	return TRUE;
}
static BOOL cleanup_common( int* seq, void* work )
{
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;
	int i;

	BTL_Printf("�N���[���A�b�v 1-1\n");

	BTLV_Delete( wk->viewCore );

	BTL_Printf("�N���[���A�b�v 1-2\n");

	for(i=0; i<NELEMS(wk->client); ++i)
	{
		if( wk->client[i] )
		{
			BTL_CLIENT_Delete( wk->client[i] );
		}
	}

	BTL_Printf("�N���[���A�b�v 1-3\n");
	PokeCon_Release( &wk->pokeconForClient );
	PokeCon_Release( &wk->pokeconForServer );

	if( wk->server )
	{
		BTL_SERVER_Delete( wk->server );
	}

	BTL_Printf("�N���[���A�b�v 1-4\n");

	return TRUE;
}
//--------------------------------------------------------------------------
/**
 * �ʐM�^�V���O���o�g��
 */
//--------------------------------------------------------------------------
static BOOL setup_comm_single( int* seq, void* work )
{
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	switch( *seq ){
	case 0:
		if( BTL_NET_IsServerDetermained() )
		{
			BTL_Printf("�T�[�o���肵�܂�����\n");
			if( BTL_NET_IsServer() )
			{
				// �T�[�o�[�}�V�����A�e�Q���҂ɃN���C�A���gID������U��
				u8 clientID_0, clientID_1;

				clientID_0 = 0;
				clientID_1 = 1;

				BTL_UTIL_SetPrintType( BTL_PRINTTYPE_SERVER );
				BTL_Printf("���V�A�T�[�o�[�ł��B\n");

				BTL_NET_NotifyClientID( clientID_0, &clientID_0, 1 );
				BTL_NET_NotifyClientID( clientID_1, &clientID_1, 1 );
			}
			else
			{
				BTL_UTIL_SetPrintType( BTL_PRINTTYPE_CLIENT );
				BTL_Printf("���V�A�T�[�o�[�ł͂Ȃ��B\n");
			}
			(*seq)++;
		}
		break;
	case 1:
		#ifdef PM_DEBUG
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{
			BTL_Printf("�N���C�A���gID�̊m��҂��Ł[��\n");
		}
		#endif
		// �����̃N���C�A���gID���m��
		if( BTL_NET_IsClientIdDetermined() )
		{
			BTL_Printf("�N���C�A���gID���m�肵�����^�C�~���O�V���N��\n");
			BTL_NET_TimingSyncStart( BTL_NET_TIMING_CLIENTID_DETERMINE );
			(*seq)++;
		}
		break;
	case 2:
		if( BTL_NET_IsTimingSync(BTL_NET_TIMING_CLIENTID_DETERMINE) )
		{
			BTL_Printf("�^�C�~���O�V���N�����܂����B\n");
			BTL_NET_StartNotifyPartyData( sp->partyPlayer );
			(*seq)++;
		}
		break;
	case 3:
		// �p�[�e�B�f�[�^���ݎ�M������
		if( BTL_NET_IsCompleteNotifyPartyData() )
		{
			BTL_Printf("�p�[�e�B�f�[�^���ݎ�M�ł��܂����B\n");
			PokeCon_Init( &wk->pokeconForClient, wk );
			PokeCon_AddParty( &wk->pokeconForClient, BTL_NET_GetPartyData(0), 0 );
			PokeCon_AddParty( &wk->pokeconForClient, BTL_NET_GetPartyData(1), 1 );
			(*seq)++;
		}
		break;
	case 4:
		if( BTL_NET_IsServer() )
		{
			PokeCon_Init( &wk->pokeconForServer, wk );
			PokeCon_AddParty( &wk->pokeconForServer, BTL_NET_GetPartyData(0), 0 );
			PokeCon_AddParty( &wk->pokeconForServer, BTL_NET_GetPartyData(1), 1 );
		}
		(*seq)++;
		break;
	case 5:
		BTL_NET_EndNotifyPartyData();
		(*seq)++;
		break;
	case 6:
		wk->numClients = 2;
		wk->posCoverClientID[BTL_POS_1ST_0] = 0;
		wk->posCoverClientID[BTL_POS_2ND_0] = 1;
		wk->myClientID = GFL_NET_GetNetID( sp->netHandle );
		wk->myOrgPos = (wk->myClientID == 0)? BTL_POS_1ST_0 : BTL_POS_2ND_0;


		// �������T�[�o
		if( BTL_NET_IsServer() )
		{
			u8 netID = GFL_NET_GetNetID( sp->netHandle );

			BTL_Printf("�T�[�o�p�̃p�[�e�B�f�[�^�Z�b�g\n");

			wk->server = BTL_SERVER_Create( wk, &wk->pokeconForServer, wk->heapID );
			wk->client[netID] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, sp->commMode, sp->netHandle,
					netID, 1, BTL_THINKER_UI, wk->heapID );
			BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[netID]), netID, 1 );
			BTL_SERVER_ReceptionNetClient( wk->server, sp->commMode, sp->netHandle, !netID, 1 );

			// �`��G���W������
			wk->viewCore = BTLV_Create( wk, wk->client[netID], &wk->pokeconForClient, HEAPID_BTL_VIEW );
			BTL_CLIENT_AttachViewCore( wk->client[netID], wk->viewCore );

			// Server �n��
			BTL_SERVER_Startup( wk->server );

			wk->mainLoop = MainLoop_Comm_Server;
		}
		// �������T�[�o�ł͂Ȃ�
		else
		{
			u8 netID = GFL_NET_GetNetID( sp->netHandle );

			BTL_Printf("�T�[�o�ł͂Ȃ��p�̃p�[�e�B�f�[�^�Z�b�g\n");

			wk->client[ netID ] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, sp->commMode, sp->netHandle, netID, 1, BTL_THINKER_UI, wk->heapID  );

			// �`��G���W������
			wk->viewCore = BTLV_Create( wk, wk->client[netID], &wk->pokeconForClient, HEAPID_BTL_VIEW );
			BTL_CLIENT_AttachViewCore( wk->client[netID], wk->viewCore );

			wk->mainLoop = MainLoop_Comm_NotServer;
		}
		(*seq)++;
		break;
	case 7:
		BTL_Printf("�Z�b�g�A�b�v����\n");
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------------------------------
/**
 * �X�^���h�A���[���^�_�u���o�g���F�Z�b�g�A�b�v
 */
//--------------------------------------------------------------------------
static BOOL setup_alone_double( int* seq, void* work )
{
	// server*1, client*2
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	// �o�g���p�|�P�����p�����[�^���p�[�e�B�f�[�^�𐶐�
	PokeCon_Init( &wk->pokeconForClient, wk );
	PokeCon_AddParty( &wk->pokeconForClient, sp->partyPlayer, 0 );
	PokeCon_AddParty( &wk->pokeconForClient, sp->partyEnemy1, 1 );

	PokeCon_Init( &wk->pokeconForServer, wk );
	PokeCon_AddParty( &wk->pokeconForServer, sp->partyPlayer, 0 );
	PokeCon_AddParty( &wk->pokeconForServer, sp->partyEnemy1, 1 );

	// Server �쐬
	wk->server = BTL_SERVER_Create( wk, &wk->pokeconForServer, wk->heapID );

	// Client �쐬
	wk->client[0] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, BTL_COMM_NONE, sp->netHandle, 0, 2, BTL_THINKER_UI, wk->heapID );
	wk->client[1] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, BTL_COMM_NONE, sp->netHandle, 1, 2, BTL_THINKER_AI, wk->heapID );
	wk->numClients = 2;
	wk->posCoverClientID[BTL_POS_1ST_0] = 0;
	wk->posCoverClientID[BTL_POS_2ND_0] = 1;
	wk->posCoverClientID[BTL_POS_1ST_1] = 0;
	wk->posCoverClientID[BTL_POS_2ND_1] = 1;

	// �`��G���W������
	wk->viewCore = BTLV_Create( wk, wk->client[0], &wk->pokeconForClient, HEAPID_BTL_VIEW );

	// �v���C���[�N���C�A���g�ɕ`��G���W�����֘A�t����
	BTL_CLIENT_AttachViewCore( wk->client[0], wk->viewCore );

	// Server �� Client ��ڑ�
	BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[0]), 0, 2 );
	BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[1]), 1, 2 );

	// Server �n��
	BTL_SERVER_Startup( wk->server );

	wk->mainLoop = MainLoop_StandAlone;

	return TRUE;
}
//--------------------------------------------------------------------------
/**
 * �X�^���h�A���[���^�_�u���o�g���F�N���[���A�b�v
 */
//--------------------------------------------------------------------------
static BOOL cleanup_alone_double( int* seq, void* work )
{
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	BTLV_Delete( wk->viewCore );

	BTL_CLIENT_Delete( wk->client[0] );
	BTL_CLIENT_Delete( wk->client[1] );
	BTL_SERVER_Delete( wk->server );

	PokeCon_Release( &wk->pokeconForClient );
	PokeCon_Release( &wk->pokeconForServer );

	return TRUE;
}
//--------------------------------------------------------------------------
/**
 * �ʐM�^�_�u���o�g���F�Z�b�g�A�b�v
 */
//--------------------------------------------------------------------------
static BOOL setup_comm_double( int* seq, void* work )
{
	BTL_MAIN_MODULE* wk = work;
	const BATTLE_SETUP_PARAM* sp = wk->setupParam;

	switch( *seq ){
	case 0:
		if( BTL_NET_IsServerDetermained() )
		{
			wk->numClients = (sp->multiMode==0)? 2 : 4;

			BTL_Printf("�T�[�o���肵�܂�����B�N���C�A���g����%d\n", wk->numClients);

			if( BTL_NET_IsServer() )
			{
				// �T�[�o�[�}�V�����A�e�Q���҂ɃN���C�A���gID������U��
				u8 clientID, numCoverPos;
				u8 i;

				BTL_UTIL_SetPrintType( BTL_PRINTTYPE_SERVER );
				BTL_Printf("���V�A�T�[�o�[�ł��B\n");

				numCoverPos = (sp->multiMode==0)? 2 : 1;
				for(i=0; i<wk->numClients; ++i)
				{
					clientID = i;
					BTL_NET_NotifyClientID( i, &clientID, numCoverPos );
				}
			}
			else
			{
				BTL_UTIL_SetPrintType( BTL_PRINTTYPE_CLIENT );
				BTL_Printf("���V�A�T�[�o�[�ł͂Ȃ��B\n");
			}
			(*seq)++;
		}
		break;
	case 1:
		#ifdef PM_DEBUG
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{
			BTL_Printf("�N���C�A���gID�̊m��҂��Ł[��\n");
		}
		#endif
		// �����̃N���C�A���gID���m��
		if( BTL_NET_IsClientIdDetermined() )
		{
			wk->myClientID = GFL_NET_GetNetID( sp->netHandle );
			BTL_Printf("�N���C�A���gID��%d�Ɋm�聨�^�C�~���O�V���N��\n", wk->myClientID);
			BTL_NET_TimingSyncStart( BTL_NET_TIMING_CLIENTID_DETERMINE );
			(*seq)++;
		}
		break;
	case 2:
		if( BTL_NET_IsTimingSync(BTL_NET_TIMING_CLIENTID_DETERMINE) )
		{
			BTL_Printf("�^�C�~���O�V���N�����܂����B\n");
			BTL_NET_StartNotifyPartyData( sp->partyPlayer );
			(*seq)++;
		}
		break;
	case 3:
		// �p�[�e�B�f�[�^���ݎ�M������
		if( BTL_NET_IsCompleteNotifyPartyData() )
		{
			u8 i;
			PokeCon_Init( &wk->pokeconForClient, wk );
			// @@@ �Ⴆ��numCliens=3��������AClientID����U���0�`2�ł����̂��H���Ԃ�_���B
			for(i=0; i<wk->numClients; ++i)
			{
				PokeCon_AddParty( &wk->pokeconForClient, BTL_NET_GetPartyData(i), i );
			}
			(*seq)++;
		}
		break;
	case 4:
		if( BTL_NET_IsServer() )
		{
			u8 i;
			PokeCon_Init( &wk->pokeconForServer, wk );
			// @@@ �Ⴆ��numCliens=3��������AClientID����U���0�`2�ł����̂��H���Ԃ�_���B
			for(i=0; i<wk->numClients; ++i)
			{
				PokeCon_AddParty( &wk->pokeconForServer, BTL_NET_GetPartyData(i), i );
			}
		}
		(*seq)++;
		break;
	case 5:
		BTL_NET_EndNotifyPartyData();
		(*seq)++;
		break;
	case 6:
		if( sp->multiMode == 0 )
		{
			wk->posCoverClientID[BTL_POS_1ST_0] = 0;
			wk->posCoverClientID[BTL_POS_2ND_0] = 1;
			wk->posCoverClientID[BTL_POS_1ST_1] = 0;
			wk->posCoverClientID[BTL_POS_2ND_1] = 1;
		}
		else
		{
			wk->posCoverClientID[BTL_POS_1ST_0] = 0;
			wk->posCoverClientID[BTL_POS_2ND_0] = 1;
			wk->posCoverClientID[BTL_POS_1ST_1] = 2;
			wk->posCoverClientID[BTL_POS_2ND_1] = 3;
		}
		{
			u8 i;

			wk->myOrgPos = 0;
			for(i=0; i<NELEMS(wk->posCoverClientID); ++i)
			{
				if( wk->posCoverClientID[i] == wk->myClientID )
				{
					wk->myOrgPos = i;
					break;
				}
			}
		}

		// �������T�[�o
		if( BTL_NET_IsServer() )
		{
			u8 netID, numCoverPos, i;

			netID = GFL_NET_GetNetID( sp->netHandle );
			numCoverPos = (sp->multiMode==0)? 2 : 1;

			BTL_Printf("�T�[�o�p�̃p�[�e�B�f�[�^�Z�b�g\n");

			wk->server = BTL_SERVER_Create( wk, &wk->pokeconForServer, wk->heapID );
			wk->client[netID] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, sp->commMode, sp->netHandle,
					netID, numCoverPos, BTL_THINKER_UI, wk->heapID );
			BTL_SERVER_AttachLocalClient( wk->server, BTL_CLIENT_GetAdapter(wk->client[netID]), netID, numCoverPos );

			for(i=0; i<wk->numClients; ++i)
			{
				if(i==netID){ continue; }
				BTL_SERVER_ReceptionNetClient( wk->server, sp->commMode, sp->netHandle, i, numCoverPos );
			}

			// �`��G���W������
			wk->viewCore = BTLV_Create( wk, wk->client[netID], &wk->pokeconForClient, HEAPID_BTL_VIEW );
			BTL_CLIENT_AttachViewCore( wk->client[netID], wk->viewCore );

			// Server �n��
			BTL_SERVER_Startup( wk->server );
			wk->mainLoop = MainLoop_Comm_Server;
		}
		// �������T�[�o�ł͂Ȃ�
		else
		{
			u8 netID, numCoverPos;

			netID = GFL_NET_GetNetID( sp->netHandle );
			numCoverPos = (sp->multiMode==0)? 2 : 1;

			BTL_Printf("�T�[�o�ł͂Ȃ��p�̃p�[�e�B�f�[�^�Z�b�g\n");

			wk->client[ netID ] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, sp->commMode, sp->netHandle,
					netID, numCoverPos, BTL_THINKER_UI, wk->heapID  );

			// �`��G���W������
			wk->viewCore = BTLV_Create( wk, wk->client[netID], &wk->pokeconForClient, HEAPID_BTL_VIEW );
			BTL_CLIENT_AttachViewCore( wk->client[netID], wk->viewCore );

			wk->mainLoop = MainLoop_Comm_NotServer;
		}
		(*seq)++;
		break;
	case 7:
		BTL_Printf("�Z�b�g�A�b�v����\n");
		return TRUE;
	}
	return FALSE;
}

//======================================================================================================
//======================================================================================================


static BOOL MainLoop_StandAlone( BTL_MAIN_MODULE* wk )
{
	BOOL quitFlag = FALSE;
	int i;

	BTL_SERVER_Main( wk->server );

	for(i=0; i<2; i++)
	{
		if( BTL_CLIENT_Main(wk->client[i]) )
		{
			quitFlag = TRUE;
		}
	}

	BTLV_CORE_Main( wk->viewCore );

	return quitFlag;
}


static BOOL MainLoop_Comm_Server( BTL_MAIN_MODULE* wk )
{
	BOOL quitFlag = FALSE;
	int i;

	BTL_SERVER_Main( wk->server );

	for(i=0; i<wk->numClients; i++)
	{
		if( wk->client[i] )
		{
			if( BTL_CLIENT_Main(wk->client[i]) )
			{
				quitFlag = TRUE;
			}
		}
	}

	BTLV_CORE_Main( wk->viewCore );

	return quitFlag;
}

static BOOL MainLoop_Comm_NotServer( BTL_MAIN_MODULE* wk )
{
	BOOL quitFlag = FALSE;
	int i;

	for(i=0; i<wk->numClients; i++)
	{
		if( wk->client[i] )
		{
			if( BTL_CLIENT_Main(wk->client[i]) )
			{
				quitFlag = TRUE;
			}
		}
	}

	BTLV_CORE_Main( wk->viewCore );

	return quitFlag;
}




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
 * �ΐ푊��^�C�v��Ԃ�
 *
 * @param   wk		
 *
 * @retval  BtlCompetitor		
 */
//=============================================================================================
BtlCompetitor BTL_MAIN_GetCompetitor( const BTL_MAIN_MODULE* wk )
{
	return wk->setupParam->competitor;
}
//=============================================================================================
/**
 * �ʐM�^�C�v��Ԃ�
 *
 * @param   wk		
 *
 * @retval  BtlCommMode		
 */
//=============================================================================================
BtlCommMode BTL_MAIN_GetCommMode( const BTL_MAIN_MODULE* wk )
{
	return wk->setupParam->commMode;
}
//=============================================================================================
/**
 * �u�ɂ���v��I���������̔����^�C�v
 *
 * @param   wk		
 *
 * @retval  BtlEscapeMode		�����^�C�v
 */
//=============================================================================================
BtlEscapeMode BTL_MAIN_GetEscapeMode( const BTL_MAIN_MODULE * wk )
{
	// @@@ ���܂̂Ƃ����ɋ�����
	return BTL_ESCAPE_MODE_OK;
}


//=============================================================================================
/**
 * ����N���C�A���gID�w��q���A���ۂ̑ΏۃN���C�A���gID�ɕϊ�
 *
 * @param   wk						���C�����W���[���n���h��
 * @param   exPos					����|�P�����ʒu�w��q
 * @param   dst						[out] �ΏۃN���C�A���gID���i�[����o�b�t�@
 *
 * @retval  u8		�ΏۃN���C�A���g��
 */
//=============================================================================================
u8 BTL_MAIN_ExpandBtlPos( const BTL_MAIN_MODULE* wk, BtlExPos exPos, u8* dst )
{
	u8 exType = EXPOS_GET_TYPE( exPos );
	u8 basePos = EXPOS_GET_BASEPOS( exPos );

	if( exType == BTL_EXPOS_DEFAULT )
	{
		dst[0] = basePos;
		return 1;
	}

	switch( wk->setupParam->rule ){
	case BTL_RULE_SINGLE:
		return expandPokePos_single( wk, exType, basePos, dst );
	case BTL_RULE_DOUBLE:
	default:
		return expandPokePos_double( wk, exType, basePos, dst );
	}
}
// �V���O���p
static u8 expandPokePos_single( const BTL_MAIN_MODULE* wk, BtlExPos exType, u8 basePos, u8* dst )
{
	switch( exType ){
	default:
		GF_ASSERT(0);
		/* fallthru */
	case BTL_EXPOS_ENEMY_ALL:
	case BTL_EXPOS_WITHOUT_ME:
		dst[0] = BTL_MAIN_GetOpponentPokePos( wk, basePos, 0 );
		break;
	case BTL_EXPOS_MYSIDE_ALL:
		dst[0] = basePos;
		break;
	case BTL_EXPOS_ALL:
		dst[0] = BTL_POS_1ST_0;
		dst[1] = BTL_POS_2ND_0;
		return 2;
	}
	return 1;
}
// �_�u���p
static u8 expandPokePos_double( const BTL_MAIN_MODULE* wk, BtlExPos exType, u8 basePos, u8* dst )
{
	switch( exType ){
	default:
		GF_ASSERT(0);
		/* fallthru */
	case BTL_EXPOS_ENEMY_ALL:
		dst[0] = BTL_MAIN_GetOpponentPokePos( wk, basePos, 0 );
		dst[1] = BTL_MAIN_GetOpponentPokePos( wk, basePos, 1 );
		return 2;
	case BTL_EXPOS_WITHOUT_ME:
		dst[0] = BTL_MAIN_GetOpponentPokePos( wk, basePos, 0 );
		dst[1] = BTL_MAIN_GetOpponentPokePos( wk, basePos, 1 );
		dst[2] = BTL_MAIN_GetNextPokePos( wk, basePos );
		return 3;
	case BTL_EXPOS_MYSIDE_ALL:
		dst[0] = basePos;
		dst[1] = BTL_MAIN_GetNextPokePos( wk, basePos );
		if( dst[0] > dst[1] )
		{
			u8 tmp = dst[0];
			dst[0] = dst[1];
			dst[1] = tmp;
		}
		return 2;
	case BTL_EXPOS_ALL:
		dst[0] = BTL_POS_1ST_0;
		dst[1] = BTL_POS_2ND_0;
		dst[2] = BTL_POS_1ST_1;
		dst[3] = BTL_POS_2ND_1;
		return 4;
	}
}
//
//	�N���C�A���gID -> �T�C�hID�ɕϊ�
//
static inline clientID_to_side( u8 clientID )
{
	return clientID & 1;
}

//=============================================================================================
/**
 * �N���C�A���gID����T�C�hID��Ԃ�
 *
 * @param   wk				
 * @param   clientID	
 *
 * @retval  BtlSide		
 */
//=============================================================================================
BtlSide BTL_MAIN_GetClientSide( const BTL_MAIN_MODULE* wk, u8 clientID )
{
	return clientID_to_side( clientID );
}

//=============================================================================================
/**
 * �N���C�A���gID����|�P�����퓬�ʒu��Ԃ�
 *
 * @param   wk				
 * @param   basePos		
 * @param   idx				
 *
 * @retval  BtlPokePos		
 */
//=============================================================================================
BtlPokePos BTL_MAIN_GetClientPokePos( const BTL_MAIN_MODULE* wk, u8 clientID, u8 posIdx )
{
	u8 i;

	for(i=0; i<NELEMS(wk->posCoverClientID); ++i)
	{
		if( wk->posCoverClientID[i] == clientID )
		{
			if( posIdx == 0 )
			{
				return i;
			}
			--posIdx;
		}
	}
//	GF_ASSERT(0);
	return BTL_POS_MAX;
}
//=============================================================================================
/**
 * �ΐ푊����̃|�P�����퓬�ʒu��Ԃ�
 *
 * @param   wk				
 * @param   basePos		
 * @param   idx				
 *
 * @retval  BtlPokePos		
 */
//=============================================================================================
BtlPokePos BTL_MAIN_GetOpponentPokePos( const BTL_MAIN_MODULE* wk, BtlPokePos basePos, u8 idx )
{
	switch( wk->setupParam->rule ){
	case BTL_RULE_SINGLE:
		GF_ASSERT(idx<1);
		break;
	case BTL_RULE_DOUBLE:
		GF_ASSERT(idx<2)
		break;
	case BTL_RULE_TRIPLE:
		GF_ASSERT(idx<3);
		break;
	default:
		GF_ASSERT(0);
		break;
	}
	if( (basePos&1) == BTL_POS_1ST_0 )
	{
		return BTL_POS_2ND_0 + (idx * 2);
	}
	else
	{
		return BTL_POS_1ST_0 + (idx * 2);
	}
}
//=============================================================================================
/**
 * �w��ʒu���猩�ėׂ�̐퓬�ʒu��Ԃ�
 *
 * @param   wk				
 * @param   basePos		
 * @param   idx				
 *
 * @retval  BtlPokePos		
 */
//=============================================================================================
BtlPokePos BTL_MAIN_GetNextPokePos( const BTL_MAIN_MODULE* wk, BtlPokePos basePos )
{
	switch( wk->setupParam->rule ){
	case BTL_RULE_SINGLE:
		GF_ASSERT(0);
		return basePos;
	case BTL_RULE_DOUBLE:
		{
			u8 retPos = (basePos + 2) & 0x03;
			BTL_Printf("nextPos %d -> %d\n", basePos, retPos);
			return retPos;
		}
	case BTL_RULE_TRIPLE:
		GF_ASSERT(0);
		return basePos;
	default:
		GF_ASSERT(0);
		return basePos;
	}
}
//=============================================================================================
/**
 * �Q�̃N���C�A���gID���ΐ푊�蓯�m�̂��̂��ǂ����𔻕�
 *
 * @param   wk		
 * @param   clientID1		
 * @param   clientID2		
 *
 * @retval  BOOL		�ΐ푊�蓯�m�Ȃ�TRUE
 */
//=============================================================================================
BOOL BTL_MAIN_IsOpponentClientID( const BTL_MAIN_MODULE* wk, u8 clientID1, u8 clientID2 )
{
	return (clientID1&1) != (clientID2&1);
}

//--------------------------------------------------------------------------
/**
 * �퓬�ʒu->�N���C�A���gID�� �ϊ�
 *
 * @param   wk		
 * @param   btlPos		
 *
 * @retval  inline u8		
 */
//--------------------------------------------------------------------------
static inline u8 btlPos_to_clientID( const BTL_MAIN_MODULE* wk, BtlPokePos btlPos )
{
	GF_ASSERT(btlPos < NELEMS(wk->posCoverClientID));
	GF_ASSERT(wk->posCoverClientID[btlPos] != BTL_CLIENT_MAX);

	return wk->posCoverClientID[btlPos];
}
//--------------------------------------------------------------------------
/**
 * �퓬�ʒu->�N���C�A���gID�C�ʒu�C���f�N�X�i�N���C�A���g����0�I���W���C�A�ԁj�ɕϊ�
 *
 * @param   wk				[in] ���C�����W���[���̃n���h��
 * @param   btlPos		[in] �퓬�ʒu
 * @param   clientID	[out]
 * @param   posIdx		[out]
 *
 */
//--------------------------------------------------------------------------
static inline void btlPos_to_cliendID_and_posIdx( const BTL_MAIN_MODULE* wk, BtlPokePos btlPos, u8* clientID, u8* posIdx )
{
	*clientID = btlPos_to_clientID( wk, btlPos );

	{
		u8 idx = 0;
		while( btlPos-- )
		{
			if( wk->posCoverClientID[ btlPos ] == *clientID  )
			{
				++idx;
			}
		}

		*posIdx = idx;
	}
}
//
// �퓬�ʒu -> ���T�C�h���̈ʒu�C���f�b�N�X�֕ϊ�
//
static inline u8 btlPos_to_sidePosIdx( BtlPokePos pos )
{
	return pos / 2;
}
// �|�P����ID -> �N���C�A���gID�ϊ�
static inline u8 PokeID_to_ClientID( u8 pokeID )
{
	u8 i, min, max;
	for(i=0; i<NELEMS(ClientBasePokeID); ++i)
	{
		min = ClientBasePokeID[i];
		max = min + TEMOTI_POKEMAX - 1;
		if( (pokeID >= min) && (pokeID <= max) )
		{
			return i;
		}
	}
	GF_ASSERT_MSG(0, "Illegal PokeID[%d]", pokeID);
	return 0;
}
//=============================================================================================
/**
 * �o�g���|�P����ID���|�P�����퓬�ʒu�ɕϊ�
 *
 * @param   wk		
 * @param   pokeID		
 *
 * @retval  BtlPokePos		�|�P�����퓬�ʒu
 */
//=============================================================================================
BtlPokePos BTL_MAIN_PokeIDtoPokePos( const BTL_MAIN_MODULE* wk, u8 pokeID )
{
	u8 clientID = PokeID_to_ClientID( pokeID );

	int idx = PokeCon_FindPokemon( &wk->pokeconForServer, clientID, pokeID );
	if( idx >= 0 )
	{
		BtlPokePos pos = BTL_MAIN_GetClientPokePos( wk, clientID, idx );
		if( pos != BTL_POS_MAX )
		{
			return pos;
		}
		GF_ASSERT_MSG(0, " not fighting pokeID [%d]", pokeID );
	}
	GF_ASSERT_MSG(0, " not including pokeID [%d] (clientID=%d)", pokeID, clientID );
	return 0;
}
//=============================================================================================
/**
 * �o�g���|�P����ID����A���̃|�P�������Ǘ�����N���C�A���gID���擾
 *
 * @param   wk		
 * @param   pokeID		
 *
 * @retval  u8		
 */
//=============================================================================================
u8 BTL_MAIN_PokeIDtoClientID( const BTL_MAIN_MODULE* wk, u8 pokeID )
{
	return PokeID_to_ClientID( pokeID );
}

//=============================================================================================
/**
 * �퓬�ʒu->�N���C�A���gID�ϊ�
 *
 * @param   wk		
 * @param   pos		
 *
 */
//=============================================================================================
u8 BTL_MAIN_BtlPosToClientID( const BTL_MAIN_MODULE* wk, BtlPokePos pos )
{
	return btlPos_to_clientID( wk, pos );
}

//=============================================================================================
/**
 * �퓬�ʒu -> �N���C�A���gID�C�|�P�����C���f�b�N�X�ϊ�
 *
 * @param   wk		
 * @param   pos		
 * @param   clientID		
 * @param   posIdx		
 *
 */
//=============================================================================================
void BTL_MAIN_BtlPosToClientID_and_PosIdx( const BTL_MAIN_MODULE* wk, BtlPokePos pos, u8* clientID, u8* posIdx )
{
	btlPos_to_cliendID_and_posIdx( wk, pos, clientID, posIdx );
}

//=============================================================================================
/**
 * �퓬�ʒu -> �`��ʒu�ϊ�
 *
 * @param   wk		
 * @param   pos		
 *
 * @retval  u8		
 */
//=============================================================================================
u8 BTL_MAIN_BtlPosToViewPos( const BTL_MAIN_MODULE* wk, BtlPokePos pos )
{
	u8 isPlayerSide;

	isPlayerSide = ( (wk->myOrgPos&1) == (pos&1) );

	if( wk->setupParam->rule == BTL_RULE_SINGLE )
	{
		return (isPlayerSide)? BTLV_MCSS_POS_AA : BTLV_MCSS_POS_BB;
	}
	else
	{
		static const u8 vpos[2][BTL_POSIDX_MAX] = {
			{ BTLV_MCSS_POS_B, BTLV_MCSS_POS_D, BTLV_MCSS_POS_F },
			{ BTLV_MCSS_POS_A, BTLV_MCSS_POS_C, BTLV_MCSS_POS_E },
		};
		u8 posIdx = btlPos_to_sidePosIdx( pos );
		return vpos[ isPlayerSide ][ posIdx ];
	}
}

//=============================================================================================
/**
 * �������m�̃|�P����ID���ǂ����𔻒肷��
 *
 * @param   pokeID1		
 * @param   pokeID2		
 *
 * @retval  BOOL		�������m�Ȃ�TRUE
 */
//=============================================================================================
BOOL BTL_MAINUTIL_IsFriendPokeID( u8 pokeID1, u8 pokeID2 )
{
	u8 side1 = (pokeID1 < (TEMOTI_POKEMAX*2));
	u8 side2 = (pokeID2 < (TEMOTI_POKEMAX*2));
	return side1 == side2;
}

//=======================================================================================================
// BTL_POKE_CONTAINER
//=======================================================================================================

static void PokeCon_Init( BTL_POKE_CONTAINER* pokecon, BTL_MAIN_MODULE* mainModule )
{
	int i;

	pokecon->mainModule = mainModule;

	for(i=0; i<NELEMS(pokecon->party); ++i)
	{
		BTL_PARTY_Initialize( &pokecon->party[i] );
	}
	for(i=0; i<NELEMS(pokecon->pokeParam); ++i)
	{
		pokecon->pokeParam[i] = NULL;
	}
}
static void PokeCon_AddParty( BTL_POKE_CONTAINER* pokecon, const POKEPARTY* party_src, u8 clientID )
{
	u8 pokeID = ClientBasePokeID[ clientID ];
	BTL_PARTY* party = &pokecon->party[ clientID ];
	u32 i, max = PokeParty_GetPokeCount( party_src );

	for(i=0; i<max; ++i, ++pokeID)
	{
		pokecon->pokeParam[ pokeID ] = BTL_POKEPARAM_Create(
												PokeParty_GetMemberPointer(party_src, i), pokeID, HEAPID_BTL_SYSTEM
		);

		#ifdef PM_DEBUG
		// ���f�o�b�O�[�u
		/*
		if (pokeID == 0 )
		{
			BTL_POKEPARAM_ChangeTokusei( pokecon->pokeParam[ pokeID ], POKETOKUSEI_KANSOUHADA );
		}
		*/
		#endif
		BTL_Printf(" Create PokeParam ID=%d, adrs=%p\n", pokeID, pokecon->pokeParam[i]);

		BTL_PARTY_AddMember( party, pokecon->pokeParam[ pokeID ] );
	}
}
static void PokeCon_Release( BTL_POKE_CONTAINER* pokecon )
{
	u32 i;
	for(i=0; i<NELEMS(pokecon->pokeParam); ++i)
	{
		if( pokecon->pokeParam[i] )
		{
			BTL_Printf(" Delete PokeParam ID=%d, adrs=%p\n", i, pokecon->pokeParam[i]);
			BTL_POKEPARAM_Delete( pokecon->pokeParam[i] );
			pokecon->pokeParam[i] = NULL;
		}
	}
}

static int PokeCon_FindPokemon( const BTL_POKE_CONTAINER* pokecon, u8 clientID, u8 pokeID )
{
	const BTL_POKEPARAM* bpp;
	const BTL_PARTY* party = &pokecon->party[ clientID ];
	u32 i, max = BTL_PARTY_GetMemberCount( party );

	for(i=0; i<max; ++i)
	{
		bpp = BTL_PARTY_GetMemberDataConst( party, i );
		if( BTL_POKEPARAM_GetID(bpp) == pokeID )
		{
			return i;
		}
	}
	return -1;
}

//=============================================================================================
/**
 * �퓬�ʒu����|�P�����f�[�^�̎擾
 *
 * @param   wk		���C�����W���[���̃n���h��
 * @param   pos		�����ʒu
 *
 * @retval  BTL_POKEPARAM*		
 */
//=============================================================================================
BTL_POKEPARAM* BTL_POKECON_GetFrontPokeData( BTL_POKE_CONTAINER* wk, BtlPokePos pos )
{
	return (BTL_POKEPARAM*)BTL_POKECON_GetFrontPokeDataConst(wk, pos);
}
//=============================================================================================
/**
 * �퓬�ʒu����|�P�����f�[�^�̎擾�iconst�j
 *
 * @param   wk		���C�����W���[���̃n���h��
 * @param   pos		�����ʒu
 *
 * @retval  const BTL_POKEPARAM*		
 */
//=============================================================================================
const BTL_POKEPARAM* BTL_POKECON_GetFrontPokeDataConst( const BTL_POKE_CONTAINER* wk, BtlPokePos pos )
{
	const BTL_PARTY* party;
	u8 clientID, posIdx;

	btlPos_to_cliendID_and_posIdx( wk->mainModule, pos, &clientID, &posIdx );

	party = &wk->party[ clientID ];

	return BTL_PARTY_GetMemberDataConst( party, posIdx );
}
//=============================================================================================
/**
 * �N���C�A���gID����|�P�����f�[�^�̎擾
 *
 * @param   wk		
 * @param   clientID		
 * @param   posIdx		
 *
 * @retval  const BTL_POKEPARAM*		
 */
//=============================================================================================
BTL_POKEPARAM* BTL_POKECON_GetClientPokeData( BTL_POKE_CONTAINER* wk, u8 clientID, u8 posIdx )
{
	BTL_PARTY* party;
	party = &wk->party[ clientID ];
	return BTL_PARTY_GetMemberData( party, posIdx );
}
//=============================================================================================
/**
 * �N���C�A���g�|�P�����f�[�^�̎擾 ( const )
 *
 * @param   wk		
 * @param   clientID		
 * @param   memberIdx		
 *
 * @retval  const BTL_POKEPARAM*		
 */
//=============================================================================================
const BTL_POKEPARAM* BTL_POKECON_GetClientPokeDataConst( const BTL_POKE_CONTAINER* wk, u8 clientID, u8 memberIdx )
{
	u8 pokeID = ClientBasePokeID[ clientID ] + memberIdx;
	return wk->pokeParam[ pokeID ];
}

BTL_POKEPARAM* BTL_POKECON_GetPokeParam( BTL_POKE_CONTAINER* wk, u8 pokeID )
{
	GF_ASSERT(pokeID<BTL_COMMITMENT_POKE_MAX);
	GF_ASSERT(wk->pokeParam[pokeID]);
	return wk->pokeParam[ pokeID ];
}

const BTL_POKEPARAM* BTL_POKECON_GetPokeParamConst( const BTL_POKE_CONTAINER* wk, u8 pokeID )
{
	GF_ASSERT(pokeID<BTL_COMMITMENT_POKE_MAX);
	GF_ASSERT(wk->pokeParam[pokeID]);

	return wk->pokeParam[ pokeID ];
}

//------------------------------------------------------------------------------
/**
 * �w��N���C�A���g�̃p�[�e�B�f�[�^��Ԃ�
 *
 * @param   wk		
 * @param   clientID		
 *
 * @retval  const POKEPARTY*		
 */
//------------------------------------------------------------------------------
BTL_PARTY* BTL_POKECON_GetPartyData( BTL_POKE_CONTAINER* wk, u32 clientID )
{
	return &wk->party[ clientID ];
}
const BTL_PARTY* BTL_POKECON_GetPartyDataConst( const BTL_POKE_CONTAINER* wk, u32 clientID )
{
	return &wk->party[ clientID ];
}

//=======================================================================================================
// BTL_PARTY
//=======================================================================================================

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

void BTL_PARTY_SwapMembers( BTL_PARTY* party, u8 idx1, u8 idx2 )
{
	GF_ASSERT(idx1<party->memberCount);
	GF_ASSERT(idx2<party->memberCount);
	{
		BTL_POKEPARAM* tmp = party->member[ idx1 ];
		party->member[ idx1 ] = party->member[ idx2 ];
		party->member[ idx2 ] = tmp;
	}
}

s16 BTL_PARTY_FindMember( const BTL_PARTY* party, const BTL_POKEPARAM* param )
{
	int i;
	for(i=0; i<party->memberCount; ++i)
	{
		if( party->member[i] == param )
		{
			return i;
		}
	}
	return -1;
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
	u32 i;
	for(i=0; i<NELEMS(wk->pokeconForServer.pokeParam); ++i)
	{
		if( wk->pokeconForServer.pokeParam[i] != NULL )
		{
			BTL_POKEPARAM_Copy(
				wk->pokeconForServer.pokeParam[i], 
				wk->pokeconForClient.pokeParam[i] );
		}
	}
}


