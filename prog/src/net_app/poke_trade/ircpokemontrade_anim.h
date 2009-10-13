//=============================================================================
/**
 * @file	  ircpokemontrade_anim.h
 * @bfief	  �|�P�������� �A�j���[�V�����Ԋu��`
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#include "ircpokemontrade_local.h"


//���ԂɊւ��鎖
#define _POKEMON_CENTER_TIME   (60)  ///�|�P�������^�񒆂ɂ�鎞��

#define _POKEMON_DELETE_TIME   (100)  ///�|�P����������


#define _POKEUP_WHITEOUT_START (480)  //�����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g
#define _POKEUP_WHITEOUT_TIMER (29)  //�����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g���� 1/
#define _POKEUP_WHITEIN_START  (510)  //�|�P���������V�[���O�̃t�F�[�h�C��
#define _POKEUP_WHITEIN_TIMER (30)  //�|�P���������V�[���O�̃t�F�[�h�C������

#define _POKE_APPEAR_START   (560)  //�|�P�����o�� �����̂͗�
#define _POKE_APPEAR_TIME    (30)  //�|�P�����o��ړ�����
#define _POKE_SIDEOUT_START   (600)  //�|�P�������E�Ɉړ����Ă���
#define _POKE_SIDEOUT_TIME    (80)  //�|�P�����o��ړ�����
#define _POKE_SIDEIN_START   (720)  //�|�P�������E�Ɉړ����Ė߂��Ă���
#define _POKE_SIDEIN_TIME    (240)   //�|�P�����o��ړ�����
#define _POKE_LEAVE_START   (980)  //�|�P�����ޏ�
#define _POKE_LEAVE_TIME    (30)  //�|�P�����ޏ�ړ�����


#define _BALL_PARTICLE_START   (1300)  ///�����X�^�[�{�[������ �p�[�e�B�N���J�n


#define _PARTICLE_DEMO2_START   (240)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START   (300)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START2  (1400)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO4_START   (-1)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO5_START   (240)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO6_START   (480)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO7_START   (1050)  ///�p�[�e�B�N���J�n



#define _POKECHANGE_WHITEOUT_START (1020)  //�|�P���������V�[���̃z���C�g�A�E�g
#define _POKECHANGE_WHITEOUT_TIMER (29)  //�|�P���������V�[���̃z���C�g�A�E�g����
#define _POKECHANGE_WHITEIN_START  (1050)  //�{�[�������O�̃t�F�[�h�C��
#define _POKECHANGE_WHITEIN_TIMER (30)  //�{�[�������O�̃t�F�[�h�C������


#define _POKESPLASH_WHITEOUT_START (1440)  //�{�[�������V�[���̃z���C�g�A�E�g
#define _POKESPLASH_WHITEOUT_TIMER (29)  //�{�[�������V�[���̃z���C�g�A�E�g����
#define _POKESPLASH_WHITEIN_START  (1470)  //�������{�[�������̃t�F�[�h�C��
#define _POKESPLASH_WHITEIN_TIMER (90)  //�������{�[�������̃t�F�[�h�C������


#define _DEMO_END (1920)   //�f�����I���Ō�̃t���[��

// ���W�Ɋւ��鎖
#define _POKEMON_PLAYER_APPEAR_POSX _MCSS_POS_X( 60)  // �����̃|�P�������o�ꂷ��ꍇ��X
#define _POKEMON_PLAYER_APPEAR_POSY _MCSS_POS_Y(140)  // �����̃|�P�������o�ꂷ��ꍇ��Y
#define _POKEMON_FRIEND_APPEAR_POSX _MCSS_POS_X(190)  // ����̃|�P�������o�ꂷ��ꍇ��X
#define _POKEMON_FRIEND_APPEAR_POSY _MCSS_POS_Y(120)  // ����̃|�P�������o�ꂷ��ꍇ��Y


#define _POKEMON_PLAYER_SIDEOUT_POSX _MCSS_POS_X(-40)  // �����̃|�P���������ɑޏꂷ��ꍇ��X
#define _POKEMON_PLAYER_SIDEOUT_POSY _MCSS_POS_Y(140)  // �����̃|�P���������ɑޏꂷ��ꍇ��Y
#define _POKEMON_FRIEND_SIDEOUT_POSX _MCSS_POS_X(290)  // ����̃|�P�������E�ɑޏꂷ��ꍇ��X
#define _POKEMON_FRIEND_SIDEOUT_POSY _MCSS_POS_Y(120)  // ����̃|�P�������E�ɑޏꂷ��ꍇ��Y


#define _POKEMON_PLAYER_SIDEIN_POSX _MCSS_POS_X(190)  // �����̃|�P������������o�ꂷ��ꍇ��X
#define _POKEMON_PLAYER_SIDEIN_POSY _MCSS_POS_Y(120)  // �����̃|�P������������o�ꂷ��ꍇ��Y
#define _POKEMON_FRIEND_SIDEIN_POSX _MCSS_POS_X( 60)  // ����̃|�P�������E����o�ꂷ��ꍇ��X
#define _POKEMON_FRIEND_SIDEIN_POSY _MCSS_POS_Y(140)  // ����̃|�P�������E����o�ꂷ��ꍇ��Y


#define _POKEMON_PLAYER_LEAVE_POSX _MCSS_POS_X(190)  // �����̃|�P��������ɑޏꂷ��ꍇ��X
#define _POKEMON_PLAYER_LEAVE_POSY _MCSS_POS_Y(-50)  // �����̃|�P��������ɑޏꂷ��ꍇ��Y
#define _POKEMON_FRIEND_LEAVE_POSX _MCSS_POS_X( 60)  // ����̃|�P���������ɑޏꂷ��ꍇ��X
#define _POKEMON_FRIEND_LEAVE_POSY _MCSS_POS_Y(240)  // ����̃|�P���������ɑޏꂷ��ꍇ��Y

