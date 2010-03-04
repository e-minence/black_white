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

  return data->questionData[ qIdx ].answerData[ aIdx ].ID;
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

  return data->questionData[ qIdx ].answerData[ aIdx ].todayCount;
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

  return data->questionData[ qIdx ].answerData[ aIdx ].totalCount;
}

//----------------------------------------------------------------------------
/**
 * @brief �񓚂̕\���J���[ ( R ) ���擾����
 */
//----------------------------------------------------------------------------
extern u8 RESEARCH_DATA_GetColorR( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( aIdx < RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex(data, qIdx) );

  return data->questionData[ qIdx ].answerData[ aIdx ].colorR;
}

//----------------------------------------------------------------------------
/**
 * @breif �񓚂̕\���J���[ ( G ) ���擾����
 */
//----------------------------------------------------------------------------
extern u8 RESEARCH_DATA_GetColorG( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( aIdx < RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex(data, qIdx) );

  return data->questionData[ qIdx ].answerData[ aIdx ].colorG;
}

//----------------------------------------------------------------------------
/**
 * @brief �񓚂̕\���J���[ ( B ) ���擾����
 */
//----------------------------------------------------------------------------
extern u8 RESEARCH_DATA_GetColorB( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx )
{
  // �C���f�b�N�X�G���[
  GF_ASSERT( aIdx < RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex(data, qIdx) );

  return data->questionData[ qIdx ].answerData[ aIdx ].colorB;
}


//----------------------------------------------------------------------------
/**
 * @brief ����ID�̃C���f�b�N�X���擾���� ( ID���w�� )
 *
 * @param data
 * @param qID  ����ID
 *
 * @return �w�肵������f�[�^�̃C���f�b�N�X
 */
//----------------------------------------------------------------------------
u8 RESEARCH_DATA_GetQuestionIndex_byID( const RESEARCH_DATA* data, u8 qID )
{
  int qIdx;

  // �w�肳�ꂽ����ID������
  for( qIdx=0; qIdx < QUESTION_NUM_PER_TOPIC; qIdx++ )
  {
    // ����
    if( data->questionData[ qIdx ].ID == qID )
    {
      return qIdx;
    }
  }

  // �w�肵��ID�͑��݂��Ȃ�
  GF_ASSERT(0);
  return 0;
}

//----------------------------------------------------------------------------
/**
 * @brief ��ID�̃C���f�b�N�X���擾���� ( ID���w�� )
 *
 * @param data
 * @param qID
 * @param aID
 *
 * @return �w�肵������ɑ΂���, �w�肵���񓚃f�[�^�̃C���f�b�N�X
 */
//----------------------------------------------------------------------------
u8 RESEARCH_DATA_GetAnswerIndex_byID( const RESEARCH_DATA* data, u8 qID, u8 aID )
{
  int qIdx, aIdx;
  const QUESTION_DATA* qData;

  // ����f�[�^���擾
  qIdx = RESEARCH_DATA_GetQuestionIndex_byID( data, qID );
  qData = &(data->questionData[ qIdx ]);

  // �w�肳�ꂽ�񓚃f�[�^������
  for( aIdx=0; aIdx < qData->answerNum; aIdx++ )
  {
    // ����
    if( qData->answerData[ aIdx ].ID == aID )
    {
      return aIdx;
    }
  }

  // �w�肳�ꂽ�񓚃f�[�^�͑��݂��Ȃ�
  GF_ASSERT(0);
  return 0;
}
