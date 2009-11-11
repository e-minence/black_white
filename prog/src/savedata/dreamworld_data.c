//============================================================================================
/**
 * @file	  dreamworld_data.c
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================

#include <gflib.h>

#include "poke_tool/poke_tool.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "savedata/dreamworld_data.h"
#include "poke_tool/poke_tool_def.h"


//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _DREAMWORLD_SAVEDATA {
  POKEMON_PASO_PARAM ppp;   ///< ����|�P����
  u8 recv_flag[DREAM_WORLD_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  u16 categoryType;    //�J�e�S��ID						2byte			���̌��ʃf�[�^�̃J�e�S���i�C�x���g�Ȃ̂��A�A�C�e���Ȃ̂��A�G���J�E���g�Ȃ̂��j
  DREAM_WORLD_TREAT_DATA treat;  //���J���f�[�^ 
	DREAM_WORLD_FURNITUREDATA furnitureID[DREAM_WORLD_DATA_MAX_FURNITURE];  //9	�z�M�Ƌ�
  u8 pokemoStatus;        //8	���M�����|�P����						1byte			���M�����|�P�����̏�Ԃ��󂯎��
  u8 padding;
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
 * @brief   DREAMWORLD_SAVEDATA�\���̂𓾂�
 * @return	DREAMWORLD_SAVEDATA�\����
 */
//--------------------------------------------------------------------------------------------

DREAMWORLD_SAVEDATA* DREAMWORLD_SV_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, DREAMWORLD_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���g������������
 * @param   DREAMWORLD_SAVEDATA�̃|�C���^
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_Init(DREAMWORLD_SAVEDATA* pSV)
{
	GFL_STD_MemClear(pSV, DREAMWORLD_SV_GetWorkSize());
}



//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�����𓾂�
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	POKEMON_PASO_PARAM
 */
//--------------------------------------------------------------------------------------------
POKEMON_PASO_PARAM* DREAMWORLD_SV_GetSleepPokemon(DREAMWORLD_SAVEDATA* pSV)
{
  return &pSV->ppp;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�������Z�b�g����
            �f�[�^�̌����͂����ł͂��ĂȂ��̂ŕK�������O�ɂ��鎖
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  POKEMON_PASO_PARAM*
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSleepPokemon(DREAMWORLD_SAVEDATA* pSV,POKEMON_PASO_PARAM* ppp)
{
  GFL_STD_MemCopy(ppp,&pSV->ppp,sizeof(POKETOOL_GetPPPWorkSize()));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ��������y�Y�̃J�e�S���[�𓾂�
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	DREAM_WORLD_RESULT_TYPE
 */
//--------------------------------------------------------------------------------------------
DREAM_WORLD_RESULT_TYPE DREAMWORLD_SV_GetCategoryType(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->categoryType;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ��������y�Y�̃J�e�S���[���Z�b�g
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	DREAM_WORLD_RESULT_TYPE
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetCategoryType(DREAMWORLD_SAVEDATA* pSV,DREAM_WORLD_RESULT_TYPE type)
{
  GF_ASSERT(type < DREAM_WORLD_RESULT_TYPE_MAX);
  if(!(type < DREAM_WORLD_RESULT_TYPE_MAX)){
    return;
  }
  pSV->categoryType = type;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ��������y�Y������
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @return	DREAM_WORLD_TREAT_DATA*
 */
//--------------------------------------------------------------------------------------------
DREAM_WORLD_TREAT_DATA* DREAMWORLD_SV_GetTreatData(DREAMWORLD_SAVEDATA* pSV)
{
  return &pSV->treat;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ��������y�Y���i�[
            �f�[�^�̌����͂����ł͂��ĂȂ��̂ŕK�������O�ɂ��鎖
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @return	DREAM_WORLD_TREAT_DATA*
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetTreatData(DREAMWORLD_SAVEDATA* pSV,DREAM_WORLD_TREAT_DATA* pTreat)
{
  GFL_STD_MemCopy(pTreat,&pSV->treat,sizeof(DREAM_WORLD_TREAT_DATA));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Ƌ�f�[�^������
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @return	DREAM_WORLD_FURNITUREDATA*
 */
//--------------------------------------------------------------------------------------------
DREAM_WORLD_FURNITUREDATA* DREAMWORLD_SV_GetFurnitureData(DREAMWORLD_SAVEDATA* pSV,int index)
{
  GF_ASSERT(index < DREAM_WORLD_DATA_MAX_FURNITURE);
  if(!(index < DREAM_WORLD_DATA_MAX_FURNITURE)){
    return NULL;
  }
  return &pSV->furnitureID[index];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Ƌ�f�[�^���i�[����
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @return	DREAM_WORLD_FURNITUREDATA*
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetFurnitureData(DREAMWORLD_SAVEDATA* pSV,int index,DREAM_WORLD_FURNITUREDATA* pF)
{
  GF_ASSERT(index < DREAM_WORLD_DATA_MAX_FURNITURE);
  if(!(index < DREAM_WORLD_DATA_MAX_FURNITURE)){
    return;
  }
  GFL_STD_MemCopy(pF,&pSV->furnitureID[index],sizeof(DREAM_WORLD_FURNITUREDATA));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�����̃X�e�[�^�X�𓾂�
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	DREAM_WORLD_RESULT_TYPE
 */
//--------------------------------------------------------------------------------------------
DREAM_WORLD_RESULT_TYPE DREAMWORLD_SV_GetPokemonStatus(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->pokemoStatus;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�����̃X�e�[�^�X���Z�b�g
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  status
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetPokemonStatus(DREAMWORLD_SAVEDATA* pSV,DREAM_WORLD_RESULT_TYPE status)
{
  GF_ASSERT(status < DREAM_WORLD_RESULT_TYPE_MAX);
  if(!(status < DREAM_WORLD_RESULT_TYPE_MAX)){
    return;
  }
  pSV->pokemoStatus = status;
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




