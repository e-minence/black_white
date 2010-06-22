//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_appbar.h
 *	@brief	�ԊO���`�F�b�N�p����ʃo�[
 *	@author	Toru=Nagihashi
 *	@date		2009.07.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//lib
#include <gflib.h>

#include "print/gf_font.h"
#include "print/printsys.h"

//resource
#include "app/app_menu_common.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�A�C�R���̎��
//=====================================
typedef enum
{	
	APPBAR_ICON_CLOSE,
	APPBAR_ICON_RETURN,
	APPBAR_ICON_CUR_D,
	APPBAR_ICON_CUR_U,
	APPBAR_ICON_CUR_L,
	APPBAR_ICON_CUR_R,
	APPBAR_ICON_CHECK,
	APPBAR_ICON_MAX,
} APPBAR_ICON;
//-------------------------------------
///	����ʃo�[
//=====================================
typedef enum
{	
	APPBAR_OPTION_MASK_CLOSE		= 1<<APPBAR_ICON_CLOSE,	//����
	APPBAR_OPTION_MASK_RETURN		= 1<<APPBAR_ICON_RETURN,	//�߂�
	APPBAR_OPTION_MASK_CUR_D		= 1<<APPBAR_ICON_CUR_D,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_U		= 1<<APPBAR_ICON_CUR_U,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_L		= 1<<APPBAR_ICON_CUR_L,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_R		= 1<<APPBAR_ICON_CUR_R,	//�J�[�\���E
	APPBAR_OPTION_MASK_CHECK		= 1<<APPBAR_ICON_CHECK,	//�`�F�b�N�{�b�N�X
} APPBAR_OPTION_MASK;
//-------------------------------------
///	����ʃo�[
//=====================================
#define APPBAR_SELECT_NONE		    ((APPBAR_ICON)GFL_UI_TP_HIT_NONE)

//-------------------------------------
///	�g�p���郊�\�[�X
//=====================================
//�o�[�̃p���b�g�{��
#define APPBAR_BG_PLT_NUM					(APP_COMMON_BAR_PLT_NUM)

//�o�[�̃{�^���̃p���b�g�{��
#define APPBAR_OBJ_PLT_NUM				(APP_COMMON_BARICON_PLT_NUM)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����ʃo�[(�A�v���P�[�V�����o�[�Ə���ɖ���)
//=====================================
typedef struct _APPBAR_WORK APPBAR_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
extern APPBAR_WORK * APPBAR_Init( APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, APP_COMMON_MAPPING mapping, GFL_FONT* p_font, PRINT_QUE* p_que, HEAPID heapID );
extern void APPBAR_Exit( APPBAR_WORK *p_wk );
extern void APPBAR_Main( APPBAR_WORK *p_wk );

extern BOOL APPBAR_IsBtnEffect( const APPBAR_WORK *cp_wk );
extern APPBAR_ICON APPBAR_GetTrg( const APPBAR_WORK *cp_wk );
extern void APPBAR_SetNormal( APPBAR_WORK *p_wk );
extern BOOL APPBAR_IsStartAnime( const APPBAR_WORK *cp_wk );

extern void APPBAR_SetVisible( APPBAR_WORK *p_wk, BOOL is_visible );
extern void APPBAR_SetTouchEnable( APPBAR_WORK *p_wk, BOOL is_use );

typedef enum
{ 
  APPBAR_BUTTON_TYPE_RETURN,  //���ǂ�
  APPBAR_BUTTON_TYPE_CANCEL,  //��߂�
}APPBAR_BUTTON_TYPE;
extern void APPBAR_ChangeButton( APPBAR_WORK *p_wk, APPBAR_BUTTON_TYPE type );

