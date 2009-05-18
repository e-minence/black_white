//==============================================================================
/**
 * @file    game_beacon_search.c
 * @brief   通信ゲームをしているビーコンをサーチする
 * @author  matsuda
 * @date    2009.05.11(月)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "net/network_define.h"
#include "system/net_err.h"

#include "field/game_beacon_search.h"
#include "gamesystem/game_comm.h"
#include "field/field_comm/field_comm_func.h"


//==============================================================================
//  定数定義
//==============================================================================
///現在のステータス
typedef enum{
  GBS_STATUS_NULL,    ///<何も動作していない
  GBS_STATUS_INIT,    ///<初期化完了
  GBS_STATUS_UPDATE,  ///<メイン処理実行中
}GBS_STATUS;

//==============================================================================
//  構造体定義
//==============================================================================
///ヒットしたビーコンの持ち主の情報
typedef struct{
  GameServiceID gsid; ///<Game Service ID
  u8 member_num;      ///<現在の参加人数
  u8 member_max;      ///<最大人数
  u8 macAddress[6];
}GBS_TARGET_INFO;

///通信ゲームビーコンサーチシステムワーク構造体
typedef struct _GAME_BEACON_SYS
{
  GBS_STATUS status;        ///<TRUE:初期化済み
  BOOL fatal_err;   ///<TRUE:エラー発生
  GBS_BEACON beacon_send_data;    ///<送信ビーコンデータ
  GBS_TARGET_INFO target_info;    ///<ビーコンで見つけた相手の情報
}GAME_BEACON_SYS;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void GameBeacon_InitCallback(void *pWork);
static void	GameBeacon_ExitCallback(void* pWork);
static GBS_TARGET_INFO * GameBeacon_UpdateBeacon(GAME_BEACON_SYS_PTR gbs);
static GBS_BEACON * GameBeacon_BeaconSearch(GAME_BEACON_SYS_PTR gbs, int *hit_index);
static void* GameBeacon_GetBeaconData(void* pWork);
static BOOL GameBeacon_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );
static void GameBeacon_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void GameBeacon_DisconnectCallBack(void* pWork);
static const GBS_BEACON * GameBeacon_CompareBeacon( const GBS_BEACON *beacon_a , const GBS_BEACON *beacon_b );
static void GameBeacon_SetBeaconParam(GBS_BEACON *beacon);


//==============================================================================
//  データ
//==============================================================================
///
static const GFLNetInitializeStruct aGFLNetInit = {
	NULL,		//NetSamplePacketTbl,  // 受信関数テーブル
	0,			// 受信テーブル要素数
	NULL,		///< ハードで接続した時に呼ばれる
	NULL,		///< ネゴシエーション完了時にコール
	NULL,		// ユーザー同士が交換するデータのポインタ取得関数
	NULL,		// ユーザー同士が交換するデータのサイズ取得関数
	GameBeacon_GetBeaconData,		// ビーコンデータ取得関数  
	GameBeacon_GetBeaconSize,		// ビーコンデータサイズ取得関数 
	GameBeacon_CheckConnectService,		// ビーコンのサービスを比較して繋いで良いかどうか判断する
	GameBeacon_ErrorCallBack,		// 通信不能なエラーが起こった場合呼ばれる
	NULL,		//FatalError
	GameBeacon_DisconnectCallBack,		// 通信切断時に呼ばれる関数(終了時
	NULL,		// オート接続で親になった場合
#if GFL_NET_WIFI
	NULL,		///< wifi接続時に自分のデータをセーブする必要がある場合に呼ばれる関数
	NULL,		///< wifi接続時にフレンドコードの入れ替えを行う必要がある場合呼ばれる関数
	NULL,		///< wifiフレンドリスト削除コールバック
	NULL,		///< DWC形式の友達リスト	
	NULL,		///< DWCのユーザデータ（自分のデータ）
	0,			///< DWCへのHEAPサイズ
	TRUE,		///< デバック用サーバにつなぐかどうか
#endif  //GFL_NET_WIFI
	0x444,		//ggid  DP=0x333,RANGER=0x178,WII=0x346
	GFL_HEAPID_APP,		//元になるheapid
	HEAPID_NETWORK + HEAPDIR_MASK,		//通信用にcreateされるHEAPID
	HEAPID_WIFI + HEAPDIR_MASK,		//wifi用にcreateされるHEAPID
	HEAPID_NETWORK + HEAPDIR_MASK,		//
	GFL_WICON_POSX,GFL_WICON_POSY,	// 通信アイコンXY位置
	4,//_MAXNUM,	//最大接続人数
	48,//_MAXSIZE,	//最大送信バイト数
	4,//_BCON_GET_NUM,  // 最大ビーコン収集数
	TRUE,		// CRC計算
	FALSE,		// MP通信＝親子型通信モードかどうか
	GFL_NET_TYPE_WIRELESS_SCANONLY,		//通信タイプの指定
	TRUE,		// 親が再度初期化した場合、つながらないようにする場合TRUE
	WB_NET_FIELDMOVE_SERVICEID,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// 赤外線タイムアウト時間
#endif
};



//==================================================================
/**
 * 初期化処理
 *
 * @retval  GAME_BEACON_SYS_PTR		ゲームビーコンシステムへのポインタ
 */
//==================================================================
void * GameBeacon_Init(int *seq, void *pwk)
{
	GAME_BEACON_SYS_PTR gbs;
	
	gbs = GFL_HEAP_AllocClearMemory(GFL_HEAP_LOWID(GFL_HEAPID_APP), sizeof(GAME_BEACON_SYS));
	GFL_NET_Init(&aGFLNetInit, GameBeacon_InitCallback, gbs);
	return gbs;
}

//==================================================================
/**
 * 初期化完了確認
 *
 * @param   pWork		
 *
 * @retval  BOOL		TRUE:初期化完了     FALSE:初期化完了待ち
 */
//==================================================================
BOOL GameBeacon_InitWait(int *seq, void *pwk, void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;

  if(gbs->status >= GBS_STATUS_INIT){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 初期化完了コールバック
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void GameBeacon_InitCallback(void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  gbs->status = GBS_STATUS_INIT;
}

//==================================================================
/**
 * 終了処理
 *
 * @param   gbs		
 *
 * ※この関数呼出し語、必ずGameBeacon_ExitWaitで終了待ちの確認をしてください
 */
//==================================================================
void GameBeacon_Exit(int *seq, void *pwk, void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
	GFL_NET_Exit(GameBeacon_ExitCallback);
}

//==================================================================
/**
 * 終了待ち
 *
 * @param   gbs		
 *
 * @retval  BOOL		TRUE:終了完了。　FALSE:終了待ち中
 */
//==================================================================
BOOL GameBeacon_ExitWait(int *seq, void *pwk, void *pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;

  if(gbs->status == GBS_STATUS_NULL){
    GFL_HEAP_FreeMemory(gbs);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 終了完了コールバック
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void	GameBeacon_ExitCallback(void* pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  OS_TPrintf("GameBeacon:ExitCallback\n");
  gbs->status = GBS_STATUS_NULL;
}

//==================================================================
/**
 * 更新処理
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//==================================================================
void GameBeacon_Update(int *seq, void *pwk, void *pWork)
{
  GAME_COMM_SYS_PTR gcsp = pwk;
  GAME_BEACON_SYS_PTR gbs = pWork;
  GBS_TARGET_INFO *target;
  
  target = GameBeacon_UpdateBeacon(gbs);
  if(target != NULL){ //対象が見つかった
    switch(target->gsid){
    case WB_NET_FIELDMOVE_SERVICEID:  //侵入(パレス)
      {
        FIELD_INVALID_PARENT_WORK *invalid_parent;
        int i;
        
        invalid_parent = GFL_HEAP_AllocClearMemory(
            GFL_HEAP_LOWID(GFL_HEAPID_APP), sizeof(FIELD_INVALID_PARENT_WORK));
        invalid_parent->game_comm = gcsp;
        for(i = 0; i < 6; i++){
          invalid_parent->parent_macAddress[i] = target->macAddress[i];
        }
        GameCommSys_ChangeReq(gcsp, GAME_COMM_NO_INVASION, invalid_parent);
      }
      break;
    default:
      OS_TPrintf("フィールドから繋ぐgsidでは無い為、無視 gsid = %d\n", target->gsid);
      break;
    }
  }
}

//--------------------------------------------------------------
/**
 * ゲームビーコン：アップデート
 * 
 *
 * @param   gbs		
 *
 * @retval  対象が見つかっていればターゲット情報のポインタが返る
 * @retval  見つかっていない場合、NULL
 */
//--------------------------------------------------------------
static GBS_TARGET_INFO * GameBeacon_UpdateBeacon(GAME_BEACON_SYS_PTR gbs)
{
  GBS_BEACON *beacon;
  GBS_TARGET_INFO * target = &gbs->target_info;
  
  if(gbs->status == GBS_STATUS_NULL){
    return NULL;
  }
  if(gbs->status == GBS_STATUS_INIT){
    GFL_NET_StartBeaconScan();
    gbs->status = GBS_STATUS_UPDATE;
    return NULL;
  }
  
  //ビーコンサーチ
  if(target->gsid == 0){ //誰も見つけていない場合にサーチをする
    int hit_index;
    
    beacon = GameBeacon_BeaconSearch(gbs, &hit_index);
    if(beacon != NULL){
      u8 *mac_address;
      int i;
      
      GFL_STD_MemClear(target, sizeof(GBS_TARGET_INFO));
      target->gsid = beacon->gsid;
      target->member_num = beacon->member_num;
      target->member_max = beacon->member_max;
      mac_address = GFL_NET_GetBeaconMacAddress(hit_index);
      for(i = 0; i < 6; i++){
        target->macAddress[i] = mac_address[i];
      }
    }
  }
  
  if(target->gsid != 0){
    return target;
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * ビーコンサーチ
 *
 * @param   gbs		
 * @param   hit_index       見つかった場合、ビーコンバッファindexが代入される
 *
 * @retval  GBS_BEACON *		見つかったビーコンデータへのポインタ(無かった場合はNULL)
 */
//--------------------------------------------------------------
static GBS_BEACON * GameBeacon_BeaconSearch(GAME_BEACON_SYS_PTR gbs, int *hit_index)
{
  int i;
  GBS_BEACON *bcon_buff;
  int target_index = -1;
  
  for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
  	bcon_buff = GFL_NET_GetBeaconData(i);
  	if(bcon_buff != NULL && bcon_buff->member_num <= bcon_buff->member_max){
  		OS_TPrintf("ビーコン受信　%d番 gsid = %d\n", i, bcon_buff->gsid);
  		if(target_index == -1){
        target_index = i;
      }
      else{
        if(GameBeacon_CompareBeacon(bcon_buff, GFL_NET_GetBeaconData(target_index)) == bcon_buff){
          target_index = i;
        }
      }
  	}
  }
  
  if(target_index != -1){
    *hit_index = target_index;
    return GFL_NET_GetBeaconData(target_index);
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * ビーコンデータ取得関数
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 *
 * @retval  void*		ビーコンデータへのポインタ
 */
//--------------------------------------------------------------
static void* GameBeacon_GetBeaconData(void* pWork)
{
  GAME_BEACON_SYS_PTR gbs = pWork;
  
  GameBeacon_SetBeaconParam(&gbs->beacon_send_data);
  return &gbs->beacon_send_data;
}

//==================================================================
/**
 * 送信するビーコンデータを作成する
 *
 * @param   beacon		ビーコンパラメータ代入先
 */
//==================================================================
static void GameBeacon_SetBeaconParam(GBS_BEACON *beacon)
{
  GFL_STD_MemClear(beacon, sizeof(GBS_BEACON));
  beacon->gsid = aGFLNetInit.gsid;
  beacon->member_num = GFL_NET_GetConnectNum();
  beacon->member_max = 4;
}

//--------------------------------------------------------------
/**
 * ビーコンデータサイズ取得関数
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 *
 * @retval  int		ビーコンデータサイズ
 */
//--------------------------------------------------------------
int GameBeacon_GetBeaconSize(void *pWork)
{
  return sizeof(GBS_BEACON);
}

//--------------------------------------------------------------
/**
 * ビーコンのサービスを比較して繋いで良いかどうか判断する
 *
 * @param   GameServiceID1		サービスID１
 * @param   GameServiceID2		サービスID２
 *
 * @retval  BOOL		TRUE:接続OK　　FALSE:接続NG
 */
//--------------------------------------------------------------
static BOOL GameBeacon_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
  return (GameServiceID1==GameServiceID2);
}

//--------------------------------------------------------------
/**
 * 通信不能なエラーが起こった場合呼ばれる 切断するしかない
 *
 * @param   pNet		ネットハンドル
 * @param   errNo		エラー番号
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void GameBeacon_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	GAME_BEACON_SYS_PTR gbs = pWork;

  NetErr_ErrorSet();
  gbs->fatal_err = TRUE;
}

//--------------------------------------------------------------
/**
 * 通信切断時に呼ばれる関数(終了時
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void GameBeacon_DisconnectCallBack(void* pWork)
{
  OS_TPrintf("GameBeacon:通信切断コールバック GameBeacon_DisconnectCallBack\n");
}

//--------------------------------------------------------------
/**
 * ビーコン比較(人数が多い、電波状況などを加味して優先するビーコンを返す)
 *
 * @param   beacon_a		比較ビーコンA
 * @param   beacon_b		比較ビーコンB
 *
 * @retval  const GBS_BEACON *		優先するビーコンのポインタを返す
 */
//--------------------------------------------------------------
static const GBS_BEACON * GameBeacon_CompareBeacon( const GBS_BEACON *beacon_a , const GBS_BEACON *beacon_b )
{
  //※check　電波状況の加味は後で入れる
  
  if(beacon_a->member_num >= beacon_b->member_num){
    return beacon_a;  //人数が多い方を優先
  }
  return beacon_b;
}


