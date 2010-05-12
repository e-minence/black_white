////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX��ʂ̃C���f�b�N�X��`
 * @file   research_list_index.h
 * @author obata
 * @date   2010.02.15
 */
////////////////////////////////////////////////////////////////
#pragma once


// ���
typedef enum {
  RRL_STATE_SETUP,             // ����
  RRL_STATE_STANDBY,           // �ŏ��̃L�[���͑҂�
  RRL_STATE_KEY_WAIT,          // �L�[���͑҂�
  RRL_STATE_AUTO_SCROLL,       // �����X�N���[��
  RRL_STATE_SLIDE_CONTROL,     // �X�N���[������
  RRL_STATE_CONFIRM_STANDBY,   // �������ڊm��̊m�F ( �X�^���o�C )
  RRL_STATE_CONFIRM_KEY_WAIT,  // �������ڊm��̊m�F ( �L�[���͑҂� )
  RRL_STATE_DETERMINE,         // �������ڊm��
  RRL_STATE_FADE_IN,           // �t�F�[�h�C��
  RRL_STATE_FADE_OUT,          // �t�F�[�h�A�E�g
  RRL_STATE_FRAME_WAIT,        // �t���[���o�ߑ҂�
  RRL_STATE_SCROLL_RESET,      // �X�N���[���̕��A
  RRL_STATE_PALETTE_RESET,     // �p���b�g�̕��A
  RRL_STATE_CLEAN_UP,          // ��Еt��
  RRL_STATE_FINISH,            // �I��
} RRL_STATE;

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

// �^�b�`�͈� ( �X�N���[���o�[ )
typedef enum {
  SCROLL_TOUCH_AREA_BAR,   // �X�N���[���o�[
  SCROLL_TOUCH_AREA_DUMMY, // �I�[�R�[�h
  SCROLL_TOUCH_AREA_NUM,   // ����
} SCROLL_TOUCH_AREA_INDEX;

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
  PALETTE_ANIME_TOPIC_CURSOR_SET,  // �������ڃJ�[�\��SET
  PALETTE_ANIME_TOPIC_SELECT,      // �������ڑI�����A�N�V����
  PALETTE_ANIME_MENU_CURSOR_ON,    // ���j���[���ڃJ�[�\��ON
  PALETTE_ANIME_MENU_CURSOR_SET,   // ���j���[���ڃJ�[�\��SET
  PALETTE_ANIME_MENU_SELECT,       // ���j���[���ڑI�����A�N�V����
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
  CLWK_SCROLL_BAR,    // �X�N���[���o�[
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
