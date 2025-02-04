//============================================================================================
/**
 * @file	musical_dist_save.h
 * @brief	ミュージカル配信用セーブデータ
 * @author	Nobuhiko Ariizumi
 * @date	2009/04/08
 */
//============================================================================================

#pragma once

#include "gamesystem/game_data.h"

//ミュージカル配信のダウンロード実サイズ
#define MUSICAL_DIST_SAVE_DL_SIZE ((127*1024)-(2-16))
//======================================================================
//	typedef struct
//======================================================================
typedef struct _MUSICAL_DIST_SAVE MUSICAL_DIST_SAVE;


//======================================================================
//	proto
//======================================================================
//ミュージカル配信データはnaixを持っていないし、コンバータで順番固定で出力しているので
//ココでenum定義
enum
{
  MUSICAL_ARCDATAID_PROGDATA = 0,
  MUSICAL_ARCDATAID_GMMDATA = 1,
  MUSICAL_ARCDATAID_SCRIPTDATA = 2,
  MUSICAL_ARCDATAID_SBNKDATA = 3,
  MUSICAL_ARCDATAID_SSEQDATA = 4,
  MUSICAL_ARCDATAID_SWAVDATA = 5,
};

//----------------------------------------------------------
//	セーブデータシステムが依存する関数
//----------------------------------------------------------
extern int MUSICAL_DIST_SAVE_GetWorkSize(void);
extern void MUSICAL_DIST_SAVE_InitWork(MUSICAL_DIST_SAVE * musSave);

//----------------------------------------------------------
//	データのセーブ・ロード
//----------------------------------------------------------
extern MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_LoadData( GAMEDATA *gamedata , HEAPID heapId );
extern void MUSICAL_DIST_SAVE_UnloadData( MUSICAL_DIST_SAVE *distSave );

//----------------------------------------------------------
//	データ操作のための関数
//----------------------------------------------------------
//データの設定(セーブ、Load・UnLoad込み)
extern MUSICAL_DIST_SAVE* MUSICAL_DIST_SAVE_SaveMusicalArchive_Init( GAMEDATA *gamedata , void *arcData , const u32 size , const HEAPID heapId );
extern const BOOL MUSICAL_DIST_SAVE_SaveMusicalArchive_Main( MUSICAL_DIST_SAVE *distSave );
//データの取得
extern void* MUSICAL_DIST_SAVE_GetMusicalArc( MUSICAL_DIST_SAVE *distSave );
extern const u32 MUSICAL_DIST_SAVE_GetMusicalArcSize( MUSICAL_DIST_SAVE *distSave );
