//============================================================================================
/**
 * @file		zknlist_ui.h
 * @brief		�}�Ӄ��X�g��� �C���^�[�t�F�C�X�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTUI
 */
//============================================================================================
#pragma	once

enum {
	ZKNLISTUI_ID_LIST = 0,	// 00: ���X�g
	ZKNLISTUI_ID_RAIL,			// 01: ���[��

	ZKNLISTUI_ID_POKE,			// 02: �|�P�������ʊG

	ZKNLISTUI_ID_ICON1,			// 03: �|�P�����A�C�R��
	ZKNLISTUI_ID_ICON2,			// 04: �|�P�����A�C�R��
	ZKNLISTUI_ID_ICON3,			// 05: �|�P�����A�C�R��
	ZKNLISTUI_ID_ICON4,			// 06: �|�P�����A�C�R��
	ZKNLISTUI_ID_ICON5,			// 07: �|�P�����A�C�R��
	ZKNLISTUI_ID_ICON6,			// 08: �|�P�����A�C�R��
	ZKNLISTUI_ID_ICON7,			// 09: �|�P�����A�C�R��

	ZKNLISTUI_ID_START,			// 10: �X�^�[�g
	ZKNLISTUI_ID_SELECT,		// 11: �Z���N�g
	ZKNLISTUI_ID_LEFT,			// 12: ��
	ZKNLISTUI_ID_RIGHT,			// 13: �E
	ZKNLISTUI_ID_Y,					// 14: �x
	ZKNLISTUI_ID_X,					// 15: �w
	ZKNLISTUI_ID_RETURN,		// 16: �߂�

	ZKNLISTUI_ID_LIST_UP,		// ��L�[
	ZKNLISTUI_ID_LIST_DOWN,	// ���L�[
};



extern int ZKNLISUI_ListMain( ZKNLISTMAIN_WORK * wk );


extern BOOL ZKNLISTUI_CheckRailHit( u32 * x, u32 * y );
