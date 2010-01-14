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
typedef struct
{ 
  FIELD_CAMERA_MODE cameraMode;   // �C�x���g�J�n���̃J�������[�h
  FIELDMAP_WORK*      fieldmap;   // ����t�B�[���h�}�b�v
  GAMESYS_WORK*           gsys;   // �Q�[���V�X�e��

} EVENT_WORK;


//==========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================== 
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Teleport( GMEVENT* event, int* seq, void* wk );


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
GMEVENT* EVENT_APPEAR_Teleport( GMEVENT* parent, 
                                GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
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
      VecFx32       offset = {0, 100<<FX32_SHIFT, 0};
      FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
      MMDL_SetVectorDrawOffsetPos( mmdl, &offset );
    }
    { // �t�F�[�h�C��
      GMEVENT* fadeInEvent;
      fadeInEvent = EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fadeInEvent );
    }
    { // �^�X�N�̒ǉ�
      FIELD_TASK* fall;
      FIELD_TASK_MAN* man;
      fall = FIELD_TASK_PlayerFall( work->fieldmap, 40, 250 );
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, fall, NULL );
    }
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
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
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    // �J���������ݒ�
    FIELD_CAMERA_SetAngleLen( camera, ZOOM_IN_DIST );
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_White( work->gsys, work->fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fade_event );
    }
    { // �^�X�N�̒ǉ�
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      FIELD_TASK* rot1;
      FIELD_TASK* rot2;
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      u32 fadein_frame;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      fadein_frame = GetSeasonDispEventFrame( work->gsys );  // �G�ߕ\���ɂ����鎞�Ԃ��擾
      val_dist = def_param.Distance << FX32_SHIFT;
      rot1 = FIELD_TASK_PlayerRotate( work->fieldmap, fadein_frame, fadein_frame/5 );
      rot2 = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 60, 8 );
      zoom = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, val_dist );
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot1, NULL );
      FIELD_TASK_MAN_AddTask( man, rot2, rot1 );
      FIELD_TASK_MAN_AddTask( man, zoom, rot1 );
    }
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 3:
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
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    // �J���������ݒ�
    FIELD_CAMERA_SetAngleLen( camera, ZOOM_IN_DIST );
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fade_event );
    }
    { // �^�X�N�̒ǉ�
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      FIELD_TASK* rot1;
      FIELD_TASK* rot2;
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      u32 fadein_frame;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      fadein_frame = GetSeasonDispEventFrame( work->gsys );  // �G�ߕ\���ɂ����鎞�Ԃ��擾
      val_dist = def_param.Distance << FX32_SHIFT;
      rot1    = FIELD_TASK_PlayerRotate( work->fieldmap, fadein_frame, fadein_frame/5 );
      rot2    = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 60, 8 );
      zoom    = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, val_dist );
      man     = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot1, NULL );
      FIELD_TASK_MAN_AddTask( man, rot2, rot1 );
      FIELD_TASK_MAN_AddTask( man, zoom, rot1 );
      // @todo SE
    }
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 3:
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
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    // �J���������ݒ�
    FIELD_CAMERA_SetAngleLen( camera, ZOOM_IN_DIST );
    { // �t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap,  FIELD_FADE_NO_WAIT );
      GMEVENT_CallEvent( event, fade_event );
    }
    { // �^�X�N�̒ǉ�
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      FIELD_TASK* rot1;
      FIELD_TASK* rot2;
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      u32 fadein_frame;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      fadein_frame = GetSeasonDispEventFrame( work->gsys );  // �G�ߕ\���ɂ����鎞�Ԃ��擾
      val_dist = def_param.Distance << FX32_SHIFT;
      rot1    = FIELD_TASK_PlayerRotate( work->fieldmap, fadein_frame, fadein_frame/5 );
      rot2    = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 60, 8 );
      zoom    = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, val_dist );
      man     = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot1, NULL );
      FIELD_TASK_MAN_AddTask( man, rot2, rot1 );
      FIELD_TASK_MAN_AddTask( man, zoom, rot1 );
    }
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        { // ���G�t�F�N�g�\��
          FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
          MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player );
          FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
          FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
        }
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
