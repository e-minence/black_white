///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���X�g��ʂ̕��A�f�[�^
 * @file   research_list_recovery.c
 * @author obata
 * @date   2010.05.21
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "research_list_def.h"
#include "research_list_recovery.h"

#include "topic_id.h"  // for TOPIC_ID_xxxx


// ���X�g��ʕ��A�f�[�^
struct _RESEARCH_RADAR_LIST_RECOVERY_DATA {

  u8  topicCursorPos;  // TOPIC_ID_xxxx
  int scrollCursorPos;
  int scrollValue;
};


/**
 * @brief ���A�f�[�^�̃��[�N�𐶐�����
 */
RRL_RECOVERY_DATA* RRL_RECOVERY_CreateData( HEAPID heapID )
{
  RRL_RECOVERY_DATA* data;

  data = GFL_HEAP_AllocClearMemory( heapID, sizeof(RRL_RECOVERY_DATA) ); 
  return data;
}

/**
 * @brief ���A�f�[�^�̃��[�N��j������
 */
void RRL_RECOVERY_DeleteData( RRL_RECOVERY_DATA* data )
{
  GFL_HEAP_FreeMemory( data );
}

/**
 * @brief ���A�f�[�^������������
 */
void RRL_RECOVERY_InitData( RRL_RECOVERY_DATA* data )
{
  data->topicCursorPos  = TOPIC_ID_STYLE;
  data->scrollCursorPos = MIN_SCROLL_CURSOR_POS;
  data->scrollValue     = MIN_SCROLL_VALUE;
}

/**
 * @brief �������ڃJ�[�\���̈ʒu���擾����
 */
u8 RRL_RECOVERY_GetTopicCursorPos( const RRL_RECOVERY_DATA* data )
{
  return data->topicCursorPos;
}

/**
 * @brief �X�N���[���J�[�\���̈ʒu���擾����
 */
int RRL_RECOVERY_GetScrollCursorPos( const RRL_RECOVERY_DATA* data )
{
  return data->scrollCursorPos;
}

/**
 * @brief �X�N���[�������l���擾����
 */
int RRL_RECOVERY_GetScrollValue( const RRL_RECOVERY_DATA* data )
{
  return data->scrollValue;
}

/**
 * @brief �������ڃJ�[�\���̈ʒu��ݒ肷��
 */
void RRL_RECOVERY_SetTopicCursorPos( RRL_RECOVERY_DATA* data, u8 topicCursorPos )
{
  data->topicCursorPos = topicCursorPos;
}

/**
 * @brief �X�N���[���J�[�\���̈ʒu��ݒ肷��
 */
void RRL_RECOVERY_SetScrollCursorPos( RRL_RECOVERY_DATA* data, int scrollCursorPos )
{
  data->scrollCursorPos = scrollCursorPos;
}

/**
 * @brief �X�N���[�������l��ݒ肷��
 */
void RRL_RECOVERY_SetScrollValue( RRL_RECOVERY_DATA* data, int scrollValue )
{
  data->scrollValue = scrollValue;
}
