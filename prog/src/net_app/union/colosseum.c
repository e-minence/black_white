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
COLOSSEUM_SYSTEM_PTR Colosseum_InitSystem(GAMEDATA *game_data, FIELD_MAIN_WORK *fieldWork, MYSTATUS *myst, BOOL intrude)
{
  COLOSSEUM_SYSTEM_PTR clsys;
  int i, my_net_id;
  COLOSSEUM_BASIC_STATUS *my_basic;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  
  //�������m�ۂƃV�X�e������
  clsys = GFL_HEAP_AllocClearMemory(HEAPID_UNION, sizeof(COLOSSEUM_SYSTEM));
  clsys->cps = CommPlayer_Init(COLOSSEUM_MEMBER_MAX, fieldWork, HEAPID_UNION);
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
  MyStatus_CopyNameStrCode(myst, my_basic->name, PERSON_NAME_SIZE + EOM_SIZE);
  my_basic->id = MyStatus_GetID(myst);
  OS_GetMacAddress(my_basic->mac_address);
  my_basic->sex = MyStatus_GetMySex(myst);
  my_basic->trainer_view = MyStatus_GetTrainerView(myst);
  my_basic->occ = TRUE;
  my_basic->force_entry = (intrude == TRUE) ? FALSE : TRUE;
  
  //�����̃g���[�i�[�J�[�h���Z�b�g
  TRAINERCARD_GetSelfData(clsys->recvbuf.tr_card[my_net_id], game_data, TRUE);
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
 * @param   stand_position		�����ʒu
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
 * @retval	�����ʒu
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
 * @param   result		TRUE:�g�pOK�B�@FALSE:�g�p�s��
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
 * @param   result		TRUE:�g�pOK�B�@FALSE:�g�p�s��
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
 * @param   net_id		        ������NetID
 * @param   stand_position		�����ʒu���
 *
 * @retval  BOOL		TRUE:����Z�b�g�B�@FALSE:�����ʒu���������Ă���B�Z�b�g���s
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
 * �ʐM�v���C���[�̍��W�p�b�P�[�W���Z�b�g
 *
 * @param   clsys		
 * @param   net_id		�Ώۃv���C���[��netID
 * @param   pack		  ���W�p�b�P�[�W
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
 * @param   net_id		�Ώۃv���C���[��netID
 * @param   dest		  ���W�����
 *
 * @retval  BOOL		TRUE:�X�V����@FALSE:�X�V�Ȃ�
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
