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
#include "gamesystem/iss_sys.h"

#include "system/main.h"

#include "field/zonedata.h"
#include "field/fieldmap.h"
#include "field/location.h"
#include "field/rail_location.h"  //RAIL_LOCATION
#include "field/eventdata_sxy.h"
#include "field/fldmmdl.h"

#include "event_fieldmap_control.h"
#include "event_mapchange.h"
#include "sound/pm_sndsys.h"		//�T�E���h�V�X�e���Q��

#include "event_fldmmdl_control.h"
#include "field_place_name.h"   //FIELD_PLACE_NAME_DisplayForce

#include "script.h"

#include "fieldmap/zone_id.h"   //��check�@���j�I����SubScreen�ݒ�b��Ώ��̈�

#include "field_sound.h"

#include "event_entrance_in.h"
#include "event_entrance_out.h"
#include "event_appear.h"
#include "event_disappear.h"
#include "field_bgm_control.h"

#include "savedata/gimmickwork.h"   //for GIMMICKWORK

static void UpdateMapParams(GAMESYS_WORK * gsys, const LOCATION * loc_req);
static void SetMMdl( GAMESYS_WORK *gsys, const LOCATION *loc_req, GAMEINIT_MODE mode );
static void setFirstBGM(GAMEDATA * gamedata, u16 zone_id);
static void AssignGimmickID(GAMEDATA * gamedata, int inZoneID);

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

typedef struct GMK_ASSIGN_DATA_tag
{
  u32 ZoneID;
  u16 ResNum;
  u16 ObjNum;
}GMK_ASSIGN_DATA;

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
		switch(game_init_work->mode){
		case GAMEINIT_MODE_FIRST:
		case GAMEINIT_MODE_DEBUG:
      SCRIPT_CallGameStartInitScript( gsys, GFL_HEAPID_APP );
			break;
		}

		UpdateMapParams(gsys, &fmw->loc_req);
		SetMMdl( gsys, &fmw->loc_req, game_init_work->mode );
		
		setFirstBGM(fmw->gamedata, fmw->loc_req.zone_id);
		
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
		fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
    FIELD_PLACE_NAME_DisplayForce(FIELDMAP_GetPlaceNameSys(fieldmap), fmw->loc_req.zone_id);
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
//	�C�x���g�F�}�b�v�؂�ւ�
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڏ����C�x���g�p���[�N
 */
//------------------------------------------------------------------
typedef struct {
	GAMESYS_WORK * gsys;
	GAMEDATA * gamedata;
	FIELD_MAIN_WORK * fieldmap;
  u16 before_zone_id;         ///<�J�ڑO�}�b�vID
	LOCATION loc_req;           ///<�J�ڐ�w��
  EXIT_TYPE exit_type;
}MAPCHANGE_WORK;

typedef MAPCHANGE_WORK* MAPCHANGE_WORK_PTR;


//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_MapChangeCore( GMEVENT* event, int* seq, void* work )
{
	MAPCHANGE_WORK*       mcw = *( (MAPCHANGE_WORK_PTR*)work );
	GAMESYS_WORK*        gsys = mcw->gsys;
	FIELD_MAIN_WORK* fieldmap = mcw->fieldmap;
	GAMEDATA*        gamedata = mcw->gamedata;

	switch( *seq )
  {
	case 0:
		//�t�B�[���h�}�b�v���I���҂�
		GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
		//�z�u���Ă������샂�f�����폜
		MMDLSYS_DeleteMMdl( GAMEDATA_GetMMdlSys(gamedata) );
		(*seq)++;
		break;
	case 1:
		//�V�����}�b�vID�A�����ʒu���Z�b�g
		UpdateMapParams(gsys, &mcw->loc_req);
		//�V�K�]�[���ɔz�u���铮�샂�f����ǉ�
		SetMMdl( gsys, &mcw->loc_req, GAMEINIT_MODE_FIRST );

	  //��check�@���j�I�����[���ւ̈ړ�����t�X�N���v�g�Ő��䂷��悤�ɂȂ�����T�u�X�N���[�����[�h��
	  //         �ύX�����̃X�N���v�g���ōs���悤�ɂ���
	  switch(mcw->loc_req.zone_id){
  	case ZONE_ID_UNION:
  	case ZONE_ID_CLOSSEUM:
  	case ZONE_ID_CLOSSEUM02:
  	  GAMEDATA_SetSubScreenMode(GAMESYSTEM_GetGameData(gsys), FIELD_SUBSCREEN_UNION);
  	  break;
  	default:
  	  GAMEDATA_SetSubScreenMode(GAMESYSTEM_GetGameData(gsys), FIELD_SUBSCREEN_NORMAL);
  	  break;
  	} 
		(*seq)++;
		break;
  case 2:
    // BGM�t�F�[�h�A�E�g�I���҂�
    if( FIELD_BGM_CONTROL_IsFade() != TRUE )
    { 
      FIELD_BGM_CONTROL_FadeIn( gamedata, mcw->loc_req.zone_id, 60 );
      (*seq)++;
    }
    break;
	case 3:
		//�t�B�[���h�}�b�v���J�n�҂�
		GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
  case 4:
		return GMEVENT_RES_FINISH; 
	}
	return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT* EVENT_MapChangeCore( MAPCHANGE_WORK* mcw )
{
  GMEVENT* event;
  MAPCHANGE_WORK_PTR* work;

  event = GMEVENT_Create( mcw->gsys, NULL, EVENT_FUNC_MapChangeCore, sizeof( MAPCHANGE_WORK_PTR ) );
  work  = GMEVENT_GetEventWork( event );
  *work = mcw;

  return event;
}

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
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    (*seq) ++;
    break;
	case 1:
    // �����i���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceIn( event, gsys, gamedata, fieldmap, mcw->loc_req, mcw->exit_type ) );
		(*seq)++;
		break;
  case 2:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw ) );
		(*seq)++;
    break;
	case 3:
    // �����ޏo�C�x���g
    GMEVENT_CallEvent( event, EVENT_EntranceOut( event, gsys, gamedata, fieldmap, mcw->loc_req ) );
		(*seq) ++;
		break;
  case 4:
		return GMEVENT_RES_FINISH; 
	}
	return GMEVENT_RES_CONTINUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByWarp(GMEVENT * event, int *seq, void*work)
{
	MAPCHANGE_WORK * mcw = work;
	GAMESYS_WORK  * gsys = mcw->gsys;
	FIELD_MAIN_WORK * fieldmap = mcw->fieldmap;
	GAMEDATA * gamedata = mcw->gamedata;
	switch (*seq) {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    (*seq) ++;
    break;
	case 1:
    // ���[�v�ޏ�C�x���g
    if( FIELDMAP_GetMapControlType( fieldmap ) != FLDMAP_CTRLTYPE_NOGRID )
    {
      GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Warp( event, gsys, fieldmap ) );
    }
    //GMEVENT_CallEvent( event, EVENT_DISAPPEAR_FallInSand( event, gsys, fieldmap ) );  // TEST:
		(*seq)++;
		break;
  case 2:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw ) );
		(*seq)++;
    break;
	case 3:
    // ���[�v�o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Warp( event, gsys, fieldmap ) );
    //GMEVENT_CallEvent( event, EVENT_APPEAR_Fall( event, gsys, fieldmap ) );   // TEST:
		(*seq) ++;
		break;
  case 4:
		return GMEVENT_RES_FINISH; 
	}
	return GMEVENT_RES_CONTINUE;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void MAPCHANGE_WORK_init(MAPCHANGE_WORK * mcw, GAMESYS_WORK * gsys)
{
  mcw->gsys = gsys;
  mcw->gamedata = GAMESYSTEM_GetGameData( gsys );
  mcw->fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  mcw->before_zone_id = FIELDMAP_GetZoneID( mcw->fieldmap );
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
  MAPCHANGE_WORK_init( mcw, gsys );
	
	LOCATION_SetDirect(&mcw->loc_req, zone_id, dir, pos->x, pos->y, pos->z);
  mcw->exit_type = EXIT_TYPE_NONE;
	
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
  MAPCHANGE_WORK_init( mcw, gsys );

	LOCATION_DEBUG_SetDefaultPos(&mcw->loc_req, zone_id);
  mcw->exit_type = EXIT_TYPE_NONE;
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_ChangeMapDefaultPosByWarp(GAMESYS_WORK * gsys,
		FIELD_MAIN_WORK * fieldmap, u16 zone_id)
{
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeByWarp, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys );

	LOCATION_DEBUG_SetDefaultPos(&mcw->loc_req, zone_id);
  mcw->exit_type = EXIT_TYPE_NONE;
	return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapByConnect(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    const CONNECT_DATA * cnct)
{
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	MAPCHANGE_WORK * mcw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
	mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys );

	if (CONNECTDATA_IsSpecialExit(cnct))
  {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
    mcw->loc_req = *sp;
	}
  else
  {
		CONNECTDATA_SetNextLocation(cnct, &mcw->loc_req);
	}
  mcw->exit_type = CONNECTDATA_GetExitType(cnct);
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
#if 0
  {
    const CONNECT_DATA * cnct = EVENTDATA_GetConnectByID(evdata, loc_req->exit_id);
    if (CONNECTDATA_GetExitType(cnct) != EXIT_TYPE_MAT)
    {
	    AddOffsetByDirection(loc_tmp->dir_id, &loc_tmp->pos);
    }
  }
#endif
	if (!result) {
		//�f�o�b�O�p�����F�{���͕s�v�Ȃ͂�
		OS_Printf("connect: debug default position\n");
		LOCATION_DEBUG_SetDefaultPos(loc_tmp, loc_req->zone_id);
	}
}
extern u32 FIELD_RAIL_LOADER_GetNearestPoint( const FIELD_RAIL_LOADER* cp_sys, const VecFx32 * pos);
//------------------------------------------------------------------
//------------------------------------------------------------------
static void MakeNewRailLocation(GAMEDATA * gamedata, EVENTDATA_SYSTEM * evdata, const LOCATION * loc_req)
{
  FIELD_RAIL_LOADER * railLoader = GAMEDATA_GetFieldRailLoader(gamedata);
  RAIL_LOCATION railLoc;
  RAIL_LOCATION_Init(&railLoc);

  if (ZONEDATA_IsRailMap(loc_req->zone_id) == TRUE
      && loc_req->type != LOCATION_TYPE_DIRECT)
  {
    LOCATION loc_tmp;
    BOOL result;
    u32 exit_id = 0;

    FIELD_RAIL_LOADER_Load( railLoader, ZONEDATA_GetRailDataID(loc_req->zone_id), GFL_HEAPID_APP );
    result = EVENTDATA_SetLocationByExitID(evdata, &loc_tmp, loc_req->exit_id);
    if (result)
    {
      OS_Printf("No Grid Exit:(%d,%d,%d)\n",
          FX_Whole(loc_tmp.pos.x), FX_Whole(loc_tmp.pos.y), FX_Whole(loc_tmp.pos.z) );
      exit_id = FIELD_RAIL_LOADER_GetNearestPoint(railLoader, &loc_tmp.pos);
    }
    railLoc.type = FIELD_RAIL_TYPE_POINT;
    railLoc.rail_index = exit_id;
    railLoc.line_grid = 0;
    railLoc.width_grid = 0;
    railLoc.key = RAIL_KEY_NULL;
    FIELD_RAIL_LOADER_Clear( railLoader );
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
    ZONEDATA_DEBUG_GetZoneName(buf, loc_req->zone_id);
    TAMADA_Printf("UpdateMapParams:%s\n", buf);
  }
	//�C�x���g�N���f�[�^�̓ǂݍ���
	EVENTDATA_SYS_Load(evdata, loc_req->zone_id);
	
	//�J�n�ʒu�Z�b�g
	MakeNewLocation(evdata, loc_req, &loc);
  MakeNewRailLocation(gamedata, evdata, loc_req);

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

	// ISS�Ƀ]�[���؂�ւ���ʒm
	{
		ISS_SYS* iss = GAMESYSTEM_GetIssSystem( gsys );
		ISS_SYS_ZoneChange( iss, loc.zone_id );
	}
	
	//�J�n�ʒu���L�����Ă���
	GAMEDATA_SetStartLocation(gamedata, &loc);

  //�M�~�b�N�A�T�C��
  AssignGimmickID(gamedata, loc.zone_id);
}

