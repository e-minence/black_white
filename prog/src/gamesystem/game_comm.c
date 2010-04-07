//==============================================================================
/**
 * @file    game_comm.c
 * @brief   ゲーム通信管理
 * @author  matsuda
 * @date    2009.05.12(火)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/game_comm.h"
#include "buflen.h"

#include "field/game_beacon_search.h"
#include "message.naix"
#include "msg/msg_invasion.h"
#include "net_app/union/union_main.h"
#include "net/wih_dwc.h"
#include "field/fieldmap_proc.h"
#include "field/intrude_comm.h"
#include "system/net_err.h"
#include "system/main.h"
#include "field/zonedata.h"
#include "musical/musical_comm_field.h"
#include "field/intrude_common.h"


//==============================================================================
//  定数定義
//==============================================================================
///シーケンス番号
typedef enum{
  GCSSEQ_INIT,
  GCSSEQ_INIT_WAIT,
  GCSSEQ_UPDATE,
  GCSSEQ_EXIT,
  GCSSEQ_EXIT_WAIT,
  GCSSEQ_FINISH,
}GCSSEQ;

///通信最大人数
#define COMM_PLAYER_MAX   (FIELD_COMM_MEMBER_MAX)

///インフォメーションメッセージキュー数
#define COMM_INFO_QUE_MAX     (6)


//==============================================================================
//  構造体定義
//==============================================================================
//--------------------------------------------------------------
//  ステータス
//--------------------------------------------------------------
///各プレイヤーの通信ステータス
typedef struct{
  u16 zone_id;        ///<現在いるゾーンID
  u16 old_zone_id;    ///<前までいたゾーンID
  u16 padding;        //
  u8 invasion_netid;  ///<侵入先ROM
  u8 old_invasion_netid;  ///<前までいた侵入先ROM
}GAME_COMM_PLAYER_STATUS;

//--------------------------------------------------------------
//  インフォメーション
//--------------------------------------------------------------
///インフォメーションのメッセージキュー構造体
typedef struct{
  u16 message_id;
  u8 net_id;
  u8 use:1;           ///<TRUE:使用中 FALSE:未使用
  u8 sex:1;
  u8 target_sex:1;
  u8      :5;
  STRBUF *name_strbuf;         ///<メッセージの主プレイヤー名
  STRBUF *target_name_strbuf;  ///<メッセージの主プレイヤーのターゲットにされたプレイヤー名
}GAME_COMM_INFO_QUE;

///インフォメーション構造体
typedef struct{
  GAME_COMM_INFO_QUE msg_que[COMM_INFO_QUE_MAX];
  u8 now_pos;       ///<貯まっているキューの開始位置
  u8 space_pos;     ///<空きキューの開始位置
  u8 padding[2];
}GAME_COMM_INFO;

//--------------------------------------------------------------
//  システム
//--------------------------------------------------------------
///ゲーム実行用ワーク構造体
typedef struct{
  int func_seq;
  u8 seq;
  u8 padding[3];
}GAME_COMM_SUB_WORK;

///ゲーム通信管理ワーク構造体
typedef struct _GAME_COMM_SYS{
  GAMEDATA *gamedata;                 ///<ゲームデータへのポインタ
  GAME_COMM_STATUS comm_status;         ///<通信ステータス
  GAME_COMM_NO game_comm_no;
  GAME_COMM_NO reserve_comm_game_no;   ///<予約通信ゲーム番号
  GAME_COMM_SUB_WORK sub_work;
  void *parent_work;                  ///<呼び出し時に引き渡すポインタ
  void *app_work;                     ///<各アプリケーションが確保したワークのポインタ
  void *reserve_parent_work;          ///<呼び出し時に引き渡すポインタ
  GAME_COMM_PLAYER_STATUS player_status[COMM_PLAYER_MAX]; ///<通信プレイヤーステータス
  GAME_COMM_INFO info;                ///<インフォメーションワーク
}GAME_COMM_SYS;

///ゲーム通信関数データ構造体
typedef struct{
  void *(*init_func)(int *seq, void *pwk);                         ///<初期化処理
  BOOL (*init_wait_func)(int *seq, void *pwk, void *app_work);     ///<初期化完了待ち
  void (*update_func)(int *seq, void *pwk, void *app_work);        ///<更新処理
  BOOL (*exit_func)(int *seq, void *pwk, void *app_work);          ///<終了処理
  BOOL (*exit_wait_func)(int *seq, void *pwk, void *app_work);     ///<終了完了待ち
  
  void (*field_create)(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork);  ///<Field作成時に呼ばれるコールバック
  void (*field_delete)(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork);  ///<Field削除時に呼ばれるコールバック
}GAME_FUNC_TBL;


//==============================================================================
//  データ
//==============================================================================
///通信ゲーム管理テーブル
static const GAME_FUNC_TBL GameFuncTbl[] = {
  //GAME_COMM_NO_NULL    1origin化
  {
    NULL,       //init
    NULL,       //init_wait
    NULL,       //update
    NULL,       //exit
    NULL,       //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
  
  //GAME_COMM_NO_FIELD_BEACON_SEARCH
  {
    GameBeacon_Init,       //init
    GameBeacon_InitWait,   //init_wait
    GameBeacon_Update,     //update
    GameBeacon_Exit,       //exit
    GameBeacon_ExitWait,   //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
  //GAME_COMM_NO_INVASION
  {
    IntrudeComm_InitCommSystem,       //init
    IntrudeComm_InitCommSystemWait,   //init_wait
    IntrudeComm_UpdateSystem,         //update
    IntrudeComm_TermCommSystem,       //exit
    IntrudeComm_TermCommSystemWait,   //exit_wait
    IntrudeComm_FieldCreate,          //field_create
    IntrudeComm_FieldDelete,          //field_delete
  },
  //GAME_COMM_NO_UNION
  {
    UnionComm_Init,       //init
    UnionComm_InitWait,   //init_wait
    UnionComm_Update,     //update
    UnionComm_Exit,       //exit
    UnionComm_ExitWait,   //exit_wait
    UnionMain_Callback_FieldCreate,       //field_create
    UnionMain_Callback_FieldDelete,       //field_delete
  },
  {
    MUS_COMM_InitGameComm,       //init
    NULL,       //init_wait
    MUS_COMM_UpdateGameComm,     //update
    MUS_COMM_ExitGameComm,       //exit
    MUS_COMM_ExitWaitGameComm,   //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
  //GAME_COMM_NO_DEBUG_SCANONLY
  {
    GameBeacon_Init,       //init
    GameBeacon_InitWait,   //init_wait
    GameBeacon_Update,     //update
    GameBeacon_Exit,       //exit
    GameBeacon_ExitWait,   //exit_wait
    NULL,       //field_create
    NULL,       //field_delete
  },
};

//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void GameCommSub_SeqSet(GAME_COMM_SUB_WORK *sub_work, u8 seq);
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, int target_net_id, u32 message_id);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ゲーム通信管理ワーク確保
 *
 * @param   heap_id		
 * @param   gamedata     ゲームデータへのポインタ
 *
 * @retval  GAME_COMM_SYS_PTR		確保したワークへのポインタ
 */
