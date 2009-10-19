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


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  OCCUPY_INFO occupy;
}INTRUDE_SAVE_WORK;


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
    occupy->town.town_occupy[i] = OCCUPY_TOWN_NEUTRALITY;
  }
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
	INTRUDE_SAVE_WORK *intsave = SaveControl_DataPtrGet(sv, GMDATA_ID_INTRUDE);

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
	INTRUDE_SAVE_WORK *intsave = SaveControl_DataPtrGet(sv, GMDATA_ID_INTRUDE);

  *dest_occupy = intsave->occupy;
}
