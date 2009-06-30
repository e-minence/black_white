//======================================================================
/**
 * @file  pokelist.h
 * @brief �|�P�������X�g
 * @author  ariizumi
 * @data  09/06/10
 *
 * ���W���[�����FPOKE_LIST
 */
//======================================================================
#pragma once

#include "poke_tool/pokeparty.h"
#include "savedata/config.h"

//-----------------------------------------------
//�ȉ�GS���ڐA(09/06/10)
//enum��typedef�Ō^�ǉ�
//-----------------------------------------------

// ���X�g�^�C�v
typedef enum
{
  PL_TYPE_SINGLE = 0,   // �V���O��
  PL_TYPE_DOUBLE,     // �_�u��
  PL_TYPE_MULTI,      // �}���`
}PL_LIST_TYPE;

// ���X�g���[�h
typedef enum
{
  PL_MODE_FIELD = 0,      // �t�B�[���h���j���[
  PL_MODE_BATTLE,         // �퓬
  PL_MODE_SELECT,         // �u�����Ă��v����
  PL_MODE_SET,            // �I�������i���j���[�Ȃ��j
  PL_MODE_IN,             // �}���`�o�g���J�n�G�t�F�N�g
  PL_MODE_ITEMUSE,        // �A�C�e���g�p
  PL_MODE_WAZASET,        // �Z�o��
  PL_MODE_WAZASET_RET,    // �X�e�[�^�X��ʂ���Z��I�����Ė߂��Ă����Ƃ��i�Z�}�V���j
  PL_MODE_LVUPWAZASET_RET,// �X�e�[�^�X��ʂ���Z��I�����Ė߂��Ă����Ƃ��i�s�v�c�ȃA���j
  PL_MODE_ITEMSET,        // �A�C�e���u��������v
  PL_MODE_ITEMSET_RET,    // �|�P�������X�g�u��������v�o�b�O����
  PL_MODE_MAILSET,        // ���[���u��������v�I��
  PL_MODE_MAILSET_BAG,    // �|�P�������X�g ���[���u��������v�I���i�o�b�O����j
  PL_MODE_CONTEST,        // �R���e�X�g�Q���I��
  PL_MODE_MAILBOX,        // ���[���{�b�N�X���烁�[������������
  PL_MODE_KAPSEL_SET,     // �J�v�Z���Z�b�g�p
  PL_MODE_SHINKA,         // �i���A�C�e���g�p
  PL_MODE_BATTLE_TOWER,   // �o�g���^���[�Q���I��p
  PL_MODE_SODATEYA,       // ��ĉ�
  PL_MODE_NPC_TRADE,      // �Q�[��������
  PL_MODE_PORUTO,         // �|���g���g�p�I��p
  PL_MODE_GURU2,          // ���邮��������܂��I��
  PL_MODE_BATTLE_STAGE,   // �o�g���X�e�[�W�Q���I��p
  PL_MODE_BATTLE_CASTLE,  // �o�g���L���b�X���Q���I��p
  PL_MODE_BATTLE_ROULETTE = (PL_MODE_BATTLE_CASTLE),  // �o�g�����[���b�g�Q���I��p
}PL_MODE_TYPE;

#define PL_MODE_NO_CANCEL ( 0x80 )  // �L�����Z���֎~

// �I�����ꂽ����
typedef enum
{
  PL_SEL_POS_POKE1 = 0, // �|�P�����P
  PL_SEL_POS_POKE2,   // �|�P�����Q
  PL_SEL_POS_POKE3,   // �|�P�����R
  PL_SEL_POS_POKE4,   // �|�P�����S
  PL_SEL_POS_POKE5,   // �|�P�����T
  PL_SEL_POS_POKE6,   // �|�P�����U
  PL_SEL_POS_ENTER,   // ����
  PL_SEL_POS_EXIT,    // �߂�

  PL_SEL_POS_EXIT2,   // �߂�(��C�ɖ߂�X�{�^���̕�
  
  PL_SEL_POS_MAX,
  
}PL_SELECT_POS;

// �I�����ꂽ����
typedef enum
{
  PL_RET_NORMAL = 0,  // �ʏ�
  PL_RET_STATUS,      // ���j���[�u�悳���݂�v
  PL_RET_CHANGE,      // ���j���[�u���ꂩ����v
  PL_RET_ITEMSET,     // ���j���[�u��������v

  PL_RET_WAZASET,     // �Z�I���ցi�Z�}�V���j
  PL_RET_LVUP_WAZASET,// �Z�I���ցi�s�v�c�ȃA���j
  PL_RET_MAILSET,     // ���[�����͂�
  PL_RET_MAILREAD,    // ���[����ǂމ�ʂ�

  PL_RET_ITEMSHINKA,  // �i����ʂցi�A�C�e���i���j
  PL_RET_LVUPSHINKA,  // �i����ʂցi���x���i���j
  
  PL_RET_BAG,         // �o�b�O����Ă΂ꂽ�ꍇ�ŁA�o�b�O�֖߂�
  
/** ������ field_skill��`���m�[�g�Ɠ������тŁI ������ **/
  PL_RET_IAIGIRI,     // ���j���[ �Z�F����������
  PL_RET_SORAWOTOBU,  // ���j���[ �Z�F������Ƃ�
  PL_RET_NAMINORI,    // ���j���[ �Z�F�Ȃ݂̂�
  PL_RET_KAIRIKI,     // ���j���[ �Z�F�����肫
  PL_RET_KIRIBARAI,   // ���j���[ �Z�F����΂炢
  PL_RET_IWAKUDAKI,   // ���j���[ �Z�F���킭����
  PL_RET_TAKINOBORI,  // ���j���[ �Z�F�����̂ڂ�
  PL_RET_ROCKCLIMB,   // ���j���[ �Z�F���b�N�N���C��

  PL_RET_FLASH,       // ���j���[ �Z�F�t���b�V��
  PL_RET_TELEPORT,    // ���j���[ �Z�F�e���|�[�g
  PL_RET_ANAWOHORU,   // ���j���[ �Z�F���Ȃ��ق�
  PL_RET_AMAIKAORI,   // ���j���[ �Z�F���܂�������
  PL_RET_OSYABERI,    // ���j���[ �Z�F������ׂ�
  PL_RET_MILKNOMI,    // ���j���[ �Z�F�~���N�̂�
  PL_RET_TAMAGOUMI,   // ���j���[ �Z�F�^�}�S����
}PL_RETURN_TYPE;

//-----------------------------------------------
// �|�P�������X�g�O���쐬�f�[�^
//-----------------------------------------------
typedef struct 
{
  POKEPARTY * pp;   // �|�P�����f�[�^
  void * myitem;    // �A�C�e���f�[�^
  void * mailblock; // ���[���f�[�^
  CONFIG * cfg;   // �R���t�B�O�f�[�^
  void * tvwk;    // �e���r���[�N
  void * reg;     // ���M�����[�V�����f�[�^

  void * scwk;    // �Z�g�p�`�F�b�N���[�N

  void * fsys;    // �t�B�[���h���[�N

  PL_MODE_TYPE  mode;     // ���X�g���[�h
  PL_LIST_TYPE  type;     // ���X�g�^�C�v
  PL_SELECT_POS  ret_sel;    // �I�����ꂽ�|�P���� or ���� or �߂� �� �����J�[�\���ʒu
  PL_RETURN_TYPE ret_mode;   // �I�����ꂽ����

  u16 item;     // �A�C�e��
  u16 waza;     // �Z
  u8  waza_pos;   // �Z�ʒu

// TODO �~���[�W�J���ɒu�������H
  u8  con_mode;   // �R���e�X�g���[�h�i�R���e�X�g/�R���N�[��/�g���C�A���j
  u8  con_type;   // �R���e�X�g�^�C�v�i�������Ƃ������Ƃ��j
  u8  con_rank;   // �R���e�X�g�����N

  u8  in_num[6];    // �Q���ԍ�
  u8  in_min:4;   // �Q���ŏ���
  u8  in_max:4;   // �Q���ő吔
  u8  in_lv;      // �Q�����x��

  int lv_cnt;     // ���x���A�b�v�J�E���^

  u16 after_mons;   // �i����̃|�P����
  int shinka_cond;  // �i���������[�N

}PLIST_DATA;

//-----------------------------------------------
//�ȏ�GS���ڐA(09/06/10)
//-----------------------------------------------

extern GFL_PROC_DATA PokeList_ProcData;

