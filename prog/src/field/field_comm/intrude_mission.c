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
//  �萔��`
//==============================================================================
///�~�b�V�������s�܂ł̃^�C��
#define MISSION_TIME_OVER     (60/2 * 60)   // 60/2 = �t�B�[���h��1/30�ׁ̈B


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void MISSION_SendUpdate(INTRUDE_COMM_SYS_PTR intcomm, MISSION_SYSTEM *mission);
static void MISSION_SetMissionFail(MISSION_SYSTEM *mission, int fail_netid);
static int _TragetNetID_Choice(INTRUDE_COMM_SYS_PTR intcomm, int accept_netid);



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
  GFL_STD_MemClear(mission, sizeof(MISSION_SYSTEM));
  mission->data.mission_no = MISSION_NO_NULL;
  mission->result.mission_data.mission_no = MISSION_NO_NULL;
  mission->data.accept_netid = INTRUDE_NETID_NULL;
  
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

  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    mission->list[i].accept_netid = INTRUDE_NETID_NULL;
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
      && mission->result.mission_data.mission_no == MISSION_NO_NULL
      && mission->result.mission_data.accept_netid == INTRUDE_NETID_NULL){
    mission->timer++;
    if(mission->timer > MISSION_TIME_OVER){
      GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
      MISSION_SetMissionFail(mission, GAMEDATA_GetIntrudeMyID(gamedata));
    }
  }
  
  //���M���N�G�X�g������Α��M
  MISSION_SendUpdate(intcomm, mission);
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
  
  if(mdata->mission_no != MISSION_NO_NULL || mdata->accept_netid != INTRUDE_NETID_NULL){
    return FALSE;
  }
  mdata->mission_no = GFUser_GetPublicRand0(MISSION_NO_MAX);
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
    if(i != accept_netid && intcomm->recv_profile & (1 << i)){
      if(target_no == 0){
        return i;
      }
    }
  }
  
  GF_ASSERT_MSG(0, "connect_num = %d, accept_netid = %d, recv_profile = %d\n", 
    connect_num, accept_netid, intcomm->recv_profile);
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
  MISSION_CHOICE_LIST *mlist = &mission->list[list->palace_area];
  
  if(mlist->accept_netid != INTRUDE_NETID_NULL){
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
 */
//==================================================================
void MISSION_SetMissionData(MISSION_SYSTEM *mission, const MISSION_DATA *src)
{
  MISSION_DATA *mdata = &mission->data;
  
  //�e�̏ꍇ�A����misison_no�̓Z�b�g����Ă���̂Ŕ���̑O�Ɏ�M�t���O���Z�b�g
  mission->parent_data_recv = TRUE;
  mission->timer = 0;
  if(mdata->mission_no != MISSION_NO_NULL || mdata->accept_netid != INTRUDE_NETID_NULL){
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
  
  if(mdata->mission_no == MISSION_NO_NULL || mdata->accept_netid == INTRUDE_NETID_NULL || mission->parent_data_recv == FALSE){
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
  
  if(mdata->accept_netid == INTRUDE_NETID_NULL || mdata->mission_no == MISSION_NO_NULL || mdata->mission_no >= MISSION_NO_MAX){
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
  
  if(result->mission_data.accept_netid == INTRUDE_NETID_NULL
      || result->mission_data.mission_no == MISSION_NO_FAIL
      || result->mission_data.mission_no == MISSION_NO_NULL 
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
  if(result->mission_data.accept_netid != INTRUDE_NETID_NULL || result->mission_data.mission_no != MISSION_NO_NULL || mission->parent_result_recv == TRUE){
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
  MISSION_DATA *data = &mission->data;
  
  if(data->accept_netid == INTRUDE_NETID_NULL || data->mission_no == MISSION_NO_NULL || result->mission_data.mission_no != MISSION_NO_NULL){
    OS_TPrintf("MissionFail�͎󂯓�����Ȃ� %d, %d\n", 
      data->mission_no, result->mission_data.mission_no);
    return;
  }
  
  result->mission_data = *data;
  result->mission_data.mission_no = MISSION_NO_FAIL;
  result->achieve_netid = fail_netid;
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
  
  if(mission->parent_result_recv == TRUE && result->mission_data.accept_netid != INTRUDE_NETID_NULL && result->mission_data.mission_no != MISSION_NO_NULL){
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
  
  if(mdata->mission_no == MISSION_NO_NULL || mdata->accept_netid == INTRUDE_NETID_NULL){
    return FALSE;
  }
  if(talk_netid == mdata->target_netid){
    return TRUE;
  }
  return FALSE;
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
s32 MISSION_GetPoint(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_RESULT *result)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  s32 point;
  
  if(result->mission_data.mission_no == MISSION_NO_FAIL){
    return 0;
  }
  
  if(result->achieve_netid == GAMEDATA_GetIntrudeMyID(gamedata)){
    point = 100;
  }
  else{
    point = 50;
  }
  return point;
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
  
  point = MISSION_GetPoint(intcomm, result);
  if(point > 0){
    OCCUPY_INFO *mine_occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
    PALACE_TOWN_RESULT town_result;
    
    //�~�b�V�����|�C���g���Z
    mine_occupy->intrude_point += point;
    //�X�̐苒��񔽉f
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
      if(cdata[i].odds == 100 || cdata[i].odds <= GFUser_GetPublicRand(100)){
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
  
  switch(mdata->cdata.type){
  case MISSION_TYPE_VICTORY:     ///<����(LV)
    MISSION_SetTargetInfo(intcomm, &exwork->victory.target_info, accept_netid);
    break;
  case MISSION_TYPE_SKILL:       ///<�Z
    break;
  case MISSION_TYPE_BASIC:       ///<��b
    MISSION_SetTargetInfo(intcomm, &exwork->victory.target_info, accept_netid);
    break;
  case MISSION_TYPE_SIZE:        ///<�傫��
  case MISSION_TYPE_ATTRIBUTE:   ///<����
    break;
  case MISSION_TYPE_ITEM:        ///<����
    MISSION_SetTargetInfo(intcomm, &exwork->victory.target_info, accept_netid);
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
  if(list->accept_netid != INTRUDE_NETID_NULL){
    return; //���ɍ쐬�ς�
  }
  list->accept_netid = accept_netid;
  list->palace_area = palace_area;
  
  occupy = Intrude_GetOccupyInfo(intcomm, palace_area);
  palace_level = occupy->intrude_level + 1;//�~�b�V�����f�[�^��1origin�ׁ̈A���킹��1origin��
  
  cdata = MISSIONDATA_Load();
  md_array_max = MISSIONDATA_GetArrayMax();
  
  occupy = Intrude_GetOccupyInfo(intcomm, palace_area);
  
  for(type = 0; type < MISSION_TYPE_MAX; type++){
    MISSIONDATA_Choice(cdata, &list->md[type].cdata, md_array_max, type, palace_level);
    MISSIONDATA_ExtraParamSet(intcomm, &list->md[type], accept_netid);
  }

  MISSIONDATA_Unload(cdata);
}

//==================================================================
/**
 * �~�b�V�����I����⃊�X�g���L�����ǂ����`�F�b�N
 *
 * @param   mission		
 * @param   palace_area		
 *
 * @retval  BOOL		TRUE:�L���@FALSE:����
 */
//==================================================================
BOOL MISSION_MissionList_CheckOcc(MISSION_SYSTEM *mission, int palace_area)
{
  if(mission->list[palace_area].accept_netid != INTRUDE_NETID_NULL){
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
      
      _Wordset_Strcode(wordset, 0, mdata->exwork.victory.target_info.name, 
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
      
      _Wordset_Strcode(wordset, 0, mdata->exwork.basic.target_info.name, 
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
      
      WORDSET_RegisterPlaceName( wordset, 0, d_item->item_no );
      
      _Wordset_Strcode(wordset, 1, mdata->exwork.item.target_info.name, 
        PERSON_NAME_SIZE + EOM_SIZE, temp_heap_id);
    }
    break;
  case MISSION_TYPE_OCCUR:       //����(�G���J�E���g)
  case MISSION_TYPE_PERSONALITY: //���i
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
