//==============================================================================
/**
 * @file    questionnaire_save.c
 * @brief   ����Ⴂ�A���P�[�g�p�Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.01.20(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/questionnaire_save.h"
#include "app/research_radar/questionnaire_index.h"
#include "app/research_radar/research_select_def.h"   //SELECT_TOPIC_MAX_NUM


//==============================================================================
//  �萔��`
//==============================================================================
///����̃o���G�[�V������
#define QUESTIONNAIRE_ITEM_NUM       (30)
///�񓚂̃o���G�[�V������(���񓚂�����)
#define QUESTIONNAIRE_ANSWER_NUM     (150)

///�{���̒����l���ő吔
#define QUESTIONNAIRE_TODAY_COUNT_MAX   (999)
///���܂ł̒����l���ő吔
#define QUESTIONNAIRE_TOTAL_COUNT_MAX   (99999)

///���█�̖{���̒����l���ő吔
#define QUESTIONNAIRE_TODAY_ANSWER_COUNT_MAX   (99)
///���█�̍��܂ł̒����l���ő吔
#define QUESTIONNAIRE_TOTAL_ANSWER_COUNT_MAX   (9999)

//==============================================================================
//  �\���̒�`
//==============================================================================
///�A���P�[�g�Z�[�u���[�N
struct _QUESTIONNAIRE_SAVE_WORK{
  u16 today_count[QUESTIONNAIRE_ITEM_NUM];        ///<�����̒����l��
  u8 today_answer[QUESTIONNAIRE_ANSWER_NUM];      ///<�����̓���(���ڕ�)
  QUESTIONNAIRE_ANSWER_WORK mine_answer;          ///<�����̓���
  u32 total_count[QUESTIONNAIRE_ITEM_NUM];        ///<���܂ł̒����l���̍��v
  u16 total_answer[QUESTIONNAIRE_ANSWER_NUM];     ///<���܂ł̓����̍��v(���ڕ�)
  
  u8 investigating_question_id[SELECT_TOPIC_MAX_NUM];    ///<�������̎���ID
};



//==============================================================================
//
//  
//
//==============================================================================
//------------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�T�C�Y��Ԃ�
 *
 * @retval  int		
 */
//------------------------------------------------------------------
u32 QuestionnaireSave_GetWorkSize( void )
{
	return sizeof(QUESTIONNAIRE_SAVE_WORK);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void QuestionnaireSave_WorkInit(void *work)
{
  QUESTIONNAIRE_SAVE_WORK *anketo_save = work;
  int work_index;
  
  GFL_STD_MemClear(anketo_save, sizeof(QUESTIONNAIRE_SAVE_WORK));
  for(work_index = 0; work_index < SELECT_TOPIC_MAX_NUM; work_index++){
    anketo_save->investigating_question_id[work_index] = INVESTIGATING_QUESTION_NULL;
  }
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^���擾
 *
 * @param   sv		�Z�[�u����V�X�e���ւ̃|�C���^
 *
 * @retval  QUESTIONNAIRE_SAVE_WORK *		�A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 */
//==================================================================
QUESTIONNAIRE_SAVE_WORK * SaveData_GetQuestionnaire(SAVE_CONTROL_WORK *sv)
{
  return SaveControl_DataPtrGet(sv, GMDATA_ID_QUESTIONNAIRE);
}

//==================================================================
/**
 * �����̉񓚃��[�N�ւ̃|�C���^���擾
 *
 * @param   qsw		�A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 *
 * @retval  QUESTIONNAIRE_ANSWER_WORK *		�񓚃��[�N�ւ̃|�C���^
 */
//==================================================================
QUESTIONNAIRE_ANSWER_WORK * Questionnaire_GetAnswerWork(QUESTIONNAIRE_SAVE_WORK *qsw)
{
  return &qsw->mine_answer;
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �A���P�[�g�񓚂�ǂݎ��
 *
 * @param   qanswer		  �񓚃��[�N�ւ̃|�C���^
 * @param   question_id ����ID
 *
 * @retval  u32		��
 *
 * �����̉񓚃f�[�^�̓ǂݎ��A�r�[�R���ɓ����Ă���񓚃f�[�^�̓ǂݎ��Ɏg�p
 */
//==================================================================
u32 QuestionnaireAnswer_ReadBit(const QUESTIONNAIRE_ANSWER_WORK *qanswer, u8 question_id)
{
  u32 start_bit = QUESTIONNAIRE_INDEX_GetBitCountOffset(question_id);
  u32 bit_size = QUESTIONNAIRE_INDEX_GetBitCount(question_id);
  u32 buf_no = start_bit / 8;
  u32 buf_bit = start_bit % 8;
  u32 ret = 0;
  u32 count = 0;
  
  GF_ASSERT(bit_size < 32);
  
  while(bit_size){
    ret |= ((qanswer->answer[buf_no] >> buf_bit) & 1) << count;
    buf_bit++;
    if(buf_bit > 7){
      buf_no++;
      buf_bit = 0;
    }
    count++;
    bit_size--;
  }
  
  return ret;
}

//==================================================================
/**
 * �A���P�[�g�񓚂���������
 *
 * @param   qanswer		  �񓚃��[�N�ւ̃|�C���^
 * @param   question_id ����ID
 * @param   data		    ��
 * 
 * �����̉񓚃f�[�^�̓ǂݎ��A�r�[�R���ɓ����Ă���񓚃f�[�^�̓ǂݎ��Ɏg�p
 */
//==================================================================
void QuestionnaireAnswer_WriteBit(QUESTIONNAIRE_ANSWER_WORK *qanswer, u8 question_id, u32 data)
{
  u32 start_bit = QUESTIONNAIRE_INDEX_GetBitCountOffset(question_id);
  u32 bit_size = QUESTIONNAIRE_INDEX_GetBitCount(question_id);
  u32 buf_no = start_bit / 8;
  u32 buf_bit = start_bit % 8;
  u32 ret = 0;
  u32 count = 0;
  
  GF_ASSERT(bit_size < 32);
  
  while(bit_size){
    qanswer->answer[buf_no] &= 0xff ^ (1 << buf_bit);  //�܂��N���A
    qanswer->answer[buf_no] |= (data & 1) << buf_bit;  //�����ăZ�b�g
    buf_bit++;
    if(buf_bit > 7){
      buf_no++;
      buf_bit = 0;
    }
    data >>= 1;
    bit_size--;
  }
}

//==================================================================
/**
 * �������̎���ID���Z�b�g
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   work_index    ID���Z�b�g���郏�[�N�ʒu(0�`SELECT_TOPIC_MAX_NUM)
 */
//==================================================================
void QuestionnaireWork_SetInvestigatingQuestion(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int work_index)
{
  GF_ASSERT(work_index < SELECT_TOPIC_MAX_NUM);
  qsw->investigating_question_id[work_index] = question_id;
}

//==================================================================
/**
 * �������̎���ID���擾����
 *
 * @param   qsw		�A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   work_index    ID���擾���郏�[�N�ʒu(0�`SELECT_TOPIC_MAX_NUM)
 *
 * @retval  int		�������̎���ID (�����w�肵�Ă��Ȃ��ꍇ��INVESTIGATING_QUESTION_NULL)
 */
//==================================================================
int QuestionnaireWork_GetInvestigatingQuestion(QUESTIONNAIRE_SAVE_WORK *qsw, int work_index)
{
  GF_ASSERT(work_index < SELECT_TOPIC_MAX_NUM);
  return qsw->investigating_question_id[work_index];
}

//==================================================================
/**
 * ����ID���̍����̒����l�����擾
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 *
 * @retval  u16		�����l��
 */
//==================================================================
u16 QuestionnaireWork_GetTodayCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id)
{
  return qsw->today_count[question_id];
}

//==================================================================
/**
 * ����ID���̍����̒����l�������Z
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   add_count     ���Z��
 */
//==================================================================
void QuestionnaireWork_AddTodayCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int add_count)
{
  qsw->today_count[question_id] += add_count;
  if(qsw->today_count[question_id] > QUESTIONNAIRE_TODAY_COUNT_MAX){
    qsw->today_count[question_id] = QUESTIONNAIRE_TODAY_COUNT_MAX;
  }
}

//==================================================================
/**
 * ����ID���̍��܂ł̍��v�����l�����擾
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 *
 * @retval  u16		�����l��
 */
//==================================================================
u16 QuestionnaireWork_GetTotalCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id)
{
  return qsw->total_count[question_id];
}

//==================================================================
/**
 * ����ID���̍��܂ł̍��v�����l�������Z
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   add_count     ���Z��
 */
//==================================================================
void QuestionnaireWork_AddTotalCount(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int add_count)
{
  qsw->total_count[question_id] += add_count;
  if(qsw->total_count[question_id] > QUESTIONNAIRE_TOTAL_COUNT_MAX){
    qsw->total_count[question_id] = QUESTIONNAIRE_TOTAL_COUNT_MAX;
  }
}

//--------------------------------------------------------------
/**
 * ����ID�ƍ���No����Ώۂ̃o�b�t�@�̗v�f�ԍ����擾����
 *
 * @param   question_id		
 * @param   answer_type		
 *
 * @retval  u32		
 */
//--------------------------------------------------------------
static u32 _Get_AnswerNo(int question_id, int answer_type)
{
  u32 answer_no;
  
  answer_no = QUESTIONNAIRE_INDEX_GetAnswerNumOffset(question_id);
  answer_no += answer_type - 1; //���񓚂��������f�[�^�\���ɂȂ��Ă��邽�߁A-1
  GF_ASSERT(answer_no < QUESTIONNAIRE_ANSWER_NUM);
  return answer_no;
}

//==================================================================
/**
 * �w�荀�ڂ̓������l�����擾�F�����̓���
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   answer_type		����ID���̍���No ��0:��0  1:��1  2:��2...
 *
 * @retval  u8		�񓚐l��
 */
//==================================================================
u8 QuestionnaireWork_GetTodayAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type)
{
  if(answer_type == 0){ //��0�͖��񓚂ׁ̈A�Œ��0��Ԃ�
    GF_ASSERT(0); //���񓚂͉��Z���Ă����f�[�^�ł͂Ȃ��ׁA�v������鎖�͂�������
    return 0;
  }
  
  return qsw->today_answer[ _Get_AnswerNo(question_id, answer_type) ];
}

//==================================================================
/**
 * �w�荀�ڂ̓������l�������Z�F�����̓���
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   answer_type		����ID���̍���No ��0:��0  1:��1  2:��2...
 * @param   add_count     ���Z��
 */
//==================================================================
void QuestionnaireWork_AddTodayAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type, int add_count)
{
  u32 answer_no;

  if(answer_type == 0){
    GF_ASSERT(0); //���񓚂͉��Z���Ă����f�[�^�ł͂Ȃ��ׁA�v������鎖�͂�������
    return;
  }

  answer_no = _Get_AnswerNo(question_id, answer_type);
  if(qsw->today_answer[answer_no] + add_count > QUESTIONNAIRE_TODAY_ANSWER_COUNT_MAX){
    qsw->today_answer[answer_no] = QUESTIONNAIRE_TODAY_ANSWER_COUNT_MAX;
  }
  else{
    qsw->today_answer[answer_no] += add_count;
  }
}

//==================================================================
/**
 * �w�荀�ڂ̓������l�����擾�F���܂ł̍��v
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   answer_type		����ID���̍���No
 *
 * @retval  u8		�񓚐l��
 */
//==================================================================
u8 QuestionnaireWork_GetTotalAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type)
{
  if(answer_type == 0){ //��0�͖��񓚂ׁ̈A�Œ��0��Ԃ�
    GF_ASSERT(0); //���񓚂͉��Z���Ă����f�[�^�ł͂Ȃ��ׁA�v������鎖�͂�������
    return 0;
  }
  
  return qsw->total_answer[ _Get_AnswerNo(question_id, answer_type) ];
}

//==================================================================
/**
 * �w�荀�ڂ̓������l�������Z�F���܂ł̍��v
 *
 * @param   qsw		        �A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 * @param   question_id		����ID
 * @param   answer_type		����ID���̍���No
 * @param   add_count     ���Z��
 *
 * @retval  u8		�񓚐l��
 */
//==================================================================
void QuestionnaireWork_AddTotalAnswerNum(QUESTIONNAIRE_SAVE_WORK *qsw, int question_id, int answer_type, int add_count)
{
  u32 answer_no;

  if(answer_type == 0){
    GF_ASSERT(0); //���񓚂͉��Z���Ă����f�[�^�ł͂Ȃ��ׁA�v������鎖�͂�������
    return;
  }

  answer_no = _Get_AnswerNo(question_id, answer_type);
  if(qsw->total_answer[answer_no] + add_count > QUESTIONNAIRE_TOTAL_ANSWER_COUNT_MAX){
    qsw->total_answer[answer_no] = QUESTIONNAIRE_TOTAL_ANSWER_COUNT_MAX;
  }
  else{
    qsw->total_answer[answer_no] += add_count;
  }
}

//==================================================================
/**
 * ���t���ς�������̃f�[�^���f����
 *
 * @param   qsw		�A���P�[�g�Z�[�u���[�N�ւ̃|�C���^
 */
//==================================================================
void QuestionnaireWork_DateChangeUpdate(QUESTIONNAIRE_SAVE_WORK *qsw)
{
  int i;
  
  for(i = 0; i < QUESTIONNAIRE_ITEM_NUM; i++){
    qsw->total_count[i] += qsw->today_count[i];
    if(qsw->total_count[i] > QUESTIONNAIRE_TOTAL_COUNT_MAX){
      qsw->total_count[i] = QUESTIONNAIRE_TOTAL_COUNT_MAX;
    }
    qsw->today_count[i] = 0;
  }
  for(i = 0; i < QUESTIONNAIRE_ANSWER_NUM; i++){
    qsw->total_answer[i] += qsw->today_answer[i];
    if(qsw->total_answer[i] > QUESTIONNAIRE_TOTAL_ANSWER_COUNT_MAX){
      qsw->total_answer[i] = QUESTIONNAIRE_TOTAL_ANSWER_COUNT_MAX;
    }
    qsw->today_answer[i] = 0;
  }
}


