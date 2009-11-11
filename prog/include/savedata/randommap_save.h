//============================================================================================
/**
 * @file	randommap_save.c
 * @brief	ランダム生成マップ用セーブデータ
 * @author	Nobuhiko Ariizumi   tomoya takahashi
 * @date	2009/04/08
 *
 * モジュール名：RANDOMMAP_SAVE
 */
//============================================================================================

#pragma once

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK参照のため

#include "field/field_wfbc_data.h"

//======================================================================
//	typedef struct
//======================================================================

typedef struct _RANDOMMAP_SAVE RANDOMMAP_SAVE;

//======================================================================
//	proto
//======================================================================

//----------------------------------------------------------
//	セーブデータシステムが依存する関数
//----------------------------------------------------------
extern int RANDOMMAP_SAVE_GetWorkSize(void);
extern void RANDOMMAP_SAVE_InitWork(RANDOMMAP_SAVE *randomMapSave);

//----------------------------------------------------------
//	
//----------------------------------------------------------
extern RANDOMMAP_SAVE* RANDOMMAP_SAVE_GetRandomMapSave( SAVE_CONTROL_WORK *sv );


//----------------------------------------------------------
// 街の情報を設定
//----------------------------------------------------------
extern void RANDOMMAP_SAVE_SetCoreWork( RANDOMMAP_SAVE* sv, const FIELD_WFBC_CORE* cp_buff );

//----------------------------------------------------------
// 街の情報を取得
//----------------------------------------------------------
extern void RANDOMMAP_SAVE_GetCoreWork( const RANDOMMAP_SAVE* sv, FIELD_WFBC_CORE* p_buff );




