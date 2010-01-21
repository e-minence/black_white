//==============================================================================
/**
 * @file    symbol_save.c
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.01.06(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/symbol_save.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V���{���|�P�����ő吔
#define SYMBOL_POKE_MAX           (200)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�V���{���G���J�E���g�Z�[�u�f�[�^
struct _SYMBOL_SAVE_WORK{
  u16 monsno[SYMBOL_POKE_MAX];    ///<�V���{���|�P����
};




//------------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�T�C�Y��Ԃ�
 *
 * @retval  int		
 */
//------------------------------------------------------------------
u32 SymbolSave_GetWorkSize( void )
{
	return sizeof(SYMBOL_SAVE_WORK);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void SymbolSave_WorkInit(void *work)
{
  SYMBOL_SAVE_WORK *symbol_save = work;
  
  GFL_STD_MemClear(symbol_save, sizeof(SYMBOL_SAVE_WORK));
}


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �|�P�����o�^
 *
 * @param   symbol_save		�V���{���Z�[�u�̈�ւ̃|�C���^
 * @param   monsno        �|�P�����ԍ�
 */
//==================================================================
void SymbolSave_Set(SYMBOL_SAVE_WORK *symbol_save, u16 monsno)
{
  int i;
  
  for(i = 0; i < SYMBOL_POKE_MAX; i++){
    if(symbol_save->monsno[i] == 0){
      symbol_save->monsno[i] = monsno;
      return;
    }
  }
}

//==================================================================
/**
 * @brief   SYMBOL_SAVE_WORK�f�[�^�擾
 * @param   pSave		�Z�[�u�f�[�^�|�C���^
 * @return  SYMBOL_SAVE_WORK�f�[�^
 */
//==================================================================

SYMBOL_SAVE_WORK* SymbolSave_GetSymbolData(SAVE_CONTROL_WORK* pSave)
{
	SYMBOL_SAVE_WORK* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_SYMBOL);
	return pData;
}
