//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut_data.c
 *	@brief	Y�{�^���o�^
 *	@author	Toru=Nagihashi
 *	@data		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "system/shortcut_data.h"
#include "item/itemsym.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�e���ԍ�����V���[�g�J�b�gID�ւ̕ϊ�
 *
 *	@param	u8 item		�A�C�e��
 *
 *	@return	SHORTCUT_ID	�V���[�g�J�b�gID
 */
//-----------------------------------------------------------------------------
SHORTCUT_ID SHORTCUT_DATA_GetItemToShortcutID( u8 item )
{	
	
#ifdef DEBUG_ONLY_FOR_toru_nagihashi
#warning ITEM_ID add or changed//�A�C�e��ID���ύX������ǉ����ꂽ�肷��͂�
#endif

	switch( item )
	{	
	case ITEM_ZITENSYA:
		return SHORTCUT_ID_ZITENSYA;
#if 0
	case ITEM_BORONOTURIZAO:
		return SHORTCUT_ID_BORONOTURIZAO;

	case ITEM_IITURIZAO:
		return SHORTCUT_ID_IITURIZAO;

	case ITEM_SUGOITURIZAO:
		return SHORTCUT_ID_SUGOITURIZAO;
#else
	case ITEM_BORONOTURIZAO:
		/* fallthrough */
	case ITEM_IITURIZAO:
		/* fallthrough */
	case ITEM_SUGOITURIZAO:
		return SHORTCUT_ID_TURIZAO;
#endif
	case ITEM_TAUNMAPPU:
		return SHORTCUT_ID_TOWNMAP;
	
	case ITEM_BATORUREKOODAA:
		return SHORTCUT_ID_BTLRECORDER;

	case ITEM_TOMODATITETYOU:
		return SHORTCUT_ID_FRIENDNOTE;

	case ITEM_PARESUHEGOO:
		return SHORTCUT_ID_PALACEGO;

	default:
		return SHORTCUT_ID_NULL;
	}
}
