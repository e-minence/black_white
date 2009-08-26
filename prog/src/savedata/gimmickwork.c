//============================================================================================
/**
 * @file	gimmickwork.c
 * @brief	�}�b�v�ŗL�̎d�|���p���[�N�����������߂̃\�[�X
 * @author	saito
 * @date	2009.08.26
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "gimmickwork_local.h"
#include "savedata/gimmickwork.h"

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̃T�C�Y�擾
 * @return	int		�T�C�Y
 */
//---------------------------------------------------------------------------
int GIMMICKWORK_GetWorkSize(void)
{
	return sizeof(GIMMICKWORK);
}

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̏�����
 * @param	gimmick		�d�|�����[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void GIMMICKWORK_Init(GIMMICKWORK * gimmick)
{
	MI_CpuClear8(gimmick, sizeof(GIMMICKWORK));
}

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̍ŏ��̔F�؏���
 * @param	gimmick		�d�|�����[�N�ւ̃|�C���^
 * @param	gimmick_id		�d�|����ID
 */
//---------------------------------------------------------------------------
void * GIMMICKWORK_Assign(GIMMICKWORK * gimmick, int gimmick_id)
{
	GIMMICKWORK_Init(gimmick);
	gimmick->id = gimmick_id;
	return gimmick->work;
}

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̎擾
 * @param	gimmick		�d�|�����[�N�ւ̃|�C���^
 * @param	gimmick_id		�g�p����T�C�Y
 */
//---------------------------------------------------------------------------
void * GIMMICKWORK_Get(GIMMICKWORK * gimmick, int gimmick_id)
{
	GF_ASSERT(gimmick->id == gimmick_id);
	return gimmick->work;
}

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N��ID�擾
 * @param	gimmick		�d�|�����[�N�ւ̃|�C���^
 * @return	int			ID
 */
//---------------------------------------------------------------------------
int GIMMICKWORK_GetAssignID(const GIMMICKWORK * gimmick)
{
	return gimmick->id;
}

//----------------------------------------------------------
/**
 * @brief	�f�[�^�ւ̃|�C���^�擾
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	�M�~�b�N�Z�[�u�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
GIMMICKWORK * SaveData_GetGimmickWork(SAVE_CONTROL_WORK * sv)
{
	return SaveControl_DataPtrGet(sv, GMDATA_ID_GIMMICK_WORK);
}


