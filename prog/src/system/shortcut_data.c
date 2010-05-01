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
typedef struct _ITEM2SHORTCUT_TBL{
  u16 itemno:10;
  u16 shortcut_id:6;
}ITEM2SHORTCUT_TBL;

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
#if 1 //switch case���Ȃ����Čy�ʉ� by iwasawa 10.05.01
  int i;
  static const ITEM2SHORTCUT_TBL Item2ShortcutTbl[] = {
 	  { ITEM_ZITENSYA, SHORTCUT_ID_ZITENSYA },
    { ITEM_SUGOITURIZAO, SHORTCUT_ID_TURIZAO },
    { ITEM_TAUNMAPPU, SHORTCUT_ID_TOWNMAP },
	  { ITEM_BATORUREKOODAA, SHORTCUT_ID_BTLRECORDER },
    { ITEM_TOMODATITETYOU, SHORTCUT_ID_FRIENDNOTE },
    { ITEM_DAUZINGUMASIN, SHORTCUT_ID_DOWSINGMACHINE },
    { ITEM_GURASIDEANOHANA, SHORTCUT_ID_GURASHIDEA },
  };

  for( i = 0;i < NELEMS( Item2ShortcutTbl);i++){
    if( Item2ShortcutTbl[i].itemno == item ){
      return Item2ShortcutTbl[i].shortcut_id;
    }
  }
	return SHORTCUT_ID_NULL;
#else
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

	case ITEM_DAUZINGUMASIN:
		return SHORTCUT_ID_DOWSINGMACHINE;

	case ITEM_GURASIDEANOHANA:
    return SHORTCUT_ID_GURASHIDEA;

	default:
		return SHORTCUT_ID_NULL;
	}
#endif
}

