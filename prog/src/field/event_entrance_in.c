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

#include "gamesystem/game_event.h"
#include "field/eventdata_type.h"
#include "field/fieldmap_proc.h"
#include "field/location.h"

#include "event_entrance_in.h"

#include "entrance_camera_settings.h"  // for ENTRANCE_CAMERA_SETTINGS
#include "event_fieldmap_control.h"    // for EVENT_FieldFadeOut_xxxx
#include "event_entrance_effect.h"     // for EVENT_FieldDoorInAnime
#include "event_fldmmdl_control.h"     // for EVENT_PlayerOneStepAnime
#include "event_disappear.h"           // for EVENT_DISAPPEAR_xxxx

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


//=======================================================================================
// ���萔
//=======================================================================================
// �C�x���g�����֐��̃^�C�v
typedef enum {
  EVENTFUNC_TYPE_NONE, // �h�A�Ȃ�
  EVENTFUNC_TYPE_DOOR, // �h�A����
  EVENTFUNC_TYPE_STEP, // �K�i
  EVENTFUNC_TYPE_WARP, // ���[�v
  EVENTFUNC_TYPE_SPx,  // ����
  EVENTFUNC_TYPE_NUM,
} EVENTFUNC_TYPE;


//=======================================================================================
// ���C�x���g ���[�N
//=======================================================================================
typedef struct {

  GAMESYS_WORK*            gameSystem;
  GAMEDATA*                gameData;
  FIELDMAP_WORK*           fieldmap;
  LOCATION                 nextLocation;      // �J�ڐ�w��
  EXIT_TYPE                exitType;          // �o������^�C�v
  BOOL                     seasonDisplayFlag; // �G�ߕ\�����s�����ǂ���
  ENTRANCE_CAMERA_SETTINGS cameraSettings;    // ����o������̃J�����ݒ�f�[�^
  FIELD_FADE_TYPE          fadeOutType;       // �G�ߕ\�����Ȃ��ꍇ��F/O�^�C�v
  BOOL                     BGMFadeWaitFlag;   // BGM �̃t�F�[�h������҂��ǂ���
  FIELD_CAMERA_MODE        initCameraMode;    // �C�x���g�J�n���̃J�������[�h

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
// �J�����̐ݒ�E���A
static void SetupCamera( EVENT_WORK* work ); // �J�������Z�b�g�A�b�v����
static void RecoverCamera( EVENT_WORK* work ); // �J���������ɖ߂�


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
  };

  // �C�x���g�����֐�������
  switch( exitType ) {
  case EXIT_TYPE_NONE:    funcType = EVENTFUNC_TYPE_NONE; break;
  case EXIT_TYPE_MAT:     funcType = EVENTFUNC_TYPE_NONE; break;
  case EXIT_TYPE_STAIRS:  funcType = EVENTFUNC_TYPE_STEP; break;
  case EXIT_TYPE_DOOR:    funcType = EVENTFUNC_TYPE_DOOR; break;
  case EXIT_TYPE_WALL:    funcType = EVENTFUNC_TYPE_STEP; break;
  case EXIT_TYPE_WARP:    funcType = EVENTFUNC_TYPE_WARP; break;
  case EXIT_TYPE_INTRUDE: funcType = EVENTFUNC_TYPE_NONE; break;
  default:                funcType = EVENTFUNC_TYPE_SPx;  break;
  }

  // ��{�t�F�[�h�A�E�g�̎�ނ�����
  prevZoneID  = FIELDMAP_GetZoneID( fieldmap );
  nextZoneID  = nextLocation.zone_id;
  fadeOutType = FIELD_FADE_GetFadeOutType( prevZoneID, nextZoneID );

  // �C�x���g�쐬
  event = GMEVENT_Create( gameSystem, parent, eventFuncTable[ funcType ], sizeof( EVENT_WORK ) );

  // �C�x���g���[�N������
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
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  // BGM�Đ�����
  case 1:
    { 
      u16 nowZoneID;

      nowZoneID = FIELDMAP_GetZoneID( fieldmap );
      FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
    }
    (*seq)++;
    break;

  // SE �Đ�
  case 2:
    if( work->fadeOutType != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
    (*seq)++; 
    break;

  // ��ʃt�F�[�h�A�E�g
  case 3:
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

  // �J�����̕��A
  case 4:
    RecoverCamera( work );
    (*seq)++;
    break; 

  // �C�x���g�I��
  case 5:
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
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  // �h�A�i���C�x���g
  case 1:
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorInAnime( 
          gameSystem, fieldmap, &(work->nextLocation), TRUE, work->seasonDisplayFlag, work->fadeOutType ) );
    (*seq)++;
    break;

  // �J�����̕��A
  case 2:
    RecoverCamera( work );
    (*seq)++;
    break; 

  case 3:
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
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  case 1:  
    // ���@�O�i
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 2: 
    { // ���݂�BGM���_���W����ISS && ����BGM���_���W����ISS ==> BGM�t�F�[�h�A�E�g
      FIELD_SOUND*  fieldSound = GAMEDATA_GetFieldSound( gameData );
      BGM_INFO_SYS* BGMInfo    = GAMEDATA_GetBGMInfoSys( gameData );
      PLAYER_WORK*  player     = GAMEDATA_GetPlayerWork( gameData, 0 );
      u32 nextBGM = FSND_GetFieldBGM( gameData, work->nextLocation.zone_id );
      u32 nowBGM = PMSND_GetBGMsoundNo();
      u8 nextIssType = BGM_INFO_GetIssType( BGMInfo, nextBGM ); 
      u8 nowIssType = BGM_INFO_GetIssType( BGMInfo, nowBGM ); 
      if( ( nextIssType == ISS_TYPE_DUNGEON ) && ( nowIssType == ISS_TYPE_DUNGEON ) ) { 
        // BGM �t�F�[�h�A�E�g
        GMEVENT* fadeOutEvent = EVENT_FSND_FadeOutBGM( gameSystem, FSND_FADE_FAST );
        GMEVENT_CallEvent( event, fadeOutEvent );
        work->BGMFadeWaitFlag = TRUE; // BGM�t�F�[�h��҂�
      }
      else { 
        // BGM �Đ�����
        u16 nowZoneID = FIELDMAP_GetZoneID( fieldmap );
        FSND_StandByNextMapBGM( fieldSound, gameData, work->nextLocation.zone_id );
        work->BGMFadeWaitFlag = FALSE; // BGM�t�F�[�h�͑҂��Ȃ�
      }
    }
    (*seq)++;
    break;
  case 3: 
    // BGM�t�F�[�h�����҂�
    if( (work->BGMFadeWaitFlag == FALSE) || (PMSND_CheckFadeOnBGM() == FALSE) )
    { 
      // SE �Đ�
      if( work->fadeOutType != FIELD_FADE_CROSS ) { PMSND_PlaySE( SEQ_SE_KAIDAN ); }
      (*seq)++; 
    }
    break;
  case 4:
    // ��ʃt�F�[�h�A�E�g
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

  // �J�����̕��A
  case 5:
    RecoverCamera( work );
    (*seq)++;
    break; 

  case 6:
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
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  // ���[�v�ޏo�C�x���g
  case 1:
		GMEVENT_CallEvent( event, EVENT_DISAPPEAR_Warp( NULL, gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // �J�����̕��A
  case 2:
    RecoverCamera( work );
    (*seq)++;
    break; 

  case 3:
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
  FIELD_CAMERA*  camera     = FIELDMAP_GetFieldCamera( work->fieldmap );

  // �����V�[�P���X
  enum {
    SEQ_SETUP_CAMERA,                   // �J�����̃Z�b�g�A�b�v
    SEQ_LOAD_ENTRANCE_CAMERA_SETTINGS,  // �J�������o�f�[�^�擾
    SEQ_CREATE_CAMERA_EFFECT_TASK,      // �J�������o�^�X�N�̍쐬
    SEQ_WAIT_CAMERA_EFFECT_TASK,        // �J�������o�^�X�N�I���҂�
    SEQ_CAMERA_STOP_TRACE_REQUEST,      // �J�����̎��@�Ǐ]OFF���N�G�X�g���s
    SEQ_WAIT_CAMERA_TRACE,              // �J�����̎��@�Ǐ]�����̏I���҂�
    SEQ_CAMERA_TRACE_OFF,               // �J�����̎��@�Ǐ]OFF
    SEQ_DOOR_IN_ANIME,                  // �h�A�i���C�x���g
    SEQ_RECOVER_CAMERA,                 // �J�����̕��A
    SEQ_EXIT,                           // �C�x���g�I��
  };

  switch( *seq ) {
  // �J�����̃Z�b�g�A�b�v
  case SEQ_SETUP_CAMERA:
    SetupCamera( work );
    *seq = SEQ_LOAD_ENTRANCE_CAMERA_SETTINGS;
    break;

  // �J�������o�f�[�^�擾
  case SEQ_LOAD_ENTRANCE_CAMERA_SETTINGS:
    // �f�[�^�擾
    ENTRANCE_CAMERA_SETTINGS_LoadData( &work->cameraSettings, work->exitType );

    // �f�[�^���L�����ǂ���
    if( work->cameraSettings.validFlag_IN ) {
      *seq = SEQ_CREATE_CAMERA_EFFECT_TASK;
    }
    else {
      *seq = SEQ_DOOR_IN_ANIME;
    } 
    break;

  // �J�������o�^�X�N�̍쐬
  case SEQ_CREATE_CAMERA_EFFECT_TASK:
    {
      u16 frame;
      u16 pitch, yaw;
      fx32 length;
      VecFx32 targetOffset;
      // �e�p�����[�^�擾
      {
        frame  = work->cameraSettings.frame;
        pitch  = work->cameraSettings.pitch;
        yaw    = work->cameraSettings.yaw;
        length = work->cameraSettings.length << FX32_SHIFT;
        VEC_Set( &targetOffset, 
                 work->cameraSettings.targetOffsetX << FX32_SHIFT,
                 work->cameraSettings.targetOffsetY << FX32_SHIFT,
                 work->cameraSettings.targetOffsetZ << FX32_SHIFT );
      }
      // �^�X�N�o�^
      {
        FIELD_TASK_MAN* taskMan;
        FIELD_TASK* zoomTaks;
        FIELD_TASK* pitchTask;
        FIELD_TASK* yawTask;
        FIELD_TASK* targetOffsetTask;
        // ����
        zoomTaks         = FIELD_TASK_CameraLinearZoom  ( fieldmap, frame, length );
        pitchTask        = FIELD_TASK_CameraRot_Pitch   ( fieldmap, frame, pitch );
        yawTask          = FIELD_TASK_CameraRot_Yaw     ( fieldmap, frame, yaw );
        targetOffsetTask = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &targetOffset );
        // �o�^
        taskMan = FIELDMAP_GetTaskManager( fieldmap );
        FIELD_TASK_MAN_AddTask( taskMan, zoomTaks, NULL );
        FIELD_TASK_MAN_AddTask( taskMan, pitchTask, NULL );
        FIELD_TASK_MAN_AddTask( taskMan, yawTask, NULL );
        FIELD_TASK_MAN_AddTask( taskMan, targetOffsetTask, NULL );
      }
    }
    *seq = SEQ_WAIT_CAMERA_EFFECT_TASK;
    break;

  // �^�X�N�I���҂�
  case SEQ_WAIT_CAMERA_EFFECT_TASK:
    {
      FIELD_TASK_MAN* taskMan;
      taskMan = FIELDMAP_GetTaskManager( fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ){ *seq = SEQ_CAMERA_STOP_TRACE_REQUEST; }
    }
    break;

  // �J�����̃g���[�X������~���N�G�X�g���s
  case SEQ_CAMERA_STOP_TRACE_REQUEST:
    if( FIELD_CAMERA_CheckTrace( camera ) == TRUE ) {
      FIELD_CAMERA_StopTraceRequest( camera );
    }
    *seq = SEQ_WAIT_CAMERA_TRACE;
    break;

  // �J�����̃g���[�X�����I���҂�
  case SEQ_WAIT_CAMERA_TRACE: 
    if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ){ *seq = SEQ_CAMERA_TRACE_OFF; }
    break;

  // �J�����̃g���[�XOFF
  case SEQ_CAMERA_TRACE_OFF:
    FIELD_CAMERA_FreeTarget( camera );
    *seq = SEQ_DOOR_IN_ANIME;
    break;

  // �h�A�i���A�j��
  case SEQ_DOOR_IN_ANIME:
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorInAnime( 
          gameSystem, fieldmap, &work->nextLocation, FALSE, work->seasonDisplayFlag, work->fadeOutType ) );
    *seq = SEQ_RECOVER_CAMERA;
    break;

  // �J�����̕��A
  case SEQ_RECOVER_CAMERA:
    RecoverCamera( work );
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
 * @brief �J�������Z�b�g�A�b�v����
 *
 * @param work
 */
//---------------------------------------------------------------------------------------
static void SetupCamera( EVENT_WORK* work )
{
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = FIELDMAP_GetFieldCamera( work->fieldmap );

  // ���[���V�X�e���̃J����������~
  if( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    FLDNOGRID_MAPPER* NGMapper = FIELDMAP_GetFldNoGridMapper( fieldmap );
    FLDNOGRID_MAPPER_SetRailCameraActive( NGMapper, FALSE );
  }

  // �J�������[�h���L��
  work->initCameraMode = FIELD_CAMERA_GetMode( camera ); 

  // �J�������[�h��ύX
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
}

//---------------------------------------------------------------------------------------
/**
 * @brief �J���������ɖ߂�
 *
 * @param work
 */
//---------------------------------------------------------------------------------------
static void RecoverCamera( EVENT_WORK* work )
{
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = FIELDMAP_GetFieldCamera( work->fieldmap );

  // �J�������[�h�𕜋A
  FIELD_CAMERA_ChangeMode( camera, work->initCameraMode );

  // ���[���V�X�e���̃J��������𕜋A
  if( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    FLDNOGRID_MAPPER* NGMapper = FIELDMAP_GetFldNoGridMapper( fieldmap );
    FLDNOGRID_MAPPER_SetRailCameraActive( NGMapper, TRUE );
  }
}
