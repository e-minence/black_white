//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		event_debug_beacon.h
 *	@brief	デバッグ	ビーコンを介しての友達コード登録
 *	@author	Toru=Nagihashi
 *	@date		2009.11.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
extern GMEVENT * EVENT_DebugBeacon( GAMESYS_WORK *p_gsys, void *work );

FS_EXTERN_OVERLAY( debug_beacon );
