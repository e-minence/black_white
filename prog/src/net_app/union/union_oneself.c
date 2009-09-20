//==============================================================================
/**
 * @file    union_oneself.c
 * @brief   ���j�I���F��������
 * @author  matsuda
 * @date    2009.07.09(��)
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

#include "field/event_ircbattle.h"
#include "net_app\irc_compatible.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V�[�P���X�ԍ�
enum{
  ONESELF_SUBPROC_INIT,
  ONESELF_SUBPROC_UPDATE,
  ONESELF_SUBPROC_EXIT,
};

///�b�������Ă��甽��������܂ł̃^�C���A�E�g����
#define ONESELF_SERVER_TIMEOUT      (30 * 20)


//==============================================================================
//  �^��`
//==============================================================================
///����^
typedef BOOL (*ONESELF_FUNC)(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);

///����Z�b�g
typedef struct{
  ONESELF_FUNC func_init;
  ONESELF_FUNC func_update;
  ONESELF_FUNC func_exit;
}ONESELF_FUNC_DATA;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static BOOL OneselfSeq_NormalInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_Leave(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
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
static BOOL OneselfSeq_ColosseumUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_MultiColosseumUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumMemberWaitUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumFirstDataSharingUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumNormal(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumStandPosition(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumPokelist(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumAllBattleReadyWait(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumBattle(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumStandingBack(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumUsePartySelect(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumPokelistReady(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumPokelistBeforeDataShare(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumLeaveUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumTrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq);


//==============================================================================
//  �f�[�^
//==============================================================================
static const ONESELF_FUNC_DATA OneselfFuncTbl[] = {
  {//UNION_STATUS_NORMAL
    OneselfSeq_NormalInit,
    OneselfSeq_NormalUpdate,
    NULL,
  },
  {//UNION_STATUS_ENTER
    NULL,
    NULL,
    NULL,
  },
  {//UNION_STATUS_LEAVE
    NULL,
    OneselfSeq_Leave,
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
    OneselfSeq_ShutdownUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_50_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_50
    NULL,
    OneselfSeq_ColosseumUpdate,
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
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_STANDARD_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_SINGLE_STANDARD
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_50_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_50
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
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_STANDARD_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_DOUBLE_STANDARD
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_50_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_50
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
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_STANDARD_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_TRIPLE_STANDARD
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_50_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_50
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
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_STANDARD_SHOOTER
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_1VS1_ROTATION_STANDARD
    NULL,
    OneselfSeq_ColosseumUpdate,
    NULL,
  },
  {//UNION_STATUS_BATTLE_MULTI
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
    OneselfSeq_ShutdownUpdate,
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
  {//UNION_STATUS_COLOSSEUM_POKELIST
    NULL,
    OneselfSeq_ColosseumPokelist,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_BATTLE_READY_WAIT
    NULL,
    OneselfSeq_ColosseumAllBattleReadyWait,
    NULL,
  },
  {//UNION_STATUS_COLOSSEUM_BATTLE
    NULL,
    OneselfSeq_ColosseumBattle,
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
 * ����UNION_STATUS���Z�b�g����
 *
 * @param   unisys		    
 * @param   req_status		UNION_STATUS_???
 *
 * @retval  BOOL		TRUE�F����
 */
//==================================================================
BOOL UnionOneself_ReqStatus(UNION_SYSTEM_PTR unisys, int req_status)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;

  situ->next_union_status = req_status;
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �I�����j���[���퓬�������ꍇ�A�Q���o���邩���M�����[�V�����`�F�b�N
 *
 * @param   menu_index		
 *
 * @retval  BOOL		TRUE:�Q��OK�@FALSE:�Q��NG
 */
//--------------------------------------------------------------
static BOOL Union_CheckEntryBattleRegulation(u32 menu_index)
{
  if(menu_index < UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50 
      || menu_index > UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD){
    return TRUE;  //�퓬�ł͂Ȃ��̂�TRUE
  }
  
  switch(menu_index){
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50_SHOOTER:        //�ΐ�:1VS1:�V���O��:LV50
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:        //�ΐ�:1VS1:�V���O��:LV50
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //�ΐ�:1VS1:�V���O��:�����Ȃ�
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:      //�ΐ�:1VS1:�V���O��:�����Ȃ�
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD_SHOOTER://�ΐ�:1VS1:�V���O��:�X�^���_�[�h
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:  //�ΐ�:1VS1:�V���O��:�X�^���_�[�h
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER:        //�ΐ�:1VS1:�_�u��:LV50
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:        //�ΐ�:1VS1:�_�u��:LV50
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //�ΐ�:1VS1:�_�u��:�����Ȃ�
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:      //�ΐ�:1VS1:�_�u��:�����Ȃ�
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD_SHOOTER:  //�ΐ�:1VS1:�_�u��:�X�^���_�[�h
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:  //�ΐ�:1VS1:�_�u��:�X�^���_�[�h
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:                 //�ΐ�:�}���`
    return TRUE;  //���̓��M�����[�V�����`�F�b�N�Ȃ��B���ڂ��������Ă����OK
  default:
    return FALSE;
  }
}

//--------------------------------------------------------------
/**
 * ���@�Ƀ|�[�Y��������
 *
 * @param   fieldWork		
 * @param   pause_flag		TRUE:�|�[�Y�@FALSE:�|�[�Y����
 */
//--------------------------------------------------------------
static void _PlayerMinePause(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, int pause_flag)
{
  unisys->player_pause = pause_flag;
  FIELDMAP_CTRL_GRID_SetPlayerPause( fieldWork, pause_flag );
}


