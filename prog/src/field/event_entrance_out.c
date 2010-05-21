/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_out.c
 * @breif  �o��������̑ޏo���̃C�x���g�쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/game_event.h"
#include "gamesystem/iss_sys.h"
#include "gamesystem/iss_dungeon_sys.h"
#include "field/eventdata_type.h"
#include "field/fieldmap_proc.h"
#include "field/location.h" 
#include "field/eventdata_sxy.h"
#include "field/zonedata.h"            // for ZONEDATA_GetBGMID
#include "sound/pm_sndsys.h"
#include "sound/bgm_info.h"

#include "field_sound.h" 
#include "event_appear.h"              // for EVENT_APPEAR_xxxx
#include "event_fieldmap_control.h"    // for EVENT_FieldFadeIn
#include "event_entrance_effect.h"     // for EVENT_FieldDoorOutAnime
#include "event_fldmmdl_control.h"     // for EVENT_PlayerOneStepAnime
#include "field_place_name.h"          // for FIELD_PLACE_NAME_DisplayOnStanderdRule
#include "fieldmap.h"                  // for FIELDMAP_GetPlaceNameSys

#include "event_entrance_out.h"



//=======================================================================================
// ���萔
//=======================================================================================
// �C�x���g�����֐��̃^�C�v
typedef enum {
  EVENTFUNC_TYPE_NONE,    // �h�A�Ȃ�
  EVENTFUNC_TYPE_DOOR,    // �h�A����
  EVENTFUNC_TYPE_STEP,    // �K�i
  EVENTFUNC_TYPE_WARP,    // ���[�v
  EVENTFUNC_TYPE_SPx,     // ����
  EVENTFUNC_TYPE_INTRUDE, // �N��
  EVENTFUNC_TYPE_NUM,
} EVENTFUNC_TYPE;


//=======================================================================================
// ���C�x���g���[�N
//=======================================================================================
typedef struct {

  ENTRANCE_EVDATA* evdata; // �o������C�x���g�̋��ʃ��[�N

} EVENT_WORK;


//=======================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//======================================================================================= 
// �eEXIT_TYPE���Ƃ̃C�x���g
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeNone( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeDoor( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeStep( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeWarp( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeSPx( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeIntrude( GMEVENT* event, int* seq, void* wk );
// BGM
static void StartStandByBGM( EVENT_WORK* work );
static void CallBGMFadeInEvent( EVENT_WORK* work, GMEVENT* parentEvent );
// ��ʂ̃t�F�[�h�C��
static void CallFadeInEvent( const EVENT_WORK* work, GMEVENT* parentEvent );
// �n���\��
static void PutPlaceNameRequest( EVENT_WORK* work );


//=======================================================================================
// �����J�֐��̒�`
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �o�����ޏo�C�x���g���쐬����
 *
 * @param evdata �o������C�x���g���ʃ��[�N
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceOut( ENTRANCE_EVDATA* evdata )
{
  GMEVENT* event;
  EVENT_WORK* work; 
  EVENTFUNC_TYPE funcType;

  // �C�x���g�e�[�u��
  const GMEVENT_FUNC eventFuncTable[ EVENTFUNC_TYPE_NUM ] = 
  {
    EVENT_FUNC_EntranceOut_ExitTypeNone,   // EVENTFUNC_TYPE_NONE �h�A�Ȃ�
    EVENT_FUNC_EntranceOut_ExitTypeDoor,   // EVENTFUNC_TYPE_DOOR �h�A����
    EVENT_FUNC_EntranceOut_ExitTypeStep,   // EVENTFUNC_TYPE_STEP �K�i
    EVENT_FUNC_EntranceOut_ExitTypeWarp,   // EVENTFUNC_TYPE_WARP ���[�v
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EVENTFUNC_TYPE_SPx, ����
    EVENT_FUNC_EntranceOut_ExitTypeIntrude, // EVENTFUNC_TYPE_INTRUDE �N��
  };

  // �C�x���g�����֐�������
  switch( evdata->exit_type_out ) {
  case EXIT_TYPE_NONE:    funcType = EVENTFUNC_TYPE_NONE;    break;
  case EXIT_TYPE_MAT:     funcType = EVENTFUNC_TYPE_NONE;    break;
  case EXIT_TYPE_STAIRS:  funcType = EVENTFUNC_TYPE_STEP;    break;
  case EXIT_TYPE_DOOR:    funcType = EVENTFUNC_TYPE_DOOR;    break;
  case EXIT_TYPE_WALL:    funcType = EVENTFUNC_TYPE_STEP;    break;
  case EXIT_TYPE_WARP:    funcType = EVENTFUNC_TYPE_WARP;    break;
  case EXIT_TYPE_INTRUDE: funcType = EVENTFUNC_TYPE_INTRUDE; break;
  default:                funcType = EVENTFUNC_TYPE_SPx;     break;
  }

  // �C�x���g�쐬
  event = GMEVENT_Create( evdata->gameSystem, evdata->parentEvent, 
      eventFuncTable[ funcType ], sizeof( EVENT_WORK ) );

  // �C�x���g�E���[�N��������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->evdata = evdata;

  return event;
}


//=======================================================================================
// ������J�֐��̒�`
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A�Ȃ����̑ޏo�C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeNone( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;

  switch( *seq ) {
  case 0:
    // BGM�Đ��J�n
    if( evdata->BGM_standby_flag ) {
      StartStandByBGM( work );
    }
    // ��ʃt�F�[�h�J�n
    CallFadeInEvent( work, event );
    (*seq)++;
    break;

  case 1:
    PutPlaceNameRequest( work ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A���莞�̑ޏo�C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeDoor( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work   = wk;
  ENTRANCE_EVDATA* evdata = work->evdata;

  switch( *seq ) {
  case 0:
    // �h�A�ޏo�C�x���g
    GMEVENT_CallEvent( event, EVENT_FieldDoorOutAnime( evdata ) );
    (*seq)++;
    break;

  case 1:
    PutPlaceNameRequest( work ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH; // �C�x���g�I��
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �K�i�̑ޏo�C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeStep( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  switch( *seq ) {
  case 0: 
    // ��ʃt�F�[�h�C���J�n
    CallFadeInEvent( work, event );
    (*seq)++;
    break;

  case 1:  
    // BGM ����
    if( evdata->BGM_standby_flag ) {
      StartStandByBGM( work );
    }
    else if( evdata->BGM_fadeout_flag ) {
      CallBGMFadeInEvent( work, event );
    }
    (*seq)++;
    break;

  case 2:
    // ���@�̈���ړ��A�j��
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    (*seq)++;
    break;

  case 3:
    PutPlaceNameRequest( work ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH; // �C�x���g�I��
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif ���[�v���̑ޏo�C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeWarp( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  switch( *seq ) {
  case 0:
    // ���[�v�o���C�x���g
		GMEVENT_CallEvent(event, EVENT_APPEAR_Warp( NULL, gameSystem, fieldmap ) );
    (*seq)++;
    break;

  case 1:
    PutPlaceNameRequest( work ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH; // �C�x���g�I��
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �ޏo�C�x���g( SPx )
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeSPx( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*      work     = wk;
  ENTRANCE_EVDATA* evdata   = work->evdata;
	FIELDMAP_WORK*   fieldmap = evdata->fieldmap;
  FIELD_CAMERA*    camera   = FIELDMAP_GetFieldCamera( fieldmap );

  switch( *seq ) {
  case 0:
    // �h�A�ޏo�C�x���g
    GMEVENT_CallEvent( event, EVENT_FieldDoorOutAnime( evdata ) );
    (*seq)++;
    break;

  case 1:
    PutPlaceNameRequest( work ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 

//---------------------------------------------------------------------------------------
/**
 * @breif �N�����̑ޏo�C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeIntrude( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  switch( *seq ) {
  case 0:
    // BGM�Đ��J�n
    if( evdata->BGM_standby_flag ) {
      StartStandByBGM( work );
    }
    (*seq)++;
    break;

  case 1:
    // ��ʃt�F�[�h�J�n
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  case 2:
    PutPlaceNameRequest( work ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH; // �C�x���g�I��
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief �X�^���o�C���� BGM �̍Đ����J�n����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------
static void StartStandByBGM( EVENT_WORK* work )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMEDATA*        gameData   = evdata->gameData;
  FIELD_SOUND*     fieldSound = GAMEDATA_GetFieldSound( gameData );

  GF_ASSERT( evdata->BGM_standby_flag );

  FSND_PlayStartBGM( fieldSound, gameData );
}

//---------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g���� BGM �̃t�F�[�h�C���C�x���g���Ăяo��
 *
 * @param work
 * @param parentEvent
 */
//---------------------------------------------------------------------------------------
static void CallBGMFadeInEvent( EVENT_WORK* work, GMEVENT* parentEvent )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	GAMEDATA*        gameData   = evdata->gameData;
  FIELD_SOUND*     fieldSound = GAMEDATA_GetFieldSound( gameData );

  GF_ASSERT( evdata->BGM_fadeout_flag );

  GMEVENT_CallEvent( parentEvent, 
      EVENT_FSND_FadeInBGM( gameSystem, FSND_FADE_FAST ) );
} 

//---------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̃t�F�[�h�C���C�x���g���Ăяo��
 *
 * @param work
 * @param parentEvent
 */
//---------------------------------------------------------------------------------------
static void CallFadeInEvent( const EVENT_WORK* work, GMEVENT* parentEvent )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  // �G�߃t�F�[�h
  if( evdata->season_disp_flag ) { 
    GMEVENT_CallEvent( parentEvent, 
        EVENT_FieldFadeIn_Season( gameSystem, fieldmap, evdata->start_season, evdata->end_season ) );
  }
  // ��{�t�F�[�h
  else {
    GMEVENT_CallEvent( parentEvent, 
        EVENT_FieldFadeIn( gameSystem, fieldmap, evdata->fadein_type, FIELD_FADE_WAIT, TRUE, 0, 0 ) );
  }
} 

//---------------------------------------------------------------------------------------
/**
 * @brief �n���\���̃��N�G�X�g�𔭍s����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------
static void PutPlaceNameRequest( EVENT_WORK* work )
{
  ENTRANCE_EVDATA* evdata   = work->evdata;

  FIELD_PLACE_NAME_DisplayOnStanderdRule( 
      FIELDMAP_GetPlaceNameSys(evdata->fieldmap), evdata->nextLocation.zone_id );
}
