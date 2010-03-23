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
#include "symbol_save_local.h"


SDK_COMPILER_ASSERT(SYMBOL_POKE_MAX == SYMBOL_NO_END_FREE_SMALL);


//==============================================================================
//  �萔��`
//==============================================================================
///�}�b�v(32x32)��1���x���ŏ��L����X�g�b�N��
#define SMALL_MAP_EXPAND_NUM      (SYMBOL_MAP_STOCK_SMALL * 3)
///�}�b�v(64x64)��1���x���ŏ��L����X�g�b�N��
#define LARGE_MAP_EXPAND_NUM      (SYMBOL_MAP_STOCK_LARGE * 3)


//==============================================================================
//  �f�[�^
//==============================================================================
///SYMBOL_ZONE_TYPE���̔z�uNo�̊J�n�ʒu�ƏI�[�ʒu
const SYMBOL_ZONE_TYPE_DATA_NO SymbolZoneTypeDataNo[] = {
  {SYMBOL_NO_START_KEEP_LARGE, SYMBOL_NO_END_KEEP_LARGE},   //SYMBOL_ZONE_TYPE_KEEP_LARGE
  {SYMBOL_NO_START_KEEP_SMALL, SYMBOL_NO_END_KEEP_SMALL},   //SYMBOL_ZONE_TYPE_KEEP_SMALL
  {SYMBOL_NO_START_FREE_LARGE, SYMBOL_NO_END_FREE_LARGE},   //SYMBOL_ZONE_TYPE_FREE_LARGE
  {SYMBOL_NO_START_FREE_SMALL, SYMBOL_NO_END_FREE_SMALL},   //SYMBOL_ZONE_TYPE_FREE_SMALL
};



//==============================================================================
//
//  
//
//==============================================================================
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
 * @brief �V���{���|�P�����f�[�^���擾
 * @param   symbol_save
 * @param   no		�z�uNo
 * @return  SYMBOL_POKEMON  �V���{���|�P�����f�[�^�ւ̃|�C���^
 */
//==================================================================
const SYMBOL_POKEMON * SymbolSave_GetSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, u32 no)
{
  GF_ASSERT( no < SYMBOL_POKE_MAX );
  return &symbol_save->symbol_poke[ no ];
}

//==================================================================
/**
 * �t���[�]�[���ɋ󂫂����邩���ׂ�
 *
 * @param   symbol_save		
 * @param   zone_type		  SYMBOL_ZONE_TYPE
 *
 * @retval  u32		        �󂫂��������ꍇ�F�z�uNo
 * @retval  u32           �󂫂������ꍇ�FSYMBOL_SPACE_NONE
 */
//==================================================================
u32 SymbolSave_CheckFreeZoneSpace(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_ZONE_TYPE zone_type)
{
  u32 i, start,end;
  SYMBOL_POKEMON *spoke;
  
  start = SymbolZoneTypeDataNo[zone_type].start;
  end = SymbolZoneTypeDataNo[zone_type].end;
  
  spoke = &symbol_save->symbol_poke[start];
  for(i = start; i < end; i++){
    if(spoke->monsno == 0){
      return i;
    }
    spoke++;
  }
  return SYMBOL_SPACE_NONE;
}

//--------------------------------------------------------------
/**
 * �z�uNo����SYMBOL_ZONE_TYPE���擾���܂�
 *
 * @param   no		�z�uNo
 *
 * @retval  SYMBOL_ZONE_TYPE		
 */
//--------------------------------------------------------------
SYMBOL_ZONE_TYPE SYMBOLZONE_GetZoneTypeFromNumber(u32 no)
{
  SYMBOL_ZONE_TYPE zone_type;
  
  for(zone_type = 0; zone_type < NELEMS(SymbolZoneTypeDataNo); zone_type++){
    if(no < SymbolZoneTypeDataNo[zone_type].end){
      return zone_type;
    }
  }
  GF_ASSERT_MSG(0, "zone_type = %d", zone_type);
  return SYMBOL_ZONE_TYPE_FREE_SMALL;
}

//==================================================================
/**
 * �J�n�z�uNo���w�肵�ăf�[�^�̑O�l�߂��s���܂�
 *
 * @param   symbol_save		�V���{���Z�[�u�̈�ւ̃|�C���^
 * @param   no		        �z�uNo
 * @param   zone_type		  SYMBOL_ZONE_TYPE
 *
 * �ߊl���ăf�[�^���폜���鎞�ɂ����̊֐����g�p���܂�
 */
//==================================================================
void SymbolSave_DataShift(SYMBOL_SAVE_WORK *symbol_save, u32 no)
{
  SYMBOL_ZONE_TYPE zone_type;
  u32 start, end;
  
  zone_type = SYMBOLZONE_GetZoneTypeFromNumber(no);
  start = no;
  end = SymbolZoneTypeDataNo[zone_type].end;

  GFL_STD_MemCopy(&symbol_save->symbol_poke[start + 1], &symbol_save->symbol_poke[start], 
    sizeof(struct _SYMBOL_POKEMON) * (end - (start + 1)));
  symbol_save->symbol_poke[end - 1].monsno = 0;
}

//==================================================================
/**
 * �V���{���|�P�������t���[�]�[���ɓo�^
 *
 * @param   symbol_save		�V���{���Z�[�u�̈�ւ̃|�C���^
 * @param   monsno        �|�P�����ԍ�
 * @param   wazano		    �Z�ԍ�
 * @param   sex		        ����(PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN)
 * @param   form_no		    �t�H�����ԍ�
 * @param   zone_type     SYMBOL_ZONE_TYPE
 *
 * �󂫂������ꍇ�͐擪�̃f�[�^���Ƃ���Ă񎮂ɒǂ��o���čŌ���ɃZ�b�g���܂�
 */
//==================================================================
void SymbolSave_SetFreeZone(SYMBOL_SAVE_WORK *symbol_save, u16 monsno, u16 wazano, u8 sex, u8 form_no, SYMBOL_ZONE_TYPE zone_type)
{
  u32 no, start, end;
  SYMBOL_POKEMON *spoke;

  start = SymbolZoneTypeDataNo[zone_type].start;
  end = SymbolZoneTypeDataNo[zone_type].end;
  
  no = SymbolSave_CheckFreeZoneSpace(symbol_save, zone_type);
  if(no == SYMBOL_SPACE_NONE){
    SymbolSave_DataShift(symbol_save, start);
    spoke = &symbol_save->symbol_poke[end - 1];
  }
  else{
    spoke = &symbol_save->symbol_poke[no];
  }
  spoke->monsno = monsno;
  spoke->wazano = wazano;
  spoke->sex = sex;
  spoke->form_no = form_no;

  //�}�b�v�̊g���`�F�b�N
  if(zone_type == SYMBOL_ZONE_TYPE_FREE_LARGE){
    if(symbol_save->map_level_large < SYMBOL_MAP_LEVEL_LARGE_MAX){
      symbol_save->map_level_large++;
    }
  }
  else if(no != SYMBOL_SPACE_NONE){
    symbol_save->map_level_small = (no - SYMBOL_NO_START_FREE_SMALL) / SMALL_MAP_EXPAND_NUM;
    if(symbol_save->map_level_small > SYMBOL_MAP_LEVEL_SMALL_MAX){
      symbol_save->map_level_small = SYMBOL_MAP_LEVEL_SMALL_MAX;  //�ꉞ
      GF_ASSERT_MSG(0, "no=%d", no - SYMBOL_NO_START_FREE_SMALL);
    }
  }
}
