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
  GAMEBEACON_DETAILS_NO_BATTLE_TRAINER,         ///<�g���[�i�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_JIMLEADER,       ///<�W�����[�_�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_BIGFOUR,         ///<�l�V���Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_CHAMPION,        ///<�`�����s�I���Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_POKEMON,         ///<�쐶�|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_SPECIAL_POKEMON, ///<���ʂȃ|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_WALK,                   ///<�ړ���
  
  GAMEBEACON_DETAILS_NO_MAX,
}GAMEBEACON_DETAILS_NO;


///�s���ԍ�
typedef enum{
  GAMEBEACON_ACTION_NULL,                     ///<�f�[�^����
  
  GAMEBEACON_ACTION_SEARCH,                   ///<�u������������T�[�`���܂����I�v
  GAMEBEACON_ACTION_BATTLE_LEADER_START,      ///<�W�����[�_�[�Ƒΐ���J�n���܂����I
  GAMEBEACON_ACTION_BATTLE_LEADER_VICTORY,    ///<�W�����[�_�[�ɏ������܂����I
  GAMEBEACON_ACTION_BATTLE_BIGFOUR_START,     ///<�l�V���Ƒΐ���J�n���܂����I
  GAMEBEACON_ACTION_BATTLE_BIGFOUR_VICTORY,   ///<�l�V���ɏ������܂����I
  GAMEBEACON_ACTION_BATTLE_CHAMPION_START,    ///<�`�����s�I���Ƒΐ���J�n���܂����I
  GAMEBEACON_ACTION_BATTLE_CHAMPION_VICTORY,  ///<�`�����s�I���ɏ������܂����I
  GAMEBEACON_ACTION_POKE_GET,                 ///<�|�P�����ߊl
  GAMEBEACON_ACTION_SP_POKE_GET,              ///<���ʂȃ|�P�����ߊl
  GAMEBEACON_ACTION_POKE_LVUP,                ///<�|�P�������x���A�b�v
  GAMEBEACON_ACTION_POKE_EVOLUTION,           ///<�|�P�����i��
  GAMEBEACON_ACTION_GPOWER,                   ///<G�p���[����
  GAMEBEACON_ACTION_SP_ITEM_GET,              ///<�M�d�i�Q�b�g
  GAMEBEACON_ACTION_PLAYTIME,                 ///<���̃v���C���Ԃ��z����
  GAMEBEACON_ACTION_ZUKAN_COMPLETE,           ///<�}�ӊ���
  GAMEBEACON_ACTION_THANKYOU_OVER,            ///<������󂯂��񐔂��K�萔�𒴂���
  GAMEBEACON_ACTION_UNION_IN,                 ///<���j�I�����[���ɓ�����
  GAMEBEACON_ACTION_DISTRIBUTION_POKE,        ///<�|�P�����z�z��
  GAMEBEACON_ACTION_DISTRIBUTION_ITEM,        ///<�A�C�e���z�z��
  GAMEBEACON_ACTION_DISTRIBUTION_ETC,         ///<���̑��z�z��
  GAMEBEACON_ACTION_THANKYOU,                 ///<�u���肪�Ƃ��I�v
  
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

//--------------------------------------------------------------
//  �r�[�R���Z�b�g
//--------------------------------------------------------------
extern void GAMEBEACON_Set_BattleLeaderStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleLeaderVictory(u16 tr_no);
extern void GAMEBEACON_Set_BattleBigFourStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleBigFourVictory(u16 tr_no);
extern void GAMEBEACON_Set_BattleChampionStart(u16 tr_no);
extern void GAMEBEACON_Set_BattleChampionVictory(u16 tr_no);
extern void GAMEBEACON_Set_PokemonGet(const STRBUF *nickname);
extern void GAMEBEACON_Set_SpecialPokemonGet(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonLevelUp(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonEvolution(const STRBUF *nickname);
extern void GAMEBEACON_Set_GPower(GPOWER_ID g_power_id);
extern void GAMEBEACON_Set_Thankyou(GAMEDATA *gamedata, u32 target_trainer_id);
extern void GAMEBEACON_Set_UnionIn(void);

//=====�ڍ׃p�����[�^�Z�b�g
extern void GAMEBEACON_Set_EncountDown(void);
extern void GAMEBEACON_Set_ZoneChange(ZONEID zone_id);
extern void GAMEBEACON_Set_ThanksRecvCount(u16 count);
extern void GAMEBEACON_Set_SuretigaiCount(u16 count);
extern void GAMEBEACON_Set_BattleTrainer(u16 trainer_code);
extern void GAMEBEACON_Set_BattlePokemon(u16 monsno);

//--------------------------------------------------------------
//  �폜�\��
//--------------------------------------------------------------
extern u32 GAMEBEACON_GetMsgID(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Wordset(const GAMEBEACON_INFO *info, WORDSET *wordset, HEAPID temp_heap_id);
