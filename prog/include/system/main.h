//===================================================================
/**
 * @file	main.h
 * @brief	�A�v���P�[�V�����@�T���v���X�P���g��
 * @author	GAME FREAK Inc.
 * @date	06.11.28
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================

//�e�q�[�v�p�錾
enum{
	GFL_HEAPID_APP = GFL_HEAPID_SYSTEM + 1,
	GFL_HEAPID_MAX
};

//�q�q�[�v�p�錾
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
	HEAPID_CONFIG,
	HEAPID_MYSTERYGIFT,
	HEAPID_NAME_INPUT,
	HEAPID_WIFICLUB,
	HEAPID_PALACE,
	HEAPID_CHILD_MAX,
};

#define	HEAPSIZE_SYSTEM	(0x008000)
#define	HEAPSIZE_APP	(0x170000)   // PL,GS�͖�0x13A000

