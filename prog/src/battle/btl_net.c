//=============================================================================================
/**
 * @file  btl_net.c
 * @brief ポケモンWB バトル  通信処理
 * @author  taya
 *
 * @date  2008.10.10  作成
 */
//=============================================================================================
#include <net.h>

#include "net\network_define.h"
#include "system\net_err.h"
#include "print\str_tool.h"

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
  NETID_NULL = 40,  ///< GFL/Net系のNetIDとしてあり得ない値

  NETID_VT_AI = 3,  ///< 通信タッグでAIに与える仮想netID

  PERAPP_DISABLE_DATA_SIZE = 4,   ///< ペラップボイスが非録音状態であることを通知するためのデータサイズ
};

enum {
  CMD_NOTIFY_SERVER_VER = GFL_NET_CMD_BATTLE,
  CMD_NOTIFY_SERVER_PARAM,
  CMD_NOTIFY_PARTY_DATA,
  CMD_NOTIFY_PERAPP_VOICE,
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
 *  一時的送信バッファ
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
 *  サーババージョン相互通知処理用ワーク
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
 *  通常受信用バッファ
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

  u16   netIDBit;
  u8    fServerParamReceived;
  u8    memberCount;
  u8    recvCount;
  u8    serverCmdReceived;
  u8    clientDataReturned;
  u8    timingID;
  u8    timingSyncStartFlag;
  u8    clientID[ BTL_NET_CONNECT_MACHINE_MAX ];

  BTLNET_SERVER_NOTIFY_PARAM  serverNotifyParam;

  // 常駐受信バッファ
  RECV_BUFFER   recvServer;
  RECV_BUFFER   recvClient[ BTL_CLIENT_MAX ];

  // 一時受信バッファ
  void* tmpRecvBuf[ BTL_NET_CONNECT_MACHINE_MAX  ];
  u32   tmpLargeBufUsedSize[ BTL_NET_CONNECT_MACHINE_MAX ];

  void* AIPartyBuffer[ BTL_CLIENT_MAX ];

  void* tmpExBuffer;
  u32   tmpExBufferUsedSize;

  // 常駐送信バッファ
  TMP_SEND_BUFFER sendBuf;

  SVVER_WORK    svverWork;

}SYSWORK;

static SYSWORK* Sys;



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static inline u32 netIDBitToConnectNum( u16 netIDBit );
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static BOOL IsBattleConnectNetID( int netID );
static u8 clientIDtoNetID( u8 clientID );
static void recv_serverParam( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_partyData( int netID, void* pWork, int size );
static void recv_partyData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static u8* getbuf_perappVoice( int netID, void* pWork, int size );
static void recv_perappVoice( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
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
static void recvBuf_ClearAll( void );
static inline void recvBuf_clear( RECV_BUFFER* buf );
static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf );
static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf );
static inline void recvBuf_setRecvedSize( RECV_BUFFER* buf, u16 size );
static inline u32 recvBuf_getData( const RECV_BUFFER* buf, const void** ppData );

static BTL_TRAINER_SEND_DATA* create_SendTrainerData( const BSP_TRAINER_DATA* tr_data, HEAPID heapID );
static void delete_SendTrainerData( BTL_TRAINER_SEND_DATA* send_tr_data );


//static const NetRecvFuncTable RecvFuncTable[] = {
const NetRecvFuncTable BtlRecvFuncTable[] = {
    { recv_serverVer,       NULL                  },
    { recv_serverParam,     NULL                  },
    { recv_partyData,       getbuf_partyData      },
    { recv_perappVoice,     getbuf_perappVoice    },
    { recv_AI_partyData,    getbuf_AI_partyData   },
    { recv_playerData,      getbuf_playerData     },
    { recv_AI_trainerData,  getbuf_AI_trainerData },
    { recv_serverCmd,       getbuf_serverCmd      },
    { recv_clientData,      getbuf_clientData     },
};


