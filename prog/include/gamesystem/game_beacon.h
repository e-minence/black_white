//==============================================================================
/**
 * @file    game_beacon.h
 * @brief   
 * @author  matsuda
 * @date    2009.12.13(��)
 */
//==============================================================================
#pragma once

#include "buflen.h"
#include "gamesystem/gamedata_def.h"
#include "print/wordset.h"
#include "field/gpower_id.h"
#include "gamesystem/playerwork.h"  //typedef ZONEID
#include "savedata/questionnaire_save.h"


//==============================================================================
//  �萔��`
//==============================================================================
///GAMEBEACON_INFO_TBL�ŋL�^���郍�O����
#define GAMEBEACON_INFO_TBL_MAX   (30)
#define GAMEBEACON_INFO_TBL_END   (GAMEBEACON_INFO_TBL_MAX-1)

///�ڍ�No
typedef enum{
  GAMEBEACON_DETAILS_NO_BATTLE_WILD_POKE,       ///<�쐶�|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_SP_POKE,         ///<���ʂȃ|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_TRAINER,         ///<�g���[�i�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_JIMLEADER,       ///<�W�����[�_�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_SP_TRAINER,      ///<���ʂȃg���[�i�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_WALK,                   ///<�ړ���
  
  GAMEBEACON_DETAILS_NO_MAX,
}GAMEBEACON_DETAILS_NO;


///�s���ԍ�
typedef enum{
  GAMEBEACON_ACTION_NULL,                     ///<�f�[�^����
  
  GAMEBEACON_ACTION_SEARCH,                   ///<�u������������T�[�`���܂����I�v      1
  GAMEBEACON_ACTION_BATTLE_WILD_POKE_START,   ///<�쐶�̃|�P�����Ƒΐ���J�n���܂����I  2
  GAMEBEACON_ACTION_BATTLE_WILD_POKE_VICTORY, ///<�쐶�̃|�P�����ɏ������܂����I        3
  GAMEBEACON_ACTION_BATTLE_SP_POKE_START,     ///<���ʂȃ|�P�����Ƒΐ���J�n���܂����I  4
  GAMEBEACON_ACTION_BATTLE_SP_POKE_VICTORY,   ///<���ʂȃ|�P�����ɏ������܂����I        5
  GAMEBEACON_ACTION_BATTLE_TRAINER_START,     ///<�g���[�i�[�Ƒΐ���J�n���܂����I      6
  GAMEBEACON_ACTION_BATTLE_TRAINER_VICTORY,   ///<�g���[�i�[�ɏ������܂����I            7
  GAMEBEACON_ACTION_BATTLE_LEADER_START,      ///<�W�����[�_�[�Ƒΐ���J�n���܂����I    8
  GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY,    ///<�W�����[�_�[�ɏ������܂����I          9
  GAMEBEACON_ACTION_BATTLE_SP_TRAINER_START,  ///<���ʂȃg���[�i�[�Ƒΐ���J�n���܂����I      10
  GAMEBEACON_ACTION_BATTLE_SP_TRAINER_VICTORY,///<���ʂȃg���[�i�[�ɏ������܂����I            11
  GAMEBEACON_ACTION_BATTLE_CHAMPION_START,    ///<�`�����s�I���Ƒΐ���J�n���܂����I(����)    12
  GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY,  ///<�`�����s�I���ɏ������܂����I(����)          13
  GAMEBEACON_ACTION_POKE_GET,                 ///<�|�P�����ߊl                          14
  GAMEBEACON_ACTION_SP_POKE_GET,              ///<���ʂȃ|�P�����ߊl                    15
  GAMEBEACON_ACTION_POKE_LVUP,                ///<�|�P�������x���A�b�v                  16
  GAMEBEACON_ACTION_POKE_EVOLUTION,           ///<�|�P�����i��                          17
  GAMEBEACON_ACTION_GPOWER,                   ///<G�p���[����                           18
  GAMEBEACON_ACTION_SP_ITEM_GET,              ///<�M�d�i�Q�b�g                          19
  GAMEBEACON_ACTION_PLAYTIME,                 ///<���̃v���C���Ԃ��z����              20
  GAMEBEACON_ACTION_ZUKAN_COMPLETE,           ///<�}�ӊ���                              21
  GAMEBEACON_ACTION_THANKYOU_OVER,            ///<������󂯂��񐔂��K�萔�𒴂���      22
  GAMEBEACON_ACTION_UNION_IN,                 ///<���j�I�����[���ɓ�����                23
  GAMEBEACON_ACTION_THANKYOU,                 ///<�u���肪�Ƃ��I�v                      24
  GAMEBEACON_ACTION_DISTRIBUTION_POKE,        ///<�|�P�����z�z��                        25
  GAMEBEACON_ACTION_DISTRIBUTION_ITEM,        ///<�A�C�e���z�z��                        26
  GAMEBEACON_ACTION_DISTRIBUTION_ETC,         ///<���̑��z�z��                          27
  GAMEBEACON_ACTION_CRITICAL_HIT,             ///<�}���ɍU�������Ă�                    28
  GAMEBEACON_ACTION_CRITICAL_DAMAGE,          ///<�}���ɍU�����󂯂�                    29
  GAMEBEACON_ACTION_ESCAPE,                   ///<�퓬���瓦���o����                    30
  GAMEBEACON_ACTION_HP_LITTLE,                ///<HP���c�菭�Ȃ�                        31
  GAMEBEACON_ACTION_PP_LITTLE,                ///<PP���c�菭�Ȃ�                        32
  GAMEBEACON_ACTION_DYING,                    ///<�擪�̃|�P�������m��                  33
  GAMEBEACON_ACTION_STATE_IS_ABNORMAL,        ///<�擪�̃|�P��������Ԉُ�              34
  GAMEBEACON_ACTION_USE_ITEM,                 ///<�A�C�e�����g�p                        35
  GAMEBEACON_ACTION_FIELD_SKILL,              ///<�t�B�[���h�Z���g�p                    36
  GAMEBEACON_ACTION_SODATEYA_EGG,             ///<��ĉ����痑�����������              37
  GAMEBEACON_ACTION_EGG_HATCH,                ///<�^�}�S���z������                      38
  GAMEBEACON_ACTION_SHOPING,                  ///<��������                              39
  GAMEBEACON_ACTION_SUBWAY,                   ///<�o�g���T�u�E�F�C���풆                40
  GAMEBEACON_ACTION_SUBWAY_STRAIGHT_VICTORIES,  ///<�o�g���T�u�E�F�C�A����              41
  GAMEBEACON_ACTION_SUBWAY_TROPHY_GET,          ///<�o�g���T�u�E�F�C�g���t�B�[�Q�b�g    42
  GAMEBEACON_ACTION_TRIALHOUSE,               ///<�g���C�A���n�E�X�ɒ��풆              43
  GAMEBEACON_ACTION_TRIALHOUSE_RANK,          ///<�g���C�A���n�E�X�Ń����N�m��          44
  GAMEBEACON_ACTION_FERRIS_WHEEL,             ///<�ϗ��Ԃɏ����                        45
  GAMEBEACON_ACTION_POKESHIFTER,              ///<�|�P�V�t�^�[�ɓ�����                  46
  GAMEBEACON_ACTION_MUSICAL,                  ///<�~���[�W�J�����풆                    47
  GAMEBEACON_ACTION_OTHER_GPOWER_USE,         ///<���l��G�p���[���g�p                   48
  GAMEBEACON_ACTION_FREEWORD,                 ///<�ꌾ���b�Z�[�W                        49
  GAMEBEACON_ACTION_DISTRIBUTION_GPOWER,      ///<G�p���[�z�z                           50
  
//  GAMEBEACON_ACTION_CONGRATULATIONS,      ///<�u���߂łƂ��I�v
//  GAMEBEACON_ACTION_UNION_OUT,            ///<���j�I�����[���ގ�
//  GAMEBEACON_ACTION_ENCOUNT_DOWN,         ///<�G���J�E���g���_�E��
  
  GAMEBEACON_ACTION_MAX,
}GAMEBEACON_ACTION;


