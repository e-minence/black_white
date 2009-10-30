//============================================================================================
/**
 * @file  b_bag.h
 * @brief �퓬�p�o�b�O��� �O�����J�t�@�C��
 * @author  Hiroyuki Nakamura
 * @date  05.02.10
 */
//============================================================================================
#ifndef B_BAG_H
#define B_BAG_H

#include "print/printsys.h"
#include "savedata/myitem_savedata.h"


//============================================================================================
//  �萔��`
//============================================================================================
// �������[�h
typedef enum {
  BBAG_MODE_NORMAL = 0, // �ʏ탂�[�h
  BBAG_MODE_SHOOTER,    // �T�|�[�g�V���[�^�[
  BBAG_MODE_GETDEMO,    // �ߊl�f�����[�h
}BtlBagMode;

// �|�P�b�gID
enum {
  BBAG_POKE_HPRCV = 0,  // HP�񕜃|�P�b�g
  BBAG_POKE_STRCV,    // ��ԉ񕜃|�P�b�g
  BBAG_POKE_BALL,     // �{�[���|�P�b�g
  BBAG_POKE_BATTLE,   // �퓬�p�|�P�b�g
  BBAG_POKE_MAX
};

// �O���ݒ�f�[�^
typedef struct {

  MYITEM_PTR  myitem; ///< �A�C�e���Z�[�u�f�[�^
  GFL_FONT*   font;   ///< �g�p�t�H���g�n���h��
  BtlBagMode  mode;   ///< ���䃂�[�h
  HEAPID      heap;   ///< �q�[�vID
  u8  energy;         ///< �G�l���M�[�l�i�V���[�^�[�p�j
  u8  reserved_energy;///< ���^�[�����Ɋ��ɑI�������A�C�e���̑��R�X�g�l�i�V���[�^�[�p�j

  u16 ret_item;   ///< �I�������A�C�e��
  u8  ret_cost;   ///< �I�������A�C�e���̏���R�X�g�i�V���[�^�[�p�j
  u8  end_flg;    ///< �I���t���O

  BAG_CURSOR* bagcursor;  ///< �o�b�O�J�[�\���̊Ǘ��\���̃|�C���^


  u32 skill_item_use; // �Z�Ŏg�p�s��
  s32 client_no;    // �N���C�A���gNo

  u8  enc_double;			// �쐶2vs2�Ń{�[�����������Ȃ��ꍇ�͂P
  u8  waza_vanish;	 // ���肪�u������Ƃԁv�ȂǂŌ����Ȃ��ꍇ�͂P
  u8  waza_migawari;	// ���肪�u�݂����v���g�p���Ă���ꍇ�͂P
  u8  ret_page;			 // �g�p�����A�C�e���̃|�P�b�g
  u8 * cursor_flg;   // �J�[�\���\���t���O

  s16 item_pos[BATTLE_BAG_POKE_MAX];  // �A�C�e���ʒu
  s16 item_scr[BATTLE_BAG_POKE_MAX];  // �A�C�e���X�N���[���J�E���^

//  u16 used_item;    // �O��g�p�����A�C�e��
//  u8  used_poke;    // �O��g�p�����A�C�e���̃|�P�b�g

}BBAG_DATA;


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �퓬�p�o�b�O�^�X�N�ǉ�
 *
 * @param dat   �o�b�O�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void BattleBag_TaskAdd( BBAG_DATA * dat );


#endif  // B_BAG_H
