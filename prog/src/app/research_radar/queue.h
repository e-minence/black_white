//////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �L���[
 * @file   queue.h
 * @author obata
 * @date   2010.02.20
 */
//////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>


//====================================================================================
// ���L���[�̕s���S�`
//====================================================================================
typedef struct _QUEUE QUEUE;


//====================================================================================
// ���L���[�̑���֐�
//====================================================================================

// �����E�j��
extern QUEUE* QUEUE_Create( int queueSize, HEAPID heapID );
extern void QUEUE_Delete( QUEUE* queue );

// �G���L���[�E�f�L���[
extern void QUEUE_EnQueue( QUEUE* queue, int val );
extern int QUEUE_DeQueue ( QUEUE* queue );

// �N���A
extern void QUEUE_Clear( QUEUE* queue );

// �`�F�b�N
extern BOOL QUEUE_IsEmpty( const QUEUE* queue ); // �󂩂ǂ���
extern BOOL QUEUE_IsFull ( const QUEUE* queue ); // ���^�����ǂ���

// �f�[�^�擾
extern int QUEUE_GetDataNum( const QUEUE* queue ); // �ێ����Ă���f�[�^�̌�
extern int QUEUE_PeekData( const QUEUE* queue, int no ); // �w�肵�����Ԃɂ���f�[�^