//==================================================================
GAME_COMM_SYS_PTR GameCommSys_Alloc(HEAPID heap_id, GAMEDATA *gamedata)
{
  GAME_COMM_SYS_PTR gcsp;
  GAME_COMM_INFO *comm_info;
  const MYSTATUS *myst;
  int i;
  
  gcsp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAME_COMM_SYS));
  gcsp->gamedata = gamedata;
  
  myst = GAMEDATA_GetMyStatus(gamedata);
  comm_info = &gcsp->info;
  for(i = 0; i < COMM_INFO_QUE_MAX; i++){
    comm_info->msg_que[i].name_strbuf = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heap_id);
    comm_info->msg_que[i].target_name_strbuf = GFL_STR_CreateBuffer(PERSON_NAME_SIZE + EOM_SIZE, heap_id);
    //吹っ飛ばないように自分のデータで埋めておく
    MyStatus_CopyNameString(myst, comm_info->msg_que[i].name_strbuf);
    MyStatus_CopyNameString(myst, comm_info->msg_que[i].target_name_strbuf);
  }
  return gcsp;
}

//==================================================================
/**
 * ゲーム通信管理ワーク解放
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Free(GAME_COMM_SYS_PTR gcsp)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  int i;
  
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL && gcsp->app_work == NULL);
  
  for(i = 0; i < COMM_INFO_QUE_MAX; i++){
    GFL_STR_DeleteBuffer(comm_info->msg_que[i].name_strbuf);
    GFL_STR_DeleteBuffer(comm_info->msg_que[i].target_name_strbuf);
  }
  GFL_HEAP_FreeMemory(gcsp);
}

//==================================================================
/**
 * ゲーム通信メイン処理
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Main(GAME_COMM_SYS_PTR gcsp)
{
  GAME_COMM_SUB_WORK *sub_work;
  const GAME_FUNC_TBL *func_tbl;
  
  if(gcsp == NULL || gcsp->game_comm_no == GAME_COMM_NO_NULL){
    return;
  }
  
  sub_work = &gcsp->sub_work;
  func_tbl = &GameFuncTbl[gcsp->game_comm_no];
  
  switch(sub_work->seq){
  case GCSSEQ_INIT:
    if(func_tbl->init_func != NULL){
      gcsp->app_work = func_tbl->init_func(&sub_work->func_seq, gcsp->parent_work);
    }
    GameCommSub_SeqSet(sub_work, GCSSEQ_INIT_WAIT);
    break;
  case GCSSEQ_INIT_WAIT:
    if(func_tbl->init_wait_func == NULL || func_tbl->init_wait_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
      //※check　現状ワイヤレスしかないので、とりあえずここでWIRELESSを代入
      gcsp->comm_status = GAME_COMM_STATUS_WIRELESS;
      GameCommSub_SeqSet(sub_work, GCSSEQ_UPDATE);
    }
    break;
  case GCSSEQ_UPDATE:
    func_tbl->update_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work);
    break;
  case GCSSEQ_EXIT:
    if(func_tbl->exit_func != NULL){
      if(func_tbl->exit_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
        GameCommSub_SeqSet(sub_work, GCSSEQ_EXIT_WAIT);
      }
    }
    else{
      GameCommSub_SeqSet(sub_work, GCSSEQ_EXIT_WAIT);
    }
    break;
  case GCSSEQ_EXIT_WAIT:
    if(func_tbl->exit_wait_func == NULL || func_tbl->exit_wait_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
      GameCommSub_SeqSet(sub_work, GCSSEQ_FINISH);
    }
    else{
      break;
    }
    //break;
  case GCSSEQ_FINISH:
    if(NetErr_App_CheckError()){
      return; //エラー発生による強制終了だった場合はエラー画面が表示されるまで待つ
    }
    gcsp->app_work = NULL;
    gcsp->game_comm_no = GAME_COMM_NO_NULL;
    gcsp->comm_status = GAME_COMM_STATUS_NULL;
    if(gcsp->reserve_comm_game_no != GAME_COMM_NO_NULL){
      GameCommSys_Boot(gcsp, gcsp->reserve_comm_game_no, gcsp->reserve_parent_work);
      gcsp->reserve_comm_game_no = GAME_COMM_NO_NULL;
      gcsp->reserve_parent_work = NULL;
    }
    break;
  }
}

//==================================================================
/**
 * フィールドマップ作成時に実行するコールバック
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Callback_FieldCreate(GAME_COMM_SYS_PTR gcsp, void *fieldWork)
{
  if(gcsp->sub_work.seq == GCSSEQ_FINISH){
    return;
  }
  
  if(GameFuncTbl[gcsp->game_comm_no].field_create != NULL){
    GameFuncTbl[gcsp->game_comm_no].field_create(gcsp->parent_work, gcsp->app_work, fieldWork);
  }
}

//==================================================================
/**
 * フィールドマップ削除時に呼ばれるコールバック
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommSys_Callback_FieldDelete(GAME_COMM_SYS_PTR gcsp, void *fieldWork)
{
  if(gcsp->sub_work.seq == GCSSEQ_FINISH){
    return;
  }

  if(GameFuncTbl[gcsp->game_comm_no].field_delete != NULL){
    GameFuncTbl[gcsp->game_comm_no].field_delete(gcsp->parent_work, gcsp->app_work, fieldWork);
  }
}

//==================================================================
/**
 * ゲーム通信起動
 *
 * @param   gcsp		        ゲーム通信管理ワークへのポインタ
 * @param   game_comm_no		起動する通信ゲーム番号(GAME_COMM_NO_???)
 * @param   parent_work     引き渡すポインタ
 */
