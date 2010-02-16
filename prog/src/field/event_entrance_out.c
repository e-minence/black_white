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
// ���C�x���g���[�N
//=======================================================================================
typedef struct
{
  GAMESYS_WORK*            gameSystem;
  GAMEDATA*                gameData;
  FIELDMAP_WORK*           fieldmap;
  LOCATION                 location;          // �J�ڐ�w��
  EXIT_TYPE                exitType;          // �o������^�C�v
  BOOL                     seasonDisplayFlag; // �G�ߕ\�����s�����ǂ���
  u8                       startSeason;       // �ŏ��ɕ\������G��
  u8                       endSeason;         // �Ō�ɕ\������G��
  ENTRANCE_CAMERA_SETTINGS cameraSettings;    // ����o������̃J�����ݒ�f�[�^
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
                            BOOL seasonDisplayFlag,
                            u8 startSeason,
                            u8 endSeason )
{
  GMEVENT* event;
  EVENT_WORK* work; 
  EXIT_TYPE exitType; 

  // �C�x���g�e�[�u��
  const GMEVENT_FUNC eventFuncTable[] = {
    EVENT_FUNC_EntranceOut_ExitTypeNone,   // EXIT_TYPE_NONE
    EVENT_FUNC_EntranceOut_ExitTypeNone,   // EXIT_TYPE_MAT
    EVENT_FUNC_EntranceOut_ExitTypeStep,   // EXIT_TYPE_STAIRS
    EVENT_FUNC_EntranceOut_ExitTypeDoor,   // EXIT_TYPE_DOOR
    EVENT_FUNC_EntranceOut_ExitTypeStep,   // EXIT_TYPE_WALL
    EVENT_FUNC_EntranceOut_ExitTypeWarp,   // EXIT_TYPE_WARP
    EVENT_FUNC_EntranceOut_ExitTypeNone,   // EXIT_TYPE_INTRUDE
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP1
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP2
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP3
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP4
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP5
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP6
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP7
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP8
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP9
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP10
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP11
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP12
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP13
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP14
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP15
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP16
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP17
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP18
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP19
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP20
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP21
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP22
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP23
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP24
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP25
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP26
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP27
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP28
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP29
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    // EXIT_TYPE_SP30
  };

  // EXIT_TYPE������
  if( location.type == LOCATION_TYPE_DIRECT )
  {
    exitType = EXIT_TYPE_NONE;
  }
  else
  {
    EVENTDATA_SYSTEM* eventData;
    const CONNECT_DATA* connectData;

    eventData   = GAMEDATA_GetEventData( gameData );
    connectData = EVENTDATA_GetConnectByID( eventData, location.exit_id );
    exitType    = CONNECTDATA_GetExitType( connectData );
  } 

  // �C�x���g�쐬
  event = GMEVENT_Create( gameSystem, parent, eventFuncTable[ exitType ], sizeof( EVENT_WORK ) );

  // �C�x���g�E���[�N��������
  work                     = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem         = gameSystem;
  work->gameData           = gameData;
  work->fieldmap           = fieldmap;
  work->location           = location;
  work->exitType           = exitType;
  work->seasonDisplayFlag  = seasonDisplayFlag;
  work->startSeason        = startSeason;
  work->endSeason          = endSeason;

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

  switch( *seq )
  {
  case 0:
    // BGM�Đ��J�n
    FSND_PlayStartBGM( fieldSound );
    (*seq)++;
    break;
  case 1:
    if( work->seasonDisplayFlag )
    { // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
    }
    else
    { // �N���X�t�F�[�h
      GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Cross( gameSystem, fieldmap ) );
    }
    (*seq)++;
    break;
  case 2:
    // �n���\���X�V���N�G�X�g
    FIELD_PLACE_NAME_Display( FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id );
    (*seq)++;
    break;
  case 3:
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

  switch( *seq )
  {
  case 0:
    // �h�A�ޏo�C�x���g
    GMEVENT_CallEvent( event, 
        EVENT_FieldDoorOutAnime( gameSystem, fieldmap, TRUE, 
                                 work->seasonDisplayFlag, work->startSeason, work->endSeason ) );
    (*seq)++;
    break;
  case 1:
    // �n���\���X�V���N�G�X�g
    FIELD_PLACE_NAME_Display( FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id );
    (*seq)++;
    break;
  case 2:
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

  switch( *seq )
  {
  case 0: 
    // ��ʃt�F�[�h�C��
    if( work->seasonDisplayFlag )
    { // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
    }
    else
    { // �N���X�t�F�[�h
      GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Cross( gameSystem, fieldmap ) );
    }
    (*seq)++;
    break;
  case 1:  // BGM�t�F�[�h�C��
    { // ���݂�BGM���_���W����ISS ==> BGM�t�F�[�h�C��
      BGM_INFO_SYS* bgm_info = GAMEDATA_GetBGMInfoSys( gameData );
      u8 season = GAMEDATA_GetSeasonID( gameData );
      u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
      u32 bgm_now = ZONEDATA_GetBGMID( zone_id, season );
      u8 iss_type_now = BGM_INFO_GetIssType( bgm_info, bgm_now ); 
      if( iss_type_now == ISS_TYPE_DUNGEON )
      {
        FIELD_SOUND* fieldSound = GAMEDATA_GetFieldSound( gameData );
        GMEVENT* fadeInEvent = EVENT_FSND_FadeInBGM( gameSystem, FSND_FADE_SHORT );
        GMEVENT_CallEvent( event, fadeInEvent );
      }
      else
      {
        FSND_PlayStartBGM( fieldSound );
      }
    }
    (*seq)++;
    break;
  case 2:
    FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id);
    (*seq)++;
    break;
  case 3:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    (*seq)++;
    break;
  case 4:
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

  switch( *seq )
  {
  case 0:
    // ���[�v�o���C�x���g
		GMEVENT_CallEvent(event, EVENT_APPEAR_Warp( NULL, gameSystem, fieldmap ) );
    (*seq)++;
    break;
  case 1:
    // �n���\���X�V���N�G�X�g
    FIELD_PLACE_NAME_Display( FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id );
    (*seq)++;
    break;
  case 2:
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
    SEQ_LOAD_ENTRANCE_CAMERA_SETTINGS,  // �J�������o�f�[�^�擾
    SEQ_DOOR_OUT_ANIME,                 // �h�A�ޏo�C�x���g
    SEQ_INIT_CAMERA_SETTINGS,           // �J�����̏�����Ԃ�ݒ肷��
    SEQ_CREATE_CAMERA_EFFECT_TASK,      // �J�������o�^�X�N�̍쐬
    SEQ_WAIT_CAMERA_EFFECT_TASK,        // �J�������o�^�X�N�I���҂�
    SEQ_DISPLAY_PLACE_NAME,             // �n���\���X�V���N�G�X�g���s
    SEQ_EXIT,                           // �C�x���g�I��
  };

  switch( *seq )
  {
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
                                 work->seasonDisplayFlag, work->startSeason, work->endSeason ) );

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
    *seq = SEQ_EXIT;
    break;

  // �C�x���g�I��
  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
