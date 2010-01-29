//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_light_status.h
 *	@brief  バトルに引き継ぐフィールドライトカラー
 *	@author	tomoya takahashi
 *	@date		2010.01.29
 *  
 *	モジュール名：FIELD_LIGHT_STATUS
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib>

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
//-------------------------------------
///	FIELD_LIGHT_STATUS
//=====================================
typedef struct {
  GXRgb   light;
  GXRgb   diffuse;
  GXRgb   ambient;
  GXRgb   specular;
  GXRgb   emission;
  GXRgb   pad;      // padding
} FIELD_LIGHT_STATUS;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

#ifdef _cplusplus
}	// extern "C"{
#endif



