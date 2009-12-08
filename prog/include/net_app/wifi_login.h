//=============================================================================
/**
 * @file	  wifi_login.h
 * @bfief	  WIFI ���O�C���֘A�̃w�b�_�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/11/17
 */
//=============================================================================


#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//-------------------------------------
///	WIFILOGIN�I��
//=====================================
typedef enum
{
  WIFILOGIN_RESULT_LOGIN, //���O�C������
  WIFILOGIN_RESULT_CANCEL,//�L�����Z������
} WIFILOGIN_RESULT;


//-------------------------------------
///	WIFILOGIN�ɓn������
//=====================================
typedef struct 
{
  GAMEDATA      *gamedata;  //[in ]�Q�[���f�[�^
  GAMESYS_WORK      *gsys;  //[in ]�Q�[���V�X�e��
//  SAVE_CONTROL_WORK *ctrl;  //[in ]�Z�[�u�f�[�^
  int bDreamWorld;          //[in ]�h���[�����[���h�̎��͔w�i�ύX
  WIFILOGIN_RESULT  result; //[out]�I�����@
} WIFILOGIN_PARAM;


typedef struct _WIFILOGIN_WORK  WIFILOGIN_WORK;

extern const GFL_PROC_DATA WiFiLogin_ProcData;

FS_EXTERN_OVERLAY(wifi_login);

