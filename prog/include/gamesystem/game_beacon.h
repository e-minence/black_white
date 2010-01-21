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

///�ڍ�No
typedef enum{
  GAMEBEACON_DETAILS_NO_BATTLE_TRAINER,         ///<�g���[�i�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_JIMLEADER,       ///<�W�����[�_�[�Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_BIGFOUR,         ///<�l�V���Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_CHAMPION,        ///<�`�����s�I���Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_POKEMON,         ///<�쐶�|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_REGEND_POKEMON,  ///<�`���|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_BATTLE_VISION_POKEMON,  ///<���|�P�����Ƒΐ풆
  GAMEBEACON_DETAILS_NO_ROAD,                   ///<���H���ړ���
  GAMEBEACON_DETAILS_NO_TOWN,                   ///<�X�A�_���W�������ړ���
  GAMEBEACON_DETAILS_NO_JIM,                    ///<�W�����ړ���
  
  GAMEBEACON_DETAILS_NO_MAX,
}GAMEBEACON_DETAILS_NO;


///�s���ԍ�
typedef enum{
  GAMEBEACON_ACTION_NULL,                 ///<�f�[�^����
  GAMEBEACON_ACTION_APPEAL,               ///<�u�������ɂ��܂��v
  GAMEBEACON_ACTION_CONGRATULATIONS,      ///<�u���߂łƂ��I�v
  GAMEBEACON_ACTION_POKE_EVOLUTION,       ///<�|�P�����i��
  GAMEBEACON_ACTION_POKE_LVUP,            ///<�|�P�������x���A�b�v
  GAMEBEACON_ACTION_POKE_GET,             ///<�|�P�����ߊl
  GAMEBEACON_ACTION_UNION_IN,             ///<���j�I�����[������
  GAMEBEACON_ACTION_UNION_OUT,            ///<���j�I�����[���ގ�
  GAMEBEACON_ACTION_ENCOUNT_DOWN,         ///<�G���J�E���g���_�E��
  
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

//--------------------------------------------------------------
//  �r�[�R���Z�b�g
//--------------------------------------------------------------
extern void GAMEBEACON_Set_Congratulations(void);
extern void GAMEBEACON_Set_PokemonEvolution(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonLevelUp(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonGet(const STRBUF *nickname);
extern void GAMEBEACON_Set_UnionIn(void);
extern void GAMEBEACON_Set_UnionOut(void);
extern void GAMEBEACON_Set_EncountDown(void);
extern void GAMEBEACON_Set_ZoneChange(ZONEID zone_id);
extern void GAMEBEACON_Set_GPower(GPOWER_ID g_power_id);
extern void GAMEBEACON_Set_ThanksRecvCount(u16 count);
extern void GAMEBEACON_Set_SuretigaiCount(u16 count);
extern void GAMEBEACON_Set_BattleTrainer(u16 trainer_code);
extern void GAMEBEACON_Set_BattlePokemon(u16 monsno);

//--------------------------------------------------------------
//  �폜�\��
//--------------------------------------------------------------
extern u32 GAMEBEACON_GetMsgID(const GAMEBEACON_INFO *info);
extern void GAMEBEACON_Wordset(const GAMEBEACON_INFO *info, WORDSET *wordset, HEAPID temp_heap_id);
