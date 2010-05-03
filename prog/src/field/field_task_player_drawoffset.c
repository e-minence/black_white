////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(���@�̕`��I�t�Z�b�g�ړ�)
 * @file   field_task_player_drawoffset.h
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_player_drawoffset.h"
#include "field_player.h"

//#define DEBUG_EVENT_ON

typedef enum {
  TRANS_TYPE_PLUS = 0,
  TRANS_TYPE_MINUS,
}TRANS_TYPE;

//========================================================================================== 
// ���^�X�N���[�N
//========================================================================================== 
typedef struct
{
  u8             seq;       // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // ����Ώۂ̃t�B�[���h�}�b�v
  TRANS_TYPE    transType;
  u16            nowFrame;  // ����t���[����
  u16            endFrame;  // �ő�t���[����
  VecFx32        vecMove;   // �ړ��x�N�g�� 
  MMDL*          mmdl;      // ����Ώۂ̓��샂�f��

} TASK_WORK;

//========================================================================================== 
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================== 
static void CalcDrawOffset( VecFx32* now, u16 nowFrame, VecFx32* max, u16 maxFrame );
static FIELD_TASK_RETVAL UpdateDrawOffset( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief �v���C���[�̕`��I�t�Z�b�g�ړ��^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param move     �ړ��x�N�g��
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_TransDrawOffset( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), UpdateDrawOffset );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->mmdl     = FIELD_PLAYER_GetMMdl( player ); 
  if (frame >= 0 )
  {
    work->transType = TRANS_TYPE_PLUS;
    work->nowFrame = 0;
    work->endFrame = frame;
  } else {
    work->transType = TRANS_TYPE_MINUS;
    work->nowFrame = 0;
    work->endFrame = -frame;
  }
  VEC_Set( &work->vecMove, vec->x, vec->y, vec->z );

  return task;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����샂�f���̃I�t�Z�b�g�ړ��^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param move     �ړ��x�N�g��
 * @param mmdl     ����Ώۂ̓��샂�f��
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_TransDrawOffsetEX( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec, MMDL* mmdl )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // �^�X�N�𐶐�
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), UpdateDrawOffset );

  // �^�X�N���[�N��������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->mmdl     = mmdl;

  if (frame >= 0 ) {
    work->transType = TRANS_TYPE_PLUS;
    work->nowFrame = 0;
    work->endFrame = frame;
  } else {
    work->transType = TRANS_TYPE_MINUS;
    work->nowFrame = 0;
    work->endFrame = -frame;
  }
  VEC_Set( &work->vecMove, vec->x, vec->y, vec->z );

  return task;
}


//========================================================================================== 
// ������J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ�����`��I�t�Z�b�g�l���v�Z����
 *
 * @param now       �v�Z���ʂ̊i�[��
 * @param nowFrame  ���݂̃t���[����
 * @param max       �ő�ړ��x�N�g��
 * @param maxFrame  �ő�t���[����
 */
//------------------------------------------------------------------------------------------
static void CalcDrawOffset( VecFx32* now, u16 nowFrame, VecFx32* max, u16 maxFrame )
{
  // ���`���
  fx32 rate = FX_F32_TO_FX32((float)nowFrame / (float)maxFrame);
  now->x = FX_Mul( max->x, rate );
  now->y = FX_Mul( max->y, rate );
  now->z = FX_Mul( max->z, rate );
#ifdef DEBUG_EVENT_ON
  OS_Printf("NOW/MAX=%03d/%03d\n", nowFrame, maxFrame );
#endif
} 


//========================================================================================== 
// ���^�X�N�����֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �`��I�t�Z�b�g�X�V����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL UpdateDrawOffset( void* wk )
{
  TASK_WORK* work = (TASK_WORK*)wk;
  MMDL*      mmdl = work->mmdl;
  VecFx32    offset;

  switch( work->seq )
  {
  case 0:
    // ���@�̕`��I�t�Z�b�g���X�V
    work->nowFrame++;
    if (work->transType == TRANS_TYPE_PLUS)
    {
      CalcDrawOffset( &offset, work->nowFrame, &work->vecMove, work->endFrame );
    } else if (work->transType == TRANS_TYPE_MINUS ) {
      u16 now = work->endFrame - work->nowFrame;
      CalcDrawOffset( &offset, now, &work->vecMove, work->endFrame );
    }
    MMDL_SetVectorDrawOffsetPos( mmdl, &offset );
    // �I���`�F�b�N
    if( work->endFrame <= work->nowFrame )
    { 
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE; 
} 
