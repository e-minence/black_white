//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut_data.h
 *	@brief	Y�{�^���o�^
 *	@author	Toru=Nagihashi
 *	@date		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	Y�{�^���o�^�ӏ�
//=====================================
typedef enum
{
	SHORTCUT_ID_ZITENSYA,
	SHORTCUT_ID_BORONOTURIZAO,
	SHORTCUT_ID_IITURIZAO,
	SHORTCUT_ID_SUGOITURIZAO,
	SHORTCUT_ID_TOWNMAP,
	SHORTCUT_ID_BAG_WAZAMACHINE,
	SHORTCUT_ID_PSTATUS_WAZA,


	SHORTCUT_ID_MAX,
	SHORTCUT_ID_NULL	= SHORTCUT_ID_MAX,	//�f�[�^�Ȃ�
} SHORTCUT_ID;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern SHORTCUT_ID SHORTCUT_DATA_GetItemToShortcutID( u8 item );
