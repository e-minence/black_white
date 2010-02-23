//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_wifi_bsubway.c
 *	@brief  Wifi�o�g���T�u�E�F�C  �C�x���g
 *	@author	tomoya takahashi
 *	@date		2010.02.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"


#include "event_fieldmap_control.h"


#include "event_wifi_bsubway.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�[�P���X
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
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	WIFI_BSUBWAY_EVENT_WORK
//=====================================
typedef struct {
  WIFI_BSUBWAY_PARAM  param;
} WIFI_BSUBWAY_EVENT_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static GMEVENT_RESULT EV_WIFIBSUBWAY_Proc( GMEVENT *event, int *seq, void *wk );

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���T�u�E�F�C�@WiFi�@�A�b�v���[�h�@�_�E�����[�h�@�C�x���g�@����
 *
 *	@param	gsys    �Q�[���V�X�e��
 *	@param	mode    �N�����[�h
 *
 *	@return �C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * WIFI_BSUBWAY_EVENT_Start( GAMESYS_WORK *gsys, WIFI_BSUBWAY_MODE mode )
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

  if(GAME_COMM_NO_NULL!= GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys))){
    GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gsys));
  }

  return p_event;
}




//--------------------------------------------------------------
/**
 * WiFiBsubway  �C�x���g
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
      TOMOYA_Printf( "WiFi bsubway Result %d\n", work->param.result );
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
