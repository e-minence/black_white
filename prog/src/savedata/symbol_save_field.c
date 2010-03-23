//==============================================================================
/**
 * @file    symbol_save_field.c
 * @brief   �V���{���G���J�E���g�p�Z�[�u�f�[�^�Ffieldmap�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2010.03.03(��)
 *
 * �t�B�[���h�ł����g�p���Ȃ��V���{���G���J�E���g�Z�[�u�A�N�Z�X�Q
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/symbol_save.h"
#include "savedata/symbol_save_notwifi.h"
#include "savedata/symbol_save_field.h"
#include "symbol_save_local.h"



//==================================================================
/**
 * �L�[�v�]�[���ɋ󂫂����邩���ׂ�
 *
 * @param   symbol_save		
 * @param   zone_type		  SYMBOL_ZONE_TYPE
 *
 * @retval  u32		        �󂫂��������ꍇ�F�z�uNo
 * @retval  u32           �󂫂������ꍇ�FSYMBOL_SPACE_NONE
 */
//==================================================================
u32 SymbolSave_Field_CheckKeepZoneSpace(SYMBOL_SAVE_WORK *symbol_save, SYMBOL_ZONE_TYPE zone_type)
{
  u32 i, start,end;
  
  start = SymbolZoneTypeDataNo[zone_type].start;
  end = SymbolZoneTypeDataNo[zone_type].end;
  
  for(i = start; i < end; i++){
    if(symbol_save->symbol_poke[i].monsno == 0){
      return i;
    }
  }
  return SYMBOL_SPACE_NONE;
}

//==================================================================
/**
 * �t���[�]�[���ɂ���|�P�������L�[�v�]�[���Ɉړ�
 *
 * @param   symbol_save		
 * @param   now_no		    �ړ��Ώۂ̃|�P�����̔z�uNo
 */
//==================================================================
void SymbolSave_Field_Move_FreeToKeep(SYMBOL_SAVE_WORK *symbol_save, u32 now_no)
{
  u32 no, end;
  SYMBOL_ZONE_TYPE zone_type;
  
  if(now_no >= SYMBOL_NO_START_FREE_SMALL){
    zone_type = SYMBOL_ZONE_TYPE_KEEP_SMALL;
  }
  else if(now_no >= SYMBOL_NO_START_FREE_LARGE){
    zone_type = SYMBOL_ZONE_TYPE_KEEP_LARGE;
  }
  else{
    GF_ASSERT_MSG(0, "now_no = %d", now_no);
    return;
  }
  end = SymbolZoneTypeDataNo[zone_type].end;
  
  no = SymbolSave_Field_CheckKeepZoneSpace(symbol_save, zone_type);
  if(no == SYMBOL_SPACE_NONE){ //�ꉞ
    GF_ASSERT(no != SYMBOL_SPACE_NONE);
    return;
  }
  
  symbol_save->symbol_poke[no] = symbol_save->symbol_poke[now_no];
  
  //�t���[�]�[����O�l��
  if(now_no + 1 < end){
    SymbolSave_DataShift(symbol_save, now_no);
  }
  symbol_save->symbol_poke[end - 1].monsno = 0;
}

//==================================================================
/**
 * �L�[�v�]�[���ɂ���|�P�������t���[�]�[���Ɉړ�
 *
 * @param   symbol_save		
 * @param   keep_no		      �ړ��Ώۂ̃|�P�����̔z�uNo
 */
//==================================================================
void SymbolSave_Field_Move_KeepToFree(SYMBOL_SAVE_WORK *symbol_save, u32 keep_no)
{
  u32 no, end;
  SYMBOL_ZONE_TYPE zone_type;
  SYMBOL_POKEMON *spoke = &symbol_save->symbol_poke[keep_no];
  
  if(keep_no >= SYMBOL_NO_START_FREE_LARGE){
    GF_ASSERT_MSG(0, "keep_no = %d", keep_no);
    return;
  }
  else if(keep_no >= SYMBOL_NO_START_KEEP_SMALL){
    zone_type = SYMBOL_ZONE_TYPE_FREE_SMALL;
  }
  else{
    zone_type = SYMBOL_ZONE_TYPE_FREE_LARGE;
  }
  end = SymbolZoneTypeDataNo[zone_type].end;
  
  //�L�[�v�]�[������t���[�]�[���ֈړ����鎞�͕K���t���[�̋󂫂����鎖�O��
  //�����Ŏ����̃f�[�^��ׂ��Ȃ��悤�ɂ����
  GF_ASSERT(SymbolSave_CheckFreeZoneSpace(symbol_save, zone_type) != SYMBOL_SPACE_NONE);
  
  SymbolSave_SetFreeZone(symbol_save, 
    spoke->monsno, spoke->wazano, spoke->sex, spoke->form_no, zone_type);

  //�L�[�v�]�[����O�l��
  if(keep_no + 1 < end){
    SymbolSave_DataShift(symbol_save, keep_no);
  }
  symbol_save->symbol_poke[end - 1].monsno = 0;
}

//==================================================================
/**
 * �V���{���|�P�����̈ړ�
 *
 * @param   symbol_save
 * @param   no          �ړ��Ώۃ|�P�����̔z�uNo.
 *
 * �z�uNo.���猻�݂̃]�[���^�C�v���������ʂ��A�K�؂Ȉړ����s��
 */
//==================================================================
BOOL SymbolSave_Field_MoveAuto( SYMBOL_SAVE_WORK *symbol_save, u32 no )
{
  BOOL result = FALSE;
  switch ( SYMBOLZONE_GetZoneTypeFromNumber( no ) )
  {
  case SYMBOL_ZONE_TYPE_KEEP_LARGE:
    //�L�[�v�]�[���i��j���t���[�]�[���i��j
    if(SymbolSave_CheckFreeZoneSpace(symbol_save, SYMBOL_ZONE_TYPE_FREE_LARGE) != SYMBOL_SPACE_NONE) {
      SymbolSave_Field_Move_KeepToFree( symbol_save, no );
      result = TRUE;
    }
    break;
  case SYMBOL_ZONE_TYPE_KEEP_SMALL:
    //�L�[�v�]�[���i���j���t���[�]�[���i���j
    if(SymbolSave_CheckFreeZoneSpace(symbol_save, SYMBOL_ZONE_TYPE_FREE_SMALL) != SYMBOL_SPACE_NONE) {
      SymbolSave_Field_Move_KeepToFree( symbol_save, no );
      result = TRUE;
    }
    break;
  case SYMBOL_ZONE_TYPE_FREE_LARGE:
    //�t���[�]�[���i��j���L�[�v�]�[���i��j
    if(SymbolSave_Field_CheckKeepZoneSpace(symbol_save, SYMBOL_ZONE_TYPE_KEEP_LARGE)
        != SYMBOL_SPACE_NONE) {
      SymbolSave_Field_Move_FreeToKeep( symbol_save, no );
      result = TRUE;
    }
    break;
  case SYMBOL_ZONE_TYPE_FREE_SMALL:
    //�t���[�]�[���i���j���L�[�v�]�[���i���j
    if(SymbolSave_Field_CheckKeepZoneSpace(symbol_save, SYMBOL_ZONE_TYPE_KEEP_SMALL)
        != SYMBOL_SPACE_NONE) {
      SymbolSave_Field_Move_FreeToKeep( symbol_save, no );
      result = TRUE;
    }
    break;
  default: GF_ASSERT( 0 );
  }
  return result;
}

