////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(���@�̗�������)
 * @file   field_task_player_fall.c
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_player_fall.h"
#include "sound/pm_sndsys.h"
#include "field_player.h"
#include "fldeff_kemuri.h"


//========================================================================================== 
// ���^�X�N���[�N
//========================================================================================== 
typedef struct {

  u8             seq;       // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // ����Ώۂ̃t�B�[���h�}�b�v
  u16            frame;     // ����t���[����
  u16            endFrame;  // �ő�t���[����
  float          moveDist;  // �ړ�����
  MMDL*          mmdl;      // ����Ώۂ̓��샂�f��

} TASK_WORK;


//========================================================================================== 
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================== 
static void CalcDrawOffset( VecFx32* vec, float frame, float max_frame, float max_y );
static FIELD_TASK_RETVAL FallPlayer( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief �v���C���[�̗��������^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param mmdl     ����Ώۂ̓��샂�f��
 * @param frame    �^�X�N����t���[����
 * @param dist     �ړ�����
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_PlayerFall( FIELDMAP_WORK* fieldmap, MMDL* mmdl, int frame, int dist )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), FallPlayer );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->frame    = 0;
  work->endFrame = frame;
  work->moveDist = dist;
	work->mmdl     = mmdl;

  return task;
}


//========================================================================================== 
// ������J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief y���W�̕`��I�t�Z�b�g���Z�o����( ���� ) 
 *
 * @param vec       �v�Z���ʂ̊i�[��
 * @param frame     ���݂̃t���[����
 * @param max_frame �ő�t���[����
 * @param max_y     �ő�I�t�Z�b�g
 */
//------------------------------------------------------------------------------------------
static void CalcDrawOffset( VecFx32* vec, float frame, float max_frame, float max_y )
{
  // y = ax^2
  float x = frame;
  float a = max_y / (max_frame * max_frame);
  float y = max_y - (a * x * x);

  vec->x = 0;
  vec->y = FX_F32_TO_FX32( y );
  vec->z = 0;
} 


//========================================================================================== 
// ���^�X�N�����֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̗�������
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL FallPlayer( void* wk )
{
  TASK_WORK*      work = (TASK_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  MMDL*           mmdl = work->mmdl;
  VecFx32       offset;

  switch( work->seq ) {
  case 0:
    // ��l���̌�����ݒ�
    MMDL_SetAcmd( mmdl, AC_DIR_D ); 
    // �`��I�t�Z�b�g������
    CalcDrawOffset( &offset, work->frame, work->endFrame, work->moveDist );
    MMDL_SetVectorDrawOffsetPos( mmdl, &offset );
    // SE�Đ�(������)
    PMSND_PlaySE( SEQ_SE_FLD_16 );
    work->seq++;
    break;
  case 1:
    // ���@�̕`��I�t�Z�b�g���X�V
    work->frame++;
    CalcDrawOffset( &offset, work->frame, work->endFrame, work->moveDist );
    MMDL_SetVectorDrawOffsetPos( mmdl, &offset );
		// �����I��
    if( work->endFrame <= work->frame )  { 
      // �������o��
      FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
      FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
      // SE�Đ�(���n��)
      PMSND_PlaySE( SEQ_SE_FLD_17 );
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE; 
} 
