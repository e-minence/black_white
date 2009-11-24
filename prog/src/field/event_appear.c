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
#include "fieldmap_tcb.h"
#include "sound/pm_sndsys.h"
#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_player_rot.h"
#include "field_task_player_fall.h"
#include "field_task_wait.h"
#include "field_task_fade.h"

#include "fldeff_kemuri.h"


//==========================================================================================
// ���萔
//========================================================================================== 
#define ZOOM_IN_DIST   (70 << FX32_SHIFT)   // �J�����̃Y�[���C������
#define ZOOM_IN_FRAME  (60)   // �Y�[���C���Ɋ|����t���[����
#define ZOOM_OUT_FRAME (60)   // �Y�[���A�E�g�Ɋ|����t���[����


//==========================================================================================
// ���C�x���g���[�N
//==========================================================================================
typedef struct
{ 
  FIELD_CAMERA_MODE cameraMode;   // �C�x���g�J�n���̃J�������[�h
  FIELDMAP_WORK*      fieldmap;   // ����t�B�[���h�}�b�v

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
    { // �^�X�N�̒ǉ�
      FIELD_TASK* fall;
      FIELD_TASK* fade_in;
      FIELD_TASK_MAN* man;
      fall    = FIELD_TASK_PlayerFall( work->fieldmap, 40, 250 );
      fade_in = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
      man     = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, fall, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_in, NULL );
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
    FIELD_CAMERA_SetAngleLen( camera, FIELD_CAMERA_GetAngleLen( camera ) - ZOOM_IN_DIST );
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̒ǉ�
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK* wait;
      FIELD_TASK* fade_in;
      FIELD_TASK_MAN* man;
      rot     = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 60, 8 );
      zoom    = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, ZOOM_IN_DIST );
      wait    = FIELD_TASK_Wait( work->fieldmap, 4 );
      fade_in = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 1 );
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot, NULL );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, wait, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_in, wait ); // �t�F�[�h�C����x�点�ăJ�����̃u�����B��
    }
    ++( *seq );
    break;
  case 2:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 3:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 4:
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
    FIELD_CAMERA_SetAngleLen( camera, FIELD_CAMERA_GetAngleLen( camera ) - ZOOM_IN_DIST );
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̒ǉ�
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK* wait;
      FIELD_TASK* fade_in;
      FIELD_TASK_MAN* man;
      rot     = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 60, 8 );
      zoom    = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, ZOOM_IN_DIST );
      wait    = FIELD_TASK_Wait( work->fieldmap, 4 );
      fade_in = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 1 );
      man     = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot, NULL );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, wait, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_in, wait ); // �t�F�[�h�C����x�点�ăJ�����̃u�����B��
    }
    ++( *seq );
    break;
  case 2:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 3:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 4:
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
    FIELD_CAMERA_SetAngleLen( camera, FIELD_CAMERA_GetAngleLen( camera ) - ZOOM_IN_DIST );
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̒ǉ�
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK* wait;
      FIELD_TASK* fade_in;
      FIELD_TASK_MAN* man;
      rot     = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 60, 8 );
      zoom    = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, ZOOM_IN_DIST );
      wait    = FIELD_TASK_Wait( work->fieldmap, 4 );
      fade_in = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1 );
      man     = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot, NULL );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, wait, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_in, wait ); // �t�F�[�h�C����x�点�ăJ�����̃u�����B��
    }
    ++( *seq );
    break;
  case 2:
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
  case 3:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
