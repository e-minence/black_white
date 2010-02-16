/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������ʂ̒萔��`
 * @file   research_menu_def.h
 * @author obata
 * @date   2010.02.13
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
#define MAIN_BG_WINDOW (GFL_BG_FRAME2_M)  // �E�B���h�E��
#define MAIN_BG_FONT   (GFL_BG_FRAME3_M)  // �t�H���g��

// �\���D�揇��
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
#define MAIN_BG_PALETTE_WINDOW_ON  (0xa)
#define MAIN_BG_PALETTE_WINDOW_OFF (0x7)
#define MAIN_BG_PALETTE_FONT       (0x8)
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
// ���� �������E�B���h�E
#define CAPTION_WINDOW_X               (0)   // X���W   (�L�����N�^�[�P��)
#define CAPTION_WINDOW_Y               (17)  // Y���W   (�L�����N�^�[�P��)
#define CAPTION_WINDOW_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define CAPTION_WINDOW_HEIGHT          (6)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CAPTION_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CAPTION_WINDOW_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define CAPTION_WINDOW_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CAPTION_WINDOW_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CAPTION_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� �^�C�g�� �E�B���h�E
#define TITLE_WINDOW_X               (2)   // X���W   (�L�����N�^�[�P��)
#define TITLE_WINDOW_Y               (0)   // Y���W   (�L�����N�^�[�P��)
#define TITLE_WINDOW_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define TITLE_WINDOW_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TITLE_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TITLE_WINDOW_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define TITLE_WINDOW_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define TITLE_WINDOW_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TITLE_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� �{�^�� ( �������e�̌��� )
#define CHANGE_BUTTON_X               (3)   // X���W   (�L�����N�^�[�P��)
#define CHANGE_BUTTON_Y               (6)   // Y���W   (�L�����N�^�[�P��)
#define CHANGE_BUTTON_WIDTH           (26)  // X�T�C�Y (�L�����N�^�[�P��)
#define CHANGE_BUTTON_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CHANGE_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CHANGE_BUTTON_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define CHANGE_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CHANGE_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CHANGE_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�

// ����� �{�^�� ( �����񍐂����� )
#define CHECK_BUTTON_X               (3)   // X���W   (�L�����N�^�[�P��)
#define CHECK_BUTTON_Y               (13)  // Y���W   (�L�����N�^�[�P��)
#define CHECK_BUTTON_WIDTH           (26)  // X�T�C�Y (�L�����N�^�[�P��)
#define CHECK_BUTTON_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CHECK_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CHECK_BUTTON_STRING_OFFSET_Y (0)   // ������̏������ݐ�I�t�Z�b�gY
#define CHECK_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CHECK_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CHECK_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�


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
// "new" �A�C�R��
#define CLWK_NEW_ICON_POS_X (32)   // x ���W
#define CLWK_NEW_ICON_POS_Y (104)  // y ���W
#define CLWK_NEW_ICON_ANIME_SEQ (NANR_obj_new)  // �A�j���[�V�����V�[�P���X
#define CLWK_NEW_ICON_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_NEW_ICON_BG_PRIORITY   (0)  // BG �D�揇��
