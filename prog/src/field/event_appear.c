////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �ޏꁕ�o��C�x���g
 * @file   event_appear.c
 * @author obata
 * @date   2009.08.28
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "event_appear.h"
#include "fieldmap.h"
#include "field_player.h"
#include "sound/pm_sndsys.h"

#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_player_rot.h"
#include "field_task_player_fall.h"
#include "field_task_wait.h"
#include "field_task_player_drawoffset.h"
#include "field_camera_anime.h"

#include "fldeff_kemuri.h"  // for FLDEFF_KEMURI_SetMMdl
#include "event_fieldmap_control.h"  // for EVENT_FieldFadeIn
#include "event_season_display.h"  // for GetSeasonDispEventFrame


//==========================================================================================
// ���萔
//========================================================================================== 
#define ZOOM_IN_DIST   (180 << FX32_SHIFT)   // �J�����̃Y�[���C������
#define ZOOM_IN_FRAME  (60)   // �Y�[���C���Ɋ|����t���[����
#define ZOOM_OUT_FRAME (60)   // �Y�[���A�E�g�Ɋ|����t���[����


//==========================================================================================
// ���C�x���g���[�N
//==========================================================================================
typedef struct { 

  FIELD_CAMERA_MODE cameraMode;   // �C�x���g�J�n���̃J�������[�h
  FIELDMAP_WORK*      fieldmap;   // ����t�B�[���h�}�b�v
  GAMESYS_WORK*           gsys;   // �Q�[���V�X�e��

  // �J�������A�p�����[�^
  fx32 length;

  //-----------------------
  FCAM_ANIME_DATA FCamAnimeData;
  FCAM_ANIME_WORK* FCamAnimeWork;
  //-----------------------

} EVENT_WORK;


