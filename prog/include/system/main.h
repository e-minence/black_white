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
  HEAPID_SAVE,
  HEAPID_NET_ERR,

  //アプリケーションヒープ
  HEAPID_TITLE,
  HEAPID_STARTMENU,
  HEAPID_NETWORK_FIX,
  HEAPID_NETWORK,              // 10
  HEAPID_ITEMMENU,
  HEAPID_WIFI,
  HEAPID_IRC,
  HEAPID_LAYOUT,
  HEAPID_BTL_SYSTEM,
  HEAPID_BTL_VIEW,
  HEAPID_BTL_NET,
  HEAPID_FIELDMAP,
  HEAPID_TITLE_DEMO,
  HEAPID_POKELIST,                 // 20
  HEAPID_TRADE,
  HEAPID_BALLOON,
  HEAPID_COMPATI,
  HEAPID_COMPATI_CONTROL,
  HEAPID_CONFIG,
  HEAPID_MYSTERYGIFT,
  HEAPID_NAME_INPUT,
  HEAPID_IRCBATTLE,
  HEAPID_WIFICLUB,
  HEAPID_WIFIP2PMATCH,        // 30
  HEAPID_WIFIP2PMATCHEX,
  HEAPID_WIFI_FOURMATCHVCHAT,
  HEAPID_WIFI_FOURMATCH,
  HEAPID_PALACE,
  HEAPID_TRCARD_SYS,
  HEAPID_TR_CARD,
  HEAPID_OEKAKI,
  HEAPID_PMS_INPUT_SYSTEM ,
  HEAPID_PMS_INPUT_VIEW,
  HEAPID_MUSICAL_PROC,          // 40
  HEAPID_MUSICAL_STRM,
  HEAPID_MUSICAL_DRESSUP,
  HEAPID_MUSICAL_STAGE,
  HEAPID_MUSICAL_LOBBY,
  HEAPID_WFLOBBY,
  HEAPID_WFLBY_ROOM,
  HEAPID_CODEIN,
  HEAPID_WFLBY_ROOMGRA,
  HEAPID_WIFINOTE,
  HEAPID_CGEAR,              // 50
  HEAPID_DEBUGWIN,
  HEAPID_IRCAURA,
  HEAPID_IRCRHYTHM,
  HEAPID_SEL_MODE,
  HEAPID_IRCCOMPATIBLE_SYSTEM,
  HEAPID_IRCCOMPATIBLE,
  HEAPID_IRCRESULT,
  HEAPID_NEWSDRAW,
  HEAPID_WLDTIMER,
  HEAPID_DEBUG_MAKEPOKE,               // 60
  HEAPID_BUCKET,
  HEAPID_UNION,
  HEAPID_POKE_STATUS,
  HEAPID_IRCRANKING,
  HEAPID_TOWNMAP,
  HEAPID_BALANCE_BALL,
  HEAPID_FOOTPRINT,
  HEAPID_ANKETO,
  HEAPID_WORLDTRADE,
  HEAPID_MICTEST,          // 70
  HEAPID_MUSICAL_SHOT,
  HEAPID_BOX_SYS,
  HEAPID_BOX_APP,
  HEAPID_IRC_BATTLE,
  HEAPID_BATTLE_CHAMPIONSHIP,
  HEAPID_WAZAOSHIE,
  HEAPID_FLD3DCUTIN,

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
  HEAPID_UI_DEBUG,
  HEAPID_BTL_DEBUG_SYS,
  HEAPID_BTL_DEBUG_VIEW,

  HEAPID_CHILD_MAX, //終端
};

#define HEAPSIZE_SYSTEM (0x008000)
#define HEAPSIZE_APP  (0x188000)   // PL,GSは約0x13A000

