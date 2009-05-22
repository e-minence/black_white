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
#include "field/field_comm/field_comm_func.h"
#include "fieldmap/zone_id.h"
#include "message.naix"
#include "msg/msg_invasion.h"


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
}GCSSEQ;

///通信最大人数
#define COMM_PLAYER_MAX   (4)

///インフォメーションメッセージキュー数
#define COMM_INFO_QUE_MAX     (24)


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
  u16 same_count;      ///<同じステータスが送られてきた回数
  u8 invasion_netid;  ///<侵入先ROM
  u8 padding;
}GAME_COMM_PLAYER_STATUS;

//--------------------------------------------------------------
//  インフォメーション
//--------------------------------------------------------------
///インフォメーションのメッセージキュー構造体
typedef struct{
  u16 message_id;
  u8 net_id;
  u8 use;           ///<TRUE:使用中 FALSE:未使用
}GAME_COMM_INFO_QUE;

///インフォメーション構造体
typedef struct{
  GAME_COMM_INFO_QUE msg_que[COMM_INFO_QUE_MAX];
  u8 now_pos;       ///<貯まっているキューの開始位置
  u8 space_pos;     ///<空きキューの開始位置
  u8 padding[2];
  
  STRBUF *name_strbuf[COMM_INFO_QUE_MAX];
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
  void (*exit_func)(int *seq, void *pwk, void *app_work);          ///<終了処理
  BOOL (*exit_wait_func)(int *seq, void *pwk, void *app_work);     ///<終了完了待ち
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
  },
  
  //GAME_COMM_NO_FIELD_BEACON_SEARCH
  {
    GameBeacon_Init,       //init
    GameBeacon_InitWait,   //init_wait
    GameBeacon_Update,     //update
    GameBeacon_Exit,       //exit
    GameBeacon_ExitWait,   //exit_wait
  },
  //GAME_COMM_NO_INVASION
  {
    FIELD_COMM_FUNC_InitCommSystem,       //init
    FIELD_COMM_FUNC_InitCommSystemWait,   //init_wait
    FIELD_COMM_FUNC_UpdateSystem,         //update
    FIELD_COMM_FUNC_TermCommSystem,       //exit
    FIELD_COMM_FUNC_TermCommSystemWait,   //exit_wait
  },
};

//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void GameCommSub_SeqSet(GAME_COMM_SUB_WORK *sub_work, u8 seq);
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, u32 message_id);


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
  int i;
  
  gcsp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAME_COMM_SYS));
  gcsp->gamedata = gamedata;
  
  comm_info = &gcsp->info;
  for(i = 0; i < COMM_PLAYER_MAX; i++){
    comm_info->name_strbuf[i] = GFL_STR_CreateBuffer(BUFLEN_PERSON_NAME, heap_id);
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
  
  for(i = 0; i < COMM_PLAYER_MAX; i++){
    GFL_STR_DeleteBuffer(comm_info->name_strbuf[i]);
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
      func_tbl->exit_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work);
    }
    GameCommSub_SeqSet(sub_work, GCSSEQ_EXIT_WAIT);
    break;
  case GCSSEQ_EXIT_WAIT:
    if(func_tbl->exit_wait_func == NULL || func_tbl->exit_wait_func(&sub_work->func_seq, gcsp->parent_work, gcsp->app_work) == TRUE){
      gcsp->app_work = NULL;
      gcsp->game_comm_no = GAME_COMM_NO_NULL;
      gcsp->comm_status = GAME_COMM_STATUS_NULL;
      if(gcsp->reserve_comm_game_no != GAME_COMM_NO_NULL){
        GameCommSys_Boot(gcsp, gcsp->reserve_comm_game_no, gcsp->reserve_parent_work);
        gcsp->reserve_comm_game_no = GAME_COMM_NO_NULL;
        gcsp->reserve_parent_work = NULL;
      }
    }
    break;
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
  GF_ASSERT(gcsp->sub_work.seq == GCSSEQ_UPDATE);
  
  GameCommSub_SeqSet(&gcsp->sub_work, GCSSEQ_EXIT);
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
 * @retval      FALSE:通常の更新処理を実行している状態
 */
