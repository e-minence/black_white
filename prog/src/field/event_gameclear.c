//============================================================================================
/**
 * @file	event_gameclear.c
 * @brief	�C�x���g�F�Q�[���N���A����
 * @date  2009.12.14
 * @author	tamada GAME FREAK inc.
 *
 * 2009.12.14 event_gameover.c����R�s�y�ō쐬
 *
 * @todo
 * �Q�[���I�[�o�[���b�Z�[�W��PROC�ɂ����̂Ń�����������Ȃ��Ƃ��ɂ�
 * �ʃI�[�o�[���C�̈�Ɉ��z������
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
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA* gamedata;
  GAMECLEAR_MODE clear_mode;
  BOOL saveSuccessFlag;
  const MYSTATUS * mystatus; 
  GAMECLEAR_MSG_PARAM para_child;
  DENDOUDEMO_PARAM dendouDemoParam; // �a������f���E�p�����[�^
}GAMECLEAR_WORK;


//============================================================================================
//============================================================================================
// ���C���V�[�P���X
enum {
	GMCLEAR_SEQ_INIT,			        // ������
  GMCLEAR_SEQ_FADEOUT,          // �t�F�[�h�A�E�g
  GMCLEAR_SEQ_COMM_END_WAIT,    // �ʐM�I���҂�
  GMCLEAR_SEQ_FIELD_CLOSE_WAIT, // �t�B�[���h�}�b�v�I���҂�
	GMCLEAR_SEQ_DENDOU_DEMO,	    // �a������f��
  GMCLEAR_SEQ_DENDOU_DEMO_WAIT, // �a������f���I���҂�
  GMCLEAR_SEQ_ENDING_DEMO,      // �G���f�B���O�f��
	GMCLEAR_SEQ_CLEAR_SCRIPT,	    // �Q�[���N���A�X�N���v�g����
	GMCLEAR_SEQ_SAVE_MESSAGE,	    // �Z�[�u�����b�Z�[�W�\��
	GMCLEAR_SEQ_END,				      // �I��
};


//============================================================================================
//============================================================================================
static u16 GetEndingDemoID( void ); // �G���f�B���O��3D�f��ID���擾����


//============================================================================================
//============================================================================================

//-----------------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�C�x���g
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
 * @retval	TRUE		�C�x���g�I��
 * @retval	FALSE		�C�x���g�p����
 *
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT GMEVENT_GameClear(GMEVENT * event, int * seq, void *work)
{
  GAMECLEAR_WORK * gcwk = work;
  GMEVENT * call_event;
  GAMESYS_WORK * gsys = gcwk->gsys;
  GAMEDATA * gamedata = gcwk->gamedata;
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( gamedata );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gcwk->gsys );
  GAME_COMM_SYS_PTR gameComm = GAMESYSTEM_GetGameCommSysPtr( gcwk->gsys );

	switch (*seq) {
	case GMCLEAR_SEQ_INIT:
    PMSND_FadeOutBGM( 30 );
		(*seq) ++;
		break;

  // �t�B�[���h�}�b�v���t�F�[�h�A�E�g
  case GMCLEAR_SEQ_FADEOUT:
    GMEVENT_CallEvent( event, 
        EVENT_FieldFadeOut_Black( gsys, fieldmap, FIELD_FADE_WAIT ) );

    //�ʐM�������Ă���ꍇ�͏I��������
    if(GameCommSys_BootCheck(gameComm) != GAME_COMM_NO_NULL){
      GameCommSys_ExitReq(gameComm);
    }
		(*seq) ++;
    break;

  // �ʐM�I���҂�
  case GMCLEAR_SEQ_COMM_END_WAIT:
    if( GameCommSys_BootCheck(gameComm) != GAME_COMM_NO_NULL ) {
      break;
    }
		(*seq) ++;
    break;

  // �t�B�[���h�}�b�v���I���҂�
  case GMCLEAR_SEQ_FIELD_CLOSE_WAIT:
    GMEVENT_CallEvent( event, EVENT_FieldClose_FieldProcOnly( gsys, fieldmap ) );
    
    // ����̓G���f�B���O��
    if( gcwk->clear_mode == GAMECLEAR_MODE_FIRST ) {
      *seq = GMCLEAR_SEQ_ENDING_DEMO;
    }
    // 2��ڈȍ~�͓a������f����
    else {
      *seq = GMCLEAR_SEQ_DENDOU_DEMO;
    }
    break;

  // �a������f���Ăяo��
  case GMCLEAR_SEQ_DENDOU_DEMO:
    GAMESYSTEM_CallProc( gsys, 
        FS_OVERLAY_ID(dendou_demo), &DENDOUDEMO_ProcData, &gcwk->dendouDemoParam );
		(*seq) ++;
		break; 
  // �a������f���I���҂�
	case GMCLEAR_SEQ_DENDOU_DEMO_WAIT:  
		if( GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL ) { break; }
    *seq = GMCLEAR_SEQ_CLEAR_SCRIPT;
		break;

  // �G���f�B���O�f��
  case GMCLEAR_SEQ_ENDING_DEMO:
    GMEVENT_CallEvent( event, 
        EVENT_CallDemo3D( gsys, event,  GetEndingDemoID(), 0, FALSE ) );
    *seq = GMCLEAR_SEQ_CLEAR_SCRIPT;
    break;

	case GMCLEAR_SEQ_CLEAR_SCRIPT:
    SCRIPT_CallGameClearScript( gsys, HEAPID_PROC ); 
		(*seq) ++;
		break;

  case GMCLEAR_SEQ_SAVE_MESSAGE:
    GMEVENT_CallProc(
        event, FS_OVERLAY_ID(gameclear_demo), &GameClearMsgProcData, &gcwk->para_child );
    (*seq) ++;
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
 * @brief	�C�x���g�R�}���h�F�ʏ�S�ŏ���
 * @param	event		�C�x���g���䃏�[�N�ւ̃|�C���^
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

  //�q�v���Z�X�ɓn���p�����[�^
  gcwk->para_child.gsys = gsys;
  gcwk->para_child.clear_mode = mode;

  //�ړ��|�P���������`�F�b�N
  MP_RecoverMovePoke( gamedata );

  // �a������f���p�����[�^��ݒ�
  gcwk->dendouDemoParam.party    = GAMEDATA_GetMyPokemon( gamedata );
  gcwk->dendouDemoParam.mystatus = GAMEDATA_GetMyStatus( gamedata );
  gcwk->dendouDemoParam.ptime    = GAMEDATA_GetPlayTimeWork( gamedata );

  return event;
} 



//==============================================================================================
//==============================================================================================

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
