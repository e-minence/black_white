/////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �A���P�[�g�̉񓚃C���f�b�N�X�Q�Ɗ֐�
 * @file   questionnaire_index.c
 * @author obata
 * @date   2010.02.15
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "questionnaire_index.h"

#include "../../../../resource/research_radar/data/answer_id.h"              // for ANSWER_ID_xxxx
#include "../../../../resource/research_radar/data/question_id.h"            // for QUESTION_ID_xxxx
#include "../../../../resource/research_radar/data/questionnaire_index.cdat" // for QuestionnaireIndex
#include "../../../../resource/research_radar/data/answer_num_question.cdat" // for AnswerNum_question[]


//---------------------------------------------------------------------------
/**
 * @brief �񓚂̍��ڐ����擾����
 *
 * @param questionID ����ID
 *
 * @return �w�肵������̉񓚑I�����̐�
 */
//---------------------------------------------------------------------------
u8 QUESTIONNAIRE_INDEX_GetAnswerNum( u8 questionID )
{
  // ID�G���[
  if( QUESTION_ID_MAX < questionID ) {
    GF_ASSERT(0);
    return 0;
  }
  return QuestionnaireIndex[ questionID ].answerNum;
}

//---------------------------------------------------------------------------
/**
 * @brief �񓚂�\������̂ɕK�v�ȃr�b�g�����擾����
 *
 * @param questionID ����ID
 *
 * @return �w�肵������̉񓚂�\������̂ɕK�v�ȃr�b�g��
 */
//---------------------------------------------------------------------------
u8 QUESTIONNAIRE_INDEX_GetBitCount( u8 questionID )
{
  // ID�G���[
  if( QUESTION_ID_MAX < questionID ) {
    GF_ASSERT(0);
    return 0;
  }
  return QuestionnaireIndex[ questionID ].bitCount;
}

//---------------------------------------------------------------------------
/**
 * @brief �Z�[�u�f�[�^�̃I�t�Z�b�g�l���擾���� ( �񓚑I�����̐� )
 *
 * @param questionID ����ID
 *
 * @return �w�肵������̉񓚑I�����̐��ɃA�N�Z�X���邽�߂̃I�t�Z�b�g
 */
//---------------------------------------------------------------------------
u8 QUESTIONNAIRE_INDEX_GetAnswerNumOffset( u8 questionID )
{
  // ID�G���[
  if( QUESTION_ID_MAX < questionID ) {
    GF_ASSERT(0);
    return 0;
  }
  return QuestionnaireIndex[ questionID ].answerNumOffset;
}

//---------------------------------------------------------------------------
/**
 * @brief �Z�[�u�f�[�^�̃I�t�Z�b�g�l���擾���� ( �r�b�g�� )
 *
 * @param questionID ����ID
 *
 * @return �w�肵������̉񓚂�\������̂ɕK�v�ȃr�b�g�� ��
 * �@�@�@�@�A�N�Z�X���邽�߂̃I�t�Z�b�g
 */
//---------------------------------------------------------------------------
u8 QUESTIONNAIRE_INDEX_GetBitCountOffset( u8 questionID )
{
  // ID�G���[
  if( QUESTION_ID_MAX < questionID ) {
    GF_ASSERT(0);
    return 0;
  }
  return QuestionnaireIndex[ questionID ].bitCountOffset;
}

//---------------------------------------------------------------------------
/**
 * @brief �w�肵���񓚂��������鎿���ID���擾����
 *
 * @param answerID ��ID ( ANSWER_ID_xxxx )
 *
 * @return �w�肵���񓚂������鎿���ID ( QUESTION_ID_xxxx )
 */
//---------------------------------------------------------------------------
u8 GetQuestionID_byAnswerID( u8 answerID )
{
  int sum;
  int qID;

  // ID�G���[
  if( ANSWER_ID_MAX < answerID ) {
    GF_ASSERT(0);
    return 0;
  }

  sum = 0;
  for( qID=0; qID < QUESTION_ID_NUM; qID++ )
  {
    sum += AnswerNum_question[ qID ];
    if( answerID <= sum ) {
      return qID;
    }
  }

  GF_ASSERT(0);
  return 0;
}
//---------------------------------------------------------------------------
/**
 * @brief �w�肵���񓚂�, �������鎿����̉��Ԗڂ̉񓚂Ȃ̂����擾����
 *
 * @param answerID ��ID ( ANSWER_ID_xxxx )
 *
 * @return �w�肵���񓚂�, �������鎿����̉��Ԗڂ̉񓚂Ȃ̂� 
 *         ( 1 �` MAX_ANSWER_NUM_PER_QUESTION )
 */
//---------------------------------------------------------------------------
u8 GetAnswerIndex_atQuestion( u8 answerID )
{
  int sum;
  int qID;

  // ID�G���[
  if( ANSWER_ID_MAX < answerID ) {
    GF_ASSERT(0);
    return 1;
  }

  sum = 0;
  for( qID=0; qID < QUESTION_ID_NUM; qID++ )
  {
    sum += AnswerNum_question[ qID ];
    if( answerID <= sum ) {
      return sum - answerID + 1;  // 1 Origin
    }
  }

  GF_ASSERT(0);
  return 1;
}
