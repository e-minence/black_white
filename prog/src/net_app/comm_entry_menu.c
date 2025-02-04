//==============================================================================
/**
 * @file    comm_entry_menu.c
 * @brief   参加募集メニュー制御
 * @author  matsuda
 * @date    2009.07.28(火)
 */
//==============================================================================
#include <gflib.h>
#include "field/fieldmap.h"
#include "message.naix"
#include "msg/msg_connect.h"
#include "net_app/comm_entry_menu.h"
#include "print/wordset.h"
#include "savedata/mystatus_local.h"
#include "comm_entry_menu_comm_command.h"


//==============================================================================
//  定数定義
//==============================================================================
///EXPANDで展開するメッセージバッファのサイズ
#define EXPAND_STRBUF_SIZE      (100)
///LIST EXPANDで展開するメッセージバッファのサイズ
#define LIST_EXPAND_STRBUF_SIZE      (64)
///NUM EXPANDで展開するメッセージバッファのサイズ
#define NUM_EXPAND_STRBUF_SIZE      (32)
///ゲーム開始前の同期番号
#define GAMESTART_SYNC_NO (1) 
///メッセージ描画リクエスト：リクエスト無し
#define _MSG_REQ_ID_NULL        (0xffff)

///ユーザーステータス
enum{
  USER_STATUS_NULL,         ///<ユーザーがいない
  USER_STATUS_READY,        ///<準備中(リストにまだ名前が載っていない)
  USER_STATUS_EXAMINATION,  ///<審査対象
  USER_STATUS_EXAMINATION_WAIT, ///<審査完了待ち
  USER_STATUS_COMPLETION,   ///<エントリー完了
  USER_STATUS_REFUSE,       ///<断った
};

///エントリーリストBMPサイズ(キャラ単位)
enum{
  LISTBMP_POS_LEFT = 2,
  LISTBMP_POS_RIGHT = 20,
  LISTBMP_POS_TOP = 6,
  LISTBMP_POS_BOTTOM_PARENT = LISTBMP_POS_TOP + 10,   //親の時のYサイズ
  LISTBMP_POS_BOTTOM_CHILD = LISTBMP_POS_TOP + 8,     //子の時のYサイズ
};

///エントリーリストのBMP書き込み位置(ドット単位)
enum{
  LISTBMP_START_NAME_X = 0,
  LISTBMP_START_NAME_Y = 0,
  LISTBMP_START_NUM_X = 0,
  LISTBMP_START_NUM_Y = 2*4*8,
};

///タイトルBMPサイズ(キャラ単位)
enum{
  TITLEBMP_POS_LEFT = 2,
  TITLEBMP_POS_RIGHT = 30,
  TITLEBMP_POS_TOP = 1,
  TITLEBMP_POS_BOTTOM = TITLEBMP_POS_TOP + 2,
};


///最終結果
typedef enum{
  DECIDE_TYPE_NULL,
  DECIDE_TYPE_OK,
  DECIDE_TYPE_NG,
}DECIDE_TYPE;

///「解散しますか？」結果
typedef enum{
  BREAKUP_TYPE_NULL,
  BREAKUP_TYPE_OK,
  BREAKUP_TYPE_NG,
}BREAKUP_TYPE;


///親機探索メニューリスト最大数
#define PARENT_LIST_MAX     (16)
///親機探索メニューリストに載っている寿命
#define PARENT_LIST_LIFE    (30*5)
///親機探索メニュー接続時タイムアウト
#define PARENT_LIST_TIMEOUT (60*5)

///_ParentSearchList_Updateの最終結果
typedef enum{
  PARENT_SEARCH_LIST_SELECT_NULL,       ///<実行中
  PARENT_SEARCH_LIST_SELECT_OK,         ///<親機と接続した
  PARENT_SEARCH_LIST_SELECT_NG,         ///<親機に断られた
  PARENT_SEARCH_LIST_SELECT_CANCEL,     ///<キャンセルした
}PARENT_SEARCH_LIST_SELECT;

///_ParentWait_UpdateCnacel戻り値
typedef enum
{
  PARENT_WAIT_CANCEL_NULL,    //操作無し
  PARENT_WAIT_CANCEL_SELECT,  //選択中
  PARENT_WAIT_CANCEL_CANCEL,  //キャンセル
}PARENT_WAIT_CANCEL_STATE;
//==============================================================================
//  構造体定義
//==============================================================================
//--------------------------------------------------------------
//  使用するメッセージの構造体
//--------------------------------------------------------------
typedef struct{
  u16 msgid_title;              ///<開催しているゲームタイトル
  u16 msgid_breakup_confirm;    ///<「解散しますか？」
  u16 msgid_breakup;            ///<「解散しました」
  u16 msgid_breakup_parent_only;  ///<「親機のみになったので解散します」
  u16 msgid_breakup_game;       ///<「都合の悪いメンバーがいるためゲームを中止します」
  u16 msgid_child_entry;        ///<「ｘｘｘをする　ともだちを　選んでください」
}ENTRYMENU_MESSAGE_PACK;

struct _ENTRYMENU_MEMBER_INFO{
  MYSTATUS mystatus[COMM_ENTRY_USER_MAX];   ///<参加者のMYSTATUS
  u8 member_bit;                            ///<有効なメンバー(bit管理)
  u8 padding[3];
};

///エントリー者のデータ
typedef struct{
  MYSTATUS mystatus;
  u8 status;
  u8 force_entry;        ///<TRUE:強制的に受け入れる
  u8 send_flag;          ///<TRUE:返事送信済み(外側から立てる)
  
  u8 mac_address[6];      ///<MACアドレス
  u8 padding[2];
}COMM_ENTRY_USER;

///「はい・いいえ」制御
typedef struct{
  FLDMENUFUNC *menufunc;
  BREAKUP_TYPE breakup_type;
  u8 seq;
  u8 padding[3];
}COMM_ENTRY_YESNO;

///親機情報
typedef struct{
  MYSTATUS mystatus;
  u8 mac_address[6];
  u8 occ;                 ///<TRUE：データ有効
  u8 padding;
  s16 life;
  u8 padding2[2];
}COMM_PARENTUSER;

///親探索リスト制御
typedef struct{
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  FLDMENUFUNC *menufunc;
  COMM_PARENTUSER parentuser[PARENT_LIST_MAX];  ///<ビーコンに引っかかった親機情報
  COMM_PARENTUSER *connect_parent;              ///<接続先として選んだ親機情報
  PARENT_SEARCH_LIST_SELECT final_select;       ///<最終結果
  u8 local_seq;
  u8 list_update_req;
  u8 list_strbuf_create;
  u8 return_seq;
  u16 timeoutCnt;
}PARENTSEARCH_LIST;