//==========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================== 
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Teleport( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Warp( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_UnionIn( GMEVENT* event, int* seq, void* wk );


//========================================================================================== 
// �����J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���� )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Fall( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Fall, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->gsys     = gsys;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���Ȃʂ��̃q�� )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Ananukenohimo( GMEVENT* parent, 
                                     GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Ananukenohimo, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->gsys     = gsys;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���Ȃ��ق� )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Anawohoru( GMEVENT* parent, 
                                 GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Anawohoru, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->gsys     = gsys;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( �e���|�[�g )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Teleport( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Teleport, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->gsys     = gsys;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���[�v )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Warp( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Warp, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->gsys     = gsys;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���j�I������ )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_UnionIn( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_UnionIn, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->gsys     = gsys;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//========================================================================================== 
// ������J�֐��̒�`
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ���� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // ��l���̕`��I�t�Z�b�g�̏����ݒ�(��ʊO�ɂ���悤�ɐݒ�)
      VecFx32       offset = {0, 250<<FX32_SHIFT, 0};
      FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
      MMDL_SetVectorDrawOffsetPos( mmdl, &offset );
			MMDL_SetAcmd( mmdl, AC_DIR_D ); // ������������
    }
    { // �t�F�[�h�C��
      GMEVENT* fadeInEvent;
      fadeInEvent = EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fadeInEvent );
    }
    { // �^�X�N�̒ǉ�
      FIELD_TASK* fall;
      FIELD_TASK_MAN* man;
      FIELD_PLAYER* player;
			MMDL* mmdl;
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
			mmdl   = FIELD_PLAYER_GetMMdl( player ); 
      fall   = FIELD_TASK_PlayerFall( work->fieldmap, mmdl, 40, 250 );
      man    = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, fall, NULL );
    }
    (*seq)++;
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        (*seq)++;
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    (*seq)++;
    break;
  case 3: 
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ���Ȃʂ��̃q�� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work       = (EVENT_WORK*)wk;
  GAMESYS_WORK*   gameSystem = work->gsys;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_CAMERA*   camera     = FIELDMAP_GetFieldCamera( fieldmap );
  FIELD_TASK_MAN* taskMan     = FIELDMAP_GetTaskManager( fieldmap ); 

  switch( *seq ) {
  case 0:
    // �J�����̃g���[�X������~���N�G�X�g���s
    FIELD_CAMERA_StopTraceRequest( camera );
    (*seq)++;
    break;
  case 1:
    // �J�����̃g���[�X�����I���҂�
    if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ) { (*seq)++; }
    break;

  case 2:
    // �^�X�N�̒ǉ�
    {
      FIELD_TASK* rot;
      rot = FIELD_TASK_PlayerRotate_SpeedDown( fieldmap, 60, 8 );
      FIELD_TASK_MAN_AddTask( taskMan, rot, NULL );
    }
    // �J�������o�J�n
    work->FCamAnimeWork = FCAM_ANIME_CreateWork( fieldmap );
    work->FCamAnimeData.frame = 60;
    work->FCamAnimeData.targetBindOffFlag = TRUE;
    work->FCamAnimeData.cameraAreaOffFlag = TRUE;
    FCAM_ANIME_SetAnimeData( work->FCamAnimeWork, &work->FCamAnimeData );
    FCAM_ANIME_SetupCamera( work->FCamAnimeWork );
    FCAM_PARAM_GetCurrentParam( camera, &work->FCamAnimeData.endParam );
    work->FCamAnimeData.startParam = work->FCamAnimeData.endParam;
    work->FCamAnimeData.startParam.length -= (100 << FX32_SHIFT);
    FCAM_ANIME_SetCameraStartParam( work->FCamAnimeWork );
    FCAM_ANIME_StartAnime( work->FCamAnimeWork );
    (*seq)++;
    break;

  case 3:
    // ��ʃt�F�[�h�C���J�n
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeIn_White( gameSystem, fieldmap,  FIELD_FADE_NO_WAIT ) );
    (*seq)++;
    break;

  case 4:
    // �^�X�N�̏I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ) { (*seq)++; }
    break;

  case 5:
    FCAM_ANIME_RecoverCamera( work->FCamAnimeWork );
    FCAM_ANIME_DeleteWork( work->FCamAnimeWork );
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ���Ȃ��ق� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work       = (EVENT_WORK*)wk;
  GAMESYS_WORK*   gameSystem = work->gsys;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_CAMERA*   camera     = FIELDMAP_GetFieldCamera( fieldmap );
  FIELD_TASK_MAN* taskMan    = FIELDMAP_GetTaskManager( fieldmap ); 

  switch( *seq ) {
  case 0:
    // �J�����̃g���[�X������~���N�G�X�g���s
    FIELD_CAMERA_StopTraceRequest( camera );
    (*seq)++;
    break;
  case 1:
    // �J�����̃g���[�X�����I���҂�
    if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ) { (*seq)++; }
    break;

  case 2:
    // �^�X�N�̒ǉ�
    {
      FIELD_TASK* rot;
      rot = FIELD_TASK_PlayerRotate_SpeedDown( fieldmap, 60, 8 );
      FIELD_TASK_MAN_AddTask( taskMan, rot, NULL );
    }
    // �J�������o�J�n
    work->FCamAnimeWork = FCAM_ANIME_CreateWork( fieldmap );
    work->FCamAnimeData.frame = 60;
    work->FCamAnimeData.targetBindOffFlag = TRUE;
    work->FCamAnimeData.cameraAreaOffFlag = TRUE;
    FCAM_ANIME_SetAnimeData( work->FCamAnimeWork, &work->FCamAnimeData );
    FCAM_ANIME_SetupCamera( work->FCamAnimeWork );
    FCAM_PARAM_GetCurrentParam( camera, &work->FCamAnimeData.endParam );
    work->FCamAnimeData.startParam = work->FCamAnimeData.endParam;
    work->FCamAnimeData.startParam.length -= (100 << FX32_SHIFT);
    FCAM_ANIME_SetCameraStartParam( work->FCamAnimeWork );
    FCAM_ANIME_StartAnime( work->FCamAnimeWork );
    (*seq)++;
    break;

  case 3:
    // ��ʃt�F�[�h�C���J�n
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeIn_White( gameSystem, fieldmap,  FIELD_FADE_NO_WAIT ) );
    (*seq)++;
    break;

  case 4:
    // �^�X�N�̏I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ) { (*seq)++; }
    break;

  case 5:
    FCAM_ANIME_RecoverCamera( work->FCamAnimeWork );
    FCAM_ANIME_DeleteWork( work->FCamAnimeWork );
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( �e���|�[�g )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Teleport( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work       = (EVENT_WORK*)wk;
  GAMESYS_WORK*   gameSystem = work->gsys;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_CAMERA*   camera     = FIELDMAP_GetFieldCamera( fieldmap );
  FIELD_TASK_MAN* taskMan    = FIELDMAP_GetTaskManager( fieldmap ); 

  switch( *seq ) {
  case 0:
    // �J�����̃g���[�X������~���N�G�X�g���s
    FIELD_CAMERA_StopTraceRequest( camera );
    (*seq)++;
    break;
  case 1:
    // �J�����̃g���[�X�����I���҂�
    if( FIELD_CAMERA_CheckTrace( camera ) == FALSE ) { (*seq)++; }
    break;

  case 2:
    // �^�X�N�̒ǉ�
    { 
      FIELD_TASK* rot;
      rot  = FIELD_TASK_PlayerRotate_SpeedDown( fieldmap, 60, 8 );
      FIELD_TASK_MAN_AddTask( taskMan, rot, NULL );
    }
    // �J�������o�J�n
    work->FCamAnimeWork = FCAM_ANIME_CreateWork( fieldmap );
    work->FCamAnimeData.frame = 60;
    work->FCamAnimeData.targetBindOffFlag = TRUE;
    work->FCamAnimeData.cameraAreaOffFlag = TRUE;
    FCAM_ANIME_SetAnimeData( work->FCamAnimeWork, &work->FCamAnimeData );
    FCAM_ANIME_SetupCamera( work->FCamAnimeWork );
    FCAM_PARAM_GetCurrentParam( camera, &work->FCamAnimeData.endParam );
    work->FCamAnimeData.startParam = work->FCamAnimeData.endParam;
    work->FCamAnimeData.startParam.length -= (100 << FX32_SHIFT);
    FCAM_ANIME_SetCameraStartParam( work->FCamAnimeWork );
    FCAM_ANIME_StartAnime( work->FCamAnimeWork );
    (*seq)++;
    break;

  case 3:
    // ��ʃt�F�[�h�C���J�n
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeIn_Black( gameSystem, fieldmap,  FIELD_FADE_NO_WAIT ) );
    (*seq)++;
    break;

  case 4:
    // �^�X�N�̏I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ) { 
      // ���G�t�F�N�g�\��
      FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
      MMDL*         mmdl   = FIELD_PLAYER_GetMMdl( player );
      FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
      FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
      (*seq)++; 
    }
    break;

  case 5:
    FCAM_ANIME_RecoverCamera( work->FCamAnimeWork );
    FCAM_ANIME_DeleteWork( work->FCamAnimeWork );
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ���[�v )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Warp( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*    work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = FIELDMAP_GetFieldCamera( fieldmap );

  switch( *seq ) {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // ��l���̕`��I�t�Z�b�g�̏����ݒ�(��ʊO�ɂ���悤�ɐݒ�)
      VecFx32 moveOffset;
      FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
      MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
      VEC_Set( &moveOffset, 0, 10<<FX32_SHIFT, 0);
      MMDL_SetVectorDrawOffsetPos( mmdl, &moveOffset );
    }
    { // �t�F�[�h�C��
      GMEVENT* fadeInEvent;
      fadeInEvent = EVENT_FieldFadeIn_Black( work->gsys, fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fadeInEvent );
    }
    { // �^�X�N�̒ǉ�
      FIELD_TASK* rotTask;
      FIELD_TASK* moveTask;
      FIELD_TASK_MAN* taskMan;
      VecFx32 moveVec;
      VEC_Set( &moveVec, 0, 150<<FX32_SHIFT, 0 );
      rotTask = FIELD_TASK_PlayerRotate( fieldmap, 24, 3 );
      moveTask = FIELD_TASK_TransDrawOffset( fieldmap, -24, &moveVec );
      taskMan = FIELDMAP_GetTaskManager( fieldmap );
      FIELD_TASK_MAN_AddTask( taskMan, rotTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, moveTask, NULL );
    }
    (*seq)++;
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* taskMan;
      taskMan = FIELDMAP_GetTaskManager( fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ) { (*seq)++; }
    }
    break;
  case 2:
    // �������o��
    {
      FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
      MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
      FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
      FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
    }
    (*seq)++;
    break;
  case 3:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    (*seq)++;
    break;
  case 4: 
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ���j�I������ )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_UnionIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(work->fieldmap);
  MMDL *player_mmdl = FIELD_PLAYER_GetMMdl(player);

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { 
      MMDL_SetAcmd(player_mmdl, AC_WARP_DOWN);
    }
    { // �t�F�[�h�C��
      GMEVENT* fadeInEvent;
      fadeInEvent = EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fadeInEvent );
    }
    (*seq)++;
    break;
  case 1:
    if(MMDL_CheckEndAcmd(player_mmdl) == TRUE){
      MMDL_EndAcmd(player_mmdl);
      (*seq)++;
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    (*seq)++;
    break;
  case 3: 
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 

