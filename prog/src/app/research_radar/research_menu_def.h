///////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ������ʂ̒萔��`
 * @file   research_menu_def.h
 * @author obata
 * @date   2010.02.13
 */
///////////////////////////////////////////////////////////////////////
#pragma once


//=====================================================================
// ��BG
//=====================================================================

//---------------------------------------------------------------------
// ��SUB-BG
//---------------------------------------------------------------------
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
//---------------------------------------------------------------------
// ��MAIN-BG
//---------------------------------------------------------------------
// BG �t���[��
#define MAIN_BG_WINDOW (GFL_BG_FRAME2_M)  // �E�B���h�E��
#define MAIN_BG_FONT   (GFL_BG_FRAME3_M)  // �t�H���g��
// �\���D�揇��
#define MAIN_BG_WINDOW_PRIORITY (2)  // �E�B���h�E��
#define MAIN_BG_FONT_PRIORITY   (1)  // �t�H���g��
// ���u�����f�B���O
#define MAIN_BG_BLEND_TARGET_1 (GX_BLEND_PLANEMASK_BG2)  // ���Ώۖ�
#define MAIN_BG_BLEND_TARGET_2 (GX_BLEND_PLANEMASK_BG1)  // ���Ώۖ�
#define MAIN_BG_BLEND_WEIGHT_1 (16)  // ���Ώۖʂ̃u�����f�B���O�W��
#define MAIN_BG_BLEND_WEIGHT_2 (5)   // ���Ώۖʂ̃u�����f�B���O�W��
//---------------------------------------------------------------------
// ���p���b�g�ԍ�
//---------------------------------------------------------------------
// MAIN-BG
#define MAIN_BG_PALETTE_WINDOW_ON         (0xa)
#define MAIN_BG_PALETTE_WINDOW_OFF_CHANGE (0x5)
#define MAIN_BG_PALETTE_WINDOW_OFF_CHECK  (0x6)
#define MAIN_BG_PALETTE_FONT_ACTIVE       (0xe)
#define MAIN_BG_PALETTE_FONT_NOT_ACTIVE   (0xf)
// SUB-BG
#define SUB_BG_PALETTE_BACK_0 (0x0)
#define SUB_BG_PALETTE_BACK_1 (0x1)
#define SUB_BG_PALETTE_BACK_2 (0x2)
#define SUB_BG_PALETTE_BACK_3 (0x3)
#define SUB_BG_PALETTE_BACK_4 (0x4)
#define SUB_BG_PALETTE_BACK_5 (0x5)
#define SUB_BG_PALETTE_WINDOW (0x6)
#define SUB_BG_PALETTE_FONT   (0xf) 
//---------------------------------------------------------------------
// ���X�N���[��
//---------------------------------------------------------------------
// ���� �������E�B���h�E
#define CAPTION_WINDOW_X               (0)   // X���W   (�L�����N�^�[�P��)
#define CAPTION_WINDOW_Y               (17)  // Y���W   (�L�����N�^�[�P��)
#define CAPTION_WINDOW_WIDTH           (32)  // X�T�C�Y (�L�����N�^�[�P��)
#define CAPTION_WINDOW_HEIGHT          (6)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CAPTION_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CAPTION_WINDOW_STRING_OFFSET_Y (16)  // ������̏������ݐ�I�t�Z�b�gY
#define CAPTION_WINDOW_STRING_COLOR_L  (3)   // �t�H���g (����) �̃J���[�ԍ�
#define CAPTION_WINDOW_STRING_COLOR_S  (4)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CAPTION_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
// ����� �^�C�g�� �E�B���h�E
#define TITLE_WINDOW_X               (2)   // X���W   (�L�����N�^�[�P��)
#define TITLE_WINDOW_Y               (0)   // Y���W   (�L�����N�^�[�P��)
#define TITLE_WINDOW_WIDTH           (28)  // X�T�C�Y (�L�����N�^�[�P��)
#define TITLE_WINDOW_HEIGHT          (4)   // Y�T�C�Y (�L�����N�^�[�P��)
#define TITLE_WINDOW_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define TITLE_WINDOW_STRING_OFFSET_Y (4)   // ������̏������ݐ�I�t�Z�b�gY
#define TITLE_WINDOW_STRING_COLOR_L  (3)   // �t�H���g (����) �̃J���[�ԍ�
#define TITLE_WINDOW_STRING_COLOR_S  (4)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define TITLE_WINDOW_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
// ����� �{�^�� ( �������e�̌��� )
#define CHANGE_BUTTON_X               (3)   // X���W   (�L�����N�^�[�P��)
#define CHANGE_BUTTON_Y               (5)   // Y���W   (�L�����N�^�[�P��)
#define CHANGE_BUTTON_WIDTH           (26)  // X�T�C�Y (�L�����N�^�[�P��)
#define CHANGE_BUTTON_HEIGHT          (5)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CHANGE_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CHANGE_BUTTON_STRING_OFFSET_Y (16)  // ������̏������ݐ�I�t�Z�b�gY
#define CHANGE_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CHANGE_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CHANGE_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�
// ����� �{�^�� ( �����񍐂����� )
#define CHECK_BUTTON_X               (3)   // X���W   (�L�����N�^�[�P��)
#define CHECK_BUTTON_Y               (12)  // Y���W   (�L�����N�^�[�P��)
#define CHECK_BUTTON_WIDTH           (26)  // X�T�C�Y (�L�����N�^�[�P��)
#define CHECK_BUTTON_HEIGHT          (5)   // Y�T�C�Y (�L�����N�^�[�P��)
#define CHECK_BUTTON_STRING_OFFSET_X (0)   // ������̏������ݐ�I�t�Z�b�gX
#define CHECK_BUTTON_STRING_OFFSET_Y (16)  // ������̏������ݐ�I�t�Z�b�gY
#define CHECK_BUTTON_STRING_COLOR_L  (1)   // �t�H���g (����) �̃J���[�ԍ�
#define CHECK_BUTTON_STRING_COLOR_S  (2)   // �t�H���g (�e)�@ �̃J���[�ԍ�
#define CHECK_BUTTON_STRING_COLOR_B  (0)   // �t�H���g (�w�i) �̃J���[�ԍ�


//=====================================================================
// ��OBJ
//=====================================================================

//---------------------------------------------------------------------
// ���Z���A�N�^�[���j�b�g
//---------------------------------------------------------------------
// SUB-OBJ
#define CLUNIT_SUB_OBJ_WORK_SIZE (10)  // �ő像�[�N��
#define CLUNIT_SUB_OBJ_PRIORITY  (0)   // �`��D�揇��
// MAIN-OBJ
#define CLUNIT_MAIN_OBJ_WORK_SIZE (10)  // �ő像�[�N��
#define CLUNIT_MAIN_OBJ_PRIORITY  (0)   // �`��D�揇��
//---------------------------------------------------------------------
// ���Z���A�N�^�[���[�N
//---------------------------------------------------------------------
// "new" �A�C�R��
#define CLWK_NEW_ICON_POS_X (32)   // x ���W
#define CLWK_NEW_ICON_POS_Y (104)  // y ���W
#define CLWK_NEW_ICON_ANIME_SEQ (NANR_obj_new)  // �A�j���[�V�����V�[�P���X
#define CLWK_NEW_ICON_SOFT_PRIORITY (0)  // �\�t�g�D�揇�� 0>0xff
#define CLWK_NEW_ICON_BG_PRIORITY   (0)  // BG �D�揇��
//---------------------------------------------------------------------
// ���ʐM�A�C�R��
//---------------------------------------------------------------------
#define WIRELESS_ICON_X (240)
#define WIRELESS_ICON_Y (0)

//=====================================================================
// ���p���b�g�A�j���[�V����
//=====================================================================
//---------------------------------------------------------------------
// �����j���[���ڃJ�[�\��ON�̖���
//---------------------------------------------------------------------
// �A�j���Ώې擪�J���[�ԍ� ( �p���b�g�擪����̃I�t�Z�b�g )
#define PALETTE_ANIME_CURSOR_ON_COLOR_OFFSET (3)
// �A�j���Ώۂ̃J���[��
#define PALETTE_ANIME_CURSOR_ON_COLOR_NUM (3)
//---------------------------------------------------------------------
// �����j���[���ڃJ�[�\���Z�b�g���̓_��
//---------------------------------------------------------------------
// �A�j���Ώې擪�J���[�ԍ� ( �p���b�g�擪����̃I�t�Z�b�g )
#define PALETTE_ANIME_CURSOR_SET_COLOR_OFFSET (6)
// �A�j���Ώۂ̃J���[��
#define PALETTE_ANIME_CURSOR_SET_COLOR_NUM (1)
//---------------------------------------------------------------------
// �����j���[���ڑI�����̖���
//---------------------------------------------------------------------
// �A�j���Ώې擪�J���[�ԍ� ( �p���b�g�擪����̃I�t�Z�b�g )
#define PALETTE_ANIME_SELECT_COLOR_OFFSET (6)
// �A�j���Ώۂ̃J���[��
#define PALETTE_ANIME_SELECT_COLOR_NUM (1)


//=====================================================================
// ���^�b�`�͈�
//=====================================================================
//---------------------------------------------------------------------
// ���u�������e��ύX����v
//---------------------------------------------------------------------
// X���W (�h�b�g�P��)
#define TOUCH_AREA_CHANGE_BUTTON_X (CHANGE_BUTTON_X * DOT_PER_CHARA)
// Y���W (�h�b�g�P��)
#define TOUCH_AREA_CHANGE_BUTTON_Y (CHANGE_BUTTON_Y * DOT_PER_CHARA)      
// X�T�C�Y (�h�b�g�P��)
#define TOUCH_AREA_CHANGE_BUTTON_WIDTH (CHANGE_BUTTON_WIDTH * DOT_PER_CHARA)  
// Y�T�C�Y (�h�b�g�P��)
#define TOUCH_AREA_CHANGE_BUTTON_HEIGHT (CHANGE_BUTTON_HEIGHT * DOT_PER_CHARA) 
//---------------------------------------------------------------------
// ���u�����񍐂��m�F����v
//---------------------------------------------------------------------
// X���W   (�h�b�g�P��)
#define TOUCH_AREA_CHECK_BUTTON_X (CHECK_BUTTON_X * DOT_PER_CHARA)
// Y���W   (�h�b�g�P��)
#define TOUCH_AREA_CHECK_BUTTON_Y (CHECK_BUTTON_Y * DOT_PER_CHARA)
// X�T�C�Y (�h�b�g�P��)
#define TOUCH_AREA_CHECK_BUTTON_WIDTH  (CHECK_BUTTON_WIDTH * DOT_PER_CHARA)
// Y�T�C�Y (�h�b�g�P��)
#define TOUCH_AREA_CHECK_BUTTON_HEIGHT (CHECK_BUTTON_HEIGHT * DOT_PER_CHARA)


//=====================================================================
// ������
//=====================================================================
#define SEQ_QUEUE_SIZE (10) // �V�[�P���X�L���[�̃T�C�Y
#define WAIT_FRAME_BUTTON (15) // �{�^�������������̑҂��t���[����
