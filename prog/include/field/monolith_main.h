//==============================================================================
/**
 * @file    monolith_main.h
 * @brief   ���m���X��ʃ��C���̃w�b�_
 * @author  matsuda
 * @date    2009.11.24(��)
 */
//==============================================================================
#pragma once

#include "field/intrude_common.h"
#include "field/field_comm/mission_types.h"
#include "field/field_comm/intrude_types.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///���m���X��ʌĂяo���悤��ParentWork
typedef struct{
  GAMESYS_WORK *gsys;
  INTRUDE_COMM_SYS_PTR intcomm;
  MISSION_CHOICE_LIST list;     ///<�I�����̃~�b�V�����ꗗ
  MONOLITH_STATUS monolith_status;
  u8 palace_area;
  u8 list_occ;                  ///<TRUE:�~�b�V�����ꗗ��M�ς� FALSE:����M(�ʐM���肪���Ȃ�)
  u8 padding[2];
}MONOLITH_PARENT_WORK;


//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const GFL_PROC_DATA MonolithProcData;

//==============================================================================
//  OVERLAY
//==============================================================================
FS_EXTERN_OVERLAY(monolith);
