//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		touchbar.h
 *	@brief	�A�v���p����ʃ^�b�`�o�[
 *	@author	Toru=Nagihashi
 *	@date		2009.09.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//lib
#include <gflib.h>

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
	TOUCHBAR_ICON_CLOSE,
	TOUCHBAR_ICON_RETURN,
	TOUCHBAR_ICON_CUR_D,
	TOUCHBAR_ICON_CUR_U,
	TOUCHBAR_ICON_CUR_L,
	TOUCHBAR_ICON_CUR_R,
	TOUCHBAR_ICON_CHECK,

	TOUCHBAR_ICON_MAX,
} TOUCHBAR_ICON;
//-------------------------------------
///	����ʃo�[�I�����Ă��Ȃ��ꍇ
//=====================================
#define TOUCHBAR_SELECT_NONE		((TOUCHBAR_ICON)GFL_UI_TP_HIT_NONE)

//-------------------------------------
///	�f�t�H���g�ʒu
//=====================================
//�o�[
#define TOUCHBAR_MENUBAR_X					(0)
#define TOUCHBAR_MENUBAR_Y					(21)
#define TOUCHBAR_MENUBAR_W					(32)
#define TOUCHBAR_MENUBAR_H					(3)
//�A�C�R��
#define TOUCHBAR_ICON_Y						(168)
#define TOUCHBAR_ICON_Y_CHECK			(172)
#define TOUCHBAR_ICON_X_01				(16)
#define TOUCHBAR_ICON_X_02				(38)
#define TOUCHBAR_ICON_X_03				(16)
#define TOUCHBAR_ICON_X_04				(38)
#define TOUCHBAR_ICON_X_05				(168)
#define TOUCHBAR_ICON_X_06				(200)
#define TOUCHBAR_ICON_X_07				(232)
//�P�̃A�C�R���̕�
#define TOUCHBAR_ICON_WIDTH					(24)
#define TOUCHBAR_ICON_HEIGHT				(24)

//-------------------------------------
///	�A�C�R���̃f�t�H���g�v���C�I���e�B
//=====================================
#define TOUCHBAR_ICON_OBJ_PRIORITY	(0)
#define TOUCHBAR_ICON_BG_PRIORITY		(0)

//-------------------------------------
///	�g�p���郊�\�[�X
//=====================================
//�o�[��BG�L�����T�C�Y
#define TOUCHBAR_BG_CHARAAREA_SIZE	(8*8*GFL_BG_1CHRDATASIZ)

//�o�[�̃p���b�g�{��
#define TOUCHBAR_BG_PLT_NUM					(APP_COMMON_BAR_PLT_NUM)

//�o�[�̃{�^���̃p���b�g�{��
#define TOUCHBAR_OBJ_PLT_NUM				(APP_COMMON_BARICON_PLT_NUM)

//�A�C�R���̍ő�o�^��
#define APBAR_ICON_REGISTER_MAX		(8)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�A�v���P�[�V�����o�[�A�C�R��
//=====================================
typedef struct 
{
	TOUCHBAR_ICON			icon;
	GFL_CLACTPOS			pos;
} TOUCHBAR_ITEM_ICON;

//-------------------------------------
///	�A�v���P�[�V�����o�[�ݒ�\����
//=====================================
typedef struct 
{
	//�A�C�R�����
	TOUCHBAR_ITEM_ICON	*p_item;	//�쐬�������A�C�R�������[�N���쐬���e�[�u����n���Ă�������
	u32									item_num;	//���̃A�C�R�����[�N�̐��ł��B

	//�^�b�`�o�[�Ŏg�p���郂�W���[��
	GFL_CLUNIT* p_unit;
	
	//���\�[�X�ǂݍ��ݏ��
	u8 bar_frm;									//BG�L�����ǂݍ��݃t���[��	AreaMan��TOUCHBAR_BG_CHARAAREA_SIZE�ǂ݂܂�
	u8 bg_plt;									//BG�g�p�p���b�g0�`0xFTOUCHBAR_BG_PLT_NUM�{�ǂ݂܂�
	u8 obj_plt;									//OBJ�p���b�g0�`0xF		TOUCHBAR_OBJ_PLT_NUM�{�ǂ݂܂�
	APP_COMMON_MAPPING mapping;	//OBJ�}�b�s���O���[�h
} TOUCHBAR_SETUP;

//-------------------------------------
///	����ʃo�[
//=====================================
typedef struct _TOUCHBAR_WORK TOUCHBAR_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	������
//=====================================
extern TOUCHBAR_WORK * TOUCHBAR_Init( TOUCHBAR_SETUP *p_setup, HEAPID heapID );
//-------------------------------------
///	�j��
//=====================================
extern void TOUCHBAR_Exit( TOUCHBAR_WORK *p_wk );
//-------------------------------------
///	���C������
//=====================================
extern void TOUCHBAR_Main( TOUCHBAR_WORK *p_wk );
//-------------------------------------
///	�A�C�R���g���K�[���	���������ĂȂ��ꍇTOUCHBAR_SELECT_NONE
//=====================================
extern TOUCHBAR_ICON TOUCHBAR_GetTrg( const TOUCHBAR_WORK *cp_wk );
//-------------------------------------
///	�A�C�R�������������	���������ĂȂ��ꍇTOUCHBAR_SELECT_NONE
//=====================================
extern TOUCHBAR_ICON TOUCHBAR_GetCont( const TOUCHBAR_WORK *cp_wk );