//==================================================================
void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work)
{
  GF_ASSERT(gcsp->game_comm_no == GAME_COMM_NO_NULL);
  
  gcsp->game_comm_no = game_comm_no;
  gcsp->parent_work = parent_work;
  GFL_STD_MemClear(&gcsp->sub_work, sizeof(GAME_COMM_SUB_WORK));
}

//==================================================================
/**
 * 終了リクエスト
 *
 * @param   gcsp		
 *
 * 終了の確認はGameCommSys_BootCheck関数を使用してください
 */
//==================================================================
void GameCommSys_ExitReq(GAME_COMM_SYS_PTR gcsp)
{
  GF_ASSERT(gcsp->sub_work.seq == GCSSEQ_UPDATE);
  
  OS_TPrintf("GameCommSys_ExitReq\n");
  GameCommSub_SeqSet(&gcsp->sub_work, GCSSEQ_EXIT);
}

//==================================================================
/**
 * 通信ゲーム切り替えリクエスト
 *
 * @param   gcsp		
 * @param   game_comm_no		切り替え後、起動する通信ゲーム番号(GAME_COMM_NO_???)
 * @param   parent_work     引き渡すポインタ
 */
//==================================================================
void GameCommSys_ChangeReq(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work)
{
  GF_ASSERT(gcsp->sub_work.seq >= GCSSEQ_UPDATE);
  
  if(gcsp->sub_work.seq == GCSSEQ_UPDATE){
    GameCommSub_SeqSet(&gcsp->sub_work, GCSSEQ_EXIT);
  }
  gcsp->reserve_comm_game_no = game_comm_no;
  gcsp->reserve_parent_work = parent_work;
}

//==================================================================
/**
 * 通信ステータスを取得
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_STATUS	通信ステータス
 */
//==================================================================
GAME_COMM_STATUS GameCommSys_GetCommStatus(GAME_COMM_SYS_PTR gcsp)
{
  const GFLNetInitializeStruct *aNetStruct;
  
  if(GFL_NET_IsInit() == FALSE){
    return GAME_COMM_STATUS_NULL;
  }
  
  aNetStruct = GFL_NET_GetNETInitStruct();
	switch(aNetStruct->bNetType){
  case GFL_NET_TYPE_WIRELESS:		///<ワイヤレス通信
  case GFL_NET_TYPE_IRC_WIRELESS:	///<赤外線通信でマッチング後、ワイヤレス通信へ移行
    gcsp->comm_status = GAME_COMM_STATUS_WIRELESS;
    break;

  case GFL_NET_TYPE_WIRELESS_SCANONLY:	///<ワイヤレス通信(スキャン専用・電源ランプ非点滅)
    {
      gcsp->comm_status = WIH_DWC_GetAllBeaconType(NULL);  //@todo
    }
    break;
  case GFL_NET_TYPE_WIFI:			///<WIFI通信
  case GFL_NET_TYPE_WIFI_LOBBY:	///<WIFI広場通信
    gcsp->comm_status = GAME_COMM_STATUS_WIFI;
    break;
  case GFL_NET_TYPE_IRC:			///<赤外線通信
    gcsp->comm_status = GAME_COMM_STATUS_IRC;
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return gcsp->comm_status;
}

//==================================================================
/**
 * 起動確認
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_NO  起動している通信ゲーム番号(何も起動していなければGAME_COMM_NO_NULL)
 */
//==================================================================
GAME_COMM_NO GameCommSys_BootCheck(GAME_COMM_SYS_PTR gcsp)
{
  if(gcsp->game_comm_no != GAME_COMM_NO_NULL){
    return gcsp->game_comm_no;
  }
  if(gcsp->reserve_comm_game_no != GAME_COMM_NO_NULL){
    return gcsp->reserve_comm_game_no;
  }
  return GAME_COMM_NO_NULL;
}

//==================================================================
/**
 * 通信ゲーム番号を取得する
 *
 * @param   gcsp		
 *
 * @retval  GAME_COMM_NO		
 */
//==================================================================
GAME_COMM_NO GameCommSys_GetCommGameNo(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->game_comm_no;
}

//==================================================================
/**
 * 通信ゲームがシステムの待ち状態か調べる
 *
 * @param   gcsp		
 *
 * @retval  		TRUE:初期化待ち、終了待ち、等のシステム系の待ち状態になっている
 * @retval      FALSE:通常の更新処理を実行している or GAME_COMM_NO_NULL 状態
 */
//==================================================================
BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp)
{
  if(gcsp->game_comm_no != GAME_COMM_NO_NULL && gcsp->sub_work.seq != GCSSEQ_UPDATE){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * アプリケーションワークのポインタを取得する
 *
 * @param   gcsp		  
 *
 * @retval  void *		アプリケーションワークへのポインタ
 */
//==================================================================
void *GameCommSys_GetAppWork(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->app_work;
}

//==================================================================
/**
 * GAMEDATAへのポインタを取得する
 *
 * @param   gcsp		
 *
 * @retval  GAMEDATA *		ゲームデータへのポインタ
 */
//==================================================================
GAMEDATA * GameCommSys_GetGameData(GAME_COMM_SYS_PTR gcsp)
{
  return gcsp->gamedata;
}

//--------------------------------------------------------------
/**
 * サブワークのシーケンス番号をセットする
 *
 * @param   sub_work		
 * @param   seq		
 */
//--------------------------------------------------------------
static void GameCommSub_SeqSet(GAME_COMM_SUB_WORK *sub_work, u8 seq)
{
  sub_work->seq = seq;
  sub_work->func_seq = 0;
}


//==============================================================================
//  プレイヤーステータス
//==============================================================================
//==================================================================
/**
 * 通信プレイヤーステータスをセットする
 *
 * @param   gcsp		  
 * @param   comm_net_id		    このステータス対象のnetID
 * @param   zone_id		        comm_net_idのプレイヤーが今いるゾーンID
 * @param   invasion_netid		comm_net_idのプレイヤーが侵入しているROMのnetID
 */
//==================================================================
void GameCommStatus_SetPlayerStatus(GAME_COMM_SYS_PTR gcsp, int comm_net_id, ZONEID zone_id, u8 invasion_netid)
{
  GAME_COMM_PLAYER_STATUS *player_status = &gcsp->player_status[comm_net_id];
  ZONEID old, now;
  
  old = player_status->old_zone_id;
  now = player_status->zone_id;
  
  if(player_status->zone_id == zone_id && player_status->invasion_netid == invasion_netid){
    return;
  }
  
  //メッセージ作成
  if(player_status->zone_id != zone_id){
    player_status->old_zone_id = player_status->zone_id;
    player_status->zone_id = zone_id;
  }
  if(player_status->invasion_netid != invasion_netid){
    player_status->old_invasion_netid = player_status->invasion_netid;
    player_status->invasion_netid = invasion_netid;
    GameCommInfo_MessageEntry_IntrudePalace(gcsp, comm_net_id, invasion_netid);
  }
}

//==================================================================
/**
 * 通信プレイヤーステータスから侵入先NetIDを取得
 *
 * @param   gcsp		
 * @param   comm_net_id		
 *
 * @retval  u8		ミッション番号
 */
//==================================================================
u8 GameCommStatus_GetPlayerStatus_InvasionNetID(GAME_COMM_SYS_PTR gcsp, int comm_net_id)
{
  GAME_COMM_PLAYER_STATUS *player_status = &gcsp->player_status[comm_net_id];
  return player_status->invasion_netid;
}


//==============================================================================
//  インフォメーションメッセージ
//==============================================================================
//--------------------------------------------------------------
/**
 * インフォメーションメッセージをキューに貯める
 *
 * @param   gcsp		      
 * @param   comm_net_id		  メッセージの主となる人物のNetID
 * @param   target_net_id		メッセージの主のターゲットとなる人物のNetID
 * @param   message_id		
 */
//--------------------------------------------------------------
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, int target_net_id, u32 message_id)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  GAME_COMM_INFO_QUE *que;
  const MYSTATUS *myst, *target_myst;

  if(comm_net_id == GAMEDATA_GetIntrudeMyID(gcsp->gamedata)){
    myst = GAMEDATA_GetMyStatus(gcsp->gamedata);
  }
  else{
    myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, comm_net_id);
  }
  if(target_net_id == GAMEDATA_GetIntrudeMyID(gcsp->gamedata)){
    target_myst = GAMEDATA_GetMyStatus(gcsp->gamedata);
  }
  else{
    target_myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, target_net_id);
  }
  
  //※check　暫定処理　通信確立＝必ず名前を交換しあっている状況ではないため
  if(MyStatus_CheckNameClear(myst) == TRUE || MyStatus_CheckNameClear(target_myst) == TRUE){
    return;
  }

  if(comm_info->msg_que[comm_info->space_pos].use == TRUE){
    //メッセージの優先順位が決まっていない今は上書き(古いキューは最新のキューで上書きされる
    que = &comm_info->msg_que[comm_info->space_pos];
    comm_info->now_pos++;   //最古のキューから表示されるようにnow_posの位置も後ろにずらす
  }
  else{ //空いているキューなのでそのまま代入
    que = &comm_info->msg_que[comm_info->space_pos];
  }

  que->net_id = comm_net_id;
  que->message_id = message_id;
  que->use = TRUE;
  
  MyStatus_CopyNameString(myst, que->name_strbuf);
  que->sex = MyStatus_GetMySex(myst);
  MyStatus_CopyNameString(target_myst, que->target_name_strbuf);
  que->target_sex = MyStatus_GetMySex(target_myst);
  
  comm_info->space_pos++;
  if(comm_info->space_pos >= COMM_INFO_QUE_MAX){
    comm_info->space_pos = 0;
  }
  if(comm_info->now_pos >= COMM_INFO_QUE_MAX){
    comm_info->now_pos = 0;
  }
}

//==================================================================
/**
 * インフォメーションメッセージを取得する
 *
 * @param   gcsp		    
 * @param   dest_msg		メッセージ代入先
 *
 * @retval  BOOL		TRUE:データあり
 * @retval  BOOL		FALSE:データなし
 */
//==================================================================
BOOL GameCommInfo_GetMessage(GAME_COMM_SYS_PTR gcsp, GAME_COMM_INFO_MESSAGE *dest_msg)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  GAME_COMM_INFO_QUE *que = &comm_info->msg_que[comm_info->now_pos];
  int i;
  
  if(que->use == FALSE){
    return FALSE;
  }
  
  //キューをGAME_COMM_INFO_MESSAGE型に変換して代入
  GFL_STD_MemClear(dest_msg, sizeof(GAME_COMM_INFO_MESSAGE));

  dest_msg->name[0] = que->name_strbuf;
  dest_msg->wordset_no[0] = 0;
  dest_msg->wordset_sex[0] = que->sex;
  dest_msg->name[1] = que->target_name_strbuf;
  dest_msg->wordset_no[1] = 1;
  dest_msg->wordset_sex[1] = que->target_sex;

  dest_msg->message_id = que->message_id;
  
  //キューの参照位置を進める
  que->use = FALSE;
  comm_info->now_pos++;
  if(comm_info->now_pos >= COMM_INFO_QUE_MAX){
    comm_info->now_pos = 0;
  }
  
  return TRUE;
}

//==================================================================
/**
 * インフォメーションメッセージ登録：「xxxx」は「xxxx」のパレスからパワーをもらった
 *
 * @param   gcsp		
 * @param   player_netid		主のNetID
 * @param   target_netid		ターゲットのNetID
 */
//==================================================================
void GameCommInfo_MessageEntry_GetPower(GAME_COMM_SYS_PTR gcsp, int player_netid, int target_netid)
{
  u16 msg_id = (player_netid == target_netid) ? msg_invasion_info_010 : msg_invasion_info_009;
  GameCommInfo_SetQue(gcsp, player_netid, target_netid, msg_id);
}

//==================================================================
/**
 * インフォメーションメッセージ登録：「xxxx」が「xxxx」のパレスに侵入しました
 *
 * @param   gcsp		
 * @param   player_netid		主のNetID
 * @param   target_netid		ターゲットのNetID
 */
//==================================================================
void GameCommInfo_MessageEntry_IntrudePalace(GAME_COMM_SYS_PTR gcsp, int player_netid, int target_netid)
{
  u16 msg_id = (player_netid == target_netid) ? msg_invasion_info_001 : msg_invasion_info_000;
  GameCommInfo_SetQue(gcsp, player_netid, target_netid, msg_id);
}

//==================================================================
/**
 * インフォメーションメッセージ登録：「xxxx」のパレスとつながりました
 *
 * @param   gcsp		
 * @param   player_netid		主のNetID
 */
