//==============================================================================
/**
 * @file    intrude_mission.c
 * @brief   �~�b�V��������    ��M�f�[�^�̃Z�b�g�Ȃǂ��s���ׁA�풓�ɔz�u
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "intrude_types.h"
#include "intrude_mission.h"
#include "intrude_comm_command.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_msg.h"
#include "intrude_main.h"
#include "intrude_work.h"
#include "print/wordset.h"
#include "field/zonedata.h"

#include "mission.naix"
#include "msg/msg_mission.h"
#include "msg/msg_mission_monolith.h"
#include "mission_ng_check.h"


SDK_COMPILER_ASSERT(MISSION_LIST_MAX == MISSION_TYPE_MAX);


//==============================================================================
//  �萔��`
//==============================================================================
///�~�b�V�����J�n�܂ł̏�������
#define _MISSION_READY_TIMER      (10)  //�b

enum{
  _SEND_MISSION_START_NULL,         //���N�G�X�g�Ȃ�
  _SEND_MISSION_START_SEND_REQ,     //���M���N�G�X�g
  _SEND_MISSION_START_SENDED,       //���M����
};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_Update_StartClientAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_SetMissionFail(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int fail_netid);
static void MISSION_Update_EntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_Update_AchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_ClearTargetInfo(MISSION_TARGET_INFO *target);
static s32 _GetMissionTime(MISSION_SYSTEM *mission);
static void MISSION_ClearMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);


//==============================================================================
//  �f�[�^
//==============================================================================
#include "mission_list_param.cdat"




//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �~�b�V�����f�[�^������
 *
 * @param   mission		�~�b�V�����V�X�e���ւ̃|�C���^
 */
//==================================================================
void MISSION_Init(MISSION_SYSTEM *mission)
{
  int i;
  
  GFL_STD_MemClear(mission, sizeof(MISSION_SYSTEM));
  mission->data.accept_netid = INTRUDE_NETID_NULL;
  MISSION_ClearTargetInfo(&mission->data.target_info);
  mission->result.mission_data.accept_netid = INTRUDE_NETID_NULL;
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    mission->result.achieve_netid[i] = INTRUDE_NETID_NULL;
  }
  
  MISSION_Init_List(mission);
}

//==================================================================
/**
 * �~�b�V�������X�g������
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Init_List(MISSION_SYSTEM *mission)
{
  int i;
  MISSION_TYPE type;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    mission->list[i].occ = FALSE;
  }
}

//==================================================================
/**
 * �~�b�V�����X�V����
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Update(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  //�e�@�Ń~�b�V�����������Ă���Ȃ玸�s�܂ł̕b�����J�E���g����
  if(GFL_NET_IsParentMachine() == TRUE 
      && MISSION_RecvCheck(mission) == TRUE){
    if(mission->result.mission_data.accept_netid == INTRUDE_NETID_NULL){
      if(mission->data.variable.ready_timer > 0){
        if(mission->send_mission_start == _SEND_MISSION_START_NULL 
            && _GetMissionTime(mission) > mission->data.variable.ready_timer){
          mission->send_mission_start = _SEND_MISSION_START_SEND_REQ;
        }
      }
      else if(mission->mission_start_ok == TRUE){
        if(_GetMissionTime(mission) > mission->data.variable.exe_timer){
          GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
          if(mission->result.mission_fail == FALSE){
            MISSION_SetMissionFail(intcomm, mission, GAMEDATA_GetIntrudeMyID(gamedata));
          }
        }
      }
    }
    else{ //�N���B���҂�����
      if(mission->mission_start_ok == TRUE){
        if(_GetMissionTime(mission) > mission->data.variable.exe_timer){
          //�~�b�V�����̐������ԂɒB�������������ďI��
          mission->mission_timeend_success = TRUE;
        }
      }
    }
  }
  
  //���M���N�G�X�g������Α��M
  MISSION_Update_EntryAnswer(intcomm, mission);
  MISSION_Update_AchieveAnswer(intcomm, mission);
  MISSION_SendUpdate(intcomm, mission);
  MISSION_Update_StartClientAnswer(intcomm, mission);
}

//--------------------------------------------------------------
/**
 * �~�b�V�����o�ߎ��Ԃ��擾
 *
 * @param   mission		
 *
 * @retval  s32		�o�ߎ���
 */
//--------------------------------------------------------------
static s32 _GetMissionTime(MISSION_SYSTEM *mission)
{
  u32 timer = GFL_RTC_GetTimeBySecond();
  
  if(timer < mission->start_timer){ //start_timer��菬�����Ȃ��Ă���ꍇ�͉�荞�݂�����
    timer += GFL_RTC_TIME_SECOND_MAX + 1;
  }
  return timer - mission->start_timer;
}

//--------------------------------------------------------------
/**
 * ���{���̃~�b�V�����̎c�莞�Ԃ��擾
 *
 * @param   mission		
 *
 * @retval  s32		�c�莞��
 */
//--------------------------------------------------------------
static s32 MISSION_GetExeMissionTimer(MISSION_SYSTEM *mission)
{
  s32 ret_timer;
  
 ret_timer = mission->data.variable.exe_timer;  //mission->data.cdata.time;
 ret_timer -= _GetMissionTime(mission);
  if(ret_timer < 0){
    return 0;
  }
  return ret_timer;
}

//==================================================================
/**
 * �~�b�V�����c�莞�Ԃ��擾
 *
 * @param   mission		
 *
 * @retval  s32		�c�莞��
 */
//==================================================================
s32 MISSION_GetMissionTimer(MISSION_SYSTEM *mission)
{
  s32 timer, ret_timer;
  
  if(MISSION_RecvCheck(mission) == TRUE 
      && mission->result.mission_data.accept_netid == INTRUDE_NETID_NULL){
    if(mission->data.variable.ready_timer > 0 || mission->mission_start_ok == FALSE){
      ret_timer = mission->data.variable.ready_timer;
      ret_timer -= _GetMissionTime(mission);
    }
    else{
      ret_timer = MISSION_GetExeMissionTimer(mission);
    }
    
    if(ret_timer < 0){
      return 0;
    }
    return ret_timer;
  }
  return 0;
}

//==================================================================
/**
 * �~�b�V�������X�g���M���N�G�X�g��ݒ肷��
 *
 * @param   mission		
 */
//==================================================================
void MISSION_Set_ListSendReq(MISSION_SYSTEM *mission, int palace_area)
{
  mission->list_send_req[palace_area] = TRUE;
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
 * �~�b�V�������X�g���Z�b�g
 *
 * @param   mission		�����
 * @param   MISSION_CHOICE_LIST		    �Z�b�g����f�[�^
 * @param   net_id    
 */
//==================================================================
void MISSION_SetMissionList(MISSION_SYSTEM *mission, const MISSION_CHOICE_LIST *list, NetID net_id)
{
  MISSION_CHOICE_LIST *mlist = &mission->list[net_id];
  
  if(mlist->occ == TRUE){
    return; //���Ɏ�M�ς�
  }
  
  *mlist = *list;
  //�s���`�F�b�N
#if 0 //����������������ƃf�[�^�̎�舵�������܂��Ă���L���ɂ���
  if(mdata->mission_no >= MISSION_NO_MAX || mdata->monolith_type >= MONOLITH_TYPE_MAX){
    return;
  }
#endif
}

//==================================================================
/**
 * �~�b�V�����f�[�^���Z�b�g
 *
 * @param   mission		�����
 * @param   src		    �Z�b�g����f�[�^
 * 
 * @retval  new_mission   TRUE:�V�K�Ɏ�M�����~�b�V�����@FALSE:��M�ς� or �����ȃ~�b�V����
 */
//==================================================================
BOOL MISSION_SetMissionData(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_DATA *src)
{
  MISSION_DATA *mdata = &mission->data;
  BOOL new_mission = FALSE;
  
  if(mission->parent_data_recv == FALSE && src->accept_netid != INTRUDE_NETID_NULL){
    new_mission = TRUE;
    mission->parent_data_recv = TRUE;
  }
  
  //�e�̏ꍇ�A����misison_no�̓Z�b�g����Ă���̂Ŕ���̑O�Ɏ�M�t���O���Z�b�g
  //MISSION_ClearMissionEntry(intcomm, mission);  ��x�����󂯂�Ȃ��̂ŃN���A�̕K�v�͖����Ȃ��� 2010.05.29(�y)
  if(mdata->accept_netid != INTRUDE_NETID_NULL){
    return new_mission;
  }
  
  *mdata = *src;
  
  //�s���`�F�b�N
#if 0 //����������������ƃf�[�^�̎�舵�������܂��Ă���L���ɂ���
  if(mdata->mission_no >= MISSION_NO_MAX || mdata->monolith_type >= MONOLITH_TYPE_MAX){
    return;
  }
#endif
  return new_mission;
}

//==================================================================
/**
 * �~�b�V�����J�n��M
 *
 * @param   mission		
 */
//==================================================================
void MISSION_RecvMissionStart(MISSION_SYSTEM *mission)
{
  MISSION_DATA *md = MISSION_GetRecvData(mission);
  if(md != NULL){
    md->variable.ready_timer = 0;
  }
}

//==================================================================
/**
 * �N���C�A���g����u�~�b�V�����J�n�錾�v����M
 *
 * @param   mission		
 * @param   net_id		
 */
//==================================================================
void MISSION_RecvMissionStartClient(MISSION_SYSTEM *mission, NetID net_id)
{
  if(mission->start_client_bit == 0){ //�N�����~�b�V�������J�n������e�����S�̃^�C�}�[���Z�b�g
    mission->start_timer = GFL_RTC_GetTimeBySecond();
  }
  mission->start_client_bit |= 1 << net_id;
  mission->send_start_client_bit |= 1 << net_id;
}

//==================================================================
/**
 * �e�����������u�~�b�V�����J�n�錾�̕Ԏ��v����M
 *
 * @param   mission		  
 * @param   net_id		  
 * @param   now_timer		�e�������������݂̃~�b�V�����̎c�莞��
 */
//==================================================================
void MISSION_RecvMissionStartClientAnswer(MISSION_SYSTEM *mission, s32 now_timer)
{
  mission->mission_start_ok = TRUE;
  mission->data.variable.exe_timer = now_timer;
  if(GFL_NET_IsParentMachine() == FALSE){ //�e��MISSION_RecvMissionStartClient�ŃZ�b�g�����
    mission->start_timer = GFL_RTC_GetTimeBySecond();
  }
  OS_TPrintf("start_timer = %d, variable.exe_timer = %d\n", mission->start_timer, now_timer);
}

//==================================================================
/**
 * �e����~�b�V�����J�nOK������������`�F�b�N
 *
 * @param   mission		
 *
 * @retval  BOOL		  TRUE:OK
 */
//==================================================================
BOOL MISSION_CheckMissionStartOK(MISSION_SYSTEM *mission)
{
  return mission->mission_start_ok;
}

//==================================================================
/**
 * �~�b�V�����f�[�^���M���N�G�X�g���������Ă���Α��M���s��
 *
 * @param   intcomm		
 * @param   mission		
 */
//==================================================================
static void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(mission->list_send_req[i] == TRUE){
      if(IntrudeSend_MissionList(intcomm, mission, i) == TRUE){
        mission->list_send_req[i] = FALSE;
      }
    }
  }
  
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
  
  if(mission->send_mission_start == _SEND_MISSION_START_SEND_REQ){
    if(IntrudeSend_MissionStart(intcomm, mission) == TRUE){
      mission->send_mission_start = _SEND_MISSION_START_SENDED;
    }
  }
}

//--------------------------------------------------------------
/**
 * �u�~�b�V�����J�n�錾�v�̕Ԏ�
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_Update_StartClientAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->send_start_client_bit > 0){
    if(IntrudeSend_MissionClientStartAnswer(
        intcomm, MISSION_GetExeMissionTimer(mission), mission->send_start_client_bit) == TRUE){
      mission->send_start_client_bit = 0;
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
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL || mission->parent_data_recv == FALSE){
    return FALSE;
  }
  return TRUE;
}

//==================================================================
/**
 * ��M�ς݂̃~�b�V�����f�[�^���擾����
 *
 * @param   mission		        
 *
 * @retval  MISSION_DATA *		�~�b�V�����f�[�^�ւ̃|�C���^(����M�̏ꍇ��NULL)
 */
//==================================================================
MISSION_DATA * MISSION_GetRecvData(MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == FALSE){
    return NULL;
  }
  return &mission->data;
}

//==================================================================
/**
 * �~�b�V�����Q���t���O���Z�b�g����
 *
 * @param   mission		
 */
//==================================================================
void MISSION_SetMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == TRUE){
    mission->mine_entry = TRUE;
    mission->mission_complete = FALSE;
    intcomm->intrude_status_mine.mission_entry = TRUE;
    intcomm->send_status = TRUE;
    OS_TPrintf("�~�b�V�����Q���t���O�Z�b�g\n");
  }
}

//--------------------------------------------------------------
/**
 * �~�b�V�����Q���t���O�����Z�b�g����
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_ClearMissionEntry(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->mine_entry == TRUE){
    mission->mine_entry = FALSE;
    intcomm->intrude_status_mine.mission_entry = FALSE;
    intcomm->send_status = TRUE;
  }
  mission->mission_complete = FALSE;
}

//==================================================================
/**
 * �~�b�V�����Q���t���O���擾����
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:�~�b�V�����ɎQ�����Ă���
 */
//==================================================================
BOOL MISSION_GetMissionEntry(const MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == TRUE){
    return mission->mine_entry;
  }
  return FALSE;
}

//==================================================================
/**
 * �~�b�V�����B���t���O���Z�b�g����
 *
 * @param   mission		
 */
//==================================================================
void MISSION_SetMissionComplete(MISSION_SYSTEM *mission)
{
  mission->mission_complete = TRUE;
}

//==================================================================
/**
 * �~�b�V�����B���t���O���擾����
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:�~�b�V�����B���ς݁@FALSE:�~�b�V�����܂����B��
 */
//==================================================================
BOOL MISSION_GetMissionComplete(const MISSION_SYSTEM *mission)
{
  return mission->mission_complete;
}

//==================================================================
/**
 * �~�b�V�������ʃf�[�^�擾
 *
 * @param   mission		
 *
 * @retval  MISSION_RESULT *		���ʃf�[�^(���ʖ���M�̏ꍇ��NULL)
 */
//==================================================================
MISSION_RESULT * MISSION_GetResultData(MISSION_SYSTEM *mission)
{
  if(MISSION_CheckRecvResult(mission) == FALSE){
    return NULL;
  }
  return &mission->result;
}

//==================================================================
/**
 * �~�b�V������V��ł����Ԃ����ׂ�
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:�~�b�V�����ŗV��ł���@FALSE:�~�b�V�����ŗV��ł��Ȃ�
 */
//==================================================================
BOOL MISSION_GetMissionPlay(MISSION_SYSTEM *mission)
{
  //�G���g���[���Ď��ۂɃ~�b�V����������Ă��邩�A
  //���ʂ��󂯎���Ă܂��\�����Ă��Ȃ����(�Q���҂łȂ���Ό��ʂ͎󂯎��Ȃ�)
  if(MISSION_GetMissionEntry(mission) == TRUE || MISSION_GetResultData(mission) != NULL){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����B���҂Ƃ��ēo�^����
 *
 * @param   mission		
 * @param   mdata		
 * @param   achieve_netid		�B���҂�NetID
 *
 * @retval  int		  �B���҂Ƃ��Ă̏���(�Ȃ����܂��Ă����ꍇ��FIELD_COMM_MEMBER_MAX)
 */
//--------------------------------------------------------------
static int _SetAchieveNetID(MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int achieve_netid)
{
  MISSION_RESULT *result = &mission->result;
  
  if(result->achieve_netid[0] == INTRUDE_NETID_NULL){
    return 0;
  }
  return FIELD_COMM_MEMBER_MAX;
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
  int ranking;
  
  OS_TPrintf("�~�b�V�����B���G���g���[ net_id=%d\n", achieve_netid);
  if(GFL_STD_MemComp(data, mdata, sizeof(MISSION_DATA) - sizeof(MISSION_VARIABLE_PARAM)) != 0){
    OS_TPrintf("�e���Ǘ����Ă���~�b�V�����f�[�^�Ɠ��e���Ⴄ�̂Ŏ󂯕t���Ȃ�\n");
    mission->result_mission_achieve[achieve_netid] = MISSION_ACHIEVE_NG;
    return FALSE;
  }
  
  ranking = _SetAchieveNetID(mission, mdata, achieve_netid);
  if(ranking == FIELD_COMM_MEMBER_MAX){
    OS_TPrintf("��ɒB���҂������̂Ŏ󂯕t���Ȃ�\n");
    mission->result_mission_achieve[achieve_netid] = MISSION_ACHIEVE_NG;
    return FALSE;
  }
  
  result->mission_data = *mdata;
  result->achieve_netid[ranking] = achieve_netid;
  
  mission->result_mission_achieve[achieve_netid] = MISSION_ACHIEVE_OK;
  mission->result_send_req = TRUE;
  
  return TRUE;
}

//==================================================================
/**
 * �~�b�V�����B���񍐂̕Ԏ����擾
 *
 * @param   mission		
 *
 * @retval  MISSIN_ACHIEVE		MISSION_ACHIEVE_NULL�̏ꍇ�͕Ԏ��͖���M
 */
//==================================================================
MISSION_ACHIEVE MISSION_GetAchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(mission->parent_achieve_recv == MISSION_ACHIEVE_OK){
    //OK�̏ꍇ�͂��̂܂܌��ʉ�ʂɂ�����悤�Ɍ��ʊ֘A�̑S�Ẵf�[�^��M��҂�
    if(MISSION_CheckRecvResult(mission) == TRUE && Intrude_CheckRecvOccupyResult(intcomm) == TRUE){
      return MISSION_ACHIEVE_OK;
    }
    return MISSION_ACHIEVE_NULL;
  }
  return mission->parent_achieve_recv;
}

//==================================================================
/**
 * �~�b�V�����B���񍐂̕Ԏ���M�o�b�t�@���N���A
 *
 * @param   mission		
 */
//==================================================================
void MISSION_ClearAchieveAnswer(MISSION_SYSTEM *mission)
{
  mission->parent_achieve_recv = MISSION_ACHIEVE_NULL;
}

//==================================================================
/**
 * �e����̃~�b�V�����B���񍐂̕Ԏ����Z�b�g
 *
 * @param   mission		
 * @param   achieve		
 */
//==================================================================
void MISSION_SetParentAchieve(MISSION_SYSTEM *mission, MISSION_ACHIEVE achieve)
{
  mission->parent_achieve_recv = achieve;
  OS_TPrintf("�~�b�V�����B���񍐂̕Ԏ� = %d\n", achieve);
}


//--------------------------------------------------------------
/**
 * �~�b�V�������s���Z�b�g
 *
 * @param   mission		
 * @param   fail_netid		
 */
//--------------------------------------------------------------
static void MISSION_SetMissionFail(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int fail_netid)
{
  MISSION_RESULT *result = &mission->result;
  MISSION_DATA *mdata = &mission->data;
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL 
      || result->mission_data.accept_netid != INTRUDE_NETID_NULL){
    OS_TPrintf("MissionFail�͎󂯓�����Ȃ� %d, %d\n", 
      mdata->accept_netid, result->mission_data.accept_netid);
    return;
  }
  
  result->mission_data = *mdata;
  result->mission_fail = TRUE;
  mission->result_send_req = TRUE;
  mission->send_mission_start = _SEND_MISSION_START_NULL;
  OS_TPrintf("�~�b�V�������s���Z�b�g\n");
}

//==================================================================
/**
 * �~�b�V�������ʂ��Z�b�g
 *
 * @param   mission		
 * @param   cp_result		
 */
//==================================================================
void MISSION_SetResult(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_RESULT *cp_result)
{
  MISSION_RESULT *result = &mission->result;
  
  GF_ASSERT(mission->recv_result == FALSE);
  *result = *cp_result;
  mission->recv_result = TRUE;
  //MISSION_ClearMissionEntry(intcomm, mission);
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
BOOL MISSION_CheckRecvResult(const MISSION_SYSTEM *mission)
{
  const MISSION_RESULT *result = &mission->result;
  
  if(mission->recv_result == TRUE 
      && result->mission_data.accept_netid != INTRUDE_NETID_NULL){
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
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL){
    return FALSE;
  }
  if(talk_netid == mdata->target_info.net_id){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ���ݎ��{���̃~�b�V�����^�C�v���擾����
 *
 * @param   mission		
 *
 * @retval  MISSION_TYPE		
 */
//==================================================================
MISSION_TYPE MISSION_GetMissionType(const MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == FALSE){
    return MISSION_TYPE_NONE;
  }
  return mission->data.cdata.type;
}

//==================================================================
/**
 * �~�b�V�������ʂ��瓾����~�b�V�����|�C���g���擾����
 *
 * @param   intcomm		
 * @param   result		
 *
 * @retval  int		�~�b�V�����|�C���g
 */
//==================================================================
s32 MISSION_GetResultPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  const MISSION_RESULT *result = &intcomm->mission.result;
  int i;
  
  if(result->mission_fail == TRUE){
    return 0;
  }
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(result->achieve_netid[i] == GAMEDATA_GetIntrudeMyID(gamedata)){
      return result->mission_data.cdata.reward;
    }
  }
  return 0;
}

//==================================================================
/**
 * �苒���ɑ΂��ă~�b�V�����B���ς݂ɂ���
 *
 * @param   intcomm		
 * @param   mission		
 */
//==================================================================
void MISSION_SetMissionClear(GAMEDATA *gamedata, INTRUDE_COMM_SYS_PTR intcomm, const MISSION_RESULT *result)
{
  OCCUPY_INFO *mine_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  int achieve_version;
  MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gamedata, result->achieve_netid[0]);
  
  achieve_version = intcomm->intrude_status[result->achieve_netid[0]].pm_version;
  achieve_version = Intrude_GetIntrudeRomVersion(achieve_version, MyStatus_GetID(myst));
  
  if(achieve_version == VERSION_WHITE){
    mine_occupy->mlst.mission_clear[result->mission_data.cdata.type] = MISSION_CLEAR_WHITE;
  }
  else if(achieve_version == VERSION_BLACK){
    mine_occupy->mlst.mission_clear[result->mission_data.cdata.type] = MISSION_CLEAR_BLACK;
  }
}

//==================================================================
/**
 * �������~�b�V�����B���҂����ׂ�
 *
 * @param   intcomm		
 * @param   mission		
 *
 * @retval  BOOL		TRUE:�~�b�V�����B���ҁ@FALSE:�B���҂ł͂Ȃ�
 */
//==================================================================
BOOL MISSION_CheckResultMissionMine(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(MISSION_CheckRecvResult(mission) == FALSE){
    return FALSE;
  }
  if(MISSION_GetResultPoint(intcomm, mission) > 0){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �~�b�V�������s(�^�C���A�E�g)���������Ă��邩���ׂ�
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:�^�C���A�E�g�̎��s���������Ă���
 */
//==================================================================
BOOL MISSION_CheckResultTimeout(MISSION_SYSTEM *mission)
{
  const MISSION_RESULT *mresult;

  mresult = MISSION_GetResultData(mission);
  if(mresult == NULL){
    return FALSE;
  }
  
  if(mresult->mission_fail == TRUE){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �~�b�V�����B����A�~�b�V�����̐������ԂɒB�������m�F
 *
 * @param   mission		
 *
 * @retval  BOOL		TRUE:�B�����ă~�b�V�����̐������Ԃ��I�����Ă���
 * @retval  BOOL    FALSE:�����ꂩ�̏����𖞂����Ă��Ȃ�
 */
//==================================================================
BOOL MISSION_CheckSuccessTimeEnd(MISSION_SYSTEM *mission)
{
  return mission->mission_timeend_success;
}

//==============================================================================
//
//  �~�b�V�����f�[�^
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �^�[�Q�b�g�����N���A
 *
 * @param   target		
 */
//--------------------------------------------------------------
static void MISSION_ClearTargetInfo(MISSION_TARGET_INFO *target)
{
  GFL_STD_MemClear(target, sizeof(MISSION_TARGET_INFO));
  target->net_id = INTRUDE_NETID_NULL;
}

//--------------------------------------------------------------
/**
 * �^�[�Q�b�g�����Z�b�g
 *
 * @param   intcomm		
 * @param   target	      	�����
 * @param   net_id		      �^�[�Q�b�g��NetID
 */
//--------------------------------------------------------------
static void MISSION_SetTargetInfo(INTRUDE_COMM_SYS_PTR intcomm, MISSION_TARGET_INFO *target, NetID net_id)
{
  const MYSTATUS *myst;
  
  target->net_id = net_id;
  
  myst = Intrude_GetMyStatus(intcomm, target->net_id);
  MyStatus_CopyNameStrCode(myst, target->name, PERSON_NAME_SIZE + EOM_SIZE);
  target->sex = MyStatus_GetMySex(myst);
}

//==================================================================
/**
 * �~�b�V�����I����⃊�X�g���쐬
 *
 * @param   intcomm		
 * @param   mission		    
 * @param   accept_netid  �~�b�V�����󒍎҂�NetID
 * @param   palace_area		�I�����𓾂�Ώۂ̃p���X�G���A
 */
//==================================================================
void MISSION_MissionList_Create(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, int accept_netid, int palace_area)
{
  MISSION_CHOICE_LIST *list;
  const OCCUPY_INFO *occupy;
  u8 monolith_type, pm_version;
  const MYSTATUS *myst;
  
  list = &mission->list[palace_area];
  if(list->occ == TRUE){
    return; //���ɍ쐬�ς�
  }
  
  occupy = Intrude_GetOccupyInfo(intcomm, palace_area);
  myst = Intrude_GetMyStatus(intcomm, palace_area);
  pm_version = MyStatus_GetRomCode( myst );
  pm_version = Intrude_GetIntrudeRomVersion(pm_version, MyStatus_GetID(myst));
  
  if(pm_version == VERSION_BLACK){
    monolith_type = MONOLITH_TYPE_BLACK;
  }
  else{
    monolith_type = MONOLITH_TYPE_WHITE;
  }
  
  list->occupy = *occupy;
  list->monolith_type = monolith_type;
  list->accept_netid = accept_netid;
  MISSION_SetTargetInfo(intcomm, &list->target_info, palace_area);
  list->occ = TRUE;
}

//==================================================================
/**
 * �~�b�V�����I����⃊�X�g���L�����ǂ����`�F�b�N
 *
 * @param   list		
 *
 * @retval  BOOL		TRUE:�L���@FALSE:����
 */
//==================================================================
BOOL MISSION_MissionList_CheckOcc(const MISSION_CHOICE_LIST *list)
{
  return list->occ;
}

//==================================================================
/**
 * �~�b�V�����f�[�^���s�����ǂ�������
 *
 * @param   mission		
 * @param   mdata		  �~�b�V�����f�[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���Ȃ��B�@FALSE:�s���ł���
 */
//==================================================================
static BOOL MISSION_MissionList_CheckNG(const MISSION_SYSTEM *mission, const MISSION_CONV_DATA *cdat)
{
  int i;
  
  if(cdat->type >= MISSION_TYPE_MAX || cdat->reward > MISSION_REWARD_MAX 
      || cdat->time > MISSION_TIME_MAX || cdat->msg_id_contents >= plc_mis_end
      || cdat->msg_id_contents_monolith >= plc_mism_end){
    return FALSE;
  }
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(cdat->talk_type[i] >= TALK_TYPE_MAX){
      return FALSE;
    }
    if(Intrude_CheckNG_OBJID(cdat->obj_id[i]) == FALSE){
      return FALSE;
    }
  }
  
  for(i = 0; i < 2; i++){
    if(cdat->type == MISSION_TYPE_ATTRIBUTE){
      const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)cdat->data;
      if(Intrude_CheckNG_Item(d_attr->item_no) == FALSE){
        return FALSE;
      }
    }
    else if(cdat->type == MISSION_TYPE_ITEM){
      const MISSION_TYPEDATA_ITEM *d_item = (void*)cdat->data;
      if(Intrude_CheckNG_Item(d_item->item_no) == FALSE){
        return FALSE;
      }
    }
  }
  
  return TRUE;
}

//==================================================================
/**
 * �~�b�V�����G���g���[
 *
 * @param   mission		
 * @param   mdata		  �~�b�V�����f�[�^�ւ̃|�C���^
 * @param   net_id		�G���g���[�҂�NetID
 *
 * @retval  BOOL		  TRUE:�G���g���[�����@FALSE:�G���g���[���s
 */
//==================================================================
BOOL MISSION_SetEntryNew(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_ENTRY_REQ *entry_req, int net_id)
{
  MISSION_DATA *exe_mdata = &mission->data;

  GFL_STD_MemClear(&mission->entry_answer[net_id], sizeof(MISSION_ENTRY_ANSWER));

  if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    OS_TPrintf("NG:���ɑ��̃~�b�V�������N�����Ă���\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG;
    return FALSE;
  }
  
  if(MISSION_MissionList_CheckNG(&intcomm->mission, &entry_req->cdata) == FALSE){
    OS_TPrintf("NG:�~�b�V�������e���s��\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG;
    return FALSE;
  }
  
#if 0 //�^�[�Q�b�g�������I�ɕ\�ɖ߂��悤�ɂ����̂ŕK�v�Ȃ��Ȃ��� 2010.05.04(��)
  if(ZONEDATA_IsPalaceField(intcomm->intrude_status[entry_req->target_info.net_id].zone_id)==TRUE
      || ZONEDATA_IsPalace(intcomm->intrude_status[entry_req->target_info.net_id].zone_id)==TRUE){
    OS_TPrintf("NG:�^�[�Q�b�g�����ɂ���\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG_TARGET_REVERSE;
    return FALSE;
  }
#endif
  
  if(entry_req->cdata.type == MISSION_TYPE_VICTORY || entry_req->cdata.type == MISSION_TYPE_ATTRIBUTE){
    if(GFL_NET_SystemGetConnectNum() != 2 || GFL_NET_GetConnectNum() != 2){
      OS_TPrintf("NG:��l��p�~�b�V���������ڑ��l������l�ł͂Ȃ�\n");
      mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG_TWIN;
      return FALSE;
    }
  }
  
  //���s����~�b�V�����Ƃ��ēo�^
  exe_mdata->cdata = entry_req->cdata;
  exe_mdata->target_info = entry_req->target_info;
  exe_mdata->accept_netid = net_id;
  exe_mdata->palace_area = entry_req->target_info.net_id;
  exe_mdata->monolith_type = entry_req->monolith_type;
  exe_mdata->variable.ready_timer = _MISSION_READY_TIMER;
  exe_mdata->variable.exe_timer = entry_req->cdata.time;
  
  //�Ԏ��Z�b�g
  mission->entry_answer[net_id].mdata = *exe_mdata;
  mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_OK;
  
  //�S���Ɏ��s�����~�b�V�����f�[�^�𑗐M����
  MISSION_Set_DataSendReq(mission);

  mission->start_timer = GFL_RTC_GetTimeBySecond();

  mission->send_mission_start = _SEND_MISSION_START_NULL;
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����G���g���[�̌��ʂ𑗐M
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_Update_EntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(mission->entry_answer[i].result != MISSION_ENTRY_RESULT_NULL){
      if(IntrudeSend_MissionEntryAnswer(intcomm, &mission->entry_answer[i], i) == TRUE){
        GFL_STD_MemClear(&mission->entry_answer[i], sizeof(MISSION_ENTRY_ANSWER));
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �~�b�V�����B���񍐂̕Ԏ��𑗐M
 *
 * @param   mission		
 */
//--------------------------------------------------------------
static void MISSION_Update_AchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(mission->result_mission_achieve[i] != MISSION_ACHIEVE_NULL){
      if(IntrudeSend_MissionAchieveAnswer(intcomm, mission->result_mission_achieve[i], i) == TRUE){
        mission->result_mission_achieve[i] = MISSION_ACHIEVE_NULL;
      }
    }
  }
}

//==================================================================
/**
 * �u�~�b�V������M���܂��v�̕Ԏ����󂯎��
 *
 * @param   mission		
 * @param   entry_answer		�e���瑗���Ă����Ԏ��f�[�^
 */
//==================================================================
void MISSION_SetRecvEntryAnswer(MISSION_SYSTEM *mission, const MISSION_ENTRY_ANSWER *entry_answer)
{
  mission->recv_entry_answer_result = entry_answer->result;
  if(entry_answer->result == MISSION_ENTRY_RESULT_OK){
    mission->data = entry_answer->mdata;
  }
}

//==================================================================
/**
 * �u�~�b�V������M���܂��v�̐e����̕Ԏ����擾
 *
 * @param   mission		
 *
 * @retval  MISSION_ENTRY_RESULT		
 * 
 * ���̊֐����ĂԂ��ƂɕԎ����󂯎��t���O�̓N���A���܂�
 */
//==================================================================
MISSION_ENTRY_RESULT MISSION_GetRecvEntryAnswer(MISSION_SYSTEM *mission)
{
  MISSION_ENTRY_RESULT result;
  
  result = mission->recv_entry_answer_result;
  MISSION_ClearRecvEntryAnswer(mission);
  return result;
}

//==================================================================
/**
 * �u�~�b�V������M���܂��v�̐e����̕Ԏ����󂯎��o�b�t�@���N���A
 *
 * @param   mission		
 */
//==================================================================
void MISSION_ClearRecvEntryAnswer(MISSION_SYSTEM *mission)
{
  mission->recv_entry_answer_result = MISSION_ENTRY_RESULT_NULL;
}

//==================================================================
/**
 * �w��NetID���~�b�V�����^�[�Q�b�g�̑��肩���ׂ�
 *
 * @param   mission		
 * @param   net_id		�����Ώۂ�NetID
 *
 * @retval  BOOL		TRUE:�~�b�V�����^�[�Q�b�g�@FALSE:�~�b�V�����^�[�Q�b�g�ł͂Ȃ�
 */
//==================================================================
BOOL MISSION_CheckMissionTargetNetID(MISSION_SYSTEM *mission, int net_id)
{
  if(MISSION_CheckRecvResult(mission) == TRUE  //���Ɍ��ʂ���M���Ă���̂Ń^�[�Q�b�g�ɂ͂Ȃ蓾�Ȃ�
      || MISSION_RecvCheck(mission) == FALSE){ //�~�b�V�������������Ă��Ȃ�
    return FALSE;
  }
  
  if(mission->data.target_info.net_id == net_id){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �~�b�V�����^�[�Q�b�g��NetID���擾
 *
 * @param   mission		
 *
 * @retval  NetID     �^�[�Q�b�g��NetID(�~�b�V���������� or �G���[���͎�����NetID)
 */
//==================================================================
NetID MISSION_GetMissionTargetNetID(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission)
{
  if(MISSION_RecvCheck(mission) == FALSE){ //�~�b�V�������������Ă��Ȃ�
    return GAMEDATA_GetIntrudeMyID(GameCommSys_GetGameData(intcomm->game_comm));
  }
  return mission->data.target_info.net_id;
}

//==================================================================
/**
 * �������g���~�b�V�����̃^�[�Q�b�g�ɂȂ��Ă��邩�𒲂ׂ�
 *
 * @param   intcomm		
 *
 * @retval  BOOL		  TRUE:�������^�[�Q�b�g�ɂȂ��Ă���
 * @retval  BOOL      FALSE:�����̓^�[�Q�b�g�ł͂Ȃ�
 */
//==================================================================
BOOL MISSION_CheckTargetIsMine(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);

  if(MISSION_CheckMissionTargetNetID(&intcomm->mission, GAMEDATA_GetIntrudeMyID(gamedata)) == TRUE){
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * ����STRCODE��WORDSET����
 *
 * @param   wordset		
 * @param   bufID		
 * @param   code		        
 * @param   str_length		  STRCODE�̒���
 * @param   temp_heap_id		�e���|�����Ŏg�p����q�[�vID
 */
//--------------------------------------------------------------
static void _Wordset_Strcode(WORDSET *wordset, u32 bufID, const STRCODE *code, int str_length, HEAPID temp_heap_id, int sex)
{
  STRBUF *strbuf = 	GFL_STR_CreateBuffer(str_length, temp_heap_id);

	GFL_STR_SetStringCodeOrderLength(strbuf, code, str_length);
  WORDSET_RegisterWord(wordset, bufID, strbuf, sex, TRUE, PM_LANG);

  GFL_STR_DeleteBuffer(strbuf);
}

//==================================================================
/**
 * �~�b�V�����^�[�Q�b�g�̖��O��Wordset����
 *
 * @param   wordset		
 * @param   bufID		
 * @param   target		
 * @param   temp_heap_id		
 */
//==================================================================
void MISSIONDATA_WordsetTargetName(WORDSET *wordset, u32 bufID, const MISSION_TARGET_INFO *target, HEAPID temp_heap_id)
{
  _Wordset_Strcode(wordset, bufID, target->name, 
    PERSON_NAME_SIZE + EOM_SIZE, temp_heap_id, target->sex);
}

//==================================================================
/**
 * �~�b�V�����f�[�^�ɑΉ��������b�Z�[�W��WORDSET����
 *
 * @param   intcomm		
 * @param   mdata		        �~�b�V�����f�[�^�ւ̃|�C���^
 * @param   wordset		      
 * @param   temp_heap_id		�e���|�����Ŏg�p����q�[�vID
 */
//==================================================================
void MISSIONDATA_Wordset(const MISSION_CONV_DATA *cdata, const MISSION_TARGET_INFO *target, WORDSET *wordset, HEAPID temp_heap_id)
{
  switch(cdata->type){
  case MISSION_TYPE_VICTORY:     //����(LV)
    {
      const MISSION_TYPEDATA_VICTORY *d_vic = (void*)cdata->data;
      
      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      
      WORDSET_RegisterNumber(wordset, 1, d_vic->battle_level, 3, 
        STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    }
    break;
  case MISSION_TYPE_SKILL:       //�Z
    {
      const MISSION_TYPEDATA_SKILL *d_skill = (void*)cdata->data;

      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      WORDSET_RegisterGPowerName( wordset, 1, d_skill->gpower_id );
    }
    break;
  case MISSION_TYPE_BASIC:       //��b
    {
      const MISSION_TYPEDATA_BASIC *d_bas = (void*)cdata->data;
      
      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      WORDSET_RegisterGPowerName( wordset, 1, d_bas->gpower_id );
    }
    break;
  case MISSION_TYPE_ATTRIBUTE:   //����
    {
      const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)cdata->data;

      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
      WORDSET_RegisterItemName( wordset, 1, d_attr->item_no );
      WORDSET_RegisterNumber( 
        wordset, 2, d_attr->price, 5, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    }
    break;
  case MISSION_TYPE_ITEM:        //����
    {
      const MISSION_TYPEDATA_ITEM *d_item = (void*)cdata->data;
      
      WORDSET_RegisterItemName( wordset, 0, d_item->item_no );
      
      MISSIONDATA_WordsetTargetName(wordset, 1, target, temp_heap_id);
    }
    break;
  case MISSION_TYPE_PERSONALITY: //���i
    {
      const MISSION_TYPEDATA_PERSONALITY *d_per = (void*)cdata->data;
      
      MISSIONDATA_WordsetTargetName(wordset, 0, target, temp_heap_id);
    }
    break;
  }
  
  //��������
  WORDSET_RegisterNumber(wordset, 2, cdata->time, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  //��V
  WORDSET_RegisterNumber(
    wordset, 3, cdata->reward, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
}

//==================================================================
/**
 * �苒��񂩂�~�b�V�������X�g���쐬����
 *
 * @param   occupy		
 */
//==================================================================
void MISSION_LIST_Create(OCCUPY_INFO *occupy)
{
  int mission_type;
  
  for(mission_type = 0; mission_type < MISSION_LIST_MAX; mission_type++){
    occupy->mlst.mission_clear[mission_type] = MISSION_CLEAR_NONE;
    MISSION_LIST_Create_Type(occupy, mission_type);
  }
}

//==================================================================
/**
 * �苒��񂩂�~�b�V�������X�g���w��^�C�v�ɑ΂��Ē��I���s��
 *
 * @param   occupy		
 * @param   mission_type		
 */
//==================================================================
void MISSION_LIST_Create_Type(OCCUPY_INFO *occupy, MISSION_TYPE mission_type)
{
  int no;
  int palace_level;
  int my_version_bit;
  
#if (PM_VERSION == VERSION_WHITE)
  my_version_bit = MISSION_DATA_VERSION_WHITE;
#elif (PM_VERSION == VERSION_BLACK)
  my_version_bit = MISSION_DATA_VERSION_BLACK;
#else
  my_version_bit = MISSION_DATA_VERSION_NEXT;
#endif

  palace_level = occupy->white_level + occupy->black_level;
  
  for(no = 0; no < NELEMS(MissionConvDataListParam); no++){
    if(MissionConvDataListParam[no].type == mission_type
        && MissionConvDataListParam[no].level <= palace_level){
      if(MissionConvDataListParam[no].version_bit & my_version_bit){
        if(MissionConvDataListParam[no].odds == 100 
            || MissionConvDataListParam[no].odds >= GFUser_GetPublicRand(100+1)){
          occupy->mlst.mission_no[mission_type] = no;
          return;
        }
      }
    }
  }
  GF_ASSERT_MSG(0, "type=%d", mission_type);
}

//==================================================================
/**
 * �~�b�V������S�Đ��e���Ă���Ȃ�΃~�b�V�������X�g��S�č쐬���Ȃ���
 *
 * @param   occupy		
 * @param   white_num   �S�苒�B�����F���̐�
 * @param   black_num   �S�苒�B�����F���̐�
 *
 * @retval  BOOL		TRUE:�쐬���Ȃ�����
 */
//==================================================================
BOOL MISSION_LIST_Create_Complete(OCCUPY_INFO *occupy, u8 *white_num, u8 *black_num)
{
  int mission_type;
  
  for(mission_type = 0; mission_type < MISSION_LIST_MAX; mission_type++){
    if(occupy->mlst.mission_clear[mission_type] == MISSION_CLEAR_NONE){
      return FALSE;
    }
    else if(occupy->mlst.mission_clear[mission_type] == MISSION_CLEAR_WHITE){
      (*white_num)++;
    }
    else{
      (*black_num)++;
    }
  }
  MISSION_LIST_Create(occupy);
  return TRUE;
}
