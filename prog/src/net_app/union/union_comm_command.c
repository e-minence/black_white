//==============================================================================
/**
 * @file    union_comm_command.c
 * @brief   ユニオンルーム：通信コマンド
 * @author  matsuda
 * @date    2009.07.14(火)
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
#include "union_types.h"
#include "union_local.h"
#include "colosseum.h"
#include "net_app/union_app.h"


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size);
static void _UnionRecv_TalkShutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MainMenuListResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MainMenuListResultAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_FirstChildParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_FirstParentAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_Mystatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_TrainerCardParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_ColosseumEntryStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_ColosseumEntryAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_ColosseumEntryAllReady(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameEntryReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameEntryReqAnswerNG(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameEntryReqAnswerOK(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameIntrudeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameIntrudeOK(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameBasicStatusReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameBasicStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameMystatusReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameMystatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameIntrudeReady(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _UnionRecv_MinigameLeaveChild(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//==============================================================================
//  データ
//==============================================================================
///ユニオン受信コマンドテーブル   ※UNION_CMD_???と並びを同じにしておくこと！！
const NetRecvFuncTable Union_CommPacketTbl[] = {
  {_UnionRecv_TalkShutdown, NULL},                      //UNION_CMD_SHUTDOWN
  {_UnionRecv_MainMenuListResult, NULL},                //UNION_CMD_MAINMENU_LIST_RESULT
  {_UnionRecv_MainMenuListResultAnswer, NULL},          //UNION_CMD_MAINMENU_LIST_RESULT_ANSWER
  {_UnionRecv_FirstChildParam, NULL},                   //UNION_CMD_FIRST_CHILD_PARAM
  {_UnionRecv_FirstParentAnswer, NULL},                 //UNION_CMD_FIRST_PARENT_ANSWER
  {_UnionRecv_Mystatus, NULL},                          //UNION_CMD_MYSTATUS
  {_UnionRecv_TrainerCardParam, _RecvHugeBuffer},       //UNION_CMD_TRAINERCARD_PARAM
  {_UnionRecv_ColosseumEntryStatus, _RecvHugeBuffer},   //UNION_CMD_COLOSSEUM_ENTRY
  {_UnionRecv_ColosseumEntryAnswer, _RecvHugeBuffer},   //UNION_CMD_COLOSSEUM_ENTRY_ANSWER
  {_UnionRecv_ColosseumEntryAllReady, NULL},            //UNION_CMD_COLOSSEUM_ENTRY_ALL_READY
  {_UnionRecv_MinigameEntryReq, NULL},                  //UNION_CMD_MINIGAME_ENTRY_REQ
  {_UnionRecv_MinigameEntryReqAnswerNG, NULL},          //UNION_CMD_MINIGAME_ENTRY_REQ_ANSWER_NG
  {_UnionRecv_MinigameEntryReqAnswerOK, NULL},          //UNION_CMD_MINIGAME_ENTRY_REQ_ANSWER_OK
  {_UnionRecv_MinigameIntrudeReq, NULL},                //UNION_CMD_MINIGAME_INTRUDE_REQ
  {_UnionRecv_MinigameIntrudeOK, NULL},                 //UNION_CMD_MINIGAME_INTRUDE_OK
  {_UnionRecv_MinigameBasicStatusReq, NULL},            //UNION_CMD_MINIGAME_BASIC_STATUS_REQ
  {_UnionRecv_MinigameBasicStatus, NULL},               //UNION_CMD_MINIGAME_BASIC_STATUS
  {_UnionRecv_MinigameMystatusReq, NULL},               //UNION_CMD_MINIGAME_MYSTATUS_REQ
  {_UnionRecv_MinigameMystatus, NULL},                  //UNION_CMD_MINIGAME_MYSTATUS
  {_UnionRecv_MinigameIntrudeReady, NULL},              //UNION_CMD_MINIGAME_INTRUDE_READY
  {_UnionRecv_MinigameLeaveChild, NULL},                //UNION_CMD_MINIGAME_LEAVE_CHILD
};
SDK_COMPILER_ASSERT(NELEMS(Union_CommPacketTbl) == UNION_CMD_NUM);



//==============================================================================
//
//  
//
//==============================================================================

//--------------------------------------------------------------
/**
 * 巨大データ用受信バッファ取得
 *
 * @param   netID		
 * @param   pWork		
 * @param   size		
 *
 * @retval  u8 *		
 */
