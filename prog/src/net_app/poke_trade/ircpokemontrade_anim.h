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
#define _POKEUP_WHITEOUT_START (380)       //�G�R���e�U�ԃ����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g
#define _POKEUP_WHITEOUT_TIMER (29)       //�G�R���e�U�ԃ����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g���� 1/
#define _POKEUP_WHITEIN_START  (430)      //�G�R���e�V�ԃ|�P���������V�[���O�̃t�F�[�h�C��
#define _POKEUP_WHITEIN_TIMER (30)        //�G�R���e�V�ԃ|�P���������V�[���O�̃t�F�[�h�C������

#define _POKECHANGE_WHITEOUT_START (930)  //�G�R���e�P�S�ԃ|�P���������V�[���̃z���C�g�A�E�g
#define _POKECHANGE_WHITEOUT_TIMER (29)      //�G�R���e�P�S�ԃ|�P���������V�[���̃z���C�g�A�E�g����
#define _POKECHANGE_WHITEIN_START  (990)  //�G�R���e�P�T�ԃ{�[�������O�̃t�F�[�h�C��
#define _POKECHANGE_WHITEIN_TIMER (60)        //�G�R���e�P�T�ԃ{�[�������O�̃t�F�[�h�C������

#define _POKESPLASH_WHITEOUT_START (1280)  //�G�R���e�Q�O�ԃ{�[�������V�[���̃z���C�g�A�E�g
#define _POKESPLASH_WHITEOUT_TIMER (29)       //�G�R���e�Q�O�ԃ{�[�������V�[���̃z���C�g�A�E�g����

#define _POKESPLASH_WHITEIN_START  (1390)  //�G�R���e�Q�P�Ԃ������{�[�������̃t�F�[�h�C��
#define _POKESPLASH_WHITEIN_TIMER (30)        //�G�R���e�Q�P�Ԃ������{�[�������̃t�F�[�h�C������

#define _DEMO_END (1740)   //�f�����I���Ō�̃t���[��


//�|�P�����̓����Ɋւ��鎞��
#define _POKEMON_DELETE_TIME   (5)     // �G�R���e�R��  �|�P����������

#define _POKE_APPEAR_START   (470)   //�G�R���e�W��  �|�P�����o�� �����̂͗�
#define _POKE_APPEAR_TIME    (20)       //�G�R���e�W��  �|�P�����o��ړ�����
#define _POKE_APPEAR_START2   (485)   //�G�R���e�X��     ����ɂ������ړ�
#define _POKE_APPEAR_TIME2   (40)       //�G�R���e�X��  ����ɂ������ړ�
#define _POKE_SIDEOUT_START   (525)  //�G�R���e�P�O��  �|�P�������E�Ɉړ����Ă���
#define _POKE_SIDEOUT_TIME    (65)      //�G�R���e�P�O��  �|�P�����o��ړ�����
#define _POKE_SIDEIN_START   (620)   //�G�R���e�P�P��  �|�P�������E�Ɉړ����Ė߂��Ă���
#define _POKE_SIDEIN_TIME    (200)     //�G�R���e�P�P�� �|�P�����o��ړ�����
#define _POKE_LEAVE_START   (910)  //�G�R���e�P�R��  �|�P�����ޏ�
#define _POKE_LEAVE_TIME    (10)      //�G�R���e�P�R��  �|�P�����ޏ�ړ�����

#define _POKEMON_CREATE_TIME   (1665)  ///�G�R���e�Q�S�� �����I�����ă|�P�����o�� 


///�p�[�e�B�N���̃^�C�~���O
#define _BALL_PARTICLE_START   (1220)  ///�����X�^�[�{�[������ �p�[�e�B�N���J�n
#define _PARTICLE_DEMO2_START   (145)  ///demo_tex002�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START   (220)  ///demo_tex003�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START2  (1220)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO4_START   (20)     ///�|�P�������{�[���G�t�F�N�g
#define _PARTICLE_DEMO4_START2   (1650)  ///�{�[�����|�P�����G�t�F�N�g
#define _PARTICLE_DEMO5_START   (220)  ///�����X�^�[�{�[�����ł�������p�[�e�B�N���J�n
#define _PARTICLE_DEMO6_START   (490)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO7_START   (1000)  ///�p�[�e�B�N���J�n

///OAM�o��̃^�C�~���O
#define _OAM_POKECREATE_START   (20)  ///�|�P�������{�[���G�t�F�N�g
#define _OAM_POKECREATE_START2  (1645)  ///�{�[�����|�P�����G�t�F�N�g




// ���W�Ɋւ��鎖
//�G�R���e�Q�Ԃ̈ړ��̒��S
#define _POKEMON_PLAYER_CENTER_POSX _MCSS_POS_X(  0)   // �����̃|�P�������^�񒆂Ɉړ�����ۂ�X
#define _POKEMON_PLAYER_CENTER_POSY _MCSS_POS_Y(-20)   // �����̃|�P�������^�񒆂Ɉړ�����ۂ�Y
#define _POKEMON_PLAYER_CENTER_POSZ _MCSS_POS_Z( 10)   // �����̃|�P�������^�񒆂Ɉړ�����ۂ�Z

//�G�R���e�W�Ԃ̍ŏ��̈ʒu
#define _POKEMON_PLAYER_APPEAR_POSX _MCSS_POS_X( -50)  // �����̃|�P������X
#define _POKEMON_PLAYER_APPEAR_POSY _MCSS_POS_Y(-150)  // �����̃|�P������Y
#define _POKEMON_PLAYER_APPEAR_POSZ _MCSS_POS_Z(  10)  // �����̃|�P������Z
#define _POKEMON_FRIEND_APPEAR_POSX _MCSS_POS_X( 60)  // ����̃|�P������X
#define _POKEMON_FRIEND_APPEAR_POSY _MCSS_POS_Y(80)  // ����̃|�P������Y
#define _POKEMON_FRIEND_APPEAR_POSZ _MCSS_POS_Z(-10)  // ����̃|�P������Z

