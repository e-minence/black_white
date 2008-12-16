//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_system.h
 *	@brief		WiFiロビー共通処理システム
 *	@author		tomoya takahashi
 *	@data		2007.09.14
 *
 *	WiFiロビー中常に動作しているシステムはここで動かす。
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_SYSTEM_H__
#define __WFLBY_SYSTEM_H__

#include "wflby_system_def.h"
#include "savedata/save_control.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern WFLBY_SYSTEM* WFLBY_SYSTEM_Init( SAVE_CONTROL_WORK* p_save, u32 heapID );
extern void WFLBY_SYSTEM_Exit( WFLBY_SYSTEM* p_wk );
extern void WFLBY_SYSTEM_Main( WFLBY_SYSTEM* p_wk );
extern void WFLBY_SYSTEM_VBlank( WFLBY_SYSTEM* p_wk );


#endif		// __WFLBY_SYSTEM_H__

