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
	switch( item )
	{	
	case ITEM_ZITENSYA:
		return SHORTCUT_ID_ZITENSYA;

	case ITEM_BORONOTURIZAO:
		return SHORTCUT_ID_BORONOTURIZAO;

	case ITEM_IITURIZAO:
		return SHORTCUT_ID_IITURIZAO;

	case ITEM_SUGOITURIZAO:
		return SHORTCUT_ID_SUGOITURIZAO;

	case ITEM_TAUNMAPPU:
		return SHORTCUT_ID_TOWNMAP;

	default:
		return SHORTCUT_ID_NULL;
	}
}
