//==============================================================================
/**
 * @file    intrude_mission.c
 * @brief   ミッション処理    受信データのセットなどを行う為、常駐に配置
 * @author  matsuda
 * @date    2009.10.26(月)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "intrude_types.h"
#include "intrude_mission.h"
#include "intrude_comm_command.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_msg.h"
#include "intrude_main.h"
#include "intrude_work.h"
#include "print/wordset.h"
#include "field/zonedata.h"

#include "mission.naix"
#include "msg/msg_mission.h"
#include "msg/msg_mission_monolith.h"
#include "mission_ng_check.h"


SDK_COMPILER_ASSERT(MISSION_LIST_MAX == MISSION_TYPE_MAX);


//==============================================================================
//  定数定義
//==============================================================================
///ミッション開始までの準備時間
#define _MISSION_READY_TIMER      (10)  //秒

enum{
  _SEND_MISSION_START_NULL,         //リクエストなし
  _SEND_MISSION_START_SEND_REQ,     //送信リクエスト
  _SEND_MISSION_START_SENDED,       //送信した
};


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_Update_StartClientAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_SetMissionFail(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int fail_netid);
static void MISSION_Update_EntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_Update_AchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_ClearTargetInfo(MISSION_TARGET_INFO *target);
static s32 _GetMissionTime(MISSION_SYSTEM *mission);
static void MISSION_ClearMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);


//==============================================================================
//  データ
//==============================================================================
#include "mission_list_param.cdat"




//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ミッションデータ初期化
 *
 * @param   mission		ミッションシステムへのポインタ
 */
//==================================================================
void MISSION_Init(MISSION_SYSTEM *mission)
{
  int i;
  
  GFL_STD_MemClear(mission, sizeof(MISSION_SYSTEM));
  mission->data.accept_netid = INTRUDE_NETID_NULL;
  MISSION_ClearTargetInfo(&mission->data.target_info);
  mission->result.mission_data.accept_netid = INTRUDE_NETID_NULL;
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    mission->result.achieve_netid[i] = INTRUDE_NETID_NULL;
  }
  
  MISSION_Init_List(mission);
}

//==================================================================
/**
 * ミッションリスト初期化
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Init_List(MISSION_SYSTEM *mission)
{
  int i;
  MISSION_TYPE type;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    mission->list[i].occ = FALSE;
  }
}

//==================================================================
/**
 * ミッション更新処理
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Update(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  //親機でミッション発動しているなら失敗までの秒数をカウントする
  if(GFL_NET_IsParentMachine() == TRUE 
      && MISSION_RecvCheck(mission) == TRUE){
    if(mission->result.mission_data.accept_netid == INTRUDE_NETID_NULL){
      if(mission->data.variable.ready_timer > 0){
        if(mission->send_mission_start == _SEND_MISSION_START_NULL 
            && _GetMissionTime(mission) > mission->data.variable.ready_timer){
          mission->send_mission_start = _SEND_MISSION_START_SEND_REQ;
        }
      }
      else if(mission->mission_start_ok == TRUE){
        if(_GetMissionTime(mission) > mission->data.variable.exe_timer){
          GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
          if(mission->result.mission_fail == FALSE){
            MISSION_SetMissionFail(intcomm, mission, GAMEDATA_GetIntrudeMyID(gamedata));
          }
        }
      }
    }
    else{ //誰か達成者がいる
      if(mission->mission_start_ok == TRUE){
        if(_GetMissionTime(mission) > mission->data.variable.exe_timer){
          //ミッションの制限時間に達したが成功して終了
          mission->mission_timeend_success = TRUE;
        }
      }
    }
  }
  
  //送信リクエストがあれば送信
  MISSION_Update_EntryAnswer(intcomm, mission);
  MISSION_Update_AchieveAnswer(intcomm, mission);
  MISSION_SendUpdate(intcomm, mission);
  MISSION_Update_StartClientAnswer(intcomm, mission);
}

//--------------------------------------------------------------
/**
 * ミッション経過時間を取得
 *
 * @param   mission		
 *
 * @retval  s32		経過時間
 */
//--------------------------------------------------------------
static s32 _GetMissionTime(MISSION_SYSTEM *mission)
{
  u32 timer = GFL_RTC_GetTimeBySecond();
  
  if(timer < mission->start_timer){ //start_timerより小さくなっている場合は回り込みが発生
    timer += GFL_RTC_TIME_SECOND_MAX + 1;
  }
  return timer - mission->start_timer;
}

//--------------------------------------------------------------
/**
 * 実施中のミッションの残り時間を取得
 *
 * @param   mission		
 *
 * @retval  s32		残り時間
 */
//--------------------------------------------------------------
static s32 MISSION_GetExeMissionTimer(MISSION_SYSTEM *mission)
{
  s32 ret_timer;
  
 ret_timer = mission->data.variable.exe_timer;  //mission->data.cdata.time;
 ret_timer -= _GetMissionTime(mission);
  if(ret_timer < 0){
    return 0;
  }
  return ret_timer;
}

//==================================================================
/**
 * ミッション残り時間を取得
 *
 * @param   mission		
 *
 * @retval  s32		残り時間
 */
//==================================================================
s32 MISSION_GetMissionTimer(MISSION_SYSTEM *mission)
{
  s32 timer, ret_timer;
  
  if(MISSION_RecvCheck(mission) == TRUE 
      && mission->result.mission_data.accept_netid == INTRUDE_NETID_NULL){
    if(mission->data.variable.ready_timer > 0 || mission->mission_start_ok == FALSE){
      ret_timer = mission->data.variable.ready_timer;
      ret_timer -= _GetMissionTime(mission);
    }
    else{
      ret_timer = MISSION_GetExeMissionTimer(mission);
    }
    
    if(ret_timer < 0){
      return 0;
    }
    return ret_timer;
  }
  return 0;
}

