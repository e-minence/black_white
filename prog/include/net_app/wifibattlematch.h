//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch.h
 *	@brief	WIFIのバトルマッチ画面
 *	@author	Toru=Nagihashi
 *	@date		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "savedata/mystatus.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	種類
//=====================================
typedef enum
{
	WIFIBATTLEMATCH_MODE_RANDOM_FREE,
	WIFIBATTLEMATCH_MODE_RANDOM_RATE,
	WIFIBATTLEMATCH_MODE_WIFI_LIMIT,
	WIFIBATTLEMATCH_MODE_WIFI_NOLIMIT,
	WIFIBATTLEMATCH_MODE_LIVE,

} WIFIBATTLEMATCH_MODE;


//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	PROCパラメータ
//=====================================
typedef struct 
{
	WIFIBATTLEMATCH_MODE	mode;
	const MYSTATUS				*p_my;
  SAVE_CONTROL_WORK     *p_save;
} WIFIBATTLEMATCH_PARAM;


//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
extern const GFL_PROC_DATA	WifiBattleMaptch_ProcData;
