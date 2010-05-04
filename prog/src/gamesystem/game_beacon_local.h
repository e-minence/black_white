//==============================================================================
/**
 * @file    game_beacon_local.h
 * @brief   �Q�[�����̃r�[�R�����Ǘ��V�X�e�� ���[�J���w�b�_
 * @author  matsuda & iwasawa
 * @date    2010.05.01
 *
 * game_beacon.c game_beacon_nw.c �ȊO�ւ̊O�����J�֎~�I
 */
//==============================================================================

#pragma once


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
  u16 backup_hour;                    ///<�v���C���ԃo�b�N�A�b�v
  u16 padding;
  
#ifdef PM_DEBUG
  BOOL  deb_beacon_priority_egnore;  //TRUE�Ȃ�r�[�R�����N�G�X�g���Ƀv���C�I���e�B����𖳎�����
#endif
}GAMEBEACON_SYSTEM;

///�Q�[���r�[�R���Ǘ��V�X�e���̃|�C���^
extern GAMEBEACON_SYSTEM * GameBeaconSys;


//--------------------------------------------------------------
/**
 * ���M�r�[�R�����Z�b�g�������̋��ʏ���
 *
 * @param   send		
 * @param   info		
 */
//--------------------------------------------------------------
extern void GAMEBEACON_SUB_SendBeaconSetCommon(GAMEBEACON_SEND_MANAGER *send);

//--------------------------------------------------------------
/**
 * �A�N�V�����ԍ��̃v���C�I���e�B�`�F�b�N
 *
 * @param   action_no		
 *
 * @retval  BOOL		    TRUE:�v���C�I���e�B�������ȏ�B�@FALSE:�v���C�I���e�B����
 */
//--------------------------------------------------------------
extern BOOL GAMEBEACON_SUB_CheckPriority(u16 action_no);

//==================================================================
/**
 * �|�P�����֘A�r�[�R�� �^�C�v�`�F�b�N
 *
 * @param   monsno
 */
//==================================================================
extern BOOL GAMEBEACON_SUB_PokeBeaconTypeIsSpecial(const monsno );

//--------------------------------------------------------------
/**
 * �j�b�N�l�[���̃R�s�[�@����������Full�̏ꍇ��EOM���������܂Ȃ��̂œƎ��R�s�[����
 *
 * @param   nickname		    �R�s�[���j�b�N�l�[��
 * @param   dest_nickname		�j�b�N�l�[���R�s�[��
 */
//--------------------------------------------------------------
extern void GAMEBEACON_SUB_StrbufNicknameCopy(const STRBUF *nickname, STRCODE *dest_nickname);

///////////////////////////////////////////////////////////////////////////
//�r�[�R���Z�b�g�n
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
void BEACONINFO_Set_PlayTime(GAMEBEACON_INFO *info, u32 hour);
void BEACONINFO_Set_SpItemGet(GAMEBEACON_INFO *info, u16 item);
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

