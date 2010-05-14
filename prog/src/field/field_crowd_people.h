//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_crowd_people.h
 *	@brief  群集管理処理
 *	@author	tomoya takahshi
 *	@date		2010.01.26
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "fieldmap_func.h"

FS_EXTERN_OVERLAY(field_crowd_people);

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
///	管理ワーク
//=====================================
typedef struct _FIELD_CROWD_PEOPLE FIELD_CROWD_PEOPLE;


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern const FLDMAPFUNC_DATA c_FLDMAPFUNC_CROWDPEOPLE_DATA;

extern void FIELD_CROWD_PEOPLE_ClearSubWindow( FIELD_CROWD_PEOPLE* p_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif



