//============================================================================================
/**
 * @file  event_gameclear.c
 * @brief �C�x���g�F�Q�[���N���A����
 * @date  2009.12.14
 * @author  tamada GAME FREAK inc.
 *
 * 2009.12.14 event_gameover.c����R�s�y�ō쐬
 *
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"  // for HEAPID_xxxx
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"
#include "sound/pm_sndsys.h"    // for PMSND_xxxx
#include "app/local_tvt_sys.h"  // for LocalTvt_ProcData etc.  
#include "demo/demo3d.h"        // for Demo3DProcData etc.
#include "demo/demo3d_demoid.h" // for DEMO3D_ID_xxxx
#include "demo/dendou_demo.h"
#include "demo/staff_roll.h"
#include "demo/the_end.h"

#include "script.h"           // for SCRIPT_CallGameClearScript
#include "event_gameclear.h"  // for GAMECLEAR_MODE
#include "event_field_fade.h"
#include "event_fieldmap_control_local.h"
#include "event_gamestart.h"
#include "event_demo3d.h"
#include "move_pokemon.h" 
#include "proc_gameclear_save.h"


//==============================================================================================
//==============================================================================================
#define MAX_SEQ_NUM (16) // ���s����V�[�P���X�̍ő吔
#define WAIT_FRAME (120)


//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {

  GAMESYS_WORK*       gsys;
  GAMEDATA*           gamedata;
  GAMECLEAR_MODE      clear_mode;
  BOOL                saveSuccessFlag;
  const MYSTATUS*     mystatus; 
  GAMECLEAR_MSG_PARAM para_child;
  DENDOUDEMO_PARAM    dendouDemoParam;    // �a������ �f���̃p�����[�^
  STAFFROLL_DATA      staffRollDemoParam; // �X�^�b�t���[�� �f���̃p�����[�^
  THE_END_PARAM       theEndDemoParam;    // �uTHE END�v�f����ʗp�̃p�����[�^

  int nowSeq; // ���݂̃V�[�P���X
  int seqArray[ MAX_SEQ_NUM ]; // ���s�V�[�P���X�z��
  int waitCount; // �҂��t���[�����J�E���^

} GAMECLEAR_WORK;


//============================================================================================
//============================================================================================
// ���C���V�[�P���X
enum {
  GMCLEAR_SEQ_INIT,             // ������
  GMCLEAR_SEQ_FADEOUT,          // �t�F�[�h�A�E�g
  GMCLEAR_SEQ_COMM_END_REQ,     // �ʐM�I�����N�G�X�g���s
  GMCLEAR_SEQ_COMM_END_WAIT,    // �ʐM�I���҂�
  GMCLEAR_SEQ_FIELD_CLOSE_WAIT, // �t�B�[���h�}�b�v�I���҂�
  GMCLEAR_SEQ_DENDOU_DEMO,      // �a������f��
  GMCLEAR_SEQ_DENDOU_DEMO_WAIT, // �a������f���I���҂�
  GMCLEAR_SEQ_STAFF_ROLL,       // �X�^�b�t���[��
  GMCLEAR_SEQ_STAFF_ROLL_WAIT,  // �X�^�b�t���[���I���҂�
  GMCLEAR_SEQ_ENDING_DEMO,      // �G���f�B���O�f��
  GMCLEAR_SEQ_CLEAR_SCRIPT,     // �Q�[���N���A�X�N���v�g����
  GMCLEAR_SEQ_SAVE_MESSAGE,     // �Z�[�u�����b�Z�[�W�\��
  GMCLEAR_SEQ_FRAME_WAIT,       // �t���[���o�ߑ҂�
  GMCLEAR_SEQ_THE_END,          //�uTHE END�v�\��
  GMCLEAR_SEQ_END,              // �I��
};


//============================================================================================
//============================================================================================
static void SetupDendouDemoParam( GAMECLEAR_WORK* work ); // �a������f���̃p�����[�^���Z�b�g�A�b�v����
static void SetupStaffRollDemoParam( GAMECLEAR_WORK* work ); // �X�^�b�t���[���f���̃p�����[�^���Z�b�g�A�b�v����
static void SetupSequence( GAMECLEAR_WORK* work ); // �V�[�P���X�̗�����Z�b�g�A�b�v����
static u16 GetEndingDemoID( void ); // �G���f�B���O��3D�f��ID���擾����
static void NowSeqFinish( GAMECLEAR_WORK* work, int* seq ); // ���݂̃V�[�P���X���I������
static void ElboardStartChampNews( GAMECLEAR_WORK* work ); // �d���f���Ƀ`�����s�I���j���[�X�̕\�����J�n����


//============================================================================================
//============================================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �Q�[���N���A�C�x���g
 * @param event   �C�x���g���䃏�[�N�ւ̃|�C���^
 * @retval  TRUE    �C�x���g�I��
 * @retval  FALSE   �C�x���g�p����
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameClear(GMEVENT * event, int * seq, void *wk)
{
  GAMECLEAR_WORK * work = wk;
  GMEVENT * call_event;
  GAMESYS_WORK * gsys = work->gsys;
  GAMEDATA * gamedata = work->gamedata;
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( gamedata );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( work->gsys );
  GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( work->gsys );

  switch( work->nowSeq ) {
  case GMCLEAR_SEQ_INIT:
    if( work->clear_mode == GAMECLEAR_MODE_DENDOU ) {
      ElboardStartChampNews( wk ); // �d���f���Ƀ`�����s�I���j���[�X��\��
    }
    PMSND_FadeOutBGM( 30 );
    NowSeqFinish( work, seq );
    break;

  // �t�B�[���h�}�b�v���t�F�[�h�A�E�g
  case GMCLEAR_SEQ_FADEOUT:
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeOut_Black( gsys, fieldmap, FIELD_FADE_WAIT ) ); 
    NowSeqFinish( work, seq );
    break;

  // �ʐM�I�����N�G�X�g���s
  case GMCLEAR_SEQ_COMM_END_REQ:
    //�ʐM�������Ă���ꍇ�͏I��������
    if(GameCommSys_BootCheck(gameComm) != GAME_COMM_NO_NULL){
      GameCommSys_ExitReq(gameComm);
    }
    NowSeqFinish( work, seq );
    break;

  // �ʐM�I���҂�
  case GMCLEAR_SEQ_COMM_END_WAIT:
    if( GameCommSys_BootCheck(gameComm) != GAME_COMM_NO_NULL ) {
      break;
    }
    NowSeqFinish( work, seq );
    break;

  // �t�B�[���h�}�b�v���I���҂�
  case GMCLEAR_SEQ_FIELD_CLOSE_WAIT:
    GMEVENT_CallEvent( event, EVENT_FieldClose_FieldProcOnly( gsys, fieldmap ) );
    NowSeqFinish( work, seq );
    break;

  // �a������f���Ăяo��
  case GMCLEAR_SEQ_DENDOU_DEMO:
    GAMESYSTEM_CallProc( gsys, 
        FS_OVERLAY_ID(dendou_demo), &DENDOUDEMO_ProcData, &work->dendouDemoParam );
    NowSeqFinish( work, seq );
    break; 
  // �a������f���I���҂�
  case GMCLEAR_SEQ_DENDOU_DEMO_WAIT:  
    if( GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL ) { break; }
    NowSeqFinish( work, seq );
    break;

  // �X�^�b�t���[�� �f���Ăяo��
  case GMCLEAR_SEQ_STAFF_ROLL:
    GAMESYSTEM_CallProc( gsys, 
        FS_OVERLAY_ID(staff_roll), &STAFFROLL_ProcData, &work->staffRollDemoParam );
    NowSeqFinish( work, seq );
    break; 

  // �X�^�b�t���[���I���҂�
  case GMCLEAR_SEQ_STAFF_ROLL_WAIT:
    if( GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL ) { break; }
    NowSeqFinish( work, seq );
    break;

  // �G���f�B���O�f��
  case GMCLEAR_SEQ_ENDING_DEMO:
    GMEVENT_CallEvent( event, 
        EVENT_CallDemo3D( gsys, event,  GetEndingDemoID(), 0, FALSE ) );
    NowSeqFinish( work, seq );
    break;

  case GMCLEAR_SEQ_CLEAR_SCRIPT:
    SCRIPT_CallGameClearScript( gsys, HEAPID_PROC ); 
    NowSeqFinish( work, seq );
    break; 
  case GMCLEAR_SEQ_SAVE_MESSAGE:
    GMEVENT_CallProc( event, 
        FS_OVERLAY_ID(gameclear_demo), &GameClearMsgProcData, &work->para_child );
    NowSeqFinish( work, seq );
    break;

  case GMCLEAR_SEQ_FRAME_WAIT:
    if( WAIT_FRAME < work->waitCount ) {
      work->waitCount = 0;
      NowSeqFinish( work, seq );
    }
    work->waitCount++;
    break;

  case GMCLEAR_SEQ_THE_END:
    GMEVENT_CallProc( event, 
        FS_OVERLAY_ID(the_end), &THE_END_ProcData, &work->theEndDemoParam );
    NowSeqFinish( work, seq );
    break;

  case GMCLEAR_SEQ_END:
    return GMEVENT_RES_FINISH;

  case GMCLEAR_SEQ_END + 1:
    /* �������[�v */
    return GMEVENT_RES_CONTINUE;

  }
  return GMEVENT_RES_CONTINUE;
}


//-----------------------------------------------------------------------------
/**
 * @brief �C�x���g�R�}���h�F�ʏ�S�ŏ���
 * @param event   �C�x���g���䃏�[�N�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_GameClear( GAMESYS_WORK * gsys, GAMECLEAR_MODE mode )
{
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  GMEVENT * event;
  GAMECLEAR_WORK * gcwk;

  event = GMEVENT_Create( gsys, NULL, GMEVENT_GameClear, sizeof(GAMECLEAR_WORK) );

  gcwk = GMEVENT_GetEventWork( event );
  gcwk->gsys = gsys;
  gcwk->gamedata = gamedata;
  gcwk->clear_mode = mode;
  gcwk->mystatus = GAMEDATA_GetMyStatus( gamedata );
  gcwk->waitCount = 0;

  //�q�v���Z�X�ɓn���p�����[�^
  gcwk->para_child.gsys = gsys;
  gcwk->para_child.clear_mode = mode;

  //�ړ��|�P���������`�F�b�N
  MP_RecoverMovePoke( gamedata );

  // �f���p�����[�^��ݒ�
  SetupDendouDemoParam( gcwk );    // �a������
  SetupStaffRollDemoParam( gcwk ); // �X�^�b�t���[��

  // ���s����V�[�P���X�̗��������
  SetupSequence( gcwk );

  return event;
} 



//==============================================================================================
//==============================================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �a������f���̃p�����[�^���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetupDendouDemoParam( GAMECLEAR_WORK* work )
{
  work->dendouDemoParam.party    = GAMEDATA_GetMyPokemon( work->gamedata );
  work->dendouDemoParam.mystatus = GAMEDATA_GetMyStatus( work->gamedata );
  work->dendouDemoParam.ptime    = GAMEDATA_GetPlayTimeWork( work->gamedata );
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�^�b�t���[���̃f���p�����[�^���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetupStaffRollDemoParam( GAMECLEAR_WORK* work )
{
  // �a������̏ꍇ�̓X�L�b�v�\
  work->staffRollDemoParam.fastMode = ( work->clear_mode == GAMECLEAR_MODE_DENDOU );

  // �������[�h��ݒ�
  {
    SAVE_CONTROL_WORK* save;
    MOJIMODE mojiMode;
    CONFIG* config;
    
    save = GAMEDATA_GetSaveControlWork( work->gamedata );
    config = SaveData_GetConfig( save );
    mojiMode = CONFIG_GetMojiMode( config );

    work->staffRollDemoParam.mojiMode = mojiMode;

    // �uTHE END�v��ʗp�̐ݒ�
    work->theEndDemoParam.mojiMode    = mojiMode;
  }
  
  
}

//-----------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�̗�����Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetupSequence( GAMECLEAR_WORK* work )
{
  int pos = 0;

  switch( work->clear_mode ) {
  default: GF_ASSERT(0);
  case GAMECLEAR_MODE_FIRST:
    work->seqArray[pos++] = GMCLEAR_SEQ_INIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_COMM_END_REQ;
    work->seqArray[pos++] = GMCLEAR_SEQ_COMM_END_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_FIELD_CLOSE_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_STAFF_ROLL;
    work->seqArray[pos++] = GMCLEAR_SEQ_STAFF_ROLL_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_FRAME_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_ENDING_DEMO;
    work->seqArray[pos++] = GMCLEAR_SEQ_FRAME_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_CLEAR_SCRIPT;
    work->seqArray[pos++] = GMCLEAR_SEQ_SAVE_MESSAGE;
    work->seqArray[pos++] = GMCLEAR_SEQ_FRAME_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_THE_END;
    work->seqArray[pos++] = GMCLEAR_SEQ_END;
    break;
  case GAMECLEAR_MODE_DENDOU:
    work->seqArray[pos++] = GMCLEAR_SEQ_INIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_COMM_END_REQ;
    work->seqArray[pos++] = GMCLEAR_SEQ_FADEOUT;
    work->seqArray[pos++] = GMCLEAR_SEQ_COMM_END_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_FIELD_CLOSE_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_DENDOU_DEMO;
    work->seqArray[pos++] = GMCLEAR_SEQ_DENDOU_DEMO_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_CLEAR_SCRIPT;
    work->seqArray[pos++] = GMCLEAR_SEQ_SAVE_MESSAGE;
    work->seqArray[pos++] = GMCLEAR_SEQ_STAFF_ROLL;
    work->seqArray[pos++] = GMCLEAR_SEQ_STAFF_ROLL_WAIT;
    work->seqArray[pos++] = GMCLEAR_SEQ_THE_END;
    work->seqArray[pos++] = GMCLEAR_SEQ_END;
    break;
  }

  work->nowSeq = work->seqArray[0];
}

//-----------------------------------------------------------------------------
/**
 * @brief �G���f�B���O��3D�f��ID���擾����
 * 
 * @return �G���f�B���O�̃f��ID
 */
//-----------------------------------------------------------------------------
static u16 GetEndingDemoID( void )
{
  u16 demo_id;

  switch( GetVersion() ) {
  default: GF_ASSERT(0);
  case VERSION_WHITE: demo_id = DEMO3D_ID_ENDING_W; break;
  case VERSION_BLACK: demo_id = DEMO3D_ID_ENDING_B; break;
  }

  return demo_id;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���݂̃V�[�P���X���I������
 */
//-----------------------------------------------------------------------------
static void NowSeqFinish( GAMECLEAR_WORK* work, int* seq )
{
  (*seq)++;
  work->nowSeq = work->seqArray[ *seq ];
}

//-----------------------------------------------------------------------------
/**
 * @brief �d���f���Ƀ`�����s�I���j���[�X�̕\�����J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ElboardStartChampNews( GAMECLEAR_WORK* work )
{
  MISC* misc;
  int minutes;

  misc = GAMEDATA_GetMiscWork( work->gamedata );
  minutes = 60 * 24; // �\�����鎞��[min]
  MISC_SetChampNewsMinutes( misc, minutes );
}
