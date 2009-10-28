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
 * @param   mission		�~�b�V�����V�X�e���ւ̃|�C���^
 */
//==================================================================
void MISSION_Init(MISSION_SYSTEM *mission)
{
  GFL_STD_MemClear(mission, sizeof(MISSION_SYSTEM));
  mission->data.mission_no = MISSION_NO_NULL;
  mission->result.mission_data.mission_no = MISSION_NO_NULL;
}

//==================================================================
/**
 * �~�b�V�����f�[�^���M���N�G�X�g��ݒ肷��
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Set_DataSendReq(MISSION_SYSTEM *mission)
{
  mission->data_send_req = TRUE;
}

//==================================================================
/**
 * �~�b�V�������N�G�X�g�f�[�^���󂯎��A�~�b�V�����𔭓�����
 *
 * @param   mission		      �~�b�V�����V�X�e���ւ̃|�C���^
 * @param   req		          ���N�G�X�g�f�[�^�ւ̃|�C���^
 * @param   accept_netid		�~�b�V�����󒍎҂�NetID
 *
 * @retval  BOOL		TRUE:�~�b�V���������B�@FALSE:�����ł��Ȃ�
 */
//==================================================================
BOOL MISSION_SetEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_REQ *req, int accept_netid)
{
  int i, target_no, connect_num;
  MISSION_DATA *mdata = &mission->data;
  
  if(mdata->mission_no != MISSION_NO_NULL){
    return FALSE;
  }
  mdata->mission_no = GFUser_GetPublicRand0(MISSION_NO_MAX);
  mdata->accept_netid = accept_netid;
  mdata->monolith_type = req->monolith_type;
  
  connect_num = GFL_NET_GetConnectNum();
  target_no = GFUser_GetPublicRand0(connect_num - 1); // -1 = �󒍎ҕ�
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(i != accept_netid && intcomm->recv_profile & (1 << i)){
      if(target_no == 0){
        mdata->target_netid = i;
        break;
      }
    }
  }
  if(i == FIELD_COMM_MEMBER_MAX){
    GF_ASSERT_MSG(0, "connect_num = %d, accept_netid = %d, recv_profile = %d\n", 
      connect_num, accept_netid, intcomm->recv_profile);
    mdata->target_netid = 0;
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
void MISSION_SetMissionData(MISSION_SYSTEM *mission, const MISSION_DATA *src)
{
  MISSION_DATA *mdata = &mission->data;
  
  //�e�̏ꍇ�A����misison_no�̓Z�b�g����Ă���̂Ŕ���̑O�Ɏ�M�t���O���Z�b�g
  mission->parent_data_recv = TRUE;
  if(mdata->mission_no != MISSION_NO_NULL){
    return;
  }
  
  *mdata = *src;
  OS_TPrintf("mission��M mission_no = %d\n", src->mission_no);
  //�s���`�F�b�N
#if 0 //����������������ƃf�[�^�̎�舵�������܂��Ă���L���ɂ���
  if(mdata->mission_no >= MISSION_NO_MAX || mdata->monolith_type >= MONOLITH_TYPE_MAX){
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
void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->data_send_req == TRUE){
    if(IntrudeSend_MissionData(intcomm, mission) == TRUE){
      mission->data_send_req = FALSE;
    }
  }
  
  if(mission->result_send_req == TRUE){
    if(IntrudeSend_MissionResult(intcomm, mission) == TRUE){
      mission->result_send_req = FALSE;
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
BOOL MISSION_RecvCheck(const MISSION_SYSTEM *mission)
{
  const MISSION_DATA *mdata = &mission->data;
  
  if(mdata->mission_no == MISSION_NO_NULL || mission->parent_data_recv == FALSE){
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
u16 MISSION_GetMissionMsgID(const MISSION_SYSTEM *mission)
{
  const MISSION_DATA *mdata = &mission->data;
  u16 msg_id;
  
  if(mdata->mission_no == MISSION_NO_NULL || mdata->mission_no >= MISSION_NO_MAX){
    return msg_invasion_mission000;
  }
  msg_id = mdata->mission_no * 2 + mdata->monolith_type;
  return msg_invasion_mission001 + msg_id;
}

//==================================================================
/**
 * �~�b�V�������ʂ���Ή��������ʃ��b�Z�[�WID���擾����
 *
 * @param   mission		
 *
 * @retval  u16		���b�Z�[�WID
 */
//==================================================================
u16 MISSION_GetAchieveMsgID(const MISSION_SYSTEM *mission, int my_netid)
{
  const MISSION_RESULT *result = &mission->result;
  
  if(result->mission_data.mission_no == MISSION_NO_NULL 
      || result->mission_data.mission_no >= MISSION_NO_MAX){
    return msg_invasion_mission_sys002;
  }
  
  if(my_netid == result->achieve_netid){
    return msg_invasion_mission001_02;
  }
  return msg_invasion_mission001_03;
}

//==================================================================
/**
 * �~�b�V�����B���񍐃G���g���[
 *
 * @param   mission		
 * @param   mdata		        �B�������~�b�V�����f�[�^
 * @param   achieve_netid		�B���҂�NetID
 *
 * @retval  BOOL		TRUE�F�B���҂Ƃ��Ď󂯕t����ꂽ
 * @retval  BOOL		FALSE�F��ɒB���҂����铙���Ď󂯕t�����Ȃ�����
 */
//==================================================================
BOOL MISSION_EntryAchieve(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid)
{
  MISSION_RESULT *result = &mission->result;
  MISSION_DATA *data = &mission->data;
  
  OS_TPrintf("�~�b�V�����B���G���g���[ net_id=%d\n", achieve_netid);
  if(result->mission_data.mission_no != MISSION_NO_NULL || mission->parent_result_recv == TRUE){
    OS_TPrintf("��ɒB���҂������̂Ŏ󂯕t���Ȃ�\n");
    return FALSE;
  }
  if(GFL_STD_MemComp(data, mdata, sizeof(MISSION_DATA)) != 0){
    OS_TPrintf("�e���Ǘ����Ă���~�b�V�����f�[�^�Ɠ��e���Ⴄ�̂Ŏ󂯕t���Ȃ�\n");
    return FALSE;
  }
  
  result->mission_data = *mdata;
  result->achieve_netid = achieve_netid;
  mission->result_send_req = TRUE;
  return TRUE;
}

//==================================================================
/**
 * �~�b�V�������ʂ��Z�b�g
 *
 * @param   mission		
 * @param   cp_result		
 */
//==================================================================
void MISSION_SetResult(MISSION_SYSTEM *mission, const MISSION_RESULT *cp_result)
{
  MISSION_RESULT *result = &mission->result;
  
  GF_ASSERT(mission->parent_result_recv == FALSE);
  *result = *cp_result;
  mission->parent_result_recv = TRUE;
}

//==================================================================
/**
 * �~�b�V�������ʎ�M�m�F
 *
 * @param   mission		�~�b�V�����V�X�e���ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:��M�����B�@FALSE:��M���Ă��Ȃ�
 */
//==================================================================
BOOL MISSION_RecvAchieve(const MISSION_SYSTEM *mission)
{
  const MISSION_RESULT *result = &mission->result;
  
  if(mission->parent_result_recv == TRUE && result->mission_data.mission_no != MISSION_NO_NULL){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �b�������F�~�b�V�����B���`�F�b�N
 *
 * @param   mission		    �~�b�V�����f�[�^�ւ̃|�C���^
 * @param   talk_netid		�b�������������NetID
 *
 * @retval  BOOL		TRUE:�B���B�@FALSE:��B��
 */
//==================================================================
BOOL MISSION_Talk_CheckAchieve(const MISSION_SYSTEM *mission, int talk_netid)
{
  const MISSION_DATA *mdata = &mission->data;
  
  if(mdata->mission_no == MISSION_NO_NULL){
    return FALSE;
  }
  if(talk_netid == mdata->target_netid){
    return TRUE;
  }
  return FALSE;
}

