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
#include "field/fldmmdl.h"

#include "./event_fieldmap_control.h"
#include "event_mapchange.h"

static void UpdateMapParams(GAMESYS_WORK * gsys, const LOCATION * loc_req);
static void CreateFldMMdl( FLDMMDLSYS *fldmmdlsys, u16 zone_id );

//============================================================================================
//
//	イベント：ゲーム開始
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
	switch(game_init_work->mode){
	case GAMEINIT_MODE_CONTINUE:
		CreateFldMMdl(GAMEDATA_GetFldMMdlSys(fmw->gamedata),0);
		LOCATION_SetDirect(&fmw->loc_req, game_init_work->mapid, game_init_work->dir, 
			game_init_work->pos.x, game_init_work->pos.y, game_init_work->pos.z);
		break;
	case GAMEINIT_MODE_FIRST:
		CreateFldMMdl(GAMEDATA_GetFldMMdlSys(fmw->gamedata),0);
		LOCATION_SetGameStart(&fmw->loc_req);
		break;
	case GAMEINIT_MODE_DEBUG:
		CreateFldMMdl(GAMEDATA_GetFldMMdlSys(fmw->gamedata),0);
		LOCATION_DEBUG_SetDefaultPos(&fmw->loc_req, game_init_work->mapid);
		break;
	}
	return event;
}

//============================================================================================
//
//		イベント：ゲーム終了
//
//============================================================================================
typedef struct {
	GAMESYS_WORK * gsys;
	FIELD_MAIN_WORK * fieldmap;
}GAME_END_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameEndEvent(GMEVENT * event, int *seq, void *work)
{
	GAME_END_WORK * gew = work;
	switch (*seq) {
	case 0:
		//フィールドマップをフェードアウト
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gew->gsys, gew->fieldmap, 0));
		(*seq)++;
		break;
	case 1:
		//フィールドマップを終了待ち
		GMEVENT_CallEvent(event, EVENT_FieldClose(gew->gsys, gew->fieldmap));
		(*seq)++;
		break;
	case 2:
		//プロセスを終了し、イベントも終了させるとゲームを終わる
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	デバッグ用：ゲーム終了
 */
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_GameEnd( GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap)
{
	GMEVENT * event = GMEVENT_Create(gsys, NULL, GameEndEvent, sizeof(GAME_END_WORK));
	GAME_END_WORK * gew = GMEVENT_GetEventWork(event);
	gew->gsys = gsys;
	gew->fieldmap = fieldmap;
	return event;
}

//============================================================================================
//
//	イベント：デバッグ用マップ切り替え
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
		//フィールドマップをフェードアウト
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, fieldmap, 0));
		(*seq)++;
		break;
	case 1:
		//フィールドマップを終了待ち
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
		(*seq)++;
		break;
	case 2:
		//新しいマップID、初期位置をセット
		UpdateMapParams(gsys, &mcw->loc_req);
		(*seq)++;
		break;
	case 3:
		//フィールドマップを開始待ち
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case 4:
		//フィールドマップをフェードイン
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
		u16 zone_id, const VecFx32 * pos, u16 dir )
{
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
	mcw->gsys = gsys;
	mcw->fieldmap = fieldmap;
	mcw->gamedata = GAMESYSTEM_GetGameData(gsys);
	
	LOCATION_SetDirect(&mcw->loc_req, zone_id, dir, pos->x, pos->y, pos->z);
	
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
//	※マップIDをインクリメントしている。最大値になったら先頭に戻る
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
//	※マップID指定型 イベント切り替え
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

	//直接指定の場合はコピーするだけ
	if (loc_req->type == LOCATION_TYPE_DIRECT) {
		*loc_tmp = *loc_req;
		return;
	}
	//開始位置セット
	result = EVENTDATA_SetLocationByExitID(evdata, loc_tmp, loc_req->exit_id);
	AddOffsetByDirection(loc_tmp->dir_id, &loc_tmp->pos);
	if (!result) {
		//デバッグ用処理：本来は不要なはず
		OS_Printf("connect: debug default position\n");
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

	//イベント起動データの読み込み
	EVENTDATA_SYS_Load(evdata, loc_req->zone_id);
	
	//開始位置セット
	MakeNewLocation(evdata, loc_req, &loc);

	//特殊接続出入口に出た場合は、前のマップの出入口位置を記憶しておく
	if (loc.type == LOCATION_TYPE_SPID) {
		//special_location = entrance_location;
		GAMEDATA_SetSpecialLocation(gamedata, GAMEDATA_GetEntranceLocation(gamedata));
	}

	//取得したLOCATION情報を自機へ反映
	{
		u16 direction;
		PLAYERWORK_setZoneID(mywork, loc.zone_id);
		PLAYERWORK_setPosition(mywork, &loc.pos);
		direction = GetDirValueByDirID(loc.dir_id);
		PLAYERWORK_setDirection(mywork, direction);
	}

	//開始位置を記憶しておく
	GAMEDATA_SetStartLocation(gamedata, &loc);
}

//--------------------------------------------------------------
//	test
//--------------------------------------------------------------
static void CreateFldMMdl( FLDMMDLSYS *fldmmdlsys, u16 zone_id )
{
	int i;
	u16 code[] = { BOY1,GIRL1,MAN1,WOMAN1,KABI32,OLDWOMAN1};
	FLDMMDL_HEADER head = {
			0,	///<識別ID
			0,	///<表示するOBJコード
			MV_RND,	///<動作コード
			0,	///<イベントタイプ
			0,	///<イベントフラグ
			0,	///<イベントID
			0,	///<指定方向
			0,	///<指定パラメタ 0
			0,	///<指定パラメタ 1
			0,	///<指定パラメタ 2
			4,	///<X方向移動制限
			4,	///<Z方向移動制限
			0,	///<グリッドX
			0,	///<グリッドZ
			0,	///<Y値 fx32型
		};
	
	for( i = 0; i < FLDMMDL_BUFFER_MAX-1; i++ ){
		head.id = i;
		head.gx = GFUser_GetPublicRand( 128 );
		head.gz = GFUser_GetPublicRand( 128 );
		head.obj_code = code[GFUser_GetPublicRand(NELEMS(code))];
		FLDMMDLSYS_AddFldMMdl( fldmmdlsys, &head, zone_id );
	}
}

