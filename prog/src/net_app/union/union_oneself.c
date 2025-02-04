//==============================================================================
/**
 * @file    union_oneself.c
 * @brief   ユニオン：自分動作
 * @author  matsuda
 * @date    2009.07.09(木)
 */
//==============================================================================
#include <gflib.h>
#include "union_types.h"
#include "union_local.h"
#include "net_app/union/union_main.h"
#include "union_oneself.h"
#include "field/field_player.h"
#include "field/fieldmap.h"
#include "net_app/union/union_msg.h"
#include "message.naix"
#include "msg/msg_union_room.h"
#include "union_comm_command.h"
#include "system/main.h"
#include "net_app/union/union_event_check.h"
#include "net_app/union/union_subproc.h"
#include "net_app/union/colosseum_comm_command.h"
#include "colosseum.h"
#include "colosseum_tool.h"
#include "union_chara.h"
#include "field/fieldmap_ctrl_grid.h"
#include "savedata/config.h"
#include "pm_define.h"
#include "field\event_colosseum_battle.h"
#include "union_tool.h"
#include "app\pms_input.h"
#include "app\pms_select.h"
#include "poke_tool/regulation_def.h"
#include "poke_tool/poke_regulation.h"
#include "savedata/battle_box_save.h"
#include "net_app/union_app.h"
#include "savedata/etc_save.h"
#include "app/trainer_card.h"

//#include "field/event_ircbattle.h"
#include "net_app\irc_compatible.h"
#include "net_app/pokemontrade.h"

#include "gamesystem/game_beacon.h"
#include "field/party_select_list.h"
#include "field/scrcmd.h"
#include "union_snd_def.h"


//==============================================================================
//  定数定義
//==============================================================================
///シーケンス番号
enum{
  ONESELF_SUBPROC_INIT,
  ONESELF_SUBPROC_UPDATE,
  ONESELF_SUBPROC_EXIT,
};

///話しかけてから反応があるまでのタイムアウト時間
#define ONESELF_SERVER_TIMEOUT      (30 * 20)


//==============================================================================
//  型定義
//==============================================================================
///動作型
typedef BOOL (*ONESELF_FUNC)(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);

