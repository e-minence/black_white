//============================================================================================
/**
 * @file	  dreamworld_data.c
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================

#include <gflib.h>

#include "savedata/save_tbl.h"
#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "savedata/dreamworld_data.h"


//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _DREAMWORLD_SAVEDATA {
  u8 recv_flag[DREAM_WORLD_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  u16 categoryType;    //�J�e�S��ID						2byte			���̌��ʃf�[�^�̃J�e�S���i�C�x���g�Ȃ̂��A�A�C�e���Ȃ̂��A�G���J�E���g�Ȃ̂��j
  DREAM_WORLD_TREAT_DATA treat;  //���J���f�[�^ 
  u8 pokemoStatus;        //8	���M�����|�P����						1byte			���M�����|�P�����̏�Ԃ��󂯎��
	DREAM_WORLD_FURNITUREDATA furnitureID[DREAM_WORLD_DATA_MAX_FURNITURE];  //9	�z�M�Ƌ�
};




//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̃T�C�Y�𓾂�
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------
int DREAMWORLD_SV_GetWorkSize(void)
{
	return sizeof(DREAMWORLD_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̂𓾂�
 * @return	�\����
 */
//--------------------------------------------------------------------------------------------

DREAMWORLD_SAVEDATA* DREAMWORLD_SV_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, DREAMWORLD_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���g������������
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_Init(DREAMWORLD_SAVEDATA* pSV)
{
	GFL_STD_MemClear(pSV, DREAMWORLD_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���󂯎�������ǂ���
 * @param	  pSV		�Z�[�u�f�[�^�ւ̃|�C���^
 * @param   num     �ʂ��ԍ�
 * @return	�󂯎������TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_IsEventRecvFlag(DREAMWORLD_SAVEDATA* pSV,int num)
{
  GF_ASSERT(num < DREAM_WORLD_DATA_MAX_EVENT);
  if(num > DREAM_WORLD_DATA_MAX_EVENT){
    return TRUE;
  }
  return (pSV->recv_flag[num / 8] & (1 << (num & 7)));
}

//------------------------------------------------------------------
/**
 * @brief	�w��̃C�x���g���������t���O�𗧂Ă�
 * @param	pSV		�Z�[�u�f�[�^�ւ̃|�C���^
 * @param	num		�C�x���g�ԍ�
 * @return	NONE
 */
//------------------------------------------------------------------
void DREAMWORLD_SV_SetEventRecvFlag(DREAMWORLD_SAVEDATA* pSV, int num)
{
  GF_ASSERT(num < DREAM_WORLD_DATA_MAX_EVENT);
  if(num > DREAM_WORLD_DATA_MAX_EVENT){
    return;
  }
  pSV->recv_flag[num / 8] |= (1 << (num & 7));
}


//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
DREAMWORLD_SAVEDATA* DREAMWORLD_SV_GetDreamWorldSaveData(SAVE_CONTROL_WORK* pSave)
{
	DREAMWORLD_SAVEDATA* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_DREAMWORLD);
	return pData;

}




