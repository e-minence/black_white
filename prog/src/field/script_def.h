//======================================================================
/**
 * @file	script_def.h
 * @brief	�X�N���v�g��`�t�@�C��
 * @author	Satoshi Nohara
 * @date	05.12.14
 *
 * �X�N���v�g���ƃv���O�����Ƃŋ��ʂ��ĎQ�Ƃ���K�v�������`���W�񂷂�w�b�_�t�@�C���B
 * �X�N���v�g����enum�Ȃ�C���L�̕��@���߂��ł��Ȃ����߁Adefine���g�p���邱�ƁB
 */
//======================================================================
#pragma once

//======================================================================
//
//  �X�N���v�g�w�b�_�֘A
//
//  �X�N���v�g���ł̓}�N���ɖ��ߍ��܂�Ă��邽�߁A���ڂ����̒l��
//  �ӎ��I�Ɏg�p���邱�Ƃ͂Ȃ��B
//======================================================================
//--------------------------------------------------------------
//����X�N���v�g��`
//--------------------------------------------------------------
#define SP_SCRID_NONE				(0)
#define SP_SCRID_SCENE_CHANGE		(1)     ///<����X�N���v�g�e�[�u���ł̃V�[�������C�x���g�f�[�^�̎w��ID
#define SP_SCRID_ZONE_CHANGE		(2)     ///<����X�N���v�g�e�[�u���ł̃]�[���؂�ւ��X�N���v�g�̎w��ID
#define SP_SCRID_FIELD_RECOVER	(3)			///<����X�N���v�g�e�[�u���ł̃t�B�[���h���A�X�N���v�g�w��ID
#define SP_SCRID_FIELD_INIT 		(4)			///<����X�N���v�g�e�[�u���ł̃t�B�[���h�������X�N���v�g�w��ID

///�C�x���g�f�[�^�e�[�u�������𔻒肷�邽�߂̒l
#define EV_DATA_END_CODE  (0xfd13)  //�K���Ȓl

//======================================================================
//
//    OBJ�֘A
//
//======================================================================
//--------------------------------------------------------------
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

//======================================================================
//    �e�R�}���h�ˑ��̒�`
//======================================================================
//--------------------------------------------------------------
//fld_menu.c
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/// BmpMenu�̐������b�Z�[�W����ID
//--------------------------------------------------------------
#define SCR_BMPMENU_EXMSG_NULL   (0xFFFF)

//--------------------------------------------------------------
//  �o�g���֘A
//--------------------------------------------------------------
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

//--------------------------------------------------------------
//�j���擾�֐��̖߂�l
//  _GET_WEEK
//--------------------------------------------------------------
#define RTC_WEEK_SUNDAY		0
#define RTC_WEEK_MONDAY		1
#define RTC_WEEK_TUESDAY	2
#define RTC_WEEK_WEDNESDAY	3
#define RTC_WEEK_THURSDAY	4
#define RTC_WEEK_FRIDAY		5
#define RTC_WEEK_SATURDAY	6

//--------------------------------------------------------------
//�o�b�W�t���O�w��ID�̒�`
//  _GET_BADGE_FLAG/_SET_BADGE_FLAG
//--------------------------------------------------------------
#define BADGE_ID_01   0
#define BADGE_ID_02   1
#define BADGE_ID_03   2
#define BADGE_ID_04   3
#define BADGE_ID_05   4
#define BADGE_ID_06   5
#define BADGE_ID_07   6
#define BADGE_ID_08   7

//--------------------------------------------------------------
// �莝���|�P�������J�E���g���[�h
//--------------------------------------------------------------
#define POKECOUNT_MODE_TOTAL         (0)  // �莝���̐�
#define POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S�������莝���̐�
#define POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P������
#define POKECOUNT_MODE_ONLY_EGG      (3)  // �^�}�S�̐�(�ʖڃ^�}�S������)
#define POKECOUNT_MODE_ONLY_DAME_EGG (4)  // �ʖڃ^�}�S�̐�
#define POKECOUNT_MODE_EMPTY         (5)  // �󂢂Ă��鐔


//--------------------------------------------------------------
// �|�P�����ߊl�ꏊ�@�`�F�b�N
//--------------------------------------------------------------
#define POKE_GET_PLACE_CHECK_WF         (0)  // �z���C�g�t�H���X�g�ŕߊl�����̂��`�F�b�N
#define POKE_GET_PLACE_CHECK_MAX        (1)  // �z���C�g�t�H���X�g�ŕߊl�����̂��`�F�b�N

//--------------------------------------------------------------
//  �����񑀍샂�[�h�w��
//--------------------------------------------------------------
#define ZUKANCTRL_MODE_SEE     (0)   ///<�����񑀍샂�[�h�F������
#define ZUKANCTRL_MODE_GET     (1)   ///<�����񑀍샂�[�h�F�߂܂�����

//--------------------------------------------------------------
//  �z�u���f���p�A�j���w��
//--------------------------------------------------------------
#define SCR_BMID_NULL           (0)       ///<�z�u���f�����ށF�Ȃ�
#define SCR_BMID_DOOR           (1)       ///<�z�u���f�����ށF�h�A
#define SCR_BMID_SANDSTREAM     (2)       ///<�z�u���f�����ށF����
#define SCR_BMID_PCMACHINE      (3)       ///<�z�u���f�����ށF�񕜃}�V��
#define SCR_BMID_PC             (4)       ///<�z�u���f�����ށF�p�\�R��
#define SCR_BMID_PCEV_DOOR      (5)       ///<�z�u���f�����ށF�|�P�Z���G���x�[�^�[�F�h�A
#define SCR_BMID_PCEV_FLOOR     (6)       ///<�z�u���f�����ށF�|�P�Z���G���x�[�^�[�F�t���A
#define SCR_BMID_WARP           (7)       ///<�z�u���f�����ށF���[�v

#define  SCR_BMANM_DOOR_OPEN    (0)       ///<�h�A�A�j���F�J��
#define  SCR_BMANM_DOOR_CLOSE   (1)       ///<�h�A�A�j���F����

#define  SCR_BMANM_PCEV_OPEN    (0)       ///<�|�P�Z���G���x�[�^�A�j���F�J��
#define  SCR_BMANM_PCEV_CLOSE   (1)       ///<�|�P�Z���G���x�[�^�A�j���F����

#define  SCR_BMANM_PCEV_UP      (0)       ///<�|�P�Z���G���x�[�^�A�j���F�オ��
#define  SCR_BMANM_PCEV_DOWN    (1)       ///<�|�P�Z���G���x�[�^�A�j���F������
//--------------------------------------------------------------
/// ���b�Z�[�W�E�B���h�E�㉺�w��
//--------------------------------------------------------------
#define SCRCMD_MSGWIN_UP (0)
#define SCRCMD_MSGWIN_DOWN (1)
#define SCRCMD_MSGWIN_NON (2)

//--------------------------------------------------------------
/// BG�E�B���h�E�^�C�v
//--------------------------------------------------------------
#define SCRCMD_BGWIN_TYPE_INFO (0)
#define SCRCMD_BGWIN_TYPE_TOWN (1)
#define SCRCMD_BGWIN_TYPE_POST (2)
#define SCRCMD_BGWIN_TYPE_ROAD (3)

//--------------------------------------------------------------
/// ����E�B���h�E�^�C�v
//--------------------------------------------------------------
#define SCRCMD_SPWIN_TYPE_LETTER (0)
#define SCRCMD_SPWIN_TYPE_BOOK (1)

//--------------------------------------------------------------
/// �o�g�����R�[�_�[�Ăяo�����[�h�w��
//--------------------------------------------------------------
#define SCRCMD_BTL_RECORDER_MODE_VIDEO   (0)
#define SCRCMD_BTL_RECORDER_MODE_MUSICAL (1)

//--------------------------------------------------------------
/// �ΐ펞�p�[�e�B�^�C�v�w��
//--------------------------------------------------------------
#define SCR_BTL_PARTY_SELECT_TEMOTI   (0) //�莝��
#define SCR_BTL_PARTY_SELECT_BTLBOX   (1) //�o�g���{�b�N�X
#define SCR_BTL_PARTY_SELECT_CANCEL   (2) //�I���L�����Z��
#define SCR_BTL_PARTY_SELECT_NG       (3) //���M�����[�V����NG

//--------------------------------------------------------------
/// �Q�[���������\�`�F�b�N�߂�l
//--------------------------------------------------------------
#define FLD_TRADE_ENABLE         (0)  // ������
#define FLD_TRADE_DISABLE_EGG    (1)  // �����s��(�^�}�S�̂���)
#define FLD_TRADE_DISABLE_MONSNO (2)  // �����s��(�Ⴄ�����X�^�[�̂���)
#define FLD_TRADE_DISABLE_SEX    (3)  // �����s��(�Ⴄ���ʂ̂���)

//--------------------------------------------------------------
/// ���C�����X�Z�[�u���[�h
//--------------------------------------------------------------
#define WIRELESS_SAVEMODE_OFF (0)
#define WIRELESS_SAVEMODE_ON  (1)

//--------------------------------------------------------------
/// �Z�v���o������
//--------------------------------------------------------------
#define SCR_WAZAOSHIE_RET_SET    (0)
#define SCR_WAZAOSHIE_RET_CANCEL (1)

//--------------------------------------------------------------
/// �{�b�N�X�I�����[�h
//--------------------------------------------------------------
#define SCR_BOX_END_MODE_MENU   (0)  // �I�����j���[�֖߂�
#define SCR_BOX_END_MODE_C_GEAR (1)  // C-gear�܂Ŗ߂�

//--------------------------------------------------------------
//--------------------------------------------------------------
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
