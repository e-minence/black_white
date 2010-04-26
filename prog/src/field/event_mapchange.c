//============================================================================================
/**
 * @file  event_mapchange.c
 * @brief マップ遷移関連イベント
 * @date  2008.11.04
 * @author  tamada GAME FREAK inc.
 *
 * @todo  FIELD_STATUS_SetFieldInitFlagをどこかに機能としてまとめるべきか検討
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
#include "sound/pm_sndsys.h" // サウンドシステム参照

#include "item/itemsym.h"

#include "field/zonedata.h"
#include "field/fieldmap.h"
#include "field/location.h"
#include "field/rail_location.h"  // RAIL_LOCATION
#include "field/eventdata_sxy.h"
#include "field/fldmmdl.h"
#include "field/fieldskill_mapeff.h"
#include "field/field_status_local.h" // FIELD_STATUS
#include "field/field_flagcontrol.h" // FIELD_FLAGCONT_INIT〜
#include "field/fieldmap_proc.h"  // FLDMAP_BASESSYS_GRID

#include "event_season_display.h"
#include "event_fieldmap_control.h"
#include "event_mapchange.h"
#include "event_fldmmdl_control.h"
#include "event_fieldmap_control_local.h" // event_mapchange内限定のFieldOpen Close
#include "field_place_name.h"   // FIELD_PLACE_NAME_DisplayForce 
#include "fieldmap/zone_id.h"   //※check　ユニオンのSubScreen設定暫定対処の為
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
#include "savedata/gimmickwork.h"   //for GIMMICKWORK
#include "field_comm/intrude_main.h"
#include "field_comm/intrude_work.h"
#include "field/field_comm/intrude_field.h" //PALACE_MAP_LEN

#include "field_task.h"
#include "field_task_manager.h"

#include "net_app/union/union_main.h" // for UNION_CommBoot
#include "savedata/intrude_save.h"

#include "system/playtime_ctrl.h" // for PLAYTIMECTRL_Start
#include "savedata/gametime.h"  // for GMTIME
#include "savedata/system_data.h" //SYSTEMDATA_
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
static GMEVENT* EVENT_ContinueMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork );

static GMEVENT_RESULT EVENT_MapChangePalaceWithCheck( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_MapChangePalace( GMEVENT* event, int* seq, void* wk );
static GMEVENT * EVENT_ChangeMapPalaceWithCheck( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc);
static GMEVENT * EVENT_ChangeMapPalace( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, LOCATION *loc);

static void setNowLoaction(LOCATION * return_loc, FIELDMAP_WORK * fieldmap);

static void Escape_SetSPEscapeLocation( GAMEDATA* gamedata, const LOCATION* loc_req );
static void Escape_GetSPEscapeLocation( const GAMEDATA* gamedata, LOCATION* loc_req );

static void MapChange_SetPlayerMoveFormNormal( GAMEDATA* gamedata );

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(intrude_system);
FS_EXTERN_OVERLAY(union_system);


//============================================================================================
//
//  イベント：ゲーム開始
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ゲーム開始処理
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
//  「さいしょから」の流れ
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ゲーム開始演出イベント用ワーク定義
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK*   gameSystem;
  GAME_INIT_WORK* gameInitWork;
} FIRST_START_WORK;

//------------------------------------------------------------------
/**
 * @brief ゲーム開始演出イベント
 */
