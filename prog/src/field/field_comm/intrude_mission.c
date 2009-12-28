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
#include "intrude_main.h"
#include "intrude_work.h"
#include "print/wordset.h"

#include "mission.naix"



//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_SetMissionFail(MISSION_SYSTEM *mission, int fail_netid);
static int _TragetNetID_Choice(INTRUDE_COMM_SYS_PTR intcomm, int accept_netid);
static void MISSION_Update_EntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_Update_AchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_ClearTargetInfo(MISSION_TARGET_INFO *target);
static s32 _GetMissionTime(MISSION_SYSTEM *mission);



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
    for(type = 0; type < MISSION_TYPE_MAX; type++){
      mission->list[i].md[type].accept_netid = INTRUDE_NETID_NULL;
    }
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
      && MISSION_RecvCheck(mission) == TRUE 
      && mission->result.mission_data.accept_netid == INTRUDE_NETID_NULL){
    if(_GetMissionTime(mission) > mission->data.cdata.time){
      GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
      MISSION_SetMissionFail(mission, GAMEDATA_GetIntrudeMyID(gamedata));
    }
  }
  
  //���M���N�G�X�g������Α��M
  MISSION_Update_EntryAnswer(intcomm, mission);
  MISSION_Update_AchieveAnswer(intcomm, mission);
  MISSION_SendUpdate(intcomm, mission);
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
    ret_timer = mission->data.cdata.time;
    ret_timer -= _GetMissionTime(mission);
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
  MISSION_DATA *mdata = &mission->data;
  
  if(mdata->accept_netid != INTRUDE_NETID_NULL){
    return FALSE;
  }
  mdata->accept_netid = accept_netid;
  mdata->monolith_type = req->monolith_type;
  mdata->zone_id = req->zone_id;
  mdata->target_netid = _TragetNetID_Choice(intcomm, accept_netid);
  
  return TRUE;
}

//--------------------------------------------------------------
/**
 * �~�b�V�����̃^�[�Q�b�g�ΏۂƂȂ�NetID�����肷��
 *
 * @param   intcomm		
 * @param   accept_netid		�~�b�V�����󒍎҂�NetID
 *
 * @retval  int		�^�[�Q�b�g�Ώێ҂�NetID
 */
//--------------------------------------------------------------
static int _TragetNetID_Choice(INTRUDE_COMM_SYS_PTR intcomm, int accept_netid)
{
  int i, target_no, connect_num;

  connect_num = GFL_NET_GetConnectNum();
  target_no = GFUser_GetPublicRand0(connect_num - 1); // -1 = �󒍎ҕ�
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(i != accept_netid && (intcomm->recv_profile & (1 << i))){
      if(target_no == 0){
        return i;
      }
      target_no--;
    }
  }
  
  GF_ASSERT_MSG(0, "connect_num = %d, accept_netid = %d, recv_profile = %d, target_no=%d\n", 
    connect_num, accept_netid, intcomm->recv_profile, target_no);
  return 0;
}

//==================================================================
/**
 * �~�b�V�������X�g���Z�b�g
 *
 * @param   mission		�����
 * @param   MISSION_CHOICE_LIST		    �Z�b�g����f�[�^
 */
