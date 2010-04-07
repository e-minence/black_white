//======================================================================
/**
 * @file	field_skill_check.h
 * @brief	�t�B�[���h�Z�̏�ԃ`�F�b�N(�Z�Y��悤
 * @author	ariizumi
 * @data	10/03/25
 *
 * ���W���[�����FFIELD_SKILL_CHECK
 */
//======================================================================
#pragma once
#include "gamesystem/game_data.h"

typedef enum
{
  FSCB_NAMINORI = 1<<0,
  FSCB_DIVING   = 1<<1,
  FSCB_FLASH    = 1<<2,
  FSCB_KAIRIKI  = 1<<3,
}FIELD_SKILL_CHECK_BIT;

typedef enum
{
  FSCR_OK,
  FSCR_NO_MACHINE,  //�Z�}�V������
  FSCR_USE_SKILL,   //���g���Ă�
}FIELD_SKILL_CHECK_RET;

extern const u16 FIELD_SKILL_CHECK_GetUseSkillBit( GAMEDATA *gameData );
//�Z�}�V���̏����Ǝg�p�󋵂Ń`�F�b�N
extern const FIELD_SKILL_CHECK_RET FIELD_SKILL_CHECK_CheckForgetSkill( GAMEDATA *gameData , const u16 wazaNo , HEAPID heapId );
//�Z�������Ă邩�����`�F�b�N
extern const BOOL FIELD_SKILL_CHECK_CanTradePoke( POKEMON_PASO_PARAM *ppp , HEAPID heapId );
