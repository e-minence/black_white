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

#include "field/research_team.h"
#include "field/research_team_def.h"
#include "savedata/misc.h" 
#include "savedata/questionnaire_save.h" 
#include "gamesystem/game_beacon.h"

#include "../../../resource/research_radar/data/question_id.h"            // for QUESTION_ID_xxxx
#include "../../../resource/research_radar/data/answer_num_question.cdat" // for AnswerNum_question[]
#include "../../../resource/research_radar/data/answer_id_question.cdat"  // for AnswerID_question[][]


//#define DEBUG_PRINT_ON


static void CheckCurrentResearchFinish( SAVE_CONTROL_WORK* save, u16* RState );
static void CheckRequestFinish_inAdvance( SAVE_CONTROL_WORK* save, u8 RID, u16* RState );
static u32 GetCurrentCount( QUESTIONNAIRE_SAVE_WORK* QSave, u8 QID );
static BOOL CheckNormCount( QUESTIONNAIRE_SAVE_WORK* QSave, u8 RID );
static BOOL CheckMaxCount( QUESTIONNAIRE_SAVE_WORK* QSave, u8 RID );
static BOOL CheckNormTime( SAVE_CONTROL_WORK* save );
static BOOL CheckResearchOne( SAVE_CONTROL_WORK* save );
static u16 GetLackTime( SAVE_CONTROL_WORK* save );
static u16 GetLackCount( SAVE_CONTROL_WORK* save );
static u16 GetPassedTime( SAVE_CONTROL_WORK* save );



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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_GET_RESEARCH_TEAM_RANK ==> %d\n", *ret );
#endif

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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_RESEARCH_TEAM_RANK_UP ==> %d\n", rank );
#endif

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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_GET_RESEARCH_REQUEST_ID ==> %d\n", *ret );
#endif

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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_GET_RESEARCH_QUESTION_ID ==> %d, %d, %d\n", *retQ1, *retQ2, *retQ3 );
#endif

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

  u16* ret;
  u16 passedHour;
  
  // �R�}���h�̈������擾
  ret = SCRCMD_GetVMWork( core, work ); // ������: �o�ߎ��Ԃ̊i�[��

  // �o�ߎ��Ԃ��Z�o
  passedHour = GetPassedTime( save );

  // �o�ߎ��Ԃ�Ԃ�
  *ret = passedHour;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_GET_RESEARCH_PASSED_TIME ==> %d\n", passedHour );
#endif

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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_START_RESEARCH: reqID=%d, qID[0]=%d, qID[1]=%d, qID[2]=%d\n", reqID, qID[0], qID[1], qID[2] );
#endif

  // �����˗�ID���Z�[�u
  MISC_SetResearchRequestID( misc, reqID );

 for( i=0; i<MAX_QNUM_PER_RESEARCH_REQ; i++ )
 {
   MISC_SetResearchQuestionID( misc, i, qID[i] ); // ����ID���Z�[�u
   MISC_SetResearchCount( misc, i, 0 ); // �����l�����N���A
 }

  // �����J�n�������Z�[�u
  MISC_SetResearchStartTimeBySecond( misc, GFL_RTC_GetDateTimeBySecond() );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_START_RESEARCH: start time=%d\n", GFL_RTC_GetDateTimeBySecond() );
#endif


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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_FINISH_RESEARCH\n" );
#endif

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
#ifdef DEBUG_PRINT_ON
      OS_TFPrintf( 3, 
          "questionID=%d, answerIdx=%d, sumCount=%d(%d+%d)\n", 
          qID, ansIdx, sumCount, totalCount, todayCount );
#endif
    }
  }

  // �ł������h�̉�ID��Ԃ�
  *retWork = majorityAnswerID;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_GET_MAJORITY_ANSWER ==> %d\n", *retWork );
#endif

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

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_SET_MY_ANSWER ==> qID=%d, aIdx=%d\n", qID, aIdx );
#endif

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