//==================================================================
void MISSION_SetMissionList(MISSION_SYSTEM *mission, const MISSION_CHOICE_LIST *list)
{
  MISSION_CHOICE_LIST *mlist = &mission->list[list->md[0].palace_area];
  
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
BOOL MISSION_SetMissionData(MISSION_SYSTEM *mission, const MISSION_DATA *src)
{
  MISSION_DATA *mdata = &mission->data;
  BOOL new_mission = FALSE;
  
  if(mission->parent_data_recv == FALSE && src->accept_netid != INTRUDE_NETID_NULL){
    new_mission = TRUE;
    mission->parent_data_recv = TRUE;
  }
  
  //�e�̏ꍇ�A����misison_no�̓Z�b�g����Ă���̂Ŕ���̑O�Ɏ�M�t���O���Z�b�g
  mission->start_timer = GFL_RTC_GetTimeBySecond();
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
 * �~�b�V�����f�[�^����Ή������~�b�V�������b�Z�[�WID���擾���� ��check�@�폜���
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
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL){
    return msg_invasion_mission000;
  }
  msg_id = mdata->cdata.mission_no * 2 + mdata->monolith_type;
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
  
  if(result->mission_data.accept_netid == INTRUDE_NETID_NULL
      || result->mission_fail == TRUE){
    return msg_invasion_mission_sys002;
  }
  
  if(my_netid == result->achieve_netid[0]){
    return msg_invasion_mission001_02;
  }
  return msg_invasion_mission001_03;
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
  int i;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(result->achieve_netid[i] == INTRUDE_NETID_NULL && mdata->cdata.reward[i] > 0){
      return i;
    }
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
  if(GFL_STD_MemComp(data, mdata, sizeof(MISSION_DATA)) != 0){
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
  mission->result_send_req = TRUE;  //��check ���C���ŐȂ��S�Ė��܂�or�������Ԃő��M����悤�ɂ���
  
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
MISSION_ACHIEVE MISSION_GetAchieveAnswer(MISSION_SYSTEM *mission)
{
  MISSION_ACHIEVE achieve = mission->parent_achieve_recv;
  
  mission->parent_achieve_recv = MISSION_ACHIEVE_NULL;
  return achieve;
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
static void MISSION_SetMissionFail(MISSION_SYSTEM *mission, int fail_netid)
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
void MISSION_SetResult(MISSION_SYSTEM *mission, const MISSION_RESULT *cp_result)
{
  MISSION_RESULT *result = &mission->result;
  
  GF_ASSERT(mission->parent_achieve_recv == FALSE);
  *result = *cp_result;
  mission->parent_achieve_recv = TRUE;
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
  
  if(mission->parent_achieve_recv == TRUE 
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
  if(talk_netid == mdata->target_netid){
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
      return result->mission_data.cdata.reward[i];
    }
  }
  return 0;
}

//==================================================================
/**
 * �~�b�V�������ʂ��瓾����~�b�V�����|�C���g�A�苒�|�C���^�����Z����
 *
 * @param   intcomm		
 * @param   mission		
 *
 * @retval  BOOL		TRUE:���Z�����B�@FALSE:�|�C���g�͔������Ȃ�����
 */
//==================================================================
BOOL MISSION_AddPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  const MISSION_RESULT *result = &intcomm->mission.result;
  int point;
  
  point = MISSION_GetResultPoint(intcomm, mission);
  if(point > 0){
    OCCUPY_INFO *mine_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
//    PALACE_TOWN_RESULT town_result;
    
    //�~�b�V�����|�C���g���Z
    mine_occupy->intrude_point += point;
    //�X�̐苒��񔽉f
  #if 0
    if(Intrude_SearchPalaceTown(result->mission_data.zone_id, &town_result) == TRUE){
      if(result->mission_data.monolith_type == MONOLITH_TYPE_WHITE){
        mine_occupy->town.town_occupy[town_result.tblno] = OCCUPY_TOWN_WHITE;
      }
      else{
        mine_occupy->town.town_occupy[town_result.tblno] = OCCUPY_TOWN_BLACK;
      }
    }
    else{
      GF_ASSERT_MSG(0, "zone_id = %d\n", result->mission_data.zone_id);
    }
  #else
    if(result->mission_data.monolith_type == MONOLITH_TYPE_WHITE){
      mine_occupy->town.town_occupy[result->mission_data.cdata.type] = OCCUPY_TOWN_WHITE;
    }
    else{
      mine_occupy->town.town_occupy[result->mission_data.cdata.type] = OCCUPY_TOWN_BLACK;
    }
  #endif
    return TRUE;
  }
  return FALSE;
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


//==============================================================================
//
//  �~�b�V�����f�[�^
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �~�b�V�����f�[�^�����[�h
 *
 * @retval  MISSION_CONV_DATA *		�~�b�V�����f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static MISSION_CONV_DATA * MISSIONDATA_Load(void)
{
  return GFL_ARC_LoadDataAlloc(ARCID_MISSION, NARC_mission_mission_data_bin, HEAPID_WORLD);
}

//--------------------------------------------------------------
/**
 * �~�b�V�����f�[�^���A�����[�h
 *
 * @param   md		�~�b�V�����f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void MISSIONDATA_Unload(MISSION_CONV_DATA *md)
{
  GFL_HEAP_FreeMemory(md);
}

//--------------------------------------------------------------
/**
 * �~�b�V�����f�[�^�̗v�f�����擾
 *
 * @retval  u32		�v�f��
 */
//--------------------------------------------------------------
static u32 MISSIONDATA_GetArrayMax(void)
{
  return GFL_ARC_GetDataSize(ARCID_MISSION, NARC_mission_mission_data_bin);
}

//--------------------------------------------------------------
/**
 * �~�b�V�����f�[�^���Y�������ɍ������̂�I�����ăo�b�t�@�Ɋi�[����
 *
 * @param   md		        �~�b�V�����f�[�^�ւ̃|�C���^
 * @param   dest_md		    �Y�������ɂ������f�[�^�̑����
 * @param   md_max		    �~�b�V�����f�[�^�v�f��
 * @param   choice_type		�I���^�C�v
 * @param   level		      �p���X���x��
 *
 * @retval  BOOL		TRUE:�����ɓ��Ă͂܂���̂��������B�@FALSE:��������
 */
//--------------------------------------------------------------
static BOOL MISSIONDATA_Choice(const MISSION_CONV_DATA *cdata, MISSION_CONV_DATA *dest_md, int md_max, MISSION_TYPE choice_type, int level)
{
  int i;
  
  for(i = 0; i < md_max; i++){
    if(cdata[i].type == choice_type && cdata[i].level <= level){
      if(cdata[i].odds == 100 || cdata[i].odds <= GFUser_GetPublicRand(100+1)){
        GFL_STD_MemCopy(&cdata[i], dest_md, sizeof(MISSION_CONV_DATA));
        return TRUE;
      }
    }
  }
  
  GF_ASSERT_MSG(0, "type=%d, level=%d\n", choice_type, level);
  GFL_STD_MemCopy(&cdata[0], dest_md, sizeof(MISSION_CONV_DATA));
  return FALSE;
}

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
 * @param   accept_netid		�~�b�V�����󒍎҂�NetID
 */
//--------------------------------------------------------------
static void MISSION_SetTargetInfo(INTRUDE_COMM_SYS_PTR intcomm, MISSION_TARGET_INFO *target, int accept_netid)
{
  const MYSTATUS *myst;
  
  target->net_id = _TragetNetID_Choice(intcomm, accept_netid);
  
  myst = Intrude_GetMyStatus(intcomm, target->net_id);
  MyStatus_CopyNameStrCode(myst, target->name, PERSON_NAME_SIZE + EOM_SIZE);
}

//--------------------------------------------------------------
/**
 * �~�b�V�����f�[�^���̊g���p�����[�^���Z�b�g����
 *
 * @param   mdata		        
 * @param   accept_netid		�~�b�V�����󒍎҂�NetID
 */
//--------------------------------------------------------------
static void MISSIONDATA_ExtraParamSet(INTRUDE_COMM_SYS_PTR intcomm, MISSION_DATA *mdata, int accept_netid)
{
  MISSION_TYPE_WORK *exwork = &mdata->exwork;
  
  MISSION_ClearTargetInfo(&mdata->target_info);
  switch(mdata->cdata.type){
  case MISSION_TYPE_VICTORY:     ///<����(LV)
    MISSION_SetTargetInfo(intcomm, &mdata->target_info, accept_netid);
    break;
  case MISSION_TYPE_SKILL:       ///<�Z
    break;
  case MISSION_TYPE_BASIC:       ///<��b
    MISSION_SetTargetInfo(intcomm, &mdata->target_info, accept_netid);
    break;
  case MISSION_TYPE_SIZE:        ///<�傫��
  case MISSION_TYPE_ATTRIBUTE:   ///<����
    break;
  case MISSION_TYPE_ITEM:        ///<����
    MISSION_SetTargetInfo(intcomm, &mdata->target_info, accept_netid);
    break;
  case MISSION_TYPE_OCCUR:       ///<����(�G���J�E���g)
  case MISSION_TYPE_PERSONALITY: ///<���i
    break;
  }
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
  MISSION_TYPE type;
  MISSION_CONV_DATA *cdata;
  u32 md_array_max;
  int palace_level;
  const OCCUPY_INFO *occupy;
  
  list = &mission->list[palace_area];
  if(list->occ == TRUE){
    return; //���ɍ쐬�ς�
  }
  
  occupy = Intrude_GetOccupyInfo(intcomm, palace_area);
  palace_level = occupy->intrude_level;
  
  cdata = MISSIONDATA_Load();
  md_array_max = MISSIONDATA_GetArrayMax();
  
  occupy = Intrude_GetOccupyInfo(intcomm, palace_area);
  
  for(type = 0; type < MISSION_TYPE_MAX; type++){
    MISSIONDATA_Choice(cdata, &list->md[type].cdata, md_array_max, type, palace_level);
    MISSIONDATA_ExtraParamSet(intcomm, &list->md[type], accept_netid);
    list->md[type].accept_netid = INTRUDE_NETID_NULL;
    list->md[type].palace_area = palace_area;
  }

  MISSIONDATA_Unload(cdata);
  
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
 * �~�b�V�����f�[�^���e�̃~�b�V�����I����⃊�X�g�ɑ��݂��Ă��邩���ׂ�
 *
 * @param   mission		
 * @param   mdata		  �~�b�V�����f�[�^�ւ̃|�C���^
 *
 * @retval  BOOL		TRUE:���݂��Ă���B�@FALSE:���݂��Ă��Ȃ�
 */
//==================================================================
BOOL MISSION_MissionList_CheckAgree(const MISSION_SYSTEM *mission, const MISSION_DATA *mdata)
{
  const MISSION_CHOICE_LIST *list;
  MISSION_TYPE type;
  
  if(mdata->accept_netid != INTRUDE_NETID_NULL){
    GF_ASSERT(0);
    return FALSE; //�܂����s����Ă��Ȃ��~�b�V�����Ȃ̂�����NETID��NULL�̂͂�
  }
  
  list = &mission->list[mdata->palace_area];
  for(type = 0; type < MISSION_TYPE_MAX; type++){
    if(GFL_STD_MemComp(mdata, &list->md[type], sizeof(MISSION_DATA)) == 0){
      return TRUE;
    }
  }
  return FALSE;
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
BOOL MISSION_SetEntryNew(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission, const MISSION_DATA *mdata, int net_id)
{
  MISSION_DATA *exe_mdata = &mission->data;

  GFL_STD_MemClear(&mission->entry_answer[net_id], sizeof(MISSION_ENTRY_ANSWER));

  if(MISSION_RecvCheck(&intcomm->mission) == TRUE){
    OS_TPrintf("NG:���ɑ��̃~�b�V�������N�����Ă���\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG;
    return FALSE;
  }
  
  if(MISSION_MissionList_CheckAgree(&intcomm->mission, mdata) == FALSE){
    OS_TPrintf("NG:�e�̎��I����⃊�X�g�ɑ��݂��Ȃ�\n");
    mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_NG;
    return FALSE;
  }
  
  //���s����~�b�V�����Ƃ��ēo�^
  *exe_mdata = *mdata;
  exe_mdata->accept_netid = net_id;

  //�Ԏ��Z�b�g
  mission->entry_answer[net_id].mdata = *exe_mdata;
  mission->entry_answer[net_id].result = MISSION_ENTRY_RESULT_OK;
  
  //�S���Ɏ��s�����~�b�V�����f�[�^�𑗐M����
  MISSION_Set_DataSendReq(mission);
  
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
static void _Wordset_Strcode(WORDSET *wordset, u32 bufID, const STRCODE *code, int str_length, HEAPID temp_heap_id)
{
  STRBUF *strbuf = 	GFL_STR_CreateBuffer(str_length, temp_heap_id);

	GFL_STR_SetStringCodeOrderLength(strbuf, code, str_length);
  WORDSET_RegisterWord(wordset, bufID, strbuf, 0, TRUE, PM_LANG);

  GFL_STR_DeleteBuffer(strbuf);
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
void MISSIONDATA_Wordset(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_DATA *mdata, WORDSET *wordset, HEAPID temp_heap_id)
{
  switch(mdata->cdata.type){
  case MISSION_TYPE_VICTORY:     //����(LV)
    {
      const MISSION_TYPEDATA_VICTORY *d_vic = (void*)mdata->cdata.data;
      
      _Wordset_Strcode(wordset, 0, mdata->target_info.name, 
        PERSON_NAME_SIZE + EOM_SIZE, temp_heap_id);
      
      WORDSET_RegisterNumber(wordset, 1, d_vic->battle_level, 3, 
        STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    }
    break;
  case MISSION_TYPE_SKILL:       //�Z
    break;
  case MISSION_TYPE_BASIC:       //��b
    {
      const MISSION_TYPEDATA_BASIC *d_bas = (void*)mdata->cdata.data;
      
      _Wordset_Strcode(wordset, 0, mdata->target_info.name, 
        PERSON_NAME_SIZE + EOM_SIZE, temp_heap_id);
    }
    break;
  case MISSION_TYPE_SIZE:        //�傫��
    break;
  case MISSION_TYPE_ATTRIBUTE:   //����
    {
      const MISSION_TYPEDATA_ATTRIBUTE *d_attr = (void*)mdata->cdata.data;
      
      WORDSET_RegisterPlaceName( wordset, 0, d_attr->zone_id );
    }
    break;
  case MISSION_TYPE_ITEM:        //����
    {
      const MISSION_TYPEDATA_ITEM *d_item = (void*)mdata->cdata.data;
      
      WORDSET_RegisterItemName( wordset, 0, d_item->item_no );
      
      _Wordset_Strcode(wordset, 1, mdata->target_info.name, 
        PERSON_NAME_SIZE + EOM_SIZE, temp_heap_id);
    }
    break;
  case MISSION_TYPE_OCCUR:       //����(�G���J�E���g)
    break;
  case MISSION_TYPE_PERSONALITY: //���i
    {
      const MISSION_TYPEDATA_PERSONALITY *d_per = (void*)mdata->cdata.data;
      
      WORDSET_RegisterNumber(wordset, 0, d_per->num, 3, 
        STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    }
    break;
  }
  
  //��������
  WORDSET_RegisterNumber(wordset, 2, mdata->cdata.time, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  //��V1�ʁ`4��
  WORDSET_RegisterNumber(
    wordset, 3, mdata->cdata.reward[0], 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_RegisterNumber(
    wordset, 4, mdata->cdata.reward[1], 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_RegisterNumber(
    wordset, 5, mdata->cdata.reward[2], 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_RegisterNumber(
    wordset, 6, mdata->cdata.reward[3], 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
}