//==================================================================
/**
 * ミッションリスト送信リクエストを設定する
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Set_ListSendReq(MISSION_SYSTEM *mission, int palace_area)
{
  mission->list_send_req[palace_area] = TRUE;
}

//==================================================================
/**
 * ミッションデータ送信リクエストを設定する
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Set_DataSendReq(MISSION_SYSTEM *mission)
{
  mission->data_send_req = TRUE;
}

//==================================================================
/**
 * ミッション結果送信リクエストを設定する
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Set_ResultSendReq(MISSION_SYSTEM *mission)
{
  mission->result_send_req = TRUE;
}

//==================================================================
/**
 * ミッションリストをセット
 *
 * @param   mission		代入先
 * @param   MISSION_CHOICE_LIST		    セットするデータ
 * @param   net_id    
 */
//==================================================================
void MISSION_SetMissionList(MISSION_SYSTEM *mission, const MISSION_CHOICE_LIST *list, NetID net_id)
{
  MISSION_CHOICE_LIST *mlist = &mission->list[net_id];
  
  if(mlist->occ == TRUE){
    return; //既に受信済み
  }
  
  *mlist = *list;
  //不正チェック
#if 0 //もう少ししっかりとデータの取り扱いが決まってから有効にする
  if(mdata->mission_no >= MISSION_NO_MAX || mdata->monolith_type >= MONOLITH_TYPE_MAX){
    return;
  }
#endif
}

//==================================================================
/**
 * ミッションデータをセット
 *
 * @param   mission		代入先
 * @param   src		    セットするデータ
 * 
 * @retval  new_mission   TRUE:新規に受信したミッション　FALSE:受信済み or 無効なミッション
 */
//==================================================================
BOOL MISSION_SetMissionData(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_DATA *src)
{
  MISSION_DATA *mdata = &mission->data;
  BOOL new_mission = FALSE;
  
  if(src->cdata.reward > MISSION_REWARD_MAX || src->cdata.time > MISSION_TIME_MAX){
    return FALSE; //不正データ
  }

  if(mission->parent_data_recv == FALSE && src->accept_netid != INTRUDE_NETID_NULL){
    new_mission = TRUE;
    mission->parent_data_recv = TRUE;
  }
  
  //親の場合、既にmisison_noはセットされているので判定の前に受信フラグをセット
  //MISSION_ClearMissionEntry(intcomm, mission);  一度しか受けれないのでクリアの必要は無くなった 2010.05.29(土)
  if(mdata->accept_netid != INTRUDE_NETID_NULL){
    return new_mission;
  }
  
  *mdata = *src;
  
  //不正チェック
#if 0 //もう少ししっかりとデータの取り扱いが決まってから有効にする
  if(mdata->mission_no >= MISSION_NO_MAX || mdata->monolith_type >= MONOLITH_TYPE_MAX){
    return;
  }
#endif
  return new_mission;
}

//==================================================================
/**
 * ミッション開始受信
 *
 * @param   mission		
 */
//==================================================================
void MISSION_RecvMissionStart(MISSION_SYSTEM *mission)
{
  MISSION_DATA *md = MISSION_GetRecvData(mission);
  if(md != NULL){
    md->variable.ready_timer = 0;
  }
}

//==================================================================
/**
 * クライアントから「ミッション開始宣言」を受信
 *
 * @param   mission		
 * @param   net_id		
 */
//==================================================================
void MISSION_RecvMissionStartClient(MISSION_SYSTEM *mission, NetID net_id)
{
  if(mission->start_client_bit == 0){ //誰かがミッションを開始したら親が持つ全体タイマーをセット
    mission->start_timer = GFL_RTC_GetTimeBySecond();
  }
  mission->start_client_bit |= 1 << net_id;
  mission->send_start_client_bit |= 1 << net_id;
}

//==================================================================
/**
 * 親からもらった「ミッション開始宣言の返事」を受信
 *
 * @param   mission		  
 * @param   net_id		  
 * @param   now_timer		親からもらった現在のミッションの残り時間
 */
//==================================================================
void MISSION_RecvMissionStartClientAnswer(MISSION_SYSTEM *mission, s32 now_timer)
{
  mission->mission_start_ok = TRUE;
  mission->data.variable.exe_timer = now_timer;
  if(GFL_NET_IsParentMachine() == FALSE){ //親はMISSION_RecvMissionStartClientでセットされる
    mission->start_timer = GFL_RTC_GetTimeBySecond();
  }
  OS_TPrintf("start_timer = %d, variable.exe_timer = %d\n", mission->start_timer, now_timer);
}

//==================================================================
/**
 * 親からミッション開始OKをもらったかチェック
 *
 * @param   mission		
 *
 * @retval  BOOL		  TRUE:OK
 */
//==================================================================
BOOL MISSION_CheckMissionStartOK(MISSION_SYSTEM *mission)
{
  return mission->mission_start_ok;
}

//==================================================================
/**
 * ミッションデータ送信リクエストが発生していれば送信を行う
 *
 * @param   intcomm		
 * @param   mission		
 */
//==================================================================
static void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(mission->list_send_req[i] == TRUE){
      if(IntrudeSend_MissionList(intcomm, mission, i) == TRUE){
        mission->list_send_req[i] = FALSE;
      }
    }
  }
  
  if(mission->data_send_req == TRUE){
    if(IntrudeSend_MissionData(intcomm, mission) == TRUE){
      mission->data_send_req = FALSE;
    }
  }
  
  if(mission->result_send_req == TRUE){
    if(IntrudeSend_MissionResult(intcomm, mission) == TRUE){
      mission->result_send_req = FALSE;
    }
  }
  
  if(mission->send_mission_start == _SEND_MISSION_START_SEND_REQ){
    if(IntrudeSend_MissionStart(intcomm, mission) == TRUE){
      mission->send_mission_start = _SEND_MISSION_START_SENDED;
    }
  }
  
  if(mission->send_answer_achieve_check_usenone_bit > 0){
    if(IntrudeSend_MissionAnswerAchieveUserNone(mission->send_answer_achieve_check_usenone_bit) == TRUE){
      mission->send_answer_achieve_check_usenone_bit = 0;
    }
  }
  if(mission->send_answer_achieve_check_use_bit > 0){
    if(IntrudeSend_MissionAnswerAchieveUserUse(mission->send_answer_achieve_check_use_bit) == TRUE){
      mission->send_answer_achieve_check_use_bit = 0;
    }
  }
}

//--------------------------------------------------------------
/**
 * 「ミッション開始宣言」の返事
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_Update_StartClientAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->send_start_client_bit > 0){
    if(IntrudeSend_MissionClientStartAnswer(
        intcomm, MISSION_GetExeMissionTimer(mission), mission->send_start_client_bit) == TRUE){
      mission->send_start_client_bit = 0;
    }
  }
}

//==================================================================
/**
 * ミッションデータ受信済みかチェック
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:受信済み　FALSE:受信していない
 */
//==================================================================
BOOL MISSION_RecvCheck(const MISSION_SYSTEM *mission)
{
  const MISSION_DATA *mdata = &mission->data;
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL || mission->parent_data_recv == FALSE){
    return FALSE;
  }
  return TRUE;
}

//==================================================================
/**
 * 受信済みのミッションデータを取得する
 *
 * @param   mission		        
 *
 * @retval  MISSION_DATA *		ミッションデータへのポインタ(未受信の場合はNULL)
 */
//==================================================================
MISSION_DATA * MISSION_GetRecvData(MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == FALSE){
    return NULL;
  }
  return &mission->data;
}

//==================================================================
/**
 * ミッション参加フラグをセットする
 *
 * @param   mission		
 */
//==================================================================
void MISSION_SetMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == TRUE){
    mission->mine_entry = TRUE;
    mission->mission_complete = FALSE;
    intcomm->intrude_status_mine.mission_entry = TRUE;
    intcomm->send_status = TRUE;
    OS_TPrintf("ミッション参加フラグセット\n");
  }
}

//--------------------------------------------------------------
/**
 * ミッション参加フラグをリセットする
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_ClearMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->mine_entry == TRUE){
    mission->mine_entry = FALSE;
    intcomm->intrude_status_mine.mission_entry = FALSE;
    intcomm->send_status = TRUE;
  }
  mission->mission_complete = FALSE;
}

//==================================================================
/**
 * ミッション参加フラグを取得する
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:ミッションに参加している
 */
//==================================================================
BOOL MISSION_GetMissionEntry(const MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == TRUE){
    return mission->mine_entry;
  }
  return FALSE;
}

//==================================================================
/**
 * ミッション達成フラグをセットする
 *
 * @param   mission		
 */
//==================================================================
void MISSION_SetMissionComplete(MISSION_SYSTEM *mission)
{
  mission->mission_complete = TRUE;
}

//==================================================================
/**
 * ミッション達成フラグを取得する
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:ミッション達成済み　FALSE:ミッションまだ未達成
 */
//==================================================================
BOOL MISSION_GetMissionComplete(const MISSION_SYSTEM *mission)
{
  return mission->mission_complete;
}

//==================================================================
/**
 * ミッション結果データ取得
 *
 * @param   mission		
 *
 * @retval  MISSION_RESULT *		結果データ(結果未受信の場合はNULL)
 */
//==================================================================
MISSION_RESULT * MISSION_GetResultData(MISSION_SYSTEM *mission)
{
  if(MISSION_CheckRecvResult(mission) == FALSE){
    return NULL;
  }
  return &mission->result;
}

//==================================================================
/**
 * ミッションを遊んでいる状態か調べる
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:ミッションで遊んでいる　FALSE:ミッションで遊んでいない
 */
//==================================================================
BOOL MISSION_GetMissionPlay(MISSION_SYSTEM *mission)
{
  //エントリーして実際にミッションをやっているか、
  //結果を受け取ってまだ表示していない状態(参加者でなければ結果は受け取らない)
  if(MISSION_GetMissionEntry(mission) == TRUE || MISSION_GetResultData(mission) != NULL){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 受信：ミッション達成者がいるかどうかを調査要求
 *
 * @param   mission		
 * @param   net_id		調査を要求しているNetID
 */
//==================================================================
void MISSION_Recv_CheckAchieveUser(MISSION_SYSTEM *mission, int net_id)
{
  MISSION_RESULT *result = &mission->result;

  if(result->achieve_netid[0] == INTRUDE_NETID_NULL){
    mission->send_answer_achieve_check_usenone_bit |= 1 << net_id;
  }
  else{
    mission->send_answer_achieve_check_use_bit |= 1 << net_id;
  }
}

//==================================================================
/**
 * ミッション達成者がいたかどうかの返事を取得
 *
 * @param   mission		
 *
 * @retval  MISSION_ACHIEVE_USER		
 */
//==================================================================
MISSION_ACHIEVE_USER MISSION_GetAnswerAchieveUser(MISSION_SYSTEM *mission)
{
  MISSION_ACHIEVE_USER ret;
  
  ret = mission->recv_answer_achieve_check_use;
  MISSION_SetRecvBufAnswerAchieveUserBuf(mission, MISSION_ACHIEVE_USER_NULL);
  return ret;
}

//==================================================================
/**
 * ミッション達成者がいるかどうかの受信バッファをクリア
 *
 * @param   mission		
 */
//==================================================================
void MISSION_SetRecvBufAnswerAchieveUserBuf(MISSION_SYSTEM *mission, MISSION_ACHIEVE_USER data)
{
  mission->recv_answer_achieve_check_use = data;
}

//--------------------------------------------------------------
/**
 * ミッション達成者として登録する
 *
 * @param   mission		
 * @param   mdata		
 * @param   achieve_netid		達成者のNetID
 *
 * @retval  int		  達成者としての順位(席が埋まっていた場合はFIELD_COMM_MEMBER_MAX)
 */
//--------------------------------------------------------------
static int _SetAchieveNetID(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid)
{
  MISSION_RESULT *result = &mission->result;
  
  if(result->achieve_netid[0] == INTRUDE_NETID_NULL){
    return 0;
  }
  return FIELD_COMM_MEMBER_MAX;
}

//==================================================================
/**
 * ミッション達成報告エントリー
 *
 * @param   mission		
 * @param   mdata		        達成したミッションデータ
 * @param   achieve_netid		達成者のNetID
 *
 * @retval  BOOL		TRUE：達成者として受け付けられた
 * @retval  BOOL		FALSE：先に達成者がいる等して受け付けられなかった
 */
//==================================================================
BOOL MISSION_EntryAchieve(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid)
{
  MISSION_RESULT *result = &mission->result;
  MISSION_DATA *data = &mission->data;
  int ranking;
  
  OS_TPrintf("ミッション達成エントリー net_id=%d\n", achieve_netid);
  if(GFL_STD_MemComp(data, mdata, sizeof(MISSION_DATA) - sizeof(MISSION_VARIABLE_PARAM)) != 0){
    OS_TPrintf("親が管理しているミッションデータと内容が違うので受け付けない\n");
    mission->result_mission_achieve[achieve_netid] = MISSION_ACHIEVE_NG;
    return FALSE;
  }
  
  ranking = _SetAchieveNetID(mission, mdata, achieve_netid);
  if(ranking == FIELD_COMM_MEMBER_MAX){
    OS_TPrintf("先に達成者がいたので受け付けない\n");
    mission->result_mission_achieve[achieve_netid] = MISSION_ACHIEVE_NG;
    return FALSE;
  }
  
  result->mission_data = *mdata;
  result->achieve_netid[ranking] = achieve_netid;
  
  mission->result_mission_achieve[achieve_netid] = MISSION_ACHIEVE_OK;
  mission->result_send_req = TRUE;
  
  return TRUE;
}

//==================================================================
/**
 * ミッション達成報告の返事を取得
 *
 * @param   mission		
 *
 * @retval  MISSIN_ACHIEVE		MISSION_ACHIEVE_NULLの場合は返事は未受信
 */
//==================================================================
MISSION_ACHIEVE MISSION_GetAchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->parent_achieve_recv == MISSION_ACHIEVE_OK){
    //OKの場合はこのまま結果画面にいけるように結果関連の全てのデータ受信を待つ
    if(MISSION_CheckRecvResult(mission) == TRUE && Intrude_CheckRecvOccupyResult(intcomm) == TRUE){
      return MISSION_ACHIEVE_OK;
    }
    return MISSION_ACHIEVE_NULL;
  }
  return mission->parent_achieve_recv;
}

//==================================================================
/**
 * ミッション達成報告の返事受信バッファをクリア
 *
 * @param   mission		
 */
//==================================================================
void MISSION_ClearAchieveAnswer(MISSION_SYSTEM *mission)
{
  mission->parent_achieve_recv = MISSION_ACHIEVE_NULL;
}

//==================================================================
/**
 * 親からのミッション達成報告の返事をセット
 *
 * @param   mission		
 * @param   achieve		
 */
//==================================================================
void MISSION_SetParentAchieve(MISSION_SYSTEM *mission, MISSION_ACHIEVE achieve)
{
  mission->parent_achieve_recv = achieve;
  OS_TPrintf("ミッション達成報告の返事 = %d\n", achieve);
}


//--------------------------------------------------------------
/**
 * ミッション失敗をセット
 *
 * @param   mission		
 * @param   fail_netid		
 */
//--------------------------------------------------------------
static void MISSION_SetMissionFail(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int fail_netid)
{
  MISSION_RESULT *result = &mission->result;
  MISSION_DATA *mdata = &mission->data;
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL 
      || result->mission_data.accept_netid != INTRUDE_NETID_NULL){
    OS_TPrintf("MissionFailは受け入れられない %d, %d\n", 
      mdata->accept_netid, result->mission_data.accept_netid);
    return;
  }
  
  result->mission_data = *mdata;
  result->mission_fail = TRUE;
  mission->result_send_req = TRUE;
  mission->send_mission_start = _SEND_MISSION_START_NULL;
  OS_TPrintf("ミッション失敗をセット\n");
}

//==================================================================
/**
 * ミッション結果をセット
 *
 * @param   mission		
 * @param   cp_result		
 */
//==================================================================
void MISSION_SetResult(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_RESULT *cp_result)
{
  MISSION_RESULT *result = &mission->result;
  
  GF_ASSERT(mission->recv_result == FALSE);
  *result = *cp_result;
  mission->recv_result = TRUE;
  //MISSION_ClearMissionEntry(intcomm, mission);
}

//==================================================================
/**
 * ミッション結果受信確認
 *
 * @param   mission		ミッションシステムへのポインタ
 *
 * @retval  BOOL		TRUE:受信した。　FALSE:受信していない
 */
