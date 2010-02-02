//============================================================================================
/**
 * @file	un_savedata.c
 * @brief	���A�֘A�Z�[�u�f�[�^
 * @author	saito
 * @date	2010.02.02
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/un_savedata_local.h"
#include "savedata/un_savedata.h"

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̃T�C�Y�擾
 * @return	int		�T�C�Y
 */
//---------------------------------------------------------------------------
int UNSV_GetWorkSize(void)
{
	return sizeof(UNSV_WORK);
}

//---------------------------------------------------------------------------
/**
 * @brief	�d�|���p���[�N�̏�����
 * @param	gimmick		�d�|�����[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void UNSV_Init(UNSV_WORK * work)
{
	MI_CpuClear8(work, sizeof(UNSV_WORK));
}
#if 0
//----------------------------------------------------------
/**
 * @brief	�������镔���̍��R�[�h�Z�b�g
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	none
 */
//----------------------------------------------------------
void UNSV_SetCountryCode(const UNSV_WORK * inWork, const u8 inCountryCode)
{
  //@todo �J���g���[�R�[�h����`�F�b�N

  //�R�[�h�Z�b�g
  inWork->TargetCountryCode = inCountryCode;
}
#endif
//----------------------------------------------------------
/**
 * @brief	�Z�[�u���[�N�փf�[�^���R�s�[
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	none
 */
//----------------------------------------------------------
void SaveData_SaveUnsvWork(const UNSV_WORK * inWork, SAVE_CONTROL_WORK * sv)
{
  UNSV_WORK *work;
  work = SaveControl_DataPtrGet(sv, GMDATA_ID_UNSV);
	MI_CpuCopy32(inWork, work, sizeof(UNSV_WORK));
}

//----------------------------------------------------------
/**
 * @brief	���[�N�փ��[�h
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	none
 */
//----------------------------------------------------------
void SaveData_LoadUnsvWork(SAVE_CONTROL_WORK * sv, UNSV_WORK * outWork)
{
  UNSV_WORK *work;
  work = SaveControl_DataPtrGet(sv, GMDATA_ID_UNSV);
	MI_CpuCopy32(work, outWork, sizeof(UNSV_WORK));
}

