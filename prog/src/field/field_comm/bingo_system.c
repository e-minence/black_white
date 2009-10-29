//==============================================================================
/**
 * @file    bingo_system.c
 * @brief   �r���S�V�X�e������
 * @author  matsuda
 * @date    2009.10.10(�y)
 * 
 * �f�[�^�A�N�Z�X�n���ʐM�R�}���h������Ă΂��ׁAintrude_comm_command.c�Ɠ����I�[�o�[���C��
 * �z�u������K�v������܂��B
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "savedata/mystatus.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "net_app/union/comm_player.h"
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "intrude_main.h"
#include "bingo_system.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void Bingo_Parent_UpdateIntrusionData(BINGO_SYSTEM *bingo);



//==================================================================
/**
 * �N���V�X�e�����[�N����r���S�V�X�e�����[�N�̃|�C���^���擾����
 *
 * @param   intcomm		
 *
 * @retval  BINGO_SYSTEM *		
 */
//==================================================================
BINGO_SYSTEM * Bingo_GetBingoSystemWork(INTRUDE_COMM_SYS_PTR intcomm)
{
  return &intcomm->bingo;
}

//==================================================================
/**
 * �r���S�V�X�e�����[�N��������
 *
 * @param   bingo		
 */
//==================================================================
void Bingo_InitBingoSystem(BINGO_SYSTEM *bingo)
{
  GFL_STD_MemClear(bingo, sizeof(BINGO_SYSTEM));
  Bingo_Clear_BingoBattleBeforeBuffer(bingo);
  Bingo_Clear_BingoIntrusionBeforeBuffer(bingo);
}

//==================================================================
/**
 * �r���S�o�g���J�n�O�ɂ��Ȃ���΂����Ȃ��o�b�t�@�̏�����
 *
 * @param   bingo		
 */
//==================================================================
void Bingo_Clear_BingoBattleBeforeBuffer(BINGO_SYSTEM *bingo)
{
  int i;

  GF_ASSERT(bingo->intrusion_netid_bit == 0);
  
  GFL_STD_MemClear(&bingo->intrusion_param, sizeof(BINGO_INTRUSION_PARAM));
  bingo->intrusion_param.occ = TRUE;  //��check�@�܂��r���S�o�g�����̂��̂������̂ŉ�
  
  bingo->intrusion_netid_bit = 0;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    bingo->parent.intrusion_player[i] = INTRUDE_NETID_NULL;
    bingo->parent.intrusion_player_answer[i] = INTRUDE_NETID_NULL;
  }
}

//==================================================================
/**
 * �����v���J�n�O�ɂ��Ȃ���΂����Ȃ��o�b�t�@�̏�����
 *
 * @param   bingo		
 */
//==================================================================
void Bingo_Clear_BingoIntrusionBeforeBuffer(BINGO_SYSTEM *bingo)
{
  bingo->intrusion_recv_answer = BINGO_INTRUSION_ANSWER_WAIT;
  GFL_STD_MemClear(&bingo->intrusion_param, sizeof(BINGO_INTRUSION_PARAM));
}

//==================================================================
/**
 * �r���S�̑��M�f�[�^������Α��M���s��
 *
 * @param   bingo		
 */
//==================================================================
void Bingo_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, BINGO_SYSTEM *bingo)
{
  int net_id;
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    //�������̕Ԏ�
    if(bingo->intrusion_answer_ok_netid_bit & (1 << net_id)){
      if(IntrudeSend_BingoIntrusionAnswer(net_id, BINGO_INTRUSION_ANSWER_OK) == TRUE){
        bingo->intrusion_answer_ok_netid_bit ^= 1 << net_id;
      }
    }
    else if(bingo->intrusion_answer_ng_netid_bit & (1 << net_id)){
      if(IntrudeSend_BingoIntrusionAnswer(net_id, BINGO_INTRUSION_ANSWER_NG) == TRUE){
        bingo->intrusion_answer_ng_netid_bit ^= 1 << net_id;
      }
    }
    
    //�����p�����[�^�v�����������ꍇ�A���M
    if(bingo->param_req_bit & (1 << net_id)){
      if(IntrudeSend_BingoBattleIntrusionParam(bingo, net_id) == TRUE){
        bingo->param_req_bit ^= 1 << net_id;
      }
    }
  }

  //�N���V�X�e���Ƃ�Parent���[�N�������������NG�A�f�[�^�X�V����
  if(intcomm->intrude_status_mine.action_status == INTRUDE_ACTION_BINGO_BATTLE){
    Bingo_Parent_UpdateIntrusionData(bingo);
  }
}

//==================================================================
/**
 * �r���S������]���N�G�X�g���󂯕t����
 *
 * @param   intcomm		
 * @param   bingo		
 * @param   net_id		������]�҂�NetID
 */
//==================================================================
void Bingo_Req_IntrusionPlayer(INTRUDE_COMM_SYS_PTR intcomm, BINGO_SYSTEM *bingo, int net_id)
{
  GF_ASSERT((bingo->intrusion_netid_bit & (1 << net_id)) == 0);
  
  if(intcomm->intrude_status_mine.action_status == INTRUDE_ACTION_BINGO_BATTLE){
    bingo->intrusion_netid_bit |= 1 << net_id;
    Bingo_Parent_EntryIntusion(bingo, net_id);
  }
  else{
    bingo->intrusion_answer_ng_netid_bit |= 1 << net_id;
    OS_TPrintf("����NG net_id = %d\n", net_id);
  }
}

//--------------------------------------------------------------
/**
 * �r���S�o�g�������痐����]�̕Ԏ����������Ă���Α��M�f�[�^�ɓo�^���܂�
 *
 * @param   bingo		
 */
//--------------------------------------------------------------
static void Bingo_Parent_UpdateIntrusionData(BINGO_SYSTEM *bingo)
{
  int i, net_id;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    net_id = bingo->parent.intrusion_player[i];
    if(net_id != INTRUDE_NETID_NULL && (bingo->intrusion_netid_bit & (1 << net_id))){
      if(bingo->parent.intrusion_player_answer[i] == BINGO_INTRUSION_ANSWER_OK){
        bingo->intrusion_answer_ok_netid_bit |= 1 << net_id;
        bingo->intrusion_netid_bit ^= 1 << net_id;
      }
      else if(bingo->parent.intrusion_player_answer[i] == BINGO_INTRUSION_ANSWER_NG){
        bingo->intrusion_answer_ng_netid_bit |= 1 << net_id;
        bingo->intrusion_netid_bit ^= 1 << net_id;
      }
      //��check�@�f�o�b�O�p �r���S�o�g�����o����܂ł̉��z��������NG�����
      else{
        bingo->parent.intrusion_player_answer[i] = GFUser_GetPublicRand(BINGO_INTRUSION_ANSWER_NG + 1);
      }
    }
  }
}

//==================================================================
/**
 * �r���S�o�g����Parent���[�N�ւ̗����v���G���g���[
 *
 * @param   bingo		  
 * @param   net_id		�G���g���[�҂̃l�b�gID
 */
//==================================================================
void Bingo_Parent_EntryIntusion(BINGO_SYSTEM *bingo, int net_id)
{
  int i, set_pos = INTRUDE_NETID_NULL;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    //�݌v�I�ɂ��肦�Ȃ��͂������ǔO�̂��߁A���ɓo�^�ς݂łȂ����m�F
    if(bingo->parent.intrusion_player[i] == net_id){
      //GF_ASSERT(0);
      return; //���ɓo�^����Ă���
    }
    else if(set_pos == INTRUDE_NETID_NULL 
        && bingo->parent.intrusion_player[i] == INTRUDE_NETID_NULL){
      set_pos = i;
    }
  }
  
  if(set_pos == INTRUDE_NETID_NULL){
    GF_ASSERT(0);
    return;
  }
  bingo->parent.intrusion_player[set_pos] = net_id;
  OS_TPrintf("������⃊�X�g�ɓo�^ net_id = %d, %d�Ԗ�\n", net_id, set_pos);
}

//==================================================================
/**
 * �r���S�������v���̕Ԏ���M�f�[�^���擾
 *
 * @param   intcomm		
 * @param   net_id		    �Ԏ������Ă����l��NetID
 * @param   answer		    BINGO_INTRUSION_ANSWER
 */
//==================================================================
BINGO_INTRUSION_ANSWER Bingo_GetBingoIntrusionAnswer(INTRUDE_COMM_SYS_PTR intcomm)
{
  BINGO_SYSTEM *bingo = Bingo_GetBingoSystemWork(intcomm);
  
  return bingo->intrusion_recv_answer;
}

//==================================================================
/**
 * �r���S�����p�����[�^���L���ɂȂ��Ă��邩���ׂ�
 * @param   intcomm		
 * @retval  BOOL		TRUE:�L��
 */
//==================================================================
BOOL Bingo_GetBingoIntrusionParam(INTRUDE_COMM_SYS_PTR intcomm)
{
  BINGO_SYSTEM *bingo = Bingo_GetBingoSystemWork(intcomm);

  return bingo->intrusion_param.occ;
}
