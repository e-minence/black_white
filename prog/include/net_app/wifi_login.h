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
#include "net/network_define.h"
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
///	WIFILOGIN�\�����@ ������͂����������\�������f�B�X�v���C
//=====================================
typedef enum
{
  WIFILOGIN_DISPLAY_DOWN, //����ʂɃe�L�X�g�ƑI������\��
  WIFILOGIN_DISPLAY_UP,   //���ʂɃe�L�X�g�ƑI������\��
} WIFILOGIN_DISPLAY;

//-------------------------------------
///	WIFILOGIN�\�����@ 
//=====================================
typedef enum
{
  WIFILOGIN_BG_NORMAL,      //�ʏ�p
  WIFILOGIN_BG_DREAM_WORLD, //�h���[�����[���h�p
} WIFILOGIN_BG;

//-------------------------------------
///	WIFILOGIN_BGM�ݒ�
//=====================================
typedef enum
{
  WIFILOGIN_BGM_NORMAL,     //�ʏ�p�i������SEQ_BGM_WIFI_ACCESS���Ăт܂��j
  WIFILOGIN_BGM_NONE,       //BGM������s��Ȃ�
} WIFILOGIN_BGM;

//-------------------------------------
///	WIFILOGIN�N�����[�h
//=====================================
typedef enum
{
  WIFILOGIN_MODE_NORMAL,      //�ʏ�N��
  WIFILOGIN_MODE_ERROR,       //�G���[�ōĂт����Ƃ��̋N��
  WIFILOGIN_MODE_NOTSAVE,     //����WIFI�ł��Z�[�u�ֈڍs���Ȃ�(�s�v�c�ȑ��蕨�ł����g��Ȃ��͂��ł�)
} WIFILOGIN_MODE;

//-------------------------------------
///	WIFILOGIN_FADE�ݒ�
//    �ȉ��̒l���}�X�N�Ƃ���OR�w�肵�Ă�������
//    ->���݂�WIFILOGOUT�����ł�
//=====================================
typedef enum
{
  WIFILOGIN_FADE_DEFAULT,     //  �u���b�N�C���u���b�N�A�E�g
  WIFILOGIN_FADE_BLACK_IN   = 1<<1,
  WIFILOGIN_FADE_BLACK_OUT  = 1<<2,
  WIFILOGIN_FADE_WHITE_IN   = 1<<3,
  WIFILOGIN_FADE_WHITE_OUT  = 1<<4,
  WIFILOGIN_FADE_BLACK      = WIFILOGIN_FADE_BLACK_IN|WIFILOGIN_FADE_BLACK_OUT,
  WIFILOGIN_FADE_WHITE      = WIFILOGIN_FADE_WHITE_IN|WIFILOGIN_FADE_WHITE_OUT,
} WIFILOGIN_FADE;

//-------------------------------------
///	WIFILOGIN�ɓn������
//=====================================
typedef struct 
{
  GAMEDATA      *gamedata;    //[in ]�Q�[���f�[�^
  WIFILOGIN_BG  bg;           //[in ]�g�p����w�i
  WIFILOGIN_DISPLAY display;  //[in ]�ǂ���̉�ʂ��g����
  NetworkServiceID_e nsid;    //[in ]�l�b�g���[�N�T�[�r�XID
  DWCSvlResult* pSvl;         //[in ]�C�V��SVL�F�؍\���� DWCSvlResult�͎��O�Ŋm�ۂ��Ă�������
  WIFILOGIN_MODE    mode;     //[in ]�N�����[�h
  WIFILOGIN_BGM     bgm;      //[in ]BGM�ݒ�
  WIFILOGIN_RESULT  result;   //[out]�I�����@
} WIFILOGIN_PARAM;


typedef struct _WIFILOGIN_WORK  WIFILOGIN_WORK;

extern const GFL_PROC_DATA WiFiLogin_ProcData;


FS_EXTERN_OVERLAY(wifi_login);

