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
extern void EVENT_DebugBeacon( GAMESYS_WORK *p_gsys, FIELDMAP_WORK *p_field, FLDMSGBG *p_msg_bg, GMEVENT *p_event, HEAPID heapID );
