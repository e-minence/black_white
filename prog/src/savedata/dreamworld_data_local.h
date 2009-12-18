//============================================================================================
/**
 * @file	  dreamworld_data_local.h
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "poke_tool/poke_tool.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "savedata/dreamworld_data.h"
#include "poke_tool/poke_tool_def.h"


//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _DREAMWORLD_SAVEDATA {
  POKEMON_PARAM pp;   ///< ����|�P����
  u8 recv_flag[DREAM_WORLD_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  u16 categoryType;    //�J�e�S��ID						2byte			���̌��ʃf�[�^�̃J�e�S���i�C�x���g�Ȃ̂��A�A�C�e���Ȃ̂��A�G���J�E���g�Ȃ̂��j
  DREAM_WORLD_TREAT_DATA treat;  //���J���f�[�^ 
	DREAM_WORLD_FURNITUREDATA furnitureID[DREAM_WORLD_DATA_MAX_FURNITURE];  //9	�z�M�Ƌ�
  u8 pokemoStatus;        //8	���M�����|�P����						1byte			���M�����|�P�����̏�Ԃ��󂯎��
  u8 pokemonIn;  ///pp�Ƀ|�P�����������Ă��邩�ǂ���
  u8 signin; //�A�J�E���g�����łɓo�^�������ǂ���
};