//==============================================================================
//  �^��`
//==============================================================================
///����M�����r�[�R���p�����[�^
typedef struct _GAMEBEACON_INFO GAMEBEACON_INFO;

///�r�[�R���p�����[�^�e�[�u��
typedef struct _GAMEBEACON_INFO_TBL GAMEBEACON_INFO_TBL;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void GAMEBEACON_Init(HEAPID heap_id);
extern void GAMEBEACON_Exit(void);
extern void GAMEBEACON_Update(void);
extern void GAMEBEACON_Setting(GAMEDATA *gamedata);
extern void GAMEBEACON_SendDataCopy(GAMEBEACON_INFO *info);
extern void GAMEBEACON_SendBeaconUpdate(void);

//--------------------------------------------------------------
//  �r�[�R�����擾
//--------------------------------------------------------------
extern BOOL GAMEBEACON_SetRecvBeacon(const GAMEBEACON_INFO *info);
extern const GAMEBEACON_INFO * GAMEBEACON_Get_BeaconLog(int log_no);
extern u32 GAMEBEACON_Get_LogCount(void);
extern int GAMEBEACON_Get_UpdateLogNo(int *start_log_no);
extern void GAMEBEACON_Reset_UpdateFlag(int log_no);
extern BOOL GAMEBEACON_Set_SearchUpdateFlag(const GAMEBEACON_INFO *info);
extern u16 GAMEBEACON_Get_RecvBeaconTime(u16 trainer_id);

