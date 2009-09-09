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


//==============================================================================
//  �萔��`
//==============================================================================
///�v���t�B�[���v�����N�G�X�g���ēx������܂ł̃E�F�C�g
#define INTRUDE_PROFILE_REQ_RETRY_WAIT    (30)

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void Intrude_CheckProfileReq(INTRUDE_COMM_SYS_PTR intcomm);



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
