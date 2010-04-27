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

  GAMESYS_WORK*     gameSystem;
  GAMEDATA*         gameData;
  FIELDMAP_WORK*    fieldmap;
  LOCATION          nextLocation;      // �J�ڐ�w��
  EXIT_TYPE         exitType;          // �o������^�C�v
  BOOL              seasonDisplayFlag; // �G�ߕ\�����s�����ǂ���
  FIELD_FADE_TYPE   fadeOutType;       // �G�ߕ\�����Ȃ��ꍇ��F/O�^�C�v
  BOOL              BGMFadeWaitFlag;   // BGM �̃t�F�[�h������҂��ǂ���
  FIELD_CAMERA_MODE initCameraMode;    // �C�x���g�J�n���̃J�������[�h
  u16               zoneID;            // �J�ڑO�̃]�[��ID
  u16               nextZoneID;        // �J�ڌ�̃]�[��ID
  GFL_TCB*          oneStepTCB;        // ���@�̈���ړ�TCB

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


//=======================================================================================
// ��public func
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �o�����i���C�x���g���쐬����
 *
 * @param parent            �e�C�x���g
 * @param gameSystem
 * @param gameData
 * @param fieldmap
 * @param nextLocation      �J�ڐ�w��
 * @param exitType          �J�ڃ^�C�v�w��
 * @param seasonDisplayFlag �G�ߕ\�����s�����ǂ���
 *
 * @return �쐬�����C�x���g
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
  switch( exitType ) {
  case EXIT_TYPE_NONE:    funcType = EVENTFUNC_TYPE_NONE;    break;
  case EXIT_TYPE_MAT:     funcType = EVENTFUNC_TYPE_NONE;    break;
  case EXIT_TYPE_STAIRS:  funcType = EVENTFUNC_TYPE_STEP;    break;
  case EXIT_TYPE_DOOR:    funcType = EVENTFUNC_TYPE_DOOR;    break;
  case EXIT_TYPE_WALL:    funcType = EVENTFUNC_TYPE_STEP;    break;
  case EXIT_TYPE_WARP:    funcType = EVENTFUNC_TYPE_WARP;    break;
  case EXIT_TYPE_INTRUDE: funcType = EVENTFUNC_TYPE_INTRUDE; break;
  default:                funcType = EVENTFUNC_TYPE_SPx;     break;
  }

  // ��{�t�F�[�h�A�E�g�̎�ނ�����
  prevZoneID  = FIELDMAP_GetZoneID( fieldmap );
  nextZoneID  = nextLocation.zone_id;
  fadeOutType = FIELD_FADE_GetFadeOutType( prevZoneID, nextZoneID );

  // �C�x���g�쐬
  event = GMEVENT_Create( gameSystem, parent, eventFuncTable[ funcType ], sizeof( EVENT_WORK ) );

  // �C�x���g���[�N������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem        = gameSystem;
  work->gameData          = gameData;
  work->fieldmap          = fieldmap;
  work->nextLocation      = nextLocation;
  work->exitType          = exitType;
  work->seasonDisplayFlag = seasonDisplayFlag;
  work->fadeOutType       = fadeOutType;
  work->zoneID            = prevZoneID;
  work->nextZoneID        = nextZoneID;
  work->oneStepTCB        = NULL;

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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch ( *seq ) {
  // BGM�Đ�����
  case 0:
    { 
      u16 nowZoneID;

      nowZoneID = FIELDMAP_GetZoneID( fieldmap );
      FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
    }
    (*seq)++;
    break;

  // SE �Đ�
  case 1:
    if( work->fadeOutType != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
    (*seq)++; 
    break;

  // ��ʃt�F�[�h�A�E�g
  case 2:
    if( work->seasonDisplayFlag )
    { // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    }
    else
    { // ��{�t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut( gameSystem, fieldmap, work->fadeOutType, FIELD_FADE_WAIT ) );
    }
    (*seq)++;
    break;

  // �C�x���g�I��
  case 3:
    return GMEVENT_RES_FINISH;
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch( *seq ) {
  // �h�A�i���C�x���g
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
 * @breif �K�i�̐i���C�x���g
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
    // ���@�̈���ړ��A�j���J�n
    work->oneStepTCB = PlayerOneStepAnimeStart( fieldmap );

    // ���݂�BGM���_���W����ISS && ����BGM���_���W����ISS ==> BGM�t�F�[�h�A�E�g
    { 
      ISS_SYS* iss = GAMESYSTEM_GetIssSystem( gameSystem );
      ISS_DUNGEON_SYS* issD = ISS_SYS_GetIssDungeonSystem( iss );
      // �J�ڂ̑O��Ƃ���ISS-D�̐��䂪�L��
      if( ISS_DUNGEON_SYS_IsActiveAt(issD, work->zoneID) && 
          ISS_DUNGEON_SYS_IsActiveAt(issD, work->nextZoneID) ) {
        // BGM �t�F�[�h�A�E�g
        GMEVENT* fadeOutEvent = EVENT_FSND_FadeOutBGM( gameSystem, FSND_FADE_SHORT );
        GMEVENT_CallEvent( event, fadeOutEvent );
        work->BGMFadeWaitFlag = TRUE; // BGM�t�F�[�h��҂�
      }
      else { 
        // BGM �Đ�����
        FIELD_SOUND* fieldSound = GAMEDATA_GetFieldSound( gameData );
        u16 nowZoneID = FIELDMAP_GetZoneID( fieldmap );
        FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
        work->BGMFadeWaitFlag = FALSE; // BGM�t�F�[�h�͑҂��Ȃ�
      }
    }
    (*seq)++;
    break;

  case 1:
    // ���@�̈���ړ��A�j���I���҂�
    if( CheckPlayerOneStepAnimeEnd( work->oneStepTCB ) ) { (*seq)++; }
    break;

  case 2: 
    // �N���X�t�F�[�h�łȂ����, SE���Đ�
    if( work->fadeOutType != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
    (*seq)++; 
    break;

  case 3:
    // ��ʃt�F�[�h�A�E�g
    if( work->seasonDisplayFlag ) { 
      // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ) );
    }
    else { 
      // ��{�t�F�[�h
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
 * @breif ���[�v���̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch( *seq ) {
  // ���[�v�ޏo�C�x���g
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
 * @breif �i���C�x���g( SPx )
 *
 * ���J�����̃A�j���[�V���� �� �h�A�i���A�j��
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSPx( GMEVENT* event, int* seq, void* wk )
{
	EVENT_WORK*     work       = wk;
	GAMESYS_WORK*   gameSystem = work->gameSystem;
	FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_CAMERA*   camera     = FIELDMAP_GetFieldCamera( fieldmap );

  // �����V�[�P���X
  enum {
    SEQ_INIT,                       // �C�x���g�J�n
    SEQ_WAIT_CAMERA_TRACE,          // �J�����̎��@�Ǐ]�����̏I���҂�
    SEQ_DOOR_IN_ANIME,              // �h�A�i���C�x���g
    SEQ_EXIT,                       // �C�x���g�I��
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
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorInAnime( gameSystem, fieldmap, &work->nextLocation, 
                                FALSE, work->seasonDisplayFlag, work->fadeOutType,
                                work->exitType ) );
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch ( *seq ) {
  // BGM�Đ�����
  case 0:
    { 
      u16 nowZoneID;

      nowZoneID = FIELDMAP_GetZoneID( fieldmap );
      FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
    }
    (*seq)++;
    break;

  // ��ʃt�F�[�h�A�E�g
  case 1:
    PMSND_PlaySE( INTSE_WARP );
    GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Cross( gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // �C�x���g�I��
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
