//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_envse_data.h
 *	@brief  環境音SEのデータ
 *	@author	tomoya takahhashi
 *	@date		2010.03.10
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gflib.h"

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
///	データ
//=====================================
typedef struct {
  u32 label;
  u32 loop;
} FLD_ENVSE_DATA;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

// 環境音チェック
extern BOOL FLD_ENVSE_DATA_IsEnvSE( u32 soundIdx );

// ループ音チェック
extern BOOL FLD_ENVSE_DATA_IsLoopSE( u32 soundIdx );


#ifdef _cplusplus
}	// extern "C"{
#endif



