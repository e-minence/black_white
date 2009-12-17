//============================================================================
/**
 *
 *	@file		intro_cmd_data.h
 *	@brief  �V�[���f�[�^�ւ̃A�N�Z�T
 *	@author	hosaka genya
 *	@data		2009.12.11
 *
 */
//============================================================================
#pragma once

#include "intro_cmd_def.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
typedef enum
{ 
  INTRO_SCENE_ID_INIT = 0,
  INTRO_SCENE_ID_00,
  INTRO_SCENE_ID_01,
  INTRO_SCENE_ID_02,
  INTRO_SCENE_ID_03,
  INTRO_SCENE_ID_04,
  INTRO_SCENE_ID_05,
  INTRO_SCENE_ID_06,
  INTRO_SCENE_ID_07,
  INTRO_SCENE_ID_MAX,
} INTRO_SCENE_ID;

#define INTRO_CMD_PARAM_MAX (4)

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

typedef enum
{ 
  CMD_READ_END = ( FALSE ), ///< ���̃R�}���h�𓯎��ɓǂݍ��݂܂Ȃ�(�f�t�H���g�l)
  CMD_READ_NEXT = ( TRUE ), ///< ���̃R�}���h�������ɓǂݍ���
} CMD_READ;

//--------------------------------------------------------------
///	
//==============================================================
typedef struct {
  INTRO_CMD_TYPE type;
  int param[ INTRO_CMD_PARAM_MAX ];
  u32 read_next : 1; ///< CMD_READ : CMD_READ_NEXT �Ȃ玟�̃R�}���h�������s
} INTRO_CMD_DATA;

//=============================================================================
/**
 *								EXTERN�錾
 */
//=============================================================================
extern const INTRO_CMD_DATA* Intro_DATA_GetCmdData( INTRO_SCENE_ID scene_id );

extern u16 Intro_DATA_GetSceneMax( void );


