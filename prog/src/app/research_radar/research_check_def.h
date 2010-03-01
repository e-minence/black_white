/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �����񍐊m�F��� �C���f�b�N�X��`
 * @file   research_check_def.h
 * @author obata
 * @date   2010.02.20
 */
/////////////////////////////////////////////////////////////////////////////////
#pragma once


//===============================================================================
// ��BG
//===============================================================================

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
#define MAIN_BG_3D     (GFL_BG_FRAME0_M)  // 3D��
#define MAIN_BG_WINDOW (GFL_BG_FRAME2_M)  // �E�B���h�E��
#define MAIN_BG_FONT   (GFL_BG_FRAME3_M)  // �t�H���g��

// �\���D�揇��
#define MAIN_BG_3D_PRIORITY     (2)  // 3D��
#define MAIN_BG_WINDOW_PRIORITY (1)  // �E�B���h�E��
#define MAIN_BG_FONT_PRIORITY   (0)  // �t�H���g��

// ���u�����f�B���O
#define MAIN_BG_BLEND_TARGET_1 (GX_BLEND_PLANEMASK_BG2)  // ���Ώۖ�
#define MAIN_BG_BLEND_TARGET_2 (GX_BLEND_PLANEMASK_BG1)  // ���Ώۖ�
#define MAIN_BG_BLEND_WEIGHT_1 (16)  // ���Ώۖʂ̃u�����f�B���O�W��
#define MAIN_BG_BLEND_WEIGHT_2 (5)  // ���Ώۖʂ̃u�����f�B���O�W��

//----------------
// ���p���b�g�ԍ�
//----------------
// MAIN-BG
#define MAIN_BG_PALETTE_BACK_0   (0x0)
#define MAIN_BG_PALETTE_BACK_1   (0x1)
#define MAIN_BG_PALETTE_BACK_2   (0x2)
#define MAIN_BG_PALETTE_BACK_3   (0x3)
#define MAIN_BG_PALETTE_BACK_4   (0x4)
#define MAIN_BG_PALETTE_BACK_5   (0x5)
#define MAIN_BG_PALETTE_MENU_OFF (0x6)
#define MAIN_BG_PALETTE_MENU_ON  (0xa)
#define MAIN_BG_PALETTE_FONT     (0xf)
// SUB-BG
#define SUB_BG_PALETTE_BACK_0 (0x0)
#define SUB_BG_PALETTE_BACK_1 (0x1)
#define SUB_BG_PALETTE_BACK_2 (0x2)
#define SUB_BG_PALETTE_BACK_3 (0x3)
#define SUB_BG_PALETTE_BACK_4 (0x4)
#define SUB_BG_PALETTE_BACK_5 (0x5)
#define SUB_BG_PALETTE_WINDOW (0x6)
#define SUB_BG_PALETTE_FONT   (0xf)

//---------------------
// ���E�B���h�E/�{�^��
//---------------------
// ���� �^�C�g��
#define TITLE_X               (2)   // X���W   (�L�����N�^�[�P��)
#define TITLE_Y               (0)   // Y���W   (�L�����N�^�[�P��)
#define TITLE_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define TITLE_HEIGHT          (3)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TITLE_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TITLE_STRING_OFFSET_Y (4)   // ������̏������ݐ�I�t�Z�b�gY
#define TITLE_STRING_COLOR_L  (3)   // �t�H���g (����) �̃J���[�ԍ�
#define TITLE_STRING_COLOR_S  (4)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TITLE_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� �������ڂ̕⑫��
#define TOPIC_CAPTION_X               (0)   // X���W   (�L�����N�^�[�P��)
#define TOPIC_CAPTION_Y               (18)  // Y���W   (�L�����N�^�[�P��)
#define TOPIC_CAPTION_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_CAPTION_HEIGHT          (2)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TOPIC_CAPTION_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TOPIC_CAPTION_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define TOPIC_CAPTION_STRING_COLOR_L  (3)   // �t�H���g (����) �̃J���[�ԍ�
#define TOPIC_CAPTION_STRING_COLOR_S  (4)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TOPIC_CAPTION_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ���� ����̕⑫��
#define QUESTION_CAPTION_X               (0)   // X���W   (�L�����N�^�[�P��)
#define QUESTION_CAPTION_Y               (20)  // Y���W   (�L�����N�^�[�P��)
#define QUESTION_CAPTION_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_CAPTION_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_CAPTION_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define QUESTION_CAPTION_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define QUESTION_CAPTION_STRING_COLOR_L  (3)   // �t�H���g (����) �̃J���[�ԍ�
#define QUESTION_CAPTION_STRING_COLOR_S  (4)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define QUESTION_CAPTION_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� ����
#define QUESTION_X               (2)   // X���W   (�L�����N�^�[�P��)
#define QUESTION_Y               (0)   // Y���W   (�L�����N�^�[�P��)
#define QUESTION_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define QUESTION_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define QUESTION_STRING_OFFSET_Y (5)   // ������̏������ݐ�I�t�Z�b�gY
#define QUESTION_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define QUESTION_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define QUESTION_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
#define QUESTION_LEFT_CURSOR_OFFSET_X  (0) // ���J�[�\�� x �I�t�Z�b�g
#define QUESTION_LEFT_CURSOR_OFFSET_Y  (12) // ���J�[�\�� y �I�t�Z�b�g
#define QUESTION_RIGHT_CURSOR_OFFSET_X (0) // �E�J�[�\�� x �I�t�Z�b�g
#define QUESTION_RIGHT_CURSOR_OFFSET_Y (12) // �E�J�[�\�� y �I�t�Z�b�g

// ����� �� ( �㉺2���� )
#define ANSWER_UPPER_X         (13)  // X���W   (�L�����N�^�[�P��, �㔼��)
#define ANSWER_UPPER_Y         (4)   // Y���W   (�L�����N�^�[�P��, �㔼��)
#define ANSWER_UPPER_WIDTH     (19)  // X�T�C�Y (�L�����N�^�[�P��, �㔼��)
#define ANSWER_UPPER_HEIGHT    (4)   // Y�T�C�Y (�L�����N�^�[�P��, �㔼��)
#define ANSWER_LOWER_X         (16)  // X���W   (�L�����N�^�[�P��, ������)
#define ANSWER_LOWER_Y         (8)   // Y���W   (�L�����N�^�[�P��, ������)
#define ANSWER_LOWER_WIDTH     (16)  // X�T�C�Y (�L�����N�^�[�P��, ������)
#define ANSWER_LOWER_HEIGHT    (3)   // Y�T�C�Y (�L�����N�^�[�P��, ������)
#define ANSWER_STRING_OFFSET_X (24)  // ������̏������ݐ�I�t�Z�b�gX
#define ANSWER_STRING_OFFSET_Y (4)   // ������̏������ݐ�I�t�Z�b�gY
#define ANSWER_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define ANSWER_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define ANSWER_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
#define ANSWER_LEFT_CURSOR_OFFSET_X  (0)  // ���J�[�\�� x �I�t�Z�b�g
#define ANSWER_LEFT_CURSOR_OFFSET_Y  (32) // ���J�[�\�� y �I�t�Z�b�g
#define ANSWER_RIGHT_CURSOR_OFFSET_X (0)  // �E�J�[�\�� x �I�t�Z�b�g
#define ANSWER_RIGHT_CURSOR_OFFSET_Y (32) // �E�J�[�\�� y �I�t�Z�b�g

// ����� �����̉�
#define MY_ANSWER_X               (17)  // X���W   (�L�����N�^�[�P��)
#define MY_ANSWER_Y               (11)  // Y���W   (�L�����N�^�[�P��)
#define MY_ANSWER_WIDTH           (15)  // X�T�C�Y (�L�����N�^�[�P��)
#define MY_ANSWER_HEIGHT          (5)   // Y�T�C�Y (�L�����N�^�[�P��)
#define MY_ANSWER_STRING_OFFSET_X (12)  // ������̏������ݐ�I�t�Z�b�gX
#define MY_ANSWER_STRING_OFFSET_Y (4)   // ������̏������ݐ�I�t�Z�b�gY
#define MY_ANSWER_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define MY_ANSWER_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define MY_ANSWER_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
#define MY_ANSWER_LEFT_CURSOR_OFFSET_X  (256) // ���J�[�\�� x �I�t�Z�b�g
#define MY_ANSWER_LEFT_CURSOR_OFFSET_Y  (256) // ���J�[�\�� y �I�t�Z�b�g
#define MY_ANSWER_RIGHT_CURSOR_OFFSET_X (256) // �E�J�[�\�� x �I�t�Z�b�g
#define MY_ANSWER_RIGHT_CURSOR_OFFSET_Y (256) // �E�J�[�\�� y �I�t�Z�b�g

// ����� �񓚐l��
#define COUNT_X               (13)  // X���W   (�L�����N�^�[�P��)
#define COUNT_Y               (16)  // Y���W   (�L�����N�^�[�P��)
#define COUNT_WIDTH           (19)  // X�T�C�Y (�L�����N�^�[�P��)
#define COUNT_HEIGHT          (5)   // Y�T�C�Y (�L�����N�^�[�P��)
#define COUNT_STRING_OFFSET_X (34)  // ������̏������ݐ�I�t�Z�b�gX
#define COUNT_STRING_OFFSET_Y (4)   // ������̏������ݐ�I�t�Z�b�gY
#define COUNT_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define COUNT_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define COUNT_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
#define COUNT_LEFT_CURSOR_OFFSET_X  (0)  // ���J�[�\�� x �I�t�Z�b�g
#define COUNT_LEFT_CURSOR_OFFSET_Y  (20) // ���J�[�\�� y �I�t�Z�b�g
#define COUNT_RIGHT_CURSOR_OFFSET_X (0)  // �E�J�[�\�� x �I�t�Z�b�g
#define COUNT_RIGHT_CURSOR_OFFSET_Y (20) // �E�J�[�\�� y �I�t�Z�b�g

// ����ʁu�������܂��傤�����イ�v
#define NO_DATA_X               (2)   // X���W   (�L�����N�^�[�P��)
#define NO_DATA_Y               (7)   // Y���W   (�L�����N�^�[�P��)
#define NO_DATA_WIDTH           (13)  // X�T�C�Y (�L�����N�^�[�P��)
#define NO_DATA_HEIGHT          (5)   // Y�T�C�Y (�L�����N�^�[�P��)
#define NO_DATA_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define NO_DATA_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define NO_DATA_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define NO_DATA_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define NO_DATA_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����ʁu�f�[�^���サ�񂿂イ�v
#define DATA_RECEIVING_X               (13)  // X���W   (�L�����N�^�[�P��)
#define DATA_RECEIVING_Y               (4)   // Y���W   (�L�����N�^�[�P��)
#define DATA_RECEIVING_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define DATA_RECEIVING_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define DATA_RECEIVING_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define DATA_RECEIVING_STRING_OFFSET_Y (4)   // ������̏������ݐ�I�t�Z�b�gY
#define DATA_RECEIVING_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define DATA_RECEIVING_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define DATA_RECEIVING_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

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
#define CLUNIT_MAIN_OBJ_PRIORITY  (1)   // �`��D�揇��

// BMP-OAM
#define CLUNIT_BMPOAM_WORK_SIZE (30)  // �ő像�[�N��
#define CLUNIT_BMPOAM_PRIORITY  (0)   // �`��D�揇��

//----------------------
// ���Z���A�N�^�[���[�N
//----------------------
// ���J�[�\��
#define CLWK_CONTROL_CURSOR_L_POS_X (0)  // x ���W
#define CLWK_CONTROL_CURSOR_L_POS_Y (0)  // y ���W
#define CLWK_CONTROL_CURSOR_L_WIDTH (16) // ��
#define CLWK_CONTROL_CURSOR_L_HEIGHT (16) // ����
#define CLWK_CONTROL_CURSOR_L_ANIME_SEQ (NANR_obj_l)  // �A�j���[�V�����V�[�P���X
#define CLWK_CONTROL_CURSOR_L_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_CONTROL_CURSOR_L_BG_PRIORITY   (0)  // BG �D�揇��
// �E�J�[�\��
#define CLWK_CONTROL_CURSOR_R_POS_X (0)  // x ���W
#define CLWK_CONTROL_CURSOR_R_POS_Y (0)  // y ���W
#define CLWK_CONTROL_CURSOR_R_WIDTH (16) // ��
#define CLWK_CONTROL_CURSOR_R_HEIGHT (16) // ����
#define CLWK_CONTROL_CURSOR_R_ANIME_SEQ (NANR_obj_r)  // �A�j���[�V�����V�[�P���X
#define CLWK_CONTROL_CURSOR_R_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_CONTROL_CURSOR_R_BG_PRIORITY   (0)  // BG �D�揇��

// �����̉񓚃A�C�R��
#define CLWK_MY_ANSWER_ICON_POS_X (0)  // x ���W
#define CLWK_MY_ANSWER_ICON_POS_Y (0)  // y ���W
#define CLWK_MY_ANSWER_ICON_ANIME_SEQ (NANR_obj_me)  // �A�j���[�V�����V�[�P���X
#define CLWK_MY_ANSWER_ICON_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_MY_ANSWER_ICON_BG_PRIORITY   (0)  // BG �D�揇��

//-------------------
// ��BMP-OAM �A�N�^�[
//-------------------
//�u�c�����������イ�c�v
#define BMPOAM_ACTOR_ANALYZING_CHARA_SIZE_X   (32)  // x �T�C�Y ( �L�����P�� )
#define BMPOAM_ACTOR_ANALYZING_CHARA_SIZE_Y   (5)   // y �T�C�Y ( �L�����P�� )
#define BMPOAM_ACTOR_ANALYZING_POS_X          (0)   // ����x���W
#define BMPOAM_ACTOR_ANALYZING_POS_Y          (76)  // ����y���W
#define BMPOAM_ACTOR_ANALYZING_PALETTE_OFFSET (1)   // �p���b�g�I�t�Z�b�g
#define BMPOAM_ACTOR_ANALYZING_SOFT_PRIORITY  (0)   // �\�t�g�v���C�I���e�B
#define BMPOAM_ACTOR_ANALYZING_BG_PRIORITY    (0)   // BG�v���C�I���e�B
#define BMPOAM_ACTOR_ANALYZING_STRING_OFFSET_X (8)  // �����̏������ݐ� x ���W�I�t�Z�b�g
#define BMPOAM_ACTOR_ANALYZING_STRING_OFFSET_Y (9)  // �����̏������ݐ� y ���W�I�t�Z�b�g
#define BMPOAM_ACTOR_ANALYZING_STRING_COLOR_L (1)   // �����̃J���[�ԍ�
#define BMPOAM_ACTOR_ANALYZING_STRING_COLOR_S (2)   // �e�̃J���[�ԍ�
#define BMPOAM_ACTOR_ANALYZING_STRING_COLOR_B (0)   // �w�i�̃J���[�ԍ�


//===============================================================================
// ������
//===============================================================================
#define SEQ_QUEUE_SIZE (5)  // �V�[�P���X�L���[�̃T�C�Y

// �V�[�P���X�̓���t���[����
#define SEQ_ANALYZE_FRAMES (120) // RESEARCH_CHECK_SEQ_ANALYZE
#define SEQ_FLASH_FRAMES   (30)  // RESEARCH_CHECK_SEQ_FLASH
#define SEQ_UPDATE_FRAMES  (60)  // RESEARCH_CHECK_SEQ_UPDATE

// �p���b�g�t�F�[�h ( �A�E�g )
#define MAIN_BG_PALETTE_FADE_OUT_TARGET_BITMASK (0xffff) // �t�F�[�h�Ώۃp���b�g�ԍ��̃}�X�N
#define MAIN_BG_PALETTE_FADE_OUT_WAIT           (2)      // �t�F�[�h�v�Z�҂�����
#define MAIN_BG_PALETTE_FADE_OUT_START_STRENGTH (0)      // �t�F�[�h�����Z�x [0, 16]
#define MAIN_BG_PALETTE_FADE_OUT_END_STRENGTH   (10)     // �t�F�[�h�ŏI�Z�x [0, 16]
#define MAIN_BG_PALETTE_FADE_OUT_COLOR          (0x0000) // �ύX��̐F

#define MAIN_OBJ_PALETTE_FADE_OUT_TARGET_BITMASK (0x0001) // �t�F�[�h�Ώۃp���b�g�ԍ��̃}�X�N
#define MAIN_OBJ_PALETTE_FADE_OUT_WAIT           (2)      // �t�F�[�h�v�Z�҂�����
#define MAIN_OBJ_PALETTE_FADE_OUT_START_STRENGTH (0)      // �t�F�[�h�����Z�x [0, 16]
#define MAIN_OBJ_PALETTE_FADE_OUT_END_STRENGTH   (10)     // �t�F�[�h�ŏI�Z�x [0, 16]
#define MAIN_OBJ_PALETTE_FADE_OUT_COLOR          (0x0000) // �ύX��̐F

// �p���b�g�t�F�[�h ( �C�� )
#define MAIN_BG_PALETTE_FADE_IN_TARGET_BITMASK (0xffff)  // �t�F�[�h�Ώۃp���b�g�ԍ��̃}�X�N
#define MAIN_BG_PALETTE_FADE_IN_WAIT           (2)       // �t�F�[�h�v�Z�҂�����
#define MAIN_BG_PALETTE_FADE_IN_START_STRENGTH (10)      // �t�F�[�h�����Z�x [0, 16]
#define MAIN_BG_PALETTE_FADE_IN_END_STRENGTH   (0)       // �t�F�[�h�ŏI�Z�x [0, 16]
#define MAIN_BG_PALETTE_FADE_IN_COLOR          (0x0000)  // �ύX��̐F

#define MAIN_OBJ_PALETTE_FADE_IN_TARGET_BITMASK (0x0001)  // �t�F�[�h�Ώۃp���b�g�ԍ��̃}�X�N
#define MAIN_OBJ_PALETTE_FADE_IN_WAIT           (2)       // �t�F�[�h�v�Z�҂�����
#define MAIN_OBJ_PALETTE_FADE_IN_START_STRENGTH (10)      // �t�F�[�h�����Z�x [0, 16]
#define MAIN_OBJ_PALETTE_FADE_IN_END_STRENGTH   (0)       // �t�F�[�h�ŏI�Z�x [0, 16]
#define MAIN_OBJ_PALETTE_FADE_IN_COLOR          (0x0000)  // �ύX��̐F

//===============================================================================
// ���^�b�`�͈�
//===============================================================================
// ����
#define TOUCH_AREA_QUESTION_X      (QUESTION_X * DOT_PER_CHARA)      // X���W   (�h�b�g�P��)
#define TOUCH_AREA_QUESTION_Y      (QUESTION_Y * DOT_PER_CHARA)      // Y���W   (�h�b�g�P��)
#define TOUCH_AREA_QUESTION_WIDTH  (QUESTION_WIDTH * DOT_PER_CHARA)  // X�T�C�Y (�h�b�g�P��)
#define TOUCH_AREA_QUESTION_HEIGHT (QUESTION_HEIGHT * DOT_PER_CHARA) // Y�T�C�Y (�h�b�g�P��)

// �J�[�\��
#define TOUCH_AREA_CONTROL_CURSOR_WIDTH  (16) // X�T�C�Y (�h�b�g�P��)
#define TOUCH_AREA_CONTROL_CURSOR_HEIGHT (16) // Y�T�C�Y (�h�b�g�P��)

//===============================================================================
// �����
//===============================================================================
#define ARROW_START_X (120) // ���̎n�_x���W
#define ARROW_START_Y (40)  // ���̎n�_y���W
