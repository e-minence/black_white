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
	TOUCHBAR_ICON_CLOSE,	//�~�{�^��
	TOUCHBAR_ICON_RETURN,	//�����{�^��
	TOUCHBAR_ICON_CUR_D,	//���{�^��
	TOUCHBAR_ICON_CUR_U,	//���{�^��
	TOUCHBAR_ICON_CUR_L,	//���{�^��
	TOUCHBAR_ICON_CUR_R,	//���{�^��
	TOUCHBAR_ICON_CHECK,	//���������{�^��

	TOUCHBAR_ICON_MAX,		//.c���Ŏg�p
	TOUCHBAR_ICON_CUTSOM1	= TOUCHBAR_ICON_MAX,	//�J�X�^���{�^���P
	TOUCHBAR_ICON_CUTSOM2,	//�J�X�^���{�^���Q
	TOUCHBAR_ICON_CUTSOM3,	//�J�X�^���{�^���R
	TOUCHBAR_ICON_CUTSOM4,	//�J�X�^���{�^���S

} TOUCHBAR_ICON;
//-------------------------------------
///	����ʃo�[�I�����Ă��Ȃ��ꍇ
//=====================================
#define TOUCHBAR_SELECT_NONE		((TOUCHBAR_ICON)GFL_UI_TP_HIT_NONE)

//-------------------------------------
///	�f�t�H���g�ʒu
//=====================================
//�o�[
#define TOUCHBAR_MENUBAR_X				(0)
#define TOUCHBAR_MENUBAR_Y				(21)
#define TOUCHBAR_MENUBAR_W				(32)
#define TOUCHBAR_MENUBAR_H				(3)
//�A�C�R��
#define TOUCHBAR_ICON_Y						(168)
#define TOUCHBAR_ICON_Y_CHECK			(172)
#define TOUCHBAR_ICON_X_00				(16)
#define TOUCHBAR_ICON_X_01				(48)
#define TOUCHBAR_ICON_X_02				(80)
#define TOUCHBAR_ICON_X_03				(112)
#define TOUCHBAR_ICON_X_04				(136)
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
#define APBAR_ICON_REGISTER_MAX			(8)

//-------------------------------------
///	�f�t�H���g�Ŏg�p����T�E���h��`
//=====================================
#define TOUCHBAR_SE_DECIDE	(SEQ_SE_DECIDE1)
#define TOUCHBAR_SE_CANCEL	(SEQ_SE_CANCEL1)
#define TOUCHBAR_SE_CLOSE	  (SEQ_SE_CLOSE1)
#define TOUCHBAR_SE_Y_REG		(SEQ_SE_SYS_07)

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

	//�ȉ��A�J�X�^���{�^���̂ݕK�v�ȏ��ł�
	u16	cg_idx;			// obj_graphic_man�̃L�����N�^�C���f�b�N�X
	u16	plt_idx;		// obj_graphic_man�̃p���b�g�C���f�b�N�X
	u16	cell_idx;		// obj_graphic_man�̃Z���@�A�j���C���f�b�N�X
	u16	active_anmseq;			// �{�^���̃A�N�e�B�u���A�j���V�[�P���X
	u16	noactive_anmseq;		// �{�^���̃m���A�N�e�B�u���A�j���V�[�P���X
	u16	push_anmseq;				// �{�^�����������Ƃ��̃A�j���V�[�P���X
	u32	key;								// �w�肳�ꂽ�L�[�̃{�^�����������Ƃ��ɃA�C�R���������ꂽ���Ƃɂ���(PAD_KEY_UP��������Ă������� �O���ƍs��Ȃ�) 
	u32	se;									// SE�@0���ƍs��Ȃ�

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
	u32 bar_frm;								//BG�L�����ǂݍ��݃t���[��	AreaMan��TOUCHBAR_BG_CHARAAREA_SIZE�ǂ݂܂�
	u32 bg_plt;									//BG�g�p�p���b�g0�`0xFTOUCHBAR_BG_PLT_NUM�{�ǂ݂܂�
	u32 obj_plt;								//OBJ�p���b�g0�`0xF		TOUCHBAR_OBJ_PLT_NUM�{�ǂ݂܂�
	APP_COMMON_MAPPING mapping;	//OBJ�}�b�s���O���[�h

	//BG�ǂ݂��܂Ȃ�
	BOOL is_notload_bg;					//TRUE���w�肷���BG��BG�p�p���b�g��ǂݍ��݂܂���
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
///	�A�C�R���g���K�[���	�������ĂȂ��ꍇTOUCHBAR_SELECT_NONE
//		�{�^���̃A�j�����I����Ă���PF�̂ݕԂ��܂�
//=====================================
extern TOUCHBAR_ICON TOUCHBAR_GetTrg( const TOUCHBAR_WORK *cp_wk );

//-------------------------------------
///	�A�C�R���g���K�[���	�������ĂȂ��ꍇTOUCHBAR_SELECT_NONE
//		�{�^����G�����u�ԂɂPF�̂ݕԂ��܂�
//=====================================
extern TOUCHBAR_ICON TOUCHBAR_GetTouch( const TOUCHBAR_WORK *cp_wk );

//-------------------------------------
///	�S�̂̐ݒ�
//(�����ł͂��ׂĂ�ICON��ݒ肵�Ă��邾���Ȃ̂ŁA�S�̂̃t���O�ƌʂ̃t���O�͏d�����Ă��܂�)
//=====================================
extern void TOUCHBAR_SetActiveAll( TOUCHBAR_WORK *p_wk, BOOL is_active );
extern void TOUCHBAR_SetVisibleAll( TOUCHBAR_WORK *p_wk, BOOL is_visible );

//=============================================================================
/**
 *	�A�C�R���ʐݒ�A�擾
 */
//=============================================================================
//-------------------------------------
///	�A�C�R���A�N�e�B�u�ݒ�
//		�A�N�e�B�u�A�m���A�N�e�B�u�ŃA�j�����ς��܂�
//		�܂��A�m���A�N�e�B�u��Ԃł̓A�C�R���������܂���
//=====================================
extern void TOUCHBAR_SetActive( TOUCHBAR_WORK *p_wk,  TOUCHBAR_ICON icon, BOOL is_active );
extern BOOL TOUCHBAR_GetActive( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon );
//-------------------------------------
///	�A�C�R����\���ݒ�
//=====================================
extern void TOUCHBAR_SetVisible( TOUCHBAR_WORK *p_wk,  TOUCHBAR_ICON icon, BOOL is_visible );
extern BOOL TOUCHBAR_GetVisible( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon );
//-------------------------------------
///	SE�ݒ�
//=====================================
extern void TOUCHBAR_SetSE( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u32 se );
extern u32 TOUCHBAR_GetSE( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon );
//-------------------------------------
///	Key�ݒ�
//=====================================
extern void TOUCHBAR_SetUseKey( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u32 key );
extern u32 TOUCHBAR_GetUseKey( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon );
//-------------------------------------
///	Flip�ݒ�
//	�t���b�v���{�^���iON,OFF�؂�ւ����{�^���j�ւ̐ݒ�
//=====================================
extern void TOUCHBAR_SetFlip( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, BOOL is_flip );
extern BOOL TOUCHBAR_GetFlip( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon );

