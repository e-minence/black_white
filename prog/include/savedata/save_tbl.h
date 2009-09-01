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
#define	SAVE_PAGE_MAX		(32)

#define	SAVE_SECTOR_SIZE	(0x1000)

//�ȉ��̓f�o�b�O�c�[���p�̃Z�[�u�̈�ł� by soga
#define EDIT_ANM_SAVE_2		( 45 * SAVE_SECTOR_SIZE )	//�|�P����anime�f�[�^
#define CB_EDIT_SAVE		( 55 * SAVE_SECTOR_SIZE )	//�J�v�Z���{�[���Z�[�u�f�[�^
#define EDIT_ANM_SAVE		( 58 * SAVE_SECTOR_SIZE )	//�|�P�����G�f�B�b�g�f�[�^
#define	POKE_ANM_SAVE		(60*SAVE_SECTOR_SIZE)		//�|�P�����A�j���c�[���p�Z�[�u�̈�i���g�p�̂͂��j
#define	DEBUG_FIGHT_SAVE	(62*SAVE_SECTOR_SIZE)		//�f�o�b�O�t�@�C�g�p�Z�[�u�̈�


///�Z�[�u�f�[�^����ID
enum{
	GMDATA_ID_CONTEST,
	GMDATA_ID_BOXDATA,	//�{�b�N�X�f�[�^�O���[�v
	GMDATA_ID_MYITEM,
	GMDATA_ID_MYPOKE,
	GMDATA_ID_PLAYER_DATA,
	GMDATA_ID_SITUATION,
	GMDATA_ID_WIFIHISTORY,
	GMDATA_ID_WIFILIST,
  GMDATA_ID_CGEAR,    //C-GEAR
	GMDATA_ID_TRCARD,		//�g���[�i�[�J�[�h(�T�C��
	GMDATA_ID_MYSTERYDATA,	//�s�v�c�ȑ��蕨
	GMDATA_ID_PERAPVOICE,	//�؃��b�v���H�C�X
	GMDATA_ID_SYSTEM_DATA,	//�V�X�e���f�[�^
	GMDATA_ID_RECORD,  		//���R�[�h(�X�R�A
	GMDATA_ID_PMS,  		//�ȈՕ�
	GMDATA_ID_MMDL,		//���샂�f��
	GMDATA_ID_WFHIROBA,		//Wi-Fi�L��
	GMDATA_ID_MUSICAL,		//�~���[�W�J��
  GMDATA_ID_RANDOMMAP,  //�����_�������}�b�v
	GMDATA_ID_IRCCOMPATIBLE,	//�����`�F�b�N
  GMDATA_ID_SP_RIBBON,	//�z�z���{��
  GMDATA_ID_EVENT_WORK, //�C�x���g���[�N
	GMDATA_ID_WORLDTRADEDATA,	//GTS
  GMDATA_ID_REGULATION_DATA, //���M�����[�V����
  GMDATA_ID_GIMMICK_WORK,    //�M�~�b�N

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

///�O���Z�[�u�f�[�^����ID�F�X�g���[�~���O
enum{
  EXGMDATA_ID_STREAMING,
};


//==============================================================================
//	�O���f�[�^�錾
//==============================================================================
extern const GFL_SVLD_PARAM SaveParam_Normal;
extern const GFL_SVLD_PARAM SaveParam_ExtraTbl[];


#endif	//__SAVE_H__