//==================================================================
BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp)
{
  if(gcsp->game_comm_no == GAME_COMM_NO_NULL || gcsp->sub_work.seq != GCSSEQ_UPDATE){
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
  
  if(player_status->zone_id == zone_id && player_status->invasion_netid == invasion_netid){
    return;
  }
  //※check　暫定処理　通信確立＝必ず名前を交換しあっている状況ではないため
  {
    MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, comm_net_id);
    if(MyStatus_CheckNameClear(myst) == TRUE){
      OS_TPrintf("INFO:名前がない net_id = %d\n", comm_net_id);
      return;
    }
  }
  
  if(player_status->zone_id != zone_id){
    player_status->old_zone_id = player_status->zone_id;
    player_status->zone_id = zone_id;
    player_status->same_count = 0;
  }
  else if(zone_id == ZONE_ID_PALACETEST){
    player_status->same_count++;
  }
  player_status->invasion_netid = invasion_netid;
  
  //メッセージ作成
  if(zone_id == ZONE_ID_PALACETEST){
    if(player_status->same_count == 0){
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test01_01 + comm_net_id);
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test03_01 + comm_net_id);
      OS_TPrintf("INFO:パレスにいます net_id = %d\n", comm_net_id);
    }
    else if(player_status->same_count > 60 * 10){
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test01_01 + comm_net_id);
      GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test03_01 + comm_net_id);
      player_status->same_count = 0;
      OS_TPrintf("INFO:継続してパレスにいます net_id = %d\n", comm_net_id);
    }
  }
  else if(player_status->old_zone_id == ZONE_ID_PALACETEST && comm_net_id != invasion_netid
      && zone_id != ZONE_ID_PALACETEST && invasion_netid == GFL_NET_SystemGetCurrentID()){
    GameCommInfo_SetQue(gcsp, comm_net_id, msg_invasion_test07_01 + comm_net_id);
    OS_TPrintf("INFO:街に侵入してきた！ net_id = %d\n", comm_net_id);
  }
}


//==============================================================================
//  インフォメーションメッセージ
//==============================================================================
//--------------------------------------------------------------
/**
 * インフォメーションメッセージをキューに貯める
 *
 * @param   gcsp		      
 * @param   comm_net_id		
 * @param   message_id		
 */
//--------------------------------------------------------------
static void GameCommInfo_SetQue(GAME_COMM_SYS_PTR gcsp, int comm_net_id, u32 message_id)
{
  GAME_COMM_INFO *comm_info = &gcsp->info;
  GAME_COMM_INFO_QUE *que;
  
  if(comm_info->msg_que[comm_info->space_pos].use == TRUE){
    //メッセージの優先順位が決まっていない今は上書き(古いキューは最新のキューで上書きされる
    que = &comm_info->msg_que[comm_info->space_pos];
    comm_info->now_pos++;   //最古のキューから表示されるようにnow_posの位置も後ろにずらす
  }
  else{ //空いているキューなのでそのまま代入
    que = &comm_info->msg_que[comm_info->space_pos];
  }

  GFL_STD_MemClear(que, sizeof(GAME_COMM_INFO_QUE));
  que->net_id = comm_net_id;
  que->message_id = message_id;
  que->use = TRUE;

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
  MYSTATUS *myst;
  
  if(que->use == FALSE){
    return FALSE;
  }
  
  //キューをGAME_COMM_INFO_MESSAGE型に変換して代入
  GFL_STD_MemClear(dest_msg, sizeof(GAME_COMM_INFO_MESSAGE));
  myst = GAMEDATA_GetMyStatusPlayer(gcsp->gamedata, que->net_id);
  MyStatus_CopyNameString(myst, comm_info->name_strbuf[que->net_id]);
  dest_msg->name[0] = comm_info->name_strbuf[que->net_id];
  dest_msg->wordset_no[0] = que->net_id;
  dest_msg->message_id = que->message_id;
  
  //キューの参照位置を進める
  que->use = FALSE;
  comm_info->now_pos++;
  if(comm_info->now_pos >= COMM_INFO_QUE_MAX){
    comm_info->now_pos = 0;
  }
  
  return TRUE;
}

