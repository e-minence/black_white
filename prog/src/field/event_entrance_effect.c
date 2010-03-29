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

#include "field/field_const.h"

#include "fieldmap.h"
#include "field_player.h"
#include "field_buildmodel.h"

#include "event_mapchange.h"        //MAPCHANGE_setPlayerVanish
#include "event_fieldmap_control.h" //EVENT_FieldFadeOut
#include "event_fldmmdl_control.h"  //EVENT_PlayerOneStepAnime
#include "event_entrance_effect.h"

#include "event_camera_act.h" 

#include "sound/pm_sndsys.h"      //PMSND_PlaySE
#include "field/field_sound.h"    // for FIELD_SOUND_ChangePlayZoneBGM


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
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

  // �ȉ��̓V�[�P���X���쒆�ɃZ�b�g�����
  FIELD_BMODEL * ctrl;

} FIELD_DOOR_ANIME_WORK;


//============================================================================================
//============================================================================================
static void GetPlayerFrontPos( FIELDMAP_WORK * fieldmap, VecFx32 * pos );
static BOOL CheckDoorAnimeFinish( const FIELD_DOOR_ANIME_WORK* work );
static BOOL CheckDoorExist( const FIELD_DOOR_ANIME_WORK* work );
static void StartDoorOpen( const FIELD_DOOR_ANIME_WORK* work );
static void StartDoorClose( const FIELD_DOOR_ANIME_WORK* work );



//------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă���C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT ExitEvent_DoorOut( GMEVENT * event, int *seq, void * wk )
{
  enum {
    SEQ_DOOROUT_INIT = 0,
    SEQ_DOOROUT_CAMERA_TRACE_WAIT,
    SEQ_DOOROUT_FADEIN,
    SEQ_DOOROUT_CAMERA_ACT_START,
    SEQ_DOOROUT_SOUND_LOAD_WAIT,
    SEQ_DOOROUT_OPENANIME_START,
    SEQ_DOOROUT_OPENANIME_WAIT,
    SEQ_DOOROUT_PLAYER_STEP,
    SEQ_DOOROUT_CLOSEANIME_START,
    SEQ_DOOROUT_CLOSEANIME_WAIT,
    SEQ_DOOROUT_END,
  };

	FIELD_DOOR_ANIME_WORK * work       = wk;
	GAMESYS_WORK *          gameSystem = work->gameSystem;
  GAMEDATA *              gameData   = work->gameData;
  FIELDMAP_WORK *         fieldmap   = work->fieldmap;
  FIELD_SOUND *           fieldSound = work->fieldSound;

  FLDMAPPER *        fldmapper  = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( fldmapper );

  switch( *seq ) {
  // �C�x���g�����ݒ�
  case SEQ_DOOROUT_INIT:
    // �t�F�[�h�C������O��, �J�����̏����ݒ���s��
    if( work->cameraAnimeFlag ) {
      EVENT_CAMERA_ACT_PrepareForDoorOut( fieldmap );
    }
    // ���@������
    MAPCHANGE_setPlayerVanish( fieldmap, TRUE );
    // �J�����̃g���[�X�@�\��OFF
    FIELD_CAMERA_StopTraceRequest( FIELDMAP_GetFieldCamera( fieldmap ) );
    *seq = SEQ_DOOROUT_CAMERA_TRACE_WAIT;
    break;

  // �J�����̃g���[�X���������҂�
  case SEQ_DOOROUT_CAMERA_TRACE_WAIT:
    // �g���[�X����������
    if( FIELD_CAMERA_CheckTrace( FIELDMAP_GetFieldCamera( fieldmap ) ) == FALSE ) {
      FSND_PlayStartBGM( fieldSound ); // BGM �Đ��J�n
      *seq = SEQ_DOOROUT_FADEIN;
    }
    break;

  // ��ʃt�F�[�h�C���J�n
  case SEQ_DOOROUT_FADEIN:
    if( work->seasonDispFlag ) { 
      // �G�߃t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Season( gameSystem, fieldmap, work->startSeason, work->endSeason ) );
    }
    else { 
      // ��{�t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn( gameSystem, fieldmap, work->fadeType, FIELD_FADE_WAIT, TRUE, 0, 0 ) );
    }
    *seq = SEQ_DOOROUT_SOUND_LOAD_WAIT;
    break;

  // �T�E���h�̓ǂݍ��ݑ҂�
  case SEQ_DOOROUT_SOUND_LOAD_WAIT:
    if( PMSND_IsLoading() != TRUE ) {
      *seq = SEQ_DOOROUT_OPENANIME_START;
    }
    break;

  // �h�A�I�[�v���J�n
  case SEQ_DOOROUT_OPENANIME_START:
    work->ctrl = FIELD_BMODEL_Create_Search( bmodel_man, BM_SEARCH_ID_DOOR, &work->doorPos );
    if( CheckDoorExist( work ) == TRUE ) {
      StartDoorOpen( work );
    }
    *seq = SEQ_DOOROUT_CAMERA_ACT_START;
    break;

  // �J�������o�J�n
  case SEQ_DOOROUT_CAMERA_ACT_START:
    if( work->cameraAnimeFlag ) {
      EVENT_CAMERA_ACT_CallDoorOutEvent( event, gameSystem, fieldmap );
    }
    // �h�A���Ȃ��ꍇ
    if( CheckDoorExist(work) == FALSE ) {
      *seq = SEQ_DOOROUT_PLAYER_STEP; 
    }
    // �h�A������ꍇ
    else {
      *seq = SEQ_DOOROUT_OPENANIME_WAIT; 
    }
    break;

  // �h�A�I�[�v���҂�
  case SEQ_DOOROUT_OPENANIME_WAIT:
    if( CheckDoorAnimeFinish(work) == TRUE ) {
      *seq = SEQ_DOOROUT_PLAYER_STEP;
    }
    break;

  // ���@�o���A����ړ��A�j��
  case SEQ_DOOROUT_PLAYER_STEP:
    MAPCHANGE_setPlayerVanish( fieldmap, FALSE );
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime( gameSystem, fieldmap ) );
    *seq = SEQ_DOOROUT_CLOSEANIME_START;
    break;

  // �h�A�N���[�Y�J�n
  case SEQ_DOOROUT_CLOSEANIME_START:
    // �h�A���Ȃ��ꍇ
    if( CheckDoorExist(work) == FALSE ) {
      *seq = SEQ_DOOROUT_END;
    }
    // �h�A������ꍇ
    else {
      StartDoorClose( work );
      *seq = SEQ_DOOROUT_CLOSEANIME_WAIT;
    }
    break;

  // �h�A�N���[�Y�҂�
  case SEQ_DOOROUT_CLOSEANIME_WAIT:
    // �h�A�A�j�����I��
    if( CheckDoorAnimeFinish(work) == TRUE ) {
      *seq = SEQ_DOOROUT_END;
    }
    break; 

  // �C�x���g�I��
  case SEQ_DOOROUT_END:
    EVENT_CAMERA_ACT_ResetCameraParameter( fieldmap );  // �J�����̐ݒ���f�t�H���g�ɖ߂�
    FIELD_BMODEL_Delete( work->ctrl );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
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
 *
 * @return ���������C�x���g
 */
//------------------------------------------------------------------
GMEVENT * EVENT_FieldDoorOutAnime ( GAMESYS_WORK * gameSystem, FIELDMAP_WORK * fieldmap, 
                                    BOOL cameraAnimeFlag,
                                    BOOL seasonDispFlag, u8 startSeason, u8 endSeason,
                                    FIELD_FADE_TYPE fadeType )
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
  work->ctrl            = NULL;

  return event;
}

//------------------------------------------------------------------
/**
 * @brief �h�A�ɓ���C�x���g
 */
//------------------------------------------------------------------
static GMEVENT_RESULT ExitEvent_DoorIn(GMEVENT * event, int *seq, void * wk)
{
  enum {
    SEQ_DOORIN_OPENANIME_START = 0,
    SEQ_DOORIN_CAMERA_ACT,
    SEQ_DOORIN_OPENANIME_WAIT,
    SEQ_DOORIN_PLAYER_ONESTEP,
    SEQ_DOORIN_BGM_STAND_BY,
    SEQ_DOORIN_FADEOUT,
    SEQ_DOORIN_END,
  };

	FIELD_DOOR_ANIME_WORK * work = wk;
	GAMESYS_WORK  * gameSystem = work->gameSystem;
  GAMEDATA * gameData = GAMESYSTEM_GetGameData( gameSystem );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );

  FLDMAPPER * fldmapper = FIELDMAP_GetFieldG3Dmapper(fieldmap);
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager(fldmapper);

  switch (*seq)
  { 
  case SEQ_DOORIN_OPENANIME_START:
    work->ctrl = FIELD_BMODEL_Create_Search( bmodel_man, BM_SEARCH_ID_DOOR, &work->doorPos );
    if (work->ctrl)
    {
      u16 seNo;
      FIELD_BMODEL_StartAnime( work->ctrl, BMANM_INDEX_DOOR_OPEN );
      if( FIELD_BMODEL_GetCurrentSENo( work->ctrl, &seNo) )
      {
        PMSND_PlaySE( seNo );
      }
    }
    *seq = SEQ_DOORIN_CAMERA_ACT;
    //*seq = SEQ_DOORIN_PLAYER_ONESTEP;
    break;

  case SEQ_DOORIN_CAMERA_ACT: 
    if( work->cameraAnimeFlag )
    {
      EVENT_CAMERA_ACT_CallDoorInEvent( event, gameSystem, fieldmap ); 
    }
    if (work->ctrl == NULL)
    { /* �G���[�悯�A�h�A���Ȃ��ꍇ */
      *seq = SEQ_DOORIN_PLAYER_ONESTEP;
    } 
    else
    {
      *seq = SEQ_DOORIN_OPENANIME_WAIT;
    }
    break;

  case SEQ_DOORIN_OPENANIME_WAIT:
    if( CheckDoorAnimeFinish(work) == TRUE ) {
      *seq = SEQ_DOORIN_PLAYER_ONESTEP;
    }
    break;

  case SEQ_DOORIN_PLAYER_ONESTEP:
    GMEVENT_CallEvent( event, EVENT_PlayerOneStepAnime(gameSystem, fieldmap) );
    *seq = SEQ_DOORIN_BGM_STAND_BY;
    break;

  case SEQ_DOORIN_BGM_STAND_BY:
    { // BGM�Đ�����
      u16 nowZoneID = FIELDMAP_GetZoneID( fieldmap );
      FIELD_SOUND* fieldSound = GAMEDATA_GetFieldSound( gameData );
      FSND_StandByNextMapBGM( fieldSound, gameData, work->loc_req.zone_id );
    }
    *seq = SEQ_DOORIN_FADEOUT;
    break;

  case SEQ_DOORIN_FADEOUT:
    // ��ʃt�F�[�h�A�E�g
    if( work->seasonDispFlag )
    { // �P�x�t�F�[�h
      GMEVENT_CallEvent( event, EVENT_FieldFadeOut_Black(gameSystem, fieldmap, FIELD_FADE_WAIT) );
    }
    else
    { // ��{�t�F�[�h
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeOut(gameSystem, fieldmap, work->fadeType, FIELD_FADE_WAIT) );
    }
    *seq = SEQ_DOORIN_END;
    break;

  case SEQ_DOORIN_END:
    if( work->cameraAnimeFlag )
    {
      EVENT_CAMERA_ACT_ResetCameraParameter( fieldmap );  // �J�����̐ݒ���f�t�H���g�ɖ߂�
    }
    FIELD_BMODEL_Delete( work->ctrl );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
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
//------------------------------------------------------------------
GMEVENT* EVENT_FieldDoorInAnime ( 
    GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, const LOCATION* loc, 
    BOOL cameraAnimeFlag, BOOL seasonDispFlag, FIELD_FADE_TYPE fadeType )
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
  work->ctrl            = NULL;

  return event;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���@�̈����̍��W���擾����
 *
 * @param fieldmap
 * @param pos �擾�������W�̊i�[��
 */
//------------------------------------------------------------------
static void GetPlayerFrontPos(FIELDMAP_WORK * fieldmap, VecFx32 * pos)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldmap);
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16 dir;

  dir = MMDL_GetDirDisp( fmmdl );

  FIELD_PLAYER_GetDirPos( player, dir, pos );
} 

