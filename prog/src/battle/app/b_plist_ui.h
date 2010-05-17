//============================================================================================
/**
 * @file		b_plist_ui.h
 * @brief		�퓬�p�|�P�������X�g��� �C���^�[�t�F�[�X�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.09.29
 */
//============================================================================================
#pragma once


//============================================================================================
//	�萔��`
//============================================================================================

// �{�^���h�c
enum {
	BPLIST_UI_LIST_POKE1 = 0,
	BPLIST_UI_LIST_POKE2,
	BPLIST_UI_LIST_POKE3,
	BPLIST_UI_LIST_POKE4,
	BPLIST_UI_LIST_POKE5,
	BPLIST_UI_LIST_POKE6,
	BPLIST_UI_LIST_RETURN,

	BPLIST_UI_CHG_ENTER = 0,	// ���ꂩ����
	BPLIST_UI_CHG_STATUS,			// �悳���݂�
	BPLIST_UI_CHG_WAZA,				// �킴���݂�
	BPLIST_UI_CHG_RETURN,			// ���ǂ�

	BPLIST_UI_STATUS_UP = 0,	// �O�̃|�P������
	BPLIST_UI_STATUS_DOWN,		// ���̃|�P������
	BPLIST_UI_STATUS_WAZA,		// �킴���݂�
	BPLIST_UI_STATUS_RETURN,	// ���ǂ�

	BPLIST_UI_WAZA_SEL1 = 0,	// �Z�P
	BPLIST_UI_WAZA_SEL2,			// �Z�Q
	BPLIST_UI_WAZA_SEL3,			// �Z�R
	BPLIST_UI_WAZA_SEL4,			// �Z�S
	BPLIST_UI_WAZA_UP,				// �O�̃|�P������
	BPLIST_UI_WAZA_DOWN,			// ���̃|�P������
	BPLIST_UI_WAZA_STATUS,		// �悳���݂�
	BPLIST_UI_WAZA_RETURN,		// ���ǂ�

	BPLIST_UI_WAZAINFO_SEL1 = 0,	// �Z�P
	BPLIST_UI_WAZAINFO_SEL2,			// �Z�Q
	BPLIST_UI_WAZAINFO_SEL3,			// �Z�R
	BPLIST_UI_WAZAINFO_SEL4,			// �Z�S
	BPLIST_UI_WAZAINFO_RETURN,		// ���ǂ�

	BPLIST_UI_PPRCV_WAZA1 = 0,	// �Z�P
	BPLIST_UI_PPRCV_WAZA2,			// �Z�Q
	BPLIST_UI_PPRCV_WAZA3,			// �Z�R
	BPLIST_UI_PPRCV_WAZA4,			// �Z�S
	BPLIST_UI_PPRCV_RETURN,			// ���ǂ�

	BPLIST_UI_WAZADEL_SEL1 = 0,	// �Z�P
	BPLIST_UI_WAZADEL_SEL2,			// �Z�Q
	BPLIST_UI_WAZADEL_SEL3,			// �Z�R
	BPLIST_UI_WAZADEL_SEL4,			// �Z�S
	BPLIST_UI_WAZADEL_SEL5,			// �Z�T
	BPLIST_UI_WAZADEL_RETURN,		// ���ǂ�

	BPLIST_UI_DELINFO_ENTER = 0,	// �킷���
	BPLIST_UI_DELINFO_RETURN,			// ���ǂ�
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ�������
 *
 * @param		wk    ���[�N
 * @param		page	�y�[�W
 * @param		pos		�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BPLISTUI_Init( BPLIST_WORK * wk, u32 page, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ��폜
 *
 * @param		wk    ���[�N
 * @param		page	�y�[�W
 * @param		pos		�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BPLISTUI_Exit( BPLIST_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ړ��؂�ւ�
 *
 * @param		wk    ���[�N
 * @param		page	�y�[�W
 * @param		pos		�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BPLISTUI_ChangePage( BPLIST_WORK * wk, u32 page, u32 pos );
