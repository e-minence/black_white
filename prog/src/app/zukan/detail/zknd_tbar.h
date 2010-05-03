//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		zknd_tbar.h
 *	@brief	�A�v���p����ʃ^�b�`�o�[
 *	@author	Koji Kawada
 *	@date		2010.02.04
 *
 *  @note   touchbar.h���R�s�y���āA
 *          (1) TOUCHBAR��ZKND_TBAR�ɒu�����A
 *          �K�v�ȋ@�\��ǉ����������ł��B
 *          Revision 14819 Date 2009.12.16�����B
 *          �ύX�ӏ��ɂ�ZKND_Modified�Ƃ������t���Ă����܂����B
 *
 *  ���W���[�����FZKND_TBAR
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
	ZKND_TBAR_ICON_CLOSE,	//�~�{�^��
	ZKND_TBAR_ICON_RETURN,	//�����{�^��
	ZKND_TBAR_ICON_CUR_D,	//���{�^��
	ZKND_TBAR_ICON_CUR_U,	//���{�^��
	ZKND_TBAR_ICON_CUR_L,	//���{�^��
	ZKND_TBAR_ICON_CUR_R,	//���{�^��
	ZKND_TBAR_ICON_CHECK,	//���������{�^��

	ZKND_TBAR_ICON_MAX,		//.c���Ŏg�p
	//ZKND_TBAR_ICON_CUTSOM1	= ZKND_TBAR_ICON_MAX,	//�J�X�^���{�^���P  // ZKND_Modified �J�X�^���̌������Ȃ��ɂ����B
	//ZKND_TBAR_ICON_CUTSOM2,	//�J�X�^���{�^���Q
	//ZKND_TBAR_ICON_CUTSOM3,	//�J�X�^���{�^���R
	//ZKND_TBAR_ICON_CUTSOM4,	//�J�X�^���{�^���S
	
  ZKND_TBAR_ICON_CUSTOM	= ZKND_TBAR_ICON_MAX,	//�J�X�^���{�^��(����ȏ�̒l��^���邱�ƁB�{�^�����Ƃɒl��ς��邱�ƁB)  // ZKND_Modified �J�X�^���̌������Ȃ��ɂ����B

} ZKND_TBAR_ICON;
//-------------------------------------
///	����ʃo�[�I�����Ă��Ȃ��ꍇ
//=====================================
#define ZKND_TBAR_SELECT_NONE		((ZKND_TBAR_ICON)GFL_UI_TP_HIT_NONE)

//-------------------------------------
///	�f�t�H���g�ʒu
//=====================================
//�o�[
#define ZKND_TBAR_MENUBAR_X				(0)
#define ZKND_TBAR_MENUBAR_Y				(21)
#define ZKND_TBAR_MENUBAR_W				(32)
#define ZKND_TBAR_MENUBAR_H				(3)
//�A�C�R��
#define ZKND_TBAR_ICON_Y						(168)
#define ZKND_TBAR_ICON_Y_CHECK			(172)
#define ZKND_TBAR_ICON_X_00				(16)
#define ZKND_TBAR_ICON_X_01				(48)
#define ZKND_TBAR_ICON_X_02				(80)
#define ZKND_TBAR_ICON_X_03				(112)
#define ZKND_TBAR_ICON_X_04				(136)
#define ZKND_TBAR_ICON_X_05				(168)
#define ZKND_TBAR_ICON_X_06				(200)
#define ZKND_TBAR_ICON_X_07				(232)
//�P�̃A�C�R���̕�
#define ZKND_TBAR_ICON_WIDTH					(24)
#define ZKND_TBAR_ICON_HEIGHT				(24)

//-------------------------------------
///	�A�C�R���̃f�t�H���g�v���C�I���e�B
//=====================================
#define ZKND_TBAR_ICON_OBJ_PRIORITY	(0)
#define ZKND_TBAR_ICON_BG_PRIORITY		(0)

//-------------------------------------
///	�g�p���郊�\�[�X
//=====================================
//�o�[��BG�L�����T�C�Y
#define ZKND_TBAR_BG_CHARAAREA_SIZE	(8*8*GFL_BG_1CHRDATASIZ)

//�o�[�̃p���b�g�{��
#define ZKND_TBAR_BG_PLT_NUM					(APP_COMMON_BAR_PLT_NUM)

//�o�[�̃{�^���̃p���b�g�{��
#define ZKND_TBAR_OBJ_PLT_NUM				(APP_COMMON_BARICON_PLT_NUM)

//�A�C�R���̍ő�o�^��
//#define APBAR_ICON_REGISTER_MAX			(8)  // ZKND_Modified �������Ȃ��ɂ����B

//-------------------------------------
///	�f�t�H���g�Ŏg�p����T�E���h��`
//=====================================
#define ZKND_TBAR_SE_DECIDE	(SEQ_SE_DECIDE1)
#define ZKND_TBAR_SE_CANCEL	(SEQ_SE_CANCEL1)
#define ZKND_TBAR_SE_CLOSE	  (SEQ_SE_CLOSE1)
#define ZKND_TBAR_SE_Y_REG		(SEQ_SE_SYS_07)