//------------------------------------------------------------------
static GMEVENT_RESULT FirstGameStartEvent( GMEVENT* event, int* seq, void* wk )
{
  FIRST_START_WORK* work       = (FIRST_START_WORK*)wk;
  GAMESYS_WORK*     gameSystem = work->gameSystem;
  GAMEDATA*         gameData   = GAMESYSTEM_GetGameData( gameSystem );

  switch( *seq ) {
  case 0:
    // VRAM全クリア
    GX_SetBankForLCDC( GX_VRAM_LCDC_ALL );
    MI_CpuClearFast( (void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE );
    GX_DisableBankForLCDC();
    (*seq)++;
    break;

  case 1:
    // 季節を初期化する
    {
      int season = PMSEASON_GetRealTimeSeasonID();
      PMSEASON_SetSeasonID( gameData, season );
    }
    // 季節を表示する
    {
      u8 season = GAMEDATA_GetSeasonID( gameData );
      GMEVENT_CallEvent( event, EVENT_SimpleSeasonDisplay( gameSystem, season, season ) );
    }
		(*seq)++;
		break;

	case 2:
    // デモを呼ぶ
    GMEVENT_CallEvent( event, EVENT_CallDemo3D( gameSystem, event, DEMO3D_ID_INTRO_TOWN, 0, TRUE ));
		(*seq)++;
		break;

	case 3:
    // イベント変更
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
 * @brief 「さいしょから」でのゲーム開始演出イベント生成
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FirstGameStart( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  FIRST_START_WORK* work;

  // イベントを生成する
  event = GMEVENT_Create( gameSystem, NULL, FirstGameStartEvent, sizeof(FIRST_START_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワークを初期化する
  work->gameSystem   = gameSystem;
  work->gameInitWork = gameInitWork;

  return event;
}

//============================================================================================
//
//    イベント：新規ゲーム開始
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief 新規ゲーム開始イベント用ワーク定義
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  LOCATION loc_req;
#ifdef  PM_DEBUG
  BOOL isDebugModeFlag;   ///<デバッグモードで開始かどうか
#endif

}FIRST_MAPIN_WORK;

//------------------------------------------------------------------
/**
 *	@brief  フィールドの新規開始初期化処理
 *
 *	@param	gsys  ゲームシステム
 *
 * @note
 *	GAME_DATA の情報で、新規ゲーム開始時に行いたい処理を記述してください。
 *
 *	このタイミングで呼ばれる処理は、OVERLAY　FIELD_INITに配置してください。
 */
//------------------------------------------------------------------
static void GAME_FieldFirstInit( GAMESYS_WORK * gsys )
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);

  // WFBC街自分の場所初期化
  FIELD_WFBC_CORE_SetUp( GAMEDATA_GetMyWFBCCoreData(gamedata), GAMEDATA_GetMyStatus(gamedata), GFL_HEAPID_APP ); //heap はテンポラリ用
  FIELD_WFBC_CORE_SetUpZoneData( GAMEDATA_GetMyWFBCCoreData(gamedata) );
}

//------------------------------------------------------------------
/**
 * @brief 新規ゲーム開始イベント
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
    MAPCHG_setupMapTools( gsys, &fmw->loc_req ); //新しいマップモードなど機能指定を行う
    MAPCHG_updateGameData( gsys, &fmw->loc_req ); //新しいマップID、初期位置をセット

    (*seq)++;
    break;

  case 1:
    // BGM フェードイン
    {
      u32 soundIdx = FSND_GetFieldBGM( gamedata, fmw->loc_req.zone_id );
      GMEVENT_CallEvent( event, EVENT_FSND_ChangeBGM( gsys, soundIdx, FSND_FADE_NONE, FSND_FADE_NORMAL ) );
    }
    (*seq)++;
    break;

  case 2:
    //フィールドマップを開始待ち
    GMEVENT_CallEvent(event, EVENT_FieldOpen_FieldProcOnly(gsys));
    (*seq)++;
    break;

  case 3:
    fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
    // 画面フェードイン
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black( gsys, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  case 4:
    // 地名を表示する
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
 * @brief 新規ゲーム開始イベント生成
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_FirstMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  FIRST_MAPIN_WORK* fmw;

  // イベントを生成する
  event = GMEVENT_Create( gameSystem, NULL, FirstMapInEvent, sizeof(FIRST_MAPIN_WORK) );
  fmw  = GMEVENT_GetEventWork( event );

  // イベントワークを初期化
  fmw->gsys = gameSystem;
  fmw->gamedata   = GAMESYSTEM_GetGameData(gameSystem);
  LOCATION_SetGameStart(&fmw->loc_req);

#ifdef PM_DEBUG
  fmw->isDebugModeFlag = FALSE;
  if ( gameInitWork->mode == GAMEINIT_MODE_DEBUG )
  {
    fmw->isDebugModeFlag = TRUE;
    //適当に手持ちポケモンをAdd
    //デバッグアイテム追加などなど…
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

  GAME_FieldFirstInit( gameSystem );  // フィールド情報の初期化
  //DS本体情報のセット
  {
    SAVE_CONTROL_WORK * svdt = GAMEDATA_GetSaveControlWork( fmw->gamedata );
    SYSTEMDATA * sysdt = SaveData_GetSystemData( svdt );
    SYSTEMDATA_Update( sysdt );
  }
  //プレイ時間カウント　開始
  PLAYTIMECTRL_Start();

  // VRAM全クリア
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  return event;
}

//============================================================================================
//
//  イベント：コンティニュー
//
//============================================================================================
//------------------------------------------------------------------
/**
 * ゲームコンティニューイベント用ワーク定義
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  LOCATION loc_req;
  BOOL sp_exit_flag;    ///<特殊接続で開始するかどうかの判定フラグ
}CONTINUE_MAPIN_WORK;

//------------------------------------------------------------------
/**
 *	@brief  フィールドのコンティニュー開始初期化処理
 *
 *	@param	gsys  ゲームシステム
 *
 * @note
 *	GAME_DATA の情報で、新規ゲーム開始時に行いたい処理を記述してください。
 *
 *	このタイミングで呼ばれる処理は、OVERLAY　FIELD_INITに配置してください。
 */
//------------------------------------------------------------------
static void GAME_FieldContinueInit( GAMESYS_WORK * gsys )
{
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData(gsys);

  // WFBCのZONE書き換え処理
  FIELD_WFBC_CORE_SetUpZoneData( GAMEDATA_GetMyWFBCCoreData(gamedata) );
}

//------------------------------------------------------------------
/**
 * ゲームコンティニューイベント
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
    { //特殊接続リクエスト：いきなり別マップに遷移する
      const LOCATION * spLoc = GAMEDATA_GetSpecialLocation( gamedata );
      cmw->loc_req = *spLoc;
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
      MAPCHG_releaseMapTools( gsys ); //動作モデルの開放など
      MAPCHG_setupMapTools( gsys, &cmw->loc_req ); //新しいマップモードなど機能指定を行う
      MAPCHG_updateGameData( gsys, &cmw->loc_req ); //新しいマップID、初期位置をセット
    }
    else
    {
      MAPCHG_setupMapTools( gsys, &cmw->loc_req ); //新しいマップモードなど機能指定を行う
      EVTIME_Update( gamedata ); //イベント時間更新
      FIELD_FLAGCONT_INIT_Continue( gamedata, cmw->loc_req.zone_id ); //コンティニューによるフラグ落とし処理
      //天気ロード決定
      //**上のFIELD_FLAGCONT_INIT_Continueで移動ポケモンの天気が抽選されるので、
      //必ず、その下で行う。**
      PM_WEATHER_UpdateSaveLoadWeatherNo( gamedata, cmw->loc_req.zone_id );
    }
    (*seq)++;
    break;

  case 1:
    // BGM フェードイン
    {
      u32 soundIdx = FSND_GetFieldBGM( gamedata, cmw->loc_req.zone_id );
      GMEVENT_CallEvent( event, EVENT_FSND_ChangeBGM( gsys, soundIdx, FSND_FADE_NONE, FSND_FADE_NORMAL ) );
    }
    (*seq)++;
    break;

  case 2:
    //フィールドマップを開始待ち
    GMEVENT_CallEvent(event, EVENT_FieldOpen_FieldProcOnly(gsys));
    (*seq)++;
    break;

  case 3:
    if(  cmw->sp_exit_flag && *(EVENTWORK_GetEventWorkAdrs( ev, WK_SYS_SCENE_COMM )) != 0 )
    {
      //ポケセンエレベータ演出で再開
      SCRIPT_CallScript( event, SCRID_POKECEN_ELEVATOR_OUT_CONTINUE, NULL, NULL, HEAPID_FIELDMAP );
    }
    else
    {
      // 画面フェードイン
      // ゲーム終了時の季節を表示する
      u8 season = GAMEDATA_GetSeasonID( gamedata );
      fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
      GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Season( gsys, fieldmap, season, season ) );
    }
    (*seq)++;
    break;

  case 4:
    // 地名を表示する
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
 * @brief ゲームコンティニューイベント生成
 *
 * @todo  LOCATION_SetDirectを使用していて問題ないか調査する
 * @todo  ペナルティ適用時にシェイミのフォルムを戻す処理を入れる
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_ContinueMapIn( GAMESYS_WORK* gameSystem, GAME_INIT_WORK* gameInitWork )
{
  GMEVENT* event;
  CONTINUE_MAPIN_WORK* cmw;
  EVENTWORK * ev;

  // イベントを生成する
  event = GMEVENT_Create( gameSystem, NULL, ContinueMapInEvent, sizeof(CONTINUE_MAPIN_WORK) );
  cmw  = GMEVENT_GetEventWork( event );

  // イベントワークを初期化
  cmw->gsys = gameSystem;
  cmw->gamedata   = GAMESYSTEM_GetGameData(gameSystem);
  LOCATION_SetDirect(&cmw->loc_req, gameInitWork->mapid, gameInitWork->dir, 
    gameInitWork->pos.x, gameInitWork->pos.y, gameInitWork->pos.z);
  ev = GAMEDATA_GetEventWork( cmw->gamedata );
  cmw->sp_exit_flag = EVENTWORK_CheckEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST );
  EVENTWORK_ResetEventFlag( ev, SYS_FLAG_SPEXIT_REQUEST );

  //時間書き換えによるペナルティチェック
  {
    SAVE_CONTROL_WORK * svdt = GAMEDATA_GetSaveControlWork(cmw->gamedata );
    SYSTEMDATA * sysdt = SaveData_GetSystemData( svdt );
    if (!SYSTEMDATA_IdentifyMACAddress(sysdt) || !SYSTEMDATA_IdentifyRTCOffset(sysdt) )
    {
      //ペナルティ時間を設定する
      GMTIME_SetPenaltyTime( SaveData_GetGameTime(svdt) );
    }
    //現在のDS本体情報を今後の設定とする
    SYSTEMDATA_Update( sysdt );
  }

  // コンティニュー設定
  GAME_FieldContinueInit( gameSystem );


  //プレイ時間カウント　開始
  PLAYTIMECTRL_Start();

  // VRAM全クリア
  GX_SetBankForLCDC(GX_VRAM_LCDC_ALL);
  MI_CpuClearFast((void *)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE);
  (void)GX_DisableBankForLCDC();

  return event;
}


//============================================================================================
//
//    イベント：ゲーム終了
//
//============================================================================================
#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief ゲーム終了イベント用ワーク
 */
//------------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
}GAME_END_WORK;

//------------------------------------------------------------------
/**
 * @brief ゲーム終了イベント
 */
//------------------------------------------------------------------
static GMEVENT_RESULT GameEndEvent(GMEVENT * event, int *seq, void *work)
{
  GAME_END_WORK * gew = work;
  switch (*seq) {
  case 0:
    //フィールドマップをフェードアウト
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gew->gsys, gew->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    //通信が動いている場合は終了させる
    if(GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gew->gsys)) != GAME_COMM_NO_NULL){
      GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gew->gsys));
    }
    (*seq)++;
    break;
  case 1:
    //通信終了待ち
    if(GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gew->gsys)) != GAME_COMM_NO_NULL){
      break;
    }
    //フィールドマップを終了待ち
    GMEVENT_CallEvent(event, EVENT_FieldClose_FieldProcOnly(gew->gsys, gew->fieldmap));
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
 * @brief デバッグ用：ゲーム終了イベント生成
 *
 * @note
 * 実際にはゲームを終了させるというインターフェイスは
 * ポケモンに存在しないので、この関数はあくまでデバッグ用。
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
//  イベント：マップ切り替え
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief マップ遷移処理イベント用ワーク
 */
//------------------------------------------------------------------
typedef struct 
{
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;

  u16 before_zone_id;   ///<遷移前マップID
  LOCATION loc_req;     ///<遷移先指定
  EXIT_TYPE exit_type;
  u8 mapchange_type; ///<チェンジタイプ EV_MAPCHG_TYPE
  VecFx32 stream_pos;  ///<流砂中心座標 ( 流砂遷移時にのみ使用 )

  BOOL seasonUpdateEnable; ///<季節の更新を許可するかどうか ( イベント呼び出し側で指定 )
  BOOL seasonUpdateOccur;  ///<季節を更新するかどうか ( イベント内で決定 )
  u16  prevSeason;         ///<遷移前の季節
  u16  nextSeason;         ///<遷移後の季節

} MAPCHANGE_WORK;

typedef MAPCHANGE_WORK* MAPCHANGE_WORK_PTR;

#define INVALID_SEASON_ID (0xff)  ///<季節ID無効値

typedef enum{
  EV_MAPCHG_NORMAL,
  EV_MAPCHG_FLYSKY,
  EV_MAPCHG_ESCAPE,
  EV_MAPCHG_TELEPORT,
  EV_MAPCHG_UNION,
} EV_MAPCHG_TYPE;


//------------------------------------------------------------------
/**
 * @brief 季節を更新するかどうかを決定する
 *
 * @param work
 */
//------------------------------------------------------------------
static void SetSeasonUpdate( MAPCHANGE_WORK* work )
{
  BOOL nextZoneIsOutdoor;
  u16 prevSeason, nextSeason;

  // 遷移前・遷移後の季節を決定
  PMSEASON_GetNextSeason( work->gameData, &prevSeason, &nextSeason );
  
  // 遷移先が屋外かどうかを判定
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

  // 季節が変化 && 遷移先が屋外
  if( (nextSeason != prevSeason) && (nextZoneIsOutdoor) )
  { 
    work->seasonUpdateOccur = TRUE; 
    work->prevSeason = prevSeason;
    work->nextSeason = nextSeason;
  }
}

