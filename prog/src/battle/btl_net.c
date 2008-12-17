//=============================================================================================
/**
 * @file	btl_net.c
 * @brief	�|�P����WB �o�g��  �ʐM����
 * @author	taya
 *
 * @date	2008.10.10	�쐬
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
	NETID_NULL = 40,	///< GFL/Net�n��NetID�Ƃ��Ă��蓾�Ȃ��l
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
 *	�ꎞ�I���M�o�b�t�@
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
 *	�T�[�o�o�[�W�������ݒʒm�����p���[�N
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
 *	�ʏ��M�p�o�b�t�@
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

	// �풓��M�o�b�t�@
	RECV_BUFFER		recvServer;
	RECV_BUFFER		recvClient[ BTL_CLIENT_MAX ];

	// �ꎞ���p�o�b�t�@
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

		BTL_Printf("[BTLNET] �����̃l�b�gID=%d, �ڑ������o�[��=%d\n", Sys->myNetID, Sys->memberCount);

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

// �T�[�o�o�[�W������M�֐��i�S�}�V�����݁j
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	const TMP_SEND_BUFFER* tsb = (const TMP_SEND_BUFFER*)pData;
	SVVER_WORK* swk = (SVVER_WORK*)(&Sys->svverWork);

	BTL_Printf("�T�[�o�o�[�W������M ... netID=%d, version=%d\n", netID, tsb->val32);

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
		// �S�����o�[�̃T�[�o�o�[�W�������󂯎������N���T�[�o�ɂȂ邩����
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
					BTL_Printf("�T�[�o�� netID=%d �̃}�V���Ɍ���\n", i);
					break;
				}
			}

			GF_ASSERT(Sys->serverNetID != NETID_NULL);
		}
	}
}

// �T�[�o�ƂȂ�}�V�����m�肵�����ǂ����`�F�b�N
BOOL BTL_NET_IsServerDetermained( void )
{
	return Sys->serverNetID != NETID_NULL;
}

// �������T�[�o���ǂ�������
BOOL BTL_NET_IsServer( void )
{
	return ( Sys->serverNetID == Sys->myNetID );
}


// �e�}�V���ɃN���C�A���gID��ʒm����i�T�[�o����̂݌Ăяo���j
void BTL_NET_NotifyClientID( NetID netID, const u8* clientID, u8 numClients )
{
	GF_ASSERT(numClients<=3);

	{
		TMP_SEND_BUFFER* tsbuf;
		u8 i;

		BTL_Printf("[BTLNET] netID=%d �̃}�V���ɃN���C�A���gID��ʒm���܂��B�󂯎�������%d,\n", netID, numClients );
		BTL_Printf("  �N���C�A���gID=[ ");

		tsbuf = &Sys->sendBuf[netID];

		for(i=0; i<numClients; i++)
		{
			tsbuf->val8[i+1] = clientID[i];
			BTL_Printf("%d ", clientID[i]);
		}
		BTL_Printf("]\n");
		tsbuf->val8[0] = numClients;

		BTL_Printf("  ���M�T�C�Y=%d, ���e=%d,%d,%d,%d\n",
			sizeof(*tsbuf), tsbuf->val8[0], tsbuf->val8[1], tsbuf->val8[2], tsbuf->val8[3] );

/*
	BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand,
			const u32 size, const void* data,
			const BOOL bFast, const BOOL bRepeat, const BOOL bSendBuffLock )
*/
		GFL_NET_SendDataEx( Sys->netHandle, netID, CMD_NOTIFY_CLIENT_ID,
					sizeof( *tsbuf ),
					tsbuf,
					FALSE,		// �D��x����������
					FALSE,		// ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
					FALSE		// GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
		);
	}
}

// �N���C�A���gID��M�֐��i�T�[�o���e�N���C�A���g�w��j
static void recv_cliendID( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	const TMP_SEND_BUFFER* tsb = pData;
	int i;

	BTL_Printf("[BTLNET] netID=%d, �N���C�A���gID��M���܂����B�o�b�t�@���e=%d,%d,%d,%d, �󂯎�������%d,\n",
			netID, tsb->val8[0], tsb->val8[1], tsb->val8[2], tsb->val8[3], tsb->val8[0]);
	BTL_Printf("    ID=[ ");

	for(i=0; i<tsb->val8[0]; i++)
	{
		Sys->myClientID[i] = tsb->val8[1+i];
		BTL_Printf("%d ", tsb->val8[1+i]);
	}
	BTL_Printf("]\n");
}

// �����̃N���C�A���gID���m�肵�����H
BOOL BTL_NET_IsClientIdDetermined( void )
{
	return Sys->myClientID[0] != BTL_CLIENT_MAX;
}

// �����̒S������N���C�A���gID���擾
u8 BTL_NET_GetMyClientID( u8 idx )
{
	GF_ASSERT( idx < BTL_CLIENT_MAX );
	GF_ASSERT( Sys->myClientID[idx] != BTL_CLIENT_MAX );

	return Sys->myClientID[ idx ];
}

// �p�[�e�B�f�[�^��A������i�S�}�V�����݁j
void BTL_NET_StartNotifyPartyData( const POKEPARTY* party )
{
	u32 size = PokeParty_GetWorkSize();

	BTL_Printf("�p�[�e�B�f�[�^�T�C�Y=%dbytes\n", size);

	GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PARTY_DATA,
				size,
				party,
				FALSE,		// �D��x����������
				TRUE,		// ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
				TRUE		// GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
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
	BTL_Printf("netID=%d�̃p�[�e�B�f�[�^��M����, pData=%p, buf=%p\n", 
			netID, pData, Sys->tmpLargeBuf[netID] );

}

// �p�[�e�B�f�[�^�̑��ݎ�M�������������H
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
	BTL_Printf("�p�[�e�B�f�[�^���ݎ�M����  member=%d\n", max);
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





void BTL_NET_SendToClient( const void* adrs, u32 size )
{
	BTL_NET_ClearRecvData();

	GFL_NET_SendDataEx( Sys->netHandle, Sys->serverNetID, GFL_NET_CMD_BATTLE, size, adrs,
				FALSE,		// �D��x����������
				TRUE,		// ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
				TRUE		// GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
	);
}

BOOL BTL_NET_CheckSendToClient( void )
{
	return GFL_NET_IsEmptySendData( Sys->netHandle );
}

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

	return TRUE;
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
	GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_TO_SERVER, size, data,
				FALSE,		// �D��x����������
				TRUE,		// ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
				TRUE		// GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
	);
}

void BTL_NET_StartCommand( BtlNetCommand cmd )
{
	// @@@�͂�ڂ�
}


BOOL BTL_NET_WaitCommand( void )
{
	// @@@�͂�ڂ�
	return TRUE;
}








static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);
	if( Sys->serverCmdReceived == FALSE )
	{
		recvBuf_Store( &Sys->recvServer, pData, size );
		Sys->serverCmdReceived = TRUE;
	}
}

static u8* getbuf_serverCmd( int netID, void* pWork, int size )
{
	return recvBuf_getBufAdrs( &Sys->recvServer );
}

static void recv_clientData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
	GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);
}
static u8* getbuf_clientData( int netID, void* pWork, int size )
{
	return recvBuf_getBufAdrs( &Sys->recvClient[netID] );
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