//--------------------------------------------------------------
static u8 * _RecvHugeBuffer(int netID, void* pWork, int size)
{
  UNION_SYSTEM_PTR unisys = pWork;
  GF_ASSERT_MSG_HEAVY(size <= UNION_HUGE_RECEIVE_BUF_SIZE, "size=%x, recv_size=%x\n", size, UNION_HUGE_RECEIVE_BUF_SIZE);
	return unisys->huge_receive_buf[netID];
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：会話中の切断リクエスト
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_TalkShutdown(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  UnionComm_SetTalkShutdown(unisys);
  unisys->my_situation.mycomm.talk_exit_req_bit |= 1 << netID;
  OS_TPrintf("切断リクエスト受信 netID=%d, bit=%d\n", netID, unisys->my_situation.mycomm.talk_exit_req_bit);
}

//==================================================================
/**
 * データ送信：会話中の切断リクエスト
 * @param   select_list		選択結果(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_TalkShutdown(void)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_TALK_SHUTDOWN, 0, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：メインメニューの選択結果
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MainMenuListResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const u32 *select = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }
  
  situ->mycomm.mainmenu_select = *select;
}

//==================================================================
/**
 * データ送信：メインメニューの選択結果
 * @param   select_list		選択結果(UNION_MSG_MENU_SELECT_???)
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MainMenuListResult(u32 select_list)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_MAINMENU_LIST_RESULT, sizeof(u32), &select_list);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：メインメニューの選択結果の返事
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MainMenuListResultAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const BOOL *yes_no = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }
  
  situ->mycomm.mainmenu_yesno_result = *yes_no;
}

//==================================================================
/**
 * データ送信：メインメニューの選択結果の返事
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MainMenuListResultAnswer(BOOL yes_no)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_MAINMENU_LIST_RESULT_ANSWER, sizeof(BOOL), &yes_no);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：子が話しかけで繋がってきた時に認証用に親に送る最初のデータ
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_FirstChildParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const UNION_FIRST_CHILD_PARAM *pFirstParam = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }
  
  OS_TPrintf("Recv FirstParam\n");
  GFL_STD_MemCopy(pFirstParam, &situ->mycomm.talk_first_param, sizeof(UNION_FIRST_CHILD_PARAM));
}

//==================================================================
/**
 * データ送信：子が話しかけで繋がってきた時に認証用に親に送る最初のデータ
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_FirstChildParam(void)
{
  UNION_FIRST_CHILD_PARAM send_data;
  
  OS_TPrintf("SEND FirstParam\n");
  GFL_STD_MemClear(&send_data, sizeof(UNION_FIRST_CHILD_PARAM));
  OS_GetMacAddress(send_data.mac_address);
  send_data.occ = TRUE;
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    UNION_CMD_FIRST_CHILD_PARAM, sizeof(UNION_FIRST_CHILD_PARAM), &send_data);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：親から子に対しての認証の返事
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_FirstParentAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const UNION_FIRST_PARENT_ANSWER *pAnswer = pData;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }
  
  OS_TPrintf("Recv ParentAnswer %d\n", *pAnswer);
  situ->mycomm.first_parent_answer = *pAnswer;
}

//==================================================================
/**
 * データ送信：親から子に対しての認証の返事
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_FirstParentAnswer(UNION_FIRST_PARENT_ANSWER answer)
{
  OS_TPrintf("SEND ParentAnswer = %d\n", answer);
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    UNION_CMD_FIRST_PARENT_ANSWER, sizeof(answer), &answer);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：MYSTATUS
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_Mystatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const MYSTATUS *pMyst = pData;
  MYSTATUS *dest_myst;
  
  OS_TPrintf("MYSTATUS受信 netID = %d\n", netID);
  dest_myst = GAMEDATA_GetMyStatusPlayer(unisys->uniparent->game_data, netID);
  MyStatus_Copy(pMyst, dest_myst);
  situ->mycomm.mystatus_recv_bit |= 1 << netID;
}

//==================================================================
/**
 * データ送信：MYSTATUS
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_Mystatus(UNION_SYSTEM_PTR unisys)
{
  OS_TPrintf("SEND MYSTATUS\n");
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
    UNION_CMD_MYSTATUS, MyStatus_GetWorkSize(), 
    GAMEDATA_GetMyStatus(unisys->uniparent->game_data));
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：トレーナーカード情報
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_TrainerCardParam(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  UNION_MY_SITUATION *situ = &unisys->my_situation;

	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }
  
  OS_TPrintf("COmmCOmmand カード受信 netID = %d\n", netID);
  situ->mycomm.target_card_receive = TRUE;
  GFL_STD_MemCopy(pData, unisys->alloc.target_card, size);
}

//==================================================================
/**
 * データ送信：トレーナーカード情報
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_TrainerCardParam(UNION_SYSTEM_PTR unisys)
{
  GF_ASSERT_HEAVY(unisys->alloc.my_card != NULL);
  
  OS_TPrintf("TR_CARD_DATA = %x\n", sizeof(TR_CARD_DATA));
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, 
    UNION_CMD_TRAINERCARD_PARAM, sizeof(TR_CARD_DATA), 
    unisys->alloc.my_card, TRUE, FALSE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：コロシアム：エントリー情報
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_ColosseumEntryStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  const COLOSSEUM_BASIC_STATUS *p_basic = pData;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
  u8 mac_address[6];
  
  if(clsys == NULL || clsys->cps == NULL || clsys->entry_menu == NULL){
    GF_ASSERT_HEAVY(0);
    return; //準備が出来ていないので受け取らない
  }
  
  OS_TPrintf("コロシアム：エントリー情報受信：net_id = %d\n", netID);
  OS_GetMacAddress(mac_address);
  CommEntryMenu_Entry(clsys->entry_menu, netID, GAMEDATA_GetMyStatus(gamedata), 
    p_basic->force_entry, mac_address);
}

//==================================================================
/**
 * データ送信：コロシアム：エントリー情報
 * @param   select_list		選択結果(UNION_MSG_MENU_SELECT_???)
 * @param   parent_only   TRUE:親のみに送信。　FALSE:全員に送信
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_ColosseumEntryStatus(COLOSSEUM_BASIC_STATUS *basic_status)
{
  OS_TPrintf("コロシアム：エントリー情報送信\n");
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
    UNION_CMD_COLOSSEUM_ENTRY, sizeof(COLOSSEUM_BASIC_STATUS), 
    basic_status, TRUE, FALSE, TRUE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：コロシアム：エントリー結果
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_ColosseumEntryAnswer(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  const COMM_ENTRY_ANSWER *answer = pData;
  
  if(clsys == NULL){
    GF_ASSERT_HEAVY(0);
    return; //準備が出来ていないので受け取らない
  }
  
  OS_TPrintf("コロシアム：エントリー結果受信：answer = %d\n", *answer);
  clsys->mine.entry_answer = *answer;
}

//==================================================================
/**
 * データ送信：コロシアム：エントリー結果
 * @param   
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_ColosseumEntryAnswer(int send_netid, COMM_ENTRY_ANSWER answer)
{
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_netid, UNION_CMD_COLOSSEUM_ENTRY_ANSWER, sizeof(COMM_ENTRY_ANSWER), &answer, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：コロシアム：エントリー全員揃った
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_ColosseumEntryAllReady(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  const COMM_ENTRY_ANSWER *answer = pData;
  
  if(clsys == NULL){
    GF_ASSERT_HEAVY(0);
    return; //準備が出来ていないので受け取らない
  }
  
  OS_TPrintf("コロシアム：エントリー全員揃った受信\n");
  clsys->entry_all_ready = TRUE;
}

//==================================================================
/**
 * データ送信：コロシアム：エントリー全員揃った
 * @param   
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_ColosseumEntryAllReady(void)
{
  return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), UNION_CMD_COLOSSEUM_ENTRY_ALL_READY, 0, NULL);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム乱入希望
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameEntryReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const MYSTATUS *myst = pData;
  
  OS_TPrintf("Recv:ミニゲーム乱入希望 net_id=%d\n", netID);
  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(0);
    unisys->minigame_entry_req_answer_ng_bit |= 1 << netID;
    return;
  }
  
  if(UnionAppSystem_SetEntryUser(unisys->alloc.uniapp, netID, myst) == TRUE){
    unisys->minigame_entry_req_answer_ok_bit |= 1 << netID;
  }
  else{
    unisys->minigame_entry_req_answer_ng_bit |= 1 << netID;
  }
}

//==================================================================
/**
 * データ送信：ミニゲーム乱入希望
 * @param   
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameEntryReq(UNION_SYSTEM_PTR unisys)
{
  unisys->minigame_entry_answer = UNION_MINIGAME_ENTRY_ANSWER_NULL;
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
    UNION_CMD_MINIGAME_ENTRY_REQ, MyStatus_GetWorkSize(), 
    GAMEDATA_GetMyStatus(unisys->uniparent->game_data), TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム乱入希望の返事(NG)
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameEntryReqAnswerNG(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const MYSTATUS *myst = pData;
  
  OS_TPrintf("Recv:ミニゲーム乱入希望の返事NG\n");
  unisys->minigame_entry_answer = UNION_MINIGAME_ENTRY_ANSWER_NG;
}

//==================================================================
/**
 * データ送信：ミニゲーム乱入希望の返事(NG)
 * @param   
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameEntryReqAnswerNG(u8 send_bit)
{
  OS_TPrintf("SEND: MinigameEntryReqAnswerNG send_bit=%d\n", send_bit);
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, 
    UNION_CMD_MINIGAME_ENTRY_REQ_ANSWER_NG, 0, NULL, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム乱入希望の返事(OK)
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameEntryReqAnswerOK(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const MYSTATUS *myst = pData;
  
  OS_TPrintf("Recv:ミニゲーム乱入希望の返事OK\n");
  unisys->minigame_entry_answer = UNION_MINIGAME_ENTRY_ANSWER_OK;
}

//==================================================================
/**
 * データ送信：ミニゲーム乱入希望の返事(OK)
 * @param   
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameEntryReqAnswerOK(u8 send_bit)
{
  OS_TPrintf("SEND: MinigameEntryReqAnswerOK send_bit=%d\n", send_bit);
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, 
    UNION_CMD_MINIGAME_ENTRY_REQ_ANSWER_OK, 0, NULL, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム乱入希望リクエスト
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameIntrudeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("MinigameIntrudeReq受信 netID = %d\n", netID);
  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(unisys->alloc.uniapp != NULL);
    return;
  }
  
  UnionAppSystem_ReqIntrude(unisys->alloc.uniapp, netID);
}

//==================================================================
/**
 * データ送信：ミニゲーム乱入希望リクエスト
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameIntrudeReq(void)
{
  OS_TPrintf("SEND MinigameIntrudeReq\n");
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
    UNION_CMD_MINIGAME_INTRUDE_REQ, 0, NULL, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム乱入許可
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameIntrudeOK(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  OS_TPrintf("MinigameIntrudeOK受信 netID = %d\n", netID);
  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(unisys->alloc.uniapp != NULL);
    return;
  }
  
  UnionAppSystem_RecvIntrudeOK(unisys->alloc.uniapp);
}

//==================================================================
/**
 * データ送信：ミニゲーム乱入許可
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameIntrudeOK(NetID send_netid)
{
  OS_TPrintf("SEND MinigameIntrudeOK ID=%d\n", send_netid);
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), send_netid, 
    UNION_CMD_MINIGAME_INTRUDE_OK, 0, NULL, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム基本情報リクエスト
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameBasicStatusReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  OS_TPrintf("MinigameBasicStatusReq受信 netID = %d\n", netID);
  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(unisys->alloc.uniapp != NULL);
    return;
  }
  
  UnionAppSystem_ReqBasicStatus(unisys->alloc.uniapp, netID);
}

//==================================================================
/**
 * データ送信：ミニゲーム基本情報リクエスト
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameBasicStatusReq(void)
{
  OS_TPrintf("SEND MinigameBasicStatusReq\n");
  return GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_NO_PARENTMACHINE, 
    UNION_CMD_MINIGAME_BASIC_STATUS_REQ, 0, NULL, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム基本情報
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameBasicStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const UNION_APP_BASIC * appbasic = pData;
  
  if(GFL_NET_IsParentMachine() == TRUE){  //自分が親の場合は無視
    return;
  }
  
  OS_TPrintf("APP_BASIC受信\n");
  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(0);
    return;
  }
  
  UnionAppSystem_SetBasicStatus(unisys->alloc.uniapp, appbasic);
}

//==================================================================
/**
 * データ送信：ミニゲーム基本情報
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameBasicStatus(const UNION_APP_BASIC *app_basic, u8 send_bit)
{
  OS_TPrintf("SEND APP_BASIC send_bit=%d\n", send_bit);
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, 
    UNION_CMD_MINIGAME_BASIC_STATUS, UnionAppSystem_GetBasicSize(), app_basic, 
    TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム用MYSTATUS要求
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameMystatusReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const UNION_APP_MY_PARAM *pAppmy = pData;
  
  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(0);
    return;
  }
  
  OS_TPrintf("ミニゲーム用MYSTATUS Req受信 netID = %d\n", netID);

  //乱入者用にReqしてきた人自身の基本情報もここで受け取る
  //最初の二人は2回両方リクエストしているので再度受け取るけど問題は無い
  UnionAppSystem_SetMystatus(unisys->alloc.uniapp, unisys, netID, pAppmy);
  //リクエスト命令なので、当然返す。これも乱入者用
  UnionAppSystem_ReqMystatus(unisys->alloc.uniapp, netID);
}

//==================================================================
/**
 * データ送信：ミニゲーム用MYSTATUS要求
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @param   myst    自分のMYSTATUS
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameMystatusReq(u8 send_bit, const MYSTATUS *myst)
{
  UNION_APP_MY_PARAM appmy;
  
  GFL_STD_MemClear(&appmy, sizeof(UNION_APP_MY_PARAM));
  appmy.myst = *myst;
  OS_GetMacAddress(appmy.mac_address);
  
  OS_TPrintf("SEND ミニゲーム用MYSTATUS Req send_bit=%d\n", send_bit);
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, UNION_CMD_MINIGAME_MYSTATUS_REQ, 
    sizeof(UNION_APP_MY_PARAM), &appmy, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲーム用MYSTATUS
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameMystatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const UNION_APP_MY_PARAM *pAppmy = pData;

  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(0);
    return;
  }
  
  OS_TPrintf("ミニゲーム用MYSTATUS受信 netID = %d\n", netID);
  UnionAppSystem_SetMystatus(unisys->alloc.uniapp, unisys, netID, pAppmy);
}

//==================================================================
/**
 * データ送信：ミニゲーム用MYSTATUS
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameMystatus(u8 send_bit, const UNION_APP_MY_PARAM *pAppmy)
{
  OS_TPrintf("SEND ミニゲーム用MYSTATUS\n");
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, UNION_CMD_MINIGAME_MYSTATUS, sizeof(UNION_APP_MY_PARAM), pAppmy, TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：乱入の準備完了宣言
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameIntrudeReady(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const UNION_APP_MY_PARAM *pAppmy = pData;
  
	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }

  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(0);
    return;
  }
  
  OS_TPrintf("ミニゲーム 乱入準備完了受信 netID = %d\n", netID);
  UnionAppSystem_SetMystatus(unisys->alloc.uniapp, unisys, netID, pAppmy);
  UnionAppSystem_SetIntrudeReady(unisys->alloc.uniapp, netID);
}

//==================================================================
/**
 * データ送信：乱入の準備完了宣言
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameIntrudeReady(u8 send_bit, const MYSTATUS *pMyst)
{
  UNION_APP_MY_PARAM appmy;
  
  GFL_STD_MemClear(&appmy, sizeof(UNION_APP_MY_PARAM));
  appmy.myst = *pMyst;
  OS_GetMacAddress(appmy.mac_address);

  OS_TPrintf("SEND ミニゲーム 乱入準備完了\n");
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, UNION_CMD_MINIGAME_INTRUDE_READY, sizeof(UNION_APP_MY_PARAM), &appmy, 
    TRUE, FALSE, FALSE);
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   コマンド受信：ミニゲームで子機の誰かが離脱しました
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none  
 */
//--------------------------------------------------------------
static void _UnionRecv_MinigameLeaveChild(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  UNION_SYSTEM_PTR unisys = pWork;
  const NetID *leave_netid = pData;
  
	if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return; //自分のデータなので無視
  }

  if(unisys->alloc.uniapp == NULL){
    GF_ASSERT_HEAVY(0);
    return;
  }
  
  OS_TPrintf("Recv ミニゲームで子機が離脱 離脱netID = %d\n", *leave_netid);
  UnionAppSystem_SetLeaveChild(unisys->alloc.uniapp, *leave_netid);
}

//==================================================================
/**
 * データ送信：ミニゲームで子機の誰かが離脱しました
 * @param   yes_no    TRUE:はい　FALSE:いいえ
 * @retval  BOOL		TRUE:送信成功。　FALSE:失敗
 */
//==================================================================
BOOL UnionSend_MinigameLeaveChild(u8 send_bit, NetID leave_netid)
{
  OS_TPrintf("SEND ミニゲームで子機が離脱 離脱NetID=%d\n", leave_netid);
  return GFL_NET_SendDataExBit(GFL_NET_HANDLE_GetCurrentHandle(), 
    send_bit, UNION_CMD_MINIGAME_LEAVE_CHILD, sizeof(leave_netid), &leave_netid, 
    TRUE, FALSE, FALSE);
}
