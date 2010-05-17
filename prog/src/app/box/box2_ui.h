//============================================================================================
/**
 * @file		box2_ui.h
 * @brief		�{�b�N�X��� �L�[�^�b�`�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	���W���[�����FBOX2_UI
 */
//============================================================================================
#pragma once


//============================================================================================
//	�萔��`
//============================================================================================

// �������h�c
enum {
	BOX2UI_INIT_ID_PTOUT_MAIN = 0,			// ��������E���C��
	BOX2UI_INIT_ID_PTOUT_BOXSEL,				// ��������E�{�b�N�X�I��

	BOX2UI_INIT_ID_PTIN_MAIN,						// ��Ă����E���C��

	BOX2UI_INIT_ID_ARRANGE_MAIN,				// �{�b�N�X�����E���C��
	BOX2UI_INIT_ID_ARRANGE_MOVE,				// �{�b�N�X�����E�|�P�����ړ��i�{�b�N�X�ԁj
	BOX2UI_INIT_ID_ARRANGE_PARTY,				// �{�b�N�X�����E�莝���|�P����
	BOX2UI_INIT_ID_ARRANGE_PARTY_MOVE,	// �{�b�N�X�����E�|�P�����ړ��i�莝���j

	BOX2UI_INIT_ID_ITEM_MAIN,						// �����������E���C��
	BOX2UI_INIT_ID_ITEM_PARTY,					// �����������E�莝���|�P����

	BOX2UI_INIT_ID_MARKING,							// �}�[�L���O

	BOX2UI_INIT_ID_BOXTHEMA_CHG,				// �{�b�N�X�e�[�}�ύX

	BOX2UI_INIT_ID_WALLPAPER_CHG,				// �ǎ��ύX

	BOX2UI_INIT_ID_BOXJUMP_MAIN,				// �{�b�N�X�W�����v

	BOX2UI_INIT_ID_BATTLEBOX_MAIN,			// �o�g���{�b�N�X�E���C��
	BOX2UI_INIT_ID_BATTLEBOX_PARTY,			// �o�g���{�b�N�X�E�p�[�e�B���상�C��

	BOX2UI_INIT_ID_SLEEP_MAIN,					// �Q������E���C��
};

