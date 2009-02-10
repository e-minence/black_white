//=============================================================================================
/**
 * @file	btl_net.c
 * @brief	ポケモンWB バトル  通信処理
 * @author	taya
 *
 * @date	2008.10.10	作成
 */
//=============================================================================================
#include <net.h>

#include "net\network_define.h"

#include "btl_common.h"
#include "btl_server_cmd.h"
#include "btl_net.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	NETID_NULL = 40,	///< GFL/Net系のNetIDとしてあり得ない値
};

enum {
	CMD_NOTIFY_SERVER_VER = GFL_NET_CMD_BATTLE,
	CMD_NOTIFY_CLIENT_ID,
	CMD_NOTIFY_PARTY_DATA,
	CMD_TO_CLIENT,
	CMD_TO_SERVER,
};


/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

//---------------------------------------------------
/**
 *	一時的送信バッファ
 */
//---------------------------------------------------
typedef struct {

	union {
		u32		val32;
		u16		val16[2];
		u8		val8[4];
	};

}TMP_SEND_BUFFER;

//---------------------------------------------------
/**
 *	サーババージョン相互通知処理用ワーク
 */
//---------------------------------------------------
typedef struct {

	struct {
		u8	version;
		u8	recvedFlag;
	}recvTable[ BTL_CLIENT_MAX ];

	u8	recvedCount;
	u8	maxVersion;

}SVVER_WORK;

//---------------------------------------------------
/**
 *	通常受信用バッファ
 */
//---------------------------------------------------
typedef struct {

	u8		data[ BTL_SERVER_CMD_QUE_SIZE ];
	u32		size;

}RECV_BUFFER;


typedef struct {

	GFL_NETHANDLE* netHandle;

	HEAPID	heapID;
	u8		myNetID;
	u8		serverNetID;

	u8		myClientID[ BTL_CLIENT_MAX ];

	u8		memberCount;
	u8		recvCount;

	u8		serverCmdReceived;
	u8		clientDataReturned;

	// 常駐受信バッファ
	RECV_BUFFER		recvServer;
	RECV_BUFFER		recvClient[ BTL_CLIENT_MAX ];

	// 一時利用バッファ
	void*	tmpLargeBuf[ BTL_NET_CONNECT_MACHINE_MAX ];
	u32		tmpLargeBufUsedSize[ BTL_NET_CONNECT_MACHINE_MAX ];

	TMP_SEND_BUFFER	sendBuf[ BTL_NET_CONNECT_MACHINE_MAX ];

	SVVER_WORK		svverWork;

}SYSWORK;

static SYSWORK* Sys;



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void recv_cliendID( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_partyData( int netID, void* pWork, int size );
static void recv_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_serverCmd( int netID, void* pWork, int size );
static void recv_clientData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_clientData( int netID, void* pWork, int size );
static inline void recvBuf_Store( RECV_BUFFER* buf, const void* pData, int size );
static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf );
static inline u32 recvBuf_getData( const RECV_BUFFER* buf, const void** ppData );
static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf );
static inline void recvBuf_clear( RECV_BUFFER* buf );



//static const NetRecvFuncTable RecvFuncTable[] = {
const NetRecvFuncTable BtlRecvFuncTable[] = {
    { recv_serverVer,	NULL },
    { recv_cliendID,	NULL },
    { recv_partyData,	getbuf_partyData },
    { recv_serverCmd,	getbuf_serverCmd },
    { recv_clientData,	getbuf_clientData },
};


void BTL_NET_InitSystem( GFL_NETHANDLE* netHandle, HEAPID heapID )
{
	if( netHandle )
	{
		int i;

		Sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(SYSWORK) );

		Sys->netHandle = netHandle;
		Sys->heapID = heapID;

		Sys->myNetID = GFL_NET_GetNetID( netHandle );
		Sys->serverNetID = NETID_NULL;

		Sys->serverCmdReceived = FALSE;
		Sys->clientDataReturned = FALSE;
		Sys->memberCount = GFL_NET_GetConnectNum();

		for(i=0; i<BTL_CLIENT_MAX; i++)
		{
			Sys->myClientID[i] = BTL_CLIENT_MAX;
		}

		for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
		{
			Sys->tmpLargeBuf[i] = NULL;
			Sys->tmpLargeBufUsedSize[i] = 0;
		}

		BTL_Printf("[BTLNET] 自分のネットID=%d, 接続メンバー数=%d\n", Sys->myNetID, Sys->memberCount);

		{
			TMP_SEND_BUFFER* tsb = &Sys->sendBuf[0];
			tsb->val32 = BTL_NET_SERVER_VERSION;
			GFL_NET_SendData( netHandle, CMD_NOTIFY_SERVER_VER, sizeof(*tsb), tsb );
		}
	}
}

void BTL_NET_QuitSystem( void )
{
	if( Sys )
	{
		GFL_HEAP_FreeMemory( Sys );
	}
}

// サーババージョン受信関数（全マシン相互）
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	const TMP_SEND_BUFFER* tsb = (const TMP_SEND_BUFFER*)pData;
	SVVER_WORK* swk = (SVVER_WORK*)(&Sys->svverWork);

	BTL_Printf("[BTLNET]サーババージョン受信 ... netID=%d, version=%d\n", netID, tsb->val32);

	if( swk->recvTable[netID].recvedFlag == FALSE )
	{
		u8 ver = tsb->val32;

		swk->recvTable[ netID ].version = ver;
		swk->recvTable[ netID ].recvedFlag = TRUE;

		if( swk->maxVersion < ver )
		{
			swk->maxVersion = ver;
		}

		swk->recvedCount++;
		// 全メンバーのサーババージョンを受け取ったら誰がサーバになるか決定
		if( swk->recvedCount >= Sys->memberCount )
		{
			int i;
			u8 numMaxVerMembers = 0;

			Sys->serverNetID = NETID_NULL;

			for(i=0; i<Sys->memberCount; i++)
			{
				if( swk->recvTable[i].version == swk->maxVersion )
				{
					Sys->serverNetID = i;
					BTL_Printf("[BTLNET]サーバは netID=%d のマシンに決定\n", i);
					break;
				}
			}

			GF_ASSERT(Sys->serverNetID != NETID_NULL);
		}
	}
}

// サーバとなるマシンが確定したかどうかチェック
BOOL BTL_NET_IsServerDetermained( void )
{
	return Sys->serverNetID != NETID_NULL;
}

// 自分がサーバかどうか判定
BOOL BTL_NET_IsServer( void )
{
	return ( Sys->serverNetID == Sys->myNetID );
}


// 各マシンにクライアントIDを通知する（サーバからのみ呼び出し）
void BTL_NET_NotifyClientID( NetID netID, const u8* clientID, u8 numCoverPos )
{
	GF_ASSERT(numCoverPos<=BTL_POSIDX_MAX);

	{
		TMP_SEND_BUFFER* tsbuf;
		u8 i;

		BTL_Printf("[BTLNET] netID=%d のマシンにクライアントIDを通知します。受け持ち場所数は%d,\n", netID, numCoverPos );
		BTL_Printf("  クライアントID=[ ");

		tsbuf = &Sys->sendBuf[netID];

		for(i=0; i<numCoverPos; i++)
		{
			tsbuf->val8[i+1] = clientID[i];
			BTL_Printf("%d ", clientID[i]);
		}
		BTL_Printf("]\n");
		tsbuf->val8[0] = numCoverPos;

		BTL_Printf("  送信サイズ=%d, 内容=%d,%d,%d,%d\n",
			sizeof(*tsbuf), tsbuf->val8[0], tsbuf->val8[1], tsbuf->val8[2], tsbuf->val8[3] );

/*
	BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand,
			const u32 size, const void* data,
			const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock )
*/
		GFL_NET_SendDataEx( Sys->netHandle, netID, CMD_NOTIFY_CLIENT_ID,
					sizeof( *tsbuf ),
					tsbuf,
					FALSE,		// 優先度を高くする
					FALSE,		// 同一コマンドがキューに無い場合のみ送信する
					FALSE		// GFL_NETライブラリの外で保持するバッファを使用
		);
	}
}