// ZKND_Modified SE�ǉ��B��
#define ZKND_TBAR_SE_SELECT	(SEQ_SE_SELECT1)
// ZKND_Modified SE�ǉ��B��

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
	ZKND_TBAR_ICON			icon;
	GFL_CLACTPOS			pos;
  u16               width;  // ZKND_Modified �����w��ł���悤�ɂ����B

	//�ȉ��A�J�X�^���{�^���̂ݕK�v�ȏ��ł�
	u16	cg_idx;			// obj_graphic_man�̃L�����N�^�C���f�b�N�X
	u16	plt_idx;		// obj_graphic_man�̃p���b�g�C���f�b�N�X
	u16	cell_idx;		// obj_graphic_man�̃Z���@�A�j���C���f�b�N�X
	u16	active_anmseq;			// �{�^���̃A�N�e�B�u���A�j���V�[�P���X
	u16	noactive_anmseq;		// �{�^���̃m���A�N�e�B�u���A�j���V�[�P���X
	u16	push_anmseq;				// �{�^�����������Ƃ��̃A�j���V�[�P���X
	u32	key;								// �w�肳�ꂽ�L�[�̃{�^�����������Ƃ��ɃA�C�R���������ꂽ���Ƃɂ���(PAD_KEY_UP��������Ă������� �O���ƍs��Ȃ�) 
	u32	se;									// SE�@0���ƍs��Ȃ�

} ZKND_TBAR_ITEM_ICON;

//-------------------------------------
///	�A�v���P�[�V�����o�[�ݒ�\����
//=====================================
typedef struct 
{
	//�A�C�R�����
	ZKND_TBAR_ITEM_ICON	*p_item;	//�쐬�������A�C�R�������[�N���쐬���e�[�u����n���Ă�������
	u32									item_num;	//���̃A�C�R�����[�N�̐��ł��B

	//�^�b�`�o�[�Ŏg�p���郂�W���[��
	GFL_CLUNIT* p_unit;
	
	//���\�[�X�ǂݍ��ݏ��
	u32 bar_frm;								//BG�L�����ǂݍ��݃t���[��	AreaMan��ZKND_TBAR_BG_CHARAAREA_SIZE�ǂ݂܂�
	u32 bg_plt;									//BG�g�p�p���b�g0�`0xFZKND_TBAR_BG_PLT_NUM�{�ǂ݂܂�
	u32 obj_plt;								//OBJ�p���b�g0�`0xF		ZKND_TBAR_OBJ_PLT_NUM�{�ǂ݂܂�
	APP_COMMON_MAPPING mapping;	//OBJ�}�b�s���O���[�h

	//BG�ǂ݂��܂Ȃ�
	BOOL is_notload_bg;					//TRUE���w�肷���BG��BG�p�p���b�g��ǂݍ��݂܂���
} ZKND_TBAR_SETUP;

//-------------------------------------
///	����ʃo�[
//=====================================
typedef struct _ZKND_TBAR_WORK ZKND_TBAR_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	������
//=====================================
extern ZKND_TBAR_WORK * ZKND_TBAR_Init( ZKND_TBAR_SETUP *p_setup, HEAPID heapID );
//-------------------------------------
///	�j��
//=====================================
extern void ZKND_TBAR_Exit( ZKND_TBAR_WORK *p_wk );
//-------------------------------------
///	���C������
//=====================================
extern void ZKND_TBAR_Main( ZKND_TBAR_WORK *p_wk );
//-------------------------------------
///	�A�C�R���g���K�[���	�������ĂȂ��ꍇZKND_TBAR_SELECT_NONE
//		�{�^���̃A�j�����I����Ă���PF�̂ݕԂ��܂�
//=====================================
extern ZKND_TBAR_ICON ZKND_TBAR_GetTrg( const ZKND_TBAR_WORK *cp_wk );

//-------------------------------------
///	�A�C�R���g���K�[���	�������ĂȂ��ꍇZKND_TBAR_SELECT_NONE
//		�{�^����G�����u�ԂɂPF�̂ݕԂ��܂�
//=====================================
extern ZKND_TBAR_ICON ZKND_TBAR_GetTouch( const ZKND_TBAR_WORK *cp_wk );

//-------------------------------------
///	�S�̂̐ݒ�
//(�����ł͂��ׂĂ�ICON��ݒ肵�Ă��邾���Ȃ̂ŁA�S�̂̃t���O�ƌʂ̃t���O�͏d�����Ă��܂�)
//=====================================
extern void ZKND_TBAR_SetActiveAll( ZKND_TBAR_WORK *p_wk, BOOL is_active );
extern void ZKND_TBAR_SetVisibleAll( ZKND_TBAR_WORK *p_wk, BOOL is_visible );

// ZKND_Modified �S�̐�p�̐ݒ��ǉ��B��
//-------------------------------------
///	�S�̂̃A�N�e�B�u��؂�ւ���
// �����ڂ��A�N�e�B�u��Ԃɂ����܂܁A�S�̐�p�̃A�N�e�B�u�t���O�̐؂�ւ����s��
//=====================================
extern void ZKND_TBAR_SetActiveWhole( ZKND_TBAR_WORK *p_wk, BOOL is_active );
extern BOOL ZKND_TBAR_GetActiveWhole( ZKND_TBAR_WORK *p_wk );
//-------------------------------------
///	�S�̂̃��b�N����������
// �����ڂ��A�N�e�B�u��Ԃɂ����܂܂������Ă���S�̐�p�̃��b�N����������
// �Q�x�������Ȃ��悤�ɁA�P�x�������牟���Ȃ��悤�Ƀ��b�N�������邱�Ƃɂ���
//=====================================
extern void ZKND_TBAR_UnlockWhole( ZKND_TBAR_WORK *p_wk );
// ZKND_Modified �S�̐�p�̐ݒ��ǉ��B��

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
extern void ZKND_TBAR_SetActive( ZKND_TBAR_WORK *p_wk,  ZKND_TBAR_ICON icon, BOOL is_active );
extern BOOL ZKND_TBAR_GetActive( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon );
//-------------------------------------
///	�A�C�R����\���ݒ�
//=====================================
extern void ZKND_TBAR_SetVisible( ZKND_TBAR_WORK *p_wk,  ZKND_TBAR_ICON icon, BOOL is_visible );
extern BOOL ZKND_TBAR_GetVisible( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon );
//-------------------------------------
///	SE�ݒ�
//=====================================
extern void ZKND_TBAR_SetSE( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, u32 se );
extern u32 ZKND_TBAR_GetSE( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon );
//-------------------------------------
///	Key�ݒ�
//=====================================
extern void ZKND_TBAR_SetUseKey( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, u32 key );
extern u32 ZKND_TBAR_GetUseKey( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon );
//-------------------------------------
///	Flip�ݒ�
//	�t���b�v���{�^���iON,OFF�؂�ւ����{�^���j�ւ̐ݒ�
//=====================================
extern void ZKND_TBAR_SetFlip( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, BOOL is_flip );
extern BOOL ZKND_TBAR_GetFlip( const ZKND_TBAR_WORK *cp_wk, ZKND_TBAR_ICON icon );

// ZKND_Modified �A�C�R���ʑ����ǉ��B��
//-------------------------------------
///	�A�C�R����GFL_CLWK���擾����
//=====================================
extern GFL_CLWK* ZKND_TBAR_GetClwk( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon );
//-------------------------------------
///	�A�C�R���̈ʒu��ύX����
//=====================================
extern void ZKND_TBAR_SetPos( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon, const GFL_CLACTPOS* pos );
//-------------------------------------
///	�A�C�R�����L�[�ŉ��������Ƃɂ���
//=====================================
extern void ZKND_TBAR_PushByKey( ZKND_TBAR_WORK *p_wk, ZKND_TBAR_ICON icon );
// ZKND_Modified �A�C�R���ʑ����ǉ��B��

