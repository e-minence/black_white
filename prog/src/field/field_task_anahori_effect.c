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

  FIELDMAP_WORK* fieldmap;  // ����Ώۂ̃t�B�[���h�}�b�v
  FLDEFF_CTRL*   fectrl;    // �t�B�[���h�G�t�F�N�g
  MMDL*          mmdl;      // ����Ώۂ̓��샂�f��
  u8             seq;       // �V�[�P���X
  u16            now_frame; // ����t���[����
  u16            end_frame; // �ŏI�t���[����
  u16            interval;  // �G�t�F�N�g�\���Ԋu

} TASK_WORK;


//========================================================================================== 
// ��prototype
//========================================================================================== 
static FIELD_TASK_RETVAL AnahoriEffectUpdate( void* wk );


//========================================================================================== 
// ��public function
//========================================================================================== 

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
  HEAPID        heap_id = FIELDMAP_GetHeapID( fieldmap );
  FIELD_PLAYER* player  = FIELDMAP_GetFieldPlayer( fieldmap );

  FIELD_TASK* task;
  TASK_WORK* work;

  // �^�X�N�𐶐�
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), AnahoriEffectUpdate );

  // �^�X�N���[�N��������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap  = fieldmap;
  work->fectrl    = FIELDMAP_GetFldEffCtrl( fieldmap );
  work->mmdl      = FIELD_PLAYER_GetMMdl( player ); 
  work->seq       = 0;
  work->now_frame = 0;
  work->end_frame = frame;
  work->interval  = interval;

  return task;
}


//========================================================================================== 
// ��private function
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief ���@��G�t�F�N�g �^�X�N�X�V����
 */
//------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL AnahoriEffectUpdate( void* wk )
{
  TASK_WORK* work = (TASK_WORK*)wk;

  switch( work->seq ) {
  case 0:
    // ��ӂ��G�t�F�N�g�\��
    if( work->now_frame % work->interval == 0 ) {
      FLDEFF_IWAKUDAKI_SetMMdl( work->mmdl, work->fectrl );
    }
    // �I���`�F�b�N
    if( work->end_frame <= work->now_frame++ ) { 
      return FIELD_TASK_RETVAL_FINISH;
    }
    break;
  }
  return FIELD_TASK_RETVAL_CONTINUE; 
} 

