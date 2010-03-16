////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �f�o�b�O���j���[�w���l���́x ( ����Ⴂ�����֘A )
 * @file   debug_numinput_research.c
 * @author obata
 * @date   2010.03.13
 */
////////////////////////////////////////////////////////////////////////////////
#include "debug_numinput_research.h"

#include "savedata/save_control.h"       // for SAVE_CONTROL_WORK
#include "savedata/questionnaire_save.h" // for QUESTIONNAIRE_SAVE_WORK 
#include "savedata/misc.h"               // for MISC

#include "app/research_radar/questionnaire_index.h"
#include "field/research_team_def.h"


//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� �����̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �擾���鎿���ID ( QUESTION_ID_xxxx )
 *
 * @return ����ɑ΂���, �����̉񓚐l��
 */
//------------------------------------------------------------------------------
u32 DEBUG_GetTodayCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // �w�肳�ꂽ����ɑ΂���, �����̉񓚐l����Ԃ�
  return QuestionnaireWork_GetTodayCount( Qsave, questionID );
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� �����̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �ݒ肷�鎿���ID ( QUESTION_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
void DEBUG_SetTodayCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID, u32 count )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;
  int addCount;
  int nowCount;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // ���ݒl, �ڕW�l����, ���Z�l���Z�o
  nowCount = QuestionnaireWork_GetTodayCount( Qsave, questionID );
  addCount = count - nowCount;

  // �w�肳�ꂽ�l�ɂȂ�悤�ɉ��Z
  QuestionnaireWork_AddTodayCount( Qsave, questionID, addCount );
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �擾���鎿���ID ( QUESTION_ID_xxxx )
 *
 * @return ����ɑ΂���, ���܂܂ł̉񓚐l��
 */
//------------------------------------------------------------------------------
u32 DEBUG_GetTotalCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // �w�肳�ꂽ����ɑ΂���, �����̉񓚐l����Ԃ�
  return QuestionnaireWork_GetTotalCount( Qsave, questionID );
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �ݒ肷�鎿���ID ( QUESTION_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
void DEBUG_SetTotalCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID, u32 count )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;
  int addCount;
  int nowCount;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // ���ݒl, �ڕW�l����, ���Z�l���Z�o
  nowCount = QuestionnaireWork_GetTotalCount( Qsave, questionID );
  addCount = count - nowCount;

  // �w�肳�ꂽ�l�ɂȂ�悤�ɉ��Z
  QuestionnaireWork_AddTotalCount( Qsave, questionID, addCount );
} 
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� �����̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID �擾����񓚂�ID ( ANSWER_ID_xxxx )
 *
 * @return ����ɑ΂���, �����̉񓚐l��
 */
//------------------------------------------------------------------------------
u32 DEBUG_GetTodayCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;
  int questionID;
  int answerIdx;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // �w�肳�ꂽ�񓚂������鎿���ID���擾
  questionID = GetQuestionID_byAnswerID( answerID );

  // �w�肳�ꂽ�񓚂�, ���Ԗڂ̉񓚂Ȃ̂����擾
  answerIdx = GetAnswerIndex_atQuestion( answerID );

  // �w�肳�ꂽ����ɑ΂���, �����̉񓚐l����Ԃ�
  return QuestionnaireWork_GetTodayAnswerNum( Qsave, questionID, answerIdx );
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� �����̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID   �ݒ肷��񓚂�ID ( ANSWER_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
void DEBUG_SetTodayCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 count )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;
  int questionID;
  int answerIdx;
  int nowCount, addCount;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // �w�肳�ꂽ�񓚂������鎿���ID���擾
  questionID = GetQuestionID_byAnswerID( answerID );

  // �w�肳�ꂽ�񓚂�, ���Ԗڂ̉񓚂Ȃ̂����擾
  answerIdx = GetAnswerIndex_atQuestion( answerID );

  // ���ݒl, �ڕW�l����, ���Z�l���Z�o
  nowCount = QuestionnaireWork_GetTodayAnswerNum( Qsave, questionID, answerIdx );
  addCount = count - nowCount;

  // �w�肳�ꂽ�l�ɂȂ�悤�ɉ��Z
  QuestionnaireWork_AddTodayAnswerNum( Qsave, questionID, answerIdx, addCount );

  // DEBUG:
  OBATA_Printf( "SetTodayCountOfAnswer: qID=%d, aID=%d, aIdx=%d, count=%d\n", 
      questionID, answerID, answerIdx,
      QuestionnaireWork_GetTodayAnswerNum( Qsave, questionID, answerIdx ) );
} 
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID �擾����񓚂�ID ( ANSWER_ID_xxxx )
 *
 * @return ����ɑ΂���, ���܂܂ł̉񓚐l��
 */
