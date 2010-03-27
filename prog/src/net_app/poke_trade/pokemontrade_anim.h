//=============================================================================
/**
 * @file	  pokemontrade_anim.h
 * @bfief	  �|�P�������� �A�j���[�V�����Ԋu��`
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#include "pokemontrade_local.h"


//���ԂɊւ��鎖
#define _POKEMON_CENTER_TIME   (42)  ///�|�P�������^�񒆂ɂ�鎞��


#define _POKEMON_VOICE_TIME   (200)  ///�|�P�����������o���Ă��鎞��


//�t�F�[�h�Ɋւ��鎞��
#define _POKEUP_WHITEOUT_START (266)       //�G�R���e�U�ԃ����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g
#define _POKEUP_WHITEOUT_TIMER (20)       //�G�R���e�U�ԃ����X�^�[�{�[�����ł����������ۂ̃z���C�g�A�E�g���� 1/
#define _POKEUP_WHITEIN_START  (295)      //�G�R���e�V�ԃ|�P���������V�[���O�̃t�F�[�h�C��
#define _POKEUP_WHITEIN_TIMER (25)        //�G�R���e�V�ԃ|�P���������V�[���O�̃t�F�[�h�C������

#define _POKECHANGE_WHITEOUT_START (651)  //�G�R���e�P�S�ԃ|�P���������V�[���̃z���C�g�A�E�g
#define _POKECHANGE_WHITEOUT_TIMER (10)      //�G�R���e�P�S�ԃ|�P���������V�[���̃z���C�g�A�E�g����
#define _POKECHANGE_WHITEIN_START  (693)  //�G�R���e�P�T�ԃ{�[�������O�̃t�F�[�h�C��
#define _POKECHANGE_WHITEIN_TIMER (25)        //�G�R���e�P�T�ԃ{�[�������O�̃t�F�[�h�C������

#define _POKESPLASH_WHITEOUT_START (896)  //�G�R���e�Q�O�ԃ{�[�������V�[���̃z���C�g�A�E�g
#define _POKESPLASH_WHITEOUT_TIMER (30)       //�G�R���e�Q�O�ԃ{�[�������V�[���̃z���C�g�A�E�g����
#define _POKESPLASH_WHITEIN_START  (970)  //�G�R���e�Q�P�Ԃ������{�[�������̃t�F�[�h�C��
#define _POKESPLASH_WHITEIN_TIMER (30)        //�G�R���e�Q�P�Ԃ������{�[�������̃t�F�[�h�C������

#define _DEMO_END (1218)   //�f�����I���Ō�̃t���[��


//�|�P�����̓����Ɋւ��鎞��
#define _POKEMON_DELETE_TIME   (4)     // �G�R���e�R��  �|�P����������

#define _POKE_APPEAR_START   (330)   //�G�R���e�W��  �|�P�����o�� �����̂͗�
#define _POKE_APPEAR_TIME    (15)       //�G�R���e�W��  �|�P�����o��ړ�����
#define _POKE_APPEAR_START2   (340)   //�G�R���e�X��     ����ɂ������ړ�
#define _POKE_APPEAR_TIME2   (28)       //�G�R���e�X��  ����ɂ������ړ�
#define _POKE_SIDEOUT_START   (368)  //�G�R���e�P�O��  �|�P�������E�Ɉړ����Ă���
#define _POKE_SIDEOUT_TIME    (46)      //�G�R���e�P�O��  �|�P�����o��ړ�����


#define _POKE_SIDEIN_START   (434)   //�G�R���e�P�P��  �|�P�������E�Ɉړ����Ė߂��Ă���
#define _POKE_SIDEIN_TIME    (103)     //�G�R���e�P�P�� �|�P�����o��ړ�����

#define _POKE_SIDEIN_START2   (504)   //�G�R���e�P�P��  ����Ƀ|�P�������E�Ɉړ����Ė߂��Ă���
#define _POKE_SIDEIN_TIME2    (128)     //�G�R���e�P�P�� ����Ƀ|�P�����o��ړ�����

#define _POKE_LEAVE_START   (637)  //�G�R���e�P�R��  �|�P�����ޏ�




#define _POKE_LEAVE_TIME    (7)      //�G�R���e�P�R��  �|�P�����ޏ�ړ�����

#define _POKEMON_CREATE_TIME   (1166)  ///�G�R���e�Q�S�� �����I�����ă|�P�����o�� 


///�p�[�e�B�N���̃^�C�~���O
#define _BALL_PARTICLE_START   (854)  ///�����X�^�[�{�[������ �p�[�e�B�N���J�n
#define _PARTICLE_DEMO2_START   (101)  ///demo_tex002�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START   (154)  ///demo_tex003�p�[�e�B�N���J�n
#define _PARTICLE_DEMO3_START2  (854)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO4_START   (14)     ///�|�P�������{�[���G�t�F�N�g
#define _PARTICLE_DEMO4_START2   (1155)  ///�{�[�����|�P�����G�t�F�N�g
#define _PARTICLE_DEMO5_START   (154)  ///�����X�^�[�{�[�����ł�������p�[�e�B�N���J�n
#define _PARTICLE_DEMO6_START   (338)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO7_START   (700)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO8_START   (210)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEMO9_START   (280)  ///�p�[�e�B�N���J�n
#define _PARTICLE_DEM10_START   (350)  ///�p�[�e�B�N���J�n

///OAM�o��̃^�C�~���O
#define _OAM_POKECREATE_START   (14)  ///�|�P�������{�[���G�t�F�N�g
#define _OAM_POKECREATE_START2  (1151)  ///�{�[�����|�P�����G�t�F�N�g




// ���W�Ɋւ��鎖
//�G�R���e�Q�Ԃ̈ړ��̒��S
#define _POKEMON_PLAYER_CENTER_POSX _MCSS_POS_X(  0)   // �����̃|�P�������^�񒆂Ɉړ�����ۂ�X
#define _POKEMON_PLAYER_CENTER_POSY _MCSS_POS_Y(-23)   // �����̃|�P�������^�񒆂Ɉړ�����ۂ�Y
#define _POKEMON_PLAYER_CENTER_POSZ _MCSS_POS_Z( 10)   // �����̃|�P�������^�񒆂Ɉړ�����ۂ�Z

#define _POKEMON_FRIEND_CENTER_POSX _MCSS_POS_X(  0)   // ����̃|�P�������^�񒆂Ɉړ�����ۂ�X
#define _POKEMON_FRIEND_CENTER_POSY _MCSS_POS_Y(-23)   // ����̃|�P�������^�񒆂Ɉړ�����ۂ�Y
#define _POKEMON_FRIEND_CENTER_POSZ _MCSS_POS_Z(-10)   // ����̃|�P�������^�񒆂Ɉړ�����ۂ�Z


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
#define _POKEMON_PLAYER_SIDEOUT_POSZ _MCSS_POS_Z(0)    // �����̃|�P������Z
#define _POKEMON_FRIEND_SIDEOUT_POSX _MCSS_POS_X(130)           // ����̃|�P������X
#define _POKEMON_FRIEND_SIDEOUT_POSY _POKEMON_FRIEND_DOWN_POSY  // ����̃|�P������Y
#define _POKEMON_FRIEND_SIDEOUT_POSZ _MCSS_POS_Z(0)  // ����̃|�P������Z

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
#define _FRONT_POKMEON_SCALE_SIZE   (1.2)
//���̃|�P�����T�C�Y
#define _BACK_POKMEON_SCALE_SIZE   (1.2)

//����㉺��  �傫���قǑ傫���h���
#define _WAVE_NUM    (8)



//SE�^�C�~���O -1�͂����ł͓��Ă�Ȃ��̂Ŏ�ł��ĂĂ���

#define _GTSSTART_SEQ_SE_DANSA	    (-1)		//�f��	wireless�����f��	�|�P�����W�����v	�҂イ
#define _GTSSTART_SEQ_SE_SYS_83			(-1)   //�f��	wireless�����f��	�|�P�������n	�Ƃ�I
#define _GTSSTART_SEQ_SE_W554_PAKIN	(1)		//�f��	wireless�����f��	�|�P�����A����	�s�J�[�[�[�I
#define _GTSSTART_SEQ_SE_BOWA2			(4)   //�f��	wireless�����f��	�{�[���ɓ���	�{���I
#define _GTSSTART_SEQ_SE_KON			(29)    //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART_SEQ_SE_KON1			(45)  //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART_SEQ_SE_KON2			(56)  //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART_SEQ_SE_KON3			(63)  //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART_SEQ_SE_TDEMO_001	(17)		//�f��	wireless�����f��	�{�[������L���L�����������o��	�L���������������c
#define _GTSSTART_SEQ_SE_TDEMO_002	(107)		//�f��	wireless�����f��	����������A���O�����ς��	�V���V���V���V���V���V���V���c
#define _GTSSTART_SEQ_SE_W028_02		(152)	//�f��	wireless�����f��	�{�[����яオ��	�V�����b�I
#define _GTSSTART_SEQ_SE_TDEMO_006	(266)		//�f��	wireless�����f��	�{�[�����V��ɓ��B�A�����ăz���C�g�A�E�g	�s�V���[�[�[���{�����[�[�[
#define _GTSSTART_SEQ_SE_TDEMO_007	(350)		//�f��	wireless�����f��	�󒆂Ɍ��̗ւ��ł��Ă������܂��	�L���L���c�L���L���c
#define _GTSSTART_SEQ_SE_TDEMO_008	(636)		//�f��	wireless�����f��	�|�P�����҃X�s�[�h�Ŕ��ł���	�����I
#define _GTSSTART_SEQ_SE_TDEMO_009	(650)		//�f��	wireless�����f��	�z���C�g�A�E�g	����������
#define _GTSSTART_SEQ_SE_TDEMO_010	(694)		//�f��	wireless�����f��	�{�[������󂩂痎���Ă���i��	�L���L���{�V���[�[�[�[��
#define _GTSSTART_SEQ_SE_TDEMO_011	(857)		//�f��	wireless�����f��	�{�[�����n��ɓ��B	�p�L���[�[���I�L���L���L���c
#define _GTSSTART2_SEQ_SE_TDEMO_001	(858)		//�f��	wireless�����f��	�{�[������L���L�����������o��(���SEQ_SE_TDEMO_011�Ɠ����ɖ炷)	�L���������������c
#define _GTSSTART2_SEQ_SE_TDEMO_009	(896)		//�f��	wireless�����f��	�ēx�z���C�g�A�E�g	����������
#define _GTSSTART2_SEQ_SE_KON			  (1048)   //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART2_SEQ_SE_KON1			  (1068)   //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART2_SEQ_SE_KON2			  (1082)   //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART2_SEQ_SE_KON3			  (1092)   //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART2_SEQ_SE_KON4			  (1103)   //�f��	wireless�����f��	�{�[���͂˂�	�R���R���R���c
#define _GTSSTART_SEQ_SE_TDEMO_004	(1123)		//�f��	wireless�����f��	�{�[������	�ق�I
#define _GTSSTART2_SEQ_SE_BOWA2			(1166)  //�f��	wireless�����f��	�|�P�����������	�{���I
#define _GTSSTART3_SEQ_SE_TDEMO_001	(1159)		//�f��	wireless�����f��	�|�P���������i���Ƃт���	�L���������������c




//----------------------------------------------------------------------------------------------
//��������ԊO���f��
/*

����ʗp ica
ir_sean.ica
ir_sean.imd
ir_sean.ita

���J�����ʒu
ir_ichi.ica

���J���������_
ir_tyushiten.ica

���p�[�e�B�N��
�g��
ir_tex001.spr�i2��o���Ă��������j

�����V�[��
ir_tex002.spr
ir_tex004.spr

�|�P�����������ʁi2��o���Ă��������j
ir_tex003.spr

������ʗp
under02.nsc
demo_backshot21.ncl
demo_backshot2.ncg


�S����660�t���[������܂��B

*/


#define _IR_POKEUP_WHITEOUT_START (130)    //�ŏ��̃z���C�g�A�E�g
#define _IR_POKEUP_WHITEOUT_TIMER (20)     //
#define _IR_POKEUP_WHITEIN_START  (180)    //
#define _IR_POKEUP_WHITEIN_TIMER (25)      //

#define _IR_POKECHANGE_WHITEOUT_START (409)  //������̃z���C�g�A�E�g
#define _IR_POKECHANGE_WHITEOUT_TIMER (10)      //
#define _IR_POKECHANGE_WHITEIN_START  (420)  //
#define _IR_POKECHANGE_WHITEIN_TIMER (25)        //


#define _IR_DEMO_END (660)   //�f�����I���Ō�̃t���[��

///�p�[�e�B�N���̃^�C�~���O
#define _IRTEX001_01_START   (-1)   ///�g��
#define _IRTEX001_02_START   (350)   ///�g��
#define _IRTEX002_01_START   (200)   ///�����V�[��
#define _IRTEX003_01_START   (20)    ///�|�P������������
#define _IRTEX003_02_START   (600)    ///�|�P������������
#define _IRTEX004_01_START   (-1) ///�����V�[��

//�|�P�����̓����Ɋւ��鎞��
#define _IR_POKEMON_DELETE_TIME   (4)     //  �|�P����������

#define _IR_POKE_APPEAR_START   (250)   //    �|�P�����o�� �����̂͗�
#define _IR_POKE_APPEAR_TIME    (150)       //  �|�P�����o��ړ�����

#define _IR_OAM_POKECREATE      (598)  //�Ō�Ƀ|�P�������o����
#define _IR_POKEMON_CREATE_TIME  (615)  //�|�P�������o������

//�|�P�����̈ʒu
#define _IR_POKEMON_PLAYER_APPEAR_POSX _MCSS_POS_X( -50)  // �����̃|�P������X
#define _IR_POKEMON_PLAYER_APPEAR_POSY _MCSS_POS_Y(-500)  // �����̃|�P������Y
#define _IR_POKEMON_PLAYER_APPEAR_POSZ _MCSS_POS_Z(10)  // �����̃|�P������Z
#define _IR_POKEMON_FRIEND_APPEAR_POSX _MCSS_POS_X( 60)  // ����̃|�P������X
#define _IR_POKEMON_FRIEND_APPEAR_POSY _MCSS_POS_Y(300)  // ����̃|�P������Y
#define _IR_POKEMON_FRIEND_APPEAR_POSZ _MCSS_POS_Z(-10)  // ����̃|�P������Z

#define _IR_POKEMON_PLAYER_UP_POSX   _IR_POKEMON_PLAYER_APPEAR_POSX  // �����̃|�P������X
#define _IR_POKEMON_PLAYER_UP_POSY   _MCSS_POS_Y(300)             // �����̃|�P������Y
#define _IR_POKEMON_PLAYER_UP_POSZ   _IR_POKEMON_PLAYER_APPEAR_POSZ  // �����̃|�P������Z
#define _IR_POKEMON_FRIEND_DOWN_POSX _IR_POKEMON_FRIEND_APPEAR_POSX  // ����̃|�P������X
#define _IR_POKEMON_FRIEND_DOWN_POSY _MCSS_POS_Y(-370)              // ����̃|�P������Y
#define _IR_POKEMON_FRIEND_DOWN_POSZ _IR_POKEMON_FRIEND_APPEAR_POSZ  // ����̃|�P������Z

//��O�̃|�P�����̃T�C�Y
#define _IRC_FRONT_POKMEON_SCALE_SIZE   (2)
//��O�̃|�P�����̃� 0-31�܂�
#define _IRC_FRONT_POKMEON_ALPHA  (15)
//���̃|�P�����T�C�Y
#define _IRC_BACK_POKMEON_SCALE_SIZE   (2)
//���̃|�P�����̃� 0-31�܂�
#define _IRC_BACK_POKMEON_ALPHA  (15)


//��яオ���Ă���SE�̃X�^�[�g�t���[��
#define _IRC_PMSND_START  (50)


//SE�^�C�~���O -1�͂����ł͓��Ă�Ȃ��̂Ŏ�ł��ĂĂ���

#define _IRCSTART_SEQ_SE_DANSA (-1)			//�f��	IR�����f��	�|�P�����W�����v	�҂イ
#define _IRCSTART_SEQ_SE_SYS_83	 (-1)		//�f��	IR�����f��	�|�P�������n	�Ƃ�I
#define _IRCSTART_SEQ_SE_W554_PAKIN (32)			//�f��	IR�����f��	�|�P�����A����	�s�J�[�[�[�I
#define _IRCSTART_SEQ_SE_BOWA2 (33)			//�f��	IR�����f��	�{�[���ɓ���	�{���I
#define _IRCSTART_SEQ_SE_KON	 (38)		//�f��	IR�����f��	�{�[���͂˂�	�R���R���R���c
//�͂˂�Ԃ񂠂Ă�
#define _IRCSTART_SEQ_SE_KON1	 (45)
#define _IRCSTART_SEQ_SE_KON2	 (56)
#define _IRCSTART_SEQ_SE_KON3	 (64)

