//////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �L���[
 * @file   queue.c
 * @author obata
 * @date   2010.02.20
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "queue.h"


//====================================================================================
// ���L���[
//====================================================================================
struct _QUEUE
{
  int* data;  // �f�[�^�z��
  int  size;  // �z��T�C�Y
  int  head;  // �擪�v�f�̃C���f�b�N�X
  int  tail;  // �����v�f�̃C���f�b�N�X( �ǉ��ʒu ) 
};


//====================================================================================
// ���O������J�֐�
//====================================================================================
static void InitQueue( QUEUE* queue ); // �L���[������������ 
static void CreateDataArea( QUEUE* queue, int size, HEAPID heapID ); // �f�[�^�̈���쐬����
static void DeleteDataArea( QUEUE* queue ); // �f�[�^�̈��j������
static void EnQueue( QUEUE* queue, int val ); // �G���L���[����
static int DeQueue( QUEUE* queue ); // �f�L���[����
static void ClearQueue( QUEUE* queue ); // �L���[����ɂ���
static BOOL QueueIsEmpty( const QUEUE* queue ); // �L���[���󂩂ǂ����𔻒肷��
static BOOL QueueIsFull( const QUEUE* queue ); // �L���[�����^�����ǂ����𔻒肷��
static int GetDataNum( const QUEUE* queue ); // �ێ����Ă���f�[�^�̌����擾����
static int PeekData( const QUEUE* queue, int no ); // �f�[�^��`������

