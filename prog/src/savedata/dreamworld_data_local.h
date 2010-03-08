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
  GFDATE gsyncTime;  //�ŏI�A�N�Z�X����
  u32 uploadCount;   //�a������  �l������B���������_�E�����[�h������l�𔽉f���Ȃ�
  POKEMON_PARAM pp;   ///< ����|�P����
  u16 itemID[DREAM_WORLD_DATA_MAX_ITEMBOX];  ///�����A�����A�C�e��
  u8 itemNum[DREAM_WORLD_DATA_MAX_ITEMBOX];
	DREAM_WORLD_FURNITUREDATA furnitureID[DREAM_WORLD_DATA_MAX_FURNITURE];  //9	�z�M�Ƌ�
  u8 pokemoStatus;        //8	���M�����|�P����						1byte			���M�����|�P�����̏�Ԃ��󂯎��
  u8 pokemonIn;  ///pp�Ƀ|�P�����������Ă��邩�ǂ���
  u8 signin; //PDW�o�^�ς�
  u8 bAccount; //�A�J�E���g�擾�ς�
  u8 furnitureNo     :7;  //�I�񂾉Ƌ�̃C���f�b�N�X  �����ꍇ DREAM_WORLD_NOFANITURE
  u8 isSyncFurniture :1;  //�Ƌ��Sync���đ��������H(�Ƌ�Řb���Ə�����
  u8 musicalNo;      ///< web�őI�������ԍ�  �����ꍇ 0xff
  u8 cgearNo;        ///< web�őI�������ԍ�  �����ꍇ 0xff
  u8 zukanNo;        ///< web�őI�������ԍ�  �����ꍇ 0xff
};

