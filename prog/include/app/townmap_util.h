//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap_util.h
 *	@brief  タウンマップ処理共通ルーチン
 *	@author	Toru=Nagihashi
 *	@date		2010.02.06
 *	@note   タウンマップで行っている主人公の位置サーチなどを
 *	        他の場所でも使うので外部化
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/game_data.h"

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

//=============================================================================
/**
 *					外部公開
*/
//=============================================================================
extern u16 TOWNMAP_UTIL_GetRootZoneID( const GAMEDATA* cp_gamedata, u16 now_zoneID );
extern BOOL TOWNMAP_UTIL_CheckFlag( GAMEDATA* p_gamedata, u16 flag );
