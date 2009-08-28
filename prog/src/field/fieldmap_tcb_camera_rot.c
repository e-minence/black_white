////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB( �J�������� )
 * @file   fieldmap_tcb_camera_rot.c
 * @author obata
 * @date   2009.08.26
 *
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "fieldmap_tcb.h"
#include "fieldmap.h"
#include "field_player.h"
#include "field/field_camera.h"


//==========================================================================================
/**
 * @brief �萔
 */
//==========================================================================================
// ��[rad]
#define PI (0x8000)   

// ��]�^�C�v
typedef enum 
{
  CAMERA_ROT_TYPE_YAW,    // ���[��]
  CAMERA_ROT_TYPE_PITCH,  // �s�b�`��]
}
CAMERA_ROT_TYPE;


//==========================================================================================
/**
 * @brief TCB���[�N�G���A
 */
//==========================================================================================
typedef struct
{
  FIELDMAP_WORK*  pFieldmap;   // �����Ώۂ̃t�B�[���h�}�b�v
  CAMERA_ROT_TYPE type;        // ��]�^�C�v
  u16             frame;       // �o�߃t���[����
  u16             endFrame;    // �I���t���[��
  u32             startAngle;  // ��]�p�����l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u32             endAngle;    // ��]�p�ŏI�l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
}
ROT_WORK;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
static void InitWork( 
    ROT_WORK* work, FIELDMAP_WORK* fieldmap, 
    CAMERA_ROT_TYPE type, u16 end_frame, u32 start_angle, u32 end_angle );
static void TCB_FUNC_RotCamera( GFL_TCB* tcb, void* work ); 
static void UpdateAngle( ROT_WORK* p_work );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �J���� ���[��] )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param angle    �ŏI�I�Ȋp�x( 2�΃��W�A����65536���������l��P�ʂƂ��鐔 )
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_CameraRotate_Yaw( FIELDMAP_WORK* fieldmap, int frame, u16 angle )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA*  cam  = FIELDMAP_GetFieldCamera( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  ROT_WORK*      work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( ROT_WORK ) ); 

  // TCB���[�N������
  InitWork( work, fieldmap, CAMERA_ROT_TYPE_YAW, frame, FIELD_CAMERA_GetAngleYaw( cam ), angle );

  // TCB��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotCamera, work, 0 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �J���� �s�b�`��] )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param angle    �ŏI�I�Ȋp�x( 2�΃��W�A����65536���������l��P�ʂƂ��鐔 )
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_AddTask_CameraRotate_Pitch( FIELDMAP_WORK* fieldmap, int frame, u16 angle )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA*  cam  = FIELDMAP_GetFieldCamera( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  ROT_WORK*      work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( ROT_WORK ) ); 

  // TCB���[�N������
  InitWork( work, fieldmap, CAMERA_ROT_TYPE_PITCH, frame, FIELD_CAMERA_GetAnglePitch( cam ), angle );

  // �^�X�N��ǉ�
  GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotCamera, work, 0 );
}


//========================================================================================== 
/**
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief ���[�N������������
 * 
 * @param work         ���������郏�[�N
 * @param fieldmap     ����Ώۂ̃t�B�[���h�}�b�v
 * @param type         ��]�^�C�v
 * @param end_frame    �ŏI�t���[����
 * @param start_angle  �����p�x( 2�΃��W�A����65536���������l��P�ʂƂ��鐔 )
 * @param end_angle    �ŏI�p�x( 2�΃��W�A����65536���������l��P�ʂƂ��鐔 )
 */
//------------------------------------------------------------------------------------------
static void InitWork( 
    ROT_WORK* work, FIELDMAP_WORK* fieldmap, 
    CAMERA_ROT_TYPE type, u16 end_frame, u32 start_angle, u32 end_angle )
{
  work->pFieldmap  = fieldmap;
  work->type       = type;
  work->frame      = 0;
  work->endFrame   = end_frame;
  work->startAngle = start_angle;
  work->endAngle   = end_angle;

  // ���̕����ɉ񂵂��Ƃ�, 180�x�ȏ�̉�]���K�v�ɂȂ�ꍇ, ���̕����ɉ�]������K�v������.
  // ��]�̕������t�ɂ��邽�߂�, ���������̊p�x��360�x���̉��ʂ𗚂�����.
  if( start_angle < end_angle )
  {
    if( PI < (end_angle - start_angle) )
    {
      work->startAngle += 2*PI;
    }
  }
  else if( end_angle < start_angle )
  {
    if( PI < (start_angle - end_angle) )
    {
      work->endAngle += 2*PI;
    }
  } 
}

//------------------------------------------------------------------------------------------
/**
 * @brief TCB���s�֐�( �J������] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotCamera( GFL_TCB* tcb, void* work )
{
  ROT_WORK* tcbwork = work;

  // �J�����ʒu���X�V
  UpdateAngle( tcbwork );

  // ���񐔓��삵����I��
  ++(tcbwork->frame);
  if( tcbwork->endFrame < tcbwork->frame )
  {
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//-----------------------------------------------------------------------------------------------
/**
 * @brief �J�����A���O�����X�V����
 *
 * @param p_work �C�x���g���[�N
 */
//-----------------------------------------------------------------------------------------------
static void UpdateAngle( ROT_WORK* p_work )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->pFieldmap );
  u16           angle;

  // ���쒆�Ȃ�, �X�V����
  if(p_work->frame <= p_work->endFrame )
  {
    // �p�x���Z�o
    //  �v�Z��f(t)=-2t^3+3t^2 
    float t = p_work->frame / (float)p_work->endFrame;
    t       = t*t*( -2*t + 3 );
    angle     = (u16)( ( (1-t) * p_work->startAngle ) + ( t * p_work->endAngle ) );

    // �^�C�v�ɉ������X�V���s��
    switch( p_work->type )
    {
    case CAMERA_ROT_TYPE_YAW:     FIELD_CAMERA_SetAngleYaw( p_camera, angle );    break;
    case CAMERA_ROT_TYPE_PITCH:   FIELD_CAMERA_SetAnglePitch( p_camera, angle );  break;
    }
  }
}
