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
  SEQ_DOOROUT_SOUND_LOAD_WAIT,
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

  // �ȉ��̓��[�N�������ɃZ�b�g�����
  GAMESYS_WORK *  gameSystem;      //<<<�Q�[���V�X�e���ւ̃|�C���^
  GAMEDATA *      gameData;        //<<<�Q�[���f�[�^�ւ̃|�C���^
  FIELDMAP_WORK * fieldmap;        //<<<�t�B�[���h�}�b�v�ւ̃|�C���^
  FIELD_SOUND *   fieldSound;      //<<<�t�B�[���h�T�E���h�ւ̃|�C���^
  LOCATION        loc_req;         //<<<���̃}�b�v���w�����P�[�V�����w�� ( BGM�t�F�[�h�����̌W�� )
  VecFx32         doorPos;         //<<<�h�A����������ꏊ
  BOOL            cameraAnimeFlag; //<<<�J�����A�j���[�V�����̗L��
  BOOL            seasonDispFlag;  //<<<�l�G�\�����s�����ǂ���
  u8              startSeason;     //<<<�ŏ��ɕ\������G�� ( �l�G�\�����s���ꍇ�̂ݗL�� )
  u8              endSeason;       //<<<�Ō�ɕ\������G�� ( �l�G�\�����s���ꍇ�̂ݗL�� )
  FIELD_FADE_TYPE fadeType;        //<<<�G�ߕ\�����s��Ȃ��ꍇ�̃t�F�[�h�^�C�v
  ECAM_WORK*      ECamWork;        //<<<�J�������o���[�N
  ECAM_PARAM      ECamParam;       //<<<�J�������o�p�����[�^
  EXIT_TYPE       exitType;        //<<<�o������^�C�v
  u16             count;           //<<<�J�E���^
  u8              playerDir;       //<<<���@�̌���
  FIELD_BMODEL *  doorBM;          //<<<�h�A�̔z�u���f��
  u8              seq[ MAX_SEQ_LENGTH ]; //<<<�V�[�P���X
  u8              seqPos;                //<<<���݂̃V�[�P���X�ʒu

} FIELD_DOOR_ANIME_WORK;


//============================================================================================
// ��index
//============================================================================================
static void SetupDoorOutEventSeq( FIELD_DOOR_ANIME_WORK* work ); // �h�A����o�Ă���C�x���g�̃V�[�P���X�̗�������肷��
static GMEVENT_RESULT ExitEvent_DoorOut( GMEVENT * event, int *seq, void * wk ); // �h�A����o�Ă���C�x���g�����֐�

static void SetupDoorInEventSeq( FIELD_DOOR_ANIME_WORK* work ); // �h�A�ɓ���C�x���g�̃V�[�P���X�̗�������肷��
static GMEVENT_RESULT ExitEvent_DoorIn(GMEVENT * event, int *seq, void * wk); // �h�A�ɓ���C�x���g�����֐�

static u8 GetPlayerDir( FIELDMAP_WORK* fieldmap ); // ���@�̌������擾����
static void GetPlayerFrontPos( FIELDMAP_WORK * fieldmap, VecFx32 * pos ); // ����O�i�����ʒu�̍��W���擾����
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


