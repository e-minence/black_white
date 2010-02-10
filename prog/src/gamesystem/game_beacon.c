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


//==============================================================================
//  �萔��`
//==============================================================================
///�r�[�R������
#define LIFE_TIME     (15 * 60)

///�z�M�p�}�W�b�N�L�[�F�|�P����
#define MAGIC_KEY_DISTRIBUTION_POKE    (0x2932013a)
///�z�M�p�}�W�b�N�L�[�F�A�C�e��
#define MAGIC_KEY_DISTRIBUTION_ITEM    (0x48841dc1)
///�z�M�p�}�W�b�N�L�[�F���̑�
#define MAGIC_KEY_DISTRIBUTION_ETC     (0x701ddc92)

///�r�[�R�����ewordset�p�e���|�����o�b�t�@����`
#define BUFLEN_BEACON_WORDSET_TMP (16*2+EOM_SIZE)

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
};

//������ASSERT�ɂЂ�����������update_log��bit�Ǘ��ł͂Ȃ��z��Ǘ��ɕύX����
SDK_COMPILER_ASSERT(GAMEBEACON_SYSTEM_LOG_MAX < 32);

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

//==============================================================================
//  �\���̒�`
//==============================================================================
///1���̃��O�f�[�^
typedef struct{
  GAMEBEACON_INFO info;
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
void BEACONINFO_Set_BattleBigFourStart(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleBigFourVictory(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleChampionStart(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_BattleChampionVictory(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_PokemonGet(GAMEBEACON_INFO *info, u16 monsno);
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
#endif
void BEACONINFO_Set_Thankyou(GAMEBEACON_INFO *info, GAMEDATA *gamedata, u32 target_trainer_id);
void BEACONINFO_Set_ZoneChange(GAMEBEACON_INFO *info, ZONEID zone_id, const GAMEDATA *cp_gamedata);
void BEACONINFO_Set_ThanksRecvCount(GAMEBEACON_INFO *info, u32 count);
void BEACONINFO_Set_SuretigaiCount(GAMEBEACON_INFO *info, u32 count);

void BEACONINFO_Set_Details_BattleWildPoke(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_Details_BattleSpPoke(GAMEBEACON_INFO *info, u16 monsno);
void BEACONINFO_Set_Details_BattleTrainer(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_BattleLeader(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_BattleBigFour(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_BattleChampion(GAMEBEACON_INFO *info, u16 tr_no);
void BEACONINFO_Set_Details_Walk(GAMEBEACON_INFO *info);
void BEACONINFO_Set_Details_IntroductionPms(GAMEBEACON_INFO *info, const PMS_DATA *pms);

#ifdef PM_DEBUG
GAMEBEACON_INFO * DEBUG_SendBeaconInfo_GetPtr(void);
#endif


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
  
  if(GameBeaconSys == NULL){
    return;
  }

  SendBeacon_Update(&bsys->send);
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
    GFI_NET_BeaconSetInfo();
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
    if(send->life > LIFE_TIME){
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
  
  GFL_STD_MemClear(setlog, sizeof(GAMEBEACON_LOG));
  setlog->info = *info;
  bsys->update_log |= 1 < bsys->end_log;
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
  
//  OS_TPrintf("SetRecv action_NO = %d\n", info->action.action_no);
  if(info->action.action_no == GAMEBEACON_ACTION_NULL || info->action.action_no >= GAMEBEACON_ACTION_MAX){
    return FALSE;
  }
  
  if(bsys->start_log <= bsys->end_log){
    for(i = bsys->start_log; i <= bsys->end_log; i++){
      if(bsys->log[i].info.trainer_id == info->trainer_id){
        if(info->action.action_no == GAMEBEACON_ACTION_SEARCH 
            || bsys->log[i].info.send_counter == info->send_counter){
//        OS_TPrintf("���Ɏ�M�ς�\n");
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
  
  if(same_player == TRUE){
    bsys->log[i].info = *info;
    bsys->update_log |= 1 << i;
  }
  else{
    BeaconInfo_Set(bsys, info);
    bsys->new_entry = TRUE;
    bsys->log_count++;
    OS_TPrintf("�Z�b�g���� %d����\n", bsys->log_count);
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
  const MISC *misc = SaveData_GetMisc( GAMEDATA_GetSaveControlWork(gamedata) );
  const MYPMS_DATA *mypms = SaveData_GetMyPmsDataConst( GAMEDATA_GetSaveControlWork(gamedata) );
  OSOwnerInfo owner_info;
  PMS_DATA pms;
  
  OS_GetOwnerInfo(&owner_info);
  
  info->version_bit = 0xffff; //�S�o�[�W�����w��
  info->zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(gamedata));
  info->townmap_root_zone_id = TOWNMAP_UTIL_GetRootZoneID(gamedata, info->zone_id);
  info->g_power_id = GPOWER_ID_NULL;
  info->trainer_id = MyStatus_GetID_Low(myst);
  info->favorite_color_index = owner_info.favoriteColor;
  info->trainer_view = MyStatus_GetTrainerView(myst);
  info->sex = MyStatus_GetMySex( myst );
  info->pm_version = PM_VERSION;
  info->language = PM_LANG;
  info->nation = MyStatus_GetMyNation(myst);
  info->area = MyStatus_GetMyArea(myst);
  info->research_team_rank = MISC_CrossComm_GetResearchTeamRank(misc);
  
  info->thanks_recv_count = MISC_CrossComm_GetThanksRecvCount(misc);
  info->suretigai_count = MISC_CrossComm_GetSuretigaiCount(misc);
  
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
  send->info.send_counter++;
  send->life = 0;
  send->beacon_update = TRUE;
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
void GAMEBEACON_Wordset(const GAMEBEACON_INFO *info, WORDSET *wordset, HEAPID temp_heap_id)
{
  u8 type;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BUFLEN_BEACON_WORDSET_TMP , temp_heap_id);

  //�g���[�i�[���W�J(�f�t�H���g)
	GFL_STR_SetStringCodeOrderLength(strbuf, info->name, PERSON_NAME_SIZE+EOM_SIZE);
  WORDSET_RegisterWord( wordset, 0, strbuf, 0, TRUE, PM_LANG);

  type = DATA_BeaconWordsetType[ info->action.action_no ];

  switch( type ){
  case BEACON_WSET_TRNAME:
    WORDSET_RegisterTrainerName( wordset, 1, info->action.tr_no );
    break;
  case BEACON_WSET_MONSNAME:
  case BEACON_WSET_POKE_W:
    WORDSET_RegisterPokeMonsNameNo( wordset, 1, info->action.monsno );
    if( type == BEACON_WSET_MONSNAME ){
      break;
    }
    //�u���C�N�X���[
  case BEACON_WSET_NICKNAME:
  	GFL_STR_SetStringCodeOrderLength(strbuf, info->action.normal.nickname, MONS_NAME_SIZE+EOM_SIZE);
    WORDSET_RegisterWord(wordset, 2, strbuf, 0, TRUE, PM_LANG);
    break;
  case BEACON_WSET_ITEM:
    WORDSET_RegisterItemName( wordset, 1, info->action.itemno );
    break;
  case BEACON_WSET_PTIME:
    WORDSET_RegisterNumber( wordset, 1, info->action.hour, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    break;
  case BEACON_WSET_THANKS:
    WORDSET_RegisterNumber( wordset, 1, info->action.thankyou_count, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    break;
  case BEACON_WSET_HAIHU_MONS:
    WORDSET_RegisterItemName( wordset, 1, info->action.distribution.monsno );
    break;
  case BEACON_WSET_HAIHU_ITEM:
    WORDSET_RegisterItemName( wordset, 1, info->action.distribution.itemno );
    break;
  }
  GFL_STR_DeleteBuffer(strbuf);
}

//==================================================================
/**
 * �r�[�R����񂩂�\�����郁�b�Z�[�WID���擾����
 *
 * @param   info		�Ώۂ̃r�[�R�����ւ̃|�C���^
 */
//==================================================================
u32 GAMEBEACON_GetMsgID(const GAMEBEACON_INFO *info)
{
  if(info->action.action_no == GAMEBEACON_ACTION_NULL || info->action.action_no >= GAMEBEACON_ACTION_MAX){
    GF_ASSERT_MSG(0, "action_no = %d\n", info->action.action_no);
    return msg_beacon_001;
  }
  return msg_beacon_001 + info->action.action_no - GAMEBEACON_ACTION_SEARCH;
}


//==============================================================================
//
//  �A���P�[�g
//
//==============================================================================
//==================================================================
/**
 * �V�����l���Ƃ������������擾����
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


//==============================================================================
//
//  �r�[�R���Z�b�g
//
//==============================================================================
//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�쐶�̃|�P�����Ƒΐ���J�n���܂���
 * @param   monsno   �|�P�����ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleWildPokeStart(u16 monsno)
{
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
 * ���M�r�[�R���Z�b�g�F�l�V���Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleBigFourStart(u16 tr_no)
{
  BEACONINFO_Set_BattleBigFourStart(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�l�V���Ƒΐ���J�n���܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleBigFourStart(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_BIGFOUR_START;
  info->action.tr_no = tr_no;

  BEACONINFO_Set_Details_BattleBigFour(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�l�V���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleBigFourVictory(u16 tr_no)
{
  BEACONINFO_Set_BattleBigFourVictory(&GameBeaconSys->send.info, tr_no);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�l�V���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void BEACONINFO_Set_BattleBigFourVictory(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->action.action_no = GAMEBEACON_ACTION_BATTLE_BIGFOUR_VICTORY;
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

  BEACONINFO_Set_Details_BattleChampion(info, tr_no);
}

//==================================================================
/**
 * ���M�r�[�R���Z�b�g�F�`�����s�I���Ƃ̑ΐ�ɏ������܂���
 * @param   tr_no   �g���[�i�[�ԍ�
 */
//==================================================================
void GAMEBEACON_Set_BattleChampionVictory(u16 tr_no)
{
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
  BEACONINFO_Set_PokemonGet(&GameBeaconSys->send.info, monsno);
  SendBeacon_SetCommon(&GameBeaconSys->send);
}

//==================================================================
/**
 * �r�[�R���Z�b�g�F�|�P�����ߊl
 *
 * @param   monsno    �|�P�����ԍ�
 */
//==================================================================
void BEACONINFO_Set_PokemonGet(GAMEBEACON_INFO *info, u16 monsno)
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
  BEACONINFO_Set_ThankyouOver(&GameBeaconSys->send.info, thankyou_count);
  SendBeacon_SetCommon(&GameBeaconSys->send);
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
#endif  //PM_DEBUG

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

  STRTOOL_Copy(MISC_CrossComm_GetSelfIntroduction(misc), 
    info->action.thankyou_message, GAMEBEACON_THANKYOU_MESSAGE_LEN);
  info->action.action_no = GAMEBEACON_ACTION_THANKYOU;
  info->action.target_trainer_id = target_trainer_id;
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
 * �l�V���Ƒΐ풆
 * @param   tr_no		�g���[�i�[�ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleBigFour(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_BIGFOUR;
  info->details.battle_trainer = tr_no;
}

//--------------------------------------------------------------
/**
 * �`�����s�I���Ƒΐ풆
 * @param   tr_no		�g���[�i�[�ԍ�
 */
//--------------------------------------------------------------
void BEACONINFO_Set_Details_BattleChampion(GAMEBEACON_INFO *info, u16 tr_no)
{
  info->details.details_no = GAMEBEACON_DETAILS_NO_BATTLE_CHAMPION;
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

#endif  //PM_DEBUG