///動作セット
typedef struct{
  ONESELF_FUNC func_init;
  ONESELF_FUNC func_update;
  ONESELF_FUNC func_exit;
}ONESELF_FUNC_DATA;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static BOOL OneselfSeq_NormalInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ForceExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_Enter(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_Leave(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ChatCallUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectReqInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectReqUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectReqExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectAnswerInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectAnswerUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectAnswerExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkInit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkExit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkListSendUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkInit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkUpdate_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkExit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkPlayGameUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkPlayGameUpdate_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TradeUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_IntrudeUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ShutdownUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_Talk_Battle_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_MinigameUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_MultiColosseumUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumMemberWaitUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumFirstDataSharingUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumNormal(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumStandPosition(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumPokelistBattle(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumStandingBack(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumUsePartySelect(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumPokelistReady(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumPokelistBeforeDataShare(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumLeaveYesNoUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumLeaveUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumTrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);

static void _CommEntryMenuCallback_Entry(void *work, const MYSTATUS *myst, const u8 *mac_address);
static void _CommEntryMenuCallback_Leave(void *work, const MYSTATUS *myst, const u8 *mac_address);


//==============================================================================
//  データ
//==============================================================================
static const ONESELF_FUNC_DATA OneselfFuncTbl[] = {
  {//UNION_STATUS_NORMAL
    OneselfSeq_NormalInit,
    OneselfSeq_NormalUpdate,
    NULL,
  },
  {//UNION_STATUS_FORCE_EXIT
    NULL,
    OneselfSeq_ForceExit,
    NULL,
  },
  {//UNION_STATUS_ENTER
    NULL,
    OneselfSeq_Enter,
    NULL,
  },
  {//UNION_STATUS_LEAVE
    NULL,
    OneselfSeq_Leave,
    NULL,
  },
  {//UNION_STATUS_CHAT_CALL
    NULL,
    OneselfSeq_ChatCallUpdate,
    NULL,
  },
  {//UNION_STATUS_CONNECT_REQ
    OneselfSeq_ConnectReqInit,
    OneselfSeq_ConnectReqUpdate,
    OneselfSeq_ConnectReqExit,
  },
  {//UNION_STATUS_CONNECT_ANSWER
    OneselfSeq_ConnectAnswerInit,
    OneselfSeq_ConnectAnswerUpdate,
    OneselfSeq_ConnectAnswerExit,
  },
  {//UNION_STATUS_TALK_PARENT
    OneselfSeq_TalkInit_Parent,
    OneselfSeq_TalkUpdate_Parent,
    OneselfSeq_TalkExit_Parent,
  },
  {//UNION_STATUS_TALK_LIST_SEND_PARENT
    NULL,
    OneselfSeq_TalkListSendUpdate_Parent,
    NULL,
  },
  {//UNION_STATUS_TALK_CHILD
    OneselfSeq_TalkInit_Child,
    OneselfSeq_TalkUpdate_Child,
    OneselfSeq_TalkExit_Child,
  },
  {//UNION_STATUS_TALK_BATTLE_PARENT
    NULL,
    OneselfSeq_Talk_Battle_Parent,
    NULL,
  },
  {//UNION_STATUS_TALK_PLAYGAME_PARENT
    NULL,
    OneselfSeq_TalkPlayGameUpdate_Parent,
    NULL,
  },
  {//UNION_STATUS_TALK_PLAYGAME_CHILD
    NULL,
    OneselfSeq_TalkPlayGameUpdate_Child,
    NULL,
  },
  {//UNION_STATUS_TRAINERCARD
    NULL,
    OneselfSeq_TrainerCardUpdate,
    NULL,
  },
  {//UNION_STATUS_PICTURE
    NULL,
    OneselfSeq_MinigameUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_FREE_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_FREE
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_FLAT_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_FLAT
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_FREE_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_FREE
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_FLAT_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_FLAT
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_FREE_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_FREE
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_FLAT_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_FLAT
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_FREE_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_FREE
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_FLAT_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_FLAT
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_MULTI_FREE_SHOOTER
    NULL,
    OneselfSeq_MultiColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_MULTI_FREE
    NULL,
    OneselfSeq_MultiColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_MULTI_FLAT_SHOOTER
    NULL,
    OneselfSeq_MultiColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_MULTI_FLAT
    NULL,
    OneselfSeq_MultiColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_TRADE
    NULL,
    OneselfSeq_TradeUpdate,
    NULL,
  },
  {//UNION_STATUS_GURUGURU
    NULL,
    OneselfSeq_MinigameUpdate,
    NULL,
  },
  {//UNION_STATUS_RECORD
    NULL,
    OneselfSeq_ShutdownUpdate,
    NULL,
  },
  {//UNION_STATUS_INTRUDE
    NULL,
    OneselfSeq_IntrudeUpdate,
    NULL,
  },
  {//UNION_STATUS_SHUTDOWN
    NULL,
    OneselfSeq_ShutdownUpdate,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_MEMBER_WAIT
    NULL,
    OneselfSeq_ColosseumMemberWaitUpdate,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_FIRST_DATA_SHARING
    NULL,
    OneselfSeq_ColosseumFirstDataSharingUpdate,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_NORMAL
    OneselfSeq_ColosseumInit,
    OneselfSeq_ColosseumNormal,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_STANDPOSITION
    NULL,
    OneselfSeq_ColosseumStandPosition,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_STANDING_BACK
    NULL,
    OneselfSeq_ColosseumStandingBack,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_USE_PARTY_SELECT
    NULL,
    OneselfSeq_ColosseumUsePartySelect,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_POKELIST_READY
    NULL,
    OneselfSeq_ColosseumPokelistReady,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_POKELIST_BEFORE_DATA_SHARE
    NULL,
    OneselfSeq_ColosseumPokelistBeforeDataShare,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_POKELIST_BATTLE
    NULL,
    OneselfSeq_ColosseumPokelistBattle,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_LEAVE_YESNO
    NULL,
    OneselfSeq_ColosseumLeaveYesNoUpdate,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_LEAVE
    NULL,
    OneselfSeq_ColosseumLeaveUpdate,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_TRAINER_CARD
    NULL,
    OneselfSeq_ColosseumTrainerCardUpdate,
    NULL,
  },
  {//UNION_STATUS_CHAT
    NULL,
    NULL,
    NULL,
  },
};
SDK_COMPILER_ASSERT(NELEMS(OneselfFuncTbl) == UNION_STATUS_MAX);



//==============================================================================
//
//  
//
//==============================================================================
int UnionOneself_Update(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  ONESELF_FUNC func;
  BOOL next_seq = TRUE;
  
  switch(situ->func_proc){
  case ONESELF_SUBPROC_INIT:
    func = OneselfFuncTbl[situ->union_status].func_init;
    break;
  case ONESELF_SUBPROC_UPDATE:
    func = OneselfFuncTbl[situ->union_status].func_update;
    break;
  case ONESELF_SUBPROC_EXIT:
    func = OneselfFuncTbl[situ->union_status].func_exit;
    break;
  }

  if(func != NULL){
    next_seq = func(unisys, situ, fieldWork, &situ->func_seq);
  }
  
  if(next_seq == TRUE){
    situ->func_seq = 0;
    situ->func_proc++;
    if(situ->func_proc > ONESELF_SUBPROC_EXIT){
      situ->func_proc = 0;
      situ->before_union_status = situ->union_status;
      situ->union_status = situ->next_union_status;
      situ->next_union_status = UNION_STATUS_NORMAL;
    }
  }
  
  return situ->union_status;
}

//==================================================================
/**
 * 次のUNION_STATUSをセットする
 *
 * @param   unisys        
 * @param   req_status    UNION_STATUS_???
 *
 * @retval  BOOL    TRUE：成功
 */
//==================================================================
static BOOL UnionOneself_ReqStatus(UNION_SYSTEM_PTR unisys, int req_status)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  situ->next_union_status = req_status;
  return TRUE;
}

//==================================================================
/**
 * 外側から次のUNION_STATUSをセットする
 *
 * @param   unisys		
 * @param   req_status		
 *
 * @retval  BOOL		TRUE:成功
 */
//==================================================================
BOOL UnionOneself_Outside_ReqStatus(UNION_SYSTEM_PTR unisys, int req_status)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  if(situ->func_proc == ONESELF_SUBPROC_UPDATE){  //UPDATE状態でないと受け付けない
    return UnionOneself_ReqStatus(unisys, req_status);
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * メニューIndexからバトルレギュレーション番号を取得する
 * @param   play_category   
 * @retval  int   レギュレーションNo
 */
//--------------------------------------------------------------
static int Union_GetPlayCategory_to_RegulationNo(u32 play_category)
{
  switch(play_category){
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //対戦:1VS1:シングル:制限なし
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:      //対戦:1VS1:シングル:制限なし
    return REG_FREE_SINGLE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER://対戦:1VS1:シングル:フラット
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT:  //対戦:1VS1:シングル:フラット
    return REG_FLAT_SINGLE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //対戦:1VS1:ダブル:制限なし
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:      //対戦:1VS1:ダブル:制限なし
    return REG_FREE_DOUBLE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER:  //対戦:1VS1:ダブル:フラット
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT:  //対戦:1VS1:ダブル:フラット
    return REG_FLAT_DOUBLE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
    return REG_FREE_TRIPLE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT:
    return REG_FLAT_TRIPLE;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
    return REG_FREE_ROTATION;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT:
    return REG_FLAT_ROTATION;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
    return REG_FREE_MALTI;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
    return REG_FLAT_MALTI;
  default:
    GF_ASSERT(0);
    return REG_FREE_SINGLE;
  }
}

//--------------------------------------------------------------
/**
 * play_categoryからシューターの有無を取得する
 * @param   play_category   
 * @retval  int   TRUE:シューターあり
 * @retval  int   FALSE:シューター無し
 */
//--------------------------------------------------------------
static BOOL Union_GetPlayCategory_to_Shooter(u32 play_category)
{
  switch(play_category){
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //対戦:1VS1:シングル:制限なし
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER://対戦:1VS1:シングル:フラット
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //対戦:1VS1:ダブル:制限なし
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER:  //対戦:1VS1:ダブル:フラット
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * プレイカテゴリーが戦闘のものかチェックする
 *
 * @param   play_category   
 *
 * @retval  BOOL    TRUE:戦闘　FALSE:戦闘以外
 */
//--------------------------------------------------------------
static BOOL Union_CheckPlayCategoryBattle(u32 play_category)
{
  if(play_category < UNION_PLAY_CATEGORY_COLOSSEUM_START 
      || play_category > UNION_PLAY_CATEGORY_COLOSSEUM_END){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * バトルボックスのPOKEPARTYをAllocして作成します
 *
 * @param   unisys    
 *
 * @retval  POKEPARTY *   AllocしたPOKEPARTY (バトルボックスが無い場合はNULL)
 */
//--------------------------------------------------------------
static POKEPARTY * _BBox_PokePartyAlloc(UNION_SYSTEM_PTR unisys)
{
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data);
  BATTLE_BOX_SAVE *bb_save = BATTLE_BOX_SAVE_GetBattleBoxSave( sv_ctrl );
  
  if(BATTLE_BOX_SAVE_IsIn( bb_save ) == FALSE){
    return NULL;
  }
  
  return BATTLE_BOX_SAVE_MakePokeParty( bb_save, HEAPID_UNION );
}

//--------------------------------------------------------------
/**
 * バトルボックスのPOKEPARTYをFreeします
 * @param   party   
 */
//--------------------------------------------------------------
static void _BBox_PokePartyFree(POKEPARTY *party)
{
  GF_ASSERT_HEAVY(party != NULL);
  GFL_HEAP_FreeMemory(party);
}

//--------------------------------------------------------------
/**
 * unisys->alloc.regulationにplay_categoryに対応したレギュレーションデータをロードします
 *
 * @param   play_category   
 */
//--------------------------------------------------------------
static void _Load_PlayCategory_to_Regulation(UNION_SYSTEM_PTR unisys, u32 play_category)
{
  int reg_no;

  GF_ASSERT(Union_CheckPlayCategoryBattle(play_category) == TRUE);
  
  reg_no = Union_GetPlayCategory_to_RegulationNo(play_category);
  PokeRegulation_LoadData(reg_no, unisys->alloc.regulation);
  if(Union_GetPlayCategory_to_Shooter(play_category) == TRUE){
    Regulation_SetParam(unisys->alloc.regulation, REGULATION_SHOOTER, REGULATION_SHOOTER_VALID);
  }
  else{
    Regulation_SetParam(unisys->alloc.regulation, REGULATION_SHOOTER, REGULATION_SHOOTER_INVALID);
  }
}

//--------------------------------------------------------------
/**
 * 手持ちポケモンのレギュレーションチェック
 *
 * @param   unisys    
 * @param   fail_bit    
 *
 * @retval  POKE_REG_RETURN_ENUM    
 *
 * unisys->alloc.regulationにレギュレーションデータがロードされている必要があります
 */
//--------------------------------------------------------------
static POKE_REG_RETURN_ENUM _CheckRegulation_Temoti(UNION_SYSTEM_PTR unisys, u32 *fail_bit)
{
  POKE_REG_RETURN_ENUM reg_ret;
  
  *fail_bit = 0;
  reg_ret = PokeRegulationMatchLookAtPokeParty(unisys->alloc.regulation, 
    GAMEDATA_GetMyPokemon(unisys->uniparent->game_data), fail_bit);
  return reg_ret;
}

//--------------------------------------------------------------
/**
 * バトルボックスのレギュレーションチェック
 *
 * @param   unisys    
 * @param   fail_bit    
 *
 * @retval  POKE_REG_RETURN_ENUM    
 *
 * unisys->alloc.regulationにレギュレーションデータがロードされている必要があります
 */
//--------------------------------------------------------------
static POKE_REG_RETURN_ENUM _CheckRegulation_BBox(UNION_SYSTEM_PTR unisys, u32 *fail_bit)
{
  POKE_REG_RETURN_ENUM reg_ret;
  POKEPARTY *bb_party = _BBox_PokePartyAlloc(unisys);
  
  *fail_bit = 0;
  
  if(bb_party != NULL){
    reg_ret = PokeRegulationMatchLookAtPokeParty(
      unisys->alloc.regulation, bb_party, fail_bit);
    _BBox_PokePartyFree(bb_party);
  }
  else{ //バトルボックスのセーブデータが存在しない
    *fail_bit = 0xffffffff;
    reg_ret = POKE_REG_ILLEGAL_POKE;  //何でもいいからエラーを返す
  }
  return reg_ret;
}

//--------------------------------------------------------------
/**
 * 選択メニューが戦闘だった場合、参加出来るかレギュレーションチェック
 *
 * @param   unisys    
 * @param   menu_index    
 * @param   temoti_fail_bit      手持ちNGbit代入先
 * @param   bbox_fail_bit        バトルボックスNGbit代入先
 *
 * @retval  BOOL    TRUE:どちらか片方だけでも参加OK　FALSE:両方とも参加NG
 */
//--------------------------------------------------------------
static BOOL Union_CheckEntryBattleRegulation(UNION_SYSTEM_PTR unisys, u32 menu_index,u32 *temoti_fail_bit, u32 *bbox_fail_bit)
{
  POKE_REG_RETURN_ENUM temoti_ret, bbox_ret;
  
  *temoti_fail_bit = 0;
  *bbox_fail_bit = 0;
  
  if(Union_CheckPlayCategoryBattle(menu_index) == FALSE){
    return TRUE;  //戦闘ではないのでTRUE
  }
  
  _Load_PlayCategory_to_Regulation(unisys, menu_index);

  //レギュレーションチェック
  temoti_ret = _CheckRegulation_Temoti(unisys, temoti_fail_bit);
  bbox_ret = _CheckRegulation_BBox(unisys, bbox_fail_bit);

  if(temoti_ret == POKE_REG_OK || bbox_ret == POKE_REG_OK){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 自機にポーズをかける
 *
 * @param   fieldWork   
 * @param   pause_flag    TRUE:ポーズ　FALSE:ポーズ解除
 */
//--------------------------------------------------------------
static void _PlayerMinePause(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, int pause_flag)
{
  unisys->player_pause = pause_flag;
  FIELDMAP_CTRL_GRID_SetPlayerPause( fieldWork, pause_flag );
}

//--------------------------------------------------------------
/**
 * 通信エラー状態かを監視して、エラーならば強制終了にシーケンスを切り替える
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:強制終了にシーケンスを切り替えた
 *
 * ユニオンルーム用。コロシアムには未対応。
 * 基本的には各シーケンスの頭にこれを仕込んでおく。
 * それだとワークの解放や状態などで出来ないシーケンスの箇所は
 * 安全なタイミングでこれを呼ぶ
 */
//--------------------------------------------------------------
static BOOL _UnionCheckError_ForceExit(UNION_SYSTEM_PTR unisys)
{
  if(NetErr_App_CheckError() != NET_ERR_CHECK_NONE){
    UnionOneself_ReqStatus(unisys, UNION_STATUS_FORCE_EXIT);
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム用：通信エラー状態かを監視して、エラーならば強制終了にシーケンスを切り替える
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:強制終了にシーケンスを切り替えた
 *
 * コロシアム用。
 * 基本的には各シーケンスの頭にこれを仕込んでおく。
 * それだとワークの解放や状態などで出来ないシーケンスの箇所は
 * 安全なタイミングでこれを呼ぶ
 */
//--------------------------------------------------------------
static BOOL _UnionCheckError_ColosseumForceExit(UNION_SYSTEM_PTR unisys)
{
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
    return TRUE;
  }
  return FALSE;
}

//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * 通常状態(何もしていない)：初期化
 *
 * @param   unisys    
 * @param   situ    
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_NormalInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UnionMyComm_Init(unisys, &situ->mycomm);
  UnionMySituation_SetParam(
    unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, (void*)UNION_PLAY_CATEGORY_UNION);

  UnionMsg_AllDel(unisys);

  _PlayerMinePause(unisys, fieldWork, FALSE);
  
  unisys->chat_call = FALSE;

  return TRUE;
}

//--------------------------------------------------------------
/**
 * 通常状態(何もしていない)：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( fieldWork );
  u16 obj_id, buf_no;
  s16 check_gx, check_gy, check_gz;
  MMDL *target_pc;
  
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  if(UnionComm_Check_ShutdownRestarts(unisys) == TRUE){
    return FALSE;
  }
  
  //外部から動作リクエストを受けているなら切り替える
  if(situ->next_union_status != UNION_STATUS_NORMAL){
    OS_TPrintf("NormalUpdate:外部リクエスト受信。切り替えます next_status=%d\n", situ->next_union_status);
    return TRUE;
  }

  if(GFL_NET_GetConnectNum() > 1){
//    GF_ASSERT_MSG(0, "Normalなのに接続！！！！\n");
    OS_TPrintf("Normal接続\n");
    UnionComm_Req_ShutdownRestarts(unisys); //意図しないタイミングの為、切断する
    return FALSE;
  }
  
  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
    FIELD_PLAYER_GetFrontGridPos(player, &check_gx, &check_gy, &check_gz);
    target_pc = MMDLSYS_SearchGridPos(fldMdlSys, check_gx, check_gz, FALSE);
    if(target_pc == NULL){
      return FALSE;
    }
    obj_id = MMDL_GetOBJID(target_pc);
    if(UNION_CHARA_CheckCommPlayer(obj_id) == FALSE 
        || UNION_CHARA_TalkCheck(unisys, obj_id) == FALSE){
      return FALSE;
    }
    
    situ->mycomm.talk_obj_id = obj_id;
    buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(obj_id);
    OS_TPrintf("ターゲット発見 buf_no = %d, gx=%d, gz=%d\n", buf_no, check_gx, check_gz);
    UnionMySituation_SetParam(unisys, 
      UNION_MYSITU_PARAM_IDX_TALK_PC, &unisys->receive_beacon[buf_no]);
    if(UNION_CHARA_CheckCharaIndex(obj_id) == UNION_CHARA_INDEX_PARENT){
      if(unisys->receive_beacon[buf_no].beacon.play_category == UNION_PLAY_CATEGORY_UNION){
        UnionMySituation_SetParam(unisys, 
          UNION_MYSITU_PARAM_IDX_CALLING_PC, &unisys->receive_beacon[buf_no]);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_CONNECT_REQ);
      }
      else{
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PLAYGAME_PARENT);
      }
    }
    else{
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PLAYGAME_CHILD);
    }
    _PlayerMinePause(unisys, fieldWork, TRUE);
    return TRUE;
  }
  
  //チャット入力のショートカットボタン押下チェック
  if(unisys->chat_call == TRUE){
    UnionOneself_ReqStatus(unisys, UNION_STATUS_CHAT_CALL);
    return TRUE;
  }

  //出口チェック
  if(UnionTool_CheckWayOut(fieldWork) == TRUE){
    if(unisys->wayout_walk == TRUE){
      _PlayerMinePause(unisys, fieldWork, TRUE);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_LEAVE);
    }
  }
  else{ //一度でも出口座標から移動した
    unisys->wayout_walk = TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 通信エラーなどによる強制終了：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ForceExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  OS_TPrintf("ForceExit seq=%d\n", *seq);
  switch(*seq){
  case 0:
    _PlayerMinePause(unisys, fieldWork, TRUE);
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionMsg_AllDel(unisys);
      (*seq)++;
    }
    break;
  case 2:
    if(NetErr_App_CheckError() != NET_ERR_CHECK_NONE){
      GAMESYSTEM_SetFieldCommErrorReq(unisys->uniparent->gsys, TRUE);
      return FALSE;
    }
    
    UnionComm_Req_Shutdown(unisys);
    (*seq)++;
    break;
  case 3:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      (*seq)++;
    }
    break;
  case 4:
    UnionAlloc_AllFree(unisys);
    UnionMyComm_Init(unisys, &situ->mycomm);
    UnionComm_Req_Restarts(unisys);
    (*seq)++;
    break;
  case 5:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      (*seq)++;
    }
    break;
  default:
    UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ユニオンルーム入室：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_Enter(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
#if 0
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);

  switch(*seq){
  case 0:
    _PlayerMinePause(unisys, fieldWork, TRUE);
    if(MMDL_CheckPossibleAcmd(player_mmdl) == TRUE){
      MMDL_SetAcmd(player_mmdl, AC_WARP_DOWN);
      (*seq)++;
    }
    else{
      OS_TPrintf("MMDL_CheckPossibleAcmd待ち\n");
    }
    break;
  case 1:
    if(MMDL_CheckEndAcmd(player_mmdl) == TRUE){
      MMDL_EndAcmd(player_mmdl);
      _PlayerMinePause(unisys, fieldWork, FALSE);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      return TRUE;
    }
    break;
  }
  return FALSE;
#else
  return TRUE;
#endif
}

//--------------------------------------------------------------
/**
 * ユニオンルーム退出：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_Leave(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  switch(*seq){
  case 0:
//    GAMEBEACON_Set_UnionOut();
    unisys->finish = TRUE;
    (*seq)++;
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ユニオンルーム退出：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ChatCallUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  switch(*seq){
  case 0:
    if(unisys->chat_call == FALSE){
      //ここでFALSEになっているという事はシーケンス遷移中に他のイベント(Xメニュー)が起動した
      //このままここで終了する
      return TRUE;
    }
    
    unisys->chat_call = FALSE;
    PMSND_PlaySE(UNION_SE_APPEAL_ICON_TOUCH);
    {
      PMS_SELECT_PARAM  *initParam;
      PMS_DATA  pmsDat;

      initParam             = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof( PMS_SELECT_PARAM ) );
      initParam->save_ctrl  = GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data);

      unisys->parent_work = initParam;
      UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_CHAT, initParam);
    }
    (*seq)++;
    break;
  case 1:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("サブPROC終了\n");
      (*seq)++;
    }
    break;
  case 2:
    {
      PMS_SELECT_PARAM  *initParam = unisys->parent_work;
      const PMS_DATA* pmsdata = initParam->out_pms_data;
      
      // 簡易会話を更新したか？
      if( initParam->out_cancel_flag == FALSE ){
        // ビーコンデータの簡易会話を書き換える & 通信データに反映
        UnionChat_SetMyPmsData(unisys, pmsdata);
      }
    }
    GFL_HEAP_FreeMemory(unisys->parent_work);
    unisys->parent_work = NULL;
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続リクエスト：初期化
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ConnectReqInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  PMSND_PlaySystemSE( UNION_SE_TALK_WIN_OPEN );
  UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_connect_union_08);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 接続リクエスト実行：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ConnectReqUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    LOCALSEQ_INIT,
    LOCALSEQ_WAIT,
    LOCALSEQ_FIRST_CHECK,
    LOCALSEQ_END,
    LOCALSEQ_LASTKEY_END,
    LOCALSEQ_RESTART_END,
    LOCALSEQ_RETRY,
    LOCALSEQ_RETRY_WAIT,
  };

  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }
  
  switch(*seq){
  case LOCALSEQ_INIT:
    OS_TPrintf("ChangeOver モード切替：親固定\n");
    GFL_NET_ChangeoverModeSet(GFL_NET_CHANGEOVER_MODE_FIX_PARENT, TRUE, NULL);
    situ->wait = 0;
    (*seq)++;
    break;
  case LOCALSEQ_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    
    if(GFL_NET_GetConnectNum() > 1){
      int s;
      OS_TPrintf("接続しました！：親\n");
      for(s = 2; s < UNION_CONNECT_PLAYER_NUM; s++){
        if(GFL_NET_IsConnectMember(s) == TRUE){ //通信ID0,1を超えた相手と繋がってしまっている
          break;
        }
      }
      if(s < UNION_CONNECT_PLAYER_NUM || GFL_NET_SystemGetConnectNum() > 2){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, UnionMsg_GetMsgID_TalkTimeout(unisys));
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        UnionComm_Req_ShutdownRestarts(unisys);
        OS_TPrintf("意図しない接続のため切断 %d %d\n", s, GFL_NET_SystemGetConnectNum());
        (*seq) = LOCALSEQ_LASTKEY_END;
        break;
      }
      
      GFL_NET_SetNoChildErrorCheck(TRUE);
      GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), FALSE); //乱入禁止
      (*seq) = LOCALSEQ_FIRST_CHECK;
    }
    else{
      situ->wait++;
      if(situ->wait > ONESELF_SERVER_TIMEOUT || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL)){
        UnionComm_Req_ShutdownRestarts(unisys);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CALLING_PC, NULL);
        if(situ->wait > ONESELF_SERVER_TIMEOUT){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, UnionMsg_GetMsgID_TalkTimeout(unisys));
        }
        else{
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_00_05);
        }
        OS_TPrintf("子が来なかった為キャンセルしました\n");
        (*seq) = LOCALSEQ_RESTART_END;
      }
    }
    break;
  case LOCALSEQ_FIRST_CHECK:
    //最初の認証データ受信待ち
    if(situ->mycomm.talk_first_param.occ == TRUE){
      if(GFL_STD_MemComp(situ->mycomm.talk_first_param.mac_address, 
          situ->mycomm.calling_pc->mac_address, 6) != 0){
        //認証NG：接続してきた人は最後に話しかけた相手ではない
        if(UnionSend_FirstParentAnswer(UNION_FIRST_PARENT_ANSWER_NG) == TRUE){
          GFL_NET_SetNoChildErrorCheck(FALSE);  //子機切断をOKにして一人になるのを待って切断
          (*seq) = LOCALSEQ_RETRY;
        }
      }
      else{ //認証OK
        if(UnionSend_FirstParentAnswer(UNION_FIRST_PARENT_ANSWER_OK) == TRUE){
          PMSND_PlaySystemSE( UNION_SE_CONNECT );
          UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
          UnionMySituation_SetParam(
            unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC,situ->mycomm.calling_pc);
          UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CALLING_PC, NULL);
          *seq = LOCALSEQ_END;
        }
      }
    }
    break;

  case LOCALSEQ_END:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;
  case LOCALSEQ_LASTKEY_END:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY){
        return TRUE;
      }
    }
    break;
  case LOCALSEQ_RESTART_END:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      *seq = LOCALSEQ_LASTKEY_END;
    }
    break;
  
  case LOCALSEQ_RETRY:
    if(GFL_NET_SystemGetConnectNum() <= 1){ //自分ひとりになったらリトライ開始
      UnionComm_Req_ShutdownRestarts(unisys);
      (*seq)++;
    }
    break;
  case LOCALSEQ_RETRY_WAIT:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      situ->mycomm.talk_first_param.occ = FALSE;
      *seq = LOCALSEQ_INIT;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続リクエスト：終了
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ConnectReqExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CALLING_PC, NULL);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 接続リクエストを受けたので接続を試みる：初期化
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ConnectAnswerInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
//  PMSND_PlaySystemSE( UNION_SE_TALK_WIN_OPEN );
  PMSND_PlaySystemSE( UNION_SE_CONNECT );
  UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_00_02);
  _PlayerMinePause(unisys, fieldWork, TRUE);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 接続リクエストを受けたので接続を試みる：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ConnectAnswerUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _LOCALSEQ_INIT,
    _LOCALSEQ_CONNECT_WAIT,
    _LOCALSEQ_CANCEL_END,
    _LOCALSEQ_SEND_FIRST_PARAM,
    _LOCALSEQ_ANSWER_WAIT,
    _LOCALSEQ_NG_END,
  };

  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case _LOCALSEQ_INIT:
    OS_TPrintf("親へ接続しにいきます\n");
    GF_ASSERT_HEAVY(situ->mycomm.answer_pc != NULL);
    OS_TPrintf("ChangeOver モード切替：子固定\n");
    GFL_NET_ChangeoverModeSet(
      GFL_NET_CHANGEOVER_MODE_FIX_CHILD, TRUE, situ->mycomm.answer_pc->mac_address);
    situ->wait = 0;
    (*seq) = _LOCALSEQ_CONNECT_WAIT;
    break;
  case _LOCALSEQ_CONNECT_WAIT:
    if(GFL_NET_GetConnectNum() > 1){
      OS_TPrintf("接続しました！：子\n");
      GFL_NET_SetNoChildErrorCheck(TRUE);
      *seq = _LOCALSEQ_SEND_FIRST_PARAM;
    }
    else{
      situ->wait++;
      if(situ->wait > ONESELF_SERVER_TIMEOUT || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL)){
        UnionComm_Req_ShutdownRestarts(unisys);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, NULL);
        if(situ->wait > ONESELF_SERVER_TIMEOUT){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_00_03);
        }
        else{
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_00_05);
        }
        OS_TPrintf("親と接続出来なかった為キャンセルしました\n");
        (*seq) = _LOCALSEQ_CANCEL_END;
      }
    }
    break;
  case _LOCALSEQ_CANCEL_END:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE
          && UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY){
        return TRUE;
      }
    }
    break;
  
  case _LOCALSEQ_SEND_FIRST_PARAM:
    if(UnionSend_FirstChildParam() == TRUE){
      *seq = _LOCALSEQ_ANSWER_WAIT;
    }
    break;
  case _LOCALSEQ_ANSWER_WAIT:
    switch(situ->mycomm.first_parent_answer){
    case UNION_FIRST_PARENT_ANSWER_OK:
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, situ->mycomm.answer_pc);
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, NULL);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_CHILD);
      return TRUE;  //UnionMsg_TalkStream_Checkは次のUNION_STATUS_TALK_CHILDで行う
    case UNION_FIRST_PARENT_ANSWER_NG:
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, NULL);
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_00_03);
      UnionComm_Req_ShutdownRestarts(unisys);
      *seq = _LOCALSEQ_NG_END;
      break;
    }
    break;
  case _LOCALSEQ_NG_END:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE
        && UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続リクエストを受けたので接続を試みる：終了
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ConnectAnswerExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, NULL);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 接続確立後の会話（親）：初期化
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkInit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  if(situ->before_union_status == UNION_STATUS_TALK_BATTLE_PARENT){
    situ->work = 1; //対戦メニューからの戻りを表す
    return TRUE;  //対戦メニューからの戻りなので初期化はしない
  }
  situ->work = 0;
  
  switch(*seq){
  case 0:
    UnionMyComm_InitMenuParam(&situ->mycomm);
    UnionMySituation_SetParam(
      unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, (void*)UNION_PLAY_CATEGORY_TALK);
    UnionMyComm_PartyAdd(&situ->mycomm, situ->mycomm.connect_pc);

    //会話関連のワークの初期化をお互いしてから会話が始まるように同期取り
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TALK_START, WB_NET_UNION);
    OS_TPrintf("会話開始前の同期待ち・・・親\n");
    (*seq)++;
    break;
  case 1:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TALK_START, WB_NET_UNION) == TRUE){
      OS_TPrintf("会話開始前の同期待ち・・・同期完了\n");
      (*seq)++;
    }
    break;
  case 2:   //MYSTATUS交換
    if(UnionSend_Mystatus(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case 3:
    if(MATH_CountPopulation(((u32)situ->mycomm.mystatus_recv_bit)) > 1){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続確立後の会話（親）：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _LOCALSEQ_INIT,
    _LOCALSEQ_TALK_WAIT,
    _LOCALSEQ_MAINMENU_SETUP,
    _LOCALSEQ_MAINMENU_MAIN,
    _LOCALSEQ_MAINMENU_FAIL,
  };

  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case _LOCALSEQ_INIT:
    if(situ->work == 1){  //バトルメニューからの戻りなのでメッセージを出さない
      situ->work = 0;
    }
    else if(situ->mycomm.first_talk == 0){
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_ParentConnectStart(situ->mycomm.talk_pc->beacon.sex));
      situ->mycomm.first_talk++;
    }
    else{
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_TalkContinue(situ->mycomm.talk_pc->beacon.sex));
    }
    (*seq)++;
    break;
  case _LOCALSEQ_TALK_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case _LOCALSEQ_MAINMENU_SETUP:   //メインメニュー描画
    UnionMsg_Menu_MainMenuSetup(unisys, fieldWork);
    (*seq)++;
    break;
  case _LOCALSEQ_MAINMENU_MAIN:
    {
      u32 select_list;
      const POKEPARTY *party = GAMEDATA_GetMyPokemon(unisys->uniparent->game_data);
      
      select_list = UnionMsg_Menu_MainMenuSelectLoop(unisys, fieldWork);
      if(select_list == FLDMENUFUNC_NULL){
        break;
      }
      UnionMsg_Menu_MainMenuDel(unisys);

      if(select_list == FLDMENUFUNC_CANCEL || select_list == UNION_MENU_SELECT_CANCEL){
        OS_TPrintf("メニューをキャンセルしました\n");
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_TalkCancel(situ->mycomm.talk_pc->beacon.sex));
        situ->mycomm.mainmenu_select = UNION_MENU_SELECT_CANCEL;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
        return TRUE;
      }
      else if(select_list == UNION_PLAY_CATEGORY_TRADE){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
        if(PokeParty_GetPokeCountNotEgg(party) < 2){
          //手持ちが2匹以上いないと交換できない
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
            UnionMsg_GetMsgID_TradeMinePokeNG(situ->mycomm.talk_pc->beacon.sex));
          *seq = _LOCALSEQ_MAINMENU_FAIL;
          break;
        }
      }
      else if(select_list == UNION_PLAY_CATEGORY_GURUGURU){
        if(PokeParty_GetPokeCountOnlyDameEgg(party) > 0){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
            UnionMsg_GetMsgID_GuruguruMineDametamago(unisys));
          *seq = _LOCALSEQ_MAINMENU_FAIL;
          break;
        }
        else if(PokeParty_GetPokeCountOnlyEgg(party) == 0){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
            UnionMsg_GetMsgID_GuruguruMineEggNG(situ->mycomm.talk_pc->beacon.sex));
          *seq = _LOCALSEQ_MAINMENU_FAIL;
          break;
        }
      }
      
      //真上の処理でbreak,returnしなかったのはここで吸収
      switch(select_list){
      case UNION_MENU_NO_SEND_BATTLE:
        OS_TPrintf("対戦メニューを選択\n");
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_BATTLE_PARENT);
        return TRUE;
      default:
        situ->mycomm.mainmenu_select = select_list;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
        return TRUE;
      }
    }
    break;
  case _LOCALSEQ_MAINMENU_FAIL:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      *seq = _LOCALSEQ_INIT;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続確立後の会話（親）：終了
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkExit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UnionMsg_Menu_MainMenuDel(unisys);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 接続確立後、選択したメニュー項目の送信と子の返事待ち（親）：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkListSendUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case 0: //相手に選択したメニューを通知
    if(UnionSend_MainMenuListResult(situ->mycomm.mainmenu_select) == TRUE){
      OS_TPrintf("リスト結果送信成功 : %d\n", situ->mycomm.mainmenu_select);
      if(situ->mycomm.mainmenu_select == UNION_MENU_SELECT_CANCEL){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
        return TRUE;
      }
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_ParentMainMenuSelectAnswerWait(
        situ->mycomm.mainmenu_select, situ->mycomm.talk_pc->beacon.sex));
      (*seq)++;
    }
    break;
  case 1: //返事待ち
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    if(situ->mycomm.mainmenu_yesno_result == TRUE){
      OS_TPrintf("「はい」受信\n");
      UnionOneself_ReqStatus(unisys, 
        UNION_STATUS_TRAINERCARD + situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD);
      (*seq)++;
    }
    else if(situ->mycomm.mainmenu_yesno_result == FALSE){
      OS_TPrintf("「いいえ」受信\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_ParentMainMenuSelectAnswerNG(situ->mycomm.mainmenu_select,
        situ->mycomm.talk_pc->beacon.sex));
      return TRUE;  //メッセージ待ちはUNION_STATUS_SHUTDOWNで行う
    }
    break;
  case 2:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続確立後の会話（子）：初期化
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkInit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case 0:
    UnionMyComm_InitMenuParam(&situ->mycomm);
    UnionMySituation_SetParam(
      unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, (void*)UNION_PLAY_CATEGORY_TALK);
    UnionMyComm_PartyAdd(&situ->mycomm, situ->mycomm.connect_pc);

    //会話関連のワークの初期化をお互いしてから会話が始まるように同期取り
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TALK_START, WB_NET_UNION);
    OS_TPrintf("会話開始前の同期待ち・・・子\n");
    (*seq)++;
    break;
  case 1:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TALK_START, WB_NET_UNION) == TRUE){
      OS_TPrintf("会話開始前の同期待ち・・・同期完了\n");
      (*seq)++;
    }
    break;
  case 2:   //MYSTATUS交換
    if(UnionSend_Mystatus(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case 3:
    if(MATH_CountPopulation(((u32)situ->mycomm.mystatus_recv_bit)) > 1){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続確立後の会話（子）：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkUpdate_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _INIT,
    _SELECT_WAIT,
    _BATTLE_REG_PRINT_WAIT,
    _YESNO_SELECT,
    _YESNO_WAIT,
    _YESNO_RESULT_SEND_WAIT,
    _REG_PRINT_TEMOTI,
    _REG_PRINT_TEMOTI_WAIT,
    _REG_PRINT_BBOX,
    _REG_PRINT_BBOX_WAIT,
    _ANSWER_MSG,
    _FINISH,
  };
  enum{
    _RESULT_NO_TEMOTI_SHORT = 1,    //交換するには手持ちが足りていない
    _RESULT_NO_EGG_SHORT,           //ぐるぐる交換するにはタマゴが足りていない
    _RESULT_NO_DAMETAMAGO,          //ダメタマゴがいるからぐるぐる交換は出来ない
  };

  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }
  
  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }

  switch(*seq){
  case _INIT:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_00_04);
    (*seq)++;
    break;
  case _SELECT_WAIT:
    if(situ->mycomm.mainmenu_select == UNION_MENU_SELECT_NULL){
      break;
    }
    else if(situ->mycomm.mainmenu_select == UNION_MENU_SELECT_CANCEL){
      OS_TPrintf("選択メニュー受信：キャンセル\n");
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_TalkCancelPartner(situ->mycomm.talk_pc->beacon.sex));
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      return TRUE;
    }
    else if(situ->mycomm.mainmenu_select < UNION_PLAY_CATEGORY_MAX){
      OS_TPrintf("選択メニュー受信：%d\n", situ->mycomm.mainmenu_select);
      UnionMsg_TalkStream_PrintPack(
        unisys, fieldWork, 
        UnionMsg_GetMsgID_ChildMainMenuSelectView(situ->mycomm.mainmenu_select));

      //戦闘の時はレギュレーション表示
      if(Union_CheckPlayCategoryBattle(situ->mycomm.mainmenu_select) == TRUE){
        _Load_PlayCategory_to_Regulation(unisys, situ->mycomm.mainmenu_select);
        UnionMsg_Menu_RegulationSetup(unisys, fieldWork, 0, 
          Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_RULE);
        (*seq) = _BATTLE_REG_PRINT_WAIT;
      }
      else{
        (*seq) = _YESNO_SELECT;
      }
    }
    else{
      if(UnionSend_MainMenuListResultAnswer(FALSE) == TRUE){
        OS_TPrintf("未知の選択メニュー受信：%d\n", situ->mycomm.mainmenu_select);
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_009);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
        return TRUE;
      }
    }
    break;
  
  case _BATTLE_REG_PRINT_WAIT:   //戦闘であればレギュレーション表示
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE){
      *seq = _YESNO_SELECT;
    }
    break;
    
  case _YESNO_SELECT:   //「はい・いいえ」選択
    UnionMsg_Menu_RegulationDel(unisys);
    UnionMsg_YesNo_Setup(unisys, fieldWork);
    (*seq)++;
    break;
  case _YESNO_WAIT:
    {
      BOOL result;
      
      situ->work = 0;
      if(UnionMsg_YesNo_SelectLoop(unisys, &result) == TRUE){
        const POKEPARTY *party = GAMEDATA_GetMyPokemon(unisys->uniparent->game_data);
        
        UnionMsg_YesNo_Del(unisys);
        //戦闘の時はレギュレーションを見て参加可能かチェック
        if(Union_CheckEntryBattleRegulation(unisys, situ->mycomm.mainmenu_select, 
            &situ->reg_temoti_fail_bit, &situ->reg_bbox_fail_bit) == FALSE){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_010);
          result = FALSE; //強制で「いいえ」を返す
        }
        else if(situ->mycomm.mainmenu_select == UNION_PLAY_CATEGORY_TRADE && result == TRUE){
          GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
          if(PokeParty_GetPokeCountNotEgg(party) < 2){
            //手持ちが2匹以上いないと交換できない
            result = FALSE; //強制で「いいえ」を返す
            situ->work = _RESULT_NO_TEMOTI_SHORT;
          }
        }
        else if(situ->mycomm.mainmenu_select == UNION_PLAY_CATEGORY_GURUGURU && result == TRUE){
          if(PokeParty_GetPokeCountOnlyDameEgg(party) > 0){
            result = FALSE;
            situ->work = _RESULT_NO_DAMETAMAGO;
          }
          else if(PokeParty_GetPokeCountOnlyEgg(party) == 0){
            result = FALSE;
            situ->work = _RESULT_NO_EGG_SHORT;
          }
        }
        situ->mycomm.mainmenu_yesno_result = result;
        (*seq)++;
      }
    }
    break;
  case _YESNO_RESULT_SEND_WAIT: //「はい・いいえ」選択結果送信
    if(UnionSend_MainMenuListResultAnswer(situ->mycomm.mainmenu_yesno_result) == TRUE){
      if(situ->reg_temoti_fail_bit > 0 && situ->reg_bbox_fail_bit > 0){
        *seq = _REG_PRINT_TEMOTI;
      }
      else{
        *seq = _ANSWER_MSG;
      }
      break;
    }
    break;
    
  case _REG_PRINT_TEMOTI:     //手持ちNGレギュレーション表示
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, situ->reg_temoti_fail_bit, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_NG_TEMOTI);
    (*seq)++;
    break;
  case _REG_PRINT_TEMOTI_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      if(situ->reg_bbox_fail_bit == 0xffffffff){  //バトルボックスを作っていないので表示スキップ
        *seq = _ANSWER_MSG;
      }
      else{
        (*seq)++;
      }
    }
    break;
  case _REG_PRINT_BBOX:       //バトルボックスNGレギュレーション表示
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, situ->reg_temoti_fail_bit, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_NG_BBOX);
    (*seq)++;
    break;
  case _REG_PRINT_BBOX_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      (*seq) = _ANSWER_MSG;
    }
    break;
    
  case _ANSWER_MSG: //「はい・いいえ」選択結果送信
    if(situ->mycomm.mainmenu_yesno_result == FALSE){
      if(situ->work == _RESULT_NO_TEMOTI_SHORT){  //手持ちが1匹だった為、交換を断った
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_04_04);
      }
      else if(situ->work == _RESULT_NO_EGG_SHORT){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_06_04);
      }
      else if(situ->work == _RESULT_NO_DAMETAMAGO){
        UnionMsg_TalkStream_PrintPack(
          unisys, fieldWork, UnionMsg_GetMsgID_GuruguruDameTamagoNG(unisys));
      }
      else{
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_ChildMainMenuSelectRefuse(situ->mycomm.mainmenu_select));
      }
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      return TRUE;  //メッセージ待ちはUNION_STATUS_SHUTDOWNで行う
    }
    else{
      UnionOneself_ReqStatus(unisys, 
        UNION_STATUS_TRAINERCARD + situ->mycomm.mainmenu_select-UNION_PLAY_CATEGORY_TRAINERCARD);
    }
    (*seq) = _FINISH;
    break;
  case _FINISH:
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 接続確立後の会話（子）：終了
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkExit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 対戦の会話（親）：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_Talk_Battle_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  BOOL next_menu, look;
  u32 select_ret;
  enum{
    LOCALSEQ_INIT,
    LOCALSEQ_MENU_SETUP,
    LOCALSEQ_MENU_LOOP,
    _RULE_LOOK,
    _RULE_LOOK_WAIT,
    _REG_PRINT_TEMOTI,
    _REG_PRINT_TEMOTI_WAIT,
    _REG_PRINT_BBOX,
    _REG_PRINT_BBOX_WAIT,
    _MENU_CONTINUE_MSG,
  };

  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }
  
  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }

  switch(*seq){
  case LOCALSEQ_INIT:
    situ->work = 0;
    (*seq)++;
    break;
  case LOCALSEQ_MENU_SETUP:
    OS_TPrintf("対戦メニューセットアップ：%d\n", situ->work);
    if(situ->work == BATTLE_MENU_INDEX_CUSTOM_MULTI){
      UnionMsg_Menu_BattleMenuMultiTitleSetup(unisys, fieldWork);
    }
    else if(situ->work == BATTLE_MENU_INDEX_NUMBER){
      UnionMsg_Menu_BattleMenuMultiTitleDel(unisys);
    }
    UnionMsg_Menu_BattleMenuSetup(unisys, fieldWork, situ->work, &situ->menu_reg);  //1vs1, 2vs2
    (*seq)++;
    break;
  case LOCALSEQ_MENU_LOOP:
    select_ret = UnionMsg_Menu_BattleMenuSelectLoop(unisys, &next_menu, &situ->menu_reg, &look, fieldWork);
    if(next_menu == TRUE){
      situ->work = select_ret;
      UnionMsg_Menu_BattleMenuDel(unisys);
      (*seq)--;
    }
    else if(select_ret == FLDMENUFUNC_CANCEL || select_ret == UNION_MENU_SELECT_CANCEL){
      UnionMsg_Menu_BattleMenuDel(unisys);
      if(situ->work > 0){
        situ->work = 0;
        (*seq)--;
      }
      else{
        UnionMsg_Menu_BattleMenuMultiTitleDel(unisys);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
        return TRUE;
      }
    }
    else if(look == TRUE){  //ルールを見る
      UnionMsg_Menu_BattleMenuMultiTitleDel(unisys);
      UnionMsg_Menu_BattleMenuDel(unisys);
      //レギュレーションデータのロード
      _Load_PlayCategory_to_Regulation(unisys, select_ret);
      situ->mycomm.mainmenu_select = select_ret;
      *seq = _RULE_LOOK;
    }
    else if(select_ret != FLDMENUFUNC_NULL){
      UnionMsg_Menu_BattleMenuMultiTitleDel(unisys);
      UnionMsg_Menu_BattleMenuDel(unisys);
      if(Union_CheckEntryBattleRegulation(unisys, select_ret, 
          &situ->reg_temoti_fail_bit, &situ->reg_bbox_fail_bit) == FALSE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_010);
        (*seq) = _REG_PRINT_TEMOTI;
        break;
      }
      situ->mycomm.mainmenu_select = select_ret;
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
      return TRUE;
    }
    break;

  case _RULE_LOOK:    //ルールを見る
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, 0, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_RULE);
    (*seq)++;
    break;
  case _RULE_LOOK_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      *seq = LOCALSEQ_MENU_SETUP;
    }
    break;

  case _REG_PRINT_TEMOTI:     //手持ちNGレギュレーション表示
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, situ->reg_temoti_fail_bit, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_NG_TEMOTI);
    (*seq)++;
    break;
  case _REG_PRINT_TEMOTI_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      if(situ->reg_bbox_fail_bit == 0xffffffff){  //バトルボックスを作っていないので表示スキップ
        *seq = _MENU_CONTINUE_MSG;
      }
      else{
        (*seq)++;
      }
    }
    break;
  case _REG_PRINT_BBOX:       //バトルボックスNGレギュレーション表示
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, situ->reg_temoti_fail_bit, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_NG_BBOX);
    (*seq)++;
    break;
  case _REG_PRINT_BBOX_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      (*seq) = _MENU_CONTINUE_MSG;
    }
    break;
  case _MENU_CONTINUE_MSG:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
      UnionMsg_GetMsgID_TalkContinue(situ->mycomm.talk_pc->beacon.sex));
    *seq = LOCALSEQ_INIT;
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 既に遊んでいる親に話しかけた：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkPlayGameUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UNION_PLAY_CATEGORY play_category;
  u16 buf_no;
  enum{
    LOCALSEQ_INIT,
    LOCALSEQ_END,
    LOCALSEQ_LASTKEY_END,
    
    LOCALSEQ_REGWIN_PRINT_WAIT,
    
    LOCALSEQ_YESNO_SETUP,
    LOCALSEQ_YESNO,

    _REG_PRINT_TEMOTI,
    _REG_PRINT_TEMOTI_WAIT,
    _REG_PRINT_BBOX,
    _REG_PRINT_BBOX_WAIT,
    
    LOCALSEQ_MINIGAME_YESNO_SETUP,
    LOCALSEQ_MINIGAME_YESNO,
  };
  
  buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(situ->mycomm.talk_obj_id);
  play_category = unisys->receive_beacon[buf_no].beacon.play_category;

  switch(*seq){
  case LOCALSEQ_INIT:
    situ->mycomm.mainmenu_select = unisys->receive_beacon[buf_no].beacon.play_category;
    switch(play_category){
    case UNION_PLAY_CATEGORY_TALK:           //会話中
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameMainMenuSelect(situ->mycomm.talk_pc->beacon.sex));
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_LASTKEY_END;
      break;
    case UNION_PLAY_CATEGORY_TRAINERCARD:    //トレーナーカード
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameTrainerCard(situ->mycomm.talk_pc->beacon.sex));
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_LASTKEY_END;
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameBattle(situ->mycomm.talk_pc->beacon.sex));
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_LASTKEY_END;
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
      if(unisys->receive_beacon[buf_no].beacon.connect_num < UnionMsg_GetMemberMax(play_category)){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_MultiIntrude(situ->mycomm.talk_pc->beacon.sex));

        //乱入可の時はレギュレーション表示
        _Load_PlayCategory_to_Regulation(unisys, play_category);
        UnionMsg_Menu_RegulationSetup(unisys, fieldWork, 0, 
          Union_GetPlayCategory_to_Shooter(play_category), REGWIN_TYPE_RULE);

        (*seq) = LOCALSEQ_REGWIN_PRINT_WAIT;
      }
      else{ //人数がいっぱい
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_PlayGameBattle(situ->mycomm.talk_pc->beacon.sex));
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        (*seq) = LOCALSEQ_LASTKEY_END;
      }
      break;
    case UNION_PLAY_CATEGORY_GURUGURU:  //ぐるぐる交換
      if(unisys->receive_beacon[buf_no].beacon.connect_num < UnionMsg_GetMemberMax(play_category)
          && unisys->receive_beacon[buf_no].beacon.entry_ng == FALSE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_GurugurIntrude(situ->mycomm.talk_pc->beacon.sex));
        (*seq) = LOCALSEQ_MINIGAME_YESNO_SETUP;
      }
      else{ //人数がいっぱい
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_PlayGameGuruguru(situ->mycomm.talk_pc->beacon.sex));
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        (*seq) = LOCALSEQ_LASTKEY_END;
      }
      break;
    case UNION_PLAY_CATEGORY_PICTURE:   //お絵かき
      if(unisys->receive_beacon[buf_no].beacon.connect_num < UnionMsg_GetMemberMax(play_category)){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_PictureIntrude(situ->mycomm.talk_pc->beacon.sex));
        (*seq) = LOCALSEQ_MINIGAME_YESNO_SETUP;
      }
      else{ //人数がいっぱい
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_PlayGamePicture(situ->mycomm.talk_pc->beacon.sex));
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        (*seq) = LOCALSEQ_LASTKEY_END;
      }
      break;
    case UNION_PLAY_CATEGORY_TRADE:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameTrade(situ->mycomm.talk_pc->beacon.sex));
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_LASTKEY_END;
      break;
    default:
      OS_TPrintf("未知の遊び play_category = %d\n", play_category);
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_unknown_game);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_LASTKEY_END;
      break;
    }
    break;

  case LOCALSEQ_LASTKEY_END:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY){
        return TRUE;
      }
    }
    break;
  case LOCALSEQ_END:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;
  
  case LOCALSEQ_REGWIN_PRINT_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE){
      *seq = LOCALSEQ_YESNO_SETUP;
    }
    break;
    
  case LOCALSEQ_YESNO_SETUP:   //「はい・いいえ」選択
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionMsg_Menu_RegulationDel(unisys);
      UnionMsg_YesNo_Setup(unisys, fieldWork);
      (*seq)++;
    }
    break;
  case LOCALSEQ_YESNO:
    {
      BOOL result;
      if(UnionMsg_YesNo_SelectLoop(unisys, &result) == TRUE){
        UnionMsg_YesNo_Del(unisys);
        if(result == FALSE){  //途中参加断る
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
            UnionMsg_GetMsgID_MultiIntrudeRefuses(situ->mycomm.talk_pc->beacon.sex));
          UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
          (*seq) = LOCALSEQ_LASTKEY_END;
        }
        else{ //途中参加する
          //乱入可のシーケンスをとりあえずセット　不可の場合は以下の処理で上書きしていく
          //レギュレーションを見て参加可能かチェック
          if(Union_CheckEntryBattleRegulation(unisys, situ->mycomm.mainmenu_select,
              &situ->reg_temoti_fail_bit, &situ->reg_bbox_fail_bit) == FALSE){
            UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_010);
            UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
            (*seq) = _REG_PRINT_TEMOTI;
          }
          else{ //乱入OK
            situ->mycomm.mainmenu_yesno_result = result;
            UnionOneself_ReqStatus(unisys, UNION_STATUS_INTRUDE);
            UnionMsg_TalkStream_PrintPack(unisys, fieldWork,
              UnionMsg_GetMsgID_MultiIntrudeWait(situ->mycomm.talk_pc->beacon.sex));
            (*seq) = LOCALSEQ_END;
          }
        }
      }
    }
    break;

  case _REG_PRINT_TEMOTI:     //手持ちNGレギュレーション表示
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, situ->reg_temoti_fail_bit, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_NG_TEMOTI);
    (*seq)++;
    break;
  case _REG_PRINT_TEMOTI_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      if(situ->reg_bbox_fail_bit == 0xffffffff){  //バトルボックスを作っていないので表示スキップ
        *seq = LOCALSEQ_END;
      }
      else{
        (*seq)++;
      }
    }
    break;
  case _REG_PRINT_BBOX:       //バトルボックスNGレギュレーション表示
    UnionMsg_Menu_RegulationSetup(unisys, fieldWork, situ->reg_temoti_fail_bit, 
      Union_GetPlayCategory_to_Shooter(situ->mycomm.mainmenu_select), REGWIN_TYPE_NG_BBOX);
    (*seq)++;
    break;
  case _REG_PRINT_BBOX_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE
        && (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL))){
      UnionMsg_Menu_RegulationDel(unisys);
      (*seq) = LOCALSEQ_END;
    }
    break;

  case LOCALSEQ_MINIGAME_YESNO_SETUP:   //ミニゲーム「はい・いいえ」選択
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionMsg_YesNo_Setup(unisys, fieldWork);
      (*seq)++;
    }
    break;
  case LOCALSEQ_MINIGAME_YESNO:
    {
      BOOL result;
      if(UnionMsg_YesNo_SelectLoop(unisys, &result) == TRUE){
        UnionMsg_YesNo_Del(unisys);
        if(result == FALSE){  //途中参加断る
          switch(situ->mycomm.mainmenu_select){
          case UNION_PLAY_CATEGORY_PICTURE:
            UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
              UnionMsg_GetMsgID_PictureIntrudeRefuses(situ->mycomm.talk_pc->beacon.sex));
            break;
          case UNION_PLAY_CATEGORY_GURUGURU:
            UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
              UnionMsg_GetMsgID_GuruguruIntrudeRefuses(situ->mycomm.talk_pc->beacon.sex));
            break;
          }
          UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
          (*seq) = LOCALSEQ_LASTKEY_END;
        }
        else{ //途中参加する
          //乱入可のシーケンスをとりあえずセット　不可の場合は以下の処理で上書きしていく
          situ->mycomm.mainmenu_yesno_result = result;
          UnionOneself_ReqStatus(unisys, UNION_STATUS_INTRUDE);
          (*seq) = LOCALSEQ_END;
          switch(situ->mycomm.mainmenu_select){
          case UNION_PLAY_CATEGORY_PICTURE:
            UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
              UnionMsg_GetMsgID_PictureIntrudeWait(situ->mycomm.talk_pc->beacon.sex));
            break;  //乱入不可にする判定は無い
          case UNION_PLAY_CATEGORY_GURUGURU:
            {
              const POKEPARTY *party = GAMEDATA_GetMyPokemon(unisys->uniparent->game_data);
              if(PokeParty_GetPokeCountOnlyDameEgg(party) > 0){ //手持ちにダメタマゴがある
                UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
                  UnionMsg_GetMsgID_GuruguruIntrudeDametamago(
                  situ->mycomm.talk_pc->beacon.sex));
                UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
                *seq = LOCALSEQ_LASTKEY_END;
              }
              else if(PokeParty_GetPokeCountOnlyEgg(party) == 0){ //タマゴが無い
                UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
                  UnionMsg_GetMsgID_GuruguruIntrudeNoEgg(situ->mycomm.talk_pc->beacon.sex));
                UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
                *seq = LOCALSEQ_LASTKEY_END;
              }
              else{ //乱入OK
                UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
                  UnionMsg_GetMsgID_GuruguruIntrudeWait(situ->mycomm.talk_pc->beacon.sex));
              }
            }
            break;
          }
        }
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 既に遊んでいる子に話しかけた：更新
 *
 * @param   unisys    
 * @param   situ      
 * @param   seq       
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TalkPlayGameUpdate_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UNION_PLAY_CATEGORY play_category;
  u16 buf_no;
  int sex;
  
  switch(*seq){
  case 0:
    sex = UNION_CHARA_GetCharaIndex_to_Sex(unisys, situ->mycomm.talk_obj_id);
    buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(situ->mycomm.talk_obj_id);
    play_category = unisys->receive_beacon[buf_no].beacon.play_category;
    switch(play_category){
    case UNION_PLAY_CATEGORY_TALK:           //会話中
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameMainMenuSelect(sex));
      break;
    case UNION_PLAY_CATEGORY_TRAINERCARD:    //トレーナーカード
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameTrainerCard(sex));
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT:      //コロシアム
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameBattle(sex));
      break;
    case UNION_PLAY_CATEGORY_GURUGURU:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameGuruguru(sex));
      break;
    case UNION_PLAY_CATEGORY_PICTURE:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGamePicture(sex));
      break;
    case UNION_PLAY_CATEGORY_TRADE:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PlayGameTrade(sex));
      break;
    default:
      OS_TPrintf("未知の遊び play_category = %d\n", situ->play_category);
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_unknown_game);
      break;
    }
    UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY){
        return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * トレーナーカード呼び出し：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case 0:   //トレーナーカードの情報を送りあう
    //同期取りの前に送信データと受信バッファを作成
    GF_ASSERT_HEAVY(unisys->alloc.my_card == NULL && unisys->alloc.target_card == NULL);
    unisys->alloc.my_card 
      = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(TR_CARD_DATA));
    unisys->alloc.target_card 
      = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(TR_CARD_DATA));
    TRAINERCARD_GetSelfData(unisys->alloc.my_card, unisys->uniparent->game_data, TRUE, FALSE, HEAPID_UNION);
    GFL_STD_MemCopy(  //一応何かの事故で受け取れなかった時のケアの為、自分のをコピーしておく
      unisys->alloc.my_card, unisys->alloc.target_card, sizeof(TR_CARD_DATA));
    
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PARAM, WB_NET_UNION);
    OS_TPrintf("トレーナーカード前の同期取り開始\n");

    //「はい、どうぞ！」
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
      UnionMsg_GetMsgID_TrainerCardStart(unisys, situ->mycomm.talk_pc->beacon.my_nation, 
      situ->mycomm.talk_pc->beacon.my_area));

    (*seq)++;
    break;
  case 1:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PARAM, WB_NET_UNION) == TRUE){
      OS_TPrintf("トレーナーカード前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case 2:
    if(UnionSend_TrainerCardParam(unisys) == TRUE){
      OS_TPrintf("トレーナーカード情報送信\n");
      (*seq)++;
    }
    break;
  case 3:
    if(situ->mycomm.target_card_receive == TRUE
        && UnionMsg_TalkStream_Check(unisys) == TRUE){
      OS_TPrintf("相手のカード受信\n");
      unisys->alloc.target_card->SignDisenable = TRUE;
      unisys->alloc.card_param = TRAINERCASR_CreateCallParam_CommData(
        unisys->uniparent->game_data, unisys->alloc.target_card, HEAPID_UNION);
      (*seq)++;
    }
    break;
  case 4:   //画面切り替え前の同期取り
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_BEFORE, WB_NET_UNION);
    OS_TPrintf("トレーナーカード画面切り替え前の同期取り開始\n");
    (*seq)++;
    break;
  case 5:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("トレーナーカード画面切り替え前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case 6:
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_TRAINERCARD, unisys->alloc.card_param);
    (*seq)++;
    break;
  case 7:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("サブPROC終了\n");
      (*seq)++;
    }
    break;
  case 8:   //トレーナーカード画面終了後の同期取り
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_01_02_4);
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_AFTER, WB_NET_UNION);
    OS_TPrintf("トレーナーカード終了後の同期取り開始\n");
    (*seq)++;
    break;
  case 9:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_AFTER, WB_NET_UNION) == TRUE){
      OS_TPrintf("トレーナーカード終了後の同期取り成功\n");
    
    #if 0//トレーナーカードのParentWorkはトレーナーカードのProc内で解放されるのでここでは解放しない
      GFL_HEAP_FreeMemory(unisys->alloc.card_param);
    #endif
      GFL_HEAP_FreeMemory(unisys->alloc.my_card);
      GFL_HEAP_FreeMemory(unisys->alloc.target_card);
      unisys->alloc.card_param = NULL;
      unisys->alloc.my_card = NULL;
      unisys->alloc.target_card = NULL;
      
      if(GFL_NET_IsParentMachine() == TRUE){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
      }
      else{
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_CHILD);
      }
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ポケモン交換：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_TradeUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _SEQ_INIT,
    _SEQ_TIMING,
    _SEQ_TIMING_WAIT,
    _SEQ_SUBPROC,
    _SEQ_SUBPROC_WAIT,
    _SEQ_SUBPROC_END_TIMING,
    _SEQ_SUBPROC_END_TIMING_WAIT,
  };

  //SUBPROCで使用したワークの解放がある為、_SEQ_SUBPROC_WAITは外す
  if((*seq) != _SEQ_SUBPROC_WAIT && _UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case _SEQ_INIT:
    //「交換を開始します！」
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_04_02);

    (*seq)++;
    break;
  case _SEQ_TIMING:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      GFL_NET_HANDLE_TimeSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_BEFORE, WB_NET_UNION);
      OS_TPrintf("ポケモン交換前の同期取り開始\n");
      (*seq)++;
    }
    break;
  case _SEQ_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_BEFORE, WB_NET_UNION) == TRUE){
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
      ETC_SAVE_WORK *etc_save = SaveData_GetEtc( GAMEDATA_GetSaveControlWork(gamedata) );
      const MYSTATUS *target_myst = GAMEDATA_GetMyStatusPlayer(gamedata, 
        GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) ^ 1);
      EtcSave_SetAcquaintance(etc_save, MyStatus_GetID(target_myst)); //知り合い登録
      OS_TPrintf("ポケモン交換前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case _SEQ_SUBPROC:
    {
      POKEMONTRADE_PARAM* eibw;
      eibw = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(POKEMONTRADE_PARAM));
      eibw->gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
      eibw->pParty = PokeParty_AllocPartyWork(HEAPID_UNION);
      //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
      //eibw->shinka_param = SHINKADEMO_AllocParam( HEAPID_PROC, eibw->gamedata, eibw->pParty, 0, 0, 0, TRUE );
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      {
        SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof( SHINKA_DEMO_PARAM ) );
        sdp->gamedata          = eibw->gamedata;
        sdp->ppt               = eibw->pParty;
        sdp->after_mons_no     = 0;
        sdp->shinka_pos        = 0;
        sdp->shinka_cond       = 0;
        sdp->b_field           = TRUE;
        sdp->b_enable_cancel   = FALSE;
        eibw->shinka_param     = sdp;
      }
      eibw->ret = POKEMONTRADE_MOVE_START;
      unisys->parent_work = eibw;
      UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_TRADE, eibw);
      (*seq)++;
    }
    break;
  case _SEQ_SUBPROC_WAIT:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      POKEMONTRADE_PARAM* eibw = unisys->parent_work;
      //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
      //SHINKADEMO_FreeParam( eibw->shinka_param );
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      {
        SHINKA_DEMO_PARAM* sdp = eibw->shinka_param;
        GFL_HEAP_FreeMemory( sdp );
      }
      GFL_HEAP_FreeMemory(eibw->pParty);
      GFL_HEAP_FreeMemory(unisys->parent_work);
      unisys->parent_work = NULL;
      OS_TPrintf("サブPROC終了\n");
      (*seq)++;
    }
    break;
  case _SEQ_SUBPROC_END_TIMING:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_connect_01_02_4);
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_AFTER, WB_NET_UNION);
    OS_TPrintf("ポケモン交換画面終了後の同期取り開始\n");
    (*seq)++;
    break;
  case _SEQ_SUBPROC_END_TIMING_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_AFTER, WB_NET_UNION) == TRUE){
      OS_TPrintf("ポケモン交換画面終了後の同期取り成功\n");
    
      if(GFL_NET_IsParentMachine() == TRUE){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
      }
      else{
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_CHILD);
      }
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 乱入処理：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_IntrudeUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  u16 buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(situ->mycomm.talk_obj_id);
  enum{
    _SEQ_INIT,
    _SEQ_MAIN,
    _SEQ_INTRUDE_NG_MSG,
    _SEQ_INTRUDE_MSG_WAIT_FINISH,
    _SEQ_MEMBER_OVER,
    _SEQ_MEMBER_OVER_SHUTDOWN_WAIT,
  };
  
  if(_UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case _SEQ_INIT:
    {
      OS_TPrintf("乱入開始します\n");
      OS_TPrintf("ChangeOver モード切替：子固定\n");
      situ->mycomm.intrude_exe = TRUE;
      Union_ChangePlayCategoryGSID(situ->mycomm.mainmenu_select); //乱入先のGSIDに変更
      GFL_NET_ChangeoverModeSet(
        GFL_NET_CHANGEOVER_MODE_FIX_CHILD, TRUE, unisys->receive_beacon[buf_no].mac_address);
    }
    situ->wait = 0;
    (*seq)++;
    break;
  case _SEQ_MAIN:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    
    if(GFL_NET_GetConnectNum() > 1){
      OS_TPrintf("接続しました！：子\n");
      GFL_NET_SetNoChildErrorCheck(TRUE);

      if(UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select) <= GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
        //自分の通信IDが参加出来るゲームの人数よりも大きい数字なら切断する
        *seq = _SEQ_MEMBER_OVER;
        break;
      }

      UnionMySituation_SetParam(
        unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, &unisys->receive_beacon[buf_no]);
      situ->mycomm.intrude = TRUE;
      //situ->mycomm.mainmenu_select = unisys->receive_beacon[buf_no].beacon.play_category;
      switch(situ->mycomm.mainmenu_select){
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_BATTLE_MULTI_FREE_SHOOTER);
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_BATTLE_MULTI_FREE);
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_BATTLE_MULTI_FLAT_SHOOTER);
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_BATTLE_MULTI_FLAT);
        break;
      case UNION_PLAY_CATEGORY_PICTURE:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_PICTURE);
        break;
      case UNION_PLAY_CATEGORY_GURUGURU:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_GURUGURU);
        break;
      default:
        OS_TPrintf("設定されていないcategory = %d\n", situ->mycomm.mainmenu_select);
        GF_ASSERT_HEAVY(0);
        break;
      }
      *seq = _SEQ_INTRUDE_MSG_WAIT_FINISH;
    }
    else{
      situ->wait++;
      if(situ->wait > ONESELF_SERVER_TIMEOUT || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL)){
        situ->mycomm.intrude_exe = FALSE;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        OS_TPrintf("親と接続出来なかった為キャンセルしました\n");
        *seq = _SEQ_MEMBER_OVER;
      }
    }
    break;
  case _SEQ_INTRUDE_NG_MSG:
    switch(situ->mycomm.mainmenu_select){
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_MultiIntrudeParentNG(situ->mycomm.talk_pc->beacon.sex));
      break;
    case UNION_PLAY_CATEGORY_PICTURE:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_PictureIntrudeParentNG(situ->mycomm.talk_pc->beacon.sex));
      break;
    case UNION_PLAY_CATEGORY_GURUGURU:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_GuruguruIntrudeParentNG(situ->mycomm.talk_pc->beacon.sex));
      break;
    default:
      OS_TPrintf("設定されていないcategory = %d\n", situ->mycomm.mainmenu_select);
      GF_ASSERT(0);
      break;
    }
    (*seq) = _SEQ_INTRUDE_MSG_WAIT_FINISH;
    break;
  case _SEQ_INTRUDE_MSG_WAIT_FINISH:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;
  
  case _SEQ_MEMBER_OVER:
    UnionComm_Req_ShutdownRestarts(unisys);
    (*seq)++;
    break;
  case _SEQ_MEMBER_OVER_SHUTDOWN_WAIT:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      *seq = _SEQ_INTRUDE_NG_MSG;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 切断処理：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ShutdownUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _SEQ_INIT,
    _SEQ_SHUTDOWN_WAIT,
    _SEQ_RESTART_WAIT,
    _SEQ_TALK_WAIT,
  };

  if((*seq) < _SEQ_TALK_WAIT && _UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }
  
  switch(*seq){
  case _SEQ_INIT:
    UnionComm_SetTalkShutdown(unisys);
    (*seq)++;
    break;

  case _SEQ_SHUTDOWN_WAIT:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE && GFL_NET_IsExit() == TRUE){
      if(situ->mycomm.connect_pc != NULL){
        UnionMyComm_PartyDel(&situ->mycomm, situ->mycomm.connect_pc);
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, NULL);
      }
      UnionComm_Req_Restarts(unisys);
      (*seq)++;
    }
    break;
  case _SEQ_RESTART_WAIT:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      (*seq)++;
    }
    break;
  case _SEQ_TALK_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & EVENT_WAIT_LAST_KEY){
        return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ミニゲーム遷移：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_MinigameUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _SEQ_INIT,
    _SEQ_TIMING_SEND,
    _SEQ_TIMING_WAIT,
    _SEQ_SUBPROC_SET,
    _SEQ_SUBPROC_WAIT,
    _SEQ_BEACON_RESTART,
    _SEQ_INTRUDE_INIT,
    _SEQ_INTRUDE_ANSWER_WAIT,
    _SEQ_INTRUDE_NG,
  };
  
  //SUBPROCのワーク解放がある為、_SEQ_SUBPROC_WAITは除く
  if((*seq) != _SEQ_SUBPROC_WAIT && (*seq) != _SEQ_BEACON_RESTART && (*seq) != _SEQ_INTRUDE_NG
      && _UnionCheckError_ForceExit(unisys) == TRUE){
    return TRUE;
  }

  switch(*seq){
  case _SEQ_INIT:
    if(situ->mycomm.intrude == TRUE){
      *seq = _SEQ_INTRUDE_INIT;
    }
    else{
      (*seq)++;
    }
    break;
  case _SEQ_TIMING_SEND:
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_MINIGAME_PROC_BEFORE, WB_NET_UNION);
    OS_TPrintf("ミニゲーム遷移前の同期取り開始\n");
    (*seq)++;
    break;
  case _SEQ_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_MINIGAME_PROC_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("ミニゲーム遷移前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case _SEQ_SUBPROC_SET:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(situ->union_status == UNION_STATUS_PICTURE){
        PICTURE_PARENT_WORK *picpwk;

        picpwk = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(PICTURE_PARENT_WORK));
        picpwk->gamedata = unisys->uniparent->game_data;
        unisys->parent_work = picpwk;
        UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_PICTURE, picpwk);
      }
      else{ //UNION_STATUS_GURUGURU
        GURUGURU_PARENT_WORK *gurupwk;
        
        gurupwk = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(GURUGURU_PARENT_WORK));
        gurupwk->gamedata = unisys->uniparent->game_data;
        gurupwk->party = GAMEDATA_GetMyPokemon(unisys->uniparent->game_data);
        gurupwk->sv = GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data);
        gurupwk->record = GAMEDATA_GetRecordPtr(unisys->uniparent->game_data);
        unisys->parent_work = gurupwk;
        UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_GURUGURU, gurupwk);
      }
      (*seq)++;
    }
    break;
  case _SEQ_SUBPROC_WAIT:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("サブPROC終了\n");
      GFL_HEAP_FreeMemory(unisys->parent_work);
      unisys->parent_work = NULL;
      if(NetErr_App_CheckError()){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_FORCE_EXIT);
        return TRUE;
      }
      else{
        UnionComm_Req_Restarts(unisys);
      //UnionComm_Req_ShutdownRestarts(unisys);
      }
      (*seq)++;
    }
    break;
  case _SEQ_BEACON_RESTART:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      OS_TPrintf("ビーコンスキャン再開完了\n");
      return TRUE;
    }
    break;
    
  case _SEQ_INTRUDE_INIT: //乱入時
    if(UnionSend_MinigameEntryReq(unisys) == TRUE){
      OS_TPrintf("ミニゲーム乱入希望送信\n");
      (*seq)++;
    }
    break;
  case _SEQ_INTRUDE_ANSWER_WAIT:
    switch(unisys->minigame_entry_answer){
    case UNION_MINIGAME_ENTRY_ANSWER_OK:
      if(situ->union_status == UNION_STATUS_PICTURE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_PictureIntrudeSuccess(situ->mycomm.talk_pc->beacon.sex));
      }
      else{ //UNION_STATUS_GURUGURU
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_GuruguruIntrudeSuccess(situ->mycomm.talk_pc->beacon.sex));
      }
      *seq = _SEQ_SUBPROC_SET;
      break;
    case UNION_MINIGAME_ENTRY_ANSWER_NG:  //親から乱入を拒否された
      if(situ->union_status == UNION_STATUS_PICTURE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_PictureIntrudeParentNG(situ->mycomm.talk_pc->beacon.sex));
      }
      else{ //UNION_STATUS_GURUGURU
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
          UnionMsg_GetMsgID_GuruguruIntrudeParentNG(situ->mycomm.talk_pc->beacon.sex));
      }
      UnionComm_Req_ShutdownRestarts(unisys);
      *seq = _SEQ_INTRUDE_NG;
      break;
    }
    break;
  case _SEQ_INTRUDE_NG:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, NULL);
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム遷移：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _SEQ_INIT,
    _SEQ_INIT_TALK_WAIT,
    _SEQ_WARP_BEFORE_TIMING,
    _SEQ_WARP_BEFORE_TIMING_WAIT,
    _SEQ_WARP_SUBPROC,
    _SEQ_WARP_SUBPROC_WAIT,
  };

  if(*seq < _SEQ_WARP_SUBPROC && _UnionCheckError_ForceExit(unisys) == TRUE){ //コロシアムに遷移前なので_UnionCheckError_ForceExitでチェックする。※コロシアム用のは使用しないこと！
    return TRUE;
  }
  
  switch(*seq){
  case _SEQ_INIT:
    //通信プレイヤー制御システムの生成
    GF_ASSERT_HEAVY(unisys->colosseum_sys == NULL);
    unisys->colosseum_sys = Colosseum_InitSystem(unisys->uniparent->game_data, 
      unisys->uniparent->gsys, unisys->uniparent->mystatus, situ->mycomm.intrude);

    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
      UnionMsg_GetMsgID_MultiStart(situ->mycomm.talk_pc->beacon.sex));

    (*seq)++;
    break;
  case _SEQ_INIT_TALK_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_WARP_BEFORE_TIMING:
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE, WB_NET_UNION);
    OS_TPrintf("コロシアム遷移前の同期取り開始\n");
    (*seq)++;
    break;
  case _SEQ_WARP_BEFORE_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("コロシアム遷移前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case _SEQ_WARP_SUBPROC:
    {
      int subproc_id;
      subproc_id = UNION_SUBPROC_ID_COLOSSEUM_WARP_START 
        + (situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_COLOSSEUM_START);
      UnionSubProc_EventSet(unisys, subproc_id, NULL);
    }
    (*seq)++;
    break;
  case _SEQ_WARP_SUBPROC_WAIT:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("コロシアム遷移のサブPROC終了\n");

      _PlayerMinePause(unisys, fieldWork, TRUE);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_MEMBER_WAIT);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * マルチ：コロシアム遷移：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_MultiColosseumUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  enum{
    _SEQ_INIT,
    _SEQ_INIT_TALK_WAIT,
    _SEQ_WARP_BEFORE_TIMING,
    _SEQ_WARP_BEFORE_TIMING_WAIT,
    _SEQ_WARP_SUBPROC,
    _SEQ_WARP_SUBPROC_WAIT,
  };

  if(*seq < _SEQ_WARP_SUBPROC && _UnionCheckError_ForceExit(unisys) == TRUE){ //コロシアムに遷移前なので_UnionCheckError_ForceExitでチェックする。※コロシアム用のは使用しないこと！
    return TRUE;
  }

  switch(*seq){
  case _SEQ_INIT:
    OS_TPrintf("マルチ対戦\n");
    if(situ->mycomm.intrude == FALSE){
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_MultiStart(situ->mycomm.talk_pc->beacon.sex));
    }
    else{
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, 
        UnionMsg_GetMsgID_MultiIntrudeSuccess(situ->mycomm.talk_pc->beacon.sex));
    }
    (*seq)++;
    break;
  case _SEQ_INIT_TALK_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_WARP_BEFORE_TIMING:
    if(situ->mycomm.intrude == FALSE){  //乱入の場合は同期取りなし
      GFL_NET_HANDLE_TimeSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE, WB_NET_UNION);
      OS_TPrintf("コロシアム遷移前の同期取り開始\n");
    }
    (*seq)++;
    break;
  case _SEQ_WARP_BEFORE_TIMING_WAIT:
    if(situ->mycomm.intrude == FALSE){  //乱入の場合は同期取りなし
      if(GFL_NET_HANDLE_IsTimeSync(
          GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE, WB_NET_UNION) == TRUE){
        OS_TPrintf("コロシアム遷移前の同期取り成功\n");
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case _SEQ_WARP_SUBPROC:
    {
      int subproc_id;
      subproc_id = UNION_SUBPROC_ID_COLOSSEUM_WARP_START 
        + (situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_COLOSSEUM_START);
      UnionSubProc_EventSet(unisys, subproc_id, NULL);
    }
    (*seq)++;
    break;
  case _SEQ_WARP_SUBPROC_WAIT:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("コロシアム遷移のサブPROC終了\n");

      //通信プレイヤー制御システムの生成
      GF_ASSERT_HEAVY(unisys->colosseum_sys == NULL);
      unisys->colosseum_sys = Colosseum_InitSystem(unisys->uniparent->game_data, 
        unisys->uniparent->gsys, unisys->uniparent->mystatus, situ->mycomm.intrude);

      _PlayerMinePause(unisys, fieldWork, TRUE);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_MEMBER_WAIT);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、メンバー集合待ち：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumMemberWaitUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  int i;
  
  GF_ASSERT_HEAVY(clsys != NULL);

  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    if(clsys->entry_menu != NULL){
      CommEntryMenu_Exit(clsys->entry_menu);
    }
    UnionMsg_AllDel(unisys);
    return TRUE;
  }
  
  switch(*seq){
  case 0:
    if(UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select) == 2){ //1vs1
      clsys->mine.entry_answer = COMM_ENTRY_ANSWER_COMPLETION;
      if(GFL_NET_IsParentMachine() == TRUE){
        if(UnionSend_ColosseumEntryAllReady() == TRUE){
          *seq = 100;
        }
      }
      else{
        *seq = 100;
      }
//      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_011);
      break;
    }
    
    //マルチ：募集処理を行う
    GFL_NET_SetNoChildErrorCheck(FALSE);  //募集中は子機が抜けてもエラーにしない
    if(GFL_NET_IsParentMachine() == TRUE){
      clsys->entry_menu = CommEntryMenu_Setup(
        GAMEDATA_GetMyStatus(unisys->uniparent->game_data), fieldWork, 
        UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select),
        UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), HEAPID_UNION,
        COMM_ENTRY_MODE_PARENT, COMM_ENTRY_GAMETYPE_COLOSSEUM, NULL);
      CommEntryMenu_SetCallback_EntryLeave(clsys->entry_menu, unisys, 
        _CommEntryMenuCallback_Entry, _CommEntryMenuCallback_Leave);
    }
    else{
//      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_011);
      if(situ->mycomm.intrude == FALSE){
        clsys->entry_menu = CommEntryMenu_Setup(
          GAMEDATA_GetMyStatus(unisys->uniparent->game_data), fieldWork, 
          UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select),
          UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), HEAPID_UNION,
          COMM_ENTRY_MODE_CHILD_PARENT_CONNECTED, COMM_ENTRY_GAMETYPE_COLOSSEUM, NULL);
      }
      else{
        clsys->entry_menu = CommEntryMenu_Setup(
          GAMEDATA_GetMyStatus(unisys->uniparent->game_data), fieldWork, 
          UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select),
          UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), HEAPID_UNION,
          COMM_ENTRY_MODE_CHILD_PARENT_DESIGNATE, COMM_ENTRY_GAMETYPE_COLOSSEUM, NULL);
      }
    }

    if(situ->mycomm.intrude == FALSE){
      //乱入可のゲームは乱入を受け入れる準備が出来たら専用のGSIDに変える(親子共に)
      Union_ChangePlayCategoryGSID(situ->play_category);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), 
        UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_SETUP, WB_NET_UNION);
    }
    
    (*seq)++;
    break;
  case 1:
    if(situ->mycomm.intrude == FALSE){
      if(GFL_NET_HANDLE_IsTimeSync(
          GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_SETUP, WB_NET_UNION) == TRUE){
        GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), TRUE);  //乱入許可
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 2:
    (*seq)++;
    break;
  case 3:
    {
      COMM_ENTRY_RESULT entry_result;
      
      entry_result = CommEntryMenu_Update(clsys->entry_menu);
      if(entry_result == COMM_ENTRY_RESULT_NULL){
        break;
      }
      
      clsys->entry_bit = CommEntryMenu_Exit(clsys->entry_menu);
      clsys->entry_menu = NULL;
      switch(entry_result){
      case COMM_ENTRY_RESULT_SUCCESS:      //メンバーが集まった
        (*seq) = 100;
        GFL_NET_SetNoChildErrorCheck(TRUE);  //フリームーブになったので再び子機が抜けたらエラー
        break;
      case COMM_ENTRY_RESULT_CANCEL:       //キャンセルして終了
        (*seq) = 200;
        break;
      case COMM_ENTRY_RESULT_NG:          //断られた
        (*seq) = 200;
        break;
      case COMM_ENTRY_RESULT_ERROR:        //エラーで終了
        (*seq) = 200;
        break;
      default:
        GF_ASSERT_HEAVY(0);
      }
    #if 0
      for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
        COMM_ENTRY_ANSWER answer;
        answer = CommEntryMenu_GetAnswer(clsys->entry_menu, i, FALSE);
        if(answer != COMM_ENTRY_ANSWER_NULL){
          if(UnionSend_ColosseumEntryAnswer(i, answer) == TRUE){
            CommEntryMenu_SetSendFlag(clsys->entry_menu, i);
          }
        }
      }
    #endif
    }
    break;
  case 4:
  #if 0
    {
      int count = 0;
      for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
        COMM_ENTRY_ANSWER answer;
        answer = CommEntryMenu_GetAnswer(clsys->entry_menu, i, TRUE);
        if(answer != COMM_ENTRY_ANSWER_NULL){
          count++;
          if(UnionSend_ColosseumEntryAnswer(i, answer) == TRUE){
            CommEntryMenu_SetSendFlag(clsys->entry_menu, i);
          }
        }
      }
      
      //全員にエントリー結果を送信済みで、接続人数と受け入れ人数が一致していれば次に進む
      if(count == 0 
          && (GFL_NET_GetConnectNum() == CommEntryMenu_GetCompletionNum(clsys->entry_menu))){
        if(UnionSend_ColosseumEntryAllReady() == TRUE){
          CommEntryMenu_Exit(clsys->entry_menu);
          clsys->entry_menu = NULL;
          (*seq) = 100;
        }
      }
    }
  #endif
    break;
    
  case 100: //集まった
    clsys->mine.entry_answer = COMM_ENTRY_ANSWER_COMPLETION;
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_FIRST_DATA_SHARING);
    return TRUE;
  
  case 200: //キャンセル
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_019);
    clsys->mine.entry_answer = COMM_ENTRY_ANSWER_REFUSE;
    (*seq)++;
    break;
  case 201:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
      _PlayerMinePause(unisys, fieldWork, TRUE);
      return TRUE;
    }
    break;
  case 300:
    clsys->mine.entry_answer = COMM_ENTRY_ANSWER_REFUSE;
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
    _PlayerMinePause(unisys, fieldWork, TRUE);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * 募集画面：ユーザー入室コールバック
 *
 * @param   work		      UNION_SYSTEM_PTR
 * @param   myst		      入室してきたプレイヤー
 * @param   mac_address		入室してきたプレイヤーのMacAddress
 */
//--------------------------------------------------------------
static void _CommEntryMenuCallback_Entry(void *work, const MYSTATUS *myst, const u8 *mac_address)
{
  UNION_SYSTEM_PTR unisys = work;
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  UnionMyComm_PartyAddParam(&situ->mycomm, mac_address, 
    MyStatus_GetTrainerView(myst), MyStatus_GetMySex(myst));
}

//--------------------------------------------------------------
/**
 * 募集画面：ユーザー退室コールバック
 *
 * @param   work		      UNION_SYSTEM_PTR
 * @param   myst		      退室したプレイヤー
 * @param   mac_address		退室したプレイヤーのMacAddress
 */
//--------------------------------------------------------------
static void _CommEntryMenuCallback_Leave(void *work, const MYSTATUS *myst, const u8 *mac_address)
{
  UNION_SYSTEM_PTR unisys = work;
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  UnionMyComm_PartyDelParam(&situ->mycomm, mac_address);
}

//--------------------------------------------------------------
/**
 * コロシアム、最初のデータ共有：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumFirstDataSharingUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());

  GF_ASSERT_HEAVY(clsys != NULL);

  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }
  
  switch(*seq){
  case 0:
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER, WB_NET_UNION);
    OS_TPrintf("最初のデータ共有前の同期取り開始\n");
    (*seq)++;
    break;
  case 1:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER, WB_NET_UNION) == TRUE){
      OS_TPrintf("最初のデータ共有前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case 2:
    OS_TPrintf("コロシアムの通信テーブルAdd\n");
    Colosseum_AddCommandTable(unisys);
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER, WB_NET_UNION);
    OS_TPrintf("通信テーブルをAddしたので同期取り開始\n");
    (*seq)++;
    break;
  case 3:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER, WB_NET_UNION) == TRUE){
      OS_TPrintf("通信テーブルをAddしたので同期取り成功\n");
      (*seq)++;
    }
    break;
  case 4:
    OS_TPrintf("全員分の基本情報を交換し合います\n");
    if(ColosseumSend_BasicStatus(&clsys->basic_status[my_net_id], FALSE) == TRUE){
      (*seq)++;
    }
    break;
  case 5:
    if(ColosseumTool_AllReceiveCheck_BasicStatus(clsys) == TRUE){
      int i, member_num;
      member_num = GFL_NET_GetConnectNum();
      for(i = 0; i < member_num; i++){
        if(clsys->basic_status[i].occ == TRUE){
          UnionMyComm_PartyAddParam(&situ->mycomm, clsys->basic_status[i].mac_address, 
            MyStatus_GetTrainerView(&clsys->basic_status[i].myst), 
            MyStatus_GetMySex(&clsys->basic_status[i].myst));
        }
      }
      OS_TPrintf("全員分の基本情報を受信完了\n");
      (*seq)++;
    }
    break;
  case 6:
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_CARD_BEFORE, WB_NET_UNION);
    OS_TPrintf("トレーナーカード交換前の同期取り開始\n");
    (*seq)++;
    break;
  case 7:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_CARD_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("トレーナーカード交換前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case 8:
    OS_TPrintf("全員分のトレーナーカードを交換し合います\n");
    if(ColosseumSend_TrainerCard(clsys->recvbuf.tr_card[my_net_id]) == TRUE){
      (*seq)++;
    }
    break;
  case 9:
    if(ColosseumTool_AllReceiveCheck_TrainerCard(clsys) == TRUE){
      OS_TPrintf("全員分のトレーナーカードを受信完了\n");
      (*seq)++;
    }
    break;
  case 10:
    OS_TPrintf("コロシアムの準備OK! 歩き回り開始！\n");
    { //知り合い登録
      GAMEDATA *gamedata = GAMESYSTEM_GetGameData(unisys->uniparent->gsys);
      ETC_SAVE_WORK *etc_save = SaveData_GetEtc( GAMEDATA_GetSaveControlWork(gamedata) );
      int member_num = GFL_NET_GetConnectNum();
      int i;
      for(i = 0; i < member_num; i++){
        if(clsys->basic_status[i].occ == TRUE 
            && i != GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
          EtcSave_SetAcquaintance(etc_save, MyStatus_GetID(&clsys->basic_status[i].myst));
        }
      }
    }
    Colosseum_CommReadySet(clsys, TRUE);
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_NORMAL);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、フリー移動：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  UnionMsg_AllDel(unisys);
  _PlayerMinePause(unisys, fieldWork, FALSE);

  return TRUE;
}

//--------------------------------------------------------------
/**
 * コロシアム、フリー移動：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumNormal(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  s16 check_gx, check_gy, check_gz;
  u32 out_index;
  
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  switch(*seq){
  case 0:
    {
      int stand_pos;
   
      //立ち位置チェック
      if(ColosseumTool_CheckStandingPosition(fieldWork, 
          UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), &stand_pos) == TRUE){
        Colosseum_Mine_SetStandingPostion(clsys, stand_pos);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDPOSITION);
        _PlayerMinePause(unisys, fieldWork, TRUE);
        return TRUE;
      }
      
      //誰かの退出受信チェック
      if(ColosseumTool_ReceiveCheck_Leave(clsys) == TRUE){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
        _PlayerMinePause(unisys, fieldWork, TRUE);
        return TRUE;
      }
      
      //自分の出口到達チェック
      if(ColosseumTool_CheckWayOut(fieldWork) == TRUE){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE_YESNO);
        _PlayerMinePause(unisys, fieldWork, TRUE);
        return TRUE;
      }
      
      //話しかけチェック
      if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
        u32 search_start_no = 0;
        FIELD_PLAYER_GetFrontGridPos(player, &check_gx, &check_gy, &check_gz);
        if(CommPlayer_SearchGridPos(clsys->cps, check_gx, check_gy, check_gz, &out_index, &search_start_no) == TRUE){
          clsys->talk_obj_id = out_index;
          OS_TPrintf("ターゲット発見! net_id = %d, gx=%d, gz=%d\n", 
            clsys->talk_obj_id, check_gx, check_gz);
          UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_TRAINER_CARD);
          _PlayerMinePause(unisys, fieldWork, TRUE);
          return TRUE;
        }
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、立ち位置にたった：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumStandPosition(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  switch(*seq){
  case 0:
//    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_012);
    (*seq)++;
    break;
  case 1:
    if(ColosseumSend_StandingPositionConfirm(clsys) == TRUE){
      OS_TPrintf("親に立ち位置の許可要求を送信\n");
      (*seq)++;
    }
    break;
  case 2:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    
    {
      int ret;
      ret = Colosseum_Mine_GetAnswerStandingPosition(clsys);
      if(ret == TRUE){
        OS_TPrintf("立ち位置OK\n");
        (*seq)++;
      }
      else if(ret == FALSE){
        OS_TPrintf("立ち位置NG\n");
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
        return TRUE;
      }
    }
    break;
  case 3: //立ち位置OK
    {
      FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
      MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);
      int stand_pos, anm_code;
      
      if(MMDL_CheckPossibleAcmd(player_mmdl) == TRUE){
        ColosseumTool_CheckStandingPosition(
          fieldWork, UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), &stand_pos);
        anm_code = (stand_pos & 1) ? AC_DIR_L : AC_DIR_R;
        MMDL_SetAcmd(player_mmdl, anm_code);
        (*seq)++;
      }
      else{
        OS_TPrintf("MMDL_CheckPossibleAcmd待ち\n");
      }
    }
    break;
  case 4:
    {
      FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
      MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);

      if(MMDL_CheckEndAcmd(player_mmdl) == TRUE){
        MMDL_EndAcmd(player_mmdl);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_USE_PARTY_SELECT);
        return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、立ち位置から後退する：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumStandingBack(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);
  int stand_pos;
  u16 anm_code;
  
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  switch(*seq){
  case 0:
    Colosseum_Mine_SetStandingPostion(clsys, COLOSSEUM_STANDING_POSITION_NULL);
    (*seq)++;
    break;
  case 1:
    if(ColosseumSend_StandingPositionConfirm(clsys) == TRUE){
      OS_TPrintf("親に立ち位置から退きます。を送信\n");
      (*seq)++;
    }
    break;
  case 2:
    if(Colosseum_Mine_GetAnswerStandingPosition(clsys) != COLOSSEUM_STANDING_POSITION_NULL){
      OS_TPrintf("受信：どいてOK\n");
      (*seq)++;
    }
    break;
  case 3:
    UnionMsg_AllDel(unisys);
    (*seq)++;
    break;
  case 4:
    if(MMDL_CheckPossibleAcmd(player_mmdl) == TRUE){
      ColosseumTool_CheckStandingPosition(
        fieldWork, UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), &stand_pos);
      anm_code = (stand_pos & 1) ? AC_WALK_R_16F : AC_WALK_L_16F;
      MMDL_SetAcmd(player_mmdl, anm_code);
      (*seq)++;
    }
    else{
      OS_TPrintf("MMDL_CheckPossibleAcmd待ち\n");
    }
    break;
  case 5:
    if(MMDL_CheckEndAcmd(player_mmdl) == TRUE){
      MMDL_EndAcmd(player_mmdl);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_NORMAL);
      return TRUE;
    }
    break;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、手持ち、バトルボックス選択：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumUsePartySelect(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  enum{
    _INIT,
    _FIRST_CHECK,
    _MENU,
    _MENU_WAIT,
    _REG_CHECK,
    _REG_FAIL_PRINT_WAIT,
    _REG_FAIL_PRINT_KEY_WAIT,
  };
  
#if 0 //通信しているものがないのでチェックしない
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }
#endif

  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }
  
  switch(*seq){
  case _INIT:
    situ->work = 0;
    (*seq)++;
    //break;
  case _FIRST_CHECK:
    {
      SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data);
      BATTLE_BOX_SAVE *bb_save = BATTLE_BOX_SAVE_GetBattleBoxSave( sv_ctrl );
      if(BATTLE_BOX_SAVE_IsIn( bb_save ) == TRUE){
#if 0
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_battle_01_22);
#endif
        (*seq)++;
      }
      else{ //バトルボックスのセーブデータが存在していないなら手持ち選択したことにする
        clsys->select_pokeparty = COLOSSEUM_SELECT_PARTY_TEMOTI;
        situ->work++; //手持ちしかない事の印
        *seq = _REG_CHECK;
      }
    }
    break;

  case _MENU:   //メインメニュー描画
    {
      u32 temoti_ng_bit, bbox_ng_bit;
      BOOL temoti_fail, bbox_fail;
      
      //POKEPARTY選択リスト表示前にこちらで使っている関連したものを全て解放
      UnionMsg_AllDel(unisys);
      
      _CheckRegulation_Temoti(unisys, &temoti_ng_bit);
      _CheckRegulation_BBox(unisys, &bbox_ng_bit);
      temoti_fail = temoti_ng_bit > 0 ? TRUE : FALSE;
      bbox_fail = bbox_ng_bit > 0 ? TRUE : FALSE;
      unisys->alloc.bbox_party = _BBox_PokePartyAlloc(unisys);
      unisys->alloc.psl = PARTY_SELECT_LIST_Setup(fieldWork, 
        GAMEDATA_GetMyPokemon(unisys->uniparent->game_data),
        unisys->alloc.bbox_party, temoti_fail, bbox_fail, HEAPID_FIELDMAP);
    }
    (*seq)++;
    break;
        
  case _MENU_WAIT:
    if(PARTY_SELECT_LIST_Main(unisys->alloc.psl) == TRUE){
      BOOL reg_ng;
      SELECT_PARTY select_party = PARTY_SELECT_LIST_Exit(unisys->alloc.psl, &reg_ng);
      unisys->alloc.psl = NULL;
      if(select_party == SELECT_PARTY_CANCEL){
        OS_TPrintf("メニューをキャンセルしました\n");
        _BBox_PokePartyFree(unisys->alloc.bbox_party);
        unisys->alloc.bbox_party = NULL;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
        return TRUE;
      }
      else{
        switch(select_party){
        case SELECT_PARTY_TEMOTI:
          clsys->select_pokeparty = COLOSSEUM_SELECT_PARTY_TEMOTI;
          break;
        case SELECT_PARTY_BBOX:
          clsys->select_pokeparty = COLOSSEUM_SELECT_PARTY_BOX;
          break;
        }
        _BBox_PokePartyFree(unisys->alloc.bbox_party);
        unisys->alloc.bbox_party = NULL;
        (*seq)++;
      }
    }
    break;

  case _REG_CHECK:
    {
      POKE_REG_RETURN_ENUM reg_ret = 0;
      u32 fail_bit = 0;
      REGWIN_TYPE regwin_type;
      
      if(clsys->select_pokeparty == COLOSSEUM_SELECT_PARTY_TEMOTI){
        reg_ret = _CheckRegulation_Temoti(unisys, &fail_bit);
        regwin_type = REGWIN_TYPE_NG_TEMOTI;
      }
      else{
        reg_ret = _CheckRegulation_BBox(unisys, &fail_bit);
        regwin_type = REGWIN_TYPE_NG_BBOX;
      }
      
      if(fail_bit > 0){
        //NGレギュレーションの表示
        UnionMsg_TalkStream_WindowSetup(unisys, fieldWork); //msgdataなどのセット
        UnionMsg_Menu_RegulationSetup(unisys, fieldWork, fail_bit, 
          Union_GetPlayCategory_to_Shooter(situ->play_category), regwin_type);
        *seq = _REG_FAIL_PRINT_WAIT;
      }
      else{
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_POKELIST_READY);
        return TRUE;
      }
    }
    break;
  case _REG_FAIL_PRINT_WAIT:
    if(UnionMsg_Menu_RegulationWait(unisys, fieldWork) == TRUE){
      //参加条件を満たしていない
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_battle_01_26);
      (*seq)++;
    }
    break;
  case _REG_FAIL_PRINT_KEY_WAIT:
    if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)){
      UnionMsg_Menu_RegulationDel(unisys);
      if(situ->work == 0){
        *seq = _INIT;
      }
      else{ //手持ちしかなくてレギュレーションに引っかかっているので後退するしかない
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
        return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、ポケモンリスト呼び出し前の全員待ち：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumPokelistReady(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  if(clsys->all_battle_ready == TRUE){
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_POKELIST_BEFORE_DATA_SHARE);
    clsys->all_battle_ready = FALSE;
    clsys->mine.battle_ready = FALSE;
    return TRUE;
  }
    
  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_battle_01_27);
    (*seq)++;
    break;
  case 1:
    if(ColosseumSend_BattleReady() == TRUE){
      clsys->mine.battle_ready = TRUE;
      (*seq)++;
    }
    break;
  case 2:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
        *seq = 100;
        break;
      }
    }
    break;
  case 100:
    if(ColosseumSend_BattleReadyCancel() == TRUE){
      (*seq)++;
    }
    break;
  case 101:
    if(clsys->mine.battle_ready == FALSE){
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * レギュレーションに沿ってPOKEPARTYのレベルを補正
 *
 * @param   regulation    
 * @param   party   
 */
//--------------------------------------------------------------
static void _PokeParty_RegulationLevelRevise(REGULATION* regulation, POKEPARTY *party)
{
  int party_max, i, level;
  int reg_range, reg_level;
  POKEMON_PARAM *pp;
  
  reg_range = Regulation_GetParam( regulation, REGULATION_LEVEL_RANGE );
  reg_level = Regulation_GetParam( regulation, REGULATION_LEVEL );
  
  party_max = PokeParty_GetPokeCount(party);
  for(i = 0; i < party_max; i++){
    pp = PokeParty_GetMemberPointer( party, i );
    level = PP_Get(pp, ID_PARA_level, NULL);
    switch(reg_range){
    case REGULATION_LEVEL_RANGE_DRAG_DOWN:  //引き下げ
      if(level > reg_level){
        POKETOOL_MakeLevelRevise(pp, reg_level);
      }
      break;
    case REGULATION_LEVEL_RANGE_SAME:         //全補正
      if(level != reg_level){
        POKETOOL_MakeLevelRevise(pp, reg_level);
      }
      break;
    case REGULATION_LEVEL_RANGE_PULL_UP:     //引き上げ
      if(level < reg_level){
        POKETOOL_MakeLevelRevise(pp, reg_level);
      }
      break;
    }
  }
}

//--------------------------------------------------------------
/**
 * コロシアム、ポケモンリスト呼び出し前の全員データ送受信：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumPokelistBeforeDataShare(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  switch(*seq){
  case 0:
    //自分の受信バッファに送信データとなるPOKEPARTYをセット
    switch(clsys->select_pokeparty){
    default:
      GF_ASSERT(0);
    case COLOSSEUM_SELECT_PARTY_TEMOTI:
      PokeParty_Copy(GAMEDATA_GetMyPokemon(unisys->uniparent->game_data), 
        clsys->recvbuf.pokeparty[my_net_id]);
      break;
    case COLOSSEUM_SELECT_PARTY_BOX:
      {
        POKEPARTY *bbox_party = _BBox_PokePartyAlloc(unisys);
        PokeParty_Copy(bbox_party, clsys->recvbuf.pokeparty[my_net_id]);
        _BBox_PokePartyFree(bbox_party);
      }
      break;
    }
    //レベル補正
//    _PokeParty_RegulationLevelRevise(
//      unisys->alloc.regulation, clsys->recvbuf.pokeparty[my_net_id]);
    PokeRegulation_ModifyLevelPokeParty( 
      unisys->alloc.regulation, clsys->recvbuf.pokeparty[my_net_id] );

    (*seq)++;
    break;
  case 1:
    Colosseum_Clear_ReceivePokeParty(clsys, TRUE);
    ColosseumTool_Clear_ReceiveStandingPos(clsys);
    ColosseumTool_Clear_ReceivePokeListSelected(clsys);

    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_LIST_POKEPARTY_BEFORE, WB_NET_UNION);
    OS_TPrintf("リスト用のPOKEPARTY送受信前の同期取り開始\n");
    (*seq)++;
    break;
  case 2:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_LIST_POKEPARTY_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("リスト用のPOKEPARTY送受信前の同期取り成功\n");
      (*seq)++;
    }
    break;
  case 3:
    if(ColosseumSend_Pokeparty(clsys->recvbuf.pokeparty[my_net_id]) == TRUE){
      OS_TPrintf("リスト用POKEPARTY送信\n");
      (*seq)++;
    }
    break;
  case 4:
    if(Colosseum_AllReceiveCheck_Pokeparty(clsys) == TRUE){
      OS_TPrintf("全員分のリスト用POKEPARTY受信\n");
      (*seq)++;
    }
    break;
  case 5:
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_STANDINGPOS_BEFORE, WB_NET_UNION);
    OS_TPrintf("立ち位置送信前の同期開始\n");
    (*seq)++;
    break;
  case 6:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_STANDINGPOS_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("立ち位置送信前の同期成功\n");
      (*seq)++;
    }
    break;
  case 7:
    if(GFL_NET_IsParentMachine() == TRUE){
      if(ColosseumSend_StandingPos(clsys->parentsys.stand_position) == TRUE){
        OS_TPrintf("みんなの立ち位置を親機が送信\n");
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 8:
    if(ColosseumTool_ReceiveCheck_StandingPos(clsys) == TRUE){
      OS_TPrintf("立ち位置受信\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_POKELIST_BATTLE);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、ポケモンリスト呼び出し：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumPokelistBattle(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  PLIST_DATA *plist;
  UNION_SUBPROC_PARENT_POKELIST *parent_list;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  enum{
    _SEQ_INIT,
    _SEQ_INIT_TALK_WAIT,
    _SEQ_INIT_TIMING_WAIT,
    _SEQ_SUBPROC_CALL,
    _SEQ_SUBPROC_WAIT,
  };
  
  if(*seq < _SEQ_SUBPROC_CALL && _UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  switch(*seq){
  case _SEQ_INIT: //「参加するポケモンを選んでください」
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      u32 msg_id;
      switch(situ->mycomm.mainmenu_select){
      default:
        GF_ASSERT(0);
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
        msg_id = msg_union_battle_01_28;  //ポケモンを選んだください
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT:
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
        msg_id = msg_union_battle_01_29;  //3匹選んで下さい
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
        msg_id = msg_union_battle_01_33;  //3匹まで選んで下さい
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT:
        msg_id = msg_union_battle_01_30;  //4匹選んで下さい
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT:
        msg_id = msg_union_battle_01_31;  //6匹選んで下さい
        break;
      }
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_id);
      (*seq)++;
    }
    break;
  case _SEQ_INIT_TALK_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      GFL_NET_HANDLE_TimeSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_POKELIST_BEFORE, WB_NET_UNION);
      (*seq)++;
    }
    break;
  case _SEQ_INIT_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(), 
        UNION_TIMING_COLOSSEUM_POKELIST_BEFORE, WB_NET_UNION) == TRUE){
      UnionMsg_AllDel(unisys);
      (*seq)++;
    }
    break;
  
  case _SEQ_SUBPROC_CALL: //ポケモンリスト画面呼び出し
    parent_list = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(UNION_SUBPROC_PARENT_POKELIST));

    plist = &parent_list->plist;
    plist->pp = clsys->recvbuf.pokeparty[my_net_id];
    plist->myitem = GAMEDATA_GetMyItem(unisys->uniparent->game_data);
    plist->cfg = SaveData_GetConfig(GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data));
    plist->mode = PL_MODE_BATTLE_WIFI;
    plist->reg = unisys->alloc.regulation;

    parent_list->battle_setup 
      = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(COLOSSEUM_BATTLE_SETUP));
    
    {//通信相手の情報をセット
      int net_id, enemy_player = 0;
      PL_COMM_PLAYER_TYPE comm_player;
      
      for(net_id = 0; net_id < GFL_NET_GetConnectNum(); net_id++){
        if(net_id != my_net_id){
          if((clsys->recvbuf.stand_position[my_net_id] & 1) == (clsys->recvbuf.stand_position[net_id] & 1)){
            comm_player = PL_COMM_PLAYER_TYPE_ALLY;
          }
          else{
            comm_player = PL_COMM_PLAYER_TYPE_ENEMY_A + enemy_player;
            enemy_player++;
          }
          plist->comm_battle[comm_player].pp = clsys->recvbuf.pokeparty[net_id];
          plist->comm_battle[comm_player].name 
            = MyStatus_GetMyName(&clsys->basic_status[net_id].myst);
          plist->comm_battle[comm_player].sex 
            = MyStatus_GetMySex(&clsys->basic_status[net_id].myst);
          OS_TPrintf("plist set net_id = %d, comm_player=%d, pp=%x\n", net_id, comm_player, clsys->recvbuf.pokeparty[net_id]);
        }
      }
    }
    switch(situ->mycomm.mainmenu_select){
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
      plist->comm_type = PL_COMM_MULTI;
      break;
    default:
      plist->comm_type = PL_COMM_SINGLE;
      break;
    }
    plist->is_disp_party = TRUE;
    plist->use_tile_limit = FALSE;
    plist->time_limit = 0;
    
    unisys->parent_work = parent_list;
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_POKELIST_BATTLE, parent_list);

    (*seq)++;
    break;
  case _SEQ_SUBPROC_WAIT:
    if(UnionSubProc_IsExits(unisys) == TRUE){
      break;
    }
    
    parent_list = unisys->parent_work;
    GFL_HEAP_FreeMemory(parent_list->battle_setup);
    GFL_HEAP_FreeMemory(unisys->parent_work);
    unisys->parent_work = NULL;

    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム、出口座標にたった：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumLeaveYesNoUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);
  enum{
    _SEQ_MSG,
    _SEQ_MSG_WAIT,
    _SEQ_YESNO_WAIT,
    _SEQ_UP_MOVE,
    _SEQ_UP_MOVE_WAIT,
  };
  
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }

  switch(*seq){
  case _SEQ_MSG:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_021);
    (*seq)++;
    break;
  case _SEQ_MSG_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }

    UnionMsg_YesNo_Setup(unisys, fieldWork);
    (*seq)++;
    break;
  case _SEQ_YESNO_WAIT:
    {
      BOOL result;
      if(UnionMsg_YesNo_SelectLoop(unisys, &result) == TRUE){
        UnionMsg_YesNo_Del(unisys);
        if(result == FALSE){  //「いいえ」：上に移動させる
          *seq = _SEQ_UP_MOVE;
        }
        else{
          UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
          return TRUE;
        }
      }
    }
    break;
  case _SEQ_UP_MOVE:
    if(MMDL_CheckPossibleAcmd(player_mmdl) == TRUE){
      MMDL_SetAcmd(player_mmdl, AC_WALK_U_16F);
      (*seq)++;
    }
    break;
  case _SEQ_UP_MOVE_WAIT:
    if(MMDL_CheckEndAcmd(player_mmdl) == TRUE){
      MMDL_EndAcmd(player_mmdl);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_NORMAL);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアム退出処理：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumLeaveUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  enum{
    LEAVE_SEQ_INIT,
    LEAVE_SEQ_SEND_LEAVE,
    LEAVE_SEQ_WAIT_ALL_LEAVE,
    LEAVE_SEQ_MSG_WAIT,
    LEAVE_SEQ_TIMING_START,
    LEAVE_SEQ_TIMING_WAIT,
    LEAVE_SEQ_SHUTDOWN_INIT,
    LEAVE_SEQ_SHUTDOWN,
    LEAVE_SEQ_SHUTDOWN_WAIT,
    LEAVE_SEQ_WARP_UNION,
    LEAVE_SEQ_WARP_UNION_WAIT,
    LEAVE_SEQ_BEACON_RESTART,
  };
  
  if(*seq < LEAVE_SEQ_SHUTDOWN && NetErr_App_CheckError() != NET_ERR_CHECK_NONE){
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      _PlayerMinePause(unisys, fieldWork, TRUE);
      *seq = LEAVE_SEQ_SHUTDOWN;
    }
  }

  switch(*seq){
  case LEAVE_SEQ_INIT:
    _PlayerMinePause(unisys, fieldWork, TRUE);
    if(clsys->mine.entry_answer == COMM_ENTRY_ANSWER_REFUSE){
      if(UnionMsg_TalkStream_Check(unisys) == TRUE){
        *seq = LEAVE_SEQ_SHUTDOWN_INIT;
      }
    }
    else{
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_019);
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_SEND_LEAVE:
    if(ColosseumSend_Leave() == TRUE){
      OS_TPrintf("「退出します」送信完了\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_WAIT_ALL_LEAVE:
    if(ColosseumTool_AllReceiveCheck_Leave(clsys) == TRUE){
      OS_TPrintf("全員分の「退出します」を受信完了\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_MSG_WAIT:
    //同期後は全員同時に切断が出来るように、先にメッセージの終了を待ってから同期を始める
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_TIMING_START:
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_LEAVE, WB_NET_UNION);
    OS_TPrintf("コロシアム切断前の同期取り：開始\n");
    (*seq)++;
    break;
  case LEAVE_SEQ_TIMING_WAIT:
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_LEAVE, WB_NET_UNION) == TRUE){
      OS_TPrintf("コロシアム切断前の同期取り：成功\n");
      GFL_NET_SetNoChildErrorCheck(FALSE);  //切断許可
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_SHUTDOWN_INIT:
    //親機は最後まで待つ
    if(GFL_NET_IsParentMachine() == TRUE){
      int s;
      u32 entry_bit = clsys->entry_bit;
      if(GFL_NET_SystemGetConnectNum() <= 1){
        (*seq)++;
        break;
      }
      for(s = 0; s < UNION_CONNECT_PLAYER_NUM; s++){
        if(GFL_NET_SystemGetCurrentID() != s){  //自分は飛ばす
          if((entry_bit & (1 << s)) && GFL_NET_IsConnectMember(s) == TRUE){
            break;
          }
        }
      }
      if(s == UNION_CONNECT_PLAYER_NUM){//コロシアム内で接続扱いになっていた全員が離脱完了している
        (*seq)++;
        break;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_SHUTDOWN:
    if(clsys->parent_force_exit == TRUE && UnionComm_Check_ShutdownRestarts(unisys) == TRUE){
      //union_comm.cでの親機によるフリームーブ中の強制切断がある為、ここで処理待ちを入れておく
      return FALSE;
    }
    
    if(NetErr_App_CheckError() != NET_ERR_CHECK_NONE){
      UnionMsg_AllDel(unisys);
      GAMESYSTEM_SetFieldCommErrorReq(unisys->uniparent->gsys, TRUE);
      return FALSE;
    }

    UnionComm_Req_Shutdown(unisys);
    (*seq)++;
    break;
  case LEAVE_SEQ_SHUTDOWN_WAIT:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      OS_TPrintf("コロシアム切断完了\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_WARP_UNION:
    clsys->colosseum_leave = TRUE;
    UnionBeacon_ClearAllReceiveData(unisys);
    UNION_CHARA_WorkInit(unisys);
    UnionMyComm_Init(unisys, &situ->mycomm);
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_UNION_WARP, NULL);
    (*seq)++;
    break;
  case LEAVE_SEQ_WARP_UNION_WAIT:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("ユニオン遷移のサブPROC終了\n");

      if(unisys->colosseum_sys != NULL){
        GF_ASSERT(0);
        Colosseum_ExitSystem(unisys->colosseum_sys, unisys->uniparent->gsys);
        unisys->colosseum_sys = NULL;
      }
      
      UnionComm_Req_Restarts(unisys);
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_BEACON_RESTART:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      OS_TPrintf("ビーコンスキャン再開完了\n");
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * コロシアムでのトレーナーカード呼び出し：更新
 *
 * @param   unisys    
 * @param   situ    
 * @param   fieldWork   
 * @param   seq   
 *
 * @retval  BOOL    
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumTrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  u32 msg_id;
  
#if 0 //通信しないのでチェックしない
  if(_UnionCheckError_ColosseumForceExit(unisys) == TRUE){
    UnionMsg_AllDel(unisys);
    return TRUE;
  }
#endif

  switch(*seq){
  case 0:
    _PlayerMinePause(unisys, fieldWork, TRUE);
    {
      int pm_version = MyStatus_GetRomCode(&clsys->basic_status[clsys->talk_obj_id].myst);
      if ( (pm_version == VERSION_BLACK) || (pm_version == VERSION_WHITE) )     //ブラック・ホワイトバージョン
      {
        TR_CARD_RANK card_rank = clsys->recvbuf.tr_card[clsys->talk_obj_id]->CardRank;
        if(card_rank < TR_CARD_RANK_BLACK) msg_id = msg_union_card_01 + card_rank;
        else if(card_rank == TR_CARD_RANK_BLACK)
        {
          switch(pm_version){
          case VERSION_BLACK:
            msg_id = msg_union_card_07;
            break;
          case VERSION_WHITE:
            msg_id = msg_union_card_06;
            break;
          default:
            msg_id = msg_union_card_08;     //念のため
            break;
          }
        }
        else msg_id = msg_union_card_08;    //念のため
      }
      else      //それ以外のバージョン
      {
        msg_id = msg_union_card_08;
      }
    }
    UnionMsg_TalkStream_WindowSetup(unisys, fieldWork);
    WORDSET_RegisterPlayerName(unisys->wordset, 0, &clsys->basic_status[clsys->talk_obj_id].myst);
    UnionMsg_TalkStream_Print(unisys, msg_id);
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE | PAD_BUTTON_CANCEL)){
        UnionMsg_AllDel(unisys);
        
        clsys->recvbuf.tr_card[clsys->talk_obj_id]->SignDisenable = TRUE;
        unisys->parent_work = TRAINERCASR_CreateCallParam_CommData(unisys->uniparent->game_data, 
          clsys->recvbuf.tr_card[clsys->talk_obj_id], HEAPID_UNION);

        UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_COLOSSEUM_TRAINERCARD, unisys->parent_work);
        (*seq)++;
      }
    }
    break;
  case 2:
    if(UnionSubProc_IsExits(unisys) == FALSE){
    #if 0//トレーナーカードのParentWorkはトレーナーカードのProc内で解放されるのでここでは解放しない
      GFL_HEAP_FreeMemory(unisys->parent_work);
    #endif
      unisys->parent_work = NULL;
      OS_TPrintf("サブPROC終了\n");
      (*seq)++;
    }
    break;
  case 3:
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_NORMAL);
    return TRUE;
  }
  
  return FALSE;
}
