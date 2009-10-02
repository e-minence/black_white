//=============================================================================
/**
 * @file	  ircpokemontrade_anim.h
 * @bfief	  �|�P�������� �A�j���[�V�����Ԋu��`
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once


// �������Œ������Ă���͂�߂�
#define ALL_SLOW_SPEED_NUM (1)    ///< ?�{������


#define ANIM_SPEED  (FX32_ONE / ALL_SLOW_SPEED_NUM)

#define _BALL_PARTICLE_START   (120)  ///�����X�^�[�{�[�����ł������� �p�[�e�B�N���J�n
#define _POKEUP_WHITEOUT_START (240)  //�����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g
#define _POKEUP_WHITEOUT_TIMER (14)  //�����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g����
#define _POKEUP_WHITEIN_START  (255)  //�|�P���������V�[���O�̃t�F�[�h�C��
#define _POKEUP_WHITEIN_TIMER (15)  //�|�P���������V�[���O�̃t�F�[�h�C������


#define _POKECHANGE_WHITEOUT_START (510)  //�|�P���������V�[���̃z���C�g�A�E�g
#define _POKECHANGE_WHITEOUT_TIMER (14)  //�|�P���������V�[���̃z���C�g�A�E�g����
#define _POKECHANGE_WHITEIN_START  (525)  //�{�[�������O�̃t�F�[�h�C��
#define _POKECHANGE_WHITEIN_TIMER (15)  //�{�[�������O�̃t�F�[�h�C������


#define _POKESPLASH_WHITEOUT_START (720)  //�{�[�������V�[���̃z���C�g�A�E�g
#define _POKESPLASH_WHITEOUT_TIMER (14)  //�{�[�������V�[���̃z���C�g�A�E�g����
#define _POKESPLASH_WHITEIN_START  (735)  //�������{�[�������̃t�F�[�h�C��
#define _POKESPLASH_WHITEIN_TIMER (45)  //�������{�[�������̃t�F�[�h�C������


#define _DEMO_END (960)