//-----------------------------------------------------------------------------
/**
 * @brief  �w�肵���˗���B�����Ă��邩�ǂ������`�F�b�N����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdCheckAchieveRequest( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  const MISC*        misc     = SaveData_GetMiscConst( save );

  u8 RID;
  u16* ret_wk;

  RID = SCRCMD_GetVMWorkValue( core, work ); // ������: �˗�ID
  ret_wk = SCRCMD_GetVMWork( core, work );   // ������: �˗��̒B���󋵂̊i�[��

  // �w�肳�ꂽ�˗������ݒ������ł���ꍇ
  if( RID == MISC_GetResearchRequestID(misc) ) {
    CheckCurrentResearchFinish( save, ret_wk );
  }
  // ���ݒ������łȂ��˗��̏ꍇ
  else {
    CheckRequestFinish_inAdvance( save, RID, ret_wk );
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, "_CHECK_ACHIEVE_REQUEST ==> " );
  switch( *ret_wk ) {
  case RESEARCH_REQ_STATE_NULL:             OS_TFPrintf( 3, "NULL" );             break;
  case RESEARCH_REQ_STATE_ACHIEVE:          OS_TFPrintf( 3, "ACHIEVE" );          break;
  case RESEARCH_REQ_STATE_FALSE_NORM_COUNT: OS_TFPrintf( 3, "FALSE_NORM_COUNT" ); break;
  case RESEARCH_REQ_STATE_FALSE_NORM_TIME:  OS_TFPrintf( 3, "FALSE_NORM_TIME" );  break;
  case RESEARCH_REQ_STATE_FALSE_ZERO_COUNT: OS_TFPrintf( 3, "FALSE_ZERO_COUNT" ); break;
  default: GF_ASSERT(0);
  }
  OS_TFPrintf( 3, "\n" );
#endif

  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief  �������̈˗��̕s���l���擾����
 * @param  core
 * @param  wk
 * @retval VMCMD_RESULT
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdGetLackForAchieve( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work     = (SCRCMD_WORK*)wk;
  GAMEDATA*          gameData = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK* save     = GAMEDATA_GetSaveControlWork( gameData );
  const MISC*        misc     = SaveData_GetMiscConst( save );

  u16* ret_wk;
  u16 lack;
  u8 RID;
  u8 RType;

  // �R�}���h�������擾
  ret_wk = SCRCMD_GetVMWork( core, work ); // ������: �s���l�̊i�[��

  // �����^�C�v���擾
  RID = MISC_GetResearchRequestID( misc );
  RType = RESEARCH_TEAM_GetResearchType( RID );

  // �B���s���l���擾
  switch( RType ) {
  case RESEARCH_REQ_TYPE_TIME:  lack = GetLackTime( save );  break;
  case RESEARCH_REQ_TYPE_COUNT: lack = GetLackCount( save ); break;
  default: GF_ASSERT(0);
  }

  *ret_wk = lack;
  return VMCMD_RESULT_CONTINUE;
}

//=============================================================================
/**
 * @brief �������̈˗��̒B�����`�F�b�N����
 *
 * @param save
 * @param RState �����󋵂̊i�[��
 */
//=============================================================================
static void CheckCurrentResearchFinish( SAVE_CONTROL_WORK* save, u16* RState )
{
  QUESTIONNAIRE_SAVE_WORK* QSave = SaveData_GetQuestionnaire( save );
  const MISC*              misc  = SaveData_GetMiscConst( save );

  u8 RID;
  u8 RType;

  RID = MISC_GetResearchRequestID( misc ); // �����˗�ID���擾
  RType = RESEARCH_TEAM_GetResearchType( RID ); // �˗��^�C�v���擾

  // �l�������̏ꍇ
  if( RType == RESEARCH_REQ_TYPE_COUNT ) {
    // �m���}�B��
    if( CheckNormCount( QSave, RID ) ) {
      *RState = RESEARCH_REQ_STATE_ACHIEVE;
    }
    // �m���}���B��
    else {
      *RState = RESEARCH_REQ_STATE_FALSE_NORM_COUNT;
    }
  }
  // ���Ԓ����̏ꍇ
  else if( RType == RESEARCH_REQ_TYPE_TIME ) {
    // �m���}���Ԃ��o��
    if( CheckNormTime( save ) ) {
      // 1�l�ȏ㒲������
      if( CheckResearchOne( save ) ) {
        *RState = RESEARCH_REQ_STATE_ACHIEVE;
      }
      // 1�l���������Ă��Ȃ�
      else {
        *RState = RESEARCH_REQ_STATE_FALSE_ZERO_COUNT;
      }
    }
    // �m���}���Ԃ��o�߂��Ă��Ȃ�
    else {
      *RState = RESEARCH_REQ_STATE_FALSE_NORM_TIME;
    }
  }
  else {
    GF_ASSERT(0);
  }
}

//=============================================================================
/**
 * @brief �w�肵���˗������łɒB�����Ă��邩�ǂ������`�F�b�N����
 *
 * @param save
 * @param RID    �`�F�b�N����˗���ID
 * @param RState �����󋵂̊i�[��
 */