//------------------------------------------------------------------------------
u32 DEBUG_GetTotalCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;
  int questionID;
  int answerIdx;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // �w�肳�ꂽ�񓚂������鎿���ID���擾
  questionID = GetQuestionID_byAnswerID( answerID );

  // �w�肳�ꂽ�񓚂�, ���Ԗڂ̉񓚂Ȃ̂����擾
  answerIdx = GetAnswerIndex_atQuestion( answerID );

  // �w�肳�ꂽ����ɑ΂���, ���܂܂ł̉񓚐l����Ԃ�
  return QuestionnaireWork_GetTotalAnswerNum( Qsave, questionID, answerIdx );
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID   �ݒ肷��񓚂�ID ( ANSWER_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
void DEBUG_SetTotalCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 count )
{
  SAVE_CONTROL_WORK* save;
  QUESTIONNAIRE_SAVE_WORK* Qsave;
  int questionID;
  int answerIdx;
  int nowCount, addCount;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  Qsave = SaveData_GetQuestionnaire( save );

  // �w�肳�ꂽ�񓚂������鎿���ID���擾
  questionID = GetQuestionID_byAnswerID( answerID );

  // �w�肳�ꂽ�񓚂�, ���Ԗڂ̉񓚂Ȃ̂����擾
  answerIdx = GetAnswerIndex_atQuestion( answerID );

  // ���ݒl, �ڕW�l����, ���Z�l���Z�o
  nowCount = QuestionnaireWork_GetTotalAnswerNum( Qsave, questionID, answerIdx );
  addCount = count - nowCount;

  // �w�肳�ꂽ�l�ɂȂ�悤�ɉ��Z
  QuestionnaireWork_AddTotalAnswerNum( Qsave, questionID, answerIdx, addCount );

  // DEBUG:
  OBATA_Printf( "SetTotalCountOfAnswer: qID=%d, aID=%d, aIdx=%d, count=%d\n", 
      questionID, answerID, answerIdx,
      QuestionnaireWork_GetTotalAnswerNum( Qsave, questionID, answerIdx ) );
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� ����Ⴂ������ �� ���������N )
 *
 * @param gameSystem
 * @param gameData
 * @param param
 *
 * @return ���������N ( RESEARCH_TEMP_RANK_xxxx )
 */
//------------------------------------------------------------------------------
u32 DEBUG_GetResearchTeamRank( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 param )
{
  SAVE_CONTROL_WORK* save;
  MISC* misc;
  int rank;

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  misc = SaveData_GetMisc( save );

  // ���������N���擾
  rank = MISC_CrossComm_GetResearchTeamRank( misc );

  return rank;
}
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� ����Ⴂ������ �� ���������N )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID   �ݒ肷��񓚂�ID ( ANSWER_ID_xxxx )
 * @param rank       �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
void DEBUG_SetResearchTeamRank( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 rank )
{
  SAVE_CONTROL_WORK* save;
  MISC* misc;

  // �����`�F�b�N
  GF_ASSERT( rank < RESEARCH_TEAM_RANK_MAX );

  // �Z�[�u�f�[�^�擾
  save = GAMEDATA_GetSaveControlWork( gameData );
  misc = SaveData_GetMisc( save );

  // ���������N��ݒ�
  MISC_CrossComm_SetResearchTeamRank( misc, rank );
}
