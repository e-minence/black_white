//==============================================================================
/**
 * @file    save_outside.h
 * @brief   �Z�[�u�Ǘ��O�̈�̃Z�[�u�V�X�e���̃w�b�_
 * @author  matsuda
 * @date    2010.04.11(��)
 */
//==============================================================================
#pragma once

#include "savedata/mystery_data.h"
#include "savedata/mystery_data_local.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�Ǘ��O�s�v�c�f�[�^�ŏ����ł���J�[�h��
#define OUTSIDE_MYSTERY_MAX     (3)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�Ǘ��O�Z�[�u�F�s�v�c�ȑ��蕨
typedef struct{
  u8 recv_flag[MYSTERY_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  GIFT_PACK_DATA card[OUTSIDE_MYSTERY_MAX];		// �J�[�h���
  RECORD_CRC crc;   //CRC & �Í����L�[   4 byte
}OUTSIDE_MYSTERY;

///�Ǘ��O�Z�[�u�V�X�e���̕s��`�^
typedef struct _OUTSIDE_SAVE_CONTROL OUTSIDE_SAVE_CONTROL;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern OUTSIDE_SAVE_CONTROL * OutsideSave_SystemLoad(HEAPID heap_id);
extern void OutsideSave_SystemUnload(OUTSIDE_SAVE_CONTROL *outsv_ctrl);
extern void OutsideSave_SaveAsyncInit(OUTSIDE_SAVE_CONTROL *outsv_ctrl);
extern BOOL OutsideSave_SaveAsyncMain(OUTSIDE_SAVE_CONTROL *outsv_ctrl);

//--------------------------------------------------------------
//  �c�[��
//--------------------------------------------------------------
extern BOOL OutsideSave_GetExistFlag(OUTSIDE_SAVE_CONTROL *outsv_ctrl);
extern BOOL OutsideSave_GetBreakFlag(OUTSIDE_SAVE_CONTROL *outsv_ctrl);
extern OUTSIDE_MYSTERY * OutsideSave_GetMysterPtr(OUTSIDE_SAVE_CONTROL *outsv_ctrl);
extern void OutsideSave_MysteryData_Outside_to_Normal(OUTSIDE_SAVE_CONTROL *outsv_ctrl, SAVE_CONTROL_WORK *normal_ctrl);
