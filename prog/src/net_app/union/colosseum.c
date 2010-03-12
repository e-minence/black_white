//==============================================================================
/**
 * @file    colosseum.c
 * @brief   �R���V�A��:�V�X�e���p�����[�^�A�N�Z�X�n�F�풓�ɔz�u(�ʐM��M�f�[�^���Z�b�g���邽��)
 * @author  matsuda
 * @date    2009.07.17(��)
 */
//==============================================================================
#include <gflib.h>
#include "net_app/union/colosseum.h"
#include "net_app/union/colosseum_types.h"
#include "field\fieldmap_proc.h"
#include "app\trainer_card.h"
#include "colosseum_types.h"
#include "system/main.h"
#include "comm_player.h"
#include "field\field_player.h"
#include "field\fieldmap.h"
#include "colosseum_comm_command.h"
#include "battle/btl_net.h"


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _BattleDemoParent_SetTrainerData(COLOSSEUM_SYSTEM_PTR clsys, COMM_BTL_DEMO_TRAINER_DATA *demo_tr, NetID net_id, HEAPID heap_id);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �R���V�A���V�X�e�����[�N�쐬
 *
 * @param   fieldWork   
 * @param   myst    
 *
 * @retval  COLOSSEUM_SYSTEM_PTR    
 */
//==================================================================
COLOSSEUM_SYSTEM_PTR Colosseum_InitSystem(GAMEDATA *game_data, GAMESYS_WORK *gsys, MYSTATUS *myst, BOOL intrude)
{
  COLOSSEUM_SYSTEM_PTR clsys;
  int i, my_net_id;
  COLOSSEUM_BASIC_STATUS *my_basic;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  
  //�������m�ۂƃV�X�e������
  clsys = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(COLOSSEUM_SYSTEM));
  clsys->cps = CommPlayer_Init(COLOSSEUM_MEMBER_MAX, gsys, HEAPID_UNION);
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    clsys->recvbuf.tr_card[i] = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(TR_CARD_DATA));
    clsys->recvbuf.pokeparty[i] = PokeParty_AllocPartyWork(HEAPID_UNION);
  }
  
  //�����l�Z�b�g
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    clsys->parentsys.stand_position[i] = COLOSSEUM_STANDING_POSITION_NULL;
    clsys->parentsys.answer_stand_position[i] = COLOSSEUM_STANDING_POSITION_NULL;
  }
  clsys->mine.stand_position = COLOSSEUM_STANDING_POSITION_NULL;
  clsys->mine.answer_stand_position = COLOSSEUM_STANDING_POSITION_NULL;
  PokeParty_Copy( //�����̎莝���|�P�������Z�b�g���Ă���
    GAMEDATA_GetMyPokemon(game_data), clsys->recvbuf.pokeparty[my_net_id]);
  
  //�����̊�{���G���A�Ƀf�[�^���Z�b�g����
  my_basic = &clsys->basic_status[my_net_id];
  MyStatus_Copy(myst, &my_basic->myst);
  OS_GetMacAddress(my_basic->mac_address);
  my_basic->occ = TRUE;
  my_basic->force_entry = (intrude == TRUE) ? FALSE : TRUE;
  my_basic->battle_server_version = BTL_NET_SERVER_VERSION;
  
  //�����̃g���[�i�[�J�[�h���Z�b�g
  TRAINERCARD_GetSelfData(clsys->recvbuf.tr_card[my_net_id], game_data, TRUE, FALSE, HEAPID_UNION);
  clsys->recvbuf.tr_card_occ[my_net_id] = TRUE;
  
  return clsys;
}

//==================================================================
/**
 * �R���V�A���V�X�e�����[�N�j��
 *
 * @param   clsys   
 */
//==================================================================
void Colosseum_ExitSystem(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i;
  
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    GFL_HEAP_FreeMemory(clsys->recvbuf.tr_card[i]);
    GFL_HEAP_FreeMemory(clsys->recvbuf.pokeparty[i]);
  }
  CommPlayer_Exit(clsys->cps);
  GFL_HEAP_FreeMemory(clsys);
}

//==================================================================
/**
 * comm_ready�t���O���Z�b�g����
 *
 * @param   clsys   
 * @param   flag    
 */
//==================================================================
void Colosseum_CommReadySet(COLOSSEUM_SYSTEM_PTR clsys, BOOL flag)
{
  clsys->comm_ready = flag;
}

//==================================================================
/**
 * �����̗����ʒu���Z�b�g
 *
 * @param   clsys             
 * @param   stand_position    �����ʒu
 */
//==================================================================
void Colosseum_Mine_SetStandingPostion(COLOSSEUM_SYSTEM_PTR clsys, int stand_position)
{
  clsys->mine.stand_position = stand_position;
  clsys->mine.answer_stand_position = COLOSSEUM_STANDING_POSITION_NULL;
}

//==================================================================
/**
 * �����̗����ʒu���擾
 *
 * @param   clsys             
 * @retval  �����ʒu
 */
//==================================================================
u8 Colosseum_Mine_GetStandingPostion(COLOSSEUM_SYSTEM_PTR clsys)
{
  return clsys->mine.stand_position;
}

//==================================================================
/**
 * �����ʒu�g�p���̌��ʂ���
 *
 * @param   clsys   
 * @param   result    TRUE:�g�pOK�B�@FALSE:�g�p�s��
 */
//==================================================================
void Colosseum_Mine_SetAnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys, int result)
{
  clsys->mine.answer_stand_position = result;
}

//==================================================================
/**
 * �����ʒu�g�p���̌��ʂ���
 *
 * @param   clsys   
 * @param   result    TRUE:�g�pOK�B�@FALSE:�g�p�s��
 */
//==================================================================
u8 Colosseum_Mine_GetAnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys)
{
  return clsys->mine.answer_stand_position;
}

//==================================================================
/**
 * �e�@��p���߁F�q���瑗���Ă��������ʒu�����Z�b�g
 *
 * @param   clsys   
 * @param   net_id            ������NetID
 * @param   stand_position    �����ʒu���
 *
 * @retval  BOOL    TRUE:����Z�b�g�B�@FALSE:�����ʒu���������Ă���B�Z�b�g���s
 *          �Ԏ��p�̑��M�o�b�t�@�ւ̃f�[�^�Z�b�g�������ɍs��
 */
//==================================================================
BOOL Colosseum_Parent_SetStandingPosition(COLOSSEUM_SYSTEM_PTR clsys, int net_id, u8 stand_position)
{
  int i;
  BOOL result;
  
  if(stand_position == COLOSSEUM_STANDING_POSITION_NULL){
    clsys->parentsys.stand_position[net_id] = stand_position;
    result = TRUE;
  }
  else{
    for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
      if(clsys->parentsys.stand_position[i] == stand_position){
        OS_TPrintf("�����ʒu�o�b�e�B���O�I ��=%d, ��=%d\n", i, net_id);
        result = FALSE;
        break;
      }
    }
    if(i == COLOSSEUM_MEMBER_MAX){
      clsys->parentsys.stand_position[net_id] = stand_position;
      result = TRUE;
    }
  }

  clsys->parentsys.answer_stand_position[net_id] = result;
  return result;
}

//==================================================================
/**
 * �e�@��p���߁F�����ʒu���̕Ԏ��f�[�^������Α��M���s��
 *
 * @param   clsys   
 */
//==================================================================
void Colosseum_Parent_SendAnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->parentsys.answer_stand_position[i] != COLOSSEUM_STANDING_POSITION_NULL){
      if(ColosseumSend_AnswerStandingPosition(clsys, i, clsys->parentsys.answer_stand_position[i]) == TRUE){
        clsys->parentsys.answer_stand_position[i] = COLOSSEUM_STANDING_POSITION_NULL;
        OS_TPrintf("�����ʒu�Ԏ����M�F�����F���M��net_id=%d\n", i);
      }
      else{
        OS_TPrintf("�����ʒu�Ԏ����M�F���s�F���M��net_id=%d\n", i);
      }
    }
  }
}

//==================================================================
/**
 * �e�@��p���߁F�퓬�����L�����Z���̗v��������Α��M���s��
 *               �S���̐퓬�������������Ă���̂ł���΁A����̑��M���s��
 *
 * @param   clsys   
 */
//==================================================================
void Colosseum_Parent_BattleReadyAnswer(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, ready_count;
  
  if(GFL_NET_IsParentMachine() == FALSE){
    return;
  }
  
  ready_count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->parentsys.battle_ready[i] == TRUE){
      ready_count++;
    }
  }
  if(ready_count == GFL_NET_GetConnectNum()){
    if(ColosseumSend_AllBattleReady() == TRUE){
      for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){  //���M���������Ȃ獡�܂ł̎�M�o�b�t�@�̓N���A
        clsys->parentsys.battle_ready[i] = FALSE;
        clsys->parentsys.battle_ready_cancel[i] = FALSE;
      }
    }
  }
  else{ //���ɑS������OK�̏ꍇ�͍��X�L�����Z�������Ȃ�
    for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
      if(clsys->parentsys.battle_ready_cancel[i] == TRUE){
        if(ColosseumSend_BattleReadyCancelOK(i) == TRUE){
          clsys->parentsys.battle_ready_cancel[i] = FALSE;
        }
      }
    }
  }
}

//==================================================================
/**
 * �ʐM�v���C���[�̍��W�p�b�P�[�W���Z�b�g
 *
 * @param   clsys   
 * @param   net_id    �Ώۃv���C���[��netID
 * @param   pack      ���W�p�b�P�[�W
 */
//==================================================================
void Colosseum_SetCommPlayerPos(COLOSSEUM_SYSTEM_PTR clsys, int net_id, const COMM_PLAYER_PACKAGE *pack)
{
  clsys->recvbuf.comm_player_pack[net_id] = *pack;
  clsys->recvbuf.comm_player_pack_update[net_id] = TRUE;
}

//==================================================================
/**
 * �ʐM�v���C���[�̍��W�p�b�P�[�W���擾
 *
 * @param   clsys   
 * @param   net_id    �Ώۃv���C���[��netID
 * @param   dest      ���W�����
 *
 * @retval  BOOL    TRUE:�X�V����@FALSE:�X�V�Ȃ�
 * 
 * ���̊֐��Ŏ擾����ƍX�V�t���O���N���A����鎖�ɒ���
 */
//==================================================================
BOOL Colosseum_GetCommPlayerPos(COLOSSEUM_SYSTEM_PTR clsys, int net_id, COMM_PLAYER_PACKAGE *dest)
{
  BOOL update_flag;
  
  *dest = clsys->recvbuf.comm_player_pack[net_id];
  update_flag = clsys->recvbuf.comm_player_pack_update[net_id];
  clsys->recvbuf.comm_player_pack_update[net_id] = FALSE;
  return update_flag;
}

//==================================================================
/**
 * �|�P�������X�g�I�������l�����擾����
 *
 * @param   clsys   
 *
 * @retval  u8      �I�������l��(�����܂�)
 */
//==================================================================
u8 ColosseumTool_ReceiveCheck_PokeListSeletedNum(COLOSSEUM_SYSTEM_PTR clsys)
{
  return clsys->recvbuf.pokelist_selected_num;
}

//==================================================================
/**
 * ��M�o�b�t�@�̃N���A�F�|�P�������X�g�I�������l��
 *
 * @param   clsys   
 */
//==================================================================
void ColosseumTool_Clear_ReceivePokeListSelected(COLOSSEUM_SYSTEM_PTR clsys)
{
  clsys->recvbuf.pokelist_selected_num = 0;
}

