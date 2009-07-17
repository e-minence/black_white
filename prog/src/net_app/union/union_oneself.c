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
#include "msg/msg_union.h"
#include "union_comm_command.h"
#include "system/main.h"
#include "net_app/union/union_event_check.h"
#include "net_app/union/union_subproc.h"
#include "net_app/union/colosseum_comm_command.h"


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
typedef BOOL (*ONESELF_FUNC)(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);

///����Z�b�g
typedef struct{
  ONESELF_FUNC func_init;
  ONESELF_FUNC func_update;
  ONESELF_FUNC func_exit;
}ONESELF_FUNC_DATA;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static BOOL OneselfSeq_NormalInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectReqInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectReqUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectReqExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectAnswerInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectAnswerUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ConnectAnswerExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkInit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkExit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkListSendUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkInit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkUpdate_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TalkExit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_TrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ShutdownUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static void OneselfSeq_ShutdownCallback( void* pWork );
static BOOL OneselfSeq_Talk_Battle_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_BattleUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);
static BOOL OneselfSeq_ColosseumMemberWaitUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq);


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
    NULL,
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
  {//UNION_STATUS_BATTLE
    NULL,
    OneselfSeq_BattleUpdate,
    NULL,
  },
  {//UNION_STATUS_TRADE
    NULL,
    OneselfSeq_ShutdownUpdate,
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
int UnionOneself_Update(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork)
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
  if(menu_index < UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_50 
      || menu_index > UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_STANDARD){
    return TRUE;  //�퓬�ł͂Ȃ��̂�TRUE
  }
  
  switch(menu_index){
  case UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_50:        //�ΐ�:2VS2:�V���O��:LV50
  case UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_FREE:      //�ΐ�:2VS2:�V���O��:�����Ȃ�
  case UNION_MSG_MENU_SELECT_BATTLE_2VS2_SINGLE_STANDARD:  //�ΐ�:2VS2:�V���O��:�X�^���_�[�h
    return TRUE;  //���̓��M�����[�V�����`�F�b�N�Ȃ��B���ڂ��������Ă����OK
  default:
    return FALSE;
  }
}

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
static BOOL OneselfSeq_NormalInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  UnionMyComm_Init(&situ->mycomm);

  UnionMsg_AllDel(unisys);

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
static BOOL OneselfSeq_NormalUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( fieldWork );
  u16 obj_id;
  s16 check_gx, check_gy, check_gz;
  MMDL *target_pc;
  
  //�O�����瓮�샊�N�G�X�g���󂯂Ă���Ȃ�؂�ւ���
  if(situ->next_union_status != UNION_STATUS_NORMAL){
    OS_TPrintf("NormalUpdate:�O�����N�G�X�g��M�B�؂�ւ��܂� next_status=%d\n", situ->next_union_status);
    return TRUE;
  }

  if(GFL_NET_GetConnectNum() > 1){
    GF_ASSERT_MSG(0, "Normal�Ȃ̂ɐڑ��I�I�I�I\n");
    //��check�@�b��Ő擪�̃r�[�R���f�[�^��ڑ�����Ƃ��đ�����Ă���
    //         �{���ł����Normal�Őڑ��͏o���Ȃ��悤�ɂ���
    UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_PARENT);
    UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, &unisys->receive_beacon[0]);
    return TRUE;
  }
  
  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
    FIELD_PLAYER_GetFrontGridPos(player, &check_gx, &check_gy, &check_gz);
    target_pc = MMDLSYS_SearchGridPos(fldMdlSys, check_gx, check_gz, FALSE);
    if(target_pc == NULL){
      return FALSE;
    }
    
    obj_id = MMDL_GetOBJID(target_pc);
    if(obj_id > UNION_RECEIVE_BEACON_MAX){
      GF_ASSERT(0);
      return FALSE;
    }
    UnionMySituation_SetParam(unisys, 
      UNION_MYSITU_PARAM_IDX_CALLING_PC, &unisys->receive_beacon[obj_id]);
    OS_TPrintf("�^�[�Q�b�g����! obj_id = %d, gx=%d, gz=%d\n", obj_id, check_gx, check_gz);
    UnionOneself_ReqStatus(unisys, UNION_STATUS_CONNECT_REQ);
    return TRUE;
  }
  
  return FALSE;
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
static BOOL OneselfSeq_ConnectReqInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
static BOOL OneselfSeq_ConnectReqUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, situ->calling_pc);
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
static BOOL OneselfSeq_ConnectReqExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
static BOOL OneselfSeq_ConnectAnswerInit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_001);
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
static BOOL OneselfSeq_ConnectAnswerUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  switch(*seq){
  case 0:
    OS_TPrintf("�e�֐ڑ����ɂ����܂�\n");
    GF_ASSERT(situ->answer_pc != NULL);
    OS_TPrintf("ChangeOver ���[�h�ؑցF�q�Œ�\n");
    GFL_NET_ChangeoverModeSet(
      GFL_NET_CHANGEOVER_MODE_FIX_CHILD, TRUE, situ->answer_pc->mac_address);
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
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_CONNECT_PC, situ->answer_pc);
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
static BOOL OneselfSeq_ConnectAnswerExit(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
static BOOL OneselfSeq_TalkInit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  UnionMyComm_Init(&unisys->my_situation.mycomm);
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
static BOOL OneselfSeq_TalkUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  switch(*seq){
  case 0:
    if(situ->connect_pc->beacon.sex == PM_MALE){
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
      case UNION_MSG_MENU_SELECT_CANCEL:
        OS_TPrintf("���j���[���L�����Z�����܂���\n");
        situ->mycomm.mainmenu_select = UNION_MSG_MENU_SELECT_CANCEL;
        UnionOneself_ReqStatus(unisys, UNION_STATUS_TALK_LIST_SEND_PARENT);
        UnionMsg_Menu_MainMenuDel(unisys);
        return TRUE;
      case UNION_MSG_MENU_SELECT_NO_SEND_BATTLE:
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
static BOOL OneselfSeq_TalkExit_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
static BOOL OneselfSeq_TalkListSendUpdate_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  switch(*seq){
  case 0: //����ɑI���������j���[��ʒm
		if(UnionSend_MainMenuListResult(situ->mycomm.mainmenu_select) == TRUE){
      OS_TPrintf("���X�g���ʑ��M���� : %d\n", situ->mycomm.mainmenu_select);
      if(situ->mycomm.mainmenu_select == UNION_MSG_MENU_SELECT_CANCEL){
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
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TRAINERCARD + situ->mycomm.mainmenu_select);
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
static BOOL OneselfSeq_TalkInit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  UnionMyComm_Init(&unisys->my_situation.mycomm);
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
static BOOL OneselfSeq_TalkUpdate_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
    if(situ->mycomm.mainmenu_select == UNION_MSG_MENU_SELECT_NULL){
      break;
    }
    else if(situ->mycomm.mainmenu_select == UNION_MSG_MENU_SELECT_CANCEL){
      OS_TPrintf("�I�����j���[��M�F�L�����Z��\n");
      UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_003_06);
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      return TRUE;
    }
    else if(situ->mycomm.mainmenu_select < UNION_MSG_MENU_SELECT_MAX){
      OS_TPrintf("�I�����j���[��M�F%d\n", situ->mycomm.mainmenu_select);
      UnionMsg_TalkStream_PrintPack(
        unisys, fieldWork, msg_union_test_003 + situ->mycomm.mainmenu_select);
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
      UnionOneself_ReqStatus(unisys, UNION_STATUS_TRAINERCARD + situ->mycomm.mainmenu_select);
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
static BOOL OneselfSeq_TalkExit_Child(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
static BOOL OneselfSeq_Talk_Battle_Parent(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
    UnionMsg_Menu_BattleMenuSetup(unisys, fieldWork, situ->work);  //1vs1, 2vs2
    (*seq)++;
    break;
  case LOCALSEQ_MENU_LOOP:
    select_ret = UnionMsg_Menu_BattleMenuSelectLoop(unisys, &next_menu);
    if(next_menu == TRUE){
      situ->work = select_ret;
      UnionMsg_Menu_BattleMenuDel(unisys);
      (*seq)--;
    }
    else if(select_ret == FLDMENUFUNC_CANCEL){
      OS_TPrintf("���j���[���L�����Z�����܂���\n");
      UnionOneself_ReqStatus(unisys, UNION_STATUS_SHUTDOWN);
      return TRUE;
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
static BOOL OneselfSeq_TrainerCardUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
      
      GFL_HEAP_FreeMemory(situ->mycomm.trcard.card_param);
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
static BOOL OneselfSeq_ShutdownUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
    if(GFL_NET_IsParentMachine() == TRUE){
      if(GFL_NET_GetConnectNum() <= 1){ //�e�͎�����l�ɂȂ��Ă���I������
        (*seq)++;
      }
      else{
        OS_TPrintf("�e�F�q�̏I���҂� �c��=%d\n", GFL_NET_GetConnectNum() - 1);
      }
    }
    else{
      (*seq)++;
    }
    break;
  case 3:
    GFL_NET_Exit( OneselfSeq_ShutdownCallback );
    (*seq)++;
    break;
  case 4:
    if(situ->connect_pc == NULL){   //OneselfSeq_ShutdownCallback��NULL�������̂�҂�
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �ؒf�����F�ؒf�����R�[���o�b�N
 *
 * @param   pWork		
 */
//--------------------------------------------------------------
static void OneselfSeq_ShutdownCallback( void* pWork )
{
  UNION_SYSTEM_PTR unisys = pWork;
  
  unisys->my_situation.connect_pc = NULL;
  OS_TPrintf("Shutdown:�ؒf�����R�[���o�b�N\n");
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
static BOOL OneselfSeq_BattleUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
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
    UnionSubProc_EventSet(unisys, UNION_SUBPROC_ID_COLOSSEUM_WARP, situ->mycomm.trcard.card_param);
    (*seq)++;
    break;
  case 5:
    if(UnionSubProc_IsExits(unisys) == FALSE){
      OS_TPrintf("�R���V�A���J�ڂ̃T�uPROC�I��\n");
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
static BOOL OneselfSeq_ColosseumMemberWaitUpdate(UNION_SYSTEM_PTR unisys, UNION_MY_SITUATION *situ, FIELD_MAIN_WORK *fieldWork, u8 *seq)
{
  switch(*seq){
  case 0:
    UnionMsg_TalkStream_PrintPack(unisys, fieldWork, msg_union_test_011);
    break;
  case 1:
    if(UnionMsg_TalkStream_Check(unisys) == FALSE){
      (*seq)++;
    }
    break;
  case 2:
    if(GFL_NET_GetConnectNum() >= UnionMsg_GetMemberMax(situ->mycomm.mainmenu_select)){
      GFL_NET_HANDLE_TimingSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER);
      OS_TPrintf("�l�����������̂œ������J�n\n");
      (*seq)++;
    }
    break;
  case 3:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER) == TRUE){
      OS_TPrintf("�l�����������̂œ�����萬��\n");
      (*seq)++;
    }
    break;
  case 4:
    OS_TPrintf("�R���V�A���̒ʐM�e�[�u��Add\n");
    Colosseum_AddCommandTable(unisys);
    GFL_NET_HANDLE_TimingSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER);
    OS_TPrintf("�ʐM�e�[�u����Add�����̂œ������J�n\n");
    (*seq)++;
    break;
  case 5:
		if(GFL_NET_HANDLE_IsTimingSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER) == TRUE){
      OS_TPrintf("�ʐM�e�[�u����Add�����̂œ�����萬��\n");
      (*seq)++;
    }
    break;
  case 6:
    OS_TPrintf("�S�����̃g���[�i�[�J�[�h���������������܂�\n");
    break;
  }
  
  return FALSE;
}
