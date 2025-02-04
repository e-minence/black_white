//==============================================================================
/**
 * @file    union_comm.c
 * @brief   ユニオンルーム：通信制御
 * @author  matsuda
 * @date    2009.07.02(木)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "union_local.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_main.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "union_comm_command.h"
#include "union_msg.h"
#include "comm_player.h"
#include "colosseum_comm_command.h"
#include "colosseum_tool.h"
#include "union_subproc.h"
#include "union_chat.h"
#include "net/net_whpipe.h"
#include "net_app/union_app.h"


//==============================================================================
//  定数定義
//==============================================================================
///HEAPID_UNIONで確保するヒープサイズ
#define HEAP_SIZE_UNION               (0x8a00)  //(0x9800)  //(0xa000)

///通信ステータス
enum{
  UNION_COMM_STATUS_NULL,         ///<何も動作していない
  UNION_COMM_STATUS_INIT_START,   ///<初期化完了
  UNION_COMM_STATUS_INIT,         ///<初期化完了
  UNION_COMM_STATUS_UPDATE,       ///<メイン更新実行中
  UNION_COMM_STATUS_EXIT_START,   ///<終了処理開始
  UNION_COMM_STATUS_EXIT,         ///<終了処理完了
};


//--------------------------------------------------------------
//  オーバーレイID
//--------------------------------------------------------------
//FS_EXTERN_OVERLAY(union_room);


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void UnionComm_InitCallback(void *pWork);
static void	UnionComm_ExitCallback(void* pWork);
static void UnionComm_TalkShutdownUpdate(UNION_SYSTEM_PTR unisys);
static void UnionComm_MinigameUpdate(UNION_SYSTEM_PTR unisys);
static void UnionComm_Colosseum_Update(UNION_SYSTEM_PTR unisys);
static void UnionComm_BeaconSearch(UNION_SYSTEM_PTR unisys);
static BOOL UnionBeacon_SetReceiveData(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address);
static BOOL UnionBeacon_CheckSubMemberSameValid(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address);
static void* UnionComm_GetBeaconData(void* pWork);
static void UnionComm_SetBeaconParam(UNION_SYSTEM_PTR unisys, UNION_BEACON *beacon);
static int UnionComm_GetBeaconSize(void *pWork);
static BOOL UnionComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork );
static void UnionComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void UnionComm_DisconnectCallBack(void* pWork);
static void UnionChat_InitLog(UNION_CHAT_LOG *log);


//==============================================================================
//  データ
//==============================================================================
static const GFLNetInitializeStruct aGFLNetInit = {
	Union_CommPacketTbl,  // 受信関数テーブル
	UNION_CMD_NUM,  			// 受信テーブル要素数
	NULL,		///< ハードで接続した時に呼ばれる
	NULL,		///< ネゴシエーション完了時にコール
	NULL,		// ユーザー同士が交換するデータのポインタ取得関数
	NULL,		// ユーザー同士が交換するデータのサイズ取得関数
	UnionComm_GetBeaconData,		// ビーコンデータ取得関数  
	UnionComm_GetBeaconSize,		// ビーコンデータサイズ取得関数 
	UnionComm_CheckConnectService,		// ビーコンのサービスを比較して繋いで良いかどうか判断する
	UnionComm_ErrorCallBack,		// 通信不能なエラーが起こった場合呼ばれる
	NULL,		//FatalError
	UnionComm_DisconnectCallBack,		// 通信切断時に呼ばれる関数(終了時
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
  SYACHI_NETWORK_GGID,  //ggid  
	GFL_HEAPID_APP,		//元になるheapid
	HEAPID_NETWORK + HEAPDIR_MASK,		//通信用にcreateされるHEAPID
	HEAPID_WIFI + HEAPDIR_MASK,		//wifi用にcreateされるHEAPID
	HEAPID_NETWORK + HEAPDIR_MASK,		//
	GFL_WICON_POSX,GFL_WICON_POSY,	// 通信アイコンXY位置
	UNION_CONNECT_PLAYER_NUM,      //_MAXNUM,	//最大接続人数
	90, //NET_SEND_SIZE_STANDARD,     //_MAXSIZE,	//最大送信バイト数
	UNION_RECEIVE_BEACON_MAX,  // 最大ビーコン収集数
	TRUE,		// CRC計算
	FALSE,		// MP通信＝親子型通信モードかどうか
	GFL_NET_TYPE_WIRELESS,		//通信タイプの指定
	TRUE,		// 親が再度初期化した場合、つながらないようにする場合TRUE
	WB_NET_UNION,	//GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD,	// 赤外線タイムアウト時間
#endif
    0,//MP親最大サイズ 512まで
    0,//dummy
};


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ユニオンシステム初期化
 *
 * @retval  UNION_SYSTEM_PTR		生成したユニオンシステムワークへのポインタ
 */
//==================================================================
static UNION_SYSTEM_PTR Union_InitSystem(UNION_PARENT_WORK *uniparent)
{
  UNION_SYSTEM_PTR unisys;
  
  GFL_HEAP_CreateHeap( HEAPID_PROC, HEAPID_UNION, HEAP_SIZE_UNION );
  unisys = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(UNION_SYSTEM));
  unisys->uniparent = uniparent;
  UnionMySituation_Clear(unisys);
  unisys->my_situation.chat_upload = TRUE;
  UnionChat_InitLog(&unisys->chat_log);
  unisys->alloc.regulation = Regulation_AllocWork(HEAPID_UNION);
  
//  GFL_OVERLAY_Load( FS_OVERLAY_ID( union_room ) );
  unisys->overlay_load = TRUE;
  
  return unisys;
}

//==================================================================
/**
 * ユニオンシステム終了
 *
 * @param   unisys		ユニオンシステムワークへのポインタ
 */
//==================================================================
static void Union_ExitSystem(UNION_PARENT_WORK *uniparent, UNION_SYSTEM_PTR unisys)
{
  if(unisys->alloc.regulation != NULL){
    GFL_HEAP_FreeMemory(unisys->alloc.regulation);
  }
  UnionAlloc_AllFree(unisys);
  GFL_HEAP_FreeMemory(unisys);
  GFL_HEAP_DeleteHeap(HEAPID_UNION);
  
  GFL_HEAP_FreeMemory(uniparent);
//  GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_room ) );
}



//==============================================================================
//  game_comm
//==============================================================================
//==================================================================
/**
 * 初期化処理
 *
 * @retval  UNION_SYSTEM_PTR		ユニオンシステムへのポインタ
 */
//==================================================================
void * UnionComm_Init(int *seq, void *pwk)
{
	UNION_SYSTEM_PTR unisys;
	
	unisys = Union_InitSystem(pwk);
	
	unisys->comm_status = UNION_COMM_STATUS_INIT_START;
	GFL_NET_Init(&aGFLNetInit, UnionComm_InitCallback, unisys);

	return unisys;
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
BOOL UnionComm_InitWait(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;

  if(unisys->comm_status >= UNION_COMM_STATUS_INIT){
    GF_ASSERT(unisys->comm_status < UNION_COMM_STATUS_UPDATE);
    unisys->comm_status = UNION_COMM_STATUS_UPDATE;
//    GFL_NET_StartBeaconScan();
    GFL_NET_Changeover(NULL);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 初期化完了コールバック　※切断→再開のコールバックとしても使用するので中身の変更には注意
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void UnionComm_InitCallback(void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  unisys->comm_status = UNION_COMM_STATUS_INIT;
  GFL_NET_ParentHandleError(FALSE);
}

//==================================================================
/**
 * 終了処理
 *
 * @param   unisys		
 */
//==================================================================
BOOL UnionComm_Exit(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  unisys->finish_exe = TRUE;
  
  GF_ASSERT(unisys->comm_status == UNION_COMM_STATUS_UPDATE);
  if(unisys->comm_status < UNION_COMM_STATUS_UPDATE){
    return FALSE; //通常ありえないけど念のためUpdateになるまで待つようにする
  }
  
  unisys->comm_status = UNION_COMM_STATUS_EXIT_START;
	if(GFL_NET_Exit(UnionComm_ExitCallback) == FALSE){
    UnionComm_ExitCallback(unisys);  //FALSEの場合は通信エラーなどで既に終了している
  }
	return TRUE;
}

//==================================================================
/**
 * 終了待ち
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:終了完了。　FALSE:終了待ち中
 */
//==================================================================
BOOL UnionComm_ExitWait(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;

  if(unisys->comm_status == UNION_COMM_STATUS_EXIT){
    Union_ExitSystem(pwk, unisys);
    //終了でエラーが残っていると常時通信が起動できない為、エラークリアしておく
    if(NetErr_App_CheckError()){
      NetErr_ErrWorkInit();
    }
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 終了完了コールバック ※切断→再開のコールバックとしても使用するので中身の変更には注意
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void	UnionComm_ExitCallback(void* pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  OS_TPrintf("ExitCallback\n");
  unisys->comm_status = UNION_COMM_STATUS_EXIT;
}

//==================================================================
/**
 * 切断→再開リクエスト
 *
 * @param   unisys		
 *
 * 接続している人との通信を終了し、ただのビーコン送受信状態に戻す
 */
//==================================================================
void UnionComm_Req_ShutdownRestarts(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq != 0){
    GF_ASSERT(0); //既にリスタート処理が行われている
    return;
  }
  unisys->restart_seq = 1;  //LOCALSEQ_MODE_CHECK
  unisys->restart_mode = UNION_RESTART_SHUTDOWN_RESTARTS;
  OS_TPrintf("切断→再開リクエストを受け付けました\n");
}

//==================================================================
/**
 * 切断リクエスト(UnionComm_Req_ShutdownRestartsを切断までの処理で完了)
 *
 * @param   unisys		
 *
 * 接続している人との通信を終了する
 */
//==================================================================
void UnionComm_Req_Shutdown(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq != 0){
    GF_ASSERT(0); //既にリスタート処理が行われている
    return;
  }
  unisys->restart_seq = 1;  //LOCALSEQ_MODE_CHECK
  unisys->restart_mode = UNION_RESTART_SHUTDOWN;
  OS_TPrintf("切断リクエストを受け付けました\n");
}

//==================================================================
/**
 * 再開リクエスト(UnionComm_Req_Shutdownで切断完了後に使用する)
 *
 * @param   unisys		
 *
 * ビーコン送受信を開始する
 */
//==================================================================
void UnionComm_Req_Restarts(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq != 0){
    GF_ASSERT(0); //既にリスタート処理が行われている
    return;
  }
  unisys->restart_seq = 1;  //LOCALSEQ_MODE_CHECK
  unisys->restart_mode = UNION_RESTART_RESTARTS;
  OS_TPrintf("再開リクエストを受け付けました\n");
}

//==================================================================
/**
 * 「切断→再開」処理を行っているか調べる
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:実行中　FALSE:何もしていない
 */
//==================================================================
BOOL UnionComm_Check_ShutdownRestarts(UNION_SYSTEM_PTR unisys)
{
  if(unisys->restart_seq == 0){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 切断→再開処理
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:「切断→再開処理」実行中
 */
//--------------------------------------------------------------
static BOOL UnionComm_ShutdownRestarts(UNION_SYSTEM_PTR unisys)
{
  enum{
    LOCALSEQ_NULL,
    LOCALSEQ_MODE_CHECK,
    LOCALSEQ_SHUTDOWN,
    LOCALSEQ_SHUTDOWN_WAIT,
    LOCALSEQ_RESTART,
    LOCALSEQ_RESTART_WAIT,
  };
  
  switch(unisys->restart_seq){
  case LOCALSEQ_NULL:
    if(unisys->restart_mode == UNION_RESTART_SHUTDOWN){
      return TRUE;  //切断のみで終わらせている状態なので通信は始まっていない。TRUEを返す
    }
    return FALSE;
  case LOCALSEQ_MODE_CHECK:
    switch(unisys->restart_mode){
    case UNION_RESTART_SHUTDOWN_RESTARTS:
    case UNION_RESTART_SHUTDOWN:
      unisys->restart_seq = LOCALSEQ_SHUTDOWN;
      break;
    case UNION_RESTART_RESTARTS:
      unisys->restart_seq = LOCALSEQ_RESTART;
      break;
    default:
      unisys->restart_seq = 0;
      GF_ASSERT(0);
      break;
    }
    break;
  case LOCALSEQ_SHUTDOWN:
    OS_TPrintf("restart 切断→再接続開始\n");
    unisys->comm_status = UNION_COMM_STATUS_EXIT_START;
    if(GFL_NET_Exit(UnionComm_ExitCallback) == FALSE){
      UnionComm_ExitCallback(unisys); //FALSEの場合は通信エラーなどで既に終了している
    }
    unisys->restart_seq++;
    break;
  case LOCALSEQ_SHUTDOWN_WAIT:
    if(unisys->comm_status == UNION_COMM_STATUS_EXIT){
      OS_TPrintf("restart 切断完了\n");
      if(unisys->restart_mode == UNION_RESTART_SHUTDOWN){
        unisys->restart_seq = LOCALSEQ_NULL;
      }
      else{
        unisys->restart_seq++;
      }
    }
    break;
  case LOCALSEQ_RESTART:
    OS_TPrintf("restart 初期化開始\n");
  	unisys->comm_status = UNION_COMM_STATUS_INIT_START;
  	GFL_NET_Init(&aGFLNetInit, UnionComm_InitCallback, unisys);
    unisys->restart_seq++;
    break;
  case LOCALSEQ_RESTART_WAIT:
    if(unisys->comm_status >= UNION_COMM_STATUS_INIT){
      OS_TPrintf("restart 初期化完了\n");
      unisys->comm_status = UNION_COMM_STATUS_UPDATE;
      GFL_NET_Changeover(NULL);
      unisys->restart_seq = LOCALSEQ_NULL;
    }
    break;
  }
  
  return TRUE;
}

//==================================================================
/**
 * 更新処理
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//==================================================================
void UnionComm_Update(int *seq, void *pwk, void *pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  
  if(UnionComm_ShutdownRestarts(unisys) == TRUE){
    return; //切断→再開 の処理を実行中
  }
  
  if(!GFL_NET_IsInit()){
    return;
  }
  if(unisys->send_beacon_update == TRUE){
    NET_WHPIPE_BeaconSetInfo();
    unisys->send_beacon_update = FALSE;
  }

  UnionComm_BeaconSearch(unisys);   //ビーコンサーチ
  
  UnionComm_Colosseum_Update(unisys); //コロシアム更新
  
  UnionComm_MinigameUpdate(unisys); //ミニゲーム更新

  UnionComm_TalkShutdownUpdate(unisys);

  //コロシアムでフリームーブ中、エラーが発生している場合はネット切断する
  //(他の子機がエラーを検出できるように)
  if(NetErr_App_CheckError() != NET_ERR_CHECK_NONE && GFL_NET_IsParentMachine() == TRUE 
      && unisys->colosseum_sys != NULL && unisys->colosseum_sys->comm_ready == TRUE 
      && unisys->my_situation.union_status == UNION_STATUS_COLOSSEUM_NORMAL
      && unisys->colosseum_sys->parent_force_exit == FALSE){
    OS_TPrintf("parent force exit!\n");
    UnionComm_Req_Shutdown(unisys);
    unisys->colosseum_sys->parent_force_exit = TRUE;
  }
}

//==================================================================
/**
 * 会話中の切断リクエスト処理を実行させる
 *
 * @param   unisys		
 */
//==================================================================
void UnionComm_SetTalkShutdown(UNION_SYSTEM_PTR unisys)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  situ->mycomm.talk_exit_exe = TRUE;
}

//==================================================================
/**
 * 会話中の切断リクエスト処理を実行されているか調べる
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:実行されている
 */
//==================================================================
BOOL UnionComm_GetTalkShutdown(UNION_SYSTEM_PTR unisys)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  return situ->mycomm.talk_exit_exe;
}

//--------------------------------------------------------------
/**
 * 会話中の切断リクエスト処理更新
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_TalkShutdownUpdate(UNION_SYSTEM_PTR unisys)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  if(situ->mycomm.talk_exit_exe == FALSE || NetErr_App_CheckError()){
    return;
  }
  
  switch(situ->mycomm.talk_exit_seq){
  case 0:
    if(UnionSend_TalkShutdown() == TRUE){
      situ->mycomm.talk_exit_seq++;
    }
    break;
  case 1:
    {
      u32 exit_bit = situ->mycomm.talk_exit_req_bit;
      if(MATH_CountPopulation(exit_bit) >= GFL_NET_GetConnectNum()){
        GFL_NET_SetNoChildErrorCheck(FALSE);  //切断許可
        situ->mycomm.talk_exit_seq++;
      }
    }
    break;
  case 2:
    if(GFL_NET_IsParentMachine() == TRUE){
      if(GFL_NET_GetConnectNum() <= 1){
        UnionComm_Req_Shutdown(unisys);
        situ->mycomm.talk_exit_seq++;
      }
    }
    else{
      UnionComm_Req_Shutdown(unisys);
      situ->mycomm.talk_exit_seq++;
    }
    break;
  default:  //全ての処理が完了
    break;
  }
}

//--------------------------------------------------------------
/**
 * ミニゲーム更新処理
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_MinigameUpdate(UNION_SYSTEM_PTR unisys)
{
  if(GFL_NET_GetConnectNum() > 1){
    //乱入希望NGの返事があるなら送信
    if(unisys->minigame_entry_req_answer_ng_bit > 0){
      if(UnionSend_MinigameEntryReqAnswerNG(unisys->minigame_entry_req_answer_ng_bit) == TRUE){
        unisys->minigame_entry_req_answer_ng_bit = 0;
      }
    }
    //乱入希望OKの返事があるなら送信
    if(unisys->minigame_entry_req_answer_ok_bit > 0){
      if(UnionSend_MinigameEntryReqAnswerOK(unisys->minigame_entry_req_answer_ok_bit) == TRUE){
        unisys->minigame_entry_req_answer_ok_bit = 0;
      }
    }
  }
  else{
    unisys->minigame_entry_req_answer_ng_bit = 0;
    unisys->minigame_entry_req_answer_ok_bit = 0;
  }
}

//--------------------------------------------------------------
/**
 * コロシアムの更新処理
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_Colosseum_Update(UNION_SYSTEM_PTR unisys)
{
  if(unisys->colosseum_sys == NULL || unisys->colosseum_sys->comm_ready == FALSE){
    return;
  }
  
  //PROCがフィールドのときのみ動作する処理
  if(Union_FieldCheck(unisys) == TRUE){
    if(UnionSubProc_IsExits(unisys) == FALSE){  //サブPROC実行中でない時に実行する処理
      //通信プレイヤーアクター管理システムが生成されているなら自分座標の転送を行う
      CommPlayer_Update(unisys->uniparent->gsys, unisys->colosseum_sys->cps);
      if(unisys->colosseum_sys->send_mine_pack_req == FALSE){
        if(CommPlayer_Mine_DataUpdate(
            unisys->colosseum_sys->cps, &unisys->colosseum_sys->send_mine_package) == TRUE){
          unisys->colosseum_sys->send_mine_pack_req = TRUE;
        }
      }
      if(unisys->colosseum_sys->send_mine_pack_req == TRUE){
        if(ColosseumSend_PosPackage(&unisys->colosseum_sys->send_mine_package) == TRUE){
          unisys->colosseum_sys->send_mine_pack_req = FALSE;
        }
      }
    }
    
    //通信プレイヤーの座標反映
    ColosseumTool_CommPlayerUpdate(unisys->colosseum_sys);
  }

  //立ち位置への返事リクエストが貯まっているなら送信を行う
  Colosseum_Parent_SendAnswerStandingPosition(unisys->colosseum_sys);
  //戦闘準備完了の返事リクエストが貯まっているなら送信を行う
  Colosseum_Parent_BattleReadyAnswer(unisys->colosseum_sys);
}

//--------------------------------------------------------------
/**
 * ビーコンサーチ
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionComm_BeaconSearch(UNION_SYSTEM_PTR unisys)
{
  int i;
  UNION_BEACON *bcon_buff;
  const u8 *mac_address_ptr;
  
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
  	bcon_buff = GFL_NET_GetBeaconData(i);
#if 0
  #ifdef PM_DEBUG //自分のキャラでNPCを全てAddするデバッグ機能
    if(bcon_buff == NULL && (GFL_UI_KEY_GetCont() & PAD_BUTTON_R)){
      int i;
      UNION_BEACON_PC *dest = unisys->receive_beacon;
      bcon_buff = &unisys->my_beacon;
      for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
        if(dest[i].beacon.data_valid != UNION_BEACON_VALID){
          GFL_STD_MemCopy(bcon_buff, &dest[i].beacon, sizeof(UNION_BEACON));
          dest[i].buffer_no = i;
          dest[i].update_flag = UNION_BEACON_RECEIVE_NEW;
          dest[i].life = UNION_CHAR_LIFE;
        }
      }
    }
  #endif
#endif
  	if(bcon_buff != NULL){
      mac_address_ptr = GFL_NET_GetBeaconMacAddress(i);
      if(mac_address_ptr != NULL){
        UnionBeacon_SetReceiveData(unisys, bcon_buff, mac_address_ptr);
      }
      GFL_NET_WLResetGFBss(i);  //ビーコンバッファクリア
  	}
  }
}

//--------------------------------------------------------------
/**
 * 受信ビーコンを受信用バッファにセットする
 *
 * @param   unisys		
 * @param   beacon		
 *
 * @retval  BOOL		TRUE:成功。　FALSE:失敗
 */
//--------------------------------------------------------------
static BOOL UnionBeacon_SetReceiveData(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address)
{
  int i;
  UNION_BEACON_PC *dest;
  
  dest = unisys->receive_beacon;
  
  if(beacon->data_valid != UNION_BEACON_VALID){
    return FALSE; //有効なビーコンではない
  }
  
  //既に受信済みのビーコンデータか確認(主メンバー)
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(dest[i].beacon.data_valid == UNION_BEACON_VALID
        && GFL_STD_MemComp(beacon_mac_address, dest[i].mac_address, 6) == 0){
      GFL_STD_MemCopy(beacon, &dest[i].beacon, sizeof(UNION_BEACON));
      //一度に同じマシンからの連続受信の可能性を考えnewの場合はフラグを変えない
      if(dest[i].update_flag != UNION_BEACON_RECEIVE_NEW){
        dest[i].update_flag = UNION_BEACON_RECEIVE_UPDATE;
      }
      dest[i].life = UNION_CHAR_LIFE;   //ビーコンの更新があったので寿命を元に戻す
//      OS_TPrintf("life回復 %d\n", OS_GetVBlankCount());
      return TRUE;
    }
  }
  
  //同じ人物が既に別グループに存在していないかチェック
  if(UnionBeacon_CheckSubMemberSameValid(unisys, beacon, beacon_mac_address) == FALSE){
    return FALSE;
  }
  
  //新規のビーコンデータなので空き領域にセットする
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(dest[i].beacon.data_valid != UNION_BEACON_VALID){
      GFL_STD_MemCopy(beacon, &dest[i].beacon, sizeof(UNION_BEACON));
      GFL_STD_MemCopy(beacon_mac_address, dest[i].mac_address, 6);
      dest[i].buffer_no = i;
      dest[i].update_flag = UNION_BEACON_RECEIVE_NEW;
      dest[i].life = UNION_CHAR_LIFE;
      return TRUE;
    }
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 受信ビーコンに対して既に同じビーコンの人物が存在していないか確認する
 * 同じ人物が既にいる場合は、その人物がいるグループを強制退出(ライフ1に設定)させる
 *
 * @param   unisys		
 * @param   beacon		新しく受信したビーコンへのポインタ
 *
 * @retval  BOOL		  TRUE:同じ人物はいなかった。　FALSE:同じ人物がいた
 */
//--------------------------------------------------------------
static BOOL UnionBeacon_CheckSubMemberSameValid(UNION_SYSTEM_PTR unisys, const UNION_BEACON *beacon, const u8 *beacon_mac_address)
{
  UNION_BEACON_PC *bpc;
  int i, member_no;
  
  bpc = unisys->receive_beacon;
  for(i = 0; i < UNION_RECEIVE_BEACON_MAX; i++){
    if(bpc->beacon.data_valid == UNION_BEACON_VALID){
      for(member_no = 0; member_no < UNION_CONNECT_PLAYER_NUM; member_no++){  //サブのみチェック
        if(bpc->beacon.party.member[member_no].occ == TRUE
            && GFL_STD_MemComp(beacon_mac_address, bpc->beacon.party.member[member_no].mac_address, 6) == 0){
          //同じ人物発見
          if(bpc->life > 0){
            bpc->life = 1;
          }
          return FALSE;
        }
      }
    }
    bpc++;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ビーコンデータ取得関数
 *
 * @param   pWork		UNION_SYSTEM_PTR
 *
 * @retval  void*		ビーコンデータへのポインタ
 */
//--------------------------------------------------------------
static void* UnionComm_GetBeaconData(void* pWork)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  UnionComm_SetBeaconParam(unisys, &unisys->my_beacon);
  return &unisys->my_beacon;
}

//==================================================================
/**
 * 送信するビーコンデータを作成する
 *
 * @param   beacon		ビーコンパラメータ代入先
 */
//==================================================================
static void UnionComm_SetBeaconParam(UNION_SYSTEM_PTR unisys, UNION_BEACON *beacon)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  GFL_STD_MemClear(beacon, sizeof(UNION_BEACON));
  
  if(situ->mycomm.connect_pc != NULL){
    if(GFL_NET_IsParentMachine() == TRUE){
      OS_GetMacAddress(beacon->connect_mac_address);
    }
    else{
      GFL_STD_MemCopy(situ->mycomm.connect_pc->mac_address, beacon->connect_mac_address, 6);
    }
  }
  else if(situ->mycomm.calling_pc != NULL){
    GFL_STD_MemCopy(situ->mycomm.calling_pc->mac_address, beacon->connect_mac_address, 6);
  }
  
  beacon->connect_num = GFL_NET_GetConnectNum();
  beacon->pm_version = PM_VERSION;
  beacon->language = PM_LANG;
  beacon->union_status = situ->union_status;
  beacon->appeal_no = situ->appeal_no;
  beacon->play_category = situ->play_category;
  
  MyStatus_CopyNameStrCode(unisys->uniparent->mystatus, beacon->name, PERSON_NAME_SIZE + EOM_SIZE);
  
  beacon->trainer_view = MyStatus_GetTrainerView( unisys->uniparent->mystatus );
  beacon->sex = MyStatus_GetMySex(unisys->uniparent->mystatus);
  beacon->trainer_id = MyStatus_GetID(unisys->uniparent->mystatus);
  
  beacon->chat_type = situ->chat_type;
  beacon->pmsdata = situ->chat_pmsdata;
  beacon->pms_rand = situ->chat_pms_rand;
  
  beacon->my_nation = MyStatus_GetMyNation(unisys->uniparent->mystatus);
  beacon->my_area = MyStatus_GetMyArea(unisys->uniparent->mystatus);

  beacon->party = situ->mycomm.party;
  
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
    WIFI_LIST* wifilist = GAMEDATA_GetWiFiList(gamedata);
    
    WifiList_GetMyFriendData( wifilist, &beacon->dwcfriend );
  }
  
  if(unisys->alloc.uniapp != NULL){
    beacon->entry_ng = UnionAppSystem_GetEntryNGFlag(unisys->alloc.uniapp);
  }
  else{
    beacon->entry_ng = FALSE;
  }
  
  //送信データ完成
  beacon->data_valid = UNION_BEACON_VALID;
  unisys->send_beacon_update = TRUE;
}

//--------------------------------------------------------------
/**
 * ビーコンデータサイズ取得関数
 *
 * @param   pWork		UNION_SYSTEM_PTR
 *
 * @retval  int		ビーコンデータサイズ
 */
//--------------------------------------------------------------
static int UnionComm_GetBeaconSize(void *pWork)
{
  return sizeof(UNION_BEACON);
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
static BOOL UnionComm_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork )
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  if(unisys->my_situation.mycomm.intrude_exe == TRUE){ //自分が乱入しようとしている状態
    if(GameServiceID1 == WB_NET_UNION || GameServiceID2 == WB_NET_UNION){
      return FALSE; //乱入で自分か相手がUNIONの場合はまだIDが切り替わっていない
    }
    return (GameServiceID1==GameServiceID2);
  }
  
  if(GameServiceID2 >= WB_NET_UNION && GameServiceID2 <= WB_NET_UNION_GURUGURU){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 通信不能なエラーが起こった場合呼ばれる 切断するしかない
 *
 * @param   pNet		ネットハンドル
 * @param   errNo		エラー番号
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void UnionComm_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	UNION_SYSTEM_PTR unisys = pWork;

//  NetErr_ErrorSet();
}

//--------------------------------------------------------------
/**
 * 通信切断時に呼ばれる関数(終了時
 *
 * @param   pWork		UNION_SYSTEM_PTR
 */
//--------------------------------------------------------------
static void UnionComm_DisconnectCallBack(void* pWork)
{
  OS_TPrintf("通信切断コールバック UnionComm_DisconnectCallBack\n");
}

//==================================================================
/**
 * ビーコン受信バッファAllクリア
 *
 * @param   unisys		
 */
//==================================================================
void UnionBeacon_ClearAllReceiveData(UNION_SYSTEM_PTR unisys)
{
  OS_TPrintf("ビーコン受信バッファ全クリア\n");
  GFL_STD_MemClear(unisys->receive_beacon, sizeof(UNION_BEACON_PC) * UNION_RECEIVE_BEACON_MAX);
}

//==================================================================
/**
 * 各シーケンスの途中でAllocされるものを一括解放
 *
 * @param   unisys		
 */
//==================================================================
void UnionAlloc_AllFree(UNION_SYSTEM_PTR unisys)
{
#if 0//トレーナーカードのParentWorkはトレーナーカードのProc内で解放されるのでNULLでなくても
     //ここでは解放しない
  GFL_HEAP_FreeMemory(unisys->alloc.card_param);
#endif
  unisys->alloc.card_param = NULL;

  if(unisys->alloc.my_card != NULL){
    GFL_HEAP_FreeMemory(unisys->alloc.my_card);
    unisys->alloc.my_card = NULL;
  }
  if(unisys->alloc.target_card != NULL){
    GFL_HEAP_FreeMemory(unisys->alloc.target_card);
    unisys->alloc.target_card = NULL;
  }

  if(unisys->alloc.bbox_party != NULL){
    GFL_HEAP_FreeMemory(unisys->alloc.bbox_party);
    unisys->alloc.bbox_party = NULL;
  }
  
  if(unisys->alloc.psl != NULL){
    //これを使用しているシーケンスはアクター生成などもしているので、単純にこれだけFreeは危険。
    //シーケンス毎、強制終了にはしないようにしているのでここには来ないはず
    GFL_HEAP_FreeMemory(unisys->alloc.psl);
    unisys->alloc.psl = NULL;
  }

#if 0 //unisysの作成時に生成、破棄時に削除しているのでここではしない
  if(unisys->alloc.regulation != NULL){
    GFL_HEAP_FreeMemory(unisys->alloc.regulation);
    unisys->alloc.regulation = NULL;
  }
#endif

  if(unisys->alloc.uniapp != NULL){
    //これを使用しているシーケンスは単純にFreeは危険。
    //シーケンス毎、強制終了にはしないようにしているのでここには来ないはず
    UnionAppSystem_FreeAppWork(unisys->alloc.uniapp);
    unisys->alloc.uniapp = NULL;
  }

  if(unisys->colosseum_sys != NULL){
    Colosseum_ExitSystem(unisys->colosseum_sys, unisys->uniparent->gsys);
    unisys->colosseum_sys = NULL;
  }
}

//==============================================================================
//  通信リクエスト
//==============================================================================
//==================================================================
/**
 * 自分シチュエーションのセット
 *
 * @param   unisys		
 * @param   index		  シチュエーションパラメータのデータIndex
 * @param   work		  各Indexで必要とするワークへのポインタ
 */
//==================================================================
void UnionMySituation_SetParam(UNION_SYSTEM_PTR unisys, UNION_MYSITU_PARAM_IDX index, void *work)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  switch(index){
  case UNION_MYSITU_PARAM_IDX_TALK_PC:
    {
      UNION_BEACON_PC *talk_pc = work;
      situ->mycomm.talk_pc = talk_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_CALLING_PC:
    {
      UNION_BEACON_PC *calling_pc = work;
      situ->mycomm.calling_pc = calling_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_ANSWER_PC:
    {
      UNION_BEACON_PC *answer_pc = work;
      situ->mycomm.answer_pc = answer_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_CONNECT_PC:
    {
      UNION_BEACON_PC *connect_pc = work;
      situ->mycomm.connect_pc = connect_pc;
    }
    break;
  case UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY:
    situ->play_category = (u32)work;
    break;
  }
}

//==================================================================
/**
 * 自分シチュエーションのクリア
 *
 * @param   unisys		
 */
//==================================================================
void UnionMySituation_Clear(UNION_SYSTEM_PTR unisys)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  GFL_STD_MemClear(situ, sizeof(UNION_MY_SITUATION));
  situ->chat_pms_rand = GFUser_GetPublicRand(0xffff);
  situ->chat_type = UNION_CHAT_TYPE_UNION_IN;
  situ->union_status = UNION_STATUS_ENTER;
  UnionMyComm_Init(unisys, &situ->mycomm);
}

//==================================================================
/**
 * UNION_MY_COMM構造体の中からメニューの返事関連の受信バッファのみを初期化
 *
 * @param   mycomm		
 */
//==================================================================
void UnionMyComm_InitMenuParam(UNION_MY_COMM *mycomm)
{
  mycomm->mainmenu_select = UNION_MENU_SELECT_NULL;
  mycomm->submenu_select = UNION_MENU_SELECT_NULL;
  mycomm->mainmenu_yesno_result = 0xff;
}

//==================================================================
/**
 * UNION_MY_COMM構造体を初期化
 *
 * @param   mycomm		
 */
//==================================================================
void UnionMyComm_Init(UNION_SYSTEM_PTR unisys, UNION_MY_COMM *mycomm)
{
  GFL_STD_MemClear(mycomm, sizeof(UNION_MY_COMM));
  UnionMyComm_InitMenuParam(mycomm);
  
  //UNION_MEMBERの自分分を埋める
  {
    UNION_MEMBER *member = &mycomm->party.member[0];
    
    OS_GetMacAddress(member->mac_address);
    member->trainer_view = MyStatus_GetTrainerView( unisys->uniparent->mystatus );
    member->sex = MyStatus_GetMySex(unisys->uniparent->mystatus);
    member->occ = TRUE;
  }
  
  mycomm->talk_obj_id = UNION_MYCOMM_NOT_TALK_OBJ_ID;
}

//==================================================================
/**
 * ユニオンパーティにプレイヤーを登録する　※パラメータ指定
 *
 * @param   mycomm		      
 * @param   mac_address		  
 * @param   trainer_view		
 * @param   sex		
 *
 * @retval  BOOL		
 */
//==================================================================
BOOL UnionMyComm_PartyAddParam(UNION_MY_COMM *mycomm, const u8 *mac_address, u8 trainer_view, u8 sex)
{
  int i;
  UNION_MEMBER *member;
  
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    member = &mycomm->party.member[i];
    if(member->occ == TRUE && GFL_STD_MemComp(mac_address, member->mac_address, 6) == 0){
      return i;   //既に登録済み
    }
  }
  
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    member = &mycomm->party.member[i];
    if(member->occ == FALSE){
      GFL_STD_MemCopy(mac_address, member->mac_address, 6);
      member->trainer_view = trainer_view;
      member->sex = sex;
      member->occ = TRUE;
      return i;
    }
  }
  
  GF_ASSERT(0); //既にメンバーが埋まっている
  return 0;
}

