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
#include "field/field_player.h"
#include "intrude_main.h"
#include "fieldmap/zone_id.h"
#include "field/zonedata.h"
#include "intrude_field.h"
#include "field/field_const.h"
#include "intrude_mission.h"
#include "poke_tool/monsno_def.h"
#include "field/field_status_local.h"  // for FIELD_STATUS_
#include "intrude_work.h"
#include "savedata/symbol_save_notwifi.h"
#include "savedata/intrude_save.h"
#include "item/itemsym.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�v���t�B�[���v�����N�G�X�g���ēx������܂ł̃E�F�C�g
#define INTRUDE_PROFILE_REQ_RETRY_WAIT    (30)

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void Intrude_CheckFieldProcAction(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckProfileReq(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckLeavePlayer(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckTalkAnswerNG(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckWfbcReq(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckSymbolReq(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_CheckMonolithStatusReq(INTRUDE_COMM_SYS_PTR intcomm);
static void Intrude_ConvertPlayerPos(INTRUDE_COMM_SYS_PTR intcomm, ZONEID mine_zone_id, fx32 mine_x, INTRUDE_STATUS *target);
static int Intrude_GetPalaceOffsetNo(const INTRUDE_COMM_SYS_PTR intcomm, int palace_area);
static void _SendBufferCreate_SymbolData(INTRUDE_COMM_SYS_PTR intcomm,const SYMBOL_DATA_REQ *p_sdr);
static void Intrude_UpdatePlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, NetID net_id);
static void Intrude_CancelCheckTalkedEventReserve(INTRUDE_COMM_SYS_PTR intcomm);

//==============================================================================
//  �f�[�^
//==============================================================================
///�X�̍��W�f�[�^   ���e�[�u�����̑�����������K��PALACE_TOWN_DATA_MAX�̐����ς��邱�ƁI
const PALACE_TOWN_DATA PalaceTownData[] = {
  {
    ZONE_ID_C07, 
    ZONE_ID_PLC07, 
    9*8, 7*8,
  },
  {
    ZONE_ID_C08, 
    ZONE_ID_PLC08, 
    0x10*8, 7*8,
  },
  {
    ZONE_ID_T03, 
    ZONE_ID_PLT03, 
    0x17*8, 7*8,
  },
  {
    ZONE_ID_C06, 
    ZONE_ID_PLC06, 
    5*8, 0xb*8+4,
  },
  {
    ZONE_ID_T04, 
    ZONE_ID_PLT04, 
    0x1b*8, 0xb*8+4,
  },
  {
    ZONE_ID_C05, 
    ZONE_ID_PLC05, 
    9*8, 0x10*8,
  },
  {
    ZONE_ID_C04, 
    ZONE_ID_PLC04, 
    0x10*8, 0x10*8,
  },
  {
    ZONE_ID_BC10,
    ZONE_ID_PLC10, 
    0x17*8, 0x10*8,
  },
};
SDK_COMPILER_ASSERT(NELEMS(PalaceTownData) == PALACE_TOWN_DATA_MAX);

//==============================================================================
//  �O���f�[�^
//==============================================================================
#include "mission_ng_check.cdat"


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
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);

  //�Q���l�����ς���Ă���Ȃ�l���𑗐M(�e�@��p)
  if(GFL_NET_IsParentMachine() == TRUE){
    int now_member = GFL_NET_GetConnectNum();
    if(now_member > intcomm->member_num){
      intcomm->member_num = now_member;
      intcomm->member_send_req = TRUE;
      GAMEDATA_SetIntrudeNum(gamedata, intcomm->member_num);
    }
  }
  if(intcomm->member_send_req == TRUE){
    if(IntrudeSend_MemberNum(intcomm, intcomm->member_num) == TRUE){
      intcomm->member_send_req = FALSE;
    }
  }

  //�t�B�[���h�X�e�[�^�X��PROC�A�N�V�����𔽉f
  Intrude_CheckFieldProcAction(intcomm);
  
  //�b��������ꂽ�C�x���g�̗\��L�����Z�����Ď�
  Intrude_CancelCheckTalkedEventReserve(intcomm);

  //�v���t�B�[���v�����N�G�X�g���󂯂Ă���Ȃ瑗�M
  if(intcomm->profile_req == TRUE){
    Intrude_SetSendProfileBuffer(intcomm);  //���M�o�b�t�@�Ɍ��݂̃f�[�^���Z�b�g
    IntrudeSend_Profile(intcomm);           //���M
  }
  //�ʐM���m������Ă��郁���o�[�Ńv���t�B�[���������Ă��Ȃ������o�[������Ȃ烊�N�G�X�g��������
  Intrude_CheckProfileReq(intcomm);
  //���E�҃`�F�b�N
  Intrude_CheckLeavePlayer(intcomm);
  
  //�b���������f��̕Ԏ������܂��Ă���Ȃ�Ԏ��𑗐M����
  Intrude_CheckTalkAnswerNG(intcomm);
  //�苒���ʑ��M
  if(intcomm->send_occupy_result_send_req == TRUE){
    if(IntrudeSend_OccupyResult(intcomm) == TRUE){
      intcomm->send_occupy_result_send_req = FALSE;
    }
  }
  //�~�b�V�����X�V����
  MISSION_Update(intcomm, &intcomm->mission);
  //�苒��񑗐M���N�G�X�g������Α��M
  if(intcomm->send_occupy == TRUE){
    if(IntrudeSend_OccupyInfo(intcomm) == TRUE){
      intcomm->send_occupy = FALSE;
    }
  }
  //WFBC���M���N�G�X�g������Α��M
  Intrude_CheckWfbcReq(intcomm);
  //�V���{���G���J�E���g�v��
  Intrude_CheckSymbolReq(intcomm);
  //���m���X�X�e�[�^�X�v��
  Intrude_CheckMonolithStatusReq(intcomm);
  
  //�v���C���[�X�e�[�^�X�X�e�[�^�X�X�V
  if(Intrude_SetSendStatus(intcomm) == TRUE){
    intcomm->send_status = TRUE;
  }

  //�v���C���[�X�e�[�^�X���M
  if(intcomm->send_status == TRUE && intcomm->send_my_position_stop == FALSE){
    if(IntrudeSend_PlayerStatus(intcomm, &intcomm->intrude_status_mine) == TRUE){
      intcomm->send_status = FALSE;
    }
  }
  
  //�Z�[�u�֘A�̃f�[�^�X�V����
  if(GAMEDATA_GetIsSave(gamedata) == FALSE){
    if(intcomm->recv_secret_item_flag == TRUE){
      INTRUDE_SAVE_WORK *intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork(gamedata) );
      if(Intrude_CheckNG_Item(intcomm->recv_secret_item.item) == FALSE){
        intcomm->recv_secret_item.item = ITEM_KIZUGUSURI;
      }
      ISC_SAVE_SetItem(intsave, &intcomm->recv_secret_item);
      intcomm->recv_secret_item_flag = FALSE;
    }
  }
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�X�e�[�^�X��PROC�A�N�V�����̕ύX�𔽉f����
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckFieldProcAction(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  FIELD_STATUS * fldstatus = GAMEDATA_GetFieldStatus(gamedata);
  PROC_ACTION proc_action = FIELD_STATUS_GetProcAction( fldstatus );

  switch(proc_action){
  case PROC_ACTION_FIELD:
    if(intcomm->intrude_status_mine.action_status == INTRUDE_ACTION_BATTLE){
      Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_FIELD);
    }
    break;
  case PROC_ACTION_BATTLE:
    if(intcomm->intrude_status_mine.action_status == INTRUDE_ACTION_FIELD){
      Intrude_SetActionStatus(intcomm, INTRUDE_ACTION_BATTLE);
    }
    break;
  }
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
      OS_TPrintf("%d�Ԃ̃v���t�B�[��������\n", net_id);
      IntrudeSend_ProfileReq();
      intcomm->profile_req_wait = INTRUDE_PROFILE_REQ_RETRY_WAIT;
      return;
    }
  }
}

//--------------------------------------------------------------
/**
 * ���E�҂�����΃v���t�B�[���폜���߂𑗐M
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckLeavePlayer(INTRUDE_COMM_SYS_PTR intcomm)
{
  int net_id;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  for(net_id = 0; net_id < intcomm->member_num; net_id++){
    if(GFL_NET_IsConnectMember(net_id) == FALSE 
        && (intcomm->recv_profile & (1 << net_id))){
      IntrudeSend_DeleteProfile(intcomm, net_id);
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
    if(intcomm->answer_talk_ng[net_id] != TALK_RAND_NULL){
      if(GFL_NET_IsConnectMember(net_id) == TRUE){
        if(IntrudeSend_TalkAnswer(
            intcomm, net_id, INTRUDE_TALK_STATUS_NG, intcomm->answer_talk_ng[net_id]) == TRUE){
          intcomm->answer_talk_ng[net_id] = TALK_RAND_NULL;
        }
      }
      else{
        intcomm->answer_talk_ng[net_id] = TALK_RAND_NULL;
      }
    }
  }

  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(intcomm->send_talk_rand_disagreement[net_id] != TALK_RAND_NULL){
      if(GFL_NET_IsConnectMember(net_id) == TRUE){
        if(IntrudeSend_TalkRandDisagreement(
            intcomm, net_id, intcomm->send_talk_rand_disagreement[net_id]) == TRUE){
          intcomm->send_talk_rand_disagreement[net_id] = TALK_RAND_NULL;
        }
      }
      else{
        intcomm->send_talk_rand_disagreement[net_id] = TALK_RAND_NULL;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * WFBC���M���N�G�X�g������Α��M
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckWfbcReq(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  FIELD_WFBC_COMM_NPC_ANS npc_ans;
  FIELD_WFBC_COMM_NPC_REQ npc_req;
  NetID net_id;
  
  if(intcomm->wfbc_req != 0){
    if(IntrudeSend_Wfbc(intcomm, intcomm->wfbc_req, GAMEDATA_GetMyWFBCCoreData(gamedata)) == TRUE){
      intcomm->wfbc_req = 0;
    }
  }
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(FIELD_WFBC_COMM_DATA_GetSendCommAnsData( 
        &intcomm->wfbc_comm_data, net_id, &npc_ans ) == TRUE){
      if(IntrudeSend_WfbcNpcAns(&npc_ans, net_id) == TRUE){
        FIELD_WFBC_COMM_DATA_ClearSendCommAnsData(&intcomm->wfbc_comm_data, net_id);
      }
    }
  }
  
  if(FIELD_WFBC_COMM_DATA_GetSendCommReqData(&intcomm->wfbc_comm_data, &npc_req, &net_id) == TRUE){
    if(IntrudeSend_WfbcNpcReq(&npc_req, net_id) == TRUE){
      FIELD_WFBC_COMM_DATA_ClearSendCommReqData(&intcomm->wfbc_comm_data);
    }
  }
}

//--------------------------------------------------------------
/**
 * �V���{���f�[�^�v�����N�G�X�g������Α��M
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckSymbolReq(INTRUDE_COMM_SYS_PTR intcomm)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  FIELD_WFBC_COMM_NPC_ANS npc_ans;
  FIELD_WFBC_COMM_NPC_REQ npc_req;
  NetID net_id;
  
  if(intcomm->wfbc_req != 0){
    if(IntrudeSend_Wfbc(intcomm, intcomm->wfbc_req, GAMEDATA_GetMyWFBCCoreData(gamedata)) == TRUE){
      intcomm->wfbc_req = 0;
    }
  }
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(intcomm->req_symbol_data[net_id].occ == TRUE){
      _SendBufferCreate_SymbolData(intcomm, &intcomm->req_symbol_data[net_id]);
      if(IntrudeSend_SymbolData(intcomm, net_id) == TRUE){
        intcomm->req_symbol_data[net_id].occ = FALSE;
      }
    }
  }
  
  if(intcomm->send_symbol_change.occ == TRUE){
    if(IntrudeSend_SymbolDataChange(&intcomm->send_symbol_change) == TRUE){
      intcomm->send_symbol_change.occ = FALSE;
    }
  }
}

//--------------------------------------------------------------
/**
 * ���M�o�b�t�@�쐬�F�V���{���G���J�E���g�f�[�^
 *
 * @param   intcomm		
 * @param   p_sdr		  �v������Ă���f�[�^
 */
//--------------------------------------------------------------
static void _SendBufferCreate_SymbolData(INTRUDE_COMM_SYS_PTR intcomm,const SYMBOL_DATA_REQ *p_sdr)
{
  INTRUDE_SYMBOL_WORK *sendbuf = &intcomm->intrude_send_symbol;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
  SYMBOL_SAVE_WORK *symbol_save = SymbolSave_GetSymbolData(sv_ctrl);
  u8 occ_num;
  
  SymbolSave_GetMapIDSymbolPokemon(
    symbol_save, sendbuf->spoke_array, SYMBOL_MAP_STOCK_MAX, p_sdr->symbol_map_id, &occ_num);
  
  sendbuf->num = occ_num;
  sendbuf->map_level_small = SymbolSave_GetMapLevelSmall(symbol_save);
  sendbuf->map_level_large = SymbolSave_GetMapLevelLarge(symbol_save);
  sendbuf->net_id = GFL_NET_SystemGetCurrentID();
  sendbuf->symbol_map_id = p_sdr->symbol_map_id;
}

//--------------------------------------------------------------
/**
 * ���m���X�X�e�[�^�X�v�����N�G�X�g������Α��M
 *
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void Intrude_CheckMonolithStatusReq(INTRUDE_COMM_SYS_PTR intcomm)
{
  if(intcomm->monost_req > 0){
    if(IntrudeSend_MonolithStatus(intcomm, intcomm->monost_req) == TRUE){
      intcomm->monost_req = FALSE;
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
  intcomm->send_status = TRUE;
  OS_TPrintf("set intrude action status = %d\n", action);
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
  GFL_STD_MemCopy(myst, &intcomm->my_profile.mystatus, MyStatus_GetWorkSize());
  
  occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  GFL_STD_MemCopy(occupy, &intcomm->my_profile.occupy, sizeof(OCCUPY_INFO));
  
  //�N���X�e�[�^�X���Z�b�g
  Intrude_SetSendStatus(intcomm);
  //�v���t�B�[�����M���ł������̃p���X�ԍ��������Ă��Ȃ��ꍇ�̓Z�b�g
  if(intcomm->intrude_status_mine.palace_area == PALACE_AREA_NO_NULL){
    Intrude_SetMinePalaceArea(intcomm, GFL_NET_SystemGetCurrentID());
  }
  intcomm->my_profile.status = intcomm->intrude_status_mine;
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
  BOOL new_player = FALSE;
  
  myst = GAMEDATA_GetMyStatusPlayer(gamedata, net_id);
  if((intcomm->recv_profile & (1 << net_id)) == 0
      || MyStatus_GetID(myst) != MyStatus_GetID(&profile->mystatus)){
    new_player = TRUE;
  }
  MyStatus_Copy(&profile->mystatus, myst);

  occupy = GAMEDATA_GetOccupyInfo(gamedata, net_id);
  GFL_STD_MemCopy(&profile->occupy, occupy, sizeof(OCCUPY_INFO));
  
  GAMEDATA_SetIntrudeSeasonID(gamedata, net_id, profile->status.season);
  
  Intrude_SetPlayerStatus(intcomm, net_id, &profile->status, TRUE);
  
  intcomm->recv_profile |= 1 << net_id;
  OS_TPrintf("�v���t�B�[����M�@net_id=%d, recv_bit=%d\n", net_id, intcomm->recv_profile);
  
  if(new_player == TRUE && net_id != GFL_NET_SystemGetCurrentID()){
    GameCommInfo_MessageEntry_PalaceConnect(intcomm->game_comm, net_id);
  }
}

//==================================================================
/**
 * �X�e�[�^�X�f�[�^����M�o�b�t�@�ɃZ�b�g
 *
 * @param   intcomm		
 * @param   net_id		
 * @param   sta		
 * @param   first_status      TRUE:���߂ăZ�b�g����X�e�[�^�X�@FALSE:2��ڈȍ~
 */
//==================================================================
void Intrude_SetPlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, int net_id, const INTRUDE_STATUS *sta, BOOL first_status)
{
  INTRUDE_STATUS *target_status;
  BOOL vanish;
  
  target_status = &intcomm->intrude_status[net_id];
  //vanish�͎�M���Ő��䂷��̂ŏ㏑�����Ȃ��B�A�����M�������\���ݒ肪���Ă��鎞�͎󂯎��
  vanish = target_status->player_pack.vanish;
  GFL_STD_MemCopy(sta, target_status, sizeof(INTRUDE_STATUS));
  target_status->player_pack.vanish = vanish;
  target_status->force_vanish = sta->player_pack.vanish;
  if(sta->player_pack.vanish == TRUE){
    target_status->player_pack.vanish = TRUE;
  }

  //�v���C���[�X�e�[�^�X�Ƀf�[�^�Z�b�g�@��game_comm�Ɏ������Ă���̂�ς��邩��
  GameCommStatus_SetPlayerStatus(intcomm->game_comm, net_id, target_status->zone_id,
    target_status->palace_area, target_status->pm_version, first_status);

  //���W�ϊ�
  if(net_id != GFL_NET_SystemGetCurrentID()){
    intcomm->backup_player_pack[net_id] = target_status->player_pack;
    Intrude_ConvertPlayerPos(intcomm, intcomm->intrude_status_mine.zone_id, 
      intcomm->intrude_status_mine.player_pack.pos.x, target_status);
  }
  
  intcomm->player_status_update |= 1 << net_id;
}

