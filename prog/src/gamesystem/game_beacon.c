//==============================================================================
/**
 * @file    game_beacon.c
 * @brief   �Q�[�����̃r�[�R�����Ǘ��V�X�e��
 * @author  matsuda
 * @date    2009.12.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/game_data.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "msg/msg_beacon_status.h"
#include "savedata/encount_sv.h"
#include "savedata/misc.h"
#include "net/net_whpipe.h"
#include "savedata/my_pms_data.h"
#include "app/townmap_util.h"
#include "net_app\union\union_beacon_tool.h"
#include "app/research_radar/research_data.h"   //QUESTION_NUM_PER_TOPIC
#include "app/research_radar/question_id.h"
#include "app/research_radar/questionnaire_index.h"
#include "savedata/playtime.h"
#include "field/research_team_def.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�z�M�p�}�W�b�N�L�[�F�|�P����
#define MAGIC_KEY_DISTRIBUTION_POKE    (0x2932013a)
///�z�M�p�}�W�b�N�L�[�F�A�C�e��
#define MAGIC_KEY_DISTRIBUTION_ITEM    (0x48841dc1)
///�z�M�p�}�W�b�N�L�[�F���̑�
#define MAGIC_KEY_DISTRIBUTION_ETC     (0x701ddc92)
///�z�M�p�}�W�b�N�L�[�FG�p���[
#define MAGIC_KEY_DISTRIBUTION_GPOWER  (0xfee48201)

//������ASSERT�ɂЂ�����������update_log��bit�Ǘ��ł͂Ȃ��z��Ǘ��ɕύX����
SDK_COMPILER_ASSERT(GAMEBEACON_SYSTEM_LOG_MAX < 32);


//==============================================================================
//  �\���̒�`
//==============================================================================
///1���̃��O�f�[�^
typedef struct{
  GAMEBEACON_INFO info;
  u16 time;                           ///<�ŐV�̃f�[�^���󂯎���Ă���o�߂��Ă���t���[����
  u16 padding;
}GAMEBEACON_LOG;

///���M�r�[�R���Ǘ�
typedef struct{
  GAMEBEACON_INFO info;               ///<���M�r�[�R��
  s16 life;                           ///<���M����
  u8 beacon_update;                   ///<TRUE:���M�r�[�R���X�V
  u8 padding;
}GAMEBEACON_SEND_MANAGER;

///�Q�[���r�[�R���Ǘ��V�X�e���̃|�C���^
typedef struct{
  GAMEDATA *gamedata;
  GAMEBEACON_SEND_MANAGER send;       ///<���M�r�[�R���Ǘ�
  GAMEBEACON_LOG log[GAMEBEACON_SYSTEM_LOG_MAX]; ///<���O(�`�F�[���o�b�t�@)
  u32 update_log;                     ///<�X�V�̂��������O�ԍ���bit�ŊǗ�(32�𒴂�����ς���)
  u32 log_count;                      ///<���O�S�̂̎�M�������J�E���g
  s8 start_log;                       ///<���O�̃`�F�[���J�n�ʒu
  s8 end_log;                         ///<���O�̃`�F�[���I�[�ʒu
  s8 log_num;                         ///<���O����
  u8 new_entry;                       ///<TRUE:�V�����l���Ƃ�������
}GAMEBEACON_SYSTEM;


//==============================================================================
//  ���[�J���ϐ�
//==============================================================================
///�Q�[���r�[�R���Ǘ��V�X�e���̃|�C���^
static GAMEBEACON_SYSTEM * GameBeaconSys = NULL;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void SendBeacon_Update(GAMEBEACON_SEND_MANAGER *send);
static void BeaconInfo_Set(GAMEBEACON_SYSTEM *bsys, const GAMEBEACON_INFO *info);
static GAMEBEACON_INFO * BeaconInfo_Get(GAMEBEACON_SYSTEM *bsys, int log_no);
static void SendBeacon_Init(GAMEBEACON_SEND_MANAGER *send, GAMEDATA * gamedata);
static void SendBeacon_SetCommon(GAMEBEACON_SEND_MANAGER *send);
static void GAMEBEACON_Set_ThanksRecvCount(u32 count);
static void GAMEBEACON_Set_SuretigaiCount(u32 count);

void BEACONINFO_Set_BattleWildPokeStart(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_BattleWildPokeVictory(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_BattleSpPokeStart(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_BattleSpPokeVictory(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_BattleTrainerStart(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleLeaderStart(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleLeaderVictory(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleSpTrainerStart(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleSpTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleChampionStart(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleChampionVictory(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_WildPokemonGet(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_SpecialPokemonGet(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_PokemonLevelUp(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_PokemonEvolution(GAMEBEACON_INFO *info, u16 monsno, const STRBUF *nickname);
void BEACONINFO_Set_GPower(GAMEBEACON_INFO *info, GPOWER_ID g_power_id);
void BEACONINFO_Set_SpItemGet(GAMEBEACON_INFO *info, u16 item);
void BEACONINFO_Set_PlayTime(GAMEBEACON_INFO *info, u32 hour);
void BEACONINFO_Set_ZukanComplete(GAMEBEACON_INFO *info);
void BEACONINFO_Set_ThankyouOver(GAMEBEACON_INFO *info, u32 thankyou_count);
void BEACONINFO_Set_UnionIn(GAMEBEACON_INFO *info);
#ifdef PM_DEBUG
void BEACONINFO_Set_DistributionPoke(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_DistributionItem(GAMEBEACON_INFO *info, u16 item);
void BEACONINFO_Set_DistributionEtc(GAMEBEACON_INFO *info);
void BEACONINFO_Set_DistributionGPower(GAMEBEACON_INFO *info, GPOWER_ID g_power_id);
#endif
void BEACONINFO_Set_CriticalHit(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_CriticalDamage(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_Escape(GAMEBEACON_INFO *info);
void BEACONINFO_Set_HPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_PPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_Dying(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_StateIsAbnormal(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_UseItem(GAMEBEACON_INFO *info, u16 item_no);
void BEACONINFO_Set_FieldSkill(GAMEBEACON_INFO *info, u16 wazano);
void BEACONINFO_Set_SodateyaEgg(GAMEBEACON_INFO *info);
void BEACONINFO_Set_EggHatch(GAMEBEACON_INFO *info, u16 monsno );
void BEACONINFO_Set_Shoping(GAMEBEACON_INFO *info);
void BEACONINFO_Set_Subway(GAMEBEACON_INFO *info);
void BEACONINFO_Set_SubwayStraightVictories(GAMEBEACON_INFO *info, u32 victory_count);
void BEACONINFO_Set_SubwayTrophyGet(GAMEBEACON_INFO *info);
void BEACONINFO_Set_TrialHouse(GAMEBEACON_INFO *info);
void BEACONINFO_Set_TrialHouseRank(GAMEBEACON_INFO *info, u8 rank);
void BEACONINFO_Set_FerrisWheel(GAMEBEACON_INFO *info);
void BEACONINFO_Set_PokeShifter(GAMEBEACON_INFO *info);
void BEACONINFO_Set_Musical(GAMEBEACON_INFO *info, const STRBUF *nickname);
void BEACONINFO_Set_OtherGPowerUse(GAMEBEACON_INFO *info, GPOWER_ID gpower_id);
void BEACONINFO_Set_Thankyou(GAMEBEACON_INFO *info, GAMEDATA *gamedata, u32 target_trainer_id);
void BEACONINFO_Set_FreeWord(GAMEBEACON_INFO *info, const STRBUF *free_word);
void BEACONINFO_Set_ZoneChange(GAMEBEACON_INFO *info, ZONEID zone_id, const GAMEDATA *cp_gamedata);
void BEACONINFO_Set_ThanksRecvCount(GAMEBEACON_INFO *info, u32 count);
void BEACONINFO_Set_SuretigaiCount(GAMEBEACON_INFO *info, u32 count);

void BEACONINFO_Set_Details_BattleWildPoke(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_Details_BattleSpPoke(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_Details_BattleTrainer(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_BattleLeader(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_BattleSpTrainer(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_Walk(GAMEBEACON_INFO *info);
void BEACONINFO_Set_Details_IntroductionPms(GAMEBEACON_INFO *info, const PMS_DATA *pms);

static BOOL beacon_PokeBeaconTypeIsSpecial(const monsno );

#ifdef PM_DEBUG
GAMEBEACON_INFO * DEBUG_SendBeaconInfo_GetPtr(void);
#endif


//==============================================================================
//  �f�[�^
//==============================================================================
#include "cross_comm.cdat"


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �Q�[���r�[�R���Ǘ��V�X�e���̍쐬
 */
//==================================================================
void GAMEBEACON_Init(HEAPID heap_id)
{
  GF_ASSERT(GameBeaconSys == NULL);
  
  GameBeaconSys = GFL_HEAP_AllocClearMemory(heap_id, sizeof(GAMEBEACON_SYSTEM));
  GameBeaconSys->end_log = -1;
}

