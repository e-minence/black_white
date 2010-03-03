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
#include "savedata/shortcut.h"
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
 *	@brief	�J�[�\��������
 *
 *	@param	SHORTCUT_CURSOR *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
void SHORTCUT_CURSOR_Init( SHORTCUT_CURSOR *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(SHORTCUT_CURSOR) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�A�C�e���ԍ�����V���[�g�J�b�gID�ւ̕ϊ�
 *
 *	@param	u8 item		�A�C�e��
 *
 *	@return	SHORTCUT_ID	�V���[�g�J�b�gID
 */
//-----------------------------------------------------------------------------
SHORTCUT_ID SHORTCUT_DATA_GetItemToShortcutID( u16 item )
{	
	
#ifdef DEBUG_ONLY_FOR_toru_nagihashi
#warning ITEM_ID add or changed//�A�C�e��ID���ύX������ǉ����ꂽ�肷��͂�
#endif

	NAGI_Printf( "�A�C�e���ϊ� %d\n", item );

	switch( item )
	{	
	case ITEM_ZITENSYA:
		return SHORTCUT_ID_ZITENSYA;

	case ITEM_BORONOTURIZAO:
		/* fallthrough */
	case ITEM_IITURIZAO:
		/* fallthrough */
	case ITEM_SUGOITURIZAO:
		return SHORTCUT_ID_TURIZAO;

	case ITEM_TAUNMAPPU:
		return SHORTCUT_ID_TOWNMAP;
	
	case ITEM_BATORUREKOODAA:
		return SHORTCUT_ID_BTLRECORDER;

	case ITEM_TOMODATITETYOU:
		return SHORTCUT_ID_FRIENDNOTE;

	case ITEM_PARESUHEGOO:
		return SHORTCUT_ID_PALACEGO;

	case ITEM_DAUZINGUMASIN:
		return SHORTCUT_ID_DOWSINGMACHINE;

	default:
		return SHORTCUT_ID_NULL;
	}
}