//=============================================================================
static void CheckRequestFinish_inAdvance( SAVE_CONTROL_WORK* save, u8 RID, u16* RState )
{
  QUESTIONNAIRE_SAVE_WORK* QSave = SaveData_GetQuestionnaire( save );
  const MISC*              misc  = SaveData_GetMiscConst( save );

  u8 RType;

  RType = RESEARCH_TEAM_GetResearchType( RID ); // �˗��^�C�v���擾

  // �l�������̏ꍇ
  if( RType == RESEARCH_REQ_TYPE_COUNT ) {
    // �m���}�B��
    if( CheckNormCount( QSave, RID ) ) {
      *RState = RESEARCH_REQ_STATE_ACHIEVE;
    }
    // �m���}���B��
    else {
      *RState = RESEARCH_REQ_STATE_FALSE_NORM_COUNT;
    }
  }
  // ���Ԓ����̏ꍇ
  else if( RType == RESEARCH_REQ_TYPE_TIME ) {
    // MAX�J�E���g�܂Œ������Ă���
    if( CheckMaxCount( QSave, RID ) ) {
      *RState = RESEARCH_REQ_STATE_ACHIEVE;
    }
    // �m���}���Ԃ��o�߂��Ă��Ȃ�
    else {
      *RState = RESEARCH_REQ_STATE_FALSE_NORM_TIME;
    }
  }
  else {
    GF_ASSERT(0);
  }
}

//=============================================================================
/**
 * @brief ���݂̒����l�����擾����
 *
 * @param QID ����ID ( QUESTION_ID_xxxx )
 *
 * @return �w�肵�������, �����_�ł̒����l�� ( ���܂܂� + ���� )
 */
//=============================================================================
static u32 GetCurrentCount( QUESTIONNAIRE_SAVE_WORK* QSave, u8 QID )
{
  u32 today = QuestionnaireWork_GetTodayCount( QSave, QID );
  u32 total = QuestionnaireWork_GetTotalCount( QSave, QID );
  u32 current = today + total;
  if( 99999 < current ) { current = 99999; }
  return current;
}

//=============================================================================
/**
 * @brief �m���}�l���̒B�����`�F�b�N����
 *
 * @param QSave
 * @param RID   �`�F�b�N���钲���˗���ID ( RESEARCH_REQ_ID_xxxx )
 *
 * @return �w�肵���˗���B�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//=============================================================================
static BOOL CheckNormCount( QUESTIONNAIRE_SAVE_WORK* QSave, u8 RID )
{
  int i;
  int norm;
  u8 QID[ MAX_QNUM_PER_RESEARCH_REQ ];
  u8 QNum;

  RESEARCH_TEAM_GetQuestionID( RID, QID );
  QNum = RESEARCH_TEAM_GetQuestionNum( RID );
  norm = RESEARCH_TEAM_GetNormCount( RID );

  for( i=0; i<QNum; i++ )
  {
    u32 count = GetCurrentCount( QSave, QID[i] );

#ifdef DEBUG_PRINT_ON
    OS_TFPrintf( 3, 
        "CheckNormCount: QID=%d, norm=%d, count=%d\n", QID[i], norm, count );
#endif

    // �m���}��B�����Ă��Ȃ�
    if( count < norm ) {
      return FALSE;
    }
  } 
  return TRUE;
}

//=============================================================================
/**
 * @brief �������̈˗��ɂ���, �m���}���Ԃ̒B�����`�F�b�N����
 *
 * @param save
 *
 * @return �������̈˗����m���}���Ԃ�B�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//=============================================================================
static BOOL CheckNormTime( SAVE_CONTROL_WORK* save )
{
  QUESTIONNAIRE_SAVE_WORK* QSave = SaveData_GetQuestionnaire( save );
  const MISC*              misc  = SaveData_GetMiscConst( save );

  int RID;
  int pass;
  int norm;

  RID  = MISC_GetResearchRequestID( misc );
  pass = GetPassedTime( save ); // �o�ߎ��Ԃ��擾
  norm = RESEARCH_TEAM_GetNormTime( RID ); // �m���}���Ԃ��擾

  GF_ASSERT( RESEARCH_TEAM_GetResearchType(RID) == RESEARCH_REQ_TYPE_TIME );

  // �o�߂��Ă���
  if( norm <= pass ) {
    return TRUE;
  }
  // �o�߂��Ă��Ȃ�
  else {
    return FALSE;
  }
}

//=============================================================================
/**
 * @brief �������̈˗��ɂ���, �����̊J�n����P�l�ł������������ǂ������`�F�b�N����
 *
 * @param save
 *
 * @return �������̈˗����J�n���Ă���, �P�l�ȏ�̒������s���Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//=============================================================================
static BOOL CheckResearchOne( SAVE_CONTROL_WORK* save )
{
  int i;
  u8 RID;
  u8 QNum;
  const MISC* misc;

  // �������̎��␔���擾
  misc = SaveData_GetMiscConst( save );
  RID  = MISC_GetResearchRequestID( misc );
  QNum = RESEARCH_TEAM_GetQuestionNum( RID );

  // �����J�n�����, �e����̒����l�����`�F�b�N
  for( i=0; i<QNum; i++ )
  {
    // �P�l���������Ă��Ȃ�����𔭌�
    if( MISC_GetResearchCount( misc, i ) == 0 ) {
      return FALSE;
    }
  }
  return TRUE;
}

//=============================================================================
/**
 * @brief MAX�J�E���g�̒B�����`�F�b�N����
 *
 * @param QSave
 * @param RID  �`�F�b�N����˗���ID
 *
 * @return �w�肵���˗��ɊY������S�����MAX�J�E���g�܂Œ������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//=============================================================================
static BOOL CheckMaxCount( QUESTIONNAIRE_SAVE_WORK* QSave, u8 RID )
{
  int i;
  u8 QID[ MAX_QNUM_PER_RESEARCH_REQ ];
  u8 QNum;
  int norm;

  RESEARCH_TEAM_GetQuestionID( RID, QID );
  QNum = RESEARCH_TEAM_GetQuestionNum( RID );
  norm = RESEARCH_TEAM_GetNormCount( RID );

  for( i=0; i<QNum; i++ )
  {
    u32 count = GetCurrentCount( QSave, QID[i] );

    // MAX�J�E���g�ɒB���Ă��Ȃ�
    if( count < 99999 ) {
      return FALSE;
    }
  } 
  return TRUE;
}

//=============================================================================
/**
 * @brief �������̈˗��̃m���}�B���s���l�����擾����
 *
 * @param save
 *
 * @return �������̈˗��ɂ���, �m���}�B���܂ł̕s���l��
 */
