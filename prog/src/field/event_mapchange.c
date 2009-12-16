//============================================================================================
/**
 * @file  event_mapchange.c
 * @brief �}�b�v�J�ڊ֘A�C�x���g
 * @date  2008.11.04
 * @author  tamada GAME FREAK inc.
 *
 * @todo  FIELD_STATUS_SetFieldInitFlag���ǂ����ɋ@�\�Ƃ��Ă܂Ƃ߂�ׂ�������
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
#include "field/fieldskill_mapeff.h"
#include "field/field_status_local.h" //FIELD_STATUS

#include "event_fieldmap_control.h"
#include "event_mapchange.h"
#include "sound/pm_sndsys.h"    //�T�E���h�V�X�e���Q��

#include "event_fldmmdl_control.h"
#include "field_place_name.h"   //FIELD_PLACE_NAME_DisplayForce

#include "script.h"

#include "fieldmap/zone_id.h"   //��check�@���j�I����SubScreen�ݒ�b��Ώ��̈�

#include "warpdata.h"
#include "move_pokemon.h"
#include "field_sound.h"

#include "event_entrance_in.h"
#include "event_entrance_out.h"
#include "event_appear.h"
#include "event_disappear.h"

#include "field/field_flagcontrol.h" //FIELD_FLAGCONT_INIT�`

#include "savedata/gimmickwork.h"   //for GIMMICKWORK

#include "net_app/union/union_main.h" // for UNION_CommBoot

#include "savedata/gametime.h"  // for GMTIME
#include "gamesystem/pm_season.h"  // for PMSEASON_TOTAL
#include "ev_time.h"  //EVTIME_Update
#include "../../../resource/fldmapdata/flagwork/flag_define.h"  //SYS_FLAG_SPEXIT_REQUEST

#ifdef PM_DEBUG
#include "../gamesystem/debug_data.h"
FS_EXTERN_OVERLAY(debug_data);
#endif

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void MAPCHG_setupMapTools( GAMESYS_WORK * gsys, const LOCATION * loc_req );
static void MAPCHG_releaseMapTools( GAMESYS_WORK * gsys );

static void MAPCHG_updateGameData( GAMESYS_WORK * gsys, const LOCATION * loc_req );

static void MAPCHG_loadMMdl( GAMEDATA * gamedata, const LOCATION *loc_req );
static void MAPCHG_loadMMdlWFBC( GAMEDATA * gamedata, const LOCATION *loc );
static void MAPCHG_releaseMMdl( GAMEDATA * gamedata );

static void MAPCHG_SetUpWfbc( GAMEDATA * gamedata, const LOCATION *loc );


static void MAPCHG_setupFieldSkillMapEff( GAMEDATA * gamedata, const LOCATION *loc_req );

static void setFirstBGM(GAMEDATA * gamedata, u16 zone_id);
static void AssignGimmickID(GAMEDATA * gamedata, int inZoneID);

static GMEVENT_RESULT EVENT_MapChangeNoFade(GMEVENT * event, int *seq, void*work);


//============================================================================================
//
//  �C�x���g�F�Q�[���J�n
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  GAMEINIT_MODE game_init_mode;
  //const GAME_INIT_WORK * game_init_work;
  LOCATION loc_req;
}FIRST_MAPIN_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FirstMapIn(GMEVENT * event, int *seq, void *work)
{
  FIRST_MAPIN_WORK * fmw = work;
  GAMESYS_WORK * gsys = fmw->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  FIELDMAP_WORK * fieldmap;
  switch (*seq) {
  case 0:
    // VRAM�S�N���A
    GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
    MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
    (void)GX_DisableBankForLCDC();

    switch(fmw->game_init_mode){
    case GAMEINIT_MODE_FIRST:
      SCRIPT_CallGameStartInitScript( gsys, GFL_HEAPID_APP );
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
      //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
      MAPCHG_setupMapTools( gsys, &fmw->loc_req );
      //�V�����}�b�vID�A�����ʒu���Z�b�g
      MAPCHG_updateGameData( gsys, &fmw->loc_req );
      break;

    case GAMEINIT_MODE_DEBUG:
      SCRIPT_CallDebugGameStartInitScript( gsys, GFL_HEAPID_APP );
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
      //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
      MAPCHG_setupMapTools( gsys, &fmw->loc_req );
      //�V�����}�b�vID�A�����ʒu���Z�b�g
      MAPCHG_updateGameData( gsys, &fmw->loc_req );
      break;

    case GAMEINIT_MODE_CONTINUE:
      if (EVENTWORK_CheckEventFlag( GAMEDATA_GetEventWork(gamedata), SYS_FLAG_SPEXIT_REQUEST ) )
      { //����ڑ����N�G�X�g�F
        const LOCATION * spLoc = GAMEDATA_GetSpecialLocation( gamedata );
        fmw->loc_req = *spLoc;
        EVENTWORK_ResetEventFlag( GAMEDATA_GetEventWork(gamedata), SYS_FLAG_SPEXIT_REQUEST );
        FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
        MAPCHG_releaseMapTools( gsys );
        //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
        MAPCHG_setupMapTools( gsys, &fmw->loc_req );
        //�V�����}�b�vID�A�����ʒu���Z�b�g
        MAPCHG_updateGameData( gsys, &fmw->loc_req );
      }
      else
      {
        //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
        MAPCHG_setupMapTools( gsys, &fmw->loc_req );
        //�C�x���g���ԍX�V
        EVTIME_Update( gamedata );
        //�R���e�B�j���[�ɂ��t���O���Ƃ�����
        FIELD_FLAGCONT_INIT_Continue( gamedata, fmw->loc_req.zone_id );
      }
      break;

    default:
      GF_ASSERT(0);
    }
    
    setFirstBGM(fmw->gamedata, fmw->loc_req.zone_id);
    
    (*seq)++;
    break;
  case 1:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq)++;
    break;
  case 2:
    fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
    { // �G�ߕ\���̏����ݒ�
      BOOL disp = FALSE;
      AREADATA* areadata = FIELDMAP_GetAreaData( fieldmap );
      BOOL outdoor = ( AREADATA_GetInnerOuterSwitch(areadata) != 0 );
      u8 season = GAMEDATA_GetSeasonID( gamedata );
      FIELD_STATUS* fstatus = GAMEDATA_GetFieldStatus( gamedata );
      if( fmw->game_init_mode == GAMEINIT_MODE_DEBUG )
      { // �f�o�b�O�X�^�[�g ==> �\���Ȃ�
        FIELD_STATUS_SetSeasonDispFlag( fstatus, FALSE );
        FIELD_STATUS_SetSeasonDispLast( fstatus, season );
      }
      else if( (fmw->game_init_mode == GAMEINIT_MODE_CONTINUE) && (outdoor != TRUE) )
      { // �u�Â�����v�ŉ����ɂ���ꍇ ==> �\���Ȃ�
        FIELD_STATUS_SetSeasonDispFlag( fstatus, FALSE );
        FIELD_STATUS_SetSeasonDispLast( fstatus, season );
      }
      else
      { // ����ȊO ==> �J�n���̋G�߂�\��
        season = (season + PMSEASON_TOTAL - 1) % PMSEASON_TOTAL;
        FIELD_STATUS_SetSeasonDispFlag( fstatus, TRUE );
        FIELD_STATUS_SetSeasonDispLast( fstatus, season );
      }
    }
    (*seq) ++;
    break;
  case 3:
    {
      GMEVENT* fade_event;
      FIELD_STATUS* fstatus;
      fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
      fstatus = GAMEDATA_GetFieldStatus( gamedata );
      if( FIELD_STATUS_GetSeasonDispFlag(fstatus) )
      {
        fade_event = EVENT_FieldFadeIn_Season(gsys, fieldmap);
      }
      else
      { 
        fade_event = EVENT_FieldFadeIn_Black(gsys, fieldmap, FIELD_FADE_WAIT);
      }
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case 4:
    fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
    if(FIELDMAP_GetPlaceNameSys(fieldmap)){
      FIELD_PLACE_NAME_DisplayForce(FIELDMAP_GetPlaceNameSys(fieldmap), fmw->loc_req.zone_id);
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n����
 *
 * @todo
 * �R���e�B�j���[����������Ő؂蕪���Ă���̂�
 * ������Ɗ֐���������K�v������
 */
//------------------------------------------------------------------
GMEVENT * EVENT_GameStart(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work)
{
  FIRST_MAPIN_WORK * fmw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, EVENT_FirstMapIn, sizeof(FIRST_MAPIN_WORK));
  fmw = GMEVENT_GetEventWork(event);
  fmw->gsys = gsys;
  fmw->gamedata = GAMESYSTEM_GetGameData(gsys);
  //fmw->game_init_work = game_init_work;
  fmw->game_init_mode = game_init_work->mode;

  switch(game_init_work->mode){
  case GAMEINIT_MODE_CONTINUE:
    LOCATION_SetDirect(&fmw->loc_req, game_init_work->mapid, game_init_work->dir, 
      game_init_work->pos.x, game_init_work->pos.y, game_init_work->pos.z);
    break;
  case GAMEINIT_MODE_FIRST:
    LOCATION_SetGameStart(&fmw->loc_req);
    break;
#ifdef PM_DEBUG
  case GAMEINIT_MODE_DEBUG:
    GFL_OVERLAY_Load( FS_OVERLAY_ID(debug_data));
    //�K���Ɏ莝���|�P������Add
    DEBUG_MyPokeAdd( GAMESYSTEM_GetGameData(gsys), GFL_HEAPID_APP );
    //�f�o�b�O�A�C�e���ǉ�
    DEBUG_MYITEM_MakeBag( GAMESYSTEM_GetGameData(gsys), GFL_HEAPID_APP );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(debug_data));
    
    LOCATION_DEBUG_SetDefaultPos(&fmw->loc_req, game_init_work->mapid);
    break;
#endif //PM_DEBUG
  }
  return event;
}

//============================================================================================
//
//    �C�x���g�F�Q�[���I��
//
//============================================================================================
typedef struct {
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
}GAME_END_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT GameEndEvent(GMEVENT * event, int *seq, void *work)
{
  GAME_END_WORK * gew = work;
  switch (*seq) {
  case 0:
    //�t�B�[���h�}�b�v���t�F�[�h�A�E�g
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gew->gsys, gew->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
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
 * @brief �f�o�b�O�p�F�Q�[���I��
 *
 * @note
 * ���ۂɂ̓Q�[�����I��������Ƃ����C���^�[�t�F�C�X��
 * �|�P�����ɑ��݂��Ȃ��̂ŁA���̊֐��͂����܂Ńf�o�b�O�p�B
 */
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_GameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, GameEndEvent, sizeof(GAME_END_WORK));
  GAME_END_WORK * gew = GMEVENT_GetEventWork(event);
  gew->gsys = gsys;
  gew->fieldmap = fieldmap;
  return event;
}

//============================================================================================
//
//  �C�x���g�F�}�b�v�؂�ւ�
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
  FIELDMAP_WORK * fieldmap;
  LOCATION loc_req;           ///<�J�ڐ�w��
  EXIT_TYPE exit_type;
  u16 before_zone_id;         ///<�J�ڑO�}�b�vID
  u8  next_season;  ///<�J�ڌ�̋G��
  u8  mapchange_type; ///<�`�F���W�^�C�v EV_MAPCHG_TYPE
  VecFx32 stream_pos;  ///<�����J�ڎ��ɂ̂ݎg�p
}MAPCHANGE_WORK;

typedef MAPCHANGE_WORK* MAPCHANGE_WORK_PTR;

#define INVALID_SEASON_ID (0xff)  ///<�G��ID�����l

typedef enum{
  EV_MAPCHG_NORMAL,
  EV_MAPCHG_FLYSKY,
  EV_MAPCHG_ESCAPE,
  EV_MAPCHG_TELEPORT,
}EV_MAPCHG_TYPE;

//------------------------------------------------------------------
/**
 * @brief �G�߂̕ω����`�F�b�N����
 */
//------------------------------------------------------------------
static void CheckSeasonChange( MAPCHANGE_WORK* work )
{
  RTCDate date_start, date_now;
  RTCTime time_start;
  GMTIME* gmtime;
  SAVE_CONTROL_WORK* scw;
  BOOL outdoor;
  u8 last_season;
  FIELD_STATUS* fstatus = GAMEDATA_GetFieldStatus( work->gamedata );

  // �Q�[���J�n�������擾
  scw    = GAMEDATA_GetSaveControlWork( work->gamedata );
  gmtime = SaveData_GetGameTime( scw );
  RTC_ConvertSecondToDateTime( &date_start, &time_start, gmtime->start_sec );
  // ���ݎ����Ƃ��̋G�߂����߂�
  GFL_RTC_GetDate( &date_now );
  work->next_season = (date_now.month + 12 - date_start.month) % PMSEASON_TOTAL;
  // �Ō�ɕ\�������G�߂��擾
  last_season = FIELD_STATUS_GetSeasonDispLast( fstatus );
  // �J�ڐ悪���������O���𔻒�
  {
    HEAPID heap_id;
    u16 area_id;
    AREADATA* areadata;
    heap_id  = FIELDMAP_GetHeapID( work->fieldmap );
    area_id  = ZONEDATA_GetAreaID( work->loc_req.zone_id );
    areadata = AREADATA_Create( heap_id, area_id, work->next_season );
    outdoor  = ( AREADATA_GetInnerOuterSwitch(areadata) != 0 );
    AREADATA_Delete( areadata );
  }
  // �G�ߕ\���̗L��������
  if( (work->next_season != last_season) && (outdoor != FALSE) )  // if(�G�ߕω�&&�J�ڐ悪���O)
  {
    FIELD_STATUS_SetSeasonDispFlag( fstatus, TRUE );
  }
  else
  {
    FIELD_STATUS_SetSeasonDispFlag( fstatus, FALSE );
  }
}
//------------------------------------------------------------------
/**
 * @brief �G�߂��X�V����
 */
//------------------------------------------------------------------
static void UpdateSeason( MAPCHANGE_WORK* work )
{
  // �J�ڐ�̋G�߂����߂��Ă��Ȃ�
  if( work->next_season == INVALID_SEASON_ID )
  {
    OBATA_Printf( "==================================\n" );
    OBATA_Printf( "�J�ڐ�̋G�߂����߂��Ă��܂���I\n" );
    OBATA_Printf( "==================================\n" );
    return;
  }
  // �Q�[���f�[�^�X�V
  GAMEDATA_SetSeasonID( work->gamedata, work->next_season );

  // DEBUG:
  OBATA_Printf( "change season: ==> %d\n", work->next_season );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_MapChangeCore( GMEVENT* event, int* seq, void* work )
{
  MAPCHANGE_WORK*     mcw = *( (MAPCHANGE_WORK_PTR*)work );
  GAMESYS_WORK*      gsys = mcw->gsys;
  FIELDMAP_WORK* fieldmap = mcw->fieldmap;
  GAMEDATA*      gamedata = mcw->gamedata;

  switch( *seq )
  {
  case 0:
    //�t�B�[���h�}�b�v���I���҂�
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
    (*seq)++;
    break;
  case 1:
    //�}�b�v���[�h�Ȃǋ@�\�w�����������
    MAPCHG_releaseMapTools( gsys );

    { 
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
    }

    // �G�߂̍X�V
    UpdateSeason( mcw );

    //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
    MAPCHG_setupMapTools( gsys, &mcw->loc_req );
    
    //�V�����}�b�vID�A�����ʒu���Z�b�g
    MAPCHG_updateGameData( gsys, &mcw->loc_req );

    //�^�C�v�ɉ������t���O������
    switch(mcw->mapchange_type){
    case EV_MAPCHG_FLYSKY:
      FIELD_FLAGCONT_INIT_FlySky( gamedata, mcw->loc_req.zone_id );
      break;
    case EV_MAPCHG_ESCAPE:
      FIELD_FLAGCONT_INIT_Escape( gamedata, mcw->loc_req.zone_id );
      break;
    case EV_MAPCHG_TELEPORT:
      FIELD_FLAGCONT_INIT_Teleport( gamedata, mcw->loc_req.zone_id );
      break;
    }
    (*seq)++;
    break;
  case 2:
    //�t�B�[���h�}�b�v���J�n�҂�
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case 3:
    // BGM�t�F�[�h�A�E�g�I���҂�
    {
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gamedata );
      if( FIELD_SOUND_IsBGMFade(fsnd) != TRUE )
      { 
        PLAYER_WORK* player = GAMEDATA_GetPlayerWork( gamedata, 0 );
        PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
        // @todo BGM�ύX���N�G�X�g�𓊂���^�C�~���O�����[��������B
        // ���݁A���ꂼ��̃}�b�v�`�F���W�C�x���g����BGM�ύX���N�G�X�g�𓊂��Ă��邪�A
        // �����Y��Ă���ꍇ�̕ی��Ƃ��āACore���ł����N�G�X�g���o���B
        // ����āA1�x�̃}�b�v�`�F���W����2��Ă΂�邱�Ƃ�����B
        // ==>����BGM�̃��N�G�X�g�Ȃ���Ȃ��B
        FIELD_SOUND_ChangePlayZoneBGM( fsnd, gamedata, form, mcw->loc_req.zone_id );
        (*seq)++;
      }
    }
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT* EVENT_MapChangeCore( MAPCHANGE_WORK* mcw, EV_MAPCHG_TYPE type )
{
  GMEVENT* event;
  MAPCHANGE_WORK_PTR* work;

  event = GMEVENT_Create( mcw->gsys, NULL, EVENT_FUNC_MapChangeCore, sizeof( MAPCHANGE_WORK_PTR ) );
  work  = GMEVENT_GetEventWork( event );
  *work = mcw;
  mcw->mapchange_type = type;
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChange(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK * mcw = work;
  GAMESYS_WORK  * gsys = mcw->gsys;
  FIELDMAP_WORK * fieldmap = mcw->fieldmap;
  GAMEDATA * gamedata = mcw->gamedata;
  switch (*seq) {
  case 0:
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    // ���샂�f���̈ړ����~�߂�
    {
      MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_PauseMoveProc( fmmdlsys );
    }
    // �����i���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceIn( event, gsys, gamedata, fieldmap, mcw->loc_req, mcw->exit_type ) );
    (*seq)++;
    break;
  case 1:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    // �����ޏo�C�x���g
    GMEVENT_CallEvent( event, EVENT_EntranceOut( event, gsys, gamedata, fieldmap, mcw->loc_req ) );
    (*seq) ++;
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT DEBUG_EVENT_QuickMapChange(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK * mcw = work;
  GAMESYS_WORK  * gsys = mcw->gsys;
  FIELDMAP_WORK * fieldmap = mcw->fieldmap;
  GAMEDATA * gamedata = mcw->gamedata;
  switch (*seq) {
  case 0:
    GMEVENT_CallEvent( event, DEBUG_EVENT_QuickFadeOut( gsys, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent( event, DEBUG_EVENT_QuickFadeIn( gsys, fieldmap ) );
    (*seq) ++;
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeNoFade(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK * mcw = work;
  GAMESYS_WORK  * gsys = mcw->gsys;
  FIELDMAP_WORK * fieldmap = mcw->fieldmap;
  GAMEDATA * gamedata = mcw->gamedata;
  switch (*seq) {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    (*seq) ++;
    break;
  case 1:
    { // BGM�X�V���N�G�X�g
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gamedata );
      PLAYER_WORK* player = GAMEDATA_GetPlayerWork( gamedata, 0 );
      PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
      FIELD_SOUND_ChangePlayZoneBGM( fsnd, gamedata, form, mcw->loc_req.zone_id );
    }
    (*seq)++;
    break;
  case 2:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, mcw->mapchange_type ) );
    (*seq)++;
    break;
  case 3:
#if 0   //�n���\���̓��N�G�X�g���Ȃ�
    if(FIELDMAP_GetPlaceNameSys(fieldmap)){
      FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), mcw->loc_req.zone_id);
    }
#endif    
    (*seq) ++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeBySandStream(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK*       mcw = work;
  GAMESYS_WORK*        gsys = mcw->gsys;
  FIELDMAP_WORK* fieldmap = mcw->fieldmap;
  GAMEDATA*        gamedata = mcw->gamedata;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    (*seq) ++;
    break;
  case 1: // �����ޏ�C�x���g
    GMEVENT_CallEvent( 
        event, EVENT_DISAPPEAR_FallInSand( event, gsys, fieldmap, &mcw->stream_pos ) );
    (*seq)++;
    break;
  case 2: // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 3: // �����o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Fall( event, gsys, fieldmap ) );
    (*seq) ++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByAnanukenohimo(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK*     mcw = work;
  GAMESYS_WORK*      gsys = mcw->gsys;
  GAMEDATA*      gamedata = mcw->gamedata;
  FIELDMAP_WORK* fieldmap = mcw->fieldmap;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    (*seq) ++;
    break;
  case 1: // �ޏ�C�x���g
    GMEVENT_CallEvent( 
        event, EVENT_DISAPPEAR_Ananukenohimo( event, gsys, fieldmap ) );
    (*seq)++;
    break;
  case 2: // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_ESCAPE ) );
    (*seq)++;
    break;
  case 3: // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Ananukenohimo( event, gsys, fieldmap ) );
    (*seq) ++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByAnawohoru(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK*     mcw = work;
  GAMESYS_WORK*      gsys = mcw->gsys;
  GAMEDATA*      gamedata = mcw->gamedata;
  FIELDMAP_WORK* fieldmap = mcw->fieldmap;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    (*seq) ++;
    break;
  case 1: // �ޏ�C�x���g
    GMEVENT_CallEvent( 
        event, EVENT_DISAPPEAR_Anawohoru( event, gsys, fieldmap ) );
    (*seq)++;
    break;
  case 2: // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_ESCAPE ) );
    (*seq)++;
    break;
  case 3: // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Anawohoru( event, gsys, fieldmap ) );
    (*seq) ++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByTeleport(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK*     mcw = work;
  GAMESYS_WORK*      gsys = mcw->gsys;
  GAMEDATA*      gamedata = mcw->gamedata;
  FIELDMAP_WORK* fieldmap = mcw->fieldmap;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    (*seq) ++;
    break;
  case 1: // �ޏ�C�x���g
    GMEVENT_CallEvent( 
        event, EVENT_DISAPPEAR_Teleport( event, gsys, fieldmap ) );
    (*seq)++;
    break;
  case 2: // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_TELEPORT ) );
    (*seq)++;
    break;
  case 3: // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Teleport( event, gsys, fieldmap ) );
    (*seq) ++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeToUnion(GMEVENT * event, int *seq, void*work)
{
  MAPCHANGE_WORK*     mcw = work;
  GAMESYS_WORK*      gsys = mcw->gsys;
  FIELDMAP_WORK* fieldmap = mcw->fieldmap;
  GAMEDATA*      gamedata = mcw->gamedata;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, fieldmap) );
    // �J�ڌ�̋G�߂����߂�
    CheckSeasonChange( mcw );
    (*seq) ++;
    break;
  case 1:
    // �����i���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceIn( event, gsys, gamedata, fieldmap, mcw->loc_req, mcw->exit_type ) );
    (*seq)++;
    break;
  case 2: // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( mcw, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 3: // ���j�I���ʐM�N��
    UNION_CommBoot( gsys );
    (*seq) ++;
    break;
  case 4:
    // �����ޏo�C�x���g
    GMEVENT_CallEvent( event, EVENT_EntranceOut( event, gsys, gamedata, fieldmap, mcw->loc_req ) );
    (*seq) ++;
    break;
  case 5:
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
  mcw->next_season = INVALID_SEASON_ID;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapPos(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
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
GMEVENT * DEBUG_EVENT_ChangeMapRailLocation(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const RAIL_LOCATION * rail_loc, u16 dir )
{
  MAPCHANGE_WORK * mcw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
  mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys );
  
  LOCATION_SetDirectRail(&mcw->loc_req, zone_id, dir, rail_loc->rail_index, rail_loc->line_grid, rail_loc->width_grid);
  mcw->exit_type = EXIT_TYPE_NONE;
  
  return event;
}



//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_ChangeMapDefaultPos(GAMESYS_WORK * gsys,
    FIELDMAP_WORK * fieldmap, u16 zone_id)
{
  MAPCHANGE_WORK * mcw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
  mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys );

  if (zone_id >= ZONEDATA_GetZoneIDMax()) {
    GF_ASSERT( 0 );
    zone_id = 0;
  }
  LOCATION_DEBUG_SetDefaultPos(&mcw->loc_req, zone_id);
  mcw->exit_type = EXIT_TYPE_NONE;
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_QuickChangeMapDefaultPos(GAMESYS_WORK * gsys,
    FIELDMAP_WORK * fieldmap, u16 zone_id)
{
  MAPCHANGE_WORK * mcw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, DEBUG_EVENT_QuickMapChange, sizeof(MAPCHANGE_WORK));
  mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys );

  if (zone_id >= ZONEDATA_GetZoneIDMax()) {
    GF_ASSERT( 0 );
    zone_id = 0;
  }
  LOCATION_DEBUG_SetDefaultPos(&mcw->loc_req, zone_id);
  mcw->exit_type = EXIT_TYPE_NONE;
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���� �j
 * @param gsys          �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap      �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param disappear_pos �������S�_�̍��W
 * @param zone_id       �J�ڂ���}�b�v��ZONE�w��
 * @param appear        �J�ڐ�ł̍��W
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapBySandStream(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, 
    const VecFx32* disappear_pos, u16 zone_id, const VecFx32* appear_pos )
{
  MAPCHANGE_WORK * mcw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeBySandStream, sizeof(MAPCHANGE_WORK));
  mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys ); 
  LOCATION_SetDirect(
      &mcw->loc_req, zone_id, DIR_DOWN, appear_pos->x, appear_pos->y, appear_pos->z);
  mcw->exit_type = EXIT_TYPE_NONE;
  VEC_Set( &mcw->stream_pos, disappear_pos->x, disappear_pos->y, disappear_pos->z );
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���Ȃʂ��̃q�� �j
 * @param gsys �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByAnanukenohimo( FIELDMAP_WORK *fieldWork, GAMESYS_WORK * gsys )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeByAnanukenohimo, sizeof(MAPCHANGE_WORK));
  work = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( work, gsys ); 
  work->loc_req = *(GAMEDATA_GetEscapeLocation( work->gamedata ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type = EXIT_TYPE_NONE;
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���Ȃ��ق� )
 * @param gsys �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByAnawohoru( GAMESYS_WORK * gsys )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeByAnawohoru, sizeof(MAPCHANGE_WORK));
  work = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( work, gsys ); 
  work->loc_req = *(GAMEDATA_GetEscapeLocation( work->gamedata ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type = EXIT_TYPE_NONE;
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i �e���|�[�g )
 * @param gsys �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByTeleport( GAMESYS_WORK * gsys )
{
  u16 warp_id;
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeByTeleport, sizeof(MAPCHANGE_WORK));
  work = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( work, gsys ); 
  warp_id = GAMEDATA_GetWarpID( work->gamedata );
  WARPDATA_GetWarpLocation( warp_id, &work->loc_req );
  LOCATION_DEBUG_SetDefaultPos( &work->loc_req, work->loc_req.zone_id );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type = EXIT_TYPE_NONE;
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapToUnion( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap )
{
  MAPCHANGE_WORK * mcw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeToUnion, sizeof(MAPCHANGE_WORK));
  mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys ); 
  LOCATION_DEBUG_SetDefaultPos( &mcw->loc_req, ZONE_ID_UNION );
  mcw->exit_type = EXIT_TYPE_NONE;
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapByConnect(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    const CONNECT_DATA * cnct, LOC_EXIT_OFS exit_ofs)
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
    CONNECTDATA_SetNextLocation(cnct, &mcw->loc_req, exit_ofs);
  }
  mcw->exit_type = CONNECTDATA_GetExitType(cnct);

  {
    //�t�B�[���h�������ւ̈ړ��̍ۂ͒E�o��ʒu���L�����Ă���
    const LOCATION * ent = GAMEDATA_GetEntranceLocation(gamedata);
    if (ZONEDATA_IsFieldMatrixID(ent->zone_id) == TRUE
        && ZONEDATA_IsFieldMatrixID(mcw->loc_req.zone_id) == FALSE)
    {
      GAMEDATA_SetEscapeLocation( gamedata, ent );
    }
  }
  return event;
}

static GMEVENT * EVENT_ChangeMapPosNoFadeCore(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    EV_MAPCHG_TYPE type, u16 zone_id, const VecFx32 * pos, u16 dir )
{
  MAPCHANGE_WORK * mcw;
  GMEVENT * event;

  event = GMEVENT_Create(gsys, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK));
  mcw = GMEVENT_GetEventWork(event);
  MAPCHANGE_WORK_init( mcw, gsys );
  
  LOCATION_SetDirect(&mcw->loc_req, zone_id, dir, pos->x, pos->y, pos->z);
  mcw->exit_type = EXIT_TYPE_NONE;
  mcw->mapchange_type = type; 
  return event;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapPosNoFade(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const VecFx32 * pos, u16 dir )
{
  return EVENT_ChangeMapPosNoFadeCore( gsys, fieldmap,
            EV_MAPCHG_NORMAL, zone_id, pos, dir );
}

GMEVENT * EVENT_ChangeMapSorawotobu(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const VecFx32 * pos, u16 dir )
{
  return EVENT_ChangeMapPosNoFadeCore( gsys, fieldmap,
            EV_MAPCHG_FLYSKY, zone_id, pos, dir );
}



//------------------------------------------------------------------
//  ���}�b�vID���C���N�������g���Ă���B�ő�l�ɂȂ�����擪�ɖ߂�
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_ChangeToNextMap(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
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

//============================================================================================
//
//
//  �}�b�v�J�ڏ���������
//
//
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��̃}�b�v�J�ڏ����i�t�B�[���h�񐶐����j
 */
//--------------------------------------------------------------
void MAPCHG_GameOver( GAMESYS_WORK * gsys )
{
  LOCATION loc_req;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  u16 warp_id = GAMEDATA_GetWarpID( gamedata );
  //�����|�C���g���擾
  WARPDATA_GetRevivalLocation( warp_id, &loc_req );
  {
    LOCATION esc;
    //�G�X�P�[�v�|�C���g���Đݒ�
    WARPDATA_GetEscapeLocation( warp_id, &esc );
    GAMEDATA_SetEscapeLocation( gamedata, &esc );
  }

  //�}�b�v���[�h�Ȃǋ@�\�w�����������
  MAPCHG_releaseMapTools( gsys );

  { 
    FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
  }
  //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
  MAPCHG_setupMapTools( gsys, &loc_req );
  
  //�V�����}�b�vID�A�����ʒu���Z�b�g
  MAPCHG_updateGameData( gsys, &loc_req );

  //�C�x���g���ԍX�V
  EVTIME_Update( gamedata );

  //�Q�[���I�[�o�[���̃t���O�̃N���A
  FIELD_FLAGCONT_INIT_GameOver( gamedata, loc_req.zone_id );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static u16 GetDirValueByDirID(EXIT_DIR dir_id)
{
  switch (dir_id) {
  default:
  case EXIT_DIR_UP: return 0x0000;
  case EXIT_DIR_LEFT: return 0x4000;
  case EXIT_DIR_DOWN: return 0x8000;
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
  result = EVENTDATA_SetLocationByExitID(evdata, loc_tmp, loc_req->exit_id, loc_req->exit_ofs );

  if (!result) {
    //�f�o�b�O�p�����F�{���͕s�v�Ȃ͂�
    OS_Printf("connect: debug default position\n");
    LOCATION_DEBUG_SetDefaultPos(loc_tmp, loc_req->zone_id);
  }
}

//------------------------------------------------------------------
/**
 * @brief Map�؂�ւ����̃f�[�^�X�V����
 *
 * @todo  �����ă}�b�v���܂������Ƃ��̏����Ƃ̋��ʕ�����������Ə������邱��
 */
//------------------------------------------------------------------
static void MAPCHG_updateGameData( GAMESYS_WORK * gsys, const LOCATION * loc_req )
{
  LOCATION loc;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  PLAYER_WORK * mywork = GAMEDATA_GetMyPlayerWork(gamedata);
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
  FIELD_STATUS* fldstatus = GAMEDATA_GetFieldStatus(gamedata);

  { 
    char buf[ZONEDATA_NAME_LENGTH*2];
    ZONEDATA_DEBUG_GetZoneName(buf, loc_req->zone_id);
    TAMADA_Printf("MAPCHG_updateGameData:%s\n", buf);
  }

  
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
    direction = GetDirValueByDirID(loc.dir_id);
    PLAYERWORK_setDirection(mywork, direction);

    if( LOCATION_GetPosType( &loc ) == LOCATION_POS_TYPE_3D ){
      PLAYERWORK_setPosition(mywork, &loc.location_pos.pos);
      PLAYERWORK_setPosType( mywork, LOCATION_POS_TYPE_3D );
    }else{
      PLAYERWORK_setRailPosition(mywork, &loc.location_pos.railpos);
      PLAYERWORK_setPosType( mywork, LOCATION_POS_TYPE_RAIL );
    }
  }

  // ISS�Ƀ]�[���؂�ւ���ʒm
  {
    ISS_SYS* iss = GAMESYSTEM_GetIssSystem( gsys );
    ISS_SYS_ZoneChange( iss, loc.zone_id );
  }
  
  //�J�n�ʒu���L�����Ă���
  GAMEDATA_SetStartLocation(gamedata, &loc);

  //���[�v��o�^
  {
    u16 warp_id;
    warp_id = WARPDATA_SearchByRoomID( loc.zone_id );
    if (warp_id) {
      GAMEDATA_SetWarpID( gamedata, warp_id );
    }
  }
  //�M�~�b�N�A�T�C��
  AssignGimmickID(gamedata, loc.zone_id);

  //����X�N���v�g�Ăяo���F�]�[���؂�ւ�
  SCRIPT_CallZoneChangeScript( gsys, HEAPID_PROC );

  //�C�x���g���ԍX�V
  EVTIME_Update( gamedata );

  //�}�b�v�J�ڎ��̃t���O������
  FIELD_FLAGCONT_INIT_MapJump( gamedata, loc.zone_id );

  //�}�b�v�����t���O�Z�b�g
  ARRIVEDATA_SetArriveFlag( gamedata, loc.zone_id );

  //�V�K�]�[���ɔz�u���铮�샂�f����ǉ�
  MAPCHG_loadMMdl( gamedata, loc_req );

  //�t�B�[���h�Z�@�}�b�v����
  MAPCHG_setupFieldSkillMapEff( gamedata, &loc );

  // WFBC�̐ݒ�
  MAPCHG_SetUpWfbc( gamedata, &loc );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void MAPCHG_loadMMdl( GAMEDATA * gamedata, const LOCATION *loc_req )
{
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
  u16 count = EVENTDATA_GetNpcCount( evdata );
  
  if( count ){
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gamedata );
    MMDLSYS *fmmdlsys = GAMEDATA_GetMMdlSys( gamedata );
    const MMDL_HEADER *header = EVENTDATA_GetNpcData( evdata );
    MMDLSYS_SetMMdl( fmmdlsys, header, loc_req->zone_id, count, evwork );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  WFBC�̃��f����ǂݍ���
 */
//-----------------------------------------------------------------------------
static void MAPCHG_loadMMdlWFBC( GAMEDATA * gamedata, const LOCATION *loc )
{
  if( ZONEDATA_IsWfbc( loc->zone_id ) )
  {
    const FIELD_WFBC_CORE* cp_wfbc;
    const FIELD_STATUS* cp_fs = GAMEDATA_GetFieldStatus( gamedata );
    MAPMODE mapmode = FIELD_STATUS_GetMapMode( cp_fs );
    MMDLSYS *fmmdlsys = GAMEDATA_GetMMdlSys( gamedata );
    MMDL_HEADER*p_header;
    u32  count;
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gamedata );


    if( mapmode == MAPMODE_NORMAL )
    {
      cp_wfbc = GAMEDATA_GetMyWFBCCoreData( gamedata );
    }
    else
    {
      // �ʐM����̃f�[�^�ɕύX
      cp_wfbc = GAMEDATA_GetWFBCCoreData( gamedata, GAMEDATA_WFBC_ID_COMM );
    }
    p_header = FIELD_WFBC_CORE_MMDLHeaderCreateHeapLo( cp_wfbc, mapmode, GFL_HEAPID_APP );

    count = FIELD_WFBC_CORE_GetPeopleNum( cp_wfbc, mapmode );


    // 
    if( p_header && (count > 0) )
    {
      TOMOYA_Printf( "WFBC MMDL SetUp\n" );
      MMDLSYS_SetMMdl( fmmdlsys, p_header, loc->zone_id, count, evwork );
      GFL_HEAP_FreeMemory( p_header );
    }
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void MAPCHG_releaseMMdl( GAMEDATA * gamedata )
{
    MMDLSYS_DeleteMMdl( GAMEDATA_GetMMdlSys(gamedata) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC�̃Z�b�g�A�b�v����
 *
 *	@param	gamedata
 *	@param	loc 
 */
//-----------------------------------------------------------------------------
static void MAPCHG_SetUpWfbc( GAMEDATA * gamedata, const LOCATION *loc )
{
  if( !ZONEDATA_IsWfbc( loc->zone_id ) )
  {
    return ;
  }

  MAPCHG_loadMMdlWFBC( gamedata, loc );

  // �܂��ɓ������I���Z
  {
    FIELD_WFBC_CORE* p_wfbc;
    const FIELD_STATUS* cp_fs = GAMEDATA_GetFieldStatus( gamedata );
    MAPMODE mapmode = FIELD_STATUS_GetMapMode( cp_fs );


    if( mapmode == MAPMODE_NORMAL )
    {
      p_wfbc = GAMEDATA_GetMyWFBCCoreData( gamedata );

      // �X�ɓ��������Z
      FIELD_WFBC_CORE_CalcMoodInTown( p_wfbc );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h�Z�@��ʌ��ʃ}�X�N�̐ݒ�
 */
//-----------------------------------------------------------------------------
static void MAPCHG_setupFieldSkillMapEff( GAMEDATA * gamedata, const LOCATION *loc_req )
{
  FIELD_STATUS * fldstatus =  GAMEDATA_GetFieldStatus( gamedata ); 
  u32 fs_effmsk = ZONEDATA_GetFieldSkillMapEffMsk( loc_req->zone_id );

  //�t���b�V���@�V�X�e���t���O�����āA�}�X�N������������
  if( FIELDSKILL_MAPEFF_MSK_IS_ON(fs_effmsk, FIELDSKILL_MAPEFF_MSK_FLASH_NEAR) )
  {
    if( FIELD_STATUS_IsFieldSkillFlash(fldstatus) )
    {
      // �t���b�V���Z�g�p��ɕϊ�
      FIELDSKILL_MAPEFF_MSK_OFF(fs_effmsk, FIELDSKILL_MAPEFF_MSK_FLASH_NEAR);
      FIELDSKILL_MAPEFF_MSK_ON(fs_effmsk, FIELDSKILL_MAPEFF_MSK_FLASH_FAR);
      
    }
  }
  FIELD_STATUS_SetFieldSkillMapEffectMsk( fldstatus, fs_effmsk );
}

//--------------------------------------------------------------
/**
 * @brief
 * @param gamedata
 *
 * @todo  �}�b�v���܂�����or��ʈÓ]���������ƈێ�����郁������ԂȂǂ͂����Őݒ�
 * @todo  ���L�́�check�̑Ώ����s��
 */
//--------------------------------------------------------------
static void MAPCHG_setupMapTools( GAMESYS_WORK * gsys, const LOCATION * loc_req )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
  //MAPCHG���W���[���Q��FieldMap�����݂��Ȃ��Ƃ��ɌĂ΂��
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  GF_ASSERT( fieldmap == NULL );

  //��check�@�X�N���v�g�Ń}�b�v�쐬�O�Ɏ��s�ł���^�C�~���O���o����΁A�����ōs���悤�ɂ�����
  if(ZONEDATA_IsPalaceField(loc_req->zone_id) || ZONEDATA_IsBingo(loc_req->zone_id)){
    FIELD_STATUS_SetMapMode( GAMEDATA_GetFieldStatus( gamedata ), MAPMODE_INTRUDE );
    //GAMEDATA_SetMapMode(gamedata, MAPMODE_INTRUDE);
  }
  //��check�@���j�I�����[���ւ̈ړ�����t�X�N���v�g�Ő��䂷��悤�ɂȂ�����T�u�X�N���[�����[�h��
  //         �ύX�����̃X�N���v�g���ōs���悤�ɂ���
  switch(loc_req->zone_id){
  case ZONE_ID_UNION:
  case ZONE_ID_CLOSSEUM:
  case ZONE_ID_CLOSSEUM02:
    GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_UNION);
    break;
  default:
    GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_NORMAL);
    break;
  } 

  //�C�x���g�N���f�[�^�̓ǂݍ���
  EVENTDATA_SYS_Load(evdata, loc_req->zone_id, GAMEDATA_GetSeasonID(gamedata) );

  //�}�g���b�N�X�f�[�^�̍X�V
  {
    MAP_MATRIX * matrix = GAMEDATA_GetMapMatrix( gamedata );
    u16 matID = ZONEDATA_GetMatrixID( loc_req->zone_id );
    MAP_MATRIX_Init( matrix, matID, loc_req->zone_id );
    MAP_MATRIX_CheckReplace( matrix, gamedata );
  }

  //�t�B�[���h�Z �}�b�v����
  MAPCHG_setupFieldSkillMapEff( gamedata, loc_req );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void MAPCHG_releaseMapTools( GAMESYS_WORK * gsys )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  //MAPCHG���W���[���Q��FieldMap�����݂��Ȃ��Ƃ��ɌĂ΂��
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  GF_ASSERT( fieldmap == NULL );

  //�z�u���Ă������샂�f�����폜
  MAPCHG_releaseMMdl( gamedata );

}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setFirstBGM(GAMEDATA * gamedata, u16 zone_id)
{
  FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gamedata );
  PLAYER_WORK *player = GAMEDATA_GetPlayerWork( gamedata, 0 );
  PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
  u32 no = FIELD_SOUND_GetFieldBGMNo( gamedata, form, zone_id );
  OS_Printf("NEXT BGM NO=%d\n",no);
  FIELD_SOUND_PlayNextBGM_Ex( fsnd, no, 0, 60 );// �Q�[���J�n����BGM�t�F�[�h�C���Ŏn�܂�
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
 * @brief �M�~�b�N�̃A�T�C��
 * 
 * @param   inZoneID    �]�[���h�c
 * 
 * @return  none
 */
//---------------------------------------------------------------------------
static void AssignGimmickID(GAMEDATA * gamedata, int inZoneID)
{
  typedef struct GMK_ASSIGN_DATA_tag
  {
    u32 ZoneID;
    u16 ResNum;
    u16 ObjNum;
  }GMK_ASSIGN_DATA;

  GIMMICKWORK *work;
  //�M�~�b�N���[�N�擾
  work = GAMEDATA_GetGimmickWork(gamedata);

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

