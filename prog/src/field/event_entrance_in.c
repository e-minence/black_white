/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @breif  �����ւ̐i�����̃C�x���g�쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////

#include "gamesystem/game_event.h"
#include "field/eventdata_type.h"
#include "field/fieldmap_proc.h"
#include "field/field_sound.h"      // FIELD_SOUND_GetFieldBGMNo
#include "field/location.h"

#include "event_entrance_in.h"

#include "event_fieldmap_control.h" // EVENT_FieldFadeOut
#include "field_door_anime.h"       // EVENT_FieldDoorInAnime
#include "event_fldmmdl_control.h"  // EVENT_PlayerOneStepAnime


//=======================================================================================
/**
 * @brief �C�x���g�E���[�N
 */
//=======================================================================================
typedef struct
{
  GAMESYS_WORK*    gsys;
  GAMEDATA*        gdata;
  FIELD_MAIN_WORK* fieldmap;
  LOCATION         location;  // �J�ڐ�w��
}
EVENT_WORK;


//=======================================================================================
/**
 * @breif ����J�֐��̃v���g�^�C�v�錾
 */
//======================================================================================= 
static void setNextBGM(GAMEDATA * gamedata, u16 zone_id);

// �eEXIT_TYPE���Ƃ̃C�x���g
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor(GMEVENT * event, int *seq, void * work);
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeStep(GMEVENT * event, int *seq, void * work);


//=======================================================================================
/**
 * @breif ���J�֐��̒�`
 */
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �����i���C�x���g���쐬����
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
                           FIELD_MAIN_WORK* fieldmap, 
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
 * @brief BGM�؂�ւ�
 */
//---------------------------------------------------------------------------------------
static void setNextBGM(GAMEDATA * gamedata, u16 zone_id)
{
  //���}���B��Ƀt�F�[�h�C���Ŏn�܂�
#if 0
  u16 trackBit = 0xfcff;	// track 9,10 OFF
  u16 nextBGM = ZONEDATA_GetBGMID(zone_id, GAMEDATA_GetSeasonID(gamedata));
//  PMSND_PlayNextBGM_EX(nextBGM, trackBit, 30, 0);
  PMSND_PlayBGM_EX(nextBGM, 0xfcff );
#else
  PLAYER_WORK *player = GAMEDATA_GetPlayerWork( gamedata, 0 );
  PLAYER_MOVE_FORM form = PLAYERWORK_GetMoveForm( player );
  u32 no = FIELD_SOUND_GetFieldBGMNo( gamedata, form, zone_id );
  OS_Printf("NEXT BGM NO=%d\n",no);
  //FIELD_SOUND_PlayBGM( no );		
  FIELD_SOUND_PlayNextBGM( no );
#endif
}

//---------------------------------------------------------------------------------------
/**
 * @breif �h�A�Ȃ����̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeNone(GMEVENT * event, int *seq, void * work)
{
	EVENT_WORK*      event_work = work;
	GAMESYS_WORK*    gsys       = event_work->gsys;
	FIELD_MAIN_WORK* fieldmap   = event_work->fieldmap;
	GAMEDATA*        gamedata   = event_work->gdata;

  switch ( *seq )
  {
  case 0:
    setNextBGM( gamedata, event_work->location.zone_id );
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
 * @breif �h�A���莞�̐i���C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceIn_ExitTypeDoor(GMEVENT * event, int *seq, void * work)
{
	EVENT_WORK*      event_work = work;
	GAMESYS_WORK*    gsys       = event_work->gsys;
	FIELD_MAIN_WORK* fieldmap   = event_work->fieldmap;

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
	FIELD_MAIN_WORK* fieldmap   = event_work->fieldmap;
	GAMEDATA*        gamedata   = event_work->gdata;

  switch ( *seq )
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gsys, fieldmap) );
    ++ *seq;
    break;
  case 1:
    setNextBGM( gamedata, event_work->location.zone_id );
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, fieldmap, 0));
    ++ *seq;
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