//====================================================================================
// ���O�����J�֐�
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �L���[���쐬����
 *
 * @param queueSize �L���[�T�C�Y
 * @param heapID    �g�p����q�[�vID
 *
 * @return ���������L���[
 */
//------------------------------------------------------------------------------------
QUEUE* QUEUE_Create( int queueSize, HEAPID heapID )
{
  QUEUE* queue;

  // ����
  queue = GFL_HEAP_AllocMemory( heapID, sizeof(QUEUE) );

  // ������
  InitQueue( queue );
  CreateDataArea( queue, queueSize, heapID );

  return queue;
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[��j������
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
void QUEUE_Delete( QUEUE* queue )
{
  DeleteDataArea( queue );
  GFL_HEAP_FreeMemory( queue );
}

//------------------------------------------------------------------------------------
/**
 * @brief �G���L���[����
 *
 * @param queue
 * @param val   �ǉ�����l
 */
//------------------------------------------------------------------------------------
void QUEUE_EnQueue( QUEUE* queue, int val )
{
  EnQueue( queue, val );
}

//------------------------------------------------------------------------------------
/**
 * @brief �f�L���[����
 *
 * @param queue
 *
 * @return �L���[������o�����l
 */
//------------------------------------------------------------------------------------
int QUEUE_DeQueue( QUEUE* queue )
{
  return DeQueue( queue );
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[����ɂ���
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
void QUEUE_Clear( QUEUE* queue )
{
  ClearQueue( queue );
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[���󂩂ǂ����𔻒肷��
 *
 * @param queue
 *
 * @return �L���[����Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------
BOOL QUEUE_IsEmpty( const QUEUE* queue )
{
  return QueueIsEmpty( queue );
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[�����^�����ǂ����𔻒肷��
 *
 * @param queue
 *
 * @return �L���[�����^���Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------
BOOL QUEUE_IsFull( const QUEUE* queue )
{
  return QueueIsFull( queue );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ێ����Ă���f�[�^�̌����擾����
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
int QUEUE_GetDataNum( const QUEUE* queue )
{
  return GetDataNum( queue );
}

//------------------------------------------------------------------------------------
/**
 * @brief �f�[�^��`������
 *
 * @param queue
 * @param no    ���Ԗڂ̃f�[�^�����邩
 *
 * @return �w�肵���ʒu�ɂ���f�[�^
 */
//------------------------------------------------------------------------------------
int QUEUE_PeekData( const QUEUE* queue, int no )
{
  return PeekData( queue, no );
}


//====================================================================================
// ���O������J�֐�
//====================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �L���[������������ 
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
static void InitQueue( QUEUE* queue )
{
  queue->data = NULL;
  queue->head = 0;
  queue->tail = 0;
}

//------------------------------------------------------------------------------------
/**
 * @brief �f�[�^�̈���쐬����
 *
 * @param queue
 * @param size   �L���[�T�C�Y
 * @param heapID �g�p����q�[�vID
 */
//------------------------------------------------------------------------------------
static void CreateDataArea( QUEUE* queue, int size, HEAPID heapID )
{
  // ���d����
  GF_ASSERT( queue->data == NULL );

  queue->data = GFL_HEAP_AllocMemory( heapID, sizeof(int) * size );
  queue->size = size;
}

//------------------------------------------------------------------------------------
/**
 * @brief �f�[�^�̈��j������
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
static void DeleteDataArea( QUEUE* queue )
{
  // ��������Ă��Ȃ�
  GF_ASSERT( queue->data != NULL );

  GFL_HEAP_FreeMemory( queue->data );
}

//------------------------------------------------------------------------------------
/**
 * @brief �G���L���[����
 *
 * @param queue
 * @param val   �ǉ�����l
 */
//------------------------------------------------------------------------------------
static void EnQueue( QUEUE* queue, int val )
{
  // �I�[�o�[�t���[
  GF_ASSERT( QueueIsFull(queue) == FALSE );

  // �l��ǉ�
  queue->data[ queue->tail ] = val;

  // �����ʒu���X�V
  queue->tail = ( queue->tail + 1 ) % queue->size;
}

//------------------------------------------------------------------------------------
/**
 * @brief �f�L���[����
 *
 * @param queue
 *
 * @return �L���[������o�����l
 */
//------------------------------------------------------------------------------------
static int DeQueue( QUEUE* queue )
{
  int val;

  // �L���[����
  GF_ASSERT( QueueIsEmpty(queue) == FALSE );

  // �擪�v�f���擾
  val = queue->data[ queue->head ];

  // �擪�ʒu���X�V
  queue->head = ( queue->head + 1 ) % queue->size;

  return val;
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[����ɂ���
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
static void ClearQueue( QUEUE* queue )
{
  queue->head = 0;
  queue->tail = 0;
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[���󂩂ǂ����𔻒肷��
 *
 * @param queue
 *
 * @return �L���[����Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------
static BOOL QueueIsEmpty( const QUEUE* queue )
{
  // �L���[����������Ă��Ȃ�
  GF_ASSERT( queue->data );

  return ( queue->head == queue->tail );
}

//------------------------------------------------------------------------------------
/**
 * @brief �L���[�����^�����ǂ����𔻒肷��
 *
 * @param queue
 *
 * @return �L���[�����^���Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//------------------------------------------------------------------------------------
static BOOL QueueIsFull( const QUEUE* queue )
{
  int next;

  // �L���[����������Ă��Ȃ�
  GF_ASSERT( queue->data );

  next = ( queue->tail + 1 ) % queue->size; 
  return ( next == queue->head );
}

//------------------------------------------------------------------------------------
/**
 * @brief �ێ����Ă���f�[�^�̌����擾����
 *
 * @param queue
 */
//------------------------------------------------------------------------------------
static int GetDataNum( const QUEUE* queue )
{
  int num;

  // �L���[����������Ă��Ȃ�
  GF_ASSERT( queue->data );

  num = queue->tail - queue->head;

  // tail �̕����傫���C���f�b�N�X�ɂ���ꍇ
  if( num < 0 )
  {
    num += queue->size;
  }
  
  return num;
}

//------------------------------------------------------------------------------------
/**
 * @brief �f�[�^��`������
 *
 * @param queue
 * @param no    ���Ԗڂ̃f�[�^�����邩
 *
 * @return �w�肵���ʒu�ɂ���f�[�^
 */
//------------------------------------------------------------------------------------
static int PeekData( const QUEUE* queue, int no )
{
  int index;

  // �f�[�^�ʒu�w��G���[
  GF_ASSERT( no < GetDataNum(queue) );

  // �w�肳�ꂽ�ԍ��̃f�[�^�̃C���f�b�N�X���v�Z
  index = ( queue->head + no ) % queue->size;

  return queue->data[ index ];
}
