//==============================================================================
/**
 * @file    symbol_save_notwifi.c
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^�Fnotwifi�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2010.03.06(�y)
 *
 * WiFi�ł͐�΂Ɏg�p���Ȃ����A����ȊO�ł͂ǂ̃^�C�~���O�ł����Ă��g�p����\���̂������
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/symbol_save.h"
#include "savedata/symbol_save_notwifi.h"
#include "symbol_save_local.h"



//==================================================================
/**
 * �}�b�v���x��(64x64)���擾
 * @param   symbol_save		
 * @retval  SYMBOL_MAP_LEVEL_LARGE		
 */
//==================================================================
SYMBOL_MAP_LEVEL_LARGE SymbolSave_GetMapLevelLarge(SYMBOL_SAVE_WORK *symbol_save)
{
  return symbol_save->map_level_large;
}

//==================================================================
/**
 * �}�b�v���x��(32x32)���擾
 * @param   symbol_save		
 * @retval  SYMBOL_MAP_LEVEL_SMALL		
 */
//==================================================================
SYMBOL_MAP_LEVEL_SMALL SymbolSave_GetMapLevelSmall(SYMBOL_SAVE_WORK *symbol_save)
{
  return symbol_save->map_level_small;
}

//--------------------------------------------------------------
/**
 * �V���{���|�P�����̃f�[�^��z��P�ʂŎ擾����
 *
 * @param   symbol_save		
 * @param   dest_array		�����
 * @param   array_num		  dest_array�̔z��v�f��
 * @param   no		        �R�s�[�J�n����z�uNo
 * @param   copy_num		  �R�s�[��
 *
 * @retval  dest_array�ɃR�s�[���ꂽ�f�[�^�ɂ����L���f�[�^�����邩
 */
//--------------------------------------------------------------
static u8 SymbolSave_GetArraySymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, int no, int copy_num)
{
  int i;
  
  GF_ASSERT(array_num <= copy_num);
  GFL_STD_MemClear(dest_array, sizeof(struct _SYMBOL_POKEMON) * array_num);
  GFL_STD_MemCopy(&symbol_save->symbol_poke[no], dest_array, sizeof(struct _SYMBOL_POKEMON) * copy_num);
  for(i = 0; i < copy_num; i++){
    if(dest_array[i].monsno == 0){
      break;
    }
  }
  return i;
}

//==================================================================
/**
 * �L�[�v�]�[��(64x64)�̃V���{���|�P�����f�[�^���ꊇ�Ŏ擾
 *
 * @param   symbol_save		
 * @param   dest_array		�����
 * @param   array_num		  dest_array�̔z��v�f��
 * @param   occ_num       dest_array�ɃR�s�[���ꂽ�f�[�^�ɂ����L���f�[�^�����邩
 *
 * @param   �擪�̔z�uNo
 */
//==================================================================
u16 SymbolSave_GetKeepLargeSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, u8 *occ_num)
{
  *occ_num = SymbolSave_GetArraySymbolPokemon(symbol_save, dest_array, array_num, 
    SYMBOL_NO_START_KEEP_LARGE, SYMBOL_NO_END_KEEP_LARGE - SYMBOL_NO_START_KEEP_LARGE);
  return SYMBOL_NO_START_KEEP_LARGE;
}

//==================================================================
/**
 * �L�[�v�]�[��(32x32)�̃V���{���|�P�����f�[�^���ꊇ�Ŏ擾
 *
 * @param   symbol_save		
 * @param   dest_array		�����
 * @param   array_num		  dest_array�̔z��v�f��
 * @param   occ_num       dest_array�ɃR�s�[���ꂽ�f�[�^�ɂ����L���f�[�^�����邩
 *
 * @param   �擪�̔z�uNo
 */
//==================================================================
u16 SymbolSave_GetKeepSmallSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, u8 *occ_num)
{
  *occ_num = SymbolSave_GetArraySymbolPokemon(symbol_save, dest_array, array_num, 
    SYMBOL_NO_START_KEEP_SMALL, SYMBOL_NO_END_KEEP_SMALL - SYMBOL_NO_START_KEEP_SMALL);
  return SYMBOL_NO_START_KEEP_SMALL;
}

//==================================================================
/**
 * �t���[�]�[��(64x64)�̃V���{���|�P�����f�[�^���ꊇ�Ŏ擾
 *
 * @param   symbol_save		
 * @param   dest_array		�����
 * @param   array_num		  dest_array�̔z��v�f��
 * @param   map_no		    �}�b�v�ԍ�
 * @param   occ_num       dest_array�ɃR�s�[���ꂽ�f�[�^�ɂ����L���f�[�^�����邩
 *
 * @param   �擪�̔z�uNo
 */
//==================================================================
u16 SymbolSave_GetFreeLargeSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, int map_no, u8 *occ_num)
{
  u16 no = SYMBOL_NO_START_FREE_LARGE + SYMBOL_MAP_STOCK_LARGE * map_no;
  GF_ASSERT(no < SYMBOL_NO_END_FREE_LARGE);
  *occ_num = SymbolSave_GetArraySymbolPokemon(
    symbol_save, dest_array, array_num, no, SYMBOL_MAP_STOCK_LARGE);
  return no;
}

//==================================================================
/**
 * �t���[�]�[��(32x32)�̃V���{���|�P�����f�[�^���ꊇ�Ŏ擾
 *
 * @param   symbol_save		
 * @param   dest_array		�����
 * @param   array_num		  dest_array�̔z��v�f��
 * @param   map_no		    �}�b�v�ԍ�
 * @param   occ_num       dest_array�ɃR�s�[���ꂽ�f�[�^�ɂ����L���f�[�^�����邩
 *
 * @param   �擪�̔z�uNo
 */
//==================================================================
u16 SymbolSave_GetFreeSmallSymbolPokemon(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_POKEMON *dest_array, int array_num, int map_no, u8 *occ_num)
{
  u16 no = SYMBOL_NO_START_FREE_SMALL + SYMBOL_MAP_STOCK_SMALL * map_no;
  GF_ASSERT(no < SYMBOL_NO_END_FREE_SMALL);
  *occ_num = SymbolSave_GetArraySymbolPokemon(
    symbol_save, dest_array, array_num, no, SYMBOL_MAP_STOCK_SMALL);
  return no;
}
