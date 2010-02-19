//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wifi_bsubway.h
 *	@brief  Wi-Fi�o�g���T�u�E�F�C
 *	@author	tomoya takahashi
 *	@date		2010.02.17
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "gamesystem/gamesystem.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WiFi�o�g���T�u�E�F�C�����@�N�����[�h
//=====================================
typedef enum {
  WIFI_BSUBWAY_MODE_SCORE_UPLOAD,         // �O��̋L�^���A�b�v���[�h
  WIFI_BSUBWAY_MODE_GAMEDATA_DOWNLOAD,    // �Q�[�������_�E�����[�h
  WIFI_BSUBWAY_MODE_SUCCESSDATA_DOWNLOAD, // �������_�E�����[�h

  WIFI_BSUBWAY_MODE_MAX, // �������Ŏg�p
} WIFI_BSUBWAY_MODE;


//-------------------------------------
///	����
//=====================================
typedef enum {
  WIFI_BSUBWAY_RESULT_OK,                   //�A�b�v�f�[�g�E�_�E�����[�h����
  WIFI_BSUBWAY_RESULT_NG,                   //���s


  WIFI_BSUBWAY_RESULT_MAX, //�������Ŏg�p
} WIFI_BSUBWAY_RESULT;


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {

  // �ݒ�f�[�^
  WIFI_BSUBWAY_MODE mode;
  GAMESYS_WORK*     p_gamesystem;

  // ���ʃf�[�^
  WIFI_BSUBWAY_RESULT result;

} WIFI_BSUBWAY_PARAM;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern const GFL_PROC_DATA WIFI_BSUBWAY_Proc;

FS_EXTERN_OVERLAY(wifi_bsubway);

#ifdef _cplusplus
}	// extern "C"{
#endif



