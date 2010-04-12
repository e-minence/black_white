////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(�J�����̒����_�␳���W�̈ړ�����)
 * @file   field_task_target_offset.h
 * @author obata
 * @date   2009.11.24
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_target_offset.h"

#include "field_camera.h"

//#define DEBUG_PRINT_ENABLE

//===============================================================================================
// ���^�X�N���[�N
//===============================================================================================
typedef struct
{
  u8                  seq;  // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // ����Ώۃt�B�[���h�}�b�v
  int               frame;  // ���݂̃t���[����
  int            endFrame;  // �ŏI�t���[����
  VecFx32     startOffset;  // �ړ��J�n�ʒu
  VecFx32       endOffset;  // �ŏI�ʒu

} TASK_WORK;


//===============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//===============================================================================================
static FIELD_TASK_RETVAL UpdateOffset( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief �J�����̒����_�␳���W�ړ��^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param offset   �ŏI�I�ȃI�t�Z�b�g�l
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraTargetOffset( FIELDMAP_WORK* fieldmap, 
                                                  int frame, 
                                                  const VecFx32* offset )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), UpdateOffset );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->frame    = 0;
  work->endFrame = frame;
  VEC_Set( &work->endOffset, offset->x, offset->y, offset->z );

  return task;
}


//===============================================================================================
// ���^�X�N�����֐�
//===============================================================================================

//-----------------------------------------------------------------------------------------------
/**
 * @brief ���@�̕��s�ړ�����
 */
//-----------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL UpdateOffset( void* wk )
{
  TASK_WORK*      work = (TASK_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( work->seq )
  {
  case 0:
    // �����I�t�Z�b�g���擾
    FIELD_CAMERA_GetTargetOffset( camera, &work->startOffset );
    work->seq++;
    break;
  case 1:
    // �I�t�Z�b�g���X�V
    work->frame++;
    {
      VecFx32 v0, v1, v2;
      fx32 w1 = FX_F32_TO_FX32( (float)work->frame / (float)work->endFrame );
      fx32 w0 = FX32_ONE - w1;
      GFL_CALC3D_VEC_MulScalar( &work->startOffset, w0, &v0 );
      GFL_CALC3D_VEC_MulScalar( &work->endOffset,   w1, &v1 );
      VEC_Add( &v0, &v1, &v2 );
      FIELD_CAMERA_SetTargetOffset( camera, &v2 );
#ifdef DEBUG_PRINT_ENABLE
      OBATA_Printf( "TASK-TOFS: frame = %d, offset = 0x%x, 0x%x, 0x%x\n", 
          work->frame, FX_Whole(v2.x), FX_Whole(v2.y), FX_Whole(v2.z) );
#endif
    }
    // �I���`�F�b�N
    if( work->endFrame <= work->frame )
    {
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}