//=============================================================================
static u16 GetLackCount( SAVE_CONTROL_WORK* save )
{
  QUESTIONNAIRE_SAVE_WORK* QSave = SaveData_GetQuestionnaire( save );
  const MISC*              misc  = SaveData_GetMiscConst( save );

  int i;
  u8 RID;
  u8 QID[ MAX_QNUM_PER_RESEARCH_REQ ];
  u8 QNum;
  u32 norm;
  u32 lack;
  u32 minCount;

  RID  = MISC_GetResearchRequestID( misc );
  norm = RESEARCH_TEAM_GetNormCount( RID );
  QNum = RESEARCH_TEAM_GetQuestionNum( RID );
  RESEARCH_TEAM_GetQuestionID( RID, QID );

  // ��ԏ��Ȃ������l�����擾
  minCount = 99999;
  for( i=0; i<QNum; i++ )
  {
    u32 count = GetCurrentCount( QSave, QID[i] );
    if( count < minCount ) { minCount = count; }
  }

  lack = norm - minCount;
  if( lack < 0 ) { lack = 0; }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( 3, 
      "GetLackCount: RID=%d, norm=%d, minCount=%d, lack=%d\n", 
      RID, norm, minCount, lack );
#endif

  return lack;
}

//=============================================================================
/**
 * @brief �������̈˗��̃m���}�B���s�����Ԃ��擾����
 */
//=============================================================================
static u16 GetLackTime( SAVE_CONTROL_WORK* save )
{
  QUESTIONNAIRE_SAVE_WORK* QSave = SaveData_GetQuestionnaire( save );
  const MISC*              misc  = SaveData_GetMiscConst( save );

  int RID;
  int pass;
  int norm;
  int lack;

  RID  = MISC_GetResearchRequestID( misc );
  pass = GetPassedTime( save ); // �o�ߎ��Ԃ��擾
  norm = RESEARCH_TEAM_GetNormTime( RID ); // �m���}���Ԃ��擾

  // �s�����Ԃ��Z�o
  lack = norm - pass; 
  if( lack < 0 ) { lack = 0; }
  return lack;
}

//=============================================================================
/**
 * @brief �������̈˗��̌o�ߎ��Ԃ��擾����
 */
//=============================================================================
static u16 GetPassedTime( SAVE_CONTROL_WORK* save )
{
  MISC* misc = SaveData_GetMisc( save );

  s64 startSec;
  u16 passedHour;

  // �������łȂ�
  if( MISC_GetResearchRequestID( misc ) == RESEARCH_REQ_ID_NONE ) {
    GF_ASSERT(0);
    return 0;
  }
  
  // �����̊J�n�������擾
  startSec = MISC_GetResearchStartTimeBySecond( misc );

  { 
    s64 nowSec, passedSec;
    RTCDate passedDate, nowDate;
    RTCTime passedTime, nowTime;

    // ���ݎ������擾
    GFL_RTC_GetDateTime( &nowDate, &nowTime );
    nowSec = RTC_ConvertDateTimeToSecond( &nowDate, &nowTime );

    // �o�ߎ���[h]���Z�o
    passedSec = nowSec - startSec;
    RTC_ConvertSecondToDateTime( &passedDate, &passedTime, passedSec );

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

  return passedHour;
}



























