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
#include "event_fieldmap_control.h" // EVENT_FieldFadeIn
#include "event_entrance_effect.h"  // EVENT_FieldDoorOutAnime
#include "event_fldmmdl_control.h"  // EVENT_PlayerOneStepAnime
#include "field_place_name.h" // FIELD_PLACE_NAME_Display
#include "fieldmap.h"       // FIELDMAP_GetPlaceNameSys
#include "field_sound.h"
#include "sound/pm_sndsys.h"
#include "sound/bgm_info.h"
#include "../../resource/sound/bgm_info/iss_type.h"
#include "field/zonedata.h"  // for ZONEDATA_GetBGMID
#include "event_appear.h"  // for EVENT_APPEAR_xxxx



//=======================================================================================
/**
 * @brief �C�x���g�E���[�N
 */
//=======================================================================================
typedef struct
{
  GAMESYS_WORK*  gameSystem;
  GAMEDATA*      gameData;
  FIELDMAP_WORK* fieldmap;
  LOCATION       location;  // �J�ڐ�w��

} EVENT_WORK;


//=======================================================================================
/**
 * @breif ����J�֐��̃v���g�^�C�v�錾
 */
//======================================================================================= 
// �eEXIT_TYPE���Ƃ̃C�x���g
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeNone(GMEVENT* event, int* seq, void* wk);
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeDoor(GMEVENT* event, int* seq, void* wk);
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeStep(GMEVENT* event, int* seq, void* wk);
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeWarp(GMEVENT* event, int* seq, void* wk);
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeSPx(GMEVENT* event, int* seq, void* wk);


//=======================================================================================
/**
 * @breif ���J�֐��̒�`
 */
//=======================================================================================

//---------------------------------------------------------------------------------------
/**
 * @brief �o�����ޏo�C�x���g���쐬����
 *
 * @param parent     �e�C�x���g
 * @param gameSystem
 * @param gameData
 * @param fieldmap
 * @param location   �J�ڐ�w��
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceOut( GMEVENT* parent, 
                            GAMESYS_WORK* gameSystem,
                            GAMEDATA* gameData, 
                            FIELDMAP_WORK* fieldmap, 
                            LOCATION location )
{
  GMEVENT* event;
  EVENT_WORK* work; 
  EXIT_TYPE exitType; 

  // �C�x���g�e�[�u��
  const GMEVENT_FUNC eventFuncTable[] = {
    EVENT_FUNC_EntranceOut_ExitTypeNone,   //EXIT_TYPE_NONE
    EVENT_FUNC_EntranceOut_ExitTypeNone,   //EXIT_TYPE_MAT
    EVENT_FUNC_EntranceOut_ExitTypeStep,   //EXIT_TYPE_STAIRS
    EVENT_FUNC_EntranceOut_ExitTypeDoor,   //EXIT_TYPE_DOOR
    EVENT_FUNC_EntranceOut_ExitTypeStep,   //EXIT_TYPE_WALL
    EVENT_FUNC_EntranceOut_ExitTypeWarp,   //EXIT_TYPE_WARP
    EVENT_FUNC_EntranceOut_ExitTypeNone,   //EXIT_TYPE_INTRUDE
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP1
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP2
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP3
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP4
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP5
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP6
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP7
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP8
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP9
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP10
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP11
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP12
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP13
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP14
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP15
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP16
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP17
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP18
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP19
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP20
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP21
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP22
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP23
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP24
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP25
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP26
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP27
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP28
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP29
    EVENT_FUNC_EntranceOut_ExitTypeSPx,    //EXIT_TYPE_SP30
  };

  // EXIT_TYPE������
  if( location.type == LOCATION_TYPE_DIRECT )
  {
    exitType = EXIT_TYPE_NONE;
  }
  else
  {
    EVENTDATA_SYSTEM* evdata;
    const CONNECT_DATA* cnct;
    evdata = GAMEDATA_GetEventData( gameData );
    cnct   = EVENTDATA_GetConnectByID( evdata, location.exit_id );
    exitType = CONNECTDATA_GetExitType( cnct );
  } 

  // �C�x���g�쐬
  event = GMEVENT_Create( gameSystem, parent, eventFuncTable[ exitType ], sizeof( EVENT_WORK ) );

  // �C�x���g�E���[�N��������
  work             = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->gameData   = gameData;
  work->fieldmap   = fieldmap;
  work->location   = location;

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
 * @breif �h�A�Ȃ����̑ޏo�C�x���g
 */