//--------------------------------------------------------------
//  �r�[�R���Z�b�g
//--------------------------------------------------------------
extern void GAMEBEACON_Set_BattlePokeStart(u16 monsno);
extern void GAMEBEACON_Set_BattlePokeVictory(u16 monsno);
extern void GAMEBEACON_Set_BattleWildPokeStart(u16 monsno);
extern void GAMEBEACON_Set_BattleWildPokeVictory(u16 monsno);
extern void GAMEBEACON_Set_BattleSpPokeStart(u16 monsno);
extern void GAMEBEACON_Set_BattleSpPokeVictory(u16 monsno);
extern void GAMEBEACON_Set_BattleTrainerStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleTrainerVictory(u16 tr_no);
extern void GAMEBEACON_Set_BattleLeaderStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleLeaderVictory(u16 tr_no);
extern void GAMEBEACON_Set_BattleSpTrainerStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleSpTrainerVictory(u16 tr_no);
extern void GAMEBEACON_Set_BattleChampionStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleChampionVictory(u16 tr_no);
extern void GAMEBEACON_Set_PokemonGet(u16 monsno);
extern void GAMEBEACON_Set_WildPokemonGet(u16 monsno);
extern void GAMEBEACON_Set_SpecialPokemonGet(u16 monsno);
extern void GAMEBEACON_Set_PokemonLevelUp(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonEvolution(u16 monsno, const STRBUF *nickname);
extern void GAMEBEACON_Set_GPower(GPOWER_ID g_power_id);
extern void GAMEBEACON_Set_SpItemGet(u16 item);
extern void GAMEBEACON_Set_PlayTime(u32 hour);
extern void GAMEBEACON_Set_ZukanComplete(void);
extern void GAMEBEACON_Set_ThankyouOver(u32 thankyou_count);
extern void GAMEBEACON_Set_UnionIn(void);
#ifdef PM_DEBUG //�C�x���g�p�̔z�zROM�ł������M�͂��Ȃ��̂Ńe�X�g�p
extern void GAMEBEACON_Set_DistributionPoke(u16 monsno);
extern void GAMEBEACON_Set_DistributionItem(u16 item);
extern void GAMEBEACON_Set_DistributionEtc(void);
extern void GAMEBEACON_Set_DistributionGPower(GPOWER_ID gpower_id);
#endif
extern void GAMEBEACON_Set_CriticalHit(const STRBUF *nickname);
extern void GAMEBEACON_Set_CriticalDamage(const STRBUF *nickname);
extern void GAMEBEACON_Set_Escape(void);
extern void GAMEBEACON_Set_HPLittle(const STRBUF *nickname);
extern void GAMEBEACON_Set_PPLittle(const STRBUF *nickname);
extern void GAMEBEACON_Set_Dying(const STRBUF *nickname);
extern void GAMEBEACON_Set_StateIsAbnormal(const STRBUF *nickname);
extern void GAMEBEACON_Set_UseItem(u16 item_no);
extern void GAMEBEACON_Set_FieldSkill(u16 wazano);
extern void GAMEBEACON_Set_SodateyaEgg(void);
extern void GAMEBEACON_Set_EggHatch(u16 monsno);
extern void GAMEBEACON_Set_Shoping(void);
extern void GAMEBEACON_Set_Subway(void);
extern void GAMEBEACON_Set_SubwayStraightVictories(u32 victory_count);
extern void GAMEBEACON_Set_SubwayTrophyGet(void);
extern void GAMEBEACON_Set_TrialHouse(void);
extern void GAMEBEACON_Set_TrialHouseRank(u8 rank);
extern void GAMEBEACON_Set_FerrisWheel(void);
extern void GAMEBEACON_Set_PokeShifter(void);
extern void GAMEBEACON_Set_Musical(const STRBUF *nickname);
extern void GAMEBEACON_Set_OtherGPowerUse(GPOWER_ID gpower_id);
extern void GAMEBEACON_Set_Thankyou(GAMEDATA *gamedata, u32 target_trainer_id);
extern void GAMEBEACON_Set_FreeWord(const STRBUF *free_word);

//=====�ڍ׃p�����[�^�Z�b�g
extern void GAMEBEACON_Set_ZoneChange(ZONEID zone_id, const GAMEDATA *cp_gamedata);
extern void GAMEBEACON_Set_Details_IntroductionPms(const PMS_DATA *pms);

//--------------------------------------------------------------
//  ���M�o�b�t�@�X�V
//--------------------------------------------------------------
extern void GAMEBEACON_SendDataUpdate_NationArea(u8 nation, u8 area);
extern void GAMEBEACON_SendDataUpdate_ResearchTeamRank(u8 research_team_rank);
extern void GAMEBEACON_SendDataUpdate_TrCardIntroduction(const PMS_DATA *pms);
extern void GAMEBEACON_SendDataUpdate_SelfIntroduction(void);
extern void GAMEBEACON_SendDataUpdate_TrainerView(int trainer_view);

//--------------------------------------------------------------
//  �A���P�[�g
//--------------------------------------------------------------
extern BOOL GAMEBEACON_Get_NewEntry(void);
extern void GAMEBEACON_SendDataUpdate_Questionnaire(QUESTIONNAIRE_ANSWER_WORK *my_ans);
#ifdef PM_DEBUG
extern void DEBUG_GAMEBEACON_Set_NewEntry(void);
extern GAMEBEACON_INFO * DEBUG_SendBeaconInfo_GetPtr(void);
extern void DEBUG_SendBeaconCancel(void);
extern void DEBUG_RecvBeaconBufferClear(void);
#endif

