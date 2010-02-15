/////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �A���P�[�g�̉񓚃C���f�b�N�X�Q�Ɗ֐�
 * @file   questionnaire_index.c
 * @author obata
 * @date   2010.02.15
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../../resource/research_radar/data/question_id.h"             // for QUESTION_ID_xxxx
#include "../../../resource/research_radar/data/questionnaire_index.h"     // for QUESTIONNAIRE_INDEX
#include "../../../resource/research_radar/data/questionnaire_index.cdat"  // for QuestionnaireIndex


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
  if( QUESTION_ID_MAX < questionID )
  {
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
extern u8 QUESTIONNAIRE_INDEX_GetBitCount( u8 questionID )
{
  // ID�G���[
  if( QUESTION_ID_MAX < questionID )
  {
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
  if( QUESTION_ID_MAX < questionID )
  {
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
extern u8 QUESTIONNAIRE_INDEX_GetBitCount( u8 questionID )
{
  // ID�G���[
  if( QUESTION_ID_MAX < questionID )
  {
    GF_ASSERT(0);
    return 0;
  }
  return QuestionnaireIndex[ questionID ].bitCountOffset;
}
