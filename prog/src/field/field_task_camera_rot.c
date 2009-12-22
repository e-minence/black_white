////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(�J�����̉�]����)
 * @file   field_task_camera_rot.c
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_camera_rot.h"
#include "field/field_camera.h"


//==========================================================================================
// ���萔
//==========================================================================================
#define PI (0x8000)   // ��[rad]

// ��]�^�C�v
typedef enum {
  CAMERA_ROT_TYPE_YAW,    // ���[��]
  CAMERA_ROT_TYPE_PITCH,  // �s�b�`��]
} CAMERA_ROT_TYPE;


//==========================================================================================
// ���^�X�N���[�N
//==========================================================================================
typedef struct
{
  u8              seq;         // �V�[�P���X
  FIELDMAP_WORK*  fieldmap;    // �����Ώۂ̃t�B�[���h�}�b�v
  CAMERA_ROT_TYPE type;        // ��]�^�C�v
  u16             frame;       // �o�߃t���[����
  u16             endFrame;    // �I���t���[��
  u32             startAngle;  // ��]�p�����l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u32             endAngle;    // ��]�p�ŏI�l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)

} ROT_WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void SetupAngle( ROT_WORK* work );
static void UpdateAngle( ROT_WORK* work );
static FIELD_TASK_RETVAL RotateCamera( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief �J�����̃��[��]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param angle    �ω��p( 2�΃��W�A����65536���������l��P�ʂƂ��鐔 )
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraRot_Yaw( FIELDMAP_WORK* fieldmap, int frame, u16 angle )
{
  ROT_WORK* work;
  FIELD_TASK* task;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA* cam = FIELDMAP_GetFieldCamera( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(ROT_WORK), RotateCamera );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq        = 0;
  work->fieldmap   = fieldmap;
  work->type       = CAMERA_ROT_TYPE_YAW;
  work->frame      = 0;
  work->endFrame   = frame;
  work->endAngle   = angle;
  
  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�����̃s�b�`��]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param angle    �ω��p( 2�΃��W�A����65536���������l��P�ʂƂ��鐔 )
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraRot_Pitch( FIELDMAP_WORK* fieldmap, int frame, u16 angle )
{
  ROT_WORK* work;
  FIELD_TASK* task;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA* cam = FIELDMAP_GetFieldCamera( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(ROT_WORK), RotateCamera );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq        = 0;
  work->fieldmap   = fieldmap;
  work->type       = CAMERA_ROT_TYPE_PITCH;
  work->frame      = 0;
  work->endFrame   = frame;
  work->endAngle   = angle;
  
  return task;
}


//==========================================================================================
// ������J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief ��]�p�x��ݒ肷��
 *
 * @param work �ݒ�Ώۂ̃��[�N
 */
//------------------------------------------------------------------------------------------
static void SetupAngle( ROT_WORK* work )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  // �����p�x�E�ŏI�p�x��ݒ�
  switch( work->type )
  {
  case CAMERA_ROT_TYPE_YAW:
    work->startAngle = FIELD_CAMERA_GetAngleYaw( camera );
    break;
  case CAMERA_ROT_TYPE_PITCH:
    work->startAngle = FIELD_CAMERA_GetAnglePitch( camera );
    break;
  }

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̊p�x��360�x���̉��ʂ𗚂�����.
  if( work->startAngle < work->endAngle )
  {
    if( PI < (work->endAngle - work->startAngle) )
    {
      work->startAngle += 2*PI;
    }
  }
  else if( work->endAngle < work->startAngle )
  {
    if( PI < (work->startAngle - work->endAngle) )
    {
      work->endAngle += 2*PI;
    }
  } 
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�����A���O�����X�V����
 *
 * @param �X�V�Ώۂ̃��[�N
 */
//------------------------------------------------------------------------------------------
static void UpdateAngle( ROT_WORK* work )
{
  float t;
  u16 angle;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_CAMERA_MODE cam_mode;

  // �X�V
  work->frame++;
  if( work->frame <= work->endFrame )
  { 
    // �p�x���Z�o
    //  �v�Z��f(t)=-2t^3+3t^2 
    t     = work->frame / (float)work->endFrame;
    t     = t*t*( -2*t + 3 );
    angle = (u16)( ( (1-t) * work->startAngle ) + ( t * work->endAngle ) );

    // �^�C�v�ɉ������X�V���s��
    cam_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    switch( work->type )
    {
    case CAMERA_ROT_TYPE_YAW:    FIELD_CAMERA_SetAngleYaw( camera, angle );    
                                 OBATA_Printf( "TASK-CAM-ROT: yaw = %x\n", angle );
                                 break;
    case CAMERA_ROT_TYPE_PITCH:  FIELD_CAMERA_SetAnglePitch( camera, angle );  
                                 OBATA_Printf( "TASK-CAM-ROT: pitch = %x\n", angle );
                                 break;
    }
    FIELD_CAMERA_ChangeMode( camera, cam_mode );
  }
}

//==========================================================================================
// ���^�X�N�����֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �J�����A���O�����X�V����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL RotateCamera( void* wk )
{
  ROT_WORK* work = (ROT_WORK*)wk;

  switch( work->seq )
  {
  case 0:
    SetupAngle( work );
    work->seq++;
    break;
  case 1:
    UpdateAngle( work );
    if( work->endFrame <= work->frame )
    {
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}
