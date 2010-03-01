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
#include "net_app\union\union_beacon_tool.h"

#include "app/townmap_data.h"
#include "arc/fieldmap/zone_id.h"
#include "tr_tool/trno_def.h"
#include "poke_tool/monsno_def.h"
#include "item/itemsym.h"
#include "app/research_radar/question_id.h"

enum{
 BEACON_WSET_DEFAULT,   //�f�t�H���g(�g���[�i�[��)  
 BEACON_WSET_TRNAME,    //�ΐ푊�薼
 BEACON_WSET_MONSNAME,  //�|�P�����푰��
 BEACON_WSET_NICKNAME,  //�|�P�����j�b�N�l�[��
 BEACON_WSET_POKE_W,  //�|�P�����j�b�N�l�[��
 BEACON_WSET_ITEM,      //�A�C�e����
 BEACON_WSET_PTIME,     //�v���C�^�C��
 BEACON_WSET_THANKS,    //����
 BEACON_WSET_HAIHU_MONS, //�z�z�����X�^�[��
 BEACON_WSET_HAIHU_ITEM, //�z�z�A�C�e����
 BEACON_WSET_MAX,
};

///�r�[�R�����ewordset�p�e���|�����o�b�t�@����`
#define BUFLEN_BEACON_WORDSET_TMP (16*2+EOM_SIZE)

/*
 *  @brief  GAMEBEACON_ACTION�^�̕��тƓ���ł���K�v������܂�
 */
static const u8 DATA_BeaconWordsetType[GAMEBEACON_ACTION_MAX] = {
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_NULL,                     ///<�f�[�^����
  
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_SEARCH,                   ///<�u������������T�[�`���܂����I�v      1
  BEACON_WSET_MONSNAME,	///<GAMEBEACON_ACTION_BATTLE_WILD_POKE_START,   ///<�쐶�̃|�P�����Ƒΐ���J�n���܂����I  2
  BEACON_WSET_MONSNAME,	///<GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY, ///<�쐶�̃|�P�����ɏ������܂����I        3
  BEACON_WSET_MONSNAME,	///<GAMEBEACON_ACTION_BATTLE_SP_POKE_START,     ///<���ʂȃ|�P�����Ƒΐ���J�n���܂����I  4
  BEACON_WSET_MONSNAME,	///<GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY,   ///<���ʂȃ|�P�����ɏ������܂����I        5
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_TRAINER_START,     ///<�g���[�i�[�Ƒΐ���J�n���܂����I      6
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY,   ///<�g���[�i�[�ɏ������܂����I            7
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_LEADER_START,      ///<�W�����[�_�[�Ƒΐ���J�n���܂����I    8
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY,    ///<�W�����[�_�[�ɏ������܂����I          9
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_BIGFOUR_START,     ///<�l�V���Ƒΐ���J�n���܂����I          10
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_BIGFOUR_VICTORY,   ///<�l�V���ɏ������܂����I                11
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_CHAMPION_START,    ///<�`�����s�I���Ƒΐ���J�n���܂����I    12
  BEACON_WSET_TRNAME,	///<GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY,  ///<�`�����s�I���ɏ������܂����I          13
  BEACON_WSET_MONSNAME,	///<GAMEBEACON_ACTION_POKE_GET,                 ///<�|�P�����ߊl                          14
  BEACON_WSET_MONSNAME,	///<GAMEBEACON_ACTION_SP_POKE_GET,              ///<���ʂȃ|�P�����ߊl                    15
  BEACON_WSET_NICKNAME,	///<GAMEBEACON_ACTION_POKE_LVUP,                ///<�|�P�������x���A�b�v                  16
  BEACON_WSET_POKE_W,	///<GAMEBEACON_ACTION_POKE_EVOLUTION,           ///<�|�P�����i��                          17
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_GPOWER,                   ///<G�p���[����                           18
  BEACON_WSET_ITEM,	  ///<GAMEBEACON_ACTION_SP_ITEM_GET,              ///<�M�d�i�Q�b�g                          19
  BEACON_WSET_PTIME,	///<GAMEBEACON_ACTION_PLAYTIME,                 ///<���̃v���C���Ԃ��z����              20
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_ZUKAN_COMPLETE,           ///<�}�ӊ���                              21
  BEACON_WSET_THANKS,	///<GAMEBEACON_ACTION_THANKYOU_OVER,            ///<������󂯂��񐔂��K�萔�𒴂���      22
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_UNION_IN,                 ///<���j�I�����[���ɓ�����                23
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_THANKYOU,                 ///<�u���肪�Ƃ��I�v                      24
  BEACON_WSET_HAIHU_MONS,	///<GAMEBEACON_ACTION_DISTRIBUTION_POKE,        ///<�|�P�����z�z��                        25
  BEACON_WSET_HAIHU_ITEM,	///<GAMEBEACON_ACTION_DISTRIBUTION_ITEM,        ///<�A�C�e���z�z��                        26
  BEACON_WSET_DEFAULT,	///<GAMEBEACON_ACTION_DISTRIBUTION_ETC,         ///<���̑��z�z��                          27
};

typedef BOOL (*BEACON_INFO_ERROR_CHECK_FUNC)(const GAMEBEACON_INFO* info );

static BOOL errchk_action_default(const GAMEBEACON_INFO* info );
static BOOL errchk_action_trainer_battle(const GAMEBEACON_INFO* info );
static BOOL errchk_action_poke_monsno(const GAMEBEACON_INFO* info );
static BOOL errchk_action_poke_nickname(const GAMEBEACON_INFO* info );
static BOOL errchk_action_poke_double(const GAMEBEACON_INFO* info );
static BOOL errchk_action_itemno(const GAMEBEACON_INFO* info );
static BOOL errchk_action_playtime(const GAMEBEACON_INFO* info );
static BOOL errchk_action_thanks_count(const GAMEBEACON_INFO* info );
static BOOL errchk_action_haifu_mons(const GAMEBEACON_INFO* info );
static BOOL errchk_action_haifu_item(const GAMEBEACON_INFO* info );

static const BEACON_INFO_ERROR_CHECK_FUNC DATA_ErrorCheckFuncTbl[BEACON_WSET_MAX] = {
  errchk_action_default,
  errchk_action_trainer_battle,
  errchk_action_poke_monsno,
  errchk_action_poke_nickname,
  errchk_action_poke_double,
  errchk_action_itemno,
  errchk_action_playtime,
  errchk_action_thanks_count,
  errchk_action_haifu_mons,
  errchk_action_haifu_item,
};

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
 * �X�^�b�N�e�[�u���Ɏc���Ă������ʐM�o�b�t�@�֖߂�
 *
 * @param   stack_infotbl		�X�^�b�N�e�[�u���ւ̃|�C���^
 *
 * �X�^�b�N�Ɏc���Ă������ΏۂɁAGAMEBEACON_Get_UpdateLogNo�ŐV���Ƃ��ĂЂ��������Ԃɂ��܂�
 * ���X�^�b�N�e�[�u����j�����鎞�Ɏg�p������̂ł�
 */
//==================================================================
void GAMEBEACON_Stack_PutBack(const GAMEBEACON_INFO_TBL *stack_infotbl)
{
  int log_no;
  
  for(log_no = 0; log_no < GAMEBEACON_INFO_TBL_MAX; log_no++){
    if(stack_infotbl->info[log_no].version_bit == 0){
      return;
    }
    GAMEBEACON_Set_SearchUpdateFlag(&stack_infotbl->info[log_no]);
  }
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
      infotbl->time[log_no] = time;
      return log_no;
    }
  }
  
  if(push_out == TRUE){
    //�X�^�b�N�e�[�u����O�l��
    MI_CpuCopy32( &infotbl->info[1], &infotbl->info[0], sizeof(GAMEBEACON_INFO)*GAMEBEACON_INFO_TBL_END);
    MI_CpuCopy16( &infotbl->time[1], &infotbl->time[0], sizeof(u16)*GAMEBEACON_INFO_TBL_END);
    infotbl->info[GAMEBEACON_INFO_TBL_END] = *set_info;
    infotbl->time[GAMEBEACON_INFO_TBL_END] = time;
    return GAMEBEACON_INFO_TBL_END;
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
 * �����O���r�[�R���e�[�u����Top����̃I�t�Z�b�gindex��z��ւ̎��Q��index�ɕϊ�
 *
 * @param   infotbl		  �r�[�R���e�[�u���ւ̃|�C���^
 * @param   ofs         �����O�g�b�v�����index�I�t�Z�b�g
 *
 * @retval  int		�z��̎��Q��index
 */
//==================================================================
int GAMEBEACON_InfoTblRing_Ofs2Idx(GAMEBEACON_INFO_TBL *infotbl, int ofs )
{
  int top = infotbl->ring_top;

  if( ofs < 0 ){
    return ((top+ofs+GAMEBEACON_INFO_TBL_MAX)%GAMEBEACON_INFO_TBL_MAX);
  }else{
    return ((top+ofs)%GAMEBEACON_INFO_TBL_MAX);
  }
}

//==================================================================
/**
 * �r�[�R���e�[�u���֐V�����r�[�R����o�^����(�����O�o�b�t�@����)
 *
 * @param   infotbl		  �r�[�R���e�[�u���ւ̃|�C���^
 * @param   set_info		�o�^����r�[�R��
 * @param   time        ��M����(���8bit�F��(0�`23)�A����8bit�F��(0�`59))
 *  @param  new_f       �V�����ǂ���(NULL��)
 * @retval  int		�o�^�������O�ʒu(�o�^�ł��Ȃ������ꍇ��GAMEBEACON_INFO_TBL_MAX)
 * 
 * ���Ƀe�[�u���ɑ��݂��Ă��郆�[�U�[�̏ꍇ�͏㏑���B
 * �V�K���[�U�[�̏ꍇ�̓e�[�u���̍Ō�ɒǉ�
 */
//==================================================================
int GAMEBEACON_InfoTblRing_SetBeacon(GAMEBEACON_INFO_TBL *infotbl, const GAMEBEACON_INFO *set_info, u16 time, BOOL* new_f)
{
  int i, log_no;
  
  //�t�ɃT�[�`
  if(new_f != NULL){
    *new_f = 0;
  }
  for( i = 0;i < GAMEBEACON_INFO_TBL_MAX;i++){  //�����f�[�^�`�F�b�N
    log_no = GAMEBEACON_InfoTblRing_Ofs2Idx(infotbl, i);

    if( infotbl->info[log_no].version_bit == 0 ){
      break;
    }
    if( infotbl->info[log_no].trainer_id == set_info->trainer_id ){

      infotbl->info[log_no] = *set_info;
      infotbl->time[log_no] = time;
      return i;
    }
  }
  //�������X�g�ɂȂ���ΐV�K�ǉ�
  if(new_f != NULL){
    *new_f = TRUE;
  }
  //��ԌÂ�����㏑��
  log_no =  GAMEBEACON_InfoTblRing_Ofs2Idx(infotbl, -1);
  infotbl->info[log_no] = *set_info;
  infotbl->time[log_no] = time;

  //�����O�g�b�v���X�V
  infotbl->ring_top =  log_no;
  infotbl->entry_num++;
  return 0;
}

//==================================================================
/**
 * �����O���r�[�R���e�[�u������w��I�t�Z�b�g�ʒu�̃r�[�R�������擾����
 *
 * @param   infotbl		  �r�[�R���e�[�u���ւ̃|�C���^
 * @param   dest_info		���O�������
 * @param   time        ��M�����̑����(���8bit�F��(0�`23)�A����8bit�F��(0�`59))
 * @param   ofs		      �����O�o�b�t�@�擪�����index�I�t�Z�b�g
 *
 * @retval  BOOL		    TRUE:���O���擾�����@�@FALSE:�擾���s(�f�[�^������)
 */
//==================================================================
BOOL GAMEBEACON_InfoTblRing_GetBeacon(GAMEBEACON_INFO_TBL *infotbl, GAMEBEACON_INFO *dest_info, u16 *time, int ofs )
{
  int log_no;
  GF_ASSERT( ofs < GAMEBEACON_INFO_TBL_MAX);
  
  log_no = GAMEBEACON_InfoTblRing_Ofs2Idx( infotbl, ofs );
  if(infotbl->info[log_no].version_bit == 0){
    return FALSE;
  }
  *dest_info = infotbl->info[log_no];
  *time = infotbl->time[log_no];
  return TRUE;
}

//==================================================================
/**
 * �����O���r�[�R���e�[�u���ɃG���g���[���Ă���r�[�R�������擾
 *
 * @param   infotbl	  �r�[�R���e�[�u���ւ̃|�C���^
 * @retval  int		    �r�[�R���o�^��
 */
//==================================================================
int GAMEBEACON_InfoTblRing_GetEntryNum(GAMEBEACON_INFO_TBL *infotbl )
{
  return infotbl->entry_num;
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
  GAMEBEACON_INFO_TBL* tbl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAMEBEACON_INFO_TBL));
  tbl->ring_top = 0; 
  tbl->entry_num = 0;
  return tbl;
}



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * ��{�f�[�^�G���[�`�F�b�N 
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  TRUE  �G���[�����o
 */
//==================================================================
BOOL GAMEBEACON_Check_Error(const GAMEBEACON_INFO *info)
{
  //�A�N�V����No�`�F�b�N
  if( info->action.action_no == GAMEBEACON_ACTION_NULL ||
      info->action.action_no >= GAMEBEACON_ACTION_MAX ) {
    return TRUE;
  }
  //�ڍ׃R�[�h�i���o�[
  if( info->details.details_no >= GAMEBEACON_DETAILS_NO_MAX ){
    return TRUE;
  }
  //�������������N
  if( info->research_team_rank >= RESEARCH_TEAM_RANK_MAX ){
    return TRUE;
  }
  //�v���C����
  if( info->play_hour >= 999 || info->play_min >= 59 ){
    return TRUE;
  }
  //G�p���[ID�`�F�b�N
  if( info->g_power_id > GPOWER_ID_MAX ) {
    return TRUE;
  }
  //�]�[��ID�`�F�b�N
  if( info->zone_id >= ZONE_ID_MAX || info->townmap_root_zone_id >= ZONE_ID_MAX ){
    return TRUE;
  }
  //����Ⴂ�l�������J�E���g
  if( info->suretigai_count > 99999 || info->thanks_recv_count > 99999 ){
    return TRUE;
  }
  //�A�N�V�����ڍ׃f�[�^�`�F�b�N
  if( (DATA_ErrorCheckFuncTbl[DATA_BeaconWordsetType[ info->action.action_no ]])(info) ){
    return TRUE;
  }
  return FALSE; //@todo �Ȃɂ������̏�����NPC���������
}


//==================================================================
/**
 * NPC�f�[�^���ǂ����`�F�b�N 
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  TRUE  NPC�f�[�^(=�ڍ׏��Ȃ�)
 */
//==================================================================
BOOL GAMEBEACON_Check_NPC(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_DISTRIBUTION_POKE:        ///<�|�P�����z�z��
  case GAMEBEACON_ACTION_DISTRIBUTION_ITEM:        ///<�A�C�e���z�z��
  case GAMEBEACON_ACTION_DISTRIBUTION_ETC:        ///<���̑��z�z��
    return TRUE;
  }
  return FALSE; //@todo �Ȃɂ������̏�����NPC���������
}

//==================================================================
/**
 * �v���C���[����STRBUF�Ɏ擾����
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  const STRCODE *		�v���C���[���ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_Get_PlayerNameToBuf(const GAMEBEACON_INFO *info, STRBUF* strbuf)
{
  GFL_STR_SetStringCodeOrderLength( strbuf, info->name, PERSON_NAME_SIZE+EOM_SIZE );
}

//==================================================================
/**
 * PM_VERSION���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  int	  	PM_VERSION
 */
//==================================================================
int GAMEBEACON_Get_PmVersion(const GAMEBEACON_INFO *info)
{
  return info->pm_version;
}

//==================================================================
/**
 * �Z��ł��鍑���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  int		  ���ԍ�
 */
//==================================================================
int GAMEBEACON_Get_Nation(const GAMEBEACON_INFO *info)
{
  return info->nation;
}

//==================================================================
/**
 * �Z��ł���n����擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  int		  �n��ԍ�
 */
//==================================================================
int GAMEBEACON_Get_Area(const GAMEBEACON_INFO *info)
{
  return info->area;
}

//==================================================================
/**
 * �d��ID���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u8		  �d��ID (�ݒ肳��Ă��Ȃ��ꍇ��GAMEBEACON_DETAILS_JOB_NULL)
 */
//==================================================================
u8 GAMEBEACON_Get_Job(const GAMEBEACON_INFO *info)
{
  u32 answer;
  
  answer = QuestionnaireAnswer_ReadBit(&info->question_answer, QUESTION_ID_JOB);
  if(answer == 0){
    return GAMEBEACON_DETAILS_JOB_NULL;
  }
  return answer - 1;  //�񓚂͖��񓚂�"0"���܂߂�0�n�܂�ׁ̈A�d��ID�ƈ�v������ׂ�-1
}

//==================================================================
/**
 * �ID���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u8		  �ID (�ݒ肳��Ă��Ȃ��ꍇ��GAMEBEACON_DETAILS_HOBBY_NULL)
 */
//==================================================================
u8 GAMEBEACON_Get_Hobby(const GAMEBEACON_INFO *info)
{
  u32 answer;
  
  answer = QuestionnaireAnswer_ReadBit(&info->question_answer, QUESTION_ID_HOBBY);
  if(answer == 0){
    return GAMEBEACON_DETAILS_HOBBY_NULL;
  }
  return answer - 1;  //�񓚂͖��񓚂�"0"���܂߂�0�n�܂�ׁ̈A�ID�ƈ�v������ׂ�-1
}

//==================================================================
/**
 * ���ȏЉ�ȈՉ�b���擾
 *
 * @param   info		    �r�[�R�����ւ̃|�C���^
 * @param   dest_pms		�����
 */
//==================================================================
void GAMEBEACON_Get_IntroductionPms(const GAMEBEACON_INFO *info, PMS_DATA *dest_pms)
{
  dest_pms->sentence_type = info->details.sentence_type;
  dest_pms->sentence_id = info->details.sentence_id;
  dest_pms->word[0] = info->details.word[0];
  dest_pms->word[1] = info->details.word[1];
}

//==================================================================
/**
 * �������������N���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  RESEARCH_TEAM_RANK		�������������N
 */
//==================================================================
RESEARCH_TEAM_RANK GAMEBEACON_Get_ResearchTeamRank(const GAMEBEACON_INFO *info)
{
  return info->research_team_rank;
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
  *dest_buf = OS_GetFavoriteColor(info->favorite_color_index);
}

//==================================================================
/**
 * �g���[�i�[ID���擾����
 * @param   info  �r�[�R�����ւ̃|�C���^
 * @retval  u8		�g���[�i�[�̌�����
 */
//==================================================================
u32 GAMEBEACON_Get_TrainerID(const GAMEBEACON_INFO *info)
{
  return info->trainer_id;
}


//==================================================================
/**
 * �g���[�i�[�̌����ڂ��擾����
 * @param   info  �r�[�R�����ւ̃|�C���^
 * @retval  u8		�g���[�i�[�̌�����
 *
 * �����ʂ����݂ŁA0-15��ID��Ԃ�
 */
//==================================================================
u8 GAMEBEACON_Get_TrainerView(const GAMEBEACON_INFO *info)
{
  if(info->sex == PM_MALE){
    return info->trainer_view;
  }
  return info->trainer_view + UNION_VIEW_INDEX_WOMAN_START;
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
 * �v���C���Ԃ��擾����
 *
 * @param   info		    �r�[�R�����ւ̃|�C���^
 * @param   dest_hour		�����F��
 * @param   dest_min		�����F��
 */
//==================================================================
void GAMEBEACON_Get_PlayTime(const GAMEBEACON_INFO *info, u16 *dest_hour, u16 *dest_min)
{
  *dest_hour = info->play_hour;
  *dest_min = info->play_min;
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
 * �^�E���}�b�v�p��ROOT_ZONE_ID���擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  ���ݒn
 */
//==================================================================
u16 GAMEBEACON_Get_TownmapRootZoneID(const GAMEBEACON_INFO *info)
{
  return info->townmap_root_zone_id;
}

//==================================================================
/**
 * ������󂯂��񐔂��擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u32		  ������󂯂���
 */
//==================================================================
u32 GAMEBEACON_Get_ThanksRecvCount(const GAMEBEACON_INFO *info)
{
  return info->thanks_recv_count;
}

//==================================================================
/**
 * ����Ⴂ�l�����擾
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u32		  ����Ⴂ�l��
 */
//==================================================================
u32 GAMEBEACON_Get_SuretigaiCount(const GAMEBEACON_INFO *info)
{
  return info->suretigai_count;
}

//==================================================================
/**
 * ���ȏЉ�b�Z�[�W���擾
 *
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @param   dest		���b�Z�[�W�����
 */
//==================================================================
void GAMEBEACON_Get_SelfIntroduction(const GAMEBEACON_INFO *info, STRBUF *dest)
{
  GFL_STR_SetStringCodeOrderLength( dest,
    info->self_introduction, GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN+EOM_SIZE );
}

//==================================================================
/**
 * ���烁�b�Z�[�W���擾
 *
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @param   dest		���b�Z�[�W�����
 */
//==================================================================
void GAMEBEACON_Get_ThankyouMessage(const GAMEBEACON_INFO *info, STRBUF *dest)
{
  GF_ASSERT(info->action.action_no == GAMEBEACON_ACTION_THANKYOU);
  GFL_STR_SetStringCode( dest, info->action.thankyou_message );
}

//==================================================================
/**
 * �ڍ׏��p�����[�^�擾�F�ڍ׏��No
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  GAMEBEACON_DETAILS_NO �ڍ�No
 */
//==================================================================
GAMEBEACON_DETAILS_NO GAMEBEACON_Get_Details_DetailsNo(const GAMEBEACON_INFO *info)
{
  return info->details.details_no;
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
  case GAMEBEACON_DETAILS_NO_BATTLE_WILD_POKE:        ///<�쐶�|�P�����Ƒΐ풆
  case GAMEBEACON_DETAILS_NO_BATTLE_SP_POKE:          ///<���ʂȃ|�P�����Ƒΐ풆
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
 * �s���p�����[�^�擾�F�|�P�����ԍ�
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		�|�P�����ԍ�
 */
//==================================================================
u16 GAMEBEACON_Get_Action_Monsno(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_BATTLE_WILD_POKE_START:
  case GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY:
  case GAMEBEACON_ACTION_BATTLE_SP_POKE_START:
  case GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY:
  case GAMEBEACON_ACTION_POKE_GET:
  case GAMEBEACON_ACTION_SP_POKE_GET:
  case GAMEBEACON_ACTION_POKE_EVOLUTION:
    return info->action.monsno;
  }
  GF_ASSERT(0);
  return info->action.monsno;
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
  case GAMEBEACON_ACTION_SP_ITEM_GET:
  case GAMEBEACON_ACTION_USE_ITEM:
    return info->action.itemno;
  }
  GF_ASSERT(0);
  return info->action.itemno;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�Z�ԍ�
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  �Z�ԍ�
 */
//==================================================================
u16 GAMEBEACON_Get_Action_WazaNo(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_FIELD_SKILL:
    return info->action.wazano;
  }
  GF_ASSERT(0);
  return info->action.wazano;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�z�z�|�P�����ԍ�
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		�|�P�����ԍ�
 */
//==================================================================
u16 GAMEBEACON_Get_Action_DistributionMonsno(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_DISTRIBUTION_POKE:          ///<�|�P�����z�z��
    return info->action.distribution.monsno;
  }
  GF_ASSERT(0);
  return info->action.distribution.monsno;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�z�z�A�C�e���ԍ�
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16		  �A�C�e���ԍ�
 */
//==================================================================
u16 GAMEBEACON_Get_Action_DistributionItemNo(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_DISTRIBUTION_ITEM:
    return info->action.distribution.itemno;
  }
  GF_ASSERT(0);
  return info->action.distribution.itemno;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�|�P�����̃j�b�N�l�[��
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  const STRCODE *		�j�b�N�l�[���ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_Get_Action_Nickname(const GAMEBEACON_INFO *info, STRBUF *dest)
{
  GFL_STR_SetStringCodeOrderLength( dest,
    info->action.normal.nickname, MONS_NAME_SIZE+EOM_SIZE );

  switch(info->action.action_no){
  case GAMEBEACON_ACTION_POKE_EVOLUTION:
  case GAMEBEACON_ACTION_POKE_LVUP:
  case GAMEBEACON_ACTION_CRITICAL_HIT:
  case GAMEBEACON_ACTION_CRITICAL_DAMAGE:
  case GAMEBEACON_ACTION_HP_LITTLE:
  case GAMEBEACON_ACTION_PP_LITTLE:
  case GAMEBEACON_ACTION_DYING:
  case GAMEBEACON_ACTION_STATE_IS_ABNORMAL:
  case GAMEBEACON_ACTION_SODATEYA_EGG:
  case GAMEBEACON_ACTION_EGG_HATCH:
  case GAMEBEACON_ACTION_MUSICAL:
    return;
  }
  GF_ASSERT(0);
  return;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�g���[�i�[�ԍ�
 * @param   info		�r�[�R�����ւ̃|�C���^
 * @retval  u16 		�g���[�i�[�ԍ�
 */
//==================================================================
u16 GAMEBEACON_Get_Action_TrNo(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_BATTLE_TRAINER_START:
  case GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY:
  case GAMEBEACON_ACTION_BATTLE_LEADER_START:
  case GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY:
  case GAMEBEACON_ACTION_BATTLE_BIGFOUR_START:
  case GAMEBEACON_ACTION_BATTLE_BIGFOUR_VICTORY:
  case GAMEBEACON_ACTION_BATTLE_CHAMPION_START:
  case GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY:
    return info->action.tr_no;
  }
  return info->action.tr_no;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F������󂯂���
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  u32		������󂯂���
 */
//==================================================================
u32 GAMEBEACON_Get_Action_ThankyouCount(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_THANKYOU_OVER:
    return info->action.thankyou_count;
  }
  GF_ASSERT(0);
  return info->action.thankyou_count;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�o�ߎ���
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  u32		�o�ߎ���(��)
 */
//==================================================================
u32 GAMEBEACON_Get_Action_Hour(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_PLAYTIME:
    return info->action.hour;
  }
  GF_ASSERT(0);
  return info->action.hour;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�A����
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  u32		�o�ߎ���(��)
 */
//==================================================================
u32 GAMEBEACON_Get_Action_VictoriCount(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_SUBWAY_STRAIGHT_VICTORIES:
  case GAMEBEACON_ACTION_SUBWAY_VICTORIES_ACHIEVE:
    return info->action.victory_count;
  }
  GF_ASSERT(0);
  return info->action.victory_count;
}

//==================================================================
/**
 * �s���p�����[�^�擾�F�g���C�A���n�E�X�����N
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  u8		TH_RANK_xxx
 */
//==================================================================
u8 GAMEBEACON_Get_Action_TrialHouseRank(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_TRIALHOUSE_RANK:
    return info->action.trial_house_rank;
  }
  GF_ASSERT(0);
  return info->action.trial_house_rank;
}

//==================================================================
/**
 * �s���p�����[�^�擾�FG�p���[
 * @param   info	�r�[�R�����ւ̃|�C���^
 * @retval  u16		G�p���[ID
 */
//==================================================================
u16 GAMEBEACON_Get_Action_GPowerID(const GAMEBEACON_INFO *info)
{
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_OTHER_GPOWER_USE:
    return info->action.gpower_id;
  }
  GF_ASSERT(0);
  return info->action.gpower_id;
}

//==================================================================
/**
 * �r�[�R�����̓��e��WORDSET����
 *
 * @param   info		        �Ώۂ̃r�[�R�����ւ̃|�C���^
 * @param   wordset		
 * @param   temp_heap_id		�����Ńe���|�����Ƃ��Ďg�p����q�[�vID
 */
//==================================================================
void GAMEBEACON_InfoWordset(const GAMEBEACON_INFO *info, WORDSET *wordset, HEAPID temp_heap_id)
{
  u8 type;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BUFLEN_BEACON_WORDSET_TMP , temp_heap_id);

  //�g���[�i�[���W�J(�f�t�H���g)
  GAMEBEACON_Get_PlayerNameToBuf(info, strbuf);
  WORDSET_RegisterWord( wordset, 0, strbuf, 0, TRUE, PM_LANG);

  {
    GAMEBEACON_ACTION action = GAMEBEACON_Get_Action_ActionNo(info);
    type = DATA_BeaconWordsetType[ action ];
  }

  switch( type ){
  case BEACON_WSET_TRNAME:
    WORDSET_RegisterTrainerName( wordset, 1, GAMEBEACON_Get_Action_TrNo(info) );
    break;
  case BEACON_WSET_MONSNAME:
  case BEACON_WSET_POKE_W:
    WORDSET_RegisterPokeMonsNameNo( wordset, 1,GAMEBEACON_Get_Action_Monsno(info));
    if( type == BEACON_WSET_MONSNAME ){
      break;
    }
    //�u���C�N�X���[
  case BEACON_WSET_NICKNAME:
    GAMEBEACON_Get_Action_Nickname( info, strbuf );
    WORDSET_RegisterWord(wordset, 2, strbuf, 0, TRUE, PM_LANG);
    break;
  case BEACON_WSET_ITEM:
    WORDSET_RegisterItemName( wordset, 1, GAMEBEACON_Get_Action_ItemNo(info) );
    break;
  case BEACON_WSET_PTIME:
    WORDSET_RegisterNumber( wordset, 1, GAMEBEACON_Get_Action_Hour(info), 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    break;
  case BEACON_WSET_THANKS:
    WORDSET_RegisterNumber( wordset, 1, GAMEBEACON_Get_Action_ThankyouCount(info), 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    break;
  case BEACON_WSET_HAIHU_MONS:
    WORDSET_RegisterItemName( wordset, 1, GAMEBEACON_Get_Action_DistributionMonsno(info));
    break;
  case BEACON_WSET_HAIHU_ITEM:
    WORDSET_RegisterItemName( wordset, 1, GAMEBEACON_Get_Action_DistributionItemNo(info));
    break;
  }
  GFL_STR_DeleteBuffer(strbuf);
}


///////////////////////////////////////////////////////////////////////////////////
/*
 *  ���[�J���֐�
 */
///////////////////////////////////////////////////////////////////////////////////

//�G���[�`�F�b�N �f�t�H���g�^�C�v
static BOOL errchk_action_default(const GAMEBEACON_INFO* info )
{
  return FALSE;
}
//�G���[�`�F�b�N �g���[�i�[�ΐ�^�C�v
static BOOL errchk_action_trainer_battle(const GAMEBEACON_INFO* info )
{
  if( info->action.tr_no == TRID_NULL || info->action.tr_no >= TRID_MAX ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N �|�P�����푰���`�F�b�N�^�C�v
static BOOL errchk_action_poke_monsno(const GAMEBEACON_INFO* info )
{
  if( info->action.monsno == 0 || 
      info->action.monsno > MONSNO_END ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N �|�P�����j�b�N�l�[���`�F�b�N�^�C�v
static BOOL errchk_action_poke_nickname(const GAMEBEACON_INFO* info )
{
  return FALSE;
}
//�G���[�`�F�b�N �푰���ƃ|�P�����j�b�N�l�[���`�F�b�N�^�C�v
static BOOL errchk_action_poke_double(const GAMEBEACON_INFO* info )
{
  if( info->action.monsno == 0 || 
      info->action.monsno > MONSNO_END ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N �A�C�e��No�`�F�b�N�^�C�v
static BOOL errchk_action_itemno(const GAMEBEACON_INFO* info )
{
  if( info->action.itemno == ITEM_DUMMY_DATA || info->action.itemno > ITEM_DATA_MAX ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N �v���C�^�C���^�C�v
static BOOL errchk_action_playtime(const GAMEBEACON_INFO* info )
{
  if( info->play_hour > 999 | info->play_min > 59 ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N ���񐔃^�C�v
static BOOL errchk_action_thanks_count(const GAMEBEACON_INFO* info )
{
  if( info->action.thankyou_count > 99999 ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N �z�z�����X�^�[���^�C�v
static BOOL errchk_action_haifu_mons(const GAMEBEACON_INFO* info )
{
  if( info->action.distribution.monsno  == 0 ||
      info->action.distribution.monsno > MONSNO_END ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}
//�G���[�`�F�b�N �z�z�A�C�e���^�C�v
static BOOL errchk_action_haifu_item(const GAMEBEACON_INFO* info )
{
  if( info->action.distribution.itemno  == ITEM_DUMMY_DATA ||
      info->action.distribution.itemno > ITEM_DATA_MAX ){
    GF_ASSERT(0);
    return TRUE;
  }
  return FALSE;
}




