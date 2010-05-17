//============================================================================================
/**
 * @file event_entrance_effect.c
 * @brief
 * @date  2009.06.13
 * @author  tamada GAME FREAK inc.
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h" 
#include "sound/pm_sndsys.h"      // for PMSND_PlaySE
#include "field/field_sound.h"    // for FIELD_SOUND_ChangePlayZoneBGM
#include "field/field_const.h"
#include "field/eventdata_type.h"

#include "fieldmap.h"
#include "field_player.h"
#include "field_buildmodel.h"

#include "entrance_event_common.h"
#include "entrance_camera.h"
#include "event_entrance_effect.h" 
#include "event_mapchange.h"        // for MAPCHANGE_setPlayerVanish
#include "event_fieldmap_control.h" // for EVENT_FieldFadeOut
#include "event_fldmmdl_control.h"  // for EVENT_PlayerOneStepAnime



//============================================================================================
// ���萔
//============================================================================================
#define MAX_SEQ_LENGTH       (10) // ��A�̃V�[�P���X�̍ő吔
#define ZERO_CAMERA_WAIT_IN  (15) // �J�����̃[���t���[���؂�ւ����o�̑҂����� ( ���鎞 )
#define ZERO_CAMERA_WAIT_OUT (15) // �J�����̃[���t���[���؂�ւ����o�̑҂����� ( �o�鎞 )

// �h�A����o�Ă���C�x���g�̃V�[�P���X�ԍ�
enum {
  SEQ_DOOROUT_INIT = 0,
  SEQ_DOOROUT_EFFECT_START,
  SEQ_DOOROUT_OPEN_ANIME_START,
  SEQ_DOOROUT_OPEN_ANIME_WAIT,
  SEQ_DOOROUT_PLAYER_STEP,
  SEQ_DOOROUT_CAMERA_ACT_START,
  SEQ_DOOROUT_CLOSE_ANIME_START,
  SEQ_DOOROUT_CLOSE_ANIME_WAIT,
  SEQ_DOOROUT_END,
  // �ȉ�, ���s���̔���őJ�ڂ���V�[�P���X
  SEQ_DOOROUT_CAMERA_TRACE_WAIT,
  SEQ_DOOROUT_WAIT_FOR_CAMERA_ACT_START,
};

// �h�A�ɓ���C�x���g�̃V�[�P���X�ԍ�
enum {
  SEQ_DOORIN_INIT = 0,
  SEQ_DOORIN_CAMERA_ACT_START,
  SEQ_DOORIN_CAMERA_ACT_WAIT,
  SEQ_DOORIN_OPEN_ANIME_START,
  SEQ_DOORIN_OPEN_ANIME_WAIT,
  SEQ_DOORIN_PLAYER_STEP,
  SEQ_DOORIN_BGM_STAND_BY,
  SEQ_DOORIN_FADEOUT,
  SEQ_DOORIN_END,
  // �ȉ�, ���s���̔���őJ�ڂ���V�[�P���X
  SEQ_DOORIN_WAIT_FOR_PLAYER_ONESTEP,
};


//============================================================================================
// ���C�x���g���[�N
//============================================================================================
typedef struct {

  ENTRANCE_EVDATA* evdata;          //<<<�o������C�x���g�̋��ʃ��[�N
  FIELD_SOUND *    fieldSound;      //<<<�t�B�[���h�T�E���h�ւ̃|�C���^
  ECAM_WORK*       ECamWork;        //<<<�J�������o���[�N
  ECAM_PARAM       ECamParam;       //<<<�J�������o�p�����[�^
  u16              count;           //<<<�J�E���^
  u8               playerDir;       //<<<���@�̌���
  VecFx32          doorSearchPos;   //<<<�h�A�����ʒu
  FIELD_BMODEL *   doorBM;          //<<<�h�A�̔z�u���f��
  u8               seq[ MAX_SEQ_LENGTH ]; //<<<�V�[�P���X
  u8               seqPos;                //<<<���݂̃V�[�P���X�ʒu

} FIELD_DOOR_ANIME_WORK;


//============================================================================================
// ��index
//============================================================================================
static void SetupDoorOutEventSeq( FIELD_DOOR_ANIME_WORK* work ); // �h�A����o�Ă���C�x���g�̃V�[�P���X�̗�������肷��
static GMEVENT_RESULT ExitEvent_DoorOut( GMEVENT * event, int *seq, void * wk ); // �h�A����o�Ă���C�x���g�����֐�

static void SetupDoorInEventSeq( FIELD_DOOR_ANIME_WORK* work ); // �h�A�ɓ���C�x���g�̃V�[�P���X�̗�������肷��
static GMEVENT_RESULT ExitEvent_DoorIn(GMEVENT * event, int *seq, void * wk); // �h�A�ɓ���C�x���g�����֐�

static u8 GetPlayerDir( FIELDMAP_WORK* fieldmap ); // ���@�̌������擾����
static void GetPlayerPos( FIELDMAP_WORK* fieldmap, VecFx32* dest ); // ���@�̍��W���擾����
static void GetPlayerFrontPos( FIELDMAP_WORK* fieldmap, VecFx32* dest ); // �����̍��W���擾����
static void GetPlayerFrontPos_GRID( FIELDMAP_WORK* fieldmap, VecFx32* dest ); // �����̍��W���擾���� ( �O���b�h�}�b�v�p )
static void GetPlayerFrontPos_RAIL( FIELDMAP_WORK* fieldmap, VecFx32* dest ); // �����̍��W���擾���� ( ���[���}�b�v�p )
static u8 GetCurrentSeq( const FIELD_DOOR_ANIME_WORK* work ); // ���݂̃V�[�P���X���擾����
static u8 GetNextSeq( const FIELD_DOOR_ANIME_WORK* work ); // ���̃V�[�P���X���擾����
static void FinishCurrentSeq( FIELD_DOOR_ANIME_WORK* work ); // ���݂̃V�[�P���X���I������
static void SearchDoorBM( FIELD_DOOR_ANIME_WORK* work ); // �h�A��T���A�z�u���f���𐶐�����
static void DeleteDoorBM( FIELD_DOOR_ANIME_WORK* work ); // ���������h�A�̔z�u���f����j������
static BOOL CheckDoorAnimeFinish( const FIELD_DOOR_ANIME_WORK* work ); // �h�A�A�j���̊������`�F�b�N����
static BOOL CheckDoorExist( const FIELD_DOOR_ANIME_WORK* work ); // �h�A�����݂��邩�ǂ������`�F�b�N����
static void StartDoorOpen( const FIELD_DOOR_ANIME_WORK* work ); // �h�A�I�[�v�����J�n����
static void StartDoorClose( const FIELD_DOOR_ANIME_WORK* work ); // �h�A�N���[�Y���J�n����
static void StartFadeInEvent( GMEVENT* event, const FIELD_DOOR_ANIME_WORK* work ); // ��ʂ̃t�F�[�h�C���C�x���g���Ăяo��
static void StartFadeOutEvent( GMEVENT* event, const FIELD_DOOR_ANIME_WORK* work ); // ��ʂ̃t�F�[�h�A�E�g�C�x���g���Ăяo��
static BOOL CheckMapChangeSE( const FIELD_DOOR_ANIME_WORK* work ); // �}�b�v�`�F���WSE�̍Đ����`�F�b�N����


//--------------------------------------------------------------------------------------------
/**
 * @brief ���@�̈����̍��W���擾����
 *
 * @param fieldmap
 * @param dest �擾�������W�̊i�[��
 */
//--------------------------------------------------------------------------------------------
void EVENT_ENTRANCE_EFFECT_GetPlayerFrontPos( FIELDMAP_WORK* fieldmap, VecFx32* dest ) 
{
  GetPlayerFrontPos( fieldmap, dest );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A����łĂ����A�̉��o�C�x���g�𐶐�����
 *
 * @param evdata �o������C�x���g�̋��ʃ��[�N
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldDoorOutAnime( ENTRANCE_EVDATA* evdata )
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * work;

  // �C�x���g�𐶐�
  event = GMEVENT_Create( 
      evdata->gameSystem, NULL, ExitEvent_DoorOut, sizeof(FIELD_DOOR_ANIME_WORK) ); 

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->evdata     = evdata;
  work->fieldSound = GAMEDATA_GetFieldSound( evdata->gameData );
  work->doorBM     = NULL;
  work->count      = 0;
  work->playerDir  = GetPlayerDir( evdata->fieldmap );
  work->seqPos     = 0;

  // �J�������o�̃Z�b�g�A�b�v�p�����[�^���쐬
  work->ECamWork = ENTRANCE_CAMERA_CreateWork( evdata->fieldmap );
  work->ECamParam.exitType  = evdata->exit_type_out;
  work->ECamParam.situation = ECAM_SITUATION_OUT;
  work->ECamParam.oneStep   = ECAM_ONESTEP_ON;

  // �h�A������
  GetPlayerPos( evdata->fieldmap, &work->doorSearchPos );
  SearchDoorBM( work );

  // �V�[�P���X�̗��������
  SetupDoorOutEventSeq( work ); 
  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă���C�x���g�̃V�[�P���X�̗�������肷��
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void SetupDoorOutEventSeq( FIELD_DOOR_ANIME_WORK* work )
{
  ENTRANCE_EVDATA* evdata   = work->evdata;
  FIELDMAP_WORK*   fieldmap = evdata->fieldmap;
  int idx = 0;

  // �h�A������ꍇ
  if( CheckDoorExist(work) ) {
    switch( GetPlayerDir(fieldmap) ) {
    default: GF_ASSERT(0);
    case DIR_UP: // �����
    case DIR_DOWN: // ������
      work->seq[idx++] = SEQ_DOOROUT_INIT;
      work->seq[idx++] = SEQ_DOOROUT_EFFECT_START;
      work->seq[idx++] = SEQ_DOOROUT_CAMERA_ACT_START;
      work->seq[idx++] = SEQ_DOOROUT_OPEN_ANIME_START;
      work->seq[idx++] = SEQ_DOOROUT_OPEN_ANIME_WAIT;
      work->seq[idx++] = SEQ_DOOROUT_PLAYER_STEP;
      work->seq[idx++] = SEQ_DOOROUT_CLOSE_ANIME_START;
      work->seq[idx++] = SEQ_DOOROUT_CLOSE_ANIME_WAIT;
      work->seq[idx++] = SEQ_DOOROUT_END; 
      break;
    case DIR_LEFT: // ������
    case DIR_RIGHT: // �E����
      work->seq[idx++] = SEQ_DOOROUT_INIT;
      work->seq[idx++] = SEQ_DOOROUT_EFFECT_START;
      work->seq[idx++] = SEQ_DOOROUT_OPEN_ANIME_START;
      work->seq[idx++] = SEQ_DOOROUT_OPEN_ANIME_WAIT;
      work->seq[idx++] = SEQ_DOOROUT_PLAYER_STEP;
      work->seq[idx++] = SEQ_DOOROUT_CAMERA_ACT_START;
      work->seq[idx++] = SEQ_DOOROUT_CLOSE_ANIME_START;
      work->seq[idx++] = SEQ_DOOROUT_CLOSE_ANIME_WAIT;
      work->seq[idx++] = SEQ_DOOROUT_END; 
      break;
    }
  }
  // �h�A���Ȃ��ꍇ
  else {
    // ����o����
    if( EXIT_TYPE_IsSpExit(evdata->exit_type_out) ) {
      work->seq[idx++] = SEQ_DOOROUT_INIT;
      work->seq[idx++] = SEQ_DOOROUT_EFFECT_START;
      work->seq[idx++] = SEQ_DOOROUT_PLAYER_STEP;
      work->seq[idx++] = SEQ_DOOROUT_CAMERA_ACT_START;
      work->seq[idx++] = SEQ_DOOROUT_END; 
    }
    // �ʏ�o����
    else {
      switch( GetPlayerDir(fieldmap) ) {
      default: GF_ASSERT(0);
      case DIR_UP: // �����
      case DIR_DOWN: // ������
        work->seq[idx++] = SEQ_DOOROUT_INIT;
        work->seq[idx++] = SEQ_DOOROUT_EFFECT_START;
        work->seq[idx++] = SEQ_DOOROUT_CAMERA_ACT_START;
        work->seq[idx++] = SEQ_DOOROUT_PLAYER_STEP;
        work->seq[idx++] = SEQ_DOOROUT_END; 
        break;
      case DIR_LEFT: // ������
      case DIR_RIGHT: // �E����
        work->seq[idx++] = SEQ_DOOROUT_INIT;
        work->seq[idx++] = SEQ_DOOROUT_EFFECT_START;
        work->seq[idx++] = SEQ_DOOROUT_PLAYER_STEP;
        work->seq[idx++] = SEQ_DOOROUT_CAMERA_ACT_START;
        work->seq[idx++] = SEQ_DOOROUT_END; 
        break;
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă���C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT ExitEvent_DoorOut( GMEVENT * event, int *seq, void * wk )
{ 
	FIELD_DOOR_ANIME_WORK * work       = wk;
  ENTRANCE_EVDATA*        evdata     = work->evdata;
	GAMESYS_WORK *          gameSystem = evdata->gameSystem;
  GAMEDATA *              gameData   = evdata->gameData;
  FIELDMAP_WORK *         fieldmap   = evdata->fieldmap;
  FIELD_SOUND *           fieldSound = work->fieldSound;
  FIELD_CAMERA *          camera     = FIELDMAP_GetFieldCamera( fieldmap ); 
  FIELD_TASK_MAN *        task_man   = FIELDMAP_GetTaskManager( fieldmap );
  u16                     zone_id    = FIELDMAP_GetZoneID( fieldmap );

  switch( *seq ) {
  // �C�x���g�����ݒ�
  case SEQ_DOOROUT_INIT:
    // ���@������
    MAPCHANGE_setPlayerVanish( fieldmap, TRUE );

    // �g���[�X�V�X�e��������
    if( FIELD_CAMERA_CheckTraceSys( camera ) == FALSE ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    // �g���[�X�V�X�e�����L��
    else {
      FIELD_CAMERA_StopTraceRequest( camera ); // �J�����̃g���[�X�@�\��OFF
      *seq = SEQ_DOOROUT_CAMERA_TRACE_WAIT;
    }
    break;

  // �J�����̃g���[�X���������҂�
  case SEQ_DOOROUT_CAMERA_TRACE_WAIT:
    // �g���[�X����������
    if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break;

  // ���o�J�n
  case SEQ_DOOROUT_EFFECT_START:
    ENTRANCE_CAMERA_Setup( work->ECamWork, &work->ECamParam ); // �J�������o���Z�b�g�A�b�v
    FSND_PlayStartBGM( fieldSound, gameData, zone_id ); // BGM �Đ��J�n
    StartFadeInEvent( event, work ); // ��ʃt�F�[�h�C���J�n
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

  // �h�A�I�[�v���J�n
  case SEQ_DOOROUT_OPEN_ANIME_START:
    StartDoorOpen( work );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

  // �h�A�I�[�v���҂�
  case SEQ_DOOROUT_OPEN_ANIME_WAIT:
    if( CheckDoorAnimeFinish(work) == TRUE ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break;

  // ���@�o���A����ړ��A�j��
  case SEQ_DOOROUT_PLAYER_STEP:
    MAPCHANGE_setPlayerVanish( fieldmap, FALSE );
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime( gameSystem, fieldmap ) );

    // �J�������o���[���t���[���؂�ւ�
    if( ENTRANCE_CAMERA_IsAnimeExist( work->ECamWork ) &&
        ENTRANCE_CAMERA_IsZeroFrameAnime( work->ECamWork ) ) {
      *seq = SEQ_DOOROUT_WAIT_FOR_CAMERA_ACT_START;
    }
    else {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break;

  // �h�A�N���[�Y�J�n
  case SEQ_DOOROUT_CLOSE_ANIME_START:
    StartDoorClose( work );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

  // �J�����̃[���t���[���؂�ւ��҂�
  case SEQ_DOOROUT_WAIT_FOR_CAMERA_ACT_START:
    if( ZERO_CAMERA_WAIT_OUT < work->count++ ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break;

  // �J�������o�J�n
  case SEQ_DOOROUT_CAMERA_ACT_START:
    ENTRANCE_CAMERA_Start( work->ECamWork );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

  // �h�A�N���[�Y�҂�
  case SEQ_DOOROUT_CLOSE_ANIME_WAIT:
    if( CheckDoorAnimeFinish(work) == TRUE ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break; 
  }

  // 1�t���[���Z�k���邽��, �C�x���g�̏I�����R�R�Ŕ��肷��
  if( GetCurrentSeq( work ) == SEQ_DOOROUT_END ) {
    if( FIELD_TASK_MAN_IsAllTaskEnd( task_man ) ) {
      ENTRANCE_CAMERA_Recover( work->ECamWork );
      ENTRANCE_CAMERA_DeleteWork( work->ECamWork );
      DeleteDoorBM( work );
      return GMEVENT_RES_FINISH;
    }
  }
  return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�ɓ����A�̉��o�C�x���g�𐶐�����
 *
 * @param evdata �o������C�x���g�̋��ʃ��[�N
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldDoorInAnime( ENTRANCE_EVDATA* evdata )
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * work;

  // �C�x���g�𐶐� 
  event = GMEVENT_Create( 
      evdata->gameSystem, NULL, ExitEvent_DoorIn, sizeof(FIELD_DOOR_ANIME_WORK) ); 

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->evdata     = evdata;
  work->fieldSound = GAMEDATA_GetFieldSound( evdata->gameData );
  work->doorBM     = NULL;
  work->count      = 0;
  work->playerDir  = GetPlayerDir( evdata->fieldmap );
  work->seqPos     = 0;

  // �J�������o�̃Z�b�g�A�b�v�p�����[�^���쐬
  work->ECamWork = ENTRANCE_CAMERA_CreateWork( evdata->fieldmap );
  work->ECamParam.exitType  = evdata->exit_type_in;
  work->ECamParam.situation = ECAM_SITUATION_IN;
  work->ECamParam.oneStep   = ECAM_ONESTEP_ON;

  // �h�A������
  GetPlayerFrontPos( evdata->fieldmap, &work->doorSearchPos );
  SearchDoorBM( work );

  // �V�[�P���X�̗��������
  SetupDoorInEventSeq( work ); 

  return event;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�ɓ���C�x���g�̃V�[�P���X�̗�������肷��
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void SetupDoorInEventSeq( FIELD_DOOR_ANIME_WORK* work )
{
  ENTRANCE_EVDATA* evdata   = work->evdata;
  FIELDMAP_WORK*   fieldmap = evdata->fieldmap;
  int idx = 0;

  // �h�A������ꍇ
  if( CheckDoorExist(work) ) {
    work->seq[idx++] = SEQ_DOORIN_INIT;
    work->seq[idx++] = SEQ_DOORIN_OPEN_ANIME_START;
    work->seq[idx++] = SEQ_DOORIN_CAMERA_ACT_START;
    work->seq[idx++] = SEQ_DOORIN_CAMERA_ACT_WAIT;
    work->seq[idx++] = SEQ_DOORIN_PLAYER_STEP;
    work->seq[idx++] = SEQ_DOORIN_BGM_STAND_BY;
    work->seq[idx++] = SEQ_DOORIN_FADEOUT;
    work->seq[idx++] = SEQ_DOORIN_END; 
  }
  // �h�A���Ȃ��ꍇ
  else {
    work->seq[idx++] = SEQ_DOORIN_INIT;
    work->seq[idx++] = SEQ_DOORIN_CAMERA_ACT_START;
    work->seq[idx++] = SEQ_DOORIN_CAMERA_ACT_WAIT;
    work->seq[idx++] = SEQ_DOORIN_PLAYER_STEP;
    work->seq[idx++] = SEQ_DOORIN_BGM_STAND_BY;
    work->seq[idx++] = SEQ_DOORIN_FADEOUT;
    work->seq[idx++] = SEQ_DOORIN_END; 
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�ɓ���C�x���g
 */
//--------------------------------------------------------------------------------------------
static GMEVENT_RESULT ExitEvent_DoorIn( GMEVENT * event, int *seq, void * wk )
{ 
  FIELD_DOOR_ANIME_WORK * work       = wk;
  ENTRANCE_EVDATA*        evdata     = work->evdata;
  GAMESYS_WORK *          gameSystem = evdata->gameSystem;
  GAMEDATA *              gameData   = evdata->gameData;
  FIELDMAP_WORK *         fieldmap   = evdata->fieldmap;
  FIELD_SOUND *           fieldSound = work->fieldSound; 
  FIELD_TASK_MAN *        task_man   = FIELDMAP_GetTaskManager( fieldmap );

  switch( *seq ) { 
  case SEQ_DOORIN_INIT:
    ENTRANCE_CAMERA_Setup( work->ECamWork, &work->ECamParam ); // �J�������o���Z�b�g�A�b�v
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // �J�������o�J�n
  case SEQ_DOORIN_CAMERA_ACT_START: 
    ENTRANCE_CAMERA_Start( work->ECamWork );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // �h�A�I�[�v���J�n
  case SEQ_DOORIN_OPEN_ANIME_START:
    StartDoorOpen( work );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // �J�������o�҂�
  case SEQ_DOORIN_CAMERA_ACT_WAIT:
    if( FIELD_TASK_MAN_IsAllTaskEnd( task_man ) ) { 
      // �J�������o���[���t���[���؂�ւ�
      if( ENTRANCE_CAMERA_IsAnimeExist( work->ECamWork ) &&
          ENTRANCE_CAMERA_IsZeroFrameAnime( work->ECamWork ) ) {
        *seq = SEQ_DOORIN_WAIT_FOR_PLAYER_ONESTEP;
      }
      else {
        *seq = GetNextSeq( work );
        FinishCurrentSeq( work );
      }
    }
    break;

    // �h�A�I�[�v���҂�
  case SEQ_DOORIN_OPEN_ANIME_WAIT:
    if( CheckDoorAnimeFinish(work) == TRUE ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break;

    // �J�����̃[���t���[���؂�ւ��҂�
  case SEQ_DOORIN_WAIT_FOR_PLAYER_ONESTEP:
    if( ZERO_CAMERA_WAIT_IN < work->count++ ) {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
    break;

    // ���@�̈���ړ��A�j���J�n
  case SEQ_DOORIN_PLAYER_STEP:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime( gameSystem, fieldmap ) );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // BGM�Đ�����
  case SEQ_DOORIN_BGM_STAND_BY:
    FSND_StandByNextMapBGM( fieldSound, gameData, evdata->nextLocation.zone_id );
    evdata->BGM_standby_flag = TRUE;
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // ��ʃt�F�[�h�J�n
  case SEQ_DOORIN_FADEOUT:
    if( CheckMapChangeSE(work) ) {
      PMSND_PlaySE( SEQ_SE_KAIDAN );
    }
    StartFadeOutEvent( event, work );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // �C�x���g�I��
  case SEQ_DOORIN_END:
    ENTRANCE_CAMERA_Recover( work->ECamWork );
    ENTRANCE_CAMERA_DeleteWork( work->ECamWork );
    DeleteDoorBM( work );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//============================================================================================
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * @brief ���@�������擾����
 *
 * @param fieldmap
 *
 * @return ���@�̌��� ( DIR_xxxx )
 */
//--------------------------------------------------------------------------------------------
static u8 GetPlayerDir( FIELDMAP_WORK* fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
  return FIELD_PLAYER_GetDir( player );
}

//-----------------------------------------------------------------------------
/**
 * @brief ���@�̍��W���擾����
 *
 * @param fieldmap
 * @param dest �擾�������W�̊i�[��
 */
//-----------------------------------------------------------------------------
static void GetPlayerPos( FIELDMAP_WORK* fieldmap, VecFx32* dest )
{
  FIELD_PLAYER* player;

  player = FIELDMAP_GetFieldPlayer( fieldmap );
  FIELD_PLAYER_GetPos( player, dest );
}

//-----------------------------------------------------------------------------
/**
 * @brief �����̍��W���擾����
 *
 * @param fieldmap
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetPlayerFrontPos( FIELDMAP_WORK* fieldmap, VecFx32* dest )
{
  if( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    GetPlayerFrontPos_RAIL( fieldmap, dest );
  }
  else {
    GetPlayerFrontPos_GRID( fieldmap, dest );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �����̍��W���擾���� ( �O���b�h�}�b�v�p )
 *
 * @param fieldmap
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetPlayerFrontPos_GRID( FIELDMAP_WORK* fieldmap, VecFx32* dest )
{
  FIELD_PLAYER* player;
  u16 playerDir;
  VecFx32 now_pos, way_vec, step_pos;

  // ���@�f�[�^���擾
  player = FIELDMAP_GetFieldPlayer( fieldmap );
  playerDir = FIELD_PLAYER_GetDir( player );

  // ����O�i�������W���Z�o
  FIELD_PLAYER_GetPos( player, &now_pos );
  FIELD_PLAYER_GetDirWay( player, playerDir, &way_vec );
  VEC_MultAdd( FIELD_CONST_GRID_FX32_SIZE, &way_vec, &now_pos, &step_pos );

  // y ���W��␳
  {
    MMDL* mmdl;
    fx32 height;

    mmdl = FIELD_PLAYER_GetMMdl( player );
    MMDL_GetMapPosHeight( mmdl, &step_pos, &height );
    step_pos.y = height;
  }

  // ���W��Ԃ�
  *dest = step_pos;
}

//-----------------------------------------------------------------------------
/**
 * @brief �����̍��W���擾���� ( ���[���}�b�v�p )
 *
 * @param fieldmap
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetPlayerFrontPos_RAIL( FIELDMAP_WORK* fieldmap, VecFx32* dest )
{
  FIELD_PLAYER* player;
  u16 playerDir;
  RAIL_LOCATION location;
  FLDNOGRID_MAPPER* NGMapper;
  const FIELD_RAIL_MAN* railMan;
  VecFx32 step_pos;

  // ���@�f�[�^���擾
  player = FIELDMAP_GetFieldPlayer( fieldmap );
  playerDir = FIELD_PLAYER_GetDir( player );

  // �����̍��W���擾
  NGMapper = FIELDMAP_GetFldNoGridMapper( fieldmap );
  railMan = FLDNOGRID_MAPPER_GetRailMan( NGMapper );
  FIELD_PLAYER_GetDirNoGridLocation( player, playerDir, &location );
  FIELD_RAIL_MAN_GetLocationPosition( railMan, &location, &step_pos );

  // ���W��Ԃ�
  *dest = step_pos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���擾���� 
 *
 * @param work
 * 
 * @return ���݂̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static u8 GetCurrentSeq( const FIELD_DOOR_ANIME_WORK* work )
{
  GF_ASSERT( work->seqPos < MAX_SEQ_LENGTH );

  return work->seq[ work->seqPos ];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̃V�[�P���X���擾���� 
 *
 * @param work
 * 
 * @return ���̃V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static u8 GetNextSeq( const FIELD_DOOR_ANIME_WORK* work )
{
  u8 nextPos;

  nextPos = work->seqPos + 1;
  GF_ASSERT( nextPos < MAX_SEQ_LENGTH );

  return work->seq[ nextPos ];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void FinishCurrentSeq( FIELD_DOOR_ANIME_WORK* work )
{
  work->seqPos++;
  work->count = 0;
  GF_ASSERT( work->seqPos < MAX_SEQ_LENGTH );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A��T���A�z�u���f���𐶐�����
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void SearchDoorBM( FIELD_DOOR_ANIME_WORK* work )
{
  ENTRANCE_EVDATA* evdata = work->evdata;
  FLDMAPPER* fldmapper;
  FIELD_BMODEL_MAN* BModelMan;

  GF_ASSERT( work->doorBM == NULL );

  // �h�A������
  fldmapper = FIELDMAP_GetFieldG3Dmapper( evdata->fieldmap );
  BModelMan = FLDMAPPER_GetBuildModelManager( fldmapper );
  work->doorBM = FIELD_BMODEL_Create_Search( BModelMan, BM_SEARCH_ID_DOOR, &work->doorSearchPos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���������h�A�̔z�u���f����j������
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void DeleteDoorBM( FIELD_DOOR_ANIME_WORK* work )
{
  if( work->doorBM ) {
    FIELD_BMODEL_Delete( work->doorBM );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�A�j�����I���������ǂ����𔻒肷��
 *
 * @param work �C�x���g���[�N
 *
 * @return �h�A�A�j�����I�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckDoorAnimeFinish( const FIELD_DOOR_ANIME_WORK* work )
{ 
  // �h�A�����݂��Ȃ�
  if( CheckDoorExist( work ) == FALSE ) {
    GF_ASSERT(0);
    return TRUE;
  }

  // �A�j�����I����Ă��Ȃ�
  if( FIELD_BMODEL_WaitCurrentAnime( work->doorBM ) == FALSE ) { return FALSE; }

  // SE �Đ���
  // 100415 �h�A����o���Ƃ��ɑ҂�����Ă��܂�����, �R�����g�A�E�g
  //if( FIELD_BMODEL_CheckCurrentSE( work->doorBM ) == TRUE ) { return FALSE; } 

  // �A�j���I��
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�����݂��邩�ǂ������`�F�b�N����
 *
 * @param work �C�x���g���[�N
 *
 * @return �h�A�����݂���ꍇ TRUE
 *          �����łȂ���� FALSE
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckDoorExist( const FIELD_DOOR_ANIME_WORK* work )
{
  if( work->doorBM == NULL ) { 
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A���J���A�j�����J�n����
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void StartDoorOpen( const FIELD_DOOR_ANIME_WORK* work )
{
  u16 seNo;

  // �h�A�����݂��Ȃ�
  if( CheckDoorExist( work ) == FALSE ) {
    GF_ASSERT(0);
    return;
  }

  // �A�j���J�n
  FIELD_BMODEL_StartAnime( work->doorBM, BMANM_INDEX_DOOR_OPEN );

  // SE���Đ�
  if( FIELD_BMODEL_GetCurrentSENo( work->doorBM, &seNo ) ) {
    PMSND_PlaySE( seNo );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�����A�j�����J�n����
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void StartDoorClose( const FIELD_DOOR_ANIME_WORK* work )
{
  u16 seNo;

  // �h�A�����݂��Ȃ�
  if( CheckDoorExist( work ) == FALSE ) {
    GF_ASSERT(0);
    return;
  }

  // �A�j���J�n
  FIELD_BMODEL_StartAnime( work->doorBM, BMANM_INDEX_DOOR_CLOSE );

  // SE���Đ�
  if( FIELD_BMODEL_GetCurrentSENo( work->doorBM, &seNo ) ) {
    PMSND_PlaySE( seNo );
  }
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�C�����J�n����
 *
 * @param event ���݂̃C�x���g
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void StartFadeInEvent( GMEVENT* event, const FIELD_DOOR_ANIME_WORK* work )
{
  ENTRANCE_EVDATA* evdata = work->evdata;
  GMEVENT* fadeEvent;

  // �G�߃t�F�[�h
  if( evdata->season_disp_flag ) { 
    fadeEvent = EVENT_FieldFadeIn_Season( 
        evdata->gameSystem, evdata->fieldmap, evdata->start_season, evdata->end_season );
  }
  // ��{�t�F�[�h
  else { 
    fadeEvent = EVENT_FieldFadeIn( 
        evdata->gameSystem, evdata->fieldmap, evdata->fadein_type, FIELD_FADE_WAIT, TRUE, 0, 0 );
  }

  GMEVENT_CallEvent( event, fadeEvent );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g���J�n����
 *
 * @param event ���݂̃C�x���g
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void StartFadeOutEvent( GMEVENT* event, const FIELD_DOOR_ANIME_WORK* work )
{
  ENTRANCE_EVDATA* evdata = work->evdata;
  GMEVENT* fadeEvent;

  // �P�x�t�F�[�h
  if( evdata->season_disp_flag ) { 
    fadeEvent = EVENT_FieldFadeOut_Black( 
        evdata->gameSystem, evdata->fieldmap, FIELD_FADE_WAIT );
  }
  // ��{�t�F�[�h
  else { 
    fadeEvent = EVENT_FieldFadeOut( 
        evdata->gameSystem, evdata->fieldmap, evdata->fadeout_type, FIELD_FADE_WAIT );
  }

  GMEVENT_CallEvent( event, fadeEvent );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �}�b�v�`�F���WSE�̍Đ����`�F�b�N����
 *
 * @param work
 *
 * @return SE���Đ�����ꍇ TRUE
 *         �Đ����Ȃ��ꍇ FALSE
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckMapChangeSE( const FIELD_DOOR_ANIME_WORK* work )
{
  ENTRANCE_EVDATA* evdata = work->evdata;

  // �h�A������ꍇ�͍Đ����Ȃ�
  if( CheckDoorExist(work) ) { return FALSE; }

  // �N���X�t�F�[�h�̏ꍇ�͍Đ����Ȃ�
  if( evdata->fadeout_type == FIELD_FADE_CROSS ) { return FALSE; }

  return TRUE;
}
