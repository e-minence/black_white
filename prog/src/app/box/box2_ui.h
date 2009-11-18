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
	BOX2UI_INIT_ID_PTOUT_MAIN = 0,		// ��������E���C��
	BOX2UI_INIT_ID_PTOUT_BOXSEL,		// ��������E�{�b�N�X�I��

	BOX2UI_INIT_ID_PTIN_MAIN,			// ��Ă����E���C��

	BOX2UI_INIT_ID_ARRANGE_MAIN,		// �{�b�N�X�����E���C��
	BOX2UI_INIT_ID_ARRANGE_MOVE,		// �{�b�N�X�����E�|�P�����ړ�
	BOX2UI_INIT_ID_ARRANGE_PARTY,		// �{�b�N�X�����E�莝���|�P����

	BOX2UI_INIT_ID_ITEM_A_MAIN,		// �����������E���C��
	BOX2UI_INIT_ID_ITEM_A_PARTY,		// �����������E�莝���|�P����

	BOX2UI_INIT_ID_MARKING,			// �}�[�L���O
	BOX2UI_INIT_ID_BOXTHEMA_CHG,		// �{�b�N�X�e�[�}�ύX
	BOX2UI_INIT_ID_WALLPAPER_CHG,		// �ǎ��ύX
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
	BOX2UI_ARRANGE_MAIN_CLOSE,		// 41: �Ƃ���

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
	BOX2UI_ARRANGE_PGT_STTUS,		// 40: �X�e�[�^�X
	BOX2UI_ARRANGE_PGT_RET,			// 41: �߂�













	// �a����E���C������
	BOX2UI_PTOUT_MAIN_POKE1 = 0,	// �莝���|�P����
	BOX2UI_PTOUT_MAIN_POKE2,
	BOX2UI_PTOUT_MAIN_POKE3,
	BOX2UI_PTOUT_MAIN_POKE4,
	BOX2UI_PTOUT_MAIN_POKE5,
	BOX2UI_PTOUT_MAIN_POKE6,

	BOX2UI_PTOUT_MAIN_RETURN,		//�u���ǂ�v

	BOX2UI_PTOUT_MAIN_AZUKERU,		//�u��������v
	BOX2UI_PTOUT_MAIN_STATUS,		//�u�悤�����݂�v
	BOX2UI_PTOUT_MAIN_MARKING,		//�u�}�[�L���O�v
	BOX2UI_PTOUT_MAIN_FREE,		//�u�ɂ����v
	BOX2UI_PTOUT_MAIN_CLOSE,		//�u�Ƃ���v

	// �a����ꏊ�I��
	BOX2UI_PTOUT_BOXSEL_TRAY1 = 0,	// �g���C�A�C�R��
	BOX2UI_PTOUT_BOXSEL_TRAY2,		// �g���C�A�C�R��
	BOX2UI_PTOUT_BOXSEL_TRAY3,		// �g���C�A�C�R��
	BOX2UI_PTOUT_BOXSEL_TRAY4,		// �g���C�A�C�R��
	BOX2UI_PTOUT_BOXSEL_TRAY5,		// �g���C�A�C�R��
	BOX2UI_PTOUT_BOXSEL_TRAY6,		// �g���C�A�C�R��

	BOX2UI_PTOUT_BOXSEL_LEFT,		// �g���C���i���j
	BOX2UI_PTOUT_BOXSEL_RIGHT,		// �g���C���i�E�j

	BOX2UI_PTOUT_BOXSEL_ENTER,		//�u�|�P��������������v
	BOX2UI_PTOUT_BOXSEL_CANCEL,	//�u��߂�v

	// ��Ă����E���C������
	BOX2UI_PTIN_MAIN_POKE1 = 0,	// 00: �|�P����
	BOX2UI_PTIN_MAIN_NAME = BOX2UI_PTIN_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �{�b�N�X��
	BOX2UI_PTIN_MAIN_LEFT,			// 31: �g���C�؂�ւ����E��
	BOX2UI_PTIN_MAIN_RIGHT,		// 32: �g���C�؂�ւ����E�E
	BOX2UI_PTIN_MAIN_RETURN,		// 33: ���ǂ�

	BOX2UI_PTIN_MAIN_TSURETEIKU,	// 34: ��Ă���
	BOX2UI_PTIN_MAIN_STATUS,		// 35: �悤�����݂�
	BOX2UI_PTIN_MAIN_MARKING,		// 36: �}�[�L���O
	BOX2UI_PTIN_MAIN_FREE,			// 37: �ɂ���
	BOX2UI_PTIN_MAIN_CLOSE,		// 38: �Ƃ���

	// �{�b�N�X�����E�|�P�����ړ�
	BOX2UI_ARRANGE_MOVE_POKE1 = 0,	// 00: �|�P����
	BOX2UI_ARRANGE_MOVE_PARTY1 = BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �莝���|�P����
	BOX2UI_ARRANGE_MOVE_RETURN = BOX2UI_ARRANGE_MOVE_PARTY1 + BOX2OBJ_POKEICON_MINE_MAX,	// 36: ��߂�

	BOX2UI_ARRANGE_MOVE_TRAY1,		// 37: �g���C�A�C�R��
	BOX2UI_ARRANGE_MOVE_TRAY2,		// 38: �g���C�A�C�R��
	BOX2UI_ARRANGE_MOVE_TRAY3,		// 39: �g���C�A�C�R��
	BOX2UI_ARRANGE_MOVE_TRAY4,		// 40: �g���C�A�C�R��
	BOX2UI_ARRANGE_MOVE_TRAY5,		// 41: �g���C�A�C�R��
	BOX2UI_ARRANGE_MOVE_TRAY6,		// 42: �g���C�A�C�R��

	BOX2UI_ARRANGE_MOVE_LEFT,		// 43: �g���C���i���j
	BOX2UI_ARRANGE_MOVE_RIGHT,		// 44: �g���C���i�E�j

	BOX2UI_ARRANGE_MOVE_CHANGE,	// 45: �{�b�N�X�����肩����

	// �{�b�N�X�����E�莝���|�P����
	BOX2UI_ARRANGE_PARTY_POKE1 = 0,	// �莝���|�P����
	BOX2UI_ARRANGE_PARTY_POKE2,
	BOX2UI_ARRANGE_PARTY_POKE3,
	BOX2UI_ARRANGE_PARTY_POKE4,
	BOX2UI_ARRANGE_PARTY_POKE5,
	BOX2UI_ARRANGE_PARTY_POKE6,

	BOX2UI_ARRANGE_PARTY_CHANGE,		//�u���ꂩ���v
	BOX2UI_ARRANGE_PARTY_RETURN,		//�u���ǂ�v

	BOX2UI_ARRANGE_PARTY_MOVE,			//�u�|�P�������ǂ��v�i���j���[�j
	BOX2UI_ARRANGE_PARTY_STATUS,		//�u�悤�����݂�v
	BOX2UI_ARRANGE_PARTY_ITEM,			//�u�������́v
	BOX2UI_ARRANGE_PARTY_MARKING,		//�u�}�[�L���O�v
	BOX2UI_ARRANGE_PARTY_FREE,			//�u�ɂ����v

	BOX2UI_ARRANGE_PARTY_CLOSE,		//�u�Ƃ���v

	// �����������E���C������
	BOX2UI_ITEM_A_MAIN_POKE1 = 0,	// 00: �|�P����
	BOX2UI_ITEM_A_MAIN_NAME = BOX2UI_ITEM_A_MAIN_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX,	// 30: �{�b�N�X��
	BOX2UI_ITEM_A_MAIN_LEFT,		// 31: �g���C�؂�ւ����E��
	BOX2UI_ITEM_A_MAIN_RIGHT,		// 32: �g���C�؂�ւ����E�E
	BOX2UI_ITEM_A_MAIN_PARTY,		// 33: �莝���|�P����
	BOX2UI_ITEM_A_MAIN_CHANGE,		// 34: ����������
	BOX2UI_ITEM_A_MAIN_RETURN,		// 35: ���ǂ�

	BOX2UI_ITEM_A_MAIN_MENU1,		// 36: �o�b�O�� or ��������
	BOX2UI_ITEM_A_MAIN_CLOSE,		// 37: �Ƃ���

	// �����������E�莝���|�P����
	BOX2UI_ITEM_A_PARTY_POKE1 = 0,	// �莝���|�P����
	BOX2UI_ITEM_A_PARTY_POKE2,
	BOX2UI_ITEM_A_PARTY_POKE3,
	BOX2UI_ITEM_A_PARTY_POKE4,
	BOX2UI_ITEM_A_PARTY_POKE5,
	BOX2UI_ITEM_A_PARTY_POKE6,

	BOX2UI_ITEM_A_PARTY_CHANGE,	//�u���ꂩ���v
	BOX2UI_ITEM_A_PARTY_RETURN,	//�u���ǂ�v

	BOX2UI_ITEM_A_PARTY_MENU1,		// �o�b�O�� or ��������
	BOX2UI_ITEM_A_PARTY_CLOSE,		//�u�Ƃ���v

	// �}�[�L���O
	BOX2UI_MARKING_MARK1 = 0,		// 00: ��
	BOX2UI_MARKING_MARK2,			// 01: ��
	BOX2UI_MARKING_MARK3,			// 02: ��
	BOX2UI_MARKING_MARK4,			// 03: �n�[�g
	BOX2UI_MARKING_MARK5,			// 04: ��
	BOX2UI_MARKING_MARK6,			// 05: ��
	BOX2UI_MARKING_ENTER,			// 06:�u�����Ă��v
	BOX2UI_MARKING_CANCEL,			// 07:�u��߂�v

	// �{�b�N�X�e�[�}�ύX
	BOX2UI_BOXTHEMA_CHG_TRAY1 = 0,		// 00: �g���C�A�C�R��
	BOX2UI_BOXTHEMA_CHG_TRAY2,			// 01: �g���C�A�C�R��
	BOX2UI_BOXTHEMA_CHG_TRAY3,			// 02: �g���C�A�C�R��
	BOX2UI_BOXTHEMA_CHG_TRAY4,			// 03: �g���C�A�C�R��
	BOX2UI_BOXTHEMA_CHG_TRAY5,			// 04: �g���C�A�C�R��
	BOX2UI_BOXTHEMA_CHG_TRAY6,			// 05: �g���C�A�C�R��

	BOX2UI_BOXTHEMA_CHG_LEFT,			// 06: �g���C���i���j
	BOX2UI_BOXTHEMA_CHG_RIGHT,			// 07: �g���C���i�E�j

	BOX2UI_BOXTHEMA_CHG_MOVE,			// 08: �{�b�N�X�����肩����
	BOX2UI_BOXTHEMA_CHG_WP,			// 09: ���ׂ���
	BOX2UI_BOXTHEMA_CHG_NANE,			// 10: �Ȃ܂�
	BOX2UI_BOXTHEMA_CHG_CANCEL,		// 11: ��߂�

	// �ǎ��ύX
	BOX2UI_WALLPAPER_CHG_ICON1 = 0,	// 00: �ǎ��A�C�R��
	BOX2UI_WALLPAPER_CHG_ICON2,		// 01: �ǎ��A�C�R��
	BOX2UI_WALLPAPER_CHG_ICON3,		// 02: �ǎ��A�C�R��
	BOX2UI_WALLPAPER_CHG_ICON4,		// 03: �ǎ��A�C�R��

	BOX2UI_WALLPAPER_CHG_LEFT,			// 04: �g���C���i���j
	BOX2UI_WALLPAPER_CHG_RIGHT,		// 05: �g���C���i�E�j

	BOX2UI_WALLPAPER_CHG_ENTER,		// 06: �ύX
	BOX2UI_WALLPAPER_CHG_CANCEL,		// 07: �L�����Z��
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ����[�N������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ����[�N�؂�ւ�
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 * @param	id		�J�[�\���ړ��f�[�^�ԍ�
 * @param	pos		�J�[�\�������ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveChange( BOX2_SYS_WORK * syswk, u32 id, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ����[�N�폜
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������������v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_PartyOutMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�|�P��������Ă����v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_PartyInMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxArrangeMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ�����
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxArrangePokeMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�|�P�����ړ����ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_ArrangePokeGetMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�{�b�N�X�������肷��v�莝���|�P��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxArrangePartyMoveMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v���C������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxItemArrangeMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v�莝���|�P��������
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxItemArrangePartyMoveMain( BOX2_SYS_WORK * syswk );


//--------------------------------------------------------------------------------------------
/**
 * �u�������̂������肷��v�������������ɃA�C�R����͂�ł���Ƃ��̑���
 *
 * @param	syswk		�{�b�N�X��ʃV�X�e�����[�N
 *
 * @return	���쌋��
 *
 *	���{�b�N�X�����E�|�P�����ړ��̃��[�N���g�p
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_BoxItemArrangeMoveHand( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ���VBLANK�ōs�����߂̃��[�N�ݒ�
 *
 * @param	appwk		�{�b�N�X��ʃA�v�����[�N
 * @param	now_pos		���݂̈ʒu
 * @param	old_pos		�O��̈ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2UI_CursorMoveVFuncWorkSet( BOX2_APP_WORK * appwk, int now_pos, int old_pos );

//--------------------------------------------------------------------------------------------
/**
 * �x�X�e�[�^�X�{�^���`�F�b�N
 *
 * @param	syswk	�{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval	"TRUE = �N����"
 * @retval	"FALSE = �N���s��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2UI_YStatusButtonCheck( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N�F�g���C�̃|�P����
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = �G���Ă��Ȃ�
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitTrayPokeTrg(void);

//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N�F�莝���|�P�����i���j
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = �G���Ă��Ȃ�
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckTrgPartyPokeLeft(void);

//--------------------------------------------------------------------------------------------
/**
 * �^�b�`�`�F�b�N�F�莝���|�P�����i�E�j
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = �G���Ă��Ȃ�
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2UI_HitCheckTrgPartyPokeRight(void);


extern u32 BOX2UI_HitCheckContTrayArrow(void);


extern BOOL BOX2UI_HitCheckTrayScroll( u32 * x, u32 * y );