///参加募集管理システム
typedef struct _COMM_ENTRY_MENU_SYSTEM{
  FIELDMAP_WORK *fieldWork;
  WORDSET *wordset;
  GFL_MSGDATA *msgdata;
  FLDMSGWIN *fldmsgwin_list;
  FLDMSGWIN *fldmsgwin_title;
  FLDMSGWIN_STREAM *fld_stream;
  STRBUF *strbuf_expand;
  STRBUF *strbuf_list_template;         ///<エントリー者のExpand元になるmsg
  STRBUF *strbuf_list_expand;           ///<エントリー者のExpand用バッファ
  STRBUF *strbuf_num_template;          ///<残り人数のExpand元になるmsg
  STRBUF *strbuf_num_expand;            ///<残り人数のExpand用バッファ
  
  COMM_ENTRY_RESULT entry_result;       ///<最終結果
  MYSTATUS mine_mystatus;
  COMM_ENTRY_USER user[COMM_ENTRY_USER_MAX];    ///<エントリー者のデータ
  BOOL examination_occ;                         ///<TRUE:審査中のユーザーがいる
  COMM_ENTRY_USER examination_user;             ///<審査中のユーザー
  COMM_ENTRY_YESNO yesno;                       ///<「はい・いいえ」制御
  PARENTSEARCH_LIST parentsearch;                 ///<親機探索リスト
  GFLNetInitializeStruct netInitStruct;         ///<ネット終了時に確保しておく
  void  *netParentWork;                         ///<ネット終了時に確保しておく
  
  void *callback_work;                          ///<コールバック時に引数として渡すユーザーポインタ
  COMM_ENTRY_MENU_CALLBACK callback_entry;     ///<入室コールバック
  COMM_ENTRY_MENU_CALLBACK callback_leave;     ///<退室コールバック
  
  ENTRYMENU_MEMBER_INFO member_info;            ///<参加者情報(親機の場合は送信バッファとして使用)
  u8 member_info_recv;                          ///<TRUE:参加者情報を受信した
  u8 game_start;                                ///<TRUE:ゲーム開始受信
  u8 game_cancel;                               ///<TRUE:ゲームキャンセル受信
  u8 mp_mode;                                   ///<TRUE:MP通信  FALSE:DS通信
  
  u8 min_num;         ///<最少人数
  u8 max_num;         ///<最大人数
  u8 seq;
  u8 entry_mode;      ///<COMM_ENTRY_MODE_xxx
  
  u8 game_type;       ///<COMM_ENTRY_GAMETYPE_xxx
  u8 send_bit_entry_ok;   ///<エントリーOKの返事(bit管理)
  u8 send_bit_entry_ng;   ///<エントリーNGの返事(bit管理)
  u8 update_member_info;          ///<メンバー情報送信リクエスト
  u8 update_member_info_reserve;  ///<メンバー情報送信予約
  u8 member_info_sending;         ///<TRUE:メンバー情報送信中
  u8 wait_user_bit;       ///<終了前に接続されているユーザー(bit管理)
  HEAPID heap_id;
  
  ENTRY_PARENT_ANSWER entry_parent_answer;
  MYSTATUS entry_parent_myst;
  u8 final_user_status[COMM_ENTRY_USER_MAX];
  
  u8 parent_mac_address[6];       ///<modeがCOMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE時のみ
  
  u8 draw_mac[COMM_ENTRY_USER_MAX][6];  ///<リストに名前が書かれているプレイヤーのMacAddress
  
  s8 draw_player_num;             ///<残り人数」の現在
  u8 game_start_num;              ///<開始時の人数
  u16 msg_req_id;                 ///<メッセージ描画リクエスト(メッセージID)
  
  u8 forbit_leave;                ///<TRUE:離脱禁止
  u8 forbit_leave_answer_req;     ///<TRUE:離脱禁止しました、の返事を返す
  u8 recv_ready_bit;              ///<ゲーム開始準備完了したユーザー(bit管理)
  u8 padding;
}COMM_ENTRY_MENU_SYSTEM;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static BOOL _Update_Parent(COMM_ENTRY_MENU_PTR em);
static BOOL _Update_Child(COMM_ENTRY_MENU_PTR em);
static BOOL _Update_ChildParentConnect(COMM_ENTRY_MENU_PTR em);
static BOOL _Update_ChildParentDesignate(COMM_ENTRY_MENU_PTR em);
static void _SendUpdate_Parent(COMM_ENTRY_MENU_PTR em);
static u8 _MemberInfoSendBufCreate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_LeaveUserUpdate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_ListUpdate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_UserStatusUpdate(COMM_ENTRY_MENU_PTR em);
static void CommEntryMenu_EraseUser(COMM_ENTRY_MENU_PTR em, NetID net_id);
static BOOL CommEntryMenu_SetExamination(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user);
static void CommEntryMenu_ExaminationUpdate(COMM_ENTRY_MENU_PTR em);
static DECIDE_TYPE CommEntryMenu_DecideUpdate(COMM_ENTRY_MENU_PTR em);
static BREAKUP_TYPE CommEntryMenu_BreakupUpdate(COMM_ENTRY_MENU_PTR em);
static BOOL CommEntryMenu_InterruptCheck(COMM_ENTRY_MENU_PTR em);
static void _MemberInfo_Setup(COMM_ENTRY_MENU_PTR em);
static void _MemberInfo_Exit(COMM_ENTRY_MENU_PTR em);
static void _ParentEntry_NumDraw(COMM_ENTRY_MENU_PTR em, int player_num);
static void _ParentEntry_NameDraw(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID);
static void _ParentEntry_NameErase(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID);
static void _MemberInfo_NameDraw(COMM_ENTRY_MENU_PTR em, const MYSTATUS *myst, NetID netID);
static void _MemberInfo_NameErase(COMM_ENTRY_MENU_PTR em, int netID);
static void _ChildEntryMember_ListRewriteUpdate(COMM_ENTRY_MENU_PTR em);
static void _StreamMsgSet(COMM_ENTRY_MENU_PTR em, u32 msg_id);
static void CommEntryMenu_SetFinalUser(COMM_ENTRY_MENU_PTR em);
static BOOL CommEntryMenu_CheckFinalUser(COMM_ENTRY_MENU_PTR em);
static void _SendBitClear(COMM_ENTRY_MENU_PTR em, NetID netID);
static void _Req_SendMemberInfo(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_Setup(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_Exit(COMM_ENTRY_MENU_PTR em);
static PARENT_SEARCH_LIST_SELECT _ParentSearchList_Update(COMM_ENTRY_MENU_PTR em);
static BOOL _ParentSearchList_ListSelectUpdate(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_ParentLifeUpdate(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_BeaconUpdate(COMM_ENTRY_MENU_PTR em);
static void _ParentSearchList_SetNewParent(COMM_ENTRY_MENU_PTR em, const u8 *mac_address, const MYSTATUS *myst);
static void _ParentSearchList_DeleteParent(COMM_ENTRY_MENU_PTR em, int parent_no);
static void _ParentSearchList_SetListString(COMM_ENTRY_MENU_PTR em);
static PARENT_WAIT_CANCEL_STATE _ParentWait_UpdateCnacel( COMM_ENTRY_MENU_PTR em );
static void _ParentWait_ExitCnacel( COMM_ENTRY_MENU_PTR em );
static u8 _GetConnectMemberBit(void);
static BOOL _CheckConnectMemberBit(u8 member_bit);


//==============================================================================
//  データ
//==============================================================================
///ゲームタイプ毎に異なるメッセージIDのデータパッケージ
ALIGN4 static const ENTRYMENU_MESSAGE_PACK _GameTypeMsgPack[] = {
  {//COMM_ENTRY_GAMETYPE_COLOSSEUM
    msg_connect_01_04,
    msg_connect_03_01,
    msg_connect_05_01,
    msg_connect_07_01,
    msg_connect_09_01,
    msg_child_00,         //未使用
  },
  {//COMM_ENTRY_GAMETYPE_MUSICAL
    msg_connect_01_04_2,
    msg_connect_03_01_2,
    msg_connect_05_01_2,
    msg_connect_07_01,    //未使用
    msg_connect_09_01_2,
    msg_child_00,
  },
  {//COMM_ENTRY_GAMETYPE_SUBWAY
    msg_connect_01_04_3,
    msg_connect_03_01_3,
    msg_connect_05_01_3,
    msg_connect_07_01,    //未使用
    msg_connect_09_01_3,
    msg_child_00_01,
  },
};
SDK_COMPILER_ASSERT(NELEMS(_GameTypeMsgPack) == COMM_ENTRY_GAMETYPE_MAX);

///プレイヤーが居ない事を示すMacAddress
ALIGN4 static const u8 _NullMacAddress[6] = {0, 0, 0, 0, 0, 0};


///親探索メニューヘッダー
static const FLDMENUFUNC_HEADER ParentSearchMenuListHeader = {
  16,                     //リスト項目数
  5,                      //表示最大項目数
  0,                      //ラベル表示Ｘ座標
  13,                     //項目表示Ｘ座標
  0,                      //カーソル表示Ｘ座標
  0,                      //表示Ｙ座標
  1,                      //表示文字色
  15,                     //表示背景色
  2,                      //表示文字影色
  0,                      //文字間隔Ｘ
  0,                      //文字間隔Ｙ
  FLDMENUFUNC_SKIP_NON,   //ページスキップタイプ
  12,                     //文字サイズX(ドット
  16,                     //文字サイズY(ドット
	1,                      //表示座標X キャラ単位
	1,                      //表示座標Y キャラ単位
	22,                     //表示サイズX キャラ単位
	10,                     //表示サイズY キャラ単位
};




//==================================================================
/**
 * 参加募集メニュー：セットアップ
 *
 * @param   fieldWork		
 * @param   min_num		  最少人数
 * @param   max_num		  最大人数
 * @param   heap_id		  ヒープID
 * @param   entry_mode  COMM_ENTRY_MODE_xxx
 * @param   game_type   COMM_ENTRY_GAMETYPE_xxx
 * @param   parent_mac_address  親のMacAddress(entry_modeがCOMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE時のみ必要)
 *
 * @retval  COMM_ENTRY_MENU_PTR		生成した参加募集メニューワークへのポインタ
 */
//==================================================================
COMM_ENTRY_MENU_PTR CommEntryMenu_Setup(const MYSTATUS *myst, FIELDMAP_WORK *fieldWork, int min_num, int max_num, HEAPID heap_id, COMM_ENTRY_MODE entry_mode, COMM_ENTRY_GAMETYPE game_type, const u8 *parent_mac_address)
{
  COMM_ENTRY_MENU_PTR em;
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  
  OS_TPrintf("CommEntryMenu_Setup\n");
  GF_ASSERT(max_num <= COMM_ENTRY_USER_MAX);
  
  em = GFL_HEAP_AllocClearMemory(heap_id, sizeof(COMM_ENTRY_MENU_SYSTEM));
  
  em->wordset = WORDSET_Create( heap_id );
  em->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_connect_dat );
  em->fld_stream = FLDMSGWIN_STREAM_AddTalkWin(fldmsg_bg, em->msgdata);
  em->strbuf_expand = GFL_STR_CreateBuffer(EXPAND_STRBUF_SIZE, heap_id);
  em->strbuf_list_template = GFL_MSG_CreateString( em->msgdata, msg_connect_01_04_4 );
  em->strbuf_list_expand = GFL_STR_CreateBuffer(LIST_EXPAND_STRBUF_SIZE, heap_id);
  if( game_type == COMM_ENTRY_GAMETYPE_MUSICAL )
  {
    em->strbuf_num_template = GFL_MSG_CreateString( em->msgdata, msg_connect_01_03_01 );
  }
  else
  {
    em->strbuf_num_template = GFL_MSG_CreateString( em->msgdata, msg_connect_01_03 );
  }
  em->strbuf_num_expand = GFL_STR_CreateBuffer(NUM_EXPAND_STRBUF_SIZE, heap_id);
  em->min_num = min_num;
  em->max_num = max_num;
  em->fieldWork = fieldWork;
  em->entry_mode = entry_mode;
  em->game_type = game_type;
  em->heap_id = heap_id;
  em->draw_player_num = -1;   //最初は必ず描画されるように-1を設定しておく
  em->msg_req_id = _MSG_REQ_ID_NULL;
  MyStatus_Copy(myst, &em->mine_mystatus);
  if(parent_mac_address != NULL){
    GFL_STD_MemCopy(parent_mac_address, em->parent_mac_address, 6);
  }
  
  GFL_NET_SetNoChildErrorCheck(FALSE);  //子機が勝手にいなくなってもエラーとしない
  CommEntryMenu_AddCommandTable(em);
  
  if(game_type == COMM_ENTRY_GAMETYPE_MUSICAL){
    em->mp_mode = TRUE;
  }
  //ビーコンのマジックナンバーを設定
  {
    GFLNetInitializeStruct *netInit = GFL_NET_GetNETInitStruct();
    COMM_ENTRY_BEACON *bcn = (COMM_ENTRY_BEACON*)netInit->beaconGetFunc(GFL_NET_GetWork());
    bcn->magic_number = COMM_ENTRY_MAGIC_NUMBER;
  }
  
  return em;
}

//==================================================================
/**
 * 参加募集メニュー削除
 *
 * @param   em		
 * 
 * @retval  最終的な参加メンバーのNetIDのbit
 */
//==================================================================
u32 CommEntryMenu_Exit(COMM_ENTRY_MENU_PTR em)
{
  u32 completion_bit = 0;
  
  completion_bit = CommEntryMenu_GetCompletionBit(em);
  
  _MemberInfo_Exit(em);
  if( em->yesno.menufunc != NULL )
  {
    FLDMENUFUNC_DeleteMenu(em->yesno.menufunc);
    em->yesno.menufunc = NULL;
  }
  
  GFL_STR_DeleteBuffer(em->strbuf_num_expand);
  GFL_STR_DeleteBuffer(em->strbuf_num_template);
  GFL_STR_DeleteBuffer(em->strbuf_list_expand);
  GFL_STR_DeleteBuffer(em->strbuf_list_template);
  GFL_STR_DeleteBuffer(em->strbuf_expand);
  
  FLDMSGWIN_STREAM_Delete(em->fld_stream);
  FLDMSGBG_DeleteMSGDATA(em->msgdata);
  WORDSET_Delete( em->wordset );
  
  if(GFL_NET_IsInit() == TRUE){
    CommEntryMenu_DelCommandTable();
  }
  GFL_HEAP_FreeMemory(em);
  
  return completion_bit;
}

//==================================================================
/**
 * ユーザーの入退室コールバック設定
 *
 * @param   em		
 * @param   work          コールバック時に引数として渡すポインタ
 * @param   entry_func		入室コールバック
 * @param   leave_func		退室コールバック
 */
//==================================================================
void CommEntryMenu_SetCallback_EntryLeave(COMM_ENTRY_MENU_PTR em, void *work, COMM_ENTRY_MENU_CALLBACK entry_func, COMM_ENTRY_MENU_CALLBACK leave_func)
{
  em->callback_work = work;
  em->callback_entry = entry_func;
  em->callback_leave = leave_func;
}

//==================================================================
/**
 * プレイヤーエントリー
 *
 * @param   em		  
 * @param   netID		      ネットID
 * @param   name		      名前
 * @param   id		        ID(トレーナーカードのID)
 * @param   force_entry		TRUE:強制受け入れ。　FALSE:受け入れるか選択する
 * @param   mac_address   MACアドレス
 */
//==================================================================
void CommEntryMenu_Entry(COMM_ENTRY_MENU_PTR em, int netID, const MYSTATUS *myst, BOOL force_entry, const u8 *mac_address)
{
  COMM_ENTRY_USER *p_user = &em->user[netID];
  
  GF_ASSERT(netID <= COMM_ENTRY_USER_MAX);
  
  MyStatus_Copy(myst, &p_user->mystatus);
  p_user->status = USER_STATUS_READY;
  p_user->force_entry = force_entry;
  GFL_STD_MemCopy(mac_address, p_user->mac_address, 6);
  
  _SendBitClear(em, netID);
  _Req_SendMemberInfo(em);
  
  if(em->callback_entry != NULL){
    em->callback_entry(em->callback_work, myst, mac_address);
  }
}

//==================================================================
/**
 * 参加募集メニュー：更新処理
 *
 * @param   em		
 *
 * @retval  COMM_ENTRY_RESULT_SUCCESS
 */
//==================================================================
COMM_ENTRY_RESULT CommEntryMenu_Update(COMM_ENTRY_MENU_PTR em)
{
  BOOL ret;
  
  switch(em->entry_mode){
  case COMM_ENTRY_MODE_PARENT:
    ret = _Update_Parent(em);
    break;
  case COMM_ENTRY_MODE_CHILD:
    ret = _Update_Child(em);
    break;
  case COMM_ENTRY_MODE_CHILD_PARENT_CONNECTED:
    ret = _Update_ChildParentConnect(em);
    break;
  case COMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE:
    ret = _Update_ChildParentDesignate(em);
    break;
  }
  
  if(ret == TRUE){
    return em->entry_result;
  }
  return COMM_ENTRY_RESULT_NULL;
}

//--------------------------------------------------------------
/**
 * 更新処理：親機
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:全ての処理が終了。
 */
//--------------------------------------------------------------
static BOOL _Update_Parent(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT,
    _SEQ_INIT_SERVER,
    _SEQ_FIRST_CONNECT_WAIT,
    _SEQ_FIRST_NEGO_WAIT,
    _SEQ_ENTRY_SEND,
    _SEQ_ENTRY,
    _SEQ_FINAL_CHECK,
    _SEQ_BREAKUP_CHECK,
    _SEQ_SEND_GAMESTART_SYNC,
    _SEQ_SEND_GAMESTART_READY,
    _SEQ_WAIT_GAMESTART_READY,
    _SEQ_WAIT_GAMESTART_READY_NG,
    _SEQ_SEND_GAMESTART,
    _SEQ_SEND_GAMECANCEL,
    _SEQ_CANCEL_MSG,
    _SEQ_CANCEL_MSG_WAIT,
    _SEQ_CANCEL_WAIT,
    _SEQ_COLOSSEUM_PARENT_ONLY,
    _SEQ_COLOSSEUM_PARENT_ONLY_MSG_WAIT,
    _SEQ_FINISH,
  };

  if(em->seq > _SEQ_INIT){
    CommEntryMenu_LeaveUserUpdate(em);
    CommEntryMenu_ListUpdate(em);
    _SendUpdate_Parent(em);
  }
  
  switch(em->seq){
  case _SEQ_INIT:
    _MemberInfo_Setup(em);

    //最初のメッセージ描画
    _StreamMsgSet(em, msg_connect_02_01);
    em->seq++;
    break;
  case _SEQ_INIT_SERVER:
    if(em->game_type == COMM_ENTRY_GAMETYPE_COLOSSEUM){
      em->seq = _SEQ_ENTRY_SEND;
    }
    else{
      GFL_NET_InitServer();
      em->seq++;
    }
    break;
  case _SEQ_FIRST_CONNECT_WAIT:
    if(GFL_NET_SystemGetConnectNum() > 1){
      if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
        OS_TPrintf("ネゴシエーション送信\n");
        em->seq++;
      }
    }
    else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
      em->seq = _SEQ_BREAKUP_CHECK;
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_FIRST_NEGO_WAIT:
    if( GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) == TRUE ){
      em->seq = _SEQ_ENTRY_SEND;
    }
    break;
  case _SEQ_ENTRY_SEND:
    if(CemSend_Entry(&em->mine_mystatus, TRUE, em->mp_mode) == TRUE){
      em->seq++;
    }
    break;
  case _SEQ_ENTRY:   //募集中
    CommEntryMenu_UserStatusUpdate(em);
    
    //エントリー者を受け入れるか選択
    CommEntryMenu_ExaminationUpdate(em);
    if(CommEntryMenu_InterruptCheck(em) == TRUE){
      int entry_num = CommEntryMenu_GetCompletionNum(em);
      u16 msg_req_id = em->msg_req_id;
      
      em->msg_req_id = _MSG_REQ_ID_NULL;
      
      if(entry_num >= em->max_num
          || (entry_num >= em->min_num && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE))){
        CommEntryMenu_SetFinalUser(em);
        em->seq = _SEQ_FINAL_CHECK;
      }
      else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
        em->seq = _SEQ_BREAKUP_CHECK;
      }
      else if(em->game_type == COMM_ENTRY_GAMETYPE_COLOSSEUM && GFL_NET_GetConnectNum() <= 1){
        em->seq = _SEQ_COLOSSEUM_PARENT_ONLY;
      }
      else{
        if(msg_req_id != _MSG_REQ_ID_NULL){
          _StreamMsgSet(em, msg_req_id);
        }
      }
    }
    break;
  case _SEQ_FINAL_CHECK:   //このメンバーでよろしいですか？
    {
      DECIDE_TYPE type;
      type = CommEntryMenu_DecideUpdate(em);
      if(type == DECIDE_TYPE_OK){
        GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), FALSE);
        em->seq = _SEQ_SEND_GAMESTART_READY;
      }
      else if(type == DECIDE_TYPE_NG){
        if( CommEntryMenu_GetCompletionNum(em) >= em->max_num )
        {
          //人数最大だったらキャンセルチェック
          em->seq = _SEQ_BREAKUP_CHECK;
        }
        else{
          em->seq = _SEQ_ENTRY;
          if( CommEntryMenu_GetCompletionNum(em) > 1 &&
              em->game_type == COMM_ENTRY_GAMETYPE_MUSICAL )
          {
            em->msg_req_id = msg_connect_02_01_01;
          }
          else
          {
            em->msg_req_id = msg_connect_02_01;
          }
        }
      }
    }
    break;
  case _SEQ_BREAKUP_CHECK:  //解散しますか？
    {
      BREAKUP_TYPE type;
      type = CommEntryMenu_BreakupUpdate(em);
      if(type == BREAKUP_TYPE_OK){
        GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), FALSE);
        em->seq = _SEQ_SEND_GAMECANCEL;
      }
      else if(type == BREAKUP_TYPE_NG){
        em->seq = _SEQ_ENTRY;
      }
    }
    break;
  case _SEQ_SEND_GAMESTART_READY:
    if(CemSend_GameStartReady(em->mp_mode) == TRUE){
      CommEntryMenu_SetGameStartReady(em, GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()));
      em->wait_user_bit = em->member_info.member_bit;
      GFL_NET_SetNoChildErrorCheck(TRUE);
      em->seq = _SEQ_WAIT_GAMESTART_READY;
    }
    break;
  case _SEQ_WAIT_GAMESTART_READY:
    {
      NetID net_id;
      u8 ready_user_bit = 1 << GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()); //自分
      
      for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
        if(GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) != net_id){  //自分は無視
          if(em->wait_user_bit & (1<<net_id)){
            if(GFL_NET_IsConnectMember(net_id) == TRUE){
              if(em->recv_ready_bit & (1<<net_id)){
                ready_user_bit |= 1 << net_id;
              }
            }
            else{ //準備完了を送信したメンバーがいなくなっているので終了
              em->seq = _SEQ_WAIT_GAMESTART_READY_NG;
              break;
            }
          }
        }
      }
      
      if(em->wait_user_bit == ready_user_bit){
        if(MATH_CountPopulation(((u32)em->recv_ready_bit)) < em->min_num){
          em->seq = _SEQ_WAIT_GAMESTART_READY_NG;
        }
        else{
          em->seq = _SEQ_SEND_GAMESTART;
        }
      }
    }
    break;
  case _SEQ_WAIT_GAMESTART_READY_NG:  //ゲーム開始準備でNGなユーザーがいたので終了
    if(CemSend_GameCancel(em->mp_mode) == TRUE){
      em->wait_user_bit = em->member_info.member_bit; //_GetConnectMemberBit();
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_CANCEL_MSG;
    }
    break;
  case _SEQ_SEND_GAMESTART:
    if(CemSend_GameStart(em->mp_mode) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
      em->seq = _SEQ_SEND_GAMESTART_SYNC;
      em->game_start_num = MATH_CountPopulation(((u32)em->recv_ready_bit));
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU);
    }
    break;

  case _SEQ_SEND_GAMESTART_SYNC:
    if( em->game_start_num != GFL_NET_SystemGetConnectNum() || em->min_num > GFL_NET_SystemGetConnectNum())
    {
      //誰かが抜けた・・・
      if(CemSend_GameCancel(em->mp_mode) == TRUE){
        em->wait_user_bit = em->member_info.member_bit; //_GetConnectMemberBit();
        em->entry_result = COMM_ENTRY_RESULT_CANCEL;
        em->seq = _SEQ_CANCEL_MSG;
      }
    }
    else if( GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU) == TRUE )
    {
      em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
      em->seq = _SEQ_FINISH;
      
      //ビーコンのマジックナンバーを消す
      {
        GFLNetInitializeStruct *netInit = GFL_NET_GetNETInitStruct();
        COMM_ENTRY_BEACON *bcn = (COMM_ENTRY_BEACON*)netInit->beaconGetFunc(GFL_NET_GetWork());
        bcn->magic_number = 0;
      }
    }
    break;
  case _SEQ_SEND_GAMECANCEL:
    if(CemSend_GameCancel(em->mp_mode) == TRUE){
      em->wait_user_bit = em->member_info.member_bit; //_GetConnectMemberBit();
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_CANCEL_WAIT;
    }
    break;

  case _SEQ_CANCEL_MSG:
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup_game);
    em->seq = _SEQ_CANCEL_MSG_WAIT;
    break;
  case _SEQ_CANCEL_MSG_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      em->seq = _SEQ_CANCEL_WAIT;
    }
    break;
  case _SEQ_CANCEL_WAIT:
    //子機抜けるまで待ち
    if(_CheckConnectMemberBit(em->wait_user_bit) == FALSE){
      em->seq = _SEQ_FINISH;
    }
    break;

  case _SEQ_COLOSSEUM_PARENT_ONLY:  //コロシアムで親が一人になった
    _StreamMsgSet(em, msg_connect_07_01);
    em->seq = _SEQ_COLOSSEUM_PARENT_ONLY_MSG_WAIT;
    break;
  case _SEQ_COLOSSEUM_PARENT_ONLY_MSG_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;

  case _SEQ_FINISH:   //終了処理
    _MemberInfo_Exit(em);
    return TRUE;
  }
  
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
  {
    em->entry_result = COMM_ENTRY_RESULT_ERROR;
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 更新処理：子機
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:全ての処理が終了。
 */
//--------------------------------------------------------------
static BOOL _Update_Child(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT_TALK,
    _SEQ_INIT_TALK_WAIT,
    _SEQ_PARENT_LIST_INIT,
    _SEQ_PARENT_LIST_MAIN,
    _SEQ_MEMBER_INIT,
    _SEQ_MEMBER_WAIT,
    _SEQ_GAME_START,
    _SEQ_GAME_START_WAIT,
    _SEQ_GAME_CANCEL,
    _SEQ_GAME_CANCEL_WAIT,
    _SEQ_NG_INIT,
    _SEQ_NG_WAIT,
    _SEQ_RETRY_INIT,
    _SEQ_RETRY_WAIT,
    _SEQ_CANCEL_INIT,
    _SEQ_FINISH,
  };

  switch(em->seq){
  case _SEQ_INIT_TALK:
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_child_entry);
    em->seq++;
    break;
  case _SEQ_INIT_TALK_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->seq++;
    }
    break;
    
  case _SEQ_PARENT_LIST_INIT:
    _ParentSearchList_Exit(em);
    _MemberInfo_Exit(em);

    em->parentsearch.connect_parent = NULL;
    _ParentSearchList_Setup(em);
    GFL_NET_StartBeaconScan();
    em->seq++;
    break;
  case _SEQ_PARENT_LIST_MAIN:   //親機探索
    {
      PARENT_SEARCH_LIST_SELECT list_select = _ParentSearchList_Update(em);
      
      switch(list_select){
      case PARENT_SEARCH_LIST_SELECT_OK:
        _ParentSearchList_Exit(em);
        //終了待ちの間に親が進んだ！
        if(em->game_start == TRUE)
        {
          em->seq = _SEQ_GAME_START;
        }
        else
        if(em->game_cancel == TRUE)
        {
          em->seq = _SEQ_GAME_CANCEL;
        }
        else
        {
          WORDSET_RegisterPlayerName( em->wordset, 0, &em->parentsearch.connect_parent->mystatus );
          _StreamMsgSet(em, msg_game_connect);
          em->seq = _SEQ_MEMBER_INIT;
        }
        break;
      case PARENT_SEARCH_LIST_SELECT_NG:
        _ParentSearchList_Exit(em);
        em->seq = _SEQ_NG_INIT;
        break;
      case PARENT_SEARCH_LIST_SELECT_CANCEL:
        _ParentSearchList_Exit(em);
        em->seq = _SEQ_CANCEL_INIT;
        break;
      }
    }
    break;
    
  case _SEQ_MEMBER_INIT:      //他のメンバーの参加完了待ち
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      _StreamMsgSet(em, msg_game_wait_member);
      _MemberInfo_Setup(em);
      em->seq++;
    }
    break;
  case _SEQ_MEMBER_WAIT:
    if( _ParentWait_UpdateCnacel(em) == PARENT_WAIT_CANCEL_CANCEL )
    {
      //メッセージ無しで抜ける
      _MemberInfo_Exit(em);
      _ParentWait_ExitCnacel(em);
      em->seq = _SEQ_CANCEL_INIT;
    }
    else
    {
      if(em->forbit_leave_answer_req == TRUE && CemSend_GameStartReadyOK(em->mp_mode) == TRUE){
        em->forbit_leave_answer_req = FALSE;
      }
      _ChildEntryMember_ListRewriteUpdate(em);
      if(em->game_start == TRUE){
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_GAME_START;
      }
      else if(em->game_cancel == TRUE){
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_GAME_CANCEL;
      }
    }
    break;
  
  case _SEQ_GAME_START:
    if( em->game_type == COMM_ENTRY_GAMETYPE_COLOSSEUM )
    {
      //ミュージカルとサブウェイは外で出す
      _StreamMsgSet(em, msg_game_start);
    }
    em->seq = _SEQ_GAME_START_WAIT;
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU);
    break;
  case _SEQ_GAME_START_WAIT:
    if( em->game_type != COMM_ENTRY_GAMETYPE_COLOSSEUM ||
        FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      if( GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU) == TRUE )
      {
        em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
        em->seq = _SEQ_FINISH;
      }
      else
      if(em->game_cancel == TRUE)
      {
        em->seq = _SEQ_GAME_CANCEL;
      }
    }
    break;
    
  case _SEQ_GAME_CANCEL:
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup_game);
    em->seq = _SEQ_GAME_CANCEL_WAIT;
    break;
  case _SEQ_GAME_CANCEL_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->seq = _SEQ_CANCEL_INIT;
    }
    break;
  
  case _SEQ_NG_INIT:
    //通信復帰のためワークと初期化構造体を退避
    em->netParentWork = GFL_NET_GetWork();
    GFL_STD_MemCopy( GFL_NET_GetNETInitStruct() , &em->netInitStruct , sizeof(GFLNetInitializeStruct) );
    CommEntryMenu_DelCommandTable();
    GFL_NET_Exit(NULL);
    WORDSET_RegisterPlayerName( em->wordset, 0, &em->entry_parent_myst);  //&em->parentsearch.connect_parent->mystatus );
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_NG_WAIT;
    break;
  case _SEQ_NG_WAIT:
    if(GFL_NET_IsExit() == TRUE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->seq = _SEQ_RETRY_INIT;
    }
    break;
  case _SEQ_RETRY_INIT:
    GFL_NET_Init( &em->netInitStruct , NULL , em->netParentWork );
    em->seq = _SEQ_RETRY_WAIT;
    break;
  case _SEQ_RETRY_WAIT:
    if(GFL_NET_IsInit() == TRUE){
      CommEntryMenu_AddCommandTable(em);
      em->seq = _SEQ_INIT_TALK;
    }
    break;
  
  case _SEQ_CANCEL_INIT:      //キャンセル処理
    em->entry_result = COMM_ENTRY_RESULT_CANCEL;
    em->seq = _SEQ_FINISH;
    break;
  case _SEQ_FINISH:
    _ParentSearchList_Exit(em);
    _MemberInfo_Exit(em);
    return TRUE;
  }
    
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
  {
    _ParentWait_ExitCnacel(em);
    _ParentSearchList_Exit(em);
    em->entry_result = COMM_ENTRY_RESULT_ERROR;
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * 更新処理：子機(既に親とネゴシエーション込みで接続済み) ※コロシアム最初のメンバー
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:全ての処理が終了。
 */
//--------------------------------------------------------------
static BOOL _Update_ChildParentConnect(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT,
    _SEQ_FORCE_ENTRY_SEND,
    _SEQ_MEMBER_INIT,
    _SEQ_MEMBER_WAIT,
    _SEQ_GAME_START,
    _SEQ_GAME_START_WAIT,
    _SEQ_GAME_CANCEL,
    _SEQ_GAME_CANCEL_WAIT,
    _SEQ_NG_INIT,
    _SEQ_NG_WAIT,
    _SEQ_CANCEL_INIT,
    _SEQ_FINISH,
  };

  switch(em->seq){
  case _SEQ_INIT:
    _StreamMsgSet(em, msg_connect_08_01);
    em->seq++;
    break;
  case _SEQ_FORCE_ENTRY_SEND:
    if(CemSend_Entry(&em->mine_mystatus, TRUE, em->mp_mode) == TRUE){
      em->seq++;
    }
    break;
  
  case _SEQ_MEMBER_INIT:      //他のメンバーの参加完了待ち
    _MemberInfo_Setup(em);
    em->seq++;
    break;
  case _SEQ_MEMBER_WAIT:
    if( _ParentWait_UpdateCnacel(em) == PARENT_WAIT_CANCEL_CANCEL )
    {
      //メッセージ無しで抜ける
      _MemberInfo_Exit(em);
      _ParentWait_ExitCnacel(em);
      em->seq = _SEQ_CANCEL_INIT;
    }
    else
    {
      if(em->forbit_leave_answer_req == TRUE && CemSend_GameStartReadyOK(em->mp_mode) == TRUE){
        em->forbit_leave_answer_req = FALSE;
      }
      _ChildEntryMember_ListRewriteUpdate(em);
      if(em->game_start == TRUE){
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_GAME_START;
      }
      else if(em->game_cancel == TRUE){
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_GAME_CANCEL;
      }
    }
    break;
  
  case _SEQ_GAME_START:
    if( em->game_type == COMM_ENTRY_GAMETYPE_COLOSSEUM )
    {
      //ミュージカルとサブウェイは外で出す
      _StreamMsgSet(em, msg_game_start);
    }
    em->seq = _SEQ_GAME_START_WAIT;
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU);
    break;
  case _SEQ_GAME_START_WAIT:
    if( em->game_type != COMM_ENTRY_GAMETYPE_COLOSSEUM ||
        FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      if( GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU) == TRUE )
      {
        em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
        em->seq = _SEQ_FINISH;
      }
      else
      if(em->game_cancel == TRUE)
      {
        em->seq = _SEQ_GAME_CANCEL;
      }
    }
    break;
    
  case _SEQ_GAME_CANCEL:
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup_game);
    em->seq = _SEQ_GAME_CANCEL_WAIT;
    break;
  case _SEQ_GAME_CANCEL_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  
  case _SEQ_NG_INIT:
    GFL_NET_Exit(NULL);
    WORDSET_RegisterPlayerName( em->wordset, 0, &em->parentsearch.connect_parent->mystatus );
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_NG_WAIT;
    break;
  case _SEQ_NG_WAIT:
    if(GFL_NET_IsExit() == TRUE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_NG;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_CANCEL_INIT:      //キャンセル処理
    em->entry_result = COMM_ENTRY_RESULT_CANCEL;
    em->seq = _SEQ_FINISH;
    break;
    
  case _SEQ_FINISH:
    _MemberInfo_Exit(em);
    return TRUE;
  }
    
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
  {
    _ParentWait_ExitCnacel(em);
    em->entry_result = COMM_ENTRY_RESULT_ERROR;
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * 更新処理：子機(接続する親機を外部から指定) ※コロシアム乱入
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:全ての処理が終了。
 */
//--------------------------------------------------------------
static BOOL _Update_ChildParentDesignate(COMM_ENTRY_MENU_PTR em)
{
  enum{
    _SEQ_INIT,
    _SEQ_PARENT_ENTRY,
    _SEQ_PARENT_ANSWER_WAIT,
    _SEQ_MEMBER_INIT,      //他のメンバーの参加完了待ち
    _SEQ_MEMBER_WAIT,
    _SEQ_GAME_START,
    _SEQ_GAME_START_WAIT,
    _SEQ_GAME_CANCEL,
    _SEQ_GAME_CANCEL_WAIT,
    _SEQ_NG_INIT,
    _SEQ_NG_WAIT,
    _SEQ_CANCEL_INIT,      //キャンセル処理
    _SEQ_FINISH,
  };

  switch(em->seq){
  case _SEQ_INIT:
    _StreamMsgSet(em, msg_connect_08_01);
    em->seq++;
    break;
  case _SEQ_PARENT_ENTRY:
    em->entry_parent_answer = ENTRY_PARENT_ANSWER_NULL;
    if(CemSend_Entry(&em->mine_mystatus, FALSE, em->mp_mode) == TRUE){
      em->seq = _SEQ_PARENT_ANSWER_WAIT;
    }
    break;
  case _SEQ_PARENT_ANSWER_WAIT:
    {
      const PARENT_WAIT_CANCEL_STATE ret = _ParentWait_UpdateCnacel(em);
      if( ret == PARENT_WAIT_CANCEL_CANCEL )
      {
        //メッセージ無しで抜ける
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_CANCEL_INIT;
      }
      else
      {
        if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_OK){
          em->seq = _SEQ_MEMBER_INIT;
          _ParentWait_ExitCnacel(em);
        }
        else if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_NG){
          em->seq = _SEQ_NG_INIT;
          _ParentWait_ExitCnacel(em);
        }
        //親が最終決定をしたらすすむ
        else if( em->game_start == TRUE )
        {
          em->seq = _SEQ_GAME_START;
          _ParentWait_ExitCnacel(em);
        }
        else if( em->game_cancel == TRUE)
        {
          em->seq = _SEQ_CANCEL_INIT;
          _ParentWait_ExitCnacel(em);
        }
      }
    }
    break;
  
  case _SEQ_MEMBER_INIT:      //他のメンバーの参加完了待ち
    _MemberInfo_Setup(em);
    em->seq++;
    break;
  case _SEQ_MEMBER_WAIT:
    if( _ParentWait_UpdateCnacel(em) == PARENT_WAIT_CANCEL_CANCEL )
    {
      //メッセージ無しで抜ける
      _MemberInfo_Exit(em);
      _ParentWait_ExitCnacel(em);
      em->seq = _SEQ_CANCEL_INIT;
    }
    else
    {
      if(em->forbit_leave_answer_req == TRUE && CemSend_GameStartReadyOK(em->mp_mode) == TRUE){
        em->forbit_leave_answer_req = FALSE;
      }
      _ChildEntryMember_ListRewriteUpdate(em);
      if(em->game_start == TRUE){
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_GAME_START;
      }
      else if(em->game_cancel == TRUE){
        _MemberInfo_Exit(em);
        _ParentWait_ExitCnacel(em);
        em->seq = _SEQ_GAME_CANCEL;
      }
    }
    break;
  
  case _SEQ_GAME_START:
    if( em->game_type == COMM_ENTRY_GAMETYPE_COLOSSEUM )
    {
      //ミュージカルとサブウェイは外で出す
      _StreamMsgSet(em, msg_game_start);
    }
    em->seq = _SEQ_GAME_START_WAIT;
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU);
    break;
  case _SEQ_GAME_START_WAIT:
    if( em->game_type != COMM_ENTRY_GAMETYPE_COLOSSEUM ||
        FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      if( GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),GAMESTART_SYNC_NO,WB_NET_COMM_ENTRY_MENU) == TRUE )
      {
        em->entry_result = COMM_ENTRY_RESULT_SUCCESS;
        em->seq = _SEQ_FINISH;
      }
      else
      if(em->game_cancel == TRUE)
      {
        em->seq = _SEQ_GAME_CANCEL;
      }
    }
    break;
    
  case _SEQ_GAME_CANCEL:
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup_game);
    em->seq = _SEQ_GAME_CANCEL_WAIT;
    break;
  case _SEQ_GAME_CANCEL_WAIT:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_CANCEL;
      em->seq = _SEQ_FINISH;
    }
    break;
  
  case _SEQ_NG_INIT:
    GFL_NET_Exit(NULL);
    WORDSET_RegisterPlayerName( em->wordset, 0, &em->entry_parent_myst);  //&em->parentsearch.connect_parent->mystatus );
    _StreamMsgSet(em, msg_game_ng);
    em->seq = _SEQ_NG_WAIT;
    break;
  case _SEQ_NG_WAIT:
    if(GFL_NET_IsExit() == TRUE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
      em->entry_result = COMM_ENTRY_RESULT_NG;
      em->seq = _SEQ_FINISH;
    }
    break;
    
  case _SEQ_CANCEL_INIT:      //キャンセル処理
    em->entry_result = COMM_ENTRY_RESULT_CANCEL;
    em->seq = _SEQ_FINISH;
    break;
    
  case _SEQ_FINISH:
    _MemberInfo_Exit(em);
    return TRUE;
  }
    
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
  {
    _ParentWait_ExitCnacel(em);
    em->entry_result = COMM_ENTRY_RESULT_ERROR;
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * 送信データ更新：親機用
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _SendUpdate_Parent(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->send_bit_entry_ok & (1 << net_id)){
      if(CemSend_EntryOK(net_id, em->mp_mode, &em->mine_mystatus) == TRUE){
        em->send_bit_entry_ok ^= 1 << net_id;
      }
    }
    else if(em->send_bit_entry_ng & (1 << net_id)){
      if(CemSend_EntryNG(net_id, em->mp_mode, &em->mine_mystatus) == TRUE){
        em->send_bit_entry_ng ^= 1 << net_id;
      }
    }
  }

  if(em->update_member_info == TRUE && em->member_info_sending == FALSE){
    u8 send_bit = _MemberInfoSendBufCreate(em);
    if(send_bit > 1){
      if(CemSend_MemberInfo(&em->member_info, send_bit, em->mp_mode) == TRUE){
        em->update_member_info = FALSE;
        em->member_info_sending = TRUE;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * 最新のメンバー情報を送信バッファに反映
 *
 * @param   em		
 *
 * @retval  u8		送信相手bit
 */
//--------------------------------------------------------------
static u8 _MemberInfoSendBufCreate(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  u8 send_bit = 0;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->user[net_id].status != USER_STATUS_NULL){
      MyStatus_Copy(&em->user[net_id].mystatus, &em->member_info.mystatus[net_id]);
      send_bit |= 1 << net_id;
    }
  }
  em->member_info.member_bit = send_bit;
  return (send_bit | 1);  //送信bitに親機の情報は必ず含める
}

//--------------------------------------------------------------
/**
 * ユーザーが居なくなっていないか更新
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_LeaveUserUpdate(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;

  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->user[net_id].status != USER_STATUS_NULL && GFL_NET_IsConnectMember(net_id) == FALSE){
      OS_TPrintf("%d番のプレイヤーがいなくなった Connect=%d SystemNum=%d status=%d\n", net_id, GFL_NET_GetConnectNum(), GFL_NET_SystemGetConnectNum(), em->user[net_id].status);
      if(em->callback_leave != NULL){
        em->callback_leave(em->callback_work, 
          &em->user[net_id].mystatus, em->user[net_id].mac_address);
      }
      CommEntryMenu_EraseUser(em, net_id);
      _SendBitClear(em, net_id);
      _Req_SendMemberInfo(em);
    }
  }
}

//--------------------------------------------------------------
/**
 * リスト更新処理
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_ListUpdate(COMM_ENTRY_MENU_PTR em)
{
  int net_id, player_num = 0;

  //リストの名前描画
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->user[net_id].status != USER_STATUS_NULL){
      _ParentEntry_NameDraw(em,  &em->user[net_id], net_id);
      player_num++;
    }
    else{
      _ParentEntry_NameErase(em, &em->user[net_id], net_id);
    }
  }
  if( player_num == 0 )
  {
    //自分は必ず出しておく
    _MemberInfo_NameDraw(em, &em->mine_mystatus, 0);
    player_num++;
  }
  if(em->draw_player_num != player_num){
    _ParentEntry_NumDraw(em, player_num);
    em->draw_player_num = player_num;
    
    //プレイヤーが減った時だけ
    if( CommEntryMenu_InterruptCheck(em) == TRUE &&
        em->yesno.menufunc == NULL &&
        em->game_type == COMM_ENTRY_GAMETYPE_MUSICAL &&
        player_num == 1 )
    {
      _StreamMsgSet(em, msg_connect_02_01);
    }
  }
}

//--------------------------------------------------------------
/**
 * ステータス更新
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_UserStatusUpdate(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  COMM_ENTRY_USER *user = em->user;

  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    //現プレイヤーのステータス更新
    switch(user->status){
    case USER_STATUS_READY:
      if(user->force_entry == TRUE){
        user->status = USER_STATUS_COMPLETION;
        em->send_bit_entry_ok |= 1 << net_id;
      }
      else{
        user->status = USER_STATUS_EXAMINATION;
      }
      break;
    case USER_STATUS_EXAMINATION:
      if(CommEntryMenu_SetExamination(em, user) == TRUE){
        user->status = USER_STATUS_EXAMINATION_WAIT;
      }
      break;
    }
    
    user++;
  }
}

//--------------------------------------------------------------
/**
 * 対象ユーザーを消す
 *
 * @param   em		
 * @param   net_id		NetID
 */
//--------------------------------------------------------------
static void CommEntryMenu_EraseUser(COMM_ENTRY_MENU_PTR em, NetID net_id)
{
  em->user[net_id].status = USER_STATUS_NULL;
}

//--------------------------------------------------------------
/**
 * 審査対象にユーザーを登録する
 *
 * @param   em		
 * @param   user		
 *
 * @retval  BOOL		TRUE:登録成功
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_SetExamination(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user)
{
  if(em->examination_occ == FALSE){
    em->examination_user = *user;
    em->examination_occ = TRUE;
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * エントリー者を受け入れるか選択
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_ExaminationUpdate(COMM_ENTRY_MENU_PTR em)
{
  COMM_ENTRY_USER *user = &em->examination_user;
  COMM_ENTRY_YESNO *yesno = &em->yesno;
  FLDMSGBG *fldmsg_bg;
  NetID net_id;
  
  if(em->examination_occ == FALSE || FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return;
  }
  
  switch(yesno->seq){
  case 0:
    WORDSET_RegisterPlayerName( em->wordset, 0, &user->mystatus );
    _StreamMsgSet(em, msg_connect_02_02);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      int status;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      else if(ret == FLDMENUFUNC_YESNO_YES){
        status = USER_STATUS_COMPLETION;
      }
      else{
        status = USER_STATUS_REFUSE;
      }
      for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
        if(em->user[net_id].status == USER_STATUS_EXAMINATION_WAIT){
          em->user[net_id].status = status;
          if(status == USER_STATUS_COMPLETION){
            em->send_bit_entry_ok |= 1 << net_id;
          }
          else{
            em->send_bit_entry_ng |= 1 << net_id;
          }
          break;
        }
      }
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      //決定とメッセージ送りが同時に来るので1フレーム待つ
      yesno->menufunc = NULL;
      yesno->seq++;
    }
    break;
  case 3:
    em->examination_occ = FALSE;
    yesno->seq = 0;

    //連続で聞くためチェック
    CommEntryMenu_UserStatusUpdate(em);
    if( em->examination_occ == FALSE )
    {
      if( CommEntryMenu_GetCompletionNum(em) > 1 &&
          em->game_type == COMM_ENTRY_GAMETYPE_MUSICAL )
      {
        em->msg_req_id = msg_connect_02_01_01;
      }
      else
      {
        em->msg_req_id = msg_connect_02_01;
      }
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * 最終決定処理
 *
 * @param   em		
 *
 * @retval  
 */
//--------------------------------------------------------------
static DECIDE_TYPE CommEntryMenu_DecideUpdate(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg;
  COMM_ENTRY_YESNO *yesno = &em->yesno;

  if(FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return DECIDE_TYPE_NULL;
  }

  switch(yesno->seq){
  case 0:
    _StreamMsgSet(em, msg_connect_02_03);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      
      if(CommEntryMenu_CheckFinalUser(em) == FALSE){
        ret = FLDMENUFUNC_YESNO_NO; //ユーザーの状況に変更があったため、最終決定をキャンセルする
      }
      else{
        ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
        if(ret == FLDMENUFUNC_YESNO_NULL){
          break;
        }
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      yesno->seq = 0;
      if(ret == FLDMENUFUNC_YESNO_YES){
        return DECIDE_TYPE_OK;
      }
      else{
        return DECIDE_TYPE_NG;
      }
    }
    break;
  }
  return DECIDE_TYPE_NULL;
}

//--------------------------------------------------------------
/**
 * 最終決定処理
 *
 * @param   em		
 *
 * @retval  
 */
//--------------------------------------------------------------
static BREAKUP_TYPE CommEntryMenu_BreakupUpdate(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg;
  COMM_ENTRY_YESNO *yesno = &em->yesno;

  if(FLDMSGWIN_STREAM_Print(em->fld_stream) == FALSE){
    return BREAKUP_TYPE_NULL;
  }

  switch(yesno->seq){
  case 0: //解散しますか？
    _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup_confirm);
    yesno->seq++;
    break;
  case 1:
    GF_ASSERT(yesno->menufunc == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 2:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      
      if(ret == FLDMENUFUNC_YESNO_NO){
        yesno->breakup_type = BREAKUP_TYPE_NG;
        yesno->seq = 100;
      }
      else{
        _StreamMsgSet(em, msg_connect_04_01);
        yesno->seq++;
      }
    }
    break;
  case 3: //本当に解散しますか？
    fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
    yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    yesno->seq++;
    break;
  case 4:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(ret == FLDMENUFUNC_YESNO_NULL){
        break;
      }
      
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
      if(ret == FLDMENUFUNC_YESNO_YES){
        _StreamMsgSet(em, _GameTypeMsgPack[em->game_type].msgid_breakup);
        yesno->seq++;
      }
      else{
        yesno->breakup_type = BREAKUP_TYPE_NG;
        yesno->seq = 100;
      }
    }
    break;
  case 5:
    yesno->seq = 0xff;
    yesno->breakup_type = BREAKUP_TYPE_OK;
    break;
  case 100: //やっぱ解散はしない
    if( CommEntryMenu_GetCompletionNum(em) > 1 &&
        em->game_type == COMM_ENTRY_GAMETYPE_MUSICAL )
    {
      em->msg_req_id = msg_connect_02_01_01;
    }
    else
    {
      em->msg_req_id = msg_connect_02_01;
    }
    yesno->seq = 0xff;
    break;
  case 0xff:
    if(yesno->menufunc != NULL){
      FLDMENUFUNC_DeleteMenu(yesno->menufunc);
      yesno->menufunc = NULL;
    }
    yesno->seq = 0;
    return yesno->breakup_type;
  }
  return BREAKUP_TYPE_NULL;
}

