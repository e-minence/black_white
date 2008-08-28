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
	GMDATA_ID_MATSU_DEBUG,
	
#if 0
	GMDATA_ID_SYSTEM_DATA,
	GMDATA_ID_PLAYER_DATA,
	GMDATA_ID_TEMOTI_POKE,
	GMDATA_ID_TEMOTI_ITEM,
	GMDATA_ID_EVENT_WORK,
	GMDATA_ID_POKETCH_DATA,
	GMDATA_ID_SITUATION,
	GMDATA_ID_ZUKANWORK,
	GMDATA_ID_SODATEYA,
	GMDATA_ID_FRIEND,
	GMDATA_ID_MISC,
	GMDATA_ID_FIELDOBJSV,
	GMDATA_ID_UNDERGROUNDDATA,
    GMDATA_ID_REGULATION,
	GMDATA_ID_IMAGECLIPDATA,
	GMDATA_ID_MAILDATA,
	GMDATA_ID_PORUTODATA,
	GMDATA_ID_RANDOMGROUP,
	GMDATA_ID_FNOTE,
	GMDATA_ID_TRCARD,
	GMDATA_ID_RECORD,
	GMDATA_ID_CUSTOM_BALL,
	GMDATA_ID_PERAPVOICE,
	GMDATA_ID_FRONTIER,
	GMDATA_ID_SP_RIBBON,
	GMDATA_ID_ENCOUNT,
	GMDATA_ID_WORLDTRADEDATA,
	GMDATA_ID_TVWORK,
	GMDATA_ID_GUINNESS,
	GMDATA_ID_WIFILIST,
	GMDATA_ID_WIFIHISTORY,
	GMDATA_ID_FUSHIGIDATA,
	GMDATA_ID_POKEPARKDATA,
	GMDATA_ID_CONTEST,
	GMDATA_ID_PMS,
	GMDATA_ID_EMAIL,
	GMDATA_ID_WFHIROBA,
#endif

	//�{�b�N�X�f�[�^�O���[�v
	GMDATA_ID_BOXDATA,

	GMDATA_ID_MAX,
};


//==============================================================================
//	�O���f�[�^�錾
//==============================================================================
extern const GFL_SVLD_PARAM SaveParam_Normal;


#endif	//__SAVE_H__

