//////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N�Ǘ�
 * @file   field_task_manager.c
 * @author obata
 * @date   2009.11.13
 */
//////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field_task.h"
#include "field_task_manager.h"


//========================================================================================
// ���^�X�N�}�l�[�W��
//========================================================================================
struct _FIELD_TASK_MAN
{
  u8     maxTaskNum;  // �^�X�N�̍ő�Ǘ���
  FIELD_TASK** task;  // �^�X�N�z��
  FIELD_TASK** trig;  // �g���K�^�X�N�z��
};


//========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================
static void RegistTask( FIELD_TASK_MAN* man, u8 idx, FIELD_TASK* task, FIELD_TASK* trig );
static void DeleteTask( FIELD_TASK_MAN* man, u8 idx );
static void RunTask( FIELD_TASK_MAN* man, u8 idx );
static void TrigBoot( FIELD_TASK_MAN* man, u8 idx );
static void RemoveTask( FIELD_TASK_MAN* man, u8 idx );


//========================================================================================
// ���쐬�E�j��
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N�}�l�[�W�����쐬����
 *
 * @param max_task_num �^�X�N�̍ő�o�^��
 * @param heap_id      �g�p����q�[�vID
 *
 * @return �쐬�����}�l�[�W��
 */
//----------------------------------------------------------------------------------------
FIELD_TASK_MAN* FIELD_TASK_MAN_Create( u8 max_task_num, HEAPID heap_id )
{
  FIELD_TASK_MAN* man;
  man = GFL_HEAP_AllocMemory( heap_id, sizeof(FIELD_TASK_MAN) );
  man->maxTaskNum = max_task_num;
  man->task = GFL_HEAP_AllocClearMemory( heap_id, sizeof(FIELD_TASK*) * max_task_num );
  man->trig = GFL_HEAP_AllocClearMemory( heap_id, sizeof(FIELD_TASK*) * max_task_num );
  return man;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N�}�l�[�W����j������
 *
 * @param man �j������}�l�[�W��
 */
//----------------------------------------------------------------------------------------
void FIELD_TASK_MAN_Delete( FIELD_TASK_MAN* man )
{
  int i;

  // �^�X�N��j��
  for( i=0; i<man->maxTaskNum; i++ )
  {
    DeleteTask( man, i );
  }
  // �^�X�N�z���j��
  GFL_HEAP_FreeMemory( man->task );
  GFL_HEAP_FreeMemory( man->trig );
  // �{�̂�j��
  GFL_HEAP_FreeMemory( man );
}


//========================================================================================
// ������
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N�}�l�[�W���𓮂���
 *
 * @param man �������}�l�[�W��
 */
//----------------------------------------------------------------------------------------
void FIELD_TASK_MAN_Main( FIELD_TASK_MAN* man )
{
  int i;

  // �S�^�X�N�𓮂���
  for( i=0; i<man->maxTaskNum; i++ )
  {
    RunTask( man, i );
  }

  // �g���K�`�F�b�N
  for( i=0; i<man->maxTaskNum; i++ )
  {
    TrigBoot( man, i );
  } 

  // �I�������^�X�N��j��
  for( i=0; i<man->maxTaskNum; i++ )
  {
    RemoveTask( man, i );
  }
}


//========================================================================================
// ������
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��o�^����
 *
 * @param man  �o�^��}�l�[�W��
 * @param task �o�^����}�l�[�W��
 * @param trig �^�X�N���s�̃g���K�ƂȂ�^�X�N(�s�v�Ȃ�NULL���w��)
 */
//----------------------------------------------------------------------------------------
void FIELD_TASK_MAN_AddTask( FIELD_TASK_MAN* man, FIELD_TASK* task, FIELD_TASK* trig )
{
  int idx;

  // �󂫗v�f������
  for( idx=0; idx<man->maxTaskNum; idx++ )
  {
    // ���� ==> �o�^ ==> �N��
    if( man->task[idx] == NULL )
    { 
      RegistTask( man, idx, task, trig );
      if( trig == NULL ) FIELD_TASK_Boot( task );  // �g���K�ݒ薳��==> ���N��
      return;
    }
  }

  // �󂢂Ă��Ȃ�
  OBATA_Printf( "FIELD_TASK_MAN: task over flow\n" );
  FIELD_TASK_Delete( task );
}


//========================================================================================
// ���擾
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �ێ��^�X�N�̐����擾����
 *
 * @param man �^�X�N�̐��𒲂ׂ�}�l�[�W��
 *
 * @return ���쒆�^�X�N�̐�
 */
//----------------------------------------------------------------------------------------
u8 FIELD_TASK_MAN_GetTaskNum( const FIELD_TASK_MAN* man )
{
  int i;
  u8 num = 0;

  // �������Ă���^�X�N�̐����J�E���g
  for( i=0; i<man->maxTaskNum; i++ )
  {
    if( man->task[i] != NULL )
    {
      num++;
    }
  }
  return num;
}

//----------------------------------------------------------------------------------------
/**
 * @brief ���ׂẴ^�X�N���I���������ǂ������擾����
 *
 * @param man �}�l�[�W��
 *
 * @return �S�^�X�N���I�����Ă����� TRUE, �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------------------
extern BOOL FIELD_TASK_MAN_IsAllTaskEnd( const FIELD_TASK_MAN* man )
{
  return (FIELD_TASK_MAN_GetTaskNum(man) == 0);
}


//========================================================================================
// ������J�֐�
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��o�^����
 *
 * @param man �o�^��}�l�[�W��
 * @param idx �o�^��C���f�b�N�X
 * @param task �o�^����^�X�N
 * @param trig �o�^����^�X�N�̃g���K�ƂȂ�^�X�N
 */
//----------------------------------------------------------------------------------------
static void RegistTask( FIELD_TASK_MAN* man, u8 idx, FIELD_TASK* task, FIELD_TASK* trig )
{
  // �w��C���f�b�N�X�͋󂢂Ă��Ȃ�
  if( man->task[idx] != NULL ) return;

  // �o�^
  man->task[idx] = task;
  man->trig[idx] = trig;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N��j������
 *
 * @param man �j���Ώۃ^�X�N��ێ�����}�l�[�W��
 * @param idx �j������^�X�N���w��
 */
//----------------------------------------------------------------------------------------
static void DeleteTask( FIELD_TASK_MAN* man, u8 idx )
{
  // �w��^�X�N�������Ă��Ȃ�
  if( man->task[idx] == NULL ) return;

  // �j��
  FIELD_TASK_Delete( man->task[idx] );
  man->task[idx] = NULL;
  man->trig[idx] = NULL;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �^�X�N�𓮂���
 *
 * @param man �������^�X�N��ێ�����}�l�[�W��
 * @param idx �������^�X�N���w��
 */
//----------------------------------------------------------------------------------------
static void RunTask( FIELD_TASK_MAN* man, u8 idx )
{
  // �w��^�X�N�������Ă��Ȃ�
  if( man->task[idx] == NULL ) return;

  // ���s
  FIELD_TASK_Main( man->task[idx] );
}

//----------------------------------------------------------------------------------------
/**
 * @brief �w��^�X�N�̃g���K�𒲂ׂċN������
 *
 * @param man �`�F�b�N����^�X�N��ێ�����}�l�[�W��
 * @param idx �`�F�b�N�Ώۃ^�X�N���w��
 */
//----------------------------------------------------------------------------------------
static void TrigBoot( FIELD_TASK_MAN* man, u8 idx )
{
  // �w��^�X�N�������Ă��Ȃ�
  if( man->task[idx] == NULL ) return; 
  // �g���K�������Ă��Ȃ�
  if( man->trig[idx] == NULL ) return;
  // �N���҂��łȂ�
  {
    FIELD_TASK_STATE state = FIELD_TASK_GetState( man->task[idx] );
    if( state != FIELD_TASK_STATE_WAIT ) return;
  } 
  // �g���K��������Ă��Ȃ�
  {
    FIELD_TASK_STATE state = FIELD_TASK_GetState( man->trig[idx] );
    if( state != FIELD_TASK_STATE_END ) return;
  }

  // �N��
  FIELD_TASK_Boot( man->task[idx] );
}  

//----------------------------------------------------------------------------------------
/**
 * @brief �w��^�X�N���I�����Ă�����폜����
 *
 * @param man �폜����^�X�N��ێ����Ă���}�l�[�W��
 * @param idx �폜����^�X�N���w��
 */
//----------------------------------------------------------------------------------------
static void RemoveTask( FIELD_TASK_MAN* man, u8 idx )
{
  // �w��^�X�N�������Ă��Ȃ�
  if( man->task[idx] == NULL ) return;

  // �I�����Ă�����j��
  if( FIELD_TASK_IsEnd( man->task[idx] ) == TRUE )
  {
    DeleteTask( man, idx );
  }
}