//==================================================================
/**
 * ゲーム開始前の準備完了フラグをセット
 *
 * @param   em		
 * @param   net_id  準備が完了した人のNetID
 */
//==================================================================
void CommEntryMenu_SetGameStartReady(COMM_ENTRY_MENU_PTR em, NetID net_id)
{
  em->recv_ready_bit |= 1 << net_id;
}

//==================================================================
/**
 * ゲーム開始フラグセット
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_SetGameStart(COMM_ENTRY_MENU_PTR em)
{
  em->game_start = TRUE;
}

//==================================================================
/**
 * ゲームキャンセルフラグセット
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_SetGameCancel(COMM_ENTRY_MENU_PTR em)
{
  em->game_cancel = TRUE;
}

//==================================================================
/**
 * 離脱禁止フラグをセット
 *
 * @param   em		
 */
//==================================================================
void CommEntryMenu_SetForbitLeave(COMM_ENTRY_MENU_PTR em)
{
  em->forbit_leave = TRUE;
  em->forbit_leave_answer_req = TRUE;
  GFL_NET_SetNoChildErrorCheck(TRUE);
}

//==================================================================
/**
 * エントリー結果の取得
 *
 * @param   em		
 * @param   netID		
 * @param   exit_mode   TRUE:親が参加メンバーを決定後、まだ返事を送信していないメンバーを取得するモードです(FALSEの時の違いは、TRUEの場合は審査にすら漏れた者の状態も返す事です)
 *
 * @retval  COMM_ENTRY_ANSWER		
 *
 * 送信済みフラグが立っているものはNULLを返します
 */
