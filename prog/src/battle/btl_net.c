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

enum {
  PRINT_FLG = FALSE,
};

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  NETID_NULL = 40,  ///< GFL/Net�n��NetID�Ƃ��Ă��蓾�Ȃ��l

  NETID_VT_AI = 3,  ///< �ʐM�^�b�O��AI�ɗ^���鉼�znetID
};

enum {
  CMD_NOTIFY_SERVER_VER = GFL_NET_CMD_BATTLE,
  CMD_NOTIFY_SERVER_PARAM,
  CMD_NOTIFY_PARTY_DATA,
  CMD_NOTIFY_AI_PARTY_DATA,
  CMD_NOTIFY_PLAYER_DATA,
  CMD_NOTIFY_AI_TRAINER_DATA,
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
  u16   size;
  u16   fCleared;

}RECV_BUFFER;


typedef struct {

  GFL_NETHANDLE* netHandle;

  HEAPID  heapID;
  u8      myNetID;
  u8      serverNetID;

  u8    fServerParamReceived;
  u8    memberCount;
  u8    recvCount;
  u8    serverCmdReceived;
  u8    clientDataReturned;
  u8    timingID;
  u8    timingSyncStartFlag;
  u8    clientID[ BTL_NET_CONNECT_MACHINE_MAX ];

  BTLNET_SERVER_NOTIFY_PARAM  serverNotifyParam;

  // �풓��M�o�b�t�@
  RECV_BUFFER   recvServer;
  RECV_BUFFER   recvClient[ BTL_CLIENT_MAX ];

  // �ꎞ��M�o�b�t�@
  void* tmpRecvBuf[ BTL_NET_CONNECT_MACHINE_MAX  ];
  u32   tmpLargeBufUsedSize[ BTL_NET_CONNECT_MACHINE_MAX ];

  void* AIPartyBuffer[ BTL_CLIENT_MAX ];

  void* tmpExBuffer;
  u32   tmpExBufferUsedSize;

  // �풓���M�o�b�t�@
  TMP_SEND_BUFFER sendBuf[ BTL_NET_CONNECT_MACHINE_MAX ];

  SVVER_WORK    svverWork;

}SYSWORK;

static SYSWORK* Sys;



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8 clientIDtoNetID( u8 clientID );
static void recv_serverParam( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_partyData( int netID, void* pWork, int size );
static void recv_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_AI_partyData( int netID, void* pWork, int size );
static void recv_AI_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_playerData( int netID, void* pWork, int size );
static void recv_playerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_AI_trainerData( int netID, void* pWork, int size );
static void recv_AI_trainerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void clearAllTmpBuffer( void );
static u8* getbuf_clientData( int netID, void* pWork, int size );
static void recv_clientData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_serverCmd( int netID, void* pWork, int size );
static inline void recvBuf_clear( RECV_BUFFER* buf );
static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf );
static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf );
static inline void recvBuf_setRecvedSize( RECV_BUFFER* buf, u16 size );
static inline u32 recvBuf_getData( const RECV_BUFFER* buf, const void** ppData );



//static const NetRecvFuncTable RecvFuncTable[] = {
const NetRecvFuncTable BtlRecvFuncTable[] = {
    { recv_serverVer,      NULL },
    { recv_serverParam,    NULL },
    { recv_partyData,      getbuf_partyData     },
    { recv_AI_partyData,   getbuf_AI_partyData  },
    { recv_playerData,     getbuf_playerData },
    { recv_AI_trainerData,  getbuf_AI_trainerData },
    { recv_serverCmd,      getbuf_serverCmd  },
    { recv_clientData,     getbuf_clientData },
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

    Sys->fServerParamReceived = FALSE;

    {
      u32 i;
      for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i)
      {
        Sys->tmpRecvBuf[i] = NULL;
        Sys->tmpLargeBufUsedSize[i] = 0;
        Sys->clientID[i] = BTL_CLIENTID_NULL;
      }
      for(i=0; i<BTL_CLIENT_MAX; ++i)
      {
        Sys->AIPartyBuffer[i] = NULL;
      }
      Sys->tmpExBuffer = NULL;
      Sys->tmpExBufferUsedSize = 0;
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
      BTL_Printf("�l�b�g����V���N��\n", result);
      return TRUE;
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

// �����̃N���C�A���gID�ƃT�[�o�o�[�W������S�}�V���ő��݂ɑ���M
BOOL BTL_NET_DetermineServer( u8 clientID )
{
  BOOL result;
  TMP_SEND_BUFFER* tsb = &Sys->sendBuf[0];
  tsb->val16[0] = BTL_NET_SERVER_VERSION;
  tsb->val16[1] = clientID;
  return GFL_NET_SendData( Sys->netHandle, CMD_NOTIFY_SERVER_VER, sizeof(*tsb), tsb );
}

// �T�[�o�o�[�W������M�֐��i�S�}�V�����݁j
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  const TMP_SEND_BUFFER* tsb = (const TMP_SEND_BUFFER*)pData;
  SVVER_WORK* swk = (SVVER_WORK*)(&Sys->svverWork);

  BTL_N_Printf( DBGSTR_NET_RecvedServerVersion, netID, tsb->val16[0]);

  if( swk->recvTable[netID].recvedFlag == FALSE )
  {
    u16 ver = tsb->val16[0];

    Sys->clientID[ netID ] = tsb->val16[1];
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_DecideClientID, netID, Sys->clientID[netID] );

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
          BTL_N_Printf( DBGSTR_NET_ServerDetermine, i);
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
BOOL BTL_NET_ImServer( void )
{
  return ( Sys->serverNetID == Sys->myNetID );
}

/**
 *  clientID -> netID �ϊ�
 */
static u8 clientIDtoNetID( u8 clientID )
{
  int i, max;
  max = GFL_NET_GetConnectNum();

  for(i=0; i<max; i++)
  {
    if( Sys->clientID[i] == clientID )
    {
      return i;
    }
  }

  return NETID_NULL;
}


// �S�N���C�A���g�ɃT�[�o�p�����[�^��ʒm����i�T�[�o����̂݌Ăяo���j
BOOL BTL_NET_NotifyServerParam( const BTLNET_SERVER_NOTIFY_PARAM* sendParam )
{
  TMP_SEND_BUFFER* tsbuf;
  BOOL result;
  u8 i;

  result = GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_SERVER_PARAM,
        sizeof( *sendParam ),
        sendParam,
        FALSE,    // �D��x����������
        FALSE,    // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        FALSE     // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );

  if( result ){
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_SendSucceed );
  }
  return result;
}

// �T�[�o�p�����[�^��M�֐��i�T�[�o���S�N���C�A���g�j
static void recv_serverParam( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  if( Sys->fServerParamReceived == FALSE )
  {
    const BTLNET_SERVER_NOTIFY_PARAM* recvParam = pData;

    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_RecvedServerParam, netID);

    Sys->serverNotifyParam = *recvParam;
    Sys->fServerParamReceived = TRUE;
  }
}

// �T�[�o�p�����[�^��M�����������H
BOOL BTL_NET_IsServerParamReceived( BTLNET_SERVER_NOTIFY_PARAM* dst )
{
  if( Sys->fServerParamReceived ){
    *dst = Sys->serverNotifyParam;
    return TRUE;
  }
  return FALSE;
}


// �p�[�e�B�f�[�^��A������i�S�}�V�����݁j
BOOL BTL_NET_StartNotifyPartyData( const POKEPARTY* party )
{
  u32 size = PokeParty_GetWorkSize();

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
  GF_ASSERT(Sys->tmpRecvBuf[ netID ] == NULL);

  Sys->tmpRecvBuf[ netID ] = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  return Sys->tmpRecvBuf[ netID ];
}

static void recv_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  Sys->tmpLargeBufUsedSize[ netID ] = size;
  BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_RecvedPartyData,
      netID, Sys->clientID[netID], pData, Sys->tmpRecvBuf[netID] );

  // ���̃u���b�N�͂����̃f�o�b�O�o�͂ł�
  {
    POKEPARTY* party = (POKEPARTY*)( pData );
    POKEMON_PARAM* pp;

    u32 i, monsno, max = PokeParty_GetPokeCount( party );
    for(i=0; i<max; ++i){
      pp = PokeParty_GetMemberPointer( party, i );
      monsno = PP_Get( pp, ID_PARA_monsno, NULL );
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
  BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_PartyDataComplete, max);
  return TRUE;
}


// �ʐM�^�b�O�p��AI�p�[�e�B�f�[�^�𑗐M�i�T�[�o���S�}�V���j
BOOL BTL_NET_StartNotify_AI_PartyData( const BTLNET_AIDATA_CONTAINER* container )
{
  u32 size = sizeof(BTLNET_AIDATA_CONTAINER) + container->dataSize;

  return GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_AI_PARTY_DATA,
        size,
        container,
        FALSE,  // �D��x����������
        TRUE,   // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        TRUE    // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );
}
static u8* getbuf_AI_partyData( int netID, void* pWork, int size )
{
  BTL_N_Printf( DBGSTR_NET_CreateAIPartyRecvBuffer );

  if( Sys->tmpExBuffer == NULL ){
    Sys->tmpExBuffer = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  }
  return Sys->tmpExBuffer;
}
static void recv_AI_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  const BTLNET_AIDATA_CONTAINER* container = pData;

  if( Sys->AIPartyBuffer[ container->clientID ] == NULL ){
    Sys->AIPartyBuffer[ container->clientID ] = GFL_HEAP_AllocClearMemory(GFL_HEAP_LOWID(Sys->heapID), container->dataSize);
  }else{
    GF_ASSERT(0);
  }

  GFL_STD_MemCopy( container->data, Sys->AIPartyBuffer[ container->clientID ], container->dataSize );

  GFL_HEAP_FreeMemory( Sys->tmpExBuffer );
  Sys->tmpExBuffer = NULL;
}
// �ʐM�^�b�O�p��AI�p�[�e�B�f�[�^����M�������`�F�b�N
BOOL BTL_NET_IsRecved_AI_PartyData( u8 clientID )
{
  return Sys->AIPartyBuffer[clientID] != NULL;
}


// ��M�����p�[�e�B�f�[�^�|�C���^���擾�i�� clientID �w��j
const POKEPARTY* BTL_NET_GetPartyData( u8 clientID )
{
  u8 netID = clientIDtoNetID( clientID );

  if( netID != NETID_NULL )
  {
    GF_ASSERT_MSG(Sys->tmpRecvBuf[netID] != NULL, "netID=%d", netID);
    GF_ASSERT_MSG(Sys->tmpLargeBufUsedSize[netID] != 0, "netID=%d", netID);

    return Sys->tmpRecvBuf[ netID ];
  }
  else if( Sys->AIPartyBuffer[clientID] != NULL )
  {
    {
      const POKEPARTY* party = (const POKEPARTY*)(Sys->AIPartyBuffer[clientID]);
      const POKEMON_PARAM* pp;
      u32 cnt, i;

      cnt = PokeParty_GetPokeCount( party );
      BTL_N_Printf( DBGSTR_NET_AIPartyInfo, clientID, cnt );
      for(i=0; i<cnt; ++i)
      {
        pp = PokeParty_GetMemberPointer( party, i );
        BTL_N_PrintfSimple(  DBGSTR_csv, PP_Get(pp, ID_PARA_monsno, NULL) );
      }
      BTL_N_PrintfSimple( DBGSTR_LF );
    }

    return Sys->tmpExBuffer;
  }
  else
  {
    GF_ASSERT_MSG(0, "clientID=%d, partyData not found\n", clientID);
    return NULL;
  }
}
// �p�[�e�B�f�[�^��M��������������
void BTL_NET_EndNotifyPartyData( void )
{
  clearAllTmpBuffer();
}



// �v���C���[�f�[�^�f�[�^��A������i�S�}�V�����݁j
BOOL BTL_NET_StartNotifyPlayerData( const MYSTATUS* playerData )
{
  u32 size = MyStatus_GetWorkSize();

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
  GF_ASSERT(Sys->tmpRecvBuf[ netID ] == NULL);

  Sys->tmpRecvBuf[ netID ] = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  return Sys->tmpRecvBuf[ netID ];
}

static void recv_playerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  Sys->tmpLargeBufUsedSize[ netID ] = size;
  BTL_Printf("netID=%d�̃v���C���[�f�[�^��M����, pData=%p, buf=%p\n",
      netID, pData, Sys->tmpRecvBuf[netID] );
}
// �v���C���[�f�[�^�̑��ݎ�M�������������H
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
// ��M�����g���[�i�[�f�[�^���擾
const MYSTATUS* BTL_NET_GetPlayerData( u8 clientID )
{
  u8 netID = clientIDtoNetID( clientID );

  if( netID != NETID_NULL )
  {
    GF_ASSERT_MSG(Sys->tmpRecvBuf[netID] != NULL, "netID=%d", netID);
    GF_ASSERT_MSG(Sys->tmpLargeBufUsedSize[netID] != 0, "netID=%d", netID);

    return Sys->tmpRecvBuf[ netID ];
  }
  else if( (Sys->tmpExBuffer != NULL) && (Sys->tmpExBufferUsedSize != 0) )
  {
    return Sys->tmpExBuffer;
  }
  else
  {
    return NULL;
  }
}

// AI�p�g���[�i�[�f�[�^��A������i�T�[�o���S�}�V���j
BOOL BTL_NET_StartNotify_AI_TrainerData( const BSP_TRAINER_DATA* tr_data )
{
  BOOL result = GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_AI_TRAINER_DATA,
        sizeof(BSP_TRAINER_DATA),
        tr_data,
        FALSE,     // �D��x����������
        TRUE,     // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
        TRUE      // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
  );
  if( result ){
    BTL_N_Printf( DBGSTR_NET_SendAITrainerData, tr_data->tr_id );
  }
  return result;
}
static u8* getbuf_AI_trainerData( int netID, void* pWork, int size )
{
  Sys->tmpExBuffer = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  return Sys->tmpExBuffer;
}
static void recv_AI_trainerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  BTL_N_Printf( DBGSTR_NET_RecvAITrainerData, ((const BSP_TRAINER_DATA*)pData)->tr_id  );
  Sys->tmpExBufferUsedSize = size;
}
// AI�g���[�i�[�f�[�^��M����������
BOOL BTL_NET_IsRecved_AI_TrainerData( void )
{
  return (Sys->tmpExBufferUsedSize != 0);
}

// ��M����AI�g���[�i�[�f�[�^���擾���擾
const BSP_TRAINER_DATA* BTL_NET_Get_AI_TrainerData( void )
{
  GF_ASSERT(Sys->tmpExBuffer != NULL);
  GF_ASSERT(Sys->tmpExBufferUsedSize != 0);

  return Sys->tmpExBuffer;
}
// �v���C���[�E�g���[�i�[�f�[�^��M��������������
void BTL_NET_EndNotifyPlayerData( void )
{
  clearAllTmpBuffer();
}

/**
 *  �S�Ă̈ꎞ���p�o�b�t�@�����
 */
static void clearAllTmpBuffer( void )
{
  int i;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    if( Sys->tmpRecvBuf[i] != NULL )
    {
      GFL_HEAP_FreeMemory( Sys->tmpRecvBuf[i] );
      Sys->tmpRecvBuf[i] = NULL;
      Sys->tmpLargeBufUsedSize[i] = 0;
    }
  }

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( Sys->AIPartyBuffer[i] != NULL ){
      GFL_HEAP_FreeMemory( Sys->AIPartyBuffer[i] );
      Sys->AIPartyBuffer[i] = NULL;
    }
  }

  if( Sys->tmpExBuffer )
  {
    GFL_HEAP_FreeMemory( Sys->tmpExBuffer );
    Sys->tmpExBuffer = NULL;
    Sys->tmpExBufferUsedSize = 0;
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
BOOL BTL_NET_SendToClient( u8 clientID, const void* adrs, u32 size )
{
  u8 netID = clientIDtoNetID( clientID );

//  GF_ASSERT(netID != NETID_NULL);
  if( netID != NETID_NULL )
  {
    BOOL result = GFL_NET_SendDataEx( Sys->netHandle, netID, CMD_TO_CLIENT, size, adrs,
          FALSE,    // �D��x����������
          FALSE,    // ����R�}���h���L���[�ɖ����ꍇ�̂ݑ��M����
          TRUE      // GFL_NET���C�u�����̊O�ŕێ�����o�b�t�@���g�p
    );
    if( result ){
      BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_SendCmdDone, netID, size);
    }
    return result;
  }
  return TRUE;
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

  BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_RecvedAllClientsData );

  return TRUE;
}

