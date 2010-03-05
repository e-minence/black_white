////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �S��ʋ��ʂ̃C���f�b�N�X��`
 * @file   research_common_index.h
 * @author obata
 * @date   2010.02.26
 */
////////////////////////////////////////////////////////////////
#pragma once

//--------------
// ���^�b�`�͈�
//--------------
typedef enum {
  COMMON_TOUCH_AREA_RETURN_BUTTON, //�u���ǂ�v�{�^��
  COMMON_TOUCH_AREA_DUMMY,         // �I�[�R�[�h
  COMMON_TOUCH_AREA_NUM,           // ����
} COMMON_TOUCH_AREA_INDEX;

//--------------------------
// ���p���b�g�A�j���[�V����
//--------------------------
typedef enum {
  COMMON_PALETTE_ANIME_RETURN, //�u���ǂ�v�A�C�R���I�����A�N�V����
  COMMON_PALETTE_ANIME_NUM,    // ����
} COMMON_PALETTE_ANIME_INDEX;

//--------
// ��OBJ
//--------
// OBJ���\�[�X
typedef enum {
  COMMON_OBJ_RESOURCE_MAIN_CHARACTER,      // MAIN-OBJ �L�����N�^
  COMMON_OBJ_RESOURCE_MAIN_PALETTE_ICON,   // MAIN-OBJ �p���b�g ( �A�C�R�� )
  COMMON_OBJ_RESOURCE_MAIN_CELL_ANIME,     // MAIN-OBJ �Z�� �A�j��
  COMMON_OBJ_RESOURCE_NUM,                 // ����
} COMMON_OBJ_RESOURCE_ID;

// �Z���A�N�^�[���j�b�g
typedef enum {
  COMMON_CLUNIT_MAIN_OBJ, // MAIN-OBJ
  COMMON_CLUNIT_NUM,      // ����
} COMMON_CLUNIT_INDEX;

// �Z���A�N�^�[���[�N
typedef enum {
  COMMON_CLWK_RETURN, //�u���ǂ�v�{�^��
  COMMON_CLWK_NUM,    // ����
} COMMON_CLWK_INDEX; 
