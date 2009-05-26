//==============================================================================
/**
 * @file    net_exchange.h
 * @brief   Wi-Fi�L��A�}�b�`���O�ŋ��ʂ��Ďg�p��������f�[�^�̃w�b�_
 * @author  matsuda
 * @date    2009.05.25(��)
 */
//==============================================================================
#pragma once

#include "savedata/save_control.h"
#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"
#include "savedata/wifihistory.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///Wi-Fi�ŋ��ʂ��������f�[�^
typedef struct{
  DWCFriendData friendData;   //�ʐM�œn���p�̃t�����h�R�[�h
  MYSTATUS mystatus;
  u8 mac[WM_SIZE_BSSID];  ///mac
  u8 my_nation;			///<�����̍�
  u8 my_area;				///<�����̒n��
}WIFI_EXCHANGE_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void NET_EXCHANGE_SetParam(SAVE_CONTROL_WORK *sv, WIFI_EXCHANGE_WORK *exc);