//---------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeNone(GMEVENT * event, int *seq, void * wk)
{
	EVENT_WORK*    work       = wk;
	GAMESYS_WORK*  gameSystem = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*      gameData   = work->gameData;

  switch( *seq )
  {
  case 0:
    { // BGM�Đ��J�n
        FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gameData );
        FSND_PlayStartBGM( fsnd );
    }
    ++ *seq;
    break;
  case 1:
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      FIELD_STATUS* fstatus; 
      fstatus = GAMEDATA_GetFieldStatus( gameData );
      if( FIELD_STATUS_GetSeasonDispFlag(fstatus) )  // if(�G�ߕ\������)
      { // �G�߃t�F�[�h
        fade_event = EVENT_FieldFadeIn_Season(gameSystem, fieldmap);
      }
      else
      { // �N���X�t�F�[�h
        fade_event = EVENT_FieldFadeIn_Cross(gameSystem, fieldmap);
      }
      GMEVENT_CallEvent( event, fade_event );
    }
    ++ *seq;
    break;
  case 2:
    FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id);
    ++ *seq;
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
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeDoor(GMEVENT * event, int *seq, void * wk)
{
	EVENT_WORK*      work = wk;
	GAMESYS_WORK*    gameSystem       = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_FieldDoorOutAnime( gameSystem, fieldmap, TRUE ) );
    ++ *seq;
    break;
  case 1:
    FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id);
    ++ *seq;
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
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeStep(GMEVENT * event, int *seq, void * wk)
{
	EVENT_WORK*      work = wk;
	GAMESYS_WORK*    gameSystem       = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*        gameData   = work->gameData;

  switch (*seq)
  {
  case 0: // ��ʃt�F�[�h�C��
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      FIELD_STATUS* fstatus; 
      fstatus = GAMEDATA_GetFieldStatus( gameData );
      if( FIELD_STATUS_GetSeasonDispFlag(fstatus) )  // if(�G�ߕ\������)
      { // �G�߃t�F�[�h
        fade_event = EVENT_FieldFadeIn_Season(gameSystem, fieldmap);
      }
      else
      { // �N���X�t�F�[�h
        fade_event = EVENT_FieldFadeIn_Cross(gameSystem, fieldmap);
      }
      GMEVENT_CallEvent( event, fade_event );
    }
    ++ *seq;
    break;
  case 1:  // BGM�t�F�[�h�C��
    { // ���݂�BGM���_���W����ISS ==> BGM�t�F�[�h�C��
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gameData );
      BGM_INFO_SYS* bgm_info = GAMEDATA_GetBGMInfoSys( gameData );
      u8 season = GAMEDATA_GetSeasonID( gameData );
      u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
      u32 bgm_now = ZONEDATA_GetBGMID( zone_id, season );
      u8 iss_type_now = BGM_INFO_GetIssType( bgm_info, bgm_now ); 
      if( iss_type_now == ISS_TYPE_DUNGEON )
      {
        FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gameData );
        GMEVENT* fadeInEvent = EVENT_FSND_FadeInBGM( gameSystem, FSND_FADE_SHORT );
        GMEVENT_CallEvent( event, fadeInEvent );
      }
      else
      {
        FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( gameData );
        FSND_PlayStartBGM( fsnd );
      }
    }
    ++ *seq;
    break;
  case 2:
    FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id);
    ++ *seq;
    break;
  case 3:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    ++ *seq;
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
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeWarp(GMEVENT * event, int *seq, void * wk)
{
	EVENT_WORK*      work = wk;
	GAMESYS_WORK*    gameSystem       = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;
	GAMEDATA*        gameData   = work->gameData;

  switch (*seq)
  {
  case 0:
		GMEVENT_CallEvent(event, EVENT_APPEAR_Warp(NULL, gameSystem, fieldmap) );
    ++ *seq;
    break;
  case 1:
    FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id);
    ++ *seq;
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
static GMEVENT_RESULT EVENT_FUNC_EntranceOut_ExitTypeSPx(GMEVENT * event, int *seq, void * wk)
{
	EVENT_WORK*      work = wk;
	GAMESYS_WORK*    gameSystem       = work->gameSystem;
	FIELDMAP_WORK* fieldmap   = work->fieldmap;

  switch (*seq)
  {
  case 0:
    GMEVENT_CallEvent( event, EVENT_FieldDoorOutAnime( gameSystem, fieldmap, FALSE ) );
    ++ *seq;
    break;
  case 1:
    FIELD_PLACE_NAME_Display(FIELDMAP_GetPlaceNameSys(fieldmap), work->location.zone_id);
    ++ *seq;
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
} 
