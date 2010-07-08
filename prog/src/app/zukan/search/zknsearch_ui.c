//============================================================================================
/**
 * @file		zknsearch_ui.c
 * @brief		�}�ӌ������ �t�h�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZKNSEARCHUI
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "ui/touchbar.h"

#include "zknsearch_main.h"
#include "zknsearch_ui.h"


//============================================================================================
//	�萔��`
//============================================================================================
// �^�b�`�o�[��̃{�^���f�[�^
#define	TB_SX			( 24 )
#define	TB_SY			( 24 )
#define	TB_PY			( TOUCHBAR_ICON_Y )
#define	TB_PX07		( TOUCHBAR_ICON_X_07 )
#define	TB_PX06		( TB_PX07 - TB_SX )
#define	TB_PX05		( TB_PX06 - TB_SX )

// �J�[�\���\������L�[
#define	PAD_MAIN_CURSOR_ON	(PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_BUTTON_DECIDE)


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void ChangePosMenuMainKey( ZKNSEARCHMAIN_WORK * wk, u32 pos );

static void MainCallBack_On( void * work, int now_pos, int old_pos );
static void MainCallBack_Off( void * work, int now_pos, int old_pos );
static void MainCallBack_Move( void * work, int now_pos, int old_pos );
static void MainCallBack_Touch( void * work, int now_pos, int old_pos );


//============================================================================================
//	�O���[�o��
//============================================================================================

// ���C���y�[�W�̃J�[�\���ړ��f�[�^
static const CURSORMOVE_DATA MainCursorMoveData[] =
{
	{ 16,  0, 16, 3,	5, 1, 0, 0,	{   0,  23, 128, 255 } },		// 00: ����
	{ 16,  3, 16, 3,	0, 2, 1, 1,	{  24,  47, 128, 255 } },		// 01: ���O
	{ 16,  6, 16, 3,	1, 3, 2, 2,	{  48,  71, 128, 255 } },		// 02: �^�C�v
	{ 16,  9, 16, 3,	2, 4, 3, 3,	{  72,  95, 128, 255 } },		// 03: �F
	{ 16, 12, 16, 5,	3, 5, 4, 4,	{  96, 135, 128, 255 } },		// 04: �`
	{ 14, 18, 18, 3,	4, 0, 6, 6,	{ 144, 167, 113, 255 } },		// 05: �X�^�[�g
	{  0, 18,  7, 3,	4, 0, 5, 5,	{ 144, 167,   0,  50 } },		// 06: ���Z�b�g

	{ 0, 0, 0, 0,	7, 7, 7, 7, { TB_PY, TB_PY+TB_SY-1, TB_PX05, TB_PX05+TB_SX-1 } },		// 07: �x
	{ 0, 0, 0, 0,	8, 8, 8, 8, { TB_PY, TB_PY+TB_SY-1, TB_PX06, TB_PX06+TB_SX-1 } },		// 08: �w
	{ 0, 0, 0, 0,	9, 9, 9, 9, { TB_PY, TB_PY+TB_SY-1, TB_PX07, TB_PX07+TB_SX-1 } },		// 09: �߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
// ���C���y�[�W�̃J�[�\���ړ��R�[���o�b�N
static const CURSORMOVE_CALLBACK MainCallBack = {
	MainCallBack_On,
	MainCallBack_Off,
	MainCallBack_Move,
	MainCallBack_Touch,
};

// ���X�g�y�[�W�^�b�`���W
static const GFL_UI_TP_HITTBL ListTouchHitTbl[] =
{
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 },		// 00: �߂�

	{   0,  23, 128, 143 },		// 01: �`�F�b�N�{�b�N�X
	{  24,  47, 128, 143 },		// 02: �`�F�b�N�{�b�N�X
	{  48,  71, 128, 143 },		// 03: �`�F�b�N�{�b�N�X
	{  72,  95, 128, 143 },		// 04: �`�F�b�N�{�b�N�X
	{  96, 119, 128, 143 },		// 05: �`�F�b�N�{�b�N�X
	{ 120, 143, 128, 143 },		// 06: �`�F�b�N�{�b�N�X
	{ 144, 167, 128, 143 },		// 07: �`�F�b�N�{�b�N�X

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};

// ���X�g�y�[�W�^�b�`���W�i�t�H�����p�j
static const GFL_UI_TP_HITTBL FormListTouchHitTbl[] =
{
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 },		// 00: �߂�

	{   0,  39, 128, 143 },		// 01: �`�F�b�N�{�b�N�X
	{  40,  79, 128, 143 },		// 02: �`�F�b�N�{�b�N�X
	{  80, 119, 128, 143 },		// 03: �`�F�b�N�{�b�N�X
	{ 120, 159, 128, 143 },		// 04: �`�F�b�N�{�b�N�X

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���y�[�W�̃J�[�\���ړ�����������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHUI_MainCursorMoveInit( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	BOOL	flg;

	if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
		wk->cmwk = CURSORMOVE_Create(
								MainCursorMoveData, &MainCallBack, wk, FALSE, pos, HEAPID_ZUKAN_SEARCH );
	}else{
		wk->cmwk = CURSORMOVE_Create(
								MainCursorMoveData, &MainCallBack, wk, TRUE, pos, HEAPID_ZUKAN_SEARCH );
		ZKNSEARCHUI_ChangeCursorPalette( wk, pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
	}
	CURSORMOVE_VanishModeSet( wk->cmwk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���y�[�W�̃J�[�\���ړ��������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHUI_CursorMoveExit( ZKNSEARCHMAIN_WORK * wk )
{
	if( wk->cmwk != NULL ){
		CURSORMOVE_Exit( wk->cmwk );
		wk->cmwk = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڂ̃p���b�g�ύX
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		pos		�ʒu
 * @param		pal		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHUI_ChangeCursorPalette( ZKNSEARCHMAIN_WORK * wk, u32 pos, u32 pal )
{
	if( pos <= ZKNSEARCHUI_RESET ){
		const CURSORMOVE_DATA * dat = CURSORMOVE_GetMoveData( wk->cmwk, pos );
		GFL_BG_ChangeScreenPalette( GFL_BG_FRAME0_M, dat->px, dat->py, dat->sx, dat->sy, pal );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���y�[�W�̃J�[�\���ړ�����
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
u32 ZKNSEARCHUI_MenuMain( ZKNSEARCHMAIN_WORK * wk )
{
	u32	ret = CURSORMOVE_MainCont( wk->cmwk );

	if( ret != CURSORMOVE_NONE ){
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_X;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
//		ChangePosMenuMainKey( wk, CURSORMOVE_PosGet(wk->cmwk) );
		return ZKNSEARCHUI_Y;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		ChangePosMenuMainKey( wk, ZKNSEARCHUI_START );
		return ZKNSEARCHUI_START;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
		}
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		ChangePosMenuMainKey( wk, ZKNSEARCHUI_RESET );
		return ZKNSEARCHUI_RESET;
	}

	if( GFL_UI_KEY_GetTrg() ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			CURSORMOVE_CursorOnOffSet( wk->cmwk, TRUE );
//			ChangePosMenuMainKey( wk, CURSORMOVE_PosGet(wk->cmwk) );
			return CURSORMOVE_CURSOR_ON;
		}
	}	

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�ύX
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		pos		�J�[�\���ʒu�ړ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangePosMenuMainKey( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	ZKNSEARCHUI_ChangeCursorPalette( wk, CURSORMOVE_PosGet(wk->cmwk), ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
	ZKNSEARCHUI_ChangeCursorPalette( wk, pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );

	CURSORMOVE_PosSet( wk->cmwk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\���\��
 *
 * @param		work			�}�ӌ�����ʃ��[�N
 * @param		now_pos		���݂̃J�[�\���ʒu
 * @param		old_pos		�O��̃J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_On( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk;
	int	trg;
	
	wk  = work;
	trg = GFL_UI_KEY_GetTrg();

	// �L�����Z�����̓J�[�\����ON�ɂ��Ȃ�
	if( ( trg & PAD_BUTTON_CANCEL ) && !( trg & PAD_MAIN_CURSOR_ON ) ){
		return;
	}

	ZKNSEARCHUI_ChangeCursorPalette( wk, now_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
	BLINKPALANM_InitAnimeCount( wk->blink );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\����\��
 *
 * @param		work			�}�ӌ�����ʃ��[�N
 * @param		now_pos		���݂̃J�[�\���ʒu
 * @param		old_pos		�O��̃J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_Off( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk;

	wk  = work;

	ZKNSEARCHUI_ChangeCursorPalette( wk, old_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
	ZKNSEARCHUI_ChangeCursorPalette( wk, now_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\���ړ�
 *
 * @param		work			�}�ӌ�����ʃ��[�N
 * @param		now_pos		���݂̃J�[�\���ʒu
 * @param		old_pos		�O��̃J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_Move( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHUI_ChangeCursorPalette( wk, old_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );
	ZKNSEARCHUI_ChangeCursorPalette( wk, now_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2 );
	BLINKPALANM_InitAnimeCount( wk->blink );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�^�b�`
 *
 * @param		work			�}�ӌ�����ʃ��[�N
 * @param		now_pos		���݂̃J�[�\���ʒu
 * @param		old_pos		�O��̃J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHUI_ChangeCursorPalette( wk, old_pos, ZKNSEARCHMAIN_MBG_PAL_BUTTON_DEF );

	if( now_pos == ZKNSEARCHUI_Y ){
		CURSORMOVE_PosSet( wk->cmwk, old_pos );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������ʕ\����̓��͑҂�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @retval	"TRUE = ���͂��ꂽ"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNSEARCHUI_Result( ZKNSEARCHMAIN_WORK * wk )
{
	if( wk->loadingCnt == 90 ){
		wk->loadingCnt = 0;
		return TRUE;
	}
	wk->loadingCnt++;

	if( GFL_UI_TP_GetTrg() ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		wk->loadingCnt = 0;
		return TRUE;
	}

	if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		wk->loadingCnt = 0;
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g���͏���
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
int ZKNSEARCHUI_ListMain( ZKNSEARCHMAIN_WORK * wk )
{
	int	ret;

	ret = GFL_UI_TP_HitTrg( ListTouchHitTbl );

	if( ret != GFL_UI_TP_HIT_NONE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_RESET;
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g���͏����i�t�H�����p�j
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
int ZKNSEARCHUI_FormListMain( ZKNSEARCHMAIN_WORK * wk )
{
	int	ret;

	ret = GFL_UI_TP_HitTrg( FormListTouchHitTbl );

	if( ret != GFL_UI_TP_HIT_NONE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return ret;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_CANCEL;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return ZKNSEARCHUI_LIST_RESET;
	}

	return ret;
}