//==================================================================
COMM_ENTRY_ANSWER CommEntryMenu_GetAnswer(COMM_ENTRY_MENU_PTR em, int netID, BOOL exit_mode)
{
  if(em->user[netID].send_flag == FALSE){
    if(em->user[netID].status == USER_STATUS_COMPLETION){
      return COMM_ENTRY_ANSWER_COMPLETION;
    }
    else if(em->user[netID].status == USER_STATUS_REFUSE){
//      return COMM_ENTRY_ANSWER_REFUSE;
      return COMM_ENTRY_ANSWER_COMPLETION;
    }
    
    if(exit_mode == TRUE){
      if(em->user[netID].status != USER_STATUS_NULL){
        return COMM_ENTRY_ANSWER_REFUSE;
      }
    }
  }
  return COMM_ENTRY_ANSWER_NULL;
}

//==================================================================
/**
 * 送信済みフラグを立てる
 *
 * @param   em		
 * @param   netID		
 *
 * CommEntryMenu_GetAnswerで取得した返事を通信相手に送信したら、これでフラグを立ててください
 * CommEntryMenu_GetAnswerでNULLが返るようになるので2重送信などが起きなくなります
 * 結果が「拒否」だった場合はリストから削除します
 */
//==================================================================
void CommEntryMenu_SetSendFlag(COMM_ENTRY_MENU_PTR em, int netID)
{
  GF_ASSERT(em->user[netID].status == USER_STATUS_COMPLETION || em->user[netID].status == USER_STATUS_REFUSE);
  
  if(em->user[netID].status == USER_STATUS_COMPLETION){
    em->user[netID].send_flag = TRUE;
  }
  else if(em->user[netID].status == USER_STATUS_REFUSE){
    _ParentEntry_NameErase(em, &em->user[netID], netID);
    GFL_STD_MemClear(&em->user[netID], sizeof(COMM_ENTRY_USER));
  }
}

//==================================================================
/**
 * エントリー完了した人数を取得する
 *
 * @param   em		
 *
 * @retval  int		エントリー完了人数
 */
//==================================================================
int CommEntryMenu_GetCompletionNum(COMM_ENTRY_MENU_PTR em)
{
  int i, count = 0;
  
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    if(em->user[i].status == USER_STATUS_COMPLETION){
      count++;
    }
  }
  return count;
}

//==================================================================
/**
 * エントリー完了している人のNetIDをbitで取得する
 *
 * @param   em		
 *
 * @retval  u32		エントリー完了している人のNetIDのビット
 *                (NetID0番と2番の人がエントリー完了しているならば2進数で[0101]
 */
//==================================================================
u32 CommEntryMenu_GetCompletionBit(COMM_ENTRY_MENU_PTR em)
{
  u32 comp_bit = 0;
  int i;
  
  for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
    if(em->user[i].status == USER_STATUS_COMPLETION){
      comp_bit |= 1 << i;
    }
  }
  return comp_bit;
}

//--------------------------------------------------------------
/**
 * 何の選択も行っていないフリーの状況か調べる
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE：フリー
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_InterruptCheck(COMM_ENTRY_MENU_PTR em)
{
  if(em->examination_occ == FALSE && FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 参加メンバー情報リストセットアップ
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _MemberInfo_Setup(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
  int bmpwin_y_size;
  
  GF_ASSERT(em->fldmsgwin_list == NULL);
  GF_ASSERT(em->fldmsgwin_title == NULL);
  
  if(em->entry_mode == COMM_ENTRY_MODE_PARENT){
    bmpwin_y_size = LISTBMP_POS_BOTTOM_PARENT;
  }
  else{
    bmpwin_y_size = LISTBMP_POS_BOTTOM_CHILD;
  }
  
  em->fldmsgwin_list = FLDMSGWIN_Add( fldmsg_bg, em->msgdata, 
    LISTBMP_POS_LEFT, LISTBMP_POS_TOP, 
    LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT, bmpwin_y_size - LISTBMP_POS_TOP);
  em->fldmsgwin_title = FLDMSGWIN_Add( fldmsg_bg, em->msgdata, 
    TITLEBMP_POS_LEFT, TITLEBMP_POS_TOP, 
    TITLEBMP_POS_RIGHT - TITLEBMP_POS_LEFT, TITLEBMP_POS_BOTTOM - TITLEBMP_POS_TOP);

  FLDMSGWIN_Print( em->fldmsgwin_title, 0, 0, _GameTypeMsgPack[em->game_type].msgid_title );
}

//--------------------------------------------------------------
/**
 * 参加メンバー情報リスト破棄
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _MemberInfo_Exit(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
  if(em->fldmsgwin_title != NULL){
    FLDMSGWIN_Delete(em->fldmsgwin_title);
    em->fldmsgwin_title = NULL;
  }
  if(em->fldmsgwin_list != NULL){
    FLDMSGWIN_Delete(em->fldmsgwin_list);
    em->fldmsgwin_list = NULL;
  }
  FLDMSGBG_ClearPrintQue( fldmsg_bg );
}

//--------------------------------------------------------------
/**
 * 残り参加人数描画
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _ParentEntry_NumDraw(COMM_ENTRY_MENU_PTR em, int player_num)
{
  if(em->fldmsgwin_list != NULL){
    FLDMSGWIN_FillClearWindow(em->fldmsgwin_list, 
      LISTBMP_START_NUM_X, LISTBMP_START_NUM_Y, 
      (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NUM_Y + 8*2);

    WORDSET_RegisterNumber( em->wordset, 0, em->max_num - player_num, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( em->wordset, em->strbuf_num_expand, em->strbuf_num_template );
    FLDMSGWIN_PrintStrBuf(em->fldmsgwin_list, 
      LISTBMP_START_NUM_X, LISTBMP_START_NUM_Y, em->strbuf_num_expand);
  }
}

//--------------------------------------------------------------
/**
 * 名前描画
 *
 * @param   em		
 * @param   user		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _ParentEntry_NameDraw(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID)
{
  if(GFL_STD_MemComp(user->mac_address, em->draw_mac[netID], 6) == 0){
    return; //既に描画済み
  }
  
//  _ParentEntry_NameErase(em, user, netID); //まず既に書かれている名前を消去
  
  _MemberInfo_NameDraw(em, &user->mystatus, netID);

  GFL_STD_MemCopy(user->mac_address, em->draw_mac[netID], 6);
}

//--------------------------------------------------------------
/**
 * 名前消去
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _ParentEntry_NameErase(COMM_ENTRY_MENU_PTR em, COMM_ENTRY_USER *user, int netID)
{
  if(GFL_STD_MemComp(em->draw_mac[netID], _NullMacAddress, 6) == 0){
    return;
  }
  
  _MemberInfo_NameErase(em, netID);
  GFL_STD_MemCopy(_NullMacAddress, em->draw_mac[netID], 6);
  
  OS_TPrintf("Name Erase = %d, %d, %d, %d\n",     
    LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, 
    (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, LISTBMP_START_NAME_Y + netID*8*4);
}

//--------------------------------------------------------------
/**
 * 名前描画
 *
 * @param   em		
 * @param   myst
 */
