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
//  �萔��`
//==============================================================================
///�������̎��₪�Ȃ��ꍇ
#define INVESTIGATING_QUESTION_NULL     (0xff)


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
extern u32 QuestionnaireAnswer_ReadBit(const QUESTIONNAIRE_ANSWER_WORK *qanswer, u8 question_id);
extern void QuestionnaireAnswer_WriteBit(QUESTIONNAIRE_ANSWER_WORK *qanswer, u8 question_id, u32 data);

extern void QuestionnaireWork_SetInvestigatingQuestion(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int work_index);
extern int QuestionnaireWork_GetInvestigatingQuestion(QUESTIONNAIRE_SAVE_WORK *qsw, int work_index);
extern u16 QuestionnaireWork_GetTodayCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id);
extern void QuestionnaireWork_AddTodayCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int add_count);
extern u32 QuestionnaireWork_GetTotalCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id);
extern void QuestionnaireWork_AddTotalCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int add_count);
extern u8 QuestionnaireWork_GetTodayAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type);
extern void QuestionnaireWork_AddTodayAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type, int add_count);
extern u16 QuestionnaireWork_GetTotalAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type);
extern void QuestionnaireWork_AddTotalAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type, int add_count);
extern void QuestionnaireWork_DateChangeUpdate(QUESTIONNAIRE_SAVE_WORK *qsw);

//--------------------------------------------------------------
//--------------------------------------------------------------
#ifdef PM_DEBUG
extern void QuestionnaireWork_ClearResearch(QUESTIONNAIRE_SAVE_WORK *qsw); 
#endif
