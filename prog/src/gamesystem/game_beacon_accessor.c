//==============================================================================
/**
 * @file    game_beacon_accessor.c
 * @brief   �r�[�R�����̒l�擾�ׂ̈̃A�N�Z�T
 * @author  matsuda
 * @date    2010.01.19(��)
 *
 * ����Ⴂ���O��ʂł����g�p���Ȃ��͂��Ȃ̂ŁA�풓�팸�̈�
 * �A�N�Z�T�����̃t�@�C���ɏW��FIELDMAP�I�[�o�[���C�ɔz�u����
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/game_data.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_accessor.h"
#include "gamesystem/game_beacon_types.h"



//==============================================================================
//
//  �r�[�R���e�[�u������
//
//==============================================================================
//==================================================================
/**
 * �r�[�R���X�^�b�N�e�[�u�����X�V
 *
 * @param   stack_infotbl		�X�^�b�N�e�[�u���ւ̃|�C���^
 *
 * @retval  int		�X�^�b�N�e�[�u���ֈڂ��ꂽ����
 */
//==================================================================
int GAMEBEACON_Stack_Update(GAMEBEACON_INFO_TBL *stack_infotbl)
{
  int start_log_no = 0, update_logno;
  const GAMEBEACON_INFO *update_info;
  int new_stack_count = 0;
  RTCTime recv_time;
  u16 time;
  
  GFL_RTC_GetTime(&recv_time);
  time = (recv_time.hour << 8) | recv_time.minute;
  
  do{
    update_logno = GAMEBEACON_Get_UpdateLogNo(&start_log_no);
    if(update_logno == GAMEBEACON_SYSTEM_LOG_MAX){
      break;  //�r�[�R�����O����
    }
    update_info = GAMEBEACON_Get_BeaconLog(update_logno);
    if(GAMEBEACON_InfoTbl_SetBeacon(
        stack_infotbl, update_info, time, FALSE) == GAMEBEACON_INFO_TBL_MAX){
      break;  //�X�^�b�N�e�[�u���������ς��ɂȂ���
    }
    GAMEBEACON_Reset_UpdateFlag(update_logno);
    new_stack_count++;
  }while(1);
  
  return new_stack_count;
}

//==================================================================
/**
 * �X�^�b�N�e�[�u������r�[�R�������擾����
 *
 * @param   stack_infotbl		�X�^�b�N�e�[�u���ւ̃|�C���^
 * @param   dest_info		    �r�[�R���������ւ̃|�C���^
 * @param   time            ��M�����̑����ւ̃|�C���^(���8bit�F��(0�`23)�A����8bit�F��(0�`59))
 *
 * @retval  BOOL		TRUE:�����擾�����@�@FALSE:���擾���s(�X�^�b�N�e�[�u���Ƀf�[�^������)
 *
 * ���擾�ɐ��������ꍇ�A�X�^�b�N�e�[�u�����̃f�[�^���O�l�߂���܂�
 */
//==================================================================
BOOL GAMEBEACON_Stack_GetInfo(GAMEBEACON_INFO_TBL *stack_infotbl, GAMEBEACON_INFO *dest_info, u16 *time)
{
  int log_no;
  
  if(stack_infotbl->info[0].version_bit == 0){
    return FALSE;
  }
  
  *dest_info = stack_infotbl->info[0];
  *time = stack_infotbl->time[0];
  
  //�X�^�b�N�e�[�u����O�l��
  for(log_no = 1; log_no < GAMEBEACON_INFO_TBL_MAX; log_no++){
    stack_infotbl->info[log_no - 1] = stack_infotbl->info[log_no];
    stack_infotbl->time[log_no - 1] = stack_infotbl->time[log_no];
  }
  GFL_STD_MemClear(&stack_infotbl->info[GAMEBEACON_INFO_TBL_MAX - 1], sizeof(GAMEBEACON_INFO));
  
  return TRUE;
}

//==================================================================
/**
 * �r�[�R���e�[�u���֐V�����r�[�R����o�^����
 *
 * @param   infotbl		  �r�[�R���e�[�u���ւ̃|�C���^
 * @param   set_info		�o�^����r�[�R��
 * @param   time        ��M����(���8bit�F��(0�`23)�A����8bit�F��(0�`59))
 * @param   push_out    TRUE:�f�[�^�������ς��̏ꍇ�A�擪�̃f�[�^���Ƃ���Ă񎮂�
 *                           �����o�����ăZ�b�g���܂��B
 *                      FALSE:�f�[�^�����ς��̏ꍇ�A�o�^�s�Ƃ��ď������I�����܂�
 *
 * @retval  int		�o�^�������O�ʒu(�o�^�ł��Ȃ������ꍇ��GAMEBEACON_INFO_TBL_MAX)
 * 
 * ���Ƀe�[�u���ɑ��݂��Ă��郆�[�U�[�̏ꍇ�͏㏑���B
 * �V�K���[�U�[�̏ꍇ�̓e�[�u���̍Ō�ɒǉ�
 */
//==================================================================
int GAMEBEACON_InfoTbl_SetBeacon(GAMEBEACON_INFO_TBL *infotbl, const GAMEBEACON_INFO *set_info, u16 time, BOOL push_out)
{
  int log_no;
  
  for(log_no = 0; log_no < GAMEBEACON_INFO_TBL_MAX; log_no++){
    if(infotbl->info[log_no].version_bit == 0 
        || infotbl->info[log_no].trainer_id == set_info->trainer_id){
      infotbl->info[log_no] = *set_info;
      return log_no;
    }
  }
  
  if(push_out == TRUE){
    //�X�^�b�N�e�[�u����O�l��
    for(log_no = 1; log_no < GAMEBEACON_INFO_TBL_MAX; log_no++){
      infotbl->info[log_no - 1] = infotbl->info[log_no];
      infotbl->time[log_no - 1] = infotbl->time[log_no];
    }
    infotbl->info[GAMEBEACON_INFO_TBL_MAX - 1] = *set_info;
    infotbl->time[GAMEBEACON_INFO_TBL_MAX - 1] = time;
    return GAMEBEACON_INFO_TBL_MAX - 1;
  }
  
  return GAMEBEACON_INFO_TBL_MAX;
}

//==================================================================
/**
 * �r�[�R���e�[�u�����烍�O�ԍ��̃r�[�R�������擾����
 *
 * @param   infotbl		  �r�[�R���e�[�u���ւ̃|�C���^
 * @param   dest_info		���O�������
 * @param   time        ��M�����̑����(���8bit�F��(0�`23)�A����8bit�F��(0�`59))
 * @param   log_no		  ���O�ԍ�
 *
 * @retval  BOOL		    TRUE:���O���擾�����@�@FALSE:�擾���s(�f�[�^������)
 */
//==================================================================
BOOL GAMEBEACON_InfoTbl_GetBeacon(GAMEBEACON_INFO_TBL *infotbl, GAMEBEACON_INFO *dest_info, u16 *time, int log_no)
{
  GF_ASSERT(log_no < GAMEBEACON_INFO_TBL_MAX);
  
  if(infotbl->info[log_no].version_bit == 0){
    return FALSE;
  }
  *dest_info = infotbl->info[log_no];
  *time = infotbl->time[log_no];
  return TRUE;
}

//==================================================================
/**
 * GAMEBEACON_INFO�\���̂�Alloc����
 *
 * @param   heap_id		
 *
 * @retval  GAMEBEACON_INFO *		
 */
//==================================================================
GAMEBEACON_INFO * GAMEBEACON_Alloc(HEAPID heap_id)
{
  return GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAMEBEACON_INFO));
}

//==================================================================
/**
 * GAMEBEACON_INFO_TBL�\���̂�Alloc����
 *
 * @param   heap_id		
 *
 * @retval  GAMEBEACON_INFO_TBL *		
 */
//==================================================================
GAMEBEACON_INFO_TBL * GAMEBEACON_InfoTbl_Alloc(HEAPID heap_id)
{
  return GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAMEBEACON_INFO_TBL));
}



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �v���C���[���ւ̃|�C���^���擾����
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  const STRCODE *		�v���C���[���ւ̃|�C���^
 */
//==================================================================
const STRCODE * GAMEBEACON_Get_PlayerName(const GAMEBEACON_INFO *info)
{
  return info->name;
}

//==================================================================
/**
 * ���C�ɓ���̐F���擾����
 *
 * @param   dest_buf		�F�̑����ւ̃|�C���^
 * @param   info        �r�[�R�����ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_Get_FavoriteColor(GXRgb *dest_buf, const GAMEBEACON_INFO *info)
{
  *dest_buf = info->favorite_color;
}

//==================================================================
/**
 * �g���[�i�[�̌����ڂ��擾����
 * @param   info  �r�[�R�����ւ̃|�C���^
 * @retval  u8		�g���[�i�[�̌�����
 */
//==================================================================
u8 GAMEBEACON_Get_TrainerView(const GAMEBEACON_INFO *info)
{
  return info->trainer_view;
}

//==================================================================
/**
 * ���ʂ��擾����
 * @param   info  �r�[�R�����ւ̃|�C���^
 * @retval  u8		����
 */
//==================================================================
u8 GAMEBEACON_Get_Sex(const GAMEBEACON_INFO *info)
{
  return info->sex;
}

//==================================================================
/**
 * �������Ă���G�p���[ID���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  GPOWER_ID		  G�p���[ID
 */
//==================================================================
GPOWER_ID GAMEBEACON_Get_GPowerID(const GAMEBEACON_INFO *info)
{
  return info->g_power_id;
}

//==================================================================
/**
 * ���ݒn���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  ���ݒn
 */
//==================================================================
u16 GAMEBEACON_Get_ZoneID(const GAMEBEACON_INFO *info)
{
  return info->zone_id;
}

//==================================================================
/**
 * ������󂯂��񐔂��擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  ������󂯂���
 */
//==================================================================
u16 GAMEBEACON_Get_ThanksRecvCount(const GAMEBEACON_INFO *info)
{
  return info->thanks_recv_count;
}

//==================================================================
/**
 * ����Ⴂ�l�����擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  ����Ⴂ�l��
 */
//==================================================================
u16 GAMEBEACON_Get_SuretigaiCount(const GAMEBEACON_INFO *info)
{
  return info->suretigai_count;
}

//==================================================================
/**
 * �ڍ׏��p�����[�^�擾�F�퓬����̃g���[�i�[�R�[�h
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  �g���[�i�[�R�[�h
 */
//==================================================================
u16 GAMEBEACON_Get_Details_BattleTrainer(const GAMEBEACON_INFO *info)
{
  switch(info->details.details_no){
  case GAMEBEACON_DETAILS_NO_BATTLE_TRAINER:
  case GAMEBEACON_DETAILS_NO_BATTLE_JIMLEADER:
  case GAMEBEACON_DETAILS_NO_BATTLE_BIGFOUR:
  case GAMEBEACON_DETAILS_NO_BATTLE_CHAMPION:
    return info->details.battle_trainer;
  }
  GF_ASSERT(0);
  return 0;
}

//==================================================================
/**
 * �ڍ׏��p�����[�^�擾�F�퓬����̃|�P����No
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  �|�P����No
 */
//==================================================================
u16 GAMEBEACON_Get_Details_BattleMonsNo(const GAMEBEACON_INFO *info)
{
  switch(info->details.details_no){
  case GAMEBEACON_DETAILS_NO_BATTLE_POKEMON:         ///<�쐶�|�P�����Ƒΐ풆
  case GAMEBEACON_DETAILS_NO_BATTLE_REGEND_POKEMON:  ///<�`���|�P�����Ƒΐ풆
  case GAMEBEACON_DETAILS_NO_BATTLE_VISION_POKEMON:  ///<���|�P�����Ƒΐ풆
    return info->details.battle_monsno;
  }
  GF_ASSERT(0);
  return 0;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�s��No
 *
 * @param   info		�r�[�R�����ւ̃|�C���^
 *
 * @retval  GAMEBEACON_ACTION_NULL		�s��No(GAMEBEACON_ACTION_xxx)
 */
//==================================================================
GAMEBEACON_ACTION GAMEBEACON_Get_Action_ActionNo(const GAMEBEACON_INFO *info)
{
  return info->action.action_no;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�A�C�e���ԍ�
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  �A�C�e���ԍ�
 */
//==================================================================
u16 GAMEBEACON_Get_Action_ItemNo(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_NULL:  //��check
  default:
    break;
  }
  return info->action.itemno;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�|�P�����̃j�b�N�l�[��
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  const STRCODE *		�j�b�N�l�[���ւ̃|�C���^
 */
//==================================================================
const STRCODE * GAMEBEACON_Get_Action_Nickname(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_POKE_EVOLUTION:
  case GAMEBEACON_ACTION_POKE_LVUP:
  case GAMEBEACON_ACTION_POKE_GET:
    return info->action.nickname;
  }
  GF_ASSERT(0);
  return info->action.nickname;
}
