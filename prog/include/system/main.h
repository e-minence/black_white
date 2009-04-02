//===================================================================
/**
 * @file	main.h
 * @brief	アプリケーション　サンプルスケルトン
 * @author	GAME FREAK Inc.
 * @date	06.11.28
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================

//親ヒープ用宣言
enum{
	GFL_HEAPID_APP = GFL_HEAPID_SYSTEM + 1,
	GFL_HEAPID_MAX
};

//子ヒープ用宣言
enum{
	HEAPID_BG=GFL_HEAPID_MAX,

	HEAPID_TITLE,
	HEAPID_STARTMENU,

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
    HEAPID_NETWORK_FIX,
    HEAPID_NETWORK,
    HEAPID_WIFI,
    HEAPID_IRC,
    HEAPID_LAYOUT,
    HEAPID_BTL_SYSTEM,
    HEAPID_BTL_VIEW,
    HEAPID_BTL_NET,
	HEAPID_FIELDMAP,
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
    HEAPID_WIFIP2PMATCH,
    HEAPID_WIFIP2PMATCHEX,
    HEAPID_WIFI_FOURMATCHVCHAT,
    HEAPID_WIFI_FOURMATCH,
	HEAPID_PALACE,
	HEAPID_TRCARD_SYS,
	HEAPID_TR_CARD,
	HEAPID_OEKAKI,
	HEAPID_PMS_INPUT_SYSTEM ,
	HEAPID_PMS_INPUT_VIEW, 
	HEAPID_MUSICAL_DRESSUP, 
	HEAPID_MUSICAL_STAGE, 
	HEAPID_WFLOBBY,
	HEAPID_WFLBY_ROOM,
	HEAPID_CODEIN,
	HEAPID_WFLBY_ROOMGRA,
	HEAPID_WIFINOTE,
	
	HEAPID_CHILD_MAX,	//終端
};

#define	HEAPSIZE_SYSTEM	(0x008000)
#define	HEAPSIZE_APP	(0x180000)   // PL,GSは約0x13A000

