///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   event_camera_act.c
 * @brief  �h�A�ɏo���肷��ۂ̃J��������C�x���g
 * @author obata
 * @date   2009.08.17
 */
///////////////////////////////////////////////////////////////////////////////////////////////
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


//=============================================================================================
/**
 * @brief �萔
 */
//=============================================================================================

// �J�����̐ݒ�
#define NEAR_PLANE (50 << FX32_SHIFT)
#define FAR_PLANE  (512 << FX32_SHIFT)

// ��h�A�ւ̏o����
#define U_DOOR_FRAME     (6)
#define U_DOOR_ZOOM_DIST (0x00b4 << FX32_SHIFT)

// ���h�A�ւ̏o����
#define L_DOOR_FRAME      (10)
#define L_DOOR_PITCH      (0x0e38)
#define L_DOOR_YAW        (0x3fff)
#define L_DOOR_ZOOM_DIST  (120 << FX32_SHIFT)

// �E�h�A�ւ̏o����
#define R_DOOR_FRAME     (10)
#define R_DOOR_PITCH     (0x0e38)
#define R_DOOR_YAW       (0xbfff)
#define R_DOOR_ZOOM_DIST (120 << FX32_SHIFT)



//=============================================================================================
/**
 * @brief �C�x���g�E���[�N
 */
//=============================================================================================
typedef struct
{
  FIELDMAP_WORK* fieldmap;   // ����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;       // �o�߃t���[����
}
EVENT_WORK;


//=============================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//=============================================================================================
static void SetFarNear( FIELDMAP_WORK* fieldmap );

// �C�x���g�����֐�
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap );
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap );

// �C�x���g�����֐�
static GMEVENT_RESULT EVENT_FUNC_UpDoorIn( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_LeftDoorIn( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_UpDoorOut( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_LeftDoorOut( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut( GMEVENT* event, int* seq, void* wk );


//=============================================================================================
/**
 * @brief ���J�֐��̒�`
 */
//=============================================================================================

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo��( �������� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_CallDoorInEvent( GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );

  // ��l���̌����ɉ������C�x���g���Ăяo��
  switch( dir )
  {
  case DIR_UP:
    GMEVENT_CallEvent( p_parent, EVENT_UpDoorIn(p_gsys, fieldmap) );
    break;
  case DIR_LEFT:
    GMEVENT_CallEvent( p_parent, EVENT_LeftDoorIn(p_gsys, fieldmap) );
    break;
  case DIR_RIGHT:
    GMEVENT_CallEvent( p_parent, EVENT_RightDoorIn(p_gsys, fieldmap) );
    break;
  default:
    break;
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_PrepareForDoorOut( FIELDMAP_WORK* fieldmap )
{
  FIELD_PLAYER*    player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL*            fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16              dir    = MMDL_GetDirDisp( fmmdl );
  FIELD_CAMERA*    cam    = FIELDMAP_GetFieldCamera( fieldmap );

  FIELD_CAMERA_MODE cam_mode = FIELD_CAMERA_GetMode( cam );
  FIELD_CAMERA_ChangeMode( cam, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  // ��l���̌����ɉ������J�����ݒ�
  switch( dir )
  {
  case DIR_DOWN:
    FIELD_CAMERA_SetAngleLen( cam, U_DOOR_ZOOM_DIST);
    break;
  case DIR_LEFT:
    FIELD_CAMERA_SetAnglePitch( cam, R_DOOR_PITCH );
    FIELD_CAMERA_SetAngleYaw( cam, R_DOOR_YAW );
    FIELD_CAMERA_SetAngleLen( cam, R_DOOR_ZOOM_DIST );
    SetFarNear( fieldmap );
    break;
  case DIR_RIGHT:
    FIELD_CAMERA_SetAnglePitch( cam, L_DOOR_PITCH );
    FIELD_CAMERA_SetAngleYaw( cam, L_DOOR_YAW );
    FIELD_CAMERA_SetAngleLen( cam, L_DOOR_ZOOM_DIST );
    SetFarNear( fieldmap );
    break;
  default:
    break;
  }
  
  FIELD_CAMERA_ChangeMode( cam, cam_mode );
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo��( �o���� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_CallDoorOutEvent( GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );

  // ��l���̌����ɉ������C�x���g���Ăяo��
  switch( dir )
  {
  case DIR_DOWN:
    GMEVENT_CallEvent( p_parent, EVENT_UpDoorOut(p_gsys, fieldmap) );
    break;
  case DIR_LEFT:
    GMEVENT_CallEvent( p_parent, EVENT_RightDoorOut(p_gsys, fieldmap) );
    break;
  case DIR_RIGHT:
    GMEVENT_CallEvent( p_parent, EVENT_LeftDoorOut(p_gsys, fieldmap) );
    break;
  default:
    break;
  }
}

//--------------------------------------------------------------------
/**
 * @breif �J������Near�v���[����Far�v���[�����f�t�H���g�ݒ�ɖ߂�
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//--------------------------------------------------------------------
void EVENT_CAMERA_ACT_ResetCameraParameter( FIELDMAP_WORK* fieldmap )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( camera, &def_param );

  // �f�t�H���g�l�ɖ߂�
  FIELD_CAMERA_SetFar( camera, def_param.Far );
  FIELD_CAMERA_SetNear( camera, def_param.Near );

  // DEBUG:
  /*
  OBATA_Printf( "reset far  = %d\n", def_param.Far >> FX32_SHIFT );
  OBATA_Printf( "reset near = %d\n", def_param.Near >> FX32_SHIFT );
  */
}


//=============================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//=============================================================================================

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g( ��ɂ���h�A�ɓ������� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*      event;
  EVENT_WORK*   work;

  // �C�x���g����
  event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_UpDoorIn, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work            = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( ���ɂ���h�A�ɓ������� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*      event;
  EVENT_WORK*   work;

  // �C�x���g����
  event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_LeftDoorIn, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work            = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( �E�ɂ���h�A�ɓ������� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*      event;
  EVENT_WORK*   work;

  // �C�x���g����
  event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorIn, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame    = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g( ��ɂ���h�A����o�Ă����Ƃ� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*      event;
  EVENT_WORK*   work;

  // �C�x���g����
  event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_UpDoorOut, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work            = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( ���ɂ���h�A����o�Ă����� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*         event;
  EVENT_WORK*      work;

  // �C�x���g����
  event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_LeftDoorOut, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work            = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return event;
}


//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( �E�ɂ���h�A����o�Ă����� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*      event;
  EVENT_WORK*   work;

  // �C�x���g����
  event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorOut, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work            = GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame     = 0;
  work = GMEVENT_GetEventWork( event );

  // ���������C�x���g��Ԃ�
  return event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������far�v���[���Enear�v���[����ݒ肷��
 *
 * @param fieldmap �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
static void SetFarNear( FIELDMAP_WORK* fieldmap )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );
  fx32             far = FIELD_CAMERA_GetFar( camera ); 
  fx32            near = FIELD_CAMERA_GetNear( camera ); 

  // DEBUG:
  /*
  OBATA_Printf( "Set FarPlane and NearPlane\n" );
  OBATA_Printf( "before far  = %x(%d)\n", far, far>>FX32_SHIFT );
  OBATA_Printf( "before near = %x(%d)\n", near, near>>FX32_SHIFT );
  */

  // Far�v���[���𔼕��ɂ���
  far = FAR_PLANE;
  FIELD_CAMERA_SetFar( camera, far );

  // Near�v���[����ݒ肷��
  near = NEAR_PLANE;
  FIELD_CAMERA_SetNear( camera, near );

  // DEBUG:
  /*
  OBATA_Printf( "after far  = %x(%d)\n", far, far>>FX32_SHIFT );
  OBATA_Printf( "after near = %x(%d)\n", near, near>>FX32_SHIFT );
  */
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ��o�����ւ̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_UpDoorIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  case 0:
    { // �^�X�N�o�^
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      zoom = FIELD_TASK_CameraLinearZoom( work->fieldmap, U_DOOR_FRAME, U_DOOR_ZOOM_DIST );
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
    }
    ++( *seq );
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���o�����ւ̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_LeftDoorIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  case 0:
    SetFarNear( work->fieldmap );
    { // �^�X�N�o�^
      FIELD_TASK* zoom;
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK_MAN* man;
      zoom  = FIELD_TASK_CameraLinearZoom( work->fieldmap, L_DOOR_FRAME, L_DOOR_ZOOM_DIST );
      pitch = FIELD_TASK_CameraRot_Pitch( work->fieldmap, L_DOOR_FRAME, L_DOOR_PITCH );
      yaw   = FIELD_TASK_CameraRot_Yaw( work->fieldmap, L_DOOR_FRAME, L_DOOR_YAW );
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, pitch, NULL );
      FIELD_TASK_MAN_AddTask( man, yaw, NULL );
    }
    ++( *seq );
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( �E�o�����ւ̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  case 0:
    SetFarNear( work->fieldmap );
    { // �^�X�N�o�^
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK* wait1;
      FIELD_TASK* zoom;
      FIELD_TASK* wait2;
      FIELD_TASK_MAN* man;
      pitch = FIELD_TASK_CameraRot_Pitch( work->fieldmap, R_DOOR_FRAME, R_DOOR_PITCH );
      yaw   = FIELD_TASK_CameraRot_Yaw( work->fieldmap, R_DOOR_FRAME, R_DOOR_YAW );
      wait1 = FIELD_TASK_Wait( work->fieldmap, 10 );
      zoom  = FIELD_TASK_CameraLinearZoom( work->fieldmap, R_DOOR_FRAME, R_DOOR_ZOOM_DIST );
      wait2 = FIELD_TASK_Wait( work->fieldmap, 20 );
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, pitch, NULL );
      FIELD_TASK_MAN_AddTask( man, yaw, NULL );
      FIELD_TASK_MAN_AddTask( man, wait1, NULL );
      FIELD_TASK_MAN_AddTask( man, zoom, wait1 ); 
      FIELD_TASK_MAN_AddTask( man, wait2, wait1 ); 
    }
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ��o��������̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_UpDoorOut( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap ); 

  switch( *seq )
  {
  case 0:
    { // �^�X�N�o�^
      FLD_CAMERA_PARAM def_param; 
      fx32 dist;
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      dist = def_param.Distance << FX32_SHIFT;
      zoom = FIELD_TASK_CameraLinearZoom( work->fieldmap, U_DOOR_FRAME, dist );
      man  = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL ); 
    }
    ++( *seq );
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���o��������̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_LeftDoorOut( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    { // ������
      SetFarNear( work->fieldmap );
    }
    { // �^�X�N�o�^
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      u16 val_pitch, val_yaw;
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      FIELD_CAMERA_GetInitialParameter( camera, &def_param );
      val_dist  = def_param.Distance << FX32_SHIFT;
      val_pitch = def_param.Angle.x;
      val_yaw   = def_param.Angle.y;
      pitch = FIELD_TASK_CameraRot_Pitch( work->fieldmap, L_DOOR_FRAME, val_pitch );
      yaw   = FIELD_TASK_CameraRot_Yaw( work->fieldmap, L_DOOR_FRAME, val_yaw );
      zoom  = FIELD_TASK_CameraLinearZoom( work->fieldmap, L_DOOR_FRAME, val_dist );
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, pitch, NULL );
      FIELD_TASK_MAN_AddTask( man, yaw, NULL );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL ); 
    }
    ++( *seq );
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( �E�o��������̐i�� )
 *
 * @param event �C�x���g
 * @param seq   �V�[�P���X�ԍ�
 * @param wk  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;
  FIELD_CAMERA*        cam = FIELDMAP_GetFieldCamera( work->fieldmap ); 

  switch( *seq )
  {
  case 0:
    SetFarNear( work->fieldmap );
    { // �^�X�N�o�^
      FLD_CAMERA_PARAM def_param; 
      fx32 val_dist;
      u16 val_yaw, val_pitch;
      FIELD_TASK* zoom;
      FIELD_TASK* wait1;
      FIELD_TASK* pitch;
      FIELD_TASK* yaw;
      FIELD_TASK* wait2;
      FIELD_TASK_MAN* man;
      FIELD_CAMERA_GetInitialParameter( cam, &def_param );
      val_dist  = def_param.Distance << FX32_SHIFT;
      val_pitch = def_param.Angle.x;
      val_yaw   = def_param.Angle.y;
      zoom  = FIELD_TASK_CameraLinearZoom( work->fieldmap, R_DOOR_FRAME, val_dist );
      pitch = FIELD_TASK_CameraRot_Pitch( work->fieldmap, R_DOOR_FRAME, val_pitch );
      yaw   = FIELD_TASK_CameraRot_Yaw( work->fieldmap, R_DOOR_FRAME, val_yaw );
      wait1 = FIELD_TASK_Wait( work->fieldmap, 10 );
      wait2 = FIELD_TASK_Wait( work->fieldmap, 20 );
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL ); 
      FIELD_TASK_MAN_AddTask( man, wait1, NULL );
      FIELD_TASK_MAN_AddTask( man, pitch, wait1 );
      FIELD_TASK_MAN_AddTask( man, yaw, wait1 );
      FIELD_TASK_MAN_AddTask( man, wait2, NULL ); 
    }
    ++( *seq );
    break;
#if 0
  case 1:
    { // �^�X�N�I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
#endif
  case 1:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
