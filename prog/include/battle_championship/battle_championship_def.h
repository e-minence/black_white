//======================================================================
/**
 * @file	battle_championship_def.h
 * @brief	�o�g�����n��`
 * @author	ariizumi
 * @data	09/10/06
 *
 * ���W���[�����FBATTLE_CHAMPIONSHIP
 */
//======================================================================
#pragma once

#include "buflen.h"
#include "poke_tool/poke_tool.h"
#include "savedata/save_control.h"
#include "savedata/regulation.h"

#define CHAMPIONSHIP_NAME_LEN (36+2)  //36+EOM
#define CHAMPIONSHIP_POKE_NUM (6)

typedef struct
{
  STRCODE name[CHAMPIONSHIP_NAME_LEN];
  u8      number; //���Ǘ��ԍ�
  u8      league; //�������[�O(�W���j�A�E�V�j�A)
  
  REGULATION regulation;
  
  POKEMON_PASO_PARAM *ppp[CHAMPIONSHIP_POKE_NUM];
  
}BATTLE_CHAMPIONSHIP_DATA;


//�f�o�b�O�p
extern void BATTLE_CHAMPIONSHIP_SetDebugData( BATTLE_CHAMPIONSHIP_DATA *csData , const HEAPID heapId ); 
