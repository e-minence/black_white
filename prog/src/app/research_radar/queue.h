////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �L���[
 * @file   queue.h
 * @author obata
 * @date   2010.02.20
 */
////////////////////////////////////////////////////////////////////////////
#include <gflib.h>


//==========================================================================
// ���L���[�̕s���S�^
//==========================================================================
typedef struct _QUEUE QUEUE;


//==========================================================================
// ���L���[�̑���֐�
//========================================================================== 
// �L���[�𐶐�����
extern QUEUE* QUEUE_Create( int queueSize, HEAPID heapID );
// �L���[��j������
extern void QUEUE_Delete( QUEUE* queue ); 
// �f�[�^���G���L���[����
extern void QUEUE_EnQueue( QUEUE* queue, int val );
// �f�[�^���f�L���[����
extern int QUEUE_DeQueue ( QUEUE* queue ); 
// �L���[���N���A����
extern void QUEUE_Clear( QUEUE* queue ); 
// �L���[�̏�Ԃ��`�F�b�N����
extern BOOL QUEUE_IsEmpty( const QUEUE* queue ); // �󂩂ǂ���
extern BOOL QUEUE_IsFull ( const QUEUE* queue ); // ���^�����ǂ���
extern int QUEUE_GetDataNum( const QUEUE* queue ); // �ێ����Ă���f�[�^�̌�
// �w�肵�����Ԃɂ���f�[�^������
extern int QUEUE_PeekData( const QUEUE* queue, int no );
