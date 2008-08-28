//==============================================================================
/**
 * @file	savedata.c
 * @brief	�Z�[�u�f�[�^�Ǘ�
 * @author	matsuda
 * @date	2008.08.26(��)
 */
//==============================================================================
#include "gflib.h"

#include "savedata.h"
#include "savedata_local.h"
#include "savedata.h"


//�Z�[�u�f�[�^���e�������̂ɕK�v�Ȋ֐��Q�Ƃ̂��߂̃w�b�_
#include "contest_savedata.h"
//#include "misc.h"
//#include "misc_local.h"


//=============================================================================
//=============================================================================

//============================================================================================
//
//
//		�����f�[�^
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���e��`
 */
//---------------------------------------------------------------------------
const SAVEDATA_TABLE SaveDataTable[] = {
//�m�[�}���f�[�^�O���[�v
	{	//�R���e�X�g
		GMDATA_ID_CONTEST,
		SVBLK_ID_NORMAL,
		(FUNC_GET_SIZE)CONDATA_GetWorkSize,
		(FUNC_INIT_WORK)CONDATA_Init,
	},
#if 0
	{	//���̑��f�[�^
		GMDATA_ID_MISC,
		SVBLK_ID_NORMAL,
		(FUNC_GET_SIZE)MISC_GetWorkSize,
		(FUNC_INIT_WORK)MISC_Init,
	},
#endif


//�{�b�N�X�f�[�^�O���[�v
#if 0
	{	//�|�P�����{�b�N�X�̃f�[�^
		GMDATA_ID_BOXDATA,
		SVBLK_ID_BOX,
		(FUNC_GET_SIZE)BOXDAT_GetTotalSize,
		(FUNC_INIT_WORK)BOXDAT_Init,
	},
#endif
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const int SaveDataTableMax = NELEMS(SaveDataTable);



//============================================================================================
//
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�{�b�N�X�f�[�^�̃|�C���^�擾
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOX_DATA	�{�b�N�X�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
#if 0
BOX_DATA * SaveData_GetBoxData(SAVEDATA * sv)
{
	return SaveData_Get(sv, GMDATA_ID_BOXDATA);
}
#endif

//============================================================================================
//
//
//			����Z�[�u�f�[�^
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^�̒�`�e�[�u��
 */
//---------------------------------------------------------------------------
const EXSAVEDATA_TABLE ExtraSaveDataTable[] = {
	{
		// �a������f�[�^
		EXDATA_ID_DENDOU,
		SAVE_PAGE_MAX + 0,
		(FUNC_GET_SIZE)DendouData_GetWorkSize,
		(FUNC_INIT_WORK)DendouData_Init,
	},

	{	//�o�g���t�����e�B�A�O���f�[�^(�i�s�Ɋ֌W�Ȃ��L�^�̂�)
		EXDATA_ID_FRONTIER,
		SAVE_PAGE_MAX + 3,
		(FUNC_GET_SIZE)FrontierEx_GetWorkSize,
		(FUNC_INIT_WORK)FrontierEx_Init,
	},
	
	{	//�퓬�^��F����
		EXDATA_ID_BATTLE_REC_MINE,
		SAVE_PAGE_MAX + 4,
		(FUNC_GET_SIZE)BattleRec_GetWorkSize,
		(FUNC_INIT_WORK)BattleRec_WorkInit,
	},
	{	//�퓬�^��F�_�E�����[�h0��
		EXDATA_ID_BATTLE_REC_DL_0,
		SAVE_PAGE_MAX + 6,
		(FUNC_GET_SIZE)BattleRec_GetWorkSize,
		(FUNC_INIT_WORK)BattleRec_WorkInit,
	},
	{	//�퓬�^��F�_�E�����[�h1��
		EXDATA_ID_BATTLE_REC_DL_1,
		SAVE_PAGE_MAX + 8,
		(FUNC_GET_SIZE)BattleRec_GetWorkSize,
		(FUNC_INIT_WORK)BattleRec_WorkInit,
	},
	{	//�퓬�^��F�_�E�����[�h2��
		EXDATA_ID_BATTLE_REC_DL_2,
		SAVE_PAGE_MAX + 10,
		(FUNC_GET_SIZE)BattleRec_GetWorkSize,
		(FUNC_INIT_WORK)BattleRec_WorkInit,
	},
};
//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^�̃e�[�u����
 */
//---------------------------------------------------------------------------
const int ExtraSaveDataTableMax = NELEMS(ExtraSaveDataTable);



//============================================================================================
//
//
//			�ʂ̓���Z�[�u�f�[�^�A�N�Z�X�֐�
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^���[�h�F�a������f�[�^
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	heap_id		���[�N���擾����q�[�v��ID
 * @param	result
 * @return	void *	�擾�����Z�[�u�f�[�^���ڂ������[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
DENDOU_SAVEDATA * SaveData_Extra_LoadDendouData(SAVEDATA * sv, int heap_id, LOAD_RESULT * result)
{
	return SaveData_Extra_LoadAlloc(sv, heap_id, EXDATA_ID_DENDOU, result);
}

//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^�Z�[�u�F�a������f�[�^
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	data		�a������f�[�^�ւ̃|�C���^
 * @retval	SAVE_RESULT_OK		�Z�[�u����
 * @retval	SAVE_RESULT_NG		�Z�[�u���s
 */
//---------------------------------------------------------------------------
SAVE_RESULT SaveData_Extra_SaveDendouData(SAVEDATA * sv, DENDOU_SAVEDATA * data)
{
	return SaveData_Extra_Save(sv, EXDATA_ID_DENDOU, data);
}


//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^���[�h�F�퓬�^��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	heap_id		���[�N���擾����q�[�v��ID
 * @param	result
 * @param   num			LOADDATA_MYREC, LOADDATA_DOWNLOAD0, ...
 * @return	void *	�擾�����Z�[�u�f�[�^���ڂ������[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
BATTLE_REC_SAVEDATA * SaveData_Extra_LoadBattleRecData(SAVEDATA * sv, int heap_id, LOAD_RESULT * result, int num)
{
	BOOL old;
	return SaveData_Extra_Mirror_LoadAlloc(sv, heap_id, EXDATA_ID_BATTLE_REC_MINE + num, result, &old);
}

//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^�Z�[�u�F�퓬�^��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	data		�퓬�^��f�[�^�ւ̃|�C���^(CHECK_TAIL_DATA�\���̕����܂߂����̂ł���
 * 						�K�v������̂�SaveData_Extra_LoadBattleRecData�֐��̖߂�l�Ŏ�ɓ��ꂽ
 *						�o�b�t�@�|�C���^��n���K�v������B
 * @param   num			LOADDATA_MYREC, LOADDATA_DOWNLOAD0, ...
 *
 * @retval	SAVE_RESULT_OK		�Z�[�u����
 * @retval	SAVE_RESULT_NG		�Z�[�u���s
 */
//---------------------------------------------------------------------------
SAVE_RESULT SaveData_Extra_SaveBattleRecData(SAVEDATA * sv, BATTLE_REC_SAVEDATA * data, int num)
{
	return SaveData_Extra_Mirror_Save(sv, EXDATA_ID_BATTLE_REC_MINE + num, data);
}

//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^���[�h�F�t�����e�B�A�O���f�[�^
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	heap_id		���[�N���擾����q�[�v��ID
 * @param	result
 * @return	void *	�擾�����Z�[�u�f�[�^���ڂ������[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
FRONTIER_EX_SAVEDATA * SaveData_Extra_LoadFrontierEx(SAVEDATA * sv, int heap_id, LOAD_RESULT * result)
{
	BOOL old;
	return SaveData_Extra_Mirror_LoadAlloc(sv, heap_id, EXDATA_ID_FRONTIER, result, &old);
}

//---------------------------------------------------------------------------
/**
 * @brief	����Z�[�u�f�[�^�Z�[�u�F�t�����e�B�A�O���f�[�^
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	data		�퓬�^��f�[�^�ւ̃|�C���^(CHECK_TAIL_DATA�\���̕����܂߂����̂ł���
 * 						�K�v������̂�SaveData_Extra_LoadFrontierEx�֐��̖߂�l�Ŏ�ɓ��ꂽ
 *						�o�b�t�@�|�C���^��n���K�v������B
 *
 * @retval	SAVE_RESULT_OK		�Z�[�u����
 * @retval	SAVE_RESULT_NG		�Z�[�u���s
 */
//---------------------------------------------------------------------------
SAVE_RESULT SaveData_Extra_SaveFrontierEx(SAVEDATA * sv, FRONTIER_EX_SAVEDATA * data)
{
	return SaveData_Extra_Mirror_Save(sv, EXDATA_ID_FRONTIER, data);
}


