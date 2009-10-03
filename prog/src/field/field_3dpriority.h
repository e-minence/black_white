//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_3dpriority.h
 *	@brief	フィールド上で、３D面をBGのように扱う場合の、カメラニア面からの表示オフセット値
 *	@author	tomoya takahashi
 *	@date		2009.05.11
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					３D面をBGのように扱う場合には、その表示優先順位をここに記述する
*/
//-----------------------------------------------------------------------------

// 天候に使用する平面
#define FLD_3DPRI_WEATHER_OFFS	( 80 << FX32_SHIFT )

#ifdef _cplusplus
}	// extern "C"{
#endif



