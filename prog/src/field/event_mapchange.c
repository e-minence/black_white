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
#include "field/rail_location.h"  //RAIL_LOCATION
#include "field/eventdata_sxy.h"
#include "field/fldmmdl.h"

#include "./event_fieldmap_control.h"
#include "event_mapchange.h"
#include "sound/pm_sndsys.h"		//�T�E���h�V�X�e���Q��

static void UpdateMapParams(GAMESYS_WORK * gsys, const LOCATION * loc_req);
static void SetFldMMdl( GAMESYS_WORK *gsys, const LOCATION *loc_req, GAMEINIT_MODE mode );

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
		SetFldMMdl( gsys, &fmw->loc_req, game_init_work->mode );
		
		{
			//���}���B��Ƀt�F�[�h�C���Ŏn�܂�
			u16 trackBit = 0xfcff;	// track 9,10 OFF
			u16 nextBGM = ZONEDATA_GetBGMID(fmw->loc_req.zone_id,
					GAMEDATA_GetSeasonID(fmw->gamedata));
			PMSND_PlayNextBGM_EX(nextBGM, trackBit, 30, 0);
		}
		
		switch(game_init_work->mode){
		case GAMEINIT_MODE_FIRST:
		case GAMEINIT_MODE_DEBUG:
			break;
		}
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
		LOCATION_SetDirect(&fmw->loc_req, game_init_work->mapid, game_init_work->dir, 
			game_init_work->pos.x, game_init_work->pos.y, game_init_work->pos.z);
		break;
	case GAMEINIT_MODE_FIRST:
		LOCATION_SetGameStart(&fmw->loc_req);
		break;
	case GAMEINIT_MODE_DEBUG:
		LOCATION_DEBUG_SetDefaultPos(&fmw->loc_req, game_init_work->mapid);
		break;
	}
	return event;
}

//============================================================================================
//
//		�C�x���g�F�Q�[���I��
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
		//�t�B�[���h�}�b�v���t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gew->gsys, gew->fieldmap, 0));
		//�ʐM�������Ă���ꍇ�͏I��������
		if(GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gew->gsys)) != GAME_COMM_NO_NULL){
      GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gew->gsys));
    }
		(*seq)++;
		break;
	case 1:
	  //�ʐM�I���҂�
	  if(GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gew->gsys)) != GAME_COMM_NO_NULL){
      break;
    }
		//�t�B�[���h�}�b�v���I���҂�
		GMEVENT_CallEvent(event, EVENT_FieldClose(gew->gsys, gew->fieldmap));
		(*seq)++;
		break;
	case 2:
		//�v���Z�X���I�����A�C�x���g���I��������ƃQ�[�����I���
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p�F�Q�[���I��
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
		{
			//���}���B��Ƀt�F�[�h�C���Ŏn�܂�
			u16 trackBit = 0xfcff;	// track 9,10 OFF
			u16 nextBGM = ZONEDATA_GetBGMID(mcw->loc_req.zone_id,
					GAMEDATA_GetSeasonID(gamedata));
			PMSND_PlayNextBGM_EX(nextBGM, trackBit, 30, 0);
		}
		//�t�B�[���h�}�b�v���t�F�[�h�A�E�g
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, fieldmap, 0));
		(*seq)++;
		break;
	case 1:
		//�t�B�[���h�}�b�v���I���҂�
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
		//�z�u���Ă������샂�f�����폜
		FLDMMDLSYS_DeleteMMdl( GAMEDATA_GetFldMMdlSys(gamedata) );
		(*seq)++;
		break;
	case 2:
		//�V�����}�b�vID�A�����ʒu���Z�b�g
		UpdateMapParams(gsys, &mcw->loc_req);
		//�V�K�]�[���ɔz�u���铮�샂�f����ǉ�
		SetFldMMdl( gsys, &mcw->loc_req, GAMEINIT_MODE_FIRST );
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
GMEVENT * DEBUG_EVENT_ChangeMapDefaultPos(GAMESYS_WORK * gsys,
		FIELD_MAIN_WORK * fieldmap, u16 zone_id)
{
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
	mcw->gsys = gsys;
	mcw->fieldmap = fieldmap;
	mcw->gamedata = GAMESYSTEM_GetGameData(gsys);
	LOCATION_DEBUG_SetDefaultPos(&mcw->loc_req, zone_id);
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
	return DEBUG_EVENT_ChangeMapDefaultPos(gsys, fieldmap, next);
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
	GMEVENT_ChangeEvent(event, DEBUG_EVENT_ChangeMapDefaultPos(gsys, fieldmap, zone_id));
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
		LOCATION_DEBUG_SetDefaultPos(loc_tmp, loc_req->zone_id);
	}
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void MakeNewRailLocation(GAMEDATA * gamedata, const LOCATION * loc_req)
{
  RAIL_LOCATION railLoc;
  RAIL_LOCATION_Init(&railLoc);

  if (ZONEDATA_GetMapRscID(loc_req->zone_id) != 1
      && loc_req->type == LOCATION_TYPE_DIRECT)
  {
    railLoc.type = FIELD_RAIL_TYPE_POINT;
    railLoc.rail_index = loc_req->exit_id;
    railLoc.line_ofs = 0;
    railLoc.width_ofs = 0;
    railLoc.key = RAIL_KEY_NULL;
  }
  GAMEDATA_SetRailLocation(gamedata, &railLoc);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void UpdateMapParams(GAMESYS_WORK * gsys, const LOCATION * loc_req)
{
	LOCATION loc;
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	PLAYER_WORK * mywork = GAMEDATA_GetMyPlayerWork(gamedata);
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);

  { 
    char buf[ZONEDATA_NAME_LENGTH*2];
    ZONEDATA_GetZoneName(0, buf, loc_req->zone_id);
    TAMADA_Printf("UpdateMapParams:%s\n", buf);
  }
	//�C�x���g�N���f�[�^�̓ǂݍ���
	EVENTDATA_SYS_Load(evdata, loc_req->zone_id);
	
	//�J�n�ʒu�Z�b�g
	MakeNewLocation(evdata, loc_req, &loc);
  MakeNewRailLocation(gamedata, loc_req);

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

//--------------------------------------------------------------
//	test
//--------------------------------------------------------------
static void SetFldMMdl( GAMESYS_WORK *gsys, const LOCATION *loc_req, GAMEINIT_MODE mode )
{
	if(	mode == GAMEINIT_MODE_FIRST || mode == GAMEINIT_MODE_DEBUG ){
		GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
		EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
		u16 count = EVENTDATA_GetNpcCount( evdata );
		
		if( count ){
			FLDMMDLSYS *fmmdlsys = GAMEDATA_GetFldMMdlSys( gamedata );
			const FLDMMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
			FLDMMDLSYS_SetFldMMdl( fmmdlsys, header, loc_req->zone_id, count );
		}
	}
}
