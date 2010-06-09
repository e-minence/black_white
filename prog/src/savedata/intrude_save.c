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
#include "field/field_comm/mission_types.h"
#include "field/field_comm/intrude_mission.h"
#include "field/intrude_secret_item_def.h"



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
  MISSION_LIST_Create(occupy);
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

//==================================================================
/**
 * �����x���A�b�v
 *
 * @param   occupy		
 * @param   add_level		���x�����Z�l
 */
//==================================================================
void OccupyInfo_LevelUpWhite(OCCUPY_INFO *occupy, int add_level)
{
  if(occupy->white_level + add_level > OCCUPY_LEVEL_MAX){
    occupy->white_level = OCCUPY_LEVEL_MAX;
  }
  else{
    occupy->white_level += add_level;
  }
}

//==================================================================
/**
 * �����x���A�b�v
 *
 * @param   occupy		
 * @param   add_level		���x�����Z�l
 */
//==================================================================
void OccupyInfo_LevelUpBlack(OCCUPY_INFO *occupy, int add_level)
{
  if(occupy->black_level + add_level > OCCUPY_LEVEL_MAX){
    occupy->black_level = OCCUPY_LEVEL_MAX;
  }
  else{
    occupy->black_level += add_level;
  }
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
  
  //�s���`�F�b�N
  {
    STRCODE EOMCODE = GFL_STR_GetEOMCode();
    int pos;
    for(pos = 0; pos < PERSON_NAME_SIZE; pos++){
      if(src->name[pos] == EOMCODE){
        break;
      }
    }
    if(pos == PERSON_NAME_SIZE){
      return;
    }
    if(src->tbl_no >= SECRET_ITEM_DATA_TBL_MAX){
      return;
    }
  }
  
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
  
  dis_no = gpower_id - GPOWER_ID_DISTRIBUTION_START;
  if(dis_no < 0 || gpower_id > GPOWER_ID_DISTRIBUTION_END){
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
  
  dis_no = gpower_id - GPOWER_ID_DISTRIBUTION_START;
  if(dis_no < 0 || gpower_id > GPOWER_ID_DISTRIBUTION_END){
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

//==================================================================
/**
 * �z�z��pG�p���[��bit���[�N��z��R�s�[����
 *
 * @param   intsave		
 * @param   dest_array		�����
 * @param   array_num		  dest_array�̔z��v�f��
 */
//==================================================================
void ISC_SAVE_GetDistributionGPower_Array(INTRUDE_SAVE_WORK *intsave, u8 *dest_array, int array_num)
{
  int i;
  
  GF_ASSERT(array_num >= INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX);
  for(i = 0; i < INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX; i++){
    dest_array[i] = intsave->gpower_distribution_bit[i];
  }
}

//==================================================================
/**
 * �~�b�V�������N���A�����񐔂��擾
 *
 * @param   intsave		
 *
 * @retval  u32		    �N���A������
 */
//==================================================================
u32 ISC_SAVE_GetMissionClearCount(INTRUDE_SAVE_WORK *intsave)
{
  return intsave->clear_mission_count;
}

//==================================================================
/**
 * �~�b�V�������N���A�����񐔂��C���N�������g
 *
 * @param   intsave		
 *
 * @retval  u32		    �C���N�������g��̃N���A������
 */
//==================================================================
u32 ISC_SAVE_IncMissionClearCount(INTRUDE_SAVE_WORK *intsave)
{
  if(intsave->clear_mission_count < INTRUDE_MISSION_CLEAR_COUNT_MAX){
    intsave->clear_mission_count++;
  }
  return intsave->clear_mission_count;
}

//==================================================================
/**
 * �p���X�؍ݎ��Ԃ��擾
 *
 * @param   intsave		
 *
 * @retval  s64		    �؍ݎ���(�b)
 */
//==================================================================
s64 ISC_SAVE_GetPalaceSojournTime(INTRUDE_SAVE_WORK *intsave)
{
  return intsave->palace_sojourn_time;
}

//==================================================================
/**
 * �p���X�؍ݎ��Ԃ��Z�b�g
 *
 * @param   intsave		
 * @param   second  �؍ݎ���(�b)
 */
//==================================================================
void ISC_SAVE_SetPalaceSojournTime(INTRUDE_SAVE_WORK *intsave, s64 second)
{
  intsave->palace_sojourn_time = second;
}

//==================================================================
/**
 * �p���X�؍ݎ��Ԃɑ΂���A�N�Z�X�F�X
 *
 * @param   intsave		
 * @param   playtime		���݂̃v���C����
 * @param   mode		    SOJOURN_TIME_xxx
 *
 * @retval  s64		SOJOURN_TIME_xxx�ɂ���ĕω�
 *
 * �p���X��������SOJOURN_TIME_PUSH�œ������Ԃ�ۑ�
 * 
 * �p���X�ގ�����SOJOURN_TIME_CALC_SET�œ������Ԃƌ��ݎ��Ԃ̍��������đ؍ݎ��Ԃɉ��Z
 * 
 * �p���X�������Ƀp���X�؍ݎ��Ԃ��擾�������ꍇ��SOJOURN_TIME_CALC_GET��
 * �������Ԃ���̃I�t�Z�b�g�����Z�����؍ݎ��Ԃ��擾
 */
//==================================================================
s64 ISC_SAVE_PalaceSojournParam(INTRUDE_SAVE_WORK *intsave, PLAYTIME *playtime, SOJOURN_TIME_MODE mode)
{
  static s64 in_time_sec;   //��������Push
  s64 temp_hour, temp_min, temp_sec, now_sec;

  temp_hour = PLAYTIME_GetHour(playtime);
  temp_min = PLAYTIME_GetMinute(playtime);
  temp_sec = PLAYTIME_GetSecond(playtime);
  now_sec = (temp_hour * 60 * 60) + (temp_min * 60) + temp_sec;
  
  switch(mode){
  case SOJOURN_TIME_PUSH:     //�������ԂƂ��Č��݂̃v���C���Ԃ��o�b�t�@�֕ۑ�
    in_time_sec = now_sec;
    break;
  case SOJOURN_TIME_CALC_SET: //�������Ԃƌ��ݎ��Ԃ̍��������đ؍ݎ��Ԃ։��Z
    OS_TPrintf("�p���X�؍ݎ��� + %d�b\n", now_sec - in_time_sec);
    intsave->palace_sojourn_time += now_sec - in_time_sec;
    break;
  case SOJOURN_TIME_CALC_GET: //�������Ԃƌ��ݎ��Ԃ̍��������ĉ��Z�����؍ݎ��Ԃ��擾
    return intsave->palace_sojourn_time + (now_sec - in_time_sec);
  }
  
  return intsave->palace_sojourn_time;
}

