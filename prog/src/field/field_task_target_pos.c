////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N (�J�����̃^�[�Q�b�g���W)
 * @file   field_task_target_pos.c
 * @author obata
 * @date   2010.04.03
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field_task.h"
#include "fieldmap.h"
#include "field/field_camera.h"
#include "field_task_target_pos.h"

//#define DEBUG_EVENT_ON


//==========================================================================================
// ���^�X�N���[�N
//==========================================================================================
typedef struct {

  u8             seq;       // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // �����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;     // �o�߃t���[����
  u16            endFrame;  // �I���t���[��
  VecFx32        startPos;  // �������W
  VecFx32        endPos;    // �ŏI���W
  VecFx32        moveVec;   // �ړ������x�N�g��
  FIELD_CAMERA_MODE cam_mode;

} TARGET_WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static FIELD_TASK_RETVAL TargetPosMove( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief �J�����𓙑��Y�[��������^�X�N���쐬����
 *
 * @param fieldmap �����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �Y�[�������܂ł̃t���[����
 * @param dist     �ŏI�I�ȋ���
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_CameraTargetMove( FIELDMAP_WORK* fieldmap, u16 frame, const VecFx32* startPos, const VecFx32* endPos )
{
  FIELD_TASK* task;
  TARGET_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TARGET_WORK), TargetPosMove );

  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq       = 0;
  work->fieldmap  = fieldmap;
  work->frame     = 0;
  work->endFrame  = frame;
  work->startPos = *startPos;
  work->endPos    = *endPos;
  VEC_Subtract( &work->endPos, &work->startPos, &work->moveVec );

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
static FIELD_TASK_RETVAL TargetPosMove( void* wk )
{
  TARGET_WORK*  work = (TARGET_WORK*)wk;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( work->seq ) {
  case 0:
    work->cam_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    work->seq++;
    break;
  case 1:
    work->frame++;
    // ���`�⊮
    { 
      VecFx32 pos;
      float t = work->frame / (float)work->endFrame;
      VEC_MultAdd( FX_F32_TO_FX32(t), &work->moveVec, &work->startPos, &pos );
      FIELD_CAMERA_SetTargetPos( camera, &pos );
#ifdef DEBUG_EVENT_ON
      OBATA_Printf( "TASK-TPOS: frame = %d, target = 0x%x, 0x%x, 0x%x\n", 
          work->frame, FX_Whole(pos.x), FX_Whole(pos.y), FX_Whole(pos.z) );
#endif
    }
    if( work->endFrame <= work->frame ) {
      FIELD_CAMERA_ChangeMode( camera, work->cam_mode );
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}
