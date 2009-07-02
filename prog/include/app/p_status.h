//======================================================================
/**
 * @file  p_status.c
 * @brief �|�P�����X�e�[�^�X
 * @author  ariizumi
 * @data  09/07/01
 *
 * ���W���[�����FPOKE_STATUS
 */
//======================================================================
#pragma once

#include "poke_tool/pokeparty.h"
#include "savedata/config.h"

//-----------------------------------------------
//�ȉ�GS���ڐA(09/07/01)
//enum��typedef�Ō^�ǉ�
//-----------------------------------------------
typedef struct {
  void * ppd;   // �|�P�����f�[�^
  CONFIG * cfg; // �R���t�B�O�f�[�^

  const STRCODE * player_name;  // ���Ă���l�̖��O
  u32 player_id;          // ���Ă���l��ID
  u8  player_sex;         // ���Ă���l�̐���

  u8  ppt;    // �p�����[�^�^�C�v
  u8  mode;   // ���[�h
  u8  max;    // �ő吔
  u8  pos;    // ���Ԗڂ̃f�[�^��

  u8  page_flg; // �y�[�W�t���O
  u8  ret_sel;  // �I�����ꂽ�Z�ʒu
  u8  ret_mode;
  u16 waza;

  u32       zukan_mode; // �}�Ӄi���o�[�\�����[�h

  void      *ribbon;  // �z�z���{���f�[�^

  void      *poruto;  // �^����|���g�̃f�[�^ ( _PURUTO_DATA )

  //PERAPVOICE    *perap;   // �؃��b�v�̖����f�[�^

  BOOL      ev_contest; // �R���e�X�g�p
  BOOL      pokethlon;  // �|�P�X�����p�t�H�[�}���X��ʕ\�����ǂ���
  BOOL      waza_chg; // �Z�����ւ������ǂ��� TRUE = ����ւ���, FALSE = ����ȊO

}PSTATUS_DATA;

// ���[�h
enum {
  PST_MODE_NORMAL = 0,  // �ʏ�
  PST_MODE_NO_WAZACHG,  // �Z����ւ��s��
  PST_MODE_WAZAADD,   // �Z�o��/�Z�Y��
  PST_MODE_PORUTO,    // �|���g�g�p       �i�g��Ȃ��j
  PST_MODE_CONDITION,   // �R���f�B�V�����ύX�f�� �i�g��Ȃ��j
};

// �p�����[�^�^�C�v
enum {
  PST_PP_TYPE_POKEPARAM = 0,  // POKEMON_PARAM
  PST_PP_TYPE_POKEPARTY,      // POKEPARTY
  PST_PP_TYPE_POKEPASO,       // POKEMON_PASO_PARAM
#if PM_DEBUG
  PST_PP_TYPE_DEBUG,
#endif
};

// �߂�
enum {
  PST_RET_DECIDE = 0,
  PST_RET_CANCEL,
};


// �y�[�WID
enum {
  PST_PAGE_INFO_MEMO = 0, // �u�|�P�������傤�ق��v�u�g���[�i�[�����v
  PST_PAGE_PARAM_B_SKILL, // �u�|�P�����̂���傭�v�u�킴�����v
  PST_PAGE_RIBBON,    // �u���˂񃊃{���v�u�|�P�X�����p�t�H�[�}���X�v
  PST_PAGE_RET,     // �u���ǂ�v
  PST_PAGE_MAX,
  PST_POKE1 = PST_PAGE_MAX,
  PST_POKE2,
  PST_POKE3,
  PST_POKE4,
  PST_POKE5,
  PST_POKE6,
};
//-----------------------------------------------
//�ȏ�GS���ڐA(09/07/01)
//-----------------------------------------------

extern GFL_PROC_DATA PokeStatus_ProcData;

