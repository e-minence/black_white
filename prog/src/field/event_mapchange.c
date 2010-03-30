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
#include "gamesystem/pm_weather.h"
#include "gamesystem/game_beacon.h"

#include "system/main.h"
#include "sound/pm_sndsys.h" // �T�E���h�V�X�e���Q��
#include "demo/demo3d.h" //�w�͂��߂���x��3D�f��

#include "item/itemsym.h"

#include "field/zonedata.h"
#include "field/fieldmap.h"
#include "field/location.h"
#include "field/rail_location.h"  // RAIL_LOCATION
#include "field/eventdata_sxy.h"
#include "field/fldmmdl.h"
#include "field/fieldskill_mapeff.h"
#include "field/field_status_local.h" // FIELD_STATUS
#include "field/field_flagcontrol.h" // FIELD_FLAGCONT_INIT�`
#include "field/fieldmap_proc.h"  // FLDMAP_BASESSYS_GRID

#include "event_season_display.h"
#include "event_fieldmap_control.h"
#include "event_mapchange.h"
#include "event_fldmmdl_control.h"
#include "event_fieldmap_control_local.h" // event_mapchange�������FieldOpen Close
#include "field_place_name.h"   // FIELD_PLACE_NAME_DisplayForce 
#include "fieldmap/zone_id.h"   //��check�@���j�I����SubScreen�ݒ�b��Ώ��̈�
#include "script.h"
#include "warpdata.h"
#include "move_pokemon.h"
#include "field_sound.h"
#include "effect_encount.h" 
#include "event_entrance_in.h"
#include "event_entrance_out.h"
#include "event_appear.h"
#include "event_disappear.h" 
#include "savedata/gimmickwork.h"   //for GIMMICKWORK
#include "field_comm/intrude_main.h"
#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN

#include "net_app/union/union_main.h" // for UNION_CommBoot

#include "system/playtime_ctrl.h" // for PLAYTIMECTRL_Start
#include "savedata/gametime.h"  // for GMTIME
#include "gamesystem/pm_season.h"  // for PMSEASON_TOTAL
#include "ev_time.h"  // EVTIME_Update
#include "../../../resource/fldmapdata/flagwork/flag_define.h"  // SYS_FLAG_SPEXIT_REQUEST
#include "../../../resource/fldmapdata/flagwork/work_define.h"  // WK_SYS_SCENE_COMM 
#include "../../../resource/fldmapdata/script/pokecen_scr_def.h"  // SCRID_POKECEN_ELEVATOR_OUT


#ifdef PM_DEBUG
#include "../gamesystem/debug_data.h"
FS_EXTERN_OVERLAY(debug_data);
#include "debug/debug_flg.h"
#endif

#include "seasonpoke_form.h"    //for SEASONPOKE_FORM_ChangeForm
#include "enceff.h"             //for ENCEFF_SetEncEff

#include "palace_warp_check.h"
#include "../../../resource/fldmapdata/script/field_ev_scr_def.h" // for SCRID_FLD_EV_

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

static void AssignGimmickID(GAMEDATA * gamedata, int inZoneID);

static GMEVENT_RESULT EVENT_MapChangeNoFade(GMEVENT * event, int *seq, void*work);

static GMEVENT* EVENT_FirstGameStart( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork );
static GMEVENT* EVENT_FirstMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork );
static GMEVENT_RESULT EVENT_MapChangePalaceWithCheck( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_MapChangePalace( GMEVENT* event, int* seq, void* wk );
static GMEVENT * EVENT_ChangeMapPalaceWithCheck( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc);
static GMEVENT * EVENT_ChangeMapPalace( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc);

static void setNowLoaction(LOCATION * return_loc, FIELDMAP_WORK * fieldmap);

//============================================================================================
//
//  �C�x���g�F�Q�[���J�n
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK*   gameSystem;
  GAME_INIT_WORK* gameInitWork;
  DEMO3D_PARAM    demo3dParam;
} FIRST_START_WORK;

typedef struct PALACE_JUMP_tag
{
  LOCATION Loc;
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
}PALACE_JUMP;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FirstGameStartEvent( GMEVENT* event, int* seq, void* wk )
{
  FIRST_START_WORK* work       = (FIRST_START_WORK*)wk;
  GAMESYS_WORK*     gameSystem = work->gameSystem;
  GAMEDATA*         gameData   = GAMESYSTEM_GetGameData( gameSystem );

  switch( *seq ) {
  case 0:
    // VRAM�S�N���A
    GX_SetBankForLCDC( GX_VRAM_LCDC_ALL );
    MI_CpuClearFast( (void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE );
    GX_DisableBankForLCDC();
    (*seq)++;
    break;

  case 1:
    // �G�߂�\������
    {
      u8 season = GAMEDATA_GetSeasonID( gameData );
      GMEVENT_CallEvent( event, EVENT_SimpleSeasonDisplay( gameSystem, season, season ) );
    }
		(*seq)++;
		break;

	case 2:
    // �f�����Ă�
    DEMO3D_PARAM_SetFromRTC( &work->demo3dParam, gameData, DEMO3D_ID_INTRO_TOWN, 0 );
    GAMESYSTEM_CallProc( gameSystem, FS_OVERLAY_ID(demo3d), &Demo3DProcData, &work->demo3dParam );
		(*seq)++;
		break;

	case 3:
    // �f���̏I����҂�
    if( GAMESYSTEM_IsProcExists( gameSystem ) == GFL_PROC_MAIN_NULL ) { (*seq)++; }
		break;

  case 4:
    // �C�x���g�ύX
    {
      GMEVENT* nextEvent;
      nextEvent = EVENT_FirstMapIn( gameSystem, work->gameInitWork );
      GMEVENT_ChangeEvent( event, nextEvent );
    }
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
static GMEVENT* EVENT_FirstGameStart( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  FIRST_START_WORK* work;

  // �C�x���g�𐶐�����
  event = GMEVENT_Create( gameSystem, NULL, FirstGameStartEvent, sizeof(FIRST_START_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������������
  work->gameSystem   = gameSystem;
  work->gameInitWork = gameInitWork;

  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  GAMEINIT_MODE game_init_mode;
  LOCATION loc_req;
  DEMO3D_PARAM demo3dParam;
}FIRST_MAPIN_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT FirstMapInEvent(GMEVENT * event, int *seq, void *work)
{
  FIRST_MAPIN_WORK * fmw = work;
  GAMESYS_WORK * gsys = fmw->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENTWORK * ev = GAMEDATA_GetEventWork( gamedata );
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
      MAPCHG_setupMapTools( gsys, &fmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
      MAPCHG_updateGameData( gsys, &fmw->loc_req ); //�V�����}�b�vID�A�����ʒu���Z�b�g
      break;

    case GAMEINIT_MODE_DEBUG:
      SCRIPT_CallDebugGameStartInitScript( gsys, GFL_HEAPID_APP );
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
      MAPCHG_setupMapTools( gsys, &fmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
      MAPCHG_updateGameData( gsys, &fmw->loc_req ); //�V�����}�b�vID�A�����ʒu���Z�b�g
      break;

    case GAMEINIT_MODE_CONTINUE:
      if (EVENTWORK_CheckEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST ) )
      { //����ڑ����N�G�X�g�F
        const LOCATION * spLoc = GAMEDATA_GetSpecialLocation( gamedata );
        fmw->loc_req = *spLoc;
        // EVENTWORK_ResetEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST );
        FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
        MAPCHG_releaseMapTools( gsys );
        MAPCHG_setupMapTools( gsys, &fmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
        MAPCHG_updateGameData( gsys, &fmw->loc_req ); //�V�����}�b�vID�A�����ʒu���Z�b�g
      }
      else
      {
        FIELD_STATUS_SetContinueFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
        MAPCHG_setupMapTools( gsys, &fmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
        EVTIME_Update( gamedata ); //�C�x���g���ԍX�V
        FIELD_FLAGCONT_INIT_Continue( gamedata, fmw->loc_req.zone_id ); //�R���e�B�j���[�ɂ��t���O���Ƃ�����
        //�V�C���[�h����
        //**���FIELD_FLAGCONT_INIT_Continue�ňړ��|�P�����̓V�C�����I�����̂ŁA
        //�K���A���̉��ōs���B**
        PM_WEATHER_UpdateSaveLoadWeatherNo( gamedata, fmw->loc_req.zone_id );
      }
      break;

    default:
      GF_ASSERT(0);
    } 

    (*seq)++;
    break;

  case 1:
    // BGM �t�F�[�h�C��
    {
      u32 soundIdx = FSND_GetFieldBGM( gamedata, fmw->loc_req.zone_id );
      GMEVENT_CallEvent( event, EVENT_FSND_ChangeBGM( gsys, soundIdx, FSND_FADE_NONE, FSND_FADE_NORMAL ) );
    }
    (*seq)++;
    break;

  case 2:
    GMEVENT_CallEvent(event, EVENT_FieldOpen_FieldProcOnly(gsys));
    (*seq)++;
    break;

  case 3:
    if( (fmw->game_init_mode == GAMEINIT_MODE_CONTINUE) && 
        EVENTWORK_CheckEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST ) )
    {
      EVENTWORK_ResetEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST );
      if ( *(EVENTWORK_GetEventWorkAdrs( ev, WK_SYS_SCENE_COMM )) != 0 ) {
        SCRIPT_CallScript( event, SCRID_POKECEN_ELEVATOR_OUT_CONTINUE, NULL, NULL, HEAPID_FIELDMAP );
        *seq = 5;
        break;
      }
    }
    (*seq) ++;
    break;

  case 4:
    fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
    // ��ʃt�F�[�h�C��
    switch( fmw->game_init_mode ) {
    case GAMEINIT_MODE_DEBUG:
      GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black( gsys, fieldmap, FIELD_FADE_WAIT ) );
      break;
    case GAMEINIT_MODE_FIRST:
      GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black( gsys, fieldmap, FIELD_FADE_WAIT ) );
      break;
    case GAMEINIT_MODE_CONTINUE:
      { // �Q�[���I�����̋G�߂�\������
        u8 season = GAMEDATA_GetSeasonID( gamedata );
        GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Season( gsys, fieldmap, season, season ) );
      }
      break;
    default:
      GF_ASSERT(0);
    }

    //�G�߃|�P�����t�H�����`�F���W�R���e�j���[���͕K���ʂ�悤�ɂ���
    {
      u8 season;
      POKEPARTY *party;
      season = GAMEDATA_GetSeasonID(gamedata);
      party = GAMEDATA_GetMyPokemon(gamedata);
      SEASONPOKE_FORM_ChangeForm(party, season);
    }
    (*seq)++;
    break;

  case 5:
    // �n����\������
    fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
    if( FIELDMAP_GetPlaceNameSys(fieldmap) )
    {
      FIELD_PLACE_NAME_DisplayForce( FIELDMAP_GetPlaceNameSys(fieldmap), fmw->loc_req.zone_id );
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�B�[���h�̐V�K�J�n����������
 *
 *	@param	gsys  �Q�[���V�X�e��
 *
 *	GAME_DATA �̏��ŁA�V�K�Q�[���J�n���ɍs�������������L�q���Ă��������B
 *
 *	���̃^�C�~���O�ŌĂ΂�鏈���́AOVERLAY�@FIELD_INIT�ɔz�u���Ă��������B
 */
//-----------------------------------------------------------------------------
static void GAME_FieldFirstInit( GAMESYS_WORK * gsys )
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);

  // WFBC�X�����̏ꏊ������
  FIELD_WFBC_CORE_SetUp( GAMEDATA_GetMyWFBCCoreData(gamedata), GAMEDATA_GetMyStatus(gamedata), GFL_HEAPID_APP ); //heap �̓e���|�����p
  FIELD_WFBC_CORE_SetUpZoneData( GAMEDATA_GetMyWFBCCoreData(gamedata) );
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
static GMEVENT* EVENT_FirstMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  FIRST_MAPIN_WORK* work;

  // �C�x���g�𐶐�����
  event = GMEVENT_Create( gameSystem, NULL, FirstMapInEvent, sizeof(FIRST_MAPIN_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N��������
  work->gsys = gameSystem;
  work->gamedata   = GAMESYSTEM_GetGameData(gameSystem);
  work->game_init_mode = gameInitWork->mode;

  switch(gameInitWork->mode){
  case GAMEINIT_MODE_CONTINUE:
    LOCATION_SetDirect(&work->loc_req, gameInitWork->mapid, gameInitWork->dir, 
      gameInitWork->pos.x, gameInitWork->pos.y, gameInitWork->pos.z);
    break;
  case GAMEINIT_MODE_FIRST:
    LOCATION_SetGameStart(&work->loc_req);
    GAME_FieldFirstInit( gameSystem );  // �t�B�[���h���̏�����
    break;
#ifdef PM_DEBUG
  case GAMEINIT_MODE_DEBUG:
#ifdef DEBUG_ONLY_FOR_iwasawa
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
       DEBUG_FLG_FlgOn( DEBUG_FLG_ShortcutBtlIn );
    }
#endif
    GFL_OVERLAY_Load( FS_OVERLAY_ID(debug_data));
    //�K���Ɏ莝���|�P������Add
    //�f�o�b�O�A�C�e���ǉ��ȂǂȂǁc
    DEBUG_SetStartData( GAMESYSTEM_GetGameData( gameSystem ), GFL_HEAPID_APP );

    GFL_OVERLAY_Unload( FS_OVERLAY_ID(debug_data));
    
    LOCATION_SetDefaultPos(&work->loc_req, gameInitWork->mapid);

    GAME_FieldFirstInit( gameSystem );  // �t�B�[���h���̏�����
    break;
#endif //PM_DEBUG
  }
  //�v���C���ԃJ�E���g�@�J�n
  PLAYTIMECTRL_Start();

  return event;
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
GMEVENT* EVENT_CallGameStart( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;

  switch( gameInitWork->mode ) {
  case GAMEINIT_MODE_FIRST:
    event = EVENT_FirstGameStart( gameSystem, gameInitWork );
    break;
  case GAMEINIT_MODE_CONTINUE:
  case GAMEINIT_MODE_DEBUG:
    event = EVENT_FirstMapIn( gameSystem, gameInitWork );
    break;
  default:
    GF_ASSERT(0);
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
    GMEVENT_CallEvent(event, EVENT_FieldClose_FieldProcOnly(gew->gsys, gew->fieldmap));
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
GMEVENT * DEBUG_EVENT_CallGameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
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
typedef struct 
{
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;

  u16 before_zone_id;   ///<�J�ڑO�}�b�vID
  LOCATION loc_req;     ///<�J�ڐ�w��
  EXIT_TYPE exit_type;
  u8 mapchange_type; ///<�`�F���W�^�C�v EV_MAPCHG_TYPE
  VecFx32 stream_pos;  ///<�������S���W ( �����J�ڎ��ɂ̂ݎg�p )

  BOOL seasonUpdateEnable; ///<�G�߂̍X�V�������邩�ǂ��� ( �C�x���g�Ăяo�����Ŏw�� )
  BOOL seasonUpdateOccur;  ///<�G�߂��X�V���邩�ǂ��� ( �C�x���g���Ō��� )
  u16  prevSeason;         ///<�J�ڑO�̋G��
  u16  nextSeason;         ///<�J�ڌ�̋G��

} MAPCHANGE_WORK;

typedef MAPCHANGE_WORK* MAPCHANGE_WORK_PTR;

#define INVALID_SEASON_ID (0xff)  ///<�G��ID�����l

typedef enum{
  EV_MAPCHG_NORMAL,
  EV_MAPCHG_FLYSKY,
  EV_MAPCHG_ESCAPE,
  EV_MAPCHG_TELEPORT,
  EV_MAPCHG_UNION,
} EV_MAPCHG_TYPE;


//------------------------------------------------------------------
/**
 * @brief �G�߂��X�V���邩�ǂ��������肷��
 *
 * @param work
 */
//------------------------------------------------------------------
static void SetSeasonUpdate( MAPCHANGE_WORK* work )
{
  BOOL nextZoneIsOutdoor;
  u16 prevSeason, nextSeason;

  // �J�ڑO�E�J�ڌ�̋G�߂�����
  PMSEASON_GetNextSeason( work->gameData, &prevSeason, &nextSeason );
  
  // �J�ڐ悪���O���ǂ����𔻒�
  {
    HEAPID heapID;
    u16 areaID;
    AREADATA* areaData;

    heapID   = FIELDMAP_GetHeapID( work->fieldmap );
    areaID   = ZONEDATA_GetAreaID( work->loc_req.zone_id );
    areaData = AREADATA_Create( heapID, areaID, PMSEASON_SPRING );
    nextZoneIsOutdoor = ( AREADATA_GetInnerOuterSwitch(areaData) != 0 );

    AREADATA_Delete( areaData );
  }

  // �G�߂��ω� && �J�ڐ悪���O
  if( (nextSeason != prevSeason) && (nextZoneIsOutdoor) )
  { 
    work->seasonUpdateOccur = TRUE; 
    work->prevSeason = prevSeason;
    work->nextSeason = nextSeason;
  }
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�`�F���W �R�A����
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_MapChangeCore( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = *( (MAPCHANGE_WORK_PTR*)wk );
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;
  GAME_COMM_SYS_PTR gcsp     = GAMESYSTEM_GetGameCommSysPtr(gameSystem);
  enum {
    MAPCORE_SEQ_FIELD_CLOSE = 0,
    MAPCORE_SEQ_COMM_CHECK,
    MAPCORE_SEQ_COMM_EXIT_WAIT,
    MAPCORE_SEQ_UPDATE_DATA,
    MAPCORE_SEQ_FIELD_OPEN,
    MAPCORE_SEQ_BGM_FADE_WAIT,
    MAPCORE_SEQ_FINISH,
  };

  switch( *seq )
  {
  case MAPCORE_SEQ_FIELD_CLOSE:
    //�t�B�[���h�}�b�v���I���҂�
    GMEVENT_CallEvent( event, EVENT_FieldClose_FieldProcOnly(gameSystem, fieldmap) );
    (*seq)++;
    break;

  case MAPCORE_SEQ_COMM_CHECK:
    if ( ZONEDATA_IsFieldBeaconNG(work->loc_req.zone_id ) == TRUE )
    { 
      GAME_COMM_NO comm_no = GameCommSys_BootCheck(gcsp);
      if ( comm_no == GAME_COMM_NO_FIELD_BEACON_SEARCH
          || comm_no == GAME_COMM_NO_INVASION )
      {
        //����}�b�v�ŁA�N��or�r�[�R���T�[�`��Ԃ̏ꍇ��
        //�ʐM�̒�~���������s
        GameCommSys_ExitReq(gcsp);
        *seq = MAPCORE_SEQ_COMM_EXIT_WAIT;
        break;
      }
    }
    *seq = MAPCORE_SEQ_UPDATE_DATA;
    break;

  case MAPCORE_SEQ_COMM_EXIT_WAIT:
    //�ʐM��~�����E�F�C�g
    if (GameCommSys_BootCheck(gcsp) == GAME_COMM_STATUS_NULL)
    {
      *seq = MAPCORE_SEQ_UPDATE_DATA;
    }
    break;

  case MAPCORE_SEQ_UPDATE_DATA:
    //�}�b�v���[�h�Ȃǋ@�\�w�����������
    MAPCHG_releaseMapTools( gameSystem );

    { 
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gameData), TRUE );
    }

    // �G�߂̍X�V
    if( (work->seasonUpdateEnable) && (work->seasonUpdateOccur) )
    {
      PMSEASON_SetSeasonID( gameData, work->nextSeason );
      OBATA_Printf( "update season %d ==> %d\n", work->prevSeason, work->nextSeason );
      //�G�߃|�P�����t�H�����`�F���W�����𖞂������̂ŁA�莝���̋G�߃|�P�������t�H�����`�F���W������
      {
        u8 season;
        POKEPARTY *party;
        season = GAMEDATA_GetSeasonID(gameData);
        party = GAMEDATA_GetMyPokemon(gameData);
        SEASONPOKE_FORM_ChangeForm(party, season);
      }
    }

    //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
    MAPCHG_setupMapTools( gameSystem, &work->loc_req );
    
    //�V�����}�b�vID�A�����ʒu���Z�b�g
    MAPCHG_updateGameData( gameSystem, &work->loc_req );

    //�r�[�R�����N�G�X�g(���j�I���ړ��͐�p�r�[�R��)
    if( work->mapchange_type != EV_MAPCHG_UNION ){
      GAMEBEACON_Set_ZoneChange( work->loc_req.zone_id, gameData );
    }
    //�^�C�v�ɉ������t���O������
    switch(work->mapchange_type){
    case EV_MAPCHG_FLYSKY:
      FIELD_FLAGCONT_INIT_FlySky( gameData, work->loc_req.zone_id );
      break;
    case EV_MAPCHG_ESCAPE:
      FIELD_FLAGCONT_INIT_Escape( gameData, work->loc_req.zone_id );
      break;
    case EV_MAPCHG_TELEPORT:
      FIELD_FLAGCONT_INIT_Teleport( gameData, work->loc_req.zone_id );
      break;
    }
    (*seq)++;
    break;
  case MAPCORE_SEQ_FIELD_OPEN:
    //�t�B�[���h�}�b�v���J�n�҂�
    GMEVENT_CallEvent( event, EVENT_FieldOpen_FieldProcOnly(gameSystem) );
    (*seq) ++;
    break;
  case MAPCORE_SEQ_BGM_FADE_WAIT:
    // BGM �t�F�[�h�����҂�
    GMEVENT_CallEvent( event, EVENT_FSND_WaitBGMFade(gameSystem) );
    (*seq) ++;
    break;
  case MAPCORE_SEQ_FINISH:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT* EVENT_MapChangeCore( MAPCHANGE_WORK* mcw, EV_MAPCHG_TYPE type )
{
  GMEVENT* event;
  MAPCHANGE_WORK_PTR* workPtr;

  event = GMEVENT_Create( mcw->gameSystem, NULL, EVENT_FUNC_MapChangeCore, sizeof(MAPCHANGE_WORK_PTR) );
  workPtr  = GMEVENT_GetEventWork( event );
  *workPtr = mcw;
  mcw->mapchange_type = type;
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChange( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;

  switch(*seq)
  {
  case 0:
    // �G�ߍX�V�̗L��������
    SetSeasonUpdate( work );
    // ���샂�f���̈ړ����~�߂�
    {
      MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_PauseMoveProc( fmmdlsys );
    }
    // �����i���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceIn( event, gameSystem, gameData, fieldmap, 
                          work->loc_req, work->exit_type, work->seasonUpdateOccur ) );
    (*seq)++;
    break;
  case 1:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    // �����ޏo�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceOut( event, gameSystem, gameData, fieldmap, work->loc_req, work->before_zone_id,
                           work->seasonUpdateOccur, 
                           PMSEASON_GetNextSeasonID(work->prevSeason), work->nextSeason ) );
    (*seq)++;
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u�ԃ}�b�v�`�F���W ( �f�o�b�O�p )
 */
//------------------------------------------------------------------
static GMEVENT_RESULT DEBUG_EVENT_QuickMapChange( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch(*seq)
  {
  case 0:
    // �t�F�[�h�A�E�g
    GMEVENT_CallEvent( event, DEBUG_EVENT_QuickFadeOut( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3:
    // �t�F�[�h�C��
    GMEVENT_CallEvent( event, DEBUG_EVENT_QuickFadeIn( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �t�F�[�h�Ȃ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeNoFade( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch(*seq) 
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 1:
    // BGM�X�V���N�G�X�g
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 2:
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief BGM��ύX���Ȃ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeBGMKeep( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK*  work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  GAMEDATA*        gameData   = work->gameData;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // ��ʃt�F�[�h�A�E�g ( �N���X�t�F�[�h )
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    // �}�b�v�`�F���W �R�A �C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;
  case 3:
    // ��ʃt�F�[�h�C�� ( �N���X�t�F�[�h )
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �p���X�ԃ��[�v(�����痠)�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangePalace_to_Palace( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK*  work       = wk;
  GAMESYS_WORK*    gameSystem = work->gameSystem;
  GAMEDATA*        gameData   = work->gameData;
  FIELDMAP_WORK*   fieldmap   = work->fieldmap;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // ��ʃt�F�[�h�A�E�g ( �N���X�t�F�[�h )
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3:
    // �}�b�v�`�F���W �R�A �C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;
  case 4:
    // ��ʃt�F�[�h�C�� ( �N���X�t�F�[�h )
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 5:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �����ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeBySandStream( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  GAMEDATA*       gameData   = work->gameData;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 1: 
    // �����ޏ�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_DISAPPEAR_FallInSand( event, gameSystem, fieldmap, &work->stream_pos ) );
    (*seq)++;
    break;
  case 2:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: 
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 4: 
    // �����o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Fall( event, gameSystem, fieldmap ) );
    (*seq) ++;
    break;
  case 5:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u���Ȃʂ��̃q���v�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByAnanukenohimo( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  GAMEDATA*       gameData   = work->gameData;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_DISAPPEAR_Ananukenohimo( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: 
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_ESCAPE ) );
    (*seq)++;
    break;
  case 4: 
    // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Ananukenohimo( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 5:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u���Ȃ��ق�v�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByAnawohoru( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  GAMEDATA*       gameData   = work->gameData;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq) ++;
    break;
  case 1: 
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Anawohoru( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_ESCAPE ) );
    (*seq)++;
    break;
  case 4: 
    // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Anawohoru( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 5:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u�e���|�[�g�v�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByTeleport( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  GAMEDATA*       gameData   = work->gameData;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Teleport( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: 
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_TELEPORT ) );
    (*seq)++;
    break;
  case 4: 
    // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Teleport( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 5:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u�C��_�a�@�ޏ�v�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeBySeaTemple( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // SE�Đ�
    PMSND_PlaySE( SEQ_SE_FLD_123 );
    (*seq)++;
    break;

    // ��{�I�ȃ}�b�v�`�F���W�ŊC�ʂ�
  case 2:
    if( PMSND_CheckPlaySE_byPlayerID( PMSND_GetSE_DefaultPlayerID(SEQ_SE_FLD_123) ) == FALSE )
    {
      GMEVENT* sub_event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK) );
      MAPCHANGE_WORK* sub_work;

      sub_work  = GMEVENT_GetEventWork( sub_event );
      *sub_work  = *work; // �R�s�[
      // �}�b�v�`�F���W �C�x���g
      GMEVENT_CallEvent( event, sub_event );
      (*seq)++;
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u���[�v�v�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeByWarp( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Warp( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2: 
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 3: 
    // �o��C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_Warp( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief ���j�I�����[������̃}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeFromUnion( GMEVENT * event, int *seq, void * wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;

  switch( *seq )
  {
  case 0:
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Teleport( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    // �G���x�[�^����̓o��X�N���v�g���s
    SCRIPT_CallScript( event, SCRID_POKECEN_ELEVATOR_OUT, NULL, NULL, HEAPID_FIELDMAP );
    (*seq)++;
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief ���j�I�����[���ւ̃}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeToUnion( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;

  switch( *seq )
  {
  case 0:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_UNION ) );
    (*seq)++;
    break;
  case 2: 
    // ���j�I���ʐM�N��
    UNION_CommBoot( gameSystem );
    (*seq)++;
    break;
  case 3:
    // �����ޏo�C�x���g
    GMEVENT_CallEvent( event, EVENT_APPEAR_UnionIn( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �}�b�v�`�F���W �C�x���g���[�N�̏�����
 */
//------------------------------------------------------------------
static void MAPCHANGE_WORK_init( MAPCHANGE_WORK* work, GAMESYS_WORK* gameSystem )
{
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem ); 

  work->gameSystem         = gameSystem;
  work->gameData           = GAMESYSTEM_GetGameData( gameSystem );
  work->fieldmap           = fieldmap;
  work->before_zone_id     = FIELDMAP_GetZoneID( fieldmap );
  work->seasonUpdateEnable = FALSE;
  work->seasonUpdateOccur  = FALSE;
  work->prevSeason         = 0;
  work->nextSeason         = 0;

  //�R���������Ƃ��Ă����ɏ���
  EFFECT_ENC_EffectAnmPauseSet( FIELDMAP_GetEncount( work->fieldmap), TRUE );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapPos( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                             u16 zoneID, const VecFx32* pos, u16 dir, BOOL seasonUpdateEnable )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem );
  dir = EXITDIR_fromDIR( dir );
  LOCATION_SetDirect( &(work->loc_req), zoneID, dir, pos->x, pos->y, pos->z ); 
  work->exit_type          = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = seasonUpdateEnable;

  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_ChangeMapRailLocation( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                            u16 zoneID, const RAIL_LOCATION* rail_loc, u16 dir )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create(gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDirectRail( &(work->loc_req), zoneID, dir, 
                          rail_loc->rail_index, rail_loc->line_grid, rail_loc->width_grid);
  work->exit_type = EXIT_TYPE_NONE;
  
  return event;
} 

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_ChangeMapDefaultPos( GAMESYS_WORK* gameSystem, 
                                          FIELDMAP_WORK* fieldmap, u16 zoneID )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  if( zoneID >= ZONEDATA_GetZoneIDMax() )
  {
    GF_ASSERT(0);
    zoneID = 0;
  }

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDefaultPos( &(work->loc_req), zoneID );
  work->exit_type = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_QuickChangeMapDefaultPos( GAMESYS_WORK * gameSystem,
                                                FIELDMAP_WORK* fieldmap, u16 zoneID )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  if( zoneID >= ZONEDATA_GetZoneIDMax() )
  {
    GF_ASSERT(0);
    zoneID = 0;
  }

  event = GMEVENT_Create( gameSystem, NULL, DEBUG_EVENT_QuickMapChange, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDefaultPos( &(work->loc_req), zoneID );
  work->exit_type = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���� �j
 * @param gameSystem          �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap      �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param disappearPos �������S�_�̍��W
 * @param zoneID       �J�ڂ���}�b�v��ZONE�w��
 * @param appearPos        �J�ڐ�ł̍��W
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapBySandStream( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,  
                                      const VecFx32* disappearPos, 
                                      u16 zoneID, const VecFx32* appearPos )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBySandStream, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDirect( &(work->loc_req), zoneID, DIR_DOWN, appearPos->x, appearPos->y, appearPos->z);
  VEC_Set( &(work->stream_pos), disappearPos->x, disappearPos->y, disappearPos->z );
  work->exit_type = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���Ȃʂ��̃q�� �j
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByAnanukenohimo( FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByAnanukenohimo, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetEscapeLocation( work->gameData ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���Ȃ��ق� )
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByAnawohoru( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByAnawohoru, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetEscapeLocation( work->gameData ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i �e���|�[�g )
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByTeleport( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;
  GAMEDATA* gameData;
  u16 warpID;

  // ���[�vID�擾
  gameData = GAMESYSTEM_GetGameData( gameSystem );
  warpID   = GAMEDATA_GetWarpID( gameData );

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByTeleport, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  WARPDATA_GetWarpLocation( warpID, &(work->loc_req) );
  LOCATION_SetDefaultPos( &(work->loc_req), work->loc_req.zone_id );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//----------------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i �C��_�a����̋����ޏ� )
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapBySeaTemple( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBySeaTemple, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetSpecialLocation( work->gameData ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���[�v )
 * @param gameSystem
 * @param fieldmap
 * @param zoneID 
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByWarp( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                u16 zoneID , const VecFx32 * pos, u16 dir )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByWarp, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem );
  dir = EXITDIR_fromDIR( dir );
  LOCATION_SetDirect( &(work->loc_req), zoneID, dir, pos->x, pos->y, pos->z ); 
  work->exit_type          = EXIT_TYPE_WARP;
  work->seasonUpdateEnable = FALSE;

  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapToUnion( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeToUnion, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDirect( &(work->loc_req), ZONE_ID_UNION, 
    EXITDIR_fromDIR(DIR_UP), NUM_FX32(184), NUM_FX32(0), NUM_FX32(248) ); 
  work->exit_type = EXIT_TYPE_NONE;
  
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapFromUnion( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;
  const LOCATION* spLoc;

  spLoc = GAMEDATA_GetSpecialLocation( GAMESYSTEM_GetGameData( gameSystem ) );

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeFromUnion, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req = *spLoc;

  return event;
} 

//------------------------------------------------------------------
/**
 * @brief �\�t�B�[���h����p���X�}�b�v�Ɉړ�����Ƃ�
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapFldToPalace( GAMESYS_WORK* gsys, u16 zone_id, const VecFx32* pos )
{
  GMEVENT * event;
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  VecFx32 calc_pos = *pos;
  
  //���t�B�[���h�ȊO����A�p���X�֔�ԏꍇ�ʏ�t�B�[���h�ւ̖߂����L�^���Ă���
  {
    LOCATION return_loc;
    setNowLoaction( &return_loc, fieldWork );
    GAMEDATA_SetPalaceReturnLocation(gamedata, &return_loc);
  }

  {
    LOCATION loc;
    int map_offset, palace_area;
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
    
    if(intcomm == NULL){
      palace_area = 0;
      map_offset = 0;
    }
    else{
      //�q�@�̏ꍇ�Apalace_area == 0 �����[�A�ł͂Ȃ�������NetID�̃p���X�����[�ׁ̈B
      palace_area = intcomm->intrude_status_mine.palace_area;
      map_offset = palace_area - GAMEDATA_GetIntrudeMyID(gamedata);
      if(map_offset < 0){
        map_offset = intcomm->member_num + map_offset;
      }
    }
    calc_pos.x += PALACE_MAP_LEN * map_offset;
    
    LOCATION_SetDirect( &loc, zone_id, EXITDIR_fromDIR( DIR_UP ), 
      calc_pos.x, calc_pos.y, calc_pos.z );
    event = EVENT_ChangeMapPalaceWithCheck( gsys, fieldWork, &loc );
  }
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �p���X���玩���̃t�B�[���h�ɖ߂�Ƃ�
 */
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapFromPalace( GAMESYS_WORK * gameSystem )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gameSystem);
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gameSystem );
#if 0  
  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem );
  //�o���Ă������߂������̂܂ܑ��
  work->loc_req = *(GAMEDATA_GetPalaceReturnLocation( gamedata ) );
  work->exit_type          = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = FALSE;
#else
  {
    LOCATION loc;
    //�o���Ă������߂������̂܂ܑ��
    loc = *(GAMEDATA_GetPalaceReturnLocation( gamedata ) );
    event = EVENT_ChangeMapPalace( gameSystem, fieldWork, &loc );
  }
#endif
  GAMEDATA_SetIntrudeReverseArea(gamedata, FALSE);
//  PMSND_PlaySE( SEQ_SE_FLD_131 ); //SE�̊m�F�p�ɃG�t�F�N�g�͖������ǂ��ĂĂ���

  return event;
}

//------------------------------------------------------------------
/// ���݈ʒu��LOCATION�ɃZ�b�g�iGRID/RAIL���Ή��j
//------------------------------------------------------------------
static void setNowLoaction(LOCATION * return_loc, FIELDMAP_WORK * fieldmap)
{
  FIELD_PLAYER * field_player = FIELDMAP_GetFieldPlayer( fieldmap );
  if ( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_GRID )
  {
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( field_player );
    const VecFx32 * now_pos = MMDL_GetVectorPosAddress( fmmdl );
    LOCATION_SetDirect( return_loc, FIELDMAP_GetZoneID(fieldmap), DIR_DOWN,
        now_pos->x, now_pos->y, now_pos->z );
  }
  else
  {
    RAIL_LOCATION rail_loc;
    FIELD_PLAYER_GetNoGridLocation( field_player, &rail_loc );
    LOCATION_SetDirectRail( return_loc,
        FIELDMAP_GetZoneID(fieldmap), EXIT_DIR_DOWN,
        rail_loc.rail_index, rail_loc.line_grid, rail_loc.width_grid);
  }
}
//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByConnect( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                   const CONNECT_DATA* connectData, LOC_EXIT_OFS exitOfs )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  MAPCHANGE_WORK_init( work, gameSystem );
  work->seasonUpdateEnable = TRUE;

  if( CONNECTDATA_IsSpecialExit( connectData ) )
  {
    //����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
    const LOCATION * sp = GAMEDATA_GetSpecialLocation( gameData );
    work->loc_req = *sp;
  }
  else
  {
    CONNECTDATA_SetNextLocation( connectData, &(work->loc_req), exitOfs );
  }
  work->exit_type = CONNECTDATA_GetExitType( connectData );

  {
    //�t�B�[���h�������ւ̈ړ��̍ۂ͒E�o��ʒu���L�����Ă���
    const LOCATION* ent = GAMEDATA_GetEntranceLocation( gameData );
    if( ZONEDATA_IsFieldMatrixID(ent->zone_id) == TRUE
        && ZONEDATA_IsFieldMatrixID(work->loc_req.zone_id) == FALSE )
    {
      GAMEDATA_SetEscapeLocation( gameData, ent );
    }
  }
  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT* EVENT_ChangeMapPosNoFadeCore( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                              EV_MAPCHG_TYPE type, u16 zoneID, const VecFx32* pos, u16 dir )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create(gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK));
  work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  dir = EXITDIR_fromDIR( dir );
  LOCATION_SetDirect( &(work->loc_req), zoneID, dir, pos->x, pos->y, pos->z );
  work->exit_type      = EXIT_TYPE_NONE;
  work->mapchange_type = type; 

  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapPosNoFade( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                   u16 zoneID, const VecFx32* pos, u16 dir )
{
  return EVENT_ChangeMapPosNoFadeCore( gameSystem, fieldmap, EV_MAPCHG_NORMAL, zoneID, pos, dir );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapSorawotobu( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                    u16 zoneID, const LOCATION* loc, u16 dir )
{
//  return EVENT_ChangeMapPosNoFadeCore( gameSystem, fieldmap, EV_MAPCHG_FLYSKY, zoneID, pos, dir );  
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create(gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK));
  work  = GMEVENT_GetEventWork(event);

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  //���P�[�V�����͗p�ӂ��Ă���̂ŃR�s�[����
  work->loc_req = *loc;
  //���������Z�b�g
  work->loc_req.dir_id = dir;
  work->exit_type      = EXIT_TYPE_NONE;
  work->mapchange_type = EV_MAPCHG_FLYSKY; 
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� (BGM�ύX�Ȃ�)
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap   �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zoneID     �J�ڂ���}�b�v��ZONE�w��
 * @param pos        �J�ڂ���}�b�v�ł̍��W�w��
 * @param dir        �J�ڂ���}�b�v�ł̕����w��
 * @return GMEVENT  ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapBGMKeep( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                  u16 zoneID, const VecFx32* pos, u16 dir )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBGMKeep, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  dir = EXITDIR_fromDIR( dir );
  LOCATION_SetDirect( &(work->loc_req), zoneID, dir, pos->x, pos->y, pos->z );
  work->exit_type = EXIT_TYPE_NONE;
  
  return event;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� (�p���X�ԃ��[�v(�����痠))
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap   �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zoneID     �J�ڂ���}�b�v��ZONE�w��
 * @param pos        �J�ڂ���}�b�v�ł̍��W�w��
 * @param dir        �J�ڂ���}�b�v�ł̕����w��
 * @return GMEVENT  ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapPalace_to_Palace( GAMESYS_WORK* gameSystem, u16 zoneID, const VecFx32* pos )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;
  VecFx32 calc_pos = *pos;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gameSystem);
  
  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangePalace_to_Palace, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  if(ZONEDATA_IsPalace(zoneID) == TRUE){
    int map_offset, palace_area;
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSystem);
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
    
    if(intcomm == NULL){
      palace_area = 0;
      map_offset = 0;
    }
    else{
      //�q�@�̏ꍇ�Apalace_area == 0 �����[�A�ł͂Ȃ�������NetID�̃p���X�����[�ׁ̈B
      palace_area = intcomm->intrude_status_mine.palace_area;
      map_offset = palace_area - GAMEDATA_GetIntrudeMyID(gamedata);
      if(map_offset < 0){
        map_offset = intcomm->member_num + map_offset;
      }
    }
    calc_pos.x += PALACE_MAP_LEN * map_offset;
  }

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDirect(&(work->loc_req), zoneID, EXITDIR_fromDIR( DIR_UP ), 
    calc_pos.x, calc_pos.y, calc_pos.z );
  work->exit_type = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = FALSE;
  
  return event;
}



//------------------------------------------------------------------
//  ���}�b�vID���C���N�������g���Ă���B�ő�l�ɂȂ�����擪�ɖ߂�
//------------------------------------------------------------------
GMEVENT* DEBUG_EVENT_ChangeToNextMap( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);
  PLAYER_WORK* myplayer = GAMEDATA_GetMyPlayerWork(gamedata);
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
//-----------------------------------------------------------------------------
/**
 *      �S�Ŏ��̏���
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	MAPCHANGE_GAMEOVER
//=====================================
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  LOCATION loc_req;
} MAPCHANGE_GAMEOVER;


//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��̃}�b�v�J�ڏ����i�t�B�[���h�񐶐����j
 */
//--------------------------------------------------------------
static void MAPCHG_GameOver( GAMESYS_WORK * gsys )
{
  LOCATION loc_req;
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData( gsys );
  u16 warp_id = GAMEDATA_GetWarpID( gamedata );

  //�����|�C���g���擾
  WARPDATA_GetRevivalLocation( warp_id, &loc_req );

  //�G�X�P�[�v�|�C���g���Đݒ�
  {
    LOCATION esc;
    u16 warp_id = GAMEDATA_GetWarpID( gamedata );
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


//----------------------------------------------------------------------------
/**
 *	@brief    �Q�[���I�[�o�[�C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameOver(GMEVENT * event, int * seq, void *work)
{
  MAPCHANGE_GAMEOVER* p_wk = work;

  switch( *seq )
  {
  // �J�ڐ��ݒ�
  case 0:
    MAPCHG_GameOver( p_wk->gsys );
    (*seq) ++;
    break;

  // BGM �Đ��J�n
  case 1: 
    {
      u32 soundIdx = FSND_GetFieldBGM( p_wk->gamedata, p_wk->loc_req.zone_id );
      GMEVENT_CallEvent( event, 
          EVENT_FSND_ChangeBGM( p_wk->gsys, soundIdx, FSND_FADE_NONE, FSND_FADE_NORMAL ) );
    }
    (*seq) ++;
    break;

  // �t�B�[���h�}�b�v�J�n
  case 2:
    GMEVENT_CallEvent( event, EVENT_FieldOpen_FieldProcOnly(p_wk->gsys) );
    (*seq) ++;
    break;

  case 3:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �S�Ŏ��̃}�b�v�J�ڏ���
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_CallGameOver( GAMESYS_WORK * gsys )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
	GMEVENT * event;
  MAPCHANGE_GAMEOVER* p_wk;
  event = GMEVENT_Create( gsys, NULL, GMEVENT_GameOver, sizeof(MAPCHANGE_GAMEOVER) );
  p_wk = GMEVENT_GetEventWork( event );
  p_wk->gsys = gsys;
  p_wk->gamedata = GAMESYSTEM_GetGameData( gsys );

  //�����|�C���g���擾
  {
    u16 warp_id = GAMEDATA_GetWarpID( p_wk->gamedata );
    WARPDATA_GetRevivalLocation( warp_id, &p_wk->loc_req );
  }

  return event;
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
    LOCATION_SetDefaultPos(loc_tmp, loc_req->zone_id);
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

  // �V�C�X�V
  PM_WEATHER_UpdateZoneChangeWeatherNo( gsys, loc.zone_id );
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
    const FIELD_STATUS* cp_fs = GAMEDATA_GetFieldStatus( gamedata );
    MAPMODE mapmode = FIELD_STATUS_GetMapMode( cp_fs );
    MMDLSYS *fmmdlsys = GAMEDATA_GetMMdlSys( gamedata );
    EVENTWORK *evwork =  GAMEDATA_GetEventWork( gamedata );
    MMDL_HEADER*p_header;
    const FIELD_WFBC_CORE* cp_wfbc;
    u32  count;

    if( mapmode == MAPMODE_NORMAL )
    {
      cp_wfbc = GAMEDATA_GetMyWFBCCoreData( gamedata );
    }
    else
    {
      // �ʐM����̃f�[�^�ɕύX
      cp_wfbc = GAMEDATA_GetWFBCCoreData( gamedata, GAMEDATA_WFBC_ID_COMM );
    }

    // �l��
    {
      p_header = FIELD_WFBC_CORE_MMDLHeaderCreateHeapLo( cp_wfbc, mapmode, GFL_HEAPID_APP );

      count = FIELD_WFBC_CORE_GetPeopleNum( cp_wfbc, mapmode );


      // 
      if( p_header && (count > 0) )
      {
        TOMOYA_Printf( "WFBC MMDL SetUp\n" );
        MMDLSYS_SetMMdl( fmmdlsys, p_header, loc->zone_id, count, evwork );
      }
      if( p_header ){
        GFL_HEAP_FreeMemory( p_header );
      }
    }

    // �A�C�e��
    {
      const FIELD_WFBC_CORE_ITEM* cp_item;

      cp_item = GAMEDATA_GetWFBCItemData( gamedata );

      // �A�C�e�����擾
      count = WFBC_CORE_ITEM_GetNum( cp_item );

      // �z�u���f���쐬
      p_header = FIELD_WFBC_CORE_ITEM_MMDLHeaderCreateHeapLo( cp_item, mapmode, cp_wfbc->type, GFL_HEAPID_APP );
      
      // 
      if( p_header && (count > 0) )
      {
        TOMOYA_Printf( "WFBC ITEM SetUp\n" );
        MMDLSYS_SetMMdl( fmmdlsys, p_header, loc->zone_id, count, evwork );
      }
      if( p_header ){
        GFL_HEAP_FreeMemory( p_header );
      }
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
//  if(ZONEDATA_IsPalaceField(loc_req->zone_id) || ZONEDATA_IsBingo(loc_req->zone_id)){
  if(GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE 
      && ZONEDATA_IsPalace(loc_req->zone_id) == FALSE){ //�p���X���ł͔����ɂ��Ȃ�
    FIELD_STATUS_SetMapMode( GAMEDATA_GetFieldStatus( gamedata ), MAPMODE_INTRUDE );
    //GAMEDATA_SetMapMode(gamedata, MAPMODE_INTRUDE);
  }
  else{
    FIELD_STATUS_SetMapMode( GAMEDATA_GetFieldStatus( gamedata ), MAPMODE_NORMAL );
  }
  //��check�@���j�I�����[���ւ̈ړ�����t�X�N���v�g�Ő��䂷��悤�ɂȂ�����T�u�X�N���[�����[�h��
  //         �ύX�����̃X�N���v�g���ōs���悤�ɂ���
  //         �~���[�W�J���T�����������悤�ɏ�������Ari100329
  switch(loc_req->zone_id){
  case ZONE_ID_UNION:
    GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_UNION);
    break;
  case ZONE_ID_CLOSSEUM:
  case ZONE_ID_CLOSSEUM02:
  case ZONE_ID_C04R0202:
    GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_NOGEAR);
    break;
  default:
    {// �i�������C�u�`���b�g�͂��̂܂� ���̑��̉�ʂ͐؂�ւ�
      u8 mode = GAMEDATA_GetSubScreenMode(gamedata);
      if((mode != FIELD_SUBSCREEN_INTRUDE) && (mode != FIELD_SUBSCREEN_BEACON_VIEW)){
        GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_NORMAL);
      }
    }
    break;
  }

  //�C�x���g�N���f�[�^�̓ǂݍ���
  EVENTDATA_SYS_Load(evdata, loc_req->zone_id, GAMEDATA_GetSeasonID(gamedata) );

  // �C�x���g�ǉ��o�^

  //�}�g���b�N�X�f�[�^�̍X�V
  {
    MAP_MATRIX * matrix = GAMEDATA_GetMapMatrix( gamedata );
    u16 matID = ZONEDATA_GetMatrixID( loc_req->zone_id );
    MAP_MATRIX_Init( matrix, matID, loc_req->zone_id, GFL_HEAP_LOWID(GFL_HEAPID_APP) );
    MAP_MATRIX_CheckReplace( matrix, gsys, GFL_HEAP_LOWID(GFL_HEAPID_APP) );
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
    u16 Assign;
    u16 ObjNum;
  }GMK_ASSIGN_DATA;

  GIMMICKWORK *work;
  //�M�~�b�N���[�N�擾
  work = GAMEDATA_GetGimmickWork(gamedata);

  //�}�b�v�W�����v�̂Ƃ��̂݃��[�N�N���A����i�����ă]�[�����؂�ւ�����ꍇ�͏��������Ȃ��j
  GIMMICKWORK_ClearWork(work);

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
        GIMMICKWORK_Assign(work, data[i].Assign);
        break;
      }
    }

    GFL_HEAP_FreeMemory( data );
    GFL_ARC_CloseDataHandle( handle );
  }
}

static GMEVENT * EVENT_ChangeMapPalaceWithCheck( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc)
{
  PALACE_JUMP* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangePalaceWithCheck, sizeof(PALACE_JUMP) );
  
  work  = GMEVENT_GetEventWork( event );
  // �C�x���g���[�N������
  work->Loc = *loc;
  work->gameSystem = gameSystem;
  work->gameData = GAMESYSTEM_GetGameData( gameSystem );
  work->fieldmap = fieldmap;

  return event;
}


static GMEVENT * EVENT_ChangeMapPalace( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc)
{
  PALACE_JUMP* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangePalace, sizeof(PALACE_JUMP) );
  
  work  = GMEVENT_GetEventWork( event );
  // �C�x���g���[�N������
  work->Loc = *loc;
  work->gameSystem = gameSystem;
  work->gameData = GAMESYSTEM_GetGameData( gameSystem );
  work->fieldmap = fieldmap;

  return event;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangePalace( GMEVENT* event, int* seq, void* wk )
{
  PALACE_JUMP* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;

  switch(*seq)
  {
  case 0:
    //�r�d
    PMSND_PlaySE( SEQ_SE_FLD_131 );
    //�G�t�F�N�g�R�[��
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(fieldmap), event, ENCEFFID_PALACE_WARP);
    (*seq)++;
    break;
  case 1:
    {
      MAPCHANGE_WORK* mc_work;
      GMEVENT* call_event;
      call_event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK) );
      mc_work  = GMEVENT_GetEventWork( call_event );
      // �C�x���g���[�N������
      MAPCHANGE_WORK_init( mc_work, gameSystem );
      //���P�[�V�����Z�b�g
      mc_work->loc_req = work->Loc;
//      LOCATION_SetDirect( &(mc_work->loc_req), work->ZoneID, work->Dir, work->Pos.x, work->Pos.y, work->Pos.z ); 
      mc_work->exit_type          = EXIT_TYPE_NONE;
      mc_work->seasonUpdateEnable = FALSE;
      //�C�x���g�R�[��
      GMEVENT_CallEvent( event, call_event );
    }
    (*seq)++;
    break;
  case 2:
    //�u���b�N�C�����N�G�X�g�@������Ƃԉ��o���p
    GMEVENT_CallEvent(event, EVENT_FlySkyBrightIn(gameSystem, fieldmap, FIELD_FADE_BLACK, FIELD_FADE_WAIT, 2));
    (*seq)++;
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//�p���X�ɂ�����邩���`�F�b�N���āA�n�j�Ȃ������C�x���g
static GMEVENT_RESULT EVENT_MapChangePalaceWithCheck( GMEVENT* event, int* seq, void* wk )
{
  PALACE_JUMP* work = (PALACE_JUMP*)wk;

  switch(*seq){
  case 0:
    //�i���\���W�`�F�b�N
    if ( PLC_WP_CHK_Check(work->gameSystem) )
    {   //�i���\
      //�i���\���b�Z�[�W�R�[��
      SCRIPT_CallScript( event, SCRID_FLD_EV_WARP_SUCCESS, NULL, NULL, FIELDMAP_GetHeapID( work->fieldmap ) );
      //�}�b�v�`�F���W�C�x���g�ύX�V�[�P���X��
      (*seq) = 1;
    }
    else
    {     //�i���s�\
      //�i���s�\���b�Z�[�W�R�[��
      SCRIPT_CallScript( event, SCRID_FLD_EV_WARP_FAILED, NULL, NULL, FIELDMAP_GetHeapID( work->fieldmap ) );
      //�I���V�[�P���X��
      (*seq) = 2;
    }
    break;
  case 1:
    {
      GMEVENT* call_event;
      GAMEDATA_SetIntrudeReverseArea(work->gameData, TRUE);
      call_event = EVENT_ChangeMapPalace( work->gameSystem, work->fieldmap, &work->Loc );
      GMEVENT_ChangeEvent(event, call_event);
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


