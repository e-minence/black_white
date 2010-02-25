////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������ʂ̃C���f�b�N�X��`
 * @file   research_menu_index.h
 * @author obata
 * @date   2010.02.13
 */
////////////////////////////////////////////////////////////////
#pragma once


// �����V�[�P���X
typedef enum {
  RESEARCH_MENU_SEQ_SETUP,    // ����
  RESEARCH_MENU_SEQ_KEY_WAIT, // �L�[���͑҂�
  RESEARCH_MENU_SEQ_FADE_OUT, // �t�F�[�h�A�E�g
  RESEARCH_MENU_SEQ_CLEAN_UP, // ��Еt��
  RESEARCH_MENU_SEQ_FINISH,   // �I��
} RESEARCH_MENU_SEQ;

// ���b�Z�[�W�f�[�^
typedef enum {
  MESSAGE_STATIC,         // ��^��
  MESSAGE_QUESTIONNAIRE,  // �A���P�[�g�֘A
  MESSAGE_NUM,            // ����
} MESSAGE_INDEX;

// ���j���[����
typedef enum {
  MENU_ITEM_CHANGE_RESEARCH,  //�u�������e��ύX����v
  MENU_ITEM_CHECK_RESEARCH,   //�u�����񍐂��m�F����v
  MENU_ITEM_NUM,              // ���j���[���ڐ�
} MENU_ITEM;

// �^�b�`�͈�
typedef enum {
  TOUCH_AREA_CHANGE_BUTTON,  //�u�������e��ύX����v
  TOUCH_AREA_CHECK_BUTTON,   //�u�����񍐂��m�F����v
  TOUCH_AREA_DUMMY,          // �I�[�R�[�h
  TOUCH_AREA_NUM,            // ����
} TOUCH_AREA_INDEX;

// BGFont �I�u�W�F�N�g
typedef enum {
  BG_FONT_CAPTION,       // ���� ������
  BG_FONT_TITLE,         // ����� �^�C�g��
  BG_FONT_CHANGE_BUTTON, // ����ʁu�������e�̌���v
  BG_FONT_CHECK_BUTTON,  // ����ʁu�����񍐂̊m�F�v
  BG_FONT_NUM,
} BG_FONT_INDEX;

// OBJ ���\�[�XID
typedef enum {
  OBJ_RESOURCE_MAIN_CHARACTER,  // MAIN-OBJ �L�����N�^
  OBJ_RESOURCE_MAIN_PALETTE,    // MAIN-OBJ �p���b�g
  OBJ_RESOURCE_MAIN_CELL_ANIME, // MAIN-OBJ �Z�� �A�j��
  OBJ_RESOURCE_SUB_CHARACTER,   // SUB-OBJ  �L�����N�^
  OBJ_RESOURCE_SUB_PALETTE,     // SUB-OBJ  �p���b�g
  OBJ_RESOURCE_SUB_CELL_ANIME,  // SUB-OBJ  �Z�� �A�j��
  OBJ_RESOURCE_NUM,             // ����
} OBJ_RESOURCE_ID;

// �Z���A�N�^�[���j�b�g
typedef enum {
  CLUNIT_SUB_OBJ,  // SUB-OBJ
  CLUNIT_MAIN_OBJ, // MAIN-OBJ
  CLUNIT_NUM,      // ����
} CLUNIT_INDEX;

// �Z���A�N�^�[���[�N
typedef enum {
  CLWK_NEW_ICON,  // "new" �A�C�R��
  CLWK_NUM,       // ����
} CLWK_INDEX; 