//==================================================================
/**
 * �v���C���[�X�e�[�^�X��ʐM�A�N�^�[�֔��f
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_UpdatePlayerStatusAll(INTRUDE_COMM_SYS_PTR intcomm)
{
  NetID net_id;
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  
  if(GAMEDATA_GetIsSave(gamedata) == TRUE){
    return; //MatrixID�̃`�F�b�N��ʐM�v���C���[��Add�ȂǂŃJ�[�h�A�N�Z�X�����
  }
  
  for(net_id = 0; net_id < FIELD_COMM_MEMBER_MAX; net_id++){
    if(intcomm->player_status_update & (1 << net_id)){
      Intrude_UpdatePlayerStatus(intcomm, net_id);
      intcomm->player_status_update ^= 1 << net_id;
    }
  }
}

//==================================================================
/**
 * �v���C���[�X�e�[�^�X��ʐM�A�N�^�[�֔��f
 *
 * @param   intcomm		
 * @param   net_id		
 */
//==================================================================
static void Intrude_UpdatePlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, NetID net_id)
{
  INTRUDE_STATUS *target_status;
  
  target_status = &intcomm->intrude_status[net_id];

  //�N���G���A�̈Ⴂ�ɂ��A�N�^�[�\���E��\���ݒ�
  if(net_id != GFL_NET_SystemGetCurrentID()){
    INTRUDE_STATUS *mine_st = &intcomm->intrude_status_mine;
    
    if(target_status->force_vanish == TRUE){
      //�����I�ɔ�\��
      target_status->player_pack.vanish = TRUE;
    }
    else if(target_status->player_pack.player_form != PLAYER_MOVE_FORM_NORMAL
        && target_status->player_pack.player_form != PLAYER_MOVE_FORM_CYCLE){
      //�\���ł��Ȃ��`�ԂȂ�Δ�\��
      target_status->player_pack.vanish = TRUE;
    }
    else if(ZONEDATA_IsPalace(target_status->zone_id) == TRUE && ZONEDATA_IsPalace(mine_st->zone_id) == TRUE){
      //���݂��p���X���ɋ���Ȃ�\��
      target_status->player_pack.vanish = FALSE;
    }
    else if(target_status->palace_area == mine_st->palace_area){
      //����ROM���ɂ���
      if(ZONEDATA_IsBingo(target_status->zone_id) == TRUE && ZONEDATA_IsBingo(mine_st->zone_id) == TRUE){
        //��l�Ƃ��V���{���}�b�v�ɂ���
        if(target_status->symbol_mapid == mine_st->symbol_mapid){
          target_status->player_pack.vanish = FALSE;
        }
        else{
          target_status->player_pack.vanish = TRUE;
        }
      }
      else{ //����ROM���ɂ��ăV���{���}�b�v�ȊO
        if(ZONEDATA_IsIntrudeSameMatrixID(mine_st->zone_id, target_status->zone_id) == TRUE){
          target_status->player_pack.vanish = FALSE;
        }
        else{
          target_status->player_pack.vanish = TRUE;
        }
      }
    }
    else{
      //�ႤROM�ɂ���
      target_status->player_pack.vanish = TRUE;
    }
  }

  //�ϑ��ɂ��OBJCODE���ς���Ă���`�F�b�N
  if(CommPlayer_CheckOcc(intcomm->cps, net_id) == TRUE){
    u16 obj_code = Intrude_GetObjCode(target_status, Intrude_GetMyStatus(GameCommSys_GetGameData(intcomm->game_comm), net_id));
    if(obj_code != CommPlayer_GetObjCode(intcomm->cps, net_id)){
      CommPlayer_Del(intcomm->cps, net_id);
      CommPlayer_Add(intcomm->cps, net_id, obj_code, &target_status->player_pack);
    }
  }
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
static void Intrude_ConvertPlayerPos(INTRUDE_COMM_SYS_PTR intcomm, ZONEID mine_zone_id, fx32 mine_x, INTRUDE_STATUS *target)
{
  PALACE_TOWN_RESULT result_a, result_b;
  BOOL search_a, search_b;
  CHECKSAME ret;
  
  Intrude_SearchPalaceTown(mine_zone_id, &result_a);
  Intrude_SearchPalaceTown(target->zone_id, &result_b);
  
  ret = Intrude_CheckSameZoneID(&result_a, &result_b);
  switch(ret){
  case CHECKSAME_SAME:
    if(ZONEDATA_IsPalace(mine_zone_id) == FALSE){
      return; //�����]�[���ɂ���̂ŕϊ��̕K�v����
    }
    //�o���Ƃ��p���X�ɂ���̂Ńp���X�G���A�ɍ��킹�����W�ϊ�
    {
      fx32 offset_x, map_len;
      int area_offset;
      
      map_len = PALACE_MAP_LEN;
      offset_x = target->player_pack.pos.x % map_len;
      area_offset = Intrude_GetPalaceOffsetNo(intcomm, target->palace_area);
      if(area_offset == intcomm->member_num - 1 && offset_x > PALACE_MAP_LEN/2 
          && mine_x < PALACE_MAP_LEN/2){
        //���肪�E�[�̃G���A�̉E���ɂ��āA���@�����[�̍����ɂ���Ȃ�A����̍��W��
        //���@�̍����ɂ����荞�݃}�b�v��̍��W�ɂ���
        target->player_pack.pos.x = -map_len + offset_x;
      }
      else if(area_offset == 0 && offset_x < PALACE_MAP_LEN/2 
          && mine_x > (PALACE_MAP_LEN*intcomm->member_num - PALACE_MAP_LEN/2)){
        //���肪���[�̃G���A�̍����ɂ��āA���@���E�[�̉E���ɂ���Ȃ�A����̍��W��
        //���@�̉E���ɂ����荞�݃}�b�v��̍��W�ɂ���
        target->player_pack.pos.x = intcomm->member_num * map_len + offset_x;
      }
      else{
        target->player_pack.pos.x = area_offset * map_len + offset_x;
      }
    }
    break;
  case CHECKSAME_SAME_REVERSE:  //�\���]�[���ɂ���̂ł��̂܂܂ł悢
  case CHECKSAME_NOT:
    return; //�Ⴄ�]�[���ɂ���̂ł��̂܂܂ł悢
  }
}

//==================================================================
/**
 * �o�b�N�A�b�v���W�����ɒʐM�v���C���[�̍��W���Čv�Z
 *
 * @param   intcomm		
 * @param   mine_x		���@�̍��WX
 */
//==================================================================
void Intrude_PlayerPosRecalc(INTRUDE_COMM_SYS_PTR intcomm, fx32 mine_x)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  int i;
  int my_netid = GAMEDATA_GetIntrudeMyID(gamedata);
  const INTRUDE_STATUS *mine = &intcomm->intrude_status_mine;
  
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if((intcomm->recv_profile & (1 << i)) && i != my_netid){
      INTRUDE_STATUS *target = &intcomm->intrude_status[i];
      target->player_pack = intcomm->backup_player_pack[i];
      Intrude_ConvertPlayerPos(intcomm, mine->zone_id, mine_x, target);
    }
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
  BOOL detect_fold;
  
  send_req = CommPlayer_Mine_DataUpdate(intcomm->cps, &ist->player_pack);
  if(ist->zone_id != zone_id){
    ist->zone_id = zone_id;
    send_req = TRUE;
  }
  
  if(ist->symbol_mapid != GAMEDATA_GetSymbolMapID(gamedata)){
    ist->symbol_mapid = GAMEDATA_GetSymbolMapID(gamedata);
    send_req = TRUE;
  }
  
  detect_fold = PAD_DetectFold();
  if(ist->detect_fold != detect_fold){
    ist->detect_fold = detect_fold;
    send_req = TRUE;
  }
  
  return send_req;
}