//==================================================================
/**
 * �Q�[���r�[�R���Ǘ��V�X�e���̔j��
 */
//==================================================================
void GAMEBEACON_Exit(void)
{
  GF_ASSERT(GameBeaconSys != NULL);
  
  GFL_HEAP_FreeMemory(GameBeaconSys);
}

//==================================================================
/**
 * �Q�[���r�[�R���Ǘ��V�X�e���̍X�V����
 */
//==================================================================
void GAMEBEACON_Update(void)
{
  GAMEBEACON_SYSTEM *bsys = GameBeaconSys;
  int i;
  
  if(GameBeaconSys == NULL){
    return;
  }

  SendBeacon_Update(&bsys->send);
  
  for(i = 0; i < GAMEBEACON_SYSTEM_LOG_MAX; i++){
    if(bsys->log[i].info.version_bit != 0){
      if(bsys->log[i].time < 0xffff){
        bsys->log[i].time++;
      }
    }
  }
}

//==================================================================
/**
 * �Q�[���r�[�R���̏����ݒ���s���܂�
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * gamedata�Ɏ����̖��O�Ȃǂ�ǂݍ��񂾌�ɁA�R�[������悤�ɂ��Ă��������B
 */
//==================================================================
void GAMEBEACON_Setting(GAMEDATA *gamedata)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;

  GameBeaconSys->gamedata = gamedata;
  SendBeacon_Init(send, gamedata);
}

//==================================================================
/**
 * �Q�[���r�[�R�����M�f�[�^���R�s�[
 *
 * @param   info		�R�s�[��ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_SendDataCopy(GAMEBEACON_INFO *info)
{
  GFL_STD_MemCopy(&GameBeaconSys->send, info, sizeof(GAMEBEACON_INFO));
}

//==================================================================
/**
 * ���M�r�[�R���ɕύX���������ꍇ�A�l�b�g�V�X�e���Ƀ��N�G�X�g��������
 *
 * @param   info		
 *
 * �ʐM���I�[�o�[���C����Ă��鎞�ɌĂяo������
 */
//==================================================================
void GAMEBEACON_SendBeaconUpdate(void)
{
  if(GameBeaconSys->send.beacon_update == TRUE){
    NET_WHPIPE_BeaconSetInfo();
    GameBeaconSys->send.beacon_update = FALSE;
  }
}

//--------------------------------------------------------------
/**
 * ���M�r�[�R���X�V����
 *
 * @param   send		
 */
//--------------------------------------------------------------
static void SendBeacon_Update(GAMEBEACON_SEND_MANAGER *send)
{
  if(send->info.action.action_no != GAMEBEACON_ACTION_SEARCH){
    send->life++;
    if(send->life > CrossCommData[send->info.action.action_no].life){
      send->life = 0;
      send->info.action.action_no = GAMEBEACON_ACTION_SEARCH;
    }
  }
}

//--------------------------------------------------------------
/**
 * ���O�ɐV�����r�[�R�������Z�b�g����
 *
 * @param   bsys		
 * @param   info		�Z�b�g����r�[�R�����
 */
//--------------------------------------------------------------
static void BeaconInfo_Set(GAMEBEACON_SYSTEM *bsys, const GAMEBEACON_INFO *info)
{
  GAMEBEACON_LOG *setlog;
  int before_end_log;
  
  MATSUDA_Printf("GameBeaconSet chain_no = %d\n", bsys->end_log);

  before_end_log = bsys->end_log;
  bsys->end_log++;
  if(bsys->end_log >= GAMEBEACON_SYSTEM_LOG_MAX){
    bsys->end_log = 0;
  }
  setlog = &bsys->log[bsys->end_log];

  if(bsys->log_num > 0 && bsys->end_log == bsys->start_log){
    bsys->start_log++;
    if(bsys->start_log >= GAMEBEACON_SYSTEM_LOG_MAX){
      bsys->start_log = 0;
    }
  }
  
  //�A���P�[�g���f
  {
    SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(bsys->gamedata);
    QUESTIONNAIRE_SAVE_WORK *questsave = SaveData_GetQuestionnaire(sv_ctrl);
    int search_question_id, i;
    u32 answer;
    
    for(i = 0; i < QUESTION_NUM_PER_TOPIC; i++){
      search_question_id = QuestionnaireWork_GetInvestigatingQuestion(questsave, i);
      if(search_question_id != INVESTIGATING_QUESTION_NULL){
        if(search_question_id == QUESTION_ID_PLAY_TIME){
          answer = info->play_hour / 10;
          if(info->play_hour % 10 == 0 && info->play_min > 0){
            answer++;
          }
          answer++; //0�͖��񓚂Ȃ̂�1�C���N�������g
          if(answer > GetAnswerNum_byQuestionID(search_question_id)){
            answer = GetAnswerNum_byQuestionID(search_question_id);
          }
        }
        else{
          answer = QuestionnaireAnswer_ReadBit(&info->question_answer, search_question_id);
        }
        
        if(answer != 0){  //0�͖���
          QuestionnaireWork_AddTodayCount(questsave, search_question_id, 1);
          QuestionnaireWork_AddTodayAnswerNum(questsave, search_question_id, answer, 1);
          bsys->new_entry = TRUE;
          MATSUDA_Printf("QuestionAnswerSet id=%d, answer=%d\n", search_question_id, answer);
        }
      }
    } 
  }

  // ���������f
  {
    SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(bsys->gamedata);
    QUESTIONNAIRE_SAVE_WORK *questsave = SaveData_GetQuestionnaire(sv_ctrl);
    MISC *misc = SaveData_GetMisc(sv_ctrl);
    int radar_q_id, team_q_id, i, j;
    u32 answer;

    for(i = 0; i < MAX_QNUM_PER_RESEARCH_REQ; i++) {
      team_q_id = MISC_GetResearchQuestionID(misc, i);
      if(team_q_id == QUESTION_ID_DUMMY){ continue; }

      for(j = 0; j < QUESTION_NUM_PER_TOPIC; j++) { 
        radar_q_id = QuestionnaireWork_GetInvestigatingQuestion(questsave, j);
        if(team_q_id == radar_q_id) {
          answer = QuestionnaireAnswer_ReadBit(&info->question_answer, team_q_id);
          if(answer != 0){  //0�͖���
            MISC_AddResearchCount(misc, i, 1);
          }
        }
      } 
    }
  }

  //���O�ɃZ�b�g
  GFL_STD_MemClear(setlog, sizeof(GAMEBEACON_LOG));
  setlog->info = *info;
  setlog->time = 0;
  bsys->update_log |= 1 << bsys->end_log;
  if(bsys->log_num < GAMEBEACON_SYSTEM_LOG_MAX){
    bsys->log_num++;
  }
}

//==================================================================
/**
 * ��M�r�[�R�����Z�b�g
 *
 * @param   info		��M�����r�[�R���f�[�^�ւ̃|�C���^
 * 
 * @retval  TRUE:�Z�b�g�����@�@FALSE:���O�ɓ����f�[�^����M�ς�
 */
//==================================================================
BOOL GAMEBEACON_SetRecvBeacon(const GAMEBEACON_INFO *info)
{
  GAMEBEACON_SYSTEM *bsys = GameBeaconSys;
  int i;
  BOOL same_player = FALSE;
  
  if((info->version_bit & (1 << (PM_VERSION - VERSION_WHITE))) == 0){
    return FALSE; //�󂯎��Ȃ��o�[�W����
  }
  
//  MATSUDA_Printf("SetRecv action_NO = %d\n", info->action.action_no);
  if(info->action.action_no == GAMEBEACON_ACTION_NULL || info->action.action_no >= GAMEBEACON_ACTION_MAX){
    return FALSE;
  }
  if(info->action.action_no == GAMEBEACON_ACTION_THANKYOU
      && GameBeaconSys->send.info.trainer_id != info->action.thanks.target_trainer_id){
    return FALSE;
  }
  
  if(bsys->start_log <= bsys->end_log){
    for(i = bsys->start_log; i <= bsys->end_log; i++){
      if(bsys->log[i].info.trainer_id == info->trainer_id){
        if(info->action.action_no == GAMEBEACON_ACTION_SEARCH 
            || bsys->log[i].info.send_counter == info->send_counter){
          return FALSE; //���O�ɓ����f�[�^����M�ς�
        }
        same_player = TRUE; //1�v���C���[�͑΂ɂȂ���1�̃��O�o�b�t�@���������Ȃ�
        break;
      }
    }
  }
  else{
    for(i = bsys->start_log; i < GAMEBEACON_SYSTEM_LOG_MAX; i++){
      if(bsys->log[i].info.trainer_id == info->trainer_id){
        if(info->action.action_no == GAMEBEACON_ACTION_SEARCH 
            || bsys->log[i].info.send_counter == info->send_counter){
          return FALSE; //���O�ɓ����f�[�^����M�ς�
        }
        same_player = TRUE; //1�v���C���[�͑΂ɂȂ���1�̃��O�o�b�t�@���������Ȃ�
        break;
      }
    }
    for(i = 0; i <= bsys->end_log; i++){
      if(bsys->log[i].info.trainer_id == info->trainer_id){
        if(info->action.action_no == GAMEBEACON_ACTION_SEARCH 
            || bsys->log[i].info.send_counter == info->send_counter){
          return FALSE; //���O�ɓ����f�[�^����M�ς�
        }
        same_player = TRUE; //1�v���C���[�͑΂ɂȂ���1�̃��O�o�b�t�@���������Ȃ�
        break;
      }
    }
  }
  
  //��M�r�[�R���f�[�^�������`�F�b�N
  if(info->action.action_no == GAMEBEACON_ACTION_DISTRIBUTION_POKE){
    if(info->action.distribution.magic_key != MAGIC_KEY_DISTRIBUTION_POKE){
      return FALSE; //�}�W�b�N�L�[�s��v�ׁ̈A�󂯎�苑��
    }
  }
  else if(info->action.action_no == GAMEBEACON_ACTION_DISTRIBUTION_ITEM){
    if(info->action.distribution.magic_key != MAGIC_KEY_DISTRIBUTION_ITEM){
      return FALSE; //�}�W�b�N�L�[�s��v�ׁ̈A�󂯎�苑��
    }
  }
  else if(info->action.action_no == GAMEBEACON_ACTION_DISTRIBUTION_ETC){
    if(info->action.distribution.magic_key != MAGIC_KEY_DISTRIBUTION_ETC){
      return FALSE; //�}�W�b�N�L�[�s��v�ׁ̈A�󂯎�苑��
    }
  }
  else if(info->action.action_no == GAMEBEACON_ACTION_DISTRIBUTION_GPOWER){
    if(info->action.distribution.magic_key != MAGIC_KEY_DISTRIBUTION_GPOWER){
      return FALSE; //�}�W�b�N�L�[�s��v�ׁ̈A�󂯎�苑��
    }
  }
  
  if(same_player == TRUE){
    bsys->log[i].info = *info;
    bsys->log[i].time = 0;
    bsys->update_log |= 1 << i;
    MATSUDA_Printf("sameBeacon %d����\n", i);
  }
  else{
    BeaconInfo_Set(bsys, info);
    bsys->log_count++;
    MATSUDA_Printf("�Z�b�g���� %d���� id=%d\n", bsys->log_count, info->trainer_id);
  }
  
#if 0
  //���O�̃p���[��񔽉f�@��check ������x�p���[�����܂��Ă�����t�H�[�}�b�g������
  switch(info->action.action_no){
  case GAMEBEACON_ACTION_ENCOUNT_DOWN:
    {
      SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(bsys->gamedata);
      ENC_SV_PTR evc_sv = EncDataSave_GetSaveDataPtr(sv_ctrl);
      EncDataSave_SetSprayCnt(evc_sv, 100);
    }
    break;
  }
#endif

  return TRUE;
}

//--------------------------------------------------------------
/**
 * ���O����r�[�R�������擾����
 *
 * @param   bsys		
 * @param   log_no  ���O�ԍ�
 *
 * @retval  GAMEBEACON_INFO *		NULL�̏ꍇ�̓f�[�^����
 */
//--------------------------------------------------------------
static GAMEBEACON_INFO * BeaconInfo_Get(GAMEBEACON_SYSTEM *bsys, int log_no)
{
//  int log_pos;
  
  if(bsys->log_num == 0 || bsys->log_num - log_no <= 0){
    return NULL;  //�f�[�^����
  }
  
#if 0 //���ڃ��O�ʒu������悤�ɕύX 2010.01.18(��)
  log_pos = bsys->end_log - log_no;
  if(log_pos < 0){
    log_pos = GAMEBEACON_SYSTEM_LOG_MAX + log_pos;
  }
  return &bsys->log[log_pos].info;
#else
  return &bsys->log[log_no].info;
#endif
}

//==================================================================
/**
 * ���O����r�[�R�������擾����
 *
 * @param   log_no  ���O�ԍ�
 *
 * @retval  GAMEBEACON_INFO *		NULL�̏ꍇ�̓f�[�^����
 */
//==================================================================
const GAMEBEACON_INFO * GAMEBEACON_Get_BeaconLog(int log_no)
{
  return BeaconInfo_Get(GameBeaconSys, log_no);
}

//==================================================================
/**
 * ���݂܂ł̃g�[�^����M���O�������擾
 *
 * @retval  u32		
 */
//==================================================================
u32 GAMEBEACON_Get_LogCount(void)
{
  return GameBeaconSys->log_count;
}

//==================================================================
/**
 * �V���̂��������O�ԍ����擾����
 *
 * @param   start_log_no		�����J�n�ʒu(�擪���猟������ꍇ��0���w��)
 *
 * @retval  int		�V���̂��������O�ԍ�(GAMEBEACON_SYSTEM_LOG_MAX�̏ꍇ�͐V������)
 *
 * start_log_no�̈ʒu���璲�׎n�߂āA�ŏ��ɐV�������������O�Ɉ��������������_��
 * return���܂��B
 * start_log_no�͐V���Ɉ����������ďI�������ꍇ�Astart_log_no�͈������������ʒu+1�̏�ԂɂȂ�܂�
 */
//==================================================================
int GAMEBEACON_Get_UpdateLogNo(int *start_log_no)
{
  for( ; (*start_log_no) < GAMEBEACON_SYSTEM_LOG_MAX; (*start_log_no)++){
    if(GameBeaconSys->update_log & (1 << (*start_log_no))){
      (*start_log_no)++;
      return (*start_log_no) - 1;
    }
  }
  return GAMEBEACON_SYSTEM_LOG_MAX;
}

//==================================================================
/**
 * �V���t���O�����Z�b�g����
 *
 * @param   log_no		
 */
//==================================================================
void GAMEBEACON_Reset_UpdateFlag(int log_no)
{
  GF_ASSERT(log_no < GAMEBEACON_SYSTEM_LOG_MAX);
  GameBeaconSys->update_log &= 0xffffffff ^ (1 << log_no);
}

//==================================================================
/**
 * ��M�o�b�t�@�̒��ɓ����l�̃r�[�R�������������A������ΐV���t���O��ON�ɂ���
 * @param   info		�����Ώۂ̃v���C���[�̃r�[�R�����
 * @retval  BOOL		TRUE:ON�ɂ����B�@FALSE:������Ȃ�����
 */
//==================================================================
BOOL GAMEBEACON_Set_SearchUpdateFlag(const GAMEBEACON_INFO *info)
{
  GAMEBEACON_SYSTEM *bsys = GameBeaconSys;
  int i;
  
  for(i = 0; i < GAMEBEACON_SYSTEM_LOG_MAX; i++){
    if(bsys->log[i].info.trainer_id == info->trainer_id){
      bsys->update_log |= 1 << i;
      MATSUDA_Printf("update on %d\n", i);
      return TRUE;
    }
  }
  return FALSE;
}




//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * ���M�r�[�R���ɏ����f�[�^���Z�b�g
 *
 * @param   send		
 * @param   gamedata		
 */
