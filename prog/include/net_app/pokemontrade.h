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
#include "demo/shinka_demo.h"
#include "app/mailbox.h"
#include "net_app/gtsnego.h"

/// ポケモン交換のタイプ
typedef enum{
  POKEMONTRADE_TYPE_IRC,   ///< 赤外線
  POKEMONTRADE_TYPE_UNION, ///< ユニオン
  POKEMONTRADE_TYPE_WIFICLUB, ///< WIFIクラブ
  POKEMONTRADE_TYPE_GTSNEGO, ///< GTSネゴシエーション
  POKEMONTRADE_TYPE_VISUAL,
  POKEMONTRADE_TYPE_EVENT = POKEMONTRADE_TYPE_VISUAL, ///< イベント
  POKEMONTRADE_TYPE_GTS, ///< GTS
  POKEMONTRADE_TYPE_GTSUP,   ///< GTSあずける
  POKEMONTRADE_TYPE_GTSDOWN,  ///< GTSひきとる
  POKEMONTRADE_TYPE_GTSMID,  ///< GTS真ん中
  POKEMONTRADE_TYPE_GTSNEGODEMO,
  POKEMONTRADE_TYPE_MAX,
} POKEMONTRADE_TYPE;


/// ポケモン交換の移動方向
typedef enum{
  POKEMONTRADE_MOVE_START,  ///< 開始
  POKEMONTRADE_MOVE_EVOLUTION,   ///< 進化
  POKEMONTRADE_MOVE_ERROR,   ///< エラー
  POKEMONTRADE_MOVE_END,   ///< 終了
  POKEMONTRADE_MOVE_MAX,
} POKEMONTRADE_MOVE;

typedef struct _POKEMON_TRADE_WORK POKEMON_TRADE_WORK;
typedef struct _POKEMONTRADE_DEMO_WORK POKEMONTRADE_DEMO_WORK;



///ポケモン交換呼び出し用パラメータ
typedef struct{
  MAILBOX_PARAM aMailBox;
  POKEMONTRADE_MOVE ret;
  int after_mons_no;
  int cond;
  GAMEDATA* gamedata;
  EVENT_GTSNEGO_WORK* pNego;
  POKEPARTY* pParty;
  SHINKA_DEMO_PARAM* shinka_param;
  DWCSvlResult* pSvl;         //[in ]任天堂SVL認証構造体 DWCSvlResultは自前で確保してください
  u16 selectBoxno;
  u16 selectIndex;
  u16 friendNo;     //wifiクラブ交換時の相手のフレンドインデックス
  u16 bDebug;
} POKEMONTRADE_PARAM;


///ポケモンデモ呼び出し用パラメータ
typedef struct{
  POKEMONTRADE_PARAM aParam;   // ※ここは入れないでください
  GAMEDATA* gamedata;   ///< gamedata
  POKEMON_PARAM* pMyPoke;      ///< ユーザーのポケモン
  POKEMON_PARAM* pNPCPoke;  ///< NPCのポケモン
  MYSTATUS* pMy;    ///<自分のMYSTATUS
  MYSTATUS* pNPC;   ///<NPCのMYSTATUS
} POKEMONTRADE_DEMO_PARAM;


extern const GFL_PROC_DATA PokemonTradeProcData;
extern const GFL_PROC_DATA PokemonTradeIrcProcData;
extern const GFL_PROC_DATA PokemonTradeWiFiProcData;
extern const GFL_PROC_DATA PokemonTradeDemoProcData;
extern const GFL_PROC_DATA PokemonTradeClubProcData;
extern const GFL_PROC_DATA PokemonTradeGTSProcData;
extern const GFL_PROC_DATA PokemonTradeGTSSendProcData;
extern const GFL_PROC_DATA PokemonTradeGTSRecvProcData;
extern const GFL_PROC_DATA PokemonTradeGTSMidProcData;