//--------------------------------------------------------------
static void _MemberInfo_NameDraw(COMM_ENTRY_MENU_PTR em, const MYSTATUS *myst, NetID netID)
{
  if(em->fldmsgwin_list != NULL){
    WORDSET_RegisterPlayerName( em->wordset, 0, myst );
    WORDSET_RegisterNumber( em->wordset, 1, MyStatus_GetID_Low(myst), 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( em->wordset, em->strbuf_list_expand, em->strbuf_list_template );
    FLDMSGWIN_PrintStrBuf(em->fldmsgwin_list, 
      LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, em->strbuf_list_expand);
  }
  else{
    GF_ASSERT(0);
  }
}

//--------------------------------------------------------------
/**
 * 名前消去
 *
 * @param   em		
 * @param   netID		
 */
//--------------------------------------------------------------
static void _MemberInfo_NameErase(COMM_ENTRY_MENU_PTR em, int netID)
{
  if(em->fldmsgwin_list != NULL){
    FLDMSGWIN_FillClearWindow(em->fldmsgwin_list, 
      LISTBMP_START_NAME_X, LISTBMP_START_NAME_Y + netID*8*2, 
      (LISTBMP_POS_RIGHT - LISTBMP_POS_LEFT) * 8, 8*2);
  }
}

//--------------------------------------------------------------
/**
 * 参加者情報全体を再描画(子機用)
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ChildEntryMember_ListRewriteUpdate(COMM_ENTRY_MENU_PTR em)
{
  int net_id;
  
  if(em->member_info_recv == FALSE){
    return;
  }
  em->member_info_recv = FALSE;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    _MemberInfo_NameErase(em, net_id);
    if(em->member_info.member_bit & (1 << net_id)){
      _MemberInfo_NameDraw(em, &em->member_info.mystatus[net_id], net_id);
    }
  }
}

//--------------------------------------------------------------
/**
 * メッセージストリーム描画
 *
 * @param   em		
 * @param   msg_id		メッセージID
 *
 * WORDSETはあらかじめセットした上で呼んでください
 */
//--------------------------------------------------------------
static void _StreamMsgSet(COMM_ENTRY_MENU_PTR em, u32 msg_id)
{
  STRBUF *temp_strbuf;
  
  temp_strbuf = GFL_MSG_CreateString( em->msgdata, msg_id );
  WORDSET_ExpandStr( em->wordset, em->strbuf_expand, temp_strbuf );
  GFL_STR_DeleteBuffer(temp_strbuf);
  
  FLDMSGWIN_STREAM_ClearMessage(em->fld_stream);
  FLDMSGWIN_STREAM_PrintStrBufStart(em->fld_stream, 0, 0, em->strbuf_expand);
}

//--------------------------------------------------------------
/**
 * 現ユーザーを最終候補者ユーザーとしてセットする
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void CommEntryMenu_SetFinalUser(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    em->final_user_status[net_id] = em->user[net_id].status;
  }
}

//--------------------------------------------------------------
/**
 * 最終候補者ユーザーと現ユーザーのステータスが一致しているか調べる
 *
 * @param   em		  
 *
 * @retval  BOOL		TRUE:一致している　FALSE:一致していない
 */
//--------------------------------------------------------------
static BOOL CommEntryMenu_CheckFinalUser(COMM_ENTRY_MENU_PTR em)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(em->final_user_status[net_id] != em->user[net_id].status){
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 指定NetIDに対する送信bitを全てクリアする
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _SendBitClear(COMM_ENTRY_MENU_PTR em, NetID netID)
{
  em->send_bit_entry_ok &= 0xff ^ (1 << netID);
  em->send_bit_entry_ng &= 0xff ^ (1 << netID);
}

//--------------------------------------------------------------
/**
 * メンバー情報送信リクエスト
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _Req_SendMemberInfo(COMM_ENTRY_MENU_PTR em)
{
  if(em->update_member_info == TRUE || em->member_info_sending == TRUE){
    //巨大データの為、既に送信リクエストがかかっている場合は、
    //送信中である、という事を考慮して予約という形でリクエストをかけておく
    em->update_member_info_reserve = TRUE;
  }
  else{
    em->update_member_info = TRUE;
    em->update_member_info_reserve = FALSE;
  }
}

//==================================================================
/**
 * メンバー情報を親機が受信した時に、次のメンバー情報送信リクエストがかかっていれば反映させる
 *
 * @param   em		
 */
//==================================================================
void CommEntyrMenu_MemberInfoReserveUpdate(COMM_ENTRY_MENU_PTR em)
{
  GF_ASSERT(em->member_info_sending == TRUE);
  em->member_info_sending = FALSE;
  em->update_member_info = em->update_member_info_reserve;
  em->update_member_info_reserve = FALSE;
}

//==================================================================
/**
 * メンバー情報のバッファポインタを取得
 *
 * @param   em		
 *
 * @retval  ENTRYMENU_MEMBER_INFO *		
 */
//==================================================================
ENTRYMENU_MEMBER_INFO * CommEntryMenu_GetMemberInfo(COMM_ENTRY_MENU_PTR em)
{
  return &em->member_info;
}

//==================================================================
/**
 * メンバー情報を受信した時のセット関数
 *
 * @param   em		
 * @param   member_info		受信したメンバー情報
 */
//==================================================================
void CommEntryMenu_RecvMemberInfo(COMM_ENTRY_MENU_PTR em, const ENTRYMENU_MEMBER_INFO *member_info)
{
  GFL_STD_MemCopy(member_info, &em->member_info, sizeof(ENTRYMENU_MEMBER_INFO));
  em->member_info_recv = TRUE;
}


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * 親機探索リストシステムセットアップ
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_Setup(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *field_msgbg = FIELDMAP_GetFldMsgBG(em->fieldWork);

  GF_ASSERT(em->parentsearch.menufunc == NULL);
  
  em->parentsearch.fldmenu_listdata = FLDMENUFUNC_CreateListData( 
    PARENT_LIST_MAX, em->heap_id );
  _ParentSearchList_SetListString(em);
  em->parentsearch.menufunc = FLDMENUFUNC_AddMenu( field_msgbg,
	  &ParentSearchMenuListHeader, em->parentsearch.fldmenu_listdata );

  em->parentsearch.local_seq = 0;
}

//--------------------------------------------------------------
/**
 * 親機探索リストシステム終了
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_Exit(COMM_ENTRY_MENU_PTR em)
{
  if(em->parentsearch.menufunc != NULL){
    FLDMENUFUNC_DeleteMenu(em->parentsearch.menufunc);
    em->parentsearch.menufunc = NULL;
    em->parentsearch.list_strbuf_create = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * 親機探索リストシステム更新
 *
 * @param   em		
 *
 * @retval  PARENT_SEARCH_LIST_SELECT		
 */
//--------------------------------------------------------------
static PARENT_SEARCH_LIST_SELECT _ParentSearchList_Update(COMM_ENTRY_MENU_PTR em)
{
  FLDMSGBG *fldmsg_bg;
  COMM_ENTRY_YESNO *yesno = &em->yesno;
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  BOOL ret;
  enum{
    _SEQ_UPDATE,
    _SEQ_NEGO_WAIT,
    _SEQ_PARENT_ANSWER_WAIT,
    _SEQ_FINISH,
    
    _SEQ_YESNO_INIT,
    _SEQ_YESNO_MAIN,
    
    _SEQ_WAIT_TIMEOUT_MSG,
  };
  
  switch(psl->local_seq){
  case _SEQ_UPDATE:
    if(psl->connect_parent == NULL){
      _ParentSearchList_ParentLifeUpdate(em);
      _ParentSearchList_BeaconUpdate(em);
      if(_ParentSearchList_ListSelectUpdate(em) == FALSE){
        psl->final_select = PARENT_SEARCH_LIST_SELECT_CANCEL;
        psl->local_seq = _SEQ_FINISH;
        break;
      }
      
      if(em->parentsearch.list_update_req == TRUE){
        _ParentSearchList_SetListString(em);
        FLDMENUFUNC_Rewrite( em->parentsearch.menufunc );
        em->parentsearch.list_update_req = FALSE;
      }
    }
    else
    {
      if(GFL_NET_SystemGetConnectNum() > 1)
      {
        if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
        {
          OS_TPrintf("ネゴシエーション送信\n");
          psl->local_seq++;
        }
      }
      else
      {
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
        {
          psl->return_seq = psl->local_seq;
          psl->local_seq = _SEQ_YESNO_INIT;
        }
        //一定時間経っても親機に接続出来なかったらキャンセル扱いにする
        psl->timeoutCnt++;
        if( psl->timeoutCnt >= PARENT_LIST_TIMEOUT )
        {
          psl->local_seq = _SEQ_WAIT_TIMEOUT_MSG;
          _StreamMsgSet(em, msg_game_connect_timeout);
        }
      }
    }
    break;
  case _SEQ_NEGO_WAIT: //ネゴシエーション完了待ち
    if( GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) == TRUE ){
      OS_TPrintf("ネゴシエーション完了\n");
      em->entry_parent_answer = ENTRY_PARENT_ANSWER_NULL;
      if(CemSend_Entry(&em->mine_mystatus, FALSE, em->mp_mode) == TRUE){
        psl->local_seq = _SEQ_PARENT_ANSWER_WAIT;
        OS_TPrintf("親にエントリーを送信\n");
      }
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
      psl->return_seq = psl->local_seq;
      psl->local_seq = _SEQ_YESNO_INIT;
    }
    //タイミングよく親が抜けた
    if(em->game_cancel == TRUE)
    {
      //キャンセル選択中に親が進んだ処理と一緒の流れに入れるためOKを帰す
      psl->final_select = PARENT_SEARCH_LIST_SELECT_OK;
      psl->local_seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_PARENT_ANSWER_WAIT: 
    if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_OK){
      OS_TPrintf("親からのエントリー返事：OK\n");
      psl->final_select = PARENT_SEARCH_LIST_SELECT_OK;
      psl->local_seq = _SEQ_FINISH;
    }
    else if(em->entry_parent_answer == ENTRY_PARENT_ANSWER_NG){
      OS_TPrintf("親からのエントリー返事：NG\n");
      psl->final_select = PARENT_SEARCH_LIST_SELECT_NG;
      psl->local_seq = _SEQ_FINISH;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
      psl->return_seq = psl->local_seq;
      psl->local_seq = _SEQ_YESNO_INIT;
    }
    //タイミングよく親が抜けた
    if(em->game_cancel == TRUE)
    {
      //キャンセル選択中に親が進んだ処理と一緒の流れに入れるためOKを帰す
      psl->final_select = PARENT_SEARCH_LIST_SELECT_OK;
      psl->local_seq = _SEQ_FINISH;
    }
    break;
  case _SEQ_FINISH:
    return psl->final_select;
    
  case _SEQ_YESNO_INIT:
    _StreamMsgSet(em, msg_game_exit);
    {
      fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
      yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
    }
    psl->local_seq = _SEQ_YESNO_MAIN;
    break;

  case _SEQ_YESNO_MAIN:
  
    {
      FLDMENUFUNC_YESNO ret;
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(FLDMSGBG_CheckFinishPrint( FIELDMAP_GetFldMsgBG(em->fieldWork) ) == TRUE
          && em->forbit_leave == TRUE){
        //選択可能な状態で離脱禁止フラグが立っている場合は強制でNOを選んだことにする
        ret = FLDMENUFUNC_YESNO_NO;
      }
      if(ret == FLDMENUFUNC_YESNO_NULL && FLDMSGBG_CheckFinishPrint( FIELDMAP_GetFldMsgBG(em->fieldWork) ) == TRUE)
      {
        //終了待ちの間に親が進んだ！
        if(em->game_start == TRUE||
           em->game_cancel == TRUE)
        {
          FLDMENUFUNC_DeleteMenu(yesno->menufunc);
          yesno->menufunc = NULL;
          psl->final_select = PARENT_SEARCH_LIST_SELECT_OK;
          psl->local_seq = _SEQ_FINISH;
        }
      }
      else
      {
        FLDMENUFUNC_DeleteMenu(yesno->menufunc);
        yesno->menufunc = NULL;
        
        if(ret == FLDMENUFUNC_YESNO_NO)
        {
          psl->local_seq = psl->return_seq;
          _StreamMsgSet(em, msg_game_wait_parent);
        }
        else
        {
          psl->final_select = PARENT_SEARCH_LIST_SELECT_CANCEL;
          psl->local_seq = _SEQ_FINISH;
        }
      }
    }
    break;

  case _SEQ_WAIT_TIMEOUT_MSG:
    if(FLDMSGWIN_STREAM_Print(em->fld_stream) == TRUE)
    {
      psl->final_select = PARENT_SEARCH_LIST_SELECT_CANCEL;
      psl->local_seq = _SEQ_FINISH;
    }
    break;
  }
  
  return PARENT_SEARCH_LIST_SELECT_NULL;
}

//--------------------------------------------------------------
/**
 * 親機探索リスト選択更新処理
 *
 * @param   em		
 *
 * @retval  BOOL		TRUE:選択中　FALSE:キャンセルを押した
 */
//--------------------------------------------------------------
static BOOL _ParentSearchList_ListSelectUpdate(COMM_ENTRY_MENU_PTR em)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  u32 select_ret;
  
  select_ret = FLDMENUFUNC_ProcMenu( psl->menufunc );
  switch(select_ret){
  case FLDMENUFUNC_NULL:
    break;
  case FLDMENUFUNC_CANCEL:
    return FALSE;
  default:
    if(psl->parentuser[select_ret].occ == TRUE){
      GF_ASSERT(psl->connect_parent == NULL);
      psl->connect_parent = &psl->parentuser[select_ret];
      GFL_NET_ConnectToParent(psl->connect_parent->mac_address);
      WORDSET_RegisterPlayerName( em->wordset, 0, &psl->connect_parent->mystatus );
      _StreamMsgSet(em, msg_game_wait_parent);
      psl->timeoutCnt = 0;
    }
    break;
  }
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 親機情報の寿命更新
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_ParentLifeUpdate(COMM_ENTRY_MENU_PTR em)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  int i;

  for(i = 0; i < PARENT_LIST_MAX; i++){
    if(psl->parentuser[i].occ == TRUE){
      psl->parentuser[i].life--;
      if(psl->parentuser[i].life <= 0){
        _ParentSearchList_DeleteParent(em, i);
        psl->list_update_req = TRUE;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ビーコンを取得してリストを更新
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_BeaconUpdate(COMM_ENTRY_MENU_PTR em)
{
  int i;
  COMM_ENTRY_BEACON *bcon_buff;
  
  for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++)
  {
  	bcon_buff = GFL_NET_GetBeaconData(i);
  	if(bcon_buff != NULL )
  	{
      if( bcon_buff->magic_number == COMM_ENTRY_MAGIC_NUMBER )
      {
        _ParentSearchList_SetNewParent(em, bcon_buff->mac_address, &bcon_buff->mystatus);
      }
  	}
  }
}

//--------------------------------------------------------------
/**
 * 新しい親機をエントリーする
 *
 * @param   em		
 * @param   mac_address		
 * @param   myst		
 */
//--------------------------------------------------------------
static void _ParentSearchList_SetNewParent(COMM_ENTRY_MENU_PTR em, const u8 *mac_address, const MYSTATUS *myst)
{
  PARENTSEARCH_LIST *psl = &em->parentsearch;
  int i;
  
  //既に登録済みでないかチェック
  for(i = 0; i < PARENT_LIST_MAX; i++){
    if(psl->parentuser[i].occ == TRUE){
      if(GFL_STD_MemComp(psl->parentuser[i].mac_address, mac_address, 6) == 0){
        psl->parentuser[i].life = PARENT_LIST_LIFE; //寿命だけ回復
        return; //既に登録済み
      }
    }
  }
  //空いている場所の一番上に登録
  for(i = 0; i < PARENT_LIST_MAX; i++){
    if(psl->parentuser[i].occ == FALSE){
      GFL_STD_MemCopy(mac_address, psl->parentuser[i].mac_address, 6);
      MyStatus_Copy(myst, &psl->parentuser[i].mystatus);
      psl->parentuser[i].life = PARENT_LIST_LIFE;
      psl->parentuser[i].occ = TRUE;
      psl->list_update_req = TRUE;
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * 親機を削除
 *
 * @param   em		
 * @param   parent_no		削除する親機No
 */
//--------------------------------------------------------------
static void _ParentSearchList_DeleteParent(COMM_ENTRY_MENU_PTR em, int parent_no)
{
  em->parentsearch.parentuser[parent_no].occ = FALSE;
}

//--------------------------------------------------------------
/**
 * 親機探索リストのメニュー項目STRBUFを更新する
 *
 * @param   em		
 */
//--------------------------------------------------------------
static void _ParentSearchList_SetListString(COMM_ENTRY_MENU_PTR em)
{
	int i;
  STRBUF *strbuf_expand, *strbuf_src, *strbuf_src_hit, *strbuf_eom;
  STRCODE str_eom = GFL_STR_GetEOMCode();
  
  if(em->parentsearch.list_strbuf_create == TRUE){
    FLDMENUFUNC_ListSTRBUFDelete(em->parentsearch.fldmenu_listdata);
  }
  
  strbuf_src = GFL_STR_CreateBuffer(64, em->heap_id);
  strbuf_src_hit = GFL_STR_CreateBuffer(64, em->heap_id);
  strbuf_expand = GFL_STR_CreateBuffer(64, em->heap_id);
  strbuf_eom = GFL_STR_CreateBuffer(4, em->heap_id);
	
  GFL_MSG_GetString( em->msgdata, msg_connect_search_000, strbuf_src );
  GFL_MSG_GetString( em->msgdata, msg_connect_search_001, strbuf_src_hit );
	for( i = 0; i < PARENT_LIST_MAX; i++ ){
    WORDSET_RegisterNumber( em->wordset, 0, i+1, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
    if(em->parentsearch.parentuser[i].occ == TRUE){
      WORDSET_RegisterPlayerName( em->wordset, 1, &em->parentsearch.parentuser[i].mystatus );
      WORDSET_RegisterNumber( em->wordset, 2, 
        MyStatus_GetID_Low(&em->parentsearch.parentuser[i].mystatus), 
        5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( em->wordset, strbuf_expand, strbuf_src_hit );
    }
    else{
    	GFL_STR_SetStringCode(strbuf_eom, &str_eom);
    	WORDSET_RegisterWord( em->wordset, 1, strbuf_eom, 0, TRUE, PM_LANG );
    	WORDSET_RegisterWord( em->wordset, 2, strbuf_eom, 0, TRUE, PM_LANG );
      WORDSET_ExpandStr( em->wordset, strbuf_expand, strbuf_src );
    }
    FLDMENUFUNC_AddStringListData( em->parentsearch.fldmenu_listdata,
  		strbuf_expand, i, em->heap_id );
	}
	
	GFL_STR_DeleteBuffer(strbuf_src_hit);
	GFL_STR_DeleteBuffer(strbuf_src);
	GFL_STR_DeleteBuffer(strbuf_expand);
	GFL_STR_DeleteBuffer(strbuf_eom);
	
	em->parentsearch.list_strbuf_create = TRUE;
}

//--------------------------------------------------------------
/**
 * 親機待ちの間のキャンセル管理
 *
 * @param   em		
 */
//--------------------------------------------------------------
static PARENT_WAIT_CANCEL_STATE _ParentWait_UpdateCnacel( COMM_ENTRY_MENU_PTR em )
{
  COMM_ENTRY_YESNO *yesno = &em->yesno;

  enum{
    _SEQ_WAIT,
    _SEQ_SELECT
  };
  switch( yesno->seq )
  {
  case _SEQ_WAIT:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {
      FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(em->fieldWork);
      yesno->menufunc = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
      _StreamMsgSet(em, msg_game_exit);

      yesno->seq = _SEQ_SELECT;
    }
    break;
  case _SEQ_SELECT:
    {
      FLDMENUFUNC_YESNO ret;
      
      ret = FLDMENUFUNC_ProcYesNoMenu(yesno->menufunc);
      if(FLDMSGBG_CheckFinishPrint( FIELDMAP_GetFldMsgBG(em->fieldWork) ) == TRUE
          && em->forbit_leave == TRUE){
        //選択可能な状態で離脱禁止フラグが立っている場合は強制でNOを選んだことにする
        ret = FLDMENUFUNC_YESNO_NO;
      }
        
      if(ret != FLDMENUFUNC_YESNO_NULL)
      {
        FLDMENUFUNC_DeleteMenu(yesno->menufunc);
        yesno->menufunc = NULL;
        
        if(ret == FLDMENUFUNC_YESNO_NO)
        {
          yesno->seq = _SEQ_WAIT;
          _StreamMsgSet(em, msg_game_wait_member);
        }
        else
        {
          return PARENT_WAIT_CANCEL_CANCEL;
        }
      }
      return PARENT_WAIT_CANCEL_SELECT;
    }
    break;
  }
  
  return PARENT_WAIT_CANCEL_NULL;
}

static void _ParentWait_ExitCnacel( COMM_ENTRY_MENU_PTR em )
{
  COMM_ENTRY_YESNO *yesno = &em->yesno;
  if( yesno->menufunc != NULL )
  {
    FLDMENUFUNC_DeleteMenu(yesno->menufunc);
    yesno->menufunc = NULL;
  }
  yesno->seq = 0;
}
//==================================================================
/**
 * エントリー時の親機からの結果をセット
 *
 * @param   em		
 * @param   answer		
 * @param   myst		  
 */
//==================================================================
void CommEntryMenu_SetEntryAnswer(COMM_ENTRY_MENU_PTR em, ENTRY_PARENT_ANSWER answer, const MYSTATUS *myst)
{
  em->entry_parent_answer = answer;
  MyStatus_Copy(myst, &em->entry_parent_myst);
}

//==================================================================
/**
 * ENTRYMENU_MEMBER_INFO構造体のサイズ取得
 *
 * @param   none		
 *
 * @retval  u32		
 */
//==================================================================
u32 CommEntryMenu_GetMemberInfoSize(void)
{
  return sizeof(ENTRYMENU_MEMBER_INFO);
}

//--------------------------------------------------------------
/**
 * 現在接続されているユーザーをビットで取得する
 *
 * @param   none		
 *
 * @retval  u8		
 */
//--------------------------------------------------------------
static u8 _GetConnectMemberBit(void)
{
  NetID net_id;
  u8 connect_member_bit = 0;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(GFL_NET_IsConnectMember(net_id) == TRUE){
      connect_member_bit |= 1 << net_id;
    }
  }
  
  return connect_member_bit;
}

//--------------------------------------------------------------
/**
 * 指定メンバー(bit)が一人以上接続状態であるかを調べる
 *
 * @param   member_bit		
 *
 * @retval  BOOL		一人でも接続が生きていればTRUE
 *                  全員切断している場合はFALSE
 */
//--------------------------------------------------------------
static BOOL _CheckConnectMemberBit(u8 member_bit)
{
  NetID net_id;
  
  for(net_id = 0; net_id < COMM_ENTRY_USER_MAX; net_id++){
    if(GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) != net_id){  //自分は無視
      if((member_bit & (1<<net_id)) && GFL_NET_IsConnectMember(net_id) == TRUE){
        return TRUE;
      }
    }
  }
  
  return FALSE;
}
