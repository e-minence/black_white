//============================================================================
/**
 *
 *	@file		intro.h
 *	@brief  3D�f���Đ��A�v��
 *	@author	genya hosaka
 *	@data		2009.11.27
 *
 */
//============================================================================
#pragma once

//	lib
#include <gflib.h>

//system
#include "gamesystem/gamesystem.h"

#include "savedata/save_control.h" // for 
#include "savedata/save_control_intr.h" // for INTR_SAVE_CONTROL

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern const GFL_PROC_DATA IntroProcData;

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
//@TODO cdat�ƌ��J�w�b�_�̘A���͔������Ȃ��B
// src/demo/intro/intro_cmd_data.cdat �ƑΉ�
typedef enum
{ 
  INTRO_SCENE_ID_INIT = 0,
  INTRO_SCENE_ID_00,
  INTRO_SCENE_ID_01,
  INTRO_SCENE_ID_02,
  INTRO_SCENE_ID_03,
  INTRO_SCENE_ID_04,
  INTRO_SCENE_ID_05,
  INTRO_SCENE_ID_05_RETAKE_YESNO,
  INTRO_SCENE_ID_06,
  INTRO_SCENE_ID_07,
  INTRO_SCENE_ID_MAX,
} INTRO_SCENE_ID;

//--------------------------------------------------------------
///	
//==============================================================
typedef enum {
  INTRO_RETCODE_NORMAL = 0, ///< �ʏ�
  INTRO_RETCODE_RETAKE,     ///< ��蒼��
} INTRO_RETCODE;

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//-------------------------------------
///	PROC�ɓn������
//=====================================
typedef struct {	
  // [IN]
  SAVE_CONTROL_WORK*  save_ctrl;
  INTRO_SCENE_ID      scene_id;
  INTR_SAVE_CONTROL*  intr_save;
  // [OUT]
  INTRO_RETCODE       retcode;
} INTRO_PARAM;

FS_EXTERN_OVERLAY(intro);

extern const GFL_PROC_DATA ProcData_Intro;




