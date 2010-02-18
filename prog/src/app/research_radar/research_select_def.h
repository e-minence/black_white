/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX��� �C���f�b�N�X��`
 * @file   research_select_def.h
 * @author obata
 * @date   2010.02.15
 */
/////////////////////////////////////////////////////////////////////////////////
#pragma once


//===============================================================================
// ������
//===============================================================================
#define SELECT_TOPIC_MAX_NUM (3)  // �I�����钲�����ڂ̐�

// �������ڂ̃X�N���[��
#define SCROLL_TOP_MARGIN    (24)  // ����X�N���[���̗]��
#define SCROLL_BOTTOM_MARGIN (48)  // �����X�N���[���̗]��
#define SCROLL_FRAME         (4)   // �X�N���[���ɗv����t���[����

// �X�N���[���o�[�܂ݕ���
#define SCROLL_CONTROL_LEFT     (248)  // x ���W
#define SCROLL_CONTROL_TOP      (32)   // ��� y ���W
#define SCROLL_CONTROL_BOTTOM   (160)  // ��� y ���W
#define SCROLL_CONTROL_STEP_NUM (TOPIC_ID_NUM)  // �ړ��͈͂̕�����

// �������ڑI���A�C�R��
#define SELECT_ICON_DRAW_OFFSET_X (0)  // �������ڃ{�^������x���W����̃I�t�Z�b�g
#define SELECT_ICON_DRAW_OFFSET_Y (10) // �������ڃ{�^������y���W����̃I�t�Z�b�g


//===============================================================================
// ��BG
//===============================================================================

//-------------
// ���E�B���h�E
//-------------
#define WND0_LEFT   (0)    // ���� x ���W
#define WND0_TOP    (24)   // ���� y ���W
#define WND0_RIGHT  (255)  // �E�� x ���W
#define WND0_BOTTOM (168)  // �E�� y ���W

//----------
// ��SUB-BG
//----------
// BG �t���[��
#define SUB_BG_WINDOW (GFL_BG_FRAME2_S)  // �E�B���h�E��
#define SUB_BG_FONT   (GFL_BG_FRAME3_S)  // �t�H���g��

// �\���D�揇��
#define SUB_BG_PRIORITY_WINDOW (1)  // �E�B���h�E��
#define SUB_BG_PRIORITY_FONT   (0)  // �t�H���g��

// ���u�����f�B���O
#define SUB_BG_BLEND_TARGET_1 (GX_BLEND_PLANEMASK_BG1)  // ���Ώۖ�
#define SUB_BG_BLEND_TARGET_2 (GX_BLEND_PLANEMASK_BG0)  // ���Ώۖ�
#define SUB_BG_BLEND_WEIGHT_1 (7)   // ���Ώۖʂ̃u�����f�B���O�W��
#define SUB_BG_BLEND_WEIGHT_2 (15)  // ���Ώۖʂ̃u�����f�B���O�W��

//-----------
// ��MAIN-BG
//-----------
// BG �t���[��
#define MAIN_BG_BAR    (GFL_BG_FRAME1_M)  // �o�[��
#define MAIN_BG_WINDOW (GFL_BG_FRAME2_M)  // �E�B���h�E��
#define MAIN_BG_FONT   (GFL_BG_FRAME3_M)  // �t�H���g��

// �\���D�揇��
#define MAIN_BG_BAR_PRIORITY    (3)  // �o�[��
#define MAIN_BG_WINDOW_PRIORITY (2)  // �E�B���h�E��
#define MAIN_BG_FONT_PRIORITY   (1)  // �t�H���g��

// ���u�����f�B���O
#define MAIN_BG_BLEND_TARGET_1 (GX_BLEND_PLANEMASK_BG2)  // ���Ώۖ�
#define MAIN_BG_BLEND_TARGET_2 (GX_BLEND_PLANEMASK_BG1)  // ���Ώۖ�
#define MAIN_BG_BLEND_WEIGHT_1 (31)  // ���Ώۖʂ̃u�����f�B���O�W��
#define MAIN_BG_BLEND_WEIGHT_2 (31)  // ���Ώۖʂ̃u�����f�B���O�W��

//----------------
// ���p���b�g�ԍ�
//----------------
// MAIN-BG
#define MAIN_BG_PALETTE_BACK_0     (0x0)
#define MAIN_BG_PALETTE_BACK_1     (0x1)
#define MAIN_BG_PALETTE_BACK_2     (0x2)
#define MAIN_BG_PALETTE_BACK_3     (0x3)
#define MAIN_BG_PALETTE_BACK_4     (0x4)
#define MAIN_BG_PALETTE_BACK_5     (0x5)
#define MAIN_BG_PALETTE_FONT       (0x8)
#define MAIN_BG_PALETTE_WINDOW_OFF (0x9)
#define MAIN_BG_PALETTE_WINDOW_ON  (0xa)
// SUB-BG
#define SUB_BG_PALETTE_BACK_0 (0x0)
#define SUB_BG_PALETTE_BACK_1 (0x1)
#define SUB_BG_PALETTE_BACK_2 (0x2)
#define SUB_BG_PALETTE_BACK_3 (0x3)
#define SUB_BG_PALETTE_BACK_4 (0x4)
#define SUB_BG_PALETTE_BACK_5 (0x5)
#define SUB_BG_PALETTE_WINDOW (0x6)
#define SUB_BG_PALETTE_FONT   (0x7)

//---------------------
// ���E�B���h�E/�{�^��
//---------------------
// ���� �^�C�g�� �E�B���h�E
#define TITLE_WINDOW_X               (2)   // X���W   (�L�����N�^�[�P��)
#define TITLE_WINDOW_Y               (0)   // Y���W   (�L�����N�^�[�P��)
#define TITLE_WINDOW_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define TITLE_WINDOW_HEIGHT          (3)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TITLE_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TITLE_WINDOW_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define TITLE_WINDOW_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define TITLE_WINDOW_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TITLE_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� �������ږ�
#define TOPIC_TITLE_X               (0)   // X���W   (�L�����N�^�[�P��)
#define TOPIC_TITLE_Y               (3)   // Y���W   (�L�����N�^�[�P��)
#define TOPIC_TITLE_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_TITLE_HEIGHT          (3)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_TITLE_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TOPIC_TITLE_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define TOPIC_TITLE_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define TOPIC_TITLE_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TOPIC_TITLE_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� �������ڂ̕⑫��
#define TOPIC_CAPTION_X               (0)   // X���W   (�L�����N�^�[�P��)
#define TOPIC_CAPTION_Y               (6)   // Y���W   (�L�����N�^�[�P��)
#define TOPIC_CAPTION_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_CAPTION_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_CAPTION_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TOPIC_CAPTION_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define TOPIC_CAPTION_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define TOPIC_CAPTION_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TOPIC_CAPTION_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� ����1
#define QUESTION_1_X               (0)   // X���W   (�L�����N�^�[�P��)
#define QUESTION_1_Y               (10)  // Y���W   (�L�����N�^�[�P��)
#define QUESTION_1_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_1_HEIGHT          (2)   // Y�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_1_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define QUESTION_1_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define QUESTION_1_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define QUESTION_1_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define QUESTION_1_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� ����2
#define QUESTION_2_X               (0)   // X���W   (�L�����N�^�[�P��)
#define QUESTION_2_Y               (12)  // Y���W   (�L�����N�^�[�P��)
#define QUESTION_2_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_2_HEIGHT          (2)   // Y�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_2_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define QUESTION_2_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define QUESTION_2_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define QUESTION_2_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define QUESTION_2_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� ����3
#define QUESTION_3_X               (0)   // X���W   (�L�����N�^�[�P��)
#define QUESTION_3_Y               (14)  // Y���W   (�L�����N�^�[�P��)
#define QUESTION_3_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_3_HEIGHT          (2)   // Y�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_3_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define QUESTION_3_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define QUESTION_3_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define QUESTION_3_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define QUESTION_3_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� ������ �E�B���h�E
#define DIRECTION_WINDOW_X               (0)   // X���W   (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_Y               (18)  // Y���W   (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_HEIGHT          (6)   // Y�T�C�Y (�L�����N�^�[�P��)
#define DIRECTION_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define DIRECTION_WINDOW_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define DIRECTION_WINDOW_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define DIRECTION_WINDOW_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define DIRECTION_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� �������ڃ{�^�� ( �擪�̍��� / ��I���� )
#define TOPIC_BUTTON_X               (2)   // X���W   (�L�����N�^�[�P��)
#define TOPIC_BUTTON_Y               (0)   // Y���W   (�L�����N�^�[�P��)
#define TOPIC_BUTTON_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_BUTTON_HEIGHT          (3)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TOPIC_BUTTON_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define TOPIC_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define TOPIC_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TOPIC_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�


//===============================================================================
// ��OBJ
//===============================================================================

//------------------------
// ���Z���A�N�^�[���j�b�g
//------------------------
// SUB-OBJ
#define CLUNIT_SUB_OBJ_WORK_SIZE (10)  // �ő像�[�N��
#define CLUNIT_SUB_OBJ_PRIORITY  (0)   // �`��D�揇��

// MAIN-OBJ
#define CLUNIT_MAIN_OBJ_WORK_SIZE (10)  // �ő像�[�N��
#define CLUNIT_MAIN_OBJ_PRIORITY  (0)   // �`��D�揇��

//----------------------
// ���Z���A�N�^�[���[�N
//----------------------
// �X�N���[���o�[
#define CLWK_SCROLL_CONTROL_POS_X (0)  // x ���W
#define CLWK_SCROLL_CONTROL_POS_Y (0)  // y ���W
#define CLWK_SCROLL_CONTROL_ANIME_SEQ (NANR_obj_bar)  // �A�j���[�V�����V�[�P���X
#define CLWK_SCROLL_CONTROL_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_SCROLL_CONTROL_BG_PRIORITY   (0)  // BG �D�揇��

// �������ڑI���A�C�R��
#define CLWK_SELECT_ICON_POS_X (0)  // x ���W
#define CLWK_SELECT_ICON_POS_Y (0)  // y ���W
#define CLWK_SELECT_ICON_ANIME_SEQ (NANR_obj_search)  // �A�j���[�V�����V�[�P���X
#define CLWK_SELECT_ICON_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_SELECT_ICON_BG_PRIORITY   (0)  // BG �D�揇��
