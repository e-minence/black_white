//======================================================================
/**
 * @file	battle_championship_def.h
 * @brief	バトル大会系定義
 * @author	ariizumi
 * @data	09/10/06
 *
 * モジュール名：BATTLE_CHAMPIONSHIP
 */
//======================================================================
#pragma once

#include "buflen.h"
#include "savedata/save_control.h"
#include "savedata/regulation.h"

#define CHAMPIONSHIP_NAME_LEN (36+2)  //36+EOM

typedef struct
{
  STRCODE name[CHAMPIONSHIP_NAME_LEN];
  u8      number; //大会管理番号
  u8      league; //○○リーグ(ジュニア・シニア)
  
  REGULATION regulation;
  
}BATTLE_CHAMPIONSHIP_DATA;

