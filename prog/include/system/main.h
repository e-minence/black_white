//===================================================================
/**
 * @file  main.h
 * @brief アプリケーション　サンプルスケルトン
 * @author  GAME FREAK Inc.
 * @date  06.11.28
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================

#pragma once

//親ヒープ用宣言
enum{
  GFL_HEAPID_APP = GFL_HEAPID_SYSTEM + 1,
  GFL_HEAPID_MAX
};

//子ヒープ用宣言
enum{
  HEAPID_BG=GFL_HEAPID_MAX,   // 2

  //常駐ヒープ
  HEAPID_WORLD,
  HEAPID_PROC,
  HEAPID_APP_CONTROL,
  HEAPID_COMMUNICATION,

  //アプリケーションヒープ
  HEAPID_TITLE,
  HEAPID_STARTMENU,
  HEAPID_NETWORK_FIX,
  HEAPID_NETWORK,
  HEAPID_ITEMMENU,              // 10
  HEAPID_WIFI,
  HEAPID_IRC,
  HEAPID_LAYOUT,
  HEAPID_BTL_SYSTEM,
  HEAPID_BTL_VIEW,
  HEAPID_BTL_NET,
  HEAPID_FIELDMAP,
  HEAPID_TITLE_DEMO,
  HEAPID_POKELIST,
  HEAPID_TRADE,                 // 20
  HEAPID_BALLOON,
  HEAPID_COMPATI,
  HEAPID_COMPATI_CONTROL,
  HEAPID_CONFIG,
  HEAPID_MYSTERYGIFT,
  HEAPID_NAME_INPUT,
  HEAPID_IRCBATTLE,
  HEAPID_WIFICLUB,
  HEAPID_WIFIP2PMATCH,
  HEAPID_WIFIP2PMATCHEX,        // 30
  HEAPID_WIFI_FOURMATCHVCHAT,
  HEAPID_WIFI_FOURMATCH,
  HEAPID_PALACE,
  HEAPID_TRCARD_SYS,
  HEAPID_TR_CARD,
  HEAPID_OEKAKI,
  HEAPID_PMS_INPUT_SYSTEM ,
  HEAPID_PMS_INPUT_VIEW,
  HEAPID_MUSICAL_PROC,
  HEAPID_MUSICAL_STRM,          // 40
  HEAPID_MUSICAL_DRESSUP,
  HEAPID_MUSICAL_STAGE,
  HEAPID_MUSICAL_LOBBY,
  HEAPID_WFLOBBY,
  HEAPID_WFLBY_ROOM,
  HEAPID_CODEIN,
  HEAPID_WFLBY_ROOMGRA,
  HEAPID_WIFINOTE,
  HEAPID_CGEAR,
  HEAPID_DEBUGWIN,              // 50
  HEAPID_IRCAURA,
  HEAPID_IRCRHYTHM,
  HEAPID_SEL_MODE,
  HEAPID_IRCCOMPATIBLE_SYSTEM,
  HEAPID_IRCCOMPATIBLE,
  HEAPID_IRCRESULT,
  HEAPID_NEWSDRAW,
  HEAPID_WLDTIMER,
  HEAPID_DEBUG_MAKEPOKE,
  HEAPID_BUCKET,                // 60
  HEAPID_UNION,
  HEAPID_POKE_STATUS,
  HEAPID_IRCRANKING,
  HEAPID_TOWNMAP,
  HEAPID_BALANCE_BALL,
  HEAPID_FOOTPRINT,
  HEAPID_ANKETO,
  HEAPID_WORLDTRADE,
  HEAPID_MICTEST,
  HEAPID_MUSICAL_SHOT,          // 70

  //デバッグ用ヒープ
  HEAPID_WATANABE_DEBUG,
  HEAPID_SOGABE_DEBUG,
  HEAPID_TAMADA_DEBUG,
  HEAPID_OHNO_DEBUG,
  HEAPID_TAYA_DEBUG,
  HEAPID_TAYA_DEBUG_SUB,
  HEAPID_NAKAHIRO_DEBUG,
  HEAPID_TOMOYA_DEBUG,
  HEAPID_MATSUDA_DEBUG,
  HEAPID_GOTO_DEBUG,
  HEAPID_ARIIZUMI_DEBUG,
  HEAPID_NAGI_DEBUG,
  HEAPID_NAGI_DEBUG_SUB,
  HEAPID_OBATA_DEBUG,

  HEAPID_CHILD_MAX, //終端
};

#define HEAPSIZE_SYSTEM (0x008000)
#define HEAPSIZE_APP  (0x188000)   // PL,GSは約0x13A000

