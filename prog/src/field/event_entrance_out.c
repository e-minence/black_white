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
#include "gamesystem/game_event.h"
#include "field/eventdata_type.h"
#include "field/fieldmap_proc.h"
#include "field/location.h"

#include "event_entrance_out.h"

#include "field/eventdata_sxy.h"
#include "field/zonedata.h"            // for ZONEDATA_GetBGMID
#include "event_appear.h"              // for EVENT_APPEAR_xxxx
#include "event_fieldmap_control.h"    // for EVENT_FieldFadeIn
#include "event_entrance_effect.h"     // for EVENT_FieldDoorOutAnime
#include "event_fldmmdl_control.h"     // for EVENT_PlayerOneStepAnime
#include "field_place_name.h"          // for FIELD_PLACE_NAME_Display
#include "fieldmap.h"                  // for FIELDMAP_GetPlaceNameSys
#include "entrance_camera_settings.h"  // for ENTRANCE_CAMERA_SETTINGS

#include "sound/pm_sndsys.h"
#include "sound/bgm_info.h"
#include "field_sound.h" 
#include "../../resource/sound/bgm_info/iss_type.h"

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
// ���C�x���g���[�N
//=======================================================================================
typedef struct {

  GAMESYS_WORK*            gameSystem;
  GAMEDATA*                gameData;
  FIELDMAP_WORK*           fieldmap;
  LOCATION                 location;          // �J�ڐ�w��
  u16                      prevZoneID;        // �J�ڑO�̃]�[��
  EXIT_TYPE                exitType;          // �o������^�C�v
  BOOL                     seasonDisplayFlag; // �G�ߕ\�����s�����ǂ���
  u8                       startSeason;       // �ŏ��ɕ\������G��
  u8                       endSeason;         // �Ō�ɕ\������G��
  ENTRANCE_CAMERA_SETTINGS cameraSettings;    // ����o������̃J�����ݒ�f�[�^
  FIELD_FADE_TYPE          fadeInType;        // �G�ߕ\�����Ȃ��ꍇ��F/I�^�C�v
  FIELD_CAMERA_MODE        initCameraMode;    // �C�x���g�J�n���̃J�������[�h

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
// �J�����̐ݒ�E���A
static void SetupCamera( EVENT_WORK* work ); // �J�������Z�b�g�A�b�v����
static void RecoverCamera( EVENT_WORK* work ); // �J���������ɖ߂�


//=======================================================================================
// �����J�֐��̒�`
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �o�����ޏo�C�x���g���쐬����
 *
 * @param parent            �e�C�x���g
 * @param gameSystem
 * @param gameData
 * @param fieldmap
 * @param location          �J�ڐ�w��
 * @param prevZoneID        �J�ڑO�̃]�[��
 * @param seasonDisplayFlag �G�ߕ\�����s�����ǂ���
 * @param startSeason       �ŏ��ɕ\������G��
 * @param endtSeason        �Ō�ɕ\������G��
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceOut( GMEVENT* parent, 
                            GAMESYS_WORK* gameSystem,
                            GAMEDATA* gameData, 
                            FIELDMAP_WORK* fieldmap, 
                            LOCATION location,
                            u16 prevZoneID,
                            BOOL seasonDisplayFlag,
                            u8 startSeason,
                            u8 endSeason )
{
  GMEVENT* event;
  EVENT_WORK* work; 
  EXIT_TYPE exitType; 
  EVENTFUNC_TYPE funcType;

  // �C�x���g�e�[�u��
  const GMEVENT_FUNC eventFuncTable[ EVENTFUNC_TYPE_NUM ] = 
  {
    EVENT_FUNC_EntranceOut_ExitTypeNone,   // EVENTFUNC_TYPE_NONE �h�A�Ȃ�
    EVENT_FUNC_EntranceOut_ExitTypeDoor,   // EVENTFUNC_TYPE_DOOR �h�A����
    EVENT_FUNC_EntranceOut_ExitTypeStep,   // EVENTFUNC_TYPE_STEP �K�i
    EVENT_FUNC_EntranceOut_ExitTypeWarp,   // EVENTFUNC_TYPE_WARP ���[�v
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EVENTFUNC_TYPE_SPx, ����
  };

  // EXIT_TYPE������
  if( location.type == LOCATION_TYPE_DIRECT ) {
    exitType = EXIT_TYPE_NONE;
  }
  else {
    EVENTDATA_SYSTEM* eventData;
    const CONNECT_DATA* connectData;

    eventData   = GAMEDATA_GetEventData( gameData );
    connectData = EVENTDATA_GetConnectByID( eventData, location.exit_id );
    exitType    = CONNECTDATA_GetExitType( connectData );
  } 

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

  // �C�x���g�쐬
  event = GMEVENT_Create( gameSystem, parent, eventFuncTable[ funcType ], sizeof( EVENT_WORK ) );

  // �C�x���g�E���[�N��������
  work                    = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem        = gameSystem;
  work->gameData          = gameData;
  work->fieldmap          = fieldmap;
  work->prevZoneID        = prevZoneID;
  work->location          = location;
  work->exitType          = exitType;
  work->seasonDisplayFlag = seasonDisplayFlag;
  work->startSeason       = startSeason;
  work->endSeason         = endSeason;
  work->fadeInType        = FIELD_FADE_GetFadeInType( prevZoneID, location.zone_id );

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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch( *seq ) {
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  // BGM�Đ��J�n
  case 1:
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;

  // ��ʃt�F�[�h�J�n
  case 2:
    if( work->seasonDisplayFlag ) { 
      // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
    }
    else {
      // ��{�t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn( gameSystem, fieldmap, work->fadeInType, FIELD_FADE_WAIT, TRUE, 0, 0 ) );
    }
    (*seq)++;
    break;

  // �C�x���g�I��
  case 3:
    RecoverCamera( work ); // �J�����̕��A
    FIELD_PLACE_NAME_Display( 
        FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id ); // �n���\���X�V���N�G�X�g
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch( *seq ) {
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  case 1:
    // �h�A�ޏo�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorOutAnime( gameSystem, fieldmap, TRUE, work->seasonDisplayFlag, 
                                 work->startSeason, work->endSeason, work->fadeInType ) );
    (*seq)++;
    break;

  // �C�x���g�I��
  case 2:
    RecoverCamera( work ); // �J�����̕��A
    FIELD_PLACE_NAME_Display( 
        FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH;
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;
  FIELD_SOUND*   fieldSound = GAMEDATA_GetFieldSound( work->gameData );

  switch( *seq ) {
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  // ��ʃt�F�[�h�C���J�n
  case 1: 
    if( work->seasonDisplayFlag ) { 
      // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
    }
    else { 
      // �N���X�t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn( gameSystem, fieldmap, work->fadeInType, FIELD_FADE_WAIT, TRUE, 0, 0 ) );
    }
    (*seq)++;
    break;

  // BGM�t�F�[�h�C��
  case 2:  
    // ���݂�BGM���_���W����ISS ==> BGM�t�F�[�h�C��
    { 
      BGM_INFO_SYS* bgm_info = GAMEDATA_GetBGMInfoSys( gameData );
      u8 season = GAMEDATA_GetSeasonID( gameData );
      u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
      u32 bgm_now = ZONEDATA_GetBGMID( zone_id, season );
      u8 iss_type_now = BGM_INFO_GetIssType( bgm_info, bgm_now ); 
      if( iss_type_now == ISS_TYPE_DUNGEON )
      {
        FIELD_SOUND* fieldSound = GAMEDATA_GetFieldSound( gameData );
        GMEVENT* fadeInEvent = EVENT_FSND_FadeInBGM( gameSystem, FSND_FADE_FAST );
        GMEVENT_CallEvent( event, fadeInEvent );
      }
      else
      {
        FSND_PlayStartBGM( fieldSound );
      }
    }
    (*seq)++;
    break;

  // ���@�̈���ړ��A�j��
  case 3:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    (*seq)++;
    break;

  // �C�x���g�I��
  case 4:
    RecoverCamera( work ); // �J�����̕��A
    FIELD_PLACE_NAME_Display(
        FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH;
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;

  switch( *seq ) {
  // �J�����̃Z�b�g�A�b�v
  case 0:
    SetupCamera( work );
    (*seq)++;
    break;

  // ���[�v�o���C�x���g
  case 1:
		GMEVENT_CallEvent(event, EVENT_APPEAR_Warp( NULL, gameSystem, fieldmap ) );
    (*seq)++;
    break;

  // �C�x���g�I��
  case 2:
    RecoverCamera( work ); // �J�����̕��A
    FIELD_PLACE_NAME_Display( 
        FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id ); // �n���\���X�V���N�G�X�g
    return GMEVENT_RES_FINISH;
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
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
  FIELD_CAMERA*  camera     = FIELDMAP_GetFieldCamera( work->fieldmap );

  // �����V�[�P���X
  enum {
    SEQ_SETUP_CAMERA,                   // �J�����̃Z�b�g�A�b�v
    SEQ_LOAD_ENTRANCE_CAMERA_SETTINGS,  // �J�������o�f�[�^�擾
    SEQ_DOOR_OUT_ANIME,                 // �h�A�ޏo�C�x���g
    SEQ_INIT_CAMERA_SETTINGS,           // �J�����̏�����Ԃ�ݒ肷��
    SEQ_CREATE_CAMERA_EFFECT_TASK,      // �J�������o�^�X�N�̍쐬
    SEQ_WAIT_CAMERA_EFFECT_TASK,        // �J�������o�^�X�N�I���҂�
    SEQ_DISPLAY_PLACE_NAME,             // �n���\���X�V���N�G�X�g���s
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
    if( work->cameraSettings.validFlag_OUT )
    {
      *seq = SEQ_INIT_CAMERA_SETTINGS;
    }
    else
    {
      *seq = SEQ_DOOR_OUT_ANIME;
    } 
    break;

  // �J�����̏�����Ԃ�ݒ肷��
  case SEQ_INIT_CAMERA_SETTINGS:
    {
      VecFx32 targetOffset;

      VEC_Set( &targetOffset, 
          work->cameraSettings.targetOffsetX << FX32_SHIFT,
          work->cameraSettings.targetOffsetY << FX32_SHIFT,
          work->cameraSettings.targetOffsetZ << FX32_SHIFT );

      FIELD_CAMERA_SetAnglePitch( camera, work->cameraSettings.pitch );
      FIELD_CAMERA_SetAngleYaw( camera, work->cameraSettings.yaw );
      FIELD_CAMERA_SetAngleLen( camera, work->cameraSettings.length << FX32_SHIFT );
      FIELD_CAMERA_SetTargetOffset( camera, &targetOffset );
    }
    *seq = SEQ_DOOR_OUT_ANIME;
    break;

  // �h�A�ޏo�C�x���g
  case SEQ_DOOR_OUT_ANIME:
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorOutAnime( gameSystem, fieldmap, FALSE, 
                                 work->seasonDisplayFlag, work->startSeason, work->endSeason, work->fadeInType ) );

    if( work->cameraSettings.validFlag_OUT )
    {
      *seq = SEQ_CREATE_CAMERA_EFFECT_TASK;
    }
    else
    {
      *seq = SEQ_DISPLAY_PLACE_NAME;
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
        FLD_CAMERA_PARAM defaultParam; 

        // �f�t�H���g�p�����[�^�擾
        FIELD_CAMERA_GetInitialParameter( camera, &defaultParam );

        frame  = work->cameraSettings.frame;
        pitch  = defaultParam.Angle.x;
        yaw    = defaultParam.Angle.y;
        length = defaultParam.Distance << FX32_SHIFT;
        VEC_Set( &targetOffset, defaultParam.Shift.x, defaultParam.Shift.y, defaultParam.Shift.z );
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
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) )
      { 
        *seq = SEQ_DISPLAY_PLACE_NAME;
      }
    }
    break;

  // �n���\���X�V���N�G�X�g���s
  case SEQ_DISPLAY_PLACE_NAME:
    FIELD_PLACE_NAME_Display( FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id );
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
