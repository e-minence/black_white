//============================================================================================
/**
 * @file	gamemsystem.h
 * @brief	ポケモンゲームシステム（ここが通常ゲームのトップレベル）
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.21
 *
 */
//============================================================================================

#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef struct _GAMESYS_WORK GAMESYS_WORK;

typedef struct _GMEVENT_CONTROL GMEVENT_CONTROL;


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern const GFL_PROC_DATA GameMainProcData;

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void GameSystem_SetNextProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void GameSystem_CallProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern GMEVENT_CONTROL * GAMESYSTEM_GetEvent(GAMESYS_WORK * gsys);
extern void GAMESYSTEM_SetEvent(GAMESYS_WORK * gsys, GMEVENT_CONTROL * event);

//------------------------------------------------------------------
/**
 * @brief	プロセス起動中かどうかを返す
 * @return	BOOL	TRUEのとき、プロセスが存在する
 */
//------------------------------------------------------------------
extern BOOL GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys);
//------------------------------------------------------------------
//------------------------------------------------------------------
extern GAMEDATA * GAMESYSTEM_GetGameData(GAMESYS_WORK * gsys);
extern PLAYER_WORK * GAMESYSTEM_GetMyPlayerWork(GAMESYS_WORK * gsys);

#ifdef	__cplusplus
} /* extern "C" */
#endif
