//============================================================================================
/**
 * @file		b_bag.h
 * @brief		�퓬�p�o�b�O��� �O�����J�t�@�C��
 * @author	Hiroyuki Nakamura
 * @date		05.02.10
 */
//============================================================================================
#pragma	once

#include "print/printsys.h"
#include "savedata/myitem_savedata.h"
#include "item/shooter_item.h"


//============================================================================================
//  �萔��`
//============================================================================================
// �������[�h
typedef enum {
  BBAG_MODE_NORMAL = 0, // �ʏ탂�[�h
  BBAG_MODE_SHOOTER,    // �T�|�[�g�V���[�^�[
  BBAG_MODE_GETDEMO,    // �ߊl�f�����[�h
  BBAG_MODE_PDC,        // �|�P�����h���[���L���b�`
}BtlBagMode;

// �|�P�b�gID
enum {
  BBAG_POKE_HPRCV = 0,  // HP�񕜃|�P�b�g
  BBAG_POKE_STRCV,    // ��ԉ񕜃|�P�b�g
  BBAG_POKE_BALL,     // �{�[���|�P�b�g
  BBAG_POKE_BATTLE,   // �퓬�p�|�P�b�g
  BBAG_POKE_MAX
};

// �{�[���g�p�ݒ�
enum BBagBallUse_tag {
  BBAG_BALLUSE_TRUE = 0,    // �g�p�\
  BBAG_BALLUSE_POKEMAX,     // �莝���E�{�b�N�X�ɋ󂫂��Ȃ����ߎg�p�s��
  BBAG_BALLUSE_DOUBLE,      // �쐶�_�u���E�G���Q�C�̂��ߎg�p�s��
};
typedef u8 BBagBallUse;

// �O���ݒ�f�[�^
typedef struct {

  MYITEM_PTR  myitem; ///< �A�C�e���Z�[�u�f�[�^
  GFL_FONT*   font;   ///< �g�p�t�H���g�n���h��
  BtlBagMode  mode;   ///< ���䃂�[�h
  HEAPID      heap;   ///< �q�[�vID

  const SHOOTER_ITEM_BIT_WORK * shooter_item_bit;   ///< �V���[�^�[�p�A�C�e�����[�N
  u8  energy;         ///< �G�l���M�[�l�i�V���[�^�[�p�j
  u8  reserved_energy;///< ���^�[�����Ɋ��ɑI�������A�C�e���̑��R�X�g�l�i�V���[�^�[�p�j

  u16 ret_item;   ///< �I�������A�C�e��
  u8  ret_cost;   ///< �I�������A�C�e���̏���R�X�g�i�V���[�^�[�p�j
  u8  end_flg;    ///< �I���t���O

  BAG_CURSOR* bagcursor;  ///< �o�b�O�J�[�\���̊Ǘ��\���̃|�C���^

  BOOL time_out_flg;    // �����I���t���O TRUE = �����I���@���o�g���������A���^�C���ŕύX���܂�

	BOOL comm_err_flg;		// �ʐM�G���[���̋����I���t���O

  BBagBallUse   ball_use;       // �{�[���𓊂����邩

  u8  ret_page;       // �g�p�����A�C�e���̃|�P�b�g
  u8 * cursor_flg;    // �J�[�\���\���t���O

  s16 item_pos[BATTLE_BAG_POKE_MAX];  // �A�C�e���ʒu
  s16 item_scr[BATTLE_BAG_POKE_MAX];  // �A�C�e���X�N���[���J�E���^

	BOOL	commFlag;			// �ʐM�ΐ킩�ǂ����@TRUE = �ʐM�ΐ�

}BBAG_DATA;


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�퓬�p�o�b�O�^�X�N�ǉ�
 *
 * @param		dat   �o�b�O�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BattleBag_TaskAdd( BBAG_DATA * dat );
