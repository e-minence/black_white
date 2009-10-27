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
#include "fieldmap/zone_id.h"
#include "field/zonedata.h"
#include "intrude_field.h"
#include "field/field_const.h"
#include "intrude_mission.h"


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
static void Intrude_ConvertPlayerPos(INTRUDE_COMM_SYS_PTR intcomm, const INTRUDE_STATUS *mine, INTRUDE_STATUS *target);
static int Intrude_GetPalaceOffsetNo(const INTRUDE_COMM_SYS_PTR intcomm, int palace_area);

//==============================================================================
//  �f�[�^
//==============================================================================
///�X�̍��W�f�[�^   ���e�[�u�����̑�����������K��PALACE_TOWN_DATA_MAX�̐����ς��邱�ƁI
const PALACE_TOWN_DATA PalaceTownData[] = {
  {
    ZONE_ID_C04, 
    ZONE_ID_PLC04, 
    7*8, 3*8,
    {
      6936/16, 7448/16, 16,
      6856/16, 7000/16, 16,
      6520/16, 6984/16, 16,
      6488/16, 7368/16, 16,
    },
  },
  {
    ZONE_ID_C05, 
    ZONE_ID_PLC05, 
    0x10*8, 3*8,
    {
      3752/16,6952/16, 0, 
      3320/16,6920/16, 0, 
      2904/16,6920/16, 0, 
      2952/16,6632/16, 0, 
    },
  },
  {
    ZONE_ID_C06, 
    ZONE_ID_PLC06, 
    0x19*8, 3*8,
    {
      1656/16,4888/16, 0, 
      1688/16,4760/16, 0, 
      1224/16,4824/16, 0, 
      1304/16,4984/16, 0, 
    },
  },
  {
    ZONE_ID_C07, 
    ZONE_ID_PLC07, 
    2*8, 0xa*8,
    {
      2792/16, 3112/16, 32,
      2792/16, 2888/16, 32,
      2984/16, 3208/16, 0, 
      3336/16, 3192/16, 32,
    },
  },
  {
    ZONE_ID_C08, 
    ZONE_ID_PLC08, 
    0x1e*8, 0xa*8,
    {
      6632/16, 2856/16, 33,
      6952/16, 2792/16, 33,
      6824/16, 2472/16, 33,
      6472/16, 2472/16, 33,
    },
  },
  {
    ZONE_ID_T03, 
    ZONE_ID_PLT03, 
    7*8, 0x11*8,
    {
      10552/16, 2824/16, 80,
      10488/16,3000/16, 0, 
      10712/16,2728/16, 0, 
      10584/16,2696/16, 0, 
    },
  },
  {
    ZONE_ID_T04, 
    ZONE_ID_PLT04, 
    0x10*8, 0x11*8,
    {
      11640/16, 4824/16, 16,
      11816/16,4888/16, 0, 
      12040/16,4840/16, 0, 
      11688/16,4936/16, 0, 
    },
  },
  {
    ZONE_ID_C02,    //��check C10�������̂łƂ肠����C02
    ZONE_ID_PLC10, 
    0x19*8, 0x11*8,
    {
      10456/16,9448/16, 0, 
      10104/16,9448/16, 0, 
      10104/16,9608/16, 0, 
      10616/16,9608/16, 0, 
    },
  },
};
SDK_COMPILER_ASSERT(NELEMS(PalaceTownData) == PALACE_TOWN_DATA_MAX);

///�p���X�փ��[�v���Ă����Ƃ��̏o�����W
static const VecFx32 PalaceWarpPos = {
  1464/16,
  0*FX32_ONE,
  440/16,
};

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
  //�Q���l�����ς���Ă���Ȃ�l���𑗐M(�e�@��p)
  if(GFL_NET_IsParentMachine() == TRUE){
    int now_member = GFL_NET_GetConnectNum();
    if(now_member > intcomm->member_num){
      intcomm->member_num = now_member;
      intcomm->member_send_req = TRUE;
    }
  }
  if(intcomm->member_send_req == TRUE){
    if(IntrudeSend_MemberNum(intcomm, intcomm->member_num) == TRUE){
      intcomm->member_send_req = FALSE;
    }
  }

  
  //�v���t�B�[���v�����N�G�X�g���󂯂Ă���Ȃ瑗�M
  if(intcomm->profile_req == TRUE){
    Intrude_SetSendProfileBuffer(intcomm);  //���M�o�b�t�@�Ɍ��݂̃f�[�^���Z�b�g
    IntrudeSend_Profile(intcomm);           //���M
  }
  //�ʐM���m������Ă��郁���o�[�Ńv���t�B�[���������Ă��Ȃ������o�[������Ȃ烊�N�G�X�g��������
  Intrude_CheckProfileReq(intcomm);
  
  //�b���������f��̕Ԏ������܂��Ă���Ȃ�Ԏ��𑗐M����
  Intrude_CheckTalkAnswerNG(intcomm);
  //�~�b�V�������M���N�G�X�g������Α��M
  MISSION_SendUpdate(intcomm, &intcomm->mission);
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
  
  for(net_id = 0; net_id < intcomm->member_num; net_id++){
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
 * �v���t�B�[�����M�o�b�t�@�Ɍ��݂̃f�[�^���Z�b�g
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_SetSendProfileBuffer(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  MYSTATUS *myst;
  OCCUPY_INFO *occupy;
  
  myst = GAMEDATA_GetMyStatus(gamedata);
  GFL_STD_MemCopy(myst, &intcomm->send_profile.mystatus, MyStatus_GetWorkSize());
  
  occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  GFL_STD_MemCopy(myst, &intcomm->send_profile.occupy, sizeof(OCCUPY_INFO));
  
  //�N���X�e�[�^�X���Z�b�g
  Intrude_SetSendStatus(intcomm);
  //�v���t�B�[�����M���ł������̃p���X�ԍ��������Ă��Ȃ��ꍇ�̓Z�b�g
  if(intcomm->intrude_status_mine.palace_area == PALACE_AREA_NO_NULL){
    intcomm->intrude_status_mine.palace_area = GFL_NET_SystemGetCurrentID();
  }
  intcomm->send_profile.status = intcomm->intrude_status_mine;
  
}

//==================================================================
/**
 * �v���t�B�[���f�[�^����M�o�b�t�@�ɃZ�b�g
 *
 * @param   intcomm		
 * @param   net_id		
 * @param   profile		
 */
//==================================================================
void Intrude_SetProfile(INTRUDE_COMM_SYS_PTR intcomm, int net_id, const INTRUDE_PROFILE *profile)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  MYSTATUS *myst;
  OCCUPY_INFO *occupy;
  
  myst = GAMEDATA_GetMyStatusPlayer(gamedata, net_id);
  MyStatus_Copy(&profile->mystatus, myst);

  occupy = GAMEDATA_GetOccupyInfo(gamedata, net_id);
  GFL_STD_MemCopy(&profile->occupy, occupy, sizeof(OCCUPY_INFO));
  
  Intrude_SetPlayerStatus(intcomm, net_id, &profile->status);
  
  intcomm->recv_profile |= 1 << net_id;
  OS_TPrintf("�v���t�B�[����M�@net_id=%d, recv_bit=%d\n", net_id, intcomm->recv_profile);
}

//==================================================================
/**
 * �X�e�[�^�X�f�[�^����M�o�b�t�@�ɃZ�b�g
 *
 * @param   intcomm		
 * @param   net_id		
 * @param   sta		
 */
//==================================================================
void Intrude_SetPlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, int net_id, const INTRUDE_STATUS *sta)
{
  INTRUDE_STATUS *target_status;
  int mission_no;
  
  target_status = &intcomm->intrude_status[net_id];
  GFL_STD_MemCopy(sta, target_status, sizeof(INTRUDE_STATUS));

  //�v���C���[�X�e�[�^�X�Ƀf�[�^�Z�b�g�@��game_comm�Ɏ������Ă���̂�ς��邩��
  if(net_id == 0){
    mission_no = target_status->mission_no;
  }
  else{
    mission_no = GameCommStatus_GetPlayerStatus_MissionNo(intcomm->game_comm, 0);
    //mission_no = GameCommStatus_GetPlayerStatus_MissionNo(game_comm, GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()));
  }
  GameCommStatus_SetPlayerStatus(intcomm->game_comm, net_id, target_status->zone_id,
    target_status->palace_area, target_status->zone_id);

  //���W�ϊ�
  if(net_id != GFL_NET_SystemGetCurrentID()){
    Intrude_ConvertPlayerPos(intcomm, &intcomm->intrude_status_mine, target_status);
  }

#if 0 //�Ⴄ�]�[���ł��\������悤�ɕς���� 2009.10.21(��)
  {//�]�[���Ⴂ�ɂ��A�N�^�[�\���E��\���ݒ�
    PLAYER_WORK *my_player = GAMEDATA_GetMyPlayerWork(GameCommSys_GetGameData(intcomm->game_comm));
    ZONEID my_zone_id = PLAYERWORK_getZoneID( my_player );
    
    if(target_status->zone_id != my_zone_id){
      target_status->player_pack.vanish = TRUE;   //�Ⴄ�]�[���ɂ���̂Ŕ�\��
    }
    else if(ZONEDATA_IsPalace(target_status->zone_id) == FALSE
        && target_status->palace_area != intcomm->intrude_status_mine.palace_area){
      //�ʏ�t�B�[���h�̓����]�[���ɋ��Ă��A�N����ROM���Ⴄ�Ȃ��\��
      target_status->player_pack.vanish = TRUE;
    }
    else{
      target_status->player_pack.vanish = FALSE;
    }
  }
#endif
}

//==================================================================
/**
 * �]�[��ID�Ɉ�v����N���^�E������������
 *
 * @param   zone_id		�T�[�`����]�[��ID
 * @param   result		���ʑ����
 *
 * @retval  BOOL		TRUE:�N���^�E�����������B�@FALSE:��������
 */
//==================================================================
BOOL Intrude_SearchPalaceTown(ZONEID zone_id, PALACE_TOWN_RESULT *result)
{
  int i;
  
  result->zone_id = zone_id;
  for(i = 0; i < PALACE_TOWN_DATA_MAX; i++){
    if(PalaceTownData[i].front_zone_id == zone_id){
      result->front_reverse = PALACE_TOWN_SIDE_FRONT;
    }
    else if(PalaceTownData[i].reverse_zone_id == zone_id){
      result->front_reverse = PALACE_TOWN_SIDE_REVERSE;
    }
    else{
      continue;
    }
    result->tblno = i;
    return TRUE;
  }
  result->tblno = PALACE_TOWN_TBLNO_NULL;
  return FALSE;
}

//==================================================================
/**
 * �����]�[���ɂ��邩����(�\���]�[��������s��)
 *
 * @param   result_a		
 * @param   result_b		
 *
 * @retval  CHECKSAME		
 */
//==================================================================
CHECKSAME Intrude_CheckSameZoneID(const PALACE_TOWN_RESULT *result_a, const PALACE_TOWN_RESULT *result_b)
{
  int town_ret_a, town_ret_b;
  
  if(result_a->zone_id == result_b->zone_id){
    return CHECKSAME_SAME;
  }
  
  if(result_a->front_reverse != result_b->front_reverse){
    if(result_a->tblno == result_b->tblno){
      return CHECKSAME_SAME_REVERSE;
    }
  }
  return CHECKSAME_NOT;
}

//--------------------------------------------------------------
/**
 * �w��p���X�G���A�����[���牽�ڂ̃p���X�Ȃ̂����擾����
 *
 * @param   intcomm		
 * @param   palace_area		�p���X�G���A
 *
 * @retval  int		�����牽�ڂ̃p���X��
 */
//--------------------------------------------------------------
static int Intrude_GetPalaceOffsetNo(const INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  int start_area, offset_area;
  
  if(palace_area >= intcomm->member_num){
    GF_ASSERT_MSG(0, "palace_area = %d, member_num = %d\n", palace_area, intcomm->member_num);
    return 0;
  }
  
  start_area = GAMEDATA_GetIntrudeMyID(GameCommSys_GetGameData(intcomm->game_comm));
  offset_area = palace_area - start_area;
  if(offset_area < 0){
    offset_area += intcomm->member_num;
  }
  return offset_area;
}

//==================================================================
/**
 * �ʐM�v���C���[�̍��W��������̉�ʂɍ��킹�ĕϊ�����
 *
 * @param   mine		  
 * @param   target		
 */
//==================================================================
static void Intrude_ConvertPlayerPos(INTRUDE_COMM_SYS_PTR intcomm, const INTRUDE_STATUS *mine, INTRUDE_STATUS *target)
{
  PALACE_TOWN_RESULT result_a, result_b;
  BOOL search_a, search_b;
  CHECKSAME ret;
  
  Intrude_SearchPalaceTown(mine->zone_id, &result_a);
  Intrude_SearchPalaceTown(target->zone_id, &result_b);
  
  ret = Intrude_CheckSameZoneID(&result_a, &result_b);
  switch(ret){
  case CHECKSAME_SAME:
    if(ZONEDATA_IsPalace(mine->zone_id) == FALSE){
      return; //�����]�[���ɂ���̂ŕϊ��̕K�v����
    }
    //�o���Ƃ��p���X�ɂ���̂Ńp���X�G���A�ɍ��킹�����W�ϊ�
    {
      fx32 offset_x, map_len;
      map_len = PALACE_MAP_LEN;
      offset_x = target->player_pack.pos.x % map_len;
      target->player_pack.pos.x 
        = Intrude_GetPalaceOffsetNo(intcomm, target->palace_area) * map_len + offset_x;
    }
    break;
  case CHECKSAME_SAME_REVERSE:  //�\���]�[���ɂ���̂ł��̂܂܂ł悢
  case CHECKSAME_NOT:
    return; //�Ⴄ�]�[���ɂ���̂ł��̂܂܂ł悢
  }
}

//==================================================================
/**
 * �N���X�e�[�^�X���M�o�b�t�@���X�V
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:�ύX����@FALSE:�ύX�Ȃ�
 */
//==================================================================
BOOL Intrude_SetSendStatus(INTRUDE_COMM_SYS_PTR intcomm)
{
  BOOL send_req = FALSE;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  PLAYER_WORK *plWork = GAMEDATA_GetMyPlayerWork( gamedata );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  INTRUDE_STATUS *ist = &intcomm->intrude_status_mine;
  int mission_no;
  
  send_req = CommPlayer_Mine_DataUpdate(intcomm->cps, &ist->player_pack);
  if(ist->zone_id != zone_id){
    ist->zone_id = zone_id;
    send_req = TRUE;
  }
  
  mission_no = PALACE_SYS_GetMissionNo(intcomm->palace);
  if(ist->mission_no != mission_no){
    ist->mission_no = mission_no;
    send_req = TRUE;
  }
  
  return send_req;
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



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �N����Ԃ��Ď����A�T�u��ʂ��m�[�}���ƐN���A���ꂼ��ɕύX����K�v�����邩�Ď�����
 *
 * @param   game_comm		
 * @param   subscreen		
 *
 * @retval  FIELD_SUBSCREEN_MODE_MAX  ����̂܂ܕύX����K�v�Ȃ�
 * @retval  ��L�ȊO                  �߂�l�̃��[�h�ɕύX����K�v������
 */
//==================================================================
FIELD_SUBSCREEN_MODE Intrude_SUBSCREEN_Watch(GAME_COMM_SYS_PTR game_comm, FIELD_SUBSCREEN_WORK *subscreen)
{
  FIELD_SUBSCREEN_MODE subscreen_mode = FIELD_SUBSCREEN_GetMode(subscreen);
  
#if 0
  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_R){
    if(subscreen_mode == FIELD_SUBSCREEN_NORMAL){
      return FIELD_SUBSCREEN_INTRUDE;
    }
    else if(subscreen_mode == FIELD_SUBSCREEN_INTRUDE){
      return FIELD_SUBSCREEN_NORMAL;
    }
  }
#else

  if(subscreen_mode == FIELD_SUBSCREEN_NORMAL){
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION
        && GameCommSys_CheckSystemWaiting(game_comm) == FALSE && GFL_NET_GetConnectNum() > 1){
      return FIELD_SUBSCREEN_INTRUDE;
    }
  }
  else if(subscreen_mode == FIELD_SUBSCREEN_INTRUDE){
    if(NetErr_App_CheckError() == TRUE){
      //�G���[�������͂܂��߂��Ȃ� (�G���[��ʂ��o�Ă���߂�悤�ɂ��Ă���)
      return FIELD_SUBSCREEN_MODE_MAX;
    }
    if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION){
      return FIELD_SUBSCREEN_NORMAL;
    }
  }
#endif

  return FIELD_SUBSCREEN_MODE_MAX;
}

//==================================================================
/**
 * �p���X�^�E���̗��t�B�[���h�̃]�[��ID���擾
 *
 * @param   town_tblno		
 *
 * @retval  int		
 */
//==================================================================
int Intrude_GetPalaceTownZoneID(int town_tblno)
{
  if(town_tblno == PALACE_TOWN_DATA_PALACE){
    return ZONE_ID_PALACE01;
  }
  return PalaceTownData[town_tblno].reverse_zone_id;
}

//==================================================================
/**
 * �p���X�^�E���̃��[�v�o������W�������_���Ŏ擾���܂�
 *
 * @param   town_tblno		
 * @param   vec		
 */
//==================================================================
void Intrude_GetPalaceTownRandPos(int town_tblno, VecFx32 *vec)
{
  int rand_no;
  
  if(town_tblno == PALACE_TOWN_DATA_PALACE){
    *vec = PalaceWarpPos;
    return;
  }
  rand_no = GFUser_GetPublicRand0(PALACE_WARP_POS_PATERN);
  vec->x = GRID_TO_FX32( PalaceTownData[town_tblno].warp_pos[rand_no].gx );
  vec->y = PalaceTownData[town_tblno].warp_pos[rand_no].y * FX32_ONE;
  vec->z = GRID_TO_FX32( PalaceTownData[town_tblno].warp_pos[rand_no].gz );
  GF_ASSERT(vec->x != 0 || vec->y != 0 || vec->z != 0); //�S��0�͐ݒ肵�Y��
}

//==================================================================
/**
 * �N���ʐM������Ɍq�����Ă��邩���ׂ�
 *
 * @param   game_comm		
 *
 * @retval  INTRUDE_COMM_SYS_PTR	�q�����Ă���Ȃ� intcomm
 * @retval  INTRUDE_COMM_SYS_PTR	�q�����Ă��Ȃ��Ȃ� NULL
 *
 * �ʐM����̕Ԏ��҂��A�e�̕Ԏ��҂��A�Ȃǂ��Ă��鎞�ɁA�r���ŃG���[���������Ă��Ȃ���
 * �Ď����鏈�������ʉ��B
 */
//==================================================================
INTRUDE_COMM_SYS_PTR Intrude_Check_CommConnect(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  
  if(NetErr_App_CheckError() == TRUE || GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION 
      || GameCommSys_CheckSystemWaiting(game_comm) == TRUE || intcomm == NULL){
    return NULL;
  }
  return intcomm;
}
