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
#define	TB_SX			( 24 )
#define	TB_SY			( 24 )
#define	TB_PY			( TOUCHBAR_ICON_Y )
#define	TB_PX07		( TOUCHBAR_ICON_X_07 )
#define	TB_PX06		( TB_PX07 - TB_SX )
#define	TB_PX05		( TB_PX06 - TB_SX )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================


//============================================================================================
//	�O���[�o��
//============================================================================================

// �^�b�`���W�e�[�u��
static const GFL_UI_TP_HITTBL TouchHitTbl[] =
{
//	{   0, 167, 128, 247 },		// 00: ���X�g
//	{   8, 159, 248, 255 },		// 01: ���[��

	{   0,  95,   0,  95 },		// 02: �|�P�������ʊG

	{   0,  23,  96, 127 },		// 03: �|�P�����A�C�R��
	{  24,  47,  96, 127 },		// 04: �|�P�����A�C�R��
	{  48,  71,  96, 127 },		// 05: �|�P�����A�C�R��
	{  72,  95,  96, 127 },		// 06: �|�P�����A�C�R��
	{  96, 119,  96, 127 },		// 07: �|�P�����A�C�R��
	{ 120, 143,  96, 127 },		// 08: �|�P�����A�C�R��
	{ 144, 167,  96, 127 },		// 09: �|�P�����A�C�R��

	{ 168, 191,   0,  71 },		// 10: �X�^�[�g
	{ 168, 191,  72, 135 },		// 11: �Z���N�g
//	{ 168, 191, 136, 159 },		// 12: ��
//	{ 168, 191, 160, 183 },		// 13: �E
	{ TB_PY, TB_PY+TB_SY-1, TB_PX05, TB_PX05+TB_SX-1 },		// 14: �x
	{ TB_PY, TB_PY+TB_SY-1, TB_PX06, TB_PX06+TB_SX-1 },		// 15: �w
	{ TB_PY, TB_PY+TB_SY-1, TB_PX07, TB_PX07+TB_SX-1 },		// 16: �߂�

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};

// ���[���X�N���[���^�b�`���W�e�[�u��
static const GFL_UI_TP_HITTBL RailHitTbl[] =
{
	{   0, 191, 224, 255 },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};



int ZKNLISTUI_ListMain( ZKNLISTMAIN_WORK * wk )
{
	int	ret;

	ret = GFL_UI_TP_HitTrg( TouchHitTbl );

	if( ret != GFL_UI_TP_HIT_NONE ){
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
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return ret;
	}

/*
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		return ZKNLISTUI_ID_POKE;
	}
*/

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_RETURN;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_X;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_START;
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
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNLISTUI_ID_Y;
	}

	return ret;
}

/*
BOOL ZKNLISTUI_CheckRailHit( u32 * x, u32 * y )
{
	if( GFL_UI_TP_HitCont( RailHitTbl ) != GFL_UI_TP_HIT_NONE ){
		GFL_UI_TP_GetPointCont( x, y );
		return TRUE;
	}
	return FALSE;
}

BOOL ZKNLISTUI_CheckListHit( u32 * x, u32 * y )
{
	if( GFL_UI_TP_HitCont( TouchHitTbl ) == ZKNLISTUI_ID_LIST ){
		GFL_UI_TP_GetPointCont( x, y );
		return TRUE;
	}
	return FALSE;
}
*/