//==================================================================
/**
 * ��b�̔F�؃f�[�^������������
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_InitTalkCertification(INTRUDE_COMM_SYS_PTR intcomm)
{
  GFL_STD_MemClear(&intcomm->recv_certifi, sizeof(INTRUDE_TALK_CERTIFICATION));
}

//==================================================================
/**
 * ��M�f�[�^�����b�̔F�؃f�[�^���擾����
 *
 * @param   intcomm		
 *
 * @retval  INTRUDE_TALK_CERTIFICATION *		��M������b�̔F�؃f�[�^(NULL���͖���M)
 */
//==================================================================
INTRUDE_TALK_CERTIFICATION * Intrude_GetTalkCertification(INTRUDE_COMM_SYS_PTR intcomm)
{
  if(intcomm->recv_certifi.occ == TRUE){
    return &intcomm->recv_certifi;
  }
  return NULL;
}

//==================================================================
/**
 * ��b�\���̏�����
 *
 * @param   intcomm		
 * @param   talk_netid		�N���A�f�[�^�Ƃ��ď������Ȃ��INTRUDE_NETID_NULL
 * @param   talk_netid		�b�����������̏������Ȃ�Θb�������������NetID
 * @param   talk_netid		�b��������ꂽ���̏������Ȃ�Θb�������Ă��������NetID
 */
