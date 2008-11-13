//============================================================================================
/**
 * @file	event_mapchange.c
 * @brief
 * @date	2008.11.04
 * @author	tamada GAME FREAK inc.
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field_data.h"

#include "event_mapchange.h"
//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	GAMEDATA * gamedata;
	GAME_INIT_WORK * game_init_work;
}FIRST_MAPIN_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FirstMapIn(GMEVENT_CONTROL * event, int *seq, void *work)
{
	FIRST_MAPIN_WORK * fmw = work;
	GAMESYS_WORK * gsys = fmw->gsys;
	GAME_INIT_WORK * game_init_work = ((FIRST_MAPIN_WORK *)work)->game_init_work;
	switch (*seq) {
	case 0:
		{
			PLAYER_WORK * mywork = GAMEDATA_GetMyPlayerWork(fmw->gamedata);
			ZONEID start_id;
			const VecFx32 *start_pos;
			start_id = game_init_work->mapid;
			PLAYERWORK_setZoneID(mywork, start_id);
			start_pos = FIELDDATA_GetStartPosition(start_id);
			PLAYERWORK_setPosition(mywork, start_pos);
			PLAYERWORK_setDirection(mywork, 0);
		}
		(*seq)++;
		break;
	case 1:
#if 1		/* �b��I�Ƀv���Z�X�o�^ */
		GameSystem_CallFieldProc(gsys);
#endif
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void DEBUG_EVENT_SetFirstMapIn(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work)
{
	FIRST_MAPIN_WORK * fmw;
	GMEVENT_CONTROL * event;
	event = GAMESYSTEM_EVENT_Set(gsys, EVENT_FirstMapIn, sizeof(FIRST_MAPIN_WORK));
	fmw = GMEVENT_GetEventWork(event);
	fmw->gsys = gsys;
	fmw->gamedata = GAMESYSTEM_GetGameData(gsys);
	fmw->game_init_work = game_init_work;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	GAMEDATA * gamedata;
	ZONEID next_map;
}MAPCHANGE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChange(GMEVENT_CONTROL * event, int *seq, void*work)
{
	MAPCHANGE_WORK * mcw = work;
	GAMESYS_WORK  * gsys = mcw->gsys;
	GAMEDATA * gamedata = mcw->gamedata;
	switch (*seq) {
	case 0:
		if (GAMESYSTEM_IsProcExists(gsys)) break;
		(*seq)++;
		break;
	case 1:
		{
			PLAYER_WORK * mywork = GAMEDATA_GetMyPlayerWork(mcw->gamedata);
			const VecFx32 *start_pos;
			PLAYERWORK_setZoneID(mywork, mcw->next_map);
			start_pos = FIELDDATA_GetStartPosition(mcw->next_map);
			PLAYERWORK_setPosition(mywork, start_pos);
			PLAYERWORK_setDirection(mywork, 0);
		}
		GameSystem_CallFieldProc(gsys);
		(*seq)++;
		break;
	case 2:
		if (!GAMESYSTEM_IsProcExists(gsys)) break;

		return GMEVENT_RES_FINISH;

	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void DEBUG_EVENT_ChangeToNextMap(GAMESYS_WORK * gsys)
{
	MAPCHANGE_WORK * mcw;
	GMEVENT_CONTROL * event;
	event = GAMESYSTEM_EVENT_Set(gsys, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
	mcw->gsys = gsys;
	mcw->gamedata = GAMESYSTEM_GetGameData(gsys);
	{
		PLAYER_WORK * myplayer = GAMEDATA_GetMyPlayerWork(mcw->gamedata);
		ZONEID next = PLAYERWORK_getZoneID(myplayer);
		next ++;
		if (next >= FIELDDATA_GetMapIDMax()) {
			next = 0;
		}
		mcw->next_map = next;
	}
}
//============================================================================================
//============================================================================================
extern const GFL_PROC_DATA TestProg1MainProcData;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent(GMEVENT_CONTROL * event, int * seq, void * work)
{
	GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);

	switch(*seq) {
	case 0:
		if (GAMESYSTEM_IsProcExists(gsys)) break;
		(*seq) ++;
		break;
	case 1:
		GameSystem_CallProc(gsys, NO_OVERLAY_ID, &TestProg1MainProcData, NULL);
		(*seq) ++;
		break;
	case 2:
		if (GAMESYSTEM_IsProcExists(gsys)) break;
		(*seq) ++;
		break;
	case 3:
		GameSystem_CallFieldProc(gsys);
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}

void DEBUG_EVENT_FieldSample(GAMESYS_WORK * gsys)
{
	GAMESYSTEM_EVENT_Set(gsys, GameChangeEvent, 0);
}
