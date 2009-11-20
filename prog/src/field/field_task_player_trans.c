/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(���@�̕��s�ړ�����)
 * @file   field_task_player_trans.c
 * @author obata
 * @date   2009.11.18
 */
/////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_player_trans.h"


//===============================================================================================
// ���^�X�N���[�N
//===============================================================================================
typedef struct
{
  u8                  seq;  // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // ����Ώۃt�B�[���h�}�b�v
  int               frame;  // ���݂̃t���[����
  int            endFrame;  // �ŏI�t���[����
  VecFx32        startPos;  // �ړ��J�n�ʒu
  VecFx32          endPos;  // �ŏI�ʒu

} TASK_WORK;


//===============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//===============================================================================================
static FIELD_TASK_RETVAL TransPlayer( void* wk );


//-----------------------------------------------------------------------------------------------
/**
 * @brief �v���C���[�̈ړ��^�X�N���쐬����
 *
 * @param fieldmap ����Ώۃt�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param pos      �ړ�����W
 */
//-----------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_TransPlayer( FIELDMAP_WORK* fieldmap, int frame, VecFx32* pos )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), TransPlayer );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->frame    = 0;
  work->endFrame = frame;
  VEC_Set( &work->endPos, pos->x, pos->y, pos->z );

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
static FIELD_TASK_RETVAL TransPlayer( void* wk )
{
  TASK_WORK*      work = (TASK_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player );

  switch( work->seq )
  {
  case 0:
    FIELD_PLAYER_GetPos( player, &work->startPos );
    work->seq++;
    break;
  case 1:
    // ���@�̍��W���X�V
    work->frame++;
    {
      VecFx32 v0, v1, v2;
      fx32 w1 = FX_F32_TO_FX32( (float)work->frame / (float)work->endFrame );
      fx32 w0 = FX32_ONE - w1;
      GFL_CALC3D_VEC_MulScalar( &work->startPos, w0, &v0 );
      GFL_CALC3D_VEC_MulScalar( &work->endPos,   w1, &v1 );
      VEC_Add( &v0, &v1, &v2 );
      FIELD_PLAYER_SetPos( player, &v2 );   // �ړ�
      MMDL_UpdateCurrentHeight( mmdl );   // �ڒn
    }
    if( work->endFrame <= work->frame )
    {
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE;
}
