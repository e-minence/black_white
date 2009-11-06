//============================================================================================
/**
 * @file	script_def.h
 * @brief	�X�N���v�g��`�t�@�C��
 * @author	Satoshi Nohara
 * @date	05.12.14
 *
 * �X�N���v�g���ƃv���O�����Ƃŋ��ʂ��ĎQ�Ƃ���K�v�������`���W�񂷂�w�b�_�t�@�C���B
 * �X�N���v�g����enum�Ȃ�C���L�̕��@���߂��ł��Ȃ����߁Adefine���g�p���邱�ƁB
 */
//============================================================================================
#pragma once

//============================================================================================
//
//  �X�N���v�g�w�b�_�֘A
//
//  �X�N���v�g���ł̓}�N���ɖ��ߍ��܂�Ă��邽�߁A���ڂ����̒l��
//  �ӎ��I�Ɏg�p���邱�Ƃ͂Ȃ��B
//============================================================================================
//-----------------------------------------------------------------------------
//����X�N���v�g��`
//-----------------------------------------------------------------------------
#define SP_SCRID_NONE				(0)
#define SP_SCRID_SCENE_CHANGE		(1)     ///<����X�N���v�g�e�[�u���ł̃V�[�������C�x���g�f�[�^�̎w��ID
#define SP_SCRID_ZONE_CHANGE		(2)     ///<����X�N���v�g�e�[�u���ł̃]�[���؂�ւ��X�N���v�g�̎w��ID
#define SP_SCRID_FIELD_RECOVER	(3)			///<����X�N���v�g�e�[�u���ł̃t�B�[���h���A�X�N���v�g�w��ID
#define SP_SCRID_FIELD_INIT 		(4)			///<����X�N���v�g�e�[�u���ł̃t�B�[���h�������X�N���v�g�w��ID

///�C�x���g�f�[�^�e�[�u�������𔻒肷�邽�߂̒l
#define EV_DATA_END_CODE  (0xfd13)  //�K���Ȓl


//============================================================================================
//
//    OBJ�֘A
//
//============================================================================================
//-----------------------------------------------------------------------------
//�G�C���A�X���ʗp�X�N���v�gID
#define SP_SCRID_ALIES				(0xffff)

//�g���[�i�[�^�C�v��`
#define SCR_EYE_TR_TYPE_SINGLE		(0)
#define SCR_EYE_TR_TYPE_DOUBLE		(1)
#define SCR_EYE_TR_TYPE_TAG			(2)

//�g���[�i�[�̔ԍ���`
#define SCR_EYE_TR_0				(0)
#define SCR_EYE_TR_1				(1)

//�A�����OBJ���擾����OBJID��`
#define SCR_OBJID_MV_PAIR			(0xf2)

//�����_�~�[OBJ���擾����OBJID��`
#define SCR_OBJID_DUMMY				(0xf1)

//============================================================================================
//
//    �e�R�}���h�ˑ��̒�`
//
//============================================================================================
//-----------------------------------------------------------------------------
//fld_menu.c
//-----------------------------------------------------------------------------
#define SCR_REPORT_OK				(0)			//�Z�[�u����
#define SCR_REPORT_CANCEL			(1)			//�Z�[�u���Ă��Ȃ�

///YES_NO_WIN�̖߂�l
#define SCR_YES   (0) //YES_NO_WIN�̖߂�l �u�͂��v��Ԃ���
#define SCR_NO    (1) //YES_NO_WIN�̖߂�l �u�������v��Ԃ���

//�V���b�v��ID��`
#define SCR_SHOPID_NULL ( 0xFF )  //�ϓ��V���b�v�Ăяo��ID

//�T�u�v���Z�X(�A�v��)�̏I���R�[�h��`
#define SCR_PROC_RETMODE_EXIT    (0) //�ꔭ�I��
#define SCR_PROC_RETMODE_NORMAL  (1) //�ʏ�I��

//�o�b�O�v���Z�X�Ăяo���R�[�h��`
#define SCR_BAG_MODE_SELL  (0)

//�t�B�[���h�ʐM�ؒf�����C�x���g�̌���
#define SCR_FIELD_COMM_EXIT_OK      (0) //����ɒʐM�ؒf�������I�����
#define SCR_FIELD_COMM_EXIT_CANCEL  (1) //�ؒf�������L�����Z��
#define SCR_FIELD_COMM_EXIT_ERROR   (2) //�ؒf�������ɂȂ�炩�̃G���[

