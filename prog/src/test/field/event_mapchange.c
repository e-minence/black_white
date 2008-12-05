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

#include "field/zonedata.h"
#include "field/fieldmap.h"
#include "field/location.h"
#include "field/eventdata_sxy.h"

#include "event_mapchange.h"

static void UpdateMapParams(GAMESYS_WORK * gsys, const LOCATION * loc_req);

typedef enum {
	FIELD_FADE_BLACK = 0,
	FIELD_FADE_WHITE,
}FIELD_FADE_TYPE;

GMEVENT * EVENT_FieldFadeOut(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap, FIELD_FADE_TYPE type);
GMEVENT * EVENT_FieldFadeIn(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap, FIELD_FADE_TYPE type);
GMEVENT * EVENT_FieldOpen(GAMESYS_WORK *gsys);
GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap);

//============================================================================================
//
//		�T�u�C�x���g
//
//============================================================================================
typedef struct {
	FIELD_FADE_TYPE fade_type;
}FADE_EVENT_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldFadeOutEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK * few = work;
	switch (*seq) {
	case 0:
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
				0, 16, 0);
		(*seq) ++;
		break;
	case 1:
		if (GFL_FADE_CheckFade() == FALSE) {
			return GMEVENT_RES_FINISH;
		}
		break;
	}
	return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldFadeOut(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap, FIELD_FADE_TYPE type)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldFadeOutEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
	few->fade_type = type;

	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldFadeInEvent(GMEVENT * event, int *seq, void * work)
{
	FADE_EVENT_WORK * few = work;
	switch (*seq) {
	case 0:
		GFL_FADE_SetMasterBrightReq(
				GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,
				16, 0, 0);
		(*seq) ++;
		break;
	case 1:
		if (GFL_FADE_CheckFade() == FALSE) {
			return GMEVENT_RES_FINISH;
		}
		break;
	}

	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldFadeIn(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap, FIELD_FADE_TYPE type)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldFadeInEvent, sizeof(FADE_EVENT_WORK));
	FADE_EVENT_WORK * few = GMEVENT_GetEventWork(event);
	few->fade_type = type;

	return event;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELD_MAIN_WORK * fieldmap;
	GAMEDATA * gamedata;
}FIELD_OPENCLOSE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldCloseEvent(GMEVENT * event, int * seq, void *work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELD_MAIN_WORK * fieldmap = focw->fieldmap;
	switch (*seq) {
	case 0:
		FIELDMAP_Close(fieldmap);
		(*seq) ++;
		break;
	case 1:
		if (GAMESYSTEM_IsProcExists(gsys)) break;
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldClose(GAMESYS_WORK *gsys, FIELD_MAIN_WORK * fieldmap)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldCloseEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = fieldmap;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FieldOpenEvent(GMEVENT * event, int *seq, void*work)
{
	FIELD_OPENCLOSE_WORK * focw = work;
	GAMESYS_WORK * gsys = focw->gsys;
	FIELD_MAIN_WORK * fieldmap;	// = focw->fieldmap;
	switch(*seq) {
	case 0:
		if (GAMESYSTEM_IsProcExists(gsys)) break;
		GAMESYSTEM_CallFieldProc(gsys);
		(*seq) ++;
		break;
	case 1:
		fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
		if (FieldMain_IsFieldUpdate(fieldmap) == FALSE) break;
		FieldMain_UpdateFieldFunc(fieldmap);
		(*seq) ++;
		break;
	case 2:
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_FieldOpen(GAMESYS_WORK *gsys)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, FieldOpenEvent, sizeof(FIELD_OPENCLOSE_WORK));
	FIELD_OPENCLOSE_WORK * focw = GMEVENT_GetEventWork(event);
	focw->gsys = gsys;
	focw->fieldmap = NULL;
	focw->gamedata = GAMESYSTEM_GetGameData(gsys);
	return event;
}

//============================================================================================
//
//	�C�x���g�F�Q�[���J�n
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	GAMEDATA * gamedata;
	GAME_INIT_WORK * game_init_work;
	LOCATION loc_req;
}FIRST_MAPIN_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FirstMapIn(GMEVENT * event, int *seq, void *work)
{
	FIRST_MAPIN_WORK * fmw = work;
	GAMESYS_WORK * gsys = fmw->gsys;
	GAME_INIT_WORK * game_init_work = fmw->game_init_work;
	FIELD_MAIN_WORK * fieldmap;
	switch (*seq) {
	case 0:
		UpdateMapParams(gsys, &fmw->loc_req);
		(*seq)++;
		break;
	case 1:
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq)++;
		break;
	case 2:
		fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
		GMEVENT_CallEvent(event,EVENT_FieldFadeIn(gsys, fieldmap, 0));
		(*seq) ++;
		break;
	case 3:
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_SetFirstMapIn(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work)
{
	FIRST_MAPIN_WORK * fmw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_FirstMapIn, sizeof(FIRST_MAPIN_WORK));
	fmw = GMEVENT_GetEventWork(event);
	fmw->gsys = gsys;
	fmw->gamedata = GAMESYSTEM_GetGameData(gsys);
	fmw->game_init_work = game_init_work;
	LOCATION_DEBUG_SetDefaultPos(&fmw->loc_req, game_init_work->mapid);
	return event;
}

//============================================================================================
//
//	�C�x���g�F�f�o�b�O�p�}�b�v�؂�ւ�
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	GAMEDATA * gamedata;
	FIELD_MAIN_WORK * fieldmap;
	LOCATION loc_req;
}MAPCHANGE_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChange(GMEVENT * event, int *seq, void*work)
{
	MAPCHANGE_WORK * mcw = work;
	GAMESYS_WORK  * gsys = mcw->gsys;
	FIELD_MAIN_WORK * fieldmap = mcw->fieldmap;
	GAMEDATA * gamedata = mcw->gamedata;
	switch (*seq) {
	case 0:
		//�t�B�[���h�}�b�v���t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, fieldmap, 0));
		(*seq)++;
		break;
	case 1:
		//�t�B�[���h�}�b�v���I���҂�
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
		(*seq)++;
		break;
	case 2:
		//�V�����}�b�vID�A�����ʒu���Z�b�g
		UpdateMapParams(gsys, &mcw->loc_req);
		(*seq)++;
		break;
	case 3:
		//�t�B�[���h�}�b�v���J�n�҂�
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case 4:
		//�t�B�[���h�}�b�v���t�F�[�h�C��
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, fieldmap, 0));
		(*seq) ++;
		break;
	case 5:
		return GMEVENT_RES_FINISH;

	}
	return GMEVENT_RES_CONTINUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_ChangeMapPos(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos)
{
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
	mcw->gsys = gsys;
	mcw->fieldmap = fieldmap;
	mcw->gamedata = GAMESYSTEM_GetGameData(gsys);
	LOCATION_SetDirect(&mcw->loc_req, zone_id, 0/* DIR */,
			pos->x, pos->y, pos->z);
	
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_ChangeMap(GAMESYS_WORK * gsys,
		FIELD_MAIN_WORK * fieldmap, u16 zone_id, s16 exit_id)
{
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
	mcw->gsys = gsys;
	mcw->fieldmap = fieldmap;
	mcw->gamedata = GAMESYSTEM_GetGameData(gsys);
	LOCATION_SetID(&mcw->loc_req, zone_id, exit_id);
	return event;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMap(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		const LOCATION * loc_req)
{
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
	mcw->gsys = gsys;
	mcw->fieldmap = fieldmap;
	mcw->gamedata = GAMESYSTEM_GetGameData(gsys);
	mcw->loc_req = *loc_req;
	return event;
}
//------------------------------------------------------------------
//	���}�b�vID���C���N�������g���Ă���B�ő�l�ɂȂ�����擪�ɖ߂�
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_ChangeToNextMap(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap)
{
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	PLAYER_WORK * myplayer = GAMEDATA_GetMyPlayerWork(gamedata);
	ZONEID next = PLAYERWORK_getZoneID(myplayer);
	next ++;
	if (next >= ZONEDATA_GetZoneIDMax()) {
		next = 0;
	}
	return DEBUG_EVENT_ChangeMap(gsys, fieldmap, next, 0);
}

//------------------------------------------------------------------
//	���}�b�vID�w��^ �C�x���g�؂�ւ�
//------------------------------------------------------------------
void DEBUG_EVENT_ChangeEventMapChange(
	GAMESYS_WORK *gsys, GMEVENT *event,
	FIELD_MAIN_WORK *fieldmap, ZONEID zone_id )
{
	if (zone_id >= ZONEDATA_GetZoneIDMax()) {
		GF_ASSERT( 0 );
		zone_id = 0;
	}
	GMEVENT_ChangeEvent(event, DEBUG_EVENT_ChangeMap(gsys, fieldmap, zone_id, 0));
}	

//============================================================================================
//
//	�C�x���g�F�ʉ�ʌĂяo��
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	FIELD_MAIN_WORK * fieldmap;
	FSOverlayID ov_id;
	const GFL_PROC_DATA * proc_data;
	void * proc_work;
}CHANGE_SAMPLE_WORK;
//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameChangeEvent(GMEVENT * event, int * seq, void * work)
{
	CHANGE_SAMPLE_WORK * csw = work;
	GAMESYS_WORK *gsys = csw->gsys;

	switch(*seq) {
	case 0:
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, csw->fieldmap, 0));
		(*seq) ++;
		break;
	case 1:
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, csw->fieldmap));
		(*seq) ++;
		break;
	case 2:
		GAMESYSTEM_CallProc(gsys, csw->ov_id, csw->proc_data, csw->proc_work);
		(*seq) ++;
		break;
	case 3:
		if (GAMESYSTEM_IsProcExists(gsys)) break;
		(*seq) ++;
		break;
	case 4:
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case 5:
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, csw->fieldmap, 0));
		(*seq) ++;
		break;
	case 6:
		return GMEVENT_RES_FINISH;
		
	}
	return GMEVENT_RES_CONTINUE;
}