//------------------------------------------------------------------
/**
 * @brief �h�A�A�j�����I���������ǂ����𔻒肷��
 *
 * @param work �C�x���g���[�N
 *
 * @return �h�A�A�j�����I�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------
static BOOL CheckDoorAnimeFinish( const FIELD_DOOR_ANIME_WORK* work )
{ 
  // �h�A�����݂��Ȃ�
  if( CheckDoorExist( work ) == FALSE ) {
    GF_ASSERT(0);
    return TRUE;
  }

  // �A�j�����I����Ă��Ȃ�
  if( FIELD_BMODEL_WaitCurrentAnime( work->ctrl ) == FALSE ) { return FALSE; }

  // SE �Đ���
  if( FIELD_BMODEL_CheckCurrentSE( work->ctrl ) == TRUE ) { return FALSE; } 

  // �A�j���I��
  return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief �h�A�����݂��邩�ǂ������`�F�b�N����
 *
 * @param work �C�x���g���[�N
 *
 * @return �h�A�����݂���ꍇ TRUE
 *          �����łȂ���� FALSE
 */
//------------------------------------------------------------------
static BOOL CheckDoorExist( const FIELD_DOOR_ANIME_WORK* work )
{
  if( work->ctrl == NULL ) { 
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//------------------------------------------------------------------
/**
 * @brief �h�A���J���A�j�����J�n����
 *
 * @param work
 */
//------------------------------------------------------------------
static void StartDoorOpen( const FIELD_DOOR_ANIME_WORK* work )
{
  u16 seNo;

  // �h�A�����݂��Ȃ�
  if( CheckDoorExist( work ) == FALSE ) {
    GF_ASSERT(0);
    return;
  }

  // �A�j���J�n
  FIELD_BMODEL_StartAnime( work->ctrl, BMANM_INDEX_DOOR_OPEN );

  // SE���Đ�
  if( FIELD_BMODEL_GetCurrentSENo( work->ctrl, &seNo ) ) {
    PMSND_PlaySE( seNo );
  }
}

//------------------------------------------------------------------
/**
 * @brief �h�A�����A�j�����J�n����
 *
 * @param work
 */
//------------------------------------------------------------------
static void StartDoorClose( const FIELD_DOOR_ANIME_WORK* work )
{
  u16 seNo;

  // �h�A�����݂��Ȃ�
  if( CheckDoorExist( work ) == FALSE ) {
    GF_ASSERT(0);
    return;
  }

  // �A�j���J�n
  FIELD_BMODEL_StartAnime( work->ctrl, BMANM_INDEX_DOOR_CLOSE );

  // SE���Đ�
  if( FIELD_BMODEL_GetCurrentSENo( work->ctrl, &seNo ) ) {
    PMSND_PlaySE( seNo );
  }
}

















