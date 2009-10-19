//==============================================================================
/**
 * @file    intrude_save.h
 * @brief   �N���Z�[�u�f�[�^
 * @author  matsuda
 * @date    2009.10.18(��)
 */
//==============================================================================
#pragma once

#include "field/intrude_common.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�苒���F�X
typedef struct{
  u16 town_occupy[INTRUDE_TOWN_MAX];     ///<�X�̐苒�p�����[�^
}OCCUPY_INFO_TOWN;

///�苒���
typedef struct{
  OCCUPY_INFO_TOWN town;                 ///<�X�̐苒���
  u16 intrude_point;                     ///<�N���|�C���g
  u8 intrude_level;                      ///<�N�����x��
  u8 padding;
}OCCUPY_INFO;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 IntrudeSave_GetWorkSize( void );
extern void IntrudeSave_WorkInit(void *work);

//--------------------------------------------------------------
//  �苒���
//--------------------------------------------------------------
extern void OccupyInfo_WorkInit(OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoUpdate(SAVE_CONTROL_WORK * sv, const OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoLoad(SAVE_CONTROL_WORK * sv, OCCUPY_INFO *dest_occupy);
