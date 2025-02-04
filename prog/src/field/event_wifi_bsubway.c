//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_wifi_bsubway.c
 *	@brief  Wifiバトルサブウェイ  イベント
 *	@author	tomoya takahashi
 *	@date		2010.02.19
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"


#include "event_fieldmap_control.h"


#include "event_wifi_bsubway.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	シーケンス
//=====================================
enum {
  SEQ_IS_FIELD_COMM,
  SEQ_FIELD_FADEOUT,
  SEQ_FIELD_CLOSE,
  SEQ_CALL_BSUBWAY,
  SEQ_CALL_BSUBWAY_WAIT,
  SEQ_FIELD_OPEN,
  SEQ_FIELD_FADEIN,
  SEQ_END,
} ;


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WIFI_BSUBWAY_EVENT_WORK
//=====================================
typedef struct {
  WIFI_BSUBWAY_PARAM  param;
  u16* ret_wk;
} WIFI_BSUBWAY_EVENT_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT EV_WIFIBSUBWAY_Proc( GMEVENT *event, int *seq, void *wk );
static GMEVENT * WIFI_BSUBWAY_EVENT_Start( GAMESYS_WORK *gsys, WIFI_BSUBWAY_MODE mode, u16* ret_wk );

//----------------------------------------------------------------------------
/**
 *	@brief  バトルサブウェイ　WiFi　アップロード　ダウンロード　イベント　生成
 */
///	GMEVENT_CreateOverlayEventCall関数用コールバック関数
//-----------------------------------------------------------------------------
GMEVENT * WIFI_BSUBWAY_EVENT_CallStart(
    GAMESYS_WORK *gsys, void* p_work )
{
  EV_WIFIBSUBWAY_PARAM* param = p_work;
  
  return WIFI_BSUBWAY_EVENT_Start( gsys, param->mode, param->ret_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトルサブウェイ　WiFi　アップロード　ダウンロード　イベント　生成
 *
 *	@param	gsys    ゲームシステム
 *	@param	mode    起動モード
 *	@param  ret_wk  戻り値格納先
 *
 *	@return イベント
 */
//-----------------------------------------------------------------------------
static GMEVENT * WIFI_BSUBWAY_EVENT_Start( GAMESYS_WORK *gsys, WIFI_BSUBWAY_MODE mode, u16* ret_wk )
{
  GMEVENT * p_event;
  GAMEDATA * p_gdata;
  WIFI_BSUBWAY_EVENT_WORK * p_wk;

  p_gdata = GAMESYSTEM_GetGameData( gsys );

  
  p_event = GMEVENT_Create( gsys, NULL,
      EV_WIFIBSUBWAY_Proc, sizeof(WIFI_BSUBWAY_EVENT_WORK) );


  p_wk = GMEVENT_GetEventWork( p_event );
  p_wk->param.mode = mode;
  p_wk->param.p_gamesystem = gsys;
  p_wk->ret_wk = ret_wk;

  if(GAME_COMM_NO_NULL!= GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys))){
    GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gsys));
  }

  return p_event;
}




//--------------------------------------------------------------
/**
 * WiFiBsubway  イベント
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EV_WIFIBSUBWAY_Proc( GMEVENT *event, int *seq, void *wk )
{
  WIFI_BSUBWAY_EVENT_WORK*work = wk;
  GAMESYS_WORK *gsys = work->param.p_gamesystem;
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  switch( *seq )
  {
  case SEQ_IS_FIELD_COMM:
    if(GAME_COMM_NO_NULL == GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys)))
    {
      (*seq)++;
    }
    break;

  case SEQ_FIELD_FADEOUT:
    GMEVENT_CallEvent( event,
      EVENT_FieldFadeOut_Black(gsys,fieldmap,FIELD_FADE_WAIT) );
    (*seq)++;
    break;

  case SEQ_FIELD_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
    (*seq)++;
    break;

  case SEQ_CALL_BSUBWAY:
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifi_bsubway), &WIFI_BSUBWAY_Proc, &work->param );
    (*seq)++;
    break;

  case SEQ_CALL_BSUBWAY_WAIT:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){

      GF_ASSERT( work->ret_wk );
      if( work->param.result == WIFI_BSUBWAY_RESULT_OK ){
        *work->ret_wk = TRUE; // 処理成功
      }else{
        *work->ret_wk = FALSE;  // 処理失敗
      }
      (*seq) ++;
    }
    break;

  case SEQ_FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;

  case SEQ_FIELD_FADEIN:
    GMEVENT_CallEvent( event,
      EVENT_FieldFadeIn_Black(gsys,fieldmap,FIELD_FADE_WAIT) );
    (*seq)++;
    break;

  case SEQ_END:
    return( GMEVENT_RES_FINISH );
  }
  
  return( GMEVENT_RES_CONTINUE );
}
