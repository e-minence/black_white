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

#include "field/research_team_def.h"
#include "savedata/misc.h" 
#include "savedata/questionnaire_save.h" 

#include "../../../resource/research_radar/data/question_id.h"            // for QUESTION_ID_xxxx
#include "../../../resource/research_radar/data/answer_num_question.cdat" // for AnswerNum_question[]
#include "../../../resource/research_radar/data/answer_id_question.cdat"  // for AnswerID_question[][]


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

  //--�������łȂ�--
  if( MISC_GetResearchRequestID( misc ) == RESEARCH_REQ_ID_NONE ) {
    passedHour = 0;
  }
  //--������--
  else { 
    s64 nowSec, passedSec;
    RTCDate passedDate;
    RTCTime passedTime;

    // �o�ߎ���[h]���Z�o
    nowSec = GFL_RTC_GetDateTimeBySecond();
    passedSec = nowSec - startSec;
    RTC_ConvertSecondToDateTime( &passedDate, &passedTime, passedSec );
    if( (0 < passedDate.year) || 
        (0 < passedDate.month) || 
        (0 < passedDate.day) )
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

  // �����˗�ID���Z�[�u
  MISC_SetResearchRequestID( misc, reqID );

 for( i=0; i<MAX_QNUM_PER_RESEARCH_REQ; i++ )
 {
   // ����ID���Z�[�u
   MISC_SetResearchQuestionID( misc, i, qID[i] );

   // �_�~�[ID
   if( qID[i] == QUESTION_ID_DUMMY ) { continue; }

   // �J�n���̉񓚐l�����Z�[�u
   {
     int count = QuestionnaireWork_GetTotalCount( qSave, qID[i] );
     MISC_SetResearchStartCount( misc, i, count );
   }
 }

  // �����J�n�������Z�[�u
  MISC_SetResearchStartTimeBySecond( misc, GFL_RTC_GetDateTimeBySecond() );

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

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief ����ɑ΂���, ���܂܂ł̉񓚐l�����擾����
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

  // �R�}���h�̈������擾
  qID     = SCRCMD_GetVMWorkValue( core, work ); // ������: ����ID
  retWork = SCRCMD_GetVMWork( core, work );      // ������: �񓚐l���̊i�[��

  // ���܂܂ł̉񓚐l�����擾
  *retWork = QuestionnaireWork_GetTotalCount( qSave, qID );

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
    int count;

    ansNum = AnswerNum_question[ qID ];
    maxCount = 0;
    for( ansIdx=0; ansIdx < ansNum; ansIdx++ )
    {
      count = QuestionnaireWork_GetTotalAnswerNum( qSave, qID, ansIdx - 1 );
      if( maxCount <= count ) {
        maxCount = count;
        majorityAnswerID = AnswerID_question[ qID ][ ansIdx ];
      }
    }
  }

  // �ł������h�̉�ID��Ԃ�
  *retWork = majorityAnswerID;

  return VMCMD_RESULT_CONTINUE;
}
