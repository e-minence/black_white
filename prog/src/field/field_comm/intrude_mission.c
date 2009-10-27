//==============================================================================
/**
 * @file    intrude_mission.c
 * @brief   �~�b�V��������    ��M�f�[�^�̃Z�b�g�Ȃǂ��s���ׁA�풓�ɔz�u
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#include <gflib.h>
#include "intrude_types.h"
#include "intrude_mission.h"
#include "intrude_comm_command.h"
#include "msg/msg_invasion.h"


//==================================================================
/**
 * �~�b�V�����f�[�^������
 *
 * @param   mission		�~�b�V�����f�[�^�ւ̃|�C���^
 */
//==================================================================
void MISSION_Init(MISSION_DATA *mission)
{
  mission->mission_no = MISSION_NO_NULL;
}

//==================================================================
/**
 * �~�b�V�������N�G�X�g�f�[�^���󂯎��A�~�b�V�����𔭓�����
 *
 * @param   mission		      �~�b�V�����f�[�^�ւ̃|�C���^
 * @param   req		          ���N�G�X�g�f�[�^�ւ̃|�C���^
 * @param   accept_netid		�~�b�V�����󒍎҂�NetID
 *
 * @retval  BOOL		TRUE:�~�b�V���������B�@FALSE:�����ł��Ȃ�
 */
//==================================================================
BOOL MISSION_SetEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_DATA *mission, const MISSION_REQ *req, int accept_netid)
{
  int i, target_no, connect_num;
  
  if(mission->mission_no != MISSION_NO_NULL){
    return FALSE;
  }
  mission->mission_no = GFUser_GetPublicRand0(MISSION_NO_MAX);
  mission->accept_netid = accept_netid;
  mission->monolith_type = req->monolith_type;
  
  connect_num = GFL_NET_GetConnectNum();
  target_no = GFUser_GetPublicRand0(connect_num - 1); // -1 = �󒍎ҕ�
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(i != accept_netid && intcomm->recv_profile & (1 << i)){
      if(target_no == 0){
        mission->target_netid = i;
        break;
      }
    }
  }
  if(i == FIELD_COMM_MEMBER_MAX){
    GF_ASSERT_MSG(0, "connect_num = %d, accept_netid = %d, recv_profile = %d\n", 
      connect_num, accept_netid, intcomm->recv_profile);
    mission->target_netid = 0;
  }
  
  return TRUE;
}

//==================================================================
/**
 * �~�b�V�����f�[�^���Z�b�g
 *
 * @param   mission		�����
 * @param   src		    �Z�b�g����f�[�^
 */
//==================================================================
void MISSION_SetMissionData(MISSION_DATA *mission, const MISSION_DATA *src)
{
  if(mission->mission_no != MISSION_NO_NULL){
    return;
  }
  
  *mission = *src;
  OS_TPrintf("mission��M mission_no = %d\n", src->mission_no);
  //�s���`�F�b�N
#if 0 //����������������ƃf�[�^�̎�舵�������܂��Ă���L���ɂ���
  if(mission->mission_no >= MISSION_NO_MAX || mission->monolith_type >= MONOLITH_TYPE_MAX){
    return;
  }
#endif
}

//==================================================================
/**
 * �~�b�V�����f�[�^���M���N�G�X�g���������Ă���Α��M���s��
 *
 * @param   intcomm		
 * @param   mission		
 */
//==================================================================
void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_DATA *mission)
{
  if(intcomm->mission_send_req == TRUE){
    if(IntrudeSend_MissonData(intcomm, mission) == TRUE){
      intcomm->mission_send_req = FALSE;
    }
  }
}

//==================================================================
/**
 * �~�b�V�����f�[�^��M�ς݂��`�F�b�N
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:��M�ς݁@FALSE:��M���Ă��Ȃ�
 */
//==================================================================
BOOL MISSION_RecvCheck(const MISSION_DATA *mission)
{
  if(mission->mission_no == MISSION_NO_NULL){
    return FALSE;
  }
  return TRUE;
}

//==================================================================
/**
 * �~�b�V�����f�[�^����Ή������~�b�V�������b�Z�[�WID���擾����
 *
 * @param   mission		
 *
 * @retval  u16		���b�Z�[�WID
 */
//==================================================================
u16 MISSION_GetMissionMsgID(const MISSION_DATA *mission)
{
  u16 msg_id;
  
  if(mission->mission_no == MISSION_NO_NULL || mission->mission_no >= MISSION_NO_MAX){
    return msg_invasion_mission000;
  }
  msg_id = mission->mission_no * 2 + mission->monolith_type;
  return msg_invasion_mission001 + msg_id;
}
