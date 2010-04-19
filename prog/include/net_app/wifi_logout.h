//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		include\net_app\wifi_logout.h
 *	@brief  WIFI ログアウト画面
 *	@author	Toru=Nagihashi
 *	@date		2010.02.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "wifi_login.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	WIFILOGOUTに渡す引数
//=====================================
typedef struct 
{
  GAMEDATA            *gamedata;//[in ]ゲームデータ
  WIFILOGIN_BG        bg;       //[in ]使用する背景 WIFILOGINと同じモジュールを使っているため
  WIFILOGIN_DISPLAY   display;  //[in ]どちらの画面を使うか WIFILOGINと同じモジュールを使っているため
  WIFILOGIN_BGM       bgm;      //[in ]BGM設定
} WIFILOGOUT_PARAM;

//=============================================================================
/**
 *					外部公開
*/
//=============================================================================
//-------------------------------------
///	プロセスデータ
//=====================================
//FS_EXTERN_OVERLAY( wifi_login );  //wifi_login.hをincludeしているので
extern const GFL_PROC_DATA  WiFiLogout_ProcData;