//-----------------------------------------------------------------------------
//  �o�g���֘A
//-----------------------------------------------------------------------------
///TRAINER_LOSE_CHECK,NORMAL_LOSE_CHECK
#define SCR_BATTLE_RESULT_LOSE (0) //����
#define SCR_BATTLE_RESULT_WIN  (1) //����

#define SCR_BATTLE_MODE_NONE    (0) //���Ƀ��[�h�w��Ȃ�
#define SCR_BATTLE_MODE_NOLOSE  (1) //�����Ȃ����[�h

///�쐶�|�P�����퓬�Ăяo�����Ɏw��ł���t���O
#define SCR_WILD_BTL_FLAG_NONE    (0x0000)  //�t���O����
#define SCR_WILD_BTL_FLAG_REGEND  (0x0001)  //�`���퓬(�G���J�E���g���b�Z�[�W���ω�����)
#define SCR_WILD_BTL_FLAG_RARE    (0x0002)  //�������A�|�P�퓬

///�쐶�|�P������@�Đ�R�[�h�`�F�b�N
#define SCR_WILD_BTL_RET_CAPTURE  (0) //�߂܂���
#define SCR_WILD_BTL_RET_ESCAPE   (1) //������
#define SCR_WILD_BTL_RET_WIN      (2) //�|����

//-----------------------------------------------------------------------------
//�j���擾�֐��̖߂�l
//  _GET_WEEK
//-----------------------------------------------------------------------------
#define RTC_WEEK_SUNDAY		0
#define RTC_WEEK_MONDAY		1
#define RTC_WEEK_TUESDAY	2
#define RTC_WEEK_WEDNESDAY	3
#define RTC_WEEK_THURSDAY	4
#define RTC_WEEK_FRIDAY		5
#define RTC_WEEK_SATURDAY	6

//-----------------------------------------------------------------------------
//�o�b�W�t���O�w��ID�̒�`
//  _GET_BADGE_FLAG/_SET_BADGE_FLAG
//-----------------------------------------------------------------------------
#define BADGE_ID_01   0
#define BADGE_ID_02   1
#define BADGE_ID_03   2
#define BADGE_ID_04   3
#define BADGE_ID_05   4
#define BADGE_ID_06   5
#define BADGE_ID_07   6
#define BADGE_ID_08   7

//-----------------------------------------------------------------------------
// �莝���|�P�������J�E���g���[�h
//-----------------------------------------------------------------------------
#define POKECOUNT_MODE_TOTAL         (0)  // �莝���̐�
#define POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S�������莝���̐�
#define POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P������
#define POKECOUNT_MODE_ONLY_EGG      (3)  // �^�}�S�̐�(�ʖڃ^�}�S������)
#define POKECOUNT_MODE_ONLY_DAME_EGG (4)  // �ʖڃ^�}�S�̐�

//-----------------------------------------------------------------------------
//  �����񑀍샂�[�h�w��
//-----------------------------------------------------------------------------
#define ZUKANCTRL_MODE_SEE     (0)   ///<�����񑀍샂�[�h�F������
#define ZUKANCTRL_MODE_GET     (1)   ///<�����񑀍샂�[�h�F�߂܂�����

//-----------------------------------------------------------------------------
//  �z�u���f���p�A�j���w��
//-----------------------------------------------------------------------------
#define  SCR_BMANM_DOOR_OPEN    (0)       ///<�h�A�A�j���F�J��
#define  SCR_BMANM_DOOR_CLOSE   (1)       ///<�h�A�A�j���F����



/*
//�t���O����֐��̃��[�h�w��p
#define	FLAG_MODE_RESET				(0)
#define	FLAG_MODE_SET				(1)
#define	FLAG_MODE_GET				(2)

//�}�Ӄ��[�h
#define SCR_SHINOU_ZUKAN_MODE	0
#define SCR_ZENKOKU_ZUKAN_MODE	1

//�p�\�R���A�j���Ǘ��i���o�[
#define SCR_PASO_ANM_NO		90


//�Z�����F3�l���āA���ꂼ��̐F�̋Z���������Ă���Ȃ�
#define WAZA_OSHIE_COL_BLUE			(0)
#define WAZA_OSHIE_COL_RED			(1)
#define WAZA_OSHIE_COL_YELLOW		(2)

//�Z�o�����ʃE�F�C�g
#define WAZA_COMMON_WAIT			(30)
#define WAZA_COMMON_WAIT2			(32)
#define WAZA_COMMON_WAIT3			(16)
*/

