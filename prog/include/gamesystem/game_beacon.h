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
  GAMEBEACON_DETAILS_NO_BATTLE_BIGFOUR,         ///<�l�V���Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_CHAMPION,        ///<�`�����s�I���Ƒΐ풆
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
  GAMEBEACON_ACTION_BATTLE_BIGFOUR_START,     ///<�l�V���Ƒΐ���J�n���܂����I          10
  GAMEBEACON_ACTION_BATTLE_BIGFOUR_VICTORY,   ///<�l�V���ɏ������܂����I                11
  GAMEBEACON_ACTION_BATTLE_CHAMPION_START,    ///<�`�����s�I���Ƒΐ���J�n���܂����I    12
  GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY,  ///<�`�����s�I���ɏ������܂����I          13
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
  
//  GAMEBEACON_ACTION_CONGRATULATIONS,      ///<�u���߂łƂ��I�v
//  GAMEBEACON_ACTION_UNION_OUT,            ///<���j�I�����[���ގ�
//  GAMEBEACON_ACTION_ENCOUNT_DOWN,         ///<�G���J�E���g���_�E��
  
  GAMEBEACON_ACTION_MAX,
}GAMEBEACON_ACTION;


///�����������N
typedef enum{
  RESEARCH_TEAM_RANK_0,
  RESEARCH_TEAM_RANK_1,
  RESEARCH_TEAM_RANK_2,
  RESEARCH_TEAM_RANK_3,
  RESEARCH_TEAM_RANK_4,
  RESEARCH_TEAM_RANK_5,
  RESEARCH_TEAM_RANK_MAX,
}RESEARCH_TEAM_RANK;


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
extern void GAMEBEACON_Set_BattleBigFourStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleBigFourVictory(u16 tr_no);
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
#endif
extern void GAMEBEACON_Set_Thankyou(GAMEDATA *gamedata, u32 target_trainer_id);

//=====�ڍ׃p�����[�^�Z�b�g
extern void GAMEBEACON_Set_ZoneChange(ZONEID zone_id, const GAMEDATA *cp_gamedata);
extern void GAMEBEACON_Set_Details_IntroductionPms(const PMS_DATA *pms);

//--------------------------------------------------------------
//  �A���P�[�g
//--------------------------------------------------------------
extern BOOL GAMEBEACON_Get_NewEntry(void);
#ifdef PM_DEBUG
extern void DEBUG_GAMEBEACON_Set_NewEntry(void);
#endif

