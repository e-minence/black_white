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
///�X�^�b�N�|�P������
#define SYMBOL_STACK_MAX          (10)
///�V���{���|�P�����ő吔
#define SYMBOL_POKE_MAX           (520 + SYMBOL_STACK_MAX)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�V���{���G���J�E���g1�C���̃Z�[�u
struct _SYMBOL_POKEMON{
  u32 monsno:11;        ///<�|�P�����ԍ�
  u32 wazano:10;        ///<�Z�ԍ�
  u32 sex:2;            ///<����(PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN)
  u32 form_no:6;        ///<�t�H�����ԍ�
  u32        :3;        //�]��
};

///�V���{���G���J�E���g�Z�[�u�f�[�^
struct _SYMBOL_SAVE_WORK{
  struct _SYMBOL_POKEMON symbol_poke[SYMBOL_POKE_MAX]; ///<�V���{���|�P����
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
  OS_TPrintf("SYMBOL_SAVE SIZE = %d\n", sizeof(SYMBOL_SAVE_WORK));
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

//==================================================================
/**
 * �V���{���|�P�����o�^
 *
 * @param   symbol_save		�V���{���Z�[�u�̈�ւ̃|�C���^
 * @param   monsno        �|�P�����ԍ�
 * @param   wazano		    �Z�ԍ�
 * @param   sex		        ����(PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN)
 * @param   form_no		    �t�H�����ԍ�
 */
//==================================================================
void SymbolSave_Set(SYMBOL_SAVE_WORK *symbol_save, u16 monsno, u16 wazano, u8 sex, u8 form_no)
{
  int i;
  SYMBOL_POKEMON *spoke = symbol_save->symbol_poke;
  
  for(i = 0; i < SYMBOL_POKE_MAX; i++){
    if(spoke->monsno == 0){
      spoke->monsno = monsno;
      spoke->wazano = wazano;
      spoke->sex = sex;
      spoke->form_no = form_no;
      return;
    }
    spoke++;
  }
}

