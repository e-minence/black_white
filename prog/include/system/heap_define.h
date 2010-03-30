//===================================================================
/**
 * @file heap_define.h
 * @brief ヒープID定義
 * @date  2010.02.08
 *
 * すべてHEAP_DEFINE( ヒープ指定名 ）という形式で記述してください。
 * HEAP_DEFINEマクロの展開方法により、enumのヒープ定義とデバッグ用の
 * ヒープ定義名テーブルに変換しています。
 */
//===================================================================

  //常駐ヒープ
  HEAP_DEFINE( HEAPID_WORLD )
  HEAP_DEFINE( HEAPID_PROC )
  HEAP_DEFINE( HEAPID_APP_CONTROL )
  HEAP_DEFINE( HEAPID_COMMUNICATION )
  HEAP_DEFINE( HEAPID_SAVE )
  HEAP_DEFINE( HEAPID_NET_ERR )

  //アプリケーションヒープ
  HEAP_DEFINE( HEAPID_TITLE )
  HEAP_DEFINE( HEAPID_STARTMENU )
  HEAP_DEFINE( HEAPID_NETWORK_FIX )
  HEAP_DEFINE( HEAPID_NETWORK )              // 12
  HEAP_DEFINE( HEAPID_ITEMMENU )
  HEAP_DEFINE( HEAPID_WIFI )
  HEAP_DEFINE( HEAPID_IRC )
  HEAP_DEFINE( HEAPID_LAYOUT )
  HEAP_DEFINE( HEAPID_BTL_SYSTEM )
  HEAP_DEFINE( HEAPID_BTL_VIEW )
  HEAP_DEFINE( HEAPID_BTL_NET )
  HEAP_DEFINE( HEAPID_FIELDMAP )              // 20
  HEAP_DEFINE( HEAPID_TITLE_DEMO )
  HEAP_DEFINE( HEAPID_POKELIST )
  HEAP_DEFINE( HEAPID_TRADE )
  HEAP_DEFINE( HEAPID_BALLOON )
  HEAP_DEFINE( HEAPID_COMPATI )
  HEAP_DEFINE( HEAPID_COMPATI_CONTROL )
  HEAP_DEFINE( HEAPID_CONFIG )
  HEAP_DEFINE( HEAPID_MYSTERYGIFT )
  HEAP_DEFINE( HEAPID_NAME_INPUT )
  HEAP_DEFINE( HEAPID_IRCBATTLE )
  HEAP_DEFINE( HEAPID_WIFICLUB )
  HEAP_DEFINE( HEAPID_WIFIP2PMATCH )        // 32
  HEAP_DEFINE( HEAPID_WIFIP2PMATCHEX )
  HEAP_DEFINE( HEAPID_WIFI_FOURMATCHVCHAT )
  HEAP_DEFINE( HEAPID_WIFI_FOURMATCH )
  HEAP_DEFINE( HEAPID_PALACE )
  HEAP_DEFINE( HEAPID_TRCARD_SYS )
  HEAP_DEFINE( HEAPID_TR_CARD )
  HEAP_DEFINE( HEAPID_OEKAKI )
  HEAP_DEFINE( HEAPID_PMS_INPUT_SYSTEM  )
  HEAP_DEFINE( HEAPID_PMS_INPUT_VIEW )
  HEAP_DEFINE( HEAPID_MUSICAL_PROC )          // 42
  HEAP_DEFINE( HEAPID_MUSICAL_STRM )
  HEAP_DEFINE( HEAPID_MUSICAL_DRESSUP )
  HEAP_DEFINE( HEAPID_MUSICAL_STAGE )
  HEAP_DEFINE( HEAPID_MUSICAL_LOBBY )
  HEAP_DEFINE( HEAPID_WFLOBBY )
  HEAP_DEFINE( HEAPID_WFLBY_ROOM )
  HEAP_DEFINE( HEAPID_CODEIN )
  HEAP_DEFINE( HEAPID_WFLBY_ROOMGRA )
  HEAP_DEFINE( HEAPID_WIFINOTE )
  HEAP_DEFINE( HEAPID_CGEAR )              // 52
  HEAP_DEFINE( HEAPID_DEBUGWIN )
  HEAP_DEFINE( HEAPID_IRCAURA )
  HEAP_DEFINE( HEAPID_IRCRHYTHM )
  HEAP_DEFINE( HEAPID_SEL_MODE )
  HEAP_DEFINE( HEAPID_IRCCOMPATIBLE_SYSTEM )
  HEAP_DEFINE( HEAPID_IRCCOMPATIBLE )
  HEAP_DEFINE( HEAPID_IRCRESULT )
  HEAP_DEFINE( HEAPID_NEWSDRAW )
  HEAP_DEFINE( HEAPID_WLDTIMER )
  HEAP_DEFINE( HEAPID_DEBUG_MAKEPOKE )               // 62
  HEAP_DEFINE( HEAPID_BUCKET )
  HEAP_DEFINE( HEAPID_UNION )
  HEAP_DEFINE( HEAPID_POKE_STATUS )
  HEAP_DEFINE( HEAPID_IRCRANKING )
  HEAP_DEFINE( HEAPID_TOWNMAP )
  HEAP_DEFINE( HEAPID_BALANCE_BALL )
  HEAP_DEFINE( HEAPID_FOOTPRINT )
  HEAP_DEFINE( HEAPID_ANKETO )
  HEAP_DEFINE( HEAPID_WORLDTRADE )
  HEAP_DEFINE( HEAPID_MICTEST )          // 72
  HEAP_DEFINE( HEAPID_MUSICAL_SHOT )
  HEAP_DEFINE( HEAPID_BOX_SYS )
  HEAP_DEFINE( HEAPID_BOX_APP )
  HEAP_DEFINE( HEAPID_IRC_BATTLE )
  HEAP_DEFINE( HEAPID_BATTLE_CHAMPIONSHIP )
  HEAP_DEFINE( HEAPID_WAZAOSHIE )
  HEAP_DEFINE( HEAPID_FLD3DCUTIN )
  HEAP_DEFINE( HEAPID_PMS_SELECT )      // 80
  HEAP_DEFINE( HEAPID_BTLRET_SYS )
  HEAP_DEFINE( HEAPID_BTLRET_VIEW )
  HEAP_DEFINE( HEAPID_GAMEOVER_MSG )
  HEAP_DEFINE( HEAPID_LOCAL_TVT )
  HEAP_DEFINE( HEAPID_WIFIBATTLEMATCH_SYS )
  HEAP_DEFINE( HEAPID_WIFIBATTLEMATCH_CORE )
  HEAP_DEFINE( HEAPID_WIFIBATTLEMATCH_SUB )
  HEAP_DEFINE( HEAPID_BATTLE_RECORDER_SYS )
  HEAP_DEFINE( HEAPID_BATTLE_RECORDER_CORE )
  HEAP_DEFINE( HEAPID_GDS_MAIN )
  HEAP_DEFINE( HEAPID_GDS_CONNECT )     //91
  HEAP_DEFINE( HEAPID_MULTIBOOT )
  HEAP_DEFINE( HEAPID_MULTIBOOT_DATA )      
  HEAP_DEFINE( HEAPID_MB_BOX )      
  HEAP_DEFINE( HEAPID_MB_CAPTURE )      
  HEAP_DEFINE( HEAPID_MONOLITH )
  HEAP_DEFINE( HEAPID_GAMESYNC )
  HEAP_DEFINE( HEAPID_MAILSYS )         
  HEAP_DEFINE( HEAPID_MAILVIEW )         
  HEAP_DEFINE( HEAPID_MAILBOX_SYS )
  HEAP_DEFINE( HEAPID_MAILBOX_APP )     //101
  HEAP_DEFINE( HEAPID_PDWACC )
  HEAP_DEFINE( HEAPID_SHINKA_DEMO )
  HEAP_DEFINE( HEAPID_ZUKAN_SYS )
  HEAP_DEFINE( HEAPID_ZUKAN_TOROKU )
  HEAP_DEFINE( HEAPID_ZUKAN_LIST )
  HEAP_DEFINE( HEAPID_DEMO3D )
  HEAP_DEFINE( HEAPID_COMM_TVT )
  HEAP_DEFINE( HEAPID_CTVT_CAMERA )   
  HEAP_DEFINE( HEAPID_INTRO )           //110
  HEAP_DEFINE( HEAPID_FIELD_SUBSCREEN )
  HEAP_DEFINE( HEAPID_VS_MULTI_LIST )
  HEAP_DEFINE( HEAPID_GURU2 )
  HEAP_DEFINE( HEAPID_DIGITALCARD_CHECK )
  HEAP_DEFINE( HEAPID_COMM_BTL_DEMO )
  HEAP_DEFINE( HEAPID_BTL_REC_SEL )
  HEAP_DEFINE( HEAPID_BATTLE_CALL )
  HEAP_DEFINE( HEAPID_BEACON_DETAIL )
  HEAP_DEFINE( HEAPID_WIFILOGIN )
  HEAP_DEFINE( HEAPID_WIFILOGOUT )      //120
  HEAP_DEFINE( HEAPID_UN_SELECT )
  HEAP_DEFINE( HEAPID_ZUKAN_SEARCH )
  HEAP_DEFINE( HEAPID_WIFIEARTH )
  HEAP_DEFINE( HEAPID_WIFI_BSUBWAY )
  HEAP_DEFINE( HEAPID_ZUKAN_TOP )
  HEAP_DEFINE( HEAPID_COMMAND_DEMO )
  HEAP_DEFINE( HEAPID_LEADERBOARD )
  HEAP_DEFINE( HEAPID_BACKUP_ERASE )
  HEAP_DEFINE( HEAPID_BATTLE_CHAMPIONSHIP_SYS )
  HEAP_DEFINE( HEAPID_AWARD )           //130
  HEAP_DEFINE( HEAPID_LIVEBATTLEMATCH_SUB )
  HEAP_DEFINE( HEAPID_POKEMON_DREAM_CATCH )
  HEAP_DEFINE( HEAPID_WIFIBATTLEMATCH_SC )
  HEAP_DEFINE( HEAPID_PSEL )
  HEAP_DEFINE( HEAPID_SUBWAY_MAP )
  HEAP_DEFINE( HEAPID_FIELD_PRBUF )
  HEAP_DEFINE( HEAPID_CG_HELP )
  HEAP_DEFINE( HEAPID_PDCRET_SYS )
  HEAP_DEFINE( HEAPID_DENDOU_PC )

  //デバッグ用ヒープ
  HEAP_DEFINE( HEAPID_WATANABE_DEBUG )
  HEAP_DEFINE( HEAPID_SOGABE_DEBUG )
  HEAP_DEFINE( HEAPID_TAMADA_DEBUG )
  HEAP_DEFINE( HEAPID_OHNO_DEBUG )
  HEAP_DEFINE( HEAPID_TAYA_DEBUG )
  HEAP_DEFINE( HEAPID_TAYA_DEBUG_SUB )
  HEAP_DEFINE( HEAPID_NAKAHIRO_DEBUG )
  HEAP_DEFINE( HEAPID_TOMOYA_DEBUG )
  HEAP_DEFINE( HEAPID_MATSUDA_DEBUG )
  HEAP_DEFINE( HEAPID_GOTO_DEBUG )
  HEAP_DEFINE( HEAPID_ARIIZUMI_DEBUG )
  HEAP_DEFINE( HEAPID_NAGI_DEBUG )
  HEAP_DEFINE( HEAPID_NAGI_DEBUG_SUB )
  HEAP_DEFINE( HEAPID_OBATA_DEBUG )
  HEAP_DEFINE( HEAPID_UI_DEBUG )
  HEAP_DEFINE( HEAPID_BTL_DEBUG_SYS )
  HEAP_DEFINE( HEAPID_BTL_DEBUG_VIEW )
  HEAP_DEFINE( HEAPID_HOSAKA_DEBUG )
  HEAP_DEFINE( HEAPID_WIFIMATCH_DEBUG )
  HEAP_DEFINE( HEAPID_DEBUG_MAKE_UNDATA )
  HEAP_DEFINE( HEAPID_KAWADA_DEBUG )


//マクロを無効化する
#undef  HEAP_DEFINE

