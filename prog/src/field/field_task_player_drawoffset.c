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


//========================================================================================== 
// ���^�X�N���[�N
//========================================================================================== 
typedef struct
{
  u8             seq;       // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // ����Ώۂ̃t�B�[���h�}�b�v
  u16            nowFrame;  // ����t���[����
  u16            endFrame;  // �ő�t���[����
  VecFx32        vecMove;   // �ړ��x�N�g�� 

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

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), UpdateDrawOffset );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->nowFrame = 0;
  work->endFrame = frame;
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
  TASK_WORK*      work = (TASK_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player ); 
  VecFx32       offset;

  switch( work->seq )
  {
  case 0:
    // ���@�̕`��I�t�Z�b�g���X�V
    work->nowFrame++;
    CalcDrawOffset( &offset, work->nowFrame, &work->vecMove, work->endFrame );
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
