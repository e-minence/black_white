//============================================================================================
/**
 * @file		b_plist.h
 * @brief		�퓬�p�|�P�������X�g��ʁ@�O�����J�w�b�_�t�@�C��
 * @author	Hiroyuki Nakamura
 * @date		05.02.01
 */
//============================================================================================
#pragma	once

#include "pm_define.h"
#include "print/printsys.h"
#include "poke_tool/pokeparty.h"
#include "battle/battle.h"
#include "system/palanm.h"


//============================================================================================
//  �萔��`
//============================================================================================
#define BPL_SEL_EXIT      ( 6 ) // �L�����Z��
#define BPL_SEL_WP_CANCEL ( 4 ) // �Z�Y��L�����Z���i�V�����o����Z�ʒu�j

#define BPL_CHANGE_SEL_NONE   ( BPL_SEL_EXIT )  // �_�u��/�g���v���ł��łɑI������Ă���|�P���������Ȃ��Ƃ�

#define BPL_SELNUM_MAX    ( 3 )     // ����ւ����ɑI���ł���ő吔

#define BPL_SELPOS_NONE   ( 0xff )  // ����ւ��Ȃ�

// ���X�g������`
enum {
  BPL_MODE_NORMAL = 0,  // �ʏ�̃|�P�����I��
  BPL_MODE_NO_CANCEL,   // �L�����Z�������̃|�P�����I��
  BPL_MODE_CHG_DEAD,    // �m������ւ��I��
  BPL_MODE_ITEMUSE,     // �A�C�e���g�p
  BPL_MODE_WAZASET,     // �Z�Y��
  BPL_MODE_WAZAINFO,    // �Z�m�F
};

// ���X�g�f�[�^
typedef struct {
  // [in]
	GAMEDATA * gamedata;		// �Q�[���f�[�^
  POKEPARTY * pp;         // �|�P�����f�[�^
  POKEPARTY * multi_pp;   // �}���`�̑��葤�|�P�����f�[�^

  GFL_FONT * font;	// �t�H���g

  HEAPID  heap;     // �q�[�vID

  BtlRule rule;     // �V���O���^�_�u���^�g���v���@���}���`�o�g�����̃��[���̓_�u���I

  BOOL  multiMode;    // �}���`�o�g�����ǂ��� TRUE = �}���`, FALSE = ����ȊO
  u8  multiPos;       // �}���`�̗����ʒu
  u8  change_sel[2];  // �_�u��/�g���v���ł��łɑI������Ă���|�P�����i���Ȃ��ꍇ��BPL_CHANGE_SEL_NONE�j
  u8  mode;           // ���X�g���[�h

  u8  sel_poke;       // �I�����ꂽ�|�P���� or �߂�@�i�����ʒu�p�j
	u8	sel_pos_index;	//�u���ꂩ���v�������ꂽ�Ƃ��ɁAsel_pos[ sel_pos_index ] �ɑI�����ꂽ�|�P�����ʒu������

	u8	fight_poke_max;	// �퓬�ɏo�Ă��鐔

  u16 item;           // �A�C�e��
  u16 chg_waza;       // ���ꂩ���֎~�Z�E�V�����o����Z

  GFL_TCBSYS* tcb_sys;
  PALETTE_FADE_PTR pfd; // �p���b�g�t�F�[�h�f�[�^

  BOOL time_out_flg;    // �����I���t���O TRUE = �����I���@���o�g���������A���^�C���ŕύX���܂�

	BOOL comm_err_flg;		// �ʐM�G���[���̋����I���t���O

  u8  skill_item_use[ TEMOTI_POKEMAX ]; // �Z�Ŏg�p�s��

  BOOL  seFlag;     // SE���Đ����邩�ǂ��� TRUE = �Đ�

  // [in & out]
  u8 * cursor_flg;              // �J�[�\���\���t���O

  // [out]
  u8  sel_pos[BPL_SELNUM_MAX];  // �I�����ꂽ�|�P�����i�L�����Z�����́A�S�̂�BPL_SELPOS_NONE�ɂȂ�j
  u8  sel_wp;     // �I�����ꂽ�Z�ʒu
  u8  end_flg;    // �I���t���O

}BPLIST_DATA;


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�|�P�������X�g�^�X�N�ǉ�
 *
 * @param		dat   ���X�g�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BattlePokeList_TaskAdd( BPLIST_DATA * dat );
