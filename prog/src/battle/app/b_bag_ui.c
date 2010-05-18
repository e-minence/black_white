//============================================================================================
/**
 * @file		b_bag_ui.c
 * @brief		�퓬�p�o�b�O��� �C���^�[�t�F�[�X�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.09.25
 */
//============================================================================================
#include <gflib.h>

#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_ui.h"


//============================================================================================
//	�萔��`
//============================================================================================


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void VanishCursor( BBAG_WORK * wk, BOOL flg );
static void MoveCursor( BBAG_WORK * wk, int pos );

static void CallBack_On( void * work, int nowPos, int oldPos );
static void CallBack_Off( void * work, int nowPos, int oldPos );
static void CallBack_Touch( void * work, int nowPos, int oldPos );
static void CallBack_Move( void * work, int nowPos, int oldPos );
static void CallBack_OnItemList( void * work, int nowPos, int oldPos );


//============================================================================================
//	�O���[�o��
//============================================================================================

// �|�P�b�g�I����ʈړ��e�[�u��
static const CURSORMOVE_DATA P1_CursorPosTbl[] =
{
	{  64,  42, 112+20,  44+16, 0, 1, 0, 2, {  1*8,  9*8-1,  0*8, 16*8-1 } },					// 0 : HP�񕜃|�P�b�g
	{  64, 114, 112+20,  44+16, 0, 4, 1, 3, { 10*8, 18*8-1,  0*8, 16*8-1 } },					// 1 : ��ԉ񕜃|�P�b�g
	{ 192,  42, 112+20,  44+16, 2, 3, 0, 2, {  1*8,  9*8-1, 16*8, 32*8-1 } },					// 2 : �{�[���|�P�b�g
	{ 192, 114, 112+20,  44+16, 2, 5, 1, 3, { 10*8, 18*8-1, 16*8, 32*8-1 } },					// 3 : �퓬�p�|�P�b�g
	{ 108, 176, 184+20,  24+16, 1, 4, 4, 5, { 19*8, 24*8-1,  1*8, 26*8-1 } },					// 4 : �Ō�Ɏg�p��������
	{ 240, 174,  32+20,  28+16, CURSORMOVE_RETBIT|3, 5, 4, 5, { 19*8, 24*8-1, 27*8, 32*8-1 } },	// 5 : �߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK P1_CallBack = {
	CallBack_On,
	CallBack_Off,
	CallBack_Move,
	CallBack_Touch,
};

