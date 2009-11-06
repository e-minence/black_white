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

#include "event_fieldmap_control.h" // EVENT_FieldFadeOut
#include "event_entrance_effect.h"  // EVENT_FieldDoorInAnime
#include "event_fldmmdl_control.h"  // EVENT_PlayerOneStepAnime
#include "field_bgm_control.h"
#include "sound/pm_sndsys.h"
#include "field_sound.h"
#include "sound/bgm_info.h"
#include "../../resource/sound/bgm_info/iss_type.h"


//=======================================================================================
/**
 * @brief イベント・ワーク
 */
//=======================================================================================
typedef struct
{
  GAMESYS_WORK*    gsys;
  GAMEDATA*        gdata;
  FIELDMAP_WORK* fieldmap;
  LOCATION         location;  // 遷移先指定
}
EVENT_WORK;


//=======================================================================================
/**
 * @breif 非公開関数のプロトタイプ宣言
 */
//======================================================================================= 

// 各EXIT_TYPEごとのイベント
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep(GMEVENT * event, int *seq, void * work);


//=======================================================================================
/**
 * @breif 公開関数の定義
 */
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief 出入口進入イベントを作成する
 *
 * @param parent    親イベント
 * @param gsys      ゲームシステム
 * @param gdata     ゲームデータ
 * @param fieldmap  フィールドマップ
 * @param location  遷移先指定
 * @param exit_type 遷移タイプ指定
 *
 * @return 作成したイベント
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceIn( GMEVENT* parent, 
                           GAMESYS_WORK* gsys,
                           GAMEDATA* gdata, 
                           FIELDMAP_WORK* fieldmap, 
                           LOCATION location, 
                           EXIT_TYPE exit_type )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // イベントテーブル
  const GMEVENT_FUNC eventFuncTable[] = 
  {
    EVENT_FUNC_EntranceIn_ExitTypeNone,   //EXIT_TYPE_NONE
    EVENT_FUNC_EntranceIn_ExitTypeNone,   //EXIT_TYPE_MAT
    EVENT_FUNC_EntranceIn_ExitTypeStep,   //EXIT_TYPE_STAIRS
    EVENT_FUNC_EntranceIn_ExitTypeDoor,   //EXIT_TYPE_DOOR
    EVENT_FUNC_EntranceIn_ExitTypeStep,   //EXIT_TYPE_WALL
  };

  // イベント作成
  event = GMEVENT_Create( gsys, parent, eventFuncTable[ exit_type ], sizeof( EVENT_WORK ) );

  // イベント・ワークを初期化
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->gdata    = gdata;
  work->fieldmap = fieldmap;
  work->location = location;

  // 作成したイベントを返す
  return event;
}


//=======================================================================================
/**
 * @breif 非公開関数の定義
 */
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @breif ドアなし時の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone(GMEVENT * event, int *seq, void * work)
{
	EVENT_WORK*      event_work = work;
	GAMESYS_WORK*    gsys       = event_work->gsys;
	FIELDMAP_WORK* fieldmap   = event_work->fieldmap;
	GAMEDATA*        gamedata   = event_work->gdata;

  switch ( *seq )
  {
  case 0:
    FIELD_BGM_CONTROL_FadeOut( gamedata, event_work->location.zone_id, 30 );
		GMEVENT_CallEvent( event, EVENT_FieldFadeOut(gsys, fieldmap, 0) );
    ++ *seq;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif ドアあり時の進入イベント
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor(GMEVENT * event, int *seq, void * work)
{
	EVENT_WORK*      event_work = work;
	GAMESYS_WORK*    gsys       = event_work->gsys;
	FIELDMAP_WORK* fieldmap   = event_work->fieldmap;

  switch ( *seq )
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_FieldDoorInAnime( gsys, fieldmap, &event_work->location ) );
    ++ *seq;
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
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep(GMEVENT * event, int *seq, void * work)
{
	EVENT_WORK*      event_work = work;
	GAMESYS_WORK*    gsys       = event_work->gsys;
	FIELDMAP_WORK* fieldmap   = event_work->fieldmap;
	GAMEDATA*        gamedata   = event_work->gdata;

  switch ( *seq )
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gsys, fieldmap) );
    ++ *seq;
    break;
  case 1: // BGMフェードアウト
    { // 現在のBGMがダンジョンISS && 次のBGMもダンジョンISS ==> BGMフェードアウト
      BGM_INFO_SYS* bgm_info = GAMEDATA_GetBGMInfoSys( gamedata );
      PLAYER_WORK* player = GAMEDATA_GetPlayerWork( gamedata, 0 );
      PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
      u32 bgm_next = FIELD_SOUND_GetFieldBGMNo( gamedata, form, event_work->location.zone_id );
      u32 bgm_now = PMSND_GetBGMsoundNo();
      u8 iss_type_next = BGM_INFO_GetIssType( bgm_info, bgm_next ); 
      u8 iss_type_now = BGM_INFO_GetIssType( bgm_info, bgm_now ); 
      if( ( iss_type_next == ISS_TYPE_DUNGEON ) &&
          ( iss_type_now == ISS_TYPE_DUNGEON ) )
      {
        PMSND_FadeOutBGM( 20 );
      }
    }
    ++ *seq;
    break;
  case 2: // BGMフェードアウト待ち
    if( PMSND_CheckFadeOnBGM() != TRUE )
    { 
      PMSND_PlaySE( SEQ_SE_KAIDAN );    // SE
      GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, fieldmap, 0));  // 画面フェードアウト
      ++ *seq;
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