static inline u32 netIDBitToConnectNum( u16 netIDBit )
{
  u32 i, max, cnt;

  for(i=0, cnt=0; i<BTL_NET_CONNECT_MACHINE_MAX; ++i){
    if( netIDBit & (1<<i) ){
      ++cnt;
    }
  }

  max = GFL_NET_GetConnectNum();
  if( cnt > max ){
    cnt = max;
  }

  return cnt;
}

void BTL_NET_InitSystem( GFL_NETHANDLE* netHandle, u16 netIDBit, HEAPID heapID )
{
  if( netHandle )
  {
    Sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(SYSWORK) );

    Sys->netHandle = netHandle;
    Sys->netIDBit = netIDBit;
    Sys->heapID = heapID;
    Sys->myNetID = GFL_NET_GetNetID( netHandle );
    Sys->serverNetID = NETID_NULL;

    Sys->serverCmdReceived = FALSE;
    Sys->clientDataReturned = FALSE;
    Sys->memberCount = netIDBitToConnectNum( netIDBit );
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

    BTL_N_Printf( DBGSTR_NET_Startup, Sys->myNetID, Sys->memberCount);
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
    clearAllTmpBuffer();
    GFL_HEAP_FreeMemory( Sys );
  }
}
/**
 *  通信エラーが発生したかチェック
 */
BOOL BTL_NET_CheckError( void )
{
  if( Sys ){
    return NetErr_App_CheckError() != NET_ERR_CHECK_NONE;
  }
  return FALSE;
}

// 自分のクライアントIDとサーババージョンを全マシンで相互に送受信
BOOL BTL_NET_DetermineServer( u8 clientID )
{
  BOOL result;
  TMP_SEND_BUFFER* tsb = &Sys->sendBuf;
  tsb->val16[0] = BTL_NET_SERVER_VERSION;
  tsb->val16[1] = clientID;

  return GFL_NET_SendData( Sys->netHandle, CMD_NOTIFY_SERVER_VER, sizeof(*tsb), tsb );
}

// サーババージョン受信関数（全マシン相互）
static void recv_serverVer( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  const TMP_SEND_BUFFER* tsb = (const TMP_SEND_BUFFER*)pData;
  SVVER_WORK* swk = (SVVER_WORK*)(&Sys->svverWork);

  BTL_N_Printf( DBGSTR_NET_RecvedServerVersion, netID, tsb->val16[0], tsb->val16[1]);

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
    // 全メンバーのサーババージョンを受け取ったら誰がサーバになるか決定
    if( swk->recvedCount >= Sys->memberCount )
    {
      int i;
      u8 numMaxVerMembers = 0;

      Sys->serverNetID = NETID_NULL;

      for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
      {
        if( (swk->recvTable[i].recvedFlag) && (swk->recvTable[i].version == swk->maxVersion) )
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
/**
 *  バトルに接続している netID か判定
 */
static BOOL IsBattleConnectNetID( int netID )
{
  SVVER_WORK* swk = (SVVER_WORK*)(&Sys->svverWork);
  return swk->recvTable[netID].recvedFlag;
}

// サーバとなるマシンが確定したかどうかチェック
BOOL BTL_NET_IsServerDetermained( void )
{
  return Sys->serverNetID != NETID_NULL;
}

// 自分がサーバかどうか判定
BOOL BTL_NET_ImServer( void )
{
  return ( Sys->serverNetID == Sys->myNetID );
}

// サーバになるマシンのサーババージョンを取得
u8 BTL_NET_GetServerVersion( void )
{
  if( Sys != NULL ){
    SVVER_WORK* swk = (SVVER_WORK*)(&Sys->svverWork);
    return swk->maxVersion;
  }
  return BTL_NET_SERVER_VERSION;
}


/**
 *  clientID -> netID 変換
 */
static u8 clientIDtoNetID( u8 clientID )
{
  int i, max;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    if( Sys->clientID[i] == clientID )
    {
      return i;
    }
  }

  return NETID_NULL;
}


// 全クライアントにサーバパラメータを通知する（サーバからのみ呼び出し）
BOOL BTL_NET_NotifyServerParam( const BTLNET_SERVER_NOTIFY_PARAM* sendParam )
{
  TMP_SEND_BUFFER* tsbuf;
  BOOL result;
  u8 i;

  result = GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_SERVER_PARAM,
        sizeof( *sendParam ),
        sendParam,
        FALSE,    // 優先度を高くする
        FALSE,    // 同一コマンドがキューに無い場合のみ送信する
        FALSE     // GFL_NETライブラリの外で保持するバッファを使用
  );

  if( result ){
    BTL_N_PrintfEx( PRINT_FLG, DBGSTR_NET_SendSucceed );
  }
  return result;
}

// サーバパラメータ受信関数（サーバ→全クライアント）
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

// サーバパラメータ受信完了したか？
BOOL BTL_NET_IsServerParamReceived( BTLNET_SERVER_NOTIFY_PARAM* dst )
{
  if( Sys->fServerParamReceived ){
    *dst = Sys->serverNotifyParam;
    return TRUE;
  }
  return FALSE;
}
/*----------------------------------------------------------------------------------------------------*/
/* パーティデータ                                                                                     */
/*----------------------------------------------------------------------------------------------------*/
// パーティデータを連絡する（全マシン相互）
BOOL BTL_NET_StartNotifyPartyData( const POKEPARTY* party )
{
  u32 size = PokeParty_GetWorkSize();

  u8 result =  GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PARTY_DATA,
        size,
        party,
        FALSE,  // 優先度を高くする
        TRUE,   // 同一コマンドがキューに無い場合のみ送信する
        TRUE    // GFL_NETライブラリの外で保持するバッファを使用
  );

  return result;
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
  BTL_N_Printf( DBGSTR_NET_RecvedPartyData,
      netID, Sys->clientID[netID], pData, Sys->tmpRecvBuf[netID] );

}
// パーティデータの相互受信が完了したか？
BOOL BTL_NET_IsCompleteNotifyPartyData( void )
{
  int i;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    if( IsBattleConnectNetID(i) )
    {
      if( Sys->tmpLargeBufUsedSize[i] == 0 )
      {
        return FALSE;
      }
    }
  }
  BTL_N_Printf( DBGSTR_NET_PartyDataComplete, Sys->memberCount );
  return TRUE;
}

/*----------------------------------------------------------------------------------------------------*/
/* ペラップボイス                                                                                     */
/*----------------------------------------------------------------------------------------------------*/

/**
 *  ペラップボイス送受信用意
 */
void BTL_NET_SetupPerappVoice( void )
{
  recvBuf_ClearAll();
  clearAllTmpBuffer();
}
/**
 * ペラップボイス送信開始（全マシン相互）
 */
BOOL BTL_NET_StartNotifyPerappVoice( const PERAPVOICE* pvoice )
{
  u32 size;
  const void* data;

  if( PERAPVOICE_GetExistFlag(pvoice) ){
    size = PERAPVOICE_GetWorkSize();
    data = (const void*)pvoice;
  }else{
    size = PERAPP_DISABLE_DATA_SIZE;
    data = &(Sys->sendBuf);
    Sys->sendBuf.val32 = 0;
  }

  return GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PERAPP_VOICE,
        size,
        data,
        FALSE,  // 優先度を高くする
        TRUE,   // 同一コマンドがキューに無い場合のみ送信する
        TRUE    // GFL_NETライブラリの外で保持するバッファを使用
  );
}


static u8* getbuf_perappVoice( int netID, void* pWork, int size )
{
  GF_ASSERT(Sys->tmpRecvBuf[ netID ] == NULL);

  if( size == PERAPP_DISABLE_DATA_SIZE )
  {
    BTL_N_Printf( DBGSTR_NET_PrappVoiceGetBufDisable, netID );
    return recvBuf_getBufAdrs( &Sys->recvClient[ netID ] );
  }
  else
  {
    BTL_N_Printf( DBGSTR_NET_PrappVoiceGetBufEnable, netID );
    Sys->tmpRecvBuf[ netID ] = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
    return Sys->tmpRecvBuf[ netID ];
  }
}

static void recv_perappVoice( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  if( size == PERAPP_DISABLE_DATA_SIZE )
  {
    recvBuf_setRecvedSize( &Sys->recvClient[netID], size );
    BTL_N_Printf( DBGSTR_NET_PerappVoiceRecvedDisable,  netID, recvBuf_isEmpty(&Sys->recvClient[netID]) );
  }
  else
  {
    Sys->tmpLargeBufUsedSize[ netID ] = size;
    BTL_N_Printf( DBGSTR_NET_PerappVoiceRecvedEnable, netID, size );
  }
}
/**
 *  ペラップボイスの相互受信が完了したか？
 */
BOOL BTL_NET_IsCompletePerappVoice( void )
{
  int i;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    if( IsBattleConnectNetID(i) )
    {
      if( recvBuf_isEmpty(&Sys->recvClient[i]) )
      {
        if( Sys->tmpLargeBufUsedSize[i] == 0 )
        {
          return FALSE;
        }
      }
    }
  }
  BTL_N_Printf( DBGSTR_NET_PerappVoiceComplete, Sys->memberCount );
  return TRUE;
}
/**
 *  受信したペラップボイスを取得（空データだった場合、NULL）
 */
const void* BTL_NET_GetPerappVoiceRaw( u8 clientID )
{
  u8 netID = clientIDtoNetID( clientID );

  BTL_N_Printf( DBGSTR_NET_PerappVoiceCheckRaw, clientID, netID );

  if( netID != NETID_NULL )
  {
    if( Sys->tmpLargeBufUsedSize[netID] != 0 ){
      return Sys->tmpRecvBuf[netID];
    }
  }
  return NULL;
}
/**
 *  ペラップボイス相互受信シーケンス終了
 */
void BTL_NET_QuitNotifyPerappVoice( void )
{
  recvBuf_ClearAll();
  clearAllTmpBuffer();
}


/*----------------------------------------------------------------------------------------------------*/
/* 通信タッグ用AIのパーティデータ                                                                     */
/*----------------------------------------------------------------------------------------------------*/

// 通信タッグ用のAIパーティデータを送信（サーバ→全マシン）
BOOL BTL_NET_StartNotify_AI_PartyData( const BTLNET_AIDATA_CONTAINER* container )
{
  u32 size = sizeof(BTLNET_AIDATA_CONTAINER) + container->dataSize;

  return GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_AI_PARTY_DATA,
        size,
        container,
        FALSE,  // 優先度を高くする
        TRUE,   // 同一コマンドがキューに無い場合のみ送信する
        TRUE    // GFL_NETライブラリの外で保持するバッファを使用
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
  BTL_N_Printf( DBGSTR_NET_RecvedAIPartyData, container->clientID );

  GFL_HEAP_FreeMemory( Sys->tmpExBuffer );
  Sys->tmpExBuffer = NULL;
}
// 通信タッグ用のAIパーティデータを受信したかチェック
BOOL BTL_NET_IsRecved_AI_PartyData( u8 clientID )
{
  return Sys->AIPartyBuffer[clientID] != NULL;
}


// 受信したパーティデータポインタを取得（※ clientID 指定）
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
    const POKEPARTY* party = (const POKEPARTY*)(Sys->AIPartyBuffer[clientID]);

    // デバッグ出力
    {
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

    return party;
  }
  else
  {
    GF_ASSERT_MSG(0, "clientID=%d, partyData not found\n", clientID);
    return NULL;
  }
}
// パーティデータ受信処理を完了する
void BTL_NET_EndNotifyPartyData( void )
{
  clearAllTmpBuffer();
}



// プレイヤーデータデータを連絡する（全マシン相互）
BOOL BTL_NET_StartNotifyPlayerData( const MYSTATUS* playerData )
{
  u32 size = MyStatus_GetWorkSize();

  return GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_PLAYER_DATA,
        size,
        playerData,
        FALSE,     // 優先度を高くする
        TRUE,     // 同一コマンドがキューに無い場合のみ送信する
        TRUE      // GFL_NETライブラリの外で保持するバッファを使用
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
  BTL_Printf("netID=%dのプレイヤーデータ受信完了, pData=%p, buf=%p\n",
      netID, pData, Sys->tmpRecvBuf[netID] );
}
// プレイヤーデータの相互受信が完了したか？
BOOL BTL_NET_IsCompleteNotifyPlayerData( void )
{
  int i;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    if( IsBattleConnectNetID(i) && (Sys->tmpLargeBufUsedSize[i] == 0) )
    {
      return FALSE;
    }
  }
  BTL_Printf("プレイヤーデータ相互受信完了  メンバー数=%d\n", max);
  return TRUE;
}
// 受信したトレーナーデータを取得
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

// AI用トレーナーデータを連絡する（サーバ→全マシン）
BOOL BTL_NET_StartNotify_AI_TrainerData( const BSP_TRAINER_DATA* tr_data )
{
  BOOL result;
  BTL_TRAINER_SEND_DATA* send_buf;

  // 送信用情報を生成
  send_buf = create_SendTrainerData( tr_data, GFL_HEAP_LOWID(Sys->heapID) );


  result = GFL_NET_SendDataEx( Sys->netHandle, GFL_NET_SENDID_ALLUSER, CMD_NOTIFY_AI_TRAINER_DATA,
        sizeof(BTL_TRAINER_SEND_DATA),
        send_buf,
        FALSE,     // 優先度を高くする
        TRUE,     // 同一コマンドがキューに無い場合のみ送信する
        FALSE      // GFL_NETライブラリの外で保持するバッファを使用
  );
  if( result ){
    BTL_N_Printf( DBGSTR_NET_SendAITrainerData, tr_data->tr_id );
  }

  delete_SendTrainerData( send_buf );

  return result;
}
static u8* getbuf_AI_trainerData( int netID, void* pWork, int size )
{
  Sys->tmpExBuffer = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(Sys->heapID), size );
  return Sys->tmpExBuffer;
}
static void recv_AI_trainerData( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  BTL_N_Printf( DBGSTR_NET_RecvAITrainerData, ((const BTL_TRAINER_SEND_DATA*)pData)->base_data.tr_id  );
  Sys->tmpExBufferUsedSize = size;
}
// AIトレーナーデータ受信完了したか
BOOL BTL_NET_IsRecved_AI_TrainerData( void )
{
  return (Sys->tmpExBufferUsedSize != 0);
}

// 受信したAIトレーナーデータを取得
const BTL_TRAINER_SEND_DATA* BTL_NET_Get_AI_TrainerData( void )
{
  GF_ASSERT(Sys->tmpExBuffer != NULL);
  GF_ASSERT(Sys->tmpExBufferUsedSize != 0);

  return Sys->tmpExBuffer;
}

void BTL_NET_Clear_AI_TrainerData( void )
{
  GF_ASSERT(Sys->tmpExBuffer != NULL);
  GF_ASSERT(Sys->tmpExBufferUsedSize != 0);

  GFL_HEAP_FreeMemory( Sys->tmpExBuffer );
  Sys->tmpExBuffer = NULL;
  Sys->tmpExBufferUsedSize = 0;
}
// プレイヤー・トレーナーデータ受信処理を完了する
void BTL_NET_EndNotifyPlayerData( void )
{
  clearAllTmpBuffer();
}

// 送信用トレーナー情報の生成・受信後のベーストレーナー情報の取得
static BTL_TRAINER_SEND_DATA* create_SendTrainerData( const BSP_TRAINER_DATA* tr_data, HEAPID heapID )
{
  BTL_TRAINER_SEND_DATA* send_tr_data;

  send_tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_TRAINER_SEND_DATA) );

  // ベース情報を保存
  GFL_STD_MemCopy( tr_data, &send_tr_data->base_data, sizeof(BSP_TRAINER_DATA) );

  // 送信トレーナー名バッファをEOMで初期化
  GFL_STD_MemFill16( send_tr_data->trainer_name, GFL_STR_GetEOMCode(), sizeof(STRCODE)*BTL_COMM_TRAINERNAME_MAX );

  // トレーナー名を保存
  if( tr_data->name ){
    send_tr_data->trainer_name_length = GFL_STR_GetBufferLength( tr_data->name );
    GF_ASSERT( send_tr_data->trainer_name_length < BTL_COMM_TRAINERNAME_MAX );
    STRTOOL_Copy( GFL_STR_GetStringCodePointer( tr_data->name ), send_tr_data->trainer_name, BTL_COMM_TRAINERNAME_MAX );
  }

  return send_tr_data;
}
static void delete_SendTrainerData( BTL_TRAINER_SEND_DATA* send_tr_data )
{
  GFL_HEAP_FreeMemory( send_tr_data );
}

