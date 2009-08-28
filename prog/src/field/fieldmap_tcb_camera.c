////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�B�[���h�}�b�vTCB( �J�������� )
 * @file   fieldmap_tcb_camera.c
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


////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �Y�[��
 */
////////////////////////////////////////////////////////////////////////////////////////////

//==========================================================================================
/**
 * @brief TCB���[�N�G���A
 */
//==========================================================================================
typedef struct
{
  u16            frame;     // �o�߃t���[����
  u16            endFrame;  // �I���t���[��
  fx32           startDist; // �J�������������l
  fx32           endDist;   // �J���������ŏI�l
  FIELDMAP_WORK* pFieldmap; // �����Ώۂ̃t�B�[���h�}�b�v
}
TCB_WORK_ZOOM;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
static void TCB_FUNC_Zoom( GFL_TCB* tcb, void* work ); 
static void UpdateDist( TCB_WORK_ZOOM* p_work );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��ǉ�����( �Y�[�� )
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 */
//------------------------------------------------------------------------------------------
void FIELDMAP_TCB_CAMERA_AddTask_Zoom( FIELDMAP_WORK* fieldmap, int frame, fx32 dist )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA*   cam = FIELDMAP_GetFieldCamera( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_ZOOM* work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_ZOOM ) ); 
  GFL_TCB*        tcb;

  // TCB���[�N������
  work->frame     = 0;
  work->endFrame  = frame;
  work->startDist = FIELD_CAMERA_GetAngleLen( cam );
  work->endDist   = FIELD_CAMERA_GetAngleLen( cam ) + dist;
  work->pFieldmap = fieldmap;

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_Zoom, work, 0 );
}


//========================================================================================== 
/**
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief TCB���s�֐�( �Y�[�� )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_Zoom( GFL_TCB* tcb, void* work )
{
  TCB_WORK_ZOOM* tcbwork = work;

  // �J�����������X�V
  UpdateDist( tcbwork );

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
 * @brief �J�����������X�V����
 *
 * @param p_work �Y�[��TCB�C�x���g���[�N
 */
//-----------------------------------------------------------------------------------------------
static void UpdateDist( TCB_WORK_ZOOM* p_work )
{
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->pFieldmap );
  fx32          dist;

  // ������Ԓ��Ȃ�, �X�V����
  if( p_work->frame <= p_work->endFrame)
  {
    float t = p_work->frame / (float)p_work->endFrame;
    dist    = FX_F32_TO_FX32( ( (1-t) * FX_FX32_TO_F32(p_work->startDist) ) + ( t * FX_FX32_TO_F32(p_work->endDist) ) );
    FIELD_CAMERA_SetAngleLen( p_camera, dist ); 
  }
}




////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �J������]
 */
////////////////////////////////////////////////////////////////////////////////////////////

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
  CAMERA_ROT_TYPE type;        // ��]�^�C�v
  u16             frame;       // �o�߃t���[����
  u16             endFrame;    // �I���t���[��
  u32             startAngle;  // ��]�p�����l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  u32             endAngle;    // ��]�p�ŏI�l(2�΃��W�A����65536���������l��P�ʂƂ��鐔)
  FIELDMAP_WORK*  pFieldmap;   // �����Ώۂ̃t�B�[���h�}�b�v
}
TCB_WORK_ROT;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
static void TCB_FUNC_RotCamera( GFL_TCB* tcb, void* work ); 
static void UpdateAngle( TCB_WORK_ROT* p_work );


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
void FIELDMAP_TCB_CAMERA_AddTask_Yaw( FIELDMAP_WORK* fieldmap, int frame, u16 angle )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA*  cam  = FIELDMAP_GetFieldCamera( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_ROT*  work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_ROT ) ); 
  GFL_TCB*        tcb;

  // TCB���[�N������
  work->type      = CAMERA_ROT_TYPE_YAW;
  work->frame     = 0;
  work->endFrame  = frame;
  work->startAngle = FIELD_CAMERA_GetAngleYaw( cam );
  work->endAngle   = angle;
  work->pFieldmap = fieldmap;

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

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotCamera, work, 0 );
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
void FIELDMAP_TCB_CAMERA_AddTask_Pitch( FIELDMAP_WORK* fieldmap, int frame, u16 angle )
{
  HEAPID      heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA*  cam  = FIELDMAP_GetFieldCamera( fieldmap );
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  TCB_WORK_ROT*  work = GFL_HEAP_AllocMemoryLo( heap_id, sizeof( TCB_WORK_ROT ) ); 
  GFL_TCB*        tcb;

  // TCB���[�N������
  work->type      = CAMERA_ROT_TYPE_PITCH;
  work->frame     = 0;
  work->endFrame  = frame;
  work->startAngle = FIELD_CAMERA_GetAnglePitch( cam );
  work->endAngle   = angle;
  work->pFieldmap = fieldmap;

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

  // TCB��ǉ�
  tcb = GFL_TCB_AddTask( tcbsys, TCB_FUNC_RotCamera, work, 0 );
}


//========================================================================================== 
/**
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief TCB���s�֐�( �J������] )
 */
//------------------------------------------------------------------------------------------
static void TCB_FUNC_RotCamera( GFL_TCB* tcb, void* work )
{
  TCB_WORK_ROT* tcbwork = work;

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
static void UpdateAngle( TCB_WORK_ROT* p_work )
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
