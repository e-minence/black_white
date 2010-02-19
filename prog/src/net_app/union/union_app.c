//==============================================================================
/**
 * @file    union_app.c
 * @brief   ���j�I�����[���ł̃~�j�Q�[������n�Funion_app�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2010.01.06(��)
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
#include "union_subproc.h"
#include "union_chat.h"
#include "net_app/union_app.h"
#include "union_app_local.h"



//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _SendUpdate_LeavePlayer(UNION_APP_PTR uniapp);
static void _SendUpdate_BasicStatus(UNION_APP_PTR uniapp);
static void _SendUpdate_Mystatus(UNION_APP_PTR uniapp);
static void _Update_IntrudeReadyCallback(UNION_APP_PTR uniapp);
static void _Update_LeaveCallback(UNION_APP_PTR uniapp);




//==============================================================================
//
//  �V�X�e�����Ŏg�p
//
//==============================================================================
//==================================================================
/**
 * ���j�I���A�v�����䃏�[�N��Alloc���܂�
 *
 * @param   heap_id		
 * @param   member_max		�Q���ő吔
 *
 * @retval  UNION_APP_PTR		
 */
//==================================================================
UNION_APP_PTR UnionAppSystem_AllocAppWork(HEAPID heap_id, u8 member_max, const MYSTATUS *myst)
{
  UNION_APP_PTR uniapp;
  int i;
  
  GF_ASSERT(member_max <= UNION_APP_MEMBER_MAX);
  
  uniapp = GFL_HEAP_AllocClearMemory(heap_id, sizeof(struct _UNION_APP_WORK));
  OS_TPrintf("aaa size uniapp = %d\n", sizeof(struct _UNION_APP_WORK)); //��check
  for(i = 0; i < UNION_APP_MEMBER_MAX; i++){
    uniapp->mystatus[i] = MyStatus_AllocWork(heap_id);
  }
  
  if(GFL_NET_IsParentMachine() == TRUE){
    uniapp->basic_status.member_bit = 3;  //�ŏ��͓�l�ʐM����n�܂� (NetID 0��1)
  }
  uniapp->basic_status.member_max = member_max;
  
  //������MYSTATUS�Z�b�g
  UnionAppSystem_SetMystatus(uniapp, 
    GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()), myst);
  
  Union_App_Parent_EntryBlock(uniapp);
  Union_App_Parent_LeaveBlock(uniapp);
  
  return uniapp;
}

//==================================================================
/**
 * ���j�I���A�v�����䃏�[�N��������܂�
 *
 * @param   uniapp		
 */
//==================================================================
void UnionAppSystem_FreeAppWork(UNION_APP_PTR uniapp)
{
  int i;
  
  for(i = 0; i < UNION_APP_MEMBER_MAX; i++){
    GFL_HEAP_FreeMemory(uniapp->mystatus[i]);
  }
  GFL_HEAP_FreeMemory(uniapp);
}

//==================================================================
/**
 * ������]�ғo�^
 *
 * @param   uniapp		
 * @param   net_id		������]�҂�NetID
 * @param   myst		  ������]�҂�MYSTATUS
 *
 * @retval  BOOL		TRUE:����OK�@FALSE:����NG
 */
