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


//�t�F�[�h�Ɋւ��鎞��
#define _POKEUP_WHITEOUT_START (400)  //�����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g
#define _POKEUP_WHITEOUT_TIMER (29)  //�����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g���� 1/
#define _POKEUP_WHITEIN_START  (430)  //�|�P���������V�[���O�̃t�F�[�h�C��
#define _POKEUP_WHITEIN_TIMER (30)  //�|�P���������V�[���O�̃t�F�[�h�C������

#define _POKECHANGE_WHITEOUT_START (880+60)  //�|�P���������V�[���̃z���C�g�A�E�g
#define _POKECHANGE_WHITEOUT_TIMER (29)  //�|�P���������V�[���̃z���C�g�A�E�g����
#define _POKECHANGE_WHITEIN_START  (910+60)  //�{�[�������O�̃t�F�[�h�C��
#define _POKECHANGE_WHITEIN_TIMER (30)  //�{�[�������O�̃t�F�[�h�C������

#define _POKESPLASH_WHITEOUT_START (1300+60)  //�{�[�������V�[���̃z���C�g�A�E�g
#define _POKESPLASH_WHITEOUT_TIMER (29)  //�{�[�������V�[���̃z���C�g�A�E�g����
#define _POKESPLASH_WHITEIN_START  (1330+60)  //�������{�[�������̃t�F�[�h�C��
#define _POKESPLASH_WHITEIN_TIMER (90)  //�������{�[�������̃t�F�[�h�C������

#define _DEMO_END (1680+60)   //�f�����I���Ō�̃t���[��


//�|�P�����̓����Ɋւ��鎞��
#define _POKEMON_DELETE_TIME   (58)  ///�|�P����������
#define _POKE_APPEAR_START   (410+60)  //�|�P�����o�� �����̂͗�
#define _POKE_APPEAR_TIME    (30)  //�|�P�����o��ړ�����
#define _POKE_SIDEOUT_START   (450+60)  //�|�P�������E�Ɉړ����Ă���
#define _POKE_SIDEOUT_TIME    (80)  //�|�P�����o��ړ�����
#define _POKE_SIDEIN_START   (530+60)  //�|�P�������E�Ɉړ����Ė߂��Ă���
#define _POKE_SIDEIN_TIME    (240)   //�|�P�����o��ړ�����
#define _POKE_LEAVE_START   (800+60)  //�|�P�����ޏ�
#define _POKE_LEAVE_TIME    (30)  //�|�P�����ޏ�ړ�����
#define _POKEMON_CREATE_TIME   (1604+60+4)  ///�����I�����ă|�P�����o�� 


///�p�[�e�B�N���̃^�C�~���O
#define _BALL_PARTICLE_START   (1180+60)  ///�����X�^�[�{�[������ �p�[�e�B�N���J�n
#define _PARTICLE_DEMO2_START   (100+60)  ///demo_tex002�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START   (160+60)  ///demo_tex003�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START2  (1200+60)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO5_START   (100+60)  ///�����X�^�[�{�[�����ł�������p�[�e�B�N���J�n
#define _PARTICLE_DEMO6_START   (400+60)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO7_START   (940+60)  ///�p�[�e�B�N���J�n

///OAM�o��̃^�C�~���O
#define _OAM_POKECREATE_START   (0+60)  ///�|�P�������{�[���G�t�F�N�g
#define _OAM_POKECREATE_START2  (1601+60+4)  ///�{�[�����|�P�����G�t�F�N�g



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
#define _POKEMON_FRIEND_LEAVE_POSY _MCSS_POS_Y(260)  // ����̃|�P���������ɑޏꂷ��ꍇ��Y