//------------------------------------------------------------------
/**
 * @brief マップチェンジ コア処理
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
    //フィールドマップを終了待ち
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
        //特定マップで、侵入orビーコンサーチ状態の場合は
        //通信の停止処理を実行
        GameCommSys_ExitReq(gcsp);
        *seq = MAPCORE_SEQ_COMM_EXIT_WAIT;
        break;
      }
    }
    *seq = MAPCORE_SEQ_UPDATE_DATA;
    break;

  case MAPCORE_SEQ_COMM_EXIT_WAIT:
    //通信停止処理ウェイト
    if (GameCommSys_BootCheck(gcsp) == GAME_COMM_STATUS_NULL)
    {
      *seq = MAPCORE_SEQ_UPDATE_DATA;
    }
    break;

  case MAPCORE_SEQ_UPDATE_DATA:
    //マップモードなど機能指定を解除する
    MAPCHG_releaseMapTools( gameSystem );

    { 
      FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gameData), TRUE );
    }

    // 季節の更新
    if( (work->seasonUpdateEnable) && (work->seasonUpdateOccur) )
    {
      TOMOYA_Printf( "season %d\n", work->nextSeason );
      PMSEASON_SetSeasonID( gameData, work->nextSeason );
      //季節ポケモンフォルムチェンジ条件を満たしたので、手持ちの季節ポケモンをフォルムチェンジさせる
      {
        u8 season;
        POKEPARTY *party;
        season = GAMEDATA_GetSeasonID(gameData);
        party = GAMEDATA_GetMyPokemon(gameData);
        SEASONPOKE_FORM_ChangeForm(gameData, party, season);
      }
    }

    //新しいマップモードなど機能指定を行う
    MAPCHG_setupMapTools( gameSystem, &work->loc_req );
    
    //新しいマップID、初期位置をセット
    MAPCHG_updateGameData( gameSystem, &work->loc_req );

    //ビーコンリクエスト(ユニオン移動は専用ビーコン)
    if( work->mapchange_type != EV_MAPCHG_UNION ){
      GAMEBEACON_Set_ZoneChange( work->loc_req.zone_id, gameData );
    }
    //タイプに応じたフラグ初期化
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
    //フィールドマップを開始待ち
    GMEVENT_CallEvent( event, EVENT_FieldOpen_FieldProcOnly(gameSystem) );
    (*seq) ++;
    break;
  case MAPCORE_SEQ_BGM_FADE_WAIT:
    // BGM フェード完了待ち
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
    // 季節更新の有無を決定
    SetSeasonUpdate( work );
    // 動作モデルの移動を止める
    {
      MMDLSYS *fmmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDLSYS_PauseMoveProc( fmmdlsys );
    }
    // 入口進入イベント
    GMEVENT_CallEvent( event, 
        EVENT_EntranceIn( event, gameSystem, gameData, fieldmap, 
                          work->loc_req, work->exit_type, work->seasonUpdateOccur ) );
    (*seq)++;
    return GMEVENT_RES_CONTINUE_DIRECT;
    break;
  case 1:
    // マップチェンジ・コア・イベント
    GMEVENT_CallEvent( event, 
        EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    // 入口退出イベント
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
 * @brief 瞬間マップチェンジ ( デバッグ用 )
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
    // フェードアウト
    GMEVENT_CallEvent( event, DEBUG_EVENT_QuickFadeOut( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // マップチェンジ・コア・イベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    // BGM変更
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3:
    // フェードイン
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
 * @brief フェードなしマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 1:
    // BGM更新リクエスト
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 2:
    // マップチェンジ コアイベント
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
 * @brief BGMを変更しないマップチェンジ
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
    // 季節更新の有無を決定
    SetSeasonUpdate( work );

    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // 画面フェードアウト
  case 1:
    // 季節フェード
    if( work->seasonUpdateEnable && work->seasonUpdateOccur ) {
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    }
    // 基本フェード
    else {
      FIELD_FADE_TYPE fadeOutType;

      fadeOutType = 
        FIELD_FADE_GetFadeOutType( work->before_zone_id, work->loc_req.zone_id );

      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut( gameSystem, fieldmap, fadeOutType, FIELD_FADE_WAIT ) );
    }
    (*seq)++;
    break;

  // マップチェンジ コア イベント
  case 2:
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;

  // 画面フェードイン
  case 3:
    // 季節フェード
    if( work->seasonUpdateEnable && work->seasonUpdateOccur ) {
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->prevSeason, work->nextSeason ) );
    }
    // 基本フェード
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
 * @brief パレス間ワープ(裏から裏)によるマップチェンジ
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
    _SEQ_FIELD_CLOSE = 8, //下画面の演出を見せる為、直前のシーケンスから間を空ける
    _SEQ_BGM_CHANGE,
    _SEQ_MAPCHG_CORE,
    _SEQ_FIELD_OPEN,
    _SEQ_FINISH,
  };
  
  switch( *seq )
  {
  case _SEQ_OBJPAUSE:
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case _SEQ_FIELD_CLOSE:
    // 画面フェードアウト ( クロスフェード )
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case _SEQ_BGM_CHANGE:
    // BGM変更
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case _SEQ_MAPCHG_CORE:
    // マップチェンジ コア イベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, work->mapchange_type ) );
    (*seq)++;
    break;
  case _SEQ_FIELD_OPEN:
    // 画面フェードイン ( クロスフェード )
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
 * @brief 流砂によるマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 1: 
    // 流砂退場イベント
    GMEVENT_CallEvent( event, 
        EVENT_DISAPPEAR_FallInSand( event, gameSystem, fieldmap, &work->stream_pos ) );
    (*seq)++;
    break;
  case 2:
    // BGM変更
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: 
    // マップチェンジ・コア・イベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 4: 
    // 流砂登場イベント
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
 * @brief 「あなぬけのヒモ」によるマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // 退場イベント
    GMEVENT_CallEvent( event, 
        EVENT_DISAPPEAR_Ananukenohimo( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    //自機のフォームを二足歩行に戻す
    MapChange_SetPlayerMoveFormNormal( gameData );
    // BGM変更
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: 
    // マップチェンジ コアイベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_ESCAPE ) );
    (*seq)++;
    break;
  case 4: 
    // 登場イベント
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
 * @brief 「あなをほる」によるマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gameSystem, fieldmap) );
    (*seq) ++;
    break;
  case 1: 
    // 退場イベント
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Anawohoru( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    //自機のフォームを二足歩行に戻す
    MapChange_SetPlayerMoveFormNormal( gameData );
    // BGM変更
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: // マップチェンジ コアイベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_ESCAPE ) );
    (*seq)++;
    break;
  case 4: 
    // 登場イベント
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
 * @brief 「テレポート」によるマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // 退場イベント
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Teleport( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2:
    //自機のフォームを二足歩行に戻す
    {
      PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gameData);
      PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
    }
    // BGM変更
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 3: 
    // マップチェンジ コアイベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_TELEPORT ) );
    (*seq)++;
    break;
  case 4: 
    // 登場イベント
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
 * @brief 「海底神殿　退場」によるマップチェンジ
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
  // ホワイトアウト
  case 0:
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_White( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;
  // 動作モデル停止
  case 1:
    //TOMOYA_Printf( "0000\n" );

    // クルクルが完了するのを待つ　event_seatemple.c内でクルクルにしている可能性があります。
    if( !FIELD_TASK_MAN_IsAllTaskEnd( p_taskMan ) ) {
      break;
    }

    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // マップチェンジ
  case 2:
    
    // マップチェンジ コアイベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;

  // ホワイトイン
  case 3:
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_White( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  // 完了
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief 「海底神殿　入場」によるマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  // ブラックアウト
  case 1:
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Black( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  // マップチェンジ BGM変更あり
  case 2:
    {
      GMEVENT* sub_event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK) );
      MAPCHANGE_WORK* sub_work;

      sub_work  = GMEVENT_GetEventWork( sub_event );
      *sub_work  = *work; // コピー
      // マップチェンジ イベント
      GMEVENT_CallEvent( event, sub_event );
      (*seq)++;
    }
    break;

  // ブラックイン
  case 3:
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    (*seq)++;
    break;

  // 完了
  case 4:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief 「ワープ」によるマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // 退場イベント
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Warp( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 2: 
    // マップチェンジ コアイベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 3: 
    // 登場イベント
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
 * @brief ユニオンルームからのマップチェンジ
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
    // 退場イベント
    GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Teleport( event, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // マップチェンジ コアイベント
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_NORMAL ) );
    (*seq)++;
    break;
  case 2:
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_system ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( intrude_system ) );
    // エレベータからの登場スクリプト実行
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
 * @brief ユニオンルームへのマップチェンジ
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
    // 動作モデル停止
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll( gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1: 
    // マップチェンジ コアイベント
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( intrude_system ) );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( union_system ) );
    GMEVENT_CallEvent( event, EVENT_MapChangeCore( work, EV_MAPCHG_UNION ) );
    (*seq)++;
    break;
  case 2: 
    // ユニオン通信起動
    UNION_CommBoot( gameSystem );
    (*seq)++;
    break;
  case 3:
    // 入口退出イベント
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
 * @brief マップチェンジ イベントワークの初期化
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

  //コモン処理としてここに書く
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

  // イベントワーク初期化
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

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDirectRail( &(work->loc_req), zoneID, dir, 
                          rail_loc->rail_index, rail_loc->line_grid, rail_loc->width_grid);
  work->exit_type = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = seasonUpdateEnable;
  
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

  // イベントワーク初期化
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

  // イベントワーク初期化
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

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ 流砂 ）
 * @param gameSystem          ゲームシステムへのポインタ
 * @param fieldmap      フィールドシステムへのポインタ
 * @param disappearPos 流砂中心点の座標
 * @param zoneID       遷移するマップのZONE指定
 * @param appearPos        遷移先での座標
 * @return GMEVENT 生成したマップ遷移イベント
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

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDirect( &(work->loc_req), zoneID, DIR_DOWN, appearPos->x, appearPos->y, appearPos->z);
  VEC_Set( &(work->stream_pos), disappearPos->x, disappearPos->y, disappearPos->z );
  work->exit_type = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ あなぬけのヒモ ）
 * @param gameSystem ゲームシステムへのポインタ
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByAnanukenohimo( FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByAnanukenohimo, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetEscapeLocation( work->gameData ));
  Escape_GetSPEscapeLocation( work->gameData, &work->loc_req );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ あなをほる )
 * @param gameSystem ゲームシステムへのポインタ
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByAnawohoru( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByAnawohoru, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetEscapeLocation( work->gameData ));
  Escape_GetSPEscapeLocation( work->gameData, &work->loc_req );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ テレポート )
 * @param gameSystem ゲームシステムへのポインタ
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByTeleport( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;
  GAMEDATA* gameData;
  u16 warpID;

  // ワープID取得
  gameData = GAMESYSTEM_GetGameData( gameSystem );
  warpID   = GAMEDATA_GetWarpID( gameData );

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByTeleport, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  WARPDATA_GetWarpLocation( warpID, &(work->loc_req) );
  LOCATION_SetDefaultPos( &(work->loc_req), work->loc_req.zone_id );
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//----------------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ 海底神殿からの強制退場 )
 * @param gameSystem ゲームシステムへのポインタ
 * @return GMEVENT 生成したマップ遷移イベント
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapBySeaTempleUp( GAMESYS_WORK* gameSystem )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBySeaTempleUp, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req      = *(GAMEDATA_GetSpecialLocation( work->gameData ));
  work->loc_req.type = LOCATION_TYPE_DIRECT;
  work->loc_req.dir_id = DIR_DOWN;  // 出てきたときは下を見る。
  work->exit_type    = EXIT_TYPE_NONE;

  return event;
}

//----------------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ 海底神殿へもぐる )
 * @param gameSystem ゲームシステムへのポインタ
 * @return GMEVENT 生成したマップ遷移イベント
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapBySeaTempleDown( GAMESYS_WORK* gameSystem, u16 zone_id )
{
  GMEVENT* event;
  MAPCHANGE_WORK* work;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBySeaTempleDown, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  LOCATION_SetDefaultPos( &(work->loc_req), zone_id );
  work->exit_type    = EXIT_TYPE_NONE;

  // 特殊接続を保存
  {
    // 自機位置を保存
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
 * @brief マップ遷移イベント生成（ ワープ )
 * @param gameSystem
 * @param fieldmap
 * @param zoneID 
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapByWarp( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                u16 zoneID , const VecFx32 * pos, u16 dir )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeByWarp, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
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

  // イベントワーク初期化
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

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  work->loc_req = *spLoc;

  return event;
} 

//------------------------------------------------------------------
/**
 * @brief 表フィールドからパレスマップに移動するとき
 */
//------------------------------------------------------------------
GMEVENT* EVENT_ChangeMapFldToPalace( GAMESYS_WORK* gsys, u16 zone_id, const VecFx32* pos )
{
  GMEVENT * event;
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  VecFx32 calc_pos = *pos;
  
  //裏フィールド以外から、パレスへ飛ぶ場合通常フィールドへの戻り先を記録しておく
  {
    LOCATION return_loc;
    setNowLoaction( &return_loc, fieldWork );
    GAMEDATA_SetPalaceReturnLocation(gamedata, &return_loc);

    //パレス滞在時間用に入室時間をPush
    ISC_SAVE_PalaceSojournParam(
      SaveData_GetIntrude(GAMEDATA_GetSaveControlWork(gamedata)), 
      GAMEDATA_GetPlayTimeWork(gamedata), SOJOURN_TIME_PUSH);
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
      //子機の場合、palace_area == 0 が左端、ではなく自分のNetIDのパレスが左端の為。
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
 * @brief パレスから自分のフィールドに戻るとき
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

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem );
  //覚えておいた戻り先をそのまま代入
  work->loc_req = *(GAMEDATA_GetPalaceReturnLocation( gamedata ) );
  work->exit_type          = EXIT_TYPE_NONE;
  work->seasonUpdateEnable = FALSE;
#else
  {
    LOCATION loc;
    //覚えておいた戻り先をそのまま代入
    loc = *(GAMEDATA_GetPalaceReturnLocation( gamedata ) );
    event = EVENT_ChangeMapPalace( gameSystem, fieldWork, &loc );
  }
#endif

  //パレス滞在時間を更新
  ISC_SAVE_PalaceSojournParam(
    SaveData_GetIntrude(GAMEDATA_GetSaveControlWork(gamedata)), 
    GAMEDATA_GetPlayTimeWork(gamedata), SOJOURN_TIME_CALC_SET);

  //LAST_STATUSをクリアしておく　※自分が最初の退室者の場合はステータスが残っている
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSystem);
    GAME_COMM_LAST_STATUS last_status = GameCommSys_GetLastStatus(game_comm);
    //他の通信が起動している場合は起動時にクリアされているのでよい
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
      GameCommSys_ClearLastStatus(game_comm);
    }
  }
  
  {
    FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    if(FIELD_PLAYER_CheckIllegalOBJCode( fld_player ) == FALSE){
      GF_ASSERT(0); //退出する時に自機以外になっているのはおかしい
      IntrudeField_PlayerDisguise(NULL, gameSystem, 0, 0, 0);
    }
  }
  
  GAMEDATA_SetIntrudeReverseArea(gamedata, FALSE);
  GAMEDATA_SetIntrudeMyID(gamedata, 0);
//  PMSND_PlaySE( SEQ_SE_FLD_131 ); //SEの確認用にエフェクトは無いけどあてておく

  return event;
}

