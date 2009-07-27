//==============================================================================
/**
 * @file    union_msg.c
 * @brief   ユニオンルーム：ツール類(オーバーレイ領域に配置されます)
 * @author  matsuda
 * @date    2009.07.13(月)
 */
//==============================================================================
#include <gflib.h>
#include "union_types.h"
#include "union_local.h"
#include "net_app/union/union_main.h"
#include "field/field_player.h"
#include "field/fieldmap.h"
#include "net_app/union/union_msg.h"
#include "message.naix"
#include "msg/msg_union.h"
#include "system/main.h"


//==============================================================================
//  プロトタイプ宣言：データ
//==============================================================================
static const FLDMENUFUNC_LIST BattleMenuList_Number[3];
static const FLDMENUFUNC_LIST BattleMenuList_Mode[5];
static const FLDMENUFUNC_LIST BattleMenuList_Reg[4];
static const FLDMENUFUNC_LIST BattleMenuList_RegDouble[4];


//==============================================================================
//  データ
//==============================================================================
//--------------------------------------------------------------
//  各ゲームの最大参加人数
//--------------------------------------------------------------
static const struct{
  u8 member_max;
  u8 padding[3];
}MenuMemberMax[] = {
  {2},      //UNION_PLAY_CATEGORY_UNION,          ///<ユニオンルーム
  {2},      //UNION_PLAY_CATEGORY_TALK,           ///<会話中
  {2},      //UNION_PLAY_CATEGORY_TRAINERCARD,    ///<トレーナーカード
  {4},      //UNION_PLAY_CATEGORY_PICTURE,        ///<お絵かき
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50,         ///<コロシアム
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE,       ///<コロシアム
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD,   ///<コロシアム
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50,         ///<コロシアム
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE,       ///<コロシアム
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD,   ///<コロシアム
  {4},      //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI,      ///<コロシアム
  {2},      //UNION_PLAY_CATEGORY_TRADE,          ///<ポケモン交換
  {4},      //UNION_PLAY_CATEGORY_GURUGURU,       ///<ぐるぐる交換
  {5},      //UNION_PLAY_CATEGORY_RECORD,         ///<レコードコーナー
};

//--------------------------------------------------------------
//  メニューヘッダー
//--------------------------------------------------------------
///メインメニューリスト
static const FLDMENUFUNC_LIST MainMenuList[] =
{
  {msg_union_select_01, (void*)UNION_PLAY_CATEGORY_TRAINERCARD},   //挨拶
  {msg_union_select_02, (void*)UNION_PLAY_CATEGORY_PICTURE},   //お絵かき
  {msg_union_select_03, (void*)UNION_MENU_NO_SEND_BATTLE},   //対戦
  {msg_union_select_04, (void*)UNION_PLAY_CATEGORY_TRADE},   //交換
  {msg_union_select_06, (void*)UNION_PLAY_CATEGORY_GURUGURU}, //ぐるぐる交換
  {msg_union_select_07, (void*)UNION_PLAY_CATEGORY_RECORD},   //レコードコーナー
  {msg_union_select_05, (void*)UNION_MENU_SELECT_CANCEL},   //やめる
};

///メニューヘッダー(メインメニュー用)
static const FLDMENUFUNC_HEADER MenuHeader_MainMenu =
{
	7,		//リスト項目数
	7,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	1,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_NON,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	16,		//文字サイズY(ドット
	32-14,		//表示座標X キャラ単位
	1,		//表示座標Y キャラ単位
	13,		//表示サイズX キャラ単位
	14,		//表示サイズY キャラ単位
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);

//--------------------------------------------------------------
//  対戦メニュー
//--------------------------------------------------------------
///二人対戦、四人対戦、やめる
static const FLDMENUFUNC_LIST BattleMenuList_Number[] =
{
  {msg_union_battle_01_01, (void*)BattleMenuList_Mode},   //二人対戦
  {msg_union_battle_01_02, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_MULTI},    //四人対戦
  {msg_union_battle_01_03, (void*)FLDMENUFUNC_CANCEL},    //やめる
};

///二人対戦：シングル、ダブル、トリプル、ローテーション、もどる
static const FLDMENUFUNC_LIST BattleMenuList_Mode[] =
{
  {msg_union_battle_01_04, (void*)BattleMenuList_Reg},      //シングル
  {msg_union_battle_01_05, (void*)BattleMenuList_RegDouble},   //ダブル
  {msg_union_battle_01_06, (void*)BattleMenuList_Number},   //トリプル
  {msg_union_battle_01_07, (void*)BattleMenuList_Number},   //ローテーション
  {msg_union_battle_01_08, (void*)BattleMenuList_Number},   //もどる
};

///二人対戦：シングル：せいげんなし、スタンダード、もどる
static const FLDMENUFUNC_LIST BattleMenuList_Reg[] =
{
  {msg_union_battle_01_09, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50},//レベル５０ルール
  {msg_union_battle_01_10, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE}, //制限なし
  {msg_union_battle_01_11, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD},//スタンダード
  {msg_union_battle_01_12, (void*)BattleMenuList_Mode},   //もどる
};

