/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_in.c
 * @breif  出入口への進入時のイベント作成
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////

#include "gamesystem/game_event.h"
#include "field/eventdata_type.h"
#include "field/fieldmap_proc.h"
#include "field/location.h"

#include "event_entrance_in.h"

#include "event_fieldmap_control.h"  // for EVENT_FieldFadeOut_xxxx
#include "event_entrance_effect.h"   // for EVENT_FieldDoorInAnime
#include "event_fldmmdl_control.h"   // for EVENT_PlayerOneStepAnime
#include "event_disappear.h"         // for EVENT_DISAPPEAR_xxxx

#include "sound/pm_sndsys.h"
#include "field_sound.h"
#include "sound/bgm_info.h"
#include "../../resource/sound/bgm_info/iss_type.h"

#include "fieldmap.h"
#include "field_task.h"  
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"

#include "field_comm/intrude_snd_def.h"


//=======================================================================================
// ■定数
//=======================================================================================
// イベント処理関数のタイプ
typedef enum {
  EVENTFUNC_TYPE_NONE, // ドアなし
  EVENTFUNC_TYPE_DOOR, // ドアあり
  EVENTFUNC_TYPE_STEP, // 階段
  EVENTFUNC_TYPE_WARP, // ワープ
  EVENTFUNC_TYPE_SPx,  // 特殊
  EVENTFUNC_TYPE_INTRUDE, // 侵入
  EVENTFUNC_TYPE_NUM,
} EVENTFUNC_TYPE;


//=======================================================================================
// ■イベント ワーク
//=======================================================================================
typedef struct {

  GAMESYS_WORK*     gameSystem;
  GAMEDATA*         gameData;
  FIELDMAP_WORK*    fieldmap;
  LOCATION          nextLocation;      // 遷移先指定
  EXIT_TYPE         exitType;          // 出入り口タイプ
  BOOL              seasonDisplayFlag; // 季節表示を行うかどうか
  FIELD_FADE_TYPE   fadeOutType;       // 季節表示がない場合のF/Oタイプ
  BOOL              BGMFadeWaitFlag;   // BGM のフェード完了を待つかどうか
  FIELD_CAMERA_MODE initCameraMode;    // イベント開始時のカメラモード

} EVENT_WORK;


//=======================================================================================
// ■index
//======================================================================================= 
// 各 EXIT_TYPE ごとのイベント
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSPx( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeIntrude( GMEVENT * event, int *seq, void * wk );


//=======================================================================================
// ■public func
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief 出入口進入イベントを作成する
 *
 * @param parent            親イベント
 * @param gameSystem
 * @param gameData
 * @param fieldmap
 * @param nextLocation      遷移先指定
 * @param exitType          遷移タイプ指定
 * @param seasonDisplayFlag 季節表示を行うかどうか
 *
 * @return 作成したイベント
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceIn( GMEVENT* parent, 
                           GAMESYS_WORK* gameSystem,
                           GAMEDATA* gameData, 
                           FIELDMAP_WORK* fieldmap, 
                           LOCATION nextLocation, 
                           EXIT_TYPE exitType,
                           BOOL seasonDisplayFlag )
{
  GMEVENT* event;
  EVENT_WORK* work;
  u16 prevZoneID, nextZoneID;
  FIELD_FADE_TYPE fadeOutType;
  EVENTFUNC_TYPE funcType;

  // イベントテーブル
  const GMEVENT_FUNC eventFuncTable[ EVENTFUNC_TYPE_NUM ] = 
  {
    EVENT_FUNC_EntranceIn_ExitTypeNone,   // EVENTFUNC_TYPE_NONE ドアなし
    EVENT_FUNC_EntranceIn_ExitTypeDoor,   // EVENTFUNC_TYPE_DOOR ドアあり
    EVENT_FUNC_EntranceIn_ExitTypeStep,   // EVENTFUNC_TYPE_STEP 階段
    EVENT_FUNC_EntranceIn_ExitTypeWarp,   // EVENTFUNC_TYPE_WARP ワープ
    EVENT_FUNC_EntranceIn_ExitTypeSPx,    // EVENTFUNC_TYPE_SPx, 特殊
    EVENT_FUNC_EntranceIn_ExitTypeIntrude,// EVENTFUNC_TYPE_INTRUDE 侵入
  };

  // イベント処理関数を決定
  switch( exitType ) {
  case EXIT_TYPE_NONE:    funcType = EVENTFUNC_TYPE_NONE; break;
  case EXIT_TYPE_MAT:     funcType = EVENTFUNC_TYPE_NONE; break;
  case EXIT_TYPE_STAIRS:  funcType = EVENTFUNC_TYPE_STEP; break;
  case EXIT_TYPE_DOOR:    funcType = EVENTFUNC_TYPE_DOOR; break;
  case EXIT_TYPE_WALL:    funcType = EVENTFUNC_TYPE_STEP; break;
  case EXIT_TYPE_WARP:    funcType = EVENTFUNC_TYPE_WARP; break;
  case EXIT_TYPE_INTRUDE: funcType = EVENTFUNC_TYPE_INTRUDE; break;
  default:                funcType = EVENTFUNC_TYPE_SPx;  break;
  }

  // 基本フェードアウトの種類を決定
  prevZoneID  = FIELDMAP_GetZoneID( fieldmap );
  nextZoneID  = nextLocation.zone_id;
  fadeOutType = FIELD_FADE_GetFadeOutType( prevZoneID, nextZoneID );

  // イベント作成
  event = GMEVENT_Create( gameSystem, parent, eventFuncTable[ funcType ], sizeof( EVENT_WORK ) );

  // イベントワーク初期化
  work                    = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem        = gameSystem;
  work->gameData          = gameData;
  work->fieldmap          = fieldmap;
  work->nextLocation      = nextLocation;
  work->exitType          = exitType;
  work->seasonDisplayFlag = seasonDisplayFlag;
  work->fadeOutType       = fadeOutType;

  return event;
}


//=======================================================================================
// ■private func
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @breif ドアなし時の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch ( *seq ) {
  // BGM再生準備
  case 0:
    { 
      u16 nowZoneID;

      nowZoneID = FIELDMAP_GetZoneID( fieldmap );
      FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
    }
    (*seq)++;
    break;

  // SE 再生
  case 1:
    if( work->fadeOutType != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
    (*seq)++; 
    break;

  // 画面フェードアウト
  case 2:
    if( work->seasonDisplayFlag )
    { // 季節フェード
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    }
    else
    { // 基本フェード
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut( gameSystem, fieldmap, work->fadeOutType, FIELD_FADE_WAIT ) );
    }
    (*seq)++;
    break;

  // イベント終了
  case 3:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif ドアあり時の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch( *seq ) {
  // ドア進入イベント
  case 0:
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorInAnime( 
          gameSystem, fieldmap, &(work->nextLocation), TRUE, work->seasonDisplayFlag, work->fadeOutType, work->exitType ) );
    (*seq)++;
    break;

  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif 階段の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*     work       = wk;
	GAMESYS_WORK*   gameSystem = work->gameSystem;
	FIELDMAP_WORK*  fieldmap   = work->fieldmap;
	GAMEDATA*       gameData   = work->gameData;
  FIELD_SOUND*    fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch ( *seq ) {
  case 0:  
    // 自機前進
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 1: 
    { // 現在のBGMがダンジョンISS && 次のBGMもダンジョンISS ==> BGMフェードアウト
      FIELD_SOUND*  fieldSound = GAMEDATA_GetFieldSound( gameData );
      BGM_INFO_SYS* BGMInfo    = GAMEDATA_GetBGMInfoSys( gameData );
      PLAYER_WORK*  player     = GAMEDATA_GetPlayerWork( gameData, 0 );
      u32 nextBGM = FSND_GetFieldBGM( gameData, work->nextLocation.zone_id );
      u32 nowBGM = PMSND_GetBGMsoundNo();
      u8 nextIssType = BGM_INFO_GetIssType( BGMInfo, nextBGM ); 
      u8 nowIssType = BGM_INFO_GetIssType( BGMInfo, nowBGM ); 
      if( ( nextIssType == ISS_TYPE_DUNGEON ) && ( nowIssType == ISS_TYPE_DUNGEON ) ) { 
        // BGM フェードアウト
        GMEVENT* fadeOutEvent = EVENT_FSND_FadeOutBGM( gameSystem, FSND_FADE_FAST );
        GMEVENT_CallEvent( event, fadeOutEvent );
        work->BGMFadeWaitFlag = TRUE; // BGMフェードを待つ
      }
      else { 
        // BGM 再生準備
        u16 nowZoneID = FIELDMAP_GetZoneID( fieldmap );
        FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
        work->BGMFadeWaitFlag = FALSE; // BGMフェードは待たない
      }
    }
    (*seq)++;
    break;
  case 2: 
    // BGMフェード完了待ち
    if( (work->BGMFadeWaitFlag == FALSE) || (PMSND_CheckFadeOnBGM() == FALSE) )
    { 
      // SE 再生
      if( work->fadeOutType != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
      (*seq)++; 
    }
    break;
  case 3:
    // 画面フェードアウト
    if( work->seasonDisplayFlag )
    { // 季節フェード
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    }
    else
    { // 基本フェード
      GMEVENT_CallEvent( event,
          EVENT_FieldFadeOut( gameSystem, fieldmap, work->fadeOutType, FIELD_FADE_WAIT ) );
    }
    (*seq)++;
    break;

  case 4:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif ワープ時の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch( *seq ) {
  // ワープ退出イベント
  case 0:
		GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Warp( NULL, gameSystem, fieldmap ) );
    (*seq)++;
    break;

  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 

//---------------------------------------------------------------------------------------
/**
 * @breif 進入イベント( SPx )
 *
 * ※カメラのアニメーション → ドア進入アニメ
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSPx( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*     work       = wk;
	GAMESYS_WORK*   gameSystem = work->gameSystem;
	FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_CAMERA*   camera     = FIELDMAP_GetFieldCamera( fieldmap );
  FIELD_TASK_MAN* taskMan    = FIELDMAP_GetTaskManager( fieldmap );

  // 処理シーケンス
  enum {
    SEQ_INIT,                       // イベント開始
    SEQ_WAIT_CAMERA_TRACE,          // カメラの自機追従処理の終了待ち
    SEQ_DOOR_IN_ANIME,              // ドア進入イベント
    SEQ_EXIT,                       // イベント終了
  };

  switch( *seq ) {
  case SEQ_INIT:
    // トレースシステムが有効 and トレース処理中
    if( FIELD_CAMERA_CheckTraceSys(camera) && FIELD_CAMERA_CheckTrace(camera) ) {
        FIELD_CAMERA_StopTraceRequest( camera );
        *seq = SEQ_WAIT_CAMERA_TRACE;
    }
    else {
      *seq = SEQ_DOOR_IN_ANIME;
    }
    break;

  // カメラのトレース処理終了待ち
  case SEQ_WAIT_CAMERA_TRACE: 
    if( FIELD_CAMERA_CheckTraceSys( camera ) == FALSE ) { 
      *seq = SEQ_DOOR_IN_ANIME; 
    }
    else if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ) {
      FIELD_CAMERA_FreeTarget( camera );
      *seq = SEQ_DOOR_IN_ANIME;
    }
    break;

  // ドア進入アニメ
  case SEQ_DOOR_IN_ANIME:
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorInAnime( gameSystem, fieldmap, &work->nextLocation, 
                                FALSE, work->seasonDisplayFlag, work->fadeOutType,
                                work->exitType ) );
    *seq = SEQ_EXIT;
    break;

  // イベント終了
  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 

//---------------------------------------------------------------------------------------
/**
 * @breif 侵入時の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeIntrude( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch ( *seq ) {
  // BGM再生準備
  case 0:
    { 
      u16 nowZoneID;

      nowZoneID = FIELDMAP_GetZoneID( fieldmap );
      FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
    }
    (*seq)++;
    break;

  // 画面フェードアウト
  case 1:
    PMSND_PlaySE( INTSE_WARP );
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // イベント終了
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
