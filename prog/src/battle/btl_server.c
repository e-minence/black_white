//=============================================================================================
/**
 * @file	btl_server.c
 * @brief	ポケモンWB バトルシステム	サーバモジュール
 * @author	taya
 *
 * @date	2008.09.06	作成
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
#include "btl_field.h"
#include "btl_server_flow.h"
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



//--------------------------------------------------------------
/**
 *	サーバメインループ関数型  - バトル終了時のみ TRUE を返す -
 */
//--------------------------------------------------------------
typedef BOOL (*ServerMainProc)(BTL_SERVER*, int*);


//--------------------------------------------------------------
/**
 *	
 */
//--------------------------------------------------------------
struct _BTL_SERVER {
	ServerMainProc		mainProc;
	int								seq;

	BTL_MAIN_MODULE*			mainModule;
	BTL_POKE_CONTAINER*		pokeCon;
	SVCL_WORK					client[ BTL_CLIENT_MAX ];
	BTL_SVFLOW_WORK*	flowWork;
	u8					numClient;
	u8					pokeDeadFlag;
	u8					quitStep;

	BTL_SERVER_CMD_QUE	queBody;
	BTL_SERVER_CMD_QUE*	que;

	HEAPID				heapID;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline void setup_client_members( SVCL_WORK* client, BTL_PARTY* party, u8 numCoverPos );
static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc );
static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectPokemon( BTL_SERVER* server, int* seq );
static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd );
static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize );
static BOOL WaitAdapterCmd( BTL_SERVER* server );
static void ResetAdapterCmd( BTL_SERVER* server );





//--------------------------------------------------------------------------------------
/**
 * サーバ生成
 *
 * @param   mainModule		バトルメインモジュールハンドラ
 * @param   heapID			生成用ヒープID
 *
 * @retval  BTL_SERVER*		生成されたサーバハンドラ
 */
//--------------------------------------------------------------------------------------
BTL_SERVER* BTL_SERVER_Create( BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon, HEAPID heapID )
{
	BTL_SERVER* sv = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER) );

	sv->mainModule = mainModule;
	sv->pokeCon = pokeCon;
	sv->numClient = 0;
	sv->heapID = heapID;
	sv->que = &sv->queBody;
	sv->quitStep = QUITSTEP_NONE;
	sv->flowWork = NULL;

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
 * サーバと同一マシン上にあるクライアントとのアダプタを接続する
 *
 * @param   server			サーバハンドラ
 * @param   adapter			アダプタ（既にクライアントに関連付けられている）
 * @param   clientID		クライアントID
 * @param   numCoverPos	クライアントが受け持つ戦闘中ポケモン数
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_AttachLocalClient( BTL_SERVER* server, BTL_ADAPTER* adapter, u8 clientID, u8 numCoverPos )
{
	GF_ASSERT(clientID < BTL_CLIENT_MAX);
	GF_ASSERT(server->client[clientID].adapter==NULL);

	{
		SVCL_WORK* client;
		BTL_PARTY* party;

		client = &server->client[ clientID ];
		party = BTL_POKECON_GetPartyData( server->pokeCon, clientID );

		client->adapter = adapter;
		client->myID = clientID;
		client->isLocalClient = TRUE;

		setup_client_members( client, party, numCoverPos );
	}

	server->numClient++;
}
//--------------------------------------------------------------------------------------
/**
 * サーバと異なるマシン上にあるクライアントとのアダプタを生成する
 *
 * @param   server			サーバハンドラ
 * @param   commMode		通信モード
 * @param   netHandle		ネットワークハンドラ
 * @param   clientID		クライアントID
 * @param   numCoverPos	クライアントが受け持つ戦闘中ポケモン数
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_ReceptionNetClient( BTL_SERVER* server, BtlCommMode commMode, GFL_NETHANDLE* netHandle, u8 clientID, u8 numCoverPos )
{
	GF_ASSERT(clientID < BTL_CLIENT_MAX);
	GF_ASSERT(server->client[clientID].adapter==NULL);

	{
		SVCL_WORK* client;
		BTL_PARTY* party;

		client = &server->client[ clientID ];
		party = BTL_POKECON_GetPartyData( server->pokeCon, clientID );

		client->adapter = BTL_ADAPTER_Create( netHandle, server->heapID, clientID );
		client->myID = clientID;
		client->isLocalClient = FALSE;

		setup_client_members( client, party, numCoverPos );
	}

	server->numClient++;
}
//--------------------------------------------------------------------------
/**
 * クライアントワークのメンバーデータ部分初期化
 *
 * @param   client				クライアントワーク
 * @param   party					パーティデータ
 * @param   numCoverPos		クライアントが受け持つ戦闘中ポケモン数
 */
//--------------------------------------------------------------------------
static inline void setup_client_members( SVCL_WORK* client, BTL_PARTY* party, u8 numCoverPos )
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
 * 全クライアント生成・接続完了後のスタートアップ処理
 *
 * @param   server		
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_Startup( BTL_SERVER* server )
{
	GF_ASSERT(server->flowWork==NULL);
	server->flowWork = BTL_SVFLOW_InitSystem( server, server->mainModule, server->pokeCon,
				&server->queBody, server->numClient, server->heapID );
}
//--------------------------------------------------------------------------------------
/**
 * サーバ削除
 *
 * @param   wk		
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_Delete( BTL_SERVER* wk )
{
	int i;

	BTL_SVFLOW_QuitSystem( wk->flowWork );

	for(i=0; i<BTL_CLIENT_MAX; ++i)
	{
		if( (wk->client[i].myID != CLIENT_DISABLE_ID)
		&&	(wk->client[i].isLocalClient == FALSE)
		){
			// 同一マシン上にあるクライアントでなければ、
			// サーバがアダプタを用意したハズなので自分で削除
			BTL_ADAPTER_Delete( wk->client[i].adapter );
		}
	}
	GFL_HEAP_FreeMemory( wk );
}
//--------------------------------------------------------------------------------------
/**
 * サーバメインループ
 *
 * @param   sv			サーバハンドラ
 *
 * @retval  BOOL		終了時 TRUE を返す
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
		BTL_Printf("イニシャライズコマンド発行\n");
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
		BTL_SVFLOW_Start_AfterPokemonIn( server->flowWork );
		if( server->que->writePtr )
		{
			BTL_Printf("再生コマンド発行\n");
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
		BTL_Printf("アクション選択コマンド発行\n");
		SetAdapterCmd( server, BTL_ACMD_SELECT_ACTION );
		(*seq)++;
		break;

	case 3:
		if( WaitAdapterCmd(server) )
		{
			ResetAdapterCmd( server );
			/*
			server->numActPokemon = sortClientAction( server, server->actOrder );
			BTL_Printf("全クライアントのアクションソート完了。処理アクション数=%d\n", server->numActPokemon);
			SCQUE_Init( server->que );
			server->pokeDeadFlag = ServerFlow_Start( server );
			*/
			server->pokeDeadFlag = BTL_SVFLOW_Start( server->flowWork );
			if( server->pokeDeadFlag )
			{
				BTL_Printf(" **** ポケモンしにました\n");
			}
			SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
			(*seq)++;
		}
		break;

	case 4:
		if( WaitAdapterCmd(server) )
		{
			BTL_Printf("コマンド再生おわりました\n");
			BTL_MAIN_SyncServerCalcData( server->mainModule );
			ResetAdapterCmd( server );
			if( server->quitStep )
			{
				BTL_Printf("サーバー終了\n");
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

			// 続行
			if( loseClientCount == 0 )
			{
				BTL_Printf("負けたクライアントいないので次のポケモン選択へ\n");
				SetAdapterCmd( server, BTL_ACMD_SELECT_POKEMON );
				(*seq)++;
			}
			// 決着
			else
			{
				// 勝敗決定
				if( loseClientCount == 1 )
				{
					server->quitStep = QUITSTEP_REQ;
				}
				// 引き分け
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
			server->pokeDeadFlag = BTL_SVFLOW_StartAfterPokeSelect( server->flowWork );
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

//--------------------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワークへのポインタを返す（存在しないとASSERT失敗）
 *
 * @param   server				
 * @param   clientID			
 *
 * @retval  BTL_SERVER_CLWORK*		
 */
//--------------------------------------------------------------------------------------
SVCL_WORK* BTL_SERVER_GetClientWork( BTL_SERVER* server, u8 clientID )
{
	GF_ASSERT_MSG( (clientID<NELEMS(server->client)), "clientID=%d", clientID);
	GF_ASSERT_MSG(server->client[clientID].myID != CLIENT_DISABLE_ID, "clientID=%d", clientID);
	return &(server->client[clientID]);
}
//--------------------------------------------------------------------------------------
/**
 * サーバ保持用クライアントワークへのポインタを返す（存在しなければNULLを返す）
 *
 * @param   server		
 * @param   clientID		
 *
 * @retval  SVCL_WORK*		
 */
//--------------------------------------------------------------------------------------
SVCL_WORK* BTL_SERVER_GetClientWorkIfEnable( BTL_SERVER* server, u8 clientID )
{
	if( server->client[clientID].myID != CLIENT_DISABLE_ID )
	{
		return &(server->client[clientID]);
	}
	return NULL;
}

u8 BTL_SVCL_GetNumActPoke( SVCL_WORK* clwk )
{
	GF_ASSERT(clwk->adapter);
	return BTL_ADAPTER_GetReturnDataCount( clwk->adapter );
}

u8 BTL_SVCL_GetNumCoverPos( SVCL_WORK* clwk )
{
	return clwk->numCoverPos;
}

const BTL_ACTION_PARAM* BTL_SVCL_GetPokeAction( SVCL_WORK* clwk, u8 posIdx )
{
	GF_ASSERT(clwk->adapter);
	GF_ASSERT(posIdx<clwk->numCoverPos);

	{
		const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( clwk->adapter );
		action += posIdx;
		return action;
	}
}

BTL_POKEPARAM* BTL_SVCL_GetFrontPokeData( SVCL_WORK* clwk, u8 posIdx )
{
	GF_ASSERT(posIdx<clwk->numCoverPos);
	return clwk->frontMember[ posIdx ];
}