/**
 *  全ての一時利用バッファを解放
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
    Sys->timingID = timingID;
    Sys->timingSyncStartFlag = TRUE;

    GFL_NET_HANDLE_TimeSyncBitStart( Sys->netHandle, timingID, WB_NET_BATTLE_SERVICEID, Sys->netIDBit );
  }
}

BOOL BTL_NET_IsTimingSync( u8 timingID )
{
  if( Sys )
  {
    if( Sys->timingSyncStartFlag )
    {
      BOOL is_sync = GFL_NET_HANDLE_IsTimeSync( Sys->netHandle, Sys->timingID, WB_NET_BATTLE_SERVICEID );
      if( is_sync ){
        Sys->timingID = BTL_NET_TIMING_NULL;
        Sys->timingSyncStartFlag = FALSE;
        return is_sync;
      }
    }
    else
    {
//      GFL_NET_TimingSyncStart( Sys->netHandle, Sys->timingID );
      GFL_NET_HANDLE_TimeSyncBitStart( Sys->netHandle, timingID, WB_NET_BATTLE_SERVICEID, Sys->netIDBit );
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
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
BOOL BTL_NET_SendToClient( u8 clientID, const void* adrs, u32 size )
{
  u8 netID = clientIDtoNetID( clientID );

//  GF_ASSERT(netID != NETID_NULL);
  if( netID != NETID_NULL )
  {
    BOOL result = GFL_NET_SendDataEx( Sys->netHandle, netID, CMD_TO_CLIENT, size, adrs,
          FALSE,    // 優先度を高くする
          FALSE,    // 同一コマンドがキューに無い場合のみ送信する
          TRUE      // GFL_NETライブラリの外で保持するバッファを使用
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
 * 全クライアントから返信があったかチェック
 *
 * @retval  BOOL    あったらTRUE
 */
//=============================================================================================
BOOL BTL_NET_CheckReturnFromClient( void )
{
  int i;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    if( IsBattleConnectNetID(i)
    &&  recvBuf_isEmpty(&Sys->recvClient[i])
    ){
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
  recvBuf_ClearAll();
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
        FALSE,    // 優先度を高くする
        TRUE,   // 同一コマンドがキューに無い場合のみ送信する
        TRUE    // GFL_NETライブラリの外で保持するバッファを使用
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
 *  すべての受信バッファを初期化
 */

static void recvBuf_ClearAll( void )
{
  int i;

  for(i=0; i<BTL_NET_CONNECT_MACHINE_MAX; i++)
  {
    recvBuf_clear( &Sys->recvClient[i] );
  }
}

/**
 *  受信バッファ：初期化
 */
static inline void recvBuf_clear( RECV_BUFFER* buf )
{
  buf->size = 0;
  buf->fCleared = TRUE;
}
/**
 *  受信バッファ：空（初期化）状態かチェック
 */
static inline BOOL recvBuf_isEmpty( const RECV_BUFFER* buf )
{
  return buf->fCleared;
}
/**
 *  受信バッファ：データ受信のためのバッファアドレス取得
 */
static inline u8* recvBuf_getBufAdrs( RECV_BUFFER* buf )
{
  return buf->data;
}
/**
 *  受信バッファ：データ受信後のパラメータ設定
 */
static inline void recvBuf_setRecvedSize( RECV_BUFFER* buf, u16 size )
{
  buf->size = size;
  buf->fCleared = FALSE;
}
/**
 *  受信バッファ：受信データ＆サイズ取得
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
  BTLNET_AIDATA_CONTAINER* container;

  container = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTLNET_AIDATA_CONTAINER)+size );
  container->dataSize = size;
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

