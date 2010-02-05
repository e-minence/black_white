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


//==============================================================================
//  �萔��`
//==============================================================================
///����̃o���G�[�V������
#define QUESTIONNAIRE_ITEM_NUM       (30)
///�񓚂̃o���G�[�V������
#define QUESTIONNAIRE_ANSWER_NUM     (150)


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
  
  GFL_STD_MemClear(anketo_save, sizeof(QUESTIONNAIRE_SAVE_WORK));
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
 * �񓚃��[�N�ւ̃|�C���^���擾
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
 * �񓚂�ǂݎ��
 *
 * @param   qanswer		  �񓚃��[�N�ւ̃|�C���^
 * @param   start_bit		�ǂݎ��J�nbit�ʒu
 * @param   bit_size		�ǂݎ��bit�T�C�Y
 *
 * @retval  u32		��
 */
//==================================================================
u32 QuestionnaireWork_ReadBit(const QUESTIONNAIRE_ANSWER_WORK *qanswer, u32 start_bit, u32 bit_size)
{
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
}

//==================================================================
/**
 * �񓚂���������
 *
 * @param   qanswer		  �񓚃��[�N�ւ̃|�C���^
 * @param   start_bit		�������݊J�nbit�ʒu
 * @param   bit_size		��������bit�T�C�Y
 * @param   data		    ��
 */
//==================================================================
void QuestionnaireWork_WriteBit(QUESTIONNAIRE_ANSWER_WORK *qanswer, u32 start_bit, u32 bit_size, u32 data)
{
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

