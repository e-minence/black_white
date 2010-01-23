//=============================================================================
/**
 * @file	  pokemontrade.h
 * @bfief	  ポケモン交換
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/07/09
 */
//=============================================================================


#pragma once

#include "poke_tool/poke_tool.h"
#include "gamesystem/game_data.h"

/// ポケモン交換のタイプ
typedef enum{
  POKEMONTRADE_IRC,   ///< 赤外線
  POKEMONTRADE_UNION, ///< ユニオン
  POKEMONTRADE_WIFICLUB, ///< WIFIクラブ
  POKEMONTRADE_GTSNEGO, ///< GTSネゴシエーション
  POKEMONTRADE_GTS, ///< GTS
  POKEMONTRADE_EVENT,  ///< イベント用
  POKEMONTRADE_EVOLUTION_RET,  ///< 進化からのもどり
  POKEMONTRADE_TYPE_MAX,
} POKEMONTRADE_TYPE;


/// ポケモン交換の戻り方
typedef enum{
  POKEMONTRADE_EVOLUTION,   ///< 進化
  POKEMONTRADE_END,   ///< 終了
  POKEMONTRADE_RETURN_MAX,
} POKEMONTRADE_RETURN;

typedef struct _POKEMON_TRADE_WORK POKEMON_TRADE_WORK;
typedef struct _POKEMONTRADE_DEMO_WORK POKEMONTRADE_DEMO_WORK;



///ポケモン交換呼び出し用パラメータ
typedef struct{
  POKEMONTRADE_TYPE type;
  POKEMONTRADE_RETURN ret;
  int after_mons_no;
  int cond;
  GAMEDATA* gamedata;
  void* pNego;
} POKEMONTRADE_PARAM;


///ポケモンデモ呼び出し用パラメータ
typedef struct{
  POKEMON_PARAM* pMyPoke;      ///< ユーザーのポケモン
  POKEMON_PARAM* pNPCPoke;  ///< NPCのポケモン
  MYSTATUS* pMy;    ///<自分のMYSTATUS
  MYSTATUS* pNPC;   ///<NPCのMYSTATUS
  GAMEDATA* gamedata;
} POKEMONTRADE_DEMO_PARAM;


extern const GFL_PROC_DATA PokemonTradeProcData;
extern const GFL_PROC_DATA PokemonTradeIrcProcData;
extern const GFL_PROC_DATA PokemonTradeWiFiProcData;
extern const GFL_PROC_DATA PokemonTradeDemoProcData;