//==================================================================
BOOL MISSION_CheckRecvResult(const MISSION_SYSTEM *mission)
{
  const MISSION_RESULT *result = &mission->result;
  
  if(mission->recv_result == TRUE 
      && result->mission_data.accept_netid != INTRUDE_NETID_NULL){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 話しかけ：ミッション達成チェック
 *
 * @param   mission		    ミッションデータへのポインタ
 * @param   talk_netid		話しかけた相手のNetID
 *
 * @retval  BOOL		TRUE:達成。　FALSE:非達成
 */
//==================================================================
BOOL MISSION_Talk_CheckAchieve(const MISSION_SYSTEM *mission, int talk_netid)
{
  const MISSION_DATA *mdata = &mission->data;
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL){
    return FALSE;
  }
  if(talk_netid == mdata->target_info.net_id){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 現在実施中のミッションタイプを取得する
 *
 * @param   mission		
 *
 * @retval  MISSION_TYPE		
 */
//==================================================================
MISSION_TYPE MISSION_GetMissionType(const MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == FALSE){
    return MISSION_TYPE_NONE;
  }
  return mission->data.cdata.type;
}

//==================================================================
/**
 * ミッション結果から得られるミッションポイントを取得する
 *
 * @param   intcomm		
 * @param   result		
 *
 * @retval  int		ミッションポイント
 */
//==================================================================
s32 MISSION_GetResultPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  const MISSION_RESULT *result = &intcomm->mission.result;
  int i;
  
  if(result->mission_fail == TRUE){
    return 0;
  }
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(result->achieve_netid[i] == GAMEDATA_GetIntrudeMyID(gamedata)){
      if(result->mission_data.cdata.reward > MISSION_REWARD_MAX){
        return 1; //不正データ
      }
      return result->mission_data.cdata.reward;
    }
  }
  return 0;
}

//==================================================================
/**
 * 占拠情報に対してミッション達成済みにする
 *
 * @param   intcomm		
 * @param   mission		
 */
//==================================================================
void MISSION_SetMissionClear(GAMEDATA *gamedata, INTRUDE_COMM_SYS_PTR intcomm, const MISSION_RESULT *result)
{
  OCCUPY_INFO *mine_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  int achieve_version;
  MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gamedata, result->achieve_netid[0]);
  
  achieve_version = intcomm->intrude_status[result->achieve_netid[0]].pm_version;
  achieve_version = Intrude_GetIntrudeRomVersion(achieve_version, MyStatus_GetID(myst));
  
  if(achieve_version == VERSION_WHITE){
    mine_occupy->mlst.mission_clear[result->mission_data.cdata.type] = MISSION_CLEAR_WHITE;
  }
  else if(achieve_version == VERSION_BLACK){
    mine_occupy->mlst.mission_clear[result->mission_data.cdata.type] = MISSION_CLEAR_BLACK;
  }
}

//==================================================================
/**
 * 自分がミッション達成者か調べる
 *
 * @param   intcomm		
 * @param   mission		
 *
 * @retval  BOOL		TRUE:ミッション達成者　FALSE:達成者ではない
 */