// �A�C�e���I����ʈړ��e�[�u��
static const CURSORMOVE_DATA P2_CursorPosTbl[] =
{
	{  64,  32, 112+20,  38+16, 0, 2, 0, 1, {  1*8,  7*8-1,  0*8, 16*8-1 } },					// 0 : �A�C�e���P
	{ 192,  32, 112+20,  38+16, 1, 3, 0, 1, {  1*8,  7*8-1, 16*8, 32*8-1 } },					// 1 : �A�C�e���Q
	{  64,  80, 112+20,  38+16, 0, 4, 2, 3, {  7*8, 13*8-1,  0*8, 16*8-1 } },					// 2 : �A�C�e���R
	{ 192,  80, 112+20,  38+16, 1, 5, 2, 3, {  7*8, 13*8-1, 16*8, 32*8-1 } },					// 3 : �A�C�e���S
	{  64, 128, 112+20,  38+16, 2, 6, 4, 5, { 13*8, 19*8-1,  0*8, 16*8-1 } },					// 4 : �A�C�e���T
	{ 192, 128, 112+20,  38+16, 3, 6, 4, 5, { 13*8, 19*8-1, 16*8, 32*8-1 } },					// 5 : �A�C�e���U
	{ 240, 174,  32+20,  28+16, CURSORMOVE_RETBIT|5, 6, 6, 6, { 19*8, 24*8-1, 27*8, 32*8-1 } },	// 6 : �߂�
	{ 0, 0, 0+20, 0+16, 7, 7, 7, 7, { 19*8, 24*8-1,  0*8,  5*8-1 } },	// 7 : �O��
	{ 0, 0, 0+20, 0+16, 8, 8, 8, 8, { 19*8, 24*8-1,  5*8, 10*8-1 } },	// 8 : ����

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK P2_CallBack = {
	CallBack_OnItemList,
	CallBack_Off,
	CallBack_Move,
	CallBack_Touch,
};

// �A�C�e���I����ʈړ��e�[�u��
static const CURSORMOVE_DATA P3_CursorPosTbl[] =
{
	{ 108, 176, 184+20,  24+16, 0, 0, 0, 1, { 19*8, 24*8-1,  1*8, 26*8-1 } },	// 0 : �g��
	{ 240, 174,  32+20,  28+16, 1, 1, 0, 1, { 19*8, 24*8-1, 27*8, 32*8-1 } },	// 1 : �߂�

	{ 0, 0, 0, 0,	0, 0, 0, 0,	{ GFL_UI_TP_HIT_END, 0, 0, 0 } }
};
static const CURSORMOVE_CALLBACK P3_CallBack = {
	CallBack_On,
	CallBack_Off,
	CallBack_Move,
	CallBack_Touch,
};

// �J�[�\���ړ��f�[�^�e�[�u��
static const CURSORMOVE_DATA * const PointTable[] = {
	P1_CursorPosTbl,
	P2_CursorPosTbl,
	P3_CursorPosTbl,
};

// �R�[���o�b�N�e�[�u��
static const CURSORMOVE_CALLBACK * const CallBackTable[] = {
	&P1_CallBack,
	&P2_CallBack,
	&P3_CallBack,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ�������
 *
 * @param		wk		���[�N
 * @param		pos		�����ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGUI_Init( BBAG_WORK * wk, u32 page, u32 pos )
{
	wk->cmwk = CURSORMOVE_Create(
								PointTable[page],
								CallBackTable[page],
								wk,
								wk->cursor_flg,
								pos,
								wk->dat->heap );
	CURSORMOVE_VanishModeSet( wk->cmwk );
	MoveCursor( wk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ��폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGUI_Exit( BBAG_WORK * wk )
{
	CURSORMOVE_Exit( wk->cmwk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ��؂�ւ�
 *
 * @param		wk		���[�N
 * @param		page	�y�[�W
 * @param		pos		�����ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGUI_ChangePage( BBAG_WORK * wk, u32 page, u32 pos )
{
	if( wk->cursor_flg == FALSE ){
		pos = 0;
	}else{
		switch( page ){
		case BBAG_PAGE_POCKET:		// �|�P�b�g�I���y�[�W
			pos = wk->poke_id;
			break;
		case BBAG_PAGE_MAIN:			// �A�C�e���I���y�[�W
			pos = wk->dat->item_pos[wk->poke_id];
			break;
		case BBAG_PAGE_ITEM:			// �A�C�e���g�p�y�[�W
			pos = 0;
			break;
		}
	}

	BBAGUI_Exit( wk );
	BBAGUI_Init( wk, page, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���\���؂�ւ�
 *
 * @param		wk		���[�N
 * @param		flg		TRUE = �\��, FALSE = ��\��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VanishCursor( BBAG_WORK * wk, BOOL flg )
{
	wk->cursor_flg = flg;
	BAPPTOOL_VanishCursor( wk->cpwk, flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ�
 *
 * @param		wk		���[�N
 * @param		pos		�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MoveCursor( BBAG_WORK * wk, int pos )
{
	const CURSORMOVE_DATA *	dat = CURSORMOVE_GetMoveData( wk->cmwk, pos );
	BAPPTOOL_MoveCursorPoint( wk->cpwk, dat );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\���\��
 *
 * @param		work		���[�N
 * @param		nowPos	���݂̈ʒu
 * @param		oldPos	�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CallBack_On( void * work, int nowPos, int oldPos )
{
	BBAG_WORK * wk = work;

	MoveCursor( wk, nowPos );
	VanishCursor( wk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\����\��
 *
 * @param		work		���[�N
 * @param		nowPos	���݂̈ʒu
 * @param		oldPos	�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CallBack_Off( void * work, int nowPos, int oldPos )
{
	BBAG_WORK * wk = work;

	VanishCursor( wk, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�^�b�`
 *
 * @param		work		���[�N
 * @param		nowPos	���݂̈ʒu
 * @param		oldPos	�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CallBack_Touch( void * work, int nowPos, int oldPos )
{
	BBAG_WORK * wk = work;

	VanishCursor( wk, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\���ړ�
 *
 * @param		work		���[�N
 * @param		nowPos	���݂̈ʒu
 * @param		oldPos	�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CallBack_Move( void * work, int nowPos, int oldPos )
{
	BBAG_WORK * wk = work;

	MoveCursor( wk, nowPos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R�[���o�b�N�F�J�[�\���\���i�A�C�e���I���y�[�W�j
 *
 * @param		work		���[�N
 * @param		nowPos	���݂̈ʒu
 * @param		oldPos	�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CallBack_OnItemList( void * work, int nowPos, int oldPos )
{
	BBAG_WORK * wk = work;

	if( nowPos == BBAG_UI_P2_LEFT || nowPos == BBAG_UI_P2_RIGHT ){
		nowPos = 	BBAG_UI_P2_ITEM1;
		CURSORMOVE_PosSet( wk->cmwk, nowPos );
	}

	MoveCursor( wk, nowPos );
	VanishCursor( wk, TRUE );
}
