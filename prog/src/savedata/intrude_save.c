//==============================================================================
/**
 * @file    intrude_save.c
 * @brief   �N���̃Z�[�u�f�[�^
 * @author  matsuda
 * @date    2009.10.18(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/intrude_save.h"
#include "intrude_save_local.h"




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
u32 IntrudeSave_GetWorkSize( void )
{
	return sizeof(INTRUDE_SAVE_WORK);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void IntrudeSave_WorkInit(void *work)
{
  INTRUDE_SAVE_WORK *intsave = work;
  
  GFL_STD_MemClear(intsave, sizeof(INTRUDE_SAVE_WORK));
  OccupyInfo_WorkInit(&intsave->occupy);
  intsave->gpower_id = GPOWER_ID_NULL;
}


//==============================================================================
//  �苒���
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void OccupyInfo_WorkInit(OCCUPY_INFO *occupy)
{
  int i;
  
  GFL_STD_MemClear(occupy, sizeof(OCCUPY_INFO));
  for(i = 0; i < INTRUDE_TOWN_MAX; i++){
    occupy->mlst.mission_clear[i] = MISSION_CLEAR_NONE;
  }
}

//==================================================================
/**
 * �苒���̃z���C�g���x�����擾
 *
 * @param   occupy		
 *
 * @retval  u16		
 */
//==================================================================
u16 OccupyInfo_GetWhiteLevel(const OCCUPY_INFO *occupy)
{
  return occupy->white_level;
}

//==================================================================
/**
 * �苒���̃u���b�N���x�����擾
 *
 * @param   occupy		
 *
 * @retval  u16		
 */
//==================================================================
u16 OccupyInfo_GetBlackLevel(const OCCUPY_INFO *occupy)
{
  return occupy->black_level;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �N���Z�[�u�f�[�^�̃|�C���^�擾
 *
 * @param   p_sv		
 *
 * @retval  INTRUDE_SAVE_WORK *		
 */
//==================================================================
INTRUDE_SAVE_WORK * SaveData_GetIntrude( SAVE_CONTROL_WORK * p_sv)
{
	return SaveControl_DataPtrGet(p_sv, GMDATA_ID_INTRUDE);
}


//==============================================================================
//	
//==============================================================================
//==================================================================
/**
 * OCCUPY_INFO�\���̂̃f�[�^���Z�[�u�f�[�^�ɔ��f������
 *
 * @param   sv		
 * @param   occupy		�������苒���
 *
 * �t�B�[���h�̃Z�[�u�O�Ȃǂɂ�����Ăяo���K�v������
 */
//==================================================================
void SaveData_OccupyInfoUpdate(SAVE_CONTROL_WORK * sv, const OCCUPY_INFO *occupy)
{
	INTRUDE_SAVE_WORK *intsave = SaveData_GetIntrude(sv);

  intsave->occupy = *occupy;
}

//==================================================================
/**
 * �Z�[�u�f�[�^����OCCUPY_INFO�\���̂̏������[�h����
 *
 * @param   sv		
 * @param   dest_occupy		�苒�������
 */
//==================================================================
void SaveData_OccupyInfoLoad(SAVE_CONTROL_WORK * sv, OCCUPY_INFO *dest_occupy)
{
	INTRUDE_SAVE_WORK *intsave = SaveData_GetIntrude(sv);

  *dest_occupy = intsave->occupy;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �B���A�C�e�����Z�[�u�f�[�^�֓o�^���܂�
 *
 * @param   intsave		
 * @param   src		    �B���A�C�e���f�[�^�ւ̃|�C���^
 * 
 * �f�[�^�������ς��̎��͌Â��f�[�^�������o���ăZ�b�g���܂�
 */
//==================================================================
void ISC_SAVE_SetItem(INTRUDE_SAVE_WORK *intsave, const INTRUDE_SECRET_ITEM_SAVE *src)
{
  int i;
  INTRUDE_SECRET_ITEM_SAVE *isis = intsave->secret_item;
  
  for(i = 0; i < INTRUDE_SECRET_ITEM_SAVE_MAX; i++){
    if(isis->item == 0){
      OS_TPrintf("save secretitem No.%d\n", i);
      *isis = *src;
      return;
    }
    isis++;
  }
  
  //�f�[�^�������ς��Ȃ̂ŁA�Â��f�[�^��O�l�߂��čŌ���ɂ���
  for(i = 1; i < INTRUDE_SECRET_ITEM_SAVE_MAX; i++){
    intsave->secret_item[i - 1] = intsave->secret_item[i];
  }
  intsave->secret_item[INTRUDE_SECRET_ITEM_SAVE_MAX - 1] = *src;
}

//==================================================================
/**
 * ���g����������G�p���[ID���Z�b�g
 *
 * @param   intsave		  
 * @param   gpower_id		G�p���[ID
 */
//==================================================================
void ISC_SAVE_SetGPowerID(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id)
{
  intsave->gpower_id = gpower_id;
}

//==================================================================
/**
 * ���g���������Ă���G�p���[ID���擾
 *
 * @param   intsave		
 *
 * @retval  GPOWER_ID		G�p���[ID (�����������Ă��Ȃ��ꍇ��GPOWER_ID_NULL)
 */
//==================================================================
GPOWER_ID ISC_SAVE_GetGPowerID(INTRUDE_SAVE_WORK *intsave)
{
  return intsave->gpower_id;
}

//==============================================================================
//  �z�z��pG�p���[��Mbit
//==============================================================================
//==================================================================
/**
 * �z�z��pG�p���[��bit��ON�ɂ���
 *
 * @param   intsave		  
 * @param   gpower_id		G�p���[
 */
//==================================================================
void ISC_SAVE_SetDistributionGPower(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id)
{
  int dis_no, flag_no, bit_no;
  
  dis_no = gpower_id - (GPOWER_ID_MAX - 10);
  if(dis_no < 0){
    GF_ASSERT(0);
    return;
  }
  
  flag_no = dis_no / 8;
  bit_no = dis_no % 8;
  intsave->gpower_distribution_bit[flag_no] |= 1 << bit_no;
}

//==================================================================
/**
 * �z�z��pG�p���[��bit���Z�b�g����Ă��邩�`�F�b�N
 *
 * @param   intsave		
 * @param   gpower_id		G�p���[ID
 *
 * @retval  BOOL		TRUE:ON����Ă���@FALSE:OFF
 */
//==================================================================
BOOL ISC_SAVE_GetDistributionGPower(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id)
{
  int dis_no, flag_no, bit_no;
  
  dis_no = gpower_id - (GPOWER_ID_MAX - 10);
  if(dis_no < 0){
    GF_ASSERT(0);
    return FALSE;
  }
  
  flag_no = dis_no / 8;
  bit_no = dis_no % 8;
  if(intsave->gpower_distribution_bit[flag_no] & (1 << bit_no)){
    return TRUE;
  }
  return FALSE;
}

