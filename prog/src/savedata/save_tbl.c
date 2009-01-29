//==============================================================================
/**
 * @file	save.c
 * @brief	WB�Z�[�u
 * @author	matsuda
 * @date	2008.08.27(��)
 */
//==============================================================================
#include <gflib.h>
#include <backup_system.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/contest_savedata.h"
#include "savedata/test_savedata.h"
#include "savedata/box_savedata.h"
#include "savedata/myitem_savedata.h"
#include "poke_tool/pokeparty.h"
#include "playerdata_local.h"
#include "savedata/wifihistory.h"
#include "savedata/wifilist.h"
#include "savedata/trainercard_data.h"
#include "savedata/mystery_data.h"
#include "savedata/situation.h"
#include "perapvoice_local.h"
#include "savedata/system_data.h"
#include "savedata/record.h"


//==============================================================================
//	�萔��`
//==============================================================================
#define MAGIC_NUMBER	(0x31053527)
#define	SECTOR_SIZE		(SAVE_SECTOR_SIZE)
#define SECTOR_MAX		(SAVE_PAGE_MAX)

///�m�[�}���̈�Ŏg�p����Z�[�u�T�C�Y
#define SAVESIZE_NORMAL		(SECTOR_SIZE * SECTOR_MAX)
///EXTRA1�̈�Ŏg�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA1		(SECTOR_SIZE)
///EXTRA2�̈�Ŏg�p����Z�[�u�T�C�Y
#define SAVESIZE_EXTRA2		(SECTOR_SIZE)

///�~���[�����O�̈�Ŏg�p����Z�[�u�J�n�A�h���X
#define MIRROR_SAVE_ADDRESS		(0x20000)


static const GFL_SAVEDATA_TABLE SaveDataTbl_Normal[] = {
	{	//�R���e�X�g
		GMDATA_ID_CONTEST,
//		SVBLK_ID_NORMAL,
		(FUNC_GET_SIZE)CONDATA_GetWorkSize,
		(FUNC_INIT_WORK)CONDATA_Init,
	},
	{	//���c�f�o�b�O�@�\�p�Z�[�u
		GMDATA_ID_MATSU_DEBUG,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_0,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_0,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_1,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_1,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_2,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_2,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_3,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_3,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_4,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_4,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_5,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_5,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_6,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_6,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_7,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_7,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_8,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_8,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_9,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_9,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_a,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_a,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_b,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_b,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_c,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_c,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_d,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_d,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},
	{	//�e�X�g
		GMDATA_ID_MATSU_DEBUG_e,
		(FUNC_GET_SIZE)DebugMatsuSave_GetWorkSize_e,
		(FUNC_INIT_WORK)DebugMatsuSave_Init,
	},

	{	//BOX�f�[�^(���g�͉��ł� Ari081107
		GMDATA_ID_BOXDATA,
		(FUNC_GET_SIZE)BOXDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXDAT_Init,
	},
	{	//�莝���A�C�e��
		GMDATA_ID_MYITEM,
		(FUNC_GET_SIZE)MYITEM_GetWorkSize,
		(FUNC_INIT_WORK)MYITEM_Init,
	},
	{	//�莝���|�P����
		GMDATA_ID_MYPOKE,
		(FUNC_GET_SIZE)PokeParty_GetWorkSize,
		(FUNC_INIT_WORK)PokeParty_InitWork,
	},
	{	//�v���C���[�f�[�^
		GMDATA_ID_PLAYER_DATA,
		(FUNC_GET_SIZE)PLAYERDATA_GetWorkSize,
		(FUNC_INIT_WORK)PLAYERDATA_Init,
	},
	{	//�󋵃f�[�^
		GMDATA_ID_SITUATION,
		(FUNC_GET_SIZE)Situation_GetWorkSize,
		(FUNC_INIT_WORK)Situation_Init,
	},
	{	//WiFi�ʐM�����f�[�^
		GMDATA_ID_WIFIHISTORY,
		(FUNC_GET_SIZE)WIFIHISTORY_GetWorkSize,
		(FUNC_INIT_WORK)WIFIHISTORY_Init,
	},
	{	//WiFi�F�B�R�[�h�f�[�^
		GMDATA_ID_WIFILIST,
		(FUNC_GET_SIZE)WifiList_GetWorkSize,
		(FUNC_INIT_WORK)WifiList_Init,
	},
	{	//�g���[�i�[�J�[�h(�T�C��
		GMDATA_ID_TRCARD,
		(FUNC_GET_SIZE)TRCSave_GetSaveDataSize,
		(FUNC_INIT_WORK)TRCSave_InitSaveData,
	},
	{	//�s�v�c�ȑ��蕨
		GMDATA_ID_MYSTERYDATA,
		(FUNC_GET_SIZE)MYSTERYDATA_GetWorkSize,
		(FUNC_INIT_WORK)MYSTERYDATA_Init,
	},
	{	//�؃��b�v�{�C�X
		GMDATA_ID_PERAPVOICE,
		(FUNC_GET_SIZE)PERAPVOICE_GetWorkSize,
		(FUNC_INIT_WORK)PERAPVOICE_Init,
	},
	{	//�V�X�e���f�[�^
		GMDATA_ID_SYSTEM_DATA,
		(FUNC_GET_SIZE)SYSTEMDATA_GetWorkSize,
		(FUNC_INIT_WORK)SYSTEMDATA_Init,
	},
	{	//���R�[�h(�X�R�A
		GMDATA_ID_RECORD,
		(FUNC_GET_SIZE)RECORD_GetWorkSize,
		(FUNC_INIT_WORK)RECORD_Init,
	},
};


//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�p�����[�^�e�[�u��
 */
//--------------------------------------------------------------
const GFL_SVLD_PARAM SaveParam_Normal = {
	SaveDataTbl_Normal,
	NELEMS(SaveDataTbl_Normal),
	0,								//�o�b�N�A�b�v�̈�擪�A�h���X
	MIRROR_SAVE_ADDRESS,			//�~���[�����O�̈�擪�A�h���X
	SAVESIZE_NORMAL,				//�g�p����o�b�N�A�b�v�̈�̑傫��
	MAGIC_NUMBER,
	GFL_BACKUP_NORMAL_FLASH,
};

