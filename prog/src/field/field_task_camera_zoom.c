////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(�J�����̃Y�[������)
 * @file   field_task_camera_zoom.c
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <gflib.h>
#include "field_task.h"
#include "fieldmap.h"
#include "field/field_camera.h"
#include "field_task_camera_zoom.h"


//==========================================================================================
// ���^�X�N���[�N
//==========================================================================================
typedef struct
{
  u8             seq;       // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // �����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;     // �o�߃t���[����
  u16            endFrame;  // �I���t���[��
  fx32           startDist; // �J�������������l
  fx32           moveDist;  // �ړ�����
  fx32           endDist;   // �J���������ŏI�l

} ZOOM_WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static FIELD_TASK_RETVAL CameraLinearZoom( void* wk );
static FIELD_TASK_RETVAL CameraSharpZoom( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief �J�����𓙑��Y�[��������^�X�N���쐬����
 *
 * @param fieldmap �����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �Y�[�������܂ł̃t���[����
 * @param dist     �Y�[������
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraLinearZoom( FIELDMAP_WORK* fieldmap, u16 frame, fx32 dist )
{
  FIELD_TASK* task;
  ZOOM_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(ZOOM_WORK), CameraLinearZoom );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq       = 0;
  work->fieldmap  = fieldmap;
  work->frame     = 0;
  work->endFrame  = frame;
  work->moveDist  = dist;

  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�������}�����Y�[��������^�X�N���쐬����
 *
 * @param fieldmap �����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �Y�[�������܂ł̃t���[����
 * @param dist     �Y�[������
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraSharpZoom( FIELDMAP_WORK* fieldmap, u16 frame, fx32 dist )
{
  FIELD_TASK* task;
  ZOOM_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(ZOOM_WORK), CameraSharpZoom );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq       = 0;
  work->fieldmap  = fieldmap;
  work->frame     = 0;
  work->endFrame  = frame;
  work->moveDist  = dist;

  return task;
}


//==========================================================================================
// ���^�X�N�����֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �J�����̓����Y�[��
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL CameraLinearZoom( void* wk )
{
  ZOOM_WORK*      work = (ZOOM_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_CAMERA_MODE cam_mode;

  switch( work->seq )
  {
  case 0:
    work->startDist = FIELD_CAMERA_GetAngleLen( camera );
    work->endDist   = FIELD_CAMERA_GetAngleLen( camera ) + work->moveDist;
    work->seq++;
    break;
  case 1:
    work->frame++;
    cam_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // ���`�⊮
      float    t = work->frame / (float)work->endFrame;
      float   d1 = FX_FX32_TO_F32( work->startDist );
      float   d2 = FX_FX32_TO_F32( work->endDist );
      float dist = (1-t) * d1 + ( t * d2);
      FIELD_CAMERA_SetAngleLen( camera, FX_F32_TO_FX32( dist ) ); 
    }
    FIELD_CAMERA_ChangeMode( camera, cam_mode );
    if( work->endFrame <= work->frame )
    {
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�����̋}�����Y�[��
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL CameraSharpZoom( void* wk )
{
  ZOOM_WORK*      work = (ZOOM_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_CAMERA_MODE cam_mode;

  switch( work->seq )
  {
  case 0:
    work->startDist = FIELD_CAMERA_GetAngleLen( camera );
    work->endDist   = FIELD_CAMERA_GetAngleLen( camera ) + work->moveDist;
    work->seq++;
    break;
  case 1:
    work->frame++;
    cam_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    if( work->frame <= work->endFrame)
    { // 2���֐� f(x) = ��ax
      float a, t, d1, d2, dist; 
      a    = 1.0f;
      t    = work->frame / (float)work->endFrame;
      t    = sqrt( a * t );
      d1   = FX_FX32_TO_F32( work->startDist );
      d2   = FX_FX32_TO_F32( work->endDist );
      dist = (1-t) * d1 + ( t * d2);
      FIELD_CAMERA_SetAngleLen( camera, FX_F32_TO_FX32( dist ) ); 
    }
    FIELD_CAMERA_ChangeMode( camera, cam_mode );
    if( work->endFrame <= work->frame )
    {
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}
