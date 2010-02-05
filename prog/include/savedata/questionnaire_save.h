//==============================================================================
/**
 * @file    questionnaire_save.h
 * @brief   ����Ⴂ�A���P�[�g�p�Z�[�u�f�[�^�̃w�b�_
 * @author  matsuda
 * @date    2010.01.20(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �^��`
//==============================================================================
typedef struct _QUESTIONNAIRE_SAVE_WORK QUESTIONNAIRE_SAVE_WORK;

//==============================================================================
//  �\���̒�`
//==============================================================================
///�A���P�[�g�̉񓚃��[�N
typedef struct{
  u8 answer[10];            ///<��(bit�P�ʂŊe����̉񓚂������Ă���)
}QUESTIONNAIRE_ANSWER_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 QuestionnaireSave_GetWorkSize( void );
extern void QuestionnaireSave_WorkInit(void *work);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern QUESTIONNAIRE_SAVE_WORK * SaveData_GetQuestionnaire(SAVE_CONTROL_WORK *sv);
extern QUESTIONNAIRE_ANSWER_WORK * Questionnaire_GetAnswerWork(QUESTIONNAIRE_SAVE_WORK *qsw);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern u32 QuestionnaireWork_ReadBit(const QUESTIONNAIRE_ANSWER_WORK *qanswer, u32 start_bit, u32 bit_size);
extern void QuestionnaireWork_WriteBit(QUESTIONNAIRE_ANSWER_WORK *qanswer, u32 start_bit, u32 bit_size, u32 data);
