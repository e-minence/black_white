/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���ʃw�b�_
 * @file   research_common.h
 * @author obata
 * @date   2010.02.06
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>


//=============================================================================== 
// ���萔
//=============================================================================== 
#define PRINT_TARGET (2)  // �f�o�b�O�o�͐�

#define ONE_CHARA_SIZE    (0x20)                   // 1�L�����N�^�[�f�[�^�̃T�C�Y [Byte]
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
#define SUB_BG_RADAR_FIRST_PLT_IDX   (1)  // �擪�p���b�g�ԍ�
#define SUB_BG_RADAR_PLT_NUM         (6)  // �g�p�p���b�g��
#define SUB_BG_RADAR_PLT_ANIME_FRAME (30) // �p���b�g�A�j���[�V�����̍X�V�Ԋu[frame]

//-----------
// ��MAIN-BG
//-----------
// �w�i��
#define MAIN_BG_BACK (GFL_BG_FRAME1_M)  // �w�iBG��
#define MAIN_BG_BACK_PRIORITY      (3)  // �w�iBG�ʂ̕\���D�揇��
#define MAIN_BG_BACK_FIRST_PLT_IDX (0)  // �w�iBG�ʂ̐擪�p���b�g�ԍ�
#define MAIN_BG_BACK_PLT_NUM       (1)  // �w�iBG�ʂ̃p���b�g��

// VRAM-Bank �ݒ�
extern const GFL_DISP_VRAM VRAMBankSettings;

// BG���[�h�ݒ�
extern const GFL_BG_SYS_HEADER BGSysHeader2D;
extern const GFL_BG_SYS_HEADER BGSysHeader3D;
