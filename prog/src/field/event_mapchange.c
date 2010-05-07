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
#include "event_demo3d.h"
#include "savedata/gimmickwork.h"           //for GIMMICKWORK
#include "field_comm/intrude_main.h"
#include "field_comm/intrude_work.h"
#include "field_comm/intrude_mission.h"
#include "field_comm/intrude_comm_command.h"
#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN

#include "field_task.h"
#include "field_task_manager.h"

#include "net_app/union/union_main.h" // for UNION_CommBoot
#include "savedata/intrude_save.h"

#include "system/playtime_ctrl.h" // for PLAYTIMECTRL_Start
#include "savedata/gametime.h"    // for GMTIME
#include "savedata/system_data.h" //SYSTEMDATA_
#include "savedata/mail_util.h"   // MAIL_BLOCK
#include "arc/message.naix"       // ���b�Z�[�W�f�[�^�Q�Ɨp
#include "msg/msg_trname.h"       // ������support_01
#include "gamesystem/pm_season.h" // for PMSEASON_TOTAL
#include "ev_time.h"              // EVTIME_Update
#include "../../../resource/fldmapdata/flagwork/flag_define.h"    // SYS_FLAG_SPEXIT_REQUEST
#include "../../../resource/fldmapdata/flagwork/work_define.h"    // WK_SYS_SCENE_COMM 
#include "../../../resource/fldmapdata/script/pokecen_scr_def.h"  // SCRID_POKECEN_ELEVATOR_OUT
#include "sheimi_normalform.h"    //for SHEIMI_NFORM_ChangeNormal

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
static GMEVENT* EVENT_ContinueMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork );

static GMEVENT_RESULT EVENT_MapChangePalaceWithCheck( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_MapChangePalace( GMEVENT* event, int* seq, void* wk );
static GMEVENT * EVENT_ChangeMapPalaceWithCheck( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc);
static GMEVENT * EVENT_ChangeMapPalace( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc);

static void setNowLoaction(LOCATION * return_loc, FIELDMAP_WORK * fieldmap);

static void Escape_SetSPEscapeLocation( GAMEDATA* gamedata, const LOCATION* loc_req );
static void Escape_GetSPEscapeLocation( const GAMEDATA* gamedata, LOCATION* loc_req );

static void MapChange_SetPlayerMoveFormNormal( GAMEDATA* gamedata );

static void MailBox_SetFirstMail( GAMEDATA* gamedata );

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(intrude_system);
FS_EXTERN_OVERLAY(union_system);


//============================================================================================
//
//  �C�x���g�F�Q�[���J�n
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n����
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
    event = EVENT_ContinueMapIn( gameSystem, gameInitWork );
    break;
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
//  �u�������傩��v�̗���
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n���o�C�x���g�p���[�N��`
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK*   gameSystem;
  GAME_INIT_WORK* gameInitWork;
} FIRST_START_WORK;

//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n���o�C�x���g
 */
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
    // �G�߂�����������
    {
      int season = PMSEASON_GetRealTimeSeasonID();
      PMSEASON_SetSeasonID( gameData, season );
    }
    // �G�߂�\������
    {
      u8 season = GAMEDATA_GetSeasonID( gameData );
      GMEVENT_CallEvent( event, EVENT_SimpleSeasonDisplay( gameSystem, season, season ) );
    }
    (*seq)++;
    break;

  case 2:
    // �f�����Ă�
    GMEVENT_CallEvent( event, EVENT_CallDemo3D( gameSystem, event, DEMO3D_ID_INTRO_TOWN, 0, TRUE ));
    (*seq)++;
    break;

  case 3:
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
 * @brief �u�������傩��v�ł̃Q�[���J�n���o�C�x���g����
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

//============================================================================================
//
//    �C�x���g�F�V�K�Q�[���J�n
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �V�K�Q�[���J�n�C�x���g�p���[�N��`
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  LOCATION loc_req;
#ifdef  PM_DEBUG
  BOOL isDebugModeFlag;   ///<�f�o�b�O���[�h�ŊJ�n���ǂ���
#endif

}FIRST_MAPIN_WORK;

//------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h�̐V�K�J�n����������
 *
 *  @param  gsys  �Q�[���V�X�e��
 *
 * @note
 *  GAME_DATA �̏��ŁA�V�K�Q�[���J�n���ɍs�������������L�q���Ă��������B
 *
 *  ���̃^�C�~���O�ŌĂ΂�鏈���́AOVERLAY�@FIELD_INIT�ɔz�u���Ă��������B
 */
//------------------------------------------------------------------
static void GAME_FieldFirstInit( GAMESYS_WORK * gsys )
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);

  // WFBC�X�����̏ꏊ������
  FIELD_WFBC_CORE_SetUp( GAMEDATA_GetMyWFBCCoreData(gamedata), GAMEDATA_GetMyStatus(gamedata), GFL_HEAPID_APP ); //heap �̓e���|�����p
  FIELD_WFBC_CORE_SetUpZoneData( GAMEDATA_GetMyWFBCCoreData(gamedata) );
}

//------------------------------------------------------------------
/**
 * @brief �V�K�Q�[���J�n�C�x���g
 */
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
#ifdef  PM_DEBUG
    if ( fmw->isDebugModeFlag ) {
      SCRIPT_CallDebugGameStartInitScript( gsys, GFL_HEAPID_APP );
    } else {
      SCRIPT_CallGameStartInitScript( gsys, GFL_HEAPID_APP );
    }
#else
    SCRIPT_CallGameStartInitScript( gsys, GFL_HEAPID_APP );
#endif
    FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
    MAPCHG_setupMapTools( gsys, &fmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
    MAPCHG_updateGameData( gsys, &fmw->loc_req ); //�V�����}�b�vID�A�����ʒu���Z�b�g

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
    //�t�B�[���h�}�b�v���J�n�҂�
    GMEVENT_CallEvent(event, EVENT_FieldOpen_FieldProcOnly(gsys));
    (*seq)++;
    break;

  case 3:
    fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
    // ��ʃt�F�[�h�C��
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black( gsys, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  case 4:
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

//------------------------------------------------------------------
/**
 * @brief �V�K�Q�[���J�n�C�x���g����
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FirstMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  FIRST_MAPIN_WORK* fmw;

  // �C�x���g�𐶐�����
  event = GMEVENT_Create( gameSystem, NULL, FirstMapInEvent, sizeof(FIRST_MAPIN_WORK) );
  fmw  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N��������
  fmw->gsys = gameSystem;
  fmw->gamedata   = GAMESYSTEM_GetGameData(gameSystem);
  LOCATION_SetGameStart(&fmw->loc_req);

#ifdef PM_DEBUG
  fmw->isDebugModeFlag = FALSE;
  if ( gameInitWork->mode == GAMEINIT_MODE_DEBUG )
  {
    fmw->isDebugModeFlag = TRUE;
    //�K���Ɏ莝���|�P������Add
    //�f�o�b�O�A�C�e���ǉ��ȂǂȂǁc
    GFL_OVERLAY_Load( FS_OVERLAY_ID(debug_data));
    DEBUG_SetStartData( GAMESYSTEM_GetGameData( gameSystem ), GFL_HEAPID_APP );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(debug_data));
    
    LOCATION_SetDefaultPos(&fmw->loc_req, gameInitWork->mapid);

#ifdef DEBUG_ONLY_FOR_iwasawa
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
       DEBUG_FLG_FlgOn( DEBUG_FLG_ShortcutBtlIn );
    }
#endif
  }
#endif //PM_DEBUG

  GAME_FieldFirstInit( gameSystem );  // �t�B�[���h���̏�����
  //DS�{�̏��̃Z�b�g
  {
    SAVE_CONTROL_WORK * svdt = GAMEDATA_GetSaveControlWork( fmw->gamedata );
    SYSTEMDATA * sysdt = SaveData_GetSystemData( svdt );
    SYSTEMDATA_Update( sysdt );
  }
  //�v���C���ԃJ�E���g�@�J�n
  PLAYTIMECTRL_Start();

  //���[���{�b�N�X�ɍŏ����烁�[������ʓ���Ă���
  MailBox_SetFirstMail( fmw->gamedata );

  // VRAM�S�N���A
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  return event;
}

//============================================================================================
//
//  �C�x���g�F�R���e�B�j���[
//
//============================================================================================
//------------------------------------------------------------------
/**
 * �Q�[���R���e�B�j���[�C�x���g�p���[�N��`
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  LOCATION loc_req;
  BOOL sp_exit_flag;    ///<����ڑ��ŊJ�n���邩�ǂ����̔���t���O
}CONTINUE_MAPIN_WORK;

//------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h�̃R���e�B�j���[�J�n����������
 *
 *  @param  gsys  �Q�[���V�X�e��
 *
 * @note
 *  GAME_DATA �̏��ŁA�V�K�Q�[���J�n���ɍs�������������L�q���Ă��������B
 *
 *  ���̃^�C�~���O�ŌĂ΂�鏈���́AOVERLAY�@FIELD_INIT�ɔz�u���Ă��������B
 */
//------------------------------------------------------------------
static void GAME_FieldContinueInit( GAMESYS_WORK * gsys )
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);

  // WFBC��ZONE������������
  FIELD_WFBC_CORE_SetUpZoneData( GAMEDATA_GetMyWFBCCoreData(gamedata) );
}

//------------------------------------------------------------------
/**
 * �Q�[���R���e�B�j���[�C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT ContinueMapInEvent(GMEVENT * event, int *seq, void *work)
{
  CONTINUE_MAPIN_WORK * cmw = work;
  GAMESYS_WORK * gsys = cmw->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENTWORK * ev = GAMEDATA_GetEventWork( gamedata );
  FIELDMAP_WORK * fieldmap;
  switch (*seq) {
  case 0:
    FIELD_STATUS_SetContinueFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
    if ( cmw->sp_exit_flag )
    { //����ڑ����N�G�X�g�F�����Ȃ�ʃ}�b�v�ɑJ�ڂ���
      const LOCATION * spLoc = GAMEDATA_GetSpecialLocation( gamedata );
      cmw->loc_req = *spLoc;
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
      MAPCHG_releaseMapTools( gsys ); //���샂�f���̊J���Ȃ�
      MAPCHG_setupMapTools( gsys, &cmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
      MAPCHG_updateGameData( gsys, &cmw->loc_req ); //�V�����}�b�vID�A�����ʒu���Z�b�g
    }
    else
    {
      MAPCHG_setupMapTools( gsys, &cmw->loc_req ); //�V�����}�b�v���[�h�Ȃǋ@�\�w����s��
      EVTIME_Update( gamedata ); //�C�x���g���ԍX�V
      FIELD_FLAGCONT_INIT_Continue( gamedata, cmw->loc_req.zone_id ); //�R���e�B�j���[�ɂ��t���O���Ƃ�����
      //�V�C���[�h����
      //**���FIELD_FLAGCONT_INIT_Continue�ňړ��|�P�����̓V�C�����I�����̂ŁA
      //�K���A���̉��ōs���B**
      PM_WEATHER_UpdateSaveLoadWeatherNo( gamedata, cmw->loc_req.zone_id );
    }
    (*seq)++;
    break;

  case 1:
    // BGM �t�F�[�h�C��
    {
      u32 soundIdx = FSND_GetFieldBGM( gamedata, cmw->loc_req.zone_id );
      GMEVENT_CallEvent( event, EVENT_FSND_ChangeBGM( gsys, soundIdx, FSND_FADE_NONE, FSND_FADE_NORMAL ) );
    }
    (*seq)++;
    break;

  case 2:
    //�t�B�[���h�}�b�v���J�n�҂�
    GMEVENT_CallEvent(event, EVENT_FieldOpen_FieldProcOnly(gsys));
    (*seq)++;
    break;

  case 3:
    if(  cmw->sp_exit_flag && *(EVENTWORK_GetEventWorkAdrs( ev, WK_SYS_SCENE_COMM )) != 0 )
    {
      //�|�P�Z���G���x�[�^���o�ōĊJ
      SCRIPT_CallScript( event, SCRID_POKECEN_ELEVATOR_OUT_CONTINUE, NULL, NULL, HEAPID_FIELDMAP );
    }
    else
    {
      // ��ʃt�F�[�h�C��
      // �Q�[���I�����̋G�߂�\������
      u8 season = GAMEDATA_GetSeasonID( gamedata );
      fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
      GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Season( gsys, fieldmap, season, season ) );
    }
    (*seq)++;
    break;

  case 4:
    // �n����\������
    fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
    if( FIELDMAP_GetPlaceNameSys(fieldmap) )
    {
      FIELD_PLACE_NAME_DisplayForce( FIELDMAP_GetPlaceNameSys(fieldmap), cmw->loc_req.zone_id );
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �Q�[���R���e�B�j���[�C�x���g����
 *
 * @todo  LOCATION_SetDirect���g�p���Ă��Ė��Ȃ�����������
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_ContinueMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  CONTINUE_MAPIN_WORK* cmw;
  EVENTWORK * ev;

  // �C�x���g�𐶐�����
  event = GMEVENT_Create( gameSystem, NULL, ContinueMapInEvent, sizeof(CONTINUE_MAPIN_WORK) );
  cmw  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N��������
  cmw->gsys = gameSystem;
  cmw->gamedata   = GAMESYSTEM_GetGameData(gameSystem);
  LOCATION_SetDirect(&cmw->loc_req, gameInitWork->mapid, gameInitWork->dir, 
    gameInitWork->pos.x, gameInitWork->pos.y, gameInitWork->pos.z);
  ev = GAMEDATA_GetEventWork( cmw->gamedata );
  cmw->sp_exit_flag = EVENTWORK_CheckEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST );
  EVENTWORK_ResetEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST );

  //���ԏ��������ɂ��y�i���e�B�`�F�b�N
  {
    SAVE_CONTROL_WORK * svdt = GAMEDATA_GetSaveControlWork(cmw->gamedata );
    SYSTEMDATA * sysdt = SaveData_GetSystemData( svdt );
    if (!SYSTEMDATA_IdentifyMACAddress(sysdt) || !SYSTEMDATA_IdentifyRTCOffset(sysdt) )
    {
      //�y�i���e�B���Ԃ�ݒ肷��
      GMTIME_SetPenaltyTime( SaveData_GetGameTime(svdt) );
      //�V�F�C�~�̃t�H������߂�
      {
        POKEPARTY *ppt;
        ppt = SaveData_GetTemotiPokemon(svdt);
        SHEIMI_NFORM_ChangeNormal(ppt);
      }
    }
    //���݂�DS�{�̏�������̐ݒ�Ƃ���
    SYSTEMDATA_Update( sysdt );
  }

  // �R���e�B�j���[�ݒ�
  GAME_FieldContinueInit( gameSystem );


  //�v���C���ԃJ�E���g�@�J�n
  PLAYTIMECTRL_Start();

  // VRAM�S�N���A
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  return event;
}


//============================================================================================
//
//    �C�x���g�F�Q�[���I��
//
//============================================================================================
#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief �Q�[���I���C�x���g�p���[�N
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
}GAME_END_WORK;

//------------------------------------------------------------------
/**
 * @brief �Q�[���I���C�x���g
 */
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
 * @brief �f�o�b�O�p�F�Q�[���I���C�x���g����
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
#endif

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
static void JudgeSeasonUpdateOccur( MAPCHANGE_WORK* work )
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
  if( (nextSeason != prevSeason) && (nextZoneIsOutdoor) ) { 
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
      TOMOYA_Printf( "season %d\n", work->nextSeason );
      PMSEASON_SetSeasonID( gameData, work->nextSeason );
      //�G�߃|�P�����t�H�����`�F���W�����𖞂������̂ŁA�莝���̋G�߃|�P�������t�H�����`�F���W������
      {
        u8 season;
        POKEPARTY *party;
        season = GAMEDATA_GetSeasonID(gameData);
        party = GAMEDATA_GetMyPokemon(gameData);
        SEASONPOKE_FORM_ChangeForm(gameData, party, season);
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
    JudgeSeasonUpdateOccur( work );
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
    return GMEVENT_RES_CONTINUE_DIRECT;
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

//--------------------------------------------------------------
/**
 * �N���p��EVENT_MapChange
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeIntrude( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  GAMEDATA*       gameData   = work->gameData;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr( work->gameSystem );
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  enum{
    SEQ_INIT,
    SEQ_ENTRANCE_IN,
    SEQ_MAPCHANGE_CORE,
    SEQ_ENTRANCE_OUT,
    SEQ_FINISH,
    SEQ_WFBC_REQ,
    SEQ_WFBC_WAIT,
    SEQ_COMM_ERROR,
  };

  switch(*seq)
  {
  case SEQ_INIT:
    // ���샂�f���̈ړ����~�߂�
    {
      MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_PauseMoveProc( fmmdlsys );
    }

    if(ZONEDATA_IsWfbc(work->loc_req.zone_id) == TRUE){
      *seq = SEQ_WFBC_REQ;
    }
    else{
      *seq = SEQ_ENTRANCE_IN;
    }
    break;
    
  case SEQ_ENTRANCE_IN:
    // �����i���C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceIn( event, gameSystem, gameData, fieldmap, 
                          work->loc_req, work->exit_type, work->seasonUpdateOccur ) );
    (*seq)++;
    return GMEVENT_RES_CONTINUE_DIRECT;
    break;
  case SEQ_MAPCHANGE_CORE:
    // �}�b�v�`�F���W�E�R�A�E�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case SEQ_ENTRANCE_OUT:
    // �����ޏo�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_EntranceOut( event, gameSystem, gameData, fieldmap, work->loc_req, work->before_zone_id,
                           work->seasonUpdateOccur, 
                           PMSEASON_GetNextSeasonID(work->prevSeason), work->nextSeason ) );
    (*seq)++;
    break;
  case SEQ_FINISH:
    return GMEVENT_RES_FINISH; 
  
  //------------------------ WFBC�v�� ---------------------------
  case SEQ_WFBC_REQ:
    if(intcomm == NULL){
      *seq = SEQ_COMM_ERROR;
    }
    else if(IntrudeSend_WfbcReq(intcomm, Intrude_GetPalaceArea(intcomm)) == TRUE){
      (*seq)++;
    }
    break;
  case SEQ_WFBC_WAIT:
    if(intcomm == NULL){
      *seq = SEQ_COMM_ERROR;
    }
    else if(Intrude_GetRecvWfbc(intcomm) == TRUE){
      *seq = SEQ_ENTRANCE_IN;
    }
    break;
  
  case SEQ_COMM_ERROR:
    // ���샂�f���̈ړ����ĊJ
    {
      MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_ClearPauseMoveProc( fmmdlsys );
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

#ifdef  PM_DEBUG
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
#endif

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

  switch( *seq ) {
  case 0:
    // �G�ߍX�V�̗L��������
    JudgeSeasonUpdateOccur( work );

    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // ��ʃt�F�[�h�A�E�g
  case 1:
    // �G�߃t�F�[�h
    if( work->seasonUpdateEnable && work->seasonUpdateOccur ) {
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    }
    // ��{�t�F�[�h
    else {
      FIELD_FADE_TYPE fadeOutType;

      fadeOutType = 
        FIELD_FADE_GetFadeOutType( work->before_zone_id, work->loc_req.zone_id );

      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut( gameSystem, fieldmap, fadeOutType, FIELD_FADE_WAIT ) );
    }
    (*seq)++;
    break;

  // �}�b�v�`�F���W �R�A �C�x���g
  case 2:
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;

  // ��ʃt�F�[�h�C��
  case 3:
    // �G�߃t�F�[�h
    if( work->seasonUpdateEnable && work->seasonUpdateOccur ) {
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->prevSeason, work->nextSeason ) );
    }
    // ��{�t�F�[�h
    else {
      FIELD_FADE_TYPE fadeOutType;

      fadeOutType = 
        FIELD_FADE_GetFadeInType( work->before_zone_id, work->loc_req.zone_id );

      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn( gameSystem, fieldmap, fadeOutType, FIELD_FADE_WAIT, TRUE, 0, 0 ) );
    }
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
  enum{
    _SEQ_OBJPAUSE,
    _SEQ_FIELD_CLOSE = 8, //����ʂ̉��o��������ׁA���O�̃V�[�P���X����Ԃ��󂯂�
    _SEQ_BGM_CHANGE,
    _SEQ_MAPCHG_CORE,
    _SEQ_FIELD_OPEN,
    _SEQ_FINISH,
  };
  
  switch( *seq )
  {
  case _SEQ_OBJPAUSE:
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case _SEQ_FIELD_CLOSE:
    // ��ʃt�F�[�h�A�E�g ( �N���X�t�F�[�h )
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case _SEQ_BGM_CHANGE:
    // BGM�ύX
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case _SEQ_MAPCHG_CORE:
    // �}�b�v�`�F���W �R�A �C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;
  case _SEQ_FIELD_OPEN:
    // ��ʃt�F�[�h�C�� ( �N���X�t�F�[�h )
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case _SEQ_FINISH:
    FIELD_PLACE_NAME_Display( FIELDMAP_GetPlaceNameSys(fieldmap), work->loc_req.zone_id );
    return GMEVENT_RES_FINISH; 
  default:
    (*seq)++;
    break;
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

  switch( *seq ) {
  case 0:
    // �G�ߍX�V�̗L��������
    JudgeSeasonUpdateOccur( work ); 
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  case 1: 
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_DISAPPEAR_Ananukenohimo( event, gameSystem, fieldmap, work->seasonUpdateOccur ) );
    (*seq)++;
    break;

  case 2:
    //���@�̃t�H�[����񑫕��s�ɖ߂�
    MapChange_SetPlayerMoveFormNormal( gameData );
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
    GMEVENT_CallEvent( event, 
        EVENT_APPEAR_Ananukenohimo( event, gameSystem, fieldmap, work->seasonUpdateOccur, work->prevSeason, work->nextSeason ) );
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

  switch( *seq ) {
  case 0:
    // �G�ߍX�V�̗L��������
    JudgeSeasonUpdateOccur( work ); 
    // ���샂�f����~
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq) ++;
    break;

  case 1: 
    // �ޏ�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_DISAPPEAR_Anawohoru( event, gameSystem, fieldmap, work->seasonUpdateOccur ) );
    (*seq)++;
    break;

  case 2:
    //���@�̃t�H�[����񑫕��s�ɖ߂�
    MapChange_SetPlayerMoveFormNormal( gameData );
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
    GMEVENT_CallEvent( event, 
        EVENT_APPEAR_Anawohoru( event, gameSystem, fieldmap, work->seasonUpdateOccur, work->prevSeason, work->nextSeason ) );
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
    //���@�̃t�H�[����񑫕��s�ɖ߂�
    {
      PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gameData);
      PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
    }
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
static GMEVENT_RESULT EVENT_MapChangeBySeaTempleUp( GMEVENT* event, int* seq, void* wk )
{
  MAPCHANGE_WORK* work       = wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_TASK_MAN* p_taskMan  = FIELDMAP_GetTaskManager( fieldmap );

  switch( *seq )
  {
  // �z���C�g�A�E�g
  case 0:
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_White( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;
  // ���샂�f����~
  case 1:
    //TOMOYA_Printf( "0000\n" );

    // �N���N������������̂�҂@event_seatemple.c���ŃN���N���ɂ��Ă���\��������܂��B
    if( !FIELD_TASK_MAN_IsAllTaskEnd( p_taskMan ) ) {
      break;
    }

    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // �}�b�v�`�F���W
  case 2:
    
    // �}�b�v�`�F���W �R�A�C�x���g
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;

  // �z���C�g�C��
  case 3:
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_White( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  // ����
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief �u�C��_�a�@����v�ɂ��}�b�v�`�F���W
 */
//------------------------------------------------------------------
static GMEVENT_RESULT EVENT_MapChangeBySeaTempleDown( GMEVENT* event, int* seq, void* wk )
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
  // �u���b�N�A�E�g
  case 1:
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Black( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  // �}�b�v�`�F���W BGM�ύX����
  case 2:
    {
      GMEVENT* sub_event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK) );
      MAPCHANGE_WORK* sub_work;

      sub_work  = GMEVENT_GetEventWork( sub_event );
      *sub_work  = *work; // �R�s�[
      // �}�b�v�`�F���W �C�x���g
      GMEVENT_CallEvent( event, sub_event );
      (*seq)++;
    }
    break;

  // �u���b�N�C��
  case 3:
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  // ����
  case 4:
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
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_system ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( intrude_system ) );
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
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( intrude_system ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( union_system ) );
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
GMEVENT* EVENT_ChangeMapRailLocation( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                            u16 zoneID, const RAIL_LOCATION* rail_loc, u16 dir, BOOL seasonUpdateEnable )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create(gameSystem, NULL, EVENT_MapChange, sizeof(MAPCHANGE_WORK));
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  dir = EXITDIR_fromDIR( dir );
  LOCATION_SetDirectRail( &(work->loc_req), zoneID, dir, 
                          rail_loc->rail_index, rail_loc->line_grid, rail_loc->width_grid);
  work->exit_type = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = seasonUpdateEnable;
  
  return event;
} 

#ifdef  PM_DEBUG
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
//------------------------------------------------------------------
GMEVENT * DEBUG_EVENT_QuickChangeMapAppoint( GAMESYS_WORK * gameSystem,
                                    FIELDMAP_WORK* fieldmap, u16 zoneID,
                                    const VecFx32 *pos )
{
  GMEVENT *event = DEBUG_EVENT_QuickChangeMapDefaultPos(
      gameSystem, fieldmap, zoneID );
  MAPCHANGE_WORK *work = GMEVENT_GetEventWork( event );
  work->loc_req.location_pos.pos = *pos;
  return event;
}
#endif

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
  work->loc_req = *(GAMEDATA_GetEscapeLocation( work->gameData ));
  Escape_GetSPEscapeLocation( work->gameData, &work->loc_req );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = TRUE; // �G�ߍX�V��L���ɂ���

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
  work->loc_req = *(GAMEDATA_GetEscapeLocation( work->gameData ));
  Escape_GetSPEscapeLocation( work->gameData, &work->loc_req );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = TRUE; // �G�ߍX�V��L���ɂ���

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
  u16 warp_zone_id;

  // ���[�vID�擾
  gameData = GAMESYSTEM_GetGameData( gameSystem );
  warpID   = GAMEDATA_GetWarpID( gameData );

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByTeleport, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  warp_zone_id = WARPDATA_GetWarpZoneID( warpID );
  LOCATION_SetDefaultPos( &(work->loc_req), warp_zone_id );
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
GMEVENT* EVENT_ChangeMapBySeaTempleUp( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBySeaTempleUp, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetSpecialLocation( work->gameData ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->loc_req.dir_id = EXIT_DIR_DOWN;  // �o�Ă����Ƃ��͉�������B
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//----------------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i �C��_�a�ւ����� )
 * @param gameSystem �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapBySeaTempleDown( GAMESYS_WORK* gameSystem, u16 zone_id )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBySeaTempleDown, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // �C�x���g���[�N������
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDefaultPos( &(work->loc_req), zone_id );
  work->exit_type    = EXIT_TYPE_NONE;
  work->loc_req.dir_id = EXIT_DIR_UP;  // �o�Ă����Ƃ��͏������B

  // ����ڑ���ۑ�
  {
    // ���@�ʒu��ۑ�
    LOCATION location;
    PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( work->gameData );
    const VecFx32 * vec = PLAYERWORK_getPosition( player );
    LOCATION_SetDirect( &location, PLAYERWORK_getZoneID( player ), 
        PLAYERWORK_getDirection_Type( player ), vec->x, vec->y, vec->z );

    GAMEDATA_SetSpecialLocation( work->gameData, &location );
  }

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

    //�p���X�؍ݎ��ԗp�ɓ������Ԃ�Push
    ISC_SAVE_PalaceSojournParam(
      SaveData_GetIntrude(GAMEDATA_GetSaveControlWork(gamedata)), 
      GAMEDATA_GetPlayTimeWork(gamedata), SOJOURN_TIME_PUSH);
  }

  {
    LOCATION loc;
    int map_offset, palace_area;
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
    
    //��ʐM or �`���[�g���A��������(���̌�ʐMOFF�ɂ���)�ꍇ�͍��[(�G���A0)
    if(intcomm == NULL || Intrude_CheckTutorialComplete(gamedata) == FALSE){
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

  //�p���X�؍ݎ��Ԃ��X�V
  ISC_SAVE_PalaceSojournParam(
    SaveData_GetIntrude(GAMEDATA_GetSaveControlWork(gamedata)), 
    GAMEDATA_GetPlayTimeWork(gamedata), SOJOURN_TIME_CALC_SET);

  //LAST_STATUS���N���A���Ă����@���������ŏ��̑ގ��҂̏ꍇ�̓X�e�[�^�X���c���Ă���
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSystem);
    GAME_COMM_LAST_STATUS last_status = GameCommSys_GetLastStatus(game_comm);
    //���̒ʐM���N�����Ă���ꍇ�͋N�����ɃN���A����Ă���̂ł悢
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
      GameCommSys_ClearLastStatus(game_comm);
    }
  }
  
  {
    FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    if(FIELD_PLAYER_CheckIllegalOBJCode( fld_player ) == FALSE){
      GF_ASSERT(0); //�ޏo���鎞�Ɏ��@�ȊO�ɂȂ��Ă���̂͂�������
      IntrudeField_PlayerDisguise(NULL, gameSystem, 0, 0, 0);
    }
  }
  
  GAMEDATA_SetIntrudeReverseArea(gamedata, FALSE);
  if(Intrude_Check_CommConnect(GAMESYSTEM_GetGameCommSysPtr(gameSystem)) == NULL){
    GAMEDATA_SetIntrudeMyID(gamedata, 0);
  }
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

    // ���ʒE�o��ݒ�
    Escape_SetSPEscapeLocation( gameData, &work->loc_req );
  }
  return event;
}