//==================================================================
BOOL MISSION_CheckResultMissionMine(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(MISSION_CheckRecvResult(mission) == FALSE){
    return FALSE;
  }
  if(MISSION_GetResultPoint(intcomm, mission) > 0){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ミッション失敗(タイムアウト)が発生しているか調べる
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:タイムアウトの失敗が発生している
 */
//==================================================================
BOOL MISSION_CheckResultTimeout(MISSION_SYSTEM *mission)
{
  const MISSION_RESULT *mresult;

  mresult = MISSION_GetResultData(mission);
  if(mresult == NULL){
    return FALSE;
  }
  
  if(mresult->mission_fail == TRUE){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ミッション達成後、ミッションの制限時間に達したか確認
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:達成してミッションの制限時間も終了している
 * @retval  BOOL    FALSE:いずれかの条件を満たしていない
 */
//==================================================================
BOOL MISSION_CheckSuccessTimeEnd(MISSION_SYSTEM *mission)
{
  return mission->mission_timeend_success;
}

//==============================================================================
//
//  ミッションデータ
//
//==============================================================================
//--------------------------------------------------------------
/**
 * ターゲット情報をクリア
 *
 * @param   target		
 */
//--------------------------------------------------------------
static void MISSION_ClearTargetInfo(MISSION_TARGET_INFO *target)
{
  GFL_STD_MemClear(target, sizeof(MISSION_TARGET_INFO));
  target->net_id = INTRUDE_NETID_NULL;
}

//--------------------------------------------------------------
/**
 * ターゲット情報をセット
 *
 * @param   intcomm		
 * @param   target	      	代入先
 * @param   net_id		      ターゲットのNetID
 */
//--------------------------------------------------------------
static void MISSION_SetTargetInfo(INTRUDE_COMM_SYS_PTR intcomm, MISSION_TARGET_INFO *target, NetID net_id)
{
  const MYSTATUS *myst;
  
  target->net_id = net_id;
  
  myst = Intrude_GetMyStatus(GameCommSys_GetGameData(intcomm->game_comm), target->net_id);
  MyStatus_CopyNameStrCode(myst, target->name, PERSON_NAME_SIZE + EOM_SIZE);
  target->sex = MyStatus_GetMySex(myst);
}

//==================================================================
/**
 * ミッション選択候補リストを作成
 *
 * @param   intcomm		
 * @param   mission		    
 * @param   accept_netid  ミッション受注者のNetID
 * @param   palace_area		選択候補を得る対象のパレスエリア
 */
//==================================================================
void MISSION_MissionList_Create(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int accept_netid, int palace_area)
{
  MISSION_CHOICE_LIST *list;
  const OCCUPY_INFO *occupy;
  u8 monolith_type, pm_version;
  const MYSTATUS *myst;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  list = &mission->list[palace_area];
  if(list->occ == TRUE){
    return; //既に作成済み
  }
  
  occupy = Intrude_GetOccupyInfo(gamedata, palace_area);
  myst = Intrude_GetMyStatus(gamedata, palace_area);
  pm_version = MyStatus_GetRomCode( myst );
  pm_version = Intrude_GetIntrudeRomVersion(pm_version, MyStatus_GetID(myst));
  
  if(pm_version == VERSION_BLACK){
    monolith_type = MONOLITH_TYPE_BLACK;
  }
  else{
    monolith_type = MONOLITH_TYPE_WHITE;
  }
  
  list->occupy = *occupy;
  list->monolith_type = monolith_type;
  list->accept_netid = accept_netid;
  MISSION_SetTargetInfo(intcomm, &list->target_info, palace_area);
  list->occ = TRUE;
}

//==================================================================
/**
 * ミッション選択候補リストが有効かどうかチェック
 *
 * @param   list		
 *
 * @retval  BOOL		TRUE:有効　FALSE:無効
 */
//==================================================================
BOOL MISSION_MissionList_CheckOcc(const MISSION_CHOICE_LIST *list)
{
  return list->occ;
}

//==================================================================
/**
 * ミッションデータが不正かどうか調査
 *
 * @param   mission		
 * @param   mdata		  ミッションデータへのポインタ
 *
 * @retval  BOOL		TRUE:問題なし。　FALSE:不正である
 */
//==================================================================
static BOOL MISSION_MissionList_CheckNG(const MISSION_SYSTEM *mission, const MISSION_CONV_DATA *cdat)
{
  int i;
  
  if(cdat->type >= MISSION_TYPE_MAX || cdat->reward > MISSION_REWARD_MAX 
      || cdat->time > MISSION_TIME_MAX || cdat->msg_id_contents >= plc_mis_end
      || cdat->msg_id_contents_monolith >= plc_mism_end){
    return FALSE;
  }
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(cdat->talk_type[i] >= TALK_TYPE_MAX){
      return FALSE;
    }
    if(Intrude_CheckNG_OBJID(cdat->obj_id[i]) == FALSE){
      return FALSE;
    }
  }
  
  for(i = 0; i < 2; i++){
    if(cdat->type == MISSION_TYPE_ATTRIBUTE){
      const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)cdat->data;
      if(Intrude_CheckNG_Item(d_attr->item_no) == FALSE){
        return FALSE;
      }
    }
    else if(cdat->type == MISSION_TYPE_ITEM){
      const MISSION_TYPEDATA_ITEM *d_item = (void*)cdat->data;
      if(Intrude_CheckNG_Item(d_item->item_no) == FALSE){
        return FALSE;
      }
    }
  }
  
  return TRUE;
}

//==================================================================
/**
 * ミッションエントリー
 *
 * @param   mission		
 * @param   mdata		  ミッションデータへのポインタ
 * @param   net_id		エントリー者のNetID
 *
 * @retval  BOOL		  TRUE:エントリー成功　FALSE:エントリー失敗
 */
//==================================================================
BOOL MISSION_SetEntryNew(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_ENTRY_REQ *entry_req, int net_id)
{
  MISSION_DATA *exe_mdata = &mission->data;

  GFL_STD_MemClear(&mission->entry_answer[net_id], sizeof(MISSION_ENTRY_ANSWER));

  if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    OS_TPrintf("NG:既に他のミッションが起動している\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG;
    return FALSE;
  }
  
  if(MISSION_MissionList_CheckNG(&intcomm->mission, &entry_req->cdata) == FALSE){
    OS_TPrintf("NG:ミッション内容が不正\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG;
    return FALSE;
  }
  
#if 0 //ターゲットを強制的に表に戻すようにしたので必要なくなった 2010.05.04(火)
  if(ZONEDATA_IsPalaceField(intcomm->intrude_status[entry_req->target_info.net_id].zone_id)==TRUE
      || ZONEDATA_IsPalace(intcomm->intrude_status[entry_req->target_info.net_id].zone_id)==TRUE){
    OS_TPrintf("NG:ターゲットが裏にいる\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG_TARGET_REVERSE;
    return FALSE;
  }
#endif
  
  if(entry_req->cdata.type == MISSION_TYPE_VICTORY || entry_req->cdata.type == MISSION_TYPE_ATTRIBUTE){
    if(GFL_NET_SystemGetConnectNum() != 2 || GFL_NET_GetConnectNum() != 2){
      OS_TPrintf("NG:二人専用ミッションだが接続人数が二人ではない\n");
      mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG_TWIN;
      return FALSE;
    }
  }
  
  //実行するミッションとして登録
  exe_mdata->cdata = entry_req->cdata;
  exe_mdata->target_info = entry_req->target_info;
  exe_mdata->accept_netid = net_id;
  exe_mdata->palace_area = entry_req->target_info.net_id;
  exe_mdata->monolith_type = entry_req->monolith_type;
  exe_mdata->variable.ready_timer = _MISSION_READY_TIMER;
  exe_mdata->variable.exe_timer = entry_req->cdata.time;
  
  //返事セット
  mission->entry_answer[net_id].mdata = *exe_mdata;
  mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_OK;
  
  //全員に実行されるミッションデータを送信する
  MISSION_Set_DataSendReq(mission);

  mission->start_timer = GFL_RTC_GetTimeBySecond();

  mission->send_mission_start = _SEND_MISSION_START_NULL;
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ミッションエントリーの結果を送信
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_Update_EntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(mission->entry_answer[i].result != MISSION_ENTRY_RESULT_NULL){
      if(IntrudeSend_MissionEntryAnswer(intcomm, &mission->entry_answer[i], i) == TRUE){
        GFL_STD_MemClear(&mission->entry_answer[i], sizeof(MISSION_ENTRY_ANSWER));
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ミッション達成報告の返事を送信
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_Update_AchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(mission->result_mission_achieve[i] != MISSION_ACHIEVE_NULL){
      if(IntrudeSend_MissionAchieveAnswer(intcomm, mission->result_mission_achieve[i], i) == TRUE){
        mission->result_mission_achieve[i] = MISSION_ACHIEVE_NULL;
      }
    }
  }
}

//==================================================================
/**
 * 「ミッション受信します」の返事を受け取る
 *
 * @param   mission		
 * @param   entry_answer		親から送られてきた返事データ
 */
//==================================================================
void MISSION_SetRecvEntryAnswer(MISSION_SYSTEM *mission, const MISSION_ENTRY_ANSWER *entry_answer)
{
  mission->recv_entry_answer_result = entry_answer->result;
  if(entry_answer->result == MISSION_ENTRY_RESULT_OK){
    mission->data = entry_answer->mdata;
  }
}

//==================================================================
/**
 * 「ミッション受信します」の親からの返事を取得
 *
 * @param   mission		
 *
 * @retval  MISSION_ENTRY_RESULT		
 * 
 * この関数を呼ぶごとに返事を受け取るフラグはクリアします
 */
//==================================================================
MISSION_ENTRY_RESULT MISSION_GetRecvEntryAnswer(MISSION_SYSTEM *mission)
{
  MISSION_ENTRY_RESULT result;
  
  result = mission->recv_entry_answer_result;
  MISSION_ClearRecvEntryAnswer(mission);
  return result;
}

//==================================================================
/**
 * 「ミッション受信します」の親からの返事を受け取るバッファをクリア
 *
 * @param   mission		
 */
//==================================================================
void MISSION_ClearRecvEntryAnswer(MISSION_SYSTEM *mission)
{
  mission->recv_entry_answer_result = MISSION_ENTRY_RESULT_NULL;
}

//==================================================================
/**
 * 指定NetIDがミッションターゲットの相手か調べる
 *
 * @param   mission		
 * @param   net_id		調査対象のNetID
 *
 * @retval  BOOL		TRUE:ミッションターゲット　FALSE:ミッションターゲットではない
 */
//==================================================================
BOOL MISSION_CheckMissionTargetNetID(MISSION_SYSTEM *mission, int net_id)
{
  if(MISSION_RecvCheck(mission) == FALSE){ //ミッションが発動していない
    return FALSE;
  }
  
  if(mission->data.target_info.net_id == net_id){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ミッションターゲットのNetIDを取得
 *
 * @param   mission		
 *
 * @retval  NetID     ターゲットのNetID(ミッション未発動 or エラー時は自分のNetID)
 */
//==================================================================
NetID MISSION_GetMissionTargetNetID(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == FALSE){ //ミッションが発動していない
    return GAMEDATA_GetIntrudeMyID(GameCommSys_GetGameData(intcomm->game_comm));
  }
  return mission->data.target_info.net_id;
}

//==================================================================
/**
 * 自分自身がミッションのターゲットになっているかを調べる
 *
 * @param   intcomm		
 *
 * @retval  BOOL		  TRUE:自分がターゲットになっている
 * @retval  BOOL      FALSE:自分はターゲットではない
 */
//==================================================================
BOOL MISSION_CheckTargetIsMine(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);

  if(MISSION_CheckMissionTargetNetID(&intcomm->mission, GAMEDATA_GetIntrudeMyID(gamedata)) == TRUE){
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * 生のSTRCODEをWORDSETする
 *
 * @param   wordset		
 * @param   bufID		
 * @param   code		        
 * @param   str_length		  STRCODEの長さ
 * @param   temp_heap_id		テンポラリで使用するヒープID
 */
//--------------------------------------------------------------
static void _Wordset_Strcode(WORDSET *wordset, u32 bufID, const STRCODE *code, int str_length, HEAPID temp_heap_id, int sex)
{
  STRBUF *strbuf = 	GFL_STR_CreateBuffer(str_length, temp_heap_id);

	GFL_STR_SetStringCodeOrderLength(strbuf, code, str_length);
  WORDSET_RegisterWord(wordset, bufID, strbuf, sex, TRUE, PM_LANG);

  GFL_STR_DeleteBuffer(strbuf);
}

//==================================================================
/**
 * ミッションターゲットの名前をWordsetする
 *
 * @param   wordset		
 * @param   bufID		
 * @param   target		
 * @param   temp_heap_id		
 */
//==================================================================
void MISSIONDATA_WordsetTargetName(WORDSET *wordset, u32 bufID, const MISSION_TARGET_INFO *target, HEAPID temp_heap_id)
{
  _Wordset_Strcode(wordset, bufID, target->name, 
    PERSON_NAME_SIZE + EOM_SIZE, temp_heap_id, target->sex);
}

//==================================================================
/**
 * ミッションデータに対応したメッセージをWORDSETする
 *
 * @param   intcomm		
 * @param   mdata		        ミッションデータへのポインタ
 * @param   wordset		      
 * @param   temp_heap_id		テンポラリで使用するヒープID
 */
//==================================================================
void MISSIONDATA_Wordset(const MISSION_CONV_DATA *cdata, const MISSION_TARGET_INFO *target, WORDSET *wordset, HEAPID temp_heap_id)
{
  switch(cdata->type){
  case MISSION_TYPE_VICTORY:     //勝利(LV)
    {
      const MISSION_TYPEDATA_VICTORY *d_vic = (void*)cdata->data;
      
      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      
      WORDSET_RegisterNumber(wordset, 1, d_vic->battle_level, 3, 
        STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    }
    break;
  case MISSION_TYPE_SKILL:       //技
    {
      const MISSION_TYPEDATA_SKILL *d_skill = (void*)cdata->data;

      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      WORDSET_RegisterGPowerName( wordset, 1, d_skill->gpower_id );
    }
    break;
  case MISSION_TYPE_BASIC:       //基礎
    {
      const MISSION_TYPEDATA_BASIC *d_bas = (void*)cdata->data;
      
      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      WORDSET_RegisterGPowerName( wordset, 1, d_bas->gpower_id );
    }
    break;
  case MISSION_TYPE_ATTRIBUTE:   //属性
    {
      const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)cdata->data;

      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      WORDSET_RegisterItemName( wordset, 1, d_attr->item_no );
      WORDSET_RegisterNumber( 
        wordset, 2, d_attr->price, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    }
    break;
  case MISSION_TYPE_ITEM:        //道具
    {
      const MISSION_TYPEDATA_ITEM *d_item = (void*)cdata->data;
      
      WORDSET_RegisterItemName( wordset, 0, d_item->item_no );
      
      MISSIONDATA_WordsetTargetName(wordset, 1, target, temp_heap_id);
    }
    break;
  case MISSION_TYPE_PERSONALITY: //性格
    {
      const MISSION_TYPEDATA_PERSONALITY *d_per = (void*)cdata->data;
      
      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
    }
    break;
  }
  
  //制限時間
  WORDSET_RegisterNumber(wordset, 2, cdata->time, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  //報酬
  {
    int reward = cdata->reward;
    if(reward > MISSION_REWARD_MAX){
      reward = 1;
    }
    WORDSET_RegisterNumber(wordset, 3, reward, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  }
}

//==================================================================
/**
 * 占拠情報からミッションリストを作成する
 *
 * @param   occupy		
 */
//==================================================================
void MISSION_LIST_Create(OCCUPY_INFO *occupy)
{
  int mission_type;
  
  for(mission_type = 0; mission_type < MISSION_LIST_MAX; mission_type++){
    occupy->mlst.mission_clear[mission_type] = MISSION_CLEAR_NONE;
    MISSION_LIST_Create_Type(occupy, mission_type);
  }
}

//==================================================================
/**
 * 占拠情報からミッションリストを指定タイプに対して抽選を行う
 *
 * @param   occupy		
 * @param   mission_type		
 */
//==================================================================
void MISSION_LIST_Create_Type(OCCUPY_INFO *occupy, MISSION_TYPE mission_type)
{
  int no;
  int palace_level;
  int my_version_bit;
  
#if (PM_VERSION == VERSION_WHITE)
  my_version_bit = MISSION_DATA_VERSION_WHITE;
#elif (PM_VERSION == VERSION_BLACK)
  my_version_bit = MISSION_DATA_VERSION_BLACK;
#else
  my_version_bit = MISSION_DATA_VERSION_NEXT;
#endif

  palace_level = occupy->white_level + occupy->black_level;
  
  for(no = 0; no < NELEMS(MissionConvDataListParam); no++){
    if(MissionConvDataListParam[no].type == mission_type
        && MissionConvDataListParam[no].level <= palace_level){
      if(MissionConvDataListParam[no].version_bit & my_version_bit){
        if(MissionConvDataListParam[no].odds == 100 
            || MissionConvDataListParam[no].odds >= GFUser_GetPublicRand(100+1)){
          occupy->mlst.mission_no[mission_type] = no;
          return;
        }
      }
    }
  }
  GF_ASSERT_MSG(0, "type=%d", mission_type);
}

//==================================================================
/**
 * ミッションを全て制覇しているならばミッションリストを全て作成しなおす
 *
 * @param   occupy		
 * @param   white_num   全占拠達成時：白の数
 * @param   black_num   全占拠達成時：黒の数
 *
 * @retval  BOOL		TRUE:作成しなおした
 */
//==================================================================
BOOL MISSION_LIST_Create_Complete(OCCUPY_INFO *occupy, u8 *white_num, u8 *black_num)
{
  int mission_type;
  
  for(mission_type = 0; mission_type < MISSION_LIST_MAX; mission_type++){
    if(occupy->mlst.mission_clear[mission_type] == MISSION_CLEAR_NONE){
      return FALSE;
    }
    else if(occupy->mlst.mission_clear[mission_type] == MISSION_CLEAR_WHITE){
      (*white_num)++;
    }
    else{
      (*black_num)++;
    }
  }
  MISSION_LIST_Create(occupy);
  return TRUE;
}

//==============================================================================
//  デバッグ
//==============================================================================
#ifdef PM_DEBUG
//==================================================================
/**
 * デバッグ用：指定ミッションタイプにセット可能なミッションNoを取得する
 *
 * @param   mission_type		MISSION_TYPE_xxx
 * @param   no		          検索開始番号(最初は0初期化した状態にしておく事)
 *
 * @retval  int		セット可能なミッション番号(-1の場合はデータ終端)
 */
//==================================================================
int DEBUG_MISSION_TypeSearch(MISSION_TYPE mission_type, int *no)
{
  int ret;
  
  for( ; *no < NELEMS(MissionConvDataListParam); (*no)++){
    if(MissionConvDataListParam[*no].type == mission_type){
      ret = *no;
      (*no)++;
      return ret;
    }
  }
  return -1;
}

//==================================================================
/**
 * デバッグ用：ミッションタイプを指定して直接ミッション番号をセット
 *
 * @param   occupy		      占拠情報へのポインタ
 * @param   mission_type		MISSION_TYPE_xxx
 * @param   no		          ミッション番号
 */
//==================================================================
void DEBUG_MISSION_TypeNoSet(OCCUPY_INFO *occupy, MISSION_TYPE mission_type, int no)
{
  occupy->mlst.mission_no[mission_type] = no;
}

//==================================================================
/**
 * デバッグ用：ミッションタイプを指定してミッション番号を取得
 *
 * @param   occupy		      占拠情報へのポインタ
 * @param   mission_type		MISSION_TYPE_xxx
 * 
 * @retval  ミッション番号
 */
//==================================================================
int DEBUG_MISSION_TypeNoGet(const OCCUPY_INFO *occupy, MISSION_TYPE mission_type)
{
  return occupy->mlst.mission_no[mission_type];
}

#endif  //PM_DEBUG

