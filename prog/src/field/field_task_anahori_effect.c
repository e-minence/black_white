////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N ( ���Ȃق�G�t�F�N�g )
 * @file   field_task_anahori_effect.h
 * @author obata
 * @date   2010.05.03
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_player.h"
#include "fldeff_iwakudaki.h"
#include "field_task_anahori_effect.h"

//#define DEBUG_PRINT_ON


//========================================================================================== 
// ���^�X�N���[�N
//========================================================================================== 
typedef struct {

  u8             seq;       // �V�[�P���X
  FIELDMAP_WORK* fieldmap;  // ����Ώۂ̃t�B�[���h�}�b�v
  FLDEFF_CTRL*   fectrl;    // �t�B�[���h�G�t�F�N�g
  u16            nowFrame;  // ����t���[����
  u16            endFrame;  // �ő�t���[����
  u16            interval;  // �G�t�F�N�g�\���Ԋu
  MMDL*          mmdl;      // ����Ώۂ̓��샂�f��

} TASK_WORK;


//========================================================================================== 
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================== 
static FIELD_TASK_RETVAL AnahoriEffectUpdate( void* wk );


//------------------------------------------------------------------------------------------
/**
 * @brief ���@��G�t�F�N�g�\���^�X�N�𐶐�����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param mmdl     ����Ώۂ̓��샂�f��
 * @param frame    �^�X�N����t���[����
 * @param interval �G�t�F�N�g�\���Ԋu
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_AnahoriEffect( FIELDMAP_WORK* fieldmap, MMDL* mmdl, int frame, int interval )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );

  // �^�X�N�𐶐�
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), AnahoriEffectUpdate );

  // �^�X�N���[�N��������
  work = FIELD_TASK_GetWork( task );
  work->seq      = 0;
  work->fieldmap = fieldmap;
  work->fectrl   = FIELDMAP_GetFldEffCtrl( fieldmap );
  work->mmdl     = FIELD_PLAYER_GetMMdl( player ); 
  work->nowFrame = 0;
  work->endFrame = frame;
  work->interval = interval;

  return task;
}


//========================================================================================== 
// ������J�֐�
//========================================================================================== 


//========================================================================================== 
// ���^�X�N�����֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief ���@��G�t�F�N�g�X�V����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL AnahoriEffectUpdate( void* wk )
{
  TASK_WORK* work = (TASK_WORK*)wk;

  switch( work->seq ) {
  case 0:
    // ��ӂ��G�t�F�N�g�\��
    if( work->nowFrame % work->interval == 0 ) {
      FLDEFF_IWAKUDAKI_SetMMdl( work->mmdl, work->fectrl );
    }
    // �I���`�F�b�N
    if( work->endFrame <= work->nowFrame++ ) { 
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE; 
} 

