//============================================================================================
/**
 * @file	  dreamworld_data.c
 * @brief	  �|�P�����h���[�����[���h�ɕK�v�ȃf�[�^
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================

#include <gflib.h>

#include "dreamworld_data_local.h"



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
POKEMON_PARAM* DREAMWORLD_SV_GetSleepPokemon(DREAMWORLD_SAVEDATA* pSV)
{
  return &pSV->pp;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�������Z�b�g����
            �f�[�^�̌����͂����ł͂��ĂȂ��̂ŕK�������O�ɂ��鎖
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  POKEMON_PASO_PARAM*
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSleepPokemon(DREAMWORLD_SAVEDATA* pSV,POKEMON_PARAM* pp)
{
  GFL_STD_MemCopy(pp,&pSV->pp,POKETOOL_GetWorkSize());
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   PDW�o�^���݃t���O������
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  PDW�o�^����
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSignin(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg)
{
  pSV->signin = bFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   PDW�o�^���݂��ǂ���
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	PDW�o�^���ݎ���TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetSignin(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->signin;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �A�J�E���g�o�^�����ł��N���������ǂ���
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  �A�J�E���g�o�^���Ă�����TRUE
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetAccount(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg)
{
  pSV->bAccount = bFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �A�J�E���g�o�^�����ł��N���������ǂ���
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	�A�J�E���g�o�^����TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetAccount(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->bAccount;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�������Z�b�g������t���O��ON
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  �|�P�����Z�b�g����TRUE
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_SetSleepPokemonFlg(DREAMWORLD_SAVEDATA* pSV,BOOL bFlg)
{
  pSV->pokemonIn = bFlg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Q�Ă�|�P�������Z�b�g�������ǂ����m�F
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @return	�|�P�����Z�b�g����TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL DREAMWORLD_SV_GetSleepPokemonFlg(DREAMWORLD_SAVEDATA* pSV)
{
  return pSV->pokemonIn;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ��������y�Y�̃A�C�e��No�𓾂�
 * @param	  pSV		DREAMWORLD_SAVEDATA
 * @param	  index �C���f�b�N�X
 * @return	�A�C�e��No
 */
//--------------------------------------------------------------------------------------------

u16 DREAMWORLD_SV_GetItem(DREAMWORLD_SAVEDATA* pSV,int index)
{
  if(index > DREAM_WORLD_DATA_MAX_ITEMBOX){
    return 0;
  }
  return pSV->itemID[index];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �A�C�e���̐����擾
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @param	  index		�ꏊ
 * @return	  num		 ��
 */
//--------------------------------------------------------------------------------------------

u8 DREAMWORLD_SV_GetItemNum(DREAMWORLD_SAVEDATA* pSV, int index)
{
  if(index > DREAM_WORLD_DATA_MAX_ITEMBOX){
    return 0;
  }
  return pSV->itemNum[index];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ������A�C�e���������
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @param	  index		�ꏊ
 * @param	  itemNo		�A�C�e���ԍ�
 * @param	  num		 ��
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_SetItem(DREAMWORLD_SAVEDATA* pSV,int index ,int itemNo, int num)
{
  if(index > DREAM_WORLD_DATA_MAX_ITEMBOX){
    return;
  }
  pSV->itemID[index] =itemNo;
  pSV->itemNum[index] =num;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ������A�C�e������
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 * @param	  index		�ꏊ
 */
//--------------------------------------------------------------------------------------------
void DREAMWORLD_SV_DeleteItem(DREAMWORLD_SAVEDATA* pSV,int index)
{
  DREAMWORLD_SV_SetItem(pSV,index,0,0);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ������A�C�e���S������
 * @param	  pSV		DREAMWORLD_SAVEDATA�|�C���^
 */
//--------------------------------------------------------------------------------------------

void DREAMWORLD_SV_ClearAllItem(DREAMWORLD_SAVEDATA* pSV)
{
  int i;

  for(i=0;i<DREAM_WORLD_DATA_MAX_ITEMBOX;i++){
    DREAMWORLD_SV_DeleteItem(pSV,i);
  }
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
DREAM_WORLD_SLEEP_TYPE DREAMWORLD_SV_GetPokemonStatus(DREAMWORLD_SAVEDATA* pSV)
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
void DREAMWORLD_SV_SetPokemonStatus(DREAMWORLD_SAVEDATA* pSV,DREAM_WORLD_SLEEP_TYPE status)
{
  GF_ASSERT(status < DREAM_WORLD_SLEEP_TYPE_MAX);
  if(!(status < DREAM_WORLD_SLEEP_TYPE_MAX)){
    return;
  }
  pSV->pokemoStatus = status;
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




