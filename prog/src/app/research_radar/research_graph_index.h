////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �O���t���
 * @file   research_graph_index.h
 * @author obata
 * @date   2010.02.20
 */
////////////////////////////////////////////////////////////////
#pragma once


// �O���t��ʂ̏��
typedef enum {
  RRG_STATE_SETUP,             // ����
  RRG_STATE_INTRUDE_SHUTDOWN,  // �N���ؒf
  RRG_STATE_STANDBY,           // �ŏ��̃L�[���͑҂�
  RRG_STATE_KEYWAIT,           // �L�[���͑҂�
  RRG_STATE_ANALYZE,           // ���
  RRG_STATE_PERCENTAGE,        // ���\��
  RRG_STATE_FLASHOUT,          // ��ʃt���b�V�� ( �A�E�g )
  RRG_STATE_FLASHIN,           // ��ʃt���b�V�� ( �C�� )
  RRG_STATE_UPDATE_AT_STANDBY, // �X�V ( �ŏ��̃L�[���͑҂��� )
  RRG_STATE_UPDATE_AT_KEYWAIT, // �X�V ( �L�[���͑҂��� )
  RRG_STATE_FADEOUT,           // �t�F�[�h�A�E�g
  RRG_STATE_WAIT,              // �t���[���o�ߑ҂�
  RRG_STATE_CLEANUP,           // ��Еt��
  RRG_STATE_FINISH,            // �I��
} RRG_STATE;

// �I������
typedef enum {
  MENU_ITEM_QUESTION,  // ����
  MENU_ITEM_ANSWER,    // ��
  MENU_ITEM_MY_ANSWER, // �����̉�
  MENU_ITEM_COUNT,     // �񓚐l��
  MENU_ITEM_ANALYZE,   //�u�񍐂�����v�{�^��
  MENU_ITEM_NUM,       // ����
} MENU_ITEM;

// �^�b�`�͈�
typedef enum {
  TOUCH_AREA_CONTROL_CURSOR_L,// ���J�[�\��
  TOUCH_AREA_CONTROL_CURSOR_R,// �E�J�[�\��
  TOUCH_AREA_QUESTION,        // ����
  TOUCH_AREA_ANSWER,          // ��
  TOUCH_AREA_MY_ANSWER,       // �����̉�
  TOUCH_AREA_COUNT,           // �񓚐l��
  TOUCH_AREA_GRAPH,           // �~�O���t
  TOUCH_AREA_ANALYZE_BUTTON,  //�u�ق��������݂�v�{�^��
  TOUCH_AREA_DUMMY,           // �I�[�R�[�h
  TOUCH_AREA_NUM,             // ����
} TOUCH_AREA_INDEX;

// �p���b�g�A�j���[�V����
typedef enum {
  PALETTE_ANIME_CURSOR_ON, // �J�[�\��ON
  PALETTE_ANIME_CURSOR_SET,// �J�[�\��SET
  PALETTE_ANIME_SELECT,    //�u�ق��������݂�v�{�^���I�����A�N�V����
  PALETTE_ANIME_ANALYZE_CURSOR_ON, //�u�ق��������݂�v�{�^���J�[�\��ON
  PALETTE_ANIME_ANALYZE_CURSOR_SET, //�u�ق��������݂�v�{�^���J�[�\��SET
  PALETTE_ANIME_HOLD,      //�u�ق��������݂�v�{�^���Ó]
  PALETTE_ANIME_RECOVER,   //�u�ق��������݂�v�{�^�����A
  PALETTE_ANIME_RECEIVE_BUTTON, //�u�f�[�^���サ�񂿂イ�v�{�^���t�F�[�h
  PALETTE_ANIME_NUM,       // ����
} PALETTE_ANIME_INDEX;

// �O���t�̕\�����[�h
typedef enum {
  GRAPH_DISP_MODE_TODAY, // �����̃f�[�^��\�� 
  GRAPH_DISP_MODE_TOTAL, // ���v�̃f�[�^��\��
  GRAPH_DISP_MODE_NUM,   // ����
} GRAPH_DISP_MODE;

// ���b�Z�[�W�f�[�^
typedef enum {
  MESSAGE_STATIC,         // ��^��
  MESSAGE_QUESTIONNAIRE,  // �A���P�[�g�֘A
  MESSAGE_ANSWER,         // ��
  MESSAGE_NUM,            // ����
} MESSAGE_INDEX;

// BGFont �I�u�W�F�N�g
typedef enum {
  SUB_BG_FONT_TITLE,            // ���� �^�C�g��
  SUB_BG_FONT_TOPIC_TITLE,      // ���� �������ږ�
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
  OBJ_RESOURCE_MAIN_CHARACTER,      // MAIN-OBJ �L�����N�^
  OBJ_RESOURCE_MAIN_PALETTE,        // MAIN-OBJ �p���b�g
  OBJ_RESOURCE_MAIN_CELL_ANIME,     // MAIN-OBJ �Z�� �A�j��
  OBJ_RESOURCE_MAIN_COMMON_PALETTE, // MAIN-OBJ �p���b�g ( ���ʑf�ރ{�^���p )
  OBJ_RESOURCE_SUB_CHARACTER,       // SUB-OBJ  �L�����N�^
  OBJ_RESOURCE_SUB_PALETTE,         // SUB-OBJ  �p���b�g
  OBJ_RESOURCE_SUB_CELL_ANIME,      // SUB-OBJ  �Z�� �A�j��
  OBJ_RESOURCE_NUM,                 // ����
} OBJ_RESOURCE_ID;

// �Z���A�N�^�[���j�b�g
typedef enum {
  CLUNIT_SUB_OBJ,    // SUB-OBJ
  CLUNIT_MAIN_OBJ,   // MAIN-OBJ
  CLUNIT_BMPOAM,     // BMP-OAM �A�N�^�[�Ŏg�p
  CLUNIT_PERCENTAGE, // ���l�̕\���Ɏg�p
  CLUNIT_NUM,        // ����
} CLUNIT_INDEX;

// �Z���A�N�^�[���[�N
typedef enum {
  CLWK_CONTROL_CURSOR_L,         // ���J�[�\��
  CLWK_CONTROL_CURSOR_R,         // �E�J�[�\��
  CLWK_MY_ANSWER_ICON_ON_BUTTON, // �����̉񓚃A�C�R�� ( �{�^���� )
  CLWK_MY_ANSWER_ICON_ON_GRAPH,  // �����̉񓚃A�C�R�� ( �O���t�� )
  CLWK_NUM,                      // ����
} CLWK_INDEX; 

// BMP-OAM
typedef enum {
  BMPOAM_ACTOR_ANALYZING,      //�u�c�����������イ�c�v
  BMPOAM_ACTOR_ANALYZE_BUTTON, //�u�ق��������݂�v�{�^��
  BMPOAM_ACTOR_NUM,            // ����
} BMPOAM_ACTOR_INDEX;