// クライアントID受信関数（サーバ→各クライアント指定）
static void recv_cliendID( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	const TMP_SEND_BUFFER* tsb = pData;
	int i;

	BTL_Printf("[BTLNET] netID=%d, クライアントID受信しました。バッファ内容=%d,%d,%d,%d, 受け持ち数は%d,\n",
			netID, tsb->val8[0], tsb->val8[1], tsb->val8[2], tsb->val8[3], tsb->val8[0]);
	BTL_Printf("    Pos=[ ");

	for(i=0; i<tsb->val8[0]; i++)
	{
		Sys->myClientID[i] = tsb->val8[1+i];
		BTL_Printf("%d ", tsb->val8[1+i]);
	}
	BTL_Printf("]\n");
}

// 自分のクライアントIDが確定したか？
BOOL BTL_NET_IsClientIdDetermined( void )
{
	return Sys->myClientID[0] != BTL_CLIENT_MAX;
}

// 自分の担当するクライアントIDを取得
u8 BTL_NET_GetMyClientID( u8 idx )
{
	GF_ASSERT( idx < BTL_CLIENT_MAX );
	GF_ASSERT( Sys->myClientID[idx] != BTL_CLIENT_MAX );

	return Sys->myClientID[ idx ];
}

// パーティデータを連絡する（全マシン相互）
void BTL_NET_StartNotifyPartyData( const POKEPARTY* party )
{
	u32 size = PokeParty_GetWorkSize();

	BTL_Printf("パーティデータサイズ=%dbytes\n", size);

	GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PARTY_DATA,
				size,
				party,
				FALSE,		// 優先度を高くする
				TRUE,		// 同一コマンドがキューに無い場合のみ送信する
				TRUE		// GFL_NETライブラリの外で保持するバッファを使用
	);
}


static u8* getbuf_partyData( int netID, void* pWork, int size )
{
	GF_ASSERT(Sys->tmpLargeBuf[ netID ] == NULL);

	Sys->tmpLargeBuf[ netID ] = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
	return Sys->tmpLargeBuf[ netID ];
}

static void recv_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	Sys->tmpLargeBufUsedSize[ netID ] = size;
	BTL_Printf("netID=%dのパーティデータ受信完了, pData=%p, buf=%p\n", 
			netID, pData, Sys->tmpLargeBuf[netID] );

}

// パーティデータの相互受信が完了したか？
BOOL BTL_NET_IsCompleteNotifyPartyData( void )
{
	int i, max;

	max = GFL_NET_GetConnectNum();

	for(i=0; i<max; i++)
	{
		if( Sys->tmpLargeBufUsedSize[i] == 0 )
		{
			return FALSE;
		}
	}
	BTL_Printf("パーティデータ相互受信完了  member=%d\n", max);
	return TRUE;
}

const POKEPARTY* BTL_NET_GetPartyData( int netID )
{
	GF_ASSERT(Sys->tmpLargeBuf[netID] != NULL);
	GF_ASSERT(Sys->tmpLargeBufUsedSize[netID] != 0);

	return Sys->tmpLargeBuf[netID];
}


void BTL_NET_EndNotifyPartyData( void )
{
	int i;

	for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
	{
		if( Sys->tmpLargeBuf[i] != NULL )
		{
			GFL_HEAP_FreeMemory( Sys->tmpLargeBuf[i] );
			Sys->tmpLargeBufUsedSize[i] = 0;
		}
	}
}


// 
void BTL_NET_TimingSyncStart( u8 timingID )
{
	GFL_NET_TimingSyncStart( Sys->netHandle, timingID );
}

BOOL BTL_NET_IsTimingSync( u8 timingID )
{
	return GFL_NET_IsTimingSync( Sys->netHandle, timingID );
}





//=============================================================================================
/**
 * 特定クライアントにデータ送信
 *
 * @param   netID		
 * @param   adrs		
 * @param   size		
 *
 */
