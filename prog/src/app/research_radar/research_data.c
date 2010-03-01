//////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �����f�[�^
 * @file   research_data.c
 * @author obata
 * @date   2010.03.01
 */
//////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "research_data.h"


//============================================================================
// ���f�[�^�擾�֐�
//============================================================================

//----------------------------------------------------------------------------
/**
 * @brief ��������ID���擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u8 RESEARCH_DATA_GetTopicID( const RESEARCH_DATA* data )
{
  return data->topicID;
}

//----------------------------------------------------------------------------
/**
 * @brief ����ID���擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u8 RESEARCH_DATA_GetQuestionID_byIndex( const RESEARCH_DATA* data, u8 qIdx )
{
  GF_ASSERT( qIdx < QUESTION_NUM_PER_TOPIC ); // �C���f�b�N�X�G���[

  return data->questionData[ qIdx ].ID;
}

//----------------------------------------------------------------------------
/**
 * @brief �񓚂̐����擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u8 RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex( const RESEARCH_DATA* data, u8 qIdx )
{
  GF_ASSERT( qIdx < QUESTION_NUM_PER_TOPIC ); // �C���f�b�N�X�G���[

  return data->questionData[ qIdx ].answerNum;
}

//----------------------------------------------------------------------------
/**
 * @brief �����̉񓚐l�����擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u16 RESEARCH_DATA_GetTodayCountOfQuestion_byIndex( const RESEARCH_DATA* data, u8 qIdx )
{
  GF_ASSERT( qIdx < QUESTION_NUM_PER_TOPIC ); // �C���f�b�N�X�G���[

  return data->questionData[ qIdx ].todayCount;
}

//----------------------------------------------------------------------------
/**
 * @brief ���܂܂ł̉񓚐l�����擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u16 RESEARCH_DATA_GetTotalCountOfQuestion_byIndex( const RESEARCH_DATA* data, u8 qIdx )
{
  GF_ASSERT( qIdx < QUESTION_NUM_PER_TOPIC ); // �C���f�b�N�X�G���[

  return data->questionData[ qIdx ].totalCount;
}

//----------------------------------------------------------------------------
/**
 * @brief ��ID���擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u16 RESEARCH_DATA_GetAnswerID_byIndex( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( aIdx < RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex(data, qIdx) ); 

  return data->questionData[ qIdx ].answerData[ qIdx ].ID;
}

//----------------------------------------------------------------------------
/**
 * @brief �����̉񓚐l�����擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u16 RESEARCH_DATA_GetTodayCountOfAnswer( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( aIdx < RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex(data, qIdx) );

  return data->questionData[ qIdx ].answerData[ qIdx ].todayCount;
}

//----------------------------------------------------------------------------
/**
 * @brief ���܂܂ł̉񓚐l�����擾����
 *
 * @param 
 */
//----------------------------------------------------------------------------
u16 RESEARCH_DATA_GetTotalCountOfAnswer( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( aIdx < RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex(data, qIdx) );

  return data->questionData[ qIdx ].answerData[ qIdx ].totalCount;
}

