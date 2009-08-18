///////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �h�A�ɏo���肷��ۂ̃J��������C�x���g
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
#define ZOOM_DIST (15 << FX32_SHIFT)


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

// �C�x���g���[�N�ݒ�֐�
static void InitWork( EVENT_WORK* p_work, FIELDMAP_WORK* p_fieldmap );
static void SetPitchAction( EVENT_WORK* p_work, u16 time, u16 start, u16 end );
static void SetYawAction( EVENT_WORK* p_work, u16 time, u32 start, u32 end );
static void SetDistAction( EVENT_WORK* p_work, u16 time, fx32 start, fx32 end );

// �J��������֐�
static void UpdateYaw( EVENT_WORK* p_work );
static void UpdatePitch( EVENT_WORK* p_work );
static void UpdateDist( EVENT_WORK* p_work );

// �C�x���g�����֐�
static GMEVENT* EVENT_UpDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_LeftDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_UpDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_LeftDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap );

// �C�x���g�����֐�
static GMEVENT_RESULT Zoom( GMEVENT* p_event, int* p_seq, void* p_work );
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
void EVENT_CAMERA_ROTATE_PrepareDoorOut( FIELDMAP_WORK* p_fieldmap )
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer( p_fieldmap );
  MMDL *         fmmdl  = FIELD_PLAYER_GetMMdl( player );
  u16            dir    = MMDL_GetDirDisp( fmmdl );
  FIELD_CAMERA*  cam    = FIELDMAP_GetFieldCamera( p_fieldmap );

  // ��l���̌����ɉ������J�����ݒ�
  switch( dir )
  {
  case DIR_DOWN:
    FIELD_CAMERA_SetAngleLen( cam, FIELD_CAMERA_GetAngleLen( cam ) - ZOOM_DIST );
    break;
  case DIR_LEFT:
    FIELD_CAMERA_SetAngleYaw( cam, 0xffff/360*270 );
    FIELD_CAMERA_SetAnglePitch( cam, 0xffff/360*20 );
    FIELD_CAMERA_SetAngleLen( cam, 100<<FX32_SHIFT );
    SetFarNear( p_fieldmap );
    break;
  case DIR_RIGHT:
    FIELD_CAMERA_SetAngleYaw( cam, 0xffff/360*90 );
    FIELD_CAMERA_SetAnglePitch( cam, 0xffff/360*20 );
    FIELD_CAMERA_SetAngleLen( cam, 100<<FX32_SHIFT );
    SetFarNear( p_fieldmap );
    break;
  default:
    break;
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
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, Zoom, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  InitWork( p_work, p_fieldmap );
  SetDistAction( p_work, 6,
      FIELD_CAMERA_GetAngleLen( p_camera ), FIELD_CAMERA_GetAngleLen( p_camera ) - ZOOM_DIST );

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
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  InitWork( p_work, p_fieldmap );
  SetPitchAction( p_work, 10, FIELD_CAMERA_GetAnglePitch( p_camera ), 0xffff / 360 * 20 );
  SetYawAction  ( p_work, 10, FIELD_CAMERA_GetAngleYaw( p_camera ),   0xffff / 360 * 90 );
  SetDistAction ( p_work, 10, FIELD_CAMERA_GetAngleLen( p_camera ),   CAMERA_DIST_MIN );

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
static GMEVENT* EVENT_RightDoorIn( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  InitWork( p_work, p_fieldmap );
  SetPitchAction( p_work, 10, FIELD_CAMERA_GetAnglePitch( p_camera ), 0xffff / 360 * 20 );
  SetYawAction  ( p_work, 10, FIELD_CAMERA_GetAngleYaw( p_camera ),   0xffff / 360 * 270 );
  SetDistAction ( p_work, 10, FIELD_CAMERA_GetAngleLen( p_camera ), CAMERA_DIST_MIN ); 

  // �J�����̏����ݒ�
  SetFarNear( p_work->pFieldmap );

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
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �J�����̃f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, Zoom, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  InitWork( p_work, p_fieldmap );
  SetDistAction( p_work, 6, FIELD_CAMERA_GetAngleLen( p_camera ), def_param.Distance * FX32_ONE );

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
  FIELD_CAMERA*    p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �J�����̃f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  InitWork( p_work, p_fieldmap );
  SetPitchAction( p_work, 10, FIELD_CAMERA_GetAnglePitch( p_camera ), def_param.Angle.x );
  SetYawAction  ( p_work, 10, FIELD_CAMERA_GetAngleYaw( p_camera ),   def_param.Angle.y );
  SetDistAction ( p_work, 10, FIELD_CAMERA_GetAngleLen( p_camera ),   def_param.Distance * FX32_ONE );

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
static GMEVENT* EVENT_RightDoorOut( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap )
{
  GMEVENT*      p_event;
  EVENT_WORK*   p_work;
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_fieldmap );
  FLD_CAMERA_PARAM def_param;

  // �J�����̃f�t�H���g�E�p�����[�^���擾
  FIELD_CAMERA_GetInitialParameter( p_camera, &def_param );

  // �C�x���g����
  p_event = GMEVENT_Create( p_gsys, NULL, RotateCamera2Step, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  p_work = GMEVENT_GetEventWork( p_event );
  InitWork( p_work, p_fieldmap );
  SetPitchAction( p_work, 10, FIELD_CAMERA_GetAnglePitch( p_camera ), def_param.Angle.x );
  SetYawAction  ( p_work, 10, FIELD_CAMERA_GetAngleYaw( p_camera ),   def_param.Angle.y );
  SetDistAction ( p_work, 10, FIELD_CAMERA_GetAngleLen( p_camera ),   def_param.Distance * FX32_ONE );

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
 * @brief �C�x���g���[�N������������
 *
 * @param p_work     ����������ϐ�
 * @param p_fieldmap �ݒ肷��t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------------------------
static void InitWork( EVENT_WORK* p_work, FIELDMAP_WORK* p_fieldmap )
{
  p_work->pFieldmap     = p_fieldmap;
  p_work->frame         = 0;
  p_work->endFramePitch = 0;
  p_work->endFrameYaw   = 0;
  p_work->endFrameDist  = 0;
  p_work->startPitch    = 0;
  p_work->endPitch      = 0;
  p_work->startYaw      = 0;
  p_work->endYaw        = 0;
  p_work->startDist     = 0;
  p_work->endDist       = 0;
}

//-----------------------------------------------------------------------------------------------
/**
 * @breif �s�b�`�̓����ݒ肷��
 *
 * @param p_work  �ݒ�ΏۃC�x���g���[�N
 * @param time    ����t���[����
 * @param start   �����l
 * @param end     �I���l
 */
//-----------------------------------------------------------------------------------------------
static void SetPitchAction( EVENT_WORK* p_work, u16 time, u16 start, u16 end )
{
  p_work->startPitch    = start;
  p_work->endPitch      = end;
  p_work->endFramePitch = time;
}

//-----------------------------------------------------------------------------------------------
/**
 * @breif ���[�̓����ݒ肷��
 *
 * @param p_work  �ݒ�ΏۃC�x���g���[�N
 * @param time    ����t���[����
 * @param start   �����l
 * @param end     �I���l
 */
//-----------------------------------------------------------------------------------------------
static void SetYawAction( EVENT_WORK* p_work, u16 time, u32 start, u32 end )
{
  p_work->startYaw    = start;
  p_work->endYaw      = end;
  p_work->endFrameYaw = time;

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
}

//-----------------------------------------------------------------------------------------------
/**
 * @breif �J���������̓����ݒ肷��
 *
 * @param p_work  �ݒ�ΏۃC�x���g���[�N
 * @param time    ����t���[����
 * @param start   �����l
 * @param end     �I���l
 */
//-----------------------------------------------------------------------------------------------
static void SetDistAction( EVENT_WORK* p_work, u16 time, fx32 start, fx32 end )
{
  p_work->startDist    = start;
  p_work->endDist      = end;
  p_work->endFrameDist = time;
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
 * @brief �C�x���g�����֐�( �Y�[���C�� or �Y�[���A�E�g )
 *
 * @param p_event �C�x���g
 * @param p_seq   �V�[�P���X�ԍ�
 * @param p_work  �C�x���g���[�N
 *
 * @return GMEVENT_RES_CONTINUE or GMEVENT_RES_FINISH
 */
//-----------------------------------------------------------------------------------------------
static GMEVENT_RESULT Zoom( GMEVENT* p_event, int* p_seq, void* p_work )
{
  EVENT_WORK* p_event_work = (EVENT_WORK*)p_work;

  // �J�����p�����[�^���X�V
  UpdateDist( p_event_work );

  // �w��񐔓��삵����I��
  p_event_work->frame++;
  if( p_event_work->endFrameDist < p_event_work->frame )
  {
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
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