//==================================================================
void Intrude_InitTalkWork(INTRUDE_COMM_SYS_PTR intcomm, int talk_netid)
{
  GFL_STD_MemClear(&intcomm->talk, sizeof(INTRUDE_TALK));
  intcomm->talk.talk_netid = talk_netid;
  intcomm->talk.answer_talk_netid = INTRUDE_NETID_NULL;
  intcomm->talk.talk_rand = TALK_RAND_NULL;
  
  Intrude_TalkRandClose(intcomm);
}

//==================================================================
/**
 * �b���������������Ȃ��悤�Ƀ����_���R�[�h������������
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_TalkRandClose(INTRUDE_COMM_SYS_PTR intcomm)
{
  intcomm->now_talk_rand++;
  intcomm->talk.now_talk_rand = intcomm->now_talk_rand;
}


//==================================================================
/**
 * ��b�G���g���[
 *
 * @param   intcomm		
 * @param   net_id		�G���g���[�҂�NetID
 * @retval  TRUE:�b���o����B�@FALSE:�b���o���Ȃ�
 */
//==================================================================
BOOL Intrude_SetTalkReq(INTRUDE_COMM_SYS_PTR intcomm, int net_id, u8 talk_rand)
{
  if((intcomm->recv_profile & (1 << net_id)) == 0){
    OS_TPrintf("�b��������ꂽ���ǁA�܂��v���t�B�[������M���Ă��Ȃ��̂Œf��\n");
    intcomm->answer_talk_ng[net_id] = talk_rand;
    return FALSE;
  }

  GF_ASSERT(talk_rand != TALK_RAND_NULL);

  switch(intcomm->intrude_status_mine.action_status){
  case INTRUDE_ACTION_FIELD:
    if(intcomm->talk.talk_netid == INTRUDE_NETID_NULL && intcomm->talk.talk_rand == TALK_RAND_NULL
        && GAMESYSTEM_IsEventExists(intcomm->gsys) == FALSE){
      intcomm->talk.talk_netid = net_id;
      intcomm->talk.talk_status = INTRUDE_TALK_STATUS_OK;
      intcomm->talk.talk_rand = talk_rand;
      intcomm->talk.now_talk_rand = talk_rand;
      Intrude_SetTalkedEventReserve(intcomm);
      OS_TPrintf("talk_ok\n");
      return TRUE;
    }
    else{
      intcomm->answer_talk_ng[net_id] = talk_rand;
      OS_TPrintf("talk_ng field\n");
    }
    break;
  default:
    intcomm->answer_talk_ng[net_id] = talk_rand;
    OS_TPrintf("talk_ng\n");
    break;
  }
  
  return FALSE;
}

//==================================================================
/**
 * �b��������ꂽ�C�x���g��\��
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_SetTalkedEventReserve(INTRUDE_COMM_SYS_PTR intcomm)
{
  intcomm->talked_event_reserve = TRUE;
}

//==================================================================
/**
 * �b��������ꂽ�C�x���g�̗\����N���A
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_ClearTalkedEventReserve(INTRUDE_COMM_SYS_PTR intcomm)
{
  intcomm->talked_event_reserve = FALSE;
}

//==================================================================
/**
 * �b�������ꂽ�C�x���g���\�񒆂ɑ��̃C�x���g���N�������ꍇ�́A
 * �b��������ꂽ�C�x���g�̗\����L�����Z������
 *
 * @param   intcomm		
 */
//==================================================================
static void Intrude_CancelCheckTalkedEventReserve(INTRUDE_COMM_SYS_PTR intcomm)
{
  u32 talk_netid;
  
  if(Intrude_CheckTalkedTo(intcomm, &talk_netid) == TRUE){
    if(GAMESYSTEM_IsEventExists(intcomm->gsys) == TRUE){
      if(intcomm->answer_talk_ng[intcomm->talk.talk_netid] == TALK_RAND_NULL){
        intcomm->answer_talk_ng[intcomm->talk.talk_netid] = intcomm->talk.talk_rand;
      }
      Intrude_InitTalkWork(intcomm, INTRUDE_NETID_NULL);
      Intrude_ClearTalkedEventReserve(intcomm);
    }
  }
}

//==================================================================
/**
 * �ʐM�v���C���[����b���������Ă��Ȃ����`�F�b�N
 *
 * @param   intcomm		
 * @param   fld_player		
 * @param   hit_netid		  �b���������Ă���ꍇ�A�Ώۃv���C���[��NetID����������
 *
 * @retval  BOOL		TRUE:�ʐM�v���C���[����b���������Ă���@FALSE:�b���������Ă��Ȃ�
 */
//==================================================================
BOOL Intrude_CheckTalkedTo(INTRUDE_COMM_SYS_PTR intcomm, u32 *hit_netid)
{
  s16 check_gx, check_gy, check_gz;
  u32 out_index;
  
  if(intcomm == NULL || intcomm->cps == NULL){
    return FALSE;
  }
  
  //��������b�������Ă��Ȃ��̂ɒl��NULL�ȊO�Ȃ̂͘b��������ꂽ����
  if(intcomm->talk.talk_netid != INTRUDE_NETID_NULL && intcomm->talked_event_reserve == TRUE){
    *hit_netid = intcomm->talk.talk_netid;
    return TRUE;
  }
  
  return FALSE;
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
void Intrude_SetTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm, int net_id, INTRUDE_TALK_STATUS talk_status, u8 talk_rand)
{
  //�L�����Z���͂��ł��󂯓����
  if(talk_status != INTRUDE_TALK_STATUS_CANCEL){
    GF_ASSERT(intcomm->talk.answer_talk_status == INTRUDE_TALK_STATUS_NULL);
  }
  
  if(intcomm->talk.talk_netid == net_id && intcomm->talk.talk_rand == talk_rand){
    intcomm->talk.answer_talk_netid = net_id;
    intcomm->talk.answer_talk_status = talk_status;
  }
  else{
    OS_TPrintf("answer �s��v my %d, %d, recv %d, %d\n", intcomm->talk.talk_netid, intcomm->talk.talk_rand, net_id, talk_rand);
  }
}

//==================================================================
/**
 * ��b�̕Ԏ���M�f�[�^�Z�b�g(�L�����Z����p)
 *
 * @param   intcomm		
 * @param   net_id		
 */
//==================================================================
void Intrude_SetTalkCancel(INTRUDE_COMM_SYS_PTR intcomm, int net_id, u8 talk_rand)
{
  Intrude_SetTalkAnswer(intcomm, net_id, INTRUDE_TALK_STATUS_CANCEL, talk_rand);
}

//==================================================================
/**
 * ��b�̕Ԏ���M�f�[�^���擾
 *
 * @param   intcomm		
 * @param   net_id		    �Ԏ������Ă����l��NetID
 * @param   talk_status		��b�X�e�[�^�X
 *
 * INTRUDE_TALK_STATUS_NULL�ȊO�̃f�[�^�������Ă����ꍇ��
 * �o�b�t�@��NULL�ŃN���A���Ă���߂�l��Ԃ��܂�
 */
//==================================================================
INTRUDE_TALK_STATUS Intrude_GetTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm)
{
  INTRUDE_TALK_STATUS ret;
  
  if(intcomm->talk.answer_talk_netid != INTRUDE_NETID_NULL){
    ret = intcomm->talk.answer_talk_status;
    if(ret != INTRUDE_TALK_STATUS_NULL){
      intcomm->talk.answer_talk_status = INTRUDE_TALK_STATUS_NULL;
      return ret;
    }
  }
  return INTRUDE_TALK_STATUS_NULL;
}

//==================================================================
/**
 * ��b�̕Ԏ���M�f�[�^���N���A
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_ClearTalkAnswer(INTRUDE_COMM_SYS_PTR intcomm)
{
  intcomm->talk.answer_talk_status = INTRUDE_TALK_STATUS_NULL;
}



//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �N�����̃m�[�}���ϑ�����OBJCODE���擾
 *
 * @param   myst
 *
 * @retval  u16		OBJCODE
 */
//==================================================================
void Intrude_GetNormalDisguiseObjCode(const MYSTATUS *myst, u16 *objcode, u8 *disguise_type, u8 *disguise_sex)
{
  static const u16 NormalDisguise_Male[] = {
    BOY1,
    BOY2,
    BOY3,
    BOY4,
  };
  static const u16 NormalDisguise_Female[] = {
    GIRL1,
    GIRL2,
    GIRL3,
    GIRL4,
  };
  int tbl_no;
  u32 trainer_id, sex;
  
  trainer_id = MyStatus_GetID(myst);
  sex = MyStatus_GetMySex(myst);
  tbl_no = ((trainer_id >> 16) + (trainer_id & 0xffff)) % NELEMS(NormalDisguise_Male);
  if(sex == PM_MALE){
    *objcode = NormalDisguise_Male[tbl_no];
    *disguise_type = TALK_TYPE_MAN;
    *disguise_sex = PM_MALE;
  }
  else{
    *objcode = NormalDisguise_Female[tbl_no];
    *disguise_type = TALK_TYPE_WOMAN;
    *disguise_sex = PM_FEMALE;
  }
}

//==================================================================
/**
 * �N���X�e�[�^�X����\������OBJ�R�[�h���擾
 *
 * @param   sta		
 * @param   myst		
 *
 * @retval  u16		OBJ�R�[�h
 */
//==================================================================
u16 Intrude_GetObjCode(const INTRUDE_STATUS *sta, const MYSTATUS *myst)
{
  u16 obj_code;
  u8 disguise_type, disguise_sex;

  if(sta->disguise_no == DISGUISE_NO_NULL){
    if(sta->player_pack.player_form == PLAYER_MOVE_FORM_CYCLE){
      obj_code = (MyStatus_GetMySex(myst) == PM_MALE) ? CYCLEHERO : CYCLEHEROINE;
    }
    else{
      obj_code = (MyStatus_GetMySex(myst) == PM_MALE) ? HERO : HEROINE;
    }
  }
  else if(sta->disguise_no == DISGUISE_NO_NORMAL){
    Intrude_GetNormalDisguiseObjCode(myst, &obj_code, &disguise_type, &disguise_sex);
  }
  else{
    obj_code = sta->disguise_no;
    if(Intrude_CheckNG_OBJID(obj_code) == FALSE){
      GF_ASSERT_MSG(0, "obj_code = %d", obj_code);
      obj_code = (MyStatus_GetMySex(myst) == PM_MALE) ? BOY1 : GIRL1;
    }
  }
  
  //�O�̂���
  if((obj_code >= OBJCODEEND_BBD && obj_code < OBJCODESTART_TPOKE) 
      || obj_code >= OBJCODEEND_TPOKE){
    obj_code = BOY1;
    GF_ASSERT(0);
  }
  
  return obj_code;
}

//--------------------------------------------------------------
/**
 * �����ȊO�̃v���C���[�����݂��Ă��邩���ׂ�
 *
 * @param   none		
 *
 * @retval  BOOL		TRUE:���݂��Ă���B�@FALSE:�N�����Ȃ�
 */
//--------------------------------------------------------------
BOOL Intrude_OtherPlayerExistence(void)
{
  if(NetErr_App_CheckError()){
    return FALSE;
  }
  if(GFL_NET_GetConnectNum() > 1){
    return TRUE;
  }
  OS_TPrintf("�N�����Ȃ��Ȃ��� %d\n", GFL_NET_GetConnectNum());
  return FALSE;
}

//==================================================================
/**
 * �����̃��m���X�X�e�[�^�X���쐬����
 *
 * @param   gamedata		
 * @param   monost		  �����
 */
//==================================================================
void Intrude_MyMonolithStatusSet(GAMEDATA *gamedata, MONOLITH_STATUS *monost)
{
  INTRUDE_SAVE_WORK *intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork(gamedata) );
  
  GFL_STD_MemClear(monost, sizeof(MONOLITH_STATUS));

  monost->clear_mission_count = ISC_SAVE_GetMissionClearCount(intsave);
  monost->palace_sojourn_time = ISC_SAVE_PalaceSojournParam(
    intsave, GAMEDATA_GetPlayTimeWork(gamedata), SOJOURN_TIME_CALC_GET);
  ISC_SAVE_GetDistributionGPower_Array(
    intsave, monost->gpower_distribution_bit, INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX);

  monost->occ = TRUE;
}

//==================================================================
/**
 * �苒���ʂ���M���Ă��邩�m�F
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE�F��M���Ă���@FALSE�F��M���Ă��Ȃ�
 */
//==================================================================
BOOL Intrude_CheckRecvOccupyResult(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->recv_occupy_result.occ;
}

//==================================================================
/**
 * OBJID���N���ŕϑ��ł���OBJID�����`�F�b�N����(�s���`�F�b�N�Ɏg�p)
 *
 * @param   obj_id		
 *
 * @retval  BOOL		TRUE:�\��OK�@FALSE:�s����OBJID
 */
//==================================================================
BOOL Intrude_CheckNG_OBJID(u16 obj_id)
{
  int i;
  
  for(i = 0; i < NELEMS(IntrudeOBJID_List); i++){
    if(IntrudeOBJID_List[i] == obj_id){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * �A�C�e�����N���̃~�b�V�����Ŕ����A����o����A�C�e�������`�F�b�N����(�s���`�F�b�N�Ɏg�p)
 *
 * @param   item_no		
 *
 * @retval  BOOL		TRUE:�\��OK�@FALSE:�s����OBJID
 */
//==================================================================
BOOL Intrude_CheckNG_Item(u16 item_no)
{
  int i;
  
  for(i = 0; i < NELEMS(IntrudeItem_List); i++){
    if(IntrudeItem_List[i] == item_no){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * �^�C���A�E�g�ɂ��ؒf�̗L���E������ݒ肷��
 *
 * @param   intcomm		
 * @param   stop		  TRUE:�^�C���A�E�g�ɂ��ؒf���s��Ȃ�
 *              		  FALSE:�^�C���A�E�g�ɂ��ؒf������
 */
//==================================================================
void Intrude_SetIsBattleFlag(INTRUDE_COMM_SYS_PTR intcomm, BOOL is_battle)
{
  intcomm->battle_proc_call = is_battle;
  OS_TPrintf("is_battle = %d\n", is_battle);
}

//==================================================================
/**
 * ������������p���X�G���A���Z�b�g���鎞�ɍs���������܂Ƃ߂�����
 *
 * @param   intcomm		
 */
//==================================================================
void Intrude_SetMinePalaceArea(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(intcomm->gsys);
  
  intcomm->intrude_status_mine.palace_area = palace_area;
  GAMEDATA_SetIntrudePalaceArea(gamedata, palace_area);
  FIELD_WFBC_COMM_DATA_SetIntrudeNetID(&intcomm->wfbc_comm_data, palace_area);
}

//==================================================================
/**
 * ���M�p�̎������W�X�V�������~����t���O���Z�b�g
 *
 * @param   intcomm		
 * @param   stop		  TRUE:�X�V��~�@FALSE:�X�V����
 */
//==================================================================
void Intrude_SetSendMyPositionStopFlag(INTRUDE_COMM_SYS_PTR intcomm, BOOL stop)
{
  intcomm->send_my_position_stop = stop;
}
