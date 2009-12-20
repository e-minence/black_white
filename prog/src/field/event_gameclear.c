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

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"

#include "event_gameclear.h"  //GAMECLEAR_MODE

#include "system/main.h"  //HEAPID_�`

#include "script.h"       //SCRIPT_CallScript
#include "event_mapchange.h"

#include "demo/demo3d.h"  //Demo3DProcData etc.
#include "app/local_tvt_sys.h"  //LocalTvt_ProcData etc.

#include "sound/pm_sndsys.h"  //PMSND_

#include "move_pokemon.h"

#include "proc_gameclear_save.h"

//==============================================================================================
//==============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  GAMECLEAR_MODE clear_mode;
  BOOL saveSuccessFlag;
  const MYSTATUS * mystatus;
  void * pWork;
}GAMECLEAR_WORK;


//============================================================================================
//============================================================================================
// ���C���V�[�P���X
enum {
	GMCLEAR_SEQ_INIT = 0,			// ������
	GMCLEAR_SEQ_DENDOU_DEMO,		// �a������f��
	GMCLEAR_SEQ_SAVE_START,			// �Z�[�u�J�n
	GMCLEAR_SEQ_SAVE_MESSAGE,		// �Z�[�u�����b�Z�[�W�\��
	GMCLEAR_SEQ_SAVE_MAIN,			// �Z�[�u���C��
	GMCLEAR_SEQ_SAVE_END,			// �Z�[�u�I��
	GMCLEAR_SEQ_WAIT1,				// �E�F�C�g�P
	GMCLEAR_SEQ_FADE_OUT,			// �t�F�[�h�A�E�g�Z�b�g
	GMCLEAR_SEQ_BGM_FADE_OUT,		// BGM�t�F�[�h�A�E�g�Z�b�g
	GMCLEAR_SEQ_WAIT2,				// �E�F�C�g�Q
	GMCLEAR_SEQ_ENDING_DEMO,		// �G���f�B���O�f��
	GMCLEAR_SEQ_END,				// �I��
};



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
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gcwk->gsys );

	switch (*seq) {
	case GMCLEAR_SEQ_INIT:
    PMSND_FadeOutBGM( 30 );
    { //�t�B�[���h�}�b�v���I��������
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gcwk->gsys );
      GMEVENT * new_event = DEBUG_EVENT_GameEnd( gcwk->gsys, fieldmap );
      GMEVENT_CallEvent( event, new_event );
    }
		(*seq) ++;
		break;

	case GMCLEAR_SEQ_DENDOU_DEMO:
    if ( PMSND_CheckFadeOnBGM() == FALSE ) break;
    GFL_FADE_SetMasterBrightReq( 
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 
        0, 16, -16 );
    SCRIPT_CallGameClearScript( gcwk->gsys, HEAPID_PROC );
#if 0
    { //�a������̑����N�̏�f��
      DEMO3D_PARAM * param = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(DEMO3D_PARAM) );
      param->demo_id = DEMO3D_ID_N_CASTLE;
      GMEVENT_CallProc( event, FS_OVERLAY_ID(demo3d), &Demo3DProcData, param );
      gcwk->pWork = param;
    }
#endif

		(*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_START:

    //�u�㏑�����܂��v�Ƃ��\������\��

		//
    //GAMEDATA_SaveAsyncStart( gamedata );
    (*seq) ++;
		break;

  case GMCLEAR_SEQ_SAVE_MESSAGE:
    GMEVENT_CallProc( event, FS_OVERLAY_ID(gameclear_demo), &GameClearMsgProcData, gcwk->gsys );
    (*seq) ++;
		break;

	case GMCLEAR_SEQ_SAVE_MAIN:
    return GMEVENT_RES_FINISH;
		break;

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
  gcwk->clear_mode = mode;
  gcwk->mystatus = GAMEDATA_GetMyStatus( gamedata );

  //�ړ��|�P���������`�F�b�N
  MP_RecoverMovePoke( gamedata );

  return event;
}


