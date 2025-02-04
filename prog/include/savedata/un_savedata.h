//============================================================================================
/**
 * @file	un_savedata.h
 * @brief	国連関連セーブデータ
 * @author	saito
 * @date	2010.02.02
 *
 */
//============================================================================================

#pragma once

#include "savedata/save_control.h"
#include "gamesystem/gamedata_def.h"  //GAMEDATA

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	仕掛け用ワークへの不完全型定義
 */
//---------------------------------------------------------------------------
typedef struct _UNSV_WORK UNSV_WORK;

//============================================================================================
//============================================================================================

//----------------------------------------------------------
/**
 * @brief   データへのポインタ取得
 * @param	  gamedata			GAMEDATAへのポインタ
 * @return	データへのポインタ
 *
 * 実態はsrc/gamesystem/game_data.cにあるが、情報隠蔽を
 * 考慮してアクセス関数群を扱うヘッダに配置している
 */
//----------------------------------------------------------
extern UNSV_WORK * GAMEDATA_GetUnsvWork(GAMEDATA * gamedata);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
extern int UNSV_GetWorkSize(void);
extern void UNSV_Init(UNSV_WORK * work);

extern void SaveData_SaveUnsvWork(const UNSV_WORK * inWork, SAVE_CONTROL_WORK * sv);
extern void SaveData_LoadUnsvWork(SAVE_CONTROL_WORK * sv, UNSV_WORK * outWork);

