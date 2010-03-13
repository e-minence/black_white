//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut_menu.h
 *	@brief	�V���[�g�J�b�g���j���[����
 *	@author	Toru=Nagihashi
 *	@date		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/game_data.h"
#include "system/shortcut_data.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�V���[�g�J�b�g���j���[�œ��͂������
//=====================================
typedef enum
{
	SHORTCUTMENU_INPUT_NONE,		//�������͂��Ă��Ȃ�
	SHORTCUTMENU_INPUT_SELECT,	//�I������
	SHORTCUTMENU_INPUT_CANCEL,	//�L�����Z������
} SHORTCUTMENU_INPUT;

//-------------------------------------
///	�V���[�g�J�b�g���j���[�N�����[�h
//=====================================
typedef enum
{
	SHORTCUTMENU_MODE_POPUP,	//������o�Ă���
	SHORTCUTMENU_MODE_STAY,		//���łɕ\������Ă���
} SHORTCUTMENU_MODE;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�V���[�g�J�b�g���j���[
//=====================================
typedef struct _SHORTCUTMENU_WORK SHORTCUTMENU_WORK;

//=============================================================================
/**
 *					�O���Q�Ɛ錾
*/
//=============================================================================
//-------------------------------------
///	�V�X�e��
//=====================================
extern SHORTCUTMENU_WORK *SHORTCUTMENU_Init( GAMEDATA *p_gamedata, SHORTCUTMENU_MODE mode, SHORTCUT_CURSOR *p_cursor, HEAPID sys_heapID, HEAPID res_heapID );
extern void SHORTCUTMENU_Exit( SHORTCUTMENU_WORK *p_wk );
extern void SHORTCUTMENU_Main( SHORTCUTMENU_WORK *p_wk );

//-------------------------------------
///	���j���[�J�n
//=====================================
extern void SHORTCUTMENU_Open( SHORTCUTMENU_WORK *p_wk );
extern void SHORTCUTMENU_Close( SHORTCUTMENU_WORK *p_wk );
extern BOOL SHORTCUTMENU_IsMove( const SHORTCUTMENU_WORK *cp_wk );

//-------------------------------------
///	�擾
//=====================================
extern SHORTCUTMENU_INPUT SHORTCUTMENU_GetInput( const SHORTCUTMENU_WORK *cp_wk, SHORTCUT_ID *p_shortcutID );
