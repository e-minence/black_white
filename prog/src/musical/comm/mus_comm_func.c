//======================================================================
/**
 * @file	mus_comm_func.c
 * @brief	ミュージカル通信機能
 * @author	ariizumi
 * @data	09/05/29
 *
 * モジュール名：MUS_COMM_FUNC
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "net/network_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "mus_comm_func.h"
#include "mus_comm_define.h"
#include "musical/musical_system.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define MUS_COMM_BEACON_MAX (4)
#define MUS_COMM_POKEDATA_SIZE (sizeof(MUSICAL_POKE_PARAM)+POKETOOL_GetWorkSize())

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//送信の種類
typedef enum
{
  MCST_FLG = GFL_NET_CMD_MUSICAL,
  MCST_POKEDATA ,
  MCST_STRMDATA ,
  
  MCST_MAX,
}MUS_COMM_SEND_TYPE;

//Flg送信の種類
typedef enum
{
  MCFT_START_GAME,
  MCFT_GAME_STATE,
  MCFT_STRM_SIZE,
  
}MUS_COMM_FLAG_TYPE;

//通信状態
typedef enum
{
  MCS_NONE,

  MCS_PARENT_REQ_NEGOTIATION,
  MCS_PARENT_WAIT_NEGOTIATION,
  MCS_PARENT_COLLECT,

  MCS_CHILD_SEARCH,
  MCS_CHILD_REQ_NEGOTIATION,
  MCS_CHILD_WAIT_NEGOTIATION,
  MCS_CHILD_CONNECT,
  
}MUS_COMM_STATE;

typedef enum
{
  MCDS_NONE,
  MCDS_START,
  MCDS_WAIT_SIZEDATA,
  MCDS_SEND,
  MCDS_WAIT_POST,
  
}MUS_COMM_DIVSEND_STATE;
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
  BOOL                isValidData;  //pokeData受信完了フラグ
  u8                  musicalIdx;
  MUS_COMM_GAME_STATE gameState;
  void *pokeData;
}MUS_COMM_USER_DATA;

struct _MUS_COMM_WORK
{
  HEAPID heapId;
  
  BOOL isInitComm;
  BOOL isRefreshUserData;
  BOOL isStartGame;
  u8   befMemberNum;
  
  MUS_COMM_BEACON beacon;
  MUS_COMM_MODE mode;   //親か子か？
  MUS_COMM_STATE commState; //もっと細かい分岐

  GAME_COMM_SYS_PTR gameComm;
  MYSTATUS *myStatus;
  
  MUS_COMM_USER_DATA userData[MUSICAL_COMM_MEMBER_NUM];
  void *selfPokeData;
  
  MUSICAL_DISTRIBUTE_DATA *distData;
  u8  strmDivIdx;
  BOOL isSendStrmMode;
  BOOL isSendingStrmData;
  MUS_COMM_DIVSEND_STATE divSendState;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void MUS_COMM_FinishNetInitCallback( void* pWork );
static void MUS_COMM_FinishNetTermCallback( void* pWork );

static const BOOL MUS_COMM_Send_Flag( MUS_COMM_WORK* work , const u8 flag , u32 value , NetID target );
static void MUS_COMM_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static void MUS_COMM_Post_MusPokeData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8*    MUS_COMM_Post_MusPokeDataBuff( int netID, void* pWork , int size );

static void MUS_COMM_Update_SendStrmData( MUS_COMM_WORK *work );
static void MUS_COMM_Post_StrmData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8*    MUS_COMM_Post_StrmDataBuff( int netID, void* pWork , int size );

static void* MUS_COMM_GetUserData(void* pWork);
static int MUS_COMM_GetUserDataSize(void* pWork);
static void MUS_COMM_CheckUserData( MUS_COMM_WORK *work );

static void*  MUS_COMM_GetBeaconData(void* pWork);
static int MUS_COMM_GetBeaconSize(void *pWork);
static BOOL MUS_COMM_BeacomCompare(GameServiceID GameServiceID1, GameServiceID GameServiceID2);

inline static void MUS_COMM_SetCommState( MUS_COMM_WORK *work , const MUS_COMM_STATE commState );

static const NetRecvFuncTable MusCommRecvTable[MCST_MAX] = 
{
  { MUS_COMM_Post_Flag   ,NULL  },
  { MUS_COMM_Post_MusPokeData , MUS_COMM_Post_MusPokeDataBuff },
  { MUS_COMM_Post_StrmData , MUS_COMM_Post_StrmDataBuff },
};

//--------------------------------------------------------------
//	ワーク作成
//--------------------------------------------------------------
MUS_COMM_WORK* MUS_COMM_CreateWork( HEAPID heapId , GAME_COMM_SYS_PTR gameComm , SAVE_CONTROL_WORK *saveCtrl , MUSICAL_DISTRIBUTE_DATA *distData )
{
  u8 i;
  MUS_COMM_WORK* work = GFL_HEAP_AllocClearMemory( heapId , sizeof( MUS_COMM_WORK ));
  
  work->gameComm = gameComm;
  work->mode = MCM_NONE;
  work->isInitComm = FALSE;
  work->selfPokeData = NULL;
  work->myStatus = SaveData_GetMyStatus(saveCtrl);
  work->distData = distData;
  work->strmDivIdx = 0;
  work->isSendStrmMode = FALSE;
  work->divSendState = MCDS_NONE;
  
  for( i=0;i<MUSICAL_COMM_MEMBER_NUM;i++ )
  {
    work->userData[i].pokeData = NULL;
  }

  MUS_COMM_SetCommState( work , MCS_NONE);

  return work;
}

//--------------------------------------------------------------
// ワーク削除
//--------------------------------------------------------------
void MUS_COMM_DeleteWork( MUS_COMM_WORK* work )
{
  u8 i;
  
  for( i=0;i<MUSICAL_COMM_MEMBER_NUM;i++ )
  {
    if( work->userData[i].pokeData != NULL )
    {
      GFL_HEAP_FreeMemory( work->userData[i].pokeData );
    }
  }
  if( work->selfPokeData != NULL )
  {
    GFL_HEAP_FreeMemory( work->selfPokeData );
  }

  if( work->isInitComm == TRUE )
  {
    GFL_NET_Exit(NULL);
  }

  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
// 通信開始
//--------------------------------------------------------------
void MUS_COMM_InitComm( MUS_COMM_WORK* work )
{
  GFLNetInitializeStruct aGFLNetInit = 
  {
    MusCommRecvTable, //NetSamplePacketTbl,  // 受信関数テーブル
    NELEMS(MusCommRecvTable), // 受信テーブル要素数
    NULL,    ///< ハードで接続した時に呼ばれる
    NULL, ///< ネゴシエーション完了時にコール
    MUS_COMM_GetUserData, // ユーザー同士が交換するデータのポインタ取得関数
    MUS_COMM_GetUserDataSize, // ユーザー同士が交換するデータのサイズ取得関数
    MUS_COMM_GetBeaconData,   // ビーコンデータ取得関数  
    MUS_COMM_GetBeaconSize,   // ビーコンデータサイズ取得関数 
    MUS_COMM_BeacomCompare, // ビーコンのサービスを比較して繋いで良いかどうか判断する
    NULL, // 通信不能なエラーが起こった場合呼ばれる
    NULL, //FatalError
    NULL, // 通信切断時に呼ばれる関数(終了時
    NULL,                   // オート接続で親になった場合
#if GFL_NET_WIFI
    NULL,                   ///< wifi接続時に自分のデータをセーブする必要がある場合に呼ばれる関数
    NULL,                   ///< wifi接続時にフレンドコードの入れ替えを行う必要がある場合呼ばれる関数
    NULL,                   ///< wifiフレンドリスト削除コールバック
    NULL,                   ///< DWC形式の友達リスト  
    NULL,                   ///< DWCのユーザデータ（自分のデータ）
    0,                      ///< DWCへのHEAPサイズ
    TRUE,                   ///< デバック用サーバにつなぐかどうか
#endif  //GFL_NET_WIFI
    0x333,                  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,         //元になるheapid
    HEAPID_NETWORK,         //通信用にcreateされるHEAPID
    HEAPID_WIFI,            //wifi用にcreateされるHEAPID
    HEAPID_NETWORK,         //
    GFL_WICON_POSX,GFL_WICON_POSY,  // 通信アイコンXY位置
    MUSICAL_COMM_MEMBER_NUM,        //_MAXNUM,  //最大接続人数
    48,                     //_MAXSIZE, //最大送信バイト数
    MUS_COMM_BEACON_MAX,    //_BCON_GET_NUM,  // 最大ビーコン収集数
    TRUE,                   // CRC計算
    FALSE,                  // MP通信＝親子型通信モードかどうか
    GFL_NET_TYPE_WIRELESS,  //通信タイプの指定
    TRUE,                   // 親が再度初期化した場合、つながらないようにする場合TRUE
    WB_NET_MUSICAL,     //GameServiceID
#if GFL_NET_IRC
    IRC_TIMEOUT_STANDARD, // 赤外線タイムアウト時間
#endif
  };  
  
  u8 i;
  
  for( i=0;i<MUSICAL_COMM_MEMBER_NUM;i++ )
  {
    work->userData[i].musicalIdx = i;
    work->userData[i].gameState = MCGS_NONE;
    work->userData[i].isValidData = FALSE;
    if( work->userData[i].pokeData != NULL )
    {
      GFL_HEAP_FreeMemory( work->userData[i].pokeData );
    }
    work->userData[i].pokeData = GFL_HEAP_AllocMemory( work->heapId , MUS_COMM_POKEDATA_SIZE );
  }
  if( work->selfPokeData != NULL )
  {
    GFL_HEAP_FreeMemory( work->selfPokeData );
  }
  work->selfPokeData = NULL;
  work->befMemberNum = 0;
  work->isRefreshUserData = FALSE;
  work->isStartGame = FALSE;
  
  GFL_NET_Init( &aGFLNetInit , MUS_COMM_FinishNetInitCallback , (void*)work );

}

//--------------------------------------------------------------
// 通信終了
//--------------------------------------------------------------
void MUS_COMM_ExitComm( MUS_COMM_WORK* work )
{
  work->mode = MCM_NONE;
  GFL_NET_Exit(MUS_COMM_FinishNetInitCallback);
}

//--------------------------------------------------------------
// 初期化用コールバック
//--------------------------------------------------------------
static void MUS_COMM_FinishNetInitCallback( void* pWork )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  work->isInitComm = TRUE;
}

//--------------------------------------------------------------
// 終了用コールバック
//--------------------------------------------------------------
static void MUS_COMM_FinishNetTermCallback( void* pWork )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  work->isInitComm = FALSE;
}

//--------------------------------------------------------------
// 通信更新
//--------------------------------------------------------------
void MUS_COMM_UpdateComm( MUS_COMM_WORK* work )
{
  switch( work->commState )
  {
    //親機
  case MCS_PARENT_REQ_NEGOTIATION:
    if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
    {
      MUS_COMM_SetCommState( work , MCS_PARENT_WAIT_NEGOTIATION);
    }
    break;
    
  case MCS_PARENT_WAIT_NEGOTIATION:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_HANDLE_IsNegotiation( selfHandle ) == TRUE )
      {
        MUS_COMM_SetCommState( work , MCS_PARENT_COLLECT);
      }
    }
    break;
    
  case MCS_PARENT_COLLECT:
    MUS_COMM_CheckUserData( work );
    break;

    //子機
  case MCS_CHILD_SEARCH:
    {
      u16 i=0;
      void* beacon;
      do
      {
        beacon = GFL_NET_GetBeaconData( i );
        i++;
      }while( beacon == NULL && i < MUS_COMM_BEACON_MAX );
      
      if( beacon != NULL )
      {
        u8 *macAdd = GFL_NET_GetBeaconMacAddress( i-1 );
        GFL_NET_ConnectToParent( macAdd );
        MUS_COMM_SetCommState( work , MCS_CHILD_REQ_NEGOTIATION);
      }
    }
    break;

  case MCS_CHILD_REQ_NEGOTIATION:
    if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
    {
      MUS_COMM_SetCommState( work , MCS_CHILD_WAIT_NEGOTIATION);
    }
    break;

  case MCS_CHILD_WAIT_NEGOTIATION:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_HANDLE_IsNegotiation( selfHandle ) == TRUE )
      {
        MUS_COMM_SetCommState( work , MCS_CHILD_CONNECT);
      }
    }
    break;
  case MCS_CHILD_CONNECT:
    MUS_COMM_CheckUserData( work );
    break;
  }
  MUS_COMM_Update_SendStrmData( work );
}

//--------------------------------------------------------------
// 親機通信開始
//--------------------------------------------------------------
void MUS_COMM_StartParent( MUS_COMM_WORK* work )
{
  work->mode = MCM_PARENT;
  MUS_COMM_SetCommState(work , MCS_PARENT_REQ_NEGOTIATION);
  GFL_NET_InitServer();
}

//--------------------------------------------------------------
// 子機通信開始
//--------------------------------------------------------------
void MUS_COMM_StartChild( MUS_COMM_WORK* work )
{
  work->mode = MCM_CHILD;
  MUS_COMM_SetCommState( work , MCS_CHILD_SEARCH);
  GFL_NET_StartBeaconScan();
}

//--------------------------------------------------------------
// ゲーム開始(新規接続禁止とフラグの送信
//--------------------------------------------------------------
const BOOL MUS_COMM_StartGame( MUS_COMM_WORK* work )
{
  if( work->mode != MCM_PARENT )
  {
    GF_ASSERT_MSG(0, "MusComm mode is not parent[%d]\n",work->mode );
    return FALSE;
  }
  {
    GFL_NETHANDLE *handle = GFL_NET_HANDLE_GetCurrentHandle();
    GFL_NET_SetClientConnect( handle , FALSE );
   
    return MUS_COMM_Send_Flag( work , MCFT_START_GAME , 0 , GFL_NET_SENDID_ALLUSER );
  }
}
const BOOL MUS_COMM_IsStartGame( MUS_COMM_WORK* work )
{
  return work->isStartGame;
}


#pragma mark [> userData func
//--------------------------------------------------------------
// ユーザーデータ設定
//--------------------------------------------------------------
static void* MUS_COMM_GetUserData(void* pWork)
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  
  //TODO 念のため名前が入ってないときに落ちないようにしておく
  if( MyStatus_CheckNameClear( work->myStatus ) == TRUE )
  {
    u16 tempName[7] = { L'N',L'o',L'N',L'a',L'm',L'e',0xFFFF };
    MyStatus_SetMyName( work->myStatus , tempName );
  }

  
  return work->myStatus;
}

static int MUS_COMM_GetUserDataSize(void* pWork)
{
  return MyStatus_GetWorkSize();
}

static void MUS_COMM_CheckUserData( MUS_COMM_WORK *work )
{
  //新しいメンバーがいるか？
  if( work->befMemberNum != GFL_NET_GetConnectNum() )
  {
    work->befMemberNum = GFL_NET_GetConnectNum();
    work->isRefreshUserData = TRUE;
  }
}

const BOOL MUS_COMM_IsRefreshUserData( MUS_COMM_WORK *work )
{
  return work->isRefreshUserData;
}
void MUS_COMM_ResetRefreshUserData( MUS_COMM_WORK *work )
{
  work->isRefreshUserData = FALSE;
}

const BOOL MUS_COMM_SetCommGameState( MUS_COMM_WORK *work , MUS_COMM_GAME_STATE state )
{
  return MUS_COMM_Send_Flag( work , MCFT_GAME_STATE , state , GFL_NET_SENDID_ALLUSER );
}


//--------------------------------------------------------------
// タイミングコマンド
//--------------------------------------------------------------
void MUS_COMM_SendTimingCommand( MUS_COMM_WORK *work , const u8 no )
{
  GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  GFL_NET_TimingSyncStart( selfHandle , no );
  ARI_TPrintf("MusComm Send timming command[%d]\n",no);
}

const BOOL MUS_COMM_CheckTimingCommand( MUS_COMM_WORK *work , const u8 no )
{
  GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  if( GFL_NET_IsTimingSync( selfHandle , no ) == TRUE )
  {
    ARI_TPrintf("MusComm Sync timming command[%d]\n",no);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
// マイステータス取得
//--------------------------------------------------------------
MYSTATUS* MUS_COMM_GetPlayerMyStatus( MUS_COMM_WORK* work , u8 idx )
{
  if( work->commState != MCS_NONE )
  {
    const GFL_NETHANDLE *handle = GFL_NET_GetNetHandle( idx );
    if( handle == NULL )
    {
      return NULL;
    }
    else
    {
      return (MYSTATUS*)GFL_NET_HANDLE_GetInfomationData( handle );
    }
  }
  return NULL;
}


#pragma mark [> beacon func
//--------------------------------------------------------------
// ビーコンデータ取得関数  
//--------------------------------------------------------------
static void*  MUS_COMM_GetBeaconData(void* pWork)
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  return (void*)&work->beacon;
}

//--------------------------------------------------------------
// ビーコンデータサイズ取得関数 
//--------------------------------------------------------------
static int MUS_COMM_GetBeaconSize(void *pWork)
{
  return sizeof( MUS_COMM_BEACON );
}

//--------------------------------------------------------------
// 接続チェック用比較
//--------------------------------------------------------------
static BOOL MUS_COMM_BeacomCompare(GameServiceID GameServiceID1, GameServiceID GameServiceID2)
{
  if( GameServiceID1 == GameServiceID2 )
  {
    return TRUE;
  }
  return FALSE;
}


#pragma mark [> send/post func
//--------------------------------------------------------------
// フラグ送受信
//--------------------------------------------------------------
typedef struct
{
  u8 flg;
  u32 value;
}MUS_COMM_FLG_PACKET;

static const BOOL MUS_COMM_Send_Flag( MUS_COMM_WORK* work , const u8 flag , u32 value , NetID target )
{
  BOOL ret;
  GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  MUS_COMM_FLG_PACKET pkt;
  pkt.flg = flag;
  pkt.value = value;
  ARI_TPrintf("Send Flg[%d:%d].\n",pkt.flg,pkt.value);

  ret = GFL_NET_SendDataEx( selfHandle , target , 
                            MCST_FLG , sizeof( MUS_COMM_FLG_PACKET ) , 
                            (void*)&pkt , TRUE , FALSE , FALSE );
  if( ret == FALSE )
  {
    ARI_TPrintf("Send Flg is failued!!\n");
  }
  return ret;
}

static void MUS_COMM_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  MUS_COMM_FLG_PACKET *pkt = (MUS_COMM_FLG_PACKET*)pData;

  ARI_TPrintf("Post Flg[%d:%d] To [%d].\n",pkt->flg,pkt->value,netID);
  switch( pkt->flg )
  {
  case MCFT_START_GAME:
    work->isStartGame = TRUE;
    break;

  case MCFT_GAME_STATE:
    work->userData[netID].gameState = pkt->value;
    break;

  case MCFT_STRM_SIZE:
    if( work->mode == MCM_PARENT )
    {
      work->divSendState = MCDS_SEND;
    }
    else
    {
      work->distData->strmData = GFL_HEAP_AllocMemory( HEAPID_MUSICAL_STRM , pkt->value );
      work->distData->strmDataSize = pkt->value;
    }
    
    break;
  }
  
}
//--------------------------------------------------------------
// ポケモンデータ送受信
//--------------------------------------------------------------
const BOOL MUS_COMM_Send_MusPokeData( MUS_COMM_WORK *work , MUSICAL_POKE_PARAM *musPoke )
{
  if( work->selfPokeData != NULL )
  {
    GFL_HEAP_FreeMemory( work->selfPokeData );
  }
  work->selfPokeData = GFL_HEAP_AllocMemory( work->heapId , MUS_COMM_POKEDATA_SIZE );
  {
    u32 pokeParaAdr = (u32)work->selfPokeData+sizeof(MUSICAL_POKE_PARAM);
    GFL_STD_MemCopy( musPoke , work->selfPokeData , sizeof(MUSICAL_POKE_PARAM) );
    GFL_STD_MemCopy( musPoke->pokePara , (void*)pokeParaAdr , POKETOOL_GetWorkSize() );
  }
  
  ARI_TPrintf("Send PokeData \n");
  {
    
    GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
    BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
                              MCST_POKEDATA , MUS_COMM_POKEDATA_SIZE , 
                              work->selfPokeData , TRUE , FALSE , TRUE );
    if( ret == FALSE )
    {
      ARI_TPrintf("Send PokeData is failued!!\n");
    }
    return ret;
  }
}
static void MUS_COMM_Post_MusPokeData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  work->userData[netID].isValidData = TRUE;
  ARI_TPrintf("MusComm Finish Post PokeData[%d].\n",netID);
  
  {
    u32 pokeParaAdr = (u32)work->userData[netID].pokeData+sizeof(MUSICAL_POKE_PARAM);
    MUSICAL_POKE_PARAM *musPoke = work->userData[netID].pokeData;
    musPoke->pokePara = (void*)pokeParaAdr;
  }

}
static u8*    MUS_COMM_Post_MusPokeDataBuff( int netID, void* pWork , int size )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  ARI_TPrintf("MusComm Start Post PokeData[%d].\n",netID);
  work->userData[netID].isValidData = FALSE;
  return work->userData[netID].pokeData;
}

MUSICAL_POKE_PARAM* MUS_COMM_GetMusPokeParam( MUS_COMM_WORK *work , const u8 musicalIdx )
{
  u8 i;
  for( i=0;i<MUSICAL_COMM_MEMBER_NUM;i++ )
  {
    if( work->userData[i].musicalIdx == musicalIdx )
    {
      if( work->userData[i].isValidData == TRUE )
      {
        return work->userData[i].pokeData;
      }
    }
  }
  return NULL;
}

//--------------------------------------------------------------
// 全員ポケデータ届いたか？
//--------------------------------------------------------------
const BOOL MUS_COMM_CheckAllPostPokeData( MUS_COMM_WORK *work )
{
  u8 i;
  
  for( i=0;i<MUSICAL_COMM_MEMBER_NUM;i++ )
  {
    if( work->userData[i].gameState != MCGS_NONE &&
        work->userData[i].isValidData == FALSE )
    {
      return FALSE;
    }
  }
  return TRUE;
}

#pragma mark [> strm func
//--------------------------------------------------------------
// ストリーミングデータ送信
//--------------------------------------------------------------
#define MUS_COMM_DIV_SIZE (0x3800)
void MUS_COMM_Start_SendStrmData( MUS_COMM_WORK *work )
{
  if( work->isSendStrmMode == FALSE )
  {
    work->isSendStrmMode = TRUE;
    work->strmDivIdx = 0;
    work->divSendState = MCDS_START;
    work->isSendingStrmData = FALSE;
  }
}

static void MUS_COMM_Update_SendStrmData( MUS_COMM_WORK *work )
{
  if( work->isSendStrmMode == TRUE )
  {
    switch(work->divSendState)
    {
    case MCDS_START:
      {
        const BOOL ret = MUS_COMM_Send_Flag( work , MCFT_STRM_SIZE , 
                                      work->distData->strmDataSize , 
                                      GFL_NET_SENDID_ALLUSER );
        if( ret == TRUE )
        {
          work->divSendState = MCDS_WAIT_SIZEDATA;
        }
      }
      break;
    
    case MCDS_WAIT_SIZEDATA:
      
      break;

    case MCDS_SEND:
      if( MUS_COMM_Send_StrmData( work , work->strmDivIdx ) == TRUE )
      {
        work->isSendingStrmData = TRUE;
        work->divSendState = MCDS_WAIT_POST;
      }
      break;

    case MCDS_WAIT_POST:
      {
        if( work->isSendingStrmData == FALSE )
        {
          if( (work->strmDivIdx)*MUS_COMM_DIV_SIZE >= work->distData->strmDataSize )
          {
            work->isSendStrmMode = FALSE;
          }
          else
          {
            //work->strmDivIdx++;
            work->divSendState = MCDS_SEND;
          }
        }
      }
      break;
    }
  }
}

const BOOL MUS_COMM_Send_StrmData( MUS_COMM_WORK *work , const u8 idx )
{
  
  void *startAdr = (void*)(((u32)work->distData->strmData)+(idx*MUS_COMM_DIV_SIZE));
  u16 dataSize;
  if( (idx+1)*MUS_COMM_DIV_SIZE > work->distData->strmDataSize )
  {
    dataSize = work->distData->strmDataSize%MUS_COMM_DIV_SIZE;
  }
  else
  {
    dataSize = MUS_COMM_DIV_SIZE;
  }
  
  ARI_TPrintf("Send StrmData[%d] \n",idx);
  {
    
    GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
    BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
                              MCST_STRMDATA , dataSize , 
                              startAdr , FALSE , TRUE , TRUE );
    if( ret == FALSE )
    {
      ARI_TPrintf("Send PokeData is failued!!\n");
    }
    return ret;
  }
}
static void MUS_COMM_Post_StrmData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  work->strmDivIdx++;
  work->isSendingStrmData = FALSE;
  ARI_TPrintf("MusComm Finish Post StrmData[%d][%d][%d].\n",netID,work->strmDivIdx,work->distData->strmDataSize);

}
static u8*    MUS_COMM_Post_StrmDataBuff( int netID, void* pWork , int size )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  void *startAdd = (void*)(((u32)work->distData->strmData)+(work->strmDivIdx*MUS_COMM_DIV_SIZE));
  
  ARI_TPrintf("MusComm Start Post StrmData[%d][%d][%d].\n",netID,work->strmDivIdx,size);
  return startAdd;
}

#pragma mark [> outer func

GAME_COMM_SYS_PTR MUS_COMM_GetGameComm( MUS_COMM_WORK* work )
{
  return work->gameComm;
}

const BOOL MUS_COMM_IsInitComm( MUS_COMM_WORK* work )
{
  return work->isInitComm;
}

MUS_COMM_MODE MUS_COMM_GetMode( MUS_COMM_WORK* work )
{
  return work->mode;
}

#pragma mark [> other func

inline static void MUS_COMM_SetCommState( MUS_COMM_WORK *work , const MUS_COMM_STATE commState )
{
  ARI_TPrintf("MusCommState[%d]\n",commState);
  work->commState = commState;
}
