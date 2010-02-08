//==============================================================================
/**
 * @file	save_tbl.h
 * @brief	WB�Z�[�u
 * @author	matsuda
 * @date	2008.08.27(��)
 */
//==============================================================================
#ifndef __SAVE_H__
#define __SAVE_H__

#include <backup_system.h>


//---------------------------------------------------------------------------
///	�Z�[�u�Ɏg�p���Ă���Z�N�^��
//---------------------------------------------------------------------------
#define	SAVE_PAGE_MAX		(36)

#define	SAVE_SECTOR_SIZE	(0x1000)

//�ȉ��̓f�o�b�O�c�[���p�̃Z�[�u�̈�ł�
#define	DEBUG_FIGHT_SAVE	(126*SAVE_SECTOR_SIZE)		//�f�o�b�O�t�@�C�g�p�Z�[�u�̈�
#define DEBUG_SURETIGAI_SAVE  (125*SAVE_SECTOR_SIZE)  //����Ⴂ�ʐM�p�Z�[�u�̈�

///EXTRA�̈�Ő퓬�^�悪�g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_BATTLE_REC   (SAVE_SECTOR_SIZE * 2)
///EXTRA�̈��CGEAR�摜���g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_CGEAR_PICTURE   (SAVE_SECTOR_SIZE * 3)
///EXTRA�̈�Ńo�g�����肪�g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_BATTLE_EXAMINATION   (SAVE_SECTOR_SIZE * 1)
///EXTRA�̈�ŃX�g���[�~���O���g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_STREAMING    (SAVE_SECTOR_SIZE * 32)  //128Kbyte
///EXTRA�̈�Ő}�ӕǎ����g�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA_ZUKAN_WALLPAPER    (SAVE_SECTOR_SIZE * 7)  //CGX6000 + PLTT1000


///�Z�[�u�f�[�^����ID
enum{
	GMDATA_ID_BOXDATA,	      //�{�b�N�X�f�[�^�O���[�v
	GMDATA_ID_BOXTRAY_01,	    //�{�b�N�X�g���[�f�[�^
	GMDATA_ID_BOXTRAY_02,
	GMDATA_ID_BOXTRAY_03,
	GMDATA_ID_BOXTRAY_04,
	GMDATA_ID_BOXTRAY_05,
	GMDATA_ID_BOXTRAY_06,
	GMDATA_ID_BOXTRAY_07,
	GMDATA_ID_BOXTRAY_08,
	GMDATA_ID_BOXTRAY_09,
	GMDATA_ID_BOXTRAY_10,
	GMDATA_ID_BOXTRAY_11,
	GMDATA_ID_BOXTRAY_12,
	GMDATA_ID_BOXTRAY_13,
	GMDATA_ID_BOXTRAY_14,
	GMDATA_ID_BOXTRAY_15,
	GMDATA_ID_BOXTRAY_16,
	GMDATA_ID_BOXTRAY_17,
	GMDATA_ID_BOXTRAY_18,
	GMDATA_ID_BOXTRAY_19,
	GMDATA_ID_BOXTRAY_20,
	GMDATA_ID_BOXTRAY_21,
	GMDATA_ID_BOXTRAY_22,
	GMDATA_ID_BOXTRAY_23,
	GMDATA_ID_BOXTRAY_24,
	GMDATA_ID_MYITEM,           // �ǂ���
	GMDATA_ID_MYPOKE,           // �Ă����|�P����
	GMDATA_ID_PLAYER_DATA,      // �v���C���[�֘A�Z�[�u�f�[�^
	GMDATA_ID_SITUATION,        // �t�B�[���h�󋵃f�[�^
	GMDATA_ID_WIFIHISTORY,      // Wi-Fi�V�񂾗���
	GMDATA_ID_WIFILIST,         // �Ƃ������R�[�h
	GMDATA_ID_WIFI_NEGOTIATION, // Wi-Fi�l�S�V�G�[�V�����p���������l��PID
  GMDATA_ID_CGEAR,            // C-GEAR
	GMDATA_ID_TRCARD,		        // �g���[�i�[�J�[�h(�T�C��
	GMDATA_ID_MYSTERYDATA,	    // �s�v�c�ȑ��蕨
  GMDATA_ID_DREAMWORLD,       // PDW
	GMDATA_ID_PERAPVOICE,	      // �؃��b�v���H�C�X
	GMDATA_ID_SYSTEM_DATA,	    // �V�X�e���f�[�^
	GMDATA_ID_RECORD,  		      // ���R�[�h(�X�R�A
	GMDATA_ID_PMS,  		        // �ȈՕ�
  GMDATA_ID_MAILDATA,         // ���[���f�[�^
	GMDATA_ID_MMDL,		          // ���샂�f��
	GMDATA_ID_MUSICAL,		      // �~���[�W�J��
  GMDATA_ID_RANDOMMAP,        // �����_�������}�b�v
	GMDATA_ID_IRCCOMPATIBLE,	  // �����`�F�b�N
  GMDATA_ID_EVENT_WORK,       // �C�x���g���[�N
	GMDATA_ID_WORLDTRADEDATA,	  // GTS
  GMDATA_ID_REGULATION_DATA,  // ���M�����[�V����
  GMDATA_ID_GIMMICK_WORK,     // �M�~�b�N
  GMDATA_ID_BATTLE_BOX,       // �o�g���{�b�N�X
  GMDATA_ID_SODATEYA_WORK,    // ��ĉ����[�N
  GMDATA_ID_ROCKPOS,          // �����肫��
	GMDATA_ID_MISC,			        // ������(�������f�[�^�̏W�܂�j
	GMDATA_ID_INTRUDE,          // �N��
	GMDATA_ID_SHORTCUT,	        // Y�{�^���o�^
	GMDATA_ID_ZUKAN,	          // �|�P�����}��
	GMDATA_ID_ENCOUNT,	        // �G���J�E���g�֘A�Z�[�u�f�[�^
  GMDATA_ID_BSUBWAY_PLAYDATA, // �o�g���T�u�E�F�C�@�v���C�f�[�^
  GMDATA_ID_BSUBWAY_SCOREDATA,// �o�g���T�u�E�F�C�@�X�R�A�f�[�^
  GMDATA_ID_RNDMATCH,         // �����_���}�b�`
  GMDATA_ID_RADAR,            // �������[�_�[
  GMDATA_ID_SYMBOL,           // �V���{���G���J�E���g
  GMDATA_ID_MYPMS,            // ��l���̈��A���f�[�^
  GMDATA_ID_QUESTIONNAIRE,    // ����Ⴂ�A���P�[�g
  GMDATA_ID_UNSV,             // ���A

	GMDATA_ID_MAX,
};

///�O���Z�[�u�f�[�^����ID�F�퓬�^��F����
enum{
  EXGMDATA_ID_BATTLE_REC_MINE,
};

///�O���Z�[�u�f�[�^����ID�F�퓬�^��F�_�E�����[�h0��
enum{
  EXGMDATA_ID_BATTLE_REC_DL_0,
};

///�O���Z�[�u�f�[�^����ID�F�퓬�^��F�_�E�����[�h1��
enum{
  EXGMDATA_ID_BATTLE_REC_DL_1,
};

///�O���Z�[�u�f�[�^����ID�F�퓬�^��F�_�E�����[�h2��
enum{
  EXGMDATA_ID_BATTLE_REC_DL_2,
};


///�O���Z�[�u�f�[�^����ID�FCGEAR�s�N�`���[
enum{
  EXGMDATA_ID_CGEAR_PICTURE,
};

///�O���Z�[�u�f�[�^����ID�FCGEAR�s�N�`���[
enum{
  EXGMDATA_ID_BATTLE_EXAMINATION,
};

///�O���Z�[�u�f�[�^����ID�F�X�g���[�~���O
enum{
  EXGMDATA_ID_STREAMING,
};

///�O���Z�[�u�f�[�^����ID�F�}�ӕǎ�
enum{
  EXGMDATA_ID_ZUKAN_WALLPAPER,
};

///�O���Z�[�u�f�[�^����ID�F�f�o�b�O�o�g��
enum{
  EXGMDATA_ID_DEBUG_BATTLE,
};


//==============================================================================
//	�O���f�[�^�錾
//==============================================================================
extern const GFL_SVLD_PARAM SaveParam_Normal;
extern const GFL_SVLD_PARAM SaveParam_ExtraTbl[];


#endif	//__SAVE_H__