//�G�R���e�X�Ԃ̈ړ���̈ʒu
#define _POKEMON_PLAYER_UP_POSX   _POKEMON_PLAYER_APPEAR_POSX  // �����̃|�P������X
#define _POKEMON_PLAYER_UP_POSY   _MCSS_POS_Y(-55)             // �����̃|�P������Y
#define _POKEMON_PLAYER_UP_POSZ   _POKEMON_PLAYER_APPEAR_POSZ  // �����̃|�P������Z
#define _POKEMON_FRIEND_DOWN_POSX _POKEMON_FRIEND_APPEAR_POSX  // ����̃|�P������X
#define _POKEMON_FRIEND_DOWN_POSY _MCSS_POS_Y( 5)              // ����̃|�P������Y
#define _POKEMON_FRIEND_DOWN_POSZ _POKEMON_FRIEND_APPEAR_POSZ  // ����̃|�P������Z


//�G�R���e�P�O��
#define _POKEMON_PLAYER_SIDEOUT_POSX _MCSS_POS_X(-140)           // �����̃|�P������X
#define _POKEMON_PLAYER_SIDEOUT_POSY _POKEMON_PLAYER_UP_POSY    // �����̃|�P������Y
#define _POKEMON_PLAYER_SIDEOUT_POSZ _POKEMON_PLAYER_UP_POSZ    // �����̃|�P������Z
#define _POKEMON_FRIEND_SIDEOUT_POSX _MCSS_POS_X(140)           // ����̃|�P������X
#define _POKEMON_FRIEND_SIDEOUT_POSY _POKEMON_FRIEND_DOWN_POSY  // ����̃|�P������Y
#define _POKEMON_FRIEND_SIDEOUT_POSZ _POKEMON_FRIEND_DOWN_POSZ  // ����̃|�P������Z

//�G�R���e�P�P��
#define _POKEMON_PLAYER_SIDEST_POSX _POKEMON_PLAYER_SIDEOUT_POSX  // �����̃|�P������������o�ꂷ��ꍇ��X
#define _POKEMON_PLAYER_SIDEST_POSY _MCSS_POS_Y(5)  // �����̃|�P������������o�ꂷ��ꍇ��Y
#define _POKEMON_PLAYER_SIDEST_POSZ _MCSS_POS_Z(-10)  // �����̃|�P������������o�ꂷ��ꍇ��Y
#define _POKEMON_FRIEND_SIDEST_POSX _POKEMON_FRIEND_SIDEOUT_POSX  // ����̃|�P�������E����o�ꂷ��ꍇ��X
#define _POKEMON_FRIEND_SIDEST_POSY _MCSS_POS_Y(-55)  // ����̃|�P�������E����o�ꂷ��ꍇ��Y
#define _POKEMON_FRIEND_SIDEST_POSZ _MCSS_POS_Z(10)  // ����̃|�P�������E����o�ꂷ��ꍇ��Y

//�G�R���e�P�Q��
#define _POKEMON_PLAYER_SIDEIN_POSX _MCSS_POS_X( 60)  // �����̃|�P������������o�ꂷ��ꍇ��X
#define _POKEMON_PLAYER_SIDEIN_POSY _POKEMON_PLAYER_SIDEST_POSY  // �����̃|�P������������o�ꂷ��ꍇ��Y
#define _POKEMON_PLAYER_SIDEIN_POSZ _POKEMON_PLAYER_SIDEST_POSZ  // �����̃|�P������������o�ꂷ��ꍇ��Y
#define _POKEMON_FRIEND_SIDEIN_POSX _MCSS_POS_X(-45)  // ����̃|�P�������E����o�ꂷ��ꍇ��X
#define _POKEMON_FRIEND_SIDEIN_POSY _POKEMON_FRIEND_SIDEST_POSY  // ����̃|�P�������E����o�ꂷ��ꍇ��Y
#define _POKEMON_FRIEND_SIDEIN_POSZ _POKEMON_FRIEND_SIDEST_POSZ  // ����̃|�P�������E����o�ꂷ��ꍇ��Y

//�G�R���e�P�R��
#define _POKEMON_PLAYER_LEAVE_POSX _POKEMON_PLAYER_SIDEIN_POSX  // �����̃|�P��������ɑޏꂷ��ꍇ��X
#define _POKEMON_PLAYER_LEAVE_POSY _MCSS_POS_Y(100)  // �����̃|�P��������ɑޏꂷ��ꍇ��Y
#define _POKEMON_PLAYER_LEAVE_POSZ _POKEMON_PLAYER_SIDEIN_POSZ  // �����̃|�P��������ɑޏꂷ��ꍇ��Y
#define _POKEMON_FRIEND_LEAVE_POSX _POKEMON_FRIEND_SIDEIN_POSX  // ����̃|�P���������ɑޏꂷ��ꍇ��X
#define _POKEMON_FRIEND_LEAVE_POSY _MCSS_POS_Y(-160)  // ����̃|�P���������ɑޏꂷ��ꍇ��Y
#define _POKEMON_FRIEND_LEAVE_POSZ _POKEMON_FRIEND_SIDEIN_POSZ  // ����̃|�P���������ɑޏꂷ��ꍇ��Y


//��O�̃|�P�����̃T�C�Y
#define _POKMEON_SCALE_SIZE   (1.2)
