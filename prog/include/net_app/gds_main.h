//==============================================================================
/**
 * @file	gds_main.h
 * @brief	GDSプロック制御メインのヘッダ
 * @author	matsuda
 * @date	2008.01.17(木)
 */
//==============================================================================
#ifndef __GDS_MAIN_H__
#define __GDS_MAIN_H__

#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "net_app/gds/gds_test.h"
#include "net_app/battle_recorder.h"


//==============================================================================
//	構造体定義
//==============================================================================
///GDSプロック制御を呼び出すときに引き渡すパラメータ構造体
typedef struct{
  GAMEDATA          *gamedata;    ///WiFiListの取得をGAMEDATA経由でするため、
	BR_MODE gds_mode;			///<BR_MODE_???
}GDSPROC_PARAM;

///GDSプロック制御のメイン構造体
typedef struct{
	GDSPROC_PARAM *proc_param;		///<呼び出しパラメータへのポインタ
	BOOL connect_success;			///<TRUE:ネット接続中。　FALSE:接続していない

  GFL_PROCSYS *proc_sys;

	DWCSvlResult aSvl;
	union{
    WIFILOGIN_PARAM login_param;
    WIFILOGOUT_PARAM logout_param;
    BATTLERECORDER_PARAM  br_param;
  };

#ifdef PM_DEBUG
  GDS_TEST_PARENT gds_test_parent;
#endif
}GDSPROC_MAIN_WORK;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern const GFL_PROC_DATA GdsMainProcData;


#endif	//__GDS_MAIN_H__
