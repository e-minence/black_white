//============================================================================================
/**
 * @file		zknlist_ui.c
 * @brief		�}�Ӄ��X�g��� �C���^�[�t�F�C�X�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTUI
 */
//============================================================================================
#include <gflib.h>

#include "ui/touchbar.h"

#include "zknlist_main.h"
#include "zknlist_ui.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �^�b�`�o�[��̃{�^�����W
#define	TB_SX			( 24 )
#define	TB_SY			( 24 )
#define	TB_PY			( TOUCHBAR_ICON_Y )
#define	TB_PX07		( TOUCHBAR_ICON_X_07 )
#define	TB_PX06		( TB_PX07 - TB_SX )
#define	TB_PX05		( TB_PX06 - TB_SX )


//============================================================================================
//	�O���[�o��
//============================================================================================

// �^�b�`���W�e�[�u��
static const GFL_UI_TP_HITTBL TouchHitTbl[] =
{
	{   0,  95,   0,  95 },		// 00: �|�P�������ʊG

	{   0,  23,  96, 127 },		// 01: �|�P�����A�C�R��
	{  24,  47,  96, 127 },		// 02: �|�P�����A�C�R��
	{  48,  71,  96, 127 },		// 03: �|�P�����A�C�R��
	{  72,  95,  96, 127 },		// 04: �|�P�����A�C�R��
	{  96, 119,  96, 127 },		// 05: �|�P�����A�C�R��
	{ 120, 143,  96, 127 },		// 06: �|�P�����A�C�R��
	{ 144, 167,  96, 127 },		// 07: �|�P�����A�C�R��

	{ 168, 191,   0,  71 },		// 08: �X�^�[�g
	{ 168, 191,  72, 135 },		// 09: �Z���N�g

	{ TB_PY, TB_PY+TB_SY-1, TB_PX05, TB_PX05+TB_SX-1 },		// 10: �x
	{ TB_PY, TB_PY+TB_SY-1, TB_PX06, TB_PX06+TB_SX-1 },		// 11: �w
	{ TB_PY, TB_PY+TB_SY-1, TB_PX07, TB_PX07+TB_SX-1 },		// 12: �߂�

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�Ӄ��X�g��ʓ��͏���
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
int ZKNLISTUI_ListMain( ZKNLISTMAIN_WORK * wk )
{
	int	ret;

	ret = GFL_UI_TP_HitTrg( TouchHitTbl );

	if( ret != GFL_UI_TP_HIT_NONE ){
		// ����
		if( ret == ZKNLISTUI_ID_START ){
			// �o�[�W�����A�b�v�O
			if( ZUKANSAVE_GetGraphicVersionUpFlag( wk->dat->savedata ) == FALSE ){
				return GFL_UI_TP_HIT_NONE;
			}
		}
		// ���[�h�ؑ�
		if( ret == ZKNLISTUI_ID_SELECT ){
			// �S���}�Ӗ��擾��
			if( ZUKANSAVE_GetZenkokuZukanFlag( wk->dat->savedata ) == FALSE ){
				return GFL_UI_TP_HIT_NONE;
			}
			// �������X�g��
			if( wk->dat->callMode == ZKNLIST_CALL_SEARCH ){
				return GFL_UI_TP_HIT_NONE;
			}
		}
		// �x�o�^
		if( ret == ZKNLISTUI_ID_Y ){
			// �������X�g��
			if( wk->dat->callMode == ZKNLIST_CALL_SEARCH ){
				return GFL_UI_TP_HIT_NONE;
			}
			if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
				GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
				FRAMELIST_SetCursorPosPaletteOff( wk->lwk );
			}
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_RETURN;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_X;
	}

	// ����
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		// �o�[�W�����A�b�v��
		if( ZUKANSAVE_GetGraphicVersionUpFlag( wk->dat->savedata ) == TRUE ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			return ZKNLISTUI_ID_START;
		}
	}

	// ���[�h�ؑ�
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		// �S���}�Ӗ��擾��
		if( ZUKANSAVE_GetZenkokuZukanFlag( wk->dat->savedata ) == FALSE ){
			return GFL_UI_TP_HIT_NONE;
		}
		// �������X�g��
		if( wk->dat->callMode == ZKNLIST_CALL_SEARCH ){
			return GFL_UI_TP_HIT_NONE;
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_SELECT;
	}

	// �x�o�^
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		// �������X�g��
		if( wk->dat->callMode == ZKNLIST_CALL_SEARCH ){
			return GFL_UI_TP_HIT_NONE;
		}
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			FRAMELIST_SetCursorPosPalette( wk->lwk );
		}else{
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		}
		return ZKNLISTUI_ID_Y;
	}

	return ret;
}
