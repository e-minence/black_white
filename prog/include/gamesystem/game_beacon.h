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


//==============================================================================
//  �萔��`
//==============================================================================
///���O����
#define GAMEBEACON_LOG_MAX       (20)


//==============================================================================
//  �\���̒�`
//==============================================================================
///����M�����r�[�R���p�����[�^
typedef struct{
  u16 action_no;                            ///<�s��No
  u16 send_counter;                         ///<���MNo(�����f�[�^�̖�������Ɏg�p)
  u32 trainer_id;                           ///<�g���[�i�[ID
  STRCODE name[BUFLEN_PERSON_NAME];         ///<�g���[�i�[��
  
  //action_no�ɂ���ăZ�b�g�������̂��ς��
  union{
    STRCODE nickname[BUFLEN_POKEMON_NAME];  ///<�|�P������(�j�b�N�l�[��)
  };
}GAMEBEACON_INFO;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void GAMEBEACON_Init(HEAPID heap_id);
extern void GAMEBEACON_Exit(void);
extern void GAMEBEACON_Update(void);
extern void GAMEBEACON_Setting(GAMEDATA *gamedata);
extern void GAMEBEACON_SendDataCopy(GAMEBEACON_INFO *info);

extern BOOL GAMEBEACON_SetRecvBeacon(const GAMEBEACON_INFO *info);
extern const GAMEBEACON_INFO * GAMEBEACON_Get_BeaconLog(int log_no);

//--------------------------------------------------------------
//  �r�[�R���Z�b�g
//--------------------------------------------------------------
extern void GAMEBEACON_Set_Congratulations(void);
extern void GAMEBEACON_Set_PokemonEvolution(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonLevelUp(const STRBUF *nickname);
extern void GAMEBEACON_Set_PokemonGet(const STRBUF *nickname);
extern void GAMEBEACON_Set_UnionIn(void);
extern void GAMEBEACON_Set_UnionOut(void);