///二人対戦：ダブル：せいげんなし、スタンダード、もどる
static const FLDMENUFUNC_LIST BattleMenuList_RegDouble[] =
{
  {msg_union_battle_01_09, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50},//レベル５０ルール
  {msg_union_battle_01_10, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE}, //制限なし
  {msg_union_battle_01_11, (void*)UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD},//スタンダード
  {msg_union_battle_01_12, (void*)BattleMenuList_Mode},   //もどる
};

///対戦メニューのデータテーブル
static const struct{
  const FLDMENUFUNC_LIST *list;
  u8 list_max;
}BattleMenuDataTbl[] = {
  {
    BattleMenuList_Number,
    3,
  },
  {
    BattleMenuList_Mode,
    5,
  },
  {
    BattleMenuList_Reg,
    4,
  },
  {
    BattleMenuList_RegDouble,
    4,
  },
};

///メニューヘッダー(対戦メニュー用)
static const FLDMENUFUNC_HEADER MenuHeader_Battle =
{
	3,		//リスト項目数
	3,		//表示最大項目数
	0,		//ラベル表示Ｘ座標
	13,		//項目表示Ｘ座標
	0,		//カーソル表示Ｘ座標
	0,		//表示Ｙ座標
	1,		//表示文字色
	15,		//表示背景色
	2,		//表示文字影色
	0,		//文字間隔Ｘ
	1,		//文字間隔Ｙ
	FLDMENUFUNC_SKIP_NON,	//ページスキップタイプ
	12,		//文字サイズX(ドット
	16,		//文字サイズY(ドット
	32-16,		//表示座標X キャラ単位
	1,		//表示座標Y キャラ単位
	15,		//表示サイズX キャラ単位
	14,		//表示サイズY キャラ単位
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * 各メニューで選択したゲームの通信最大参加人数を取得する
 *
 * @param   menu_index		UNION_MSG_MENU_SELECT_???
 *
 * @retval  int		最大参加人数
 */
//==================================================================
int UnionMsg_GetMemberMax(UNION_PLAY_CATEGORY menu_index)
{
  if(menu_index >= UNION_PLAY_CATEGORY_MAX){
    GF_ASSERT(menu_index < UNION_PLAY_CATEGORY_MAX);
    return 0;
  }
  return MenuMemberMax[menu_index].member_max;
}

//==================================================================
/**
 * UnionMsg系のものを全て一括削除
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_AllDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_YesNo_Del(unisys);
  UnionMsg_Menu_MainMenuDel(unisys);
  UnionMsg_TalkStream_WindowDel(unisys);
}

//==================================================================
/**
 * 会話ウィンドウ：セットアップ
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;
  
  if(unisys->fld_msgwin_stream == NULL){
    GF_ASSERT(unisys->msgdata == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_union_dat );
    unisys->fld_msgwin_stream = FLDMSGWIN_STREAM_AddTalkWin(fldmsg_bg, unisys->msgdata);
  }
  else{ //既にウィンドウが表示されている場合はメッセージエリアのクリアを行う
    FLDMSGWIN_STREAM_ClearMessage(unisys->fld_msgwin_stream);
  }
}

//==================================================================
/**
 * 会話ウィンドウ：削除
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream != NULL){
    FLDMSGWIN_STREAM_ClearWindow(unisys->fld_msgwin_stream);
    FLDMSGWIN_STREAM_Delete(unisys->fld_msgwin_stream);
    unisys->fld_msgwin_stream = NULL;
  }

  if(unisys->msgdata != NULL){
    FLDMSGBG_DeleteMSGDATA(unisys->msgdata);
    unisys->msgdata = NULL;
  }
}

//==================================================================
/**
 * 会話ウィンドウ：出力開始
 *
 * @param   unisys		
 * @param   str_id		メッセージID
 */
//==================================================================
void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id)
{
  FLDMSGPRINT *msg_print;
  
  GF_ASSERT(unisys->fld_msgwin_stream != NULL);
  FLDMSGWIN_STREAM_PrintStart(unisys->fld_msgwin_stream, 0, 0, str_id);
}

//==================================================================
/**
 * 会話ウィンドウ：終了チェック
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:終了。　FALSE:継続中
 */
//==================================================================
BOOL UnionMsg_TalkStream_Check(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream == NULL){
    return TRUE;
  }
  return FLDMSGWIN_STREAM_Print(unisys->fld_msgwin_stream);
}

//==================================================================
/**
 * 会話ウィンドウ：出力開始に加え、メッセージウィンドウが無い場合は生成も行う
 *
 * @param   unisys		  
 * @param   fieldWork		
 * @param   str_id		  メッセージID
 */