//=============================================================================================
void BTL_NET_SendToClient( u8 netID, const void* adrs, u32 size )
{
	GFL_NET_SendDataEx( Sys->netHandle, netID, CMD_TO_CLIENT, size, adrs,
				FALSE,		// 優先度を高くする
				FALSE,		// 同一コマンドがキューに無い場合のみ送信する
				TRUE		// GFL_NETライブラリの外で保持するバッファを使用
	);
	BTL_Printf("[BTLNET] マシン(%d) に %d bytes 送信開始\n", netID, size);
}

//=============================================================================================
/**
 * 全クライアントから返信があったかチェック
 *
 * @retval  BOOL		あったらTRUE
 */
//=============================================================================================
BOOL BTL_NET_CheckReturnFromClient( void )
{
	int i;

	for(i=0; i<Sys->memberCount; i++)
	{
		if( recvBuf_isEmpty(&Sys->recvClient[i]) )
		{
			return FALSE;
		}
	}

	BTL_Printf("[BTLNET] 全クライアントからのデータ返ってきたよ\n");

	return TRUE;
}

u32 BTL_NET_GetRecvClientData( u8 netID, const void** pptr )
{
	return recvBuf_getData( &Sys->recvClient[netID], pptr );
}


static u8* getbuf_clientData( int netID, void* pWork, int size )
{
	return recvBuf_getBufAdrs( &Sys->recvClient[netID] );
}

static void recv_clientData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);

	Sys->recvClient[ netID ].size = size;
	if( Sys->recvClient[netID].size == 0 )
	{
		Sys->recvClient[netID].size = 1;
	}
	BTL_Printf("[BTLNET] クライアント(NETID=%d)からの受信完了, サイズ=%d\n", netID, size);
}



void BTL_NET_ClearRecvData( void )
{
	int i;

	for(i=0; i<Sys->memberCount; i++)
	{
		recvBuf_clear( &Sys->recvClient[i] );
	}
}



//---------------------------------

BOOL BTL_NET_IsServerCmdReceived( void )
{
	return Sys->serverCmdReceived;
}


u32 BTL_NET_GetReceivedCmdData( const void** ppDest )
{
	GF_ASSERT( Sys->serverCmdReceived );

	return recvBuf_getData( &Sys->recvServer, ppDest );
}


void BTL_NET_ReturnToServer( const void* data, u32 size )
{
	GFL_NET_SendDataEx( Sys->netHandle, Sys->serverNetID, CMD_TO_SERVER, size, data,
				FALSE,		// 優先度を高くする
				TRUE,		// 同一コマンドがキューに無い場合のみ送信する
				TRUE		// GFL_NETライブラリの外で保持するバッファを使用
	);

	Sys->serverCmdReceived = FALSE;
	BTL_Printf("[BTLNET]  サーバへ返信, フラグオフ\n");

}












static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);
	if( Sys->serverCmdReceived == FALSE )
	{
//		recvBuf_Store( &Sys->recvServer, pData, size );
		Sys->recvServer.size = size;
		Sys->serverCmdReceived = TRUE;
		BTL_Printf("[BTLNET]  サーバコマンド受信, フラグオン\n");
	}
}

static u8* getbuf_serverCmd( int netID, void* pWork, int size )
{
	return recvBuf_getBufAdrs( &Sys->recvServer );
}



static inline void recvBuf_Store( RECV_BUFFER* buf, const void* pData, int size )
{
//	GF_ASSERT_MSG(size<=sizeof(buf->data), "buffer over! size=%d\n", size);
//	GFL_STD_MemCopy( pData, buf->data, size );
	buf->size = size;
}

static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf )
{
	return buf->data;
}

static inline u32 recvBuf_getData( const RECV_BUFFER* buf, const void** ppData )
{
	GF_ASSERT(buf->size);
	*ppData = buf->data;
	return buf->size;
}

static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf )
{
	return buf->size == 0;
}

static inline void recvBuf_clear( RECV_BUFFER* buf )
{
	buf->size = 0;
}


