//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		c_gear_pattern.h
 *	@brief  C Gear 初期　パターン　管理
 *	@author	tomoya takahashi
 *	@date		2010.04.05
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/c_gear_data.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define CGEAR_PATTERN_MAX (10)  // patternの最大値
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

extern void CGEAR_PATTERN_SetUp( CGEAR_SAVEDATA* pCGSV, ARCHANDLE* handle, u32 pattern, HEAPID heapID );

#ifdef _cplusplus
}	// extern "C"{
#endif



