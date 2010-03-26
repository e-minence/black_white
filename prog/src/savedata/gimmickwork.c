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
#include "savedata/gimmickwork_local.h"
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
  gimmick->id = GIMMICK_NO_ASSIGN;
}

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̃N���A�i����M�~�b�N���[�N�̃N���A�͍s��Ȃ��j
 * @param	gimmick		�d�|�����[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void GIMMICKWORK_ClearWork(GIMMICKWORK * gimmick)
{
  int i;
  gimmick->id = GIMMICK_NO_ASSIGN;
  for(i=0;i<32;i++)
  {
    gimmick->work[i] = 0;
  }
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
  if (gimmick->id != GIMMICK_NO_ASSIGN){
    GF_ASSERT_MSG(0,"��d�A�T�C�� %d => %d",gimmick->id, gimmick_id);
  }
	GIMMICKWORK_ClearWork(gimmick);
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
	GF_ASSERT_MSG(gimmick->id == gimmick_id," %d  %d\n",gimmick->id,gimmick_id);
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
#if 0
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
#endif
//----------------------------------------------------------
/**
 * @brief	�Z�[�u���[�N�փf�[�^���R�s�[
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	�M�~�b�N�Z�[�u�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
void SaveData_SaveGimmickWork(const GIMMICKWORK * inGimmick, SAVE_CONTROL_WORK * sv)
{
  GIMMICKWORK *gimmick;
  gimmick = SaveControl_DataPtrGet(sv, GMDATA_ID_GIMMICK_WORK);
	MI_CpuCopy32(inGimmick, gimmick, sizeof(GIMMICKWORK));
}

//----------------------------------------------------------
/**
 * @brief	�M�~�b�N���[�N�փ��[�h
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	�M�~�b�N�Z�[�u�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
void SaveData_LoadGimmickWork(SAVE_CONTROL_WORK * sv, GIMMICKWORK * outGimmick)
{
  GIMMICKWORK *gimmick;
  gimmick = SaveControl_DataPtrGet(sv, GMDATA_ID_GIMMICK_WORK);
	MI_CpuCopy32(gimmick, outGimmick, sizeof(GIMMICKWORK));
}




