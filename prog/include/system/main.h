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
  HEAPID_NETWORK,              // 12
  HEAPID_ITEMMENU,
  HEAPID_WIFI,
  HEAPID_IRC,
  HEAPID_LAYOUT,
  HEAPID_BTL_SYSTEM,
  HEAPID_BTL_VIEW,
  HEAPID_BTL_NET,
  HEAPID_FIELDMAP,              // 20
  HEAPID_TITLE_DEMO,
  HEAPID_POKELIST,
  HEAPID_TRADE,
  HEAPID_BALLOON,
  HEAPID_COMPATI,
  HEAPID_COMPATI_CONTROL,
  HEAPID_CONFIG,
  HEAPID_MYSTERYGIFT,
  HEAPID_NAME_INPUT,
  HEAPID_IRCBATTLE,
  HEAPID_WIFICLUB,
  HEAPID_WIFIP2PMATCH,        // 32
  HEAPID_WIFIP2PMATCHEX,
  HEAPID_WIFI_FOURMATCHVCHAT,
  HEAPID_WIFI_FOURMATCH,
  HEAPID_PALACE,
  HEAPID_TRCARD_SYS,
  HEAPID_TR_CARD,
  HEAPID_OEKAKI,
  HEAPID_PMS_INPUT_SYSTEM ,
  HEAPID_PMS_INPUT_VIEW,
  HEAPID_MUSICAL_PROC,          // 42
  HEAPID_MUSICAL_STRM,
  HEAPID_MUSICAL_DRESSUP,
  HEAPID_MUSICAL_STAGE,
  HEAPID_MUSICAL_LOBBY,
  HEAPID_WFLOBBY,
  HEAPID_WFLBY_ROOM,
  HEAPID_CODEIN,
  HEAPID_WFLBY_ROOMGRA,
  HEAPID_WIFINOTE,
  HEAPID_CGEAR,              // 52
  HEAPID_DEBUGWIN,
  HEAPID_IRCAURA,
  HEAPID_IRCRHYTHM,
  HEAPID_SEL_MODE,
  HEAPID_IRCCOMPATIBLE_SYSTEM,
  HEAPID_IRCCOMPATIBLE,
  HEAPID_IRCRESULT,
  HEAPID_NEWSDRAW,
  HEAPID_WLDTIMER,
  HEAPID_DEBUG_MAKEPOKE,               // 62
  HEAPID_BUCKET,
  HEAPID_UNION,
  HEAPID_POKE_STATUS,
  HEAPID_IRCRANKING,
  HEAPID_TOWNMAP,
  HEAPID_BALANCE_BALL,
  HEAPID_FOOTPRINT,
  HEAPID_ANKETO,
  HEAPID_WORLDTRADE,
  HEAPID_MICTEST,          // 72
  HEAPID_MUSICAL_SHOT,
  HEAPID_BOX_SYS,
  HEAPID_BOX_APP,
  HEAPID_IRC_BATTLE,
  HEAPID_BATTLE_CHAMPIONSHIP,
  HEAPID_WAZAOSHIE,
  HEAPID_FLD3DCUTIN,
  HEAPID_PMS_SELECT,      // 80
  HEAPID_BTLRET_SYS,
  HEAPID_BTLRET_VIEW,
  HEAPID_GAMEOVER_MSG,
  HEAPID_LOCAL_TVT,
  HEAPID_WIFIBATTLEMATCH_SYS,
  HEAPID_WIFIBATTLEMATCH_CORE,
  HEAPID_WIFIBATTLEMATCH_SUB,
  HEAPID_BATTLE_RECORDER_SYS,
  HEAPID_BATTLE_RECORDER_CORE,
  HEAPID_GDS_MAIN,
  HEAPID_GDS_CONNECT,     //91
  HEAPID_MULTIBOOT,
  HEAPID_MULTIBOOT_DATA,      
  HEAPID_MB_BOX,      
  HEAPID_MB_CAPTURE,      
  HEAPID_MONOLITH,
  HEAPID_GAMESYNC,
  HEAPID_MAILSYS,         
  HEAPID_MAILVIEW,         
  HEAPID_MAILBOX_SYS,
  HEAPID_MAILBOX_APP,     //101
  HEAPID_PDWACC,
  HEAPID_SHINKA_DEMO,
  HEAPID_ZUKAN_SYS,
  HEAPID_ZUKAN_TOROKU,
  HEAPID_ZUKAN_LIST,
  HEAPID_DEMO3D,
  HEAPID_COMM_TVT,
  HEAPID_CTVT_CAMERA,   
  HEAPID_INTRO,						//110
  HEAPID_FIELD_SUBSCREEN,
	HEAPID_VS_MULTI_LIST,

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
  HEAPID_HOSAKA_DEBUG,
  HEAPID_WIFIMATCH_DEBUG,

  HEAPID_CHILD_MAX, //終端
};

#define HEAPSIZE_SYSTEM (0x008000)
#define HEAPSIZE_APP  (0x186000)   // PL,GSは約0x13A000
#define HEAPSIZE_DSI  (0x486000)   // DSIは16M

//将来的にTWLで増えたメモリは別HEAPIDから撮るための定義
#define GFL_HEAPID_TWL (GFL_HEAPID_APP)