u32 BTL_NET_GetRecvClientData( u8 clientID, const void** pptr )
{
  u8 netID = clientIDtoNetID( clientID );
  GF_ASSERT(netID != NETID_NULL );
  return recvBuf_getData( &Sys->recvClient[netID], pptr );
}


static u8* getbuf_clientData( int netID, void* pWork, int size )
{
  return recvBuf_getBufAdrs( &Sys->recvClient[netID] );
}

static void recv_clientData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);

  recvBuf_setRecvedSize( &Sys->recvClient[netID], size );

  Sys->recvClient[ netID ].size = size;
  Sys->recvClient[ netID ].fCleared = FALSE;

  BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_RecvedClientData, netID, size);
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
  BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_ReturnToServerTrying, size);
  if( result ){
    Sys->serverCmdReceived = FALSE;
    BTL_N_PrintfSimpleEx( PRINT_FLG, DBGSTR_done );
  }
  BTL_N_PrintfSimpleEx( PRINT_FLG, DBGSTR_LF );
  return result;
}

static void recv_serverCmd( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  GF_ASSERT(size<BTL_SERVER_CMD_QUE_SIZE);
  if( Sys->serverCmdReceived == FALSE )
  {
    recvBuf_setRecvedSize( &Sys->recvServer, size );
    Sys->serverCmdReceived = TRUE;
  }
}

static u8* getbuf_serverCmd( int netID, void* pWork, int size )
{
  return recvBuf_getBufAdrs( &Sys->recvServer );
}


/**
 *  ��M�o�b�t�@�F������
 */
static inline void recvBuf_clear( RECV_BUFFER* buf )
{
  buf->size = 0;
  buf->fCleared = TRUE;
}
/**
 *  ��M�o�b�t�@�F��i�������j��Ԃ��`�F�b�N
 */
static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf )
{
  return buf->fCleared;
}
/**
 *  ��M�o�b�t�@�F�f�[�^��M�̂��߂̃o�b�t�@�A�h���X�擾
 */
static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf )
{
  return buf->data;
}
/**
 *  ��M�o�b�t�@�F�f�[�^��M��̃p�����[�^�ݒ�
 */
static inline void recvBuf_setRecvedSize( RECV_BUFFER* buf, u16 size )
{
  buf->size = size;
  buf->fCleared = FALSE;
}
/**
 *  ��M�o�b�t�@�F��M�f�[�^���T�C�Y�擾
 */
static inline u32 recvBuf_getData( const RECV_BUFFER* buf, const void** ppData )
{
  GF_ASSERT(buf->fCleared==FALSE);
  *ppData = buf->data;
  return buf->size;
}



/*--------------------------------------------------------------------------*/
/* AIData Container                                                         */
/*--------------------------------------------------------------------------*/

BTLNET_AIDATA_CONTAINER*  BTL_NET_AIDC_Create( u32 size, HEAPID heapID )
{
  BTLNET_AIDATA_CONTAINER* container = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTLNET_AIDATA_CONTAINER)+size );
  container->dataSize = heapID;
  container->clientID = BTL_CLIENTID_NULL;
  return container;
}
void BTL_NET_AIDC_Delete( BTLNET_AIDATA_CONTAINER* container )
{
  GFL_HEAP_FreeMemory( container );
}

void BTL_NET_AIDC_SetData( BTLNET_AIDATA_CONTAINER* container, const void* src, u8 clientID )
{
  container->clientID = clientID;
  GFL_STD_MemCopy( src, container->data, container->dataSize );
}