// �{�^���h�c
enum {
	// �{�b�N�X�����E���C������
	BOX2UI_ARRANGE_MAIN_POKE1 = 0,	// 00: �|�P����
	BOX2UI_ARRANGE_MAIN_NAME = BOX2UI_ARRANGE_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �{�b�N�X��
	BOX2UI_ARRANGE_MAIN_LEFT,			// 31: �g���C�؂�ւ����E��
	BOX2UI_ARRANGE_MAIN_RIGHT,		// 32: �g���C�؂�ւ����E�E
	BOX2UI_ARRANGE_MAIN_PARTY,		// 33: �莝���|�P����
	BOX2UI_ARRANGE_MAIN_RETURN1,	// 34: �߂�P
	BOX2UI_ARRANGE_MAIN_RETURN2,	// 35: �߂�Q

	BOX2UI_ARRANGE_MAIN_GET,			// 36: ����
	BOX2UI_ARRANGE_MAIN_STATUS,		// 37: �悤�����݂�
	BOX2UI_ARRANGE_MAIN_ITEM,			// 38: ��������
	BOX2UI_ARRANGE_MAIN_MARKING,	// 39: �}�[�L���O
	BOX2UI_ARRANGE_MAIN_FREE,			// 40: �ɂ���
	BOX2UI_ARRANGE_MAIN_CLOSE,		// 41: ��߂�

	BOX2UI_ARRANGE_MAIN_CONV,			// 42: �֗����[�h

	// �{�b�N�X�����E�u���ށv�g���C�ړ�
	BOX2UI_ARRANGE_PGT_POKE1 = 0,	// 00: �|�P����
	BOX2UI_ARRANGE_PGT_NAME = BOX2UI_ARRANGE_PGT_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,		// 30: �{�b�N�X��
	BOX2UI_ARRANGE_PGT_LEFT,		// 31: �{�b�N�X�؂�ւ����i���j
	BOX2UI_ARRANGE_PGT_RIGHT,		// 32: �{�b�N�X�؂�ւ����i�E�j

	BOX2UI_ARRANGE_PGT_TRAY1,		// 33: �g���C�A�C�R��
	BOX2UI_ARRANGE_PGT_TRAY2,		// 34: �g���C�A�C�R��
	BOX2UI_ARRANGE_PGT_TRAY3,		// 35: �g���C�A�C�R��
	BOX2UI_ARRANGE_PGT_TRAY4,		// 36: �g���C�A�C�R��
	BOX2UI_ARRANGE_PGT_TRAY5,		// 37: �g���C�A�C�R��
	BOX2UI_ARRANGE_PGT_TRAY6,		// 38: �g���C�A�C�R��

	BOX2UI_ARRANGE_PGT_PARTY,		// 39: �莝���|�P����
	BOX2UI_ARRANGE_PGT_STATUS,	// 40: �X�e�[�^�X
	BOX2UI_ARRANGE_PGT_RET,			// 41: �߂�

	BOX2UI_ARRANGE_PGT_CONV,		// 42: �֗����[�h

	// �{�b�N�X�����E�莝���|�P����
	BOX2UI_ARRANGE_PARTY_POKE1 = 0,	// 00: �莝���|�P����
	BOX2UI_ARRANGE_PARTY_POKE2,
	BOX2UI_ARRANGE_PARTY_POKE3,
	BOX2UI_ARRANGE_PARTY_POKE4,
	BOX2UI_ARRANGE_PARTY_POKE5,
	BOX2UI_ARRANGE_PARTY_POKE6,

	BOX2UI_ARRANGE_PARTY_BOXLIST,		// 06:�u�{�b�N�X���X�g�v
	BOX2UI_ARRANGE_PARTY_RETURN1,		// 07: �߂�P
	BOX2UI_ARRANGE_PARTY_RETURN2,		// 08: �߂�Q

	BOX2UI_ARRANGE_PARTY_GET,				// 09: ����
	BOX2UI_ARRANGE_PARTY_STATUS,		// 10: �悤�����݂�
	BOX2UI_ARRANGE_PARTY_ITEM,			// 11: ��������
	BOX2UI_ARRANGE_PARTY_MARKING,		// 12: �}�[�L���O
	BOX2UI_ARRANGE_PARTY_FREE,			// 13: �ɂ���
	BOX2UI_ARRANGE_PARTY_CLOSE,			// 14: ��߂�

	BOX2UI_ARRANGE_PARTY_CONV,			// 15: �֗����[�h

	// �{�b�N�X�����E�u���ށv�g���C�ړ��i�莝���j
	BOX2UI_ARRANGE_PTGT_POKE1 = 0,	// 00: �|�P����
	BOX2UI_ARRANGE_PTGT_PARTY_POKE = BOX2UI_ARRANGE_PTGT_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �莝���|�P����
	BOX2UI_ARRANGE_PTGT_NAME = BOX2UI_ARRANGE_PTGT_PARTY_POKE + BOX2OBJ_POKEICON_MINE_MAX,	// 36: �{�b�N�X��
	BOX2UI_ARRANGE_PTGT_LEFT,			// 37: �{�b�N�X�؂�ւ����i���j
	BOX2UI_ARRANGE_PTGT_RIGHT,		// 38: �{�b�N�X�؂�ւ����i�E�j
	BOX2UI_ARRANGE_PTGT_BOXLIST,	// 39: �{�b�N�X���X�g
	BOX2UI_ARRANGE_PTGT_STATUS,		// 40: �X�e�[�^�X
	BOX2UI_ARRANGE_PTGT_RET,			// 41: �߂�

	BOX2UI_ARRANGE_PTGT_CONV,			// 42: �֗����[�h

	// �o�g���{�b�N�X�E���C������
	BOX2UI_BATTLEBOX_MAIN_POKE1 = 0,	// 00: �|�P����
	BOX2UI_BATTLEBOX_MAIN_NAME = BOX2UI_BATTLEBOX_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �{�b�N�X��
	BOX2UI_BATTLEBOX_MAIN_LEFT,			// 31: �g���C�؂�ւ����E��
	BOX2UI_BATTLEBOX_MAIN_RIGHT,		// 32: �g���C�؂�ւ����E�E
	BOX2UI_BATTLEBOX_MAIN_PARTY,		// 33: �o�g���{�b�N�X
	BOX2UI_BATTLEBOX_MAIN_RETURN1,	// 34: �߂�P
	BOX2UI_BATTLEBOX_MAIN_RETURN2,	// 35: �߂�Q

	BOX2UI_BATTLEBOX_MAIN_GET,			// 36: ���ǂ�����
	BOX2UI_BATTLEBOX_MAIN_STATUS,		// 37: �悤�����݂�
	BOX2UI_BATTLEBOX_MAIN_ITEM,			// 38: ��������
	BOX2UI_BATTLEBOX_MAIN_CLOSE,		// 39: ��߂�

	BOX2UI_BATTLEBOX_MAIN_CONV,			// 40: �֗����[�h

	// �o�g���{�b�N�X�E�p�[�e�B����
	BOX2UI_BATTLEBOX_PARTY_POKE1 = 0,	// 00: �莝���|�P����
	BOX2UI_BATTLEBOX_PARTY_POKE2,
	BOX2UI_BATTLEBOX_PARTY_POKE3,
	BOX2UI_BATTLEBOX_PARTY_POKE4,
	BOX2UI_BATTLEBOX_PARTY_POKE5,
	BOX2UI_BATTLEBOX_PARTY_POKE6,

	BOX2UI_BATTLEBOX_PARTY_BOXLIST,		// 06:�u�{�b�N�X���X�g�v
	BOX2UI_BATTLEBOX_PARTY_RETURN1,		// 07: �߂�P
	BOX2UI_BATTLEBOX_PARTY_RETURN2,		// 08: �߂�Q

	BOX2UI_BATTLEBOX_PARTY_GET,				// 09: ���ǂ�����
	BOX2UI_BATTLEBOX_PARTY_STATUS,		// 10: �悤�����݂�
	BOX2UI_BATTLEBOX_PARTY_ITEM,			// 11: ��������
	BOX2UI_BATTLEBOX_PARTY_CLOSE,			// 12: ��߂�

	BOX2UI_BATTLEBOX_PARTY_CONV,			// 13: �֗����[�h

	// ��Ă����E���C������
	BOX2UI_PTIN_MAIN_POKE1 = 0,		// 00: �|�P����
	BOX2UI_PTIN_MAIN_NAME = BOX2UI_PTIN_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �{�b�N�X��
	BOX2UI_PTIN_MAIN_LEFT,				// 31: �g���C�؂�ւ����E��
	BOX2UI_PTIN_MAIN_RIGHT,				// 32: �g���C�؂�ւ����E�E
	BOX2UI_PTIN_MAIN_RETURN1,			// 33: �߂�P
	BOX2UI_PTIN_MAIN_RETURN2,			// 34: �߂�Q

	BOX2UI_PTIN_MAIN_TSURETEIKU,	// 35: ��Ă���
	BOX2UI_PTIN_MAIN_STATUS,			// 36: �悤�����݂�
	BOX2UI_PTIN_MAIN_MARKING,			// 37: �}�[�L���O
	BOX2UI_PTIN_MAIN_FREE,				// 38: �ɂ���
	BOX2UI_PTIN_MAIN_CLOSE,				// 39: �Ƃ���

	// �a����E���C������
	BOX2UI_PTOUT_MAIN_POKE1 = 0,	// 00: �莝���|�P����
	BOX2UI_PTOUT_MAIN_POKE2,
	BOX2UI_PTOUT_MAIN_POKE3,
	BOX2UI_PTOUT_MAIN_POKE4,
	BOX2UI_PTOUT_MAIN_POKE5,
	BOX2UI_PTOUT_MAIN_POKE6,

	BOX2UI_PTOUT_MAIN_RETURN1,		// 06: �߂�P
	BOX2UI_PTOUT_MAIN_RETURN2,		// 07: �߂�Q

	BOX2UI_PTOUT_MAIN_AZUKERU,		// 08: ��������
	BOX2UI_PTOUT_MAIN_STATUS,			// 09: �悤�����݂�
	BOX2UI_PTOUT_MAIN_MARKING,		// 10: �}�[�L���O
	BOX2UI_PTOUT_MAIN_FREE,				// 11: �ɂ���
	BOX2UI_PTOUT_MAIN_CLOSE,			// 12: �Ƃ���

	// �a����ꏊ�I��
	BOX2UI_PTOUT_BOXSEL_NAME = 0,	// 00: �g���C��
	BOX2UI_PTOUT_BOXSEL_TRAY,			// 01: �g���C
	BOX2UI_PTOUT_BOXSEL_LEFT,			// 02: �g���C�؂�ւ����E��
	BOX2UI_PTOUT_BOXSEL_RIGHT,		// 03: �g���C�؂�ւ����E�E
	BOX2UI_PTOUT_BOXSEL_RETURN,		// 04: �߂�


	// �����������E���C������
	BOX2UI_ITEM_MAIN_POKE1 = 0,		// 00: �|�P����
	BOX2UI_ITEM_MAIN_NAME = BOX2UI_ITEM_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,		// 30: �{�b�N�X��
	BOX2UI_ITEM_MAIN_LEFT,				// 31: �g���C�؂�ւ����E��
	BOX2UI_ITEM_MAIN_RIGHT,				// 32: �g���C�؂�ւ����E�E
	BOX2UI_ITEM_MAIN_PARTY,				// 33: �莝���|�P����
	BOX2UI_ITEM_MAIN_RETURN1,			// 34: �߂�P
	BOX2UI_ITEM_MAIN_RETURN2,			// 35: �߂�Q

	BOX2UI_ITEM_MAIN_MENU1,				// 36: ���ǂ�����
	BOX2UI_ITEM_MAIN_MENU2,				// 37: �o�b�O�� or ��������
	BOX2UI_ITEM_MAIN_CLOSE,				// 38: �Ƃ���

	BOX2UI_ITEM_MAIN_CONV,				// 39: �֗����[�h

	// �����������E�莝���|�P����
	BOX2UI_ITEM_PARTY_POKE1 = 0,	// 00: �莝���|�P����
	BOX2UI_ITEM_PARTY_POKE2,
	BOX2UI_ITEM_PARTY_POKE3,
	BOX2UI_ITEM_PARTY_POKE4,
	BOX2UI_ITEM_PARTY_POKE5,
	BOX2UI_ITEM_PARTY_POKE6,

	BOX2UI_ITEM_PARTY_BOXLIST,		// 06: �{�b�N�X���X�g
	BOX2UI_ITEM_PARTY_RETURN1,		// 07: �߂�P
	BOX2UI_ITEM_PARTY_RETURN2,		// 08: �߂�Q

	BOX2UI_ITEM_PARTY_MENU1,			// 09: ���ǂ�����
	BOX2UI_ITEM_PARTY_MENU2,			// 10: �o�b�O�� or ��������
	BOX2UI_ITEM_PARTY_CLOSE,			// 11: �Ƃ���

	BOX2UI_ITEM_PARTY_CONV,				// 12: �֗����[�h

	// �Q������
	BOX2UI_SLEEP_MAIN_POKE1 = 0,	// 00: �|�P����
	BOX2UI_SLEEP_MAIN_NAME = BOX2UI_SLEEP_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �{�b�N�X��
	BOX2UI_SLEEP_MAIN_LEFT,			// 31: �g���C�؂�ւ����E��
	BOX2UI_SLEEP_MAIN_RIGHT,		// 32: �g���C�؂�ւ����E�E
	BOX2UI_SLEEP_MAIN_RETURN,		// 33: �߂�

	BOX2UI_SLEEP_MAIN_SET,			// 34: �˂�����
	BOX2UI_SLEEP_MAIN_CLOSE,		// 35: ��߂�

	// �{�b�N�X�e�[�}�ύX
	BOX2UI_BOXTHEMA_MENU1 = 0,		// 00: �W�����v����
	BOX2UI_BOXTHEMA_MENU2,				// 01: ���ׂ���
	BOX2UI_BOXTHEMA_MENU3,				// 02: �Ȃ܂�
	BOX2UI_BOXTHEMA_MENU4,				// 03: ��߂�

	// �{�b�N�X�W�����v
	BOX2UI_BOXJUMP_TRAY1 = 0,		// 00: �g���C�A�C�R��
	BOX2UI_BOXJUMP_TRAY2,				// 01: �g���C�A�C�R��
	BOX2UI_BOXJUMP_TRAY3,				// 02: �g���C�A�C�R��
	BOX2UI_BOXJUMP_TRAY4,				// 03: �g���C�A�C�R��
	BOX2UI_BOXJUMP_TRAY5,				// 04: �g���C�A�C�R��
	BOX2UI_BOXJUMP_TRAY6,				// 05: �g���C�A�C�R��
	BOX2UI_BOXJUMP_RET,					// 06: �߂�

	// �ǎ��I��
	BOX2UI_WALLPAPER_MENU1 = 0,		// 00: ���j���[�P
	BOX2UI_WALLPAPER_MENU2,				// 01: ���j���[�Q
	BOX2UI_WALLPAPER_MENU3,				// 02: ���j���[�R
	BOX2UI_WALLPAPER_MENU4,				// 03: ���j���[�S
	BOX2UI_WALLPAPER_MENU5,				// 04: ���j���[�T
	BOX2UI_WALLPAPER_MENU6,				// 05: ���ǂ�

	// �}�[�L���O
	BOX2UI_MARKING_MARK1 = 0,		// 00: ��
	BOX2UI_MARKING_MARK2,				// 01: ��
	BOX2UI_MARKING_MARK3,				// 02: ��
	BOX2UI_MARKING_MARK4,				// 03: �n�[�g
	BOX2UI_MARKING_MARK5,				// 04: ��
	BOX2UI_MARKING_MARK6,				// 05: ��
	BOX2UI_MARKING_ENTER,				// 06:�u�����Ă��v
	BOX2UI_MARKING_CANCEL,			// 07:�u��߂�v
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ����[�N������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ����[�N�؂�ւ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		id			�J�[�\���ړ��f�[�^�ԍ�
 * @param		pos			�J�[�\�������ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveChange( BOX2_SYS_WORK * syswk, u32 id, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ����[�N�폜
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�w��ʒu�ɃJ�[�\���n�a�i��z�u
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_PutHandCursor( BOX2_SYS_WORK * syswk, int pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�|�P��������������v���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_PartyOutMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�|�P��������������v�g���C�I���F�w��ʒu����{�b�N�X���ւ̈ړ������Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_PartyOutBoxSelCursorMoveSet( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�|�P��������������v�g���C�I���F�{�b�N�X������w��ʒu�ւ̈ړ������Z�b�g
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_PartyOutBoxSelCursorMovePutSet( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�|�P��������Ă����v���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_PartyInMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�{�b�N�X�������肷��v���C������
 *
 * @param		syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxArrangeMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����擾���̃^�b�`�o�[�{�^���ݒ�
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 * @param		pos			���݂̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_SetTouchBarIconPokeGet( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�{�b�N�X�������肷��v�|�P�����ړ����ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_ArrangePokeGetMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�{�b�N�X�������肷��v�莝���|�P��������
 *
 * @param		syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxArrangePartyMoveMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�{�b�N�X�������肷��v�|�P�����ړ����ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_ArrangePartyPokeGetMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�������̂������肷��v���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxItemArrangeMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�������̂������肷��v�莝���|�P��������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxItemArrangePartyMoveMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�o�g���{�b�N�X�v���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BattleBoxMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�o�g���{�b�N�X�v�p�[�e�B�|�P��������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BattleBoxPartyMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�Q�����|�P����������ԁv���C������
 *
 * @param		syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_SleepMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ���VBLANK�ōs�����߂̃��[�N�ݒ�i�ʒu�w��j
 *
 * @param		appwk			�{�b�N�X��ʃA�v�����[�N
 * @param		now_pos		���݂̈ʒu
 * @param		old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveVFuncWorkSet( BOX2_APP_WORK * appwk, int now_pos, int old_pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ���VBLANK�ōs�����߂̃��[�N�ݒ�i���W�w��j
 *
 * @param		appwk		�{�b�N�X��ʃA�v�����[�N
 * @param		nx			�ړ��ʒu�w���W
 * @param		ny			�ړ��ʒu�x���W
 * @param		ox			�O��ʒu�w���W
 * @param		oy			�O��ʒu�x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveVFuncWorkSetDirect( BOX2_APP_WORK * appwk, u8 nx, u8 ny, u8 ox, u8 oy );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�g���C�̃|�P�����i�g���K�j
 *
 * @param		none
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckTrgTrayPoke(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�g���C�̃|�P�����i���W�w��j
 *
 * @param		x				�w���W
 * @param		y				�x���W
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckPosTrayPoke( u32 x, u32 y );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�莝���|�P�����i���j
 *
 * @param		none
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckTrgPartyPokeLeft(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�莝���|�P�����i�E�j
 *
 * @param		none
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckTrgPartyPokeRight(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�g���C�؂�ւ����
 *
 * @param		none
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckContTrayArrow(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�g���C�A�C�R���㉺�X�N���[��
 *
 * @param		none
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckContTrayIconScroll(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�g���C�A�C�R���X�N���[���J�n
 *
 * @param		x			�w���W�i�[�ꏊ
 * @param		y			�x���W�i�[�ꏊ
 *
 * @retval	"TRUE = �^�b�`����"
 * @retval	"FALSE = �^�b�`�Ȃ�"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2UI_HitCheckTrayScrollStart( u32 * x, u32 * y );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�g���C�A�C�R���X�N���[���p��
 *
 * @param		x			�w���W�i�[�ꏊ
 * @param		y			�x���W�i�[�ꏊ
 *
 * @retval	"TRUE = �^�b�`����"
 * @retval	"FALSE = �^�b�`�Ȃ�"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2UI_HitCheckTrayScroll( u32 * x, u32 * y );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�w����W�̃g���C�A�C�R�����擾
 *
 * @param		x			�w���W
 * @param		y			�x���W
 *
 * @retval	"BOX2MAIN_GETPOS_NONE = �g���C�A�C�R���Ȃ�"
 * @retval	"BOX2MAIN_GETPOS_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckTrayIcon( u32 x, u32 y );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�莝���t���[���i���j
 *
 * @param		none
 *
 * @retval	"TRUE = �^�b�`����"
 * @retval	"FALSE = �^�b�`�Ȃ�"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2UI_HitCheckPartyFrameLeft(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N�F�{�b�N�X/�莝���؂�ւ��{�^��
 *
 * @param		none
 *
 * @retval	"GFL_UI_TP_HIT_NONE = �^�b�`�Ȃ�"
 * @retval	"GFL_UI_TP_HIT_NONE != ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckModeChange(void);
