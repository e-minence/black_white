///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������h�A�ɏo���肷��ۂ̃J������]�C�x���g
 * @author obata
 * @date   2009.08.17
 */
///////////////////////////////////////////////////////////////////////////////////////////////
#include "event_camera_rotate.h"
#include "gamesystem/game_event.h"
#include "field/field_camera.h"
#include "field/fieldmap.h"


//=============================================================================================
/**
 * @brief �萔
 */
//=============================================================================================

#define CAMERA_DIST_MIN (100 << FX32_SHIFT)


//=============================================================================================
/**
 * @brief �C�x���g�E���[�N
 */
//=============================================================================================
typedef struct
{
  FIELDMAP_WORK* pFieldmap;
  u16 frame;        // �o�߃t���[����
  u16 endFramePitch;// �I���t���[��
  u16 endFrameYaw;  // �I���t���[��
  u16 endFrameDist; // �I���t���[��
  u16 startPitch;   // �s�b�`�����l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u16 endPitch;     // �s�b�`�ŏI�l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u32 startYaw;     // ���[�����l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u32 endYaw;       // ���[�ŏI�l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  fx32 startDist;   // �J�������������l
  fx32 endDist;     // �J���������ŏI�l
}
EVENT_WORK;


//=============================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//=============================================================================================
static void SetFarNear( FIELDMAP_WORK* p_fieldmap );
static void ResetFarNear( FIELDMAP_WORK* p_fieldmap );
static void UpdateYaw( EVENT_WORK* p_work );
static void UpdatePitch( EVENT_WORK* p_work );
static void UpdateDist( EVENT_WORK* p_work );

// �C�x���g�����֐�
static GMEVENT* EVENT_CameraRotateLeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_CameraRotateRightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_CameraRotateLeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_CameraRotateRightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );

// �C�x���g�����֐�
static GMEVENT_RESULT RotateCamera( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT RotateCamera3Step( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT RotateCamera2Step( GMEVENT* p_event, int* p_seq, void* p_work );
static GMEVENT_RESULT RotateCameraInOneFrame( GMEVENT* p_event, int* p_seq, void* p_work );


//=============================================================================================
/**
 * @brief ���J�֐��̒�`
 */
//=============================================================================================

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g���Ăяo��( �������� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ROTATE_CallDoorInEvent( GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );

  // �����ɉ������C�x���g���Ăяo��
  if( ( dir == DIR_LEFT ) )
  {
    GMEVENT_CallEvent( p_parent, EVENT_CameraRotateLeftDoorIn(p_gsys, p_fieldmap) );
  } 
  else if( ( dir == DIR_RIGHT ) )
  {
    GMEVENT_CallEvent( p_parent, EVENT_CameraRotateRightDoorIn(p_gsys, p_fieldmap) );
  } 
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief ���E�ɂ���h�A����o�Ă����ۂ�, �J�����̏����ݒ���s��
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ROTATE_PrepareDoorOut( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );
  FIELD_CAMERA*  cam    = FIELDMAP_GetFieldCamera( p_fieldmap );

  // ��l���̌����ɉ������J�����ݒ�
  if( dir == DIR_LEFT )
  {
    FIELD_CAMERA_SetAngleYaw( cam, 0xffff/360*270 );
    FIELD_CAMERA_SetAnglePitch( cam, 0xffff/360*20 );
    FIELD_CAMERA_SetAngleLen( cam, 100<<FX32_SHIFT );
    SetFarNear( p_fieldmap );
  }
  else if( dir == DIR_RIGHT )
  {
    FIELD_CAMERA_SetAngleYaw( cam, 0xffff/360*90 );
    FIELD_CAMERA_SetAnglePitch( cam, 0xffff/360*20 );
    FIELD_CAMERA_SetAngleLen( cam, 100<<FX32_SHIFT );
    SetFarNear( p_fieldmap );
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������]�C�x���g���Ăяo��( �o���� )
 *
 * @param p_parent   �e�C�x���g
 * @param p_gsys     �Q�[���V�X�e��
 * @param p_fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//-----------------------------------------------------------------------------------------------
void EVENT_CAMERA_ROTATE_CallDoorOutEvent( GMEVENT* p_parent, GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );

  // �����ɉ������C�x���g���Ăяo��
  if( ( dir == DIR_LEFT ) )
  {
    GMEVENT_CallEvent( p_parent, EVENT_CameraRotateRightDoorOut(p_gsys, p_fieldmap) );
  } 
  else if( ( dir == DIR_RIGHT ) )
  {
    GMEVENT_CallEvent( p_parent, EVENT_CameraRotateLeftDoorOut(p_gsys, p_fieldmap) );
  } 
}


//=============================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//=============================================================================================

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
static GMEVENT* EVENT_CameraRotateLeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  // �C�x���g����
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera, sizeof( EVENT_WORK ) );
  // p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera3Step, sizeof( EVENT_WORK ) );
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCameraInOneFrame, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap     = p_fieldmap;
  p_work->frame         = 0;
  p_work->endFramePitch = 10;
  p_work->endFrameYaw   = 10;
  p_work->endFrameDist  = 10;
  p_work->startPitch    = FIELD_CAMERA_GetAnglePitch( p_camera );
  p_work->endPitch      = 0xffff / 360 * 20;
  p_work->startYaw      = FIELD_CAMERA_GetAngleYaw( p_camera );
  p_work->endYaw        = 0xffff / 360 * 90;
  p_work->startDist     = FIELD_CAMERA_GetAngleLen( p_camera );
  p_work->endDist       = CAMERA_DIST_MIN;

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̃��[�l��360�x���̉��ʂ𗚂�����.
  if( p_work->startYaw < p_work->endYaw )
  {
    if( 0x7fff < (p_work->endYaw - p_work->startYaw) )
    {
      p_work->startYaw += 0xffff;
    }
  }
  else if( p_work->endYaw < p_work->startYaw )
  {
    if( 0x7fff < (p_work->startYaw - p_work->endYaw) )
    {
      p_work->endYaw += 0xffff;
    }
  } 

  // �J�����̏����ݒ�
  SetFarNear( p_work->pFieldmap );

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
static GMEVENT* EVENT_CameraRotateRightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  // �C�x���g����
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera, sizeof( EVENT_WORK ) );
  // p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera3Step, sizeof( EVENT_WORK ) );
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCameraInOneFrame, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap = p_fieldmap;
  p_work->frame = 0;
  p_work->endFramePitch = 10;
  p_work->endFrameYaw   = 10;
  p_work->endFrameDist  = 10;
  p_work->startPitch = FIELD_CAMERA_GetAnglePitch( p_camera );
  p_work->endPitch = 0xffff / 360 * 20;
  p_work->startYaw = FIELD_CAMERA_GetAngleYaw( p_camera );
  p_work->endYaw = 0xffff / 360 * 270;
  p_work->startDist = FIELD_CAMERA_GetAngleLen( p_camera );
  p_work->endDist = CAMERA_DIST_MIN; 

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̃��[�l��360�x���̉��ʂ𗚂�����.
  if( p_work->startYaw < p_work->endYaw )
  {
    if( 0x7fff < (p_work->endYaw - p_work->startYaw) )
    {
      p_work->startYaw += 0xffff;
    }
  }
  else if( p_work->endYaw < p_work->startYaw )
  {
    if( 0x7fff < (p_work->startYaw - p_work->endYaw) )
    {
      p_work->endYaw += 0xffff;
    }
  } 

  // �J�����̏����ݒ�
  SetFarNear( p_work->pFieldmap );

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
static GMEVENT* EVENT_CameraRotateLeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*         p_event;
  EVENT_WORK*      p_work;
  FIELD_CAMERA*    p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �J�����̃f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �C�x���g����
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera, sizeof( EVENT_WORK ) );
  // p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera3Step, sizeof( EVENT_WORK ) );
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCameraInOneFrame, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap     = p_fieldmap;
  p_work->frame         = 0;
  p_work->endFramePitch = 10;
  p_work->endFrameYaw   = 10;
  p_work->endFrameDist  = 10;
  p_work->startPitch    = FIELD_CAMERA_GetAnglePitch( p_camera );
  p_work->endPitch      = def_param.Angle.x;
  p_work->startYaw      = FIELD_CAMERA_GetAngleYaw( p_camera );
  p_work->endYaw        = def_param.Angle.y;
  p_work->startDist     = FIELD_CAMERA_GetAngleLen( p_camera );
  p_work->endDist       = def_param.Distance * FX32_ONE;

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̃��[�l��360�x���̉��ʂ𗚂�����.
  if( p_work->startYaw < p_work->endYaw )
  {
    if( 0x7fff < (p_work->endYaw - p_work->startYaw) )
    {
      p_work->startYaw += 0xffff;
    }
  }
  else if( p_work->endYaw < p_work->startYaw )
  {
    if( 0x7fff < (p_work->startYaw - p_work->endYaw) )
    {
      p_work->endYaw += 0xffff;
    }
  } 

  // �J�����̏����ݒ�
  SetFarNear( p_work->pFieldmap );

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
static GMEVENT* EVENT_CameraRotateRightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �J�����̃f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �C�x���g����
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera, sizeof( EVENT_WORK ) );
  // p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera3Step, sizeof( EVENT_WORK ) );
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );
  //p_event = GMEVENT_Create( p_gsys, NULL, RotateCameraInOneFrame, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  p_work->pFieldmap     = p_fieldmap;
  p_work->frame         = 0;
  p_work->endFramePitch = 10;
  p_work->endFrameYaw   = 10;
  p_work->endFrameDist  = 10;
  p_work->startPitch    = FIELD_CAMERA_GetAnglePitch( p_camera );
  p_work->endPitch      = def_param.Angle.x;
  p_work->startYaw      = FIELD_CAMERA_GetAngleYaw( p_camera );
  p_work->endYaw        = def_param.Angle.y;
  p_work->startDist     = FIELD_CAMERA_GetAngleLen( p_camera );
  p_work->endDist       = def_param.Distance * FX32_ONE;

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̃��[�l��360�x���̉��ʂ𗚂�����.
  if( p_work->startYaw < p_work->endYaw )
  {
    if( 0x7fff < (p_work->endYaw - p_work->startYaw) )
    {
      p_work->startYaw += 0xffff;
    }
  }
  else if( p_work->endYaw < p_work->startYaw )
  {
    if( 0x7fff < (p_work->startYaw - p_work->endYaw) )
    {
      p_work->endYaw += 0xffff;
    }
  } 

  // �J�����̏����ݒ�
  SetFarNear( p_work->pFieldmap );

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
  OBATA_Printf( "Set FarPlane and NearPlane\n" );
  OBATA_Printf( "before far  = %x(%d)\n", far, far>>FX32_SHIFT );
  OBATA_Printf( "before near = %x(%d)\n", near, near>>FX32_SHIFT );

  // Far�v���[���𔼕��ɂ���
  far /= 2;
  FIELD_CAMERA_SetFar( p_camera, far );

  // Near�v���[����ݒ肷��
  near = 50 << FX32_SHIFT;
  FIELD_CAMERA_SetNear( p_camera, near );

  // DEBUG:
  OBATA_Printf( "after far  = %x(%d)\n", far, far>>FX32_SHIFT );
  OBATA_Printf( "after near = %x(%d)\n", near, near>>FX32_SHIFT );
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J������far�v���[���Enear�v���[�����f�t�H���g�l�ɖ߂�
 *
 * @param p_fieldmap �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
static void ResetFarNear( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_CAMERA*    p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �f�t�H���g�l�ɖ߂�
  FIELD_CAMERA_SetFar( p_camera, def_param.Far );
  FIELD_CAMERA_SetNear( p_camera, def_param.Near );

  // DEBUG:
  OBATA_Printf( "reset far  = %d\n", def_param.Far >> FX32_SHIFT );
  OBATA_Printf( "reset near = %d\n", def_param.Near >> FX32_SHIFT );
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �s�b�`���X�V����
 *
 * @param p_work �C�x���g���[�N
 */
//-----------------------------------------------------------------------------------------------
static void UpdatePitch( EVENT_WORK* p_work )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->pFieldmap );
  u16           pitch;
  float         progress;

  if(p_work->frame <= p_work->endFramePitch )
  { 
    progress = p_work->frame / (float)p_work->endFramePitch;
    pitch    = (u16)( ( (1-progress) * p_work->startPitch ) + ( progress * p_work->endPitch ) );
    FIELD_CAMERA_SetAnglePitch( p_camera, pitch );
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief ���[���X�V����
 *
 * @param p_work �C�x���g���[�N
 */
//-----------------------------------------------------------------------------------------------
static void UpdateYaw( EVENT_WORK* p_work )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->pFieldmap );
  u16           yaw;
  float         progress;
  if(p_work->frame <= p_work->endFrameYaw )
  {
    progress = p_work->frame / (float)p_work->endFrameYaw;
    yaw      = (u16)( ( (1-progress) * p_work->startYaw ) + ( progress * p_work->endYaw ) );
    FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J�����������X�V����
 *
 * @param p_work �C�x���g���[�N
 */
//-----------------------------------------------------------------------------------------------
static void UpdateDist( EVENT_WORK* p_work )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->pFieldmap );
  fx32          dist;
  float         progress;
  if(p_work->frame <= p_work->endFrameDist )
  {
    progress = p_work->frame / (float)p_work->endFrameDist;
    dist     = FX_F32_TO_FX32( ( (1-progress) * FX_FX32_TO_F32(p_work->startDist) ) + ( progress * FX_FX32_TO_F32(p_work->endDist) ) );
  FIELD_CAMERA_SetAngleLen( p_camera, dist ); 
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���[�E�s�b�`�E�����𓯎��ɕύX )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT RotateCamera( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  // �J�����p�����[�^���X�V
  UpdatePitch( p_event_work );
  UpdateYaw( p_event_work );
  UpdateDist( p_event_work );

  // �w��񐔓��삵����I��
  p_event_work->frame++;
  if( ( p_event_work->endFrameYaw < p_event_work->frame ) &&
      ( p_event_work->endFramePitch < p_event_work->frame ) &&
      ( p_event_work->endFrameDist < p_event_work->frame ) )
  {
    ResetFarNear( p_event_work->pFieldmap );
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���[�E�s�b�`�E������i�K�I�ɕύX )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT RotateCamera3Step( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK*   p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA* p_camera     = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap );
  
  switch( *p_seq )
  {
  case 0:
    UpdateDist( p_event_work );
    if( p_event_work->endFrameDist < ++(p_event_work->frame) )
    {
      ++ *p_seq;
      p_event_work->frame = 0;
    }
    break;
  case 1:
    UpdatePitch( p_event_work );
    if( p_event_work->endFramePitch < ++(p_event_work->frame) )
    {
      ++ *p_seq;
      p_event_work->frame = 0;
    }
    break;
  case 2:
    UpdateYaw( p_event_work );
    if( p_event_work->endFrameYaw < ++(p_event_work->frame) )
    {
      ResetFarNear( p_event_work->pFieldmap );
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���[�E�s�b�`�E������i�K�I�ɕύX )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT RotateCamera2Step( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK*   p_event_work = (EVENT_WORK*)p_work;
  
  switch( *p_seq )
  {
  case 0:
    UpdateDist( p_event_work );
    UpdatePitch( p_event_work );
    p_event_work->frame++;
    if( ( p_event_work->endFramePitch < (p_event_work->frame) ) &&
        ( p_event_work->endFrameDist  < (p_event_work->frame) ) )
    {
      ++ *p_seq;
      p_event_work->frame = 0;
    }
    break;
  case 1:
    UpdateYaw( p_event_work );
    p_event_work->frame++;
    if( p_event_work->endFrameYaw < p_event_work->frame )
    {
      ResetFarNear( p_event_work->pFieldmap );
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �C�x���g�����֐�( ���[�E�s�b�`�E�������u���ɕύX )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT RotateCameraInOneFrame( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK*   p_event_work = (EVENT_WORK*)p_work;
  FIELD_CAMERA* p_camera     = FIELDMAP_GetFieldCamera( p_event_work->pFieldmap );

  switch( *p_seq )
  {
  case 0:
    if( 0 < ++(p_event_work->frame) )
    {
      ++ *p_seq;
      p_event_work->frame = 0;
    }
    break;
  case 1:
      FIELD_CAMERA_SetAngleLen( p_camera, p_event_work->endDist );
      FIELD_CAMERA_SetAngleYaw( p_camera, p_event_work->endYaw );
      FIELD_CAMERA_SetAnglePitch( p_camera, p_event_work->endPitch );
      ++ *p_seq;
    break;
  case 2:
    if( 10 < ++(p_event_work->frame) )
    {
      ++ *p_seq;
      ResetFarNear( p_event_work->pFieldmap );
      return GMEVENT_RES_FINISH;
    }
    break;
  }

  return GMEVENT_RES_CONTINUE;
}
