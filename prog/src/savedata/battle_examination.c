//======================================================================
/**
 * @file     battle_examination.c
 * @brief    �o�g������Z�[�u�f�[�^
 * @authaor  k.ohno
 * @date     10.01.08
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "savedata/save_tbl.h"
#include "poke_tool/poke_tool.h"

#include "savedata/save_control.h"
#include "savedata/battle_examination.h"
#include "battle/bsubway_battle_data.h"


//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̃T�C�Y�𓾂�
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------
int BATTLE_EXAMINATION_SAVE_GetWorkSize(void)
{
	return sizeof(BATTLE_EXAMINATION_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̂𓾂�
 * @return	�\����
 */
//--------------------------------------------------------------------------------------------

BATTLE_EXAMINATION_SAVEDATA* BATTLE_EXAMINATION_SAVE_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, BATTLE_EXAMINATION_SAVE_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���g������������
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------

void BATTLE_EXAMINATION_SAVE_Init(BATTLE_EXAMINATION_SAVEDATA* pSV)
{
  GFL_STD_MemClear(pSV,sizeof(BATTLE_EXAMINATION_SAVEDATA));
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�[�^�̃|�C���^�擾
 *
 *	@param	SAVE_CONTROL_WORK * p_sv	�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 *	@return	BATTLE_EXAMINATION_SAVEDATA
 */
//-----------------------------------------------------------------------------
BATTLE_EXAMINATION_SAVEDATA *BATTLE_EXAMINATION_SAVE_GetSvPtr( SAVE_CONTROL_WORK * p_sv)
{	
	BATTLE_EXAMINATION_SAVEDATA	*data;
  data = SaveControl_Extra_DataPtrGet(p_sv, SAVE_EXTRA_ID_BATTLE_EXAMINATION, 0);
//  data = SaveControl_DataPtrGet(p_sv, GMDATA_ID_EXAMINATION);
	return data;
}

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------

//  if(LOAD_RESULT_OK== SaveControl_Extra_LoadWork(pSave
//,            SAVE_EXTRA_ID_BATTLE_EXAMINATION, HEAPID_FIELDMAP,
  //                      pCGearWork,SAVESIZE_EXTRA_BATTLE_EXAMINATION)){

//--------------------------------------------------------------------------------------------
/**
 * @brief   ����p�f�[�^���擾����
 * @param   pSV   �Z�[�u�f�[�^�|�C���^
 * @param   �擾�C���f�b�N�X�@0�`BATTLE_EXAMINATION_MAX-1
 * @return	�f�[�^�擪�A�h���X
 */
//--------------------------------------------------------------------------------------------
BSUBWAY_PARTNER_DATA *BATTLE_EXAMINATION_SAVE_GetData(BATTLE_EXAMINATION_SAVEDATA* pSV, const u32 inIdx)
{
  BSUBWAY_PARTNER_DATA *data;
  if ( inIdx >= BATTLE_EXAMINATION_MAX ){
    GF_ASSERT_MSG(0, " idx error $d",inIdx);
    data = &(pSV->trainer[0]);
  }
  else data = &(pSV->trainer[inIdx]);

  return data;
}