FS_EXTERN_OVERLAY(watanabe_sample);
extern const GFL_PROC_DATA TestProg1MainProcData;
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_FieldSample(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap)
{
#if 1
	GMEVENT * event = GMEVENT_Create(gsys, NULL, GameChangeEvent, sizeof(CHANGE_SAMPLE_WORK));
	CHANGE_SAMPLE_WORK * csw = GMEVENT_GetEventWork(event);
	csw->gsys = gsys;
	csw->fieldmap = fieldmap;
	csw->ov_id = FS_OVERLAY_ID(watanabe_sample);
	csw->proc_data = & TestProg1MainProcData;
	csw->proc_work = NULL;
#endif
	return event;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void AddOffsetByDirection(EXIT_DIR dir_id, VecFx32 * pos)
{
	switch (dir_id) {
	case EXIT_DIR_UP:	pos->z -= FX32_ONE * 16; break;
	case EXIT_DIR_DOWN:	pos->z += FX32_ONE * 16; break;
	case EXIT_DIR_LEFT:	pos->x -= FX32_ONE * 16; break;
	case EXIT_DIR_RIGHT:pos->x += FX32_ONE * 16; break;
	default:
		break;
	}
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static u16 GetDirValueByDirID(EXIT_DIR dir_id)
{
	switch (dir_id) {
	default:
	case EXIT_DIR_UP:	return 0x0000;
	case EXIT_DIR_LEFT:	return 0x4000;
	case EXIT_DIR_DOWN:	return 0x8000;
	case EXIT_DIR_RIGHT:return 0xc000;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void MakeNewLocation(const EVENTDATA_SYSTEM * evdata, const LOCATION * loc_req,
		LOCATION * loc_tmp)
{
	BOOL result;

	//���ڎw��̏ꍇ�̓R�s�[���邾��
	if (loc_req->type == LOCATION_TYPE_DIRECT) {
		*loc_tmp = *loc_req;
		return;
	}
	//�J�n�ʒu�Z�b�g
	result = EVENTDATA_SetLocationByExitID(evdata, loc_tmp, loc_req->exit_id);
	AddOffsetByDirection(loc_tmp->dir_id, &loc_tmp->pos);
	if (!result) {
		//�f�o�b�O�p�����F�{���͕s�v�Ȃ͂�
		OS_Printf("connect: debug default position\n");
		GF_ASSERT(0);
		LOCATION_DEBUG_SetDefaultPos(loc_tmp, loc_req->zone_id);
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void UpdateMapParams(GAMESYS_WORK * gsys, const LOCATION * loc_req)
{
	LOCATION loc;
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	PLAYER_WORK * mywork = GAMEDATA_GetMyPlayerWork(gamedata);
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);

	//�C�x���g�N���f�[�^�̓ǂݍ���
	EVENTDATA_SYS_Load(evdata, loc_req->zone_id);

	//�J�n�ʒu�Z�b�g
	MakeNewLocation(evdata, loc_req, &loc);

	//����ڑ��o�����ɏo���ꍇ�́A�O�̃}�b�v�̏o�����ʒu���L�����Ă���
	if (loc.type == LOCATION_TYPE_SPID) {
		//special_location = entrance_location;
		GAMEDATA_SetSpecialLocation(gamedata, GAMEDATA_GetEntranceLocation(gamedata));
	}

	//�擾����LOCATION�������@�֔��f
	{
		u16 direction;
		PLAYERWORK_setZoneID(mywork, loc.zone_id);
		PLAYERWORK_setPosition(mywork, &loc.pos);
		direction = GetDirValueByDirID(loc.dir_id);
		PLAYERWORK_setDirection(mywork, direction);
	}

	//�J�n�ʒu���L�����Ă���
	GAMEDATA_SetStartLocation(gamedata, &loc);
}



