//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_rail_loader.h
 *	@brief	フィールドレールローダー
 *	@author	tomoya takahashi
 *	@date		2009.07.07
 *
 *	モジュール名：FIELD_RAIL_LOADER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_rail.h"

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
///	ワーク
//=====================================
typedef struct _FIELD_RAIL_LOADER FIELD_RAIL_LOADER;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// システム作成・破棄
extern FIELD_RAIL_LOADER* FIELD_RAIL_LOADER_Create( u32 heapID );
extern void FIELD_RAIL_LOADER_Delete( FIELD_RAIL_LOADER* p_sys );

// レール情報読み込み・クリア
extern void FIELD_RAIL_LOADER_Load( FIELD_RAIL_LOADER* p_sys, u32 datano, u32 heapID );
extern void FIELD_RAIL_LOADER_Clear( FIELD_RAIL_LOADER* p_sys );

// レールセットアップ情報取得
extern const RAIL_SETTING* FIELD_RAIL_LOADER_GetData( const FIELD_RAIL_LOADER* cp_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif



