/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���ʃw�b�_
 * @file   research_common.h
 * @author obata
 * @date   2010.02.06
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once


//=============================================================================== 
// ���萔
//=============================================================================== 
#define PRINT_TARGET (2)  // �f�o�b�O�o�͐�

#define ONE_CHARA_SIZE    (0x20)                   // 1�L�����N�^�[�f�[�^�̃T�C�Y [Byte]
#define ONE_PALETTE_SIZE  (0x20)                   // 1�p���b�g(16�F��)�̃T�C�Y [Byte]
#define FULL_PALETTE_SIZE (ONE_PALETTE_SIZE * 16)  // 16�p���b�g�̃T�C�Y [Byte]

// ����
#define SUB_BG_BACK_GROUND (GFL_BG_FRAME0_S)  // �w�iBG��
#define SUB_BG_BACK_GROUND_PRIORITY      (3)  // �w�iBG�ʂ̕\���D�揇��
#define SUB_BG_BACK_GROUND_FIRST_PLT_IDX (0)  // �w�iBG�ʂ̐擪�p���b�g�ԍ�
#define SUB_BG_BACK_GROUND_PLT_NUM       (6)  // �w�iBG�ʂ̃p���b�g��

// �����
#define MAIN_BG_BACK_GROUND (GFL_BG_FRAME1_M)  // �w�iBG��
#define MAIN_BG_BACK_GROUND_PRIORITY      (3)  // �w�iBG�ʂ̕\���D�揇��
#define MAIN_BG_BACK_GROUND_FIRST_PLT_IDX (0)  // �w�iBG�ʂ̐擪�p���b�g�ԍ�
#define MAIN_BG_BACK_GROUND_PLT_NUM       (6)  // �w�iBG�ʂ̃p���b�g��
