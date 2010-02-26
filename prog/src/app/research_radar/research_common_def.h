/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ �������e�ύX��� �C���f�b�N�X��`
 * @file   research_select_def.h
 * @author obata
 * @date   2010.02.15
 */
/////////////////////////////////////////////////////////////////////////////////
#pragma once


//------------------------
// ���Z���A�N�^�[���j�b�g
//------------------------
// MAIN-OBJ
#define COMMON_CLUNIT_MAIN_OBJ_WORK_SIZE (10) // �ő像�[�N��
#define COMMON_CLUNIT_MAIN_OBJ_PRIORITY  (1)  // �`��D�揇��

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
#define CLWK_RETURN_SOFT_PRIORITY (0) // �\�t�g�D�揇��
#define CLWK_RETURN_BG_PRIORITY   (0) // BG �D�揇��
