//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fieldmap_ctrl_hybrid.h
 *	@brief  マップコントロール　ハイブリッド
 *	@author	tomoya takahashi
 *	@date		2009.10.27
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "fieldmap.h"

#include "field_player_grid.h"
#include "field_player_nogrid.h"

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
///	FIELDMAP_CTRL_HYBRIDワーク
//=====================================
typedef struct _FIELDMAP_CTRL_HYBRID FIELDMAP_CTRL_HYBRID;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

extern const DEPEND_FUNCTIONS FieldMapCtrl_HybridFunctions;



// 情報の取得
extern FLDMAP_BASESYS_TYPE FIELDMAP_CTRL_HYBRID_GetBaseSystemType( const FIELDMAP_CTRL_HYBRID* cp_wk );
extern FIELD_PLAYER_GRID* FIELDMAP_CTRL_HYBRID_GetFieldPlayerGrid( const FIELDMAP_CTRL_HYBRID* cp_wk );
extern FIELD_PLAYER_NOGRID* FIELDMAP_CTRL_HYBRID_GetFieldPlayerNoGrid( const FIELDMAP_CTRL_HYBRID* cp_wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



