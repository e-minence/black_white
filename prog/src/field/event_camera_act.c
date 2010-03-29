/////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   event_camera_act.c
 * @brief  �h�A�ɏo���肷��ۂ̃J��������C�x���g
 * @author obata
 * @date   2009.08.17
 */
/////////////////////////////////////////////////////////////////////////////////////
#include "event_camera_act.h"
#include "gamesystem/game_event.h"
#include "field/field_camera.h"
#include "field/fieldmap.h"
#include "fieldmap_tcb.h"
#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_wait.h"


//=====================================================================================
// ���萔
//=====================================================================================

// �J�����̐ݒ�
#define NEAR_PLANE (50 << FX32_SHIFT)
#define FAR_PLANE  (512 << FX32_SHIFT)

// ��h�A�ւ̏o����
#define U_DOOR_FRAME     (20)
#define U_DOOR_ZOOM_DIST (0x00b4 << FX32_SHIFT)

// ���h�A�ւ̏o����
#define L_DOOR_FRAME      (20)
#define L_DOOR_PITCH      (0x0d33)
#define L_DOOR_YAW        (0x0e0f)
#define L_DOOR_ZOOM_DIST  (0x00ec << FX32_SHIFT)

// �E�h�A�ւ̏o����
#define R_DOOR_FRAME     (20)
#define R_DOOR_PITCH     (0x25fc)
#define R_DOOR_YAW       (0xf2cc)
#define R_DOOR_ZOOM_DIST (0x00ec << FX32_SHIFT)



//=====================================================================================
// ���C�x���g ���[�N
//=====================================================================================
typedef struct {
  FIELDMAP_WORK* fieldmap; // ����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;    // �o�߃t���[����
} EVENT_WORK;


//=====================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=====================================================================================
static void SetFarNear( FIELDMAP_WORK* fieldmap );

// �C�x���g�����֐�
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );

// �C�x���g�����֐�
static GMEVENT_RESULT EVENT_FUNC_UpDoorIn( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_LeftDoorIn( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_UpDoorOut( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_LeftDoorOut( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut( GMEVENT* event, int* seq, void* wk );


//=====================================================================================
// ���O�� ���J�֐�
//=====================================================================================

//-------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo�� ( IN )
 *
 * @param parent     �e�C�x���g
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 */
//-------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_CallDoorInEvent( 
    GMEVENT* parent, GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL*         mmdl   = FIELD_PLAYER_GetMMdl( player );
  u16           dir    = MMDL_GetDirDisp( mmdl );
  GMEVENT*      event  = NULL;

  // ��l���̌����ɉ������C�x���g��I��
  switch( dir ) {
  case DIR_UP:    event = EVENT_UpDoorIn( gameSystem, fieldmap ); break; 
  case DIR_LEFT:  event = EVENT_LeftDoorIn( gameSystem, fieldmap ); break; 
  case DIR_RIGHT: event = EVENT_RightDoorIn( gameSystem, fieldmap ); break; 
  default: GF_ASSERT(0); break;
  }

  if( event ) {
    GMEVENT_CallEvent( parent, event );
  }
}

//-------------------------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//-------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_PrepareForDoorOut( FIELDMAP_WORK* fieldmap )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL*         mmdl   = FIELD_PLAYER_GetMMdl( player );
  u16           dir    = MMDL_GetDirDisp( mmdl );

  FIELD_CAMERA_MODE cam_mode;

  // �J�������[�h��ύX
  cam_mode = FIELD_CAMERA_GetMode( camera );
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  // ��l���̌����ɉ������J�����ݒ�
  switch( dir ) {
  case DIR_DOWN: 
    FIELD_CAMERA_SetAngleLen( camera, U_DOOR_ZOOM_DIST );  
    break; 
  case DIR_LEFT: 
    FIELD_CAMERA_SetAnglePitch( camera, R_DOOR_PITCH );
    FIELD_CAMERA_SetAngleYaw( camera, R_DOOR_YAW );
    FIELD_CAMERA_SetAngleLen( camera, R_DOOR_ZOOM_DIST );
    //SetFarNear( fieldmap );
    break; 
  case DIR_RIGHT: 
    FIELD_CAMERA_SetAnglePitch( camera, L_DOOR_PITCH );
    FIELD_CAMERA_SetAngleYaw( camera, L_DOOR_YAW );
    FIELD_CAMERA_SetAngleLen( camera, L_DOOR_ZOOM_DIST );
    //SetFarNear( fieldmap );
    break;
  }
  
  // �J�������[�h�̕��A
  FIELD_CAMERA_ChangeMode( camera, cam_mode );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo�� ( OUT )
 *
 * @param parent     �e�C�x���g
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 */
//-------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_CallDoorOutEvent( 
    GMEVENT* parent, GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL*         mmdl   = FIELD_PLAYER_GetMMdl( player );
  u16           dir    = MMDL_GetDirDisp( mmdl );
  GMEVENT*      event  = NULL;

  // ��l���̌����ɉ������C�x���g��I��
  switch( dir ) {
  case DIR_DOWN:  event = EVENT_UpDoorOut( gameSystem, fieldmap ); break;
  case DIR_LEFT:  event = EVENT_RightDoorOut( gameSystem, fieldmap ); break;
  case DIR_RIGHT: event = EVENT_LeftDoorOut( gameSystem, fieldmap ); break;
  default: break;
  }

  if( event ) {
    GMEVENT_CallEvent( parent, event );
  }
}

//-------------------------------------------------------------------------------------
/**
 * @breif �J������Near/Far�v���[�����f�t�H���g�ݒ�ɖ߂�
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//-------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_ResetCameraParameter( FIELDMAP_WORK* fieldmap )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  FLD_CAMERA_PARAM defParam;

  // �f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( camera, &defParam );

  // �f�t�H���g�l�ɖ߂�
  FIELD_CAMERA_SetFar( camera, defParam.Far );
  FIELD_CAMERA_SetNear( camera, defParam.Near );
}


//=====================================================================================
// ������J�֐�
//=====================================================================================

//-------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g�𐶐����� ( ��ɂ���h�A�ɓ������� )
 *
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return ���������C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*    event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_UpDoorIn, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g�𐶐����� ( ���ɂ���h�A�ɓ������� )
 *
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return ���������C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*    event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_LeftDoorIn, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g�𐶐����� ( �E�ɂ���h�A�ɓ������� )
 *
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return ���������C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*      event;
  EVENT_WORK*   work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_RightDoorIn, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g�𐶐����� ( ��ɂ���h�A����o�Ă����Ƃ� )
 *
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return ���������C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*    event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_UpDoorOut, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g�𐶐����� ( ���ɂ���h�A����o�Ă����� )
 *
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*    event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_LeftDoorOut, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}


//-------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g�𐶐�����( �E�ɂ���h�A����o�Ă����� )
 *
 * @param gameSystem �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return ���������C�x���g
 */
//-------------------------------------------------------------------------------------
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*    event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, EVENT_FUNC_RightDoorOut, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �J������far/near�v���[����ݒ肷��
 *
 * @param fieldmap
 */
//-------------------------------------------------------------------------------------
static void SetFarNear( FIELDMAP_WORK* fieldmap )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  fx32             far = FIELD_CAMERA_GetFar( camera ); 
  fx32            near = FIELD_CAMERA_GetNear( camera ); 

  // Far�v���[���𔼕��ɂ���
  far = FAR_PLANE;
  FIELD_CAMERA_SetFar( camera, far );

  // Near�v���[����ݒ肷��
  near = NEAR_PLANE;
  FIELD_CAMERA_SetNear( camera, near );
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐� ( ��o�����ւ̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk    �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_UpDoorIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    // �^�X�N�o�^
    { 
      FIELD_TASK* zoom;
      zoom = FIELD_TASK_CameraLinearZoom( fieldmap, U_DOOR_FRAME, U_DOOR_ZOOM_DIST );
      FIELD_TASK_MAN_AddTask( taskMan, zoom, NULL );
    }
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���o�����ւ̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk    �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_LeftDoorIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    // �J���������ݒ�
    //SetFarNear( fieldmap );
    // �^�X�N�o�^
    {
      FIELD_TASK* zoom;
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      //zoom  = FIELD_TASK_CameraLinearZoom( fieldmap, L_DOOR_FRAME, L_DOOR_ZOOM_DIST );
      //pitch = FIELD_TASK_CameraRot_Pitch( fieldmap, L_DOOR_FRAME, L_DOOR_PITCH );
      yaw   = FIELD_TASK_CameraRot_Yaw( fieldmap, L_DOOR_FRAME, L_DOOR_YAW );
      //FIELD_TASK_MAN_AddTask( taskMan, zoom, NULL );
      //FIELD_TASK_MAN_AddTask( taskMan, pitch, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, yaw, NULL );
    }
    (*seq)++;
    break;
  case 1:
    // �^�X�N�I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { (*seq)++; }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( �E�o�����ւ̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    // �^�X�N�o�^
    { 
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK* wait1;
      FIELD_TASK* zoom;
      FIELD_TASK* wait2;
      //pitch = FIELD_TASK_CameraRot_Pitch( fieldmap, R_DOOR_FRAME, R_DOOR_PITCH );
      yaw   = FIELD_TASK_CameraRot_Yaw( fieldmap, R_DOOR_FRAME, R_DOOR_YAW );
      //wait1 = FIELD_TASK_Wait( fieldmap, 10 );
      //zoom  = FIELD_TASK_CameraLinearZoom( fieldmap, R_DOOR_FRAME, R_DOOR_ZOOM_DIST );
      //wait2 = FIELD_TASK_Wait( fieldmap, 20 );
      //FIELD_TASK_MAN_AddTask( taskMan, pitch, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, yaw, NULL );
      //FIELD_TASK_MAN_AddTask( taskMan, wait1, NULL );
      //FIELD_TASK_MAN_AddTask( taskMan, zoom, wait1 ); 
      //FIELD_TASK_MAN_AddTask( taskMan, wait2, wait1 ); 
    }
    (*seq)++;
    break;
  case 1:
    // �^�X�N�I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { (*seq)++; }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ��o��������̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk    �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_UpDoorOut( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_CAMERA*   camera   = FIELDMAP_GetFieldCamera( work->fieldmap ); 
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    // �^�X�N�o�^
    { 
      FLD_CAMERA_PARAM def_param; 
      fx32 dist;
      FIELD_TASK* zoom;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      dist = def_param.Distance << FX32_SHIFT;
      zoom = FIELD_TASK_CameraLinearZoom( fieldmap, U_DOOR_FRAME, dist );
      FIELD_TASK_MAN_AddTask( taskMan, zoom, NULL ); 
    }
    (*seq)++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���o��������̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_LeftDoorOut( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_CAMERA*   camera   = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    // ������
    //SetFarNear( fieldmap );
    // �^�X�N�o�^
    { 
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      u16 val_pitch, val_yaw;
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK* zoom;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      val_dist  = def_param.Distance << FX32_SHIFT;
      val_pitch = def_param.Angle.x;
      val_yaw   = def_param.Angle.y;
      pitch = FIELD_TASK_CameraRot_Pitch( fieldmap, L_DOOR_FRAME, val_pitch );
      yaw   = FIELD_TASK_CameraRot_Yaw( fieldmap, L_DOOR_FRAME, val_yaw );
      zoom  = FIELD_TASK_CameraLinearZoom( fieldmap, L_DOOR_FRAME, val_dist );
      FIELD_TASK_MAN_AddTask( taskMan, pitch, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, yaw, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, zoom, NULL ); 
    }
    (*seq)++;
    break;
  case 1:
    // �^�X�N�I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { (*seq)++; }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//-------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( �E�o����OUT )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_CAMERA*   camera   = FIELDMAP_GetFieldCamera( work->fieldmap ); 
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    //SetFarNear( fieldmap );
    // �^�X�N�o�^
    {
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      u16 val_yaw, val_pitch;
      FIELD_TASK* zoom;
      FIELD_TASK* wait1;
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK* wait2;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      val_dist  = def_param.Distance << FX32_SHIFT;
      val_pitch = def_param.Angle.x;
      val_yaw   = def_param.Angle.y;
      zoom  = FIELD_TASK_CameraLinearZoom( fieldmap, R_DOOR_FRAME, val_dist );
      pitch = FIELD_TASK_CameraRot_Pitch( fieldmap, R_DOOR_FRAME, val_pitch );
      yaw   = FIELD_TASK_CameraRot_Yaw( fieldmap, R_DOOR_FRAME, val_yaw );
      wait1 = FIELD_TASK_Wait( fieldmap, 10 );
      wait2 = FIELD_TASK_Wait( fieldmap, 20 );
      FIELD_TASK_MAN_AddTask( taskMan, zoom, NULL ); 
      FIELD_TASK_MAN_AddTask( taskMan, wait1, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, pitch, wait1 );
      FIELD_TASK_MAN_AddTask( taskMan, yaw, wait1 );
      FIELD_TASK_MAN_AddTask( taskMan, wait2, NULL ); 
    }
    (*seq)++;
    break;
  case 1:
    // �^�X�N�I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { (*seq)++; }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
