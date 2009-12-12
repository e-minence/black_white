//=============================================================================================
/**
 * @file  btl_net.c
 * @brief �|�P����WB �o�g��  �ʐM����
 * @author  taya
 *
 * @date  2008.10.10  �쐬
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
  NETID_NULL = 40,  ///< GFL/Net�n��NetID�Ƃ��Ă��蓾�Ȃ��l
};

enum {
  CMD_NOTIFY_SERVER_VER = GFL_NET_CMD_BATTLE,
  CMD_NOTIFY_DEBUG_PARAM,
  CMD_NOTIFY_PARTY_DATA,
  CMD_NOTIFY_PLAYER_DATA,
  CMD_TO_CLIENT,
  CMD_TO_SERVER,
};


/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

//---------------------------------------------------
/**
 *  �ꎞ�I���M�o�b�t�@
 */
//---------------------------------------------------
typedef struct {

  union {
    u32   val32;
    u16   val16[2];
    u8    val8[4];
  };

}TMP_SEND_BUFFER;

//---------------------------------------------------
/**
 *  �T�[�o�o�[�W�������ݒʒm�����p���[�N
 */
//---------------------------------------------------
typedef struct {

  struct {
    u8  version;
    u8  recvedFlag;
  }recvTable[ BTL_CLIENT_MAX ];

  u8  recvedCount;
  u8  maxVersion;

}SVVER_WORK;

//---------------------------------------------------
/**
 *  �ʏ��M�p�o�b�t�@
 */
//---------------------------------------------------
typedef struct {

  u8    data[ BTL_SERVER_CMD_QUE_SIZE ];
  u32   size;

}RECV_BUFFER;


typedef struct {

  GFL_NETHANDLE* netHandle;

  HEAPID  heapID;
  u8    myNetID;
  u8    serverNetID;

  u16   debugFlagBit;
  u8    fDebugParamReceived;

  u8    memberCount;
  u8    recvCount;
  u8    serverCmdReceived;
  u8    clientDataReturned;
  u8    timingID;
  u8    timingSyncStartFlag;

  // �풓��M�o�b�t�@
  RECV_BUFFER   recvServer;
  RECV_BUFFER   recvClient[ BTL_CLIENT_MAX ];

  // �ꎞ���p�o�b�t�@
  void* tmpLargeBuf[ BTL_NET_CONNECT_MACHINE_MAX ];
  u32   tmpLargeBufUsedSize[ BTL_NET_CONNECT_MACHINE_MAX ];

  TMP_SEND_BUFFER sendBuf[ BTL_NET_CONNECT_MACHINE_MAX ];

  SVVER_WORK    svverWork;

}SYSWORK;

static SYSWORK* Sys;



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void recv_debugParam( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_partyData( int netID, void* pWork, int size );
static void recv_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_playerData( int netID, void* pWork, int size );
static void recv_playerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_clientData( int netID, void* pWork, int size );
static void recv_clientData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_serverCmd( int netID, void* pWork, int size );
static inline void recvBuf_Store( RECV_BUFFER* buf, const void* pData, int size );
static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf );
static inline u32 recvBuf_getData( const RECV_BUFFER* buf, const void** ppData );
static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf );
static inline void recvBuf_clear( RECV_BUFFER* buf );



//static const NetRecvFuncTable RecvFuncTable[] = {
const NetRecvFuncTable BtlRecvFuncTable[] = {
    { recv_serverVer, NULL },
    { recv_debugParam,  NULL },
    { recv_partyData,   getbuf_partyData },
    { recv_playerData,  getbuf_playerData },
    { recv_serverCmd,   getbuf_serverCmd },
    { recv_clientData,  getbuf_clientData },
};


