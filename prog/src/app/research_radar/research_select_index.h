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
  RESEARCH_SELECT_SEQ_SETUP,        // ����
  RESEARCH_SELECT_SEQ_STANDBY,      // �ŏ��̃L�[���͑҂�
  RESEARCH_SELECT_SEQ_KEY_WAIT,     // �L�[���͑҂�
  RESEARCH_SELECT_SEQ_SCROLL_WAIT,  // �X�N���[�������҂�
  RESEARCH_SELECT_SEQ_CONFIRM,      // �������ڊm��̊m�F
  RESEARCH_SELECT_SEQ_DETERMINE,    // �������ڊm��
  RESEARCH_SELECT_SEQ_FADE_IN,      // �t�F�[�h�C��
  RESEARCH_SELECT_SEQ_FADE_OUT,     // �t�F�[�h�A�E�g
  RESEARCH_SELECT_SEQ_PALETTE_RESET,// �p���b�g�̕��A
  RESEARCH_SELECT_SEQ_CLEAN_UP,     // ��Еt��
  RESEARCH_SELECT_SEQ_FINISH,       // �I��
} RESEARCH_SELECT_SEQ;

// �I������
typedef enum {
  MENU_ITEM_DETERMINATION_OK,     //�u�����Ă��v
  MENU_ITEM_DETERMINATION_CANCEL, //�u��߂�v
  MENU_ITEM_NUM,                  // ����
} MENU_ITEM;

// �^�b�`�͈� ( ���j���[���� )
typedef enum {
  MENU_TOUCH_AREA_OK_BUTTON,     //�u����v�{�^��
  MENU_TOUCH_AREA_CANCEL_BUTTON, //�u��߂�v�{�^��
  MENU_TOUCH_AREA_DUMMY,         // �I�[�R�[�h
  MENU_TOUCH_AREA_NUM,           // ����
} MENU_TOUCH_AREA_INDEX;

// �^�b�`�͈� ( �������ڃ��X�g )
typedef enum {
  TOPIC_TOUCH_AREA_TOPIC_0,       // ��������0
  TOPIC_TOUCH_AREA_TOPIC_1,       // ��������1
  TOPIC_TOUCH_AREA_TOPIC_2,       // ��������2
  TOPIC_TOUCH_AREA_TOPIC_3,       // ��������3
  TOPIC_TOUCH_AREA_TOPIC_4,       // ��������4
  TOPIC_TOUCH_AREA_TOPIC_5,       // ��������5
  TOPIC_TOUCH_AREA_TOPIC_6,       // ��������6
  TOPIC_TOUCH_AREA_TOPIC_7,       // ��������7
  TOPIC_TOUCH_AREA_TOPIC_8,       // ��������8
  TOPIC_TOUCH_AREA_TOPIC_9,       // ��������9
  TOPIC_TOUCH_AREA_DUMMY,         // �I�[�R�[�h
  TOPIC_TOUCH_AREA_NUM,           // ����
} TOPIC_TOUCH_AREA_INDEX;

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
  OBJ_RESOURCE_MAIN_PALETTE_COMMON_BUTTON, // MAIN-OBJ �p���b�g
  OBJ_RESOURCE_SUB_CHARACTER,   // SUB-OBJ  �L�����N�^
  OBJ_RESOURCE_SUB_PALETTE,     // SUB-OBJ  �p���b�g
  OBJ_RESOURCE_SUB_CELL_ANIME,  // SUB-OBJ  �Z�� �A�j��
  OBJ_RESOURCE_NUM,             // ����
} OBJ_RESOURCE_ID;

// �p���b�g�A�j���[�V����
typedef enum {
  PALETTE_ANIME_TOPIC_CURSOR_ON,   // �������ڃJ�[�\��ON
  PALETTE_ANIME_TOPIC_SELECT,      // �������ڑI�����A�N�V����
  PALETTE_ANIME_MENU_CURSOR_ON, // ���j���[���ڃJ�[�\��ON
  PALETTE_ANIME_MENU_SELECT,    // ���j���[���ڑI�����A�N�V����
  PALETTE_ANIME_NUM,               // ����
} PALETTE_ANIME_INDEX;

// �Z���A�N�^�[���j�b�g
typedef enum {
  CLUNIT_SUB_OBJ,  // SUB-OBJ
  CLUNIT_MAIN_OBJ, // MAIN-OBJ
  CLUNIT_BMPOAM,   // BMP-OAM �A�N�^�[�Ŏg�p
  CLUNIT_NUM,      // ����
} CLUNIT_INDEX;

// �Z���A�N�^�[���[�N
typedef enum {
  CLWK_SCROLL_CONTROL,// �X�N���[���o�[�̂܂�
  CLWK_SELECT_ICON_0, // �������ڑI���A�C�R��0
  CLWK_NUM,           // ����
} CLWK_INDEX; 

// BMP-OAM
typedef enum {
  BMPOAM_ACTOR_CONFIRM,    // �m�F���b�Z�[�W
  BMPOAM_ACTOR_OK,         //�u�����Ă��v
  BMPOAM_ACTOR_CANCEL,     //�u��߂�v
  BMPOAM_ACTOR_DETERMINE,  //�u���傤�����@���������܂��I�v
  BMPOAM_ACTOR_NUM,        // ����
} BMPOAM_ACTOR_INDEX;
