////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������񍐊m�F��ʂ̃C���f�b�N�X��`
 * @file   research_check_index.h
 * @author obata
 * @date   2010.02.20
 */
////////////////////////////////////////////////////////////////
#pragma once


// �����V�[�P���X
typedef enum {
  RESEARCH_CHECK_SEQ_SETUP,     // �����V�[�P���X
  RESEARCH_CHECK_SEQ_KEY_WAIT,  // �L�[���͑҂��V�[�P���X
  RESEARCH_CHECK_SEQ_ANALYZE,   // ��̓V�[�P���X
  RESEARCH_CHECK_SEQ_FLASH,     // ��ʃt���b�V���V�[�P���X
  RESEARCH_CHECK_SEQ_UPDATE,    // �X�V�V�[�P���X
  RESEARCH_CHECK_SEQ_FADE_OUT,  // �t�F�[�h�A�E�g
  RESEARCH_CHECK_SEQ_CLEAN_UP,  // ��Еt���V�[�P���X
  RESEARCH_CHECK_SEQ_FINISH,    // �I���V�[�P���X
} RESEARCH_CHECK_SEQ;

// �I������
typedef enum {
  MENU_ITEM_QUESTION,  // ����
  MENU_ITEM_ANSWER,    // ��
  MENU_ITEM_MY_ANSWER, // �����̉�
  MENU_ITEM_COUNT,     // �񓚐l��
  MENU_ITEM_NUM,       // ����
} MENU_ITEM;

// �^�b�`�͈�
typedef enum {
  TOUCH_AREA_CONTROL_CURSOR_L,// ���J�[�\��
  TOUCH_AREA_CONTROL_CURSOR_R,// �E�J�[�\��
  TOUCH_AREA_QUESTION,        // ����
  TOUCH_AREA_RETURN_BUTTON,   //�u���ǂ�v�{�^��
  TOUCH_AREA_DUMMY,           // �I�[�R�[�h
  TOUCH_AREA_NUM,             // ����
} TOUCH_AREA_INDEX;

// �����f�[�^�̕\���^�C�v
typedef enum {
  DATA_DISP_TYPE_TODAY, // �����̃f�[�^��\�� 
  DATA_DISP_TYPE_TOTAL, // ���v�̃f�[�^��\��
  DATA_DISP_TYPE_NUM,   // ����
} DATA_DISP_TYPE;

// ���b�Z�[�W�f�[�^
typedef enum {
  MESSAGE_STATIC,         // ��^��
  MESSAGE_QUESTIONNAIRE,  // �A���P�[�g�֘A
  MESSAGE_NUM,            // ����
} MESSAGE_INDEX;

// BGFont �I�u�W�F�N�g
typedef enum {
  SUB_BG_FONT_TITLE,            // ���� �^�C�g��
  SUB_BG_FONT_TOPIC_CAPTION,    // ���� �������ڂ̕⑫��
  SUB_BG_FONT_QUESTION_CAPTION, // ���� ����̕⑫��
  MAIN_BG_FONT_QUESTION,        // ����� ����
  MAIN_BG_FONT_ANSWER,          // ����� ��
  MAIN_BG_FONT_MY_ANSWER,       // ����� �����̉�
  MAIN_BG_FONT_COUNT,           // ����� �񓚐l��
  MAIN_BG_FONT_NO_DATA,         // ����ʁu�������܂��傤�����イ�v
  MAIN_BG_FONT_DATA_RECEIVING,  // ����ʁu�f�[�^����Ƃ����イ�v
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
  CLUNIT_BMPOAM,   // BMP-OAM �A�N�^�[�Ŏg�p
  CLUNIT_NUM,      // ����
} CLUNIT_INDEX;

// �Z���A�N�^�[���[�N
typedef enum {
  CLWK_CONTROL_CURSOR_L, // ���J�[�\��
  CLWK_CONTROL_CURSOR_R, // �E�J�[�\��
  CLWK_MY_ANSWER_ICON,   // �����̉񓚃A�C�R��
  CLWK_NUM,              // ����
} CLWK_INDEX; 

// BMP-OAM
typedef enum {
  BMPOAM_ACTOR_ANALYZING,  //�u�c�����������イ�c�v
  BMPOAM_ACTOR_NUM,        // ����
} BMPOAM_ACTOR_INDEX;