//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A����łĂ����A�̉��o�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap
 * @param cameraAnimeFlag �J�����A�j���[�V�����̗L��
 * @param seasonDispFlag  �l�G�\�����s�����ǂ���
 * @param startSeason     �ŏ��ɕ\������G�� ( �l�G�\�����s���ꍇ�̂ݗL�� )
 * @param endSeason       �Ō�ɕ\������G�� ( �l�G�\�����s���ꍇ�̂ݗL�� )
 * @param fadeType        �G�ߕ\�����s��Ȃ��ꍇ�̃t�F�[�h�^�C�v
 * @param exitType        �o������^�C�v
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT * EVENT_FieldDoorOutAnime ( GAMESYS_WORK * gameSystem, FIELDMAP_WORK * fieldmap, 
                                    BOOL cameraAnimeFlag,
                                    BOOL seasonDispFlag, u8 startSeason, u8 endSeason,
                                    FIELD_FADE_TYPE fadeType,
                                    EXIT_TYPE exitType )
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * work;

  // �C�x���g�𐶐�
  event = GMEVENT_Create( 
      gameSystem, NULL, ExitEvent_DoorOut, sizeof(FIELD_DOOR_ANIME_WORK) ); 

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem );
  work->fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  work->fieldSound = GAMEDATA_GetFieldSound( work->gameData );
  GetPlayerFrontPos(fieldmap, &work->doorPos);
  work->cameraAnimeFlag = cameraAnimeFlag;
  work->seasonDispFlag  = seasonDispFlag;
  work->startSeason     = startSeason;
  work->endSeason       = endSeason;
  work->fadeType        = fadeType;
  work->doorBM          = NULL;
  work->exitType        = exitType;
  work->count           = 0;
  work->playerDir       = GetPlayerDir( fieldmap );
  work->seqPos          = 0;

  // �J�������o�̃Z�b�g�A�b�v�p�����[�^���쐬
  work->ECamWork = ENTRANCE_CAMERA_CreateWork( fieldmap );
  work->ECamParam.exitType  = exitType;
  work->ECamParam.situation = ECAM_SITUATION_OUT;
  work->ECamParam.oneStep   = ECAM_ONESTEP_ON;

  // �h�A������
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
  FIELDMAP_WORK* fieldmap = work->fieldmap;
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
    if( EXIT_TYPE_IsSpExit(work->exitType) ) {
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
	GAMESYS_WORK *          gameSystem = work->gameSystem;
  GAMEDATA *              gameData   = work->gameData;
  FIELDMAP_WORK *         fieldmap   = work->fieldmap;
  FIELD_SOUND *           fieldSound = work->fieldSound;
  FIELD_CAMERA *          camera     = FIELDMAP_GetFieldCamera( fieldmap ); 
  FIELD_TASK_MAN *   task_man   = FIELDMAP_GetTaskManager( fieldmap );

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
    FSND_PlayStartBGM( fieldSound ); // BGM �Đ��J�n
    StartFadeInEvent( event, work ); // ��ʃt�F�[�h�C���J�n
    *seq = SEQ_DOOROUT_SOUND_LOAD_WAIT;
    break;

  // �T�E���h�̓ǂݍ��ݑ҂�
  case SEQ_DOOROUT_SOUND_LOAD_WAIT:
    if( PMSND_IsLoading() ) {
      OS_Printf( "WARNING!!: BGM�̓ǂݍ��݂��Ԃɍ����Ă��܂���!\n" ); // �x��!!
    }
    else {
      *seq = GetNextSeq( work );
      FinishCurrentSeq( work );
    }
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

  // �C�x���g�I��
  case SEQ_DOOROUT_END:
    ENTRANCE_CAMERA_Recover( work->ECamWork );
    ENTRANCE_CAMERA_DeleteWork( work->ECamWork );
    DeleteDoorBM( work );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief �h�A�ɓ����A�̉��o�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap
 * @param loc             ���̃}�b�v���w��
 * @param cameraAnimeFlag �J�����A�j���[�V�����̗L��
 * @param seasonDispFlag  �l�G�\�����s�����ǂ���
 * @param fadeType        �l�G�\�����s��Ȃ��ꍇ�̃t�F�[�h�^�C�v
 *
 * @return ���������C�x���g
 */
//--------------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldDoorInAnime ( 
    GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, const LOCATION* loc, 
    BOOL cameraAnimeFlag, BOOL seasonDispFlag, FIELD_FADE_TYPE fadeType,
    EXIT_TYPE exitType )
{
  GMEVENT * event;
  FIELD_DOOR_ANIME_WORK * work;

  // �C�x���g�𐶐� 
  event = GMEVENT_Create( 
      gameSystem, NULL, ExitEvent_DoorIn, sizeof(FIELD_DOOR_ANIME_WORK) ); 

  // �C�x���g���[�N��������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gameSystem;
  work->gameData   = GAMESYSTEM_GetGameData( gameSystem );
  work->fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  work->fieldSound = GAMEDATA_GetFieldSound( work->gameData );
  work->loc_req    = *loc;
  GetPlayerFrontPos(fieldmap, &work->doorPos);
  work->cameraAnimeFlag = cameraAnimeFlag;
  work->seasonDispFlag  = seasonDispFlag;
  work->fadeType        = fadeType;
  work->doorBM          = NULL;
  work->exitType        = exitType;
  work->count           = 0;
  work->playerDir       = GetPlayerDir( fieldmap );
  work->seqPos          = 0;

  // �J�������o�̃Z�b�g�A�b�v�p�����[�^���쐬
  work->ECamWork = ENTRANCE_CAMERA_CreateWork( fieldmap );
  work->ECamParam.exitType  = exitType;
  work->ECamParam.situation = ECAM_SITUATION_IN;
  work->ECamParam.oneStep   = ECAM_ONESTEP_ON;

  // �h�A������
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
  FIELDMAP_WORK* fieldmap = work->fieldmap;
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
  GAMESYS_WORK *          gameSystem = work->gameSystem;
  GAMEDATA *              gameData   = work->gameData;
  FIELDMAP_WORK *         fieldmap   = work->fieldmap;
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
    FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // ��ʃt�F�[�h�J�n
  case SEQ_DOORIN_FADEOUT:
    StartFadeOutEvent( event, work );
    *seq = GetNextSeq( work );
    FinishCurrentSeq( work );
    break;

    // �C�x���g�I��
  case SEQ_DOORIN_END:
    if( FIELD_TASK_MAN_IsAllTaskEnd( task_man ) ) {
      ENTRANCE_CAMERA_Recover( work->ECamWork );
      ENTRANCE_CAMERA_DeleteWork( work->ECamWork );
      DeleteDoorBM( work );
    }
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

//--------------------------------------------------------------------------------------------
/**
 * @brief ���@�̈����̍��W���擾����
 *
 * @param fieldmap
 * @param pos �擾�������W�̊i�[��
 */
//--------------------------------------------------------------------------------------------
static void GetPlayerFrontPos(FIELDMAP_WORK * fieldmap, VecFx32 * pos)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16 dir;

  dir = MMDL_GetDirDisp( fmmdl );

  FIELD_PLAYER_GetDirPos( player, dir, pos );
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
  FLDMAPPER* fldmapper;
  FIELD_BMODEL_MAN* BModelMan;

  GF_ASSERT( work->doorBM == NULL );

  fldmapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
  BModelMan = FLDMAPPER_GetBuildModelManager( fldmapper );
  work->doorBM = FIELD_BMODEL_Create_Search( BModelMan, BM_SEARCH_ID_DOOR, &work->doorPos );
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
  if( FIELD_BMODEL_CheckCurrentSE( work->doorBM ) == TRUE ) { return FALSE; } 

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
  GMEVENT* fadeEvent;

  if( work->seasonDispFlag ) { 
    // �G�߃t�F�[�h
    fadeEvent = EVENT_FieldFadeIn_Season( 
        work->gameSystem, work->fieldmap, work->startSeason, work->endSeason );
  }
  else { 
    // ��{�t�F�[�h
    fadeEvent = EVENT_FieldFadeIn( 
        work->gameSystem, work->fieldmap, work->fadeType, FIELD_FADE_WAIT, TRUE, 0, 0 );
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
  GMEVENT* fadeEvent;

  if( work->seasonDispFlag ) { 
    // �P�x�t�F�[�h
    fadeEvent = EVENT_FieldFadeOut_Black( 
        work->gameSystem, work->fieldmap, FIELD_FADE_WAIT );
  }
  else { 
    // ��{�t�F�[�h
    fadeEvent = EVENT_FieldFadeOut( 
        work->gameSystem, work->fieldmap, work->fadeType, FIELD_FADE_WAIT );
  }

  GMEVENT_CallEvent( event, fadeEvent );
}
