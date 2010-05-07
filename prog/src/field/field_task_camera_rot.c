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
//#define DEBUG_EVENT_ON
#define PI (0x8000)   // ��[rad]

// ��]�^�C�v
typedef enum {
  CAMERA_ROT_TYPE_YAW,    // ���[��]
  CAMERA_ROT_TYPE_PITCH,  // �s�b�`��]
} CAMERA_ROT_TYPE;


//==========================================================================================
// ���^�X�N���[�N
//==========================================================================================
typedef struct {

  FIELDMAP_WORK*  fieldmap;
  FIELD_CAMERA*   camera;

  CAMERA_ROT_TYPE rot_type;    // ��]�^�C�v
  u8              seq;         // �V�[�P���X
  u16             now_frame;   // �o�߃t���[����
  u16             end_frame;   // �I���t���[��
  u32             start_angle; // ��]�p�����l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u32             end_angle;   // ��]�p�ŏI�l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)

} ROT_WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void SetupStartAngle( ROT_WORK* work );
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

  // �^�X�N�𐶐�
  task = FIELD_TASK_Create( heap_id, sizeof(ROT_WORK), RotateCamera );

  // �^�X�N���[�N��������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap  = fieldmap;
  work->camera    = FIELDMAP_GetFieldCamera( fieldmap );
  work->seq       = 0;
  work->rot_type  = CAMERA_ROT_TYPE_YAW;
  work->now_frame = 0;
  work->end_frame = frame;
  work->end_angle = angle;
  
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

  // �^�X�N�𐶐�
  task = FIELD_TASK_Create( heap_id, sizeof(ROT_WORK), RotateCamera );

  // �^�X�N���[�N��������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap   = fieldmap;
  work->camera     = FIELDMAP_GetFieldCamera( fieldmap );
  work->seq        = 0;
  work->rot_type   = CAMERA_ROT_TYPE_PITCH;
  work->now_frame  = 0;
  work->end_frame  = frame;
  work->end_angle  = angle;
  
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
static void SetupStartAngle( ROT_WORK* work )
{
  FIELD_CAMERA* camera = work->camera;
  u32 end_angle = work->end_angle;
  u32 start_angle;

  // �����p�x��ݒ�
  switch( work->rot_type ) {
  default: GF_ASSERT(0);
  case CAMERA_ROT_TYPE_YAW:   start_angle = FIELD_CAMERA_GetAngleYaw( camera ); break;
  case CAMERA_ROT_TYPE_PITCH: start_angle = FIELD_CAMERA_GetAnglePitch( camera ); break;
  }

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̊p�x��360�x���̉��ʂ𗚂�����.
  if( start_angle < end_angle ) {
    if( PI < (end_angle - start_angle) ) {
      start_angle += 2*PI;
    }
  }
  else if( end_angle < start_angle ) {
    if( PI < (start_angle - end_angle) ) {
      end_angle += 2*PI;
    }
  } 

  // ���[�N�̒l���X�V
  work->start_angle = start_angle;
  work->end_angle   = end_angle;
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
  FIELD_CAMERA* camera = work->camera;
  FIELD_CAMERA_MODE camera_mode;

  // �X�V
  work->now_frame++;

  if( work->now_frame <= work->end_frame ) { 
    float t;
    u16 angle;

    // �p�x���Z�o
    t     = work->now_frame / (float)work->end_frame;
    t     = t*t*( -2*t + 3 );
    angle = (u16)( ( (1-t) * work->start_angle ) + ( t * work->end_angle ) );

    camera_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

    // �^�C�v�ɉ������X�V���s��
    switch( work->rot_type ) {
    case CAMERA_ROT_TYPE_YAW:    FIELD_CAMERA_SetAngleYaw( camera, angle );    break;
    case CAMERA_ROT_TYPE_PITCH:  FIELD_CAMERA_SetAnglePitch( camera, angle );  break;
    }

    FIELD_CAMERA_ChangeMode( camera, camera_mode ); 
  }

#ifdef DEBUG_EVENT_ON
  OBATA_Printf( "TASK-CAM-ROT: now_frame = %d, pitch = 0x%x\n", work->now_frame, angle ); 
#endif
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

  switch( work->seq ) {
  case 0:
    SetupStartAngle( work );
    work->seq++;
    break;

  case 1:
    UpdateAngle( work );
    if( work->end_frame <= work->now_frame ) {
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}
