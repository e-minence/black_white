//==============================================================================
/**
 * @file    debug_beacon.c
 * @brief   �f�o�b�O�p�̃r�[�R�����쐬
 * @author  matsuda
 * @date    2010.02.06(�y)
 */
//==============================================================================
#ifdef PM_DEBUG

#include <gflib.h>
#include "gamesystem/game_data.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "msg/msg_beacon_status.h"
#include "savedata/encount_sv.h"
#include "savedata/misc.h"
#include "net/net_whpipe.h"
#include "savedata/my_pms_data.h"

#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/debug_beacon.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void BEACONINFO_Set_BattleWildPokeStart(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_BattleWildPokeVictory(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_BattleSpPokeStart(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_BattleSpPokeVictory(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_BattleTrainerStart(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleTrainerVictory(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleLeaderStart(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleLeaderVictory(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleBigFourStart(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleBigFourVictory(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleChampionStart(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_BattleChampionVictory(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_PokemonGet(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_SpecialPokemonGet(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_PokemonLevelUp(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_PokemonEvolution(GAMEBEACON_INFO *info, u16 monsno, const STRBUF *nickname);
extern void BEACONINFO_Set_GPower(GAMEBEACON_INFO *info, GPOWER_ID g_power_id);
extern void BEACONINFO_Set_SpItemGet(GAMEBEACON_INFO *info, u16 item);
extern void BEACONINFO_Set_PlayTime(GAMEBEACON_INFO *info, u32 hour);
extern void BEACONINFO_Set_ZukanComplete(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_ThankyouOver(GAMEBEACON_INFO *info, u32 thankyou_count);
extern void BEACONINFO_Set_UnionIn(GAMEBEACON_INFO *info);
#ifdef PM_DEBUG
extern void BEACONINFO_Set_DistributionPoke(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_DistributionItem(GAMEBEACON_INFO *info, u16 item);
extern void BEACONINFO_Set_DistributionEtc(GAMEBEACON_INFO *info);
#endif
extern void BEACONINFO_Set_CriticalHit(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_CriticalDamage(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_Escape(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_HPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_PPLittle(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_Dying(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_StateIsAbnormal(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_UseItem(GAMEBEACON_INFO *info, u16 item_no);
extern void BEACONINFO_Set_FieldSkill(GAMEBEACON_INFO *info, u16 wazano);
extern void BEACONINFO_Set_SodateyaEgg(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_EggHatch(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_Shoping(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_Subway(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_SubwayStraightVictories(GAMEBEACON_INFO *info, u32 victory_count);
extern void BEACONINFO_Set_SubwayVictoriesAchieve(GAMEBEACON_INFO *info, u32 victory_count);
extern void BEACONINFO_Set_TrialHouse(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_TrialHouseRank(GAMEBEACON_INFO *info, u8 rank);
extern void BEACONINFO_Set_FerrisWheel(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_PokeShifter(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_Musical(GAMEBEACON_INFO *info, const STRBUF *nickname);
extern void BEACONINFO_Set_OtherGPowerUse(GAMEBEACON_INFO *info, GPOWER_ID gpower_id);
extern void BEACONINFO_Set_Thankyou(GAMEBEACON_INFO *info, GAMEDATA *gamedata, u32 target_trainer_id);
extern void BEACONINFO_Set_ZoneChange(GAMEBEACON_INFO *info, ZONEID zone_id, const GAMEDATA *cp_gamedata);
extern void BEACONINFO_Set_ThanksRecvCount(GAMEBEACON_INFO *info, u32 count);
extern void BEACONINFO_Set_SuretigaiCount(GAMEBEACON_INFO *info, u32 count);

extern void BEACONINFO_Set_Details_BattleWildPoke(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_Details_BattleSpPoke(GAMEBEACON_INFO *info, u16 monsno);
extern void BEACONINFO_Set_Details_BattleTrainer(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_Details_BattleLeader(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_Details_BattleBigFour(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_Details_BattleChampion(GAMEBEACON_INFO *info, u16 tr_no);
extern void BEACONINFO_Set_Details_Walk(GAMEBEACON_INFO *info);
extern void BEACONINFO_Set_Details_IntroductionPms(GAMEBEACON_INFO *info, const PMS_DATA *pms);

extern GAMEBEACON_INFO * DEBUG_SendBeaconInfo_GetPtr(void);


//--------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------
static void _SetDebugBeaconInfo(GAMEBEACON_INFO *info, const DEBUG_GAMEBEACON_INFO *debuginfo, const DEBUG_ACTION_DATA *debugaction);


#if 0 //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "fieldmap/zone_id.h"
#include "item/itemsym.h"
//--------------------------------------------------------------
//  sample:�e�X�g�p���z�l���f�[�^
//--------------------------------------------------------------
static const DEBUG_GAMEBEACON_INFO DebugBeaconTbl[] = {
  {
    3,                            //���R�[�h
    1,                            //�n��R�[�h
    99999,                        //����Ⴂ�l��
    GPOWER_ID_2,                  //�������Ă���G�p���[
    VERSION_BLACK,                //PM_VERSION
    ZONE_ID_C02,                  //���ݒn
    PM_MALE,                      //����
    LANG_SPAIN,                   //PM_LANG
    ZONE_ID_C02,                  //�^�E���}�b�v�p��ROOT_ZONE_ID
    5,                            //���j�I�����[���ł̌�����(�O�`�P�T)
    RESEARCH_TEAM_RANK_1,         //�����������N
    8,                            //�{�̏��̐F(Index) �O�`�P�T
    12345,                        //������󂯂���
    55332,                        //�g���[�i�[ID
    {L'��',L'��',L'��',L'��',0xffff},   //�g���[�i�[��
    {L'��',L'��',L'��',L'��',L'��',L'��',L'�I',L'�H',0xffff},   //���ȏЉ�
    {2, 3, 10, 20},               //�ȈՉ�b(sentence_type, sentence_id, word_0, word_1)
    {                             //�f�t�H���g�s���f�[�^
      GAMEBEACON_ACTION_BATTLE_WILD_POKE_START,
      MONSNO_MYUU,
    },
  },
  {
    5,                            //���R�[�h
    0,                            //�n��R�[�h
    888,                          //����Ⴂ�l��
    GPOWER_ID_4,                  //�������Ă���G�p���[
    VERSION_WHITE,                //PM_VERSION
    ZONE_ID_C02GYM0101,           //���ݒn
    PM_FEMALE,                    //����
    LANG_JAPAN,                   //PM_LANG
    ZONE_ID_C02,                  //�^�E���}�b�v�p��ROOT_ZONE_ID
    9,                            //���j�I�����[���ł̌�����(�O�`�P�T)
    RESEARCH_TEAM_RANK_3,         //�����������N
    15,                           //�{�̏��̐F(Index) �O�`�P�T
    7,                            //������󂯂���
    104,                          //�g���[�i�[ID
    {L'W',L'h',L'i',L't',L'e',0xffff},   //�g���[�i�[��
    {L'H',L'e',L'l',L'l',L'o',L'!',0xffff},   //���ȏЉ�
    {0, 0, 1, 2},                 //�ȈՉ�b(sentence_type, sentence_id, word_0, word_1)
    {                             //�f�t�H���g�s���f�[�^
      GAMEBEACON_ACTION_SEARCH,
      0,
    },
  },
};

//--------------------------------------------------------------
//  sample:�s���f�[�^
//--------------------------------------------------------------
static const DEBUG_ACTION_DATA DebugActionTbl[] = {
  {
    GAMEBEACON_ACTION_SEARCH,
    0,
  },
  {
    GAMEBEACON_ACTION_BATTLE_WILD_POKE_START,
    MONSNO_MYUU,
  },
  {
    GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY,
    MONSNO_MYUU,
  },
  {
    GAMEBEACON_ACTION_BATTLE_TRAINER_START,
    20,
  },
  {
    GAMEBEACON_ACTION_BATTLE_LEADER_START,
    30,
  },
  {
    GAMEBEACON_ACTION_POKE_LVUP,
    L'��',L'��',L'��',L'��',L'��',0xffff,
  },
  {
    GAMEBEACON_ACTION_POKE_EVOLUTION,
    30, L'��',L'��',L'��',0xffff,
  },
  {
    GAMEBEACON_ACTION_GPOWER,
    GPOWER_ID_1,
  },
  {
    GAMEBEACON_ACTION_SP_ITEM_GET,
    ITEM_KOORINAOSI,
  },
  {
    GAMEBEACON_ACTION_THANKYOU_OVER,
    64317,
  },
};

#if 0
//��M�o�b�t�@�ֈꊇ�Z�b�g
for(i = 0; i < NELEMS(DebugBeaconTbl); i++){
  DebugRecvBuf_BeaconSet(&DebugBeaconTbl[i], NULL);
};


//�s���A�N�V�������w�肵�āA��M�o�b�t�@�փZ�b�g
                          ���Ώۂ̐l���f�[�^���w��      ���C�ӂ̍s���f�[�^���w��
  DebugRecvBuf_BeaconSet(&DebugBeaconTbl[select_no], &DebugActionTbl[select_action]);
  

//���M�r�[�R���������ł͂Ȃ��A�w�肵���l�ɐ�������đ��M����
  DebugSendBuf_BeaconSet(&DebugBeaconTbl[select_no]);
  GAMEBEACON_Set_xxx    ��{��񂪐����������͒ʏ�̑��M���߂��g�p����
#endif
#endif  //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



//==============================================================================
//
//  
//
//==============================================================================

//==================================================================
/**
 * �����̑��M�r�[�R�������z�l���f�[�^�ɐ������
 *
 * @param   debuginfo		���z�l���f�[�^
 */
//==================================================================
void DebugSendBuf_BeaconSet(const DEBUG_GAMEBEACON_INFO *debuginfo)
{
  GAMEBEACON_INFO *send = DEBUG_SendBeaconInfo_GetPtr();
  
  _SetDebugBeaconInfo(send, debuginfo, NULL);
}

//==================================================================
/**
 * �r�[�R����M�o�b�t�@�։��z�l���f�[�^����̃r�[�R���Ƃ��ċ����Z�b�g
 *
 * @param   debuginfo		  ���z�l���f�[�^
 * @param   debugaction		�s���f�[�^(NULL�̏ꍇ�͉��z�l���f�[�^�����f�t�H���g�s���f�[�^��K�p)
 */
//==================================================================
void DebugRecvBuf_BeaconSet(const DEBUG_GAMEBEACON_INFO *debuginfo, const DEBUG_ACTION_DATA *debugaction)
{
  GAMEBEACON_INFO info = {0};
  
  _SetDebugBeaconInfo(&info, debuginfo, debugaction);

  //��M�o�b�t�@�փZ�b�g
  info.send_counter = GFUser_GetPublicRand0(32); //�J�E���^�[�������悤�����_���Ŗ���擾
  if(GAMEBEACON_SetRecvBeacon(&info) == FALSE){
    info.send_counter++; //�����_�������\��������̂Ŏ��s�̏ꍇ�̓J�E���^��i�߂Ă������
    GAMEBEACON_SetRecvBeacon(&info);
  }
}

//--------------------------------------------------------------
/**
 * ���z�l���f�[�^����r�[�R�����쐬����
 *
 * @param   info		      �f�[�^�����
 * @param   debuginfo		  ���z�l���f�[�^
 * @param   debugaction		�s���f�[�^
 */
//--------------------------------------------------------------
static void _SetDebugBeaconInfo(GAMEBEACON_INFO *info, const DEBUG_GAMEBEACON_INFO *debuginfo, const DEBUG_ACTION_DATA *debugaction)
{
  const DEBUG_ACTION_DATA *src_action;
  int i;
  
  info->version_bit = 0xffff;
  info->zone_id = debuginfo->zone_id;
  info->g_power_id = debuginfo->g_power_id;
  info->trainer_id = debuginfo->trainer_id;
  info->favorite_color_index = debuginfo->favorite_color_index;
  info->trainer_view = debuginfo->trainer_view;
  info->sex = debuginfo->sex;
  info->pm_version = debuginfo->pm_version;
  info->language = debuginfo->language;
  info->nation = debuginfo->nation;
  info->area = debuginfo->area;
  info->research_team_rank = debuginfo->research_team_rank;
  info->thanks_recv_count = debuginfo->thanks_recv_count;
  info->suretigai_count = debuginfo->suretigai_count;
  info->townmap_root_zone_id = debuginfo->townmap_root_zone_id;
  
  BEACONINFO_Set_Details_IntroductionPms(info, &debuginfo->pmsdata);
  
  for(i = 0; i < PERSON_NAME_SIZE; i++){
    info->name[i] = debuginfo->name[i];
  }
  for(i = 0; i < GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN; i++){
    info->self_introduction[i] = debuginfo->self_introduction[i];
  }
  
  //�s���f�[�^�Z�b�g
  if(debugaction != NULL){
    src_action = debugaction;
  }
  else{
    src_action = &debuginfo->action_data;
  }
  switch(src_action->action_no){
  case GAMEBEACON_ACTION_SEARCH:                   ///<�u������������T�[�`���܂����I�v      1
    info->action.action_no = GAMEBEACON_ACTION_SEARCH;
    break;
  case GAMEBEACON_ACTION_BATTLE_WILD_POKE_START:   ///<�쐶�̃|�P�����Ƒΐ���J�n���܂����I  2
    BEACONINFO_Set_BattleWildPokeStart(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY: ///<�쐶�̃|�P�����ɏ������܂����I        3
    BEACONINFO_Set_BattleWildPokeVictory(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_SP_POKE_START:     ///<���ʂȃ|�P�����Ƒΐ���J�n���܂����I  4
    BEACONINFO_Set_BattleSpPokeStart(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY:   ///<���ʂȃ|�P�����ɏ������܂����I        5
    BEACONINFO_Set_BattleSpPokeVictory(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_TRAINER_START:     ///<�g���[�i�[�Ƒΐ���J�n���܂����I      6
    BEACONINFO_Set_BattleTrainerStart(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY:   ///<�g���[�i�[�ɏ������܂����I            7
    BEACONINFO_Set_BattleTrainerVictory(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_LEADER_START:      ///<�W�����[�_�[�Ƒΐ���J�n���܂����I    8
    BEACONINFO_Set_BattleLeaderStart(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY:    ///<�W�����[�_�[�ɏ������܂����I          9
    BEACONINFO_Set_BattleLeaderVictory(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_BIGFOUR_START:     ///<�l�V���Ƒΐ���J�n���܂����I          10
    BEACONINFO_Set_BattleBigFourStart(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_BIGFOUR_VICTORY:   ///<�l�V���ɏ������܂����I                11
    BEACONINFO_Set_BattleBigFourVictory(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_CHAMPION_START:    ///<�`�����s�I���Ƒΐ���J�n���܂����I    12
    BEACONINFO_Set_BattleChampionStart(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY:  ///<�`�����s�I���ɏ������܂����I          13
    BEACONINFO_Set_BattleChampionVictory(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_POKE_GET:                 ///<�|�P�����ߊl                          14
    BEACONINFO_Set_PokemonGet(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_SP_POKE_GET:              ///<���ʂȃ|�P�����ߊl                    15
    BEACONINFO_Set_SpecialPokemonGet(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_POKE_LVUP:                ///<�|�P�������x���A�b�v                  16
    {
      STRBUF *temp_nickname = GFL_STR_CreateBuffer(BUFLEN_POKEMON_NAME, HEAPID_WORLD);
    	GFL_STR_SetStringCodeOrderLength(temp_nickname, src_action->param, BUFLEN_POKEMON_NAME);
      BEACONINFO_Set_PokemonLevelUp(info, temp_nickname);
      GFL_STR_DeleteBuffer(temp_nickname);
    }
    break;
  case GAMEBEACON_ACTION_POKE_EVOLUTION:           ///<�|�P�����i��                          17
    {
      STRBUF *temp_nickname = GFL_STR_CreateBuffer(BUFLEN_POKEMON_NAME, HEAPID_WORLD);
    	GFL_STR_SetStringCodeOrderLength(temp_nickname, &src_action->param[1], BUFLEN_POKEMON_NAME);
      BEACONINFO_Set_PokemonEvolution(info, src_action->param[0], temp_nickname);
      GFL_STR_DeleteBuffer(temp_nickname);
    }
    break;
  case GAMEBEACON_ACTION_GPOWER:                   ///<G�p���[����                           18
    BEACONINFO_Set_GPower(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_SP_ITEM_GET:              ///<�M�d�i�Q�b�g                          19
    BEACONINFO_Set_SpItemGet(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_PLAYTIME:                 ///<���̃v���C���Ԃ��z����              20
    BEACONINFO_Set_PlayTime(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_ZUKAN_COMPLETE:           ///<�}�ӊ���                              21
    BEACONINFO_Set_ZukanComplete(info);
    break;
  case GAMEBEACON_ACTION_THANKYOU_OVER:            ///<������󂯂��񐔂��K�萔�𒴂���      22
    BEACONINFO_Set_ThankyouOver(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_UNION_IN:                 ///<���j�I�����[���ɓ�����                23
    BEACONINFO_Set_UnionIn(info);
    break;
  case GAMEBEACON_ACTION_THANKYOU:                 ///<�u���肪�Ƃ��I�v                      24
    GF_ASSERT(0); //���Ή�
    //BEACONINFO_Set_Thankyou(GAMEBEACON_INFO *info, GAMEDATA *gamedata, u32 target_trainer_id);
    break;
  case GAMEBEACON_ACTION_DISTRIBUTION_POKE:        ///<�|�P�����z�z��                        25
    BEACONINFO_Set_DistributionPoke(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_DISTRIBUTION_ITEM:        ///<�A�C�e���z�z��                        26
    BEACONINFO_Set_DistributionItem(info, src_action->param[0]);
    break;
  case GAMEBEACON_ACTION_DISTRIBUTION_ETC:         ///<���̑��z�z��                          27
    BEACONINFO_Set_DistributionEtc(info);
    break;
  default:
    GF_ASSERT(0);
    break;
  }
}

#endif  //PM_DEBUG