void BTL_NET_InitSystem( GFL_NETHANDLE* netHandle, HEAPID heapID )
{
  if( netHandle )
  {
    Sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(SYSWORK) );

    Sys->netHandle = netHandle;
    Sys->heapID = heapID;
    Sys->myNetID = GFL_NET_GetNetID( netHandle );
    Sys->serverNetID = NETID_NULL;

    Sys->serverCmdReceived = FALSE;
    Sys->clientDataReturned = FALSE;
    Sys->memberCount = GFL_NET_GetConnectNum();
    Sys->timingID = BTL_NET_TIMING_NULL;
    Sys->timingSyncStartFlag = FALSE;

    Sys->debugFlagBit = 0;
    Sys->fDebugParamReceived = FALSE;

    {
      u32 i;
      for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i){
        Sys->tmpLargeBuf[i] = NULL;
        Sys->tmpLargeBufUsedSize[i] = 0;
      }
    }

    BTL_Printf("�����̃l�b�gID=%d, �ڑ������o�[��=%d\n", Sys->myNetID, Sys->memberCount);
    BTL_NET_TimingSyncStart( BTL_NET_TIMING_INITIALIZE );
  }
  else
  {
    Sys = NULL;
  }
}
BOOL BTL_NET_IsInitialized( void )
{
  if( Sys )
  {
    if( BTL_NET_IsTimingSync(BTL_NET_TIMING_INITIALIZE) )
    {
      BOOL result;
      TMP_SEND_BUFFER* tsb = &Sys->sendBuf[0];
      tsb->val32 = BTL_NET_SERVER_VERSION;
      result = GFL_NET_SendData( Sys->netHandle, CMD_NOTIFY_SERVER_VER, sizeof(*tsb), tsb );
      BTL_Printf("�V���N�����܂��� ... result=%d\n", result);
      return result;
    }
    return FALSE;
  }
  else{
    return TRUE;
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


// �S�}�V���Ƀf�o�b�O�p�����[�^��ʒm����i�T�[�o����̂݌Ăяo���j
BOOL BTL_NET_NotifyDebugParam( u16 debugFlagBit )
{
  TMP_SEND_BUFFER* tsbuf;
  BOOL result;
  u8 i;

  tsbuf = &Sys->sendBuf[0];
  tsbuf->val16[0] = debugFlagBit;

  BTL_Printf("�S�}�V���Ƀf�o�b�OBitFlag�𑗐M : flag=%04x\n", tsbuf->val16[0]);

  result = GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_DEBUG_PARAM,
        sizeof( *tsbuf ),
        tsbuf,
        FALSE,    // �D��x����������
        FALSE,    // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        FALSE     // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );

  if( result ){
    BTL_Printf("  ���M����\n");
  }
  return result;
}

// �f�o�b�O�p�����[�^��M�֐��i�T�[�o���S�N���C�A���g�j
static void recv_debugParam( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  if( Sys->fDebugParamReceived == FALSE )
  {
    const TMP_SEND_BUFFER* tsb = pData;

    BTL_Printf("netID=%d, �f�o�b�O�p�����[�^��M���܂����B�o�b�t�@���e=%d,%d,\n",
        netID, tsb->val16[0], tsb->val16[1]);

    Sys->debugFlagBit = tsb->val16[0];
    Sys->fDebugParamReceived = TRUE;
  }
}

// �f�o�b�O�p�����[�^��M�����������H
BOOL BTL_NET_IsDebugParamReceived( u16* debugFlagBit )
{
  if( Sys->fDebugParamReceived ){
    *debugFlagBit = Sys->debugFlagBit;
    return TRUE;
  }
  return FALSE;
}


// �p�[�e�B�f�[�^��A������i�S�}�V�����݁j
BOOL BTL_NET_StartNotifyPartyData( const POKEPARTY* party )
{
  u32 size = PokeParty_GetWorkSize();

  BTL_Printf("�p�[�e�B�f�[�^�T�C�Y=%dbytes\n", size);

  return GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PARTY_DATA,
        size,
        party,
        FALSE,  // �D��x����������
        TRUE,   // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        TRUE    // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
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
  {
    POKEPARTY* party = (POKEPARTY*)(pData);
    POKEMON_PARAM* pp;

    u32 i, monsno, max = PokeParty_GetPokeCount( party );
    BTL_Printf("  members = %d\n", max);
    for(i=0; i<max; ++i){
      pp = PokeParty_GetMemberPointer( party, i );
      monsno = PP_Get( pp, ID_PARA_monsno, NULL );
      BTL_Printf("  monsno[%d] = %d\n", i, monsno);
    }
  }

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
  GF_ASSERT_MSG(Sys->tmpLargeBuf[netID] != NULL, "netID=%d", netID);
  GF_ASSERT_MSG(Sys->tmpLargeBufUsedSize[netID] != 0, "netID=%d", netID);

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
      Sys->tmpLargeBuf[i] = NULL;
      Sys->tmpLargeBufUsedSize[i] = 0;
    }
  }
}

// �v���C���[�f�[�^�f�[�^��A������i�S�}�V�����݁j
BOOL BTL_NET_StartNotifyPlayerData( const MYSTATUS* playerData )
{
  u32 size = MyStatus_GetWorkSize();

  BTL_Printf("�v���C���[�f�[�^�T�C�Y=%dbytes\n", size);

  return GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PLAYER_DATA,
        size,
        playerData,
        FALSE,     // �D��x����������
        TRUE,     // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        TRUE      // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );
}


