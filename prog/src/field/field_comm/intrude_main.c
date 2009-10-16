//==============================================================================
/**
 * @file    intrude_main.c
 * @brief   �N�����C���F�풓�ɔz�u
 * @author  matsuda
 * @date    2009.09.03(��)
 *
 * �ʐM���N�����A��Ɏ��s���ꑱ���郁�C���ł�
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
#include "palace_sys.h"
#include "intrude_main.h"
#include "bingo_system.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�v���t�B�[���v�����N�G�X�g���ēx������܂ł̃E�F�C�g
#define INTRUDE_PROFILE_REQ_RETRY_WAIT    (30)

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void Intrude_CheckProfileReq(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckTalkAnswerNG(INTRUDE_COMM_SYS_PTR intcomm);



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �N�����C�����[�v
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_Main(INTRUDE_COMM_SYS_PTR intcomm)
{
  //�v���t�B�[���v�����N�G�X�g���󂯂Ă���Ȃ瑗�M
  if(intcomm->profile_req == TRUE){
    IntrudeSend_Profile(intcomm);
  }
  //�ʐM���m������Ă��郁���o�[�Ńv���t�B�[���������Ă��Ȃ������o�[������Ȃ烊�N�G�X�g��������
  Intrude_CheckProfileReq(intcomm);
  
  //�b���������f��̕Ԏ������܂��Ă���Ȃ�Ԏ��𑗐M����
  Intrude_CheckTalkAnswerNG(intcomm);
  //�r���S�̑��M���N�G�X�g������Α��M
  Bingo_SendUpdate(intcomm, Bingo_GetBingoSystemWork(intcomm));
}

//--------------------------------------------------------------
/**
 * �ʐM���m������Ă��郁���o�[�Ńv���t�B�[���������Ă��Ȃ������o�[������Ȃ烊�N�G�X�g��������
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckProfileReq(INTRUDE_COMM_SYS_PTR intcomm)
{
  int net_id;
  
  if(intcomm->profile_req_wait > 0){
    intcomm->profile_req_wait--;
    return;
  }
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(GFL_NET_IsConnectMember(net_id) == TRUE 
        && (intcomm->recv_profile & (1 << net_id)) == 0){
      IntrudeSend_ProfileReq();
      intcomm->profile_req_wait = INTRUDE_PROFILE_REQ_RETRY_WAIT;
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * �b������NG�̕Ԏ����������Ă���Α��M���s��
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckTalkAnswerNG(INTRUDE_COMM_SYS_PTR intcomm)
{
  int net_id;
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(intcomm->answer_talk_ng_bit & (1 << net_id)){
      if(GFL_NET_IsConnectMember(net_id) == TRUE){
        if(IntrudeSend_TalkAnswer(intcomm, net_id, INTRUDE_TALK_STATUS_NG) == TRUE){
          intcomm->answer_talk_ng_bit ^= 1 << net_id;
        }
      }
      else{
        intcomm->answer_talk_ng_bit ^= 1 << net_id;
      }
    }
  }
}


//==============================================================================
//  �A�N�Z�X�֐�
//==============================================================================
//==================================================================
/**
 * �A�N�V�����X�e�[�^�X���Z�b�g
 *
 * @param   intcomm		
 * @param   action		
 */
//==================================================================
void Intrude_SetActionStatus(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_ACTION action)
{
  intcomm->intrude_status_mine.action_status = action;
}

//==================================================================
/**
 * ��b�\���̏�����
 *
 * @param   intcomm		
 * @param   talk_netid		
 */
//==================================================================
void Intrude_InitTalkWork(INTRUDE_COMM_SYS_PTR intcomm, int talk_netid)
{
  GFL_STD_MemClear(&intcomm->talk, sizeof(INTRUDE_TALK));
  intcomm->talk.talk_netid = talk_netid;
  intcomm->talk.answer_talk_netid = INTRUDE_NETID_NULL;
}

//==================================================================
/**
 * ��b�G���g���[
 *
 * @param   intcomm		
 * @param   net_id		�G���g���[�҂�NetID
 */
//==================================================================
void Intrude_SetTalkReq(INTRUDE_COMM_SYS_PTR intcomm, int net_id)
{
  if((intcomm->recv_profile & (1 << net_id)) == 0){
    OS_TPrintf("�b��������ꂽ���ǁA�܂��v���t�B�[������M���Ă��Ȃ��̂Œf��\n");
    intcomm->answer_talk_ng_bit |= 1 << net_id;
    return;
  }

  switch(intcomm->intrude_status_mine.action_status){
  case INTRUDE_ACTION_FIELD:
    if(intcomm->talk.talk_netid == INTRUDE_NETID_NULL){
      intcomm->talk.talk_netid = net_id;
      intcomm->talk.talk_status = INTRUDE_TALK_STATUS_OK;
    }
    else{
      intcomm->answer_talk_ng_bit |= 1 << net_id;
    }
    break;
  default:
    intcomm->answer_talk_ng_bit |= 1 << net_id;
    break;
  }
}

//==================================================================
/**
 * ��b�̕Ԏ���M�f�[�^�Z�b�g
 *
 * @param   intcomm		
 * @param   net_id		    �Ԏ������Ă����l��NetID
 * @param   talk_status		��b�X�e�[�^�X
 */
//==================================================================
void Intrude_SetTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm, int net_id, INTRUDE_TALK_STATUS talk_status)
{
  if(intcomm->talk.answer_talk_netid == INTRUDE_NETID_NULL && intcomm->talk.talk_netid == net_id){
    intcomm->talk.answer_talk_netid = net_id;
    intcomm->talk.answer_talk_status = talk_status;
  }
}

//==================================================================
/**
 * ��b�̕Ԏ���M�f�[�^���擾
 *
 * @param   intcomm		
 * @param   net_id		    �Ԏ������Ă����l��NetID
 * @param   talk_status		��b�X�e�[�^�X
 */
//==================================================================
INTRUDE_TALK_STATUS Intrude_GetTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm)
{
  if(intcomm->talk.answer_talk_netid != INTRUDE_NETID_NULL){
    return intcomm->talk.answer_talk_status;
  }
  return INTRUDE_TALK_STATUS_NULL;
}

