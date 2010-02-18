////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX��ʂ̃C���f�b�N�X��`
 * @file   research_select_index.h
 * @author obata
 * @date   2010.02.15
 */
////////////////////////////////////////////////////////////////
#pragma once


// �����V�[�P���X
typedef enum {
  RESEARCH_SELECT_SEQ_SETUP,       // ����
  RESEARCH_SELECT_SEQ_KEY_WAIT,    // �L�[���͑҂�
  RESEARCH_SELECT_SEQ_SCROLL_WAIT, // �X�N���[�������҂�
  RESEARCH_SELECT_SEQ_TO_CONFIRM,  // �������ڊm��̊m�F�V�[�P���X�ւ̏���
  RESEARCH_SELECT_SEQ_CONFIRM,     // �������ڊm��̊m�F
  RESEARCH_SELECT_SEQ_DETERMINE,   // �������ڊm��
  RESEARCH_SELECT_SEQ_CLEAN_UP,    // ��Еt��
  RESEARCH_SELECT_SEQ_FINISH,      // �I��
} RESEARCH_SELECT_SEQ;

// �I������
typedef enum {
  MENU_ITEM_DETERMINATION_OK,     //�u�����Ă��v
  MENU_ITEM_DETERMINATION_CANCEL, //�u��߂�v
  MENU_ITEM_NUM,                  // ����
} MENU_ITEM;

// ���b�Z�[�W�f�[�^
typedef enum {
  MESSAGE_STATIC,         // ��^��
  MESSAGE_QUESTIONNAIRE,  // �A���P�[�g�֘A
  MESSAGE_NUM,            // ����
} MESSAGE_INDEX;

// BGFont �I�u�W�F�N�g
typedef enum {
  BG_FONT_TITLE,         // ���� �^�C�g��
  BG_FONT_DIRECTION,     // ���� ������
  BG_FONT_TOPIC_TITLE,   // ���� �������ږ�
  BG_FONT_TOPIC_CAPTION, // ���� �������ڂ̕⑫��
  BG_FONT_QUESTION_1,    // ���� ����1
  BG_FONT_QUESTION_2,    // ���� ����2
  BG_FONT_QUESTION_3,    // ���� ����3
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
  CLWK_SCROLL_CONTROL,// �X�N���[���o�[�̂܂�
  CLWK_SELECT_ICON_0, // �������ڑI���A�C�R��0
  CLWK_SELECT_ICON_1, // �������ڑI���A�C�R��1
  CLWK_SELECT_ICON_2, // �������ڑI���A�C�R��2
  CLWK_NUM,           // ����
} CLWK_INDEX; 