//--------------------------------------------------------------
//	test
//--------------------------------------------------------------
static void SetMMdl( GAMESYS_WORK *gsys, const LOCATION *loc_req, GAMEINIT_MODE mode )
{
	if(	mode == GAMEINIT_MODE_FIRST || mode == GAMEINIT_MODE_DEBUG ){
		GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
		EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
		u16 count = EVENTDATA_GetNpcCount( evdata );
		
		if( count ){
      EVENTWORK *evwork =  GAMEDATA_GetEventWork( gamedata );
			MMDLSYS *fmmdlsys = GAMEDATA_GetMMdlSys( gamedata );
			const MMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
			MMDLSYS_SetMMdl( fmmdlsys, header, loc_req->zone_id, count, evwork );
		}
	}
}

//--------------------------------------------------------------
static void setFirstBGM(GAMEDATA * gamedata, u16 zone_id)
{
  PLAYER_WORK *player = GAMEDATA_GetPlayerWork( gamedata, 0 );
  PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
  u32 no = FIELD_SOUND_GetFieldBGMNo( gamedata, form, zone_id );
  OS_Printf("NEXT BGM NO=%d\n",no);
  FIELD_SOUND_PlayBGM( no );
}
//============================================================================================
//============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
void MAPCHANGE_setPlayerVanish(FIELDMAP_WORK * fieldmap, BOOL vanish_flag)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
  MMDL *fmmdl = FIELD_PLAYER_GetMMdl( player );
  MMDL_SetStatusBitVanish( fmmdl, vanish_flag );
}

//---------------------------------------------------------------------------
/**
 * @brief	�M�~�b�N�̃A�T�C��
 * 
 * @param	  inZoneID    �]�[���h�c
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
static void AssignGimmickID(GAMEDATA * gamedata, int inZoneID)
{
  GIMMICKWORK *work;
  SAVE_CONTROL_WORK* sv= GAMEDATA_GetSaveControlWork(gamedata);
  //�M�~�b�N���[�N�擾
	work = SaveData_GetGimmickWork(sv);

  //�}�b�v�W�����v�̂Ƃ��̂ݏ���������i�����ă]�[�����؂�ւ�����ꍇ�͏��������Ȃ��j
	GIMMICKWORK_Init(work);

  //�M�~�b�N�f�[�^����
  {
    u32 num, i;
    ARCHANDLE *handle;
    GMK_ASSIGN_DATA *data;
    handle = GFL_ARC_OpenDataHandle( ARCID_GIMMICK_ASSIGN, GFL_HEAP_LOWID(GFL_HEAPID_APP) );
    data = (GMK_ASSIGN_DATA *)GFL_ARC_LoadDataAllocByHandle( handle, 0, GFL_HEAP_LOWID(GFL_HEAPID_APP) );

    num = GFL_ARC_GetDataSizeByHandle( handle, 0 ) / sizeof(GMK_ASSIGN_DATA);

//    OS_Printf("gimmick_num = %d\n",num);

    for (i=0;i<num;i++){
      if ( data[i].ZoneID == inZoneID){
        //�M�~�b�N�����B�A�T�C������
        GIMMICKWORK_Assign(work, i+1);
        break;
      }
    }

    GFL_HEAP_FreeMemory( data );
    GFL_ARC_CloseDataHandle( handle );
  }
}