static u8* getbuf_playerData( int netID, void* pWork, int size )
{
  GF_ASSERT(Sys->tmpLargeBuf[ netID ] == NULL);

  Sys->tmpLargeBuf[ netID ] = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  return Sys->tmpLargeBuf[ netID ];
}

static void recv_playerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  Sys->tmpLargeBufUsedSize[ netID ] = size;
  BTL_Printf("netID=%d�̃v���C���[�f�[�^��M����, pData=%p, buf=%p\n",
      netID, pData, Sys->tmpLargeBuf[netID] );

}
// �p�[�e�B�f�[�^�̑��ݎ�M�������������H
BOOL BTL_NET_IsCompleteNotifyPlayerData( void )
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
  BTL_Printf("�v���C���[�f�[�^���ݎ�M����  �����o�[��=%d\n", max);
  return TRUE;
}

const MYSTATUS* BTL_NET_GetPlayerData( int netID )
{
  GF_ASSERT_MSG(Sys->tmpLargeBuf[netID] != NULL, "netID=%d", netID);
  GF_ASSERT_MSG(Sys->tmpLargeBufUsedSize[netID] != 0, "netID=%d", netID);

  return Sys->tmpLargeBuf[netID];
}

void BTL_NET_EndNotifyPlayerData( void )
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
  if( Sys )
  {
    GFL_NET_TimingSyncStart( Sys->netHandle, timingID );
    Sys->timingID = timingID;
    Sys->timingSyncStartFlag = TRUE;

    // @@@ �{���͂�������
//  Sys->timingSyncStartFlag = GFL_NET_TimingSyncStart( Sys->netHandle, timingID );
  }
}

BOOL BTL_NET_IsTimingSync( u8 timingID )
{
  if( Sys )
  {
    if( Sys->timingSyncStartFlag ){
      BOOL is_sync = GFL_NET_IsTimingSync( Sys->netHandle, Sys->timingID );
      if( is_sync ){
        Sys->timingID = BTL_NET_TIMING_NULL;
        Sys->timingSyncStartFlag = FALSE;
        return is_sync;
      }
    }else{
      GFL_NET_TimingSyncStart( Sys->netHandle, Sys->timingID );
      Sys->timingSyncStartFlag = TRUE;
      // @@@ �{���͂�������
  //    Sys->timingSyncStartFlag = GFL_NET_TimingSyncStart( Sys->netHandle, Sys->timingID );
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}





//=============================================================================================
/**
 * ����N���C�A���g�Ƀf�[�^���M
 *
 * @param   netID
 * @param   adrs
 * @param   size
 *
 */
//=============================================================================================
BOOL BTL_NET_SendToClient( u8 netID, const void* adrs, u32 size )
{
  BOOL result = GFL_NET_SendDataEx( Sys->netHandle, netID, CMD_TO_CLIENT, size, adrs,
        FALSE,    // �D��x����������
        FALSE,    // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        TRUE      // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );
  if( result ){
    BTL_Printf("�}�V��(%d) �� %d bytes ���M�J�n\n", netID, size);
  }
  return result;
}

//=============================================================================================
/**
 * �S�N���C�A���g����ԐM�����������`�F�b�N
 *
 * @retval  BOOL    ��������TRUE
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

  BTL_Printf("�S�N���C�A���g����̃f�[�^�Ԃ��Ă�����\n");

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
  BTL_Printf("�N���C�A���g(NETID=%d)����̎�M����, �T�C�Y=%d\n", netID, size);
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


BOOL BTL_NET_ReturnToServer( const void* data, u32 size )
{
  BOOL result;

  result = GFL_NET_SendDataEx( Sys->netHandle, Sys->serverNetID, CMD_TO_SERVER, size, data,
        FALSE,    // �D��x����������
        TRUE,   // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        TRUE    // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );
  if( result ){
    Sys->serverCmdReceived = FALSE;
    BTL_Printf("�T�[�o�֕ԐM, �t���O�I�t\n");
  }
  return result;
}












static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);
  if( Sys->serverCmdReceived == FALSE )
  {
//    recvBuf_Store( &Sys->recvServer, pData, size );
    Sys->recvServer.size = size;
    Sys->serverCmdReceived = TRUE;
    BTL_Printf("�T�[�o�R�}���h��M, �t���O�I��\n");
  }
}

static u8* getbuf_serverCmd( int netID, void* pWork, int size )
{
  return recvBuf_getBufAdrs( &Sys->recvServer );
}



static inline void recvBuf_Store( RECV_BUFFER* buf, const void* pData, int size )
{
//  GF_ASSERT_MSG(size<=sizeof(buf->data), "buffer over! size=%d\n", size);
//  GFL_STD_MemCopy( pData, buf->data, size );
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


