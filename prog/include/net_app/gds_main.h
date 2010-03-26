//==============================================================================
/**
 * @file	gds_main.h
 * @brief	GDS�v���b�N���䃁�C���̃w�b�_
 * @author	matsuda
 * @date	2008.01.17(��)
 */
//==============================================================================
#ifndef __GDS_MAIN_H__
#define __GDS_MAIN_H__

#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "net_app/battle_recorder.h"


//==============================================================================
//	�\���̒�`
//==============================================================================
///GDS�v���b�N������Ăяo���Ƃ��Ɉ����n���p�����[�^�\����
typedef struct{
  GAMEDATA          *gamedata;    ///WiFiList�̎擾��GAMEDATA�o�R�ł��邽�߁A
	BR_MODE gds_mode;			///<BR_MODE_???
	u8 padding[3];
}GDSPROC_PARAM;

///GDS�v���b�N����̃��C���\����
typedef struct{
	GDSPROC_PARAM *proc_param;		///<�Ăяo���p�����[�^�ւ̃|�C���^
	BOOL connect_success;			///<TRUE:�l�b�g�ڑ����B�@FALSE:�ڑ����Ă��Ȃ�

  GFL_PROCSYS *proc_sys;

	DWCSvlResult aSVL;
	union{
    WIFILOGIN_PARAM login_param;
    WIFILOGOUT_PARAM logout_param;
    BATTLERECORDER_PARAM  br_param;
  };
}GDSPROC_MAIN_WORK;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern const GFL_PROC_DATA GdsMainProcData;


#endif	//__GDS_MAIN_H__
