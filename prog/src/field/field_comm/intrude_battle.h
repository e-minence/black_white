//==============================================================================
/**
 * @file    intrude_battle.h
 * @brief   侵入：通信対戦ヘッダ
 * @author  matsuda
 * @date    2009.11.16(月)
 */
//==============================================================================
#pragma once

//==============================================================================
//  構造体定義
//==============================================================================
typedef struct{
  GAMESYS_WORK *gsys;
  u8 target_netid;
  u8 flat_level;        ///<フラットレベル(0の場合はフリー)
  u8 max_poke_num;      ///<手持ちポケモン最大数(先頭からこの数分までしか出場させない)
  u8 padding;
}INTRUDE_BATTLE_PARENT;


//==============================================================================
//  外部関数宣言
//==============================================================================
extern GMEVENT* EVENT_IntrudeBattle_CallBattle( GAMESYS_WORK* gsys, INTRUDE_BATTLE_PARENT *ibp );