//==================================================================
/**
 * �S������POKEPARTY���󂯎��Ă��邩���ׂ�
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:�S������M���Ă���
 */
//==================================================================
BOOL Colosseum_AllReceiveCheck_Pokeparty(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.pokeparty_occ[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * ��M�o�b�t�@�̃N���A�FPOKEPARTY
 *
 * @param   clsys		
 * @param   except_for_mine   TRUE:�����̃f�[�^�̓N���A���Ȃ�(��M�t���O�͗��Ƃ�)
 */
//==================================================================
void Colosseum_Clear_ReceivePokeParty(COLOSSEUM_SYSTEM_PTR clsys, BOOL except_for_mine)
{
  int i, my_net_id;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(except_for_mine == FALSE || my_net_id != i){
      PokeParty_InitWork(clsys->recvbuf.pokeparty[i]);
    }
    clsys->recvbuf.pokeparty_occ[i] = FALSE;
  }
}

//==================================================================
/**
 * �o�g���J�n�E�I���f���pParentWork�쐬
 *
 * @param   clsys		
 * @param   cbdp		
 * @param   heap_id		
 */
//==================================================================
void Colosseum_SetupBattleDemoParent(COLOSSEUM_SYSTEM_PTR clsys, COMM_BTL_DEMO_PARAM *cbdp, HEAPID heap_id)
{
  int net_id, member_max, set_id, stand_pos;
  
  GFL_STD_MemClear(cbdp, sizeof(COMM_BTL_DEMO_PARAM));
  
  cbdp->type = COMM_BTL_DEMO_TYPE_MULTI_START;
  member_max = 0;
  for(net_id = 0; net_id < COLOSSEUM_MEMBER_MAX; net_id++){
    member_max++;
    if(clsys->recvbuf.pokeparty_occ[net_id] == FALSE){
      cbdp->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      break;
    }
  }
  
  for(net_id = 0; net_id < member_max; net_id++){
    stand_pos = clsys->recvbuf.stand_position[net_id];
    if((stand_pos & 1) == 0){  //����
      set_id = COMM_BTL_DEMO_TRDATA_A;
    }
    else{ //�E��
      if(cbdp->type == COMM_BTL_DEMO_TYPE_NORMAL_START){
        set_id = COMM_BTL_DEMO_TRDATA_B;
      }
      else{
        set_id = COMM_BTL_DEMO_TRDATA_C;
      }
    }
    _BattleDemoParent_SetTrainerData(
      clsys, &cbdp->trainer_data[set_id + (stand_pos >> 1)], net_id, heap_id);
  }
}

//==================================================================
/**
 * �o�g���J�n�E�I���f���pParentWork�������
 *
 * @param   cbdp		
 */
//==================================================================
void Colosseum_DeleteBattleDemoParent(COMM_BTL_DEMO_PARAM *cbdp)
{
  GFL_STD_MemClear(cbdp, sizeof(COMM_BTL_DEMO_PARAM));
}

//--------------------------------------------------------------
/**
 * �o�g���J�n�E�I���f���Ŏg�p����g���[�i�[�f�[�^���Z�b�g
 *
 * @param   clsys		
 * @param   demo_tr		�g���[�i�[�f�[�^�����
 * @param   net_id		�Ώۂ�NetID
 * @param   heap_id		
 */
//--------------------------------------------------------------
static void _BattleDemoParent_SetTrainerData(COLOSSEUM_SYSTEM_PTR clsys, COMM_BTL_DEMO_TRAINER_DATA *demo_tr, NetID net_id, HEAPID heap_id)
{
  demo_tr->party = clsys->recvbuf.pokeparty[net_id];
  demo_tr->mystatus = &clsys->basic_status[net_id].myst;
  demo_tr->server_version = clsys->basic_status[net_id].battle_server_version;
}