//==================================================================
/**
 * ユニオンパーティにプレイヤーを登録する   ※UNION_BEACON_PC指定
 *
 * @param   unisys		
 * @param   pc		    登録対象のPCへのポインタ
 *
 * @retval  TRUE:登録成功　　FALSE:失敗
 */
//==================================================================
BOOL UnionMyComm_PartyAdd(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc)
{
  return UnionMyComm_PartyAddParam(mycomm, pc->mac_address, pc->beacon.trainer_view, pc->beacon.sex);
}

//==================================================================
/**
 * ユニオンパーティからプレイヤーを削除する   ※PARAM指定
 *
 * @param   unisys		
 * @param   pc		    削除対象のPCへのポインタ
 */
//==================================================================
void UnionMyComm_PartyDelParam(UNION_MY_COMM *mycomm, const u8 *mac_address)
{
  int i;
  UNION_MEMBER *member;
  
  for(i = 0; i < UNION_CONNECT_PLAYER_NUM; i++){
    member = &mycomm->party.member[i];
    if(member->occ == TRUE && GFL_STD_MemComp(mac_address, member->mac_address, 6) == 0){
      GFL_STD_MemClear(member, sizeof(UNION_MEMBER));
      OS_TPrintf("メンバー削除 %d\n", i);
      return;
    }
  }
}

//==================================================================
/**
 * ユニオンパーティからプレイヤーを削除する   ※UNION_BEACON_PC指定
 *
 * @param   unisys		
 * @param   pc		    削除対象のPCへのポインタ
 */
//==================================================================
void UnionMyComm_PartyDel(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc)
{
  UnionMyComm_PartyDelParam(mycomm, pc->mac_address);
}

//--------------------------------------------------------------
/**
 * チャットログ管理データを初期化
 *
 * @param   log		チャットログ管理データへのポインタ
 */
//--------------------------------------------------------------
static void UnionChat_InitLog(UNION_CHAT_LOG *log)
{
  int i;
  
  GFL_STD_MemClear(log, sizeof(UNION_CHAT_LOG));
  for(i = 0; i < UNION_CHAT_LOG_MAX; i++){
    GFL_STD_MemFill(log->chat[i].mac_address, 0xff, 6);
  }
  log->chat_log_count = -1;
  log->chat_view_no = -1;
  log->old_chat_view_no = -1;
}

//==================================================================
/**
 * 自分のチャットデータをセットする
 *
 * @param   unisys		
 * @param   pmsdata		
 */
//==================================================================
void UnionChat_SetMyPmsData(UNION_SYSTEM_PTR unisys, const PMS_DATA *pmsdata)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  situ->chat_pmsdata = *pmsdata;
  situ->chat_type = UNION_CHAT_TYPE_NORMAL;
  situ->chat_pms_rand++;
  situ->chat_upload = TRUE;
}

//==================================================================
/**
 * 自分のチャットデータ(募集メッセージ)をセットする
 *
 * @param   unisys		
 * @param   chat_type		
 */
//==================================================================
void UnionChat_SetRecruit(UNION_SYSTEM_PTR unisys, UNION_CHAT_TYPE chat_type)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  GF_ASSERT(chat_type != UNION_CHAT_TYPE_NORMAL);
  situ->chat_type = chat_type;
  situ->chat_pms_rand++;
  situ->chat_upload = TRUE;
}

//==================================================================
/**
 * 乱入可のゲームは別のゲームが繋がらないように専用のGSIDに変更する
 *
 * @param   play_category		
 */
//==================================================================
void Union_ChangePlayCategoryGSID(u32 play_category)
{
  switch(play_category){
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
    GFL_NET_ChangeGameService(WB_NET_UNION_BATTLE_MULTI_FREE_SHOOTER, UNION_CONNECT_PLAYER_NUM);
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
    GFL_NET_ChangeGameService(WB_NET_UNION_BATTLE_MULTI_FREE, UNION_CONNECT_PLAYER_NUM);
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    GFL_NET_ChangeGameService(WB_NET_UNION_BATTLE_MULTI_FLAT_SHOOTER, UNION_CONNECT_PLAYER_NUM);
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
    GFL_NET_ChangeGameService(WB_NET_UNION_BATTLE_MULTI_FLAT, UNION_CONNECT_PLAYER_NUM);
    break;

  case UNION_PLAY_CATEGORY_PICTURE:
    GFL_NET_ChangeGameService(WB_NET_UNION_PICTURE, UNION_CONNECT_PLAYER_NUM);
    break;
  case UNION_PLAY_CATEGORY_GURUGURU:
    GFL_NET_ChangeGameService(WB_NET_UNION_GURUGURU, UNION_CONNECT_PLAYER_NUM);
    break;
  }
}
