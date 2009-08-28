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
#define U_DOOR_ZOOM_DIST (15 << FX32_SHIFT)   // �Y�[�����鋗��

// ���h�A�ւ̏o����
#define L_DOOR_FRAME (10)
#define L_DOOR_PITCH (0xffff * 20 / 360)
#define L_DOOR_YAW   (0xffff * 90 / 360)
#define L_DOOR_ZOOM_DIST  (120 << FX32_SHIFT)

// �E�h�A�ւ̏o����
#define R_DOOR_FRAME (10)
#define R_DOOR_PITCH (0xffff * 20 / 360)
#define R_DOOR_YAW   (0xffff * 270 / 360)
#define R_DOOR_ZOOM_DIST  (120 << FX32_SHIFT)



//=============================================================================================
/**
 * @brief �C�x���g�E���[�N
 */
//=============================================================================================
typedef struct
{
  FIELDMAP_WORK* pFieldmap;   // ����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;       // �o�߃t���[����
}
EVENT_WORK;


//=============================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//=============================================================================================
static void SetFarNear( FIELDMAP_WORK* p_fieldmap );

// �C�x���g�����֐�
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );

// �C�x���g�����֐�
static GMEVENT_RESULT EVENT_FUNC_UpDoorIn( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_LeftDoorIn( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn_2Step( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn_1Frame( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_UpDoorOut( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_LeftDoorOut( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut_2Step( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut_1Frame( GMEVENT* p_event, int* p_seq, void* p_work );


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
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_CallDoorInEvent( GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );

  // ��l���̌����ɉ������C�x���g���Ăяo��
  switch( dir )
  {
  case DIR_UP:
    GMEVENT_CallEvent( p_parent, EVENT_UpDoorIn(p_gsys, p_fieldmap) );
    break;
  case DIR_LEFT:
    GMEVENT_CallEvent( p_parent, EVENT_LeftDoorIn(p_gsys, p_fieldmap) );
    break;
  case DIR_RIGHT:
    GMEVENT_CallEvent( p_parent, EVENT_RightDoorIn(p_gsys, p_fieldmap) );
    break;
  default:
    break;
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_PrepareForDoorOut( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER*    player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL*            fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16              dir    = MMDL_GetDirDisp( fmmdl );
  FIELD_CAMERA*    cam    = FIELDMAP_GetFieldCamera( p_fieldmap );

  // ��l���̌����ɉ������J�����ݒ�
  switch( dir )
  {
  case DIR_DOWN:
    FIELD_CAMERA_SetAngleLen( cam, FIELD_CAMERA_GetAngleLen( cam ) - U_DOOR_ZOOM_DIST);
    break;
  case DIR_LEFT:
    FIELD_CAMERA_SetAnglePitch( cam, R_DOOR_PITCH );
    FIELD_CAMERA_SetAngleYaw( cam, R_DOOR_YAW );
    FIELD_CAMERA_SetAngleLen( cam, FIELD_CAMERA_GetAngleLen( cam ) - R_DOOR_ZOOM_DIST );
    SetFarNear( p_fieldmap );
    break;
  case DIR_RIGHT:
    FIELD_CAMERA_SetAnglePitch( cam, L_DOOR_PITCH );
    FIELD_CAMERA_SetAngleYaw( cam, L_DOOR_YAW );
    FIELD_CAMERA_SetAngleLen( cam, FIELD_CAMERA_GetAngleLen( cam ) - L_DOOR_ZOOM_DIST );
    SetFarNear( p_fieldmap );
    break;
  default:
    break;
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g���Ăяo��( �o���� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ACT_CallDoorOutEvent( GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );

  // ��l���̌����ɉ������C�x���g���Ăяo��
  switch( dir )
  {
  case DIR_DOWN:
    GMEVENT_CallEvent( p_parent, EVENT_UpDoorOut(p_gsys, p_fieldmap) );
    break;
  case DIR_LEFT:
    GMEVENT_CallEvent( p_parent, EVENT_RightDoorOut(p_gsys, p_fieldmap) );
    break;
  case DIR_RIGHT:
    GMEVENT_CallEvent( p_parent, EVENT_LeftDoorOut(p_gsys, p_fieldmap) );
    break;
  default:
    break;
  }
}

//--------------------------------------------------------------------
/**
 * @breif �J������Near�v���[����Far�v���[�����f�t�H���g�ݒ�ɖ߂�
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//--------------------------------------------------------------------
void EVENT_CAMERA_ACT_ResetCameraParameter( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_CAMERA*    p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �f�t�H���g�l�ɖ߂�
  FIELD_CAMERA_SetFar( p_camera, def_param.Far );
  FIELD_CAMERA_SetNear( p_camera, def_param.Near );

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
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_UpDoorIn, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work            = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return p_event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( ���ɂ���h�A�ɓ������� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_LeftDoorIn, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work            = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return p_event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( �E�ɂ���h�A�ɓ������� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;

  // �C�x���g����
  //p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorIn, sizeof( EVENT_WORK ) );
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorIn_2Step, sizeof( EVENT_WORK ) );
  //p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorIn_1Frame, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work            = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return p_event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J��������C�x���g( ��ɂ���h�A����o�Ă����Ƃ� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_UpDoorOut, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work            = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return p_event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( ���ɂ���h�A����o�Ă����� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*         p_event;
  EVENT_WORK*      p_work;

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_LeftDoorOut, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work            = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame     = 0;

  // ���������C�x���g��Ԃ�
  return p_event;
}


//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g( �E�ɂ���h�A����o�Ă����� )
 *
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;

  // �C�x���g����
  //p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorOut, sizeof( EVENT_WORK ) );
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_FUNC_RightDoorOut_2Step, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work            = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame     = 0;
  p_work = GMEVENT_GetEventWork( p_event );

  // ���������C�x���g��Ԃ�
  return p_event;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������far�v���[���Enear�v���[����ݒ肷��
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
static void SetFarNear( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  fx32          far      = FIELD_CAMERA_GetFar( p_camera ); 
  fx32          near     = FIELD_CAMERA_GetNear( p_camera ); 

  // DEBUG:
  /*
  OBATA_Printf( "Set FarPlane and NearPlane\n" );
  OBATA_Printf( "before far  = %x(%d)\n", far, far>>FX32_SHIFT );
  OBATA_Printf( "before near = %x(%d)\n", near, near>>FX32_SHIFT );
  */

  // Far�v���[���𔼕��ɂ���
  far = far * 2 / 3;
  FIELD_CAMERA_SetFar( p_camera, far );

  // Near�v���[����ݒ肷��
  near = NEAR_PLANE;
  FIELD_CAMERA_SetNear( p_camera, near );

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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_UpDoorIn( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  switch( *p_seq )
  {
  case 0:
    FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, U_DOOR_FRAME, -U_DOOR_ZOOM_DIST );
    ++( *p_seq );
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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_LeftDoorIn( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  switch( *p_seq )
  {
  case 0:
    SetFarNear( p_event_work->pFieldmap );
    FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, L_DOOR_FRAME, -L_DOOR_ZOOM_DIST );
    FIELDMAP_TCB_AddTask_CameraRotate_Yaw( p_event_work->pFieldmap, L_DOOR_FRAME, L_DOOR_YAW );
    FIELDMAP_TCB_AddTask_CameraRotate_Pitch( p_event_work->pFieldmap, L_DOOR_FRAME, L_DOOR_PITCH );
    ++( *p_seq );
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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  switch( *p_seq )
  {
  case 0:
    SetFarNear( p_event_work->pFieldmap );
    FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, R_DOOR_FRAME, -R_DOOR_ZOOM_DIST );
    FIELDMAP_TCB_AddTask_CameraRotate_Yaw( p_event_work->pFieldmap, R_DOOR_FRAME, R_DOOR_YAW );
    FIELDMAP_TCB_AddTask_CameraRotate_Pitch( p_event_work->pFieldmap, R_DOOR_FRAME, R_DOOR_PITCH );
    ++( *p_seq );
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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn_2Step( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  switch( *p_seq )
  {
  case 0:
    SetFarNear( p_event_work->pFieldmap );
    FIELDMAP_TCB_AddTask_CameraRotate_Yaw( p_event_work->pFieldmap, 20, R_DOOR_YAW );
    FIELDMAP_TCB_AddTask_CameraRotate_Pitch( p_event_work->pFieldmap, 20, R_DOOR_PITCH );
    p_event_work->frame = 0;
    ++( *p_seq );
    break;
  case 1:
    if( 10 < p_event_work->frame++ )
    {
      ++( *p_seq );
    }
    break;
  case 2:
    FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, R_DOOR_FRAME, -R_DOOR_ZOOM_DIST );
    ++( *p_seq );
    p_event_work->frame = 0;
    break;
  case 3:
    if( 20 < p_event_work->frame++ )
    {
      ++( *p_seq );
    }
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( �E�o�����ւ̐i�� )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorIn_1Frame( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK*   p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA* cam          = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap );

  SetFarNear( p_event_work->pFieldmap );
  FIELD_CAMERA_SetAnglePitch( cam, R_DOOR_PITCH );
  FIELD_CAMERA_SetAngleYaw( cam, R_DOOR_YAW );
  FIELD_CAMERA_SetAngleLen( cam, FIELD_CAMERA_GetAngleLen( cam ) - R_DOOR_ZOOM_DIST );

  return GMEVENT_RES_FINISH;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ��o��������̐i�� )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_UpDoorOut( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  switch( *p_seq )
  {
  case 0:
    FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, U_DOOR_FRAME, U_DOOR_ZOOM_DIST );
    ++( *p_seq );
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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_LeftDoorOut( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA*   p_camera = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap );

  switch( *p_seq )
  {
  case 0:
    {
      FLD_CAMERA_PARAM def_param; 
      FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );
      SetFarNear( p_event_work->pFieldmap );
      FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, L_DOOR_FRAME, L_DOOR_ZOOM_DIST );
      FIELDMAP_TCB_AddTask_CameraRotate_Yaw( p_event_work->pFieldmap, L_DOOR_FRAME, def_param.Angle.y );
      FIELDMAP_TCB_AddTask_CameraRotate_Pitch( p_event_work->pFieldmap, L_DOOR_FRAME, def_param.Angle.x );
    }
    ++( *p_seq );
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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA*   p_camera = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap );

  switch( *p_seq )
  {
  case 0:
    {
      FLD_CAMERA_PARAM def_param; 
      FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );
      SetFarNear( p_event_work->pFieldmap );
      FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, R_DOOR_FRAME, R_DOOR_ZOOM_DIST );
      FIELDMAP_TCB_AddTask_CameraRotate_Yaw( p_event_work->pFieldmap, R_DOOR_FRAME, def_param.Angle.y );
      FIELDMAP_TCB_AddTask_CameraRotate_Pitch( p_event_work->pFieldmap, R_DOOR_FRAME, def_param.Angle.x );
    }
    ++( *p_seq );
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
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut_2Step( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA*        cam = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap ); 
  FLD_CAMERA_PARAM def_param; 

  // �f�t�H���g�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( cam, &def_param );

  switch( *p_seq )
  {
  case 0:
    SetFarNear( p_event_work->pFieldmap );
    FIELDMAP_TCB_AddTask_CameraZoom( p_event_work->pFieldmap, 20, R_DOOR_ZOOM_DIST );
    p_event_work->frame = 0;
    ++( *p_seq );
    break;
  case 1:
    if( 10 < p_event_work->frame++ )
    {
      ++( *p_seq );
    }
    break;
  case 2:
    FIELDMAP_TCB_AddTask_CameraRotate_Yaw( p_event_work->pFieldmap, 15, def_param.Angle.y );
    FIELDMAP_TCB_AddTask_CameraRotate_Pitch( p_event_work->pFieldmap, 15, def_param.Angle.x );
    ++( *p_seq );
    p_event_work->frame = 0;
    break;
  case 3:
    if( 20 < p_event_work->frame++ )
    {
      ++( *p_seq );
    }
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( �E�o��������̐i�� )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_RightDoorOut_1Frame( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA*   cam = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap ); 
  FLD_CAMERA_PARAM def_param; 

  // �f�t�H���g�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( cam, &def_param );

  SetFarNear( p_event_work->pFieldmap );
  FIELD_CAMERA_SetAnglePitch( cam, def_param.Angle.x );
  FIELD_CAMERA_SetAngleYaw( cam, def_param.Angle.y );
  FIELD_CAMERA_SetAngleLen( cam, def_param.Distance );

  return GMEVENT_RES_CONTINUE;
}
