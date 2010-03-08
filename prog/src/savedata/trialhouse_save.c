//============================================================================================
/**
 * @file	trialhouse_save.c
 * @brief	�g���C�A���n�E�X�Z�[�u�f�[�^
 * @author	saito
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/trialhouse_save_local.h"
#include "savedata/trialhouse_save.h"

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̃T�C�Y�擾
 * @return	int		�T�C�Y
 */
//---------------------------------------------------------------------------
int THSV_GetWorkSize(void)
{
	return sizeof(THSV_WORK);
}

//---------------------------------------------------------------------------
/**
 * @brief	���[�N�̏�����
 * @param	work		���[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void THSV_Init(THSV_WORK * work)
{
	MI_CpuClear8(work, sizeof(THSV_WORK));
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�|�C���^��Ԃ�
 * @param	work		���[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
THSV_WORK * THSV_GetSvPtr( SAVE_CONTROL_WORK *sv )
{
  return SaveControl_DataPtrGet(sv, GMDATA_ID_THSV);
}

