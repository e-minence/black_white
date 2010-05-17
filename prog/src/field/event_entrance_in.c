/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_in.c
 * @breif  �o�����ւ̐i�����̃C�x���g�쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h> 
#include "gamesystem/game_event.h"
#include "gamesystem/iss_sys.h"
#include "gamesystem/iss_dungeon_sys.h"
#include "sound/pm_sndsys.h"
#include "field/eventdata_type.h"
#include "field/fieldmap_proc.h"
#include "field/location.h"
#include "field_comm/intrude_snd_def.h"

#include "fieldmap.h"
#include "field_sound.h"
#include "event_entrance_in.h" 
#include "event_fieldmap_control.h"  // for EVENT_FieldFadeOut_xxxx
#include "event_entrance_effect.h"   // for EVENT_FieldDoorInAnime
#include "event_fldmmdl_control.h"   // for PlayerOneStepStart, CheckPlayerOneStepEnd
#include "event_disappear.h"         // for EVENT_DISAPPEAR_xxxx
#include "entrance_event_common.h"   // for ENTRANCE_EVDATA



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
// ���C�x���g ���[�N
//=======================================================================================
typedef struct {

  FIELD_CAMERA_MODE initCameraMode;  // �C�x���g�J�n���̃J�������[�h
  GFL_TCB*          oneStepTCB;      // ���@�̈���ړ�TCB

  // �o������C�x���g�̋��ʃ��[�N
  ENTRANCE_EVDATA* evdata; 

} EVENT_WORK;


//=======================================================================================
// ��index
//======================================================================================= 
// �e EXIT_TYPE ���Ƃ̃C�x���g
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSPx( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeIntrude( GMEVENT * event, int *seq, void * wk );
// BGM
static BOOL CheckBGMFadeOut_at_Dungeon( EVENT_WORK* work );
static void StandByNextBGM( EVENT_WORK* work );
static void CallBGMFadeOutEvent( EVENT_WORK* work, GMEVENT* parentEvent );
// ��ʂ̃t�F�[�h�A�E�g
static void CallFadeOutEvent( const EVENT_WORK* work, GMEVENT* parentEvent );

//=======================================================================================
// ��public func
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �o�����i���C�x���g���쐬����
 *
 * @param evdata �o������C�x���g�̋��ʃf�[�^
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceIn( ENTRANCE_EVDATA* evdata )
{
  GMEVENT* event;
  EVENT_WORK* work;
  EVENTFUNC_TYPE funcType;

  // �C�x���g�e�[�u��
  const GMEVENT_FUNC eventFuncTable[ EVENTFUNC_TYPE_NUM ] = 
  {
    EVENT_FUNC_EntranceIn_ExitTypeNone,   // EVENTFUNC_TYPE_NONE �h�A�Ȃ�
    EVENT_FUNC_EntranceIn_ExitTypeDoor,   // EVENTFUNC_TYPE_DOOR �h�A����
    EVENT_FUNC_EntranceIn_ExitTypeStep,   // EVENTFUNC_TYPE_STEP �K�i
    EVENT_FUNC_EntranceIn_ExitTypeWarp,   // EVENTFUNC_TYPE_WARP ���[�v
    EVENT_FUNC_EntranceIn_ExitTypeSPx,    // EVENTFUNC_TYPE_SPx, ����
    EVENT_FUNC_EntranceIn_ExitTypeIntrude,// EVENTFUNC_TYPE_INTRUDE �N��
  };

  // �C�x���g�����֐�������
  switch( evdata->exit_type_in ) {
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

  // �C�x���g���[�N������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->oneStepTCB = NULL; 
  work->evdata     = evdata;

  return event;
}


//=======================================================================================
// ��private func
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A�Ȃ����̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;
	GAMEDATA*        gameData   = evdata->gameData;
  FIELD_SOUND*     fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch ( *seq ) {
  case 0:
    // BGM �ύX����
    StandByNextBGM( work );

    // SE �Đ�
    if( evdata->fadeout_type != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }

    // ��ʃt�F�[�h�A�E�g�J�n
    CallFadeOutEvent( work, event );

    (*seq)++; 
    break;

  case 1:
    return GMEVENT_RES_FINISH; // �C�x���g�I��
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A���莞�̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  switch( *seq ) {
  case 0:
    // �h�A�i���C�x���g
    GMEVENT_CallEvent( event, EVENT_FieldDoorInAnime( evdata ) );
    (*seq)++;
    break;

  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �K�i�̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;
	GAMEDATA*        gameData   = evdata->gameData;
  FIELD_SOUND*     fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch ( *seq ) {
  case 0:  
    // BGM ����
    if( CheckBGMFadeOut_at_Dungeon(work) ) {
      // BGM �t�F�[�h�A�E�g
      CallBGMFadeOutEvent( work, event );
    }
    else {
      // BGM �Đ�����
      StandByNextBGM( work );
    }

    // ���@�̈���ړ��A�j���J�n
    work->oneStepTCB = PlayerOneStepAnimeStart( fieldmap );

    (*seq)++;
    break;

  case 1:
    // ���@�̈���ړ��A�j�����I��
    if( CheckPlayerOneStepAnimeEnd( work->oneStepTCB ) ) {
      // �N���X�t�F�[�h�łȂ����, SE���Đ�
      if( evdata->fadeout_type != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
      // ��ʃt�F�[�h�A�E�g�J�n
      CallFadeOutEvent( work, event ); 
      (*seq)++; 
    }
    break;

  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif ���[�v���̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  switch( *seq ) {
  case 0:
    // ���[�v�ޏo�C�x���g
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
 * @breif �i���C�x���g( SPx )
 *
 * ���J�����̃A�j���[�V���� �� �h�A�i���A�j��
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSPx( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;
  FIELD_CAMERA*    camera     = FIELDMAP_GetFieldCamera( fieldmap );

  // �����V�[�P���X
  enum {
    SEQ_INIT,               // �C�x���g�J�n
    SEQ_WAIT_CAMERA_TRACE,  // �J�����̎��@�Ǐ]�����̏I���҂�
    SEQ_DOOR_IN_ANIME,      // �h�A�i���C�x���g
    SEQ_EXIT,               // �C�x���g�I��
  };

  switch( *seq ) {
  case SEQ_INIT:
    // �g���[�X�V�X�e�����L�� and �g���[�X������
    if( FIELD_CAMERA_CheckTraceSys(camera) && FIELD_CAMERA_CheckTrace(camera) ) {
        FIELD_CAMERA_StopTraceRequest( camera );
        *seq = SEQ_WAIT_CAMERA_TRACE;
    }
    else {
      *seq = SEQ_DOOR_IN_ANIME;
    }
    break;

  // �J�����̃g���[�X�����I���҂�
  case SEQ_WAIT_CAMERA_TRACE: 
    if( FIELD_CAMERA_CheckTraceSys( camera ) == FALSE ) { 
      *seq = SEQ_DOOR_IN_ANIME; 
    }
    else if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ) {
      FIELD_CAMERA_FreeTarget( camera );
      *seq = SEQ_DOOR_IN_ANIME;
    }
    break;

  // �h�A�i���A�j��
  case SEQ_DOOR_IN_ANIME:
    GMEVENT_CallEvent( event, EVENT_FieldDoorInAnime( evdata ) );
    *seq = SEQ_EXIT;
    break;

  // �C�x���g�I��
  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 

//---------------------------------------------------------------------------------------
/**
 * @breif �N�����̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeIntrude( GMEVENT * event, int *seq, void * wk )
{
	EVENT_WORK*      work       = wk;
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;
	GAMEDATA*        gameData   = evdata->gameData;
  FIELD_SOUND*     fieldSound = GAMEDATA_GetFieldSound( gameData );

  switch ( *seq ) {
  case 0:
    // BGM �ύX����
    StandByNextBGM( work );
    PMSND_PlaySE( INTSE_WARP );
    // ��ʃt�F�[�h�A�E�g�J�n
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  case 1:
    return GMEVENT_RES_FINISH; // �C�x���g�I��
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief �_���W�������� BGM ������@�����肷��
 *
 * @param work
 *
 * @return BGM ���t�F�[�h�A�E�g������ꍇ TRUE
 */
//---------------------------------------------------------------------------------------
static BOOL CheckBGMFadeOut_at_Dungeon( EVENT_WORK* work )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
  ISS_SYS*         iss        = GAMESYSTEM_GetIssSystem( gameSystem );
  ISS_DUNGEON_SYS* issD       = ISS_SYS_GetIssDungeonSystem( iss );

  // �J�ڂ̑O��Ƃ���ISS-D�̐��䂪�L��
  if( ISS_DUNGEON_SYS_IsOn( issD ) &&
      ISS_DUNGEON_SYS_IsActiveAt(issD, evdata->prev_zone_id) && 
      ISS_DUNGEON_SYS_IsActiveAt(issD, evdata->nextLocation.zone_id) ) {
    return TRUE;
  }

  return FALSE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief BGM �̕ύX����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------
static void StandByNextBGM( EVENT_WORK* work )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMEDATA*        gameData   = evdata->gameData;
  FIELD_SOUND*     fieldSound = GAMEDATA_GetFieldSound( gameData );

  FSND_StandByNextMapBGM( fieldSound, gameData, evdata->nextLocation.zone_id );

  evdata->BGM_standby_flag = TRUE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief BGM �t�F�[�h�A�E�g�C�x���g���Ă�
 *
 * @param work
 * @param parentEvent
 */
//---------------------------------------------------------------------------------------
static void CallBGMFadeOutEvent( EVENT_WORK* work, GMEVENT* parentEvent )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem; 
  GMEVENT* event;

  event = EVENT_FSND_FadeOutBGM( gameSystem, FSND_FADE_SHORT );
  GMEVENT_CallEvent( parentEvent, event );

  evdata->BGM_fadeout_flag = TRUE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief ��ʂ̃t�F�[�h�A�E�g�C�x���g���Ăяo��
 *
 * @param work
 * @param parentEvent
 */
//---------------------------------------------------------------------------------------
static void CallFadeOutEvent( const EVENT_WORK* work, GMEVENT* parentEvent )
{
  ENTRANCE_EVDATA* evdata     = work->evdata;
	GAMESYS_WORK*    gameSystem = evdata->gameSystem;
	FIELDMAP_WORK*   fieldmap   = evdata->fieldmap;

  // �G�߃t�F�[�h
  if( evdata->season_disp_flag ) { 
    GMEVENT_CallEvent( parentEvent, 
        EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
  }
  // ��{�t�F�[�h
  else { 
    GMEVENT_CallEvent( parentEvent, 
        EVENT_FieldFadeOut( gameSystem, fieldmap, evdata->fadeout_type, FIELD_FADE_WAIT ) );
  }
}
