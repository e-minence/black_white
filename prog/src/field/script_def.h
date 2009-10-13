//============================================================================================
/**
 * @file	script_def.h
 * @brief	�X�N���v�g��`�t�@�C��
 * @author	Satoshi Nohara
 * @date	05.12.14
 */
//============================================================================================
#ifndef SCRIPT_DEF_H
#define SCRIPT_DEF_H


//============================================================================================
//
//	��`
//
//============================================================================================
///����X�N���v�g��`
#define SP_SCRID_NONE				(0)
#define SP_SCRID_SCENE_CHANGE		(1)     ///<����X�N���v�g�e�[�u���ł̃V�[�������C�x���g�f�[�^�̎w��ID
#define SP_SCRID_ZONE_CHANGE		(2)     ///<����X�N���v�g�e�[�u���ł̃]�[���؂�ւ��X�N���v�g�̎w��ID
#define SP_SCRID_FIELD_RECOVER	(3)			///<����X�N���v�g�e�[�u���ł̃t�B�[���h���A�X�N���v�g�w��ID
#define SP_SCRID_FIELD_INIT 		(4)			///<����X�N���v�g�e�[�u���ł̃t�B�[���h�������X�N���v�g�w��ID

///�C�x���g�f�[�^�e�[�u�������𔻒肷�邽�߂̒l
#define EV_DATA_END_CODE  (0xfd13)  //�K���Ȓl


//�G�C���A�X���ʗp�X�N���v�gID
#define SP_SCRID_ALIES				(0xffff)

//�g���[�i�[�^�C�v��`
#define SCR_EYE_TR_TYPE_SINGLE		(0)
#define SCR_EYE_TR_TYPE_DOUBLE		(1)
#define SCR_EYE_TR_TYPE_TAG			(2)

//�g���[�i�[�̔ԍ���`
#define SCR_EYE_TR_0				(0)
#define SCR_EYE_TR_1				(1)

//fld_menu.c
#define SCR_REPORT_OK				(0)			//�Z�[�u����
#define SCR_REPORT_CANCEL			(1)			//�Z�[�u���Ă��Ȃ�

//�A�����OBJ���擾����OBJID��`
#define SCR_OBJID_MV_PAIR			(0xf2)

//�����_�~�[OBJ���擾����OBJID��`
#define SCR_OBJID_DUMMY				(0xf1)

//�t���O����֐��̃��[�h�w��p
#define	FLAG_MODE_RESET				(0)
#define	FLAG_MODE_SET				(1)
#define	FLAG_MODE_GET				(2)

//�G���x�[�^�A�j��
#define T07R0103_ELEVATOR_LOOP		(4)
#define C05R0103_ELEVATOR_LOOP		(4)
#define C05R0803_ELEVATOR_LOOP		(4)
#define C07R0206_ELEVATOR_LOOP		(4)
#define C08R0802_ELEVATOR_LOOP		(4)
#define C01R0208_ELEVATOR_LOOP		(4)

//�j���擾�֐��̖߂�l
#define RTC_WEEK_SUNDAY		0
#define RTC_WEEK_MONDAY		1
#define RTC_WEEK_TUESDAY	2
#define RTC_WEEK_WEDNESDAY	3
#define RTC_WEEK_THURSDAY	4
#define RTC_WEEK_FRIDAY		5
#define RTC_WEEK_SATURDAY	6

//�}�Ӄ��[�h
#define SCR_SHINOU_ZUKAN_MODE	0
#define SCR_ZENKOKU_ZUKAN_MODE	1

//�p�\�R���A�j���Ǘ��i���o�[
#define SCR_PASO_ANM_NO		90

//�ʑ��C�x���g�i���o�[
#define T07R0201_EVENT_RIVAL		0
#define T07R0201_EVENT_SUPPORT		1
#define T07R0201_EVENT_DOCTOR		2
#define T07R0201_EVENT_MAMA			3
#define T07R0201_EVENT_LE1			4
#define T07R0201_EVENT_LE2			5
#define T07R0201_EVENT_LE3			6
#define T07R0201_EVENT_LE4			7
#define T07R0201_EVENT_LE5			8
#define T07R0201_EVENT_LE6			9
#define T07R0201_EVENT_LE7			10
#define T07R0201_EVENT_LE8			11
#define T07R0201_EVENT_CHAMP		12
#define T07R0201_EVENT_BIG_LE		13
#define T07R0201_EVENT_LE_LE		14
#define T07R0201_EVENT_EVENT_MAX	15

//�Z�����F3�l���āA���ꂼ��̐F�̋Z���������Ă���Ȃ�
#define WAZA_OSHIE_COL_BLUE			(0)
#define WAZA_OSHIE_COL_RED			(1)
#define WAZA_OSHIE_COL_YELLOW		(2)

//�Z�o�����ʃE�F�C�g
#define WAZA_COMMON_WAIT			(30)
#define WAZA_COMMON_WAIT2			(32)
#define WAZA_COMMON_WAIT3			(16)

//���W�C�x���g���[�N�̒�`(���ޏꏊ�̐��ȏ�̓K���Ȓl)
//#define REZI_WORK_2_ALL_NUM			(7)					//r0102���̐�
//#define REZI_WORK_4_ALL_NUM			(7)					//r0104���̐�
//#define REZI_WORK_6_ALL_NUM			(7)					//r0106���̐�
#define REZI_WORK_2_ALL_NUM			(127)					//r0102���̐�(111 1111)
#define REZI_WORK_4_ALL_NUM			(127)					//r0104���̐�(111 1111)
#define REZI_WORK_6_ALL_NUM			(127)					//r0106���̐�(111 1111)
#define REZI_WORK_POS_OK			(260)				//�S�ē���
#define REZI_WORK_TALK_OK			(270)				//��x�b�������Đ퓬��������
#define REZI_WORK_CAPTURE_NG		(280)				//�ߊl���s
#define REZI_WORK_CAPTURE_OK		(290)				//�ߊl����

// �莝���|�P�������J�E���g���[�h
#define POKECOUNT_MODE_TOTAL         (0)  // �莝���̐�
#define POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S�������莝���̐�
#define POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P������
#define POKECOUNT_MODE_ONLY_EGG      (3)  // �^�}�S�̐�(�ʖڃ^�}�S������)


#endif	/* SCRIPT_DEF_H */