//--------------------------------------------------------------
static void SendBeacon_Init(GAMEBEACON_SEND_MANAGER *send, GAMEDATA * gamedata)
{
  GAMEBEACON_INFO *info = &send->info;
  MYSTATUS *myst = GAMEDATA_GetMyStatus(gamedata);
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  const MISC *misc = SaveData_GetMisc( sv );
  const MYPMS_DATA *mypms = SaveData_GetMyPmsDataConst( sv );
  OSOwnerInfo owner_info;
  PMS_DATA pms;
  
  OS_GetOwnerInfo(&owner_info);
  
  info->version_bit = 0xffff; //�S�o�[�W�����w��
  info->zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(gamedata));
  info->townmap_root_zone_id = TOWNMAP_UTIL_GetRootZoneID(gamedata, info->zone_id);
  info->g_power_id = GPOWER_ID_NULL;
  info->trainer_id = MyStatus_GetID_Low(myst);
  info->favorite_color_index = owner_info.favoriteColor;
  info->sex = MyStatus_GetMySex( myst );
  if(info->sex == PM_MALE){
    info->trainer_view = MyStatus_GetTrainerView(myst);
  }
  else{ //3bit�Ɏ��߂�
    info->trainer_view = MyStatus_GetTrainerView(myst) - UNION_VIEW_INDEX_WOMAN_START;
  }
  info->pm_version = PM_VERSION;
  info->language = PM_LANG;
  info->nation = MyStatus_GetMyNation(myst);
  info->area = MyStatus_GetMyArea(myst);
  info->research_team_rank = MISC_CrossComm_GetResearchTeamRank(misc);
  
  info->thanks_recv_count = MISC_CrossComm_GetThanksRecvCount(misc);
  info->suretigai_count = MISC_CrossComm_GetSuretigaiCount(misc);
  
  {
    PLAYTIME *sv_playtime = SaveData_GetPlayTime( GAMEDATA_GetSaveControlWork(gamedata) );
    info->play_hour = PLAYTIME_GetHour( sv_playtime );
    info->play_min = PLAYTIME_GetMinute( sv_playtime );
  }
  
  {
    QUESTIONNAIRE_SAVE_WORK *qsw = SaveData_GetQuestionnaire(sv);
    QUESTIONNAIRE_ANSWER_WORK *ans = Questionnaire_GetAnswerWork(qsw);
    GFL_STD_MemCopy(ans, &info->question_answer, sizeof(QUESTIONNAIRE_ANSWER_WORK));
  }
  
  MYPMS_GetPms( mypms, MYPMS_PMS_TYPE_INTRODUCTION, &pms );
  GAMEBEACON_Set_Details_IntroductionPms(&pms);

  { //���O�����ȏЉ�R�s�[�@��������Full�̏ꍇ��EOM���������܂Ȃ��̂�
    //STRTOOL_Copy�͎g�p�����ɓƎ��R�s�[
    STRCODE code_eom = GFL_STR_GetEOMCode();
    const STRCODE *src_code;
    int i;
    
    src_code = MyStatus_GetMyName(myst);
    for(i = 0; i < PERSON_NAME_SIZE; i++){
      info->name[i] = src_code[i];
      if(src_code[i] == code_eom){
        break;
      }
    }

    src_code = MISC_CrossComm_GetSelfIntroduction(misc);
    for(i = 0; i < GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN; i++){
      info->self_introduction[i] = src_code[i];
      if(src_code[i] == code_eom){
        break;
      }
    }
  }
  
  //�ڍ׏��
  BEACONINFO_Set_Details_Walk(info);

  //�s���p�����[�^
  info->action.action_no = GAMEBEACON_ACTION_NULL;
}

//--------------------------------------------------------------
/**
 * ���M�r�[�R�����Z�b�g�������̋��ʏ���
 *
 * @param   send		
 * @param   info		
 */
//--------------------------------------------------------------
static void SendBeacon_SetCommon(GAMEBEACON_SEND_MANAGER *send)
{
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(GameBeaconSys->gamedata);
  PLAYTIME *sv_playtime = SaveData_GetPlayTime( sv_ctrl );
  const MISC *misc = SaveData_GetMisc( sv_ctrl );
  
  send->info.play_hour = PLAYTIME_GetHour( sv_playtime );
  send->info.play_min = PLAYTIME_GetMinute( sv_playtime );

  send->info.thanks_recv_count = MISC_CrossComm_GetThanksRecvCount(misc);
  send->info.suretigai_count = MISC_CrossComm_GetSuretigaiCount(misc);

  send->info.send_counter++;
  send->life = 0;
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * ��M�r�[�R���o�b�t�@����w��g���[�i�[ID�̃r�[�R�����������A�Ō�̃f�[�^����M���Ă��牽�t���[���o�߂��Ă��邩���擾����
 *
 * @param   trainer_id		�����Ώۂ̃g���[�i�[ID(����2�o�C�g)
 *
 * @retval  u16		�Ō�̃f�[�^����M���Ă���̌o�߃t���[����
 *                ��M�o�b�t�@�Ɏw�肵�����[�U�[�����Ȃ��ꍇ��0xffff
 */
//==================================================================
u16 GAMEBEACON_Get_RecvBeaconTime(u16 trainer_id)
{
  GAMEBEACON_SYSTEM *bsys = GameBeaconSys;
  int i;
  
  for(i = 0; i < GAMEBEACON_SYSTEM_LOG_MAX; i++){
    if(bsys->log[i].info.version_bit != 0 && bsys->log[i].info.trainer_id == trainer_id){
      return bsys->log[i].time;
    }
  }
  return 0xffff;
}

//==============================================================================
//
//  �A���P�[�g
//
//==============================================================================
//==================================================================
/**
 * �A���P�[�g�p�F�V�����l���Ƃ������������擾����
 *
 * @retval  BOOL		TRUE:�V�����l���Ƃ��������@�@FALSE:�������Ă��Ȃ�
 */
//==================================================================
BOOL GAMEBEACON_Get_NewEntry(void)
{
  int new_entry;
  
  if(GameBeaconSys == NULL){
    return FALSE;
  }
  new_entry = GameBeaconSys->new_entry;
  GameBeaconSys->new_entry = FALSE;
  return new_entry;
}


//==============================================================================
//
//  ���M�f�[�^�X�V
//      ����̃r�[�R�����M�ɔ����đ��M�o�b�t�@�̒l���X�V����
//      send_counter�͑��₳�Ȃ��ׁA���ɓ����A�N�V�������󂯎���Ă���l�ɔ��f�����̂�
//      �ʂ̃A�N�V�������������Ă���ɂȂ�B
//
//==============================================================================
//==================================================================
/**
 * ���M�f�[�^�X�V�F�����̍ŐV�̃A���P�[�g�f�[�^���r�[�R�����M�o�b�t�@�ɔ��f����
 *
 * @param   my_ans		�����̃A���P�[�g�f�[�^�ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_SendDataUpdate_Questionnaire(QUESTIONNAIRE_ANSWER_WORK *my_ans)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  GAMEBEACON_INFO *info = &send->info;

  GFL_STD_MemCopy(my_ans, &info->question_answer, sizeof(QUESTIONNAIRE_ANSWER_WORK));
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * ���M�f�[�^�X�V�F���R�[�h�A�n��R�[�h���r�[�R�����M�o�b�t�@�ɔ��f����
 *
 * @param   nation		���R�[�h
 * @param   area		  �n��R�[�h
 */
//==================================================================
void GAMEBEACON_SendDataUpdate_NationArea(u8 nation, u8 area)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  GAMEBEACON_INFO *info = &send->info;

  info->nation = nation;  //MyStatus_GetMyNation(myst);
  info->area = area;  //MyStatus_GetMyArea(myst);
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * ���M�f�[�^�X�V�F�������������N�𑗐M�o�b�t�@�ɔ��f����
 *
 * @param   research_team_rank		
 */
//==================================================================
void GAMEBEACON_SendDataUpdate_ResearchTeamRank(u8 research_team_rank)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  GAMEBEACON_INFO *info = &send->info;
  info->research_team_rank = research_team_rank;  //MISC_CrossComm_GetResearchTeamRank(misc);
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * ���M�f�[�^�X�V�F�g���[�i�[�J�[�h�̎��ȏЉ�𑗐M�o�b�t�@�ɔ��f����
 *
 * @param   pms		
 */
//==================================================================
void GAMEBEACON_SendDataUpdate_TrCardIntroduction(const PMS_DATA *pms)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  GAMEBEACON_Set_Details_IntroductionPms(pms);
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * ���M�f�[�^�X�V�F�g���[�i�[�J�[�h�̃��j�I���̌����ڂ𑗐M�o�b�t�@�ɔ��f����
 *
 * @param   trainer_view		
 */
//==================================================================
void GAMEBEACON_SendDataUpdate_TrainerView(int trainer_view)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  GAMEBEACON_INFO *info = &send->info;
  info->trainer_view = trainer_view;
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * ���M�f�[�^�X�V�F����Ⴂ�ʐM�̎��ȏЉ�𑗐M�o�b�t�@�ɔ��f����
 */
//==================================================================
void GAMEBEACON_SendDataUpdate_SelfIntroduction(void)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  GAMEBEACON_INFO *info = &send->info;
  SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(GameBeaconSys->gamedata);
  const MISC *misc = SaveData_GetMisc( sv_ctrl );
  const STRCODE *src_code;
  STRCODE code_eom = GFL_STR_GetEOMCode();
  int i;
  
  src_code = MISC_CrossComm_GetSelfIntroduction(misc);
  for(i = 0; i < GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN; i++){
    info->self_introduction[i] = src_code[i];
    if(src_code[i] == code_eom){
      break;
    }
  }
  send->beacon_update = TRUE;
}

//==================================================================
/**
 * �f�o�b�O�p�F�����ŐV�����l���Ƃ��������t���O�𗧂Ă�
 */
//==================================================================
#ifdef PM_DEBUG
void DEBUG_GAMEBEACON_Set_NewEntry(void)
{
  GameBeaconSys->new_entry = TRUE;
}
#endif

//--------------------------------------------------------------
/**
 * �A�N�V�����ԍ��̃v���C�I���e�B�`�F�b�N
 *
 * @param   action_no		
 *
 * @retval  BOOL		    TRUE:�v���C�I���e�B�������ȏ�B�@FALSE:�v���C�I���e�B����
 */
//--------------------------------------------------------------
static BOOL _CheckPriority(u16 action_no)
{
  if(CrossCommData[action_no].priority < CrossCommData[GameBeaconSys->send.info.action.action_no].priority){
    return FALSE;
  }
  return TRUE;
}


//==============================================================================
//
//  �r�[�R���Z�b�g
//
//==============================================================================
//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattlePokeStart(u16 monsno)
{
  if( beacon_PokeBeaconTypeIsSpecial( monsno )){
    GAMEBEACON_Set_BattleSpPokeStart(monsno);
  }else{
    GAMEBEACON_Set_BattleWildPokeStart(monsno);
  }
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattlePokeVictory(u16 monsno)
{
  if( beacon_PokeBeaconTypeIsSpecial( monsno )){
    GAMEBEACON_Set_BattleSpPokeVictory(monsno);
  }else{
    GAMEBEACON_Set_BattleWildPokeVictory(monsno);
  }
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�쐶�|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleWildPokeStart(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_WILD_POKE_START) == FALSE){ return; };
  BEACONINFO_Set_BattleWildPokeStart(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�쐶�̃|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleWildPokeStart(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_WILD_POKE_START;
  info->action.monsno = monsno;
 
  BEACONINFO_Set_Details_BattleWildPoke(info, monsno);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�쐶�̃|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleWildPokeVictory(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleWildPokeVictory(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�쐶�̃|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleWildPokeVictory(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃ|�P�����Ƒΐ���J�n���܂���
 * @param   tr_no   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpPokeStart(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_POKE_START) == FALSE){ return; };
  BEACONINFO_Set_BattleSpPokeStart(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃ|�P�����Ƒΐ���J�n���܂���
 * @param   tr_no   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpPokeStart(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_POKE_START;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_BattleSpPoke(info, monsno);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃ|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpPokeVictory(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleSpPokeVictory(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃ|�P�����Ƃ̑ΐ�ɏ������܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpPokeVictory(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY;
  info->action.monsno = monsno;
  
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleTrainerStart(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_TRAINER_START) == FALSE){ return; };
  BEACONINFO_Set_BattleTrainerStart(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleTrainerStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_TRAINER_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleTrainer(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleTrainerVictory(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleTrainerVictory(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�W�����[�_�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleLeaderStart(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_LEADER_START) == FALSE){ return; };
  BEACONINFO_Set_BattleLeaderStart(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleLeaderStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_LEADER_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleLeader(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�W�����[�_�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleLeaderVictory(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleLeaderVictory(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�W�����[�_�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleLeaderVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpTrainerStart(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_TRAINER_START) == FALSE){ return; };
  BEACONINFO_Set_BattleSpTrainerStart(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpTrainerStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_TRAINER_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleSpTrainer(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleSpTrainerVictory(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_SP_TRAINER_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleSpTrainerVictory(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃg���[�i�[�Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleSpTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_SP_TRAINER_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�`�����s�I���Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleChampionStart(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_CHAMPION_START) == FALSE){ return; };
  BEACONINFO_Set_BattleChampionStart(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�`�����s�I���Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleChampionStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_CHAMPION_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleSpTrainer(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�`�����s�I���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleChampionVictory(u16 tr_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY) == FALSE){ return; };
  BEACONINFO_Set_BattleChampionVictory(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�`�����s�I���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleChampionVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����ߊl
 *
 * @param   monsno    �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_PokemonGet(u16 monsno)
{
  if( beacon_PokeBeaconTypeIsSpecial( monsno )){
    GAMEBEACON_Set_SpecialPokemonGet(monsno);
  }else{
    GAMEBEACON_Set_WildPokemonGet(monsno);
  }
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�쐶�|�P�����ߊl
 *
 * @param   monsno    �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_WildPokemonGet(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_POKE_GET) == FALSE){ return; };
  BEACONINFO_Set_WildPokemonGet(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����ߊl
 *
 * @param   monsno    �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_WildPokemonGet(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_POKE_GET;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���ʂȃ|�P�����ߊl
 *
 * @param   monsno    �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_SpecialPokemonGet(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_SP_POKE_GET) == FALSE){ return; };
  BEACONINFO_Set_SpecialPokemonGet(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���ʂȃ|�P�����ߊl
 *
 * @param   monsno    �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_SpecialPokemonGet(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_POKE_GET;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_Walk(info);
}

//--------------------------------------------------------------
/**
 * �j�b�N�l�[���̃R�s�[�@����������Full�̏ꍇ��EOM���������܂Ȃ��̂œƎ��R�s�[����
 *
 * @param   nickname		    �R�s�[���j�b�N�l�[��
 * @param   dest_nickname		�j�b�N�l�[���R�s�[��
 */
//--------------------------------------------------------------
static void _StrbufNicknameCopy(const STRBUF *nickname, STRCODE *dest_nickname)
{
  STRCODE temp_name[BUFLEN_POKEMON_NAME];
  STRCODE code_eom = GFL_STR_GetEOMCode();
  int i;

  //��������Full�̏ꍇ��EOM���������܂Ȃ��̂œƎ��R�s�[

  //STRBUF�ɓ����Ă���STRCODE��EOM�����݂œW�J�ł���ꎞ�o�b�t�@�Ɏ��o��
  GFL_STR_GetStringCode(nickname, temp_name, BUFLEN_POKEMON_NAME);
  for(i = 0; i < MONS_NAME_SIZE; i++){
    dest_nickname[i] = temp_name[i];
    if(temp_name[i] == code_eom){
      break;
    }
  }
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�������x���A�b�v
 *
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PokemonLevelUp(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_POKE_LVUP) == FALSE){ return; };
  BEACONINFO_Set_PokemonLevelUp(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�������x���A�b�v
 *
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_PokemonLevelUp(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_POKE_LVUP;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����i��
 *
 * @param   monsno  		�Ώۃ|�P�����̃|�P�����ԍ�
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PokemonEvolution(u16 monsno, const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_POKE_EVOLUTION) == FALSE){ return; };
  BEACONINFO_Set_PokemonEvolution(&GameBeaconSys->send.info, monsno, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����i��
 *
 * @param   monsno  		�Ώۃ|�P�����̃|�P�����ԍ�
 * @param   nickname		�Ώۃ|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_PokemonEvolution(GAMEBEACON_INFO *info, u16 monsno, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_POKE_EVOLUTION;
  info->action.monsno = monsno;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���MG�p���[����
 *
 * @param   g_power_id		G�p���[ID
 */
//==================================================================
void GAMEBEACON_Set_GPower(GPOWER_ID g_power_id)
{
  if(_CheckPriority(GAMEBEACON_ACTION_GPOWER) == FALSE){ return; };
  BEACONINFO_Set_GPower(&GameBeaconSys->send.info, g_power_id);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * G�p���[����
 *
 * @param   g_power_id		G�p���[ID
 */
//==================================================================
void BEACONINFO_Set_GPower(GAMEBEACON_INFO *info, GPOWER_ID g_power_id)
{
  info->action.action_no = GAMEBEACON_ACTION_GPOWER;
  info->g_power_id = g_power_id;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�M�d�i���v���C���[������
 *
 * @param   item		�A�C�e���ԍ�
 */
//==================================================================
void GAMEBEACON_Set_SpItemGet(u16 item)
{
  if(_CheckPriority(GAMEBEACON_ACTION_SP_ITEM_GET) == FALSE){ return; };
  BEACONINFO_Set_SpItemGet(&GameBeaconSys->send.info, item);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�M�d�i���v���C���[������
 *
 * @param   item		�A�C�e���ԍ�
 */
//==================================================================
void BEACONINFO_Set_SpItemGet(GAMEBEACON_INFO *info, u16 item)
{
  info->action.action_no = GAMEBEACON_ACTION_SP_ITEM_GET;
  info->action.itemno = item;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�v���C���[�̃v���C���Ԃ��K�萔�𒴂���
 *
 * @param   hour    ��
 */
//==================================================================
void GAMEBEACON_Set_PlayTime(u32 hour)
{
  if(_CheckPriority(GAMEBEACON_ACTION_PLAYTIME) == FALSE){ return; };
  BEACONINFO_Set_PlayTime(&GameBeaconSys->send.info, hour);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�v���C���[�̃v���C���Ԃ��K�萔�𒴂���
 *
 * @param   hour    ��
 */
//==================================================================
void BEACONINFO_Set_PlayTime(GAMEBEACON_INFO *info, u32 hour)
{
  info->action.action_no = GAMEBEACON_ACTION_PLAYTIME;
  info->action.hour = hour;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�����}�ӂ�����
 */
//==================================================================
void GAMEBEACON_Set_ZukanComplete(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_ZUKAN_COMPLETE) == FALSE){ return; };
  BEACONINFO_Set_ZukanComplete(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����}�ӂ�����
 */
//==================================================================
void BEACONINFO_Set_ZukanComplete(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_ZUKAN_COMPLETE;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F������󂯂��񐔂��K�萔�𒴂���
 * @param   thankyou_count	�����
 */
//==================================================================
void GAMEBEACON_Set_ThankyouOver(u32 thankyou_count)
{
  static const u8 regu_count[] = {10, 30, 50, 100, 200};
  int i;
  if(_CheckPriority(GAMEBEACON_ACTION_THANKYOU_OVER) == FALSE){ return; };
  for(i = 0; i < NELEMS(regu_count); i++){
    if(thankyou_count == regu_count[i]){
      BEACONINFO_Set_ThankyouOver(&GameBeaconSys->send.info, thankyou_count);
      SendBeacon_SetCommon(&GameBeaconSys->send);
      return;
    }
  }
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F������󂯂��񐔂��K�萔�𒴂���
 * @param   thankyou_count	�����
 */
//==================================================================
void BEACONINFO_Set_ThankyouOver(GAMEBEACON_INFO *info, u32 thankyou_count)
{
  info->action.action_no = GAMEBEACON_ACTION_THANKYOU_OVER;
  info->action.thankyou_count = thankyou_count;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���j�I�����[������
 */
//==================================================================
void GAMEBEACON_Set_UnionIn(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_UNION_IN) == FALSE){ return; };
  BEACONINFO_Set_UnionIn(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���j�I�����[������
 */
//==================================================================
void BEACONINFO_Set_UnionIn(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_UNION_IN;

  BEACONINFO_Set_Details_Walk(info);
}

#ifdef PM_DEBUG //�C�x���g�p�̔z�zROM�ł������M�͂��Ȃ��̂Ńe�X�g�p
//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�_�E�����[�h�z�M(�|�P����)���{��
 *
 * @param   monsno		�|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_DistributionPoke(u16 monsno)
{
  if(_CheckPriority(GAMEBEACON_ACTION_DISTRIBUTION_POKE) == FALSE){ return; };
  BEACONINFO_Set_DistributionPoke(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�_�E�����[�h�z�M(�|�P����)���{��
 *
 * @param   monsno		�|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_DistributionPoke(GAMEBEACON_INFO *info, u16 monsno)
{
  info->action.action_no = GAMEBEACON_ACTION_DISTRIBUTION_POKE;
  info->action.distribution.monsno = monsno;
  info->action.distribution.magic_key = MAGIC_KEY_DISTRIBUTION_POKE;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�_�E�����[�h�z�M(�A�C�e��)���{��
 *
 * @param   item		�A�C�e���ԍ�
 */
//==================================================================
void GAMEBEACON_Set_DistributionItem(u16 item)
{
  if(_CheckPriority(GAMEBEACON_ACTION_DISTRIBUTION_ITEM) == FALSE){ return; };
  BEACONINFO_Set_DistributionItem(&GameBeaconSys->send.info, item);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�_�E�����[�h�z�M(�A�C�e��)���{��
 *
 * @param   item		�A�C�e���ԍ�
 */
//==================================================================
void BEACONINFO_Set_DistributionItem(GAMEBEACON_INFO *info, u16 item)
{
  info->action.action_no = GAMEBEACON_ACTION_DISTRIBUTION_ITEM;
  info->action.distribution.itemno = item;
  info->action.distribution.magic_key = MAGIC_KEY_DISTRIBUTION_ITEM;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�_�E�����[�h�z�M(���̑�)���{��
 */
//==================================================================
void GAMEBEACON_Set_DistributionEtc(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_DISTRIBUTION_ETC) == FALSE){ return; };
  BEACONINFO_Set_DistributionEtc(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�_�E�����[�h�z�M(���̑�)���{��
 */
//==================================================================
void BEACONINFO_Set_DistributionEtc(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_DISTRIBUTION_ETC;
  info->action.distribution.magic_key = MAGIC_KEY_DISTRIBUTION_ETC;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�C�x���gG�p���[�z�z
 *
 * @param   g_power_id		G�p���[ID
 */
//==================================================================
void GAMEBEACON_Set_DistributionGPower(GPOWER_ID g_power_id)
{
  if(_CheckPriority(GAMEBEACON_ACTION_DISTRIBUTION_GPOWER) == FALSE){ return; };
  BEACONINFO_Set_GPower(&GameBeaconSys->send.info, g_power_id);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�C�x���gG�p���[�z�z
 *
 * @param   g_power_id		G�p���[ID
 */
//==================================================================
void BEACONINFO_Set_DistributionGPower(GAMEBEACON_INFO *info, GPOWER_ID g_power_id)
{
  info->action.action_no = GAMEBEACON_ACTION_DISTRIBUTION_GPOWER;
  info->action.distribution.magic_key = MAGIC_KEY_DISTRIBUTION_GPOWER;
  info->action.distribution.gpower_id = g_power_id;

  BEACONINFO_Set_Details_Walk(info);
}
#endif  //PM_DEBUG

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬�F�v���C���[�̃|�P�������}���ɍU�������Ă�
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��(�U���������|�P����)
 */
//==================================================================
void GAMEBEACON_Set_CriticalHit(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_CRITICAL_HIT) == FALSE){ return; };
  BEACONINFO_Set_CriticalHit(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�퓬�F�v���C���[�̃|�P�������}���ɍU�������Ă�
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��(�U���������|�P����)
 */
//==================================================================
void BEACONINFO_Set_CriticalHit(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_CRITICAL_HIT;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
//  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬�F�v���C���[�̃|�P�������}���ɍU�����󂯂�
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��(�U�����󂯂��|�P����)
 */
//==================================================================
void GAMEBEACON_Set_CriticalDamage(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_CRITICAL_DAMAGE) == FALSE){ return; };
  BEACONINFO_Set_CriticalDamage(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�퓬�F�v���C���[�̃|�P�������}���ɍU�����󂯂�
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��(�U�����󂯂��|�P����)
 */
//==================================================================
void BEACONINFO_Set_CriticalDamage(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_CRITICAL_DAMAGE;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬���瓦���o����
 */
//==================================================================
void GAMEBEACON_Set_Escape(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_ESCAPE) == FALSE){ return; };
  BEACONINFO_Set_Escape(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�퓬���瓦���o����
 */
//==================================================================
void BEACONINFO_Set_Escape(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_ESCAPE;

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������HP��5���ȏ㏭�Ȃ��Ȃ��Ă���
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_HPLittle(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_HP_LITTLE) == FALSE){ return; };
  BEACONINFO_Set_HPLittle(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������HP��5���ȏ㏭�Ȃ��Ȃ��Ă���
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_HPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_HP_LITTLE;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������PP��1�ł�0�ɂȂ��Ă���
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_PPLittle(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_PP_LITTLE) == FALSE){ return; };
  BEACONINFO_Set_PPLittle(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P������PP��1�ł�0�ɂȂ��Ă���
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_PPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_PP_LITTLE;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�퓬�̃|�P�������m��
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_Dying(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_DYING) == FALSE){ return; };
  BEACONINFO_Set_Dying(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P�������m��
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_Dying(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_DYING;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�퓬�̃|�P��������Ԉُ�
 *
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_StateIsAbnormal(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_STATE_IS_ABNORMAL) == FALSE){ return; };
  BEACONINFO_Set_StateIsAbnormal(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�퓬��A�擪�̃|�P��������Ԉُ�
 *
 * @param   info		
 * @param   nickname		�Ώۂ̃|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_StateIsAbnormal(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_STATE_IS_ABNORMAL;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�v���C���[��������g�p
 *
 * @param   item_no   �A�C�e���ԍ�
 */
//==================================================================
void GAMEBEACON_Set_UseItem(u16 item_no)
{
  if(_CheckPriority(GAMEBEACON_ACTION_USE_ITEM) == FALSE){ return; };
  BEACONINFO_Set_UseItem(&GameBeaconSys->send.info, item_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�v���C���[��������g�p
 *
 * @param   info		
 * @param   item_no   �A�C�e���ԍ�
 */
//==================================================================
void BEACONINFO_Set_UseItem(GAMEBEACON_INFO *info, u16 item_no)
{
  info->action.action_no = GAMEBEACON_ACTION_USE_ITEM;
  info->action.itemno = item_no;
  
//  �ڍׂ͒��O�̂��ێ�
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�t�B�[���h�Z���g�p
 *
 * @param   wazano    �Z�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_FieldSkill(u16 wazano)
{
  if(_CheckPriority(GAMEBEACON_ACTION_FIELD_SKILL) == FALSE){ return; };
  BEACONINFO_Set_FieldSkill(&GameBeaconSys->send.info, wazano);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�t�B�[���h�Z���g�p
 *
 * @param   info		
 * @param   wazano		�Z�ԍ�
 */
//==================================================================
void BEACONINFO_Set_FieldSkill(GAMEBEACON_INFO *info, u16 wazano)
{
  info->action.action_no = GAMEBEACON_ACTION_FIELD_SKILL;
  info->action.wazano = wazano;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�^�}�S����ĉ�����󂯎����
 */
//==================================================================
void GAMEBEACON_Set_SodateyaEgg( void )
{
  if(_CheckPriority(GAMEBEACON_ACTION_SODATEYA_EGG) == FALSE){ return; };
  BEACONINFO_Set_SodateyaEgg(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�^�}�S����ĉ�����󂯎����
 *
 * @param   info		
 */
//==================================================================
void BEACONINFO_Set_SodateyaEgg(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_SODATEYA_EGG;
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�^�}�S���z��
 *
 * @param   monsno		�z�������^�}�S�̃|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_EggHatch( u16 monsno )
{
  if(_CheckPriority(GAMEBEACON_ACTION_EGG_HATCH) == FALSE){ return; };
  BEACONINFO_Set_EggHatch(&GameBeaconSys->send.info, monsno );
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�^�}�S���z��
 *
 * @param   info		
 * @param   monsno		�z�������^�}�S�̃|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_EggHatch(GAMEBEACON_INFO *info, u16 monsno )
{
  info->action.action_no = GAMEBEACON_ACTION_EGG_HATCH;
  info->action.monsno = monsno;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F��������
 */
//==================================================================
void GAMEBEACON_Set_Shoping(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_SHOPING) == FALSE){ return; };
  BEACONINFO_Set_Shoping(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F��������
 */
//==================================================================
void BEACONINFO_Set_Shoping(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_SHOPING;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�o�g���T�u�E�F�C���풆
 */
//==================================================================
void GAMEBEACON_Set_Subway(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_SUBWAY) == FALSE){ return; };
  BEACONINFO_Set_Subway(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�o�g���T�u�E�F�C���풆
 */
//==================================================================
void BEACONINFO_Set_Subway(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_SUBWAY;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�o�g���T�u�E�F�C�A����
 *
 * @param   victory_count		�A����
 */
//==================================================================
void GAMEBEACON_Set_SubwayStraightVictories(u32 victory_count)
{
  if(_CheckPriority(GAMEBEACON_ACTION_SUBWAY_STRAIGHT_VICTORIES) == FALSE){ return; };
  BEACONINFO_Set_SubwayStraightVictories(&GameBeaconSys->send.info, victory_count);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�o�g���T�u�E�F�C�A����(���E���h��)
 *
 * @param   victory_count		�����i�񂾃��E���h��
 *
 * �L�^�Ƃ��ĐL�тĂ���<�A����>�ł͂Ȃ��A7��1�Z�b�g�̉����ڂ���1�`7�œn���Ă�������
 */
//==================================================================
void BEACONINFO_Set_SubwayStraightVictories(GAMEBEACON_INFO *info, u32 victory_count)
{
  info->action.action_no = GAMEBEACON_ACTION_SUBWAY_STRAIGHT_VICTORIES;
  info->action.victory_count = victory_count;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�o�g���T�u�E�F�C�Ńg���t�B�[�擾
 *
 * @param   victory_count		�A����
 */
//==================================================================
void GAMEBEACON_Set_SubwayTrophyGet( void )
{
  if(_CheckPriority(GAMEBEACON_ACTION_SUBWAY_TROPHY_GET) == FALSE){ return; };
  BEACONINFO_Set_SubwayTrophyGet(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�o�g���T�u�E�F�C�Ńg���t�B�[�Q�b�g
 *
 * @param   victory_count		�A����
 */
//==================================================================
void BEACONINFO_Set_SubwayTrophyGet( GAMEBEACON_INFO *info )
{
  info->action.action_no = GAMEBEACON_ACTION_SUBWAY_TROPHY_GET;
  info->action.victory_count = 0;
  
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���C�A���n�E�X�ɒ��풆
 */
//==================================================================
void GAMEBEACON_Set_TrialHouse(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_TRIALHOUSE) == FALSE){ return; };
  BEACONINFO_Set_TrialHouse(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���C�A���n�E�X�ɒ��풆
 */
//==================================================================
void BEACONINFO_Set_TrialHouse(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_TRIALHOUSE;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�g���C�A���n�E�X�Ń����N�m��
 *
 * @param   rank      TH_RANK_xxx
 */
//==================================================================
void GAMEBEACON_Set_TrialHouseRank(u8 rank)
{
  if(_CheckPriority(GAMEBEACON_ACTION_TRIALHOUSE_RANK) == FALSE){ return; };
  BEACONINFO_Set_TrialHouseRank(&GameBeaconSys->send.info, rank);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�g���C�A���n�E�X�Ń����N�m��
 *
 * @param   info		
 * @param   rank		TH_RANK_xxx
 */
//==================================================================
void BEACONINFO_Set_TrialHouseRank(GAMEBEACON_INFO *info, u8 rank)
{
  info->action.action_no = GAMEBEACON_ACTION_TRIALHOUSE_RANK;
  info->action.trial_house_rank = rank;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�ϗ��Ԃɏ����
 */
//==================================================================
void GAMEBEACON_Set_FerrisWheel(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_FERRIS_WHEEL) == FALSE){ return; };
  BEACONINFO_Set_FerrisWheel(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�ϗ��Ԃɏ����
 */
//==================================================================
void BEACONINFO_Set_FerrisWheel(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_FERRIS_WHEEL;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�|�P�V�t�^�[����
 */
//==================================================================
void GAMEBEACON_Set_PokeShifter(void)
{
  if(_CheckPriority(GAMEBEACON_ACTION_POKESHIFTER) == FALSE){ return; };
  BEACONINFO_Set_PokeShifter(&GameBeaconSys->send.info);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�V�t�^�[����
 */
//==================================================================
void BEACONINFO_Set_PokeShifter(GAMEBEACON_INFO *info)
{
  info->action.action_no = GAMEBEACON_ACTION_POKESHIFTER;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�~���[�W�J������
 *
 * @param   nickname		�Q�������|�P�����̃j�b�N�l�[��
 */
//==================================================================
void GAMEBEACON_Set_Musical(const STRBUF *nickname)
{
  if(_CheckPriority(GAMEBEACON_ACTION_MUSICAL) == FALSE){ return; };
  BEACONINFO_Set_Musical(&GameBeaconSys->send.info, nickname);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�~���[�W�J������
 *
 * @param   info		
 * @param   nickname		�Q�������|�P�����̃j�b�N�l�[��
 */
//==================================================================
void BEACONINFO_Set_Musical(GAMEBEACON_INFO *info, const STRBUF *nickname)
{
  info->action.action_no = GAMEBEACON_ACTION_MUSICAL;
  _StrbufNicknameCopy(nickname, info->action.normal.nickname);

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F���l�̃v���C���[����󂯎����G�p���[���g�p
 *
 * @param   gpower_id		G�p���[ID
 */
//==================================================================
void GAMEBEACON_Set_OtherGPowerUse(GPOWER_ID gpower_id)
{
  if(_CheckPriority(GAMEBEACON_ACTION_OTHER_GPOWER_USE) == FALSE){ return; };
  BEACONINFO_Set_OtherGPowerUse(&GameBeaconSys->send.info, gpower_id);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F���l�̃v���C���[����󂯎����G�p���[���g�p
 *
 * @param   info		
 * @param   gpower_id		G�p���[ID
 */
//==================================================================
void BEACONINFO_Set_OtherGPowerUse(GAMEBEACON_INFO *info, GPOWER_ID gpower_id)
{
  info->action.action_no = GAMEBEACON_ACTION_OTHER_GPOWER_USE;
  info->action.gpower_id = gpower_id;

  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�u���肪�Ƃ��I�v
 *
 * @param   gamedata		
 * @param   target_trainer_id   ��������鑊��̃g���[�i�[ID
 */
//==================================================================
void GAMEBEACON_Set_Thankyou(GAMEDATA *gamedata, u32 target_trainer_id)
{
  if(_CheckPriority(GAMEBEACON_ACTION_THANKYOU) == FALSE){ return; };
  BEACONINFO_Set_Thankyou(&GameBeaconSys->send.info, gamedata, target_trainer_id);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�u���肪�Ƃ��I�v
 *
 * @param   gamedata		
 * @param   target_trainer_id   ��������鑊��̃g���[�i�[ID
 */
//==================================================================
void BEACONINFO_Set_Thankyou(GAMEBEACON_INFO *info, GAMEDATA *gamedata, u32 target_trainer_id)
{
  const MISC *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(gamedata) );

  STRTOOL_Copy(MISC_CrossComm_GetThankyouMessage(misc), 
    info->action.thanks.thankyou_message, GAMEBEACON_THANKYOU_MESSAGE_LEN);
  info->action.action_no = GAMEBEACON_ACTION_THANKYOU;
  info->action.thanks.target_trainer_id = target_trainer_id;
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�ꌾ���b�Z�[�W
 *
 * @param   gamedata		
 * @param   free_word   �ꌾ���b�Z�[�W
 */
//==================================================================
void GAMEBEACON_Set_FreeWord( const STRBUF *free_word )
{
  if(_CheckPriority(GAMEBEACON_ACTION_FREEWORD) == FALSE){ return; };
  BEACONINFO_Set_FreeWord(&GameBeaconSys->send.info, free_word);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�ꌾ���b�Z�[�W
 *
 * @param   gamedata		
 * @param   free_word   �ꌾ���b�Z�[�W
 */
//==================================================================
void BEACONINFO_Set_FreeWord(GAMEBEACON_INFO *info, const STRBUF *free_word)
{
  GFL_STR_GetStringCode(free_word, info->action.freeword_message, GAMEBEACON_FREEWORD_MESSAGE_LEN);
  info->action.action_no = GAMEBEACON_ACTION_FREEWORD;
}

//==================================================================
/**
 * ���M���̑��Z�b�g�F�]�[���؂�ւ�
 *
 * @param   zone_id   ���ݒn
 */
//==================================================================
void GAMEBEACON_Set_ZoneChange(ZONEID zone_id, const GAMEDATA *cp_gamedata)
{
  BEACONINFO_Set_ZoneChange(&GameBeaconSys->send.info, zone_id, cp_gamedata);
}

//==================================================================
/**
 * ���̑��Z�b�g�F�]�[���؂�ւ�
 *
 * @param   zone_id   ���ݒn
 */
//==================================================================
void BEACONINFO_Set_ZoneChange(GAMEBEACON_INFO *info, ZONEID zone_id, const GAMEDATA *cp_gamedata)
{
  info->zone_id = zone_id;
  info->townmap_root_zone_id = TOWNMAP_UTIL_GetRootZoneID(cp_gamedata, zone_id);
  info->details.details_no = GAMEBEACON_DETAILS_NO_WALK;
  BEACONINFO_Set_Details_Walk(info);
}

//==================================================================
/**
 * ���M���̑��Z�b�g�F������󂯂���
 *
 * @param   count		
 */
//==================================================================
static void GAMEBEACON_Set_ThanksRecvCount(u32 count)
{
  BEACONINFO_Set_ThanksRecvCount(&GameBeaconSys->send.info, count);
}

//==================================================================
/**
 * ���̑��Z�b�g�F������󂯂���
 *
 * @param   count		
 */
//==================================================================
void BEACONINFO_Set_ThanksRecvCount(GAMEBEACON_INFO *info, u32 count)
{
  info->thanks_recv_count = count;
}

//==================================================================
/**
 * ���M���̑��Z�b�g�F���������l��
 *
 * @param   count		
 */
//==================================================================
static void GAMEBEACON_Set_SuretigaiCount(u32 count)
{
  BEACONINFO_Set_SuretigaiCount(&GameBeaconSys->send.info, count);
}

//==================================================================
/**
 * ���̑��Z�b�g�F���������l��
 *
 * @param   count		
 */
//==================================================================
void BEACONINFO_Set_SuretigaiCount(GAMEBEACON_INFO *info, u32 count)
{
  info->suretigai_count = count;
}

//==============================================================================
//  �ڍ׃p�����[�^
//==============================================================================
//--------------------------------------------------------------
/**
 * �ڍ׃Z�b�g�F�쐶�̃|�P�����Ƒΐ풆
 * @param   monsno		�|�P�����ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleWildPoke(GAMEBEACON_INFO *info, u16 monsno)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_WILD_POKE;
  info->details.battle_monsno = monsno;
}

//--------------------------------------------------------------
/**
 * ���ʂȃ|�P�����Ƒΐ풆
 * @param   monsno		�|�P�����ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleSpPoke(GAMEBEACON_INFO *info, u16 monsno)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_SP_POKE;
  info->details.battle_monsno = monsno;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�Ƒΐ풆
 * @param   tr_no		�g���[�i�[�ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleTrainer(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_TRAINER;
  info->details.battle_trainer = tr_no;
}

//--------------------------------------------------------------
/**
 * �W�����[�_�[�Ƒΐ풆
 * @param   tr_no		�g���[�i�[�ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleLeader(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_JIMLEADER;
  info->details.battle_trainer = tr_no;
}

//--------------------------------------------------------------
/**
 * ���ʂȃg���[�i�[�Ƒΐ풆
 * @param   tr_no		�g���[�i�[�ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleSpTrainer(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_SP_TRAINER;
  info->details.battle_trainer = tr_no;
}

//--------------------------------------------------------------
/**
 * �u�X�A���H�A�_���W�����v���ړ���
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_Walk(GAMEBEACON_INFO *info)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_WALK;
  //ZONE_ID��GAMEBEACON_Set_ZoneChange���g�p���ĕʂɃZ�b�g���Ă��炤�B
  //�����ł͏ڍ�No���ړ����ɖ߂����ƂŁA�Ō�ɃZ�b�g���Ă���
  //GAMEBEACON_Set_ZoneChange�̃]�[��ID���\�������悤�ɂ��Ă���
  //�]�[��ID�̓]�[���؂�ւ��������ɃZ�b�g���Ă��炤�C���[�W�B
  //�����̂悤�ɂ��Ȃ���Ώڍ�No���u�ړ����v�̃r�[�R���Z�b�g���ߑS�Ă�
  //�@�]�[��ID��n���K�v���o�Ă���
}

//==================================================================
/**
 * �r�[�R���ڍ׃Z�b�g�F���ȏЉ�ȈՉ�b
 *
 * @param   pms		�ȈՉ�b�f�[�^�ւ̃|�C���^
 */
//==================================================================
void BEACONINFO_Set_Details_IntroductionPms(GAMEBEACON_INFO *info, const PMS_DATA *pms)
{
  GAMEBEACON_SEND_MANAGER *send = &GameBeaconSys->send;
  
  send->info.details.sentence_type = pms->sentence_type;
  send->info.details.sentence_id = pms->sentence_id;
  send->info.details.word[0] = pms->word[0];
  send->info.details.word[1] = pms->word[1];
}

//==================================================================
/**
 * ���M�r�[�R���ڍ׃Z�b�g�F���ȏЉ�ȈՉ�b
 *
 * @param   pms		�ȈՉ�b�f�[�^�ւ̃|�C���^
 */
//==================================================================
void GAMEBEACON_Set_Details_IntroductionPms(const PMS_DATA *pms)
{
  BEACONINFO_Set_Details_IntroductionPms(&GameBeaconSys->send.info, pms);
}

//==================================================================
/**
 * �|�P�����֘A�r�[�R�� �^�C�v�`�F�b�N
 *
 * @param   monsno
 */
//==================================================================
static BOOL beacon_PokeBeaconTypeIsSpecial(const monsno )
{
  int i;
  static const u16 sp_poke[] = {
    MONSNO_ATOSU, 
    MONSNO_PORUTOSU, 
    MONSNO_ARAMISU, 
    MONSNO_KAZAKAMI, 
    MONSNO_RAIKAMI, 
    MONSNO_SIN, 
    MONSNO_MU, 
    MONSNO_TUTINOKAMI, 
    MONSNO_RAI, 
    MONSNO_DARUTANISU, 
    MONSNO_MERODHIA, 
    MONSNO_INSEKUTA, 
    MONSNO_BIKUTHI, 
  };

  for(i = 0;i < NELEMS(sp_poke);i++){
    if( sp_poke[i] == monsno ){
      return TRUE;
    }
  }
  return FALSE;
}




#ifdef PM_DEBUG
//==================================================================
/**
 * �f�o�b�O�p�F�����̑��M�r�[�R�����o�b�t�@�̃|�C���^���擾����
 * @retval  GAMEBEACON_INFO *		�����̑��M�r�[�R�����o�b�t�@�ւ̃|�C���^
 */
//==================================================================
GAMEBEACON_INFO * DEBUG_SendBeaconInfo_GetPtr(void)
{
  GF_ASSERT(GameBeaconSys != NULL);
  return &GameBeaconSys->send.info;
}

//==================================================================
/**
 * �f�o�b�O�p�F��M���O��S���N���A����
 */
//==================================================================
void DEBUG_RecvBeaconBufferClear(void)
{
  GFL_STD_MemClear(GameBeaconSys->log, sizeof(GAMEBEACON_LOG) * GAMEBEACON_SYSTEM_LOG_MAX);
}

#endif  //PM_DEBUG