//------------------------------------------------------------------
/// 現在位置をLOCATIONにセット（GRID/RAIL両対応）
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
    //特殊接続先が指定されている場合、記憶しておいた場所に飛ぶ
    const LOCATION * sp = GAMEDATA_GetSpecialLocation( gameData );
    work->loc_req = *sp;
  }
  else
  {
    CONNECTDATA_SetNextLocation( connectData, &(work->loc_req), exitOfs );
  }
  work->exit_type = CONNECTDATA_GetExitType( connectData );

  {
    //フィールド→屋内への移動の際は脱出先位置を記憶しておく
    const LOCATION* ent = GAMEDATA_GetEntranceLocation( gameData );
    if( ZONEDATA_IsFieldMatrixID(ent->zone_id) == TRUE
        && ZONEDATA_IsFieldMatrixID(work->loc_req.zone_id) == FALSE )
    {
      GAMEDATA_SetEscapeLocation( gameData, ent );
    }

    // 特別脱出先設定
    Escape_SetSPEscapeLocation( gameData, &work->loc_req );
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

  // イベントワーク初期化
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

  //自機のフォームを二足歩行に戻す
  {
    PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gameData);
    PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
  }

  event = GMEVENT_Create(gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK));
  work  = GMEVENT_GetEventWork(event);

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  //ロケーションは用意してあるのでコピーする
  work->loc_req = *loc;
  //方向だけセット
  work->loc_req.dir_id = dir;
  work->exit_type      = EXIT_TYPE_NONE;
  work->mapchange_type = EV_MAPCHG_FLYSKY; 

  {
    // 特別脱出先設定
    Escape_SetSPEscapeLocation( gameData, &work->loc_req );
  }
  
  return event;
}

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 (BGM変更なし)
 * @param gameSystem ゲームシステムへのポインタ
 * @param fieldmap   フィールドシステムへのポインタ
 * @param zoneID     遷移するマップのZONE指定
 * @param pos        遷移するマップでの座標指定
 * @param dir        遷移するマップでの方向指定
 * @return GMEVENT  生成したマップ遷移イベント
 */
//------------------------------------------------------------------
GMEVENT * EVENT_ChangeMapBGMKeep( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                  u16 zoneID, const VecFx32* pos, u16 dir )
{
  MAPCHANGE_WORK* work;
  GMEVENT* event;

  event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeBGMKeep, sizeof(MAPCHANGE_WORK) );
  work  = GMEVENT_GetEventWork( event );

  // イベントワーク初期化
  MAPCHANGE_WORK_init( work, gameSystem ); 
  dir = EXITDIR_fromDIR( dir );
  LOCATION_SetDirect( &(work->loc_req), zoneID, dir, pos->x, pos->y, pos->z );
  work->exit_type = EXIT_TYPE_NONE;
  
  return event;
}

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 (パレス間ワープ(裏から裏))
 * @param gameSystem ゲームシステムへのポインタ
 * @param fieldmap   フィールドシステムへのポインタ
 * @param zoneID     遷移するマップのZONE指定
 * @param pos        遷移するマップでの座標指定
 * @param dir        遷移するマップでの方向指定
 * @return GMEVENT  生成したマップ遷移イベント
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
      //子機の場合、palace_area == 0 が左端、ではなく自分のNetIDのパレスが左端の為。
      palace_area = intcomm->intrude_status_mine.palace_area;
      map_offset = palace_area - GAMEDATA_GetIntrudeMyID(gamedata);
      if(map_offset < 0){
        map_offset = intcomm->member_num + map_offset;
      }
    }
    calc_pos.x += PALACE_MAP_LEN * map_offset;
  }

  // イベントワーク初期化
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
//  全滅時イベント
//
//
//============================================================================================
//-------------------------------------
/**
 * @brief ゲームオーバーイベント用ワーク
 */
//-------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gamedata;
  LOCATION loc_req;
} MAPCHANGE_GAMEOVER;


//--------------------------------------------------------------
/**
 * @brief 全滅時のマップ遷移処理（フィールド非生成時）
 */
//--------------------------------------------------------------
static void MAPCHG_GameOver( GAMESYS_WORK * gsys )
{
  LOCATION loc_req;
  GAMEDATA* gamedata = GAMESYSTEM_GetGameData( gsys );
  u16 warp_id = GAMEDATA_GetWarpID( gamedata );

  //復活ポイントを取得
  WARPDATA_GetRevivalLocation( warp_id, &loc_req );

  //エスケープポイントを再設定
  {
    LOCATION esc;
    u16 warp_id = GAMEDATA_GetWarpID( gamedata );
    WARPDATA_GetEscapeLocation( warp_id, &esc );
    GAMEDATA_SetEscapeLocation( gamedata, &esc );
  }

  //マップモードなど機能指定を解除する
  MAPCHG_releaseMapTools( gsys );

  { 
    FIELD_STATUS_SetFieldInitFlag( GAMEDATA_GetFieldStatus(gamedata), TRUE );
  }
  //新しいマップモードなど機能指定を行う
  MAPCHG_setupMapTools( gsys, &loc_req );
  
  //新しいマップID、初期位置をセット
  MAPCHG_updateGameData( gsys, &loc_req );

  //イベント時間更新
  EVTIME_Update( gamedata );

  //ゲームオーバー時のフラグのクリア
  FIELD_FLAGCONT_INIT_GameOver( gamedata, loc_req.zone_id );
}


//----------------------------------------------------------------------------
/**
 *	@brief    ゲームオーバーイベント
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameOver(GMEVENT * event, int * seq, void *work)
{
  MAPCHANGE_GAMEOVER* p_wk = work;

  switch( *seq )
  {
  // 遷移先を設定
  case 0:
    MAPCHG_GameOver( p_wk->gsys );
    (*seq) ++;
    break;

  // BGM 再生開始
  case 1: 
    //自機のフォームを二足歩行に戻す
    {
      GAMEDATA *gameData = GAMESYSTEM_GetGameData(p_wk->gsys);
      PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gameData);
      PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
    }
    // BGM 再生開始
    {
      u32 soundIdx = FSND_GetFieldBGM( p_wk->gamedata, p_wk->loc_req.zone_id );
      GMEVENT_CallEvent( event, 
          EVENT_FSND_ChangeBGM( p_wk->gsys, soundIdx, FSND_FADE_NONE, FSND_FADE_NORMAL ) );
    }
    (*seq) ++;
    break;

  // フィールドマップ開始
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
 *	@brief  ゲームオーバーイベント生成
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

  //復活ポイントを取得
  {
    u16 warp_id = GAMEDATA_GetWarpID( p_wk->gamedata );
    WARPDATA_GetRevivalLocation( warp_id, &p_wk->loc_req );
  }

  return event;
}

//============================================================================================
//
//
//  マップ遷移初期化処理
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

  //直接指定の場合はコピーするだけ
  if (loc_req->type == LOCATION_TYPE_DIRECT) {
    *loc_tmp = *loc_req;
    return;
  }
  //開始位置セット
  result = EVENTDATA_SetLocationByExitID(evdata, loc_tmp, loc_req->exit_id, loc_req->exit_ofs );

  if (!result) {
    //デバッグ用処理：本来は不要なはず
    OS_Printf("connect: debug default position\n");
    LOCATION_SetDefaultPos(loc_tmp, loc_req->zone_id);
  }
}

//------------------------------------------------------------------
/**
 * @brief Map切り替え時のデータ更新処理
 *
 * @todo  歩いてマップをまたいだときの処理との共通部分をきちんと処理すること
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

  // ISSにゾーン切り替えを通知
  {
    ISS_SYS* iss = GAMESYSTEM_GetIssSystem( gsys );
    ISS_SYS_ZoneChange( iss, loc.zone_id );
  }
  
  //開始位置を記憶しておく
  GAMEDATA_SetStartLocation(gamedata, &loc);

  //ワープ先登録
  {
    u16 warp_id;
    warp_id = WARPDATA_SearchByRoomID( loc.zone_id );
    if (warp_id) {
      GAMEDATA_SetWarpID( gamedata, warp_id );
    }
  }
  //ギミックアサイン
  AssignGimmickID(gamedata, loc.zone_id);

  //特殊スクリプト呼び出し：ゾーン切り替え
  SCRIPT_CallZoneChangeScript( gsys, HEAPID_PROC );

  //イベント時間更新
  EVTIME_Update( gamedata );

  //マップ遷移時のフラグ初期化
  FIELD_FLAGCONT_INIT_MapJump( gamedata, loc.zone_id );

  //マップ到着フラグセット
  ARRIVEDATA_SetArriveFlag( gamedata, loc.zone_id );

  //新規ゾーンに配置する動作モデルを追加
  MAPCHG_loadMMdl( gamedata, loc_req );

  //フィールド技　マップ効果
  MAPCHG_setupFieldSkillMapEff( gamedata, &loc );

  // WFBCの設定
  MAPCHG_SetUpWfbc( gamedata, &loc );

  // 天気更新
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
 *  @brief  WFBCのモデルを読み込む
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
      // 通信相手のデータに変更
      cp_wfbc = GAMEDATA_GetWFBCCoreData( gamedata, GAMEDATA_WFBC_ID_COMM );
    }

    // 人物
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

    // アイテム
    {
      const FIELD_WFBC_CORE_ITEM* cp_item;

      cp_item = GAMEDATA_GetWFBCItemData( gamedata );

      // アイテム数取得
      count = WFBC_CORE_ITEM_GetNum( cp_item );

      // 配置モデル作成
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
 *	@brief  WFBCのセットアップ処理
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

  // まちに入った！加算
  {
    FIELD_WFBC_CORE* p_wfbc;
    const FIELD_STATUS* cp_fs = GAMEDATA_GetFieldStatus( gamedata );
    MAPMODE mapmode = FIELD_STATUS_GetMapMode( cp_fs );


    if( mapmode == MAPMODE_NORMAL )
    {
      p_wfbc = GAMEDATA_GetMyWFBCCoreData( gamedata );

      // 街に入った加算
      FIELD_WFBC_CORE_CalcMoodInTown( p_wfbc );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  フィールド技　画面効果マスクの設定
 */
//-----------------------------------------------------------------------------
static void MAPCHG_setupFieldSkillMapEff( GAMEDATA * gamedata, const LOCATION *loc_req )
{
  FIELD_STATUS * fldstatus =  GAMEDATA_GetFieldStatus( gamedata ); 
  u32 fs_effmsk = ZONEDATA_GetFieldSkillMapEffMsk( loc_req->zone_id );

  //フラッシュ　システムフラグを見て、マスクを書き換える
  if( FIELDSKILL_MAPEFF_MSK_IS_ON(fs_effmsk, FIELDSKILL_MAPEFF_MSK_FLASH_NEAR) )
  {
    if( FIELD_STATUS_IsFieldSkillFlash(fldstatus) )
    {
      // フラッシュ技使用後に変換
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
 * @todo  マップをまたいでor画面暗転中もずっと維持されるメモリ状態などはここで設定
 * @todo  下記の※checkの対処を行う
 */
//--------------------------------------------------------------
static void MAPCHG_setupMapTools( GAMESYS_WORK * gsys, const LOCATION * loc_req )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
  //MAPCHGモジュール群はFieldMapが存在しないときに呼ばれる
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  GF_ASSERT( fieldmap == NULL );

  //※check　スクリプトでマップ作成前に実行できるタイミングが出来れば、そこで行うようにしたい
//  if(ZONEDATA_IsPalaceField(loc_req->zone_id) || ZONEDATA_IsBingo(loc_req->zone_id)){
  if(GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE 
      && ZONEDATA_IsPalace(loc_req->zone_id) == FALSE){ //パレス島では白黒にしない
    FIELD_STATUS_SetMapMode( GAMEDATA_GetFieldStatus( gamedata ), MAPMODE_INTRUDE );
    //GAMEDATA_SetMapMode(gamedata, MAPMODE_INTRUDE);
  }
  else{
    FIELD_STATUS_SetMapMode( GAMEDATA_GetFieldStatus( gamedata ), MAPMODE_NORMAL );
  }
  //※check　ユニオンルームへの移動を受付スクリプトで制御するようになったらサブスクリーンモードの
  //         変更もそのスクリプト内で行うようにする
  //         ミュージカル控え室も同じように処理するAri100329
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
    //裏フィールドにいるのに下画面が侵入になっていない場合は侵入にする
    if(GAMEDATA_GetIntrudeReverseArea(gamedata) == TRUE 
        && GAMEDATA_GetSubScreenMode(gamedata) != FIELD_SUBSCREEN_INTRUDE){
      GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_INTRUDE);
    }
    //表フィールドにいるのに下画面が侵入になっている時はNORMALにする
    else if(GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE 
        && GAMEDATA_GetSubScreenMode(gamedata) == FIELD_SUBSCREEN_INTRUDE){
      GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_NORMAL);
    }
    
    {// 進入かライブチャットかダウジングはそのまま その他の画面は切り替え
      u8 mode = GAMEDATA_GetSubScreenMode(gamedata);
      if((mode != FIELD_SUBSCREEN_INTRUDE) && (mode != FIELD_SUBSCREEN_BEACON_VIEW)
         && (mode != FIELD_SUBSCREEN_DOWSING)){
        GAMEDATA_SetSubScreenMode(gamedata, FIELD_SUBSCREEN_NORMAL);
      }
    }
    break;
  }

  //イベント起動データの読み込み
  EVENTDATA_SYS_Load(evdata, loc_req->zone_id, GAMEDATA_GetSeasonID(gamedata) );

  // イベント追加登録

  //マトリックスデータの更新
  {
    MAP_MATRIX * matrix = GAMEDATA_GetMapMatrix( gamedata );
    u16 matID = ZONEDATA_GetMatrixID( loc_req->zone_id );
    MAP_MATRIX_Init( matrix, matID, loc_req->zone_id, GFL_HEAP_LOWID(GFL_HEAPID_APP) );
    MAP_MATRIX_CheckReplace( matrix, gsys, GFL_HEAP_LOWID(GFL_HEAPID_APP) );
  }

  //フィールド技 マップ効果
  MAPCHG_setupFieldSkillMapEff( gamedata, loc_req );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void MAPCHG_releaseMapTools( GAMESYS_WORK * gsys )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  //MAPCHGモジュール群はFieldMapが存在しないときに呼ばれる
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
  GF_ASSERT( fieldmap == NULL );

  //配置していた動作モデルを削除
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
 * @brief ギミックのアサイン
 * 
 * @param   inZoneID    ゾーンＩＤ
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
  //ギミックワーク取得
  work = GAMEDATA_GetGimmickWork(gamedata);

  //マップジャンプのときのみワーククリアする（歩いてゾーンが切り替わった場合は初期化しない）
  GIMMICKWORK_ClearWork(work);

  //ギミックデータ検索
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
        //ギミック発見。アサインする
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
//  マップ遷移：パレス関連
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
  // イベントワーク初期化
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
  // イベントワーク初期化
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
    //ＳＥ
    PMSND_PlaySE( SEQ_SE_FLD_131 );
    //エフェクトコール
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(fieldmap), event, ENCEFFID_PALACE_WARP);
    (*seq)++;
    break;
  case 1:
    {
      MAPCHANGE_WORK* mc_work;
      GMEVENT* call_event;
      call_event = GMEVENT_Create( gameSystem, NULL, EVENT_MapChangeNoFade, sizeof(MAPCHANGE_WORK) );
      mc_work  = GMEVENT_GetEventWork( call_event );
      // イベントワーク初期化
      MAPCHANGE_WORK_init( mc_work, gameSystem );
      //ロケーションセット
      mc_work->loc_req = work->Loc;
//      LOCATION_SetDirect( &(mc_work->loc_req), work->ZoneID, work->Dir, work->Pos.x, work->Pos.y, work->Pos.z ); 
      mc_work->exit_type          = EXIT_TYPE_NONE;
      //季節更新禁止
      mc_work->seasonUpdateEnable = FALSE;
      //イベントコール
      GMEVENT_CallEvent( event, call_event );
    }
    (*seq)++;
    break;
  case 2:
    //ブラックインリクエスト　そらをとぶ演出流用
    GMEVENT_CallEvent(event, EVENT_FlySkyBrightIn(gameSystem, fieldmap, FIELD_FADE_BLACK, FIELD_FADE_WAIT, 2));
    (*seq)++;
    break;
  case 3:
    return GMEVENT_RES_FINISH; 
  }
  return GMEVENT_RES_CONTINUE;
}

