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

#include "gamesystem/game_data.h"
#include "poke_tool/pokeparty.h"
#include "savedata/config.h"


//-----------------------------------------------
//�ȉ�GS���ڐA(09/07/01)
//enum��typedef�Ō^�ǉ�
//-----------------------------------------------
typedef struct {
  void * ppd;   // �|�P�����f�[�^
  CONFIG * cfg; // �R���t�B�O�f�[�^
  GAMEDATA *game_data; //�Q�[���f�[�^

  //game_data����擾����悤�ɕύX
  //const STRCODE * player_name;  // ���Ă���l�̖��O
  //u32 player_id;          // ���Ă���l��ID
  //u8  player_sex;         // ���Ă���l�̐���

  u8  ppt;    // �p�����[�^�^�C�v
  u8  mode;   // ���[�h
  u8  max;    // �ő吔
  u8  pos;    // ���Ԗڂ̃f�[�^��

  u8  page;     //�����\���y�[�W  //PSTATUS_PAGE_TYPE //�Z���[�h�ł͖���
  u8  ret_sel;  // �I�����ꂽ�Z�ʒu
  u8  ret_mode;
  u16 waza;     //�Z�ǉ����͋Z�ԍ��B0�őI�����[�h

  u32       zukan_mode; // �}�Ӄi���o�[�\�����[�h

  //void      *poruto;  // �^����|���g�̃f�[�^ ( _PURUTO_DATA )

  //PERAPVOICE    *perap;   // �؃��b�v�̖����f�[�^

//  BOOL      ev_contest; // �R���e�X�g�p
//  BOOL      pokethlon;  // �|�P�X�����p�t�H�[�}���X��ʕ\�����ǂ���
  BOOL      waza_chg; // �Z�����ւ������ǂ��� TRUE = ����ւ���, FALSE = ����ȊO

  BOOL      canExitButton;  //X�߂肪�g���邩�H
  BOOL      isExitRequest;  //���Ԑ؂ꎞ�A�I�����N�G�X�g���o��
  

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
  PST_RET_EXIT,
  
};


typedef enum
{
  PPT_INFO,     //�|�P�������
  PPT_SKILL,    //�Z
  PPT_RIBBON,   //���{��
  
  PPT_SKILL_ADD,    //�Z�n

  PPT_MAX,
}PSTATUS_PAGE_TYPE;

//-----------------------------------------------
//�ȏ�GS���ڐA(09/07/01)
//-----------------------------------------------

extern GFL_PROC_DATA PokeStatus_ProcData;