//==================================================================
BOOL UnionAppSystem_SetEntryUser(UNION_APP_PTR uniapp, NetID net_id, const MYSTATUS *myst)
{
  if(uniapp->entry_block == FALSE){
    u32 member_num = MATH_CountPopulation((u32)(uniapp->basic_status.member_bit));
    u32 entry_num = MATH_CountPopulation((u32)(uniapp->entry_reserve_bit));
    if(member_num + entry_num < uniapp->basic_status.member_max){
      uniapp->entry_reserve_bit |= 1 << net_id;
      //MyStatus_Copy(myst, uniapp->mystatus[net_id]);
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * ���j�I���A�v������X�V����
 *
 * @param   uniapp		
 * @param   unisys		
 */
//==================================================================
void UnionAppSystem_Update(UNION_APP_PTR uniapp, UNION_SYSTEM_PTR unisys)
{
  if(uniapp->shutdown == TRUE){
    return;
  }
  if(uniapp->shutdown_req == TRUE){
    UnionComm_Req_Shutdown(unisys);
    uniapp->shutdown_req = FALSE;
    uniapp->shutdown_exe = TRUE;
    return;
  }
  if(uniapp->shutdown_exe == TRUE){
    if(UnionComm_Check_ShutdownRestarts(unisys) == FALSE){
      uniapp->shutdown_exe = FALSE;
      uniapp->shutdown = TRUE;
    }
    return;
  }
  
  
  if(GFL_NET_IsParentMachine() == TRUE){
    _SendUpdate_BasicStatus(uniapp);
    _SendUpdate_LeavePlayer(uniapp);
  }
  _SendUpdate_Mystatus(uniapp);
  _Update_IntrudeReadyCallback(uniapp);
  _Update_LeaveCallback(uniapp);
}

//--------------------------------------------------------------
/**
 * ���E�҂�����Η��E�������𑼂̃����o�[�֑��M����
 *
 * @param   uniapp		
 */
//--------------------------------------------------------------
static void _SendUpdate_LeavePlayer(UNION_APP_PTR uniapp)
{
  u32 now_member_bit = uniapp->basic_status.member_bit;
  int net_id;
  
  //�ޏo�҂�NetID�𒲍�
  for(net_id = 0; net_id < UNION_APP_MEMBER_MAX; net_id++){
    if((uniapp->basic_status.member_bit & (1 << net_id))
        && GFL_NET_IsConnectMember(net_id) == FALSE){
      now_member_bit ^= 1 << net_id;
    }
  }

  //�c���Ă��郁���o�[�֌����đޏo�҂�NetID�𑗐M����
  if(now_member_bit != uniapp->basic_status.member_bit){
    for(net_id = 0; net_id < UNION_APP_MEMBER_MAX; net_id++){
      if((uniapp->basic_status.member_bit & (1 << net_id))
          && (now_member_bit & (1 << net_id)) == 0){
        if(UnionSend_MinigameLeaveChild(now_member_bit, net_id) == TRUE){
          uniapp->basic_status.member_bit ^= 1 << net_id;
        }
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ��{���v�����N�G�X�g���������Ă���Α��M
 *
 * @param   uniapp		
 */
//--------------------------------------------------------------
static void _SendUpdate_BasicStatus(UNION_APP_PTR uniapp)
{
  int i;
  
  if(uniapp->recv_basic_status_req_bit > 0){
    if(UnionSend_MinigameBasicStatus(
        &uniapp->basic_status, uniapp->recv_basic_status_req_bit) == TRUE){
      uniapp->recv_basic_status_req_bit = 0;
    }
  }
}

//--------------------------------------------------------------
/**
 * MYSTATUS�v�����N�G�X�g���������Ă���Α��M
 *
 * @param   uniapp		
 */
//--------------------------------------------------------------
static void _SendUpdate_Mystatus(UNION_APP_PTR uniapp)
{
  int i;
  
  if(uniapp->recv_mystatus_req_bit > 0){
    if(UnionSend_MinigameMystatus(uniapp->recv_mystatus_req_bit,
        uniapp->mystatus[GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())]) == TRUE){
      uniapp->recv_mystatus_req_bit = 0;
    }
  }
}

//--------------------------------------------------------------
/**
 * �����R�[���o�b�N
 *
 * @param   uniapp		
 */
//--------------------------------------------------------------
static void _Update_IntrudeReadyCallback(UNION_APP_PTR uniapp)
{
  int net_id;
  
  if(uniapp->recv_intrude_ready_bit > 0 && uniapp->entry_callback != NULL){
    for(net_id = 0; net_id < UNION_APP_MEMBER_MAX; net_id++){
      if(uniapp->recv_intrude_ready_bit & (1 << net_id)){
        if((uniapp->recv_mystatus_bit & (1 << net_id)) 
            && uniapp->entry_reserve_bit & (1 << net_id)){
          OS_TPrintf("����CallBack NetID=%d\n", net_id);
          uniapp->entry_callback(net_id, uniapp->mystatus[net_id], uniapp->userwork);
          //�\���Ԃ��琳�K�����o�[�ւƈڂ�
          uniapp->entry_reserve_bit ^= 1 << net_id;
          uniapp->basic_status.member_bit |= 1 << net_id;
        }
        uniapp->recv_intrude_ready_bit ^= 1 << net_id;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * ���E�R�[���o�b�N
 *
 * @param   uniapp		
 */
//--------------------------------------------------------------
static void _Update_LeaveCallback(UNION_APP_PTR uniapp)
{
  int net_id;
  
  if(uniapp->recv_leave_bit > 0 && uniapp->leave_callback != NULL){
    for(net_id = 0; net_id < UNION_APP_MEMBER_MAX; net_id++){
      if(uniapp->recv_leave_bit & (1 << net_id)){
        if((uniapp->recv_mystatus_bit & (1 << net_id)) 
            && uniapp->basic_status.member_bit & (1 << net_id)){
          OS_TPrintf("���ECallBack NetID=%d\n", net_id);
          uniapp->leave_callback(net_id, uniapp->mystatus[net_id], uniapp->userwork);
          //���K�����o�[��bit�𗎂Ƃ�
          uniapp->basic_status.member_bit ^= 1 << net_id;
          uniapp->recv_mystatus_bit ^= 1 << net_id;
        }
        uniapp->recv_leave_bit ^= 1 << net_id;
      }
    }
  }
}

//==================================================================
/**
 * ��{���v�����N�G�X�g��o�^
 *
 * @param   uniapp		
 * @param   net_id		�v�������NetID
 */
//==================================================================
void UnionAppSystem_ReqBasicStatus(UNION_APP_PTR uniapp, NetID net_id)
{
  uniapp->recv_basic_status_req_bit |= 1 << net_id;
}

//==================================================================
/**
 * ��{�����Z�b�g
 *
 * @param   uniapp		
 * @param   basic_status		
 */
//==================================================================
void UnionAppSystem_SetBasicStatus(UNION_APP_PTR uniapp, const UNION_APP_BASIC *basic_status)
{
  GFL_STD_MemCopy(basic_status, &uniapp->basic_status, sizeof(UNION_APP_BASIC_STATUS));
}

//==================================================================
/**
 * MYSTATUS�v�����N�G�X�g��o�^
 *
 * @param   uniapp		
 * @param   net_id		�v�������NetID
 */
//==================================================================
void UnionAppSystem_ReqMystatus(UNION_APP_PTR uniapp, NetID net_id)
{
  uniapp->recv_mystatus_req_bit |= 1 << net_id;
}

//==================================================================
/**
 * MYSTATUS���Z�b�g
 *
 * @param   uniapp		
 * @param   net_id		
 * @param   myst		
 */
//==================================================================
void UnionAppSystem_SetMystatus(UNION_APP_PTR uniapp, NetID net_id, const MYSTATUS *myst)
{
  MyStatus_Copy(myst, uniapp->mystatus[net_id]);
  uniapp->recv_mystatus_bit |= 1 << net_id;
}

//==================================================================
/**
 * ��{�����󂯎���Ă��邩���ׂ�
 *
 * @param   uniapp		
 *
 * @retval  BOOL		TRUE:�󂯎���Ă���B�@FALSE:�󂯎���Ă��Ȃ�
 */
//==================================================================
BOOL UnionAppSystem_CheckBasicStatus(UNION_APP_PTR uniapp)
{
  if(uniapp->basic_status.member_bit > 0){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �����҂��ł͂Ȃ������ɃQ�[���ɎQ�����Ă��郆�[�U�[�S������MYSTATUS���󂯎���Ă��邩���ׂ�
 *
 * @param   uniapp		
 *
 * @retval  BOOL		TRUE:�S�����󂯎���Ă���
 */
//==================================================================
BOOL UnionAppSystem_CheckMystatus(UNION_APP_PTR uniapp)
{
  int net_id;
  
  for(net_id = 0; net_id < UNION_APP_MEMBER_MAX; net_id++){
    if(uniapp->basic_status.member_bit & (1 << net_id)){
      if(GFL_NET_IsConnectMember(net_id) == TRUE){  //�ʐM���؂�Ă�����͖̂�������
        if((uniapp->recv_mystatus_bit & (1 << net_id)) == 0){
          return FALSE; //���ł�MYSTATUS���͂��Ă��Ȃ����̂�������
        }
      }
    }
  }
  
  return TRUE;
}

//==================================================================
/**
 * �������������錾����M
 *
 * @param   uniapp		
 * @param   net_id		�����҂�NetID
 */
//==================================================================
void UnionAppSystem_SetIntrudeReady(UNION_APP_PTR uniapp, NetID net_id)
{
  if((uniapp->recv_mystatus_bit & (1 << net_id)) && (uniapp->entry_reserve_bit & (1 << net_id))){
    uniapp->recv_intrude_ready_bit |= 1 << net_id;
  }
  else{
    GF_ASSERT_MSG(0, "recv_mystbit=%d, entry_reserve_bit=%d, net_id=%d\n", 
      uniapp->recv_mystatus_bit, uniapp->entry_reserve_bit, net_id);
  }
}

//==================================================================
/**
 * �q�@�����E���܂�������M
 *
 * @param   uniapp		
 * @param   net_id		���E�����q�@��NetID
 */
//==================================================================
void UnionAppSystem_SetLeaveChild(UNION_APP_PTR uniapp, NetID net_id)
{
  if((uniapp->recv_mystatus_bit & (1 << net_id)) && (uniapp->basic_status.member_bit & (1 << net_id))){
    uniapp->recv_leave_bit |= 1 << net_id;
  }
  else{
    GF_ASSERT_MSG(0, "recv_mystbit=%d, member_bit=%d, net_id=%d\n", 
      uniapp->recv_mystatus_req_bit, uniapp->basic_status.member_bit, net_id);
  }
}

//==================================================================
/**
 * ��{���\���̂̃T�C�Y���擾����
 *
 * @retval  u32		
 */
//==================================================================
u32 UnionAppSystem_GetBasicSize(void)
{
  return sizeof(UNION_APP_BASIC_STATUS);
}



//==============================================================================
//
//  �A�v�����Ŏg�p
//
//==============================================================================
//==================================================================
/**
 * �����A�ޏo���̃R�[���o�b�N�֐���o�^
 *
 * @param   uniapp		        ���j�I���A�v�����䃏�[�N�ւ̃|�C���^
 * @param   entry_callback		�����R�[���o�b�N
 * @param   eave_callback		  �ޏo�R�[���o�b�N
 * @param   userwork		      �R�[���o�b�N�֐��ň����Ƃ��ēn����郏�[�N�̃|�C���^
 */
//==================================================================
void Union_App_SetCallback(UNION_APP_PTR uniapp, UNION_APP_CALLBACK_ENTRY_FUNC entry_callback, UNION_APP_CALLBACK_LEAVE_FUNC leave_callback, void *userwork)
{
  uniapp->entry_callback = entry_callback;
  uniapp->leave_callback = leave_callback;
  uniapp->userwork = userwork;
}

//==================================================================
/**
 * �������֎~����@���e�@��p����
 *
 * @param   uniapp		
 *
 * @retval  BOOL		  TRUE:�֎~�ɂ����B�@FALSE:�֎~�ɏo���Ȃ�(���ɗ�����]�҂�����)
 */
//==================================================================
BOOL Union_App_Parent_EntryBlock(UNION_APP_PTR uniapp)
{
  if(uniapp->entry_reserve_bit == 0){
    GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), FALSE);
    uniapp->entry_block = TRUE;
    OS_TPrintf("uniapp �����֎~�ɂ��܂���\n");
    return TRUE;
  }
  
  return FALSE;
}

//==================================================================
/**
 * �����֎~�����Z�b�g����@���e�@��p����
 *
 * @param   uniapp		
 */
//==================================================================
void Union_App_Parent_ResetEntryBlock(UNION_APP_PTR uniapp)
{
  GFL_NET_SetClientConnect(GFL_NET_HANDLE_GetCurrentHandle(), TRUE);
  uniapp->entry_block = FALSE;
  OS_TPrintf("uniapp ����OK�ɂ��܂���\n");
}

//==================================================================
/**
 * �ޏo���֎~����(Union_App_ReqLeave�̌��ʂ����NG�ɂȂ�܂�)�@���e�@��p����
 *
 * @param   uniapp		
 */
//==================================================================
void Union_App_Parent_LeaveBlock(UNION_APP_PTR uniapp)
{
  uniapp->leave_block = TRUE;
}

//==================================================================
/**
 * �ޏo�֎~�����Z�b�g���܂��@���e�@��p����
 *
 * @param   uniapp		
 */
//==================================================================
void Union_App_Parent_ResetLeaveBlock(UNION_APP_PTR uniapp)
{
  uniapp->leave_block = FALSE;
}

//==================================================================
/**
 * �e�@�ɓr���ޏo�̋������N�G�X�g���܂�
 *
 * @param   uniapp		
 *
 * ���̊֐����s��AUnion_App_GetLeaveResult�Ō��ʂ̎擾���s���Ă�������
 */
//==================================================================
void Union_App_ReqLeave(UNION_APP_PTR uniapp)
{
  uniapp->send_leave_req = TRUE;
  uniapp->recv_leave_req_result = UNION_APP_LEAVE_NULL;
}

//==================================================================
/**
 * �r���ޏo���N�G�X�g�̐e�@����̕Ԏ����擾���܂�
 *
 * @param   uniapp		
 *
 * @retval  UNION_APP_LEAVE		�e�@����̕Ԏ�
 */
//==================================================================
UNION_APP_LEAVE Union_App_GetLeaveResult(UNION_APP_PTR uniapp)
{
  return uniapp->recv_leave_req_result;
}

//==================================================================
/**
 * �ʐM�ؒf���܂�
 *
 * @param   uniapp		
 */
//==================================================================
void Union_App_Shutdown(UNION_APP_PTR uniapp)
{
  uniapp->shutdown_req = TRUE;
}

//==================================================================
/**
 * �ʐM�ؒf�̊����҂�
 *
 * @param   uniapp		
 *
 * @retval  BOOL		TRUE:�ʐM�ؒf�����@FALSE:������
 */
//==================================================================
BOOL Union_App_WaitShutdown(UNION_APP_PTR uniapp)
{
  return uniapp->shutdown;
}

//==================================================================
/**
 * �w��NetID��MYSTATUS���擾���܂�
 *
 * @param   net_id		�擾�����������NetID
 *
 * @retval  const MYSTATUS *		MYSTATUS�ւ̃|�C���^(NetID�̃��[�U�[���ޏo���Ă���ꍇ��NULL)
 */
//==================================================================
const MYSTATUS * Union_App_GetMystatus(UNION_APP_PTR uniapp, NetID net_id)
{
  if((uniapp->recv_mystatus_bit & (1 << net_id)) == 0 
      || (uniapp->basic_status.member_bit & (1 << net_id)) == 0){
    return NULL;
  }
  
  return uniapp->mystatus[net_id];
}

//==================================================================
/**
 * �ڑ����Ă���v���C���[��NetID��bit�P�ʂŎ擾���܂�
 *    ��GFL_NET_SendDataExBit�ł��̂܂܎g����`�ł�
 *
 * @param   uniapp		
 *
 * @retval  u32		�ڑ��v���C���[NetID(bit�P��)
 *
 * NetID:0�� NetID:2�� �̃v���C���[�������q�����Ă���ꍇ��
 *    return (1 << 0) | (1 << 2);
 * �Ƃ������l���Ԃ�܂�
 */
//==================================================================
u32 Union_App_GetMemberNetBit(UNION_APP_PTR uniapp)
{
  return uniapp->basic_status.member_bit;
}

//==================================================================
/**
 * �ڑ��l�����擾���܂�
 *
 * @param   uniapp		
 *
 * @retval  u8		�ڑ��l��
 */
//==================================================================
u8 Union_App_GetMemberNum(UNION_APP_PTR uniapp)
{
  return MATH_CountPopulation((u32)(uniapp->basic_status.member_bit)); 
}
