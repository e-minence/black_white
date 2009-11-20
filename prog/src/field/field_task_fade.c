/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N( ��ʃt�F�[�h )
 * @file   field_task_fade.c
 * @author obata
 * @date   2009.11.20
 */
/////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_fade.h"


//===============================================================================================
// ���^�X�N���[�N
//===============================================================================================
typedef struct
{
  // ����Ώۃt�B�[���h�}�b�v
  FIELDMAP_WORK* fieldmap;

  int mode;      // �t�F�[�h���[�h�iGFL_FADE_MASTER_BRIGHT_BLACKOUT/_WHITEOUT)
  int startEvy;  // �X�^�[�g�P�x�i0�`16�j
  int endEvy;    // �G���h�P�x�i0�`16�j
  int wait;      // �t�F�[�h�X�s�[�h

} TASK_WORK;


//===============================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//===============================================================================================
static FIELD_TASK_RETVAL FadeRequest( void* wk );


//-----------------------------------------------------------------------------------------------
/**
 * @brief �t�B�[���h�^�X�N���쐬����( ��ʃt�F�[�h���N�G�X�g���� )
 *
 * @param fieldmap  ����Ώۃt�B�[���h�}�b�v
 * @param mode	    �t�F�[�h���[�h�iGFL_FADE_MASTER_BRIGHT_BLACKOUT/_WHITEOUT)
 * @param	start_evy	�X�^�[�g�P�x�i0�`16�j
 * @param	end_evy		�G���h�P�x�i0�`16�j
 * @param	wait		  �t�F�[�h�X�s�[�h
 *
 * @return �쐬�����^�X�N
 */
//-----------------------------------------------------------------------------------------------
FIELD_TASK* FIELD_TASK_Fade( FIELDMAP_WORK* fieldmap, 
                             int mode, int start_evy, int end_evy, int wait )
{
  FIELD_TASK* task;
  TASK_WORK* work;
  HEAPID heap_id = FIELDMAP_GetHeapID( fieldmap );

  // ����
  task = FIELD_TASK_Create( heap_id, sizeof(TASK_WORK), FadeRequest );
  // ������
  work = FIELD_TASK_GetWork( task );
  work->fieldmap = fieldmap;
  work->mode     = mode;
  work->startEvy = start_evy;
  work->endEvy   = end_evy;
  work->wait     = wait; 
  return task;
}


//===============================================================================================
// ���^�X�N�����֐�
//===============================================================================================

//-----------------------------------------------------------------------------------------------
/**
 * @brief ��ʃt�F�[�h���N�G�X�g
 */
//-----------------------------------------------------------------------------------------------
static FIELD_TASK_RETVAL FadeRequest( void* wk )
{ 
  TASK_WORK* work = (TASK_WORK*)wk;

  // ���N�G�X�g
  GFL_FADE_SetMasterBrightReq( work->mode, work->startEvy, work->endEvy, work->wait );
  return FIELD_TASK_RETVAL_FINISH;
}