//==============================================================================
//  
//==============================================================================
//--------------------------------------------------------------
/**
 * �ʏ���(�������Ă��Ȃ�)�F������
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
  

  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ʏ���(�������Ă��Ȃ�)�F�X�V
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
  
  //�O�����瓮�샊�N�G�X�g���󂯂Ă���Ȃ�؂�ւ���
  if(situ->next_union_status != UNION_STATUS_NORMAL){
    OS_TPrintf("NormalUpdate:�O�����N�G�X�g��M�B�؂�ւ��܂� next_status=%d\n", situ->next_union_status);
    return TRUE;
  }

  if(GFL_NET_GetConnectNum() > 1){
//    GF_ASSERT_MSG(0, "Normal�Ȃ̂ɐڑ��I�I�I�I\n");
    OS_TPrintf("Normal�Ȃ̂ɐڑ��I�I�I�I\n");
    //��check�@�b��Ő擪�̃r�[�R���f�[�^��ڑ�����Ƃ��đ�����Ă���
    //         �{���ł����Normal�Őڑ��͏o���Ȃ��悤�ɂ���
    UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
    UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, situ->last_calling_pc);
    _PlayerMinePause(unisys, fieldWork, TRUE);
    return TRUE;
  }
  
  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
    FIELD_PLAYER_GetFrontGridPos(player, &check_gx, &check_gy, &check_gz);
    target_pc = MMDLSYS_SearchGridPos(fldMdlSys, check_gx, check_gz, FALSE);
    if(target_pc == NULL){
      return FALSE;
    }
    
    obj_id = MMDL_GetOBJID(target_pc);
    situ->mycomm.talk_obj_id = obj_id;
    buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(obj_id);
    OS_TPrintf("�^�[�Q�b�g����! buf_no = %d, gx=%d, gz=%d\n", buf_no, check_gx, check_gz);
    if(UNION_CHARA_CheckCharaIndex(obj_id) == UNION_CHARA_INDEX_PARENT){
      if(unisys->receive_beacon[buf_no].beacon.play_category == UNION_PLAY_CATEGORY_UNION){
        UnionMySituation_SetParam(unisys, 
          UNION_MYSITU_PARAM_IDX_CALLING_PC, &unisys->receive_beacon[buf_no]);
        situ->last_calling_pc = &unisys->receive_beacon[buf_no];
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
  
  //�o���`�F�b�N
  if(UnionTool_CheckWayOut(fieldWork) == TRUE){
    if(unisys->debug_wayout_walk == TRUE){
      _PlayerMinePause(unisys, fieldWork, TRUE);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_LEAVE);
    }
  }
  else{
    unisys->debug_wayout_walk = TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���j�I�����[���ޏo�F�X�V
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
  unisys->finish = TRUE;
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ڑ����N�G�X�g�F������
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
  UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_000);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ڑ����N�G�X�g���s�F�X�V
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
    LOCALSEQ_END,
  };
  
  switch(*seq){
  case LOCALSEQ_INIT:
    OS_TPrintf("ChangeOver ���[�h�ؑցF�e�Œ�\n");
    GFL_NET_ChangeoverModeSet(GFL_NET_CHANGEOVER_MODE_FIX_PARENT, TRUE, NULL);
    situ->wait = 0;
    (*seq)++;
    break;
  case LOCALSEQ_WAIT:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    
    if(GFL_NET_GetConnectNum() > 1){
      OS_TPrintf("�ڑ����܂����I�F�e\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC,situ->mycomm.calling_pc);
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CALLING_PC, NULL);
      (*seq)++;
    }
    else{
      situ->wait++;
      if(situ->wait > ONESELF_SERVER_TIMEOUT || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)){
        GFL_NET_ChangeoverModeSet(GFL_NET_CHANGEOVER_MODE_NORMAL, FALSE, NULL);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CALLING_PC, NULL);
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_007);
        OS_TPrintf("�q�����Ȃ������׃L�����Z�����܂���\n");
        (*seq)++;
      }
    }
    break;
  case LOCALSEQ_END:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �ڑ����N�G�X�g�F�I��
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
 * �ڑ����N�G�X�g���󂯂��̂Őڑ������݂�F������
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
  UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_001);
  _PlayerMinePause(unisys, fieldWork, TRUE);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ڑ����N�G�X�g���󂯂��̂Őڑ������݂�F�X�V
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
  switch(*seq){
  case 0:
    OS_TPrintf("�e�֐ڑ����ɂ����܂�\n");
    GF_ASSERT(situ->mycomm.answer_pc != NULL);
    OS_TPrintf("ChangeOver ���[�h�ؑցF�q�Œ�\n");
    GFL_NET_ChangeoverModeSet(
      GFL_NET_CHANGEOVER_MODE_FIX_CHILD, TRUE, situ->mycomm.answer_pc->mac_address);
    situ->wait = 0;
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    
    if(GFL_NET_GetConnectNum() > 1){
      OS_TPrintf("�ڑ����܂����I�F�q\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_CHILD);
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, situ->mycomm.answer_pc);
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, NULL);
      (*seq)++;
    }
    else{
      situ->wait++;
      if(situ->wait > ONESELF_SERVER_TIMEOUT || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)){
        GFL_NET_ChangeoverModeSet(GFL_NET_CHANGEOVER_MODE_NORMAL, FALSE, NULL);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_ANSWER_PC, NULL);
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_007);
        OS_TPrintf("�e�Ɛڑ��o���Ȃ������׃L�����Z�����܂���\n");
        (*seq)++;
      }
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
 * �ڑ����N�G�X�g���󂯂��̂Őڑ������݂�F�I��
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
 * �ڑ��m����̉�b�i�e�j�F������
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
  UnionMyComm_InitMenuParam(&situ->mycomm);
  UnionMySituation_SetParam(
    unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, (void*)UNION_PLAY_CATEGORY_TALK);
  UnionMyComm_PartyAdd(&situ->mycomm, situ->mycomm.connect_pc);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ڑ��m����̉�b�i�e�j�F�X�V
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
  switch(*seq){
  case 0:
    if(situ->mycomm.connect_pc->beacon.sex == PM_MALE){
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_002);
    }
    else{
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_002);
    }
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case 2:   //���C�����j���[�`��
    UnionMsg_Menu_MainMenuSetup(unisys, fieldWork);
    (*seq)++;
    break;
  case 3:
    {
      u32 select_list;
      
      select_list = UnionMsg_Menu_MainMenuSelectLoop(unisys);
      switch(select_list){
      case FLDMENUFUNC_NULL:
        break;
      case FLDMENUFUNC_CANCEL:
      case UNION_MENU_SELECT_CANCEL:
        OS_TPrintf("���j���[���L�����Z�����܂���\n");
        situ->mycomm.mainmenu_select = UNION_MENU_SELECT_CANCEL;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
        UnionMsg_Menu_MainMenuDel(unisys);
        return TRUE;
      case UNION_MENU_NO_SEND_BATTLE:
        OS_TPrintf("�ΐ탁�j���[��I��\n");
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_BATTLE_PARENT);
        UnionMsg_Menu_MainMenuDel(unisys);
        return TRUE;
      default:
        situ->mycomm.mainmenu_select = select_list;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
        UnionMsg_Menu_MainMenuDel(unisys);
        return TRUE;
      }
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �ڑ��m����̉�b�i�e�j�F�I��
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
 * �ڑ��m����A�I���������j���[���ڂ̑��M�Ǝq�̕Ԏ��҂��i�e�j�F�X�V
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
  switch(*seq){
  case 0: //����ɑI���������j���[��ʒm
		if(UnionSend_MainMenuListResult(situ->mycomm.mainmenu_select) == TRUE){
      OS_TPrintf("���X�g���ʑ��M���� : %d\n", situ->mycomm.mainmenu_select);
      if(situ->mycomm.mainmenu_select == UNION_MENU_SELECT_CANCEL){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
        return TRUE;
      }
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_004);
      (*seq)++;
    }
    break;
  case 1: //�Ԏ��҂�
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    if(situ->mycomm.mainmenu_yesno_result == TRUE){
      OS_TPrintf("�u�͂��v��M\n");
      UnionOneself_ReqStatus(unisys, 
        UNION_STATUS_TRAINERCARD + situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD);
      (*seq)++;
    }
    else if(situ->mycomm.mainmenu_yesno_result == FALSE){
      OS_TPrintf("�u�������v��M\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_006);
      (*seq)++;
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
 * �ڑ��m����̉�b�i�q�j�F������
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
  UnionMyComm_InitMenuParam(&situ->mycomm);
  UnionMySituation_SetParam(
    unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, (void*)UNION_PLAY_CATEGORY_TALK);
  UnionMyComm_PartyAdd(&situ->mycomm, situ->mycomm.connect_pc);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �ڑ��m����̉�b�i�q�j�F�X�V
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
  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }

  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_008);
    (*seq)++;
    break;
  case 1:
    if(situ->mycomm.mainmenu_select == UNION_MENU_SELECT_NULL){
      break;
    }
    else if(situ->mycomm.mainmenu_select == UNION_MENU_SELECT_CANCEL){
      OS_TPrintf("�I�����j���[��M�F�L�����Z��\n");
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_003_06);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      return TRUE;
    }
    else if(situ->mycomm.mainmenu_select < UNION_PLAY_CATEGORY_MAX){
      OS_TPrintf("�I�����j���[��M�F%d\n", situ->mycomm.mainmenu_select);
      UnionMsg_TalkStream_PrintPack(
        unisys, fieldWork, 
        msg_union_test_003 + situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD);
      (*seq)++;
    }
    else{
      if(UnionSend_MainMenuListResultAnswer(FALSE) == TRUE){
        OS_TPrintf("���m�̑I�����j���[��M�F%d\n", situ->mycomm.mainmenu_select);
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_009);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
        return TRUE;
      }
    }
    break;
  case 2:   //�u�͂��E�������v�I��
    UnionMsg_YesNo_Setup(unisys, fieldWork);
    (*seq)++;
    break;
  case 3:
    {
      BOOL result;
      if(UnionMsg_YesNo_SelectLoop(unisys, &result) == TRUE){
        UnionMsg_YesNo_Del(unisys);
        //�퓬�̎��̓��M�����[�V���������ĎQ���\���`�F�b�N
        if(Union_CheckEntryBattleRegulation(situ->mycomm.mainmenu_select) == FALSE){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_010);
          result = FALSE; //�����Łu�������v��Ԃ�
        }
        situ->mycomm.mainmenu_yesno_result = result;
        (*seq)++;
      }
    }
    break;
  case 4: //�u�͂��E�������v�I�����ʑ��M
    if(UnionSend_MainMenuListResultAnswer(situ->mycomm.mainmenu_yesno_result) == TRUE){
      if(situ->mycomm.mainmenu_yesno_result == FALSE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_006_01);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      }
      else{
        UnionOneself_ReqStatus(unisys, 
          UNION_STATUS_TRAINERCARD + situ->mycomm.mainmenu_select-UNION_PLAY_CATEGORY_TRAINERCARD);
      }
      return TRUE;
    }
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �ڑ��m����̉�b�i�q�j�F�I��
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
 * �ΐ�̉�b�i�e�j�F�X�V
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
  BOOL next_menu;
  u32 select_ret;
  enum{
    LOCALSEQ_INIT,
    LOCALSEQ_MENU_SETUP,
    LOCALSEQ_MENU_LOOP,
  };
  
  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }

  switch(*seq){
  case LOCALSEQ_INIT:
    situ->work = 0;
    (*seq)++;
    break;
  case LOCALSEQ_MENU_SETUP:
    OS_TPrintf("�ΐ탁�j���[�Z�b�g�A�b�v�F%d\n", situ->work);
    UnionMsg_Menu_BattleMenuSetup(unisys, fieldWork, situ->work, &situ->menu_reg);  //1vs1, 2vs2
    (*seq)++;
    break;
  case LOCALSEQ_MENU_LOOP:
    select_ret = UnionMsg_Menu_BattleMenuSelectLoop(unisys, &next_menu, &situ->menu_reg);
    if(next_menu == TRUE){
      situ->work = select_ret;
      UnionMsg_Menu_BattleMenuDel(unisys);
      (*seq)--;
    }
    else if(select_ret == FLDMENUFUNC_CANCEL || select_ret == UNION_MENU_SELECT_CANCEL){
      UnionMsg_Menu_MainMenuDel(unisys);
      if(situ->work > 0){
        situ->work--;
        (*seq)--;
      }
      else{
        OS_TPrintf("���j���[���L�����Z�����܂���\n");
        situ->mycomm.mainmenu_select = UNION_MENU_SELECT_CANCEL;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
        return TRUE;
      }
    }
    else if(select_ret != FLDMENUFUNC_NULL){
      UnionMsg_Menu_BattleMenuDel(unisys);
      if(Union_CheckEntryBattleRegulation(select_ret) == FALSE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_010);
        (*seq) = LOCALSEQ_INIT;
        break;
      }
      situ->mycomm.mainmenu_select = select_ret;
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���ɗV��ł���e�ɘb���������F�X�V
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
    
    LOCALSEQ_YESNO_SETUP,
    LOCALSEQ_YESNO,
  };
  
  buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(situ->mycomm.talk_obj_id);

  switch(*seq){
  case LOCALSEQ_INIT:
    play_category = unisys->receive_beacon[buf_no].beacon.play_category;
    switch(play_category){
    case UNION_PLAY_CATEGORY_TALK:           //��b��
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_013);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_END;
      break;
    case UNION_PLAY_CATEGORY_TRAINERCARD:    //�g���[�i�[�J�[�h
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_014);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_END;
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_015);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_END;
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:      //�R���V�A��
      if(unisys->receive_beacon[buf_no].beacon.connect_num < UnionMsg_GetMemberMax(play_category)){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_016);
        (*seq) = LOCALSEQ_YESNO_SETUP;
      }
      else{ //�l���������ς�
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_016_01);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        (*seq) = LOCALSEQ_END;
      }
      break;
    default:
      OS_TPrintf("���m�̗V�� play_category = %d\n", situ->play_category);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      (*seq) = LOCALSEQ_END;
      break;
    }
    break;
  case LOCALSEQ_END:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;

  case LOCALSEQ_YESNO_SETUP:   //�u�͂��E�������v�I��
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionMsg_YesNo_Setup(unisys, fieldWork);
      (*seq)++;
    }
    break;
  case LOCALSEQ_YESNO:
    {
      BOOL result;
      if(UnionMsg_YesNo_SelectLoop(unisys, &result) == TRUE){
        UnionMsg_YesNo_Del(unisys);
        if(result == FALSE){
          UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_018);
          UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
          (*seq) = LOCALSEQ_END;
        }
        else{
          //���M�����[�V���������ĎQ���\���`�F�b�N
          if(Union_CheckEntryBattleRegulation(situ->mycomm.mainmenu_select) == FALSE){
            UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_010);
            UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
            (*seq) = LOCALSEQ_END;
          }
          else{
            situ->mycomm.mainmenu_yesno_result = result;
            UnionOneself_ReqStatus(unisys, UNION_STATUS_INTRUDE);
            (*seq) = LOCALSEQ_END;
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
 * ���ɗV��ł���q�ɘb���������F�X�V
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
  
  switch(*seq){
  case 0:
    buf_no = UNION_CHARA_GetCharaIndex_to_ParentNo(situ->mycomm.talk_obj_id);
    play_category = unisys->receive_beacon[buf_no].beacon.play_category;
    switch(play_category){
    case UNION_PLAY_CATEGORY_TALK:           //��b��
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_013);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      break;
    case UNION_PLAY_CATEGORY_TRAINERCARD:    //�g���[�i�[�J�[�h
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_014);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD_SHOOTER:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:      //�R���V�A��
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD_SHOOTER:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD:
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_015);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:      //�R���V�A��
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_016_01);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      break;
    default:
      OS_TPrintf("���m�̗V�� play_category = %d\n", situ->play_category);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
      break;
    }
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�J�[�h�Ăяo���F�X�V
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
  switch(*seq){
  case 0:   //�g���[�i�[�J�[�h�̏��𑗂肠��
    //�������̑O�ɑ��M�f�[�^�Ǝ�M�o�b�t�@���쐬
    GF_ASSERT(situ->mycomm.trcard.my_card == NULL && situ->mycomm.trcard.target_card == NULL);
    situ->mycomm.trcard.my_card 
      = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(TR_CARD_DATA));
    situ->mycomm.trcard.target_card 
      = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(TR_CARD_DATA));
    TRAINERCARD_GetSelfData(situ->mycomm.trcard.my_card, unisys->uniparent->game_data, TRUE);
    GFL_STD_MemCopy(  //�ꉞ�����̎��̂Ŏ󂯎��Ȃ��������̃P�A�ׁ̈A�����̂��R�s�[���Ă���
      situ->mycomm.trcard.my_card, situ->mycomm.trcard.target_card, sizeof(TR_CARD_DATA));
    
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PARAM);
    OS_TPrintf("�g���[�i�[�J�[�h�O�̓������J�n\n");

    //�u�͂��A�ǂ����I�v
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_005);

    (*seq)++;
    break;
  case 1:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PARAM) == TRUE){
      OS_TPrintf("�g���[�i�[�J�[�h�O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 2:
    if(UnionSend_TrainerCardParam(unisys) == TRUE){
      OS_TPrintf("�g���[�i�[�J�[�h��񑗐M\n");
      (*seq)++;
    }
    break;
  case 3:
    if(situ->mycomm.trcard.target_card_receive == TRUE
        && UnionMsg_TalkStream_Check(unisys) == TRUE){
      OS_TPrintf("����̃J�[�h��M\n");
      situ->mycomm.trcard.card_param = TRAINERCASR_CreateCallParam_CommData(
        unisys->uniparent->game_data, situ->mycomm.trcard.target_card, HEAPID_UNION);
      (*seq)++;
    }
    break;
  case 4:   //��ʐ؂�ւ��O�̓������
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_BEFORE);
    OS_TPrintf("�g���[�i�[�J�[�h��ʐ؂�ւ��O�̓������J�n\n");
    (*seq)++;
    break;
  case 5:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_BEFORE) == TRUE){
      OS_TPrintf("�g���[�i�[�J�[�h��ʐ؂�ւ��O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 6:
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_TRAINERCARD, situ->mycomm.trcard.card_param);
    (*seq)++;
    break;
  case 7:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("�T�uPROC�I��\n");
      (*seq)++;
    }
    break;
  case 8:   //�g���[�i�[�J�[�h��ʏI����̓������
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_008);
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_AFTER);
    OS_TPrintf("�g���[�i�[�J�[�h�I����̓������J�n\n");
    (*seq)++;
    break;
  case 9:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRAINERCARD_PROC_AFTER) == TRUE){
      OS_TPrintf("�g���[�i�[�J�[�h�I����̓�����萬��\n");
    
    #if 0//�g���[�i�[�J�[�h��ParentWork�̓g���[�i�[�J�[�h��Proc���ŉ�������̂ł����ł͉�����Ȃ�
      GFL_HEAP_FreeMemory(situ->mycomm.trcard.card_param);
    #endif
      GFL_HEAP_FreeMemory(situ->mycomm.trcard.my_card);
      GFL_HEAP_FreeMemory(situ->mycomm.trcard.target_card);
      situ->mycomm.trcard.card_param = NULL;
      situ->mycomm.trcard.my_card = NULL;
      situ->mycomm.trcard.target_card = NULL;
      
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
 * �g���[�i�[�J�[�h�Ăяo���F�X�V
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
  switch(*seq){
  case 0:   //�g���[�i�[�J�[�h�̏��𑗂肠��
    //�u�͂��A�ǂ����I�v
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_005);

    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      GFL_NET_HANDLE_TimingSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_BEFORE);
      OS_TPrintf("�|�P���������O�̓������J�n\n");
      (*seq)++;
    }
    break;
  case 2:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_BEFORE) == TRUE){
      OS_TPrintf("�|�P���������O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 3:
    {
      struct _EVENT_IRCBATTLE_WORK{
        GAMESYS_WORK * gsys;
        FIELDMAP_WORK * fieldmap;
        SAVE_CONTROL_WORK *ctrl;
        BATTLE_SETUP_PARAM para;
        BOOL isEndProc;
        int selectType;
      	IRC_COMPATIBLE_PARAM	compatible_param;	//�ԊO�����j���[�ɓn�����
      } * eibw;

      eibw = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(struct _EVENT_IRCBATTLE_WORK));
      
      eibw->gsys = unisys->uniparent->gsys;
      eibw->fieldmap = fieldWork;
      eibw->ctrl = GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data);
      eibw->selectType = EVENTIRCBTL_ENTRYMODE_TRADE;
      
      unisys->parent_work = eibw;
      UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_TRADE, eibw);
      (*seq)++;
    }
    break;
  case 4:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      GFL_HEAP_FreeMemory(unisys->parent_work);
      unisys->parent_work = NULL;
      OS_TPrintf("�T�uPROC�I��\n");
      (*seq)++;
    }
    break;
  case 5:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_008);
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_AFTER);
    OS_TPrintf("�|�P����������ʏI����̓������J�n\n");
    (*seq)++;
    break;
  case 6:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_TRADE_PROC_AFTER) == TRUE){
      OS_TPrintf("�|�P����������ʏI����̓�����萬��\n");
    
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
 * ���������F�X�V
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

  switch(*seq){
  case 0:
    {
      OS_TPrintf("�����J�n���܂�\n");
      OS_TPrintf("ChangeOver ���[�h�ؑցF�q�Œ�\n");
      GFL_NET_ChangeoverModeSet(
        GFL_NET_CHANGEOVER_MODE_FIX_CHILD, TRUE, unisys->receive_beacon[buf_no].mac_address);
    }
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_012);
    situ->wait = 0;
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      break;
    }
    
    if(GFL_NET_GetConnectNum() > 1){
      OS_TPrintf("�ڑ����܂����I�F�q\n");
      UnionMySituation_SetParam(
        unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, &unisys->receive_beacon[buf_no]);
      situ->mycomm.intrude = TRUE;
      situ->mycomm.mainmenu_select = unisys->receive_beacon[buf_no].beacon.play_category;
      switch(situ->mycomm.mainmenu_select){
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:
        UnionOneself_ReqStatus(unisys, UNION_STATUS_BATTLE_MULTI);
        break;
      default:
        OS_TPrintf("�ݒ肳��Ă��Ȃ�category = %d\n", situ->mycomm.mainmenu_select);
        GF_ASSERT(0);
        break;
      }
      (*seq)++;
    }
    else{
      situ->wait++;
      if(situ->wait > ONESELF_SERVER_TIMEOUT || (GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)){
        GFL_NET_ChangeoverModeSet(GFL_NET_CHANGEOVER_MODE_NORMAL, FALSE, NULL);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_NORMAL);
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_007);
        OS_TPrintf("�e�Ɛڑ��o���Ȃ������׃L�����Z�����܂���\n");
        (*seq)++;
      }
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
 * �ؒf�����F�X�V
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
  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }

  switch(*seq){
  case 0:
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_SHUTDOWN);
    OS_TPrintf("�ؒf�O�̓������F�J�n\n");
    (*seq)++;
    break;
  case 1:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_SHUTDOWN) == TRUE){
      OS_TPrintf("�ؒf�O�̓������F����\n");
      (*seq)++;
    }
    break;
  case 2:
    UnionComm_Req_ShutdownRestarts(unisys);
    (*seq)++;
    break;
  case 3:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      UnionMyComm_PartyDel(&situ->mycomm, situ->mycomm.connect_pc);
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, NULL);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���J�ځF�X�V
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
  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_talkboy_03_02);
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case 2:
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE);
    OS_TPrintf("�R���V�A���J�ڑO�̓������J�n\n");
    (*seq)++;
    break;
  case 3:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE) == TRUE){
      OS_TPrintf("�R���V�A���J�ڑO�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 4:
    {
      int subproc_id;
      subproc_id = UNION_SUBPROC_ID_COLOSSEUM_WARP_START 
        + (situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_COLOSSEUM_START);
      UnionSubProc_EventSet(unisys, subproc_id, NULL);
    }
    (*seq)++;
    break;
  case 5:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("�R���V�A���J�ڂ̃T�uPROC�I��\n");

      //�ʐM�v���C���[����V�X�e���̐���
      GF_ASSERT(unisys->colosseum_sys == NULL);
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
 * �}���`�F�R���V�A���J�ځF�X�V
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
  switch(*seq){
  case 0:
    OS_TPrintf("�}���`�ΐ�\n");
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_talkboy_03_02);
    (*seq)++;
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case 2:
    if(situ->mycomm.intrude == FALSE){  //�����̏ꍇ�͓������Ȃ�
      GFL_NET_HANDLE_TimingSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE);
      OS_TPrintf("�R���V�A���J�ڑO�̓������J�n\n");
    }
    (*seq)++;
    break;
  case 3:
    if(situ->mycomm.intrude == FALSE){  //�����̏ꍇ�͓������Ȃ�
  		if(GFL_NET_HANDLE_IsTimingSync(
  		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_PROC_BEFORE) == TRUE){
        OS_TPrintf("�R���V�A���J�ڑO�̓�����萬��\n");
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 4:
    {
      int subproc_id;
      subproc_id = UNION_SUBPROC_ID_COLOSSEUM_WARP_START 
        + (situ->mycomm.mainmenu_select - UNION_PLAY_CATEGORY_COLOSSEUM_START);
      UnionSubProc_EventSet(unisys, subproc_id, NULL);
    }
    (*seq)++;
    break;
  case 5:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("�R���V�A���J�ڂ̃T�uPROC�I��\n");

      //�ʐM�v���C���[����V�X�e���̐���
      GF_ASSERT(unisys->colosseum_sys == NULL);
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
 * �R���V�A���A�����o�[�W���҂��F�X�V
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
  
  GF_ASSERT(clsys != NULL);
  
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
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_011);
      break;
    }
    
    //�}���`�F��W�������s��
    if(GFL_NET_IsParentMachine() == TRUE){
      clsys->entry_menu = CommEntryMenu_Setup(fieldWork, 
        UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select),
        UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), HEAPID_UNION);
    }
    else{
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_011);
    }
    (*seq)++;
    break;
  case 1:
    if(UnionSend_ColosseumEntryStatus(&clsys->basic_status[my_net_id]) == TRUE){
      if(GFL_NET_IsParentMachine() == TRUE){
        (*seq)++;
      }
      else{
        (*seq) = 100;
      }
    }
    break;
  case 2:
    if(CommEntryMenu_Update(clsys->entry_menu) == TRUE){
      (*seq)++;
    }
    for(i = 0; i < COMM_ENTRY_USER_MAX; i++){
      COMM_ENTRY_ANSWER answer;
      answer = CommEntryMenu_GetAnswer(clsys->entry_menu, i, FALSE);
      if(answer != COMM_ENTRY_ANSWER_NULL){
        if(UnionSend_ColosseumEntryAnswer(i, answer) == TRUE){
          CommEntryMenu_SetSendFlag(clsys->entry_menu, i);
        }
      }
    }
    break;
  case 3:
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
      
      //�S���ɃG���g���[���ʂ𑗐M�ς݂ŁA�ڑ��l���Ǝ󂯓���l������v���Ă���Ύ��ɐi��
      if(count == 0 
          && (GFL_NET_GetConnectNum() == CommEntryMenu_GetCompletionNum(clsys->entry_menu))){
        if(UnionSend_ColosseumEntryAllReady() == TRUE){
          CommEntryMenu_Exit(clsys->entry_menu);
          clsys->entry_menu = NULL;
          (*seq) = 100;
        }
      }
    }
    break;
    
  case 100:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      if(clsys->mine.entry_answer == COMM_ENTRY_ANSWER_COMPLETION){
        (*seq)++;
      }
      else if(clsys->mine.entry_answer == COMM_ENTRY_ANSWER_REFUSE){
        UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_019);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
        _PlayerMinePause(unisys, fieldWork, TRUE);
        return TRUE;
      }
    }
    break;
  case 101:
    if(clsys->entry_all_ready == TRUE){
      OS_TPrintf("�S���W�܂���\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_FIRST_DATA_SHARING);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���A�ŏ��̃f�[�^���L�F�X�V
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

  GF_ASSERT(clsys != NULL);
  
  switch(*seq){
  case 0:
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER);
    OS_TPrintf("�ŏ��̃f�[�^���L�O�̓������J�n\n");
    (*seq)++;
    break;
  case 1:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER) == TRUE){
      OS_TPrintf("�ŏ��̃f�[�^���L�O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 2:
    OS_TPrintf("�R���V�A���̒ʐM�e�[�u��Add\n");
    Colosseum_AddCommandTable(unisys);
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER);
    OS_TPrintf("�ʐM�e�[�u����Add�����̂œ������J�n\n");
    (*seq)++;
    break;
  case 3:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER) == TRUE){
      OS_TPrintf("�ʐM�e�[�u����Add�����̂œ�����萬��\n");
      (*seq)++;
    }
    break;
  case 4:
    OS_TPrintf("�S�����̊�{���������������܂�\n");
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
            clsys->basic_status[i].trainer_view, clsys->basic_status[i].sex);
        }
      }
      OS_TPrintf("�S�����̊�{������M����\n");
      (*seq)++;
    }
    break;
  case 6:
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_CARD_BEFORE);
    OS_TPrintf("�g���[�i�[�J�[�h�����O�̓������J�n\n");
    (*seq)++;
    break;
  case 7:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_CARD_BEFORE) == TRUE){
      OS_TPrintf("�g���[�i�[�J�[�h�����O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 8:
    OS_TPrintf("�S�����̃g���[�i�[�J�[�h�������������܂�\n");
    if(ColosseumSend_TrainerCard(clsys->recvbuf.tr_card[my_net_id]) == TRUE){
      (*seq)++;
    }
    break;
  case 9:
    if(ColosseumTool_AllReceiveCheck_TrainerCard(clsys) == TRUE){
      OS_TPrintf("�S�����̃g���[�i�[�J�[�h����M����\n");
      (*seq)++;
    }
    break;
  case 10:
    OS_TPrintf("�R���V�A���̏���OK! �������J�n�I\n");
    Colosseum_CommReadySet(clsys, TRUE);
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_NORMAL);
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���A�t���[�ړ��F�X�V
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
 * �R���V�A���A�t���[�ړ��F�X�V
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
  
  switch(*seq){
  case 0:
    {
      int stand_pos;
   
      //�����ʒu�`�F�b�N
      if(ColosseumTool_CheckStandingPosition(fieldWork, 
          UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select), &stand_pos) == TRUE){
        Colosseum_Mine_SetStandingPostion(clsys, stand_pos);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDPOSITION);
        _PlayerMinePause(unisys, fieldWork, TRUE);
        return TRUE;
      }
      
      //�o�� or �N���̑ޏo��M�`�F�b�N
      if(ColosseumTool_ReceiveCheck_Leave(clsys) == TRUE 
          || ColosseumTool_CheckWayOut(fieldWork) == TRUE){
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_LEAVE);
        _PlayerMinePause(unisys, fieldWork, TRUE);
        return TRUE;
      }
      
      //�b�������`�F�b�N
      if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
        FIELD_PLAYER_GetFrontGridPos(player, &check_gx, &check_gy, &check_gz);
        if(CommPlayer_SearchGridPos(clsys->cps, check_gx, check_gz, &out_index) == TRUE){
          clsys->talk_obj_id = out_index;
          OS_TPrintf("�^�[�Q�b�g����! net_id = %d, gx=%d, gz=%d\n", 
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
 * �R���V�A���A�����ʒu�ɂ������F�X�V
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
  
  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_012);
    (*seq)++;
    break;
  case 1:
    if(ColosseumSend_StandingPositionConfirm(clsys) == TRUE){
      OS_TPrintf("�e�ɗ����ʒu�̋��v���𑗐M\n");
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
        OS_TPrintf("�����ʒuOK\n");
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_USE_PARTY_SELECT);
        return TRUE;
      }
      else if(ret == FALSE){
        OS_TPrintf("�����ʒuNG\n");
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
 * �R���V�A���A�����ʒu�����ނ���F�X�V
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
  
  switch(*seq){
  case 0:
    Colosseum_Mine_SetStandingPostion(clsys, COLOSSEUM_STANDING_POSITION_NULL);
    (*seq)++;
    break;
  case 1:
    if(ColosseumSend_StandingPositionConfirm(clsys) == TRUE){
      OS_TPrintf("�e�ɗ����ʒu����ނ��܂��B�𑗐M\n");
      (*seq)++;
    }
    break;
  case 2:
    if(Colosseum_Mine_GetAnswerStandingPosition(clsys) != COLOSSEUM_STANDING_POSITION_NULL){
      OS_TPrintf("��M�F�ǂ���OK\n");
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
      OS_TPrintf("MMDL_CheckPossibleAcmd�҂�\n");
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
 * �R���V�A���A�莝���A�o�g���{�b�N�X�I���F�X�V
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

  if(UnionMsg_TalkStream_Check(unisys) == FALSE){
    return FALSE;
  }
  
  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_battle_01_22);
    (*seq)++;
    break;
  case 1:   //���C�����j���[�`��
    UnionMsg_Menu_PokePartySelectMenuSetup(unisys, fieldWork);
    (*seq)++;
    break;
  case 2:
    {
      u32 select_list;
      
      select_list = UnionMsg_Menu_PokePartySelectMenuSelectLoop(unisys);
      switch(select_list){
      case FLDMENUFUNC_NULL:
        break;
      case FLDMENUFUNC_CANCEL:
        OS_TPrintf("���j���[���L�����Z�����܂���\n");
        UnionMsg_Menu_PokePartySelectMenuDel(unisys);
        UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
        return TRUE;
      case COLOSSEUM_SELECT_PARTY_TEMOTI:
      case COLOSSEUM_SELECT_PARTY_BOX_A:
      case COLOSSEUM_SELECT_PARTY_BOX_B:
        UnionMsg_Menu_PokePartySelectMenuDel(unisys);
        clsys->select_pokeparty = select_list;
        (*seq)++;
        break;
      }
    }
    break;
  case 3:
    if(1){  //�Q�������𖞂����Ă��邩�`�F�b�N  ��check�@��ō쐬
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_POKELIST_READY);
      return TRUE;
    }
    else{ //�Q�������𖞂����Ă��Ȃ�
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_battle_01_26);
      *seq = 0;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���A�|�P�������X�g�Ăяo���O�̑S���҂��F�X�V
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
      if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_B){
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
 * �R���V�A���A�|�P�������X�g�Ăяo���O�̑S���f�[�^����M�F�X�V
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
  
  switch(*seq){
  case 0:
    //�����̎�M�o�b�t�@�ɑ��M�f�[�^�ƂȂ�POKEPARTY���Z�b�g
    switch(clsys->select_pokeparty){
    default:
      GF_ASSERT(0);
    case COLOSSEUM_SELECT_PARTY_TEMOTI:
      PokeParty_Copy(GAMEDATA_GetMyPokemon(unisys->uniparent->game_data), 
        clsys->recvbuf.pokeparty[my_net_id]);
      break;
    case COLOSSEUM_SELECT_PARTY_BOX_A:
      //��check�@�Ƃ肠�����莝��
      PokeParty_Copy(GAMEDATA_GetMyPokemon(unisys->uniparent->game_data), 
        clsys->recvbuf.pokeparty[my_net_id]);
      break;
    case COLOSSEUM_SELECT_PARTY_BOX_B:
      //��check�@�Ƃ肠�����莝��
      PokeParty_Copy(GAMEDATA_GetMyPokemon(unisys->uniparent->game_data), 
        clsys->recvbuf.pokeparty[my_net_id]);
      break;
    }
    (*seq)++;
    break;
  case 1:
    ColosseumTool_Clear_ReceivePokeParty(clsys, TRUE);
    ColosseumTool_Clear_ReceiveStandingPos(clsys);
    ColosseumTool_Clear_ReceivePokeListSelected(clsys);

    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_LIST_POKEPARTY_BEFORE);
    OS_TPrintf("���X�g�p��POKEPARTY����M�O�̓������J�n\n");
    (*seq)++;
    break;
  case 2:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_LIST_POKEPARTY_BEFORE) == TRUE){
      OS_TPrintf("���X�g�p��POKEPARTY����M�O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 3:
    if(ColosseumSend_Pokeparty(clsys->recvbuf.pokeparty[my_net_id]) == TRUE){
      OS_TPrintf("���X�g�pPOKEPARTY���M\n");
      (*seq)++;
    }
    break;
  case 4:
    if(ColosseumTool_AllReceiveCheck_Pokeparty(clsys) == TRUE){
      OS_TPrintf("�S�����̃��X�g�pPOKEPARTY��M\n");
      (*seq)++;
    }
    break;
  case 5:
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_STANDINGPOS_BEFORE);
    OS_TPrintf("�����ʒu���M�O�̓����J�n\n");
    (*seq)++;
    break;
  case 6:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_STANDINGPOS_BEFORE) == TRUE){
      OS_TPrintf("�����ʒu���M�O�̓�������\n");
      (*seq)++;
    }
    break;
  case 7:
    if(GFL_NET_IsParentMachine() == TRUE){
      if(ColosseumSend_StandingPos(clsys->parentsys.stand_position) == TRUE){
        OS_TPrintf("�݂�Ȃ̗����ʒu��e�@�����M\n");
        (*seq)++;
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 8:
    if(ColosseumTool_ReceiveCheck_StandingPos(clsys) == TRUE){
      OS_TPrintf("�����ʒu��M\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_POKELIST);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���A�|�P�������X�g�Ăяo���F�X�V
 *
 * @param   unisys		
 * @param   situ		
 * @param   fieldWork		
 * @param   seq		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumPokelist(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  PLIST_DATA *plist;
  UNION_SUBPROC_PARENT_POKELIST *parent_list;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  
  switch(*seq){
  case 0: //�u�Q������|�P������I��ł��������v
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      u32 msg_id;
      switch(situ->mycomm.mainmenu_select){
      default:
        GF_ASSERT(0);
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:
        msg_id = msg_union_battle_01_28;
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:
        msg_id = msg_union_battle_01_29;
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:
        msg_id = msg_union_battle_01_30;
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD_SHOOTER:
      case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD:
        msg_id = msg_union_battle_01_31;
        break;
      case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:
        msg_id = msg_union_battle_01_29;
        break;
      }
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_id);
      (*seq)++;
    }
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionMsg_AllDel(unisys);
      (*seq)++;
    }
    break;
  
  case 2: //�|�P�������X�g��ʌĂяo��
    parent_list = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(UNION_SUBPROC_PARENT_POKELIST));
    
    plist = &parent_list->plist;
    plist->pp = clsys->recvbuf.pokeparty[my_net_id];
    plist->myitem = GAMEDATA_GetMyItem(unisys->uniparent->game_data);
    plist->cfg = SaveData_GetConfig(GAMEDATA_GetSaveControlWork(unisys->uniparent->game_data));
    plist->mode = PL_MODE_BATTLE;
    if(situ->mycomm.mainmenu_select >= UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER 
        && situ->mycomm.mainmenu_select <= UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD){
      plist->in_min = 2;
    }
    else{
      plist->in_min = 1;
    }
    if(situ->mycomm.mainmenu_select == UNION_PLAY_CATEGORY_COLOSSEUM_MULTI){
      plist->in_max = 3;
    }
    else{
      plist->in_max = 6;
    }
    plist->in_lv = 100;
    OS_TPrintf("in_min = %d, in_max = %d\n", plist->in_min, plist->in_max);
    
    {//�ʐM����̏����Z�b�g
      int net_id, enemy_player = 0;
      PL_COMM_PLAYER_TYPE comm_player;
      
      for(net_id = 0; net_id < GFL_NET_GetConnectNum(); net_id++){
        if(net_id != my_net_id){
          if(my_net_id == (clsys->recvbuf.stand_position[net_id] & 1)){
            comm_player = PL_COMM_PLAYER_TYPE_ALLY;
          }
          else{
            comm_player = PL_COMM_PLAYER_TYPE_ENEMY_A + enemy_player;
            enemy_player++;
          }
          plist->comm_battle[comm_player].pp = clsys->recvbuf.pokeparty[net_id];
          plist->comm_battle[comm_player].name = clsys->basic_status[net_id].name;
          plist->comm_battle[comm_player].sex = clsys->basic_status[net_id].sex;
        }
      }
    }
    
    unisys->parent_work = parent_list;
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_POKELIST, parent_list);

    (*seq)++;
    break;
  case 3:
    if(UnionSubProc_IsExits(unisys) == TRUE){
      break;
    }
    
    parent_list = unisys->parent_work;
    plist = &parent_list->plist;

    GF_ASSERT_MSG(plist->ret_mode == PL_RET_NORMAL, "plist->ret_mode �s�� %d\n", plist->ret_mode);
    switch(plist->ret_sel){
    case PL_SEL_POS_EXIT:
      GF_ASSERT(0); //�L�����Z���ŏI���͑I�����Ƃ��Ė����Ȃ��� 2009.09.16(��)
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);
      (*seq)++;
      break;
    case PL_SEL_POS_ENTER:
      {//�����̎�M�o�b�t�@�Ƀ��X�g�őI�񂾏��Ƀ|�P�����f�[�^���Z�b�g
        int entry_no, temoti_no;
        POKEPARTY *temoti_party;
        POKEPARTY *dest_party = clsys->recvbuf.pokeparty[my_net_id];
        
        temoti_party = PokeParty_AllocPartyWork(HEAPID_UNION);
        PokeParty_Copy(dest_party, temoti_party);
        PokeParty_InitWork(dest_party);
        
        for(entry_no = 0; entry_no < TEMOTI_POKEMAX; entry_no++){
          if(plist->in_num[entry_no] == 0){
            break;
          }
          PokeParty_Add(dest_party, 
            PokeParty_GetMemberPointer(temoti_party, plist->in_num[entry_no] - 1));
          OS_TPrintf("�|�P�����莝���o�^ entry_no=%d, in_num=%d\n", entry_no, plist->in_num[entry_no]);
        }
        
        GFL_HEAP_FreeMemory(temoti_party);
      }
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_BATTLE_READY_WAIT);
      (*seq)++;
      break;
    default:
      OS_TPrintf("plist->ret_sel �s�� %d\n", plist->ret_sel);
      GF_ASSERT(0);
      break;
    }
    break;
  case 4:
    GFL_HEAP_FreeMemory(unisys->parent_work);
    unisys->parent_work = NULL;
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���A�S�����퓬�̏�������������̂�҂F�X�V
 *
 * @param   unisys		
 * @param   situ		
 * @param   fieldWork		
 * @param   seq		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumAllBattleReadyWait(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  
  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_battle_01_32);
    (*seq)++;
    break;
  case 1:
    if(ColosseumSend_PokeListSelected() == TRUE){
      (*seq)++;
    }
    break;
  case 2:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_BATTLE);
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���A�퓬��ʌĂяo���F�X�V
 *
 * @param   unisys		
 * @param   situ		
 * @param   fieldWork		
 * @param   seq		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
static BOOL OneselfSeq_ColosseumBattle(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELDMAP_WORK *fieldWork, u8 *seq)
{
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  COLOSSEUM_BATTLE_SETUP *battle_setup;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  int member_num, i;
  
  switch(*seq){
  case 0:
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      ColosseumTool_Clear_ReceivePokeParty(clsys, TRUE);

      GFL_NET_HANDLE_TimingSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_POKEPARTY_BEFORE);
      OS_TPrintf("�o�g���p��POKEPARTY����M�O�̓������J�n\n");
      (*seq)++;
    }
    break;
  case 1:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_POKEPARTY_BEFORE) == TRUE){
      OS_TPrintf("�o�g���p��POKEPARTY����M�O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case 2:
    if(ColosseumSend_Pokeparty(clsys->recvbuf.pokeparty[my_net_id]) == TRUE){
      OS_TPrintf("POKEPARTY���M\n");
      (*seq)++;
    }
    break;
  case 3:
    if(ColosseumTool_AllReceiveCheck_Pokeparty(clsys) == TRUE){
      OS_TPrintf("�S������POKEPARTY��M\n");
      (*seq)++;
    }
    break;
  case 4:
    battle_setup = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(COLOSSEUM_BATTLE_SETUP));
    
    battle_setup->partyPlayer = clsys->recvbuf.pokeparty[my_net_id];
    
    unisys->parent_work = battle_setup;
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_BATTLE, battle_setup);

    (*seq)++;
    break;
  case 5:
    if(UnionSubProc_IsExits(unisys) == TRUE){
      break;
    }
    
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_STANDING_BACK);

    GFL_HEAP_FreeMemory(unisys->parent_work);
    unisys->parent_work = NULL;
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���ޏo�����F�X�V
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
    LEAVE_SEQ_SHUTDOWN,
    LEAVE_SEQ_SHUTDOWN_WAIT,
    LEAVE_SEQ_WARP_UNION,
    LEAVE_SEQ_WARP_UNION_WAIT,
    LEAVE_SEQ_BEACON_RESTART,
  };
  
  switch(*seq){
  case LEAVE_SEQ_INIT:
    _PlayerMinePause(unisys, fieldWork, TRUE);
    if(clsys->mine.entry_answer == COMM_ENTRY_ANSWER_REFUSE){
      if(UnionMsg_TalkStream_Check(unisys) == TRUE){
        *seq = LEAVE_SEQ_SHUTDOWN;
      }
    }
    else{
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_019);
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_SEND_LEAVE:
    if(ColosseumSend_Leave() == TRUE){
      OS_TPrintf("�u�ޏo���܂��v���M����\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_WAIT_ALL_LEAVE:
    if(ColosseumTool_AllReceiveCheck_Leave(clsys) == TRUE){
      OS_TPrintf("�S�����́u�ޏo���܂��v����M����\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_MSG_WAIT:
    //������͑S�������ɐؒf���o����悤�ɁA��Ƀ��b�Z�[�W�̏I����҂��Ă��瓯�����n�߂�
    if(UnionMsg_TalkStream_Check(unisys) == TRUE){
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_TIMING_START:
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_LEAVE);
    OS_TPrintf("�R���V�A���ؒf�O�̓������F�J�n\n");
    (*seq)++;
    break;
  case LEAVE_SEQ_TIMING_WAIT:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_LEAVE) == TRUE){
      OS_TPrintf("�R���V�A���ؒf�O�̓������F����\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_SHUTDOWN:
    UnionComm_Req_Shutdown(unisys);
    (*seq)++;
    break;
  case LEAVE_SEQ_SHUTDOWN_WAIT:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      OS_TPrintf("�R���V�A���ؒf����\n");
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_WARP_UNION:
    clsys->colosseum_leave = TRUE;
    UnionBeacon_ClearAllReceiveData(unisys);
    UnionMyComm_Init(unisys, &situ->mycomm);
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_UNION_WARP, NULL);
    (*seq)++;
    break;
  case LEAVE_SEQ_WARP_UNION_WAIT:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("���j�I���J�ڂ̃T�uPROC�I��\n");
      
      UnionComm_Req_Restarts(unisys);
      (*seq)++;
    }
    break;
  case LEAVE_SEQ_BEACON_RESTART:
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      OS_TPrintf("�r�[�R���X�L�����ĊJ����\n");
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �R���V�A���ł̃g���[�i�[�J�[�h�Ăяo���F�X�V
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

  switch(*seq){
  case 0:
    _PlayerMinePause(unisys, fieldWork, TRUE);

    unisys->parent_work = TRAINERCASR_CreateCallParam_CommData(
      unisys->uniparent->game_data, clsys->recvbuf.tr_card[clsys->talk_obj_id], HEAPID_UNION);

    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_COLOSSEUM_TRAINERCARD, unisys->parent_work);
    (*seq)++;
    break;
  case 1:
    if(UnionSubProc_IsExits(unisys) == FALSE){
    #if 0//�g���[�i�[�J�[�h��ParentWork�̓g���[�i�[�J�[�h��Proc���ŉ�������̂ł����ł͉�����Ȃ�
      GFL_HEAP_FreeMemory(unisys->parent_work);
    #endif
      unisys->parent_work = NULL;
      OS_TPrintf("�T�uPROC�I��\n");
      (*seq)++;
    }
    break;
  case 2:
    UnionOneself_ReqStatus(unisys, UNION_STATUS_COLOSSEUM_NORMAL);
    return TRUE;
  }
  
  return FALSE;
}
