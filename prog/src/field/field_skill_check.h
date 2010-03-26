//======================================================================
/**
 * @file	field_skill_check.h
 * @brief	フィールド技の状態チェック(技忘れよう
 * @author	ariizumi
 * @data	10/03/25
 *
 * モジュール名：FIELD_SKILL_CHECK
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
  FSCR_NO_MACHINE,  //技マシン無い
  FSCR_USE_SKILL,   //今使ってる
}FIELD_SKILL_CHECK_RET;

extern const u16 FIELD_SKILL_CHECK_GetUseSkillBit( GAMEDATA *gameData );
extern const FIELD_SKILL_CHECK_RET FIELD_SKILL_CHECK_CheckForgetSkill( GAMEDATA *gameData , const u16 wazaNo , HEAPID heapId );
