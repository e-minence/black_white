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

#include "event_fieldmap_control.h" // EVENT_FieldFadeOut
#include "event_entrance_effect.h"  // EVENT_FieldDoorInAnime
#include "event_fldmmdl_control.h"  // EVENT_PlayerOneStepAnime
#include "sound/pm_sndsys.h"
#include "field_sound.h"
#include "sound/bgm_info.h"
#include "../../resource/sound/bgm_info/iss_type.h"


//=======================================================================================
/**
 * @brief �C�x���g�E���[�N
 */
//=======================================================================================
typedef struct
{
  GAMESYS_WORK*    gsys;
  GAMEDATA*        gdata;
  FIELDMAP_WORK* fieldmap;
  LOCATION         location;  // �J�ڐ�w��
}
EVENT_WORK;


//=======================================================================================
/**
 * @breif ����J�֐��̃v���g�^�C�v�錾
 */
//======================================================================================= 

// �eEXIT_TYPE���Ƃ̃C�x���g
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP1(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP2(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP3(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP4(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP5(GMEVENT * event, int *seq, void * work);


//=======================================================================================
/**
 * @breif ���J�֐��̒�`
 */
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �o�����i���C�x���g���쐬����
 *
 * @param parent    �e�C�x���g
 * @param gsys      �Q�[���V�X�e��
 * @param gdata     �Q�[���f�[�^
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param location  �J�ڐ�w��
 * @param exit_type �J�ڃ^�C�v�w��
 *
 * @return �쐬�����C�x���g
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

  // �C�x���g�e�[�u��
  const GMEVENT_FUNC eventFuncTable[] = 
  {
    EVENT_FUNC_EntranceIn_ExitTypeNone,   //EXIT_TYPE_NONE
    EVENT_FUNC_EntranceIn_ExitTypeNone,   //EXIT_TYPE_MAT
    EVENT_FUNC_EntranceIn_ExitTypeStep,   //EXIT_TYPE_STAIRS
    EVENT_FUNC_EntranceIn_ExitTypeDoor,   //EXIT_TYPE_DOOR
    EVENT_FUNC_EntranceIn_ExitTypeStep,   //EXIT_TYPE_WALL
    EVENT_FUNC_EntranceIn_ExitTypeWarp,   //EXIT_TYPE_WARP
    EVENT_FUNC_EntranceIn_ExitTypeSP1,    //EXIT_TYPE_SP1
    EVENT_FUNC_EntranceIn_ExitTypeSP2,    //EXIT_TYPE_SP2
    EVENT_FUNC_EntranceIn_ExitTypeSP3,    //EXIT_TYPE_SP3
    EVENT_FUNC_EntranceIn_ExitTypeSP4,    //EXIT_TYPE_SP4
    EVENT_FUNC_EntranceIn_ExitTypeSP5,    //EXIT_TYPE_SP5
  };

  // �C�x���g�쐬
  event = GMEVENT_Create( gsys, parent, eventFuncTable[ exit_type ], sizeof( EVENT_WORK ) );

  // �C�x���g�E���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->gdata    = gdata;
  work->fieldmap = fieldmap;
  work->location = location;

  // �쐬�����C�x���g��Ԃ�
  return event;
}


//=======================================================================================
/**
 * @breif ����J�֐��̒�`
 */
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A�Ȃ����̐i���C�x���g
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
    { // BGM�X�V���N�G�X�g
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gamedata );
      PLAYER_WORK* player = GAMEDATA_GetPlayerWork( gamedata, 0 );
      PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
      FIELD_SOUND_ChangePlayZoneBGM( fsnd, gamedata, form, event_work->location.zone_id );
    }
		GMEVENT_CallEvent( event, EVENT_FieldFadeOut(gsys, fieldmap, 0, FIELD_FADE_WAIT) );
    ++ *seq;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A���莞�̐i���C�x���g
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
 * @breif �K�i�̐i���C�x���g
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
  case 1: // BGM�t�F�[�h�A�E�g
    { // ���݂�BGM���_���W����ISS && ����BGM���_���W����ISS ==> BGM�t�F�[�h�A�E�g
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gamedata );
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
        FIELD_SOUND_FadeOutPushBGM( fsnd, 20 );
      }
    }
    ++ *seq;
    break;
  case 2: // BGM�t�F�[�h�A�E�g�҂�
    if( PMSND_CheckFadeOnBGM() != TRUE )
    { 
      PMSND_PlaySE( SEQ_SE_KAIDAN );    // SE
      GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, fieldmap, 0, FIELD_FADE_WAIT));  // ��ʃt�F�[�h�A�E�g
      ++ *seq;
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif ���[�v���̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeWarp(GMEVENT * event, int *seq, void * work)
{
	EVENT_WORK*      event_work = work;
	GAMESYS_WORK*    gsys       = event_work->gsys;
	FIELDMAP_WORK* fieldmap   = event_work->fieldmap;
	GAMEDATA*        gamedata   = event_work->gdata;

  switch ( *seq )
  {
  case 0:
    { // BGM�X�V���N�G�X�g
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gamedata );
      PLAYER_WORK* player = GAMEDATA_GetPlayerWork( gamedata, 0 );
      PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
      FIELD_SOUND_ChangePlayZoneBGM( fsnd, gamedata, form, event_work->location.zone_id );
    }
		GMEVENT_CallEvent( event, EVENT_FieldFadeOut(gsys, fieldmap, 0, FIELD_FADE_WAIT) );
    ++ *seq;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @breif �i���C�x���g( SP1 )
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP1(GMEVENT * event, int *seq, void * work)
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
 * @breif �i���C�x���g( SP2 )
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP2(GMEVENT * event, int *seq, void * work)
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
 * @breif �i���C�x���g( SP3 )
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP3(GMEVENT * event, int *seq, void * work)
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
 * @breif �i���C�x���g( SP4 )
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP4(GMEVENT * event, int *seq, void * work)
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
 * @breif �i���C�x���g( SP5 )
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeSP5(GMEVENT * event, int *seq, void * work)
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
