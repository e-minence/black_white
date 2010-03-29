///////////////////////////////////////////////////////////////////////////////
/**
 * @file	 scrcmd_research.c
 * @brief	 �X�N���v�g�R�}���h: ����Ⴂ�������֘A
 * @date   2010.03.09
 * @author obata
 */
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd_research.h"

#include "scrcmd_work.h"
#include "script_local.h"
#include "scrcmd.h"
#include "event_research_team.h"

#include "field/research_team_def.h"
#include "savedata/misc.h" 
#include "savedata/questionnaire_save.h" 
#include "gamesystem/game_beacon.h"

#include "../../../resource/research_radar/data/question_id.h"            // for QUESTION_ID_xxxx
#include "../../../resource/research_radar/data/answer_num_question.cdat" // for AnswerNum_question[]
#include "../../../resource/research_radar/data/answer_id_question.cdat"  // for AnswerID_question[][]


//-----------------------------------------------------------------------------
/**
 * @brief  �������̑��������N���擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetResearchTeamRank( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*       script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*              misc     = SaveData_GetMisc( save );
  u16* ret;
  
  ret = SCRCMD_GetVMWork( core, work ); // ������: ���������N�̊i�[��

  // ���������N���擾����
  *ret = MISC_CrossComm_GetResearchTeamRank( misc );

  // DEBUG:
  OS_TFPrintf( 3, "_GET_RESEARCH_TEAM_RANK ==> %d\n", *ret );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief  �������̑��������N��1�グ��
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdResearchTeamRankUp( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*       script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*              misc     = SaveData_GetMisc( save );
  int rank;
  
  // ���������N���擾����
  rank = MISC_CrossComm_GetResearchTeamRank( misc );

  // ���łɍō������N
  if( RESEARCH_TEAM_RANK_5 <= rank ) {
    GF_ASSERT(0); // ����ȏ�͏オ��Ȃ�
    return VMCMD_RESULT_CONTINUE;
  }

  // �����N�A�b�v
  rank++;
  MISC_CrossComm_SetResearchTeamRank( misc, rank );

  // �r�[�R�����M�o�b�t�@�ɔ��f������
  GAMEBEACON_SendDataUpdate_ResearchTeamRank( rank );

  // DEBUG:
  OS_TFPrintf( 3, "_RESEARCH_TEAM_RANK_UP ==> %d\n", rank );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �󂯂Ă��钲���˗�ID���擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetResearchRequestID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*       script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*              misc     = SaveData_GetMisc( save );
  u16* ret;
  
  ret = SCRCMD_GetVMWork( core, work ); // ������: �����˗�ID�̊i�[��

  // �������̒����˗�ID���擾
  *ret = MISC_GetResearchRequestID( misc );

  // DEBUG:
  OS_TFPrintf( 3, "_GET_RESEARCH_REQUEST_ID ==> %d\n", *ret );

  return VMCMD_RESULT_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @brief �������̎���ID���擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetResearchQuestionID( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*       script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*              misc     = SaveData_GetMisc( save );
  u16* retQ1;
  u16* retQ2;
  u16* retQ3;
  
  // �R�}���h�̈������擾
  retQ1 = SCRCMD_GetVMWork( core, work ); // ������: ����ID1�̊i�[��
  retQ2 = SCRCMD_GetVMWork( core, work ); // ������: ����ID2�̊i�[��
  retQ3 = SCRCMD_GetVMWork( core, work ); // ��O����: ����ID3�̊i�[��

  // �������̎���ID���擾
  *retQ1 = MISC_GetResearchQuestionID( misc, 0 ); 
  *retQ2 = MISC_GetResearchQuestionID( misc, 1 ); 
  *retQ3 = MISC_GetResearchQuestionID( misc, 2 ); 

  // DEBUG:
  OS_TFPrintf( 3, "_GET_RESEARCH_QUESTION_ID ==> %d, %d, %d\n", *retQ1, *retQ2, *retQ3 );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �������̒����ɂ���, �o�ߎ���[h]���擾���� ( MAX 24 )
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetResearchPassedTime( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*       script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*              misc     = SaveData_GetMisc( save );

  u16* ret;
  s64 startSec;
  u16 passedHour;
  
  // �R�}���h�̈������擾
  ret = SCRCMD_GetVMWork( core, work ); // ������: �o�ߎ��Ԃ̊i�[��

  // �����̊J�n�������擾
  startSec = MISC_GetResearchStartTimeBySecond( misc );

  // �������łȂ�
  if( MISC_GetResearchRequestID( misc ) == RESEARCH_REQ_ID_NONE ) {
    passedHour = 0;
  }
  // ������
  else { 
    s64 nowSec, passedSec;
    RTCDate passedDate, nowDate;
    RTCTime passedTime, nowTime;

    // ���ݎ������擾
    GFL_RTC_GetDateTime( &nowDate, &nowTime );
    nowSec = RTC_ConvertDateTimeToSecond( &nowDate, &nowTime );
    OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME: year=%d, month=%d, day=%d\n", nowDate.year, nowDate.month, nowDate.day );
    OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME: hour=%d, minute=%d, second=%d\n", nowTime.hour, nowTime.minute, nowTime.second );
    OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME: startSec=%ld, nowSec=%ld\n", startSec, nowSec );

    // �o�ߎ���[h]���Z�o
    passedSec = nowSec - startSec;
    RTC_ConvertSecondToDateTime( &passedDate, &passedTime, passedSec );
    OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME: startSec=%ld, nowSec=%ld, passedSec=%ld\n", startSec, nowSec, passedSec ); 
    OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME: year=%d, month=%d, day=%d\n", passedDate.year, passedDate.month, passedDate.day );
    OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME: hour=%d, minute=%d, second=%d\n", passedTime.hour, passedTime.minute, passedTime.second );

    if( (0 < passedDate.year) ||  // 1�N�ȏオ�o��
        (1 < passedDate.month) || // 1���ȏオ�o��
        (1 < passedDate.day) )    // 1���ȏオ�o��
    { 
      passedHour = 24; // �ő�24���ԂƂ��� ( ����ȏ�͕K�v�Ȃ��̂� )
    }
    else
    {
      passedHour = passedTime.hour;
    }
  }

  // �o�ߎ��Ԃ�Ԃ�
  *ret = passedHour;

  // DEBUG:
  OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME ==> %d\n", passedHour );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �������J�n����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdStartResearch( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*             work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*             script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*                gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*       save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*                    misc     = SaveData_GetMisc( save );
  QUESTIONNAIRE_SAVE_WORK* qSave    = SaveData_GetQuestionnaire( save );

  int i;
  u16 reqID;
  u16 qID[ MAX_QNUM_PER_RESEARCH_REQ ];

  // �R�}���h�̈������擾
  reqID  = SCRCMD_GetVMWorkValue( core, work ); // ������: �����˗�ID
  qID[0] = SCRCMD_GetVMWorkValue( core, work ); // ������: ����ID1
  qID[1] = SCRCMD_GetVMWorkValue( core, work ); // ��O����: ����ID2
  qID[2] = SCRCMD_GetVMWorkValue( core, work ); // ��l����: ����ID3

  // DEBUG:
  OS_TFPrintf( 3, "_START_RESEARCH: reqID=%d, qID[0]=%d, qID[1]=%d, qID[2]=%d\n", reqID, qID[0], qID[1], qID[2] );

  // �����˗�ID���Z�[�u
  MISC_SetResearchRequestID( misc, reqID );

 for( i=0; i<MAX_QNUM_PER_RESEARCH_REQ; i++ )
 {
   // ����ID���Z�[�u
   MISC_SetResearchQuestionID( misc, i, qID[i] );

   // �_�~�[ID
   if( qID[i] == QUESTION_ID_DUMMY ) { continue; }

   // �J�n��(�����_)�̉񓚐l�����Z�[�u
   {
     int totalCount = QuestionnaireWork_GetTotalCount( qSave, qID[i] );
     int todayCount = QuestionnaireWork_GetTodayCount( qSave, qID[i] );
     int sumCount = totalCount + todayCount;  
     MISC_SetResearchStartCount( misc, i, sumCount );

     // DEBUG:
     OS_TFPrintf( 3, "_START_RESEARCH: sumCount=%d\n", sumCount );
   }
 }

  // �����J�n�������Z�[�u
  MISC_SetResearchStartTimeBySecond( misc, GFL_RTC_GetDateTimeBySecond() );

  // DEBUG:
  OS_TFPrintf( 3, "_START_RESEARCH: start time=%d\n", GFL_RTC_GetDateTimeBySecond() );


  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �������I������
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdFinishResearch( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*             work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*             script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*                gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*       save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*                    misc     = SaveData_GetMisc( save );
  QUESTIONNAIRE_SAVE_WORK* qSave    = SaveData_GetQuestionnaire( save );

  int i;

  // �Z�[�u�f�[�^���N���A
  MISC_SetResearchRequestID( misc, RESEARCH_REQ_ID_NONE );
  for( i=0; i<MAX_QNUM_PER_RESEARCH_REQ; i++ )
  {
    MISC_SetResearchQuestionID( misc, i, QUESTION_ID_DUMMY );
  }

  // DEBUG:
  OS_TFPrintf( 3, "_FINISH_RESEARCH\n" );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief ����ɑ΂���, �����_�̉񓚐l�����擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetTotalCountOfQuestion( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*             work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*             script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*                gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*       save     = GAMEDATA_GetSaveControlWork( gameData );
  QUESTIONNAIRE_SAVE_WORK* qSave    = SaveData_GetQuestionnaire( save );

  int i;
  u16 qID;
  u16* retWork;
  int totalCount;
  int todayCount;
  int sumCount;

  // �R�}���h�̈������擾
  qID     = SCRCMD_GetVMWorkValue( core, work ); // ������: ����ID
  retWork = SCRCMD_GetVMWork( core, work );      // ������: �񓚐l���̊i�[��

  // ���܂܂ł̉񓚐l�����擾
  totalCount = QuestionnaireWork_GetTotalCount( qSave, qID );
  todayCount = QuestionnaireWork_GetTodayCount( qSave, qID );
  sumCount   = totalCount + todayCount;
  *retWork   = sumCount;

  // DEBUG:
  OS_TFPrintf( 3, "_GET_RESEARCH_TOTAL_COUNT ==> %d\n", *retWork );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief ����ɑ΂���, �����J�n���̉񓚐l�����擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetStartCountOfQuestion( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*       script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  MISC*              misc     = SaveData_GetMisc( save );

  int i;
  u16 qID;
  u16* retWork;

  // �R�}���h�̈������擾
  qID     = SCRCMD_GetVMWorkValue( core, work ); // ������: ����ID
  retWork = SCRCMD_GetVMWork( core, work );      // ������: �񓚐l���̊i�[��

  // �Z�[�u�f�[�^����, �Y�����鎿��ID�̒����J�n���̉񓚐l�����擾����
  for( i=0; i<MAX_QNUM_PER_RESEARCH_REQ; i++ )
  {
    if( MISC_GetResearchQuestionID(misc, i) == qID ) {
      *retWork = MISC_GetResearchStartCount( misc, i );
    }
  }

  // DEBUG:
  OS_TFPrintf( 3, "_GET_RESEARCH_TODAY_COUNT ==> %d\n", *retWork );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �w�肵������ɂ���, �ł������h�̉�ID���擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetMajorityAnswerOfQuestion( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*             work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*             script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*                gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*       save     = GAMEDATA_GetSaveControlWork( gameData );
  QUESTIONNAIRE_SAVE_WORK* qSave    = SaveData_GetQuestionnaire( save );

  u16 qID;
  u16* retWork;
  u16 majorityAnswerID;

  // �R�}���h�̈������擾
  qID     = SCRCMD_GetVMWorkValue( core, work ); // ������: ����ID
  retWork = SCRCMD_GetVMWork( core, work );      // ������: ��ID�̊i�[��

  GF_ASSERT( qID <= QUESTION_ID_MAX );

  // �ł������h�̉�ID����������
  {
    int ansNum;
    int ansIdx;
    int maxCount;

    ansNum = AnswerNum_question[ qID ];
    maxCount = 0;
    for( ansIdx=0; ansIdx < ansNum; ansIdx++ )
    {
      int totalCount = QuestionnaireWork_GetTotalAnswerNum( qSave, qID, ansIdx+1 );
      int todayCount = QuestionnaireWork_GetTotalAnswerNum( qSave, qID, ansIdx+1 );
      int sumCount   = totalCount + todayCount;
      if( maxCount <= sumCount ) {
        maxCount = sumCount;
        majorityAnswerID = AnswerID_question[ qID ][ ansIdx ];
      }
      OS_TFPrintf( 3, 
          "questionID=%d, answerIdx=%d, sumCount=%d(%d+%d)\n", 
          qID, ansIdx, sumCount, totalCount, todayCount );
    }
  }

  // �ł������h�̉�ID��Ԃ�
  *retWork = majorityAnswerID;

  // DEBUG:
  OS_TFPrintf( 3, "_GET_MAJORITY_ANSWER ==> %d\n", *retWork );

  return VMCMD_RESULT_CONTINUE;
} 

//-----------------------------------------------------------------------------
/**
 * @brief �w�肵������ɂ��Ă�, �����̉񓚂�ݒ肷��
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdSetMyAnswer( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*              work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*              script   = SCRCMD_WORK_GetScriptWork( work );
  GAMEDATA*                 gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK*        save     = GAMEDATA_GetSaveControlWork( gameData );
  QUESTIONNAIRE_SAVE_WORK*  qSave    = SaveData_GetQuestionnaire( save );
  QUESTIONNAIRE_ANSWER_WORK* myAnswer = Questionnaire_GetAnswerWork( qSave );
  u16 qID;
  u16 aIdx;

  // �R�}���h�̈������擾
  qID  = SCRCMD_GetVMWorkValue( core, work ); // ������: ����ID
  aIdx = SCRCMD_GetVMWorkValue( core, work ); // ������: ���Ԗڂ̉񓚂�

  // �񓚂��Z�b�g
  QuestionnaireAnswer_WriteBit( myAnswer, qID, aIdx );
  GAMEBEACON_SendDataUpdate_Questionnaire( myAnswer );

  // DEBUG:
  OS_TFPrintf( 3, "_SET_MY_ANSWER ==> qID=%d, aIdx=%d\n", qID, aIdx );

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief  �v���C���[�̑�������\������
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdDispResearchTeamInfo( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*  script   = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gameSystem = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( script, EVENT_DispResearchTeamInfo( gameSystem ) );
  return VMCMD_RESULT_SUSPEND;
}