//==================================================================
void UnionMsg_TalkStream_PrintPack(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, u32 str_id)
{
  UnionMsg_TalkStream_WindowSetup(unisys, fieldWork);
  UnionMsg_TalkStream_Print(unisys, str_id);
}

//==================================================================
/**
 * 「はい」「いいえ」選択ウィンドウセットアップ
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_YesNo_Setup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;

  if(unisys->fldmenu_yesno_func == NULL){
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->fldmenu_yesno_func = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
  }
}

//==================================================================
/**
 * 「はい」「いいえ」選択ウィンドウ削除
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_YesNo_Del(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_yesno_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_yesno_func);
    unisys->fldmenu_yesno_func = NULL;
  }
}

//==================================================================
/**
 * 「はい」「いいえ」選択待ち
 *
 * @param   unisys		
 * @param   result		結果代入先(TRUE:はい、FALSE:いいえ)
 *
 * @retval  BOOL		TRUE:選択した。　FALSE:選択中
 */
//==================================================================
BOOL UnionMsg_YesNo_SelectLoop(UNION_SYSTEM_PTR unisys, BOOL *result)
{
  FLDMENUFUNC_YESNO yes_no;
  
  GF_ASSERT(unisys->fldmenu_yesno_func != NULL);
  
  yes_no = FLDMENUFUNC_ProcYesNoMenu(unisys->fldmenu_yesno_func);
  if(yes_no == FLDMENUFUNC_YESNO_NULL){
    return FALSE;
  }
  else if(yes_no == FLDMENUFUNC_YESNO_YES){
    *result = TRUE;
  }
  else{
    *result = FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * メニューリスト：セットアップ
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *menuhead)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  
  if(unisys->fldmenu_func == NULL){
    fldmenu_listdata = FLDMENUFUNC_CreateMakeListData(
      menulist, list_max, unisys->msgdata, HEAPID_UNION);
    unisys->fldmenu_func = FLDMENUFUNC_AddMenu(fldmsg_bg, menuhead, fldmenu_listdata);
  }
}

//--------------------------------------------------------------
/**
 * メニューリスト：削除
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_func);
    unisys->fldmenu_func = NULL;
  }
}

//--------------------------------------------------------------
/**
 * メニューリスト：選択待ち
 *
 * @param   unisys		
 *
 * @retval  u32		結果
 */
//--------------------------------------------------------------
static u32 UnionMsg_Menu_SelectLoop(UNION_SYSTEM_PTR unisys)
{
  GF_ASSERT(unisys->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcMenu(unisys->fldmenu_func);
}

//==================================================================
/**
 * メインメニュー：セットアップ
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_MainMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
{
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    MainMenuList, NELEMS(MainMenuList), &MenuHeader_MainMenu);
}

//==================================================================
/**
 * メインメニュー：削除
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_MainMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * メインメニュー：選択待ち
 *
 * @param   unisys		
 * 
 * @retval  u32		結果
 */
//==================================================================
u32 UnionMsg_Menu_MainMenuSelectLoop(UNION_SYSTEM_PTR unisys)
{
  return UnionMsg_Menu_SelectLoop(unisys);
}

//==================================================================
/**
 * 対戦メニュー：セットアップ
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork, int menu_index)
{
  int i;
  FLDMENUFUNC_HEADER head;
  
  head = MenuHeader_Battle;
  head.count = BattleMenuDataTbl[menu_index].list_max;
  head.line = BattleMenuDataTbl[menu_index].list_max;
  head.bmpsize_y = head.count * 2;
  
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    BattleMenuDataTbl[menu_index].list, BattleMenuDataTbl[menu_index].list_max, &head);
}

//==================================================================
/**
 * 対戦メニュー：削除
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * 対戦メニュー：選択待ち
 *
 * @param   unisys		
 * @param   next_sub_menu		TRUE:次のサブメニューがある　FALSE:最終結果
 *
 * @retval  u32		next_sub_menuがTRUEの場合、次のメニューIndex
 * @retval  u32		next_sub_menuがFALSEの場合、最終結果番号(これを通信で送る)
 */
//==================================================================
u32 UnionMsg_Menu_BattleMenuSelectLoop(UNION_SYSTEM_PTR unisys, BOOL *next_sub_menu)
{
  u32 menu_ret;
  int i;
  
  *next_sub_menu = FALSE;
  
  menu_ret = UnionMsg_Menu_SelectLoop(unisys);
  if(menu_ret < UNION_PLAY_CATEGORY_MAX 
      || menu_ret == FLDMENUFUNC_NULL || menu_ret == FLDMENUFUNC_CANCEL){
    return menu_ret;
  }
  
  for(i = 0; i < NELEMS(BattleMenuDataTbl); i++){
    if(BattleMenuDataTbl[i].list == (void*)menu_ret){
      *next_sub_menu = TRUE;
      return i;
    }
  }
  
  GF_ASSERT_MSG(0, "menu_ret=%d\n", menu_ret);
  return 0;
}

