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
#include "savedata/save_tbl.h"
#include "savedata/contest_savedata.h"


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


static const GFL_SAVEDATA_TABLE SaveDataTbl_Normal[] = {
	{	//�R���e�X�g
		GMDATA_ID_CONTEST,
//		SVBLK_ID_NORMAL,
		(FUNC_GET_SIZE)CONDATA_GetWorkSize,
		(FUNC_INIT_WORK)CONDATA_Init,
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
	SAVESIZE_NORMAL,				//�g�p����o�b�N�A�b�v�̈�̑傫��
	MAGIC_NUMBER,
	GFL_BACKUP_NORMAL_FLASH,
};