//パレスにもぐれるかをチェックして、ＯＫならもぐるイベント
static GMEVENT_RESULT EVENT_MapChangePalaceWithCheck( GMEVENT* event, int* seq, void* wk )
{
  PALACE_JUMP* work = (PALACE_JUMP*)wk;

  switch(*seq){
  case 0:
    //進入可能座標チェック
    if ( PLC_WP_CHK_Check(work->gameSystem) )
    {   //進入可能
      //進入可能メッセージコール
      SCRIPT_CallScript( event, SCRID_FLD_EV_WARP_SUCCESS, NULL, NULL, FIELDMAP_GetHeapID( work->fieldmap ) );
      //マップチェンジイベント変更シーケンスへ
      (*seq) = 1;
    }
    else
    {     //進入不可能
      //進入不可能メッセージコール
      SCRIPT_CallScript( event, SCRID_FLD_EV_WARP_FAILED, NULL, NULL, FIELDMAP_GetHeapID( work->fieldmap ) );
      //終了シーケンスへ
      (*seq) = 2;
    }
    break;
  case 1:
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
 *	@brief  脱出先の特殊設定
 *
 *	@param	gamedata      ゲームデータ
 *	@param	loc_req       リクエストロケーション
 */
//-----------------------------------------------------------------------------
static void Escape_SetSPEscapeLocation( GAMEDATA* gamedata, const LOCATION* loc_req )
{
  const LOCATION* loc_now;
  const LOCATION* ent;
  
  ent = GAMEDATA_GetEntranceLocation( gamedata );
  loc_now = GAMEDATA_GetStartLocation( gamedata );

  // 接続先がD09
  if( ZONEDATA_IsChampionLord( loc_req->zone_id ) ){
    // 今、チャンピオンリーグ
    if( loc_now->zone_id == ZONE_ID_C09 ){
      
      GAMEDATA_SetEscapeLocation( gamedata, ent );
    }else if( ZONEDATA_IsChampionLord( loc_now->zone_id ) == FALSE ){ // ChampionLord以外
      LOCATION location;

      //D09のデフォルトを設定
      LOCATION_SetDefaultPos( &location, ZONE_ID_D09 );
      GAMEDATA_SetEscapeLocation( gamedata, &location );
    }
  }

}


//----------------------------------------------------------------------------
/**
 *	@brie 特殊な脱出先を取得する
 *
 *	@param	gamedata  ゲームデータ
 *	@param	loc_req   ロケーション
 */
//-----------------------------------------------------------------------------
static void Escape_GetSPEscapeLocation( const GAMEDATA* gamedata, LOCATION* loc_req )
{
  const LOCATION* loc_now;
  
  loc_now = GAMEDATA_GetStartLocation( gamedata );

  // 今が海底神殿なら、特殊接続を設定
  if( ZONEDATA_IsSeaTemple( loc_now->zone_id ) ){
    *loc_req      = *(GAMEDATA_GetSpecialLocation( gamedata ));
      
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  自機のフォームを通常に戻す
 *
 *	@param	gamedata  ゲームデータ
 */
//-----------------------------------------------------------------------------
static void MapChange_SetPlayerMoveFormNormal( GAMEDATA* gamedata )
{
  PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork(gamedata);
  u32 form = PLAYERWORK_GetMoveForm( player );

  if( form != PLAYER_MOVE_FORM_DIVING ){
    PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_NORMAL );
  }else{
    // ダイビングは波乗りに
    PLAYERWORK_SetMoveForm( player, PLAYER_MOVE_FORM_SWIM );
  }
}