#define _IRCSTART_SEQ_SE_TDEMO_001 (30)			//�f��	IR�����f��	�{�[������L���L�����������o��	�L���������������c
#define _IRCSTART_SEQ_SE_W028_02 (110)			//�f��	IR�����f��	�{�[����яオ��	�V�����b�I
#define _IRCSTART_SEQ_SE_TDEMO_009 (130)			//�f��	IR�����f��	��x�ڂ̃z���C�g�A�E�g	����������
#define _IRCSTART_SEQ_SE_TDEMO_002 (241)			//�f��	IR�����f��	�{�[�����󒆈ړ��i�S�b�����炢�j	�V���V���V���V���V���V���V���c
#define _IRCSTART_SEQ_SE_TDEMO_003 (320)			//�f��	IR�����f��	�|�P���������A���ꂿ����	�t�H�A�b
#define _IRCSTART_SEQ_SE_W054_01 (364)			//�f��	IR�����f��	�g�䂪�L����A�{�[�������	�������c
#define _IRCSTART_SEQ_SE_W179_02 (409)			//�f��	IR�����f��	���ԕ��z���C�g�A�E�g	�L�[��
#define _IRCSTART_SEQ_SE_W307_03 (484)			//�f��	IR�����f��	�{�[����󂩂痎��	�V���E�E�E�E�E
#define _IRCSTART2_SEQ_SE_KON	 (494)	//	�f��	IR�����f��	�{�[���͂˂�	�R���R���R���c

#define _IRCSTART2_SEQ_SE_KON1	 (514)	//	�f��	IR�����f��	�{�[���͂˂�	�R���R���R���c
#define _IRCSTART2_SEQ_SE_KON2	 (528)	//	�f��	IR�����f��	�{�[���͂˂�	�R���R���R���c
#define _IRCSTART2_SEQ_SE_KON3	 (538)	//	�f��	IR�����f��	�{�[���͂˂�	�R���R���R���c
#define _IRCSTART2_SEQ_SE_KON4	 (547)	//	�f��	IR�����f��	�{�[���͂˂�	�R���R���R���c


#define _IRCSTART_SEQ_SE_TDEMO_004	 (604)		//�f��	IR�����f��	�{�[������	�ق�I
#define _IRCSTART_SEQ_SE_TDEMO_005 (615)			//�f��	IR�����f��	�{�[������|�P������яo��	�p�L�[�[�[��
#define _IRCSTART2_SEQ_SE_TDEMO_001 (605)			//�f��	IR�����f��	��яo�����|�P��������L���L������������яo��	�L�����������c

