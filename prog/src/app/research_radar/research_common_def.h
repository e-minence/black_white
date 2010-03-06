/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX��� �萔��`
 * @file   research_common_def.h
 * @author obata
 * @date   2010.02.15
 */
/////////////////////////////////////////////////////////////////////////////////
#pragma once 


#define PRINT_TARGET (2)  // �f�o�b�O���̏o�͐�

#define DOT_PER_CHARA     (8)                      // 1�L���� = 8�h�b�g
#define ONE_CHARA_SIZE    (0x20)                   // 1�L�����N�^�[�f�[�^�̃T�C�Y [Byte]
#define ONE_COLOR_SIZE    (2)                      // 1�J���[�f�[�^�̃T�C�Y [Byte]
#define ONE_PALETTE_SIZE  (0x20)                   // 1�p���b�g(16�F��)�̃T�C�Y [Byte]
#define FULL_PALETTE_SIZE (ONE_PALETTE_SIZE * 16)  // 16�p���b�g�̃T�C�Y [Byte]


//----------
// ��SUB-BG
//----------
// �w�i��
#define SUB_BG_BACK (GFL_BG_FRAME0_S)  // BG �t���[��
#define SUB_BG_BACK_PRIORITY      (3)  // �\���D�揇��
#define SUB_BG_BACK_FIRST_PLT_IDX (0)  // �擪�p���b�g�ԍ�
#define SUB_BG_BACK_PLT_NUM       (1)  // �g�p�p���b�g��
// ���[�_�[��
#define SUB_BG_RADAR (GFL_BG_FRAME1_S)    // BG �t���[��
#define SUB_BG_RADAR_PRIORITY        (2)  // �\���D�揇��
#define SUB_BG_RADAR_FIRST_PLT_IDX   (0)  // �擪�p���b�g�ԍ�
#define SUB_BG_RADAR_PLT_NUM         (6)  // �g�p�p���b�g��
#define SUB_BG_RADAR_PLT_ANIME_FRAME (10) // �p���b�g�A�j���[�V�����̍X�V�Ԋu[frame]


//-----------
// ��MAIN-BG
//-----------
// �w�i��
#define MAIN_BG_BACK (GFL_BG_FRAME1_M)  // �w�iBG��
#define MAIN_BG_BACK_PRIORITY      (3)  // �w�iBG�ʂ̕\���D�揇��
#define MAIN_BG_BACK_FIRST_PLT_IDX (0)  // �w�iBG�ʂ̐擪�p���b�g�ԍ�
#define MAIN_BG_BACK_PLT_NUM       (1)  // �w�iBG�ʂ̃p���b�g��


//------------------------
// ���Z���A�N�^�[���j�b�g
//------------------------
// MAIN-OBJ
#define COMMON_CLUNIT_MAIN_OBJ_WORK_SIZE (1)  // �ő像�[�N��
#define COMMON_CLUNIT_MAIN_OBJ_PRIORITY  (2)  // �`��D�揇��


//-----------------------
// ���Z���A�N�^�[���[�N
//-----------------------

//�u���ǂ�v�{�^��
#define RETURN_BUTTON_X      (27) // X���W ( �L�����N�^�P�� )
#define RETURN_BUTTON_Y      (21) // Y���W ( �L�����N�^�P�� )
#define RETURN_BUTTON_WIDTH  (3)  // ��   ( �L�����N�^�P�� )
#define RETURN_BUTTON_HEIGHT (3)  // ���� ( �L�����N�^�P�� )

#define TOUCH_AREA_RETURN_BUTTON_X (RETURN_BUTTON_X * DOT_PER_CHARA) // X���W ( �h�b�g�P�� )
#define TOUCH_AREA_RETURN_BUTTON_Y (RETURN_BUTTON_Y * DOT_PER_CHARA) // Y���W ( �h�b�g�P�� )
#define TOUCH_AREA_RETURN_BUTTON_WIDTH  (RETURN_BUTTON_WIDTH * DOT_PER_CHARA) // X�T�C�Y ( �h�b�g�P�� )
#define TOUCH_AREA_RETURN_BUTTON_HEIGHT (RETURN_BUTTON_HEIGHT * DOT_PER_CHARA) // Y�T�C�Y ( �h�b�g�P�� )

#define CLWK_RETURN_POS_X (RETURN_BUTTON_X * DOT_PER_CHARA) // x ���W ( �h�b�g�P�� )
#define CLWK_RETURN_POS_Y (RETURN_BUTTON_Y * DOT_PER_CHARA) // y ���W ( �h�b�g�P�� )
#define CLWK_RETURN_ANIME_SEQ     (1) // �A�j���[�V�����V�[�P���X
#define CLWK_RETURN_SOFT_PRIORITY (3) // �\�t�g�D�揇��
#define CLWK_RETURN_BG_PRIORITY   (1) // BG �D�揇��
