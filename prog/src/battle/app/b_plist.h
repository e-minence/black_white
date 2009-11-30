//============================================================================================
/**
 * @file	b_plist.h
 * @brief	�퓬�p�|�P�������X�g��ʁ@�O�����J�w�b�_�t�@�C��
 * @author	Hiroyuki Nakamura
 * @date	05.02.01
 */
//============================================================================================
#ifndef B_PLIST_H
#define B_PLIST_H


#include "pm_define.h"
#include "print/printsys.h"
#include "poke_tool/pokeparty.h"
#include "battle/battle.h"


//============================================================================================
//	�萔��`
//============================================================================================
#define	BPL_SEL_EXIT			( 6 )	// �L�����Z��
#define	BPL_SEL_WP_CANCEL	( 4 )	// �Z�Y��L�����Z���i�V�����o����Z�ʒu�j

#define	BPL_CHANGE_SEL_NONE		( BPL_SEL_EXIT )	// �_�u��/�g���v���ł��łɑI������Ă���|�P���������Ȃ��Ƃ�

#define	BPL_SELNUM_MAX		( 3 )			// ����ւ����ɑI���ł���ő吔

#define	BPL_SELPOS_SET		( 0xfe )	// ����ւ���K�v����
#define	BPL_SELPOS_NONE		( 0xff )	// ����ւ���K�v�Ȃ�

// ���X�g������`
enum {
	BPL_MODE_NORMAL = 0,	// �ʏ�̃|�P�����I��
	BPL_MODE_NO_CANCEL,		// �L�����Z���s��
	BPL_MODE_ITEMUSE,		// �A�C�e���g�p
	BPL_MODE_WAZASET,		// �Z�Y��
};

// ���X�g�f�[�^
typedef struct {
	// [in]
	POKEPARTY * pp;		// �|�P�����f�[�^

	GFL_FONT * font;

	HEAPID	heap;			// �q�[�vID

	BtlRule	rule;			// �V���O���^�_�u���^�g���v��

	u8	multi_pos;			// �}���`�̗����ʒu
	u8	change_sel[2];	// �_�u��/�g���v���ł��łɑI������Ă���|�P�����i���Ȃ��ꍇ��BPL_CHANGE_SEL_NONE�j
	u8	mode;						// ���X�g���[�h

	u16	item;						// �A�C�e��
	u16	chg_waza;				// ���ꂩ���֎~�Z�E�V�����o����Z

	// [in & out]
	u8	sel_poke;									// �I�����ꂽ�|�P���� or �߂�
	u8	sel_pos[BPL_SELNUM_MAX];	// �I�����ꂽ�|�P����
	u8 * cursor_flg;							// �J�[�\���\���t���O

	// [out]
	u8	sel_wp;			// �I�����ꂽ�Z�ʒu
	u8	end_flg;		// �I���t���O



//	void * myitem;		// �A�C�e���f�[�^

//	BATTLE_WORK * bw;		// �퓬�p���[�N

//	u8	ret_mode;				// �I�����ꂽ����
//	u32	skill_item_use[2];	// �Z�Ŏg�p�s��
//	u16	hp_rcv;			// HP�񕜗�
//	s32 client_no;						// �N���C�A���gNo
//	u8	list_row[TEMOTI_POKEMAX];	// ���X�g���E
//	u8	bag_page;		// �o�b�O�̃y�[�W

}BPLIST_DATA;


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �퓬�p�|�P�������X�g�^�X�N�ǉ�
 *
 * @param	dat		���X�g�f�[�^
 *
 * @return none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_TaskAdd( BPLIST_DATA * dat );


#endif	/* B_PLIST_H */