//==================================================================
void GameCommInfo_MessageEntry_PalaceConnect(GAME_COMM_SYS_PTR gcsp, int player_netid)
{
  GameCommInfo_SetQue(gcsp, player_netid, player_netid, msg_invasion_info_002);
}

//==================================================================
/**
 * インフォメーションメッセージ登録：「xxxx」は帰っていきました
 *
 * @param   gcsp		
 * @param   player_netid		主のNetID
 */
//==================================================================
void GameCommInfo_MessageEntry_Leave(GAME_COMM_SYS_PTR gcsp, int player_netid)
{
  GameCommInfo_SetQue(gcsp, player_netid, player_netid, msg_invasion_info_003);
}

//==================================================================
/**
 * インフォメーションメッセージ登録：ミッション成功
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommInfo_MessageEntry_MissionSuccess(GAME_COMM_SYS_PTR gcsp)
{
  GameCommInfo_SetQue(gcsp, GAMEDATA_GetIntrudeMyID(gcsp->gamedata), 
    GAMEDATA_GetIntrudeMyID(gcsp->gamedata), msg_invasion_info_003);
}

//==================================================================
/**
 * インフォメーションメッセージ登録：ミッション失敗
 *
 * @param   gcsp		
 */
//==================================================================
void GameCommInfo_MessageEntry_MissionFail(GAME_COMM_SYS_PTR gcsp)
{
  GameCommInfo_SetQue(gcsp, GAMEDATA_GetIntrudeMyID(gcsp->gamedata), 
    GAMEDATA_GetIntrudeMyID(gcsp->gamedata), msg_invasion_info_003);
}