//==================================================================
/**
 * �N���p��EVENT_ChangeMapByConnect
 */
//==================================================================
GMEVENT* EVENT_ChangeMapByConnectIntrude( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                   const CONNECT_DATA* connectData, LOC_EXIT_OFS exitOfs )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;
  GAMEDATA* gameData;

  gameData = GAMESYSTEM_GetGameData( gameSystem );

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeIntrude, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  MAPCHANGE_WORK_init( work, gameSystem );
  work->seasonUpdateEnable = TRUE;

  CONNECTDATA_SetNextLocation( connectData, &(work->loc_req), exitOfs );

  work->exit_type = CONNECTDATA_GetExitType( connectData );

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
  GAMEDATA *gameData = GAMESYSTEM_GetGameData(gameSystem);

  //���@�̃t�H�[����񑫕��s�ɖ߂�
  {
    PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gameData);
    PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
  }

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

  {
    // ���ʒE�o��ݒ�
    Escape_SetSPEscapeLocation( gameData, &work->loc_req );
  }
  
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




//============================================================================================
//
//
//  �S�Ŏ��C�x���g
//
//
//============================================================================================
//-------------------------------------
/**
 * @brief �Q�[���I�[�o�[�C�x���g�p���[�N
 */
//-------------------------------------
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
    u16 warp_zone_id = WARPDATA_GetWarpZoneID( warp_id );
    LOCATION_SetDefaultPos( &esc, warp_zone_id );
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
 *  @brief    �Q�[���I�[�o�[�C�x���g
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
    //���@�̃t�H�[����񑫕��s�ɖ߂�
    {
      GAMEDATA *gameData = GAMESYSTEM_GetGameData(p_wk->gsys);
      PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gameData);
      PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
    }
    // BGM �Đ��J�n
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
 *  @brief  �Q�[���I�[�o�[�C�x���g����
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

//============================================================================================
//
//
//  �}�b�v�J�ڏ���������
//
//
//============================================================================================
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

#ifdef PM_DEBUG
  { 
    char buf[ZONEDATA_NAME_LENGTH*2];
    ZONEDATA_DEBUG_GetZoneName(buf, loc_req->zone_id);
    TAMADA_Printf("MAPCHG_updateGameData:%s\n", buf);
  }
#endif  //PM_DEBUG
  
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
 *  @brief  WFBC�̃Z�b�g�A�b�v����
 *
 *  @param  gamedata
 *  @param  loc 
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
    //���t�B�[���h�ɂ���̂ɉ���ʂ��N���ɂȂ��Ă��Ȃ��ꍇ�͐N���ɂ���
    if(GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE 
        && GAMEDATA_GetSubScreenMode(gamedata) != FIELD_SUBSCREEN_INTRUDE){
      GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_INTRUDE);
    }
    //�\�t�B�[���h�ɂ���̂ɉ���ʂ��N���ɂȂ��Ă��鎞��NORMAL�ɂ���
    else if(GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE 
        && GAMEDATA_GetSubScreenMode(gamedata) == FIELD_SUBSCREEN_INTRUDE){
      GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_NORMAL);
    }
    
    {// �i�������C�u�`���b�g���_�E�W���O�͂��̂܂� ���̑��̉�ʂ͐؂�ւ�
      u8 mode = GAMEDATA_GetSubScreenMode(gamedata);
      if((mode != FIELD_SUBSCREEN_INTRUDE) && (mode != FIELD_SUBSCREEN_BEACON_VIEW)
         && (mode != FIELD_SUBSCREEN_DOWSING)){
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

//============================================================================================
//
//
//  �}�b�v�J�ځF�p���X�֘A
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct PALACE_JUMP_tag
{
  LOCATION Loc;
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
}PALACE_JUMP;

//------------------------------------------------------------------
//------------------------------------------------------------------
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


//------------------------------------------------------------------
//------------------------------------------------------------------
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
      //�G�ߍX�V�֎~
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
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(work->gameSystem);

  switch(*seq){
  case 0:
    {
      INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
      BOOL is_target = FALSE;
      
      if(intcomm != NULL){
        is_target = IntrudeField_Check_Tutorial_OR_TargetMine(intcomm);
      }
      
      //�i���\���W�`�F�b�N
      if ( is_target == FALSE && PLC_WP_CHK_Check(work->gameSystem) )
      {   //�i���\
        //�i���\���b�Z�[�W�R�[��
        SCRIPT_CallScript( event, SCRID_FLD_EV_WARP_SUCCESS, NULL, NULL, FIELDMAP_GetHeapID( work->fieldmap ) );

        //�`���[�g���A�����S�Ċ������Ă��Ȃ��ꍇ�͏펞�ʐM��OFF
        //(�q�Ƃ��ăp���X�ɓ���ƃ`���[�g���A���ō�������)
        if(Intrude_CheckTutorialComplete(work->gameData) == FALSE){
          GameCommSys_ExitReq( game_comm );
        }

        //�}�b�v�`�F���W�C�x���g�ύX�V�[�P���X��
        (*seq) = 1;
      }
      else
      {     //�i���s�\
        //�i���s�\���b�Z�[�W�R�[��
        u32 scr_id;
        
        if(is_target == FALSE){
          scr_id = SCRID_FLD_EV_WARP_FAILED;
        }
        else{
          scr_id = SCRID_FLD_EV_WARP_FAILED_MISSION_TARGET;
        }
        SCRIPT_CallScript( event, scr_id, NULL, NULL, FIELDMAP_GetHeapID( work->fieldmap ) );
        //�I���V�[�P���X��
        (*seq) = 2;
      }
    }
    break;
  case 1:
    if(Intrude_CheckTutorialComplete(work->gameData) == FALSE){
      if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_NULL){
        break;
      }
      GameCommSys_ClearLastStatus(game_comm);
    }
    
    {
      GMEVENT* call_event;
      GAMEDATA_SetIntrudeReverseArea(work->gameData, TRUE);
      GAMEDATA_SetIntrudeMyID(work->gameData, GFL_NET_SystemGetCurrentID());
      call_event = EVENT_ChangeMapPalace( work->gameSystem, work->fieldmap, &work->Loc );
      GMEVENT_ChangeEvent(event, call_event);
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}




//----------------------------------------------------------------------------
/**
 *  @brief  �E�o��̓���ݒ�
 *
 *  @param  gamedata      �Q�[���f�[�^
 *  @param  loc_req       ���N�G�X�g���P�[�V����
 */
//-----------------------------------------------------------------------------
static void Escape_SetSPEscapeLocation( GAMEDATA* gamedata, const LOCATION* loc_req )
{
  const LOCATION* loc_now;
  const LOCATION* ent;
  
  ent = GAMEDATA_GetEntranceLocation( gamedata );
  loc_now = GAMEDATA_GetStartLocation( gamedata );

  // �ڑ��悪D09
  if( ZONEDATA_IsChampionLord( loc_req->zone_id ) ){
    // ���A�`�����s�I�����[�O
    if( loc_now->zone_id == ZONE_ID_C09 ){
      
      GAMEDATA_SetEscapeLocation( gamedata, ent );
    }else if( ZONEDATA_IsChampionLord( loc_now->zone_id ) == FALSE ){ // ChampionLord�ȊO
      LOCATION location;

      //D09�̃f�t�H���g��ݒ�
      LOCATION_SetDefaultPos( &location, ZONE_ID_D09 );
      GAMEDATA_SetEscapeLocation( gamedata, &location );
    }
  }

}


//----------------------------------------------------------------------------
/**
 *  @brie ����ȒE�o����擾����
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  @param  loc_req   ���P�[�V����
 */
//-----------------------------------------------------------------------------
static void Escape_GetSPEscapeLocation( const GAMEDATA* gamedata, LOCATION* loc_req )
{
  const LOCATION* loc_now;
  
  loc_now = GAMEDATA_GetStartLocation( gamedata );

  // �����C��_�a�Ȃ�A����ڑ���ݒ�
  if( ZONEDATA_IsSeaTemple( loc_now->zone_id ) ){
    *loc_req      = *(GAMEDATA_GetSpecialLocation( gamedata ));
      
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���@�̃t�H�[����ʏ�ɖ߂�
 *
 *  @param  gamedata  �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------
static void MapChange_SetPlayerMoveFormNormal( GAMEDATA* gamedata )
{
  PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gamedata);
  u32 form = PLAYERWORK_GetMoveForm( player );

  if( form != PLAYER_MOVE_FORM_DIVING ){
    PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
  }else{
    // �_�C�r���O�͔g����
    PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_SWIM );
  }
}


static const PMS_DATA mail_pmsdata[]={
  // �u���傤�ԁv���Ă���ā@���肪�Ƃ��I
  {
    PMS_TYPE_PECULIAR,  9,
    { 1406, PMS_WORD_NULL},
  },
  // �u�h�L�h�L�v�Ł@�Ƃ��Ă��悩�����ł��I
  {
    PMS_TYPE_PECULIAR,  10,
    { 1649, PMS_WORD_NULL},
  },
  // �܂��u���傤�ԁv���悤�ˁI�@�u�o�C�o�C�v
  {
    PMS_TYPE_PECULIAR,  20,
    { 1406, 2058},
    
  },

};

//----------------------------------------------------------------------------------
/**
 * @brief ���[���{�b�N�X�Ƀx������̃��[�����ŏ��������Ă���
 *
 * @param   gamedata    
 */
//----------------------------------------------------------------------------------
static void MailBox_SetFirstMail( GAMEDATA* gamedata )
{
  MAIL_DATA   *mail;
  GFL_MSGDATA *TrainerMsgData;
  STRBUF  *strbuf;
  STRCODE strdata[BUFLEN_PERSON_NAME];

  // �g���[�i�[�����b�Z�[�W�f�[�^�I�[�v��
  TrainerMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                                NARC_message_trname_dat, GFL_HEAPID_APP );
  strbuf = GFL_MSG_CreateString( TrainerMsgData, support_01 );

  // �T�|�[�g���̐���������擾
  GFL_STR_GetStringCode( strbuf, strdata, BUFLEN_PERSON_NAME );

  // ���[���f�[�^�쐬
  mail = MailData_CreateWork( GFL_HEAPID_APP );

  // �T�|�[�g���Z�b�g
  MailData_SetWriterName( mail, strdata );
  // ����
  MailData_SetWriterSex( mail, PM_FEMALE );
  // ���[���f�U�C��No
  MailData_SetDesignNo( mail, 3 );
  // ���R�[�h�Z�b�g
  MailData_SetCountryCode( mail, PM_LANG );
  // �J�Z�b�g�o�[�W����
  MailData_SetCasetteVersion( mail, PM_VERSION );
  // �ȈՉ�b�Z�b�g
  MailData_SetMsgByIndex( mail, (PMS_DATA*)&mail_pmsdata[0], 0 );
  MailData_SetMsgByIndex( mail, (PMS_DATA*)&mail_pmsdata[1], 1 );
  MailData_SetMsgByIndex( mail, (PMS_DATA*)&mail_pmsdata[2], 2 );
  // �ȈՃ��[�h�u�g���[�i�[�v�Z�b�g
  MailData_SetFormBit( mail, 1419 );

  // ���[���{�b�N�X�Ƀ��[�����Z�b�g
  {
    MAIL_BLOCK *mail_block = GAMEDATA_GetMailBlock( gamedata );
    MAIL_AddMailFormWork( mail_block, MAILBLOCK_PASOCOM, 0, mail);
  }

  // ���[�N���
  GFL_HEAP_FreeMemory( mail );
  GFL_STR_DeleteBuffer( strbuf );
  GFL_MSG_Delete( TrainerMsgData );
}
